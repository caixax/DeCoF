// ======================================
// Cry of Fear inventory panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_RECBInventory.h"
#include "guess_Documents.h"
#include "guess_HUDControl.h"
#include "..\game_shared\vgui_loadtga.h"

extern int guess_iWheelchair;

bool guess_g_bInventoryFadeOut;

CRECBInventory::CRECBInventory( int iTrans, int iRemoveMe, int x, int y, int wide, int tall ) : CMenuPanel( iTrans, iRemoveMe, x, y, wide, tall )
{
	int iWide, iTall;
	int iX, iY;
	int i;

	guess_m_pBackgroundPanel = new Panel( 0, 0, ScreenWidth, ScreenHeight );
	guess_m_pBackgroundPanel->setParent( this );
	guess_m_pBackgroundPanel->setBgColor( 0, 0, 0, 0 );

	guess_m_iThree = 0;

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

	iX = ScreenWidth / 2 - 374;
	iY = ScreenHeight / 2 - 240;

	guess_m_pInventoryImage = vgui_LoadTGA( "gfx/vgui/640_inventory.tga" );
	if ( guess_m_pInventoryImage )
		guess_m_pInventoryImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/640_inventory.tga!\n" );

	guess_m_pInventory = new ImagePanel( guess_m_pInventoryImage );
	guess_m_pInventory->setParent( this );
	guess_m_pInventory->setPos( iX, iY );
	guess_m_pInventory->setVisible( true );

	guess_m_pItemImages[0] = new CImageLabel( NULL, 31, 68 );
	guess_m_pItemImages[0]->setVisible( true );
	guess_m_pItemImages[0]->setParent( guess_m_pInventory );
	guess_m_pItemImages[0]->setImage( NULL );

	guess_m_pItemImages[1] = new CImageLabel( NULL, 133, 68 );
	guess_m_pItemImages[1]->setVisible( true );
	guess_m_pItemImages[1]->setParent( guess_m_pInventory );
	guess_m_pItemImages[1]->setImage( NULL );

	guess_m_pItemImages[2] = new CImageLabel( NULL, 235, 68 );
	guess_m_pItemImages[2]->setVisible( true );
	guess_m_pItemImages[2]->setParent( guess_m_pInventory );
	guess_m_pItemImages[2]->setImage( NULL );

	guess_m_pItemImages[3] = new CImageLabel( NULL, 31, 227 );
	guess_m_pItemImages[3]->setVisible( true );
	guess_m_pItemImages[3]->setParent( guess_m_pInventory );
	guess_m_pItemImages[3]->setImage( NULL );

	guess_m_pItemImages[4] = new CImageLabel( NULL, 133, 227 );
	guess_m_pItemImages[4]->setVisible( true );
	guess_m_pItemImages[4]->setParent( guess_m_pInventory );
	guess_m_pItemImages[4]->setImage( NULL );

	guess_m_pItemImages[5] = new CImageLabel( NULL, 235, 227 );
	guess_m_pItemImages[5]->setVisible( true );
	guess_m_pItemImages[5]->setParent( guess_m_pInventory );
	guess_m_pItemImages[5]->setImage( NULL );

	guess_m_pItemImages[6] = new CImageLabel( NULL, 398, 68 );
	guess_m_pItemImages[6]->setVisible( true );
	guess_m_pItemImages[6]->setParent( guess_m_pInventory );
	guess_m_pItemImages[6]->setImage( NULL );

	guess_m_pItemImages[7] = new CImageLabel( NULL, 500, 68 );
	guess_m_pItemImages[7]->setVisible( true );
	guess_m_pItemImages[7]->setParent( guess_m_pInventory );
	guess_m_pItemImages[7]->setImage( NULL );

	guess_m_pItemImages[8] = new CImageLabel( NULL, 31, 348 );
	guess_m_pItemImages[8]->setVisible( true );
	guess_m_pItemImages[8]->setParent( guess_m_pInventory );
	guess_m_pItemImages[8]->setImage( NULL );

	guess_m_pItemImages[9] = new CImageLabel( NULL, 602, 68 );
	guess_m_pItemImages[9]->setVisible( true );
	guess_m_pItemImages[9]->setParent( guess_m_pInventory );
	guess_m_pItemImages[9]->setImage( NULL );

	guess_m_pItemButtons[0] = new ColorButton( "", 36, 73, 99, 99, false, false );
	guess_m_pItemButtons[0]->setParent( guess_m_pInventory );
	guess_m_pItemButtons[0]->addActionSignal( new CRECBInventoryHandler_Command( this, 1 ) );

	guess_m_pItemButtons[1] = new ColorButton( "", 138, 73, 99, 99, false, false );
	guess_m_pItemButtons[1]->setParent( guess_m_pInventory );
	guess_m_pItemButtons[1]->addActionSignal( new CRECBInventoryHandler_Command( this, 2 ) );

	guess_m_pItemButtons[2] = new ColorButton( "", 240, 73, 99, 99, false, false );
	guess_m_pItemButtons[2]->setParent( guess_m_pInventory );
	guess_m_pItemButtons[2]->addActionSignal( new CRECBInventoryHandler_Command( this, 3 ) );

	guess_m_pItemButtons[3] = new ColorButton( "", 36, 232, 99, 99, false, false );
	guess_m_pItemButtons[3]->setParent( guess_m_pInventory );
	guess_m_pItemButtons[3]->addActionSignal( new CRECBInventoryHandler_Command( this, 4 ) );

	guess_m_pItemButtons[4] = new ColorButton( "", 138, 232, 99, 99, false, false );
	guess_m_pItemButtons[4]->setParent( guess_m_pInventory );
	guess_m_pItemButtons[4]->addActionSignal( new CRECBInventoryHandler_Command( this, 5 ) );

	guess_m_pItemButtons[5] = new ColorButton( "", 240, 232, 99, 99, false, false );
	guess_m_pItemButtons[5]->setParent( guess_m_pInventory );
	guess_m_pItemButtons[5]->addActionSignal( new CRECBInventoryHandler_Command( this, 6 ) );

	guess_m_pOptionButtons[0] = new ColorButton( "", 373, 226, 363, 47, false, false );
	guess_m_pOptionButtons[0]->setParent( guess_m_pInventory );
	guess_m_pOptionButtons[0]->addActionSignal( new CRECBInventoryHandler_Command( this, 7 ) );
	guess_m_pOptionButtons[0]->setVisible( false );

	guess_m_pOptionButtons[1] = new ColorButton( "", 373, 276, 363, 47, false, false );
	guess_m_pOptionButtons[1]->setParent( guess_m_pInventory );
	guess_m_pOptionButtons[1]->addActionSignal( new CRECBInventoryHandler_Command( this, 8 ) );
	guess_m_pOptionButtons[1]->setVisible( false );

	guess_m_pOptionButtons[2] = new ColorButton( "", 373, 325, 363, 47, false, false );
	guess_m_pOptionButtons[2]->setParent( guess_m_pInventory );
	guess_m_pOptionButtons[2]->addActionSignal( new CRECBInventoryHandler_Command( this, 10 ) );
	guess_m_pOptionButtons[2]->setVisible( false );

	guess_m_pOptionButtons[3] = new ColorButton( "", 373, 376, 363, 47, false, false );
	guess_m_pOptionButtons[3]->setParent( guess_m_pInventory );
	guess_m_pOptionButtons[3]->addActionSignal( new CRECBInventoryHandler_Command( this, 15 ) );
	guess_m_pOptionButtons[3]->setVisible( false );

	guess_m_pOptionButtons[4] = new ColorButton( "", 373, 226, 363, 97, false, false );
	guess_m_pOptionButtons[4]->setParent( guess_m_pInventory );
	guess_m_pOptionButtons[4]->addActionSignal( new CRECBInventoryHandler_Command( this, 16 ) );

	for ( i = 0; i < 6; i++ )
	{
		guess_m_pItemButtons[i]->setUnArmedBorderColor( 0, 0, 0, 255 );
		guess_m_pItemButtons[i]->setArmedBorderColor( 255, 255, 255, 128 );
		guess_m_pItemButtons[i]->setUnArmedColor( 0, 0, 0, 255 );
		guess_m_pItemButtons[i]->setArmedColor( 0, 0, 0, 255 );
	}

	for ( i = 0; i < 5; i++ )
	{
		guess_m_pOptionButtons[i]->setUnArmedBorderColor( 0, 0, 0, 255 );
		guess_m_pOptionButtons[i]->setArmedBorderColor( 255, 255, 255, 128 );
		guess_m_pOptionButtons[i]->setUnArmedColor( 0, 0, 0, 255 );
		guess_m_pOptionButtons[i]->setArmedColor( 0, 0, 0, 255 );
	}

	guess_m_pQuickSlot1Button = new ColorButton( "", 403, 73, 99, 99, false, false );
	guess_m_pQuickSlot1Button->setParent( guess_m_pInventory );
	guess_m_pQuickSlot1Button->addActionSignal( new CRECBInventoryHandler_Command( this, 12 ) );
	guess_m_pQuickSlot1Button->setUnArmedBorderColor( 0, 0, 0, 255 );
	guess_m_pQuickSlot1Button->setArmedBorderColor( 255, 255, 255, 255 );
	guess_m_pQuickSlot1Button->setUnArmedColor( 0, 0, 0, 255 );
	guess_m_pQuickSlot1Button->setArmedColor( 0, 0, 0, 255 );

	guess_m_pQuickSlot2Button = new ColorButton( "", 505, 73, 99, 99, false, false );
	guess_m_pQuickSlot2Button->setParent( guess_m_pInventory );
	guess_m_pQuickSlot2Button->addActionSignal( new CRECBInventoryHandler_Command( this, 13 ) );
	guess_m_pQuickSlot2Button->setUnArmedBorderColor( 0, 0, 0, 255 );
	guess_m_pQuickSlot2Button->setArmedBorderColor( 255, 255, 255, 255 );
	guess_m_pQuickSlot2Button->setUnArmedColor( 0, 0, 0, 255 );
	guess_m_pQuickSlot2Button->setArmedColor( 0, 0, 0, 255 );

	guess_m_pQuickSlot3Button = new ColorButton( "", 607, 73, 99, 99, false, false );
	guess_m_pQuickSlot3Button->setParent( guess_m_pInventory );
	guess_m_pQuickSlot3Button->addActionSignal( new CRECBInventoryHandler_Command( this, 14 ) );
	guess_m_pQuickSlot3Button->setUnArmedBorderColor( 0, 0, 0, 255 );
	guess_m_pQuickSlot3Button->setArmedBorderColor( 255, 255, 255, 255 );
	guess_m_pQuickSlot3Button->setUnArmedColor( 0, 0, 0, 255 );
	guess_m_pQuickSlot3Button->setArmedColor( 0, 0, 0, 255 );

	guess_m_pPopupImage = new CImageLabel( NULL, 0, 0 );
	guess_m_pPopupImage->setVisible( false );
	guess_m_pPopupImage->setParent( this );
	guess_m_pPopupImage->setImage( NULL );

	guess_m_pMenuImage = new CImageLabel( NULL, -5, -5 );
	guess_m_pMenuImage->setVisible( false );
	guess_m_pMenuImage->setParent( guess_m_pInventory );
	guess_m_pMenuImage->setImage( vgui_LoadTGA( "gfx/vgui/inventory_weapons.tga" ) );

	guess_m_pMenuButtons[0] = new ColorButton( "", 0, 0, 126, 42, false, false );
	guess_m_pMenuButtons[0]->setParent( guess_m_pInventory );
	guess_m_pMenuButtons[0]->addActionSignal( new CRECBInventoryHandler_Command( this, 50 ) );
	guess_m_pMenuButtons[0]->setVisible( false );
	guess_m_pMenuButtons[0]->setContentAlignment( Label::a_center );

	guess_m_pMenuButtons[1] = new ColorButton( "", 0, 42, 126, 42, false, false );
	guess_m_pMenuButtons[1]->setParent( guess_m_pInventory );
	guess_m_pMenuButtons[1]->addActionSignal( new CRECBInventoryHandler_Command( this, 51 ) );
	guess_m_pMenuButtons[1]->setVisible( false );
	guess_m_pMenuButtons[1]->setContentAlignment( Label::a_center );

	guess_m_pMenuButtons[2] = new ColorButton( "", 0, 84, 126, 42, false, false );
	guess_m_pMenuButtons[2]->setParent( guess_m_pInventory );
	guess_m_pMenuButtons[2]->addActionSignal( new CRECBInventoryHandler_Command( this, 52 ) );
	guess_m_pMenuButtons[2]->setVisible( false );
	guess_m_pMenuButtons[2]->setContentAlignment( Label::a_center );

	guess_m_pMenuButtons[3] = new ColorButton( "", 0, 126, 126, 42, false, false );
	guess_m_pMenuButtons[3]->setParent( guess_m_pInventory );
	guess_m_pMenuButtons[3]->addActionSignal( new CRECBInventoryHandler_Command( this, 53 ) );
	guess_m_pMenuButtons[3]->setVisible( false );
	guess_m_pMenuButtons[3]->setContentAlignment( Label::a_center );

	guess_m_pMenuButtons[4] = new ColorButton( "", 0, 126, 42, 42, false, false );
	guess_m_pMenuButtons[4]->setParent( guess_m_pInventory );
	guess_m_pMenuButtons[4]->addActionSignal( new CRECBInventoryHandler_Command( this, 54 ) );
	guess_m_pMenuButtons[4]->setVisible( false );
	guess_m_pMenuButtons[4]->setContentAlignment( Label::a_center );

	guess_m_pMenuButtons[5] = new ColorButton( "", 42, 126, 42, 42, false, false );
	guess_m_pMenuButtons[5]->setParent( guess_m_pInventory );
	guess_m_pMenuButtons[5]->addActionSignal( new CRECBInventoryHandler_Command( this, 55 ) );
	guess_m_pMenuButtons[5]->setVisible( false );
	guess_m_pMenuButtons[5]->setContentAlignment( Label::a_center );

	guess_m_pMenuButtons[6] = new ColorButton( "", 84, 126, 42, 42, false, false );
	guess_m_pMenuButtons[6]->setParent( guess_m_pInventory );
	guess_m_pMenuButtons[6]->addActionSignal( new CRECBInventoryHandler_Command( this, 56 ) );
	guess_m_pMenuButtons[6]->setVisible( false );
	guess_m_pMenuButtons[6]->setContentAlignment( Label::a_center );

	for ( i = 0; i < 7; i++ )
	{
		guess_m_pMenuButtons[i]->setUnArmedBorderColor( 255, 255, 255, 128 );
		guess_m_pMenuButtons[i]->setArmedBorderColor( 255, 255, 255, 0 );
		guess_m_pMenuButtons[i]->setUnArmedColor( 255, 255, 255, 128 );
		guess_m_pMenuButtons[i]->setArmedColor( 255, 255, 255, 0 );
	}

	guess_m_pSelection = new Panel( 36, 73, 99, 99 );
	guess_m_pSelection->setParent( guess_m_pInventory );
	guess_m_pSelection->setBgColor( 0, 0, 0, 255 );
	guess_m_pSelection->setVisible( false );
	guess_m_pSelection->setBorder( new LineBorder( Color( 255, 0, 0, 255 ) ) );
	guess_m_pSelection->setPaintBorderEnabled( true );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hInventoryScheme = pSchemes->getSchemeHandle( "inventory" );
	Font *pInventoryFont = pSchemes->getFont( hInventoryScheme );

	guess_m_pObjectiveLabel = new Label( "", 404, 354, 301, 97 );
	guess_m_pObjectiveLabel->setParent( guess_m_pInventory );
	guess_m_pObjectiveLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pObjectiveLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pObjectiveLabel->setFont( pInventoryFont );
	guess_m_pObjectiveLabel->setContentAlignment( Label::a_center );
	guess_m_pObjectiveLabel->setVisible( true );
}

