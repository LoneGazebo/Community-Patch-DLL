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
#include <CvGameCoreDLLPCH.h>
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

	Method(InitUnit);
	Method(DisbandUnit);
	Method(AddFreeUnit);

	Method(ChooseTech);

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

	Method(IsTurnActive);
	Method(IsSimultaneousTurns);

	Method(FindNewCapital);
	Method(CanRaze);
	Method(Raze);
	Method(Disband);
	Method(CanReceiveGoody);
	Method(ReceiveGoody);
	Method(DoGoody);
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
	Method(GetAveragePopulation);
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
	Method(GetGoldPerTurnFromDiplomacy);
	Method(GetRouteGoldTimes100);
	Method(GetCityConnectionGold);
	Method(GetCityConnectionGoldTimes100);
	Method(GetGoldPerTurnFromReligion);

	// Culture

	Method(GetTotalJONSCulturePerTurn);

	Method(GetJONSCulturePerTurnFromCities);

	Method(GetJONSCulturePerTurnFromExcessHappiness);

	Method(GetCultureWonderMultiplier);

	Method(GetJONSCulturePerTurnForFree);
	Method(ChangeJONSCulturePerTurnForFree);

	Method(GetJONSCulturePerTurnFromMinorCivs);
	Method(ChangeJONSCulturePerTurnFromMinorCivs);
	Method(GetCulturePerTurnFromMinorCivs);
	Method(GetCulturePerTurnFromMinor);

	Method(GetCulturePerTurnFromReligion);

	Method(GetCultureCityModifier);

	Method(GetJONSCulture);
	Method(SetJONSCulture);
	Method(ChangeJONSCulture);

	Method(GetJONSCultureEverGenerated);

	// Faith

	Method(GetFaith);
	Method(SetFaith);
	Method(ChangeFaith);
	Method(GetTotalFaithPerTurn);
	Method(GetFaithPerTurnFromCities);
	Method(GetFaithPerTurnFromMinorCivs);
	Method(GetFaithPerTurnFromReligion);
	Method(HasCreatedPantheon);
	Method(GetBeliefInPantheon);
	Method(HasCreatedReligion);
	Method(CanCreatePantheon);
	Method(GetReligionCreatedByPlayer);
	Method(GetFoundedReligionEnemyCityCombatMod);
	Method(GetFoundedReligionFriendlyCityCombatMod);
	Method(GetMinimumFaithNextGreatProphet);
	Method(HasReligionInMostCities);
	Method(DoesUnitPassFaithPurchaseCheck);

	// Happiness

	Method(GetHappiness);
	Method(SetHappiness);

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
	Method(GetHappinessFromResourceVariety);
	Method(GetExtraHappinessPerLuxury);
	Method(GetHappinessFromReligion);
	Method(GetHappinessFromNaturalWonders);

	Method(GetUnhappiness);
	Method(GetUnhappinessForecast);

	Method(GetUnhappinessFromCityForUI);

	Method(GetUnhappinessFromCityCount);
	Method(GetUnhappinessFromCapturedCityCount);
	Method(GetUnhappinessFromCityPopulation);
	Method(GetUnhappinessFromCitySpecialists);
	Method(GetUnhappinessFromOccupiedCities);
	Method(GetUnhappinessFromPuppetCityPopulation);
	Method(GetUnhappinessFromUnits);
	Method(ChangeUnhappinessFromUnits);

	Method(GetUnhappinessMod);
	Method(GetCityCountUnhappinessMod);
	Method(GetOccupiedPopulationUnhappinessMod);
	Method(GetCapitalUnhappinessMod);
	Method(GetTraitCityUnhappinessMod);
	Method(GetTraitPopUnhappinessMod);
	Method(IsHalfSpecialistUnhappiness);

	Method(GetHappinessPerGarrisonedUnit);
	Method(SetHappinessPerGarrisonedUnit);
	Method(ChangeHappinessPerGarrisonedUnit);

	Method(GetHappinessFromTradeRoutes);
	Method(GetHappinessPerTradeRoute);
	Method(SetHappinessPerTradeRoute);
	Method(ChangeHappinessPerTradeRoute);

	Method(GetTradeRouteGoldModifier);

	Method(GetHappinessFromMinorCivs);
	Method(GetHappinessFromMinor);

	// END Happiness

	Method(GetBarbarianCombatBonus);
	Method(SetBarbarianCombatBonus);
	Method(ChangeBarbarianCombatBonus);
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

	Method(GetPolicyCatchSpiesModifier);

	Method(GetNumPolicyBranchesFinished);
	Method(IsPolicyBranchFinished);

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
	Method(GetExpInBorderModifier);

	Method(GetLevelExperienceModifier);

	Method(GetCultureBombTimer);
	Method(GetConversionTimer);

	Method(GetCapitalCity);
	Method(IsHasLostCapital);
	Method(GetCitiesLost);

	Method(GetPower);
	Method(GetMilitaryMight);
	Method(GetPopScore);
	Method(GetLandScore);
	Method(GetWondersScore);
	Method(GetTechScore);
	Method(GetTotalTimePlayed);

	Method(GetScore);
	Method(GetScoreFromCities);
	Method(GetScoreFromPopulation);
	Method(GetScoreFromLand);
	Method(GetScoreFromWonders);
	Method(GetScoreFromTechs);
	Method(ChangeScoreFromTechs);
	Method(GetScoreFromFutureTech);
	Method(ChangeScoreFromFutureTech);

	Method(IsGoldenAgeCultureBonusDisabled);

	Method(IsMinorCiv);
	Method(GetMinorCivType);
	Method(GetMinorCivTrait);
	Method(GetPersonality);
	Method(IsMinorCivHasUniqueUnit);
	Method(GetMinorCivUniqueUnit);
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
	Method(GetMinorCivNumActiveQuestsForPlayer);
	Method(GetQuestData1);
	Method(GetQuestData2);
	Method(GetQuestTurnsRemaining);
	Method(IsMinorCivContestLeader);
	Method(GetMinorCivContestValueForLeader);
	Method(GetMinorCivContestValueForPlayer);
	Method(IsMinorCivUnitSpawningDisabled);
	Method(IsMinorCivRouteEstablishedWithMajor);
	Method(IsMinorWarQuestWithMajorActive);
	Method(GetMinorWarQuestWithMajorRemainingCount);
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
	Method(CanMajorBullyGold);
	Method(CanMajorBullyUnit);
	Method(CanMajorBuyout);
	Method(GetBuyoutCost);
	Method(CanMajorGiftTileImprovement);
	Method(CanMajorGiftTileImprovementAtPlot);
	Method(GetGiftTileImprovementCost);
	Method(AddMinorCivQuestIfAble);

	Method(IsAlive);
	Method(IsEverAlive);
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

	Method(Units);
	Method(GetNumUnits);
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
	Method(IsDoFMessageTooSoon);
	Method(IsDoF);
	Method(IsPlayerDoFwithAnyFriend);
	Method(IsPlayerDoFwithAnyEnemy);
	Method(IsPlayerDenouncedFriend);
	Method(IsPlayerDenouncedEnemy);
	Method(IsUntrustworthyFriend);
	Method(GetNumFriendsDenouncedBy);
	Method(IsFriendDenouncedUs);
	Method(GetWeDenouncedFriendCount);
	Method(IsFriendDeclaredWarOnUs);
	Method(GetWeDeclaredWarOnFriendCount);
	//Method(IsWorkingAgainstPlayerAccepted);
	Method(GetCoopWarAcceptedState);
	Method(GetNumWarsFought);

	Method(GetLandDisputeLevel);
	Method(GetVictoryDisputeLevel);
	Method(GetWonderDisputeLevel);
	Method(GetMinorCivDisputeLevel);
	Method(GetWarmongerThreat);
	Method(IsPlayerNoSettleRequestEverAsked);
	Method(IsPlayerStopSpyingRequestEverAsked);
	Method(IsDemandEverMade);
	Method(GetNumCiviliansReturnedToMe);
	Method(GetNumTimesCultureBombed);
	Method(GetNegativeReligiousConversionPoints);
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
	Method(IsPlayerBrokenCoopWarPromise);
	Method(GetOtherPlayerNumProtectedMinorsKilled);
	Method(GetOtherPlayerNumProtectedMinorsAttacked);
	Method(GetTurnsSincePlayerBulliedProtectedMinor);
	Method(IsHasPlayerBulliedProtectedMinor);
	Method(IsDenouncedPlayer);
	Method(IsDenouncingPlayer);
	Method(IsPlayerRecklessExpander);
	Method(GetRecentTradeValue);
	Method(GetCommonFoeValue);
	Method(GetRecentAssistValue);
	Method(IsGaveAssistanceTo);
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

	Method(GetNumNotifications);
	Method(GetNotificationStr);
	Method(GetNotificationSummaryStr);
	Method(GetNotificationIndex);
	Method(GetNotificationTurn);
	Method(GetNotificationDismissed);
	Method(AddNotification);

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

	Method(GetTraitGoldenAgeCombatModifier);
	Method(GetTraitCityStateCombatModifier);
	Method(GetTraitGreatGeneralExtraBonus);
	Method(GetTraitGreatScientistRateModifier);
	Method(IsTraitBonusReligiousBelief);
	Method(GetHappinessFromLuxury);
	Method(IsAbleToAnnexCityStates);
	Method(IsUsingMayaCalendar);
	Method(GetMayaCalendarString);
	Method(GetMayaCalendarLongString);

	Method(GetExtraBuildingHappinessFromPolicies);

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

	Method(GetPlayerBuildingClassYieldChange);
	Method(GetPlayerBuildingClassHappiness);

	Method(WasResurrectedBy);
	Method(WasResurrectedThisTurnBy);

	Method(GetOpinionTable);
	Method(GetDealValue);
	Method(GetDealMyValue);
	Method(GetDealTheyreValue);

	Method(GetEspionageCityStatus);
	Method(GetNumSpies);
	Method(GetNumUnassignedSpies);
	Method(GetEspionageSpies);
	Method(HasSpyEstablishedSurveillance);
	Method(CanSpyStageCoup);
	Method(GetAvailableSpyRelocationCities);
	Method(GetNumTechsToSteal);
	Method(GetIntrigueMessages);
	Method(HasRecentIntrigueAbout);
	Method(GetRecentIntrigueInfo);
	Method(GetCoupChanceOfSuccess);

	Method(IsConnected);
	Method(IsObserver);

	Method(HasTurnTimerExpired);

	Method(HasUnitOfClassType);
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

	CvCity* pkCity = pkPlayer->initCity(x, y, bBumpUnits);
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
//CyUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION);
int CvLuaPlayer::lInitUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const int x = lua_tointeger(L, 3);
	const int y = lua_tointeger(L, 4);
	const UnitAITypes eUnitAI = (UnitAITypes)luaL_optint(L, 5, NO_UNITAI);
	const DirectionTypes eFacingDirection = (DirectionTypes)luaL_optint(L, 6, NO_DIRECTION);

	CvUnit* pkUnit = pkPlayer->initUnit(eUnit, x, y, eUnitAI, eFacingDirection);
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}
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
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);

	const bool bResult = pkPlayer->IsCapitalConnectedToCity(pkCity);
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
	return BasicLuaMethod(L, &CvPlayerAI::canReceiveGoody);
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
//bool canFound(int iX, int iY);
int CvLuaPlayer::lCanFound(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::canFound);
}
//------------------------------------------------------------------------------
//void found(int iX, int iY);
int CvLuaPlayer::lFound(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::found);
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

	const bool bResult = pkPlayer->canTrain(eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUniqueUnitStatus);
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
	const UnitHandle MyUnitHandle;
	MyUnitHandle = pkPlayer->GetFirstReadyUnit();

	CvLuaUnit::Push(L, MyUnitHandle);
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
int CvLuaPlayer::lGetAveragePopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getAveragePopulation);
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
//int GetGoldPerTurnFromDiplomacy();
int CvLuaPlayer::lGetGoldPerTurnFromDiplomacy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetGoldPerTurnFromDiplomacy());
	return 1;
}
//------------------------------------------------------------------------------
//int GetRouteGoldTimes100(CvCity* pCity);
int CvLuaPlayer::lGetRouteGoldTimes100(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2);
	lua_pushinteger(L, pkPlayer->GetTreasury()->GetRouteGoldTimes100(pkCity));
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
// DEPRECATED, use lGetCulturePerTurnFromMinorCivs instead
//int getJONSCulturePerTurnFromMinorCivs();
int CvLuaPlayer::lGetJONSCulturePerTurnFromMinorCivs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetJONSCulturePerTurnFromMinorCivs);
}
//------------------------------------------------------------------------------
// DEPRECATED, does nothing
//void changeJONSCulturePerTurnFromMinorCivs(int iChange);
int CvLuaPlayer::lChangeJONSCulturePerTurnFromMinorCivs(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::ChangeJONSCulturePerTurnFromMinorCivs);
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

	const bool bResult = pkPlayer->GetReligions()->HasCreatedReligion();
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
//bool GetFoundedReligionEnemyCityCombatMod();
int CvLuaPlayer::lGetFoundedReligionEnemyCityCombatMod(lua_State* L)
{
	int iRtnValue = 0;

	CvPlayerAI* pkPlayer = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	if(pkPlot)
	{
		CvCity* pPlotCity = pkPlot->getWorkingCity();
		if(pPlotCity)
		{
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
			ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(pkPlayer->GetID());
			if(eFoundedReligion != NO_RELIGION && eReligion == eFoundedReligion)
			{
				const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, pkPlayer->GetID());
				if(pReligion)
				{
					iRtnValue = pReligion->m_Beliefs.GetCombatModifierEnemyCities();
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
		CvCity* pPlotCity = pkPlot->getWorkingCity();
		if(pPlotCity)
		{
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
			ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(pkPlayer->GetID());
			if(eFoundedReligion != NO_RELIGION && eReligion == eFoundedReligion)
			{
				const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, pkPlayer->GetID());
				if(pReligion)
				{
					iRtnValue = pReligion->m_Beliefs.GetCombatModifierFriendlyCities();
				}
			}
		}

	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}
//------------------------------------------------------------------------------
// int GetMinimumFaithNextGreatProphet() const
int CvLuaPlayer::lGetMinimumFaithNextGreatProphet(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);

	int iFaith = pkPlayer->GetReligions()->GetCostNextProphet(true /*bIncludeBeliefDiscounts*/, true /*bAdjustForSpeedDifficulty*/);
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
	return BasicLuaMethod(L, &CvPlayerAI::GetHappiness);
}
//------------------------------------------------------------------------------
//void SetHappiness(int iNewValue);
int CvLuaPlayer::lSetHappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::SetHappiness);
}

