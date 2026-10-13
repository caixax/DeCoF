// ======================================
// Cry of Fear telephone puzzle panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Telephone.h"
#include "..\game_shared\vgui_loadtga.h"
#include "event_api.h"

CTelephone::CTelephone() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
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

	guess_m_pTelephoneImage = vgui_LoadTGA( "gfx/vgui/puzzle/telephone_vgui.tga" );
	if ( guess_m_pTelephoneImage )
		guess_m_pTelephoneImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/puzzle/telephone_vgui.tga!\n" );

	guess_m_pTelephoneImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	guess_m_pTelephone = new ImagePanel( guess_m_pTelephoneImage );
	guess_m_pTelephone->setParent( this );
	guess_m_pTelephone->setPos( x, y );
	guess_m_pTelephone->setVisible( true );

	pButton = new CommandButton( "", 0, 0, 20, 20, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 12 ) );

	pButton = new CommandButton( "", 140, 144, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 1 ) );

	pButton = new CommandButton( "", 175, 144, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 2 ) );

	pButton = new CommandButton( "", 211, 144, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 3 ) );

	pButton = new CommandButton( "", 139, 178, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 4 ) );

	pButton = new CommandButton( "", 174, 178, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 5 ) );

	pButton = new CommandButton( "", 210, 178, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 6 ) );

	pButton = new CommandButton( "", 137, 213, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 7 ) );

	pButton = new CommandButton( "", 173, 213, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 8 ) );

	pButton = new CommandButton( "", 210, 213, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 9 ) );

	pButton = new CommandButton( "", 173, 249, 30, 30, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 10 ) );

	pButton = new CommandButton( "", 251, 288, 57, 27, true, true );
	pButton->setParent( guess_m_pTelephone );
	pButton->addActionSignal( new CTelephoneHandler_Command( this, 11 ) );

	setBgColor( 0, 0, 0, 0 );

	Initialize();
}

void CTelephone::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 1:
		guess_PressDigit( 1 );
		break;
	case 2:
		guess_PressDigit( 2 );
		break;
	case 3:
		guess_PressDigit( 3 );
		break;
	case 4:
		guess_PressDigit( 4 );
		break;
	case 5:
		guess_PressDigit( 5 );
		break;
	case 6:
		guess_PressDigit( 6 );
		break;
	case 7:
		guess_PressDigit( 7 );
		break;
	case 8:
		guess_PressDigit( 8 );
		break;
	case 9:
		guess_PressDigit( 9 );
		break;
	case 10:
		guess_PressDigit( 0 );
		break;
	case 11:
		guess_Dial();
		break;
	case 12:
		if ( !guess_m_bLocked )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnPlaySoundByName( "phone/telephone_receiver_down.wav", 1 );
		}
		break;
	}
}

void CTelephone::guess_PressDigit( int digit )
{
	char szDigit[4];

	if ( guess_m_bLocked )
		return;

	if ( strlen( guess_m_szDialed ) < 15 )
	{
		itoa( digit, szDigit, 10 );
		strcat( guess_m_szDialed, szDigit );
		gEngfuncs.pfnPlaySoundByName( "phone/telephone_button_press.wav", 1 );
	}
}

void CTelephone::guess_Dial()
{
	cl_entity_t *pPlayer;

	if ( strlen( guess_m_szDialed ) >= 1 && !guess_m_bLocked )
	{
		pPlayer = gEngfuncs.GetLocalPlayer();

		gEngfuncs.pfnPlaySoundByName( "phone/telephone_button_press.wav", 1 );

		if ( !strcmp( guess_m_szNumber, guess_m_szDialed ) )
			gEngfuncs.pfnServerCmd( "telephoneright" );
		else
			gEngfuncs.pfnServerCmd( "telephonewrong" );

		if ( pPlayer )
			gEngfuncs.pEventAPI->EV_StopSound( -1, CHAN_ITEM, "phone/telephone_dial_tone.wav" );

		strncpy( guess_m_szDialed, "", 16 );
	}
}

void CTelephone::Initialize()
{
	strcpy( guess_m_szNumber, "19357185" );
	strncpy( guess_m_szDialed, "", 16 );
	setVisible( false );
}

int guess_MsgTelephone( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iOn = READ_BYTE();

	if ( gViewPort->guess_m_pTelephone )
	{
		gViewPort->guess_m_pTelephone->guess_m_bLocked = false;

		if ( iOn > 0 )
		{
			if ( iOn == 1 )
			{
				if ( !gViewPort->guess_m_pTelephone->isVisible() )
				{
					gViewPort->guess_m_pTelephone->setVisible( true );
					gEngfuncs.pfnPlaySoundByName( "phone/telephone_receiver_up.wav", 1 );
					gViewPort->guess_m_pTelephone->guess_m_flFade = 255;
				}

				gViewPort->UpdateCursorState();
				strcpy( gViewPort->guess_m_pTelephone->guess_m_szNumber, READ_STRING() );
				return 1;
			}
			else if ( iOn == 2 )
			{
				cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

				if ( pPlayer )
				{
					gEngfuncs.pEventAPI->EV_StopSound( -1, CHAN_ITEM, "phone/telephone_dial_tone.wav" );
					gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_ITEM, "phone/telephone_dial_tone.wav", 1.0, 0, 0, 100 );
				}
				return 1;
			}
			else if ( iOn == 3 )
			{
				gViewPort->guess_m_pTelephone->guess_m_bLocked = true;
			}
		}
		else
		{
			if ( gViewPort->guess_m_pTelephone->isVisible() == true )
			{
				gViewPort->guess_m_pTelephone->setVisible( false );
				gEngfuncs.pfnPlaySoundByName( "phone/telephone_receiver_down.wav", 1 );
			}

			gViewPort->UpdateCursorState();
			strcpy( gViewPort->guess_m_pTelephone->guess_m_szNumber, READ_STRING() );
			return 1;
		}
	}

	return 1;
}

void guess_TelephoneInit()
{
	gEngfuncs.pfnHookUserMsg( "Telephone", guess_MsgTelephone );
}

void CTelephone::paint()
{
	if ( guess_m_flFade > 0 )
		guess_m_flFade -= gHUD.m_flTimeDelta * 700;

	if ( guess_m_flFade < 0 )
		guess_m_flFade = 0;

	guess_m_pTelephoneImage->setColor( Color( 255, 255, 255, guess_m_flFade ) );

	int alpha = guess_m_flFade;
	if ( alpha < 128 )
		alpha = 128;

	setBgColor( Color( 0, 0, 0, alpha ) );
}

void CTelephone::setVisible( bool state )
{
	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

	strncpy( guess_m_szDialed, "", 16 );

	if ( state )
	{
		if ( pPlayer )
			gEngfuncs.pEventAPI->EV_PlaySound( -1, pPlayer->curstate.origin, CHAN_ITEM, "phone/telephone_dial_tone.wav", 1.0, 0, 0, 100 );
		gEngfuncs.pfnServerCmd( "freeze 1024" );
	}
	else
	{
		if ( pPlayer )
			gEngfuncs.pEventAPI->EV_StopSound( -1, CHAN_ITEM, "phone/telephone_dial_tone.wav" );
		gEngfuncs.pfnServerCmd( "unfreeze 1024" );
	}

	Panel::setVisible( state );
}
