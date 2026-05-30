/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaSupport.cpp
//!  \brief     Private implementation of the Gamecore Lua framework.
//!
//!		This file includes methods for registering game data w/ Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaEnums.h"
#include "CvLuaFractal.h"
#include "CvLuaGameInfo.h"
#include "CvLuaMap.h"
#include "CvLuaGame.h"
#include "CvLuaPlayer.h"
#include "CvLuaTeam.h"
#include "CvLuaCity.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#include "CvLuaTeamTech.h"
#include "CvLuaDeal.h"
#include "CvLuaArea.h"
#include "CvLuaLeague.h"

#pragma warning(disable:4800 ) //forcing value to bool 'true' or 'false'

//------------------------------------------------------------------------------
// Utility methods
//------------------------------------------------------------------------------
bool luaL_optbool(lua_State* L, int idx, bool bdefault)
{
	if(lua_isnoneornil(L, idx))
	{
		return bdefault;
	}
	else
	{
		return (bool)lua_toboolean(L, idx);
	}
}

//------------------------------------------------------------------------------
void LuaSupport::RegisterScriptData(lua_State* L)
{
	//Register some plain datatables
	CvLuaEnums::Register(L);
	CvLuaGameInfo::Register(L);

	//Register static interfaces
	CvLuaFractal::Register(L);
	CvLuaMap::Register(L);
	CvLuaGame::Register(L);

	//Register Players and Teams - those instances already exist
	CvLuaPlayer::Register(L);
	CvLuaTeam::Register(L);

	//Compat for older mods:
	//Additionally put Player and Team directly into the gamecore table again
	lua_getglobal(L,"LuaTypes");
	lua_getfield(L,-1,"Team");
	lua_setglobal(L,"Team");
	lua_getfield(L,-1,"Player");
	lua_setglobal(L,"Player");
	lua_pop(L,1);

	//Register additional lua types, for which instances will only be created on demand
	CvLuaCity::PushTypeTable(L);
	CvLuaPlot::PushTypeTable(L);
	CvLuaUnit::PushTypeTable(L);
	CvLuaTeamTech::PushTypeTable(L);
	CvLuaDeal::PushTypeTable(L);
	CvLuaArea::PushTypeTable(L);
	CvLuaLeague::PushTypeTable(L);
}

void LuaSupport::InitLuaFramework()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	lua_State* L = pkScriptSystem->CreateLuaThread("VP_LUAAPI");

	lua_pushvalue(L,LUA_REGISTRYINDEX);
	lua_setglobal(L,"REGISTRY");

	const char* luaCommand = ""

