// ======================================
// Cry of Fear billboard panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _VGUIBILLBOARD_H
#define _VGUIBILLBOARD_H
using namespace vgui;

class CVGUIBillboard : public Panel
{
public:
	CVGUIBillboard();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	virtual void setVisible( bool state );

protected:
	virtual void paintBackground();
	virtual void paint();

public:
	Image	*guess_m_pBackgroundImage;	// 0xbc, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;	// 0xc0
	ImagePanel	*guess_m_pImage;		// 0xc4, the picture of the Board message
	Label	*guess_m_pLabel;			// 0xc8, repeats the message line of the HUD panel
	Font	*guess_m_pFont;				// 0xcc, scheme "credits"
};

class CBillboardHandler_Command : public ActionSignal
{
private:
	CVGUIBillboard	*m_pFather;
	int				m_cmd;

public:
	CBillboardHandler_Command( CVGUIBillboard *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_BoardInit();

#endif // _VGUIBILLBOARD_H
