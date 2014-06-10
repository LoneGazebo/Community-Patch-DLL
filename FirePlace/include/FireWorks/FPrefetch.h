//------------------------------------------------------------------------------------------------
//
//  ***************** FIRAXIS GAME ENGINE   ********************
//
//! \file		FPrefetch.h
//! \author		Bart Muzzin -- 2008/04/22
//! \brief		Macros associated with prefetching
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2008 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef FPrefetch_h
#define FPrefetch_h

#ifdef _XENON
#define FPREFETCH( ptr ) __dcbt( 0, ptr );
#define FPREFETCH_2x( ptr ) __dcbt( 0, ptr ); __dcbt( 128, ptr );
#define PREFETCH_SIZE 128

static void PrefetchRegion(const char* pHead, const uint uiSize)
{
    const uint uiLoop = (uiSize+(PREFETCH_SIZE-1))/PREFETCH_SIZE;
    for(uint iCurr = 0; iCurr<uiLoop;++iCurr)
    {
        FPREFETCH(pHead+(iCurr*PREFETCH_SIZE));
    }
}

#elif defined(_PS3)
#define FPREFETCH( ptr ) __dcbt(ptr );
#define FPREFETCH_2x( ptr )  __dcbt( ptr ); __dcbt( ((byte*)ptr) + 128);
#define PREFETCH_SIZE 128

static void PrefetchRegion(const char* pHead, const uint uiSize)
{
    const uint uiLoop = (uiSize+(PREFETCH_SIZE-1))/PREFETCH_SIZE;
    for(uint iCurr = 0; iCurr<uiLoop;++iCurr)
    {
        FPREFETCH(pHead+(iCurr*PREFETCH_SIZE));
    }
}

#elif defined(_WINPC)

// In windows, we can't rely on the cache line being known at compile time, because
// the code can run on processors with different cache sizes. So, we need to find out
// at runtime. This auto-constructed class fills out ms_iCacheLineSize with the CPU's L2
// cache line size
//class FPrefetchAutoCacheLineSizer
//{
//public:
//	FPrefetchAutoCacheLineSizer();
//	static int ms_iCacheLineSize;
//};
//static FPrefetchAutoCacheLineSizer g_kAutoCacheLine;
#include <xmmintrin.h>
#define FPREFETCH( ptr ) _mm_prefetch((const char*)ptr,  _MM_HINT_T0 );
#define FPREFETCH_2x( ptr ) FPREFETCH(ptr); FPREFETCH(ptr+64 /*FPrefetchAutoCacheLineSizer::ms_iCacheLineSize*/);
#define PREFETCH_SIZE 64
// Guess 64 byte cache, until linking problem is resolved.
static void PrefetchRegion(const char* pHead, const uint uiSize)
{
    const uint uiLoop = (uiSize+(PREFETCH_SIZE-1))/PREFETCH_SIZE;
    for(uint iCurr = 0; iCurr<uiLoop;++iCurr)
    {
        FPREFETCH(pHead+(iCurr*PREFETCH_SIZE));
    }
}

#endif

#endif // FPrefetch_h
