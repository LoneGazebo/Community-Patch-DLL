//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FFileDefines.h
//
//  AUTHOR:  Nathan Mefford
//
//  PURPOSE: Common #defines for file system.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef		FFILEDEFINES_H
#define		FFILEDEFINES_H
#pragma		once

// Platform-specific support
#undef		SUPPORT_ASYNC_IO			// Define to support asynchronous I/O

#if			defined(_WINPC)
	#define	SUPPORT_ASYNC_IO
#elif		defined(_XENON)
	#define	ENABLE_PAKFILE_BUFFERING
	#define	SUPPORT_ASYNC_IO
#elif		defined(_XBOX)
	#define	ENABLE_PAKFILE_BUFFERING
	#define	SUPPORT_ASYNC_IO
#elif		defined(_PS3)
	#define	ENABLE_PAKFILE_BUFFERING
	#define	SUPPORT_ASYNC_IO
#endif	//	UNDEFINED_PLATFORM

//---------------------------------------------------------------------------------------
// Flags for opening files
//---------------------------------------------------------------------------------------
#ifdef		SUPPORT_ASYNC_IO
#define		FFILE_FLAG_ASYNCHRONOUS		0x01	// Causes the file to be opened for asynchronous access
#else
#define		FFILE_FLAG_ASYNCHRONOUS		0		// for people who inisist on hard coding this
#endif	//	SUPPORT_ASYNC_IO
#ifdef		WIN32
#define 	FFILE_FLAG_RANDOM_ACCESS	0x02	// Causes the file to be opened optimized for random access (as opposed to sequential scan).
#endif	//	WIN32
#define		FFILE_FLAG_HIDE_ERRORS		0x04	// Causes the file system to not display error messages in the debugger if the file is not found.
#define		FFILE_FLAG_BIG_ENDIAN		0x10	// Causes the file to read as if the input was big endian
#define		FFILE_FLAG_UNICODE			0x20	// Used in FTextFile -- encoding is Unicode
#ifdef		_XENON
#define		FFILE_FLAG_NO_CACHE			0x40	// Causes the file to be opened without caching
#endif	//	_XENON

// Default flags by OS:
#ifdef		_XBOX
#define		FFILE_FLAG_DEFAULT			FFILE_FLAG_ASYNCHRONOUS
#endif	//	_XBOX
#ifndef		FFILE_FLAG_DEFAULT
#define		FFILE_FLAG_DEFAULT			0
#endif	//	FFILE_FLAG_DEFAULT

//---------------------------------------------------------------------------------------
//	Static constants
//---------------------------------------------------------------------------------------
// Default size of file buffers for buffered I/O
#ifdef		_WINPC
const uint FFILE_DEFAULT_BUFFER_SIZE = 0; // KWG: Let Windows do its own buffering, ours is somewhat broken at the moment. 64 * KILOBYTE;
#elif		defined(_XBOX)
// note to anyone who might care:
// for the X360 it's actually using 32K from the _XBOX tag rather than
// the 128K that was intended below.  this is because the X360 also uses
// the _XBOX and it finds that one first.  
// By default, manually control the buffering -- buffer must be a multiple of the sector-size
const uint FFILE_DEFAULT_BUFFER_SIZE = 32 * KILOBYTE;
#elif		defined(_XENON)
// By default, manually control the buffering -- 128K is optimal for Xenon
const uint FFILE_DEFAULT_BUFFER_SIZE = 128 * KILOBYTE;
#elif		defined(_PS3)
// By default, manually control the buffering -- 64K is optimal for PS3
const uint FFILE_DEFAULT_BUFFER_SIZE = 64 * KILOBYTE;
#elif		defined(_NDS)
// Allow Nintendo DS to handle buffering by default
const uint FFILE_DEFAULT_BUFFER_SIZE = 0;
#else	//	UNDEFINED_PLATFORM
// Default to 32K buffers
const uint FFILE_DEFAULT_BUFFER_SIZE = 32 * KILOBYTE;
#endif	//	_WINPC


//---------------------------------------------------------------------------------------
// File system enums
//---------------------------------------------------------------------------------------
enum FileErr
{
	FILE_OK,
	FILE_NOTFOUND,
	FILE_ALREADYEXISTS,
	FILE_BADMODE,
	FILE_BADSEEK,
	FILE_MAPERROR,
	FILE_VIEWERROR,
	FILE_BADNAME,
	FILE_BADSIG,
	FILE_DISKFULL,
	FILE_CORRUPT,
};

#endif	//	FFILEDEFINES_H

