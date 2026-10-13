// ======================================
// Cry of Fear HUD panel
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "const.h"
#include "entity_types.h"
#include "cdll_int.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_HUDControl.h"
#include "..\game_shared\vgui_loadtga.h"
#include "windows.h"

#include <string.h>
#include <stdio.h>

// a method of CHud in 1.6a (called with ecx = &gHUD): the key bound to a command, read from config.cfg

extern bool		guess_g_bMPModel;
extern bool		guess_bWheelchairLock;
extern int		guess_g_iDickMode;
extern int		guess_iWheelchair;

int		guess_g_iLetterboxHeight;
float	guess_g_flLookingAt;
int		guess_g_iSimonMode;


int guess_MsgStamina( const char *pszName, int iSize, void *pbuf )
{
	if ( gViewPort->guess_m_pHUDControl )
	{
		BEGIN_READ( pbuf, iSize );
		gViewPort->guess_m_pHUDControl->guess_m_iStamina = READ_BYTE();
		float flTime = READ_COORD();
		if ( flTime > 0 )
			gViewPort->guess_m_pHUDControl->guess_m_flStaminaFlashTime = gEngfuncs.GetClientTime() + flTime;
	}

	return 1;
}

int guess_MsgCoFLogo( const char *pszName, int iSize, void *pbuf )
{
	if ( !gViewPort->guess_m_pHUDControl )
		return 1;

	BEGIN_READ( pbuf, iSize );
	gViewPort->guess_m_pHUDControl->guess_m_iLogo = READ_BYTE();
	gViewPort->guess_m_pHUDControl->guess_m_iLogoAlpha = 255;
	gViewPort->guess_m_pHUDControl->guess_m_bLogoF5D = false;
	gViewPort->guess_m_pHUDControl->guess_m_flLogoTime = 0;

	switch ( gViewPort->guess_m_pHUDControl->guess_m_iLogo )
	{
	case 0:
		gViewPort->guess_m_pHUDControl->guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_logo.tga" );
		break;
	case 1:
		gViewPort->guess_m_pHUDControl->guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_andreas.tga" );
		break;
	case 2:
		gViewPort->guess_m_pHUDControl->guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_james.tga" );
		break;
	case 3:
		gViewPort->guess_m_pHUDControl->guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_jordy.tga" );
		break;
	case 4:
		gViewPort->guess_m_pHUDControl->guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_stig.tga" );
		break;
	case 5:
		gViewPort->guess_m_pHUDControl->guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_lasse.tga" );
		break;
	case 6:
		gViewPort->guess_m_pHUDControl->guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_aina.tga" );
		break;
	}

	gViewPort->guess_m_pHUDControl->guess_m_pLogo = new ImagePanel( gViewPort->guess_m_pHUDControl->guess_m_pLogoImage );
	gViewPort->guess_m_pHUDControl->guess_m_pLogo->setParent( gViewPort->guess_m_pHUDControl );
	int x = ScreenWidth / 2 - gViewPort->guess_m_pHUDControl->guess_m_pLogo->getWide() / 2;
	int y = ScreenHeight / 2 - gViewPort->guess_m_pHUDControl->guess_m_pLogo->getTall() / 2;
	gViewPort->guess_m_pHUDControl->guess_m_pLogo->setPos( x, y );
	gViewPort->guess_m_pHUDControl->guess_m_pLogoImage->setColor( Color( 255, 255, 255, 255 ) );

	if ( gViewPort->guess_m_pHUDControl->guess_m_iLogo == 0 )
		gViewPort->guess_m_pHUDControl->guess_m_flLogoTime = gEngfuncs.GetClientTime() + 5;
	else
		gViewPort->guess_m_pHUDControl->guess_m_flLogoTime = gEngfuncs.GetClientTime() + 3;

	gViewPort->guess_m_pHUDControl->guess_m_bShowLogo = true;

	return 1;
}

int guess_MsgShowHint( const char *pszName, int iSize, void *pbuf )
{
	char szImage[256];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pHUDControl )
	{
		strcpy( szImage, READ_STRING() );

		if ( strlen( szImage ) > 1 )
		{
			gViewPort->guess_m_pHUDControl->guess_m_pInventoryHint->setImage( vgui_LoadTGA( szImage ) );
			gViewPort->guess_m_pHUDControl->guess_m_pInventoryHint->setPos( gViewPort->guess_m_pHUDControl->guess_m_pInventoryHint->getWide() * 1.2 - gViewPort->guess_m_pHUDControl->guess_m_pInventoryHint->getWide(), gViewPort->guess_m_pHUDControl->guess_m_iInventoryHintY );
			gViewPort->guess_m_pHUDControl->guess_m_bInventoryHint = true;
			gViewPort->guess_m_pHUDControl->guess_m_flHintSlideInTime = gEngfuncs.GetClientTime();
			gViewPort->guess_m_pHUDControl->guess_m_flHintSlideInLength = 1;
			gViewPort->guess_m_pHUDControl->guess_m_flHintSlideOutTime = gEngfuncs.GetClientTime() + 15;
			gViewPort->guess_m_pHUDControl->guess_m_flHintSlideOutLength = 1;
		}
	}

	return 1;
}

int guess_MsgCameraZoom( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	gHUD.guess_m_flCameraZoomFOV = READ_COORD();
	gHUD.guess_m_iCameraZoom = READ_BYTE();
	gHUD.guess_m_bCameraZoomOn = READ_BYTE() != 0;

	if ( gHUD.guess_m_iCameraZoom )
		gHUD.guess_m_flNextSwayTime = gHUD.m_flTime;

	return 1;
}

int guess_MsgHealthCap( const char *pszName, int iSize, void *pbuf )
{
	if ( gViewPort->guess_m_pHUDControl )
	{
		BEGIN_READ( pbuf, iSize );
		int iType = READ_BYTE();

		switch ( iType )
		{
		case 0:
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBarFull->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar_bar.tga" ) );
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBar->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar.tga" ) );
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBarEmpty->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar_barempty.tga" ) );
			break;
		case 1:
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBarFull->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar_bar2.tga" ) );
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBar->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar2.tga" ) );
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBarEmpty->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar_barempty2.tga" ) );
			break;
		case 2:
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBarFull->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar_bar3.tga" ) );
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBar->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar3.tga" ) );
			gViewPort->guess_m_pHUDControl->guess_m_pHealthBarEmpty->setImage( vgui_LoadTGA( "gfx/vgui/640_healthbar_barempty3.tga" ) );
			break;
		}
	}

	return 1;
}

int guess_MsgChapter( const char *pszName, int iSize, void *pbuf )
{
	char szSound[256];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pHUDControl )
	{
		strcpy( gViewPort->guess_m_pHUDControl->guess_m_szChapter, READ_STRING() );
		gViewPort->guess_m_pHUDControl->guess_m_flChapterAlpha = 255;
		gViewPort->guess_m_pHUDControl->guess_m_bChapterFadeOut = false;
		gViewPort->guess_m_pHUDControl->guess_m_bChapterF1E = false;
		gViewPort->guess_m_pHUDControl->guess_m_flChapterHoldTime = 0;
		gViewPort->guess_m_pHUDControl->guess_m_flChapterNextChar = 0;
		gViewPort->guess_m_pHUDControl->guess_m_iChapterChars = 0;

		strncpy( szSound, READ_STRING(), 256 );
		if ( strlen( szSound ) > 1 )
			gEngfuncs.pfnPlaySoundByName( szSound, 1 );
	}

	return 1;
}

