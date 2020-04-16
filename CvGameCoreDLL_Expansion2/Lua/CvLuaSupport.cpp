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
	CvLuaEnums::Register(L);
	CvLuaFractal::Register(L);
	CvLuaGameInfo::Register(L);
	CvLuaMap::Register(L);
	CvLuaGame::Register(L);
	CvLuaPlayer::Register(L);
	CvLuaTeam::Register(L);
}

//------------------------------------------------------------------------------
void LuaSupport::DumpCallStack(lua_State* L, FILogFile* pLog)
{
	FStringFixedBuffer(szTemp, 512);
    lua_Debug entry;
    int depth = 0; 

	while (lua_getstack(L, depth, &entry))
	{
		int status = lua_getinfo(L, "Sln", &entry);
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
	if (MOD_CORE_DISABLE_LUA_HOOKS)
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
	if (MOD_CORE_DISABLE_LUA_HOOKS)
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
	if (MOD_CORE_DISABLE_LUA_HOOKS)
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
	if (MOD_CORE_DISABLE_LUA_HOOKS)
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
	if (MOD_CORE_DISABLE_LUA_HOOKS)
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
