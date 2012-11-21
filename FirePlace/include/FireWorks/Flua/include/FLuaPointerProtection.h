//------------------------------------------------------------------------------------------------
//
//  ***************** FIRAXIS GAME ENGINE   ********************
//
//! \file		FLuaPointerProtection.h
//! \author		Eric Jordan -- 4/16/2009
//! \brief		Using pointer protection with types exposed to lua will cause the lua-side pointers
//              to an object to be set to NULL when that object is destroyed.
//
//              To give pointer protection to a type you must:
//              1) Use FLUA_POINTER_PROTECTION_MEMBER() in the class as a public member.
//                 Example:
//                 class MyClass { public: FLUA_POINTER_PROTECTION_MEMBER(); };
//              2) Use FLUA_TYPE_USES_POINTER_PROTECTION(TYPE) outside of the class in the header file.
//                 Example:
//                 FLUA_TYPE_USES_POINTER_PROTECTION(MyClass)
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef FLuaPointerProtection_h
#define FLuaPointerProtection_h
#pragma once

#include "FLuaCommon.h"
#include "FLuaTypes.h"

// FLUA_POINTER_PROTECTION_MEMBER()
// Adds a FLuaPointerProtection member to a class.  Must have public visibility.
#define FLUA_POINTER_PROTECTION_MEMBER() FLua::Details::PointerProtection FLuaPointerProtection

// FLUA_TYPE_USES_POINTER_PROTECTION(TYPE)
// Forces FLua to use the CData from the FLuaPointerProtection member of an object.  Use this in an h file.
#define FLUA_TYPE_USES_POINTER_PROTECTION(TYPE) \
	namespace FLua{ namespace Details{ \
	static void PushCData(lua_State *L, TYPE *pVal) { \
		if( pVal ) pVal->FLuaPointerProtection.PushCData(L, (void*)pVal); \
		else { \
			lua_pushlightuserdata(L, pVal); \
		}\
	} \
	}}

namespace FLua
{
	namespace Details
	{
		class PointerProtection
		{
		public:
			PointerProtection();
			~PointerProtection();

			// Don't ever call this outside of FLua!!!
			void PushCData(lua_State *L, void *pVal);

		private:
			struct UserDataNode
			{
				UserDataNode() : m_pkNext(NULL) {}
				Value m_kUD;
				UserDataNode *m_pkNext;
			};

			UserDataNode m_kUserData;
		};
	}
}

#endif
