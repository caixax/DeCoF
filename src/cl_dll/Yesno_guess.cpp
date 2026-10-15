// ======================================
// Cry of Fear yes/no question panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Yesno.h"
#include "..\game_shared\vgui_loadtga.h"

CYesno::CYesno() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int x, y;

	guess_m_pImage = vgui_LoadTGA( "gfx/vgui/640_savegame.tga" );
	if ( guess_m_pImage )
		guess_m_pImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_savegame.tga!\n" );

	x = ScreenWidth / 2 - 160;
	y = ScreenHeight / 1.25 - 100;

	guess_m_pImagePanel = new ImagePanel( guess_m_pImage );
	guess_m_pImagePanel->setParent( this );
	guess_m_pImagePanel->setPos( x, y );
	guess_m_pImagePanel->setVisible( true );

	guess_m_pYesButton = new CommandButton( "", 36, 149, 79, 42, true, true );
	guess_m_pYesButton->setParent( guess_m_pImagePanel );
	guess_m_pYesButton->addActionSignal( new CYesnoHandler_Command( this, 2 ) );

	guess_m_pNoButton = new CommandButton( "", 204, 149, 79, 42, true, true );
	guess_m_pNoButton->setParent( guess_m_pImagePanel );
	guess_m_pNoButton->addActionSignal( new CYesnoHandler_Command( this, 3 ) );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CYesno::ActionSignal( int cmd )
{
	char szCommand[128];

	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		setVisible( false );
		gViewPort->UpdateCursorState();
		break;
	case 2:
		if ( guess_m_iActivate )
		{
			sprintf( szCommand, "yesnoactivate %i", guess_m_iActivate );
			gEngfuncs.pfnServerCmd( szCommand );
			gEngfuncs.pfnPlaySoundByName( "save/save_yes.wav", 1 );
			setVisible( false );
			gViewPort->UpdateCursorState();
			if ( !strcmp( gEngfuncs.pfnGetLevelName(), "maps/video_stress_test.bsp" ) )
				gEngfuncs.pfnServerCmd( "stresscontinue" );
			break;
		}

		gEngfuncs.pfnServerCmd( "unfreeze 1024" );
		gEngfuncs.pfnPlaySoundByName( "save/save_no.wav", 1 );
		setVisible( false );
		gViewPort->UpdateCursorState();
		if ( !strcmp( gEngfuncs.pfnGetLevelName(), "maps/video_stress_test.bsp" ) )
			gEngfuncs.pfnServerCmd( "stresscontinue" );
		break;
	case 3:
		sprintf( szCommand, "yesnono %i", guess_m_iActivate );
		gEngfuncs.pfnServerCmd( szCommand );
		gEngfuncs.pfnPlaySoundByName( "save/save_no.wav", 1 );
		setVisible( false );
		gViewPort->UpdateCursorState();

		if ( !strcmp( gEngfuncs.pfnGetLevelName(), "maps/video_stress_test.bsp" ) )
			gEngfuncs.pfnServerCmd( "stresscontinue" );
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown Yesno ActionSignal %i.\n", cmd );
		break;
	}
}

void CYesno::Initialize()
{
	setVisible( false );
	guess_m_iActivate = 0;
}

int guess_MsgYesNo( const char *pszName, int iSize, void *pbuf )
{
	char szImage[128];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pYesno )
	{
		sprintf( szImage, "%s", READ_STRING() );
		gViewPort->guess_m_pYesno->guess_m_iActivate = READ_SHORT();

		if ( strlen( szImage ) > 1 )
		{
			if ( vgui_LoadTGA( szImage ) )
			{
				gViewPort->guess_m_pYesno->guess_m_pImagePanel->setImage( vgui_LoadTGA( szImage ) );
				gViewPort->guess_m_pYesno->setVisible( true );
				gViewPort->UpdateCursorState();
			}
			else
				gEngfuncs.Con_Printf( "Cannot load %s!\n", szImage );
		}
		else
			gEngfuncs.Con_Printf( "No message sent\n" );
	}

	return 1;
}

void guess_YesNoInit()
{
	gEngfuncs.pfnHookUserMsg( "YesNo", guess_MsgYesNo );
}
