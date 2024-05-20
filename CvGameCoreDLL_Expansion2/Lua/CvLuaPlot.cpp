/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaPlot.cpp
//!  \brief     Private implementation to CvLuaPlot.
//!
//!		This file includes the implementation for a Lua Plot object.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#include "CvLuaPlayer.h"
#include "../CvGameCoreUtils.h"

#pragma warning(disable:4800 ) //forcing value to bool 'true' or 'false'

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);

//safety measure against illegal pointers passed from lua
#define CHECK_PLOT_VALID(p) if(!p||!GC.getMap().isPlot(p->getX(),p->getY())|(p<GC.getMap().plotByIndexUnchecked(0))|(p>GC.getMap().plotByIndexUnchecked(GC.getMap().numPlots()-1))) return 0;

//------------------------------------------------------------------------------
void CvLuaPlot::PushMethods(lua_State* L, int t)
{
	Method(CanHaveFeature);
	Method(GetFeatureType);
	Method(GetTerrainType);
	Method(GetTurnDamage);
	Method(IsRiver);
	Method(IsWater);
	Method(IsBlockaded);
	Method(SetFeatureType);
	Method(SetTerrainType);

	Method(IsNone);
	Method(Erase);
	Method(GetTeam);

	Method(NukeExplosion);

	Method(GetInlandCorner);

	Method(UpdateFog);
	Method(UpdateVisibility);
	Method(IsAdjacentToArea);
	Method(IsAdjacentToLandmass);
	Method(ShareAdjacentArea);
	Method(IsAdjacentToLand);
	Method(IsAdjacentToShallowWater);
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	Method(IsAdjacentToIce);
#endif
	Method(IsCoastalLand);

	Method(IsWithinTeamCityRadius);

	Method(IsLake);
	Method(IsFreshWater);

	Method(IsRiverSide);
	Method(IsRiverConnection);
	Method(IsRiverCrossingFlowClockwise);
	Method(GetRiverID);

	Method(GetNearestLandArea);
	Method(SeeFromLevel);
	Method(GetNearestLandPlot);
	Method(SeeThroughLevel);
	Method(CanHaveResource);
	Method(CanHaveImprovement);
	Method(CanBuild);
	Method(GetBuildTime);
	Method(GetBuildTurnsLeft);
	Method(GetBuildTurnsTotal);
	Method(GetBuildTypeNeededToImproveResource);
	Method(GetFeatureProduction);

	Method(GetBestDefender);
	Method(GetSelectedUnit);
	Method(GetUnitPower);

	Method(DefenseModifier);
	Method(MovementCost);

	Method(GetExtraMovePathCost);
	Method(ChangeExtraMovePathCost);

	Method(IsAdjacentOwned);
	Method(IsAdjacentPlayer);
	Method(IsAdjacentTeam);

	Method(IsOwned);
	Method(IsBarbarian);
	Method(IsRevealedBarbarian);
	Method(HasBarbarianCamp);
	Method(HasDig);

	Method(IsVisible);
	Method(IsActiveVisible);
	Method(IsVisibleToWatchingHuman);
	Method(IsAdjacentVisible);
	Method(IsAdjacentNonvisible);
	Method(IsAdjacentRevealed);
	Method(IsAdjacentNonrevealed);

	Method(RemoveGoody);
	Method(IsGoody);
	Method(IsRevealedGoody);

	Method(IsFriendlyTerritory);

	Method(IsCity);
	Method(IsFriendlyCity);
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
	Method(isFriendlyCityOrPassableImprovement);
#endif
	Method(IsEnemyCity);
	Method(IsBeingWorked);

	Method(IsUnit);
	Method(IsVisibleEnemyDefender);
	Method(GetNumDefenders);
	Method(GetNumVisibleEnemyDefenders);
	Method(GetNumVisiblePotentialEnemyDefenders);
	Method(IsVisibleEnemyUnit);
	Method(IsVisibleOtherUnit);

	Method(GetNumFriendlyUnitsOfType);
	Method(getNumFriendlyUnitsOfType);
	Method(IsFighting);

#if defined(MOD_GLOBAL_STACKING_RULES)
	Method(GetUnitLimit);
#endif

	Method(IsRoute);
	Method(IsTradeRoute);
	Method(IsCityConnection)
	Method(IsImpassable);

	Method(GetX);
	Method(GetY);
	Method(GetIndex);
	Method(At);
	Method(GetPlotIndex);
	Method(GetLatitude);
	Method(Area);
	Method(WaterArea);
	Method(GetArea);
	Method(SetArea);
	Method(GetLandmass);
	Method(GetFeatureVariety);

	Method(GetOwnershipDuration);
	Method(IsOwnershipScore);
	Method(SetOwnershipDuration);
	Method(ChangeOwnershipDuration);

	Method(GetImprovementDuration);
	Method(SetImprovementDuration);
	Method(ChangeImprovementDuration);

	Method(GetUpgradeProgress);
	Method(GetUpgradeTimeLeft);

	Method(SetUpgradeProgress);
	Method(ChangeUpgradeProgress);

	Method(GetCityRadiusCount);
	Method(IsCityRadius);

	Method(IsStartingPlot);
	Method(SetStartingPlot);
	Method(IsNEOfRiver);
	Method(SetNEOfRiver);
	Method(IsWOfRiver);
	Method(SetWOfRiver);
	Method(IsNWOfRiver);
	Method(SetNWOfRiver);
	Method(GetRiverEFlowDirection);
	Method(GetRiverSEFlowDirection);
	Method(GetRiverSWFlowDirection);

	Method(IsPotentialCityWork);
	Method(IsPotentialCityWorkForArea);

	Method(GetOwner);
	Method(SetOwner);
	Method(GetPlotType);
	Method(IsFlatlands);
	Method(IsHills);
	Method(IsOpenGround);
	Method(IsRoughGround);
	Method(IsMountain);

	Method(SetPlotType);
	Method(GetResourceType);
	Method(GetNonObsoleteResourceType);
	Method(SetResourceType);
	Method(GetNumResource);
	Method(SetNumResource);
	Method(ChangeNumResource);

	Method(GetImprovementType);
	Method(SetImprovementType);
	Method(SetImprovementPillaged);
	Method(GetRouteType);
	Method(SetRouteType);
	Method(IsRoutePillaged);
	Method(SetRoutePillaged);

	Method(GetPlayerThatBuiltImprovement);
	Method(SetPlayerThatBuiltImprovement);
	Method(GetPlayerResponsibleForImprovement);
	Method(SetPlayerResponsibleForImprovement);
	Method(GetPlayerResponsibleForRoute);
	Method(SetPlayerResponsibleForRoute);
	Method(GetPlayerThatClearedBarbCampHere);
	Method(SetPlayerThatClearedBarbCampHere);

	Method(GetPlotCity);
	Method(GetWorkingCity);
	Method(GetWorkingCityOverride);
	Method(GetReconCount);
	Method(GetRiverCrossingCount);
	Method(GetYield);
    Method(ChangeYield);
	Method(CalculateNatureYield);
	Method(CalculateBestNatureYield);
	Method(CalculateTotalBestNatureYield);
	Method(CalculateImprovementYieldChange);
	Method(CalculateYield);
	Method(HasYield);
	Method(GetYieldWithBuild);

	Method(CountNumAirUnits);

	Method(GetFoundValue);
	Method(IsBestAdjacentFound);

	Method(GetPlayerCityRadiusCount);
	Method(IsPlayerCityRadius);

	Method(GetVisibilityCount);
	Method(ChangeVisibilityCount);

	Method(GetRevealedOwner);
	Method(GetRevealedTeam);

	Method(IsRiverCrossing);
	Method(IsRiverCrossingToPlot);

	Method(IsRevealed);
	Method(SetRevealed);
	Method(GetRevealedImprovementType);
	Method(GetRevealedRouteType);
	Method(GetBuildProgress);
	Method(ChangeBuildProgress);

	Method(GetInvisibleVisibilityCount);
	Method(IsInvisibleVisible);
	Method(ChangeInvisibleVisibilityCount);

	Method(GetNumUnits);
	Method(GetUnit);

	Method(GetNumLayerUnits);
	Method(GetLayerUnit);

	Method(GetScriptData);
	Method(SetScriptData);

	Method(GetActiveFogOfWarMode);

	Method(IsImprovementPillaged);
	Method(IsImprovementEmbassy);

	Method(CanSeePlot);

	Method(GetContinentArtType);
	Method(SetContinentArtType);

	Method(IsResourceConnectedByImprovement);

	Method(IsBuildRemovesFeature);

	Method(SetArchaeologicalRecord);
	Method(AddArchaeologicalRecord);
	Method(GetArchaeologyArtifactType);
	Method(GetArchaeologyArtifactEra);
	Method(GetArchaeologyArtifactPlayer1);
	Method(GetArchaeologyArtifactPlayer2);
	Method(GetArchaeologyArtifactWork);
	Method(HasWrittenArtifact);

	Method(GetCityPurchaseID);
	Method(SetCityPurchaseID);
	Method(GetAirUnitsTooltip);

	Method(GetPlannedRouteState);
	Method(SetPlannedRouteState);

	Method(IsMainRoutePlan);
	Method(IsShortcutRoutePlan);
	Method(IsStrategicRoutePlan);

	Method(AddMessage);
	Method(AddPopupMessage);

	Method(IsCivilization);
	Method(HasFeature);
	Method(IsNaturalWonder);
	Method(HasAnyNaturalWonder);
	Method(HasNaturalWonder);
	Method(IsFeatureIce);
	Method(IsFeatureJungle);
	Method(IsFeatureMarsh);
	Method(IsFeatureOasis);
	Method(IsFeatureFloodPlains);
	Method(IsFeatureForest);
	Method(IsFeatureFallout);
	Method(IsFeatureAtoll);
	Method(IsFeatureLake);
	Method(IsFeatureRiver);
	Method(HasImprovement);
	Method(HasPlotType);
	Method(IsPlotMountain);
	Method(IsPlotMountains);
	Method(IsPlotHill);
	Method(IsPlotHills);
	Method(IsPlotLand);
	Method(IsPlotOcean);
	Method(HasResource);
	Method(HasRoute);
	Method(IsRouteRoad);
	Method(IsRouteRailroad);
	Method(HasTerrain);
	Method(IsTerrainGrass);
	Method(IsTerrainPlains);
	Method(IsTerrainDesert);
	Method(IsTerrainTundra);
	Method(IsTerrainSnow);
	Method(IsTerrainCoast);
	Method(IsTerrainOcean);
	Method(IsTerrainMountain);
	Method(IsTerrainMountains);
	Method(IsTerrainHill);
	Method(IsTerrainHills);
	Method(IsAdjacentToFeature);
	Method(IsWithinDistanceOfFeature);
	Method(IsAdjacentToImprovement);
	Method(IsWithinDistanceOfImprovement);
	Method(IsAdjacentToPlotType);
	Method(IsWithinDistanceOfPlotType);
	Method(IsAdjacentToResource);
	Method(IsWithinDistanceOfResource);
	Method(IsAdjacentToTerrain);
	Method(IsWithinDistanceOfTerrain);
	Method(GetEffectiveFlankingBonus);
	Method(GetEffectiveFlankingBonusAtRange);
}
//------------------------------------------------------------------------------
void CvLuaPlot::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}
//------------------------------------------------------------------------------
const char* CvLuaPlot::GetTypeName()
{
	return "Plot";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------
int CvLuaPlot::lCanHaveFeature(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	const int featureType = lua_tointeger(L, 2);
	const bool canHasFeature = pkPlot->canHaveFeature((FeatureTypes)featureType);

	lua_pushboolean(L, canHasFeature);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetFeatureType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	const int featureType = (int)pkPlot->getFeatureType();
	lua_pushinteger(L, featureType);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetTerrainType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	lua_pushinteger(L, pkPlot->getTerrainType());

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetTurnDamage(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	const bool bIgnoreTerrainDamage = lua_toboolean(L, 2);
	const bool bIgnoreFeatureDamage = lua_toboolean(L, 3);
	const bool bExtraTerrainDamage  = lua_toboolean(L, 4);
	const bool bExtraFeatureDamage  = lua_toboolean(L, 5);

	lua_pushinteger(L, pkPlot->getTurnDamage(bIgnoreTerrainDamage, bIgnoreFeatureDamage, bExtraTerrainDamage, bExtraFeatureDamage));

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lIsRiver(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	lua_pushboolean(L, pkPlot->isRiver());

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lIsWater(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	lua_pushboolean(L, pkPlot->isWater());

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lIsBlockaded(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);

	lua_pushboolean(L, pkPlot->isBlockaded(ePlayer));

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lSetFeatureType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const int featureType = lua_tointeger(L, 2);

	pkPlot->setFeatureType((FeatureTypes)featureType);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lSetTerrainType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	const int terrainType = lua_tointeger(L, 2);
	const bool bRecalculate = lua_toboolean(L, 3);
	const bool bRebuildGraphics = lua_toboolean(L, 4);

	pkPlot->setTerrainType((TerrainTypes)terrainType, bRecalculate, bRebuildGraphics);

	return 0;
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// CyPlot methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//bool isNone();
int CvLuaPlot::lIsNone(lua_State* L)
{
	const bool bDoesNotExist = (GetInstance(L, false) == NULL);
	lua_pushboolean(L, bDoesNotExist);

	return 1;
}
//------------------------------------------------------------------------------
//void erase();
int CvLuaPlot::lErase(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::erase);
}

//------------------------------------------------------------------------------
//int getTeam();
int CvLuaPlot::lGetTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getTeam);
}

//------------------------------------------------------------------------------
//void nukeExplosion(int iRange, CyUnit* pNukeUnit);
int CvLuaPlot::lNukeExplosion(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::nukeExplosion);
}

//------------------------------------------------------------------------------
//void updateFog();
int CvLuaPlot::lUpdateFog(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::updateFog);
}

