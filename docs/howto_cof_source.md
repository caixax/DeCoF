# Writing a Cry of Fear source file for hl.dll

How the first files (`src/dlls/CoF_Glock.cpp`, `src/dlls/CoF_P345.cpp`) were written and matched. Follow the conventions in README.md for everything else (naming, writing rules, no decompiler artifacts, guess_ prefix).

## Inputs for one unit

| What | Where |
|---|---|
| Functions of the unit (1.6a names) | `tools/units/hl_dlls_<file>.txt`, `docs/progress.csv` |
| Function order and line in the original file (1.55 debug build) | `docs/lines_hl_1.55.csv` (write the file in this order) |
| Ghidra pseudo C (reference only, never commit) | `build/decomp/hl/dlls/<file>/` (`python tools/build.py decomp hl/dlls/<file>`) |
| Exact instructions, strings, constants, member offsets of globals | `python tools/disasm.py hl "<decorated name>"` |
| The same function in the 1.55 debug build (statement lines from the PDB line table, block structure, dropped constants, dead code) | `python tools/disasm.py --version 1.55 hl "<decorated name>"` |
| Parameter and local names, block structure (1.55) | `python tools/pdbtypes.py versions/1.55/cl_dlls/hl.pdb func "CFoo::"` |
| Class declaration (members, methods, order) | `python tools/hdrgen.py CFoo` (weapon classes are already in `src/dlls/weapons.h`) |
| Member at an offset / vtable slot | `python tools/member.py --src src/dlls/items.cpp CBasePlayerWeapon 0x1e4`, `python tools/member.py --vt CBasePlayerWeapon 0x198` |
| Animation enum | `python tools/mdlseq.py versions/1.6a/cryoffear/models/weapons/<w>/v_<w>.mdl PREFIX` |

## Keep the output small

Every tool output stays in the working context, so large outputs repeated over many builds are the main cost of this work:

- `python tools/build.py check <unit>` prints only the functions below 100% (`--all` for every function).
- `python tools/objdiff_tool.py show <unit> "<name>"` prints only the differing rows with 3 rows of context (`--full` for every row).
- `python tools/disasm.py --version 1.55 --outline hl "<name>"` prints one row per source statement (calls, strings, constants, member offsets, jump targets as line numbers): read the 1.55 build this way and use the full listing only for the one statement you need. `--outline` also works on the 1.6a code. A byte count limits any listing: `python tools/disasm.py hl <va> 0x80`.
- Do not print whole files or whole decompiler dumps; read the part you need.

## Loop

1. Write the file (functions in the 1.55 line order, class `CFooAmmo` defined in the .cpp with in-class bodies like the HLSDK ammo classes).
2. `python tools/build.py check hl/dlls/<file>`: compiles the unit and prints the match of every function (one-unit objdiff report).
3. `python tools/objdiff_tool.py show hl/dlls/<file> "<decorated name>"`: retail on the left, ours on the right, differing rows marked `*`. Rows that differ only by a Ghidra name on the left (`s_...`, `DAT_...`, `FUN_...`) are naming, not code: they disappear after a harvest and a re-delink and need no source change.
4. For shape questions (if/else order, operand order, locals) write the alternatives into a variants file and run `python tools/try_variants.py hl/dlls/<file> "<decorated name>" <file> --keep`.
5. After 8 failed attempts on a function, stop: it stays TODO or becomes EQUIVALENT with the exact remaining difference in progress.csv.

## Facts that hold for the CoF weapons

