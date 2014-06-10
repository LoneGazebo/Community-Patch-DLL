/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaPlayer.h
//!  \brief     Public interface to CvLuaPlayer.
//!
//!		This includes the public interface to CvLuaPlayer.
//!		CvLuaPlayer is a Lua exposed version of CvPlayerAI instances.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUAPLAYER_H
#define CVLUAPLAYER_H

#include "CvLuaScopedInstance.h"

class CvLuaPlayer : public CvLuaScopedInstance<CvLuaPlayer, CvPlayerAI>
{
public:
	//! Push all player instances to Lua
	static void Register(lua_State* L);

	//! Push CvPlayerAI methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Required by CvLuaScopedInstance.
	static void HandleMissingInstance(lua_State* L);

	//! Required by CvLuaScopedInstance.
	static const char* GetTypeName();

protected:

	//! Protected Lua call to register all player instances.
	static int pRegister(lua_State* L);

	static int lInitCity(lua_State* L);
	static int lAcquireCity(lua_State* L);
	static int lKillCities(lua_State* L);

	static int lGetNewCityName(lua_State* L);

	static int lInitUnit(lua_State* L);
	static int lDisbandUnit(lua_State* L);
	static int lAddFreeUnit(lua_State* L);

	static int lChooseTech(lua_State* L);

	static int lKillUnits(lua_State* L);
	static int lIsHuman(lua_State* L);
	static int lIsBarbarian(lua_State* L);
	static int lGetName(lua_State* L);
	static int lGetNameKey(lua_State* L);
	static int lGetNickName(lua_State* L);
	static int lGetCivilizationDescription(lua_State* L);
	static int lGetCivilizationDescriptionKey(lua_State* L);
	static int lGetCivilizationShortDescription(lua_State* L);
	static int lGetCivilizationShortDescriptionKey(lua_State* L);
	static int lGetCivilizationAdjective(lua_State* L);
	static int lGetCivilizationAdjectiveKey(lua_State* L);
	static int lIsWhiteFlag(lua_State* L);
	static int lGetStateReligionName(lua_State* L);
	static int lGetStateReligionKey(lua_State* L);
	static int lGetWorstEnemyName(lua_State* L);
	static int lGetArtStyleType(lua_State* L);

	static int lCountCityFeatures(lua_State* L);
	static int lCountNumBuildings(lua_State* L);

	static int lGetNumWorldWonders(lua_State* L);
	static int lChangeNumWorldWonders(lua_State* L);
	static int lGetNumWondersBeatenTo(lua_State* L);
	static int lSetNumWondersBeatenTo(lua_State* L);

	static int lIsCapitalConnectedToCity(lua_State* L);

	static int lIsTurnActive(lua_State* L);
	static int lIsSimultaneousTurns(lua_State* L);

	static int lFindNewCapital(lua_State* L);
	static int lCanRaze(lua_State* L);
	static int lRaze(lua_State* L);
	static int lDisband(lua_State* L);
	static int lCanReceiveGoody(lua_State* L);
	static int lReceiveGoody(lua_State* L);
	static int lDoGoody(lua_State* L);
	static int lCanFound(lua_State* L);
	static int lFound(lua_State* L);

	static int lCanTrain(lua_State* L);
	static int lCanConstruct(lua_State* L);
	static int lCanCreate(lua_State* L);
	static int lCanPrepare(lua_State* L);
	static int lCanMaintain(lua_State* L);

	static int lIsProductionMaxedUnitClass(lua_State* L);
	static int lIsProductionMaxedBuildingClass(lua_State* L);
	static int lIsProductionMaxedProject(lua_State* L);
	static int lGetUnitProductionNeeded(lua_State* L);
	static int lGetBuildingProductionNeeded(lua_State* L);
	static int lGetProjectProductionNeeded(lua_State* L);

	static int lHasReadyUnit(lua_State* L);
	static int lGetFirstReadyUnit(lua_State* L);
	static int lGetFirstReadyUnitPlot(lua_State* L);

	static int lHasBusyUnit(lua_State* L);
	static int lHasBusyMovingUnit(lua_State* L);

	static int lGetBuildingClassPrereqBuilding(lua_State* L);

	static int lRemoveBuildingClass(lua_State* L);

