//-----------------------------------------------------------------------------
//
// STL Block Allocator
//
//-----------------------------------------------------------------------------
//
// This allocator never frees its internal memory pools; however, it does
// reuse memory when it's available.  As with all STL allocators, all memory
// management is stateless.
//
// In most cases, this behavior won't be an issue since the allocated memory
// will be continuously recycled over the course of the program. However, there
// are situations where this allocator is inappropriate.  For example, this
// allocator shouldn't be used with a container that could spike with an
// unusually high memory footprint in situations such as loading.  It also
// shouldn't be used in one-use situations where the memory is intended to be
// released to the OS.
//
// NOTE:  this is a non-tracking allocator.  it only works in tracking builds
// for those places where tracking is not needed (i.e., only in the tracking
// system itself).  DO NOT USE ANYWHERE ELSE!
//
//-----------------------------------------------------------------------------
//
// This allocator is not thread safe.
//
//-----------------------------------------------------------------------------
// Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __FIRAXIS_STL_BLOCK_ALLOCATOR_H__
#define __FIRAXIS_STL_BLOCK_ALLOCATOR_H__

//-----------------------------------------------------------------------------
// predecs for stuff we really really don't want to expose to the outside
// world but apparently have to anyway.
//-----------------------------------------------------------------------------
void FireFreeNoTracking(void * pBlock);
void * FireMallocNoTracking(size_t nSize, const char * szFile, int nLine );
void * FireMalloc(size_t nSize, const char * szFile, int nLine, int nPoolType, int nPoolTag );

//-----------------------------------------------------------------------------
// STL Block Allocator
//-----------------------------------------------------------------------------

template <typename T, unsigned int uBlockCountT, unsigned int pool_type = 0>
class FSTLBlockAllocator
{
	//-------------------------------------------------------------------------
	// Internal Allocator Types (Required)
	//-------------------------------------------------------------------------
	public:

		// Value Type
		typedef T value_type;

		// Pointer Types
		typedef       T * pointer;
		typedef const T * const_pointer;

		// Reference Types
		typedef       T & reference;
		typedef const T & const_reference;

		// Size Types
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;

	//-------------------------------------------------------------------------
	// Rebind Interface
	//-------------------------------------------------------------------------
	public:

		template <typename U> struct rebind
		{
			typedef FSTLBlockAllocator<U, uBlockCountT> other;
		};

	//-------------------------------------------------------------------------
	// Construction
	//-------------------------------------------------------------------------
	public:

		FSTLBlockAllocator()
		{
		}

		FSTLBlockAllocator(const FSTLBlockAllocator<T, uBlockCountT> & rhs)
		{
		}

		template <typename U, unsigned int uBlockCountU>
		FSTLBlockAllocator(const FSTLBlockAllocator<U, uBlockCountU> & rhs)
		{
		}

	//-------------------------------------------------------------------------
	// Destruction
	//-------------------------------------------------------------------------
	public:

		~FSTLBlockAllocator()
		{
		}

	//-------------------------------------------------------------------------
	// Operators
	//-------------------------------------------------------------------------
	public:

		template <typename U, unsigned int uBlockCountU>
		FSTLBlockAllocator<T, uBlockCountT> & operator=(const FSTLBlockAllocator<U, uBlockCountU> & rhs)
		{
			return * this;
		}

