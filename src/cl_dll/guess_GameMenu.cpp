// ======================================
// Cry of Fear game menu
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include <string>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_GameMenu.h"
#include "guess_SaveLoad.h"
#include "guess_Unlockables.h"
#include "guess_VGUIExtraMenu.h"
#include "guess_Language.h"
#include "guess_Difficulty.h"
#include "..\game_shared\vgui_loadtga.h"
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>

void guess_IrrKlangPlay( const char *pszFile );
void guess_IrrKlangSetLoop( bool bLoop );

const char *guess_GetVersion();

CGameMenu::CGameMenu() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int tsWide, tsTall;
	int logoWide, logoTall;
	int x, y;
	int logoX, logoY;
	int i;

	guess_m_pLogoImage = vgui_LoadTGA( "gfx/vgui/menu_logo.tga" );
	if ( guess_m_pLogoImage )
		guess_m_pLogoImage->setColor( Color( 255, 255, 255, 0 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/menu_logo.tga!\n" );

	guess_m_pNewGameImage = vgui_LoadTGA( "gfx/vgui/menu/newgame.tga" );
	if ( guess_m_pNewGameImage )
		guess_m_pNewGameImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pLoadGameImage = vgui_LoadTGA( "gfx/vgui/menu/loadgame.tga" );
	if ( guess_m_pLoadGameImage )
		guess_m_pNewGameImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pCustomImage = vgui_LoadTGA( "gfx/vgui/menu/customc.tga" );
	if ( guess_m_pCustomImage )
		guess_m_pCustomImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pJoinServerImage = vgui_LoadTGA( "gfx/vgui/menu/jserver.tga" );
	if ( guess_m_pJoinServerImage )
		guess_m_pJoinServerImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pOptionsImage = vgui_LoadTGA( "gfx/vgui/menu/options.tga" );
	if ( guess_m_pOptionsImage )
		guess_m_pOptionsImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pLanguageImage = vgui_LoadTGA( "gfx/vgui/menu/language.tga" );
	if ( guess_m_pLanguageImage )
		guess_m_pLanguageImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pUnlockablesImage = vgui_LoadTGA( "gfx/vgui/menu/unlockables.tga" );
	if ( guess_m_pUnlockablesImage )
		guess_m_pUnlockablesImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pExtrasImage = vgui_LoadTGA( "gfx/vgui/menu/extras.tga" );
	if ( guess_m_pExtrasImage )
		guess_m_pExtrasImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pHostServerImage = vgui_LoadTGA( "gfx/vgui/menu/hserver.tga" );
	if ( guess_m_pHostServerImage )
		guess_m_pHostServerImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pQuitImage = vgui_LoadTGA( "gfx/vgui/menu/quit.tga" );
	if ( guess_m_pQuitImage )
		guess_m_pQuitImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pLogoTSImage = vgui_LoadTGA( "gfx/vgui/menu_logo_ts.tga" );
	if ( guess_m_pLogoTSImage )
		guess_m_pLogoTSImage->setColor( Color( 255, 255, 255, 0 ) );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hCreditsScheme = pSchemes->getSchemeHandle( "credits" );
	guess_m_pCreditsFont = pSchemes->getFont( hCreditsScheme );

	guess_m_pVersionLabel = new Label( "", 5, 5, 256, 32 );
	guess_m_pVersionLabel->setFont( guess_m_pCreditsFont );
	guess_m_pVersionLabel->setParent( this );
	guess_m_pVersionLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pVersionLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pVersionLabel->setVisible( true );
	guess_m_pVersionLabel->setText( guess_GetVersion() );
	guess_m_pVersionLabel->setContentAlignment( Label::a_west );

	guess_m_pNewGameImage->getSize( wide, tall );
	guess_m_pLogoTSImage->getSize( tsWide, tsTall );
	y = ScreenHeight - tall;
	x = ScreenWidth / 2 - wide / 2;

	guess_m_pLogoImage->getSize( logoWide, logoTall );
	logoX = ScreenWidth / 2 - logoWide / 2;
	logoY = ScreenHeight / 2 - logoTall * 3;

	guess_m_pLogo = new ImagePanel( guess_m_pLogoImage );
	guess_m_pLogo->setParent( this );
	guess_m_pLogo->setPos( logoX, logoY );
	guess_m_pLogo->setVisible( true );

	guess_m_pLogoTS = new ImagePanel( guess_m_pLogoTSImage );
	guess_m_pLogoTS->setParent( this );
	guess_m_pLogoTS->setPos( ScreenWidth - tsWide - 15, ScreenHeight - tsTall - 15 );
	guess_m_pLogoTS->setVisible( true );

	guess_m_pNewGame = new ImagePanel( guess_m_pNewGameImage );
	guess_m_pNewGame->setParent( this );
	guess_m_pNewGame->setPos( x, y - tall * 10 );
	guess_m_pNewGame->setVisible( true );

	guess_m_pLoadGame = new ImagePanel( guess_m_pLoadGameImage );
	guess_m_pLoadGame->setParent( this );
	guess_m_pLoadGame->setPos( x, y - tall * 9 );
	guess_m_pLoadGame->setVisible( true );

	guess_m_pCustom = new ImagePanel( guess_m_pCustomImage );
	guess_m_pCustom->setParent( this );
	guess_m_pCustom->setPos( x, y - tall * 8 );
	guess_m_pCustom->setVisible( true );

	guess_m_pJoinServer = new ImagePanel( guess_m_pJoinServerImage );
	guess_m_pJoinServer->setParent( this );
	guess_m_pJoinServer->setPos( x, y - tall * 7 );
	guess_m_pJoinServer->setVisible( true );

	guess_m_pOptions = new ImagePanel( guess_m_pOptionsImage );
	guess_m_pOptions->setParent( this );
	guess_m_pOptions->setPos( x, y - tall * 2 );
	guess_m_pOptions->setVisible( true );

	guess_m_pLanguage = new ImagePanel( guess_m_pLanguageImage );
	guess_m_pLanguage->setParent( this );
	guess_m_pLanguage->setPos( x, y - tall * 5 );
	guess_m_pLanguage->setVisible( true );

	guess_m_pUnlockables = new ImagePanel( guess_m_pUnlockablesImage );
	guess_m_pUnlockables->setParent( this );
	guess_m_pUnlockables->setPos( x, y - tall * 4 );
	guess_m_pUnlockables->setVisible( true );

	guess_m_pExtras = new ImagePanel( guess_m_pExtrasImage );
	guess_m_pExtras->setParent( this );
	guess_m_pExtras->setPos( x, y - tall * 3 );
	guess_m_pExtras->setVisible( true );

	guess_m_pHostServer = new ImagePanel( guess_m_pHostServerImage );
	guess_m_pHostServer->setParent( this );
	guess_m_pHostServer->setPos( x, y - tall * 6 );
	guess_m_pHostServer->setVisible( true );

	guess_m_pQuit = new ImagePanel( guess_m_pQuitImage );
	guess_m_pQuit->setParent( this );
	guess_m_pQuit->setPos( x, y - tall );
	guess_m_pQuit->setVisible( true );

	guess_m_pNewGameButton = new ColorButton( "", x, y - tall * 10, wide, tall, false, true );
	guess_m_pNewGameButton->setParent( this );
	guess_m_pNewGameButton->addActionSignal( new CGameMenuHandler_Command( this, 1 ) );

	guess_m_pLoadGameButton = new ColorButton( "", x, y - tall * 9, wide, tall, false, true );
	guess_m_pLoadGameButton->setParent( this );
	guess_m_pLoadGameButton->addActionSignal( new CGameMenuHandler_Command( this, 2 ) );

	guess_m_pCustomButton = new ColorButton( "", x, y - tall * 8, wide, tall, false, true );
	guess_m_pCustomButton->setParent( this );
	guess_m_pCustomButton->addActionSignal( new CGameMenuHandler_Command( this, 3 ) );

	guess_m_pJoinServerButton = new ColorButton( "", x, y - tall * 7, wide, tall, false, true );
	guess_m_pJoinServerButton->setParent( this );
	guess_m_pJoinServerButton->addActionSignal( new CGameMenuHandler_Command( this, 4 ) );

	guess_m_pOptionsButton = new ColorButton( "", x, y - tall * 2, wide, tall, false, true );
	guess_m_pOptionsButton->setParent( this );
	guess_m_pOptionsButton->addActionSignal( new CGameMenuHandler_Command( this, 5 ) );

	guess_m_pLanguageButton = new ColorButton( "", x, y - tall * 5, wide, tall, false, true );
	guess_m_pLanguageButton->setParent( this );
	guess_m_pLanguageButton->addActionSignal( new CGameMenuHandler_Command( this, 6 ) );

	guess_m_pUnlockablesButton = new ColorButton( "", x, y - tall * 4, wide, tall, false, true );
	guess_m_pUnlockablesButton->setParent( this );
	guess_m_pUnlockablesButton->addActionSignal( new CGameMenuHandler_Command( this, 7 ) );

	guess_m_pExtrasButton = new ColorButton( "", x, y - tall * 3, wide, tall, false, true );
	guess_m_pExtrasButton->setParent( this );
	guess_m_pExtrasButton->addActionSignal( new CGameMenuHandler_Command( this, 8 ) );

	guess_m_pHostServerButton = new ColorButton( "", x, y - tall * 6, wide, tall, false, true );
	guess_m_pHostServerButton->setParent( this );
	guess_m_pHostServerButton->addActionSignal( new CGameMenuHandler_Command( this, 9 ) );

	guess_m_pQuitButton = new ColorButton( "", x, y - tall, wide, tall, false, true );
	guess_m_pQuitButton->setParent( this );
	guess_m_pQuitButton->addActionSignal( new CGameMenuHandler_Command( this, 10 ) );

	// list of the custom campaigns
	int iCancelWide = ScreenWidth;

	guess_m_pCampaignList = new CTFScrollPanel( ScreenWidth / 2 - 350, ScreenHeight / 2 - 262, 700, 525 );
	guess_m_pCampaignList->setParent( this );
	guess_m_pCampaignList->setScrollBarVisible( false, false );

	ScrollBar *pScrollBar = guess_m_pCampaignList->getVerticalScrollBar();
	pScrollBar->setSlider( new CCampaignSlider( 0, 700 - 1, 700, ( 525 - ( 700 * 2 ) ) + 2, true ) );

	for ( i = 0; i < guess_MAX_CAMPAIGNS; i++ )
	{
		guess_m_pCampaignButton[i] = new ColorButton( "N/A", 25, i * 32, 650, 32, false, true );
		guess_m_pCampaignButton[i]->setParent( guess_m_pCampaignList->getClient() );
		guess_m_pCampaignButton[i]->addActionSignal( new CGameMenuHandler_Command( this, i + 50 ) );
		guess_m_pCampaignButton[i]->setVisible( false );
		guess_m_pCampaignButton[i]->setText( "N/A" );
		guess_m_pCampaignButton[i]->setContentAlignment( Label::a_center );
	}

	guess_m_pAuthorLabel = new Label( " ", 0, 0, ScreenWidth, 64 );
	guess_m_pAuthorLabel->setParent( this );
	guess_m_pAuthorLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pAuthorLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pAuthorLabel->setText( " " );
	guess_m_pAuthorLabel->setFont( guess_m_pCreditsFont );
	guess_m_pAuthorLabel->setContentAlignment( Label::a_east );

	guess_m_pCancelButton = new ColorButton( "Cancel", 0, ScreenHeight / 2 + 262, iCancelWide, 32, false, true );
	guess_m_pCancelButton->setParent( this );
	guess_m_pCancelButton->addActionSignal( new CGameMenuHandler_Command( this, 11 ) );
	guess_m_pCancelButton->setVisible( false );
	guess_m_pCancelButton->setContentAlignment( Label::a_center );
	guess_m_pCancelButton->setUnArmedColor( 255, 255, 255, 0 );
	guess_m_pCancelButton->setArmedColor( 255, 0, 0, 0 );

	setBgColor( 0, 0, 0, 255 );

	guess_m_pCampaignList->setScrollValue( 0, 0 );
	guess_m_pCampaignList->validate();

	Initialize();
}

void CGameMenu::ActionSignal( int cmd )
{
	int i;

	if ( !gEngfuncs.GetLocalPlayer() )
		return;

	int index = cmd - 50;

	if ( index >= 0 && index < 30 && guess_m_Campaigns[index].bValid )
	{
		setVisible( false );
		gViewPort->UpdateCursorState();
		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );

		if ( gViewPort->guess_m_pDifficulty )
		{
			guess_m_pszCampaign = guess_m_Campaigns[index].szFirstMap;
			gViewPort->guess_m_pDifficulty->setVisible( true );
			gViewPort->UpdateCursorState();
		}
	}

	switch ( cmd )
	{
	case 1:
		setVisible( false );
		gViewPort->UpdateCursorState();
		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );

		if ( gViewPort->guess_m_pDifficulty )
		{
			guess_m_pszCampaign = NULL;
			gViewPort->guess_m_pDifficulty->setVisible( true );
			gViewPort->UpdateCursorState();
		}
		break;
	case 2:
		setVisible( false );
		gViewPort->UpdateCursorState();
		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );

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

			gViewPort->guess_m_pSaveLoad->guess_m_iMode = 1;
			gViewPort->guess_m_pSaveLoad->guess_UpdateSlots();
			gViewPort->guess_m_pSaveLoad->setVisible( true );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnServerCmd( "freeze 1024" );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );
		}
		break;
	case 3:
		guess_m_pNewGameButton->setVisible( false );
		guess_m_pLoadGameButton->setVisible( false );
		guess_m_pCustomButton->setVisible( false );
		guess_m_pJoinServerButton->setVisible( false );
		guess_m_pOptionsButton->setVisible( false );
		guess_m_pLanguageButton->setVisible( false );
		guess_m_pUnlockablesButton->setVisible( false );
		guess_m_pExtrasButton->setVisible( false );
		guess_m_pHostServerButton->setVisible( false );
		guess_m_pQuitButton->setVisible( false );
		guess_m_pLogo->setVisible( false );
		guess_m_pNewGame->setVisible( false );
		guess_m_pLoadGame->setVisible( false );
		guess_m_pCustom->setVisible( false );
		guess_m_pJoinServer->setVisible( false );
		guess_m_pOptions->setVisible( false );
		guess_m_pLanguage->setVisible( false );
		guess_m_pUnlockables->setVisible( false );
		guess_m_pExtras->setVisible( false );
		guess_m_pHostServer->setVisible( false );
		guess_m_pQuit->setVisible( false );
		guess_m_pCancelButton->setVisible( true );
		guess_m_pCampaignList->setVisible( true );
		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );

		for ( i = 0; i < guess_MAX_CAMPAIGNS; i++ )
			guess_m_pCampaignButton[i]->setVisible( true );

		guess_LoadCampaigns();
		guess_m_pCampaignList->validate();
		break;
	case 4:
		gEngfuncs.pfnServerCmd( "optionshack 0" );
		break;
	case 5:
		gEngfuncs.pfnServerCmd( "optionshack 2" );
		break;
	case 6:
		setVisible( false );
		gViewPort->UpdateCursorState();
		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );

		if ( gViewPort->guess_m_pLanguage )
		{
			gViewPort->guess_m_pLanguage->setVisible( true );
			gViewPort->UpdateCursorState();
		}
		break;
	case 7:
		if ( gViewPort && gViewPort->guess_m_pUnlockables )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			gViewPort->guess_m_pUnlockables->setVisible( true );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );
		}
		break;
	case 8:
		if ( gViewPort->guess_m_pVGUIExtraMenu )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			gViewPort->guess_m_pVGUIExtraMenu->setVisible( true );
			gViewPort->UpdateCursorState();
		}
		break;
	case 9:
		gEngfuncs.pfnServerCmd( "optionshack 1" );
		break;
	case 10:
		setVisible( false );
		gEngfuncs.pfnClientCmd( "quit" );
		gViewPort->UpdateCursorState();
		break;
	case 11:
		guess_m_pNewGameButton->setVisible( true );
		guess_m_pLoadGameButton->setVisible( true );
		guess_m_pCustomButton->setVisible( true );
		guess_m_pJoinServerButton->setVisible( true );
		guess_m_pOptionsButton->setVisible( true );
		guess_m_pLanguageButton->setVisible( true );
		guess_m_pUnlockablesButton->setVisible( true );
		guess_m_pExtrasButton->setVisible( true );
		guess_m_pHostServerButton->setVisible( true );
		guess_m_pQuitButton->setVisible( true );
		guess_m_pLogo->setVisible( true );
		guess_m_pNewGame->setVisible( true );
		guess_m_pLoadGame->setVisible( true );
		guess_m_pCustom->setVisible( true );
		guess_m_pJoinServer->setVisible( true );
		guess_m_pOptions->setVisible( true );
		guess_m_pLanguage->setVisible( true );
		guess_m_pUnlockables->setVisible( true );
		guess_m_pExtras->setVisible( true );
		guess_m_pHostServer->setVisible( true );
		guess_m_pQuit->setVisible( true );
		guess_m_pCancelButton->setVisible( false );
		guess_m_pCampaignList->setVisible( false );
		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );

		for ( i = 0; i < guess_MAX_CAMPAIGNS; i++ )
			guess_m_pCampaignButton[i]->setVisible( false );
		break;
	}
}

