# Cry of Fear decompilation

A matching decompilation of Cry of Fear (GoldSrc engine, 32-bit x86, MSVC). The goal is source code that, built with the original compilers, produces the same machine code as the retail binaries, function by function, and then a playable game assembled from the rebuilt binaries plus the retail assets.

The target is the Steam standalone release, version string `1.6a`.

![progress](docs/progress.svg)

Detailed numbers per binary and category are in [docs/PROGRESS.md](docs/PROGRESS.md), generated from [docs/progress.csv](docs/progress.csv) (one row per retail function with its status, match percentage and notes).

## Scope

| Binary | What it is | Compiler | Source |
|---|---|---|---|
| `cryoffear/cl_dlls/hl.dll` | Server game DLL | MSVC 19.27 (VS 2019 16.7) | `src/dlls`, `src/pm_shared`, `src/game_shared`, `src/common` |
| `cryoffear/cl_dlls/client.dll` | Client DLL with the Paranoia OpenGL renderer | MSVC 19.27 (VS 2019 16.7) | `src/cl_dll` (not started) |
| `hw.dll` (`sw.dll` is identical) | GoldSrc engine, CoF build of 2013-04-21 | Visual C++ 6.0 | `src/engine_hw` (not started) |
| `opengl32.dll` | Paranoia OpenGL wrapper | Visual C++ 6.0 | `src/opengl32` (complete, links identical to retail) |
| `cryoffear/cl_dlls/GameUI.dll` | Menus (VGUI2) | Visual C++ 6.0 | `src/gameui` (not started) |
| `cof.exe` | Engine launcher | Visual C++ 6.0 | `src/launcher` (not started) |
| `CoFLaunchApp.exe` | Settings launcher | .NET (C#) | `src/coflaunchapp` (not started) |

Third-party libraries shipped with the game (Steam, tier0, vstdlib, VGUI, FileSystem, Cg, DevIL, irrKlang, id3lib, Miles, voice codecs) are out of scope; the retail files are used as they are.

How each compiler was identified, and the flags, are documented in [docs/toolchains.md](docs/toolchains.md).

## Repository layout

| Path | Contents |
|---|---|
| `src/` | Reconstructed source, one directory per binary |
| `tools/` | Build, delink, diff and analysis scripts (`tools/units.json` lists every translation unit and its flags) |
| `tools/symbols/` | Names of retail functions and data, each with the evidence it comes from |
| `docs/` | Progress, struct layouts, toolchains, references, original bugs, session log |
| `versions/` | Retail game builds (not committed, supply your own) |
| `refs/` | Reference source repositories (not committed) |
| `build/`, `dist/` | Build output and the assembled game (not committed) |

## Requirements

- A legally owned copy of Cry of Fear. No game files are part of this repository. The retail 1.6a tree goes into `versions/1.6a`; older builds used for symbols (1.0, 1.55) go next to it, see [docs/versions.csv](docs/versions.csv).
- Python 3 with `pefile` and `capstone`.
- MSVC 14.27 (cl 19.27.29112) with the Windows 10 SDK 10.0.19041 for hl.dll and client.dll.
- Visual C++ 6.0 (cl 12.00.8168) for opengl32.dll, hw.dll, GameUI.dll and cof.exe. It has no free official download.
- Ghidra with ghidra-delinker-extension (to cut the retail binaries into objects) and objdiff-cli (to compare).

Tool locations are set at the top of `tools/msvc.py`, `tools/msvc6.py` and `tools/build.py`; [docs/tooling.md](docs/tooling.md) lists the versions in use and where they come from.

## Working on a unit

```
python tools/build.py check hl/dlls/cof_glock        # compile one unit and print the match of every function
python tools/objdiff_tool.py show hl/dlls/cof_glock "?PrimaryAttack@CGlock@@UAEXXZ"   # retail and ours side by side
python tools/disasm.py --version 1.55 hl "?PrimaryAttack@CGlock@@UAEXXZ"            # the same function in the 1.55 debug build, with source line numbers
python tools/update_status.py --report build/check_project/hl_dlls_cof_glock/report.json
```

`python tools/build.py target <units>` delinks the retail objects, `python tools/build.py report` produces the full report, and `python tools/make_dist.py` assembles `dist/` from the retail tree and every binary that links completely from source. The workflow for game DLL source files, with the patterns that matter for matching, is in [docs/howto_cof_source.md](docs/howto_cof_source.md).

The pre-commit hook in `tools/hooks/` regenerates the progress report whenever `docs/progress.csv` is committed. Install it with:

```
cp tools/hooks/pre-commit .git/hooks/pre-commit
```

## Conventions

- Function status is MATCHED (objdiff reports 100 percent), EQUIVALENT (compiles and the logic is verified against the assembly, with the remaining difference recorded) or TODO. Nothing else counts as done.
- Source follows the Half-Life SDK and Paranoia style it descends from. No decompiler artifacts (`FUN_`, `DAT_`, `iVar1`, raw offsets where a member exists).
- Names come from evidence: the PDBs of the older builds, strings, vtables, or reference source. Anything else carries the prefix `guess_`.
- Original behavior is kept, bugs included. Known bugs are listed in [docs/original_bugs.md](docs/original_bugs.md).
- Commits: one per function or small group, message `<dll>: <function or file> (<status>, <percent>)`, with `docs/progress.csv` committed together with the code it describes. Changes to retail names are committed on their own.

## References

The expected ancestry of the code is the [Half-Life SDK](https://github.com/ValveSoftware/halflife), [Paranoia](https://github.com/HLSources/Paranoia) and Spirit of Half-Life 1.7. Other community projects (ReHLDS, Xash3D FWGS, cry-of-fear-src, cof-enhanced, RebuiltSimon, MadSimonX, CryOfFear-FirstPerson) are consulted only, and everything they claim is checked in the binary. See [docs/refs.md](docs/refs.md) for what each one provides and its license.

## Legal

This project is not affiliated with or endorsed by Team Psykskallar or Valve. It contains no game assets or binaries. Code derived from the Half-Life SDK is subject to the Half-Life SDK license.