int guess_MsgControlHint( const char *pszName, int iSize, void *pbuf )
{
	char szName[256];
	char szFile[256];
	char szText[256];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pHUDControl )
	{
		strncpy( szName, READ_STRING(), 256 );
		strncpy( szFile, READ_STRING(), 256 );
		ParseHintTokens( szName, szFile );

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

int guess_MsgDevDonator( const char *pszName, int iSize, void *pbuf )
{
	if ( gViewPort->guess_m_pHUDControl )
	{
		BEGIN_READ( pbuf, iSize );
		int iPlayer = READ_BYTE();
		gViewPort->guess_m_pHUDControl->guess_m_iDevDonator[iPlayer] = READ_BYTE();
	}

	return 1;
}

int guess_MsgHoldBreath( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	gHUD.guess_m_iHoldBreath = READ_BYTE();

	return 1;
}

int guess_MsgIntrodn( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iType = READ_BYTE();
	READ_BYTE();
	int iCoop = READ_BYTE();

	if ( !gViewPort || !gViewPort->guess_m_pHUDControl )
		return 1;

	int iLanguage;
	if ( iCoop )
		iLanguage = CVAR_GET_FLOAT( "cl_coop_language" );
	else
		iLanguage = CVAR_GET_FLOAT( "cof_subtitlelanguage" );

	if ( !gViewPort->guess_m_pHUDControl->guess_m_pIntroduction->isVisible() )
	{
		gViewPort->guess_m_pHUDControl->guess_m_iIntroductionAlpha = 255;
		gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage->setColor( Color( 255, 255, 255, 255 ) );
		gViewPort->guess_m_pHUDControl->guess_m_bIntroductionFadeIn = true;

		switch ( iLanguage )
		{
		case 0:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/english/english_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/english/english_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/english/english_coop.TGA" );
			break;
		case 2:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/dutch/dutch_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/dutch/dutch_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/dutch/dutch_coop.TGA" );
			break;
		case 3:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/french/french_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/french/french_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/french/french_coop.TGA" );
			break;
		case 4:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/german/german_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/german/german_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/german/german_coop.TGA" );
			break;
		case 5:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/norwegian/norwegian_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/norwegian/norwegian_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/norwegian/norwegian_coop.TGA" );
			break;
		case 6:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/spanish/spanish_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/spanish/spanish_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/spanish/spanish_coop.TGA" );
			break;
		case 7:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/swedish/swedish_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/swedish/swedish_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/swedish/swedish_coop.TGA" );
			break;
		default:
			if ( iType == 0 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/english/english_sp.TGA" );
			else if ( iType == 1 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/english/english_doc.TGA" );
			else if ( iType == 2 )
				gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/english/english_coop.TGA" );
			break;
		}

		gViewPort->guess_m_pHUDControl->guess_m_pIntroduction->setImage( gViewPort->guess_m_pHUDControl->guess_m_pIntroductionImage );
		gViewPort->guess_m_pHUDControl->guess_m_pIntroduction->setVisible( true );
	}
	else
		gViewPort->guess_m_pHUDControl->guess_m_bIntroductionFadeIn = false;

	return 1;
}

int guess_MsgLobby( const char *pszName, int iSize, void *pbuf )
{
	if ( gViewPort->guess_m_pHUDControl )
	{
		BEGIN_READ( pbuf, iSize );
		int iPlayer = READ_BYTE();
		int iReady = READ_BYTE();
		int iLobby = READ_BYTE();
		int i;

		if ( iLobby )
		{
			gViewPort->guess_m_pHUDControl->guess_m_bLobby = false;
			gViewPort->guess_m_pHUDControl->guess_m_pVoteLabel->setVisible( false );
			gViewPort->guess_m_pHUDControl->guess_m_pVoteKickLabel->setVisible( false );
			for ( i = 0; i < 4; i++ )
				gViewPort->guess_m_pHUDControl->guess_m_pLobbyLabel[i]->setVisible( false );
			gViewPort->guess_m_pHUDControl->guess_m_pLobbyBackground->setVisible( false );
		}
		else
		{
			gViewPort->guess_m_pHUDControl->guess_m_bLobby = true;
			gViewPort->guess_m_pHUDControl->guess_m_pVoteLabel->setVisible( true );
			gViewPort->guess_m_pHUDControl->guess_m_pVoteKickLabel->setVisible( true );
			for ( i = 0; i < 4; i++ )
				gViewPort->guess_m_pHUDControl->guess_m_pLobbyLabel[i]->setVisible( true );
			gViewPort->guess_m_pHUDControl->guess_m_pLobbyBackground->setImage( vgui_LoadTGA( "gfx/vgui/lobby_background.tga" ) );
			gViewPort->guess_m_pHUDControl->guess_m_pLobbyBackground->setSize( 800, 600 );
			gViewPort->guess_m_pHUDControl->guess_m_pLobbyBackground->setPos( ScreenWidth / 2 - 400, ScreenHeight / 2 - 300 );
			gViewPort->guess_m_pHUDControl->guess_m_pLobbyBackground->setVisible( true );
		}

		int iIndex = iPlayer - 1;
		if ( iIndex >= 0 && iIndex < 4 )
		{
			if ( iReady )
				gViewPort->guess_m_pHUDControl->guess_m_bPlayerReady[iIndex] = true;
			else
				gViewPort->guess_m_pHUDControl->guess_m_bPlayerReady[iIndex] = false;
		}
	}

	return 1;
}

int guess_MsgLookingAt( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_g_flLookingAt = READ_BYTE();

	return 1;
}

int guess_MsgMPModel( const char *pszName, int iSize, void *pbuf )
{
	guess_g_bMPModel = false;

	return 1;
}

int guess_MsgSimonMode( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_g_iSimonMode = READ_BYTE();

	return 1;
}

int guess_MsgTimer( const char *pszName, int iSize, void *pbuf )
{
	if ( gViewPort->guess_m_pHUDControl )
	{
		BEGIN_READ( pbuf, iSize );
		int iMode = READ_BYTE();
		int iSeconds = READ_BYTE();
		int iMinutes = READ_BYTE();
		int iHours = READ_BYTE();
		int iLimit = READ_SHORT();

		if ( iMode == 0 )
		{
			gViewPort->guess_m_pHUDControl->guess_m_bTimer = false;
			gViewPort->guess_m_pHUDControl->guess_m_flTimerNextSecond = gEngfuncs.GetClientTime();
			gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds = 0;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerMinutes = 0;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerHours = 0;
		}
		else if ( iMode == 1 )
		{
			gViewPort->guess_m_pHUDControl->guess_m_bTimer = true;
			gViewPort->guess_m_pHUDControl->guess_m_flTimerNextSecond = gEngfuncs.GetClientTime();
			gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds = iSeconds;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerMinutes = iMinutes;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerHours = iHours;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerLimit = iLimit;
		}
		else if ( iMode == 2 )
		{
			gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds = iSeconds;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerMinutes = iMinutes;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerHours = iHours;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerLimit = iLimit;
		}
	}

	return 1;
}

int guess_MsgTimerPlus( const char *pszName, int iSize, void *pbuf )
{
	char szSeconds[32];
	char szMinutes[32];
	char szHours[32];
	char szTime[128];

	BEGIN_READ( pbuf, iSize );
	int iSeconds = READ_BYTE();
	int iShow = READ_BYTE();
	int iMode = READ_BYTE();

	if ( gViewPort && gViewPort->guess_m_pHUDControl )
	{
		switch ( iMode )
		{
		case 1:
			gViewPort->guess_m_pHUDControl->guess_m_bTimerPaused = true;
			return 1;
		case 2:
			gViewPort->guess_m_pHUDControl->guess_m_bTimerPaused = false;
			break;
		case 3:
			gViewPort->guess_m_pHUDControl->guess_m_bTimerPaused = true;
			break;
		}

		gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds += iSeconds;

		if ( gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds > 59 )
		{
			gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds = ( gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds > 60 ) ? gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds - 60 : 0;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerMinutes++;
		}

		if ( gViewPort->guess_m_pHUDControl->guess_m_iTimerMinutes > 59 )
		{
			gViewPort->guess_m_pHUDControl->guess_m_iTimerHours++;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds = 0;
			gViewPort->guess_m_pHUDControl->guess_m_iTimerMinutes = 0;
		}

		int iSecondsLeft = 60 - gViewPort->guess_m_pHUDControl->guess_m_iTimerSeconds;
		int iMinutesLeft = gViewPort->guess_m_pHUDControl->guess_m_iTimerLimit / 60 - gViewPort->guess_m_pHUDControl->guess_m_iTimerMinutes - 1;

		if ( iMinutesLeft < 0 )
			iMinutesLeft = 0;
		if ( iSecondsLeft < 0 )
			iSecondsLeft = 0;
		if ( gViewPort->guess_m_pHUDControl->guess_m_iTimerHours < 0 )
			gViewPort->guess_m_pHUDControl->guess_m_iTimerHours = 0;

		sprintf( szSeconds, ( iSecondsLeft >= 10 ) ? "%i" : "0%i", iSecondsLeft );
		sprintf( szMinutes, ( iMinutesLeft >= 10 ) ? "%i" : "0%i", iMinutesLeft );
		sprintf( szHours, ( gViewPort->guess_m_pHUDControl->guess_m_iTimerHours >= 10 ) ? "%i" : "0%i", gViewPort->guess_m_pHUDControl->guess_m_iTimerHours );
		sprintf( szTime, "%s:%s:%s", szHours, szMinutes, szSeconds );
		gViewPort->guess_m_pHUDControl->guess_m_pTimerLabel->setText( szTime );
		gViewPort->guess_m_pHUDControl->guess_m_flTimerNextSecond = gEngfuncs.GetClientTime() + 1;
		gEngfuncs.pfnPlaySoundByName( "common/survival_timeplus.wav", 2 );

		if ( iShow )
		{
			for ( int i = 0; i < 32; i++ )
			{
				if ( gViewPort && !gViewPort->guess_m_pHUDControl->guess_m_pTimerPlusLabel[i]->isVisible() )
				{
					int w, h;
					gViewPort->guess_m_pHUDControl->guess_m_pCreditsFont->getTextSize( szTime, w, h );
					gViewPort->guess_m_pHUDControl->guess_m_pTimerPlusLabel[i]->setFgColor( 0, 128, 0, 0 );
					gViewPort->guess_m_pHUDControl->guess_m_pTimerPlusLabel[i]->setPos( ScreenWidth / 2 + w / 2, 0 );
					gViewPort->guess_m_pHUDControl->guess_m_pTimerLabel->setFgColor( 128, 255, 128, 0 );
					sprintf( szTime, "+%i", iSeconds );
					gViewPort->guess_m_pHUDControl->guess_m_pTimerPlusLabel[i]->setText( szTime );
					gViewPort->guess_m_pHUDControl->guess_m_pTimerPlusLabel[i]->setVisible( true );
					break;
				}
			}
		}
	}

	return 1;
}

void ParseHintTokens( char *pszName, char *pszFile )
{
	char szBefore1[256];
	char szBefore2[256];
	char szBefore3[256];
	char token[1024];
	char szAfter[256];
	bool bFound;
	int i, j;

	if ( !gViewPort->guess_m_pHUDControl )
		return;

	gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[0] = '\0';
	gViewPort->guess_m_pHUDControl->guess_m_szHintLine2[0] = '\0';
	gViewPort->guess_m_pHUDControl->guess_m_szHintLine3[0] = '\0';
	gViewPort->guess_m_pHUDControl->guess_m_szHintNoButton[0] = '\0';
	gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton[0] = '\0';

	char *pfile = (char *)gEngfuncs.COM_LoadFile( pszFile, 5, NULL );
	if ( !pfile )
	{
		gEngfuncs.Con_Printf( "ParseHintTokens Couldn't open file %s\n", pszFile );
		return;
	}

	while ( pfile = gEngfuncs.COM_ParseFile( pfile, token ) )
	{
		if ( _stricmp( token, pszName ) )
			continue;

		pfile = gEngfuncs.COM_ParseFile( pfile, token );
		if ( _stricmp( token, "{" ) )
		{
			gEngfuncs.Con_Printf( "ParseHintTokens Error parsing file %s. (expected { )\n", pszFile );
			return;
		}

		pfile = gEngfuncs.COM_ParseFile( pfile, token );
		while ( _stricmp( token, "}" ) )
		{
			if ( !_stricmp( token, "line1" ) )
			{
				pfile = gEngfuncs.COM_ParseFile( pfile, token );
				strncpy( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1, token, 256 );
			}
			else if ( !_stricmp( token, "line2" ) )
			{
				pfile = gEngfuncs.COM_ParseFile( pfile, token );
				strncpy( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2, token, 256 );
			}
			else if ( !_stricmp( token, "line3" ) )
			{
				pfile = gEngfuncs.COM_ParseFile( pfile, token );
				strncpy( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3, token, 256 );
			}
			else if ( !_stricmp( token, "nobutton" ) )
			{
				pfile = gEngfuncs.COM_ParseFile( pfile, token );
				strncpy( gViewPort->guess_m_pHUDControl->guess_m_szHintNoButton, token, 256 );
			}
			else if ( !_stricmp( token, "cfgbutton" ) )
			{
				pfile = gEngfuncs.COM_ParseFile( pfile, token );
				strncpy( gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton, token, 64 );
			}

			pfile = gEngfuncs.COM_ParseFile( pfile, token );
		}
	}

	// pfile is NULL here: the loaded file is never freed
	gEngfuncs.COM_FreeFile( pfile );

	if ( strlen( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1 ) > 2 )
	{
		bFound = false;
		j = 0;
		for ( i = 0; i < 256; i++ )
		{
			if ( !bFound )
			{
				if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[i] == '*' )
				{
					szBefore1[i] = '\0';
					bFound = true;
				}
				else
					szBefore1[i] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[i];
			}
			else
			{
				if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[i] != '\0' )
					szAfter[j++] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[i];
				else
					szAfter[j++] = '\0';
			}
		}

		strcpy( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1, szBefore1 );

		if ( bFound )
		{
			if ( gHUD.guess_GetKeyForCommand( gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton ) )
				strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1, gHUD.guess_GetKeyForCommand( gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton ) );
			else
				strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1, gViewPort->guess_m_pHUDControl->guess_m_szHintNoButton );
			strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1, szAfter );
		}
	}

	if ( strlen( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2 ) > 2 )
	{
		bFound = false;
		j = 0;
		for ( i = 0; i < 256; i++ )
		{
			if ( !bFound )
			{
				if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2[i] == '*' )
				{
					szBefore2[i] = '\0';
					bFound = true;
				}
				else
					szBefore2[i] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine2[i];
			}
			else
			{
				if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2[i] != '\0' )
					szAfter[j++] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine2[i];
				else
					szAfter[j++] = '\0';
			}
		}

		strcpy( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2, szBefore2 );

		if ( bFound )
		{
			if ( gHUD.guess_GetKeyForCommand( gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton ) )
				strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2, gHUD.guess_GetKeyForCommand( gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton ) );
			else
				strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2, gViewPort->guess_m_pHUDControl->guess_m_szHintNoButton );
			strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine2, szAfter );
		}
	}

	if ( strlen( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3 ) > 2 )
	{
		bFound = false;
		j = 0;
		for ( i = 0; i < 256; i++ )
		{
			if ( !bFound )
			{
				if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3[i] == '*' )
				{
					szBefore3[i] = '\0';
					bFound = true;
				}
				else
					szBefore3[i] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine3[i];
			}
			else
			{
				if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3[i] != '\0' )
					szAfter[j++] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine3[i];
				else
					szAfter[j++] = '\0';
			}
		}

		strcpy( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3, szBefore3 );

		if ( bFound )
		{
			if ( gHUD.guess_GetKeyForCommand( gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton ) )
				strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3, gHUD.guess_GetKeyForCommand( gViewPort->guess_m_pHUDControl->guess_m_szHintCfgButton ) );
			else
				strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3, gViewPort->guess_m_pHUDControl->guess_m_szHintNoButton );
			strcat( gViewPort->guess_m_pHUDControl->guess_m_szHintLine3, szAfter );
		}
	}
}

