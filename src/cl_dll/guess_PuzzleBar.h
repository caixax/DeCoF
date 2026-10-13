// ======================================
// Cry of Fear boiler puzzle bar panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _PUZZLEBAR_H
#define _PUZZLEBAR_H
using namespace vgui;

class CPuzzleBar : public Panel
{
public:
	CPuzzleBar();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	virtual void setVisible( bool state );

public:
	Image	*guess_m_pImage;			// 0xbc, gfx/vgui/640_boilerpuzzle.tga
	ImagePanel	*guess_m_pImagePanel;	// 0xc0, parent of the buttons
};

class CPuzzleBarHandler_Command : public ActionSignal
{
private:
	CPuzzleBar	*m_pFather;
	int			m_cmd;

public:
	CPuzzleBarHandler_Command( CPuzzleBar *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_PuzzlebarInit();

#endif // _PUZZLEBAR_H
