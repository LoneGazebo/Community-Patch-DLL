//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FFireTypes.h
//
//  AUTHOR:  David McKibbin  --  9/17/2002
//
//  PURPOSE: FireEngine data types and macros
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef		FFIRETYPES_H
#define		FFIRETYPES_H
#pragma		once

typedef unsigned char		byte;
typedef unsigned short		word;
typedef unsigned int		uint;
typedef unsigned long		dword;
#if		defined(_PS3)
// In SDK 2.0, qword is defined as vector unsigned char
//typedef uint64_t			qword;
typedef int64_t				__int64;
#elif	defined(_NDS)
typedef u64					qword;
typedef s64					__int64;
#else	//	_PS3,_NDS
typedef unsigned __int64	qword;
#endif	//	_PS3,_NDS
typedef wchar_t         	wchar;

#ifdef		_PS3
// Add Common Windows Types
typedef char CHAR;
typedef wchar_t WCHAR;

typedef unsigned char UCHAR;

typedef short SHORT;
typedef long  LONG;

typedef unsigned short USHORT;
typedef unsigned long  ULONG;

typedef unsigned short u_short;
typedef unsigned long  u_long;

typedef byte  BYTE;
typedef word  WORD;
typedef uint  UINT;
typedef dword DWORD;

typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef ULONG_PTR SIZE_T;

typedef int8_t    INT8,   *PINT8;
typedef int16_t   INT16,  *PINT16;
typedef int32_t   INT32,  *PINT32;
typedef int64_t   INT64,  *PINT64;
typedef uint8_t   UINT8,  *PUINT8;
typedef uint16_t  UINT16, *PUINT16;
typedef uint32_t  UINT32, *PUINT32;
typedef uint64_t  UINT64, *PUINT64;

typedef int BOOL;

#ifndef		FALSE
#define		FALSE	0
#endif	//	FALSE

#ifndef		TRUE
#define		TRUE	1
#endif	//	TRUE

typedef struct _POINT
{
    long x;
    long y;
}   POINT, *PPOINT, *LPPOINT;

typedef struct tagRECT
{
    long left;
    long top;
    long right;
    long bottom;
}   RECT, *PRECT, *LPRECT;

//-- Windows Message Passing Types ------------------------

// Dummy Type
typedef int HWND;

// Messaging Types
typedef UINT * WPARAM;
typedef LONG * LPARAM;
typedef LONG * LRESULT;

//-- Windows Message Passing Types ------------------------
#endif	//	_PS3

#ifdef		_NDS
typedef char CHAR;
typedef wchar_t WCHAR;

typedef unsigned char UCHAR;

typedef short SHORT;
typedef long  LONG;

typedef unsigned short USHORT;
typedef unsigned long  ULONG;

typedef unsigned short u_short;
typedef unsigned long  u_long;

typedef byte  BYTE;
typedef word  WORD;
typedef uint  UINT;
typedef dword DWORD;

typedef s8    INT8,   *PINT8;
typedef s16   INT16,  *PINT16;
typedef s32   INT32,  *PINT32;
typedef s64   INT64,  *PINT64;
typedef u8   UINT8,  *PUINT8;
typedef u16  UINT16, *PUINT16;
typedef u32  UINT32, *PUINT32;
typedef u64  UINT64, *PUINT64;

typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef struct _POINT
{
    long x;
    long y;
}   POINT, *PPOINT, *LPPOINT;

typedef struct tagRECT
{
    long left;
    long top;
    long right;
    long bottom;
}   RECT, *PRECT, *LPRECT;

// TODOPS3 - This could cause endian problems.
#define LOWORD(l) ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l) ((WORD)((DWORD)(l) >> 16))

//-- Windows Message Passing Types ------------------------

	// Dummy Type
	typedef int HWND;

	// Messaging Types
	typedef UINT * WPARAM;
	typedef LONG * LPARAM;
	typedef LONG * LRESULT;

//-- Windows Message Passing Types ------------------------

#endif	//	_NDS

typedef UINT64	uint64;
typedef UINT32	uint32;
typedef UINT16	uint16;
typedef UINT8	uint8;
typedef INT32	int32;
typedef INT64	int64;
typedef INT16	int16;
typedef INT8	int8;
 

