/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaGameInfo.cpp
//!  \brief     Public interface to CvLuaGameInfo.
//!
//!		This file includes the implementation for exposing game infos to Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaGameInfo.h"
#include <LuaExposures.h>

// Namespace aliases to reduce typing.
namespace Lua = FLua::Details;
using FLua::Table;

//------------------------------------------------------------------------------
void CvLuaGameInfo::Register(lua_State* L)
{
	//Register other game info structures.
	Lua::CCallWithErrorHandling(L, pRegisterActions);
	Lua::CCallWithErrorHandling(L, pRegisterTypesInfo);
}

//------------------------------------------------------------------------------
int CvLuaGameInfo::pRegisterActions(lua_State* L)
{
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "GameInfoActions");

	const int t = lua_gettop(L);
	int idx = 0;

	typedef std::vector<CvActionInfo*> ActionInfos;
	ActionInfos& infos = GC.getActionInfo();
	for(ActionInfos::const_iterator it = infos.begin();
	        it != infos.end(); ++it)
	{
		CvActionInfo* pEntry = (*it);
		lua_createtable(L, 0, 0);

		Table kEntry(L, -1);

		kEntry["MissionData"]		= pEntry->getMissionData();
		kEntry["CommandData"]		= pEntry->getCommandData();

		kEntry["AutomateType"]		= pEntry->getAutomateType();
		kEntry["InterfaceModeType"]	= pEntry->getInterfaceModeType();
		kEntry["MissionType"]		= pEntry->getMissionType();
		kEntry["CommandType"]		= pEntry->getCommandType();
		kEntry["ControlType"]		= pEntry->getControlType();
		kEntry["OriginalIndex"]		= pEntry->getOriginalIndex();

		kEntry["ConfirmCommand"]	= pEntry->isConfirmCommand();
		kEntry["Visible"]			= pEntry->isVisible();
		kEntry["SubType"]			= (int)pEntry->getSubType();

		kEntry["Type"]				= pEntry->GetType();
		kEntry["TextKey"]			= pEntry->GetTextKey();

		kEntry["ActionInfoIndex"]	= pEntry->getActionInfoIndex();
		kEntry["HotKeyVal"]			= pEntry->getHotKeyVal();
		kEntry["HotKeyPriority"]	= pEntry->getHotKeyPriority();
		kEntry["HotKeyValAlt"]		= pEntry->getHotKeyValAlt();
		kEntry["HotKeyPriorityAlt"]	= pEntry->getHotKeyPriorityAlt();
		kEntry["OrderPriority"]		= pEntry->getOrderPriority();

		kEntry["AltDown"]			= pEntry->isAltDown();
		kEntry["ShiftDown"]			= pEntry->isShiftDown();
		kEntry["CtrlDown"]			= pEntry->isCtrlDown();
		kEntry["AltDownAlt"]		= pEntry->isAltDownAlt();
		kEntry["ShiftDownAlt"]		= pEntry->isShiftDownAlt();
		kEntry["CtrlDownAlt"]		= pEntry->isCtrlDownAlt();

		kEntry["HotKey"]			= pEntry->getHotKeyString();
		kEntry["Help"]				= pEntry->GetHelp();
		kEntry["DisabledHelp"]		= pEntry->GetDisabledHelp();

		lua_rawseti(L, t, idx);

		++idx;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGameInfo::pRegisterTypesInfo(lua_State* L)
{
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "GameInfoTypes");

	const CvGlobals::InfosMap& kInfosMap = GC.GetInfoTypes();
	for(CvGlobals::InfosMap::const_iterator it = kInfosMap.begin();
	        it != kInfosMap.end(); ++ it)
	{
		lua_pushinteger(L, (*it).second);
		lua_setfield(L, -2, (*it).first.c_str());
	}

	return 0;
}
