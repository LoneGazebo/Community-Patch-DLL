//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FFileStream.h
//
//  AUTHOR:		Mustafa Thamer	--  11/25/2003
//
//  PURPOSE:	Stream class for file streams.
//				Easily read and write data to those kinds of streams using the same baseclass interface.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef		FFILESTREAM_H
#define		FFILESTREAM_H
#pragma		once

#include "FDataStream.h"
#include "FIFile.h"

//
// File stream
//
class FFileStream: public FDataStream
{	
public:
	// Constructors:
	//-------------
	FFileStream();

	// Destructors:
	//------------
	virtual ~FFileStream();

	// Virtual:
	//--------
	virtual bool Open(const char* szName, FIFile::OpenMode eMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE);
	virtual bool Open(const wchar_t* wszName, FIFile::OpenMode eMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE);

	virtual bool Close();
	virtual unsigned int ReadIt(unsigned int byteCount, void* buffer);
	virtual unsigned int WriteIt(unsigned int byteCount, const void* buffer);
	virtual void Skip(unsigned int deltaByteCount);
	virtual bool AtEnd() const;
	virtual void Rewind();
	virtual void FastFwd();
	virtual void Truncate();
	virtual void Flush();
	virtual unsigned int GetEOF() const;
	virtual void RaiseException( std::exception& kException ) const;
		
	// Methods:
	//--------
	FIFile* GetFile();
	void SetFile(FIFile* file);
		

private:
	// Members:
	//--------
	FIFile* m_pRef;
};

#endif	//	FFILESTREAM_H

