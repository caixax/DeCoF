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
| `CSwitchblade::PrimaryAttack`, `RageAttack` (CoF_Switchblade.cpp) | In the first stance, case 2 plays `P1ATTACK2`, so `P1ATTACK3` is never used; in the second stance, PrimaryAttack case 2 plays `P2ATTACK2` instead of `P2ATTACK3`. | the pushed sequence numbers |
| `CSwitchblade::ItemPostFrame` (CoF_Switchblade.cpp) | The sprint-from block times both stances with `SWITCHBLADE_P1_SPRINT_IDLE` instead of the sprint-from sequence, and the knife-stance jump only reads the length of `P2_SPRINT_TO` without playing it. | constant sequence pushed to GetSequenceTime, no SendWeaponAnim |
| `CSwitchblade::Deploy` (CoF_Switchblade.cpp) | The draw sounds only play when `pev->max_health != 1`. | the test before the sound calls |
| `CNightstick` attacks (CoF_NightStick.cpp) | All three attacks set `MeleeHitThinkNightStick2` as the think. | the same function pointer stored in each |
| `CAxe::Fidget` (CoF_Axe.cpp) | Calls `RANDOM_LONG( 0, 599 )` and ignores the result. | the call with no use of eax |
| `CSledgeHammer` (CoF_Sledgehammer.cpp) | Checks no suicide state, unlike the other melee weapons. | no m_iSuicideState test |
| `CTwitcher4` (CoF_Twitcher4.cpp) | Uses the Twitcher 1 skill values (`twitcherHealth`, `twitcherDmgOneSlash`, `twitcherDmgBothSlash`); the `twitcher4*` members are never read. Same in 1.55. | the gSkillData offsets loaded |
| `CFaceless`, `CFacelessV` (CoF_Faceless.cpp, CoF_FacelessValve.cpp) | Use `zombieHealth` and the zombie slash damages; the `faceless*` members are never read. Same in 1.55. | the gSkillData offsets loaded |
| `CFaster::TraceAttack` (CoF_Faster.cpp) | No `break` after `case HITGROUP_HEAD`: it falls into case 8, so a head hit makes sparks and plays `HeadHitSound` twice. | no jmp between the two case bodies |
| `SpawnHeadGibs` (CZombie in CoF_Slower.cpp, CSlower3) | Loops `i` from 0 to 4 while the switch covers 0 to 3; for `i == 4`, `vecVol` is uninitialized (in retail it keeps the previous gib's velocity). | loop bound 5, four case bodies |
| `CFaster::Precache` (CoF_Faster.cpp) | Precaches `pDeathSounds` twice. | two identical precache loops |
| `CZombie::TraceAttack` (CoF_Slower.cpp) | Calls `IsHeadless()` and ignores the result (retail drops the call). | 1.55 call with no use of the result |
| `CZombie::Spawn` (CoF_Slower.cpp) | Uneven hull: the minimum is `-scale, -scale, 0`, the maximum the fixed `8, 8, 72`. | the UTIL_SetSize arguments |
| `PM_CheckParamters` (pm_shared.c) | `inwheelchair` is set to 0 or 1 and then tested with `!= 2`, so the test is always true. | the compare constant |
| `PM_PlayStepSound` (pm_shared.c) | Twig snaps use `RandomLong( 0, 8 )` with cases 0 and 2 to 9: case 1 plays nothing and twig_snap9 never plays; the volume is `RandomLong( 0, 0 ) + fvol`; `roll` is set to 0 in both branches and then doubled. | the switch range and the arguments |
| `PM_DropPunchAngle` (pm_shared.c) | The `damping = 3` assignments are always overwritten. | dead stores in 1.55 |
| `PM_UnDuckNew` (pm_shared.c) | Sets `bInDuck = true` right before testing `!bInDuck`; nothing calls it. | the store before the test |
| `PM_WalkMove` (pm_shared.c) | The low stance multiplies wishspeed by 0.75 a second time when `iStepLeft` is set. | two multiplies by 0.75 |
| `PM_PlayWaterSounds` (pm_shared.c) | Empty in 1.6a (and without code in 1.55), so entering or leaving water plays no wade sound, although PM_PlayerMove still calls it. | retail body is `ret 0` |
| `CMobile::FlashOff` (CoF_MobilePhone.cpp) | Uses `\|=` inside `if ( flags & bit )`, so it never clears the flashlight flags. | `or` where an `and not` was meant |
| `CSyringe::SecondaryAttack` (CoF_Syringe.cpp) | The multiplayer loop calls `CBaseEntity::Instance( tr.pHit )` (the outer trace) instead of `trace.pHit`; `vecEnd` uses `gpGlobals->v_forward` before `UTIL_MakeVectors`; `UTIL_MakeVectors( angles )` is overwritten right away by `UTIL_MakeVectors( v_angle )`; an unreachable CPR revive block follows the final return (1.55 lines 436 to 543, no code). | operands of the calls; the 1.55 line gap |
| `CMobile::MessageGet` (CoF_MobilePhone.cpp) | `line = curLine;` assigns an int to a std::string as one character; `if ( sLineText && ... )` tests an array address (always true); `DestinationText` is never initialized, so garbage is sent when no line matches. | the calls and tests in 1.55 and 1.6a |
| `CCameraSpot` (bmodels.cpp and CoF_Camera.cpp) | Two different classes with the same name (one definition rule violation); the linker keeps the bmodels `GetClassPtr`, so `camera_spot` entities get the bmodels class. | both classes in the 1.55 PDB modules |
| `CFlare::FlareThink` (CoF_Flare.cpp) | Sets effects bit 2 (`EF_MUZZLEFLASH`); `EF_BRIGHTLIGHT` (4) was probably intended. | the or'ed constant |
| `CDualWeapon::Reload` (CoF_DualPistol.cpp) | Returns unless the game path contains "editor321@planethalflife.com", so dual weapon reload is effectively disabled; each hand's reload also requires a switchblade (id 6) in the other hand. | the strstr test on GetCurrentPath |
| `CDualWeapon::Deploy` (CoF_DualPistol.cpp) | The mobile phone case sets `gDualWeapon.m_fIdleAnimation1Time = 6.66` twice; `m_fIdleAnimation2Time` is never set. | two stores to the same member |
| `VP70Shoot` (CoF_DualPistol.cpp) | Throttles the right hand's empty click with the left hand's timer (`gDualWeapon.m_fEmptyClickTime`). | the member read |
| `MessageGet` (CoF_DualPistol.cpp) | Same bug as `CMobile::MessageGet`: tests the unused array `sLineText` (always true) and can send an uninitialized `DestinationText`. | the test and the write |
| `GetSprintFrom`, `GetSprintTo`, `GetSprintIdle` (CoF_DualPistol.cpp) | Never called: the sprint code in `CDualWeapon::ItemPostFrame` is commented out, but retail still contains them. | no callers in 1.6a |
| `CBaseMonster::GetPointsForKill` (monsters.cpp) | Adds to `pKiller->playerstats.suvivalpoints` before the `if ( pKiller && pKiller->IsPlayer() )` null check. | the store before the test |
| `CBaseMonster::MonsterThink` (monsters.cpp) | The second `m_bFrozenByCamera` branch leaves `flInterval` uninitialized (dead code: the first check already returns). | 1.55 statement order |
| `CMonsterMaker::MakeMonster` (monstermaker.cpp) | Calls `pMonst->Spawn()` a second time after DispatchSpawn for any classname containing "monster_". | the extra virtual call |
| `CBaseMonster::Look`, Book Simon block (monsters.cpp) | Sets `m_hEnemy = pPlayer` twice. | two EHANDLE assignments |
| `CSawRunner` sounds (CoF_SawRunner.cpp) | `pAttackHitSounds` holds `chainsaw_attack_miss.wav` and `pAttackMissSounds` holds `chainsaw_attack_hit.wav`. Same in 1.55. | the string tables |
| `CSawCrazy::MonsterThink` (CoF_SawCrazy.cpp) | After a saw hit it sets `fuser1` (the idle sound timer) instead of `fuser2`; `fuser2` never advances, so the saw does 200 damage on every think. | the stored entvars field |
| `CMonsterBaby::Killed`, `FakeDeath` (CoF_MonsterBaby.cpp) | With `CallGibMonster()` commented out, `if ( ShouldGibMonster( iGib ) ) return;` makes the return the if body, so an already dead baby that should not gib runs the death code again; in FakeDeath the `else if ( pev->flags & FL_MONSTER )` hangs off `if ( pOwner )`, so `BecomeDead()` is skipped when there is an owner. | 1.55 line layout |
| `CTaller` punch (CoF_Taller.cpp) | The swing sound plays on every punch, hits included (no `else`). | no jump over the sound call |
| `CMonsterCustom::m_SaveData` (CoF_MonsterCustom.cpp) | Lists `attackhitwallsound1` twice and never saves `attackhitwallsound2`. | the retail save table |
| `CSuicider::HeadExplode` (CoF_Suicider.cpp) | `vecSrc` is never set (its GetAttachment is commented out), so the damage sphere is centred on an uninitialized vector. | no store before the use |
| `CMonsterCrazyBitch::MonsterThink` (CoF_MonsterCrazyBitch.cpp) | No `break` after the rifle case (falls into the shotgun case) nor after the TMP case (falls into the MP5 and VP70 cases). | case bodies run into each other |
| Zombie-derived monsters (several CoF_*.cpp) | Keep the HLSDK test `(m_Activity == ACT_MELEE_ATTACK1) \|\| (m_Activity == ACT_MELEE_ATTACK1)`. | inherited from zombie.cpp |
| `CCrab` (CoF_Crab.cpp) | Uses the krypande skill values and the slower sounds (possibly intended). | the gSkillData offsets and strings |
| `CCoFLadderManager::KeyValue` (CoF_Ladder.cpp) | `animation13` does not increment `m_iNumAnimations`. | the missing increment in that case |
| `CCoFLadderManager::StartNextAnimation` (CoF_Ladder.cpp) | Inside `if ( m_bDescending )` it also tests `!m_bDescending`, so that branch never runs. | the nested test |
| `CCoFLadderManager::StartPreviousAnimation` (CoF_Ladder.cpp) | Overwrites `m_cAnimation[]` entries for good and sets `m_bEnableControl = true` even when no delay was found. | stores in both paths |
| `CCoFLadderManagerUse::Use` (CoF_Ladder.cpp) | The use sound plays on the activator, not on the entity. | the edict passed to the sound call |
| `CKeyitemModel::Spawn`, `KeyitemUse` (CoF_KeyItem.cpp) | A key with no item file is removed but the function carries on; keys with spawnflag 1 are removed when the difficulty is not 4; on difficulty 4 KeyitemUse sets the player's `m_iTapes = 5`. | 1.55 statement order |
| `CWheelchair::TakeDamage`, `HandleAnimEvent` (CoF_WheelchairPlayer.cpp) | `flDamage = pev->health - 1` is never used; HandleAnimEvent calls the base handler twice for default events. | dead store, two calls |
| `CPushableVan` (CoF_PushableVan.cpp) | Passes an unused argument to the "Max nerds are grabbing" message. | the extra push |
| `CInterDoorCoop::ShiftMonsters`, `Use` (CoF_InterDoorCoop.cpp) | ShiftMonsters calls `pTarget->MyMonsterPointer()` instead of using the entity it found; Use checks the "useunlock" door against the classname "inter_door" (not "inter_door_coop") and then casts it to CInterDoorCoop. | the call and the string |
| `CInterDoor::EndSequence` (CoF_InterDoor.cpp) | Both branches of the `m_iMusicFadeIn` test call `PlayerSetMP3( ..., 0 )`, so the fade-in setting is never used. | identical calls |
| `CNumeralPlateLeft::Think` (CoF_NumeralPlate.cpp) | `scriptleft`, `scriptright` and `codenumber` are uninitialized when the skin is not 1 to 3. | no store on the default path |
| `CCofTelephone::TelephoneRight`, `TelephoneWrong` (CoF_Telephone.cpp) | Use `m_pPlayer->pev` before checking `m_pPlayer`. | the test after the use |
| `CStatuePuzzleComplete::Use`, `CheckStatuePuzzle` (CoF_Statues.cpp) | No NULL test of `pActivator`; CheckStatuePuzzle may pass a NULL player. | no test |
| `CTapeRecorder::BlinkThink` (CoF_TapeRecorder.cpp) | Checks `pSprite` for NULL only after using it. | the test after the use |
| `CKeypadNew` (CoF_3DKeypad.cpp) | Reads the keyboard with `GetAsyncKeyState` in the server DLL. | the import call |
| `CRoofBoss` (CoF_RoofBoss.cpp) | `pList[32]` is passed with `listMax` 96 (stack overflow risk); `if ( !m_bHasAttacked ) m_bHasAttacked = false;` and the same for `m_bStartingAttack` do nothing; `if ( !pEdict ) continue;` comes after `pEdict->free` was read; `pHead` is used without a null check. | the pushed list size and the dead stores |
| `CBookSimon::SetActivity` (CoF_SickSimon.cpp) | Returns when `frags != 0` and then tests `frags != 0` again: dead code. | the second test |
| `CDoctorBoss::DecideWhatToDo`, `MonsterThink`; `CDoctorHelper::Killed` (CoF_DoctorBoss.cpp) | Case 0 falls through into case 1 and then case 2; the `m_iGunType` switch has both gun branches inside each case; CDoctorHelper::Killed looks the doctor up and does nothing with it. | the case bodies and the unused result |
| `CRuben::GetDamageAmount` (monster_ruben.cpp) | Returns a fixed 5 instead of a skill value. | the constant |
| Sewer boss, chainsaw boss (CoF_SewerBoss.cpp, CoF_ChainsawBoss.cpp) | `IgnoreConditions` tests `ACT_MELEE_ATTACK1` twice (from the HLSDK zombie). | inherited |
| `UTIL_SplashCalculation` (util.cpp) | Passes an uninitialized `Vector Position` to WaterSplash, which ignores that parameter. | the uninitialized local |
| `UTIL_WallBetween` (util.cpp) | Calls Classify twice and returns TRUE on both branches; the null check on pEntity comes after the pointer is used. | both returns |
| `UTIL_BrushEntInSight` (util.cpp) | `!pHit || pHit->IsBSPModel()` style test reads `pHit->pev` when pHit is NULL, pEntity can be NULL at FClassnameIs, and it passes `pev->mins/maxs` (local bounds, not absmin/absmax) to UTIL_EntitiesInBox. | the order of the tests and the bounds |
| `UTIL_Remove` (util.cpp) | Sets `m_bIsBoss = false` twice. | the second store |
| `guess_UTIL_PlayWeaponSound` (util.cpp) | Always sends entity index 1 in singleplayer. | the constant |
