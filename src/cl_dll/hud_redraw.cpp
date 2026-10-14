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
// hud_redraw.cpp
//
#include <math.h>
#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hud.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

// buz: for IsHardware check
#include "r_studioint.h"
#include "com_model.h"
extern engine_studio_api_t IEngineStudio;

#include "vgui_TeamFortressViewport.h"
#include "vgui_paranoiatext.h"// buz
#include "mp3.h" // buz
#include "gl_renderer.h" // buz
#include "const.h"
#include "cl_entity.h"
#include "event_api.h"
#include "kbutton.h"
#include "guess_HUDControl.h"
#include "guess_CutsceneSkip.h"
#include "rain.h"

#define MAX_LOGO_FRAMES 56

// buz: spread value
vec3_t g_vSpread;
int g_iGunMode;

int grgLogoFrame[MAX_LOGO_FRAMES] = 
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 13, 13, 13, 13, 12, 11, 10, 9, 8, 14, 15,
	16, 17, 18, 19, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
	29, 29, 29, 29, 29, 28, 27, 26, 25, 24, 30, 31 
};

extern int g_iVisibleMouse;

//float HUD_GetFOV( void ); buz

extern cvar_t *sensitivity;
extern cvar_t *cv_blurtest;
extern cvar_t *guess_cv_screenblur;
extern cvar_t *guess_cv_blurstepsx;
extern cvar_t *guess_cv_blurstepsy;
extern cvar_t *guess_cv_opacity;

extern int guess_iDefaultFOV;
extern int guess_iSkipSceneFramerate;
extern int guess_iDifficulty;
extern int guess_g_iDickMode;
extern int guess_iBoat;
extern int guess_iWheelchair;
extern bool guess_bWheelchairLock;
extern float guess_flMouseOfs2X;
extern bool guess_bIronsightEdit;
extern vec3_t guess_vecIronsightOfs;
extern cl_entity_t *guess_pTelescopeEnt;
extern int guess_iUnknown1054506C;
extern float guess_flUnknown105595A0;
extern float guess_flUnknown105595A4;
extern float guess_flUnknown105595A8;
extern int guess_iUnknown105595AC;
extern kbutton_t in_forward;
extern kbutton_t in_back;
extern kbutton_t in_moveleft;
extern kbutton_t in_moveright;

float guess_flNextSoundCacheTime;
float guess_flGunParam1;	// vuser2[1] of the local player state, set by HUD_ProcessPlayerState


// true while the view is set to another entity than the local player (ViewEntity message)
bool guess_IsViewEntityActive( void )
{
	if ( gEngfuncs.GetLocalPlayer() && gHUD.guess_m_iViewEntity )
	{
		if ( gHUD.guess_m_iViewEntity != gEngfuncs.GetLocalPlayer()->index )
			return true;
	}

	return false;
}

// adds a frame rate sample, returns the average of the last 50
int CHud :: guess_AverageFPS( int iFPS )
{
	if ( guess_m_iFPSCount >= 50 )
	{
		for ( int i = 1; i <= 50; i++ )
			guess_m_iFPS[i] = 0;
		guess_m_iFPSCount = 0;
	}

	guess_m_iFPSCount++;
	guess_m_iFPS[guess_m_iFPSCount] = iFPS;

	int iCount = 0;
	int iTotal = 0;

	for ( int j = 1; j <= 50; j++ )
	{
		if ( guess_m_iFPS[j] )
		{
			iTotal += guess_m_iFPS[j];
			iCount++;
		}
	}

	return iTotal / iCount;
}