void CRECBInventory::ActionSignal( int cmd )
{
	char szCommand[256];
	char szCmd[32];
	int i;

	const char *pszLevel = gEngfuncs.pfnGetLevelName();

	switch ( cmd )
	{
	case 0:
		break;
	case 1:
		guess_SelectItem( 1 );
		break;
	case 2:
		guess_SelectItem( 2 );
		break;
	case 3:
		guess_SelectItem( 3 );
		break;
	case 4:
		guess_SelectItem( 4 );
		break;
	case 5:
		guess_SelectItem( 5 );
		break;
	case 6:
		guess_SelectItem( 6 );
		break;
	case 7:
		guess_SetAction( 1 );
		break;
	case 8:
		guess_SetAction( 2 );
		break;
	case 9:
		guess_SetAction( 3 );
		break;
	case 10:
		guess_SetAction( 4 );
		break;
	case 11:
		gViewPort->HideTopMenu();
		break;
	case 12:
		break;
	case 13:
		break;
	case 14:
		break;
	case 15:
		if ( !strstr( pszLevel, "cof_" ) )
			guess_SetAction( 7 );
		break;
	case 16:
		if ( !strstr( pszLevel, "cof_" ) && gViewPort && gViewPort->guess_m_pDocuments )
		{
			gViewPort->HideTopMenu();
			gViewPort->guess_m_pDocuments->setVisible( true );
			gViewPort->UpdateCursorState();
		}
		break;
	case 50:
		sprintf( szCmd, "inventoryequip %i\n", guess_m_iSelected );
		gEngfuncs.pfnServerCmd( szCmd );
		guess_m_pMenuImage->setVisible( false );
		for ( i = 0; i < 7; i++ )
			guess_m_pMenuButtons[i]->setVisible( false );
		break;
	case 51:
		{
			int x, y;

			guess_m_iCombine = guess_m_iSelected;
			guess_m_pItemButtons[guess_m_iSelected - 1]->getPos( x, y );
			guess_m_iSelectionAlpha = 255;
			guess_g_bInventoryFadeOut = true;
			guess_m_pMenuImage->setVisible( false );
			guess_m_pSelection->setPos( x, y );
			guess_m_pSelection->setVisible( true );
			for ( i = 0; i < 7; i++ )
				guess_m_pMenuButtons[i]->setVisible( false );
		}
		break;
	case 52:
		sprintf( szCmd, "inventorydrop %i\n", guess_m_iSelected );
		gEngfuncs.pfnServerCmd( szCmd );
		guess_m_pMenuImage->setVisible( false );
		for ( i = 0; i < 7; i++ )
			guess_m_pMenuButtons[i]->setVisible( false );
		break;
	case 53:
		{
			int x, y;

			guess_m_iDualWield = guess_m_iSelected;
			guess_m_pItemButtons[guess_m_iSelected - 1]->getPos( x, y );
			guess_m_iSelectionAlpha = 255;
			guess_g_bInventoryFadeOut = true;
			guess_m_pSelection->setPos( x, y );
			guess_m_pSelection->setVisible( true );
			guess_m_pMenuImage->setVisible( false );
			for ( i = 0; i < 7; i++ )
				guess_m_pMenuButtons[i]->setVisible( false );
		}
		break;
	case 54:
		if ( strlen( gHUD.guess_GetInventoryItem( guess_m_iSelected ) ) && strstr( gHUD.guess_GetInventoryItem( guess_m_iSelected ), "weapon_" ) )
		{
			guess_m_pItemImages[6]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( guess_m_iSelected ) ) ) );
			strncpy( gHUD.guess_m_szInventoryWeapon1, gHUD.guess_GetInventoryItem( guess_m_iSelected ), 128 );
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			sprintf( szCommand, "quickselset1 %i\n", guess_m_iSelected );
			gEngfuncs.pfnServerCmd( szCommand );
			guess_m_pMenuImage->setVisible( false );
			for ( i = 0; i < 7; i++ )
				guess_m_pMenuButtons[i]->setVisible( false );
		}
		else
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );

		guess_m_iAction = 0;
		break;
	case 55:
		if ( strlen( gHUD.guess_GetInventoryItem( guess_m_iSelected ) ) && strstr( gHUD.guess_GetInventoryItem( guess_m_iSelected ), "weapon_" ) )
		{
			guess_m_pItemImages[7]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( guess_m_iSelected ) ) ) );
			strncpy( gHUD.guess_m_szInventoryWeapon2, gHUD.guess_GetInventoryItem( guess_m_iSelected ), 128 );
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			sprintf( szCommand, "quickselset2 %i\n", guess_m_iSelected );
			gEngfuncs.pfnServerCmd( szCommand );
			guess_m_pMenuImage->setVisible( false );
			for ( i = 0; i < 7; i++ )
				guess_m_pMenuButtons[i]->setVisible( false );
		}
		else
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );

		guess_m_iAction = 0;
		break;
	case 56:
		if ( strlen( gHUD.guess_GetInventoryItem( guess_m_iSelected ) ) && strstr( gHUD.guess_GetInventoryItem( guess_m_iSelected ), "weapon_" ) )
		{
			guess_m_pItemImages[9]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( guess_m_iSelected ) ) ) );
			strncpy( gHUD.guess_m_szInventoryWeapon3, gHUD.guess_GetInventoryItem( guess_m_iSelected ), 128 );
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			sprintf( szCommand, "quickselset3 %i\n", guess_m_iSelected );
			gEngfuncs.pfnServerCmd( szCommand );
			guess_m_pMenuImage->setVisible( false );
			for ( i = 0; i < 7; i++ )
				guess_m_pMenuButtons[i]->setVisible( false );
		}
		else
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );

		guess_m_iAction = 0;
		break;
	default:
		gEngfuncs.Con_DPrintf( "Unknown Inventory ActionSignal %i.\n", cmd );
		break;
	}
}

