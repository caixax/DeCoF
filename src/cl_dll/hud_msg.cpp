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
//  hud_msg.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "mp3.h"
#include "rain.h"
#include "event_api.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_HUDControl.h"
#include "guess_RECBInventory.h"
#include "guess_Documents.h"
#include "..\game_shared\vgui_loadtga.h"
#include <direct.h>

//LRC - the fogging fog
float g_fFogColor[3];
float g_fStartDist;
float g_fEndDist;
//int g_iFinalStartDist; //for fading
int g_iFinalEndDist;   //for fading
float g_fFadeDuration; //negative = fading out

int guess_iDifficulty;				// "difficulty" cvar, read at InitHUD
int guess_iGetUpTaps;				// FallOver message, +jump presses needed to send "tapgetup"
int guess_iGetUpTapCount;			// +jump presses since the FallOver message
bool guess_bCustomFingerTexture;	// CustomFng message
bool guess_bCustomGloveTexture;		// CustomGlv message
bool guess_bWheelchairLock;			// Wheelchair message
bool guess_bUnknown10542BB3;		// cleared by AttachCam
int guess_g_iDickMode;
int guess_iWheelchair;				// Wheelchair message, the entity (0 when off)
int guess_iBoat;					// Boat message, the entity (0 when off)
cl_entity_t *guess_pAttachCamEnt;	// AttachCam message, the entity the view is attached to
int guess_iAttachCamFlag;			// AttachCam message
int guess_iUnknown10542BC8;			// AttachCam message
float guess_flUnknown10542BCC;		// AttachCam message
float guess_flUnknown10542BD0;		// AttachCam message
float guess_flUnknown10542BD4;		// AttachCam message

char com_token[ 1500 ];

vec3_t guess_vecUnknown105431B4;	// AttachCam message
vec3_t guess_vecUnknown105431C0;	// AttachCam message, the view angles
vec3_t guess_vecUnknown105431CC;	// AttachCam message, render_origin

#define MAX_CLIENTS 32

extern BEAM *pBeam;
extern BEAM *pBeam2;

extern "C" int iClientResetMaterials;

extern cl_entity_t *guess_pCamTargetEnt;
extern vec3_t guess_vecWheelchairAngles;
extern vec3_t guess_vecCameraAttachment;
extern vec3_t render_origin;
extern int guess_iSkipSceneFramerate;
extern int guess_iGreyFade;					// GreyFade message
extern int guess_iUnknown104CC468;			// cleared by the Concuss message, never read
extern bool guess_bUnknown10559514;			// cleared by the AttachCam message
extern bool guess_g_bTakePhoto;				// TakePhoto message, the next frame is captured
extern bool guess_g_bHandsLoaded;
extern bool guess_g_bSleeveLoaded;
extern bool guess_g_bGloveLoaded;
extern bool guess_g_bPoliceSleeveLoaded;

void guess_MuzzleFlash( cl_entity_t *pEntity, int iAttachment, int iType, float flScale );

// Cry of Fear renderer objects reset by InitHUD (the source files that define
// them are not written yet: texture cache at 0x10545118, Cg context at
// 0x10545050, Cg water shader at 0x10545078)
class guess_CTextureManager
{
public:
	bool guess_IsHardware( void );		// IEngineStudio.IsHardware() == 1
	void guess_Reset( void );
	void guess_LoadPlayerShadow( void );	// "playershadow"
};

class guess_CCgContext
{
public:
	void guess_Shutdown( void );
	bool guess_Init( void );			// cgCreateContext, vertex and fragment profiles
};

class guess_CCgWater
{
public:
	void guess_Init( void );			// "cg_water", "black_vp.cg", "fp20_water_fp.cg"
};

extern guess_CTextureManager guess_g_TextureManager;
extern guess_CCgContext guess_g_CgContext;
extern guess_CCgWater guess_g_CgWater;

//=========================================================
// Declarations this file takes from the Steamworks SDK
// (steam_api.h, isteamfriends.h, steamclientpublic.h)
//=========================================================
typedef unsigned int uint32;
typedef unsigned __int64 uint64;

enum EAccountType
{
	k_EAccountTypeInvalid = 0,
	k_EAccountTypeIndividual = 1,
	k_EAccountTypeMultiseat = 2,
	k_EAccountTypeGameServer = 3,
	k_EAccountTypeAnonGameServer = 4,
	k_EAccountTypePending = 5,
	k_EAccountTypeContentServer = 6,
	k_EAccountTypeClan = 7,
	k_EAccountTypeChat = 8,
	k_EAccountTypeConsoleUser = 9,
	k_EAccountTypeAnonUser = 10,
	k_EAccountTypeMax
};

enum EUniverse
{
	k_EUniverseInvalid = 0,
	k_EUniversePublic = 1,
	k_EUniverseBeta = 2,
	k_EUniverseInternal = 3,
	k_EUniverseDev = 4,
	k_EUniverseMax
};

class CSteamID
{
public:
	CSteamID()
	{
		m_steamid.m_unAll64Bits = 0;
	}

	uint32 GetAccountID() const { return m_steamid.m_comp.m_unAccountID; }
	bool BClanAccount() const { return m_steamid.m_comp.m_EAccountType == k_EAccountTypeClan; }

private:
	union SteamID_t
	{
		struct SteamIDComponent_t
		{
			uint32			m_unAccountID : 32;
			unsigned int	m_unAccountInstance : 20;
			unsigned int	m_EAccountType : 4;
			EUniverse		m_EUniverse : 8;
		} m_comp;

		uint64 m_unAll64Bits;
	} m_steamid;
};

