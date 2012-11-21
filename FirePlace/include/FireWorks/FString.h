//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FString.h
//
//  AUTHOR:		Mike Breitkreutz	--  08/26/2003
//
//  PURPOSE:	Implements a string class with support for both unicode and ascii.
// 
//				FStringA represents ANSI strings.
//				FStringW represents UNICODE strings.
//				FString is defined as an FStringW if _UNICODE is defined, FStringA otherwise.
// 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				IMPORTANT RULES:
//				The m_pszString MUST be the ONLY member of the class.
//				Add additional data members to the FStringData class.
//				FString cannot contain ANY virtual functions.
// 
//				If these rules are not followed, the Format() functions will crash.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//		Nintendo DS Only:
//		*** Header implementation notes are at the bottom of <FStringA.inl> ***
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef		FSTRING_H
#define		FSTRING_H
#pragma		once

#include	"FArray.h"
#include	"FCrc32.h"

#ifdef		_DEBUG
#define		ENABLE_FSTRING_STATS
#endif	//	_DEBUG

//---------------------------------------------------------------------------------------
// Defines
//
#ifdef		_PS3
#define		DEFAULT_PATH_DELIMITER_A	'/'
#define		DEFAULT_PATH_DELIMITER_W	L'/'
#else	//	_PS3
#define		DEFAULT_PATH_DELIMITER_A	'\\'
#define		DEFAULT_PATH_DELIMITER_W	L'\\'
#endif	//	_PS3
#define		DEFAULT_PATH_DELIMITER		_T( DEFAULT_PATH_DELIMITER_A )	// Character used to delimit paths

//---------------------------------------------------------------------------------------
// Macros
//
#define	 FSTRING_CHECK_BUFFER	assert( !IsLocked() && "Don't forget to call ReleaseBuffer" )	// Warning for using GetBuffer/ReleaseBuffer safely

//---------------------------------------------------------------------------------------
// Typedefs
//
#ifndef		LPSTR
typedef char* LPSTR;
#endif	//	LPSTR
#ifndef		LPCSTR
typedef const char* LPCSTR;
#endif	//	LPCSTR
#ifndef		LPWSTR
typedef wchar* LPWSTR;
#endif	//	LPWSTR
#ifndef		LPCWSTR
typedef const wchar* LPCWSTR;
#endif	//	LPCWSTR

typedef uint FStringHashType;
//---------------------------------------------------------------------------------------
// Constants
//
const size_t nFSTRING_HASH_TRAITS_BUCKET_SIZE	= 3;	// Mean number of elements per bucket -- must be > 0
const size_t nFSTRING_HASH_TRAITS_MIN_BUCKETS	= 8;	// Minimum number of buckets to maintain -- must be > 0 and a power of 2

//---------------------------------------------------------------------------------------
// FStringA
//
//	Represents an ANSI string.
//
class FStringA
{
#ifndef		_NDS
public:
	// Structures:
	//-----------
	struct FStringAData	// Data for ANSI strings.
	{
		// Using all ints for conformity
		int m_iAllocLength;	// Number of chars allocated for the string but not including null terminator
		int m_iDataLength;	// Length of the string not including null terminator
		USHORT m_bLocked;	// If true, the buffer is locked; meaning GetBuffer() was called to get write access to the StringData
		USHORT m_bFixed;	// If true, the buffer is a fixed length and it cannot be re-allocated or released
		//char szData[ m_iAllocLength ];

		// Access to actual string/character data
		LPSTR StringData( void );
	};
#endif	//	!_NDS

public:
	// Enums:
	//------
	enum CtorType
	{
		FORMAT = 1,	// pass this in to Format constructor
	};

#ifdef		_NDS
	enum FlagType
	{
		LOCKED = 1,	// active GetBuffer(), before ReleaseBuffer()
		STATIC = 2,	// static string alloc'd on Frame Heap
		FIXED  = 4,	// fixed string: no buffer ops
		LARGE  = 8,	// larger than 65499 characters
		UNUSED = 16
	};
#endif	//	_NDS

	// Classes:
	//--------
	class FStringAHashTraits	// Traits for hash_map
	{
	public:
		// Enums:
		//------
		enum
		{
			bucket_size = nFSTRING_HASH_TRAITS_BUCKET_SIZE,	// Mean number of elements per bucket
			min_buckets	= nFSTRING_HASH_TRAITS_MIN_BUCKETS,	// Minimum number of buckets to maintain
		};

		// Constructors:
		//-------------
		FStringAHashTraits( void );

		// Operators:
		//----------
		size_t operator()( const FStringA& str ) const;
		bool operator()( const FStringA& str1, const FStringA& str2 ) const;
	};

	class FStringAHashTraitsNoCase	// Traits for hash_map with case-insensitve compare
	{
	public:
		// Enums:
		//------
		enum
		{
			bucket_size = nFSTRING_HASH_TRAITS_BUCKET_SIZE,	// Mean number of elements per bucket
			min_buckets	= nFSTRING_HASH_TRAITS_MIN_BUCKETS,	// Minimum number of buckets to maintain
		};

		// Constructors:
		//-------------
		FStringAHashTraitsNoCase( void );

		// Operators:
		//----------
		size_t operator()( const FStringA& str ) const;
		bool operator()( const FStringA& str1, const FStringA& str2 ) const;
	};

	class FBuffer	// Convenient class for accessing the internal buffer
	{
	public:
		// Constructors:
		//-------------
		FBuffer( FStringA& str, int iMinBufLength = 0, bool bNullTerminated = true );

