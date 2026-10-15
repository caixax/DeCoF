// ======================================
// Cry of Fear game over panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_GameOver.h"
#include "guess_SaveLoad.h"
#include "..\game_shared\vgui_loadtga.h"

extern int guess_iMenuMode;
extern int guess_iDoctorMode;
extern int guess_iVGUIMenuNoMusic;

void guess_IrrKlangPlay( const char *pszFile );

CGameOver::CGameOver() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	int loadWide, loadTall;
	int exitWide, exitTall;
	int loadX, loadY;
	int exitX, exitY;
	int exitButtonX;

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );

	guess_m_pImage = vgui_LoadTGA( "gfx/vgui/game_over.tga" );
	if ( guess_m_pImage )
		guess_m_pImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/game_over.tga!\n" );

	guess_m_pLoadImage = vgui_LoadTGA( "gfx/vgui/game_over_load.tga" );
	if ( guess_m_pLoadImage )
		guess_m_pLoadImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/game_over_load.tga!\n" );

	guess_m_pExitImage = vgui_LoadTGA( "gfx/vgui/game_over_exit.tga" );
	if ( guess_m_pExitImage )
		guess_m_pExitImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/game_over_exit.tga!\n" );

	guess_m_pImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	guess_m_pLoadImage->getSize( loadWide, loadTall );
	guess_m_pExitImage->getSize( exitWide, exitTall );

	loadX = wide / 2 - loadWide / 2;
	loadY = tall / 2 + loadTall;
	exitX = wide / 2 - exitWide / 2;
	exitY = loadY + exitTall;
	exitButtonX = wide / 2 - 51;

	guess_m_pPanel = new ImagePanel( guess_m_pImage );
	guess_m_pPanel->setParent( this );
	guess_m_pPanel->setPos( x, y );
	guess_m_pPanel->setVisible( true );

	guess_m_pLoad = new ImagePanel( guess_m_pLoadImage );
	guess_m_pLoad->setParent( guess_m_pPanel );
	guess_m_pLoad->setPos( loadX, loadY );
	guess_m_pLoad->setVisible( true );

	guess_m_pExit = new ImagePanel( guess_m_pExitImage );
	guess_m_pExit->setParent( guess_m_pPanel );
	guess_m_pExit->setPos( exitX, exitY );
	guess_m_pExit->setVisible( true );

	guess_m_pLoadButton = new ColorButton( "", loadX, loadY, 288, 36, false, true );
	guess_m_pLoadButton->setParent( guess_m_pPanel );
	guess_m_pLoadButton->addActionSignal( new CGameOverHandler_Command( this, 1 ) );
	guess_m_pLoadButton->setPos( loadX, loadY );

	guess_m_pExitButton = new ColorButton( "", exitButtonX, exitY, 105, 37, false, true );
	guess_m_pExitButton->setParent( guess_m_pPanel );
	guess_m_pExitButton->addActionSignal( new CGameOverHandler_Command( this, 2 ) );
	guess_m_pExitButton->setPos( exitButtonX, exitY );

	guess_m_flAlpha = 255;

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

CGameOver::~CGameOver()
{
	delete guess_m_pImage;
	delete guess_m_pLoadImage;
	delete guess_m_pExitImage;
}

void CGameOver::ActionSignal( int cmd )
{
	char szCommand[256];

	switch ( cmd )
	{
	case 1:
		if ( guess_iDoctorMode )
		{
			if ( strlen( guess_m_szCustomDoc ) > 2 )
			{
				strcpy( szCommand, "map " );
				strcat( szCommand, guess_m_szCustomDoc );
				gEngfuncs.pfnClientCmd( szCommand );
			}
			else
			{
				gEngfuncs.pfnClientCmd( "map c_doc_city" );
			}
		}
		else if ( gViewPort->guess_m_pSaveLoad )
		{
			setVisible( false );

			gViewPort->guess_m_pSaveLoad->guess_m_pSlot1Button->cursorExited();
			gViewPort->guess_m_pSaveLoad->guess_m_pSlot2Button->cursorExited();
			gViewPort->guess_m_pSaveLoad->guess_m_pSlot3Button->cursorExited();
			gViewPort->guess_m_pSaveLoad->guess_m_pSlot4Button->cursorExited();
			gViewPort->guess_m_pSaveLoad->guess_m_pSlot5Button->cursorExited();

			gViewPort->guess_m_pSaveLoad->guess_m_flSlot1ClickTime = 0;
			gViewPort->guess_m_pSaveLoad->guess_m_flSlot2ClickTime = 0;
			gViewPort->guess_m_pSaveLoad->guess_m_flSlot3ClickTime = 0;
			gViewPort->guess_m_pSaveLoad->guess_m_flSlot4ClickTime = 0;
			gViewPort->guess_m_pSaveLoad->guess_m_flSlot5ClickTime = 0;
			gViewPort->guess_m_pSaveLoad->guess_m_iMode = 1;
			gViewPort->guess_m_pSaveLoad->guess_UpdateSlots();
			gViewPort->guess_m_pSaveLoad->setVisible( true );
			gViewPort->UpdateCursorState();

			gEngfuncs.pfnServerCmd( "freeze 1024" );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		}
		break;
	case 2:
		setVisible( false );

		if ( guess_iMenuMode )
		{
			gEngfuncs.pfnClientCmd( "map c_game_menu1" );
			gViewPort->UpdateCursorState();
		}
		else
		{
			gEngfuncs.pfnServerCmd( "closegame" );
			gViewPort->UpdateCursorState();
		}
		break;
	}
}