"local G = REGISTRY._LOADED._G ;\n"
"local Threads = G.Threads ;\n"
"local IncludeFileList = G.IncludeFileList ;\n"
"local string_find = string.find ;\n"
"local debug_getinfo = debug.getinfo ;\n"
"\n"
"local assert = assert ;\n"
"local coroutine = coroutine ;\n"
"local error = error ;\n"
"local getfenv = G.getfenv ;\n"
"local ipairs = ipairs ;\n"
"local pairs = pairs ;\n"
"local pcall = pcall ;\n"
"local print = print ;\n"
"local rawget = G.rawget ;\n"
"local rawset = G.rawset ;\n"
"local select = select ;\n"
"local setfenv = G.setfenv ;\n"
"local setmetatable = setmetatable ;\n"
"local type = type ;\n"
"local _ENV = getfenv(0) ;\n"
"\n"
"Threads[coroutine.running()] = {} ; -- Prevent my global env from being cleared out by the engine!\n"
"\n"
"local function sandboxedCall( func, ... ) setfenv( 0, getfenv(0), setfenv( 0, getfenv(func) ), pcall( func, ... ) ) end;\n"
"local newThreadCallback = nil ;\n"
"local function setNewThreadCallback( func ) newThreadCallback = ( type(func) == 'function' and func ) or error('Not a function!') end;\n"
"local function setfenv2( func, env, ... ) local ret = getfenv(func) ; setfenv(func, env) ; return ret, ... end;\n"
"local function callWithEnv( func, env, ... )\n"
"	assert( type(func) == 'function', 'First argument to callWithEnv must be a function!' ) ;\n"
"	assert( type(env) == 'table', 'Second argument to callWithEnv must be a table!' ) ;\n"
"	return select( 2, assert( select( 2, setfenv2( func, setfenv2( func, env ), pcall(func, ...) ) ) ) ) ;\n"
"end;\n"
"local function getSource( func )\n"
"	assert( type(func) == 'function', 'Argument must be a function!') ;\n"
"	return debug_getinfo(func).source ;\n"
"end ;\n"
"\n"
"local function getThreadEnvByName( name )\n"
"	for k,v in pairs(Threads) do\n"
"		if v.StateName == name then\n"
"			return v ;\n"
"		end\n"
"	end\n"
"end;\n"
"\n"
"local doesIncludeExist = function( file )\n"
"	local idx = 0-#file ;\n"
"	for _,val in ipairs(IncludeFileList) do\n"
"		if string_find(val,file,idx) then\n"
"			return val ;\n"
"		end\n"
"	end\n"
"	return false ;\n"
"end;\n"
"\n"
"local function copyTableEntries( to, from )\n"
"	for k,v in pairs(from) do \n"
"		to[k] = v ;\n"
"	end\n"
"end;\n"
"\n"
"local extraApi = {\n"
"	setNewThreadCallback = setNewThreadCallback ;\n"
"	rawset = rawset ;\n"
"	rawget = rawget ;\n"
"	newproxy = G.newproxy ;\n"
"	settenv = function( env,... ) local ret = getfenv(0); setfenv(0, env); return ret,... end ;\n"
"	gettenv = function() return getfenv(0) end ;\n"
"	doesIncludeExist = doesIncludeExist ;\n"
"	callWithEnv = callWithEnv ;\n"
"	getSource = getSource ;\n"
"	firaxisGameInfo = G.GameInfo ;\n"
"};\n"
"\n"
"if G.jit then\n"
"	extraApi.jit = true ;\n"
"	extraApi.bit = G.require('bit');\n"
"end\n"
"\n"
"local baseApi = nil ;\n"
"local loaderCoroutine = coroutine.create( function( env )\n"
"	while true do\n"
"		print('################################################################################');\n"
"		print('(Re-)Loading api context!');\n"
"		setfenv(0,env);\n"
"		print('<-Switching context! ContextPtr:', env.ContextPtr);\n"
"		if env.VPUI_loader then\n"
"			print('Releasing previous api context! ContextPtr:', env.VPUI_loader);\n"
"			env.ContextPtr:ReleaseChild(env.VPUI_loader);\n"
"		end\n"
"		local status, result = pcall( env.ContextPtr.LoadNewContext, env.ContextPtr, 'VPUI_loader' );\n"
"		setfenv(0,_ENV);\n"
"		if status then\n"
"			print('New api context created! ContextPtr:' , result or 'NIL?');\n"
"		else\n"
"			print('Failed to create API context! Error-message:');\n"
"			print(result);\n"
"			result = false ;\n"
"		end\n"
"		env.VPUI_loader = result ;\n"
"		print('################################################################################');\n"
"		env = coroutine.yield();\n"
"	end\n"
"end);\n"
"\n"
"local function getBaseApi( reload ) \n"
"	if reload or (not baseApi) then\n"
"		newThreadCallback = nil ;\n"
"		local ttenv = getThreadEnvByName('ToolTips') ;\n"
"		baseApi = { quicktraceback = ttenv.quicktraceback } ;\n"
"		coroutine.resume( loaderCoroutine , ttenv );\n"
"	end\n"
"	return baseApi;\n"
"end;\n"
"\n"
"local GAMECORE_EXPOSED_TO_API = false ;\n"
"local GAMECORE_DELETE_NEXT_CHECK = false ;\n"
"local function validateGamecoreAndApiStatus( delete_next_time )\n"
"	if GAMECORE_DELETE_NEXT_CHECK then\n"
"		G.GameCore = nil ;\n"
"		baseApi = nil ;\n"
"		newThreadCallback = nil ;\n"
"	elseif not GAMECORE_EXPOSED_TO_API then\n"
"		baseApi = nil ;\n"
"		newThreadCallback = nil ;\n"
"	end\n"
"	GAMECORE_EXPOSED_TO_API = true ;\n"
"	GAMECORE_DELETE_NEXT_CHECK = delete_next_time ;\n"
"end;\n"
"\n"
"local function reset()\n"
"	baseApi = nil ;\n"
"	newThreadCallback = nil ;\n"
"	if GAMECORE_EXPOSED_TO_API then\n"
"		G.GameCore = nil ;\n"
"		GAMECORE_EXPOSED_TO_API = false ;\n"
"		GAMECORE_DELETE_NEXT_CHECK = false ;\n"
"	end\n"
"end;\n"
"\n"
"local function pushApi( to )\n"
"	copyTableEntries( to, getBaseApi() );\n"
"	to._ENV = to ;\n"
"	if newThreadCallback then sandboxedCall( newThreadCallback, to ) end;\n"
"end;\n"
"\n"
"local IMPORT_BLACKLIST = {\n"
"	StateName = true ;\n"
"	Events = true ;\n"
"	LuaEvents = true ;\n"
"	GameEvents = true ;\n"
"	Controls = true ;\n"
"	ContextPtr = true ;\n"
"};\n"
"\n"
"local registrationListenerVPUILoader = { __newindex = function( to, key, value )\n"
"	if key == 'PreGame' then\n"
"		setmetatable(to,nil);\n"
"		copyTableEntries(to, extraApi);\n"
"		to.exportedObjects = baseApi ;\n"
"		to.GameCore = G.GameCore ;\n"
"		to._ENV = to ;\n"
"	end\n"
"	rawset(to, key, value);\n"
"	if not IMPORT_BLACKLIST[key] then \n"
"		baseApi[key] = value;\n"
"	end\n"
"end;};\n"
"\n"
"local registrationListenerFinishBeforeControls = { __newindex = function( to, key, value )\n"
"	if key == 'Controls' and not(G.GameCore and G.GameCore.Controls == value) then\n"
"		setmetatable(to,nil);\n"
"		pushApi(to);\n"
"	end\n"
"	rawset(to, key, value);\n"
"end;};\n"
"\n"
"local registrationListenerWaitForPreGame = { __newindex = function( to, key, value )\n"
"	if key == 'PreGame' then\n"
"		setmetatable(to, registrationListenerFinishBeforeControls);\n"
"	end\n"
"end;};\n"
"\n"
"local registrationListenerFinishAfterArtInfo = { __newindex = function( to, key, value )\n"
"	if key == 'ArtInfo' and not(G.GameCore and G.GameCore.ArtInfo == value) then\n"
"		setmetatable(to,nil);\n"
"		pushApi(to);\n"
"	else\n"
"		rawset(to, key, value);\n"
"	end\n"
"end;};\n"
"\n"
"local registrationListenerPotentiallyMatchmakingLib = { __newindex = function( to, key, value )\n"
"	if key == 'Matchmaking' and not(G.GameCore and G.GameCore.Matchmaking == value) then\n"
"		setmetatable(to, registrationListenerWaitForPreGame);\n"
"	else\n"
"		setmetatable(to, registrationListenerFinishAfterArtInfo);\n"
"	end\n"
"end;};\n"
"\n"
"local registrationListenerWaitForEvents = { __newindex = function( to, key, value )\n"
"	if key == 'LuaEvents' or key == 'Events' then\n"
"		rawset(to, key, value);\n"
"	elseif key == 'UI' then\n"
"		setmetatable(to, registrationListenerPotentiallyMatchmakingLib);\n"
"	end\n"
"end;};\n"
"\n"
"local registrationListenerCheckoutType = { __newindex = function( to, key, value )\n"
"	local stn = to.StateName ;\n"
"	setmetatable(to,nil);\n"
"	if stn == nil then -- ParseMapScript or GameCore creation - Have yet to figure out how to handle them efficiently\n"
"		print('VP_LUAAPI: Ignoring exe thread!');\n"
"		rawset(to, key, value);\n"
"	elseif stn == 'VP_LUAAPI' then\n"
"		print('VP_LUAAPI: Ignoring another me?');\n"
"		reset();\n"
"		rawset(to,key,value);\n"
"	elseif stn == 'ToolTips' then\n"
"		print('VP_LUAAPI: Ignoring ToolTips context!');\n"
"		reset();\n"
"		rawset(to, key, value);\n"
"	elseif stn == 'VPUI_loader' then\n"
"		setmetatable(to, registrationListenerVPUILoader);\n"
"		to[key]=value; --don't use rawset here!\n"
"	else\n"
"		if stn == 'Map Script' or stn == 'InGame' then\n"
"			validateGamecoreAndApiStatus(true);\n"
"		elseif stn == 'LoadScreen' or stn == 'WaitingForPlayers' then\n"
"			validateGamecoreAndApiStatus(false);\n"
"		elseif stn == 'FrontEnd' then\n"
"			reset();\n"
"		end\n"
"		setmetatable(to, registrationListenerWaitForEvents);\n"
"	end\n"
"end;};\n"
"\n"
"if getmetatable(Threads) then\n"
"	print('Warning: Metatable for Threads already present!');\n"
"end;\n"
"\n"
"setmetatable(Threads, { __newindex = function( to, thread, env )\n"
"	rawset(to, thread, env);\n"
"	if doesIncludeExist('VPUI_core.lua') then\n"
"		setmetatable(env, registrationListenerCheckoutType);\n"
"	else\n"
"		print('VP_LUAAPI: Shutting down!');\n"
"		setmetatable(to, nil);\n"
"	end\n"
"end;});\n"
"\n"
"print('Embedded script ran without errors!');\n";

	luaL_dostring(L,luaCommand);
	
	pkScriptSystem->FreeLuaThread(L);
}

//------------------------------------------------------------------------------
void LuaSupport::DumpCallStack(lua_State* L, FILogFile* pLog)
{
	CvString szTemp;
    lua_Debug entry;
    int depth = 0; 

	while (lua_getstack(L, depth, &entry))
	{
		szTemp.Format("%s (%d): %s\n", entry.source ? entry.source : "?", entry.currentline, entry.name ? entry.name : "?");
		depth++;

		if (pLog)
			pLog->Msg(szTemp.c_str());
		else
			OutputDebugString(szTemp.c_str());
	}
}

//------------------------------------------------------------------------------
bool LuaSupport::CallHook(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallHook(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallTestAll(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallTestAll(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallTestAny(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallTestAny(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, int& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallAccumulator(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, float& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallAccumulator(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
