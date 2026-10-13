// ======================================
// Cry of Fear unlockables menu
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_Unlockables.h"
#include "guess_GameMenu.h"
#include "guess_BookMenu.h"
#include "..\game_shared\vgui_loadtga.h"

extern int guess_iMenuMode;

CUnlockables::CUnlockables() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x, y;
	int i;

	guess_m_pMenuImage = vgui_LoadTGA( "gfx/vgui/unlockables/unlockables_menu.tga" );
	if ( guess_m_pMenuImage )
		guess_m_pMenuImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/unlockables/unlockables_menu.tga!\n" );

	guess_m_pMenuImage->getSize( wide, tall );
	x = ScreenWidth / 2 - wide / 2;
	y = ScreenHeight / 2 - tall / 2;

	guess_m_pMenu = new ImagePanel( guess_m_pMenuImage );
	guess_m_pMenu->setParent( this );
	guess_m_pMenu->setPos( x, y );
	guess_m_pMenu->setVisible( true );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hCreditsScheme = pSchemes->getSchemeHandle( "credits" );
	Font *pFont = pSchemes->getFont( hCreditsScheme );

	guess_m_pTextLabel = new Label( "", 126, 428, 549, 47 );
	guess_m_pTextLabel->setParent( guess_m_pMenu );
	guess_m_pTextLabel->setBgColor( 0, 0, 0, 255 );
	guess_m_pTextLabel->setFgColor( 255, 255, 255, 0 );
	guess_m_pTextLabel->setFont( pFont );
	guess_m_pTextLabel->setContentAlignment( Label::a_center );

	guess_m_pBackButton = new ColorButton( "", 304, 503, 192, 54, false, false );
	guess_m_pBackButton->setParent( guess_m_pMenu );
	guess_m_pBackButton->addActionSignal( new CUnlockablesHandler_Command( this, 50 ) );

	for ( i = 0; i < 21; i++ )
	{
		guess_m_pLockedImage[i] = vgui_LoadTGA( "gfx/vgui/unlockables/unlockables_locked.tga" );
		if ( guess_m_pLockedImage[i] )
			guess_m_pLockedImage[i]->setColor( Color( 255, 255, 255, 1 ) );
		else
			gEngfuncs.Con_Printf( "Cannot load gfx/vgui/unlockables/unlockables_locked.tga!\n" );
	}

	guess_m_pLockedPanel[0] = new ImagePanel( guess_m_pLockedImage[0] );
	guess_m_pLockedPanel[0]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[0]->setPos( 85, 139 );
	guess_m_pLockedPanel[0]->setVisible( true );

	guess_m_pItemButton[0] = new ColorButton( "", 85, 139, 90, 90, false, false );
	guess_m_pItemButton[0]->setParent( guess_m_pMenu );
	guess_m_pItemButton[0]->addActionSignal( new CUnlockablesHandler_Command( this, 1 ) );

	guess_m_pLockedPanel[1] = new ImagePanel( guess_m_pLockedImage[1] );
	guess_m_pLockedPanel[1]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[1]->setPos( 175, 139 );
	guess_m_pLockedPanel[1]->setVisible( true );

	guess_m_pItemButton[1] = new ColorButton( "", 175, 139, 90, 90, false, false );
	guess_m_pItemButton[1]->setParent( guess_m_pMenu );
	guess_m_pItemButton[1]->addActionSignal( new CUnlockablesHandler_Command( this, 2 ) );

	guess_m_pLockedPanel[2] = new ImagePanel( guess_m_pLockedImage[2] );
	guess_m_pLockedPanel[2]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[2]->setPos( 265, 139 );
	guess_m_pLockedPanel[2]->setVisible( true );

	guess_m_pItemButton[2] = new ColorButton( "", 265, 139, 90, 90, false, false );
	guess_m_pItemButton[2]->setParent( guess_m_pMenu );
	guess_m_pItemButton[2]->addActionSignal( new CUnlockablesHandler_Command( this, 3 ) );

	guess_m_pLockedPanel[3] = new ImagePanel( guess_m_pLockedImage[3] );
	guess_m_pLockedPanel[3]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[3]->setPos( 355, 139 );
	guess_m_pLockedPanel[3]->setVisible( true );

	guess_m_pItemButton[3] = new ColorButton( "", 355, 139, 90, 90, false, false );
	guess_m_pItemButton[3]->setParent( guess_m_pMenu );
	guess_m_pItemButton[3]->addActionSignal( new CUnlockablesHandler_Command( this, 4 ) );

	guess_m_pLockedPanel[4] = new ImagePanel( guess_m_pLockedImage[4] );
	guess_m_pLockedPanel[4]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[4]->setPos( 445, 139 );
	guess_m_pLockedPanel[4]->setVisible( true );

	guess_m_pItemButton[4] = new ColorButton( "", 445, 139, 90, 90, false, false );
	guess_m_pItemButton[4]->setParent( guess_m_pMenu );
	guess_m_pItemButton[4]->addActionSignal( new CUnlockablesHandler_Command( this, 5 ) );

	guess_m_pLockedPanel[5] = new ImagePanel( guess_m_pLockedImage[5] );
	guess_m_pLockedPanel[5]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[5]->setPos( 535, 139 );
	guess_m_pLockedPanel[5]->setVisible( true );

	guess_m_pItemButton[5] = new ColorButton( "", 535, 139, 90, 90, false, false );
	guess_m_pItemButton[5]->setParent( guess_m_pMenu );
	guess_m_pItemButton[5]->addActionSignal( new CUnlockablesHandler_Command( this, 6 ) );

	guess_m_pLockedPanel[6] = new ImagePanel( guess_m_pLockedImage[6] );
	guess_m_pLockedPanel[6]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[6]->setPos( 625, 139 );
	guess_m_pLockedPanel[6]->setVisible( true );

	guess_m_pItemButton[6] = new ColorButton( "", 625, 139, 90, 90, false, false );
	guess_m_pItemButton[6]->setParent( guess_m_pMenu );
	guess_m_pItemButton[6]->addActionSignal( new CUnlockablesHandler_Command( this, 7 ) );

	guess_m_pLockedPanel[7] = new ImagePanel( guess_m_pLockedImage[7] );
	guess_m_pLockedPanel[7]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[7]->setPos( 85, 229 );
	guess_m_pLockedPanel[7]->setVisible( true );

	guess_m_pItemButton[7] = new ColorButton( "", 85, 229, 90, 90, false, false );
	guess_m_pItemButton[7]->setParent( guess_m_pMenu );
	guess_m_pItemButton[7]->addActionSignal( new CUnlockablesHandler_Command( this, 8 ) );

	guess_m_pLockedPanel[8] = new ImagePanel( guess_m_pLockedImage[8] );
	guess_m_pLockedPanel[8]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[8]->setPos( 175, 229 );
	guess_m_pLockedPanel[8]->setVisible( true );

	guess_m_pItemButton[8] = new ColorButton( "", 175, 229, 90, 90, false, false );
	guess_m_pItemButton[8]->setParent( guess_m_pMenu );
	guess_m_pItemButton[8]->addActionSignal( new CUnlockablesHandler_Command( this, 9 ) );

	guess_m_pLockedPanel[9] = new ImagePanel( guess_m_pLockedImage[9] );
	guess_m_pLockedPanel[9]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[9]->setPos( 265, 229 );
	guess_m_pLockedPanel[9]->setVisible( true );

	guess_m_pItemButton[9] = new ColorButton( "", 265, 229, 90, 90, false, false );
	guess_m_pItemButton[9]->setParent( guess_m_pMenu );
	guess_m_pItemButton[9]->addActionSignal( new CUnlockablesHandler_Command( this, 10 ) );

	guess_m_pLockedPanel[10] = new ImagePanel( guess_m_pLockedImage[10] );
	guess_m_pLockedPanel[10]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[10]->setPos( 355, 229 );
	guess_m_pLockedPanel[10]->setVisible( true );

	guess_m_pItemButton[10] = new ColorButton( "", 355, 229, 90, 90, false, false );
	guess_m_pItemButton[10]->setParent( guess_m_pMenu );
	guess_m_pItemButton[10]->addActionSignal( new CUnlockablesHandler_Command( this, 11 ) );

	guess_m_pLockedPanel[11] = new ImagePanel( guess_m_pLockedImage[11] );
	guess_m_pLockedPanel[11]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[11]->setPos( 445, 229 );
	guess_m_pLockedPanel[11]->setVisible( true );

	guess_m_pItemButton[11] = new ColorButton( "", 445, 229, 90, 90, false, false );
	guess_m_pItemButton[11]->setParent( guess_m_pMenu );
	guess_m_pItemButton[11]->addActionSignal( new CUnlockablesHandler_Command( this, 12 ) );

	guess_m_pLockedPanel[12] = new ImagePanel( guess_m_pLockedImage[12] );
	guess_m_pLockedPanel[12]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[12]->setPos( 535, 229 );
	guess_m_pLockedPanel[12]->setVisible( true );

	guess_m_pItemButton[12] = new ColorButton( "", 535, 229, 90, 90, false, false );
	guess_m_pItemButton[12]->setParent( guess_m_pMenu );
	guess_m_pItemButton[12]->addActionSignal( new CUnlockablesHandler_Command( this, 13 ) );

	guess_m_pLockedPanel[13] = new ImagePanel( guess_m_pLockedImage[13] );
	guess_m_pLockedPanel[13]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[13]->setPos( 625, 229 );
	guess_m_pLockedPanel[13]->setVisible( true );

	guess_m_pItemButton[13] = new ColorButton( "", 625, 229, 90, 90, false, false );
	guess_m_pItemButton[13]->setParent( guess_m_pMenu );
	guess_m_pItemButton[13]->addActionSignal( new CUnlockablesHandler_Command( this, 14 ) );

	guess_m_pLockedPanel[14] = new ImagePanel( guess_m_pLockedImage[14] );
	guess_m_pLockedPanel[14]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[14]->setPos( 85, 319 );
	guess_m_pLockedPanel[14]->setVisible( true );

	guess_m_pItemButton[14] = new ColorButton( "", 85, 319, 90, 90, false, false );
	guess_m_pItemButton[14]->setParent( guess_m_pMenu );
	guess_m_pItemButton[14]->addActionSignal( new CUnlockablesHandler_Command( this, 15 ) );

	guess_m_pLockedPanel[15] = new ImagePanel( guess_m_pLockedImage[15] );
	guess_m_pLockedPanel[15]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[15]->setPos( 175, 319 );
	guess_m_pLockedPanel[15]->setVisible( true );

	guess_m_pItemButton[15] = new ColorButton( "", 175, 319, 90, 90, false, false );
	guess_m_pItemButton[15]->setParent( guess_m_pMenu );
	guess_m_pItemButton[15]->addActionSignal( new CUnlockablesHandler_Command( this, 16 ) );

	guess_m_pLockedPanel[16] = new ImagePanel( guess_m_pLockedImage[16] );
	guess_m_pLockedPanel[16]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[16]->setPos( 265, 319 );
	guess_m_pLockedPanel[16]->setVisible( true );

	guess_m_pItemButton[16] = new ColorButton( "", 265, 319, 90, 90, false, false );
	guess_m_pItemButton[16]->setParent( guess_m_pMenu );
	guess_m_pItemButton[16]->addActionSignal( new CUnlockablesHandler_Command( this, 17 ) );

	guess_m_pLockedPanel[17] = new ImagePanel( guess_m_pLockedImage[17] );
	guess_m_pLockedPanel[17]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[17]->setPos( 355, 319 );
	guess_m_pLockedPanel[17]->setVisible( true );

	guess_m_pItemButton[17] = new ColorButton( "", 355, 319, 90, 90, false, false );
	guess_m_pItemButton[17]->setParent( guess_m_pMenu );
	guess_m_pItemButton[17]->addActionSignal( new CUnlockablesHandler_Command( this, 18 ) );

	guess_m_pLockedPanel[18] = new ImagePanel( guess_m_pLockedImage[18] );
	guess_m_pLockedPanel[18]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[18]->setPos( 445, 319 );
	guess_m_pLockedPanel[18]->setVisible( true );

	guess_m_pItemButton[18] = new ColorButton( "", 445, 319, 90, 90, false, false );
	guess_m_pItemButton[18]->setParent( guess_m_pMenu );
	guess_m_pItemButton[18]->addActionSignal( new CUnlockablesHandler_Command( this, 19 ) );

	guess_m_pLockedPanel[19] = new ImagePanel( guess_m_pLockedImage[19] );
	guess_m_pLockedPanel[19]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[19]->setPos( 535, 319 );
	guess_m_pLockedPanel[19]->setVisible( true );

	guess_m_pItemButton[19] = new ColorButton( "", 535, 319, 90, 90, false, false );
	guess_m_pItemButton[19]->setParent( guess_m_pMenu );
	guess_m_pItemButton[19]->addActionSignal( new CUnlockablesHandler_Command( this, 20 ) );

	guess_m_pLockedPanel[20] = new ImagePanel( guess_m_pLockedImage[20] );
	guess_m_pLockedPanel[20]->setParent( guess_m_pMenu );
	guess_m_pLockedPanel[20]->setPos( 625, 319 );
	guess_m_pLockedPanel[20]->setVisible( true );

	guess_m_pItemButton[20] = new ColorButton( "", 625, 319, 90, 90, false, false );
	guess_m_pItemButton[20]->setParent( guess_m_pMenu );
	guess_m_pItemButton[20]->addActionSignal( new CUnlockablesHandler_Command( this, 21 ) );

	for ( i = 0; i < 21; i++ )
	{
		guess_m_pItemButton[i]->setUnArmedBorderColor( 0, 0, 0, 255 );
		guess_m_pItemButton[i]->setArmedBorderColor( 255, 0, 0, 192 );
		guess_m_pItemButton[i]->setUnArmedColor( 0, 0, 0, 255 );
		guess_m_pItemButton[i]->setArmedColor( 0, 0, 0, 255 );
	}

	setBgColor( 0, 0, 0, 0 );

	Initialize();
}

