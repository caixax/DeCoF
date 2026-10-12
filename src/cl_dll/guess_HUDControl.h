// ======================================
// Cry of Fear HUD panel header file
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _HUDCONTROL_H
#define _HUDCONTROL_H
using namespace vgui;

class CHUDControl : public Panel
{
public:
	CHUDControl();
	~CHUDControl();
	void Initialize();

	void guess_UpdateHUDVisibility();
	void guess_UpdateLogo();
	void guess_DrawMessages();
	void guess_ShowSMSIcon( bool bShow );

protected:
	virtual void solve();
	virtual void paintBackground();
	virtual void paint();

public:
	int		guess_m_iMessageType;		// 0xbc, text color of the message strip (5 = grey)
	int		guess_m_iMessageTime;		// 0xc0
	int		guess_m_iMessageC4;			// 0xc4
	int		guess_m_iMessageC8;			// 0xc8
	int		guess_m_iMessageCC;			// 0xcc
	int		guess_m_iMessageD0;			// 0xd0
	char	guess_m_szMessage[256];		// 0xd4, "You got the %s" and the ProFont messages
	bool	guess_m_bShowLogo;			// 0x1d4
	char	guess_m_szMessage2[256];	// 0x1d5, second line of the message strip
	int		guess_m_iDevDonator[5];		// 0x2d8, indexed by player
	Label	*guess_m_pTimerPlusLabel[32];	// 0x2ec, "+0" popups of TimerPlus
	float	guess_m_flTimerPlusTime;	// 0x36c
	int		guess_m_iTimerLimit;		// 0x370
	bool	guess_m_bTimerPaused;		// 0x374
	Panel	*guess_m_pLetterboxTop;		// 0x378
	Panel	*guess_m_pLetterboxBottom;	// 0x37c
	Panel	*guess_m_pOverlay;			// 0x380
	Label	*guess_m_pVoteLabel;		// 0x384, "Press Vote button when ready"
	Label	*guess_m_pLobbyLabel[4];	// 0x388
	Label	*guess_m_pVoteKickLabel;	// 0x398, "Type votekick name in chat to votekick a player"
	bool	guess_m_bLobby;				// 0x39c
	bool	guess_m_bPlayerReady[4];	// 0x39d
	Label	*guess_m_pTimerLabel;		// 0x3a4, "00:00:00"
	Label	*guess_m_pMessageLabel;		// 0x3a8
	Label	*guess_m_pMessageLabel2;	// 0x3ac
	Label	*guess_m_pHintLabel;		// 0x3b0
	Label	*guess_m_pSayTextLabel[5];	// 0x3b4
	Label	*guess_m_pUnused3C8;		// 0x3c8, never used in 1.6a
	Label	*guess_m_pLookingAtLabel;	// 0x3cc
	Label	*guess_m_pPlayerLabel[4];	// 0x3d0
	Label	*guess_m_pDeathLabel;		// 0x3e0
	float	guess_m_flLookingAtTime;	// 0x3e4
	ImagePanel	*guess_m_pLobbyBackground;	// 0x3e8
	int		guess_m_iDeathAlpha;		// 0x3ec
	bool	guess_m_bTimer;				// 0x3f0
	float	guess_m_flTimerNextSecond;	// 0x3f4
	int		guess_m_iTimerSeconds;		// 0x3f8
	int		guess_m_iTimerMinutes;		// 0x3fc
	int		guess_m_iTimerHours;		// 0x400
	char	guess_m_szHintLine1[256];	// 0x404, "line1" of the hint file
	char	guess_m_szHintLine2[256];	// 0x504, "line2"
	char	guess_m_szHintLine3[256];	// 0x604, "line3"
	char	guess_m_szHintNoButton[256];	// 0x704, "nobutton"
	char	guess_m_szHintCfgButton[64];	// 0x804, "cfgbutton"
	int		guess_m_iHintLine;			// 0x844
	bool	guess_m_bHint;				// 0x848
	int		guess_m_iHintAlpha;			// 0x84c
	float	guess_m_flHintTime;			// 0x850
	int		guess_m_iUnused854;			// 0x854, never used in 1.6a
	int		guess_m_iUnused858;			// 0x858, never used in 1.6a
	int		guess_m_iSayTextX;			// 0x85c
	int		guess_m_iSayTextY;			// 0x860
	char	guess_m_szUnused864[6][256];	// 0x864, never used in 1.6a
	ImagePanel	*guess_m_pIntroduction;	// 0xe64
	Image	*guess_m_pIntroductionImage;	// 0xe68
	bool	guess_m_bIntroductionFadeIn;	// 0xe6c
	int		guess_m_iIntroductionAlpha;	// 0xe70
	ImagePanel	*guess_m_pHintBar;		// 0xe74
	Image	*guess_m_pHintBarImage;		// 0xe78
	ImagePanel	*guess_m_pVignetteTL;	// 0xe7c
	ImagePanel	*guess_m_pVignetteTR;	// 0xe80
	ImagePanel	*guess_m_pVignetteBL;	// 0xe84
	ImagePanel	*guess_m_pVignetteBR;	// 0xe88
	Panel	*guess_m_pBlackScreen;		// 0xe8c
	Font	*guess_m_pPagerFont;		// 0xe90, scheme "pager"
	Font	*guess_m_pChapterTitleFont;	// 0xe94, scheme "chaptertitle"
	char	guess_m_szChapter[128];		// 0xe98
	float	guess_m_flMessageAlpha;		// 0xf18
	bool	guess_m_bMessageFadeOut;	// 0xf1c
	bool	guess_m_bMessageHold;		// 0xf1d
	bool	guess_m_bChapterF1E;		// 0xf1e
	float	guess_m_flMessageHoldTime;	// 0xf20
	float	guess_m_flMessageF24;		// 0xf24
	int		guess_m_iMessageChars;		// 0xf28
	float	guess_m_flChapterAlpha;		// 0xf2c
	bool	guess_m_bChapterFadeOut;	// 0xf30
	bool	guess_m_bChapterHold;		// 0xf31
	float	guess_m_flChapterHoldTime;	// 0xf34
	float	guess_m_flChapterNextChar;	// 0xf38
	int		guess_m_iChapterChars;		// 0xf3c
	int		guess_m_iChapterType;		// 0xf40
	float	guess_m_flHintSlideInTime;	// 0xf44
	float	guess_m_flHintSlideInLength;	// 0xf48
	float	guess_m_flHintSlideOutTime;	// 0xf4c
	float	guess_m_flHintSlideOutLength;	// 0xf50
	Font	*guess_m_pCreditsFont;		// 0xf54, scheme "credits"
	int		guess_m_iLogoAlpha;			// 0xf58
	bool	guess_m_bLogoF5C;			// 0xf5c
	bool	guess_m_bLogoF5D;			// 0xf5d
	float	guess_m_flLogoTime;			// 0xf60
	int		guess_m_iLogo;				// 0xf64
	Image	*guess_m_pFollowIcon[4];		// 0xf68, gfx/vgui/coop_follow_icon.tga
	ImagePanel	*guess_m_pFollowIconPanel[4];	// 0xf78
	Image	*guess_m_pSyringeIcon[4];		// 0xf88, gfx/vgui/coop_syringe_icon.tga
	ImagePanel	*guess_m_pSyringeIconPanel[4];	// 0xf98
	int		guess_m_iUnusedFA8[5];		// 0xfa8, never used in 1.6a
	float	guess_m_flZoomFOV;			// 0xfbc
	float	guess_m_flZoomLength;		// 0xfc0
	float	guess_m_flZoomTime;			// 0xfc4
	ImagePanel	*guess_m_pHealthBar;		// 0xfc8
	ImagePanel	*guess_m_pHealthBarFull;	// 0xfcc
	ImagePanel	*guess_m_pHealthBarEmpty;	// 0xfd0
	ImagePanel	*guess_m_pLogo;				// 0xfd4
	ImagePanel	*guess_m_pStaminaBar;		// 0xfd8
	ImagePanel	*guess_m_pStaminaBarFull;	// 0xfdc
	ImagePanel	*guess_m_pStaminaBarEmpty;	// 0xfe0
	float	guess_m_flStaminaFlashTime;	// 0xfe4
	Panel	*guess_m_pStaminaFlash;		// 0xfe8
	Image	*guess_m_pLogoImage;		// 0xfec
	ImagePanel	*guess_m_pSMSIcon;		// 0xff0
	bool	guess_m_bSMSIcon;			// 0xff4
	float	guess_m_flSMSIconTime;		// 0xff8
	int		guess_m_iSMSIconAlpha;		// 0xffc
	int		guess_m_iStamina;			// 0x1000
	ImagePanel	*guess_m_pInventoryHint;	// 0x1004
	bool	guess_m_bInventoryHint;		// 0x1008
	float	guess_m_flInventoryHintTime;	// 0x100c
	int		guess_m_iUnused1010;		// 0x1010, never used in 1.6a
	int		guess_m_iInventoryHintY;	// 0x1014
	Image	*guess_m_pSMSIconImage;		// 0x1018, gfx/vgui/640_hudelem_smsicon.tga
	Image	*guess_m_pInventoryHintImage;	// 0x101c, gfx/vgui/hints/hint_inventory.tga
	Image	*guess_m_pCrosshairImage;	// 0x1020, gfx/vgui/crosshair.tga
	Image	*guess_m_pHealthBarImage;	// 0x1024, gfx/vgui/640_healthbar.tga
	Image	*guess_m_pHealthBarFullImage;	// 0x1028, gfx/vgui/640_healthbar_bar.tga
	Image	*guess_m_pHealthBarEmptyImage;	// 0x102c, gfx/vgui/640_healthbar_barempty.tga
	Image	*guess_m_pStaminaBarImage;	// 0x1030, gfx/vgui/640_staminabar.tga
	Image	*guess_m_pStaminaBarFullImage;	// 0x1034, gfx/vgui/640_staminabar_bar.tga
	Image	*guess_m_pStaminaBarEmptyImage;	// 0x1038, gfx/vgui/640_staminabar_barempty.tga
	int		guess_m_iUnused103C[4];		// 0x103c, never used in 1.6a
};

void guess_HUDControlInit();
void ParseHintTokens( char *pszName, char *pszFile );

#endif // _HUDCONTROL_H