	//-------------------------------------------------------------------------
	// Standard Allocator Interface (C++ Standard 20.1.5)
	//-------------------------------------------------------------------------
	public:

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		pointer allocate(size_type nCount, const void * pHint)
		{
			if (nCount == 1)
			{
				return static_cast<pointer>(AllocateFromPool());
			}
			else
			{
				return static_cast<pointer>(FireMalloc(nCount * sizeof(T), __FILE__, __LINE__, pool_type, 0));
			}
		}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		pointer allocate(size_type nCount)
		{
			return allocate(nCount, NULL);
		}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		void deallocate(pointer pMemory, size_type nCount)
		{
			if (nCount == 1)
			{
				FreeFromPool(pMemory);
			}
			else
			{
				FireFreeNoTracking(pMemory);
			}
		}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		void construct(pointer pMemory, const T & rValue)
		{
			::new (pMemory) T(rValue);
		}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		void destroy(pointer pObject)
		{
			pObject->~T();
		}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		pointer address(reference rObject) const
		{
			return & rObject;
		}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		const_pointer address(const_reference rObject) const
		{
			return & rObject;
		}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
		size_type max_size() const
		{
			size_type nCount = static_cast<size_type>(-1) / sizeof (T); 

			if (nCount)
			{
				return nCount;
			}
			else
			{
				return 1;
			}
		}

	//-------------------------------------------------------------------------
	// Implementation (Stateless)
	//-------------------------------------------------------------------------
	private:

		//---------------------------------------------------------------------
		// Allocates a block from the internal memory pool.
		//---------------------------------------------------------------------
		void * AllocateFromPool()
		{
			if (s_pFreeStack == NULL)
			{
				CreatePool();
			}

			return Pop();
		}

		//---------------------------------------------------------------------
		// Frees a block from the internal memory pool.
		//---------------------------------------------------------------------
		void FreeFromPool(void * pBlock)
		{
			Push(pBlock);
		}

		//---------------------------------------------------------------------
		// Calculates the internal block size.
		//---------------------------------------------------------------------
		size_t GetBlockSize() const
		{
			// So we don't depend on the elusive MAX macro
			return (sizeof(T) > sizeof(void *) ? sizeof(T) : sizeof(void *));
		}

		//---------------------------------------------------------------------
		// Creates a memory pool based on the template parameters.
		//---------------------------------------------------------------------
		void CreatePool()
		{
			// Note: This memory is never freed.
			char * pMemoryPool = reinterpret_cast<char *>(FireMallocNoTracking(GetBlockSize() * uBlockCountT, __FILE__, __LINE__));

			if (pMemoryPool)
			{   
				for (unsigned int i = 0; i < uBlockCountT; i++)
				{
					Push(pMemoryPool);
					pMemoryPool += GetBlockSize();
				}
			}
		}

		//---------------------------------------------------------------------
		// Pushes a block onto the free stack.
		//---------------------------------------------------------------------
		void Push(void * pBlock)
		{
			*(reinterpret_cast<void **>(pBlock)) = s_pFreeStack;
			s_pFreeStack = pBlock;
		}

		//---------------------------------------------------------------------
		// Pops a block off of the free stack.
		//---------------------------------------------------------------------
		void * Pop()
		{
			void * pBlock = s_pFreeStack;

			if (pBlock)
			{
				s_pFreeStack = *(reinterpret_cast<void **>(pBlock));
			}

			return pBlock;
		}

		// Free Stack Pointer (Top)
		static void * s_pFreeStack;
};

//-----------------------------------------------------------------------------
// Static Member Variables
//-----------------------------------------------------------------------------

template <typename T, unsigned int uBlockCountT, unsigned int pool_type>
void * FSTLBlockAllocator<T, uBlockCountT, pool_type>::s_pFreeStack = NULL;

//-----------------------------------------------------------------------------
// Non-Member Operators
//-----------------------------------------------------------------------------

template <typename T, unsigned int uBlockCountT, typename U, unsigned int uBlockCountU>
inline bool operator==(const FSTLBlockAllocator<T, uBlockCountT> & lhs, const FSTLBlockAllocator<U, uBlockCountU> & rhs)
{
	return true;
}

template <typename T, unsigned int uBlockCountT, typename U, unsigned int uBlockCountU>
inline bool operator!=(const FSTLBlockAllocator<T, uBlockCountT> & lhs, const FSTLBlockAllocator<U, uBlockCountU> & rhs)
{
	return false;
}

//-----------------------------------------------------------------------------
// Replace The Original Memory Management Functions
//-----------------------------------------------------------------------------

	#include "FDefNew.h"

#endif // Include Guard
