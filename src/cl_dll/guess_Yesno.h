// ======================================
// Cry of Fear yes/no question panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _YESNO_H
#define _YESNO_H
using namespace vgui;

class CYesno : public Panel
{
public:
	CYesno();
	void Initialize();
	void ActionSignal( int cmd );

public:
	int		guess_m_iActivate;			// 0xbc, sent with yesnoactivate and yesnono
	Image	*guess_m_pImage;			// 0xc0, gfx/vgui/640_savegame.tga
	ImagePanel	*guess_m_pImagePanel;	// 0xc4, gets the image of the YesNo message
	CommandButton	*guess_m_pYesButton;	// 0xc8
	CommandButton	*guess_m_pNoButton;		// 0xcc
};

class CYesnoHandler_Command : public ActionSignal
{
private:
	CYesno	*m_pFather;
	int		m_cmd;

public:
	CYesnoHandler_Command( CYesno *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_YesNoInit();

#endif // _YESNO_H
