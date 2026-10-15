// ======================================
// Cry of Fear TV frame panel header file
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _TVFRAME_H
#define _TVFRAME_H
using namespace vgui;

class CTVFrame : public Panel
{
public:
	CTVFrame();
	void Initialize();

public:
	int		guess_m_iTVFrame;			// 0xbc
	int		guess_m_iUnusedC0[8];		// 0xc0, never used in 1.6a (sizeof 0xe0 from the new in the viewport)
};

void guess_TVFrameInit();

#endif // _TVFRAME_H
