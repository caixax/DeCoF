/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// hud.cpp
//
// implementation of CHud class
//

//LRC - define to help track what calls are made on changelevel, save/restore, etc
#define ENGINE_DEBUG

#include <fstream>
#include <string>
#include <vector>

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "hud_servers.h"
#include "vgui_int.h"
#include "vgui_TeamFortressViewport.h"
#include "mp3.h"
#include "rain.h" // buz
#include "grass.h" // buz
#include "vgui_subtitles.h" // buz
#include "vgui_radio.h" // buz
#include "vgui_hud.h" // buz
#include "vgui_tabpanel.h" // buz

#include "demo.h"
#include "demo_api.h"
#include "vgui_scorepanel.h"
#include "triapiobjects.h"

#include "glmanager.h" // buz
#include "gl_renderer.h" // buz
#include "exception.h" // buz
#include "log.h"

//=========================================================
// Declarations this file takes from the Steamworks SDK
// (steam_api.h, isteamuser.h, steamclientpublic.h)
//=========================================================
typedef unsigned __int64 uint64;
typedef int HSteamUser;

class CSteamID
{
public:
	uint64 ConvertToUint64() const
	{
		return m_unAll64Bits;
	}

private:
	uint64 m_unAll64Bits;
};

class ISteamUser
{
public:
	virtual HSteamUser GetHSteamUser() = 0;
	virtual bool BLoggedOn() = 0;
	virtual CSteamID GetSteamID() = 0;
};

extern "C" __declspec( dllimport ) ISteamUser * __cdecl SteamUser();
//=========================================================

class CHLVoiceStatusHelper : public IVoiceStatusHelper
{
public:
	virtual void GetPlayerTextColor(int entindex, int color[3])
	{
		color[0] = color[1] = color[2] = 255;

		if( entindex >= 0 && entindex < sizeof(g_PlayerExtraInfo)/sizeof(g_PlayerExtraInfo[0]) )
		{
			int iTeam = g_PlayerExtraInfo[entindex].teamnumber;

			if ( iTeam < 0 )
			{
				iTeam = 0;
			}

			iTeam = iTeam % iNumberOfTeamColors;

			color[0] = iTeamColors[iTeam][0];
			color[1] = iTeamColors[iTeam][1];
			color[2] = iTeamColors[iTeam][2];
		}
	}

	virtual void UpdateCursorState()
	{
		gViewPort->UpdateCursorState();
	}

	virtual int	GetAckIconHeight()
	{
		return ScreenHeight - gHUD.m_iFontHeight*3 - 6;
	}

	virtual bool			CanShowSpeakerLabels()
	{
		if( gViewPort && gViewPort->m_pScoreBoard )
			return !gViewPort->m_pScoreBoard->isVisible();
		else
			return false;
	}
};
int guess_iDefaultFOV = 90;
int guess_iSkipSceneFramerate = 2;	// host_framerate while a scene is skipped (SkipScene message, CHud::Redraw)
static CHLVoiceStatusHelper g_VoiceStatusHelper;


extern client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount);

extern cvar_t *sensitivity;

int guess_iMenuMode;			// 1 for the game menu map, 2 after the to3dmenu command
bool guess_bXboxFix;			// cleared by the xboxfix command
int guess_iHudActive;			// set by Init and VidInit, cleared by the destructor
cvar_t *cl_lw = NULL;
int guess_iGoro;				// "goro" cvar
float guess_flWeaponIdleSpeed;	// "cl_weapon_idle_speed" cvar
float guess_flWeaponIdleDampener;	// "cl_weapon_idle_dampener" cvar

extern int guess_iDonator;
extern cl_entity_t *guess_pAttachCamEnt;	// the entity the view is attached to (AttachCam message)
extern int guess_iBoat;
extern int guess_iWheelchair;
extern bool guess_bWheelchairLock;
extern cl_entity_t *guess_pTelescopeEnt;
extern cl_entity_t *guess_pCamTargetEnt;
extern vec3_t guess_vecIronsightOfs;
extern bool guess_bCustomFingerTexture;	// CustomFng message
extern bool guess_bCustomGloveTexture;	// CustomGlv message
extern int guess_iGreyFade;				// GreyFade message
extern float guess_flNextSoundCacheTime;	// "ambience/soundcache" in CHud::Redraw
extern bool guess_bUnknown105431E9;
extern bool guess_bUnknown10545081;
extern float guess_flUnknown105595A0;
extern float guess_flUnknown105595A4;
extern float guess_flUnknown105595A8;
extern int guess_iUnknown105595AC;
extern int guess_iUnknown1055959C;

int guess_IsInDonatorGroup1( void );	// Steam clan 0x29D6CA
int guess_IsInDonatorGroup2( void );	// Steam clan 0x46AE0A

void guess_IrrKlangInit( void );
void guess_IrrKlangSetParams( float flParm1, float flParm2, float flParm3, float flParm4 );
void guess_IrrKlangStop( float flFadeTime );

void ShutdownInput (void);

//DECLARE_MESSAGE(m_Logo, Logo)
int __MsgFunc_Logo(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Logo(pszName, iSize, pbuf );
}

//DECLARE_MESSAGE(m_Logo, Logo)
//LRC
int __MsgFunc_HUDColor(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_HUDColor(pszName, iSize, pbuf );
}

