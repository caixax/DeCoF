// ======================================
// Cry of Fear save and load slots panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include <fstream>
#include <string>
#include <vector>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "com_weapons.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_SaveLoad.h"
#include "guess_GameMenu.h"
#include "guess_GameOver.h"
#include "..\game_shared\vgui_loadtga.h"
#include "event_api.h"
#include <windows.h>
#include <time.h>

extern int guess_iMenuMode;

void guess_IrrKlangStop( float flFadeTime );
void GetCurrentPath( char *path );

CSaveLoad::CSaveLoad() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	Panel *pPanel;
	CommandButton *pButton;

	guess_m_pBackgroundImage = vgui_LoadTGA( "gfx/vgui/save_load.tga" );
	if ( guess_m_pBackgroundImage )
		guess_m_pBackgroundImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/save_load.tga!\n" );

	guess_m_pBackgroundImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	guess_m_pBackground = new ImagePanel( guess_m_pBackgroundImage );
	guess_m_pBackground->setParent( this );
	guess_m_pBackground->setPos( x, y );
	guess_m_pBackground->setVisible( true );

	pButton = new CommandButton( "", 260, 398, 119, 43, true, true );
	pButton->setParent( guess_m_pBackground );
	pButton->addActionSignal( new CSaveLoadHandler_Command( this, 1 ) );

	guess_m_pSlot1Label = new Label( "", 93, 154, 455, 29 );
	guess_m_pSlot1Label->setParent( guess_m_pBackground );
	guess_m_pSlot1Label->setBgColor( 0, 0, 0, 1 );
	guess_m_pSlot1Label->setFgColor( 255, 255, 255, 1 );

	guess_m_pSlot2Label = new Label( "", 93, 190, 455, 29 );
	guess_m_pSlot2Label->setParent( guess_m_pBackground );
	guess_m_pSlot2Label->setBgColor( 0, 0, 0, 1 );
	guess_m_pSlot2Label->setFgColor( 255, 255, 255, 1 );

	guess_m_pSlot3Label = new Label( "", 93, 226, 455, 29 );
	guess_m_pSlot3Label->setParent( guess_m_pBackground );
	guess_m_pSlot3Label->setBgColor( 0, 0, 0, 1 );
	guess_m_pSlot3Label->setFgColor( 255, 255, 255, 1 );

	guess_m_pSlot4Label = new Label( "", 93, 262, 455, 29 );
	guess_m_pSlot4Label->setParent( guess_m_pBackground );
	guess_m_pSlot4Label->setBgColor( 0, 0, 0, 1 );
	guess_m_pSlot4Label->setFgColor( 255, 255, 255, 1 );

	guess_m_pSlot5Label = new Label( "", 93, 296, 455, 29 );
	guess_m_pSlot5Label->setParent( guess_m_pBackground );
	guess_m_pSlot5Label->setBgColor( 0, 0, 0, 1 );
	guess_m_pSlot5Label->setFgColor( 255, 255, 255, 1 );

	guess_m_pSlot1Button = new ColorButton( "", 93, 154, 455, 29, false, false );
	guess_m_pSlot1Button->setParent( guess_m_pBackground );
	guess_m_pSlot1Button->addActionSignal( new CSaveLoadHandler_Command( this, 2 ) );

	guess_m_pSlot2Button = new ColorButton( "", 93, 190, 455, 29, true, true );
	guess_m_pSlot2Button->setParent( guess_m_pBackground );
	guess_m_pSlot2Button->addActionSignal( new CSaveLoadHandler_Command( this, 3 ) );

	guess_m_pSlot3Button = new ColorButton( "", 93, 226, 455, 29, true, true );
	guess_m_pSlot3Button->setParent( guess_m_pBackground );
	guess_m_pSlot3Button->addActionSignal( new CSaveLoadHandler_Command( this, 4 ) );

	guess_m_pSlot4Button = new ColorButton( "", 93, 262, 455, 29, true, true );
	guess_m_pSlot4Button->setParent( guess_m_pBackground );
	guess_m_pSlot4Button->addActionSignal( new CSaveLoadHandler_Command( this, 5 ) );

	guess_m_pSlot5Button = new ColorButton( "", 93, 296, 455, 29, true, true );
	guess_m_pSlot5Button->setParent( guess_m_pBackground );
	guess_m_pSlot5Button->addActionSignal( new CSaveLoadHandler_Command( this, 6 ) );

	// black out the screen around the picture
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

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CSaveLoad::ActionSignal( int cmd )
{
	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

	switch ( cmd )
	{
	case 1:
		if ( !strcmp( gEngfuncs.pfnGetLevelName(), "maps/c_loadgame.bsp" ) )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnServerCmd( "closegame" );

			if ( pPlayer )
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_AUTO, "save/save_no.wav", 1.0, 0.8, 0, PITCH_NORM );
		}
		else if ( strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();

			if ( gViewPort && gViewPort->guess_m_pGameMenu )
			{
				gViewPort->guess_m_pGameMenu->setVisible( true );
				gViewPort->UpdateCursorState();
			}
		}
		else if ( strstr( gEngfuncs.pfnGetLevelName(), "difficulty_" ) || strstr( gEngfuncs.pfnGetLevelName(), "server_" ) || strstr( gEngfuncs.pfnGetLevelName(), "unlockables" ) )
		{
			if ( guess_iMenuMode )
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnClientCmd( "map c_game_menu1" );
			}
			else
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnClientCmd( "disconnect" );
			}
		}
		else if ( CL_IsDead() )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();

			if ( gViewPort && gViewPort->guess_m_pGameOver )
			{
				gViewPort->guess_m_pGameOver->setVisible( true );
				gViewPort->UpdateCursorState();
			}
		}
		else
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnServerCmd( "unfreeze 666" );

			if ( pPlayer )
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_AUTO, "save/save_no.wav", 1.0, 0.8, 0, PITCH_NORM );
		}
		break;
	case 2:
		if ( gEngfuncs.GetClientTime() < guess_m_flSlot1ClickTime )
		{
			if ( guess_m_iMode )
			{
				byte *pFile = gEngfuncs.COM_LoadFile( "SAVE/saveinfo1.cof", 5, NULL );

				if ( pFile )
				{
					setVisible( false );
					gViewPort->UpdateCursorState();
					gEngfuncs.pfnServerCmd( "unfreeze 1024" );

					if ( strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
						guess_IrrKlangStop( 0 );

					gEngfuncs.COM_FreeFile( pFile );
					gEngfuncs.pfnServerCmd( "cofload 1" );
				}
			}
			else
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnServerCmd( "unfreezesave 1024 1" );

				gHUD.guess_m_flSaveHackTime = gEngfuncs.GetClientTime() + 3.25;
				gHUD.guess_m_iSaveHackSlot = 1;
				gHUD.guess_m_bSaveHack = true;

				guess_WriteSaveInfo( "saveinfo1.cof" );
			}
		}

		guess_m_flSlot1ClickTime = gEngfuncs.GetClientTime() + 0.5;
		break;
	case 3:
		if ( gEngfuncs.GetClientTime() < guess_m_flSlot2ClickTime )
		{
			if ( guess_m_iMode )
			{
				byte *pFile = gEngfuncs.COM_LoadFile( "SAVE/saveinfo2.cof", 5, NULL );

				if ( pFile )
				{
					setVisible( false );
					gViewPort->UpdateCursorState();
					gEngfuncs.pfnServerCmd( "unfreeze 1024" );

					if ( strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
						guess_IrrKlangStop( 0 );

					gEngfuncs.COM_FreeFile( pFile );
					gEngfuncs.pfnServerCmd( "cofload 2" );
				}
			}
			else
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnServerCmd( "unfreezesave 1024 2" );

				gHUD.guess_m_flSaveHackTime = gEngfuncs.GetClientTime() + 3.25;
				gHUD.guess_m_iSaveHackSlot = 2;
				gHUD.guess_m_bSaveHack = true;

				guess_WriteSaveInfo( "saveinfo2.cof" );
			}
		}

		guess_m_flSlot2ClickTime = gEngfuncs.GetClientTime() + 0.5;
		break;
	case 4:
		if ( gEngfuncs.GetClientTime() < guess_m_flSlot3ClickTime )
		{
			if ( guess_m_iMode )
			{
				byte *pFile = gEngfuncs.COM_LoadFile( "SAVE/saveinfo3.cof", 5, NULL );

				if ( pFile )
				{
					setVisible( false );
					gViewPort->UpdateCursorState();
					gEngfuncs.pfnServerCmd( "unfreeze 1024" );

					if ( strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
						guess_IrrKlangStop( 0 );

					gEngfuncs.COM_FreeFile( pFile );
					gEngfuncs.pfnServerCmd( "cofload 3" );
				}
			}
			else
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnServerCmd( "unfreezesave 1024 3" );

				gHUD.guess_m_flSaveHackTime = gEngfuncs.GetClientTime() + 3.25;
				gHUD.guess_m_iSaveHackSlot = 3;
				gHUD.guess_m_bSaveHack = true;

				guess_WriteSaveInfo( "saveinfo3.cof" );
			}
		}

		guess_m_flSlot3ClickTime = gEngfuncs.GetClientTime() + 0.5;
		break;
	case 5:
		if ( gEngfuncs.GetClientTime() < guess_m_flSlot4ClickTime )
		{
			if ( guess_m_iMode )
			{
				byte *pFile = gEngfuncs.COM_LoadFile( "SAVE/saveinfo4.cof", 5, NULL );

				if ( pFile )
				{
					setVisible( false );
					gViewPort->UpdateCursorState();
					gEngfuncs.pfnServerCmd( "unfreeze 1024" );

					if ( strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
						guess_IrrKlangStop( 0 );

					gEngfuncs.COM_FreeFile( pFile );
					gEngfuncs.pfnServerCmd( "cofload 4" );
				}
			}
			else
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnServerCmd( "unfreezesave 1024 4" );

				gHUD.guess_m_flSaveHackTime = gEngfuncs.GetClientTime() + 3.25;
				gHUD.guess_m_iSaveHackSlot = 4;
				gHUD.guess_m_bSaveHack = true;

				guess_WriteSaveInfo( "saveinfo4.cof" );
			}
		}

		guess_m_flSlot4ClickTime = gEngfuncs.GetClientTime() + 0.5;
		break;
	case 6:
		if ( gEngfuncs.GetClientTime() < guess_m_flSlot5ClickTime )
		{
			if ( guess_m_iMode )
			{
				byte *pFile = gEngfuncs.COM_LoadFile( "SAVE/saveinfo5.cof", 5, NULL );

				if ( pFile )
				{
					setVisible( false );
					gViewPort->UpdateCursorState();
					gEngfuncs.pfnServerCmd( "unfreeze 1024" );

					if ( strstr( gEngfuncs.pfnGetLevelName(), "game_menu" ) )
						guess_IrrKlangStop( 0 );

					gEngfuncs.COM_FreeFile( pFile );
					gEngfuncs.pfnServerCmd( "cofload 5" );
				}
			}
			else
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnServerCmd( "unfreezesave 1024 5" );

				gHUD.guess_m_flSaveHackTime = gEngfuncs.GetClientTime() + 3.25;
				gHUD.guess_m_iSaveHackSlot = 5;
				gHUD.guess_m_bSaveHack = true;

				guess_WriteSaveInfo( "saveinfo5.cof" );
			}
		}

		guess_m_flSlot5ClickTime = gEngfuncs.GetClientTime() + 0.5;
		break;
	}
}

