# Toolchains per DLL and game version

Generated from `tools/pe_inventory.py` (Rich header, optional header,
debug directory) and `tools/pdbfile.py` (S_COMPILE2 records in the PDBs).
Raw data: `docs/versions.csv`.

## Summary

| Version | DLL | Compiler | Linker | Config | CRT |
|---|---|---|---|---|---|
| 2010 prototype | client.dll | cl 13.10.3077 (VC++ .NET 2003 RTM) | link 7.10.3077 | Release (`cl_dll\Release\client.pdb`) | static |
| 2010 prototype | server.dll | cl 13.10.3077 | link 7.10.3077 | unknown, no debug directory | static |
| 1.0 | client.dll | cl 13.10.3077 | link 7.10.3077 | Release (`cl_dll\Release\client.pdb`) | static |
| 1.0 | hl.dll | cl 13.10.3077 | link 7.10.3077 | Debug, /ZI (PDB present) | LIBCMTD (static debug) |
| 1.4 | hl.dll | cl 13.10.3077 | link 7.10.3077 | Debug, /ZI (PDB present) | LIBCMTD |
| 1.55 | client.dll | cl 13.10.3077 | link 7.10.3077 | Release | static |
| 1.55 | hl.dll | cl 13.10.3077 | link 7.10.3077 | Debug, /ZI (PDB present) | LIBCMTD |
| **1.6a** | **client.dll** | **cl 19.27.29112 (VS 2019 16.7, v142 14.27)** | **link 14.27.29112** | Release (`D:\CoFFinalPatch\code\cl_dll\Release\client.pdb`) | static |
| **1.6a** | **hl.dll** | **cl 19.27.29112 (VS 2019 16.7, v142 14.27)** | **link 14.27.29112** | Release (`D:\CoFFinalPatch\code\dlls\Releasehl\hl.pdb`) | static |
| all | opengl32.dll | cl 12.00.8168 (VC++ 6.0 RTM, Standard Edition) | link 6.00.8168 | Release | MSVCRT.dll (/MD) |

## Evidence

### 2010 to 1.55: Visual C++ .NET 2003

- Rich header of every client.dll, hl.dll and server.dll from the prototype
  to 1.55: `Utc1310_C/CPP.3077`, `Masm710.3077`, `Export710.3077`,
  `Linker710.3077`. Optional header linker version 7.10.
- The three hl.dll PDBs (1.0, 1.4, 1.55) agree: every game object carries
  S_COMPILE2 "Microsoft (R) Optimizing Compiler" 13.10.3077, the link
  record is "Microsoft (R) LINK" 7.10.3077, and the CRT comes from
  `c:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Vc7\lib\LIBCMTD.lib`
  and `libcpmtd.lib`.