	static int lCanBuild(lua_State* L);
	static int lIsBuildBlockedByFeature(lua_State* L);
	static int lGetBestRoute(lua_State* L);
	static int lGetImprovementUpgradeRate(lua_State* L);

	static int lCalculateTotalYield(lua_State* L);

	static int lCalculateUnitCost(lua_State* L);
	static int lCalculateUnitSupply(lua_State* L);

	static int lGetNumMaintenanceFreeUnits(lua_State* L);

	static int lGetBuildingGoldMaintenance(lua_State* L);
	static int lSetBaseBuildingGoldMaintenance(lua_State* L);
	static int lChangeBaseBuildingGoldMaintenance(lua_State* L);

	static int lGetImprovementGoldMaintenance(lua_State* L);
	static int lCalculateGoldRate(lua_State* L);
	static int lCalculateGoldRateTimes100(lua_State* L);
	static int lCalculateGrossGoldTimes100(lua_State* L);
	static int lCalculateInflatedCosts(lua_State* L);
	static int lCalculateResearchModifier(lua_State* L);
	static int lIsResearch(lua_State* L);
	static int lCanEverResearch(lua_State* L);
	static int lCanResearch(lua_State* L);
	static int lGetCurrentResearch(lua_State* L);
	static int lIsCurrentResearchRepeat(lua_State* L);
	static int lIsNoResearchAvailable(lua_State* L);
	static int lGetResearchTurnsLeft(lua_State* L);
	static int lGetResearchCost(lua_State* L);
	static int lGetResearchProgress(lua_State* L);

	static int lUnitsRequiredForGoldenAge(lua_State* L);
	static int lUnitsGoldenAgeCapable(lua_State* L);
	static int lUnitsGoldenAgeReady(lua_State* L);
	static int lGreatGeneralThreshold(lua_State* L);
	static int lSpecialistYield(lua_State* L);

	static int lGetStartingPlot(lua_State* L);
	static int lSetStartingPlot(lua_State* L);
	static int lGetTotalPopulation(lua_State* L);
	static int lGetAveragePopulation(lua_State* L);
	static int lGetRealPopulation(lua_State* L);

	static int lGetNewCityExtraPopulation(lua_State* L);
	static int lChangeNewCityExtraPopulation(lua_State* L);

	static int lGetTotalLand(lua_State* L);
	static int lGetTotalLandScored(lua_State* L);

	static int lGetGold(lua_State* L);
	static int lSetGold(lua_State* L);
	static int lChangeGold(lua_State* L);
	static int lCalculateGrossGold(lua_State* L);
	static int lGetLifetimeGrossGold(lua_State* L);
	static int lGetGoldFromCitiesTimes100(lua_State* L);
	static int lGetGoldPerTurnFromDiplomacy(lua_State* L);
	static int lGetRouteGoldTimes100(lua_State* L);
	static int lGetCityConnectionGold(lua_State* L);
	static int lGetCityConnectionGoldTimes100(lua_State* L);

	static int lGetTotalJONSCulturePerTurn(lua_State* L);

	static int lGetJONSCulturePerTurnFromCities(lua_State* L);

	static int lGetJONSCulturePerTurnFromExcessHappiness(lua_State* L);

	static int lGetCultureWonderMultiplier(lua_State* L);

	static int lGetJONSCulturePerTurnForFree(lua_State* L);
	static int lChangeJONSCulturePerTurnForFree(lua_State* L);

	static int lGetJONSCulturePerTurnFromMinorCivs(lua_State* L);
	static int lChangeJONSCulturePerTurnFromMinorCivs(lua_State* L);

	static int lGetCultureCityModifier(lua_State* L);

	static int lGetJONSCulture(lua_State* L);
	static int lSetJONSCulture(lua_State* L);
	static int lChangeJONSCulture(lua_State* L);

	static int lGetJONSCultureEverGenerated(lua_State *L);

	static int lGetHappiness(lua_State* L);
	static int lSetHappiness(lua_State* L);

	static int lGetExcessHappiness(lua_State* L);

	static int lIsEmpireUnhappy(lua_State* L);
	static int lIsEmpireVeryUnhappy(lua_State* L);
	static int lIsEmpireSuperUnhappy(lua_State* L);