void CSaveLoad::Initialize()
{
	setVisible( false );
}

int guess_MsgSaveLoad( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pSaveLoad )
	{
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

		gViewPort->guess_m_pSaveLoad->guess_m_iMode = READ_BYTE();

		if ( !gViewPort->guess_m_pSaveLoad->guess_m_iMode )
			strcpy( gViewPort->guess_m_pSaveLoad->guess_m_szSaveName, READ_STRING() );

		gViewPort->guess_m_pSaveLoad->guess_UpdateSlots();
		gViewPort->guess_m_pSaveLoad->setVisible( true );
		gViewPort->UpdateCursorState();
		gEngfuncs.pfnServerCmd( "freeze 1024" );

		cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

		if ( pPlayer )
			gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_AUTO, "save/save_yes.wav", 1.0, 0.8, 0, PITCH_NORM );
	}

	return 1;
}

bool guess_DirectoryExists( const char *pszPath )
{
	return GetFileAttributes( pszPath ) != INVALID_FILE_ATTRIBUTES;
}

void CSaveLoad::guess_UpdateSlots()
{
	guess_m_pSlot1Label->setText( "" );
	guess_m_pSlot2Label->setText( "" );
	guess_m_pSlot3Label->setText( "" );
	guess_m_pSlot4Label->setText( "" );
	guess_m_pSlot5Label->setText( "" );

	guess_ReadSlot( 1 );
	guess_ReadSlot( 2 );
	guess_ReadSlot( 3 );
	guess_ReadSlot( 4 );
	guess_ReadSlot( 5 );
}

