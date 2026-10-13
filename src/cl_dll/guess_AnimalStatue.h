// ======================================
// Cry of Fear animal statue puzzle panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _ANIMALSTATUE_H
#define _ANIMALSTATUE_H
using namespace vgui;

class CAnimalStatue : public Panel
{
public:
	CAnimalStatue();
	~CAnimalStatue();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

public:
	Panel	*guess_m_pBlackPanel;		// 0xbc
	Image	*guess_m_pBackgroundImage;	// 0xc0, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;	// 0xc4
	int		guess_m_iStatue;			// 0xc8, from the Statue message, sent back in movestatue
	Image	*guess_m_pButtonsImage;		// 0xcc, gfx/vgui/640_eagle_button.tga
	ImagePanel	*guess_m_pButtons;		// 0xd0, parent of the buttons
};

class CAnimalStatueHandler_Command : public ActionSignal
{
private:
	CAnimalStatue	*m_pFather;
	int				m_cmd;

public:
	CAnimalStatueHandler_Command( CAnimalStatue *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_StatueInit();

#endif // _ANIMALSTATUE_H
