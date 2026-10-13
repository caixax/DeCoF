// ======================================
// Cry of Fear game over panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _GAMEOVER_H
#define _GAMEOVER_H
using namespace vgui;

class CGameOver : public Panel
{
public:
	CGameOver();
	~CGameOver();
	void Initialize();
	void ActionSignal( int cmd );

	virtual void setVisible( bool state );

protected:
	virtual void solve();

public:
	Image	*guess_m_pImage;			// 0xbc, gfx/vgui/game_over.tga
	ImagePanel	*guess_m_pPanel;		// 0xc0, parent of the buttons
	Image	*guess_m_pLoadImage;		// 0xc4, gfx/vgui/game_over_load.tga
	ImagePanel	*guess_m_pLoad;			// 0xc8
	Image	*guess_m_pExitImage;		// 0xcc, gfx/vgui/game_over_exit.tga
	ImagePanel	*guess_m_pExit;			// 0xd0
	ColorButton	*guess_m_pLoadButton;	// 0xd4
	ColorButton	*guess_m_pExitButton;	// 0xd8
	char	guess_m_szCustomDoc[256];	// 0xdc, map of the CustomDoc message, restarted in doctor mode
	float	guess_m_flAlpha;			// 0x1dc, fades the pictures in
	bool	guess_m_bLoadArmed;			// 0x1e0
	bool	guess_m_bExitArmed;			// 0x1e1
};

class CGameOverHandler_Command : public ActionSignal
{
private:
	CGameOver	*m_pFather;
	int			m_cmd;

public:
	CGameOverHandler_Command( CGameOver *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_CustomDocInit();

#endif // _GAMEOVER_H
