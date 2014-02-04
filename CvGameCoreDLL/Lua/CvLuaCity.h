/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once
#ifndef CVLUACITY_H
#define CVLUACITY_H

#include "CvLuaScopedInstance.h"

class CvLuaCity : public CvLuaScopedInstance<CvLuaCity, CvCity>
{
public:
	//! Push CvCity methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Required by CvLuaScopedInstance.
	static void HandleMissingInstance(lua_State* L);

	//! Required by CvLuaScopedInstance.
	static const char* GetTypeName();

protected:

	static int lIsNone(lua_State* L);
	static int lKill(lua_State* L);

	static int lCreateGreatGeneral(lua_State* L);

	static int lDoTask(lua_State* L);
	static int lChooseProduction(lua_State* L);
	static int lGetCityPlotIndex(lua_State* L);
	static int lGetCityIndexPlot(lua_State* L);
	static int lCanWork(lua_State* L);
	static int lIsPlotBlockaded(lua_State* L);
	static int lClearWorkingOverride(lua_State* L);
	static int lCountNumImprovedPlots(lua_State* L);
	static int lCountNumWaterPlots(lua_State* L);
	static int lCountNumRiverPlots(lua_State* L);

	static int lFindPopulationRank(lua_State* L);
	static int lFindBaseYieldRateRank(lua_State* L);
	static int lFindYieldRateRank(lua_State* L);

	static int lAllUpgradesAvailable(lua_State* L);
	static int lIsWorldWondersMaxed(lua_State* L);
	static int lIsTeamWondersMaxed(lua_State* L);
	static int lIsNationalWondersMaxed(lua_State* L);
	static int lIsBuildingsMaxed(lua_State* L);

	static int lCanTrainTooltip(lua_State* L);
	static int lCanTrain(lua_State* L);
	static int lCanConstructTooltip(lua_State* L);
	static int lCanConstruct(lua_State* L);
	static int lCanCreate(lua_State* L);
	static int lCanPrepare(lua_State* L);
	static int lCanMaintain(lua_State* L);

	static int lGetPurchaseUnitTooltip(lua_State* L);
	static int lGetPurchaseBuildingTooltip(lua_State* L);

	static int lCanJoin(lua_State* L);
	static int lIsBuildingLocalResourceValid(lua_State* L);

	static int lGetResourceDemanded(lua_State* L);
	static int lSetResourceDemanded(lua_State* L);
	static int lDoPickResourceDemanded(lua_State* L);

	static int lGetFoodTurnsLeft(lua_State* L);
	static int lIsProduction(lua_State* L);
	static int lIsProductionLimited(lua_State* L);
	static int lIsProductionUnit(lua_State* L);
	static int lIsProductionBuilding(lua_State* L);
	static int lIsProductionProject(lua_State* L);
	static int lIsProductionSpecialist(lua_State* L);
	static int lIsProductionProcess(lua_State* L);

	static int lCanContinueProduction(lua_State* L);
	static int lGetProductionExperience(lua_State* L);
	static int lAddProductionExperience(lua_State* L);

	static int lGetProductionUnit(lua_State* L);
	static int lGetProductionUnitAI(lua_State* L);
	static int lGetProductionBuilding(lua_State* L);
	static int lGetProductionProject(lua_State* L);
	static int lGetProductionSpecialist(lua_State* L);
	static int lGetProductionProcess(lua_State* L);
	static int lGetProductionName(lua_State* L);
	static int lGetProductionNameKey(lua_State* L);
	static int lGetGeneralProductionTurnsLeft(lua_State* L);
	static int lIsFoodProduction(lua_State* L);
	static int lGetFirstUnitOrder(lua_State* L);
	static int lGetFirstProjectOrder(lua_State* L);
	static int lGetFirstSpecialistOrder(lua_State* L);

