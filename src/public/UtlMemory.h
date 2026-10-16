//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: A growable memory class.
//
// $NoKeywords: $
//=============================================================================

#ifndef UTLMEMORY_H
#define UTLMEMORY_H

#ifdef _WIN32
#pragma once
#endif

#include <stdlib.h>

//-----------------------------------------------------------------------------
// The CUtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template< class T >
class CUtlMemory
{
public:
	// constructor, destructor
	CUtlMemory( int nGrowSize = 0, int nInitSize = 0 );
	CUtlMemory( T* pMemory, int numElements );
	~CUtlMemory();

	// element access
	T& operator[]( int i );
	T const& operator[]( int i ) const;
	T& Element( int i );
	T const& Element( int i ) const;

	// Can we use this index?
	bool IsIdxValid( int i ) const;

	// Gets the base address (can change when adding elements!)
	T* Base();
	T const* Base() const;

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, int numElements );

	// Size
	int NumAllocated() const;
	int Count() const;

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( int num = 1 );

	// Makes sure we've got at least this much memory
	void EnsureCapacity( int num );

	// Memory deallocation
	void Purge();

	// is the memory externally allocated?
	bool IsExternallyAllocated() const;

	// Set the size by which the memory grows
	void SetGrowSize( int size );

private:
	enum
	{
		EXTERNAL_BUFFER_MARKER = -1,
	};

	T* m_pMemory;
	int m_NumAllocated;
	int m_GrowSize;
};

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
template< class T >
CUtlMemory<T>::~CUtlMemory()
{
	Purge();
}

//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< class T >
inline T& CUtlMemory<T>::operator[]( int i )
{
	return m_pMemory[i];
}

//-----------------------------------------------------------------------------
// Gets the base address (can change when adding elements!)
//-----------------------------------------------------------------------------
template< class T >
inline T* CUtlMemory<T>::Base()
{
	return m_pMemory;
}

//-----------------------------------------------------------------------------
// is the memory externally allocated?
//-----------------------------------------------------------------------------
template< class T >
bool CUtlMemory<T>::IsExternallyAllocated() const
{
	return m_GrowSize == EXTERNAL_BUFFER_MARKER;
}

//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template< class T >
void CUtlMemory<T>::Purge()
{
	if (!IsExternallyAllocated())
	{
		if (m_pMemory)
		{
			free( (void*)m_pMemory );
			m_pMemory = 0;
		}
		m_NumAllocated = 0;
	}
}

#endif // UTLMEMORY_H
