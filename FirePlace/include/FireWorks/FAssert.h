#ifndef FASSERT_H
#define FASSERT_H

// Only compile in FAssert's if FASSERT_ENABLE is defined.  By default, however, let's key off of
// _DEBUG.  Sometimes, however, it's useful to enable asserts in release builds, and you can do that
// simply by changing the following lines to define FASSERT_ENABLE or using project settings to override
#if !defined(FINAL_RELEASE)
#define		FASSERT_ENABLE
#endif	//	FINAL_RELEASE

#ifdef		_NDS
#undef		FASSERT_ENABLE
#endif

#if			defined(NDEBUG) && !defined(_NDS)	// _assert() is in ALL run-time-libs, but not prototyped in RELEASE
extern "C" _CRTIMP void __cdecl _assert(const char *, const char *, unsigned);
#endif	//	NDEBUG

// VERIFY() works like assert() but is also called in release.
#undef		VERIFY
#ifdef		_DEBUG
#define		VERIFY(f)	FAssert(f)
#else	//	_DEBUG
#define		VERIFY(f)	(f)
#endif	//	_DEBUG

// These macros will cause an FAssert, and if the expression is false, perform the given code.
// This can be used, for example, to return from a function when something unexpected happens:
// ReturnValue * MyFunction( Class * pkParameter )
// {
//	FAssertExecuteMsg( pkParameter != 0, return NULL, "Unexpected NULL parameter." );
//	... // real code goes here.
// }
#define FAssertExecute( expr, statement ) FAssert(expr); if ( !(expr) ) { statement; };
#define FAssertExecuteMsg( expr, statement, msg ) FAssertMsg(expr, msg); if ( !(expr) ) { statement; };
#define FAssertExecuteMsg0( expr, statement, _fmt ) FAssertMsg(expr, _fmt); if ( !(expr) ) { statement; };
#define FAssertExecuteMsg1( expr, statement, _fmt, _p0 ) FAssertMsg1(expr, _fmt, _p0); if ( !(expr) ) { statement; };
#define FAssertExecuteMsg2( expr, statement, _fmt, _p0, _p1 ) FAssertMsg2(expr, _fmt, _p0, _p1); if ( !(expr) ) { statement; };
#define FAssertExecuteMsg3( expr, statement, _fmt, _p0, _p1, _p2 ) FAssertMsg3(expr, _fmt, _p0, _p1, _p2); if ( !(expr) ) { statement; };
#define FAssertExecuteMsg4( expr, statement, _fmt, _p0, _p1, _p2, _p3 ) FAssertMsg4(expr, msg_fmt, _p0, _p1, _p2, _p3); if ( !(expr) ) { statement; };

#if !defined(FXS_IS_DLL)
// functions to allow runtime control over assert mechanism
void FAssertEnable(bool bEnable);
bool FAssertIsEnabled();
void FAssertEnableDialog(bool bEnable);
bool FAssertIsDialogEnabled();

// These must be set (and called) to do things, such as device locking on Xenon
typedef void (*FASSERT_CALLBACK)();
void SetPreAssertCallback( FASSERT_CALLBACK pfnPreAssertCallback );
void SetPostAssertCallback( FASSERT_CALLBACK pfnPostAssertCallback );
typedef void (*FASSERT_LOG_CALLBACK)(const char* pszMessage);
void SetAssertLogCallback( FASSERT_LOG_CALLBACK pfnAssertLogCallback );
#endif