	static int lGetHappinessFromPolicies(lua_State* L);
	static int lGetHappinessFromBuildings(lua_State* L);
	static int lChangeHappinessFromBuildings(lua_State* L);

	static int lGetExtraHappinessPerCity(lua_State* L);
	static int lChangeExtraHappinessPerCity(lua_State* L);

	static int lGetHappinessFromResources(lua_State* L);
	static int lGetHappinessFromResourceVariety(lua_State* L);
	static int lGetExtraHappinessPerLuxury(lua_State* L);
	static int lGetHappinessFromReligion(lua_State* L);
	static int lGetHappinessFromNaturalWonders(lua_State* L);

	static int lGetUnhappiness(lua_State* L);
	static int lGetUnhappinessForecast(lua_State* L);

	static int lGetUnhappinessFromCityForUI(lua_State* L);

	static int lGetUnhappinessFromCityCount(lua_State* L);
	static int lGetUnhappinessFromCapturedCityCount(lua_State* L);
	static int lGetUnhappinessFromCityPopulation(lua_State* L);
	static int lGetUnhappinessFromCitySpecialists(lua_State* L);
	static int lGetUnhappinessFromPuppetCityPopulation(lua_State* L);
	static int lGetUnhappinessFromOccupiedCities(lua_State* L);
	static int lGetUnhappinessFromUnits(lua_State* L);
	static int lChangeUnhappinessFromUnits(lua_State* L);

	static int lGetUnhappinessMod(lua_State* L);
	static int lGetCityCountUnhappinessMod(lua_State* L);
	static int lGetOccupiedPopulationUnhappinessMod(lua_State* L);
	static int lGetCapitalUnhappinessMod(lua_State* L);
	static int lGetTraitCityUnhappinessMod(lua_State* L);
	static int lGetTraitPopUnhappinessMod(lua_State* L);
	static int lIsHalfSpecialistUnhappiness(lua_State* L);

	static int lGetHappinessFromGarrisonedUnits(lua_State* L);
	static int lChangeHappinessFromGarrisonedUnits(lua_State* L);
	static int lGetHappinessPerGarrisonedUnit(lua_State* L);
	static int lSetHappinessPerGarrisonedUnit(lua_State* L);
	static int lChangeHappinessPerGarrisonedUnit(lua_State* L);

	static int lGetHappinessFromTradeRoutes(lua_State* L);
	static int lGetHappinessPerTradeRoute(lua_State* L);
	static int lSetHappinessPerTradeRoute(lua_State* L);
	static int lChangeHappinessPerTradeRoute(lua_State* L);

	static int lGetHappinessFromMinor(lua_State* L);

	static int lGetBarbarianCombatBonus(lua_State* L);
	static int lSetBarbarianCombatBonus(lua_State* L);
	static int lChangeBarbarianCombatBonus(lua_State* L);

	static int lGetGarrisonedCityRangeStrikeModifier(lua_State* L);
	static int lChangeGarrisonedCityRangeStrikeModifier(lua_State* L);

	static int lIsAlwaysSeeBarbCamps(lua_State* L);
	static int lSetAlwaysSeeBarbCampsCount(lua_State* L);
	static int lChangeAlwaysSeeBarbCampsCount(lua_State* L);

	static int lIsPolicyBlocked(lua_State* L);
	static int lIsPolicyBranchBlocked(lua_State* L);
	static int lIsPolicyBranchUnlocked(lua_State* L);
	static int lSetPolicyBranchUnlocked(lua_State* L);
	static int lGetNumPolicyBranchesUnlocked(lua_State* L);
	static int lGetPolicyBranchChosen(lua_State* L);
	static int lGetNumPolicyBranchesAllowed(lua_State* L);
	static int lGetNumPolicies(lua_State* L);
	static int lHasPolicy(lua_State* L);
	static int lSetHasPolicy(lua_State* L);
	static int lGetNextPolicyCost(lua_State* L);
	static int lCanAdoptPolicy(lua_State* L);
	static int lDoAdoptPolicy(lua_State* L);
	static int lCanUnlockPolicyBranch(lua_State* L);
	static int lGetDominantPolicyBranchForTitle(lua_State* L);
	static int lGetBranchPicked1(lua_State* L);
	static int lGetBranchPicked2(lua_State* L);
	static int lGetBranchPicked3(lua_State* L);