	static int lGetNumTrainUnitAI(lua_State* L);
	static int lGetFirstBuildingOrder(lua_State* L);
	static int lIsUnitFoodProduction(lua_State* L);
	static int lGetProduction(lua_State* L);
	static int lGetProductionTimes100(lua_State* L);
	static int lGetProductionNeeded(lua_State* L);
	static int lGetUnitProductionNeeded(lua_State* L);
	static int lGetBuildingProductionNeeded(lua_State* L);
	static int lGetProjectProductionNeeded(lua_State* L);
	static int lGetProductionTurnsLeft(lua_State* L);
	static int lGetUnitProductionTurnsLeft(lua_State* L);
	static int lGetBuildingProductionTurnsLeft(lua_State* L);
	static int lGetProjectProductionTurnsLeft(lua_State* L);
	static int lGetSpecialistProductionTurnsLeft(lua_State* L);

	static int lCreateApolloProgram(lua_State* L);

	static int lIsCanPurchase(lua_State* L);
	static int lGetUnitPurchaseCost(lua_State* L);
	static int lGetBuildingPurchaseCost(lua_State* L);
	static int lGetProjectPurchaseCost(lua_State* L);

	static int lSetProduction(lua_State* L);
	static int lChangeProduction(lua_State* L);

	static int lGetYieldModifierTooltip(lua_State* L);
	static int lGetProductionModifier(lua_State* L);
	static int lGetCurrentProductionDifference(lua_State* L);
	static int lGetCurrentProductionDifferenceTimes100(lua_State* L);
	static int lGetUnitProductionModifier(lua_State* L);
	static int lGetBuildingProductionModifier(lua_State* L);
	static int lGetProjectProductionModifier(lua_State* L);
	static int lGetSpecialistProductionModifier(lua_State* L);

	static int lGetExtraProductionDifference(lua_State* L);

	static int lCanHurry(lua_State* L);
	static int lHurry(lua_State* L);
	static int lGetConscriptUnit(lua_State* L);
	static int lGetConscriptPopulation(lua_State* L);
	static int lConscriptMinCityPopulation(lua_State* L);
	static int lCanConscript(lua_State* L);
	static int lConscript(lua_State* L);
	static int lGetResourceYieldRateModifier(lua_State* L);
	static int lGetHandicapType(lua_State* L);
	static int lGetCivilizationType(lua_State* L);
	static int lGetPersonalityType(lua_State* L);
	static int lGetArtStyleType(lua_State* L);
	static int lGetCitySizeType(lua_State* L);

	static int lIsBarbarian(lua_State* L);
	static int lIsHuman(lua_State* L);
	static int lIsVisible(lua_State* L);

	static int lIsCapital(lua_State* L);
	static int lIsOriginalCapital(lua_State* L);
	static int lIsCoastal(lua_State* L);

	static int lFoodConsumption(lua_State* L);
	static int lFoodDifference(lua_State* L);
	static int lFoodDifferenceTimes100(lua_State* L);
	static int lGrowthThreshold(lua_State* L);
	static int lProductionLeft(lua_State* L);
	static int lHurryCost(lua_State* L);
	static int lHurryGold(lua_State* L);
	static int lHurryPopulation(lua_State* L);
	static int lHurryProduction(lua_State* L);
	static int lMaxHurryPopulation(lua_State* L);

	static int lGetNumBuilding(lua_State* L);
	static int lIsHasBuilding(lua_State* L);
	static int lGetNumActiveBuilding(lua_State* L);
	static int lGetID(lua_State* L);
	static int lGetX(lua_State* L);
	static int lGetY(lua_State* L);
	static int lAt(lua_State* L);
	static int lAtPlot(lua_State* L);
	static int lPlot(lua_State* L);
	static int lArea(lua_State* L);
	static int lWaterArea(lua_State* L);
	static int lGetRallyPlot(lua_State* L);

	static int lCanBuyPlot(lua_State* L);
	static int lCanBuyPlotAt(lua_State* L);
	static int lCanBuyAnyPlot(lua_State* L);
	static int lGetNextBuyablePlot(lua_State* L);
	static int lGetBuyablePlotList(lua_State* L);
	static int lGetBuyPlotCost(lua_State* L);

