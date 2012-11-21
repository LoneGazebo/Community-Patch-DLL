//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//! \file		FEndian.h
//! \author		Bart Muzzin -- 2-7-2006
//! \brief		Functions related to endianness of the target machine
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#pragma once
#ifndef FENDIAN_H
#define FENDIAN_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      FEndian
//!  \brief		Class containing (static) functions and defines for endianness
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FEndian
{
	public:

		enum EEndianness
		{
			ENDIAN_UNKNOWN,
			ENDIAN_LITTLE,
			ENDIAN_BIG
		};

		static EEndianness GetEndianness();
		template<class T> static void SwapEndian( T * kData, EEndianness kInputEndian );
		static void SwapEndianStruct( void * kData, const char * szFormat, EEndianness kInputEndian );

	private:

		static void* EndianSwitchWorker( void* pData, const char* format,
			int blockRepeatCount = 1, const char** updatedFormat = NULL );

		static EEndianness	ms_eTargetEndianness;	// The endianness of the currently executing machine
};

//------------------------------------------------------------------------------------------------
// FUNCTION:    FEndian::SwapEndian
//! \brief      Converts the endianness of a variable, depending on the input endianness
//! \param		kData The data to convert endianness
//! \param		kInputEndian The endianness of the incoming data
//! \retval     See FEndian::EEndianness
//------------------------------------------------------------------------------------------------

// Specific instantiations for sizes 2, 4 and 8, must exist, this handles any unexpected sizes
template<class T> inline void FEndian::SwapEndian( T * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	FAssertMsg1( 0, "Unexpected data type encountered when swapping endianness (size=%d, expecting 2, 4 or 8", sizeof(T));
}

template<> inline void FEndian::SwapEndian( short * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[1]);
}

template<> inline void FEndian::SwapEndian( unsigned short * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[1]);
}

template<> inline void FEndian::SwapEndian( wchar_t * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[1]);
}

template<> inline void FEndian::SwapEndian( int * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[3]);
	std::swap( szBuffer[1], szBuffer[2]);
}

template<> inline void FEndian::SwapEndian( unsigned int * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[3]);
	std::swap( szBuffer[1], szBuffer[2]);
}

template<> inline void FEndian::SwapEndian( unsigned long * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[3]);
	std::swap( szBuffer[1], szBuffer[2]);
}

template<> inline void FEndian::SwapEndian( float * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[3] );
	std::swap( szBuffer[1], szBuffer[2] );
}

template<> inline void FEndian::SwapEndian( __int64 * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[7] );
	std::swap( szBuffer[1], szBuffer[6] );
	std::swap( szBuffer[2], szBuffer[5] );
	std::swap( szBuffer[3], szBuffer[4] );
}

template<> inline void FEndian::SwapEndian( double * kData, EEndianness kInputEndian )
{
	if ( GetEndianness() == kInputEndian ) return;
	char * szBuffer = (char*)kData;
	std::swap( szBuffer[0], szBuffer[7] );
	std::swap( szBuffer[1], szBuffer[6] );
	std::swap( szBuffer[2], szBuffer[5] );
	std::swap( szBuffer[3], szBuffer[4] );
}

#endif // FENDIAN_H
