//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: A dictionary mapping from symbol to structure
//
// $Header: $
// $NoKeywords: $
//=============================================================================

#ifndef UTLDICT_H
#define UTLDICT_H

#ifdef _WIN32
#pragma once
#endif

#include "UtlRBTree.h"
#include "utlsymbol.h"

//-----------------------------------------------------------------------------
// A dictionary mapping from symbol to structure
//-----------------------------------------------------------------------------
template <class T, class I>
class CUtlDict
{
public:
	// constructor, destructor
	// Left at growSize = 0, the memory will first allocate 1 element and double in size
	// at each increment.
	CUtlDict( bool caseInsensitive = true, int growSize = 0, int initSize = 0 );
	~CUtlDict( );

	// gets particular elements
	T&         Element( I i );
	const T&   Element( I i ) const;
	T&         operator[]( I i );
	const T&   operator[]( I i ) const;

	// gets element names
	char	   *GetElementName( I i );
	char const *GetElementName( I i ) const;

	void		SetElementName( I i, char const *pName );

	// Number of elements
	unsigned int Count() const;

	// Checks if a node is valid and in the tree
	bool  IsValidIndex( I i ) const;

	// Invalid index
	static I InvalidIndex();

	// Insert method (inserts in order)
	I  Insert( const char *pName, const T &element );

	// Find method
	I  Find( const char *pName ) const;

	// Remove methods
	void	RemoveAt( I i );
	void	Remove( const char *search );
	void	RemoveAll( );

	// Purge memory
	void	Purge();

	// Iteration methods
	I		First() const;
	I		Next( I i ) const;

protected:
	struct DictElement_t
	{
		CUtlSymbol	m_Name;
		T			m_Data;
	};

	static bool DictLessFunc( const DictElement_t &src1, const DictElement_t &src2 );
	typedef CUtlRBTree< DictElement_t, I > DictElementMap_t;
	DictElementMap_t m_Elements;
	mutable CUtlSymbolTable m_SymbolTable;
};

#endif // UTLDICT_H