	static int lGetGarrisonedUnit(lua_State* L);

	static int lGetGameTurnFounded(lua_State* L);
	static int lGetGameTurnAcquired(lua_State* L);
	static int lGetGameTurnLastExpanded(lua_State* L);
	static int lGetPopulation(lua_State* L);
	static int lSetPopulation(lua_State* L);
	static int lChangePopulation(lua_State* L);
	static int lGetRealPopulation(lua_State* L);

	static int lGetHighestPopulation(lua_State* L);
	static int lSetHighestPopulation(lua_State* L);
	//static int lGetWorkingPopulation(lua_State* L);
	//static int lGetSpecialistPopulation(lua_State* L);
	static int lGetNumGreatPeople(lua_State* L);
	static int lGetBaseGreatPeopleRate(lua_State* L);
	static int lGetGreatPeopleRate(lua_State* L);
	static int lGetTotalGreatPeopleRateModifier(lua_State* L);
	static int lChangeBaseGreatPeopleRate(lua_State* L);
	static int lGetGreatPeopleRateModifier(lua_State* L);
	static int lGetGreatPeopleProgress(lua_State* L);
	static int lChangeGreatPeopleProgress(lua_State* L);

	static int lGetJONSCultureStored(lua_State* L);
	static int lSetJONSCultureStored(lua_State* L);
	static int lChangeJONSCultureStored(lua_State* L);
	static int lGetJONSCultureLevel(lua_State* L);
	static int lSetJONSCultureLevel(lua_State* L);
	static int lChangeJONSCultureLevel(lua_State* L);
	static int lDoJONSCultureLevelIncrease(lua_State* L);
	static int lGetJONSCultureThreshold(lua_State* L);

	static int lGetJONSCulturePerTurn(lua_State* L);

	static int lGetBaseJONSCulturePerTurn(lua_State* L);
	static int lGetJONSCulturePerTurnFromBuildings(lua_State* L);
	static int lChangeJONSCulturePerTurnFromBuildings(lua_State* L);
	static int lGetJONSCulturePerTurnFromPolicies(lua_State* L);
	static int lChangeJONSCulturePerTurnFromPolicies(lua_State* L);
	static int lGetJONSCulturePerTurnFromSpecialists(lua_State* L);
	static int lChangeJONSCulturePerTurnFromSpecialists(lua_State* L);
	static int lGetJONSCulturePerTurnFromTerrain(lua_State* L);
	static int lChangeJONSCulturePerTurnFromTerrain(lua_State* L);
	static int lGetJONSCulturePerTurnFromTraits(lua_State* L);

	static int lGetCultureRateModifier(lua_State* L);
	static int lChangeCultureRateModifier(lua_State* L);

	static int lGetNumWorldWonders(lua_State* L);
	static int lGetNumTeamWonders(lua_State* L);
	static int lGetNumNationalWonders(lua_State* L);
	static int lGetNumBuildings(lua_State* L);

	static int lGetWonderProductionModifier(lua_State* L);
	static int lChangeWonderProductionModifier(lua_State* L);

	static int lGetLocalResourceWonderProductionMod(lua_State* L);

	static int lChangeHealRate(lua_State* L);

	static int lIsNoOccupiedUnhappiness(lua_State* L);

	static int lGetFood(lua_State* L);
	static int lGetFoodTimes100(lua_State* L);
	static int lSetFood(lua_State* L);
	static int lChangeFood(lua_State* L);
	static int lGetFoodKept(lua_State* L);
	static int lGetMaxFoodKeptPercent(lua_State* L);
	static int lGetOverflowProduction(lua_State* L);
	static int lSetOverflowProduction(lua_State* L);
	static int lGetFeatureProduction(lua_State* L);
	static int lSetFeatureProduction(lua_State* L);
	static int lGetMilitaryProductionModifier(lua_State* L);
	static int lGetSpaceProductionModifier(lua_State* L);
	static int lGetBuildingDefense(lua_State* L);
	static int lGetFreeExperience(lua_State* L);
	static int lGetCurrAirlift(lua_State* L);
	static int lGetMaxAirlift(lua_State* L);
	static int lGetAirModifier(lua_State* L);
	static int lGetNukeModifier(lua_State* L);
	//static int lGetFreeSpecialist(lua_State* L);

