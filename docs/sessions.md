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