class ISteamFriends
{
public:
	virtual const char *GetPersonaName() = 0;
	virtual void SetPersonaName( const char *pchPersonaName ) = 0;
	virtual int GetPersonaState() = 0;
	virtual int GetFriendCount( int iFriendFlags ) = 0;
	virtual CSteamID GetFriendByIndex( int iFriend, int iFriendFlags ) = 0;
	virtual int GetFriendRelationship( CSteamID steamIDFriend ) = 0;
	virtual int GetFriendPersonaState( CSteamID steamIDFriend ) = 0;
	virtual const char *GetFriendPersonaName( CSteamID steamIDFriend ) = 0;
	virtual bool GetFriendGamePlayed( CSteamID steamIDFriend, void *pFriendGameInfo ) = 0;
	virtual const char *GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName ) = 0;
	virtual bool HasFriend( CSteamID steamIDFriend, int iFriendFlags ) = 0;
	virtual int GetClanCount() = 0;
	virtual CSteamID GetClanByIndex( int iClan ) = 0;
};

extern "C" __declspec( dllimport ) ISteamFriends * __cdecl SteamFriends();
//=========================================================

void guess_IrrKlangPlay( const char *pszFile );
void guess_IrrKlangPlaySFX( const char *pszFile, int iType, bool bFlag, vec3_t vecOrigin );
void guess_IrrKlangSetParams( float flParm1, float flParm2, float flParm3, float flParm4 );
void guess_IrrKlangSetLoop( bool bLoop );
void guess_IrrKlangSetStopping( bool bStopping );
void guess_IrrKlangStop( float flFadeTime );

//
// COM_Parse, the same code as in multiplay_gamerules.cpp of the server
//
char *COM_Parse (char *data)
{
	int             c;
	int             len;

	len = 0;
	com_token[0] = 0;

	if (!data)
		return NULL;

// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}

// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}


// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
			break;
	} while (c>32);

	com_token[len] = 0;
	return data;
}

void GetCurrentPath( char *path )
{
	_getcwd( path, 260 );
}

char *CHud::CheckItem( char *pszName, char *pszFile )
{
	int gSCounter = 0;
	char szItemName[512];

	int length;
	char *pFile;
	char *aFile = pFile = (char *)gEngfuncs.COM_LoadFile( pszFile, 5, &length );

	if ( !pFile || !length )
		return "";

	while ( 1 )
	{
		pFile = COM_Parse( pFile );

		if ( strlen( com_token ) <= 0 )
			break;

		strcpy( szItemName, com_token );

		pFile = COM_Parse( pFile );

		if ( com_token[0] != '{' )
		{
			gEngfuncs.Con_Printf( " *\n" );
			gEngfuncs.Con_Printf( " * CheckItem parsing error: expecting { after key name\n" );
			gEngfuncs.Con_Printf( " *\n" );
			break;
		}

		while ( 1 )
		{
			pFile = COM_Parse( pFile );

			if ( com_token[0] == '}' )
				break;

			if ( !strcmp( "data", com_token ) )
			{
				pFile = COM_Parse( pFile );

				if ( !strcmp( szItemName, pszName ) )
				{
					gEngfuncs.COM_FreeFile( aFile );
					return com_token;
				}
			}
		}

		gSCounter++;
	}

	gEngfuncs.COM_FreeFile( aFile );
	return "";
}

// Steam clan 0x29D6CA
int guess_IsInDonatorGroup1( void )
{
	if ( SteamFriends() )
	{
		int iClans = SteamFriends()->GetClanCount();

		if ( SteamFriends()->GetClanCount() > 0 )
		{
			for ( int i = 0; i < iClans; i++ )
			{
				CSteamID clanID = SteamFriends()->GetClanByIndex( i );

				if ( clanID.BClanAccount() && clanID.GetAccountID() == 0x29D6CA )
					return 1;
			}
		}
	}

	return 0;
}

// Steam clan 0x46AE0A
int guess_IsInDonatorGroup2( void )
{
	if ( SteamFriends() )
	{
		int iClans = SteamFriends()->GetClanCount();

		if ( SteamFriends()->GetClanCount() > 0 )
		{
			for ( int i = 0; i < iClans; i++ )
			{
				CSteamID clanID = SteamFriends()->GetClanByIndex( i );

				if ( clanID.BClanAccount() && clanID.GetAccountID() == 0x46AE0A )
					return 1;
			}
		}
	}

	return 0;
}

/// USER-DEFINED SERVER MESSAGE HANDLERS

int CHud :: MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf )
{
	ASSERT( iSize == 0 );

	// clear all hud data
	HUDLIST *pList = m_pHudList;

	while ( pList )
	{
		if ( pList->p )
			pList->p->Reset();
		pList = pList->pNext;
	}

	guess_pCamTargetEnt = NULL;

	// reset sensitivity
	m_flMouseSensitivity = 0;

	// reset concussion effect
	m_iConcussionEffect = 0;

	guess_m_flResetTime = gEngfuncs.GetClientTime();
	guess_m_flUnknown2A84 = 0;
	guess_m_bUnknown2A88 = false;

	guess_m_iFPSMin = 0;
	guess_m_iFPSMax = 0;
	guess_m_iFPSAverage = 0;
	guess_m_bFPSToggle = false;
	guess_m_flNextFPSTime = gEngfuncs.GetClientTime();

	for ( int i = 1; i <= 100; i++ )
		guess_m_iFPS[i] = 0;

	guess_m_iLadder = 0;
	guess_m_flUnknown2A9C = 0;
	guess_m_vecViewOfs.x = guess_m_vecViewOfs.y = guess_m_vecViewOfs.z = 0;
	guess_m_flLadderTime = gEngfuncs.GetClientTime();

	//LRC - reset fog
	g_fStartDist = 0;
	g_fEndDist = 0;

	guess_m_iFallOver = 0;
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
	guess_m_BlackBar.guess_m_iOn = 0;
	guess_m_bViewLock = false;

	if ( gViewPort && gViewPort->guess_m_pRECBInventory )
	{
		for ( int i = 1; i <= 6; i++ )
			guess_GetInventoryItem( i )[0] = 0;

		gViewPort->guess_m_pRECBInventory->guess_Reset();
	}

	if ( !strcmp( gEngfuncs.pfnGetLevelName(), "maps/video_stress_test.bsp" ) )
	{
		gEngfuncs.Cvar_SetValue( "r_weather", 0 );
		gEngfuncs.Cvar_SetValue( "cg_water", 0 );
		gEngfuncs.Cvar_SetValue( "gl_bump", 0 );
		gEngfuncs.Cvar_SetValue( "gl_dynlight", 0 );
	}

	return 1;
}