void guess_HUDControlInit()
{
	gEngfuncs.pfnHookUserMsg( "Stamina", guess_MsgStamina );
	gEngfuncs.pfnHookUserMsg( "CoFLogo", guess_MsgCoFLogo );
	gEngfuncs.pfnHookUserMsg( "ShowHint", guess_MsgShowHint );
	gEngfuncs.pfnHookUserMsg( "CameraZoom", guess_MsgCameraZoom );
	gEngfuncs.pfnHookUserMsg( "HealthCap", guess_MsgHealthCap );
	gEngfuncs.pfnHookUserMsg( "Chapter", guess_MsgChapter );
	gEngfuncs.pfnHookUserMsg( "ControlHint", guess_MsgControlHint );
	gEngfuncs.pfnHookUserMsg( "Timer", guess_MsgTimer );
	gEngfuncs.pfnHookUserMsg( "Lobby", guess_MsgLobby );
	gEngfuncs.pfnHookUserMsg( "MPModel", guess_MsgMPModel );
	gEngfuncs.pfnHookUserMsg( "HoldBreath", guess_MsgHoldBreath );
	gEngfuncs.pfnHookUserMsg( "DevDonator", guess_MsgDevDonator );
	gEngfuncs.pfnHookUserMsg( "Introdn", guess_MsgIntrodn );
	gEngfuncs.pfnHookUserMsg( "LookingAt", guess_MsgLookingAt );
	gEngfuncs.pfnHookUserMsg( "TimerPlus", guess_MsgTimerPlus );
	gEngfuncs.pfnHookUserMsg( "SimonMode", guess_MsgSimonMode );
}

