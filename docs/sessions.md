# Session log

## 2026-10-10: Phase 0 (setup), stopped on blockers

### Done

1. `git init`, `.gitignore` (versions/, versions.zip, refs/, build/, dist/, binaries, PDBs, assets, RE databases). Repo-local `core.autocrlf=false`.
2. Leftover search over all of `versions/` (*.pdb, *.map, *.cpp, *.c, *.h, *.dsp, *.vcproj, *.sln, *.def, *.lib, *.obj and other build debris):
   - `versions/1.0/CryOfFear/cl_dlls/hl.pdb` (matches 1.0 hl.dll)
   - `versions/1.55/cl_dlls/hl.pdb` (matches 1.55 hl.dll)
   - `versions/hl.pdb` + `versions/hl.dll`, loose in the versions root, dated 2012-08-02, the 1.4 hl.dll with its PDB (MD5 of hl.dll matches the Ghidra import log `versions/application.log`, project "ghidra_11_2_cof_14"). `versions/1.4_pdb/` does not exist; this file pair plays its role.
   - Nothing else: no source, map, project, def or lib files anywhere.
   - Prototype devkit holds only ZHLT compile tools (hlbsp/hlcsg/hlrad/hlvis 2005), `readme.chm` and `Spirit1.7.fgd` (identical to the fgd shipped with Spirit of Half-Life 1.7).
   - The prototype gamedll is `cl_dlls/server.dll` (not hl.dll), plus an older `Copy of server.dll` (2009-09-04).
3. Inventory: `docs/versions.csv` via `tools/pe_inventory.py`.
4. Toolchains: `docs/toolchains.md`. Prototype to 1.55 used VC++ .NET 2003 RTM (13.10.3077), hl.dll as a Debug /ZI build. **1.6a client.dll and hl.dll were rebuilt in Oct 2020 with VS 2019 16.7 (cl 19.27.29112, link 14.27.29112), Release, static CRT, no LTCG.** opengl32.dll is Paranoia's wrapper, VC6 RTM Standard Edition, unchanged since 2006.
5. Target version string: `1.6a` (client.dll .rdata, used next to the main menu logo code; reference at file offset 0x2D3A1). liblist.gam says `version "1.6"`. Steam app 223710, buildid 5821301, depot 223711 manifest 3465817078078599269. The `1.6.0` string in client.dll is the irrKlang SDK version check, not the game version.
6. RE import: **blocked, no Ghidra or IDA MCP connected.** PDB symbols were exported without the RE tool by `tools/pdb_symbols.py`: `docs/symbols_1.0.csv`, `docs/symbols_1.4.csv`, `docs/symbols_1.55.csv` (va, rva, section, kind, size, object file, name, undecorated, decorated). About 6,400 named game functions in 1.55.
7. Refs cloned (all except Spirit) and summarized in `docs/refs.md`. Spirit of Half-Life research done (see below), not cloned.
8. Build setup: **blocked** (target compiler not installed, no RE tool to delink retail objects for objdiff, objdiff-cli not installed).
9. `tools/make_dist.py` written and dry-run tested (copies versions/1.6a to dist/, replaces only DLLs that the build emitted to build/bin/).

### Early Phase 1 finding (recorded in docs/structs.md)

The entvars_t extra field is new in 1.6a. The 1.55 `gEntvarsDescription` table has stock HLSDK offsets (`sequence` at 0x128); the 1.6a table has every field from `sequence` on shifted by +4 (size 0x2A8). The PDB types therefore describe the old layout.

### Tools written

- `tools/pdbfile.py`: minimal MSF 7.00 PDB reader (info, DBI modules, section contributions, S_COMPILE2/3, S_ENVBLOCK, procedures, publics).
- `tools/pe_inventory.py`: writes docs/versions.csv.
- `tools/pdb_symbols.py`: PDB to CSV symbol export (undecorates through dbghelp.dll).
- `tools/make_dist.py`.

### Spirit of Half-Life version (awaiting confirmation)

Recommendation: Spirit 1.7. Evidence:
- `devkit/Spirit1.7.fgd` in the 2010 prototype is identical to the fgd inside the Spirit 1.7 release.
- Entity classnames (hl.dll exports vs `LINK_ENTITY_TO_CLASS` in each Spirit release 1.0 to 1.9): CoF has every Spirit entity of 1.6/1.7 except `calc_angletransform`, `env_mirror`, `monster_bullsquid`, `scripted_tanksequence`; none of the 1.5-only items removed in 1.6 (`item_antirad`, `item_camera`, `item_flare`, `item_medicalkit`, `player_marker`, `weapon_debug`, `weapon_generic`); none of the 1.8 additions (`calc_numfromnum`, `watcher_number`, ...) or 1.9 additions.
- CoF has Spirit's `ambient_fmodstream` and `trigger_mp3audio`, which Paranoia lacks, so Spirit code entered CoF directly, not only through Paranoia. The first object in the hl.dll PDB is `fmod_sounds.obj`.
- CoF ships `cg.dll`, `cgGL.dll` and an `aurora/` folder, matching the Spirit 1.5a4+ distribution.
- Caveat: in the only mirror found (sohl-modders), the 1.6 and 1.7 source trees are byte-identical; 1.7 differs only in data files. The original Spirit 1.7 archive, if you have it, would settle whether its code differs from 1.6.

### Next session starts here

Waiting for the user to:
1. Connect a Ghidra or IDA MCP server.
2. Install MSVC v142 14.27 (cl 19.27.29112) from the official VS 2019 installer, or say how to proceed.
3. Confirm the Spirit version before it is cloned. Then resume Phase 0 at step 6 (import into RE tool), then step 8.

## 2026-10-10 (continued): Phase 0 completed

The user authorized downloading and installing all tooling (official sources only) and asked for documentation of everything found, calls included.

### Done

