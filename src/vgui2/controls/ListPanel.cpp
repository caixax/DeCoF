//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vgui/Cursor.h>
#include <vgui/IInput.h>
#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/KeyValues.h>
#include <vgui/MouseCode.h>

#include <vgui_controls/Controls.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/ImageList.h>

using namespace vgui2;

//-----------------------------------------------------------------------------
// Purpose: sets the image list used by image columns
//-----------------------------------------------------------------------------
void ListPanel::SetImageList(ImageList *imageList, bool deleteImageListWhenDone)
{
	// get rid of existing list image if there's one and we're supposed to get rid of it
	if ( m_pImageList && m_bDeleteImageListWhenDone )
	{
		delete m_pImageList;
	}

	m_bDeleteImageListWhenDone = deleteImageListWhenDone;
	m_pImageList = imageList;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ListPanel::SetColumnHeaderHeight( int height )
{
	m_iHeaderHeight = height;
}

//-----------------------------------------------------------------------------
// Purpose: adds a column header
//-----------------------------------------------------------------------------
void ListPanel::AddColumnHeader(int index, const char *columnName, const char *columnText, int width, int columnFlags)
{
	if (columnFlags & COLUMN_FIXEDSIZE && !(columnFlags & COLUMN_RESIZEWITHWINDOW))
	{
		// for fixed size columns, set the min & max widths to be the same as the default width
		AddColumnHeader(index, columnName, columnText, width, width, width, columnFlags);
	}
	else
	{
		AddColumnHeader(index, columnName, columnText, width, 20, 10000, columnFlags);
	}
}
