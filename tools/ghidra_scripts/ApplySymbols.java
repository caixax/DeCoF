// Apply a symbol map (CSV: va,name,kind) to the current program.
//
// kind is "func" or "data". For every row a label with the exact (mangled)
// name is added at va. MSVC C++ names ("?...") are added as secondary labels
// so readable primary names survive; the delinker's MSVC symbol preference
// picks the "?" label on export. C names ("_Foo" or "_Foo@8") get the primary
// name without the leading underscore, which the delinker turns back into
// the decorated name. Functions are created when missing.
//
// Usage (headless): -postScript ApplySymbols.java <csv>
//@category CoF

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolTable;

public class ApplySymbols extends GhidraScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		if (args.length != 1) {
			throw new IllegalArgumentException("usage: ApplySymbols.java <csv>");
		}
		List<String> lines = Files.readAllLines(Paths.get(args[0]));
		SymbolTable symtab = currentProgram.getSymbolTable();
		int applied = 0;
		for (String line : lines) {
			line = line.strip();
			if (line.isEmpty() || line.startsWith("#") || line.startsWith("va,")) {
				continue;
			}
			String[] f = line.split(",", -1);
			Address addr = toAddr(Long.parseUnsignedLong(f[0], 16));
			String name = f[1];
			String kind = f.length > 2 ? f[2] : "func";

			if (kind.equals("func") && getFunctionAt(addr) == null) {
				CreateFunctionCmd cmd = new CreateFunctionCmd(addr);
				if (!cmd.applyTo(currentProgram, monitor)) {
					printerr("could not create function at " + addr);
				}
			}

			boolean exists = false;
			for (Symbol s : symtab.getSymbols(addr)) {
				if (s.getName().equals(name)) {
					exists = true;
				}
			}
			if (name.startsWith("?")) {
				if (!exists) {
					symtab.createLabel(addr, name, SourceType.USER_DEFINED);
				}
			}
			else {
				// The delinker looks for a label equal to "_" + primary name,
				// so keep the plain name as primary and add the decorated one.
				String plain = name.startsWith("_") ? name.substring(1) : name;
				Function fn = getFunctionAt(addr);
				if (fn != null) {
					fn.setName(plain, SourceType.USER_DEFINED);
				}
				else {
					Symbol s = symtab.createLabel(addr, plain, SourceType.USER_DEFINED);
					s.setPrimary();
				}
				if (!exists && !plain.equals(name)) {
					symtab.createLabel(addr, name, SourceType.USER_DEFINED);
				}
			}
			applied++;
		}
		println("ApplySymbols: " + applied + " symbols applied");
	}
}
