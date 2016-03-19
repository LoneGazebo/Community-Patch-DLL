#pragma once

#include <FireWorks\FFireTypes.h>
#include <FireWorks\FILogFile.h>

#include "pp.h"

// Comment/Uncomment to toggle logging
//#undef __JDHLOGFILE

// Comment/Uncomment to toggle ANALYZING
#define MOD_ANALYZE


namespace jdh
{

	enum LOG_LEVEL
	{
		ERROR
		, WARNING
		, INFO
		, DEBUG
		, ANALYSE
		, NONE
	};

	TOSTRING_DECL(LOG_LEVEL, level)
	{
		switch (level) {
		case NONE:
			buffer += "    NONE: "; break;
		case ANALYSE:
			buffer += " ANALYSE: "; break;
		case DEBUG:
			buffer += "   DEBUG: "; break;
		case INFO:
			buffer += "    INFO: "; break;
		case WARNING:
			buffer += " WARNING: "; break;
		case ERROR:
			buffer += "   ERROR: "; break;
		default:
			buffer += " UNKNOWN: ";
		}
	}

#ifdef __JDHLOGFILE
	namespace impl 
	{
		PCSTR timestring();

		extern LOG_LEVEL g_LogLevel;
		extern std::stack<LOG_LEVEL> g_LogLevelStack;

		extern size_t g_IdentIndex /*= 0*/;
		const size_t g_IdentMax = 40;
		extern char g_Ident[g_IdentMax+1]; // = "\0                                       ";

		inline void IdentInc()
		{
			JDH_ASSERT_NOLOG(g_IdentIndex + 2 < g_IdentMax - 2);
			JDH_ASSERT_NOLOG(g_IdentIndex % 2 == 0);
			g_Ident[g_IdentIndex] = ' ';
			g_IdentIndex += 2;
			g_Ident[g_IdentIndex] = '\0';
		}
		inline void IdentDec()
		{
			JDH_ASSERT_NOLOG(g_IdentIndex >= 2);
			JDH_ASSERT_NOLOG(g_IdentIndex % 2 == 0);
			g_Ident[g_IdentIndex] = ' ';
			g_IdentIndex -= 2;
			g_Ident[g_IdentIndex] = '\0';
		}
		struct ScopedIdent
		{
			ScopedIdent() { IdentInc(); }
			~ScopedIdent() { IdentDec(); }
		};

		class LogLockGuard
		{
		public:
			typedef boost::mutex mutex;

		private:
			static mutex s_m;

		public:
			BOOST_THREAD_NO_COPYABLE(LogLockGuard);

			explicit LogLockGuard() { s_m.lock(); }
			~LogLockGuard() { s_m.unlock(); }
		};

		std::string& LogFileBuffer(LOG_LEVEL eLevel, LogLockGuard& lock);


		inline void Log(std::string const & buffer)
		{
			LOGFILEMGR.GetLog(__JDHLOGFILE, FILogFile::kDontFlushOnWrite | FILogFile::kWriteToConsole)->Msg(buffer.c_str());
			OutputDebugString(buffer.c_str()); OutputDebugString("\n");
		}
	}


	inline void SetLogLevel(LOG_LEVEL eLogLevel)
	{
		impl::g_LogLevel = eLogLevel;
	}
#else
	inline void SetLogLevel(LOG_LEVEL eLogLevel) {}
#endif // __JDHLOGFILE
}



#ifdef __JDHLOGFILE


#define __JDHLOG_APPEND(buffer, ...)         BOOST_PP_EXPR_IF(MSVC_VARIADIC_SIZE(__VA_ARGS__), jdh::append(buffer, __VA_ARGS__))
#define __JDHLOG_APPEND_AS_ARG(buffer, ...)  BOOST_PP_EXPR_IF(MSVC_VARIADIC_SIZE(__VA_ARGS__), jdh::appendAsArg(buffer, __VA_ARGS__))

