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
| `CCoFPlayerCheck::Use` (effects.cpp) | Case 3 tests `player1trigger` but fires `player3trigger`. | the field name |
| `CCoFStats::Use` (effects.cpp) | Normal mode: a score of 9 to 12 gives grade 0. Doctor mode with unlocks: `unlockflags = 0` wipes the earlier flags before setting 0x1000000. | the grade table and the store |
| `CCoFRandomTimedSpawner::CheckIfDead` (effects.cpp) | In the `iuser4` branch the delay from `iuser1` is overwritten by the final `SetNextThink( 0.25 )` (no `return`). | the missing return |
| `CBasePlayer::PostThink` (player.cpp) | In multiplayer, `pev->frags = m_iFlashlightFlags;` overwrites the score with the flashlight flags, which ScoreInfo then sends. | the store |
| `CBasePlayer::PostThink` (player.cpp) | The punch logic tests `frags != 4` but sets `frags = 10`, and tests `!= 5` but sets `11` (1.55 set 4 and 5; the tests were not updated). | the constants |
| `CBasePlayer::PostThink` (player.cpp) | The first CDualWeapon block uses `pDualWeapon` without a null check (the second checks it); dead leftovers `dx`/`dy`, an unused `pCameraTarget`, and `pev->flags |= FL_ONGROUND` in a branch that already requires it. | as written |
| `GetEyePosition` (animation.cpp) | `vecEyePosition = pstudiohdr->eyeposition;` assigns the pointer parameter (Vector converts to float*) instead of copying the eye position, so the caller's vector is never written. | the assignment |
| Save/Restore of CCheckBrush, CFuncValve, CCoFLadderManagerUse, CStatueUse, CCoFCoopAllPlayersButton, CAnimatedCameraModel, CPasswordNote, CCofInterDoorOnOff, CCoFMDLCutscene | `IMPLEMENT_SAVERESTORE( CClass, CBaseEntity )` although the classes derive from CBaseToggle, CBaseAnimating or CBaseMonster: the fields of the base classes in between are not saved (confirmed in the 1.55 debug build). | the macro base |
| CMp5 (guess_CoF_MP5.cpp) | Dry fire in iron sights sends sequence 20, which v_mp5.mdl lacks (0 to 19); Precache and Deploy use the TMP player model; Reload and Spawn keep TMP values (3 s reloads, ammo "mp5" with 300 default); WeaponIdle sends no idle animation in iron sights; CMP5ModeSaveRestoreMP5::ObjectCaps calls m_pMoveWith->ObjectCaps() and drops the result. | as written |
| CHud notes (client hud.cpp) | AddNote and the two note loops use indexes 1 to 16 on a 16-entry array: note 16 overwrites the members after the array (iron sights, NewFade, clothes). | the index range |
| CHud::Init and ResetHUD (client) | Clear 400 bytes from the FPS sample array, which also clears the sample count stored after it. | the size |
| CHUDControl (client guess_HUDControl.cpp) | The constructor reads members not set yet (+0xE70, +0x1014, +0x85C/+0x860); the hint bar is placed at ScreenHeight/2 - 400 while ControlHint later uses ScreenWidth; ParseHintTokens calls COM_FreeFile(NULL) so hint files leak; saytext labels are filled with the varargs setText (chat text used as a format string); a finished hint sets the label visible instead of hidden; the message fade uses strlen(m_szChapter) and the chapter block tests the message hold flag; several handlers load images or create panels on every message without freeing; Initialize calls setFgColor on the timer label before its null check. | as written |
| CHudDeathNotice::MsgFunc_DeathMsg (client death.cpp) | Reads the killer byte and ignores it. | as written |
| EV_ClientSound (client ev_hldm.cpp, guess_EV_ClientSound) | Case 21 (revolver) has no break and falls into case 11 (rifle): the revolver sound plays, then a rifle sound with the same iparam2. | the missing break |
| V_DropPunchAngle (client view.cpp) | The new spring branch runs only when GetMaxClients() > 1. | as written |
| ServersCancel (client hud_servers.cpp) | Calls g_pServers->RequestList() instead of CancelRequest(), so cancelling the server list requests it again (RequestList at 100745F0 is a byte match). | the call |
| CPuzzleBar2 to CPuzzleBar5 ::Use (CoF_PuzzleBars.cpp) | SetThink( &CPuzzleBar::BarCheck ) instead of the class's own BarCheck (copy-paste from CPuzzleBar): the bars run the first bar's check on their own entity. | the think function |
| CBowlingBall::BowlingUse (guess_CoF_BowlingBall.cpp) | Looks up the player with GetClassPtr and does nothing with it: using the ball has no effect. | as written |
| CComputer (client guess_Computer.cpp) | The password field shows "Invalid password or password!". | the string |
| CVGUIBillboard (client guess_VGUIBillboard.cpp) | paint passes the HUD message as the format string of Label::setText; the constructor loads 640_gustavdahlwelcome.tga twice and leaks the first copy. | as written |
| CDocuments (client guess_Documents.cpp) | Reads gHUD notes 1 to 16 of a 16-entry array (one past the end). | the index range |
| CGameOver::solve (client guess_GameOver.cpp) | Returns before Panel::solve when there is no local player. | as written |
| CAmmoInventory (client guess_AmmoInventory.cpp) | setVisible tests the pfnGetLevelName pointer, which is never null; the panel is never created, so the AmmoInv message does nothing. | as written |
| CBossBar (client guess_BossBar.cpp) | Every bar is placed at y = image height. | as written |
| CSave (client guess_Save.cpp) | The constructor's error text names 640_panelback.tga instead of 640_savegame.tga. | the string |
| guess_MsgYesNo (client guess_Yesno.cpp) | Calls vgui_LoadTGA twice and leaks the first image. | as written |
| CPhone (client guess_Phone.cpp) | setText( guess_m_szNumber, strlen( ... ) ) resolves to the variadic setText( fmt, ... ), so the dialled number is used as a format string. | as written |
| CKeypad (client guess_Keypad.cpp) | The destructor deletes 3 of its 5 images. | as written |
| CNoteDocument (client guess_NoteDocument.cpp) | guess_ClearString clears only the first character; "languages/norwegian/" plus the page name overflows the 16-byte text arrays; guess_SetPage uses an uninitialised image when the page is not 1 to 4; guess_PrevPage case 1 falls through and case 2 has an extra m_iPage != 1 test; paint calls drawSetTextPos but prints nothing. | as written |
| CRECBInventory (client guess_RECBInventory.cpp) | A successful combine resets guess_m_iDualWield instead of guess_m_iCombine; guess_IsCursorOver uses the width for both axes; solve allocates a new LineBorder every frame; Initialize clears each quick-slot weapon string before looking up its picture, so the picture is never loaded; guess_DoAction is never called. | as written |
| ScorePanel (client vgui_ScorePanel.cpp) | RebuildTeams: the inner for ( int j ... ) shadows the outer j, so the empty-slot search result is ignored; Update returns right after GetAllPlayersInfo, so the rest of the scoreboard code is dead. | as written |
| CGameMenu (client guess_GameMenu.cpp) | The constructor checks the loadgame.tga load result but sets the colour of the newgame picture; guess_ParseCampaign never frees the loaded file and computes w * 1.2 without using it; guess_LoadCampaigns never calls FindClose; every rollover loads a new TGA. | as written |
| CCredits (client guess_Credits.cpp) | Every language maps coconc.txt to that language's docconc.txt; the unlock label is placed at y = ScreenWidth / 2; the grade shadow label takes its alpha from the grade label; guess_ShowUnlock overwrites its distance argument with 50. | as written |
| CUnlockables (client guess_Unlockables.cpp) | guess_m_flNextClick (0xC4) is never written, so ActionSignal reads an uninitialised float. | as written |
| CClothesMenu (client guess_ClothesMenu.cpp) | A 13th button is created; clicking it prints "Unknown clothesmenu ActionSignal 13"; guess_SetCustomHoodie never frees the loaded file. | as written |
| CSaveLoad::guess_ReadSlot (client guess_SaveLoad.cpp) | The slot label gets the line left over after the read loop. | as written |
| CVGUIExtraMenu (client guess_VGUIExtraMenu.cpp) | Every rollover loads a new TGA and never frees it. | as written |
| guess_MsgCoopStats (client guess_CoopStats.cpp) | Always writes "Ending seen: 7" first. | as written |
| guess_SetSteamAppValue (client glmanager.cpp) | Opens HKCU\Software\Valve\Steam\Apps\70_Cry of Fear with RegCreateKeyEx and never closes the key. | as written |
| GLManager::VidInit (client glmanager.cpp) | The success message says "Succesfully". | the string |
| CTabPanel::LoadMessage (client vgui_tabpanel.cpp) | FontFromMessage advances only a copy of the text pointer, so the text and the title are shown with their @font prefix. | as written |
| CHud::MsgFunc_AttachCam (client hud_msg.cpp) | Reads a SHORT, two BYTEs and three LONGs while the server writes only a SHORT and a BYTE, so it reads past the end of the message; the LONGs land in a vector that render_origin overwrites later. | as written |
| CHud::MsgFunc_ViewLock (client hud_msg.cpp) | Reads a second COORD past the end of the message when the first COORD is not 666. | as written |
| ApplyCFGSettingToPlayer, ApplyCFGSettingToPlayerLate (hl multiplay_gamerules.cpp) | The quicksel1 to quicksel3 lines copy the player's m_sQuickSelN into the line buffer (strcpy in the wrong direction), so saved quick-select slots are never restored. | as written |
| ApplyCFGSettingToPlayer, ApplyCFGSettingToPlayerLate (hl multiplay_gamerules.cpp) | Each config line is copied into a 256-byte buffer with no length check. | as written |
| V_CalcNormalRefdef (client view.cpp) | The bob loop adds bob * 0.4 * forward to view->angles instead of view->origin. | as written |
| V_CalcNormalRefdef (client view.cpp) | The wheelchair block calls AngleVectors( viewmodel->angles, ... ) even when viewmodel is NULL, and never uses the results. | as written |
| V_CalcNormalRefdef (client view.cpp) | The maglite block tests tr->endpos (an array) for NULL; rendercolor / 255 is an integer division, so the color is 0 or 1; the dlight key is ent->index in one branch and view->index in the other; guess_iUnknown10542BC8 = 0 is stored twice. | as written |
