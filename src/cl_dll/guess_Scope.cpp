// ======================================
// Cry of Fear telescope scope panel
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Scope.h"
#include "..\game_shared\vgui_loadtga.h"

CScope::CScope() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	Panel *pPanel;

	guess_m_pScopeImage = vgui_LoadTGA( "gfx/vgui/telescope_scope.tga" );
	if ( guess_m_pScopeImage )
		guess_m_pScopeImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/telescope_scope.tga!\n" );

	guess_m_pScopeImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	// black borders around the scope picture
	pPanel = new Panel( 0, 0, 0, 0 );
	pPanel->setParent( this );
	pPanel->setSize( ScreenWidth / 2 - wide / 2, ScreenHeight );
	pPanel->setBgColor( 0, 0, 0, 0 );
	pPanel->setVisible( true );

	pPanel = new Panel( 0, 0, 0, 0 );
	pPanel->setParent( this );
	pPanel->setSize( ScreenWidth / 2 - wide / 2, ScreenHeight );
	pPanel->setPos( ScreenWidth / 2 + wide / 2, 0 );
	pPanel->setBgColor( 0, 0, 0, 0 );
	pPanel->setVisible( true );

	pPanel = new Panel( 0, 0, 0, 0 );
	pPanel->setParent( this );
	pPanel->setSize( wide, ScreenHeight / 2 - tall / 2 );
	pPanel->setPos( ScreenWidth / 2 - wide / 2, 0 );
	pPanel->setBgColor( 0, 0, 0, 0 );
	pPanel->setVisible( true );

	pPanel = new Panel( 0, 0, 0, 0 );
	pPanel->setParent( this );
	pPanel->setSize( wide, ScreenHeight / 2 - tall / 2 );
	pPanel->setPos( ScreenWidth / 2 - wide / 2, ScreenHeight / 2 + tall / 2 );
	pPanel->setBgColor( 0, 0, 0, 0 );
	pPanel->setVisible( true );

	guess_m_pScopePanel = new ImagePanel( guess_m_pScopeImage );
	guess_m_pScopePanel->setParent( this );
	guess_m_pScopePanel->setPos( x, y );
	guess_m_pScopePanel->setVisible( true );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CScope::Initialize()
{
	guess_m_iScope = 0;
	setVisible( false );
}

int guess_MsgScope( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pScope )
	{
		int iScope = READ_BYTE();
		gViewPort->guess_m_pScope->guess_m_iScope = iScope;

		if ( iScope )
		{
			switch ( iScope )
			{
			case 1:
				gViewPort->guess_m_pScope->guess_m_pScopePanel->setImage( vgui_LoadTGA( "gfx/vgui/telescope_scope.tga" ) );
				break;
			case 2:
				gViewPort->guess_m_pScope->guess_m_pScopePanel->setImage( vgui_LoadTGA( "gfx/vgui/telescope_scope.tga" ) );
				break;
			default:
				gViewPort->guess_m_pScope->guess_m_pScopePanel->setImage( vgui_LoadTGA( "gfx/vgui/telescope_scope.tga" ) );
				break;
			}

			if ( !gViewPort->guess_m_pScope->isVisible() )
				gViewPort->guess_m_pScope->setVisible( true );
		}
		else
		{
			if ( gViewPort->guess_m_pScope->isVisible() )
				gViewPort->guess_m_pScope->setVisible( false );
		}
	}

	return 1;
}

void guess_ScopeInit()
{
	gEngfuncs.pfnHookUserMsg( "Scope", guess_MsgScope );
}