	static int lIsResistance(lua_State* L);
	static int lGetResistanceTurns(lua_State* L);
	static int lChangeResistanceTurns(lua_State* L);

	static int lIsRazing(lua_State* L);
	static int lGetRazingTurns(lua_State* L);
	static int lChangeRazingTurns(lua_State* L);

	static int lIsOccupied(lua_State* L);
	static int lSetOccupied(lua_State* L);

	static int lIsPuppet(lua_State* L);
	static int lSetPuppet(lua_State* L);

	static int lGetHappinessFromBuildings(lua_State* L);
	static int lGetHappiness(lua_State* L);

	static int lIsNeverLost(lua_State* L);
	static int lSetNeverLost(lua_State* L);
	static int lIsDrafted(lua_State* L);
	static int lSetDrafted(lua_State* L);
	static int lIsAirliftTargeted(lua_State* L);
	static int lSetAirliftTargeted(lua_State* L);

	static int lIsBlockaded (lua_State* L);

	static int lGetWeLoveTheKingDayCounter(lua_State* L);
	static int lSetWeLoveTheKingDayCounter(lua_State* L);
	static int lChangeWeLoveTheKingDayCounter(lua_State* L);

	static int lGetNumThingsProduced(lua_State* L);

	//static int lIsCitizensAutomated(lua_State* L);
	//static int lSetCitizensAutomated(lua_State* L);
	static int lIsProductionAutomated(lua_State* L);
	static int lSetProductionAutomated(lua_State* L);
	static int lSetCitySizeBoost(lua_State* L);
	static int lGetOwner(lua_State* L);
	static int lGetTeam(lua_State* L);
	static int lGetPreviousOwner(lua_State* L);
	static int lGetOriginalOwner(lua_State* L);
	static int lGetSeaPlotYield(lua_State* L);
	static int lGetRiverPlotYield(lua_State* L);
	static int lGetLakePlotYield(lua_State* L);

	static int lGetBaseYieldRate(lua_State* L);

	static int lGetBaseYieldRateFromTerrain(lua_State* L);
	static int lChangeBaseYieldRateFromTerrain(lua_State* L);

	static int lGetBaseYieldRateFromBuildings(lua_State* L);
	static int lChangeBaseYieldRateFromBuildings(lua_State* L);

	static int lGetBaseYieldRateFromSpecialists(lua_State* L);
	static int lChangeBaseYieldRateFromSpecialists(lua_State* L);

	static int lGetBaseYieldRateFromMisc(lua_State* L);
	static int lChangeBaseYieldRateFromMisc(lua_State* L);

	static int lGetYieldPerPopTimes100(lua_State* L);

	static int lGetBaseYieldRateModifier(lua_State* L);
	static int lGetYieldRate(lua_State* L);
	static int lGetYieldRateTimes100(lua_State* L);
	static int lGetYieldRateModifier(lua_State* L);

	static int lGetExtraSpecialistYield(lua_State* L);
	static int lGetExtraSpecialistYieldOfType(lua_State* L);

	static int lGetDomainFreeExperience(lua_State* L);
	static int lGetDomainProductionModifier(lua_State* L);

	static int lIsEverOwned(lua_State* L);

