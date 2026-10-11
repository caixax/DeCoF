# Writing a Cry of Fear source file for hl.dll

How the first files (`src/dlls/CoF_Glock.cpp`, `src/dlls/CoF_P345.cpp`)
were written and matched. Follow AGENTS.md for everything else (naming,
writing rules, no decompiler artifacts, guess_ prefix).

## Inputs for one unit

| What | Where |
|---|---|
| Functions of the unit (1.6a names) | `tools/units/hl_dlls_<file>.txt`, `docs/progress.csv` |
| Function order and line in the original file (1.55 debug build) | `docs/lines_hl_1.55.csv` (write the file in this order) |
| Ghidra pseudo C (reference only, never commit) | `build/decomp/hl/dlls/<file>/` (`python tools/build.py decomp hl/dlls/<file>`) |
| Exact instructions, strings, constants, member offsets of globals | `python tools/disasm.py hl "<decorated name>"` |
| Parameter and local names, block structure (1.55) | `python tools/pdbtypes.py versions/1.55/cl_dlls/hl.pdb func "CFoo::"` |
| Class declaration (members, methods, order) | `python tools/hdrgen.py CFoo` (weapon classes are already in `src/dlls/weapons.h`) |
| Member at an offset / vtable slot | `python tools/member.py --src src/dlls/items.cpp CBasePlayerWeapon 0x1e4`, `python tools/member.py --vt CBasePlayerWeapon 0x198` |
| Animation enum | `python tools/mdlseq.py versions/1.6a/cryoffear/models/weapons/<w>/v_<w>.mdl PREFIX` |

## Loop

1. Write the file (functions in the 1.55 line order, class `CFooAmmo`
   defined in the .cpp with in-class bodies like the HLSDK ammo classes).
2. `python tools/build.py check hl/dlls/<file>`: compiles the unit and
   prints the match of every function (one-unit objdiff report).
3. `python tools/objdiff_tool.py show hl/dlls/<file> "<decorated name>"`:
   retail on the left, ours on the right, differing rows marked `*`.
   Rows that differ only by a Ghidra name on the left (`s_...`, `DAT_...`,
   `FUN_...`) are naming, not code: they disappear after a harvest and a
   re-delink and need no source change.
4. For shape questions (if/else order, operand order, locals) write the
   alternatives into a variants file and run
   `python tools/try_variants.py hl/dlls/<file> "<decorated name>" <file> --keep`.
5. After 8 failed attempts on a function, stop: it stays TODO or becomes
   EQUIVALENT with the exact remaining difference in progress.csv.

## Facts that hold for the CoF weapons

- Globals: `gpGlobals->v_forward` is +0x28, `v_up` +0x34, `v_right` +0x40.
  Shell eject: `vecShellVelocity = gpGlobals->v_right * 220 + gpGlobals->v_up
  * 200 + gpGlobals->v_forward * 40`, `vecShellOrigin = pev->origin +
  pev->view_ofs + gpGlobals->v_forward * 80 + gpGlobals->v_right * 20`,
  `EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iShell,
  TE_BOUNCE_SHELL )`.
- Fire event: `UTIL_MakeVectors( v_angle ); Vector vecSrc =
  m_pPlayer->GetGunPosition( ); Vector vecShellEnd = vecSrc +
  gpGlobals->v_right * 32;` then `PLAYBACK_EVENT_FULL( flags, ...,
  m_usThirdFlash, 0.0, m_pPlayer->pev->origin, vecShellEnd, 0, 0, m_iId, 0,
  0, 0 )`; `int flags; if ( m_pPlayer->m_iViewValue != 1 ) flags =
  FEV_NOTHOST; else flags = 0;` comes before `UTIL_MakeVectors`.
- Bullets: `vecDir = m_pPlayer->FireBulletsPlayer( 1, vecShellSrc,
  vecAiming, m_AccuracyVector, 8192, BULLET_PLAYER_357, 2,
  gSkillData.Dmg<Weapon>, m_pPlayer->pev, m_pPlayer->random_seed )` (check
  the bullet type and the gSkillData member against the disassembly).
- Fire sound: `guess_UTIL_PlayWeaponSound( "weapons_new/<w>/<w>_fire.wav",
  0, true, m_pPlayer->EarPosition(), m_pPlayer, true )`.
- Timers that `ja` skip in retail (`comiss member, time; ja skip`) are
  written `!( member > gpGlobals->time )`; plain early returns are
  `if ( member > gpGlobals->time ) return;`.
- In ItemPostFrame, `m_iSprintState = 0;` is the last statement of the
  sprint/jump blocks (after the timers): MSVC hoists the store up to the
  last read of gpGlobals->time, so its retail position tells where it was.
- Store a GetSequenceTime result in the function scope `float time;`
  before using it (`time = GetSequenceTime( ... ); m_iNextSprintAnim = time
  + gpGlobals->time;`).
- The suicide punch sound: `{ int pitch = 90 + RANDOM_LONG( 0, 15 );
  EMIT_SOUND_DYN( ..., pitch ); }`.
- `pev->body` from the flashlight flag: `if ( m_pPlayer->m_iFlashlightFlags
  & 4 ) pev->body = 2; else pev->body = 1;` (not a ternary).
- Which branch comes first (`if ( m_iClip )` or `if ( !m_iClip )`) varies
  per weapon and per function: follow the retail jump (`je`/`jne`) and the
  push order.
- Ammo `AddAmmo`: two separate `GiveAmmo` calls in an if/else on
  `g_pGameRules && g_pGameRules->IsMultiplayer()`, each followed by the
  `EMIT_SOUND` and `return TRUE;`, then `return FALSE;`.
- `GiveAmmo` has a fourth argument (`false`); `SendWeaponAnim` defaults
  (`skiplocal = 1, body = 0`) are used everywhere.
- Ironsight toggle button is `IN_CANCEL`; suicide button mash tests
  `IN_ATTACK | IN_ATTACK2`; sprinting is `pev->flags & (1<<18)`.
- `IronsightsTo`/`IronsightsFrom` are the same in every weapon except the
  enum names (copy from `CoF_P345.cpp`).

## What not to do

- Do not edit shared headers (`cbase.h`, `weapons.h`, `player.h`,
  `util.h`, ...) without coordinating: several files depend on them.
- Do not rename retail symbols to make a function match. Retail names
  come from evidence (older builds, strings, vtables, exports) and are
  committed separately with the evidence (`tools/symbols/hl_manual.csv`).
