/*
Fast vector class designed to be as simple as possible
while retaining the basic behavior of std::vector.  This class 
should really only be used for very simple classes and structures,
since the only benefit is speed for handling large numbers of elements.
It supports standard iterators through pointer functionality.

The first template parameter, T, is the type to store in the vector.

The second template parameter, L, is the number of elements to reserve in
the class.  This allows the first L elements to be stored in the class without
an extra memory allocation, and is nice for situations where you are pretty sure
that the size will not get bigger than L, but cannot be sure.

The third template parameter tells the template whether to call the copy 
constructor or simply memcpy data.  This is useful if you have a structure
which is a simple data container but has a constructor defined, since the 
compiler will want to call the constructor but it is correct to memcpy.

Author: John Kloetzli
9/9/2008

version 1.3

*/

#pragma once

#include "vector"
#include "FAssert.h"
#include "new"
#include "iterator"

//#define BREAK_ON_REPEATED_RESIZE 4
//#define BREAK_ON_STATIC_RESIZE 1

//Disable some warnings
#if defined(_WIN32) || defined(_WIN64)
#pragma warning( push )
#pragma warning( disable : 4100)
#pragma warning( disable : 4127)
#endif


/////////////////////////////////////////////////////////////////////////////////////
//Pre-declarations
/////////////////////////////////////////////////////////////////////////////////////

template< 
	class T, 
	bool bPODType 
> class BaseVector;

template< 
	class T, 
	bool bPODType = false, 
	unsigned int AllocPool = c_eMPoolTypeContainer, 
	unsigned int nSubID = 0,
	class FAST_VEC_ALLOC = typename BaseVector< T, bPODType >::FDefaultFastVectorAllocator
> class FFastVector;

template< 
	class T, 
	unsigned int L = 1, 
	bool bPODType = false, 
	unsigned int AllocPool = c_eMPoolTypeContainer, 
	unsigned int nSubID = 0
> class FStaticVector;