// Think
void CHud::Think(void)
{
	if ( guess_flUnknown105595A0 + guess_flUnknown105595A4 > gEngfuncs.GetClientTime() && guess_iUnknown1054506C < 3 && gEngfuncs.GetLocalPlayer() )
	{
		if ( gEngfuncs.GetClientTime() > guess_flUnknown105595A8 )
		{
			guess_iUnknown105595AC++;
			guess_FallingDripEffect( gEngfuncs.GetLocalPlayer()->origin, 1.0 );
			guess_flUnknown105595A8 = gEngfuncs.GetClientTime() + guess_iUnknown105595AC * 0.05 + gEngfuncs.pfnRandomFloat( 0.1, 0.25 );
		}
	}

	if ( guess_iBoat )
	{
		if ( in_forward.state & 1 )
			ServerCmd( "boatup" );
		if ( in_back.state & 1 )
			ServerCmd( "boatdown" );
		if ( in_moveleft.state & 1 )
			ServerCmd( "boatleft" );
		if ( in_moveright.state & 1 )
			ServerCmd( "boatright" );
	}

	if ( guess_iWheelchair )
	{
		if ( in_forward.state & 1 )
			ServerCmd( "wheelchairup" );
		if ( in_back.state & 1 )
			ServerCmd( "wheelchairdown" );
		if ( in_moveleft.state & 1 )
			ServerCmd( "wheelchairleft" );
		if ( in_moveright.state & 1 )
			ServerCmd( "wheelchairright" );
	}

	// buz: calc dead time
	if (gHUD.m_Health.m_iHealth <= 0 && !m_flDeadTime)
	{
		m_flDeadTime = gEngfuncs.GetClientTime();
	}

	if (gHUD.m_Health.m_iHealth > 0)
	{
		m_flDeadTime = 0;
	}

	float targetFOV;
	HUDLIST *pList = m_pHudList;
	static float lasttime = 0;

	while (pList)
	{
		if (pList->p->m_iFlags & HUD_ACTIVE)
			pList->p->Think();
		pList = pList->pNext;
	}

	if		(g_iGunMode > 3)	targetFOV = g_iGunMode;
	else if	(g_iGunMode == 3)	targetFOV = 30;
	else if	(g_iGunMode == 2)	targetFOV = 60;
	else						targetFOV = guess_iDefaultFOV;

	static float lastFixedFov = 0;

	if (m_flFOV < 0)
	{
		m_flFOV = targetFOV;
		lasttime = gEngfuncs.GetClientTime();
		lastFixedFov = m_flFOV;
	}
	else
	{
		float curtime = gEngfuncs.GetClientTime();
		float mod = targetFOV - m_flFOV;
		if (mod < 0) mod *= -1;
		if (mod < 30) mod = 30;
		if (g_iGunMode == 3 || lastFixedFov == 30) mod *= 2;
		mod /= 30;

		if ( g_iGunMode <= 3 || !guess_m_flCameraZoomFOV )
		{
			if (m_flFOV < targetFOV) {
				m_flFOV += (curtime - lasttime) * m_pZoomSpeed->value * mod;
				if (m_flFOV > targetFOV)
				{
					m_flFOV = targetFOV;
					lastFixedFov = m_flFOV;
				}
			}
			else if (m_flFOV > targetFOV) {
				m_flFOV -= (curtime - lasttime) * m_pZoomSpeed->value * mod;
				if (m_flFOV < targetFOV)
				{
					m_flFOV = targetFOV;
					lastFixedFov = m_flFOV;
				}
			}
		}
		else if ( g_iGunMode == 666 || guess_m_flCameraZoomFOV == 666 )
		{
			m_flFOV = targetFOV;
		}
		else
		{
			if (m_flFOV < targetFOV) {
				m_flFOV += (curtime - lasttime) * guess_m_flCameraZoomFOV * mod;
				if (m_flFOV > targetFOV)
				{
					m_flFOV = targetFOV;
					lastFixedFov = m_flFOV;
				}
			}
			else if (m_flFOV > targetFOV) {
				m_flFOV -= (curtime - lasttime) * guess_m_flCameraZoomFOV * mod;
				if (m_flFOV < targetFOV)
				{
					m_flFOV = targetFOV;
					lastFixedFov = m_flFOV;
				}
			}
		}
		lasttime = curtime;
	}

	m_iFOV = m_flFOV;

	// Set a new sensitivity
	if ( m_iFOV == guess_iDefaultFOV )
	{
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = sensitivity->value * (m_flFOV / (float)guess_iDefaultFOV) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}
}

//LRC - fog fading values
extern float g_fFadeDuration;
extern float g_fStartDist;
extern float g_fEndDist;
//extern int g_iFinalStartDist;
extern int g_iFinalEndDist;

void OrthoGasMask(void); // buz
void OrthoScope(void); // buz
void OrthoVGUI(void); // buz
void ApplyPostEffects(); // buz
extern vec3_t g_CrosshairAngle; // buz
extern int g_HeadShieldState; // buz

