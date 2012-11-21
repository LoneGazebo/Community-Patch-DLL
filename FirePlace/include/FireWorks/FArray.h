//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FArray.h
//
//  AUTHOR:  Don Wuenschell, first among programmers
//
//  PURPOSE: Fixed-size and dynamically-sized arrays.
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FARRAY_H
#define		FARRAY_H
#pragma		once

#include "FSTLBlockAllocatorMT.h"

//===========================================================================

namespace Firaxis
{
	//////////////////////////////////////////////////////////////////////////
	// Basic dynamic array - must be sized specifically!
	//////////////////////////////////////////////////////////////////////////
	template < class T, unsigned int AllocPool, unsigned int nSubID > 
	struct DynamicArray
	{
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		DynamicArray() 
			: m_nElements(0), m_pData(NULL) 
		{};
		DynamicArray( unsigned int nElements ) 
			: m_nElements(0), m_pData(NULL)
		{
			Resize( nElements );
		};

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		~DynamicArray()
		{
			delete[] m_pData;
		};


		//////////////////////////////////////////////////////////////////////////
		//Resize the array, releasing all data which is currently stored
		//////////////////////////////////////////////////////////////////////////
		void Resize( unsigned int nElements )
		{
			delete[] m_pData;
			m_nElements = nElements;
			m_pData = FNEW( T[m_nElements], AllocPool, nSubID );
		};

		//////////////////////////////////////////////////////////////////////////
		// Operator overload for bounds checking when asserts are enabled
		//////////////////////////////////////////////////////////////////////////
		T& operator[]( unsigned int i )
		{
			FAssert( i < m_nElements );
			return m_pData[i];
		};

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		T* m_pData;
		unsigned int m_nElements;
	};

	//////////////////////////////////////////////////////////////////////////
	// Basic dynamic array - must be sized specifically!
	//////////////////////////////////////////////////////////////////////////
	template < class T, unsigned int AllocPool, unsigned int nSubID > 
	struct DynamicArray2D
	{
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		DynamicArray2D() 
			: m_nX(0), m_nY(0), m_pData(NULL) 
		{};
		DynamicArray2D( unsigned int nX, unsigned int nY ) 
			: m_nX(0), m_nY(0), m_pData(NULL) 
		{
			Resize( nX, nY );
		};

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		~DynamicArray2D()
		{
			Release();
		};


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		void Resize( unsigned int nX, unsigned int nY )
		{
			Release();
			m_nX = nX;
			m_nY = nY;
			m_pData = FNEW( T[nX * nY], AllocPool, nSubID );
		};
		void Release()
		{
			delete[] m_pData; m_pData = NULL;
			m_nX = m_nY = 0;
		};

		//////////////////////////////////////////////////////////////////////////
		// Functor-style fast access
		//////////////////////////////////////////////////////////////////////////
		T& operator()( unsigned int nX, unsigned int nY )
		{
			FAssert( nX < m_nX );
			FAssert( nY < m_nY );
			return m_pData[ nX * m_nX + nY ];
		};


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		T* m_pData;
		unsigned int m_nX, m_nY;
	};

	//////////////////////////////////////////////////////////////////////////
    // Static (fixed-size) array
	//////////////////////////////////////////////////////////////////////////
    template < class T, uint ARRAY_SIZE >
    class Array
    {
		typedef Array< T, ARRAY_SIZE > THIS_TYPE;
    public:
	    // Operators:
	    //----------
	    const T& operator []( uint i ) const
	    {
#		ifdef _MSC_VER
#			pragma warning ( push )
#			pragma warning ( disable : 6385 ) //  Invalid data: accessing 'm_akItems' the readable size is X but X+N bytes might be read
			// sad day. 
#		endif//_MSC_VER
		    assert( i < ARRAY_SIZE );
		    return ( m_akItems[ i ] );
#		ifdef _MSC_VER
#		pragma warning ( pop )
#		endif//_MSC_VER
	    }
	    T& operator []( uint i )
	    {
#		ifdef _MSC_VER
#			pragma warning ( push )
#			pragma warning ( disable : 6385 ) //  Invalid data: accessing 'm_akItems' the readable size is X but X+N bytes might be read
			// sad day. 
#		endif//_MSC_VER
		    assert( i < ARRAY_SIZE );
		    return ( m_akItems[ i ] );
#		ifdef _MSC_VER
#		pragma warning ( pop )
#		endif//_MSC_VER
		}

		bool operator == ( const THIS_TYPE& kRHS ) const
		{
			for( unsigned int i = 0; i < ARRAY_SIZE; ++i )
			{
				if( !(m_akItems[i] == kRHS.m_akItems[i] ) )
				{
					return false;
				}
			}
			return true;
		}



	    const Array< T, ARRAY_SIZE >& operator =( const Array< T, ARRAY_SIZE >& akItems )
	    {
		    for ( uint i = 0; i < ARRAY_SIZE; i++ )
		    {
			    m_akItems[ i ] = akItems.m_akItems[ i ];
		    }

		    return ( *this );
	    }

		unsigned int size( void ) const
		{
			return ARRAY_SIZE;
		}


		unsigned int SizeInBytes() const
		{
			return size() * sizeof(T);
		}
		void* Bytes()
		{
			return (void*)&m_akItems[0];
		}
		const void* Bytes() const
		{
			return (void*)&m_akItems[0];
		}