#ifdef		_PS3
typedef pthread_t	threadid_t;
#else	//	_PS3
typedef DWORD		threadid_t;
#endif	//	_PS3

typedef unsigned short port_t;

#ifdef		_PS3
typedef unsigned int socklen_t;
#else	//	_PS3
typedef int socklen_t;
#endif	//	_PS3

// These classes allow an STL object to compare object values instead of
// comparing the value of the objects' pointers.
#include <functional>
template<class _Ty>
struct ptr_less : std::binary_function<_Ty, _Ty, bool>
{
	bool operator()(const _Ty& _X, const _Ty& _Y) const
	{
		return (*_X < *_Y);
	}
};

//---------------------------------------------------------------------------------------
//
//	To enable coloring of the above keywords in MSVC:
//
//		In the same folder as "MsDev.exe", create a text file 
//		("UserType.dat") with the above keywords one per line.
//
//	"C:\Program Files\Microsoft Visual Studio .NET\Common7\IDE\UserType.dat"
//
//	To customize the coloring of user defined keywords:
//
//		Tools / Options / Environment / Fonts and Colors / Display items: "User Keywords"
//
//	For help:
//
//		"Defining Keywords in Visual C++"
//
//---------------------------------------------------------------------------------------
// Or just use CodeWright where it's built into the IDE >:P
//---------------------------------------------------------------------------------------

//
//	NANs:  Quiet or Signaling NaNs can be either sign.
//
#define  QNAN	0x7FC00000	// mantissa can't be all zero
#define  SNAN	0x7F800001


#define  BIT(n)		(1<<(n))

#define  BIT0		BIT( 0)
#define  BIT1		BIT( 1)
#define  BIT2		BIT( 2)
#define  BIT3		BIT( 3)
#define  BIT4		BIT( 4)
#define  BIT5		BIT( 5)
#define  BIT6		BIT( 6)
#define  BIT7		BIT( 7)
#define  BIT8		BIT( 8)
#define  BIT9		BIT( 9)
#define  BIT10		BIT(10)
#define  BIT11		BIT(11)
#define  BIT12		BIT(12)
#define  BIT13		BIT(13)
#define  BIT14		BIT(14)
#define  BIT15		BIT(15)
#define  BIT16		BIT(16)
#define  BIT17		BIT(17)
#define  BIT18		BIT(18)
#define  BIT19		BIT(19)
#define  BIT20		BIT(20)
#define  BIT21		BIT(21)
#define  BIT22		BIT(22)
#define  BIT23		BIT(23)
#define  BIT24		BIT(24)
#define  BIT25		BIT(25)
#define  BIT26		BIT(26)
#define  BIT27		BIT(27)
#define  BIT28		BIT(28)
#define  BIT29		BIT(29)
#define  BIT30		BIT(30)
#define  BIT31		BIT(31)


#define MAX_CHAR                            (0x7f)
#define MIN_CHAR                            (0x80)
#define MAX_SHORT                           (0x7fff)
#define MIN_SHORT                           (0x8000)
#define MAX_INT                             (0x7fffffff)
#define MIN_INT                             (0x80000000)
#define MAX_UNSIGNED_CHAR                   (0xff)
#define MIN_UNSIGNED_CHAR                   (0x00)
#define MAX_UNSIGNED_SHORT                  (0xffff)
#define MIN_UNSIGNED_SHORT                  (0x0000)
#define MAX_UNSIGNED_INT                    (0xffffffff)
#define MIN_UNSIGNED_INT                    (0x00000000)

#ifndef FPAD_N // Pads the size so that its a multiple of N.
#define FPAD_N(size, N) (((size) + ((N)-1)) & (~((N)-1)))
#endif

#ifndef FPAD_8 // Pads the size so that its a multiple of 8.
#define FPAD_8(size) FPAD_N(size, 8)
#endif

#ifndef FPAD_16 // Pads the size so that its a multiple of 16.
#define FPAD_16(size) FPAD_N(size, 16)
#endif

//----------------------------------------- MACROS -----------------------------------------------

template<class T> __forceinline void SAFE_DELETE( T *& pkInstance )
{
	delete pkInstance;
	pkInstance = NULL;
};