- **Exact target toolchain.** cl 19.27.29112 / link 14.27.29112 extracted from the official VS 2019 16.7.7 package (Microsoft CDN, SHA-256 checked against the channel manifest) into `I:\Tools\msvc-14.27.29112`. Headers and libraries from VS 2019 Build Tools (MSVC 14.27.29110 side by side, installed through the official installer) and Windows SDK 10.0.19041. The SxS toolset's own cl is 19.27.29120 and is not used.
- **RE tool.** Ghidra 12.1.4, GhidraMCP 7.0.0-rc.2 (bethington/ghidra-mcp: 209 tools, headless server on a persistent project, Ghidra 12.1.4 support) and ghidra-delinker-extension 0.9.2. All 12 binaries imported into `re/CoF.gpr` (`tools/ghidra_import.py`), PDBs applied to the 1.0, 1.4 and 1.55 hl.dll. `.mcp.json` registers the bridge; the server runs with `tools\ghidra_mcp_server.bat`. See `docs/tooling.md`.
- **Spirit 1.7** cloned to `refs/Spirit` at `d50205b`.
- **Build and objdiff (Phase 0 step 8) proven.** `tools/build.py` compiles units with the exact compiler; `DelinkUnits.java` delinks retail units into COFF objects with relocations; objdiff-cli 3.8.2 reports `hl/dlls/h_export` at 100% (GiveFnptrsToDll, DllMain). Committed as `hl: h_export.cpp (MATCHED, 100)`.
- **Flags deduced** for the 1.6a game DLLs: `/O2 /Oy- /GS /GR /EHsc /MT`, SSE2, no `/GL` (`docs/toolchains.md`).
- **Interface docs.** `tools/scan_calls.py` and `docs/interfaces.md`: engine table usage, user messages, cvars, commands, client-server command strings, imports and exports for 1.6a and 1.55, plus the 1.55 to 1.6a diff. Raw data in `docs/calls/`.
- **Struct facts** added to `docs/structs.md`: engine table sizes (`enginefuncs_t` 144 members, pre-Steam; `cl_enginefunc_t` 101) and the two Steam-era `triangleapi_t` members the 1.6a client calls.
- `src/` seeded with the Paranoia 1.2 headers (unverified baseline) plus `src/dlls/h_export.cpp`.

### Not done

- opengl32.dll needs VC6 Standard Edition (cl 12.00.8168). Microsoft never offered it as a free download; only the Visual Studio Subscriptions archive has it. Not downloaded from unofficial mirrors (project rule).

### Status

| DLL | Functions known | MATCHED | EQUIVALENT |
|---|---|---|---|
| hl.dll 1.6a | 2 mapped so far | 2 | 0 |
| client.dll 1.6a | 0 | 0 | 0 |
| opengl32.dll | 0 | 0 | 0 |

Percentages against the full function count come with Phase 1, when `docs/progress.csv` lists every function.

### Next session starts here

1. Start the MCP server: `tools\ghidra_mcp_server.bat --project I:\Projects\cof-decompiled\re\CoF.gpr --program /1.6a/hl.dll`, then approve the `ghidra` MCP server in Claude Code.
2. Phase 1 (after the user confirms): carry names from the 1.55 PDB build to 1.6a by function matching, fill `docs/progress.csv` for every 1.6a function, `tools/symbols/hl_1.6a.csv`, TU list and link order.

## 2026-10-11: Phase 1 (map) and scope extension

### Done

- **Feature export** of every build from Ghidra (`ExportFeatures.java`).
- **hl.dll 1.55 -> 1.6a matching** (`match_versions.py`): anchors from exports, RTTI vtables (482 classes keep their 1.55 vtable length), unique strings, named globals; call graph propagation. 4,241 1.6a functions carry 1.55 names. Checked against Ghidra's own RTTI names: 368 agree, the 6 differences are folded identical functions or CRT.
- **Exact static CRT found**: the 1.6a DLLs link MSVC 14.27.29110 libcmt / libvcruntime / libcpmt (internal build 28920) and SDK 10.0.19041 libucrt (build 27412), byte for byte. Library code starts at 0x10121FE5 (hl.dll) and 0x100B42A4 (client.dll); class LIB.
- **Reference builds** of HLSDK c7240b9, Spirit 1.7 and Paranoia 1.2 with the exact compiler (`refbuild.py`, about 590 objects) and reference-guided naming (`refmatch.py`): byte matches checked on relocation targets, names propagated through relocations, plus a similarity phase for functions CoF modified. This is how client.dll got names (no PDB exists for any client).
- **docs/progress.csv**: every 1.6a function, class, source file, first version, change history across 1.0/1.4/1.55, evidence.
  - hl.dll: 4,960 game functions, 85.3% named (88.1% of bytes). HLSDK 1,677, SPIRIT 461, PARANOIA 96, COF 1,995, UNKNOWN 731; LIB 4,343.
  - client.dll: 2,146 game functions, 39.4% named (40.9% of bytes). HLSDK 544, PARANOIA 261, SPIRIT 19, COF 21, UNKNOWN 1,301; LIB 4,992. The Paranoia renderer files are 70% named by bytes.
- **Translation units and link order** (`docs/units.md`, `units_*.csv`). 1.6a: game_shared, pm_shared, then dlls/ (or cl_dll/) alphabetically.
- **Structs** (`docs/structs.md`): CBaseEntity 0xD8 -> 0xDC in 1.6a (new 4 bytes after `ammo_groza` at 0xC0), so every entity class is 4 bytes larger; CBasePlayer +344 own bytes; 5 more classes with own new fields. Evidence in `docs/class_sizes.csv` (allocation sizes) and the save tables.
- All names applied in Ghidra (`ApplyNames.java`, demangled with signatures).
- One-command pipeline: `tools/map_names.py`.
- **Scope extended by the user**: hw.dll (engine), the Paranoia renderer as its own milestone, opengl32.dll, GameUI.dll, cof.exe, CoFLaunchApp.exe. Project rules updated. hw.dll, GameUI.dll and cof.exe imported in Ghidra (10,281 / 6,140 / 396 functions). ReHLDS and Xash3D FWGS cloned as engine references.

### Decompiled so far

hl.dll 2 functions MATCHED (`h_export.cpp`); everything else TODO.

### Blockers

- Visual C++ 6.0 is required to match hw.dll, opengl32.dll, GameUI.dll and cof.exe; there is no free official download. Without it those binaries can only reach EQUIVALENT.
- hw.dll has no symbols in any shipped file. Best symbol source: the Linux build of the Half-Life engine (`hw.so`, not stripped) from the Half-Life Steam depot, downloadable with steamcmd if the user owns Half-Life.

### Next session starts here

1. Start the MCP server (`tools\ghidra_mcp_server.bat --project I:\Projects\cof-decompiled\re\CoF.gpr --program /1.6a/hl.dll`) and run `mcp__ghidra__connect_instance` with project "CoF".
2. Phase 1 for hw.dll (needs a symbol source, see blockers) and GameUI.dll.
3. Phase 2 on hl.dll: HLSDK functions first, starting with whole HLSDK translation units whose functions all matched reference bytes.

## 2026-10-11 (continued): engine mapping started

