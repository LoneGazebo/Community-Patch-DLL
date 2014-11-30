//---------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Classes

inline FStringW::FStringWHashTraits::FStringWHashTraits( void )
{
	// does nothing
}

inline size_t FStringW::FStringWHashTraits::operator()( const FStringW& str ) const
{
	return ( str.Hash() );
}

inline bool FStringW::FStringWHashTraits::operator()( const FStringW& str1, const FStringW& str2 ) const
{
	return ( str1 < str2 );
}

inline FStringW::FStringWHashTraitsNoCase::FStringWHashTraitsNoCase( void )
{
	// does nothing
}

inline size_t FStringW::FStringWHashTraitsNoCase::operator()( const FStringW& str ) const
{
	return ( str.Hash() );
}

inline bool FStringW::FStringWHashTraitsNoCase::operator()( const FStringW& str1, const FStringW& str2 ) const
{
	return ( str1.CompareNoCase( str2 ) < 0 );
}

inline FStringW::FBuffer::FBuffer( FStringW& str, int iMinLength, bool bNullTerminated ): m_strRef( str )
{
	m_bNullTerminated = bNullTerminated;
	m_iLength = iMinLength;
	m_pszBuffer = m_strRef.GetBuffer( m_iLength );
}

inline FStringW::FBuffer::~FBuffer( void )
{
	m_strRef.ReleaseBuffer( m_bNullTerminated ? -1 : m_iLength );
}

inline FStringW::FBuffer::operator LPWSTR( void )
{
	return ( m_pszBuffer );
}

#ifndef		_NDS
template < size_t nLENGTH >
inline FStringW::FStringWFixedBuffer< nLENGTH >::FStringWFixedBuffer( FStringW& str )
{
	assert( nLENGTH > 0 );
	m_kStringData.m_bLocked = 0;
	m_kStringData.m_bFixed = 1;
	m_kStringData.m_iAllocLength = nLENGTH;
	m_kStringData.m_iDataLength = 0;
	m_szStringBuffer[ 0 ] = 0;
	str.SetFixedBuffer( &m_kStringData );
}
#endif	//	!_NDS