void CAM_ToFirstPerson(void);

void CHud :: MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf )
{
	CAM_ToFirstPerson();
}

void CHud :: MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf )
{
//	CONPRINT("MSG:InitHUD");
	//LRC - clear the fog
	g_fStartDist = 0;
	g_fEndDist = 0;

	guess_m_iMaglite = 0;
	guess_m_iUnreadSMS = 0;
	guess_m_flUnreadSMSTime = gEngfuncs.GetClientTime();
	gViewPort->guess_m_flKeyInputTime = gEngfuncs.GetClientTime();
	guess_m_iViewEntity = 0;
	guess_m_flForwardTapTime = gEngfuncs.GetClientTime();
	guess_m_flBackTapTime = gEngfuncs.GetClientTime();
	guess_m_flLeftTapTime = gEngfuncs.GetClientTime();
	guess_m_flRightTapTime = gEngfuncs.GetClientTime();

	guess_iDifficulty = gEngfuncs.pfnGetCvarFloat( "difficulty" );

	guess_m_flResetTime = gEngfuncs.GetClientTime();
	guess_m_flUnknown2A84 = 0;
	guess_m_bUnknown2A88 = false;
	guess_m_bViewLock = false;

	m_iSkyMode = SKY_OFF; //LRC

	// prepare all hud data
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if ( pList->p )
			pList->p->InitHUDData();
		pList = pList->pNext;
	}

	//Probably not a good place to put this.
	pBeam = pBeam2 = NULL;

	if ( guess_g_TextureManager.guess_IsHardware() )
	{
		guess_g_TextureManager.guess_Reset();
		guess_g_TextureManager.guess_LoadPlayerShadow();
		guess_g_CgContext.guess_Shutdown();
		guess_g_CgContext.guess_Init();
		guess_g_CgWater.guess_Init();
	}
}

//LRC
void CHud :: MsgFunc_SetFog( const char *pszName, int iSize, void *pbuf )
{
//	CONPRINT("MSG:SetFog");
	BEGIN_READ( pbuf, iSize );

	for ( int i = 0; i < 3; i++ )
		 g_fFogColor[ i ] = READ_BYTE();

	g_fFadeDuration = READ_SHORT();
	g_fStartDist = READ_SHORT();

	if (g_fFadeDuration > 0)
	{
//		// fading in
//		g_fStartDist = READ_SHORT();
		g_iFinalEndDist = READ_SHORT();
//		g_fStartDist = FOG_LIMIT;
		g_fEndDist = FOG_LIMIT;
	}
	else if (g_fFadeDuration < 0)
	{
//		// fading out
//		g_iFinalStartDist =
		g_iFinalEndDist = g_fEndDist = READ_SHORT();
	}
	else
	{
//		g_fStartDist = READ_SHORT();
		g_fEndDist = READ_SHORT();
	}
}

//LRC
void CHud :: MsgFunc_KeyedDLight( const char *pszName, int iSize, void *pbuf )
{
//	CONPRINT("MSG:KeyedDLight");
	BEGIN_READ( pbuf, iSize );

// as-yet unused:
//	float	decay;				// drop this each second
//	float	minlight;			// don't add when contributing less
//	qboolean	dark;			// subtracts light instead of adding (doesn't seem to do anything?)

	int iKey = READ_BYTE();
	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight( iKey );

	int bActive = READ_BYTE();
	if (!bActive)
	{
		// die instantly
		dl->die = gEngfuncs.GetClientTime();
	}
	else
	{
		// never die
		dl->die = gEngfuncs.GetClientTime() + 1E6;

		dl->origin[0] = READ_COORD();
		dl->origin[1] = READ_COORD();
		dl->origin[2] = READ_COORD();
		dl->radius = READ_BYTE();
		dl->color.r = READ_BYTE();
		dl->color.g = READ_BYTE();
		dl->color.b = READ_BYTE();
	}
}

//LRC
void CHud :: MsgFunc_AddShine( const char *pszName, int iSize, void *pbuf )
{
}

//LRC
void CHud :: MsgFunc_SetSky( const char *pszName, int iSize, void *pbuf )
{
//	CONPRINT("MSG:SetSky");
	BEGIN_READ( pbuf, iSize );

	m_iSkyMode = READ_BYTE();
	m_vecSkyPos.x = READ_COORD();
	m_vecSkyPos.y = READ_COORD();
	m_vecSkyPos.z = READ_COORD();
}

int CHud :: MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_Teamplay = READ_BYTE();

	return 1;
}


int CHud :: MsgFunc_Damage(const char *pszName, int iSize, void *pbuf )
{
	int		armor, blood;
	Vector	from;
	int		i;
	float	count;

	BEGIN_READ( pbuf, iSize );
	armor = READ_BYTE();
	blood = READ_BYTE();

	for (i=0 ; i<3 ; i++)
		from[i] = READ_COORD();

	count = (blood * 0.5) + (armor * 0.5);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually

	return 1;
}

int CHud :: MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iConcussionEffect = READ_BYTE();

	if ( READ_BYTE() )
	{
		guess_m_bConcussed = true;
	}
	else
	{
		guess_m_bConcussed = false;
		guess_iUnknown104CC468 = 0;
	}

	return 1;
}

// buz: gasmask message
int CHud :: MsgFunc_GasMask( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iGasMask = READ_BYTE();
//	m_flNextBreathTime = m_flTime + gEngfuncs.pfnRandomFloat( 2, 5 );
	return 1;
}

