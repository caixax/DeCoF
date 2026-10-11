# Original bugs and oddities kept in the source

The reconstructed source keeps the behavior of the retail 1.6a binaries, including their bugs (see the conventions in README.md). This file lists what was found, so nobody "fixes" it by accident.

## hl.dll

| Where | What retail does | Evidence |
|---|---|---|
| `CShotgunAmmo::AddAmmo` (CoF_Shotgun.cpp) | When `pev->message` is set, it is passed as the ammo amount to `GiveAmmo`. `message` is a `string_t` (an offset into the string pool), so a mapper-set message gives a huge amount. | retail reads entvars +0x1d8 (`message` in 1.6a, +0x1d4 in 1.55) into the amount argument |
| `CFAMAS::PrimaryAttack`, `CFAMAS::SuicideShot` (CoF_FAMAS.cpp) | Play `weapons_new/famas/famas_shoot.wav`, while `Precache` loads `weapons/famas/famas_shoot.wav`. The played sound is never precached by the weapon. | string literals in the three functions |
| `CTmp::PrimaryAttack` (CoF_TMP.cpp) | Calls `g_pGameRules->IsMultiplayer()` for the looping fire sound without the usual null test of `g_pGameRules`. | no `test ecx, ecx` before the virtual call |
| `CBasePlayerItem::m_SaveData` (weapons.cpp) | Lists `m_iSprintIdleAnim1` twice (once as `FIELD_INTEGER`, once as `FIELD_TIME`); `m_iSprintIdleAnim2` is never saved. | both table entries point at the same offset in the retail save table |
| `CHalfLifeMultiplay::ClientCommand`, "vote" (multiplay_gamerules.cpp) | No `break` after a vote is stored: one vote fills every empty `pVoted` slot and adds to the yes or no count once per empty slot, up to 32 times. | the store loop in 1.55 and 1.6a has no exit after the first free slot |
| `CHalfLifeMultiplay::ClientDisconnected` (multiplay_gamerules.cpp) | `pSurvivalEntity` is always NULL after its `while` loop, so the survival mode branch never runs and players always get the game over path. | the loop runs until the search returns NULL, then the branch tests the same pointer |
| `CGameRules::StartVoteKick` (gamerules.cpp) | If `GETPLAYERAUTHID` returns NULL, `m_bInVoteKick` is still set while the victim id stays NULL; `VoteKickThink` never finishes that vote and no new vote kick can start. | no NULL test before the flag is set |
| `CHalfLifeMultiplay::Think`, lobby (multiplay_gamerules.cpp) | `m_flNextLobbyUpdateTime` is set inside the per-player loop, so each second only the first player's ready state is sent. | the store is inside the loop body |
| `CGameRules::RefreshSkillData` (gamerules.cpp) | Reads the monster and player damage adjustment cvars twice (harmless). | the cvar block appears twice in 1.6a |
