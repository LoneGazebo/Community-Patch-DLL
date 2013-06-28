/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);

void CvLuaArea::PushMethods(lua_State* L, int t)
{
	Method(IsNone);

	Method(CalculateTotalBestNatureYield);
	Method(CountCoastalLand);
	Method(CountNumUniqueResourceTypes);
	Method(GetID);
	Method(GetNumTiles);
	//Method(IsLake);
	Method(GetNumOwnedTiles);
	Method(GetNumUnownedTiles);
	Method(GetNumRiverEdges);
	Method(GetNumCities);
	Method(GetNumUnits);
	Method(GetTotalPopulation);
	Method(GetNumStartingPlots);
	Method(IsWater);

	Method(GetUnitsPerPlayer);
	Method(GetCitiesPerPlayer);
	Method(GetPopulationPerPlayer);
	Method(GetFreeSpecialist);

	Method(GetNumRevealedTiles);
	Method(GetNumUnrevealedTiles);

	Method(GetTargetCity);
	Method(GetYieldRateModifier);

	Method(GetNumResources);
	Method(GetNumTotalResources);
	Method(GetNumImprovements);
}
//------------------------------------------------------------------------------
void CvLuaArea::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}
//------------------------------------------------------------------------------
const char* CvLuaArea::GetTypeName()
{
	return "Area";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------
//bool isNone();
int CvLuaArea::lIsNone(lua_State* L)
{
	const bool bExists = (GetInstance(L, false) != NULL);
	lua_pushboolean(L, bExists);

	return 1;
}
//int calculateTotalBestNatureYield();
int CvLuaArea::lCalculateTotalBestNatureYield(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->calculateTotalBestNatureYield();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int countCoastalLand();
int CvLuaArea::lCountCoastalLand(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->countCoastalLand();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int countNumUniqueResourceTypes();
int CvLuaArea::lCountNumUniqueResourceTypes(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->countNumUniqueResourceTypes();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getID();
int CvLuaArea::lGetID(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->GetID();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumTiles();
int CvLuaArea::lGetNumTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumTiles();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isLake();
//int CvLuaArea::lIsLake(lua_State* L)
//{
//	CvArea* pkArea = GetInstance(L);
//
//	const bool bResult = pkArea->isLake();
//	lua_pushboolean(L, bResult);
//	return 1;
//}
//------------------------------------------------------------------------------
//int getNumOwnedTiles();
int CvLuaArea::lGetNumOwnedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumOwnedTiles();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumUnownedTiles();
int CvLuaArea::lGetNumUnownedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumUnownedTiles();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumRiverEdges();
int CvLuaArea::lGetNumRiverEdges(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumRiverEdges();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumCities();
int CvLuaArea::lGetNumCities(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumCities();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumUnits();
int CvLuaArea::lGetNumUnits(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumUnits();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getTotalPopulation();
int CvLuaArea::lGetTotalPopulation(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getTotalPopulation();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumStartingPlots();
int CvLuaArea::lGetNumStartingPlots(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumStartingPlots();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isWater();
int CvLuaArea::lIsWater(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const bool bResult = pkArea->isWater();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnitsPerPlayer(PlayerTypes eIndex);
int CvLuaArea::lGetUnitsPerPlayer(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getUnitsPerPlayer(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getCitiesPerPlayer(PlayerTypes eIndex);
int CvLuaArea::lGetCitiesPerPlayer(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getCitiesPerPlayer(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getPopulationPerPlayer(PlayerTypes eIndex);
int CvLuaArea::lGetPopulationPerPlayer(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getPopulationPerPlayer(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getFreeSpecialist(PlayerTypes eIndex);
int CvLuaArea::lGetFreeSpecialist(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getFreeSpecialist(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumRevealedTiles(TeamTypes eIndex);
int CvLuaArea::lGetNumRevealedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const TeamTypes eIndex = (TeamTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumRevealedTiles(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumUnrevealedTiles(TeamTypes eIndex);
int CvLuaArea::lGetNumUnrevealedTiles(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const TeamTypes eIndex = (TeamTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumUnrevealedTiles(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//CyCity* getTargetCity(PlayerTypes eIndex);
int CvLuaArea::lGetTargetCity(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex = (PlayerTypes)lua_tointeger(L, 2);

	CvCity* pkCity = pkArea->getTargetCity(eIndex);
	CvLuaCity::Push(L, pkCity);
	return 1;
}
//------------------------------------------------------------------------------
//int getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2);
int CvLuaArea::lGetYieldRateModifier(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const PlayerTypes eIndex1 = (PlayerTypes)lua_tointeger(L, 2);
	const YieldTypes eIndex2 = (YieldTypes)lua_tointeger(L, 3);

	const int iResult = pkArea->getYieldRateModifier(eIndex1, eIndex2);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumResources(ResourceTypes eResource);
int CvLuaArea::lGetNumResources(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumResources(eResource);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumTotalResources();
int CvLuaArea::lGetNumTotalResources(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);

	const int iResult = pkArea->getNumTotalResources();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumImprovements(ImprovementTypes eImprovement);
int CvLuaArea::lGetNumImprovements(lua_State* L)
{
	CvArea* pkArea = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes)lua_tointeger(L, 2);

	const int iResult = pkArea->getNumImprovements(eImprovement);
	lua_pushinteger(L, iResult);
	return 1;
}