//LRC
int __MsgFunc_SetFog(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_SetFog( pszName, iSize, pbuf );
	return 1;
}

//LRC
int __MsgFunc_KeyedDLight(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_KeyedDLight( pszName, iSize, pbuf );
	return 1;
}

//LRC
int __MsgFunc_AddShine(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_AddShine( pszName, iSize, pbuf );
	return 1;
}

//LRC
int __MsgFunc_SetSky(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_SetSky( pszName, iSize, pbuf );
	return 1;
}

int __MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
{
#ifdef ENGINE_DEBUG
//	CONPRINT("## ResetHUD\n");
#endif
	return gHUD.MsgFunc_ResetHUD(pszName, iSize, pbuf );
}

int __MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
{
#ifdef ENGINE_DEBUG
//	CONPRINT("## InitHUD\n");
#endif
	gHUD.MsgFunc_InitHUD( pszName, iSize, pbuf );
	return 1;
}

int __MsgFunc_ViewMode(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_ViewMode( pszName, iSize, pbuf );
	return 1;
}

int __MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_SetFOV( pszName, iSize, pbuf );
}

int __MsgFunc_Concuss(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Concuss( pszName, iSize, pbuf );
}

int __MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_GameMode( pszName, iSize, pbuf );
}

// buz
int __MsgFunc_GasMask(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_GasMask( pszName, iSize, pbuf );
}

// buz
int __MsgFunc_SpecTank(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_SpecTank( pszName, iSize, pbuf );
}

int __MsgFunc_ProFont(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_ProFont( pszName, iSize, pbuf );
}

int __MsgFunc_Maglite(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Maglite( pszName, iSize, pbuf );
}

int __MsgFunc_Blur(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Blur( pszName, iSize, pbuf );
}

int __MsgFunc_UnreadSMS(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_UnreadSMS( pszName, iSize, pbuf );
}

int __MsgFunc_ViewKnock(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_ViewKnock( pszName, iSize, pbuf );
}

int __MsgFunc_CoFWeapon(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_CoFWeapon( pszName, iSize, pbuf );
}

int __MsgFunc_ViewEntity(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_ViewEntity( pszName, iSize, pbuf );
}

int __MsgFunc_ViewLock(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_ViewLock( pszName, iSize, pbuf );
}

int __MsgFunc_Ladder(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Ladder( pszName, iSize, pbuf );
}

int __MsgFunc_FallOver(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_FallOver( pszName, iSize, pbuf );
}

int __MsgFunc_BlackWhite(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_BlackWhite( pszName, iSize, pbuf );
}

int __MsgFunc_WeaponBody(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_WeaponBody( pszName, iSize, pbuf );
}

int __MsgFunc_PhoneOff(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_PhoneOff( pszName, iSize, pbuf );
}

int __MsgFunc_Muzzleflash(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Muzzleflash( pszName, iSize, pbuf );
}

int __MsgFunc_FlashFlags(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_FlashFlags( pszName, iSize, pbuf );
}

int __MsgFunc_Boat(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Boat( pszName, iSize, pbuf );
}

int __MsgFunc_Ripple(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Ripple( pszName, iSize, pbuf );
}

int __MsgFunc_AddNote(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_AddNote( pszName, iSize, pbuf );
}

int __MsgFunc_AttachCam(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_AttachCam( pszName, iSize, pbuf );
}

int __MsgFunc_BigPrick(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_BigPrick( pszName, iSize, pbuf );
}

int __MsgFunc_NewFade(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_NewFade( pszName, iSize, pbuf );
}

int __MsgFunc_SkipScene(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_SkipScene( pszName, iSize, pbuf );
}

int __MsgFunc_IronSights(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_IronSights( pszName, iSize, pbuf );
}

int __MsgFunc_Wheelchair(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Wheelchair( pszName, iSize, pbuf );
}

int __MsgFunc_GreyFade(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_GreyFade( pszName, iSize, pbuf );
}

int __MsgFunc_TakePhoto(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_TakePhoto( pszName, iSize, pbuf );
}

int __MsgFunc_Objectives(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Objectives( pszName, iSize, pbuf );
}

int __MsgFunc_Unlocks(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Unlocks( pszName, iSize, pbuf );
}

int __MsgFunc_CustomSlv(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_CustomSlv( pszName, iSize, pbuf );
}

int __MsgFunc_CustomFng(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_CustomFng( pszName, iSize, pbuf );
}

int __MsgFunc_CustomGlv(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_CustomGlv( pszName, iSize, pbuf );
}

int __MsgFunc_PlaySFX(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_PlaySFX( pszName, iSize, pbuf );
}

int __MsgFunc_Inventory(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Inventory( pszName, iSize, pbuf );
}

int __MsgFunc_Materials(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_Materials( pszName, iSize, pbuf );
}

// TFFree Command Menu
void __CmdFunc_OpenCommandMenu(void)
{
	if ( gViewPort )
	{
		gViewPort->ShowCommandMenu( gViewPort->m_StandardMenu );
	}
}

// TFC "special" command
void __CmdFunc_InputPlayerSpecial(void)
{
	if ( gViewPort )
	{
		gViewPort->InputPlayerSpecial();
	}
}

void __CmdFunc_CloseCommandMenu(void)
{
	if ( gViewPort )
	{
		gViewPort->InputSignalHideCommandMenu();
	}
}