CHUDControl::CHUDControl() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int i;

	guess_m_pLobbyBackground = new ImagePanel( vgui_LoadTGA( "gfx/vgui/vignette/vignette_tl.tga" ) );
	guess_m_pLobbyBackground->setParent( this );
	guess_m_pLobbyBackground->setPos( ScreenWidth / 2 - 400, ScreenHeight / 2 - 300 );
	guess_m_pLobbyBackground->setVisible( true );

	guess_m_pVignetteTL = new ImagePanel( vgui_LoadTGA( "gfx/vgui/vignette/vignette_tl.tga" ) );
	guess_m_pVignetteTL->setParent( this );
	guess_m_pVignetteTL->setPos( 0, 0 );
	guess_m_pVignetteTL->setVisible( false );

	guess_m_pVignetteTR = new ImagePanel( vgui_LoadTGA( "gfx/vgui/vignette/vignette_tr.tga" ) );
	guess_m_pVignetteTR->setParent( this );
	guess_m_pVignetteTR->setPos( ScreenWidth - 256, 0 );
	guess_m_pVignetteTR->setVisible( false );

	guess_m_pVignetteBL = new ImagePanel( vgui_LoadTGA( "gfx/vgui/vignette/vignette_bl.tga" ) );
	guess_m_pVignetteBL->setParent( this );
	guess_m_pVignetteBL->setPos( 0, ScreenHeight - 256 );
	guess_m_pVignetteBL->setVisible( false );

	guess_m_pVignetteBR = new ImagePanel( vgui_LoadTGA( "gfx/vgui/vignette/vignette_br.tga" ) );
	guess_m_pVignetteBR->setParent( this );
	guess_m_pVignetteBR->setPos( ScreenWidth - 256, ScreenHeight - 256 );
	guess_m_pVignetteBR->setVisible( false );

	if ( guess_m_pSMSIconImage = vgui_LoadTGA( "gfx/vgui/640_hudelem_smsicon.tga" ) )
		guess_m_pSMSIconImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_hudelem_smsicon.tga!\n" );

	if ( guess_m_pInventoryHintImage = vgui_LoadTGA( "gfx/vgui/hints/hint_inventory.tga" ) )
		guess_m_pInventoryHintImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/hints/hint_inventory.tga!\n" );

	if ( guess_m_pCrosshairImage = vgui_LoadTGA( "gfx/vgui/crosshair.tga" ) )
		guess_m_pCrosshairImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/crosshair.tga!\n" );

	if ( guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/640_logo_logo.tga" ) )
		guess_m_pLogoImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_logo_logo.tga!\n" );

	if ( guess_m_pHealthBarImage = vgui_LoadTGA( "gfx/vgui/640_healthbar.tga" ) )
		guess_m_pHealthBarImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_healthbar.tga!\n" );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hCreditsScheme = pSchemes->getSchemeHandle( "credits" );
	SchemeHandle_t hChapterTitleScheme = pSchemes->getSchemeHandle( "chaptertitle" );
	SchemeHandle_t hPagerScheme = pSchemes->getSchemeHandle( "pager" );
	Font *pFont = pSchemes->getFont( hCreditsScheme );
	guess_m_pPagerFont = pSchemes->getFont( hPagerScheme );
	guess_m_pCreditsFont = pSchemes->getFont( hCreditsScheme );
	guess_m_pChapterTitleFont = pSchemes->getFont( hChapterTitleScheme );

	if ( guess_m_pHealthBarFullImage = vgui_LoadTGA( "gfx/vgui/640_healthbar_bar.tga" ) )
		guess_m_pHealthBarFullImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_healthbar_bar.tga!\n" );

	if ( guess_m_pHealthBarEmptyImage = vgui_LoadTGA( "gfx/vgui/640_healthbar_barempty.tga" ) )
		guess_m_pHealthBarEmptyImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_healthbar_barempty.tga!\n" );

	if ( guess_m_pStaminaBarImage = vgui_LoadTGA( "gfx/vgui/640_staminabar.tga" ) )
		guess_m_pStaminaBarImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_staminabar.tga!\n" );

	if ( guess_m_pStaminaBarFullImage = vgui_LoadTGA( "gfx/vgui/640_staminabar_bar.tga" ) )
		guess_m_pStaminaBarFullImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_staminabar_bar.tga!\n" );

	if ( guess_m_pStaminaBarEmptyImage = vgui_LoadTGA( "gfx/vgui/640_staminabar_barempty.tga" ) )
		guess_m_pStaminaBarEmptyImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_staminabar_barempty.tga!\n" );

	if ( guess_m_pHintBarImage = vgui_LoadTGA( "gfx/vgui/640_hint_bar.tga" ) )
		guess_m_pHintBarImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_hint_bar.tga!\n" );

	if ( guess_m_pIntroductionImage = vgui_LoadTGA( "gfx/vgui/introductions/english/english_sp.tga" ) )
		guess_m_pIntroductionImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/introductions/english/english_sp.tga!\n" );

	for ( i = 0; i < 4; i++ )
	{
		if ( guess_m_pFollowIcon[i] = vgui_LoadTGA( "gfx/vgui/coop_follow_icon.tga" ) )
			guess_m_pFollowIcon[i]->setColor( Color( 255, 255, 255, 1 ) );
		else
			gEngfuncs.Con_Printf( "Cannot load gfx/vgui/coop_follow_icon.tga %i!\n", i );

		if ( guess_m_pSyringeIcon[i] = vgui_LoadTGA( "gfx/vgui/coop_syringe_icon.tga" ) )
			guess_m_pSyringeIcon[i]->setColor( Color( 255, 255, 255, 1 ) );
		else
			gEngfuncs.Con_Printf( "Cannot load gfx/vgui/coop_syringe_icon.tga %i!\n", i );
	}

	setBgColor( 0, 0, 0, 255 );

	guess_m_pIntroduction = new ImagePanel( guess_m_pIntroductionImage );
	guess_m_pIntroduction->setParent( this );
	guess_m_pIntroduction->setPos( ScreenWidth / 2 - 256, ScreenHeight / 2 - 256 );
	guess_m_pIntroduction->setVisible( false );
	guess_m_pIntroductionImage->setColor( Color( 255, 255, 255, guess_m_iIntroductionAlpha ) );

	guess_m_pSMSIcon = new ImagePanel( guess_m_pSMSIconImage );
	guess_m_pSMSIcon->setParent( this );
	guess_m_pSMSIcon->setPos( guess_m_pSMSIcon->getWide(), guess_m_pSMSIcon->getTall() );
	guess_m_pSMSIcon->setVisible( false );

	guess_m_pInventoryHint = new ImagePanel( guess_m_pInventoryHintImage );
	guess_m_pInventoryHint->setParent( this );
	guess_m_pInventoryHint->setPos( guess_m_pInventoryHint->getWide() * 1.2 - guess_m_pInventoryHint->getWide(), guess_m_iInventoryHintY );
	guess_m_pInventoryHint->setVisible( false );

	guess_m_pLogo = new ImagePanel( guess_m_pLogoImage );
	guess_m_pLogo->setParent( this );
	int x = ScreenWidth / 2 - guess_m_pLogo->getWide() / 2;
	int y = ScreenHeight / 2 - guess_m_pLogo->getTall() / 2;
	guess_m_pLogo->setPos( x, y );
	guess_m_pLogoImage->setColor( Color( 255, 255, 255, 255 ) );
	guess_m_pLogo->setVisible( false );

	guess_m_pOverlay = new Panel( 0, 0, ScreenWidth, ScreenHeight );
	guess_m_pOverlay->setParent( this );
	guess_m_pOverlay->setSize( ScreenWidth, ScreenHeight );
	guess_m_pOverlay->setPos( 0, 0 );
	guess_m_pOverlay->setBgColor( 33, 177, 120, 128 );
	guess_m_pOverlay->setVisible( false );

	guess_m_pHealthBar = new ImagePanel( guess_m_pHealthBarImage );
	guess_m_pHealthBar->setParent( this );
	guess_m_pHealthBar->setPos( guess_m_pHealthBar->getWide(), ScreenHeight - guess_m_pHealthBar->getTall() );
	guess_m_pHealthBar->setVisible( true );

	guess_m_pHealthBarFull = new ImagePanel( guess_m_pHealthBarFullImage );
	guess_m_pHealthBarFull->setParent( guess_m_pHealthBar );
	guess_m_pHealthBarFull->setPos( 11, 5 );
	guess_m_pHealthBarFull->setVisible( true );

	guess_m_pHealthBarEmpty = new ImagePanel( guess_m_pHealthBarEmptyImage );
	guess_m_pHealthBarEmpty->setParent( guess_m_pHealthBar );
	guess_m_pHealthBarEmpty->setPos( 11, 5 );
	guess_m_pHealthBarEmpty->setVisible( true );

	guess_m_pStaminaBar = new ImagePanel( guess_m_pStaminaBarImage );
	guess_m_pStaminaBar->setParent( this );
	guess_m_pStaminaBar->setPos( guess_m_pStaminaBar->getWide() * 2, ScreenHeight - guess_m_pStaminaBar->getTall() );
	guess_m_pStaminaBar->setVisible( true );

	guess_m_pStaminaBarFull = new ImagePanel( guess_m_pStaminaBarFullImage );
	guess_m_pStaminaBarFull->setParent( guess_m_pStaminaBar );
	guess_m_pStaminaBarFull->setPos( 11, 5 );
	guess_m_pStaminaBarFull->setVisible( true );

	guess_m_pStaminaBarEmpty = new ImagePanel( guess_m_pStaminaBarEmptyImage );
	guess_m_pStaminaBarEmpty->setParent( guess_m_pStaminaBar );
	guess_m_pStaminaBarEmpty->setPos( 11, 5 );
	guess_m_pStaminaBarEmpty->setVisible( true );

	int wide, tall;
	guess_m_pStaminaBarEmptyImage->getSize( wide, tall );

	guess_m_pStaminaFlash = new Panel( 255, 0, 0, 255 );
	guess_m_pStaminaFlash->setParent( guess_m_pStaminaBarFull );
	guess_m_pStaminaFlash->setSize( wide, tall );
	guess_m_pStaminaFlash->setPos( 0, 0 );
	guess_m_pStaminaFlash->setBgColor( 255, 0, 0, 255 );
	guess_m_pStaminaFlash->setVisible( true );

	guess_m_pBlackScreen = new Panel( 0, 0, 0, 0 );
	guess_m_pBlackScreen->setParent( this );
	guess_m_pBlackScreen->setSize( ScreenWidth, ScreenHeight );
	guess_m_pBlackScreen->setBgColor( 0, 0, 0, 255 );
	guess_m_pBlackScreen->setVisible( false );

	guess_m_pVoteLabel = new Label( " ", ScreenWidth / 2 - 128, ScreenHeight / 2, 256, 32 );
	guess_m_pVoteLabel->setParent( this );
	guess_m_pVoteLabel->setFont( guess_m_pPagerFont );
	guess_m_pVoteLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pVoteLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pVoteLabel->setText( "Press Vote button when ready" );
	guess_m_pVoteLabel->setContentAlignment( Label::a_center );
	guess_m_pVoteLabel->setVisible( false );

	for ( i = 0; i < 4; i++ )
	{
		guess_m_pLobbyLabel[i] = new Label( " ", ScreenWidth / 2 - 512 + i * 256, ScreenHeight / 2 + 32, 256, 32 );
		guess_m_pLobbyLabel[i]->setParent( guess_m_pLobbyBackground );
		guess_m_pLobbyLabel[i]->setFont( guess_m_pPagerFont );
		guess_m_pLobbyLabel[i]->setFgColor( 255, 255, 255, 0 );
		guess_m_pLobbyLabel[i]->setBgColor( 0, 0, 0, 255 );
		guess_m_pLobbyLabel[i]->setText( "Gay" );
		guess_m_pLobbyLabel[i]->setContentAlignment( Label::a_center );
		guess_m_pLobbyLabel[i]->setVisible( false );
	}

	guess_m_pLobbyLabel[0]->setPos( 116, 257 );
	guess_m_pLobbyLabel[1]->setPos( 116, 440 );
	guess_m_pLobbyLabel[2]->setPos( 429, 257 );
	guess_m_pLobbyLabel[3]->setPos( 429, 440 );

	guess_m_pVoteKickLabel = new Label( " ", ScreenWidth / 2 - 128, ScreenHeight / 2 + 64, 256, 32 );
	guess_m_pVoteKickLabel->setParent( this );
	guess_m_pVoteKickLabel->setFont( guess_m_pPagerFont );
	guess_m_pVoteKickLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pVoteKickLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pVoteKickLabel->setText( "Type votekick name in chat to votekick a player" );
	guess_m_pVoteKickLabel->setContentAlignment( Label::a_center );
	guess_m_pVoteKickLabel->setVisible( false );

	guess_m_pVoteLabel->setParent( guess_m_pLobbyBackground );
	guess_m_pVoteLabel->setPos( 272, 492 );
	guess_m_pVoteKickLabel->setParent( guess_m_pLobbyBackground );
	guess_m_pVoteKickLabel->setPos( 272, 524 );

	guess_m_pTimerLabel = new Label( " ", 0, 0, ScreenWidth, 64 );
	guess_m_pTimerLabel->setParent( this );
	guess_m_pTimerLabel->setFont( guess_m_pCreditsFont );
	guess_m_pTimerLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pTimerLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pTimerLabel->setText( "00:00:00" );
	guess_m_pTimerLabel->setContentAlignment( Label::a_center );
	guess_m_pTimerLabel->setVisible( false );

	for ( i = 0; i < 32; i++ )
	{
		guess_m_pTimerPlusLabel[i] = new Label( " ", 0, 0, 64, 128 );
		guess_m_pTimerPlusLabel[i]->setParent( this );
		guess_m_pTimerPlusLabel[i]->setFont( guess_m_pChapterTitleFont );
		guess_m_pTimerPlusLabel[i]->setFgColor( 0, 128, 0, 0 );
		guess_m_pTimerPlusLabel[i]->setBgColor( 0, 0, 0, 255 );
		guess_m_pTimerPlusLabel[i]->setText( "+0" );
		guess_m_pTimerPlusLabel[i]->setContentAlignment( Label::a_northwest );
		guess_m_pTimerPlusLabel[i]->setVisible( false );
	}

	guess_m_pMessageLabel2 = new Label( " ", 0, ScreenHeight / 2, ScreenWidth, 64 );
	guess_m_pMessageLabel2->setParent( this );
	guess_m_pMessageLabel2->setFont( guess_m_pChapterTitleFont );
	guess_m_pMessageLabel2->setFgColor( 0, 0, 0, 0 );
	guess_m_pMessageLabel2->setBgColor( 0, 0, 0, 255 );
	guess_m_pMessageLabel2->setText( " " );
	guess_m_pMessageLabel2->setContentAlignment( Label::a_center );

	guess_m_pLookingAtLabel = new Label( " ", ScreenWidth / 2 - 128, ScreenHeight / 2 - 40, 256, 32 );
	guess_m_pLookingAtLabel->setParent( this );
	guess_m_pLookingAtLabel->setFont( guess_m_pPagerFont );
	guess_m_pLookingAtLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pLookingAtLabel->setBgColor( 255, 255, 255, 255 );
	guess_m_pLookingAtLabel->setText( " " );
	guess_m_pLookingAtLabel->setContentAlignment( Label::a_center );

	int iBarX, iBarY;
	guess_m_pStaminaBar->getPos( iBarX, iBarY );

	guess_m_pDeathLabel = new Label( "", ScreenWidth - 133, 5, 128, 40 );
	guess_m_pDeathLabel->setParent( this );
	guess_m_pDeathLabel->setFont( guess_m_pPagerFont );
	guess_m_pDeathLabel->setFgColor( 255, 0, 0, 255 );
	guess_m_pDeathLabel->setBgColor( 255, 255, 255, 255 );
	guess_m_pDeathLabel->setText( "" );
	guess_m_pDeathLabel->setContentAlignment( Label::a_east );

	for ( i = 0; i < 4; i++ )
	{
		guess_m_pPlayerLabel[i] = new Label( "", ScreenWidth - 133, ScreenHeight / 1.5 + i * 48, 128, 48 );
		guess_m_pPlayerLabel[i]->setParent( this );
		guess_m_pPlayerLabel[i]->setFont( guess_m_pPagerFont );
		guess_m_pPlayerLabel[i]->setFgColor( 255, 255, 255, 0 );
		guess_m_pPlayerLabel[i]->setBgColor( 255, 255, 255, 255 );
		guess_m_pPlayerLabel[i]->setText( "" );
		guess_m_pPlayerLabel[i]->setContentAlignment( Label::a_east );
		guess_m_pPlayerLabel[i]->setVisible( false );

		guess_m_pFollowIconPanel[i] = new ImagePanel( guess_m_pFollowIcon[i] );
		guess_m_pFollowIconPanel[i]->setParent( this );
		guess_m_pFollowIconPanel[i]->setPos( ScreenWidth - 261, ScreenHeight / 1.5 + i * 48 );
		guess_m_pFollowIconPanel[i]->setVisible( false );

		guess_m_pSyringeIconPanel[i] = new ImagePanel( guess_m_pSyringeIcon[i] );
		guess_m_pSyringeIconPanel[i]->setParent( this );
		guess_m_pSyringeIconPanel[i]->setPos( ScreenWidth - 133, ScreenHeight / 1.5 + i * 48 );
		guess_m_pSyringeIconPanel[i]->setVisible( false );
	}

	for ( i = 0; i < 5; i++ )
	{
		if ( !guess_m_iSayTextY )
			guess_m_iSayTextY = ScreenHeight / 2;
		if ( !guess_m_iSayTextX )
			guess_m_iSayTextX = 5;

		int w, h;
		guess_m_pPagerFont->getTextSize( "ysizetestg", w, h );

		guess_m_pSayTextLabel[i] = new Label( "", guess_m_iSayTextX, guess_m_iSayTextY + i * 32, ScreenWidth, 32 );
		guess_m_pSayTextLabel[i]->setParent( this );
		guess_m_pSayTextLabel[i]->setFont( guess_m_pPagerFont );
		guess_m_pSayTextLabel[i]->setFgColor( 255, 255, 255, 0 );
		guess_m_pSayTextLabel[i]->setBgColor( 0, 0, 0, 255 );
		guess_m_pSayTextLabel[i]->setText( "" );
		guess_m_pSayTextLabel[i]->setContentAlignment( Label::a_west );
		guess_m_pSayTextLabel[i]->setVisible( true );
	}

	guess_g_iLetterboxHeight = ScreenHeight / 480.0f * 25.0f + 0.5f;

	guess_m_pLetterboxTop = new Panel( 0, 0, ScreenWidth, guess_g_iLetterboxHeight );
	guess_m_pLetterboxTop->setParent( this );
	guess_m_pLetterboxTop->setSize( ScreenWidth, guess_g_iLetterboxHeight );
	guess_m_pLetterboxTop->setBgColor( 0, 0, 0, 0 );
	guess_m_pLetterboxTop->setVisible( true );

	guess_m_pLetterboxBottom = new Panel( 0, ScreenHeight - guess_g_iLetterboxHeight, ScreenWidth, guess_g_iLetterboxHeight );
	guess_m_pLetterboxBottom->setParent( this );
	guess_m_pLetterboxBottom->setSize( ScreenWidth, guess_g_iLetterboxHeight );
	guess_m_pLetterboxBottom->setBgColor( 0, 0, 0, 0 );
	guess_m_pLetterboxBottom->setVisible( true );

	guess_m_pMessageLabel = new Label( " ", 0, 0, ScreenWidth, 64 );
	guess_m_pMessageLabel->setParent( this );
	guess_m_pMessageLabel->setFont( guess_m_pCreditsFont );
	guess_m_pMessageLabel->setFgColor( 0, 0, 0, 0 );
	guess_m_pMessageLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pMessageLabel->setText( " " );
	guess_m_pMessageLabel->setContentAlignment( Label::a_center );

	// the x position uses ScreenHeight, the ControlHint message corrects it with ScreenWidth
	guess_m_pHintBar = new ImagePanel( guess_m_pHintBarImage );
	guess_m_pHintBar->setParent( this );
	guess_m_pHintBar->setPos( ScreenHeight / 2 - 400, -48 );
	guess_m_pHintBar->setVisible( false );

	guess_m_pHintLabel = new Label( " ", ScreenHeight / 2 - 400, -48, 800, 48 );
	guess_m_pHintLabel->setParent( this );
	guess_m_pHintLabel->setFont( guess_m_pCreditsFont );
	guess_m_pHintLabel->setFgColor( 0, 0, 0, 0 );
	guess_m_pHintLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pHintLabel->setText( " " );
	guess_m_pHintLabel->setContentAlignment( Label::a_center );

	Initialize();
}

