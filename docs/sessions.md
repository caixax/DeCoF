# Session log

## 2026-10-10: Phase 0 (setup), stopped on blockers

### Done

1. `git init`, `.gitignore` (versions/, versions.zip, refs/, build/, dist/,
   binaries, PDBs, assets, RE databases). Repo-local `core.autocrlf=false`.
2. Leftover search over all of `versions/` (*.pdb, *.map, *.cpp, *.c, *.h,
   *.dsp, *.vcproj, *.sln, *.def, *.lib, *.obj and other build debris):
   - `versions/1.0/CryOfFear/cl_dlls/hl.pdb` (matches 1.0 hl.dll)
   - `versions/1.55/cl_dlls/hl.pdb` (matches 1.55 hl.dll)
   - `versions/hl.pdb` + `versions/hl.dll`, loose in the versions root,
     dated 2012-08-02, the 1.4 hl.dll with its PDB (MD5 of hl.dll matches
     the Ghidra import log `versions/application.log`, project
     "ghidra_11_2_cof_14"). `versions/1.4_pdb/` does not exist; this file
     pair plays its role.
   - Nothing else: no source, map, project, def or lib files anywhere.
   - Prototype devkit holds only ZHLT compile tools (hlbsp/hlcsg/hlrad/hlvis
     2005), `readme.chm` and `Spirit1.7.fgd` (identical to the fgd shipped
     with Spirit of Half-Life 1.7).
   - The prototype gamedll is `cl_dlls/server.dll` (not hl.dll), plus an
     older `Copy of server.dll` (2009-09-04).
3. Inventory: `docs/versions.csv` via `tools/pe_inventory.py`.
4. Toolchains: `docs/toolchains.md`. Prototype to 1.55 used VC++ .NET 2003
   RTM (13.10.3077), hl.dll as a Debug /ZI build. **1.6a client.dll and
   hl.dll were rebuilt in Oct 2020 with VS 2019 16.7 (cl 19.27.29112,
   link 14.27.29112), Release, static CRT, no LTCG.** opengl32.dll is
   Paranoia's wrapper, VC6 RTM Standard Edition, unchanged since 2006.
5. Target version string: `1.6a` (client.dll .rdata, used next to the main
   menu logo code; reference at file offset 0x2D3A1). liblist.gam says
   `version "1.6"`. Steam app 223710, buildid 5821301, depot 223711
   manifest 3465817078078599269. The `1.6.0` string in client.dll is the
   irrKlang SDK version check, not the game version.
6. RE import: **blocked, no Ghidra or IDA MCP connected.** PDB symbols were
   exported without the RE tool by `tools/pdb_symbols.py`:
   `docs/symbols_1.0.csv`, `docs/symbols_1.4.csv`, `docs/symbols_1.55.csv`
   (va, rva, section, kind, size, object file, name, undecorated,
   decorated). About 6,400 named game functions in 1.55.
7. Refs cloned (all except Spirit) and summarized in `docs/refs.md`.
   Spirit of Half-Life research done (see below), not cloned.
8. Build setup: **blocked** (target compiler not installed, no RE tool to
   delink retail objects for objdiff, objdiff-cli not installed).
9. `tools/make_dist.py` written and dry-run tested (copies versions/1.6a to
   dist/, replaces only DLLs that the build emitted to build/bin/).

### Early Phase 1 finding (recorded in docs/structs.md)

The entvars_t extra field is new in 1.6a. The 1.55 `gEntvarsDescription`
table has stock HLSDK offsets (`sequence` at 0x128); the 1.6a table has
every field from `sequence` on shifted by +4 (size 0x2A8). The PDB types
therefore describe the old layout.

### Tools written

- `tools/pdbfile.py`: minimal MSF 7.00 PDB reader (info, DBI modules,
  section contributions, S_COMPILE2/3, S_ENVBLOCK, procedures, publics).
- `tools/pe_inventory.py`: writes docs/versions.csv.
- `tools/pdb_symbols.py`: PDB to CSV symbol export (undecorates through
  dbghelp.dll).
- `tools/make_dist.py`.

### Spirit of Half-Life version (awaiting confirmation)

Recommendation: Spirit 1.7. Evidence:
- `devkit/Spirit1.7.fgd` in the 2010 prototype is identical to the fgd
  inside the Spirit 1.7 release.
- Entity classnames (hl.dll exports vs `LINK_ENTITY_TO_CLASS` in each
  Spirit release 1.0 to 1.9): CoF has every Spirit entity of 1.6/1.7
  except `calc_angletransform`, `env_mirror`, `monster_bullsquid`,
  `scripted_tanksequence`; none of the 1.5-only items removed in 1.6
  (`item_antirad`, `item_camera`, `item_flare`, `item_medicalkit`,
  `player_marker`, `weapon_debug`, `weapon_generic`); none of the 1.8
  additions (`calc_numfromnum`, `watcher_number`, ...) or 1.9 additions.
