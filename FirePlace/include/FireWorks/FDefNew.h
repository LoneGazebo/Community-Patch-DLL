//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Copyright (c) 2006 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __FDEF_NEW_H__
#define __FDEF_NEW_H__

// sjh:  what is this for?
#if !defined(USE_MFC)

#ifdef FXS_MEMORY_TRACKER
	#define NI_MEMORY_DEBUGGER
#endif

//-----------------------------------------------------------------------------
// Standard C++ Memory Management Functions
//-----------------------------------------------------------------------------

	#ifdef _PS3 // MS Specific Block Types (Defined in crtdbg.h)

		#define _FREE_BLOCK   0
		#define _NORMAL_BLOCK 1
		#define _CRT_BLOCK    2
		#define _IGNORE_BLOCK 3
		#define _CLIENT_BLOCK 4
		#define _MAX_BLOCKS   5

	#endif

//-----------------------------------------------------------------------------
// Firaxis memory macros.  Always use these to allocate/free memory.
//-----------------------------------------------------------------------------

#if defined(FXS_MEMORY_TRACKER) || defined(FXS_MEMORY_MANAGER)
// if you need new, use this one
	#define FNEW( type, mpool, tag ) new(_NORMAL_BLOCK, __FILE__, __LINE__, mpool, tag) type
	#define FMALLOC( size, mpool, tag ) FireMalloc( size, __FILE__, __LINE__, mpool, tag )
	#define FMALLOC_EXP( size, file, line, pool, tag ) FireMalloc( size, file, line, pool, tag )	// for those who want to override the file/line magic
	#define FREALLOC( ptr, size, mpool, tag ) FireRealloc(ptr, size, __FILE__, __LINE__, mpool, tag)
	#define FREALLOC_EXP( ptr, size, file, line, pool, tag ) FireRealloc( ptr, size, file, line, pool, tag)
	#define FMALLOCALIGNED( size, align, mpool, tag ) FireMallocAligned(size, align, __FILE__, __LINE__, mpool, tag)
	#define FMALLOCALIGNED_EXP( size, align, file, line, pool, tag ) FireMallocAligned( size, align, file, line, pool, tag )
	#define FFREE( ptr ) FireFree(ptr)
	#define FFREEALIGNED( ptr ) FireFreeAligned(ptr)
	#define FMEMSIZE( ptr ) FireMemSize(ptr)
#else
	#define	FNEW( type, mpool, tag ) new type
	#define FMALLOC( size, mpool, tag ) malloc( size )
	#define FMALLOC_EXP( size, file, line, mpool, tag ) malloc( size )
	#define FREALLOC( ptr, size, mpool, tag ) realloc(ptr, size)
	#define FREALLOC_EXP( ptr, size, file, line, pool, tag ) realloc(ptr, size)
	#define FMALLOCALIGNED( size, align, mpool, tag ) FireMallocAlignedNoTracking(size, align, __FILE__, __LINE__)
	#define FMALLOCALIGNED_EXP( size, align, file, line, mpool, tag ) FireMallocAlignedNoTracking(size, align, file, line )
	#define FFREE( ptr ) free(ptr)
	#define FFREEALIGNED( ptr ) FireFreeAlignedNoTracking(ptr)
	#define FMEMSIZE( ptr ) _msize(ptr)
#endif // FXS_MEMORY_MANAGER

//#define new USE_FNEW_INSTEAD

#endif	// !USE_MFC

#endif // __FDEF_NEW_H__
