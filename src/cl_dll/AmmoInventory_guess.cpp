// ======================================
// Cry of Fear ammo inventory panel
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_AmmoInventory.h"
#include "..\game_shared\vgui_loadtga.h"

CAmmoInventory::CAmmoInventory() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	Panel *pPanel;

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

	guess_m_pInventoryImage = vgui_LoadTGA( "gfx/vgui/ammo/ammo_inventory.tga" );
	if ( guess_m_pInventoryImage )
		guess_m_pInventoryImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/ammo/ammo_inventory.tga!\n" );

	guess_m_pInventoryImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	guess_m_pInventory = new ImagePanel( guess_m_pInventoryImage );
	guess_m_pInventory->setParent( this );
	guess_m_pInventory->setPos( x, y );
	guess_m_pInventory->setVisible( true );

	guess_m_pSlotImage[1] = new CImageLabel( NULL, 31, 45 );
	guess_m_pSlotImage[1]->setVisible( true );
	guess_m_pSlotImage[1]->setParent( guess_m_pInventory );
	guess_m_pSlotImage[1]->setImage( NULL );

	guess_m_pSlotImage[2] = new CImageLabel( NULL, 133, 45 );
	guess_m_pSlotImage[2]->setVisible( true );
	guess_m_pSlotImage[2]->setParent( guess_m_pInventory );
	guess_m_pSlotImage[2]->setImage( NULL );

	guess_m_pSlotImage[3] = new CImageLabel( NULL, 235, 45 );
	guess_m_pSlotImage[3]->setVisible( true );
	guess_m_pSlotImage[3]->setParent( guess_m_pInventory );
	guess_m_pSlotImage[3]->setImage( NULL );

	guess_m_pSlotImage[4] = new CImageLabel( NULL, 31, 167 );
	guess_m_pSlotImage[4]->setVisible( true );
	guess_m_pSlotImage[4]->setParent( guess_m_pInventory );
	guess_m_pSlotImage[4]->setImage( NULL );

	guess_m_pSlotImage[5] = new CImageLabel( NULL, 133, 167 );
	guess_m_pSlotImage[5]->setVisible( true );
	guess_m_pSlotImage[5]->setParent( guess_m_pInventory );
	guess_m_pSlotImage[5]->setImage( NULL );

	guess_m_pSlotImage[6] = new CImageLabel( NULL, 235, 167 );
	guess_m_pSlotImage[6]->setVisible( true );
	guess_m_pSlotImage[6]->setParent( guess_m_pInventory );
	guess_m_pSlotImage[6]->setImage( NULL );

	guess_m_pCountLabel[1] = new Label( "", 36, 50, 99, 99 );
	guess_m_pCountLabel[1]->setVisible( true );
	guess_m_pCountLabel[1]->setParent( guess_m_pInventory );
	guess_m_pCountLabel[1]->setBgColor( 0, 0, 0, 255 );
	guess_m_pCountLabel[1]->setFgColor( 255, 255, 255, 0 );

	guess_m_pCountLabel[2] = new Label( "", 138, 50, 99, 99 );
	guess_m_pCountLabel[2]->setVisible( true );
	guess_m_pCountLabel[2]->setParent( guess_m_pInventory );
	guess_m_pCountLabel[2]->setBgColor( 0, 0, 0, 255 );
	guess_m_pCountLabel[2]->setFgColor( 255, 255, 255, 0 );

	guess_m_pCountLabel[3] = new Label( "", 240, 50, 99, 99 );
	guess_m_pCountLabel[3]->setVisible( true );
	guess_m_pCountLabel[3]->setParent( guess_m_pInventory );
	guess_m_pCountLabel[3]->setBgColor( 0, 0, 0, 255 );
	guess_m_pCountLabel[3]->setFgColor( 255, 255, 255, 0 );

	guess_m_pCountLabel[4] = new Label( "", 36, 172, 99, 99 );
	guess_m_pCountLabel[4]->setVisible( true );
	guess_m_pCountLabel[4]->setParent( guess_m_pInventory );
	guess_m_pCountLabel[4]->setBgColor( 0, 0, 0, 255 );
	guess_m_pCountLabel[4]->setFgColor( 255, 255, 255, 0 );

	guess_m_pCountLabel[5] = new Label( "", 138, 172, 99, 99 );
	guess_m_pCountLabel[5]->setVisible( true );
	guess_m_pCountLabel[5]->setParent( guess_m_pInventory );
	guess_m_pCountLabel[5]->setBgColor( 0, 0, 0, 255 );
	guess_m_pCountLabel[5]->setFgColor( 255, 255, 255, 0 );

	guess_m_pCountLabel[6] = new Label( "", 240, 172, 99, 99 );
	guess_m_pCountLabel[6]->setVisible( true );
	guess_m_pCountLabel[6]->setParent( guess_m_pInventory );
	guess_m_pCountLabel[6]->setBgColor( 0, 0, 0, 255 );
	guess_m_pCountLabel[6]->setFgColor( 255, 255, 255, 0 );

	guess_m_pSelectedImage = new CImageLabel( NULL, 31, 289 );
	guess_m_pSelectedImage->setVisible( true );
	guess_m_pSelectedImage->setParent( guess_m_pInventory );
	guess_m_pSelectedImage->setImage( NULL );

	guess_m_pButton[1] = new ColorButton( "", 36, 50, 99, 99, false, false );
	guess_m_pButton[1]->setParent( guess_m_pInventory );
	guess_m_pButton[1]->addActionSignal( new CAmmoInventoryHandler_Command( this, 1 ) );

	guess_m_pButton[2] = new ColorButton( "", 138, 50, 99, 99, false, false );
	guess_m_pButton[2]->setParent( guess_m_pInventory );
	guess_m_pButton[2]->addActionSignal( new CAmmoInventoryHandler_Command( this, 2 ) );

	guess_m_pButton[3] = new ColorButton( "", 240, 50, 99, 99, false, false );
	guess_m_pButton[3]->setParent( guess_m_pInventory );
	guess_m_pButton[3]->addActionSignal( new CAmmoInventoryHandler_Command( this, 3 ) );

	guess_m_pButton[4] = new ColorButton( "", 36, 172, 99, 99, false, false );
	guess_m_pButton[4]->setParent( guess_m_pInventory );
	guess_m_pButton[4]->addActionSignal( new CAmmoInventoryHandler_Command( this, 4 ) );

	guess_m_pButton[5] = new ColorButton( "", 138, 172, 99, 99, false, false );
	guess_m_pButton[5]->setParent( guess_m_pInventory );
	guess_m_pButton[5]->addActionSignal( new CAmmoInventoryHandler_Command( this, 5 ) );

	guess_m_pButton[6] = new ColorButton( "", 240, 172, 99, 99, false, false );
	guess_m_pButton[6]->setParent( guess_m_pInventory );
	guess_m_pButton[6]->addActionSignal( new CAmmoInventoryHandler_Command( this, 6 ) );

	guess_m_pDropButton = new ColorButton( "", 7, 415, 360, 50, false, false );
	guess_m_pDropButton->setParent( guess_m_pInventory );
	guess_m_pDropButton->addActionSignal( new CAmmoInventoryHandler_Command( this, 7 ) );

	setBgColor( 0, 0, 0, 0 );

	Initialize();
}

