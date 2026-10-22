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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ListPanel::SetUserData( int itemID, unsigned int userData )
{
	if ( !m_DataItems.IsValidIndex(itemID) )
		return;

	m_DataItems[itemID]->userData = userData;
}

//-----------------------------------------------------------------------------
// Purpose: returns the number of VISIBLE items
//-----------------------------------------------------------------------------
int ListPanel::GetItemCount()
{
	return m_VisibleItems.Count();
}

//-----------------------------------------------------------------------------
// Purpose: returns -1 if invalid index or item not visible
//-----------------------------------------------------------------------------
int ListPanel::GetItemCurrentRow(int itemID)
{
	return m_VisibleItems.Find(itemID);
}

//-----------------------------------------------------------------------------
// Purpose: gets the item ID of an item by row (0 based)
//-----------------------------------------------------------------------------
int ListPanel::GetItemIDFromRow(int currentRow)
{
	if (!m_VisibleItems.IsValidIndex(currentRow))
		return -1;

	return m_VisibleItems[currentRow];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int ListPanel::InvalidItemID() const
{
	return m_DataItems.InvalidIndex();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool ListPanel::IsValidItemID(int itemID)
{
	return m_DataItems.IsValidIndex(itemID);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
ListPanelItem *ListPanel::GetItemData( int itemID )
{
	if ( !m_DataItems.IsValidIndex( itemID ) )
		return NULL;

	return m_DataItems[ itemID ];
}

//-----------------------------------------------------------------------------
// Purpose: returns user data for itemID
//-----------------------------------------------------------------------------
unsigned int ListPanel::GetItemUserData(int itemID)
{
	if ( !m_DataItems.IsValidIndex( itemID ) )
		return 0;

	return m_DataItems[itemID]->userData;
}

//-----------------------------------------------------------------------------
// Purpose: updates the view with any changes to the data
//-----------------------------------------------------------------------------
void ListPanel::ApplyItemChanges(int itemID)
{
	// reindex the item and then redraw
	IndexItem(itemID);
	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ListPanel::RereadAllItems()
{
	//!! need to make this more efficient
	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: returns the number of selected items
//-----------------------------------------------------------------------------
int ListPanel::GetSelectedItemsCount()
{
	return m_SelectedItems.Count();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int ListPanel::GetSelectedItem(int selectionIndex)
{
	if ( m_SelectedItems.IsValidIndex(selectionIndex))
		return m_SelectedItems[selectionIndex];

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int ListPanel::GetSelectedColumn()
{
	return m_iSelectedColumn;
}

//-----------------------------------------------------------------------------
// Purpose: sets no item as selected
//-----------------------------------------------------------------------------
void ListPanel::ClearSelectedItems()
{
	m_SelectedItems.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose: sets this single item as the only selected item
//-----------------------------------------------------------------------------
void ListPanel::SetSingleSelectedItem( int itemID )
{
	ClearSelectedItems();
	AddSelectedItem(itemID);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ListPanel::OnSizeChanged(int wide, int tall)
{
	BaseClass::OnSizeChanged(wide, tall);
	InvalidateLayout();
	Repaint();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ListPanel::PaintBackground()
{
	BaseClass::PaintBackground();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ListPanel::SetSortColumn(int column)
{
	m_iSortColumn = column;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ListPanel::SetSelectIndividualCells(bool state)
{
	m_bCanSelectIndividualCells = state;
}

//-----------------------------------------------------------------------------
// Purpose: returns true if this panel should save its settings
//-----------------------------------------------------------------------------
bool ListPanel::HasUserConfigSettings()
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void ListPanel::SetAllowUserModificationOfColumns(bool allowed)
{
	m_bAllowUserAddDeleteColumns = allowed;
}

//-----------------------------------------------------------------------------
// Purpose: iteration. Use these until they return InvalidItemID to iterate all the items.
//-----------------------------------------------------------------------------
int ListPanel::FirstItem() const
{
	return m_DataItems.Head();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int ListPanel::NextItem( int iItem ) const
{
	return m_DataItems.Next( iItem );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int ListPanel::GetNumColumnHeaders() const
{
	return m_CurrentColumns.Count();
}
