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

	//Register additional lua types, for which instances will only be created on demand
	CvLuaCity::PushTypeTable(L);
	CvLuaPlot::PushTypeTable(L);
	CvLuaUnit::PushTypeTable(L);
	CvLuaTeamTech::PushTypeTable(L);

	//-----------------------------------------
	// Restore access to some core lua functionality hidden away by Fireaxis, but still keep the sandbox intact
	// Use lua_dostring(), because it's more comfortable to code and easier to see what's happening

	lua_pushvalue(L, LUA_REGISTRYINDEX);
	lua_setglobal(L, "R");

	const char* luacommand = "" 

	"local G = R._LOADED._G;"
	"R = nil;"

	"rawset = G.rawset;"
	"rawget = G.rawget;"
	"newproxy = G.newproxy;"
	"if G.jit then LUAJIT = true; bitops = G.require('bit') end;"

	"FireaxisObjects={"
		"UI = G.UI;"
		"Threads = G.Threads;"
		"GameInfo = G.GameInfo;"
		"GameDefines = G.GameDefines;"
	"};"

	"local g = G.getfenv;"
	"GameCore = g(0);"
	"_ENV = G.setmetatable( {}, {"
		"__index = function(T,k) return g(0)[k] end;"
		"__newindex = function(T,k,v) g(0)[k] = v end;"
		"__call = function() return g(0) end;"
	"});";
	
	luaL_dostring(L,luacommand);

	//------------------------------------------

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