//------------------------------------------------------------------------------
//void updateVisibility();
int CvLuaPlot::lUpdateVisibility(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::updateVisibility);
}
//------------------------------------------------------------------------------
//bool isAdjacentToArea(CyArea* pArea);
int CvLuaPlot::lIsAdjacentToArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvArea* pkArea = CvLuaArea::GetInstance(L, 2);

	const bool bResult = pkPlot->isAdjacentToArea(pkArea);
	lua_pushboolean(L, bResult);

	return 1;
}
//------------------------------------------------------------------------------
//bool isAdjacentToLandmass(int iLandmass);
int CvLuaPlot::lIsAdjacentToLandmass(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentToLandmass);
}
//------------------------------------------------------------------------------
//bool shareAdjacentArea(CyPlot* pPlot);
int CvLuaPlot::lShareAdjacentArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvPlot* pkPlot2 = GetInstance(L, 2);

	const bool bResult = pkPlot->shareAdjacentArea(pkPlot2);
	lua_pushboolean(L, bResult);

	return 1;
}
//------------------------------------------------------------------------------
//bool isAdjacentToLand();
int CvLuaPlot::lIsAdjacentToLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentToLand);
}
//------------------------------------------------------------------------------
//bool IsAdjacentToShallowWater()
int CvLuaPlot::lIsAdjacentToShallowWater(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentToShallowWater);
}
#if defined(MOD_PROMOTIONS_CROSS_ICE)
//------------------------------------------------------------------------------
//bool IsAdjacentToIce()
int CvLuaPlot::lIsAdjacentToIce(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentToIce);
}
#endif
//------------------------------------------------------------------------------
//bool isCoastalLand();
int CvLuaPlot::lIsCoastalLand(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const bool bResult = pkPlot->isCoastalLand(-1, true, false);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer);
int CvLuaPlot::lIsWithinTeamCityRadius(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isWithinTeamCityRadius);
}
//------------------------------------------------------------------------------
//bool isLake();
int CvLuaPlot::lIsLake(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isLake);
}
//------------------------------------------------------------------------------
//bool isFreshWater();
int CvLuaPlot::lIsFreshWater(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isFreshWater);
}

