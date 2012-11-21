//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FFreeListTrashArray.h
//
//  AUTHOR:  Soren Johnson
//
//  PURPOSE: A dynamic array with a free list that keeps track of its own memory...
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FFREELISTTRASHARRAY_H
#define		FFREELISTTRASHARRAY_H
#pragma		once

#include	"FFreeListArrayBase.h"

#define FLTA_ID_SHIFT		(13)
#define FLTA_MAX_BUCKETS	(1 << FLTA_ID_SHIFT)
#define FLTA_INDEX_MASK		(FLTA_MAX_BUCKETS - 1)
#define FLTA_ID_MASK		(~(FLTA_INDEX_MASK))
#define FLTA_GROWTH_FACTOR	(2)

template <class T>
class FFreeListTrashArray : public FFreeListArrayBase<T>
{
public:

	FFreeListTrashArray();
	virtual ~FFreeListTrashArray();
								
	virtual void Init(int iNumSlots = 8);
	virtual void Uninit();
	virtual T* GetAt(int iID);
	virtual const T* GetAt(int iID) const;

	T* Add();
	bool Remove(T* pData);
	bool RemoveAt(int iID);
	virtual void RemoveAll();

	void Load(T* pData);

	int GetNumSlots() const
	{
		return this->m_iNumSlots;
	}

	int GetLastIndex() const
	{
		return this->m_iLastIndex;
	}
	void SetLastIndex(int iNewValue)
	{
		this->m_iLastIndex = iNewValue;
	}

	int GetFreeListHead() const
	{
		return this->m_iFreeListHead;
	}
	void SetFreeListHead(int iNewValue)
	{
		this->m_iFreeListHead = iNewValue;
	}

	int GetFreeListCount() const
	{
		return this->m_iFreeListCount;
	}
	void SetFreeListCount(int iNewValue)
	{
		this->m_iFreeListCount = iNewValue;
	}

	int GetCurrentID() const
	{
		return m_iCurrentID;
	}
	void SetCurrentID(int iNewValue)
	{
		assert((iNewValue & FLTA_INDEX_MASK) == 0);
		assert((iNewValue & FLTA_ID_MASK) != 0);
		m_iCurrentID = iNewValue;
	}

	int GetNextFreeIndex(int iIndex) const
	{
		if ((iIndex >= GetNumSlots()) || (m_pArray == NULL))
		{
			assert(false);
			return FFreeList::INVALID_INDEX;
		}
		return m_pArray[iIndex].iNextFreeIndex;
	}
	void SetNextFreeIndex(int iIndex, int iNewValue)
	{
		if ((iIndex >= GetNumSlots()) || (m_pArray == NULL))
		{
			assert(false);
			return;
		}
		m_pArray[iIndex].iNextFreeIndex = iNewValue;
	}

	void Read( FDataStream* pStream );
	void Write( FDataStream* pStream ) const;

protected:

	struct FFreeListTrashArrayNode
	{
		int iNextFreeIndex;
		T* pData;
	};

	int m_iCurrentID;
	FFreeListTrashArrayNode* m_pArray;

	virtual void GrowArray();
};



// Public functions...

template <class T>
FFreeListTrashArray<T>::FFreeListTrashArray()
{
	m_iCurrentID = FLTA_MAX_BUCKETS;
	m_pArray = NULL;
}


template <class T>
FFreeListTrashArray<T>::~FFreeListTrashArray()
{
	Uninit();
}


template <class T>
void FFreeListTrashArray<T>::Init(int iNumSlots)
{
	int iCount;
	int iI;

	assert(iNumSlots >= 0);

	// make sure it's binary...
	if ((iNumSlots > 0) && ((iNumSlots - 1) & iNumSlots) != 0)
	{
		// find high bit
		iCount = 0;
		while (iNumSlots != 1)
		{
			iNumSlots >>= 1;
			iCount++;
		}
		iNumSlots = (1 << (iCount + 1));
	}

	assert(((iNumSlots - 1) & iNumSlots) == 0);
	assert((this->m_iNumSlots <= FLTA_MAX_BUCKETS) && "FFreeListTrashArray<T>::init() size too large");

	Uninit();

	this->m_iNumSlots = iNumSlots;
	this->m_iLastIndex = FFreeList::INVALID_INDEX;
	this->m_iFreeListHead = FFreeList::INVALID_INDEX;
	this->m_iFreeListCount = 0;
	m_iCurrentID = FLTA_MAX_BUCKETS;

	if (this->m_iNumSlots > 0)
	{
		m_pArray = FNEW( FFreeListTrashArrayNode[this->m_iNumSlots], c_eMPoolTypeContainer, 0 );

		for (iI = 0; iI < this->m_iNumSlots; iI++)
		{
			m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
			m_pArray[iI].pData = NULL;
		}
	}
}