- User confirmed owning Half-Life; the Linux steam_legacy build was downloaded with steamcmd (cached login, the same steamcmd that installed CoF 1.6a). `hw.so` and `gameui.so` carry full symbols.
- `match_versions.py` generalized for cross-platform builds: names from the Ghidra feature export when no PDB export exists, globals mapped through the name pointer at the start of structs (cvar_t, command tables; field offsets and ELF relocations handled, Ghidra's ELF rebase detected), data propagation through matched pairs.
- Results: hw.dll 1,769 functions named (1,352 from Linux symbols), GameUI.dll 403, cof.exe 4 (the Linux launcher is a different program). Rows added to `docs/progress.csv` (class ENGINE / LIB / UNKNOWN); C names applied in Ghidra.
- The user asked to fetch Visual C++ 6.0 from the internet. Declined: it has never been free and mirror copies are unlicensed; the project rules also forbid unofficial compiler sources. VC6 binaries proceed as EQUIVALENT unless the user supplies a licensed copy.

Next: GCC/MSVC vtable alignment for GameUI.dll and the C++ parts of hw.dll (Ghidra did not recover VC6 vtables yet), then Phase 2 on hl.dll.

## 2026-10-11 (continued): VC6 installed, opengl32.dll fully matched

- The user supplied a disc image of Visual C++ 6.0 Professional Edition (part X04-58170) as the copy covered by their family's license. Converted from the Alcohol 120% raw image (2448-byte sectors, Mode 1) to ISO, MD5 checked, `VC98` copied to `I:\Tools\vc6` without running the installer. cl 12.00.8168 / link 6.00.8168, the exact builds in the retail Rich headers. The image was moved out of the repo (`tools/_download/` is now git-ignored).
- **opengl32.dll is fully decompiled**: Paranoia's wrapper source (`src/opengl32/`, two Russian comments translated) compiled with VC6 at `/G6 /Gz /MD /GX /Od /Op` matches all 367 functions (objdiff 100%, 22,929 bytes). The full link (`python tools/build.py link opengl32`) is identical to retail in every section; only the timestamps and the Rich header's compiler-edition tag (Professional instead of Standard) differ.
- New tools: `tools/msvc6.py`, `tools/compare_pe.py`, `tools/update_status.py` (objdiff report to progress.csv), `build.py link <dll>`, per-DLL `toolchain` and `functions_file` in `tools/units.json`.

Status: opengl32.dll 367/367 MATCHED (100%); hl.dll 2 MATCHED; others TODO.

## 2026-10-11 (continued): Phase 2 on hl.dll, first CoF file

- Delinking works for all 152 hl units (duplicate names resolved by evidence rank; named globals get a defined extent so references into them are symbol+offset; CRT, jump table and stale label fixes).
- Layouts: `tools/pdbtypes.py` reads the full type stream of the 1.0/1.4/ 1.55 hl.pdb, `tools/pdb_lines.py` the file and line of every function (`docs/lines_hl_1.55.csv`; 1.55 is a debug build). `tools/hdrgen.py` rebuilds classes in PDB declaration order on top of the Paranoia text; `tools/layoutcheck.py` compares compiler layouts with the PDB. CoF headers done for CBaseEntity (+1.6a member), CBaseMonster, CBasePlayer (1.6a layout from the save table, size 0x281C), weapons, gamerules, skill and more. Engine struct changes found: entvars_t +4 after light_level, usercmd_t.is_donator, clientdata_t.isdonator, weapon_data_t m_flNextTertiaryAttack (`docs/structs.md`).
- Naming: entity exports are 5-byte tail calls into GetClassPtr<T> (`tools/fix_link_entities.py`); gmsg variables named through LinkUserMessages (`tools/name_usermsgs.py`); save tables merged from the retail Save functions (`tools/savetable_src.py`).
- Compiler flag: hl.dll uses `/O2 /Ob1` (user approved after the read-only experiment in `docs/toolchains.md`; client.dll and opengl32.dll unchanged, client.dll to be re-tested first once client units exist).
- First CoF source file: `src/dlls/CoF_Glock.cpp`, 27 MATCHED and 3 EQUIVALENT of 30. Workflow tools: `build.py check <unit>`, `build.py decomp <unit>` (Ghidra pseudo C into build/decomp), `tools/disasm.py`, `tools/member.py` (member and vtable offsets), `tools/mdlseq.py` (animation enums from the view model), `tools/try_variants.py`.
- The /Ob1 regression (CSquadMonster::ScheduleChange) was a naming error of the 1.55 name carry: the body is VacateSlot, ScheduleChange is the jmp thunk in the vtable. Other tail-call thunks may be misnamed the same way; scanning vtables for 5-byte jmp thunks is a good next step.
- Progress report: `tools/progress.py` (docs/PROGRESS.md, progress.svg, progress.json), regenerated by `tools/hooks/pre-commit`.

Status: hl.dll 1,510 MATCHED + 3 EQUIVALENT of 5,434 functions; opengl32.dll 367/367; client.dll, hw.dll, GameUI.dll, cof.exe 0. Nine Paranoia-based hl units (client, combat, gamerules, ggrenade, maprules, multiplay_gamerules, player, triggers, weapons) do not compile against the CoF headers yet and need CoF rewrites; 94 CoF units have no source yet.

Next: vtable scan for misnamed jmp thunks; then the other CoF weapons (same structure as the glock), then player.cpp.

## 2026-10-11 (continued): weapons, base units, naming and delinker fixes

- CoF weapons, all 100% except one function: glock, P345, VP70, TMP, revolver, M16, rifle, FAMAS, G43; shotgun 25 of 26 (ItemPostFrame EQUIVALENT, register allocation). Written by agents from `docs/howto_cof_source.md`, which now holds the matching patterns (read the 1.55 debug build first: `disasm.py --version 1.55` prints the PDB statement lines).
- Paranoia-based units ported to CoF and compiling: weapons.cpp 88/88, gamerules.cpp 7/7, multiplay_gamerules.cpp 58/59, ggrenade.cpp 14/14, maprules.cpp 55/55, triggers.cpp 363/363, combat.cpp 43/44 (Killed: calls to empty same-file functions that MSVC 19.27 drops), client.cpp 52/63 (ClientCommand 99.995%; several functions wait for class headers).
- Headers checked in the binary: skilldata_t G43 member, entvars_t 0x2AC / edict_t 0x32C, the 1.6a CBaseEntity field at 0xC4 (donator status), CWorld custom gloves at 0xF8, Bullet enum and SpawnBlood from the 1.55 PDB, iMaxAmmo1 out of line, CHalfLifeRules members, CGib::SpawnGib, inline weapon virtuals, cof_doctorboss.h.
- Retail names, each with evidence: signature check by ret size (`tools/sigcheck.py`), byte-proven names for 190 unnamed functions (`tools/bytematch.py`, `tools/symbols/hl_bytes.csv`), misnamed GetClassPtr/InitTrigger, private save tables, RTTI vtable names, SufficientlyOffTheGround. `tools/sync_names.py` carries map changes into progress.csv and the unit lists.
- Delinker: functions include their switch jump tables, alignment nops and no relocations on immediates that only look like addresses (base relocation check); the offcut table labels that made objdiff hang are gone.
- Measurement: the objdiff report ignores relocation target names; `tools/relocaudit.py` checks them on matched functions (16 real differences among 1,829, all retail naming).
- Repo: README.md, Markdown without hard wraps, .gitignore cleanup, AGENTS.MD untracked, all commits rewritten to the author caixax.
- Original bugs found are listed in `docs/original_bugs.md`.

Status: hl.dll 2,711 MATCHED + 1 EQUIVALENT of 5,438 functions (44.0% of code bytes); opengl32.dll 367/367; client.dll, hw.dll, GameUI.dll, cof.exe 0.

Running at the end of this entry: agents on player.cpp and on 30 CoF units without source (melee weapons, tool items, two monster families, dual and book weapons).

Next: commit the agents' units; class headers that client.cpp and multiplay_gamerules.cpp still declare locally (documents, boat, wheelchair, telephone, padlock, computer, particle, CEnvModel, CCoFSurvivalMode, CCoopLevelChangeManager, Steam user stats), then UTIL_BloodDrips with CParticle; full re-delink once no agent runs; remaining 56 CoF units; then client.dll (re-test /Ob1 first).

## 2026-10-12: agents resumed, hl.dll at 69.8%

- Resumed the agents cut off by the usage limit (SendMessage keeps their context). Committed per unit: 30 CoF units at 100% (melee weapons, tool items, dual pistol and dual weapon, book laser, wheelchair, 24 monsters), syringe and suicider with one EQUIVALENT function each, pm_shared.c 53/56 (PM_PlayerMove EQUIVALENT: retail calls an empty function that MSVC 19.27 drops), and the HLSDK sweeps: monsters, AI_BaseNPC_Schedule, nodes, talkmonster, genericmonster, monstermaker, h_cine, bmodels, cbase, world, explode at 100%; buttons 84/88, items 60/62, lights 35/39, sound 56/63, util 116/137, effects.cpp 659/733, player.cpp 150/166.
- Delinker: no $LN labels on retail tables (they split functions), int3 after a final no-return call kept, a spurious Ghidra function inside CGraph::CheckNode removed. `tools/symbols/hl_forget.csv` holds names removed for good.
- Names: byte-proven names for the dual weapon helpers and their STL instantiations (now class LIB), MakeMonster/MakeMonsterThink swap, ToggleSwitch, MessageGet, ParticleRestore, FacingIdeal and others; `sync_names.py` sends a byte-proven name to the object that holds it.
- Headers: human hull 12 (not 16).
- `tools/csvio.py`: progress.csv writes use a temporary file with retries (a Windows sharing error had dropped the flare rows once).

Status: hl.dll 3,943 MATCHED + 4 EQUIVALENT of 5,377 functions (69.8% of code bytes); opengl32.dll 367/367; client.dll, hw.dll, GameUI.dll, cof.exe 0.

Stopped at the user's request (token budget): the agents on player.cpp, effects.cpp and the world/utility sweep (their work up to that point is committed).

Next: finish player.cpp (16 functions), effects.cpp (74), util/sound/buttons/lights/items leftovers; class headers still declared locally (client.cpp, multiplay_gamerules.cpp, spitter: cof_importantclassdefs.h, effects.h classes, CParticle, CMp5, CAcidSpit), util.h declarations (UTIL_BloodDrips 5 parameters, UTIL_SplashCalculation, SET_VIEW, UTIL_ProMessage), guess_CLASS_COF_MONSTER, g_sModelIndexNull; full re-delink of all units; the 26 CoF units without source (bosses, doors, puzzles, ladder, boat, subtitles, phone call...); then client.dll.

## 2026-10-12 (second session)

Done and committed:
- hl.dll: util, subs, sound, buttons, lights, items, effects at 100%; player.cpp 218 of 220 plus PostThink, CheatImpulseCommands and ClientCommand EQUIVALENT (register swaps, notes in progress.csv). hl.dll totals (non-LIB): 4,876 MATCHED, 10 EQUIVALENT, 490 TODO of 5,376 (90.7% MATCHED).
- Delinker: biased switch references into case blocks dropped; noexcept destructors keep their 5 int3. bytematch now widens a unit's range to the neighbouring files: 94 already matching hl functions were found outside the attributed ranges and named.
- ScreenFade is 16 bytes in 1.6a (docs/structs.md); LinkUserMessages registers size 10. UTIL_BloodDrips declared once in util.h. Names fixed: EMIT_GROUPID_SUIT, GetClassPtr<CShot/CBloodSplat/CSprayCan/CBasePlayerItem>, gmsgFaggot.
- update_status now reads the check report of the given units (build/report.json is stale) and floors percentages.
- client.dll set up: Paranoia cl_dll copied to src/cl_dll, /Ob1 confirmed (51 functions match only with it, none without). Names: 419 undecorated names decorated (client_decorated.csv), pm_math C names fixed, object layout written to progress.csv (tools/client_layout.py). New tools: align_names.py (position names), name_unit.py (locked naming pass), add_name.py (agent-recorded names with evidence). cdll_int.h uses the HLSDK HSPRITE fix. client totals: 155 MATCHED of 2,146 non-LIB rows.
- docs/agent_prompt.md: prompt template with the name evidence rule; guide has a client section with no notes from the test units.

Stopped mid-work (user out of tokens), NOT reviewed, NOT committed:
- Client batch: Sonnet health+death, saytext+menu, in_camera+inputw32; Opus message+input+status_icons. Edited files in the working tree: src/cl_dll/death.cpp, health.cpp, in_camera.cpp, menu.cpp, saytext.cpp, status_icons.cpp; name files client_bytes/position/manual.csv (client_manual.csv has an agent add_name entry to review), unit lists and progress.csv rows.
- Verified by me before the stop: saytext 6 of 7 (Draw 51.84%), menu 7 of 7, health 3 of 3 named, death 3 of 4 (MsgFunc_DeathMsg needs a viewport member at 0x142c: a CoF class with a Font* at +0xe90 and a panel at +0x3e0, header work for Opus).
- Sonnet observations so far: saytext+menu 54k tokens and 7 tool uses (did not run the naming pass), health+death 81k tokens and 22 tool uses (stopped correctly at the shared header). Metrics script: scratchpad agent_metrics.py (counts check/show/disasm/shell from subagents/agent-<id>.jsonl).

Next:
1. Review the working tree edits above with my own check, then resume or redo the client batch and give the user the comparison table (counted from the command logs).
2. Death notice viewport class (vgui_TeamFortressViewport.h) on Opus.
3. Full re-delink of every unit and a full report compared with progress.csv (no agent running).
4. Remaining hl TODO: CFlier::HandleAnimEvent 99.99%, class header extraction list.

## 2026-10-12 (third session, evening)

Totals (non-LIB rows): hl.dll 5158 MATCHED, 11 EQUIVALENT, 174 TODO of 5343 (96.5 percent); client.dll 749 MATCHED, 1397 TODO of 2146 (34.9 percent).

Done and committed:
- Client batch test finished (Sonnet on health, death, saytext, menu, in_camera, inputw32, message; Opus on status_icons and input). Review fixes needed for Sonnet results: saytext padding struct with casted vtable calls (reverted), health stand-in global and padding member, inputw32 stand-in global, hl GetEyePosition construction, StudioFrameAdvance moved to the wrong file. Opus results needed none. The comparison numbers are in this session's report.
- Verification gap closed: client globals (gHUD at 0x1018A0C0) are named with extents, relocaudit reports wrong members and real symbol differences (with same-address, ICF fold, CRT, STL and import aliases), and update_status keeps such functions TODO. This exposed 25 client and about 40 hl functions that read wrong members or called wrong bases; most are fixed (IMPLEMENT_SAVERESTORE bases of nine CoF classes, SetModelCollisionBox, the m_playerSaveData name, a stale -200 label).
- CHud (0x5590 bytes), CHudHealth, CHudAmmo, CVoiceStatus, TeamFortressViewport and CHUDControl layouts rebuilt; death.cpp, saytext.cpp, message.cpp, menu and status_icons fully matched.
- hl: singleplay_gamerules, doors, locus, animation, animating, h_battery, spectator, paranoia_wpn, monsters, func_break, plats, cof_flygare, cof_phonecall, player GiveAmmo, and the new MP5 file (guess_CoF_MP5.cpp, 30 of 30); 185 header inline functions added to units; STL instantiations classed LIB; statue move functions named by position.
- Tools: bytematch (client, widened windows, relocation tie-break, ICF folds, --std), align_names (position names with guards), name_unit and add_name (locked; add_name writes atomically after a crash truncated hl_manual.csv), client_layout, decorate_names, rtti.py, update_status with the relocation check, and delinker fixes (case-block offcuts, destructor padding, zero-filled extents, decorated import names, multiple-inheritance vtable names).
- Khronos glext.h and khrplatform.h in src/external for the renderer; renderer and VGUI units with baselines (139 of 161 renderer functions match with the Paranoia source).
- 32 units for the Cry of Fear VGUI classes (guess_<Class>.cpp from RTTI and the constructor ranges).

Running when this was written (not committed): renderer milestone (Opus); hud.cpp and hud_redraw.cpp (Opus, also declares the CHud method at 1006CB10 that guess_HUDControl.cpp calls through a free stand-in); hud_spectator, cdll_int, ammo, view and ev_hldm leftovers (Sonnet); Cry of Fear menus batch A (Opus).

Next: review those agents' results with my own check and relocaudit; switch guess_HUDControl.cpp to the CHud method; more Cry of Fear menu batches; vgui_TeamFortressViewport.cpp, vgui_ScorePanel, vgui_subtitles; hl leftovers (about 120 unnamed rows outside units).

## 2026-10-12 (third session, stop)

Stopped for tokens. Committed since the previous note: hud_spectator 28/28 and cdll_int 23/23 (MATCHED), ammo 48/49, view 28/29, ev_hldm 39/40; ModelAttach and Wheelchair globals named once; SetModes, EV_FireGlock2 and the 1009D190 stub fixed in Ghidra; every switch in client.dll and hl.dll checked resolved; guide patterns.

NOT reviewed, NOT committed (in the working tree, from four stopped agents): renderer files (gl_*.cpp, StudioModelRenderer.cpp, GameStudioModelRenderer.cpp, particlemgr.cpp, quake_bsp.cpp, tri.cpp), hud.cpp and hud.h (CHud core agent; was also to declare the CHud method at 1006CB10 for guess_HUDControl.cpp), Cry of Fear menu files batch A and B (src/cl_dll/guess_*.cpp/.h new files), their unit lists, client name csvs and progress.csv rows. Next session: review each with my own check plus relocaudit, then commit or redo.

Open items: guess_HUDControl.cpp still calls a free stand-in for the CHud method at 1006CB10 (two functions TODO); ammo WpnInfo/WpnDeploy/GetMags/SetMags need CHudAmmo methods in hud.h; a ParticleSystemManager member at 10080B90 is needed by MsgFunc_WpnDeploy; V_CalcNormalRefdef and CHudAmmo::Draw are large Cry of Fear rewrites still open.

## 2026-10-13

Totals: hl.dll 5186 MATCHED of 5335 (97.2%), 10 EQUIVALENT, 139 TODO. client.dll 1524 MATCHED of 2159 (70.6%), 19 EQUIVALENT, 616 TODO (from about 1107 MATCHED at the start of the day). opengl32 367 MATCHED. hw.dll, GameUI, launcher not started (VC6 toolchain missing).
Committed today: the Cry of Fear menu batches B, C and D (CCoopStats, CSaveLoad, CVGUIExtraMenu, CUnlockables, CTelephone, CGameMenu, CClothesMenu, CCredits, CAmmoInventory, CDocuments, CAnimalStatue, CVGUIBillboard, CBossBar, CComputer, CDifficulty, CGameOver, CYesno, CSave, CPuzzleBar, CPhone, CKeypad, CUserDocument, CNoteDocument, CRECBInventory); vgui_TeamFortressViewport 143/144, vgui_ScorePanel, vgui_subtitles, hud_redraw 8/11, hud 85/87; util.cpp and the hl/ files (hl_events, hl_objects, hl_weapons) as new units; pm_shared complete in both DLLs (client duck and step punch flags, PM_PlayWaterSounds is return; plus the old body, which also fixed the hl.dll EQUIVALENT); rain.cpp and tri.cpp complete, StudioModelRenderer 34/39, gl_studiomodel 22/25; about 40 already written functions that were missing from their unit lists (voice_status, voice_banmgr, hud_msg, entity, parsemsg, saytext, text_message, hud_spectator, hud_servers and others).
Nothing from the four agents stopped at the end is left in the working tree except what was committed (the hud, view and glmanager agents had not written sources yet). The untracked wayback_patches.txt in the root is not ours; it was left alone.
New units without code yet: client/cl_dll/glmanager (Paranoia GLManager, 14 functions at 99.x%: retail members sit 0x48 bytes later from about +0x138, and DrawBlurTest shows gl+0x54 differs too, so check the whole layout) and client/cl_dll/mp3 (Cry of Fear irrKlang and id3lib music code, 1007E870..10080780, three functions named; other files already call guess_IrrKlangPlay (1007FAF0), guess_IrrKlangSetLoop (1007FBB0), guess_IrrKlangStop (1007FC10)).
The GLManager global gl (0x101BE5F8, 0x228 bytes) is now named in the client map, so relocaudit checks every gl.member offset. GL_RestoreRenderer and DrawBlurTest dropped to TODO for that reason; run tools/update_status.py on every renderer unit (gl_*.cpp, quake_bsp, glows, StudioModelRenderer, hud_redraw) to find the others, then fix glmanager.h.
Relocaudit is blind for globals the client map does not name: gEngfuncs (2800 references), gViewPort (1005), pmove (452), IEngineStudio, g_fxArray, g_HeadShieldEnt, gWR, render_origin, v_origin, v_angles and more are DAT_ in the delinked targets. Name each one with its address and extent (client_manual.csv plus client_datasizes.csv) and rerun update_status on all client units; expect a few more MATCHED functions to show wrong member offsets.
Next client work, in order: GLManager layout and the renderer recheck; hud.cpp and hud_msg.cpp Cry of Fear handlers (about 30, incl. CHud::CheckItem at 1006EE90 and GetCurrentPath at 1006EE60, name from the hl.dll PDB: _getcwd( path, 260 )); view.cpp (V_CalcNormalRefdef 8402 bytes plus eleven unnamed functions at 100AD9D0..100AF150 and one at 100B31E0); mp3.cpp; ammo.cpp Draw (8615) and helpers; input.cpp leftovers (InitInput, CL_CreateMove, IN_*Down); entity.cpp HUD_StudioEvent and four more; particles (ParticleType/ParticleSystem layouts); voice_vgui_tweakdlg (m_pVoiceTweak is at 0xD8, before m_DlgWidth, m_DlgHeight, m_Label).
Facts learned: an empty thiscall method compiles to ret 0 and its cross-file callers keep the call (no dead code needed); a free function called from the same file needs return; followed by the old body or the call disappears. Header inline VGUI functions (ColorButton::paint, CMenuPanel::Open, CTransparentPanel::paintBackground) are compiled by many objects; their retail copy belongs to one object only. Ghidra treats a function that only jumps as a thunk; when a tail call lands on the next function Ghidra merges both (Game_AddObjects and UpdateBeams, UpdateOnPlayerInfo and the CHUDControl method at 1003BC00).
hl.dll leftovers unchanged: 139 TODO, mostly unnamed STL rows and header inline functions; hl _sscanf (100017B0) should be class LIB; TeamID at 100017A0 is a second 6-byte function returning another string (find its real owner).

## 2026-10-14

Totals: hl.dll 5219 MATCHED of 5250 (99.2% of bytes done with 10 EQUIVALENT, 21 TODO). client.dll 1745 MATCHED of 2080 (82.9% done with 19 EQUIVALENT, 316 TODO). opengl32 367 MATCHED. hw.dll (9169), GameUI (5938) and cof.exe (277) all TODO, waiting for Visual C++ 6.0. CoFLaunchApp.exe (.NET) not inventoried yet. The progress SVG now draws every binary, including the ones not started.
Client map: 582 globals named with their extents (tools/symbols/client_manual.csv, client_datasizes.csv), from relocation pairs of functions with identical size (tools/globals_vote.py and tools/globals_name.py; a global defined in no object of ours gets its type size, not the gap to the next symbol). That exposed real bugs that the audit could not see before: CAM_Think, IN_JoyMove and IN_MouseMove test guess_iPlayerMode (101B2F10, the server pev->fuser3 sent in vuser4[0]) instead of g_iUser1; cam_ofs is 1054370C (CL_CameraOffset), guess_cam_old is 10543700; guess_iKeyEvents was the AttachCam entity pointer guess_pAttachCamEnt (10542BC0) with guess_iAttachCamFlag (10542BC4); CHud::Think swapped two floats; CHud::VidInit stored guess_vecIronsightOfs components in the wrong order; CCredits::guess_ShowUnlock resets g_HeadShieldState, not the head shield entity; DrawBlurTest calls guess_CaptureBlurTexture with four arguments. A renamed global or function keeps its old Ghidra label as primary: add the old label to client_forget.csv (or hl_forget.csv) or the delinker keeps exporting it.
GLManager: retail has 0x48 bytes before the gl function pointers (six 12-byte arrays; Init fills the ones at 0x30 and 0x3c). With that, every renderer function verifies against the named gl global, and glmanager.cpp is 19 of 19 (VidInit offers to switch to OpenGL and writes LaunchOptions to the Steam registry key).
Other work: HLSDK game_shared vgui_checkbutton2, vgui_grid, vgui_listbox, vgui_slider2 and ScrollBar2 were classified LIB; they are units now and match (VGUI Dar<> templates stay LIB, like STL). STL and iostream rows in the menu objects and in hl.dll classified LIB. Header inline VGUI functions added to the units that hold their retail copy. hl.dll: util.cpp and weapons.cpp complete, player.cpp 241 of 243 (several functions were written but never named in the map: name them with add_name.py and they match or need one small fix), guess_CoF_MP5 32 of 32. Helpers added: tools/stage_rows.py stages only the given rows of a symbol csv (the agents keep writing to the same files), tools/set_sources.py sets the progress source file of a unit's functions. Agents' work committed: hud_msg 46/53, input 74/75, inputw32 18/18, entity 7/8, particlesys 17/17, particlemsg 5/5, particlemgr 8/9, view 35/41.
Open client work: hud.cpp CoF handlers (CheckItem 1006EE90 and its tokenizer 1006ED50, GetCurrentPath 1006EE60, AddNote, AttachCam, BigPrick and the small ones), the rest of hud_msg, view.cpp (V_CalcNormalRefdef and the unnamed functions), mp3.cpp (irrKlang and id3lib), ammo.cpp Draw, StudioModelRenderer and gl_studiomodel leftovers, the 51 unnamed functions after rain.cpp, vgui_tabpanel.cpp (unit created; the Paranoia file does not compile yet: FontFromMessage takes const char *&), the CRadioIcon panel after vgui_paranoiatext (1009C630..1009CA90), the blur code after battery.cpp (1001CB30..1001CF80, direct opengl32 calls, cv_blurtest), the CS-model flash code after demo.cpp, the parser after text_message.cpp, CHud2::Initialize (writes +0xF8 and +0x108, check the constructor layout), CTeamMenuPanel constructor.
Open hl.dll work: multiplay_gamerules (100C5040, 100C5300 4706 bytes, 100C82C0), plats (100D49D0, 100D6AE0), cof_dualpistol (1002F1E0, 100358C0), the Steam group helper 100E4E50 and its caller 100DC080, 100DCAF0, 100F4C30, 100B23A0..100B2740, 100179F0, 100010A0, the CVoiceGameMgr and header inline rows. tools/cand_sizes.py lists unnamed retail rows of a unit next to our unmapped functions of similar size.

## 2026-10-14 (evening)

Totals (non-LIB rows): hl.dll 5243 MATCHED, 11 EQUIVALENT, 83 TODO (5337). client.dll 1819 MATCHED, 20 EQUIVALENT, 230 TODO. opengl32.dll 367 MATCHED. hw.dll, GameUI.dll and cof.exe not started (Visual C++ 6.0 is installed in I:\Tools\vc6; hw.dll also has VC6 processor pack and VS 2005 objects). CoFLaunchApp.exe not inventoried.

Committed: hud_msg 52 of 53 (MsgFunc_Objectives EQUIVALENT), hud 88 of 90, hud_update.cpp unit (CHud::UpdateClientData), in_fov and cl_fovmultiplier named; hl leftovers (ApplyCFGSettingToPlayer EQUIVALENT 99.98%, ApplyCFGSettingToPlayerLate, DeleteAllCFGFiles, h_ai.cpp unit, CreateWithScript, CSprayCan::Spawn, guess_IsInSteamGroup, guess_CanUseWeapon, CDualWeapon stubs, CBasePlayerItem::AddDuplicate inline in weapons.h); view.cpp V_CalcNormalRefdef rewritten (97.87%, differences in progress.csv notes).

Inventory holes: `python tools/gaps.py <dll>` lists game code that no progress.csv row covers (code, switch tables, import thunks). hl.dll had 14.5 KB of code holes: tools/ghidra_scripts/FillGaps.java created the functions in Ghidra and 98 rows were added (11 LIB, 87 UNKNOWN; 83 still TODO). They include GameDLLInit (0x100AE130, 5092 bytes, game.cpp), client.cpp interface functions reached only through the function table (GetGameDescription, RegisterEncoders, ServerDeactivate, StartFrame, ParmsChangeLevel), DeadRemoveThink (0x10017AA0), the spectator methods, and five copies of one 637-byte function (0x1002B230, 0x100580A0, 0x10059DC0, 0x100722B0, 0x10078CE0; a static function from a header, followed by a table Ghidra defined as data). bytematch proposed 50 names for them (tools/symbols/hl_bytes.csv) but only 4 rows took a name after the delink: check why first. client.dll has 7.9 KB of code holes (114), not filled yet: run FillGaps for client.dll the same way (scratchpad runner: args are start:end pairs from gaps.py).

hl.dll link: `python tools/build.py link hl` (link section in tools/units.json: link order from the retail image, uppercase alphabetical with the two guess_ units placed by address; src/dlls/hl.def; steam_api.lib generated by tools/implib.py; rc.exe from the SDK bin). It now fails only on unresolved externals: no units yet for dlls/globals.cpp (g_vecZero, g_ulFrameCount, g_Language, g_iSkillLevel, gDisplayTitle, g_fGameOver, the CoF mode globals m_iSimonMode, g_iSurvivalMode, g_iDoctorMode, g_fNightmare, g_iAoMLevels, m_iMusicManagerLoops, m_fMusicManagerFadeTime, m_bHadSimonMode) and dlls/game.cpp (the cvars and GameDLLInit); gDualWeapon must be defined in cbase.cpp and showobjective in effects.cpp (1.55 PDB objects); CBaseEntity::DeadRemoveThink is missing from cbase.cpp. The 1.55 PDB (docs/symbols_1.55.csv) lists every symbol of game.obj and globals.obj.

Open client items: hud_msg.cpp declares guess_MuzzleFlash with an int third parameter, entity.cpp defines it with bool (the client link will fail; hud_msg calls it with 0). 0x1055959C is a float (view.cpp defines guess_flUnknown1055959C) but ammo.cpp and hud.cpp still use extern int guess_iUnknown1055959C, and the map has the int name. 0x105595C0 is guess_vecWheelchairViewAngles (input.cpp uses guess_vecWheelchairAngles, which is 0x105594FC). 0x10542BB3 is guess_bAttachCam in StudioModelRenderer.cpp and guess_bUnknown10542BB3 in hud_msg.cpp. guess_iWheelchair and guess_iBoat are used as cl_entity_t pointers (view.cpp casts them; retype them). Map entries still to add from the view agent: 100ADEC0 ?guess_CalcViewModelLag@@YAXMAAVVector@@00PAUcl_entity_s@@_N@Z and 100B31E0 ?guess_VectorIsValid@@YA_NMMM@Z (old labels to client_forget.csv, replace them in client_cl_dll_view.txt), 1004BC30 guess_MuzzleFlash (entity unit), data 105458D0 guess_vecBoatOrigin, 105595C0 guess_vecWheelchairViewAngles, 10550F00 guess_vecModelAttachOrigin, 10550318 and 10550330 guess_vecDualAttachment0/1, 104CE4A3 guess_bUnknown104CE4A3.

mp3.cpp is blocked: it needs the irrKlang 1.3.0 SDK headers and the id3lib 3.8.3 headers (official downloads, headers only, kept out of git).

Uncommitted: three agents (renderer leftovers 3, client HUD core, client VGUI and menus) were stopped mid-task. Their edits are still in the working tree (src/cl_dll: battery, in_camera, input, vgui_hud, guess_CgContext/CgWater/PBuffer/TextureManager and others; src/game_shared/voice_status.cpp; client symbol rows and unit lists). Not verified: next session, run `build.py check` on their units, compare with HEAD for regressions, then commit or revert each file.

## 2026-10-15

hl.dll: every function MATCHED or EQUIVALENT (5317 MATCHED, 11 EQUIVALENT, 0 TODO). The 5 "functions" left by the inventory were 3-byte nops padding switch tables (dropped). The full link (`python tools/build.py link hl`) now has the same section addresses and sizes as retail, identical imports and export names, identical .rsrc: retail was compiled with `/Zc:inline` (unreferenced inline functions are not emitted) and linked with `/OPT:NOREF /OPT:NOICF` (link section `cflags_extra`, objects in build/link_obj so objdiff keeps the /Gy objects). Functions are placed alphabetically by decorated name inside each object, which is why the user switched unproven names to the `_guess` suffix (AGENTS.md; rename pending until no agent edits src/: `python tools/rename_guess.py --write`). Remaining layout drift: the EQUIVALENT functions with a different size (CSuicider::Killed -16, CBaseMonster::Killed -32 through alignment), 16-byte padding differences inside effects.cpp (retail has 17 bytes of int3 after Save@CCofSkipManager at 1009AFDF, cause unknown), then .data order. Game test (`tools/run_game.ps1`, `make_dist.py --keep`): the rebuilt hl.dll plays the intro of c_start into c_start2 with logs identical to retail.

client.dll: 1993+ MATCHED; inventory holes filled (191 rows, tools/gaps.py and FillGaps.java); link section set up (`build.py link client`): it fails only on undefined globals and the mp3 functions (list with `python tools/data_home.py client --link-log`).

hw.dll: VC6 /Od confirmed for the engine; first unit engine/crc.c 13/13 MATCHED. hw.so and gameui.so carry full DWARF: tools/elf_files.py (file and definition line of every function), dwarf_file.py (prototypes and locals), dwarf_headers.py (original header declarations), align_linux.py (names by source order between anchors), vtable_linux.py (virtual methods through RTTI and _ZTV vtables), range.py (pseudo C plus exact strings of an address range). hw.dll 62% and GameUI.dll 47% of code attributed to source files; 428 hw functions identical to VC6 LIBCMT.LIB classified LIB. Unattributed hw regions: vgui2/controls library, D3D wrapper, Windows-only engine files (snd_win and friends), Aureal A3D SDK, DirectX helpers (to classify).

Running at the end of this entry: agents on client StudioModelRenderer/gl_studiomodel, client mp3 (irrKlang 1.6.0 and id3lib 3.8.3 headers in refs/), hw cvar/cmd/zone/info/CONSOLE/KEYS, hw common/delta/net_chan.

## 2026-10-17

Counts (non-LIB rows): hl 5317 MATCHED + 11 EQUIVALENT of 5328; client 2136 MATCHED + 21 EQUIVALENT of 2234 (77 TODO); opengl32 367 of 367; cof.exe 43 MATCHED + 1 EQUIVALENT of 52 (the other 8 rows are VC6 $E static initializer helpers objdiff cannot pair); CoFLaunchApp 49 + 2 EQUIVALENT of 51; hw 465+ MATCHED of 8741; GameUI 22 of 5549 (pilot running).

Names: unproven names use the `_guess` suffix (rename done, commit caa2ab8). Literal check: relocaudit now compares string literals and float constants with the retail bytes (MSVC `??_C@` names carry a JamCRC of the whole string, VC6 `$SG` literals are read from the object). It found wrong values behind 100% scores in hl and client (weapon timings, GAME_NAME, sprite and sound paths), all fixed in 7e16ff3; hl and client now audit with 0 real differences except ~CBlackBar.

client.dll links from src (3dd8e01) and loads in the game: `python tools/build.py link client`, `python tools/make_dist.py --keep`, `powershell -File tools/run_game.ps1 -Seconds 90 -Map c_start -Log <log> -Shots <dir>`. With the rebuilt hl.dll, client.dll and opengl32.dll the c_start intro runs with the HUD; in one run the level change to c_start2 did not happen within 90 s while the window had lost focus (to recheck). implib.py now imports stdcall names undecorated (DevIL). Debugging the game: WinDbg (official, winget) with cdb copied to I:\Tools\windbg_x86; scratchpad scripts hang_stacks.ps1 and hang_windows.ps1 dumped stacks and windows of a stalled run.

cof.exe links from src (407854b): flags /G5 /W3 /O1 /MT /GR, link section with /debug and /base:0x1400000; remaining link differences: a 33 MB zero block in retail .data (unknown owner), global order, no .rsrc yet.

hw.dll: committed units crc, cvar, cmd, zone, info, CONSOLE, KEYS, common, delta, net_chan, pmovetst, world, hashpak, sv_log, sv_upld, all at 100% with 0 real relocation differences. tools/globals_linux.py names 653 hw globals by pairing the Linux hw.so relocations through the functions that use them (Jaccard); `--members <global>` shows where Windows struct offsets differ. tools/vc6_slots.py predicts VC6 /Od stack slots (a hash of the local names). DelinkUnits fixes: names of C functions with a leading underscore (_PM_PlayerTrace) no longer ping-pong with their siblings, and a global's extent (tools/symbols/hw_datasizes.csv) may clear unreferenced typed data. The Ghidra MCP server: `tools\ghidra_mcp_server.bat --project I:\Projects\cof-decompiled\re\CoF.gpr --program /1.6a/hw.dll`.

Running at the end of this entry: agents on hw GL renderer core (qgl.c, GL_DRAW.C, GL_RMAIN.C, GL_RMISC.C, GL_WARP.C), hw CL_MAIN.C and client.h, GameUI pilot (gameui section in units.json, GameUI_Interface.cpp and small files, src/gameui, src/vgui2, src/public).

Later on 2026-10-17: hw.dll 1012 MATCHED (crc, cvar, cmd, zone, info, CONSOLE, KEYS, common, delta, net_chan, pmovetst, world, hashpak, sv_log, sv_upld, CL_MAIN, qgl, GL_DRAW, GL_RMAIN, GL_RMISC, GL_WARP all 100%); GameUI.dll 138 MATCHED (build section VC6 /G5 /W3 /Od /MT /GR; units public/interface, GameUI/Sys_Utils, GameUI/GameUI_Interface, vgui2/controls/controls, public/KeyValues); client.dll 2181 MATCHED + 23 EQUIVALENT (dynamic initializers via tools/dyninit.py, 17 menu destructors, VGui_Shutdown's SteamAPI_Shutdown, MsgFunc_ViewMode, the unused MuzzleFlash_guess variant); the linked client.dll now has retail's image size (muzzleflash_guess and tpmuzzleflash_guess are 9-entity arrays; tools/data_drift.py).

Findings: hw.dll's 7 processor pack objects are bzip2 1.0.2 (official source compiles with VC6 /O2 /DWIN32 /DBZ_EXPORT; the API functions match, the rest needs cl 12.00.9044, asked the user). docs/hw_regions.md maps the unattributed hw code: engine files (in_win, snd_win, GL_VIDNT, vgui2 engine surface, voice record), the vgui2 controls library, the D3D wrapper, and third-party IGA/Radial, Aureal A3D, DirectX manager (scope question to the user). refs/Quake (id Software) added as the naming reference for Windows-only engine files. Ghidra fixes: DelinkUnits keeps names of C functions with a leading underscore, extents may clear unreferenced typed data; one-jump functions with real names are no longer Ghidra thunks in client.dll (the thunk hid DirectorMessage's READ_WORD calls).

Registered and waiting: hw units sv_phys, sv_user, pr_cmds, r_studio, CL_TENT, R_PART, SND_DMA, SND_MIX, SND_MEM, net_ws, Sequence, CL_DEMO. Running at the end of this entry: agents on hw sv_main.c, hw CL_PARSE/cl_ents/cl_pred/cl_parsefn, hw host.c/host_cmd.c, GameUI vgui2 controls (Panel, Label, Button, EditablePanel, to be shared with hw.dll).
