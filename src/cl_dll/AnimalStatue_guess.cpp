// ======================================
// Cry of Fear animal statue puzzle panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_AnimalStatue.h"
#include "..\game_shared\vgui_loadtga.h"

CAnimalStatue::CAnimalStatue() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	CommandButton *pButton;

	guess_m_pBlackPanel = new Panel( 0, 0, ScreenWidth, ScreenHeight );
	guess_m_pBlackPanel->setParent( this );
	guess_m_pBlackPanel->setBgColor( 0, 0, 0, 0 );

	guess_m_pBackgroundImage = vgui_LoadTGA( "gfx/vgui/backgroundtoall.tga" );
	if ( guess_m_pBackgroundImage )
		guess_m_pBackgroundImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/backgroundtoall.tga!\n" );

	guess_m_pBackgroundImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	guess_m_pBackground = new ImagePanel( guess_m_pBackgroundImage );
	guess_m_pBackground->setParent( this );
	guess_m_pBackground->setPos( x, y );
	guess_m_pBackground->setVisible( true );

	guess_m_pButtonsImage = vgui_LoadTGA( "gfx/vgui/640_eagle_button.tga" );
	if ( guess_m_pButtonsImage )
		guess_m_pButtonsImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_eagle_button.tga!\n" );

	x = ScreenWidth / 2 - 320;
	y = ScreenHeight / 2 - 240;

	guess_m_pButtons = new ImagePanel( guess_m_pButtonsImage );
	guess_m_pButtons->setParent( this );
	guess_m_pButtons->setPos( x, y );
	guess_m_pButtons->setVisible( true );

	pButton = new CommandButton( "", 0, 0, 640, 480, true, true );
	pButton->setParent( guess_m_pButtons );
	pButton->addActionSignal( new CAnimalStatueHandler_Command( this, 1 ) );

	pButton = new CommandButton( "", 290, 210, 60, 60, true, true );
	pButton->setParent( guess_m_pButtons );
	pButton->addActionSignal( new CAnimalStatueHandler_Command( this, 2 ) );

	pButton = new CommandButton( "", 290, 330, 60, 60, true, true );
	pButton->setParent( guess_m_pButtons );
	pButton->addActionSignal( new CAnimalStatueHandler_Command( this, 4 ) );

	pButton = new CommandButton( "", 230, 270, 60, 60, true, true );
	pButton->setParent( guess_m_pButtons );
	pButton->addActionSignal( new CAnimalStatueHandler_Command( this, 5 ) );

	pButton = new CommandButton( "", 350, 270, 60, 60, true, true );
	pButton->setParent( guess_m_pButtons );
	pButton->addActionSignal( new CAnimalStatueHandler_Command( this, 3 ) );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

CAnimalStatue::~CAnimalStatue()
{
	delete guess_m_pButtonsImage;
	delete guess_m_pBackgroundImage;
}

void CAnimalStatue::ActionSignal( int cmd )
{
	char szCommand[128];

	if ( !guess_m_iStatue )
		return;

	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		setVisible( false );
		gViewPort->UpdateCursorState();
		break;
	case 2:
		sprintf( szCommand, "movestatue 0 %i", guess_m_iStatue );
		gEngfuncs.pfnServerCmd( szCommand );
		gEngfuncs.pfnPlaySoundByName( "statue_puzzle/statue_switch.wav", 1 );
		break;
	case 3:
		sprintf( szCommand, "movestatue 1 %i", guess_m_iStatue );
		gEngfuncs.pfnServerCmd( szCommand );
		gEngfuncs.pfnPlaySoundByName( "statue_puzzle/statue_switch.wav", 1 );
		break;
	case 4:
		sprintf( szCommand, "movestatue 2 %i", guess_m_iStatue );
		gEngfuncs.pfnServerCmd( szCommand );
		gEngfuncs.pfnPlaySoundByName( "statue_puzzle/statue_switch.wav", 1 );
		break;
	case 5:
		sprintf( szCommand, "movestatue 3 %i", guess_m_iStatue );
		gEngfuncs.pfnServerCmd( szCommand );
		gEngfuncs.pfnPlaySoundByName( "statue_puzzle/statue_switch.wav", 1 );
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown AnimalStatue ActionSignal %i.\n", cmd );
		break;
	}
}

void CAnimalStatue::Initialize()
{
	setVisible( false );
	guess_m_iStatue = 0;
}

int CAnimalStatue::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && ( !strcmp( pszCurrentBinding, "toggleconsole" ) || !strcmp( pszCurrentBinding, "movestatue" ) ) )
		return 1;

	return 0;
}

int guess_MsgStatue( const char *pszName, int iSize, void *pbuf )
{
	char szImage[128];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pAnimalStatue )
	{
		gViewPort->guess_m_pAnimalStatue->guess_m_iStatue = READ_BYTE();

		if ( !gViewPort->guess_m_pAnimalStatue->guess_m_iStatue )
		{
			if ( gViewPort->guess_m_pAnimalStatue->isVisible() )
				gViewPort->guess_m_pAnimalStatue->setVisible( false );

			gViewPort->UpdateCursorState();
			return 1;
		}

		sprintf( szImage, "%s", READ_STRING() );

		if ( strlen( szImage ) > 1 )
		{
			if ( vgui_LoadTGA( szImage ) )
			{
				gViewPort->guess_m_pAnimalStatue->guess_m_pButtons->setImage( vgui_LoadTGA( szImage ) );

				if ( !gViewPort->guess_m_pAnimalStatue->isVisible() )
				{
					gViewPort->guess_m_pAnimalStatue->setVisible( true );
					gViewPort->UpdateCursorState();
				}
			}
			else
			{
				gEngfuncs.Con_Printf( "Cannot load %s!\n", szImage );
			}
		}
		else
		{
			gEngfuncs.Con_Printf( "No message sent\n" );
		}
	}

	return 1;
}

void guess_StatueInit()
{
	gEngfuncs.pfnHookUserMsg( "Statue", guess_MsgStatue );
}
