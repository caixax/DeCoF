//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//
// A growable array class that maintains a free list and keeps elements
// in the same location
//=============================================================================

#ifndef UTLVECTOR_H
#define UTLVECTOR_H

#ifdef _WIN32
#pragma once
#endif

#include "UtlMemory.h"

//-----------------------------------------------------------------------------
// The CUtlVector class:
// A growable array class which doubles in size by default.
// It will always keep all elements consecutive in memory, and may move the
// elements around in memory (via a realloc) when elements are inserted or
// removed. Clients should therefore refer to the elements of the vector
// by index (they should *never* maintain pointers to elements in the vector).
//-----------------------------------------------------------------------------
template< class T >
class CUtlVector
{
public:
	typedef T ElemType_t;

	// constructor, destructor
	CUtlVector( int growSize = 0, int initSize = 0 );
	CUtlVector( T* pMemory, int numElements );
	~CUtlVector();

	// element access
	T& operator[]( int i );
	T const& operator[]( int i ) const;
	T& Element( int i );
	T const& Element( int i ) const;

	// Gets the base address (can change when adding elements!)
	T* Base();
	T const* Base() const;

	// Returns the number of elements in the vector
	// SIZE IS DEPRECATED!
	int Count() const;
	int Size() const;	// don't use me!

	// Is element index valid?
	bool IsValidIndex( int i ) const;

	// Adds an element, uses default constructor
	int AddToHead();
	int AddToTail();
	int InsertBefore( int elem );
	int InsertAfter( int elem );

	// Adds an element, uses copy constructor
	int AddToHead( T const& src );
	int AddToTail( T const& src );
	int InsertBefore( int elem, T const& src );
	int InsertAfter( int elem, T const& src );

	// Adds multiple elements, uses default constructor
	int AddMultipleToHead( int num );
	int AddMultipleToTail( int num );
	int InsertMultipleBefore( int elem, int num );
	int InsertMultipleAfter( int elem, int num );

	// Calls RemoveAll() then AddMultipleToTail.
	void SetSize( int size );

	// Calls SetSize and copies each element.
	void CopyArray( T const *pArray, int size );

	// Add the specified array to the tail.
	int AddVectorToTail( CUtlVector<T> const &src );

	// Finds an element (element needs operator== defined)
	int Find( T const& src ) const;

	bool HasElement( T const& src );

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity( int num );

	// Element removal
	void FastRemove( int elem );	// doesn't preserve order
	void Remove( int elem );		// preserves order, shifts elements
	void FindAndRemove( T const& src );	// removes first occurrence of src, preserves order, shifts elements
	void RemoveMultiple( int elem, int num );	// preserves order, shifts elements
	void RemoveAll();				// doesn't deallocate memory

	// Memory deallocation
	void Purge();

	// Set the size by which it grows when it needs to allocate more memory.
	void SetGrowSize( int size );

protected:
	// Can't copy this unless we explicitly do it!
	CUtlVector( CUtlVector const& vec );

	// Grows the vector
	void GrowVector( int num = 1 );

	// Shifts elements....
	void ShiftElementsRight( int elem, int num = 1 );
	void ShiftElementsLeft( int elem, int num = 1 );

	// For easier access to the elements through the debugger
	void ResetDbgInfo();

	CUtlMemory<T> m_Memory;
	int m_Size;

	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T *m_pElements;
};

#endif // UTLVECTOR_H
