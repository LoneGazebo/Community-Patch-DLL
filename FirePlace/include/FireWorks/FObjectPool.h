//	$Revision: #4 $		$Author: mbreitkreutz $ 	$DateTime: 2005/06/13 13:35:55 $
//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FObjectPool.h
//
//  AUTHOR:  Dan McGarry  --  09/02/2003
//
//  PURPOSE: Object Pool template
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FOBJECTPOOL_H
#define		FOBJECTPOOL_H
#pragma		once

#include	"FCriticalSection.h"

#define DEFAULT_FOBJECT_POOL_SIZE 8

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:      FQueue
//
//  PURPOSE:    Pool of pre-allocated objects to avoid repeated allocations in loops
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<class T>
class FObjectPool {
public:
	/* Constructors */
	// Default constructor
	FObjectPool( uint uiSize = DEFAULT_FOBJECT_POOL_SIZE, bool bGrow = true );

	// Copy constructor
	FObjectPool(const FObjectPool<T>& source);

	// Assignment operator
	FObjectPool<T>& operator=(const FObjectPool<T>& source);

	// Destructor
	virtual ~FObjectPool();

	// Get a free object pointer
	T* GetFreeObject();

	// Release a pointer to an object that was being used
	void Release(T* pObject);

protected:

	// Advance the free object pointer
	uint GetNextFreeIndex();
	
	// Thread safety
	void Lock()
		{ m_Locker.Enter(); }
	void Unlock()
		{ m_Locker.Leave(); }

	// The representative structure
	struct FPoolNode
	{
		T* pObject;	// The pre-allocated object pointer
		bool bFree;	// Flag indicating whether object is being used
	};

	// Array of objects
	FPoolNode * m_pStorage;

	// Thread safety lock
	FCriticalSection m_Locker;

	// Initialization parameters
	uint m_uiSize;
	bool m_bGrow;

	// Flag indicating a full array
	bool m_bFull;

	// First free pointer's index in storage
	uint m_uiFirstFreeIndex;
};

//---------------------------------------------------------------------------------------
//  FUNCTION:   FObjectPool( uint uiSize = DEFAULT_POOL_SIZE, bool bGrow = true )
//
//  DESC:       The default constructor, accepts optional initial size
//---------------------------------------------------------------------------------------
template<class T> 
FObjectPool<T>::FObjectPool( uint uiSize, bool bGrow )
{
	// Allocate the array memory
	m_pStorage = FNEW( FPoolNode[uiSize], c_eMPoolTypeContainer, 0 );

	#ifndef		_NDS
	if (!m_pStorage)
		throw(-1);
	#endif	//	!_NDS

	// Now pre-allocated each object in that array
	for (uint i = 0; i < uiSize; ++i)
	{
		m_pStorage[i].pObject = FNEW( T, c_eMPoolTypeContainer, 0 );
		m_pStorage[i].bFree = true;
	}

	m_uiSize = uiSize;
	m_bGrow = bGrow;
	m_bFull = false;
	m_uiFirstFreeIndex = 0;
}

//---------------------------------------------------------------------------------------
//  FUNCTION:   FObjectPool( const FObjectPool<T>& source )
//
//  DESC:       The default constructor, accepts optional initial size
//---------------------------------------------------------------------------------------
template<class T> 
FObjectPool<T>::FObjectPool( const FObjectPool<T>& source )
{
	source.Lock(); 
	
	// Allocate the array memory
	m_pStorage = FNEW( T(source.m_uiSize * sizeof(FPoolNode)), c_eMPoolTypeContainer, 0 );

	#ifndef		_NDS
	if (!m_pStorage)
		throw(-1);
	#endif	//	!_NDS

	// Pre-allocate and set each object in the array
	for (int i = 0; i < source.m_uiSize; ++i)
	{
		m_pStorage[i].pObject = FNEW( T( *(source.m_pStorage[i].pObject) ), c_eMPoolTypeContainer, 0 );
		m_pStorage[i].bFree = source.m_pStorage[i].bFree;
	}
	
	m_uiSize = source.m_uiSize;
	m_bGrow = source.m_bGrow;
	m_bFull = source.m_bFull;
	m_uiFirstFreeIndex = source.m_uiFirstFreeIndex;

	source.Unlock();
}