- CoF has Spirit's `ambient_fmodstream` and `trigger_mp3audio`, which
  Paranoia lacks, so Spirit code entered CoF directly, not only through
  Paranoia. The first object in the hl.dll PDB is `fmod_sounds.obj`.
- CoF ships `cg.dll`, `cgGL.dll` and an `aurora/` folder, matching the
  Spirit 1.5a4+ distribution.
- Caveat: in the only mirror found (sohl-modders), the 1.6 and 1.7 source
  trees are byte-identical; 1.7 differs only in data files. The original
  Spirit 1.7 archive, if you have it, would settle whether its code
  differs from 1.6.

### Next session starts here

Waiting for the user to:
1. Connect a Ghidra or IDA MCP server.
2. Install MSVC v142 14.27 (cl 19.27.29112) from the official VS 2019
   installer, or say how to proceed.
3. Confirm the Spirit version before it is cloned.
Then resume Phase 0 at step 6 (import into RE tool), then step 8.

## 2026-10-10 (continued): Phase 0 completed

The user authorized downloading and installing all tooling (official
sources only) and asked for documentation of everything found, calls
included.

### Done

- **Exact target toolchain.** cl 19.27.29112 / link 14.27.29112 extracted
  from the official VS 2019 16.7.7 package (Microsoft CDN, SHA-256 checked
  against the channel manifest) into `I:\Tools\msvc-14.27.29112`. Headers
  and libraries from VS 2019 Build Tools (MSVC 14.27.29110 side by side,
  installed through the official installer) and Windows SDK 10.0.19041.
  The SxS toolset's own cl is 19.27.29120 and is not used.
- **RE tool.** Ghidra 12.1.4, GhidraMCP 7.0.0-rc.2 (bethington/ghidra-mcp:
  209 tools, headless server on a persistent project, Ghidra 12.1.4
  support) and ghidra-delinker-extension 0.9.2. All 12 binaries imported
  into `re/CoF.gpr` (`tools/ghidra_import.py`), PDBs applied to the 1.0,
  1.4 and 1.55 hl.dll. `.mcp.json` registers the bridge; the server runs
  with `tools\ghidra_mcp_server.bat`. See `docs/tooling.md`.
- **Spirit 1.7** cloned to `refs/Spirit` at `d50205b`.
- **Build and objdiff (Phase 0 step 8) proven.** `tools/build.py` compiles
  units with the exact compiler; `DelinkUnits.java` delinks retail units
  into COFF objects with relocations; objdiff-cli 3.8.2 reports
  `hl/dlls/h_export` at 100% (GiveFnptrsToDll, DllMain). Committed as
  `hl: h_export.cpp (MATCHED, 100)`.
- **Flags deduced** for the 1.6a game DLLs: `/O2 /Oy- /GS /GR /EHsc /MT`,
  SSE2, no `/GL` (`docs/toolchains.md`).
- **Interface docs.** `tools/scan_calls.py` and `docs/interfaces.md`:
  engine table usage, user messages, cvars, commands, client-server command
  strings, imports and exports for 1.6a and 1.55, plus the 1.55 to 1.6a
  diff. Raw data in `docs/calls/`.
- **Struct facts** added to `docs/structs.md`: engine table sizes
  (`enginefuncs_t` 144 members, pre-Steam; `cl_enginefunc_t` 101) and the
  two Steam-era `triangleapi_t` members the 1.6a client calls.
- `src/` seeded with the Paranoia 1.2 headers (unverified baseline) plus
  `src/dlls/h_export.cpp`.

### Not done

- opengl32.dll needs VC6 Standard Edition (cl 12.00.8168). Microsoft never
  offered it as a free download; only the Visual Studio Subscriptions
  archive has it. Not downloaded from unofficial mirrors (AGENTS.md).

### Status

| DLL | Functions known | MATCHED | EQUIVALENT |
|---|---|---|---|
| hl.dll 1.6a | 2 mapped so far | 2 | 0 |
| client.dll 1.6a | 0 | 0 | 0 |
| opengl32.dll | 0 | 0 | 0 |

Percentages against the full function count come with Phase 1, when
`docs/progress.csv` lists every function.

### Next session starts here

1. Start the MCP server: `tools\ghidra_mcp_server.bat --project
   I:\Projects\cof-decompiled\re\CoF.gpr --program /1.6a/hl.dll`, then
   approve the `ghidra` MCP server in Claude Code.
2. Phase 1 (after the user confirms): carry names from the 1.55 PDB build
   to 1.6a by function matching, fill `docs/progress.csv` for every 1.6a
   function, `tools/symbols/hl_1.6a.csv`, TU list and link order.

## 2026-10-11: Phase 1 (map) and scope extension

### Done

- **Feature export** of every build from Ghidra (`ExportFeatures.java`).
- **hl.dll 1.55 -> 1.6a matching** (`match_versions.py`): anchors from
  exports, RTTI vtables (482 classes keep their 1.55 vtable length), unique
  strings, named globals; call graph propagation. 4,241 1.6a functions carry
  1.55 names. Checked against Ghidra's own RTTI names: 368 agree, the 6
  differences are folded identical functions or CRT.
