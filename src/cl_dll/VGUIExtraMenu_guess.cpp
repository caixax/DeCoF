// ======================================
// Cry of Fear extras menu
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_VGUIExtraMenu.h"
#include "guess_GameMenu.h"
#include "..\game_shared\vgui_loadtga.h"

//=========================================================
// Declarations this file takes from the Steamworks SDK
// (steam_api.h, isteamfriends.h, steamclientpublic.h)
//=========================================================
typedef unsigned __int64 uint64;
typedef uint64 SteamAPICall_t;

class CSteamID
{
private:
	uint64 m_unAll64Bits;
};

struct FriendGameInfo_t;

class ISteamFriends
{
public:
	virtual const char *GetPersonaName() = 0;
	virtual SteamAPICall_t SetPersonaName( const char *pchPersonaName ) = 0;
	virtual int GetPersonaState() = 0;
	virtual int GetFriendCount( int iFriendFlags ) = 0;
	virtual CSteamID GetFriendByIndex( int iFriend, int iFriendFlags ) = 0;
	virtual int GetFriendRelationship( CSteamID steamIDFriend ) = 0;
	virtual int GetFriendPersonaState( CSteamID steamIDFriend ) = 0;
	virtual const char *GetFriendPersonaName( CSteamID steamIDFriend ) = 0;
	virtual bool GetFriendGamePlayed( CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo ) = 0;
	virtual const char *GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName ) = 0;
	virtual bool HasFriend( CSteamID steamIDFriend, int iFriendFlags ) = 0;
	virtual int GetClanCount() = 0;
	virtual CSteamID GetClanByIndex( int iClan ) = 0;
	virtual const char *GetClanName( CSteamID steamIDClan ) = 0;
	virtual const char *GetClanTag( CSteamID steamIDClan ) = 0;
	virtual bool GetClanActivityCounts( CSteamID steamIDClan, int *pnOnline, int *pnInGame, int *pnChatting ) = 0;
	virtual SteamAPICall_t DownloadClanActivityCounts( CSteamID *psteamIDClans, int cClansToRequest ) = 0;
	virtual int GetFriendCountFromSource( CSteamID steamIDSource ) = 0;
	virtual CSteamID GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend ) = 0;
	virtual bool IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource ) = 0;
	virtual void SetInGameVoiceSpeaking( CSteamID steamIDUser, bool bSpeaking ) = 0;
	virtual void ActivateGameOverlay( const char *pchDialog ) = 0;
	virtual void ActivateGameOverlayToUser( const char *pchDialog, CSteamID steamID ) = 0;
	virtual void ActivateGameOverlayToWebPage( const char *pchURL ) = 0;
};

extern "C" __declspec( dllimport ) ISteamFriends * __cdecl SteamFriends();
//=========================================================

