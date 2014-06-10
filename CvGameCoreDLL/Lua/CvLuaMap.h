/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaMap.h
//!  \brief     Public interface to CvLuaMap.
//!
//!		This file includes the interface for a Lua Map singleton.
//!		The Map table in Lua represents the current map in use.  Since only
//!		one map is in use during a given game, the Lua exposures are
//!		implemented as a singleton.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUAMAP_H
#define CVLUAMAP_H

#include "CvLuaStaticInstance.h"

class CvLuaMap : public CvLuaStaticInstance<CvLuaMap, CvMap>
{
public:
	//! Returns the name of the static instance.
	static const char* GetInstanceName();

	//! Returns the static instance.
	static CvMap* GetInstance(lua_State* L, int idx);

	//! Registers member methods with instance.
	static void RegisterMembers(lua_State* L);

protected:

	//! (Lua) CvMap Area iteration
	static int lAreas(lua_State* L);
	static int lAreasAux(lua_State* L);

	static int lFindBiggestArea(lua_State* L);

	//! (Lua) CvMap::FindWater.
	static int lFindWater(lua_State* L);

	//! (Lua) CvMap::GetClimate.
	static int lGetClimate(lua_State* L);

	//! (Lua) CvMap::GetFractalFlags.
	static int lGetFractalFlags(lua_State* L);

	static int lGetNumPlots(lua_State* L);

	//! (Lua) CvMap::GetNumResources.
	static int lGetNumResources(lua_State* L);

	//! (Lua) CvMap::GetNumResourcesOnLand.
	static int lGetNumResourcesOnLand(lua_State* L);

	//! (Lua) CvMap::Plot.
	static int lGetPlot(lua_State* L);
	static int lGetPlotByIndex(lua_State* L);

	static int lGetPlotXY(lua_State* L);

	static int lGetArea(lua_State* L);

	//! (Lua) CvMap::GetGridSize.
	static int lGetGridSize(lua_State* L);

	static int lGetIndexAfterLastArea(lua_State* L);

	static int lGetLandPlots(lua_State* L);

	static int lGetNumAreas(lua_State* L);

	static int lGetNumLandAreas(lua_State* L);

	//! (Lua) CvMap::GetRandomResourceQuantity.
	static int lGetRandomResourceQuantity(lua_State* L);

	//! (Lua) CvMap::GetSeaLevel.
	static int lGetSeaLevel(lua_State* L);

	//! (Lua) CvMap::GetWorldSize.
	static int lGetWorldSize(lua_State* L);

	static int lIsPlot(lua_State* L);

	//! (Lua) CvMap::IsWrapX.
	static int lIsWrapX(lua_State* L);

	//! (Lua) CvMap::IsWrapY.
	static int lIsWrapY(lua_State* L);

	static int lMaxPlotDistance(lua_State* L);

	//! (Lua) Rand using CvMap::GetRand.
	static int lRand(lua_State* L);

	//! (Lua) calculate areas
	static int lCalculateAreas(lua_State* L);

	//! (Lua) recalculate areas
	static int lRecalculateAreas(lua_State* L);

	//! (Lua) distance in hexes between two plots
	static int lPlotDistance(lua_State* L);

	//! (Lua) find a plot within a certain range offset (in hexes) from a plot (in grid space)
	static int lPlotXYWithRangeCheck(lua_State* L);

	//! (Lua) find an adjacent plot in a certain direction from this plot
	static int lPlotDirection(lua_State* L);

	//! (Lua) the default "continent stamper"
	static int lDefaultContinentStamper(lua_State* L);

	//! (Lua) the default natural wonder placer
	static int lDoPlaceNaturalWonders(lua_State* L);

	//! (Lua) gets a custom map option of a specified name.
	static int lGetCustomOption(lua_State* L);

	static int lUpdateDeferredFog(lua_State* L);

	static int lChangeAIMapHint(lua_State* L);
	static int lGetAIMapHint(lua_State* L);

};

#endif //CVLUAMAP_H