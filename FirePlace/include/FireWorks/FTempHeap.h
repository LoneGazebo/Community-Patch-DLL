//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//
//
//                  FTempHeap
//
//  Designed to create a safe global scratch pool for systems to use temporarily
//
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

#ifndef FTEMP_HEAP_H
#define FTEMP_HEAP_H
#include "FAtomic.h"
#include "FUncopyable.h"


class TempHeap
{
public:

    enum eFlags
    {
        eOwnsMemory = 0x00000001
    };

    TempHeap();
    ~TempHeap();

    void Shutdown();

    uint Startup(const uint uiNumBytes, byte* pMemBlock);

    byte* Allocate(const uint uiBytes);
    void  DeAllocate(const byte* pBytes);

    BOOL OwnsAllocation(const byte* pBytes) const;

    class HeapLock
    {
    public:

        HeapLock() : m_uiLock(0){};
    
        enum { eWriteShift = 16 };
    
        //-----------------------------------------------------------
        class WriteLock_Infinite
        {
        public:
    
            WriteLock_Infinite(volatile LONG& uiLockRef);
            ~WriteLock_Infinite() {m_uiLock=0;} // one writer allowed at a time so this does not need to be atomic
    
            volatile LONG& m_uiLock;
        };
    
        //-----------------------------------------------------------
        class WriteLock_Single
        {
        public:
    
            WriteLock_Single(volatile LONG& uiLockRef);
    
            BOOL IsLocked() const { return m_uiLock&(1<<eWriteShift);}
    
            ~WriteLock_Single() {};
    
            volatile LONG& m_uiLock;
        };
    
        //-----------------------------------------------------------
        class ReadLock_Infinite
        {
        public:
            ReadLock_Infinite(volatile LONG& uiLockRef);
    
            ~ReadLock_Infinite(){};
    
            volatile LONG& m_uiLock;
        };
    
    
        //-----------------------------------------------------------
        //-----------------------------------------------------------
        typedef WriteLock_Infinite WRITE_LOCK_INFINITE;
        typedef WriteLock_Single   WRITE_LOCK_SINGLE;
        typedef ReadLock_Infinite  READ_LOCK_INFINITE;
    
        //-----------------------------------------------------------
        volatile LONG& Acquire() { return m_uiLock; }
    
        __declspec( align(16)) volatile LONG m_uiLock;
    };

    uint GetCapacity() const { return m_uiCapacity; };
	// Be careful, this is not thread-safe, so you can get an erroneous number back if allocations are happening when you query.
	uint GetAvailable() const { return m_uiCapacity - m_uiCurrentOffset; };

    void AssertIfNotCompletelyReleased() const
    {
        FAssertMsg(!m_Lock.m_uiLock, "TempHeap has outstanding allocations");
    }

protected:
    HeapLock m_Lock;

    byte*           m_pMemHead;
    volatile LONG   m_uiCurrentOffset;
    uint            m_uiCapacity;

    DWORD m_dwFlags;
};




#ifdef FXS_IS_DLL
extern TempHeap* GetTempHeap();
//extern void BindTempHeapProc();
#else
extern "C" {
	__declspec(dllexport) TempHeap* GetTempHeap();
    extern TempHeap g_TempHeap;


//extern void SetDLLTempHeap(TempHeap* pTempHeap);
};
#endif


//====================================================================================================
// Temp heap allocator for FFastVector
//====================================================================================================
struct FTempHeapFastVectorAllocator
{
	static void* AllocAligned( unsigned int nBytes, unsigned int nAlign, unsigned int nAllocPool, unsigned int nAllocSubID )
	{
		FAssertMsg( nAlign <= 16, "Unavailable alignment requested from TempHeap" );
		void* pRet = (void*)GetTempHeap()->Allocate( nBytes );
		if( !pRet )
			pRet = FMALLOCALIGNED( nBytes, nAlign, nAllocPool, nAllocSubID );

		return pRet;
	}

	static void FreeAligned( void* pBlock )
	{
		TempHeap* pHeap = GetTempHeap();
		if( pHeap->OwnsAllocation( (byte*) pBlock ) )
		{
			pHeap->DeAllocate( (byte*) pBlock );
			return;
		}
		FFREEALIGNED( (unsigned char*)pBlock );
	}
};


//====================================================================================================
/// A scoped wrapper for allocations from the temp heap
//====================================================================================================
template< class T, int POOL, int TAG >
class FTempArray : public FUncopyable
{
public:

    FTempArray( uint nSize )
    {
        TempHeap* pHeap = GetTempHeap();
        byte* pBytes = pHeap->Allocate( nSize * sizeof(T) );
        if( !pBytes )
            pBytes = reinterpret_cast<byte*>( FMALLOC( nSize*sizeof(T), POOL, TAG ) );

        m_pAlloc = reinterpret_cast<T*>(pBytes);       
    }

    ~FTempArray( )
    {
        TempHeap* pHeap = GetTempHeap();
        const byte* pAlloc = reinterpret_cast<const byte*>( m_pAlloc );
        if( pHeap->OwnsAllocation( pAlloc  ) )
            pHeap->DeAllocate( pAlloc );
        else
            FFREE( reinterpret_cast<void*>( m_pAlloc ) );
    }

    operator const T* () const { return m_pAlloc; }
	operator T* () { return m_pAlloc; }
	T* operator ->() const { return m_pAlloc; }
    const T* get() const { return m_pAlloc; };
    T* get() { return m_pAlloc; };


private:

    T* m_pAlloc;
};



#endif