- Globals: `gpGlobals->v_forward` is +0x28, `v_up` +0x34, `v_right` +0x40. Shell eject: `vecShellVelocity = gpGlobals->v_right * 220 + gpGlobals->v_up * 200 + gpGlobals->v_forward * 40`, `vecShellOrigin = pev->origin + pev->view_ofs + gpGlobals->v_forward * 80 + gpGlobals->v_right * 20`, `EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL )`.
- Fire event: `UTIL_MakeVectors( v_angle ); Vector vecSrc = m_pPlayer->GetGunPosition( ); Vector vecShellEnd = vecSrc + gpGlobals->v_right * 32;` then `PLAYBACK_EVENT_FULL( flags, ..., m_usThirdFlash, 0.0, m_pPlayer->pev->origin, vecShellEnd, 0, 0, m_iId, 0, 0, 0 )`; `int flags; if ( m_pPlayer->m_iViewValue != 1 ) flags = FEV_NOTHOST; else flags = 0;` comes before `UTIL_MakeVectors`.
- Bullets: `vecDir = m_pPlayer->FireBulletsPlayer( 1, vecShellSrc, vecAiming, m_AccuracyVector, 8192, BULLET_PLAYER_357, 2, gSkillData.Dmg<Weapon>, m_pPlayer->pev, m_pPlayer->random_seed )` (check the bullet type and the gSkillData member against the disassembly).
- Fire sound: `guess_UTIL_PlayWeaponSound( "weapons_new/<w>/<w>_fire.wav", 0, true, m_pPlayer->EarPosition(), m_pPlayer, true )`.
- Timers that `ja` skip in retail (`comiss member, time; ja skip`) are written `!( member > gpGlobals->time )`; plain early returns are `if ( member > gpGlobals->time ) return;`.
- In ItemPostFrame, `m_iSprintState = 0;` is the last statement of the sprint/jump blocks (after the timers): MSVC hoists the store up to the last read of gpGlobals->time, so its retail position tells where it was.
- Store a GetSequenceTime result in the function scope `float time;` before using it (`time = GetSequenceTime( ... ); m_iNextSprintAnim = time + gpGlobals->time;`).
- The suicide punch sound: `{ int pitch = 90 + RANDOM_LONG( 0, 15 ); EMIT_SOUND_DYN( ..., pitch ); }`.
- `pev->body` from the flashlight flag: `if ( m_pPlayer->m_iFlashlightFlags & 4 ) pev->body = 2; else pev->body = 1;` (not a ternary).
- Which branch comes first (`if ( m_iClip )` or `if ( !m_iClip )`) varies per weapon and per function: follow the retail jump (`je`/`jne`) and the push order.
- Ammo `AddAmmo`: two separate `GiveAmmo` calls in an if/else on `g_pGameRules && g_pGameRules->IsMultiplayer()`, each followed by the `EMIT_SOUND` and `return TRUE;`, then `return FALSE;`.
- `GiveAmmo` has a fourth argument (`false`); `SendWeaponAnim` defaults (`skiplocal = 1, body = 0`) are used everywhere.
- Ironsight toggle button is `IN_CANCEL`; suicide button mash tests `IN_ATTACK | IN_ATTACK2`; sprinting is `pev->flags & (1<<18)`.
- `IronsightsTo`/`IronsightsFrom` are the same in every weapon except the enum names (copy from `CoF_P345.cpp`).

## Read the 1.55 debug build first

The 1.55 hl.dll is an unoptimized debug build of nearly the same source. `disasm.py --version 1.55` prints a `-- line N` row where each source statement starts (1.55 PDB line table): write the statements in that order and with that if/else layout. Most remaining differences after a first draft come from source structure that the 1.6a release build merges or drops, and the debug build still shows it:

- Statements repeated in both branches of an if/else (the release build merges the copies after the branches). Example: glock AttachTaclight is `if ( m_iClip ) { SendWeaponAnim( GLOCK_DRAW ); m_pPlayer->m_flNextAttack = gpGlobals->time + 1; } else { ...NOSHOT...; same store }`. Writing the store once after the if/else changes register use.
- Unused locals whose initializer has side effects or affects register allocation. The 1.55 PDB lists them (`tools/pdbtypes.py ... func`). Examples: `float PlayerSpeed = m_pPlayer->pev->velocity.Length2D();` after `Vector vecDir;` in PrimaryAttack (without it the v_angle + punchangle sum loads its x operands in the other order); `int pitch = 95 + RANDOM_LONG( 0, 10 );` left before a sound call that no longer uses it (retail calls RANDOM_LONG and drops the result).
- Constants the release build folds away (the `+ 0x5f` above is the 95).
- `x + 1` against `x + 1.0`: the debug build shows `fadd dword` (float constant, write `1`) or `fadd qword` (double, write `1.0`).

## More patterns

- Operand order of a sum: the operand retail loads first is usually the second one in the source. `v_angle + punchangle` loads punchangle (+0x68) first.
- Comparison constants survive: `cmp 3; jl` is `>= 3`, `cmp 2; jle` is `> 2`. Write the constant that appears in the retail `cmp`.
- `if ( g_pGameRules && !g_pGameRules->IsMultiplayer() )` and `if ( !g_pGameRules || !g_pGameRules->IsMultiplayer() )` differ in where the null test jumps: both jumps to the same target means `&&`.
- Spawn tests `pev->frags == -2` in most weapons (P345 uses -1): check the constant per weapon.
- Weapons without `_NOSHOT` sequences send every animation without a clip test; the guide's clip-branch notes only apply where the model has them.

