// ======================================
// Cry of Fear cinematic black bars
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_HUDControl.h"
#include "guess_Credits.h"

extern int guess_g_iLetterboxHeight;

DECLARE_MESSAGE( guess_m_BlackBar, BlackBars )

int CBlackBar::Init( void )
{
	HOOK_MESSAGE( BlackBars );

	m_iFlags |= HUD_ACTIVE;
	gHUD.AddHudElem( this );

	return 1;
}

int CBlackBar::VidInit( void )
{
	guess_m_HUD_blackbar = gHUD.GetSpriteIndex( "cof_blackbar" );
	int iHeight = gHUD.GetSpriteRect( guess_m_HUD_blackbar ).bottom - gHUD.GetSpriteRect( guess_m_HUD_blackbar ).top;
	guess_m_iBottom = ScreenHeight;
	guess_m_iTop = -iHeight - 1;

	return 1;
}

int CBlackBar::Draw( float flTime )
{
	return 1;
}

int CBlackBar::MsgFunc_BlackBars( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	guess_m_iOn = READ_BYTE();

	if ( guess_m_iOn )
	{
		int iHeight = gHUD.GetSpriteRect( guess_m_HUD_blackbar ).bottom - gHUD.GetSpriteRect( guess_m_HUD_blackbar ).top;
		guess_m_iBottom = ScreenHeight;
		guess_m_iTop = -iHeight - 1;

		if ( gViewPort )
		{
			if ( gViewPort->guess_m_pHUDControl )
			{
				int x, y, x2, y2;

				gViewPort->guess_m_pHUDControl->guess_m_pLetterboxTop->getPos( x, y );
				gViewPort->guess_m_pHUDControl->guess_m_pLetterboxBottom->getPos( x2, y2 );
				gViewPort->guess_m_pHUDControl->guess_m_pLetterboxTop->setPos( x, -guess_g_iLetterboxHeight );
				gViewPort->guess_m_pHUDControl->guess_m_pLetterboxBottom->setPos( x2, ScreenHeight );
			}

			if ( gViewPort && gViewPort->guess_m_pCredits && gViewPort->guess_m_pCredits->isVisible() )
			{
				int x, y, x2, y2;

				gViewPort->guess_m_pCredits->guess_m_pLetterboxTop->getPos( x, y );
				gViewPort->guess_m_pCredits->guess_m_pLetterboxBottom->getPos( x2, y2 );
				gViewPort->guess_m_pCredits->guess_m_pLetterboxTop->setPos( x, -guess_g_iLetterboxHeight );
				gViewPort->guess_m_pCredits->guess_m_pLetterboxBottom->setPos( x2, ScreenHeight );
			}
		}
	}

	return 1;
}
