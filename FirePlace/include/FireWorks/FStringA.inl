//---------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Classes

inline FStringA::FStringAHashTraits::FStringAHashTraits( void )
{
	// does nothing
}

inline size_t FStringA::FStringAHashTraits::operator()( const FStringA& str ) const
{
	return ( str.Hash() );
}

inline bool FStringA::FStringAHashTraits::operator()( const FStringA& str1, const FStringA& str2 ) const
{
	return ( str1 < str2 );
}

inline FStringA::FStringAHashTraitsNoCase::FStringAHashTraitsNoCase( void )
{
	// does nothing
}

inline size_t FStringA::FStringAHashTraitsNoCase::operator()( const FStringA& str ) const
{
	return ( str.Hash() );
}

inline bool FStringA::FStringAHashTraitsNoCase::operator()( const FStringA& str1, const FStringA& str2 ) const
{
	return ( str1.CompareNoCase( str2 ) < 0 );
}

inline FStringA::FBuffer::FBuffer( FStringA& str, int iMinLength, bool bNullTerminated ): m_strRef( str )
{
	m_bNullTerminated = bNullTerminated;
	m_iLength = iMinLength;
	m_pszBuffer = m_strRef.GetBuffer( m_iLength );
}

inline FStringA::FBuffer::~FBuffer( void )
{
	m_strRef.ReleaseBuffer( m_bNullTerminated ? -1 : m_iLength );
}

inline FStringA::FBuffer::operator LPSTR( void )
{
	return ( m_pszBuffer );
}

#ifndef		_NDS
template < size_t nLENGTH >
inline FStringA::FStringAFixedBuffer< nLENGTH >::FStringAFixedBuffer( FStringA& str )
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
inline FStringA::FStringAFixedBuffer< nLENGTH >::FStringAFixedBuffer()
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
inline FStringA::FStringA( void )
{
	Init();
}

#ifdef		_NDS
// Construct STATIC type
inline FStringA::FStringA( FlagType /* ignored */ )
{
	static int strEmpty[] = { FIXED | STATIC, 0 };	// Empty string

	m_pszString = (LPSTR)&strEmpty[1];
}
#endif	//	_NDS

// Copy constructor
inline FStringA::FStringA( const FStringA& strSource )
{
	Init();
	Copy( strSource.GetLength(), strSource );
}

// Construct from a character
inline FStringA::FStringA( char ch, int iRepeat )
{
	Init();
	SetLength( iRepeat );
	if ( iRepeat > 0 )
	{
		memset( m_pszString, 1, sizeof ( char ) * iRepeat );
		_strnset( m_pszString, ch, iRepeat );
		m_pszString[ iRepeat ] = '\0';
	}
}

// Construct from a string
inline FStringA::FStringA( LPCSTR lpszSource )
{
	Init();
	Copy( SafeStrlen( lpszSource ), lpszSource );
}

// Construct from a string of the specified length
inline FStringA::FStringA( LPCSTR lpszSource, int iLength )
{
	Init();
	Copy( iLength, lpszSource );
}

// Construct from std::string
inline FStringA::FStringA( const std::string& strSource )
{
	Init();
	Copy( (int)strSource.size(), strSource.c_str() );
}

