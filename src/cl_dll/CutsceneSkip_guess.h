// ======================================
// Cry of Fear cutscene skip panel header file
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _CUTSCENESKIP_H
#define _CUTSCENESKIP_H
using namespace vgui;

class CCutsceneSkip : public Panel
{
public:
	CCutsceneSkip();
	void Initialize();

public:
	Image	*guess_m_pDotsImage;		// 0xbc, gfx/vgui/loading_dots.tga
	ImagePanel	*guess_m_pDotsPanel;	// 0xc0
};

#endif // _CUTSCENESKIP_H