CHUDControl::~CHUDControl()
{
	delete guess_m_pSMSIconImage;
	delete guess_m_pInventoryHintImage;
	delete guess_m_pCrosshairImage;
	delete guess_m_pHealthBarImage;
	delete guess_m_pHealthBarFullImage;
	delete guess_m_pHealthBarEmptyImage;
	delete guess_m_pStaminaBarImage;
	delete guess_m_pStaminaBarFullImage;
	delete guess_m_pStaminaBarEmptyImage;
	delete guess_m_pLogoImage;
	delete guess_m_pHintBarImage;
	delete guess_m_pIntroductionImage;
}

void CHUDControl::guess_UpdateHUDVisibility()
{
	int i;

	if ( !strcmp( gEngfuncs.pfnGetLevelName(), "maps/c_nightmare.bsp" ) )
	{
		if ( guess_m_pSMSIcon->isVisible() )
			guess_m_pSMSIcon->setVisible( false );
		if ( guess_m_pInventoryHint->isVisible() )
			guess_m_pInventoryHint->setVisible( false );
		if ( guess_m_pHealthBar->isVisible() )
			guess_m_pHealthBar->setVisible( false );
		if ( guess_m_pHealthBarFull->isVisible() )
			guess_m_pHealthBarFull->setVisible( false );
		if ( guess_m_pHealthBarEmpty->isVisible() )
			guess_m_pHealthBarEmpty->setVisible( false );
		if ( guess_m_pStaminaBar->isVisible() )
			guess_m_pStaminaBar->setVisible( false );
		if ( guess_m_pStaminaBarFull->isVisible() )
			guess_m_pStaminaBarFull->setVisible( false );
		if ( guess_m_pStaminaBarEmpty->isVisible() )
			guess_m_pStaminaBarEmpty->setVisible( false );
		return;
	}

	if ( !gHUD.m_pCvarDraw->value || ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL | HIDEHUD_HEALTH ) ) )
	{
		if ( guess_m_pSMSIcon->isVisible() )
			guess_m_pSMSIcon->setVisible( false );
		if ( guess_m_pInventoryHint->isVisible() )
			guess_m_pInventoryHint->setVisible( false );
		if ( guess_m_pTimerLabel->isVisible() )
			guess_m_pTimerLabel->setVisible( false );
		for ( i = 0; i < 32; i++ )
		{
			if ( guess_m_pTimerPlusLabel[i]->isVisible() )
				guess_m_pTimerPlusLabel[i]->setVisible( false );
		}
		if ( guess_m_pOverlay->isVisible() )
			guess_m_pOverlay->setVisible( false );
		if ( guess_m_pHealthBar->isVisible() )
			guess_m_pHealthBar->setVisible( false );
		if ( guess_m_pHealthBarFull->isVisible() )
			guess_m_pHealthBarFull->setVisible( false );
		if ( guess_m_pHealthBarEmpty->isVisible() )
			guess_m_pHealthBarEmpty->setVisible( false );
		if ( guess_m_pStaminaBar->isVisible() )
			guess_m_pStaminaBar->setVisible( false );
		if ( guess_m_pStaminaBarFull->isVisible() )
			guess_m_pStaminaBarFull->setVisible( false );
		if ( guess_m_pStaminaBarEmpty->isVisible() )
			guess_m_pStaminaBarEmpty->setVisible( false );
	}

	if ( !gHUD.m_iHideHUDDisplay && gHUD.m_pCvarDraw->value )
	{
		if ( !guess_m_pSMSIcon->isVisible() && guess_m_bSMSIcon )
			guess_m_pSMSIcon->setVisible( true );
		if ( !guess_m_pInventoryHint->isVisible() && guess_m_bInventoryHint )
			guess_m_pInventoryHint->setVisible( true );
		if ( !guess_m_pHealthBar->isVisible() )
			guess_m_pHealthBar->setVisible( true );
		if ( !guess_m_pHealthBarFull->isVisible() )
			guess_m_pHealthBarFull->setVisible( true );
		if ( !guess_m_pHealthBarEmpty->isVisible() )
			guess_m_pHealthBarEmpty->setVisible( true );
		if ( !guess_m_pStaminaBar->isVisible() )
			guess_m_pStaminaBar->setVisible( true );
		if ( !guess_m_pStaminaBarFull->isVisible() )
			guess_m_pStaminaBarFull->setVisible( true );
		if ( !guess_m_pStaminaBarEmpty->isVisible() )
			guess_m_pStaminaBarEmpty->setVisible( true );
	}
}

void CHUDControl::Initialize()
{
	int i;

	guess_m_bTimerPaused = false;
	guess_g_iSimonMode = 0;
	gHUD.guess_m_szSleeveTexture[0] = '\0';
	gHUD.guess_m_szFingerTexture[0] = '\0';
	gHUD.guess_m_szGloveTexture[0] = '\0';

	guess_m_pTimerLabel->setFgColor( 128, 255, 128, 0 );

	if ( guess_m_bTimer || ( guess_m_pTimerLabel && guess_m_pTimerLabel->isVisible() ) )
	{
		if ( guess_m_pTimerLabel )
		{
			guess_m_bTimer = false;
			guess_m_pTimerLabel->setVisible( false );
			for ( i = 0; i < 32; i++ )
			{
				if ( guess_m_pTimerPlusLabel[i]->isVisible() )
					guess_m_pTimerPlusLabel[i]->setVisible( false );
			}
		}
	}

	guess_m_iTimerLimit = 0;
	guess_m_iTimerSeconds = 0;
	guess_m_iTimerMinutes = 0;
	guess_m_iTimerHours = 0;
	guess_m_iIntroductionAlpha = 255;
	guess_m_bIntroductionFadeIn = false;
	guess_m_pIntroduction->setVisible( false );
	guess_m_pLobbyBackground->setVisible( false );
	guess_m_bLobby = false;

	for ( i = 0; i <= MAX_PLAYERS; i++ )
	{
		gHUD.m_Health.guess_m_iAdLibState[i] = 0;
		gHUD.m_Health.guess_m_bSyringe[i] = false;
		gHUD.m_Health.guess_m_flAdLibTime[i] = gEngfuncs.GetClientTime();
	}

	for ( i = 0; i < 4; i++ )
	{
		guess_m_bPlayerReady[i] = false;
		guess_m_pFollowIconPanel[i]->setVisible( false );
		guess_m_pSyringeIconPanel[i]->setVisible( false );
	}

	guess_m_pLookingAtLabel->setText( "" );
	guess_m_pLookingAtLabel->setVisible( false );
	guess_m_flLookingAtTime = gEngfuncs.GetClientTime();
	guess_m_flTimerPlusTime = gEngfuncs.GetClientTime();
	guess_m_flTimerNextSecond = gEngfuncs.GetClientTime();
	guess_m_pDeathLabel->setText( "" );
	guess_m_iDeathAlpha = 255;
	guess_m_bLogoF5C = false;
	guess_m_iLogo = 0;
	setVisible( true );

	guess_m_flSMSIconTime = gEngfuncs.GetClientTime();
	guess_m_iSMSIconAlpha = 255;
	guess_m_pSMSIconImage->setColor( Color( 255, 255, 255, 255 ) );
	guess_ShowSMSIcon( false );
	guess_m_pLogo->setVisible( false );
	guess_m_bSMSIcon = false;

	guess_m_iMessageType = 5;
	guess_m_iChapterType = 5;
	guess_m_flStaminaFlashTime = gEngfuncs.GetClientTime();
	guess_m_szHintLine1[0] = '\0';
	guess_m_szHintLine2[0] = '\0';
	guess_m_szHintLine3[0] = '\0';
	guess_m_szHintNoButton[0] = '\0';
	guess_m_iLogoAlpha = 255;
	guess_m_bShowLogo = false;
	guess_m_szMessage2[0] = '\0';
	guess_m_flHintSlideInTime = -1;
	guess_m_flHintSlideInLength = -1;
	guess_m_flHintSlideOutTime = -1;
	guess_m_flHintSlideOutLength = -1;
	guess_m_flLogoTime = 0;
	guess_m_iMessageType = 0;
	guess_m_iMessageTime = 0;
	guess_m_iMessageC4 = 0;
	guess_m_iMessageC8 = 0;
	guess_m_iMessageCC = 0;
	guess_m_iMessageD0 = 0;
	guess_m_szMessage[0] = '\0';
	guess_m_flZoomLength = 0;
	guess_m_flZoomTime = 0;
	guess_m_flZoomFOV = 0;

	for ( i = 0; i < 5; i++ )
		guess_m_pSayTextLabel[i]->setText( "" );

	guess_m_pMessageLabel2->setText( "" );
	guess_m_pMessageLabel->setText( "" );
	guess_m_pHintLabel->setText( "" );

	guess_m_bHint = false;
	guess_m_iHintAlpha = 255;
	guess_m_szChapter[0] = '\0';
	guess_m_flMessageAlpha = 0;
	guess_m_bMessageFadeOut = false;
	guess_m_bMessageHold = false;
	guess_m_flMessageHoldTime = 0;
	guess_m_flMessageF24 = 0;
	guess_m_iMessageChars = 0;
	guess_m_iChapterType = 0;
	guess_m_flChapterAlpha = 0;
	guess_m_bChapterFadeOut = false;
	guess_m_bChapterF1E = false;
	guess_m_flChapterHoldTime = 0;
	guess_m_flChapterNextChar = 0;
	guess_m_iChapterChars = 0;

	guess_m_pInventoryHint->setVisible( false );
	guess_m_bInventoryHint = false;
	guess_m_flInventoryHintTime = gEngfuncs.GetClientTime();
	guess_m_iInventoryHintY = -160;
}

void CHUDControl::guess_UpdateLogo()
{
	if ( guess_m_bShowLogo )
	{
		if ( CVAR_GET_FLOAT( "host_framerate" ) != 0 )
		{
			if ( guess_m_pLogo->isVisible() )
				guess_m_pLogo->setVisible( false );
			guess_m_bShowLogo = false;
			guess_m_pLogoImage->setColor( Color( 255, 255, 255, 255 ) );
			return;
		}

		if ( gEngfuncs.GetClientTime() < guess_m_flLogoTime )
		{
			if ( !guess_m_pLogo->isVisible() )
				guess_m_pLogo->setVisible( true );

			guess_m_iLogoAlpha -= 15;
			if ( guess_m_iLogoAlpha < 0 )
				guess_m_iLogoAlpha = 0;
			guess_m_pLogoImage->setColor( Color( 255, 255, 255, guess_m_iLogoAlpha ) );
		}
		else
		{
			guess_m_iLogoAlpha += 15;
			if ( guess_m_iLogoAlpha > 255 )
				guess_m_iLogoAlpha = 255;
			guess_m_pLogoImage->setColor( Color( 255, 255, 255, guess_m_iLogoAlpha ) );

			if ( guess_m_pLogo->isVisible() && guess_m_iLogoAlpha >= 255 )
			{
				guess_m_pLogo->setVisible( false );
				guess_m_bShowLogo = false;
				guess_m_iLogoAlpha = 255;
			}
		}
	}
	else
	{
		if ( guess_m_pLogo->isVisible() )
			guess_m_pLogo->setVisible( false );
	}
}

