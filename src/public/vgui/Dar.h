//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: Holds the enumerated list of default cursors
//
// $NoKeywords: $
//=============================================================================

#ifndef DAR_H
#define DAR_H

#ifdef _WIN32
#pragma once
#endif

#include <stdlib.h>
#include <string.h>
#include <vgui/VGUI.h>

namespace vgui2
{

//-----------------------------------------------------------------------------
// Purpose: Simple lightweight dynamic array implementation
//-----------------------------------------------------------------------------
template<class ELEMTYPE> class Dar
{
public:
	Dar();
	Dar(int initialCapacity);
	~Dar();

public:
	void EnsureCapacity(int wantedCapacity);
	void SetCount(int count);
	int GetCount();
	int AddElement(ELEMTYPE elem);
	void MoveElementToEnd( ELEMTYPE elem );
	int FindElement(ELEMTYPE elem);
	bool HasElement(ELEMTYPE elem);
	int PutElement(ELEMTYPE elem);
	void InsertElementAt(ELEMTYPE elem,int index);
	void SetElementAt(ELEMTYPE elem,int index);
	void RemoveElementAt(int index);
	void RemoveElementsBefore(int index);
	void RemoveElement(ELEMTYPE elem);
	void RemoveAll();
	ELEMTYPE operator[](int index);
	void *GetBaseData();
	void CopyFrom(Dar<ELEMTYPE> &dar);

protected:
	int _count;
	int _capacity;
	ELEMTYPE *_data;
};

} // namespace vgui2

#endif // DAR_H
