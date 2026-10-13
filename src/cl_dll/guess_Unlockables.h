// ======================================
// Cry of Fear unlockables menu header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _UNLOCKABLES_H
#define _UNLOCKABLES_H
using namespace vgui;

#define guess_MAX_UNLOCKABLES	22	// 21 are used

class CUnlockables : public Panel
{
public:
	CUnlockables();
	~CUnlockables();
	void Initialize();
	void ActionSignal( int cmd );
	int guess_KeyInput( int down, int keynum, const char *pszCurrentBinding );

	virtual void setVisible( bool state );

protected:
	virtual void solve();

public:
	int		guess_m_iUnknownBC;				// 0xbc, never used in 1.6a
	int		guess_m_iArmed;					// 0xc0, 1 + index of the item under the cursor, 0 for none
	float	guess_m_flNextClick;			// 0xc4, ActionSignal ignores clicks before this time; never written in 1.6a
	ColorButton	*guess_m_pItemButton[guess_MAX_UNLOCKABLES];	// 0xc8
	Label	*guess_m_pTextLabel;			// 0x120, description of the item under the cursor
	ColorButton	*guess_m_pBackButton;		// 0x124
	Image	*guess_m_pLockedImage[guess_MAX_UNLOCKABLES];	// 0x128, gfx/vgui/unlockables/unlockables_locked.tga
	ImagePanel	*guess_m_pLockedPanel[guess_MAX_UNLOCKABLES];	// 0x180, hidden once the item is unlocked
	Image	*guess_m_pMenuImage;			// 0x1d8, gfx/vgui/unlockables/unlockables_menu.tga
	ImagePanel	*guess_m_pMenu;				// 0x1dc
};

class CUnlockablesHandler_Command : public ActionSignal
{
private:
	CUnlockables	*m_pFather;
	int			m_cmd;

public:
	CUnlockablesHandler_Command( CUnlockables *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _UNLOCKABLES_H