	static int lGetNumPolicyBranchesFinished(lua_State* L);
	static int lIsPolicyBranchFinished(lua_State* L);

	static int lIsAnarchy(lua_State* L);
	static int lGetAnarchyNumTurns(lua_State* L);
	static int lSetAnarchyNumTurns(lua_State* L);
	static int lChangeAnarchyNumTurns(lua_State* L);

	static int lGetAdvancedStartPoints(lua_State* L);
	static int lSetAdvancedStartPoints(lua_State* L);
	static int lChangeAdvancedStartPoints(lua_State* L);
	static int lGetAdvancedStartUnitCost(lua_State* L);
	static int lGetAdvancedStartCityCost(lua_State* L);
	static int lGetAdvancedStartPopCost(lua_State* L);
	static int lGetAdvancedStartBuildingCost(lua_State* L);
	static int lGetAdvancedStartImprovementCost(lua_State* L);
	static int lGetAdvancedStartRouteCost(lua_State* L);
	static int lGetAdvancedStartTechCost(lua_State* L);
	static int lGetAdvancedStartVisibilityCost(lua_State* L);

	static int lGetAttackBonusTurns(lua_State* L);

	static int lGetGoldenAgeProgressThreshold(lua_State* L);
	static int lGetGoldenAgeProgressMeter(lua_State* L);
	static int lSetGoldenAgeProgressMeter(lua_State* L);
	static int lChangeGoldenAgeProgressMeter(lua_State* L);
	static int lGetNumGoldenAges(lua_State* L);
	static int lSetNumGoldenAges(lua_State* L);
	static int lChangeNumGoldenAges(lua_State* L);
	static int lGetGoldenAgeTurns(lua_State* L);
	static int lGetGoldenAgeLength(lua_State* L);
	static int lIsGoldenAge(lua_State* L);
	static int lChangeGoldenAgeTurns(lua_State* L);
	static int lGetNumUnitGoldenAges(lua_State* L);
	static int lChangeNumUnitGoldenAges(lua_State* L);
	static int lGetStrikeTurns(lua_State* L);
	static int lGetGoldenAgeModifier(lua_State* L);

	static int lGetHurryModifier(lua_State* L);

	static int lCreateGreatGeneral(lua_State* L);
	static int lGetGreatPeopleCreated(lua_State* L);
	static int lGetGreatGeneralsCreated(lua_State* L);
	static int lGetGreatPeopleThresholdModifier(lua_State* L);
	static int lGetGreatGeneralsThresholdModifier(lua_State* L);
	static int lGetGreatPeopleRateModifier(lua_State* L);
	static int lGetGreatGeneralRateModifier(lua_State* L);
	static int lGetDomesticGreatGeneralRateModifier(lua_State* L);

	static int lGetProductionModifier(lua_State* L);
	static int lGetUnitProductionModifier(lua_State* L);
	static int lGetBuildingProductionModifier(lua_State* L);
	static int lGetProjectProductionModifier(lua_State* L);
	static int lGetSpecialistProductionModifier(lua_State* L);

	static int lGetMaxGlobalBuildingProductionModifier(lua_State* L);
	static int lGetMaxTeamBuildingProductionModifier(lua_State* L);
	static int lGetMaxPlayerBuildingProductionModifier(lua_State* L);
	static int lGetFreeExperience(lua_State* L);
	static int lGetFeatureProductionModifier(lua_State* L);
	static int lGetWorkerSpeedModifier(lua_State* L);
	static int lGetImprovementUpgradeRateModifier(lua_State* L);
	static int lGetMilitaryProductionModifier(lua_State* L);
	static int lGetSpaceProductionModifier(lua_State* L);
	static int lGetSettlerProductionModifier(lua_State* L);
	static int lGetCapitalSettlerProductionModifier(lua_State* L);
	static int lGetWonderProductionModifier(lua_State* L);

	static int lGetUnitProductionMaintenanceMod(lua_State* L);
	static int lGetNumUnitsSupplied(lua_State* L);
	static int lGetNumUnitsSuppliedByHandicap(lua_State* L);
	static int lGetNumUnitsSuppliedByCities(lua_State* L);
	static int lGetNumUnitsSuppliedByPopulation(lua_State* L);
	static int lGetNumUnitsOutOfSupply(lua_State* L);

