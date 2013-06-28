/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaGameInfo.h
//!  \brief     Public interface to CvLuaGameInfo.
//!
//!		This file includes the interface for exposing game infos to Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUAGAMEINFO_H
#define CVLUAGAMEINFO_H

class CvLuaGameInfo
{
public:
	//! Register all game infos data in Lua.
	static void Register(lua_State* L);

private:

	//! Registers CvActionInfo data.
	static int pRegisterActions(lua_State* L);

	//! Registers info types.
	static int pRegisterTypesInfo(lua_State* L);
};


#endif //CVLUAGAMEINFO_H