		// Destructors:
		//------------
		~FBuffer( void );

		// Operators:
		//----------
		operator LPSTR( void );


	private:
		// Members:
		//--------
		bool m_bNullTerminated;	// If true, the string is assumed to be null-terminated

		FStringA& m_strRef;	// The reference to the FString containing the buffer

		int m_iLength;	// The length of the string if it is not null-terminated

		LPSTR m_pszBuffer;	// The actual string buffer
	};

	#ifndef		_NDS
	template < size_t nLENGTH >
	class FStringAFixedBuffer	// Helper class used to create strings of a fixed-length
	{
	public:
		// Constructors:
		//-------------
		FStringAFixedBuffer( FStringA& str );
		FStringAFixedBuffer();
		
		FStringAData &GetStringData() { return m_kStringData; }

	private:
		// Members:
		//--------
		FStringAData m_kStringData;	// Data for the fixed-length string; must come first

		char m_szStringBuffer[ nLENGTH ];	// Fixed-length string buffer; must be second
	};
	#endif	//	_NDS

	// Constructors:
	//-------------
	         FStringA( void );
	         FStringA( const FStringA& strSource );
	         FStringA( char ch, int iRepeat );
	         FStringA( LPCSTR lpszSource );
	         FStringA( LPCSTR lpszSource, int iLength );
	explicit FStringA( int iLength );
			 #ifdef		_NDS
	explicit FStringA( int iLength, void* pBuffer );
			 #endif	//	_NDS
	         FStringA( const std::string& strSource );
	         FStringA( CtorType /* ignored, but needed to disambiguate vs other constructors */, LPCSTR lpszFormat, ... );
			 #ifdef		_NDS
			 FStringA( FlagType /* ignored */ );
			 #endif	//	_NDS

	// Destructors:
	//------------
	~FStringA( void );

	// Operators:
	//----------
	// Assignment
	const FStringA& operator=( const FStringA& strSource );
	const FStringA& operator=( char ch );
	const FStringA& operator=( LPCSTR lpszSource );
	const FStringA& operator=( const std::string& strSource );
	
	// Addition + assignment
	const FStringA& operator+=( const FStringA& strSource );
	const FStringA& operator+=( char ch );
	const FStringA& operator+=( LPCSTR lpszSource );
	const FStringA& operator+=( const std::string& strSource );
	
	// Addition
	       FStringA operator+( const FStringA& s2 );
	       FStringA operator+( const std::string& s2 );
	       FStringA operator+( char ch );
	friend FStringA operator+( char ch, const FStringA& s2 );
	       FStringA operator+( LPCSTR s2 );
	friend FStringA operator+( LPCSTR s1, const FStringA& s2 );
	
	// Subtraction + assignment
	const FStringA& operator-=( const FStringA& strSource );
	const FStringA& operator-=( const std::string& strSource );
	const FStringA& operator-=( char ch );
	const FStringA& operator-=( LPCSTR lpszSource );

	// Subtraction
	       FStringA operator-( const FStringA& s2 );
	       FStringA operator-( const std::string& s2 );
	       FStringA operator-( char ch );
	friend FStringA operator-( char ch, const FStringA& s2 );
	       FStringA operator-( LPCSTR s2 );
	friend FStringA operator-( LPCSTR s1, const FStringA& s2 );

	// Equality
	       bool operator==( const FStringA& s2 ) const;
	       bool operator==( const std::string& s2 ) const;
	       bool operator==( LPCSTR s2 ) const;
	friend bool operator==( LPCSTR s1, const FStringA& s2 );

	// Inequality
	       bool operator!=( const FStringA& s2 ) const;
	       bool operator!=( const std::string& s2 ) const;
	       bool operator!=( LPCSTR s2 ) const;
	friend bool operator!=( LPCSTR s1, const FStringA& s2 );

	// Less than
	       bool operator<( const FStringA& s2 ) const;
	       bool operator<( const std::string& s2 ) const;
	       bool operator<( LPCSTR s2 ) const;
	friend bool operator<( LPCSTR s1, const FStringA& s2 );

	// Less than or equal
	       bool operator<=( const FStringA& s2 ) const;
	       bool operator<=( const std::string& s2 ) const;
	       bool operator<=( LPCSTR s2 ) const;
	friend bool operator<=( LPCSTR s1, const FStringA& s2 );

	// Greater than
	       bool operator>( const FStringA& s2 ) const;
	       bool operator>( const std::string& s2 ) const;
	       bool operator>( LPCSTR s2 ) const;
	friend bool operator>( LPCSTR s1, const FStringA& s2 );

	// Greater than or equal
	       bool operator>=( const FStringA& s2 ) const;
	       bool operator>=( const std::string& s2 ) const;
	       bool operator>=( LPCSTR s2 ) const;
	friend bool operator>=( LPCSTR s1, const FStringA& s2 );

	// Shifting
	FStringA  operator<< ( int iCount ) const;
	FStringA& operator<<=( int iCount );
	FStringA  operator>> ( int iCount ) const;
	FStringA& operator>>=( int iCount );

	// Subscript
	char& operator[](  int  iIndex );
	char& operator[]( uint uiIndex );
	char  operator[](  int  iIndex ) const;
	char  operator[]( uint uiIndex ) const;

	// Casting
	operator LPCSTR( void ) const;

	// Methods:
	//--------
	// Attributes and operations
	void Empty( void );
	bool IsEmpty( void ) const;
	int  GetLength( void ) const;
	int  GetMaxLength( void ) const;

