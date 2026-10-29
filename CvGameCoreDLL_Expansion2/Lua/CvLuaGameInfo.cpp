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
#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "!CvLuaApi.h"
#include "CvLuaGameInfo.h"

//------------------------------------------------------------------------------
void CvLuaGameInfo::RegisterGlobals(lua_State* L)
{
	//Register other game info structures.
	luaVp_pcall(L, pRegisterActions);
	luaVp_pcall(L, pRegisterTypesInfo);
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
	for(ActionInfos::const_iterator it = infos.begin(); it != infos.end(); ++it)
	{
		CvActionInfo* pEntry = (*it);
		lua_createtable(L, 0, 0);

		luaEx_quickset(L, -1, "MissionData", pEntry->getMissionData() );
		luaEx_quickset(L, -1, "CommandData", pEntry->getCommandData() );

		luaEx_quickset(L, -1, "AutomateType", pEntry->getAutomateType() );
		luaEx_quickset(L, -1, "InterfaceModeType", pEntry->getInterfaceModeType() );
		luaEx_quickset(L, -1, "MissionType", pEntry->getMissionType() );
		luaEx_quickset(L, -1, "CommandType", pEntry->getCommandType() );
		luaEx_quickset(L, -1, "ControlType", pEntry->getControlType() );
		luaEx_quickset(L, -1, "OriginalIndex", pEntry->getOriginalIndex() );

		luaEx_quickset(L, -1, "ConfirmCommand", pEntry->isConfirmCommand() );
		luaEx_quickset(L, -1, "Visible", pEntry->isVisible() );
		luaEx_quickset(L, -1, "SubType", (int)pEntry->getSubType() );

		luaEx_quickset(L, -1, "Type", pEntry->GetType() );
		luaEx_quickset(L, -1, "TextKey", pEntry->GetTextKey() );

		luaEx_quickset(L, -1, "ActionInfoIndex", pEntry->getActionInfoIndex() );
		luaEx_quickset(L, -1, "HotKeyVal", pEntry->getHotKeyVal() );
		luaEx_quickset(L, -1, "HotKeyPriority", pEntry->getHotKeyPriority() );
		luaEx_quickset(L, -1, "HotKeyValAlt", pEntry->getHotKeyValAlt() );
		luaEx_quickset(L, -1, "HotKeyPriorityAlt", pEntry->getHotKeyPriorityAlt() );
		luaEx_quickset(L, -1, "OrderPriority", pEntry->getOrderPriority() );

		luaEx_quickset(L, -1, "AltDown", pEntry->isAltDown() );
		luaEx_quickset(L, -1, "ShiftDown", pEntry->isShiftDown() );
		luaEx_quickset(L, -1, "CtrlDown", pEntry->isCtrlDown() );
		luaEx_quickset(L, -1, "AltDownAlt", pEntry->isAltDownAlt() );
		luaEx_quickset(L, -1, "ShiftDownAlt", pEntry->isShiftDownAlt() );
		luaEx_quickset(L, -1, "CtrlDownAlt", pEntry->isCtrlDownAlt() );

		luaEx_quickset(L, -1, "HotKey", pEntry->getHotKeyString() );
		luaEx_quickset(L, -1, "Help", pEntry->GetHelp() );
		luaEx_quickset(L, -1, "DisabledHelp", pEntry->GetDisabledHelp() );

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
