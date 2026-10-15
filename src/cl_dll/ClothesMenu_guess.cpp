// ======================================
// Cry of Fear clothes menu
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_ClothesMenu.h"
#include "guess_HUDControl.h"

// the custom costume textures are loaded again when these are cleared (studio renderer)
extern bool guess_g_bHoodieLoaded;
extern bool guess_g_bSleeveLoaded;
extern bool guess_g_bJeansLoaded;
extern bool guess_g_bFaceLoaded;

void guess_SetCustomHoodie();

CClothesMenu::CClothesMenu() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	ColorButton *pButton;
	int y = 0;
	int i;
	int tall = ScreenHeight / 12;
	int wide = ScreenWidth / 2;

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	for ( i = 0; i <= 12; i++ )
	{
		guess_m_pClothesButton[i] = new ColorButton( "", ScreenWidth / 2, y, wide, tall, false, false );
		guess_m_pClothesButton[i]->setParent( this );
		guess_m_pClothesButton[i]->addActionSignal( new CClothesMenuHandler_Command( this, i + 1 ) );
		guess_m_pClothesButton[i]->setFont( pSchemes->getFont( pSchemes->getSchemeHandle( "credits" ) ) );
		guess_m_pClothesButton[i]->setUnArmedBorderColor( 255, 255, 255, 200 );
		guess_m_pClothesButton[i]->setUnArmedColor( 255, 255, 255, 0 );
		guess_m_pClothesButton[i]->setArmedBorderColor( 255, 255, 255, 150 );
		guess_m_pClothesButton[i]->setArmedColor( 255, 0, 0, 32 );
		guess_m_pClothesButton[i]->setBgColor( 0, 0, 0, 128 );
		guess_m_pClothesButton[i]->setFgColor( 255, 255, 255, 128 );
		y += tall;
	}

	pButton = new ColorButton( "Close", 0, ScreenHeight - tall, wide / 2, tall, false, false );
	pButton->setParent( this );
	pButton->addActionSignal( new CClothesMenuHandler_Command( this, 50 ) );
	pButton->setFont( pSchemes->getFont( pSchemes->getSchemeHandle( "credits" ) ) );
	pButton->setUnArmedBorderColor( 255, 255, 255, 200 );
	pButton->setUnArmedColor( 255, 255, 255, 0 );
	pButton->setArmedBorderColor( 255, 255, 255, 150 );
	pButton->setArmedColor( 255, 0, 0, 32 );
	pButton->setBgColor( 0, 0, 0, 128 );
	pButton->setFgColor( 255, 255, 255, 128 );

	pButton = new ColorButton( "Reset", wide / 2, ScreenHeight - tall, wide / 2, tall, false, false );
	pButton->setParent( this );
	pButton->addActionSignal( new CClothesMenuHandler_Command( this, 66 ) );
	pButton->setFont( pSchemes->getFont( pSchemes->getSchemeHandle( "credits" ) ) );
	pButton->setUnArmedBorderColor( 255, 255, 255, 200 );
	pButton->setUnArmedColor( 255, 255, 255, 0 );
	pButton->setArmedBorderColor( 255, 255, 255, 150 );
	pButton->setArmedColor( 255, 0, 0, 32 );
	pButton->setBgColor( 0, 0, 0, 128 );
	pButton->setFgColor( 255, 255, 255, 128 );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CClothesMenu::ActionSignal( int cmd )
{
	if ( cmd != 50 && gEngfuncs.GetClientTime() < guess_m_flNextClick )
		return;

	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		if ( guess_m_iUnlocked & 1 )
		{
			gHUD.guess_m_iClothesState = 1;
			guess_g_bHoodieLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 1" );
		}
		break;
	case 2:
		if ( guess_m_iUnlocked & 2 )
		{
			gHUD.guess_m_iClothesState = 2;
			guess_g_bHoodieLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 2" );
		}
		break;
	case 3:
		if ( guess_m_iUnlocked & 4 )
		{
			gHUD.guess_m_iClothesState = 3;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 3" );
		}
		break;
	case 4:
		if ( guess_m_iUnlocked & 8 )
		{
			gHUD.guess_m_iClothesState = 4;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 4" );
		}
		break;
	case 5:
		if ( guess_m_iUnlocked & 0x10 )
		{
			gHUD.guess_m_iClothesState = 5;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 5" );
		}
		break;
	case 6:
		if ( guess_m_iUnlocked & 0x20 )
		{
			gHUD.guess_m_iClothesState = 6;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 6" );
		}
		break;
	case 7:
		if ( guess_m_iUnlocked & 0x40 )
		{
			gHUD.guess_m_iClothesState = 7;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 7" );
		}
		break;
	case 8:
		if ( guess_m_iUnlocked & 0x80 )
		{
			gHUD.guess_m_iClothesState = 8;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 8" );
		}
		break;
	case 9:
		if ( guess_m_iUnlocked & 0x100 )
		{
			gHUD.guess_m_iClothesState = 9;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 9" );
		}
		break;
	case 10:
		if ( guess_m_iUnlocked & 0x200 )
		{
			gHUD.guess_m_iClothesState = 10;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bJeansLoaded = false;
			guess_g_bFaceLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 10" );
		}
		break;
	case 11:
		if ( guess_m_iUnlocked & 0x400 )
		{
			gHUD.guess_m_iClothesState = 11;
			guess_g_bHoodieLoaded = false;
			guess_g_bSleeveLoaded = false;
			guess_g_bFaceLoaded = false;
			guess_g_bJeansLoaded = false;
			gEngfuncs.pfnServerCmd( "setclothes 11" );
		}
		break;
	case 12:
		if ( guess_m_iUnlocked & 0x800 )
		{
			guess_SetCustomHoodie();
			guess_m_flNextClick = gEngfuncs.GetClientTime() + 0.5;
		}
		break;
	case 50:
		setVisible( false );
		gViewPort->UpdateCursorState();
		gEngfuncs.pfnServerCmd( "closeclothes" );
		break;
	case 66:
		gHUD.guess_m_iClothesState = 0;
		guess_g_bHoodieLoaded = false;
		guess_g_bSleeveLoaded = false;
		guess_g_bJeansLoaded = false;
		guess_g_bFaceLoaded = false;
		gEngfuncs.pfnServerCmd( "setclothes 0" );
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown clothesmenu ActionSignal %i.\n", cmd );
		break;
	}
}

