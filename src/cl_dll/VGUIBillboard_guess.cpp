// ======================================
// Cry of Fear billboard panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_VGUIBillboard.h"
#include "guess_HUDControl.h"
#include "..\game_shared\vgui_loadtga.h"

CVGUIBillboard::CVGUIBillboard() : Panel( 0, 0, ScreenWidth, ScreenHeight )
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

	if ( !vgui_LoadTGA( "gfx/vgui/640_gustavdahlwelcome.tga" ) )
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_gustavdahlwelcome.tga!\n" );

	x = ScreenWidth / 2 - 320;
	y = ScreenHeight / 2 - 240;

	guess_m_pImage = new ImagePanel( vgui_LoadTGA( "gfx/vgui/640_gustavdahlwelcome.tga" ) );
	guess_m_pImage->setParent( this );
	guess_m_pImage->setPos( x, y );
	guess_m_pImage->setVisible( true );

	pButton = new CommandButton( "", x, y, 640, 480, true, true );
	pButton->setParent( this );
	pButton->addActionSignal( new CBillboardHandler_Command( this, 1 ) );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	wide = 640;
	tall = 480;
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "credits" );
	SchemeHandle_t hChapterScheme = pSchemes->getSchemeHandle( "chaptertitle" );
	SchemeHandle_t hTextScheme = pSchemes->getSchemeHandle( "credits" );
	guess_m_pFont = pSchemes->getFont( hTitleScheme );

	guess_m_pLabel = new Label( " ", 0, 0, ScreenWidth, 64 );
	guess_m_pLabel->setParent( this );
	guess_m_pLabel->setFont( guess_m_pFont );
	guess_m_pLabel->setFgColor( 0, 0, 0, 0 );
	guess_m_pLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pLabel->setText( " " );
	guess_m_pLabel->setContentAlignment( Label::a_center );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

void CVGUIBillboard::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		setVisible( false );
		gViewPort->UpdateCursorState();
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown Billboard ActionSignal %i.\n", cmd );
		break;
	}
}

void CVGUIBillboard::Initialize()
{
	guess_m_pLabel->setText( "" );
	setVisible( false );
}

int CVGUIBillboard::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && !strcmp( pszCurrentBinding, "toggleconsole" ) )
		return 1;

	return 0;
}

int guess_MsgBoard( const char *pszName, int iSize, void *pbuf )
{
	char szImage[128];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pVGUIBillboard )
	{
		sprintf( szImage, "%s", READ_STRING() );

		if ( strlen( szImage ) > 1 )
		{
			if ( vgui_LoadTGA( szImage ) )
				gViewPort->guess_m_pVGUIBillboard->guess_m_pImage->setImage( vgui_LoadTGA( szImage ) );
			else
				gEngfuncs.Con_Printf( "Cannot load %s!\n", szImage );
		}
		else
		{
			gEngfuncs.Con_Printf( "No message sent\n" );
		}

		if ( !gViewPort->guess_m_pVGUIBillboard->isVisible() )
		{
			gViewPort->guess_m_pVGUIBillboard->setVisible( true );
			gViewPort->UpdateCursorState();
		}
	}

	return 1;
}

void guess_BoardInit()
{
	gEngfuncs.pfnHookUserMsg( "Board", guess_MsgBoard );
}

void CVGUIBillboard::paint()
{
	int x, y;
	int wide, tall;
	int r, g, b, a;

	if ( gViewPort && gViewPort->guess_m_pHUDControl && gViewPort->guess_m_pHUDControl->guess_m_szMessage[0] )
	{
		if ( strlen( gViewPort->guess_m_pHUDControl->guess_m_szMessage ) > 1 )
		{
			gViewPort->guess_m_pHUDControl->guess_m_pMessageLabel->getPos( x, y );
			gViewPort->guess_m_pHUDControl->guess_m_pMessageLabel->getSize( wide, tall );
			gViewPort->guess_m_pHUDControl->guess_m_pMessageLabel->getFgColor( r, g, b, a );

			guess_m_pLabel->setSize( wide, tall );
			guess_m_pLabel->setText( gViewPort->guess_m_pHUDControl->guess_m_szMessage );
			guess_m_pLabel->setPos( x, y );
			guess_m_pLabel->setContentAlignment( Label::a_center );
			guess_m_pLabel->setFgColor( r, g, b, a );
		}
	}

	Panel::paint();
}

void CVGUIBillboard::paintBackground()
{
	Panel::paintBackground();
}

void CVGUIBillboard::setVisible( bool state )
{
	Panel::setVisible( state );
}
