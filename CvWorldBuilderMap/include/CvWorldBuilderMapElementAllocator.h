//---------------------------------------------------------------------------------------
//
//  *****************   CIV 5 World Builder Map   ********************
//
//  FILE:    CvWorldBuilderMapElementAllocator.h
//
//  AUTHOR:  Eric Jordan  --  4/20/2010
//
//  PURPOSE:
//		This little monstrosity was created to support the Civ 5 World Builder map format
//		and is used for Units and Cities.  It's sort of like a hybrid between a depot
//      allocator and a vector.  It's designed to minimize allocations and serialization
//      bloat.  While it's an interesting data structure it is very special case for the
//      world builder maps and probably shouldn't be used elsewhere without a good understanding
//      for how it works.
//
//		IMPORTANT: This data structure assumes T is POD.  It does not call copy ctors or dtors.
//		           It does call the default ctor to ensure proper data initialization.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#pragma once
#ifndef CvWorldBuilderMapElementAllocator_h
#define CvWorldBuilderMapElementAllocator_h

template<class T, uint TInitialSize>
class CvWorldBuilderMapElementAllocator
{
	// I am a friend to others like me
	template<class TOther, uint TOtherInitialSize> friend class CvWorldBuilderMapElementAllocator;

public:
	class Handle
	{
		// Only the allocator can give these an index
		friend CvWorldBuilderMapElementAllocator;

	public:
		// -1 will serve as our invalid index.  Since our index
		// type is unsigned this just means that it will be the
		// maximum (all bits 1)
		Handle() : m_wIndex(word(-1)) {}
		inline const bool Valid() const { return m_wIndex != word(-1); }

		// All handles created outside of the allocator are invalid.
		// This function is just for writing self-documenting code.
		inline static Handle InvalidHandle() { return Handle(word(-1)); }

		inline const bool operator ==(const Handle kRhs) const { return m_wIndex == kRhs.m_wIndex; }
		inline const bool operator !=(const Handle kRhs) const { return m_wIndex != kRhs.m_wIndex; }

		inline const word ToWord() const { return m_wIndex; }
		inline const int ToInt() const { return m_wIndex; }

	private:
		explicit Handle(word wIndex) : m_wIndex(wIndex) {}

		// Handles store indicies as words just because that's
		// what is most appropriate for the world builder map
		// format.  Otherwise it would make sense to do them as uint.
		// If I really meant for this to be a library level utility
		// then perhaps the index type would be a template parameter.
		word m_wIndex;
	};

	CvWorldBuilderMapElementAllocator() : m_aData(NULL), m_iFreeList(-1), m_uiSize(0) {}
	~CvWorldBuilderMapElementAllocator() { Release(); }

	void Release()
	{
		// Free allocated memory
		if( m_aData ) FFREE(m_aData);

		// Re-initialize data members.  (this also sets m_aData to NULL again)
		new(this)CvWorldBuilderMapElementAllocator();
	}

	const Handle Alloc()
	{
		// If there are no free elements then grow
		if( m_iFreeList < 0 )
		{
			if( !Grow((m_uiSize == 0)? TInitialSize : m_uiSize * 2) )
				return Handle::InvalidHandle(); // If we fail to grow then return an invalid handle
		}

		// Get the head of the free list
		const int iFree = m_iFreeList;

		// Update Free List
		m_iFreeList = m_aData[iFree].iNextFreeEntry;

		// Initialize data by calling the default ctor
		new(&(m_aData[iFree]))T();

		// Return a handle to the free element
		return Handle((word)iFree);
	}

	// Warning: No dtor called!
	void Free(Handle &h)
	{
		if( h.m_wIndex < m_uiSize )
		{
			// Simply add the element back to the free list.
			// Place it at the head since this is easiest.
			m_aData[h.m_wIndex].iNextFreeEntry = m_iFreeList;
			m_iFreeList = h.m_wIndex;
		}

		h.m_wIndex = (word)-1;
	}

	_Ret_opt_ inline T *operator[](Handle h)
	{
		if( h.m_wIndex < m_uiSize ) return (T*)&(m_aData[h.m_wIndex]);
		else return NULL;
	}

	_Ret_opt_ inline const T *operator[](Handle h) const
	{
		if( uint(h.m_wIndex) < m_uiSize ) return (const T*)&(m_aData[h.m_wIndex]);
		else return NULL;
	}

	uint GetSerializedSize() const
	{
		if( m_uiSize == 0 )
			return 0;
		else
			return (uint)(m_uiSize * sizeof(Entry) + sizeof(m_iFreeList));
	}

	// pvDest must point to a buffer that's at least as big as the value from
	// GetSerializationSize() which should be checked immediately before serialization
	void Serialize(void *pvDest) const
	{
		FAssert(pvDest);

		if( m_uiSize == 0 )
			return;

		if( pvDest != NULL )
		{
			memcpy(pvDest, &m_iFreeList, sizeof(m_iFreeList));
			memcpy((byte*)pvDest + sizeof(m_iFreeList), m_aData, m_uiSize * sizeof(Entry));
		}
	}