	static int lIsRevealed(lua_State* L);
	static int lSetRevealed(lua_State* L);
	static int lGetNameKey(lua_State* L);
	static int lGetName(lua_State* L);
	static int lSetName(lua_State* L);
	static int lIsHasResourceLocal(lua_State* L);
	static int lGetBuildingProduction(lua_State* L);
	static int lSetBuildingProduction(lua_State* L);
	static int lChangeBuildingProduction(lua_State* L);
	static int lGetBuildingProductionTime(lua_State* L);
	static int lSetBuildingProductionTime(lua_State* L);
	static int lChangeBuildingProductionTime(lua_State* L);
	static int lGetBuildingOriginalOwner(lua_State* L);
	static int lGetBuildingOriginalTime(lua_State* L);
	static int lGetUnitProduction(lua_State* L);
	static int lSetUnitProduction(lua_State* L);
	static int lChangeUnitProduction(lua_State* L);
	static int lGetGreatPeopleUnitRate(lua_State* L);
	static int lGetGreatPeopleUnitProgress(lua_State* L);
	static int lSetGreatPeopleUnitProgress(lua_State* L);
	static int lChangeGreatPeopleUnitProgress(lua_State* L);

	static int lIsCanAddSpecialistToBuilding(lua_State* L);
	static int lGetSpecialistUpgradeThreshold(lua_State* L);
	static int lGetNumSpecialistsAllowedByBuilding(lua_State* L);
	static int lGetSpecialistCount(lua_State* L);
	static int lGetSpecialistGreatPersonProgress(lua_State* L);
	static int lGetSpecialistGreatPersonProgressTimes100(lua_State* L);
	static int lChangeSpecialistGreatPersonProgressTimes100(lua_State* L);
	static int lGetNumSpecialistsInBuilding(lua_State* L);
	static int lDoReallocateCitizens(lua_State* L);
	static int lDoVerifyWorkingPlots(lua_State* L);
	static int lIsNoAutoAssignSpecialists(lua_State* L);

	static int lGetFocusType(lua_State* L);
	static int lSetFocusType(lua_State* L);

	static int lIsForcedAvoidGrowth(lua_State* L);

	static int lGetUnitCombatFreeExperience(lua_State* L);
	static int lGetFreePromotionCount(lua_State* L);
	static int lIsFreePromotion(lua_State* L);
	static int lGetSpecialistFreeExperience(lua_State* L);

	static int lUpdateStrengthValue(lua_State* L);
	static int lGetStrengthValue(lua_State* L);

	static int lGetDamage(lua_State* L);
	static int lSetDamage(lua_State* L);
	static int lChangeDamage(lua_State* L);

	static int lCanRangeStrike(lua_State* L);
	static int lCanRangeStrikeNow(lua_State* L);
	static int lCanRangeStrikeAt(lua_State* L);
	static int lHasPerformedRangedStrikeThisTurn(lua_State* L);
	static int lRangeCombatUnitDefense(lua_State* L);
	static int lRangeCombatDamage(lua_State* L);
	static int lGetAirStrikeDefenseDamage(lua_State* L);

	static int lIsWorkingPlot(lua_State* L);
	static int lAlterWorkingPlot(lua_State* L);
	static int lIsForcedWorkingPlot(lua_State* L);
	static int lGetNumForcedWorkingPlots(lua_State *L);
	static int lGetNumRealBuilding(lua_State* L);
	static int lSetNumRealBuilding(lua_State* L);
	static int lGetNumFreeBuilding(lua_State* L);
	static int lIsBuildingSellable(lua_State* L);
	static int lGetSellBuildingRefund(lua_State* L);
	static int lGetTotalBaseBuildingMaintenance(lua_State* L);

	static int lClearOrderQueue(lua_State* L);
	static int lPushOrder(lua_State* L);
	static int lPopOrder(lua_State* L);
	static int lGetOrderQueueLength(lua_State* L);
	static int lGetOrderFromQueue(lua_State* L);

	static int lGetBuildingYieldChange(lua_State* L);
	static int lSetBuildingYieldChange(lua_State* L);

	static int lAI_avoidGrowth(lua_State* L);

	static int lGetScriptData(lua_State* L);
	static int lSetScriptData(lua_State* L);

	static int lGetNumCityPlots(lua_State* L);

	static int lCanPlaceUnitHere (lua_State* L);

	static int lGetSpecialistYield(lua_State* L);
	static int lGetCultureFromSpecialist(lua_State* L);
};

#endif //CVLUACITY_H