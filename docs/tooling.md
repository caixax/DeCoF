# Tooling

Everything below lives outside the repository on this machine. Paths are the defaults the scripts use.

| Tool | Location | Source |
|---|---|---|
| cl.exe / link.exe 19.27.29112 / 14.27.29112 | `I:\Tools\msvc-14.27.29112\bin\Hostx86\x86` | official VS 2019 16.7.7 package `Microsoft.VisualCpp.Tools.HostX86.TargetX86` 14.27.29112 (+ `.Resources` en-US), SHA-256 checked against Microsoft's channel manifest |
| MSVC 14.27 headers and libraries | `C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.27.29110` | VS 2019 Build Tools, component `Microsoft.VisualStudio.Component.VC.14.27.x86.x64` (its own cl is 19.27.29120, not used) |
| Windows 10 SDK 10.0.19041.0 | `C:\Program Files (x86)\Windows Kits\10` | VS 2019 Build Tools, component `Microsoft.VisualStudio.Component.Windows10SDK.19041` |
| Ghidra 12.1.4 | `I:\Tools\ghidra_12.1.4_PUBLIC` | NSA GitHub release, SHA-256 checked |
| GhidraMCP 7.0.0-rc.2 (plugin + headless server) | `...\Ghidra\Extensions\GhidraMCP` | github.com/bethington/ghidra-mcp |
| ghidra-delinker-extension 0.9.2 | `...\Ghidra\Extensions\ghidra-delinker-extension` | github.com/boricj/ghidra-delinker-extension (Apache-2.0) |
| MCP bridge (Python 3.13 venv) | `I:\Tools\ghidra-mcp-venv\Scripts\bridge-mcp-ghidra.exe` | ghidra-mcp release wheel 7.0.0 |
| objdiff-cli 3.8.2 | `I:\Tools\bin\objdiff-cli.exe` | github.com/encounter/objdiff |
| JDK 21 (Temurin) | system install | already present |
| Python 3.14 + pefile, capstone | system install | pip |

Downloads are kept in `I:\Tools\_downloads`.

Why a separate compiler directory: the retail Rich header says build 29112. The 14.27 toolset that the current VS 2019 installer offers is the final servicing build 29120. VS 16.7.6 and 16.7.7 (October 2020, the month the 1.6a DLLs were linked) ship 29112, so that package was extracted on its own and is used with the 14.27.29110 headers and libraries (same CRT version in both releases).

## Ghidra project

- `re/CoF.gpr` (git-ignored), created by `python tools/ghidra_import.py`.
- Folders `/1.6a`, `/1.55`, `/1.4`, `/1.0`, `/proto`; PDBs applied to the 1.0, 1.4 and 1.55 hl.dll.

## MCP server (for the RE tools in Claude Code)

1. Start the headless server (keeps running, holds the project lock):
   ```
   tools\ghidra_mcp_server.bat --project I:\Projects\cof-decompiled\re\CoF.gpr --program /1.6a/hl.dll
   ```
   It listens on 127.0.0.1:8089 only, with script execution enabled.
2. `.mcp.json` in the repo root registers the stdio bridge as the `ghidra` MCP server, with `GHIDRA_MCP_REQUIRE_PROGRAM_SELECTORS=1`: every tool call must name its program (for example `/1.6a/hl.dll` or `/1.55/hl.dll`), so a rename can not land in the wrong version.
3. Claude Code picks the server up at session start and asks once to approve it.

## Build and diff

```
python tools/build.py base   [unit ...]   compile src/ with cl 19.27.29112
python tools/build.py target [unit ...]   apply symbols, delink retail units
python tools/build.py objdiff             write objdiff.json
python tools/build.py report              per unit / per function match
python tools/build.py all    [unit ...]
```

- Units and flags: `tools/units.json`. Retail addresses and decorated names: `tools/symbols/<dll>_1.6a.csv` (`va,name,kind`).
- `target` writes a job file and runs `tools/ghidra_scripts/DelinkUnits.java` through the MCP server when it is up, or through headless Ghidra when it is not. The script adds the decorated names as labels (readable primary names are kept) and exports each unit as a COFF object with relocations, so objdiff compares relocation targets by symbol name.
- Outputs: `build/base/...obj` (ours), `build/target/...obj` (retail), `build/report.json`.

Flags in use for the 1.6a game DLLs (evidence in `docs/toolchains.md`): `/O2 /Oy- /GS /GR /EHsc /MT /fp:precise`, default `/arch:SSE2`, no `/GL`. Confirmed so far by one fully matching unit (`hl/dlls/h_export`).

## Other scripts

| Script | Purpose |
|---|---|
| `tools/pe_inventory.py` | `docs/versions.csv` |
| `tools/pdbfile.py`, `tools/pdb_symbols.py` | PDB reading, `docs/symbols_<version>.csv` |
| `tools/scan_calls.py` | engine table usage, messages, cvars, commands (`docs/calls/`) |
| `tools/hdrstruct.py` | member lists of function tables from headers |
| `tools/make_dist.py` | assemble `dist/` from retail 1.6a plus fully linked DLLs |
| `tools/progress.py` | `docs/progress.json`, `docs/progress.svg`, `docs/PROGRESS.md` from `docs/progress.csv` (never edit the outputs by hand) |

## Progress report hook

`tools/hooks/pre-commit` reruns `tools/progress.py` on the staged `docs/progress.csv` and stages the three outputs whenever the CSV is part of a commit. `.git/hooks` is not versioned; install the hook once per clone:

    cp tools/hooks/pre-commit .git/hooks/pre-commit

## Phase 1 naming pipeline

`python tools/map_names.py [--skip-features] [--skip-refbuild] [--apply]` runs everything below in order (about 2 minutes plus the reference builds). The MCP server must be running for the feature export and for `--apply`.

| Step | Script | Output |
|---|---|---|
| Ghidra feature export (functions, callees, strings, data refs, constants, vtables) | `tools/ghidra_scripts/ExportFeatures.java` | `build/features/<dll>_<version>.jsonl` |
| Interface scan | `tools/scan_calls.py` | `docs/calls/` |
| Static CRT byte matching against the exact 1.6a libraries | `tools/objmatch.py` | `build/match/<dll>_1.6a_crt.csv` |
| Reference builds of HLSDK (c7240b9), Spirit 1.7, Paranoia 1.2 with cl 19.27.29112 | `tools/refbuild.py` (+ `tools/refshim/refshim.h`) | `build/refobj/` |
| Index of every function defined in the reference trees | `tools/ref_index.py` | `build/ref_index.json` |
| 1.55 (PDB) to 1.6a name matching for hl.dll | `tools/match_versions.py` | `build/match/hl_1.55_to_1.6a.csv` |
| Reference-guided naming (bytes + relocation targets + similarity) | `tools/refmatch.py` | `build/match/<dll>_1.6a_refnames.csv` |
| Function list with class, source file, versions, status | `tools/make_progress.py` | `docs/progress.csv` |
| Translation units and link order | `tools/make_units.py` | `docs/units_<dll>.csv` |
| Entity class sizes 1.55 vs 1.6a | `tools/class_sizes.py` | `docs/class_sizes.csv` |
| Symbol maps, applied in Ghidra with demangling | `tools/ghidra_scripts/ApplyNames.java` | `tools/symbols/<dll>_1.6a.csv` |

Hand-curated names go in `tools/symbols/<dll>_manual.csv` (never overwritten by the pipeline). Evidence for every name is kept in the `evidence` column of `docs/progress.csv`.
