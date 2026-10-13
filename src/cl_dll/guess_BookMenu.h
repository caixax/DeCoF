// ======================================
// Cry of Fear book menu header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _BOOKMENU_H
#define _BOOKMENU_H
using namespace vgui;

class CBookMenu : public Panel
{
public:
	CBookMenu();
	~CBookMenu();
	void Initialize();
	void ActionSignal( int cmd );

	virtual void setVisible( bool state );

public:
	Image	*guess_m_pArrowsImage;		// 0xbc, gfx/vgui/left_right.tga
	ImagePanel	*guess_m_pArrows;		// 0xc0
	Image	*guess_m_pCancelImage;		// 0xc4, gfx/vgui/640_notecancel.tga
	ImagePanel	*guess_m_pCancel;		// 0xc8
};

class CBookMenuHandler_Command : public ActionSignal
{
private:
	CBookMenu	*m_pFather;
	int			m_cmd;

public:
	CBookMenuHandler_Command( CBookMenu *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _BOOKMENU_H