	// Access to string implementation buffer as "C" character array
	void Reserve( int iLength );
	void Release( void );
	void FreeExtra( void );
	LPCSTR c_str( void ) const;
	LPCSTR GetCString( void ) const;

	// Character operations
	char First( void ) const;
	char Last( void ) const;
	char GetAt( int iIndex ) const;
	void SetAt( int iIndex, char ch );
	void SetAt( int iIndex, int iLength, LPCSTR lpszSource, int iFirst = 0 );

	// String comparison
	bool MatchesPattern( LPCSTR lpszPattern, bool bCaseSensitive = false ) const;
	int Compare( LPCSTR lpsz ) const;
	int Compare( LPCSTR lpsz, int iLength ) const;
	int CompareNoCase( LPCSTR lpsz ) const;
	int CompareNoCase( LPCSTR lpsz, int iLength ) const;
	int Collate( LPCSTR lpsz ) const;
	int CollateNoCase( LPCSTR lpsz ) const;

	// Simple sub-string extraction
	FStringA Mid( int iFirst, int iCount ) const;
	FStringA Mid( int iFirst ) const;
	FStringA Left( int iCount ) const;
	FStringA Right( int iCount ) const;
	void Mid( FStringA & szDest, int iFirst, int iCount ) const;
	void Mid( FStringA & szDest, int iFirst ) const;
	static void Mid( FStringA & szDest, LPCSTR lpszSrc, int iFirst, int iCount );
	void Left( FStringA & szDest, int iCount ) const;
	void Right( FStringA & szDest, int iCount ) const;

	// Advanced sub-string extraction
	FStringA SpanIncluding( LPCSTR lpszCharSet, int iStart = 0 ) const;
	FStringA SpanExcluding( LPCSTR lpszCharSet, int iStart = 0 ) const;
	FStringA Extract( int& iStart, LPCSTR lpszCharSet ) const;
	FStringA Extract( int& iStart, char chEnd ) const;
	FStringA Extract( int& iStart, char chBegin, char chEnd ) const;
	void SpanIncluding( FStringA & szDest, LPCSTR lpszCharSet, int iStart = 0 ) const;
	void SpanExcluding( FStringA & szDest, LPCSTR lpszCharSet, int iStart = 0 ) const;
	void Extract( FStringA & szDest, int& iStart, LPCSTR lpszCharSet ) const;
	void Extract( FStringA & szDest, int& iStart, char chEnd ) const;
	void Extract( FStringA & szDest, int& iStart, char chBegin, char chEnd ) const;
	static void Extract( FStringA & szDest, LPCSTR szSrc, int& iStart, LPCSTR szDelimiters );

