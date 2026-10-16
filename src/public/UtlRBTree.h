//========= Copyright (c) 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $Header: $
// $NoKeywords: $
//=============================================================================

#ifndef UTLRBTREE_H
#define UTLRBTREE_H

#ifdef _WIN32
#pragma once
#endif

#include "UtlMemory.h"

//-----------------------------------------------------------------------------
// A red-black binary search tree
//-----------------------------------------------------------------------------
template <class T, class I = unsigned short>
class CUtlRBTree
{
public:
	// Less func typedef
	// Returns true if the first parameter is "less" than the second
	typedef bool (*LessFunc_t)( T const &, T const & );

	// constructor, destructor
	// Left at growSize = 0, the memory will first allocate 1 element and double in size
	// at each increment.
	// LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
	CUtlRBTree( int growSize = 0, int initSize = 0, LessFunc_t lessfunc = 0 );
	~CUtlRBTree();

	// gets particular elements
	T&		Element( I i );
	T const &Element( I i ) const;
	T&		operator[]( I i );
	T const &operator[]( I i ) const;

	// Gets the root
	I  Root() const;

	// Num elements
	unsigned int Count() const;

	// Max "size" of the vector
	I  MaxElement() const;

	// Gets the children
	I  Parent( I i ) const;
	I  LeftChild( I i ) const;
	I  RightChild( I i ) const;

	// Tests if a node is a left or right child
	bool  IsLeftChild( I i ) const;
	bool  IsRightChild( I i ) const;

	// Tests if root or leaf
	bool  IsRoot( I i ) const;
	bool  IsLeaf( I i ) const;

	// Checks if a node is valid and in the tree
	bool  IsValidIndex( I i ) const;

	// Checks if the tree as a whole is valid
	bool  IsValid() const;

	// Invalid index
	static I InvalidIndex();

	// returns the tree depth (not a very fast operation)
	int   Depth( I node ) const;
	int   Depth() const;

	// Sets the less func
	void SetLessFunc( LessFunc_t func );

	// Allocation method
	I  NewNode();

	// Insert method (inserts in order)
	I  Insert( T const &insert );
	void Insert( const T *pArray, int nItems );

	// Find method
	I  Find( T const &search ) const;

	// Remove methods
	void     RemoveAt( I i );
	bool     Remove( T const &remove );
	void     RemoveAll( );

	// Allocation, deletion
	void  FreeNode( I i );

	// Iteration
	I  FirstInorder() const;
	I  NextInorder( I i ) const;
	I  PrevInorder( I i ) const;
	I  LastInorder() const;

	I  FirstPreorder() const;
	I  NextPreorder( I i ) const;
	I  PrevPreorder( I i ) const;
	I  LastPreorder( ) const;

	I  FirstPostorder() const;
	I  NextPostorder( I i ) const;

	// If you change the search key, this can be used to reinsert the
	// element into the tree.
	void	Reinsert( I elem );

protected:
	enum NodeColor_t
	{
		RED = 0,
		BLACK
	};

	struct Links_t
	{
		I  m_Left;
		I  m_Right;
		I  m_Parent;
		I  m_Tag;
	};

	struct Node_t : public Links_t
	{
		T  m_Data;
	};

	// Sets the children
	void  SetParent( I i, I parent );
	void  SetLeftChild( I i, I child  );
	void  SetRightChild( I i, I child  );
	void  LinkToParent( I i, I parent, bool isLeft );

	// Gets at the links
	Links_t const &Links( I i ) const;
	Links_t &Links( I i );

	// Checks if a link is red or black
	bool IsRed( I i ) const;
	bool IsBlack( I i ) const;

	// Sets/gets node color
	NodeColor_t Color( I i ) const;
	void        SetColor( I i, NodeColor_t c );

	// operations required to preserve tree balance
	void RotateLeft(I i);
	void RotateRight(I i);
	void InsertRebalance(I i);
	void RemoveRebalance(I i);

	// Insertion, removal
	I  InsertAt( I parent, bool leftchild );

	// copy constructors not allowed
	CUtlRBTree( CUtlRBTree<T, I> const &tree );

	// Inserts a node into the tree, doesn't copy the data in.
	void FindInsertionPosition( T const &insert, I &parent, bool &leftchild );

	// Remove and add back an element in the tree.
	void	Unlink( I elem );
	void	Link( I elem );

	// Used for sorting.
	LessFunc_t m_LessFunc;

	CUtlMemory<Node_t> m_Elements;
	I m_Root;
	I m_NumElements;
	I m_FirstFree;
	I m_TotalElements;
};

#endif // UTLRBTREE_H