CVGUIExtraMenu::CVGUIExtraMenu() : Panel( ScreenWidth / 2 - 400, ScreenHeight / 2 - 300, 800, 600 )
{
	int wide, tall;
	int cancelWide, cancelTall;

	guess_m_pDonateImage = vgui_LoadTGA( "gfx/vgui/menu/donate.tga" );
	guess_m_pDonateImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pSoundtrack1Image = vgui_LoadTGA( "gfx/vgui/menu/soundtrack1.tga" );
	guess_m_pSoundtrack1Image->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pSoundtrack2Image = vgui_LoadTGA( "gfx/vgui/menu/soundtrack2.tga" );
	guess_m_pSoundtrack2Image->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pClothesImage = vgui_LoadTGA( "gfx/vgui/menu/clothes.tga" );
	guess_m_pClothesImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pAoMDCImage = vgui_LoadTGA( "gfx/vgui/menu/aomdc.tga" );
	guess_m_pAoMDCImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pFacebookImage = vgui_LoadTGA( "gfx/vgui/menu/facebook.tga" );
	guess_m_pFacebookImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pCancelImage = vgui_LoadTGA( "gfx/vgui/menu/cancel.tga" );
	guess_m_pCancelImage->setColor( Color( 255, 255, 255, 0 ) );

	guess_m_pDonateImage->getSize( wide, tall );
	guess_m_pCancelImage->getSize( cancelWide, cancelTall );

	guess_m_pDonate = new ImagePanel( guess_m_pDonateImage );
	guess_m_pDonate->setParent( this );
	guess_m_pDonate->setPos( 400 - wide / 2, 0 );
	guess_m_pDonate->setVisible( true );

	guess_m_pSoundtrack1 = new ImagePanel( guess_m_pSoundtrack1Image );
	guess_m_pSoundtrack1->setParent( this );
	guess_m_pSoundtrack1->setPos( 400 - wide / 2, tall );
	guess_m_pSoundtrack1->setVisible( true );

	guess_m_pSoundtrack2 = new ImagePanel( guess_m_pSoundtrack2Image );
	guess_m_pSoundtrack2->setParent( this );
	guess_m_pSoundtrack2->setPos( 400 - wide / 2, tall * 2 );
	guess_m_pSoundtrack2->setVisible( true );

	guess_m_pClothes = new ImagePanel( guess_m_pClothesImage );
	guess_m_pClothes->setParent( this );
	guess_m_pClothes->setPos( 400 - wide / 2, tall * 3 );
	guess_m_pClothes->setVisible( true );

	guess_m_pAoMDC = new ImagePanel( guess_m_pAoMDCImage );
	guess_m_pAoMDC->setParent( this );
	guess_m_pAoMDC->setPos( 400 - wide / 2, tall * 4 );
	guess_m_pAoMDC->setVisible( true );

	guess_m_pFacebook = new ImagePanel( guess_m_pFacebookImage );
	guess_m_pFacebook->setParent( this );
	guess_m_pFacebook->setPos( 400 - wide / 2, tall * 5 );
	guess_m_pFacebook->setVisible( true );

	guess_m_pCancel = new ImagePanel( guess_m_pCancelImage );
	guess_m_pCancel->setParent( this );
	guess_m_pCancel->setPos( 400 - cancelWide / 2, 600 - cancelTall );
	guess_m_pCancel->setVisible( true );

	guess_m_pDonateButton = new ColorButton( "", 400 - wide / 2, 0, wide, tall, false, true );
	guess_m_pDonateButton->setParent( this );
	guess_m_pDonateButton->addActionSignal( new CVGUIExtraMenuHandler_Command( this, 2 ) );

	guess_m_pSoundtrack1Button = new ColorButton( "", 400 - wide / 2, tall, wide, tall, false, true );
	guess_m_pSoundtrack1Button->setParent( this );
	guess_m_pSoundtrack1Button->addActionSignal( new CVGUIExtraMenuHandler_Command( this, 3 ) );

	guess_m_pSoundtrack2Button = new ColorButton( "", 400 - wide / 2, tall * 2, wide, tall, false, true );
	guess_m_pSoundtrack2Button->setParent( this );
	guess_m_pSoundtrack2Button->addActionSignal( new CVGUIExtraMenuHandler_Command( this, 4 ) );

	guess_m_pClothesButton = new ColorButton( "", 400 - wide / 2, tall * 3, wide, tall, false, true );
	guess_m_pClothesButton->setParent( this );
	guess_m_pClothesButton->addActionSignal( new CVGUIExtraMenuHandler_Command( this, 5 ) );

	guess_m_pAoMDCButton = new ColorButton( "", 400 - wide / 2, tall * 4, wide, tall, false, true );
	guess_m_pAoMDCButton->setParent( this );
	guess_m_pAoMDCButton->addActionSignal( new CVGUIExtraMenuHandler_Command( this, 6 ) );

	guess_m_pFacebookButton = new ColorButton( "", 400 - wide / 2, tall * 5, wide, tall, false, true );
	guess_m_pFacebookButton->setParent( this );
	guess_m_pFacebookButton->addActionSignal( new CVGUIExtraMenuHandler_Command( this, 7 ) );

	guess_m_pCancelButton = new ColorButton( "", 400 - cancelWide / 2, 600 - cancelTall, cancelWide, cancelTall, false, true );
	guess_m_pCancelButton->setParent( this );
	guess_m_pCancelButton->addActionSignal( new CVGUIExtraMenuHandler_Command( this, 1 ) );

	setBgColor( 0, 0, 0, 255 );

	Initialize();
}

CVGUIExtraMenu::~CVGUIExtraMenu()
{
	delete guess_m_pDonateImage;
	delete guess_m_pSoundtrack1Image;
	delete guess_m_pSoundtrack2Image;
	delete guess_m_pClothesImage;
	delete guess_m_pAoMDCImage;
	delete guess_m_pFacebookImage;
	delete guess_m_pCancelImage;
}

void CVGUIExtraMenu::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 1:
		setVisible( false );
		gViewPort->UpdateCursorState();

		if ( gViewPort && gViewPort->guess_m_pGameMenu )
		{
			gViewPort->guess_m_pGameMenu->setVisible( true );
			gViewPort->UpdateCursorState();
		}
		break;
	case 2:
		if ( SteamFriends() )
			SteamFriends()->ActivateGameOverlayToWebPage( "http://www.cry-of-fear.com/donate.php" );

		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		break;
	case 3:
		if ( SteamFriends() )
			SteamFriends()->ActivateGameOverlayToWebPage( "http://www.cdbaby.com/cd/andreasronnberg" );

		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		break;
	case 4:
		if ( SteamFriends() )
			SteamFriends()->ActivateGameOverlayToWebPage( "http://www.cdbaby.com/cd/andreasronnberg2" );

		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		break;
	case 5:
		if ( SteamFriends() )
			SteamFriends()->ActivateGameOverlayToWebPage( "http://teampsykskallar.spreadshirt.se/" );

		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		break;
	case 6:
		if ( SteamFriends() )
			SteamFriends()->ActivateGameOverlayToWebPage( "http://www.moddb.com/mods/afraid-of-monsters-dc" );

		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		break;
	case 7:
		if ( SteamFriends() )
			SteamFriends()->ActivateGameOverlayToWebPage( "http://www.facebook.com/CryOfFearOfficial" );

		gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
		break;
	}
}