// Construct with a specified length -- string remains uninitialized
inline FStringA::FStringA( int iLength )
{
	Init();
	Reserve( iLength );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor

inline FStringA::~FStringA( void )
{
	Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assignment Operators

inline const FStringA& FStringA::operator=( const FStringA& strSource )
{
	Copy( strSource.GetLength(), strSource );

	return ( *this );
}

inline const FStringA& FStringA::operator=( const std::string& strSource )
{
	Copy( (int)strSource.size(), strSource.c_str() );

	return ( *this );
}

inline const FStringA& FStringA::operator=( char ch )
{
	Copy( ch ? 1 : 0, &ch );

	return ( *this );
}

inline const FStringA& FStringA::operator=( LPCSTR lpszSource )
{
	Copy( SafeStrlen( lpszSource ), lpszSource );

	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Addition + Assignment Operators

inline const FStringA& FStringA::operator+=( const FStringA& strSource )
{
	Concat( strSource.GetLength(), strSource );

	return ( *this );
}

inline const FStringA& FStringA::operator+=( const std::string& strSource )
{
	Concat( (int)strSource.size(), strSource.c_str() );

	return ( *this );
}

inline const FStringA& FStringA::operator+=( char ch )
{
	Concat( ch ? 1 : 0, &ch );

	return ( *this );
}

inline const FStringA& FStringA::operator+=( LPCSTR lpszSource )
{
	Concat( SafeStrlen( lpszSource ), lpszSource );

	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Addition Operators

inline FStringA FStringA::operator+( const FStringA& s2 )
{
	FStringA str;

	str = *this;
	str.Concat( s2.GetLength(), s2 );

	return ( str );
}

inline FStringA FStringA::operator+( const std::string& s2 )
{
	FStringA str;

	str = *this;
	str.Concat( (int)s2.size(), s2.c_str() );

	return ( str );
}

inline FStringA FStringA::operator+( char ch )
{
	FStringA str;

	str = *this;
	str.Concat( ch ? 1 : 0, &ch );

	return ( str );
}

inline FStringA operator+( char ch, const FStringA& s2 )
{
	FStringA str;

	str = ch;
	str.Concat( s2.GetLength(), s2 );

	return ( str );
}

inline FStringA FStringA::operator+( LPCSTR s2 )
{
	FStringA str;

	str = *this;
	str.Concat( SafeStrlen( s2 ), s2 );

	return ( str );
}

inline FStringA operator+( LPCSTR s1, const FStringA& s2 )
{
	FStringA str;

	str = s1;
	str.Concat( s2.GetLength(), s2 );

	return ( str );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subtraction + Assignment Operators

inline const FStringA& FStringA::operator-=( const FStringA& strSource )
{
	Delete( strSource );

	return ( *this );
}

inline const FStringA& FStringA::operator-=( const std::string& strSource )
{
	Delete( strSource.c_str() );

	return ( *this );
}

inline const FStringA& FStringA::operator-=( char ch )
{
	Delete( ( LPCSTR )&ch );

	return ( *this );
}

inline const FStringA& FStringA::operator-=( LPCSTR lpszSource )
{
	Delete( lpszSource );

	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subtraction Operators

inline FStringA FStringA::operator-( const FStringA& s2 )
{
	FStringA str;

	str = *this;
	str.Delete( s2 );

	return ( str );
}

inline FStringA FStringA::operator-( const std::string& s2 )
{
	FStringA str;

	str = *this;
	str.Delete( s2.c_str() );

	return ( str );
}

inline FStringA FStringA::operator-( char ch )
{
	FStringA str;

	str = *this;
	str.Delete( ( LPCSTR )&ch );

	return ( str );
}


inline FStringA operator-( char ch, const FStringA& s2 )
{
	FStringA str;

	str = ch;
	str.Delete( s2 );

	return ( str );
}

inline FStringA FStringA::operator-( LPCSTR s2 )
{
	FStringA str;

	str = *this;
	str.Delete( s2 );

	return ( str );
}

inline FStringA operator-( LPCSTR s1, const FStringA& s2 )
{
	FStringA str;

	str = s1;
	str.Delete( s2 );

	return ( str );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Equality Operators

inline bool FStringA::operator==( const FStringA& s2 ) const
{
	return ( Compare( s2 ) == 0 );
}

inline bool FStringA::operator==( const std::string& s2 ) const
{
	return ( Compare( s2.c_str() ) == 0 );
}

inline bool FStringA::operator==( LPCSTR s2 ) const
{
	return ( Compare( s2 ) == 0 );
}

inline bool operator==( LPCSTR s1, const FStringA& s2 )
{
	return ( s2.Compare( s1 ) == 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inequality Operators

inline bool FStringA::operator!=( const FStringA& s2 ) const
{
	return ( Compare( s2 ) != 0 );
}

inline bool FStringA::operator!=( const std::string& s2 ) const
{
	return ( Compare( s2.c_str() ) != 0 );
}

inline bool FStringA::operator!=( LPCSTR s2 ) const
{
	return ( Compare( s2 ) != 0 );
}

inline bool operator!=( LPCSTR s1, const FStringA& s2 )
{
	return ( s2.Compare( s1 ) != 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Less Than Operators

inline bool FStringA::operator<( const FStringA& s2 ) const
{
	return ( Compare( s2 ) < 0 );
}

inline bool FStringA::operator<( const std::string& s2 ) const
{
	return ( Compare( s2.c_str() ) < 0 );
}

inline bool FStringA::operator<( LPCSTR s2 ) const
{
	return ( Compare( s2 ) < 0 );
}

inline bool operator<( LPCSTR s1, const FStringA& s2 )
{
	return ( s2.Compare( s1 ) > 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Less Than Or Equal Operators

inline bool FStringA::operator<=( const FStringA& s2 ) const
{
	return ( Compare( s2 ) <= 0 );
}

inline bool FStringA::operator<=( const std::string& s2 ) const
{
	return ( Compare( s2.c_str() ) <= 0 );
}

inline bool FStringA::operator<=( LPCSTR s2 ) const
{
	return ( Compare( s2 ) <= 0 );
}

inline bool operator<=( LPCSTR s1, const FStringA& s2 )
{
	return ( s2.Compare( s1 ) >= 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Greater Than Operators

inline bool FStringA::operator>( const FStringA& s2 ) const
{
	return ( Compare( s2 ) > 0 );
}

inline bool FStringA::operator>( const std::string& s2 ) const
{
	return ( Compare( s2.c_str() ) > 0 );
}

inline bool FStringA::operator>( LPCSTR s2 ) const
{
	return ( Compare( s2 ) > 0 );
}

inline bool operator>( LPCSTR s1, const FStringA& s2 )
{
	return ( s2.Compare( s1 ) < 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Greater Than Or Equal Operators

inline bool FStringA::operator>=( const FStringA& s2 ) const
{
	return ( Compare( s2 ) >= 0 );
}

inline bool FStringA::operator>=( const std::string& s2 ) const
{
	return ( Compare( s2.c_str() ) >= 0 );
}

inline bool FStringA::operator>=( LPCSTR s2 ) const
{
	return ( Compare( s2 ) >= 0 );
}

inline bool operator>=( LPCSTR s1, const FStringA& s2 )
{
	return ( s2.Compare( s1 ) <= 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shifting Operators

inline FStringA FStringA::operator<<( int iCount ) const
{
	return ( Right( GetLength() - iCount ) );
}

inline FStringA& FStringA::operator<<=( int iCount )
{
	*this = *this << iCount;
	return ( *this );
}

inline FStringA FStringA::operator>>( int iCount ) const
{
	return ( Left( GetLength() - iCount ) );
}

inline FStringA& FStringA::operator>>=( int iCount )
{
	*this = *this >> iCount;
	return ( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subscript Operators

inline char& FStringA::operator[]( int iIndex )
{
	FSTRING_CHECK_BUFFER;
	assert( ( uint )iIndex < ( uint )GetLength() );
	return ( m_pszString[ iIndex ] );
}

inline char& FStringA::operator[]( uint uiIndex )
{
	FSTRING_CHECK_BUFFER;
	assert( uiIndex < ( uint )GetLength() );
	return ( m_pszString[ uiIndex ] );
}

inline char FStringA::operator[]( int iIndex ) const
{
	return ( GetAt( iIndex ) );
}

inline char FStringA::operator[]( uint uiIndex ) const
{
	return ( GetAt( uiIndex ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Casting Operators

inline FStringA::operator LPCSTR( void ) const
{
	return ( GetCString() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Character Operations

inline char FStringA::First( void ) const
{
	FSTRING_CHECK_BUFFER;
	return ( IsEmpty() ? 0 : m_pszString[ 0 ] );
}

inline char FStringA::Last( void ) const
{
	FSTRING_CHECK_BUFFER;
	return ( IsEmpty() ? 0 : m_pszString[ GetLength() - 1 ] );
}

inline char FStringA::GetAt( int iIndex ) const
{
	FSTRING_CHECK_BUFFER;
	assert( (uint)iIndex < (uint)GetLength() );
	return ( m_pszString[ iIndex ] );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Comparison

inline int FStringA::Compare( LPCSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( strcmp( m_pszString, lpsz ) );
}

inline int FStringA::Compare( LPCSTR lpsz, int iLength ) const
{
	FSTRING_CHECK_BUFFER;
	return ( strncmp( m_pszString, lpsz, iLength ) );
}

inline int FStringA::CompareNoCase( LPCSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( _stricmp( m_pszString, lpsz ) );
}

inline int FStringA::CompareNoCase( LPCSTR lpsz, int iLength ) const
{
	FSTRING_CHECK_BUFFER;
	return ( _strnicmp( m_pszString, lpsz, iLength ) );
}

inline int FStringA::Collate( LPCSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( strcoll( m_pszString, lpsz ) );
}

inline int FStringA::CollateNoCase( LPCSTR lpsz ) const
{
	FSTRING_CHECK_BUFFER;
	return ( _stricoll( m_pszString, lpsz ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Sub-String Extraction

inline FStringA FStringA::Mid( int iFirst ) const
{
	return ( Mid( iFirst, GetLength() - iFirst ) );
}

inline FStringA FStringA::Left( int iCount ) const
{
	return ( Mid( 0, iCount ) );
}

inline FStringA FStringA::Right( int iCount ) const
{
	return ( Mid( GetLength() - iCount, iCount ) );
}

inline void FStringA::Mid( FStringA & szDest, int iFirst ) const
{
	return ( Mid( szDest, iFirst, GetLength() - iFirst ) );
}

inline void FStringA::Left( FStringA & szDest, int iCount ) const
{
	return ( Mid( szDest, 0, iCount ) );
}

inline void FStringA::Right( FStringA & szDest, int iCount ) const
{
	return ( Mid( szDest, GetLength() - iCount, iCount ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Advanced Sub-String Extraction

inline FStringA FStringA::SpanIncluding( LPCSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		return ( FStringA() );
	}
	return ( Left( (int)strspn( m_pszString + iStart, lpszCharSet ) ) );
}

inline FStringA FStringA::SpanExcluding( LPCSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		return ( FStringA() );
	}
	return ( Left( (int)strcspn( m_pszString + iStart, lpszCharSet ) ) );
}

inline void FStringA::SpanIncluding( FStringA & szDest, LPCSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		szDest = "";
		return;
	}
	Left( szDest, (int)strspn( m_pszString + iStart, lpszCharSet ) );
}

inline void FStringA::SpanExcluding( FStringA & szDest, LPCSTR lpszCharSet, int iStart ) const
{
	FSTRING_CHECK_BUFFER;

	if ( ( iStart >= GetLength() ) || ( iStart < 0 ) )
	{
		szDest = "";
		return;
	}
	Left( szDest, (int)strcspn( m_pszString + iStart, lpszCharSet ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Path Extraction

inline void FStringA::ReplaceDriveLetter( LPCSTR lpszNewDrive, char chPathDelimiter )
{
	FStringFixedBufferA(sFileDir,MAX_PATH);
	FStringFixedBufferA(sFileName,MAX_PATH);
	ExtractFileDir( sFileDir, chPathDelimiter );
	ExtractFileName( sFileName, chPathDelimiter );
	*this = lpszNewDrive;
	*this += sFileDir;
	*this += sFileName;
}

inline void FStringA::ReplaceFileDir( LPCSTR lpszNewDir, char chPathDelimiter )
{
	FStringFixedBufferA(sDriveLetter, 16);
	FStringFixedBufferA(sFileName, MAX_PATH);
	ExtractDriveLetter(sDriveLetter);
	ExtractFileName(sFileName);
	*this = sDriveLetter;
	*this += lpszNewDir;
	*this += sFileName;
}

inline void FStringA::ReplaceFileExtension( LPCSTR lpszNewExt, char chPathDelimiter )
{
	FStringFixedBufferA(sFilePath,MAX_PATH);
	FStringFixedBufferA(sFileTitle,MAX_PATH);
	ExtractFilePath( sFilePath, chPathDelimiter );
	ExtractFileTitle( sFileTitle, chPathDelimiter );
	*this = sFilePath;
	*this += sFileTitle;
	*this += lpszNewExt;
}

inline void FStringA::ReplaceFileName( LPCSTR lpszNewName, char chPathDelimiter )
{
	FStringFixedBufferA(sTemp,MAX_PATH);
	ExtractFilePath( sTemp, chPathDelimiter );
	*this = sTemp;
	*this += lpszNewName;
}

inline void FStringA::ReplaceFilePath( LPCSTR lpszNewPath, char chPathDelimiter )
{
	FStringFixedBufferA(sFileName,MAX_PATH);
	ExtractFileName( sFileName, chPathDelimiter );
	*this = lpszNewPath;
	*this += sFileName;
}

inline void FStringA::ReplaceFileTitle( LPCSTR lpszNewTitle, char chPathDelimiter )
{
	FStringFixedBufferA(sFilePath,MAX_PATH);
	FStringFixedBufferA(sFileExtension,MAX_PATH);
	ExtractFilePath(sFilePath, chPathDelimiter );
	ExtractFileExtension(sFileExtension, chPathDelimiter);
	*this = sFilePath;
	*this += lpszNewTitle;
	*this += sFileExtension;
}

inline void FStringA::SplitPath( FStringA& strDriveLetter, FStringA& strFileDir, FStringA& strFileName, FStringA& strFileExt, char chPathDelimiter ) const
{
	strDriveLetter = ExtractDriveLetter();
	strFileDir = ExtractFileDir( chPathDelimiter );
	strFileName = ExtractFileName( chPathDelimiter );
	strFileExt = ExtractFileExtension( chPathDelimiter );
}

inline void FStringA::StandardizePath( bool bChangeCase, bool bTerminate )
{
#ifdef		_PS3
	Replace( '\\', '/' );
#else	//	_PS3
	Replace( '/', '\\' );
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

	if ( bTerminate && ( Last() != DEFAULT_PATH_DELIMITER_A ) )
	{
		*this += DEFAULT_PATH_DELIMITER_A;
	}
}

inline void FStringA::StripDriveLetter( char chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKA( szTempDir,  MAX_PATH );
	FStringSTKA( szTempName, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferA( szTempDir, 512 );
	FStringFixedBufferA( szTempName, 512 );
	#endif	//	_NDS	
	ExtractFileDir(  szTempDir,  chPathDelimiter );
	ExtractFileName( szTempName, chPathDelimiter );
	szTempDir += szTempName;
	*this = szTempDir;
}

inline void FStringA::StripFileDir( char chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKA( szTempDrive, MAX_PATH );
	FStringSTKA( szTempName,  MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferA(szTempDrive, 16 );
	FStringFixedBufferA(szTempName, 512 );
	#endif	//	_NDS
	ExtractDriveLetter( szTempDrive );
	ExtractFileName( szTempName, chPathDelimiter );
	szTempDrive += szTempName;
	*this = szTempDrive;
}

inline void FStringA::StripFileExtension( char chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKA( szTempPath,  MAX_PATH );
	FStringSTKA( szTempTitle, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferA(szTempPath, 512 );
	FStringFixedBufferA(szTempTitle, 512 );
	#endif	//	_NDS
	ExtractFilePath(  szTempPath,  chPathDelimiter );
	ExtractFileTitle( szTempTitle, chPathDelimiter );
	szTempPath += szTempTitle;
	*this = szTempPath; 
}

inline void FStringA::StripFileName( char chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKA( szTempPath, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferA(szTempPath, 512);
	#endif	//	_NDS
	ExtractFilePath( szTempPath, chPathDelimiter );
	*this = szTempPath;
}

inline void FStringA::StripFilePath( char chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKA( szTempName, MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferA(szTempName, 512);
	#endif	//	_NDS
	ExtractFileName( szTempName, chPathDelimiter );
	*this = szTempName;
}

inline void FStringA::StripFileTitle( char chPathDelimiter )
{
	#ifdef		_NDS
	FStringSTKA( szTempPath, MAX_PATH );
	FStringSTKA( szTempExt,  MAX_PATH );
	#else	//	_NDS
	FStringFixedBufferA(szTempPath, 512 );
	FStringFixedBufferA(szTempExt, 512 );
	#endif	//	_NDS
	ExtractFilePath( szTempPath, chPathDelimiter );
	ExtractFileExtension( szTempExt, chPathDelimiter );
	szTempPath += szTempExt;
	*this = szTempPath;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Upper/Lower/Reverse Conversion

inline void FStringA::MakeUpper( void )
{
	FSTRING_CHECK_BUFFER;
#ifdef		_PS3
	PS3_strupr( m_pszString );
#else	//	_PS3
	_strupr( m_pszString );
#endif	//	_PS3
}

inline void FStringA::MakeLower( void )
{
	FSTRING_CHECK_BUFFER;
#ifdef		_PS3
	PS3_strlwr( m_pszString );
#else	//	_PS3
	_strlwr( m_pszString );
#endif	//	_PS3
}

inline void FStringA::MakeReverse( void )
{
	FSTRING_CHECK_BUFFER;
	_strrev( m_pszString );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Trimming Whitespace

inline void FStringA::Trim( void )
{
	TrimLeft();
	TrimRight();
}

inline void FStringA::Trim( char chTarget )
{
	TrimLeft( chTarget );
	TrimRight( chTarget );
}

inline void FStringA::Trim( LPCSTR lpszTargets )
{
	TrimLeft( lpszTargets );
	TrimRight( lpszTargets );
}

inline void FStringA::TrimWhitespace( void )
{
	Replace( '\t', ' ' );
	Trim( ' ' );
	RemoveRepeated( ' ' );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Advanced manipulation

inline int FStringA::Replace( char chOld, LPCSTR lpszNew )
{
	const char szOld[ 2 ] = { chOld };

	return ( Replace( szOld, lpszNew ) );
}

inline int FStringA::Replace( LPCSTR lpszOld, char chNew )
{
	const char szNew[ 2 ] = { chNew };

	return ( Replace( lpszOld, szNew ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String concatenation

inline void FStringA::Concat( int iLength, LPCSTR lpszSource, int iFirst )
{
	SetAt( GetLength(), iLength, lpszSource, iFirst );
}

inline void FStringA::Concat( LPCSTR lpszSource )
{
	Concat( SafeStrlen( lpszSource ), lpszSource );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Numeric conversion

// Calculates a hash value for the string
inline uint FStringA::Hash( void ) const
{
	FSTRING_CHECK_BUFFER;
	return ( g_CRC32.Calc( ( void* )m_pszString, GetLength() * sizeof ( char ) ) );
}

// Calculates a hash value for the string
// static 
inline uint FStringA::Hash( LPCSTR pszStr )
{
	FAssert(pszStr != NULL);
	return ( g_CRC32.Calc( ( void* )pszStr, SafeStrlen(pszStr) * sizeof ( char ) ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access to string implementation buffer as "C" character array

#ifndef		_NDS
inline int FStringA::GetAllocLength( void ) const
{
	return ( GetData()->m_iAllocLength );
}
#endif	//	!_NDS

inline LPCSTR FStringA::GetCString( void ) const
{ 
	FSTRING_CHECK_BUFFER;
	return ( m_pszString );
}

// stl-style accessor
inline LPCSTR FStringA::c_str( void ) const
{
	return ( GetCString() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Methods

inline int FStringA::SafeStrlen( LPCSTR lpsz )
{
	return ( lpsz ? ( int )strlen( lpsz ) : 0 );
}

inline int FStringA::SafeStrcmp( LPCSTR lpszStr1, LPCSTR lpszStr2 )
{
	if ( lpszStr1 == lpszStr2 ) return 0;
	if ( lpszStr1 == NULL && lpszStr2 != NULL )
		return -1;
	if ( lpszStr2 == NULL && lpszStr1 != NULL ) 
		return 1;
	return strcmp( lpszStr1, lpszStr2 );
}

inline int FStringA::SafeStricmp( LPCSTR lpszStr1, LPCSTR lpszStr2 )
{
	if ( lpszStr1 == lpszStr2 ) return 0;
	if ( lpszStr1 == NULL && lpszStr2 != NULL )
		return -1;
	if ( lpszStr2 == NULL && lpszStr1 != NULL ) 
		return 1;
	return _stricmp( lpszStr1, lpszStr2 );
}


#ifndef		_NDS
inline size_t FStringA::SizeofFStringData( void )
{
	return ( sizeof ( FStringAData ) );
}
#endif	//	!_NDS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FStringAData

#ifndef		_NDS
inline LPSTR FStringA::FStringAData::StringData( void )
{
	return ( ( LPSTR )( this + 1 ) );
}
#endif	//	!_NDS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Protected methods

#ifndef		_NDS
inline FStringA::FStringAData* FStringA::GetData( void ) const
{
	assert( m_pszString != NULL );

	return ( ( ( FStringAData* )m_pszString ) - 1 );
}
#endif	//	!_NDS

inline void FStringA::Init( void )
{
#ifdef		_NDS
	static int strEmpty[] = { FIXED, 0 };	// Empty string

	m_pszString = (LPSTR)&strEmpty[1];
#else	//	_NDS
	m_pszString = FStringA_GetStringInitData()->StringData();
#endif	//	_NDS
}

#ifndef		_NDS
inline void FStringA::Release( void )
{
	Release( GetData() );
	m_pszString = NULL;
	Init();
}
#endif	//	!_NDS

#ifndef		_NDS
inline void FStringA::Release( FStringAData* pkData )
{
	FSTRING_CHECK_BUFFER;
	if ( pkData && ( pkData != FStringA_GetStringInitData() ) && !pkData->m_bFixed )
	{
#ifdef		ENABLE_FSTRING_STATS
		extern uint g_uiFStringBytesCurrent;
		g_uiFStringBytesCurrent -= sizeof ( FStringAData ) + ( sizeof ( char ) * pkData->m_iAllocLength + 1 );
#endif	//	ENABLE_FSTRING_STATS

		// Free any memory associated with the string
		delete [] ( byte* )pkData;
	}
}
#endif	//	!_NDS

#ifndef		_NDS
inline void FStringA::SetFixedBuffer( FStringAData* pkData )
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
inline void FStringA::Empty( void )
{
	SetLength( 0 );
}

//------------------------------------------------------------------------------------------------
inline bool FStringA::IsEmpty( void ) const
{
	return ( GetLength() == 0 );
}

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline uint& FStringA::Header( int index )
{
	return ((uint*)m_pszString)[-index];
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline const uint& FStringA::Header( int index ) const
{
	return ((uint*)m_pszString)[-index];
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline void* FStringA::GetAllocPtr( void )
{
	int header = IsLarge() ? 8 : 4;

	return ( m_pszString - header );
}

#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline int FStringA::GetMaxLength( void ) const
{
	int length = GetAllocLength();
	if (length == 0)  return 0;
	int header = IsLarge() ? 8 : 4;
	return (length - header) / sizeof(char) - 1;
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
inline void FStringA::DecLength( size_t iLength )
{
	SetLength( GetLength() - iLength );
}

//------------------------------------------------------------------------------------------------
inline int FStringA::GetLength( void ) const
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
inline void FStringA::SetCharCount( int iLength )
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
inline int FStringA::GetAllocLength( void ) const
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
inline bool FStringA::IsFixed( void ) const
{
#ifdef		_NDS
	return ( Header(1) & FIXED );
#else	//	_NDS
	return ( GetData()->m_bFixed ? true : false );
#endif	//	_NDS
}

//------------------------------------------------------------------------------------------------
inline bool FStringA::IsLocked( void ) const
{
#ifdef		_NDS
	return ( Header(1) & LOCKED );
#else	//	_NDS
	return ( GetData()->m_bLocked ? true : false );
#endif	//	_NDS
}

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline bool FStringA::IsStatic( void ) const
{
	return ( Header(1) & STATIC );
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
#ifdef		_NDS
inline bool FStringA::IsLarge( void ) const
{
	return ( Header(1) & LARGE );
}
#endif	//	_NDS

//------------------------------------------------------------------------------------------------
inline void FStringA::LockBuffer( void )
{
	assert( !IsLocked() );
#ifdef		_NDS
	Header(1) |= LOCKED;
#else	//	_NDS
	GetData()->m_bLocked = 1;
#endif	//_NDS
}

//------------------------------------------------------------------------------------------------
inline void FStringA::UnlockBuffer( void )
{
	assert( IsLocked() );
#ifdef		_NDS
	Header(1) &= ~LOCKED;
#else	//	_NDS
	GetData()->m_bLocked = 0;
#endif	//	_NDS
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	AllocBuffer
#ifdef		_NDS
inline void FStringA::AllocBuffer( int nChars )
{
	int flags  = (IsStatic() && GetAllocLength()==0) ? STATIC+FIXED : 0;	// only create new STATIC if null
	int header = 4;								// header bytes
	int buffer = (nChars + 1) * sizeof(char);	// buffer bytes
	int nAlloc = header + buffer;				// malloc bytes

	int maxAlloc = (0xFFFF & ~31) / (flags ? 8 : 1);	// max SMALL alloc

	int maxSmall = (maxAlloc - header) / sizeof(char) - 1;	// largest nChars for SMALL header

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

	m_pszString = (LPSTR)pkData + header;	// assign new string, overwrite old, will orphan if FIXED

	Header(1) = nAlloc | flags;
	SetCharCount( 0 );
	m_pszString[0] = '\0';
}
#endif	//	_NDS

//////////////////////////////////////////////////////////////////////////////////////////////////
// Construct FIXED from an external buffer
#ifdef		_NDS
inline FStringA::FStringA( int iLength, void* pBuffer )
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

	m_pszString = (LPSTR)pBuffer + header;

	Header(1) = iLength | flags;
	SetCharCount( 0 );
	m_pszString[0] = '\0';
}
#endif	//	_NDS

//************************************************************************************************
//****************    F S T R I N G :: H E A D E R    ~~    N O T E S    *************************
//****************           N i n t e n d o   D S   O n l y             *************************
//************************************************************************************************
//
//	ALL header manipulation code is above, allowing the CPP file and the rest of
//	this INL file to be generic.  The two immediately prior functions are too
//	large for inline, but wanted to keep all header code together in one place.
//
//	FStrings have a compressed header and is based on 32-byte allocation blocks.
//	Most strings will be SMALL (<= 65499 chars) with a 4 byte header as follows:
//
//	  |-------------------------------.-------------------------.---------|
//	  |        Current nChars         |       Alloc Blocks      |  FLAGS  |
//	  |-------------------------------.-------------------------.---------|
//
//	This header is ALWAYS accessed as a 32-bit integer to be endian agnostic.
//	Currently, the FLAGS field is 5-bits with only 4 in use.
//	This maps to the lower 5 unused bits of 32-byte allocation sizes.
//
//	Other allocation block sizes can be easily implemented.
//
//	Largest SMALL string is based on largest alloc block size, minus header and null terminator.
//
//	FIXED type strings are supported that operate on a fixed buffer passed into a c'tor.
//	These are typically used for stack based FStrings and hence the macro:
//
//					FStringSTK( varname, buffer_size );
//
//	Many small static strings are permanent throughout the game.  To conserve memory,
//  a new STATIC type has been created.  This is a FIXED string that is alloc'd from
//	a low-overhead memory heap.  For NDS/Wii, this is a Frame Heap with 4-byte granularity
//	and zero overhead.  NDS malloc manager always allocs 32-byte increments with a full
//	32-byte OS header.  So on average, we save 48-bytes per static string.  The original
//	FString implementation was particularly misfit to NDS as it had a 16-byte header and
//	32-byte increment data areas, yielding a minimum FString of 96-bytes or on average
//	80-bytes of overhead per FString.
//
//	The current implementation of STATIC uses a 4-byte block size, thus limiting
//	STATIC SMALL strings to 8183 chars.  Could easily be 16375 since the upper
//	flag bit is unused.
//
//	For both types of FIXED strings, a safety mechanism is built in: should the string be
//	extended beyond the fixed buffer length, a normal memory FString will be created and
//	the original fixed buffer will be orphaned.  There is an assert and a counter in 
//	::SetLength() to detect this condition.
//
//************************************************************************************************
//****************    F S T R I N G :: H E A D E R    ~~    N O T E S    *************************
//************************************************************************************************