void CAmmoInventory::ActionSignal( int cmd )
{
	switch ( cmd )
	{
	case 1:
		if ( guess_m_iDropMode )
		{
			if ( guess_m_iAmmoType[1] )
			{
				gEngfuncs.pfnServerCmd( "dropammo 1" );
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			}
			else
			{
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
			}
		}
		break;
	case 2:
		if ( guess_m_iDropMode )
		{
			if ( guess_m_iAmmoType[2] )
			{
				gEngfuncs.pfnServerCmd( "dropammo 2" );
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			}
			else
			{
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
			}
		}
		break;
	case 3:
		if ( guess_m_iDropMode )
		{
			if ( guess_m_iAmmoType[3] )
			{
				gEngfuncs.pfnServerCmd( "dropammo 3" );
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			}
			else
			{
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
			}
		}
		break;
	case 4:
		if ( guess_m_iDropMode )
		{
			if ( guess_m_iAmmoType[4] )
			{
				gEngfuncs.pfnServerCmd( "dropammo 4" );
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			}
			else
			{
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
			}
		}
		break;
	case 5:
		if ( guess_m_iDropMode )
		{
			if ( guess_m_iAmmoType[5] )
			{
				gEngfuncs.pfnServerCmd( "dropammo 5" );
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			}
			else
			{
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
			}
		}
		break;
	case 6:
		if ( guess_m_iDropMode )
		{
			if ( guess_m_iAmmoType[6] )
			{
				gEngfuncs.pfnServerCmd( "dropammo 6" );
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_click.wav", 1 );
			}
			else
			{
				gEngfuncs.pfnPlaySoundByName( "inventory/inventory_no.wav", 1 );
			}
		}
		break;
	case 7:
		guess_m_iDropMode = 1;
		gEngfuncs.pfnPlaySoundByName( "inventory/inventory_toggle.wav", 1 );
		break;
	}
}

void CAmmoInventory::Initialize()
{
	guess_m_iDropMode = 0;
	guess_m_iSelected = 0;
	setVisible( false );

	guess_m_iAmmoType[1] = 0;
	guess_m_iAmmoType[2] = 0;
	guess_m_iAmmoType[3] = 0;
	guess_m_iAmmoType[4] = 0;
	guess_m_iAmmoType[5] = 0;
	guess_m_iAmmoType[6] = 0;
}

int CAmmoInventory::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding )
	{
		if ( !strcmp( pszCurrentBinding, "toggleconsole" ) )
			return 1;

		if ( !strcmp( pszCurrentBinding, "+inventory" ) && isVisible() )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnPlaySoundByName( "inventory/inventory_close.wav", 1 );
		}
	}

	return 0;
}