void CVGUIExtraMenu::Initialize()
{
	setVisible( false );
}

void CVGUIExtraMenu::setVisible( bool state )
{
	if ( state )
	{
		guess_m_bDonateArmed = false;
		guess_m_bSoundtrack1Armed = false;
		guess_m_bSoundtrack2Armed = false;
		guess_m_bClothesArmed = false;
		guess_m_bAoMDCArmed = false;
		guess_m_bFacebookArmed = false;
		guess_m_bCancelArmed = false;
	}

	Panel::setVisible( state );
}

void CVGUIExtraMenu::solve()
{
	if ( guess_m_pDonateButton->isArmed() )
	{
		if ( !guess_m_bDonateArmed )
		{
			guess_m_pDonate->setImage( vgui_LoadTGA( "gfx/vgui/menu/donate_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bDonateArmed = true;
		}
	}
	else if ( guess_m_bDonateArmed )
	{
		guess_m_pDonate->setImage( vgui_LoadTGA( "gfx/vgui/menu/donate.tga" ) );
		guess_m_bDonateArmed = false;
	}

	if ( guess_m_pSoundtrack1Button->isArmed() )
	{
		if ( !guess_m_bSoundtrack1Armed )
		{
			guess_m_pSoundtrack1->setImage( vgui_LoadTGA( "gfx/vgui/menu/soundtrack1_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bSoundtrack1Armed = true;
		}
	}
	else if ( guess_m_bSoundtrack1Armed )
	{
		guess_m_pSoundtrack1->setImage( vgui_LoadTGA( "gfx/vgui/menu/soundtrack1.tga" ) );
		guess_m_bSoundtrack1Armed = false;
	}

	if ( guess_m_pSoundtrack2Button->isArmed() )
	{
		if ( !guess_m_bSoundtrack2Armed )
		{
			guess_m_pSoundtrack2->setImage( vgui_LoadTGA( "gfx/vgui/menu/soundtrack2_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bSoundtrack2Armed = true;
		}
	}
	else if ( guess_m_bSoundtrack2Armed )
	{
		guess_m_pSoundtrack2->setImage( vgui_LoadTGA( "gfx/vgui/menu/soundtrack2.tga" ) );
		guess_m_bSoundtrack2Armed = false;
	}

	if ( guess_m_pClothesButton->isArmed() )
	{
		if ( !guess_m_bClothesArmed )
		{
			guess_m_pClothes->setImage( vgui_LoadTGA( "gfx/vgui/menu/clothes_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bClothesArmed = true;
		}
	}
	else if ( guess_m_bClothesArmed )
	{
		guess_m_pClothes->setImage( vgui_LoadTGA( "gfx/vgui/menu/clothes.tga" ) );
		guess_m_bClothesArmed = false;
	}

	if ( guess_m_pAoMDCButton->isArmed() )
	{
		if ( !guess_m_bAoMDCArmed )
		{
			guess_m_pAoMDC->setImage( vgui_LoadTGA( "gfx/vgui/menu/aomdc_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bAoMDCArmed = true;
		}
	}
	else if ( guess_m_bAoMDCArmed )
	{
		guess_m_pAoMDC->setImage( vgui_LoadTGA( "gfx/vgui/menu/aomdc.tga" ) );
		guess_m_bAoMDCArmed = false;
	}

	if ( guess_m_pFacebookButton->isArmed() )
	{
		if ( !guess_m_bFacebookArmed )
		{
			guess_m_pFacebook->setImage( vgui_LoadTGA( "gfx/vgui/menu/facebook_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bFacebookArmed = true;
		}
	}
	else if ( guess_m_bFacebookArmed )
	{
		guess_m_pFacebook->setImage( vgui_LoadTGA( "gfx/vgui/menu/facebook.tga" ) );
		guess_m_bFacebookArmed = false;
	}

	if ( guess_m_pCancelButton->isArmed() )
	{
		if ( !guess_m_bCancelArmed )
		{
			guess_m_pCancel->setImage( vgui_LoadTGA( "gfx/vgui/menu/cancel_mo.tga" ) );
			gEngfuncs.pfnPlaySoundByName( "ui/3d_rollover.wav", 1 );
			guess_m_bCancelArmed = true;
		}
	}
	else if ( guess_m_bCancelArmed )
	{
		guess_m_pCancel->setImage( vgui_LoadTGA( "gfx/vgui/menu/cancel.tga" ) );
		guess_m_bCancelArmed = false;
	}

	Panel::solve();
}
