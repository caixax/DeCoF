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
