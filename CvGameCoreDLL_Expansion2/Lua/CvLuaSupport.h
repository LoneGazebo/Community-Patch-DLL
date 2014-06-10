/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaSupport.h
//!  \brief     Public header of the Gamecore Lua framework.
//!
//!		This file includes all initial dependencies to adding Lua support to
//!		Gamecore.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUASUPPORT_H
#define CVLUASUPPORT_H

// Standard Lua includes
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};

// Fireworks Lua utilities
#include <Fireworks/FLua/Include/FLua.h>
#include <Fireworks/FLua/Include/FLuaFStringSupport.h>

// Utilities
#include "CvLuaArgsHandle.h"
class ICvEngineScriptSystem1;
class ICvEngineScriptSystemArgs1;

namespace LuaSupport
{

//!	Called to register all game script data into Lua.
void RegisterScriptData(lua_State* L);
//! Dump the Lua callstack to the output stream
void DumpCallStack(lua_State* L);

bool CallHook(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
bool CallTestAll(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
bool CallTestAny(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
bool CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, int& value);
bool CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, float& value);

}

extern bool luaL_optbool(lua_State* L, int idx, bool bdefault);

#endif //CVLUASUPPORT_H
