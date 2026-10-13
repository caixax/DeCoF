// ======================================
// Cry of Fear documents panel header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _DOCUMENTS_H
#define _DOCUMENTS_H
using namespace vgui;

#define guess_MAX_DOCUMENTS	16

// all arrays are indexed by the document number, 1 to guess_MAX_DOCUMENTS
class CDocuments : public Panel
{
public:
	CDocuments();
	~CDocuments();
	void Initialize();
	void ActionSignal( int cmd );
	int KeyInput( int down, int keynum, const char *pszCurrentBinding );

	virtual void setVisible( bool state );

public:
	int		guess_m_iHidden[guess_MAX_DOCUMENTS + 1];				// 0xbc, AddNote message, a hidden document cannot be recalled
	CommandButton	*guess_m_pButton[guess_MAX_DOCUMENTS + 1];	// 0x100
	Image	*guess_m_pDocumentsImage;						// 0x144, gfx/vgui/640_documents.tga
	ImagePanel	*guess_m_pDocuments;						// 0x148, parent of the buttons
	Image	*guess_m_pBackgroundImage;						// 0x14c, gfx/vgui/backgroundtoall.tga
	ImagePanel	*guess_m_pBackground;						// 0x150
	ImagePanel	*guess_m_pCodeDigit1[guess_MAX_DOCUMENTS + 1];	// 0x154, gfx/vgui/notes/code_%i.tga set by the AddNote message
	ImagePanel	*guess_m_pCodeDigit2[guess_MAX_DOCUMENTS + 1];	// 0x198
	ImagePanel	*guess_m_pCodeDigit3[guess_MAX_DOCUMENTS + 1];	// 0x1dc
	ImagePanel	*guess_m_pCodeDigit4[guess_MAX_DOCUMENTS + 1];	// 0x220
	Image	*guess_m_pHiddenNoteImage[guess_MAX_DOCUMENTS + 1];	// 0x264, gfx/vgui/notes/hiddennote1.tga
	ImagePanel	*guess_m_pNote[guess_MAX_DOCUMENTS + 1];			// 0x2a8
};

class CDocumentsHandler_Command : public ActionSignal
{
private:
	CDocuments	*m_pFather;
	int			m_cmd;

public:
	CDocumentsHandler_Command( CDocuments *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _DOCUMENTS_H
