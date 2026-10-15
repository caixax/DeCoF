// ======================================
// Cry of Fear note document panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _NOTEDOCUMENT_H
#define _NOTEDOCUMENT_H
using namespace vgui;

class CNoteDocument : public CMenuPanel
{
public:
	CNoteDocument( int iTrans, int iRemoveMe, int x, int y, int wide, int tall );
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	void guess_ClearString( char *str );
	int guess_CountLines();
	void guess_NextLine();
	void guess_NextPage();
	void guess_PrevLine();
	void guess_PrevPage();
	void guess_SetPage( int page );
	void guess_ReadLine();

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
	CommandButton	*guess_m_pCloseButton;	// 0xd4, command 3
	CTransparentPanel	*guess_m_pPanel;	// 0xd8, 640x480 window
	CImageLabel	*guess_m_pImageLabel;		// 0xdc, picture of the page
	CImageLabel	*guess_m_pCancelLabel;		// 0xe0, notecancel
	int		guess_m_iLine;					// 0xe4, line of the text file shown in guess_m_szLine
	int		guess_m_iPage;					// 0xe8, page (1 to 4), picks the text file
	char	guess_m_szImage1[16];			// 0xec, picture of page 1, from the NoteDoc message
	char	guess_m_szImage2[16];			// 0xfc
	char	guess_m_szImage3[16];			// 0x10c
	char	guess_m_szImage4[16];			// 0x11c
	char	guess_m_szSound[48];			// 0x12c, played by guess_SetPage
	char	guess_m_szText1[16];			// 0x15c, text file of page 1 (under notes/)
	char	guess_m_szText2[16];			// 0x16c
	char	guess_m_szText3[16];			// 0x17c
	char	guess_m_szText4[16];			// 0x18c
	char	guess_m_szLine[128];			// 0x19c, current line of the text file
	bool	guess_m_bTitle;					// 0x21c, guess_m_szLine is the first line
	Label	*guess_m_pLineLabel;			// 0x220, shows guess_m_szLine
	Image	*guess_m_pBackgroundImage;		// 0x224, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;		// 0x228
	Font	*guess_m_pFont;					// 0x22c, "credits" scheme font
};

class CNoteDocumentHandler_Command : public ActionSignal
{
private:
	CNoteDocument	*m_pFather;
	int				m_cmd;

public:
	CNoteDocumentHandler_Command( CNoteDocument *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

void guess_NoteDocInit();

#endif // _NOTEDOCUMENT_H
