//------------------------------------------------------------------------------------------------
//
//  ***************** FIRAXIS GAME ENGINE   ********************
//
//! \file		FLuaStaticFunctions.h
//! \author		Eric Jordan -- 3/09/2009
//! \brief		Use to automatically expose static C++ functions to Lua.  All manipulations of the
//              lua stack are handled internally in FLua.
//
//              Example:
//              int DoSomething(const char *szArg0, uint uiArg1) {...}
//              FLUA_EXPOSE(DoSomething); // DoSomething may now be called from Lua
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef FLuaStaticFunctions_h
#define FLuaStaticFunctions_h
#pragma once

#include "FLuaCommon.h"

// FLUA_EXPOSE_REX(FUNC, NAME, CATEGORY_FLAGS)
// Exposes FUNC to lua under with NAME as the function name in lua.
// Restrict the exposure to only states that fall into one of the provided category flags
#define FLUA_EXPOSE_REX(FUNC, NAME, CATEGORY_FLAGS) \
	int LuaThunk_##NAME##(lua_State *L) { return FLua::Details::Call(&FUNC, L, #NAME); } \
	int LuaToString_##NAME##(lua_State *L) { \
		char szDescription[256]; \
		FLua::Details::DescribeFunc(&FUNC, #NAME, szDescription, 256); \
		lua_pushstring(L, szDescription); \
		return 1; \
	} \
	static FLua::StaticFunctions::Registrar LuaReg_##NAME##(#NAME, &LuaThunk_##NAME##, &LuaToString_##NAME##, CATEGORY_FLAGS)

// FLUA_EXPOSE_EX(FUNC, NAME)
// Exposes FUNC to lua under with NAME as the function name in lua
#define FLUA_EXPOSE_EX(FUNC, NAME) FLUA_EXPOSE_REX(FUNC, NAME, FLua::StaticFunctions::sm_uiUnrestrictedExposure)

// FLUA_EXPOSE_R(FUNC, CATEGORY_FLAGS)
// Exposes FUNC to lua with FUNC as the function name in lua
// Restrict the exposure to only states that fall into one of the provided category flags
#define FLUA_EXPOSE_R(FUNC, CATEGORY_FLAGS) FLUA_EXPOSE_REX(FUNC, FUNC, CATEGORY_FLAGS)

// FLUA_EXPOSE(FUNC)
// Exposes FUNC to lua with FUNC as the function name in lua
#define FLUA_EXPOSE(FUNC) FLUA_EXPOSE_EX(FUNC, FUNC)

// FLUA_FUNC(RET, NAME, ARGLIST)
// Creates a static function and exposes it to lua.
// RET is the return value.  NAME is the function name. ARGLIST is the argument list.
// Example: FLUA_FUNC(void, MyFunc, (const bool bArg))
#define FLUA_FUNC(RET, NAME, ARGLIST) \
	RET NAME##ARGLIST##; \
	FLUA_EXPOSE(NAME); \
	RET NAME##ARGLIST

namespace FLua
{
	// StaticFunctions - Manager for static functions exposed to lua
	class StaticFunctions
	{
	public:
		static const unsigned int sm_uiMaxFunctions = 1024;
		static const unsigned int sm_uiUnrestrictedExposure = (unsigned int)-1; // Category flags for unrestricted exposure
		typedef void RegistrationFunc(lua_State *L);

		struct Registrar {
			inline Registrar( _In_z_ const char *szName, lua_CFunction pfnThunk, lua_CFunction pfnToString, unsigned int uiCategoryFlags) {
				StaticFunctions &kRef = Ref();

				// If there are no free entries for functions then we're in trouble
				assert(kRef.m_uiFuncEntryCount < sm_uiMaxFunctions && "Not enough space for exposing functions");

				// Add the entry
				if( kRef.m_uiFuncEntryCount < sm_uiMaxFunctions )
				{
					kRef.m_aFuncEntries[kRef.m_uiFuncEntryCount++].Set(szName, pfnThunk, pfnToString, uiCategoryFlags);
				}
			}
		};