- Original source root: `s:\CryofFear\Sourcecode\` (2012) and
  `s:\half-life\CryOfFear\sourcecode\code_src\` (2010 prototype client).

### hl.dll 1.0 to 1.55 is a Debug build

- Objects live in `s:\CryofFear\Sourcecode\dlls\debughl\`, the HLSDK
  Debug output directory of `hl.dsp`.
- S_COMPILE2 flags of the 159 game objects in 1.55: 156 C++ and 3 C objects
  with flag 0x100 (fEC, Edit and Continue, so /ZI). VC++ 7.1 rejects /ZI
  together with /O1, /O2 or /Ox, so this code is unoptimized (/Od).
  CRT objects carry 0x2000 (/GS) instead.
- Linked against LIBCMTD.lib (static debug CRT, /MTd).
- Exports `trip_beam` and `?ManagerReport@CMultiManager@@QAEXXZ`. Both
  exist only under `_DEBUG` in the HLSDK. They are gone in 1.6a.
- All three PDBs match their DLLs exactly (RSDS GUID and age).
  1.4 and 1.55 share PDB GUID D13BE348195D42DBBCCBF0E581F409B5 (ages 54
  and 90): the same incremental PDB, linked again 36 times between the two
  releases. 1.0 has its own GUID (age 784).

### 1.6a (Steam): Visual Studio 2019 16.7, Release

- PE timestamps 2020-10-28 (client.dll) and 2020-10-30 (hl.dll).
- Optional header linker version 14.27, subsystem version 6.00, new
  `.rsrc` section (side-by-side manifest).
- Rich header of hl.dll: `Utc1900_CPP.29112` x159, `Utc1900_C.29112` x3,
  `Linker1400.29112`, `Export1400.29112`, `Cvtres1400.29112`.
  client.dll: `Utc1900_CPP.29112` x140, `Utc1900_C.29112` x3, same tools.
  Build 29112 with linker 14.27 is the v142 toolset of VS 2019 16.7
  (cl.exe 19.27.29112, link.exe 14.27.29112).
- The other entries come from prebuilt static libraries, not from game code:
  `Utc1900_*.27412` and `.28920`, `Masm1400.*` (static CRT and UCRT),
  `Implib800.50727` (DevIL, irrKlang), `Linker600.8447` (Cg import libs),
  `Implib1000.30319` (steam_api.lib), `AliasObj1100.41118`.
- No `Utc1900_LTCG_*` entries, so no /GL (no whole program optimization).
- Imports contain no MSVCR/VCRUNTIME DLL, so the CRT is static (/MT).
- 1.55 hl.dll had 159 game objects. 1.6a hl.dll has 162. The new exports
  (`CBowlingBall`, `CMp5`, `weapon_mp5`, `ammo_mp5`, `bowling_ball`,
  `cof_mp5modesaverestore`) account for the growth.
- The 102 extra client.dll exports are implicit move constructors and move
  assignment operators of the exported VGUI classes (`$$QAV01@`), which a
  C++11 compiler generates. They are not new game code.

### opengl32.dll: Paranoia wrapper, Visual C++ 6.0 Standard

- Same file in the prototype, 1.0 and 1.6a
  (SHA-256 26732d7a9aeb8079..., PE timestamp 2006-11-27).
- Rich header: `Utc12_CPP_Std.8168` x2, `Linker600.8168`, `Masm613.7299`.
  "Std" is the Standard Edition, whose compiler has no optimizer (it
  accepts /O2 and ignores it).
- Contains `PARANOIA_HACKS_V1` and `opengl32.log`. Source is in
  `refs/Paranoia/code_src/opengl32/` (VC6 project, `/G6 /Gz /MD /W4 /GX /O2`).
- The prototype also has `parnaoia_opengl32.dll` (2008, VC++ .NET 2003), a
  later rebuild of the same wrapper that CoF never loads.

### Preliminary flag evidence for 1.6a (byte scans of .text, to be confirmed in the RE tool)

| Pattern | 1.6a hl.dll | 1.6a client.dll | 1.55 hl.dll (Debug) |
|---|---|---|---|
| `/GS` prologue `mov eax,[__security_cookie]; xor eax,ebp` | 365 | 547 | 0 |
| SSE `movss` loads | 11043 | 5233 | 0 |
| `this` spilled to `[ebp-x]` | 391 | 373 | 5522 |
| `/RTC` stack fill (`mov eax,0CCCCCCCCh; rep stosd`) | 0 | 0 | 0 |
| frame `push ebp; mov ebp,esp` | 6416 | 5401 | about one per function |
| aligned frames `and esp,-8/-16` | 246 | 52 | 5 |

Reading: optimized Release code (/O2-class), `/GS` on, SSE2 scalar math
(the VS 2019 default `/arch:SSE2`), no /RTC. Many functions keep an EBP
frame, so `/Oy-` is possible; this needs disassembly to settle. The 1.55
Debug build has no /RTC stack fill either (/ZI /Od without /RTC1).

## Change between versions

1. Prototype to 1.55: no toolchain change (VC++ .NET 2003 RTM throughout).
2. 1.55 to 1.6a: client.dll and hl.dll were rebuilt in October 2020 with
   VS 2019 16.7 (toolset 14.27). hl.dll also changed from Debug to Release.
   The codegen of the target has nothing in common with the PDB builds, so
   the PDBs give names, types, source files and link order, not byte
   patterns.
3. opengl32.dll never changed.

## Toolchains needed for matching

| Purpose | Toolchain | Status on this machine |
|---|---|---|
| 1.6a client.dll and hl.dll (the target) | MSVC v142 14.27, cl 19.27.29112 | not installed (only 14.44 from VS 2022 Build Tools) |
| opengl32.dll | Visual C++ 6.0 RTM Standard Edition, cl 12.00.8168 | not installed |
| Optional: solving functions in the 1.55 PDB build first | Visual C++ .NET 2003 RTM, cl 13.10.3077 | not installed |
