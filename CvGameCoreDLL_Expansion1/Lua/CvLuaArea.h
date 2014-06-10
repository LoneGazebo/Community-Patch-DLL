/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once
#ifndef CVLUAAREA_H
#define CVLUAAREA_H

#include "CvLuaScopedInstance.h"

class CvLuaArea :  public CvLuaScopedInstance<CvLuaArea, CvArea>
{
public:

	//! Push CvArea methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Required by CvLuaScopedInstance.
	static void HandleMissingInstance(lua_State* L);

	//! Required by CvLuaScopedInstance.
	static const char* GetTypeName();

protected:
	static int lIsNone(lua_State* L);

	static int lCalculateTotalBestNatureYield(lua_State* L);
	static int lCountCoastalLand(lua_State* L);
	static int lCountNumUniqueResourceTypes(lua_State* L);
	static int lGetID(lua_State* L);
	static int lGetNumTiles(lua_State* L);
	//static int lIsLake(lua_State* L);
	static int lGetNumOwnedTiles(lua_State* L);
	static int lGetNumUnownedTiles(lua_State* L);
	static int lGetNumRiverEdges(lua_State* L);
	static int lGetNumCities(lua_State* L);
	static int lGetNumUnits(lua_State* L);
	static int lGetTotalPopulation(lua_State* L);
	static int lGetNumStartingPlots(lua_State* L);
	static int lIsWater(lua_State* L);

	static int lGetUnitsPerPlayer(lua_State* L);
	static int lGetCitiesPerPlayer(lua_State* L);
	static int lGetPopulationPerPlayer(lua_State* L);
	static int lGetFreeSpecialist(lua_State* L);

	static int lGetNumRevealedTiles(lua_State* L);
	static int lGetNumUnrevealedTiles(lua_State* L);

	static int lGetTargetCity(lua_State* L);
	static int lGetYieldRateModifier(lua_State* L);

	static int lGetNumResources(lua_State* L);
	static int lGetNumTotalResources(lua_State* L);
	static int lGetNumImprovements(lua_State* L);
};

#endif //CVLUAAREA_H