// ======================================
// Cry of Fear cutscene skip panel
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_CutsceneSkip.h"
#include "..\game_shared\vgui_loadtga.h"

CCutsceneSkip::CCutsceneSkip() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	Panel *pPanel;

	guess_m_pDotsImage = vgui_LoadTGA( "gfx/vgui/loading_dots.tga" );
	if ( guess_m_pDotsImage )
		guess_m_pDotsImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/loading_dots.tga!\n" );

	guess_m_pDotsImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	// black borders around the picture
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

	guess_m_pDotsPanel = new ImagePanel( guess_m_pDotsImage );
	guess_m_pDotsPanel->setParent( this );
	guess_m_pDotsPanel->setPos( x, y );
	guess_m_pDotsPanel->setVisible( true );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CCutsceneSkip::Initialize()
{
	setVisible( false );
}