//------------------------------------------------------------------------------
//bool isRiverSide();
int CvLuaPlot::lIsRiverSide(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRiverSide);
}
//------------------------------------------------------------------------------
//bool isRiverConnection(DirectionTypes eDirection);
int CvLuaPlot::lIsRiverConnection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRiverConnection);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lIsRiverCrossingFlowClockwise(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const DirectionTypes eDirection = (DirectionTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlot->isRiverCrossingFlowClockwise(eDirection);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetRiverID();
int CvLuaPlot::lGetRiverID(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetRiverID);
}

//------------------------------------------------------------------------------
//int getNearestLandArea();
int CvLuaPlot::lGetNearestLandArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNearestLandArea);
}
//------------------------------------------------------------------------------
//int seeFromLevel(TeamTypes eTeam);
int CvLuaPlot::lSeeFromLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::seeFromLevel);
}
//------------------------------------------------------------------------------
//CvPlot* getNearestLandPlot();
int CvLuaPlot::lGetNearestLandPlot(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvPlot* pkNearestPlot = pkPlot->getNearestLandPlot();
	CvLuaPlot::Push(L, pkNearestPlot);
	return 1;
}
//------------------------------------------------------------------------------
//int seeThroughLevel();
int CvLuaPlot::lSeeThroughLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::seeThroughLevel);
}
//------------------------------------------------------------------------------
//bool canHaveResource(ResourceTypes eResource, bool bIgnoreLatitude);
int CvLuaPlot::lCanHaveResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::canHaveResource);
}
//------------------------------------------------------------------------------
//bool canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam, bool bPotential);
int CvLuaPlot::lCanHaveImprovement(lua_State* L)
{
	//this is a hack, Lua uses a TeamType and we use it as a PlayerType
	return BasicLuaMethod(L, &CvPlot::canHaveImprovement);
}
//------------------------------------------------------------------------------
//bool canBuild(BuildTypes eBuild, PlayerTypes ePlayer, bool bTestVisible);
int CvLuaPlot::lCanBuild(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::canBuild);
}
//------------------------------------------------------------------------------
//int getBuildTime(BuildTypes eBuild);
int CvLuaPlot::lGetBuildTime(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildTime);
}
//------------------------------------------------------------------------------
//int getBuildTurnsLeft(BuildTypes eBuild, int iNowExtra, int iThenExtra);
int CvLuaPlot::lGetBuildTurnsLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildTurnsLeft);
}
//------------------------------------------------------------------------------
//int getBuildTurnsTotal(BuildTypes eBuild);
int CvLuaPlot::lGetBuildTurnsTotal(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildTurnsTotal);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetBuildTypeNeededToImproveResource(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	ImprovementTypes eImprovement = pkPlot->getImprovementTypeNeededToImproveResource(pkPlot->getOwner(),true,true);

	for (int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);

		if (NULL != pkBuild && pkBuild->getRoute() == NO_ROUTE && eImprovement == (ImprovementTypes)pkBuild->getImprovement())
		{ 
			lua_pushinteger(L, eBuild); 
			return 1;
		}
	}

	lua_pushinteger(L, -1);
	return 1;
}
//------------------------------------------------------------------------------
//int getFeatureProduction(BuildTypes eBuild, TeamTypes eTeam, CyCity* ppCity);
int CvLuaPlot::lGetFeatureProduction(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const BuildTypes eBuild = (BuildTypes)lua_tointeger(L, 2);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 3);
	CvCity* pCity;

	const int iResult = pkPlot->getFeatureProduction(eBuild, ePlayer, &pCity);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//CyUnit* getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, CyUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove);
int CvLuaPlot::lGetBestDefender(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const PlayerTypes eOwner = (PlayerTypes)lua_tointeger(L, 2);
	const PlayerTypes eAttackingPlayer = (PlayerTypes)lua_tointeger(L, 3);
	CvUnit* pkAttacker = CvLuaUnit::GetInstance(L, 4, false);
	const bool bTestAtWar = luaL_optint(L, 5, 0);
	const bool bIgnoreVisibility = luaL_optint(L, 6, 0);
	const bool bTestCanMove = luaL_optint(L, 7, 0);
	CvUnit* pkUnit = pkPlot->getBestDefender(eOwner, eAttackingPlayer, pkAttacker, bTestAtWar, bIgnoreVisibility, bTestCanMove);
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}
//------------------------------------------------------------------------------
//CvUnit* getSelectedUnit();
int CvLuaPlot::lGetSelectedUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvUnit* pkUnit = pkPlot->getSelectedUnit();
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnitPower(PlayerTypes eOwner);
int CvLuaPlot::lGetUnitPower(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getUnitPower);
}

//------------------------------------------------------------------------------
//int defenseModifier(TeamTypes iDefendTeam, bool bIgnoreBuilding, bool bHelp);
int CvLuaPlot::lDefenseModifier(lua_State* L)
{
	return BasicLuaMethod<int,TeamTypes,bool,bool,bool>(L, &CvPlot::defenseModifier);
}
//------------------------------------------------------------------------------
//int movementCost(CyUnit* pUnit, CyPlot* pFromPlot);
int CvLuaPlot::lMovementCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::movementCost);
}

//------------------------------------------------------------------------------
//int CvPlot::GetEffectiveFlankingBonus(CvUnit* pUnit, CvUnit* pOtherUnit, CvPlot* pOtherUnitPlot) const
int CvLuaPlot::lGetEffectiveFlankingBonus(lua_State* L)
{
	CvPlot* pkPlot =  GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L,2);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 3);
	CvPlot* pkOtherPlot =  GetInstance(L, 4);

	int iResult = pkPlot->GetEffectiveFlankingBonus(pkUnit, pkOtherUnit, pkOtherPlot);

	lua_pushinteger(L, iResult);
	return 1;

	//return BasicLuaMethod<int,const CvUnit*,const CvUnit*,const CvPlot*>(L, &CvPlot::GetEffectiveFlankingBonus);
}

//int CvPlot::GetEffectiveFlankingBonusAtRange(const CvUnit* pAttackingUnit, const CvUnit* pDefendingUnit) const
int CvLuaPlot::lGetEffectiveFlankingBonusAtRange(lua_State* L)
{
	CvPlot* pkTargetPlot = GetInstance(L);
	CvUnit* pkAttackingUnit = CvLuaUnit::GetInstance(L,2);
	CvUnit* pkDefendingUnit = CvLuaUnit::GetInstance(L,3);
	
	int iResult = pkTargetPlot->GetEffectiveFlankingBonusAtRange(pkAttackingUnit, pkDefendingUnit);

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int getExtraMovePathCost();
int CvLuaPlot::lGetExtraMovePathCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getExtraMovePathCost);
}
//------------------------------------------------------------------------------
//void changeExtraMovePathCost(int iChange);
int CvLuaPlot::lChangeExtraMovePathCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeExtraMovePathCost);
}

//------------------------------------------------------------------------------
//bool isAdjacentOwned();
int CvLuaPlot::lIsAdjacentOwned(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentOwned);
}
//------------------------------------------------------------------------------
//bool isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly);
int CvLuaPlot::lIsAdjacentPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentPlayer);
}
//------------------------------------------------------------------------------
//bool isAdjacentTeam(TeamTypes eTeam, bool bLandOnly);
int CvLuaPlot::lIsAdjacentTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentTeam);
}
//------------------------------------------------------------------------------
//bool isOwned();
int CvLuaPlot::lIsOwned(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isOwned);
}
//------------------------------------------------------------------------------
//bool isBarbarian();
int CvLuaPlot::lIsBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isBarbarian);
}
//------------------------------------------------------------------------------
//bool isRevealedBarbarian();
int CvLuaPlot::lIsRevealedBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRevealedBarbarian);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lHasBarbarianCamp(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::HasBarbarianCamp);
}

//------------------------------------------------------------------------------
int CvLuaPlot::lHasDig(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::HasDig);
}

//------------------------------------------------------------------------------
//bool isVisible(TeamTypes eTeam, bool bDebug);
int CvLuaPlot::lIsVisible(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	lua_pushboolean(L, pkPlot->isVisible(eTeam, bDebug));
	return 1;
}
//------------------------------------------------------------------------------
//bool isActiveVisible(bool bDebug);
int CvLuaPlot::lIsActiveVisible(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const bool bDebug = luaL_optbool(L, 2, false);

	if (bDebug)
		lua_pushboolean(L, pkPlot->isVisible(GC.getGame().getActiveTeam(), true) );
	else
		lua_pushboolean(L, pkPlot->isActiveVisible());

	return 1;
}
//------------------------------------------------------------------------------
//bool isVisibleToWatchingHuman();
int CvLuaPlot::lIsVisibleToWatchingHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isVisibleToWatchingHuman);
}
//------------------------------------------------------------------------------
//bool isAdjacentVisible(TeamTypes eTeam, bool bDebug);
int CvLuaPlot::lIsAdjacentVisible(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushboolean(L, pkPlot->isAdjacentVisible(eTeam));
	else
		lua_pushboolean(L, pkPlot->isAdjacentVisible(eTeam, true));

	return 1;
}
//------------------------------------------------------------------------------
//bool isAdjacentNonvisible(TeamTypes eTeam);
int CvLuaPlot::lIsAdjacentNonvisible(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentNonvisible);
}
//------------------------------------------------------------------------------
//bool isAdjacentRevealed(TeamTypes eTeam);
int CvLuaPlot::lIsAdjacentRevealed(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentRevealed);
}
//------------------------------------------------------------------------------
//bool isAdjacentNonrevealed(TeamTypes eTeam);
int CvLuaPlot::lIsAdjacentNonrevealed(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isAdjacentNonrevealed);
}