const char *guess_GetVersion()
{
	return "1.6a";
}

void CGameMenu::Initialize()
{
	guess_m_bUnknownC0 = false;
	memset( guess_m_Campaigns, 0, sizeof( guess_m_Campaigns ) );

	guess_m_pCampaignList->setScrollValue( 0, 0 );
	guess_m_pszCampaign = NULL;

	setVisible( false );
}

int guess_MsgGameMenu( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pGameMenu )
	{
		gViewPort->guess_m_pGameMenu->guess_m_bNewGameArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bLoadGameArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bCustomArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bJoinServerArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bOptionsArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bLanguageArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bUnlockablesArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bExtrasArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bHostServerArmed = false;
		gViewPort->guess_m_pGameMenu->guess_m_bQuitArmed = false;

		gViewPort->guess_m_pGameMenu->guess_m_pNewGame->setImage( vgui_LoadTGA( "gfx/vgui/menu/newgame.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pLoadGame->setImage( vgui_LoadTGA( "gfx/vgui/menu/loadgame.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pCustom->setImage( vgui_LoadTGA( "gfx/vgui/menu/customc.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pJoinServer->setImage( vgui_LoadTGA( "gfx/vgui/menu/jserver.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pOptions->setImage( vgui_LoadTGA( "gfx/vgui/menu/options.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pLanguage->setImage( vgui_LoadTGA( "gfx/vgui/menu/language.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pUnlockables->setImage( vgui_LoadTGA( "gfx/vgui/menu/unlockables.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pExtras->setImage( vgui_LoadTGA( "gfx/vgui/menu/extras.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pHostServer->setImage( vgui_LoadTGA( "gfx/vgui/menu/hserver.tga" ) );
		gViewPort->guess_m_pGameMenu->guess_m_pQuit->setImage( vgui_LoadTGA( "gfx/vgui/menu/quit.tga" ) );

		gViewPort->guess_m_pGameMenu->setVisible( true );
		gViewPort->UpdateCursorState();
		gEngfuncs.pfnServerCmd( "freeze 1024" );
	}

	return 1;
}

void CGameMenu::guess_ParseCampaign( char *pszFile, int index )
{
	char token[1024];
	char szPath[256];
	char *pFile;

	gEngfuncs.Con_Printf( "PARSING CUSTOM CAMPAIGN: %s\n", pszFile );

	strcpy( szPath, "maps/" );
	strcat( szPath, pszFile );

	pFile = (char *)gEngfuncs.COM_LoadFile( szPath, 5, NULL );

	if ( !pFile )
	{
		gEngfuncs.Con_Printf( "PARSING CUSTOM CAMPAIGN: Couldn't open campaign file %s\n", pszFile );
		return;
	}

	for ( pFile = gEngfuncs.COM_ParseFile( pFile, token ); pFile; pFile = gEngfuncs.COM_ParseFile( pFile, token ) )
	{
		if ( !stricmp( token, "firstmap" ) )
		{
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
			strncpy( guess_m_Campaigns[index].szFirstMap, token, 64 );
			guess_m_Campaigns[index].bValid = true;
		}
		else if ( !stricmp( token, "author" ) )
		{
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
			strncpy( guess_m_Campaigns[index].szAuthor, token, 64 );
			guess_m_Campaigns[index].bValid = true;
		}
		else if ( !stricmp( token, "description" ) )
		{
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
			strncpy( guess_m_Campaigns[index].szDescription, token, 64 );
			guess_m_Campaigns[index].bValid = true;

			if ( guess_m_pCampaignButton[index] )
			{
				int w, h;
				int x, y;
				int wide, tall;

				guess_m_pCreditsFont->getTextSize( token, w, h );
				guess_m_pCampaignButton[index]->getPos( x, y );
				guess_m_pCampaignButton[index]->setText( token );
				guess_m_pCampaignButton[index]->setArmedColor( 255, 0, 0, 0 );
				guess_m_pCampaignButton[index]->setUnArmedColor( 255, 255, 255, 0 );
				guess_m_pCampaignButton[index]->getSize( wide, tall );
				w = w * 1.2;
				guess_m_pCampaignButton[index]->setContentAlignment( Label::a_center );
			}
		}
	}

	if ( !guess_m_Campaigns[index].bValid )
		gEngfuncs.Con_Printf( "PARSING CUSTOM CAMPAIGN: Couldn't parse data from campaign file %s\n", pszFile );
}

void CGameMenu::guess_LoadCampaigns()
{
	int i;

	for ( i = 0; i < guess_MAX_CAMPAIGNS; i++ )
	{
		guess_m_pCampaignButton[i]->setText( "N/A" );
		guess_m_pCampaignButton[i]->setUnArmedColor( 255, 255, 255, 0 );
		guess_m_pCampaignButton[i]->setArmedColor( 255, 255, 255, 0 );
	}

	guess_m_pszCampaign = NULL;
	guess_m_bUnknownC0 = false;
	memset( guess_m_Campaigns, 0, sizeof( guess_m_Campaigns ) );

	char szDir[1024];
	strcpy( szDir, gEngfuncs.pfnGetGameDirectory() );
	strcat( szDir, "\\maps\\" );

	std::string dir = szDir;
	std::string ext = "*.custom";
	std::string search = dir + ext;

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( search.c_str(), &fd );

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			std::string file = dir + fd.cFileName;
			struct stat st;

			if ( !stat( file.c_str(), &st ) )
			{
				for ( i = 0; i < guess_MAX_CAMPAIGNS; i++ )
				{
					if ( !guess_m_Campaigns[i].bValid )
					{
						guess_ParseCampaign( fd.cFileName, i );
						break;
					}
				}
			}
		} while ( FindNextFile( hFind, &fd ) > 0 );
	}
}

void CGameMenu::guess_ResetButtonPositions()
{
	int wide, tall;
	int tsWide, tsTall;
	int logoWide, logoTall;
	int x, y;

	guess_m_pNewGameImage->getSize( wide, tall );
	guess_m_pLogoTSImage->getSize( tsWide, tsTall );
	y = ScreenHeight - tall;
	x = ScreenWidth / 2 - wide / 2;

	guess_m_pLogoImage->getSize( logoWide, logoTall );

	guess_m_pNewGame->setPos( x, y - tall * 10 );
	guess_m_pLoadGame->setPos( x, y - tall * 9 );
	guess_m_pCustom->setPos( x, y - tall * 8 );
	guess_m_pJoinServer->setPos( x, y - tall * 7 );
	guess_m_pOptions->setPos( x, y - tall * 2 );
	guess_m_pLanguage->setPos( x, y - tall * 5 );
	guess_m_pUnlockables->setPos( x, y - tall * 4 );
	guess_m_pExtras->setPos( x, y - tall * 3 );
	guess_m_pHostServer->setPos( x, y - tall * 6 );
	guess_m_pQuit->setPos( x, y - tall );
}

void guess_GameMenuInit()
{
	gEngfuncs.pfnHookUserMsg( "GameMenu", guess_MsgGameMenu );
}

void CCampaignSlider::paintBackground()
{
	int wide, tall, nobx, noby;

	getPaintSize( wide, tall );
	getNobPos( nobx, noby );

	drawSetColor( 128, 128, 128, 150 );
	drawFilledRect( 0, 0, wide, tall );

	drawSetColor( 200, 200, 200, 0 );
	drawFilledRect( 0, nobx, wide, noby );

	drawSetColor( 0, 0, 0, 150 );
	drawOutlinedRect( 0, nobx, wide - 1, noby - 1 );

	drawSetColor( 255, 255, 255, 150 );
	drawOutlinedRect( 1, nobx + 1, wide, noby );

	drawSetColor( 0, 0, 0, 0 );
	drawOutlinedRect( 0, nobx, wide, noby );
}

void CGameMenu::setVisible( bool state )
{
	if ( state )
	{
		guess_ResetButtonPositions();
		guess_IrrKlangSetLoop( true );
		guess_IrrKlangPlay( "gamestartup.mp3" );

		guess_m_iArmedCampaign = -1;
		guess_m_bCampaignArmed = true;

		guess_m_pNewGameButton->setVisible( true );
		guess_m_pLoadGameButton->setVisible( true );
		guess_m_pCustomButton->setVisible( true );
		guess_m_pJoinServerButton->setVisible( true );
		guess_m_pOptionsButton->setVisible( true );
		guess_m_pLanguageButton->setVisible( true );
		guess_m_pUnlockablesButton->setVisible( true );
		guess_m_pExtrasButton->setVisible( true );
		guess_m_pHostServerButton->setVisible( true );
		guess_m_pQuitButton->setVisible( true );
		guess_m_pLogo->setVisible( true );
		guess_m_pNewGame->setVisible( true );
		guess_m_pLoadGame->setVisible( true );
		guess_m_pCustom->setVisible( true );
		guess_m_pJoinServer->setVisible( true );
		guess_m_pOptions->setVisible( true );
		guess_m_pLanguage->setVisible( true );
		guess_m_pUnlockables->setVisible( true );
		guess_m_pExtras->setVisible( true );
		guess_m_pHostServer->setVisible( true );
		guess_m_pQuit->setVisible( true );
		guess_m_pCancelButton->setVisible( false );
		guess_m_pCampaignList->setVisible( false );
		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 0.5 );

		for ( int i = 0; i < guess_MAX_CAMPAIGNS; i++ )
			guess_m_pCampaignButton[i]->setVisible( false );
	}

	Panel::setVisible( state );
}

void CGameMenu::solve()
{
	char szText[256];

	if ( !gEngfuncs.GetLocalPlayer() )
		return;

	guess_m_pLogoImage->setColor( Color( 255, 255, 255, gEngfuncs.pfnRandomLong( 0, 50 ) ) );

	if ( guess_m_pNewGameButton->isArmed() )
	{
		if ( !guess_m_bNewGameArmed )
		{
			guess_m_pNewGame->setImage( vgui_LoadTGA( "gfx/vgui/menu/newgame_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bNewGameArmed = true;
			guess_m_pNewGame->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pNewGame->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bNewGameArmed )
	{
		guess_m_pNewGame->setImage( vgui_LoadTGA( "gfx/vgui/menu/newgame.tga" ) );
		guess_m_bNewGameArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pLoadGameButton->isArmed() )
	{
		if ( !guess_m_bLoadGameArmed )
		{
			guess_m_pLoadGame->setImage( vgui_LoadTGA( "gfx/vgui/menu/loadgame_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bLoadGameArmed = true;
			guess_m_pLoadGame->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pLoadGame->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bLoadGameArmed )
	{
		guess_m_pLoadGame->setImage( vgui_LoadTGA( "gfx/vgui/menu/loadgame.tga" ) );
		guess_m_bLoadGameArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pCustomButton->isArmed() )
	{
		if ( !guess_m_bCustomArmed )
		{
			guess_m_pCustom->setImage( vgui_LoadTGA( "gfx/vgui/menu/customc_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bCustomArmed = true;
			guess_m_pCustom->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pCustom->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bCustomArmed )
	{
		guess_m_pCustom->setImage( vgui_LoadTGA( "gfx/vgui/menu/customc.tga" ) );
		guess_m_bCustomArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pJoinServerButton->isArmed() )
	{
		if ( !guess_m_bJoinServerArmed )
		{
			guess_m_pJoinServer->setImage( vgui_LoadTGA( "gfx/vgui/menu/jserver_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bJoinServerArmed = true;
			guess_m_pJoinServer->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pJoinServer->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bJoinServerArmed )
	{
		guess_m_pJoinServer->setImage( vgui_LoadTGA( "gfx/vgui/menu/jserver.tga" ) );
		guess_m_bJoinServerArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pOptionsButton->isArmed() )
	{
		if ( !guess_m_bOptionsArmed )
		{
			guess_m_pOptions->setImage( vgui_LoadTGA( "gfx/vgui/menu/options_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bOptionsArmed = true;
			guess_m_pOptions->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pOptions->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bOptionsArmed )
	{
		guess_m_pOptions->setImage( vgui_LoadTGA( "gfx/vgui/menu/options.tga" ) );
		guess_m_bOptionsArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pLanguageButton->isArmed() )
	{
		if ( !guess_m_bLanguageArmed )
		{
			guess_m_pLanguage->setImage( vgui_LoadTGA( "gfx/vgui/menu/language_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bLanguageArmed = true;
			guess_m_pLanguage->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pLanguage->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bLanguageArmed )
	{
		guess_m_pLanguage->setImage( vgui_LoadTGA( "gfx/vgui/menu/language.tga" ) );
		guess_m_bLanguageArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pUnlockablesButton->isArmed() )
	{
		if ( !guess_m_bUnlockablesArmed )
		{
			guess_m_pUnlockables->setImage( vgui_LoadTGA( "gfx/vgui/menu/unlockables_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bUnlockablesArmed = true;
			guess_m_pUnlockables->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pUnlockables->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bUnlockablesArmed )
	{
		guess_m_pUnlockables->setImage( vgui_LoadTGA( "gfx/vgui/menu/unlockables.tga" ) );
		guess_m_bUnlockablesArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pExtrasButton->isArmed() )
	{
		if ( !guess_m_bExtrasArmed )
		{
			guess_m_pExtras->setImage( vgui_LoadTGA( "gfx/vgui/menu/extras_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bExtrasArmed = true;
			guess_m_pExtras->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pExtras->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bExtrasArmed )
	{
		guess_m_pExtras->setImage( vgui_LoadTGA( "gfx/vgui/menu/extras.tga" ) );
		guess_m_bExtrasArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pHostServerButton->isArmed() )
	{
		if ( !guess_m_bHostServerArmed )
		{
			guess_m_pHostServer->setImage( vgui_LoadTGA( "gfx/vgui/menu/hserver_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bHostServerArmed = true;
			guess_m_pHostServer->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pHostServer->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bHostServerArmed )
	{
		guess_m_pHostServer->setImage( vgui_LoadTGA( "gfx/vgui/menu/hserver.tga" ) );
		guess_m_bHostServerArmed = false;
		guess_ResetButtonPositions();
	}

	if ( guess_m_pQuitButton->isArmed() )
	{
		if ( !guess_m_bQuitArmed )
		{
			guess_m_pQuit->setImage( vgui_LoadTGA( "gfx/vgui/menu/quit_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 0.5 );
			guess_m_bQuitArmed = true;
			guess_m_pQuit->getPos( guess_m_iArmedX, guess_m_iArmedY );
		}
		else
		{
			guess_m_pQuit->setPos( guess_m_iArmedX + gEngfuncs.pfnRandomLong( -3, 3 ), guess_m_iArmedY );
		}
	}
	else if ( guess_m_bQuitArmed )
	{
		guess_m_pQuit->setImage( vgui_LoadTGA( "gfx/vgui/menu/quit.tga" ) );
		guess_m_bQuitArmed = false;
		guess_ResetButtonPositions();
	}

	bool bArmed = false;
	guess_m_iArmedCampaign = -1;

	for ( int i = 0; i < guess_MAX_CAMPAIGNS; i++ )
	{
		if ( guess_m_pCampaignButton[i]->isArmed() && guess_m_iArmedCampaign != i )
		{
			guess_m_iArmedCampaign = i;

			if ( guess_m_Campaigns[i].szAuthor && guess_m_Campaigns[i].bValid )
			{
				sprintf( szText, "Author: %s", guess_m_Campaigns[i].szAuthor );
				guess_m_pAuthorLabel->setText( szText );
				guess_m_pAuthorLabel->setVisible( true );
			}
			else
			{
				guess_m_pAuthorLabel->setText( "" );
				guess_m_pAuthorLabel->setVisible( false );
			}

			guess_m_bCampaignArmed = true;
		}

		if ( guess_m_pCampaignButton[i]->isArmed() )
		{
			guess_m_iArmedCampaign = i;
			bArmed = true;
		}
	}

	if ( !guess_m_bCampaignArmed && guess_m_iArmedCampaign >= 0 )
		guess_m_iArmedCampaign = -1;

	if ( !bArmed && guess_m_pAuthorLabel->isVisible() )
	{
		guess_m_pAuthorLabel->setText( "" );
		guess_m_pAuthorLabel->setVisible( false );
	}

	Panel::solve();
}
