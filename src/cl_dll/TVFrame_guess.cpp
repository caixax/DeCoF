// ======================================
// Cry of Fear TV frame panel
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_TVFrame.h"

CTVFrame::CTVFrame() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CTVFrame::Initialize()
{
	guess_m_iTVFrame = 0;
	setVisible( false );
}

int guess_MsgTVFrame( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	// tests the scope panel, not the TV frame
	if ( gViewPort->guess_m_pScope )
		READ_BYTE();

	return 1;
}

void guess_TVFrameInit()
{
	gEngfuncs.pfnHookUserMsg( "TVFrame", guess_MsgTVFrame );
}