/////////////////////////////////////////////////////////////////////////////////////
// Base vector class - Not meant to be initialized directly
/////////////////////////////////////////////////////////////////////////////////////
template< class T, bool bPODType > class BaseVector
{
public:

	/////////////////////////////////////////////////////////////////////////////////////
	// Default allocator
	/////////////////////////////////////////////////////////////////////////////////////
	struct FDefaultFastVectorAllocator
	{
		static void* AllocAligned( unsigned int nBytes, unsigned int nAlign, unsigned int nAllocPool, unsigned int nAllocSubID )
		{
			return FMALLOCALIGNED( nBytes, nAlign, nAllocPool, nAllocSubID );
		}

		static void FreeAligned( void* pBlock )
		{
			FFREEALIGNED( pBlock );
		}
	};


	typedef BaseVector< T, bPODType > THIS_TYPE;

    typedef T& reference;
    typedef const T& const_reference;
    typedef unsigned int size_type;
    typedef size_type difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;


	~BaseVector(){};

	/////////////////////////////////////////////////////////////////////////////////////
	// Functions to delete
	/////////////////////////////////////////////////////////////////////////////////////
	void pop_back(){
		FAssert(m_uiCurrSize > 0);
		Destroy(&m_pData[--m_uiCurrSize], 1);
	};
	void clear(){
		Destroy(m_pData, m_uiCurrSize);
		m_uiCurrSize = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Getters/setters
	/////////////////////////////////////////////////////////////////////////////////////
	T& operator[](unsigned int ui) {
		FAssert(ui < m_uiCurrSize);
		return m_pData[ui];
	};
	

    inline const T& operator[](unsigned int ui) const{
		FAssert(ui < m_uiCurrSize);
		return m_pData[ui];
	};

    inline const T& at( unsigned int i ) const {
	    FAssert(i < m_uiCurrSize);
		return m_pData[i];
    };

    inline T& at( unsigned int i ) {
	    FAssert(i < m_uiCurrSize);
		return m_pData[i];
    };

    inline T& front() { 
        return *m_pData; 
    };
	inline T& back(){
		FAssert( m_uiCurrSize );
        return *(m_pData+m_uiCurrSize-1); 
    };
    
    inline const T& front() const { 
        return *m_pData; 
    };
    inline const T& back() const { 
		FAssert( m_uiCurrSize );
        return *(m_pData+m_uiCurrSize-1);
    };

	bool empty() const{
		return size() == 0;
	};
	unsigned int size() const{
		return m_uiCurrSize;
	};
	unsigned int capacity() const{
		return m_uiCurrMaxSize;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Get the pointer iterators for the class.
	/////////////////////////////////////////////////////////////////////////////////////

	//DEPRECATED
	T* iter(unsigned int ui) const{
		FAssert(ui < m_uiCurrSize);
		return m_pData+ui;
	};
	T* get_iterator(unsigned int ui) const{
		FAssert(ui < m_uiCurrSize);
		return m_pData+ui;
	};
	const T* get_iterator_const(unsigned int ui) const{
		FAssert(ui < m_uiCurrSize);
		return m_pData+ui;
	};

    inline T* begin() { 
        return m_pData; 
    };
    inline T* end() { 
        return m_pData+m_uiCurrSize; 
    };

    inline const T* begin() const { 
        return m_pData; 
    };
    inline const T* end() const { 
        return m_pData+m_uiCurrSize; 
    };

    void InPlaceInit(const BaseVector& rhs, T* pInPlaceData)
    {
        m_uiCurrSize    = rhs.m_uiCurrSize;
        m_uiCurrMaxSize = rhs.m_uiCurrMaxSize;
        m_pData = pInPlaceData;
        memcpy(m_pData,rhs.m_pData, sizeof(T) * m_uiCurrSize);
    }

protected:
	BaseVector() : m_uiCurrSize(0), m_uiCurrMaxSize(0), m_pData(NULL) {};

	//Call destructor for each element
	void Destroy(T* pVal, unsigned int uiNumElements)
	{
		if( !bPODType){
			for(unsigned int i = 0; i < uiNumElements; ++i){
				pVal[i].~T();
			}
		}
	};

	//Copy list of elements, calling copy constructor for each element
	void Copy(const THIS_TYPE& RHS){
		m_uiCurrSize = RHS.m_uiCurrSize;
		if( bPODType ){
			memcpy( (void*)m_pData, (void*)RHS.m_pData, sizeof(T)*RHS.m_uiCurrSize);
		}else{
			for(unsigned int i = 0; i < m_uiCurrSize; ++i){
				new( (void*)&m_pData[i] )T( RHS.m_pData[i] );
			}
		}
	};

	T* m_pData;						//The main data store.  May point to the local memory store.
	unsigned int m_uiCurrSize;		//The current number of elements in the vector
	unsigned int m_uiCurrMaxSize;	//The maximum number of elements which can be stored in the current memory store.

	template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID, class FAST_VEC_ALLOC >
	friend void* operator new( size_t uiSize, FFastVector< T, bPODType, AllocPool, nSubID, FAST_VEC_ALLOC >& kVector );

	template< class T, unsigned int L, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
	friend void* operator new( size_t uiSize, FStaticVector< T, L, bPODType, AllocPool, nSubID >& kVector );
};

/////////////////////////////////////////////////////////////////////////////////////
// Vector class which allocates memory according to a particular memory pool.
//
//T - the template class to store in the vector.
//
//bPODType - Set this to true if the data you are working on is a native type or just
// "plain-old-data," in which case faster memory copy and no constructor/destructor
// can be used internally.  Use with caution, since it will hose objects which do have
// non-trivial constructors.
//
//AllocPool - The memory pool to allocate from.
//
//nSubID - The memory sub ID to tag allocations with
//
//buseTempHeap - If true, allocations will be sent to the temp heap, using FMALLOC as a fallback
////////////////////////////////////////////////////////////////////////
template< 
	class T, 
	bool bPODType,
	unsigned int AllocPool, 
	unsigned int nSubID,
    class FAST_VEC_ALLOC
> class FFastVector : public BaseVector< T, bPODType >
{
public:

	typedef FFastVector< T, bPODType, AllocPool, nSubID, FAST_VEC_ALLOC > THIS_TYPE;
	typedef BaseVector< T, bPODType > BASE_TYPE;
	typedef T TYPE;

	typedef TYPE* iterator;
    typedef const TYPE* const_iterator;
    typedef TYPE& reference;
    typedef const TYPE& const_reference;
    typedef BASE_TYPE::size_type size_type;
    typedef size_type difference_type;
    typedef TYPE value_type;
    typedef T* pointer;
    typedef const T* const_pointer;



    /////////////////////////////////////////////////////////////////////////////////////
	// Default constructor, copy constructor, destructor and operator=
	/////////////////////////////////////////////////////////////////////////////////////
	FFastVector(const THIS_TYPE& RHS)
	{
		m_uiCurrSize = RHS.m_uiCurrSize;
		m_uiCurrMaxSize = RHS.m_uiCurrMaxSize;
		if( RHS.m_uiCurrMaxSize)
		{
			m_pData = Alloc(RHS.m_uiCurrMaxSize);
		}
#ifdef BREAK_ON_REPEATED_RESIZE
		m_nResizeTimes = 0;
#endif
		Copy(RHS);
	};
	FFastVector(unsigned int uiStartingMaxSize = 0)
	{
#ifdef BREAK_ON_REPEATED_RESIZE
		m_nResizeTimes = 0;
#endif
		m_pData = Alloc(uiStartingMaxSize);
	};
	~FFastVector(){
		Free(m_pData, m_uiCurrSize);
	};
	void operator = (const THIS_TYPE& RHS){
		if( m_uiCurrMaxSize <= RHS.m_uiCurrSize){
			Free(m_pData, m_uiCurrSize);
			m_pData = Alloc(RHS.m_uiCurrMaxSize);
		}else{
			Destroy(m_pData, m_uiCurrSize);
		}
		Copy(RHS);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Reserve space for up to N elements inside the vector
	/////////////////////////////////////////////////////////////////////////////////////
	void reserve(unsigned int uiResSize )
	{
		GrowSizeToFit(uiResSize);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	// Resize the vector to exactly N elements
	// Note: This does not call constructors on new elements created by this call OR call
	// destructors on elements removed!  Use with caution!
	/////////////////////////////////////////////////////////////////////////////////////
	void setsize( unsigned int uiNewSize ){
		SetSize(uiNewSize);
		m_uiCurrSize = MIN(uiNewSize, m_uiCurrMaxSize);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// Resize the vector to at least N elements
	// Note: This does not call constructors on new elements created by this call OR call
	// destructors on elements removed!  Use with caution!
	/////////////////////////////////////////////////////////////////////////////////////
	void resize( unsigned int uiNewSize )
	{
		if( m_uiCurrMaxSize < uiNewSize ){
			GrowSize(uiNewSize);
		}
		m_uiCurrSize = MIN(uiNewSize, m_uiCurrMaxSize);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//Add/remove elements without needing a copy constructor
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int push_back()
	{
		if( m_uiCurrSize == m_uiCurrMaxSize )
			GrowSize(m_uiCurrMaxSize);
		new( (void*)&m_pData[m_uiCurrSize] )T();
		return m_uiCurrSize++;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Add/remove elements
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int push_back(const T& element)
	{
		if( m_uiCurrSize == m_uiCurrMaxSize )
			GrowSize(m_uiCurrMaxSize);
		new( (void*)&m_pData[m_uiCurrSize] )T(element);
		return m_uiCurrSize++;
	};

	//Add n elements to the end of the vector
	void push_back( const T* pElements,  unsigned int uiNum)
	{
		unsigned int uNewSize = uiNum + m_uiCurrSize;
		if( uNewSize > m_uiCurrMaxSize )
		{
#ifdef BREAK_ON_REPEATED_RESIZE
			FAssert( ++m_nResizeTimes < BREAK_ON_REPEATED_RESIZE );
#endif
			GrowSize(uNewSize);
		}

		if(bPODType){
			memcpy( (void*) &m_pData[m_uiCurrSize], pElements, sizeof(T) * uiNum );
		}else{
			for(unsigned int i = m_uiCurrSize; i < uNewSize; ++i){
				new( (void*)&m_pData[i] )T(*(pElements++));
			}
		}

		m_uiCurrSize = uNewSize;
	}

	//Add uiNum copies of element to the end of the vector.
	void push_back_copy( const T& element, unsigned int uiNum  )
	{
		uiNum += m_uiCurrSize;
		if( uiNum > m_uiCurrMaxSize )
			GrowSize(uiNum);
		for(unsigned int i = m_uiCurrSize; i < uiNum; ++i){
			new( (void*)&m_pData[i] )T(element);
		}
		m_uiCurrSize = uiNum;
	};

protected:

	//Grow the size of the internal data store by a factor of two
	void GrowSize(unsigned int uiFit)
	{
		unsigned int nOld = m_uiCurrMaxSize;
		if( m_uiCurrMaxSize == 0 ) m_uiCurrMaxSize = 1;
		while( uiFit >= m_uiCurrMaxSize ){

			//Try to double size...
			UINT uiNewSize = m_uiCurrMaxSize<<1;

			//...on overflow bail and set to exact size
			if( uiNewSize < m_uiCurrMaxSize ){
				m_uiCurrMaxSize = uiFit;
				break;
			}

			//...otherwise use the doubled size
			m_uiCurrMaxSize = uiNewSize;
		}

		T* pTemp = Alloc(m_uiCurrMaxSize);
		if (pTemp)
		{
			if( bPODType ){
				memcpy( (void*)pTemp, (void*)m_pData, sizeof(T)*m_uiCurrSize);
			}else{
				for(unsigned int i = 0; i < m_uiCurrSize; ++i)
					new( (void*)&(pTemp[i]) )T( m_pData[i]);
			}
			Free(m_pData, m_uiCurrSize);
			m_pData = pTemp;
		}
		else
		{
			FAssertMsg2(0, "Failed to grow array size from %u to %u", nOld, m_uiCurrMaxSize);
			m_uiCurrMaxSize = nOld;
		}
	};

	//Grow the internal data store to fit N elements compactly
	void GrowSizeToFit(unsigned int uiFit)
	{
		if( uiFit <= m_uiCurrMaxSize )
			return;

		SetSize(uiFit);
	};

	void SetSize(unsigned int uiFit){

		unsigned int nOld = m_uiCurrSize;

		m_uiCurrSize = MIN( m_uiCurrSize, uiFit );

		T* pTemp = NULL;
		if( uiFit > 0 ){
			pTemp = Alloc(uiFit);
			if (pTemp)
			{
				if( bPODType ){
					memcpy( (void*)pTemp, (void*)m_pData, sizeof(T)*m_uiCurrSize);
				}else{
					for(unsigned int i = 0; i < m_uiCurrSize; ++i)
						new( (void*)&(pTemp[i]) )T( m_pData[i]);
				}
				m_uiCurrMaxSize = uiFit;
			}
			else
			{
				FAssertMsg2(0, "Failed to set array size from %u to %u", nOld, uiFit);
				m_uiCurrSize = nOld;
				return;
			}
		}
		else
		{
			m_uiCurrMaxSize = 0;
		}
		Free(m_pData, nOld);
		m_pData = pTemp;
	};

	//Allocate memory as bytes
	T* Alloc( unsigned int uiSize ){
		if( uiSize > 0 ){
            
            T* pRet = (T*)FAST_VEC_ALLOC::AllocAligned( uiSize*sizeof(T), __alignof(T), AllocPool, nSubID );
			m_uiCurrMaxSize = uiSize;
			return pRet;
		}
		return NULL;
	};

	//Free the memory store.
	void Free(T* pVal, unsigned int uiNumElements)
	{
		if( !bPODType) 
            Destroy(pVal, uiNumElements);

		FAST_VEC_ALLOC::FreeAligned( (void*)pVal );
	};

#ifdef BREAK_ON_REPEATED_RESIZE 
	unsigned int m_nResizeTimes;
#endif

	template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID, class FAST_VEC_ALLOC >
	friend void* operator new( size_t uiSize, THIS_TYPE& kVector );
};

// Placement new on a FFastVector allows you to call a constructor directly on memory internal to the vector.
// This operation will place the new object on the end resizing the vector by 1.  Just like push_back.
template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID, class FAST_VEC_ALLOC >
void* operator new(size_t uiSize, FFastVector< T, bPODType, AllocPool, nSubID, FAST_VEC_ALLOC >& kVector)
{
	// Make sure the type is of the correct size to fit into the vector
	FAssertMsg(uiSize == sizeof T, "Using placement new on FFastVector with the wrong type!");

	unsigned int nSize = kVector.size();
	if( kVector.m_uiCurrSize == kVector.m_uiCurrMaxSize )
	{
		kVector.GrowSize( nSize+1 );
	}
	kVector.m_uiCurrSize = nSize+1;
	return &(kVector[nSize]);
}

/////////////////////////////////////////////////////////////////////////////////////
// Static vector class which reserves a certain amount of space within the class, only
// allocating when you overflow that space.  After allocation, the internal space is not
// used.
//
//T - the template class to store in the vector.
//
//L - the number of elements in local storage before allocating memory.
//
//bPODType - Set this to true if the data you are working on is a native type or just
// "plain-old-data," in which case faster memory copy and no constructor/destructor
// can be used internally.  Use with caution, since it will hose objects which do have
// non-trivial constructors.
//
//AllocPool - The memory pool to allocate from.
//
//nSubID - The memory sub ID to tag allocations with
////////////////////////////////////////////////////////////////////////
template< 
	class T, 
	unsigned int L, 
	bool bPODType,
	unsigned int AllocPool, 
	unsigned int nSubID
> class FStaticVector : public BaseVector< T, bPODType >
{
	typedef FStaticVector< T, L, bPODType, AllocPool, nSubID > THIS_TYPE;
	typedef BaseVector< T, bPODType > BASE_TYPE;

public:
	typedef T* iterator;
    typedef const T* const_iterator;

    typedef T& reference;
    typedef const T& const_reference;
    typedef unsigned int size_type;
    typedef size_type difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    /////////////////////////////////////////////////////////////////////////////////////
	// Default constructor, copy constructor, destructor and operator=
	/////////////////////////////////////////////////////////////////////////////////////
	FStaticVector(const THIS_TYPE& RHS)
	{
		m_uiCurrSize = RHS.m_uiCurrSize;
		m_uiCurrMaxSize = RHS.m_uiCurrMaxSize;
		m_pData = Alloc(RHS.m_uiCurrMaxSize);
#ifdef BREAK_ON_STATIC_RESIZE
		m_iNumResized = 0;
#endif
		Copy(RHS);
	};
	FStaticVector()
    {
		m_uiCurrMaxSize = L;
		m_pData = Alloc(m_uiCurrMaxSize);
#ifdef BREAK_ON_STATIC_RESIZE
		m_iNumResized = 0;
#endif
    };
	FStaticVector(unsigned int uiStartingMaxSize)
	{
		m_uiCurrMaxSize = uiStartingMaxSize;
		m_pData = Alloc(m_uiCurrMaxSize);
#ifdef BREAK_ON_STATIC_RESIZE
		m_iNumResized = 0;
#endif
	};
	~FStaticVector(){
		Free(m_pData, m_uiCurrSize);
	};
	void operator = (const THIS_TYPE& RHS){
		if( m_uiCurrMaxSize <= RHS.m_uiCurrSize){
			Free(m_pData, m_uiCurrSize);
			m_pData = Alloc(RHS.m_uiCurrMaxSize);
		}else{
			Destroy(m_pData, m_uiCurrSize);
		}
		Copy(RHS);
	};

    bool operator == (const THIS_TYPE& RHS) const {
        bool bResult = false;

        if( m_uiCurrSize != RHS.m_uiCurrSize )
        {
            goto Cleanup;
        }

        for( UINT uIdx = 0; uIdx < m_uiCurrSize; ++uIdx )
        {
            if( m_pData[ uIdx ] != RHS.m_pData[ uIdx ] )
            {
                goto Cleanup;
            }
        }
        bResult = true;

Cleanup:

		return bResult;

	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Reserve space for up to N elements inside the vector
	/////////////////////////////////////////////////////////////////////////////////////
	void reserve(unsigned int uiResSize )
	{
		GrowSizeToFit(uiResSize);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	// Resize the vector to at least N elements - note that this might create garbage in unused
	// element positions!
	/////////////////////////////////////////////////////////////////////////////////////
	void resize(unsigned int uiNewSize )
	{
		if( m_uiCurrMaxSize < uiNewSize ){
			GrowSize(uiNewSize);
		}
		m_uiCurrSize = uiNewSize;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Add/remove elements
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int push_back(const T& element){
		m_bIsResized = false;
		if( m_uiCurrSize == m_uiCurrMaxSize )
			GrowSize(m_uiCurrMaxSize);
		new( (void*)&m_pData[m_uiCurrSize] )T(element);
		return m_uiCurrSize++;
	};

	//Add n elements to the end of the vector
	void push_back( const T* pElements,  unsigned int uiNum)
	{
		m_bIsResized = false;
		unsigned int uNewSize = uiNum + m_uiCurrSize;
		if( uNewSize > m_uiCurrMaxSize )
			GrowSize(uNewSize);

		if(bPODType)
			memcpy( (void*) &m_pData[m_uiCurrSize], pElements, sizeof(T) * uiNum );
		else
			for(unsigned int i = m_uiCurrSize; i < uNewSize; ++i)
				new( (void*)&m_pData[i] )T(*(pElements++));
		
		m_uiCurrSize = uNewSize;
	}
    // remove the element pointed to by 'it' and shrink the list
    void erase( iterator it )
	{
		m_bIsResized = false;

		unsigned int uIndex = it - m_pData;

        for(unsigned int i = uIndex; i + 1< m_uiCurrSize; ++i)
        {
            m_pData[i] = m_pData[i+1];
        }
		
		--m_uiCurrSize;
	}

	//Add uiNum copies of element to the end of the vector.
	void push_back_copy( const T& element, unsigned int uiNum  )
	{
		m_bIsResized = false;
		uiNum += m_uiCurrSize;
		if( uiNum > m_uiCurrMaxSize )
		{
#ifdef BREAK_ON_STATIC_RESIZE
			FAssert( ++m_iNumResized < BREAK_ON_STATIC_RESIZE );
#endif
			GrowSize(uiNum);
		}
		for(unsigned int i = m_uiCurrSize; i < uiNum; ++i){
			new( (void*)&m_pData[i] )T(element);
		}
		m_uiCurrSize = uiNum;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Returns false if the vector has grown larger than the local memory store
	/////////////////////////////////////////////////////////////////////////////////////
	bool is_local_only() const{
		return (m_pData == m_aData);
	};

protected:

	//Grow the size of the internal data store by a factor of two
	void GrowSize(unsigned int uiFit)
	{
		if( m_uiCurrMaxSize == 0 ) m_uiCurrMaxSize = 1;
		while( uiFit >= m_uiCurrMaxSize ){

			//Try to double size...
			UINT uiNewSize = m_uiCurrMaxSize<<1;

			//...on overflow bail and set to exact size
			if( uiNewSize < m_uiCurrMaxSize ){
				m_uiCurrMaxSize = uiFit;
				break;
			}

			//...otherwise use the doubled size
			m_uiCurrMaxSize = uiNewSize;
		}

		T* pTemp = Alloc(m_uiCurrMaxSize);
		if( bPODType ){
			memcpy( (void*)pTemp, (void*)m_pData, sizeof(T)*m_uiCurrSize);
		}else{
			for(unsigned int i = 0; i < m_uiCurrSize; ++i)
				new( (void*)&(pTemp[i]) )T( m_pData[i]);
		}
		Free(m_pData, m_uiCurrSize);
		m_pData = pTemp;

		m_bIsResized = true;
	};

	//Grow the internal data store to fit N elements compactly
	void GrowSizeToFit(unsigned int uiFit)
	{
		if( uiFit <= m_uiCurrMaxSize )
			return;

		m_uiCurrMaxSize = uiFit;
		T* pTemp = Alloc(m_uiCurrMaxSize);
		if( bPODType ){
			memcpy( (void*)pTemp, (void*)m_pData, sizeof(T)*m_uiCurrSize);
		}else{
			for(unsigned int i = 0; i < m_uiCurrSize; ++i)
				new( (void*)&(pTemp[i]) )T( m_pData[i]);
		}
		Free(m_pData, m_uiCurrSize);
		m_pData = pTemp;

		m_bIsResized = true;
	};

	//Allocate memory as bytes
	T* Alloc(unsigned int uiSize){
		T* pRet;
		if( uiSize > L ){
			pRet = (T*)FMALLOCALIGNED( uiSize*sizeof(T), __alignof(T), AllocPool, nSubID );
			m_uiCurrMaxSize = uiSize;
		}else{
			pRet = (T*)m_aData;
			m_uiCurrMaxSize = L;
		}
		return pRet;
	};

	//Free the memory store.
	void Free(T* pVal, unsigned int uiNumElements)
	{
		if( !bPODType) Destroy(pVal, uiNumElements);
		if( pVal != (T*)m_aData )
			FFREEALIGNED( (unsigned char*)pVal );
	};

	unsigned char m_aData[L*sizeof(T)]; //Local memory store, used until the data will not fit inside any more.
	bool m_bIsResized;					//Whether the last call to push_back called the memory store to resize.

#ifdef BREAK_ON_STATIC_RESIZE
	unsigned char m_iNumResized;
#endif

	template< class T, unsigned int L, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
	friend void* operator new( size_t uiSize, FStaticVector< T, L, bPODType, AllocPool, nSubID >& kVector );
};

///////////////////////////////////////////////////////////////////////////////
//
//  Temporary class for a simple, fixed based list.
//  NOTE: Going over the list size will AV
//
//////////////////////////////////////////////////////////////////////////////
template< 
	class T, 
	unsigned int L, 
	bool bPODType,
	unsigned int AllocPool, 
	unsigned int nSubID = 0
> class FFastSmallFixedList 
{
public:
    FFastSmallFixedList()
    {
        clear();
    };

    void
    clear()
    {
        mVec.resize( 0 );
    };

    T*
    nodeNum( int iIndex )
    {
        return getAt( iIndex );
    };

    const T*
    nodeNum( int iIndex ) const
    {

        return getAt( iIndex );
    };

    T*
    getAt( INT iIndex )
    {
        if ( (UINT)iIndex < mVec.size() )
	    {
		    return &mVec[ iIndex ];
	    }
	    else
	    {
		    return NULL;
	    }
    };

    const T*
    getAt( INT iIndex ) const 
    {
        if ( (UINT)iIndex < mVec.size() )
	    {
		    return &mVec[ iIndex ];
	    }
	    else
	    {
		    return NULL;
	    }
    };

    T*
    next( T* pNode )
    {
        INT iIndex = ( pNode + 1 ) - &mVec[ 0 ];
        return getAt( iIndex );
    }

    T*
    prev( T* pNode )
    {

        INT iIndex = ( pNode - 1 ) - &mVec[ 0 ];
        return getAt( iIndex );
    }

    const T*
    next( const T* pNode ) const 
    {

        INT iIndex = ( pNode + 1 ) - &mVec[ 0 ];
        return getAt( iIndex );
    }

    const T*
    prev( const T* pNode ) const
    {
        INT iIndex = ( pNode - 1 ) - &mVec[ 0 ];
        return getAt( iIndex );
    }

    int
    getLength() const
    {
        return mVec.size();
    }

    __forceinline
    T* head()
    {
        UINT uSize = mVec.size();

        if( uSize > 0 )
        {
            return &mVec[ 0 ];
        }
        else
        {
            return NULL;
        }
    };

    __forceinline
    T* tail()
    {
        UINT uSize = mVec.size();

        if( uSize > 0 )
        {
            return &mVec[ uSize - 1 ];
        }
        else
        {
            return NULL;
        }
    };
    __forceinline
    const T* head() const
    {
        UINT uSize = mVec.size();

        if( uSize > 0 )
        {
            return &mVec[ 0];
        }
        else
        {
            return NULL;
        }
    };

    __forceinline
    const T* tail() const 
    {
        UINT uSize = mVec.size();

        if( uSize > 0 )
        {
            return &mVec[ uSize - 1 ];
        }
        else
        {
            return NULL;
        }
    };

    __forceinline void
    insertAtEnd( T* ptData )
    { 
        mVec.push_back( *ptData );       
    };

    __forceinline void
    swapUp( int iIndex )
    {
        if ( iIndex + 1 < (INT)mVec.size() && iIndex >= 0)
        {
            T Temp = mVec[ iIndex + 1];
            mVec[ iIndex + 1 ] = mVec[ iIndex ];
            mVec[ iIndex ] = Temp;
        }
    }
        
    __forceinline void
    insertBefore( T* ptData, T* ptBefore )
    {        
        UINT uSize = mVec.size();

		INT iIndex = ptBefore - &mVec[ 0 ];

		mVec.resize( uSize + 1 );

        for( INT iIdx = (INT)uSize; iIdx > iIndex; --iIdx )
        {
            mVec[ iIdx ] = mVec[ iIdx - 1 ];
        }

        mVec[ iIndex ] = *ptData;

    }

    __forceinline void
    insertAtBeginning( T* ptData )
	{        
		UINT uSize = mVec.size();

		mVec.resize( uSize + 1 );

        for( INT iIdx = (INT)uSize; iIdx >= 1; --iIdx )
        {
            mVec[ iIdx ] = mVec[ iIdx - 1 ];
        }

        mVec[ 0 ] = *ptData;
    };
    
    __forceinline T*
    deleteNode( T* tData )
    {
        UINT uDeletePos = tData - &mVec[ 0 ];

        for( UINT uIdx = uDeletePos; uIdx + 1 < mVec.size(); ++uIdx )
        {
            mVec[ uIdx ] = mVec[ uIdx + 1 ];
        }

        //  List shrinks by 1
        mVec.resize( mVec.size() - 1 );

        if( uDeletePos < mVec.size() )
        {
	        return &mVec[ uDeletePos ];
        }
        else
        {
            return NULL;
        }
    };

private:
    
    FStaticVector< T, L, bPODType, AllocPool, nSubID>  mVec;
 
};

// Placement new on a FStaticVector allows you to call a constructor directly on memory internal to the vector.
// This operation will place the new object on the end resizing the vector by 1.  Just like push_back.
template< class T, unsigned int L, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
void* operator new(size_t uiSize, FStaticVector< T, L, bPODType, AllocPool, nSubID >& kVector)
{
	// Make sure the type is of the correct size to fit into the vector
	FAssertMsg(uiSize == sizeof T, "Using placement new on FFastVector with the wrong type!");

	unsigned int nSize = kVector.size();
	if( kVector.m_uiCurrSize == kVector.m_uiCurrMaxSize )
	{
		kVector.GrowSize( nSize+1 );
	}
	kVector.m_uiCurrSize = nSize+1;
	return &(kVector[nSize]);
}

/////////////////////////////////////////////////////////////////////////////////////
// Fixed vector class allocates a pre-determined (runtime) size and does not allocate
// any additional space.  This is somewhat similar to FStaticVector, but difference, beyond
// the ability to grow is that there is no copy constructor call for resizing which is
// helpful if the copy constructor is private.
//
//T - the template class to store in the vector.
//
//L - the number of elements in local storage before allocating memory.
//
//bPODType - Set this to true if the data you are working on is a native type or just
// "plain-old-data," in which case faster memory copy and no constructor/destructor
// can be used internally.  Use with caution, since it will hose objects which do have
// non-trivial constructors.
//
//AllocPool - The memory pool to allocate from.
//
//nSubID - The memory sub ID to tag allocations with
////////////////////////////////////////////////////////////////////////
template< 
	class T, 
	bool bPODType,
	unsigned int AllocPool, 
	unsigned int nSubID = 0
> class FFixedVector : public BaseVector< T, bPODType >
{
	typedef FFixedVector< T, bPODType, AllocPool, nSubID > THIS_TYPE;
	typedef BaseVector< T, bPODType > BASE_TYPE;

public:
	typedef T* iterator;
    typedef const T* const_iterator;

    typedef T& reference;
    typedef const T& const_reference;
    typedef unsigned int size_type;
    typedef size_type difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    /////////////////////////////////////////////////////////////////////////////////////
	// Default constructor, copy constructor, destructor and operator=
	/////////////////////////////////////////////////////////////////////////////////////
	FFixedVector(const THIS_TYPE& RHS)
	{
		m_uiCurrSize = MIN(RHS.m_uiCurrSize, m_uiCurrMaxSize);
		CopyMin(RHS);
	};

	FFixedVector()
    {
		m_uiCurrMaxSize = 0;
		m_pData = Alloc(m_uiCurrMaxSize);
    };
	FFixedVector(unsigned int uiStartingMaxSize)
	{
		m_uiCurrMaxSize = uiStartingMaxSize;
		m_pData = Alloc(m_uiCurrMaxSize);
	};
	~FFixedVector(){
		Free(m_pData, m_uiCurrSize);
	};
	void operator = (const THIS_TYPE& RHS){
		Destroy(m_pData, m_uiCurrSize);
		CopyMin(RHS);
	};

    bool operator == (const THIS_TYPE& RHS) const {
        bool bResult = false;

        if( m_uiCurrSize != RHS.m_uiCurrSize )
        {
            goto Cleanup;
        }

        for( UINT uIdx = 0; uIdx < m_uiCurrSize; ++uIdx )
        {
            if( m_pData[ uIdx ] != RHS.m_pData[ uIdx ] )
            {
                goto Cleanup;
            }
        }
        bResult = true;

Cleanup:

		return bResult;

	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Reserve space for up to N elements inside the vector
	/////////////////////////////////////////////////////////////////////////////////////
	void reserve(unsigned int uiResSize )
	{
		if (m_uiCurrMaxSize == 0)
		{
			m_uiCurrMaxSize = uiResSize;
			m_pData = Alloc(m_uiCurrMaxSize);
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////
	// Resize the vector to at least N elements
	/////////////////////////////////////////////////////////////////////////////////////
	void resize(unsigned int uiNewSize )
	{
		reserve(uiNewSize);
		if (uiNewSize <= m_uiCurrMaxSize){
			if(!bPODType)
			{
				for(unsigned int i = m_uiCurrSize; i < uiNewSize; ++i)
					new( (void*)&m_pData[i] )T();
			}
			m_uiCurrSize = uiNewSize;
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//Add/remove elements
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int push_back(const T& element){
		if( m_uiCurrSize < m_uiCurrMaxSize )
		{
			new( (void*)&m_pData[m_uiCurrSize] )T(element);
			return m_uiCurrSize++;
		}
		return m_uiCurrSize;
	};

	//Add n elements to the end of the vector
	void push_back( const T* pElements,  unsigned int uiNum)
	{
		unsigned int uNewSize = uiNum + m_uiCurrSize;
		FAssert(uNewSize < m_uiCurrMaxSize);
		if( uNewSize <= m_uiCurrMaxSize )
		{
			if(bPODType)
				memcpy( (void*) &m_pData[m_uiCurrSize], pElements, sizeof(T) * uiNum );
			else
				for(unsigned int i = m_uiCurrSize; i < uNewSize; ++i)
					new( (void*)&m_pData[i] )T(*(pElements++));

			m_uiCurrSize = uNewSize;
		}
	}
    // remove the element pointed to by 'it' and shrink the list
    void erase( iterator it )
	{
		unsigned int uIndex = it - m_pData;

        for(unsigned int i = uIndex; i + 1< m_uiCurrSize; ++i)
        {
            m_pData[i] = m_pData[i+1];
        }
		
		--m_uiCurrSize;
	}

	//Add uiNum copies of element to the end of the vector.
	void push_back_copy( const T& element, unsigned int uiNum  )
	{
		uiNum += m_uiCurrSize;
		FAssert(uiNum <= m_uiCurrMaxSize);
		if( uiNum <= m_uiCurrMaxSize )
		{
			for(unsigned int i = m_uiCurrSize; i < uiNum; ++i){
				new( (void*)&m_pData[i] )T(element);
			}
			m_uiCurrSize = uiNum;
		}
	};

protected:

	//Copy list of elements, calling copy constructor for each element
	void CopyMin(const THIS_TYPE& RHS){
		m_uiCurrSize = MIN(RHS.m_uiCurrSize, m_uiCurrMaxSize);
		if( bPODType ){
			memcpy( (void*)m_pData, (void*)RHS.m_pData, sizeof(T)*m_uiCurrSize);
		}else{
			for(unsigned int i = 0; i < m_uiCurrSize; ++i){
				new( (void*)&m_pData[i] )T( RHS.m_pData[i] );
			}
		}
	};

	//Allocate memory as bytes
	T* Alloc(unsigned int uiSize){
		if (uiSize > 0)
		{
			T* pRet = (T*)FMALLOCALIGNED( uiSize*sizeof(T), __alignof(T), AllocPool, nSubID );
			m_uiCurrMaxSize = uiSize;
			return pRet;
		}
		return NULL;
	};

	//Free the memory store.
	void Free(T* pVal, unsigned int uiNumElements)
	{
		if( !bPODType) Destroy(pVal, uiNumElements);
		FFREEALIGNED( (unsigned char*)pVal );
	};

	template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
	friend void* operator new( size_t uiSize, FFixedVector< T, bPODType, AllocPool, nSubID >& kVector );
};

#if defined(_WIN32) || defined(_WIN64)
#pragma warning( pop )
#endif

