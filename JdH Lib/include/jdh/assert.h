#pragma once

#include <FireWorks/FAssert.h>
#include <CvAssert.h>

#include "pp.h"
#include "tostring.h"

#ifdef JDH_ASSERT_ENABLE
#pragma message("JDH_ASSERT: enabled")
#	define JDH_Assert(expr, ...) { MSVC_VARIADIC_SWITCH(__JDH_Assert_, __VA_ARGS__)(expr, __VA_ARGS__) __analysis_assume((bool)(expr)); }
#	define __JDH_Assert_0(expr, ...) \
		static bool bIgnoreAlways = false; \
		if (!(expr)) { \
			if (jdh::impl::AssertDlg(#expr, __FILE__, __LINE__, bIgnoreAlways, NULL)) \
			{ __debugbreak(); } \
		}
#	define __JDH_Assert_N(expr, ...) \
		static bool bIgnoreAlways = false; \
		if (!(expr)) { \
			std::string& buffer = jdh::impl::GetTempString(); \
			jdh::append(buffer, __VA_ARGS__); \
			if (jdh::impl::AssertDlg(#expr, __FILE__, __LINE__, bIgnoreAlways, buffer.c_str())) \
			{ __debugbreak(); } \
		}
#	define JDH_AssertFmt(expr, fmt, ...) \
		{ \
			static bool bIgnoreAlways = false; \
			if (!(expr)) { \
				if (jdh::impl::AssertDlg(#expr, __FILE__, __LINE__, bIgnoreAlways, jdh::szprintf(fmt, __VA_ARGS__))) \
				{ __debugbreak(); } \
			} \
			__analysis_assume((bool)(expr)); \
		}
#	define JDH_Verify(expr, ...) JDH_Assert(expr, __VA_ARGS__)
#   define JDH_ASSERT_NOLOG(expr, ...) { ::jdh::impl::g_bIsAssertLogDisabled = true; JDH_Assert(expr, __VA_ARGS__);  ::jdh::impl::g_bIsAssertLogDisabled = false; }
#	ifdef FASSERT_ENABLE
#		undef FAssert
#		undef FAssertMsg
#		undef FAssertMsg0
#		undef FAssertMsg1
#		undef FAssertMsg2
#		undef FAssertMsg3
#		undef FAssertMsg4
#		undef FAssertMsg6
#		define FAssert( expr )											JDH_Assert(expr)
#		define FAssertMsg( expr, msg )									JDH_Assert(expr, msg)
#		define FAssertMsg0( expr, msg )									JDH_Assert(expr, msg)
#		define FAssertMsg1( expr, _fmt, _p0 )							JDH_AssertFmt(expr, _fmt, _p0)
#		define FAssertMsg2( expr, _fmt, _p0, _p1 )						JDH_AssertFmt(expr, _fmt, _p0, _p1)
#		define FAssertMsg3( expr, _fmt, _p0, _p1, _p2 )					JDH_AssertFmt(expr, _fmt, _p0, _p1, _p2)
#		define FAssertMsg4( expr, _fmt, _p0, _p1, _p2, _p3 )			JDH_AssertFmt(expr, _fmt, _p0, _p1, _p2, _p3)
#		define FAssertMsg6( expr, _fmt, _p0, _p1, _p2, _p3, _p4, _p5 )	JDH_AssertFmt(expr, _fmt, _p0, _p1, _p2, _p3, _p4, _p5)
#	endif // FASSERT_ENABLE
#	ifdef CVASSERT_ENABLE
#		undef CvAssertFmt
#		undef CvAssertMsg
#		undef CvAssert
#		undef CvAssert_Debug
#		undef CvAssertMsg_Debug
#		define CvAssertFmt(expr, fmt, ...)		JDH_AssertFmt( expr, fmt, __VA_ARGS__ )
#		define CvAssertMsg(expr, msg)			JDH_Assert(expr, msg)
#		define CvAssert(expr)					JDH_Assert(expr)
#		define CvAssert_Debug(expr)				JDH_AssertDebug(expr)
#		define CvAssertMsg_Debug(expr, msg)		JDH_AssertDebug(expr, msg)
#	endif // CVASSERT_ENABLE
#else // JDH_ASSERT_ENABLE
#	pragma message("JDH_ASSERT: disabled.")
#	define JDH_Assert(expr, ...)			(__noop)
#	define JDH_AssertFmt(expr, fmt, ...)	(__noop)
#	define JDH_Verify(expr, ...) (expr)		(__noop)
#	define JDH_ASSERT_NOLOG(expr, ...)		(__noop)
#endif // JDH_ASSERT_ENABLE

#ifdef CVASSERT_ENABLE
#	pragma message("  CVASSERT: enabled")
#else
#	pragma message("  CVASSERT: disabled")
#endif
#ifdef FASSERT_ENABLE
#	pragma message("   FASSERT: enabled")
#else
#	pragma message("   FASSERT: disabled")
#endif

#ifdef FINAL_RELEASE
#	define JDH_AssertDebug(expr, ...)	(__noop)
#else
#	define JDH_AssertDebug(expr, ...)	JDH_Assert(expr, __VA_ARGS__)
#endif // FINAL_RELEASE

namespace jdh
{
	namespace impl
	{
		extern HMODULE	g_hGDLL /*= NULL*/;

		// Runtime control to disable assert dlg
		extern bool		g_bIsAssertDisabled /*= false*/;
		extern bool		g_bIsAssertLogDisabled /*= false*/;

		bool AssertDlg(_In_ PCSTR szExpr, _In_ PCSTR szFile, unsigned int line, _Inout_ bool& bIgnoreAlways, _In_opt_ PCSTR szMsg);
	}
}