void CRECBInventory::guess_Reset()
{
	for ( int i = 0; i < 10; i++ )
		guess_m_pItemImages[i]->setImage( NULL );

	guess_m_iAction = 0;
	guess_m_iActionItem2 = 0;
	guess_m_iActionItem1 = 0;
	guess_m_iHoverItem = 0;
}

void CRECBInventory::guess_DoAction( int slot )
{
	char szCommand[256];
	char szCmd[32];

	if ( guess_m_iAction <= 0 )
		return;

	const char *pszLevel = gEngfuncs.pfnGetLevelName();

	switch ( guess_m_iAction )
	{
	case 1:
		sprintf( szCmd, "inventoryequip %i\n", slot );
		gEngfuncs.pfnServerCmd( szCmd );
		break;
	case 2:
		if ( !guess_m_iActionItem1 || !guess_m_iActionItem2 )
			return;

		if ( guess_m_iActionItem1 == guess_m_iActionItem2 )
		{
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
		}
		else
		{
			if ( strlen( gHUD.CheckItem( "combined_result", gHUD.guess_GetInventoryItem( guess_m_iActionItem1 ) ) ) <= 1 )
				break;

			sprintf( szCommand, "inventorycombine %i %i %s\n", guess_m_iActionItem1, guess_m_iActionItem2, gHUD.CheckItem( "combined_result", gHUD.guess_GetInventoryItem( guess_m_iActionItem1 ) ) );
			gEngfuncs.pfnServerCmd( szCommand );
		}

		guess_m_iActionItem1 = 0;
		guess_m_iActionItem2 = 0;
		break;
	case 3:
		sprintf( szCmd, "inventoryuse %i\n", slot );
		gEngfuncs.pfnServerCmd( szCmd );
		break;
	case 4:
		sprintf( szCmd, "inventorydrop %i\n", slot );
		gEngfuncs.pfnServerCmd( szCmd );
		break;
	case 5:
		if ( strlen( gHUD.guess_GetInventoryItem( slot ) ) && strstr( gHUD.guess_GetInventoryItem( slot ), "weapon_" ) )
		{
			guess_m_pItemImages[6]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( slot ) ) ) );
			strncpy( gHUD.guess_m_szInventoryWeapon1, gHUD.guess_GetInventoryItem( slot ), 128 );
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			sprintf( szCmd, "quickselset1 %i\n", slot );
			gEngfuncs.pfnServerCmd( szCmd );
		}
		else
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
		break;
	case 6:
		if ( strlen( gHUD.guess_GetInventoryItem( slot ) ) && strstr( gHUD.guess_GetInventoryItem( slot ), "weapon_" ) )
		{
			guess_m_pItemImages[7]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( slot ) ) ) );
			strncpy( gHUD.guess_m_szInventoryWeapon2, gHUD.guess_GetInventoryItem( slot ), 128 );
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			sprintf( szCmd, "quickselset2 %i\n", slot );
			gEngfuncs.pfnServerCmd( szCmd );
		}
		else
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
		break;
	case 7:
		if ( !strstr( pszLevel, "cof_" ) && guess_m_iActionItem1 && guess_m_iActionItem2 )
		{
			if ( guess_m_iActionItem1 == guess_m_iActionItem2 )
			{
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
				guess_m_iActionItem2 = 0;
				guess_m_iActionItem1 = 0;
				return;
			}

			if ( strlen( gHUD.CheckItem( "classname", gHUD.guess_GetInventoryItem( guess_m_iActionItem1 ) ) ) > 1 )
			{
				sprintf( szCommand, "inventorydualwield %i %i\n", guess_m_iActionItem1, guess_m_iActionItem2 );
				gEngfuncs.pfnServerCmd( szCommand );
				guess_m_iActionItem2 = 0;
				guess_m_iActionItem1 = 0;
				return;
			}
		}
		return;
	case 8:
		if ( strlen( gHUD.guess_GetInventoryItem( slot ) ) && strstr( gHUD.guess_GetInventoryItem( slot ), "weapon_" ) )
		{
			guess_m_pItemImages[9]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( slot ) ) ) );
			strncpy( gHUD.guess_m_szInventoryWeapon3, gHUD.guess_GetInventoryItem( slot ), 128 );
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			sprintf( szCmd, "quickselset3 %i\n", slot );
			gEngfuncs.pfnServerCmd( szCmd );
		}
		else
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
		break;
	default:
		return;
	}

	guess_m_iAction = 0;
}

