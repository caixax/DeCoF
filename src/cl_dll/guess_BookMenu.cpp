// ======================================
// Cry of Fear book menu
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "com_model.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_BookMenu.h"
#include "guess_Unlockables.h"
#include "..\game_shared\vgui_loadtga.h"

#define SHIELD_ON	0
#define SHIELD_OFF	1

extern cl_entity_t g_HeadShieldEnt;
extern int g_HeadShieldState;
extern int guess_g_iHeadShieldType;

CBookMenu::CBookMenu() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	CommandButton *pButton;

	guess_m_pCancelImage = vgui_LoadTGA( "gfx/vgui/640_notecancel.tga" );
	if ( guess_m_pCancelImage )
		guess_m_pCancelImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_notecancel.tga!\n" );

	guess_m_pArrowsImage = vgui_LoadTGA( "gfx/vgui/left_right.tga" );
	if ( guess_m_pArrowsImage )
		guess_m_pArrowsImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/left_right.tga!\n" );

	// the cancel picture and its button are placed at (wide, tall)
	guess_m_pCancelImage->getSize( wide, tall );

	guess_m_pCancel = new ImagePanel( guess_m_pCancelImage );
	guess_m_pCancel->setParent( this );
	guess_m_pCancel->setPos( wide, tall );
	guess_m_pCancel->setVisible( true );

	pButton = new CommandButton( "", wide, tall, wide, tall, true, true );
	pButton->setParent( this );
	pButton->addActionSignal( new CBookMenuHandler_Command( this, 3 ) );

	guess_m_pArrowsImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight - tall * 2;

	guess_m_pArrows = new ImagePanel( guess_m_pArrowsImage );
	guess_m_pArrows->setParent( this );
	guess_m_pArrows->setPos( x, y );
	guess_m_pArrows->setVisible( true );

	pButton = new CommandButton( "", x, y, 100, 50, true, true );
	pButton->setParent( this );
	pButton->addActionSignal( new CBookMenuHandler_Command( this, 1 ) );

	pButton = new CommandButton( "", x + 100, y, 100, 50, true, true );
	pButton->setParent( this );
	pButton->addActionSignal( new CBookMenuHandler_Command( this, 2 ) );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

CBookMenu::~CBookMenu()
{
	delete guess_m_pArrowsImage;
	delete guess_m_pCancelImage;
}

void CBookMenu::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 1:
		if ( gHUD.guess_m_bMenuClick && guess_g_iHeadShieldType == 7 && gEngfuncs.GetClientTime() > gHUD.guess_m_flMenuClickTime )
		{
			switch ( g_HeadShieldEnt.curstate.sequence )
			{
			case 2:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 7;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			case 4:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 6;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			case 6:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 7;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			}
		}
		break;
	case 2:
		if ( gHUD.guess_m_bMenuClick && guess_g_iHeadShieldType == 7 && gEngfuncs.GetClientTime() > gHUD.guess_m_flMenuClickTime )
		{
			switch ( g_HeadShieldEnt.curstate.sequence )
			{
			case 0:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 2;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			case 1:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 2;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			case 2:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 4;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			case 6:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 4;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			case 7:
				g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
				g_HeadShieldEnt.curstate.sequence = 2;
				gHUD.guess_m_flMenuClickTime = gEngfuncs.GetClientTime() + 1;
				gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
				break;
			}
		}
		break;
	case 3:
		if ( gViewPort && gViewPort->guess_m_pUnlockables )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();

			gHUD.guess_m_bMenuClick = false;
			guess_g_iHeadShieldType = 0;
			g_HeadShieldState = SHIELD_OFF;

			gViewPort->guess_m_pUnlockables->setVisible( true );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		}
		break;
	}
}

void CBookMenu::Initialize()
{
	setVisible( false );
}

void CBookMenu::setVisible( bool state )
{
	if ( state )
	{
		if ( guess_g_iHeadShieldType != 7 )
			guess_g_iHeadShieldType = 7;

		g_HeadShieldState = SHIELD_ON;
		gHUD.guess_m_bMenuClick = true;
		gEngfuncs.pfnPlaySoundByName( "common/page_turn.wav", 1 );
		g_HeadShieldEnt.curstate.animtime = gEngfuncs.GetClientTime();
		g_HeadShieldEnt.curstate.sequence = 0;
	}

	Panel::setVisible( state );
}
