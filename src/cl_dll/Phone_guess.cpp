// ======================================
// Cry of Fear mobile phone panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Phone.h"
#include "..\game_shared\vgui_loadtga.h"

CPhone::CPhone() : Panel( 0, 0, ScreenWidth, ScreenHeight )
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

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );

	x = ScreenWidth / 2 - 131;
	y = ScreenHeight / 2 - 281;

	guess_m_pPhoneImage = vgui_LoadTGA( "gfx/vgui/640_phone.tga" );
	if ( guess_m_pPhoneImage )
		guess_m_pPhoneImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_phone.tga!\n" );

	guess_m_pPhone = new ImagePanel( guess_m_pPhoneImage );
	guess_m_pPhone->setParent( this );
	guess_m_pPhone->setPos( x, y );
	guess_m_pPhone->setVisible( true );

	pButton = new CommandButton( "", 17, 306, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 1 ) );

	pButton = new CommandButton( "", 97, 306, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 2 ) );

	pButton = new CommandButton( "", 177, 306, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 3 ) );

	pButton = new CommandButton( "", 17, 344, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 4 ) );

	pButton = new CommandButton( "", 97, 344, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 5 ) );

	pButton = new CommandButton( "", 177, 344, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 6 ) );

	pButton = new CommandButton( "", 17, 382, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 7 ) );

	pButton = new CommandButton( "", 97, 382, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 8 ) );

	pButton = new CommandButton( "", 177, 382, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 9 ) );

	pButton = new CommandButton( "", 98, 420, 64, 26, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 10 ) );

	pButton = new CommandButton( "", 94, 220, 75, 75, true, true );
	pButton->setParent( guess_m_pPhone );
	pButton->addActionSignal( new CPhoneHandler_Command( this, 11 ) );

	guess_m_pNumberLabel = new Label( "", 65, 84, 131, 20 );
	guess_m_pNumberLabel->setVisible( true );
	guess_m_pNumberLabel->setParent( guess_m_pPhone );
	guess_m_pNumberLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pNumberLabel->setBgColor( 0, 0, 0, 0 );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CPhone::ActionSignal( int cmd )
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
	}
}

void CPhone::guess_PressDigit( int digit )
{
	char szDigit[4];

	sprintf( szDigit, "%d", digit );

	switch ( digit )
	{
	case 0:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_zero.wav", 1 );
		break;
	case 1:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_one.wav", 1 );
		break;
	case 2:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_two.wav", 1 );
		break;
	case 3:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_three.wav", 1 );
		break;
	case 4:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_four.wav", 1 );
		break;
	case 5:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_five.wav", 1 );
		break;
	case 6:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_six.wav", 1 );
		break;
	case 7:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_seven.wav", 1 );
		break;
	case 8:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_eight.wav", 1 );
		break;
	case 9:
		gEngfuncs.pfnPlaySoundByName( "phone/tone_nine.wav", 1 );
		break;
	}

	strcat( guess_m_szNumber, szDigit );

	if ( strlen( guess_m_szNumber ) >= 8 )
		guess_Dial();

	guess_m_pNumberLabel->setText( guess_m_szNumber, strlen( guess_m_szNumber ) );
	guess_m_pNumberLabel->setContentAlignment( Label::a_center );
}

void CPhone::guess_Clear()
{
	strncpy( guess_m_szNumber, "", 16 );

	guess_m_pNumberLabel->setText( guess_m_szNumber, strlen( guess_m_szNumber ) );
	guess_m_pNumberLabel->setContentAlignment( Label::a_center );
}

void CPhone::Initialize()
{
	setVisible( false );
	guess_Clear();
}

int CPhone::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && ( !strcmp( pszCurrentBinding, "toggleconsole" ) || !strcmp( pszCurrentBinding, "phonenumber" ) ) )
		return 1;

	return 0;
}

int guess_MsgPhone( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pPhone )
	{
		int iOn = READ_BYTE();

		if ( !gViewPort->guess_m_pPhone->isVisible() && iOn )
		{
			gViewPort->guess_m_pPhone->setVisible( true );
			gViewPort->UpdateCursorState();
		}
	}

	return 1;
}

void CPhone::guess_Dial()
{
	char szCommand[128];

	sprintf( szCommand, "phonenumber %s", guess_m_szNumber );
	gEngfuncs.pfnServerCmd( szCommand );

	guess_Clear();
	strncpy( guess_m_szNumber, "", 16 );

	gEngfuncs.pfnPlaySoundByName( "phone/tone_send.wav", 1 );

	if ( isVisible() )
	{
		setVisible( false );
		gViewPort->UpdateCursorState();
	}
}

void guess_PhoneInit()
{
	gEngfuncs.pfnHookUserMsg( "Phone", guess_MsgPhone );
}