void CRECBInventory::Initialize()
{
	if ( gViewPort && gViewPort->guess_m_pRECBInventory )
		gViewPort->guess_m_pRECBInventory->guess_Update();

	guess_m_pItemImages[6]->setImage( NULL );
	gHUD.guess_m_szInventoryWeapon1[0] = 0;
	if ( strlen( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon1 ) ) > 2 )
		guess_m_pItemImages[6]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon1 ) ) );

	guess_m_pItemImages[7]->setImage( NULL );
	gHUD.guess_m_szInventoryWeapon2[0] = 0;
	if ( strlen( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon2 ) ) > 2 )
		guess_m_pItemImages[7]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon2 ) ) );

	guess_m_pItemImages[9]->setImage( NULL );
	gHUD.guess_m_szInventoryWeapon3[0] = 0;
	if ( strlen( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon3 ) ) > 2 )
		guess_m_pItemImages[9]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon3 ) ) );
}

int CRECBInventory::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding )
	{
		if ( !strcmp( pszCurrentBinding, "toggleconsole" ) )
			return 1;

		if ( !strcmp( pszCurrentBinding, "+inventory" ) && isVisible() )
		{
			gViewPort->HideTopMenu();
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_close.wav", 1 );
		}
	}

	return 0;
}

bool CRECBInventory::guess_IsCursorOver( int x, int y, int wide, int tall )
{
	int cx, cy;

	if ( !gViewPort )
		return false;

	getApp()->getCursorPos( cx, cy );

	if ( cx >= x && cx <= x + wide && cy >= y && cy <= y + wide )
		return true;

	return false;
}

