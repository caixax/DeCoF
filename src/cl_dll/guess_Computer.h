// ======================================
// Cry of Fear computer puzzle panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _COMPUTER_H
#define _COMPUTER_H
using namespace vgui;

class CComputer : public Panel
{
public:
	CComputer();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	virtual void setVisible( bool state );

protected:
	virtual void solve();

public:
	float	guess_m_flNextKeyTime;		// 0xbc
	int		guess_m_iLoggedOn;			// 0xc0, from the Computer message
	int		guess_m_iUserLength;		// 0xc4, typing sound when the length changes
	int		guess_m_iPasswordLength;	// 0xc8
	Image	*guess_m_pBackgroundImage;	// 0xcc, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;	// 0xd0
	TextEntry	*guess_m_pUser;			// 0xd4, "Enter a username..."
	TextEntry	*guess_m_pPassword;		// 0xd8, "Enter a password..."
	Image	*guess_m_pScreenImage;		// 0xdc, gfx/vgui/puzzle/computer/computer_logon.tga
	ImagePanel	*guess_m_pScreen;		// 0xe0, parent of the buttons and the text entries
	TextPanel	*guess_m_pText;			// 0xe4, "Jill - %s! - David"
	int		guess_m_iUnusedE8[4];		// 0xe8, never used in 1.6a (sizeof 0xf8 from the new in the viewport)
};

class CComputerHandler_Command : public ActionSignal
{
private:
	CComputer	*m_pFather;
	int			m_cmd;

public:
	CComputerHandler_Command( CComputer *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_ComputerInit();

#endif // _COMPUTER_H