#ifdef FASSERT_ENABLE

	// Set breakpoint creation macros
	#if defined (_WINPC) || defined (_XENON)
	#define FASSERT_BREAKPOINT __debugbreak()
	#else
	#define FASSERT_BREAKPOINT assert(0)
	#endif

	// Each platform must implement this function, which returns whether to break or not
	bool FAssertDlg( const char*, const char*, const char*, unsigned int, bool& );

	#define FAssert( expr )	\
	{ \
		static bool bIgnoreAlways = false; \
		if( !bIgnoreAlways && !(expr) ) \
		{ \
			if( FAssertDlg( #expr, 0, __FILE__, __LINE__, bIgnoreAlways ) ) \
				{ FASSERT_BREAKPOINT; } \
		} \
	}

	#define FAssertMsg( expr, msg ) \
	{ \
		static bool bIgnoreAlways = false; \
		if( !bIgnoreAlways && !(expr) ) \
		{ \
			if( FAssertDlg( #expr, msg, __FILE__, __LINE__, bIgnoreAlways ) ) \
				{ FASSERT_BREAKPOINT; } \
		} \
	}

	// Defined for convenience, these allow you to format arguments in your asserts
	// Unfortunately, since they are macros, you can't use var_arg, and so you have to
	// know how many parameters you are using at compile time.

	#define FAssertMsg0( expr, _fmt ) FAssertMsg( expr, _fmt )
	#define FAssertMsg1( expr, _fmt, _p0 ) \
	{ \
		static bool bIgnoreAlways = false; \
		if( !bIgnoreAlways && !(expr) ) \
		{ \
			char szAssertMsgBuffer[2048]; \
			sprintf( szAssertMsgBuffer, _fmt, _p0 ); \
			if( FAssertDlg( #expr, szAssertMsgBuffer, __FILE__, __LINE__, bIgnoreAlways ) ) \
					{ FASSERT_BREAKPOINT; } \
		} \
	}
	#define FAssertMsg2( expr, _fmt, _p0, _p1 ) \
	{ \
		static bool bIgnoreAlways = false; \
		if( !bIgnoreAlways && !(expr) ) \
		{ \
			char szAssertMsgBuffer[2048]; \
			sprintf( szAssertMsgBuffer, _fmt, _p0, _p1 ); \
			if( FAssertDlg( #expr, szAssertMsgBuffer, __FILE__, __LINE__, bIgnoreAlways ) ) \
					{ FASSERT_BREAKPOINT; } \
		} \
	}
	#define FAssertMsg3( expr, _fmt, _p0, _p1, _p2 ) \
	{ \
		static bool bIgnoreAlways = false; \
		if( !bIgnoreAlways && !(expr) ) \
		{ \
			char szAssertMsgBuffer[2048]; \
			sprintf( szAssertMsgBuffer, _fmt, _p0, _p1, _p2 ); \
			if( FAssertDlg( #expr, szAssertMsgBuffer, __FILE__, __LINE__, bIgnoreAlways ) ) \
					{ FASSERT_BREAKPOINT; } \
		} \
	}
	#define FAssertMsg4( expr, _fmt, _p0, _p1, _p2, _p3 ) \
	{ \
		static bool bIgnoreAlways = false; \
		if( !bIgnoreAlways && !(expr) ) \
		{ \
			char szAssertMsgBuffer[2048]; \
			sprintf( szAssertMsgBuffer, _fmt, _p0, _p1, _p2, _p3 ); \
			if( FAssertDlg( #expr, szAssertMsgBuffer, __FILE__, __LINE__, bIgnoreAlways ) ) \
					{ FASSERT_BREAKPOINT; } \
		} \
	}

    #define FAssertMsg6( expr, _fmt, _p0, _p1, _p2, _p3, _p4, _p5 ) \
    { \
    static bool bIgnoreAlways = false; \
    if( !bIgnoreAlways && !(expr) ) \
    { \
    char szAssertMsgBuffer[2048]; \
    sprintf( szAssertMsgBuffer, _fmt, _p0, _p1, _p2, _p3, _p4, _p5 ); \
    if( FAssertDlg( #expr, szAssertMsgBuffer, __FILE__, __LINE__, bIgnoreAlways ) ) \
    { FASSERT_BREAKPOINT; } \
    } \
    }

#else
	// FASSERT_ENABLE not defined
	#define FAssert( expr )
	#define FAssertMsg( expr, msg )
	#define FAssertMsg0( expr, _fmt )
	#define FAssertMsg1( expr, _fmt, _p0 )
	#define FAssertMsg2( expr, _fmt, _p0, _p1 )
	#define FAssertMsg3( expr, _fmt, _p0, _p1, _p2 )
	#define FAssertMsg4( expr, _fmt, _p0, _p1, _p2, _p3 )
	#define FAssertMsg6( expr, _fmt, _p0, _p1, _p2, _p3, _p4, _p5 )

#endif

#define	FASSERT( expr, msg )	FAssertMsg( expr, msg )

#ifndef		ASSERT
#define		ASSERT					FAssert
#endif	//	ASSERT

// A class that disables asserts when it is instantiated and
// re-enables them (if they were enabled to being with) when 
// it goes out of scope
class FScopedAssertDisabler 
{
	protected:
		bool m_bAssertsEnabled;

	public:
		FScopedAssertDisabler();
		~FScopedAssertDisabler();
};

#endif // FASSERT_H