template <class T>
void FFreeListTrashArray<T>::Uninit()
{
	if (m_pArray != NULL)
	{
		RemoveAll();

		SAFE_DELETE_ARRAY(m_pArray);
	}
}


template <class T>
T* FFreeListTrashArray<T>::Add()
{
	int iIndex;

	if (m_pArray == NULL) 
	{
		Init();
	}

	if ((this->m_iLastIndex == this->m_iNumSlots - 1) &&
		(this->m_iFreeListCount == 0))
	{
		if ((this->m_iNumSlots * FLTA_GROWTH_FACTOR) > FLTA_MAX_BUCKETS)
		{
			return NULL;
		}

		GrowArray();
	}

	if (this->m_iFreeListCount > 0)
	{
		iIndex = this->m_iFreeListHead;
		this->m_iFreeListHead = m_pArray[this->m_iFreeListHead].iNextFreeIndex;
		this->m_iFreeListCount--;
	}
	else
	{
		this->m_iLastIndex++;
		iIndex = this->m_iLastIndex;
	}

	m_pArray[iIndex].pData = FNEW( T, c_eMPoolTypeContainer, 0 );
	m_pArray[iIndex].iNextFreeIndex = FFreeList::INVALID_INDEX;

	m_pArray[iIndex].pData->SetID(m_iCurrentID + iIndex);
	m_iCurrentID += FLTA_MAX_BUCKETS;

	return m_pArray[iIndex].pData;
}


template <class T>
T* FFreeListTrashArray<T>::GetAt(int iID)
{
	int iIndex;

	if ((iID == FFreeList::INVALID_INDEX) || (m_pArray == NULL))
	{
		return NULL;
	}

	iIndex = (iID & FLTA_INDEX_MASK);

	assert(iIndex >= 0);

	if ((iIndex <= this->m_iLastIndex) && 
		(m_pArray[iIndex].pData != NULL))
	{
		if (((iID & FLTA_ID_MASK) == 0) || (m_pArray[iIndex].pData->GetID() == iID))
		{
			return m_pArray[iIndex].pData;
		}
	}

	return NULL;
}

template <class T>
const T* FFreeListTrashArray<T>::GetAt(int iID) const
{
	int iIndex;

	if ((iID == FFreeList::INVALID_INDEX) || (m_pArray == NULL))
	{
		return NULL;
	}

	iIndex = (iID & FLTA_INDEX_MASK);

	assert(iIndex >= 0);

	if ((iIndex <= this->m_iLastIndex) && 
		(m_pArray[iIndex].pData != NULL))
	{
		if (((iID & FLTA_ID_MASK) == 0) || (m_pArray[iIndex].pData->GetID() == iID))
		{
			return m_pArray[iIndex].pData;
		}
	}

	return NULL;
}

template <class T>
bool FFreeListTrashArray<T>::Remove(T* pData)
{
	int iI;

	assert(m_pArray != NULL);

	if (pData != NULL)
	{
		for (iI = 0; iI <= this->m_iLastIndex; iI++)
		{
			if (m_pArray[iI].pData == pData)
			{
				return RemoveAt(iI);
			}
		}
	}

	return false;
}


template <class T>
bool FFreeListTrashArray<T>::RemoveAt(int iID)
{
	int iIndex;

	if ((iID == FFreeList::INVALID_INDEX) || (m_pArray == NULL))
	{
		return false;
	}

	iIndex = (iID & FLTA_INDEX_MASK);

	assert(iIndex >= 0);

	if ((iIndex <= this->m_iLastIndex) && 
		(m_pArray[iIndex].pData != NULL))
	{
		if (((iID & FLTA_ID_MASK) == 0) || (m_pArray[iIndex].pData->GetID() == iID))
		{
			delete m_pArray[iIndex].pData;
			m_pArray[iIndex].pData = NULL;

			m_pArray[iIndex].iNextFreeIndex = this->m_iFreeListHead;
			this->m_iFreeListHead = iIndex;
			this->m_iFreeListCount++;

			return true;
		}
		else
		{
			assert(false);
		}
	}

	return false;
}


