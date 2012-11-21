/*
Two fast list classes designed to be as simple as possible while retaining 
the basic behavior of std::list.  These class work correctly with aligned data
types, which STL does not always do.

Author: John Kloetzli
9/9/2008

version 1.4
*/

#ifndef FAST_LIST
#define FAST_LIST

#include "assert.h"
#include "limits.h"
#include "iterator"
#include "FFastVector.h"
#include "FFastAllocator.h"
#include "FPrefetch.h"


#define ANCHOR_NODE_INDEX 0x0fffffff
#define DELETED_MASK (0x80000000)


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Helper structures to handle connectivity between nodes in lists
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
// Policy for each list in a FMultiList.  Basically holds the size, start
// and end pointers of a single list.
////////////////////////////////////////////////////////////////////////
struct NullListPolicy
{
	NullListPolicy() : uiSize( 0 ),uiFirst( ANCHOR_NODE_INDEX ),uiLast( ANCHOR_NODE_INDEX ){};
	NullListPolicy( unsigned int uiSize, unsigned int uiFirst, unsigned int uiLast )
		: uiSize( uiSize ), uiFirst( uiFirst ), uiLast( uiLast ) {};
	unsigned int uiSize, uiFirst, uiLast;
};


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct ListNode{
	unsigned int uiNext;		//The index of the next node (top bit is whether the node has been deleted)
	unsigned uiPrev : 31;		//The index of the prev node
	unsigned bValid : 1;
};
struct NullMultiListNodePolicy
{
	//Array of list connectivity
	ListNode node;

	//These six functions are required for the policy to work
	unsigned int LIST_GetNext() const{ return node.uiNext; };
	unsigned int LIST_GetPrev() const{ return node.uiPrev; };
	void LIST_SetNext(unsigned int uiNodeIndex){ node.uiNext = uiNodeIndex; };
	void LIST_SetPrev(unsigned int uiNodeIndex){ node.uiPrev = uiNodeIndex; };
	bool LIST_GetDeleted() const{ return !node.bValid; };
	void LIST_SetDeleted(bool bDeleted){ node.bValid = !bDeleted; };

	//These six functions are required for the node to be allocated correctly
	unsigned int ALLOC_GetNext() const{ return node.uiNext; };
	void ALLOC_SetNext(unsigned int uiNodeIndex){ node.uiNext = uiNodeIndex; };
	bool ALLOC_GetDeleted() const{ return LIST_GetDeleted(); };
	void ALLOC_SetDeleted(bool bDeleted){ LIST_SetDeleted( bDeleted ); };
};
template<class T> struct MultiListNodePolicy : public NullMultiListNodePolicy
{
	MultiListNodePolicy(){};
	MultiListNodePolicy( const T& x ) : data( x ){};

	//User data
	T data;
};


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//Pre-declarations
////////////////////////////////////////////////////////////////////////

template< 
	class T,
	unsigned int AllocPool = c_eMPoolTypeContainer,
	unsigned int SubID = 0
> class FFastList;

template<
	class T,
	class T_ALLOCATOR = FFastAllocator< T >,
	bool bReferenceToAlloc = false