// buz: special tank message
int CHud :: MsgFunc_SpecTank( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	m_SpecTank_on = READ_BYTE();
	if (m_SpecTank_on == 0) // turn off
		return 1;
	else if (m_SpecTank_on == 2) // only ammo update
	{
		m_SpecTank_Ammo = READ_LONG();
		m_Ammo.m_fFade = 200.0f;
	}
	else // turn on
	{
		m_SpecTank_point.x = READ_COORD();
		m_SpecTank_point.y = READ_COORD();
		m_SpecTank_point.z = READ_COORD();
		m_SpecTank_defYaw = READ_COORD();
		m_SpecTank_coneHor = READ_COORD();
		m_SpecTank_coneVer = READ_COORD();
		m_SpecTank_distFwd = READ_COORD();
		m_SpecTank_distUp = READ_COORD();
		m_SpecTank_Ammo = READ_LONG();
	}
	return 1;
}


int CHud :: MsgFunc_PlayMP3( const char *pszName, int iSize, void *pbuf ) //AJH -Killar MP3
{
	char szFile[256];

	BEGIN_READ( pbuf, iSize );

	int iLoop = READ_BYTE();
	int iPlay = READ_BYTE();
	float flFadeTime = READ_COORD();
	strncpy( szFile, READ_STRING(), 256 );

	if ( gEngfuncs.pfnGetCvarFloat( "cl_downwiththesickness" ) == 1 )
		strncpy( szFile, "downwiththesickness.mp3", 256 );

	// turn on or off?
	if ( iPlay )
	{
		if ( flFadeTime != 0 )
			guess_IrrKlangSetParams( -1, -1, gEngfuncs.GetClientTime(), flFadeTime );

		if ( iLoop )
			guess_IrrKlangSetLoop( true );
		else
			guess_IrrKlangSetLoop( false );

		guess_IrrKlangSetStopping( false );
		guess_IrrKlangPlay( szFile );
	}
	else
	{
		guess_IrrKlangSetStopping( true );
		guess_IrrKlangStop( flFadeTime );
	}

	return 1;
}


// rain
extern rain_properties Rain;

int CHud :: MsgFunc_RainData( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
		Rain.dripsPerSecond =	READ_SHORT();
		Rain.distFromPlayer =	READ_COORD();
		Rain.windX =			READ_COORD();
		Rain.windY =			READ_COORD();
		Rain.randX =			READ_COORD();
		Rain.randY =			READ_COORD();
		Rain.weatherMode =		READ_SHORT();
		Rain.globalHeight =		READ_COORD();

	return 1;
}

int CHud :: MsgFunc_ProFont( const char *pszName, int iSize, void *pbuf )
{
	char szText[256];
	char szRespawn[128];

	BEGIN_READ( pbuf, iSize );

	int iType = READ_BYTE();
	strncpy( szText, READ_STRING(), 256 );

	if ( strstr( szText, "DEADNOTIFY" ) )
	{
		if ( gHUD.guess_GetKeyForCommand( "+jump" ) )
			sprintf( szRespawn, "Press %s to respawn (lose weapons) or wait to be revived", gHUD.guess_GetKeyForCommand( "+jump" ) );
		else
			strcpy( szRespawn, "Press jump to respawn (lose weapons) or wait to be revived" );

		strncpy( szText, szRespawn, 256 );
	}

	if ( gViewPort && gViewPort->guess_m_pHUDControl )
	{
		strcpy( gViewPort->guess_m_pHUDControl->guess_m_szMessage, szText );
		gViewPort->guess_m_pHUDControl->guess_m_flMessageAlpha = 255;
		gViewPort->guess_m_pHUDControl->guess_m_bMessageFadeOut = false;
		gViewPort->guess_m_pHUDControl->guess_m_bMessageHold = false;
		gViewPort->guess_m_pHUDControl->guess_m_flMessageHoldTime = 0;
		gViewPort->guess_m_pHUDControl->guess_m_iMessageType = iType;
	}

	return 1;
}

int CHud :: MsgFunc_Maglite( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_m_iMaglite = READ_BYTE();
	return 1;
}

int CHud :: MsgFunc_Blur( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_m_iBlur = READ_BYTE();
	return 1;
}

int CHud :: MsgFunc_UnreadSMS( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	guess_m_iUnreadSMS = READ_BYTE();
	int iBatteryLow = READ_BYTE();

	if ( gViewPort && gViewPort->guess_m_pHUDControl )
	{
		if ( !gHUD.guess_m_iUnreadSMS )
			gViewPort->guess_m_pHUDControl->guess_ShowSMSIcon( false );

		if ( gHUD.guess_m_iUnreadSMS )
		{
			if ( iBatteryLow )
				gViewPort->guess_m_pHUDControl->guess_m_pSMSIcon->setImage( vgui_LoadTGA( "gfx/vgui/phone_battery_low.tga" ) );
			else
				gViewPort->guess_m_pHUDControl->guess_m_pSMSIcon->setImage( vgui_LoadTGA( "gfx/vgui/640_hudelem_smsicon.tga" ) );

			gViewPort->guess_m_pHUDControl->guess_ShowSMSIcon( true );
		}
	}

	return 1;
}

int CHud :: MsgFunc_ViewKnock( const char *pszName, int iSize, void *pbuf )
{
	vec3_t angles;
	gEngfuncs.GetViewAngles( angles );

	BEGIN_READ( pbuf, iSize );

	float flKnock = READ_BYTE();
	angles[0] -= flKnock;

	if ( !gHUD.guess_m_bViewLock )
		gEngfuncs.SetViewAngles( angles );

	if ( guess_iWheelchair )
		guess_vecWheelchairAngles[0] -= flKnock;

	return 1;
}

int CHud :: MsgFunc_CoFWeapon( const char *pszName, int iSize, void *pbuf )
{
	char szWeapon[128];

	BEGIN_READ( pbuf, iSize );

	READ_BYTE();
	strcpy( szWeapon, READ_STRING() );

	return 1;
}

int CHud :: MsgFunc_ViewEntity( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_m_iViewEntity = READ_SHORT();
	return 1;
}