#define __JDHLOG_IDENT_INC(eLogLevel, pre, args, post) \
	jdh::impl::LogLockGuard lock; \
	jdh::impl::g_LogLevelStack.push(eLogLevel); \
	if (eLogLevel <= jdh::impl::g_LogLevel) { \
		std::string& buffer = jdh::impl::LogFileBuffer(eLogLevel, lock); \
		__JDHLOG_APPEND(buffer, BOOST_PP_REM pre); \
		__JDHLOG_APPEND_AS_ARG(buffer, BOOST_PP_REM args); \
		__JDHLOG_APPEND(buffer, BOOST_PP_REM post); \
		jdh::impl::Log(buffer); \
		jdh::impl::IdentInc(); \
	}
#define __JDHLOG_IDENT_DEC(pre, args, post) \
	jdh::impl::LogLockGuard lock; \
	JDH_ASSERT_NOLOG(!jdh::impl::g_LogLevelStack.empty()); \
	jdh::LOG_LEVEL eLogLevel = jdh::impl::g_LogLevelStack.top(); \
	jdh::impl::g_LogLevelStack.pop(); \
	if (eLogLevel <= jdh::impl::g_LogLevel) { \
		jdh::impl::IdentDec(); \
		std::string& buffer = jdh::impl::LogFileBuffer(eLogLevel, lock); \
		__JDHLOG_APPEND(buffer, BOOST_PP_REM pre); \
		__JDHLOG_APPEND_AS_ARG(buffer, BOOST_PP_REM args); \
		__JDHLOG_APPEND(buffer, BOOST_PP_REM post); \
		jdh::impl::Log(buffer); \
	}
#define __JDHLOG(eLogLevel, pre, args, post) \
	if (eLogLevel <= jdh::impl::g_LogLevel) { \
		jdh::impl::LogLockGuard lock; \
		std::string& buffer = jdh::impl::LogFileBuffer(eLogLevel, lock); \
		__JDHLOG_APPEND(buffer, BOOST_PP_REM pre); \
		__JDHLOG_APPEND_AS_ARG(buffer, BOOST_PP_REM args); \
		__JDHLOG_APPEND(buffer, BOOST_PP_REM post); \
		jdh::impl::Log(buffer); \
	}

#define JDHLOG(eLogLevel, ...)				{__JDHLOG(eLogLevel, (__VA_ARGS__), (), ())}
#define JDHLOG_FORMAT(eLogLevel, fmt, ...)	{__JDHLOG(eLogLevel, (), (jdh::szprintf(fmt, __VA_ARGS__)), ())}
#define JDHLOG_BEGIN(eLogLevel, ...)		{__JDHLOG_IDENT_INC(eLogLevel, (__VA_ARGS__), (), (" {"))}
#define JDHLOG_END(...)						{__JDHLOG_IDENT_DEC(("} // "), (), (__VA_ARGS__))}
#define JDHLOG_RETURN(type, r, ...)			{type rVal = r; {__JDHLOG_IDENT_DEC(("} return ", rVal, "; // "), (), (__VA_ARGS__))} return rVal;}
#define JDHLOG_FUNC(eLogLevel, ...)			{__JDHLOG(eLogLevel, (/*__FILE__ "::"*/ __FUNCTION__ ": "), (), (__VA_ARGS__))}
#define JDHLOG_FUNC_BEGIN(eLogLevel, ...)	{__JDHLOG_IDENT_INC(eLogLevel, (/*__FILE__ "::"*/ __FUNCTION__ "("), (__VA_ARGS__), (") {"))}
#define JDHLOG_FUNC_END(...)				{__JDHLOG_IDENT_DEC(("} // " __FUNCTION__ ": "), (), (__VA_ARGS__))}
#define JDHLOG_FUNC_RETURN(type, r, ...)	{type rVal = r; {__JDHLOG_IDENT_DEC(("} return ", rVal, "; // " __FUNCTION__ ": "), (), (__VA_ARGS__))} return rVal;}


#else