CUnlockables::~CUnlockables()
{
	for ( int i = 0; i < 21; i++ )
		delete guess_m_pLockedImage[i];

	delete guess_m_pMenuImage;
}

void CUnlockables::ActionSignal( int cmd )
{
	if ( cmd != 50 && gEngfuncs.GetClientTime() < guess_m_flNextClick )
		return;

	switch ( cmd )
	{
	case 1:
		if ( gHUD.guess_m_iUnlocks & 1 || gHUD.guess_m_iUnlocks & 2 || gHUD.guess_m_iUnlocks & 4 || gHUD.guess_m_iUnlocks & 8 || gHUD.guess_m_iUnlocks & 16 )
		{
			if ( gViewPort && gViewPort->guess_m_pBookMenu )
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gViewPort->guess_m_pBookMenu->setVisible( true );
				gViewPort->UpdateCursorState();
			}
		}
		break;
	case 4:
		if ( gHUD.guess_m_iUnlocks & 0x80000000 )
		{
			setVisible( false );
			gViewPort->UpdateCursorState();
			gEngfuncs.pfnClientCmd( "map c_doc_city" );
		}
		break;
	case 50:
		if ( guess_iMenuMode )
		{
			if ( gViewPort && gViewPort->guess_m_pGameMenu )
			{
				setVisible( false );
				gViewPort->UpdateCursorState();
				gViewPort->guess_m_pGameMenu->setVisible( true );
				gViewPort->UpdateCursorState();
				gEngfuncs.pfnPlaySoundByName( "ui/3d_click.wav", 1 );
			}
		}
		else if ( gEngfuncs.pfnGetLevelName() && strstr( gEngfuncs.pfnGetLevelName(), "unlockable" ) )
		{
			gEngfuncs.pfnClientCmd( "disconnect" );
		}
		break;
	}
}