//------------------------------------------------------------------------------
//void removeGoody();
int CvLuaPlot::lRemoveGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::removeGoody);
}
//------------------------------------------------------------------------------
//bool isGoody();
int CvLuaPlot::lIsGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isGoody);
}
//------------------------------------------------------------------------------
//bool isRevealedGoody(TeamTypes eTeam);
int CvLuaPlot::lIsRevealedGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRevealedGoody);
}
//------------------------------------------------------------------------------
//bool IsFriendlyTerritory(PlayerTypes ePlayer);
int CvLuaPlot::lIsFriendlyTerritory(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlot->IsFriendlyTerritory(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool isCity();
int CvLuaPlot::lIsCity(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isCity);
}
//------------------------------------------------------------------------------
//bool isFriendlyCity(CyUnit* pUnit);
int CvLuaPlot::lIsFriendlyCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);

	//ignored
	//const bool bCheckImprovement = lua_toboolean(L, 3);

	const bool bResult = pkPlot->isFriendlyCity(*pkUnit);
	lua_pushboolean(L, bResult);
	return 1;
}
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
//bool isFriendlyCityOrPassableImprovement(CyUnit* pUnit, bool bCheckImprovement);
int CvLuaPlot::lisFriendlyCityOrPassableImprovement(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);
	
	//unused, only for backward compatibility
	//const bool bCheckImprovement = lua_toboolean(L, 3);

	const bool bResult = pkPlot->isCoastalCityOrPassableImprovement(pkUnit->getOwner(), true, true);
	lua_pushboolean(L, bResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool isEnemyCity(CyUnit* pUnit);
int CvLuaPlot::lIsEnemyCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bResult = pkPlot->isEnemyCity(*pkUnit);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isBeingWorked();
int CvLuaPlot::lIsBeingWorked(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isBeingWorked);
}

//------------------------------------------------------------------------------
//bool isUnit();
int CvLuaPlot::lIsUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isUnit);
}
//------------------------------------------------------------------------------
//bool isVisibleEnemyDefender(CyUnit* pUnit);
int CvLuaPlot::lIsVisibleEnemyDefender(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bResult = pkPlot->isVisibleEnemyDefender(pkUnit);
	lua_pushboolean(L, bResult);

	return 1;
}
//------------------------------------------------------------------------------
//int getNumDefenders(PlayerTypes ePlayer);
int CvLuaPlot::lGetNumDefenders(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumDefenders);
}
//------------------------------------------------------------------------------
//int getNumVisibleEnemyDefenders(CyUnit* pUnit);
int CvLuaPlot::lGetNumVisibleEnemyDefenders(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumVisibleEnemyDefenders);
}
//------------------------------------------------------------------------------
//int getNumVisiblePotentialEnemyDefenders(CyUnit* pUnit);
int CvLuaPlot::lGetNumVisiblePotentialEnemyDefenders(lua_State* L)
{
	//not implemented
	lua_pushboolean(L, 0);
	return 1;
}
//------------------------------------------------------------------------------
//bool isVisibleEnemyUnit(PlayerTypes ePlayer);
int CvLuaPlot::lIsVisibleEnemyUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlot->isVisibleEnemyUnit(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isVisibleOtherUnit(PlayerTypes ePlayer);
int CvLuaPlot::lIsVisibleOtherUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isVisibleOtherUnit);
}
//------------------------------------------------------------------------------
//int GetNumFriendlyUnitsOfType(CvUnit* pUnit);
int CvLuaPlot::lGetNumFriendlyUnitsOfType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);

	int iResult = pkUnit->CountStackingUnitsAtPlot(pkPlot);

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetNumFriendlyUnitsOfType(CvUnit* pUnit);
int CvLuaPlot::lgetNumFriendlyUnitsOfType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);

	int iResult = pkUnit->CountStackingUnitsAtPlot(pkPlot);

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool isFighting();
int CvLuaPlot::lIsFighting(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isFighting);
}
#if defined(MOD_GLOBAL_STACKING_RULES)
//------------------------------------------------------------------------------
//int getUnitLimit();
int CvLuaPlot::lGetUnitLimit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getUnitLimit);
}
#endif
//------------------------------------------------------------------------------
//bool isRoute();
int CvLuaPlot::lIsRoute(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRoute);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lIsTradeRoute(lua_State* L)
{
	// Duplicate of IsCityConnection() kept for backwards compatibility
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 2, -1);
	const bool bIndustrial = luaL_optbool(L, 3, false);
	const bool bResult = pkPlot->IsCityConnection(ePlayer, bIndustrial);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lIsCityConnection(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 2, -1);
	const bool bIndustrial = luaL_optbool(L, 3, false);
	const bool bResult = pkPlot->IsCityConnection(ePlayer, bIndustrial);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool isImpassable();
int CvLuaPlot::lIsImpassable(lua_State* L)
{
#if defined(MOD_BALANCE_CORE)
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	TeamTypes eTeam = (TeamTypes) luaL_optinteger(L, 2, NO_TEAM);
	const bool bResult = pkPlot->isImpassable(eTeam);
	lua_pushboolean(L, bResult);
	return 1;
#else
	return BasicLuaMethod(L, &CvPlot::isImpassable);
#endif
}

//------------------------------------------------------------------------------
//int getX();
int CvLuaPlot::lGetX(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getX);
}
//------------------------------------------------------------------------------
//int getY();
int CvLuaPlot::lGetY(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getY);
}
//------------------------------------------------------------------------------
//int getIndex();
int CvLuaPlot::lGetIndex(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlotIndex);
}
//------------------------------------------------------------------------------
//bool at(int iX, int iY);
int CvLuaPlot::lAt(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::at);
}
//------------------------------------------------------------------------------
//bool GetIndex();
int CvLuaPlot::lGetPlotIndex(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlotIndex);
}
//------------------------------------------------------------------------------
//int getLatitude();
int CvLuaPlot::lGetLatitude(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getLatitude);
}
//------------------------------------------------------------------------------
//CyArea* area();
int CvLuaPlot::lArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvArea* pkArea = pkPlot->area();
	CvLuaArea::Push(L, pkArea);
	return 1;
}
//------------------------------------------------------------------------------
//CyArea* waterArea();
int CvLuaPlot::lWaterArea(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvLuaArea::Push(L, pkPlot->GetLargestAdjacentWaterArea());
	return 1;
}
//------------------------------------------------------------------------------
//int getArea();
int CvLuaPlot::lGetArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getArea);
}
//------------------------------------------------------------------------------
//int setArea();
int CvLuaPlot::lSetArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setArea);
}
//------------------------------------------------------------------------------
//int getLandmass();
int CvLuaPlot::lGetLandmass(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getLandmass);
}
//------------------------------------------------------------------------------
//int getFeatureVariety();
int CvLuaPlot::lGetFeatureVariety(lua_State* L)
{
	lua_pushinteger(L, 0);
	return 1;
}

//------------------------------------------------------------------------------
//int getOwnershipDuration();
int CvLuaPlot::lGetOwnershipDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getOwnershipDuration);
}
//------------------------------------------------------------------------------
//bool isOwnershipScore();
int CvLuaPlot::lIsOwnershipScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isOwnershipScore);
}
//------------------------------------------------------------------------------
//void setOwnershipDuration(int iNewValue);
int CvLuaPlot::lSetOwnershipDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setOwnershipDuration);
}
//------------------------------------------------------------------------------
//void changeOwnershipDuration(int iChange);
int CvLuaPlot::lChangeOwnershipDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeOwnershipDuration);
}
//------------------------------------------------------------------------------
//plot GetInlandCorner()
int CvLuaPlot::lGetInlandCorner(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvPlot* pkInlandCorner = pkPlot->getInlandCorner();

	CvLuaPlot::Push(L, pkInlandCorner);
	return 1;
}
//------------------------------------------------------------------------------
//int getImprovementDuration();
int CvLuaPlot::lGetImprovementDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getImprovementDuration);
}
//------------------------------------------------------------------------------
//void setImprovementDuration(int iNewValue);
int CvLuaPlot::lSetImprovementDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setImprovementDuration);
}
//------------------------------------------------------------------------------
//void changeImprovementDuration(int iChange);
int CvLuaPlot::lChangeImprovementDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeImprovementDuration);
}

