//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FMemoryStream.h
//
//  AUTHOR:		Mustafa Thamer	--  11/25/2003
//
//  PURPOSE:	Stream class for memory streams.
//				Easily read and write data to those kinds of streams using the same baseclass interface.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2006 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef		FMEMORYSTREAM_H
#define		FMEMORYSTREAM_H
#pragma		once

#include "FDataStream.h"

//
// Memory stream
//
class FMemoryStream : public FDataStream 
{
public:
	// Constructors:
	//-------------
	// JAR : todo  - refactor these constructors. FMemoryStream myBroken(char *, int) will silently compile as it converts
	// the char * and int to the 2-bool implementation. E.g. FMemoryStream myCopy(other.GetBuf(), other.GetEOF()), followed
	// by Bad Things.
	/*
	FMemoryStream(int size, const void* data, uint	uMemoryPool, uint uMemoryTag, bool bRealloc=false, bool bReallocMin=false);
	FMemoryStream(uint	uMemoryPool, uint uMemoryTag, bool bRealloc=true, bool bReallocMin=false);
	FMemoryStream(int size, const void* data, bool bRealloc=false, bool bReallocMin=false);
	FMemoryStream(bool bRealloc=true, bool bReallocMin=false);
	*/
	FMemoryStream(int size, const void* data, uint	uMemoryPool, uint uMemoryTag, bool bRealloc, bool bReallocMin);
	FMemoryStream(uint	uMemoryPool, uint uMemoryTag, bool bRealloc, bool bReallocMin);
	FMemoryStream(int size, const void* data, bool bRealloc, bool bReallocMin);
	FMemoryStream(int size, const void* data, bool bRealloc);
	FMemoryStream(int size, const void* data);
	FMemoryStream(bool bRealloc, bool bReallocMin);
	FMemoryStream(bool bRealloc);
	FMemoryStream();
	FMemoryStream(const FMemoryStream &);

	// Destructors:
	//------------
	virtual ~FMemoryStream( void );

	// Abstract:
	//---------
	virtual unsigned int ReadIt(unsigned int byteCount, void * buffer);
	virtual unsigned int WriteIt(unsigned int byteCount, const void* buffer);
	virtual void Skip(unsigned int deltaByteCount);
	virtual void Rewind();

	// Virtual:
	//--------
	virtual void Init(int size, const void* data, bool bRealloc=false);
	virtual void Uninit();
	virtual bool AtEnd() const;
	virtual void FastFwd();
	virtual void Truncate();
	virtual unsigned int GetEOF() const;	// returns size
	virtual void CopyToMem(void* mem);

	// Methods:
	//--------
	void  Grow( uint uiMinAdditionalSizeRequired = 0 );
	char* GetBuf();
	const char* GetBuf() const;
	void Init(int size, const void* data, bool bRealloc, bool bReallocMin);

	FMemoryStream & operator=(const FMemoryStream &);

protected:
	// Members:
	//--------
	char*	m_pStart;
	char*	m_pData;		//!< Current read/write position
	char*	m_pStop;		//!< End of the file (used contents of the memory buffer)
	char*	m_pEndOfBuffer;	//!< True end of the memory buffer
	bool    m_bRealloc;		//!< Will the memory stream grow if you write too much?
	bool	m_bReallocMin;	//!< Will the memory stream only allocate the minimum extra requested space (or double)?

	uint	m_uMemoryPool, m_uMemoryTag;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Memory Stream RLE  ==  FMemoryStream w/RLE compression
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FMemoryStreamRLE : public FMemoryStream 
{
public:
	// Constructors:
	//-------------
	FMemoryStreamRLE( int size, const void* data, bool bRealloc=false ) : FMemoryStream( size, data, bRealloc, false ) { Reset(); }
	FMemoryStreamRLE( bool bRealloc=true ) : FMemoryStream( bRealloc, false ) { Reset(); }

	// Abstract:
	//---------
	virtual uint ReadIt ( uint byteCount,       void* buffer );
	virtual uint WriteIt( uint byteCount, const void* buffer );

	virtual void Skip( uint deltaByteCount ) { char ch; while ((int)deltaByteCount-- > 0) ReadIt( 1, &ch ); }
	virtual void Rewind() { Reset(); FMemoryStream::Rewind(); }

protected:
	void Reset() { Run = 0; Last = EOF; }

	// Members:
	//--------
	uint  Last;		// the last char in the i/o stream
	uint  Run;		// and its current run length
};

#endif	//FMEMORYSTREAM_H