template <class T>
void FFreeListTrashArray<T>::RemoveAll()
{
	int iI;

	if (m_pArray == NULL)
	{
		return;
	}

	this->m_iLastIndex = FFreeList::INVALID_INDEX;
	this->m_iFreeListHead = FFreeList::INVALID_INDEX;
	this->m_iFreeListCount = 0;

	for (iI = 0; iI < this->m_iNumSlots; iI++)
	{
		m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
		if (m_pArray[iI].pData != NULL)
		{
			delete m_pArray[iI].pData;
		}
		m_pArray[iI].pData = NULL;
	}
}


template <class T>
void FFreeListTrashArray<T>::Load(T* pData)
{
	assert(pData != NULL);
	assert(m_pArray != NULL);
	if (!pData || !m_pArray) return;

	assert((pData->GetID() & FLTA_ID_MASK) < m_iCurrentID);

	int iIndex = (pData->GetID() & FLTA_INDEX_MASK);

	assert(iIndex < FLTA_MAX_BUCKETS);
	assert(iIndex <= this->m_iLastIndex);
	assert(m_pArray[iIndex].pData == NULL);
	assert(m_pArray[iIndex].iNextFreeIndex == FFreeList::INVALID_INDEX);

	m_pArray[iIndex].pData = pData;
}

// Protected functions...

template <class T>
void FFreeListTrashArray<T>::GrowArray()
{
	FFreeListTrashArrayNode* pOldArray;
	int iOldNumSlots;
	int iI;

	assert(m_pArray != NULL);
	
	if(!m_pArray) return;

	pOldArray = m_pArray;
	iOldNumSlots = this->m_iNumSlots;

	this->m_iNumSlots *= FLTA_GROWTH_FACTOR;
	assert((this->m_iNumSlots <= FLTA_MAX_BUCKETS) && "FFreeListTrashArray<T>::growArray() size too large");
	m_pArray = FNEW( FFreeListTrashArrayNode[this->m_iNumSlots], c_eMPoolTypeContainer, 0 );

	for (iI = 0; iI < this->m_iNumSlots; iI++)
	{
		if (iI < iOldNumSlots)
		{
			m_pArray[iI] = pOldArray[iI];
		}
		else
		{
			m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
			m_pArray[iI].pData = NULL;
		}
	}

	delete [] pOldArray;
}

//
// use when list contains non-streamable types
//
template < class T >
inline void FFreeListTrashArray< T >::Read( FDataStream* pStream )
{
	int iTemp;
	pStream->Read( &iTemp );
	Init( iTemp );
	pStream->Read( &iTemp );
	SetLastIndex( iTemp );
	pStream->Read( &iTemp );
	SetFreeListHead( iTemp );
	pStream->Read( &iTemp );
	SetFreeListCount( iTemp );
	pStream->Read( &iTemp );
	SetCurrentID( iTemp );

	int i;

	for ( i = 0; i < GetNumSlots(); i++ )
	{
		pStream->Read( &iTemp );
		SetNextFreeIndex( i, iTemp );
	}

	int iCount;
	pStream->Read( &iCount );

	for ( i = 0; i < iCount; i++ )
	{
		T* pData = FNEW( T, c_eMPoolTypeContainer, 0 );
		pStream->Read( sizeof ( T ), ( byte* )pData );
		Load( pData );
	}
}

template < class T >
inline void FFreeListTrashArray< T >::Write( FDataStream* pStream ) const
{
	pStream->Write( GetNumSlots() );
	pStream->Write( GetLastIndex() );
	pStream->Write( GetFreeListHead() );
	pStream->Write( GetFreeListCount() );
	pStream->Write( GetCurrentID() );

	int i;

	for ( i = 0; i < GetNumSlots(); i++ )
	{
		pStream->Write( GetNextFreeIndex( i ) );
	}

	pStream->Write( FFreeListTrashArray<T>::GetCount() );

	for ( i = 0; i < FFreeListTrashArray<T>::GetIndexAfterLast(); i++ )
	{
		if ( GetAt( i ) )
		{
			pStream->Write( sizeof ( T ), ( byte* )GetAt( i ) );
		}
	}
}