void CHUDControl::guess_DrawMessages()
{
	float flTime = gEngfuncs.GetClientTime();

	// control hint bar
	if ( guess_m_bHint )
	{
		int x, y;

		if ( guess_m_flHintTime > flTime )
		{
			if ( !guess_m_pHintLabel->isVisible() )
				guess_m_pHintLabel->setVisible( true );
			if ( !guess_m_pHintBar->isVisible() )
				guess_m_pHintBar->setVisible( true );

			if ( guess_m_iHintAlpha > 0 )
				guess_m_iHintAlpha -= 10;
			if ( guess_m_iHintAlpha < 0 )
				guess_m_iHintAlpha = 0;

			guess_m_pHintLabel->getPos( x, y );
			if ( y < 0 )
				y += 2;
			if ( y > 0 )
				y = 0;

			guess_m_pHintBar->setPos( ScreenWidth / 2 - 400, y );
			guess_m_pHintBarImage->setColor( Color( 255, 255, 255, guess_m_iHintAlpha ) );
			guess_m_pHintLabel->setPos( ScreenWidth / 2 - 400, y );
			guess_m_pHintLabel->setFgColor( 255, 255, 255, guess_m_iHintAlpha );
		}
		else
		{
			if ( guess_m_iHintAlpha < 255 )
				guess_m_iHintAlpha += 10;
			if ( guess_m_iHintAlpha > 255 )
				guess_m_iHintAlpha = 255;

			guess_m_pHintLabel->setFgColor( 255, 255, 255, guess_m_iHintAlpha );

			guess_m_pHintLabel->getPos( x, y );
			if ( y > -48 )
				y -= 2;
			if ( y < -48 )
				y = -48;

			guess_m_pHintLabel->setPos( ScreenWidth / 2 - 400, y );
			guess_m_pHintBar->setPos( ScreenWidth / 2 - 400, y );
			guess_m_pHintBarImage->setColor( Color( 255, 255, 255, guess_m_iHintAlpha ) );

			if ( guess_m_iHintAlpha >= 255 )
			{
				int w, h;

				switch ( guess_m_iHintLine )
				{
				case 1:
					guess_m_iHintLine = 2;
					if ( strlen( guess_m_szHintLine2 ) > 2 )
					{
						guess_m_pHintLabel->setText( guess_m_szHintLine2 );
						guess_m_pCreditsFont->getTextSize( guess_m_szHintLine2, w, h );
						guess_m_pHintLabel->setPos( ScreenWidth / 2 - 400, -48 );
						guess_m_bHint = true;
						guess_m_iHintAlpha = 255;
						guess_m_flHintTime = gEngfuncs.GetClientTime() + 5;
					}
					else
					{
						if ( !guess_m_pHintLabel->isVisible() )
						{
							guess_m_bHint = false;
							guess_m_pHintLabel->setVisible( true );
						}
						if ( guess_m_pHintBar->isVisible() == true )
							guess_m_pHintBar->setVisible( false );
					}
					break;
				case 2:
					guess_m_iHintLine = 3;
					if ( strlen( guess_m_szHintLine3 ) > 2 )
					{
						guess_m_pHintLabel->setText( guess_m_szHintLine3 );
						guess_m_pCreditsFont->getTextSize( guess_m_szHintLine3, w, h );
						guess_m_pHintLabel->setPos( ScreenWidth / 2 - 400, -48 );
						guess_m_bHint = true;
						guess_m_iHintAlpha = 255;
						guess_m_flHintTime = gEngfuncs.GetClientTime() + 5;
					}
					else
					{
						if ( !guess_m_pHintLabel->isVisible() )
						{
							guess_m_bHint = false;
							guess_m_pHintLabel->setVisible( true );
						}
						if ( guess_m_pHintBar->isVisible() == true )
							guess_m_pHintBar->setVisible( false );
					}
					break;
				case 3:
					if ( !guess_m_pHintLabel->isVisible() )
					{
						guess_m_bHint = false;
						guess_m_pHintLabel->setVisible( true );
					}
					if ( guess_m_pHintBar->isVisible() == true )
						guess_m_pHintBar->setVisible( false );
					break;
				default:
					if ( !guess_m_pHintLabel->isVisible() )
					{
						guess_m_bHint = false;
						guess_m_pHintLabel->setVisible( true );
					}
					if ( guess_m_pHintBar->isVisible() == true )
						guess_m_pHintBar->setVisible( false );
					break;
				}
			}
		}
	}
	else
	{
		if ( guess_m_pHintLabel->isVisible() )
			guess_m_pHintLabel->setVisible( false );
		if ( guess_m_pHintBar->isVisible() == true )
			guess_m_pHintBar->setVisible( false );
	}

	// chapter title
	if ( guess_m_szChapter[0] )
	{
		int w, h, x, y;

		if ( guess_m_bChapterHold && flTime > guess_m_flChapterHoldTime )
		{
			guess_m_bChapterFadeOut = true;
			guess_m_bChapterHold = false;
			guess_m_flChapterAlpha = 0;
			guess_m_iChapterChars = strlen( guess_m_szChapter );
		}

		if ( flTime > guess_m_flChapterNextChar )
		{
			guess_m_flChapterNextChar = flTime + 0.025;
			if ( !guess_m_bMessageFadeOut )
			{
				if ( guess_m_iChapterChars < (int)strlen( guess_m_szChapter ) )
					guess_m_iChapterChars++;
			}
		}

		if ( !guess_m_bChapterFadeOut )
		{
			if ( !guess_m_bChapterHold )
			{
				if ( guess_m_flChapterAlpha > 0 )
					guess_m_flChapterAlpha = guess_m_flChapterAlpha - gHUD.m_flTimeDelta * 250;
				if ( guess_m_flChapterAlpha < 0 )
				{
					guess_m_bChapterHold = true;
					guess_m_flChapterAlpha = 0;
					guess_m_flChapterHoldTime = flTime + 3.0;
				}
			}
		}
		else if ( guess_m_bChapterFadeOut == true && !guess_m_bChapterHold )
		{
			if ( guess_m_flChapterAlpha < 255.0 )
				guess_m_flChapterAlpha = gHUD.m_flTimeDelta * 125 + guess_m_flChapterAlpha;
			if ( guess_m_flChapterAlpha > 255.0 )
			{
				guess_m_bChapterFadeOut = false;
				guess_m_flChapterAlpha = 255;
				guess_m_szChapter[0] = '\0';
			}
		}

		drawSetTextFont( guess_m_pChapterTitleFont );
		if ( guess_m_iChapterType <= 5 )
			drawSetTextColor( 255, 255, 255, (int)guess_m_flChapterAlpha );
		else
			drawSetTextColor( 255, 0, 0, (int)guess_m_flChapterAlpha );
		guess_m_pChapterTitleFont->getTextSize( guess_m_szChapter, w, h );
		x = ScreenWidth / 2 - w / 2;
		y = ScreenHeight / 2 - h / 2;
		drawSetTextPos( x, y );

		if ( guess_m_szChapter[0] && guess_m_iChapterType <= 5 )
			gEngfuncs.pfnFillRGBA( x, y + h, (int)( guess_m_iChapterChars / (float)strlen( guess_m_szChapter ) * w ), 2, 255, 255, 255, (int)( 255.0f - guess_m_flChapterAlpha ) );

		guess_m_pMessageLabel2->setSize( ScreenWidth, 64 );
		guess_m_pMessageLabel2->setText( guess_m_szChapter );
		guess_m_pMessageLabel2->setPos( 0, ScreenHeight / 2 - 32 );
		guess_m_pMessageLabel2->setContentAlignment( Label::a_center );
		if ( guess_m_iChapterType <= 5 )
			guess_m_pMessageLabel2->setFgColor( 255, 255, 255, (int)guess_m_flChapterAlpha );
		else
			guess_m_pMessageLabel2->setFgColor( 255, 0, 0, (int)guess_m_flChapterAlpha );
	}

	// message strip
	if ( !(int)CVAR_GET_FLOAT( "cl_subtitles" ) )
		return;

	if ( guess_m_szMessage[0] )
	{
		unsigned int iLen2 = strlen( guess_m_szMessage2 );
		if ( strlen( guess_m_szMessage ) > 1 )
		{
			if ( guess_m_bMessageHold && flTime > guess_m_flMessageHoldTime )
			{
				guess_m_bMessageFadeOut = true;
				guess_m_bMessageHold = false;
				guess_m_flMessageAlpha = 0;
				guess_m_iMessageTime = 0;
				guess_m_iMessageChars = strlen( guess_m_szChapter );
			}

			if ( !guess_m_bMessageFadeOut )
			{
				if ( !guess_m_bMessageHold )
				{
					if ( guess_m_flMessageAlpha > 0 )
						guess_m_flMessageAlpha = guess_m_flMessageAlpha - gHUD.m_flTimeDelta * 500;
					if ( guess_m_flMessageAlpha < 0 )
					{
						guess_m_bMessageHold = true;
						guess_m_flMessageAlpha = 0;
						guess_m_flMessageHoldTime = flTime + 4.0;
					}
				}
			}
			else if ( guess_m_bMessageFadeOut == true && !guess_m_bMessageHold )
			{
				if ( guess_m_flMessageAlpha < 255.0 )
					guess_m_flMessageAlpha = gHUD.m_flTimeDelta * 500 + guess_m_flMessageAlpha;
				if ( guess_m_flMessageAlpha > 255.0 )
				{
					guess_m_bMessageFadeOut = false;
					guess_m_flMessageAlpha = 255;
					guess_m_szMessage[0] = '\0';
				}
			}

			int a = guess_m_flMessageAlpha;
			drawSetTextFont( guess_m_pCreditsFont );

			switch ( guess_m_iMessageType )
			{
			case 0:
			case 3:
				drawSetTextColor( 255, 255, 0, a );
				break;
			case 1:
				drawSetTextColor( 0, 128, 255, a );
				break;
			case 4:
				drawSetTextColor( 0, 255, 0, a );
				break;
			case 5:
				drawSetTextColor( 200, 200, 200, a );
				break;
			case 2:
			case 6:
				drawSetTextColor( 255, 0, 0, a );
				break;
			}

			int w, h, w2, x, y;
			guess_m_pCreditsFont->getTextSize( guess_m_szMessage, w, h );
			if ( iLen2 > 1 )
				guess_m_pCreditsFont->getTextSize( guess_m_szMessage2, w2, h );

			if ( guess_m_iMessageType <= 5 )
			{
				x = ScreenWidth / 2 - w / 2;
				y = ScreenHeight - h * 2;
			}
			else
			{
				x = 10;
				y = ScreenHeight / 2 - h / 2;
			}

			drawSetTextPos( x, iLen2 > 1 ? y - h : y );

			guess_m_pMessageLabel->setSize( ScreenWidth, 64 );
			guess_m_pMessageLabel->setText( guess_m_szMessage );
			guess_m_pCreditsFont->getTextSize( guess_m_szMessage, w, h );
			guess_m_pMessageLabel->setPos( 0, ScreenHeight - guess_g_iLetterboxHeight * 3 );
			guess_m_pMessageLabel->setContentAlignment( Label::a_center );

			switch ( guess_m_iMessageType )
			{
			case 0:
			case 3:
				guess_m_pMessageLabel->setFgColor( 255, 255, 0, a );
				break;
			case 1:
				guess_m_pMessageLabel->setFgColor( 0, 128, 255, a );
				break;
			case 4:
				guess_m_pMessageLabel->setFgColor( 0, 255, 0, a );
				break;
			case 5:
				guess_m_pMessageLabel->setFgColor( 200, 200, 200, a );
				break;
			case 2:
			case 6:
				guess_m_pMessageLabel->setFgColor( 255, 0, 0, a );
				break;
			}

			if ( iLen2 > 1 )
			{
				guess_m_pCreditsFont->getTextSize( guess_m_szMessage2, w, h );
				drawSetTextPos( ScreenWidth / 2 - w / 2, ScreenHeight - h * 2 );
			}
		}
	}
	else
	{
		guess_m_iMessageCC = 0;
		guess_m_iMessageD0 = 0;
		guess_m_iMessageC4 = 0;
		guess_m_iMessageC8 = 0;
		guess_m_iMessageType = 5;
		if ( strlen( guess_m_szMessage ) > 1 )
			guess_m_szMessage[0] = '\0';
		if ( strlen( guess_m_szMessage2 ) > 1 )
			guess_m_szMessage2[0] = '\0';
	}
}

