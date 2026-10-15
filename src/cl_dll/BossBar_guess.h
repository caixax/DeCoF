// ======================================
// Cry of Fear boss health bar header file
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _BOSSBAR_H
#define _BOSSBAR_H
using namespace vgui;

class CBossBar : public Panel
{
public:
	CBossBar();
	~CBossBar();
	void Initialize();

	virtual void solve();

public:
	int		guess_m_iState;				// 0xbc, 0 hidden, 1 shown, 2 hidden on the next solve
	char	guess_m_szEmptyImage[128];	// 0xc0
	char	guess_m_szFullImage[128];	// 0x140
	int		guess_m_iHealth;			// 0x1c0
	int		guess_m_iMaxHealth;			// 0x1c4
	int		guess_m_iUnused1C8;			// 0x1c8, only cleared in 1.6a
	Image	*guess_m_pFullImage;		// 0x1cc, gfx/vgui/boss/doctor_health_full.tga
	Image	*guess_m_pEmptyImage;		// 0x1d0, gfx/vgui/boss/doctor_health_empty.tga
	ImagePanel	*guess_m_pFull;			// 0x1d4
	ImagePanel	*guess_m_pEmpty;		// 0x1d8
	ImagePanel	*guess_m_pBar;			// 0x1dc, cut to the health fraction in solve
};

void guess_BossBarInit();

#endif // _BOSSBAR_H