int guess_MsgInventChck( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort && gViewPort->guess_m_pRECBInventory )
		gViewPort->guess_m_pRECBInventory->guess_Update();

	return 1;
}

int guess_MsgQSlotClear( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iSlot = READ_BYTE();
	int iOn = READ_BYTE();

	if ( gViewPort->guess_m_pRECBInventory )
	{
		switch ( iSlot )
		{
		case 1:
			if ( !iOn )
			{
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[6]->setImage( NULL );
				gHUD.guess_m_szInventoryWeapon1[0] = 0;
			}
			else
			{
				if ( strlen( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon1 ) ) > 2 )
					gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[6]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon1 ) ) );
			}
			break;
		case 2:
			if ( !iOn )
			{
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[7]->setImage( NULL );
				gHUD.guess_m_szInventoryWeapon2[0] = 0;
			}
			else
			{
				if ( strlen( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon2 ) ) > 2 )
					gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[7]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon2 ) ) );
			}
			break;
		case 3:
			if ( !iOn )
			{
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[9]->setImage( NULL );
				gHUD.guess_m_szInventoryWeapon3[0] = 0;
			}
			else
			{
				if ( strlen( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon3 ) ) > 2 )
					gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[9]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_m_szInventoryWeapon3 ) ) );
			}
			break;
		}
	}

	return 1;
}