> class FCustomList;


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// Basic linked list class.  Implemented with an FCustomList with default settings.
// This is basically a convenience class to hide the underlying policy for each 
// node.  If you know what you are doing, you should really use FCustomList directly.
////////////////////////////////////////////////////////////////////////
template< class T, unsigned int AllocPool, unsigned int SubID > class FFastList 
	: protected FCustomList< MultiListNodePolicy< T >, FFastAllocator< MultiListNodePolicy< T >, false, AllocPool, SubID > >
{
public:
	// Typdefs for this type and the base type
	typedef FFastList< T, AllocPool, SubID > TYPE;
	typedef FFastAllocator< MultiListNodePolicy< T >, false, AllocPool, SubID > ALLOC_TYPE;
	typedef FCustomList< MultiListNodePolicy< T >, ALLOC_TYPE > BASE_TYPE;


	////////////////////////////////////////////////////////////////////////
	// Override the iterators to return the data type without the containing structure
	////////////////////////////////////////////////////////////////////////

	// non-const iterator
	class iterator : public BASE_TYPE::iterator{
	public:
		explicit iterator(){};
		explicit iterator( unsigned int uiPos, TYPE* pVec ) 
			: BASE_TYPE::iterator( uiPos, (BASE_TYPE*)pVec ) {};
		~iterator(){};

		T& operator*(){  return BASE_TYPE::iterator::operator*().data; };
		T* operator->(){  return &( BASE_TYPE::iterator::operator*().data );  };
	};

	// const iterator
	class const_iterator : public BASE_TYPE::const_iterator{
	public:
		explicit const_iterator(){};
		explicit const_iterator( unsigned int uiPos, const TYPE* pVec ) 
			: BASE_TYPE::const_iterator( uiPos, (const BASE_TYPE*)pVec ) {};
		~const_iterator(){};

		const T & operator*() const{  return BASE_TYPE::const_iterator::operator*().data;  };
		const T* operator->() const{  return &( BASE_TYPE::const_iterator::operator*().data );  };
	};



	////////////////////////////////////////////////////////////////////////
	// Constructors and copy constructor
	////////////////////////////////////////////////////////////////////////
	explicit FFastList() : BASE_TYPE() {};
	explicit FFastList( unsigned int uiCapacity ) : BASE_TYPE(uiCapacity) {};
	explicit TYPE( const TYPE& rhs ) : BASE_TYPE( rhs ) {};

	//Copy operator
	const TYPE& operator = ( const TYPE& rhs ){  
		return BASE_TYPE::operator= ( rhs );  
	};

	////////////////////////////////////////////////////////////////////////
	// Methods to insert/remove elements from the list.
	////////////////////////////////////////////////////////////////////////
	unsigned int insert( iterator it, const T& x ){
		return BASE_TYPE::insert( it, MultiListNodePolicy< T >( x ) );
	};
	unsigned int push_front( const T& x ){
		return BASE_TYPE::push_front( MultiListNodePolicy< T >(x) );
	};
	unsigned int push_back( const T& x ){
		return BASE_TYPE::push_back( MultiListNodePolicy< T >(x) );
	};

	////////////////////////////////////////////////////////////////////////
	//Getters
	////////////////////////////////////////////////////////////////////////
	T& front() { return BASE_TYPE::front().data; };
	T& back() { return BASE_TYPE::back().data; };
	const T& front() const { return BASE_TYPE::front().data; };
	const T& back() const { return BASE_TYPE::back().data; };
	bool empty() const{ return BASE_TYPE::empty(); };
	unsigned int size() const{ return BASE_TYPE::size(); };

	////////////////////////////////////////////////////////////////////////
	//Clear the entire list
	////////////////////////////////////////////////////////////////////////
	void clear(){ BASE_TYPE::clear(); };

	////////////////////////////////////////////////////////////////////////
	// Member functions which get/use various iterators
	////////////////////////////////////////////////////////////////////////
	iterator begin(){ return iterator( m_uiFirst, this ); };
	iterator end(){ return iterator( ANCHOR_NODE_INDEX, this ); };
	const_iterator begin() const{ return const_iterator( m_uiFirst, this ); };
	const_iterator end() const{ return const_iterator( ANCHOR_NODE_INDEX, this ); };
	const_iterator begin_const() const{ return const_iterator( m_uiFirst, this ); };
	const_iterator end_const() const{ return const_iterator( ANCHOR_NODE_INDEX, this ); };

	iterator get_iterator( unsigned int index ){ 
		assert( index == ANCHOR_NODE_INDEX || get_allocator().is_element_valid( index ) );
		return iterator( index, this );
	};
	const_iterator get_iterator_const( unsigned int index ) const{ 
		assert( index == ANCHOR_NODE_INDEX || get_allocator().is_element_valid( index ) );
		return const_iterator( index, this );
	};


	////////////////////////////////////////////////////////////////////////
	// Methods to remove nodes
	//
	// NOTE: For erase(), not using get_iterator because the returned iterator
	// may refer to the ANCHOR_NODE_INDEX, which would cause an assert if the
	// get_iterator function were used.
	////////////////////////////////////////////////////////////////////////
	iterator erase( unsigned int uiIndex ){ 
		return iterator( BASE_TYPE::erase( uiIndex ).get_index(), this ); 
	};
	iterator erase( iterator it ){
		return iterator( BASE_TYPE::erase( it ).get_index(), this );
	};
	iterator erase( iterator it, iterator jt ){
		return iterator( BASE_TYPE::erase( it, jt ).get_index(), this );
	};
	void pop_front(){ return BASE_TYPE::pop_front(); };
	void pop_back(){ return BASE_TYPE::pop_back(); };

	////////////////////////////////////////////////////////////////////////
	// Accessors to direct elements in the array.  These accessors are not in the STL but
	// make the list much more useful because you can keep references to individual elements.
	// NOTE: These indices are valid as long as you do not delete the item stored at that
	// index, even if you add/remove other items.
	////////////////////////////////////////////////////////////////////////
	bool is_element_valid( unsigned int i ) const{ return BASE_TYPE::is_element_valid( i ); };
	T& get_element( unsigned int i ) { return BASE_TYPE::get_element( i ).data; };
	const T& get_element( unsigned int i ) const{ return BASE_TYPE::get_element( i ).data; };


	const ALLOC_TYPE& get_allocator() const{ return m_kAllocator; };
	ALLOC_TYPE& get_allocator(){ return m_kAllocator; };

};



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// The core class contains all functionality without an allocator or 
// public constructors.  It gets "sandwiched" between the head and tail
// classes, which provide constructors and allocator, respectively.
////////////////////////////////////////////////////////////////////////
template< class T, class T_ALLOCATOR, class TAIL >class FCustomList_Core : public TAIL
{
public:
	typedef FCustomList_Core< T, T_ALLOCATOR, TAIL > TYPE;

	////////////////////////////////////////////////////////////////////////
	// iterator tail determines whether the iterator has const or non-const access
	////////////////////////////////////////////////////////////////////////
	class base_iterator_tail_const{
	public:
		base_iterator_tail_const( const TYPE* pFastList ) : m_pFastList( pFastList ) {};
		const TYPE* m_pFastList;
	};
	class base_iterator_tail{
	public:
		base_iterator_tail( TYPE* pFastList ) : m_pFastList( pFastList ) {};
		TYPE* m_pFastList;
	};


	////////////////////////////////////////////////////////////////////////
	// Base iterator class which defined all iterator-ness except whether
	// access is const or non-const.
	////////////////////////////////////////////////////////////////////////
	template< class TAIL >
	class base_iterator : 
		public std::iterator<std::bidirectional_iterator_tag, MultiListNodePolicy<T> >, 
		public TAIL
	{
	public:
		explicit base_iterator() : m_uiCurrPos( ANCHOR_NODE_INDEX ), TAIL(NULL) {};
		explicit base_iterator( unsigned int uiPos, TYPE* pVec )
			: m_uiCurrPos( uiPos ), TAIL( pVec ) {};
		explicit base_iterator( unsigned int uiPos, const TYPE* pVec )
			: m_uiCurrPos( uiPos ), TAIL( pVec ) {};

		~base_iterator(){};

		const base_iterator operator++(int notused){
			base_iterator temp = *this;
			++(*this);
			return temp;
		};
		base_iterator& operator++(){
			if( m_uiCurrPos == ANCHOR_NODE_INDEX ){
				m_uiCurrPos = m_pFastList->m_uiFirst;
			}else{
				m_uiCurrPos = m_pFastList->get_allocator()[ m_uiCurrPos ].LIST_GetNext();
			}
			return *this;
		};

		base_iterator& Inc_prefetch_next(){
			if( m_uiCurrPos == ANCHOR_NODE_INDEX ){
				m_uiCurrPos = m_pFastList->m_uiFirst;
			}else{
				m_uiCurrPos = m_pFastList->get_allocator()[ m_uiCurrPos ].LIST_GetNext();
			}

			if( m_uiCurrPos!=ANCHOR_NODE_INDEX ) {
				const uint uiNextPos = m_pFastList->get_allocator()[ m_uiCurrPos ].LIST_GetNext();
				if( uiNextPos!=ANCHOR_NODE_INDEX )
					PrefetchRegion(reinterpret_cast<const char*>(m_pFastList->get_allocator().GetStoragePtr())+(uiNextPos*sizeof(MultiListNodePolicy<T>)), sizeof(MultiListNodePolicy<T>) );
			}
			return *this;
		};
		const base_iterator operator--(int notused){
			base_iterator temp = *this;
			++(*this);
			return *this;
		}
		base_iterator& operator--(){
			if( m_uiCurrPos == ANCHOR_NODE_INDEX ){
				m_uiCurrPos = m_pFastList->m_uiLast;
			}else{
				m_uiCurrPos = m_pFastList->get_allocator()[m_uiCurrPos].LIST_GetPrev();
			}
			return *this;
		};


		bool operator== (const base_iterator& rhs) const{
			return m_uiCurrPos == rhs.m_uiCurrPos;
		};

		bool operator!= (const base_iterator& rhs) const{
			return m_uiCurrPos != rhs.m_uiCurrPos;
		};

		//Determine if this position is valid, or if it has been deleted.
		bool is_valid(){
			return !m_pFastList->get_allocator()[ m_uiCurrPos ].LIST_IsDeleted();
		};

		//Get the index into the array that the iterator is currently pointing to.
		unsigned int get_index() const{
			return m_uiCurrPos;
		};

	protected:
		unsigned int m_uiCurrPos;
		friend class TYPE;
	};

	////////////////////////////////////////////////////////////////////////
	// non-const iterator
	////////////////////////////////////////////////////////////////////////
	class iterator : public base_iterator< base_iterator_tail >{
	public:
		typedef base_iterator< base_iterator_tail > BASE;

		explicit iterator(){};
		explicit iterator( unsigned int uiPos, TYPE* pVec )
			: BASE( uiPos, pVec ) {};
		~iterator(){};

		T& operator*(){
			return m_pFastList->get_allocator()[ m_uiCurrPos ];
		};
		T* operator->(){
			return &m_pFastList->get_allocator()[ m_uiCurrPos ];
		};
	};

	////////////////////////////////////////////////////////////////////////
	// const iterator
	////////////////////////////////////////////////////////////////////////
	class const_iterator : public base_iterator< base_iterator_tail_const >{
	public:
		typedef base_iterator< base_iterator_tail_const > BASE;

		explicit const_iterator(){};
		explicit const_iterator( unsigned int uiPos, const TYPE* pVec )
			: BASE( uiPos, pVec ) {};
		~const_iterator(){};

		const T & operator*() const{
			return m_pFastList->get_allocator()[ m_uiCurrPos ];
		};
		const T* operator->() const{
			return &m_pFastList->get_allocator()[ m_uiCurrPos ];
		};
	};

	////////////////////////////////////////////////////////////////////////
	// Protected constructors are only called by the "head" superclass.
	////////////////////////////////////////////////////////////////////////
protected:
	explicit FCustomList_Core()
		: TAIL(), m_uiFirst( ANCHOR_NODE_INDEX ), m_uiLast( ANCHOR_NODE_INDEX ), m_uiSize(0) {};

	explicit FCustomList_Core( unsigned int uiCapacity )
		: TAIL(uiCapacity), m_uiFirst( ANCHOR_NODE_INDEX ), m_uiLast( ANCHOR_NODE_INDEX ), m_uiSize(0) {};

	explicit FCustomList_Core( T_ALLOCATOR* pAlloc )
		: TAIL( pAlloc ), m_uiFirst( ANCHOR_NODE_INDEX ), m_uiLast( ANCHOR_NODE_INDEX ), m_uiSize(0) {};

	explicit FCustomList_Core( const TYPE& rhs )
		: m_uiFirst( rhs.m_uiFirst), m_uiLast( rhs.m_uiLast ), m_uiSize( rhs.m_uiSize ), TAIL( rhs ) {};

public:

	//Copy operator
	const TYPE& operator = ( const TYPE& rhs )
	{
		m_uiFirst = rhs.m_uiFirst;
		m_uiLast = rhs.m_uiLast;
		m_uiSize = rhs.m_uiSize;

		TAIL::operator = ( rhs );

		return *this;
	};

	////////////////////////////////////////////////////////////////////////
	// Methods to insert/remove elements from the list
	////////////////////////////////////////////////////////////////////////

	//Insert a new element
	unsigned int insert( iterator it, const T& x ){
		unsigned int uiNewIndex = this->get_allocator().Alloc( x );
		insert_existing( it, uiNewIndex );
		return uiNewIndex;
	};
	//Insert an element which has already been allocated
	void insert_existing( iterator it, unsigned int uiNewIndex ){
		assert( this->get_allocator().is_element_valid(uiNewIndex) );
		unsigned int uiCurrIndex = it.get_index();

		if( uiCurrIndex == ANCHOR_NODE_INDEX ){
			push_back_existing( uiNewIndex );
		}else{
			InsertBefore( uiNewIndex, uiCurrIndex );
		}

		m_uiSize++;
		if( uiCurrIndex == m_uiFirst )
			m_uiFirst = uiNewIndex;
		if( uiCurrIndex == ANCHOR_NODE_INDEX )
			m_uiLast = uiNewIndex;
	};


	//Push a new element to the front of a list
	unsigned int push_front( const T& x ){
		unsigned int uiNewIndex = get_allocator().Alloc( x );
		push_front_existing( uiNewIndex );
		return uiNewIndex;
	};
	//Push an already-allocated element to the front of a list
	void push_front_existing( unsigned int uiNewIndex ){
		unsigned int uiOldFirst = m_uiFirst;
		m_uiFirst = uiNewIndex;
		m_uiSize++;
		if( m_uiLast == ANCHOR_NODE_INDEX ) m_uiLast = m_uiFirst;
		InsertBefore( m_uiFirst, uiOldFirst );
	};


	//Push a new element to the back of a list
	unsigned int push_back( const T& x )
	{
		unsigned int uiNewIndex = get_allocator().Alloc( x );
		push_back_existing( uiNewIndex );
		return uiNewIndex;
	};
	//Push an already-allocated element to the back of a list
	void push_back_existing( unsigned int uiNewIndex ){
		unsigned int uiOldLast = m_uiLast;
		m_uiLast = uiNewIndex;
		m_uiSize++;
		if( m_uiFirst == ANCHOR_NODE_INDEX ) m_uiFirst = m_uiLast;
		InsertAfter(m_uiLast, uiOldLast);
	};


	T& front() {
		return get_allocator()[ m_uiFirst ];
	};
	T& back() {
		return get_allocator()[ m_uiLast ];
	};
	unsigned int front_index() const { return m_uiFirst; };
	unsigned int back_index() const { return m_uiLast; };
	const T& front() const {
		return get_allocator()[ m_uiFirst ];
	};
	const T& back() const {
		return get_allocator()[ m_uiLast ];
	};

	bool empty() const{
		return ( m_uiSize == 0 );
	};
	unsigned int size() const{
		return m_uiSize;
	};

	//Clear the entire list
	void clear(){
		UnLink( m_uiFirst, ANCHOR_NODE_INDEX );
	};

	////////////////////////////////////////////////////////////////////////
	// Member functions which get/use various iterators
	////////////////////////////////////////////////////////////////////////
	iterator begin(){ return iterator( m_uiFirst, this ); };
	iterator end(){ return iterator( ANCHOR_NODE_INDEX, this ); };
	const_iterator begin() const{ return const_iterator( m_uiFirst, this ); };
	const_iterator end() const{ return const_iterator( ANCHOR_NODE_INDEX, this ); };


	const_iterator begin_const() const{ return const_iterator( m_uiFirst, this ); };
	const_iterator end_const() const{ return const_iterator( ANCHOR_NODE_INDEX, this ); };

	iterator get_iterator( unsigned int index ){ 
		assert( get_allocator().is_element_valid( index ) );
		return iterator( index, this );
	};
	const_iterator get_iterator_const( unsigned int index ) const{ 
		assert( get_allocator().is_element_valid( index ) )
			return const_iterator( index, this );
	};


	////////////////////////////////////////////////////////////////////////
	// Methods to remove nodes
	//
	// NOTE: For erase(), not using get_iterator because the returned iterator
	// may refer to the ANCHOR_NODE_INDEX, which would cause an assert if the
	// get_iterator function were used.
	////////////////////////////////////////////////////////////////////////
	iterator erase( unsigned int uiIndex ){
		const unsigned int uiNext = get_allocator()[uiIndex].LIST_GetNext();
		UnLink( uiIndex, uiNext );
		return iterator( uiNext, this );
	};
	iterator erase( iterator  it ){
		const unsigned int uiNext = get_allocator()[it.get_index()].LIST_GetNext();
		UnLink( it.get_index(), uiNext );
		return iterator( uiNext, this );
	};
	iterator erase( iterator  it, iterator jt ){
		const unsigned int uiRetIndex = jt.get_index();
		if( it.get_index() != uiRetIndex )
			UnLink( it.get_index(), uiRetIndex );
		return iterator( uiRetIndex, this );
	};
	void pop_front()
	{
		if( m_uiFirst != ANCHOR_NODE_INDEX){
			unsigned int uiNewFront = get_allocator()[ m_uiFirst ].LIST_GetNext();
			UnLink( m_uiFirst, uiNewFront );
			m_uiFirst = uiNewFront;
		}
	};
	void pop_back()
	{
		if( m_uiLast != ANCHOR_NODE_INDEX ){
			unsigned int uiNewBack = get_allocator()[ m_uiLast ].LIST_GetPrev();
			UnLink( m_uiLast, ANCHOR_NODE_INDEX );
			m_uiLast = uiNewBack;
		}
	};

	////////////////////////////////////////////////////////////////////////
	//Accessors which are not in the STL but should be because the STL is stupid.
	////////////////////////////////////////////////////////////////////////
	bool is_element_valid(unsigned int i) const{
		return get_allocator().is_element_valid(i);
	};
	T& get_element( unsigned int i ){
		return get_allocator()[i];
	};
	const T& get_element(unsigned int i) const{
		return get_allocator()[i];
	};

protected:

	//Set the internal links to add element i before element j
	void InsertBefore(unsigned int i, unsigned int j)
	{
		assert( get_allocator().is_element_valid(i) );

		T* a = &get_allocator()[i];
		if( j == ANCHOR_NODE_INDEX ){
			a->LIST_SetNext(ANCHOR_NODE_INDEX);
			a->LIST_SetPrev(ANCHOR_NODE_INDEX);
		}else{
			T* b = &get_allocator()[j];

			//Set the links for the new node
			unsigned int uiBPrev = b->LIST_GetPrev();
			a->LIST_SetNext(j);
			a->LIST_SetPrev( uiBPrev);

			//Fix the links for the next and previous nodes
			if( uiBPrev != ANCHOR_NODE_INDEX )
				get_allocator()[uiBPrev].LIST_SetNext(i);
			b->LIST_SetPrev(i);
		}
	};

	//Set the internal links to add element i after element j
	void InsertAfter(unsigned int i, unsigned int j)
	{
		assert( get_allocator().is_element_valid(i) );

		T* a = &get_allocator()[i];
		if( j == ANCHOR_NODE_INDEX ){
			a->LIST_SetNext(ANCHOR_NODE_INDEX);
			a->LIST_SetPrev(ANCHOR_NODE_INDEX);
		}else{
			T* b = &get_allocator()[j];

			//Set the links for the new node
			unsigned int uiBNext = b->LIST_GetNext();
			a->LIST_SetPrev(j);
			a->LIST_SetNext(uiBNext);

			//Fix the links for the next and previous nodes
			if( uiBNext != ANCHOR_NODE_INDEX )
				get_allocator()[uiBNext].LIST_SetPrev(i);
			b->LIST_SetNext(i);
		}
	};

	//Reverse-Unlink an internal chain of elements and add them to the free space.
	void UnLink( unsigned int uiStart, unsigned int uiEnd ){
		if( uiStart == ANCHOR_NODE_INDEX ){ return; }
		assert( get_allocator().is_element_valid(uiStart) );

		if( uiStart == m_uiFirst){ m_uiFirst = uiEnd; }
		uiStart = get_allocator()[uiStart].LIST_GetPrev();
		if( uiEnd == ANCHOR_NODE_INDEX ){
			uiEnd = m_uiLast;
			m_uiLast = uiStart;
		}else{
			assert( get_allocator().is_element_valid(uiEnd) );
			uiEnd = get_allocator()[uiEnd].LIST_GetPrev();
		}

		while( uiStart != uiEnd ){
			T& kEnd = get_allocator()[uiEnd];
			unsigned int uiNext = kEnd.LIST_GetNext();
			unsigned int uiPrev = kEnd.LIST_GetPrev();
			if( uiNext != ANCHOR_NODE_INDEX ) get_allocator()[uiNext].LIST_SetPrev( uiPrev );
			if( uiPrev != ANCHOR_NODE_INDEX ) get_allocator()[uiPrev].LIST_SetNext( uiNext );
			kEnd.LIST_SetDeleted(true);
			get_allocator().FreeIfDeleted(uiEnd);

			m_uiSize--;

			uiEnd = uiPrev;
		}
	};

	unsigned int m_uiSize, m_uiFirst, m_uiLast;
};

