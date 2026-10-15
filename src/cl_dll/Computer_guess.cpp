// ======================================
// Cry of Fear computer puzzle panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include <VGUI_TextEntry.h>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "event_api.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Computer.h"
#include "..\game_shared\vgui_loadtga.h"

CComputer::CComputer() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	int r, g, b, a;
	Panel *pPanel;
	CommandButton *pButton;

	pPanel = new Panel( 0, 0, ScreenWidth, ScreenHeight );
	pPanel->setParent( this );
	pPanel->setSize( ScreenWidth, ScreenHeight );
	pPanel->setPos( 0, 0 );
	pPanel->setBgColor( 0, 0, 0, 0 );
	pPanel->setVisible( true );

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

	guess_m_pScreenImage = vgui_LoadTGA( "gfx/vgui/puzzle/computer/computer_logon.tga" );
	if ( guess_m_pScreenImage )
		guess_m_pScreenImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/puzzle/computer/computer_logon.tga!\n" );

	x = ScreenWidth / 2 - 277;
	y = ScreenHeight / 2 - 240;

	guess_m_pScreen = new ImagePanel( guess_m_pScreenImage );
	guess_m_pScreen->setParent( this );
	guess_m_pScreen->setPos( x, y );
	guess_m_pScreen->setVisible( true );

	pButton = new CommandButton( "", 157, 320, 75, 23, true, true );
	pButton->setParent( guess_m_pScreen );
	pButton->addActionSignal( new CComputerHandler_Command( this, 1 ) );

	pButton = new CommandButton( "", 238, 320, 75, 23, true, true );
	pButton->setParent( guess_m_pScreen );
	pButton->addActionSignal( new CComputerHandler_Command( this, 2 ) );

	pButton = new CommandButton( "", 478, 447, 29, 29, true, true );
	pButton->setParent( guess_m_pScreen );
	pButton->addActionSignal( new CComputerHandler_Command( this, 2 ) );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );

	guess_m_pUser = new TextEntry( "", 160, 204, 239, 16 );
	guess_m_pUser->setVisible( true );
	guess_m_pUser->setParent( guess_m_pScreen );
	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
	guess_m_pUser->setFgColor( r, g, b, a );
	guess_m_pUser->setFgColor( Scheme::sc_primary1 );
	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	guess_m_pUser->setBgColor( r, g, b, a );

	guess_m_pPassword = new TextEntry( "", 160, 231, 239, 16 );
	guess_m_pPassword->setVisible( true );
	guess_m_pPassword->setParent( guess_m_pScreen );
	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
	guess_m_pPassword->setFgColor( r, g, b, a );
	guess_m_pPassword->setFgColor( Scheme::sc_primary1 );
	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	guess_m_pPassword->setBgColor( r, g, b, a );

	guess_m_pText = new TextPanel( "", 194, 162, 211, 152 );
	guess_m_pText->setParent( guess_m_pScreen );
	guess_m_pText->setVisible( false );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CComputer::ActionSignal( int cmd )
{
	char szUser[256];
	char szPassword[256];
	char szCommand[128];

	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		if ( guess_m_iLoggedOn )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			return;
		}
		guess_m_pUser->getText( 0, szUser, 256 );
		guess_m_pPassword->getText( 0, szPassword, 256 );
		sprintf( szCommand, "computercheck %s %s", szUser, szPassword );
		gEngfuncs.pfnServerCmd( szCommand );
		break;
	case 2:
		setVisible( false );
		gViewPort->UpdateCursorState();
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown Computer ActionSignal %i.\n", cmd );
		break;
	}
}

void CComputer::Initialize()
{
	setVisible( false );
	guess_m_iLoggedOn = 0;
	guess_m_flNextKeyTime = gEngfuncs.GetClientTime();
}

int CComputer::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && !strcmp( pszCurrentBinding, "toggleconsole" ) )
		return 1;

	return 0;
}

int guess_MsgComputer( const char *pszName, int iSize, void *pbuf )
{
	char szText[512];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort && gViewPort->guess_m_pComputer )
	{
		if ( !gViewPort->guess_m_pComputer->isVisible() )
		{
			gViewPort->guess_m_pComputer->setVisible( true );
			gViewPort->UpdateCursorState();
		}

		gViewPort->UpdateCursorState();

		gViewPort->guess_m_pComputer->guess_m_iLoggedOn = READ_BYTE();
		int iWrongUser = READ_BYTE();
		int iWrongPassword = READ_BYTE();
		int iSilent = READ_BYTE();

		sprintf( szText, "Jill - %s! - David\n", READ_STRING() );
		gViewPort->guess_m_pComputer->guess_m_pText->setText( szText );
		gViewPort->guess_m_pComputer->guess_m_pText->setFgColor( 0, 0, 0, 0 );

		if ( gViewPort->guess_m_pComputer->guess_m_iLoggedOn )
		{
			gViewPort->guess_m_pComputer->guess_m_pScreen->setImage( vgui_LoadTGA( "gfx/vgui/puzzle/computer/computer_loggedon.tga" ) );
			gViewPort->guess_m_pComputer->guess_m_pUser->setVisible( false );
			gViewPort->guess_m_pComputer->guess_m_pPassword->setVisible( false );

			cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();
			gViewPort->guess_m_pComputer->guess_m_pText->setVisible( true );

			if ( pPlayer && !iSilent )
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_AUTO, "buttons/computer_logon.wav", 0.7, 0.8, 0, PITCH_NORM );
		}
		else
		{
			gViewPort->guess_m_pComputer->guess_m_pScreen->setImage( vgui_LoadTGA( "gfx/vgui/puzzle/computer/computer_logon.tga" ) );
			gViewPort->guess_m_pComputer->guess_m_pUser->setVisible( true );
			gViewPort->guess_m_pComputer->guess_m_pPassword->setVisible( true );

			cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();
			if ( pPlayer && ( iWrongUser || iWrongPassword ) )
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_AUTO, "buttons/computer_deny.wav", 0.7, 0.8, 0, PITCH_NORM );

			gViewPort->guess_m_pComputer->guess_m_pText->setVisible( false );

			if ( iWrongUser || iWrongPassword )
			{
				gViewPort->guess_m_pComputer->guess_m_pUser->setText( "Invalid username or password!", 29 );
				gViewPort->guess_m_pComputer->guess_m_pPassword->setText( "Invalid password or password!", 29 );
			}
		}
	}

	return 1;
}

