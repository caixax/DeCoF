// ======================================
// Cry of Fear save question panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _SAVE_H
#define _SAVE_H
using namespace vgui;

class CSave : public Panel
{
public:
	CSave();
	void Initialize();
	void ActionSignal( int cmd );

public:
	Image	*guess_m_pImage;			// 0xbc, gfx/vgui/640_savegame.tga
	ImagePanel	*guess_m_pImagePanel;	// 0xc0, parent of the yes and no buttons
};

class CSaveHandler_Command : public ActionSignal
{
private:
	CSave	*m_pFather;
	int		m_cmd;

public:
	CSaveHandler_Command( CSave *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_SaveInit();

#endif // _SAVE_H