//------------------------------------------------------------------------------
//int GetExcessHappiness();
int CvLuaPlayer::lGetExcessHappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetExcessHappiness);
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

	const int iUnhappiness = pkPlayer->GetUnhappiness(pkAssumeCityAnnexed, pkAssumeCityPuppeted);
	lua_pushinteger(L, iUnhappiness);
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
	return BasicLuaMethod(L, &CvPlayerAI::GetUnhappinessFromCapturedCityCount);
}

//------------------------------------------------------------------------------
//int GetUnhappinessFromCityPopulation() const;
int CvLuaPlayer::lGetUnhappinessFromCityPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetUnhappinessFromCityPopulation);
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
	return BasicLuaMethod(L, &CvPlayerAI::GetUnhappinessFromOccupiedCities);
}

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
int CvLuaPlayer::lGetTradeRouteGoldModifier(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int iResult = pkPlayer->GetTreasury()->GetTradeRouteGoldModifier();
	lua_pushinteger(L, iResult);
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

	pkPlayer->GetPlayerPolicies()->SetPolicyBranchUnlocked(eBranchType, bNewValue);
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

	const int iResult = pkPlayer->GetPlayerPolicies()->GetNumPoliciesOwned();
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
int CvLuaPlayer::lGetAttackBonusTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetAttackBonusTurns);
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

	pkPlayer->createGreatGeneral(eGreatPersonUnit, x, y);
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
	return BasicLuaMethod(L, &CvPlayerAI::GetNumUnitsSuppliedByHandicap);
}
//------------------------------------------------------------------------------
//int GetNumUnitsSuppliedByCities();
int CvLuaPlayer::lGetNumUnitsSuppliedByCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumUnitsSuppliedByCities);
}
//------------------------------------------------------------------------------
//int GetNumUnitsSuppliedByPopulation();
int CvLuaPlayer::lGetNumUnitsSuppliedByPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::GetNumUnitsSuppliedByPopulation);
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
//int getPopScore();
int CvLuaPlayer::lGetPopScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getPopScore);
}
//------------------------------------------------------------------------------
//int getLandScore();
int CvLuaPlayer::lGetLandScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getLandScore);
}
//------------------------------------------------------------------------------
//int getWondersScore();
int CvLuaPlayer::lGetWondersScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getWondersScore);
}
//------------------------------------------------------------------------------
//int getTechScore();
int CvLuaPlayer::lGetTechScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getTechScore);
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
//void ChangeScoreFromTechs(int iChange);
int CvLuaPlayer::lChangeScoreFromTechs(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	int iChange = lua_tointeger(L, 2);

	pkPlayer->ChangeScoreFromTechs(iChange);
	return 1;
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

int CvLuaPlayer::lIsGoldenAgeCultureBonusDisabled(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::IsGoldenAgeCultureBonusDisabled);
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
int CvLuaPlayer::lGetMinorCivNumActiveQuestsForPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iResult = pkPlayer->GetMinorCivAI()->GetNumActiveQuestsForPlayer(ePlayer);
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
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->IsWarQuestWithMajorActive(ePlayer);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMinorWarQuestWithMajorRemainingCount(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const PlayerTypes eFriendlyPlayer = (PlayerTypes) lua_tointeger(L, 2);
	const PlayerTypes eEnemyPlayer = (PlayerTypes) lua_tointeger(L, 3);
	const int iResult = pkPlayer->GetMinorCivAI()->GetNumEnemyUnitsLeftToKillByMajor(eFriendlyPlayer, eEnemyPlayer);
	lua_pushinteger(L, iResult);
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
//------------------------------------------------------------------------------
//bool CanMajorBullyGold(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorBullyGold(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorBullyGold(eMajor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanMajorBullyUnit(PlayerTypes eMajor);
int CvLuaPlayer::lCanMajorBullyUnit(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eMajor = (PlayerTypes) lua_tointeger(L, 2);

	const bool bResult = pkPlayer->GetMinorCivAI()->CanMajorBullyUnit(eMajor);
	lua_pushboolean(L, bResult);
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
	return BasicLuaMethod(L, &CvPlayerAI::getResourceImport);
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
	return BasicLuaMethod(L, &CvPlayerAI::getCombatExperience);
}
//------------------------------------------------------------------------------
//void changeCombatExperience(int iChange);
int CvLuaPlayer::lChangeCombatExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeCombatExperience);
}
//------------------------------------------------------------------------------
//void setCombatExperience(int iExperience);
int CvLuaPlayer::lSetCombatExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setCombatExperience);
}
//------------------------------------------------------------------------------
//int getLifetimeCombatExperience();
int CvLuaPlayer::lGetLifetimeCombatExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getLifetimeCombatExperience);
}
//------------------------------------------------------------------------------
//int getNavalCombatExperience();
int CvLuaPlayer::lGetNavalCombatExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNavalCombatExperience);
}
//------------------------------------------------------------------------------
//void changeNavalCombatExperience(int iChange);
int CvLuaPlayer::lChangeNavalCombatExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::changeNavalCombatExperience);
}
//------------------------------------------------------------------------------
//void setCombatExperience(int iExperience);
int CvLuaPlayer::lSetNavalCombatExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::setNavalCombatExperience);
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
//CyCity* getCity(int iID);
int CvLuaPlayer::lGetCityByID(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	const int id = lua_tointeger(L, 2);

	CvCity* pkCity = pkPlayer->getCity(id);
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

	CvLuaUnit::Push(L, pkPlayer->getUnit(id));
	return 1;
}
//------------------------------------------------------------------------------
//int getNumUnits();
int CvLuaPlayer::lGetNumUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::getNumUnits);
}
//------------------------------------------------------------------------------
//void AI_updateFoundValues(bool bStartingLoc);
int CvLuaPlayer::lAI_updateFoundValues(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::AI_updateFoundValues);
}
//------------------------------------------------------------------------------
//int AI_foundValue(int iX, int iY, int iMinUnitRange/* = -1*/, bool bStartingLoc/* = false*/);
int CvLuaPlayer::lAI_foundValue(lua_State* L)
{
	return BasicLuaMethod(L, &CvPlayerAI::AI_foundValue);
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
	unsigned int uiDataSet = pkPlayer->getReplayDataSetIndex("REPLAYDATASET_AGRICULTURE");
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

	lua_pushinteger(L, pkPlayer->GetPlotDanger(*pkPlot));
	return 1;
}
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

	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetApproachTowardsUsGuess(ePlayer));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetMajorCivApproach(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	lua_pushinteger(L, pkPlayer->GetDiplomacyAI()->GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false));
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
int CvLuaPlayer::lIsPlayerDoFwithAnyFriend(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsPlayerDoFwithAnyFriend(eWithPlayer);

	lua_pushboolean(L, bTooSoon);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerDoFwithAnyEnemy(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bTooSoon = pkPlayer->GetDiplomacyAI()->IsPlayerDoFwithAnyEnemy(eWithPlayer);

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

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsUntrustworthyFriend();

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
////------------------------------------------------------------------------------
////void IsWorkingAgainstPlayerAccepted(PlayerTypes eWithPlayer, eAgainstPlayer);
//int CvLuaPlayer::lIsWorkingAgainstPlayerAccepted(lua_State* L)
//{
//	CvPlayerAI* pkPlayer = GetInstance(L);
//	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);
//	PlayerTypes eAgainstPlayer = (PlayerTypes) lua_tointeger(L, 3);
//
//	const bool bAccepted = pkPlayer->GetDiplomacyAI()->IsWorkingAgainstPlayerAccepted(eWithPlayer, eAgainstPlayer);
//
//	lua_pushboolean(L, bAccepted);
//	return 1;
//}
//------------------------------------------------------------------------------
//void GetCoopWarAcceptedState(PlayerTypes eWithPlayer, eAgainstPlayer);
int CvLuaPlayer::lGetCoopWarAcceptedState(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eWithPlayer = (PlayerTypes) lua_tointeger(L, 2);
	PlayerTypes eAgainstPlayer = (PlayerTypes) lua_tointeger(L, 3);

	const int iState = pkPlayer->GetDiplomacyAI()->GetCoopWarAcceptedState(eWithPlayer, eAgainstPlayer);

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
int CvLuaPlayer::lIsPlayerNoSettleRequestEverAsked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerNoSettleRequestEverAsked(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsPlayerStopSpyingRequestEverAsked(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);
	const bool bValue = pkPlayer->GetDiplomacyAI()->IsPlayerStopSpyingRequestEverAsked(eOtherPlayer);

	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lIsDemandEverMade(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsDemandEverMade(eOtherPlayer);

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

	const int iValue = pkPlayer->GetDiplomacyAI()->GetTurnsSincePlayerBulliedProtectedMinor(eOtherPlayer);

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
int CvLuaPlayer::lIsDenouncingPlayer(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsDenouncingPlayer(eOtherPlayer);

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
int CvLuaPlayer::lIsGaveAssistanceTo(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	PlayerTypes eOtherPlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsGaveAssistanceTo(eOtherPlayer);

	lua_pushboolean(L, bValue);
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

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsCapitalCapturedBy(eOtherPlayer);

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

	const bool bValue = pkPlayer->GetDiplomacyAI()->IsAngryAboutSidedWithTheirProtectedMinor(eOtherPlayer);

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

	pkPlayer->GetDiplomacyAI()->SetDoFCounter(eOtherPlayer, 0);
	pkPlayer->GetDiplomacyAI()->SetDoFAccepted(eOtherPlayer, true);
	GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->SetDoFCounter(pkPlayer->GetID(), 0);
	GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->SetDoFAccepted(pkPlayer->GetID(), true);

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
	{
		return 0;
	}

	const size_t cuiMaxDirectives = 3;
	const size_t cuiDirectiveSize = 4;
	BuilderDirective aDirective[ cuiDirectiveSize ];
	bool bUseDirective[cuiDirectiveSize];
	CvPlot* pDirectivePlots[cuiDirectiveSize] = {0};

	pkPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pWorkerUnit, aDirective, cuiDirectiveSize, true);

	for(uint ui = 0; ui < cuiDirectiveSize; ui++)
	{
		bUseDirective[ui] = false;

		if(aDirective[ui].m_eDirective != BuilderDirective::NUM_DIRECTIVES)
		{
			CvPlot* pPlot = GC.getMap().plot(aDirective[ui].m_sX, aDirective[ui].m_sY);
			if(pPlot != NULL)
			{
				// Don't recommend plots that are already improved
				if(aDirective[ui].m_eDirective != BuilderDirective::BUILD_IMPROVEMENT || pPlot->getImprovementType() == NO_IMPROVEMENT)
				{
					pDirectivePlots[ui] = pPlot;
					bUseDirective[ui] = true;
				}
			}
		}
	}

	lua_createtable(L, cuiMaxDirectives, 0);
	int iPositionIndex = lua_gettop(L);
	int i = 1;

	for(uint ui = 0; ui < cuiDirectiveSize && i < cuiMaxDirectives; ui++)
	{
		if(bUseDirective[ui] == true)
		{
			lua_createtable(L, 0, 2);
			CvLuaPlot::Push(L, pDirectivePlots[ui]);
			lua_setfield(L, -2, "plot");
			lua_pushinteger(L, aDirective[ui].m_eBuild);
			lua_setfield(L, -2, "buildType");

			lua_rawseti(L, iPositionIndex, i);
			i++;
		}
	}

	return 1;
}

typedef CvWeightedVector<CvPlot*, 800, true> WeightedPlotVector;

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
			if(!pkPlayer->canFound(iPlotX, iPlotY))
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

				//iValue = pPlot->getFoundValue(pkPlayer->GetID());
				iValue = pkPlayer->AI_foundValue(iPlotX, iPlotY, -1, false);

				iDistanceDropoff = (iDistanceDropoffMod * iSettlerDistance) / iEvalDistance;
				iValue = iValue * (100 - iDistanceDropoff) / 100;
				iDanger = pkPlayer->GetPlotDanger(*pPlot);
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
			bool bCanFindPath = pFoundingUnit->GeneratePath(pPlot, MOVE_TERRITORY_NO_UNEXPLORED, true, &iPathTurns);
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

	const CvPlotsVector& aiPlots = pkPlayer->GetPlots();
	// go through all the plots the player has under their control
	for(uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
	{
		// when we encounter the first plot that is invalid, the rest of the list will be invalid
		if(aiPlots[uiPlotIndex] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);

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
				if(pkImprovement->IsImprovementResourceTrade(eResource))
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

			if(!pkImprovementInfo->IsImprovementResourceTrade(eResource))
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

	const CvPlotsVector& aiPlots = pkPlayer->GetPlots();

	bool bResult = false;

	// go through all the plots the player has under their control
	for(uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
	{
		// when we encounter the first plot that is invalid, the rest of the list will be invalid
		if(aiPlots[uiPlotIndex] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
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

	const CvPlotsVector& aiPlots = pkPlayer->GetPlots();

	// go through all the plots the player has under their control
	for(uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
	{
		// when we encounter the first plot that is invalid, the rest of the list will be invalid
		if(aiPlots[uiPlotIndex] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
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
		lua_pushboolean(L, pkPlayer->GetPlayerTraits()->IsAbleToAnnexCityStates());
	}
	return 1;
}
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
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetBuildingClassType();

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


//------------------------------------------------------------------------------
int CvLuaPlayer::lGetNextCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pCurrentCity = CvLuaCity::GetInstance(L, 2);
	CvCity* pLoopCity = NULL;
	CvCity* pNextCity = NULL;
	int	iLoop = pCurrentCity->getIndex();
	iLoop++;

	do
	{
		pLoopCity = pkPlayer->nextCity(&iLoop, false);

		if(pLoopCity == NULL)
		{
			pLoopCity = pkPlayer->firstCity(&iLoop, false);
		}

		if((pLoopCity != NULL) && (pLoopCity != pCurrentCity) && !pLoopCity->IsPuppet())  // we don't want the player to be able to cycle to puppeted cities - it kind of defeats teh whole purpose
		{
			pNextCity = pLoopCity;
		}

	}
	while((pLoopCity != pCurrentCity) && !pNextCity);


	CvLuaCity::Push(L, pNextCity);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPrevCity(lua_State* L)
{
	CvPlayerAI* pkPlayer = GetInstance(L);
	CvCity* pCurrentCity = CvLuaCity::GetInstance(L, 2);
	CvCity* pLoopCity = NULL;
	CvCity* pPrevCity = NULL;
	int	iLoop = pCurrentCity->getIndex();
	iLoop--;

	do
	{
		pLoopCity = pkPlayer->nextCity(&iLoop, true);

		if(pLoopCity == NULL)
		{
			pLoopCity = pkPlayer->firstCity(&iLoop, true);
		}

		if((pLoopCity != NULL) && (pLoopCity != pCurrentCity) && !pLoopCity->IsPuppet())  // we don't want the player to be able to cycle to puppeted cities - it kind of defeats teh whole purpose
		{
			pPrevCity = pLoopCity;
		}

	}
	while((pLoopCity != pCurrentCity) && !pPrevCity);

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
	CvTemporaryZone zone;
	zone.SetX(iX);
	zone.SetY(iY);
	zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
	zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
	pkPlayer->GetTacticalAI()->AddTemporaryZone(zone);

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

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetPlayerBuildingClassYieldChange(lua_State* L)
{
	const BuildingClassTypes eBuildingClass = (BuildingClassTypes)luaL_checkint(L, 2);
	const YieldTypes eYieldType = (YieldTypes)luaL_checkint(L, 3);

	CvPlayer* pkPlayer = GetInstance(L);
	if(pkPlayer)
	{
		int iChange = pkPlayer->GetBuildingClassYieldChange(eBuildingClass, eYieldType);
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
	if(pkPlayer)
	{
		int iChange = 0;

		for(int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingClassTypes eParentBuildingClass = (BuildingClassTypes) iI;

			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eParentBuildingClass);
			if(!pkBuildingClassInfo)
			{
				continue;
			}
			
			BuildingTypes eParentBuilding = (BuildingTypes)pkPlayer->getCivilizationInfo().getCivilizationBuildings(eParentBuildingClass);
			if (eParentBuilding != NO_BUILDING && pkPlayer->countNumBuildings(eParentBuilding) > 0)
			{
				CvBuildingEntry* pkParentBuilding = GC.getBuildingInfo(eParentBuilding);
				if (pkParentBuilding)
				{
					iChange += pkParentBuilding->GetBuildingClassHappiness(eOtherBuildingClass);
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
struct Opinion
{
	Localization::String m_str;
	int m_iValue;
};

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
	PlayerTypes eWithPlayer = (PlayerTypes)lua_tointeger(L, 2);
	CvDiplomacyAI* pDiploAI = pkPlayer->GetDiplomacyAI();


	std::vector<Opinion> aOpinions;
	int iValue;

	int iVisibleApproach = GET_PLAYER(eWithPlayer).GetDiplomacyAI()->GetApproachTowardsUsGuess(pkPlayer->GetID());
	if (GET_TEAM(pkPlayer->getTeam()).isAtWar(GET_PLAYER(eWithPlayer).getTeam()))
	{
		Opinion kOpinion;
		kOpinion.m_iValue = 99999;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_AT_WAR");
		aOpinions.push_back(kOpinion);
	}
	else
	{
		if (GET_PLAYER(eWithPlayer).GetDiplomacyAI()->GetNumWarsFought(pkPlayer->GetID()) > 0)
		{
			if (iVisibleApproach == MAJOR_CIV_APPROACH_FRIENDLY || iVisibleApproach == MAJOR_CIV_APPROACH_NEUTRAL)
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 0;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PAST_WAR_NEUTRAL");
				aOpinions.push_back(kOpinion);
			}
			else
			{
				Opinion kOpinion;
				kOpinion.m_iValue = 0;
				kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PAST_WAR_BAD");
				aOpinions.push_back(kOpinion);
			}
		}
	}

	if (iVisibleApproach != MAJOR_CIV_APPROACH_FRIENDLY) 
	{
		// land dispute
		iValue = pDiploAI->GetLandDisputeLevelScore(eWithPlayer);
		if (iValue < 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_LAND_DISPUTE");
			aOpinions.push_back(kOpinion);
		}
		else if (iValue > 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_LAND_DISPUTE");
			aOpinions.push_back(kOpinion);
		}

		// wonder dispute
		iValue = pDiploAI->GetWonderDisputeLevelScore(eWithPlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WONDER_DISPUTE");
			aOpinions.push_back(kOpinion);
		}

		// minor civ dispute
		iValue = pDiploAI->GetMinorCivDisputeLevelScore(eWithPlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MINOR_CIV_DISPUTE");
			aOpinions.push_back(kOpinion);
		}

		// warmonger dispute
		iValue = pDiploAI->GetWarmongerThreatScore(eWithPlayer);
		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_WARMONGER_THREAT");
			aOpinions.push_back(kOpinion);
		}
	}

	if (iVisibleApproach == MAJOR_CIV_APPROACH_AFRAID)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = 0;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_AFRAID");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetCiviliansReturnedToMeScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CIVILIANS_RETURNED");
		aOpinions.push_back(kOpinion);
	}
	
	iValue = pDiploAI->GetResurrectedScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RESURRECTED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetLiberatedCitiesScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITIES_LIBERATED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetEmbassyScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HAS_EMBASSY");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetForgaveForSpyingScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_FORGAVE_FOR_SPYING");
		aOpinions.push_back(kOpinion);
	}
	
	iValue = pDiploAI->GetNoSetterRequestScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_SETTLE_ASKED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetStopSpyingRequestScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_STOP_SPYING_ASKED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDemandEverMadeScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_DEMAND");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetTimesCultureBombedScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CULTURE_BOMB");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetReligiousConversionPointsScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RELIGIOUS_CONVERSIONS");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetHasAdoptedHisReligionScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_MY_RELIGION");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetHasAdoptedMyReligionScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_ADOPTING_HIS_RELIGION");
		aOpinions.push_back(kOpinion);
	}
	
	iValue = pDiploAI->GetSameLatePoliciesScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SAME_LATE_POLICY_TREES");
		PolicyBranchTypes eBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
		kOpinion.m_str << GC.getPolicyBranchInfo(eBranch)->GetDescription();
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDifferentLatePoliciesScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DIFFERENT_LATE_POLICY_TREES");
		PolicyBranchTypes eTheirBranch = pkPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
		PolicyBranchTypes eMyBranch = GET_PLAYER(eWithPlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
		kOpinion.m_str << GC.getPolicyBranchInfo(eMyBranch)->GetDescription();
		kOpinion.m_str << GC.getPolicyBranchInfo(eTheirBranch)->GetDescription();
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetTimesRobbedScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAUGHT_STEALING");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetTimesIntrigueSharedScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SHARED_INTRIGUE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenMilitaryPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MILITARY_PROMISE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenMilitaryPromiseWithAnybodyScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MILITARY_PROMISE_BROKEN_WITH_OTHERS");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetIgnoredMilitaryPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MILITARY_PROMISE_IGNORED");
		aOpinions.push_back(kOpinion);
	}
	
	iValue = pDiploAI->GetBrokenExpansionPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_EXPANSION_PROMISE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetIgnoredExpansionPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_EXPANSION_PROMISE_IGNORED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenBorderPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BORDER_PROMISE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetIgnoredBorderPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BORDER_PROMISE_IGNORED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenAttackCityStatePromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITY_STATE_PROMISE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenAttackCityStatePromiseWithAnybodyScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITY_STATE_PROMISE_BROKEN_WITH_OTHERS");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetIgnoredAttackCityStatePromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CITY_STATE_PROMISE_IGNORED");
		aOpinions.push_back(kOpinion);
	}
	
	iValue = pDiploAI->GetBrokenBullyCityStatePromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_BROKEN");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetIgnoredBullyCityStatePromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_BULLY_CITY_STATE_PROMISE_IGNORED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenNoConvertPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_CONVERT_PROMISE_BROKEN");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetIgnoredNoConvertPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NO_CONVERT_PROMISE_IGNORED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenSpyPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SPY_PROMISE_BROKEN");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetIgnoredSpyPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SPY_PROMISE_IGNORED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetBrokenCoopWarPromiseScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_COOP_WAR_PROMISE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetAngryAboutProtectedMinorKilledScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PROTECTED_MINORS_KILLED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetAngryAboutProtectedMinorAttackedScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PROTECTED_MINORS_ATTACKED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetAngryAboutProtectedMinorBulliedScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PROTECTED_MINORS_BULLIED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetAngryAboutSidedWithProtectedMinorScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_SIDED_WITH_MINOR");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDOFAcceptedScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DOF");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDOFWithAnyFriendScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_DOF");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDOFWithAnyEnemyScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DOF_WITH_ENEMY");
		aOpinions.push_back(kOpinion);
	}

	// TRAITOR BEGIN
	{
		iValue = 0;
		Localization::String str;
		int iTempValue;

		iTempValue = pDiploAI->GetFriendDenouncementScore(eWithPlayer);
		if (iTempValue > iValue)
		{
			iValue = iTempValue;
			str = Localization::Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_PEOPLE_WE_TRUST_MORE");
		}

		iTempValue = pDiploAI->GetWeDenouncedFriendScore(eWithPlayer);
		if (iTempValue > iValue)
		{
			iValue = iTempValue;
			str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIENDS");
		}

		iTempValue = pDiploAI->GetFriendDenouncedUsScore(eWithPlayer);
		if (iTempValue > iValue)
		{
			iValue = iTempValue;
			str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_FRIEND_DENOUNCED");
		}

		iTempValue = pDiploAI->GetWeDeclaredWarOnFriendScore(eWithPlayer);
		if (iTempValue > iValue)
		{
			iValue = iTempValue;
			str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DECLARED_WAR_ON_FRIENDS");
		}

		iTempValue = pDiploAI->GetFriendDeclaredWarOnUsScore(eWithPlayer);
		if (iTempValue > iValue)
		{
			iValue = iTempValue;
			str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_FRIEND_DECLARED_WAR");
		}

		if (iValue != 0)
		{
			Opinion kOpinion;
			kOpinion.m_iValue = iValue;
			kOpinion.m_str = str;
			aOpinions.push_back(kOpinion);
		}
	}
	// TRAITOR END

	//iValue = pDiploAI->GetRequestsRefusedScore(eWithPlayer);
	//if (iValue != 0)
	//{
	//	Opinion kOpinion;
	//	kOpinion.m_iValue = iValue;
	//	kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_REFUSED_REQUESTS");
	//	aOpinions.push_back(kOpinion);
	//}

	iValue = pDiploAI->GetDenouncedUsScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_US");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDenouncedThemScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_THEM");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDenouncedFriendScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIEND");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetDenouncedEnemyScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_MUTUAL_ENEMY");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetRecklessExpanderScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_RECKLESS_EXPANDER");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetRecentTradeScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_TRADE_PARTNER");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetCommonFoeScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_COMMON_FOE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetRecentAssistScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		if (iValue > 0)
		{
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_REFUSED_REQUESTS");
		}
		else if (iValue < 0)
		{
			kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_ASSISTANCE_TO_THEM");
		}
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetNukedByScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_NUKED");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetCapitalCapturedByScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_CAPTURED_CAPITAL");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetGaveAssistanceToScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_ASSISTANCE_FROM_THEM");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetPaidTributeToScore(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_DIPLO_PAID_TRIBUTE");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetScenarioModifier1(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_SPECIFIC_DIPLO_STRING_1");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetScenarioModifier2(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_SPECIFIC_DIPLO_STRING_2");
		aOpinions.push_back(kOpinion);
	}

	iValue = pDiploAI->GetScenarioModifier3(eWithPlayer);
	if (iValue != 0)
	{
		Opinion kOpinion;
		kOpinion.m_iValue = iValue;
		kOpinion.m_str = Localization::Lookup("TXT_KEY_SPECIFIC_DIPLO_STRING_3");
		aOpinions.push_back(kOpinion);
	}

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
	int iValueImOffering, iValueTheirOffering;
	lua_pushinteger(L, pkThisPlayer->GetDealAI()->GetDealValue(pkDeal, iValueImOffering, iValueTheirOffering, false));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetDealMyValue(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, 2);
	int iValueImOffering, iValueTheyreOffering;
	pkThisPlayer->GetDealAI()->GetDealValue(pkDeal, iValueImOffering, iValueTheyreOffering, false);
	lua_pushinteger(L, iValueImOffering);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetDealTheyreValue(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, 2);
	int iValueImOffering, iValueTheyreOffering;
	pkThisPlayer->GetDealAI()->GetDealValue(pkDeal, iValueImOffering, iValueTheyreOffering, false);
	lua_pushinteger(L, iValueTheyreOffering);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaPlayer::lGetEspionageCityStatus(lua_State* L)
{
	CvPlayerAI* pkThisPlayer = GetInstance(L);
	CvPlayerEspionage* pkPlayerEspionage = pkThisPlayer->GetEspionage();

	lua_createtable(L, 0, 0);
	int index = 1;

	// first pass to get the largest base potential available
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
				CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
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

		const char* szSpyName = pkThisPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
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
		case SPY_STATE_RIG_ELECTION:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_RIGGING_ELECTION");
			break;
		case SPY_STATE_COUNTER_INTEL:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_COUNTER_INTEL");
			break;
		case SPY_STATE_DEAD:
			lua_pushstring(L, "TXT_KEY_SPY_STATE_DEAD");
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

		lua_rawseti(L, -2, index++);
	}
	return 1;
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
				if(pkPlayerEspionage->CanMoveSpyTo(pCity, uiSpyIndex))
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
