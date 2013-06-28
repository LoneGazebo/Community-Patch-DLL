/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaMap.cpp
//!  \brief     Private implementation to CvLuaMap.
//!
//!		This file includes the implementation for a Lua Map singleton.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaMap.h"
#include "CvLuaPlot.h"

#include "..\CvFractal.h"
#include "..\CvMap.h"
#include "..\CvGameCoreUtils.h"

#define Method(func) RegisterMethod(L, l##func, #func);

//------------------------------------------------------------------------------
const char* CvLuaMap::GetInstanceName()
{
	return "Map";
}
//------------------------------------------------------------------------------
CvMap* CvLuaMap::GetInstance(lua_State* /*L*/, int /*idx*/)
{
	return &GC.getMap();
}
//------------------------------------------------------------------------------
void CvLuaMap::RegisterMembers(lua_State* L)
{
	Method(Areas);
	Method(FindBiggestArea);
	Method(FindWater);
	Method(GetClimate);
	Method(GetFractalFlags);
	Method(GetGridSize);
	Method(GetNumPlots);
	Method(GetNumResources);
	Method(GetNumResourcesOnLand);
	Method(GetPlot);
	Method(GetPlotByIndex);
	Method(GetPlotXY);
	Method(GetArea);
	Method(GetIndexAfterLastArea);
	Method(GetLandPlots);
	Method(GetNumAreas);
	Method(GetNumLandAreas);
	Method(GetRandomResourceQuantity);
	Method(GetSeaLevel);
	Method(GetWorldSize);
	Method(IsPlot);
	Method(IsWrapX);
	Method(IsWrapY);
	Method(MaxPlotDistance);
	Method(Rand);
	Method(CalculateAreas);
	Method(RecalculateAreas);
	Method(PlotDistance);
	Method(PlotXYWithRangeCheck);
	Method(PlotDirection);
	Method(DefaultContinentStamper);
	Method(DoPlaceNaturalWonders);
	Method(GetCustomOption);
	Method(UpdateDeferredFog);
	Method(ChangeAIMapHint);
	Method(GetAIMapHint);
}
//------------------------------------------------------------------------------
int CvLuaMap::lAreas(lua_State* L)
{
	lua_pushcclosure(L, CvLuaMap::lAreasAux, 0);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lAreasAux(lua_State* L)
{
	int it = 0;
	CvArea* pkArea = NULL;

	if(lua_isnoneornil(L, 2))
	{
		pkArea = GC.getMap().firstArea(&it);
	}
	else
	{
		it = lua_tointeger(L, 2);
		pkArea = GC.getMap().nextArea(&it);
	}

	if(pkArea)
	{
		lua_pushinteger(L, it);
		CvLuaArea::Push(L, pkArea);
		return 2;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lFindBiggestArea(lua_State* L)
{
	const bool bWater = lua_toboolean(L, 1);

	CvArea* pkArea = GC.getMap().findBiggestArea(bWater);
	if(pkArea)
	{
		CvLuaArea::Push(L, pkArea);
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lFindWater(lua_State* L)
{
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 1);
	const int iRange = luaL_checkint(L, 2);
	const bool bFreshWater = lua_toboolean(L, 3);
	const bool bResult = GC.getMap().findWater(pkPlot, iRange, bFreshWater);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetClimate(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().getClimate());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetFractalFlags(lua_State* L)
{
	int flags = GC.getMap().getMapFractalFlags();
	lua_createtable(L, 0, 2);
	if((flags & CvFractal::FRAC_WRAP_X) != 0)
	{
		lua_pushboolean(L, TRUE);
		lua_setfield(L, -2, "FRAC_WRAP_X");
	}

	if((flags & CvFractal::FRAC_WRAP_Y) != 0)
	{
		lua_pushboolean(L, TRUE);
		lua_setfield(L, -2, "FRAC_WRAP_Y");
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetGridSize(lua_State* L)
{
	const int width  = GC.getMap().getGridWidth();
	const int height = GC.getMap().getGridHeight();

	lua_pushinteger(L, width);
	lua_pushinteger(L, height);
	return 2;
}

//------------------------------------------------------------------------------
int CvLuaMap::lGetNumPlots(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().numPlots());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetNumResources(lua_State* L)
{
	const ResourceTypes eResource = (ResourceTypes)luaL_checkinteger(L, 1);

	const int iResult = GC.getMap().getNumResources(eResource);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetNumResourcesOnLand(lua_State* L)
{
	const ResourceTypes eResource = (ResourceTypes)luaL_checkinteger(L, 1);

	const int iResult = GC.getMap().getNumResourcesOnLand(eResource);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetPlot(lua_State* L)
{
	const int x = lua_tointeger(L, 1);
	const int y = lua_tointeger(L, 2);

	CvPlot* pPlot = GC.getMap().plot(x, y);
	if(pPlot)
	{
		CvLuaPlot::Push(L, pPlot);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaMap::lGetPlotByIndex(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);

	CvPlot* pPlot = GC.getMap().plotByIndex(iIndex);
	if(pPlot)
	{
		CvLuaPlot::Push(L, pPlot);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
// plot GetPlotXY([int x], [int y], int dx, int dy)
int CvLuaMap::lGetPlotXY(lua_State* L)
{
	int x, y, dx, dy;
	const int count = lua_gettop(L);

	if(count == 2)
	{
		x = 0;
		y = 0;
		dx = lua_tointeger(L, 1);
		dy = lua_tointeger(L, 2);
	}
	else
	{
		x = lua_tointeger(L, 1);
		y = lua_tointeger(L, 2);
		dx = lua_tointeger(L, 3);
		dy = lua_tointeger(L, 4);
	}

	CvPlot* pkPlot = plotXY(x, y, dx, dy);
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaMap::lGetArea(lua_State* L)
{
	const int iID = lua_tointeger(L, 1);

	CvArea* pArea = GC.getMap().getArea(iID);
	if(pArea)
	{
		CvLuaArea::Push(L, pArea);
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetIndexAfterLastArea(lua_State* L)
{
	const int iResult = GC.getMap().getIndexAfterLastArea();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetLandPlots(lua_State* L)
{
	const int iResult = GC.getMap().getLandPlots();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetNumAreas(lua_State* L)
{
	const int iResult = GC.getMap().getNumAreas();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetNumLandAreas(lua_State* L)
{
	const int iResult = GC.getMap().getNumLandAreas();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetRandomResourceQuantity(lua_State* L)
{
	const ResourceTypes eResource = (ResourceTypes)luaL_checkinteger(L, 1);

	const int iResult = GC.getMap().getRandomResourceQuantity(eResource);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetSeaLevel(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().getSeaLevel());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetWorldSize(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().getWorldSize());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lIsPlot(lua_State* L)
{
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);

	const bool bResult = GC.getMap().isPlot(x, y);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lIsWrapX(lua_State* L)
{
	lua_pushboolean(L, GC.getMap().isWrapX());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lIsWrapY(lua_State* L)
{
	lua_pushboolean(L, GC.getMap().isWrapY());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lMaxPlotDistance(lua_State* L)
{
	int iResult = GC.getMap().maxPlotDistance();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lRand(lua_State* L)
{
	const int max_num = luaL_checkinteger(L, 1);
	const char* strLog = luaL_checkstring(L, 2);
	const int rand_val = GC.getGame().getMapRandNum(max_num, strLog);

	lua_pushinteger(L, rand_val);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lCalculateAreas(lua_State* L)
{
	GC.getMap().calculateAreas();
	return 0;
}//------------------------------------------------------------------------------
int CvLuaMap::lRecalculateAreas(lua_State* L)
{
	GC.getMap().recalculateAreas();
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lPlotDistance(lua_State* L)
{
	int iX1 = lua_tointeger(L, 1);
	int iY1 = lua_tointeger(L, 2);
	int iX2 = lua_tointeger(L, 3);
	int iY2 = lua_tointeger(L, 4);

	int iRetVal = plotDistance(iX1, iY1, iX2, iY2);

	lua_pushinteger(L, iRetVal);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lPlotXYWithRangeCheck(lua_State* L)
{
	int iX = lua_tointeger(L, 1);
	int iY = lua_tointeger(L, 2);
	int iDeltaHexX = lua_tointeger(L, 3);
	int iDeltaHexY = lua_tointeger(L, 4);
	int iRange = lua_tointeger(L, 5);
	CvPlot* pkPlot = plotXYWithRangeCheck(iX, iY, iDeltaHexX, iDeltaHexY, iRange);
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lPlotDirection(lua_State* L)
{
	int iX = lua_tointeger(L, 1);
	int iY = lua_tointeger(L, 2);
	DirectionTypes eDirection = (DirectionTypes)lua_tointeger(L, 3);

	CvPlot* pkPlot = plotDirection(iX, iY, eDirection);

	CvLuaPlot::Push(L, pkPlot);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaMap::lDefaultContinentStamper(lua_State* L)
{
	GC.getMap().DefaultContinentStamper();
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lDoPlaceNaturalWonders(lua_State* L)
{
	GC.getMap().DoPlaceNaturalWonders();
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetCustomOption(lua_State* L)
{
	const char* szOptionName = luaL_checkstring(L, 1);
	int i = 0;
	if(CvPreGame::GetMapOption(szOptionName, i))
	{
		lua_pushinteger(L, i);
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lUpdateDeferredFog(lua_State* L)
{
	GC.getMap().updateDeferredFog();
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lChangeAIMapHint(lua_State* L)
{
	int iHint = lua_tointeger(L, 1);
	GC.getMap().ChangeAIMapHint(iHint);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaMap::lGetAIMapHint(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().GetAIMapHint());
	return 1;
}
