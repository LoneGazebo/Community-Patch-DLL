/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaPlot.h
//!  \brief     Public interface to CvLuaPlot.
//!
//!		This file includes the interface for a Lua Plot object.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUAPLOT_H
#define CVLUAPLOT_H

#include "CvLuaScopedInstance.h"

class CvLuaPlot : public CvLuaScopedInstance<CvLuaPlot, CvPlot>
{
public:
	//! Push CvPlot methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Required by CvLuaScopedInstance.
	static void HandleMissingInstance(lua_State* L);

	//! Required by CvLuaScopedInstance.
	static const char* GetTypeName();

protected:

	//! (LUA) CvPlot::CanHaveFeature.
	static int lCanHaveFeature(lua_State* L);

	//! (LUA) CvPlot::GetFeatureType.
	static int lGetFeatureType(lua_State* L);

	//! (LUA) CvPlot::GetTerrainType.
	static int lGetTerrainType(lua_State* L);

	//! (LUA) CvPlot::IsRiver.
	static int lIsRiver(lua_State* L);

	//! (LUA) CvPlot::IsWater.
	static int lIsWater(lua_State* L);

	//! (LUA) CvPlot::SetFeatureType.
	static int lSetFeatureType(lua_State* L);

	//! (LUA) CvPlot::SetTerrainType.
	static int lSetTerrainType(lua_State* L);

	//The following is a dump from CyPlot
	static int lIsNone(lua_State* L);
	static int lErase(lua_State* L);
	static int lGetTeam(lua_State* L);

	static int lNukeExplosion(lua_State* L);

	static int lUpdateFog(lua_State* L);
	static int lUpdateVisibility(lua_State* L);
	static int lIsAdjacentToArea(lua_State* L);
	static int lShareAdjacentArea(lua_State* L);
	static int lIsAdjacentToLand(lua_State* L);
	static int lIsAdjacentToShallowWater(lua_State* L);
	static int lIsCoastalLand(lua_State* L);

	static int lIsWithinTeamCityRadius(lua_State* L);

	static int lIsLake(lua_State* L);
	static int lIsFreshWater(lua_State* L);

	static int lIsRiverSide(lua_State* L);
	static int lIsRiverConnection(lua_State* L);
	static int lIsRiverCrossingFlowClockwise(lua_State* L);

	static int lGetInlandCorner(lua_State* L);
	static int lGetNearestLandArea(lua_State* L);
	static int lSeeFromLevel(lua_State* L);
	static int lGetNearestLandPlot(lua_State* L);
	static int lSeeThroughLevel(lua_State* L);
	static int lCanHaveResource(lua_State* L);
	static int lCanHaveImprovement(lua_State* L);
	static int lCanBuild(lua_State* L);
	static int lGetBuildTime(lua_State* L);
	static int lGetBuildTurnsLeft(lua_State* L);
	static int lGetBuildTurnsTotal(lua_State* L);
	static int lGetFeatureProduction(lua_State* L);

	static int lGetBestDefender(lua_State* L);
	static int lGetSelectedUnit(lua_State* L);
	static int lGetUnitPower(lua_State* L);

	static int lDefenseModifier(lua_State* L);
	static int lMovementCost(lua_State* L);

	static int lGetExtraMovePathCost(lua_State* L);
	static int lChangeExtraMovePathCost(lua_State* L);

	static int lIsAdjacentOwned(lua_State* L);
	static int lIsAdjacentPlayer(lua_State* L);
	static int lIsAdjacentTeam(lua_State* L);

	static int lIsOwned(lua_State* L);
	static int lIsBarbarian(lua_State* L);
	static int lIsRevealedBarbarian(lua_State* L);
	static int lHasBarbarianCamp(lua_State* L);
	static int lIsVisible(lua_State* L);
	static int lIsActiveVisible(lua_State* L);
	static int lIsVisibleToWatchingHuman(lua_State* L);
	static int lIsAdjacentVisible(lua_State* L);
	static int lIsAdjacentNonvisible(lua_State* L);
	static int lIsAdjacentRevealed(lua_State* L);
	static int lIsAdjacentNonrevealed(lua_State* L);

	static int lRemoveGoody(lua_State* L);
	static int lIsGoody(lua_State* L);
	static int lIsRevealedGoody(lua_State* L);

	static int lIsFriendlyTerritory(lua_State* L);

	static int lIsCity(lua_State* L);
	static int lIsFriendlyCity(lua_State* L);
	static int lIsEnemyCity(lua_State* L);
	static int lIsBeingWorked(lua_State* L);

	static int lIsUnit(lua_State* L);
	static int lIsVisibleEnemyDefender(lua_State* L);
	static int lGetNumDefenders(lua_State* L);
	static int lGetNumVisibleEnemyDefenders(lua_State* L);
	static int lGetNumVisiblePotentialEnemyDefenders(lua_State* L);
	static int lIsVisibleEnemyUnit(lua_State* L);
	static int lIsVisibleOtherUnit(lua_State* L);
	static int lGetNumFriendlyUnitsOfType(lua_State* L);
	static int lIsFighting(lua_State* L);

	static int lIsRoute(lua_State* L);
	static int lIsTradeRoute(lua_State* L);
	static int lIsValidDomainForLocation(lua_State* L);
	static int lIsValidDomainForAction(lua_State* L);
	static int lIsImpassable(lua_State* L);