int CHud :: MsgFunc_ViewLock( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iLock = READ_BYTE();

	if ( READ_COORD() == 666 )
		guess_m_flViewLockTime = 666;
	else
		guess_m_flViewLockTime = gEngfuncs.GetClientTime() + READ_COORD();

	if ( guess_m_flViewLockTime > 0 )
		guess_m_bViewLockWasOn = ( guess_m_bViewLock != 0 );
	else
		guess_m_bViewLockWasOn = false;

	guess_m_bViewLock = ( iLock != 0 );

	return 1;
}

int CHud :: MsgFunc_Ladder( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	guess_m_iLadder = READ_BYTE();
	guess_m_vecLadder.x = READ_COORD();
	guess_m_vecLadder.y = READ_COORD();
	guess_m_vecLadder.z = READ_COORD();
	guess_m_flLadderTime = gEngfuncs.GetClientTime();

	return 1;
}

int CHud :: MsgFunc_FallOver( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	guess_m_iFallOver = READ_BYTE();
	guess_iGetUpTaps = READ_BYTE();
	guess_iGetUpTapCount = 0;
	guess_m_flLadderTime = gEngfuncs.GetClientTime();

	return 1;
}

int CHud :: MsgFunc_BlackWhite( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_m_iBlackWhite = READ_BYTE();
	return 1;
}

int CHud :: MsgFunc_WeaponBody( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iBody = READ_BYTE();
	cl_entity_t *view = gEngfuncs.GetViewModel();

	if ( view )
		view->curstate.body = iBody;

	return 1;
}

int CHud :: MsgFunc_PhoneOff( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	if ( READ_BYTE() )
		guess_m_bPhoneOff = true;
	else
		guess_m_bPhoneOff = false;
	return 1;
}

int CHud :: MsgFunc_Muzzleflash( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iEntity = READ_SHORT();
	int iAttachment = READ_BYTE();
	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex( iEntity );

	if ( pEntity )
		guess_MuzzleFlash( pEntity, iAttachment, 0, 1.0 );

	return 1;
}

int CHud :: MsgFunc_FlashFlags( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_m_iFlashFlags = READ_SHORT();
	return 1;
}

int CHud :: MsgFunc_Boat( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iEntity = READ_SHORT();

	if ( iEntity <= 0 )
		guess_iBoat = 0;
	else
		guess_iBoat = (int)gEngfuncs.GetEntityByIndex( iEntity );

	return 1;
}

int CHud :: MsgFunc_Ripple( const char *pszName, int iSize, void *pbuf )
{
	vec3_t origin;

	BEGIN_READ( pbuf, iSize );

	origin.x = READ_COORD();
	origin.y = READ_COORD();
	origin.z = READ_COORD();

	guess_RippleEffect( origin, READ_SHORT() );

	return 1;
}

int CHud :: MsgFunc_AddNote( const char *pszName, int iSize, void *pbuf )
{
	char szCode[32];
	char szDigit1[128];
	char szDigit2[128];
	char szDigit3[128];
	char szDigit4[128];
	int iDigit[32];

	BEGIN_READ( pbuf, iSize );

	int iNote = READ_BYTE();
	int iHidden = READ_BYTE();
	int iCode = READ_SHORT();
	int iCodeSource = READ_BYTE();

	if ( iNote < 1 || iNote > 16 )
		return 1;

	if ( iHidden && gViewPort && gViewPort->guess_m_pDocuments )
		gViewPort->guess_m_pDocuments->guess_m_iHidden[iNote] = 1;
	else
		gViewPort->guess_m_pDocuments->guess_m_iHidden[iNote] = 0;

	strcpy( guess_m_Notes[iNote].guess_szLine[4], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szLine[5], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szLine[6], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szLine[7], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szLine[0], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szLine[1], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szLine[2], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szLine[3], READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szText1, READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szText2, READ_STRING() );
	strcpy( guess_m_Notes[iNote].guess_szText3, READ_STRING() );

	if ( iCode && iCodeSource )
	{
		if ( !gViewPort )
			return 1;

		if ( gViewPort->guess_m_pDocuments )
		{
			switch ( iCodeSource )
			{
			case 1:
				gViewPort->guess_m_pDocuments->guess_m_pNote[iNote]->setImage( vgui_LoadTGA( "gfx/vgui/notes/codefrompc.tga" ) );
				break;
			case 2:
				gViewPort->guess_m_pDocuments->guess_m_pNote[iNote]->setImage( vgui_LoadTGA( "gfx/vgui/notes/codefromvideotape.tga" ) );
				break;
			case 3:
				gViewPort->guess_m_pDocuments->guess_m_pNote[iNote]->setImage( vgui_LoadTGA( "gfx/vgui/notes/codefromwindow.tga" ) );
				break;
			case 4:
				gViewPort->guess_m_pDocuments->guess_m_pNote[iNote]->setImage( vgui_LoadTGA( "gfx/vgui/notes/phonecode.tga" ) );
				break;
			}

			sprintf( szCode, "%i", iCode );

			for ( unsigned int i = 0; i < strlen( szCode ); i++ )
				iDigit[i] = szCode[i] - '0';

			if ( iDigit[0] >= 0 && iDigit[0] <= 9 )
				sprintf( szDigit1, "gfx/vgui/notes/code_%i.tga", iDigit[0] );
			else
				strcpy( szDigit1, "gfx/vgui/notes/code_1.tga" );

			if ( iDigit[1] >= 0 && iDigit[1] <= 9 )
				sprintf( szDigit2, "gfx/vgui/notes/code_%i.tga", iDigit[1] );
			else
				strcpy( szDigit2, "gfx/vgui/notes/code_1.tga" );

			if ( iDigit[2] >= 0 && iDigit[2] <= 9 )
				sprintf( szDigit3, "gfx/vgui/notes/code_%i.tga", iDigit[2] );
			else
				strcpy( szDigit3, "gfx/vgui/notes/code_1.tga" );

			if ( iDigit[3] >= 0 && iDigit[3] <= 9 )
				sprintf( szDigit4, "gfx/vgui/notes/code_%i.tga", iDigit[3] );
			else
				strcpy( szDigit4, "gfx/vgui/notes/code_1.tga" );

			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit1[iNote]->setImage( vgui_LoadTGA( szDigit1 ) );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit2[iNote]->setImage( vgui_LoadTGA( szDigit2 ) );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit3[iNote]->setImage( vgui_LoadTGA( szDigit3 ) );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit4[iNote]->setImage( vgui_LoadTGA( szDigit4 ) );

			int x, y;
			int noteWide, noteTall;
			int digitWide, digitTall;
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit1[iNote]->getPos( x, y );
			gViewPort->guess_m_pDocuments->guess_m_pNote[iNote]->getSize( noteWide, noteTall );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit1[iNote]->getSize( digitWide, digitTall );

			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit1[iNote]->setPos( x + noteWide, y );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit2[iNote]->setPos( x + noteWide + digitWide, y );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit3[iNote]->setPos( x + noteWide + digitWide * 2, y );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit4[iNote]->setPos( x + noteWide + digitWide * 3, y );

			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit1[iNote]->setVisible( true );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit2[iNote]->setVisible( true );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit3[iNote]->setVisible( true );
			gViewPort->guess_m_pDocuments->guess_m_pCodeDigit4[iNote]->setVisible( true );
		}
	}

	if ( gViewPort && gViewPort->guess_m_pDocuments && !iHidden )
	{
		gViewPort->guess_m_pDocuments->guess_m_pNote[iNote]->setImage( vgui_LoadTGA( guess_m_Notes[iNote].guess_szText3 ) );
		gViewPort->guess_m_pDocuments->guess_m_pNote[iNote]->setVisible( true );
	}

	return 1;
}

