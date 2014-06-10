/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_WORLD_BUILDER_MAP_LOADER_H
#define CIV5_WORLD_BUILDER_MAP_LOADER_H

class CvWorldBuilderMap;

class CvWorldBuilderMapLoader
{
public:

	static const CvWorldBuilderMapLoaderMapInfo& GetCurrentMapInfo();

	// Map Loading.  Start with preload.  Then call setup functions.
	static bool Preload(_In_z_ const wchar_t* wszFilename, bool bScenario);
	static void SetupGameOptions();
	static void SetupPlayers();
	static void SetInitialItems(bool bFirstCall);
	static bool InitMap();

	// Deprecated load function
	static bool Load(_In_z_ const wchar_t* wszFilename);

	// Map Saving
	static bool Save(_In_z_ const wchar_t* wszFilename, const char* szMapName = NULL);

	// Post-Load Utilities
	static int LoadModData(lua_State* L);
	static int RunPostProcessScript(lua_State* L);
	static void ValidateTerrain();
	static void ValidateCoast();
	static void ClearResources();
	static void ClearGoodies();

	static WorldSizeTypes GetCurrentWorldSizeType();
	static WorldSizeTypes GetWorldSizeType(const CvWorldBuilderMap& kMap);

	// Player slot mapping utilities
	static void ResetPlayerSlots();
	static void MapPlayerToSlot(uint uiPlayer, PlayerTypes ePlayerSlot);
	static PlayerTypes GetMapPlayerSlot(uint uiPlayer);
	static int GetMapPlayerCount();

	// Map Preview Functions
	static uint PreviewPlayableCivCount(_In_z_ const wchar_t* wszFilename);
	static int GetMapPreview(lua_State* L);
	static int GetMapPlayers(lua_State* L);

	// Used to place random goodies when loading a naked map (no scenario data)
	static int AddRandomItems(lua_State* L);
	static int ScatterResources(lua_State* L);
	static int ScatterGoodies(lua_State* L);
};

#endif // CIV5_WORLD_BUILDER_MAP_LOADER_H