	static int lGetX(lua_State* L);
	static int lGetY(lua_State* L);
	static int lAt(lua_State* L);
	static int lGetPlotIndex(lua_State* L);
	static int lGetLatitude(lua_State* L);
	static int lArea(lua_State* L);
	static int lWaterArea(lua_State* L);
	static int lGetArea(lua_State* L);
	static int lSetArea(lua_State* L);
	static int lGetFeatureVariety(lua_State* L);

	static int lGetOwnershipDuration(lua_State* L);
	static int lIsOwnershipScore(lua_State* L);
	static int lSetOwnershipDuration(lua_State* L);
	static int lChangeOwnershipDuration(lua_State* L);

	static int lGetImprovementDuration(lua_State* L);
	static int lSetImprovementDuration(lua_State* L);
	static int lChangeImprovementDuration(lua_State* L);

	static int lGetUpgradeProgress(lua_State* L);
	static int lGetUpgradeTimeLeft(lua_State* L);

	static int lSetUpgradeProgress(lua_State* L);
	static int lChangeUpgradeProgress(lua_State* L);

	static int lGetCityRadiusCount(lua_State* L);
	static int lIsCityRadius(lua_State* L);

	static int lIsStartingPlot(lua_State* L);
	static int lSetStartingPlot(lua_State* L);
	static int lIsNEOfRiver(lua_State* L);
	static int lSetNEOfRiver(lua_State* L);
	static int lIsWOfRiver(lua_State* L);
	static int lSetWOfRiver(lua_State* L);
	static int lIsNWOfRiver(lua_State* L);
	static int lSetNWOfRiver(lua_State* L);
	static int lGetRiverEFlowDirection(lua_State* L);
	static int lGetRiverSEFlowDirection(lua_State* L);
	static int lGetRiverSWFlowDirection(lua_State* L);

	static int lIsPotentialCityWork(lua_State* L);
	static int lIsPotentialCityWorkForArea(lua_State* L);

	static int lGetOwner(lua_State* L);
	static int lSetOwner(lua_State* L);
	static int lGetPlotType(lua_State* L);
	static int lIsFlatlands(lua_State* L);
	static int lIsHills(lua_State* L);
	static int lIsOpenGround(lua_State* L);
	static int lIsRoughGround(lua_State* L);
	static int lIsMountain(lua_State* L);

	static int lSetPlotType(lua_State* L);
	static int lGetResourceType(lua_State* L);
	static int lGetNonObsoleteResourceType(lua_State* L);
	static int lSetResourceType(lua_State* L);
	static int lGetNumResource(lua_State* L);
	static int lSetNumResource(lua_State* L);
	static int lChangeNumResource(lua_State* L);

	static int lGetImprovementType(lua_State* L);
	static int lSetImprovementType(lua_State* L);
	static int lSetImprovementPillaged(lua_State* L);
	static int lGetRouteType(lua_State* L);
	static int lSetRouteType(lua_State* L);
	static int lIsRoutePillaged(lua_State* L);

	static int lGetPlotCity(lua_State* L);
	static int lGetWorkingCity(lua_State* L);
	static int lGetWorkingCityOverride(lua_State* L);
	static int lGetReconCount(lua_State* L);
	static int lGetRiverCrossingCount(lua_State* L);
	static int lGetYield(lua_State* L);
	static int lCalculateNatureYield(lua_State* L);
	static int lCalculateBestNatureYield(lua_State* L);
	static int lCalculateTotalBestNatureYield(lua_State* L);
	static int lCalculateImprovementYieldChange(lua_State* L);
	static int lCalculateYield(lua_State* L);
	static int lHasYield(lua_State* L);
	static int lGetYieldWithBuild(lua_State* L);

	static int lCountNumAirUnits(lua_State* L);

	static int lGetFoundValue(lua_State* L);
	static int lIsBestAdjacentFound(lua_State* L);

	static int lGetPlayerCityRadiusCount(lua_State* L);
	static int lIsPlayerCityRadius(lua_State* L);

	static int lGetVisibilityCount(lua_State* L);
	static int lChangeVisibilityCount(lua_State* L);

	static int lGetRevealedOwner(lua_State* L);
	static int lGetRevealedTeam(lua_State* L);

	static int lIsRiverCrossing(lua_State* L);
	static int lIsRiverCrossingToPlot(lua_State* L);

	static int lIsRevealed(lua_State* L);
	static int lSetRevealed(lua_State* L);
	static int lGetRevealedImprovementType(lua_State* L);
	static int lGetRevealedRouteType(lua_State* L);
	static int lGetBuildProgress(lua_State* L);
	static int lChangeBuildProgress(lua_State* L);

	static int lGetCultureRangeCities(lua_State* L);
	static int lIsCultureRangeCity(lua_State* L);

	static int lGetInvisibleVisibilityCount(lua_State* L);
	static int lIsInvisibleVisible(lua_State* L);
	static int lChangeInvisibleVisibilityCount(lua_State* L);

	static int lGetNumUnits(lua_State* L);
	static int lGetUnit(lua_State* L);

	static int lGetScriptData(lua_State* L);
	static int lSetScriptData(lua_State* L);
	static int lGetActiveFogOfWarMode(lua_State* L);

	static int lIsImprovementPillaged(lua_State* L);

	static int lCanSeePlot(lua_State* L);

	static int lGetContinentArtType(lua_State* L);
	static int lSetContinentArtType(lua_State* L);

	static int lIsResourceConnectedByImprovement(lua_State* L);

	static int lIsBuildRemovesFeature(lua_State* L);
};

#endif