//---------------------------------------------------------------------------------------
//  FUNCTION:   operator=( const FObjectPool &source )
//
//  DESC:       The assignment operator
//---------------------------------------------------------------------------------------
template<class T> 
FObjectPool<T>& FObjectPool<T>::operator=( const FObjectPool<T>& source )
{
	// Lock for thread safety
	Lock();
	source.Lock();

	// Reallocate the array memory if necessary
	if (m_uiSize < source.m_uiSize)
	{
		FPoolNode *pOldStorage = m_pStorage;
		m_pStorage = FNEW( FPoolNode[ source.m_uiSize ], c_eMPoolTypeFireWorks, 0);
		memmove( m_pStorage, pOldStorage, sizeof( FPoolNode ) * m_uiSize );
		SAFE_DELETE_ARRAY( pOldStorage );
	}

	#ifndef		_NDS
	if (!m_pStorage)
		throw(-1);
	#endif	//	!_NDS

	// Pre-allocate and set each object in the array
	for (int i = 0; i < source.m_uiSize; ++i)
	{
		// Allocate if necessary
		if (i >= m_uiSize)
			m_pStorage[i].pObject = FNEW( T(), c_eMPoolTypeContainer, 0 );
        
		// Set the data
		m_pStorage[i].pObject = source.m_pStorage[i].pObject;
		m_pStorage[i].bFree = source.m_pStorage[i].bFree;
	}

	m_uiSize = source.m_uiSize;
	m_bGrow = source.m_bGrow;
	m_bFull = source.m_bFull;
	m_uiFirstFreeIndex = source.m_uiFirstFreeIndex;

	source.Unlock();
	Unlock();
}

//---------------------------------------------------------------------------------------
//  FUNCTION:   ~FObjectPool( void )
//
//  DESC:       The destructor
//---------------------------------------------------------------------------------------
template<class T>
FObjectPool<T>::~FObjectPool()
{
	Lock(); 

	// Get rid of each allocated object
	for (uint i = 0; i < m_uiSize; ++i)
	{
		if (m_pStorage[i].pObject)
			delete m_pStorage[i].pObject;
	}

	// Get rid of the array
	SAFE_DELETE_ARRAY(m_pStorage);
	Unlock();
}

//---------------------------------------------------------------------------------------
//  FUNCTION:   GetFreeObject( )
//
//  DESC:       Get a pointer to free object space - expand if necessary (and permitted)
//
//	RETURN:		Pointer to a free object, or NULL if none available
//---------------------------------------------------------------------------------------
template<class T> 
T* FObjectPool<T>::GetFreeObject( )
{
	Lock();

	T* pReturnObj = NULL;

	// Do we have any allocated objects not being used?
	if (m_bFull)
	{
		// If we have no available objects, see if we should grow
		if (m_bGrow)
		{
			// We want to grow - double the size of the storage and pre-allocate all the objects
			// (This is where we eat it to save time later)
			FPoolNode *pOldStorage = m_pStorage;
			m_pStorage = FNEW( FPoolNode[ m_uiSize * 2 ], c_eMPoolTypeFireWorks, 0);
			memmove( m_pStorage, pOldStorage, sizeof( FPoolNode ) * m_uiSize );
			SAFE_DELETE_ARRAY( pOldStorage );

			// If we get a memory error, bad bad things will happen.  Just bail
			#ifndef		_NDS
			if (!m_pStorage)
				throw(-1);
			#endif	//	!_NDS

			// Pre-allocate and set each object in the array
			for (uint i = m_uiSize; i < (2*m_uiSize); ++i)
			{
				m_pStorage[i].pObject = FNEW( T, c_eMPoolTypeContainer, 0 );
				m_pStorage[i].bFree = true;
			}

			m_uiFirstFreeIndex = m_uiSize;
			m_uiSize *= 2;
			m_bFull = false;
		}
	}

	// If we have free objects, give one to the caller
	if (!m_bFull)
	{
		// Set the free pointer
		pReturnObj = m_pStorage[m_uiFirstFreeIndex].pObject;
		m_pStorage[m_uiFirstFreeIndex].bFree = false;

		m_uiFirstFreeIndex = GetNextFreeIndex();
	}

	Unlock();

	return pReturnObj;
}

//---------------------------------------------------------------------------------------
//  FUNCTION:   Release( T* pObject )
//
//  DESC:       Release control over a object in storage
//
//	RETURN:		Pointer to a free object, or NULL if none available
//---------------------------------------------------------------------------------------
template<class T> 
void FObjectPool<T>::Release( T* pObject )
{
	// Thread-safety
	Lock();

	// Find this pointer -> can we do better than linear search here?
	for (uint i = 0; i < m_uiSize; ++i)
	{
		if (m_pStorage[i].pObject == pObject)
		{
			// We found it
			m_pStorage[i].bFree = true;
			m_bFull = false;

			// See if we can move the first available index
			if (i < m_uiFirstFreeIndex)
				m_uiFirstFreeIndex = i;

			break;
		}
	}

	Unlock();
}

//---------------------------------------------------------------------------------------
//  FUNCTION:   GetNextFreeIndex( )
//
//  DESC:       Return the index of the next unused object
//
//	RETURN:		Index to free object, or array-size if full
//---------------------------------------------------------------------------------------
template<class T>
uint FObjectPool<T>::GetNextFreeIndex( )
{
	// Find the next free index
	m_bFull = true;
	for (; m_uiFirstFreeIndex < m_uiSize; ++m_uiFirstFreeIndex)
	{
		if (m_pStorage[m_uiFirstFreeIndex].bFree)
		{
			m_bFull = false;
			break;
		}
	}

	return m_uiFirstFreeIndex;
}

#endif	//FOBJECTPOOL_H