void CUnlockables::Initialize()
{
	guess_m_iArmed = 0;
	setVisible( false );
}

int CUnlockables::guess_KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
	if ( pszCurrentBinding && !strcmp( pszCurrentBinding, "toggleconsole" ) )
		return 1;

	return 0;
}

void guess_UnlockablesInit()
{
	return;
}

void CUnlockables::setVisible( bool state )
{
	if ( state )
	{
		if ( gHUD.guess_m_iUnlocks & 1 || gHUD.guess_m_iUnlocks & 2 || gHUD.guess_m_iUnlocks & 4 || gHUD.guess_m_iUnlocks & 8 || gHUD.guess_m_iUnlocks & 16 )
			guess_m_pLockedPanel[0]->setVisible( false );
		else
			guess_m_pLockedPanel[0]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x20 )
			guess_m_pLockedPanel[1]->setVisible( false );
		else
			guess_m_pLockedPanel[1]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x400000 )
			guess_m_pLockedPanel[2]->setVisible( false );
		else
			guess_m_pLockedPanel[2]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x80000000 )
			guess_m_pLockedPanel[3]->setVisible( false );
		else
			guess_m_pLockedPanel[3]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x1000000 )
			guess_m_pLockedPanel[4]->setVisible( false );
		else
			guess_m_pLockedPanel[4]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x800000 )
			guess_m_pLockedPanel[5]->setVisible( false );
		else
			guess_m_pLockedPanel[5]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x40 )
			guess_m_pLockedPanel[6]->setVisible( false );
		else
			guess_m_pLockedPanel[6]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x80000 )
			guess_m_pLockedPanel[7]->setVisible( false );
		else
			guess_m_pLockedPanel[7]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x100000 )
			guess_m_pLockedPanel[8]->setVisible( false );
		else
			guess_m_pLockedPanel[8]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x200000 )
			guess_m_pLockedPanel[9]->setVisible( false );
		else
			guess_m_pLockedPanel[9]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x80 )
			guess_m_pLockedPanel[10]->setVisible( false );
		else
			guess_m_pLockedPanel[10]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x100 )
			guess_m_pLockedPanel[11]->setVisible( false );
		else
			guess_m_pLockedPanel[11]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x200 )
			guess_m_pLockedPanel[12]->setVisible( false );
		else
			guess_m_pLockedPanel[12]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x400 )
			guess_m_pLockedPanel[13]->setVisible( false );
		else
			guess_m_pLockedPanel[13]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x800 )
			guess_m_pLockedPanel[14]->setVisible( false );
		else
			guess_m_pLockedPanel[14]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x1000 )
			guess_m_pLockedPanel[15]->setVisible( false );
		else
			guess_m_pLockedPanel[15]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x2000 )
			guess_m_pLockedPanel[16]->setVisible( false );
		else
			guess_m_pLockedPanel[16]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x4000 )
			guess_m_pLockedPanel[17]->setVisible( false );
		else
			guess_m_pLockedPanel[17]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x8000 )
			guess_m_pLockedPanel[18]->setVisible( false );
		else
			guess_m_pLockedPanel[18]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x10000 )
			guess_m_pLockedPanel[19]->setVisible( false );
		else
			guess_m_pLockedPanel[19]->setVisible( true );
		if ( gHUD.guess_m_iUnlocks & 0x20000 )
			guess_m_pLockedPanel[20]->setVisible( false );
		else
			guess_m_pLockedPanel[20]->setVisible( true );
	}

	Panel::setVisible( state );
}

