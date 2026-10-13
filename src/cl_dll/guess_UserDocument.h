// ======================================
// Cry of Fear computer user and password document panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _USERDOCUMENT_H
#define _USERDOCUMENT_H
using namespace vgui;

class CUserDocument : public CMenuPanel
{
public:
	CUserDocument( int iTrans, int iRemoveMe, int x, int y, int wide, int tall );
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	const char *guess_GetPassword( int index );
	const char *guess_GetUsername( int index );

	virtual void setVisible( bool state )
	{
		Panel::setVisible( state );
	}

protected:
	virtual void paintBackground()
	{
		Panel::paintBackground();
	}

	virtual void paint();

public:
	CommandButton	*guess_m_pCloseButton;	// 0xd4, command 1
	CImageLabel	*guess_m_pCancelLabel;		// 0xd8, notecancel
	Font	*guess_m_pFont;					// 0xdc, "credits" scheme font
	Image	*guess_m_pDocumentImage;		// 0xe0, gfx/vgui/640_pass1.tga
	ImagePanel	*guess_m_pDocument;			// 0xe4, gets 640_user%i.tga or 640_pass%i.tga
	Image	*guess_m_pBackgroundImage;		// 0xe8, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;		// 0xec
	int		guess_m_iType;					// 0xf0, first byte of the UserDoc message (1 user name, 2 password)
	int		guess_m_iIndex;					// 0xf4, second byte of the UserDoc message (1 to 8)
};

class CUserDocumentHandler_Command : public ActionSignal
{
private:
	CUserDocument	*m_pFather;
	int				m_cmd;

public:
	CUserDocumentHandler_Command( CUserDocument *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_UserDocInit();

#endif // _USERDOCUMENT_H