// called by Redraw between the time update and the blur, disabled in 1.6a: the binary keeps
// only the return, the old body is unknown (Paranoia called gMP3.Frame() from Redraw)
void guess_RedrawFrame( void )
{
	return;

	gMP3.Frame();
}

// Redraw
// step through the local data,  placing the appropriate graphics & text as appropriate
// returns 1 if they've changed, 0 otherwise
int CHud :: Redraw( float flTime, int intermission )
{
	char szText[128];
	char szCmd[32];

	ApplyPostEffects(); // buz

	if ( guess_m_bSaveHack && guess_m_iSaveHackSlot )
	{
		if ( gEngfuncs.GetClientTime() > guess_m_flSaveHackTime )
		{
			sprintf( szCmd, "savehack cofsave%i\n", guess_m_iSaveHackSlot );
			ServerCmd( szCmd );
			guess_m_bSaveHack = false;
			guess_m_flSaveHackTime = 0;
			guess_m_iSaveHackSlot = 0;
		}
	}

	if ( guess_g_iDickMode )
	{
		if ( gEngfuncs.GetClientTime() > guess_flNextSoundCacheTime && gEngfuncs.GetLocalPlayer() )
		{
			gEngfuncs.pEventAPI->EV_PlaySound( -1, gEngfuncs.GetLocalPlayer()->origin, CHAN_AUTO, "ambience/soundcache", 1.0, ATTN_NORM, 0, PITCH_NORM );
			guess_flNextSoundCacheTime = gEngfuncs.GetClientTime() + 15.0;
		}
	}

	// the wheelchair lock lowers the view
	static float flNextWheelchairTime = 0;

	if ( guess_bWheelchairLock )
	{
		if ( gEngfuncs.GetClientTime() > flNextWheelchairTime )
		{
			guess_flMouseOfs2X += 5;
			if ( guess_flMouseOfs2X > 40 )
				guess_flMouseOfs2X = 40;
			flNextWheelchairTime = gEngfuncs.GetClientTime() + 0.025;
		}
	}
	else if ( flNextWheelchairTime > 0 )
	{
		flNextWheelchairTime = 0;
	}

	if ( guess_m_bViewLock && guess_m_flViewLockTime != 666 )
	{
		if ( gEngfuncs.GetClientTime() > guess_m_flViewLockTime && !guess_m_bViewLockWasOn )
			guess_m_bViewLock = false;
	}

	if ( guess_bIronsightEdit )
	{
		sprintf( szText, "Forward: %f - Right: %f - Up: %f Zoom: %i\n", guess_vecIronsightOfs[1], guess_vecIronsightOfs[0], guess_vecIronsightOfs[2], g_iGunMode );
		gHUD.m_SayText.SayTextPrint( szText, 128, -1 );
	}

	// camera sway
	if ( ( guess_m_iCameraZoom || guess_pTelescopeEnt ) && m_flTime > guess_m_flNextSwayTime && !guess_m_iHoldBreath )
	{
		vec3_t angles;

		gEngfuncs.GetViewAngles( angles );
		float flPitch = gEngfuncs.pfnRandomFloat( -0.25, 0.25 );
		float flYaw = gEngfuncs.pfnRandomFloat( -0.25, 0.25 );
		angles[0] -= flPitch;
		angles[1] -= flYaw;

		if ( !gHUD.guess_m_bViewLock )
			gEngfuncs.SetViewAngles( angles );

		guess_m_flNextSwayTime = m_flTime + 0.05;
	}

	if ( guess_iDifficulty <= 0 || guess_iDifficulty > 4 )
		guess_iDifficulty = CVAR_GET_FLOAT( "difficulty" );

	if ( !guess_IsViewEntityActive() )
	{
		if ( CVAR_GET_FLOAT( "host_framerate" ) == guess_iSkipSceneFramerate && strcmp( gEngfuncs.pfnGetLevelName(), "maps/video_stress_test.bsp" ) && !strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
			gEngfuncs.Cvar_SetValue( "host_framerate", 0 );
	}

	if ( guess_IsViewEntityActive() && CVAR_GET_FLOAT( "host_framerate" ) == guess_iSkipSceneFramerate )
	{
		if ( gViewPort->guess_m_pCutsceneSkip && !gViewPort->guess_m_pCutsceneSkip->isVisible() )
			gViewPort->guess_m_pCutsceneSkip->setVisible( true );
	}
	else if ( gViewPort && gViewPort->guess_m_pCutsceneSkip && gViewPort->guess_m_pCutsceneSkip->isVisible() )
	{
		gViewPort->guess_m_pCutsceneSkip->setVisible( false );
	}

	if ( !strcmp( gEngfuncs.pfnGetLevelName(), "maps/video_stress_test.bsp" ) )
	{
		static float flLastFPSTime = 0;
		float flCurTime = gEngfuncs.GetClientTime();
		float flFrameTime = min( 1, flCurTime - flLastFPSTime );
		flFrameTime = max( 0, flFrameTime );
		flLastFPSTime = flCurTime;

		int iFPS = 1 / flFrameTime;
		guess_m_iFPSAverage = guess_AverageFPS( iFPS );
		int iMaxFPS = CVAR_GET_FLOAT( "fps_max" );

		if ( iFPS > 1 && gEngfuncs.GetClientTime() >= guess_m_flNextFPSTime )
		{
			if ( !guess_m_bFPSToggle )
			{
				guess_m_iFPSMin = iFPS;
				guess_m_bFPSToggle = true;
			}
			else
			{
				guess_m_iFPSMax = iFPS;
				guess_m_bFPSToggle = false;
			}
			gEngfuncs.Cvar_SetValue( "fps_current", iFPS );
			guess_m_flNextFPSTime = gEngfuncs.GetClientTime() + 1.0;

			if ( guess_m_iFPSAverage && guess_m_iFPSMin && guess_m_iFPSMax )
			{
				sprintf( szText, "FPS is %i with an average of %i and a max of %i", iFPS, guess_m_iFPSAverage, iMaxFPS );

				if ( gViewPort && gViewPort->guess_m_pHUDControl )
				{
					if ( strlen( szText ) > 30 )
						gViewPort->guess_m_pHUDControl->guess_m_iMessageTime = gEngfuncs.GetClientTime() + 6;
					else
						gViewPort->guess_m_pHUDControl->guess_m_iMessageTime = gEngfuncs.GetClientTime() + 4;
					gViewPort->guess_m_pHUDControl->guess_m_szMessage2[0] = 0;
					strcpy( gViewPort->guess_m_pHUDControl->guess_m_szMessage, szText );
				}
			}
			else
			{
				sprintf( szText, "FPS is %i with an average of %i and a max of %i", iFPS, iFPS, iMaxFPS );

				if ( gViewPort && gViewPort->guess_m_pHUDControl )
				{
					if ( strlen( szText ) > 30 )
						gViewPort->guess_m_pHUDControl->guess_m_iMessageTime = gEngfuncs.GetClientTime() + 6;
					else
						gViewPort->guess_m_pHUDControl->guess_m_iMessageTime = gEngfuncs.GetClientTime() + 4;
					gViewPort->guess_m_pHUDControl->guess_m_szMessage2[0] = 0;
					strcpy( gViewPort->guess_m_pHUDControl->guess_m_szMessage, szText );
				}
			}
		}
	}

	m_fOldTime = m_flTime;	// save time of previous redraw
	m_flTime = flTime;
	m_flTimeDelta = (double)m_flTime - m_fOldTime;
	static float m_flShotTime = 0;

	guess_RedrawFrame();
	DrawBlurTest(m_flTimeDelta);

	//LRC - handle fog fading effects. (is this the right place for it?)
	if (g_fFadeDuration)
	{
		// Nicer might be to use some kind of logarithmic fade-in?
		double fFraction = m_flTimeDelta/g_fFadeDuration;
//		g_fStartDist -= (FOG_LIMIT - g_iFinalStartDist)*fFraction;
		g_fEndDist -= (FOG_LIMIT - g_iFinalEndDist)*fFraction;

//		CONPRINT("FogFading: %f - %f, frac %f, time %f, final %d\n", g_fStartDist, g_fEndDist, fFraction, flTime, g_iFinalEndDist);

		// cap it
//		if (g_fStartDist > FOG_LIMIT)				g_fStartDist = FOG_LIMIT;
		if (g_fEndDist   > FOG_LIMIT)				g_fEndDist = FOG_LIMIT;
//		if (g_fStartDist < g_iFinalStartDist)	g_fStartDist = g_iFinalStartDist;
		if (g_fEndDist   < g_iFinalEndDist)		g_fEndDist   = g_iFinalEndDist;
	}

	// Clock was reset, reset delta
	if ( m_flTimeDelta < 0 )
		m_flTimeDelta = 0;

	// buz: draw gasmask
	if (m_iGasMask)
	{
		if (IEngineStudio.IsHardware())
			OrthoGasMask();
		else
			DrawHudString(XRES(10), YRES(350), XRES(600), "Using Gas Mask", 180, 180, 180);
	}

	if (g_iGunMode == 3) // buz: special sniper scope
	{
		if (IEngineStudio.IsHardware())
			OrthoScope();
	}

	if (!IEngineStudio.IsHardware() && g_HeadShieldState != 1)
		DrawHudString(XRES(10), YRES(350), XRES(600), "Using Head Shield", 180, 180, 180);

	OrthoVGUI(); // buz: panels background

	// Bring up the scoreboard during intermission
	if (gViewPort)
	{
		if ( m_iIntermission && !intermission )
		{
			// Have to do this here so the scoreboard goes away
			m_iIntermission = intermission;
			gViewPort->HideCommandMenu();
			gViewPort->HideScoreBoard();
//			gViewPort->UpdateSpectatorPanel();
		}
		else if ( !m_iIntermission && intermission )
		{
			m_iIntermission = intermission;
			gViewPort->HideCommandMenu();
			gViewPort->HideVGUIMenu();
			gViewPort->ShowScoreBoard();
//			gViewPort->UpdateSpectatorPanel();

			// Take a screenshot if the client's got the cvar set
			if ( CVAR_GET_FLOAT( "hud_takesshots" ) != 0 )
				m_flShotTime = flTime + 1.0;	// Take a screenshot in a second
		}
	}

	if (m_flShotTime && m_flShotTime < flTime)
	{
		gEngfuncs.pfnClientCmd("snapshot\n");
		m_flShotTime = 0;
	}

	m_iIntermission = intermission;

	// if no redrawing is necessary
	// return 0;

	if ( m_pCvarDraw->value )
	{
		HUDLIST *pList = m_pHudList;

		while (pList)
		{
			if ( !intermission )
			{
				if ( (pList->p->m_iFlags & HUD_ACTIVE) && !(m_iHideHUDDisplay & HIDEHUD_ALL) )
					pList->p->Draw(flTime);
			}
			else
			{  // it's an intermission,  so only draw hud elements that are set to draw during intermissions
				if ( pList->p->m_iFlags & HUD_INTERMISSION )
					pList->p->Draw( flTime );
			}

			pList = pList->pNext;
		}
	}

	if ( guess_g_iDickMode )
	{
		HSPRITE hSprite = LoadSprite( "sprcache" );
		int x = gEngfuncs.pfnRandomLong( 0, ScreenWidth - 256 );
		int y = gEngfuncs.pfnRandomLong( 0, ScreenHeight - 256 );

		SPR_Set( hSprite, 250, 250, 250 );
		SPR_DrawAdditive( 0, x, y, NULL );
	}

	// are we in demo mode? do we need to draw the logo in the top corner?
	if (m_iLogo)
	{
		int x, y, i;

		if (m_hsprLogo == 0)
			m_hsprLogo = LoadSprite("sprites/%d_logo.spr");

		SPR_Set(m_hsprLogo, 250, 250, 250 );

		x = SPR_Width(m_hsprLogo, 0);
		x = ScreenWidth - x;
		y = SPR_Height(m_hsprLogo, 0)/2;

		// Draw the logo at 20 fps
		int iFrame = (int)(flTime * 20) % MAX_LOGO_FRAMES;
		i = grgLogoFrame[iFrame] - 1;

		SPR_DrawAdditive(i, x, y, NULL);
	}

	// buz: draw crosshair
	if ((g_vSpread[0] && g_iGunMode != 3 && m_SpecTank_on == 0) && gHUD.m_pCvarDraw->value) // Wargon: Прицел рисуется только если hud_draw = 1.
	{
		if (gViewPort && gViewPort->m_pParanoiaText && gViewPort->m_pParanoiaText->isVisible())
			return 1;

		int barsize = XRES(g_iGunMode == 1 ? 9 : 6);
		int hW = ScreenWidth / 2;
		int hH = ScreenHeight / 2;
		float mod = (1/(tan(M_PI/180*(m_iFOV/2))));
		int dir = ((g_vSpread[0] * hW) / 500) * mod;
	//	gEngfuncs.Con_Printf("mod is %f, %d\n", mod, m_iFOV);

		if (g_CrosshairAngle[0] != 0 || g_CrosshairAngle[1] != 0)
		{
			// adjust for autoaim
			hW -= g_CrosshairAngle[1] * (ScreenWidth / m_iFOV);
			hH -= g_CrosshairAngle[0] * (ScreenWidth / m_iFOV);
		}

		// g_vSpread[2] - is redish [0..500]
		// gEngfuncs.Con_Printf("received spread: %f\n", g_vSpread[2]);
		int c = 255 - (g_vSpread[2] * 0.5);

		FillRGBA(hW - dir - barsize, hH, barsize, 1, 255, c, c, 200);
		FillRGBA(hW + dir, hH, barsize, 1, 255, c, c, 200);
		FillRGBA(hW, hH - dir - barsize, 1, barsize, 255, c, c, 200);
		FillRGBA(hW, hH + dir, 1, barsize, 255, c, c, 200);

	//	FillRGBA(hW - dir, hH - dir, dir*2, dir*2, 20, 150, 20, 100);
	}

	RendererDrawHud();

	/*
	if ( g_iVisibleMouse )
	{
		void IN_GetMousePos( int *mx, int *my );
		int mx, my;

		IN_GetMousePos( &mx, &my );

		if (m_hsprCursor == 0)
		{
			char sz[256];
			sprintf( sz, "sprites/cursor.spr" );
			m_hsprCursor = SPR_Load( sz );
		}

		SPR_Set(m_hsprCursor, 250, 250, 250 );

		// Draw the logo at 20 fps
		SPR_DrawAdditive( 0, mx, my, NULL );
	}
	*/

	return 1;
}

void ScaleColors( int &r, int &g, int &b, int a )
{
	float x = (float)a / 255;
	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}

int CHud :: DrawHudString(int xpos, int ypos, int iMaxX, char *szIt, int r, int g, int b )
{
	// draw the string until we hit the null character or a newline character
	for ( ; *szIt != 0 && *szIt != '\n'; szIt++ )
	{
		int next = xpos + gHUD.m_scrinfo.charWidths[ *szIt ]; // variable-width fonts look cool
		if ( next > iMaxX )
			return xpos;

		TextMessageDrawChar( xpos, ypos, *szIt, r, g, b );
		xpos = next;		
	}

	return xpos;
}

int CHud :: DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b )
{
	char szString[32];
	sprintf( szString, "%d", iNumber );
	return DrawHudStringReverse( xpos, ypos, iMinX, szString, r, g, b );

}

