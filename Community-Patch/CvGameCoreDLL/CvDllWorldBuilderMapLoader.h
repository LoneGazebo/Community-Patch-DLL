/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvDllInterfaces.h"

class CvDllWorldBuilderMapLoader : public ICvWorldBuilderMapLoader2
{
public:
	CvDllWorldBuilderMapLoader();
	~CvDllWorldBuilderMapLoader();

	void* DLLCALL QueryInterface(GUID guidInterface);

	static void operator delete(void* p);
	static void* operator new (size_t bytes);

	const CvWorldBuilderMapLoaderMapInfo& DLLCALL GetCurrentMapInfo();

	// Map Loading.  Start with preload.  Then call setup functions.
	bool DLLCALL Preload( _In_z_ const wchar_t* wszFilename, bool bScenario);
	void DLLCALL SetupGameOptions();
	void DLLCALL SetupPlayers();
	void DLLCALL SetInitialItems(bool bFirstCall);
	bool DLLCALL InitMap();

	// Map Saving
	bool DLLCALL Save( _In_z_ const wchar_t* wszFilename, const char *szMapName = NULL);

	// Post-Load Utilities
	int DLLCALL LoadModData(lua_State *L);
	void DLLCALL ValidateTerrain();
	void DLLCALL ValidateCoast();
	void DLLCALL ClearResources();
	void DLLCALL ClearGoodies();

	// Player slot mapping utilities
	void DLLCALL ResetPlayerSlots();
	void DLLCALL MapPlayerToSlot(uint uiPlayer, PlayerTypes ePlayerSlot);

	// Map Preview Functions
	unsigned int DLLCALL PreviewPlayableCivCount( _In_z_ const wchar_t* wszFilename);
	int DLLCALL GetMapPreview(lua_State *L);
	int DLLCALL GetMapPlayers(lua_State *L);

	// Used to place random goodies when loading a naked map (no scenario data)
	int DLLCALL AddRandomItems(lua_State *L);
	int DLLCALL ScatterResources(lua_State *L);
	int DLLCALL ScatterGoodies(lua_State *L);

	PlayerTypes DLLCALL GetMapPlayerSlot(uint uiPlayer);
	int DLLCALL GetMapPlayerCount();

	void DLLCALL GenerateRandomMap(const char* szMapScript);

	WorldSizeTypes DLLCALL GetWorldSizeType() const;

private:
	void DLLCALL Destroy();
};
