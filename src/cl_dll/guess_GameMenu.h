// ======================================
// Cry of Fear game menu header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _GAMEMENU_H
#define _GAMEMENU_H
using namespace vgui;

#define guess_MAX_CAMPAIGNS	256

// a custom campaign, read from maps/*.custom
typedef struct
{
	char	szFirstMap[64];		// "firstmap"
	char	szDescription[64];	// "description", the text of its button
	char	szAuthor[64];		// "author"
	bool	bValid;
} guess_campaign_t;

class CCampaignSlider : public Slider
{
public:
	CCampaignSlider( int x, int y, int wide, int tall, bool vertical ) : Slider( x, y, wide, tall, vertical )
	{
	}

	virtual void paintBackground();
};

class CGameMenu : public Panel
{
public:
	CGameMenu();
	void Initialize();
	void ActionSignal( int cmd );

	void guess_ParseCampaign( char *pszFile, int index );
	void guess_LoadCampaigns();
	void guess_ResetButtonPositions();

	virtual void setVisible( bool state );

protected:
	virtual void solve();

public:
	Label	*guess_m_pVersionLabel;			// 0xbc, "1.6a" in the top left corner
	bool	guess_m_bUnknownC0;				// 0xc0, only cleared
	guess_campaign_t	guess_m_Campaigns[guess_MAX_CAMPAIGNS + 1];	// 0xc1
	ColorButton	*guess_m_pCampaignButton[guess_MAX_CAMPAIGNS];	// 0xc284
	int		guess_m_iUnknownC684;			// 0xc684, never used in 1.6a
	int		guess_m_iUnknownC688;			// 0xc688, never used in 1.6a
	Label	*guess_m_pAuthorLabel;			// 0xc68c, "Author: %s" of the campaign under the cursor
	Font	*guess_m_pCreditsFont;			// 0xc690, scheme "credits"
	int		guess_m_iArmedCampaign;			// 0xc694, campaign button under the cursor, -1 for none
	bool	guess_m_bCampaignArmed;			// 0xc698
	bool	guess_m_bNewGameArmed;		// 0xc699, the newgame_mo.tga picture is shown
	bool	guess_m_bLoadGameArmed;		// 0xc69a, the loadgame_mo.tga picture is shown
	bool	guess_m_bCustomArmed;		// 0xc69b, the customc_mo.tga picture is shown
	bool	guess_m_bJoinServerArmed;		// 0xc69c, the jserver_mo.tga picture is shown
	bool	guess_m_bOptionsArmed;		// 0xc69d, the options_mo.tga picture is shown
	bool	guess_m_bLanguageArmed;		// 0xc69e, the language_mo.tga picture is shown
	bool	guess_m_bUnlockablesArmed;		// 0xc69f, the unlockables_mo.tga picture is shown
	bool	guess_m_bExtrasArmed;		// 0xc6a0, the extras_mo.tga picture is shown
	bool	guess_m_bHostServerArmed;		// 0xc6a1, the hserver_mo.tga picture is shown
	bool	guess_m_bQuitArmed;		// 0xc6a2, the quit_mo.tga picture is shown
	char	*guess_m_pszCampaign;			// 0xc6a4, first map of the campaign picked for the difficulty menu, NULL for a new game
	int		guess_m_iArmedX;				// 0xc6a8, position of the picture under the cursor, it shakes around it
	int		guess_m_iArmedY;				// 0xc6ac
	CTFScrollPanel	*guess_m_pCampaignList;	// 0xc6b0
	ColorButton	*guess_m_pNewGameButton;	// 0xc6b4
	ColorButton	*guess_m_pLoadGameButton;	// 0xc6b8
	ColorButton	*guess_m_pCustomButton;	// 0xc6bc
	ColorButton	*guess_m_pJoinServerButton;	// 0xc6c0
	ColorButton	*guess_m_pOptionsButton;	// 0xc6c4
	ColorButton	*guess_m_pLanguageButton;	// 0xc6c8
	ColorButton	*guess_m_pUnlockablesButton;	// 0xc6cc
	ColorButton	*guess_m_pExtrasButton;	// 0xc6d0
	ColorButton	*guess_m_pHostServerButton;	// 0xc6d4
	ColorButton	*guess_m_pQuitButton;	// 0xc6d8
	ColorButton	*guess_m_pCancelButton;		// 0xc6dc, closes the campaign list
	Image	*guess_m_pLogoImage;			// 0xc6e0, gfx/vgui/menu_logo.tga
	ImagePanel	*guess_m_pLogo;				// 0xc6e4
	Image	*guess_m_pLogoTSImage;			// 0xc6e8, gfx/vgui/menu_logo_ts.tga
	ImagePanel	*guess_m_pLogoTS;			// 0xc6ec
	Image	*guess_m_pNewGameImage;		// 0xc6f0, gfx/vgui/menu/newgame.tga
	ImagePanel	*guess_m_pNewGame;		// 0xc6f4
	Image	*guess_m_pLoadGameImage;		// 0xc6f8, gfx/vgui/menu/loadgame.tga
	ImagePanel	*guess_m_pLoadGame;		// 0xc6fc
	Image	*guess_m_pCustomImage;		// 0xc700, gfx/vgui/menu/customc.tga
	ImagePanel	*guess_m_pCustom;		// 0xc704
	Image	*guess_m_pJoinServerImage;		// 0xc708, gfx/vgui/menu/jserver.tga
	ImagePanel	*guess_m_pJoinServer;		// 0xc70c
	Image	*guess_m_pOptionsImage;		// 0xc710, gfx/vgui/menu/options.tga
	ImagePanel	*guess_m_pOptions;		// 0xc714
	Image	*guess_m_pLanguageImage;		// 0xc718, gfx/vgui/menu/language.tga
	ImagePanel	*guess_m_pLanguage;		// 0xc71c
	Image	*guess_m_pUnlockablesImage;		// 0xc720, gfx/vgui/menu/unlockables.tga
	ImagePanel	*guess_m_pUnlockables;		// 0xc724
	Image	*guess_m_pExtrasImage;		// 0xc728, gfx/vgui/menu/extras.tga
	ImagePanel	*guess_m_pExtras;		// 0xc72c
	Image	*guess_m_pHostServerImage;		// 0xc730, gfx/vgui/menu/hserver.tga
	ImagePanel	*guess_m_pHostServer;		// 0xc734
	Image	*guess_m_pQuitImage;		// 0xc738, gfx/vgui/menu/quit.tga
	ImagePanel	*guess_m_pQuit;		// 0xc73c
	int		guess_m_iUnknownC740;			// 0xc740, never used in 1.6a
	int		guess_m_iUnknownC744;			// 0xc744, never used in 1.6a
};

class CGameMenuHandler_Command : public ActionSignal
{
private:
	CGameMenu	*m_pFather;
	int			m_cmd;

public:
	CGameMenuHandler_Command( CGameMenu *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _GAMEMENU_H