void CSaveLoad::guess_ReadSlot( int slot )
{
	char szPath[256];

	strcpy( szPath, gEngfuncs.pfnGetGameDirectory() );
	strcat( szPath, "/SAVE/" );
	sprintf( szPath, "%ssaveinfo%i.cof", szPath, slot );

	std::ifstream file( szPath );
	std::string line;
	std::vector<std::string> lines;

	if ( file )
	{
		while ( std::getline( file, line ) )
			lines.push_back( line );

		switch ( slot )
		{
		case 1:
			guess_m_pSlot1Label->setText( line.c_str() );
			guess_m_pSlot1Label->setContentAlignment( Label::a_center );
			break;
		case 2:
			guess_m_pSlot2Label->setText( line.c_str() );
			guess_m_pSlot2Label->setContentAlignment( Label::a_center );
			break;
		case 3:
			guess_m_pSlot3Label->setText( line.c_str() );
			guess_m_pSlot3Label->setContentAlignment( Label::a_center );
			break;
		case 4:
			guess_m_pSlot4Label->setText( line.c_str() );
			guess_m_pSlot4Label->setContentAlignment( Label::a_center );
			break;
		case 5:
			guess_m_pSlot5Label->setText( line.c_str() );
			guess_m_pSlot5Label->setContentAlignment( Label::a_center );
			break;
		}
	}
}

