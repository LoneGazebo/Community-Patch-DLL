//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FFreeListArrayBase.h
//
//  AUTHOR:  Mustafa Thamer
//
//  PURPOSE: abstract base class for FFreeListArray and FFreeListTrashArray
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FFREELISTARRAYBASE_H
#define		FFREELISTARRAYBASE_H
#pragma		once

namespace FFreeList
{
	enum
	{
		INVALID_INDEX	= -1,
		FREE_LIST_INDEX	= -2
	};
};

template <class T>
class FFreeListArrayBase
{
public:

	FFreeListArrayBase();
	virtual ~FFreeListArrayBase() {}
	virtual void Init(int iNumSlots = 8) = 0;
	virtual void Uninit() = 0;
	virtual T* GetAt(int iIndex) = 0;
	virtual const T* GetAt(int iIndex) const = 0;

	T* operator[]( int iIndex );
	const T * operator[](int index) const;

	// start at the beginning of the list and return the first item or NULL when done
	T* BeginIter(int* pIterIdx);

	// iterate from the current position and return the next item found or NULL when done
	T* NextIter(int* pIterIdx);

	// start at the end of the list and return the last item or NULL when done
	T* EndIter(int* pIterIdx);

	// iterate from the current position and return the prev item found or NULL when done
	T* PrevIter(int* pIterIdx);

	// Returns the iIndex after the last iIndex in the array containing an element
	int GetIndexAfterLast()	{	return m_iLastIndex + 1;	}

	// start at the beginning of the list and return the first item or NULL when done
	const T* BeginIter(int* pIterIdx) const;

	// iterate from the current position and return the next item found or NULL when done
	const T* NextIter(int* pIterIdx) const;

	// start at the end of the list and return the last item or NULL when done
	const T* EndIter(int* pIterIdx) const;

	// iterate from the current position and return the prev item found or NULL when done
	const T* PrevIter(int* pIterIdx) const;

	// Returns the iIndex after the last iIndex in the array containing an element
	int GetIndexAfterLast()	const {	return m_iLastIndex + 1;	}

	// Returns the number of elements in the array (NOTE: this is a non-packed array, so this value is NOT the last iIndex in the array...)
	int GetCount()	const {	return m_iLastIndex - m_iFreeListCount + 1;	}

	virtual void RemoveAll() = 0;
protected:
	int m_iFreeListHead;
	int m_iFreeListCount;
	int m_iLastIndex;
	int m_iNumSlots;

	virtual void GrowArray() = 0;
};

template <class T>
FFreeListArrayBase<T>::FFreeListArrayBase()
{
	m_iFreeListHead = FFreeList::FREE_LIST_INDEX;
	m_iFreeListCount = 0;
	m_iLastIndex = FFreeList::INVALID_INDEX;
	m_iNumSlots = 0;
}


//
// operators
//

template < class T >
inline T* FFreeListArrayBase< T >::operator[]( int iIndex )
{
	return ( GetAt( iIndex ) );
}

template < class T >
inline const T* FFreeListArrayBase< T >::operator[]( int iIndex ) const
{
	return ( GetAt( iIndex ) );
}

//
// iteration functions
//

// start at the beginning of the list and return the first item or NULL when done
template <class T>
T* FFreeListArrayBase<T>::BeginIter(int* pIterIdx)
{
	*pIterIdx = 0;
	return NextIter(pIterIdx);
}

// iterate from the current position and return the next item found or NULL when done
template <class T>
T* FFreeListArrayBase<T>::NextIter(int* pIterIdx)
{
	for( ; (*pIterIdx)<GetIndexAfterLast(); (*pIterIdx)++)
	{
		T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)++;	// prime for next call
			return pObj;
		}
	}
	return NULL;
}

// start at the end of the list and return the last item or NULL when done
template <class T>
T* FFreeListArrayBase<T>::EndIter(int* pIterIdx)
{
	*pIterIdx = GetIndexAfterLast()-1;
	return PrevIter(pIterIdx);
}

// iterate from the current position and return the prev item found or NULL when done
template <class T>
T* FFreeListArrayBase<T>::PrevIter(int* pIterIdx)
{
	for( ; (*pIterIdx)>=0; (*pIterIdx)--)
	{
		T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)--;	// prime for next call
			return pObj;
		}
	}
	return NULL;
}

/***************************************/

// start at the beginning of the list and return the first item or NULL when done
template <class T>
const T* FFreeListArrayBase<T>::BeginIter(int* pIterIdx) const
{
	*pIterIdx = 0;
	return NextIter(pIterIdx);
}

// iterate from the current position and return the next item found or NULL when done
template <class T>
const T* FFreeListArrayBase<T>::NextIter(int* pIterIdx) const
{
	for( ; (*pIterIdx)<GetIndexAfterLast(); (*pIterIdx)++)
	{
		const T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)++;	// prime for next call
			return pObj;
		}
	}
	return NULL;
}

// start at the end of the list and return the last item or NULL when done
template <class T>
const T* FFreeListArrayBase<T>::EndIter(int* pIterIdx) const
{
	*pIterIdx = GetIndexAfterLast()-1;
	return PrevIter(pIterIdx);
}

// iterate from the current position and return the prev item found or NULL when done
template <class T>
const T* FFreeListArrayBase<T>::PrevIter(int* pIterIdx) const
{
	for( ; (*pIterIdx)>=0; (*pIterIdx)--)
	{
		const T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)--;	// prime for next call
			return pObj;
		}
	}
	return NULL;
}


#endif	//	FFREELISTARRAYBASE_H