int CHud :: MsgFunc_AttachCam( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iEntity = READ_SHORT();
	guess_iAttachCamFlag = READ_BYTE();
	guess_iUnknown10542BC8 = READ_BYTE();
	guess_vecUnknown105431B4.x = READ_LONG();
	guess_vecUnknown105431B4.y = READ_LONG();
	guess_vecUnknown105431B4.z = READ_LONG();

	guess_iUnknown10542BC8 = 1;
	guess_vecUnknown105431B4 = render_origin;
	guess_flUnknown10542BCC = 2;
	guess_flUnknown10542BD4 = 2;
	guess_flUnknown10542BD0 = 8.5;
	guess_bUnknown10542BB3 = false;
	guess_bUnknown10559514 = false;

	if ( iEntity <= 0 )
	{
		if ( guess_pAttachCamEnt )
		{
			guess_iUnknown10542BC8 = 2;
			guess_vecUnknown105431B4 = guess_vecCameraAttachment;
			guess_flUnknown10542BCC = 2.5;
		}

		guess_pAttachCamEnt = NULL;
	}
	else
	{
		guess_pAttachCamEnt = gEngfuncs.GetEntityByIndex( iEntity );
		guess_vecUnknown105431CC = render_origin;
		gEngfuncs.GetViewAngles( guess_vecUnknown105431C0 );
	}

	return 1;
}

int CHud :: MsgFunc_BigPrick( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	READ_BYTE();
	return 1;
}

int CHud :: MsgFunc_NewFade( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	guess_m_iFadeR = READ_BYTE();
	guess_m_iFadeG = READ_BYTE();
	guess_m_iFadeB = READ_BYTE();
	guess_m_iFadeAlpha = READ_BYTE();
	guess_m_flFadeStart = gEngfuncs.GetClientTime();
	guess_m_flFadeTime = READ_COORD();

	int iFadeOut = READ_BYTE();
	int iHold = READ_BYTE();

	guess_m_flFadeEnd = gEngfuncs.GetClientTime() + READ_COORD();
	if ( iFadeOut )
		guess_m_bFadeOut = true;
	else
		guess_m_bFadeOut = false;
	if ( iHold )
		guess_m_bFadeHold = true;
	else
		guess_m_bFadeHold = false;

	return 1;
}

int CHud :: MsgFunc_SkipScene( const char *pszName, int iSize, void *pbuf )
{
	if ( guess_IsViewEntityActive() )
	{
		if ( gEngfuncs.pfnGetCvarFloat( "host_framerate" ) == 0 )
		{
			if ( strcmp( gEngfuncs.pfnGetLevelName(), "maps/video_stress_test.bsp" ) )
			{
				if ( !strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
					gEngfuncs.Cvar_SetValue( "host_framerate", guess_iSkipSceneFramerate );
			}
		}
	}

	return 1;
}

int CHud :: MsgFunc_IronSights( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	guess_m_vecIronSightOfs.x = READ_COORD();
	guess_m_vecIronSightOfs.y = READ_COORD();
	guess_m_vecIronSightOfs.z = READ_COORD();
	READ_BYTE();

	if ( READ_BYTE() )
	{
		guess_m_bIronSights = true;
	}
	else
	{
		guess_m_vecIronSightOfs.x = 0;
		guess_m_vecIronSightOfs.y = 0;
		guess_m_vecIronSightOfs.z = 0;
		guess_m_bIronSights = false;
	}

	return 1;
}

int CHud :: MsgFunc_Wheelchair( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iEntity = READ_SHORT();

	if ( READ_BYTE() )
	{
		guess_bWheelchairLock = true;
		return 1;
	}

	guess_iWheelchair = ( iEntity <= 0 ) ? 0 : (int)gEngfuncs.GetEntityByIndex( iEntity );
	guess_vecWheelchairAngles[1] = 0;
	guess_vecWheelchairAngles[0] = 0;

	return 1;
}

int CHud :: MsgFunc_GreyFade( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iFadeIn = READ_BYTE();
	guess_iGreyFade = READ_BYTE();
	guess_m_flGreyFadeStart = 0;
	guess_m_flGreyFadeEnd = 0;

	if ( iFadeIn )
		guess_m_flGreyFadeEnd = gEngfuncs.GetClientTime();
	else
		guess_m_flGreyFadeStart = gEngfuncs.GetClientTime();

	return 1;
}

int CHud :: MsgFunc_TakePhoto( const char *pszName, int iSize, void *pbuf )
{
	guess_g_bTakePhoto = true;
	return 1;
}

int CHud :: MsgFunc_Objectives( const char *pszName, int iSize, void *pbuf )
{
	char szText[256];

	BEGIN_READ( pbuf, iSize );

	strcpy( gHUD.guess_m_szObjectiveTitle, READ_STRING() );
	strcpy( gHUD.guess_m_szObjectiveText, READ_STRING() );

	if ( READ_BYTE() && strlen( gHUD.guess_m_szObjectiveTitle ) > 2 && strlen( gHUD.guess_m_szObjectiveText ) > 2 )
	{
		ParseHintTokens( gHUD.guess_m_szObjectiveTitle, gHUD.guess_m_szObjectiveText );

		gViewPort->guess_m_pHUDControl->guess_m_iHintLine = 1;

		if ( strlen( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1 ) > 2 )
		{
			int iLen = 0;
			for ( unsigned int i = 0; i < strlen( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1 ); i++ )
			{
				iLen++;
				if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[i] == '@' )
					szText[i] = ' ';
				else
					szText[i] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[i];
			}
			szText[iLen] = '\0';

			int w, h;
			gViewPort->guess_m_pHUDControl->guess_m_pHintLabel->setText( szText );
			gViewPort->guess_m_pHUDControl->guess_m_pCreditsFont->getTextSize( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1, w, h );
			gViewPort->guess_m_pHUDControl->guess_m_pHintLabel->setPos( ScreenWidth / 2 - 400, -48 );
			gViewPort->guess_m_pHUDControl->guess_m_bHint = true;
			gViewPort->guess_m_pHUDControl->guess_m_iHintAlpha = 255;
			gViewPort->guess_m_pHUDControl->guess_m_flHintTime = gEngfuncs.GetClientTime() + 5;
		}
	}

	return 1;
}

