// ======================================
// Cry of Fear telescope scope panel header file
// (class name from the RTTI of client.dll, file name guessed)
// ======================================

#ifndef _SCOPE_H
#define _SCOPE_H
using namespace vgui;

class CScope : public Panel
{
public:
	CScope();
	void Initialize();

public:
	int		guess_m_iScope;				// 0xbc, Scope message
	Image	*guess_m_pScopeImage;		// 0xc0, gfx/vgui/telescope_scope.tga
	ImagePanel	*guess_m_pScopePanel;	// 0xc4
};

void guess_ScopeInit();

#endif // _SCOPE_H
