//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: Creates a HTML control
//
// $NoKeywords: $
//=============================================================================

#ifndef HTML_H
#define HTML_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/Panel.h>

namespace vgui2
{

//-----------------------------------------------------------------------------
// Purpose: Control to display HTML content
//			(only the part of the class that other files of the
//			library use is declared here so far)
//-----------------------------------------------------------------------------
class HTML : public Panel
{
	DECLARE_CLASS_SIMPLE( HTML, Panel );

public:
	HTML(Panel *parent, const char *name, bool allowJavaScript = false);
	~HTML();
};

} // namespace vgui2

#endif // HTML_H
