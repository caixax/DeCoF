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
| Visual C# 2010 compiler, csc.exe 4.0.30319.1 (C# 4) | `I:\Tools\csc40` | official .NET Framework 4.0 redistributable `dotNetFx40_Full_x86_x64.exe` (download.microsoft.com, Microsoft Authenticode signature valid), extracted, see "CoFLaunchApp.exe" below |
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
   tools\ghidra_mcp_server.bat --project I:\COFPP\cof-decompiled\re\CoF.gpr --program /1.6a/hl.dll
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
- A name labels one retail address only (the COFF exporter refuses duplicates): `merged_symbols` in build.py keeps one address per name (curated maps first, then the strongest progress.csv evidence) and DelinkUnits.java drops stale copies in Ghidra. Names local to their object may repeat, one address per unit: `$LN...` jump tables and the VC6 dynamic initializers `_$E1`, `_$E2`, ... (DelinkUnits takes the copy nearest the unit's code), and the file statics listed in `tools/symbols/<dll>_local.csv` (`va,name,unit,evidence`, unit without the dll: a static `CloneString` in BitmapImagePanel.cpp, the file-local `operator new`/`operator delete` of hw BaseUISurface_Font.cpp). Besides the listed copies such a name keeps one other address (the copy every other object uses, such as the CRT `operator new`). The job file then names the address, `include <name> <va>`: the listed copy for its unit, the remaining copy for every other unit.
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
| `tools/compare_pe.py` | section by section comparison of a linked DLL with retail (run by `build.py link`) |
| `tools/link_diff.py` | per-function comparison of a linked DLL with retail: code, extra functions, references, data contents, exports, imports, Rich header, layout shifts |
| `tools/progress.py` | `docs/progress.json`, `docs/progress.svg`, `docs/PROGRESS.md` from `docs/progress.csv` (never edit the outputs by hand) |

## Linking and comparing a DLL (Phase 3)

```
python tools/build.py link hl                      build/link/hl/hl.dll + .map, copied to build/bin/
python tools/link_diff.py hl [--all] [--csv out]   explain every difference with retail
python tools/make_dist.py --bin build/dist_hl      dist/ with only the DLLs in that folder rebuilt
```

- The link section of `tools/units.json` holds the flags, the link order (`order`, retail image order), `src/dlls/hl.def`, the libraries and `implibs` (steam_api.lib made from the retail DLL by `tools/implib.py`). The link compiles every unit again into `build/link_obj/` with `cflags_extra` (`/Zc:inline`); objdiff keeps using `build/base/`.
- `build/bin/` holds every DLL that ever linked (client.dll, opengl32.dll, CoFLaunchApp.exe too). To test one DLL alone, copy it into its own folder and pass that folder to `make_dist.py --bin`.
- link 14.27 never lets a base relocation straddle a 4 KB page: it moves the next COMDAT by one 16-byte step instead (retail and rebuilt both have 0 straddling relocations). A function whose size differs (the EQUIVALENT ones) therefore changes which functions get this padding further down, so the code layout toggles by 16 bytes until the next such case. It is not a flag difference.
- Library members are pulled in the order their symbols are first needed: retail GetCurrentPath (player.cpp) called the oldnames alias `getcwd`, ours calls `_getcwd`, which puts libucrt getcwd.obj (and after it widechartomultibyte.obj and the GetFullPathNameW import) earlier. GetCurrentPath now calls `getcwd` (approved by the user, 2026-10-24): the static CRT object order and the kernel32 import order are identical to retail, and objdiff still reports 100%.
- Functions are placed alphabetically by decorated name inside each object, data in definition order: an unproven `_guess` name that sorts elsewhere moves its function (UTIL_PlayWeaponSound_guess sat after UTIL_ParticleEffect; renamed PlayWeaponSound_guess, it sorts into its retail slot between PRECACHE_SOUND and ReadEntVars), and the .data/.rdata order shows the original definition order of each file, which the function order cannot.
- Functions Ghidra never created (or merged into a neighbour) are found by link_diff as "found in retail by code but without a progress.csv row": shrink the neighbour with `tools/ghidra_scripts/SetBodyRange.java`, create the function with `FillGaps.java` (both through the GhidraMCP `/run_ghidra_script`, then `/save_program`), add the name to `tools/symbols/hl_manual.csv` and the unit list, re-delink and check.
- The Rich header shows 4 more C++ objects in retail (159 against 155, objects that add no code) and the Steamworks SDK steam_api.lib (VS 2005 import library, prod 123 build 50727) where ours is made by lib 14.27; the import hints differ for the same reason (kernel32.lib from an older SDK, the SDK's steam_api.lib).

### client.dll

```
python tools/build.py link client                  build/link/client/client.dll + .map (no error, no warning)
python tools/link_diff.py client --all             functions, exports, imports, layout
python tools/data_drift.py client                  where named globals drift (missing, resized or reordered data)
```

- link_diff compares an import slot by the imported function (the IAT name), not by the symbol map label: AnimateNextFrame_guess called timeGetTime where retail calls GetTickCount, and objdiff had passed it because the retail label was wrong.
- link_diff's data check covers each referenced object only up to the next referenced address and does not follow pointers, so a pointer table with other strings passes (view.cpp modelmap held the 15 HLSDK p_/v_ model pairs, retail only the flashlight). data_drift.py and a masked byte comparison of .data find such cases.
- Every vgui class is `__declspec(dllexport)` (VGUI.h), so each object that includes a vgui header emits that class's vtable and its implicit copy and move constructors and assignment operators as COMDATs (they are client.dll exports). The first object in link order keeps them, and inside an object the vtables follow class definition order. The include lists therefore decide where they land: VGUI_TextEntry.h in vgui_TeamFortressViewport.h after VGUI_Slider.h puts TextEntry into voice_vgui_tweakdlg.obj, vgui_ScorePanel.h after HUDControl_guess.h puts TablePanel, HeaderPanel and TextGrid into HUDControl_guess.obj after the CHUDControl vtable, as in retail.
- An inline header function emitted out of line is placed by its decorated name like any other function, so a byte-matched inline name at a place where it can not sort is another function: 10044020 is not cl_util.h's PlaySound but InventorySound_guess of the RECBInventory object.
- String literals inside an object's .rdata come in the order the source first uses them, so they show the original function definition order of each file, which ours does not always have (ammo.cpp, AmmoInventory_guess.cpp, ClothesMenu_guess.cpp and others). The RTTI type descriptors of cdll_int.obj are in another order too, and anonymous namespace names (`?A0x...`) are a hash of the original source path. None of this changes code.
- The retail static UCRT is older than the Windows 10 SDK 10.0.19041 one we link: retail _fptostr.obj has the four-argument should_round_up (19041 adds __acrt_has_trailing_digits and check_trailing), so the CRT code is about 300 bytes shorter around cfout.obj, and an extra 5-byte jmp follows the _Lockit functions (the same as hl.dll). Linking with the 10.0.18362 UCRT is a toolchain change that waits for the user.

### cof.exe (VC6 launcher)

```
python tools/build.py link launcher                build/link/launcher/launcher.exe + .map (VC6 link 6.00), copied to build/bin/
python tools/link_diff.py launcher
python tools/make_dist.py --keep --bin build/dist_hl --bin build/dist_launcher
```

- The EXE has no base relocations: link_diff reconstructs the places they would be (absolute operands found by a linear sweep of the code, aligned words that point into the image in the data sections) for both images, so the per-function and reference comparison works as for the DLLs.
- `"resources"` in a link section names a retail PE whose resources are linked in through `tools/res_from_pe.py` (a .res written into build/link/<dll>/ at link time, converted by the linker's cvtres), so the icons are never committed. The retail .rsrc of cof.exe was rewritten after the link by a resource editor (CoF's icon added as icon 5 and a language-neutral group icon 101 next to Valve's ones, 4-byte data alignment, code page 1252, directory version 4.0, SizeOfInitializedData recomputed from the raw section sizes); cvtres gives the same resources (types, names, languages, bytes) with 8-byte alignment, code page 0 and version 0.0, and the VC6 linker counts the .bss part of .data in SizeOfInitializedData.
- make_dist takes `--bin` several times (the last folder that has a binary wins); `launcher.exe` from a bin folder becomes `cof.exe` in dist/. Every binary under test keeps its own folder (build/dist_hl, build/dist_launcher, ...), and a test build names all of them: a binary missing from the folders given goes back to retail.

### GameUI.dll (VC6 menus)

```
python tools/build.py link gameui                  build/link/gameui/GameUI.dll + .map (VC6 link 6.00, no error, no warning)
python tools/link_diff.py gameui --all
python tools/make_dist.py --keep --bin build/dist_hl --bin build/dist_launcher --bin build/dist_coflaunchapp --bin build/dist_client --bin build/dist_gameui
```

- Link options from the retail PE: `/dll /machine:I386 /subsystem:windows /incremental:no /debug /pdb:...` (linker 6.00, base 0x10000000, 4 KB file alignment, an NB10 record after the last section). `/debug` makes /OPT:NOREF the default, which retail shows (no identical COMDAT folding; `/opt:ref` folds the ??_G deleting destructors and drops code). Libraries: the VC6 kernel32.lib, user32.lib, wsock32.lib; LIBCMT and OLDNAMES come from the objects. Retail imports nothing else (vgui2, tier0 and vstdlib are reached through CreateInterface factories), and the only export is CreateInterface (interface.cpp dllexport).
- `"staticlibs": {"vgui_controls.lib": "vgui2/controls/controls"}` makes build.py put that unit and every later unit of the link order into a static library (lib.exe of the toolchain), placed by `"@vgui_controls.lib"` in `"libs"` after the import libraries. Retail has the import thunks between OptionsSubVoice.obj and controls.obj; with the library VC6 link pulls the members in the retail order and every object starts at its retail address, the kernel32 import order and hints are identical and most of the static CRT falls into place.
- Objects that disagree on a decorated name only show up at the link (objdiff compares each object alone): `struct IBaseSystem` in IGameUI.h against `class IBaseSystem` of the HLTV headers (DemoPlayerDialog.obj needs both through g_pSystemWrapper and its BaseSystemModule vtable), `extern int RandomLong(int,int)` against Random.cpp's `int32` (`__int32`, `_H`), windows.h renaming Panel::PostMessage to PostMessageA in files that include it before the vgui headers, and `<wchar.h>` before `<ctype.h>` defining iswspace and iswprint inline (VC6 wchar.h has C++ inline versions, ctype.h has `#elif 0`), which clashes with LIBCMT _wctype.obj.
- link_diff found four call differences that objdiff had passed at 100% (the target objects were delinked before the callees had names): CCareerMapFrame::OnCommand "close" calls CTaskbar::OnOpenNewCareerNameDialog, the career boxes and frames add their buttons with Dar::PutElement, TextImage::RecalculateNewLinePositions tests iswprint (hw.dll too). A comparison of every string referenced by paired functions found L"\n\n" in ShowCareerMapDescription and "Resource/DemoEventsDialog.res".
- Not reproducible with our toolchain: the Rich header says the 131 GameUI objects were compiled by the Standard Edition (prod 22), ours by the Professional cl (prod 11). Inside most objects the COMDATs (inline and template functions, compiler-generated destructors, vcall thunks) come in another order than ours (our cl emits them in first-reference order; /Zi, /Z7 and /Zd change nothing), and with them the string literals, RTTI descriptors and __real constants in .data/.rdata. Retail also has the unreferenced `const int` SAVEGAME_MAPNAME_LEN, SAVEGAME_COMMENT_LEN, SAVEGAME_ELAPSED_LEN (32, 80, 32) in .rdata of LoadGameDialog.obj and SaveGameDialog.obj; our cl drops unreferenced internal constants unless they are extern or their address is taken.
- Path dependent bytes: the NB10 PDB path (`C:\Documents and Settings\XPMUser\Desktop\GoldSource\gameui\Release\GameUI.pdb`, 26 bytes longer than ours, the whole file size difference) and the three anonymous namespace RTTI names (GripPanel and CaptionGripPanel of Frame.cpp, ScrollBarButton of ScrollBar.cpp), which hold the full source path (`C:\Documents and Settings\XPMUser\Desktop\GoldSourcegui2\controls\...`) and a number; vc6_anon fixes the symbol names, not these strings (68 bytes of .data).

### CoFLaunchApp.exe (.NET settings launcher)

```
python tools/coflaunchapp/build.py [--retail-path]   build/coflaunchapp/CoFLaunchApp.exe, copied to build/bin/ and build/dist_coflaunchapp/
python tools/coflaunchapp/compare.py [--show NAME]   per method IL, types, manifest, resources, metadata blob, whole file
python tools/coflaunchapp/progress.py                rewrites the coflaunchapp rows of docs/progress.csv
python tools/make_dist.py --keep --bin build/dist_hl --bin build/dist_launcher --bin build/dist_coflaunchapp
```

- Compiler: Visual C# 2010, csc 4.0.30319.1 (C# 4). The csc.exe of the installed framework (`C:\Windows\Microsoft.NET\Framework\v4.0.30319`, 4.8.9232) is the C# 5 compiler and differs only in .maxstack: C# 2, 3 and 4 count the object a newobj pushes before popping its arguments, C# 5 gives the exact depth. `--csc C:\Windows\Microsoft.NET\Framework\v4.0.30319\csc.exe` shows the two methods that differ (InitializeComponent 7 against 6, Resources.get_ResourceManager 3 against 2).
- .NET 4.0 can not be installed over 4.8, so the compiler is extracted, not installed. `7z x dotNetFx40_Full_x86_x64.exe -ox`, then from `x\netfx_Core.mzz` (a CAB) take `csc.exe_x86`, `csc.exe.config_x86`, `cscompui.dll_x86_enu`, `msvcr_clr_dll_x86` and `default.win32manifest_x86` and name them as the MSI File table does: `I:\Tools\csc40\csc.exe`, `csc.exe.config`, `1033\cscui.dll` (csc 4.0 asks for cscui.dll), `msvcr100_clr0400.dll` and `default.win32manifest`.
- `msvcr100_clr0400.dll` must sit next to csc.exe: the copy the 4.8 runtime leaves in SysWOW64 is a 14.8 build and csc 4.0 fails to start with 0xC000007B. Without `default.win32manifest` next to csc.exe no manifest is embedded and the .rsrc is 524 bytes short (the 4.0 file has the same bytes as the 4.8 one).
- csc 4.0 runs on the installed 4.8 runtime (its config asks for v4.0.30319) and references the 4.8 assemblies of the framework folder. The metadata and PE writers belong to the runtime, which is why the PE header says linker 11.0, as retail does.
- Source order: csc 4.0 and 4.8 emit types, members, assembly references and assembly attributes in command line order; build.py's order gives the retail metadata (the VS 2010 template order does not).
- `--retail-path` maps a temporary `subst U:` to build/coflaunchapp/retail_drive and compiles in `U:\common\Cry of Fear\CryOfFear\StatsResetProgram\CoFLaunchApp\CoFLaunchApp\obj\x86\Release`, so the debug directory holds the retail PDB path and compare.py can check the whole file. It refuses to run when U: exists and always removes the mapping. Without it the PDB path is 49 bytes shorter and everything after the debug record moves.
- Result with `--retail-path`: 51 of 51 methods identical, metadata identical except the MVID, managed and Win32 resources identical, 2,683,904 bytes on both sides. The only differing bytes are per build fields: PE and debug timestamps (8), MVID (16), PDB GUID and age (16) and the drive letter of the PDB path (csc.exe writes `u:`, retail has `U:`).

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

Hand-curated names go in `tools/symbols/<dll>_manual.csv` (never overwritten by the pipeline); a name that several objects define as a file static gets one row per extra copy in `tools/symbols/<dll>_local.csv`. Evidence for every name is kept in the `evidence` column of `docs/progress.csv`.