void guess_SaveLoadInit()
{
	gEngfuncs.pfnHookUserMsg( "SaveLoad", guess_MsgSaveLoad );
}

void CSaveLoad::guess_WriteSaveInfo( const char *pszFile )
{
	char szDir[MAX_PATH];
	char szPath[256];
	char szTime[256];

	GetCurrentPath( szDir );
	strcat( szDir, "/" );
	strcat( szDir, gEngfuncs.pfnGetGameDirectory() );
	strcat( szDir, "/SAVE/" );

	for ( int i = 0; i < (int)strlen( szDir ); i++ )
	{
		if ( szDir[i] == '/' )
			szDir[i] = '\\';
	}

	if ( !guess_DirectoryExists( szDir ) )
	{
		gEngfuncs.Con_Printf( "Creating save directory %s\n", szDir );

		if ( CreateDirectory( szDir, NULL ) )
			gEngfuncs.Con_Printf( "Made save directory\n" );
	}

	strcpy( szPath, gEngfuncs.pfnGetGameDirectory() );
	strcat( szPath, "/SAVE/" );
	strcat( szPath, pszFile );

	std::ofstream file;
	file.open( szPath );

	if ( file )
	{
		time_t t;
		time( &t );
		strcpy( szTime, ctime( &t ) );

		if ( szTime[strlen( szTime ) - 1] == '\n' )
			szTime[strlen( szTime ) - 1] = 0;

		file << guess_m_szSaveName << " - " << szTime;
		file.close();
	}
}