void CUnlockables::solve()
{
	char szText[128];

	if ( isVisible() )
	{
		for ( int i = 0; i < 21; i++ )
		{
			if ( guess_m_pItemButton[i] && guess_m_pTextLabel && guess_m_pItemButton[i]->isArmed() && guess_m_iArmed != i + 1 )
			{
				guess_m_iArmed = i + 1;

				switch ( i )
				{
				case 0:
					if ( gHUD.guess_m_iUnlocks & 1 || gHUD.guess_m_iUnlocks & 2 || gHUD.guess_m_iUnlocks & 4 || gHUD.guess_m_iUnlocks & 8 || gHUD.guess_m_iUnlocks & 16 )
						guess_m_pTextLabel->setText( "Click to view all unlocked hint pages." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 1:
					if ( gHUD.guess_m_iUnlocks & 0x20 )
						guess_m_pTextLabel->setText( "Nightmare difficulty mode." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 2:
					if ( gHUD.guess_m_iUnlocks & 0x400000 )
						guess_m_pTextLabel->setText( "Developer commentary - Enable in difficulty menu." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 3:
					if ( gHUD.guess_m_iUnlocks & 0x80000000 )
						guess_m_pTextLabel->setText( "Doctor\'s Story. Click to play." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 4:
					if ( gHUD.guess_GetKeyForCommand( "flashlight" ) )
						sprintf( szText, "Nightvision - press %s to enable.", gHUD.guess_GetKeyForCommand( "flashlight" ) );
					else
						strcpy( szText, "Nightvision - Press flashlight button to enable." );

					if ( gHUD.guess_m_iUnlocks & 0x1000000 )
						guess_m_pTextLabel->setText( szText );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 5:
					if ( gHUD.guess_m_iUnlocks & 0x800000 )
						guess_m_pTextLabel->setText( "Hidden package." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 6:
					if ( gHUD.guess_m_iUnlocks & 0x40 )
						guess_m_pTextLabel->setText( "FAMAS assault rifle with infinite ammo." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 7:
					if ( gHUD.guess_m_iUnlocks & 0x80000 )
						guess_m_pTextLabel->setText( "AoMDC axe." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 8:
					if ( gHUD.guess_m_iUnlocks & 0x100000 )
						guess_m_pTextLabel->setText( "Camera of death." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 9:
					if ( gHUD.guess_m_iUnlocks & 0x200000 )
						guess_m_pTextLabel->setText( "Simon\'s book." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 10:
					if ( gHUD.guess_m_iUnlocks & 0x80 )
						guess_m_pTextLabel->setText( "David Leatherhoff suit - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 11:
					if ( gHUD.guess_m_iUnlocks & 0x100 )
						guess_m_pTextLabel->setText( "ModDB hoodie - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 12:
					if ( gHUD.guess_m_iUnlocks & 0x200 )
						guess_m_pTextLabel->setText( "Hello Kitty suit - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 13:
					if ( gHUD.guess_m_iUnlocks & 0x400 )
						guess_m_pTextLabel->setText( "Afraid of Monsters suit - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 14:
					if ( gHUD.guess_m_iUnlocks & 0x800 )
						guess_m_pTextLabel->setText( "Camouflage hoodie - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 15:
					if ( gHUD.guess_m_iUnlocks & 0x1000 )
						guess_m_pTextLabel->setText( "Half-Life Creations hoodie - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 16:
					if ( gHUD.guess_m_iUnlocks & 0x2000 )
						guess_m_pTextLabel->setText( "Black Metal suit - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 17:
					if ( gHUD.guess_m_iUnlocks & 0x4000 )
						guess_m_pTextLabel->setText( "Team Psykskallar hoodie - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 18:
					if ( gHUD.guess_m_iUnlocks & 0x8000 )
						guess_m_pTextLabel->setText( "Awesome suit - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 19:
					if ( gHUD.guess_m_iUnlocks & 0x10000 )
						guess_m_pTextLabel->setText( "Sick Simon suit - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				case 20:
					if ( gHUD.guess_m_iUnlocks & 0x20000 )
						guess_m_pTextLabel->setText( "AoM Twitcher suit - Equip in secret room." );
					else
						guess_m_pTextLabel->setText( "This item is currently locked." );

					guess_m_pTextLabel->setContentAlignment( Label::a_center );
					break;
				}
			}
		}

		if ( guess_m_pTextLabel && guess_m_iArmed && !guess_m_pItemButton[guess_m_iArmed - 1]->isArmed() )
		{
			guess_m_iArmed = 0;
			guess_m_pTextLabel->setText( "" );
			guess_m_pTextLabel->setContentAlignment( Label::a_center );
		}
	}

	Panel::solve();
}