//------------------------------------------------------------------------------
//int getUpgradeProgress();
int CvLuaPlot::lGetUpgradeProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getUpgradeProgress);
}
//------------------------------------------------------------------------------
//int getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer);
int CvLuaPlot::lGetUpgradeTimeLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getUpgradeTimeLeft);
}

//------------------------------------------------------------------------------
//void setUpgradeProgress(int iNewValue);
int CvLuaPlot::lSetUpgradeProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setUpgradeProgress);
}
//------------------------------------------------------------------------------
//void changeUpgradeProgress(int iChange);
int CvLuaPlot::lChangeUpgradeProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeUpgradeProgress);
}
//------------------------------------------------------------------------------
//int getCityRadiusCount();
int CvLuaPlot::lGetCityRadiusCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getCityRadiusCount);
}
//------------------------------------------------------------------------------
//int isCityRadius();
int CvLuaPlot::lIsCityRadius(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isCityRadius);
}

//------------------------------------------------------------------------------
//bool isStartingPlot();
int CvLuaPlot::lIsStartingPlot(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isStartingPlot);
}
//------------------------------------------------------------------------------
//void setStartingPlot(bool bNewValue);
int CvLuaPlot::lSetStartingPlot(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setStartingPlot);
}
//------------------------------------------------------------------------------
//bool isNEOfRiver();
int CvLuaPlot::lIsNEOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isNEOfRiver);
}
//------------------------------------------------------------------------------
//void setNEOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir);
int CvLuaPlot::lSetNEOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setNEOfRiver);
}
//------------------------------------------------------------------------------
//bool isWOfRiver();
int CvLuaPlot::lIsWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isWOfRiver);
}
//------------------------------------------------------------------------------
//void setWOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir);
int CvLuaPlot::lSetWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setWOfRiver);
}
//------------------------------------------------------------------------------
//bool isNWOfRiver();
int CvLuaPlot::lIsNWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isNWOfRiver);
}
//------------------------------------------------------------------------------
//void setNWOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir);
int CvLuaPlot::lSetNWOfRiver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setNWOfRiver);
}
//------------------------------------------------------------------------------
//FlowDirectionTypes getRiverEFlowDirection();
int CvLuaPlot::lGetRiverEFlowDirection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverEFlowDirection);
}
//------------------------------------------------------------------------------
//FlowDirectionTypes getRiverSEFlowDirection();
int CvLuaPlot::lGetRiverSEFlowDirection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverSEFlowDirection);
}
//------------------------------------------------------------------------------
//FlowDirectionTypes getRiverSWFlowDirection();
int CvLuaPlot::lGetRiverSWFlowDirection(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverSWFlowDirection);
}

//------------------------------------------------------------------------------
//bool isPotentialCityWork();
int CvLuaPlot::lIsPotentialCityWork(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isPotentialCityWork);
}
//------------------------------------------------------------------------------
//bool isPotentialCityWorkForArea(CyArea* pArea);
int CvLuaPlot::lIsPotentialCityWorkForArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isPotentialCityWorkForArea);
}

//------------------------------------------------------------------------------
//PlayerTypes getOwner();
int CvLuaPlot::lGetOwner(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const int iPlayer  = pkPlot->getOwner();
	const int iCity = pkPlot->getOwningCityID();

	lua_pushinteger(L, iPlayer);
	lua_pushinteger(L, iCity);
	return 2;
}
//------------------------------------------------------------------------------
//void setOwner(PlayerTypes eNewValue, int iAcquiringCityID, bool bCheckUnits = true, bool bUpdateResources = true);
int CvLuaPlot::lSetOwner(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const PlayerTypes eNewValue = (PlayerTypes)lua_tointeger(L, 2);
	const int iAcquiringCityID = lua_tointeger(L, 3);
	const bool bCheckUnits = luaL_optbool(L, 4, true);

	pkPlot->setOwner(eNewValue, iAcquiringCityID, bCheckUnits);
	return 1;
}
//------------------------------------------------------------------------------
//PlotTypes getPlotType();
int CvLuaPlot::lGetPlotType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getPlotType);
}
//------------------------------------------------------------------------------
//bool isFlatlands();
int CvLuaPlot::lIsFlatlands(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isFlatlands);
}
//------------------------------------------------------------------------------
//bool isHills();
int CvLuaPlot::lIsHills(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isHills);
}

//------------------------------------------------------------------------------
//bool isOpenGround();
int CvLuaPlot::lIsOpenGround(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isOpenGround);
}
//------------------------------------------------------------------------------
//bool isRoughGround();
int CvLuaPlot::lIsRoughGround(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRoughGround);
}
//------------------------------------------------------------------------------
//bool isMountain();
int CvLuaPlot::lIsMountain(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isMountain);
}

//------------------------------------------------------------------------------
//void setPlotType(PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics, bool bEraseUnitsIfWater);
int CvLuaPlot::lSetPlotType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setPlotType);
}
//------------------------------------------------------------------------------
//ResourceTypes getResourceType(TeamTypes eTeam);
int CvLuaPlot::lGetResourceType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	if(pkPlot != NULL)
	{
		if(lua_gettop(L) >= 2)
		{
			TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
			ResourceTypes eResource = pkPlot->getResourceType(eTeam);
			lua_pushinteger(L, eResource);
			return 1;
		}
		else
		{
			ResourceTypes eResource = pkPlot->getResourceType(NO_TEAM);
			lua_pushinteger(L, eResource);
			return 1;
		}
	}
	else
	{
		lua_pushinteger(L, -1);
		return 1;
	}
}
//------------------------------------------------------------------------------
//ResourceTypes getNonObsoleteResourceType(TeamTypes eTeam);
int CvLuaPlot::lGetNonObsoleteResourceType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNonObsoleteResourceType);
}
//------------------------------------------------------------------------------
//void setResourceType(ResourceTypes eNewValue, int iNumResource);
int CvLuaPlot::lSetResourceType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setResourceType);
}
//------------------------------------------------------------------------------
//int getNumResource();
int CvLuaPlot::lGetNumResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumResource);
}
//------------------------------------------------------------------------------
//void setNumResource(int iNum);
int CvLuaPlot::lSetNumResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setNumResource);
}
//------------------------------------------------------------------------------
//void changeNumResource(int iChange);
int CvLuaPlot::lChangeNumResource(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeNumResource);
}