	static int lGetCityDefenseModifier(lua_State* L);
	static int lGetNumNukeUnits(lua_State* L);
	static int lGetNumOutsideUnits(lua_State* L);

	static int lGetGoldPerUnit(lua_State* L);
	static int lChangeGoldPerUnitTimes100(lua_State* L);
	static int lGetGoldPerMilitaryUnit(lua_State* L);
	static int lGetExtraUnitCost(lua_State* L);
	static int lGetNumMilitaryUnits(lua_State* L);
	static int lGetHappyPerMilitaryUnit(lua_State* L);
	static int lIsMilitaryFoodProduction(lua_State* L);
	static int lGetHighestUnitLevel(lua_State* L);

	static int lGetConscriptCount(lua_State* L);
	static int lSetConscriptCount(lua_State* L);
	static int lChangeConscriptCount(lua_State* L);

	static int lGetMaxConscript(lua_State* L);
	static int lGetOverflowResearch(lua_State* L);
	static int lGetExpInBorderModifier(lua_State* L);

	static int lGetLevelExperienceModifier(lua_State* L);

	static int lGetCultureBombTimer(lua_State* L);
	static int lGetConversionTimer(lua_State* L);

	static int lGetCapitalCity(lua_State* L);
	static int lIsHasLostCapital(lua_State* L);
	static int lGetCitiesLost(lua_State* L);

	static int lGetPower(lua_State* L);
	static int lGetMilitaryMight(lua_State* L);
	static int lGetPopScore(lua_State* L);
	static int lGetLandScore(lua_State* L);
	static int lGetWondersScore(lua_State* L);
	static int lGetTechScore(lua_State* L);
	static int lGetTotalTimePlayed(lua_State* L);

	static int lGetScore(lua_State* L);
	static int lGetScoreFromCities(lua_State* L);
	static int lGetScoreFromPopulation(lua_State* L);
	static int lGetScoreFromLand(lua_State* L);
	static int lGetScoreFromWonders(lua_State* L);
	static int lGetScoreFromTechs(lua_State* L);
	static int lChangeScoreFromTechs(lua_State* L);
	static int lGetScoreFromFutureTech(lua_State* L);
	static int lChangeScoreFromFutureTech(lua_State* L);


	// Minor Civ stuff
	static int lIsMinorCiv(lua_State* L);
	static int lGetMinorCivType(lua_State* L);
	static int lGetMinorCivTrait(lua_State* L);
	static int lGetPersonality(lua_State* L);
	static int lGetAlly(lua_State* L);
	static int lIsFriends(lua_State* L);
	static int lIsAllies(lua_State* L);
	static int lIsPlayerHasOpenBorders(lua_State* L);
	static int lIsPlayerHasOpenBordersAutomatically(lua_State* L);
	static int lGetFriendshipChangePerTurnTimes100(lua_State* L);
	static int lGetMinorCivFriendshipWithMajor(lua_State* L);
	static int lChangeMinorCivFriendshipWithMajor(lua_State* L);
	static int lGetFriendshipNeededForNextLevel(lua_State* L);
	static int lGetMinorCivFriendshipLevelWithMajor(lua_State* L);
	static int lGetActiveQuestForPlayer(lua_State* L);
	static int lGetQuestData1(lua_State* L);
	static int lGetQuestData2(lua_State* L);
	static int lIsMinorCivUnitSpawningDisabled(lua_State* L);
	static int lIsMinorCivRouteEstablishedWithMajor(lua_State* L);
	static int lIsMinorWarQuestWithMajorActive(lua_State* L);
	static int lGetMinorWarQuestWithMajorRemainingCount(lua_State* L);
	static int lGetTurnsSinceThreatenedByBarbarians(lua_State* L);
	static int lGetFriendshipFromGoldGift(lua_State* L);
	static int lGetMinorCivFavoriteMajor(lua_State* L);
	static int lGetMinorCivScienceFriendshipBonus(lua_State* L);
	static int lGetMinorCivCultureFriendshipBonus(lua_State* L);
	static int lGetCurrentCultureBonus(lua_State* L);
	static int lGetMinorCivHappinessFriendshipBonus(lua_State* L);
	static int lGetCurrentCapitalFoodBonus(lua_State* L);
	static int lGetCurrentOtherCityFoodBonus(lua_State* L);
	static int lGetCurrentSpawnEstimate(lua_State* L);
	static int lGetCurrentScienceFriendshipBonusTimes100(lua_State* L);
	static int lIsPeaceBlocked(lua_State* L);
	static int lIsMinorPermanentWar(lua_State* L);
	static int lGetNumMinorCivsMet(lua_State* L);
	static int lDoMinorLiberationByMajor(lua_State* L);