void CHUDControl::guess_ShowSMSIcon( bool bShow )
{
	guess_m_iSMSIconAlpha = 255;
	guess_m_bSMSIcon = bShow;
	guess_m_pSMSIcon->setVisible( bShow );
}

void CHUDControl::paint()
{
	int h, w;
	int x, y;

	if ( guess_g_iDickMode )
	{
		switch ( gEngfuncs.pfnRandomLong( 1, 4 ) )
		{
		case 1:
			drawSetTextFont( guess_m_pCreditsFont );
			drawSetTextColor( 255, 255, 0, 1 );
			guess_m_pCreditsFont->getTextSize( "Dicks", w, h );
			x = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenWidth / 2 - w / 2 );
			y = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenHeight / 2 - h / 2 );
			drawSetTextPos( x, y );
			drawPrintText( "Dicks", 5 );
			break;
		case 2:
			drawSetTextFont( guess_m_pCreditsFont );
			drawSetTextColor( 255, 255, 0, 1 );
			guess_m_pCreditsFont->getTextSize( "Balls", w, h );
			x = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenWidth / 2 - w / 2 );
			y = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenHeight / 2 - h / 2 );
			drawSetTextPos( x, y );
			drawPrintText( "Balls", 5 );
			break;
		case 3:
			drawSetTextFont( guess_m_pCreditsFont );
			drawSetTextColor( 255, 255, 0, 1 );
			guess_m_pCreditsFont->getTextSize( "Dongs", w, h );
			x = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenWidth / 2 - w / 2 );
			y = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenHeight / 2 - h / 2 );
			drawSetTextPos( x, y );
			drawPrintText( "Dongs", 5 );
			break;
		case 4:
			drawSetTextFont( guess_m_pCreditsFont );
			drawSetTextColor( 255, 255, 0, 1 );
			guess_m_pCreditsFont->getTextSize( "Butts", w, h );
			x = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenWidth / 2 - w / 2 );
			y = gEngfuncs.pfnRandomLong( 0, 50 ) + ( ScreenHeight / 2 - h / 2 );
			drawSetTextPos( x, y );
			drawPrintText( "Butts", 5 );
			break;
		}
	}

	guess_DrawMessages();
	Panel::paint();
}

void CHUDControl::paintBackground()
{
	Panel::paintBackground();
}

