//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: Base classes for the career screens
//
// $NoKeywords: $
//=============================================================================

#ifndef CAREERBASEFRAME_H
#define CAREERBASEFRAME_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/Dar.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>

namespace vgui2
{
class IBorder;
class IImage;
class Label;
class TextImage;
};

//-----------------------------------------------------------------------------
// Purpose: Draws a gradient between two colors
//-----------------------------------------------------------------------------
class ColorFade
{
public:
	enum FadeDir
	{
		up,
		down,
		left,
		right
	};

	ColorFade();
	void SetFade( Color startColor, Color endColor, float fadePercent, FadeDir direction );
	void DrawFade( int x, int y, int w, int h );

private:
	Color m_startColor;
	Color m_endColor;
	float m_fadePercent;
	FadeDir m_direction;
};

//-----------------------------------------------------------------------------
// Purpose: A button with an image and text
//-----------------------------------------------------------------------------
class CCareerButton : public vgui2::Button
{
public:
	CCareerButton( vgui2::Panel *parent, const char *buttonName, const char *buttonText, const char *image, bool textFirst );

	virtual void SetArmedBorder( vgui2::IBorder *border );
	virtual void ApplySchemeSettings( vgui2::IScheme *pScheme );
	virtual void Paint();

protected:
	virtual vgui2::IBorder *GetBorder( bool depressed, bool armed, bool selected, bool keyfocus );

	vgui2::IBorder *m_armedBorder;
	vgui2::IImage *m_image;
	vgui2::TextImage *m_textImage;
	bool m_textFirst;
	int m_textPad;
	int m_imagePad;
	Color m_textNormalColor;
	Color m_textDisabledColor;
};

#endif // CAREERBASEFRAME_H