int guess_MsgQSlotUpdate( const char *pszName, int iSize, void *pbuf )
{
	char szCommand[128];

	BEGIN_READ( pbuf, iSize );
	int iSlot = READ_BYTE();
	int iItem = READ_BYTE();

	if ( gViewPort->guess_m_pRECBInventory )
	{
		switch ( iSlot )
		{
		case 1:
			if ( strlen( gHUD.guess_GetInventoryItem( iItem ) ) && strstr( gHUD.guess_GetInventoryItem( iItem ), "weapon_" ) )
			{
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[6]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( iItem ) ) ) );
				strncpy( gHUD.guess_m_szInventoryWeapon1, gHUD.guess_GetInventoryItem( iItem ), 128 );
				sprintf( szCommand, "quickselset1 %i\n", iItem );
				gEngfuncs.pfnServerCmd( szCommand );
			}
			break;
		case 2:
			if ( strlen( gHUD.guess_GetInventoryItem( iItem ) ) && strstr( gHUD.guess_GetInventoryItem( iItem ), "weapon_" ) )
			{
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[7]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( iItem ) ) ) );
				strncpy( gHUD.guess_m_szInventoryWeapon2, gHUD.guess_GetInventoryItem( iItem ), 128 );
				sprintf( szCommand, "quickselset2 %i\n", iItem );
				gEngfuncs.pfnServerCmd( szCommand );
			}
			break;
		case 3:
			if ( strlen( gHUD.guess_GetInventoryItem( iItem ) ) && strstr( gHUD.guess_GetInventoryItem( iItem ), "weapon_" ) )
			{
				gViewPort->guess_m_pRECBInventory->guess_m_pItemImages[9]->setImage( vgui_LoadTGA( gHUD.CheckItem( "inventory_picture", gHUD.guess_GetInventoryItem( iItem ) ) ) );
				strncpy( gHUD.guess_m_szInventoryWeapon3, gHUD.guess_GetInventoryItem( iItem ), 128 );
				sprintf( szCommand, "quickselset3 %i\n", iItem );
				gEngfuncs.pfnServerCmd( szCommand );
			}
			break;
		}
	}

	return 1;
}

void CRECBInventory::guess_SelectItem( int slot )
{
	char szCommand[256];
	char szCmd[32];

	if ( guess_m_iDualWield )
	{
		if ( guess_m_iDualWield != slot )
		{
			if ( strlen( gHUD.CheckItem( "classname", gHUD.guess_GetInventoryItem( slot ) ) ) > 1 )
			{
				sprintf( szCmd, "inventorydualwield %i %i\n", guess_m_iDualWield, slot );
				gEngfuncs.pfnServerCmd( szCmd );
				guess_m_iDualWield = 0;
				guess_g_bInventoryFadeOut = false;
				guess_m_pSelection->setVisible( false );
				return;
			}

			guess_m_iDualWield = 0;
			guess_g_bInventoryFadeOut = false;
			guess_m_pSelection->setVisible( false );
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
			return;
		}
	}
	else
	{
		if ( guess_m_iCombine )
		{
			if ( guess_m_iCombine != slot )
			{
				if ( strlen( gHUD.CheckItem( "combined_result", gHUD.guess_GetInventoryItem( slot ) ) ) > 1 )
				{
					sprintf( szCommand, "inventorycombine %i %i %s\n", guess_m_iCombine, slot, gHUD.CheckItem( "combined_result", gHUD.guess_GetInventoryItem( guess_m_iCombine ) ) );
					gEngfuncs.pfnServerCmd( szCommand );
					guess_m_iDualWield = 0;
					guess_g_bInventoryFadeOut = false;
					guess_m_pSelection->setVisible( false );
					return;
				}

				guess_m_iCombine = 0;
				guess_g_bInventoryFadeOut = false;
				guess_m_pSelection->setVisible( false );
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
				return;
			}
		}
		else
		{
			if ( strlen( gHUD.guess_GetInventoryItem( slot ) ) > 1 )
			{
				int x, y, wide, tall;
				bool bItem = false;

				guess_m_iSelected = slot;
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_toggle.wav", 1 );

				if ( !strstr( gHUD.guess_GetInventoryItem( guess_m_iSelected ), "weapon_" ) )
					bItem = true;

				guess_m_pItemButtons[guess_m_iSelected - 1]->getPos( x, y );
				guess_m_pItemButtons[guess_m_iSelected - 1]->getSize( wide, tall );
				x += wide;
				guess_m_pMenuButtons[0]->getSize( wide, tall );

				guess_m_pMenuImage->setVisible( true );
				guess_m_pMenuImage->setPos( x - 5, y - 5 );

				if ( bItem )
					guess_m_pMenuImage->setImage( vgui_LoadTGA( "gfx/vgui/inventory_items.tga" ) );
				else
					guess_m_pMenuImage->setImage( vgui_LoadTGA( "gfx/vgui/inventory_weapons.tga" ) );

				for ( int i = 1; i <= 7; i++ )
				{
					guess_m_pMenuButtons[i - 1]->setPos( x, y );

					if ( i >= 4 )
					{
						if ( !bItem )
							guess_m_pMenuButtons[i - 1]->setVisible( true );
						else
							guess_m_pMenuButtons[i - 1]->setVisible( false );
					}
					else
					{
						guess_m_pMenuButtons[i - 1]->setVisible( true );
					}

					if ( i <= 4 )
						y += tall;

					if ( i >= 5 )
						x += 42;
				}
			}
			return;
		}
	}

	gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
}

void CRECBInventory::guess_SetAction( int action )
{
	gEngfuncs.pfnPlaySoundByName( "inventory/inventory_toggle.wav", 1 );

	guess_m_iAction = action;
	guess_m_iActionItem2 = 0;
	guess_m_iActionItem1 = 0;
}