//------------------------------------------------------------------------------
//ImprovementTypes getImprovementType();
int CvLuaPlot::lGetImprovementType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getImprovementType);
}
//------------------------------------------------------------------------------
//void setImprovementType(ImprovementTypes eNewValue);
int CvLuaPlot::lSetImprovementType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setImprovementType);
}
//------------------------------------------------------------------------------
//void setImprovementType(bool b);
int CvLuaPlot::lSetImprovementPillaged(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetImprovementPillaged);
}
//------------------------------------------------------------------------------
//RouteTypes getRouteType();
int CvLuaPlot::lGetRouteType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRouteType);
}
//------------------------------------------------------------------------------
//void setRouteType(RouteTypes eNewValue);
int CvLuaPlot::lSetRouteType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::setRouteType);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lIsRoutePillaged(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	lua_pushboolean(L, pkPlot->IsRoutePillaged());
	return 1;
}
//------------------------------------------------------------------------------
//void setRoutePillaged(bool b);
int CvLuaPlot::lSetRoutePillaged(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetRoutePillaged);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetPlayerThatBuiltImprovement(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlayerThatBuiltImprovement);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lSetPlayerThatBuiltImprovement(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetPlayerThatBuiltImprovement);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetPlayerResponsibleForImprovement(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlayerResponsibleForImprovement);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lSetPlayerResponsibleForImprovement(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetPlayerResponsibleForImprovement);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetPlayerResponsibleForRoute(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlayerResponsibleForRoute);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lSetPlayerResponsibleForRoute(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetPlayerResponsibleForRoute);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lGetPlayerThatClearedBarbCampHere(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlayerThatClearedBarbCampHere);
}
//------------------------------------------------------------------------------
int CvLuaPlot::lSetPlayerThatClearedBarbCampHere(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetPlayerThatClearedBarbCampHere);
}
//------------------------------------------------------------------------------
//CyCity* getPlotCity();
int CvLuaPlot::lGetPlotCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvCity* pkCity = pkPlot->getPlotCity();
	CvLuaCity::Push(L, pkCity);
	return 1;
}
//------------------------------------------------------------------------------
//CyCity* getOwningCity();
int CvLuaPlot::lGetWorkingCity(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	CvCity* pkCity = pkPlot->getEffectiveOwningCity();
	CvLuaCity::Push(L, pkCity);
	return 1;
}
//------------------------------------------------------------------------------
//CyCity* getOwningCityOverride();
int CvLuaPlot::lGetWorkingCityOverride(lua_State* L)
{
	//CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	//CvCity* pkCity = pkPlot->getOwningCityOverride();

	//working city already considers override
	CvLuaCity::Push(L, NULL);
	return 1;
}
//------------------------------------------------------------------------------
//int getReconCount();
int CvLuaPlot::lGetReconCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getReconCount);
}
//------------------------------------------------------------------------------
//int getRiverCrossingCount();
int CvLuaPlot::lGetRiverCrossingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getRiverCrossingCount);
}
//------------------------------------------------------------------------------
//int getYield(YieldTypes eIndex);
int CvLuaPlot::lGetYield(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkPlot->getYield(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void changeYield(YieldTypes eYield, int iChange);
int CvLuaPlot::lChangeYield(lua_State* L)
{
    return BasicLuaMethod(L, &CvPlot::changeYield);
}
//------------------------------------------------------------------------------
//int calculateNatureYield(YieldTypes eIndex, TeamTypes eTeam, bool bIgnoreFeature = false);
int CvLuaPlot::lCalculateNatureYield(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlot->calculateNatureYield(eIndex, ePlayer, pkPlot->getFeatureType(), pkPlot->getResourceType(GET_PLAYER(ePlayer).getTeam()), pkPlot->getPlotCity());
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam);
int CvLuaPlot::lCalculateBestNatureYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::calculateBestNatureYield);
}
//------------------------------------------------------------------------------
//int calculateTotalBestNatureYield(TeamTypes eTeam);
int CvLuaPlot::lCalculateTotalBestNatureYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::calculateTotalBestNatureYield);
}
//------------------------------------------------------------------------------
//int calculateImprovementYieldChange(YieldTypes eYield, PlayerTypes ePlayer, ImprovementTypes eImprovement, bool bOptimal, RouteTypes eAssumeThisRoute);
int CvLuaPlot::lCalculateImprovementYieldChange(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const ImprovementTypes eImprovement = (ImprovementTypes)lua_tointeger(L, 2);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L,3);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 4);
	const bool bOptimal = luaL_optbool(L, 5, false);

	RouteTypes eRoute = (RouteTypes)luaL_optint(L, 6, NUM_ROUTE_TYPES);
	if (lua_gettop(L) == 6)
		eRoute = (RouteTypes)lua_tointeger(L, 6);

	if (eRoute == NUM_ROUTE_TYPES)
		eRoute = pkPlot->getRouteType();
	if (pkPlot->IsRoutePillaged())
		eRoute = NO_ROUTE;

	const int iResult = pkPlot->calculateImprovementYield(eYield, ePlayer, eImprovement, eRoute, pkPlot->getFeatureType(), pkPlot->getResourceType(GET_PLAYER(ePlayer).getTeam()), false, pkPlot->getEffectiveOwningCity(), bOptimal);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int calculateYield(YieldTypes eIndex, bool bDisplay = false, const CvCity* pOwningCity = NULL);
int CvLuaPlot::lCalculateYield(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const YieldTypes eYield = CvLuaArgs::toValue<YieldTypes>(L, 2);
	const bool bDisplay = luaL_optbool(L, 3, false);
	const int iResult = pkPlot->calculateYield(eYield, bDisplay);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool hasYield();
int CvLuaPlot::lHasYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::hasYield);
}
//------------------------------------------------------------------------------
//bool GetYieldWithBuild();
int CvLuaPlot::lGetYieldWithBuild(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const BuildTypes eBuild = (BuildTypes)lua_tointeger(L, 2);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 3);
	const bool bUpgrade = luaL_optbool(L, 4, false);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 5);

	const CvCity* pOwningCity = pkPlot->getEffectiveOwningCity();
	if (pOwningCity)
	{
		ReligionTypes eMajority = pOwningCity->GetCityReligions()->GetReligiousMajority();
		BeliefTypes eSecondaryPantheon = pOwningCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();

		const CvReligion* pReligion = (eMajority != NO_RELIGION) ? GC.getGame().GetGameReligions()->GetReligion(eMajority, pOwningCity->getOwner()) : 0;
		const CvBeliefEntry* pBelief = (eSecondaryPantheon != NO_BELIEF) ? GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon) : 0;
		int iResult = pkPlot->getYieldWithBuild(eBuild, eYield, bUpgrade, false, ePlayer, pOwningCity, pReligion, pBelief);
#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
		// Mod for civs keeping their pantheon belief forever
		if (MOD_RELIGION_PERMANENT_PANTHEON)
		{
			if (GC.getGame().GetGameReligions()->HasCreatedPantheon(pOwningCity->getOwner()))
			{
				const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, pOwningCity->getOwner());
				BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(pOwningCity->getOwner());
				if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
				{
					if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eMajority, pOwningCity->getOwner()))) // check that the our religion does not have our belief, to prevent double counting
					{
						iResult += pkPlot->getYieldWithBuild(eBuild, eYield, bUpgrade, false, ePlayer, pOwningCity, pPantheon, NULL);
					}
				}
			}
		}
