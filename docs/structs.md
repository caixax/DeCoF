# Struct and class layouts that differ from the HLSDK

Only layouts confirmed in the binary go here. Each entry names the binary,
the evidence and the method.

## entvars_t (hl.dll 1.6a): 4 bytes inserted after light_level

Confirmed 2026-10-10 from the save/restore table `gEntvarsDescription`
(102 TYPEDESCRIPTION records of 16 bytes: fieldType, fieldName,
fieldOffset, fieldSize, flags).

| Field | HLSDK / CoF 1.55 offset | CoF 1.6a offset |
|---|---|---|
| gravity | 0x11C | 0x11C |
| light_level | 0x124 | 0x124 |
| *(unnamed, 4 bytes, not saved)* | - | 0x128 |
| sequence | 0x128 | 0x12C |
| frame | 0x130 | 0x134 |
| health | 0x160 | 0x164 |
| flags | 0x1A4 | 0x1A8 |
| iuser1 | 0x244 | 0x248 |
| euser4 (last saved field) | 0x2A0 | 0x2A4 |
| sizeof(entvars_t) | 0x2A4 | 0x2A8 |

- 1.55 table: VA 0x10216B40 (PDB symbol `gEntvarsDescription`), offsets
  identical to the stock HLSDK. The 2012 builds (and so the PDB type
  information) use the stock layout.
- 1.6a table: VA 0x1021AAE0 (file offset 0x219AE0). Every field from
  `sequence` on is shifted by +4.
- The new field is absent from the save table, so its name and type are
  unknown. Placeholder name until evidence appears: `guess_light_level_pad`.
- Consequence: `edict_t` grows to at least 0x328 in 1.6a. cof-enhanced
  reports an edict stride of 0x32C, which would mean a second 4-byte change
  elsewhere in `edict_t`; not yet verified.

## Engine interface tables (1.6a)

Confirmed 2026-10-10 from the copy loops in the exported entry points
(`rep movsd` counts) and the member lists of the headers in `src/`.

| Table | Size in 1.6a | Members | Matches | Evidence |
|---|---|---|---|---|
| `enginefuncs_t` (hl.dll `g_engfuncs` 0x10222F48) | 0x240 | 144 | Paranoia / pre-Steam HLSDK; the Steam-era HLSDK has 159 | `GiveFnptrsToDll` copies 0x90 dwords |
| `DLL_FUNCTIONS` (hl.dll `gFunctionTable` 0x102119F8) | 0xC8 | 50 | HLSDK, `INTERFACE_VERSION` 140 | `GetEntityAPI` copies 0x32 dwords |
| `NEW_DLL_FUNCTIONS` | not exported | - | - | no `GetNewDLLFunctions` export |
| `cl_enginefunc_t` (client.dll `gEngfuncs` 0x101B2608) | 0x194 | 101 | Paranoia, `CLDLL_INTERFACE_VERSION` 7 | `Initialize` copies 0x65 dwords |

## triangleapi_t (client.dll 1.6a): two Steam-era members

client.dll calls `gEngfuncs.pTriAPI` slots 18 and 19 (offsets 0x48 and
0x4C). Paranoia's `common/triangleapi.h` (copied into `src/`) ends before
them; the Steam-era HLSDK names them `Color4fRendermode` and `FogParams`.
`src/common/triangleapi.h` needs both members appended once client code is
reconstructed. Evidence: `docs/calls/client_1.6a_engfuncs.csv`.

## CBaseEntity (hl.dll 1.6a): 4 bytes larger

Confirmed 2026-10-10 (Phase 1). Evidence:

1. **Allocation sizes** of all 468 entity classes present in both builds
   (`docs/class_sizes.csv`, `tools/class_sizes.py`): 462 classes are exactly
   4 bytes larger in 1.6a. The size is the constant passed to
   `pfnPvAllocEntPrivateData` by `GetClassPtr<T>` (inlined in each
   `LINK_ENTITY_TO_CLASS` export in 1.6a; pushed for
   `CBaseEntity::operator new` in the 1.55 debug build).