template < size_t nLENGTH >
inline FStringW::FStringWFixedBuffer< nLENGTH >::FStringWFixedBuffer()
{
	assert( nLENGTH > 0 );
	m_kStringData.m_bLocked = 0;
	m_kStringData.m_bFixed = 1;
	m_kStringData.m_iAllocLength = nLENGTH;
	m_kStringData.m_iDataLength = 0;
	m_szStringBuffer[ 0 ] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default constructor
inline FStringW::FStringW( void )
{
	Init();
}

#ifdef		_NDS
// Construct STATIC type
inline FStringW::FStringW( FlagType /* ignored */ )
{
	static int strEmpty[] = { FIXED | STATIC, 0 };	// Empty string

	m_pszString = (LPWSTR)&strEmpty[1];
}
#endif

// Copy constructor
inline FStringW::FStringW( const FStringW& strSource )
{
	Init();
	Copy( strSource.GetLength(), strSource );
}

// Construct from a unicode character
inline FStringW::FStringW( wchar ch, int iRepeat )
{
	Init();
	SetLength( iRepeat );
	if ( iRepeat > 0 )
	{
		memset( m_pszString, 1, sizeof ( wchar ) * iRepeat );
		_wcsnset( m_pszString, ch, iRepeat );
		m_pszString[ iRepeat ] = L'\0';
	}
}

// Convert from a unicode string
inline FStringW::FStringW( LPCWSTR lpszSource )
{
	Init();
	Copy( SafeStrlen( lpszSource ), lpszSource );
}

// Convert from a unicode string of the specified length
inline FStringW::FStringW( LPCWSTR lpszSource, int iLength )
{
	Init();
	Copy( iLength, lpszSource );
}

// Construct from std::wstring
inline FStringW::FStringW( const std::wstring& strSource )
{
	Init();
	Copy( (int)strSource.size(), strSource.c_str() );
}

// Construct with a specified length -- string remains uninitialized
inline FStringW::FStringW( int iLength )
{
	Init();
	Reserve( iLength );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor

inline FStringW::~FStringW( void )
{
	Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assignment Operators

inline const FStringW& FStringW::operator=( const FStringW& strSource )
{
	Copy( strSource.GetLength(), strSource );

	return ( *this );
}

inline const FStringW& FStringW::operator=( wchar ch )
{
	Copy( ch ? 1 : 0, &ch );

	return ( *this );
}

inline const FStringW& FStringW::operator=( LPCWSTR lpszSource )
{
	Copy( SafeStrlen( lpszSource ), lpszSource );

	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Addition + Assignment Operators

inline const FStringW& FStringW::operator+=( const FStringW& strSource )
{
	Concat( strSource.GetLength(), strSource );

	return ( *this );
}

inline const FStringW& FStringW::operator+=( wchar ch )
{
	Concat( ch ? 1 : 0, &ch );

	return ( *this );
}

inline const FStringW& FStringW::operator+=( LPCWSTR lpszSource )
{
	Concat( SafeStrlen( lpszSource ), lpszSource );

	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Addition Operators

inline FStringW FStringW::operator+( const FStringW& s2 )
{
	FStringW str;

	str = *this;
	str.Concat( s2.GetLength(), s2 );

	return ( str );
}

inline FStringW FStringW::operator+( wchar ch )
{
	FStringW str;

	str = *this;
	str.Concat( ch ? 1 : 0, &ch );

	return ( str );
}

inline FStringW operator+( wchar ch, const FStringW& s2 )
{
	FStringW str;

	str = ch;
	str.Concat( s2.GetLength(), s2 );

	return ( str );
}

inline FStringW FStringW::operator+( LPCWSTR s2 )
{
	FStringW str;

	str = *this;
	str.Concat( SafeStrlen( s2 ), s2 );

	return ( str );
}

inline FStringW operator+( LPCWSTR s1, const FStringW& s2 )
{
	FStringW str;

	str = s1;
	str.Concat( s2.GetLength(), s2 );

	return ( str );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subtraction + Assignment Operators

inline const FStringW& FStringW::operator-=( const FStringW& strSource )
{
	Delete( strSource );

	return ( *this );
}

inline const FStringW& FStringW::operator-=( wchar ch )
{
	FStringW strTemp;

	strTemp = ch;
	Delete( strTemp );

	return ( *this );
}

inline const FStringW& FStringW::operator-=( const std::wstring& strSource )
{
	FStringW strTemp;

	strTemp = strSource.c_str();
	Delete( strTemp );

	return ( *this );
}

inline const FStringW& FStringW::operator-=( LPCWSTR lpszSource )
{
	Delete( lpszSource );

	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subtraction Operators

inline FStringW FStringW::operator-( const FStringW& s2 )
{
	FStringW str;

	str = *this;
	str.Delete( s2 );

	return ( str );
}

inline FStringW FStringW::operator-( wchar ch )
{
	FStringW str;

	str = *this;
	str.Delete( ( LPCWSTR )&ch );

	return ( str );
}

inline FStringW operator-( wchar ch, const FStringW& s2 )
{
	FStringW str;

	str = ch;
	str.Delete( s2 );

	return ( str );
}

inline FStringW FStringW::operator-( const std::wstring& s2 )
{
	FStringW str;
	FStringW strTemp;

	str = *this;
	strTemp = s2.c_str();
	str.Delete( strTemp );

	return ( str );
}

inline FStringW FStringW::operator-( LPCWSTR s2 )
{
	FStringW str;

	str = *this;
	str.Delete( s2 );

	return ( str );
}

inline FStringW operator-( LPCWSTR s1, const FStringW& s2 )
{
	FStringW str;

	str = s1;
	str.Delete( s2 );

	return ( str );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Equality Operators

inline bool FStringW::operator==( const FStringW& s2 ) const
{
	return ( Compare( s2 ) == 0 );
}

inline bool FStringW::operator==( LPCWSTR s2 ) const
{
	return ( Compare( s2 ) == 0 );
}

inline bool operator==( LPCWSTR s1, const FStringW& s2 )
{
	return ( s2.Compare( s1 ) == 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inequality Operators

inline bool FStringW::operator!=( const FStringW& s2 ) const
{
	return ( Compare( s2 ) != 0 );
}

inline bool FStringW::operator!=( LPCWSTR s2 ) const
{
	return ( Compare( s2 ) != 0 );
}

inline bool operator!=( LPCWSTR s1, const FStringW& s2 )
{
	return ( s2.Compare( s1 ) != 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Less Than Operators

inline bool FStringW::operator<( const FStringW& s2 ) const
{
	return ( Compare( s2 ) < 0 );
}

inline bool FStringW::operator<( LPCWSTR s2 ) const
{
	return ( Compare( s2 ) < 0 );
}

inline bool operator<( LPCWSTR s1, const FStringW& s2 )
{
	return ( s2.Compare( s1 ) > 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Less Than Or Equal Operators

inline bool FStringW::operator<=( const FStringW& s2 ) const
{
	return ( Compare( s2 ) <= 0 );
}

inline bool FStringW::operator<=( LPCWSTR s2 ) const
{
	return ( Compare( s2 ) <= 0 );
}

inline bool operator<=( LPCWSTR s1, const FStringW& s2 )
{
	return ( s2.Compare( s1 ) >= 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Greater Than Operators

inline bool FStringW::operator>( const FStringW& s2 ) const
{
	return ( Compare( s2 ) > 0 );
}

inline bool FStringW::operator>( LPCWSTR s2 ) const
{
	return ( Compare( s2 ) > 0 );
}

inline bool operator>( LPCWSTR s1, const FStringW& s2 )
{
	return ( s2.Compare( s1 ) < 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Greater Than Or Equal Operators

inline bool FStringW::operator>=( const FStringW& s2 ) const
{
	return ( Compare( s2 ) >= 0 );
}

inline bool FStringW::operator>=( LPCWSTR s2 ) const
{
	return ( Compare( s2 ) >= 0 );
}

inline bool operator>=( LPCWSTR s1, const FStringW& s2 )
{
	return ( s2.Compare( s1 ) <= 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shifting Operators

inline FStringW FStringW::operator<<( int iCount ) const
{
	return ( Right( GetLength() - iCount ) );
}

inline FStringW& FStringW::operator<<=( int iCount )
{
	*this = *this << iCount;
	return ( *this );
}

inline FStringW FStringW::operator>>( int iCount ) const
{
	return ( Left( GetLength() - iCount ) );
}

inline FStringW& FStringW::operator>>=( int iCount )
{
	*this = *this >> iCount;
	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subscript Operators

inline wchar& FStringW::operator[]( int iIndex )
{
	FSTRING_CHECK_BUFFER;
	assert( ( uint )iIndex < ( uint )GetLength() );
	return ( m_pszString[ iIndex ] );
}

inline wchar& FStringW::operator[]( uint uiIndex )
{
	FSTRING_CHECK_BUFFER;
	assert( uiIndex < ( uint )GetLength() );
	return ( m_pszString[ uiIndex ] );
}

inline wchar FStringW::operator[]( int iIndex ) const
{
	return ( GetAt( iIndex ) );
}

inline wchar FStringW::operator[]( uint uiIndex ) const
{
	return ( GetAt( uiIndex ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Casting Operators

inline FStringW::operator LPCWSTR( void ) const
{
	return ( GetCString() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Character Operations

inline wchar FStringW::First( void ) const
{
	FSTRING_CHECK_BUFFER;
	return ( IsEmpty() ? 0 : m_pszString[ 0 ] );
}

inline wchar FStringW::Last( void ) const
{
	FSTRING_CHECK_BUFFER;
	return ( IsEmpty() ? 0 : m_pszString[ GetLength() - 1 ] );
}

inline wchar FStringW::GetAt( int iIndex ) const
{
	FSTRING_CHECK_BUFFER;
	assert( (uint)iIndex < (uint)GetLength() );
	return ( m_pszString[ iIndex ] );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Comparison

inline int FStringW::Compare( LPCWSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( wcscmp( m_pszString, lpsz ) );
}

inline int FStringW::Compare( LPCWSTR lpsz, int iLength ) const
{
	FSTRING_CHECK_BUFFER;
	return ( wcsncmp( m_pszString, lpsz, iLength ) );
}

inline int FStringW::CompareNoCase( LPCWSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( _wcsicmp( m_pszString, lpsz ) );
}

inline int FStringW::CompareNoCase( LPCWSTR lpsz, int iLength ) const
{
	FSTRING_CHECK_BUFFER;
	return ( _wcsnicmp( m_pszString, lpsz, iLength ) );
}

inline int FStringW::Collate( LPCWSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( wcscoll( m_pszString, lpsz ) );
}

inline int FStringW::CollateNoCase( LPCWSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( _wcsicoll( m_pszString, lpsz ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Sub-String Extraction

inline FStringW FStringW::Mid( int iFirst ) const
{
	return ( Mid( iFirst, GetLength() - iFirst ) );
}

inline FStringW FStringW::Left( int iCount ) const
{
	return ( Mid( 0, iCount ) );
}

inline FStringW FStringW::Right( int iCount ) const
{
	return ( Mid( GetLength() - iCount, iCount ) );
}

inline void FStringW::Mid( FStringW & szDest, int iFirst ) const
{
	return ( Mid( szDest, iFirst, GetLength() - iFirst ) );
}

inline void FStringW::Left( FStringW & szDest, int iCount ) const
{
	return ( Mid( szDest, 0, iCount ) );
}

inline void FStringW::Right( FStringW & szDest, int iCount ) const
{
	return ( Mid( szDest, GetLength() - iCount, iCount ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Advanced Sub-String Extraction

inline FStringW FStringW::SpanIncluding( LPCWSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		return ( FStringW() );
	}
	return ( Left( (int)wcsspn( m_pszString, lpszCharSet ) ) );
}

inline FStringW FStringW::SpanExcluding( LPCWSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		return ( FStringW() );
	}
	return ( Left( (int)wcscspn( m_pszString, lpszCharSet ) ) );
}

inline void FStringW::SpanIncluding( FStringW & szDest, LPCWSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		szDest = L"";
		return;
	}
	Left( szDest, (int)wcsspn( m_pszString, lpszCharSet ) );
}

inline void FStringW::SpanExcluding( FStringW & szDest, LPCWSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		szDest = L"";
		return;
	}
	Left( szDest, (int)wcscspn( m_pszString, lpszCharSet ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Path Extraction

inline void FStringW::ReplaceDriveLetter( LPCWSTR lpszNewDrive, wchar chPathDelimiter )
{
	*this = lpszNewDrive + ExtractFileDir( chPathDelimiter ) + ExtractFileName( chPathDelimiter );
}

inline void FStringW::ReplaceFileDir( LPCWSTR lpszNewDir, wchar chPathDelimiter )
{
	*this = ExtractDriveLetter() + lpszNewDir + ExtractFileName( chPathDelimiter );
}

inline void FStringW::ReplaceFileExtension( LPCWSTR lpszNewExt, wchar chPathDelimiter )
{
	*this = ExtractFilePath( chPathDelimiter ) + ExtractFileTitle( chPathDelimiter ) + lpszNewExt;
}

inline void FStringW::ReplaceFileName( LPCWSTR lpszNewName, wchar chPathDelimiter )
{
	*this = ExtractFilePath( chPathDelimiter ) + lpszNewName;
}

inline void FStringW::ReplaceFilePath( LPCWSTR lpszNewPath, wchar chPathDelimiter )
{
	*this = lpszNewPath + ExtractFileName( chPathDelimiter );
}

inline void FStringW::ReplaceFileTitle( LPCWSTR lpszNewTitle, wchar chPathDelimiter )
{
	*this = ExtractFilePath( chPathDelimiter ) + lpszNewTitle + ExtractFileExtension( chPathDelimiter );
}

inline void FStringW::SplitPath( FStringW& strDriveLetter, FStringW& strFileDir, FStringW& strFileName, FStringW& strFileExt, wchar chPathDelimiter ) const
{
	strDriveLetter = ExtractDriveLetter();
	strFileDir = ExtractFileDir( chPathDelimiter );
	strFileName = ExtractFileName( chPathDelimiter );
	strFileExt = ExtractFileExtension( chPathDelimiter );
}

inline void FStringW::StandardizePath( bool bChangeCase, bool bTerminate )
{
#ifdef		_PS3
	Replace( L'\\', L'/' );
#else	//	_PS3
	Replace( L'/', L'\\' );
#endif	//	_PS3

	if ( bChangeCase )
	{
#ifdef		_PS3
		MakeLower();
#else	//	_PS3
#ifdef		_XBOX
		MakeUpper();
#endif	//	_XBOX
#endif	//	_PS3
	}

	if ( bTerminate && ( Last() != DEFAULT_PATH_DELIMITER_W ) )
	{
		*this += DEFAULT_PATH_DELIMITER_W;
	}
}

inline void FStringW::StripDriveLetter( wchar chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKW( szTempDir,  MAX_PATH );
	FStringSTKW( szTempName, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferW( szTempDir, 512 );
	FStringFixedBufferW( szTempName, 512 );
	#endif	//	_NDS	
	ExtractFileDir(  szTempDir,  chPathDelimiter );
	ExtractFileName( szTempName, chPathDelimiter );
	szTempDir += szTempName;
	*this = szTempDir;
}

inline void FStringW::StripFileDir( wchar chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKW( szTempDrive, MAX_PATH );
	FStringSTKW( szTempName,  MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferW(szTempDrive, 16 );
	FStringFixedBufferW(szTempName, 512 );
	#endif	//	_NDS
	ExtractDriveLetter(szTempDrive);
	ExtractFileName( szTempName, chPathDelimiter );
	szTempDrive += szTempName;
	*this = szTempDrive;
}

inline void FStringW::StripFileExtension( wchar chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKW( szTempPath,  MAX_PATH );
	FStringSTKW( szTempTitle, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferW(szTempPath, 512 );
	FStringFixedBufferW(szTempTitle, 512 );
	#endif	//	_NDS
	ExtractFilePath(  szTempPath,  chPathDelimiter );
	ExtractFileTitle( szTempTitle, chPathDelimiter );
	szTempPath += szTempTitle;
	*this = szTempPath; 
}

inline void FStringW::StripFileName( wchar chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKW( szTempPath, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferW(szTempPath, 512);
	#endif	//	_NDS
	ExtractFilePath( szTempPath, chPathDelimiter );
	*this = szTempPath;
}

inline void FStringW::StripFilePath( wchar chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKW( szTempName, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferW(szTempName, 512);
	#endif	//	_NDS
	ExtractFileName( szTempName, chPathDelimiter );
	*this = szTempName;
}

inline void FStringW::StripFileTitle( wchar chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKW( szTempPath, MAX_PATH );
	FStringSTKW( szTempExt,  MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferW(szTempPath, 512 );
	FStringFixedBufferW(szTempExt, 512 );
	#endif	//	_NDS
	ExtractFilePath( szTempPath, chPathDelimiter );
	ExtractFileExtension( szTempExt, chPathDelimiter );
	szTempPath += szTempExt;
	*this = szTempPath;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Upper/Lower/Reverse Conversion

inline void FStringW::MakeUpper( void )
{
	FSTRING_CHECK_BUFFER;
	_wcsupr( m_pszString );
}

inline void FStringW::MakeLower( void )
{
	FSTRING_CHECK_BUFFER;
	_wcslwr( m_pszString );
}

inline void FStringW::MakeReverse( void )
{
	FSTRING_CHECK_BUFFER;
	_wcsrev( m_pszString );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Trimming Whitespace

inline void FStringW::Trim( void )
{
	TrimLeft();
	TrimRight();
}

inline void FStringW::Trim( wchar chTarget )
{
	TrimLeft( chTarget );
	TrimRight( chTarget );
}

inline void FStringW::Trim( LPCWSTR lpszTargets )
{
	TrimLeft( lpszTargets );
	TrimRight( lpszTargets );
}

inline void FStringW::TrimWhitespace( void )
{
	Replace( L'\t', L' ' );
	Trim( L' ' );
	RemoveRepeated( L' ' );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Advanced manipulation

inline int FStringW::Replace( wchar chOld, LPCWSTR lpszNew )
{
	const wchar szOld[ 2 ] = { chOld };

	return ( Replace( szOld, lpszNew ) );
}

inline int FStringW::Replace( LPCWSTR lpszOld, wchar chNew )
{
	const wchar szNew[ 2 ] = { chNew };

	return ( Replace( lpszOld, szNew ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String concatenation

inline void FStringW::Concat( int iLength, LPCWSTR lpszSource, int iFirst )
{
	SetAt( GetLength(), iLength, lpszSource, iFirst );
}

inline void FStringW::Concat( LPCWSTR lpszSource )
{
	Concat( SafeStrlen( lpszSource ), lpszSource );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Numeric conversion

// Calculates a hash value for the string
inline uint FStringW::Hash( void ) const
{
	FSTRING_CHECK_BUFFER;
	return ( g_CRC32.Calc( ( void* )m_pszString, GetLength() * sizeof ( wchar ) ) );
}

// Calculates a hash value for the string
// static 
inline uint FStringW::Hash( LPCWSTR pszStr )
{
	FAssert(pszStr != NULL);
	return ( g_CRC32.Calc( ( void* )pszStr, SafeStrlen(pszStr) * sizeof ( wchar ) ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access to string implementation buffer as "C" character array

#ifndef		_NDS
inline int FStringW::GetAllocLength( void ) const
{
	return ( GetData()->m_iAllocLength );
}
#endif	//	!_NDS

inline LPCWSTR FStringW::GetCString( void ) const
{ 
	FSTRING_CHECK_BUFFER;
	return ( m_pszString );
}

// stl-style accessor
inline LPCWSTR FStringW::c_str( void ) const
{
	return ( GetCString() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Methods

inline int FStringW::SafeStrlen( LPCWSTR lpsz )
{
	return ( lpsz ? ( int )wcslen( lpsz ) : 0 );
}

inline int FStringW::SafeStrcmp( LPCWSTR lpszStr1, LPCWSTR lpszStr2 )
{
	if ( lpszStr1 == lpszStr2 ) return 0;
	if ( lpszStr1 == NULL && lpszStr2 != NULL )
		return -1;
	if ( lpszStr2 == NULL && lpszStr1 != NULL ) 
		return 1;
	return wcscmp( lpszStr1, lpszStr2 );
}

inline int FStringW::SafeStricmp( LPCWSTR lpszStr1, LPCWSTR lpszStr2 )
{
	if ( lpszStr1 == lpszStr2 ) 
		return 0;
	if ( lpszStr1 == NULL && lpszStr2 != NULL )
		return -1;
	if ( lpszStr2 == NULL && lpszStr1 != NULL ) 
		return 1;
	return _wcsicmp( lpszStr1, lpszStr2 );
}


#ifndef		_NDS
inline size_t FStringW::SizeofFStringData( void )
{
	return ( sizeof ( FStringWData ) );
}
#endif	//	!_NDS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FStringAData

#ifndef		_NDS
inline LPWSTR FStringW::FStringWData::StringData( void )
{
	return ( ( LPWSTR )( this + 1 ) );
}
#endif	//	!_NDS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Protected methods

#ifndef		_NDS
inline FStringW::FStringWData* FStringW::GetData( void ) const
{
	assert( m_pszString != NULL );

	return ( ( ( FStringWData* )m_pszString ) - 1 );
}
#endif	//	!_NDS

inline void FStringW::Init( void )
{
#ifdef		_NDS
	static int strEmpty[] = { FIXED, 0 };	// Empty string

	m_pszString = (LPWSTR)&strEmpty[1];
#else	//	_NDS
	m_pszString = FStringW_GetStringInitData()->StringData();
#endif	//	_NDS
}

#ifndef		_NDS
inline void FStringW::Release( void )
{
	Release( GetData() );
	Init();
}
#endif	//	!_NDS

#ifndef		_NDS
inline void FStringW::Release( FStringWData* pkData )
{
	FSTRING_CHECK_BUFFER;
	if ( pkData && ( pkData != FStringW_GetStringInitData() ) && !pkData->m_bFixed )
	{
#ifdef		ENABLE_FSTRING_STATS
		extern uint g_uiFStringBytesCurrent;
		g_uiFStringBytesCurrent -= sizeof ( FStringWData ) + ( sizeof ( char ) * pkData->m_iAllocLength + 1 );
#endif	//	ENABLE_FSTRING_STATS

		// Free any memory associated with the string
		delete [] ( byte* )pkData;
	}
}
#endif	//	!_NDS

#ifndef		_NDS
inline void FStringW::SetFixedBuffer( FStringWData* pkData )
{
	assert( pkData != NULL );
	if(!pkData) return;
	assert( pkData->m_bFixed == 1 );
	assert( pkData->m_iAllocLength > 0 );
	Release();
	m_pszString = pkData->StringData();
}
#endif	//	!_NDS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes and Operations

inline void FStringW::Empty( void )
{
	SetLength( 0 );
}

//------------------------------------------------------------------------------------------------
inline bool FStringW::IsEmpty( void ) const
{
	return ( GetLength() == 0 );
}

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline uint& FStringW::Header( int index )
{
	return ((uint*)m_pszString)[-index];
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline const uint& FStringW::Header( int index ) const
{
	return ((uint*)m_pszString)[-index];
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline void* FStringW::GetAllocPtr( void )
{
	int header = IsLarge() ? 8 : 4;

	return ( (LPSTR)m_pszString - header );
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline int FStringW::GetMaxLength( void ) const
{
	int length = GetAllocLength();
	if (length == 0)  return 0;
	int header = IsLarge() ? 8 : 4;
	return (length - header) / sizeof(wchar) - 1;
}
#endif

//------------------------------------------------------------------------------------------------
inline void FStringW::DecLength( size_t iLength )
{
	SetLength( GetLength() - iLength );
}

//------------------------------------------------------------------------------------------------
inline int FStringW::GetLength( void ) const
{
	#ifdef		_NDS
	if (IsLarge())
		return Header(2);
	else
		return Header(1) >> 16;
	#else	//	_NDS
	FSTRING_CHECK_BUFFER;
	return ( GetData()->m_iDataLength );
	#endif	//	_NDS
}

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline void FStringW::SetCharCount( int iLength )
{
	FSTRING_CHECK_BUFFER;

	if (IsLarge())
		Header(2) = iLength;
	else
		Header(1) = ((uint)iLength<<16) + (Header(1) & 0xFFFF);
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline int FStringW::GetAllocLength( void ) const
{
	int length = Header(1) & ~31;

	if (!IsLarge())  length &= 0xFFFF;

	if (IsStatic())  length /= 8;

	return length;
}
#endif	//	_NDS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Protected methods

//------------------------------------------------------------------------------------------------
inline bool FStringW::IsFixed( void ) const
{
#ifdef		_NDS
	return ( Header(1) & FIXED );
#else	//	_NDS
	return ( GetData()->m_bFixed ? true : false );
#endif	//	_NDS
}

//------------------------------------------------------------------------------------------------
inline bool FStringW::IsLocked( void ) const
{
#ifdef		_NDS
	return ( Header(1) & LOCKED );
#else	//	_NDS
	return ( GetData()->m_bLocked ? true : false );
#endif	//	_NDS
}

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline bool FStringW::IsStatic( void ) const
{
	return ( Header(1) & STATIC );
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline bool FStringW::IsLarge( void ) const
{
	return ( Header(1) & LARGE );
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
inline void FStringW::LockBuffer( void )
{
	assert( !IsLocked() );
#ifdef		_NDS
	Header(1) |= LOCKED;
#else	//	_NDS
	GetData()->m_bLocked = 1;
#endif	//	_NDS
}

//------------------------------------------------------------------------------------------------
inline void FStringW::UnlockBuffer( void )
{
	assert( IsLocked() );
#ifdef		_NDS
	Header(1) &= ~LOCKED;
#else	//	_NDS
	GetData()->m_bLocked = 0;
#endif	//	_NDS
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AllocBuffer
#ifdef		_NDS
inline void FStringW::AllocBuffer( int nChars )
{
	int flags  = (IsStatic() && GetAllocLength()==0) ? STATIC+FIXED : 0;	// only create new STATIC if null
	int header = 4;								// header bytes
	int buffer = (nChars + 1) * sizeof(wchar);	// buffer bytes
	int nAlloc = header + buffer;				// malloc bytes

	int maxAlloc = (0xFFFF & ~31) / (flags ? 8 : 1);	// max SMALL alloc

	int maxSmall = (maxAlloc - header) / sizeof(wchar) - 1;	// largest nChars for SMALL header

	if (nChars > maxSmall)
	{
		header = 8;
		nAlloc = header + buffer;
		flags |= LARGE;
	}

	void* pkData = NULL;

	if (flags & STATIC)
	{
		nAlloc = (nAlloc +  3) &  ~3;	// round UP to 4-byte block
		pkData = MallocX( nAlloc );
		nAlloc *= 8;					// shift #blocks up past flags
	}
	else
	{
		nAlloc = (nAlloc + 31) & ~31;	// round UP to 32-byte block
		pkData = Malloc( nAlloc );
	}
	FAssert( pkData != NULL );

	m_pszString = (LPWSTR)((LPSTR)pkData + header);	// assign new string, overwrite old, will orphan if FIXED

	Header(1) = nAlloc | flags;
	SetCharCount( 0 );
	m_pszString[0] = L'\0';
}
#endif	//	_NDS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construct FIXED from an external buffer
#ifdef		_NDS
inline FStringW::FStringW( int iLength, void* pBuffer )
{
	Init();	// safety

	if (!pBuffer)  return;

	iLength &= ~31;	// total FString block MUST be multiple of 32-bytes

	int flags  = FIXED;
	int header = 4;		// assume SMALL

	if ((uint)iLength >= 65536)	
	{
		flags |= LARGE;
		header = 8;
	}

	m_pszString = (LPWSTR)((LPSTR)pBuffer + header);

	Header(1) = iLength | flags;
	SetCharCount( 0 );
	m_pszString[0] = L'\0';
}
#endif	//	_NDS