	static int lIsAlive(lua_State* L);
	static int lIsEverAlive(lua_State* L);
	static int lIsExtendedGame(lua_State* L);
	static int lIsFoundedFirstCity(lua_State* L);

	static int lGetEndTurnBlockingType(lua_State* L);
	static int lGetEndTurnBlockingNotificationIndex(lua_State* L);

	static int lIsStrike(lua_State* L);

	static int lGetID(lua_State* L);
	static int lGetHandicapType(lua_State* L);
	static int lGetCivilizationType(lua_State* L);
	static int lGetLeaderType(lua_State* L);
	static int lGetPersonalityType(lua_State* L);
	static int lSetPersonalityType(lua_State* L);
	static int lGetCurrentEra(lua_State* L);

	static int lGetTeam(lua_State* L);

	static int lGetPlayerColors(lua_State* L);
	static int lGetPlayerColor(lua_State* L);

	static int lGetSeaPlotYield(lua_State* L);
	static int lGetYieldRateModifier(lua_State* L);
	static int lGetCapitalYieldRateModifier(lua_State* L);
	static int lGetExtraYieldThreshold(lua_State* L);

	// Science

	static int lGetScience(lua_State* L);
	static int lGetScienceTimes100(lua_State* L);

	static int lGetScienceFromCitiesTimes100(lua_State* L);
	static int lGetScienceFromOtherPlayersTimes100(lua_State* L);
	static int lGetScienceFromHappinessTimes100(lua_State* L);
	static int lGetScienceFromResearchAgreementsTimes100(lua_State* L);
	static int lGetScienceFromBudgetDeficitTimes100(lua_State* L);

	// END Science

	static int lGetProximityToPlayer(lua_State* L);
	static int lDoUpdateProximityToPlayer(lua_State* L);

	static int lIsOption(lua_State* L);
	static int lSetOption(lua_State* L);
	static int lIsPlayable(lua_State* L);
	static int lSetPlayable(lua_State* L);

	static int lGetNumResourceUsed(lua_State* L);
	static int lGetNumResourceTotal(lua_State* L);
	static int lChangeNumResourceTotal(lua_State* L);
	static int lGetNumResourceAvailable(lua_State* L);

	static int lGetResourceExport(lua_State* L);
	static int lGetResourceImport(lua_State* L);
	static int lGetResourceFromMinors(lua_State* L);

	static int lGetImprovementCount(lua_State* L);

	static int lIsBuildingFree(lua_State* L);
	static int lGetUnitClassCount(lua_State* L);
	static int lIsUnitClassMaxedOut(lua_State* L);
	static int lGetUnitClassMaking(lua_State* L);
	static int lGetUnitClassCountPlusMaking(lua_State* L);

	static int lGetBuildingClassCount(lua_State* L);
	static int lIsBuildingClassMaxedOut(lua_State* L);
	static int lGetBuildingClassMaking(lua_State* L);
	static int lGetBuildingClassCountPlusMaking(lua_State* L);
	static int lGetHurryCount(lua_State* L);
	static int lIsHasAccessToHurry(lua_State* L);
	static int lIsCanHurry(lua_State* L);
	static int lGetHurryGoldCost(lua_State* L);

	static int lIsResearchingTech(lua_State* L);
	static int lSetResearchingTech(lua_State* L);

	static int lGetCombatExperience(lua_State* L);
	static int lChangeCombatExperience(lua_State* L);
	static int lSetCombatExperience(lua_State* L);
	static int lGetLifetimeCombatExperience(lua_State* L);

	static int lGetSpecialistExtraYield(lua_State* L);

	static int lFindPathLength(lua_State* L);

