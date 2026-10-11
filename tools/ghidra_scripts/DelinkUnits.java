// Apply a symbol map and delink translation units into COFF objects.
//
// One argument: a job file with one directive per line:
//   symbols <csv>      apply a symbol map (va,name,kind) before exporting
//   unit <out.obj>     start a unit written to out.obj
//   include <name>     add the function or data labelled <name> to the unit
//
// Symbol map rules: MSVC C++ names ("?...") are added as secondary labels so
// readable primary names survive. C names ("_Foo", "_Foo@8") become the
// primary name without the underscore plus a secondary label with the full
// decorated name, which is what the delinker's MSVC symbol preference needs.
// Missing functions are created.
//
// Units are exported with the ghidra-delinker-extension: the "Relocation
// table synthesizer" analyzer runs on the unit's address set, then the
// "COFF relocatable object" exporter writes the file. Works under
// analyzeHeadless (-postScript DelinkUnits.java <job>) and through the
// GhidraMCP /run_ghidra_script endpoint.
//@category CoF

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.app.plugin.core.analysis.AutoAnalysisManager;
import ghidra.app.script.GhidraScript;
import ghidra.app.services.Analyzer;
import ghidra.app.util.Option;
import ghidra.app.util.exporter.Exporter;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.data.ArrayDataType;
import ghidra.program.model.data.Undefined;
import ghidra.program.model.data.Undefined1DataType;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolTable;
import ghidra.program.model.symbol.SymbolType;
import ghidra.util.classfinder.ClassSearcher;

