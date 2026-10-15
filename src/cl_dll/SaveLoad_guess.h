// ======================================
// Cry of Fear save and load slots panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _SAVELOAD_H
#define _SAVELOAD_H
using namespace vgui;

class CSaveLoad : public Panel
{
public:
	CSaveLoad();
	void Initialize();
	void ActionSignal( int cmd );

	void guess_UpdateSlots();
	void guess_ReadSlot( int slot );
	void guess_WriteSaveInfo( const char *pszFile );

protected:
	virtual void solve()
	{
		Panel::solve();
	}

public:
	Label	*guess_m_pSlot1Label;			// 0xbc, text of saveinfo1.cof
	Label	*guess_m_pSlot2Label;			// 0xc0, text of saveinfo2.cof
	Label	*guess_m_pSlot3Label;			// 0xc4, text of saveinfo3.cof
	Label	*guess_m_pSlot4Label;			// 0xc8, text of saveinfo4.cof
	Label	*guess_m_pSlot5Label;			// 0xcc, text of saveinfo5.cof
	ColorButton	*guess_m_pSlot1Button;		// 0xd0
	ColorButton	*guess_m_pSlot2Button;		// 0xd4
	ColorButton	*guess_m_pSlot3Button;		// 0xd8
	ColorButton	*guess_m_pSlot4Button;		// 0xdc
	ColorButton	*guess_m_pSlot5Button;		// 0xe0
	float	guess_m_flSlot1ClickTime;		// 0xe4, a second click before this time picks the slot
	float	guess_m_flSlot2ClickTime;		// 0xe8
	float	guess_m_flSlot3ClickTime;		// 0xec
	float	guess_m_flSlot4ClickTime;		// 0xf0
	float	guess_m_flSlot5ClickTime;		// 0xf4
	char	guess_m_szSaveName[128];		// 0xf8, from the SaveLoad message, written to the save info
	int		guess_m_iMode;					// 0x178, 0 saves, else loads
	Image	*guess_m_pBackgroundImage;		// 0x17c, gfx/vgui/save_load.tga
	ImagePanel	*guess_m_pBackground;		// 0x180
};

class CSaveLoadHandler_Command : public ActionSignal
{
private:
	CSaveLoad	*m_pFather;
	int			m_cmd;

public:
	CSaveLoadHandler_Command( CSaveLoad *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _SAVELOAD_H