void CGameOver::Initialize()
{
	guess_m_szCustomDoc[0] = 0;
	setVisible( false );
}

int guess_MsgCustomDoc( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pGameOver )
		strcpy( gViewPort->guess_m_pGameOver->guess_m_szCustomDoc, READ_STRING() );

	return 1;
}

void guess_CustomDocInit()
{
	gEngfuncs.pfnHookUserMsg( "CustomDoc", guess_MsgCustomDoc );
}

void CGameOver::setVisible( bool state )
{
	if ( state )
	{
		if ( !guess_iVGUIMenuNoMusic )
			guess_IrrKlangPlay( "game_over.mp3" );

		guess_m_bLoadArmed = false;
		guess_m_bExitArmed = false;
		guess_m_flAlpha = 255;

		if ( guess_iDoctorMode )
			guess_m_pLoad->setImage( vgui_LoadTGA( "gfx/vgui/game_over_restart.tga" ) );
		else
			guess_m_pLoad->setImage( vgui_LoadTGA( "gfx/vgui/game_over_load.tga" ) );

		guess_m_pExit->setImage( vgui_LoadTGA( "gfx/vgui/game_over_exit.tga" ) );
	}

	Panel::setVisible( state );
	gViewPort->UpdateCursorState();
}

void CGameOver::solve()
{
	if ( guess_m_flAlpha > 0 )
		guess_m_flAlpha -= gHUD.m_flTimeDelta * 500;

	if ( guess_m_flAlpha < 0 )
		guess_m_flAlpha = 0;

	guess_m_pImage->setColor( Color( 255, 255, 255, guess_m_flAlpha ) );
	guess_m_pLoadImage->setColor( Color( 255, 255, 255, guess_m_flAlpha ) );
	guess_m_pExitImage->setColor( Color( 255, 255, 255, guess_m_flAlpha ) );

	if ( !gEngfuncs.GetLocalPlayer() )
		return;

	if ( guess_m_pLoadButton->isArmed() )
	{
		if ( !guess_m_bLoadArmed )
		{
			if ( guess_iDoctorMode )
				guess_m_pLoad->setImage( vgui_LoadTGA( "gfx/vgui/game_over_restart2.tga" ) );
			else
				guess_m_pLoad->setImage( vgui_LoadTGA( "gfx/vgui/game_over_load2.tga" ) );

			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bLoadArmed = true;
		}
	}
	else if ( guess_m_bLoadArmed )
	{
		if ( guess_iDoctorMode )
			guess_m_pLoad->setImage( vgui_LoadTGA( "gfx/vgui/game_over_restart.tga" ) );
		else
			guess_m_pLoad->setImage( vgui_LoadTGA( "gfx/vgui/game_over_load.tga" ) );

		guess_m_bLoadArmed = false;
	}

	if ( guess_m_pExitButton->isArmed() )
	{
		if ( !guess_m_bExitArmed )
		{
			guess_m_pExit->setImage( vgui_LoadTGA( "gfx/vgui/game_over_exit2.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bExitArmed = true;
		}
	}
	else if ( guess_m_bExitArmed )
	{
		guess_m_pExit->setImage( vgui_LoadTGA( "gfx/vgui/game_over_exit.tga" ) );
		guess_m_bExitArmed = false;
	}

	Panel::solve();
}
