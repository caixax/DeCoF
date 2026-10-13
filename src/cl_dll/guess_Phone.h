// ======================================
// Cry of Fear mobile phone panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _PHONE_H
#define _PHONE_H
using namespace vgui;

class CPhone : public Panel
{
public:
	CPhone();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	void guess_PressDigit( int digit );
	void guess_Clear();
	void guess_Dial();

	virtual void setVisible( bool state )
	{
		Panel::setVisible( state );
	}

public:
	Image	*guess_m_pPhoneImage;			// 0xbc, gfx/vgui/640_phone.tga
	ImagePanel	*guess_m_pPhone;			// 0xc0, parent of the buttons and the number label
	Image	*guess_m_pBackgroundImage;		// 0xc4, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;		// 0xc8
	char	guess_m_szNumber[16];			// 0xcc, digits dialed so far, sent with phonenumber
	int		guess_m_iUnknownDC;				// 0xdc, not used by the client code
	Label	*guess_m_pNumberLabel;			// 0xe0, shows guess_m_szNumber
};

class CPhoneHandler_Command : public ActionSignal
{
private:
	CPhone	*m_pFather;
	int		m_cmd;

public:
	CPhoneHandler_Command( CPhone *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_PhoneInit();

#endif // _PHONE_H