//-------------------------------
// Serialization helper templates:
//-------------------------------

//
// use when list contains streamable types
//
template < class T >
inline void ReadStreamableFFreeListTrashArray( FFreeListTrashArray< T >& flist, FDataStream* pStream )
{
	int iTemp;
	pStream->Read( &iTemp );
	flist.Init( iTemp );
	pStream->Read( &iTemp );
	flist.SetLastIndex( iTemp );
	pStream->Read( &iTemp );
	flist.SetFreeListHead( iTemp );
	pStream->Read( &iTemp );
	flist.SetFreeListCount( iTemp );
	pStream->Read( &iTemp );
	flist.SetCurrentID( iTemp );

	int i;

	for ( i = 0; i < flist.GetNumSlots(); i++ )
	{
		pStream->Read( &iTemp );
		flist.SetNextFreeIndex( i, iTemp );
	}

	int iCount;
	pStream->Read( &iCount );

	for ( i = 0; i < iCount; i++ )
	{
		T* pData = FNEW( T, c_eMPoolTypeContainer, 0 );
		pData->Read( pStream );
		flist.Load( pData );
	}
}

template < class T >
inline void WriteStreamableFFreeListTrashArray( FFreeListTrashArray< T >& flist, FDataStream* pStream )
{
	pStream->Write( flist.GetNumSlots() );
	pStream->Write( flist.GetLastIndex() );
	pStream->Write( flist.GetFreeListHead() );
	pStream->Write( flist.GetFreeListCount() );
	pStream->Write( flist.GetCurrentID() );

	int i;

	for ( i = 0; i < flist.GetNumSlots(); i++ )
	{
		pStream->Write( flist.GetNextFreeIndex( i ) );
	}

	pStream->Write( flist.GetCount() );

	for ( i = 0; i < flist.GetIndexAfterLast(); i++ )
	{
		if ( flist[ i ] )
		{
			flist[ i ]->Write( pStream );
		}
	}
}


template<typename T>
FDataStream & operator<<(FDataStream & writeTo, const FFreeListTrashArray<T> & readFrom)
{
	writeTo << readFrom.GetNumSlots();
	writeTo << readFrom.GetLastIndex();
	writeTo << readFrom.GetFreeListHead();
	writeTo << readFrom.GetFreeListCount();
	writeTo << readFrom.GetCurrentID();

	int i;

	for ( i = 0; i < readFrom.GetNumSlots(); i++ )
	{
		writeTo << readFrom.GetNextFreeIndex( i );
	}

	writeTo << readFrom.GetCount();

	for ( i = 0; i < readFrom.GetIndexAfterLast(); i++ )
	{
		if ( readFrom[ i ] )
		{
			const T & entry = *(readFrom[i]);
			writeTo << entry;
		}
	}
	return writeTo;
}

template<typename T>
FDataStream & operator>>(FDataStream & readFrom, FFreeListTrashArray<T> & writeTo)
{
	int iTemp;
	readFrom >> iTemp;
	writeTo.Init( iTemp );
	readFrom >> iTemp;
	writeTo.SetLastIndex(iTemp);
	readFrom >> iTemp;
	writeTo.SetFreeListHead(iTemp);
	readFrom >> iTemp;
	writeTo.SetFreeListCount(iTemp);
	readFrom >> iTemp;
	writeTo.SetCurrentID(iTemp);

	int i;

	for ( i = 0; i < writeTo.GetNumSlots(); i++ )
	{
		readFrom >> iTemp;
		writeTo.SetNextFreeIndex(i, iTemp);
	}

	int iCount;
	readFrom >> iCount;

	for ( i = 0; i < iCount; i++ )
	{
		T* pData = new T;
		readFrom >> *pData;
		writeTo.Load(pData);
	}
	return readFrom;
}

#endif	//	FFREELISTTRASHARRAY_H