void __CmdFunc_ForceCloseCommandMenu( void )
{
	if ( gViewPort )
	{
		gViewPort->HideCommandMenu();
	}
}

void __CmdFunc_ToggleServerBrowser( void )
{
	if ( gViewPort )
	{
		gViewPort->ToggleServerBrowser();
	}
}

void __CmdFunc_guess_FinishLadder( void )
{
	guess_pAttachCamEnt = NULL;
}

void __CmdFunc_guess_DonatorCheck( void )
{
	guess_iDonator = 0;

	bool bGroup2 = false;
	if ( guess_IsInDonatorGroup2() )
		bGroup2 = true;

	if ( !guess_IsInDonatorGroup1() )
	{
		if ( !bGroup2 )
			guess_iDonator = 0;
		else if ( bGroup2 == true )
			guess_iDonator = 3;
	}
	else
	{
		if ( !bGroup2 )
			guess_iDonator = 1;
		else if ( bGroup2 == true )
			guess_iDonator = 2;
	}

	if ( SteamUser() )
	{
		CSteamID steamID = SteamUser()->GetSteamID();

		if ( steamID.ConvertToUint64() == 268893370.0 )
			guess_iDonator = 4;

		if ( steamID.ConvertToUint64() == 268992206.0 )
			guess_iDonator = 4;
	}
}

