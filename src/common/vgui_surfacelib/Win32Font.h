//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: a Windows GDI font of the surface library (vgui_surfacelib, linked
//			into the engine as an optimized static library); only the methods
//			the engine calls and the members they read are declared here
//
// $NoKeywords: $
//=============================================================================

#ifndef WIN32FONT_H
#define WIN32FONT_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: encapsulates a windows font
//-----------------------------------------------------------------------------
class CWin32Font
{
public:
	// writes the char into the specified 32bpp texture
	void GetCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba );

	// gets the abc widths for a character
	void GetCharABCWidths( int ch, int &a, int &b, int &c );

	// returns the height of the font, in pixels
	int GetHeight();

	// returns true if this font is underlined
	bool GetUnderlined() { return m_bUnderlined; }

private:
	char m_szName[32];
	int m_iTall;
	int m_iWeight;
	int m_iFlags;
	bool m_bAntiAliased;
	bool m_bRotary;
	bool m_bAdditive;
	int m_iDropShadowOffset;
	bool m_bUnderlined;
	int m_iHeight;
};

#endif // WIN32FONT_H
