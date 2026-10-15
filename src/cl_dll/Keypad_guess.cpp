// ======================================
// Cry of Fear keypad puzzle panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Keypad.h"
#include "..\game_shared\vgui_loadtga.h"

CKeypad::CKeypad() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	CommandButton *pButton;

	guess_m_pBackgroundPanel = new Panel( 0, 0, ScreenWidth, ScreenHeight );
	guess_m_pBackgroundPanel->setParent( this );
	guess_m_pBackgroundPanel->setBgColor( 0, 0, 0, 0 );

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

	guess_m_pPanelBackImage = vgui_LoadTGA( "gfx/vgui/640_panelback.tga" );
	if ( guess_m_pPanelBackImage )
		guess_m_pPanelBackImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_panelback.tga!\n" );

	guess_m_pPanel2BackImage = vgui_LoadTGA( "gfx/vgui/640_panel2back.tga" );
	if ( guess_m_pPanel2BackImage )
		guess_m_pPanel2BackImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_panel2back.tga!\n" );

	guess_m_pPanelOffImage = vgui_LoadTGA( "gfx/vgui/640_paneloff.tga" );
	if ( guess_m_pPanelOffImage )
		guess_m_pPanelOffImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_paneloff.tga!\n" );

	guess_m_pPanelOnImage = vgui_LoadTGA( "gfx/vgui/640_panelon.tga" );
	if ( guess_m_pPanelOnImage )
		guess_m_pPanelOnImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_panelon.tga!\n" );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hCreditsScheme = pSchemes->getSchemeHandle( "credits" );
	guess_m_pFont = pSchemes->getFont( hCreditsScheme );

	x = ScreenWidth / 2 - 320;
	y = ScreenHeight / 2 - 240;

	guess_m_pPanelBack = new ImagePanel( guess_m_pPanelBackImage );
	guess_m_pPanelBack->setParent( this );
	guess_m_pPanelBack->setPos( x, y );
	guess_m_pPanelBack->setVisible( true );

	guess_m_pPanel2Back = new ImagePanel( guess_m_pPanel2BackImage );
	guess_m_pPanel2Back->setParent( guess_m_pPanelBack );
	guess_m_pPanel2Back->setPos( 0, 0 );
	guess_m_pPanel2Back->setVisible( false );

	pButton = new CommandButton( "", 3, 3, 20, 20, true, true );
	pButton->setParent( guess_m_pPanelBack );
	pButton->addActionSignal( new CKeypadHandler_Command( this, 1 ) );

	guess_m_pPanelOff = new ImagePanel( guess_m_pPanelOffImage );
	guess_m_pPanelOff->setParent( guess_m_pPanelBack );
	guess_m_pPanelOff->setPos( 520, 0 );
	guess_m_pPanelOff->setVisible( true );

	guess_m_pPanelOn = new ImagePanel( guess_m_pPanelOnImage );
	guess_m_pPanelOn->setParent( guess_m_pPanelBack );
	guess_m_pPanelOn->setPos( 520, 0 );
	guess_m_pPanelOn->setVisible( false );

	guess_m_pKey1 = new CommandButton( "", 170, 90, 100, 100, true, true );
	guess_m_pKey1->setParent( guess_m_pPanelBack );
	guess_m_pKey1->addActionSignal( new CKeypadHandler_Command( this, 2 ) );

	guess_m_pKey2 = new CommandButton( "", 270, 90, 100, 100, true, true );
	guess_m_pKey2->setParent( guess_m_pPanelBack );
	guess_m_pKey2->addActionSignal( new CKeypadHandler_Command( this, 3 ) );

	guess_m_pKey3 = new CommandButton( "", 370, 90, 100, 100, true, true );
	guess_m_pKey3->setParent( guess_m_pPanelBack );
	guess_m_pKey3->addActionSignal( new CKeypadHandler_Command( this, 4 ) );

	guess_m_pKey4 = new CommandButton( "", 170, 190, 100, 100, true, true );
	guess_m_pKey4->setParent( guess_m_pPanelBack );
	guess_m_pKey4->addActionSignal( new CKeypadHandler_Command( this, 5 ) );

	guess_m_pKey5 = new CommandButton( "", 270, 190, 100, 100, true, true );
	guess_m_pKey5->setParent( guess_m_pPanelBack );
	guess_m_pKey5->addActionSignal( new CKeypadHandler_Command( this, 6 ) );

	guess_m_pKey6 = new CommandButton( "", 370, 190, 100, 100, true, true );
	guess_m_pKey6->setParent( guess_m_pPanelBack );
	guess_m_pKey6->addActionSignal( new CKeypadHandler_Command( this, 7 ) );

	guess_m_pKey7 = new CommandButton( "", 170, 290, 100, 100, true, true );
	guess_m_pKey7->setParent( guess_m_pPanelBack );
	guess_m_pKey7->addActionSignal( new CKeypadHandler_Command( this, 8 ) );

	guess_m_pKey8 = new CommandButton( "", 270, 290, 100, 100, true, true );
	guess_m_pKey8->setParent( guess_m_pPanelBack );
	guess_m_pKey8->addActionSignal( new CKeypadHandler_Command( this, 9 ) );

	guess_m_pKey9 = new CommandButton( "", 370, 290, 100, 100, true, true );
	guess_m_pKey9->setParent( guess_m_pPanelBack );
	guess_m_pKey9->addActionSignal( new CKeypadHandler_Command( this, 10 ) );

	guess_m_pCodeLabel = new Label( "", 247, 39, 149, 32 );
	guess_m_pCodeLabel->setParent( guess_m_pPanelBack );
	guess_m_pCodeLabel->setFont( guess_m_pFont );
	guess_m_pCodeLabel->setFgColor( 0, 0, 0, 1 );
	guess_m_pCodeLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pCodeLabel->setText( "    " );
	guess_m_pCodeLabel->setContentAlignment( Label::a_center );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