	static int lGetQueuePosition(lua_State* L);
	static int lClearResearchQueue(lua_State* L);
	static int lPushResearch(lua_State* L);
	static int lPopResearch(lua_State* L);
	static int lGetLengthResearchQueue(lua_State* L);
	static int lAddCityName(lua_State* L);
	static int lGetNumCityNames(lua_State* L);
	static int lGetCityName(lua_State* L);

	static int lGetCities(lua_State* L);
	static int lGetNumCities(lua_State* L);
	static int lGetCityByID(lua_State* L);

	static int lGetUnits(lua_State* L);
	static int lGetNumUnits(lua_State* L);
	static int lGetUnitByID(lua_State* L);

	static int lAI_updateFoundValues(lua_State* L);
	static int lAI_foundValue(lua_State* L);

	static int lGetScoreHistory(lua_State* L);
	static int lGetEconomyHistory(lua_State* L);
	static int lGetIndustryHistory(lua_State* L);
	static int lGetAgricultureHistory(lua_State* L);
	static int lGetPowerHistory(lua_State* L);
	static int lGetReplayData(lua_State* L);
	static int lSetReplayDataValue(lua_State* L);


	static int lGetScriptData(lua_State* L);
	static int lSetScriptData(lua_State* L);

	static int lGetNumPlots(lua_State* L);

	static int lGetNumPlotsBought(lua_State* L);
	static int lSetNumPlotsBought(lua_State* L);
	static int lChangeNumPlotsBought(lua_State* L);

	static int lGetBuyPlotCost(lua_State* L);
	static int lGetPlotDanger(lua_State* L);

	// Diplomacy Stuff

	static int lDoBeginDiploWithHuman(lua_State* L);
	static int lDoTradeScreenOpened(lua_State* L);
	static int lDoTradeScreenClosed(lua_State* L);
	static int lGetMajorCivApproach(lua_State* L);
	static int lGetApproachTowardsUsGuess(lua_State* L);
	static int lIsWillAcceptPeaceWithPlayer(lua_State* L);
	static int lIsProtectingMinor(lua_State* L);
	static int lIsDontSettleMessageTooSoon(lua_State* L);
	static int lIsDoFMessageTooSoon(lua_State* L);
	static int lIsDoF(lua_State* L);
	static int lIsPlayerDoFwithAnyFriend(lua_State* L);
	static int lIsPlayerDoFwithAnyEnemy(lua_State* L);
	static int lIsPlayerDenouncedFriend(lua_State* L);
	static int lIsPlayerDenouncedEnemy(lua_State* L);
	static int lIsUntrustworthyFriend(lua_State* L);
	static int lGetNumFriendsDenouncedBy(lua_State* L);
	static int lIsFriendDenouncedUs(lua_State* L);
	static int lGetWeDenouncedFriendCount(lua_State* L);
	static int lIsFriendDeclaredWarOnUs(lua_State* L);
	static int lGetWeDeclaredWarOnFriendCount(lua_State* L);
	static int lGetNumRequestsRefused(lua_State* L);
	//static int lIsWorkingAgainstPlayerAccepted(lua_State* L);
	static int lGetCoopWarAcceptedState(lua_State* L);
	static int lGetNumWarsFought(lua_State* L);

	static int lGetLandDisputeLevel(lua_State* L);
	static int lGetVictoryDisputeLevel(lua_State* L);
	static int lGetWonderDisputeLevel(lua_State* L);
	static int lGetMinorCivDisputeLevel(lua_State* L);
	static int lGetWarmongerThreat(lua_State* L);
	static int lIsPlayerNoSettleRequestEverAsked(lua_State* L);
	static int lIsDemandEverMade(lua_State* L);
	static int lGetNumCiviliansReturnedToMe(lua_State* L);
	static int lGetNumTimesCultureBombed(lua_State* L);
	static int lIsPlayerBrokenMilitaryPromise(lua_State* L);
	static int lIsPlayerIgnoredMilitaryPromise(lua_State* L);
	static int lIsPlayerBrokenExpansionPromise(lua_State* L);
	static int lIsPlayerIgnoredExpansionPromise(lua_State* L);
	static int lIsPlayerBrokenBorderPromise(lua_State* L);
	static int lIsPlayerIgnoredBorderPromise(lua_State* L);
	static int lIsPlayerBrokenCityStatePromise(lua_State* L);
	static int lIsPlayerIgnoredCityStatePromise(lua_State* L);
	static int lIsPlayerBrokenCoopWarPromise(lua_State* L);
	static int lGetOtherPlayerNumProtectedMinorsKilled(lua_State* L);
	static int lGetOtherPlayerNumProtectedMinorsAttacked(lua_State* L);
	static int lIsDenouncedPlayer(lua_State* L);
	static int lIsDenouncingPlayer(lua_State* L);
	static int lIsPlayerRecklessExpander(lua_State* L);
	static int lGetRecentTradeValue(lua_State* L);
	static int lGetCommonFoeValue(lua_State* L);
	static int lGetRecentAssistValue(lua_State* L);
	static int lIsLiberatedCapital(lua_State* L);
	static int lIsLiberatedCity(lua_State* L);
	static int lIsGaveAssistanceTo(lua_State* L);
	static int lIsHasPaidTributeTo(lua_State* L);
	static int lIsNukedBy(lua_State* L);
	static int lIsCapitalCapturedBy(lua_State* L);