    private:

	    // Members:
		T m_akItems[ ARRAY_SIZE ];	// The array of items
    };

	//////////////////////////////////////////////////////////////////////////
	// Basic Static 2D array
	//////////////////////////////////////////////////////////////////////////
	template < class T, unsigned int X, unsigned int Y> 
	struct Array2D
	{
		Array2D() : m_X(X), m_Y(Y) {};
		T m_Data[X][Y];
		unsigned int m_X,m_Y;
	};
};


//===========================================================================

// Dynamic Array that extends std::vector
template < class T, eMPoolType pool_type >
class FArray: public std::vector< T, FSTL_Tagged_Allocator<T, 16, pool_type> >
{
public:
	// Constructors:
	//-------------
	FArray( uint uiMaxSize = 0, uint uiGrowBy = 0 );

	// Methods:
	//--------
	uint GetGrowBy( void ) const;
	void SetGrowBy( uint uiGrowBy );

	bool AddExclusive( const T& kElement );
	uint Add( const T& kElement );
	void SetAt( uint uiIndex, const T& kElement );

	T Remove( uint uiIndex );
	T RemoveEnd( void );
	
	uint Find( const T& kElement, uint uiStart = 0 ) const;
		
	template< class Predicate >
	void Sort( Predicate pred );		

private:
	// Members:
	//--------
	uint m_uiGrowBy;	// Number of slots to grow array when full
};

//---------------------------------------------------------------------------
template < class T, eMPoolType pool_type >
inline FArray< T, pool_type >::FArray( uint uiMaxSize, uint uiGrowBy )
{
	FArray<T, pool_type>::reserve( uiMaxSize );
	SetGrowBy( uiGrowBy );
}

//---------------------------------------------------------------------------
// Set/Get number of slots to grow array when full

template < class T, eMPoolType pool_type >
inline uint FArray< T, pool_type >::GetGrowBy( void ) const
{
	return ( m_uiGrowBy );
}

template < class T, eMPoolType pool_type >
inline void FArray< T, pool_type >::SetGrowBy( uint uiGrowBy )
{
	m_uiGrowBy = uiGrowBy;
}

//---------------------------------------------------------------------------

// Add to the array, growing if necessary
template < class T, eMPoolType pool_type >
inline uint FArray< T, pool_type >::Add( const T& kElement )
{
	if ( GetGrowBy() && ( FArray<T, pool_type>::capacity() == FArray<T,pool_type>::size() ) )
	{
		FArray<T, pool_type>::reserve( FArray<T, pool_type>::size() + GetGrowBy() );
	}
	FArray<T, pool_type>::push_back( kElement );

	return ( static_cast< int >( FArray<T, pool_type>::size() - 1 ) );
}

// Add element if it isn't already in the array -- returns true if the element was added
template < class T, eMPoolType pool_type >
inline bool FArray< T, pool_type >::AddExclusive( const T& kElement )
{
	if ( Find( kElement ) != ( uint )-1 )
	{
		// nothing was added
		return ( false );
	}
	else
	{
		// add the element
		Add( kElement );
		return ( true );
	}
}

// Assign the element at the specified index
template < class T, eMPoolType pool_type >
inline void FArray< T, pool_type >::SetAt( uint uiIndex, const T& kElement )
{
	assert( uiIndex < (FArray<T, pool_type >::size()) );
	FArray<T,pool_type>::at( uiIndex ) = kElement;
}

//---------------------------------------------------------------------------
// Remove the element at the specified position

template < class T, eMPoolType pool_type >
inline T FArray< T, pool_type >::Remove( uint uiIndex )
{
	T kElement;	// Element to return

	assert( uiIndex < (FArray<T,pool_type>::size()) );

	kElement = FArray<T,pool_type>::at( uiIndex );
	FArray<T, pool_type>::erase( FArray<T,pool_type>::begin() + uiIndex );

	return ( kElement );
}

template < class T, eMPoolType pool_type >
inline T FArray< T,pool_type >::RemoveEnd( void )
{
	T kElement;	// Element to return

	assert( (FArray<T, pool_type>::size()) > 0 );

	kElement = FArray<T, pool_type>::back();
	FArray<T,pool_type>::pop_back();

	return ( kElement );
}

//---------------------------------------------------------------------------
// Find this element in the array
// If element is not found, will return -1
template < class T, eMPoolType pool_type >
inline uint FArray< T,pool_type >::Find( const T& kElement, uint uiStart ) const
{
	if ( uiStart >= FArray<T, pool_type>::size() )
	{
		return ( -1 );
	}

	typename FArray<T,pool_type>::const_iterator it = std::find( FArray<T, pool_type>::begin() + uiStart, FArray<T,pool_type>::end(), kElement );
	if ( it != FArray<T,pool_type>::end() )
	{
		return ( static_cast< uint >( it - FArray<T, pool_type>::begin() ) );
	}
	else
	{
		return ( -1 );
	}
}

//---------------------------------------------------------------------------
// Sort the array
template< class T, eMPoolType pool_type>
template< class Predicate >
inline void FArray<T,pool_type>::Sort( Predicate pred )
{
	std::sort( FArray<T,pool_type>::begin(), FArray<T, pool_type>::end(), pred );
}

#endif	//FARRAY_H
