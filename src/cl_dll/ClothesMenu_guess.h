// ======================================
// Cry of Fear clothes menu header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _CLOTHESMENU_H
#define _CLOTHESMENU_H
using namespace vgui;

#define guess_MAX_CLOTHES	13	// 12 are used

class CClothesMenu : public Panel
{
public:
	CClothesMenu();
	void Initialize();
	void ActionSignal( int cmd );
	int guess_KeyInput( int down, int keynum, const char *pszCurrentBinding );

	void guess_UpdateButtons();

public:
	float	guess_m_flNextClick;			// 0xbc, ActionSignal ignores clicks before this time
	ColorButton	*guess_m_pClothesButton[guess_MAX_CLOTHES];	// 0xc0
	int		guess_m_iUnlocked;				// 0xf4, bit field of the unlocked clothes, from the Clothes message
};

class CClothesMenuHandler_Command : public ActionSignal
{
private:
	CClothesMenu	*m_pFather;
	int			m_cmd;

public:
	CClothesMenuHandler_Command( CClothesMenu *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _CLOTHESMENU_H
