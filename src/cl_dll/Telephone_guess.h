// ======================================
// Cry of Fear telephone puzzle panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _TELEPHONE_H
#define _TELEPHONE_H
using namespace vgui;

class CTelephone : public Panel
{
public:
	CTelephone();
	void Initialize();
	void ActionSignal( int cmd );

	void guess_PressDigit( int digit );
	void guess_Dial();

	virtual void setVisible( bool state );

protected:
	virtual void paint();

public:
	Image	*guess_m_pBackgroundImage;		// 0xbc, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;		// 0xc0
	bool	guess_m_bLocked;				// 0xc4, set by the Telephone message (3), blocks the buttons
	float	guess_m_flFade;					// 0xc8, alpha of the telephone picture, 255 when shown
	char	guess_m_szNumber[16];			// 0xcc, number to dial, from the Telephone message
	char	guess_m_szDialed[16];			// 0xdc, digits dialed so far
	Image	*guess_m_pTelephoneImage;		// 0xec, gfx/vgui/puzzle/telephone_vgui.tga
	ImagePanel	*guess_m_pTelephone;		// 0xf0, parent of the buttons
};

class CTelephoneHandler_Command : public ActionSignal
{
private:
	CTelephone	*m_pFather;
	int			m_cmd;

public:
	CTelephoneHandler_Command( CTelephone *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _TELEPHONE_H