void CClothesMenu::Initialize()
{
	guess_m_iUnlocked = 0;
	setVisible( false );
}

int CClothesMenu::guess_KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && !strcmp( pszCurrentBinding, "toggleconsole" ) )
		return 1;

	return 0;
}

int guess_MsgClothes( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pClothesMenu )
	{
		gViewPort->guess_m_pClothesMenu->guess_m_iUnlocked = READ_LONG();
		gHUD.guess_m_iClothes = READ_BYTE();
		gHUD.guess_m_iClothesState = READ_BYTE();
		int iShow = READ_BYTE();

		gViewPort->guess_m_pClothesMenu->guess_UpdateButtons();

		if ( iShow )
		{
			gViewPort->guess_m_pClothesMenu->setVisible( true );
			gViewPort->UpdateCursorState();
			gViewPort->guess_m_pClothesMenu->guess_m_flNextClick = gEngfuncs.GetClientTime() + 0.5;
		}
	}

	return 1;
}

void CClothesMenu::guess_UpdateButtons()
{
	if ( guess_m_iUnlocked & 1 )
		guess_m_pClothesButton[0]->setText( "David Leatherhoff suit" );
	else
		guess_m_pClothesButton[0]->setText( "Locked" );
	guess_m_pClothesButton[0]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 2 )
		guess_m_pClothesButton[1]->setText( "ModDB hoodie" );
	else
		guess_m_pClothesButton[1]->setText( "Locked" );
	guess_m_pClothesButton[1]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 4 )
		guess_m_pClothesButton[2]->setText( "Hello Kitty suit" );
	else
		guess_m_pClothesButton[2]->setText( "Locked" );
	guess_m_pClothesButton[2]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 8 )
		guess_m_pClothesButton[3]->setText( "Afraid of Monsters suit" );
	else
		guess_m_pClothesButton[3]->setText( "Locked" );
	guess_m_pClothesButton[3]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x10 )
		guess_m_pClothesButton[4]->setText( "Camouflage hoodie" );
	else
		guess_m_pClothesButton[4]->setText( "Locked" );
	guess_m_pClothesButton[4]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x20 )
		guess_m_pClothesButton[5]->setText( "Half-Life Creations hoodie" );
	else
		guess_m_pClothesButton[5]->setText( "Locked" );
	guess_m_pClothesButton[5]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x40 )
		guess_m_pClothesButton[6]->setText( "Black Metal suit" );
	else
		guess_m_pClothesButton[6]->setText( "Locked" );
	guess_m_pClothesButton[6]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x80 )
		guess_m_pClothesButton[7]->setText( "Team Psykskallar hoodie" );
	else
		guess_m_pClothesButton[7]->setText( "Locked" );
	guess_m_pClothesButton[7]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x100 )
		guess_m_pClothesButton[8]->setText( "Awesome suit" );
	else
		guess_m_pClothesButton[8]->setText( "Locked" );
	guess_m_pClothesButton[8]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x200 )
		guess_m_pClothesButton[9]->setText( "Sick Simon suit" );
	else
		guess_m_pClothesButton[9]->setText( "Locked" );
	guess_m_pClothesButton[9]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x400 )
		guess_m_pClothesButton[10]->setText( "AoM Twitcher suit" );
	else
		guess_m_pClothesButton[10]->setText( "Locked" );
	guess_m_pClothesButton[10]->setContentAlignment( Label::a_center );

	if ( guess_m_iUnlocked & 0x800 )
		guess_m_pClothesButton[11]->setText( "Custom hoodie" );
	else
		guess_m_pClothesButton[11]->setText( "Locked" );
	guess_m_pClothesButton[11]->setContentAlignment( Label::a_center );

}

void guess_SetCustomHoodie()
{
	char szCommand[256];

	if ( gEngfuncs.COM_LoadFile( "models/costumes/custom_hoodie.tga", 5, NULL ) )
	{
		gHUD.guess_m_iClothesState = 12;
		guess_g_bHoodieLoaded = false;
		guess_g_bJeansLoaded = false;
		guess_g_bSleeveLoaded = false;
		guess_g_bFaceLoaded = false;
		sprintf( szCommand, "setclothes 12 %s", "custom_hoodie.tga" );
		gEngfuncs.pfnServerCmd( szCommand );
	}
	else
	{
		strcpy( gViewPort->guess_m_pHUDControl->guess_m_szMessage, "To use custom hoodies place a .tga called custom_hoodie.tga in the models\\costumes folder" );
		gViewPort->guess_m_pHUDControl->guess_m_flMessageAlpha = 255;
		gViewPort->guess_m_pHUDControl->guess_m_bMessageFadeOut = false;
		gViewPort->guess_m_pHUDControl->guess_m_bMessageHold = false;
		gViewPort->guess_m_pHUDControl->guess_m_flMessageHoldTime = 0;
		gViewPort->guess_m_pHUDControl->guess_m_iMessageType = 5;
	}
}

void guess_ClothesInit()
{
	gHUD.guess_m_iClothesState = 0;
	gEngfuncs.pfnHookUserMsg( "Clothes", guess_MsgClothes );
}