- A four-timer chain `m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flNextTertiaryAttack = m_flTimeWeaponIdle = gpGlobals->time + X;` compiles to `shufps` + `movups [esi+0x1cc]`: when retail has that pair, write the chain.
- `m_bReloading = m_bWantToEndReload = false;` compiles to one word store.
- Identical blocks written twice in the source (early outs, doctor mode and normal `MESSAGE_BEGIN` blocks) are merged by the compiler: write them twice when the 1.55 line rows show two copies.
- Globals that no header declares (`g_iDoctorMode`, `m_usRevolverShells`, `gmsg*` ids) take an `extern` at the top of the .cpp, with the type from the 1.55 PDB (`docs/symbols_1.55.csv`, module `globals.obj` and others).
- Switch jump tables are part of the function on both sides since the delinker change of this session; `switchD_...` names on the retail side are naming only.

## Patterns from the second batch of weapons

- The fire event flags follow the 1.55 form of each weapon: `int flags = 0; if ( m_pPlayer->m_iViewValue != 1 ) flags = FEV_NOTHOST;` (M16, rifle, shotgun, FAMAS, G43) or the if/else form (pistols). Both give `setne`, but the choice moves other code, so take it from the 1.55 line rows.
- A 1.55 `int pitch = 95 + RANDOM_LONG( 0, 10 );` before the fire sound is kept only when retail 1.6a still calls RANDOM_LONG there (VP70); in most weapons 1.6a dropped the line together with the call.
- Checks that an earlier return already implies (`m_iSprintState == 1 || m_iSprintState > 2` after `if ( m_iSprintState ) return;`) are written anyway in the 1.55 order; the release build removes them.
- Two calls that differ only in one argument (FireBulletsPlayer with `m_AccuracyVector` or `VECTOR_CONE_1DEGREES`, three GiveAmmo calls) are written out in full as in 1.55; retail merges them into one call with a select.
- `test [m_afButtonPressed], 0x801` can come from `( x & IN_ATTACK ) || ( x & IN_ATTACK2 )`.
- A WeaponIdle made only of early returns compiles to `ret 0`, the same as an empty body: keep the 1.55 body.
- Rifle-class melee: `if ( m_pPlayer->m_fStamina <= 0 ) return;` then `m_pPlayer->SetNextThink( 0.23 ); m_pPlayer->SetThink( &CBasePlayer::MeleePushBack );`. Pistols use `WeaponMelee`.
- Take member offsets from 1.6a (`tools/member.py`); 1.55 offsets differ (CBasePlayerWeapon members 4 lower, CBasePlayer very different). Use 1.55 only for statement order.
- The `check` printout rounds per-function percentages: trust the unit line and `matched_functions` in `build/check_project/<unit>/report.json`.

## Patterns from the base units (weapons, combat, client, gamerules, triggers)

- The 1.55 debug build is not always the same code as 1.6a: compare with the 1.6a disassembly before trusting a 1.55 statement (FireBulletsPlayer moved the doctor boss test into the shot loop; entvars_t offsets from 0x128 on are 4 higher in 1.6a).
- A call in the 1.55 debug build with no test after it comes from an `if` whose whole body is commented out; the release build keeps the call and drops the test.
- MSVC 19.27 drops calls to functions of the same file whose compiled body has no side effects (empty void function, `return FALSE;`), whatever the definition order and even with `__declspec(noinline)`. If retail keeps such a call, it cannot be reproduced from one file; note it and move on. Exception: an empty function that takes a `Vector` by value (`CBoat::Splash`) keeps its calls, because building the argument copy is not removed with it.
- `/Ob1` sometimes keeps an inline helper as a call (the third `FStrEq` of a function stays out of line while the first two are inlined): write the source plainly. A helper the 1.55 build shows as a function (`GET_INFOBUFFER`) has to be `inline` in our source.
- Write `FStrEq(...)` where retail calls it, `!strcmp( STRING( pev->classname ), ... )` where retail reads `pev->classname` directly, and `FClassnameIs( edict(), ... )` where retail goes through `pContainingEntity`.
- A dead `sub eax, N` after the case tests of a switch gives the value of a case whose body was merged into default: a cheap check of enum values.
- After `ucomiss`/`fucompp` and `test ah, 0x44`: `jnp skip` means the source says `!=`, `jp skip` means `==`.
- Register base/index order in inlined copy loops and the load order of commutative operands depend on the whole function; a change in one block can flip another. Stop early on these.
- `for ( i = 0; i < ARRAYSIZE( a ); i++ )` with a function scope `int i` compiles to an unsigned compare (`jb`).
- A call written `CGameRules::VoteKickThink()` is a direct call in both builds: when the 1.55 build calls a virtual directly, the source used the qualified name.
- A Steam API call compiles to `call [IAT]` and needs `extern "C" __declspec(dllimport)`.
- In many objects the 1.6a functions are laid out in alphabetical order of their decorated names (some, like cbase.obj, keep source order). The neighbours of an unnamed retail function tell roughly what its name must be.
- Functions unnamed in the retail map are missing from the unit lists and never reported. `tools/bytematch.py` pairs them with our functions by bytes; check the address gaps of a unit when a function you wrote is not in the check output.
- To test a header change without touching the repo, copy the .cpp next to a modified copy of the header in a scratch folder (`#include "x.h"` looks next to the .cpp first; an extra `/I` does not override it).

