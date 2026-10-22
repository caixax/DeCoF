//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: BuildGroup.cpp handles all the functionality for BuildMode
//
// $NoKeywords: $
//=============================================================================

#include <stdio.h>
#include <string.h>

#include <vgui/KeyValues.h>

#include <vgui_controls/BuildGroup.h>
#include <vgui_controls/Panel.h>

using namespace vgui2;

//-----------------------------------------------------------------------------
// Purpose: registers that a control settings file may be loaded
//			use when the dialog may have multiple states and the editor will need to be able to switch between them
//-----------------------------------------------------------------------------
void BuildGroup::RegisterControlSettingsFile(const char *controlResourceName, const char *pathID)
{
	// add the file into a list for build mode
	CUtlSymbol sym(controlResourceName);
	if (!m_RegisteredControlSettingsFiles.IsValidIndex(m_RegisteredControlSettingsFiles.Find(sym)))
	{
		m_RegisteredControlSettingsFiles.AddToTail(sym);
	}
}