#endif
		lua_pushinteger(L, iResult);
		return 1;
	}
	else
	{
		const int iResult = pkPlot->getYieldWithBuild(eBuild, eYield, bUpgrade, false, ePlayer, NULL, NULL, NULL);
		lua_pushinteger(L, iResult);
		return 1;
	}
}
//------------------------------------------------------------------------------
//int countNumAirUnits(TeamTypes ePlayer);
int CvLuaPlot::lCountNumAirUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::countNumAirUnits);
}
//------------------------------------------------------------------------------
//int getFoundValue(PlayerTypes eIndex);
int CvLuaPlot::lGetFoundValue(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getFoundValue);
}
//------------------------------------------------------------------------------
//bool isBestAdjacentFound(PlayerTypes eIndex);
int CvLuaPlot::lIsBestAdjacentFound(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isBestAdjacentFoundValue);
}
//------------------------------------------------------------------------------
//int getPlayerCityRadiusCount(PlayerTypes eIndex);
int CvLuaPlot::lGetPlayerCityRadiusCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getPlayerCityRadiusCount);
}
//------------------------------------------------------------------------------
//bool isPlayerCityRadius(PlayerTypes eIndex);
int CvLuaPlot::lIsPlayerCityRadius(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isPlayerCityRadius);
}
//------------------------------------------------------------------------------
//int getVisibilityCount(TeamTypes eTeam);
int CvLuaPlot::lGetVisibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getVisibilityCount);
}
//------------------------------------------------------------------------------
//void changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisibleType, bool bInformExplorationTracking, bool bAlwaysSeeInvisible);
int CvLuaPlot::lChangeVisibilityCount(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const int iChange = lua_tointeger(L, 3);
	const int eSeeInvisible = lua_tointeger(L, 4);
	const bool bInformExplorationTracking = lua_toboolean(L, 5);
	const bool bAlwaysSeeInvisible = lua_toboolean(L, 6);

	if(lua_gettop(L) >= 7)
	{
		CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 7);
		pkPlot->changeVisibilityCount(eTeam, iChange, static_cast<InvisibleTypes>(eSeeInvisible), bInformExplorationTracking, bAlwaysSeeInvisible, pkUnit);
	}
	else
	{
		pkPlot->changeVisibilityCount(eTeam, iChange, static_cast<InvisibleTypes>(eSeeInvisible), bInformExplorationTracking, bAlwaysSeeInvisible);
	}

	return 0;
}
//------------------------------------------------------------------------------
//PlayerTypes getRevealedOwner(TeamTypes eTeam, bool bDebug);
int CvLuaPlot::lGetRevealedOwner(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedOwner(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedOwner(eTeam, true));

	return 1;
}
//------------------------------------------------------------------------------
//TeamTypes getRevealedTeam(TeamTypes eTeam, bool bDebug);
int CvLuaPlot::lGetRevealedTeam(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedTeam(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedTeam(eTeam, true));

	return 1;
}
//------------------------------------------------------------------------------
//bool isRiverCrossing(DirectionTypes eIndex);
int CvLuaPlot::lIsRiverCrossing(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isRiverCrossing);
}
//------------------------------------------------------------------------------
//bool IsRiverCrossingToPlot(DirectionTypes eIndex);
int CvLuaPlot::lIsRiverCrossingToPlot(lua_State* L)
{
	bool bIsCrossingRiver = false;
	CvPlot* pkMyPlot = GetInstance(L);
	CvPlot* pkOtherPlot = GetInstance(L, 2);

	if(pkMyPlot->isAdjacent(pkOtherPlot) && pkMyPlot->isRiverCrossing(directionXY(pkMyPlot, pkOtherPlot)))
	{
		bIsCrossingRiver = true;
	}

	lua_pushboolean(L, bIsCrossingRiver);
	return 1;
}
//------------------------------------------------------------------------------
//bool isRevealed(TeamTypes eTeam, bool bDebug);
int CvLuaPlot::lIsRevealed(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushboolean(L, pkPlot->isRevealed(eTeam));
	else
		lua_pushboolean(L, pkPlot->isRevealed(eTeam, true));

	return 1;
}
//------------------------------------------------------------------------------
//void setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly, TeamTypes eFromTeam);
int CvLuaPlot::lSetRevealed(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bNewValue = lua_toboolean(L, 3);
	const bool bTerrainOnly = luaL_optint(L, 4, 0);
	const TeamTypes eFromTeam = (TeamTypes)luaL_optint(L, 5, NO_TEAM);
	if(lua_gettop(L) >= 6)
	{
		CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 6);
		pkPlot->setRevealed(eTeam, bNewValue, pkUnit, bTerrainOnly, eFromTeam);
	}
	else
	{
		pkPlot->setRevealed(eTeam, bNewValue, NULL, bTerrainOnly, eFromTeam);
	}

	return 0;
}
//------------------------------------------------------------------------------
//ImprovementTypes getRevealedImprovementType(TeamTypes eTeam, bool bDebug);
int CvLuaPlot::lGetRevealedImprovementType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedImprovementType(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedImprovementType(eTeam, true));

	return 1;
}
//------------------------------------------------------------------------------
//RouteTypes getRevealedRouteType(TeamTypes eTeam, bool bDebug);
int CvLuaPlot::lGetRevealedRouteType(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = luaL_optbool(L, 3, false);

	if (!bDebug)
		lua_pushinteger(L, pkPlot->getRevealedRouteType(eTeam));
	else
		lua_pushinteger(L, pkPlot->getRevealedRouteType(eTeam, true));

	return 1;
}
//------------------------------------------------------------------------------
//int getBuildProgress(BuildTypes eBuild);
int CvLuaPlot::lGetBuildProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getBuildProgress);
}
//------------------------------------------------------------------------------
//bool changeBuildProgress(BuildTypes eBuild, int iChange, TeamTypes eTeam);
int CvLuaPlot::lChangeBuildProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeBuildProgress);
}
//------------------------------------------------------------------------------
//int getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible);
int CvLuaPlot::lGetInvisibleVisibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getInvisibleVisibilityCount);
}
//------------------------------------------------------------------------------
//bool isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible);
int CvLuaPlot::lIsInvisibleVisible(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::isInvisibleVisible);
}
//------------------------------------------------------------------------------
//void changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange);
int CvLuaPlot::lChangeInvisibleVisibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::changeInvisibleVisibilityCount);
}
//------------------------------------------------------------------------------
//int getNumUnits();
int CvLuaPlot::lGetNumUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getNumUnits);
}
//------------------------------------------------------------------------------
//CvUnit* getUnit(int iIndex);
int CvLuaPlot::lGetUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const int iIndex = lua_tointeger(L, 2);

	CvUnit* pkUnit = pkPlot->getUnitByIndex(iIndex);
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumLayerUnits();
int CvLuaPlot::lGetNumLayerUnits(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const int iLayerID = luaL_optinteger(L, 2, -1);

	lua_pushinteger(L, pkPlot->getNumLayerUnits(iLayerID));
	return 1;
}
//------------------------------------------------------------------------------
//CvUnit* getUnit(int iIndex);
int CvLuaPlot::lGetLayerUnit(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const int iIndex = lua_tointeger(L, 2);
	const int iLayerID = luaL_optinteger(L, 3, -1);

	CvUnit* pkUnit = pkPlot->getLayerUnit(iIndex, iLayerID);
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}
//------------------------------------------------------------------------------
//string getScriptData();
int CvLuaPlot::lGetScriptData(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const CvString strResult = pkPlot->getScriptData();
	lua_pushstring(L, strResult.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//void setScriptData(string szNewValue);
int CvLuaPlot::lSetScriptData(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);
	const char* strNewValue = lua_tostring(L, 2);

	pkPlot->setScriptData(strNewValue);
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lGetActiveFogOfWarMode(lua_State* L)
{
	CvPlot* pkPlot = GetInstance(L); CHECK_PLOT_VALID(pkPlot);

	int fow = 0;
	// convert from game fog of war defines to engine fog of war defines
	switch(pkPlot->GetActiveFogOfWarMode())
	{
	case FOGOFWARMODE_OFF:
		fow = 2;
	case FOGOFWARMODE_UNEXPLORED:
		fow = 0;
	case FOGOFWARMODE_NOVIS:
		fow = 1;
	}

	lua_pushinteger(L, fow);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsImprovementPillaged() const
int CvLuaPlot::lIsImprovementPillaged(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::IsImprovementPillaged);
}

//------------------------------------------------------------------------------
int CvLuaPlot::lIsImprovementEmbassy(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::IsImprovementEmbassy);
}

//------------------------------------------------------------------------------
//bool CvPlot::canSeePlot(CvPlot *pPlot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection)
int CvLuaPlot::lCanSeePlot(lua_State* L)
{
	CvPlot* pkThisPlot = GetInstance(L);
	CvPlot* pkThatPlot = GetInstance(L, 2);
	TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 3);
	int iRange = lua_tointeger(L, 4);
	DirectionTypes eFacingDirection = (DirectionTypes) lua_tointeger(L, 5);

	bool bCanSee = false;
	if(pkThisPlot)
	{
		//need to add one to the range to maintain backward compatibility
		bCanSee = pkThisPlot->canSeePlot(pkThatPlot, eTeam, iRange + 1, eFacingDirection);
	}

	lua_pushboolean(L, bCanSee);
	return 1;

}