## Patterns from the melee weapons

- CAxe, CNightstick, CBranch, CSledgeHammer and CBookSimonSledgehammer are not in weapons.h: declare them in the .cpp (from `hdrgen.py`) before `LINK_ENTITY_TO_CLASS`, with their small inline methods (`ShouldWeaponIdle`, `CheckRangeAttack1/2`) in the class body. The 1.55 rows tell where each class lives.
- Where 1.55 calls `EMIT_SOUND` (the util.h inline), retail shows `EMIT_SOUND_DYN( ..., 0, 100 )`: write `EMIT_SOUND`.
- A pushed constant sequence in `GetSequenceTime` instead of a load of `pev->weaponanim` means the source passes the sequence directly.
- The melee ItemPostFrame is one template (axe, nightstick, branch, booklaser): function scope `float speed` and `float time = 0`, a second `float speed` inside the fourth block; the sledgehammer drops the suicide check, the half time on sprint-to and the idle timer on sprint-from.
- Content can differ from 1.55: Book Simon's SetYawSpeed and Spawn read the `cof_monsterhumanyawspeed` and `cof_monsterhumanscale` cvars in 1.6a. Use 1.55 for statement order and check the content against retail.

## Patterns from the CoF monsters

- The 1.6a monsters read cvars where 1.55 had constants: `SetYawSpeed` uses `ys = CVAR_GET_FLOAT( "cof_monsterhumanyawspeed" );`, `Spawn` uses `int guess_iScale = CVAR_GET_FLOAT( "cof_monsterhumanscale" );` then `UTIL_SetSize( pev, Vector( -s, -s, 0 ), Vector( s, s, 72 ) )` (one int local gives the single `cvttss2si` plus `neg`).
- entvars `+0x114` is `body`: `if ( pev->body == -1 ) pev->body = RANDOM_LONG( 0, 10 );`. The view offset `0x41e00000` (28) is `VEC_VIEW`.
- `CheckTraceHullAttack` has no default arguments in CoF: `( 55, gSkillData.x, DMG_SLASH, false, 0 )`.
- Keep the HLSDK `velocity + v_forward * -100`; retail compiles it to `mulss 100` plus `subss`.
- In 1.55, a `break` in the last switch case produces no code; a missing `break` before another case is a missing `jmp`, which is how a fallthrough shows.
- A local the 1.55 PDB lists for an otherwise empty body is still declared (`CSlower3::AlertSound` is `int pitch;` with the rest commented out; retail is `ret 0`).
- The Killed copies inline the kill statistics: `if ( m_iLastDamageType & (1 << 26) ) ((CBasePlayer *)pAttacker)->playerstats.monsters_killed_melee++; else ...monsters_killed_shooting++;` then `GetPointsForKill( (char *)STRING( pev->classname ), (CBasePlayer *)pAttacker )`; only the twitchers add the `weapon_axe` check that sets `m_iFoundHoodies |= 0x20000`.
- `bool newgore;` is defined in CoF_Slower.cpp (the 1.55 PDB module); player.cpp declares it `extern`.

## Patterns from the tool items

- For commutative adds the 1.55 operand order is not reliable for 1.6a: `Ignite` needed `gpGlobals->time + gSkillData.flareBurnTime` although 1.55 has the reverse. The general rule (retail loads the second source operand first) is what worked.
- A retail function with no entry in the symbol map is absorbed by its neighbour and shows as a very low match on an otherwise identical function: check the address against our functions with `tools/bytematch.py` or a direct byte compare.
- In single-symbol `objdiff_tool.py show`, references to other functions of the same unit (`mov [esi+0x78], &Func`, `jmp Func`) are flagged; the unit report counts them as matched.
- The 1.55 debug build drops unreachable code after a `return` (except loop heads reached by a back edge): a jump in the line numbers means dead source.
- Two separate bool stores (`m_bThrowing = false; m_bThrown = false;`) merge into one word store, like the chained form.
- An unused `float speed = ...Length2D();` disappears entirely in 1.6a, sqrt included.
- Where 1.55 calls `?EMIT_SOUND_DYN` directly with `edict()`, write `EMIT_SOUND_DYN( edict(), ... )`; `STOP_SOUND` inlines to `EMIT_SOUND_DYN( ..., 0, 0, SND_STOP, PITCH_NORM )`. `strcpy( x, "" )` becomes one byte store and `strcmp` an inline loop.