	void Deserialize(const void *pvBuffer, uint uiSize)
	{
		Release();

		if( pvBuffer == NULL || uiSize == 0 )
			return;

		memcpy(&m_iFreeList, pvBuffer, sizeof(m_iFreeList));

		const uint uiRemaning = (uint)(uiSize - sizeof(m_iFreeList));
		m_aData = (Entry*)FMALLOC(uiRemaning, c_eMPoolTypeGame, 0);
		FAssertMsg(m_aData != NULL, "Failed to allocate!");
		if( m_aData != NULL )
		{
			memcpy(m_aData, (byte*)pvBuffer + sizeof(m_iFreeList), uiRemaning);
			m_uiSize = uiRemaning / sizeof(Entry);
		}
		else
		{
			m_iFreeList = -1;
		}
	}

	// Use convert to when you have an element allocator with a legacy type and need to convert the data inside of it.
	// This is not mind blowingly efficient.  Then again, data migration rarely is.
	template<class TargetType, uint TargetInitialSize>
	void ConvertTo(CvWorldBuilderMapElementAllocator<TargetType, TargetInitialSize> &kTarget) const
	{
		typedef CvWorldBuilderMapElementAllocator<TargetType, TargetInitialSize>::Entry TargetEntry;

		kTarget.Release();

		if( m_aData != NULL && m_uiSize != 0 )
		{
			// Allocate the new memory block
			kTarget.m_aData = (TargetEntry*)FMALLOC(sizeof(TargetEntry) * m_uiSize, c_eMPoolTypeGame, 0);
			FAssertMsg(kTarget.m_aData != NULL, "Failed to allocate!");
			if( kTarget.m_aData == NULL )
				return;

			// Update the target's size and free list
			kTarget.m_iFreeList = m_iFreeList;
			kTarget.m_uiSize = m_uiSize;

			// Create a temporary array to notate which entries are free
			const uint uiFreeEntriesSize = m_uiSize * sizeof(bool);
			bool *abFreeEntries = (bool*)FTempHeapFastVectorAllocator::AllocAligned(uiFreeEntriesSize, 4, c_eMPoolTypeGame, 0);
			ZeroMemory(abFreeEntries, uiFreeEntriesSize);

			{	// Determine which entries are in the free list
				int iFree = m_iFreeList;
				while( iFree > 0 && iFree < (int)m_uiSize )
				{
					abFreeEntries[iFree] = true;
					iFree = m_aData[iFree].iNextFreeEntry;
				}
			}

			// Convert everything over to the target
			for( uint i = 0; i < m_uiSize; ++i )
			{
				if( abFreeEntries[i] )
				{
					kTarget.m_aData[i].iNextFreeEntry = m_aData[i].iNextFreeEntry; // Just a free list entry
				}
				else
				{
					const T *pSource = (T*)&(m_aData[i]);
					TargetType *pTarget = (TargetType*)&(kTarget.m_aData[i]);
					new(pTarget)TargetType(*pSource); // Do a type conversion
				}
			}

			FTempHeapFastVectorAllocator::FreeAligned(abFreeEntries);
		}
	}

private:
	bool Grow(uint uiSize)
	{
		if( m_uiSize >= uiSize )
			return true;

		// Allocate the new memory block
		Entry *aNew = (Entry*)FMALLOC(sizeof(Entry) * uiSize, c_eMPoolTypeGame, 0);
		FAssertMsg(aNew != NULL, "Failed to allocate!");
		if( aNew == NULL )
			return false;

		// If there was a previous memory block then copy it over and free it
		if( m_uiSize > 0 )
		{
			// This does a POD copy and does not call copy ctors!
			memcpy(aNew, m_aData, m_uiSize * sizeof(T));
			FFREE(m_aData);
		}

		// Fill in free list for new entries
		for( uint i = m_uiSize; i < uiSize - 1; ++i )
			aNew[i].iNextFreeEntry = i + 1;

		// Keep existing free list if for some reason there is one
		if( m_iFreeList >= 0 )
			aNew[uiSize - 1].iNextFreeEntry = m_iFreeList;
		else
			aNew[uiSize - 1].iNextFreeEntry = -1;

		// Free list now starts at start of new entries
		m_iFreeList = m_uiSize;

		// Update data and size members
		m_aData = aNew;
		m_uiSize = uiSize;

		return true;
	}

	// The entries are used both to store the data and
	// to work like a linked free list (linked by index
	// in this case).  The union makes sure the memory
	// is shared.  An entry is either in the free list
	// or is being used as a T.
	struct Entry
	{
		union
		{
			byte mem[sizeof(T)];
			int iNextFreeEntry;
		};
	};

	Entry *m_aData;
	int m_iFreeList;
	uint m_uiSize;
};

#endif // CvWorldBuilderMapElementAllocator_h
