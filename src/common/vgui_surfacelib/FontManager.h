//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: font handle management of the surface library (vgui_surfacelib,
//			linked into the engine as an optimized static library)
//
// $NoKeywords: $
//=============================================================================

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>

#ifdef CreateFont
#undef CreateFont
#endif

using vgui2::HFont;

//-----------------------------------------------------------------------------
// Purpose: Creates and maintains a set of fonts (only the methods the engine
//			calls are declared here; the library is not part of this build)
//-----------------------------------------------------------------------------
class CFontManager
{
public:
	HFont CreateFont();
	bool AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c);
	int GetFontTall(HFont font);
	int GetFontAscent(HFont font, wchar_t wch);
	int GetCharacterWidth(HFont font, int ch);
	bool GetFontUnderlined(HFont font);
	void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall);
};

// singleton accessor
extern CFontManager &FontManager();

#endif // FONTMANAGER_H