## Patterns from the dual weapons

- One sound call, not two: when retail has a single `call dword ptr [eax+0x104]` (EarPosition) where 1.55 had a per-hand if/else of EMIT_SOUND_DYN, 1.6a used one `guess_UTIL_PlayWeaponSound` call. Two identical calls in an if/else compile to a hoisted `mov eax, [eax+0x104]; call eax` instead; when the branches also hold other statements (an ammo decrement), the duplicated form matches.
- gmsgKillWpnPart messages use `MSG_ONE_UNRELIABLE` (8).
- Empty helpers (CameraShoot, TMPShoot, SetBodyForWeapons) lose their calls in retail, so their switch cases merge with the default.
- CDualWeaponRight members are 4 bytes higher in 1.6a than in 1.55, like CBasePlayerWeapon: never take offsets from 1.55.
- An `if ( x )` with no body and code only in the else (lines commented out in 1.55) compiles the same as `if ( !x )`.
- Functions missing from a unit list are not checked at all: compare every function you write that the check does not list against its retail address (`tools/bytematch.py`).

## Patterns from the AI sweep

- Float `fabs`: monsters.cpp and AI_BaseNPC_Schedule.cpp use the float overload (`andps` mask, float compare), func_break.cpp and nodes.cpp the double one. In 1.55 those files called `?fabs@@YAMM@Z`; with MSVC 19.27 the float overload comes from `<cmath>`, so those files include it after the project headers.
- `xor eax, eax; je` inside a 1.55 condition is a literal `&& 0`: 1.6a keeps the calls before it (`IsMultiplayer()`) and drops the body.
- When objdiff shows `.dword $LN` inserts, a truncated retail function or a function paired with the wrong retail name, compare the raw bytes with relocations masked before changing source: these were all retail-side problems (fixed in the delinker, the Ghidra function bodies or the name map).

## Patterns from the second monster group

- Monster size: `int guess_iScale = CVAR_GET_FLOAT( "cof_monsterhumanscale" ); UTIL_SetSize( pev, Vector( -guess_iScale, -guess_iScale, 0 ), Vector( guess_iScale, guess_iScale, H ) );`; for the taller the doubling is in the declaration (`(int)CVAR_GET_FLOAT( ... ) * 2`). Yaw speed: `int ys; ys = CVAR_GET_FLOAT( "cof_monsterhumanyawspeed" );`.
- A 1.55 `if ( X )` followed by a `return` line whose false jump goes to the next statement is an if whose call was commented out: the return became the body.
- A call to a same-file function like `IsHeadless()` stays when its result is tested and is dropped when unused.
- A 1.55 `jmp` after an if/else plus a retail fall-through into the next case means a missing `break`; merged identical cases are still written out separately.
- Use `EXPORT` on the think and touch functions retail exports (`SawTouch`, `FakeDeath`, `ActivateSewmo`, `ActivateSpitter`, the `InactiveThink`s).
- `PRECACHE_SOUND( int )` compiles to a call to a helper (`STRING()` then `PRECACHE_SOUND`); `FClassnameIs( pEnt->pev, ... )` inlines as a strcmp on `pev->classname`.

## Patterns from the world entities

- A 1.55 `jmp` back to the loop increment is `continue`, not `break`.
- `CBaseAnimating::HandleAnimEvent` is an empty inline, so calls to it vanish; a switch with only `default:` that calls `CBaseMonster::HandleAnimEvent` compiles to a tail jump.
- If the retail push order looks swapped against 1.55, check which parameter is which (the first push is the last parameter), as in `GetDescriptionFromItem( "3dmdl", STRING( pev->message ) )`.
- A class that grew between builds shows in the retail save table count and the member offsets: `CCoFLadderManager::m_cAnimation` is `int [33]` in 1.6a (32 in 1.55).

## What not to do

- Do not edit shared headers (`cbase.h`, `weapons.h`, `player.h`, `util.h`, ...) without coordinating: several files depend on them.
- Do not rename retail symbols to make a function match. Retail names come from evidence (older builds, strings, vtables, exports) and are committed separately with the evidence (`tools/symbols/hl_manual.csv`).
