/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#ifndef CVLUASUPPORT_H
#define CVLUASUPPORT_H

#include "CvLuaArgsHandle.h"

typedef struct lua_State lua_State;
typedef int(*lua_CFunction)(lua_State*);
typedef void(*lua_CVoidFunc)(lua_State*);

namespace LuaSupport {
	
	void NotifyDatabaseCached();
	void NotifyDllUnload();
	
	// Can be used to invoke lua functions without actually including lua headers
	void LuaPCall(lua_State* L,lua_CFunction f);
	void LuaPCall(lua_State* L,lua_CVoidFunc f);

	// Called to register all game script data into Lua.
	void RegisterScriptData(lua_State* L);
	
	// Equivalent to what include(script) does in lua
	void RunScript(lua_State* L, const char* script);

	// Fire game events
	bool CallHook(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
	bool CallTestAll(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
	bool CallTestAny(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value);
	bool CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, int& value);
	bool CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, float& value);
}

#endif //CVLUASUPPORT_H