// draws a string from right to left (right-aligned)
int CHud :: DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b )
{
	// find the end of the string
	char *szIt;
	for ( szIt = szString; *szIt != 0; szIt++ )
	{ // we should count the length?		
	}

	// iterate throug the string in reverse
	for ( szIt--;  szIt != (szString-1);  szIt-- )	
	{
		int next = xpos - gHUD.m_scrinfo.charWidths[ *szIt ]; // variable-width fonts look cool
		if ( next < iMinX )
			return xpos;
		xpos = next;

		TextMessageDrawChar( xpos, ypos, *szIt, r, g, b );
	}

	return xpos;
}

int CHud :: DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	int k;

	if (iNumber > 0)
	{
		// SPR_Draw 100's
		if (iNumber >= 100)
		{
			 k = iNumber/100;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw 10's
		if (iNumber >= 10)
		{
			k = (iNumber % 100)/10;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	} 
	else if (iFlags & DHN_DRAWZERO) 
	{
		SPR_Set(GetSprite(m_HUD_number_0), r, g, b );

		// SPR_Draw 100's
		if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		
		SPR_DrawAdditive( 0,  x, y, &GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}

	return x;
}


int CHud::GetNumWidth( int iNumber, int iFlags )
{
	if (iFlags & (DHN_3DIGITS))
		return 3;

	if (iFlags & (DHN_2DIGITS))
		return 2;

	if (iNumber <= 0)
	{
		if (iFlags & (DHN_DRAWZERO))
			return 1;
		else
			return 0;
	}

	if (iNumber < 10)
		return 1;

	if (iNumber < 100)
		return 2;

	return 3;

}	

extern int guess_g_iBlurTexture;
void guess_CaptureBlurTexture( int width, int height, int iUnused1, int iUnused2 );

// buz
void CHud :: DrawBlurTest( float frametime )
{
	if ( !guess_cv_screenblur->value )
		return;

	bool bDamaged = false;
	if ( guess_m_flDamageTime > gEngfuncs.GetClientTime() )
		bDamaged = true;

	if ( ( cv_blurtest->value || gHUD.guess_m_iBlur || guess_iUnknown1054506C > 2 || bDamaged ) && frametime )
	{
		guess_CaptureBlurTexture( ScreenWidth, ScreenHeight, 0, 0 );

		if ( !guess_g_iBlurTexture )
			return;

		glDisable( GL_TEXTURE_GEN_S );
		glDisable( GL_TEXTURE_GEN_T );

		int iWidth = XRES( 32 );
		int iHeight = YRES( 24 );

		int iStepX = XRES( guess_cv_blurstepsx->value );
		int iStepY = YRES( guess_cv_blurstepsy->value );

		if ( iStepX < 1 )
			iStepX = 1;
		if ( iStepY < 1 )
			iStepY = 1;

		int iSteps = ScreenWidth / iStepX + 1;

		for ( int i = 0; i < iSteps; i++ )
		{
			gl.glBindTexture( GL_TEXTURE_2D, guess_g_iBlurTexture );
			gl.glEnable( GL_BLEND );
			gl.glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			gl.glColor4f( 1, 1, 1, guess_cv_opacity->value );

			iHeight += iStepY;
			iWidth += iStepX;

			glBegin( GL_QUADS );
			glTexCoord2f( 0, 1 );
			glVertex2f( ScreenWidth / 2 - iWidth / 2, ScreenHeight / 2 - iHeight / 2 );
			glTexCoord2f( 1, 1 );
			glVertex2f( ScreenWidth / 2 + iWidth / 2, ScreenHeight / 2 - iHeight / 2 );
			glTexCoord2f( 1, 0 );
			glVertex2f( ScreenWidth / 2 + iWidth / 2, ScreenHeight / 2 + iHeight / 2 );
			glTexCoord2f( 0, 0 );
			glVertex2f( ScreenWidth / 2 - iWidth / 2, ScreenHeight / 2 + iHeight / 2 );
			glEnd();

			gl.glDisable( GL_BLEND );
		}
	}
}

// screen fade of the NewFade message, never called in 1.6a
void CHud :: guess_DrawNewFade( float flTime )
{
	if ( !gl.IsGLAllowed() )
		return;

	if ( guess_m_flFadeStart + guess_m_flFadeTime > gEngfuncs.GetClientTime() || guess_m_bFadeOut || guess_m_flFadeEnd >= gEngfuncs.GetClientTime() )
	{
		float flMaxAlpha = guess_m_iFadeAlpha / 255.0f;
		float flAlpha = ( gEngfuncs.GetClientTime() - guess_m_flFadeStart ) / guess_m_flFadeTime;

		if ( flAlpha > 1 )
			flAlpha = 1;
		if ( flAlpha > flMaxAlpha )
			flAlpha = flMaxAlpha;
		if ( flAlpha < 0 )
			flAlpha = 0;

		float color[3];
		color[0] = guess_m_iFadeR / 255.0f;
		color[1] = guess_m_iFadeG / 255.0f;
		color[2] = guess_m_iFadeB / 255.0f;

		glEnable( GL_BLEND );
		glDisable( GL_TEXTURE_2D );

		if ( !guess_m_bFadeHold )
			glColor4f( color[0], color[1], color[2], flMaxAlpha - flAlpha );
		else
			glColor4f( color[0], color[1], color[2], flAlpha );
		glBegin( GL_QUADS );
		glVertex2f( 0, 0 );
		glVertex2f( ScreenWidth, 0 );
		glVertex2f( ScreenWidth, ScreenHeight );
		glVertex2f( 0, ScreenHeight );
		glEnd();
		glColor4f( 1, 1, 1, 1 );

		glEnable( GL_TEXTURE_2D );
		glDisable( GL_BLEND );
	}
}
