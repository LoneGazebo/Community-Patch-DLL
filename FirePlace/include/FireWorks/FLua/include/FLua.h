//------------------------------------------------------------------------------------------------
//
//  ***************** FIRAXIS GAME ENGINE   ********************
//
//! \file		FLua.h
//! \author		Eric Jordan -- 3/12/2009
//! \brief		Includes core FLua headers
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef FLua_h
#define FLua_h
#pragma once

#include "FLuaTypes.h"
#include "FLuaGlobalData.h"
#include "FLuaStaticFunctions.h"
#include "FLuaTypeExposure.h"
#include "FLuaPointerProtection.h"

namespace FLua
{
	// RegisterAll - Register all exposed statics to lua
	inline void RegisterAll(lua_State *L)
	{
		GlobalData::Register(L);
		StaticFunctions::Register(L);
		TypeExposures::Register(L, TypeExposures::sm_uiUnrestrictedExposure);
	}

	// RegisterAll - Register all exposed statics to lua
	inline void RegisterAll(lua_State *L, unsigned int uiCategoryFlags)
	{
		GlobalData::Register(L);
		StaticFunctions::Register(L, uiCategoryFlags);
		TypeExposures::Register(L, uiCategoryFlags);
	}

	// tointeger with a nul check.
	inline lua_Integer tointeger(lua_State* L, int iIndex) 
	{ 
		FAssert(!lua_isnil(L, iIndex)); 
		return lua_tointeger(L, iIndex);
	}

	// tonumber with a nul check.
	inline lua_Number tonumber(lua_State* L, int iIndex) 
	{ 
		FAssert(!lua_isnil(L, iIndex)); 
		return lua_tonumber(L, iIndex);
	}

	// toboolean with a nul check.
	inline int toboolean(lua_State* L, int iIndex) 
	{ 
		FAssert(!lua_isnil(L, iIndex)); 
		return lua_toboolean(L, iIndex);
	}

}

#endif