void CRECBInventory::guess_Update()
{
	const char *pszLevel = gEngfuncs.pfnGetLevelName();

	if ( strstr( pszLevel, "cof_" ) )
	{
		guess_m_pBackground->setVisible( true );
		guess_m_pInventory->setImage( vgui_LoadTGA( "gfx/vgui/640_inventory_p.tga" ) );

		guess_m_pItemButtons[0]->setPos( 36, 91 );
		guess_m_pItemImages[0]->setPos( 31, 86 );
		guess_m_pItemButtons[1]->setPos( 138, 91 );
		guess_m_pItemImages[1]->setPos( 133, 86 );
		guess_m_pItemButtons[2]->setPos( 240, 91 );
		guess_m_pItemImages[2]->setPos( 235, 86 );
		guess_m_pItemButtons[3]->setPos( 36, 213 );
		guess_m_pItemImages[3]->setPos( 31, 208 );
		guess_m_pItemButtons[4]->setPos( 138, 213 );
		guess_m_pItemImages[4]->setPos( 133, 208 );
		guess_m_pItemButtons[5]->setPos( 240, 213 );
		guess_m_pItemImages[5]->setPos( 235, 208 );
		guess_m_pItemImages[8]->setPos( 31, 329 );
		guess_m_pQuickSlot1Button->setPos( 403, 90 );
		guess_m_pQuickSlot2Button->setPos( 505, 90 );
		guess_m_pQuickSlot3Button->setPos( 607, 90 );
		guess_m_pItemImages[6]->setPos( 398, 85 );
		guess_m_pItemImages[7]->setPos( 500, 85 );
		guess_m_pItemImages[9]->setPos( 602, 85 );

		guess_m_pOptionButtons[0]->setSize( 363, 42 );
		guess_m_pOptionButtons[0]->setPos( 373, 243 );
		guess_m_pOptionButtons[1]->setSize( 363, 42 );
		guess_m_pOptionButtons[1]->setPos( 373, 285 );
		guess_m_pOptionButtons[2]->setSize( 363, 42 );
		guess_m_pOptionButtons[2]->setPos( 373, 327 );
		guess_m_pOptionButtons[3]->setVisible( false );
		guess_m_pOptionButtons[4]->setVisible( false );

		setBgColor( 255, 0, 0, 255 );
	}
	else
	{
		guess_m_pBackground->setVisible( true );

		if ( guess_m_iThree )
			guess_m_pInventory->setImage( vgui_LoadTGA( "gfx/vgui/640_inventory_three.tga" ) );
		else
			guess_m_pInventory->setImage( vgui_LoadTGA( "gfx/vgui/640_inventory.tga" ) );


		guess_m_pItemButtons[0]->setPos( 36, 73 );
		guess_m_pItemImages[0]->setPos( 31, 68 );
		guess_m_pItemButtons[1]->setPos( 138, 73 );
		guess_m_pItemImages[1]->setPos( 133, 68 );
		guess_m_pItemButtons[2]->setPos( 240, 73 );
		guess_m_pItemImages[2]->setPos( 235, 68 );
		guess_m_pItemButtons[3]->setPos( 36, 232 );
		guess_m_pItemImages[3]->setPos( 31, 227 );
		guess_m_pItemButtons[4]->setPos( 138, 232 );
		guess_m_pItemImages[4]->setPos( 133, 227 );
		guess_m_pItemButtons[5]->setPos( 240, 232 );
		guess_m_pItemImages[5]->setPos( 235, 227 );
		guess_m_pItemImages[8]->setPos( 31, 348 );
		guess_m_pQuickSlot1Button->setPos( 403, 73 );
		guess_m_pQuickSlot2Button->setPos( 505, 73 );
		guess_m_pQuickSlot3Button->setPos( 607, 73 );
		guess_m_pItemImages[6]->setPos( 398, 68 );
		guess_m_pItemImages[7]->setPos( 500, 68 );
		guess_m_pItemImages[9]->setPos( 602, 68 );

		guess_m_pOptionButtons[0]->setSize( 363, 47 );
		guess_m_pOptionButtons[0]->setPos( 373, 226 );
		guess_m_pOptionButtons[1]->setSize( 363, 47 );
		guess_m_pOptionButtons[1]->setPos( 373, 276 );
		guess_m_pOptionButtons[2]->setSize( 363, 47 );
		guess_m_pOptionButtons[2]->setPos( 373, 325 );
		guess_m_pOptionButtons[3]->setVisible( false );
		guess_m_pOptionButtons[4]->setVisible( true );

		setBgColor( 0, 0, 0, 255 );
	}

	guess_m_pBackgroundPanel->setBgColor( 0, 0, 0, 0 );
}

void guess_QSlotInit()
{
	gEngfuncs.pfnHookUserMsg( "QSlotClear", guess_MsgQSlotClear );
	gEngfuncs.pfnHookUserMsg( "QSlotUpdate", guess_MsgQSlotUpdate );
	gEngfuncs.pfnHookUserMsg( "InventChck", guess_MsgInventChck );
}

void CRECBInventory::setVisible( bool state )
{
	int i;

	for ( i = 0; i < 7; i++ )
		guess_m_pMenuButtons[i]->setVisible( false );

	guess_m_pMenuImage->setVisible( false );
	guess_m_pSelection->setVisible( false );

	guess_m_iSelectionAlpha = 0;
	guess_m_iSelected = 0;
	guess_m_iCombine = 0;
	guess_m_iDualWield = 0;
	guess_m_iAction = 0;
	guess_m_iActionItem2 = 0;
	guess_m_iActionItem1 = 0;
	guess_m_iHoverItem = 0;

	if ( state )
	{
		if ( guess_m_pItemImages[8] )
			guess_m_pItemImages[8]->setImage( NULL );

		if ( guess_m_pPopupImage )
			guess_m_pPopupImage->setImage( NULL );

		guess_m_bUnknownE0 = false;
		guess_m_iUnknownE4 = 0;

		if ( strlen( gHUD.guess_m_szObjectiveTitle ) > 2 && strlen( gHUD.guess_m_szObjectiveText ) > 2 )
		{
			ParseHintTokens( gHUD.guess_m_szObjectiveTitle, gHUD.guess_m_szObjectiveText );

			if ( strlen( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1 ) > 2 )
			{
				char szText[256];
				unsigned int j;

				for ( i = 0, j = 0; j < strlen( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1 ); j++ )
				{
					i++;

					if ( gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[j] == '@' )
						szText[j] = '\n';
					else
						szText[j] = gViewPort->guess_m_pHUDControl->guess_m_szHintLine1[j];
				}

				szText[i] = 0;
				guess_m_pObjectiveLabel->setText( szText );
			}

			guess_m_pObjectiveLabel->setVisible( true );
		}
		else
			guess_m_pObjectiveLabel->setVisible( false );
	}
	else
	{
		if ( guess_m_pPopupImage )
			guess_m_pPopupImage->setImage( NULL );

		guess_m_bUnknownE0 = false;
		guess_m_iUnknownE4 = 0;

		if ( gEngfuncs.pfnGetLevelName() )
		{
			for ( i = 0; i < 5; i++ )
			{
				if ( guess_m_pOptionButtons[i] && guess_m_pOptionButtons[i]->isVisible() )
					guess_m_pOptionButtons[i]->cursorExited();
			}

			for ( i = 0; i < 6; i++ )
			{
				if ( guess_m_pItemButtons[i] && guess_m_pItemButtons[i]->isVisible() )
					guess_m_pItemButtons[i]->cursorExited();
			}

			guess_m_pQuickSlot1Button->cursorExited();
			guess_m_pQuickSlot2Button->cursorExited();
			guess_m_pQuickSlot3Button->cursorExited();
		}

		guess_m_pItemImages[8]->setImage( NULL );
	}

	Panel::setVisible( state );
}