//------------------------------------------------------------------------------
int CvLuaPlot::lGetContinentArtType(lua_State* L)
{
	CvPlot* pkThisPlot = GetInstance(L);
	if(pkThisPlot)
	{
		lua_pushinteger(L, pkThisPlot->GetContinentType());
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lSetContinentArtType(lua_State* L)
{
	CvPlot* pkThisPlot = GetInstance(L);
	if(pkThisPlot)
	{
		const int iContinentType = lua_tointeger(L, 2);
		pkThisPlot->SetContinentType((char) iContinentType);
	}
	return 0;
}

//------------------------------------------------------------------------------
//bool CvPlot::IsResourceConnectedByImprovement(iImprovement)
int CvLuaPlot::lIsResourceConnectedByImprovement(lua_State* L)
{
	bool bResult = false;
	CvPlot* kPlot = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes) lua_tointeger(L, 2);
	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	if(pkImprovementInfo)
	{
		bResult = pkImprovementInfo->IsConnectsResource(kPlot->getResourceType(GC.getGame().getActiveTeam()));
	}

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lIsBuildRemovesFeature(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	const BuildTypes eBuild = (BuildTypes) lua_tointeger(L, 2);
	bool bFeatureRemoved = false;

	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if(NULL != pkBuildInfo && kPlot->getFeatureType() != NO_FEATURE)
		bFeatureRemoved = pkBuildInfo->isFeatureRemove(kPlot->getFeatureType());

	lua_pushboolean(L, bFeatureRemoved);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lSetArchaeologicalRecord(lua_State* L)
{
	CvPlot* pPlot = GetInstance(L);
	GreatWorkArtifactClass eType = CvLuaArgs::toValue<GreatWorkArtifactClass>(L, 2);
	EraTypes eEra = CvLuaArgs::toValue<EraTypes>(L, 3);
	PlayerTypes ePlayer1 = CvLuaArgs::toValue<PlayerTypes>(L, 4);
	PlayerTypes ePlayer2 = (PlayerTypes) luaL_optinteger(L, 5, NO_PLAYER);
	
	pPlot->SetArchaeologicalRecord(eType, eEra, ePlayer1, ePlayer2);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lAddArchaeologicalRecord(lua_State* L)
{
	CvPlot* pPlot = GetInstance(L);
	GreatWorkArtifactClass eType = CvLuaArgs::toValue<GreatWorkArtifactClass>(L, 2);
	EraTypes eEra = CvLuaArgs::toValue<EraTypes>(L, 3);
	PlayerTypes ePlayer1 = CvLuaArgs::toValue<PlayerTypes>(L, 4);
	PlayerTypes ePlayer2 = (PlayerTypes) luaL_optinteger(L, 5, NO_PLAYER);
	
	pPlot->AddArchaeologicalRecord(eType, eEra, ePlayer1, ePlayer2);
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lGetArchaeologyArtifactType(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iType = kPlot->GetArchaeologicalRecord().m_eArtifactType;
	lua_pushinteger(L, iType);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lGetArchaeologyArtifactEra(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iEra = kPlot->GetArchaeologicalRecord().m_eEra;
	lua_pushinteger(L, iEra);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lGetArchaeologyArtifactPlayer1(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iPlayer = kPlot->GetArchaeologicalRecord().m_ePlayer1;
	lua_pushinteger(L, iPlayer);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lGetArchaeologyArtifactPlayer2(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iPlayer = kPlot->GetArchaeologicalRecord().m_ePlayer2;
	if (iPlayer == -1)
		iPlayer = (int)BARBARIAN_PLAYER;
	lua_pushinteger(L, iPlayer);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lGetArchaeologyArtifactWork(lua_State* L)
{
	CvPlot* kPlot = GetInstance(L);
	int iWork = (int)kPlot->GetArchaeologicalRecord().m_eWork;
	lua_pushinteger(L, iWork);
	return 1;
}

//------------------------------------------------------------------------------
//bool HasWrittenArtifact();
int CvLuaPlot::lHasWrittenArtifact(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::HasWrittenArtifact);
}

//------------------------------------------------------------------------------
//int GetCityPurchaseID();
int CvLuaPlot::lGetCityPurchaseID(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::getOwningCityID);
}

//------------------------------------------------------------------------------
//void SetCityPurchaseID(int ID);
int CvLuaPlot::lSetCityPurchaseID(lua_State* L)
{
	const int idx = GetStartingArgIndex();
	CvPlot* kPlot = GetInstance(L);
	const int iID = lua_tointeger(L, idx);
	//we assume the plot owner and the city owner are the same!
	kPlot->setOwningCity(kPlot->getOwner(), iID);
	return 0;
}

int CvLuaPlot::lGetAirUnitsTooltip(lua_State* L)
{
	CvString AirTT = "";
	int iNumAirUnits = 0;
	CvPlot* kPlot = GetInstance(L);

	CvString tempAirTT = "";
	if (kPlot != NULL)
	{
		for (int iUnitLoop = 0; iUnitLoop < kPlot->getNumUnits(); iUnitLoop++)
		{
			CvUnit* pAirUnit = kPlot->getUnitByIndex(iUnitLoop);
			if (pAirUnit == NULL)
				continue;

			if (pAirUnit->getDomainType() != DOMAIN_AIR)
				continue;

			iNumAirUnits++;

			if (!tempAirTT.IsEmpty())
				tempAirTT += "[NEWLINE]";

			tempAirTT += "[ICON_BULLET] ";
			tempAirTT += pAirUnit->getName();
		}
	}

	if (iNumAirUnits > 0)
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_STATIONED_AIRCRAFT");
		localizedText << iNumAirUnits;
		const char* const localized = localizedText.toUTF8();
		AirTT = localized;
		AirTT += "[NEWLINE]";
		AirTT += tempAirTT;
	}

	lua_pushstring(L, AirTT.c_str());
	return 1;
}

//------------------------------------------------------------------------------
//void GetPlannedRouteState(PlayerTypes ePlayer);
int CvLuaPlot::lGetPlannedRouteState(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::GetPlannedRouteState);
}

//------------------------------------------------------------------------------
//void SetPlannedRouteState(PlayerTypes ePlayer, RoutePlanTypes eRoutePlanType);
int CvLuaPlot::lSetPlannedRouteState(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlot::SetPlannedRouteState);
}

//------------------------------------------------------------------------------
//bool IsMainRouteTile(PlayerTypes ePlayer);
int CvLuaPlot::lIsMainRoutePlan(lua_State* L)
{
	CvPlot* pPlot = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);

	bool bResult = GET_PLAYER(ePlayer).GetBuilderTaskingAI()->IsMainRoutePlot(pPlot);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool IsShortcutRouteTile(PlayerTypes ePlayer);
int CvLuaPlot::lIsShortcutRoutePlan(lua_State* L)
{
	CvPlot* pPlot = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);

	bool bResult = GET_PLAYER(ePlayer).GetBuilderTaskingAI()->IsShortcutRoutePlot(pPlot);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool IsStrategicRouteTile(PlayerTypes ePlayer);
int CvLuaPlot::lIsStrategicRoutePlan(lua_State* L)
{
	CvPlot* pPlot = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);

	bool bResult = GET_PLAYER(ePlayer).GetBuilderTaskingAI()->IsStrategicRoutePlot(pPlot);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlot::lAddMessage(lua_State* L)
{
	CvPlot* pPlot = GetInstance(L);
	const char* szMessage = lua_tostring(L, 2);
	const PlayerTypes ePlayer = (PlayerTypes) luaL_optinteger(L, 3, GC.getGame().getActivePlayer());

	SHOW_PLOT_MESSAGE(pPlot, ePlayer, szMessage);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlot::lAddPopupMessage(lua_State* L)
{
	CvPlot* pPlot = GetInstance(L);
	const char* szMessage = lua_tostring(L, 2);
	const float fDelay = (float) luaL_optnumber(L, 3, 0.0);
	const PlayerTypes ePlayer = (PlayerTypes) luaL_optinteger(L, 4, GC.getGame().getActivePlayer());

	SHOW_PLOT_POPUP(pPlot, ePlayer, szMessage);
	return 0;
}

LUAAPIIMPL(Plot, IsCivilization)
LUAAPIIMPL(Plot, HasFeature)
LUAAPIIMPL(Plot, IsNaturalWonder)
LUAAPIIMPL(Plot, HasAnyNaturalWonder)
LUAAPIIMPL(Plot, HasNaturalWonder)
LUAAPIIMPL(Plot, IsFeatureIce)
LUAAPIIMPL(Plot, IsFeatureJungle)
LUAAPIIMPL(Plot, IsFeatureMarsh)
LUAAPIIMPL(Plot, IsFeatureOasis)
LUAAPIIMPL(Plot, IsFeatureFloodPlains)
LUAAPIIMPL(Plot, IsFeatureForest)
LUAAPIIMPL(Plot, IsFeatureFallout)
LUAAPIIMPL(Plot, IsFeatureAtoll)
LUAAPIIMPL(Plot, IsFeatureLake)
LUAAPIIMPL(Plot, IsFeatureRiver)
LUAAPIIMPL(Plot, HasImprovement)
LUAAPIIMPL(Plot, HasPlotType)
LUAAPIIMPL(Plot, IsPlotMountain)
LUAAPIIMPL(Plot, IsPlotMountains)
LUAAPIIMPL(Plot, IsPlotHill)
LUAAPIIMPL(Plot, IsPlotHills)
LUAAPIIMPL(Plot, IsPlotLand)
LUAAPIIMPL(Plot, IsPlotOcean)
LUAAPIIMPL(Plot, HasResource)
LUAAPIIMPL(Plot, HasRoute)
LUAAPIIMPL(Plot, IsRouteRoad)
LUAAPIIMPL(Plot, IsRouteRailroad)
LUAAPIIMPL(Plot, HasTerrain)
LUAAPIIMPL(Plot, IsTerrainGrass)
LUAAPIIMPL(Plot, IsTerrainPlains)
LUAAPIIMPL(Plot, IsTerrainDesert)
LUAAPIIMPL(Plot, IsTerrainTundra)
LUAAPIIMPL(Plot, IsTerrainSnow)
LUAAPIIMPL(Plot, IsTerrainCoast)
LUAAPIIMPL(Plot, IsTerrainOcean)
LUAAPIIMPL(Plot, IsTerrainMountain)
LUAAPIIMPL(Plot, IsTerrainMountains)
LUAAPIIMPL(Plot, IsTerrainHill)
LUAAPIIMPL(Plot, IsTerrainHills)
LUAAPIIMPL(Plot, IsAdjacentToFeature)
LUAAPIIMPL(Plot, IsWithinDistanceOfFeature)
LUAAPIIMPL(Plot, IsAdjacentToImprovement)
LUAAPIIMPL(Plot, IsWithinDistanceOfImprovement)
LUAAPIIMPL(Plot, IsAdjacentToPlotType)
LUAAPIIMPL(Plot, IsWithinDistanceOfPlotType)
LUAAPIIMPL(Plot, IsAdjacentToResource)
LUAAPIIMPL(Plot, IsWithinDistanceOfResource)
LUAAPIIMPL(Plot, IsAdjacentToTerrain)
LUAAPIIMPL(Plot, IsWithinDistanceOfTerrain)