	// Path extraction
	FStringA ExtractDriveLetter( void ) const;
	FStringA ExtractFileDir( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	FStringA ExtractFileExtension( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	FStringA ExtractFileName( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	FStringA ExtractFilePath( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	FStringA ExtractFileTitle( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	void ExtractDriveLetter( FStringA & szDest ) const;
	void ExtractFileDir( FStringA & szDest, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	void ExtractFileExtension( FStringA & szDest, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	void ExtractFileName( FStringA & szDest, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	void ExtractFilePath( FStringA & szDest, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	void ExtractFileTitle( FStringA & szDest, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	void ReplaceDriveLetter( LPCSTR lpszNewDrive, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void ReplaceFileDir( LPCSTR lpszNewDir, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void ReplaceFileExtension( LPCSTR lpszNewExt, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void ReplaceFileName( LPCSTR lpszNewName, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void ReplaceFilePath( LPCSTR lpszNewPath, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void ReplaceFileTitle( LPCSTR lpszNewTitle, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void SplitPath( FStringA& strDriveLetter, FStringA& strFileDir, FStringA& strFileName, FStringA& strFileExt, char chPathDelimiter = DEFAULT_PATH_DELIMITER_A ) const;
	void StandardizePath( bool bChangeCase, bool bTerminate );
	void StripDriveLetter( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void StripFileDir( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void StripFileExtension( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void StripFileName( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void StripFilePath( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );
	void StripFileTitle( char chPathDelimiter = DEFAULT_PATH_DELIMITER_A );

	// Upper/Lower/Reverse conversion
	void MakeUpper( void );
	void MakeLower( void );
	void MakeReverse( void );

	// Trimming whitespace
	void TrimRight( void );
	void TrimRight( char chTarget );
	void TrimRight( LPCSTR lpszTargets );
	void TrimLeft( void );
	void TrimLeft( char chTarget );
	void TrimLeft( LPCSTR lpszTargets );
	void Trim( void );
	void Trim( char chTarget );
	void Trim( LPCSTR lpszTargets );
	void TrimWhitespace( void );
	void Truncate( int iNewLength );

	// Advanced manipulation
	int Delete( int iIndex, int iCount = 1 );
	int Delete( LPCSTR lpszSource );
	int Insert( int iIndex, char ch );
	int Insert( int iIndex, LPCSTR lpsz );
	int Remove( char chRemove );
	int Remove( LPCSTR lpszRemove );
	int RemoveRepeated( char chRemove );
	int Replace( char chOld, char chNew );
	int Replace( LPCSTR lpszOld, LPCSTR lpszNew );
	int Replace( char chOld, LPCSTR lpszNew );
	int Replace( LPCSTR lpszOld, char chNew );
	int Wrap( int iMaxLineLength );

	// Searching
	int Find( char ch, int iStart = 0 ) const;
	int Find( LPCSTR lpszSub, int iStart = 0 ) const;
	int FindOneOf( LPCSTR lpszCharSet, int iStart = 0 ) const;
	static int FindOneOf( LPCSTR lpszSrc, LPCSTR lpszCharSet, int iStart = 0);

	int ReverseFind( char ch, int iStart = -1 ) const;
	int FindCount( char ch, int iStart = 0 ) const;
	int FindToken( LPCSTR lpszToken, int iStart = 0 ) const;
	int FindNextWord( int iStart = 0 ) const;
	int FindPrevWord( int iStart = -1 ) const;

	// Simple formatting
	void __cdecl Format( LPCSTR lpszFormat, ... );
	void __cdecl Format( uint uiFormatID, ... );			// from custom (fxs) string table
#ifdef		_WINPC
	void __cdecl FormatResource( uint uiFormatID, ... );	// from microsoft string table in .rc file
#endif	//	_WINPC
	void FormatSize( uint uiSize );
	void FormatV( LPCSTR lpszFormat, va_list vaArgList );
	void TranslateEscapeCodes( void );

	// Load from string resources
	bool LoadString( uint uiID );								// from custom (fxs) string table
#ifdef		_WINPC
	bool LoadResource( uint uiID, HINSTANCE hInstance = NULL );	// from microsoft string table in .rc file
#endif	//	_WINPC

	// String copying
	void Copy( int iLength, LPCSTR lpszSource, int iFirst = 0 );
	void CopyUTF8(LPCSTR lpszSource, char chBad = '*');
	void CopyToUTF8(FStringA* pkOutput);

	//Convert a UTF-8 encoded string into the local system code page then Copy.
	//NOTE:	FString does not track character encoding!
	//		Most uses of FString assume the local system code page which may or may not be UTF-8.
	//		This method should always be used when working with UTF-8 buffers.
	bool CopyUTF8toMultiByte(LPCSTR szSource);


	// String concatenation
	void Concat( int iLength, LPCSTR lpszSource, int iFirst = 0 );
	void Concat( LPCSTR lpszSource );

	// Numeric conversion
	int ConvertToInt( int iRadix = 10 , bool bReadHex = true) const;
	float ConvertToFloat( int iRadix = 10 ) const;
	uint Hash( void ) const;

	// Encryption
	void Encrypt( void );
	void Decrypt( void );

	void SetFixedBuffer( FStringAData* pkData );

	void TakeBufferFrom( FStringA& szDonor );

	// Static Methods:
	//---------------
	static int SafeStrlen( LPCSTR lpsz );
	static int SafeStrcmp( LPCSTR lpszStr1, LPCSTR lpszStr2 );
	static int SafeStricmp( LPCSTR lpszStr1, LPCSTR lpszStr2 );
	static TCHAR * SafeStrDup( LPCSTR lpsz );
	
	static size_t SizeofFStringData( void );

	static FStringHashType Hash( LPCSTR lpsz );

	static char* InitializeFixedBufferAt( size_t iSize, void* pkBuffer, size_t iMaxBuffer );
	static void CopyToUTF8(LPCSTR pszSource, FStringA* pkOutput);
		
protected:
	// Methods:
	//--------
	bool IsLocked( void ) const;
	bool IsFixed(  void ) const;
#ifdef		_NDS
	bool IsLarge(  void ) const;
	bool IsStatic( void ) const;
#endif	//	_NDS

	bool MatchesOnePattern( LPCSTR lpszPattern, bool bCaseSensitive = false ) const;
	
#ifndef		_NDS
	FStringAData* GetData( void ) const;
#endif	//	_NDS

	int FindDriveLetter( void ) const;

	LPSTR GetBuffer( int iMinBufLength = 0 );

	void Init( void );
	void LockBuffer( void );
	void ReleaseBuffer( size_t iNewLength = -1 );
	void SetLength( size_t iLength );
	void DecLength( size_t iLength );
	void UnlockBuffer( void );
	void SetCharCount(  size_t iLength );
	int  GetAllocLength( void ) const;

#ifndef		_NDS
	void Reallocate( size_t iAllocSize );
	void Release( FStringAData* pkData );
#endif	//	!_NDS

	void  AllocBuffer( size_t nChars );

#ifdef		_NDS
	void* GetAllocPtr( void );

		  uint& Header( int index );		// Header accessors
	const uint& Header( int index ) const;
#endif	//	_NDS

private:
	// Members:
	//--------
	// WARNING: The m_pszString MUST be the ONLY member of this class!!!
	LPSTR m_pszString;	// Actual string data

	friend class FStringHashPalette; //  please don't hate me....
	friend class FStringW;
};

// Access to the common initializer data for an empty string
#if !defined(FXS_IS_DLL)
extern "C" 
{
	__declspec(dllexport) extern FStringA::FStringAData* FStringA_GetStringInitData();
}
#else
FStringA::FStringAData* FStringA_GetStringInitData();
#endif

// Friendly helpers
FStringA operator+( char ch, const FStringA& s2 );
FStringA operator+( LPCSTR s1, const FStringA& s2 );

FStringA operator-( char ch, const FStringA& s2 );
FStringA operator-( LPCSTR s1, const FStringA& s2 );

bool operator==( LPCSTR s1, const FStringA& s2 );
bool operator!=( LPCSTR s1, const FStringA& s2 );

bool operator<( LPCSTR s1, const FStringA& s2 );
bool operator<=( LPCSTR s1, const FStringA& s2 );
bool operator>( LPCSTR s1, const FStringA& s2 );
bool operator>=( LPCSTR s1, const FStringA& s2 );

//---------------------------------------------------------------------------------------
// FStringW
//
//	Represents a UNICODE string.
//
class FStringW
{
#ifndef		_NDS
public:
	// Structures:
	//-----------
	struct FStringWData	// Data for UNICODE strings.
	{
		// Using all ints for conformity
		int m_iAllocLength;	// Number of wchars allocated for the string but not including null terminator
		int m_iDataLength;	// Length of the string not including null terminator
		USHORT m_bLocked;	// If true, the buffer is locked; meaning GetBuffer() was called to get write access to the StringData
		USHORT m_bFixed;	// If true, the buffer is a fixed length and it cannot be re-allocated or released
		//wchar szData[ m_iAllocLength ];

		// Access to actual string/character data
		LPWSTR StringData( void );
	};
#endif	//	!_NDS

public:
	// Enums:
	//------
	enum CtorType
	{
		FORMAT = 1,	// pass this in to Format constructor
	};

#ifdef		_NDS
	enum FlagType
	{
		LOCKED = 1,	// active GetBuffer(), before ReleaseBuffer()
		STATIC = 2,	// static string alloc'd on Frame Heap
		FIXED  = 4,	// fixed string: no buffer ops
		LARGE  = 8,	// larger than 65499 characters
		UNUSED = 16
	};
#endif	//	_NDS

	// Classes:
	//--------
	class FStringWHashTraits	// Traits for hash_map
	{
	public:
		// Enums:
		//------
		enum
		{
			bucket_size = nFSTRING_HASH_TRAITS_BUCKET_SIZE,	// Mean number of elements per bucket
			min_buckets	= nFSTRING_HASH_TRAITS_MIN_BUCKETS,	// Minimum number of buckets to maintain
		};

		// Constructors:
		//-------------
		FStringWHashTraits( void );

		// Operators:
		//----------
		size_t operator()( const FStringW& str ) const;
		bool operator()( const FStringW& str1, const FStringW& str2 ) const;
	};

	class FStringWHashTraitsNoCase	// Traits for hash_map with case-insensitve comparison
	{
	public:
		// Enums:
		//------
		enum
		{
			bucket_size = nFSTRING_HASH_TRAITS_BUCKET_SIZE,	// Mean number of elements per bucket
			min_buckets	= nFSTRING_HASH_TRAITS_MIN_BUCKETS,	// Minimum number of buckets to maintain
		};

		// Constructors:
		//-------------
		FStringWHashTraitsNoCase( void );

		// Operators:
		//----------
		size_t operator()( const FStringW& str ) const;
		bool operator()( const FStringW& str1, const FStringW& str2 ) const;
	};

	class FBuffer	// Convenient class for accessing the internal buffer
	{
	public:
		// Constructors:
		//-------------
		FBuffer( FStringW& str, int iMinBufLength = 0, bool bNullTerminated = true );

		// Destructors:
		//------------
		~FBuffer( void );

		// Operators:
		//----------
		operator LPWSTR( void );


	private:
		// Members:
		//--------
		bool m_bNullTerminated;	// If true, the string is assumed to be null-terminated

		FStringW& m_strRef;	// The reference to the FString containing the buffer

		int m_iLength;	// The length of the string if it is not null-terminated

		LPWSTR m_pszBuffer;	// The actual string buffer
	};

	#ifndef		_NDS
	template < size_t nLENGTH >
	class FStringWFixedBuffer	// Helper class used to create strings of a fixed-length
	{
	public:
		// Constructors:
		//-------------
		FStringWFixedBuffer( FStringW& str );
		FStringWFixedBuffer();

		FStringWData &GetStringData() { return m_kStringData; }

	private:
		// Members:
		//--------
		FStringWData m_kStringData;	// Data for the fixed-length string; must come first
		
		wchar m_szStringBuffer[ nLENGTH ];	// Fixed-length string buffer; must be second
	};
	#endif	//	_NDS

	// Constructors:
	//-------------
	         FStringW( void );
	         FStringW( const FStringW& strSource );
	         FStringW( wchar ch, int iRepeat );
	         FStringW( LPCWSTR lpszSource );
	         FStringW( LPCWSTR lpszSource, int iLength );
	explicit FStringW( int iLength );
			 #ifdef		_NDS
	explicit FStringW( int iLength, void* pBuffer );
			 #endif
	         FStringW( const std::wstring& strSource );
	         FStringW( CtorType /* ignored, but needed to disambiguate vs other constructors */, LPCWSTR lpszFormat, ... );
			 #ifdef		_NDS
			 FStringW( FlagType /* ignored */ );
			 #endif	//	_NDS

	// Destructors:
	//------------
	~FStringW( void );

	// Operators:
	//----------
	// Assignment
	const FStringW& operator=( const FStringW& strSource );
	const FStringW& operator=( const std::wstring& strSource );
	const FStringW& operator=( wchar ch );
	const FStringW& operator=( LPCWSTR lpszSource );

	// Addition + assignment
	const FStringW& operator+=( const FStringW& strSource );
	const FStringW& operator+=( const std::wstring& strSource );
	const FStringW& operator+=( wchar ch );
	const FStringW& operator+=( LPCWSTR lpszSource );

	// Addition
	       FStringW operator+( const FStringW& s2 );
	       FStringW operator+( const std::wstring& s2 );
	       FStringW operator+( wchar ch );
	friend FStringW operator+( wchar ch, const FStringW& s2 );
	       FStringW operator+( LPCWSTR s2 );
	friend FStringW operator+( LPCWSTR s1, const FStringW& s2 );

	// Subtraction + assignment
	const FStringW& operator-=( const FStringW& strSource );
	const FStringW& operator-=( const std::wstring& strSource );
	const FStringW& operator-=( wchar ch );
	const FStringW& operator-=( LPCWSTR lpszSource );

	// Subtraction
	       FStringW operator-( const FStringW& s2 );
	       FStringW operator-( const std::wstring& s2 );
	       FStringW operator-( wchar ch );
	friend FStringW operator-( wchar ch, const FStringW& s2 );
	       FStringW operator-( LPCWSTR s2 );
	friend FStringW operator-( LPCWSTR s1, const FStringW& s2 );

	// Equality
	       bool operator==( const FStringW& s2 ) const;
	       bool operator==( LPCWSTR s2 ) const;
	friend bool operator==( LPCWSTR s1, const FStringW& s2 );

	// Inequality
	       bool operator!=( const FStringW& s2 ) const;
	       bool operator!=( LPCWSTR s2 ) const;
	friend bool operator!=( LPCWSTR s1, const FStringW& s2 );

	// Less than
	       bool operator<( const FStringW& s2 ) const;
	       bool operator<( LPCWSTR s2 ) const;
	friend bool operator<( LPCWSTR s1, const FStringW& s2 );

	// Less than or equal
	       bool operator<=( const FStringW& s2 ) const;
	       bool operator<=( LPCWSTR s2 ) const;
	friend bool operator<=( LPCWSTR s1, const FStringW& s2 );

	// Greater than
	       bool operator>( const FStringW& s2 ) const;
	       bool operator>( LPCWSTR s2 ) const;
	friend bool operator>( LPCWSTR s1, const FStringW& s2 );

	// Greater than or equal
	       bool operator>=( const FStringW& s2 ) const;
	       bool operator>=( LPCWSTR s2 ) const;
	friend bool operator>=( LPCWSTR s1, const FStringW& s2 );

	// Shifting
	FStringW  operator<< ( int iCount ) const;
	FStringW& operator<<=( int iCount );
	FStringW  operator>> ( int iCount ) const;
	FStringW& operator>>=( int iCount );

	// Subscript
	wchar& operator[]( int iIndex );
	wchar& operator[]( uint uiIndex );
	wchar operator[]( int iIndex ) const;
	wchar operator[]( uint uiIndex ) const;

	// Casting
	operator LPCWSTR( void ) const;

	// Methods:
	//--------
	// Attributes and operations
	void Empty( void );
	bool IsEmpty( void ) const;
	int  GetLength( void ) const;
	int  GetMaxLength( void ) const;

	// Access to string implementation buffer as "C" character array
	void Reserve( int iLength );
	void Release( void );
	void FreeExtra( void );
	LPCWSTR c_str( void ) const;
	LPCWSTR GetCString( void ) const;

	// Character operations
	wchar First( void ) const;
	wchar Last( void ) const;
	wchar GetAt( int iIndex ) const;
	void SetAt( int iIndex, wchar ch );
	void SetAt( int iIndex, int iLength, LPCWSTR lpszSource, int iFirst = 0 );

	// String comparison
	bool MatchesPattern( LPCWSTR lpszPattern, bool bCaseSensitive = false ) const;
	int Compare( LPCWSTR lpsz ) const;
	int Compare( LPCWSTR lpsz, int iLength ) const;
	int CompareNoCase( LPCWSTR lpsz ) const;
	int CompareNoCase( LPCWSTR lpsz, int iLength ) const;
	int Collate( LPCWSTR lpsz ) const;
	int CollateNoCase( LPCWSTR lpsz ) const;

	// Simple sub-string extraction
	FStringW Mid( int iFirst, int iCount ) const;
	FStringW Mid( int iFirst ) const;
	FStringW Left( int iCount ) const;
	FStringW Right( int iCount ) const;
	void Mid( FStringW & szDest, int iFirst, int iCount ) const;
	void Mid( FStringW & szDest, int iFirst ) const;
	void Left( FStringW & szDest, int iCount ) const;
	void Right( FStringW & szDest, int iCount ) const;

	// Advanced sub-string extraction
	FStringW SpanIncluding( LPCWSTR lpszCharSet, int iStart = 0 ) const;
	FStringW SpanExcluding( LPCWSTR lpszCharSet, int iStart = 0 ) const;
	FStringW Extract( int& iStart, LPCWSTR lpszCharSet ) const;
	FStringW Extract( int& iStart, wchar chEnd ) const;
	FStringW Extract( int& iStart, wchar chBegin, wchar chEnd ) const;
	void SpanIncluding( FStringW & szDest, LPCWSTR lpszCharSet, int iStart = 0 ) const;
	void SpanExcluding( FStringW & szDest, LPCWSTR lpszCharSet, int iStart = 0 ) const;
	void Extract( FStringW & szDest, int& iStart, LPCWSTR lpszCharSet ) const;
	void Extract( FStringW & szDest, int& iStart, wchar chEnd ) const;
	void Extract( FStringW & szDest, int& iStart, wchar chBegin, wchar chEnd ) const;

	// Path extraction
	FStringW ExtractDriveLetter( void ) const;
	FStringW ExtractFileDir( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	FStringW ExtractFileExtension( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	FStringW ExtractFileName( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	FStringW ExtractFilePath( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	FStringW ExtractFileTitle( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	void ExtractDriveLetter( FStringW & szDest ) const;
	void ExtractFileDir( FStringW & szDest, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	void ExtractFileExtension( FStringW & szDest, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	void ExtractFileName( FStringW & szDest, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	void ExtractFilePath( FStringW & szDest, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	void ExtractFileTitle( FStringW & szDest, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	void ReplaceDriveLetter( LPCWSTR lpszNewDrive, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void ReplaceFileDir( LPCWSTR lpszNewPath, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void ReplaceFileExtension( LPCWSTR lpszNewExt, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void ReplaceFileName( LPCWSTR lpszNewName, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void ReplaceFilePath( LPCWSTR lpszNewPath, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void ReplaceFileTitle( LPCWSTR lpszNewTitle, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void SplitPath( FStringW& strDriveLetter, FStringW& strFileDir, FStringW& strFileName, FStringW& strFileExt, wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W ) const;
	void StandardizePath( bool bChangeCase, bool bTerminate );
	void StripDriveLetter( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void StripFileDir( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void StripFileExtension( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void StripFileName( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void StripFilePath( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );
	void StripFileTitle( wchar chPathDelimiter = DEFAULT_PATH_DELIMITER_W );

	// Upper/Lower/Reverse conversion
	void MakeUpper( void );
	void MakeLower( void );
	void MakeReverse( void );

	// Trimming whitespace
	void TrimRight( void );
	void TrimRight( wchar chTarget );
	void TrimRight( LPCWSTR lpszTargets );
	void TrimLeft( void );
	void TrimLeft( wchar chTarget );
	void TrimLeft( LPCWSTR lpszTargets );
	void Trim( void );
	void Trim( wchar chTarget );
	void Trim( LPCWSTR lpszTargets );
	void TrimWhitespace( void );
	void Truncate( int iNewLength );

	// Advanced manipulation
	int Delete( int iIndex, int iCount = 1 );
	int Delete( LPCWSTR lpszSource );
	int Insert( int iIndex, wchar ch );
	int Insert( int iIndex, LPCWSTR lpsz );
	int Remove( wchar chRemove );
	int Remove( LPCWSTR lpszRemove );
	int RemoveRepeated( wchar chRemove );
	int Replace( wchar chOld, wchar chNew );
	int Replace( LPCWSTR lpszOld, LPCWSTR lpszNew );
	int Replace( wchar chOld, LPCWSTR lpszNew );
	int Replace( LPCWSTR lpszOld, wchar chNew );
	int Wrap( int iMaxLineLength );

	// Searching
	int Find( wchar ch, int iStart = 0 ) const;
	int Find( LPCWSTR lpszSub, int iStart = 0 ) const;
	int FindOneOf( LPCWSTR lpszCharSet, int iStart = 0 ) const;
	int ReverseFind( wchar ch, int iStart = -1 ) const;
	int FindCount( wchar ch, int iStart = 0 ) const;
	int FindToken( LPCWSTR lpszToken, int iStart = 0 ) const;
	int FindNextWord( int iStart = 0 ) const;
	int FindPrevWord( int iStart = -1 ) const;

	// Simple formatting
	void __cdecl Format( LPCWSTR lpszFormat, ... );
	void __cdecl Format( uint uiFormatID, ... );			// from custom (fxs) string table
#ifdef		_WINPC
	void __cdecl FormatResource( uint uiFormatID, ... );	// from microsoft string table in .rc file
#endif	//	_WINPC
	void FormatSize( uint uiSize );
	void FormatV( LPCWSTR lpszFormat, va_list vaArgList );
	void TranslateEscapeCodes( void );

	// Load from string resources
	bool LoadString( uint uiID );								// from custom (fxs) string table
#ifdef	_WINPC
	bool LoadResource( uint uiID, HINSTANCE hInstance = NULL );	// from microsoft string table in .rc file
#endif	//_WINPC

	// String copying
	void Copy( int iLength, LPCWSTR lpszSource, int iFirst = 0 );
	void CopySystemLocale(LPCSTR lpszSource);
	void CopyUTF8(LPCSTR lpszSource);
	void CopyToUTF8(FStringA* pkOutput) const;
	void CopyToSystemLocale(FStringA* pkOutput) const;

	// String concatenation
	void Concat( int iLength, LPCWSTR lpszSource, int iFirst = 0 );
	void Concat( LPCWSTR lpszSource );

	// Numeric conversion
	int ConvertToInt( int iRadix = 10, bool bReadHex = true ) const;
	float ConvertToFloat( int iRadix = 10 ) const;
	uint Hash( void ) const;

	// Encryption
	void Encrypt( void );
	void Decrypt( void );

	void SetFixedBuffer( FStringWData* pkData );

	void TakeBufferFrom( FStringW& szDonor );

	// Static Methods:
	//---------------
	static int SafeStrlen( LPCWSTR lpsz );
	static int SafeStrcmp( LPCWSTR lpszStr1, LPCWSTR lpszStr2 );
	static int SafeStricmp( LPCWSTR lpszStr1, LPCWSTR lpszStr2 );
	static WCHAR * SafeStrDup( LPCWSTR lpsz );
		
	static size_t SizeofFStringData( void );

	static FStringHashType Hash( LPCWSTR lpsz );

	static void CopyToUTF8(LPCWSTR pszSource, FStringA* pkOutput);
	static void CopyToSystemLocale(LPCWSTR pszSource, FStringA* pkOutput);

protected:
	// Methods:
	//--------
	bool IsLocked( void ) const;
	bool IsFixed(  void ) const;
#ifdef		_NDS
	bool IsLarge(  void ) const;
	bool IsStatic( void ) const;
#endif	//	_NDS

	bool MatchesOnePattern( LPCWSTR lpszPattern, bool bCaseSensitive = false ) const;
	
#ifndef		_NDS
	FStringWData* GetData( void ) const;
#endif	//	_NDS

	int FindDriveLetter( void ) const;
	
	LPWSTR GetBuffer( int iMinBufLength = 0 );

	void Init( void );
	void LockBuffer( void );
	void ReleaseBuffer( size_t iNewLength = -1 );
	void SetLength( size_t iLength );
	void DecLength( size_t iLength );
	void UnlockBuffer( void );
	void SetCharCount(  size_t iLength );
	int  GetAllocLength( void ) const;

#ifndef		_NDS
	void Reallocate( size_t iAllocSize );
	void Release( FStringWData* pkData );
#endif	//	!_NDS

	void* GetAllocPtr( void );
	void  AllocBuffer( size_t nChars );

#ifdef		_NDS
		  uint& Header( int index );		// Header accessors
	const uint& Header( int index ) const;
#endif	//	_NDS

private:
	// Members:
	//--------
	// WARNING: The m_pszString MUST be the ONLY member of this class!!!
	LPWSTR m_pszString;	// Actual string data
	
	friend class FStringHashPalette; //  please don't hate me....
};

// Access to the common initializer data for an empty string
#if !defined(FXS_IS_DLL)
extern "C" 
{
	__declspec(dllexport) extern FStringW::FStringWData* FStringW_GetStringInitData();
}
#else
FStringW::FStringWData* FStringW_GetStringInitData();
#endif

// Friendly helpers
FStringW operator+( wchar ch, const FStringW& s2 );
FStringW operator+( LPCWSTR s1, const FStringW& s2 );

FStringW operator-( wchar ch, const FStringW& s2 );
FStringW operator-( LPCWSTR s1, const FStringW& s2 );

bool operator==( LPCWSTR s1, const FStringW& s2 );
bool operator!=( LPCWSTR s1, const FStringW& s2 );

bool operator<( LPCWSTR s1, const FStringW& s2 );
bool operator<=( LPCWSTR s1, const FStringW& s2 );
bool operator>( LPCWSTR s1, const FStringW& s2 );
bool operator>=( LPCWSTR s1, const FStringW& s2 );

//---------------------------------------------------------------------------------------
// FString
//
//	Represents either an ANSI or UNICODE string depending on the current compilation
#ifndef		_UNICODE
	typedef FStringA FString;
	typedef FStringA::FStringAHashTraits		FStringHashTraits;
	typedef	FStringA::FStringAHashTraitsNoCase	FStringHashTraitsNoCase;
	#ifndef		_NDS
	#define		FStringFixedBufferObj	FStringA::FStringAFixedBuffer
	#else	//	!_NDS
	#define		FStringFixedBufferObj	FStringSTKA
	#define		FStringSTK				FStringSTKA
	#endif	//	!_NDS
#else	//	_UNICODE
	typedef FStringW FString;
	typedef FStringW::FStringWHashTraits		FStringHashTraits;
	typedef	FStringW::FStringWHashTraitsNoCase	FStringHashTraitsNoCase;
	#ifndef		_NDS
	#define		FStringFixedBufferObj	FStringW::FStringWFixedBuffer
	#else	//	!_NDS
	#define		FStringFixedBufferObj	FStringSTKW
	#define		FStringSTK				FStringSTKW
	#endif	//	!_NDS
#endif	//	_UNICODE

#ifdef		_NDS
	#define		FStringFixedBuffer					FStringSTK	// legacy
	#define		FStringSTKA( varname, size )  char varname##_buf[ (4 + size*sizeof( char) + 31) & ~31 ];  FStringA varname( sizeof(varname##_buf), varname##_buf )
	#define		FStringSTKW( varname, size )  char varname##_buf[ (4 + size*sizeof(wchar) + 31) & ~31 ];  FStringW varname( sizeof(varname##_buf), varname##_buf )
#else	//	_NDS
	#define		FStringFixedBuffer( varname, size ) FString varname; FStringFixedBufferObj<size> FStringFixedBuffer_##varname(varname); varname
	#define		FStringFixedBufferA( varname, size ) FStringA varname; FStringA::FStringAFixedBuffer<size> FStringFixedBuffer_##varname(varname); varname
	#define		FStringFixedBufferW( varname, size ) FStringW varname; FStringW::FStringWFixedBuffer<size> FStringFixedBuffer_##varname(varname); varname
#endif	//	_NDS

#ifdef  _NDS
#  define	MAX_PATH  252
#endif

#include	"FStringW.inl"
#include	"FStringA.inl"

#ifndef		LPCTSTR
#ifndef		_UNICODE
#define		LPCTSTR	LPCSTR
#else	//	_UNICODE
#define		LPCTSTR	LPCWSTR
#endif	//	_UNICODE
#endif	//	LPCTSTR

#ifndef		LPTSTR
#ifndef		_UNICODE
#define		LPTSTR	LPSTR
#else	//	_UNICODE
#define		LPTSTR	LPWSTR
#endif	//	_UNICODE
#endif	//	LPCTSTR

// Helper types
typedef FString* 				  FStringPtr;
typedef FArray< FStringPtr, c_eMPoolTypeFString > FStringPtrArray;

// Global data
extern const FString g_strEMPTY;	// A global empty string

typedef std::vector<std::string> StdStringVector;

#endif	//	FSTRING_H