CKeypad::~CKeypad()
{
	delete guess_m_pPanelBackImage;
	delete guess_m_pPanelOffImage;
	delete guess_m_pPanelOnImage;
}

void CKeypad::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		setVisible( false );
		gViewPort->UpdateCursorState();
		break;
	case 2:
		guess_PressKey( 1 );
		break;
	case 3:
		guess_PressKey( 2 );
		break;
	case 4:
		guess_PressKey( 3 );
		break;
	case 5:
		guess_PressKey( 4 );
		break;
	case 6:
		guess_PressKey( 5 );
		break;
	case 7:
		guess_PressKey( 6 );
		break;
	case 8:
		guess_PressKey( 7 );
		break;
	case 9:
		guess_PressKey( 8 );
		break;
	case 10:
		guess_PressKey( 9 );
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown Keypad ActionSignal %i.\n", cmd );
		break;
	}
}

void CKeypad::guess_Activate()
{
	gEngfuncs.pfnPlaySoundByName( "keypad/keypad_activate.wav", 1 );

	if ( guess_m_iType == 0 )
	{
		guess_m_pPanelOn->setVisible( true );
		guess_m_pPanelOff->setVisible( false );
	}
}

void CKeypad::guess_PressKey( int key )
{
	char szDigit[4];
	char szStars[8];

	if ( guess_m_iActivated )
	{
		gEngfuncs.pfnPlaySoundByName( "keypad/keypad_press.wav", 1 );
		return;
	}

	if ( strlen( guess_m_szCode ) == 4 )
	{
		if ( guess_m_iType == 2 )
		{
			for ( int i = 1; i <= strlen( guess_m_szCode ); i++ )
			{
				if ( i == 1 )
					strcpy( szStars, "*" );
				else
					strcat( szStars, "*" );
			}

			guess_m_pCodeLabel->setText( szStars );
			guess_m_pCodeLabel->setContentAlignment( Label::a_center );
		}

		guess_Submit();
		return;
	}

	itoa( key, szDigit, 10 );
	strcat( guess_m_szCode, szDigit );
	gEngfuncs.pfnPlaySoundByName( "keypad/keypad_press.wav", 1 );

	if ( guess_m_iType == 2 )
	{
		for ( int i = 1; i <= strlen( guess_m_szCode ); i++ )
		{
			if ( i == 1 )
				strcpy( szStars, "*" );
			else
				strcat( szStars, "*" );
		}

		guess_m_pCodeLabel->setText( szStars );
		guess_m_pCodeLabel->setContentAlignment( Label::a_center );
	}

	if ( strlen( guess_m_szCode ) == 4 )
		guess_Submit();
}

