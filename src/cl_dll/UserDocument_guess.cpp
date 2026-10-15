// ======================================
// Cry of Fear computer user and password document panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_UserDocument.h"
#include "..\game_shared\vgui_loadtga.h"

CUserDocument::CUserDocument( int iTrans, int iRemoveMe, int x, int y, int wide, int tall ) : CMenuPanel( iTrans, iRemoveMe, x, y, wide, tall )
{
	int iXPos, iYPos;
	int iWide, iTall;
	int iX, iY;

	guess_m_pBackgroundImage = vgui_LoadTGA( "gfx/vgui/backgroundtoall.tga" );
	if ( guess_m_pBackgroundImage )
		guess_m_pBackgroundImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/backgroundtoall.tga!\n" );

	guess_m_pBackgroundImage->getSize( iWide, iTall );
	iX = ScreenWidth / 2 - iWide / 2;
	iY = ScreenHeight / 2 - iTall / 2;

	guess_m_pBackground = new ImagePanel( guess_m_pBackgroundImage );
	guess_m_pBackground->setParent( this );
	guess_m_pBackground->setPos( iX, iY );
	guess_m_pBackground->setVisible( true );

	iXPos = ScreenWidth / 2 - 320;
	iYPos = ScreenHeight / 2 - 240;

	setBgColor( Color( 0, 0, 0, 0 ) );

	guess_m_pDocumentImage = vgui_LoadTGA( "gfx/vgui/640_pass1.tga" );
	if ( guess_m_pDocumentImage )
		guess_m_pDocumentImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_pass1.tga!\n" );

	guess_m_pDocument = new ImagePanel( guess_m_pDocumentImage );
	guess_m_pDocument->setParent( this );
	guess_m_pDocument->setPos( iXPos, iYPos );
	guess_m_pDocument->setVisible( true );

	guess_m_pCancelLabel = new CImageLabel( "notecancel", iXPos - 5, iYPos - 5 );
	guess_m_pCancelLabel->setVisible( true );
	guess_m_pCancelLabel->setParent( this );

	guess_m_pCloseButton = new CommandButton( "", iXPos, iYPos, 20, 20, true, true );
	guess_m_pCloseButton->setParent( this );
	guess_m_pCloseButton->addActionSignal( new CUserDocumentHandler_Command( this, 1 ) );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hCreditsScheme = pSchemes->getSchemeHandle( "credits" );
	guess_m_pFont = pSchemes->getFont( hCreditsScheme );
}

void CUserDocument::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		gViewPort->HideTopMenu();
		gEngfuncs.pfnServerCmd( "closedocument" );
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown userdoc ActionSignal %i.\n", cmd );
		break;
	}
}

const char *CUserDocument::guess_GetPassword( int index )
{
	switch ( index )
	{
	case 1:
		return "apple45";
	case 2:
		return "cloudyday";
	case 3:
		return "rosesarered";
	case 4:
		return "junesummer";
	case 5:
		return "unforgettable";
	case 6:
		return "thebeatles54";
	case 7:
		return "marriageinapril";
	case 8:
		return "jillanddavid";
	}

	return "?";
}

const char *CUserDocument::guess_GetUsername( int index )
{
	switch ( index )
	{
	case 1:
		return "davidspc";
	case 2:
		return "jillusername";
	case 3:
		return "excelsior";
	case 4:
		return "dorrarpc";
	case 5:
		return "confuseduser";
	case 6:
		return "leatherhoff45";
	case 7:
		return "ea45";
	case 8:
		return "dorrardavid";
	}

	return "?";
}

void CUserDocument::Initialize()
{
	guess_m_iType = 0;
	guess_m_iIndex = 0;
}

int CUserDocument::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && !strcmp( pszCurrentBinding, "toggleconsole" ) )
		return 1;

	return 0;
}

int guess_MsgUserDoc( const char *pszName, int iSize, void *pbuf )
{
	char szImage[128];

	BEGIN_READ( pbuf, iSize );

	if ( gViewPort && gViewPort->guess_m_pUserDocument )
	{
		gViewPort->guess_m_pUserDocument->guess_m_iType = READ_BYTE();
		gViewPort->guess_m_pUserDocument->guess_m_iIndex = READ_BYTE();

		if ( gViewPort->guess_m_pUserDocument->guess_m_iType == 1 )
		{
			sprintf( szImage, "gfx/vgui/640_user%i.tga", gViewPort->guess_m_pUserDocument->guess_m_iIndex );
			gViewPort->guess_m_pUserDocument->guess_m_pDocument->setImage( vgui_LoadTGA( szImage ) );
		}

		if ( gViewPort->guess_m_pUserDocument->guess_m_iType == 2 )
		{
			sprintf( szImage, "gfx/vgui/640_pass%i.tga", gViewPort->guess_m_pUserDocument->guess_m_iIndex );
			gViewPort->guess_m_pUserDocument->guess_m_pDocument->setImage( vgui_LoadTGA( szImage ) );
		}

		gViewPort->ShowVGUIMenu( 34 );
	}

	return 1;
}

void guess_UserDocInit()
{
	gEngfuncs.pfnHookUserMsg( "UserDoc", guess_MsgUserDoc );
}

void CUserDocument::paint()
{
	if ( guess_m_iType == 1 && guess_m_iIndex )
	{
		int wide, tall;
		const char *pszText = guess_GetUsername( guess_m_iIndex );

		drawSetTextFont( guess_m_pFont );
		drawSetTextColor( 200, 200, 200, 1 );
		guess_m_pFont->getTextSize( pszText, wide, tall );
		drawSetTextPos( ScreenWidth / 2 - wide / 2, ScreenHeight - tall * 2 );
		drawPrintText( pszText, strlen( pszText ) );
	}

	if ( guess_m_iType == 2 && guess_m_iIndex )
	{
		int wide, tall;
		const char *pszText = guess_GetPassword( guess_m_iIndex );

		drawSetTextFont( guess_m_pFont );
		drawSetTextColor( 200, 200, 200, 1 );
		guess_m_pFont->getTextSize( pszText, wide, tall );
		drawSetTextPos( ScreenWidth / 2 - wide / 2, ScreenHeight - tall * 2 );
		drawPrintText( pszText, strlen( pszText ) );
	}

	Panel::paint();
}