void guess_ComputerInit()
{
	gEngfuncs.pfnHookUserMsg( "Computer", guess_MsgComputer );
}

void CComputer::setVisible( bool state )
{
	if ( state )
	{
		guess_m_flNextKeyTime = gEngfuncs.GetClientTime();
		guess_m_pUser->setText( "Enter a username...", 19 );
		guess_m_pPassword->setText( "Enter a password...", 19 );

		if ( guess_m_iLoggedOn )
		{
			guess_m_pUser->setVisible( false );
			guess_m_pPassword->setVisible( false );
		}
		else
		{
			guess_m_pUser->setVisible( true );
			guess_m_pPassword->setVisible( true );
		}
	}
	else
	{
		guess_m_pUser->setText( "Enter a username...", 19 );
		guess_m_pPassword->setText( "Enter a password...", 19 );

		if ( guess_m_iLoggedOn )
		{
			guess_m_pUser->setVisible( false );
			guess_m_pPassword->setVisible( false );
		}
		else
		{
			guess_m_pUser->setVisible( true );
			guess_m_pPassword->setVisible( true );
		}

		gEngfuncs.pfnServerCmd( "stopcomputer" );
	}

	Panel::setVisible( state );
}

void CComputer::solve()
{
	char szUser[256];
	char szPassword[256];

	if ( isVisible() && guess_m_pUser->isVisible() && guess_m_pPassword->isVisible() && !guess_m_iLoggedOn )
	{
		guess_m_pUser->getText( 0, szUser, 256 );
		guess_m_pPassword->getText( 0, szPassword, 256 );

		if ( ( GetAsyncKeyState( VK_RETURN ) & 0x8000 ) && gEngfuncs.GetClientTime() > guess_m_flNextKeyTime )
		{
			char szUser2[256];
			char szPassword2[256];
			char szCommand[128];

			guess_m_flNextKeyTime = gEngfuncs.GetClientTime() + 1;
			guess_m_pUser->getText( 0, szUser2, 256 );
			guess_m_pPassword->getText( 0, szPassword2, 256 );
			sprintf( szCommand, "computercheck %s %s", szUser2, szPassword2 );
			gEngfuncs.pfnServerCmd( szCommand );
		}

		if ( guess_m_pUser->hasFocus() )
		{
			if ( !strcmp( szUser, "Enter a username..." ) )
				guess_m_pUser->setText( "", 0 );

			if ( ( GetAsyncKeyState( VK_TAB ) & 0x8000 ) && gEngfuncs.GetClientTime() > guess_m_flNextKeyTime )
			{
				guess_m_flNextKeyTime = gEngfuncs.GetClientTime() + 1;
				guess_m_pPassword->requestFocus();
			}
		}

		if ( guess_m_pPassword->hasFocus() )
		{
			if ( !strcmp( szPassword, "Enter a password..." ) )
				guess_m_pPassword->setText( "", 0 );

			if ( ( GetAsyncKeyState( VK_TAB ) & 0x8000 ) && gEngfuncs.GetClientTime() > guess_m_flNextKeyTime )
			{
				guess_m_flNextKeyTime = gEngfuncs.GetClientTime() + 1;
				guess_m_pUser->requestFocus();
			}
		}

		cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();
		int iPitch = PITCH_NORM - gEngfuncs.pfnRandomLong( -2, 2 );

		if ( guess_m_iUserLength != strlen( szUser ) )
		{
			if ( pPlayer )
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_BODY, "buttons/keyboard_type.wav", 0.7, 0.8, 0, iPitch );

			guess_m_iUserLength = strlen( szUser );
		}

		if ( guess_m_iPasswordLength != strlen( szPassword ) )
		{
			if ( pPlayer )
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_BODY, "buttons/keyboard_type.wav", 0.7, 0.8, 0, iPitch );

			guess_m_iPasswordLength = strlen( szPassword );
		}
	}

	Panel::solve();
}