template<class T> __forceinline void SAFE_DELETE_ARRAY( T *& pkInstanceArray )
{
	delete[] pkInstanceArray;
	pkInstanceArray = NULL;
};

template<class T> __forceinline void SAFE_RELEASE( T *& pkRefObject )
{
	if (pkRefObject) pkRefObject->Release();
	pkRefObject = NULL;
};

#define SAFE_DELETE_INTERNAL(p)			{ if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY_INTERNAL(p)	{ if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE_INTERNAL(p)		{ if(p) { (p)->Release(); (p)=NULL; } }

#define SAFE_STRCPY(dest,src,len)	{ strncpy(dest,src,len); dest[len-1]=NULL; }

// Use a template function to call free and set the pointer to NULL.
template<typename T> __forceinline void SAFE_FREE( T ** pT )
{
	if ( *pT != NULL )
	{
		free( *pT );
		*pT = NULL;
	}
}

// Template functors and function for deleting all elements in a container
struct FSafeDelete
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p );
	}
};

struct FSafeDeleteFirst
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p.first );
	}
};

struct FSafeDeleteSecond
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p.second );
	}
};

struct FSafeDeletePair
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p.first );
		SAFE_DELETE( p.second );
	}
};

template < typename Container, typename Functor > __forceinline
void SAFE_DELETE_ALL( Container c, Functor f )
{
	std::for_each( c.begin(), c.end(), f );
}
template < typename Container > __forceinline
void SAFE_DELETE_ALL( Container c )
{
	SAFE_DELETE_ALL( c, FSafeDelete() );
}

// Size constants
const uint KILOBYTE	= 1024;
const uint MEGABYTE	= KILOBYTE * 1024;
const uint GIGABYTE	= MEGABYTE * 1024;
#define		CONVERT_BT_TO_KB(bt)	((bt+KILOBYTE-1)/KILOBYTE)
#define		CONVERT_BT_TO_MB(bt)	((bt+MEGABYTE-1)/MEGABYTE)
#define		CONVERT_BT_TO_GB(bt)	((bt+GIGABYTE-1)/GIGABYTE)
#define		CONVERT_KB_TO_MB(kb)	((kb+KILOBYTE-1)/KILOBYTE)
#define		CONVERT_KB_TO_GB(kb)	((kb+MEGABYTE-1)/MEGABYTE)
#define		CONVERT_MB_TO_GB(mb)	((mb+MEGABYTE-1)/MEGABYTE)
#define		CONVERT_KB_TO_BT(kb)	(kb*KILOBYTE)
#define		CONVERT_MB_TO_BT(mb)	(mb*MEGABYTE)
#define		CONVERT_GB_TO_BT(gb)	(gb*GIGABYTE)
#define		CONVERT_MB_TO_KB(mb)	(mb*KILOBYTE)
#define		CONVERT_GB_TO_KB(gb)	(gb*MEGABYTE)
#define		CONVERT_GB_TO_MB(gb)	(gb*KILOBYTE)

//--
//  FOURCC builder - from <DirectX/AviRiff.h>
//
typedef dword FOURCC;  // <DirectX/DmDls.h> & <PlatformSDK/MMSystem.h>
#define		FCC(ch4)	((((DWORD)(ch4) & 0x000000FF) << 24) |  \
            	      	(((DWORD)(ch4) & 0x0000FF00) <<  8) |   \
		                (((DWORD)(ch4) & 0x00FF0000) >>  8) |   \
		                (((DWORD)(ch4) & 0xFF000000) >> 24))


#ifndef		WIN32
struct GUID
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	byte           Data4[ 8 ];

	GUID( void )
	{
		Data1 = 0;
		Data2 = 0;
		Data3 = 0;
		memset( Data4, 0, sizeof ( Data4 ) );
	}
};

inline bool operator==( const GUID & lhs, const GUID & rhs )
{
	if ( lhs.Data1 == rhs.Data1 &&
		 lhs.Data2 == rhs.Data2 &&
		 lhs.Data3 == rhs.Data3 &&
		 memcmp(lhs.Data4, rhs.Data4, 8) == 0
	   )
	{
		return true;
	}

	return false;
}
#endif	//	WIN32
typedef GUID GameDescription;

#endif	//	FFIRETYPES_H