void CKeypad::Initialize()
{
	strncpy( guess_m_szCode, "", 8 );
	setVisible( false );

	guess_m_iType = 0;
	guess_m_iActivated = 0;

	guess_m_pPanelBack->setVisible( true );
	guess_m_pPanel2Back->setVisible( false );

	if ( guess_m_iType == 0 )
	{
		guess_m_pPanelOff->setVisible( true );
		guess_m_pPanelOn->setVisible( false );
	}
	else
	{
		guess_m_pPanelOff->setVisible( false );
		guess_m_pPanelOn->setVisible( false );
	}
}

int CKeypad::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && ( !strcmp( pszCurrentBinding, "toggleconsole" ) || !strcmp( pszCurrentBinding, "keypadnumber" ) ) )
		return 1;

	return 0;
}

int guess_MsgKeypad( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pKeypad )
	{
		gViewPort->guess_m_pKeypad->guess_m_iType = READ_BYTE();
		gViewPort->guess_m_pKeypad->guess_m_iActivated = READ_BYTE();
		gViewPort->guess_m_pKeypad->guess_UpdateLayout();

		int iOn = READ_BYTE();

		switch ( gViewPort->guess_m_pKeypad->guess_m_iType )
		{
		case 0:
			gViewPort->guess_m_pKeypad->guess_m_pPanelBack->setImage( vgui_LoadTGA( "gfx/vgui/640_panelback.tga" ) );
			gViewPort->guess_m_pKeypad->guess_m_pPanelOff->setVisible( true );
			gViewPort->guess_m_pKeypad->guess_m_pPanelOn->setVisible( false );
			break;
		case 1:
			gViewPort->guess_m_pKeypad->guess_m_pPanelOff->setVisible( false );
			gViewPort->guess_m_pKeypad->guess_m_pPanelOn->setVisible( false );
			gViewPort->guess_m_pKeypad->guess_m_pPanelBack->setImage( vgui_LoadTGA( "gfx/vgui/640_panel2back.tga" ) );
			break;
		case 2:
			gViewPort->guess_m_pKeypad->guess_m_pPanelOff->setVisible( false );
			gViewPort->guess_m_pKeypad->guess_m_pPanelOn->setVisible( false );
			gViewPort->guess_m_pKeypad->guess_m_pPanelBack->setImage( vgui_LoadTGA( "gfx/vgui/keypad3.tga" ) );
			break;
		}

		if ( gViewPort->guess_m_pKeypad->guess_m_iActivated )
			gViewPort->guess_m_pKeypad->guess_Activate();

		if ( !gViewPort->guess_m_pKeypad->isVisible() && iOn )
		{
			gViewPort->guess_m_pKeypad->setVisible( true );
			gViewPort->UpdateCursorState();
		}
	}

	return 1;
}

void CKeypad::guess_Submit()
{
	char szCommand[128];

	sprintf( szCommand, "keypadnumber %s", guess_m_szCode );
	gEngfuncs.pfnServerCmd( szCommand );

	strncpy( guess_m_szCode, "", 8 );
}

void CKeypad::guess_UpdateLayout()
{
	if ( guess_m_iType == 0 || guess_m_iType == 1 )
	{
		guess_m_pCodeLabel->setText( "" );
		guess_m_pCodeLabel->setVisible( false );
	}
	else
	{
		guess_m_pCodeLabel->setText( "" );
		guess_m_pCodeLabel->setVisible( true );
	}

	guess_m_pKey1->setSize( 100, 100 );
	guess_m_pKey1->setPos( 170, 90 );
	guess_m_pKey2->setSize( 100, 100 );
	guess_m_pKey2->setPos( 270, 90 );
	guess_m_pKey3->setSize( 100, 100 );
	guess_m_pKey3->setPos( 370, 90 );
	guess_m_pKey4->setSize( 100, 100 );
	guess_m_pKey4->setPos( 170, 190 );
	guess_m_pKey5->setSize( 100, 100 );
	guess_m_pKey5->setPos( 270, 190 );
	guess_m_pKey6->setSize( 100, 100 );
	guess_m_pKey6->setPos( 370, 190 );
	guess_m_pKey7->setSize( 100, 100 );
	guess_m_pKey7->setPos( 170, 290 );
	guess_m_pKey8->setSize( 100, 100 );
	guess_m_pKey8->setPos( 270, 290 );
	guess_m_pKey9->setSize( 100, 100 );
	guess_m_pKey9->setPos( 370, 290 );
}

void guess_KeypadInit()
{
	gEngfuncs.pfnHookUserMsg( "Keypad", guess_MsgKeypad );
}
