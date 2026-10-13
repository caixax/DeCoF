// ======================================
// Cry of Fear subtitle language menu header file
// (class names from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _LANGUAGE_H
#define _LANGUAGE_H
using namespace vgui;

class CLanguage : public Panel
{
public:
	CLanguage();
	void Initialize();
	void ActionSignal( int cmd );

public:
	Image	*guess_m_pBackgroundImage;		// 0xbc, gfx/vgui/language_settings.tga
	ImagePanel	*guess_m_pBackground;		// 0xc0, parent of the language buttons
};

class CLanguageHandler_Command : public ActionSignal
{
private:
	CLanguage	*m_pFather;
	int			m_cmd;

public:
	CLanguageHandler_Command( CLanguage *panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel *panel )
	{
		m_pFather->ActionSignal( m_cmd );
	}
};

#endif // _LANGUAGE_H