////////////////////////////////////////////////////////////////////////
// Head/Tail pair with the allocator as a data member.  This is the default
// behavior.
////////////////////////////////////////////////////////////////////////
template< class T_ALLOCATOR > class FCustomList_Tail_Member
{
protected:
	typedef FCustomList_Tail_Member< T_ALLOCATOR > TYPE;

	FCustomList_Tail_Member(){};
	FCustomList_Tail_Member( unsigned int uiCapacity ) : m_kAllocator( uiCapacity )  {};
	FCustomList_Tail_Member( const TYPE& rhs ) : m_kAllocator( rhs.m_kAllocator ) {};
	~FCustomList_Tail_Member(){ m_kAllocator.clear(); };

	const TYPE& operator = ( const TYPE& rhs ){
		m_kAllocator = rhs.m_kAllocator;
	};

	T_ALLOCATOR m_kAllocator;

public:
	//Functions to access the allocator
	const T_ALLOCATOR& get_allocator() const{ return m_kAllocator; };
	T_ALLOCATOR& get_allocator(){ return m_kAllocator; };
};
template< class T, class T_ALLOCATOR, bool bReferenceToAlloc > class FCustomList 
: public FCustomList_Core< T, T_ALLOCATOR, FCustomList_Tail_Member< T_ALLOCATOR > >
{
public:
	typedef FCustomList< T, T_ALLOCATOR > TYPE;
	typedef FCustomList_Core< T, T_ALLOCATOR, FCustomList_Tail_Member< T_ALLOCATOR > > CORE;
	typedef FCustomList_Tail_Member< T_ALLOCATOR > TAIL;

	FCustomList() : CORE() {};

	FCustomList( unsigned int uiCapacity )
		: CORE(uiCapacity) {};

	FCustomList( const TYPE& rhs )
		: CORE( rhs ) {};

	const TYPE& operator=( const TYPE& rhs ){
		CORE = rhs;
	};
};

