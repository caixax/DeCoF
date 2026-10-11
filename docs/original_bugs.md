# Original bugs and oddities kept in the source

The reconstructed source keeps the behavior of the retail 1.6a binaries, including their bugs (see the conventions in README.md). This file lists what was found, so nobody "fixes" it by accident.

## hl.dll

| Where | What retail does | Evidence |
|---|---|---|
| `CShotgunAmmo::AddAmmo` (CoF_Shotgun.cpp) | When `pev->message` is set, it is passed as the ammo amount to `GiveAmmo`. `message` is a `string_t` (an offset into the string pool), so a mapper-set message gives a huge amount. | retail reads entvars +0x1d8 (`message` in 1.6a, +0x1d4 in 1.55) into the amount argument |
| `CFAMAS::PrimaryAttack`, `CFAMAS::SuicideShot` (CoF_FAMAS.cpp) | Play `weapons_new/famas/famas_shoot.wav`, while `Precache` loads `weapons/famas/famas_shoot.wav`. The played sound is never precached by the weapon. | string literals in the three functions |
| `CTmp::PrimaryAttack` (CoF_TMP.cpp) | Calls `g_pGameRules->IsMultiplayer()` for the looping fire sound without the usual null test of `g_pGameRules`. | no `test ecx, ecx` before the virtual call |
| `CG43` damage in 1.55 | The 1.55 G43 took its bullet damage from `gSkillData.suitchargerCapacity`; 1.6a added its own skill member (`guess_DmgG43`, cvar `sk_cof_g43`). | 1.55 PDB `skilldata_t` has no G43 member and the 1.55 PrimaryAttack reads +0x144 |
| `CBasePlayerItem::m_SaveData` (weapons.cpp) | Lists `m_iSprintIdleAnim1` twice (once as `FIELD_INTEGER`, once as `FIELD_TIME`); `m_iSprintIdleAnim2` is never saved. | both table entries point at the same offset in the retail save table |