int guess_MsgAmmoInv( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pAmmoInventory )
	{
		int iSlot = READ_BYTE();
		int iType = READ_BYTE();
		int iCount = READ_BYTE();
		char szCount[4];

		itoa( iCount, szCount, 10 );

		gViewPort->guess_m_pAmmoInventory->guess_m_iAmmoType[iSlot] = iType;
		gViewPort->guess_m_pAmmoInventory->guess_m_pCountLabel[iSlot]->setText( szCount );
		gViewPort->guess_m_pAmmoInventory->guess_m_pCountLabel[iSlot]->setContentAlignment( Label::a_southwest );

		if ( iType )
		{
			switch ( iType )
			{
			case 1:
				if ( gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot] )
					gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/glock_slot.tga" ) );
				break;
			case 2:
				if ( gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot] )
					gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/m16_slot.tga" ) );
				break;
			case 3:
				if ( gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot] )
					gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/p345_slot.tga" ) );
				break;
			case 4:
				if ( gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot] )
					gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/shotgun_slot.tga" ) );
				break;
			case 5:
				if ( gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot] )
					gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/tmp_slot.tga" ) );
				break;
			}
		}
		else
		{
			if ( gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot] )
			{
				gViewPort->guess_m_pAmmoInventory->guess_m_pSlotImage[iSlot]->setImage( NULL );

				if ( gViewPort->guess_m_pAmmoInventory->guess_m_iSelected == iSlot && gViewPort->guess_m_pAmmoInventory->guess_m_pSelectedImage )
					gViewPort->guess_m_pAmmoInventory->guess_m_pSelectedImage->setImage( NULL );
			}
		}
	}

	return 1;
}

void guess_AmmoInvInit()
{
	gEngfuncs.pfnHookUserMsg( "AmmoInv", guess_MsgAmmoInv );
}

void CAmmoInventory::setVisible( bool state )
{
	int i;

	guess_m_iDropMode = 0;
	guess_m_iSelected = 0;

	if ( guess_m_pSelectedImage )
		guess_m_pSelectedImage->setImage( NULL );

	if ( gEngfuncs.pfnGetLevelName() )
	{
		for ( i = 1; i <= guess_MAX_AMMO_SLOTS; i++ )
		{
			if ( guess_m_pButton[i] && guess_m_pButton[i]->isVisible() )
				guess_m_pButton[i]->cursorExited();
		}

		if ( guess_m_pDropButton )
			guess_m_pDropButton->cursorExited();

		if ( state )
		{
			for ( i = 1; i <= guess_MAX_AMMO_SLOTS; i++ )
			{
				switch ( guess_m_iAmmoType[i] )
				{
				case 1:
					if ( guess_m_pSlotImage[i] )
						guess_m_pSlotImage[i]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/glock_slot.tga" ) );
					break;
				case 2:
					if ( guess_m_pSlotImage[i] )
						guess_m_pSlotImage[i]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/m16_slot.tga" ) );
					break;
				case 3:
					if ( guess_m_pSlotImage[i] )
						guess_m_pSlotImage[i]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/p345_slot.tga" ) );
					break;
				case 4:
					if ( guess_m_pSlotImage[i] )
						guess_m_pSlotImage[i]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/shotgun_slot.tga" ) );
					break;
				case 5:
					if ( guess_m_pSlotImage[i] )
						guess_m_pSlotImage[i]->setImage( vgui_LoadTGA( "gfx/vgui/ammo/tmp_slot.tga" ) );
					break;
				}
			}
		}
	}

	Panel::setVisible( state );
}

void CAmmoInventory::solve()
{
	if ( isVisible() )
	{
		if ( guess_m_iSelected && guess_m_pButton[guess_m_iSelected] && !guess_m_pButton[guess_m_iSelected]->isArmed() )
		{
			guess_m_iSelected = 0;
			guess_m_pSelectedImage->setImage( NULL );
		}

		for ( int i = 1; i <= guess_MAX_AMMO_SLOTS; i++ )
		{
			if ( guess_m_pButton[i] && guess_m_pButton[i]->isArmed() && guess_m_iSelected != i && guess_m_iAmmoType[i] )
			{
				guess_m_iSelected = i;

				switch ( guess_m_iAmmoType[i] )
				{
				case 1:
					guess_m_pSelectedImage->setImage( vgui_LoadTGA( "gfx/vgui/ammo/glock_check.tga" ) );
					break;
				case 2:
					guess_m_pSelectedImage->setImage( vgui_LoadTGA( "gfx/vgui/ammo/m16_check.tga" ) );
					break;
				case 3:
					guess_m_pSelectedImage->setImage( vgui_LoadTGA( "gfx/vgui/ammo/p345_check.tga" ) );
					break;
				case 4:
					guess_m_pSelectedImage->setImage( vgui_LoadTGA( "gfx/vgui/ammo/shotgun_check.tga" ) );
					break;
				case 5:
					guess_m_pSelectedImage->setImage( vgui_LoadTGA( "gfx/vgui/ammo/tmp_check.tga" ) );
					break;
				}
			}
		}
	}

	Panel::solve();
}