////////////////////////////////////////////////////////////////////////
// Head/Tail pair with the allocator as a pointer which has to be 
// passed to the constructor.  This behavior is selected through
// partial template specialization.
////////////////////////////////////////////////////////////////////////
template< class T_ALLOCATOR > class FCustomList_Tail_Pointer
{
protected:
	typedef FCustomList_Tail_Pointer< T_ALLOCATOR > TYPE;

	FCustomList_Tail_Pointer() : m_pAlloc( NULL ) {};
	FCustomList_Tail_Pointer( T_ALLOCATOR* pAlloc ) : m_pAlloc( pAlloc ) {};
	FCustomList_Tail_Pointer( const TYPE& rhs ) : m_pAlloc( rhs.m_pAlloc ) {};

	const TYPE& operator = ( const TYPE& rhs ){
		m_pAlloc = rhs.m_pAlloc;
		return *this;
	};

	mutable T_ALLOCATOR* m_pAlloc;

public:
	//Functions to access/swap out the allocator
	void set_allocator( T_ALLOCATOR* pAlloc ){ m_pAlloc = pAlloc; };
	T_ALLOCATOR& get_allocator() const { return *m_pAlloc; };

};
template< class T, class T_ALLOCATOR > class FCustomList< T, T_ALLOCATOR, true >
: public FCustomList_Core< T, T_ALLOCATOR, FCustomList_Tail_Pointer< T_ALLOCATOR > >
{
public:
	typedef FCustomList< T, T_ALLOCATOR > TYPE;
	typedef FCustomList_Tail_Pointer< T_ALLOCATOR > TAIL;
	typedef FCustomList_Core< T, T_ALLOCATOR, TAIL > CORE;

	FCustomList() : CORE() {};
	FCustomList( T_ALLOCATOR* pAlloc ) : CORE( pAlloc ) {};
	FCustomList( const TYPE& rhs ) : CORE( rhs ) {};

	const TYPE& operator=( const TYPE& rhs ){
		CORE = rhs;
	};
};


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//Class for holding multiple list instances efficiently.  This class
// is a much more efficient representation for a set of parallel lists 
// than creating multiple FFastList or std::list instances.  
//
//Under the hood this class uses two FFastVectors for memory allocation - 
// one for holding the info about each list (start and end pointers, etc)
// and one to hold all of the elements in all lists.
//
//Template parameters:
//T - the type to store in the FMultiList.  Needs to have the functionality
// of ListFunctions in order to work correctly, so other objects will have
// to be wrapped in a ListFunctions object.
//
//L - The number of elements to statically allocate.
//
//M - The number of lists to statically allocate.  If the number of lists
// to store is known at compile time you can reserve the space for the info
// about each list in the class statically, avoiding a memory allocation and
// possible cache misses.  This will increase the size of the class 
// by L*3*sizeof(unsigned int)
//
//M - The type to store for each list.
// 
////////////////////////////////////////////////////////////////////////

