/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaEnums.h
//!  \brief     Public interface to CvLuaEnums.
//!
//!		This file includes the interface for exposing enums to Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUAENUMS_H
#define CVLUAENUMS_H

class CvLuaEnums
{
public:
	//! Registers all game enums in Lua.
	static void Register(lua_State* L);

protected:

	//! Protected call to assist registration.
	static int pRegister(lua_State* L);

	//! Registers a single enum (Must be called between EnumStart and EnumEnd).
	static void pRegisterEnum(lua_State* L, const char* enumName, const int enumVal);
	static void pRegisterEnumUInt(lua_State* L, const char* enumName, const uint enumVal);

	//! Registers 'dynamic' enumerations created by game info database.
	static void RegisterDynamicEnums(lua_State* L, _In_z_ const char* szTableName,
					_In_z_ const char* szIdField = "_ROWID_", _In_z_ const char* szNameField = "Type", _In_opt_z_ const char* szCountName = NULL);

	//! Denotes the start of registering a specific type of enum.
	static void EnumStart(lua_State* L, const char* enumType);

	//! Denotes the end of registering a specific type of enum.
	static void EnumEnd(lua_State* L);

};

#endif //CVLUAENUMS_H