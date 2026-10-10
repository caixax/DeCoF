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
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolTable;
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
						export(out, set);
						units++;
					}
					out = new File(arg);
					set = new AddressSet();
					break;
				case "include":
					set.add(findSymbol(arg));
					break;
				default:
					throw new IllegalArgumentException("unknown directive: " + cmd);
			}
		}
		if (out != null) {
			export(out, set);
			units++;
		}
		println("DelinkUnits: exported " + units + " unit(s)");
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
				if (!name.startsWith("?")) {
					String plain = name.startsWith("_") ? name.substring(1) : name;
					Function fn = getFunctionAt(addr);
					if (fn != null) {
						fn.setName(plain, SourceType.USER_DEFINED);
					}
					else if (!hasLabel(symtab, addr, plain)) {
						symtab.createLabel(addr, plain, SourceType.USER_DEFINED).setPrimary();
					}
				}
				if (!hasLabel(symtab, addr, name)) {
					symtab.createLabel(addr, name, SourceType.USER_DEFINED);
				}
				applied++;
			}
		}
		finally {
			currentProgram.endTransaction(tx, true);
		}
		println("DelinkUnits: " + applied + " symbols applied from " + csv);
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
			}
			else {
				CodeUnit cu = currentProgram.getListing().getCodeUnitAt(s.getAddress());
				set.add(cu.getMinAddress(), cu.getMaxAddress());
			}
		}
		return set;
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
			throw new RuntimeException("failed to export " + file);
		}
	}
}