void CRECBInventory::solve()
{
	int i;

	if ( isVisible() )
	{
		if ( guess_m_pSelection->isVisible() )
		{
			if ( guess_g_bInventoryFadeOut )
			{
				if ( guess_m_iSelectionAlpha > 0 )
					guess_m_iSelectionAlpha -= 15;

				if ( guess_m_iSelectionAlpha <= 0 )
				{
					guess_g_bInventoryFadeOut = false;
					guess_m_iSelectionAlpha = 0;
				}
			}
			else
			{
				if ( guess_m_iSelectionAlpha < 255 )
					guess_m_iSelectionAlpha += 15;

				if ( guess_m_iSelectionAlpha >= 255 )
				{
					guess_g_bInventoryFadeOut = true;
					guess_m_iSelectionAlpha = 255;
				}
			}

			guess_m_pSelection->setBorder( new LineBorder( Color( 255, 0, 0, guess_m_iSelectionAlpha ) ) );
		}

		if ( guess_iWheelchair && isVisible() )
		{
			gViewPort->HideTopMenu();
			return;
		}

		if ( getApp()->isMouseDown( MOUSE_LEFT, NULL ) )
		{
			bool bArmed = false;

			for ( i = 0; i < 7; i++ )
			{
				if ( guess_m_pMenuButtons[i]->isArmed() )
					bArmed = true;
			}

			if ( !bArmed )
			{
				for ( i = 0; i < 7; i++ )
					guess_m_pMenuButtons[i]->setVisible( false );

				guess_m_pMenuImage->setVisible( false );
			}
		}

		if ( guess_m_pPopupImage->isVisible() )
			guess_m_pPopupImage->setVisible( false );

		for ( i = 1; i <= 6; i++ )
		{
			if ( guess_m_pItemButtons[i - 1]->isArmed() && strlen( gHUD.guess_GetInventoryItem( i ) ) > 1 )
			{
				guess_m_pItemImages[8]->setImage( vgui_LoadTGA( gHUD.CheckItem( "check_picture", gHUD.guess_GetInventoryItem( i ) ) ) );
				guess_m_iHoverItem = i;
			}
		}

		if ( guess_m_pQuickSlot1Button->isArmed() && guess_m_iHoverItem != 7 && strlen( gHUD.guess_m_szInventoryWeapon1 ) > 1 )
		{
			guess_m_iHoverItem = 7;
			guess_m_pItemImages[8]->setImage( vgui_LoadTGA( gHUD.CheckItem( "check_picture", gHUD.guess_m_szInventoryWeapon1 ) ) );
		}

		if ( guess_m_pQuickSlot2Button->isArmed() && guess_m_iHoverItem != 8 && strlen( gHUD.guess_m_szInventoryWeapon2 ) > 1 )
		{
			guess_m_iHoverItem = 8;
			guess_m_pItemImages[8]->setImage( vgui_LoadTGA( gHUD.CheckItem( "check_picture", gHUD.guess_m_szInventoryWeapon2 ) ) );
		}

		if ( guess_m_pQuickSlot3Button->isArmed() && guess_m_iHoverItem != 9 && strlen( gHUD.guess_m_szInventoryWeapon3 ) > 1 )
		{
			guess_m_iHoverItem = 9;
			guess_m_pItemImages[8]->setImage( vgui_LoadTGA( gHUD.CheckItem( "check_picture", gHUD.guess_m_szInventoryWeapon3 ) ) );
		}

		if ( guess_m_iHoverItem >= 1 && guess_m_iHoverItem <= 6 && !guess_m_pItemButtons[guess_m_iHoverItem - 1]->isArmed() )
		{
			guess_m_pItemImages[8]->setImage( NULL );
			guess_m_iHoverItem = 0;
		}

		if ( guess_m_iHoverItem == 7 && !guess_m_pQuickSlot1Button->isArmed() )
		{
			guess_m_pItemImages[8]->setImage( NULL );
			guess_m_iHoverItem = 0;
		}

		if ( guess_m_iHoverItem == 8 && !guess_m_pQuickSlot2Button->isArmed() )
		{
			guess_m_pItemImages[8]->setImage( NULL );
			guess_m_iHoverItem = 0;
		}

		if ( guess_m_iHoverItem == 9 && !guess_m_pQuickSlot3Button->isArmed() )
		{
			guess_m_pItemImages[8]->setImage( NULL );
			guess_m_iHoverItem = 0;
		}

		if ( guess_m_pQuickSlot1Button->isArmed() && guess_m_iHoverItem != 7 )
			guess_m_iHoverItem = 7;

		if ( guess_m_pQuickSlot2Button->isArmed() && guess_m_iHoverItem != 8 )
			guess_m_iHoverItem = 8;

		if ( guess_m_pQuickSlot3Button->isArmed() && guess_m_iHoverItem != 9 )
			guess_m_iHoverItem = 9;
	}

	Panel::solve();
}
