/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaPlayer.cpp
//!  \brief     Private implementation to CvLuaPlayer.
//!
//!		This file includes the implementation for a Lua Player instance.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaCity.h"
#include "CvLuaPlayer.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#include "CvLuaDeal.h"
#include "../CvDiplomacyAI.h"
#include "../CvMinorCivAI.h"
#include "../CvDealClasses.h"
#include "../CvDealAI.h"
#include "../CvGameCoreUtils.h"
#include "../CvInternalGameCoreUtils.h"
#include "ICvDLLUserInterface.h"
#include "CvDllInterfaces.h"

// include this last to turn warnings into errors for code analysis
#include "LintFree.h"

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);

//------------------------------------------------------------------------------
void CvLuaPlayer::Register(lua_State* L)
{
	FLua::Details::CCallWithErrorHandling(L, pRegister);
}
//------------------------------------------------------------------------------
void CvLuaPlayer::PushMethods(lua_State* L, int t)
{
	Method(InitCity);
	Method(AcquireCity);
	Method(KillCities);

	Method(GetNewCityName);
	Method(IsCityNameValid);

	Method(InitUnit);
	Method(InitUnitWithNameOffset);
	Method(InitNamedUnit);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	Method(GetResourceMonopolyPlayer);
	Method(GetMonopolyPercent);
	Method(HasGlobalMonopoly);
	Method(HasStrategicMonopoly);
	Method(GetResourcesMisc);
	Method(IsShowImports);
#endif
	Method(DisbandUnit);
	Method(AddFreeUnit);

	Method(ChooseTech);

	Method(GetSpecificUnitType);
	Method(GetSpecificBuildingType);

	Method(KillUnits);
	Method(IsHuman);
	Method(IsBarbarian);
	Method(GetName);
	Method(GetNameKey);
	Method(GetNickName);
	Method(GetCivilizationDescription);
	Method(GetCivilizationDescriptionKey);
	Method(GetCivilizationShortDescription);
	Method(GetCivilizationShortDescriptionKey);
	Method(GetCivilizationAdjective);
	Method(GetCivilizationAdjectiveKey);
	Method(IsWhiteFlag);
	Method(GetStateReligionName);
	Method(GetStateReligionKey);
	Method(GetWorstEnemyName);
	Method(GetArtStyleType);

	Method(CountCityFeatures);
	Method(CountNumBuildings);

	Method(GetNumWorldWonders);
	Method(ChangeNumWorldWonders);
	Method(GetNumWondersBeatenTo);
	Method(SetNumWondersBeatenTo);

	Method(IsCapitalConnectedToCity);
	Method(IsPlotConnectedToPlot);

	Method(IsTurnActive);
	Method(IsSimultaneousTurns);

	Method(FindNewCapital);
	Method(CanRaze);
	Method(Raze);
	Method(Disband);

	Method(CanReceiveGoody);
	Method(ReceiveGoody);
	Method(DoGoody);
	Method(CanGetGoody);

	Method(CanFound);
	Method(Found);

	Method(CanTrain);
	Method(CanConstruct);
	Method(CanCreate);
	Method(CanPrepare);
	Method(CanMaintain);

	Method(IsCanPurchaseAnyCity);
	Method(GetFaithPurchaseType);
	Method(SetFaithPurchaseType);
	Method(GetFaithPurchaseIndex);
	Method(SetFaithPurchaseIndex);

	Method(IsProductionMaxedUnitClass);
	Method(IsProductionMaxedBuildingClass);
	Method(IsProductionMaxedProject);
	Method(GetUnitProductionNeeded);
	Method(GetBuildingProductionNeeded);
	Method(GetProjectProductionNeeded);

#if defined(MOD_PROCESS_STOCKPILE)
	Method(GetMaxStockpile);
#endif

	Method(HasReadyUnit);
	Method(GetFirstReadyUnit);
	Method(GetFirstReadyUnitPlot);

	Method(HasBusyUnit);
	Method(HasBusyMovingUnit);

	Method(GetBuildingClassPrereqBuilding);

	Method(RemoveBuildingClass);

	Method(CanBuild);
	Method(IsBuildBlockedByFeature);
	Method(GetBestRoute);
	Method(GetImprovementUpgradeRate);

	Method(CalculateTotalYield);

	Method(CalculateUnitCost);
	Method(CalculateUnitSupply);

	Method(GetNumMaintenanceFreeUnits);

	Method(GetBaseBuildingMaintenance);
	Method(GetBaseUnitMaintenance);
	Method(GetBuildingGoldMaintenance);
	Method(SetBaseBuildingGoldMaintenance);
	Method(ChangeBaseBuildingGoldMaintenance);

	Method(GetImprovementGoldMaintenance);
	Method(CalculateGoldRate);
	Method(CalculateGoldRateTimes100);
	Method(CalculateGrossGoldTimes100);
	Method(CalculateInflatedCosts);
	Method(CalculateResearchModifier);
	Method(IsResearch);
	Method(CanEverResearch);
	Method(CanResearch);
	Method(CanResearchForFree);
	Method(GetCurrentResearch);
	Method(IsCurrentResearchRepeat);
	Method(IsNoResearchAvailable);
	Method(GetResearchTurnsLeft);
	Method(GetResearchCost);
	Method(GetResearchProgress);

	Method(UnitsRequiredForGoldenAge);
	Method(UnitsGoldenAgeCapable);
	Method(UnitsGoldenAgeReady);
	Method(GreatGeneralThreshold);
	Method(GreatAdmiralThreshold);
	Method(SpecialistYield);
	Method(SetGreatGeneralCombatBonus);
	Method(GetGreatGeneralCombatBonus);

	Method(GetStartingPlot);
	Method(SetStartingPlot);
	Method(GetTotalPopulation);
	Method(GetAveragePopulation100);
	Method(GetRealPopulation);

	Method(GetNewCityExtraPopulation);
	Method(ChangeNewCityExtraPopulation);

	Method(GetTotalLand);
	Method(GetTotalLandScored);

	Method(GetGold);
	Method(SetGold);
	Method(ChangeGold);
	Method(CalculateGrossGold);
	Method(GetLifetimeGrossGold);
	Method(GetGoldFromCitiesTimes100);
	Method(GetGoldFromCitiesMinusTradeRoutesTimes100);
	Method(GetGoldPerTurnFromDiplomacy);
	Method(GetCityConnectionRouteGoldTimes100);
	Method(GetCityConnectionGold);
	Method(GetCityConnectionGoldTimes100);
	Method(GetGoldPerTurnFromReligion);
	Method(GetGoldPerTurnFromTradeRoutes);
	Method(GetGoldPerTurnFromTradeRoutesTimes100);
	Method(GetGoldPerTurnFromTraits);

	Method(GetInternalTradeRouteGoldBonus);
	//GAP
	Method(GetGAPFromReligion);
	Method(GetGAPFromCities);
	Method(GetGAPFromTraits);
	// Culture

	Method(GetTotalJONSCulturePerTurn);

	Method(GetJONSCulturePerTurnFromCities);

	Method(GetJONSCulturePerTurnFromExcessHappiness);
	Method(GetJONSCulturePerTurnFromTraits);

	Method(GetCultureWonderMultiplier);

	Method(GetJONSCulturePerTurnForFree);
	Method(ChangeJONSCulturePerTurnForFree);

	Method(GetCulturePerTurnFromMinorCivs);
	Method(GetCulturePerTurnFromMinor);

	Method(GetCulturePerTurnFromReligion);
	Method(GetCulturePerTurnFromBonusTurns);
	Method(GetCultureCityModifier);

	Method(GetJONSCulture);
	Method(SetJONSCulture);
	Method(ChangeJONSCulture);

	Method(GetJONSCultureEverGenerated);

	Method(GetLastTurnLifetimeCulture);
	Method(GetInfluenceOn);
	Method(GetLastTurnInfluenceOn);
	Method(GetInfluencePerTurn);
	Method(GetInfluenceLevel);
	Method(GetInfluenceTrend);
	Method(GetTurnsToInfluential);
	Method(GetNumCivsInfluentialOn);
	Method(GetNumCivsToBeInfluentialOn);
	Method(GetInfluenceTradeRouteScienceBonus);
	Method(GetInfluenceTradeRouteGoldBonus);
	Method(GetWoundedUnitDamageMod);
	Method(SetCapitalCity);
	Method(SetOriginalCapitalXY);
	Method(GetNumWonders);
	Method(GetOriginalCapitalPlot);
#if defined(MOD_BALANCE_CORE_POLICIES)
	Method(GetNoUnhappinessExpansion);
	Method(GetFractionOriginalCapitalsUnderControl);
	Method(GetTechDeviation);
	Method(GetTourismPenalty);
	Method(GetTechsToFreePolicy);
#endif
	Method(GetInfluenceCityStateSpyRankBonus);
	Method(GetInfluenceMajorCivSpyRankBonus);
	Method(GetInfluenceSpyRankTooltip);
	Method(GetTourism);
	Method(GetTourismModifierWith);
	Method(GetTourismModifierWithTooltip);
	Method(GetPublicOpinionType);
	Method(GetPublicOpinionPreferredIdeology);
	Method(GetPublicOpinionTooltip);
	Method(GetPublicOpinionUnhappiness);
	Method(GetPublicOpinionUnhappinessTooltip);
	Method(ChangeInfluenceOnAllPlayers);
	Method(ChangeInfluenceOnPlayer);

	Method(DoSwapGreatWorks);
	Method(DoSwapGreatWorksHuman);
	Method(HasAvailableGreatWorkSlot);
	Method(GetCityOfClosestGreatWorkSlot);
	Method(GetBuildingOfClosestGreatWorkSlot);
	Method(GetNextDigCompletePlot);
	Method(GetWrittenArtifactCulture);
	Method(GetNumGreatWorks);
	Method(GetNumGreatWorkSlots);

	// Faith

	Method(GetFaith);
	Method(SetFaith);
	Method(ChangeFaith);
	Method(GetTotalFaithPerTurn);
	Method(GetFaithPerTurnFromCities);
	Method(GetFaithPerTurnFromMinorCivs);
	Method(GetGoldPerTurnFromMinorCivs);
	Method(GetSciencePerTurnFromMinorCivs);
	Method(GetFaithPerTurnFromReligion);
	Method(HasCreatedPantheon);
	Method(GetBeliefInPantheon);
	Method(GetBeliefsInPantheon);
	Method(HasCreatedReligion);
	Method(CanCreatePantheon);
	Method(GetReligionCreatedByPlayer);
	Method(GetOriginalReligionCreatedByPlayer);
	Method(GetFoundedReligionEnemyCityCombatMod);
	Method(GetFoundedReligionFriendlyCityCombatMod);
#if defined(MOD_BALANCE_CORE_BELIEFS)
	Method(GetYieldPerTurnFromReligion);
#endif
	Method(GetMinimumFaithNextGreatProphet);
	Method(HasReligionInMostCities);
	Method(DoesUnitPassFaithPurchaseCheck);

	// Happiness

	Method(GetHappiness);
	Method(SetHappiness);
	Method(GetEmpireHappinessForCity);
	Method(GetEmpireUnhappinessForCity);
	Method(GetEmpireHappinessFromCities);
	Method(GetBonusHappinessFromLuxuriesFlat);
	Method(GetBonusHappinessFromLuxuriesFlatForUI);
	Method(GetHandicapHappiness);
	Method(GetHappinessForGAP);

	Method(GetExcessHappiness);
	Method(IsEmpireUnhappy);
	Method(IsEmpireVeryUnhappy);
	Method(IsEmpireSuperUnhappy);

	Method(GetHappinessFromPolicies);
	Method(GetHappinessFromCities);
	Method(GetHappinessFromBuildings);

	Method(GetExtraHappinessPerCity);
	Method(ChangeExtraHappinessPerCity);

	Method(GetHappinessFromResources);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	Method(GetHappinessFromResourceMonopolies);
	Method(GetUnhappinessFromCitizenNeeds);
	Method(GetHappinessFromCitizenNeeds);
#endif
	Method(GetHappinessFromResourceVariety);
	Method(GetExtraHappinessPerLuxury);
	Method(GetHappinessFromReligion);
	Method(GetHappinessFromNaturalWonders);
	Method(GetHappinessFromLeagues);

	Method(GetUnhappiness);
	Method(GetUnhappinessForecast);

	Method(GetUnhappinessFromCityForUI);
	Method(GetHappinessRatioRawPercent);

	Method(GetUnhappinessFromCityCount);
	Method(GetUnhappinessFromCapturedCityCount);
	Method(GetUnhappinessFromCityPopulation);
	Method(GetUnhappinessFromCitySpecialists);
	Method(GetUnhappinessFromOccupiedCities);
	Method(GetUnhappinessFromPuppetCityPopulation);
	Method(GetUnhappinessFromPublicOpinion);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	Method(GetUnhappinessFromWarWeariness);
	Method(GetWarWeariness);
	Method(SetWarWeariness);
	Method(GetWarWearinessSupplyReduction);
	Method(GetTechSupplyReduction);
#endif
	Method(GetUnhappinessFromUnits);
	Method(ChangeUnhappinessFromUnits);

	Method(GetUnhappinessMod);
	Method(GetCityCountUnhappinessMod);
	Method(GetOccupiedPopulationUnhappinessMod);
	Method(GetCapitalUnhappinessMod);
	Method(GetTraitCityUnhappinessMod);
	Method(GetTraitPopUnhappinessMod);
	Method(GetPuppetYieldPenalty);
	Method(IsHalfSpecialistUnhappiness);

	Method(GetHappinessPerGarrisonedUnit);
	Method(SetHappinessPerGarrisonedUnit);
	Method(ChangeHappinessPerGarrisonedUnit);

	Method(GetHappinessFromTradeRoutes);
	Method(GetHappinessPerTradeRoute);
	Method(SetHappinessPerTradeRoute);
	Method(ChangeHappinessPerTradeRoute);

	Method(GetCityConnectionTradeRouteGoldModifier);

	Method(GetHappinessFromMinorCivs);
	Method(GetHappinessFromMinor);

	// END Happiness

	Method(GetBarbarianCombatBonus);
	Method(SetBarbarianCombatBonus);
	Method(ChangeBarbarianCombatBonus);
	Method(GetCombatBonusVsHigherPop);
	Method(GetWarScore);
	Method(GetPlayerMilitaryStrengthComparedToUs);
	Method(GetPlayerEconomicStrengthComparedToUs);
	Method(GetWarDamageValue);
	Method(IsWantsPeaceWithPlayer);
	Method(GetTreatyWillingToOffer);
	Method(DoUpdateWarDamage);
	Method(DoUpdatePeaceTreatyWillingness);
	Method(GetDominationResistance);
	Method(GetCombatBonusVsHigherTech);
	Method(GetCombatBonusVsLargerCiv);

	Method(GetGarrisonedCityRangeStrikeModifier);
	Method(ChangeGarrisonedCityRangeStrikeModifier);

	Method(IsAlwaysSeeBarbCamps);
	Method(SetAlwaysSeeBarbCampsCount);
	Method(ChangeAlwaysSeeBarbCampsCount);

	Method(IsPolicyBlocked);
	Method(IsPolicyBranchBlocked);
	Method(IsPolicyBranchUnlocked);
	Method(SetPolicyBranchUnlocked);
	Method(GetNumPolicyBranchesUnlocked);
	Method(GetPolicyBranchChosen);
	Method(GetNumPolicyBranchesAllowed);
	Method(GetNumPolicies);
	Method(GetNumPoliciesInBranch);
	Method(GetNumPoliciesPurchasedInBranch);
	Method(HasPolicy);
	Method(SetHasPolicy);
	Method(GetNextPolicyCost);
	Method(CanAdoptPolicy);
	Method(DoAdoptPolicy);
	Method(CanUnlockPolicyBranch);
	Method(GetDominantPolicyBranchForTitle);
	Method(GetLateGamePolicyTree);
	Method(GetBranchPicked1);
	Method(GetBranchPicked2);
	Method(GetBranchPicked3);
	Method(GrantPolicy);
	Method(RevokePolicy);
	Method(SwapPolicy);
	Method(CanAdoptIdeology);
	Method(CanAdoptTenet);

	Method(GetPolicyCatchSpiesModifier);

	Method(GetNumPolicyBranchesFinished);
	Method(IsPolicyBranchFinished);

	Method(GetAvailableTenets);
	Method(GetTenet);

	Method(IsAnarchy);
	Method(GetAnarchyNumTurns);
	Method(SetAnarchyNumTurns);
	Method(ChangeAnarchyNumTurns);

	Method(GetAdvancedStartPoints);
	Method(SetAdvancedStartPoints);
	Method(ChangeAdvancedStartPoints);
	Method(GetAdvancedStartUnitCost);
	Method(GetAdvancedStartCityCost);
	Method(GetAdvancedStartPopCost);
	Method(GetAdvancedStartBuildingCost);
	Method(GetAdvancedStartImprovementCost);
	Method(GetAdvancedStartRouteCost);
	Method(GetAdvancedStartTechCost);
	Method(GetAdvancedStartVisibilityCost);

	Method(GetAttackBonusTurns);
	Method(GetCultureBonusTurns);
	Method(GetTourismBonusTurns);

	Method(GetGoldenAgeProgressThreshold);
	Method(GetGoldenAgeProgressMeter);
	Method(SetGoldenAgeProgressMeter);
	Method(ChangeGoldenAgeProgressMeter);
	Method(GetNumGoldenAges);
	Method(SetNumGoldenAges);
	Method(ChangeNumGoldenAges);
	Method(GetGoldenAgeTurns);
	Method(GetGoldenAgeLength);
	Method(IsGoldenAge);
	Method(ChangeGoldenAgeTurns);
	Method(GetNumUnitGoldenAges);
	Method(ChangeNumUnitGoldenAges);
	Method(GetStrikeTurns);
	Method(GetGoldenAgeModifier);
	Method(GetGoldenAgeTourismModifier);
	Method(GetGoldenAgeGreatWriterRateModifier);
	Method(GetGoldenAgeGreatArtistRateModifier);
	Method(GetGoldenAgeGreatMusicianRateModifier);
	Method(GetGoldenAgeGreatScientistRateModifier);
	Method(GetGoldenAgeGreatEngineerRateModifier);
	Method(GetGoldenAgeGreatMerchantRateModifier);
#if defined(MOD_DIPLOMACY_CITYSTATES) && defined(MOD_API_UNIFIED_YIELDS)
	Method(GetGoldenAgeGreatDiplomatRateModifier);
#endif

	Method(GetHurryModifier);

	Method(CreateGreatGeneral);
	Method(GetGreatPeopleCreated);
	Method(GetGreatGeneralsCreated);
	Method(GetGreatPeopleThresholdModifier);
	Method(GetGreatGeneralsThresholdModifier);
	Method(GetGreatAdmiralsThresholdModifier);
	Method(GetGreatPeopleRateModifier);
	Method(GetGreatGeneralRateModifier);
	Method(GetDomesticGreatGeneralRateModifier);
	Method(GetGreatWriterRateModifier);
	Method(GetGreatArtistRateModifier);
	Method(GetGreatMusicianRateModifier);
	Method(GetGreatScientistRateModifier);
	Method(GetGreatMerchantRateModifier);
	Method(GetGreatEngineerRateModifier);
#if defined(MOD_DIPLOMACY_CITYSTATES)
	Method(GetGreatDiplomatRateModifier);
	Method(GetScienceRateFromMinorAllies);
	Method(GetScienceRateFromLeagueAid);
	Method(GetLeagueCultureCityModifier);
#endif
	Method(GetArtsyGreatPersonRateModifier);
	Method(GetScienceyGreatPersonRateModifier);

	Method(GetPolicyGreatPeopleRateModifier);
	Method(GetPolicyGreatWriterRateModifier);
	Method(GetPolicyGreatArtistRateModifier);
	Method(GetPolicyGreatMusicianRateModifier);
	Method(GetPolicyGreatScientistRateModifier);
	Method(GetPolicyGreatMerchantRateModifier);
	Method(GetPolicyGreatEngineerRateModifier);
#if defined(MOD_DIPLOMACY_CITYSTATES)
	Method(GetPolicyGreatDiplomatRateModifier);
#endif

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	Method(GetMonopolyGreatPersonRateModifier);
	Method(GetMonopolyGreatPersonRateChange);
#endif

	Method(GetProductionModifier);
	Method(GetUnitProductionModifier);
	Method(GetBuildingProductionModifier);
	Method(GetProjectProductionModifier);
	Method(GetSpecialistProductionModifier);
	Method(GetMaxGlobalBuildingProductionModifier);
	Method(GetMaxTeamBuildingProductionModifier);
	Method(GetMaxPlayerBuildingProductionModifier);
	Method(GetFreeExperience);
	Method(GetFeatureProductionModifier);
	Method(GetWorkerSpeedModifier);
#if defined(MOD_CIV6_WORKER)
	Method(GetImprovementBuilderCost);
#endif
	Method(GetImprovementUpgradeRateModifier);
	Method(GetMilitaryProductionModifier);
	Method(GetSpaceProductionModifier);
	Method(GetSettlerProductionModifier);
	Method(GetCapitalSettlerProductionModifier);
	Method(GetWonderProductionModifier);

	Method(GetUnitProductionMaintenanceMod);
	Method(GetNumUnitsSupplied);
	Method(GetNumUnitsSuppliedByHandicap);
	Method(GetNumUnitsSuppliedByCities);
	Method(GetNumUnitsSuppliedByPopulation);
	Method(GetNumUnitsOutOfSupply);

	Method(GetCityDefenseModifier);
	Method(GetNumNukeUnits);
	Method(GetNumOutsideUnits);

	Method(GetGoldPerUnit);
	Method(ChangeGoldPerUnitTimes100);
	Method(GetGoldPerMilitaryUnit);
	Method(GetExtraUnitCost);
	Method(GetNumMilitaryUnits);
	Method(GetHappyPerMilitaryUnit);
	Method(IsMilitaryFoodProduction);
	Method(GetHighestUnitLevel);

	Method(GetConscriptCount);
	Method(SetConscriptCount);
	Method(ChangeConscriptCount);

	Method(GetMaxConscript);
	Method(GetOverflowResearch);
	Method(SetOverflowResearch);
	Method(ChangeOverflowResearch);
	Method(GetExpInBorderModifier);

	Method(GetLevelExperienceModifier);

	Method(GetCultureBombTimer);
	Method(GetConversionTimer);

	Method(GetCapitalCity);
	Method(IsHasLostCapital);
	Method(GetCitiesLost);

	Method(GetPower);
	Method(GetMilitaryMight);
	Method(GetMilitaryMightForCS);
	Method(GetTotalTimePlayed);

	Method(GetScore);
	Method(GetScoreFromCities);
	Method(GetScoreFromPopulation);
	Method(GetScoreFromLand);
	Method(GetScoreFromWonders);
	Method(GetScoreFromTechs);
	Method(GetScoreFromFutureTech);
	Method(ChangeScoreFromFutureTech);
	Method(GetScoreFromPolicies);
	Method(GetScoreFromGreatWorks);
	Method(GetScoreFromReligion);
	Method(GetScoreFromScenario1);
	Method(ChangeScoreFromScenario1);
	Method(GetScoreFromScenario2);
	Method(ChangeScoreFromScenario2);
	Method(GetScoreFromScenario3);
	Method(ChangeScoreFromScenario3);
	Method(GetScoreFromScenario4);
	Method(ChangeScoreFromScenario4);

	Method(IsGoldenAgeCultureBonusDisabled);

	Method(IsMajorCiv);
	Method(IsMinorCiv);
	Method(GetMinorCivType);
	Method(GetMinorCivTrait);
	Method(GetPersonality);
	Method(SetPersonality);
	Method(IsMinorCivHasUniqueUnit);
	Method(GetMinorCivUniqueUnit);
	Method(SetMinorCivUniqueUnit);
	Method(GetAlly);
	Method(GetAlliedTurns);
	Method(IsFriends);
	Method(IsAllies);
	Method(IsPlayerHasOpenBorders);
	Method(IsPlayerHasOpenBordersAutomatically);
	Method(GetFriendshipChangePerTurnTimes100);
	Method(GetMinorCivFriendshipWithMajor);
	Method(ChangeMinorCivFriendshipWithMajor);
	Method(GetMinorCivFriendshipAnchorWithMajor);
	Method(GetMinorCivFriendshipLevelWithMajor);
	Method(GetActiveQuestForPlayer);
	Method(IsMinorCivActiveQuestForPlayer);
	Method(SetMinorCivActiveQuestForPlayer);
	Method(GetMinorCivNumActiveQuestsForPlayer);
	Method(IsMinorCivDisplayedQuestForPlayer);
	Method(GetMinorCivNumDisplayedQuestsForPlayer);
	Method(GetQuestData1);
	Method(GetQuestData2);
	Method(GetQuestTurnsRemaining);
	Method(QuestSpyActionsRemaining);
	Method(GetXQuestBuildingRemaining);
	Method(GetExplorePercent);
	Method(GetXQuestBuildingRemaining);
	Method(GetRewardString);
	Method(GetExplorePercent);
	Method(GetTargetCityString);
	Method(IsMinorCivContestLeader);
	Method(GetMinorCivContestValueForLeader);
	Method(GetMinorCivContestValueForPlayer);
	Method(IsMinorCivUnitSpawningDisabled);
	Method(IsMinorCivRouteEstablishedWithMajor);
	Method(IsMinorWarQuestWithMajorActive);
	Method(GetMinorWarQuestWithMajorRemainingCount);
	Method(IsProxyWarActiveForMajor);
	Method(IsThreateningBarbariansEventActiveForPlayer);
	Method(GetTurnsSinceThreatenedByBarbarians);
	Method(GetTurnsSinceThreatenedAnnouncement);
	Method(GetFriendshipFromGoldGift);
	Method(GetFriendshipNeededForNextLevel);
	Method(GetMinorCivFavoriteMajor);
	Method(GetMinorCivScienceFriendshipBonus);
	Method(GetMinorCivCultureFriendshipBonus); // DEPRECATED
	Method(GetMinorCivCurrentCultureFlatBonus);
	Method(GetMinorCivCurrentCulturePerBuildingBonus);
	Method(GetCurrentCultureBonus); // DEPRECATED
	Method(GetMinorCivCurrentCultureBonus);
	Method(GetMinorCivHappinessFriendshipBonus); // DEPRECATED
	Method(GetMinorCivCurrentHappinessFlatBonus);
	Method(GetMinorCivCurrentHappinessPerLuxuryBonus);
	Method(GetMinorCivCurrentHappinessBonus);
	Method(GetMinorCivCurrentFaithBonus);
	Method(IsNoAlly);
	Method(GetPermanentAlly);
	Method(GetMinorCivCurrentGoldBonus);
	Method(GetMinorCivCurrentScienceBonus);
	Method(GetCurrentCapitalFoodBonus);
	Method(GetCurrentOtherCityFoodBonus);
	Method(GetCurrentSpawnEstimate);
	Method(GetCurrentScienceFriendshipBonusTimes100);
	Method(IsPeaceBlocked);
	Method(IsMinorPermanentWar);
	Method(GetNumMinorCivsMet);
	Method(DoMinorLiberationByMajor);
	Method(IsProtectedByMajor);
	Method(CanMajorProtect);
	Method(CanMajorStartProtection);
	Method(CanMajorWithdrawProtection);
	Method(GetTurnLastPledgedProtectionByMajor);
	Method(GetTurnLastPledgeBrokenByMajor);
	Method(GetMinorCivBullyGoldAmount);
	Method(SetBullyUnit);
	Method(GetBullyUnit);
	Method(GetYieldTheftAmount);
	Method(GetPledgeProtectionInvalidReason);
	Method(CanMajorBullyGold);
	Method(GetMajorBullyGoldDetails);
	Method(CanMajorBullyUnit);
	Method(GetMajorBullyUnitDetails);
	Method(GetMajorBullyValue);
	Method(CanMajorBuyout);
	Method(CanMajorMarry);
	Method(DoMarriage);
	Method(IsMarried);
	Method(GetMarriageCost);
	Method(GetBuyoutCost);
	Method(CanMajorGiftTileImprovement);
	Method(CanMajorGiftTileImprovementAtPlot);
	Method(GetGiftTileImprovementCost);
	Method(AddMinorCivQuestIfAble);
	Method(GetFriendshipFromUnitGift);
#if defined(MOD_BALANCE_CORE_MINORS)
	Method(GetJerk);
	Method(GetCoupCooldown);
#endif
	Method(GetNumDenouncements);
	Method(GetNumDenouncementsOfPlayer);
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	Method(GetUnhappinessFromCityCulture);
	Method(GetUnhappinessFromCityScience);
	Method(GetUnhappinessFromCityDefense);
	Method(GetUnhappinessFromCityGold);
	Method(GetUnhappinessFromCityConnection);
	Method(GetUnhappinessFromCityPillaged);
	Method(GetUnhappinessFromCityStarving);
	Method(GetUnhappinessFromCityMinority);
#endif
	Method(GetUnhappinessFromJFDSpecial);
#if defined(MOD_BALANCE_CORE_HAPPINESS_LUXURY)
	Method(GetBonusHappinessFromLuxuries);
	Method(GetScalingNationalPopulationRequrired);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
	Method(GetPuppetUnhappinessMod);
	Method(GetCapitalUnhappinessModCBP);
#endif

	Method(IsAlive);
	Method(IsEverAlive);
	Method(IsPotentiallyAlive);
	Method(IsExtendedGame);
	Method(IsFoundedFirstCity);

	Method(GetEndTurnBlockingType);
	Method(GetEndTurnBlockingNotificationIndex);
	Method(HasReceivedNetTurnComplete);
	Method(IsStrike);

	Method(GetID);
	Method(GetHandicapType);
	Method(GetCivilizationType);
	Method(GetLeaderType);
	Method(SetLeaderType);
	Method(GetPersonalityType);
	Method(SetPersonalityType);
	Method(GetCurrentEra);

	Method(GetTeam);

	Method(GetPlayerColor);
	Method(GetPlayerColors);

	Method(GetSeaPlotYield);
	Method(GetYieldRateModifier);
	Method(GetCapitalYieldRateModifier);
	Method(GetExtraYieldThreshold);

	// Science

	Method(GetScience);
	Method(GetScienceTimes100);

	Method(GetScienceFromCitiesTimes100);
	Method(GetScienceFromOtherPlayersTimes100);
	Method(GetScienceFromHappinessTimes100);
	Method(GetScienceFromResearchAgreementsTimes100);
	Method(GetScienceFromBudgetDeficitTimes100);

	// END Science

	Method(GetProximityToPlayer);
	Method(DoUpdateProximityToPlayer);

	Method(GetIncomingUnitType);
	Method(GetIncomingUnitCountdown);

	Method(IsOption);
	Method(SetOption);
	Method(IsPlayable);
	Method(SetPlayable);

	Method(GetNumResourceUsed);
	Method(GetNumResourceTotal);
	Method(ChangeNumResourceTotal);
	Method(GetNumResourceAvailable);

	Method(GetResourceExport);
	Method(GetResourceImport);
	Method(GetResourceFromMinors);

	Method(GetImprovementCount);

	Method(IsBuildingFree);
	Method(GetUnitClassCount);
	Method(IsUnitClassMaxedOut);
	Method(GetUnitClassMaking);
	Method(GetUnitClassCountPlusMaking);

	Method(GetBuildingClassCount);
	Method(IsBuildingClassMaxedOut);
	Method(GetBuildingClassMaking);
	Method(GetBuildingClassCountPlusMaking);
	Method(GetHurryCount);
	Method(IsHasAccessToHurry);
	Method(IsCanHurry);
	Method(GetHurryGoldCost);

	//Method(IsSpecialistValid);
	Method(IsResearchingTech);
	Method(SetResearchingTech);

	Method(GetCombatExperience);
	Method(ChangeCombatExperience);
	Method(SetCombatExperience);
	Method(GetLifetimeCombatExperience);
	Method(GetNavalCombatExperience);
	Method(ChangeNavalCombatExperience);
	Method(SetNavalCombatExperience);

	Method(GetSpecialistExtraYield);

	Method(FindPathLength);

	Method(GetQueuePosition);
	Method(ClearResearchQueue);
	Method(PushResearch);
	Method(PopResearch);
	Method(GetLengthResearchQueue);
	Method(AddCityName);
	Method(GetNumCityNames);
	Method(GetCityName);

	Method(Cities);
	Method(GetNumCities);
	Method(GetCityByID);
	Method(GetNumPuppetCities);

	Method(Units);
	Method(GetNumUnits);
	Method(GetNumUnitsToSupply);
	Method(GetNumUnitsOfType);
	Method(GetNumUnitPromotions);
	Method(GetUnitByID);

	Method(AI_updateFoundValues);
	Method(AI_foundValue);

	Method(GetScoreHistory);
	Method(GetEconomyHistory);
	Method(GetIndustryHistory);
	Method(GetAgricultureHistory);
	Method(GetPowerHistory);

	Method(GetReplayData);
	Method(SetReplayDataValue);

	Method(GetScriptData);
	Method(SetScriptData);

	Method(GetNumPlots);

	Method(GetNumPlotsBought);
	Method(SetNumPlotsBought);
	Method(ChangeNumPlotsBought);

	Method(GetBuyPlotCost);
	Method(GetPlotDanger);

	Method(GetBuyPlotDistance);
	Method(GetWorkPlotDistance);

#if defined(MOD_TRAITS_CITY_WORKING) || defined(MOD_BUILDINGS_CITY_WORKING) || defined(MOD_POLICIES_CITY_WORKING) || defined(MOD_TECHS_CITY_WORKING)
	Method(GetCityWorkingChange);
	Method(ChangeCityWorkingChange);
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS) || defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS) || defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS) || defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	Method(GetCityAutomatonWorkersChange);
	Method(ChangeCityAutomatonWorkersChange);
#endif

	Method(DoBeginDiploWithHuman);
	Method(DoTradeScreenOpened);
	Method(DoTradeScreenClosed);
	Method(GetMajorCivApproach);
	Method(GetApproachTowardsUsGuess);
	Method(IsWillAcceptPeaceWithPlayer);
	Method(IsProtectingMinor);
	Method(IsDontSettleMessageTooSoon);
	Method(IsStopSpyingMessageTooSoon);
	Method(IsAskedToStopConverting);
	Method(IsAskedToStopDigging);
	Method(IsDoFMessageTooSoon);
	Method(IsDoF);
	Method(GetDoFCounter);
	Method(IsPlayerDoFWithAnyFriend);
	Method(IsPlayerDoFWithAnyEnemy);
	Method(IsPlayerDenouncedFriend);
	Method(IsPlayerDenouncedEnemy);
	Method(IsUntrustworthyFriend);
	Method(GetNumFriendsDenouncedBy);
	Method(IsFriendDenouncedUs);
	Method(GetWeDenouncedFriendCount);
	Method(IsFriendDeclaredWarOnUs);
	Method(GetWeDeclaredWarOnFriendCount);
	Method(CanRequestCoopWar);
	Method(GetCoopWarAcceptedState);
	Method(GetNumWarsFought);

	Method(GetLandDisputeLevel);
	Method(GetVictoryDisputeLevel);
	Method(GetWonderDisputeLevel);
	Method(GetMinorCivDisputeLevel);
	Method(GetWarmongerThreat);
	Method(IsDemandEverMade);
	Method(GetNumCiviliansReturnedToMe);
	Method(GetNumLandmarksBuiltForMe);
	Method(GetNumTimesCultureBombed);
	Method(GetNegativeReligiousConversionPoints);
	Method(GetNegativeArchaeologyPoints);
	Method(HasOthersReligionInMostCities);
	Method(IsPlayerBrokenMilitaryPromise);
	Method(IsPlayerIgnoredMilitaryPromise);
	Method(IsPlayerBrokenExpansionPromise);
	Method(IsPlayerIgnoredExpansionPromise);
	Method(IsPlayerBrokenBorderPromise);
	Method(IsPlayerIgnoredBorderPromise);
	Method(IsPlayerBrokenAttackCityStatePromise);
	Method(IsPlayerIgnoredAttackCityStatePromise);
	Method(IsPlayerBrokenBullyCityStatePromise);
	Method(IsPlayerIgnoredBullyCityStatePromise);
	Method(IsPlayerBrokenSpyPromise);
	Method(IsPlayerIgnoredSpyPromise);
	Method(IsPlayerForgivenForSpying);
	Method(IsPlayerBrokenNoConvertPromise);
	Method(IsPlayerIgnoredNoConvertPromise);
	Method(IsPlayerBrokenNoDiggingPromise);
	Method(IsPlayerIgnoredNoDiggingPromise);
	Method(IsPlayerBrokenCoopWarPromise);
	Method(GetOtherPlayerNumProtectedMinorsKilled);
	Method(GetOtherPlayerNumProtectedMinorsAttacked);
	Method(GetTurnsSincePlayerBulliedProtectedMinor);
	Method(IsHasPlayerBulliedProtectedMinor);
	Method(IsDenouncedPlayer);
	Method(GetDenouncedPlayerCounter);
	Method(IsDenouncingPlayer);
	Method(IsDenounceMessageTooSoon);
	Method(IsPlayerRecklessExpander);
	Method(GetRecentTradeValue);
	Method(GetCommonFoeValue);
	Method(GetRecentAssistValue);
	Method(IsHasPaidTributeTo);
	Method(IsNukedBy);
	Method(IsCapitalCapturedBy);
	Method(IsAngryAboutProtectedMinorKilled);
	Method(IsAngryAboutProtectedMinorAttacked);
	Method(IsAngryAboutProtectedMinorBullied);
	Method(IsAngryAboutSidedWithTheirProtectedMinor);
	Method(GetNumTimesRobbedBy);
	Method(GetNumTimesIntrigueSharedBy);

	Method(DoForceDoF);
	Method(DoForceDenounce);

	Method(IsHasDefensivePact);
	Method(IsHasDefensivePactWithPlayer);

	Method(GetNumTurnsMilitaryPromise);
	Method(GetNumTurnsExpansionPromise);
	Method(GetNumTurnsBorderPromise);

	Method(GetNumNotifications);
	Method(GetNotificationStr);
	Method(GetNotificationSummaryStr);
	Method(GetNotificationIndex);
	Method(GetNotificationTurn);
	Method(GetNotificationDismissed);
	Method(AddNotification);
	Method(AddNotificationName);
	Method(DismissNotification);

	Method(GetRecommendedWorkerPlots);
	Method(GetRecommendedFoundCityPlots);
	Method(GetUnimprovedAvailableLuxuryResource);
	Method(IsAnyPlotImproved);
	Method(GetPlayerVisiblePlot);

	Method(GetEverPoppedGoody);
	Method(GetClosestGoodyPlot);
	Method(IsAnyGoodyPlotAccessible);
	Method(GetPlotHasOrder);
	Method(GetAnyUnitHasOrderToGoody);
	Method(GetEverTrainedBuilder);

	Method(GetNumFreeTechs);
	Method(SetNumFreeTechs);
	Method(GetNumFreePolicies);
	Method(SetNumFreePolicies);
	Method(ChangeNumFreePolicies);
	Method(GetNumFreeTenets);
	Method(SetNumFreeTenets);
	Method(ChangeNumFreeTenets);
	Method(GetNumFreeGreatPeople);
	Method(SetNumFreeGreatPeople);
	Method(ChangeNumFreeGreatPeople);
	Method(GetNumMayaBoosts);
	Method(SetNumMayaBoosts);
	Method(ChangeNumMayaBoosts);
	Method(GetNumFaithGreatPeople);
	Method(SetNumFaithGreatPeople);
	Method(ChangeNumFaithGreatPeople);
	Method(GetUnitBaktun);
	Method(IsFreeMayaGreatPersonChoice);
	Method(IsProphetValid);
	Method(GetTraitGoldenAgeCombatModifier);
	Method(GetTraitCityStateCombatModifier);
	Method(GetTraitGreatGeneralExtraBonus);
	Method(GetTraitGreatScientistRateModifier);
#if defined(MOD_TRAITS_ANY_BELIEF)
	Method(IsTraitAnyBelief);
#endif
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
	Method(IsBullyAnnex);
#endif
	Method(IsTraitBonusReligiousBelief);
	Method(GetHappinessFromLuxury);
	Method(IsAbleToAnnexCityStates);
	Method(IsDiplomaticMarriage);
	Method(IsGPWLTKD);
	Method(IsCarnaval);
	Method(GetTraitConquestOfTheWorldCityAttackMod);
	Method(IsUsingMayaCalendar);
	Method(GetMayaCalendarString);
	Method(GetMayaCalendarLongString);

	Method(GetExtraBuildingHappinessFromPolicies);

#if defined(MOD_BALANCE_CORE_POLICIES)
	Method(GetExtraYieldWorldWonder);
#endif

	Method(GetNextCity);
	Method(GetPrevCity);

	Method(GetFreePromotionCount);
	Method(IsFreePromotion);
	Method(ChangeFreePromotionCount);

	Method(GetEmbarkedGraphicOverride);
	Method(SetEmbarkedGraphicOverride);

	Method(AddTemporaryDominanceZone);

	Method(GetNaturalWonderYieldModifier);

	Method(GetPolicyBuildingClassYieldModifier);
	Method(GetPolicyBuildingClassYieldChange);
	Method(GetPolicyEspionageModifier);
	Method(GetPolicyEspionageCatchSpiesModifier);

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	Method(GetPolicyConversionModifier);
#endif

	Method(GetPlayerBuildingClassYieldChange);
	Method(GetPlayerBuildingClassHappiness);

	Method(WasResurrectedBy);
	Method(WasResurrectedThisTurnBy);

	Method(GetOpinionTable);
	Method(GetDealValue);
	Method(GetDealMyValue);
	Method(GetDealTheyreValue);
	Method(MayNotAnnex);

	Method(GetEspionageCityStatus);
	Method(GetTotalValueToMeNormal);
	Method(GetTotalValueToMe);
	Method(GetRandomIntrigue);
	Method(GetCachedValueOfPeaceWithHuman);
	Method(GetSpyChanceAtCity);
	Method(GetCityPotentialInfo);
	Method(GetCityWithSpy);
	Method(GetNumSpies);
	Method(GetNumUnassignedSpies);
	Method(GetEspionageSpies);
	Method(EspionageCreateSpy);
	Method(EspionagePromoteSpy);
	Method(EspionageSetPassive);
	Method(EspionageSetOutcome);
	Method(HasSpyEstablishedSurveillance);
	Method(IsSpyDiplomat);
	Method(IsSpySchmoozing);
	Method(CanSpyStageCoup);
	Method(GetAvailableSpyRelocationCities);
	Method(GetNumTechsToSteal);
	Method(GetIntrigueMessages);
	Method(HasRecentIntrigueAbout);
	Method(GetRecentIntrigueInfo);
	Method(GetCoupChanceOfSuccess);
	Method(IsMyDiplomatVisitingThem);
	Method(IsOtherDiplomatVisitingMe);

	Method(GetTradeRouteRange);
	Method(GetInternationalTradeRoutePlotToolTip);
	Method(GetInternationalTradeRoutePlotMouseoverToolTip);
	Method(GetNumInternationalTradeRoutesUsed);
	Method(GetNumInternationalTradeRoutesAvailable);
	Method(GetPotentialInternationalTradeRouteDestinationsFrom);
	Method(GetPotentialInternationalTradeRouteDestinations);
	Method(GetInternationalTradeRouteBaseBonus);
	Method(GetInternationalTradeRouteGPTBonus);
	Method(GetInternationalTradeRouteResourceBonus);
	Method(GetCityResourceBonus);
	Method(GetInternationalTradeRouteResourceTraitModifier);
	Method(GetInternationalTradeRouteExclusiveBonus);
	Method(GetInternationalTradeRouteYourBuildingBonus);
	Method(GetInternationalTradeRouteTheirBuildingBonus);
	Method(GetInternationalTradeRoutePolicyBonus);
	Method(GetInternationalTradeRouteOtherTraitBonus);
	Method(GetInternationalTradeRouteRiverModifier);
	Method(GetTradeRouteTurns);
	Method(GetTradeConnectionDistanceValueModifierTimes100);
	Method(GetTradeRouteTurns);
	Method(GetTradeConnectionDistance);
	Method(GetTradeConnectionOpenBordersModifierTimes100);
	Method(GetInternationalTradeRouteCorporationModifier);
	Method(GetInternationalTradeRouteCorporationModifierScience);
	Method(GetNumberofGlobalFranchises);
	Method(GetNumberofOffices);
	Method(GetMaxFranchises);
	Method(GetCorporation);
	Method(GetCorporationFoundedTurn);
	Method(GetCurrentOfficeBenefit);
	Method(GetTradeRouteBenefitHelper);
	Method(GetNumFranchisesTooltip);
	Method(CanCreateFranchiseInCity);
	Method(GetInternationalTradeRouteDomainModifier);
	Method(GetTradeRouteYieldModifier);
	Method(GetInternationalTradeRouteTotal);
	Method(GetInternationalTradeRouteScience);
	Method(GetInternationalTradeRouteCulture);
	Method(GetInternationalTradeRouteProduction);
	Method(GetInternationalTradeRouteFood);
	Method(GetMinorCivGoldBonus);
	Method(IsConnectedToPlayer);
	Method(IsConnectionBonus);
	Method(GetPotentialTradeUnitNewHomeCity);
	Method(GetPotentialAdmiralNewPort);
	Method(GetNumAvailableTradeUnits);
	Method(GetTradeUnitType);
	Method(GetTradeYourRoutesTTString);
	Method(GetTradeToYouRoutesTTString);
	Method(GetTradeRoutes);
	Method(GetTradeRoutesAvailable);
	Method(GetTradeRoutesToYou);
	Method(GetNumTechDifference);
	Method(GetNumPolicyDifference);

	// Culture functions. Not sure where they should go
	Method(GetGreatWorks);
	Method(GetSwappableGreatWriting);
	Method(GetSwappableGreatArt);
	Method(GetSwappableGreatArtifact);
	Method(GetSwappableGreatMusic);
	Method(GetOthersGreatWorks);

	Method(CanCommitVote);
	Method(GetCommitVoteDetails);

	Method(IsConnected);
	Method(IsObserver);

	Method(HasTurnTimerExpired);

	Method(HasUnitOfClassType);

	Method(GetWarmongerPreviewString);
	Method(GetLiberationPreviewString);

	Method(AddMessage);

	Method(IsVassalageAcceptable);
	Method(GetVassalGoldMaintenance);
	Method(GetYieldPerTurnFromVassals);
	Method(GetHappinessFromVassals);
	Method(GetScoreFromVassals);
	Method(GetMilitaryAggressivePosture);
	Method(MoveRequestTooSoon);
	Method(GetPlayerMoveTroopsRequestCounter);
	Method(GetMyShareOfVassalTaxes);
	Method(GetExpensePerTurnFromVassalTaxes);
	Method(GetVassalTaxContribution);
	Method(GetVassalScore);
	Method(GetVassalTreatedScore);
	Method(GetVassalDemandScore);
	Method(GetVassalTaxScore);
	Method(GetVassalProtectScore);
	Method(GetVassalFailedProtectScore);
	Method(GetVassalTreatmentLevel);
	Method(GetVassalTreatmentToolTip);
	Method(GetVassalIndependenceTooltipAsMaster);
	Method(GetVassalIndependenceTooltipAsVassal);
	Method(GetYieldPerTurnFromMinors);
	Method(SetYieldPerTurnFromMinors);
	Method(GetScoreFromMinorAllies);
	Method(GetScoreFromMilitarySize);
	Method(HasBelief);
	Method(HasBuilding);
	Method(HasBuildingClass);
	Method(HasAnyWonder);
	Method(HasWonder);
	Method(IsCivilization);
	Method(IsInEra);
	Method(HasReachedEra);
	Method(HasAnyNaturalWonder);
	Method(HasNaturalWonder);
	// Method(HasPolicy);
	Method(HasTenet);
	Method(HasPolicyBranch);
	Method(HasIdeology);
	Method(HasProject);
	Method(IsAtPeace);
	Method(IsAtPeaceAllMajors);
	Method(IsAtPeaceAllMinors);
	Method(IsAtPeaceWith);
	Method(IsAtWar);
	Method(IsAtWarAnyMajor);
	Method(IsAtWarAnyMinor);
	Method(IsAtWarWith);
	Method(HasPantheon);
	Method(HasAnyReligion);
	Method(HasReligion);
	Method(HasEnhancedReligion);
	Method(IsConnectedTo);
	Method(HasSpecialistSlot);
	Method(HasSpecialist);
	Method(HasTech);
	Method(HasAnyDomesticTradeRoute);
	Method(HasAnyInternationalTradeRoute);
	Method(HasAnyTradeRoute);
	Method(HasAnyTradeRouteWith);
	Method(HasUnit);
	Method(HasUnitClass);

	Method(HasTrait);
	Method(HasAnyHolyCity);
	Method(HasHolyCity);
	Method(HasCapturedHolyCity);
	Method(HasEmbassyWith);
	Method(DoForceDefPact);
	Method(GetCivOpinion);
	Method(GetMajorityReligion);
	//JFD
	Method(GetWLTKDResourceTT);
	Method(GetNumNationalWonders);
	Method(GetNumInternationalTradeRoutes);
	Method(GetNumInternalTradeRoutes);
	Method(GetStateReligion);
	Method(GetNumCitiesWithStateReligion);
	Method(GetNumCitiesWithoutStateReligion);
	Method(SetStateReligion);

	Method(SetPiety);
	Method(ChangePiety);
	Method(GetPiety);
	Method(GetPietyRate);
	Method(SetPietyRate);
	Method(ChangePietyRate);
	Method(GetTurnsSinceConversion);
	Method(SetTurnsSinceConversion);
	Method(HasStateReligion);
	Method(HasSecularized);
	Method(SetHasSecularized);
	Method(IsPagan);

	Method(GetSovereignty);
	Method(SetSovereignty);
	Method(ChangeSovereignty);

	Method(GetGovernment);
	Method(SetGovernment);
	Method(HasGovernment);

	Method(GetReformCooldown);
	Method(SetReformCooldown);
	Method(ChangeReformCooldown);

	Method(GetGovernmentCooldown);
	Method(SetGovernmentCooldown);
	Method(ChangeGovernmentCooldown);

	Method(GetReformCooldownRate);
	Method(SetReformCooldownRate);
	Method(ChangeReformCooldownRate);

	Method(GetGovernmentCooldownRate);
	Method(SetGovernmentCooldownRate);
	Method(ChangeGovernmentCooldownRate);

	Method(GetPoliticLeader);
	Method(SetPoliticLeader);

	Method(SetPoliticLeaderKey);
	Method(GetPoliticLeaderKey);

	Method(GetLegislatureName);
	Method(SetLegislatureName);

	Method(GetPoliticPercent);
	Method(SetPoliticPercent);

	Method(GetCurrency);
	Method(SetCurrency);
	
	Method(HasCurrency);

	Method(SetCurrencyName);
	Method(GetCurrencyName);

	Method(SetProsperityScore);
	Method(GetProsperityScore);

	//JFD Contracts
	Method(PlayerHasContract);
	Method(PlayerHasAnyContract);
	Method(GetContractTurnsRemaining);
	Method(GetContractGoldMaintenance);
	Method(ChangeContractTurns);
	Method(StartContract);
	Method(EndContract);
	Method(UnitIsActiveContractUnit);
	Method(GetNumActivePlayerContracts);
	Method(DisbandContractUnits);
	Method(InitContractUnits);

	//Other

	Method(CountAllFeature);
	Method(CountAllWorkedFeature);
	Method(CountAllImprovement);
	Method(CountAllWorkedImprovement);
	Method(CountAllPlotType);
	Method(CountAllWorkedPlotType);
	Method(CountAllResource);
	Method(CountAllWorkedResource);
	Method(CountAllTerrain);
	Method(CountAllWorkedTerrain);

#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
	Method(GetResponsibleForRouteCount);
	Method(GetResponsibleForImprovementCount);
#endif
	Method(DoInstantYield);
	Method(GetInstantYieldHistoryTooltip);
#if defined(MOD_BALANCE_CORE_EVENTS)
	Method(GetDisabledTooltip);
	Method(GetEspionageValues);
	Method(GetScaledEventChoiceValue);
	Method(IsEventChoiceActive);
	Method(DoEventChoice);
	Method(DoStartEvent);
	Method(DoCancelEventChoice);
	Method(GetEventCooldown);
	Method(SetEventCooldown);
	Method(GetEventChoiceCooldown);
	Method(SetEventChoiceCooldown);
	Method(IsEventChoiceValid);
	Method(GetEventHappiness);
	Method(GetActivePlayerEventChoices);
	Method(GetActiveCityEventChoices);
	Method(GetRecentPlayerEventChoices);
	Method(GetRecentCityEventChoices);
#endif

#if defined(MOD_BATTLE_ROYALE)
	Method(GetNumMilitarySeaUnits);
	Method(GetNumMilitaryAirUnits);
	Method(GetNumMilitaryLandUnits);
	Method(GetMilitarySeaMight);
	Method(GetMilitaryAirMight);
	Method(GetMilitaryLandMight);
#endif
}
//------------------------------------------------------------------------------
void CvLuaPlayer::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}
//------------------------------------------------------------------------------
const char* CvLuaPlayer::GetTypeName()
{
	return "Player";
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------
int CvLuaPlayer::pRegister(lua_State* L)
{
	lua_getglobal(L, "Players");
	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "Players");
	}

	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayerAI* pkPlayer = &(GET_PLAYER((PlayerTypes)i));
		CvLuaPlayer::Push(L, pkPlayer);
		lua_rawseti(L, -2, i);
	}

	return 0;
}
//------------------------------------------------------------------------------
//CvCity* initCity(int x, int y, bBumpUnits = true);
int CvLuaPlayer::lInitCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);
	const bool bBumpUnits = luaL_optint(L, 4, 1);
	const bool bInitialFounding = luaL_optint(L, 5, 1);
	const ReligionTypes eInitialReligion = (ReligionTypes) luaL_optint(L, 6, NO_RELIGION);
	CvCity* pkCity = pkPlayer->initCity(x, y, bBumpUnits, bInitialFounding, eInitialReligion);
	pkPlayer->DoUpdateNextPolicyCost();
	CvLuaCity::Push(L, pkCity);
	return 1;
}
//------------------------------------------------------------------------------
//void acquireCity(CyCity* pCity, bool bConquest, bool bTrade);
int CvLuaPlayer::lAcquireCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	const bool bConquest = lua_toboolean(L, 3);
	const bool bTrade = lua_toboolean(L, 4);

	pkPlayer->acquireCity(pkCity, bConquest, bTrade);
	return 0;
}
//------------------------------------------------------------------------------
//void killCities();
int CvLuaPlayer::lKillCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::killCities);
}

//------------------------------------------------------------------------------
//string getNewCityName();
int CvLuaPlayer::lGetNewCityName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		CvString cityName = pkPlayer->getNewCityName();
		lua_pushstring(L, cityName.c_str());
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsCityNameValid(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvString strName = lua_tostring(L, 2);
	const bool bTestDestroyed = luaL_optint(L, 3, 0);
	
	const bool bResult = pkPlayer->isCityNameValid(strName, bTestDestroyed);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//CvUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION, bool bHistoric = true);
int CvLuaPlayer::lInitUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const int x = lua_tointeger(L, 3);
	const int y = lua_tointeger(L, 4);
	const UnitAITypes eUnitAI = (UnitAITypes)luaL_optint(L, 5, NO_UNITAI);
	const DirectionTypes eFacingDirection = (DirectionTypes)luaL_optint(L, 6, NO_DIRECTION);
	const bool bHistoric = luaL_optbool(L, 7, true);

	CvUnit* pkUnit = pkPlayer->initUnit(eUnit, x, y, eUnitAI, REASON_LUA, false, true, 0, 0, NO_CONTRACT, bHistoric);
	if (pkUnit)
		pkUnit->setFacingDirection(eFacingDirection);

	CvLuaUnit::Push(L, pkUnit);
	return 1;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CvUnit* initUnitWithNameOffset(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION, bool bHistoric = true);
int CvLuaPlayer::lInitUnitWithNameOffset(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const int iNameOffset = lua_tointeger(L, 3);
	const int x = lua_tointeger(L, 4);
	const int y = lua_tointeger(L, 5);
	const UnitAITypes eUnitAI = (UnitAITypes)luaL_optint(L, 6, NO_UNITAI);
	const DirectionTypes eFacingDirection = (DirectionTypes)luaL_optint(L, 7, NO_DIRECTION);
	const bool bHistoric = luaL_optbool(L, 8, true);

	CvUnit* pkUnit = pkPlayer->initUnitWithNameOffset(eUnit, iNameOffset, x, y, eUnitAI, REASON_LUA, false, true, 0, 0, NO_CONTRACT, bHistoric);
	if (pkUnit)
		pkUnit->setFacingDirection(eFacingDirection);

	CvLuaUnit::Push(L, pkUnit);
	return 1;
}

//CvUnit* CvPlayer::initNamedUnit(UnitTypes eUnit, const char* strKey, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION);
int CvLuaPlayer::lInitNamedUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const char* strKey = lua_tostring(L, 3);
	const int x = lua_tointeger(L, 4);
	const int y = lua_tointeger(L, 5);
	const UnitAITypes eUnitAI = (UnitAITypes)luaL_optint(L, 6, NO_UNITAI);
	const DirectionTypes eFacingDirection = (DirectionTypes)luaL_optint(L, 7, NO_DIRECTION);

	CvUnit* pkUnit = pkPlayer->initNamedUnit(eUnit, strKey, x, y, eUnitAI, REASON_LUA);
	if (pkUnit)
		pkUnit->setFacingDirection(eFacingDirection);

	CvLuaUnit::Push(L, pkUnit);
	return 1;
}

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
// ---------------------------------------------------------------------
//bool CvPlayer::GetResourceMonopolyPlayer(ResourceTypes eResource)
int CvLuaPlayer::lGetResourceMonopolyPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlayer->HasGlobalMonopoly(eResource);
	lua_pushboolean(L, bResult);
	return 1;
}
// -----------------------------------------------------------------------------
// int CvPlayer::GetMonopolyPercent(ResourceTypes eResource)
int CvLuaPlayer::lGetMonopolyPercent(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetMonopolyPercent(eResource);
	lua_pushinteger(L, iResult);
	return 1;
}
// -----------------------------------------------------------------------------
// int CvPlayer::HasGlobalMonopoly(ResourceTypes eResource)
int CvLuaPlayer::lHasGlobalMonopoly(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlayer->HasGlobalMonopoly(eResource);
	lua_pushboolean(L, bResult);
	return 1;
}
// -----------------------------------------------------------------------------
// int CvPlayer::GetMonopolyPercent(ResourceTypes eResource)
int CvLuaPlayer::lHasStrategicMonopoly(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
	const int bResult = pkPlayer->HasStrategicMonopoly(eResource);
	lua_pushboolean(L, bResult);
	return 1;
}
// -----------------------------------------------------------------------------
// int CvPlayer::GetResourcesMisc(ResourceTypes eResource)
int CvLuaPlayer::lGetResourcesMisc(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->getNumResourcesFromOther(eResource);
	lua_pushinteger(L, iResult);
	return 1;
}

// -----------------------------------------------------------------------------
// int CvPlayer::IsShowImports()
int CvLuaPlayer::lIsShowImports(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushboolean(L, (pkPlayer->GetPlayerTraits()->IsImportsCountTowardsMonopolies() || pkPlayer->IsCSResourcesCountMonopolies()));
	return 1;
}
#endif
//------------------------------------------------------------------------------
//void disbandUnit(bool bAnnounce);
int CvLuaPlayer::lDisbandUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::disbandUnit);
}
//------------------------------------------------------------------------------
//CvPlot *addFreeUnit(UnitTypes eUnit, UnitAITypes eUnitAI = NO_UNITAI)
int CvLuaPlayer::lAddFreeUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const UnitAITypes eUnitAI = (UnitAITypes)luaL_optint(L, 3, NO_UNITAI);

	CvPlot* pkPlot = pkPlayer->addFreeUnit(eUnit, eUnitAI);
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}
//------------------------------------------------------------------------------
//void killUnits();
int CvLuaPlayer::lKillUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::killUnits);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetSpecificUnitType(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvString strType = lua_tostring(L, 2);
	
	const UnitTypes eUnitType = pkPlayer->GetSpecificUnitType(strType, true);
	lua_pushinteger(L, eUnitType);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetSpecificBuildingType(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvString strType = lua_tostring(L, 2);
	
	const BuildingTypes eBuildingType = pkPlayer->GetSpecificBuildingType(strType, true);
	lua_pushinteger(L, eBuildingType);
	return 1;
}

//------------------------------------------------------------------------------
//void CvPlayer::chooseTech(int iDiscover, const char* strText, TechTypes iTechJustDiscovered)
int CvLuaPlayer::lChooseTech(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iDiscover = lua_tointeger(L, 2);
	CvString strText = lua_tostring(L, 3);
	TechTypes iTechJustDiscovered = (TechTypes)lua_tointeger(L, 4);

	pkPlayer->chooseTech(iDiscover, strText, iTechJustDiscovered);
	return 1;
}
//------------------------------------------------------------------------------
//bool isHuman();
int CvLuaPlayer::lIsHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isHuman);
}
//------------------------------------------------------------------------------
//bool isBarbarian();
int CvLuaPlayer::lIsBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isBarbarian);
}
//------------------------------------------------------------------------------
//string getName([form]);
int CvLuaPlayer::lGetName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const char* szName = pkPlayer->getName();

	lua_pushstring(L, szName);

	return 1;
}
//------------------------------------------------------------------------------
//wstring getNameKey();
int CvLuaPlayer::lGetNameKey(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getNameKey());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNickName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getNickName());
	return 1;
}

//------------------------------------------------------------------------------
//string getCivilizationDescription();
int CvLuaPlayer::lGetCivilizationDescription(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getCivilizationDescription());
	return 1;
}
//------------------------------------------------------------------------------
//string getCivilizationDescriptionKey();
int CvLuaPlayer::lGetCivilizationDescriptionKey(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getCivilizationDescriptionKey());
	return 1;
}
//------------------------------------------------------------------------------
//string getCivilizationShortDescription();
int CvLuaPlayer::lGetCivilizationShortDescription(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getCivilizationShortDescription());
	return 1;
}
//------------------------------------------------------------------------------
//string getCivilizationShortDescriptionKey();
int CvLuaPlayer::lGetCivilizationShortDescriptionKey(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getCivilizationShortDescriptionKey());
	return 1;
}
//------------------------------------------------------------------------------
//string getCivilizationAdjective(int iForm);
int CvLuaPlayer::lGetCivilizationAdjective(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getCivilizationAdjective());
	return 1;
}
//------------------------------------------------------------------------------
//string getCivilizationAdjectiveKey();
int CvLuaPlayer::lGetCivilizationAdjectiveKey(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getCivilizationAdjectiveKey());
	return 1;
}
//------------------------------------------------------------------------------
//bool isWhiteFlag();
int CvLuaPlayer::lIsWhiteFlag(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isWhiteFlag);
}
//------------------------------------------------------------------------------
//wstring GetStateReligionName();
int CvLuaPlayer::lGetStateReligionName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetStateReligionName());
	return 1;
}
//------------------------------------------------------------------------------
//wstring GetStateReligionKey();
int CvLuaPlayer::lGetStateReligionKey(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetStateReligionKey());
	return 1;
}
//------------------------------------------------------------------------------
//wstring getWorstEnemyName();
int CvLuaPlayer::lGetWorstEnemyName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getWorstEnemyName());
	return 1;
}
//------------------------------------------------------------------------------
//ArtStyleTypes  getArtStyleType();
int CvLuaPlayer::lGetArtStyleType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getArtStyleType);
}
//------------------------------------------------------------------------------
//int countCityFeatures(FeatureTypes  eFeature);
int CvLuaPlayer::lCountCityFeatures(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::countCityFeatures);
}
//------------------------------------------------------------------------------
//int countNumBuildings(BuildingTypes  eBuilding);
int CvLuaPlayer::lCountNumBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::countNumBuildings);
}
//------------------------------------------------------------------------------
//int GetNumWorldWonders();
int CvLuaPlayer::lGetNumWorldWonders(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	int iWonderCount = 0;

	int iBuildingLoop;
	BuildingTypes eBuilding;

	// Loop through all buildings, see if they're a world wonder
	for(iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		eBuilding = (BuildingTypes) iBuildingLoop;
		CvBuildingEntry* pkBuildingEntry = GC.getBuildingInfo(eBuilding);
		if(pkBuildingEntry)
		{
			if(::isWorldWonderClass(pkBuildingEntry->GetBuildingClassInfo()))
			{
				iWonderCount += pkPlayer->countNumBuildings(eBuilding);
			}
		}
	}

	lua_pushinteger(L, iWonderCount);
	return 1;
}
//------------------------------------------------------------------------------
//void ChangeNumWorldWonders(int iChange);
int CvLuaPlayer::lChangeNumWorldWonders(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iChange = lua_tointeger(L, 2);

	pkPlayer->ChangeNumWonders(iChange);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumWondersBeatenTo(int iOtherPlayer);
int CvLuaPlayer::lGetNumWondersBeatenTo(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	int iWondersBeatenTo = pkPlayer->GetDiplomacyAI()->GetNumWondersBeatenTo(eOtherPlayer);

	lua_pushinteger(L, iWondersBeatenTo);
	return 1;
}
//------------------------------------------------------------------------------
//void SetNumWondersBeatenTo(int iOtherPlayer, int iValue);
int CvLuaPlayer::lSetNumWondersBeatenTo(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iValue = lua_tointeger(L, 3);

	if(iValue > 0)
	{
		pkPlayer->GetDiplomacyAI()->SetNumWondersBeatenTo(eOtherPlayer, iValue);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsCapitalConnectedToCity(lua_State* L)
{
	//CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);

	const bool bResult = pkCity->IsConnectedToCapital();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlotConnectedToPlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkFromPlot = CvLuaPlot::GetInstance(L, 2);
	CvPlot* pkToPlot = CvLuaPlot::GetInstance(L, 3);
	bool bAllowHarbors = luaL_optbool(L, 4, false);
	bool bAssumeOpenBorders = luaL_optbool(L, 5, false);

	const bool bResult = IsPlotConnectedToPlot(pkPlayer->GetID(), pkFromPlot, pkToPlot, ROUTE_ANY, bAllowHarbors, bAssumeOpenBorders);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isTurnActive( void );
int CvLuaPlayer::lIsTurnActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isTurnActive);
}

//------------------------------------------------------------------------------
//bool IsSimultaneousTurns( void );
int CvLuaPlayer::lIsSimultaneousTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isSimultaneousTurns);
}

//------------------------------------------------------------------------------
//void findNewCapital();
int CvLuaPlayer::lFindNewCapital(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::findNewCapital);
}
//------------------------------------------------------------------------------
//bool canRaze(CyCity* pCity);
int CvLuaPlayer::lCanRaze(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	bool bIgnoreCapitals = luaL_optbool(L, 3, false);

	const bool bResult = pkPlayer->canRaze(pkCity, bIgnoreCapitals);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void raze(CyCity* pCity);
int CvLuaPlayer::lRaze(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);

	pkPlayer->raze(pkCity);
	return 0;
}
//------------------------------------------------------------------------------
//void disband(CyCity* pCity);
int CvLuaPlayer::lDisband(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);

	pkPlayer->disband(pkCity);
	return 0;
}
//------------------------------------------------------------------------------
//bool canReceiveGoody(CyPlot* pPlot, GoodyTypes  eGoody, CyUnit* pUnit);
int CvLuaPlayer::lCanReceiveGoody(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pPlot = CvLuaPlot::GetInstance(L, 2);
	GoodyTypes eGoody = (GoodyTypes)lua_tointeger(L, 3);
	CvUnit* pUnit = CvLuaUnit::GetInstance(L, 4);

	bool bResult = pkPlayer->canReceiveGoody(pPlot, eGoody, pUnit);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void receiveGoody(CyPlot* pPlot, GoodyTypes  eGoody, CyUnit* pUnit);
int CvLuaPlayer::lReceiveGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::receiveGoody);
}
//------------------------------------------------------------------------------
//void doGoody(CyPlot* pPlot, CyUnit* pUnit);
int CvLuaPlayer::lDoGoody(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::doGoody);
}
//------------------------------------------------------------------------------
// This function checks the handicap as well as CanReceiveGoody to test validity
int CvLuaPlayer::lCanGetGoody(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pPlot = CvLuaPlot::GetInstance(L, 2);
	GoodyTypes eGoody = (GoodyTypes)lua_tointeger(L, 3);
	CvUnit* pUnit = CvLuaUnit::GetInstance(L, 4);

	bool bResult = false;
	// Need to have Goodies in the Handicap file to pick from
	if(pkPlayer->getHandicapInfo().getNumGoodies() > 0)
	{
		for(int iGoodyLoop = 0; iGoodyLoop < pkPlayer->getHandicapInfo().getNumGoodies(); iGoodyLoop++)
		{
			GoodyTypes eThisGoody = (GoodyTypes) pkPlayer->getHandicapInfo().getGoodies(iGoodyLoop);
			if(eGoody == eThisGoody && pkPlayer->canReceiveGoody(pPlot, eThisGoody, pUnit))
			{
				bResult = true;
				break;
			}
		}
	}

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool canFound(int iX, int iY);
int CvLuaPlayer::lCanFound(lua_State* L)
{
	return BasicLuaMethod<bool,int,int>(L, &CvPlayerAI::canFoundCity);
}
//------------------------------------------------------------------------------
//void found(int iX, int iY);
int CvLuaPlayer::lFound(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iX = lua_tointeger(L, 2);
	const int iY = lua_tointeger(L, 3);

	pkPlayer->foundCity(iX, iY);

	return 0;
}

//------------------------------------------------------------------------------
//bool canTrain(UnitTypes  eUnit, bool bContinue, bool bTestVisible);
int CvLuaPlayer::lCanTrain(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 2);
	const bool bContinue = lua_toboolean(L, 3);
	const bool bTestVisible = lua_toboolean(L, 4);
	const bool bIgnoreCost = lua_toboolean(L, 5);
	const bool bIgnoreUniqueUnitStatus = lua_toboolean(L, 6);

	const bool bResult = pkPlayer->canTrainUnit(eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUniqueUnitStatus);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost);
int CvLuaPlayer::lCanConstruct(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iBuilding = lua_tointeger(L, 2);
	const bool bContinue = luaL_optint(L, 3, 0);
	const bool bTestVisible = luaL_optint(L, 4, 0);
	const bool bIgnoreCost = luaL_optint(L, 5, 0);
	const bool bResult = pkPlayer->canConstruct((BuildingTypes)iBuilding, bContinue, bTestVisible, bIgnoreCost);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canCreate(ProjectTypes  eProject, bool bContinue, bool bTestVisible);
int CvLuaPlayer::lCanCreate(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::canCreate);
}
//------------------------------------------------------------------------------
//bool canPrepare(SpecialistTypes  eSpecialist, bool bContinue);
int CvLuaPlayer::lCanPrepare(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::canPrepare);
}
//------------------------------------------------------------------------------
//bool canMaintain(ProcessTypes  eProcess, bool bContinue);
int CvLuaPlayer::lCanMaintain(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::canMaintain);
}
//------------------------------------------------------------------------------
//bool IsCanPurchaseAnyCity(bool bOnlyTestVisible, UnitTypes eUnitType, BuildingTypes eBuildingType, YieldTypes ePurchaseYield);
int CvLuaPlayer::lIsCanPurchaseAnyCity(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const bool bTestPurchaseCost = lua_toboolean(L, 2);
	const bool bTestTrainable = lua_toboolean(L, 3);
	const UnitTypes eUnitType = (UnitTypes) lua_tointeger(L, 4);
	const BuildingTypes eBuildingType = (BuildingTypes) lua_tointeger(L, 5);
	const YieldTypes ePurchaseYield = (YieldTypes) lua_tointeger(L, 6);

	const bool bResult = pkPlayer->IsCanPurchaseAnyCity(bTestPurchaseCost, bTestTrainable, eUnitType, eBuildingType, ePurchaseYield);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool GetFaithPurchaseType();
int CvLuaPlayer::lGetFaithPurchaseType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetFaithPurchaseType);
}
//------------------------------------------------------------------------------
//void SetFaithPurchaseType(FaithPurchaseTypes eType);
int CvLuaPlayer::lSetFaithPurchaseType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetFaithPurchaseType);
}
//------------------------------------------------------------------------------
//bool GetFaithPurchaseIndex();
int CvLuaPlayer::lGetFaithPurchaseIndex(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetFaithPurchaseIndex);
}
//------------------------------------------------------------------------------
//void SetFaithPurchaseIndex(int iIndex);
int CvLuaPlayer::lSetFaithPurchaseIndex(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetFaithPurchaseIndex);
}
//------------------------------------------------------------------------------
//bool isProductionMaxedUnitClass(UnitClassTypes  eUnitClass);
int CvLuaPlayer::lIsProductionMaxedUnitClass(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isProductionMaxedUnitClass);
}
//------------------------------------------------------------------------------
//bool isProductionMaxedBuildingClass(BuildingClassTypes  eBuildingClass, bool bAcquireCity);
int CvLuaPlayer::lIsProductionMaxedBuildingClass(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isProductionMaxedBuildingClass);
}
//------------------------------------------------------------------------------
//bool isProductionMaxedProject(ProjectTypes  eProject);
int CvLuaPlayer::lIsProductionMaxedProject(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isProductionMaxedProject);
}
//------------------------------------------------------------------------------
//int getUnitProductionNeeded(UnitTypes  iIndex);
int CvLuaPlayer::lGetUnitProductionNeeded(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes iIndex = (UnitTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->getProductionNeeded(iIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getBuildingProductionNeeded(BuildingTypes  iIndex);
int CvLuaPlayer::lGetBuildingProductionNeeded(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const BuildingTypes iIndex = (BuildingTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->getProductionNeeded(iIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getProjectProductionNeeded(ProjectTypes  iIndex);
int CvLuaPlayer::lGetProjectProductionNeeded(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ProjectTypes iIndex = (ProjectTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->getProductionNeeded(iIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_PROCESS_STOCKPILE)
//------------------------------------------------------------------------------
//int getMaxStockpile();
int CvLuaPlayer::lGetMaxStockpile(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getMaxStockpile();
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool hasReadyUnit() const;
int CvLuaPlayer::lHasReadyUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->hasReadyUnit();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetFirstReadyUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const CvUnit* MyCvUnit = pkPlayer->GetFirstReadyUnit();

	CvLuaUnit::Push(L, const_cast<CvUnit*>(MyCvUnit));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetFirstReadyUnitPlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pPlot = NULL;
	const CvUnit* pUnit = pkPlayer->GetFirstReadyUnit();
	if(pUnit)
	{
		pPlot = pUnit->plot();
	}

	CvLuaPlot::Push(L, pPlot);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lHasBusyUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushboolean(L, pkPlayer->hasBusyUnit());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lHasBusyMovingUnit(lua_State* L)
{
	lua_pushboolean(L, false);	// Obsolete function.  Units are never busy moving, movement is always instant in the game core.
	return 1;
}

//------------------------------------------------------------------------------
//int getBuildingClassPrereqBuilding(BuildingTypes  eBuilding, BuildingClassTypes  ePrereqBuildingClass, int iExtra);
int CvLuaPlayer::lGetBuildingClassPrereqBuilding(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getBuildingClassPrereqBuilding);
}

//------------------------------------------------------------------------------
//void removeBuildingClass(BuildingClassTypes  eBuildingClass);
int CvLuaPlayer::lRemoveBuildingClass(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::removeBuildingClass);
}

//------------------------------------------------------------------------------
//bool canBuild(CyPlot* pPlot, BuildTypes  eBuild, bool bTestEra = false, bool bTestVisible = false, bool bTestGold = false);
int CvLuaPlayer::lCanBuild(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const BuildTypes eBuild = (BuildTypes)lua_tointeger(L, 3);
	const bool bTestEra = luaL_optint(L, 4, 0);
	const bool bTestVisible = luaL_optint(L, 5, 0);
	const bool bTestGold = luaL_optint(L, 6, 0);

	const bool bResult = pkPlayer->canBuild(pkPlot, eBuild, bTestEra, bTestVisible, bTestGold);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool IsBuildBlockedByFeature(BuildTypes  eBuild, FeatureTypes eFeature);
int CvLuaPlayer::lIsBuildBlockedByFeature(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const BuildTypes eBuild = (BuildTypes)lua_tointeger(L, 2);
	const FeatureTypes eFeature = (FeatureTypes)lua_tointeger(L, 3);

	const bool bResult = pkPlayer->IsBuildBlockedByFeature(eBuild, eFeature);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//RouteTypes  getBestRoute(CyPlot* pPlot) const;
int CvLuaPlayer::lGetBestRoute(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	const RouteTypes eBestRoute = pkPlayer->getBestRoute(pkPlot);
	lua_pushinteger(L, eBestRoute);
	return 1;
}
//------------------------------------------------------------------------------
//int getImprovementUpgradeRate() const;
int CvLuaPlayer::lGetImprovementUpgradeRate(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getImprovementUpgradeRate);
}
//------------------------------------------------------------------------------
//int calculateTotalYield(YieldTypes  eYield);
int CvLuaPlayer::lCalculateTotalYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::calculateTotalYield);
}

//------------------------------------------------------------------------------
//int calculateUnitCost();
int CvLuaPlayer::lCalculateUnitCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::calculateUnitCost);
}
//------------------------------------------------------------------------------
//int calculateUnitSupply();
int CvLuaPlayer::lCalculateUnitSupply(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->calculateUnitSupply();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetNumMaintenanceFreeUnits();
int CvLuaPlayer::lGetNumMaintenanceFreeUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumMaintenanceFreeUnits);
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//int GetBaseBuildingMaintenance();
int CvLuaPlayer::lGetBaseBuildingMaintenance(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetTreasury()->GetBaseBuildingGoldMaintenance();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetBaseUnitMaintenance();
int CvLuaPlayer::lGetBaseUnitMaintenance(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	// If player has 0 Cities then no Unit cost
	if(pkPlayer->getNumCities() == 0)
	{
		return 0;
	}

	const CvHandicapInfo& playerHandicap = pkPlayer->getHandicapInfo();
	int iFreeUnits = playerHandicap.getGoldFreeUnits();

	// Defined in XML by unit info type
	iFreeUnits += pkPlayer->GetNumMaintenanceFreeUnits();
	iFreeUnits += pkPlayer->getBaseFreeUnits();

	int iPaidUnits = max(0, pkPlayer->getNumUnits() - iFreeUnits);

	int iBaseUnitCost = iPaidUnits * pkPlayer->getGoldPerUnitTimes100();

	int iResult = (iBaseUnitCost / 100);
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int GetBuildingGoldMaintenance();
int CvLuaPlayer::lGetBuildingGoldMaintenance(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetTreasury()->GetBuildingGoldMaintenance();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int SetBaseBuildingGoldMaintenance();
int CvLuaPlayer::lSetBaseBuildingGoldMaintenance(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->GetTreasury()->SetBaseBuildingGoldMaintenance(iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int ChangeBaseBuildingGoldMaintenance();
int CvLuaPlayer::lChangeBaseBuildingGoldMaintenance(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->GetTreasury()->ChangeBaseBuildingGoldMaintenance(iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetImprovementGoldMaintenance();
int CvLuaPlayer::lGetImprovementGoldMaintenance(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetTreasury()->GetImprovementGoldMaintenance();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int calculateGoldRate();
int CvLuaPlayer::lCalculateGoldRate(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::calculateGoldRate);
}
//------------------------------------------------------------------------------
//int CalculateGoldRateTimes100();
int CvLuaPlayer::lCalculateGoldRateTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::calculateGoldRateTimes100);
}
//------------------------------------------------------------------------------
//int CalculateGrossGoldTimes100();
int CvLuaPlayer::lCalculateGrossGoldTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetTreasury()->CalculateGrossGoldTimes100();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int CalculateInflatedCosts();
int CvLuaPlayer::lCalculateInflatedCosts(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetTreasury()->CalculateInflatedCosts();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int calculateResearchModifier(TechTypes  eTech);
int CvLuaPlayer::lCalculateResearchModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::calculateResearchModifier);
}
//------------------------------------------------------------------------------
//bool isResearch();
int CvLuaPlayer::lIsResearch(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const bool bResult = pkPlayer->GetPlayerTechs()->IsResearch();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canEverResearch(TechTypes  eTech);
int CvLuaPlayer::lCanEverResearch(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes eTech = (TechTypes)lua_tointeger(L, 2);

	const bool bResult
	    = pkPlayer->GetPlayerTechs()->CanEverResearch(eTech);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanResearch(TechTypes eTech, bool bTrade = false);
int CvLuaPlayer::lCanResearch(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes eTech = (TechTypes)luaL_checkinteger(L, 2);
	const bool bTrade = luaL_optint(L, 3, 0);

	const bool bResult
	    = pkPlayer->GetPlayerTechs()->CanResearch(eTech, bTrade);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanResearchForFree(TechTypes eTech);
int CvLuaPlayer::lCanResearchForFree(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes eTech = (TechTypes)luaL_checkinteger(L, 2);

	const bool bResult = pkPlayer->GetPlayerTechs()->CanResearchForFree(eTech);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//TechTypes getCurrentResearch();
int CvLuaPlayer::lGetCurrentResearch(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes iResult = pkPlayer->GetPlayerTechs()->GetCurrentResearch();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isCurrentResearchRepeat();
int CvLuaPlayer::lIsCurrentResearchRepeat(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const bool bResult
	    = pkPlayer->GetPlayerTechs()->IsCurrentResearchRepeat();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isNoResearchAvailable();
int CvLuaPlayer::lIsNoResearchAvailable(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const bool bResult
	    = pkPlayer->GetPlayerTechs()->IsNoResearchAvailable();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getResearchTurnsLeft(TechTypes  eTech, bool bOverflow);
int CvLuaPlayer::lGetResearchTurnsLeft(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes eTech	= (TechTypes)lua_tointeger(L, 2);
	const bool bOverflow	= lua_toboolean(L, 3);

	const int iResult
	    = pkPlayer->GetPlayerTechs()->GetResearchTurnsLeft(eTech, bOverflow);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetResearchCost(TechTypes  eTech);
int CvLuaPlayer::lGetResearchCost(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes eTech	= (TechTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetPlayerTechs()->GetResearchCost(eTech);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetResearchProgress(TechTypes  eTech);
int CvLuaPlayer::lGetResearchProgress(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes eTech	= (TechTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetPlayerTechs()->GetResearchProgress(eTech);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int unitsRequiredForGoldenAge();
int CvLuaPlayer::lUnitsRequiredForGoldenAge(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::unitsRequiredForGoldenAge);
}
//------------------------------------------------------------------------------
//int unitsGoldenAgeCapable();
int CvLuaPlayer::lUnitsGoldenAgeCapable(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::unitsGoldenAgeCapable);
}
//------------------------------------------------------------------------------
//int unitsGoldenAgeReady();
int CvLuaPlayer::lUnitsGoldenAgeReady(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::unitsGoldenAgeReady);
}
//------------------------------------------------------------------------------
//int GreatGeneralThreshold(bool bMilitary);
int CvLuaPlayer::lGreatGeneralThreshold(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::greatGeneralThreshold);
}
//------------------------------------------------------------------------------
//int GreatAdmiralThreshold(bool bMilitary);
int CvLuaPlayer::lGreatAdmiralThreshold(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::greatAdmiralThreshold);
}
//------------------------------------------------------------------------------
//int specialistYield(SpecialistTypes  eSpecialist, YieldTypes  eYield);
int CvLuaPlayer::lSpecialistYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::specialistYield);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lSetGreatGeneralCombatBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetGreatGeneralCombatBonus(iValue);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGreatGeneralCombatBonus(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetGreatGeneralCombatBonus);
}

//------------------------------------------------------------------------------
//CvPlot* getStartingPlot()
int CvLuaPlayer::lGetStartingPlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvPlot* pkPlot = pkPlayer->getStartingPlot();
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}
//------------------------------------------------------------------------------
//void setStartingPlot(CyPlot* pPlot);
int CvLuaPlayer::lSetStartingPlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	pkPlayer->setStartingPlot(pkPlot);
	return 0;
}
//------------------------------------------------------------------------------
//int getTotalPopulation();
int CvLuaPlayer::lGetTotalPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getTotalPopulation);
}
//------------------------------------------------------------------------------
//int getAveragePopulation();
int CvLuaPlayer::lGetAveragePopulation100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, (int)(pkPlayer->getAveragePopulation()*100));
	return 1;
}
//------------------------------------------------------------------------------
//long getRealPopulation();
int CvLuaPlayer::lGetRealPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getRealPopulation);
}
//------------------------------------------------------------------------------
//int GetNewCityExtraPopulation() const;
int CvLuaPlayer::lGetNewCityExtraPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNewCityExtraPopulation);
}
//------------------------------------------------------------------------------
//void ChangeNewCityExtraPopulation(int iChange);
int CvLuaPlayer::lChangeNewCityExtraPopulation(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iValue = lua_tointeger(L, 2);
	pkPlayer->ChangeNewCityExtraPopulation(iValue);

	return 1;
}
//------------------------------------------------------------------------------
//int getTotalLand();
int CvLuaPlayer::lGetTotalLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getTotalLand);
}
//------------------------------------------------------------------------------
//int getTotalLandScored();
int CvLuaPlayer::lGetTotalLandScored(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getTotalLandScored);
}
//------------------------------------------------------------------------------
//int getGold();
int CvLuaPlayer::lGetGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGold());
	return 1;
}
//------------------------------------------------------------------------------
//void setGold(int iNewValue);
int CvLuaPlayer::lSetGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iValue = lua_tointeger(L, 2);
	pkPlayer->GetTreasury()->SetGold(iValue);

	return 1;
}
//------------------------------------------------------------------------------
//void changeGold(int iChange);
int CvLuaPlayer::lChangeGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iValue = lua_tointeger(L, 2);
	pkPlayer->GetTreasury()->ChangeGold(iValue);

	return 1;
}
//------------------------------------------------------------------------------
//int CalculateGrossGold();
int CvLuaPlayer::lCalculateGrossGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->CalculateGrossGold());
	return 1;
}
//------------------------------------------------------------------------------
//int GetLifetimeGrossGold();
int CvLuaPlayer::lGetLifetimeGrossGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetLifetimeGrossGold());
	return 1;
}
//------------------------------------------------------------------------------
//int GetGoldFromCitiesTimes100();
int CvLuaPlayer::lGetGoldFromCitiesTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGoldFromCitiesTimes100());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldFromCitiesMinusTradeRoutesTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iResult = pkPlayer->GetTreasury()->GetGoldFromCitiesTimes100(true);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetGoldPerTurnFromDiplomacy();
int CvLuaPlayer::lGetGoldPerTurnFromDiplomacy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGoldPerTurnFromDiplomacy());
	return 1;
}
//------------------------------------------------------------------------------
//int GetCityConnectionRouteGoldTimes100(CvCity* pCity);
int CvLuaPlayer::lGetCityConnectionRouteGoldTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetCityConnectionRouteGoldTimes100(pkCity));
	return 1;
}
//------------------------------------------------------------------------------
//int GetCityConnectionGold();
int CvLuaPlayer::lGetCityConnectionGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetCityConnectionGold());
	return 1;
}
//------------------------------------------------------------------------------
//int GetCityConnectionGoldTimes100();
int CvLuaPlayer::lGetCityConnectionGoldTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetCityConnectionGoldTimes100());
	return 1;
}
//------------------------------------------------------------------------------
//int GetGoldPerTurnFromReligion();
int CvLuaPlayer::lGetGoldPerTurnFromReligion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGoldPerTurnFromReligion());
	return 1;
}
//------------------------------------------------------------------------------
//int GetGoldPerTurnFromTradeRoutes();
int CvLuaPlayer::lGetGoldPerTurnFromTradeRoutes(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGoldPerTurnFromTradeRoutes());
	return 1;
}
//------------------------------------------------------------------------------
//int GetGoldPerTurnFromTradeRoutes();
int CvLuaPlayer::lGetGoldPerTurnFromTradeRoutesTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGoldPerTurnFromTradeRoutesTimes100());
	return 1;
}
//------------------------------------------------------------------------------
//int GetGoldPerTurnFromTraits();
int CvLuaPlayer::lGetGoldPerTurnFromTraits(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGoldPerTurnFromTraits());
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//int GetInternalTradeRouteGoldBonus();
int CvLuaPlayer::lGetInternalTradeRouteGoldBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetInternalTradeRouteGoldBonus());
	return 1;
}
//------------------------------------------------------------------------------
//int GetGAPFromReligion();
int CvLuaPlayer::lGetGAPFromReligion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iGAP = pkPlayer->GetYieldPerTurnFromReligion(YIELD_GOLDEN_AGE_POINTS);
	lua_pushinteger(L, iGAP);
	return 1;
}
//------------------------------------------------------------------------------
//int GetGAPFromCities();
int CvLuaPlayer::lGetGAPFromCities(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	const int iGAP = pkPlayer->GetGoldenAgePointsFromCities();
	lua_pushinteger(L, iGAP);
	return 1;
}
//------------------------------------------------------------------------------
//int GetGAPFromTraits();
int CvLuaPlayer::lGetGAPFromTraits(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iGAP = pkPlayer->GetYieldPerTurnFromTraits(YIELD_GOLDEN_AGE_POINTS);
	lua_pushinteger(L, iGAP);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int GetTotalJONSCulturePerTurn();
int CvLuaPlayer::lGetTotalJONSCulturePerTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetTotalJONSCulturePerTurn);
}
//------------------------------------------------------------------------------
//int getJONSCulturePerTurnFromCities();
int CvLuaPlayer::lGetJONSCulturePerTurnFromCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetJONSCulturePerTurnFromCities);
}
//------------------------------------------------------------------------------
//int getJONSCulturePerTurnFromExcessHappiness();
int CvLuaPlayer::lGetJONSCulturePerTurnFromExcessHappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetJONSCulturePerTurnFromExcessHappiness);
}
//------------------------------------------------------------------------------
//int getJONSCulturePerTurnFromTraits();
int CvLuaPlayer::lGetJONSCulturePerTurnFromTraits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetJONSCulturePerTurnFromTraits);
}
//------------------------------------------------------------------------------
//int GetCultureWonderMultiplier();
int CvLuaPlayer::lGetCultureWonderMultiplier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCultureWonderMultiplier);
}
//------------------------------------------------------------------------------
//int getJONSCulturePerTurnForFree();
int CvLuaPlayer::lGetJONSCulturePerTurnForFree(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetJONSCulturePerTurnForFree);
}
//------------------------------------------------------------------------------
//void changeJONSCulturePerTurnForFree(int iChange);
int CvLuaPlayer::lChangeJONSCulturePerTurnForFree(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeJONSCulturePerTurnForFree);
}
//------------------------------------------------------------------------------
//int GetCulturePerTurnFromMinorCivs();
int CvLuaPlayer::lGetCulturePerTurnFromMinorCivs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCulturePerTurnFromMinorCivs);
}
//------------------------------------------------------------------------------
//int GetCulturePerTurnFromMinor(int iMinor);
int CvLuaPlayer::lGetCulturePerTurnFromMinor(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCulturePerTurnFromMinor);
}
//------------------------------------------------------------------------------
//int GetCulturePerTurnFromReligion();
int CvLuaPlayer::lGetCulturePerTurnFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCulturePerTurnFromReligion);
}
//------------------------------------------------------------------------------
//int GetCulturePerTurnFromBonusTurns();
int CvLuaPlayer::lGetCulturePerTurnFromBonusTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCulturePerTurnFromBonusTurns);
}
//------------------------------------------------------------------------------
//int GetCultureCityModifier();
int CvLuaPlayer::lGetCultureCityModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetJONSCultureCityModifier);
}

//------------------------------------------------------------------------------
//int getJONSCulture();
int CvLuaPlayer::lGetJONSCulture(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getJONSCulture);
}
//------------------------------------------------------------------------------
//void setJONSCulture(int iNewValue);
int CvLuaPlayer::lSetJONSCulture(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setJONSCulture);
}
//------------------------------------------------------------------------------
//void changeJONSCulture(int iChange);
int CvLuaPlayer::lChangeJONSCulture(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeJONSCulture);
}
//------------------------------------------------------------------------------
//int GetJONSCultureEverGenerated();
int CvLuaPlayer::lGetJONSCultureEverGenerated(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetJONSCultureEverGenerated);
}
//------------------------------------------------------------------------------
//int GetLastTurnLifetimeCulture();
int CvLuaPlayer::lGetLastTurnLifetimeCulture(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetCulture()->GetLastTurnLifetimeCulture();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetInfluenceOn();
int CvLuaPlayer::lGetInfluenceOn(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetCulture()->GetInfluenceOn(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetLastTurnInfluenceOn();
int CvLuaPlayer::lGetLastTurnInfluenceOn(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetCulture()->GetLastTurnInfluenceOn(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetInfluencePerTurn();
int CvLuaPlayer::lGetInfluencePerTurn(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetCulture()->GetTourismPerTurnIncludingInstant(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetInfluenceLevel();
int CvLuaPlayer::lGetInfluenceLevel(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	InfluenceLevelTypes eResult = pkPlayer->GetCulture()->GetInfluenceLevel(ePlayer);
	lua_pushinteger(L, (int)eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetInfluenceTrend();
int CvLuaPlayer::lGetInfluenceTrend(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	InfluenceLevelTrend eResult = pkPlayer->GetCulture()->GetInfluenceTrend(ePlayer);
	lua_pushinteger(L, (int)eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTurnsToInfluential();
int CvLuaPlayer::lGetTurnsToInfluential(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetCulture()->GetTurnsToInfluential(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumCivsInfluentialOn();
int CvLuaPlayer::lGetNumCivsInfluentialOn(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetCulture()->GetNumCivsInfluentialOn();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumCivsToBeInfluentialOn();
int CvLuaPlayer::lGetNumCivsToBeInfluentialOn(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetCulture()->GetNumCivsToBeInfluentialOn();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInfluenceTradeRouteGoldBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 2);
	
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	int iResult = 0;
	PlayerTypes ePlayer = pkPlayer->GetID();
	if (ePlayer != eOtherPlayer)
	{
		iResult = pkPlayer->GetCulture()->GetInfluenceTradeRouteGoldBonus(eOtherPlayer);
	}
#else
	const int iResult = pkPlayer->GetCulture()->GetInfluenceTradeRouteGoldBonus(eOtherPlayer);
#endif
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetWoundedUnitDamageMod(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	const int iResult = pkPlayer->GetWoundedUnitDamageMod();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetCapitalCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	
	pkPlayer->setCapitalCity(pkCity);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetOriginalCapitalXY(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	
	pkPlayer->setOriginalCapitalXY(pkCity);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumWonders(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	const int iResult = pkPlayer->GetNumWonders();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetOriginalCapitalPlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvPlot *pOriginalCapitalPlot = GC.getMap().plot(pkPlayer->GetOriginalCapitalX(), pkPlayer->GetOriginalCapitalY());
	CvLuaPlot::Push(L, pOriginalCapitalPlot);
	return 1;
}
#if defined(MOD_BALANCE_CORE_POLICIES)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNoUnhappinessExpansion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	const bool bResult = pkPlayer->GetNoUnhappinessExpansion();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetFractionOriginalCapitalsUnderControl(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iOCCount = luaL_optint(L, 2, 0);

	if (pkPlayer == NULL)
		return 0;

	int iTotal = 0;

	if (iOCCount > 0)
	{
		int iCivCount = 0;
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_PLAYERS; iLoopPlayer++)
		{
			CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
			if (kPlayer.isEverAlive() && kPlayer.isMajorCiv() && !kPlayer.isObserver())
				iCivCount++;
		}

		iTotal = iOCCount * 100 / iCivCount;
	}

	lua_pushinteger(L, iTotal);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTechDeviation(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	if (pkPlayer == NULL)
		return 0;

	// Mod for City Count
	int iMod = pkPlayer->GetTechDeviation();	// Default is 15, gets smaller on larger maps

	lua_pushinteger(L, iMod);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTourismPenalty(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	if (pkPlayer == NULL)
		return 0;

	// Mod for City Count
	int iMod = GC.getMap().getWorldInfo().GetNumCitiesTourismCostMod();	// Default is 15, gets smaller on larger maps

	lua_pushinteger(L, iMod);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTechsToFreePolicy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	if (pkPlayer == NULL)
		return 0;

	if (pkPlayer->GetPlayerTraits()->GetFreePolicyPerXTechs() <= 0)
	{
		lua_pushinteger(L, -1);
		return 1;
	}

	int iTechMultiplier = (GET_TEAM(pkPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown() / pkPlayer->GetPlayerTraits()->GetFreePolicyPerXTechs()) + 1;

	int iTechsNeeded =  pkPlayer->GetPlayerTraits()->GetFreePolicyPerXTechs() * iTechMultiplier;
	int iTechsWeHave = GET_TEAM(pkPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown();

	iTechsNeeded -= iTechsWeHave;

	lua_pushinteger(L, iTechsNeeded);
	return 1;
}

#endif
//------------------------------------------------------------------------------
//int GetInfluenceTradeRouteScienceBonus();
int CvLuaPlayer::lGetInfluenceTradeRouteScienceBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 2);
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	int iResult = 0;
	PlayerTypes ePlayer = pkPlayer->GetID();
	if (ePlayer != eOtherPlayer)
	{
		iResult = pkPlayer->GetCulture()->GetInfluenceTradeRouteScienceBonus(eOtherPlayer);
	}
#else
	const int iResult = pkPlayer->GetCulture()->GetInfluenceTradeRouteScienceBonus(eOtherPlayer);
#endif
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetInfluenceCityStateSpyRankBonus();
int CvLuaPlayer::lGetInfluenceCityStateSpyRankBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eCityStatePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetCulture()->GetInfluenceCityStateSpyRankBonus(eCityStatePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetInfluenceMajorCivSpyRankBonus();
int CvLuaPlayer::lGetInfluenceMajorCivSpyRankBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eOtherPlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetInfluenceSpyRankTooltip();
int CvLuaPlayer::lGetInfluenceSpyRankTooltip(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvString szSpyName = lua_tostring(L, 2);
	CvString szRank = lua_tostring(L, 3);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 4);
	bool bNoBasicHelp = luaL_optbool(L, 5, false);
	int iSpyID = luaL_optint(L, 6, -1);
	const CvString szResult = pkPlayer->GetCulture()->GetInfluenceSpyRankTooltip(szSpyName, szRank, eOtherPlayer, bNoBasicHelp, iSpyID);
	lua_pushstring(L, szResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTourism();
int CvLuaPlayer::lGetTourism(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iResult = pkPlayer->GetCulture()->GetTourism();
	if (!MOD_BALANCE_CORE_TOURISM_HUNDREDS)
		iResult /= 100;

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTourismModifierWith();
int CvLuaPlayer::lGetTourismModifierWith(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetCulture()->GetTourismModifierWith(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetTourismModifierWithTooltip();
int CvLuaPlayer::lGetTourismModifierWithTooltip(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushstring(L, pkPlayer->GetCulture()->GetTourismModifierWithTooltip(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
//PublicOpinionTypes GetPublicOpinionType();
int CvLuaPlayer::lGetPublicOpinionType(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetCulture()->GetPublicOpinionType());
	return 1;
}
//------------------------------------------------------------------------------
//PolicyBranchTypes GetPublicOpinionPreferredIdeology();
int CvLuaPlayer::lGetPublicOpinionPreferredIdeology(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetCulture()->GetPublicOpinionPreferredIdeology());
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetPublicOpinionTooltip();
int CvLuaPlayer::lGetPublicOpinionTooltip(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetCulture()->GetPublicOpinionTooltip());
	return 1;
}
//------------------------------------------------------------------------------
//int GetPublicOpinionUnhappiness();
int CvLuaPlayer::lGetPublicOpinionUnhappiness(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetCulture()->GetPublicOpinionUnhappiness());
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetPublicOpinionUnhappinessTooltip();
int CvLuaPlayer::lGetPublicOpinionUnhappinessTooltip(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetCulture()->GetPublicOpinionUnhappinessTooltip());
	return 1;
}
//------------------------------------------------------------------------------
//int ChangeInfluenceOnAllPlayers(iBaseInfluence, bApplyModifiers=true, bModifyForGameSpeed=false);
int CvLuaPlayer::lChangeInfluenceOnAllPlayers(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iBaseInfluence = lua_tointeger(L, 2);
	bool bApplyModifiers = luaL_optbool(L, 3, true);
	bool bModifyForGameSpeed = luaL_optbool(L, 4, false);

	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes eOtherPlayer = (PlayerTypes)uiPlayer;
		if (eOtherPlayer != eActivePlayer) {
			pkPlayer->GetCulture()->ChangeInfluenceOn(eOtherPlayer, iBaseInfluence, bApplyModifiers, bModifyForGameSpeed);
		}
	}
	
	return 0;
}
//------------------------------------------------------------------------------
//int ChangeInfluenceOnPlayer(iOtherPlayer, iBaseInfluence, bApplyModifiers=true, bModifyForGameSpeed=false);
int CvLuaPlayer::lChangeInfluenceOnPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 2);
	int iBaseInfluence = lua_tointeger(L, 3);
	bool bApplyModifiers = luaL_optbool(L, 4, true);
	bool bModifyForGameSpeed = luaL_optbool(L, 5, false);
	
	pkPlayer->GetCulture()->ChangeInfluenceOn(eOtherPlayer, iBaseInfluence, bApplyModifiers, bModifyForGameSpeed);

	return 0;
}
//------------------------------------------------------------------------------
//void DoSwapGreatWorks(eFocusYield);
int CvLuaPlayer::lDoSwapGreatWorks(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	YieldTypes eFocusYield = static_cast<YieldTypes>(lua_tointeger(L, 2));
	pkPlayer->GetCulture()->DoSwapGreatWorks(eFocusYield);
#else
	pkPlayer->GetCulture()->DoSwapGreatWorks();
#endif
	return 0;
}
//------------------------------------------------------------------------------
//void DoSwapGreatWorksHuman();
int CvLuaPlayer::lDoSwapGreatWorksHuman(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	bool bSwap = lua_toboolean(L, 2);
	pkPlayer->GetCulture()->DoSwapGreatWorksHuman(bSwap);
	return 0;
}
//------------------------------------------------------------------------------
//bool HasAvailableGreatWorkSlot(eGreatWorkSlot);
int CvLuaPlayer::lHasAvailableGreatWorkSlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	GreatWorkSlotType eGreatWorkSlot = static_cast<GreatWorkSlotType>(lua_tointeger(L, 2));
	const bool bResult = pkPlayer->GetCulture()->HasAvailableGreatWorkSlot(eGreatWorkSlot);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//CvCity *GetCityOfClosestGreatWorkSlot(int iX, int iY, GreatWorkSlotType eGreatWorkSlot);
int CvLuaPlayer::lGetCityOfClosestGreatWorkSlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iX = lua_tointeger(L, 2);
	int iY = lua_tointeger(L, 3);
	GreatWorkSlotType eGreatWorkSlot = static_cast<GreatWorkSlotType>(lua_tointeger(L, 4));
	BuildingClassTypes eBuildingClass;
	int iSlot;
	CvCity* pkCity = pkPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(iX, iY, eGreatWorkSlot, &eBuildingClass, &iSlot);
	if (pkCity)
	{
		CvLuaCity::Push(L, pkCity);
		return 1;
	}
	else
	{
		return 0;
	}
}
//------------------------------------------------------------------------------
//BuildingType GetBuildingOfClosestGreatWorkSlot(int iX, int iY, GreatWorkSlotType eGreatWorkSlot);
int CvLuaPlayer::lGetBuildingOfClosestGreatWorkSlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iX = lua_tointeger(L, 2);
	int iY = lua_tointeger(L, 3);
	GreatWorkSlotType eGreatWorkSlot = static_cast<GreatWorkSlotType>(lua_tointeger(L, 4));
	BuildingClassTypes eBuildingClass;
	int iSlot;
	CvCity* pkCity = pkPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(iX, iY, eGreatWorkSlot, &eBuildingClass, &iSlot);
	CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(pkPlayer->getCivilizationType());
	if (pkCity && pkCivInfo)
	{
		int iBuilding = -1;
		if (MOD_BUILDINGS_THOROUGH_PREREQUISITES || pkPlayer->GetPlayerTraits()->IsKeepConqueredBuildings())
		{
			iBuilding = (int)pkCity->GetCityBuildings()->GetBuildingTypeFromClass(eBuildingClass);
		}
		else
		{
			iBuilding = pkCivInfo->getCivilizationBuildings(eBuildingClass);
		}
		lua_pushinteger(L, iBuilding);
		return 1;
	}
	else
	{
		return 0;
	}
}
//------------------------------------------------------------------------------
//CvPlot *GetNextDigCompletePlot();
int CvLuaPlayer::lGetNextDigCompletePlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = pkPlayer->GetCulture()->GetNextDigCompletePlot();
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetWrittenArtifactCulture(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iCulture = pkPlayer->GetCulture()->GetWrittenArtifactCulture();
	lua_pushinteger(L, iCulture);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumGreatWorks(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iNumWorks = pkPlayer->GetCulture()->GetNumGreatWorks();
	lua_pushinteger(L, iNumWorks);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumGreatWorkSlots(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iNumWorks = pkPlayer->GetCulture()->GetNumGreatWorkSlots();
	lua_pushinteger(L, iNumWorks);
	return 1;
}
//--------------------------------------------------------------------------------
//int GetFaith();
int CvLuaPlayer::lGetFaith(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetFaith);
}
//------------------------------------------------------------------------------
//void SetFaith(int iNewValue);
int CvLuaPlayer::lSetFaith(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetFaith);
}
//------------------------------------------------------------------------------
//void ChangeFaith(int iNewValue);
int CvLuaPlayer::lChangeFaith(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeFaith);
}
//------------------------------------------------------------------------------
//int GetTotalFaithPerTurn();
int CvLuaPlayer::lGetTotalFaithPerTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetTotalFaithPerTurn);
}
//------------------------------------------------------------------------------
//int GetFaithPerTurnFromCities();
int CvLuaPlayer::lGetFaithPerTurnFromCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetFaithPerTurnFromCities);
}
//------------------------------------------------------------------------------
//int GetFaithPerTurnFromMinorCivs();
int CvLuaPlayer::lGetFaithPerTurnFromMinorCivs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetFaithPerTurnFromMinorCivs);
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//int GetGoldPerTurnFromMinorCivs();
int CvLuaPlayer::lGetGoldPerTurnFromMinorCivs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetGoldPerTurnFromMinorCivs);
}
//------------------------------------------------------------------------------
//int GetSciencePerTurnFromMinorCivs();
int CvLuaPlayer::lGetSciencePerTurnFromMinorCivs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetSciencePerTurnFromMinorCivs);
}
#endif
//------------------------------------------------------------------------------
//int GetFaithPerTurnFromReligion();
int CvLuaPlayer::lGetFaithPerTurnFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetFaithPerTurnFromReligion);
}
//------------------------------------------------------------------------------
//bool HasCreatedPantheon();
int CvLuaPlayer::lHasCreatedPantheon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const bool bResult = pkPlayer->GetReligions()->HasCreatedPantheon();
	lua_pushboolean(L, bResult);

	return 1;
}
//------------------------------------------------------------------------------
//bool GetBeliefInPantheon();
int CvLuaPlayer::lGetBeliefInPantheon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const BeliefTypes eBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(pkPlayer->GetID());
	lua_pushinteger(L, eBelief);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetBeliefsInPantheon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	// If this player has created a (local) religion, we need to use that instead!
	ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(pkPlayer->GetID());
	if (eReligion == NO_RELIGION) eReligion = RELIGION_PANTHEON;

	CvReligionBeliefs beliefs = GC.getGame().GetGameReligions()->GetReligion(eReligion, pkPlayer->GetID())->m_Beliefs;
	for(int iI = 0; iI < beliefs.GetNumBeliefs(); iI++)
	{
		const BeliefTypes eBelief = beliefs.GetBelief(iI);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
//bool CanCreatePantheon();
int CvLuaPlayer::lCanCreatePantheon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	bool bCheckFaith = lua_toboolean(L, 2);

	const bool bResult = GC.getGame().GetGameReligions()->CanCreatePantheon(pkPlayer->GetID(), bCheckFaith) == CvGameReligions::FOUNDING_OK;
	lua_pushboolean(L, bResult);

	return 1;
}
//------------------------------------------------------------------------------
//bool HasCreatedReligion();
int CvLuaPlayer::lHasCreatedReligion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	const bool bIgnoreLocal = luaL_optbool(L, 2, false);
	const bool bResult = pkPlayer->GetReligions()->HasCreatedReligion(bIgnoreLocal);
#else
	const bool bResult = pkPlayer->GetReligions()->HasCreatedReligion();
#endif
	lua_pushboolean(L, bResult);

	return 1;
}
//------------------------------------------------------------------------------
//bool GetReligionCreatedByPlayer();
int CvLuaPlayer::lGetReligionCreatedByPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const ReligionTypes eReligion = pkPlayer->GetReligions()->GetReligionCreatedByPlayer();
	lua_pushinteger(L, eReligion);

	return 1;
}

//------------------------------------------------------------------------------
//bool GetReligionCreatedByPlayer();
int CvLuaPlayer::lGetOriginalReligionCreatedByPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const ReligionTypes eReligion = pkPlayer->GetReligions()->GetOriginalReligionCreatedByPlayer();
	lua_pushinteger(L, eReligion);

	return 1;
}
//------------------------------------------------------------------------------
//bool GetFoundedReligionEnemyCityCombatMod();
int CvLuaPlayer::lGetFoundedReligionEnemyCityCombatMod(lua_State* L)
{
	int iRtnValue = 0;

	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	if(pkPlot)
	{
		CvCity* pPlotCity = pkPlot->getOwningCity();
		if(pPlotCity)
		{
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
			ReligionTypes eFoundedReligion = GET_PLAYER(pkPlayer->GetID()).GetReligions()->GetReligionCreatedByPlayer();
			if(eFoundedReligion != NO_RELIGION && eReligion == eFoundedReligion)
			{
				const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, pkPlayer->GetID());
				if(pReligion)
				{
					iRtnValue = pReligion->m_Beliefs.GetCombatModifierEnemyCities(pkPlayer->GetID(), pPlotCity);
				}
			}
		}

	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}
//------------------------------------------------------------------------------
//bool GetFoundedReligionFriendlyCityCombatMod();
int CvLuaPlayer::lGetFoundedReligionFriendlyCityCombatMod(lua_State* L)
{
	int iRtnValue = 0;

	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	if(pkPlot)
	{
		CvCity* pPlotCity = pkPlot->getOwningCity();
		if(pPlotCity)
		{
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
			ReligionTypes eFoundedReligion = GET_PLAYER(pkPlayer->GetID()).GetReligions()->GetReligionCreatedByPlayer();
			if(eFoundedReligion != NO_RELIGION && eReligion == eFoundedReligion)
			{
				const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, pkPlayer->GetID());
				if(pReligion)
				{
					iRtnValue = pReligion->m_Beliefs.GetCombatModifierFriendlyCities(pkPlayer->GetID(), pPlotCity);
				}
			}
		}

	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}
#if defined(MOD_BALANCE_CORE_BELIEFS)
// int GetYieldPerTurnFromReligion(YieldTypes eYield)
int CvLuaPlayer::lGetYieldPerTurnFromReligion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const YieldTypes eIndex2 = (YieldTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetYieldPerTurnFromReligion(eIndex2);
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
// int GetMinimumFaithNextGreatProphet() const
int CvLuaPlayer::lGetMinimumFaithNextGreatProphet(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int iFaith = pkPlayer->GetReligions()->GetCostNextProphet(true /*bIncludeBeliefDiscounts*/, true /*bAdjustForSpeedDifficulty*/, MOD_GLOBAL_TRULY_FREE_GP);
#else
	int iFaith = pkPlayer->GetReligions()->GetCostNextProphet(true /*bIncludeBeliefDiscounts*/, true /*bAdjustForSpeedDifficulty*/);
#endif
	lua_pushinteger(L, iFaith);

	return 1;
}
//------------------------------------------------------------------------------
// bool HasReligionInMostCities() const
int CvLuaPlayer::lHasReligionInMostCities(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);

	bool bResult = pkPlayer->GetReligions()->HasReligionInMostCities(eReligion);
	lua_pushboolean(L, bResult);

	return 1;
}
//------------------------------------------------------------------------------
// bool DoesUnitPassFaithPurchaseCheck(UnitTypes eUnit)
int CvLuaPlayer::lDoesUnitPassFaithPurchaseCheck(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);

	bool bRtnValue = CvReligionAIHelpers::DoesUnitPassFaithPurchaseCheck(*pkPlayer, eUnit);

	lua_pushboolean(L, bRtnValue);

	return 1;
}
//------------------------------------------------------------------------------
//int GetHappiness();
int CvLuaPlayer::lGetHappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::GetHappiness);
}
//------------------------------------------------------------------------------
//void SetHappiness(int iNewValue);
int CvLuaPlayer::lSetHappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::SetHappiness);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetEmpireHappinessForCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetEmpireHappinessForCity();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetEmpireUnhappinessForCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetEmpireUnhappinessForCity();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetEmpireHappinessFromCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::GetEmpireHappinessFromCities);
}

int CvLuaPlayer::lGetHappinessForGAP(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessForGAP);
}

//------------------------------------------------------------------------------
//int GetExcessHappiness();
int CvLuaPlayer::lGetExcessHappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetExcessHappiness);
}

//------------------------------------------------------------------------------
//int GetExcessHappiness();
int CvLuaPlayer::lGetHappinessRatioRawPercent(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessRatioRawPercent);
}

//------------------------------------------------------------------------------
//bool IsEmpireUnhappy() const;
int CvLuaPlayer::lIsEmpireUnhappy(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsEmpireUnhappy);
}

//------------------------------------------------------------------------------
//bool IsEmpireVeryUnhappy() const;
int CvLuaPlayer::lIsEmpireVeryUnhappy(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsEmpireVeryUnhappy);
}

//------------------------------------------------------------------------------
//bool IsEmpireSuperUnhappy() const;
int CvLuaPlayer::lIsEmpireSuperUnhappy(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsEmpireSuperUnhappy);
}

//------------------------------------------------------------------------------
//int GetHappinessFromPolicies() const;
int CvLuaPlayer::lGetHappinessFromPolicies(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromPolicies);
}

//------------------------------------------------------------------------------
//int GetHappinessFromCities() const;
int CvLuaPlayer::lGetHappinessFromCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromCities);
}

//------------------------------------------------------------------------------
//int GetHappinessFromBuildings() const;
int CvLuaPlayer::lGetHappinessFromBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromBuildings);
}

//------------------------------------------------------------------------------
//int GetExtraHappinessPerCity() const;
int CvLuaPlayer::lGetExtraHappinessPerCity(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetExtraHappinessPerCity);
}

//------------------------------------------------------------------------------
//void ChangeExtraHappinessPerCity(int iChange);
int CvLuaPlayer::lChangeExtraHappinessPerCity(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeExtraHappinessPerCity);
}

//------------------------------------------------------------------------------
//int GetHappinessFromResources() const;
int CvLuaPlayer::lGetHappinessFromResources(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromResources);
}
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
//------------------------------------------------------------------------------
//int GetHappinessFromResourceMonopolies() const;
int CvLuaPlayer::lGetHappinessFromResourceMonopolies(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromResourceMonopolies);
}

//int getUnhappinessFromCitizenNeeds() const;
int CvLuaPlayer::lGetUnhappinessFromCitizenNeeds(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getUnhappinessFromCitizenNeeds);
}
//int getUnhappinessFromCitizenNeeds() const;
int CvLuaPlayer::lGetHappinessFromCitizenNeeds(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getHappinessFromCitizenNeeds);
}

int CvLuaPlayer::lGetBonusHappinessFromLuxuriesFlat(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetBonusHappinessFromLuxuriesFlat);
}
int CvLuaPlayer::lGetBonusHappinessFromLuxuriesFlatForUI(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetBonusHappinessFromLuxuriesFlatForUI);
}

int CvLuaPlayer::lGetHandicapHappiness(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	int iHappiness = pkPlayer->getHandicapInfo().getHappinessDefault() + GC.getGame().getGameSpeedInfo().GetStartingHappiness();
	
	lua_pushinteger(L, iHappiness);
	return 1;
}

#endif
//------------------------------------------------------------------------------
//int GetHappinessFromResourceVariety() const;
int CvLuaPlayer::lGetHappinessFromResourceVariety(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromResourceVariety);
}

//------------------------------------------------------------------------------
//int GetExtraHappinessPerLuxury() const;
int CvLuaPlayer::lGetExtraHappinessPerLuxury(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetExtraHappinessPerLuxury);
}

//------------------------------------------------------------------------------
//int GetHappinessFromReligion() const;
int CvLuaPlayer::lGetHappinessFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromReligion);
}

//------------------------------------------------------------------------------
//int GetHappinessFromNaturalWonders() const;
int CvLuaPlayer::lGetHappinessFromNaturalWonders(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromNaturalWonders);
}

//------------------------------------------------------------------------------
//int GetHappinessFromLeagues() const;
int CvLuaPlayer::lGetHappinessFromLeagues(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromLeagues);
}

//------------------------------------------------------------------------------
//int GetUnhappiness() const;
int CvLuaPlayer::lGetUnhappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetUnhappiness);
}

//------------------------------------------------------------------------------
//int GetUnhappinessForecast() const;
int CvLuaPlayer::lGetUnhappinessForecast(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkAssumeCityAnnexed = CvLuaCity::GetInstance(L, 2, false);
	CvCity* pkAssumeCityPuppeted = CvLuaCity::GetInstance(L, 3, false);

	int iPrevUnhappiness = pkPlayer->GetUnhappiness();
	const int iUnhappiness = pkPlayer->DoUpdateTotalUnhappiness(pkAssumeCityAnnexed, pkAssumeCityPuppeted);
	lua_pushinteger(L, iUnhappiness);

	pkPlayer->SetUnhappiness(iPrevUnhappiness);
	return 1;
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromCityForUI() const;
int CvLuaPlayer::lGetUnhappinessFromCityForUI(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2, false);

	const int iUnhappiness = pkPlayer->GetUnhappinessFromCityForUI(pkCity);
	lua_pushinteger(L, iUnhappiness);
	return 1;
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromCityCount() const;
int CvLuaPlayer::lGetUnhappinessFromCityCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pAnnexedCity = CvLuaCity::GetInstance(L, 2, false);
	CvCity* pPuppetedCity = CvLuaCity::GetInstance(L, 3, false);
	const int iResult = pkPlayer->GetUnhappinessFromCityCount(pAnnexedCity, pPuppetedCity);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromCapturedCityCount() const;
int CvLuaPlayer::lGetUnhappinessFromCapturedCityCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pAnnexedCity = CvLuaCity::GetInstance(L, 2, false);
	CvCity* pPuppetedCity = CvLuaCity::GetInstance(L, 3, false);
	const int iResult = pkPlayer->GetUnhappinessFromCapturedCityCount(pAnnexedCity, pPuppetedCity);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromCityPopulation() const;
int CvLuaPlayer::lGetUnhappinessFromCityPopulation(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pAnnexedCity = CvLuaCity::GetInstance(L, 2, false);
	CvCity* pPuppetedCity = CvLuaCity::GetInstance(L, 3, false);
	const int iResult = pkPlayer->GetUnhappinessFromCityPopulation(pAnnexedCity, pPuppetedCity);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromCitySpecialists() const;
int CvLuaPlayer::lGetUnhappinessFromCitySpecialists(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pAnnexedCity = CvLuaCity::GetInstance(L, 2, false);
	CvCity* pPuppetedCity = CvLuaCity::GetInstance(L, 3, false);
	const int iResult = pkPlayer->GetUnhappinessFromCitySpecialists(pAnnexedCity, pPuppetedCity);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromPuppetCityPopulation() const;
int CvLuaPlayer::lGetUnhappinessFromPuppetCityPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetUnhappinessFromPuppetCityPopulation);
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromOccupiedCities() const;
int CvLuaPlayer::lGetUnhappinessFromOccupiedCities(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pAnnexedCity = CvLuaCity::GetInstance(L, 2, false);
	CvCity* pPuppetedCity = CvLuaCity::GetInstance(L, 3, false);
	const int iResult = pkPlayer->GetUnhappinessFromOccupiedCities(pAnnexedCity, pPuppetedCity);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromPublicOpinion() const;
int CvLuaPlayer::lGetUnhappinessFromPublicOpinion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetCulture()->GetPublicOpinionUnhappiness();
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE_HAPPINESS)
//------------------------------------------------------------------------------
//int GetUnhappinessFromWarWeariness() const;
int CvLuaPlayer::lGetUnhappinessFromWarWeariness(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetUnhappinessFromWarWeariness();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetUnhappinessFromWarWeariness() const;
int CvLuaPlayer::lGetWarWeariness(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetCulture()->GetWarWeariness();
	lua_pushinteger(L, min(75, iResult));
	return 1;
}
//------------------------------------------------------------------------------
//int GetUnhappinessFromWarWeariness() const;
int CvLuaPlayer::lSetWarWeariness(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = lua_tointeger(L, 2);
	pkPlayer->GetCulture()->SetWarWeariness(iResult);
	return 0;
}
int CvLuaPlayer::lGetWarWearinessSupplyReduction(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iSupply = pkPlayer->GetNumUnitsSuppliedByHandicap();
	iSupply += pkPlayer->GetNumUnitsSuppliedByCities();
	iSupply += pkPlayer->GetNumUnitsSuppliedByPopulation();

	int iWarWeariness = pkPlayer->GetCulture()->GetWarWeariness() / 2;
	int iMod = (100 - min(75, iWarWeariness));
	int iSupplyReduction = iSupply;
	iSupplyReduction *= iMod;
	iSupplyReduction /= 100;

	lua_pushinteger(L, iSupply - iSupplyReduction);
	return 1;
}

int CvLuaPlayer::lGetTechSupplyReduction(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iTotalSupplyWithoutReduction = pkPlayer->GetNumUnitsSuppliedByHandicap(true);
	iTotalSupplyWithoutReduction += pkPlayer->GetNumUnitsSuppliedByCities(true);
	iTotalSupplyWithoutReduction += pkPlayer->GetNumUnitsSuppliedByPopulation(true);

	int iTotalSupplyWithReduction = pkPlayer->GetNumUnitsSuppliedByHandicap();
	iTotalSupplyWithReduction += pkPlayer->GetNumUnitsSuppliedByCities();
	iTotalSupplyWithReduction += pkPlayer->GetNumUnitsSuppliedByPopulation();

	int iTotalSupply = (iTotalSupplyWithoutReduction - iTotalSupplyWithReduction);

	lua_pushinteger(L, iTotalSupply);
	return 1;
}

#endif
//------------------------------------------------------------------------------
//int GetUnhappinessFromUnits() const;
int CvLuaPlayer::lGetUnhappinessFromUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetUnhappinessFromUnits);
}

//------------------------------------------------------------------------------
//void ChangeUnhappinessFromUnits(int iChange);
int CvLuaPlayer::lChangeUnhappinessFromUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeUnhappinessFromUnits);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetUnhappinessMod(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetUnhappinessMod);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCityCountUnhappinessMod(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCityCountUnhappinessMod);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetOccupiedPopulationUnhappinessMod(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetOccupiedPopulationUnhappinessMod);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCapitalUnhappinessMod(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCapitalUnhappinessMod);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTraitCityUnhappinessMod(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerTraits()->GetCityUnhappinessModifier();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTraitPopUnhappinessMod(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerTraits()->GetPopulationUnhappinessModifier();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPuppetYieldPenalty(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);

	int iResult = pkPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + pkPlayer->GetPuppetYieldPenaltyMod();
	switch (eYield)
	{
		case(YIELD_FOOD) :
			iResult += GC.getPUPPET_GROWTH_MODIFIER();
			break;
		case(YIELD_PRODUCTION) :
			iResult += GC.getPUPPET_PRODUCTION_MODIFIER();
			break;
		case(YIELD_SCIENCE) :
			iResult += GC.getPUPPET_SCIENCE_MODIFIER();
			break;
		case(YIELD_GOLD) :
			iResult += GC.getPUPPET_GOLD_MODIFIER();
			break;
		case(YIELD_FAITH) :
			iResult += GC.getPUPPET_FAITH_MODIFIER();
			break;
		case(YIELD_TOURISM) :
			iResult += GC.getPUPPET_TOURISM_MODIFIER();
			break;
		case(YIELD_CULTURE) :
			iResult += GC.getPUPPET_CULTURE_MODIFIER();
			break;
	}
	if (iResult > 0)
		iResult = 0;

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lIsHalfSpecialistUnhappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isHalfSpecialistUnhappiness);
}

//------------------------------------------------------------------------------
//int GetHappinessPerGarrisonedUnit() const;
int CvLuaPlayer::lGetHappinessPerGarrisonedUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessPerGarrisonedUnit);
}
//------------------------------------------------------------------------------
//void SetHappinessPerGarrisonedUnit(int iValue);
int CvLuaPlayer::lSetHappinessPerGarrisonedUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetHappinessPerGarrisonedUnit);
}
//------------------------------------------------------------------------------
//void ChangeHappinessPerGarrisonedUnit(int iChange);
int CvLuaPlayer::lChangeHappinessPerGarrisonedUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeHappinessPerGarrisonedUnit);
}
//------------------------------------------------------------------------------
//int GetHappinessFromTradeRoutes() const;
int CvLuaPlayer::lGetHappinessFromTradeRoutes(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromTradeRoutes);
}
//------------------------------------------------------------------------------
//int GetHappinessPerTradeRoute() const;
int CvLuaPlayer::lGetHappinessPerTradeRoute(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessPerTradeRoute);
}
//------------------------------------------------------------------------------
//void SetHappinessPerTradeRoute(int iValue);
int CvLuaPlayer::lSetHappinessPerTradeRoute(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetHappinessPerTradeRoute);
}
//------------------------------------------------------------------------------
//void ChangeHappinessPerTradeRoute(int iChange);
int CvLuaPlayer::lChangeHappinessPerTradeRoute(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeHappinessPerTradeRoute);
}
//------------------------------------------------------------------------------
//void GetTradeRouteModifier ()
int CvLuaPlayer::lGetCityConnectionTradeRouteGoldModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetTreasury()->GetCityConnectionTradeRouteGoldModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void GetInternationalTradeRoutePlotToolTip ()
int CvLuaPlayer::lGetInternationalTradeRoutePlotToolTip(lua_State* L)
{
	lua_createtable(L, 0, 0);
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pPlot = CvLuaPlot::GetInstance(L, 2, false);
	std::vector<CvString> astrToolTips;
	astrToolTips = pkPlayer->GetTrade()->GetPlotToolTips(pPlot);

	int index = 1;
	for (int i = astrToolTips.size(); i > 0; i--)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);
		lua_pushstring(L, astrToolTips[i - 1]);
		lua_setfield(L, t, "String");
		lua_rawseti(L, -2, index++);
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRoutePlotMouseoverToolTip(lua_State* L)
{
	lua_createtable(L, 0, 0);
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pPlot = CvLuaPlot::GetInstance(L, 2, false);
	std::vector<CvString> astrToolTips;
	astrToolTips = pkPlayer->GetTrade()->GetPlotMouseoverToolTips(pPlot);

	int index = 1;
	for (int i = astrToolTips.size(); i > 0; i--)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);
		lua_pushstring(L, astrToolTips[i - 1]);
		lua_setfield(L, t, "String");
		lua_rawseti(L, -2, index++);
	}

	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumInternationalTradeRoutesUsed(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	//for historical reasons, we don't return the actual trade routes but the number of trade units here
	const int iResult = pkPlayer->GetTrade()->GetNumTradeUnits(true);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumInternationalTradeRoutesAvailable(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetTrade()->GetNumTradeRoutesPossible();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPotentialInternationalTradeRouteDestinationsFrom(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2, false);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3, false);

	return GetPotentialInternationalTradeRouteDestinationsHelper(L, pkPlayer, pkUnit, pkCity->plot());
}

int CvLuaPlayer::lGetPotentialInternationalTradeRouteDestinations(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2, false);

	return GetPotentialInternationalTradeRouteDestinationsHelper(L, pkPlayer, pkUnit, pkUnit->plot());
}

int CvLuaPlayer::GetPotentialInternationalTradeRouteDestinationsHelper(lua_State* L, CvPlayerAI* pkPlayer, CvUnit* pkUnit, CvPlot* pkUnitPlot)
{
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = pkUnitPlot->getPlotCity();

	//make sure we always have fresh data
	GC.getGame().GetGameTrade()->InvalidateTradePathCache(pkPlayer->GetID());

	lua_createtable(L, 0, 0);
	int index = 1;

	if (pkUnit->canMakeTradeRoute(pkUnitPlot))
	{
		PlayerTypes ePlayer = NO_PLAYER;
		for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
		{
			ePlayer = (PlayerTypes)ui;

			if (!GET_PLAYER(ePlayer).isAlive())
			{
				continue;
			}

			if (GET_PLAYER(ePlayer).isBarbarian())
			{
				continue;
			}

			CvPlayerTrade* pOtherPlayerTrade = GET_PLAYER(ePlayer).GetTrade();

			int iCityLoop;
			CvCity* pLoopCity = NULL;
			for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
			{
				int iLoopCityX = pLoopCity->getX();
				int iLoopCityY = pLoopCity->getY();

				for (uint uiConnectionType = 0; uiConnectionType < NUM_TRADE_CONNECTION_TYPES; uiConnectionType++)
				{
					TradeConnectionType eConnectionType = (TradeConnectionType)uiConnectionType;
					if (pkUnit->canMakeTradeRouteAt(pkUnitPlot, iLoopCityX, iLoopCityY, eConnectionType))
					{
						TradeConnection kTradeConnection;
						kTradeConnection.SetCities(pOriginCity,pLoopCity);
						kTradeConnection.m_eDomain = pkUnit->getDomainType();
						kTradeConnection.m_eConnectionType = eConnectionType;

						lua_createtable(L, 0, 0);
						const int t = lua_gettop(L);
						lua_pushinteger(L, pLoopCity->getX());
						lua_setfield(L, t, "X");
						lua_pushinteger(L, pLoopCity->getY());
						lua_setfield(L, t, "Y");
						lua_pushinteger(L, uiConnectionType);
						lua_setfield(L, t, "TradeConnectionType");
						
						ReligionTypes eToReligion = NO_RELIGION;
						int iToPressureAmount = 0;
						ReligionTypes eFromReligion = NO_RELIGION;
						int iFromPressureAmount = 0;

						pOriginCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pLoopCity, eToReligion, iToPressureAmount);
						pLoopCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pOriginCity, eFromReligion, iFromPressureAmount);

						int iTradeReligionModifer = pkPlayer->GetPlayerTraits()->GetTradeReligionModifier();
#if defined(MOD_BALANCE_CORE_POLICIES)
						iTradeReligionModifer += pkPlayer->GetTradeReligionModifier();
#endif
						if (iTradeReligionModifer != 0)
						{
							iToPressureAmount *= 100 + iTradeReligionModifer;
							iToPressureAmount /= 100;
						}

						// Internally pressure is now 10 times greater than what is shown to user
						iToPressureAmount /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();
						iFromPressureAmount /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();

						lua_pushinteger(L, eToReligion);
						lua_setfield(L, t, "ToReligion");
						lua_pushinteger(L, iToPressureAmount);
						lua_setfield(L, t, "ToPressureAmount");
						lua_pushinteger(L, eFromReligion);
						lua_setfield(L, t, "FromReligion");
						lua_pushinteger(L, iFromPressureAmount);
						lua_setfield(L, t, "FromPressureAmount");

						lua_pushboolean(L, pPlayerTrade->IsPreviousTradeRoute(pOriginCity, pLoopCity, pkUnit->getDomainType(), eConnectionType));
						lua_setfield(L, t, "OldTradeRoute");

						int iInnerIndex = 1;
						lua_createtable(L, 0, 0);
						for (uint uiYield = 0; uiYield < NUM_YIELD_TYPES; uiYield++)
						{
							lua_createtable(L, 0, 0);
							const int t2 = lua_gettop(L);
							lua_pushinteger(L, pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, (YieldTypes)uiYield, true));
							lua_setfield(L, t2, "Mine");
							lua_pushinteger(L, pOtherPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, (YieldTypes)uiYield, false));
							lua_setfield(L, t2, "Theirs");
							lua_rawseti(L, -2, iInnerIndex++);
						}
						lua_setfield(L, t, "Yields");
						lua_rawseti(L, -2, index++);
					}
				}
			}
		}
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteBaseBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	bool bOrigin = lua_toboolean(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionBaseValueTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteGPTBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	bool bOrigin = lua_toboolean(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionGPTValueTimes100(kTradeConnection, YIELD_GOLD, true, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteResourceBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	bool bOrigin = lua_toboolean(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionResourceValueTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCityResourceBonus(lua_State* L)
{
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	bool bOrigin = lua_toboolean(L, 4);
	
	int iOurResources = 0;
	int iTheirResources = 0;
	for (int i = 0; i < GC.getNumResourceInfos(); i++)
	{
		ResourceTypes eResource = (ResourceTypes)i;
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo)
		{
			bool bOurMonopoly = false;
			bool bTheirMonopoly = false;
			if (GET_PLAYER(pOriginCity->getOwner()).HasGlobalMonopoly(eResource))
				bOurMonopoly = true;;
			if (GET_PLAYER(pDestCity->getOwner()).HasGlobalMonopoly(eResource))
				bTheirMonopoly= true;

			int iTempOurs = 0;
			int iTempTheirs = 0;
			if (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iTempOurs += bOurMonopoly ? pOriginCity->GetNumResourceLocal(eResource, true) * 2 : pOriginCity->GetNumResourceLocal(eResource, true);
				iTempTheirs += bTheirMonopoly ? pDestCity->GetNumResourceLocal(eResource, true) * 2 : pDestCity->GetNumResourceLocal(eResource, true);
			}

			//bonus only applies for resources unique to one or the other city.
			if (iTempOurs > 0 && iTempTheirs > 0)
				continue;

			iOurResources += iTempOurs;
			iTheirResources += iTempTheirs;
		}
	}

	lua_pushinteger(L, bOrigin ? iOurResources : iTheirResources);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteResourceTraitModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetTradeRouteResourceModifier());

	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteExclusiveBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);

	int iResult = pPlayerTrade->GetTradeConnectionExclusiveValueTimes100(kTradeConnection, YIELD_GOLD);
	lua_pushinteger(L, iResult);

	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteYourBuildingBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteTheirBuildingBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRoutePolicyBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;
	kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionPolicyValueTimes100(kTradeConnection, YIELD_GOLD);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteOtherTraitBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;
	kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteRiverModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

	int iResult = pPlayerTrade->GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeConnectionDistanceValueModifierTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity, pDestCity);
	kTradeConnection.m_eDomain = eDomain;
	kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeRouteTurns(lua_State* L)
{
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);

	int iTurns = GC.getGame().GetGameTrade()->GetTradeRouteTurns(pOriginCity, pDestCity, eDomain, NULL)-1;
	lua_pushinteger(L, iTurns);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeConnectionDistance(lua_State* L)
{
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity, pDestCity);
	kTradeConnection.m_eDomain = eDomain;

	SPath path;
	bool bTradeAvailable = GC.getGame().GetGameTrade()->IsValidTradeRoutePath(pOriginCity, pDestCity, kTradeConnection.m_eDomain, &path);
	if (!bTradeAvailable)
		return 0;

	int iLength = path.iNormalizedDistanceRaw / SPath::getNormalizedDistanceBase();
	lua_pushinteger(L, iLength);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeConnectionOpenBordersModifierTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	bool bOrigin = lua_toboolean(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteCorporationModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	bool bOrigin = lua_toboolean(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);

	int iResult = pPlayerTrade->GetTradeConnectionCorporationModifierTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteCorporationModifierScience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	bool bOrigin = lua_toboolean(L, 4);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionCorporationModifierTimes100(kTradeConnection, YIELD_SCIENCE, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumberofGlobalFranchises(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iResult = pkPlayer->GetCorporations()->GetNumFranchises();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumberofOffices(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iResult = pkPlayer->GetCorporations()->GetNumOffices();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMaxFranchises(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iMax = pkPlayer->GetCorporations()->GetMaxNumFranchises();
	lua_pushinteger(L, iMax);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCorporation(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iResult = pkPlayer->GetCorporations()->GetFoundedCorporation();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCorporationFoundedTurn(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvCorporation* pCorporation = pkPlayer->GetCorporations()->GetCorporation();
	int iResult = -1;
	if (pCorporation != NULL)
	{
		iResult = pCorporation->m_iTurnFounded;
	}
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCurrentOfficeBenefit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetCorporations()->GetCurrentOfficeBenefit());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeRouteBenefitHelper(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetCorporations()->GetTradeRouteBenefit());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumFranchisesTooltip(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetCorporations()->GetNumFranchisesTooltip());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lCanCreateFranchiseInCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkFromCity = CvLuaCity::GetInstance(L, 2);
	CvCity* pkToCity = CvLuaCity::GetInstance(L, 3);
	lua_pushboolean(L, pkPlayer->GetCorporations()->CanCreateFranchiseInCity(pkFromCity, pkToCity));
	return 1;
}

#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteDomainModifier(lua_State* L)
{
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 2);
	int iResult = GC.getGame().GetGameTrade()->GetDomainModifierTimes100(eDomain);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeRouteYieldModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	int iResult = 0;
	if (pkPlayer->getCapitalCity() == pOriginCity || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion())
	{
		if (pOriginCity->getOwner() == pDestCity->getOwner())
		{
			iResult += pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_CAPITAL_MODIFIER);
		}
		else
		{
			iResult += pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);
		}
	}

	if (pOriginCity->getOwner() == pDestCity->getOwner())
	{
		iResult += pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_MODIFIER);
	}
	else
	{
		iResult += pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MODIFIER);
	}

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteTotal(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

	if (pOriginCity->getOwner() != pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;
	}
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	else if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteScience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

	if (pOriginCity->getOwner() != pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;
	}
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	else if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_SCIENCE, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;	
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteCulture(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity, pDestCity);
	kTradeConnection.m_eDomain = eDomain;

	if (pOriginCity->getOwner() != pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;
	}
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	else if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_CULTURE, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteProduction(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

	if (pOriginCity->getOwner() != pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;
	}
	else
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_PRODUCTION;

	int iResult = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_PRODUCTION, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;	
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetInternationalTradeRouteFood(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

	if (pOriginCity->getOwner() != pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_INTERNATIONAL;
	}
	else
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_FOOD;

	int iResult = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_FOOD, bOrigin);
	lua_pushinteger(L, iResult);

	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivGoldBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	CvCity* pOriginCity = CvLuaCity::GetInstance(L, 2, true);
	CvCity* pDestCity = CvLuaCity::GetInstance(L, 3, true);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 4);
	bool bOrigin = lua_toboolean(L, 5);

	TradeConnection kTradeConnection;
	kTradeConnection.SetCities(pOriginCity,pDestCity);
	kTradeConnection.m_eDomain = eDomain;

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && pOriginCity->getOwner() == pDestCity->getOwner())
	{
		kTradeConnection.m_eConnectionType = TRADE_CONNECTION_GOLD_INTERNAL;
	}
#endif

	int iResult = pPlayerTrade->GetMinorCivGoldBonus(kTradeConnection, YIELD_GOLD, bOrigin);
	lua_pushinteger(L, iResult);
	return 1;	
}

#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPotentialTradeUnitNewHomeCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2, false);
	CvPlot* pkUnitPlot = pkUnit->plot();

	lua_createtable(L, 0, 0);
	int index = 1;

	if (pkUnit->canChangeTradeUnitHomeCity(pkUnitPlot))
	{
		int iCityLoop;
		CvCity* pLoopCity = NULL;
		for(pLoopCity = pkPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iCityLoop))
		{
			int iLoopCityX = pLoopCity->getX();
			int iLoopCityY = pLoopCity->getY();

			// can't change to its own home city
			if (pLoopCity->plot() == pkUnitPlot)
			{
				continue;
			}

			if (pkUnit->canChangeTradeUnitHomeCityAt(pkUnitPlot, iLoopCityX, iLoopCityY))
			{
				lua_createtable(L, 0, 0);
				const int t = lua_gettop(L);
				lua_pushinteger(L, pLoopCity->getX());
				lua_setfield(L, t, "X");
				lua_pushinteger(L, pLoopCity->getY());
				lua_setfield(L, t, "Y");
				lua_rawseti(L, -2, index++);
			}
		}
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPotentialAdmiralNewPort(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2, false);
	CvPlot* pkUnitPlot = pkUnit->plot();

	lua_createtable(L, 0, 0);
	int index = 1;

	if (pkUnit->canChangeAdmiralPort(pkUnitPlot))
	{
		int iCityLoop;
		CvCity* pLoopCity = NULL;
		for(pLoopCity = pkPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iCityLoop))
		{
			int iLoopCityX = pLoopCity->getX();
			int iLoopCityY = pLoopCity->getY();

			// can't change to its own home city
			if (pLoopCity->plot() == pkUnitPlot)
			{
				continue;
			}

			if (pkUnit->canChangeAdmiralPortAt(pkUnitPlot, iLoopCityX, iLoopCityY))
			{
				lua_createtable(L, 0, 0);
				const int t = lua_gettop(L);
				lua_pushinteger(L, pLoopCity->getX());
				lua_setfield(L, t, "X");
				lua_pushinteger(L, pLoopCity->getY());
				lua_setfield(L, t, "Y");
				lua_rawseti(L, -2, index++);
			}
		}
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumAvailableTradeUnits(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 2);
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();

	int iCount = 0;
	int iLoopUnit;
	CvUnit* pLoopUnit;
	for (pLoopUnit = pkPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = pkPlayer->nextUnit(&iLoopUnit))
	{
		if (pLoopUnit->isTrade() && pLoopUnit->getDomainType() == eDomain && !pTrade->IsUnitIDUsed(pLoopUnit->GetID()))
		{
			iCount++;
		}
	}

	lua_pushinteger(L, iCount);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeUnitType(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetTrade()->GetTradeUnit(eDomain, pkPlayer));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeYourRoutesTTString(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();

	CvString strResult = "";	

	const TradeConnection* pConnection = NULL;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		pConnection = &(pTrade->GetTradeConnection(ui));
		if (pConnection->m_eOriginOwner == pkPlayer->GetID())
		{
			CvPlot* pOriginPlot = GC.getMap().plot(pConnection->m_iOriginX, pConnection->m_iOriginY);
			CvPlot* pDestPlot = GC.getMap().plot(pConnection->m_iDestX, pConnection->m_iDestY);
			if (pOriginPlot == NULL || pDestPlot == NULL)
			{
				continue;
			}

			CvCity* pOriginCity = pOriginPlot->getPlotCity();
			CvCity* pDestCity = pDestPlot->getPlotCity();

			if (pOriginCity == NULL || pDestCity == NULL)
			{
				continue;
			}


			CvString strOriginYieldsStr = "";
			for (uint uiYield = 0; uiYield < NUM_YIELD_TYPES; uiYield++)
			{
				YieldTypes eYield = (YieldTypes)uiYield;
				int iYieldQuantity = pPlayerTrade->GetTradeConnectionValueTimes100(*pConnection, eYield, true);
				if (iYieldQuantity != 0)
				{
					if (strOriginYieldsStr != "") 
					{
						strOriginYieldsStr += ", ";
					}

					switch (eYield)
					{
					case YIELD_FOOD:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FOOD_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_PRODUCTION:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_PRODUCTION_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_GOLD:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_GOLD_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_SCIENCE:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_SCIENCE_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_CULTURE:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_CULTURE_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_FAITH:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FAITH_YIELD_TT", iYieldQuantity / 100);
						break;
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
					case YIELD_TOURISM:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_TOURISM_YIELD_TT", iYieldQuantity / 100);
						break;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
					case YIELD_GOLDEN_AGE_POINTS:
						strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_GOLDEN_AGE_POINTS_YIELD_TT", iYieldQuantity / 100);
						break;
#endif
					}
				}
			}

			CvString strDestYieldsStr = "";
			for (uint uiYield = 0; uiYield < NUM_YIELD_TYPES; uiYield++)
			{
				YieldTypes eYield = (YieldTypes)uiYield;
				int iYieldQuantity = pPlayerTrade->GetTradeConnectionValueTimes100(*pConnection, eYield, false);
				if (iYieldQuantity != 0)
				{
					switch (eYield)
					{
					case YIELD_FOOD:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FOOD_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_PRODUCTION:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_PRODUCTION_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_GOLD:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_GOLD_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_SCIENCE:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_SCIENCE_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_CULTURE:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_CULTURE_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_FAITH:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FAITH_YIELD_TT", iYieldQuantity / 100);
						break;
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
					case YIELD_TOURISM:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_TOURISM_YIELD_TT", iYieldQuantity / 100);
						break;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
					case YIELD_GOLDEN_AGE_POINTS:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_GOLDEN_AGE_POINTS_YIELD_TT", iYieldQuantity / 100);
						break;
#endif
					}
				}
			}

			//CvUnitEntry* pUnitEntry = GC.getUnitInfo(pPlayerTrade->GetTradeUnit(pConnection->m_eDomain));

			Localization::String strBuffer;
			if (pConnection->m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
			{
				if (strOriginYieldsStr != "" && strDestYieldsStr != "")
				{
					strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_YOUR_ROUTE_BOTH_TT");
				}
				else if (strOriginYieldsStr != "")
				{
					strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_YOUR_ROUTE_ONLY_ORIGIN_TT");
				}
				else if (strDestYieldsStr != "")
				{
					strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_YOUR_ROUTE_ONLY_DEST_TT");
				}
			}
			else
			{
				if (strOriginYieldsStr != "" && strDestYieldsStr != "")
				{
					strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_INTERNAL_YOUR_ROUTE_BOTH_TT");
				}
				else if (strOriginYieldsStr != "")
				{
					strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_INTERNAL_YOUR_ROUTE_ONLY_ORIGIN_TT");
				}
				else if (strDestYieldsStr != "")
				{
					strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_INTERNAL_YOUR_ROUTE_ONLY_DEST_TT");
				}
			}

			strBuffer << pOriginCity->getNameKey();
			if (strOriginYieldsStr != "")
			{
				strBuffer << strOriginYieldsStr;
			}
			
			if (pConnection->m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
			{
				if (GET_PLAYER(pDestCity->getOwner()).isMinorCiv())
				{
					strBuffer << "TXT_KEY_TOP_PANEL_ITR_CITY_STATE_TT";
				}
				else
				{
					strBuffer << GET_PLAYER(pDestCity->getOwner()).getCivilizationShortDescription();
				}
			}

			strBuffer << pDestCity->getNameKey();
			if (strDestYieldsStr != "")
			{
				strBuffer << strDestYieldsStr;
			}

			if (strResult != "")
			{
				strResult += "[NEWLINE]";
			}

			strResult += strBuffer.toUTF8();
		}
	}
	
	lua_pushstring(L, strResult);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeToYouRoutesTTString(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();

	CvString strResult = "";	

	const TradeConnection* pConnection = NULL;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		pConnection = &(pTrade->GetTradeConnection(ui));

		// don't include internal trade, but does this not count teams sharing stuff between each other
		if (pConnection->m_eConnectionType != TRADE_CONNECTION_INTERNATIONAL)
		{
			continue;
		}

		if (pConnection->m_eDestOwner == pkPlayer->GetID())
		{
			CvPlot* pOriginPlot = GC.getMap().plot(pConnection->m_iOriginX, pConnection->m_iOriginY);
			CvPlot* pDestPlot = GC.getMap().plot(pConnection->m_iDestX, pConnection->m_iDestY);
			if (pOriginPlot == NULL || pDestPlot == NULL)
			{
				continue;
			}

			CvCity* pOriginCity = pOriginPlot->getPlotCity();
			CvCity* pDestCity = pDestPlot->getPlotCity();

			if (pOriginCity == NULL || pDestCity == NULL)
			{
				continue;
			}

			CvString strOriginYieldsStr = "";
			//for (uint uiYield = 0; uiYield < NUM_YIELD_TYPES; uiYield++)
			//{
			//	YieldTypes eYield = (YieldTypes)uiYield;
			//	int iYieldQuantity = pPlayerTrade->GetTradeConnectionValueTimes100(*pConnection, eYield, false);
			//	if (iYieldQuantity != 0)
			//	{
			//		switch (eYield)
			//		{
			//		case YIELD_FOOD:
			//			strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FOOD_YIELD_TT", iYieldQuantity / 100);
			//			break;
			//		case YIELD_PRODUCTION:
			//			strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_PRODUCTION_YIELD_TT", iYieldQuantity / 100);
			//			break;
			//		case YIELD_GOLD:
			//			strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_GOLD_YIELD_TT", iYieldQuantity / 100);
			//			break;
			//		case YIELD_SCIENCE:
			//			strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_SCIENCE_YIELD_TT", iYieldQuantity / 100);
			//			break;
			//		case YIELD_CULTURE:
			//			strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_CULTURE_YIELD_TT", iYieldQuantity / 100);
			//			break;
			//		case YIELD_FAITH:
			//			strOriginYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FAITH_YIELD_TT", iYieldQuantity / 100);
			//			break;
			//		}
			//	}
			//}

			CvString strDestYieldsStr = "";
			for (uint uiYield = 0; uiYield < NUM_YIELD_TYPES; uiYield++)
			{
				YieldTypes eYield = (YieldTypes)uiYield;
				int iYieldQuantity = pPlayerTrade->GetTradeConnectionValueTimes100(*pConnection, eYield, false);
				if (iYieldQuantity != 0)
				{
					if (strDestYieldsStr != "") 
					{
						strDestYieldsStr += ", ";
					}
					switch (eYield)
					{
					case YIELD_FOOD:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FOOD_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_PRODUCTION:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_PRODUCTION_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_GOLD:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_GOLD_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_SCIENCE:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_SCIENCE_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_CULTURE:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_CULTURE_YIELD_TT", iYieldQuantity / 100);
						break;
					case YIELD_FAITH:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_FAITH_YIELD_TT", iYieldQuantity / 100);
						break;
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
					case YIELD_TOURISM:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_TOURISM_YIELD_TT", iYieldQuantity / 100);
						break;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
					case YIELD_GOLDEN_AGE_POINTS:
						strDestYieldsStr += GetLocalizedText("TXT_KEY_TOP_PANEL_ITR_GOLDEN_AGE_POINTS_YIELD_TT", iYieldQuantity / 100);
						break;
#endif
					}
				}
			}

			//CvUnitEntry* pUnitEntry = GC.getUnitInfo(pPlayerTrade->GetTradeUnit(pConnection->m_eDomain));

			Localization::String strBuffer;
			if (strOriginYieldsStr != "" && strDestYieldsStr != "")
			{
				strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_TO_YOU_ROUTE_BOTH_TT");
			}
			else if (strOriginYieldsStr != "")
			{
				strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_TO_YOU_ROUTE_ONLY_ORIGIN_TT");
			}
			else if (strDestYieldsStr != "")
			{
				strBuffer = Localization::Lookup("TXT_KEY_TOP_PANEL_ITR_TO_YOU_ROUTE_ONLY_DEST_TT");
			}

			if (pConnection->m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
			{
				if (GET_PLAYER(pDestCity->getOwner()).isMinorCiv())
				{
					strBuffer << "TXT_KEY_TOP_PANEL_ITR_CITY_STATE_TT";
				}
				else
				{
					strBuffer << GET_PLAYER(pOriginCity->getOwner()).getCivilizationShortDescription();
				}
			}

			strBuffer << pOriginCity->getNameKey();
			if (strOriginYieldsStr != "")
			{
				strBuffer << strOriginYieldsStr;
			}

			strBuffer << pDestCity->getNameKey();
			if (strDestYieldsStr != "")
			{
				strBuffer << strDestYieldsStr;
			}

			if (strResult != "")
			{
				strResult += "[NEWLINE]";
			}

			strResult += strBuffer.toUTF8();
		}
	}

	lua_pushstring(L, strResult);

	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsConnectedToPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();

	const bool bResult = pPlayerTrade->IsConnectedToPlayer(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}	
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsConnectionBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	bool bResult = false;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		int iYieldQuantity = pkPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner(eYield);
		if (iYieldQuantity > 0)
		{
			bResult = true;
			break;
		}
	}

	lua_pushboolean(L, bResult);
	return 1;
}	
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeRoutes(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_createtable(L, 0, 0);
	int index = 1;

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));
		if (pConnection->m_eOriginOwner != pkPlayer->GetID())
		{
			continue;
		}

		CvCity* pFromCity = GC.getMap().plot(pConnection->m_iOriginX, pConnection->m_iOriginY)->getPlotCity();
		CvCity* pToCity = GC.getMap().plot(pConnection->m_iDestX, pConnection->m_iDestY)->getPlotCity();

		CvPlayer* pToPlayer = &GET_PLAYER(pToCity->getOwner());

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lua_pushinteger(L, pConnection->m_eConnectionType);
		lua_setfield(L, t, "TradeConnectionType");
		lua_pushinteger(L, pConnection->m_eDomain);
		lua_setfield(L, t, "Domain");
		lua_pushinteger(L, pkPlayer->getCivilizationType());
		lua_setfield(L, t, "FromCivilizationType");
		lua_pushinteger(L , pkPlayer->GetID());
		lua_setfield(L, t, "FromID");
		lua_pushstring(L, pFromCity->getName());
		lua_setfield(L, t, "FromCityName");
		CvLuaCity::Push(L, pFromCity);
		lua_setfield(L, t, "FromCity");
		lua_pushinteger(L, GET_PLAYER(pConnection->m_eDestOwner).getCivilizationType());
		lua_setfield(L, t, "ToCivilizationType");
		lua_pushinteger(L, pToPlayer->GetID());
		lua_setfield(L, t, "ToID");
		lua_pushstring(L, pToCity->getName());
		lua_setfield(L, t, "ToCityName");
		CvLuaCity::Push(L, pToCity);
		lua_setfield(L, t, "ToCity");
		lua_pushinteger(L, pkPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_GOLD, true));
		lua_setfield(L, t, "FromGPT");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_GOLD, false));
		lua_setfield(L, t, "ToGPT");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_FOOD, false));
		lua_setfield(L, t, "ToFood");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_PRODUCTION, false));
		lua_setfield(L, t, "ToProduction");
		lua_pushinteger(L, pkPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_SCIENCE, true));
		lua_setfield(L, t, "FromScience");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_SCIENCE, false));
		lua_setfield(L, t, "ToScience");
		lua_pushinteger(L, pkPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_CULTURE, true));
		lua_setfield(L, t, "FromCulture");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_CULTURE, false));
		lua_setfield(L, t, "ToCulture");



		ReligionTypes eToReligion = NO_RELIGION;
		int iToPressure = 0;
		ReligionTypes eFromReligion = NO_RELIGION;
		int iFromPressure = 0;

		pFromCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pToCity, eToReligion, iToPressure);
		pToCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pFromCity, eFromReligion, iFromPressure);
		
		// Internally pressure is now 10 times greater than what is shown to user
		iToPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();
		iFromPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();

		lua_pushinteger(L, eToReligion);
		lua_setfield(L, t, "ToReligion");
		lua_pushinteger(L, iToPressure);
		lua_setfield(L, t, "ToPressure");
		lua_pushinteger(L, eFromReligion);
		lua_setfield(L, t, "FromReligion");
		lua_pushinteger(L, iFromPressure);
		lua_setfield(L, t, "FromPressure");
		int iToDelta = (pFromCity->GetBaseTourism() / 100) * pFromCity->GetCityCulture()->GetTourismMultiplier(pToPlayer->GetID(), true, true, false, true, true);
		int iFromDelta = (pToCity->GetBaseTourism() / 100) * pToCity->GetCityCulture()->GetTourismMultiplier(pkPlayer->GetID(), true, true, false, true, true);

		lua_pushinteger(L, iFromDelta);
		lua_setfield(L, t, "FromTourism");
		lua_pushinteger(L, iToDelta);
		lua_setfield(L, t, "ToTourism");

		lua_pushinteger(L, pConnection->m_iTurnRouteComplete - GC.getGame().getGameTurn());
		lua_setfield(L, t, "TurnsLeft");
		lua_pushinteger(L, pConnection->m_unitID);
		lua_setfield(L, t, "UnitID");
		lua_pushboolean(L, pConnection->m_bTradeUnitRecalled);
		lua_setfield(L, t, "IsRecalled");
		lua_pushinteger(L, pConnection->m_iCircuitsCompleted);
		lua_setfield(L, t, "CircuitsCompleted");
		lua_pushinteger(L, pConnection->m_iCircuitsToComplete);
		lua_setfield(L, t, "CircuitsToComplete");
		lua_pushboolean(L, pConnection->m_bTradeUnitMovingForward);
		lua_setfield(L, t, "MovingForward");

		lua_rawseti(L, -2, index++);
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeRoutesAvailable(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_createtable(L, 0, 0);
	int index = 1;

	//CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	CvPlayerTrade* pPlayerTrade = pkPlayer->GetTrade();
	// for each domain type
	//  for each origin city
	//    for each destination city
	//      display if a connection can be made

	int iOriginCityLoop;
	CvCity* pOriginCity = NULL;
	for (pOriginCity = pkPlayer->firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = pkPlayer->nextCity(&iOriginCityLoop))
	{
		PlayerTypes eOtherPlayer = NO_PLAYER;
		for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
		{
			eOtherPlayer = (PlayerTypes)ui;

			if (!GET_PLAYER(eOtherPlayer).isAlive())
			{
				continue;
			}

			if (GET_PLAYER(eOtherPlayer).isBarbarian())
			{
				continue;
			}

			int iDestCityLoop;
			CvCity* pDestCity = NULL;
			for (pDestCity = GET_PLAYER(eOtherPlayer).firstCity(&iDestCityLoop); pDestCity != NULL; pDestCity = GET_PLAYER(eOtherPlayer).nextCity(&iDestCityLoop))
			{
				// if this is the same city
				if (pOriginCity == pDestCity)
				{
					continue;
				}

				DomainTypes eDomain = NO_DOMAIN;
				for (eDomain = (DomainTypes)0; eDomain < NUM_DOMAIN_TYPES; eDomain = (DomainTypes)(eDomain + 1))
				{
					// if this isn't a valid trade domain, ignore
					if (eDomain != DOMAIN_LAND && eDomain != DOMAIN_SEA)
					{
						continue;
					}

					bool bCheckPath = true;
					TradeConnectionType eConnection = NUM_TRADE_CONNECTION_TYPES;
					for (uint uiConnectionTypes = 0; uiConnectionTypes < NUM_TRADE_CONNECTION_TYPES; uiConnectionTypes++)
					{
						eConnection = (TradeConnectionType)uiConnectionTypes;
				
						const TradeConnection* pConnection = pPlayerTrade->GetTradeConnection(pOriginCity, pDestCity);

						bool bTradeAvailable = !pConnection && pPlayerTrade->CanCreateTradeRoute(pOriginCity, pDestCity, eDomain, eConnection, true, bCheckPath);
						if (!bTradeAvailable)
						{
							continue;
						}

						bCheckPath = false;	// Once a trade route is valid for a domain, all further connections can assume the path is valid.

						lua_createtable(L, 0, 0);
						const int t = lua_gettop(L);

						TradeConnection kConnection;
						kConnection.SetCities(pOriginCity,pDestCity);
						kConnection.m_eConnectionType = eConnection;
						kConnection.m_eDomain = eDomain;

						int iTurnsLeft = -1;
						
						if (pConnection && pConnection->m_eDomain == eDomain)
						{
							iTurnsLeft = pConnection->m_iTurnRouteComplete - GC.getGame().getGameTurn();
						}

						lua_pushinteger(L, kConnection.m_eConnectionType);
						lua_setfield(L, t, "TradeConnectionType");
						lua_pushinteger(L, eDomain);
						lua_setfield(L, t, "Domain");
						lua_pushinteger(L, pkPlayer->getCivilizationType());
						lua_setfield(L, t, "FromCivilizationType");
						lua_pushinteger(L , pkPlayer->GetID());
						lua_setfield(L, t, "FromID");
						lua_pushstring(L, pOriginCity->getName());
						lua_setfield(L, t, "FromCityName");
						CvLuaCity::Push(L, pOriginCity);
						lua_setfield(L, t, "FromCity");
						lua_pushinteger(L, GET_PLAYER(eOtherPlayer).getCivilizationType());
						lua_setfield(L, t, "ToCivilizationType");
						lua_pushinteger(L, eOtherPlayer);
						lua_setfield(L, t, "ToID");
						lua_pushstring(L, pDestCity->getName());
						lua_setfield(L, t, "ToCityName");
						CvLuaCity::Push(L, pDestCity);
						lua_setfield(L, t, "ToCity");
						lua_pushinteger(L, pkPlayer->GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_GOLD, true));
						lua_setfield(L, t, "FromGPT");
						lua_pushinteger(L, GET_PLAYER(eOtherPlayer).GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_GOLD, false));
						lua_setfield(L, t, "ToGPT");
						lua_pushinteger(L, GET_PLAYER(eOtherPlayer).GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_FOOD, false));
						lua_setfield(L, t, "ToFood");
						lua_pushinteger(L, GET_PLAYER(eOtherPlayer).GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_PRODUCTION, false));
						lua_setfield(L, t, "ToProduction");
						lua_pushinteger(L,  GET_PLAYER(eOtherPlayer).GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_SCIENCE, true));
						lua_setfield(L, t, "FromScience");
						lua_pushinteger(L, GET_PLAYER(eOtherPlayer).GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_SCIENCE, false));
						lua_setfield(L, t, "ToScience");
						lua_pushinteger(L, GET_PLAYER(eOtherPlayer).GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_CULTURE, true));
						lua_setfield(L, t, "FromCulture");
						lua_pushinteger(L, GET_PLAYER(eOtherPlayer).GetTrade()->GetTradeConnectionValueTimes100(kConnection, YIELD_CULTURE, false));
						lua_setfield(L, t, "ToCulture");

						ReligionTypes eToReligion = NO_RELIGION;
						int iToPressure = 0;
						ReligionTypes eFromReligion = NO_RELIGION;
						int iFromPressure = 0;

						pOriginCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pDestCity, eToReligion, iToPressure);
						pDestCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pOriginCity, eFromReligion, iFromPressure);

						if (iTurnsLeft < 0)
						{
							int iTradeReligionModifer = pkPlayer->GetPlayerTraits()->GetTradeReligionModifier();
#if defined(MOD_BALANCE_CORE_POLICIES)
							iTradeReligionModifer += pkPlayer->GetTradeReligionModifier();
#endif
							if (iTradeReligionModifer != 0)
							{
								iToPressure *= 100 + iTradeReligionModifer;
								iToPressure /= 100;
							}
						}

						// Internally pressure is now 10 times greater than what is shown to user
						iToPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();
						iFromPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();

						lua_pushinteger(L, eToReligion);
						lua_setfield(L, t, "ToReligion");
						lua_pushinteger(L, iToPressure);
						lua_setfield(L, t, "ToPressure");
						lua_pushinteger(L, eFromReligion);
						lua_setfield(L, t, "FromReligion");
						lua_pushinteger(L, iFromPressure);
						lua_setfield(L, t, "FromPressure");

						int iToDelta = (pOriginCity->GetBaseTourism() / 100) * pOriginCity->GetCityCulture()->GetTourismMultiplier(eOtherPlayer, true, true, false, true, true);
						int iFromDelta = (pDestCity->GetBaseTourism() / 100) * pDestCity->GetCityCulture()->GetTourismMultiplier(pkPlayer->GetID(), true, true, false, true, true);

						lua_pushinteger(L, iFromDelta);
						lua_setfield(L, t, "FromTourism");
						lua_pushinteger(L, iToDelta);
						lua_setfield(L, t, "ToTourism");

						lua_pushinteger(L, iTurnsLeft);
						lua_setfield(L, t, "TurnsLeft");

						lua_rawseti(L, -2, index++);


					}
				}
			}
		}
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeRoutesToYou(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_createtable(L, 0, 0);
	int index = 1;

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));
		// internal trade route. Ignore.
		if (pConnection->m_eOriginOwner == pConnection->m_eDestOwner)
		{
			continue;
		}

		// trade route does not involve target player. Ignore.
		if (pConnection->m_eDestOwner != pkPlayer->GetID())
		{
			continue;
		}

		CvCity* pFromCity = GC.getMap().plot(pConnection->m_iOriginX, pConnection->m_iOriginY)->getPlotCity();
		CvCity* pToCity = GC.getMap().plot(pConnection->m_iDestX, pConnection->m_iDestY)->getPlotCity();

		CvPlayer* pFromPlayer = &GET_PLAYER(pFromCity->getOwner());
		CvPlayer* pToPlayer = &GET_PLAYER(pToCity->getOwner());

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lua_pushinteger(L, pConnection->m_eConnectionType);
		lua_setfield(L, t, "TradeConnectionType");
		lua_pushinteger(L, pConnection->m_eDomain);
		lua_setfield(L, t, "Domain");
		lua_pushinteger(L, pkPlayer->getCivilizationType());
		lua_setfield(L, t, "FromCivilizationType");
		lua_pushinteger(L , pFromCity->getOwner());
		lua_setfield(L, t, "FromID");
		lua_pushstring(L, pFromCity->getName());
		lua_setfield(L, t, "FromCityName");
		CvLuaCity::Push(L, pFromCity);
		lua_setfield(L, t, "FromCity");
		lua_pushinteger(L, GET_PLAYER(pConnection->m_eDestOwner).getCivilizationType());
		lua_setfield(L, t, "ToCivilizationType");
		lua_pushinteger(L, pToCity->getOwner());
		lua_setfield(L, t, "ToID");
		lua_pushstring(L, pToCity->getName());
		lua_setfield(L, t, "ToCityName");
		CvLuaCity::Push(L, pToCity);
		lua_setfield(L, t, "ToCity");
		lua_pushinteger(L, pFromPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_GOLD, true));
		lua_setfield(L, t, "FromGPT");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_GOLD, false));
		lua_setfield(L, t, "ToGPT");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_FOOD, false));
		lua_setfield(L, t, "ToFood");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_PRODUCTION, false));
		lua_setfield(L, t, "ToProduction");
		lua_pushinteger(L, pFromPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_SCIENCE, true));
		lua_setfield(L, t, "FromScience");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_SCIENCE, false));
		lua_setfield(L, t, "ToScience");
		lua_pushinteger(L, pFromPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_CULTURE, true));
		lua_setfield(L, t, "FromCulture");
		lua_pushinteger(L, pToPlayer->GetTrade()->GetTradeConnectionValueTimes100(*pConnection, YIELD_CULTURE, false));
		lua_setfield(L, t, "ToCulture");

		ReligionTypes eToReligion = NO_RELIGION;
		int iToPressure = 0;
		ReligionTypes eFromReligion = NO_RELIGION;
		int iFromPressure = 0;

		pFromCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pToCity, eToReligion, iToPressure);
		pToCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pFromCity, eFromReligion, iFromPressure);

		// Internally pressure is now 10 times greater than what is shown to user
		iToPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();
		iFromPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();

		lua_pushinteger(L, eToReligion);
		lua_setfield(L, t, "ToReligion");
		lua_pushinteger(L, iToPressure);
		lua_setfield(L, t, "ToPressure");
		lua_pushinteger(L, eFromReligion);
		lua_setfield(L, t, "FromReligion");
		lua_pushinteger(L, iFromPressure);
		lua_setfield(L, t, "FromPressure");

		int iToDelta = (pFromCity->GetBaseTourism() / 100) * pFromCity->GetCityCulture()->GetTourismMultiplier(pToPlayer->GetID(), true, true, false, true, true);
		int iFromDelta = (pToCity->GetBaseTourism() / 100) * pToCity->GetCityCulture()->GetTourismMultiplier(pkPlayer->GetID(), true, true, false, true, true);

		lua_pushinteger(L, iFromDelta);
		lua_setfield(L, t, "FromTourism");
		lua_pushinteger(L, iToDelta);
		lua_setfield(L, t, "ToTourism");

		lua_pushinteger(L, GC.getGame().getGameTurn() - pConnection->m_iTurnRouteComplete);
		lua_setfield(L, t, "TurnsLeft");

		lua_rawseti(L, -2, index++);
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumTechDifference(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, GC.getGame().GetGameTrade()->GetTechDifference(pkPlayer->GetID(), eOtherPlayer));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumPolicyDifference(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, GC.getGame().GetGameTrade()->GetPolicyDifference(pkPlayer->GetID(), eOtherPlayer));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGreatWorks(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	GreatWorkClass eGreatWorkClass = (GreatWorkClass)lua_tointeger(L, 2);

	lua_createtable(L, 0, 0);
	int index = 1;

	int iCityLoop;
	CvCity* pCity = NULL;
	for (pCity = pkPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = pkPlayer->nextCity(&iCityLoop))
	{
		for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			const CvCivilizationInfo& playerCivilizationInfo = pkPlayer->getCivilizationInfo();
			BuildingTypes eBuilding = NO_BUILDING;

			if (MOD_BUILDINGS_THOROUGH_PREREQUISITES || pkPlayer->GetPlayerTraits()->IsKeepConqueredBuildings())
			{
				eBuilding = pCity->GetCityBuildings()->GetBuildingTypeFromClass((BuildingClassTypes)iBuildingClassLoop);
			}
			else
			{
				eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
			}

			if (eBuilding != NO_BUILDING)
			{
				CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
				if (pkBuilding)
				{
					if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						int iNumSlots = pkBuilding->GetGreatWorkCount();
						for (int iI = 0; iI < iNumSlots; iI++)
						{
							int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
							if (iGreatWorkIndex != -1)
							{
								if (GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex].m_eClassType == eGreatWorkClass)
								{
									lua_createtable(L, 0, 0);
									const int t = lua_gettop(L);
									lua_pushinteger(L, iGreatWorkIndex);
									lua_setfield(L, t, "Index");
									lua_pushinteger(L, GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex].m_ePlayer);
									lua_setfield(L, t, "Creator");
									int iEra = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex].m_eEra;
									lua_pushinteger(L, iEra);
									lua_setfield(L, t, "Era");
									lua_rawseti(L, -2, index++);
								}
							}
						}
					}
				}
			}
		}
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetOthersGreatWorks(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_createtable(L, 0, 0);
	int index = 1;

	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes eOtherPlayer = (PlayerTypes)uiPlayer;

		// skip if me
		if (eOtherPlayer == pkPlayer->GetID())
		{
			continue;
		}

		// skip if dead
		if (!GET_PLAYER(eOtherPlayer).isAlive())
		{
			continue;
		}

		// skip if at war
		if (GET_TEAM(pkPlayer->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			continue;
		}

		// skip if have not met
		if (!GET_TEAM(pkPlayer->getTeam()).isHasMet(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			continue;
		}

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lua_pushinteger(L, eOtherPlayer);
		lua_setfield(L, t, "iPlayer");

		// writing
		int iWritingWorkIndex = GET_PLAYER(eOtherPlayer).GetCulture()->GetSwappableWritingIndex();
		PlayerTypes eWritingWorkPlayer = NO_PLAYER;
		EraTypes eWritingWorkEra = NO_ERA;
		lua_pushinteger(L, iWritingWorkIndex);
		lua_setfield(L, t, "WritingIndex");
		if (iWritingWorkIndex != -1)
		{
			eWritingWorkPlayer = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iWritingWorkIndex].m_ePlayer;
			eWritingWorkEra = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iWritingWorkIndex].m_eEra;
		}
		lua_pushinteger(L, eWritingWorkPlayer);
		lua_setfield(L, t, "WritingCreator");
		lua_pushinteger(L, eWritingWorkEra);
		lua_setfield(L, t, "WritingEra");

		// art
		int iArtWorkIndex = GET_PLAYER(eOtherPlayer).GetCulture()->GetSwappableArtIndex();
		PlayerTypes eArtWorkPlayer = NO_PLAYER;
		EraTypes eArtWorkEra = NO_ERA;
		lua_pushinteger(L, iArtWorkIndex);
		lua_setfield(L, t, "ArtIndex");
		if (iArtWorkIndex != -1)
		{
			eArtWorkPlayer = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iArtWorkIndex].m_ePlayer;
			eArtWorkEra = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iArtWorkIndex].m_eEra;
		}
		lua_pushinteger(L, eArtWorkPlayer);
		lua_setfield(L, t, "ArtCreator");
		lua_pushinteger(L, eArtWorkEra);
		lua_setfield(L, t, "ArtEra");

		// artifact
		int iArtifactWorkIndex = GET_PLAYER(eOtherPlayer).GetCulture()->GetSwappableArtifactIndex();
		PlayerTypes eArtifactWorkPlayer = NO_PLAYER;
		EraTypes eArtifactWorkEra = NO_ERA;
		lua_pushinteger(L, iArtifactWorkIndex);
		lua_setfield(L, t, "ArtifactIndex");
		if (iArtifactWorkIndex != -1)
		{
			eArtifactWorkPlayer = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iArtifactWorkIndex].m_ePlayer;
			eArtifactWorkEra = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iArtifactWorkIndex].m_eEra;
		}
		lua_pushinteger(L, eArtifactWorkPlayer);
		lua_setfield(L, t, "ArtifactCreator");
		lua_pushinteger(L, eArtifactWorkEra);
		lua_setfield(L, t, "ArtifactEra");

		// music
		int iMusicWorkIndex = GET_PLAYER(eOtherPlayer).GetCulture()->GetSwappableMusicIndex();
		PlayerTypes eMusicWorkPlayer = NO_PLAYER;
		EraTypes eMusicWorkEra = NO_ERA;
		lua_pushinteger(L, iMusicWorkIndex);
		lua_setfield(L, t, "MusicIndex");
		if (iMusicWorkIndex != -1)
		{
			eMusicWorkPlayer = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iMusicWorkIndex].m_ePlayer;
			eMusicWorkEra = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iMusicWorkIndex].m_eEra;
		}
		lua_pushinteger(L, eMusicWorkPlayer);
		lua_setfield(L, t, "MusicCreator");
		lua_pushinteger(L, eMusicWorkEra);
		lua_setfield(L, t, "MusicEra");

		lua_rawseti(L, -2, index++);
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetSwappableGreatWriting (lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetCulture()->GetSwappableWritingIndex());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetSwappableGreatArt (lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetCulture()->GetSwappableArtIndex());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetSwappableGreatArtifact (lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetCulture()->GetSwappableArtifactIndex());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetSwappableGreatMusic (lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetCulture()->GetSwappableMusicIndex());
	return 1;
}

//------------------------------------------------------------------------------
//int GetHappinessFromMinorCivs() const;
int CvLuaPlayer::lGetHappinessFromMinorCivs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromMinorCivs);
}
//------------------------------------------------------------------------------
//int GetHappinessFromMinor(PlayerTypes eMinor) const;
int CvLuaPlayer::lGetHappinessFromMinor(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromMinor);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetBarbarianCombatBonus(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetBarbarianCombatBonus);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetBarbarianCombatBonus(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetBarbarianCombatBonus);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeBarbarianCombatBonus(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeBarbarianCombatBonus);
}
//------------------------------------------------------------------------------
#if defined(MOD_BALANCE_CORE)
int CvLuaPlayer::lGetCombatBonusVsHigherPop(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetCombatBonusVsHigherPop());
	}
	return 1;
}
int CvLuaPlayer::lGetWarScore(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetDiplomacyAI()->GetWarScore(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetPlayerMilitaryStrengthComparedToUs(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetPlayerEconomicStrengthComparedToUs(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetDiplomacyAI()->GetPlayerEconomicStrengthComparedToUs(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetWarDamageValue(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	int iOurWarDamage = pkPlayer->GetDiplomacyAI()->GetWarDamageValue(ePlayer);
	lua_pushinteger(L, iOurWarDamage);
	return 1;
}
int CvLuaPlayer::lIsWantsPeaceWithPlayer(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const bool bResult = pkPlayer->GetDiplomacyAI()->IsWantsPeaceWithPlayer(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lGetTreatyWillingToOffer(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetDiplomacyAI()->GetTreatyWillingToOffer(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}

// void DoUpdateWarDamageLevel();
int CvLuaPlayer::lDoUpdateWarDamage(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	pkPlayer->GetDiplomacyAI()->DoUpdateWarDamage();
	return 0;
}

// void DoUpdatePeaceTreatyWillingness();
int CvLuaPlayer::lDoUpdatePeaceTreatyWillingness(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	pkPlayer->GetDiplomacyAI()->DoUpdatePeaceTreatyWillingness();
	return 0;
}

int CvLuaPlayer::lGetDominationResistance(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	int iResistancePower = pkPlayer->GetDominationResistance(ePlayer);
	lua_pushinteger(L, iResistancePower);

	return 1;
}

#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCombatBonusVsHigherTech(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetCombatBonusVsHigherTech());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCombatBonusVsLargerCiv(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetCombatBonusVsLargerCiv());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAlwaysSeeBarbCamps(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsAlwaysSeeBarbCamps);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetAlwaysSeeBarbCampsCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetAlwaysSeeBarbCampsCount);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeAlwaysSeeBarbCampsCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeAlwaysSeeBarbCampsCount);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGarrisonedCityRangeStrikeModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetGarrisonedCityRangeStrikeModifier);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeGarrisonedCityRangeStrikeModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeGarrisonedCityRangeStrikeModifier);
}

//------------------------------------------------------------------------------
//bool IsPolicyBlocked();
int CvLuaPlayer::lIsPolicyBlocked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyTypes ePolicy = (PolicyTypes)lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetPlayerPolicies()->IsPolicyBlocked(ePolicy);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool IsPolicyBranchBlocked();
int CvLuaPlayer::lIsPolicyBranchBlocked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes ePolicyBranch = (PolicyBranchTypes)lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetPlayerPolicies()->IsPolicyBranchBlocked(ePolicyBranch);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool IsPolicyBranchUnlocked();
int CvLuaPlayer::lIsPolicyBranchUnlocked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes eBranchType = (PolicyBranchTypes)lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranchType);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//void SetPolicyBranchUnlocked();
int CvLuaPlayer::lSetPolicyBranchUnlocked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes eBranchType = (PolicyBranchTypes)lua_tointeger(L, 2);
	const bool bNewValue = lua_toboolean(L, 3);
	const bool bRevolution = lua_toboolean(L, 4);

	pkPlayer->GetPlayerPolicies()->SetPolicyBranchUnlocked(eBranchType, bNewValue, bRevolution);
	return 1;
}

//------------------------------------------------------------------------------
//int GetNumPolicyBranchesUnlocked();
int CvLuaPlayer::lGetNumPolicyBranchesUnlocked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPolicyBranchesUnlocked();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetNumPolicyBranchesAllowed();
int CvLuaPlayer::lGetNumPolicyBranchesAllowed(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPolicyBranchesAllowed();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool GetPolicyBranchChosen(PolicyTypes  iIndex);
int CvLuaPlayer::lGetPolicyBranchChosen(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iID = lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetPlayerPolicies()->GetPolicyBranchChosen(iID);
	lua_pushinteger(L, iResult);
	return 1;
}


//------------------------------------------------------------------------------
//int GetNumPolicies();
int CvLuaPlayer::lGetNumPolicies(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
#if defined(MOD_BALANCE_CORE)
	bool bIgnoreFinishers = luaL_optbool(L, 2, false);
	bool bIgnoreDummy = luaL_optbool(L, 3, false);
	if(pkPlayer->GetPlayerPolicies())
	{
		const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPoliciesOwned(bIgnoreFinishers, bIgnoreDummy);
#else
		const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPoliciesOwned();
#endif
		lua_pushinteger(L, iResult);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
//int GetNumPolicies();
int CvLuaPlayer::lGetNumPoliciesInBranch(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes eIndex = (PolicyBranchTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPoliciesOwnedInBranch(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetNumPolicies();
int CvLuaPlayer::lGetNumPoliciesPurchasedInBranch(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes eIndex = (PolicyBranchTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPoliciesPurchasedInBranch(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool hasPolicy(PolicyTypes  iIndex);
int CvLuaPlayer::lHasPolicy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyTypes iIndex = (PolicyTypes)lua_tointeger(L, 2);

	const bool bResult
	    = pkPlayer->GetPlayerPolicies()->HasPolicy(iIndex);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setHasPolicy(PolicyTypes  eIndex, bool bNewValue);
int CvLuaPlayer::lSetHasPolicy(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setHasPolicy);
}
//------------------------------------------------------------------------------
//int getNextPolicyCost();
int CvLuaPlayer::lGetNextPolicyCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNextPolicyCost);
}
//------------------------------------------------------------------------------
//bool canAdoptPolicy(PolicyTypes  iIndex);
int CvLuaPlayer::lCanAdoptPolicy(lua_State* L)
{
	const PolicyTypes ePolicy = static_cast<PolicyTypes>(luaL_checkinteger(L, 2));
	bool bIgnoreCost = luaL_optbool(L, 3, false);

	CvPlayerAI* pkPlayer = GetInstance(L);
	CvAssert(pkPlayer != NULL);
	if(pkPlayer != NULL)
	{
		CvPlayerPolicies* pkPolicies = pkPlayer->GetPlayerPolicies();
		CvAssert(pkPolicies != NULL);
		if(pkPolicies != NULL)
		{
			bool bResult = pkPolicies->CanAdoptPolicy(ePolicy, bIgnoreCost);
			lua_pushboolean(L, bResult);
			return 1;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
//void doAdoptPolicy(PolicyTypes  eIndex);
int CvLuaPlayer::lDoAdoptPolicy(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::doAdoptPolicy);
}


//------------------------------------------------------------------------------
//bool CanUnlockPolicyBranch(PolicyBranchTypes  iIndex);
int CvLuaPlayer::lCanUnlockPolicyBranch(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes iIndex = (PolicyBranchTypes)lua_tointeger(L, 2);

	const bool bResult
	    = pkPlayer->GetPlayerPolicies()->CanUnlockPolicyBranch(iIndex);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool GetDominantPolicyBranchForTitle();
int CvLuaPlayer::lGetDominantPolicyBranchForTitle(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerPolicies()->GetDominantPolicyBranchForTitle();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//PolicyBranchTypes GetLateGamePolicyTree() const;
int CvLuaPlayer::lGetLateGamePolicyTree(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool GetBranchPicked1();
int CvLuaPlayer::lGetBranchPicked1(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerPolicies()->GetBranchPicked1();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool GetBranchPicked2();
int CvLuaPlayer::lGetBranchPicked2(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerPolicies()->GetBranchPicked2();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool GetBranchPicked3();
int CvLuaPlayer::lGetBranchPicked3(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerPolicies()->GetBranchPicked3();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//void GrantPolicy(PolicyTypes iPolicy, bool bFree=false);
int CvLuaPlayer::lGrantPolicy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyTypes iPolicy = (PolicyTypes)lua_tointeger(L, 2);
	bool bFree = luaL_optbool(L, 3, false);

	const bool bResult = pkPlayer->grantPolicy(iPolicy, bFree);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//void RevokePolicy(PolicyTypes iPolicy);
int CvLuaPlayer::lRevokePolicy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyTypes iPolicy = (PolicyTypes)lua_tointeger(L, 2);

	const bool bResult = pkPlayer->revokePolicy(iPolicy);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//void SwapPolicy(PolicyTypes iNewPolicy, PolicyTypes iOldPolicy);
int CvLuaPlayer::lSwapPolicy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyTypes iPolicyNew = (PolicyTypes)lua_tointeger(L, 2);
	const PolicyTypes iPolicyOld = (PolicyTypes)lua_tointeger(L, 3);

	const bool bResult = pkPlayer->swapPolicy(iPolicyNew, iPolicyOld);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanAdoptIdeology(PolicyBranchTypes  iIndex);
int CvLuaPlayer::lCanAdoptIdeology(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes iIndex = (PolicyBranchTypes)lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetPlayerPolicies()->CanAdoptIdeology(iIndex);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool CanAdoptTenet();
int CvLuaPlayer::lCanAdoptTenet(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyTypes ePolicy = static_cast<PolicyTypes>(luaL_checkinteger(L, 2));
	bool bIgnoreCost = luaL_optbool(L, 3, false);

	bool bResult = false;

	CvPlayerPolicies* pkPolicies = pkPlayer->GetPlayerPolicies();
	CvAssert(pkPolicies != NULL);
	if(pkPolicies != NULL)
	{
		bResult = pkPolicies->CanAdoptPolicy(ePolicy, bIgnoreCost);
	}

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetPolicyCatchSpiesModifier();
int CvLuaPlayer::lGetPolicyCatchSpiesModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetNumPolicyBranchesFinished();
int CvLuaPlayer::lGetNumPolicyBranchesFinished(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPolicyBranchesFinished();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int IsPolicyBranchFinished();
int CvLuaPlayer::lIsPolicyBranchFinished(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iID = lua_tointeger(L, 2);

	const bool iResult = pkPlayer->GetPlayerPolicies()->IsPolicyBranchFinished(PolicyBranchTypes(iID));
	lua_pushboolean(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//<list of PolicyTypes> GetAvailableTenets(int iLevel);
int CvLuaPlayer::lGetAvailableTenets(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iLevel = lua_tointeger(L, 2);

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	PolicyBranchTypes eBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	if (eBranch != NO_POLICY_BRANCH_TYPE)
	{
		std::vector<PolicyTypes> availableTenets = pkPlayer->GetPlayerPolicies()->GetAvailableTenets(eBranch, iLevel);
		for(std::vector<PolicyTypes>::iterator it = availableTenets.begin();
			it!= availableTenets.end(); ++it)
		{
			const PolicyTypes ePolicy = (*it);
			lua_pushinteger(L, ePolicy);
			lua_rawseti(L, t, idx++);
		}
	}

	return 1;
}

//------------------------------------------------------------------------------
//int GetTenet(PolicyBranchTypes eBranch, int iLevel, int iIndex);
int CvLuaPlayer::lGetTenet(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PolicyBranchTypes eBranch = (PolicyBranchTypes)lua_tointeger(L, 2);
	const int iLevel = lua_tointeger(L, 3);
	const int iIndex = lua_tointeger(L, 4);

	const int iResult = pkPlayer->GetPlayerPolicies()->GetTenet(eBranch, iLevel, iIndex);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int IsAnarchy();
int CvLuaPlayer::lIsAnarchy(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsAnarchy);
}
//------------------------------------------------------------------------------
//int GetAnarchyNumTurns();
int CvLuaPlayer::lGetAnarchyNumTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetAnarchyNumTurns);
}
//------------------------------------------------------------------------------
//int SetAnarchyNumTurns();
int CvLuaPlayer::lSetAnarchyNumTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetAnarchyNumTurns);
}
//------------------------------------------------------------------------------
//int ChangeAnarchyNumTurns();
int CvLuaPlayer::lChangeAnarchyNumTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeAnarchyNumTurns);
}

//------------------------------------------------------------------------------
//int getAdvancedStartPoints();
int CvLuaPlayer::lGetAdvancedStartPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartPoints);
}
//------------------------------------------------------------------------------
//void setAdvancedStartPoints(int iNewValue);
int CvLuaPlayer::lSetAdvancedStartPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setAdvancedStartPoints);
}
//------------------------------------------------------------------------------
//void changeAdvancedStartPoints(int iChange);
int CvLuaPlayer::lChangeAdvancedStartPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeAdvancedStartPoints);
}
//------------------------------------------------------------------------------
//int getAdvancedStartUnitCost(UnitTypes  eUnit, bool bAdd, CyPlot* pPlot);
int CvLuaPlayer::lGetAdvancedStartUnitCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartUnitCost);
}
//------------------------------------------------------------------------------
//int getAdvancedStartCityCost(bool bAdd, CyPlot* pPlot);
int CvLuaPlayer::lGetAdvancedStartCityCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartCityCost);
}
//------------------------------------------------------------------------------
//int getAdvancedStartPopCost(bool bAdd, CyCity* pCity);
int CvLuaPlayer::lGetAdvancedStartPopCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartPopCost);
}
//------------------------------------------------------------------------------
//int getAdvancedStartBuildingCost(BuildingTypes  eBuilding, bool bAdd, CyCity* pCity);
int CvLuaPlayer::lGetAdvancedStartBuildingCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartBuildingCost);
}
//------------------------------------------------------------------------------
//int getAdvancedStartImprovementCost(ImprovementTypes  eImprovement, bool bAdd, CyPlot* pPlot);
int CvLuaPlayer::lGetAdvancedStartImprovementCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartImprovementCost);
}
//------------------------------------------------------------------------------
//int getAdvancedStartRouteCost(RouteTypes  eRoute, bool bAdd, CyPlot* pPlot);
int CvLuaPlayer::lGetAdvancedStartRouteCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartRouteCost);
}
//------------------------------------------------------------------------------
//int getAdvancedStartTechCost(TechTypes  eTech, bool bAdd);
int CvLuaPlayer::lGetAdvancedStartTechCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartTechCost);
}
//------------------------------------------------------------------------------
//int getAdvancedStartVisibilityCost(bool bAdd, CyPlot* pPlot);
int CvLuaPlayer::lGetAdvancedStartVisibilityCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAdvancedStartVisibilityCost);
}

//------------------------------------------------------------------------------
//int GetAttackBonusTurns();
int CvLuaPlayer::lGetAttackBonusTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetAttackBonusTurns);
}

//------------------------------------------------------------------------------
//int GetCultureBonusTurns();
int CvLuaPlayer::lGetCultureBonusTurns(lua_State* L)
{
#if defined(MOD_BALANCE_CORE)
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = (pkPlayer->GetCultureBonusTurns() + pkPlayer->GetCultureBonusTurnsConquest()); 
	lua_pushinteger(L, iResult);
	return 1;
#else
	return BasicLuaMethod(L, &CvPlayerAI::GetCultureBonusTurns);
#endif
}

//------------------------------------------------------------------------------
//int GetTourismBonusTurns();
int CvLuaPlayer::lGetTourismBonusTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetTourismBonusTurns);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeProgressThreshold(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetGoldenAgeProgressThreshold);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeProgressMeter(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetGoldenAgeProgressMeter);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lSetGoldenAgeProgressMeter(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetGoldenAgeProgressMeter);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeGoldenAgeProgressMeter(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeGoldenAgeProgressMeter);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumGoldenAges(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumGoldenAges);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lSetNumGoldenAges(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetNumGoldenAges);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeNumGoldenAges(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeNumGoldenAges);
}

//------------------------------------------------------------------------------
//int getGoldenAgeTurns();
int CvLuaPlayer::lGetGoldenAgeTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGoldenAgeTurns);
}
//------------------------------------------------------------------------------
//int getGoldenAgeLength();
int CvLuaPlayer::lGetGoldenAgeLength(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGoldenAgeLength);
}
//------------------------------------------------------------------------------
//bool isGoldenAge();
int CvLuaPlayer::lIsGoldenAge(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isGoldenAge);
}
//------------------------------------------------------------------------------
//void changeGoldenAgeTurns(int iChange);
int CvLuaPlayer::lChangeGoldenAgeTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeGoldenAgeTurns);
}
//------------------------------------------------------------------------------
//int getNumUnitGoldenAges();
int CvLuaPlayer::lGetNumUnitGoldenAges(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumUnitGoldenAges);
}
//------------------------------------------------------------------------------
//void changeNumUnitGoldenAges(int iChange);
int CvLuaPlayer::lChangeNumUnitGoldenAges(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeNumUnitGoldenAges);
}
//------------------------------------------------------------------------------
//int getStrikeTurns();
int CvLuaPlayer::lGetStrikeTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getStrikeTurns);
}
//------------------------------------------------------------------------------
//int getGoldenAgeModifier();
int CvLuaPlayer::lGetGoldenAgeModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGoldenAgeModifier);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeTourismModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeTourismModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeGreatWriterRateModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeGreatArtistRateModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeGreatMusicianRateModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeGreatScientistRateModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	GreatPersonTypes eGreatPerson = GetGreatPersonFromUnitClass((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_SCIENTIST"));
	int iResult = 0;
	if (eGreatPerson != NO_GREATPERSON)
	{
		iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeGreatEngineerRateModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	GreatPersonTypes eGreatPerson = GetGreatPersonFromUnitClass((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_ENGINEER"));
	int iResult = 0;
	if (eGreatPerson != NO_GREATPERSON)
	{
		iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeGreatMerchantRateModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	GreatPersonTypes eGreatPerson = GetGreatPersonFromUnitClass((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MERCHANT"));
	int iResult = 0;
	if (eGreatPerson != NO_GREATPERSON)
	{
		iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
	}
	lua_pushinteger(L, iResult);
	return 1;
}

#endif
#if defined(MOD_DIPLOMACY_CITYSTATES) && defined(MOD_API_UNIFIED_YIELDS)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetGoldenAgeGreatDiplomatRateModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	GreatPersonTypes eGreatPerson = GetGreatPersonFromUnitClass((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"));
	int iResult = 0;
	if (eGreatPerson != NO_GREATPERSON)
	{
		iResult = pkPlayer->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
	}
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int getHurryModifier(HurryTypes  eHurry);
int CvLuaPlayer::lGetHurryModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getHurryModifier);
}
//------------------------------------------------------------------------------
//void CreateGreatGeneral(int eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience, int iX, int iY);
int CvLuaPlayer::lCreateGreatGeneral(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eGreatPersonUnit = (UnitTypes)lua_tointeger(L, 2);
	const int x = lua_tointeger(L, 3);
	const int y = lua_tointeger(L, 4);

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	const bool bIsFree = luaL_optint(L, 5, 0);
	pkPlayer->createGreatGeneral(eGreatPersonUnit, x, y, bIsFree);
#else
	pkPlayer->createGreatGeneral(eGreatPersonUnit, x, y);
#endif
	return 0;
}
//------------------------------------------------------------------------------
//int getGreatPeopleCreated();
int CvLuaPlayer::lGetGreatPeopleCreated(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatPeopleCreated);
}
//------------------------------------------------------------------------------
//int getGreatGeneralsCreated();
int CvLuaPlayer::lGetGreatGeneralsCreated(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatGeneralsCreated);
}
//------------------------------------------------------------------------------
//int getGreatPeopleThresholdModifier();
int CvLuaPlayer::lGetGreatPeopleThresholdModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatPeopleThresholdModifier);
}
//------------------------------------------------------------------------------
//int getGreatGeneralsThresholdModifier();
int CvLuaPlayer::lGetGreatGeneralsThresholdModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatGeneralsThresholdModifier);
}
//------------------------------------------------------------------------------
//int getGreatAdmiralsThresholdModifier();
int CvLuaPlayer::lGetGreatAdmiralsThresholdModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatAdmiralsThresholdModifier);
}
//------------------------------------------------------------------------------
//int getGreatPeopleRateModifier();
int CvLuaPlayer::lGetGreatPeopleRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatPeopleRateModifier);
}
//------------------------------------------------------------------------------
//int getGreatGeneralRateModifier();
int CvLuaPlayer::lGetGreatGeneralRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatGeneralRateModifier);
}
//------------------------------------------------------------------------------
//int getDomesticGreatGeneralRateModifier();
int CvLuaPlayer::lGetDomesticGreatGeneralRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getDomesticGreatGeneralRateModifier);
}

//------------------------------------------------------------------------------
//int getGreatWriterRateModifier();
int CvLuaPlayer::lGetGreatWriterRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatWriterRateModifier);
}

//------------------------------------------------------------------------------
//int getGreatArtistRateModifier();
int CvLuaPlayer::lGetGreatArtistRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatArtistRateModifier);
}

//------------------------------------------------------------------------------
//int getGreatMusicianRateModifier();
int CvLuaPlayer::lGetGreatMusicianRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatMusicianRateModifier);
}

//------------------------------------------------------------------------------
//int getGreatScientistRateModifier();
int CvLuaPlayer::lGetGreatScientistRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatScientistRateModifier);
}

//------------------------------------------------------------------------------
//int getGreatMerchantRateModifier();
int CvLuaPlayer::lGetGreatMerchantRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatMerchantRateModifier);
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
//------------------------------------------------------------------------------
//int getGreatDiplomatRateModifier();
int CvLuaPlayer::lGetGreatDiplomatRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatDiplomatRateModifier);
}
//------------------------------------------------------------------------------
//int GetScienceRateFromMinorAllies();
int CvLuaPlayer::lGetScienceRateFromMinorAllies(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceRateFromMinorAllies);
}
//------------------------------------------------------------------------------
//int GetScienceRateFromLeagueAid();
int CvLuaPlayer::lGetScienceRateFromLeagueAid(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceRateFromLeagueAid);
}
//int GetLeagueCultureCityModifier();
int CvLuaPlayer::lGetLeagueCultureCityModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetLeagueCultureCityModifier);
}
#endif
//int GetArtsyGreatPersonRateModifier();
int CvLuaPlayer::lGetArtsyGreatPersonRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getArtsyGreatPersonRateModifier);
}
//int GetScienceyGreatPersonRateModifier();
int CvLuaPlayer::lGetScienceyGreatPersonRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getScienceyGreatPersonRateModifier);
}
//------------------------------------------------------------------------------
//int getGreatEngineerRateModifier();
int CvLuaPlayer::lGetGreatEngineerRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGreatEngineerRateModifier);
}

//------------------------------------------------------------------------------
//int GetPolicyGreatPeopleRateModifier();
int CvLuaPlayer::lGetPolicyGreatPeopleRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_PERSON_RATE));
	}
	return 1;
}

//------------------------------------------------------------------------------
//int GetPolicyGreatWriterRateModifier();
int CvLuaPlayer::lGetPolicyGreatWriterRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_WRITER_RATE));
	}
	return 1;
}

//------------------------------------------------------------------------------
//int GetPolicyGreatArtistRateModifier();
int CvLuaPlayer::lGetPolicyGreatArtistRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_ARTIST_RATE));
	}
	return 1;
}

//------------------------------------------------------------------------------
//int GetPolicyGreatMusicianRateModifier();
int CvLuaPlayer::lGetPolicyGreatMusicianRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_MUSICIAN_RATE));
	}
	return 1;
}

//------------------------------------------------------------------------------
//int GetPolicyGreatScientistRateModifier();
int CvLuaPlayer::lGetPolicyGreatScientistRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_SCIENTIST_RATE));
	}
	return 1;
}

//------------------------------------------------------------------------------
//int GetPolicyGreatMerchantRateModifier();
int CvLuaPlayer::lGetPolicyGreatMerchantRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_MERCHANT_RATE));
	}
	return 1;
}
#if defined(MOD_DIPLOMACY_CITYSTATES)
//------------------------------------------------------------------------------
//int GetPolicyGreatDiplomatRateModifier();
int CvLuaPlayer::lGetPolicyGreatDiplomatRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_DIPLOMAT_RATE));
	return 1;
}
#endif

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
//------------------------------------------------------------------------------
// int getMonopolyGreatPersonRateModifier(SpecialistTypes eSpecialist) const;
int CvLuaPlayer::lGetMonopolyGreatPersonRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	SpecialistTypes eSpecialist = (SpecialistTypes)lua_tointeger(L, 2);
	GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
	int iModifier = 0;
	// Do we get increased great person rate from a resource monopoly?
	if (eGreatPerson != NO_GREATPERSON)
	{
		iModifier = pkPlayer->getSpecificGreatPersonRateModifierFromMonopoly(eGreatPerson);
	}

	lua_pushinteger(L, iModifier);
	return 1;
}

//------------------------------------------------------------------------------
// int getMonopolyGreatPersonRateChange(SpecialistTypes eSpecialist) const;
int CvLuaPlayer::lGetMonopolyGreatPersonRateChange(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	SpecialistTypes eSpecialist = (SpecialistTypes)lua_tointeger(L, 2);
	GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
	int iModifier = 0;
	// Do we get increased great person rate from a resource monopoly?
	if (eGreatPerson != NO_GREATPERSON)
	{
		iModifier = pkPlayer->getSpecificGreatPersonRateChangeFromMonopoly(eGreatPerson);
	}

	lua_pushinteger(L, iModifier);
	return 1;
}
#endif

//------------------------------------------------------------------------------
//int GetPolicyGreatEngineerRateModifier();
int CvLuaPlayer::lGetPolicyGreatEngineerRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GREAT_ENGINEER_RATE));
	}
	return 1;
}

//------------------------------------------------------------------------------
//void GetProductionModifier();
int CvLuaPlayer::lGetProductionModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->getProductionModifier());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetUnitProductionModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)luaL_checkinteger(L, 2);
	lua_pushinteger(L, pkPlayer->getProductionModifier(eUnit));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetBuildingProductionModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes)luaL_checkinteger(L, 2);
	lua_pushinteger(L, pkPlayer->getProductionModifier(eBuilding));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetProjectProductionModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ProjectTypes eProject = (ProjectTypes)luaL_checkinteger(L, 2);
	lua_pushinteger(L, pkPlayer->getProductionModifier(eProject));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetSpecialistProductionModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const SpecialistTypes eSpecialist = (SpecialistTypes)luaL_checkinteger(L, 2);
	lua_pushinteger(L, pkPlayer->getProductionModifier(eSpecialist));
	return 1;
}
//------------------------------------------------------------------------------
//int getMaxGlobalBuildingProductionModifier();
int CvLuaPlayer::lGetMaxGlobalBuildingProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getMaxGlobalBuildingProductionModifier);
}
//------------------------------------------------------------------------------
//int getMaxTeamBuildingProductionModifier();
int CvLuaPlayer::lGetMaxTeamBuildingProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getMaxTeamBuildingProductionModifier);
}
//------------------------------------------------------------------------------
//int getMaxPlayerBuildingProductionModifier();
int CvLuaPlayer::lGetMaxPlayerBuildingProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getMaxPlayerBuildingProductionModifier);
}
//------------------------------------------------------------------------------
//int getFreeExperience();
int CvLuaPlayer::lGetFreeExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getFreeExperience);
}
//------------------------------------------------------------------------------
//int getFeatureProductionModifier();
int CvLuaPlayer::lGetFeatureProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getFeatureProductionModifier);
}
//------------------------------------------------------------------------------
//int getWorkerSpeedModifier();
int CvLuaPlayer::lGetWorkerSpeedModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getWorkerSpeedModifier);
}
#if defined(MOD_CIV6_WORKER)
//------------------------------------------------------------------------------
//int GetImprovementBuilderCost(BuildTypes);
int CvLuaPlayer::lGetImprovementBuilderCost(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const BuildTypes iBuild = (BuildTypes)lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetImprovementBuilderCost(iBuild);
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int getImprovementUpgradeRateModifier();
int CvLuaPlayer::lGetImprovementUpgradeRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getImprovementUpgradeRateModifier);
}
//------------------------------------------------------------------------------
//int getMilitaryProductionModifier();
int CvLuaPlayer::lGetMilitaryProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getMilitaryProductionModifier);
}
//------------------------------------------------------------------------------
//int getSpaceProductionModifier();
int CvLuaPlayer::lGetSpaceProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getSpaceProductionModifier);
}
//------------------------------------------------------------------------------
//int getSettlerProductionModifier();
int CvLuaPlayer::lGetSettlerProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getSettlerProductionModifier);
}
//------------------------------------------------------------------------------
//int getCapitalSettlerProductionModifier();
int CvLuaPlayer::lGetCapitalSettlerProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getCapitalSettlerProductionModifier);
}
//------------------------------------------------------------------------------
//int getWonderProductionModifier();
int CvLuaPlayer::lGetWonderProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getWonderProductionModifier);
}
//------------------------------------------------------------------------------
//int GetUnitProductionMaintenanceMod();
int CvLuaPlayer::lGetUnitProductionMaintenanceMod(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetUnitProductionMaintenanceMod);
}
//------------------------------------------------------------------------------
//int GetNumUnitsSupplied();
int CvLuaPlayer::lGetNumUnitsSupplied(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumUnitsSupplied);
}
//------------------------------------------------------------------------------
//int GetNumUnitsSuppliedByHandicap();
int CvLuaPlayer::lGetNumUnitsSuppliedByHandicap(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetNumUnitsSuppliedByHandicap(true);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumUnitsSuppliedByCities();
int CvLuaPlayer::lGetNumUnitsSuppliedByCities(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetNumUnitsSuppliedByCities(true);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumUnitsSuppliedByPopulation();
int CvLuaPlayer::lGetNumUnitsSuppliedByPopulation(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetNumUnitsSuppliedByPopulation(true);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumUnitsOutOfSupply();
int CvLuaPlayer::lGetNumUnitsOutOfSupply(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumUnitsOutOfSupply);
}
//------------------------------------------------------------------------------
//int getCityDefenseModifier();
int CvLuaPlayer::lGetCityDefenseModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getCityDefenseModifier);
}
//------------------------------------------------------------------------------
//int getNumNukeUnits();
int CvLuaPlayer::lGetNumNukeUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumNukeUnits);
}
//------------------------------------------------------------------------------
//int getNumOutsideUnits();
int CvLuaPlayer::lGetNumOutsideUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumOutsideUnits);
}

//------------------------------------------------------------------------------
//int getGoldPerUnit();
int CvLuaPlayer::lGetGoldPerUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGoldPerUnit);
}
//------------------------------------------------------------------------------
//void ChangeGoldPerUnitTimes100();
int CvLuaPlayer::lChangeGoldPerUnitTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeGoldPerUnitTimes100);
}
//------------------------------------------------------------------------------
//int getGoldPerMilitaryUnit();
int CvLuaPlayer::lGetGoldPerMilitaryUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getGoldPerMilitaryUnit);
}
//------------------------------------------------------------------------------
//int getExtraUnitCost();
int CvLuaPlayer::lGetExtraUnitCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getExtraUnitCost);
}
//------------------------------------------------------------------------------
//int getNumMilitaryUnits();
int CvLuaPlayer::lGetNumMilitaryUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumMilitaryUnits);
}
//------------------------------------------------------------------------------
//int getHappyPerMilitaryUnit();
int CvLuaPlayer::lGetHappyPerMilitaryUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getHappyPerMilitaryUnit);
}
//------------------------------------------------------------------------------
//bool isMilitaryFoodProduction();
int CvLuaPlayer::lIsMilitaryFoodProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isMilitaryFoodProduction);
}
//------------------------------------------------------------------------------
//int getHighestUnitLevel();
int CvLuaPlayer::lGetHighestUnitLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getHighestUnitLevel);
}

//------------------------------------------------------------------------------
//int getConscriptCount();
int CvLuaPlayer::lGetConscriptCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getConscriptCount);
}
//------------------------------------------------------------------------------
//void setConscriptCount(int iNewValue);
int CvLuaPlayer::lSetConscriptCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setConscriptCount);
}
//------------------------------------------------------------------------------
//void changeConscriptCount(int iChange);
int CvLuaPlayer::lChangeConscriptCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeConscriptCount);
}

//------------------------------------------------------------------------------
//int getMaxConscript();
int CvLuaPlayer::lGetMaxConscript(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getMaxConscript);
}
//------------------------------------------------------------------------------
//int getOverflowResearch();
int CvLuaPlayer::lGetOverflowResearch(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getOverflowResearch);
}
//------------------------------------------------------------------------------
//int getOverflowResearch();
int CvLuaPlayer::lSetOverflowResearch(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setOverflowResearch);
}
//------------------------------------------------------------------------------
//int getOverflowResearch();
int CvLuaPlayer::lChangeOverflowResearch(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeOverflowResearch);
}
//------------------------------------------------------------------------------
//bool getExpInBorderModifier();
int CvLuaPlayer::lGetExpInBorderModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getExpInBorderModifier);
}
//------------------------------------------------------------------------------
//int getLevelExperienceModifier();
int CvLuaPlayer::lGetLevelExperienceModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getLevelExperienceModifier);
}
//------------------------------------------------------------------------------
//int getCultureBombTimer();
int CvLuaPlayer::lGetCultureBombTimer(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getCultureBombTimer);
}
//------------------------------------------------------------------------------
//int getConversionTimer();
int CvLuaPlayer::lGetConversionTimer(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getConversionTimer);
}
//------------------------------------------------------------------------------
//CvCity* getCapitalCity()
int CvLuaPlayer::lGetCapitalCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvCity* pkCity = pkPlayer->getCapitalCity();
	CvLuaCity::Push(L, pkCity);
	return 1;
}
//------------------------------------------------------------------------------
//int IsHasLostCapital();
int CvLuaPlayer::lIsHasLostCapital(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsHasLostCapital);
}
//------------------------------------------------------------------------------
//int getCitiesLost();
int CvLuaPlayer::lGetCitiesLost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getCitiesLost);
}

//------------------------------------------------------------------------------
//int getPower();
int CvLuaPlayer::lGetPower(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getPower);
}
//------------------------------------------------------------------------------
//int GetMilitaryMight();
int CvLuaPlayer::lGetMilitaryMight(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetMilitaryMight);
}
//------------------------------------------------------------------------------
//int GetMilitaryMight();
int CvLuaPlayer::lGetMilitaryMightForCS(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvWeightedVector<PlayerTypes> veMilitaryRankings;
	PlayerTypes eMajorLoop;

	int iRankRatio = 0;
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajorLoop = (PlayerTypes)iMajorLoop;
		if (GET_PLAYER(eMajorLoop).isAlive() && !GET_PLAYER(eMajorLoop).isMinorCiv())
		{
			veMilitaryRankings.push_back(eMajorLoop, GET_PLAYER(eMajorLoop).GetMilitaryMight(true)); // Don't recalculate within a turn, can cause inconsistency
		}
	}
	veMilitaryRankings.SortItems();
	for (int iRanking = 0; iRanking < veMilitaryRankings.size(); iRanking++)
	{
		if (veMilitaryRankings.GetElement(iRanking) == pkPlayer->GetID())
		{
			iRankRatio = ((veMilitaryRankings.size() - iRanking) * 100) / veMilitaryRankings.size();
			break;
		}
	}
	lua_pushinteger(L, iRankRatio);
	return 1;
}

//------------------------------------------------------------------------------
//int getTotalTimePlayed();
int CvLuaPlayer::lGetTotalTimePlayed(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getTotalTimePlayed);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScore);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromCities);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromPopulation);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromLand);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromWonders(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromWonders);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromTechs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromTechs);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromFutureTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromFutureTech);
}
//------------------------------------------------------------------------------
//void ChangeScoreFromFutureTech(int iChange);
int CvLuaPlayer::lChangeScoreFromFutureTech(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iChange = lua_tointeger(L, 2);

	pkPlayer->ChangeScoreFromFutureTech(iChange);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromScenario1(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromScenario1);
}
//------------------------------------------------------------------------------
//void ChangeScoreFromScenario1(int iChange);
int CvLuaPlayer::lChangeScoreFromScenario1(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iChange = lua_tointeger(L, 2);

	pkPlayer->ChangeScoreFromScenario1(iChange);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromScenario2(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromScenario2);
}
//------------------------------------------------------------------------------
//void ChangeScoreFromScenario2(int iChange);
int CvLuaPlayer::lChangeScoreFromScenario2(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iChange = lua_tointeger(L, 2);

	pkPlayer->ChangeScoreFromScenario2(iChange);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromScenario3(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromScenario3);
}
//------------------------------------------------------------------------------
//void ChangeScoreFromScenario3(int iChange);
int CvLuaPlayer::lChangeScoreFromScenario3(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iChange = lua_tointeger(L, 2);

	pkPlayer->ChangeScoreFromScenario3(iChange);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromScenario4(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromScenario4);
}
//------------------------------------------------------------------------------
//void ChangeScoreFromScenario4(int iChange);
int CvLuaPlayer::lChangeScoreFromScenario4(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iChange = lua_tointeger(L, 2);

	pkPlayer->ChangeScoreFromScenario4(iChange);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromPolicies(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromPolicies);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromGreatWorks(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromGreatWorks);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromReligion);
}

int CvLuaPlayer::lIsGoldenAgeCultureBonusDisabled(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsGoldenAgeCultureBonusDisabled);
}

//------------------------------------------------------------------------------
//bool isMajorCiv();
int CvLuaPlayer::lIsMajorCiv(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isMajorCiv);
}
//------------------------------------------------------------------------------
//bool isMinorCiv();
int CvLuaPlayer::lIsMinorCiv(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isMinorCiv);
}
//------------------------------------------------------------------------------
//bool getMinorCivID();
int CvLuaPlayer::lGetMinorCivType(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetMinorCivType();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivTrait(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetTrait();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPersonality(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetPersonality();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetPersonality(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iPersonality = lua_tointeger(L, 2);

	pkPlayer->GetMinorCivAI()->SetPersonality((MinorCivPersonalityTypes) iPersonality);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorCivHasUniqueUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsHasUniqueUnit();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivUniqueUnit(lua_State *L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetUniqueUnit();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetMinorCivUniqueUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 2);

	pkPlayer->GetMinorCivAI()->SetUniqueUnit(eUnit);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetAlly(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetAlly();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetAlliedTurns(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetAlliedTurns();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsFriends(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsFriends(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAllies(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsAllies(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerHasOpenBorders(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsPlayerHasOpenBorders(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerHasOpenBordersAutomatically(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsPlayerHasOpenBordersAutomatically(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetFriendshipChangePerTurnTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetFriendshipChangePerTurnTimes100(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivFriendshipWithMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void ChangeMinorCivFriendshipWithMajor(PlayerTypes ePlayer, int iChange);
int CvLuaPlayer::lChangeMinorCivFriendshipWithMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	int iChange = lua_tointeger(L, 3);

	pkPlayer->GetMinorCivAI()->ChangeFriendshipWithMajor(ePlayer, iChange);
	return 1;
}
//------------------------------------------------------------------------------
//void GetMinorCivFriendshipAnchorWithMajor(PlayerTypes eMajor);
int CvLuaPlayer::lGetMinorCivFriendshipAnchorWithMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetFriendshipAnchorWithMajor(eMajor);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPeaceBlocked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsPeaceBlocked(eTeam);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorPermanentWar(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsPermanentWar(eTeam);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumMinorCivsMet(lua_State* L)
{
	int iNumMinorCivsMet = 0;
	CvPlayerAI* pkPlayer = GetInstance(L);
	TeamTypes eTeam = pkPlayer->getTeam();

	for(int i = MAX_MAJOR_CIVS; i < MAX_CIV_PLAYERS; i++)
	{
		TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)i).getTeam();

		if(eOtherTeam == eTeam)
		{
			continue;
		}

		if(GET_TEAM(eTeam).isHasMet(eOtherTeam))
		{
			iNumMinorCivsMet += 1;
		}
	}

	lua_pushinteger(L, iNumMinorCivsMet);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetFriendshipNeededForNextLevel(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetFriendshipNeededForNextLevel(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivFriendshipLevelWithMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetFriendshipLevelWithMajor(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
// Deprecated
int CvLuaPlayer::lGetActiveQuestForPlayer(lua_State* L)
{
	return lIsMinorCivActiveQuestForPlayer(L);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorCivActiveQuestForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	const int bResult = pkPlayer->GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eType);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lSetMinorCivActiveQuestForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes)lua_tointeger(L, 3);
	const int iTurn = lua_tointeger(L, 4);
	const PlayerTypes eCallingPlayer = (PlayerTypes)luaL_optint(L, 5, NO_PLAYER);

	pkPlayer->GetMinorCivAI()->AddQuestForPlayer(ePlayer, eType, iTurn, eCallingPlayer);
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivNumActiveQuestsForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetNumActiveQuestsForPlayer(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorCivDisplayedQuestForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	const int bResult = pkPlayer->GetMinorCivAI()->IsDisplayedQuestForPlayer(ePlayer, eType);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivNumDisplayedQuestsForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetNumDisplayedQuestsForPlayer(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetQuestData1(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	const int iResult = pkPlayer->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetQuestData2(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	const int iResult = pkPlayer->GetMinorCivAI()->GetQuestData2(ePlayer, eType);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetQuestTurnsRemaining(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);
	const int iGameTurn = lua_tointeger(L, 4);

	const int iResult = pkPlayer->GetMinorCivAI()->GetQuestTurnsRemaining(ePlayer, eType, iGameTurn);
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetRewardString(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	CvString sResult = pkPlayer->GetMinorCivAI()->GetRewardString(ePlayer, eType);
	lua_pushstring(L, sResult);
	return 1;
}
int CvLuaPlayer::lGetTargetCityString(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	CvString sResult = pkPlayer->GetMinorCivAI()->GetTargetCityString(ePlayer, eType);
	lua_pushstring(L, sResult);
	return 1;
}
int CvLuaPlayer::lGetExplorePercent(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	const int iResult = pkPlayer->GetMinorCivAI()->GetExplorePercent(ePlayer, eType);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetXQuestBuildingRemaining(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 4);
	int iNeeded = pkPlayer->GetMinorCivAI()->GetQuestData2(ePlayer, eType);
	int iBuilt = GET_PLAYER(ePlayer).getNumBuildings(eBuilding);

	const int iResult = (iNeeded - iBuilt);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lQuestSpyActionsRemaining(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);
	const PlayerTypes eTargetPlayer = (PlayerTypes)pkPlayer->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
	int iNeeded = pkPlayer->GetMinorCivAI()->GetQuestData2(ePlayer, eType);
	int iDone = GET_PLAYER(ePlayer).GetEspionage()->GetNumSpyActionsDone(eTargetPlayer);

	const int iResult = (iNeeded - iDone);
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorCivContestLeader(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);
	
	const bool bResult = pkPlayer->GetMinorCivAI()->IsContestLeader(ePlayer, eType);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivContestValueForLeader(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetContestValueForLeader(eType);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivContestValueForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const MinorCivQuestTypes eType = (MinorCivQuestTypes) lua_tointeger(L, 3);

	const int iResult = pkPlayer->GetMinorCivAI()->GetContestValueForPlayer(ePlayer, eType);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorCivUnitSpawningDisabled(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsUnitSpawningDisabled(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorCivRouteEstablishedWithMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsRouteConnectionEstablished(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMinorWarQuestWithMajorActive(lua_State* L)
{
	const bool bResult = false;
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorWarQuestWithMajorRemainingCount(lua_State* L)
{
	const int iResult = 0;
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsProxyWarActiveForMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsProxyWarActiveForMajor(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsThreateningBarbariansEventActiveForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsThreateningBarbariansEventActiveForPlayer(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
// antonjs: Deprecated, kept here for backwards compatibility
int CvLuaPlayer::lGetTurnsSinceThreatenedByBarbarians(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetMinorCivAI()->GetTurnsSinceThreatenedAnnouncement();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTurnsSinceThreatenedAnnouncement(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetMinorCivAI()->GetTurnsSinceThreatenedAnnouncement();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetFriendshipFromGoldGift(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);
	const int iGold = lua_tointeger(L, 3);

	const int iResult = pkPlayer->GetMinorCivAI()->GetFriendshipFromGoldGift(eMajor, iGold);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivFavoriteMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetAlly());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivScienceFriendshipBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetScienceFriendshipBonus());
	return 1;
}
//------------------------------------------------------------------------------
// antonjs: Deprecated, kept here for backwards compatibility
int CvLuaPlayer::lGetMinorCivCultureFriendshipBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);

	int iValue = 0;
	iValue += pkPlayer->GetMinorCivAI()->GetCultureFlatFriendshipBonus(ePlayer);
	iValue += pkPlayer->GetMinorCivAI()->GetCulturePerBuildingFriendshipBonus(ePlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentCultureFlatBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentCultureFlatBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentCulturePerBuildingBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentCulturePerBuildingBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
// antonjs: Deprecated, kept here for backwards compatibility
int CvLuaPlayer::lGetCurrentCultureBonus(lua_State* L)
{
	return lGetMinorCivCurrentCultureBonus(L);
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentCultureBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentCultureBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
// antonjs: Deprecated, kept here for backwards compatibility
int CvLuaPlayer::lGetMinorCivHappinessFriendshipBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);

	int iValue = 0;
	iValue += pkPlayer->GetMinorCivAI()->GetHappinessFlatFriendshipBonus(ePlayer);
	iValue += pkPlayer->GetMinorCivAI()->GetHappinessPerLuxuryFriendshipBonus(ePlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentHappinessFlatBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentHappinessFlatBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentHappinessPerLuxuryBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentHappinessPerLuxuryBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentHappinessBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentHappinessBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentFaithBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentFaithBonus(ePlayer));
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsNoAlly(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->GetMinorCivAI()->IsNoAlly();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPermanentAlly(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetPermanentAlly();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentGoldBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentGoldBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivCurrentScienceBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentScienceBonus(ePlayer));
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCurrentCapitalFoodBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentCapitalFoodBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCurrentOtherCityFoodBonus(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentOtherCityFoodBonus(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCurrentSpawnEstimate(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentSpawnEstimate(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCurrentScienceFriendshipBonusTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkPlayer->GetMinorCivAI()->GetCurrentScienceFriendshipBonusTimes100(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
//void DoMinorLiberationByMajor(PlayerTypes eLiberator, TeamTypes eConquerorTeam);
int CvLuaPlayer::lDoMinorLiberationByMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eLiberator = (PlayerTypes) lua_tointeger(L, 2);
	TeamTypes eConquerorTeam = (TeamTypes) lua_tointeger(L, 3);

	pkPlayer->GetMinorCivAI()->DoLiberationByMajor(eLiberator, eConquerorTeam);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsProtectedByMajor(PlayerTypes eMajor);
int CvLuaPlayer::lIsProtectedByMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsProtectedByMajor(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorProtect(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorProtect(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorProtect(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorStartProtection(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorStartProtection(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorStartProtection(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorWithdrawProtection(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorWithdrawProtection(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorWithdrawProtection(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor);
int CvLuaPlayer::lGetTurnLastPledgedProtectionByMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetMinorCivAI()->GetTurnLastPledgedProtectionByMajor(eMajor);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor);
int CvLuaPlayer::lGetTurnLastPledgeBrokenByMajor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetMinorCivAI()->GetTurnLastPledgeBrokenByMajor(eMajor);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetMinorCivBullyGoldAmount(PlayerTypes eMajor);
int CvLuaPlayer::lGetMinorCivBullyGoldAmount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetMinorCivAI()->GetBullyGoldAmount(eMajor);
	lua_pushinteger(L, iValue);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//int SetBullyUnit(PlayerTypes eMajor, YieldTypes eYield);
int CvLuaPlayer::lSetBullyUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	UnitClassTypes eUnitClass = (UnitClassTypes) lua_tointeger(L, 2);

	pkPlayer->GetMinorCivAI()->SetBullyUnit(eUnitClass);
	return 0;
}
int CvLuaPlayer::lGetBullyUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	UnitClassTypes  eUnitClass = pkPlayer->GetMinorCivAI()->GetBullyUnit();
	if(eUnitClass != NO_UNITCLASS)
	{
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo != NULL)
		{
			const UnitTypes eUnit = ((UnitTypes)(pkPlayer->GetSpecificUnitType(eUnitClass)));
			if(eUnit != NO_UNIT)
			{
				lua_pushinteger(L, eUnit);
				return 1;
			}
		}
	}		

	lua_pushinteger(L, (UnitTypes) GC.getInfoTypeForString("UNIT_WORKER"));
	return 1;
}
//------------------------------------------------------------------------------
//int GetYieldTheftAmount(PlayerTypes eMajor, YieldTypes eYield);
int CvLuaPlayer::lGetYieldTheftAmount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);
	const YieldTypes eYield = (YieldTypes) lua_tointeger(L, 3);

	const int iValue = pkPlayer->GetMinorCivAI()->GetYieldTheftAmount(eMajor, eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetPledgeProtectionInvalidReason(PlayerTypes eMajor);
int CvLuaPlayer::lGetPledgeProtectionInvalidReason(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes)lua_tointeger(L, 2);

	const CvString sResult = pkPlayer->GetMinorCivAI()->GetPledgeProtectionInvalidReason(eMajor);
	lua_pushstring(L, sResult);
	return 1;
}

#endif
//------------------------------------------------------------------------------
//bool CanMajorBullyGold(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorBullyGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	if (MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
	{
		int iScore = pkPlayer->GetMinorCivAI()->CalculateBullyScore(eMajor, /*bForUnit*/ false);
		const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorBullyGold(eMajor, iScore);
		lua_pushboolean(L, bResult);
		return 1;
	}
	else
	{
		const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorBullyGold(eMajor);
		lua_pushboolean(L, bResult);
		return 1;
	}
}
//------------------------------------------------------------------------------
//bool GetMajorBullyGoldDetails(PlayerTypes eMajor);
int CvLuaPlayer::lGetMajorBullyGoldDetails(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const CvString sResult = pkPlayer->GetMinorCivAI()->GetMajorBullyGoldDetails(eMajor);
	lua_pushstring(L, sResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorBullyUnit(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorBullyUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);
	int iTargetVal = (PlayerTypes)luaL_optint(L, 3, 0);

	if (iTargetVal > 0)
	{
		int iScore = pkPlayer->GetMinorCivAI()->CalculateBullyScore(eMajor, /*bForUnit*/ true);
		const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorBullyUnit(eMajor, (iScore - iTargetVal));
		lua_pushboolean(L, bResult);
		return 1;
	}
	else
	{
		const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorBullyUnit(eMajor);
		lua_pushboolean(L, bResult);
		return 1;
	}
}
//------------------------------------------------------------------------------
//bool GetMajorBullyUnitDetails(PlayerTypes eMajor);
int CvLuaPlayer::lGetMajorBullyUnitDetails(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const CvString sResult = pkPlayer->GetMinorCivAI()->GetMajorBullyUnitDetails(eMajor);
	lua_pushstring(L, sResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool GetMajorBullyUnitDetails(PlayerTypes eMajor);
int CvLuaPlayer::lGetMajorBullyValue(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes)lua_tointeger(L, 2);
	bool bForUnit = luaL_optbool(L, 3, false);

	//since this is just for UI, flip it to positive.
	const int iValue = abs(pkPlayer->GetMinorCivAI()->CalculateBullyScore(eMajor, bForUnit));

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorBuyout(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorBuyout(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorBuyout(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//bool CanMajorMarry(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorMarry(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorDiploMarriage(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void DoMarriage(PlayerTypes eMajor);
int CvLuaPlayer::lDoMarriage(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	pkPlayer->GetMinorCivAI()->DoMarriage(eMajor);
	return 0;
}
//------------------------------------------------------------------------------
//void DoMarriage(PlayerTypes eMajor);
int CvLuaPlayer::lIsMarried(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsMarried(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetMarriageCost(PlayerTypes eMajor);
int CvLuaPlayer::lGetMarriageCost(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const int iCost = pkPlayer->GetMinorCivAI()->GetMarriageCost(eMajor);
	lua_pushinteger(L, iCost);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int GetBuyoutCost(PlayerTypes eMajor);
int CvLuaPlayer::lGetBuyoutCost(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const int iCost = pkPlayer->GetMinorCivAI()->GetBuyoutCost(eMajor);
	lua_pushinteger(L, iCost);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorGiftTileImprovement(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorGiftTileImprovement(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorGiftTileImprovement(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorGiftTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY);
int CvLuaPlayer::lCanMajorGiftTileImprovementAtPlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);
	const int iX = lua_tointeger(L, 3);
	const int iY = lua_tointeger(L, 4);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorGiftTileImprovementAtPlot(eMajor, iX, iY);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetGiftTileImprovementCost(PlayerTypes eMajor);
int CvLuaPlayer::lGetGiftTileImprovementCost(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const int iCost = pkPlayer->GetMinorCivAI()->GetGiftTileImprovementCost(eMajor);
	lua_pushinteger(L, iCost);
	return 1;
}
//------------------------------------------------------------------------------
//bool AddMinorCivQuestIfAble(PlayerTypes eMajor, MinorCivQuestTypes eQuest);
int CvLuaPlayer::lAddMinorCivQuestIfAble(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);
	MinorCivQuestTypes eQuest = (MinorCivQuestTypes) lua_tointeger(L, 3);

	const bool bResult = pkPlayer->GetMinorCivAI()->AddQuestIfAble(eMajor, eQuest);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetFriendshipFromUnitGift(PlayerTypes eMajor, bool bGreatPerson, bool bDistanceGift);
int CvLuaPlayer::lGetFriendshipFromUnitGift(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);
	bool bGreatPerson = lua_toboolean(L, 3);
	bool bDistanceGift = lua_toboolean(L, 4);

	const int iResult = pkPlayer->GetMinorCivAI()->GetFriendshipFromUnitGift(eMajor, bGreatPerson, bDistanceGift);
	lua_pushinteger(L, iResult);
	return 1;
}

#if defined(MOD_BALANCE_CORE_MINORS) || defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//------------------------------------------------------------------------------
//int GetJerk(TeamTypes eTeam);
int CvLuaPlayer::lGetJerk(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetJerkTurnsRemaining(eTeam);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetCoupCooldown(TeamTypes eTeam);
int CvLuaPlayer::lGetCoupCooldown(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetMinorCivAI()->GetCoupCooldown();
	lua_pushinteger(L, iResult);
	return 1;
}
#endif

//------------------------------------------------------------------------------
//int GetNumDenouncements();
int CvLuaPlayer::lGetNumDenouncements(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetDiplomacyAI()->GetNumDenouncements();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumDenouncementsOfPlayer();
int CvLuaPlayer::lGetNumDenouncementsOfPlayer(lua_State* L)
{
	
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetDiplomacyAI()->GetNumDenouncementsOfPlayer();
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE_HAPPINESS)
//------------------------------------------------------------------------------
//int getUnhappinessFromCityCulture();
int CvLuaPlayer::lGetUnhappinessFromCityCulture(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityCulture();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnhappinessFromCityScience();
int CvLuaPlayer::lGetUnhappinessFromCityScience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityScience();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnhappinessFromCityDefense();
int CvLuaPlayer::lGetUnhappinessFromCityDefense(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityDefense();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnhappinessFromCityGold();
int CvLuaPlayer::lGetUnhappinessFromCityGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityGold();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnhappinessFromCityConnection();
int CvLuaPlayer::lGetUnhappinessFromCityConnection(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityConnection();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnhappinessFromCityPillaged();
int CvLuaPlayer::lGetUnhappinessFromCityPillaged(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityPillaged();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnhappinessFromCityStarving();
int CvLuaPlayer::lGetUnhappinessFromCityStarving(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityStarving();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnhappinessFromCityMinority();
int CvLuaPlayer::lGetUnhappinessFromCityMinority(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityMinority();
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaPlayer::lGetUnhappinessFromJFDSpecial(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->getUnhappinessFromCityJFDSpecial();
	lua_pushinteger(L, iResult);
	return 1;
}
#endif

#if defined(MOD_BALANCE_CORE_HAPPINESS_LUXURY)
//------------------------------------------------------------------------------
//int getBonusHappinessFromLuxuries();
int CvLuaPlayer::lGetBonusHappinessFromLuxuries(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetBonusHappinessFromLuxuries();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetScalingNationalPopulationRequrired();
int CvLuaPlayer::lGetScalingNationalPopulationRequrired(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetScalingNationalPopulationRequrired(eBuilding);
	lua_pushinteger(L, iResult);
	return 1;
}
#endif

#if defined(MOD_BALANCE_CORE_HAPPINESS_MODIFIERS)
//------------------------------------------------------------------------------
//int GetPuppetUnhappinessMod();
int CvLuaPlayer::lGetPuppetUnhappinessMod(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetPuppetUnhappinessMod();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetCapitalUnhappinessModCBP();
int CvLuaPlayer::lGetCapitalUnhappinessModCBP(lua_State* L)
{
	const int iResult = GC.getBALANCE_HAPPINESS_CAPITAL_MODIFIER();
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool isAlive();
int CvLuaPlayer::lIsAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isAlive);
}
//------------------------------------------------------------------------------
//bool isEverAlive();
int CvLuaPlayer::lIsEverAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isEverAlive);
}

//------------------------------------------------------------------------------
//bool isEverAlive();
int CvLuaPlayer::lIsPotentiallyAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isPotentiallyAlive);
}
//------------------------------------------------------------------------------
//bool isExtendedGame();
int CvLuaPlayer::lIsExtendedGame(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isExtendedGame);
}
//------------------------------------------------------------------------------
//bool isFoundedFirstCity();
int CvLuaPlayer::lIsFoundedFirstCity(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isFoundedFirstCity);
}

//------------------------------------------------------------------------------
//EndTurnBlockingType GetEndTurnBlockingType()
int CvLuaPlayer::lGetEndTurnBlockingType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetEndTurnBlockingType);
}
//------------------------------------------------------------------------------
//EndTurnBlockingType GetEndTurnBlockingNotificationIndex()
int CvLuaPlayer::lGetEndTurnBlockingNotificationIndex(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetEndTurnBlockingNotificationIndex);
}

//------------------------------------------------------------------------------
//bool isStrike();
int CvLuaPlayer::lIsStrike(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isStrike);
}
//------------------------------------------------------------------------------
//int getID();
int CvLuaPlayer::lGetID(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetID);
}
//------------------------------------------------------------------------------
//int HandicapTypes getHandicapType();
int CvLuaPlayer::lGetHandicapType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getHandicapType);
}
//------------------------------------------------------------------------------
//int CivilizationTypes getCivilizationType();
int CvLuaPlayer::lGetCivilizationType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getCivilizationType);
}
//------------------------------------------------------------------------------
//LeaderHeadTypes  getLeaderType();
int CvLuaPlayer::lGetLeaderType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getLeaderType);
}
//------------------------------------------------------------------------------
//void  setLeaderType(LeaderHeadTypes eNewleader);
int CvLuaPlayer::lSetLeaderType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setLeaderType);
}
//------------------------------------------------------------------------------
//LeaderHeadTypes  getPersonalityType()
int CvLuaPlayer::lGetPersonalityType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getPersonalityType);
}
//------------------------------------------------------------------------------
//void setPersonalityType(LeaderHeadTypes  eNewValue);
int CvLuaPlayer::lSetPersonalityType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setPersonalityType);
}
//------------------------------------------------------------------------------
//ErasTypes  GetCurrentEra();
int CvLuaPlayer::lGetCurrentEra(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCurrentEra);
}
//------------------------------------------------------------------------------
//int getTeam();
int CvLuaPlayer::lGetTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getTeam);
}
//------------------------------------------------------------------------------
//ColorTypes GetPlayerColor();
int CvLuaPlayer::lGetPlayerColor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerColorTypes eColor = CvPreGame::playerColor(pkPlayer->GetID());
	lua_pushinteger(L, eColor);
	return 1;
}
//------------------------------------------------------------------------------
//primaryColor, secondaryColor  getPlayerColors();
int CvLuaPlayer::lGetPlayerColors(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerColorTypes eColor = CvPreGame::playerColor(pkPlayer->GetID());

	CvPlayerColorInfo* pkPlayerColor = GC.GetPlayerColorInfo(eColor);

	if(pkPlayerColor == NULL)
	{
		luaL_error(L, "Could not find player color at row %d", eColor);
		return 0;
	}

	const ColorTypes ePrimaryColor	 = (ColorTypes)pkPlayerColor->GetColorTypePrimary();
	const ColorTypes eSecondaryColor = (ColorTypes)pkPlayerColor->GetColorTypeSecondary();

	CvColorInfo* pkPrimaryColor = GC.GetColorInfo(ePrimaryColor);
	if(pkPrimaryColor == NULL)
	{
		luaL_error(L, "Could not find primary color at row %d", ePrimaryColor);
		return 0;
	}
	const CvColorA& kPrimaryColor = pkPrimaryColor->GetColor();

	CvColorInfo* pkSecondaryColor = GC.GetColorInfo(eSecondaryColor);
	if(pkSecondaryColor == NULL)
	{
		luaL_error(L, "Could not find secondary color at row %d", eSecondaryColor);
		return 0;
	}
	const CvColorA& kSecondaryColor = pkSecondaryColor->GetColor();

	//Now export colors
	lua_createtable(L, 0, 4);
	lua_pushnumber(L, kPrimaryColor.r);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, kPrimaryColor.g);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, kPrimaryColor.b);
	lua_setfield(L, -2, "z");
	lua_pushnumber(L, kPrimaryColor.a);
	lua_setfield(L, -2, "w");

	lua_createtable(L, 0, 4);
	lua_pushnumber(L, kSecondaryColor.r);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, kSecondaryColor.g);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, kSecondaryColor.b);
	lua_setfield(L, -2, "z");
	lua_pushnumber(L, kSecondaryColor.a);
	lua_setfield(L, -2, "w");

	return 2;
}
//------------------------------------------------------------------------------
//int getSeaPlotYield(YieldTypes eIndex);
int CvLuaPlayer::lGetSeaPlotYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getSeaPlotYield);
}
//------------------------------------------------------------------------------
//int getYieldRateModifier(YieldTypes eIndex);
int CvLuaPlayer::lGetYieldRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getYieldRateModifier);
}
//------------------------------------------------------------------------------
//int getCapitalYieldRateModifier(YieldTypes eIndex);
int CvLuaPlayer::lGetCapitalYieldRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getCapitalYieldRateModifier);
}
//------------------------------------------------------------------------------
//int getExtraYieldThreshold(YieldTypes eIndex);
int CvLuaPlayer::lGetExtraYieldThreshold(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getExtraYieldThreshold);
}
//------------------------------------------------------------------------------
//int getScience();
int CvLuaPlayer::lGetScience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScience);
}
//------------------------------------------------------------------------------
//int GetScienceTimes100();
int CvLuaPlayer::lGetScienceTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceTimes100);
}
//int GetScienceFromCitiesTimes100();
int CvLuaPlayer::lGetScienceFromCitiesTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceFromCitiesTimes100);
}
//int GetScienceFromOtherPlayersTimes100();
int CvLuaPlayer::lGetScienceFromOtherPlayersTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceFromOtherPlayersTimes100);
}
//int GetScienceFromHappinessTimes100();
int CvLuaPlayer::lGetScienceFromHappinessTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceFromHappinessTimes100);
}
//int GetScienceFromResearchAgreementsTimes100();
int CvLuaPlayer::lGetScienceFromResearchAgreementsTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceFromResearchAgreementsTimes100);
}
//int GetScienceFromBudgetDeficitTimes100();
int CvLuaPlayer::lGetScienceFromBudgetDeficitTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScienceFromBudgetDeficitTimes100);
}
//------------------------------------------------------------------------------
//int GetProximityToPlayer(PlayerTypes  eIndex);
int CvLuaPlayer::lGetProximityToPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetProximityToPlayer);
}
//------------------------------------------------------------------------------
//void DoUpdateProximityToPlayer(PlayerTypes  eIndex);
int CvLuaPlayer::lDoUpdateProximityToPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::DoUpdateProximityToPlayer);
}
//------------------------------------------------------------------------------
//int GetIncomingUnitType(PlayerTypes eFromPlayer);
int CvLuaPlayer::lGetIncomingUnitType(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::GetIncomingUnitType);
}
//------------------------------------------------------------------------------
//int GetIncomingUnitCountdown(PlayerTypes eFromPlayer);
int CvLuaPlayer::lGetIncomingUnitCountdown(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::GetIncomingUnitCountdown);
}
//------------------------------------------------------------------------------
//bool isOption(PlayerOptionTypes  eIndex);
int CvLuaPlayer::lIsOption(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isOption);
}
//------------------------------------------------------------------------------
//void setOption(PlayerOptionTypes  eIndex, bool bNewValue);
int CvLuaPlayer::lSetOption(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setOption);
}
//------------------------------------------------------------------------------
//bool isPlayable();
int CvLuaPlayer::lIsPlayable(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isPlayable);
}
//------------------------------------------------------------------------------
//void setPlayable(bool bNewValue);
int CvLuaPlayer::lSetPlayable(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setPlayable);
}
//------------------------------------------------------------------------------
//int getNumResourceUsed(ResourceTypes  iIndex);
int CvLuaPlayer::lGetNumResourceUsed(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumResourceUsed);
}
//------------------------------------------------------------------------------
//int getNumResourceTotal(ResourceTypes  iIndex, bool bIncludeImport);
int CvLuaPlayer::lGetNumResourceTotal(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumResourceTotal);
}
//------------------------------------------------------------------------------
//void changeNumResourceTotal(ResourceTypes  iIndex, int iChange);
int CvLuaPlayer::lChangeNumResourceTotal(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeNumResourceTotal);
}
//------------------------------------------------------------------------------
//int getNumResourceAvailable(ResourceTypes  iIndex, bool bIncludeImport);
int CvLuaPlayer::lGetNumResourceAvailable(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumResourceAvailable);
}

//------------------------------------------------------------------------------
//int getResourceExport(ResourceTypes  iIndex);
int CvLuaPlayer::lGetResourceExport(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getResourceExport);
}
//------------------------------------------------------------------------------
//int getResourceImport(ResourceTypes  iIndex);
int CvLuaPlayer::lGetResourceImport(lua_State* L)
{
	//we have to sum up several types of import here
	//everything except GetResourceFromMinors because that has it's own method
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes) lua_tointeger(L, 2);

	int iSum = 
		pkPlayer->getResourceImportFromMajor(eResource) + 
		pkPlayer->getResourceFromMinors(eResource) +
		pkPlayer->getResourceSiphoned(eResource);
	
	lua_pushinteger(L, iSum);
	return 1;
}
//------------------------------------------------------------------------------
//int getResourceFromMinors(ResourceTypes  iIndex);
int CvLuaPlayer::lGetResourceFromMinors(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getResourceFromMinors);
}

//------------------------------------------------------------------------------
//int getImprovementCount(ImprovementTypes  iIndex);
int CvLuaPlayer::lGetImprovementCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getImprovementCount);
}

//------------------------------------------------------------------------------
//bool isBuildingFree(BuildingTypes  iIndex);
int CvLuaPlayer::lIsBuildingFree(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isBuildingFree);
}
//------------------------------------------------------------------------------
//int getUnitClassCount(UnitClassTypes  eIndex);
int CvLuaPlayer::lGetUnitClassCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getUnitClassCount);
}
//------------------------------------------------------------------------------
//bool isUnitClassMaxedOut(UnitClassTypes  eIndex, int iExtra);
int CvLuaPlayer::lIsUnitClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isUnitClassMaxedOut);
}
//------------------------------------------------------------------------------
//int getUnitClassMaking(UnitClassTypes  eIndex);
int CvLuaPlayer::lGetUnitClassMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getUnitClassMaking);
}
//------------------------------------------------------------------------------
//int getUnitClassCountPlusMaking(UnitClassTypes  eIndex);
int CvLuaPlayer::lGetUnitClassCountPlusMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getUnitClassCountPlusMaking);
}
//------------------------------------------------------------------------------
//int getBuildingClassCount(BuildingClassTypes  iIndex);
int CvLuaPlayer::lGetBuildingClassCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getBuildingClassCount);
}
//------------------------------------------------------------------------------
//bool isBuildingClassMaxedOut(BuildingClassTypes  iIndex, int iExtra);
int CvLuaPlayer::lIsBuildingClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isBuildingClassMaxedOut);
}
//------------------------------------------------------------------------------
//int getBuildingClassMaking(BuildingClassTypes  iIndex);
int CvLuaPlayer::lGetBuildingClassMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getBuildingClassMaking);
}
//------------------------------------------------------------------------------
//int getBuildingClassCountPlusMaking(BuildingClassTypes  iIndex);
int CvLuaPlayer::lGetBuildingClassCountPlusMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getBuildingClassCountPlusMaking);
}
//------------------------------------------------------------------------------
//int getHurryCount(HurryTypes  eIndex);
int CvLuaPlayer::lGetHurryCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getHurryCount);
}
//------------------------------------------------------------------------------
//bool IsHasAccessToHurry(HurryTypes  eIndex);
int CvLuaPlayer::lIsHasAccessToHurry(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsHasAccessToHurry);
}
//------------------------------------------------------------------------------
//bool IsCanHurry(HurryTypes  eIndex);
int CvLuaPlayer::lIsCanHurry(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsCanHurry);
}
//------------------------------------------------------------------------------
//int GetHurryGoldCost(HurryTypes  eIndex);
int CvLuaPlayer::lGetHurryGoldCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHurryGoldCost);
}

//------------------------------------------------------------------------------
//bool isResearchingTech(TechTypes  iIndex);
int CvLuaPlayer::lIsResearchingTech(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const TechTypes iIndex = (TechTypes)lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetPlayerTechs()->IsResearchingTech(iIndex);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setResearchingTech(TechTypes eIndex, bool bNewValue);
int CvLuaPlayer::lSetResearchingTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setResearchingTech);
}

//------------------------------------------------------------------------------
//int getCombatExperience();
int CvLuaPlayer::lGetCombatExperience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_pushinteger(L, pkPlayer->getCombatExperienceTimes100() / 100);
	return 1;
}
//------------------------------------------------------------------------------
//void changeCombatExperience(int iChange);
int CvLuaPlayer::lChangeCombatExperience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iExperience = lua_tointeger(L, 2);

	pkPlayer->changeCombatExperienceTimes100(iExperience * 100);
	return 0;
}
//------------------------------------------------------------------------------
//void setCombatExperience(int iExperience);
int CvLuaPlayer::lSetCombatExperience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iExperience = lua_tointeger(L, 2);

	pkPlayer->setCombatExperienceTimes100(iExperience * 100);
	return 0;
}
//------------------------------------------------------------------------------
//int getLifetimeCombatExperience();
int CvLuaPlayer::lGetLifetimeCombatExperience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_pushinteger(L, pkPlayer->getLifetimeCombatExperienceTimes100() / 100);
	return 1;
}
//------------------------------------------------------------------------------
//int getNavalCombatExperience();
int CvLuaPlayer::lGetNavalCombatExperience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_pushinteger(L, pkPlayer->getNavalCombatExperienceTimes100() / 100);
	return 1;
}
//------------------------------------------------------------------------------
//void changeNavalCombatExperience(int iChange);
int CvLuaPlayer::lChangeNavalCombatExperience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iExperience = lua_tointeger(L, 2);

	pkPlayer->changeNavalCombatExperienceTimes100(iExperience * 100);
	return 0;
}
//------------------------------------------------------------------------------
//void setCombatExperience(int iExperience);
int CvLuaPlayer::lSetNavalCombatExperience(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iExperience = lua_tointeger(L, 2);

	pkPlayer->setNavalCombatExperienceTimes100(iExperience * 100);
	return 0;
}
//------------------------------------------------------------------------------
//int getSpecialistExtraYield(SpecialistTypes  eIndex1, YieldTypes  eIndex2);
int CvLuaPlayer::lGetSpecialistExtraYield(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const SpecialistTypes eIndex1 = (SpecialistTypes)lua_tointeger(L, 2);
	const YieldTypes eIndex2 = (YieldTypes)lua_tointeger(L, 3);

	const int iResult = pkPlayer->getSpecialistExtraYield(eIndex1, eIndex2) +
	                    pkPlayer->GetPlayerTraits()->GetSpecialistYieldChange(eIndex1, eIndex2);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int findPathLength(TechTypes  eTech, bool bCost);
// If bCost is false, then it returns number of techs that need to be researched to acquire eTech
// If bCost is true, then it returns the cost of a currently researched tech
int CvLuaPlayer::lFindPathLength(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::findPathLength);
}
//------------------------------------------------------------------------------
//int getQueuePosition( TechTypes  eTech );
int CvLuaPlayer::lGetQueuePosition(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getQueuePosition);
}
//------------------------------------------------------------------------------
//void clearResearchQueue();
int CvLuaPlayer::lClearResearchQueue(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::clearResearchQueue);
}
//------------------------------------------------------------------------------
//bool pushResearch(TechTypes  iIndex, bool bClear);
int CvLuaPlayer::lPushResearch(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::pushResearch);
}
//------------------------------------------------------------------------------
//void popResearch(TechTypes  eTech);
int CvLuaPlayer::lPopResearch(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::popResearch);
}
//------------------------------------------------------------------------------
//int getLengthResearchQueue();
int CvLuaPlayer::lGetLengthResearchQueue(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getLengthResearchQueue);
}
//------------------------------------------------------------------------------
//void addCityName(string szName);
int CvLuaPlayer::lAddCityName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		CvString cityName = lua_tostring(L, 2);
		pkPlayer->addCityName(cityName);
	}

	return 0;
}
//------------------------------------------------------------------------------
//int getNumCityNames();
int CvLuaPlayer::lGetNumCityNames(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumCityNames);
}
//------------------------------------------------------------------------------
//string getCityName(int iIndex);
int CvLuaPlayer::lGetCityName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		const int index = luaL_checkint(L, 2);
		CvString cityName = pkPlayer->getCityName(index);
		lua_pushstring(L, cityName.c_str());
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
// Aux Method used by lCities.
int CvLuaPlayer::lCitiesAux(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = NULL;

	int i = -1;
	lua_pushvalue(L, lua_upvalueindex(1));
	int t = lua_gettop(L);

	lua_rawgeti(L, t, 1);
	if(!lua_isnil(L, -1))
	{
		i = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	pkCity = (i == -1)? pkPlayer->firstCity(&i) : pkPlayer->nextCity(&i);

	lua_pushinteger(L, i);
	lua_rawseti(L, t, 1);

	if(pkCity)
	{
		CvLuaCity::Push(L, pkCity);
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
// Method for iterating through cities (behaves like pairs)
int CvLuaPlayer::lCities(lua_State* L)
{
	lua_createtable(L, 1, 0);
	lua_pushcclosure(L, lCitiesAux, 1);		/* generator, */
	lua_pushvalue(L, 1);					/* state (self) */
	return 2;
}
//------------------------------------------------------------------------------
//int getNumCities();
int CvLuaPlayer::lGetNumCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumCities);
}
//------------------------------------------------------------------------------
//int getNumCities();
int CvLuaPlayer::lGetNumPuppetCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumPuppetCities);
}
//------------------------------------------------------------------------------
//CyCity* getCity(int iID);
int CvLuaPlayer::lGetCityByID(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int id = lua_tointeger(L, 2);

	CvCity* pkCity = pkPlayer->getCity(id);

	//sometimes Lua city IDs are actually sequential indices
	//global IDs start at 1000
	if (!pkCity && id<1000)
	{
		if (id>0)
		{
			id--;
			pkCity = pkPlayer->nextCity(&id);
		}
		else
			pkCity = pkPlayer->firstCity(&id);
	}

	CvLuaCity::Push(L, pkCity);
	return 1;
}
//------------------------------------------------------------------------------
// Aux Method used by lUnits.
int CvLuaPlayer::lUnitsAux(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvUnit* pkUnit = NULL;

	lua_pushvalue(L, lua_upvalueindex(1));
	int t = lua_gettop(L);

	lua_rawgeti(L, t, 1);
	int i = -1;
	if(!lua_isnil(L, -1))
	{
		i = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	pkUnit = (i == -1)? pkPlayer->firstUnit(&i) : pkPlayer->nextUnit(&i);

	lua_pushinteger(L, i);
	lua_rawseti(L, t, 1);

	if(pkUnit)
	{
		CvLuaUnit::Push(L, pkUnit);
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
// Method for iterating through units (behaves like pairs)
int CvLuaPlayer::lUnits(lua_State* L)
{
	lua_createtable(L, 1, 0);
	lua_pushcclosure(L, lUnitsAux, 1);		/* generator, */
	lua_pushvalue(L, 1);					/* state (self)*/
	return 2;
}
//------------------------------------------------------------------------------
//CvUnit GetUnitByID();
int CvLuaPlayer::lGetUnitByID(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int id = lua_tointeger(L, 2);

	CvUnit* pUnit = pkPlayer->getUnit(id);

	//sometimes Lua unit IDs are actually sequential indices
	//global IDs start at 1000
	if (!pUnit && id<1000)
	{
		if (id>0)
		{
			id--;
			pUnit = pkPlayer->nextUnit(&id);
		}
		else
			pUnit = pkPlayer->firstUnit(&id);
	}

	CvLuaUnit::Push(L, pUnit);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumUnits();
int CvLuaPlayer::lGetNumUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumUnits);
}
//------------------------------------------------------------------------------
//int getNumUnits();
int CvLuaPlayer::lGetNumUnitsToSupply(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumUnitsToSupply);
}
//------------------------------------------------------------------------------
//int GetNumUnitsOfType();
int CvLuaPlayer::lGetNumUnitsOfType(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const bool bIncludeBeingTrained = luaL_optbool(L, 3, false);

	const int iResult = pkPlayer->GetNumUnitsOfType(eUnit, bIncludeBeingTrained);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumUnitPromotions();
int CvLuaPlayer::lGetNumUnitPromotions(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::GetNumUnitPromotions);
}
//------------------------------------------------------------------------------
//void AI_updateFoundValues(bool bStartingLoc);
int CvLuaPlayer::lAI_updateFoundValues(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::updatePlotFoundValues);
}
//------------------------------------------------------------------------------
//int AI_foundValue(int iX, int iY, int iMinUnitRange/* = -1*/, bool bStartingLoc/* = false*/);
int CvLuaPlayer::lAI_foundValue(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayer::getPlotFoundValue);
}

//------------------------------------------------------------------------------
//int getScoreHistory(int iTurn) const;
int CvLuaPlayer::lGetScoreHistory(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	unsigned int uiTurn = (unsigned int)luaL_checkint(L, 2);
	unsigned int uiDataSet = pkPlayer->getReplayDataSetIndex("REPLAYDATASET_SCORE");
	lua_pushinteger(L, pkPlayer->getReplayDataValue(uiDataSet, uiTurn));
	return 1;
}
//------------------------------------------------------------------------------
//int getEconomyHistory(int iTurn) const;
int CvLuaPlayer::lGetEconomyHistory(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	unsigned int uiTurn = (unsigned int)luaL_checkint(L, 2);
	unsigned int uiDataSet = pkPlayer->getReplayDataSetIndex("REPLAYDATASET_ECONOMY");
	lua_pushinteger(L, pkPlayer->getReplayDataValue(uiDataSet, uiTurn));
	return 1;
}
//------------------------------------------------------------------------------
//int getIndustryHistory(int iTurn) const;
int CvLuaPlayer::lGetIndustryHistory(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	unsigned int uiTurn = (unsigned int)luaL_checkint(L, 2);
	unsigned int uiDataSet = pkPlayer->getReplayDataSetIndex("REPLAYDATASET_INDUSTRY");
	lua_pushinteger(L, pkPlayer->getReplayDataValue(uiDataSet, uiTurn));
	return 1;
}
//------------------------------------------------------------------------------
//int getAgricultureHistory(int iTurn) const;
int CvLuaPlayer::lGetAgricultureHistory(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	unsigned int uiTurn = (unsigned int)luaL_checkint(L, 2);
	unsigned int uiDataSet = pkPlayer->getReplayDataSetIndex("REPLAYDATASET_AGRICULTURE");
	lua_pushinteger(L, pkPlayer->getReplayDataValue(uiDataSet, uiTurn));
	return 1;
}
//------------------------------------------------------------------------------
//int getPowerHistory(int iTurn) const;
int CvLuaPlayer::lGetPowerHistory(lua_State* /*L*/)
{
	return 0;
}
//------------------------------------------------------------------------------
//table[dataSetName][turn] GetReplayData(nil)
int CvLuaPlayer::lGetReplayData(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const unsigned int numDataSets = pkPlayer->getNumReplayDataSets();

	lua_createtable(L, 0, numDataSets);
	for(unsigned int uiDataSet = 0; uiDataSet < numDataSets; ++uiDataSet)
	{
		lua_pushstring(L, pkPlayer->getReplayDataSetName(uiDataSet));

		CvPlayer::TurnData data = pkPlayer->getReplayDataHistory(uiDataSet);

		lua_createtable(L, data.size() - 1, 1);

		for(CvPlayer::TurnData::iterator it = data.begin(); it != data.end(); ++it)
		{
			lua_pushinteger(L, (*it).second);
			lua_rawseti(L, -2, (*it).first);
		}

		lua_rawset(L, -3);
	}

	return 1;
}
//------------------------------------------------------------------------------
//void SetReplayDataValue(string DataSetName, int turn, int Value)
int CvLuaPlayer::lSetReplayDataValue(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const char* szDataSet = luaL_checkstring(L, 2);
	int iTurn = luaL_checkint(L, 3);
	int iValue = luaL_checkint(L, 4);

	unsigned int uiDataSet = pkPlayer->getReplayDataSetIndex(szDataSet);
	pkPlayer->setReplayDataValue(uiDataSet, iTurn, iValue);

	return 0;
}
//------------------------------------------------------------------------------
//string getScriptData() const;
int CvLuaPlayer::lGetScriptData(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->getScriptData().c_str());
	return 1;
}
//------------------------------------------------------------------------------
//void setScriptData(string szNewValue);
int CvLuaPlayer::lSetScriptData(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const char* strScriptData = lua_tostring(L, 2);

	pkPlayer->setScriptData(strScriptData);
	return 0;
}
//------------------------------------------------------------------------------
//int GetNumPlots();
int CvLuaPlayer::lGetNumPlots(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumPlots);
}
//------------------------------------------------------------------------------
//int GetNumPlotsBought();
int CvLuaPlayer::lGetNumPlotsBought(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumPlotsBought);
}
//------------------------------------------------------------------------------
//void SetNumPlotsBought(int iValue);
int CvLuaPlayer::lSetNumPlotsBought(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetNumPlotsBought);
}
//------------------------------------------------------------------------------
//void ChangeNumPlotsBought(int iChange);
int CvLuaPlayer::lChangeNumPlotsBought(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeNumPlotsBought);
}
//------------------------------------------------------------------------------
//int GetBuyPlotCost();
int CvLuaPlayer::lGetBuyPlotCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetBuyPlotCost);
}
//------------------------------------------------------------------------------
// int GetPlotDanger();
int CvLuaPlayer::lGetPlotDanger(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	lua_pushinteger(L, pkPlayer->GetPlotDanger(*pkPlot,false));
	return 1;
}
//------------------------------------------------------------------------------
//int getBuyPlotDistance();
int CvLuaPlayer::lGetBuyPlotDistance(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getBuyPlotDistance);
}
//------------------------------------------------------------------------------
//int getWorkPlotDistance();
int CvLuaPlayer::lGetWorkPlotDistance(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getWorkPlotDistance);
}
#if defined(MOD_TRAITS_CITY_WORKING) || defined(MOD_BUILDINGS_CITY_WORKING) || defined(MOD_POLICIES_CITY_WORKING) || defined(MOD_TECHS_CITY_WORKING)
//------------------------------------------------------------------------------
//int getCityWorkingChange();
int CvLuaPlayer::lGetCityWorkingChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCityWorkingChange);
}
//------------------------------------------------------------------------------
//void changeCityWorkingChange(int iChange);
int CvLuaPlayer::lChangeCityWorkingChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeCityWorkingChange);
}
#endif
#if defined(MOD_TRAITS_CITY_AUTOMATON_WORKERS) || defined(MOD_BUILDINGS_CITY_AUTOMATON_WORKERS) || defined(MOD_POLICIES_CITY_AUTOMATON_WORKERS) || defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
//------------------------------------------------------------------------------
//int getCityAutomatonWorkersChange();
int CvLuaPlayer::lGetCityAutomatonWorkersChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetCityAutomatonWorkersChange);
}
//------------------------------------------------------------------------------
//void changeCityAutomatonWorkersChange(int iChange);
int CvLuaPlayer::lChangeCityAutomatonWorkersChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeCityAutomatonWorkersChange);
}
#endif
//------------------------------------------------------------------------------
//void DoBeginDiploWithHuman();
int CvLuaPlayer::lDoBeginDiploWithHuman(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	pkPlayer->GetDiplomacyAI()->DoBeginDiploWithHuman();
	return 1;
}
//------------------------------------------------------------------------------
//void DoTradeScreenOpened();
int CvLuaPlayer::lDoTradeScreenOpened(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	pkPlayer->GetDealAI()->DoTradeScreenOpened();
	return 1;
}
//------------------------------------------------------------------------------
//void DoTradeScreenClosed();
int CvLuaPlayer::lDoTradeScreenClosed(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	bool bAIWasMakingOffer = lua_toboolean(L, 2);

	pkPlayer->GetDealAI()->DoTradeScreenClosed(bAIWasMakingOffer);
	return 1;
}
//------------------------------------------------------------------------------
//void GetApproachTowardsUsGuess(PlayerTypes ePlayer);
int CvLuaPlayer::lGetApproachTowardsUsGuess(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetVisibleApproachTowardsUs(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMajorCivApproach(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetCivApproach(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
//void IsWillAcceptPeaceWithPlayer(PlayerTypes ePlayer);
int CvLuaPlayer::lIsWillAcceptPeaceWithPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bWantsPeace = pkPlayer->GetDiplomacyAI()->IsWantsPeaceWithPlayer(ePlayer);

	lua_pushboolean(L, bWantsPeace);
	return 1;
}
//------------------------------------------------------------------------------
//void IsProtectingMinor(PlayerTypes ePlayer);
int CvLuaPlayer::lIsProtectingMinor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMinor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bProtecting = GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(pkPlayer->GetID());

	lua_pushboolean(L, bProtecting);
	return 1;
}
//------------------------------------------------------------------------------
//void IsDontSettleMessageTooSoon(PlayerTypes eWithPlayer);
int CvLuaPlayer::lIsDontSettleMessageTooSoon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsDontSettleMessageTooSoon(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
//void IsStopSpyingMessageTooSoon(PlayerTypes eWithPlayer);
int CvLuaPlayer::lIsStopSpyingMessageTooSoon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsStopSpyingMessageTooSoon(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}

//------------------------------------------------------------------------------
//void IsAskedToStopConverting(PlayerTypes eWithPlayer);
int CvLuaPlayer::lIsAskedToStopConverting(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bAsked = pkPlayer->GetDiplomacyAI()->IsPlayerAskedNotToConvert(eWithPlayer);

	lua_pushboolean(L, bAsked);
	return 1;
}

//------------------------------------------------------------------------------
//void IsAskedToStopDigging(PlayerTypes eWithPlayer);
int CvLuaPlayer::lIsAskedToStopDigging(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bAsked = pkPlayer->GetDiplomacyAI()->IsPlayerAskedNotToDig(eWithPlayer);

	lua_pushboolean(L, bAsked);
	return 1;
}

//------------------------------------------------------------------------------
//void IsDoFMessageTooSoon(PlayerTypes eWithPlayer);
int CvLuaPlayer::lIsDoFMessageTooSoon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsDoFMessageTooSoon(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
//void IsDoF(PlayerTypes eWithPlayer);
int CvLuaPlayer::lIsDoF(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsDoFAccepted(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetDoFCounter(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iCounter = pkPlayer->GetDiplomacyAI()->GetTurnsSinceBefriendedPlayer(eWithPlayer);

	lua_pushinteger(L, iCounter);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerDoFWithAnyFriend(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsPlayerDoFWithAnyFriend(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerDoFWithAnyEnemy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsPlayerDoFWithAnyEnemy(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerDenouncedFriend(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsPlayerDenouncedFriend(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerDenouncedEnemy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsPlayerDenouncedEnemy(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsUntrustworthyFriend(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes)lua_tointeger(L, 2);

	if (eOtherPlayer == NO_PLAYER)
	{
		lua_pushboolean(L, false);
		return 0;
	}

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsUntrustworthy(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumFriendsDenouncedBy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNumFriendsDenouncedBy();

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsFriendDenouncedUs(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsFriendDenouncedUs(ePlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetWeDenouncedFriendCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetWeDenouncedFriendCount();

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsFriendDeclaredWarOnUs(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsFriendDeclaredWarOnUs(ePlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetWeDeclaredWarOnFriendCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetWeDeclaredWarOnFriendCount();

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lCanRequestCoopWar(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eAllyPlayer = (PlayerTypes) lua_tointeger(L, 2);
	PlayerTypes eTargetPlayer = (PlayerTypes) lua_tointeger(L, 3);

	const bool bValue = pkPlayer->GetDiplomacyAI()->CanRequestCoopWar(eAllyPlayer, eTargetPlayer);

	lua_pushinteger(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCoopWarAcceptedState(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eAllyPlayer = (PlayerTypes) lua_tointeger(L, 2);
	PlayerTypes eTargetPlayer = (PlayerTypes) lua_tointeger(L, 3);

	const int iState = pkPlayer->GetDiplomacyAI()->GetCoopWarState(eAllyPlayer, eTargetPlayer);

	lua_pushinteger(L, iState);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumWarsFought(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iNum = pkPlayer->GetDiplomacyAI()->GetNumWarsFought(eWithPlayer);

	lua_pushinteger(L, iNum);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetLandDisputeLevel(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetLandDisputeLevel(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetVictoryDisputeLevel(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetVictoryDisputeLevel(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetWonderDisputeLevel(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetWonderDisputeLevel(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorCivDisputeLevel(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetMinorCivDisputeLevel(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetWarmongerThreat(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetWarmongerThreat(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsDemandEverMade(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsDemandMade(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumCiviliansReturnedToMe(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNumCiviliansReturnedToMe(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumLandmarksBuiltForMe(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNumLandmarksBuiltForMe(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumTimesCultureBombed(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNumTimesCultureBombed(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNegativeReligiousConversionPoints(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNegativeReligiousConversionPoints(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNegativeArchaeologyPoints(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNegativeArchaeologyPoints(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lHasOthersReligionInMostCities(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetReligions()->HasOthersReligionInMostCities(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenMilitaryPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenMilitaryPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
// Removed code but kept function for compatibility
int CvLuaPlayer::lIsPlayerIgnoredMilitaryPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredMilitaryPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenExpansionPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenExpansionPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerIgnoredExpansionPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredExpansionPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenBorderPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenBorderPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerIgnoredBorderPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredBorderPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenAttackCityStatePromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenAttackCityStatePromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerIgnoredAttackCityStatePromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredAttackCityStatePromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenBullyCityStatePromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenBullyCityStatePromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerIgnoredBullyCityStatePromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredBullyCityStatePromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenSpyPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenSpyPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerIgnoredSpyPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredSpyPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerForgivenForSpying(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerForgaveForSpying(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenNoConvertPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenNoConvertPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerIgnoredNoConvertPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredNoConvertPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenNoDiggingPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenNoDiggingPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerIgnoredNoDiggingPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerIgnoredNoDiggingPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerBrokenCoopWarPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerBrokenCoopWarPromise(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetOtherPlayerNumProtectedMinorsKilled(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetOtherPlayerNumProtectedMinorsKilled(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetOtherPlayerNumProtectedMinorsAttacked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetOtherPlayerNumProtectedMinorsAttacked(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
// int GetTurnsSincePlayerBulliedProtectedMinor(int iOtherPlayer);
// Returns MAX_TURNS_SAFE_ESTIMATE if OtherPlayer has never bullied a protected minor
int CvLuaPlayer::lGetTurnsSincePlayerBulliedProtectedMinor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	int iTurn = pkPlayer->GetDiplomacyAI()->GetOtherPlayerBulliedProtectedMinorTurn(eOtherPlayer);
	const int iValue = (iTurn != -1) ? (GC.getGame().getGameTurn() - iTurn) : -1;

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
// bool IsHasPlayerBulliedProtectedMinor(int iOtherPlayer);
int CvLuaPlayer::lIsHasPlayerBulliedProtectedMinor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = (pkPlayer->GetDiplomacyAI()->GetOtherPlayerProtectedMinorBullied(eOtherPlayer) != NO_PLAYER);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsDenouncedPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsDenouncedPlayer(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetDenouncedPlayerCounter(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetTurnsSinceDenouncedPlayer(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsDenouncingPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsDenouncingPlayer(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsDenounceMessageTooSoon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsDenounceMessageTooSoon(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerRecklessExpander(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerRecklessExpander(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetRecentTradeValue(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetRecentTradeValue(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCommonFoeValue(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetCommonFoeValue(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetRecentAssistValue(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetRecentAssistValue(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsHasPaidTributeTo(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsHasPaidTributeTo(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsNukedBy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsNukedBy(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsCapitalCapturedBy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsCapitalCapturedBy(eOtherPlayer, true, false);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAngryAboutProtectedMinorKilled(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsAngryAboutProtectedMinorKilled(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAngryAboutProtectedMinorAttacked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsAngryAboutProtectedMinorAttacked(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAngryAboutProtectedMinorBullied(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsAngryAboutProtectedMinorBullied(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAngryAboutSidedWithTheirProtectedMinor(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsAngryAboutSidedWithProtectedMinor(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumTimesRobbedBy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNumTimesRobbedBy(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumTimesIntrigueSharedBy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pkPlayer->GetDiplomacyAI()->GetNumTimesIntrigueSharedBy(eOtherPlayer);

	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lDoForceDoF(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	// Can't do this while at war!
	if (pkPlayer->IsAtWarWith(eOtherPlayer))
		return 1;

	pkPlayer->GetDiplomacyAI()->SetDoFAccepted(eOtherPlayer, true);
	GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->SetDoFAccepted(pkPlayer->GetID(), true);

	if(pkPlayer->GetDiplomacyAI()->GetDoFType(eOtherPlayer) == DOF_TYPE_ALLIES || GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetDoFType(pkPlayer->GetID()) == DOF_TYPE_ALLIES)
	{
		pkPlayer->GetDiplomacyAI()->SetDoFType(eOtherPlayer, DOF_TYPE_BATTLE_BROTHERS);
		GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->SetDoFType(pkPlayer->GetID(), DOF_TYPE_BATTLE_BROTHERS);
	}
	else if(pkPlayer->GetDiplomacyAI()->GetDoFType(eOtherPlayer) == DOF_TYPE_FRIENDS || GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetDoFType(pkPlayer->GetID()) == DOF_TYPE_FRIENDS) 
	{
		pkPlayer->GetDiplomacyAI()->SetDoFType(eOtherPlayer, DOF_TYPE_ALLIES);
		GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->SetDoFType(pkPlayer->GetID(), DOF_TYPE_ALLIES);
	}
	else if(pkPlayer->GetDiplomacyAI()->GetDoFType(eOtherPlayer) == DOF_TYPE_NEW || GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetDoFType(pkPlayer->GetID()) == DOF_TYPE_NEW)  
	{
		pkPlayer->GetDiplomacyAI()->SetDoFType(eOtherPlayer, DOF_TYPE_FRIENDS);
		GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->SetDoFType(pkPlayer->GetID(), DOF_TYPE_FRIENDS);
	}
	else if(pkPlayer->GetDiplomacyAI()->GetDoFType(eOtherPlayer) == DOF_TYPE_UNTRUSTWORTHY || GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetDoFType(pkPlayer->GetID()) == DOF_TYPE_UNTRUSTWORTHY)  
	{
		pkPlayer->GetDiplomacyAI()->SetDoFType(eOtherPlayer, DOF_TYPE_NEW);
		GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->SetDoFType(pkPlayer->GetID(), DOF_TYPE_NEW);
	}

	vector<PlayerTypes> v;
	v.push_back(eOtherPlayer);
	pkPlayer->GetDiplomacyAI()->DoReevaluatePlayers(v);

	vector<PlayerTypes> v2;
	v2.push_back(pkPlayer->GetID());
	GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->DoReevaluatePlayers(v2);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lDoForceDenounce(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	pkPlayer->GetDiplomacyAI()->DoDenouncePlayer(eOtherPlayer);

	// Show leader if active player is being denounced
	if(GC.getGame().getActivePlayer() == eOtherPlayer)
	{
		const char* strText = pkPlayer->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
		gDLL->GameplayDiplomacyAILeaderMessage(pkPlayer->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, strText, LEADERHEAD_ANIM_NEGATIVE);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsHasDefensivePact(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	bool bValue = false;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
		if(ePlayerLoop != eOtherPlayer && ePlayerLoop != NO_PLAYER && ePlayerLoop != pkPlayer->GetID())
		{
			if(GET_TEAM(pkPlayer->getTeam()).IsHasDefensivePact(GET_PLAYER(ePlayerLoop).getTeam()))
			{
				bValue = true;
				break;
			}
		}
	}

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsHasDefensivePactWithPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	lua_pushboolean(L, GET_TEAM(pkPlayer->getTeam()).IsHasDefensivePact(GET_PLAYER(eOtherPlayer).getTeam()));
	return 1;
}
int CvLuaPlayer::lGetNumTurnsMilitaryPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes)lua_tointeger(L, 2);
	int iValue = pkPlayer->GetDiplomacyAI()->GetPlayerMadeMilitaryPromise(eWithPlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaPlayer::lGetNumTurnsExpansionPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes)lua_tointeger(L, 2);
	int iValue = pkPlayer->GetDiplomacyAI()->GetPlayerMadeExpansionPromise(eWithPlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaPlayer::lGetNumTurnsBorderPromise(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes)lua_tointeger(L, 2);
	int iValue = pkPlayer->GetDiplomacyAI()->GetPlayerMadeBorderPromise(eWithPlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//void AddNotification()
int CvLuaPlayer::lAddNotification(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvNotifications* pNotifications = pkPlayer->GetNotifications();
	int notificationID = -1;
	if(pNotifications)
	{
		int iExtraData = -1;
		if(lua_gettop(L) >= 8)
			iExtraData = lua_tointeger(L, 8);

		notificationID = pNotifications->Add((NotificationTypes) lua_tointeger(L, 2),
		                                     lua_tostring(L, 3),
		                                     lua_tostring(L, 4),
		                                     lua_tointeger(L, 5),
		                                     lua_tointeger(L, 6),
		                                     lua_tointeger(L, 7),
		                                     iExtraData);
	}
	lua_pushinteger(L, notificationID);

	return 1;
}

//------------------------------------------------------------------------------
//void AddNotification()
int CvLuaPlayer::lAddNotificationName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvNotifications* pNotifications = pkPlayer->GetNotifications();
	int notificationID = -1;
	if(pNotifications)
	{
		int iExtraData = -1;
		if(lua_gettop(L) >= 8)
			iExtraData = lua_tointeger(L, 8);

		notificationID = pNotifications->AddByName(lua_tostring(L, 2),
		                                     lua_tostring(L, 3),
		                                     lua_tostring(L, 4),
		                                     lua_tointeger(L, 5),
		                                     lua_tointeger(L, 6),
		                                     lua_tointeger(L, 7),
		                                     iExtraData);
	}
	lua_pushinteger(L, notificationID);

	return 1;
}
//------------------------------------------------------------------------------
//void DismissNotification()
int CvLuaPlayer::lDismissNotification(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvNotifications* pNotifications = pkPlayer->GetNotifications();
	if (pNotifications) {
		int iIndex = lua_tointeger(L, 2);
		bool bUserInvoked = lua_toboolean(L, 3);
		
		pNotifications->Dismiss(iIndex, bUserInvoked);
	}

	return 0;
}

//------------------------------------------------------------------------------
//int GetNumNotifications();
int CvLuaPlayer::lGetNumNotifications(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetNotifications()->GetNumNotifications());
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetNotificationStr();
int CvLuaPlayer::lGetNotificationStr(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iIndex = lua_tointeger(L, 2);

	lua_pushstring(L, pkPlayer->GetNotifications()->GetNotificationStr(iIndex));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNotificationSummaryStr(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iIndex = lua_tointeger(L, 2);

	lua_pushstring(L, pkPlayer->GetNotifications()->GetNotificationSummary(iIndex));
	return 1;
}
//------------------------------------------------------------------------------
//int GetNotificationIndex();
int CvLuaPlayer::lGetNotificationIndex(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iIndex = lua_tointeger(L, 2);

	lua_pushinteger(L, pkPlayer->GetNotifications()->GetNotificationID(iIndex));
	return 1;
}
//------------------------------------------------------------------------------
//int GetNotificationTurn();
int CvLuaPlayer::lGetNotificationTurn(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iIndex = lua_tointeger(L, 2);

	lua_pushinteger(L, pkPlayer->GetNotifications()->GetNotificationTurn(iIndex));
	return 1;
}
//------------------------------------------------------------------------------
//int GetNotificationDismissed();
int CvLuaPlayer::lGetNotificationDismissed(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iIndex = lua_tointeger(L, 2);
	lua_pushboolean(L, pkPlayer->GetNotifications()->IsNotificationDismissed(iIndex));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetRecommendedWorkerPlots(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvUnit* pWorkerUnit = NULL;

	//Get first selected worker.
	CvEnumerator<ICvUnit1> selectedUnits(GC.GetEngineUserInterface()->GetSelectedUnits());
	while(selectedUnits.MoveNext())
	{
		auto_ptr<ICvUnit1> pUnit(selectedUnits.GetCurrent());
		if(pUnit.get() != NULL)
		{
			CvUnitEntry* pUnitEntry = GC.getUnitInfo(pUnit->GetUnitType());
			if(pUnitEntry && pUnitEntry->GetWorkRate() > 0)
			{
				pWorkerUnit = GC.UnwrapUnitPointer(pUnit.get());
				break;
			}
		}
	}

	//Early out
	if(pWorkerUnit == NULL)
		return 0;

	//fake the reachable plots, ignore all other workers
	map<CvUnit*, ReachablePlots> allplots;
	SPathFinderUserData data(pWorkerUnit, 0, 3);
	allplots[pWorkerUnit] = GC.GetPathFinder().GetPlotsInReach(pWorkerUnit->plot(), data);

	BuilderDirective aDirective = pkPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pWorkerUnit,allplots);
	if(aDirective.m_eDirective == BuilderDirective::NUM_DIRECTIVES)
		return 0;

	//don't look at me ... this is just for stupid lua
	const size_t cuiMaxDirectives = 3;
	const size_t cuiDirectiveSize = 1;

	lua_createtable(L, cuiMaxDirectives, 0);
	int iPositionIndex = lua_gettop(L);
	int i = 1;

	for(uint ui = 0; ui < cuiDirectiveSize && i < cuiMaxDirectives; ui++)
	{
		lua_createtable(L, 0, 2);
		CvLuaPlot::Push(L, GC.getMap().plot(aDirective.m_sX, aDirective.m_sY));
		lua_setfield(L, -2, "plot");
		lua_pushinteger(L, aDirective.m_eBuild);
		lua_setfield(L, -2, "buildType");

		lua_rawseti(L, iPositionIndex, i);
		i++;
	}

	return 1;
}

typedef CvWeightedVector<CvPlot*> WeightedPlotVector;

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetRecommendedFoundCityPlots(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvUnit* pFoundingUnit = NULL;

	//Get first selected unit that can found cities.
	CvEnumerator<ICvUnit1> selectedUnits(GC.GetEngineUserInterface()->GetSelectedUnits());
	while(selectedUnits.MoveNext())
	{
		auto_ptr<ICvUnit1> pUnit(selectedUnits.GetCurrent());
		if(pUnit.get() != NULL)
		{
			CvUnit* pkUnit = GC.UnwrapUnitPointer(pUnit.get());
			if(pkUnit != NULL && pkUnit->isFound())
			{
				pFoundingUnit = pkUnit;
				break;
			}
		}
	}

	if(pFoundingUnit == NULL)
		return 0;

	int iSettlerDistance;
	int iDistanceDropoff;
	int iSettlerX, iSettlerY;
	int iPathTurns;
	int iValue;
	int iDanger;

	// Get coordinates for settler's plot
	iSettlerX = pFoundingUnit->getX();
	iSettlerY = pFoundingUnit->getY();

	int iEvalDistance = GC.getSETTLER_EVALUATION_DISTANCE();
	int iDistanceDropoffMod = GC.getSETTLER_DISTANCE_DROPOFF_MODIFIER();
	int iBeginSearchX = iSettlerX - iEvalDistance;
	int iBeginSearchY = iSettlerY - iEvalDistance;
	int iEndSearchX   = iSettlerX + iEvalDistance;
	int iEndSearchY   = iSettlerY + iEvalDistance;

	CvMap& kMap = GC.getMap();

	TeamTypes eUnitTeam = pFoundingUnit->getTeam();

	CvCity* pCapital = pkPlayer->getCapitalCity();
	int iCapArea = NULL;
	if(pCapital)
	{
		iCapArea = pCapital->getArea();
	}

	WeightedPlotVector aBestPlots;
	aBestPlots.reserve((iEvalDistance+1) * 2);

	for(int iPlotX = iBeginSearchX; iPlotX != iEndSearchX; iPlotX++)
	{
		for(int iPlotY = iBeginSearchY; iPlotY != iEndSearchY; iPlotY++)
		{
			CvPlot* pPlot = kMap.plot(iPlotX, iPlotY);
			if(!pPlot)
			{
				continue;
			}

			//if (!pPlot->isVisible(pUnit->getTeam(), false /*bDebug*/))
			if(!pPlot->isRevealed(eUnitTeam))
			{
				continue;
			}

			// Can't actually found here!
			if(!pkPlayer->canFoundCity(iPlotX, iPlotY))
			{
				continue;
			}

			//// This operation is just for settling on the same continent as the capital
			//if(pCapital && pPlot->getArea() != iCapArea)
			//{
			//	continue;
			//}

			// Do we have to check if this is a safe place to go?
			if(!pPlot->isVisibleEnemyUnit(pkPlayer->GetID()))
			{
				iSettlerDistance = plotDistance(iPlotX, iPlotY, iSettlerX, iSettlerY);

				iValue = pkPlayer->getPlotFoundValue(iPlotX, iPlotY);

				iDistanceDropoff = (iDistanceDropoffMod * iSettlerDistance) / iEvalDistance;
				iValue = iValue * (100 - iDistanceDropoff) / 100;
				iDanger = pkPlayer->GetPlotDanger(*pPlot,false);
				if(iDanger < 1000)
				{
					iValue = ((1000 - iDanger) * iValue) / 1000;

					aBestPlots.push_back(pPlot, iValue);
				}
			}
		}
	}

	int iReturnSize = 0;
	int iFailLimit = 20;		// Paths can be really slow to create, bail if we fail too many times.
	#define MAX_RECCOMEND_RETURN 3
	CvPlot* aPlots[MAX_RECCOMEND_RETURN];

	uint uiListSize;
	if ((uiListSize = aBestPlots.size()) > 0)
	{
		aBestPlots.SortItems();	// highest score will be first.
		for (uint i = 0; i < uiListSize; ++i )	
		{
			CvPlot* pPlot = aBestPlots.GetElement(i);
			bool bCanFindPath = pFoundingUnit->GeneratePath(pPlot, 0, 12, &iPathTurns);
			if(bCanFindPath)
			{
				aPlots[iReturnSize] = pPlot;
				++iReturnSize;
				if (iReturnSize == MAX_RECCOMEND_RETURN)
					break;
			}
			else
			{
				if (iFailLimit-- == 0)
					break;
			}
		}
	}

	lua_createtable(L, iReturnSize, 0);
	if (iReturnSize > 0)
	{
		int iPositionIndex = lua_gettop(L);
		for(int i = 0; i < iReturnSize; i++)
		{
			CvLuaPlot::Push(L, aPlots[i]);
			lua_rawseti(L, iPositionIndex, i + 1);
		}
	}

	return 1;
}


//------------------------------------------------------------------------------
int CvLuaPlayer::lGetUnimprovedAvailableLuxuryResource(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	CvPlot* pResultPlot = NULL;

	const PlotIndexContainer& aiPlots = pkPlayer->GetPlots();
	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if (!pPlot)
		{
			continue;
		}

		// check to see if a resource is here. If not, bail out!
		ResourceTypes eResource = pPlot->getResourceType(pkPlayer->getTeam());
		if(eResource == NO_RESOURCE)
		{
			continue;
		}

		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource == NULL)
		{
			continue;
		}

		// Is this a Luxury Resource?
		if(pkResource->getResourceUsage() != RESOURCEUSAGE_LUXURY)
		{
			continue;
		}

		if(pkPlayer->getNumResourceTotal(eResource) > 0)
		{
			continue;
		}

		// if the resource is already improved
		ImprovementTypes eExistingPlotImprovement = pPlot->getImprovementType();
		if(eExistingPlotImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eExistingPlotImprovement);
			if(pkImprovement != NULL)
			{
				if (pkImprovement->IsExpandedImprovementResourceTrade(eResource))
				{
					continue;
				}
			}
		}

		// see if we can improve the resource
		for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
		{
			BuildTypes eBuild = (BuildTypes)iBuildIndex;
			CvBuildInfo* buildInfo = GC.getBuildInfo(eBuild);
			if(buildInfo == NULL)
				continue;

			ImprovementTypes eImprovement = (ImprovementTypes)buildInfo->getImprovement();
			if(eImprovement == NO_IMPROVEMENT)
			{
				continue;
			}

			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
			if(pkImprovementInfo == NULL)
			{
				continue;
			}

			if (!pkImprovementInfo->IsExpandedImprovementResourceTrade(eResource))
			{
				continue;
			}

			if(!pkPlayer->canBuild(pPlot, eBuild, false, true /*bTestVisible*/, false /*bTestGold*/))
			{
				continue;
			}

			int iBuildTurnsLeft = pPlot->getBuildTurnsLeft(eBuild, pkPlayer->GetID(), 0, 0);
			if(iBuildTurnsLeft > 0 && iBuildTurnsLeft < 4000)
			{
				continue;
			}

			pResultPlot = pPlot;
			break;
		}

		if(pResultPlot != NULL)
		{
			break;
		}
	}

	if(pResultPlot)
	{
		CvLuaPlot::Push(L, pResultPlot);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAnyPlotImproved(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const PlotIndexContainer& aiPlots = pkPlayer->GetPlots();

	bool bResult = false;

	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if(!pPlot)
		{
			continue;
		}

		if(pPlot->getPlotCity())
		{
			continue;
		}

		if(pPlot->getImprovementType() != NO_IMPROVEMENT || pPlot->getRouteType() != NO_ROUTE)
		{
			bResult = true;
			break;
		}
	}

	lua_pushboolean(L, bResult);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPlayerVisiblePlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlayerAI* pkPlayer2 = CvLuaPlayer::GetInstance(L, 2);

	const PlotIndexContainer& aiPlots = pkPlayer->GetPlots();

	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if(!pPlot)
		{
			continue;
		}

		if(pPlot->isRevealed(pkPlayer2->getTeam()))
		{
			CvLuaPlot::Push(L, pPlot);
			return 1;
		}
	}

	lua_pushnil(L);
	return 1;
}


//------------------------------------------------------------------------------
//bool GetEverPoppedGoody (void); // has this player ever popped a goody hut
int CvLuaPlayer::lGetEverPoppedGoody(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushboolean(L, pkPlayer->GetEverPoppedGoody());
	return 1;
}
//------------------------------------------------------------------------------
// bool CanAccessGoody (void); // can any of the player's units access any of the goody huts
int CvLuaPlayer::lGetClosestGoodyPlot(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pPlot = pkPlayer->GetClosestGoodyPlot();

	if(pPlot)
	{
		CvLuaPlot::Push(L, pPlot);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAnyGoodyPlotAccessible(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pPlot = pkPlayer->GetClosestGoodyPlot(true /*bStopAfterFindingFirst*/);

	lua_pushboolean(L, pPlot ? true : false);

	return 1;
}

//------------------------------------------------------------------------------
// bool GetPlotHasOrder (CvPlot* Plot); // are any of the player's units directed to this plot?
int CvLuaPlayer::lGetPlotHasOrder(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	lua_pushboolean(L, pkPlayer->GetPlotHasOrder(pkPlot));
	return 1;
}
//------------------------------------------------------------------------------
// bool GetAnyUnitHasOrderToGoody (void);
int CvLuaPlayer::lGetAnyUnitHasOrderToGoody(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushboolean(L, pkPlayer->GetAnyUnitHasOrderToGoody());
	return 1;
}
//------------------------------------------------------------------------------
// bool GetEverTrainedBuilder (void);
int CvLuaPlayer::lGetEverTrainedBuilder(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushboolean(L, pkPlayer->GetEverTrainedBuilder());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumFreeTechs(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushnumber(L, pkPlayer->GetNumFreeTechs());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetNumFreeTechs(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumTechs = lua_tointeger(L, 2);

	pkPlayer->SetNumFreeTechs(iNumTechs);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumFreePolicies(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushnumber(L, pkPlayer->GetNumFreePolicies());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetNumFreePolicies(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumPolicies = lua_tointeger(L, 2);

	pkPlayer->SetNumFreePolicies(iNumPolicies);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeNumFreePolicies(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumPolicies = lua_tointeger(L, 2);

	pkPlayer->ChangeNumFreePolicies(iNumPolicies);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumFreeTenets(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushnumber(L, pkPlayer->GetNumFreeTenets());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetNumFreeTenets(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumTenets = lua_tointeger(L, 2);
	const bool bCountAsFreePolicies = lua_toboolean(L, 3);

	pkPlayer->SetNumFreeTenets(iNumTenets, bCountAsFreePolicies);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeNumFreeTenets(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumTenets = lua_tointeger(L, 2);
	const bool bCountAsFreePolicies = lua_toboolean(L, 3);

	pkPlayer->ChangeNumFreeTenets(iNumTenets, bCountAsFreePolicies);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumFreeGreatPeople(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushnumber(L, pkPlayer->GetNumFreeGreatPeople());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetNumFreeGreatPeople(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumGreatPeople = lua_tointeger(L, 2);

	pkPlayer->SetNumFreeGreatPeople(iNumGreatPeople);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeNumFreeGreatPeople(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumGreatPeople = lua_tointeger(L, 2);

	pkPlayer->ChangeNumFreeGreatPeople(iNumGreatPeople);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumMayaBoosts(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushnumber(L, pkPlayer->GetNumMayaBoosts());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetNumMayaBoosts(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumBoosts = lua_tointeger(L, 2);

	pkPlayer->SetNumMayaBoosts(iNumBoosts);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeNumMayaBoosts(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumBoosts = lua_tointeger(L, 2);

	pkPlayer->ChangeNumMayaBoosts(iNumBoosts);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumFaithGreatPeople(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushnumber(L, pkPlayer->GetNumFaithGreatPeople());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lSetNumFaithGreatPeople(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumGreatPeople = lua_tointeger(L, 2);

	pkPlayer->SetNumFaithGreatPeople(iNumGreatPeople);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lChangeNumFaithGreatPeople(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iNumGreatPeople = lua_tointeger(L, 2);

	pkPlayer->ChangeNumFaithGreatPeople(iNumGreatPeople);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetUnitBaktun(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	lua_pushnumber(L, pkPlayer->GetPlayerTraits()->GetUnitBaktun(eUnit));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsFreeMayaGreatPersonChoice(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, pkPlayer->GetPlayerTraits()->IsFreeMayaGreatPersonChoice());
	}
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsProphetValid(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, pkPlayer->GetPlayerTraits()->IsProphetValid());
	}
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lHasReceivedNetTurnComplete(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, gDLL->HasReceivedTurnComplete(pkPlayer->GetID()));
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTraitGoldenAgeCombatModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetGoldenAgeCombatModifier());
	}
	return 1;
}
#if defined(MOD_BALANCE_CORE)
int CvLuaPlayer::lGetTraitConquestOfTheWorldCityAttackMod(lua_State* L)
{
	int iRtnValue = 0;

	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	if(pkPlot && pkPlayer)
	{
		if(pkPlayer->isGoldenAge())
		{
			CvCity* pPlotCity = pkPlot->getOwningCity();
			if(pPlotCity)
			{
				if(!GET_PLAYER(pPlotCity->getOwner()).isMinorCiv())
				{
					iRtnValue = pkPlayer->GetPlayerTraits()->GetConquestOfTheWorldCityAttack();
				}
			}
		}
	}
	lua_pushinteger(L, iRtnValue);
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTraitCityStateCombatModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetCityStateCombatModifier());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTraitGreatGeneralExtraBonus(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetGreatGeneralExtraBonus());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTraitGreatScientistRateModifier(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushinteger(L, pkPlayer->GetPlayerTraits()->GetGreatScientistRateModifier());
	}
	return 1;
}
#if defined(MOD_TRAITS_ANY_BELIEF)
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsTraitAnyBelief(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushboolean(L, pkPlayer->GetPlayerTraits()->IsAnyBelief());
	return 1;
}
#endif
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsBullyAnnex(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, pkPlayer->GetPlayerTraits()->IsBullyAnnex());
	}
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsTraitBonusReligiousBelief(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, pkPlayer->GetPlayerTraits()->IsBonusReligiousBelief());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetHappinessFromLuxury(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
		int iLuxuryHappiness = pkPlayer->GetHappinessFromLuxury(eResource);
		if (iLuxuryHappiness > 0) 
		{
			iLuxuryHappiness += pkPlayer->GetExtraHappinessPerLuxury();
		}
		lua_pushinteger(L, iLuxuryHappiness);
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsAbleToAnnexCityStates(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, pkPlayer->IsAbleToAnnexCityStates());
	}
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsDiplomaticMarriage(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, pkPlayer->IsDiplomaticMarriage());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsGPWLTKD(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if (pkPlayer && pkPlayer->GetPlayerTraits()->IsGPWLTKD())
	{
		lua_pushboolean(L, true);
		return 1;
	}
	else if (pkPlayer && pkPlayer->GetPlayerTraits()->IsGreatWorkWLTKD())
	{
		lua_pushboolean(L, true);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsCarnaval(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		if(pkPlayer->GetPlayerTraits()->GetWLTKDGATimer() > 0)
		{
			lua_pushboolean(L, true);
		}
		else
		{
			lua_pushboolean(L, false);
		}
	}
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsUsingMayaCalendar(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushboolean(L, pkPlayer->GetPlayerTraits()->IsUsingMayaCalendar());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMayaCalendarString(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushstring(L, pkPlayer->GetPlayerTraits()->GetMayaCalendarString());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMayaCalendarLongString(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		lua_pushstring(L, pkPlayer->GetPlayerTraits()->GetMayaCalendarLongString());
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetExtraBuildingHappinessFromPolicies(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			BuildingClassTypes eBuildingClass = pkBuildingInfo->GetBuildingClassType();

			int iExtraHappiness = 0;

			for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
			{
				const PolicyTypes ePolicy = static_cast<PolicyTypes>(iPolicyLoop);
				CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
				if(pkPolicyInfo)
				{
					if(pkPlayer->GetPlayerPolicies()->HasPolicy(ePolicy) && !pkPlayer->GetPlayerPolicies()->IsPolicyBlocked(ePolicy))
					{
						iExtraHappiness += pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass);
					}
				}
			}

			lua_pushinteger(L, iExtraHappiness);
			return 1;
		}
	}

	//BUG: This can't be right...
	lua_pushinteger(L, -1);
	return 0;
}
#if defined(MOD_BALANCE_CORE_POLICIES)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetExtraYieldWorldWonder(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{

		int iExtraYield = 0;
		const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);
		const YieldTypes eYieldType = (YieldTypes)luaL_checkint(L, 3);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			if(::isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
			{
#if defined(MOD_API_UNIFIED_YIELDS)
				iExtraYield += pkPlayer->GetPlayerTraits()->GetYieldChangeWorldWonder(eYieldType);
#endif
				for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
				{
					const PolicyTypes ePolicy = static_cast<PolicyTypes>(iPolicyLoop);
					CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
					if(pkPolicyInfo)
					{
						if(pkPlayer->GetPlayerPolicies()->HasPolicy(ePolicy) && !pkPlayer->GetPlayerPolicies()->IsPolicyBlocked(ePolicy))
						{
							iExtraYield += pkPolicyInfo->GetYieldChangeWorldWonder(eYieldType);
						}
					}
				}
			}

			lua_pushinteger(L, iExtraYield);
			return 1;
		}
	}

	//BUG: This can't be right...
	lua_pushinteger(L, -1);
	return 0;
}
#endif

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNextCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pCurrentCity = CvLuaCity::GetInstance(L, 2);
	CvCity* pNextCity = pkPlayer->nextCity(pCurrentCity);
	CvLuaCity::Push(L, pNextCity);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPrevCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pCurrentCity = CvLuaCity::GetInstance(L, 2);
	CvCity* pPrevCity = pkPlayer->nextCity(pCurrentCity,true);
	CvLuaCity::Push(L, pPrevCity);
	return 1;
}

//------------------------------------------------------------------------------
//int GetFreePromotionCount() const;
int CvLuaPlayer::lGetFreePromotionCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iPromotionIndex = lua_tointeger(L, 2);
	int iResult = pkPlayer->GetFreePromotionCount((PromotionTypes)iPromotionIndex);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int IsFreePromotion() const;
int CvLuaPlayer::lIsFreePromotion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iPromotionIndex = lua_tointeger(L, 2);
	bool bResult = pkPlayer->IsFreePromotion((PromotionTypes)iPromotionIndex);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//void ChangeFreePromotionCount(PromotionTypes ePromotion, int iChange);
int CvLuaPlayer::lChangeFreePromotionCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iPromotionIndex = lua_tointeger(L, 2);
	const int iValue = lua_tointeger(L, 3);
	pkPlayer->ChangeFreePromotionCount((PromotionTypes)iPromotionIndex, iValue);
	return 1;
}

//------------------------------------------------------------------------------
//CvString GetEmbarkedGraphicOverride() const
int CvLuaPlayer::lGetEmbarkedGraphicOverride(lua_State* L)
{
	CvPlayer* pPlayer = GetInstance(L);
	lua_pushstring(L, pPlayer->GetEmbarkedGraphicOverride());
	return 1;
}

//------------------------------------------------------------------------------
//void SetEmbarkedGraphicOverride(CvString szGraphic)
int CvLuaPlayer::lSetEmbarkedGraphicOverride(lua_State* L)
{
	CvPlayer* pPlayer = GetInstance(L);
	const CvString szGraphic = lua_tostring(L, 2);

	pPlayer->SetEmbarkedGraphicOverride(szGraphic);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lAddTemporaryDominanceZone(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iX = lua_tointeger(L, 2);
	const int iY = lua_tointeger(L, 3);

	// Notify tactical AI to focus on this area
	pkPlayer->GetTacticalAI()->AddFocusArea( GC.getMap().plot(iX,iY), 2, GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() );
	return 1;
}
//------------------------------------------------------------------------------
int  CvLuaPlayer::lGetNaturalWonderYieldModifier(lua_State* L)
{
	int iYieldModifier = 0;
	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		CvPlayerTraits* pkPlayerTraits = pkPlayer->GetPlayerTraits();
		if(pkPlayerTraits)
		{
			iYieldModifier = pkPlayerTraits->GetNaturalWonderYieldModifier();
		}
	}

	lua_pushinteger(L, iYieldModifier);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPolicyBuildingClassYieldModifier(lua_State* L)
{
	const BuildingClassTypes eBuildingClass = (BuildingClassTypes)luaL_checkint(L, 2);
	const YieldTypes eYieldType = (YieldTypes)luaL_checkint(L, 3);

	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		int modifier = pkPlayer->GetPlayerPolicies()->GetBuildingClassYieldModifier(eBuildingClass, eYieldType);
		modifier += pkPlayer->GetBuildingClassYieldModifier(eBuildingClass, eYieldType);
		lua_pushinteger(L, modifier);

		return 1;
	}

	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPolicyBuildingClassYieldChange(lua_State* L)
{
	const BuildingClassTypes eBuildingClass = (BuildingClassTypes)luaL_checkint(L, 2);
	const YieldTypes eYieldType = (YieldTypes)luaL_checkint(L, 3);

	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		int modifier = pkPlayer->GetPlayerPolicies()->GetBuildingClassYieldChange(eBuildingClass, eYieldType);
		lua_pushinteger(L, modifier);

		return 1;
	}

	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPolicyEspionageModifier(lua_State* L)
{
	const PolicyTypes iIndex = (PolicyTypes)lua_tointeger(L, 2);
	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(iIndex);
	CvAssertMsg(pkPolicyInfo, "pkPolicyInfo is null!");
	if (!pkPolicyInfo)
	{
		return 0;
	}

	lua_pushinteger(L, pkPolicyInfo->GetStealTechSlowerModifier());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPolicyEspionageCatchSpiesModifier(lua_State* L)
{
	const PolicyTypes iIndex = (PolicyTypes)lua_tointeger(L, 2);
	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(iIndex);
	CvAssertMsg(pkPolicyInfo, "pkPolicyInfo is null!");
	if (!pkPolicyInfo)
	{
		return 0;
	}

	lua_pushinteger(L, pkPolicyInfo->GetCatchSpiesModifier());
	return 1;
}

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPolicyConversionModifier(lua_State* L)
{
	const PolicyTypes iIndex = (PolicyTypes)lua_tointeger(L, 2);
	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(iIndex);
	CvAssertMsg(pkPolicyInfo, "pkPolicyInfo is null!");
	if (!pkPolicyInfo)
	{
		return 0;
	}

	lua_pushinteger(L, pkPolicyInfo->GetConversionModifier());
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPlayerBuildingClassYieldChange(lua_State* L)
{
	const BuildingClassTypes eBuildingClass = (BuildingClassTypes)luaL_checkint(L, 2);
	const YieldTypes eYieldType = (YieldTypes)luaL_checkint(L, 3);

	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		int iChange = pkPlayer->GetBuildingClassYieldChange(eBuildingClass, eYieldType) + pkPlayer->GetPlayerTraits()->GetBuildingClassYieldChange(eBuildingClass, eYieldType);
		lua_pushinteger(L, iChange);

		return 1;
	}

	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPlayerBuildingClassHappiness(lua_State* L)
{
	const BuildingClassTypes eOtherBuildingClass = (BuildingClassTypes)luaL_checkint(L, 2);

	CvPlayer* pkPlayer = GetInstance(L);
	if (pkPlayer)
	{
		int iChange = 0;

		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingClassTypes eParentBuildingClass = (BuildingClassTypes) iI;

			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eParentBuildingClass);
			if (!pkBuildingClassInfo)
				continue;

			if (MOD_BUILDINGS_THOROUGH_PREREQUISITES || pkPlayer->GetPlayerTraits()->IsKeepConqueredBuildings())
			{
				if (pkPlayer->getBuildingClassCount(eParentBuildingClass) > 0)
				{
					int iLoop = 0;

					for (const CvCity* pLoopCity = pkPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iLoop))
					{
						if (pLoopCity->HasBuildingClass(eParentBuildingClass))
						{
							BuildingTypes eParentBuilding = pLoopCity->GetCityBuildings()->GetBuildingTypeFromClass(eParentBuildingClass);
							if (eParentBuilding != NO_BUILDING)
							{
								CvBuildingEntry* pkParentBuilding = GC.getBuildingInfo(eParentBuilding);
								if (pkParentBuilding)
								{
									iChange += pkParentBuilding->GetBuildingClassHappiness(eOtherBuildingClass);
								}
							}
						}
					}

				}
			}
			else
			{
				BuildingTypes eParentBuilding = (BuildingTypes)pkPlayer->getCivilizationInfo().getCivilizationBuildings(eParentBuildingClass);
				if (eParentBuilding != NO_BUILDING && pkPlayer->countNumBuildings(eParentBuilding) > 0)
				{
					CvBuildingEntry* pkParentBuilding = GC.getBuildingInfo(eParentBuilding);
					if (pkParentBuilding)
					{
						iChange += pkParentBuilding->GetBuildingClassHappiness(eOtherBuildingClass) * pkPlayer->getNumBuildings(eParentBuilding);
					}
				}
			}
		}

		lua_pushinteger(L, iChange);

		return 1;
	}

	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lWasResurrectedBy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResurrected = pkPlayer->GetDiplomacyAI()->WasResurrectedBy(eWithPlayer);

	lua_pushboolean(L, bResurrected);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lWasResurrectedThisTurnBy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResurrected = pkPlayer->GetDiplomacyAI()->WasResurrectedThisTurnBy(eWithPlayer);

	lua_pushboolean(L, bResurrected);
	return 1;
}
//------------------------------------------------------------------------------
#if !defined(MOD_EVENTS_DIPLO_MODIFIERS)
struct Opinion
{
	Localization::String m_str;
	int m_iValue;
};
#endif

struct OpinionEval
{
	bool operator()(Opinion const& a, Opinion const& b) const
	{
		return a.m_iValue < b.m_iValue;
	}
};

int CvLuaPlayer::lGetOpinionTable(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	CvDiplomacyAI* pDiplo = pkPlayer->GetDiplomacyAI();

	// Initialize variables
	std::vector<Opinion> aOpinions;
	int iValue = 0;
	int iTempValue = 0;

	bool bHuman = pkPlayer->isHuman();
	bool bTeammate = pDiplo->IsTeammate(ePlayer);
	bool bShowAllModifiers = GC.getGame().IsShowAllOpinionModifiers();
	bool bShowAllValues = bHuman ? false : GC.getGame().IsShowAllOpinionValues();
	bool bHideDisputes = bShowAllModifiers ? false : pDiplo->ShouldHideDisputeMods(ePlayer);
	bool bHideNegatives = bShowAllModifiers ? false : pDiplo->ShouldHideNegativeMods(ePlayer);
	bool bPretendNoDisputes = bHideDisputes && bHideNegatives;
	bool bObserver = GET_PLAYER(ePlayer).isObserver() || !pkPlayer->isMajorCiv() || !GET_PLAYER(ePlayer).isMajorCiv() || !pkPlayer->isAlive() || !GET_PLAYER(ePlayer).isAlive();
	bool bUNActive = GC.getGame().IsUnitedNationsActive();
	bool bJustMet = GC.getGame().IsDiploDebugModeEnabled() ? false : (GET_TEAM(pkPlayer->getTeam()).GetTurnsSinceMeetingTeam(GET_PLAYER(ePlayer).getTeam()) == 0); // Don't display certain modifiers if we just met them

	CivApproachTypes eSurfaceApproach = pDiplo->GetSurfaceApproach(ePlayer);
	CivApproachTypes eTrueApproach = pDiplo->GetCivApproach(ePlayer);

	//--------------------------------//
	// [PART 1: SPECIAL INDICATORS]	  //
	//--------------------------------//

	if (!bObserver)
	{
		// Gone to war in the past?
		if (!pDiplo->IsAtWar(ePlayer) && pDiplo->GetNumWarsFought(ePlayer) > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			CvString str;

			if (bHuman)
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_PAST_WAR_BAD").toUTF8();
			}
			// Do not display this if AI is passive or a vassal/master of the player
			else if (!pDiplo->IsVassal(ePlayer) && !pDiplo->IsMaster(ePlayer) && !GC.getGame().IsAIPassiveTowardsHumans())
			{
				if (pDiplo->IsActHostileTowardsHuman(ePlayer))
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_PAST_WAR_BAD").toUTF8();
				}
				else
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_PAST_WAR_NEUTRAL").toUTF8();
				}
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		// Special indicators
		if (!bHuman && !bTeammate)
		{
			// Debug mode approach reveal
			if (GC.getGame().IsDiploDebugModeEnabled())
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 0;
				CvString str;

				switch (eTrueApproach)
				{
				case CIV_APPROACH_WAR:
					if (pDiplo->IsAtWar(ePlayer))
					{
						str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_WAR").toUTF8();
					}
					else
					{
						str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_PLANNING_WAR").toUTF8();
					}
					break;
				case CIV_APPROACH_HOSTILE:
					str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_HOSTILE").toUTF8();
					break;
				case CIV_APPROACH_DECEPTIVE:
					str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_DECEPTIVE").toUTF8();
					break;
				case CIV_APPROACH_GUARDED:
					str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_GUARDED").toUTF8();
					break;
				case CIV_APPROACH_AFRAID:
					str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_AFRAID").toUTF8();
					break;
				case CIV_APPROACH_FRIENDLY:
					str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_FRIENDLY").toUTF8();
					break;
				default:
					str = Localization::Lookup("TXT_KEY_DIPLO_REAL_APPROACH_NEUTRAL").toUTF8();
					break;
				}

				kOpinion.m_str = str;
				aOpinions.push_back(kOpinion);
			}

			// Untrustworthy friend?
			if (!bJustMet && !pDiplo->IsAlwaysAtWar(ePlayer) && pDiplo->IsUntrustworthy(ePlayer))
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 0;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_UNTRUSTWORTHY_FRIEND");
				aOpinions.push_back(kOpinion);
			}

			// Base opinion score?
			iValue = pDiplo->GetBaseOpinionScore(ePlayer);
			if (iValue != 0 && !pDiplo->IsAlwaysAtWar(ePlayer) && GC.getGame().IsShowBaseHumanOpinion())
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				CvString str;

				if (iValue >= /*30*/ GC.getOPINION_THRESHOLD_COMPETITOR())
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VERY_BAD_BASE_OPINION").toUTF8();
				}
				else if (iValue > 0)
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_BAD_BASE_OPINION").toUTF8();
				}
				else if (iValue <= /*-30*/ GC.getOPINION_THRESHOLD_FAVORABLE())
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VERY_GOOD_BASE_OPINION").toUTF8();
				}
				else
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_GOOD_BASE_OPINION").toUTF8();
				}

				kOpinion.m_str = str;
				aOpinions.push_back(kOpinion);
			}

			// Gandhi? :)
			if (GC.getGame().IsNuclearGandhiEnabled() && pDiplo->IsAtWar(ePlayer) && pkPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
			{
				if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsNukedBy(pkPlayer->GetID()) || pkPlayer->getNumNukeUnits() > 0)
				{
					Opinion kOpinion;
					kOpinion.m_iValue = 9999;
					kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NUCLEAR_GANDHI");
					aOpinions.push_back(kOpinion);
				}
			}
		}
	}

	//--------------------------------//
	// [PART 2A: HUMANS]			  //
	//--------------------------------//

	if (bHuman && !bObserver)
	{
		// DoF?
		if (pDiplo->IsDoFAccepted(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -9;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DOF");
			aOpinions.push_back(kOpinion);
		}
		// DoF with same players?
		if (pDiplo->IsPlayerDoFWithAnyFriend(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -8;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_DOF");
			aOpinions.push_back(kOpinion);
		}
		// DoF with enemy?
		if (pDiplo->IsPlayerDoFWithAnyEnemy(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = bTeammate ? 0 : 13;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DOF_WITH_ENEMY");
			aOpinions.push_back(kOpinion);
		}
		// Denounced same player?
		if (pDiplo->IsPlayerDenouncedEnemy(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -11;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_ENEMY");
			aOpinions.push_back(kOpinion);
		}
		// Denounced a friend?
		if (pDiplo->IsPlayerDenouncedFriend(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = bTeammate ? 0 : 11;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIEND");
			aOpinions.push_back(kOpinion);
		}
		// DP?
		if (pDiplo->IsHasDefensivePact(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -20;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DP");
			aOpinions.push_back(kOpinion);
		}
		// DP with same players?
		if (pDiplo->IsPlayerDPWithAnyFriend(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -15;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DP_MUTUAL");
			aOpinions.push_back(kOpinion);
		}
		// DP with enemy?
		if (pDiplo->IsPlayerDPWithAnyEnemy(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 15;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DP_WITH_ENEMY");
			aOpinions.push_back(kOpinion);
		}
		// Research Agreement?
		if (pDiplo->IsHasResearchAgreement(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -5;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MADE_RESEARCH_AGREEMENT");
			aOpinions.push_back(kOpinion);
		}
		// Currently trading?
		if (GC.getGame().GetGameDeals().IsReceivingItemsFromPlayer(pkPlayer->GetID(), ePlayer, true))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -40;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_PARTNER");
			aOpinions.push_back(kOpinion);
		}
		else if (GC.getGame().GetGameDeals().IsReceivingItemsFromPlayer(pkPlayer->GetID(), ePlayer, false))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -12;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_PARTNER");
			aOpinions.push_back(kOpinion);
		}
		else if (pkPlayer->GetTrade()->GetAllTradeValueFromPlayerTimes100(YIELD_GOLD, ePlayer) > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -10;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_PARTNER");
			aOpinions.push_back(kOpinion);
		}
		// Fought against a common foe?
		iValue = pDiplo->GetCommonFoeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_COMMON_FOE");
			aOpinions.push_back(kOpinion);
		}
		// Pledged to Protect the same City-States?
		iValue = pDiplo->GetPtPSameCSScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -7;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SAME_PTP");
			aOpinions.push_back(kOpinion);
		}
		// Embassy?
		bool bUsEmbassy = pDiplo->IsHasEmbassy(ePlayer);
		bool bThemEmbassy = GET_PLAYER(ePlayer).GetDiplomacyAI()->IsHasEmbassy(pkPlayer->GetID());
		if (bUsEmbassy && bThemEmbassy)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -1;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_EMBASSY");
			aOpinions.push_back(kOpinion);
		}
		else if (bUsEmbassy)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -1;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HAS_EMBASSY");
			aOpinions.push_back(kOpinion);
		}
		else if (bThemEmbassy)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -1;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WE_HAVE_EMBASSY");
			aOpinions.push_back(kOpinion);
		}
		// Diplomat?
		iValue = pDiplo->GetDiplomatScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -10;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DIPLOMAT");
			aOpinions.push_back(kOpinion);
		}
		// Open Borders?
		bool bThemOpen = pDiplo->IsHasOpenBorders(ePlayer);
		bool bUsOpen = GET_PLAYER(ePlayer).GetDiplomacyAI()->IsHasOpenBorders(pkPlayer->GetID());
		if (bThemOpen && bUsOpen)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -12;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_OPEN_BORDERS_MUTUAL");
			aOpinions.push_back(kOpinion);
		}
		else if (bThemOpen)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -12;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_OPEN_BORDERS_US");
			aOpinions.push_back(kOpinion);
		}
		else if (bUsOpen)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -6;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_OPEN_BORDERS_THEM");
			aOpinions.push_back(kOpinion);
		}
		iValue = pDiplo->GetLiberatedCapitalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -130;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LIBERATED_CAPITAL");
			aOpinions.push_back(kOpinion);
		}
		iValue = pDiplo->GetLiberatedHolyCityScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -120;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LIBERATED_HOLY_CITY");
			aOpinions.push_back(kOpinion);
		}
		iValue = pDiplo->GetReturnedCapitalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -110;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RETURNED_CAPITAL");
			aOpinions.push_back(kOpinion);
		}
		iValue = pDiplo->GetReturnedHolyCityScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -100;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RETURNED_HOLY_CITY");
			aOpinions.push_back(kOpinion);
		}
		if (!bTeammate)
		{
			// Stole territory from us?
			if (pDiplo->GetNumTimesCultureBombed(ePlayer) > 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 50;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CULTURE_BOMB");
				aOpinions.push_back(kOpinion);
			}
			// Stole artifacts from us?
			if (pDiplo->GetNumArtifactsEverDugUp(ePlayer) > 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 18;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_STOLEN_ARTIFACTS");
				aOpinions.push_back(kOpinion);
			}
			// Stole from us using spies?
			if (pDiplo->GetNumTimesRobbedBy(ePlayer) > 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 33;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAUGHT_STEALING");
				aOpinions.push_back(kOpinion);
			}
			// Stole our City-State Alliances?
			if (pDiplo->GetNumTimesPerformedCoupAgainstUs(ePlayer) > 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 35;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PERFORMED_COUP");
				aOpinions.push_back(kOpinion);
			}
			// Nuked us?
			if (pDiplo->IsNukedBy(ePlayer))
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 1000;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NUKED");
				aOpinions.push_back(kOpinion);
			}
			// Captured our Holy City?
			if (pDiplo->IsPlayerCapturedHolyCity(ePlayer))
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 2000;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAPTURED_HOLY_CITY");
				aOpinions.push_back(kOpinion);
			}
			// Captured our capital?
			if (pDiplo->IsPlayerCapturedCapital(ePlayer))
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 3000;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAPTURED_CAPITAL");
				aOpinions.push_back(kOpinion);
			}
			// Killed or captured our civilians?
			iValue = pDiplo->GetCivilianKillerScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RAZED");
				aOpinions.push_back(kOpinion);			
			}
			// Converted our cities?
			iValue = pDiplo->GetNegativeReligiousConversionPoints(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = (iValue * 2);
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_CONVERSIONS");
				aOpinions.push_back(kOpinion);
			}
			// Warmongering!
			iValue = pDiplo->GetWarmongerThreatScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				CvString str;

				switch (pDiplo->GetWarmongerThreat(ePlayer))
				{
				case THREAT_CRITICAL:
					str = pDiplo->IsNukedBy(ePlayer) ? Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_CRITICAL_NUCLEAR").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_CRITICAL").toUTF8();
					break;
				case THREAT_SEVERE:
					str = Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_SEVERE").toUTF8();
					break;
				case THREAT_MAJOR:
					str = Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_MAJOR").toUTF8();
					break;
				default:
					str = Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_MINOR").toUTF8();
					break;
				}

				kOpinion.m_str = str;
				aOpinions.push_back(kOpinion);
			}
		}
		iValue = pDiplo->GetNumSamePolicies(ePlayer);
		if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -3;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SAME_POLICIES");
			aOpinions.push_back(kOpinion);
		}
		else if (iValue < 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = bTeammate ? 0 : 3;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_POLICIES");
			aOpinions.push_back(kOpinion);
		}
		// Same religion?
		if (pDiplo->IsPlayerSameReligion(ePlayer))
		{
			CvString str;

			ReligionTypes eOurStateReligion = pkPlayer->GetReligions()->GetCurrentReligion(false);
			ReligionTypes eOurMajorityReligion = pkPlayer->GetReligions()->GetReligionInMostCities();
			ReligionTypes eTheirStateReligion = GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion(false);
			ReligionTypes eTheirMajorityReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();

			if (eOurStateReligion != NO_RELIGION && eOurStateReligion == eTheirMajorityReligion)
			{
				iValue = -60;
				str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_MY_RELIGION").toUTF8();
			}
			else if (eTheirStateReligion != NO_RELIGION && eTheirStateReligion == eOurMajorityReligion)
			{
				iValue = -30;
				str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_HIS_RELIGION").toUTF8();
			}
			else
			{
				iValue = -30;
				str = Localization::Lookup("TXT_KEY_DIPLO_SAME_MAJORITY_RELIGIONS").toUTF8();
			}

			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}
		// Different religions?
		else if (pDiplo->IsPlayerOpposingReligion(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = bTeammate ? 0 : 30;
			kOpinion.m_str = bTeammate ? Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_DIFFERENCES_NEUTRAL") : Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_DIFFERENCES");
			aOpinions.push_back(kOpinion);
		}
		// Same ideology?
		if (pDiplo->IsPlayerSameIdeology(ePlayer))
		{
			Opinion kOpinion;
			PolicyBranchTypes eBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
			kOpinion.m_iValue = -999;
			kOpinion.m_str = (!pkPlayer->IsVassalOfSomeone() && !GET_PLAYER(ePlayer).IsVassalOfSomeone()) ? Localization::Lookup("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES") : Localization::Lookup("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES_VASSAL");
			kOpinion.m_str << GC.getPolicyBranchInfo(eBranch)->GetDescription();
			aOpinions.push_back(kOpinion);
		}
		// Different ideologies?
		else if (pDiplo->IsPlayerOpposingIdeology(ePlayer))
		{
			Opinion kOpinion;
			PolicyBranchTypes eOurBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
			PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
			kOpinion.m_iValue = bTeammate ? 0 : 999;
			kOpinion.m_str = (!GET_PLAYER(ePlayer).IsVassalOfSomeone()) ? Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES") : Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES_VASSAL");
			kOpinion.m_str << GC.getPolicyBranchInfo(eTheirBranch)->GetDescription();
			kOpinion.m_str << GC.getPolicyBranchInfo(eOurBranch)->GetDescription();
			aOpinions.push_back(kOpinion);
		}
		// World Congress?
		if (pDiplo->GetTheySupportedOurProposalTurn(ePlayer) > -1)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -36;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}
		else if (pDiplo->GetTheyFoiledOurProposalTurn(ePlayer) > -1)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = bTeammate ? 0 : 36;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}
		if (pDiplo->GetTheySupportedOurHostingTurn(ePlayer) > -1)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -38;
			CvString str;

			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL && pLeague->GetConsecutiveHostedSessions() > 1)
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_MAINTAINED_THEIR_HOSTING_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_MAINTAINED_THEIR_HOSTING").toUTF8();
			}
			else
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_HOSTING_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_HOSTING").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}
		// Vassalage?
		if (MOD_DIPLOMACY_CIV4_FEATURES)
		{
			if (pDiplo->IsMaster(ePlayer))
			{
				Opinion kOpinion;
				kOpinion.m_iValue = -99;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WE_ARE_MASTER");
				aOpinions.push_back(kOpinion);
			}
			else if (pDiplo->IsVassal(ePlayer))
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 0;
				kOpinion.m_str = (pDiplo->IsVoluntaryVassalage(ePlayer)) ? Localization::Lookup("TXT_KEY_DIPLO_WE_ARE_VOLUNTARY_VASSAL") : Localization::Lookup("TXT_KEY_DIPLO_WE_ARE_VASSAL");
				aOpinions.push_back(kOpinion);
			}
			int iValue = pDiplo->GetSameMasterScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = -31;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SAME_MASTER");
				aOpinions.push_back(kOpinion);
			}
		}
	}

	//--------------------------------//
	// [PART 2B: AI TEAMMATES]		  //
	//--------------------------------//

	else if (bTeammate && !bObserver)
	{
		iValue = pDiplo->GetCiviliansReturnedToMeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CIVILIANS_RETURNED");
			aOpinions.push_back(kOpinion);
		}
		
		iValue = pDiplo->GetLandmarksBuiltForMeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LANDMARKS_BUILT");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetResurrectedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RESURRECTED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLiberatedCapitalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LIBERATED_CAPITAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLiberatedHolyCityScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LIBERATED_HOLY_CITY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLiberatedCitiesScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITIES_LIBERATED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetReturnedCapitalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RETURNED_CAPITAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetReturnedHolyCityScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RETURNED_HOLY_CITY");
			aOpinions.push_back(kOpinion);
		}

		if (pDiplo->IsPlayerSameReligion(ePlayer))
		{
			CvString str;

			ReligionTypes eAIStateReligion = pkPlayer->GetReligions()->GetCurrentReligion(false);
			ReligionTypes eAIMajorityReligion = pkPlayer->GetReligions()->GetReligionInMostCities();
			ReligionTypes eHumanStateReligion = GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion(false);
			ReligionTypes eHumanMajorityReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();
			
			if (eAIStateReligion != NO_RELIGION && eAIStateReligion == eHumanMajorityReligion)
			{
				iValue = pDiplo->GetReligionScore(ePlayer);
				str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_MY_RELIGION").toUTF8();
			}
			else if (eHumanStateReligion != NO_RELIGION && eHumanStateReligion == eAIMajorityReligion)
			{
				iValue = pDiplo->GetReligionScore(ePlayer);
				str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_HIS_RELIGION").toUTF8();
			}
			else
			{
				iValue = pDiplo->GetReligionScore(ePlayer);
				str = Localization::Lookup("TXT_KEY_DIPLO_SAME_MAJORITY_RELIGIONS").toUTF8();
			}

			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}
		else if (pDiplo->IsPlayerOpposingReligion(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_DIFFERENCES_NEUTRAL");
			aOpinions.push_back(kOpinion);
		}

		if (pDiplo->IsPlayerSameIdeology(ePlayer))
		{
			Opinion kOpinion;
			PolicyBranchTypes eBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
			kOpinion.m_iValue = pDiplo->GetIdeologyScore(ePlayer);
			kOpinion.m_str = (!pkPlayer->IsVassalOfSomeone() && !GET_PLAYER(ePlayer).IsVassalOfSomeone()) ? Localization::Lookup("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES") : Localization::Lookup("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES_VASSAL");
			kOpinion.m_str << GC.getPolicyBranchInfo(eBranch)->GetDescription();
			aOpinions.push_back(kOpinion);
		}
		// Different ideologies?
		else if (pDiplo->IsPlayerOpposingIdeology(ePlayer))
		{
			Opinion kOpinion;
			PolicyBranchTypes eOurBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
			PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
			kOpinion.m_iValue = 0;
			kOpinion.m_str = (!GET_PLAYER(ePlayer).IsVassalOfSomeone()) ? Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES") : Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES_VASSAL");
			kOpinion.m_str << GC.getPolicyBranchInfo(eTheirBranch)->GetDescription();
			kOpinion.m_str << GC.getPolicyBranchInfo(eOurBranch)->GetDescription();
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetTimesIntrigueSharedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SHARED_INTRIGUE");
			aOpinions.push_back(kOpinion);
		}

		if (pDiplo->IsPlayerDoFWithAnyEnemy(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DOF_WITH_ENEMY");
			aOpinions.push_back(kOpinion);
		}

		if (pDiplo->IsPlayerDenouncedFriend(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIEND");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDenouncedEnemyScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_ENEMY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetRecentTradeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_PARTNER");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetCommonFoeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_COMMON_FOE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetRecentAssistScore(ePlayer);
		if (iValue < 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_ASSISTANCE_TO_THEM");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetNumSamePolicies(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = (iValue > 0) ? pDiplo->GetPolicyScore(ePlayer) : 0;
			kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_SAME_POLICIES") : Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_POLICIES");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetPtPSameCSScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SAME_PTP");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLikedTheirProposalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_LIKED_OUR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_LIKED_OUR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}
		
		iValue = pDiplo->GetDislikedTheirProposalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_DISLIKED_OUR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_DISLIKED_OUR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetSupportedOurProposalScore(ePlayer);
		if (iValue < 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}
		else if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetSupportedMyHostingScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL && pLeague->GetConsecutiveHostedSessions() > 1)
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_MAINTAINED_THEIR_HOSTING_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_MAINTAINED_THEIR_HOSTING").toUTF8();
			}
			else
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_HOSTING_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_HOSTING").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}
	}

	//--------------------------------//
	// [PART 2C: OTHER AI PLAYERS]	  //
	//--------------------------------//

	else if (!bObserver)
	{
		////////////////////////////////////
		// DISPUTE MODIFIERS
		////////////////////////////////////

		if (!bJustMet)
		{
			// Land Dispute
			if (bPretendNoDisputes)
			{
				int iEra = (int)pkPlayer->GetCurrentEra();
				if (iEra <= 0)
					iEra = 1;

				iValue = /*-10*/ GC.getOPINION_WEIGHT_LAND_NONE();

				if (pDiplo->IsConqueror())
				{
					iValue += (/*-5*/ GC.getOPINION_WEIGHT_LAND_NONE_WARMONGER() - pDiplo->GetNeediness()) * iEra / 2;
				}

				iValue *= pDiplo->GetBoldness();

				if (GET_PLAYER(ePlayer).isHuman() && GET_PLAYER(ePlayer).getHandicapInfo().getAIDeclareWarProb() > 200)
				{
					iValue *= GET_PLAYER(ePlayer).getHandicapInfo().getAIDeclareWarProb();
					iValue /= 1000;
				}
				else if (!GET_PLAYER(ePlayer).isHuman() && GC.getGame().getHandicapInfo().getAIDeclareWarProb() > 200)
				{
					iValue *= GC.getGame().getHandicapInfo().getAIDeclareWarProb();
					iValue /= 1000;
				}
				else
				{
					iValue /= 5;
				}
			}
			else if (bHideDisputes)
			{
				iValue = 0;
			}
			else
			{
				iValue = pDiplo->GetLandDisputeLevelScore(ePlayer);
			}

			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_LAND_DISPUTE") : Localization::Lookup("TXT_KEY_DIPLO_NO_LAND_DISPUTE");
				aOpinions.push_back(kOpinion);
			}

			// Wonder Dispute
			if (bPretendNoDisputes && !pDiplo->IsPlayerWonderSpammer(ePlayer))
			{
				int iEra = (int)pkPlayer->GetCurrentEra();
				if (iEra <= 0)
					iEra = 1;

				iValue = /*0*/ GC.getOPINION_WEIGHT_WONDER_NONE();

				if (pDiplo->IsCultural())
				{
					iValue += (/*-5*/ GC.getOPINION_WEIGHT_WONDER_NONE_CULTURAL() - pDiplo->GetNeediness()) * iEra / 2;
				}

				iValue *= pDiplo->GetWonderCompetitiveness();

				if (GET_PLAYER(ePlayer).isHuman() && GET_PLAYER(ePlayer).getHandicapInfo().getAIDeclareWarProb() > 200)
				{
					iValue *= GET_PLAYER(ePlayer).getHandicapInfo().getAIDeclareWarProb();
					iValue /= 1000;
				}
				else if (!GET_PLAYER(ePlayer).isHuman() && GC.getGame().getHandicapInfo().getAIDeclareWarProb() > 200)
				{
					iValue *= GC.getGame().getHandicapInfo().getAIDeclareWarProb();
					iValue /= 1000;
				}
				else
				{
					iValue /= 5;
				}
			}
			else if (bHideDisputes)
			{
				iValue = 0;
			}
			else
			{
				iValue = pDiplo->GetWonderDisputeLevelScore(ePlayer);
			}

			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_WONDER_DISPUTE") : Localization::Lookup("TXT_KEY_DIPLO_NO_WONDER_DISPUTE");
				aOpinions.push_back(kOpinion);
			}

			// Minor Civ Dispute
			if (bPretendNoDisputes && pDiplo->GetNumTimesTheyLoweredOurInfluence(ePlayer) < 8 && !pDiplo->IsMinorCivTroublemaker(ePlayer, true))
			{
				int iEra = (int)pkPlayer->GetCurrentEra();
				if (iEra <= 0)
					iEra = 1;

				iValue = /*0*/ GC.getOPINION_WEIGHT_MINOR_CIV_NONE();

				if (pDiplo->IsDiplomat())
				{
					iValue += (/*-5*/ GC.getOPINION_WEIGHT_MINOR_CIV_NONE_DIPLOMAT() - pDiplo->GetNeediness()) * iEra / 2;
				}

				iValue *= pDiplo->GetMinorCivCompetitiveness();

				if (GET_PLAYER(ePlayer).isHuman() && GET_PLAYER(ePlayer).getHandicapInfo().getAIDeclareWarProb() > 200)
				{
					iValue *= GET_PLAYER(ePlayer).getHandicapInfo().getAIDeclareWarProb();
					iValue /= 1000;
				}
				else if (!GET_PLAYER(ePlayer).isHuman() && GC.getGame().getHandicapInfo().getAIDeclareWarProb() > 200)
				{
					iValue *= GC.getGame().getHandicapInfo().getAIDeclareWarProb();
					iValue /= 1000;
				}
				else
				{
					iValue /= 5;
				}
			}
			else if (bHideDisputes)
			{
				iValue = 0;
			}
			else
			{
				iValue = pDiplo->GetMinorCivDisputeLevelScore(ePlayer);
			}

			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_MINOR_CIV_DISPUTE") : Localization::Lookup("TXT_KEY_DIPLO_NO_MINOR_CIV_DISPUTE");
				aOpinions.push_back(kOpinion);
			}

			// Tech Block
			iValue = bHideDisputes ? 0 : pDiplo->GetTechBlockLevelScore(ePlayer);

			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;

				if (iValue > 0)
				{
					if (pDiplo->GetTechBlockLevel(ePlayer) == BLOCK_LEVEL_FIERCE)
						kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TECH_BLOCK_FIERCE");
					else
						kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TECH_BLOCK");
				}
				else
				{
					kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_TECH_BLOCK");
				}

				aOpinions.push_back(kOpinion);
			}

			// Policy Block
			iValue = bHideDisputes ? 0 : pDiplo->GetPolicyBlockLevelScore(ePlayer);

			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;

				if (iValue > 0)
				{
					if (pDiplo->GetPolicyBlockLevel(ePlayer) == BLOCK_LEVEL_FIERCE)
						kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_POLICY_BLOCK_FIERCE");
					else
						kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_POLICY_BLOCK");
				}
				else
				{
					kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_POLICY_BLOCK");
				}

				aOpinions.push_back(kOpinion);
			}

			// Victory Dispute
			iValue = (bHideDisputes || bHideNegatives) ? 0 : pDiplo->GetVictoryDisputeLevelScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				CvString str;
				
				if (pDiplo->GetVictoryDisputeLevel(ePlayer) == DISPUTE_LEVEL_FIERCE)
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VICTORY_DISPUTE_CBP_FIERCE").toUTF8();
				}
				else if (pDiplo->GetVictoryDisputeLevel(ePlayer) == DISPUTE_LEVEL_STRONG)
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VICTORY_DISPUTE_CBP_STRONG").toUTF8();
				}
				else if (pDiplo->GetVictoryDisputeLevel(ePlayer) == DISPUTE_LEVEL_WEAK)
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VICTORY_DISPUTE_CBP_WEAK").toUTF8();
				}

				kOpinion.m_str = str;
				aOpinions.push_back(kOpinion);	
			}

			// Victory Block
			iValue = (bHideDisputes || bHideNegatives) ? 0 : pDiplo->GetVictoryBlockLevelScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				CvString str;
				
				if (pDiplo->GetVictoryBlockLevel(ePlayer) == BLOCK_LEVEL_FIERCE)
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VICTORY_BLOCK_FIERCE").toUTF8();
				}
				else if (pDiplo->GetVictoryBlockLevel(ePlayer) == BLOCK_LEVEL_STRONG)
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VICTORY_BLOCK_STRONG").toUTF8();
				}
				else if (pDiplo->GetVictoryBlockLevel(ePlayer) == BLOCK_LEVEL_WEAK)
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_VICTORY_BLOCK_WEAK").toUTF8();
				}

				kOpinion.m_str = str;
				aOpinions.push_back(kOpinion);
			}

			// Aggressive Posture
			if (!pDiplo->IsAtWar(ePlayer) && !GET_PLAYER(ePlayer).GetDiplomacyAI()->IsHasOpenBorders(pkPlayer->GetID()))
			{
				iValue = pDiplo->GetMilitaryAggressivePosture(ePlayer) * 5;
				if (iValue != 0)
				{
					Opinion kOpinion;
					kOpinion.m_iValue = iValue;
					CvString str;

					if (pDiplo->GetMilitaryAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_HIGH)
					{
						str = Localization::Lookup("TXT_KEY_DIPLO_AGGRESSIVE_POSTURE_HIGH").toUTF8();
					}
					else
					{
						str = Localization::Lookup("TXT_KEY_DIPLO_AGGRESSIVE_POSTURE_MEDIUM").toUTF8();
					}

					kOpinion.m_str = str;
					aOpinions.push_back(kOpinion);
				}
			}
		}

		// Warmongering! (Always visible if at war.)
		iValue = (bHideNegatives && !pDiplo->IsAtWar(ePlayer)) ? 0 : pDiplo->GetWarmongerThreatScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			switch (pDiplo->GetWarmongerThreat(ePlayer))
			{
			case THREAT_CRITICAL:
				str = pDiplo->IsNukedBy(ePlayer) ? Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_CRITICAL_NUCLEAR").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_CRITICAL").toUTF8();
				break;
			case THREAT_SEVERE:
				str = Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_SEVERE").toUTF8();
				break;
			case THREAT_MAJOR:
				str = Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_MAJOR").toUTF8();
				break;
			default:
				str = Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT_MINOR").toUTF8();
				break;
			}

			str += " ";

			// Aztecs have a special message.
			if (!GC.getGame().isOption(GAMEOPTION_RANDOM_PERSONALITIES) && pkPlayer->GetPlayerTraits()->GetGoldenAgeFromVictory() != 0)
			{
				str += Localization::Lookup("TXT_KEY_WARMONGER_HATE_AZTECS").toUTF8();
			}
			// India/Venice have a special message.
			else if (!GC.getGame().isOption(GAMEOPTION_RANDOM_PERSONALITIES) && (pkPlayer->GetPlayerTraits()->IsNoAnnexing() || pkPlayer->GetPlayerTraits()->IsPopulationBoostReligion()))
			{
				str += Localization::Lookup("TXT_KEY_WARMONGER_HATE_MAXIMAL").toUTF8();
			}
			// Otherwise, give a hint as to this player's WarmongerHate flavor. This is relevant for humans too!
			else
			{
				if (pDiplo->GetWarmongerHate() >= 9)
				{
					str += Localization::Lookup("TXT_KEY_WARMONGER_HATE_EXTREME").toUTF8();
				}
				else if (pDiplo->GetWarmongerHate() >= 7)
				{
					str += Localization::Lookup("TXT_KEY_WARMONGER_HATE_HIGH_CBP").toUTF8();
				}
				else if (pDiplo->GetWarmongerHate() >= 5)
				{
					str += Localization::Lookup("TXT_KEY_WARMONGER_HATE_MID").toUTF8();
				}
				else if (pDiplo->GetWarmongerHate() >= 3)
				{
					str += Localization::Lookup("TXT_KEY_WARMONGER_HATE_LOW").toUTF8();
				}
				else
				{
					str += Localization::Lookup("TXT_KEY_WARMONGER_HATE_MINIMAL").toUTF8();
				}
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		// Civilians killed/captured
		iValue = pDiplo->GetCivilianKillerScore(ePlayer);
		iTempValue = pDiplo->GetCivilianKillerGlobalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = bHideNegatives ? iValue : max(iValue, iTempValue);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RAZED");
			aOpinions.push_back(kOpinion);
		}
		else if (iTempValue != 0 && !bHideNegatives && !bJustMet)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iTempValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RAZED_OTHER");
			aOpinions.push_back(kOpinion);
		}

		////////////////////////////////////
		// PROMISES
		////////////////////////////////////

		// Human Promises
		iValue = pDiplo->GetPlayerMadeMilitaryPromise(ePlayer);
		if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = GetLocalizedText("TXT_KEY_DIPLO_MILITARY_PROMISE_TURNS", iValue);
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetPlayerMadeExpansionPromise(ePlayer);
		if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = GetLocalizedText("TXT_KEY_DIPLO_EXPANSION_PROMISE_TURNS", iValue);
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetPlayerMadeBorderPromise(ePlayer);
		if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = GetLocalizedText("TXT_KEY_DIPLO_BORDER_PROMISE_TURNS", iValue);
			aOpinions.push_back(kOpinion);
		}
		
		// AI Promises
		iValue = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetPlayerMadeMilitaryPromise(pkPlayer->GetID());
		if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = GetLocalizedText("TXT_KEY_DIPLO_AI_MILITARY_PROMISE_TURNS", iValue);
			aOpinions.push_back(kOpinion);
		}

		iValue = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetPlayerMadeExpansionPromise(pkPlayer->GetID());
		if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			kOpinion.m_str = GetLocalizedText("TXT_KEY_DIPLO_AI_EXPANSION_PROMISE_TURNS", iValue);
			aOpinions.push_back(kOpinion);
		}

		// Timer to avoid backstabbing penalties
		if (pDiplo->IsDoFBroken(ePlayer) && !pDiplo->IsAtWar(ePlayer) && !GET_PLAYER(ePlayer).IsVassalOfSomeone())
		{
			if (!pDiplo->IsFriendDenouncedUs(ePlayer) && !pDiplo->IsFriendDeclaredWarOnUs(ePlayer))
			{
				int iTurns = pDiplo->GetTurnsSinceDoFBroken(ePlayer);
				int iTimer = /*10*/ GC.getDOF_BROKEN_BACKSTAB_TIMER();

				if (iTurns < iTimer)
				{	
					iValue = (iTimer - iTurns);
					Opinion kOpinion;
					kOpinion.m_iValue = 0;
					CvString str;
					
					if (!pDiplo->IsFriendDeclaredWarOnUs(ePlayer))
					{
						if (!pDiplo->IsFriendDenouncedUs(ePlayer))
						{
							str = GetLocalizedText("TXT_KEY_DIPLO_BACKSTAB_WARNING_TURNS", iValue);
						}
						else
						{
							str = GetLocalizedText("TXT_KEY_DIPLO_BACKSTAB_WARNING_WAR_ONLY_TURNS", iValue);
						}
					}
					else
					{
						if (!pDiplo->IsFriendDenouncedUs(ePlayer))
						{
							str = GetLocalizedText("TXT_KEY_DIPLO_BACKSTAB_WARNING_DENOUNCE_ONLY_TURNS", iValue);
						}
					}

					kOpinion.m_str = str;
					aOpinions.push_back(kOpinion);
				}
			}
		}

		////////////////////////////////////
		// CAN BE BOTH POSITIVE AND NEGATIVE
		////////////////////////////////////

		iValue = pDiplo->GetNumSamePolicies(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetPolicyScore(ePlayer);

			if (bHideNegatives && iValue < 0)
				kOpinion.m_iValue = 0;

			kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_SAME_POLICIES") : Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_POLICIES");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetReligionScore(ePlayer);
		if (pDiplo->IsPlayerSameReligion(ePlayer))
		{
			CvString str;

			ReligionTypes eAIStateReligion = pkPlayer->GetReligions()->GetCurrentReligion(false);
			ReligionTypes eAIMajorityReligion = pkPlayer->GetReligions()->GetReligionInMostCities();
			ReligionTypes eHumanStateReligion = GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion(false);
			ReligionTypes eHumanMajorityReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();
			
			if (eAIStateReligion != NO_RELIGION && eAIStateReligion == eHumanMajorityReligion)
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_MY_RELIGION").toUTF8();
			}
			else if (eHumanStateReligion != NO_RELIGION && eHumanStateReligion == eAIMajorityReligion)
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_HIS_RELIGION").toUTF8();
			}
			else
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_SAME_MAJORITY_RELIGIONS").toUTF8();
			}

			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}
		else if (pDiplo->IsPlayerOpposingReligion(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = bHideNegatives ? 0 : iValue;
			kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_DIFFERENCES") : Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_DIFFERENCES_NEUTRAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIdeologyScore(ePlayer);
		if (pDiplo->IsPlayerSameIdeology(ePlayer))
		{
			Opinion kOpinion;
			PolicyBranchTypes eBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = (!pkPlayer->IsVassalOfSomeone() && !GET_PLAYER(ePlayer).IsVassalOfSomeone()) ? Localization::Lookup("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES") : Localization::Lookup("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES_VASSAL");
			kOpinion.m_str << GC.getPolicyBranchInfo(eBranch)->GetDescription();
			aOpinions.push_back(kOpinion);
		}
		else if (pDiplo->IsPlayerOpposingIdeology(ePlayer))
		{
			Opinion kOpinion;
			PolicyBranchTypes eOurBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
			PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
			kOpinion.m_iValue = bHideNegatives ? 0 : iValue;
			kOpinion.m_str = (!GET_PLAYER(ePlayer).IsVassalOfSomeone()) ? Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES") : Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES_VASSAL");
			kOpinion.m_str << GC.getPolicyBranchInfo(eTheirBranch)->GetDescription();
			kOpinion.m_str << GC.getPolicyBranchInfo(eOurBranch)->GetDescription();
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLeagueAlignmentScore(ePlayer);
		if (iValue > 0 && (bHideDisputes || bHideNegatives))
		{
			iValue = 0;
		}

		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			if (iValue > 0)
			{
				if (pDiplo->GetPrimeLeagueCompetitor() == ePlayer)
				{
					str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_PRIME_LEAGUE_COMPETITOR_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_PRIME_LEAGUE_COMPETITOR").toUTF8();
				}
				else
				{
					str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_BAD_LEAGUE_ALIGNMENT_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_BAD_LEAGUE_ALIGNMENT").toUTF8();
				}
			}
			else
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_GOOD_LEAGUE_ALIGNMENT_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_GOOD_LEAGUE_ALIGNMENT").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		if (MOD_DIPLOMACY_CIV4_FEATURES)
		{
			if (pDiplo->IsMaster(ePlayer))
			{
				Opinion kOpinion;
				kOpinion.m_iValue = pDiplo->GetMasterScore(ePlayer);
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WE_ARE_MASTER");
				aOpinions.push_back(kOpinion);
			}
			else if (pDiplo->IsVassal(ePlayer))
			{
				if (pDiplo->IsVoluntaryVassalage(ePlayer))
				{
					Opinion kOpinion;
					kOpinion.m_iValue = pDiplo->GetVassalScore(ePlayer);
					kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WE_ARE_VOLUNTARY_VASSAL");
					aOpinions.push_back(kOpinion);
				}
				else
				{
					Opinion kOpinion;
					kOpinion.m_iValue = pDiplo->GetVassalScore(ePlayer);
					kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WE_ARE_VASSAL");
					aOpinions.push_back(kOpinion);
				}

				Opinion kOpinion;
				kOpinion.m_iValue = pDiplo->GetVassalTreatedScore(ePlayer);
				CvString str;

				VassalTreatmentTypes eTreatmentLevel = pDiplo->GetVassalTreatmentLevel(ePlayer);
				switch (eTreatmentLevel)
				{
					case VASSAL_TREATMENT_CONTENT:
						str = Localization::Lookup("TXT_KEY_DIPLO_VASSAL_TREATMENT_CONTENT").toUTF8();
						break;
					case VASSAL_TREATMENT_DISAGREE:
						str = Localization::Lookup("TXT_KEY_DIPLO_VASSAL_TREATMENT_DISAGREE").toUTF8();
						break;
					case VASSAL_TREATMENT_MISTREATED:
						str = Localization::Lookup("TXT_KEY_DIPLO_VASSAL_TREATMENT_MISTREATED").toUTF8();
						break;
					case VASSAL_TREATMENT_UNHAPPY:
						str = Localization::Lookup("TXT_KEY_DIPLO_VASSAL_TREATMENT_UNHAPPY").toUTF8();
						break;
					case VASSAL_TREATMENT_ENSLAVED:
						str = Localization::Lookup("TXT_KEY_DIPLO_VASSAL_TREATMENT_ENSLAVED").toUTF8();
						break;
				}

				kOpinion.m_str = str;
				aOpinions.push_back(kOpinion);
			}
			else
			{
				iValue = pDiplo->GetTooManyVassalsScore(ePlayer);
				if (iValue != 0)
				{
					Opinion kOpinion;
					kOpinion.m_iValue = iValue;
					kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TOO_MANY_VASSALS");
					aOpinions.push_back(kOpinion);
				}

				iValue = pDiplo->GetSameMasterScore(ePlayer);
				if (iValue != 0)
				{
					Opinion kOpinion;
					kOpinion.m_iValue = iValue;
					kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SAME_MASTER");
					aOpinions.push_back(kOpinion);
				}
			}

			iValue = pDiplo->GetMasterLiberatedMeFromVassalageScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MASTER_LIBERATED_ME_FROM_VASSALAGE");
				aOpinions.push_back(kOpinion);
			}

			iValue = pDiplo->GetHappyAboutVassalagePeacefullyRevokedScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_VASSAL_PEACEFULLY_REVOKED");
				aOpinions.push_back(kOpinion);
			}

			iValue = pDiplo->GetAngryAboutVassalageForcefullyRevokedScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_VASSAL_FORCEFULLY_REVOKED");
				aOpinions.push_back(kOpinion);
			}

			// Vassal protect VS. failed protect
			if (pDiplo->GetVassalProtectScore(ePlayer) != (pDiplo->GetVassalFailedProtectScore(ePlayer) * -1))
			{
				iValue = pDiplo->GetVassalProtectScore(ePlayer) + pDiplo->GetVassalFailedProtectScore(ePlayer);
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_VASSAL_FAILED_PROTECT") : Localization::Lookup("TXT_KEY_DIPLO_VASSAL_PROTECT");
				aOpinions.push_back(kOpinion);
			}

			iValue = pDiplo->GetBrokenVassalAgreementScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_ATTACKED_VASSAL_ME");
				aOpinions.push_back(kOpinion);
			}
		}

		iValue = pDiplo->GetRecentAssistScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = (iValue > 0) ? Localization::Lookup("TXT_KEY_DIPLO_REFUSED_REQUESTS") : Localization::Lookup("TXT_KEY_DIPLO_ASSISTANCE_TO_THEM");
			aOpinions.push_back(kOpinion);
		}

		////////////////////////////////////
		// POSITIVES
		////////////////////////////////////

		// Embassy?
		bool bUsEmbassy = pDiplo->IsHasEmbassy(ePlayer);
		bool bThemEmbassy = GET_PLAYER(ePlayer).GetDiplomacyAI()->IsHasEmbassy(pkPlayer->GetID());
		if (bUsEmbassy && bThemEmbassy)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetEmbassyScore(ePlayer);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_EMBASSY");
			aOpinions.push_back(kOpinion);
		}
		else if (bUsEmbassy)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetEmbassyScore(ePlayer);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HAS_EMBASSY");
			aOpinions.push_back(kOpinion);
		}
		else if (bThemEmbassy)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetEmbassyScore(ePlayer);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WE_HAVE_EMBASSY");
			aOpinions.push_back(kOpinion);
		}
		// Open Borders?
		bool bThemOpen = pDiplo->IsHasOpenBorders(ePlayer);
		bool bUsOpen = GET_PLAYER(ePlayer).GetDiplomacyAI()->IsHasOpenBorders(pkPlayer->GetID());
		if (bThemOpen && bUsOpen)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetOpenBordersScore(ePlayer);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_OPEN_BORDERS_MUTUAL");
			aOpinions.push_back(kOpinion);
		}
		else if (bThemOpen)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetOpenBordersScore(ePlayer);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_OPEN_BORDERS_US");
			aOpinions.push_back(kOpinion);
		}
		else if (bUsOpen)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetOpenBordersScore(ePlayer);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_OPEN_BORDERS_THEM");
			aOpinions.push_back(kOpinion);
		}
		// Diplomat?
		if (pkPlayer->GetEspionage()->IsOtherDiplomatVisitingMe(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = pDiplo->GetDiplomatScore(ePlayer);
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DIPLOMAT");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDPAcceptedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DP");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDPWithAnyFriendScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DP_MUTUAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetResearchAgreementScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MADE_RESEARCH_AGREEMENT");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetCiviliansReturnedToMeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CIVILIANS_RETURNED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLandmarksBuiltForMeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LANDMARKS_BUILT");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetResurrectedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RESURRECTED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLiberatedCapitalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LIBERATED_CAPITAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLiberatedHolyCityScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LIBERATED_HOLY_CITY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLiberatedCitiesScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITIES_LIBERATED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetReturnedCapitalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RETURNED_CAPITAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetReturnedHolyCityScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RETURNED_HOLY_CITY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetForgaveForSpyingScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_FORGAVE_FOR_SPYING");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetTimesIntrigueSharedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SHARED_INTRIGUE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDOFAcceptedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = pDiplo->IsDoFAccepted(ePlayer) ? Localization::Lookup("TXT_KEY_DIPLO_DOF") : Localization::Lookup("TXT_KEY_DIPLO_PREVIOUSLY_FRIENDS");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDOFWithAnyFriendScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_DOF");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDenouncedEnemyScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_ENEMY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetRecentTradeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_PARTNER");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetCommonFoeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_COMMON_FOE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetPtPSameCSScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SAME_PTP");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetLikedTheirProposalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_LIKED_OUR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_LIKED_OUR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetSupportedOurProposalScore(ePlayer);
		if (iValue < 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			// Are we still happy over their past support?
			if (pDiplo->IsSupportedOurProposalAndThenFoiledUs(ePlayer))
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL_PREVIOUSLY_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL_PREVIOUSLY").toUTF8();
			}
			else
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_PROPOSAL").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetSupportedMyHostingScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague->GetConsecutiveHostedSessions() > 1)
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_MAINTAINED_THEIR_HOSTING_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_MAINTAINED_THEIR_HOSTING").toUTF8();
			}
			else
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_HOSTING_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_SUPPORTED_THEIR_HOSTING").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		////////////////////////////////////
		// OBVIOUS NEGATIVES
		////////////////////////////////////

		iValue = pDiplo->GetNoSettleRequestScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_SETTLE_ASKED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetStopSpyingRequestScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_STOP_SPYING_ASKED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDemandMadeScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			// Have we accepted a demand from our master? Then we've paid tribute.
			if (pDiplo->IsHasPaidTributeTo(ePlayer))
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_PAID_TRIBUTE").toUTF8();
			}
			else
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_DEMAND").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDPWithAnyEnemyScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DP_WITH_ENEMY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetTimesRobbedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAUGHT_STEALING");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetTradeRoutesPlunderedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PLUNDERING_OUR_TRADE_ROUTES");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetTimesPlottedAgainstUsScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PLOTTED_AGAINST_US");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetReligiousConversionPointsScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_CONVERSIONS");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetTimesPerformedCoupScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PERFORMED_COUP");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetStoleArtifactsScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_STOLEN_ARTIFACTS");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenMilitaryPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MILITARY_PROMISE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredMilitaryPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MILITARY_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenExpansionPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_EXPANSION_PROMISE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredExpansionPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_EXPANSION_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenBorderPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BORDER_PROMISE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredBorderPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BORDER_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenAttackCityStatePromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITY_STATE_PROMISE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredAttackCityStatePromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITY_STATE_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}
		
		iValue = pDiplo->GetBrokenBullyCityStatePromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_BROKEN");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredBullyCityStatePromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenNoConvertPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_CONVERT_PROMISE_BROKEN");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredNoConvertPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_CONVERT_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenNoDiggingPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_DIG_PROMISE_BROKEN");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredNoDiggingPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_DIG_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenSpyPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SPY_PROMISE_BROKEN");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetIgnoredSpyPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SPY_PROMISE_IGNORED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetBrokenCoopWarPromiseScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_COOP_WAR_PROMISE");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetAngryAboutProtectedMinorKilledScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PROTECTED_MINORS_KILLED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetAngryAboutProtectedMinorAttackedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PROTECTED_MINORS_ATTACKED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetAngryAboutProtectedMinorBulliedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PROTECTED_MINORS_BULLIED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDOFWithAnyEnemyScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DOF_WITH_ENEMY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDenouncedScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			if (pDiplo->IsDenouncedPlayer(ePlayer) && pDiplo->IsDenouncedByPlayer(ePlayer))
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_DENOUNCEMENT").toUTF8();
			}
			else if (pDiplo->IsDenouncedPlayer(ePlayer))
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_US").toUTF8();
			}
			else
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_THEM").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDenouncedFriendScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIEND");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetNukedByScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NUKED");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetCapitalCapturedByScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAPTURED_CAPITAL");
			aOpinions.push_back(kOpinion);
		}
		
		iValue = pDiplo->GetHolyCityCapturedByScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAPTURED_HOLY_CITY");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetResurrectorAttackedUsScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RESURRECTOR_ATTACKED_US");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetDislikedTheirProposalScore(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_DISLIKED_OUR_PROPOSAL_UN") : Localization::Lookup("TXT_KEY_DIPLO_DISLIKED_OUR_PROPOSAL");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetSupportedOurProposalScore(ePlayer);
		if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			CvString str;

			// Are we still holding a grudge over their past opposition?
			if (pDiplo->IsFoiledOurProposalAndThenSupportedUs(ePlayer))
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL_PREVIOUSLY_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL_PREVIOUSLY").toUTF8();
			}
			else
			{
				str = bUNActive ? Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL_UN").toUTF8() : Localization::Lookup("TXT_KEY_DIPLO_FOILED_THEIR_PROPOSAL").toUTF8();
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}

		////////////////////////////////////
		// SUBTLE NEGATIVES
		////////////////////////////////////

		if (!bHideNegatives && !bJustMet)
		{
			iValue = pDiplo->GetDenouncedByOurFriendScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_PEOPLE_WE_TRUST_MORE");
				aOpinions.push_back(kOpinion);
			}

			iValue = pDiplo->GetBrokenMilitaryPromiseWithAnybodyScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MILITARY_PROMISE_BROKEN_WITH_OTHERS");
				aOpinions.push_back(kOpinion);
			}

			iValue = pDiplo->GetBrokenAttackCityStatePromiseWithAnybodyScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITY_STATE_PROMISE_BROKEN_WITH_OTHERS");
				aOpinions.push_back(kOpinion);
			}

			iValue = pDiplo->GetAngryAboutSidedWithProtectedMinorScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SIDED_WITH_MINOR");
				aOpinions.push_back(kOpinion);
			}

			// TRAITOR OPINION START
			iValue = 0;
			CvString str;

			iTempValue = pDiplo->GetFriendDenouncementScore(ePlayer);
			if (iTempValue > iValue)
			{
				iValue = iTempValue;
				str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DENOUNCED_BY_FRIENDS").toUTF8();
			}

			iTempValue = pDiplo->GetWeDenouncedFriendScore(ePlayer);
			if (iTempValue > iValue)
			{
				iValue = iTempValue;
				str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIENDS").toUTF8();
			}

			int iFriendDenouncedUsScore = pDiplo->GetFriendDenouncedUsScore(ePlayer);
			if (iFriendDenouncedUsScore > iValue)
			{
				iValue = iFriendDenouncedUsScore;
				str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_FRIEND_DENOUNCED").toUTF8();
			}

			int iDOWFriendScore = pDiplo->GetWeDeclaredWarOnFriendScore(ePlayer);
			if (iDOWFriendScore > iValue)
			{
				iValue = iDOWFriendScore;
				str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DECLARED_WAR_ON_FRIENDS").toUTF8();
			}

			int iFriendDeclaredWarOnUsScore = pDiplo->GetFriendDeclaredWarOnUsScore(ePlayer);
			if (iFriendDeclaredWarOnUsScore > iValue)
			{
				iValue = iFriendDeclaredWarOnUsScore;
				str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_FRIEND_DECLARED_WAR").toUTF8();
			}

			// If there was a personal betrayal, that matters more to the AI
			if (iFriendDeclaredWarOnUsScore != 0)
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_FRIEND_DECLARED_WAR").toUTF8();
			}
			else if (iFriendDenouncedUsScore != 0 && iDOWFriendScore <= 0)
			{
				str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_FRIEND_DENOUNCED").toUTF8();
			}

			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = str;
				aOpinions.push_back(kOpinion);
			}
			// TRAITOR OPINION END

			iValue = pDiplo->GetRecklessExpanderScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RECKLESS_EXPANDER");
				aOpinions.push_back(kOpinion);
			}

			iValue = pDiplo->GetWonderSpammerScore(ePlayer);
			if (iValue != 0)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = iValue;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WONDER_SPAMMER");
				aOpinions.push_back(kOpinion);
			}
		}

		////////////////////////////////////
		// MODMOD MODIFIERS
		////////////////////////////////////

#if defined(MOD_EVENTS_DIPLO_MODIFIERS)
		iValue = pDiplo->GetDiploModifiers(ePlayer, aOpinions);
#else
		iValue = pDiplo->GetScenarioModifier1(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_SPECIFIC_DIPLO_STRING_1");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetScenarioModifier2(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_SPECIFIC_DIPLO_STRING_2");
			aOpinions.push_back(kOpinion);
		}

		iValue = pDiplo->GetScenarioModifier3(ePlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_SPECIFIC_DIPLO_STRING_3");
			aOpinions.push_back(kOpinion);
		}
#endif
	}

	//--------------------------------//
	// [PART 3: APPROACH HINT]		  //
	//--------------------------------//

	// Do not display this if the opinion table thus far is empty (this case is handled at the LUA level instead).
	if (!bObserver && !aOpinions.empty())
	{
		// Same team?
		if (bTeammate)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = -99999;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_TEAMMATE");
			aOpinions.push_back(kOpinion);
		}
		// At war?
		else if (pDiplo->IsAtWar(ePlayer))
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 99999;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_AT_WAR");
			aOpinions.push_back(kOpinion);
		}
		// If not at war and debug mode is not enabled, a hint explaining the AI's current approach is displayed
		else if (!GC.getGame().IsDiploDebugModeEnabled())
		{
			Opinion kOpinion;
			kOpinion.m_iValue = 0;
			CvString str;

			switch (eSurfaceApproach)
			{
			case CIV_APPROACH_HOSTILE:
				str = Localization::Lookup("TXT_KEY_DIPLO_HOSTILE").toUTF8();
				break;
			case CIV_APPROACH_GUARDED:
				str = Localization::Lookup("TXT_KEY_DIPLO_GUARDED").toUTF8();
				break;
			case CIV_APPROACH_AFRAID:
				str = Localization::Lookup("TXT_KEY_DIPLO_AFRAID").toUTF8();
				break;
			case CIV_APPROACH_FRIENDLY:
				str = Localization::Lookup("TXT_KEY_DIPLO_FRIENDLY").toUTF8();
				break;
			default:
				if (pDiplo->IsActHostileTowardsHuman(ePlayer))
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_NEUTRAL_HOSTILE").toUTF8();
				}
				else
				{
					str = Localization::Lookup("TXT_KEY_DIPLO_NEUTRAL_FRIENDLY").toUTF8();
				}
				break;
			}

			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}
	}

	//--------------------------------//
	// [PART 4: SORTING]			  //
	//--------------------------------//

	std::stable_sort(aOpinions.begin(), aOpinions.end(), OpinionEval());

	lua_createtable(L, 0, 0);
	int index = 1;
	std::string strOutput;
	const char* strEmpty = "";
	std::string strEndColor = "[ENDCOLOR]";
	size_t EndColorFound;
	std::string strColorPrefix = "[COLOR_";
	size_t BeginColorPrefixFound;
	std::string strColorSuffix = "]";
	size_t BeginColorSuffixFound;
	std::string strFullPositiveColor = "[COLOR_POSITIVE_TEXT]";
	std::string strPartialPositiveColor = "[COLOR_FADING_POSITIVE_TEXT]";
	std::string strNeutralColor = "[COLOR_GREY]";
	std::string strPartialNegativeColor = "[COLOR_FADING_NEGATIVE_TEXT]";
	std::string strFullNegativeColor = "[COLOR_NEGATIVE_TEXT]";

	for (uint ui = 0; ui < aOpinions.size(); ui++)
	{
		strOutput = aOpinions[ui].m_str.toUTF8();

		EndColorFound = strOutput.rfind(strEndColor);
		if (EndColorFound != string::npos)
		{
			strOutput.replace(EndColorFound, strEndColor.length(), strEmpty);
		}

		BeginColorPrefixFound = strOutput.find(strColorPrefix);
		if (BeginColorPrefixFound != string::npos)
		{
			BeginColorSuffixFound = strOutput.find(strColorSuffix);
			if (BeginColorSuffixFound != string::npos)
			{
				strOutput.erase(BeginColorPrefixFound, (BeginColorSuffixFound - BeginColorPrefixFound) + 1);
			}
		}

		if (aOpinions[ui].m_iValue > 10)
		{
			strOutput.insert(0, strFullNegativeColor);
		}
		else if (aOpinions[ui].m_iValue > 0)
		{
			strOutput.insert(0, strPartialNegativeColor);
		}
		else if (aOpinions[ui].m_iValue == 0)
		{
			strOutput.insert(0, strNeutralColor);
		}
		else if (aOpinions[ui].m_iValue >= -10)
		{
			strOutput.insert(0, strPartialPositiveColor);
		}
		else if (aOpinions[ui].m_iValue < -10)
		{
			strOutput.insert(0, strFullPositiveColor);
		}

		// Should we display the number value of opinion modifiers?
		if (bShowAllValues)
		{
			CvString strTemp;

			// Reverse the value of the opinion so as to not confuse players
			strTemp.Format(" (%d)", -(aOpinions[ui].m_iValue));

			strOutput += strTemp;
		}

		strOutput += strEndColor;

		lua_pushstring(L, strOutput.c_str());
		lua_rawseti(L, -2, index);
		index++;
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetDealValue (lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, 2);
	lua_pushinteger(L, pkThisPlayer->GetDealAI()->GetDealValue(pkDeal));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetDealMyValue(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, 2);
	if (pkThisPlayer->GetID() == pkDeal->GetFromPlayer())
		lua_pushinteger(L, pkDeal->GetFromPlayerValue());
	else
		lua_pushinteger(L, pkDeal->GetToPlayerValue());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetDealTheyreValue(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, 2);
	if (pkThisPlayer->GetID() == pkDeal->GetFromPlayer())
		lua_pushinteger(L, pkDeal->GetFromPlayerValue());
	else
		lua_pushinteger(L, pkDeal->GetToPlayerValue());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lMayNotAnnex(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	lua_pushboolean(L, pkThisPlayer->GetPlayerTraits()->IsNoAnnexing());
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTotalValueToMeNormal(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, 2);
	int iResult = 0;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague != NULL && pLeague->IsTradeEmbargoed(pkThisPlayer->GetID(), GC.getGame().getActivePlayer()))
	{
		iResult = -99999;
		lua_pushinteger(L, iResult);
		return 1;
	}
	iResult = pkThisPlayer->GetDealAI()->GetDealValue(pkDeal);
	if(iResult == INT_MAX || iResult == (INT_MAX * -1))
	{
		iResult = -1;
		lua_pushinteger(L, iResult);
		return 1;
	}

	if (pkThisPlayer->GetDealAI()->WithinAcceptableRange(pkDeal->GetOtherPlayer(pkThisPlayer->GetID()), iResult))
		iResult = 0;
 
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTotalValueToMe(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, 2);
	int iResult = 0;
	iResult = pkThisPlayer->GetDealAI()->GetDealValue(pkDeal);
	if(iResult < 0)
	{
		iResult *= -1;
	}
	if((iResult == INT_MAX) || (iResult == (INT_MAX * -1)))
	{
		iResult = -1;
		lua_pushinteger(L, iResult);
		return 1;
	}

	if (pkThisPlayer->GetDealAI()->WithinAcceptableRange(pkDeal->GetOtherPlayer(pkThisPlayer->GetID()), iResult))
		iResult = 0;

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCachedValueOfPeaceWithHuman(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	int iResult = pkThisPlayer->GetCachedValueOfPeaceWithHuman();
	if(iResult < 0)
	{
		iResult *= -1;
	}
	if(iResult == MAX_INT)
	{
		iResult = -1;
	}
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetSpyChanceAtCity(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	int iSpyIndex = lua_tointeger(L, 3);
	bool bNoBasic = lua_toboolean(L, 4);

	lua_pushstring(L, pkPlayerEspionage->GetSpyChanceAtCity(pkCity, iSpyIndex, bNoBasic));
	return 1;
}
int CvLuaPlayer::lGetCityPotentialInfo(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	bool bNoBasic = lua_toboolean(L, 3);

	lua_pushstring(L, pkPlayerEspionage->GetCityPotentialInfo(pkCity, bNoBasic));
	return 1;
}

int CvLuaPlayer::lGetCityWithSpy(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();
	int iSpyIndex = lua_tointeger(L, 2);
	CvCity* pSpyCity = pkPlayerEspionage->GetCityWithSpy(iSpyIndex);

	CvLuaCity::Push(L, pSpyCity);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetRandomIntrigue(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	int iSpyIndex = lua_tointeger(L, 3);
	
	pkPlayerEspionage->GetRandomIntrigue(pkCity, iSpyIndex);
	return 0;
}
#endif
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetEspionageCityStatus(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	lua_createtable(L, 0, 0);
	int index = 1;
	// first pass to get the largest base potential available
#if !defined(MOD_BALANCE_CORE_SPIES)
	int iLargestBasePotential = 0;
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		const PlayerTypes ePlayer(static_cast<PlayerTypes>(i));
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		if(!kPlayer.isAlive() || kPlayer.isBarbarian())
		{
			continue;
		}

		int iLoop = 0;
		for(CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
		{
			if(pkPlayerEspionage->CanEverMoveSpyTo(pCity))
			{
				CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
				int iPotential = 0;
				if (pCity->getOwner() == pkThisPlayer->GetID())
				{
					iPotential = pkPlayerEspionage->CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, -1);
				}
				else
				{
					iPotential = pCityEspionage->m_aiLastBasePotential[pkThisPlayer->GetID()];
				}

				if (iPotential > iLargestBasePotential)
				{
					iLargestBasePotential = iPotential;
				}
			}
		}
	}
#endif
	// second pass to set the values
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		const PlayerTypes ePlayer(static_cast<PlayerTypes>(i));
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		if(!kPlayer.isAlive() || kPlayer.isBarbarian())
		{
			continue;
		}

		int iLoop = 0;
		for(CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
		{
			if(pkPlayerEspionage->CanEverMoveSpyTo(pCity))
			{
#if !defined(MOD_BALANCE_CORE_SPIES)
				CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
#endif
				lua_createtable(L, 0, 0);
				const int t = lua_gettop(L);

				lua_pushinteger(L, kPlayer.GetID());
				lua_setfield(L, t, "PlayerID");

				lua_pushinteger(L, pCity->GetID());
				lua_setfield(L, t, "CityID");

				lua_pushinteger(L, pCity->getX());
				lua_setfield(L, t, "CityX");

				lua_pushinteger(L, pCity->getY());
				lua_setfield(L, t, "CityY");

				lua_pushinteger(L, kPlayer.getCivilizationType());
				lua_setfield(L, t, "CivilizationType");

				lua_pushinteger(L, kPlayer.getTeam());
				lua_setfield(L, t, "Team");

				CvString strName = pCity->getNameKey();
				lua_pushstring(L, strName.c_str());
				lua_setfield(L, t, "Name");

				lua_pushinteger(L, pCity->getPopulation());
				lua_setfield(L, t, "Population");
#if defined(MOD_BALANCE_CORE_SPIES)
				int iRate = pCity->GetEspionageRanking();
				lua_pushinteger(L, iRate);
				lua_setfield(L, t, "Potential");

				lua_pushinteger(L, iRate);
				lua_setfield(L, t, "BasePotential");

				lua_pushinteger(L, 10);
				lua_setfield(L, t, "LargestBasePotential");
#endif
#if !defined(MOD_BALANCE_CORE_SPIES)
				if(pCity->getOwner() == pkThisPlayer->GetID())
				{
					int iRate = pkPlayerEspionage->CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, -1);
					lua_pushinteger(L, iRate);
				}
				else
				{
					lua_pushinteger(L, pCityEspionage->m_aiLastPotential[pkThisPlayer->GetID()]);
				}
				lua_setfield(L, t, "Potential");

				if (pCity->getOwner() == pkThisPlayer->GetID())
				{
					lua_pushinteger(L, pkPlayerEspionage->CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, -1));
				}
				else
				{
					lua_pushinteger(L,  pCityEspionage->m_aiLastBasePotential[pkThisPlayer->GetID()]);
				}
				lua_setfield(L, t, "BasePotential");

				lua_pushinteger(L, iLargestBasePotential);
				lua_setfield(L, t, "LargestBasePotential");
#endif
				lua_rawseti(L, -2, index++);
			}
		}
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumSpies(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	const int lNumSpies = (pkPlayerEspionage != NULL)? pkPlayerEspionage->GetNumSpies() : 0;

	lua_pushinteger(L, lNumSpies);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumUnassignedSpies(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	const int lNumSpies = (pkPlayerEspionage != NULL)? pkPlayerEspionage->GetNumUnassignedSpies() : 0;

	lua_pushinteger(L, lNumSpies);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetEspionageSpies(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	lua_createtable(L, 0, 0);
	int index = 1;

	for(uint uiSpy = 0; uiSpy < pkPlayerEspionage->m_aSpyList.size(); ++uiSpy)
	{
		CvEspionageSpy* pSpy = &(pkPlayerEspionage->m_aSpyList[uiSpy]);

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lua_pushinteger(L, uiSpy);
		lua_setfield(L, t, "AgentID");

		lua_pushinteger(L, pSpy->m_iCityX);
		lua_setfield(L, t, "CityX");

		lua_pushinteger(L, pSpy->m_iCityY);
		lua_setfield(L, t, "CityY");

		const char* szSpyName = pSpy->GetSpyName(pkThisPlayer);

		lua_pushstring(L, szSpyName);
		lua_setfield(L, t, "Name");

		switch(pSpy->m_eRank)
		{
		case SPY_RANK_RECRUIT:
			lua_pushstring(L, "TXT_KEY_SPY_RANK_0");
			break;
		case SPY_RANK_AGENT:
			lua_pushstring(L, "TXT_KEY_SPY_RANK_1");
			break;
		case SPY_RANK_SPECIAL_AGENT:
			lua_pushstring(L, "TXT_KEY_SPY_RANK_2");
			break;
		default:
			CvAssertMsg(false, "pSpy->m_eRank not in case statement");
			break;
		}
		lua_setfield(L, t, "Rank");

		switch(pSpy->m_eSpyState)
		{
		case SPY_STATE_UNASSIGNED:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_UNASSIGNED");
			break;
		case SPY_STATE_TRAVELLING:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_TRAVELLING");
			break;
		case SPY_STATE_SURVEILLANCE:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_SURVEILLANCE");
			break;
		case SPY_STATE_GATHERING_INTEL:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_GATHERING_INTEL");
			break;
		case SPY_STATE_BUILDING_NETWORK:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_BUILDING_NETWORK");
			break;
		case SPY_STATE_RIG_ELECTION:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_RIGGING_ELECTION");
			break;
		case SPY_STATE_COUNTER_INTEL:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_COUNTER_INTEL");
			break;
		case SPY_STATE_DEAD:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_DEAD");
			break;
		case SPY_STATE_MAKING_INTRODUCTIONS:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_MAKING_INTRODUCTIONS");
			break;
		case SPY_STATE_SCHMOOZE:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_SCHMOOZING");
			break;
		case SPY_STATE_TERMINATED:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_TERMINATED");
			break;
		default:
			CvAssertMsg(false, "pSpy->m_eSpyState not in case statement");
			break;
		}
		lua_setfield(L, t, "State");

		lua_pushinteger(L, pkPlayerEspionage->GetTurnsUntilStateComplete(uiSpy));
		lua_setfield(L, t, "TurnsLeft");

		lua_pushinteger(L, pkPlayerEspionage->GetPercentOfStateComplete(uiSpy));
		lua_setfield(L, t, "PercentComplete");

		lua_pushboolean(L, pkPlayerEspionage->HasEstablishedSurveillance(uiSpy));
		lua_setfield(L, t, "EstablishedSurveillance");

		lua_pushboolean(L, pkPlayerEspionage->IsDiplomat(uiSpy));
		lua_setfield(L, t, "IsDiplomat");

		lua_pushboolean(L, pSpy->m_bPassive);
		lua_setfield(L, t, "Passive");
		lua_rawseti(L, -2, index++);
	}
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lEspionageCreateSpy(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	pkPlayer->GetEspionage()->CreateSpy();

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lEspionagePromoteSpy(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	int iSpyIndex = lua_tointeger(L, 2);
	pkPlayer->GetEspionage()->LevelUpSpy(iSpyIndex);

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lEspionageSetPassive(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	int iSpyIndex = lua_tointeger(L, 2);
	bool bPassive = lua_toboolean(L, 3);
	pkPlayer->GetEspionage()->SetPassive(iSpyIndex, bPassive);

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lEspionageSetOutcome(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	int iSpyIndex = lua_tointeger(L, 2);
	int iSpyResult = lua_tointeger(L, 3);
	bool bAffectsDiplomacy = lua_toboolean(L, 4);
	pkPlayer->GetEspionage()->SetOutcome(iSpyIndex, iSpyResult, bAffectsDiplomacy);

	return 0;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lHasSpyEstablishedSurveillance(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	int iSpyIndex = lua_tointeger(L, 2);
	bool bSurveillance = pkPlayer->GetEspionage()->HasEstablishedSurveillance(iSpyIndex);

	lua_pushboolean(L, bSurveillance);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsSpyDiplomat(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	int iSpyIndex = lua_tointeger(L, 2);
	bool bIsDiplomat = pkPlayer->GetEspionage()->IsDiplomat(iSpyIndex);

	lua_pushboolean(L, bIsDiplomat);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lIsSpySchmoozing(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	int iSpyIndex = lua_tointeger(L, 2);
	bool bIsDiplomat = pkPlayer->GetEspionage()->IsSchmoozing(iSpyIndex);

	lua_pushboolean(L, bIsDiplomat);
	return 1;	
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lCanSpyStageCoup(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	int iSpyIndex = lua_tointeger(L, 2);
	bool bCanStageCoup = pkPlayer->GetEspionage()->CanStageCoup(iSpyIndex);

	lua_pushboolean(L, bCanStageCoup);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetAvailableSpyRelocationCities(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	uint uiSpyIndex = luaL_checkinteger(L, 2);

	lua_createtable(L, 0, 0);
	int index = 1;

	for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		const PlayerTypes ePlayer(static_cast<PlayerTypes>(i));
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		if(kPlayer.isAlive())
		{
			int iLoop = 0;
			// Just find first coastal city
			for(CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				if(pkPlayerEspionage->CanMoveSpyTo(pCity, uiSpyIndex, false))
				{
					lua_createtable(L, 0, 0);
					const int t = lua_gettop(L);

					lua_pushinteger(L, kPlayer.GetID());
					lua_setfield(L, t, "PlayerID");

					lua_pushinteger(L, pCity->GetID());
					lua_setfield(L, t, "CityID");

					lua_pushinteger(L, kPlayer.getCivilizationType());
					lua_setfield(L, t, "CivilizationType");

					lua_pushinteger(L, kPlayer.getTeam());
					lua_setfield(L, t, "Team");

					CvString strName = pCity->getName();
					lua_pushstring(L, strName.c_str());
					lua_setfield(L, t, "Name");

					lua_pushinteger(L, pCity->getPopulation());
					lua_setfield(L, t, "Population");

					//TODO: Replace temp 99 w/ City Potential Espionage Value.
					lua_pushinteger(L, 99);
					lua_setfield(L, t, "Potential");

					lua_rawseti(L, -2, index++);
				}
			}
		}
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNumTechsToSteal(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	int iPlayer = lua_tointeger(L, 2);
	CvAssertMsg(iPlayer >= 0 && iPlayer < MAX_MAJOR_CIVS, "iPlayer out of bounds");
	PlayerTypes ePlayer = (PlayerTypes)iPlayer;
	lua_pushinteger(L, pkPlayerEspionage->GetNumTechsToSteal(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetIntrigueMessages(lua_State* L)
{
	lua_createtable(L, 0, 0);
	int index = 1;

	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	for(int i = pkPlayerEspionage->m_aIntrigueNotificationMessages.size(); i > 0; i--)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		Localization::String strIntrigueMessage = pkPlayerEspionage->GetIntrigueMessage(i - 1);
		const char* szIntrigueMessage = strIntrigueMessage.toUTF8();
		lua_pushstring(L, szIntrigueMessage);
		lua_setfield(L, t, "String");
		lua_pushinteger(L, pkPlayerEspionage->m_aIntrigueNotificationMessages[i - 1].m_iTurnNum);
		lua_setfield(L, t, "Turn");
		lua_pushinteger(L, pkPlayerEspionage->m_aIntrigueNotificationMessages[i - 1].m_eDiscoveringPlayer);
		lua_setfield(L, t, "DiscoveringPlayer");
		const char* szIntrigueSpy = pkPlayerEspionage->m_aIntrigueNotificationMessages[i - 1].m_strSpyName.c_str();
		lua_pushstring(L, szIntrigueSpy);
		lua_setfield(L, t, "SpyName");

		lua_rawseti(L, -2, index++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lHasRecentIntrigueAbout(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	int iPlayer = lua_tointeger(L, 2);
	CvAssertMsg(iPlayer >= 0 && iPlayer < MAX_MAJOR_CIVS, "iPlayer out of bounds");
	PlayerTypes ePlayer = (PlayerTypes)iPlayer;
	lua_pushboolean(L, pkPlayerEspionage->HasRecentIntrigueAbout(ePlayer));

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetRecentIntrigueInfo(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	int iPlayer = lua_tointeger(L, 2);
	CvAssertMsg(iPlayer >= 0 && iPlayer < MAX_MAJOR_CIVS, "iPlayer out of bounds");
	PlayerTypes eTargetPlayer = (PlayerTypes)iPlayer;
	IntrigueNotificationMessage* pNotificationMessage = pkPlayerEspionage->GetRecentIntrigueInfo(eTargetPlayer);

	PlayerTypes ePlotter = NO_PLAYER;
	CvIntrigueType eIntrigueType = NUM_INTRIGUE_TYPES;
	if (pNotificationMessage)
	{
		ePlotter = pNotificationMessage->m_eSourcePlayer;
		eIntrigueType = (CvIntrigueType)pNotificationMessage->m_iIntrigueType;
	}

	lua_pushinteger(L, ePlotter);
	lua_pushinteger(L, eIntrigueType);

	return 2;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCoupChanceOfSuccess(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);

	int iSpyIndex = pkPlayerEspionage->GetSpyIndexInCity(pkCity);
	CvAssertMsg(iSpyIndex >= 0, "iSpyIndex out of bounds");
	if(iSpyIndex < 0)
	{
		lua_pushinteger(L, 0);
		return 1;
	}

	lua_pushinteger(L, pkPlayerEspionage->GetCoupChanceOfSuccess(iSpyIndex));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsMyDiplomatVisitingThem(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayerEspionage->IsMyDiplomatVisitingThem(eOtherPlayer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsOtherDiplomatVisitingMe(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayerEspionage->IsOtherDiplomatVisitingMe(eOtherPlayer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetTradeRouteRange(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	const DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 2);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3);

	CvPlayerTrade* pkPlayerTrade = pkThisPlayer->GetTrade();
	lua_pushinteger(L, pkPlayerTrade->GetTradeRouteRange(eDomain, pkCity));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lCanCommitVote(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	const PlayerTypes eToPlayer = (PlayerTypes) lua_tointeger(L, 2);

	CvLeagueAI* pkPlayerLeagueAI = pkThisPlayer->GetLeagueAI();
	lua_pushboolean(L, pkPlayerLeagueAI->CanCommitVote(eToPlayer));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetCommitVoteDetails(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	const PlayerTypes eToPlayer = (PlayerTypes) lua_tointeger(L, 2);

	CvLeagueAI* pkPlayerLeagueAI = pkThisPlayer->GetLeagueAI();
	lua_pushstring(L, pkPlayerLeagueAI->GetCommitVoteDetails(eToPlayer));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lIsConnected(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isConnected);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lIsObserver(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::isObserver);
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lHasTurnTimerExpired(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::hasTurnTimerExpired);
}

//-------------------------------------------------------------------------
int CvLuaPlayer::lHasUnitOfClassType(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	
	UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(luaL_checkint(L, 2));

	bool bResult = false;
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = pkThisPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pkThisPlayer->nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL && pLoopUnit->getUnitClassType() == eUnitClass)
		{
			bResult = true;
			break;
		}
	}

	lua_pushboolean(L, bResult);
	return 1;
}

//-------------------------------------------------------------------------
int CvLuaPlayer::lGetWarmongerPreviewString(lua_State* L)
{
	const PlayerTypes eOwner = (PlayerTypes) lua_tointeger(L, 2);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3, false);
	if(pkCity)
	{
		const PlayerTypes eToPlayer = (PlayerTypes)luaL_optint(L, 5, NO_PLAYER);
		lua_pushstring(L, CvDiplomacyAIHelpers::GetWarmongerPreviewString(eOwner, pkCity, eToPlayer));
	}
	else
	{
		const PlayerTypes eToPlayer = (PlayerTypes)luaL_optint(L, 5, NO_PLAYER);
		lua_pushstring(L, CvDiplomacyAIHelpers::GetWarmongerPreviewString(eOwner, NULL, eToPlayer));
	}
	return 1;
}

int CvLuaPlayer::lGetLiberationPreviewString(lua_State* L)
{
	const PlayerTypes eOriginalOwner = (PlayerTypes) lua_tointeger(L, 2);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3);
	if(pkCity)
	{
		const PlayerTypes eToPlayer = (PlayerTypes)luaL_optint(L, 5, NO_PLAYER);
		lua_pushstring(L, CvDiplomacyAIHelpers::GetLiberationPreviewString(eOriginalOwner, pkCity, eToPlayer));
	}
	else
	{
		const PlayerTypes eToPlayer = (PlayerTypes)luaL_optint(L, 5, NO_PLAYER);
		lua_pushstring(L, CvDiplomacyAIHelpers::GetLiberationPreviewString(eOriginalOwner, NULL, eToPlayer));
	}
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lAddMessage(lua_State* L)
{
	SHOW_PLAYER_MESSAGE(GetInstance(L), lua_tostring(L, 2));
	return 0;
}
LUAAPIIMPL(Player, CountAllFeature)
LUAAPIIMPL(Player, CountAllWorkedFeature)
LUAAPIIMPL(Player, CountAllImprovement)
LUAAPIIMPL(Player, CountAllWorkedImprovement)
LUAAPIIMPL(Player, CountAllPlotType)
LUAAPIIMPL(Player, CountAllWorkedPlotType)
LUAAPIIMPL(Player, CountAllResource)
LUAAPIIMPL(Player, CountAllWorkedResource)
LUAAPIIMPL(Player, CountAllTerrain)
LUAAPIIMPL(Player, CountAllWorkedTerrain)

#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
//-------------------------------------------------------------------------
int CvLuaPlayer::lGetResponsibleForRouteCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const RouteTypes eRoute = (RouteTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->getResponsibleForRouteCount(eRoute);
	lua_pushinteger(L, iResult);
	return 1;
}
//-------------------------------------------------------------------------
int CvLuaPlayer::lGetResponsibleForImprovementCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->getResponsibleForImprovementCount(eImprovement);
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//-------------------------------------------------------------------------
int CvLuaPlayer::lGetYieldPerTurnFromMinors(lua_State* L)
{
CvPlayerAI* pkPlayer = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetYieldPerTurnFromMinors(eYield);
	lua_pushinteger(L, iResult);
	return 1;
}
//-------------------------------------------------------------------------
int CvLuaPlayer::lSetYieldPerTurnFromMinors(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = lua_tointeger(L, 3);
	pkPlayer->SetYieldPerTurnFromMinors(eYield, iValue);
	return 1;
}
//-------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromMinorAllies(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromMinorAllies);
}
//-------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromMilitarySize(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromMilitarySize);
}
//-------------------------------------------------------------------------
int CvLuaPlayer::lGetScoreFromVassals(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetScoreFromVassals);
}
//-------------------------------------------------------------------------------
//int GetHappinessFromVassals();
int CvLuaPlayer::lGetHappinessFromVassals(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetHappinessFromVassals);
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromVassals();
int CvLuaPlayer::lGetYieldPerTurnFromVassals(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const YieldTypes eYield = (YieldTypes) lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetYieldPerTurnFromVassals(eYield);
	lua_pushinteger(L, iResult);
	return 1;
}
//-----------------------------------------------------------------------------
//int GetVassalGoldMaintenance();
int CvLuaPlayer::lGetVassalGoldMaintenance(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	const int iResult = pkPlayer->GetTreasury()->GetVassalGoldMaintenance();
	lua_pushinteger(L, iResult);
	return 1;
}
//-----------------------------------------------------------------------------
int CvLuaPlayer::lIsVassalageAcceptable(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsVassalageAcceptable(eOtherPlayer, false);

	lua_pushboolean(L, bValue);

	return 1;
}
// -------------------
int CvLuaPlayer::lGetMilitaryAggressivePosture(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetMilitaryAggressivePosture(eOtherPlayer));
	return 1;
}
// --------------------
int CvLuaPlayer::lMoveRequestTooSoon(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	lua_pushboolean(L, pkPlayer->GetDiplomacyAI()->IsTooSoonForMoveTroopsRequest(eOtherPlayer));
	return 1;
}

int CvLuaPlayer::lGetPlayerMoveTroopsRequestCounter(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	int iTurn = pkPlayer->GetDiplomacyAI()->GetPlayerMoveTroopsRequestAcceptedTurn(eOtherPlayer);
	const int iValue = (iTurn != -1) ? (GC.getGame().getGameTurn() - iTurn) : -1;

	lua_pushinteger(L, iValue);
	return 1;
}

// CvTreasury::GetExpensePerTurnFromVassalTaxes()
int CvLuaPlayer::lGetExpensePerTurnFromVassalTaxes(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetExpensePerTurnFromVassalTaxes());
	return 1;
}

// CvTreasury::GetMyShareOfVassalTaxes()
int CvLuaPlayer::lGetMyShareOfVassalTaxes(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetMyShareOfVassalTaxes());
	return 1;
}

// CvTreasury::GetVassalTaxContribution(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalTaxContribution(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetVassalTaxContribution(eOtherPlayer));
	return 1;
}

// CvDiplomacyAI::GetVassalScore(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalScore(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	
	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetVassalScore(eOtherPlayer));
	return 1;
}

// CvDiplomacyAI::GetVassalTreatedScore(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalTreatedScore(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetVassalTreatedScore(eOtherPlayer));
	return 1;
}

// CvDiplomacyAI::GetVassalDemandScore(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalDemandScore(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	
	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetVassalDemandScore(eOtherPlayer));
	return 1;
}

// CvDiplomacyAI::GetVassalTaxScore(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalTaxScore(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	
	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetVassalTaxScore(eOtherPlayer));
	return 1;
}

// CvDiplomacyAI::GetVassalProtectScore(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalProtectScore(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	
	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetVassalProtectScore(eOtherPlayer));
	return 1;
}

// CvDiplomacyAI::GetVassalFailedProtectScore(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalFailedProtectScore(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	
	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetVassalFailedProtectScore(eOtherPlayer));
	return 1;
}

// CvDiplomacyAI::GetVassalTreatmentLevel(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalTreatmentLevel(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	VassalTreatmentTypes eResult = pkPlayer->GetDiplomacyAI()->GetVassalTreatmentLevel(eOtherPlayer);
	lua_pushinteger(L, (int)eResult);
	return 1;
}

// CvDiplomacyAI::GetVassalTreatmentToolTip(PlayerTypes ePlayer)
int CvLuaPlayer::lGetVassalTreatmentToolTip(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushstring(L, pkPlayer->GetDiplomacyAI()->GetVassalTreatmentToolTip(ePlayer));
	return 1;
}


int CvLuaPlayer::lGetVassalIndependenceTooltipAsMaster(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushstring(L, pkPlayer->GetVassalIndependenceTooltipAsMaster(ePlayer));
	return 1;
}

int CvLuaPlayer::lGetVassalIndependenceTooltipAsVassal(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetVassalIndependenceTooltipAsVassal());
	return 1;
}

LUAAPIIMPL(Player, HasBelief)
LUAAPIIMPL(Player, HasBuilding)
LUAAPIIMPL(Player, HasBuildingClass)
LUAAPIIMPL(Player, HasAnyWonder)
LUAAPIIMPL(Player, HasWonder)
LUAAPIIMPL(Player, IsCivilization)
LUAAPIIMPL(Player, IsInEra)
LUAAPIIMPL(Player, HasReachedEra)
LUAAPIIMPL(Player, HasAnyNaturalWonder)
LUAAPIIMPL(Player, HasNaturalWonder)
// LUAAPIIMPL(Player, HasPolicy)
LUAAPIIMPL(Player, HasTenet)
LUAAPIIMPL(Player, HasPolicyBranch)
LUAAPIIMPL(Player, HasIdeology)
LUAAPIIMPL(Player, HasProject)
LUAAPIIMPL(Player, IsAtPeace)
LUAAPIIMPL(Player, IsAtPeaceAllMajors)
LUAAPIIMPL(Player, IsAtPeaceAllMinors)
LUAAPIIMPL(Player, IsAtPeaceWith)
LUAAPIIMPL(Player, IsAtWar)
LUAAPIIMPL(Player, IsAtWarAnyMajor)
LUAAPIIMPL(Player, IsAtWarAnyMinor)
LUAAPIIMPL(Player, IsAtWarWith)
LUAAPIIMPL(Player, HasPantheon)
LUAAPIIMPL(Player, HasAnyReligion)
LUAAPIIMPL(Player, HasReligion)
LUAAPIIMPL(Player, HasEnhancedReligion)
LUAAPIIMPL(Player, IsConnectedTo)
LUAAPIIMPL(Player, HasSpecialistSlot)
LUAAPIIMPL(Player, HasSpecialist)
LUAAPIIMPL(Player, HasTech)
LUAAPIIMPL(Player, HasAnyDomesticTradeRoute)
LUAAPIIMPL(Player, HasAnyInternationalTradeRoute)
LUAAPIIMPL(Player, HasAnyTradeRoute)
LUAAPIIMPL(Player, HasAnyTradeRouteWith)
LUAAPIIMPL(Player, HasUnit)
LUAAPIIMPL(Player, HasUnitClass)

LUAAPIIMPL(Player, HasTrait)
LUAAPIIMPL(Player, HasAnyHolyCity)
LUAAPIIMPL(Player, HasHolyCity)
LUAAPIIMPL(Player, HasCapturedHolyCity)
LUAAPIIMPL(Player, HasEmbassyWith)

//------------------------------------------------------------------------------
//void DoForceDefPact(int iOtherPlayer);
int CvLuaPlayer::lDoForceDefPact(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	if(eOtherPlayer != NO_PLAYER)
	{
		CvTeam& pOtherTeam = GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam());

		GET_TEAM(pkPlayer->getTeam()).SetHasDefensivePact(pOtherTeam.GetID(), true);
		pOtherTeam.SetHasDefensivePact(pkPlayer->getTeam(), true);
	}
	return 1;
}

int CvLuaPlayer::lGetWLTKDResourceTT(lua_State* L)
{
	CvString WLTKDTT = "";
	CvPlayer* pkPlayer = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
	if (eResource != NO_RESOURCE)
	{
		CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = pkPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iLoop))
		{
			if (pLoopCity == NULL)
				continue;

			if (pLoopCity->GetWeLoveTheKingDayCounter() > 0)
				continue;

			if (pLoopCity->GetResourceDemanded() == eResource)
			{
				if (WLTKDTT == "")
					WLTKDTT = Localization::Lookup("TXT_KEY_TRADE_WLTKD_RESOURCE_CITIES").toUTF8();

				WLTKDTT += "[NEWLINE][ICON_BULLET] ";
				WLTKDTT += pLoopCity->getName();
			}
		}
	}

	lua_pushstring(L, WLTKDTT.c_str());
	return 1;
}
int CvLuaPlayer::lGetNumNationalWonders(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	CvCity* pLoopCity;
	int iLoop;
	int iResult = 0;
	for(pLoopCity = pkPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iLoop))
	{
		if(pLoopCity == NULL)
			continue;

		iResult += pLoopCity->getNumNationalWonders();
	}
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetNumInternationalTradeRoutes(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetNumInternationalRoutes();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetNumInternalTradeRoutes(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetNumInternalRoutes();
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaPlayer::lGetCivOpinion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetDiplomacyAI()->GetCivOpinion(ePlayer);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetMajorityReligion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const ReligionTypes eReligion = pkPlayer->GetReligions()->GetReligionInMostCities();
	lua_pushinteger(L, eReligion);
	return 1;
}
int CvLuaPlayer::lGetStateReligion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const ReligionTypes eReligion = pkPlayer->GetReligions()->GetStateReligion();
	lua_pushinteger(L, eReligion);
	return 1;
}
int CvLuaPlayer::lGetNumCitiesWithStateReligion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	const int iValue = pkPlayer->GetReligions()->GetNumCitiesWithStateReligion(eReligion);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaPlayer::lGetNumCitiesWithoutStateReligion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	int iCities = pkPlayer->getNumCities();
	int iReligious = pkPlayer->GetReligions()->GetNumCitiesWithStateReligion(eReligion);
	int iValue = (iCities - iReligious);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaPlayer::lSetStateReligion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	pkPlayer->GetReligions()->SetStateReligion(eReligion);
	return 1;
}
int CvLuaPlayer::lGetPiety(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPiety();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetPiety(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetPiety(iValue);
	return 1;
}
int CvLuaPlayer::lChangePiety(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->ChangePiety(iValue);
	return 1;
}
int CvLuaPlayer::lGetPietyRate(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPietyRate();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetPietyRate(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetPietyRate(iValue);
	return 1;
}
int CvLuaPlayer::lChangePietyRate(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->ChangePietyRate(iValue);
	return 1;
}
int CvLuaPlayer::lGetTurnsSinceConversion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetTurnsSinceConversion();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetTurnsSinceConversion(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetTurnsSinceConversion(iValue);
	return 1;
}
int CvLuaPlayer::lHasStateReligion(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->HasStateReligion();
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lHasSecularized(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->HasSecularized();
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lSetHasSecularized(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const bool bValue = lua_toboolean(L, 2);

	pkPlayer->SetHasSecularized(bValue);
	return 1;
}
int CvLuaPlayer::lIsPagan(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->IsPagan();
	lua_pushboolean(L, bResult);
	return 1;
}

int CvLuaPlayer::lGetSovereignty(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetSovereignty();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetSovereignty(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetSovereignty(iValue);
	return 1;
}
int CvLuaPlayer::lChangeSovereignty(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->ChangeSovereignty(iValue);
	return 1;
}
int CvLuaPlayer::lGetGovernment(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetGovernment();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetGovernment(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetGovernment(iValue);
	return 1;
}
int CvLuaPlayer::lHasGovernment(lua_State* L)
{

	CvPlayer* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->HasGovernment();
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lGetReformCooldown(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetReformCooldown();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetReformCooldown(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetReformCooldown(iValue);
	return 1;
}
int CvLuaPlayer::lChangeReformCooldown(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->ChangeReformCooldown(iValue);
	return 1;
}

int CvLuaPlayer::lGetGovernmentCooldown(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetGovernmentCooldown();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetGovernmentCooldown(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetGovernmentCooldown(iValue);
	return 1;
}
int CvLuaPlayer::lChangeGovernmentCooldown(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->ChangeGovernmentCooldown(iValue);
	return 1;
}

int CvLuaPlayer::lGetReformCooldownRate(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetReformCooldownRate();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetReformCooldownRate(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetReformCooldownRate(iValue);
	return 1;
}
int CvLuaPlayer::lChangeReformCooldownRate(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->ChangeReformCooldownRate(iValue);
	return 1;
}

int CvLuaPlayer::lGetGovernmentCooldownRate(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetGovernmentCooldownRate();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetGovernmentCooldownRate(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetGovernmentCooldownRate(iValue);
	return 1;
}
int CvLuaPlayer::lChangeGovernmentCooldownRate(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->ChangeGovernmentCooldownRate(iValue);
	return 1;
}

int CvLuaPlayer::lGetPoliticLeader(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetPoliticLeader();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetPoliticLeader(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetPoliticLeader(iValue);
	return 1;
}

int CvLuaPlayer::lGetPoliticLeaderKey(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetPoliticLeaderKey());
	return 1;
}
int CvLuaPlayer::lSetPoliticLeaderKey(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const char* strKey = lua_tostring(L, 2);

	pkPlayer->SetPoliticLeaderKey(strKey);
	return 1;
}

int CvLuaPlayer::lGetLegislatureName(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetLegislatureName());
	return 1;
}
int CvLuaPlayer::lSetLegislatureName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const char* strKey = lua_tostring(L, 2);

	pkPlayer->SetLegislatureName(strKey);
	return 1;
}

int CvLuaPlayer::lGetPoliticPercent(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iID = lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetPoliticPercent(iID);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetPoliticPercent(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iID = lua_tointeger(L, 2);
	const int iValue = lua_tointeger(L, 3);

	pkPlayer->SetPoliticPercent(iID, iValue);
	return 1;
}

int CvLuaPlayer::lGetCurrencyName(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	lua_pushstring(L, pkPlayer->GetCurrencyName());
	return 1;
}
int CvLuaPlayer::lSetCurrencyName(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const char* strKey = lua_tostring(L, 2);

	pkPlayer->SetCurrencyName(strKey);
	return 1;
}

int CvLuaPlayer::lGetCurrency(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetCurrency();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetCurrency(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iID = lua_tointeger(L, 2);

	pkPlayer->SetCurrency(iID);
	return 1;
}
int CvLuaPlayer::lHasCurrency(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->HasCurrency();
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lGetProsperityScore(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetProsperityScore();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lSetProsperityScore(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkPlayer->SetProsperityScore(iValue);
	return 1;
}

//Contracts
int CvLuaPlayer::lPlayerHasContract(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlayer->GetContracts()->PlayerHasContract(eContract);
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lPlayerHasAnyContract(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const bool bResult = pkPlayer->GetContracts()->PlayerHasAnyContract();
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lGetContractTurnsRemaining(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);
	const int iResult = pkPlayer->GetContracts()->GetContractTurnsRemaining(eContract);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lGetContractGoldMaintenance(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetTreasury()->GetContractGoldMaintenance();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lChangeContractTurns(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);
	const int iValue = lua_tointeger(L, 3);

	pkPlayer->GetContracts()->ChangeContractEndTurn(eContract, iValue);
	return 1;
}

int CvLuaPlayer::lStartContract(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);

	pkPlayer->GetContracts()->StartContract(eContract);
	return 1;
}
int CvLuaPlayer::lEndContract(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);

	GC.getGame().GetGameContracts()->EndContract(eContract, pkPlayer->GetID());
	return 1;
}
int CvLuaPlayer::lUnitIsActiveContractUnit(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const bool bResult = pkPlayer->GetContracts()->UnitIsActiveContractUnit(eUnit);
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lGetNumActivePlayerContracts(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetContracts()->GetNumActivePlayerContracts();
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaPlayer::lInitContractUnits(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);
	pkPlayer->GetContracts()->InitContractUnits(eContract);
	return 0;
}
int CvLuaPlayer::lDisbandContractUnits(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);
	pkPlayer->GetContracts()->DisbandContractUnits(eContract);
	return 0;
}

int CvLuaPlayer::lDoInstantYield(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iYield = lua_tointeger(L, 3);
	const bool bSuppress = luaL_optbool(L, 4, false);
	int iCity = luaL_optint(L, 5, -1);

	CvCity* pkCity = NULL;
	if (iCity != -1)
	{
		pkCity = pkPlayer->getCity(iCity);

		//sometimes Lua city IDs are actually sequential indices
		//global IDs start at 1000
		if (!pkCity && iCity < 1000)
		{
			if (iCity > 0)
			{
				iCity--;
				pkCity = pkPlayer->nextCity(&iCity);
			}
			else
			{
				pkCity = pkPlayer->firstCity(&iCity);
			}
		}
	}

	pkPlayer->doInstantYield(INSTANT_YIELD_TYPE_LUA, false, NO_GREATPERSON, NO_BUILDING, iYield, false, NO_PLAYER, NULL, bSuppress, pkCity, false, true, false, eYield);

	return 1;
}
int CvLuaPlayer::lGetInstantYieldHistoryTooltip(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const int Turns = lua_tointeger(L, 2);
	const CvString string = pkPlayer->getInstantYieldHistoryTooltip(GC.getGame().getGameTurn(), Turns);

	lua_pushstring(L, string.c_str());
	return 1;
}

#if defined(MOD_BALANCE_CORE_EVENTS)
int CvLuaPlayer::lGetDisabledTooltip(lua_State* L)
{
	CvString DisabledTT = "";
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEventChoice = (EventChoiceTypes)lua_tointeger(L, 2);
	if(eEventChoice != NO_EVENT_CHOICE)
	{
		DisabledTT = pkPlayer->GetDisabledTooltip(eEventChoice);
	}

	lua_pushstring(L, DisabledTT.c_str());
	return 1;
}
int CvLuaPlayer::lGetEspionageValues(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	CvString CoreYieldTip = "";
	const CityEventTypes eEvent = (CityEventTypes)lua_tointeger(L, 2);
	const int uiSpyIndex = lua_tointeger(L, 3);
	if (eEvent != NO_EVENT)
	{
		CoreYieldTip = pkPlayer->GetEspionage()->GetEventHelpText(eEvent, uiSpyIndex);
	}

	lua_pushstring(L, CoreYieldTip.c_str());
	return 1;
}
int CvLuaPlayer::lGetScaledEventChoiceValue(lua_State* L)
{
	CvString CoreYieldTip = "";
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEventChoice = (EventChoiceTypes)lua_tointeger(L, 2);
	const bool bYieldsOnly = lua_toboolean(L, 3);
	if(eEventChoice != NO_EVENT_CHOICE)
	{
		CoreYieldTip = pkPlayer->GetScaledHelpText(eEventChoice, bYieldsOnly);
	}

	lua_pushstring(L, CoreYieldTip.c_str());
	return 1;
}
int CvLuaPlayer::lIsEventChoiceActive(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEventChoice = (EventChoiceTypes)lua_tointeger(L, 2);
	const bool bInstantEvents = luaL_optbool(L, 3, false);
	bool bResult = false;
	if(eEventChoice != NO_EVENT_CHOICE)
	{
		CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eEventChoice);
		if(pkEventChoiceInfo != NULL)
		{
			if(pkPlayer->IsEventChoiceActive(eEventChoice))
			{
				if(bInstantEvents)
				{
					if(!pkEventChoiceInfo->isOneShot() && !pkEventChoiceInfo->Expires())
					{
						for(int iLoop = 0; iLoop < GC.getNumEventInfos(); iLoop++)
						{
							EventTypes eEvent = (EventTypes)iLoop;
							if(eEvent != NO_EVENT)
							{
								if(pkEventChoiceInfo->isParentEvent(eEvent))
								{
									CvModEventInfo* pkEventInfo = GC.getEventInfo(eEvent);
									if(pkEventInfo != NULL)
									{
										if(pkEventInfo->getNumChoices() == 1)
										{
											bResult = true;
											break;
										}
									}
								}
							}
						}
					}
				}
				else
				{
					if(pkEventChoiceInfo->isOneShot() || pkEventChoiceInfo->Expires())
					{
						bResult = true;
					}
				}
			}
		}
	}
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaPlayer::lDoEventChoice(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEventChoice = (EventChoiceTypes)lua_tointeger(L, 2);
	pkPlayer->DoEventChoice(eEventChoice);
	return 1;
}
int CvLuaPlayer::lDoStartEvent(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventTypes eEvent = (EventTypes)lua_tointeger(L, 2);
	pkPlayer->DoStartEvent(eEvent);
	return 1;
}
int CvLuaPlayer::lDoCancelEventChoice(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEvent = (EventChoiceTypes)lua_tointeger(L, 2);
	pkPlayer->DoCancelEventChoice(eEvent);
	return 1;
}
int CvLuaPlayer::lGetEventCooldown(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventTypes eEvent = (EventTypes)lua_tointeger(L, 2);
	CvModEventInfo* pkEventInfo = GC.getEventInfo(eEvent);
	if(pkEventInfo != NULL && pkEventInfo->isOneShot())
	{
		lua_pushinteger(L, -1);
		return 1;
	}
	const int iCooldown = pkPlayer->GetEventCooldown(eEvent);
	lua_pushinteger(L, iCooldown);
	return 1;
}
int CvLuaPlayer::lGetEventChoiceCooldown(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEvent = (EventChoiceTypes)lua_tointeger(L, 2);
	CvModEventChoiceInfo* pkEventInfo = GC.getEventChoiceInfo(eEvent);
	if(pkEventInfo != NULL && pkEventInfo->isOneShot())
	{
		lua_pushinteger(L, -1);
		return 1;
	}
	const int iCooldown = pkPlayer->GetEventChoiceDuration(eEvent);
	lua_pushinteger(L, iCooldown);
	return 1;
}
int CvLuaPlayer::lSetEventCooldown(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventTypes eEvent = (EventTypes)lua_tointeger(L, 2);
	int iNewCooldown = lua_tointeger(L, 3);
	pkPlayer->SetEventCooldown(eEvent, iNewCooldown);
	return 1;
}
int CvLuaPlayer::lSetEventChoiceCooldown(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEvent = (EventChoiceTypes)lua_tointeger(L, 2);
	int iNewCooldown = lua_tointeger(L, 3);
	pkPlayer->SetEventChoiceDuration(eEvent, iNewCooldown);
	return 1;
}
int CvLuaPlayer::lIsEventChoiceValid(lua_State* L)
{
	CvPlayer* pkPlayer = GetInstance(L);
	const EventChoiceTypes eEventChoice = (EventChoiceTypes)lua_tointeger(L, 2);
	const EventTypes eEvent = (EventTypes)lua_tointeger(L, 3);
	const bool bValue = pkPlayer->IsEventChoiceValid(eEventChoice, eEvent);

	lua_pushboolean(L, bValue);

	return 1;
}


int CvLuaPlayer::lGetEventHappiness(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iHappiness = 0;
	CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = pkPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iLoop))
	{
		iHappiness += pLoopCity->GetEventHappiness();
	}
	lua_pushinteger(L, iHappiness);
	return 1;
}

int CvLuaPlayer::lGetActivePlayerEventChoices(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	
	lua_createtable(L, 0, 0);
	int idx = 1;

	for (int iI = 0; iI < GC.getNumEventChoiceInfos(); iI++)
	{
		EventChoiceTypes eEventChoice = (EventChoiceTypes)iI;
		if (eEventChoice == NO_EVENT_CHOICE)
			continue;

		CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eEventChoice);
		if (pkEventChoiceInfo != NULL)
		{
			if (pkPlayer->IsEventChoiceActive(eEventChoice))
			{
				EventTypes eParentEvent = NO_EVENT;
				bool bNotChoice = false;
				for (int iLoop = 0; iLoop < GC.getNumEventInfos(); iLoop++)
				{
					EventTypes eEvent = (EventTypes)iLoop;
					if (eEvent != NO_EVENT)
					{
						if (pkEventChoiceInfo->isParentEvent(eEvent))
						{
							eParentEvent = eEvent;
							CvModEventInfo* pkEventInfo = GC.getEventInfo(eEvent);
							if (pkEventInfo != NULL)
							{
								if (pkEventInfo->getNumChoices() == 1)
								{
									bNotChoice = true;
								}
								break;
							}
						}
					}
				}
				if (bNotChoice)
					continue;			

				int iDuration = pkPlayer->GetEventChoiceDuration(eEventChoice);
				if (!pkEventChoiceInfo->Expires())
				{ 
					iDuration = -1;
				}

				lua_createtable(L, 0, 0);
				const int t = lua_gettop(L);

				lua_pushinteger(L, eEventChoice);
				lua_setfield(L, t, "EventChoice");
				lua_pushinteger(L, iDuration);
				lua_setfield(L, t, "Duration");
				lua_pushinteger(L, eParentEvent);
				lua_setfield(L, t, "ParentEvent");

				lua_rawseti(L, -2, idx++);
			}
		}
	}

	return 1;
}
int CvLuaPlayer::lGetActiveCityEventChoices(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_createtable(L, 0, 0);
	int idx = 1;

	for (int iI = 0; iI < GC.getNumCityEventChoiceInfos(); iI++)
	{
		CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)iI;
		if (eEventChoice == NO_EVENT_CHOICE_CITY)
			continue;

		CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
		if (pkEventChoiceInfo != NULL)
		{
			CvCity* pLoopCity;
			int iLoop;

			for (pLoopCity = pkPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iLoop))
			{
				if (pLoopCity != NULL && pLoopCity->IsEventChoiceActive(eEventChoice))
				{
					CityEventTypes eParentEvent = NO_EVENT_CITY;
					bool bNotChoice = false;
					for (int iLoop = 0; iLoop < GC.getNumCityEventInfos(); iLoop++)
					{
						CityEventTypes eEvent = (CityEventTypes)iLoop;
						if (eEvent != NO_EVENT)
						{
							if (pkEventChoiceInfo->isParentEvent(eEvent))
							{
								eParentEvent = eEvent;
								CvModCityEventInfo* pkCityEventInfo = GC.getCityEventInfo(eEvent);
								if (pkCityEventInfo != NULL)
								{
									if (pkCityEventInfo->getNumChoices() == 1)
									{
										bNotChoice = true;
									}
									break;
								}
							}
						}
					}

					if (bNotChoice)
						continue;

					int iDuration = pLoopCity->GetEventChoiceDuration(eEventChoice);
					if (!pkEventChoiceInfo->Expires())
					{
						iDuration = -1;
					}

					lua_createtable(L, 0, 0);
					const int t = lua_gettop(L);

					lua_pushinteger(L, eEventChoice);
					lua_setfield(L, t, "EventChoice");
					lua_pushinteger(L, iDuration);
					lua_setfield(L, t, "Duration");
					lua_pushinteger(L, eParentEvent);
					lua_setfield(L, t, "ParentEvent");
					lua_pushinteger(L, pLoopCity->getX());
					lua_setfield(L, t, "CityX");
					lua_pushinteger(L, pLoopCity->getY());
					lua_setfield(L, t, "CityY");

					lua_rawseti(L, -2, idx++);
				}
			}
		}
	}

	return 1;
}

int CvLuaPlayer::lGetRecentPlayerEventChoices(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_createtable(L, 0, 0);
	int idx = 1;

	for (int iI = 0; iI < GC.getNumEventChoiceInfos(); iI++)
	{
		EventChoiceTypes eEventChoice = (EventChoiceTypes)iI;
		if (eEventChoice == NO_EVENT_CHOICE)
			continue;

		CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eEventChoice);
		if (pkEventChoiceInfo != NULL)
		{
			if (pkPlayer->IsEventChoiceActive(eEventChoice))
			{
				bool bInstant = false;
				EventTypes eParentEvent = NO_EVENT;
				for (int iLoop = 0; iLoop < GC.getNumEventInfos(); iLoop++)
				{
					EventTypes eEvent = (EventTypes)iLoop;
					if (eEvent != NO_EVENT)
					{
						if (pkEventChoiceInfo->isParentEvent(eEvent))
						{							
							CvModEventInfo* pkEventInfo = GC.getEventInfo(eEvent);
							if (pkEventInfo != NULL)
							{
								if (pkEventInfo->getNumChoices() == 1)
								{
									bInstant = true;
								}
								if (pkEventInfo->getNumChoices() > 1)
								{
									eParentEvent = eEvent;
								}
								break;
							}
						}
					}
				}
				if (bInstant)
				{
					int iDuration = pkPlayer->GetEventChoiceDuration(eEventChoice);
					if (!pkEventChoiceInfo->Expires())
					{
						iDuration = -1;
					}

					lua_createtable(L, 0, 0);
					const int t = lua_gettop(L);

					lua_pushinteger(L, eEventChoice);
					lua_setfield(L, t, "EventChoice");
					lua_pushinteger(L, iDuration);
					lua_setfield(L, t, "Duration");
					if (bInstant)
					{
						lua_pushinteger(L, -1);
						lua_setfield(L, t, "ParentEvent");
					}
					else
					{
						lua_pushinteger(L, eParentEvent);
						lua_setfield(L, t, "ParentEvent");
					}					

					lua_rawseti(L, -2, idx++);
				}
			}
		}
	}
	return 1;
}
int CvLuaPlayer::lGetRecentCityEventChoices(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	lua_createtable(L, 0, 0);
	int idx = 1;

	for (int iI = 0; iI < GC.getNumCityEventChoiceInfos(); iI++)
	{
		CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)iI;
		if (eEventChoice == NO_EVENT_CHOICE_CITY)
			continue;

		CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
		if (pkEventChoiceInfo != NULL)
		{
			CvCity* pLoopCity;
			int iLoop;

			for (pLoopCity = pkPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pkPlayer->nextCity(&iLoop))
			{
				if (pLoopCity != NULL && pLoopCity->IsEventChoiceActive(eEventChoice))
				{
					bool bInstant = false;
					CityEventTypes eParentEvent = NO_EVENT_CITY;
					for (int iLoop = 0; iLoop < GC.getNumCityEventInfos(); iLoop++)
					{
						CityEventTypes eEvent = (CityEventTypes)iLoop;
						if (eEvent != NO_EVENT)
						{
							if (pkEventChoiceInfo->isParentEvent(eEvent))
							{
								CvModCityEventInfo* pkCityEventInfo = GC.getCityEventInfo(eEvent);
								if (pkCityEventInfo != NULL)
								{
									if (pkCityEventInfo->getNumChoices() == 1)
									{
										bInstant = true;
									}
									else if (pkCityEventInfo->getNumChoices() > 1)
									{
										eParentEvent = eEvent;
									}
									break;
								}
							}
						}
					}
					if (bInstant)
					{
						int iDuration = pLoopCity->GetEventChoiceDuration(eEventChoice);
						if (!pkEventChoiceInfo->Expires())
						{
							iDuration = -1;
						}
						bool bEspionage = pkEventChoiceInfo->IsEspionageEffect();

						lua_createtable(L, 0, 0);
						const int t = lua_gettop(L);

						lua_pushinteger(L, eEventChoice);
						lua_setfield(L, t, "EventChoice");
						lua_pushinteger(L, iDuration);
						lua_setfield(L, t, "Duration");
						lua_pushboolean(L, bEspionage);
						lua_setfield(L, t, "Espionage");
						if (bInstant)
						{
							lua_pushinteger(L, -1);
							lua_setfield(L, t, "ParentEvent");
						}
						else
						{
							lua_pushinteger(L, eParentEvent);
							lua_setfield(L, t, "ParentEvent");
						}
						lua_pushinteger(L, pLoopCity->getX());
						lua_setfield(L, t, "CityX");
						lua_pushinteger(L, pLoopCity->getY());
						lua_setfield(L, t, "CityY");

						lua_rawseti(L, -2, idx++);
					}
				}
			}
		}
	}

	return 1;
}
#endif

#if defined(MOD_BATTLE_ROYALE)
//------------------------------------------------------------------------------
//int getNumMilitarySeaUnits();
int CvLuaPlayer::lGetNumMilitarySeaUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumMilitarySeaUnits);
}
//------------------------------------------------------------------------------
//int getNumMilitaryAirUnits();
int CvLuaPlayer::lGetNumMilitaryAirUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumMilitaryAirUnits);
}
//------------------------------------------------------------------------------
//int getNumMilitaryLandUnits();
int CvLuaPlayer::lGetNumMilitaryLandUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumMilitaryLandUnits);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//int GetMilitarySeaMight();
int CvLuaPlayer::lGetMilitarySeaMight(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetMilitarySeaMight);
}
//------------------------------------------------------------------------------
//int GetMilitaryAirMight();
int CvLuaPlayer::lGetMilitaryAirMight(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetMilitaryAirMight);
}
//------------------------------------------------------------------------------
//int GetMilitaryLandMight();
int CvLuaPlayer::lGetMilitaryLandMight(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetMilitaryLandMight);
}
#endif