// TFFree Command Menu Message Handlers
int __MsgFunc_ValClass(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ValClass( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_TeamNames(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamNames( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_Feign(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_Feign( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_Detpack(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_Detpack( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_VGUIMenu(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_VGUIMenu( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_MOTD(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_MOTD( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_BuildSt(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_BuildSt( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_RandomPC(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_RandomPC( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_ServerName(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ServerName( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ScoreInfo( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamScore( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamInfo( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_Spectator(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_Spectator( pszName, iSize, pbuf );
	return 0;
}

int __MsgFunc_AllowSpec(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_AllowSpec( pszName, iSize, pbuf );
	return 0;
}


int __MsgFunc_PlayMP3(const char *pszName, int iSize, void *pbuf )
{
	return gHUD.MsgFunc_PlayMP3( pszName, iSize, pbuf );
}

void __CmdFunc_StopMP3( void )
{
	guess_IrrKlangStop( 0 );
}

void __CmdFunc_guess_BeginSPGame( void )
{
	gEngfuncs.pfnClientCmd( "disconnect;maxplayers 1;deathmatch 0;map c_difficulty_settings" );
}

void __CmdFunc_guess_To3DMenu( void )
{
	if ( gEngfuncs.GetMaxClients() > 1 )
	{
		gEngfuncs.pfnClientCmd( "disconnect;maxplayers 1;deathmatch 0" );
		return;
	}

	gEngfuncs.pfnClientCmd( "map c_game_menu1" );
	guess_iMenuMode = 2;
}

void __CmdFunc_guess_ReturnMenu( void )
{
	gEngfuncs.pfnClientCmd( "disconnect;maxplayers 1;deathmatch 0" );
}

void __CmdFunc_guess_MP3Player( void )
{
}

void __CmdFunc_guess_ToUnlockables( void )
{
	if ( gEngfuncs.GetMaxClients() > 1 )
	{
		gEngfuncs.pfnClientCmd( "disconnect;maxplayers 1;deathmatch 0" );
		return;
	}

	gEngfuncs.pfnClientCmd( "map c_unlockables" );
}

// not hooked in 1.6a
void __CmdFunc_guess_Unlockables( void )
{
	if ( gEngfuncs.pfnGetLevelName() && strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
	{
		gEngfuncs.pfnClientCmd( "disconnect;maxplayers 1;deathmatch 0;map c_unlockables" );
	}
	else if ( guess_iHudActive )
	{
		PlaySound( "inventory/inventory_no.wav", 1 );
	}
	else
	{
		gEngfuncs.pfnClientCmd( "disconnect;maxplayers 1;deathmatch 0;map c_unlockables" );
	}
}

void guess_TextureTest( void );

void __CmdFunc_guess_TextureTest( void )
{
	guess_TextureTest();
}

// buz
int __MsgFunc_TextWindow(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ShowTextWindow( pszName, iSize, pbuf );
	return 0;
}

// buz
int __MsgFunc_RainData(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_RainData( pszName, iSize, pbuf );
}

void InitHeadShield( void ); // buz
void InitGlows();
void GammaGraphInit();
void InitPostEffects();

void CanUseInit( void ); // Wargon: »конка юза.
void guess_HUDControlInit( void );
void guess_BoardInit( void );
void guess_NoteDocInit( void );
void guess_KeypadInit( void );
void guess_PhoneInit( void );
void guess_PuzzlebarInit( void );
void guess_SaveInit( void );
void guess_YesNoInit( void );
void guess_StatueInit( void );
void guess_ComputerInit( void );
void guess_UserDocInit( void );
void guess_PadlockInit( void );
void guess_SaveLoadInit( void );
void guess_GameMenuInit( void );
void guess_ScopeInit( void );
void guess_TVFrameInit( void );
void guess_BossBarInit( void );
void guess_TelephoneInit( void );
void guess_QSlotInit( void );
void guess_AmmoInvInit( void );
void guess_CreditsInit( void );
void guess_ClothesInit( void );
void guess_UnlockablesInit( void );
void guess_CoopStatsInit( void );
void guess_CustomDocInit( void );

// This is called every time the DLL is loaded
void CHud :: Init( void )
{
	if ( !guess_m_pVoiceTweak )
		guess_m_pVoiceTweak = gEngfuncs.pVoiceTweak;

	guess_m_szSleeveTexture[0] = 0;
	guess_m_szFingerTexture[0] = 0;
	gHUD.guess_m_szGloveTexture[0] = 0;
	guess_bCustomFingerTexture = false;
	guess_bCustomGloveTexture = false;
	guess_m_bSaveHack = false;
	guess_m_flSaveHackTime = 0;
	guess_m_iSaveHackSlot = 0;
	guess_iHudActive = 1;
	guess_m_flDamageTime = 0;

	Log("Initializing variables\n");
	gl.Init();

	guess_m_iBlur = 0;
	guess_flNextSoundCacheTime = 0;
	guess_m_bMenuClick = false;
	guess_m_flMenuClickTime = 0;
	guess_bUnknown10545081 = false;

	InitGlows();
	RendererInit();
	GammaGraphInit();
	InitPostEffects();

	guess_m_hMuzzleflash = 0;
	guess_iGreyFade = 0;
	guess_m_szObjectiveTitle[0] = 0;
	guess_m_szObjectiveText[0] = 0;

#ifdef ENGINE_DEBUG
//	CONPRINT("## CHud::Init\n");
#endif
	InitRain();	// rain
	InitHeadShield(); // buz
	GrassInit(); // buz
	RadioIconInit(); // buz
	Hud2Init(); // buz
	SubtitleInit(); // buz
	TabPanelInit(); // buz
	guess_HUDControlInit();
	guess_BoardInit();
	guess_NoteDocInit();
	guess_KeypadInit();
	guess_PhoneInit();
	guess_PuzzlebarInit();
	guess_SaveInit();
	guess_YesNoInit();
	guess_StatueInit();
	guess_ComputerInit();
	guess_UserDocInit();
	guess_PadlockInit();
	guess_SaveLoadInit();
	guess_GameMenuInit();
	guess_ScopeInit();
	guess_TVFrameInit();
	guess_BossBarInit();
	guess_TelephoneInit();
	guess_QSlotInit();
	guess_AmmoInvInit();
	guess_CreditsInit();
	guess_ClothesInit();
	guess_UnlockablesInit();
	guess_CoopStatsInit();
	guess_CustomDocInit();

	guess_m_bConcussed = false;

	CanUseInit(); // Wargon: »конка юза.

	HOOK_MESSAGE( Logo );
	HOOK_MESSAGE( ResetHUD );
	HOOK_MESSAGE( GameMode );
	HOOK_MESSAGE( InitHUD );
	HOOK_MESSAGE( ViewMode );
	HOOK_MESSAGE( SetFOV );
	HOOK_MESSAGE( Concuss );
	HOOK_MESSAGE( HUDColor ); //LRC
	HOOK_MESSAGE( SetFog ); //LRC
	HOOK_MESSAGE( KeyedDLight ); //LRC
	HOOK_MESSAGE( AddShine ); //LRC
	HOOK_MESSAGE( SetSky ); //LRC
	HOOK_MESSAGE( GasMask ); //buz
	HOOK_MESSAGE( SpecTank ); //buz
	HOOK_MESSAGE( TextWindow ); // buz
	HOOK_MESSAGE( RainData );// buz
	HOOK_MESSAGE( ProFont );
	HOOK_MESSAGE( Maglite );
	HOOK_MESSAGE( Blur );
	HOOK_MESSAGE( UnreadSMS );
	HOOK_MESSAGE( ViewKnock );
	HOOK_MESSAGE( CoFWeapon );
	HOOK_MESSAGE( ViewEntity );
	HOOK_MESSAGE( ViewLock );
	HOOK_MESSAGE( Ladder );
	HOOK_MESSAGE( FallOver );
	HOOK_MESSAGE( BlackWhite );
	HOOK_MESSAGE( WeaponBody );
	HOOK_MESSAGE( PhoneOff );
	HOOK_MESSAGE( Muzzleflash );
	HOOK_MESSAGE( FlashFlags );
	HOOK_MESSAGE( Boat );
	HOOK_MESSAGE( Ripple );
	HOOK_MESSAGE( AddNote );
	HOOK_MESSAGE( AttachCam );
	HOOK_MESSAGE( BigPrick );
	HOOK_MESSAGE( NewFade );
	HOOK_MESSAGE( SkipScene );
	HOOK_MESSAGE( IronSights );
	HOOK_MESSAGE( Wheelchair );
	HOOK_MESSAGE( GreyFade );
	HOOK_MESSAGE( TakePhoto );
	HOOK_MESSAGE( Objectives );
	HOOK_MESSAGE( Unlocks );
	HOOK_MESSAGE( CustomSlv );
	HOOK_MESSAGE( CustomFng );
	HOOK_MESSAGE( CustomGlv );
	HOOK_MESSAGE( PlaySFX );

	// TFFree CommandMenu
	HOOK_COMMAND( "+commandmenu", OpenCommandMenu );
	HOOK_COMMAND( "-commandmenu", CloseCommandMenu );
	HOOK_COMMAND( "ForceCloseCommandMenu", ForceCloseCommandMenu );
	HOOK_COMMAND( "special", InputPlayerSpecial );
	HOOK_COMMAND( "togglebrowser", ToggleServerBrowser );
	HOOK_COMMAND( "finishladder", guess_FinishLadder );
	HOOK_COMMAND( "donatorcheck", guess_DonatorCheck );

	HOOK_MESSAGE( ValClass );
	HOOK_MESSAGE( TeamNames );
	HOOK_MESSAGE( Feign );
	HOOK_MESSAGE( Detpack );
	HOOK_MESSAGE( MOTD );
	HOOK_MESSAGE( BuildSt );
	HOOK_MESSAGE( RandomPC );
	HOOK_MESSAGE( ServerName );
	HOOK_MESSAGE( ScoreInfo );
	HOOK_MESSAGE( TeamScore );
	HOOK_MESSAGE( TeamInfo );

	HOOK_MESSAGE( Spectator );
	HOOK_MESSAGE( AllowSpec );

	// VGUI Menus
	HOOK_MESSAGE( VGUIMenu );

	HOOK_MESSAGE( Inventory );
	HOOK_MESSAGE( Materials );

	CVAR_CREATE( "dev_commentary", "0", FCVAR_ARCHIVE );
	m_pZoomSpeed = gEngfuncs.pfnRegisterVariable( "cl_zoomspeed","100", 0 );
	CVAR_CREATE( "hud_classautokill", "1", FCVAR_ARCHIVE | FCVAR_USERINFO );		// controls whether or not to suicide immediately on TF class switch
	CVAR_CREATE( "hud_takesshots", "0", FCVAR_ARCHIVE );		// controls whether or not to automatically take screenshots at the end of a round
	CVAR_CREATE( "brightvalue", "0.5", FCVAR_ARCHIVE | FCVAR_USERINFO );
	CVAR_CREATE( "skybrightvalue", "0.1", FCVAR_ARCHIVE | FCVAR_USERINFO );
	CVAR_CREATE( "goro", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "skybrightcolourr", "0.0", FCVAR_ARCHIVE | FCVAR_USERINFO );
	CVAR_CREATE( "skybrightcolourg", "0.0", FCVAR_ARCHIVE | FCVAR_USERINFO );
	CVAR_CREATE( "skybrightcolourb", "0.0", FCVAR_ARCHIVE | FCVAR_USERINFO );
	guess_iGoro = CVAR_GET_FLOAT( "goro" );

	HOOK_MESSAGE( PlayMP3 );
	HOOK_COMMAND( "stopmp3", StopMP3 );
	guess_IrrKlangSetParams( 0, 0, 0, 0 );

	HOOK_COMMAND( "beginspgame", guess_BeginSPGame );
	HOOK_COMMAND( "to3dmenu", guess_To3DMenu );
	HOOK_COMMAND( "returnmenu", guess_ReturnMenu );
	HOOK_COMMAND( "returnmenu", guess_ReturnMenu );
	HOOK_COMMAND( "mp3player", guess_MP3Player );
	HOOK_COMMAND( "unlockablescmd", guess_ToUnlockables );
	HOOK_COMMAND( "texturetest", guess_TextureTest );

	m_iLogo = 0;
	m_iFOV = guess_iDefaultFOV;
	m_iHUDColor = 0x00FFFFFF;

	guess_m_iFPSMin = 0;
	guess_m_iFPSMax = 0;
	guess_m_iFPSAverage = 0;
	guess_m_bFPSToggle = false;
	guess_m_flNextFPSTime = gEngfuncs.GetClientTime();
	for ( int i = 1; i <= 100; i++ )
		guess_m_iFPS[i] = 0;

	CVAR_CREATE( "zoom_sensitivity_ratio", "1.2", 0 );
	RainInfo = gEngfuncs.pfnRegisterVariable( "cl_raininfo", "0", 0 );	// rain
//	default_fov = CVAR_CREATE( "default_fov", "90", 0 );// buz: turn off
	m_pCvarStealMouse = CVAR_CREATE( "hud_capturemouse", "1", FCVAR_ARCHIVE );
	m_pCvarDraw = CVAR_CREATE( "hud_draw", "1", FCVAR_ARCHIVE );
	cl_lw = gEngfuncs.pfnGetCvarPointer( "cl_lw" );

	// Wargon: fmod_volume регулирует громкость звуков, воспроизводимых через FMOD.
	CVAR_CREATE( "fmod_volume", "1.0", FCVAR_ARCHIVE );

	m_pSpriteList = NULL;

	// Clear any old HUD list
	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			free( pList );
		}
		m_pHudList = NULL;
	}

	// In case we get messages before the first update -- time will be valid
	m_flTime = 1.0;

	m_Ammo.Init();
	m_Health.Init();
	m_SayText.Init();
	m_Spectator.Init();
	m_Geiger.Init();
	m_Train.Init();
	m_Battery.Init();
	m_Flash.Init();
	m_Message.Init();
	m_StatusBar.Init();
	m_DeathNotice.Init();
	m_AmmoSecondary.Init();
	m_TextMessage.Init();
	m_StatusIcons.Init();
	guess_m_Lensflare.Init();
	GetClientVoiceMgr()->Init(&g_VoiceStatusHelper, (vgui::Panel**)&gViewPort);
	m_Particle.Init(); // (LRC) -- 30/08/02 November235: Particles to Order
	guess_m_Neffect.Init();
	guess_m_BlackBar.Init();

	m_Menu.Init();

	ServersInit();

	if ( !gEngfuncs.CheckParm( "-nomusic", NULL ) )
		guess_IrrKlangInit();

	MsgFunc_ResetHUD(0, 0, NULL );
	Log("END Initializing variables\n");
}

// CHud destructor
// cleans up memory allocated for m_rg* arrays
CHud :: ~CHud()
{
#ifdef ENGINE_DEBUG
//	CONPRINT("## CHud::destructor\n");
#endif

//	ResetRain();	// rain

	guess_iHudActive = 0;

	delete [] m_rghSprites;
	delete [] m_rgrcRects;
	delete [] m_rgszSpriteNames;

	RendererCleanup();
//	g_objmanager.Reset();

	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			free( pList );
		}
		m_pHudList = NULL;
	}

	ServersShutdown();
//	UnInstallExceptionHandler(); // buz
	UnloadCrashRpt(); // buz
}

// GetSpriteIndex()
// searches through the sprite list loaded from hud.txt for a name matching SpriteName
// returns an index into the gHUD.m_rghSprites[] array
// returns 0 if sprite not found
int CHud :: GetSpriteIndex( const char *SpriteName )
{
	// look through the loaded sprite name list for SpriteName
	for ( int i = 0; i < m_iSpriteCount; i++ )
	{
		if ( strncmp( SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH ) == 0 )
			return i;
	}

	return -1; // invalid sprite
}

void ResetHeadShield(); // buz
void ResetGlows();
void CAM_ToFirstPerson( void );

void CHud :: VidInit( void )
{
	guess_iMenuMode = 1;
	guess_m_flDamageTime = 0;
	guess_m_bSaveHack = false;
	guess_m_flSaveHackTime = 0;
	guess_m_iSaveHackSlot = 0;
	guess_m_szSleeveTexture[0] = 0;
	guess_m_szFingerTexture[0] = 0;
	gHUD.guess_m_szGloveTexture[0] = 0;
	guess_bCustomFingerTexture = false;
	guess_bCustomGloveTexture = false;
	guess_flNextSoundCacheTime = 0;
	guess_iHudActive = 1;
	guess_IrrKlangSetParams( 0, 0, 0, 0 );

	Log("\nInitializing video\n");

	guess_m_szObjectiveTitle[0] = 0;
	guess_m_szObjectiveText[0] = 0;
	guess_m_bMenuClick = false;
	guess_m_flMenuClickTime = 0;
	guess_bUnknown10545081 = false;
	guess_iGoro = CVAR_GET_FLOAT( "goro" );
	guess_m_flGreyFadeEnd = 0;
	guess_m_flGreyFadeStart = 0;
	guess_m_flViewLockTime = -1;
	guess_m_bViewLockWasOn = false;
	guess_m_flUnknown350 = -1;
	guess_m_vecUnknown344 = Vector( 0, 0, 0 );
	guess_iGreyFade = 0;
	guess_flWeaponIdleSpeed = CVAR_GET_FLOAT( "cl_weapon_idle_speed" );
	guess_flWeaponIdleDampener = CVAR_GET_FLOAT( "cl_weapon_idle_dampener" );
	guess_bUnknown105431E9 = false;
	guess_m_bIronSights = false;
	guess_vecIronsightOfs.y = guess_vecIronsightOfs.x = guess_vecIronsightOfs.z = 0;
	guess_m_vecIronSightOfs.x = guess_m_vecIronSightOfs.y = guess_m_vecIronSightOfs.z = 0;
	guess_m_vecIronSightOfs2.x = guess_m_vecIronSightOfs2.y = guess_m_vecIronSightOfs2.z = 0;
	guess_flUnknown105595A0 = 0;
	guess_flUnknown105595A4 = 0;
	guess_flUnknown105595A8 = 0;
	guess_iUnknown105595AC = 0;
	guess_m_iFlashFlags = 0;
	guess_pTelescopeEnt = NULL;
	guess_pAttachCamEnt = NULL;
	guess_iBoat = 0;
	guess_iWheelchair = 0;
	guess_m_iCameraZoom = 0;
	guess_m_flNextSwayTime = m_flTime;
	guess_pCamTargetEnt = NULL;
	guess_bWheelchairLock = false;
	guess_m_iHoldBreath = 0;
	guess_iUnknown1055959C = 0;
	guess_bXboxFix = false;
	guess_m_bCameraZoomOn = false;
	guess_m_bXboxFix[0] = false;
	guess_m_bXboxFix[1] = false;
	guess_m_bXboxFix[2] = false;
	guess_m_bXboxFix[3] = false;
	guess_m_bXboxFix[4] = false;
	guess_m_bXboxFix[5] = false;
	guess_m_bXboxFix[6] = false;
	guess_m_bXboxFix[7] = false;
	guess_m_bXboxFix[8] = false;
	guess_m_bXboxFix[9] = false;
	guess_m_bXboxFix[10] = false;
	guess_m_bXboxFix[11] = false;
	guess_m_bXboxFix[14] = false;
	guess_m_bXboxFix[15] = false;
	guess_m_bXboxFix[16] = false;
	guess_m_bXboxFix[17] = false;
	guess_m_bXboxFix[18] = false;
	guess_m_bXboxFix[19] = false;
	guess_m_bXboxFix[20] = false;
	guess_m_bXboxFix[21] = false;
	guess_m_bXboxFix[22] = false;
	guess_m_bXboxFix[23] = false;
	guess_m_bXboxFix[24] = false;
	guess_m_bXboxFix[25] = false;
	guess_m_bXboxFix[12] = false;
	guess_m_bXboxFix[13] = false;

	gl.VidInit(); // buz

	guess_m_bConcussed = false;

	ResetRain();	// rain

	m_flDeadTime = 0; // buz
	m_iGasMask = 0; // buz
	m_SpecTank_on = 0; // buz

	guess_m_flResetTime = gEngfuncs.GetClientTime();
	guess_m_flUnknown2A84 = 0;
	guess_m_bUnknown2A88 = false;
	guess_m_iBlackWhite = 0;

	ResetHeadShield(); // buz
	m_flFOV = -1; // buz
	guess_m_flCameraZoomFOV = 0;

	GrassVidInit();

	ResetGlows();

	g_objmanager.Reset();

	RendererVidInit();

	guess_m_hMuzzleflash = SPR_Load( "sprites/muzzleflash.spr" );

#ifdef ENGINE_DEBUG
//	CONPRINT("## CHud::VidInit\n");
#endif
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	// ----------
	// Load Sprites
	// ---------
//	m_hsprFont = LoadSprite("sprites/%d_font.spr");

	m_hsprLogo = 0;
	m_hsprCursor = 0;
	m_iFOV = guess_iDefaultFOV;

	if (ScreenWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;

	// Only load this once
	if ( !m_pSpriteList )
	{
		// we need to load the hud.txt, and all sprites within
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			// count the number of sprites of the appropriate res
			m_iSpriteCount = 0;
			client_sprite_t *p = m_pSpriteList;
			int j;
			for ( j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
					m_iSpriteCount++;
				p++;
			}

			// allocated memory for sprite handle arrays
 			m_rghSprites = new HSPRITE[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];

			p = m_pSpriteList;
			int index = 0;
			for ( j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
				{
					char sz[256];
					sprintf(sz, "sprites/%s.spr", p->szSprite);
					m_rghSprites[index] = SPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					strncpy( &m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH );

					index++;
				}

				p++;
			}
		}
	}
	else
	{
		// we have already have loaded the sprite reference from hud.txt, but
		// we need to make sure all the sprites have been loaded (we've gone through a transition, or loaded a save game)
		client_sprite_t *p = m_pSpriteList;
		int index = 0;
		for ( int j = 0; j < m_iSpriteCountAllRes; j++ )
		{
			if ( p->iRes == m_iRes )
			{
				char sz[256];
				sprintf( sz, "sprites/%s.spr", p->szSprite );
				m_rghSprites[index] = SPR_Load(sz);
				index++;
			}

			p++;
		}
	}

	// assumption: number_1, number_2, etc, are all listed and loaded sequentially
	m_HUD_number_0 = GetSpriteIndex( "number_0" );

	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;

	m_Ammo.VidInit();
	m_Health.VidInit();
	m_Spectator.VidInit();
	m_Geiger.VidInit();
	m_Train.VidInit();
	m_Battery.VidInit();
	m_Flash.VidInit();
	m_Message.VidInit();
	m_StatusBar.VidInit();
	m_DeathNotice.VidInit();
	m_SayText.VidInit();
	m_Menu.VidInit();
	m_AmmoSecondary.VidInit();
	m_TextMessage.VidInit();
	guess_m_Neffect.VidInit();
	guess_m_BlackBar.VidInit();
	m_StatusIcons.VidInit();
	guess_m_Lensflare.Init();
	GetClientVoiceMgr()->VidInit();
	m_Particle.VidInit(); // (LRC) -- 30/08/02 November235: Particles to Order

	guess_m_iMaglite = 0;
	guess_m_iUnreadSMS = 0;
	guess_m_flUnreadSMSTime = gEngfuncs.GetClientTime();

	if ( gViewPort )
		gViewPort->guess_m_flKeyInputTime = gEngfuncs.GetClientTime();

	guess_m_iViewEntity = 0;
	guess_m_flForwardTapTime = gEngfuncs.GetClientTime();
	guess_m_flBackTapTime = gEngfuncs.GetClientTime();
	guess_m_flLeftTapTime = gEngfuncs.GetClientTime();
	guess_m_flRightTapTime = gEngfuncs.GetClientTime();
	guess_m_iUnknown2A90 = 0;

	guess_m_iFadeR = 0;
	guess_m_iFadeG = 0;
	guess_m_iFadeB = 0;
	guess_m_iFadeA = 0;
	guess_m_iFadeAlpha = 0;
	guess_m_flFadeStart = -1;
	guess_m_flFadeTime = -1;
	guess_m_flFadeEnd = -1;
	guess_m_bFadeOut = false;
	guess_m_bFadeHold = false;

	Log("End Initializing video\n");

	CAM_ToFirstPerson();
}

int CHud::MsgFunc_Logo(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	// update Train data
	m_iLogo = READ_BYTE();

	return 1;
}

//LRC
int CHud::MsgFunc_HUDColor(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	m_iHUDColor = READ_LONG();

	return 1;
}

//float g_lastFOV = 0.0; buz

/*
============
COM_FileBase
============
*/
// Extracts the base name of a file (no path, no extension, assumes '/' as path separator)
void COM_FileBase ( const char *in, char *out)
{
	int len, start, end;

	len = strlen( in );

	// scan backward for '.'
	end = len - 1;
	while ( end && in[end] != '.' && in[end] != '/' && in[end] != '\\' )
		end--;

	if ( in[end] != '.' )		// no '.', copy to end
		end = len-1;
	else
		end--;					// Found ',', copy to left of '.'


	// Scan backward for '/'
	start = len-1;
	while ( start >= 0 && in[start] != '/' && in[start] != '\\' )
		start--;

	if ( in[start] != '/' && in[start] != '\\' )
		start = 0;
	else
		start++;

	// Length of new sting
	len = end - start + 1;

	// Copy partial string
	strncpy( out, &in[start], len );
	// Terminate it
	out[len] = 0;
}

/*
=================
HUD_IsGame

=================
*/
int HUD_IsGame( const char *game )
{
	const char *gamedir;
	char gd[ 1024 ];

	gamedir = gEngfuncs.pfnGetGameDirectory();
	if ( gamedir && gamedir[0] )
	{
		COM_FileBase( gamedir, gd );
		if ( !stricmp( gd, game ) )
			return 1;
	}
	return 0;
}

/*
=====================
HUD_GetFOV

Returns last FOV
=====================
*/
#if 0 // buz
float HUD_GetFOV( void )
{
	if ( gEngfuncs.pDemoAPI->IsRecording() )
	{
		// Write it
		int i = 0;
		unsigned char buf[ 100 ];

		// Active
		*( float * )&buf[ i ] = g_lastFOV;
		i += sizeof( float );

		Demo_WriteBuffer( TYPE_ZOOM, i, buf );
	}

	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
	{
		g_lastFOV = g_demozoom;
	}
	return g_lastFOV;
}
#endif

int CHud::MsgFunc_SetFOV(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	int newfov = READ_BYTE();
//	int def_fov = CVAR_GET_FLOAT( "default_fov" ); // buz: turn off

	//Weapon prediction already takes care of changing the fog. ( g_lastFOV ).
//	if ( cl_lw && cl_lw->value )
//		return 1; // buz: nety

#if 0 // buz: ignore server's fov commands
	g_lastFOV = newfov;

	if ( newfov == 0 )
	{
		m_iFOV = 90;//def_fov; // buz: turn off
	}
	else
	{
		m_iFOV = newfov;
	}

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if ( m_iFOV == 90)//def_fov ) // buz: turn off
	{
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = sensitivity->value * ((float)newfov / /*(float)def_fov*/90) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}
#endif

	return 1;
}


void CHud::AddHudElem(CHudBase *phudelem)
{
	HUDLIST *pdl, *ptemp;

//phudelem->Think();

	if (!phudelem)
		return;

	pdl = (HUDLIST *)malloc(sizeof(HUDLIST));
	if (!pdl)
		return;

	memset(pdl, 0, sizeof(HUDLIST));
	pdl->p = phudelem;

	if (!m_pHudList)
	{
		m_pHudList = pdl;
		return;
	}

	ptemp = m_pHudList;

	while (ptemp->pNext)
		ptemp = ptemp->pNext;

	ptemp->pNext = pdl;
}

float CHud::GetSensitivity( void )
{
	return m_flMouseSensitivity;
}

char *CHud::guess_GetKeyForCommand( char *pszCommand )
{
	static char szKey[256];
	szKey[0] = 0;

	char szConfig[256];
	strcpy( szConfig, gEngfuncs.pfnGetGameDirectory() );
	strcat( szConfig, "/config.cfg" );

	std::ifstream file( szConfig );
	std::string line;
	std::vector<std::string> tokens;

	while ( std::getline( file, line ) )
	{
		char szLine[256];
		strcpy( szLine, line.c_str() );
		szLine[line.length()] = 0;

		if ( strstr( szLine, "bind" ) && strstr( szLine, pszCommand ) )
		{
			bool bInQuotes = false;
			int j = 0;

			for ( int i = 1; i <= (int)strlen( szLine ); i++ )
			{
				if ( bInQuotes )
				{
					if ( szLine[i] == '"' )
					{
						szKey[j] = 0;
						return szKey;
					}

					szKey[j] = toupper( szLine[i] );
					j++;
				}
				else if ( szLine[i] == '"' )
				{
					bInQuotes = true;
				}
			}

			return NULL;
		}
	}

	gEngfuncs.Con_Printf( "Couldn't get key for bind %s!\n", pszCommand );
	return NULL;
}

char *CHud::guess_GetInventoryItem( int iSlot )
{
	switch ( iSlot )
	{
	case 1:
		return guess_m_szInventoryItem1;
	case 2:
		return guess_m_szInventoryItem2;
	case 3:
		return guess_m_szInventoryItem3;
	case 4:
		return guess_m_szInventoryItem4;
	case 5:
		return guess_m_szInventoryItem5;
	case 6:
		return guess_m_szInventoryItem6;
	}

	return "";
}

int CHud::guess_AddUnknown5590( int iValue )
{
	return guess_m_iUnknown5590 + iValue;
}

int CHud::guess_SubUnknown5590( int iValue )
{
	return iValue - guess_AddUnknown5590( 0 );
}