int CHud :: MsgFunc_Unlocks( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_m_iUnlocks = READ_LONG();
	return 1;
}

int CHud :: MsgFunc_CustomSlv( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	strncpy( guess_m_szSleeveTexture, READ_STRING(), 256 );

	if ( strlen( guess_m_szSleeveTexture ) <= 2 )
		guess_m_szSleeveTexture[0] = 0;

	guess_g_bSleeveLoaded = false;
	guess_g_bPoliceSleeveLoaded = false;

	return 1;
}

int CHud :: MsgFunc_CustomFng( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	strncpy( guess_m_szFingerTexture, READ_STRING(), 256 );
	guess_bCustomFingerTexture = true;

	if ( strlen( guess_m_szFingerTexture ) <= 2 )
	{
		guess_bCustomFingerTexture = false;
		guess_m_szFingerTexture[0] = 0;
	}

	guess_g_bHandsLoaded = false;

	return 1;
}

int CHud :: MsgFunc_CustomGlv( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	strncpy( guess_m_szGloveTexture, READ_STRING(), 256 );
	guess_bCustomGloveTexture = true;

	if ( strlen( guess_m_szGloveTexture ) <= 2 )
	{
		guess_bCustomGloveTexture = false;
		guess_m_szGloveTexture[0] = 0;
	}

	guess_g_bGloveLoaded = false;

	return 1;
}

int CHud :: MsgFunc_PlaySFX( const char *pszName, int iSize, void *pbuf )
{
	char szName[256];
	char szSound[256];

	BEGIN_READ( pbuf, iSize );

	char *pszSound = READ_STRING();
	int iType = READ_BYTE();
	int iFlag = READ_BYTE();
	float x = READ_COORD();
	float y = READ_COORD();
	float z = READ_COORD();
	int iEngine = READ_BYTE();
	READ_SHORT();

	vec3_t origin = Vector( x, y, z );

	if ( iEngine )
	{
		if ( strstr( pszSound, "weapons_new/" ) )
		{
			bool bFound = false;
			int j = 0;

			for ( unsigned int i = 0; i < strlen( pszSound ); i++ )
			{
				if ( bFound )
					szName[j++] = pszSound[i];
				else if ( pszSound[i] == '/' || pszSound[i] == '\\' )
					bFound = true;
			}

			if ( bFound )
			{
				szName[j] = '\0';
				strcpy( szSound, "weapons/" );
				strcat( szSound, szName );
				pszSound = szSound;
			}
		}

		if ( strstr( pszSound, "fire" ) || strstr( pszSound, "shoot" ) )
			gEngfuncs.pEventAPI->EV_PlaySound( 0, origin, CHAN_WEAPON, pszSound, 1.0, 0.8, 0, PITCH_NORM );
		else
			gEngfuncs.pEventAPI->EV_PlaySound( 0, origin, CHAN_AUTO, pszSound, 1.0, 0.8, 0, PITCH_NORM );

		return 1;
	}
	else
	{
		bool bFlag = false;
		if ( iFlag )
			bFlag = true;
		guess_IrrKlangPlaySFX( pszSound, iType, bFlag, origin );

		return 1;
	}
}

