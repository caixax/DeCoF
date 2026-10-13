// ======================================
// Cry of Fear boss health bar
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "guess_BossBar.h"
#include "..\game_shared\vgui_loadtga.h"

CBossBar::CBossBar() : Panel( 0, 0, ScreenWidth, ScreenHeight )
{
	int wide, tall;
	int x;

	guess_m_pFullImage = vgui_LoadTGA( "gfx/vgui/boss/doctor_health_full.tga" );
	if ( guess_m_pFullImage )
		guess_m_pFullImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/boss/doctor_health_full.tga!\n" );

	guess_m_pEmptyImage = vgui_LoadTGA( "gfx/vgui/boss/doctor_health_empty.tga" );
	if ( guess_m_pEmptyImage )
		guess_m_pEmptyImage->setColor( Color( 255, 255, 255, 1 ) );
	else
		gEngfuncs.Con_Printf( "Cannot load gfx/vgui/boss/doctor_health_empty.tga!\n" );

	guess_m_pFull = new ImagePanel( guess_m_pFullImage );
	guess_m_pFull->setParent( this );
	wide = guess_m_pFull->getWide();
	tall = guess_m_pFull->getTall();
	x = ScreenWidth / 2 - wide / 2;
	guess_m_pFull->setPos( x, tall );
	guess_m_pFull->setVisible( true );

	guess_m_pEmpty = new ImagePanel( guess_m_pEmptyImage );
	guess_m_pEmpty->setParent( this );
	guess_m_pEmpty->setPos( x, tall );
	guess_m_pEmpty->setVisible( true );

	guess_m_pBar = new ImagePanel( guess_m_pFullImage );
	guess_m_pBar->setParent( this );
	guess_m_pBar->setPos( x, tall );
	guess_m_pBar->setVisible( true );

	setBgColor( 0, 0, 0, 255 );
	guess_m_iUnused1C8 = 0;

	Initialize();
}

CBossBar::~CBossBar()
{
	delete guess_m_pFullImage;
	delete guess_m_pEmptyImage;
}

void CBossBar::Initialize()
{
	guess_m_iUnused1C8 = 0;
	setVisible( false );
}

int guess_MsgBossBar( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	if ( gViewPort->guess_m_pBossBar )
	{
		gViewPort->guess_m_pBossBar->guess_m_iUnused1C8 = 0;
		gViewPort->guess_m_pBossBar->guess_m_iState = READ_BYTE();
		sprintf( gViewPort->guess_m_pBossBar->guess_m_szEmptyImage, "%s", READ_STRING() );
		sprintf( gViewPort->guess_m_pBossBar->guess_m_szFullImage, "%s", READ_STRING() );

		gViewPort->guess_m_pBossBar->guess_m_pFull->setImage( vgui_LoadTGA( gViewPort->guess_m_pBossBar->guess_m_szFullImage ) );
		gViewPort->guess_m_pBossBar->guess_m_pBar->setImage( vgui_LoadTGA( gViewPort->guess_m_pBossBar->guess_m_szFullImage ) );
		gViewPort->guess_m_pBossBar->guess_m_pEmpty->setImage( vgui_LoadTGA( gViewPort->guess_m_pBossBar->guess_m_szEmptyImage ) );

		int iHealth = READ_SHORT();
		if ( iHealth < 0 )
			iHealth = 0;

		if ( gViewPort->guess_m_pBossBar->guess_m_iState == 1 )
			gViewPort->guess_m_pBossBar->guess_m_iHealth = iHealth;
		else
			gViewPort->guess_m_pBossBar->guess_m_iHealth = 0;

		gViewPort->guess_m_pBossBar->guess_m_iMaxHealth = READ_SHORT();

		if ( gViewPort->guess_m_pBossBar->guess_m_iState == 1 )
			gViewPort->guess_m_pBossBar->setVisible( true );
		else if ( gViewPort->guess_m_pBossBar->guess_m_iState == 0 )
			gViewPort->guess_m_pBossBar->setVisible( false );
	}

	return 1;
}

void guess_BossBarInit()
{
	gEngfuncs.pfnHookUserMsg( "BossBar", guess_MsgBossBar );
}

void CBossBar::solve()
{
	int tall = guess_m_pEmpty->getTall();
	int wide = guess_m_pEmpty->getWide();

	if ( guess_m_iState == 2 )
		setVisible( false );

	int width = (int)( (float)guess_m_iHealth / (float)guess_m_iMaxHealth * (float)wide );
	if ( width < 1 )
		width = 0;

	guess_m_pBar->setSize( width, tall );

	Panel::solve();
}