2. **Save tables**: every field of `CBaseEntity::m_SaveData` (offsets 0x08
   to 0x88, from `b_Saving` to `m_bFadeOut`) has the same offset in both
   builds, while every field of `CBaseDelay`, `CBaseAnimating`,
   `CBaseToggle`, `CBaseMonster`, `CBasePlayerItem` and
   `CBasePlayerWeapon` is shifted by +4 (`CBaseDelay::m_flDelay`
   0xD8 -> 0xDC).
3. `CBasePlayer::TabulateAmmo` (1.6a 0x100F5EB0) still writes `ammo_9mm`
   ... `ammo_groza` at 0x8C ... 0xC0, the 1.55 offsets.

So `sizeof(CBaseEntity)` is 0xD8 in 1.55 (PDB) and 0xDC in 1.6a, and the
new 4 bytes lie between 0xC4 and the end of the class: before or among
`m_flStartThrow` (0xC4), `m_flReleaseThrow` (0xC8), `m_chargeReady`
(0xCC), `m_fInAttack` (0xD0), `m_fireState` (0xD4), or at the end. The
exact position is open until a 1.6a function that touches those fields is
decompiled. MadSimonX puts an `int __PADDINGD4` at the end (unverified).
Placeholder name: `guess_cbaseentity_pad`.

1.55 layout (PDB, Ghidra `get_struct_layout`): vtable 0x00, `pev` 0x04,
`b_Saving` 0x08, `m_bFrozenByCamera` 0x09, `m_bIsBoss` 0x0A (CoF), Spirit
MoveWith / Assist / think fields 0x0C-0x77, `m_pfnThink`..`m_pfnBlocked`
0x78-0x87, `m_bFadeOut` 0x88 (CoF), `ammo_9mm`..`ammo_hornets`,
`ammo_argrens` (HLSDK) and `ammo_aps`, `ammo_barret`, `ammo_ak`,
`ammo_asval`, `ammo_rpk`, `ammo_groza` (Paranoia) 0x8C-0xC3, then the
HLSDK weapon state fields 0xC4-0xD7.

## Classes that grew more than CBaseEntity (1.55 -> 1.6a)

| Class (classname) | 1.55 | 1.6a | Delta | Own new data |
|---|---|---|---|---|
| CBasePlayer (`player`) | 0x26C0 | 0x281C | +348 | 344 bytes |
| CPhoneCall (`cof_phonecall`) | 0x1F8 | 0x204 | +12 | 8 bytes |
| CAnimatedCameraModel (`camera_model`) | 0x3EC | 0x3F4 | +8 | 4 bytes |
| CCoFMDLCutscene (`cof_mdlcutscene`) | 0x3EC | 0x3F4 | +8 | 4 bytes |
| CCutsceneModel (`cutscene_model`) | 0x188 | 0x190 | +8 | 4 bytes |
| CCoFLadderManager (`cof_ladder_manager`) | 0x1AC | 0x1B4 | +8 | 4 bytes |

New classes in 1.6a: `CMp5` (`weapon_mp5`, 0x1FC), `CMP5Ammo` (`ammo_mp5`,
0xE0), `CBowlingBall` (`bowling_ball`, 0x100), `CMP5ModeSaveRestoreMP5`
(`cof_mp5modesaverestore`, 0xDC). MadSimonX documents three "1.6a" padding
blocks in CBasePlayer totalling 316 bytes; to be checked against the 344
measured here.

## Source of class layouts for Phase 2

- 1.55 PDB (Ghidra program `/1.55/hl.dll`): full layouts of every 2012
  class, member names included. Apply the 1.6a deltas above on top.
- 1.6a vtables (RTTI): 482 game classes keep the 1.55 vtable length, so no
  virtual functions were added or removed in those classes.