template< 
	class T,
	class T_ALLOCATOR,
	unsigned int AllocPool = c_eMPoolTypeContainer,
	unsigned int nSubID = 0
> class FMultiList
{
public:
	typedef FCustomList< T, T_ALLOCATOR, true > LIST_TYPE;
	typedef FMultiList< T, T_ALLOCATOR, AllocPool, nSubID > TYPE;
	typedef FFastVector< LIST_TYPE, true, AllocPool, nSubID > VECTOR_TYPE;
	typedef typename VECTOR_TYPE::iterator list_iterator;

	////////////////////////////////////////////////////////////////////////
	// non-const iterator
	////////////////////////////////////////////////////////////////////////
	class iterator : public LIST_TYPE::iterator{
	public:
		explicit iterator(){};
		explicit iterator( const typename LIST_TYPE::iterator& it, unsigned int uiList )
			: LIST_TYPE::iterator( it ), m_uiList(uiList) {};
		~iterator(){};

		T& operator*(){
			return LIST_TYPE::iterator::operator* ();
		};
		T* operator->(){
			return LIST_TYPE::iterator::operator-> ();
		};

	protected:
		unsigned int m_uiList;
		friend TYPE;
	};

	////////////////////////////////////////////////////////////////////////
	// const iterator
	////////////////////////////////////////////////////////////////////////
	class const_iterator : public LIST_TYPE::const_iterator{
	public:
		explicit const_iterator(){};
		explicit const_iterator( const typename LIST_TYPE::const_iterator& it, unsigned int uiList )
			: LIST_TYPE::const_iterator( it ), m_uiList(uiList) {};
		
		~const_iterator(){};

		const T& operator*() const{
			return LIST_TYPE::const_iterator::operator* ();
		};
		const T* operator->() const{
			return LIST_TYPE::const_iterator::operator->();
		};

	protected:
		unsigned int m_uiList;
		friend TYPE;
	};

	////////////////////////////////////////////////////////////////////////
	// Constructor takes the initial number of lists to initialize.  Any lists
	// referenced outside of this range will return an error unless the number of
	// lists has been changed using set_min_num_lists()
	////////////////////////////////////////////////////////////////////////
	explicit FMultiList( T_ALLOCATOR& kAlloc )
		:m_pAlloc( & kAlloc ) {};
	explicit FMultiList( unsigned int uiNumLists, T_ALLOCATOR& kAlloc )
		: m_pAlloc( &kAlloc ), m_kVector( uiNumLists ) 
	{
		for( unsigned int i = 0; i < uiNumLists; ++i ){	
			new( m_kVector ) LIST_TYPE( m_pAlloc );
		}
	};

	~FMultiList(){};

	//Copy operator
	const TYPE& operator = ( const TYPE& rhs )
	{
		m_pAlloc = rhs.m_pAlloc;
		m_kVector = rhs.m_kVector;
	};

	////////////////////////////////////////////////////////////////////////
	// Methods to insert/remove elements from the list
	////////////////////////////////////////////////////////////////////////

	//Insert a new element
	unsigned int insert( iterator it, const T& x, unsigned int uiList){
		return m_kVector[uiList].insert( it, x );
	};
	//Insert an element which has already been allocated
	void insert_existing( iterator it, unsigned int uiNewIndex, unsigned int uiList){
		return m_kVector[uiList].insert_existing( it, uiNewIndex );
	};


	//Push a new element to the front of a list
	unsigned int push_front( const T& x, unsigned int uiList){
		return m_kVector[uiList].push_front(x);
	};
	//Push an already-allocated element to the front of a list
	void push_front_existing( unsigned int uiNewIndex, unsigned int uiList){
		return m_kVector[uiList].push_front_existing( uiNewIndex );
	};


	//Push a new element to the back of a list
	unsigned int push_back( const T& x, unsigned int uiList){
		return m_kVector[uiList].push_back(x);
	};
	//Push an already-allocated element to the back of a list
	void push_back_existing( unsigned int uiNewIndex, unsigned int uiList ){
		return m_kVector[uiList].push_back_existing( uiNewIndex );
	};


	T& front(unsigned int uiList) {
		return m_kVector[uiList].front();
	};
	T& back(unsigned int uiList) {
		return m_kVector[uiList].back();
	};
	const T& front(unsigned int uiList) const {
		return m_kVector[uiList].front();
	};
	const T& back(unsigned int uiList) const {
		return m_kVector[uiList].back();
	};

	bool empty(unsigned int uiList) const{
		return m_kVector[uiList].empty();
	};
	unsigned int size(unsigned int uiList) const{
		return m_kVector[uiList].size();
	};
	unsigned int num_lists() const{
		return m_kVector.size();
	};

	//Extend the FMultiLists to include at least this many lists
	void set_min_num_lists(unsigned int uiNumLists){
		while( m_kVector.size() < uiNumLists ){
			new( m_kVector ) LIST_TYPE( m_pAlloc );
		}
	};

	//Iterators for the list set
	list_iterator list_begin(){
		return m_kVector.begin();	
	};
	list_iterator list_end(){
		return m_kVector.end();
	};

	//Clear a specific sublist
	void clear(unsigned int uiList){
		m_kVector[uiList].clear();
	};

	//Clear all sublists
	void clear(){
		for( unsigned int i = 0; i < m_kVector.size(); i++ ){
			m_kVector[i].clear();
		}
	};

	//Clear the vector of lists
	void clear_lists_vec(){
		clear();
		m_kVector.clear();
	};

	////////////////////////////////////////////////////////////////////////
	// Member functions which get/use various iterators
	////////////////////////////////////////////////////////////////////////
	iterator begin( unsigned int uiList ){  return iterator( m_kVector[uiList].begin(), uiList );  };
	iterator end( unsigned int uiList ){  return iterator( m_kVector[uiList].end(), uiList );  };
	const_iterator begin( unsigned int uiList ) const{  return const_iterator( m_kVector[uiList].begin(), uiList );  };
	const_iterator end( unsigned int uiList ) const{  return const_iterator( m_kVector[uiList].end(), uiList );  };

	const_iterator begin_const( unsigned int uiList ) const{ return const_iterator( m_kVector[uiList].begin_const(), uiList); };
	const_iterator end_const( unsigned int uiList ) const{ return const_iterator( m_kVector[uiList].end_const(), uiList); };

	iterator get_iterator( unsigned int index, unsigned int uiList ){ 
		return iterator( m_kVector[uiList].get_iterator(index), uiList );
	};
	const_iterator get_iterator_const( unsigned int index, unsigned int uiList ) const{ 
		return const_iterator( m_kVector[uiList].get_iterator(index), uiList );
	};


	////////////////////////////////////////////////////////////////////////
	// Methods to remove nodes
	//
	// NOTE: For erase(), not using get_iterator because the returned iterator
	// may refer to the ANCHOR_NODE_INDEX, which would cause an assert if the
	// get_iterator function were used.
	////////////////////////////////////////////////////////////////////////
	iterator erase( unsigned int uiIndex, unsigned int uiList ){
		return iterator( m_kVector[uiList].erase( uiIndex ), uiList );
	};
	iterator erase( iterator it ){
		return iterator( m_kVector[it.m_uiList].erase( it.get_index() ), it.m_uiList );
	};
	iterator erase( iterator it, iterator jt ){
		assert( it.m_uiList == jt.m_uiList );
		return iterator( m_kVector[it.m_uiList].erase( it, jt), it.m_uiList );
	};
	void pop_front( unsigned int uiList ){
		return m_kVector[ uiList ].pop_front();
	};
	void pop_back( unsigned int uiList ){
		return m_kVector[ uiList ].pop_back();
	};

	////////////////////////////////////////////////////////////////////////
	//Accessors which are not in the STL but should be because the STL is stupid.
	////////////////////////////////////////////////////////////////////////
	bool is_element_valid(unsigned int i) const{
		return m_pAlloc->is_element_valid(i);
	};
	T& get_element(unsigned int i) const{
		return (*m_pAlloc)[i];
	};

protected:

	VECTOR_TYPE m_kVector;	//Data for each element of the list
	T_ALLOCATOR* m_pAlloc;
};