	static int lDoForceDoF(lua_State* L);
	static int lDoForceDenounce(lua_State* L);

	// END Diplomacy Stuff

	static int lGetNumNotifications(lua_State* L);
	static int lGetNotificationStr(lua_State* L);
	static int lGetNotificationSummaryStr(lua_State* L);
	static int lGetNotificationIndex(lua_State* L);
	static int lGetNotificationTurn(lua_State* L);
	static int lGetNotificationDismissed(lua_State* L);
	static int lAddNotification(lua_State* L);

	static int lGetRecommendedWorkerPlots(lua_State* L);
	static int lGetRecommendedFoundCityPlots(lua_State* L);

	static int lGetUnimprovedAvailableLuxuryResource(lua_State* L);
	static int lIsAnyPlotImproved(lua_State* L);
	static int lGetPlayerVisiblePlot(lua_State* L);

	// slewis - tutorial functions
	static int lGetEverPoppedGoody(lua_State* L);
	static int lGetClosestGoodyPlot(lua_State* L);
	static int lIsAnyGoodyPlotAccessible(lua_State* L);
	static int lGetPlotHasOrder(lua_State* L);
	static int lGetAnyUnitHasOrderToGoody(lua_State* L);
	static int lGetEverTrainedBuilder(lua_State* L);
	// end tutorial functions

	static int lGetNumFreeTechs(lua_State* L);
	static int lSetNumFreeTechs(lua_State* L);
	static int lGetNumFreePolicies(lua_State* L);
	static int lSetNumFreePolicies(lua_State* L);
	static int lChangeNumFreePolicies(lua_State* L);
	static int lGetNumFreeGreatPeople(lua_State* L);
	static int lSetNumFreeGreatPeople(lua_State* L);
	static int lChangeNumFreeGreatPeople(lua_State* L);

	static int lUnitsAux(lua_State* L);
	static int lUnits(lua_State* L);

	static int lCitiesAux(lua_State* L);
	static int lCities(lua_State* L);

	static int lHasReceivedNetTurnComplete(lua_State* L);
	static int lGetTraitGoldenAgeCombatModifier(lua_State* L);
	static int lGetTraitCityStateCombatModifier(lua_State* L);
	static int lGetTraitGreatGeneralExtraBonus(lua_State* L);
	static int lGetTraitGreatScientistRateModifier(lua_State* L);

	static int lGetExtraBuildingHappinessFromPolicies(lua_State* L);

	static int lGetPrevCity(lua_State* L);
	static int lGetNextCity(lua_State* L);

	static int lGetFreePromotionCount(lua_State* L);
	static int lIsFreePromotion(lua_State* L);
	static int lChangeFreePromotionCount(lua_State* L);

	static int lGetEmbarkedGraphicOverride(lua_State* L);
	static int lSetEmbarkedGraphicOverride(lua_State* L);

	static int lAddTemporaryDominanceZone(lua_State* L);

	static int lGetNaturalWonderYieldModifier(lua_State* L);

	static int lGetPolicyBuildingClassYieldModifier(lua_State* L);

	static int lIsConnected(lua_State* L);
	static int lIsObserver(lua_State* L);

	static int lHasTurnTimerExpired(lua_State* L);

	static int lHasUnitOfClassType(lua_State* L);

};

#endif //CVLUAPLAYER_H