		struct CustomRegistrar {
			inline CustomRegistrar(RegistrationFunc *pfn) {
				StaticFunctions &kRef = Ref();

				// If there are no free entries for functions then we're in trouble
				assert(kRef.m_uiCustomRegistrationCount < sm_uiMaxFunctions && "Not enough space for exposing functions");

				// Add the entry
				if( kRef.m_uiCustomRegistrationCount < sm_uiMaxFunctions )
				{
					kRef.m_afnCustomRegistrationFunctions[kRef.m_uiCustomRegistrationCount++] = pfn;
				}
			}
		};

		// Register static functions with the given lua state
		static void Register(lua_State *L);
		static void Register(lua_State *L, unsigned int uiCategoryFlags);

	private:
		struct FuncEntry
		{
			inline void Set(const char *szName, lua_CFunction pfnThunk, lua_CFunction pfnToString, unsigned int uiCategoryFlags) {
				m_szName = szName; m_pfnThunk = pfnThunk; m_pfnToString = pfnToString; m_uiCategoryFlags = uiCategoryFlags;
			}

			const char *m_szName;
			unsigned int m_uiCategoryFlags;
			lua_CFunction m_pfnThunk;
			lua_CFunction m_pfnToString;
		};

		StaticFunctions() : m_uiFuncEntryCount(0), m_uiCustomRegistrationCount(0) {}
		static inline StaticFunctions &Ref() { static StaticFunctions kInst; return kInst; }

		FuncEntry m_aFuncEntries[sm_uiMaxFunctions];
		size_t m_uiFuncEntryCount;

		RegistrationFunc *m_afnCustomRegistrationFunctions[sm_uiMaxFunctions];
		size_t m_uiCustomRegistrationCount;
	};

	namespace Details
	{
		// Default static functions:

		// TypeInfo - Takes one argument.  Returns a string describing the type of that argument.
		//   Only meant to be called from within lua!
		int TypeInfo(lua_State *L);

		// IsNull - Takes one argument.  Returns true if argument has a CData field with NULL in it.
		//   Only meant to be called from within lua!.
		int IsNull(lua_State *L);


		// Call specializations for static functions