public class DelinkUnits extends GhidraScript {
	private static final String EXPORTER = "COFF relocatable object";
	private static final String ANALYZER = "Relocation table synthesizer";

	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		if (args.length != 1) {
			throw new IllegalArgumentException("usage: DelinkUnits.java <job file>");
		}
		File out = null;
		AddressSet set = null;
		int units = 0;
		for (String raw : Files.readAllLines(Paths.get(args[0]))) {
			String line = raw.strip();
			if (line.isEmpty() || line.startsWith("#")) {
				continue;
			}
			int sp = line.indexOf(' ');
			String cmd = line.substring(0, sp);
			String arg = line.substring(sp + 1).strip();
			switch (cmd) {
				case "symbols":
					applySymbols(arg);
					break;
				case "unit":
					if (out != null) {
						exportSafe(out, set);
						units++;
					}
					out = new File(arg);
					set = new AddressSet();
					break;
				case "forget":
					forgetSymbols(arg);
					break;
				case "include":
					try {
						set.add(findSymbol(arg));
					}
					catch (IllegalArgumentException e) {
						printerr("DelinkUnits: skipping " + arg + " (" + e.getMessage() + ")");
					}
					break;
				default:
					throw new IllegalArgumentException("unknown directive: " + cmd);
			}
		}
		if (out != null) {
			exportSafe(out, set);
			units++;
		}
		println("DelinkUnits: exported " + units + " unit(s)");
	}

	// Remove names that the symbol map dropped at an address (CSV va,name):
	// labels are deleted, a function carrying the name gets its default name
	private void forgetSymbols(String csv) throws Exception {
		if (!new File(csv).exists()) {
			return;
		}
		SymbolTable symtab = currentProgram.getSymbolTable();
		int tx = currentProgram.startTransaction("Forget CoF names");
		int n = 0;
		try {
			for (String raw : Files.readAllLines(Paths.get(csv))) {
				String line = raw.strip();
				if (line.isEmpty() || line.startsWith("va,")) {
					continue;
				}
				String[] f = line.split(",", -1);
				Address addr = toAddr(Long.parseUnsignedLong(f[0], 16));
				String name = f[1];
				String plain = name.startsWith("_") ? name.substring(1) : name;
				List<Symbol> here = new ArrayList<>();
				for (Symbol s : symtab.getSymbols(addr)) {
					here.add(s);
				}
				for (Symbol s : here) {
					if (!s.getName().equals(name) && !s.getName().equals(plain)) {
						continue;
					}
					Function fn = getFunctionAt(addr);
					try {
						if (fn != null && fn.getSymbol().equals(s)) {
							fn.setName(null, SourceType.DEFAULT);
						}
						else {
							s.delete();
						}
						n++;
					}
					catch (Exception e) {
						printerr("could not forget " + name + " at " + addr);
					}
				}
			}
		}
		finally {
			currentProgram.endTransaction(tx, true);
		}
		println("DelinkUnits: forgot " + n + " names");
	}

	private void applySymbols(String csv) throws Exception {
		SymbolTable symtab = currentProgram.getSymbolTable();
		int tx = currentProgram.startTransaction("Apply CoF symbol map");
		int applied = 0;
		try {
			for (String raw : Files.readAllLines(Paths.get(csv))) {
				String line = raw.strip();
				if (line.isEmpty() || line.startsWith("#") || line.startsWith("va,")) {
					continue;
				}
				String[] f = line.split(",", -1);
				Address addr = toAddr(Long.parseUnsignedLong(f[0], 16));
				String name = f[1];
				String kind = f.length > 2 ? f[2] : "func";

				if (kind.equals("func") && getFunctionAt(addr) == null) {
					if (!new CreateFunctionCmd(addr).applyTo(currentProgram, monitor)) {
						printerr("could not create function at " + addr);
					}
				}
				// a name lives at one address: drop stale copies elsewhere (the
				// exporter refuses duplicate names), for the decorated name and
				// for the plain primary name of C symbols
				// ($LN jump table labels are local to their object: the same
				// name may label several addresses)
				String[] spellings = name.startsWith("$") ? new String[0]
						: name.startsWith("_") ? new String[] { name, name.substring(1) }
						: new String[] { name };
				for (String spelling : spellings) {
					List<Symbol> stale = new ArrayList<>();
					symtab.getSymbols(spelling).forEach(stale::add);
					for (Symbol other : stale) {
						if (other.getAddress().equals(addr) || other.getSource() == SourceType.DEFAULT) {
							continue;
						}
						Function ofn = getFunctionAt(other.getAddress());
						try {
							if (ofn != null && ofn.getSymbol().equals(other)) {
								ofn.setName(null, SourceType.DEFAULT);
							}
							else {
								other.delete();
							}
						}
						catch (Exception e) {
							printerr("could not clear " + spelling + " at " + other.getAddress());
						}
					}
				}
				try {
					Function fn = getFunctionAt(addr);
					if (name.startsWith("_")) {
						// the delinker looks for "_" + primary name
						makePrimary(symtab, fn, addr, name.substring(1));
					}
					else if (!name.startsWith("?")) {
						// "@name@n" (fastcall), "$LN" jump table labels and other
						// names are emitted as the primary name itself
						makePrimary(symtab, fn, addr, name);
					}
					if (!hasLabel(symtab, addr, name)) {
						symtab.createLabel(addr, name, SourceType.USER_DEFINED);
					}
				}
				catch (ghidra.util.exception.DuplicateNameException e) {
					// name already used by a folded copy or applied earlier
				}
				// the delinker prefers a "?" label: drop C++ names left at this
				// address by earlier, wrong naming passes
				Function cur = getFunctionAt(addr);
				List<Symbol> here = new ArrayList<>();
				for (Symbol s : symtab.getSymbols(addr)) {
					here.add(s);
				}
				for (Symbol s : here) {
					if (s.getName().startsWith("?") && !s.getName().equals(name) && !s.isDynamic()
							&& (cur == null || !cur.getSymbol().equals(s))) {
						s.delete();
					}
				}
				if (kind.equals("data") && f.length > 3 && !f[3].isEmpty()) {
					defineExtent(symtab, addr, Integer.parseInt(f[3]));
				}
				applied++;
			}
		}
		finally {
			currentProgram.endTransaction(tx, true);
		}
		println("DelinkUnits: " + applied + " symbols applied from " + csv);
	}

	// Define a named global as undefined1[size] so that references into its
	// middle are offsets from its symbol. The extent stops before the next
	// named symbol; ranges holding instructions or typed data (pointers,
	// strings: their references must survive) are left alone.
	private void defineExtent(SymbolTable symtab, Address addr, int size) {
		if (size <= 1) {
			return;
		}
		Listing listing = currentProgram.getListing();
		Address end = addr.add(size - 1);
		Data first = listing.getDataAt(addr);
		if (first != null && first.getLength() >= size) {
			return;
		}
		for (Symbol s : symtab.getSymbols(new AddressSet(addr.add(1), end), SymbolType.LABEL, true)) {
			if (!s.isDynamic() && s.getSource() != SourceType.DEFAULT) {
				end = s.getAddress().subtract(1);
				break;
			}
		}
		if (end.compareTo(addr) <= 0) {
			return;
		}
		AddressSet range = new AddressSet(addr, end);
		if (listing.getInstructions(range, true).hasNext()) {
			return;
		}
		for (Data d : listing.getDefinedData(range, true)) {
			boolean undefinedType = Undefined.isUndefined(d.getDataType()) || d.getDataType() instanceof ArrayDataType
					&& Undefined.isUndefined(((ArrayDataType) d.getDataType()).getDataType());
			if (!d.getMinAddress().equals(addr) && !undefinedType) {
				return;
			}
			if (d.getMaxAddress().compareTo(end) > 0) {
				return;
			}
		}
		try {
			clearListing(addr, end);
			int n = (int) end.subtract(addr) + 1;
			createData(addr, new ArrayDataType(Undefined1DataType.dataType, n, 1));
		}
		catch (Exception e) {
			printerr("could not define extent at " + addr + ": " + e.getMessage());
		}
	}

	private static void makePrimary(SymbolTable symtab, Function fn, Address addr, String primary)
			throws Exception {
		if (fn != null) {
			if (!fn.getName().equals(primary)) {
				// a secondary label with that name would make the rename fail
				for (Symbol s : symtab.getSymbols(addr)) {
					if (s.getName().equals(primary) && !s.equals(fn.getSymbol())) {
						s.delete();
						break;
					}
				}
				fn.setName(primary, SourceType.USER_DEFINED);
			}
			return;
		}
		for (Symbol s : symtab.getSymbols(addr)) {
			if (s.getName().equals(primary)) {
				if (!s.isPrimary()) {
					s.setPrimary();
				}
				return;
			}
		}
		symtab.createLabel(addr, primary, SourceType.USER_DEFINED).setPrimary();
	}

	private static boolean hasLabel(SymbolTable symtab, Address addr, String name) {
		for (Symbol s : symtab.getSymbols(addr)) {
			if (s.getName().equals(name)) {
				return true;
			}
		}
		return false;
	}

	private AddressSet findSymbol(String name) {
		// C names are stored without their leading underscore as primary,
		// and with it as a secondary label; either spelling resolves.
		AddressSet set = new AddressSet();
		SymbolTable symtab = currentProgram.getSymbolTable();
		List<Symbol> found = new ArrayList<>();
		symtab.getSymbols(name).forEach(found::add);
		if (found.isEmpty()) {
			throw new IllegalArgumentException("symbol not found: " + name);
		}
		for (Symbol s : found) {
			Function fn = getFunctionAt(s.getAddress());
			if (fn != null) {
				set.add(fn.getBody());
				addTrailingTables(set, fn);
			}
			else {
				CodeUnit cu = currentProgram.getListing().getCodeUnitAt(s.getAddress());
				set.add(cu.getMinAddress(), cu.getMaxAddress());
			}
		}
		return set;
	}

	// MSVC emits the jump tables of a switch (the dword target table and the
	// byte index table) right after the function, inside its COMDAT; Ghidra
	// keeps them out of the function body. Add the bytes between the body
	// and the end of the last data the function references there, up to the
	// next function, so the table is part of the function like in our object.
	private void addTrailingTables(AddressSet set, Function fn) {
		Address end = fn.getBody().getMaxAddress();
		Function next = getFunctionAfter(end);
		if (next == null) {
			return;
		}
		Address limit = next.getEntryPoint();
		Address last = null;
		Listing listing = currentProgram.getListing();
		for (Address from : fn.getBody().getAddresses(true)) {
			for (Reference r : currentProgram.getReferenceManager().getReferencesFrom(from)) {
				Address to = r.getToAddress();
				// only data reads (the table load of the switch jump), never
				// calls or jumps into code Ghidra left outside any function
				if (!r.getReferenceType().isData() || listing.getInstructionContaining(to) != null) {
					continue;
				}
				if (to.compareTo(end) > 0 && to.compareTo(limit) < 0) {
					CodeUnit cu = listing.getCodeUnitContaining(to);
					Address max = cu != null ? cu.getMaxAddress() : to;
					if (max.compareTo(limit) >= 0) {
						max = limit.subtract(1);
					}
					if (last == null || max.compareTo(last) > 0) {
						last = max;
					}
				}
			}
		}
		if (last == null) {
			return;
		}
		// the referenced item is only the start of a table (Ghidra defines
		// the entries one by one): take everything up to the next function
		// except the int3 fill the linker puts between functions
		Address stop = limit.subtract(1);
		try {
			while (stop.compareTo(last) > 0 && getByte(stop) == (byte) 0xCC) {
				stop = stop.subtract(1);
			}
		}
		catch (Exception e) {
			stop = last;
		}
		set.add(end.add(1), stop);
		// the byte index table of a two-level switch is read with
		// movzx r, byte ptr [r + table], an operand Ghidra often leaves
		// without a reference: add it, or the relocation synthesizer keeps
		// the raw absolute address
		int tx = currentProgram.startTransaction("Switch table references");
		try {
			for (Instruction ins : listing.getInstructions(fn.getBody(), true)) {
				for (int op = 0; op < ins.getNumOperands(); op++) {
					for (Object o : ins.getOpObjects(op)) {
						if (!(o instanceof Scalar)) {
							continue;
						}
						long v = ((Scalar) o).getUnsignedValue();
						Address to;
						try {
							to = toAddr(v);
						}
						catch (Exception e) {
							continue;
						}
						if (to.compareTo(end) <= 0 || to.compareTo(stop) > 0) {
							continue;
						}
						// the switch analysis may have referenced the table
						// minus the index bias (after dec eax): the synthesizer
						// only relocates a primary reference that equals the
						// encoded address
						Reference exact = null;
						for (Reference r : ins.getOperandReferences(op)) {
							if (r.getToAddress().equals(to)) {
								exact = r;
							}
						}
						if (exact == null) {
							exact = currentProgram.getReferenceManager().addMemoryReference(ins.getAddress(), to,
								RefType.DATA, SourceType.ANALYSIS, op);
						}
						if (!exact.isPrimary()) {
							currentProgram.getReferenceManager().setPrimary(exact, true);
						}
						// the biased reference would be exported as an offcut
						// label inside the table ("PTR_...+3"), which objdiff
						// cannot handle: drop it
						for (Reference r : ins.getOperandReferences(op)) {
							Address a = r.getToAddress();
							if (!a.equals(to) && a.compareTo(fn.getBody().getMaxAddress()) > 0
									&& a.compareTo(stop) <= 0) {
								currentProgram.getReferenceManager().delete(r);
							}
						}
					}
				}
			}
		}
		finally {
			currentProgram.endTransaction(tx, true);
		}
	}

	// a failing unit must not abort the script: that would roll back the
	// symbol changes of the whole run
	private void exportSafe(File file, AddressSet set) {
		try {
			export(file, set);
		}
		catch (Exception e) {
			printerr("DelinkUnits: " + e.getMessage());
		}
	}

	private void export(File file, AddressSet set) throws Exception {
		AutoAnalysisManager aam = AutoAnalysisManager.getAnalysisManager(currentProgram);
		Analyzer analyzer = aam.getAnalyzer(ANALYZER);
		if (analyzer == null) {
			throw new IllegalStateException("analyzer not found: " + ANALYZER);
		}
		aam.scheduleOneTimeAnalysis(analyzer, set);
		aam.waitForAnalysis(null, monitor);

		Exporter exporter = null;
		for (Exporter e : ClassSearcher.getInstances(Exporter.class)) {
			if (e.getName().equals(EXPORTER)) {
				exporter = e;
			}
		}
		if (exporter == null) {
			throw new IllegalStateException("exporter not found: " + EXPORTER);
		}
		List<Option> options = new ArrayList<>(exporter.getOptions(() -> currentProgram));
		exporter.setOptions(options);
		file.getParentFile().mkdirs();
		if (!exporter.export(file, currentProgram, set, monitor)) {
			throw new RuntimeException("failed to export " + file + ": " + exporter.getMessageLog());
		}
	}
}