- **Exact static CRT found**: the 1.6a DLLs link MSVC 14.27.29110 libcmt /
  libvcruntime / libcpmt (internal build 28920) and SDK 10.0.19041 libucrt
  (build 27412), byte for byte. Library code starts at 0x10121FE5 (hl.dll)
  and 0x100B42A4 (client.dll); class LIB.
- **Reference builds** of HLSDK c7240b9, Spirit 1.7 and Paranoia 1.2 with the
  exact compiler (`refbuild.py`, about 590 objects) and reference-guided
  naming (`refmatch.py`): byte matches checked on relocation targets, names
  propagated through relocations, plus a similarity phase for functions CoF
  modified. This is how client.dll got names (no PDB exists for any client).
- **docs/progress.csv**: every 1.6a function, class, source file, first
  version, change history across 1.0/1.4/1.55, evidence.
  - hl.dll: 4,960 game functions, 85.3% named (88.1% of bytes). HLSDK 1,677,
    SPIRIT 461, PARANOIA 96, COF 1,995, UNKNOWN 731; LIB 4,343.
  - client.dll: 2,146 game functions, 39.4% named (40.9% of bytes). HLSDK
    544, PARANOIA 261, SPIRIT 19, COF 21, UNKNOWN 1,301; LIB 4,992. The
    Paranoia renderer files are 70% named by bytes.
- **Translation units and link order** (`docs/units.md`, `units_*.csv`).
  1.6a: game_shared, pm_shared, then dlls/ (or cl_dll/) alphabetically.
- **Structs** (`docs/structs.md`): CBaseEntity 0xD8 -> 0xDC in 1.6a (new 4
  bytes after `ammo_groza` at 0xC0), so every entity class is 4 bytes larger;
  CBasePlayer +344 own bytes; 5 more classes with own new fields. Evidence in
  `docs/class_sizes.csv` (allocation sizes) and the save tables.
- All names applied in Ghidra (`ApplyNames.java`, demangled with signatures).
- One-command pipeline: `tools/map_names.py`.
- **Scope extended by the user**: hw.dll (engine), the Paranoia renderer as
  its own milestone, opengl32.dll, GameUI.dll, cof.exe, CoFLaunchApp.exe.
  AGENTS.md updated. hw.dll, GameUI.dll and cof.exe imported in Ghidra
  (10,281 / 6,140 / 396 functions). ReHLDS and Xash3D FWGS cloned as engine
  references.

### Decompiled so far

hl.dll 2 functions MATCHED (`h_export.cpp`); everything else TODO.

### Blockers

- Visual C++ 6.0 is required to match hw.dll, opengl32.dll, GameUI.dll and
  cof.exe; there is no free official download. Without it those binaries can
  only reach EQUIVALENT.
- hw.dll has no symbols in any shipped file. Best symbol source: the Linux
  build of the Half-Life engine (`hw.so`, not stripped) from the Half-Life
  Steam depot, downloadable with steamcmd if the user owns Half-Life.

### Next session starts here

1. Start the MCP server (`tools\ghidra_mcp_server.bat --project
   I:\Projects\cof-decompiled\re\CoF.gpr --program /1.6a/hl.dll`) and run
   `mcp__ghidra__connect_instance` with project "CoF".
2. Phase 1 for hw.dll (needs a symbol source, see blockers) and GameUI.dll.
3. Phase 2 on hl.dll: HLSDK functions first, starting with whole HLSDK
   translation units whose functions all matched reference bytes.

## 2026-10-11 (continued): engine mapping started

- User confirmed owning Half-Life; the Linux steam_legacy build was
  downloaded with steamcmd (cached login, the same steamcmd that installed
  CoF 1.6a). `hw.so` and `gameui.so` carry full symbols.
- `match_versions.py` generalized for cross-platform builds: names from the
  Ghidra feature export when no PDB export exists, globals mapped through the
  name pointer at the start of structs (cvar_t, command tables; field offsets
  and ELF relocations handled, Ghidra's ELF rebase detected), data
  propagation through matched pairs.
- Results: hw.dll 1,769 functions named (1,352 from Linux symbols), GameUI.dll
  403, cof.exe 4 (the Linux launcher is a different program). Rows added to
  `docs/progress.csv` (class ENGINE / LIB / UNKNOWN); C names applied in
  Ghidra.
- The user asked to fetch Visual C++ 6.0 from the internet. Declined: it has
  never been free and mirror copies are unlicensed; AGENTS.md also forbids
  unofficial compiler sources. VC6 binaries proceed as EQUIVALENT unless the
  user supplies a licensed copy.

Next: GCC/MSVC vtable alignment for GameUI.dll and the C++ parts of hw.dll
(Ghidra did not recover VC6 vtables yet), then Phase 2 on hl.dll.