		// 0 args with no return value
		static int Call(void(*pfn)(), lua_State *L, const TCHAR *szLuaFnName) {
			if( lua_gettop(L) == 0 ) (*pfn)();
			else Error(_T("%s could not be called: takes no arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 0 args with return value
		template<class TRet>
		static int Call(TRet(*pfn)(), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			if( lua_gettop(L) == 0 ) {
				TRet ret = (*pfn)();
				RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
				Push( L, pushMe );
				return 1;
			}
			else Error(_T("%s could not be called: takes no arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 1 arg with no return value
		template<class TArg0>
		static int Call(void(*pfn)(TArg0), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 1 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				if( bValidArgs )
					(*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 1 argument but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 1 arg with return value
		template<class TRet, class TArg0>
		static int Call(TRet(*pfn)(TArg0), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 1 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				if( bValidArgs ) {
					TRet ret = (*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 1 argument but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 2 args with no return value
		template<class TArg0, class TArg1>
		static int Call(void(*pfn)(TArg0, TArg1), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 2 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				if( bValidArgs )
					(*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 2 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 2 args with return value
		template<class TRet, class TArg0, class TArg1>
		static int Call(TRet(*pfn)(TArg0, TArg1), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 2 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				if( bValidArgs ) {
					TRet ret = (*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 2 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 3 args with no return value
		template<class TArg0, class TArg1, class TArg2>
		static int Call(void(*pfn)(TArg0, TArg1, TArg2), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 3 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				Analog2 arg2 = Get<Analog2>(L, 3); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
				if( bValidArgs ) {
					(*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
					       FromLuaAnalog<TArg1>::Convert(arg1),
						   FromLuaAnalog<TArg2>::Convert(arg2));
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 3 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 3 args with return value
		template<class TRet, class TArg0, class TArg1, class TArg2>
		static int Call(TRet(*pfn)(TArg0, TArg1, TArg2), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 3 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				Analog2 arg2 = Get<Analog2>(L, 3); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
				if( bValidArgs ) {
					TRet ret = (*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
					                  FromLuaAnalog<TArg1>::Convert(arg1),
									  FromLuaAnalog<TArg2>::Convert(arg2));
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 3 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 4 args with no return value
		template<class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(void(*pfn)(TArg0, TArg1, TArg2, TArg3), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 4 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				Analog2 arg2 = Get<Analog2>(L, 3); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
				Analog3 arg3 = Get<Analog3>(L, 4); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
				if( bValidArgs ) {
					(*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
					       FromLuaAnalog<TArg1>::Convert(arg1),
					       FromLuaAnalog<TArg2>::Convert(arg2),
						   FromLuaAnalog<TArg3>::Convert(arg3));
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 4 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 4 args with return value
		template<class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(TRet(*pfn)(TArg0, TArg1, TArg2, TArg3), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 4 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				Analog2 arg2 = Get<Analog2>(L, 3); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
				Analog3 arg3 = Get<Analog3>(L, 4); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
				if( bValidArgs ) {
					TRet ret = (*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
					                  FromLuaAnalog<TArg1>::Convert(arg1),
					                  FromLuaAnalog<TArg2>::Convert(arg2),
					                  FromLuaAnalog<TArg3>::Convert(arg3));
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 4 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 5 args with no return value
		template<class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(void(*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 5 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				Analog2 arg2 = Get<Analog2>(L, 3); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
				Analog3 arg3 = Get<Analog3>(L, 4); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
				Analog4 arg4 = Get<Analog4>(L, 5); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
				if( bValidArgs ) {
					(*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
					       FromLuaAnalog<TArg1>::Convert(arg1),
					       FromLuaAnalog<TArg2>::Convert(arg2),
					       FromLuaAnalog<TArg3>::Convert(arg3),
						   FromLuaAnalog<TArg4>::Convert(arg4));
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 5 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// 5 args with return value
		template<class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(TRet(*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 5 ) {
				bool bValidArgs = true;
				Analog0 arg0 = Get<Analog0>(L, 1); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
				Analog1 arg1 = Get<Analog1>(L, 2); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
				Analog2 arg2 = Get<Analog2>(L, 3); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
				Analog3 arg3 = Get<Analog3>(L, 4); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
				Analog4 arg4 = Get<Analog4>(L, 5); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
				if( bValidArgs ) {
					TRet ret = (*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
					                  FromLuaAnalog<TArg1>::Convert(arg1),
					                  FromLuaAnalog<TArg2>::Convert(arg2),
					                  FromLuaAnalog<TArg3>::Convert(arg3),
									  FromLuaAnalog<TArg4>::Convert(arg4));
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
			}
			else Error(_T("%s could not be called: requires 5 arguments but received %i."), szLuaFnName, lua_gettop(L));
			return 0;
		}

		// DescribeFunc specializations for static functions

		// 0 args
		template<class TRet>
		static void DescribeFunc(TRet(pfn)(), const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( void )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName);
		}

		// 1 arg
		template<class TRet, class TArg0>
		static void DescribeFunc(TRet(pfn)(TArg0), const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get());
		}

		// 2 args
		template<class TRet, class TArg0, class TArg1>
		static void DescribeFunc(TRet(pfn)(TArg0, TArg1), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get());
		}

		// 3 args
		template<class TRet, class TArg0, class TArg1, class TArg2>
		static void DescribeFunc(TRet(pfn)(TArg0, TArg1, TArg2), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get());
		}

		// 4 args
		template<class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		static void DescribeFunc(TRet(pfn)(TArg0, TArg1, TArg2, TArg3), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get());
		}

		// 5 args
		template<class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static void DescribeFunc(TRet(pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get(),
				DescribeType<TArg4>(), PtrDescription<TArg4>::Get());
		}
	}
}

#endif
