/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_MAPGENERATOR_H
#define CIV5_MAPGENERATOR_H

class CvMapGenerator
{
public:
	CvMapGenerator(const char* szMapScriptName);
	virtual ~CvMapGenerator();

	//! Call Lua function to obtain map overrides.
	bool GetMapInitData(CvMapInitData& kData, WorldSizeTypes eWorldSize);

	//! Call Lua function to generate random map.
	void GenerateRandomMap();

	//! Call Lua function to obtain initial game items overrides.
	bool GetGameInitialItemsOverrides(CvGameInitialItemsOverrides& kOverrides);

protected:

	//! Protected Lua continuation of GetMapInitData.
	static int pGetMapInitData(lua_State* L);

	//! Protected Lua continuation of GenerateRandomMap.
	static int pGenerateRandomMap(lua_State* L);

	//! Protected Lua continuation of GetGameInitialItemsOverrides
	static int pGetGameInitialItemsOverrides(lua_State* L);

private:
	CvMapGenerator(const CvMapGenerator&);	//Stub to prevent compiler generated function.
	CvMapGenerator& operator=(const CvMapGenerator&);	//Stub to prevent compiler generated function.

	lua_State* m_pkLuaState;
};

#endif