int CHud :: MsgFunc_Inventory( const char *pszName, int iSize, void *pbuf )
{
	char szItem[128];
	char szText[128];

	BEGIN_READ( pbuf, iSize );

	int iSlot = READ_BYTE();
	int iReset = READ_BYTE();
	strncpy( szItem, READ_STRING(), 128 );
	int iThree = READ_BYTE();
	int iSilent = READ_BYTE();
	int iTactical = READ_BYTE();

	strcpy( guess_GetInventoryItem( iSlot ), szItem );

	if ( !gViewPort || !gViewPort->guess_m_pRECBInventory )
		return 1;

	if ( iThree )
	{
		if ( gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[3]->isVisible() )
			gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[3]->setVisible( false );
		if ( gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[4]->isVisible() )
			gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[4]->setVisible( false );
		if ( gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[5]->isVisible() )
			gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[5]->setVisible( false );

		if ( gViewPort->guess_m_pRECBInventory->guess_m_iThree == 0 )
		{
			gViewPort->guess_m_pRECBInventory->guess_m_pInventory->setImage( vgui_LoadTGA( "gfx/vgui/640_inventory_three.tga" ) );
			gViewPort->guess_m_pRECBInventory->guess_m_iThree = 1;
		}
	}
	else
	{
		if ( !gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[3]->isVisible() )
			gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[3]->setVisible( true );
		if ( !gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[4]->isVisible() )
			gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[4]->setVisible( true );
		if ( !gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[5]->isVisible() )
			gViewPort->guess_m_pRECBInventory->guess_m_pItemButtons[5]->setVisible( true );

		if ( gViewPort->guess_m_pRECBInventory->guess_m_iThree == 1 )
		{
			gViewPort->guess_m_pRECBInventory->guess_m_pInventory->setImage( vgui_LoadTGA( "gfx/vgui/640_inventory.tga" ) );
			gViewPort->guess_m_pRECBInventory->guess_m_iThree = 0;
		}
	}

	if ( iReset )
		gViewPort->guess_m_pRECBInventory->guess_Reset();

	if ( iSlot )
	{
		if ( strlen( gHUD.guess_GetInventoryItem( iSlot ) ) > 1 )
		{
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[iSlot - 1]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", guess_GetInventoryItem( iSlot ) ) ) );

			char *pszNiceName = CheckItem( "nice_name", szItem );
			if ( pszNiceName )
			{
				int iNotBook = strcmp( pszNiceName, "Simon's Book" );
				sprintf( szText, iNotBook ? "You got the %s" : "You got %s", CheckItem( "nice_name", szItem ) );

				if ( gViewPort && gViewPort->guess_m_pHUDControl )
				{
					gViewPort->guess_m_pHUDControl->guess_m_iMessageTime = gEngfuncs.GetClientTime() + 4;

					if ( !iSilent )
					{
						strcpy( gViewPort->guess_m_pHUDControl->guess_m_szMessage, szText );
						gViewPort->guess_m_pHUDControl->guess_m_flMessageAlpha = 255;
						gViewPort->guess_m_pHUDControl->guess_m_bMessageFadeOut = false;
						gViewPort->guess_m_pHUDControl->guess_m_bMessageHold = false;
						gViewPort->guess_m_pHUDControl->guess_m_flMessageHoldTime = 0;
						gViewPort->guess_m_pHUDControl->guess_m_iMessageType = 5;
					}
				}
			}
		}
		else
		{
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[iSlot - 1]->setImage( NULL );
		}
	}

	if ( iTactical )
	{
		for ( int i = 1; i <= 6; i++ )
		{
			if ( !strcmp( "inventoryitems/weapons/weapon_glock.txt", guess_GetInventoryItem( i ) ) )
			{
				strcpy( guess_GetInventoryItem( i ), "inventoryitems/weapons/weapon_glocktac.txt" );
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[i - 1]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", guess_GetInventoryItem( i ) ) ) );
			}
		}

		if ( !strcmp( "inventoryitems/weapons/weapon_glock.txt", gHUD.guess_m_szInventoryWeapon1 ) )
		{
			strcpy( gHUD.guess_m_szInventoryWeapon1, "inventoryitems/weapons/weapon_glocktac.txt" );
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[6]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon1 ) ) );
		}

		if ( !strcmp( "inventoryitems/weapons/weapon_glock.txt", gHUD.guess_m_szInventoryWeapon2 ) )
		{
			strcpy( gHUD.guess_m_szInventoryWeapon2, "inventoryitems/weapons/weapon_glocktac.txt" );
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[7]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon2 ) ) );
		}

		if ( !strcmp( "inventoryitems/weapons/weapon_glock.txt", gHUD.guess_m_szInventoryWeapon3 ) )
		{
			strcpy( gHUD.guess_m_szInventoryWeapon3, "inventoryitems/weapons/weapon_glocktac.txt" );
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[9]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon3 ) ) );
		}
	}
	else
	{
		for ( int i = 1; i <= 6; i++ )
		{
			if ( !strcmp( "inventoryitems/weapons/weapon_glocktac.txt", guess_GetInventoryItem( i ) ) )
			{
				strcpy( guess_GetInventoryItem( i ), "inventoryitems/weapons/weapon_glock.txt" );
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[i - 1]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", guess_GetInventoryItem( i ) ) ) );
			}
		}

		if ( !strcmp( "inventoryitems/weapons/weapon_glocktac.txt", gHUD.guess_m_szInventoryWeapon1 ) )
		{
			strcpy( gHUD.guess_m_szInventoryWeapon1, "inventoryitems/weapons/weapon_glock.txt" );
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[6]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon1 ) ) );
		}

		if ( !strcmp( "inventoryitems/weapons/weapon_glocktac.txt", gHUD.guess_m_szInventoryWeapon2 ) )
		{
			strcpy( gHUD.guess_m_szInventoryWeapon2, "inventoryitems/weapons/weapon_glock.txt" );
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[7]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon2 ) ) );
		}

		// original bug: tests the second quick slot again
		if ( !strcmp( "inventoryitems/weapons/weapon_glocktac.txt", gHUD.guess_m_szInventoryWeapon2 ) )
		{
			strcpy( gHUD.guess_m_szInventoryWeapon3, "inventoryitems/weapons/weapon_glock.txt" );
			gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[9]->setImage( vgui_LoadTGA( CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon3 ) ) );
		}
	}

	return 1;
}

int CHud :: MsgFunc_Materials( const char *pszName, int iSize, void *pbuf )
{
	iClientResetMaterials = 1;
	return 1;
}

bool strstrtolower( char *path, char *convert )
{
	char szLowerGay[256];
	strncpy( szLowerGay, convert, 256 );

	for ( int k = 0; szLowerGay[k]; k++ )
		szLowerGay[k] = tolower( szLowerGay[k] );

	if ( strstr( path, szLowerGay ) )
		return true;

	return false;
}