#    define JDHLOG(eLogLevel, ...)
#    define JDHLOG_FORMAT(eLogLevel, fmt, ...)
#    define JDHLOG_BEGIN(eLogLevel, ...)
#    define JDHLOG_END(...)
#    define JDHLOG_FUNC(eLogLevel, ...)
#    define JDHLOG_FUNC_BEGIN(eLogLevel, ...)
#    define JDHLOG_FUNC_END(...)
#    define JDHLOG_RETURN(type, r, ...)

#endif // __JDHLOGFILE



#ifdef MOD_ANALYZE

#  define JDHLOG_ANALYZE(...)				JDHLOG(jdh::ANALYZE, __VA_ARGS__)
#  define JDHLOG_ANALYZE_FORMAT(fmt, ...)	JDHLOG_FORMAT(jdh::ANALYZE, fmt, __VA_ARGS__)
#  define JDHLOG_ANALYZE_BEGIN(...)			JDHLOG_BEGIN(jdh::ANALYZE, __VA_ARGS__)
#  define JDHLOG_ANALYZE_END				JDHLOG_END
#  define JDHLOG_ANALYZE_FUNC(...)			JDHLOG_FUNC(jdh::ANALYZE, __VA_ARGS__)
#  define JDHLOG_ANALYZE_FUNC_BEGIN(...)	JDHLOG_FUNC_BEGIN(jdh::ANALYZE, __VA_ARGS__)
#  define JDHLOG_ANALYZE_FUNC_END			JDHLOG_FUNC_END
#  define JDHLOG_ANALYZE_RETURN				JDHLOG_RETURN

#else

#    define JDHLOG_ANALYZE(eLogLevel, ...)
#    define JDHLOG_ANALYZE_FORMAT(eLogLevel, fmt, ...)
#    define JDHLOG_ANALYZE_BEGIN(eLogLevel, ...)
#    define JDHLOG_ANALYZE_END(...)
#    define JDHLOG_ANALYZE_FUNC(eLogLevel, ...)
#    define JDHLOG_ANALYZE_FUNC_BEGIN(eLogLevel, ...)
#    define JDHLOG_ANALYZE_FUNC_END(...)
#    define JDHLOG_ANALYZE_RETURN(type, r, ...)

#endif

#ifdef MOD_DEBUG

#  define JDHLOG_DEBUG(...)					JDHLOG(jdh::DEBUG, __VA_ARGS__)
#  define JDHLOG_DEBUG_FORMAT(fmt, ...)		JDHLOG_FORMAT(jdh::DEBUG, fmt, __VA_ARGS__)
#  define JDHLOG_DEBUG_BEGIN(...)			JDHLOG_BEGIN(jdh::DEBUG, __VA_ARGS__)
#  define JDHLOG_DEBUG_END					JDHLOG_END
#  define JDHLOG_DEBUG_FUNC(...)			JDHLOG_FUNC(jdh::DEBUG, __VA_ARGS__)
#  define JDHLOG_DEBUG_FUNC_BEGIN(...)		JDHLOG_FUNC_BEGIN(jdh::DEBUG, __VA_ARGS__)
#  define JDHLOG_DEBUG_FUNC_END				JDHLOG_FUNC_END
#  define JDHLOG_DEBUG_RETURN				JDHLOG_RETURN

#else

#  define JDHLOG_DEBUG(eLogLevel, ...)
#  define JDHLOG_DEBUG_FORMAT(eLogLevel, fmt, ...)
#  define JDHLOG_DEBUG_BEGIN(eLogLevel, ...)
#  define JDHLOG_DEBUG_END(...)
#  define JDHLOG_DEBUG_FUNC(eLogLevel, ...)
#  define JDHLOG_DEBUG_FUNC_BEGIN(eLogLevel, ...)
#  define JDHLOG_DEBUG_FUNC_END(...)
#  define JDHLOG_DEBUG_RETURN(type, r, ...)

#endif // MOD_DEBUG
