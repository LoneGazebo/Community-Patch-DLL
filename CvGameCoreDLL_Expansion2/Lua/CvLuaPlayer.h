/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(IsCityNameValid, bool, sName, bTestDestroyed);
#endif

	static int lInitUnit(lua_State* L);
	static int lInitUnitWithNameOffset(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lInitNamedUnit(lua_State* L);
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	static int lGetResourceMonopolyPlayer(lua_State* L);
	static int lGetMonopolyPercent(lua_State* L);
#endif
	static int lDisbandUnit(lua_State* L);
	static int lAddFreeUnit(lua_State* L);

	static int lChooseTech(lua_State* L);

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(GetSpecificUnitType, int, sUnitClass);
	LUAAPIEXTN(GetSpecificBuildingType, int, sBuildingClass);
#endif

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
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(IsPlotConnectedToPlot, bool, pFromPlot, pToPlot);
#endif

	static int lIsTurnActive(lua_State* L);
	static int lIsSimultaneousTurns(lua_State* L);

	static int lFindNewCapital(lua_State* L);
	static int lCanRaze(lua_State* L);
	static int lRaze(lua_State* L);
	static int lDisband(lua_State* L);
	static int lCanReceiveGoody(lua_State* L);
	static int lReceiveGoody(lua_State* L);
	static int lDoGoody(lua_State* L);
	static int lCanGetGoody(lua_State* L);
	static int lCanFound(lua_State* L);
	static int lFound(lua_State* L);

	static int lCanTrain(lua_State* L);
	static int lCanConstruct(lua_State* L);
	static int lCanCreate(lua_State* L);
	static int lCanPrepare(lua_State* L);
	static int lCanMaintain(lua_State* L);

	static int lIsCanPurchaseAnyCity(lua_State* L);
	static int lGetFaithPurchaseType(lua_State* L);
	static int lSetFaithPurchaseType(lua_State* L);
	static int lGetFaithPurchaseIndex(lua_State* L);
	static int lSetFaithPurchaseIndex(lua_State* L);

	static int lIsProductionMaxedUnitClass(lua_State* L);
	static int lIsProductionMaxedBuildingClass(lua_State* L);
	static int lIsProductionMaxedProject(lua_State* L);
	static int lGetUnitProductionNeeded(lua_State* L);
	static int lGetBuildingProductionNeeded(lua_State* L);
	static int lGetProjectProductionNeeded(lua_State* L);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_PROCESS_STOCKPILE)
	LUAAPIEXTN(GetMaxStockpile, int);
#endif

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
	static int lCanResearchForFree(lua_State* L);
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
	static int lGreatAdmiralThreshold(lua_State* L);
	static int lSpecialistYield(lua_State* L);
	static int lSetGreatGeneralCombatBonus(lua_State* L);
	static int lGetGreatGeneralCombatBonus(lua_State* L);

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
	static int lGetGoldFromCitiesMinusTradeRoutesTimes100(lua_State* L);
	static int lGetGoldPerTurnFromDiplomacy(lua_State* L);
	static int lGetCityConnectionRouteGoldTimes100(lua_State* L);
	static int lGetCityConnectionGold(lua_State* L);
	static int lGetCityConnectionGoldTimes100(lua_State* L);
	static int lGetGoldPerTurnFromReligion(lua_State* L);
	static int lGetGoldPerTurnFromTradeRoutes(lua_State* L);
	static int lGetGoldPerTurnFromTradeRoutesTimes100(lua_State* L);
	static int lGetGoldPerTurnFromTraits(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetInternalTradeRouteGoldBonus(lua_State* L);
	static int lGetGAPFromReligion(lua_State* L);
	static int lGetGAPFromCities(lua_State* L);
	static int lGetGAPFromTraits(lua_State* L);
#endif
	static int lGetTotalJONSCulturePerTurn(lua_State* L);

	static int lGetJONSCulturePerTurnFromCities(lua_State* L);

	static int lGetJONSCulturePerTurnFromExcessHappiness(lua_State* L);
	static int lGetJONSCulturePerTurnFromTraits(lua_State* L);

	static int lGetCultureWonderMultiplier(lua_State* L);

	static int lGetJONSCulturePerTurnForFree(lua_State* L);
	static int lChangeJONSCulturePerTurnForFree(lua_State* L);

	static int lGetJONSCulturePerTurnFromMinorCivs(lua_State* L); // DEPRECATED, use lGetCulturePerTurnFromMinorCivs instead
	static int lChangeJONSCulturePerTurnFromMinorCivs(lua_State* L); // DEPRECATED, does nothing
	static int lGetCulturePerTurnFromMinorCivs(lua_State* L);
	static int lGetCulturePerTurnFromMinor(lua_State* L);

	static int lGetCulturePerTurnFromReligion(lua_State* L);
	static int lGetCulturePerTurnFromBonusTurns(lua_State* L);
	static int lGetCultureCityModifier(lua_State* L);

	static int lGetJONSCulture(lua_State* L);
	static int lSetJONSCulture(lua_State* L);
	static int lChangeJONSCulture(lua_State* L);

	static int lGetJONSCultureEverGenerated(lua_State* L);

	static int lGetLastTurnLifetimeCulture(lua_State* L);
	static int lGetInfluenceOn(lua_State* L);
	static int lGetLastTurnInfluenceOn(lua_State* L);
	static int lGetInfluencePerTurn(lua_State* L);
	static int lGetInfluenceLevel(lua_State* L);
	static int lGetInfluenceTrend(lua_State* L);
	static int lGetTurnsToInfluential(lua_State* L);
	static int lGetNumCivsInfluentialOn(lua_State* L);
	static int lGetNumCivsToBeInfluentialOn(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE)
	LUAAPIEXTN(GetInfluenceTradeRouteGoldBonus, int);
	LUAAPIEXTN(GetWoundedUnitDamageMod, int);
	LUAAPIEXTN(SetCapitalCity, void);
	LUAAPIEXTN(SetOriginalCapitalXY, void);
	LUAAPIEXTN(GetNumWonders, int);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE_POLICIES)
	LUAAPIEXTN(GetNoUnhappinessExpansion, int);
