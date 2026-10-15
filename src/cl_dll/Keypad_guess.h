// ======================================
// Cry of Fear keypad puzzle panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _KEYPAD_H
#define _KEYPAD_H
using namespace vgui;

class CKeypad : public Panel
{
public:
	CKeypad();
	~CKeypad();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	void guess_Activate();
	void guess_PressKey( int key );
	void guess_Submit();
	void guess_UpdateLayout();

protected:
	virtual void paintBackground()
	{
		Panel::paintBackground();
	}

public:
	Panel	*guess_m_pBackgroundPanel;		// 0xbc
	Image	*guess_m_pBackgroundImage;		// 0xc0, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;		// 0xc4
	int		guess_m_iActivated;				// 0xc8, second byte of the Keypad message, blocks the keys
	int		guess_m_iType;					// 0xcc, first byte of the Keypad message, 0 to 2 (panel picture)
	char	guess_m_szCode[8];				// 0xd0, digits typed so far, sent with keypadnumber
	Font	*guess_m_pFont;					// 0xd8, "credits" scheme font
	Label	*guess_m_pCodeLabel;			// 0xdc, shows one star per typed digit (type 2)
	CommandButton	*guess_m_pKey1;			// 0xe0
	CommandButton	*guess_m_pKey2;			// 0xe4
	CommandButton	*guess_m_pKey3;			// 0xe8
	CommandButton	*guess_m_pKey4;			// 0xec
	CommandButton	*guess_m_pKey5;			// 0xf0
	CommandButton	*guess_m_pKey6;			// 0xf4
	CommandButton	*guess_m_pKey7;			// 0xf8
	CommandButton	*guess_m_pKey8;			// 0xfc
	CommandButton	*guess_m_pKey9;			// 0x100
	Image	*guess_m_pPanelBackImage;		// 0x104, gfx/vgui/640_panelback.tga
	ImagePanel	*guess_m_pPanelBack;		// 0x108, parent of the keys
	Image	*guess_m_pPanel2BackImage;		// 0x10c, gfx/vgui/640_panel2back.tga
	ImagePanel	*guess_m_pPanel2Back;		// 0x110
	Image	*guess_m_pPanelOffImage;		// 0x114, gfx/vgui/640_paneloff.tga
	ImagePanel	*guess_m_pPanelOff;			// 0x118
	Image	*guess_m_pPanelOnImage;			// 0x11c, gfx/vgui/640_panelon.tga
	ImagePanel	*guess_m_pPanelOn;			// 0x120
};

class CKeypadHandler_Command : public ActionSignal
{
private:
	CKeypad	*m_pFather;
	int		m_cmd;

public:
	CKeypadHandler_Command( CKeypad *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_KeypadInit();

#endif // _KEYPAD_H