void CHUDControl::solve()
{
	int i;
	int r, g, b, a;
	int x, y, x2, y2;
	char szText[128];
	char szTime[128];
	char szSeconds[32];
	char szMinutes[32];
	char szHours[32];

	// introduction picture
	if ( guess_m_pIntroduction->isVisible() )
	{
		if ( guess_m_bIntroductionFadeIn )
		{
			if ( guess_m_iIntroductionAlpha > 0 )
				guess_m_iIntroductionAlpha -= 2;
			if ( guess_m_iIntroductionAlpha < 0 )
				guess_m_iIntroductionAlpha = 0;
			guess_m_pIntroductionImage->setColor( Color( 255, 255, 255, guess_m_iIntroductionAlpha ) );
		}
		else
		{
			if ( guess_m_iIntroductionAlpha < 255 )
				guess_m_iIntroductionAlpha += 2;
			guess_m_pIntroductionImage->setColor( Color( 255, 255, 255, guess_m_iIntroductionAlpha ) );

			if ( guess_m_iIntroductionAlpha >= 255 )
			{
				guess_m_iIntroductionAlpha = 255;
				guess_m_pIntroductionImage->setColor( Color( 255, 255, 255, 255 ) );
				guess_m_pIntroduction->setVisible( false );
			}
		}
	}

	// timer colors and the "+" popups
	if ( guess_m_bTimer )
	{
		if ( gEngfuncs.GetClientTime() > guess_m_flTimerPlusTime )
		{
			guess_m_flTimerPlusTime = gEngfuncs.GetClientTime() + 0.05;

			guess_m_pTimerLabel->getFgColor( r, g, b, a );
			if ( r < 255 )
			{
				b += 5;
				r += 5;
			}
			if ( r > 255 )
			{
				r = 255;
				b = 255;
				guess_m_pTimerLabel->setFgColor( 255, g, 255, a );
			}
			if ( guess_g_iSimonMode )
			{
				r = 128;
				g = 0;
				b = 0;
			}
			guess_m_pTimerLabel->setFgColor( r, g, b, a );

			for ( i = 0; i < 32; i++ )
			{
				if ( guess_m_pTimerPlusLabel[i]->isVisible() )
				{
					guess_m_pTimerPlusLabel[i]->getFgColor( r, g, b, a );
					a += 15;
					if ( a > 255 )
					{
						a = 255;
						guess_m_pTimerPlusLabel[i]->setFgColor( r, g, b, 255 );
						guess_m_pTimerPlusLabel[i]->setVisible( false );
					}
					else
					{
						guess_m_pTimerPlusLabel[i]->setFgColor( r, g, b, a );
						guess_m_pTimerPlusLabel[i]->getPos( x, y );
						x++;
						guess_m_pTimerPlusLabel[i]->setPos( x, y );
					}
				}
			}
		}
	}

	// lobby
	if ( guess_m_pLobbyLabel[0] && guess_m_bLobby )
	{
		if ( gHUD.guess_GetKeyForCommand( "+attack" ) )
			sprintf( szText, "Press %s to become ready/unready", gHUD.guess_GetKeyForCommand( "+attack" ) );
		else
			strcpy( szText, "Press Primary Attack to become ready/unready" );
		guess_m_pVoteLabel->setText( szText );

		for ( i = 0; i < 4; i++ )
		{
			if ( g_PlayerInfoList[i + 1].name && strlen( g_PlayerInfoList[i + 1].name ) >= 1 )
			{
				char szReady[256];
				if ( guess_m_bPlayerReady[i] )
				{
					guess_m_pLobbyLabel[i]->setFgColor( 0, 128, 0, 0 );
					sprintf( szReady, "%s - Ready", g_PlayerInfoList[i + 1].name );
				}
				else
				{
					guess_m_pLobbyLabel[i]->setFgColor( 128, 0, 0, 0 );
					sprintf( szReady, "%s - Not ready", g_PlayerInfoList[i + 1].name );
				}
				guess_m_pLobbyLabel[i]->setText( szReady );
			}
			else
			{
				guess_m_pLobbyLabel[i]->setText( "-" );
				guess_m_pLobbyLabel[i]->setFgColor( 255, 255, 255, 0 );
			}
		}
	}

	// survival timer
	if ( guess_m_bTimer )
	{
		if ( !guess_m_pTimerLabel->isVisible() )
			guess_m_pTimerLabel->setVisible( true );

		if ( gEngfuncs.GetClientTime() > guess_m_flTimerNextSecond && !guess_m_bTimerPaused )
		{
			guess_m_iTimerSeconds++;
			if ( guess_m_iTimerSeconds > 59 )
			{
				guess_m_iTimerSeconds = 0;
				guess_m_iTimerMinutes++;
			}

			int iSecondsLeft = 60 - guess_m_iTimerSeconds;
			int iMinutesLeft = guess_m_iTimerLimit / 60 - guess_m_iTimerMinutes - 1;

			if ( iMinutesLeft < 0 )
				iMinutesLeft = 0;
			if ( iSecondsLeft < 0 )
				iSecondsLeft = 0;
			if ( guess_m_iTimerHours < 0 )
				guess_m_iTimerHours = 0;

			sprintf( szSeconds, ( iSecondsLeft >= 10 ) ? "%i" : "0%i", iSecondsLeft );
			sprintf( szMinutes, ( iMinutesLeft >= 10 ) ? "%i" : "0%i", iMinutesLeft );
			sprintf( szHours, ( guess_m_iTimerHours >= 10 ) ? "%i" : "0%i", guess_m_iTimerHours );
			sprintf( szTime, "%s:%s:%s", szHours, szMinutes, szSeconds );
			guess_m_pTimerLabel->setText( szTime );
			guess_m_flTimerNextSecond = gEngfuncs.GetClientTime() + 1;
		}
	}

	if ( gHUD.guess_m_iFlashFlags & 0x40 )
	{
		if ( !guess_m_pOverlay->isVisible() )
			guess_m_pOverlay->setVisible( true );
		else
			guess_m_pOverlay->setBgColor( 33, 177, 120, gEngfuncs.pfnRandomLong( 0, 5 ) + 192 );
	}
	else
	{
		if ( guess_m_pOverlay->isVisible() )
			guess_m_pOverlay->setVisible( false );
	}

	if ( gHUD.m_fPlayerDead )
	{
		guess_m_pMessageLabel2->setText( "" );
		guess_m_pMessageLabel->setText( "" );
		guess_m_pHintLabel->setText( "" );

		guess_m_bHint = false;
		guess_m_iHintAlpha = 255;
		guess_m_szChapter[0] = '\0';
		guess_m_flMessageAlpha = 0;
		guess_m_bMessageFadeOut = false;
		guess_m_bMessageHold = false;
		guess_m_flMessageHoldTime = 0;
		guess_m_flMessageF24 = 0;
		guess_m_iMessageChars = 0;
		guess_m_iChapterType = 0;
		guess_m_flChapterAlpha = 0;
		guess_m_bChapterFadeOut = false;
		guess_m_bChapterF1E = false;
		guess_m_flChapterHoldTime = 0;
		guess_m_flChapterNextChar = 0;
		guess_m_iChapterChars = 0;
	}

	// co-op player list
	if ( gEngfuncs.pfnGetLevelName() && strstr( gEngfuncs.pfnGetLevelName(), "cof_" ) )
	{
		if ( guess_m_iDeathAlpha < 255 )
			guess_m_iDeathAlpha += 5;
		if ( guess_m_iDeathAlpha < 0 )
			guess_m_iDeathAlpha = 0;
		else if ( guess_m_iDeathAlpha > 255 )
			guess_m_iDeathAlpha = 255;

		if ( !guess_m_pDeathLabel->isVisible() )
			guess_m_pDeathLabel->setVisible( true );
		guess_m_pDeathLabel->setFgColor( 255, 0, 0, guess_m_iDeathAlpha );

		for ( i = 0; i < 5; i++ )
		{
			if ( !guess_m_pSayTextLabel[i]->isVisible() )
				guess_m_pSayTextLabel[i]->setVisible( true );
		}

		for ( i = 0; i < 4; i++ )
		{
			if ( g_PlayerInfoList[i + 1].name && strlen( g_PlayerInfoList[i + 1].name ) >= 1 )
			{
				if ( !guess_m_pPlayerLabel[i]->isVisible() )
					guess_m_pPlayerLabel[i]->setVisible( true );

				if ( gHUD.m_Health.guess_m_flAdLibTime[i] > gEngfuncs.GetClientTime() )
				{
					if ( !guess_m_pFollowIconPanel[i]->isVisible() )
						guess_m_pFollowIconPanel[i]->setVisible( true );
				}
				else
				{
					if ( guess_m_pFollowIconPanel[i]->isVisible() )
						guess_m_pFollowIconPanel[i]->setVisible( false );
				}

				if ( gHUD.m_Health.guess_m_bSyringe[i] )
				{
					if ( !guess_m_pSyringeIconPanel[i]->isVisible() )
						guess_m_pSyringeIconPanel[i]->setVisible( true );
				}
				else
				{
					if ( guess_m_pSyringeIconPanel[i]->isVisible() )
						guess_m_pSyringeIconPanel[i]->setVisible( false );
				}

				int w, h, w2, h2;
				sprintf( szTime, "%s - Health %i - Ping %d", g_PlayerInfoList[i + 1].name, gHUD.m_Health.guess_m_iCoopHealth[i + 1], g_PlayerInfoList[i + 1].ping );
				guess_m_pPagerFont->getTextSize( szTime, w, h );
				guess_m_pPlayerLabel[i]->getSize( w2, h2 );
				guess_m_pPlayerLabel[i]->getPos( x, y );
				guess_m_pPlayerLabel[i]->setSize( w, h );
				guess_m_pPlayerLabel[i]->setPos( ScreenWidth - w - 5, h * i + ScreenHeight / 1.5 );
				guess_m_pPlayerLabel[i]->setText( szTime );
				guess_m_pPlayerLabel[i]->getPos( x, y );
				guess_m_pFollowIconPanel[i]->setPos( x - 28, y + 4 );
				guess_m_pSyringeIconPanel[i]->setPos( x - 14, y + 4 );

				if ( guess_m_iDevDonator[i + 1] == 1 )
					guess_m_pPlayerLabel[i]->setFgColor( 117, 0, 0, 0 );
				else if ( guess_m_iDevDonator[i + 1] == 2 )
					guess_m_pPlayerLabel[i]->setFgColor( 255, 211, 81, 0 );
				else
					guess_m_pPlayerLabel[i]->setFgColor( 255, 255, 255, 0 );
			}
			else
			{
				if ( guess_m_pPlayerLabel[i]->isVisible() )
					guess_m_pPlayerLabel[i]->setVisible( false );
				if ( guess_m_pFollowIconPanel[i]->isVisible() )
					guess_m_pFollowIconPanel[i]->setVisible( false );
				if ( guess_m_pSyringeIconPanel[i]->isVisible() )
					guess_m_pSyringeIconPanel[i]->setVisible( false );
			}
		}

		if ( guess_m_flLookingAtTime > gEngfuncs.GetClientTime() )
		{
			if ( !guess_m_pLookingAtLabel->isVisible() )
				guess_m_pLookingAtLabel->setVisible( true );
		}
		else
		{
			if ( guess_m_pLookingAtLabel->isVisible() )
				guess_m_pLookingAtLabel->setVisible( false );
		}

		if ( guess_g_flLookingAt != 0 && guess_g_flLookingAt <= 4 )
		{
			char *pszName = g_PlayerInfoList[(int)guess_g_flLookingAt].name;
			if ( pszName )
			{
				guess_m_flLookingAtTime = gEngfuncs.GetClientTime() + 0.1;
				guess_m_pLookingAtLabel->setText( pszName );
			}
		}
	}
	else
	{
		for ( i = 0; i < 4; i++ )
		{
			if ( guess_m_pPlayerLabel[i]->isVisible() )
				guess_m_pPlayerLabel[i]->setVisible( false );
			if ( guess_m_pFollowIconPanel[i]->isVisible() )
				guess_m_pFollowIconPanel[i]->setVisible( false );
			if ( guess_m_pSyringeIconPanel[i]->isVisible() )
				guess_m_pSyringeIconPanel[i]->setVisible( false );
		}

		for ( i = 0; i < 5; i++ )
		{
			if ( guess_m_pSayTextLabel[i]->isVisible() )
				guess_m_pSayTextLabel[i]->setVisible( false );
		}

		if ( guess_m_pLookingAtLabel->isVisible() )
			guess_m_pLookingAtLabel->setVisible( false );
		if ( guess_m_pDeathLabel->isVisible() )
			guess_m_pDeathLabel->setVisible( false );
		if ( guess_m_iDeathAlpha < 255 )
			guess_m_iDeathAlpha = 255;
	}

	// stamina flash
	guess_m_pStaminaFlash->getBgColor( r, g, b, a );
	if ( guess_m_flStaminaFlashTime > gEngfuncs.GetClientTime() )
	{
		guess_m_pStaminaFlash->setBgColor( 255, 0, 0, 128 );
	}
	else
	{
		if ( a < 255 )
		{
			a += 15;
			guess_m_pStaminaFlash->setBgColor( r, g, b, a );
		}
		if ( a > 255 )
		{
			a = 255;
			guess_m_pStaminaFlash->setBgColor( r, g, b, 255 );
		}
	}

	// letterbox
	static bool s_bLetterboxFrame = false;
	int iSpeed = guess_g_iLetterboxHeight / 15;
	if ( iSpeed <= 0 )
		iSpeed = 1;

	if ( gHUD.guess_m_BlackBar.guess_m_iOn )
	{
		s_bLetterboxFrame = !s_bLetterboxFrame;
		if ( s_bLetterboxFrame )
		{
			guess_m_pLetterboxTop->getPos( x, y );
			guess_m_pLetterboxBottom->getPos( x2, y2 );

			if ( y < 0 )
				y += iSpeed;
			if ( y > 0 )
				y = 0;
			if ( y2 > ScreenHeight - guess_g_iLetterboxHeight )
				y2 -= iSpeed;
			if ( y2 < ScreenHeight - guess_g_iLetterboxHeight )
				y2 = ScreenHeight - guess_g_iLetterboxHeight;

			guess_m_pLetterboxTop->setPos( x, y );
			guess_m_pLetterboxBottom->setPos( x2, y2 );
		}
	}
	else
	{
		s_bLetterboxFrame = !s_bLetterboxFrame;
		if ( s_bLetterboxFrame )
		{
			guess_m_pLetterboxTop->getPos( x, y );
			guess_m_pLetterboxBottom->getPos( x2, y2 );

			if ( y > -guess_g_iLetterboxHeight )
				y -= iSpeed;
			if ( y < -guess_g_iLetterboxHeight )
				y = -guess_g_iLetterboxHeight;
			if ( y2 < ScreenHeight )
				y2 += iSpeed;
			if ( y2 > ScreenHeight )
				y2 = ScreenHeight;

			guess_m_pLetterboxTop->setPos( x, y );
			guess_m_pLetterboxBottom->setPos( x2, y2 );
		}
	}

	guess_UpdateHUDVisibility();
	guess_UpdateLogo();

	if ( guess_iWheelchair && guess_m_pStaminaBar->isVisible() )
		guess_m_pStaminaBar->setVisible( false );
	if ( guess_iWheelchair && guess_m_pStaminaBarFull->isVisible() )
		guess_m_pStaminaBarFull->setVisible( false );
	if ( guess_iWheelchair && guess_m_pStaminaBarEmpty->isVisible() )
		guess_m_pStaminaBarEmpty->setVisible( false );

	// camera zoom
	if ( gEngfuncs.GetClientTime() >= guess_m_flZoomTime )
	{
		if ( guess_m_flZoomTime + guess_m_flZoomLength > gEngfuncs.GetClientTime() )
		{
			float flFrac = ( gEngfuncs.GetClientTime() - guess_m_flZoomTime ) / guess_m_flZoomLength;
			if ( flFrac > 1.0f )
				gHUD.m_iFOV = guess_m_flZoomFOV;

			if ( gEngfuncs.GetClientTime() > guess_m_flZoomTime + guess_m_flZoomLength )
				gHUD.m_iFOV = guess_m_flZoomFOV;
			else
				gHUD.m_iFOV = 90 - (int)( flFrac * guess_m_flZoomFOV );

			if ( gHUD.m_iFOV > guess_m_flZoomFOV )
				gHUD.m_iFOV = guess_m_flZoomFOV;
		}
	}

	// sms icon fade
	if ( guess_m_pSMSIcon->isVisible() && guess_m_bSMSIcon )
	{
		if ( gEngfuncs.GetClientTime() > guess_m_flSMSIconTime )
		{
			if ( guess_m_iSMSIconAlpha > 0 )
			{
				guess_m_iSMSIconAlpha -= 15;
				guess_m_pSMSIconImage->setColor( Color( 255, 255, 255, guess_m_iSMSIconAlpha ) );
			}
			guess_m_flSMSIconTime = gEngfuncs.GetClientTime() + 0.05;
		}
	}

	// inventory hint slide
	if ( guess_m_pInventoryHint->isVisible() && guess_m_bInventoryHint )
	{
		guess_m_pInventoryHint->getPos( x, y );

		if ( gEngfuncs.GetClientTime() >= guess_m_flHintSlideInTime )
		{
			if ( guess_m_flHintSlideInTime + guess_m_flHintSlideInLength > gEngfuncs.GetClientTime() )
			{
				guess_m_pInventoryHint->setPos( x, (int)( ( gEngfuncs.GetClientTime() - guess_m_flHintSlideInTime ) / guess_m_flHintSlideInLength * 160 ) - 154 );
				if ( y < -161 )
				{
					guess_m_bInventoryHint = false;
					y = -161;
				}
			}
		}

		if ( gEngfuncs.GetClientTime() >= guess_m_flHintSlideOutTime )
		{
			if ( guess_m_flHintSlideOutTime + guess_m_flHintSlideOutLength > gEngfuncs.GetClientTime() )
			{
				float flFrac = ( gEngfuncs.GetClientTime() - guess_m_flHintSlideOutTime ) / guess_m_flHintSlideOutLength;
				if ( flFrac > 1.0f )
					guess_m_bInventoryHint = false;
				guess_m_pInventoryHint->setPos( x, (int)( flFrac * -160 ) );
				if ( y < -161 )
				{
					guess_m_bInventoryHint = false;
					y = -161;
				}
				else if ( y > 0 )
					y = 0;
			}
		}

		if ( gEngfuncs.GetClientTime() < guess_m_flHintSlideOutTime && ( GetAsyncKeyState( VK_RETURN ) & 0x8000 ) )
		{
			guess_m_flHintSlideInTime = 0;
			guess_m_flHintSlideInLength = 0;
			guess_m_flHintSlideOutTime = gEngfuncs.GetClientTime();
			guess_m_flHintSlideOutLength = 1;
		}
	}

	// health and stamina bars
	int iWide = guess_m_pHealthBarEmpty->getWide();
	int iHealth = gHUD.m_Health.m_iHealth;
	if ( guess_bWheelchairLock )
		iHealth = 0;
	if ( iHealth < 0 )
		iHealth = 0;
	guess_m_pHealthBarEmpty->setSize( iWide, ( 100 - iHealth ) * 1.18 );
	guess_m_pStaminaBarEmpty->setSize( guess_m_pStaminaBarEmpty->getWide(), ( 100 - guess_m_iStamina ) * 1.18 );

	Panel::solve();
}