#endif
	static int lGetInfluenceTradeRouteScienceBonus(lua_State* L);
	static int lGetInfluenceCityStateSpyRankBonus(lua_State* L);
	static int lGetInfluenceMajorCivSpyRankBonus(lua_State* L);
	static int lGetInfluenceSpyRankTooltip(lua_State* L);
	static int lGetTourism(lua_State* L);
	static int lGetTourismModifierWith(lua_State* L);
	static int lGetTourismModifierWithTooltip(lua_State* L);
	static int lGetPublicOpinionType(lua_State* L);
	static int lGetPublicOpinionPreferredIdeology(lua_State* L);
	static int lGetPublicOpinionTooltip(lua_State* L);
	static int lGetPublicOpinionUnhappiness(lua_State* L);
	static int lGetPublicOpinionUnhappinessTooltip(lua_State* L);

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(DoSwapGreatWorks, void, iFocusYield);
	LUAAPIEXTN(DoSwapGreatWorksHuman, void);
#endif
	static int lHasAvailableGreatWorkSlot(lua_State* L);
	static int lGetCityOfClosestGreatWorkSlot(lua_State* L);
	static int lGetBuildingOfClosestGreatWorkSlot(lua_State* L);
	static int lGetNextDigCompletePlot(lua_State* L);
	static int lGetWrittenArtifactCulture (lua_State* L);
	static int lGetNumGreatWorks(lua_State *L);
	static int lGetNumGreatWorkSlots(lua_State *L);

	static int lGetFaith(lua_State* L);
	static int lSetFaith(lua_State* L);
	static int lChangeFaith(lua_State* L);
	static int lGetTotalFaithPerTurn(lua_State* L);
	static int lGetFaithPerTurnFromCities(lua_State* L);
	static int lGetFaithPerTurnFromMinorCivs(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetGoldPerTurnFromMinorCivs(lua_State* L);
	static int lGetSciencePerTurnFromMinorCivs(lua_State* L);
#endif
	static int lGetFaithPerTurnFromReligion(lua_State* L);
	static int lHasCreatedPantheon(lua_State* L);
	static int lGetBeliefInPantheon(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
	LUAAPIEXTN(GetBeliefsInPantheon, table);
#endif
	static int lCanCreatePantheon(lua_State* L);
	static int lHasCreatedReligion(lua_State* L);
	static int lGetReligionCreatedByPlayer(lua_State* L);
	static int lGetFoundedReligionEnemyCityCombatMod(lua_State* L);
	static int lGetFoundedReligionFriendlyCityCombatMod(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE_BELIEFS)
	LUAAPIEXTN(GetYieldPerTurnFromReligion, int);
#endif
	static int lGetMinimumFaithNextGreatProphet(lua_State* L);
	static int lHasReligionInMostCities(lua_State* L);
	static int lDoesUnitPassFaithPurchaseCheck(lua_State* L);

	static int lGetHappiness(lua_State* L);
	static int lSetHappiness(lua_State* L);

	static int lGetExcessHappiness(lua_State* L);

	static int lIsEmpireUnhappy(lua_State* L);
	static int lIsEmpireVeryUnhappy(lua_State* L);
	static int lIsEmpireSuperUnhappy(lua_State* L);

	static int lGetHappinessFromPolicies(lua_State* L);
	static int lGetHappinessFromCities(lua_State* L);
	static int lGetHappinessFromBuildings(lua_State* L);

	static int lGetExtraHappinessPerCity(lua_State* L);
	static int lChangeExtraHappinessPerCity(lua_State* L);

	static int lGetHappinessFromResources(lua_State* L);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	LUAAPIEXTN(GetHappinessFromResourceMonopolies, int);
#endif
	static int lGetHappinessFromResourceVariety(lua_State* L);
	static int lGetExtraHappinessPerLuxury(lua_State* L);
	static int lGetHappinessFromReligion(lua_State* L);
	static int lGetHappinessFromNaturalWonders(lua_State* L);
	static int lGetHappinessFromLeagues(lua_State* L);

	static int lGetUnhappiness(lua_State* L);
	static int lGetUnhappinessForecast(lua_State* L);

	static int lGetUnhappinessFromCityForUI(lua_State* L);

	static int lGetUnhappinessFromCityCount(lua_State* L);
	static int lGetUnhappinessFromCapturedCityCount(lua_State* L);
	static int lGetUnhappinessFromCityPopulation(lua_State* L);
	static int lGetUnhappinessFromCitySpecialists(lua_State* L);
	static int lGetUnhappinessFromPuppetCityPopulation(lua_State* L);
	static int lGetUnhappinessFromOccupiedCities(lua_State* L);
	static int lGetUnhappinessFromPublicOpinion(lua_State* L);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	LUAAPIEXTN(GetUnhappinessFromWarWeariness, int);
#endif
	static int lGetUnhappinessFromUnits(lua_State* L);
	static int lChangeUnhappinessFromUnits(lua_State* L);

	static int lGetUnhappinessMod(lua_State* L);
	static int lGetCityCountUnhappinessMod(lua_State* L);
	static int lGetOccupiedPopulationUnhappinessMod(lua_State* L);
	static int lGetCapitalUnhappinessMod(lua_State* L);
	static int lGetTraitCityUnhappinessMod(lua_State* L);
	static int lGetTraitPopUnhappinessMod(lua_State* L);
	static int lIsHalfSpecialistUnhappiness(lua_State* L);

	static int lGetHappinessPerGarrisonedUnit(lua_State* L);
	static int lSetHappinessPerGarrisonedUnit(lua_State* L);
	static int lChangeHappinessPerGarrisonedUnit(lua_State* L);

	static int lGetHappinessFromTradeRoutes(lua_State* L);
	static int lGetHappinessPerTradeRoute(lua_State* L);
	static int lSetHappinessPerTradeRoute(lua_State* L);
	static int lChangeHappinessPerTradeRoute(lua_State* L);

	static int lGetCityConnectionTradeRouteGoldModifier(lua_State* L);

	static int lGetHappinessFromMinorCivs(lua_State* L);
	static int lGetHappinessFromMinor(lua_State* L);

	static int lGetBarbarianCombatBonus(lua_State* L);
	static int lSetBarbarianCombatBonus(lua_State* L);
	static int lChangeBarbarianCombatBonus(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	LUAAPIEXTN(GetCombatBonusVsHigherPop, int);
	LUAAPIEXTN(GetWarScore, int);
	LUAAPIEXTN(GetPlayerMilitaryStrengthComparedToUs, int);
	LUAAPIEXTN(GetPlayerEconomicStrengthComparedToUs, int);
	LUAAPIEXTN(GetWarDamageLevel, int);
	LUAAPIEXTN(IsWillingToMakePeaceWithHuman, bool);
	LUAAPIEXTN(GetTreatyWillingToOffer, int);
	LUAAPIEXTN(GetMajorCivOpinion, int);
	LUAAPIEXTN(GetMajorityReligion, int);
	//JFD
	LUAAPIEXTN(GetStateReligion, int);
	LUAAPIEXTN(SetStateReligion, int);
	LUAAPIEXTN(GetNumCitiesWithStateReligion, int);
	LUAAPIEXTN(GetNumCitiesWithoutStateReligion, int);
	LUAAPIEXTN(SetPiety, int);
	LUAAPIEXTN(ChangePiety, int);
	LUAAPIEXTN(GetPiety, int);
	LUAAPIEXTN(GetPietyRate, int);
	LUAAPIEXTN(SetPietyRate, int);
	LUAAPIEXTN(ChangePietyRate, int);
	LUAAPIEXTN(GetTurnsSinceConversion, int);
	LUAAPIEXTN(SetTurnsSinceConversion, int);
	LUAAPIEXTN(HasStateReligion, int);
	LUAAPIEXTN(HasSecularized, int);
	LUAAPIEXTN(SetHasSecularized, int);
	LUAAPIEXTN(IsPagan, int);

	LUAAPIEXTN(GetSovereignty, int);
	LUAAPIEXTN(SetSovereignty, int);
	LUAAPIEXTN(ChangeSovereignty, int);

	LUAAPIEXTN(GetGovernment, int);
	LUAAPIEXTN(SetGovernment, int);
	LUAAPIEXTN(HasGovernment, int);

	LUAAPIEXTN(GetReformCooldown, int);
	LUAAPIEXTN(SetReformCooldown, int);
	LUAAPIEXTN(ChangeReformCooldown, int);

	LUAAPIEXTN(GetGovernmentCooldown, int);
	LUAAPIEXTN(SetGovernmentCooldown, int);
	LUAAPIEXTN(ChangeGovernmentCooldown, int);

	LUAAPIEXTN(GetReformCooldownRate, int);
	LUAAPIEXTN(SetReformCooldownRate, int);
	LUAAPIEXTN(ChangeReformCooldownRate, int);

	LUAAPIEXTN(GetGovernmentCooldownRate, int);
	LUAAPIEXTN(SetGovernmentCooldownRate, int);
	LUAAPIEXTN(ChangeGovernmentCooldownRate, int);

	LUAAPIEXTN(GetPoliticLeader, int);
	LUAAPIEXTN(SetPoliticLeader, int);

	LUAAPIEXTN(SetPoliticLeaderKey, int);
	LUAAPIEXTN(GetPoliticLeaderKey, int);

	LUAAPIEXTN(SetLegislatureName, int);
	LUAAPIEXTN(GetLegislatureName, int);

	LUAAPIEXTN(GetPoliticPercent, int);
	LUAAPIEXTN(SetPoliticPercent, int);

	LUAAPIEXTN(GetCurrency, int);
	LUAAPIEXTN(SetCurrency, int);
	
	LUAAPIEXTN(HasCurrency, int);

	LUAAPIEXTN(SetCurrencyName, int);
	LUAAPIEXTN(GetCurrencyName, int);
#endif
	static int lGetCombatBonusVsHigherTech(lua_State* L);
	static int lGetCombatBonusVsLargerCiv(lua_State* L);

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
	static int lGetNumPoliciesInBranch(lua_State* L);
	static int lHasPolicy(lua_State* L);
	static int lSetHasPolicy(lua_State* L);
	static int lGetNextPolicyCost(lua_State* L);
	static int lCanAdoptPolicy(lua_State* L);
	static int lDoAdoptPolicy(lua_State* L);
	static int lCanUnlockPolicyBranch(lua_State* L);
	static int lGetDominantPolicyBranchForTitle(lua_State* L);
	static int lGetLateGamePolicyTree(lua_State* L);
	static int lGetBranchPicked1(lua_State* L);
	static int lGetBranchPicked2(lua_State* L);
	static int lGetBranchPicked3(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(GrantPolicy, bool, iPolicy, bFree);
	LUAAPIEXTN(RevokePolicy, bool, iPolicy);
	LUAAPIEXTN(SwapPolicy, bool, iPolicyNew, iPolicyOld);
	LUAAPIEXTN(CanAdoptIdeology, bool, iIdeologyBranch);
	LUAAPIEXTN(CanAdoptTenet, bool, iTenetPolicy, bIgnoreCost);
#endif
	
	static int lGetPolicyCatchSpiesModifier(lua_State* L);

	static int lGetNumPolicyBranchesFinished(lua_State* L);
	static int lIsPolicyBranchFinished(lua_State* L);

	static int lGetAvailableTenets(lua_State* L);
	static int lGetTenet(lua_State* L);

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
	static int lGetCultureBonusTurns(lua_State* L);
	static int lGetTourismBonusTurns(lua_State* L);

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
	static int lGetGoldenAgeTourismModifier(lua_State* L);
	static int lGetGoldenAgeGreatWriterRateModifier(lua_State* L);
	static int lGetGoldenAgeGreatArtistRateModifier(lua_State* L);
	static int lGetGoldenAgeGreatMusicianRateModifier(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	LUAAPIEXTN(GetGoldenAgeGreatScientistRateModifier, int);
	LUAAPIEXTN(GetGoldenAgeGreatEngineerRateModifier, int);
	LUAAPIEXTN(GetGoldenAgeGreatMerchantRateModifier, int);
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES) && defined(MOD_API_UNIFIED_YIELDS)
	LUAAPIEXTN(GetGoldenAgeGreatDiplomatRateModifier, int);
#endif

	static int lGetHurryModifier(lua_State* L);

	static int lCreateGreatGeneral(lua_State* L);
	static int lGetGreatPeopleCreated(lua_State* L);
	static int lGetGreatGeneralsCreated(lua_State* L);
	static int lGetGreatPeopleThresholdModifier(lua_State* L);
	static int lGetGreatGeneralsThresholdModifier(lua_State* L);
	static int lGetGreatAdmiralsThresholdModifier(lua_State* L);
	static int lGetGreatPeopleRateModifier(lua_State* L);
	static int lGetGreatGeneralRateModifier(lua_State* L);
	static int lGetDomesticGreatGeneralRateModifier(lua_State* L);
	static int lGetGreatWriterRateModifier(lua_State* L);
	static int lGetGreatArtistRateModifier(lua_State* L);
	static int lGetGreatMusicianRateModifier(lua_State* L);
	static int lGetGreatScientistRateModifier(lua_State* L);
	static int lGetGreatMerchantRateModifier(lua_State* L);
	static int lGetGreatEngineerRateModifier(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CITYSTATES)
	LUAAPIEXTN(GetGreatDiplomatRateModifier, int);
	LUAAPIEXTN(GetScienceRateFromMinorAllies, int);
	LUAAPIEXTN(GetScienceRateFromLeagueAid, int);
	LUAAPIEXTN(GetLeagueCultureCityModifier, int);
#endif

	static int lGetPolicyGreatPeopleRateModifier(lua_State* L);
	static int lGetPolicyGreatWriterRateModifier(lua_State* L);
	static int lGetPolicyGreatArtistRateModifier(lua_State* L);
	static int lGetPolicyGreatMusicianRateModifier(lua_State* L);
	static int lGetPolicyGreatScientistRateModifier(lua_State* L);
	static int lGetPolicyGreatMerchantRateModifier(lua_State* L);
	static int lGetPolicyGreatEngineerRateModifier(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CITYSTATES)
	LUAAPIEXTN(GetPolicyGreatDiplomatRateModifier, int);
#endif

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
	static int lGetTotalTimePlayed(lua_State* L);

	static int lGetScore(lua_State* L);
	static int lGetScoreFromCities(lua_State* L);
	static int lGetScoreFromPopulation(lua_State* L);
	static int lGetScoreFromLand(lua_State* L);
	static int lGetScoreFromWonders(lua_State* L);
	static int lGetScoreFromTechs(lua_State* L);
	static int lGetScoreFromFutureTech(lua_State* L);
	static int lChangeScoreFromFutureTech(lua_State* L);
	static int lGetScoreFromPolicies(lua_State* L);
	static int lGetScoreFromGreatWorks(lua_State* L);
	static int lGetScoreFromReligion(lua_State* L);
	static int lGetScoreFromScenario1(lua_State* L);
	static int lChangeScoreFromScenario1(lua_State* L);
	static int lGetScoreFromScenario2(lua_State* L);
	static int lChangeScoreFromScenario2(lua_State* L);
	static int lGetScoreFromScenario3(lua_State* L);
	static int lChangeScoreFromScenario3(lua_State* L);
	static int lGetScoreFromScenario4(lua_State* L);
	static int lChangeScoreFromScenario4(lua_State* L);

	static int lIsGoldenAgeCultureBonusDisabled(lua_State* L);

	// Minor Civ stuff
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(IsMajorCiv, bool);
#endif
	static int lIsMinorCiv(lua_State* L);
	static int lGetMinorCivType(lua_State* L);
	static int lGetMinorCivTrait(lua_State* L);
	static int lGetPersonality(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(SetPersonality, void, iPersonality);
#endif
	static int lIsMinorCivHasUniqueUnit(lua_State* L);
	static int lGetMinorCivUniqueUnit(lua_State* L);
	static int lSetMinorCivUniqueUnit(lua_State* L);
	static int lGetAlly(lua_State* L);
	static int lGetAlliedTurns(lua_State* L);
	static int lIsFriends(lua_State* L);
	static int lIsAllies(lua_State* L);
	static int lIsPlayerHasOpenBorders(lua_State* L);
	static int lIsPlayerHasOpenBordersAutomatically(lua_State* L);
	static int lGetFriendshipChangePerTurnTimes100(lua_State* L);
	static int lGetMinorCivFriendshipWithMajor(lua_State* L);
	static int lChangeMinorCivFriendshipWithMajor(lua_State* L);
	static int lGetMinorCivFriendshipAnchorWithMajor(lua_State* L);
	static int lGetFriendshipNeededForNextLevel(lua_State* L);
#if defined(MOD_BALANCE_CORE_MINORS) || defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	LUAAPIEXTN(GetJerk, int);
	LUAAPIEXTN(GetCoupCooldown, int);
#endif
#if defined(MOD_BALANCE_CORE)
	LUAAPIEXTN(GetNumDenouncements, int);
	LUAAPIEXTN(GetNumDenouncementsOfPlayer, int);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	LUAAPIEXTN(GetUnhappinessFromCityCulture, int);
	LUAAPIEXTN(GetUnhappinessFromCityScience, int);
	LUAAPIEXTN(GetUnhappinessFromCityDefense, int);
	LUAAPIEXTN(GetUnhappinessFromCityGold, int);
	LUAAPIEXTN(GetUnhappinessFromCityConnection, int);
	LUAAPIEXTN(GetUnhappinessFromCityPillaged, int);
	LUAAPIEXTN(GetUnhappinessFromCityStarving, int);
	LUAAPIEXTN(GetUnhappinessFromCityMinority, int);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_LUXURY)
	LUAAPIEXTN(GetBonusHappinessFromLuxuries, int);
	LUAAPIEXTN(GetPopNeededForLux, int);
	LUAAPIEXTN(GetCurrentTotalPop, int);
	LUAAPIEXTN(GetScalingNationalPopulationRequrired, int);
	LUAAPIEXTN(GetBaseLuxuryHappiness, int);
	LUAAPIEXTN(GetLuxuryBonusPlusOne, int);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	LUAAPIEXTN(CalculateUnhappinessTooltip, int);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	LUAAPIEXTN(GetPuppetUnhappinessMod, int);
	LUAAPIEXTN(GetCapitalUnhappinessModCBP, int);
#endif
	static int lGetMinorCivFriendshipLevelWithMajor(lua_State* L);
	static int lGetActiveQuestForPlayer(lua_State* L); // DEPRECATED
	static int lIsMinorCivActiveQuestForPlayer(lua_State* L);
	static int lGetMinorCivNumActiveQuestsForPlayer(lua_State* L);
	static int lIsMinorCivDisplayedQuestForPlayer(lua_State* L);
	static int lGetMinorCivNumDisplayedQuestsForPlayer(lua_State* L);
	static int lGetQuestData1(lua_State* L);
	static int lGetQuestData2(lua_State* L);
	static int lGetQuestTurnsRemaining(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetRewardString(lua_State* L);
	static int lGetTargetCityString(lua_State* L);
	static int lGetExplorePercent(lua_State* L);
	static int lGetXQuestBuildingRemaining(lua_State* L);
	static int lQuestSpyActionsRemaining(lua_State* L);
#endif
	static int lIsMinorCivContestLeader(lua_State* L);
	static int lGetMinorCivContestValueForLeader(lua_State* L);
	static int lGetMinorCivContestValueForPlayer(lua_State* L);
	static int lIsMinorCivUnitSpawningDisabled(lua_State* L);
	static int lIsMinorCivRouteEstablishedWithMajor(lua_State* L);
	static int lIsMinorWarQuestWithMajorActive(lua_State* L); // DEPRECATED, no longer a quest
	static int lGetMinorWarQuestWithMajorRemainingCount(lua_State* L); // DEPRECATED, no longer a quest
	static int lIsProxyWarActiveForMajor(lua_State* L);
	static int lIsThreateningBarbariansEventActiveForPlayer(lua_State* L);
	static int lGetTurnsSinceThreatenedByBarbarians(lua_State* L); // DEPRECATED, use lGetTurnsSinceThreatenedAnnouncement instead
	static int lGetTurnsSinceThreatenedAnnouncement(lua_State* L);
	static int lGetFriendshipFromGoldGift(lua_State* L);
	static int lGetMinorCivFavoriteMajor(lua_State* L);
	static int lGetMinorCivScienceFriendshipBonus(lua_State* L);
	static int lGetMinorCivCultureFriendshipBonus(lua_State* L); // DEPRECATED
	static int lGetMinorCivCurrentCultureFlatBonus(lua_State* L);
	static int lGetMinorCivCurrentCulturePerBuildingBonus(lua_State* L);
	static int lGetCurrentCultureBonus(lua_State* L); // DEPRECATED, use lGetMinorCivCurrentCultureBonus instead
	static int lGetMinorCivCurrentCultureBonus(lua_State* L);
	static int lGetMinorCivHappinessFriendshipBonus(lua_State* L); // DEPRECATED
	static int lGetMinorCivCurrentHappinessFlatBonus(lua_State* L);
	static int lGetMinorCivCurrentHappinessPerLuxuryBonus(lua_State* L);
	static int lGetMinorCivCurrentHappinessBonus(lua_State* L);
	static int lGetMinorCivCurrentFaithBonus(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetMinorCivCurrentGoldBonus(lua_State* L);
	static int lGetMinorCivCurrentScienceBonus(lua_State* L);
	static int lGetPermanentAlly(lua_State* L);
	static int lIsNoAlly(lua_State* L);
#endif
	static int lGetCurrentCapitalFoodBonus(lua_State* L);
	static int lGetCurrentOtherCityFoodBonus(lua_State* L);
	static int lGetCurrentSpawnEstimate(lua_State* L);
	static int lGetCurrentScienceFriendshipBonusTimes100(lua_State* L);
	static int lIsPeaceBlocked(lua_State* L);
	static int lIsMinorPermanentWar(lua_State* L);
	static int lGetNumMinorCivsMet(lua_State* L);
	static int lDoMinorLiberationByMajor(lua_State* L);
	static int lIsProtectedByMajor(lua_State* L);
	static int lCanMajorProtect(lua_State* L);
	static int lCanMajorStartProtection(lua_State* L);
	static int lCanMajorWithdrawProtection(lua_State* L);
	static int lGetTurnLastPledgedProtectionByMajor(lua_State* L);
	static int lGetTurnLastPledgeBrokenByMajor(lua_State* L);
	static int lGetMinorCivBullyGoldAmount(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lSetBullyUnit(lua_State* L);
	static int lGetBullyUnit(lua_State* L);
	static int lGetYieldTheftAmount(lua_State* L);
#endif
	static int lCanMajorBullyGold(lua_State* L);
	static int lGetMajorBullyGoldDetails(lua_State* L);
	static int lCanMajorBullyUnit(lua_State* L);
	static int lGetMajorBullyUnitDetails(lua_State* L);
	static int lCanMajorBuyout(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lCanMajorMarry(lua_State* L);
	static int lDoMarriage(lua_State* L);
	static int lIsMarried(lua_State* L);
	static int lGetMarriageCost(lua_State* L);
#endif
	static int lGetBuyoutCost(lua_State* L);
	static int lCanMajorGiftTileImprovement(lua_State* L);
	static int lCanMajorGiftTileImprovementAtPlot(lua_State* L);
	static int lGetGiftTileImprovementCost(lua_State* L);
	static int lAddMinorCivQuestIfAble(lua_State* L);
	static int lGetFriendshipFromUnitGift(lua_State* L);

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
#if defined(MOD_API_EXTENSIONS) && defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(SetLeaderType, void, iNewLeader);
#endif
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

	static int lGetIncomingUnitType(lua_State* L);
	static int lGetIncomingUnitCountdown(lua_State* L);

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
	static int lGetNavalCombatExperience(lua_State* L);
	static int lChangeNavalCombatExperience(lua_State* L);
	static int lSetNavalCombatExperience(lua_State* L);

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
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(GetNumPuppetCities, int);
#endif
	static int lGetCityByID(lua_State* L);

	static int lGetUnits(lua_State* L);
	static int lGetNumUnits(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetNumUnitsNoCivlian(lua_State* L);
#endif
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

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(GetBuyPlotDistance, int);
	LUAAPIEXTN(GetWorkPlotDistance, int);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TRAITS_CITY_WORKING) || defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BUILDINGS_CITY_WORKING) || defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_POLICIES_CITY_WORKING) || defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TECHS_CITY_WORKING)
	LUAAPIEXTN(GetCityWorkingChange, int);
	LUAAPIEXTN(ChangeCityWorkingChange, void, iChange);
#endif

	// Diplomacy Stuff

	static int lDoBeginDiploWithHuman(lua_State* L);
	static int lDoTradeScreenOpened(lua_State* L);
	static int lDoTradeScreenClosed(lua_State* L);
	static int lGetMajorCivApproach(lua_State* L);
	static int lGetApproachTowardsUsGuess(lua_State* L);
	static int lIsWillAcceptPeaceWithPlayer(lua_State* L);
	static int lIsProtectingMinor(lua_State* L);
	static int lIsDontSettleMessageTooSoon(lua_State* L);
	static int lIsStopSpyingMessageTooSoon(lua_State* L);
	static int lIsAskedToStopConverting(lua_State* L);
	static int lIsAskedToStopDigging(lua_State* L);
	static int lIsDoFMessageTooSoon(lua_State* L);
	static int lIsDoF(lua_State* L);
	static int lGetDoFCounter(lua_State* L);
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
	static int lIsPlayerStopSpyingRequestEverAsked(lua_State* L);
	static int lIsDemandEverMade(lua_State* L);
	static int lGetNumCiviliansReturnedToMe(lua_State* L);
	static int lGetNumLandmarksBuiltForMe(lua_State* L);
	static int lGetNumTimesCultureBombed(lua_State* L);
	static int lGetNegativeReligiousConversionPoints(lua_State* L);
	static int lGetNegativeArchaeologyPoints(lua_State* L);
	static int lHasOthersReligionInMostCities(lua_State* L);
	static int lIsPlayerBrokenMilitaryPromise(lua_State* L);
	static int lIsPlayerIgnoredMilitaryPromise(lua_State* L);
	static int lIsPlayerBrokenExpansionPromise(lua_State* L);
	static int lIsPlayerIgnoredExpansionPromise(lua_State* L);
	static int lIsPlayerBrokenBorderPromise(lua_State* L);
	static int lIsPlayerIgnoredBorderPromise(lua_State* L);
	static int lIsPlayerBrokenAttackCityStatePromise(lua_State* L);
	static int lIsPlayerIgnoredAttackCityStatePromise(lua_State* L);
	static int lIsPlayerBrokenBullyCityStatePromise(lua_State* L);
	static int lIsPlayerIgnoredBullyCityStatePromise(lua_State* L);
	static int lIsPlayerBrokenSpyPromise(lua_State* L);
	static int lIsPlayerIgnoredSpyPromise(lua_State* L);
	static int lIsPlayerForgivenForSpying(lua_State* L);
	static int lIsPlayerBrokenNoConvertPromise(lua_State* L);
	static int lIsPlayerIgnoredNoConvertPromise(lua_State* L);
	static int lIsPlayerBrokenNoDiggingPromise(lua_State* L);
	static int lIsPlayerIgnoredNoDiggingPromise(lua_State* L);
	static int lIsPlayerBrokenCoopWarPromise(lua_State* L);
	static int lGetOtherPlayerNumProtectedMinorsKilled(lua_State* L);
	static int lGetOtherPlayerNumProtectedMinorsAttacked(lua_State* L);
	static int lGetTurnsSincePlayerBulliedProtectedMinor(lua_State* L);
	static int lIsHasPlayerBulliedProtectedMinor(lua_State* L);
	static int lIsDenouncedPlayer(lua_State* L);
	static int lGetDenouncedPlayerCounter(lua_State* L);
	static int lIsDenouncingPlayer(lua_State* L);
	static int lIsPlayerRecklessExpander(lua_State* L);
	static int lGetRecentTradeValue(lua_State* L);
	static int lGetCommonFoeValue(lua_State* L);
	static int lGetRecentAssistValue(lua_State* L);
	static int lIsGaveAssistanceTo(lua_State* L);
	static int lIsHasPaidTributeTo(lua_State* L);
	static int lIsNukedBy(lua_State* L);
	static int lIsCapitalCapturedBy(lua_State* L);
	static int lIsAngryAboutProtectedMinorKilled(lua_State* L);
	static int lIsAngryAboutProtectedMinorAttacked(lua_State* L);
	static int lIsAngryAboutProtectedMinorBullied(lua_State* L);
	static int lIsAngryAboutSidedWithTheirProtectedMinor(lua_State* L);
	static int lGetNumTimesRobbedBy(lua_State* L);
	static int lGetNumTimesIntrigueSharedBy(lua_State* L);

	static int lDoForceDoF(lua_State* L);
	static int lDoForceDenounce(lua_State* L);

#if defined(MOD_BALANCE_CORE_DEALS)
	LUAAPIEXTN(IsHasDefensivePact, bool);
	LUAAPIEXTN(IsHasDefensivePactWithPlayer, bool);
#endif
	// END Diplomacy Stuff

	static int lGetNumNotifications(lua_State* L);
	static int lGetNotificationStr(lua_State* L);
	static int lGetNotificationSummaryStr(lua_State* L);
	static int lGetNotificationIndex(lua_State* L);
	static int lGetNotificationTurn(lua_State* L);
	static int lGetNotificationDismissed(lua_State* L);
	static int lAddNotification(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS)
	static int lAddNotificationName(lua_State* L);
	LUAAPIEXTN(DismissNotification, void, iIndex, bUserInvoked);
#endif

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
	static int lGetNumFreeTenets(lua_State* L);
	static int lSetNumFreeTenets(lua_State* L);
	static int lChangeNumFreeTenets(lua_State* L);
	static int lGetNumFreeGreatPeople(lua_State* L);
	static int lSetNumFreeGreatPeople(lua_State* L);
	static int lChangeNumFreeGreatPeople(lua_State* L);
	static int lGetNumMayaBoosts(lua_State* L);
	static int lSetNumMayaBoosts(lua_State* L);
	static int lChangeNumMayaBoosts(lua_State* L);
	static int lGetNumFaithGreatPeople(lua_State* L);
	static int lSetNumFaithGreatPeople(lua_State* L);
	static int lChangeNumFaithGreatPeople(lua_State* L);
	static int lGetUnitBaktun(lua_State* L);
	static int lIsFreeMayaGreatPersonChoice(lua_State *L);
#if defined(MOD_BALANCE_CORE)
	static int lIsProphetValid(lua_State *L);
#endif

	static int lUnitsAux(lua_State* L);
	static int lUnits(lua_State* L);

	static int lCitiesAux(lua_State* L);
	static int lCities(lua_State* L);

	static int lHasReceivedNetTurnComplete(lua_State* L);
	static int lGetTraitGoldenAgeCombatModifier(lua_State* L);
	static int lGetTraitCityStateCombatModifier(lua_State* L);
	static int lGetTraitGreatGeneralExtraBonus(lua_State* L);
	static int lGetTraitGreatScientistRateModifier(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TRAITS_ANY_BELIEF)
	LUAAPIEXTN(IsTraitAnyBelief, bool);
#endif
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
	LUAAPIEXTN(IsBullyAnnex, bool);
#endif
	static int lIsTraitBonusReligiousBelief(lua_State* L);
	static int lGetHappinessFromLuxury(lua_State* L);
	static int lIsAbleToAnnexCityStates(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lIsDiplomaticMarriage(lua_State* L);
	static int lIsGPWLTKD(lua_State* L);
	static int lIsCarnaval(lua_State* L);
#endif
	static int lIsUsingMayaCalendar(lua_State* L);
	static int lGetMayaCalendarString(lua_State* L);
	static int lGetMayaCalendarLongString(lua_State* L);

	static int lGetExtraBuildingHappinessFromPolicies(lua_State* L);

#if defined(MOD_BALANCE_CORE_POLICIES)
	static int lGetExtraYieldWorldWonder(lua_State* L);
#endif

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
	static int lGetPolicyBuildingClassYieldChange(lua_State* L);
	static int lGetPolicyEspionageModifier(lua_State* L);
	static int lGetPolicyEspionageCatchSpiesModifier(lua_State* L);
	
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	LUAAPIEXTN(GetPolicyConversionModifier, int, iPolicyType);
#endif
	static int lGetPlayerBuildingClassYieldChange(lua_State* L);
	static int lGetPlayerBuildingClassHappiness(lua_State* L);

	static int lWasResurrectedBy(lua_State* L);
	static int lWasResurrectedThisTurnBy(lua_State* L);

	static int lGetOpinionTable(lua_State* L);
	static int lGetDealValue(lua_State* L);
	static int lGetDealMyValue(lua_State* L);
	static int lGetDealTheyreValue(lua_State* L);

	static int lMayNotAnnex(lua_State* L);

	//Espionage Methods
	static int lGetEspionageCityStatus(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetRandomIntrigue(lua_State* L);
	static int lGetCachedValueOfPeaceWithHuman(lua_State* L);
	static int lGetTotalValueToMe(lua_State* L);
#endif
	static int lGetNumSpies(lua_State* L);
	static int lGetNumUnassignedSpies(lua_State* L);
	static int lGetEspionageSpies(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_ESPIONAGE)
	LUAAPIEXTN(EspionageCreateSpy, void);
	LUAAPIEXTN(EspionagePromoteSpy, void, iSpyIndex);
	LUAAPIEXTN(EspionageSetPassive, void, iSpyIndex, bPassive);
	LUAAPIEXTN(EspionageSetOutcome, void, iSpyIndex, iSpyResult, bAffectsDiplomacy);
#endif
	static int lHasSpyEstablishedSurveillance(lua_State* L);
	static int lIsSpyDiplomat(lua_State* L);
	static int lIsSpySchmoozing(lua_State* L);
	static int lCanSpyStageCoup(lua_State* L);
	static int lGetAvailableSpyRelocationCities(lua_State* L);
	static int lGetNumTechsToSteal(lua_State* L);
	static int lGetIntrigueMessages(lua_State* L);
	static int lHasRecentIntrigueAbout(lua_State* L);
	static int lGetRecentIntrigueInfo(lua_State* L);
	static int lGetCoupChanceOfSuccess(lua_State* L);
	static int lIsMyDiplomatVisitingThem(lua_State* L);
	static int lIsOtherDiplomatVisitingMe(lua_State* L);
	// end Espionage Methods

	// International Trade
	static int lGetTradeRouteRange(lua_State* L);
	static int lGetInternationalTradeRoutePlotToolTip(lua_State* L);
	static int lGetInternationalTradeRoutePlotMouseoverToolTip(lua_State* L);
	static int lGetNumInternationalTradeRoutesUsed(lua_State* L);
	static int lGetNumInternationalTradeRoutesAvailable(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_TRADEROUTES)
    static int GetPotentialInternationalTradeRouteDestinationsHelper(lua_State* L, CvPlayerAI* pkPlayer, CvUnit* pkUnit, CvPlot* pkUnitPlot);
	LUAAPIEXTN(GetPotentialInternationalTradeRouteDestinationsFrom, table, pUnit, pCity);
#endif
	static int lGetPotentialInternationalTradeRouteDestinations(lua_State* L);
	static int lGetInternationalTradeRouteBaseBonus(lua_State* L);
	static int lGetInternationalTradeRouteGPTBonus(lua_State* L);
	static int lGetInternationalTradeRouteResourceBonus(lua_State* L);
	static int lGetInternationalTradeRouteResourceTraitModifier(lua_State* L);
	static int lGetInternationalTradeRouteExclusiveBonus(lua_State* L);
	static int lGetInternationalTradeRouteYourBuildingBonus(lua_State* L);
	static int lGetInternationalTradeRouteTheirBuildingBonus(lua_State* L);
	static int lGetInternationalTradeRoutePolicyBonus(lua_State* L);
	static int lGetInternationalTradeRouteOtherTraitBonus(lua_State* L);
	static int lGetInternationalTradeRouteRiverModifier(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetTradeConnectionOpenBordersModifierTimes100(lua_State* L);
	static int lGetInternationalTradeRouteCorporationModifier(lua_State* L);
	static int lGetInternationalTradeRouteCorporationModifierScience(lua_State* L);
	static int lGetNumberofGlobalFranchises(lua_State* L);
	static int lGetNumberofOffices(lua_State* L);
	static int lGetCorporationName(lua_State* L);
	static int lGetCorporationHelper(lua_State* L);
	static int lGetMaxFranchises(lua_State* L);
	static int lGetCorpID(lua_State* L);
	static int lGetCorporationHeadquarters(lua_State* L);
	static int lGetOfficeBuilding(lua_State* L);
	static int lGetFranchiseBuilding(lua_State* L);
	static int lGetCorporationFoundedTurn(lua_State* L);
	static int lGetCurrentOfficeBenefit(lua_State* L);
#endif
	static int lGetInternationalTradeRouteDomainModifier(lua_State* L);
	static int lGetInternationalTradeRouteTotal(lua_State* L);
	static int lGetInternationalTradeRouteScience(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetInternationalTradeRouteProduction(lua_State* L);
	static int lGetInternationalTradeRouteFood(lua_State* L);
	static int lGetMinorCivGoldBonus(lua_State* L);
	static int lIsConnectedToPlayer(lua_State* L);
	static int lIsConnectionBonus(lua_State* L);
#endif
	static int lGetPotentialTradeUnitNewHomeCity(lua_State* L);
	static int lGetPotentialAdmiralNewPort(lua_State* L);
	static int lGetNumAvailableTradeUnits(lua_State* L);
	static int lGetTradeUnitType(lua_State* L);
	static int lGetTradeYourRoutesTTString(lua_State* L);
	static int lGetTradeToYouRoutesTTString(lua_State* L);
	static int lGetTradeRoutes(lua_State* L);
	static int lGetTradeRoutesAvailable(lua_State* L);
	static int lGetTradeRoutesToYou(lua_State* L);
	static int lGetNumTechDifference(lua_State* L);
	// End international trade

	// culture
	static int lGetGreatWorks (lua_State* L);
	static int lGetOthersGreatWorks (lua_State* L);
	static int lGetSwappableGreatWriting (lua_State* L);
	static int lGetSwappableGreatArt (lua_State* L);
	static int lGetSwappableGreatArtifact (lua_State* L);
	static int lGetSwappableGreatMusic (lua_State* L);
	// end culture

	// Leagues
	static int lCanCommitVote(lua_State* L);
	static int lGetCommitVoteDetails(lua_State* L);

	static int lIsConnected(lua_State* L);
	static int lIsObserver(lua_State* L);
	static int lHasTurnTimerExpired(lua_State* L);
	static int lHasUnitOfClassType(lua_State* L);

	// Warmongering
	static int lGetWarmongerPreviewString(lua_State* L);
	static int lGetLiberationPreviewString(lua_State* L);

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(AddMessage, void, sMessage);
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CIV4_FEATURES)
	LUAAPIEXTN(GetVassalGoldMaintenance, int);
	LUAAPIEXTN(IsVassalageAcceptable, bool, iOtherPlayer);
	LUAAPIEXTN(GetYieldPerTurnFromVassals, int);
	LUAAPIEXTN(GetHappinessFromVassals, int);
	LUAAPIEXTN(GetScoreFromVassals, int);
	LUAAPIEXTN(GetMilitaryAggressivePosture, int, iOtherPlayer);
	LUAAPIEXTN(MoveRequestTooSoon, bool, iOtherPlayer);
	LUAAPIEXTN(GetPlayerMoveTroopsRequestCounter, int, iOtherPlayer);
	LUAAPIEXTN(GetExpensePerTurnFromVassalTaxes, int);
	LUAAPIEXTN(GetMyShareOfVassalTaxes, int);
	LUAAPIEXTN(GetVassalTaxContribution, int);
	LUAAPIEXTN(GetVassalScore, int);
	LUAAPIEXTN(GetVassalTreatedScore, int);
	LUAAPIEXTN(GetVassalDemandScore, int);
	LUAAPIEXTN(GetVassalTaxScore, int);
	LUAAPIEXTN(GetVassalProtectScore, int);
	LUAAPIEXTN(GetVassalFailedProtectScore, int);
	LUAAPIEXTN(GetVassalTreatmentLevel, int);
	LUAAPIEXTN(GetVassalTreatmentToolTip, CvString, iOtherPlayer);
	LUAAPIEXTN(GetVassalIndependenceTooltipAsMaster, CvString, iOtherPlayer);
	LUAAPIEXTN(GetVassalIndependenceTooltipAsVassal, CvString);
#endif
#if defined(MOD_BALANCE_CORE)
	LUAAPIEXTN(GetScoreFromMinorAllies, int);
	LUAAPIEXTN(GetScoreFromMilitarySize, int);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(HasBelief, bool, iBeliefType);
	LUAAPIEXTN(HasBuilding, bool, iBuildingType);
	LUAAPIEXTN(HasBuildingClass, bool, iBuildingClassType);
	LUAAPIEXTN(HasAnyWonder, bool);
	LUAAPIEXTN(HasWonder, bool, iBuildingType);
	LUAAPIEXTN(IsCivilization, bool, iCivilizationType);
	LUAAPIEXTN(IsInEra, bool, iEraType);
	LUAAPIEXTN(HasReachedEra, bool, iEraType);
	LUAAPIEXTN(HasAnyNaturalWonder, bool);
	LUAAPIEXTN(HasNaturalWonder, bool, iFeatureType);
	// LUAAPIEXTN(HasPolicy, bool, iPolicyType);
	LUAAPIEXTN(HasTenet, bool, iPolicyType);
	LUAAPIEXTN(HasPolicyBranch, bool, iPolicyBranchType);
	LUAAPIEXTN(HasIdeology, bool, iPolicyBranchType);
	LUAAPIEXTN(HasProject, bool, iProjectType);
	LUAAPIEXTN(IsAtPeace, bool);
	LUAAPIEXTN(IsAtPeaceAllMajors, bool);
	LUAAPIEXTN(IsAtPeaceAllMinors, bool);
	LUAAPIEXTN(IsAtPeaceWith, bool, iPlayer);
	LUAAPIEXTN(IsAtWar, bool);
	LUAAPIEXTN(IsAtWarAnyMajor, bool);
	LUAAPIEXTN(IsAtWarAnyMinor, bool);
	LUAAPIEXTN(IsAtWarWith, bool, iPlayer);
	LUAAPIEXTN(HasPantheon, bool);
	LUAAPIEXTN(HasAnyReligion, bool);
	LUAAPIEXTN(HasReligion, bool, iReligionType);
	LUAAPIEXTN(HasEnhancedReligion, bool);
	LUAAPIEXTN(IsConnectedTo, bool, iPlayer);
	LUAAPIEXTN(HasSpecialistSlot, bool, iSpecialistType);
	LUAAPIEXTN(HasSpecialist, bool, iSpecialistType);
	LUAAPIEXTN(HasTech, bool, iTechType);
	LUAAPIEXTN(HasAnyDomesticTradeRoute, bool);
	LUAAPIEXTN(HasAnyInternationalTradeRoute, bool);
	LUAAPIEXTN(HasAnyTradeRoute, bool);
	LUAAPIEXTN(HasAnyTradeRouteWith, bool, iPlayer);
	LUAAPIEXTN(HasUnit, bool, iUnitType);
	LUAAPIEXTN(HasUnitClass, bool, iUnitClassType);

	LUAAPIEXTN(HasTrait, bool, iTrait);
	LUAAPIEXTN(HasAnyHolyCity, bool);
	LUAAPIEXTN(HasHolyCity, bool, eReligion);
	LUAAPIEXTN(HasCapturedHolyCity, bool, eReligion);
	LUAAPIEXTN(HasEmbassyWith, bool, iOtherPlayer);
	LUAAPIEXTN(DoForceDefPact, void, iOtherPlayer);
	LUAAPIEXTN(CountAllFeature, int, iFeatureType);
	LUAAPIEXTN(CountAllWorkedFeature, int, iFeatureType);
	LUAAPIEXTN(CountAllImprovement, int, iImprovementType);
	LUAAPIEXTN(CountAllWorkedImprovement, int, iImprovementType);
	LUAAPIEXTN(CountAllPlotType, int, iPlotType);
	LUAAPIEXTN(CountAllWorkedPlotType, int, iPlotType);
	LUAAPIEXTN(CountAllResource, int, iResourceType);
	LUAAPIEXTN(CountAllWorkedResource, int, iResourceType);
	LUAAPIEXTN(CountAllTerrain, int, iTerrainType);
	LUAAPIEXTN(CountAllWorkedTerrain, int, iTerrainType);
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
	static int lGetScaledEventChoiceValue (lua_State* L);
	static int lIsEventChoiceActive (lua_State* L);
	static int lDoEventChoice (lua_State* L);
	static int lDoStartEvent (lua_State* L);
	static int lDoCancelEventChoice (lua_State* L);
	static int lGetEventCooldown  (lua_State* L);
	static int lSetEventCooldown (lua_State* L);
	static int lGetEventChoiceCooldown  (lua_State* L);
	static int lSetEventChoiceCooldown  (lua_State* L);
	static int lIsEventChoiceValid (lua_State* L);
#endif
};

#endif //CVLUAPLAYER_H