template< 
	class T, 
	class T_ALLOCATOR = FFastAllocator< T >,
	unsigned int J = 1,
	class M = NullListPolicy
> class FMultiList2;

template< 
	class T,
	class T_ALLOCATOR,
	unsigned int J,
	class M
> class FMultiList2
{
public:
	typedef FMultiList2< T, T_ALLOCATOR, J, M > TYPE;
	typedef M* list_iterator;	//Iterator through the lists

private:

protected:

	////////////////////////////////////////////////////////////////////////
	// Base iterator class which defined all iterator-ness except whether
	// access is const or non-const.
	////////////////////////////////////////////////////////////////////////
	class base_iterator : public std::iterator<std::bidirectional_iterator_tag, T>{
	public:
		explicit base_iterator() : m_uiCurrPos(ANCHOR_NODE_INDEX), m_uiCurrList(0){};
		explicit base_iterator(unsigned int uiPos, const TYPE* pVec, unsigned int uiList)
			: m_uiCurrPos(uiPos), m_pFastList(pVec), m_uiCurrList(uiList)
		{};
		~base_iterator(){};

		const base_iterator operator++(int notused){
			base_iterator temp = *this;
			if( m_uiCurrPos == ANCHOR_NODE_INDEX ){
				m_uiCurrPos = m_pFastList->m_aLD[m_uiCurrList].uiFirst;
			}else{
				m_uiCurrPos = (*m_pFastList->m_pVec)[m_uiCurrPos].LIST_GetNext();
			}
			return temp;
		};
		base_iterator& operator++(){
			if( m_uiCurrPos == ANCHOR_NODE_INDEX ){
				m_uiCurrPos = m_pFastList->m_aLD[m_uiCurrList].uiFirst;
			}else{
				m_uiCurrPos = (*m_pFastList->m_pVec)[m_uiCurrPos].LIST_GetNext();
			}
			return *this;
		};
		base_iterator& operator--(){
			if( m_uiCurrPos == ANCHOR_NODE_INDEX ){
				m_uiCurrPos = m_pFastList->m_aLD[m_uiCurrList].uiLast;
			}else{
				m_uiCurrPos = (*m_pFastList->m_pVec)[m_uiCurrPos].LIST_GetPrev();
			}
			return *this;
		};
		const base_iterator operator--(int notused){
			base_iterator temp = *this;
			if( m_uiCurrPos == ANCHOR_NODE_INDEX ){
				m_uiCurrPos = m_pFastList->m_aLD[m_uiCurrList].uiLast;
			}else{
				m_uiCurrPos = (*m_pFastList->m_pVec)[m_uiCurrPos].LIST_GetPrev();
			}
			return *this;
		}


		bool operator== (const base_iterator& rhs) const{
			return m_uiCurrPos == rhs.m_uiCurrPos;
		};

		bool operator!= (const base_iterator& rhs) const{
			return m_uiCurrPos != rhs.m_uiCurrPos;
		};

		//Determine if this position is valid, or if it has been deleted.
		bool is_valid(){
			return !m_pFastList->get_element(m_uiCurrPos).LIST_IsDeleted();
		};

		//Get the index into the array that the iterator is currently pointing to.
		unsigned int get_index() const{
			return m_uiCurrPos;
		};
		unsigned int get_list() const{
			return m_uiCurrList;
		};

	protected:
		unsigned int m_uiCurrList;
		unsigned int m_uiCurrPos;
		const TYPE* m_pFastList;
		friend class TYPE;
	};

public:

	////////////////////////////////////////////////////////////////////////
	// non-const iterator
	////////////////////////////////////////////////////////////////////////
	class iterator : public base_iterator{
	public:
		explicit iterator(){};
		explicit iterator(unsigned int uiPos, TYPE* pVec, unsigned int uiList)
			: base_iterator(uiPos, pVec, uiList) {};
		~iterator(){};

		T& operator*() const{
			return (*m_pFastList->m_pVec)[m_uiCurrPos];
		};
		T* operator->() const{
			return &(*m_pFastList->m_pVec)[m_uiCurrPos];
		};
	};

	////////////////////////////////////////////////////////////////////////
	// const iterator
	////////////////////////////////////////////////////////////////////////
	class const_iterator : public base_iterator{
	public:
		explicit const_iterator(){};
		explicit const_iterator(unsigned int uiPos, const TYPE* pVec, unsigned int uiList)
			: base_iterator(uiPos, pVec, uiList) {};
		~const_iterator(){};

		const T& operator*() const{
			return (*m_pFastList->m_pVec)[m_uiCurrPos];
		};
		const T* operator->() const{
			return &(*m_pFastList->m_pVec)[m_uiCurrPos];
		};
	};

	////////////////////////////////////////////////////////////////////////
	// Constructor takes the initial number of lists to initialize.  Any lists
	// referenced outside of this range will return an error unless the number of
	// lists has been changed using set_min_num_lists()
	////////////////////////////////////////////////////////////////////////
	explicit FMultiList2()
		: m_pVec( &m_DefaultVec ) 
	{
		set_min_num_lists(J);	
	};

	explicit FMultiList2( unsigned int uiCapacity )
		: m_DefaultVec( uiCapacity ),
		m_pVec( &m_DefaultVec )
	{
		set_min_num_lists(J);
	};
	explicit FMultiList2(unsigned int uiCapacity, unsigned int uiNumLists)
		: m_aLD( uiNumLists ),
		m_DefaultVec( uiCapacity ),
		m_pVec( &m_DefaultVec )
	{
		set_min_num_lists(J);
		m_aLD.push_back_copy(M(), uiNumLists);
	};
	explicit FMultiList2( const TYPE& rhs )
		: m_aLD( rhs.m_aLD ),
		m_DefaultVec( rhs.m_DefaultVec ),
		m_pVec( &m_DefaultVec )
	{
		//If the allocator was overridden, use the overridden one
		if( rhs.m_pVec != &rhs.m_DefaultVec ){
			m_pVec = rhs.m_pVec;
		}
	};

	explicit FMultiList2( unsigned int uiNumLists, T_ALLOCATOR& alloc )
		: m_aLD( uiNumLists ),
		m_pVec( &alloc )
	{
		m_aLD.push_back_copy(M(), uiNumLists);
	};
	~FMultiList2(){};

	//Copy operator
	const TYPE& operator = ( const TYPE& rhs )
	{
		m_DefaultVec = rhs.m_DefaultVec;
		m_aLD = rhs.m_aLD;
		
		//Overridden allocators are preserved across assignment
		m_pVec = &m_DefaultVec;
		if( rhs.m_pVec != &rhs.m_DefaultVec ){
			m_pVec = rhs.m_pVec;
		}

		return *this;
	};

	////////////////////////////////////////////////////////////////////////
	// Methods to insert/remove elements from the list
	////////////////////////////////////////////////////////////////////////

	//Insert a new element
	unsigned int insert( iterator it, const T& x, unsigned int uiList){
		unsigned int uiNewIndex = m_pVec->Alloc(x);
		insert_existing( it, uiNewIndex, uiList );
		return uiNewIndex;
	};
	//Insert an element which has already been allocated
	void insert_existing( iterator it, unsigned int uiNewIndex, unsigned int uiList){
		assert( m_pVec->is_element_valid(uiNewIndex) );
		unsigned int uiCurrIndex = it.get_index();
		InsertBefore(uiNewIndex, uiCurrIndex);

		M& kLD = m_aLD[uiList];
		kLD.uiSize++;
		if( uiCurrIndex == kLD.uiFirst )
			kLD.uiFirst = uiNewIndex;
		if( uiCurrIndex == ANCHOR_NODE_INDEX )
			kLD.uiLast = kLD.uiFirst;
	};


	//Push a new element to the front of a list
	unsigned int push_front( const T& x, unsigned int uiList){
		unsigned int uiNewIndex = m_pVec->Alloc(x);
		push_front_existing(uiNewIndex, uiList);
		return uiNewIndex;
	};
	//Push an already-allocated element to the front of a list
	void push_front_existing( unsigned int uiNewIndex, unsigned int uiList){
		M& kLD = m_aLD[uiList];
		unsigned int uiOldFirst = kLD.uiFirst;
		kLD.uiFirst = uiNewIndex;
		kLD.uiSize++;
		if( kLD.uiLast == ANCHOR_NODE_INDEX ) kLD.uiLast = kLD.uiFirst;
		InsertBefore(kLD.uiFirst, uiOldFirst);
	};


	//Push a new element to the back of a list
	unsigned int push_back( const T& x, unsigned int uiList)
	{
		unsigned int uiNewIndex = m_pVec->Alloc(x);
		push_back_existing(uiNewIndex, uiList);
		return uiNewIndex;
	};
	//Push an already-allocated element to the back of a list
	void push_back_existing( unsigned int uiNewIndex, unsigned int uiList ){
		M& kLD = m_aLD[uiList];
		unsigned int uiOldLast = kLD.uiLast;
		kLD.uiLast = uiNewIndex;
		kLD.uiSize++;
		if( kLD.uiFirst == ANCHOR_NODE_INDEX ) kLD.uiFirst = kLD.uiLast;
		InsertAfter(kLD.uiLast, uiOldLast);
	};


	T& front(unsigned int uiList) {
		return (*m_pVec)[ m_aLD[uiList].uiFirst ];
	};
	T& back(unsigned int uiList) {
		return (*m_pVec)[ m_aLD[uiList].uiLast ];
	};
    const T& front(unsigned int uiList) const {
		return (*m_pVec)[ m_aLD[uiList].uiFirst ];
	};
	const T& back(unsigned int uiList) const {
		return (*m_pVec)[ m_aLD[uiList].uiLast ];
	};

	bool empty(unsigned int uiList) const{
		return size(uiList) == 0;
	};
	unsigned int size(unsigned int uiList) const{
		return m_aLD[uiList].uiSize;
	};
	unsigned int num_lists() const{
		return m_aLD.size();
	};

	//Extend the FMultiLists to include at least this many lists
	void set_min_num_lists(unsigned int uiNumLists){
		if( uiNumLists > num_lists() ){
			m_aLD.push_back_copy(M(), uiNumLists - num_lists() );
		}
	};


	//Get the list policy for a specific list
	M& get_list( unsigned int uiList ){
		return m_aLD[uiList];
	};
	const M& get_list( unsigned int uiList ) const{
		return m_aLD[uiList];
	};
	unsigned int list_push_back( const M& kListData ){
		return m_aLD.push_back(kListData);
	};

	//Iterators for the list set
	list_iterator list_begin(){
		return m_aLD.begin();
	};
	list_iterator list_end(){
		return m_aLD.end();
	};

	//Clear a specific sublist
	void clear(unsigned int uiList){
		UnLink(m_aLD[uiList].uiFirst, ANCHOR_NODE_INDEX, uiList);
	};

	//Clear all sublists
	void clear(){
		unsigned int uiSize = m_aLD.size();
		for( unsigned int i = 0; i <uiSize; ++i){
			if( m_aLD[i].uiSize > 0 ){
				UnLink(m_aLD[i].uiFirst, ANCHOR_NODE_INDEX, i);
			}
		}
	};

	//Clear the vector of lists
	void clear_lists_vec(){
		clear();
		m_aLD.clear();
	};

	////////////////////////////////////////////////////////////////////////
	// Member functions which get/use various iterators
	////////////////////////////////////////////////////////////////////////
	iterator begin(unsigned int uiList){ return iterator(m_aLD[uiList].uiFirst, this, uiList); };
	iterator end(unsigned int uiList){ return iterator(ANCHOR_NODE_INDEX, this, uiList); };
 	const_iterator begin(unsigned int uiList) const{ return const_iterator(m_aLD[uiList].uiFirst, this, uiList); };
	const_iterator end(unsigned int uiList) const{ return const_iterator(ANCHOR_NODE_INDEX, this, uiList); };


	const_iterator begin_const(unsigned int uiList) const{ return const_iterator(m_aLD[uiList].uiFirst, this, uiList); };
	const_iterator end_const(unsigned int uiList) const{ return const_iterator(ANCHOR_NODE_INDEX, this, uiList); };
	
    iterator get_iterator(unsigned int index, unsigned int uiList){ 
		assert( m_pVec->is_element_valid( index ) );
		return iterator(index, this, uiList);
	};
    const_iterator get_iterator_const(unsigned int index, unsigned int uiList) const{ 
		assert( m_pVec->is_element_valid( index ) )
		return const_iterator(index, this, uiList);
	};
    

	////////////////////////////////////////////////////////////////////////
	// Methods to remove nodes
	//
	// NOTE: For erase(), not using get_iterator because the returned iterator
	// may refer to the ANCHOR_NODE_INDEX, which would cause an assert if the
	// get_iterator function were used.
	////////////////////////////////////////////////////////////////////////
	iterator erase( unsigned int uiIndex, unsigned int uiList){
		const unsigned int uiNext = (*m_pVec)[uiIndex].LIST_GetNext();
		UnLink(uiIndex, uiNext, uiList);
		return iterator(uiNext, this, uiList);
	};
	iterator erase( iterator  it ){
		const unsigned int uiNext = (*m_pVec)[it.get_index()].LIST_GetNext();
		UnLink(it.get_index(), uiNext, it.get_list());
		return iterator(uiNext, this, it.get_list());
	};
	iterator erase( iterator  it, iterator jt){
		assert( it.get_list() == jt.get_list() );

		const unsigned int uiRetIndex = jt.get_index();
		if( it.get_index() != uiRetIndex )
			UnLink(it.get_index(), uiRetIndex, it.get_list());
		return iterator(uiRetIndex, this, it.get_list());
	};
	void pop_front(unsigned int uiList)
	{
		M* pLD = &m_aLD[uiList];

		if( pLD->uiFirst != ANCHOR_NODE_INDEX){
			unsigned int uiNewFront = (*m_pVec)[ pLD->uiFirst ].LIST_GetNext();
			UnLink( pLD->uiFirst, uiNewFront, uiList);
			pLD->uiFirst = uiNewFront;
		}
	};
	void pop_back(unsigned int uiList)
	{
		M* pLD = &m_aLD[uiList];

		if( pLD->uiLast != ANCHOR_NODE_INDEX ){
			unsigned int uiNewBack = (*m_pVec)[ pLD->uiLast ].LIST_GetPrev();
			UnLink(pLD->uiLast, ANCHOR_NODE_INDEX, uiList);
			pLD->uiLast = uiNewBack;
		}
	};

	////////////////////////////////////////////////////////////////////////
	//Accessors which are not in the STL but should be because the STL is stupid.
	////////////////////////////////////////////////////////////////////////
	bool is_element_valid(unsigned int i) const{
		return m_pVec->is_element_valid(i);
	};
	T& get_element(unsigned int i) const{
		return (*m_pVec)[i];
	};

protected:

	//Set the internal links to add element i before element j
	void InsertBefore(unsigned int i, unsigned int j)
	{
		assert( m_pVec->is_element_valid(i) );

		T* a = &(*m_pVec)[i];
		if( j == ANCHOR_NODE_INDEX ){
			a->LIST_SetNext(ANCHOR_NODE_INDEX);
			a->LIST_SetPrev(ANCHOR_NODE_INDEX);
		}else{
			T* b = &(*m_pVec)[j];

			//Set the links for the new node
			unsigned int uiBPrev = b->LIST_GetPrev();
			a->LIST_SetNext(j);
			a->LIST_SetPrev( uiBPrev);

			//Fix the links for the next and previous nodes
			if( uiBPrev != ANCHOR_NODE_INDEX )
				(*m_pVec)[uiBPrev].LIST_SetNext(i);
			b->LIST_SetPrev(i);
		}
	};

	//Set the internal links to add element i after element j
	void InsertAfter(unsigned int i, unsigned int j)
	{
		assert( m_pVec->is_element_valid(i) );

		T* a = &(*m_pVec)[i];
		if( j == ANCHOR_NODE_INDEX ){
			a->LIST_SetNext(ANCHOR_NODE_INDEX);
			a->LIST_SetPrev(ANCHOR_NODE_INDEX);
		}else{
			T* b = &(*m_pVec)[j];

			//Set the links for the new node
			unsigned int uiBNext = b->LIST_GetNext();
			a->LIST_SetPrev(j);
			a->LIST_SetNext(uiBNext);

			//Fix the links for the next and previous nodes
			if( uiBNext != ANCHOR_NODE_INDEX )
				(*m_pVec)[uiBNext].LIST_SetPrev(i);
			b->LIST_SetNext(i);
		}
	};

	//Reverse-Unlink an internal chain of elements and add them to the free space.
	void UnLink(unsigned int uiStart, unsigned int uiEnd, unsigned int uiList){
		if( uiStart == ANCHOR_NODE_INDEX ){ return; }
		assert( m_pVec->is_element_valid(uiStart) );

		M& kLD = m_aLD[uiList];
		if( uiStart == kLD.uiFirst){ kLD.uiFirst = uiEnd; }
		uiStart = (*m_pVec)[uiStart].LIST_GetPrev();
		if( uiEnd == ANCHOR_NODE_INDEX ){
			uiEnd = kLD.uiLast;
			kLD.uiLast = uiStart;
		}else{
			assert( m_pVec->is_element_valid(uiEnd) );
			uiEnd = (*m_pVec)[uiEnd].LIST_GetPrev();
		}

		while( uiStart != uiEnd ){
			T& kEnd = (*m_pVec)[uiEnd];
			unsigned int uiNext = kEnd.LIST_GetNext();
			unsigned int uiPrev = kEnd.LIST_GetPrev();
			if( uiNext != ANCHOR_NODE_INDEX ) (*m_pVec)[uiNext].LIST_SetPrev( uiPrev );
			if( uiPrev != ANCHOR_NODE_INDEX ) (*m_pVec)[uiPrev].LIST_SetNext( uiNext );
			kEnd.LIST_SetDeleted(true);
			m_pVec->FreeIfDeleted(uiEnd);

			kLD.uiSize--;

			uiEnd = uiPrev;
		}
	};

	FStaticVector<M, J> m_aLD;	//Data for each element of the list
	T_ALLOCATOR m_DefaultVec;
	T_ALLOCATOR* m_pVec;					//The currently used allocator
};

#endif
