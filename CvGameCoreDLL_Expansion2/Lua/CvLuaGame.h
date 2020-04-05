/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaGame.h
//!  \brief     Public interface to CvLuaGame.
//!
//!		This file includes the interface for exposing the Game namespace to
//!		Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUAGAME_H
#define CVLUAGAME_H

#include "CvLuaStaticInstance.h"

class CvLuaGame : public CvLuaStaticInstance<CvLuaGame, CvGame>
{
public:
	//! Returns the name of the static instance.
	static const char* GetInstanceName();

	//! Returns the static instance
	static CvGame* GetInstance(lua_State* L = NULL, int idx = 0);

	//! Registers member methods with instance.
	static void RegisterMembers(lua_State* L);

protected:

	static int lCanHandleAction(lua_State* L);
	static int lHandleAction(lua_State* L);

	static int lUpdateScore(lua_State* L);
	static int lCycleCities(lua_State* L);
	static int lCycleUnits(lua_State* L);
	static int lCyclePlotUnits(lua_State* L);

	static int lSelectionListMove(lua_State* L);
	static int lSelectionListGameNetMessage(lua_State* L);
	static int lSelectedCitiesGameNetMessage(lua_State* L);
	static int lCityPushOrder(lua_State* L);
	static int lCityPurchaseUnit(lua_State* L);
	static int lCityPurchaseBuilding(lua_State* L);
	static int lCityPurchaseProject(lua_State* L);

	static int lGetProductionPerPopulation(lua_State* L);

	static int lGetAdjustedPopulationPercent(lua_State* L);
	static int lGetAdjustedLandPercent(lua_State* L);

	static int lGetUnitedNationsCountdown(lua_State* L); // DEPRECATED
	static int lSetUnitedNationsCountdown(lua_State* L); // DEPRECATED

	static int lCountCivPlayersAlive(lua_State* L);
	static int lCountCivPlayersEverAlive(lua_State* L);
	static int lCountCivTeamsAlive(lua_State* L);
	static int lCountCivTeamsEverAlive(lua_State* L);

	static int lCountHumanPlayersAlive(lua_State* L);
	static int lCountHumanPlayersEverAlive(lua_State* L);

	static int lCountTotalCivPower(lua_State* L);
	static int lCountTotalNukeUnits(lua_State* L);
	static int lCountKnownTechNumTeams(lua_State* L);

	static int lGoldenAgeLength(lua_State* L);
	static int lVictoryDelay(lua_State* L);
	static int lGetImprovementUpgradeTime(lua_State* L);
	static int lCanTrainNukes(lua_State* L);

	static int lGetCurrentEra(lua_State* L);
	static int lGetDiploResponse(lua_State* L);

	static int lGetActiveTeam(lua_State* L);
	static int lGetActiveCivilizationType(lua_State* L);
	static int lIsNetworkMultiPlayer(lua_State* L);
	static int lIsGameMultiPlayer(lua_State* L);
	static int lIsTeamGame(lua_State* L);

	static int lReviveActivePlayer(lua_State* L);

	static int lGetNumHumanPlayers(lua_State* L);
	static int lGetNumSequentialHumans(lua_State* L);
	static int lGetGameTurn(lua_State* L);
	static int lSetGameTurn(lua_State* L);
	static int lGetTurnYear(lua_State* L);
	static int lGetGameTurnYear(lua_State* L);
	static int lGetTurnString(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(GetDateString, char*, iTurn);
#endif

	static int lGetElapsedGameTurns(lua_State* L);
	static int lGetMaxTurns(lua_State* L);
	static int lSetMaxTurns(lua_State* L);
	static int lChangeMaxTurns(lua_State* L);
	static int lGetMaxCityElimination(lua_State* L);
	static int lSetMaxCityElimination(lua_State* L);
	static int lGetNumAdvancedStartPoints(lua_State* L);
	static int lSetNumAdvancedStartPoints(lua_State* L);
	static int lGetStartTurn(lua_State* L);
	static int lGetWinningTurn(lua_State* L);
	static int lGetStartYear(lua_State* L);
	static int lSetStartYear(lua_State* L);
	static int lGetEstimateEndTurn(lua_State* L);
	static int lSetEstimateEndTurn(lua_State* L);
	static int lGetTurnSlice(lua_State* L);
	static int lGetMinutesPlayed(lua_State* L);
	static int lGetTargetScore(lua_State* L);
	static int lSetTargetScore(lua_State* L);

	static int lIsStaticTutorialActive(lua_State* L);
	static int lSetStaticTutorialActive(lua_State* L);
	static int lIsEverRightClickMoved(lua_State* L);
	static int lSetEverRightClickMoved(lua_State* L);

	static int lIsTutorialLogging(lua_State* L);
	static int lIsTutorialDebugging(lua_State* L);
	static int lGetTutorialLevel(lua_State* L);

	static int lHasAdvisorMessageBeenSeen(lua_State* L);
	static int lSetAdvisorMessageHasBeenSeen(lua_State* L);

	static int lSetAdvisorBadAttackInterrupt(lua_State* L);
	static int lSetAdvisorCityAttackInterrupt(lua_State* L);

	static int lGetAllowRClickMovementWhileScrolling(lua_State* L);

	static int lGetNumGameTurnActive(lua_State* L);
	static int lCountNumHumanGameTurnActive(lua_State* L);
	static int lGetNumCities(lua_State* L);
	static int lGetNumCivCities(lua_State* L);
	static int lGetTotalPopulation(lua_State* L);

	static int lGetNoNukesCount(lua_State* L);
	static int lIsNoNukes(lua_State* L);
	static int lChangeNoNukesCount(lua_State* L);
	static int lGetNukesExploded(lua_State* L);
	static int lChangeNukesExploded(lua_State* L);

	static int lGetMaxPopulation(lua_State* L);
	static int lGetInitPopulation(lua_State* L);
	static int lGetInitLand(lua_State* L);
	static int lGetInitTech(lua_State* L);
	static int lGetInitWonders(lua_State* L);
	static int lGetNumWorldWonders(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS)
	static int lIsWorldWonderClass(lua_State* L);
#endif

	static int lGetAIAutoPlay(lua_State* L);
	static int lSetAIAutoPlay(lua_State* L);

	static int lIsScoreDirty(lua_State* L);
	static int lSetScoreDirty(lua_State* L);
	static int lIsCircumnavigated(lua_State* L);
	static int lMakeCircumnavigated(lua_State* L);

	static int lDoFromUIDiploEvent(lua_State* L);

	static int lIsDebugMode(lua_State* L);
	static int lSetDebugMode(lua_State* L);
	static int lToggleDebugMode(lua_State* L);
	static int lUpdateFOW(lua_State* L);

	static int lGetPitbossTurnTime(lua_State* L);
	static int lSetPitbossTurnTime(lua_State* L);
	static int lIsHotSeat(lua_State* L);
	static int lIsPbem(lua_State* L);
	static int lIsPitboss(lua_State* L);
	static int lIsSimultaneousTeamTurns(lua_State* L);

	static int lIsFinalInitialized(lua_State* L);

	static int lGetActivePlayer(lua_State* L);
	static int lSetActivePlayer(lua_State* L);
	static int lGetPausePlayer(lua_State* L);
	static int lSetPausePlayer(lua_State* L);
	static int lIsPaused(lua_State* L);
	static int lGetBestLandUnit(lua_State* L);
	static int lGetBestLandUnitCombat(lua_State* L);

	static int lGetFaithCost(lua_State* L);

	static int lGetWinner(lua_State* L);
	static int lGetVictory(lua_State* L);
	static int lSetWinner(lua_State* L);
	static int lGetGameState(lua_State* L);
	static int lSetGameState(lua_State* L);

	static int lGetVotesNeededForDiploVictory(lua_State* L);
	static int lIsUnitedNationsActive(lua_State* L);

	// DEPRECATED
	static int lGetNumVictoryVotesTallied(lua_State* L);
	static int lGetVoteCast(lua_State* L);
	static int lGetPreviousVoteCast(lua_State* L);
	static int lGetNumVotesForTeam(lua_State* L);
	static int lSetNumVotesForTeam(lua_State* L);
	static int lChangeNumVotesForTeam(lua_State* L);
	// End DEPRECATED

	static int lGetHandicapType(lua_State* L);
	static int lGetCalendar(lua_State* L);
	static int lGetStartEra(lua_State* L);
	static int lGetGameSpeedType(lua_State* L);
	static int lGetRankPlayer(lua_State* L);
	static int lGetPlayerRank(lua_State* L);
	static int lGetPlayerScore(lua_State* L);
	static int lGetRankTeam(lua_State* L);
	static int lGetTeamRank(lua_State* L);
	static int lGetTeamScore(lua_State* L);
	static int lIsOption(lua_State* L);
	static int lSetOption(lua_State* L);
	static int lIsMPOption(lua_State* L);
	static int lGetUnitCreatedCount(lua_State* L);
	static int lGetUnitClassCreatedCount(lua_State* L);
	static int lIsUnitClassMaxedOut(lua_State* L);
	static int lGetBuildingClassCreatedCount(lua_State* L);
	static int lIsBuildingClassMaxedOut(lua_State* L);

	static int lGetProjectCreatedCount(lua_State* L);
	static int lIsProjectMaxedOut(lua_State* L);

	static int lIsVictoryValid(lua_State* L);
	static int lSetVictoryValid(lua_State* L);
	static int lIsSpecialUnitValid(lua_State* L);
	static int lMakeSpecialUnitValid(lua_State* L);
	static int lIsNukesValid(lua_State* L);
	static int lMakeNukesValid(lua_State* L);
	static int lIsInAdvancedStart(lua_State* L);

	static int lSetName(lua_State* L);
	static int lGetName(lua_State* L);
	static int lRand(lua_State* L);
	static int lCalculateSyncChecksum(lua_State* L);
	static int lCalculateOptionsChecksum(lua_State* L);

	static int lGetReplayMessage(lua_State* L);
	static int lGetReplayMessages(lua_State* L);
	static int lGetNumReplayMessages(lua_State* L);
	static int lGetReplayInfo(lua_State* L);

	static int lSaveReplay(lua_State* L);

	static int lAddPlayer(lua_State* L);

	static int lSetPlotExtraYield(lua_State* L);
	static int lChangePlotExtraCost(lua_State* L);

	static int lIsCivEverActive(lua_State* L);
	static int lIsLeaderEverActive(lua_State* L);
	static int lIsUnitEverActive(lua_State* L);
	static int lIsBuildingEverActive(lua_State* L);

	static int lCanDoControl(lua_State* L);
	static int lDoControl(lua_State* L);

	static int lDoMinorPledgeProtection(lua_State* L);
	static int lDoMinorGoldGift(lua_State* L); // old name, kept here for backwards compatibility with old Lua
	static int lDoMinorGiftGold(lua_State* L);
	static int lDoMinorGiftTileImprovement(lua_State* L);
	static int lDoMinorBullyGold(lua_State* L);
	static int lDoMinorBullyUnit(lua_State* L);
	static int lDoMinorBuyout(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lDoMinorMarriage(lua_State* L);
#endif

	static int lGetBestWondersPlayer(lua_State* L);
	static int lGetBestPoliciesPlayer(lua_State* L);
	static int lGetBestGreatPeoplePlayer(lua_State* L);

	static int lIsEverAttackedTutorial(lua_State* L);

	static int lGameplayDiplomacyAILeaderMessage(lua_State* L);

	static int lGetResourceUsageType(lua_State* L);

#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
	static int lGetNumResourceTotalRequiredForUnit(lua_State* L);
#endif

	static int lGetNumResourceRequiredForUnit(lua_State* L);
	static int lGetNumResourceRequiredForBuilding(lua_State* L);

	static int lIsCombatWarned(lua_State* L);
	static int lSetCombatWarned(lua_State* L);

	static int lGetAdvisorCounsel(lua_State* L);
	static int lSetAdvisorRecommenderCity(lua_State* L);

	static int lIsUnitRecommended(lua_State* L);
	static int lIsBuildingRecommended(lua_State* L);
	static int lIsProjectRecommended(lua_State* L);

	static int lSetAdvisorRecommenderTech(lua_State* L);
	static int lIsTechRecommended(lua_State* L);

	static int lGetVariableCitySizeFromPopulation(lua_State* L);

	static int lGetResearchAgreementCost(lua_State* L);

	//! Gets a custom game option of specified name.
	static int lGetCustomOption(lua_State* L);

	static int lGetNumCitiesPolicyCostMod(lua_State* L);
	static int lGetNumCitiesTechCostMod(lua_State* L);

	static int lGetNumCitiesTourismCostMod(lua_State* L);

	static int lGetBuildingYieldChange(lua_State* L);
	static int lGetBuildingYieldModifier(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lGetPovertyHappinessChangeBuilding(lua_State* L);
	static int lGetDefenseHappinessChangeBuilding(lua_State* L);
	static int lGetUnculturedHappinessChangeBuilding(lua_State* L);
	static int lGetIlliteracyHappinessChangeBuilding(lua_State* L);
	static int lGetMinorityHappinessChangeBuilding(lua_State* L);
	static int lGetPovertyHappinessChangeBuildingGlobal(lua_State* L);
	static int lGetDefenseHappinessChangeBuildingGlobal(lua_State* L);
	static int lGetUnculturedHappinessChangeBuildingGlobal(lua_State* L);
	static int lGetIlliteracyHappinessChangeBuildingGlobal(lua_State* L);
	static int lGetMinorityHappinessChangeBuildingGlobal(lua_State* L);
	static int lGetBuildingCorporateGPChange(lua_State* L);
	static int lGetPromiseDuration(lua_State* L);
	static int lGetCorporationFounder(lua_State* L);
	static int lGetCorporationHeadquarters(lua_State* L);
	static int lGetNumCorporationsFounded(lua_State* L);
	static int lGetNumAvailableCorporations(lua_State* L);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	static int lGetGreatestPlayerResourceMonopoly(lua_State* L);
#endif
#endif

	static int lGetWorldNumCitiesUnhappinessPercent(lua_State* L);

	static int lGetDealDuration(lua_State* L);
	static int lGetRelationshipDuration(lua_State* L);
	static int lGetPeaceDuration(lua_State* L);

	static int lGetUnitUpgradesTo(lua_State* L);

	static int lGetCombatPrediction(lua_State* L);

	static int lGetTimeString(lua_State* L);

	static int lGetMinimumFaithNextPantheon(lua_State* L);
	static int lSetMinimumFaithNextPantheon(lua_State* L);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
	LUAAPIEXTN(IsInSomeReligion, bool, iBelief, iPlayer);
#endif
	static int lGetAvailablePantheonBeliefs(lua_State* L);
	static int lGetAvailableFounderBeliefs(lua_State* L);
	static int lGetAvailableFollowerBeliefs(lua_State* L);
	static int lGetAvailableEnhancerBeliefs(lua_State* L);
	static int lGetAvailableBonusBeliefs(lua_State* L);
	static int lGetAvailableReformationBeliefs(lua_State* L);

	static int lGetNumFollowers(lua_State* L);
	static int lGetNumCitiesFollowing(lua_State* L);

	static int lGetBeliefsInReligion(lua_State* L);
	static int lGetNumReligionsFounded(lua_State* L);
	static int lGetNumReligionsStillToFound(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lBeliefIsInReligion(lua_State* L);
#endif
	static int lGetHolyCityForReligion(lua_State* L);
	static int lGetReligionName(lua_State* L);
	static int lGetFounderBenefitsReligion(lua_State* L);

	static int lFoundPantheon(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
	LUAAPIEXTN(EnhancePantheon, void, iPlayer, iBelief);
#endif
	static int lFoundReligion(lua_State* L);
	static int lEnhanceReligion(lua_State* L);
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(AddReformation, void, iPlayer, iReligion, iBelief);
#endif
	static int lSetHolyCity(lua_State* L);
	static int lGetFounder(lua_State* L);
	static int lSetFounder(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lSetFoundYear(lua_State* L);
	static int lGetFoundYear(lua_State* L);
#endif

	static int lGetTurnsBetweenMinorCivElections(lua_State* L);
	static int lGetTurnsUntilMinorCivElection(lua_State* L);
	
#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(IsAchievementUnlocked, bool, iAchievement);
	LUAAPIEXTN(GetSteamStat, int, iSteamStat);
#endif

	static int lGetNumActiveLeagues(lua_State* L);
	static int lGetNumLeaguesEverFounded(lua_State* L);
	static int lGetLeague(lua_State* L);
	static int lGetActiveLeague(lua_State* L);
#if defined(MOD_BALANCE_CORE)
	static int lDoEnactResolution(lua_State* L);
	static int lDoRepealResolution(lua_State* L);
	static int lIsBeliefValid(lua_State* L);
#endif
	
	static int lIsProcessingMessages(lua_State* L);

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(CreateGreatWork, int, iGreatWorkType, iOwningPlayer, iEra, sCreator);
#endif
	static int lGetGreatWorkTooltip(lua_State* L);
	static int lGetGreatWorkName(lua_State* L);
	static int lGetGreatWorkType(lua_State* L);
	static int lGetGreatWorkClass(lua_State* L);
	static int lGetGreatWorkArtist(lua_State* L);
	static int lGetGreatWorkEra(lua_State* L);
	static int lGetGreatWorkEraAbbreviation(lua_State* L);
	static int lGetGreatWorkEraShort(lua_State* L);
	static int lGetGreatWorkCreator(lua_State* L);
	static int lGetGreatWorkController(lua_State* L);
	static int lGetGreatWorkCurrentThemingBonus(lua_State* L);
	static int lGetArtifactName(lua_State* L);

	static int lGetNumFreePolicies(lua_State* L);

	static int lGetLongestCityConnectionPlots(lua_State* L);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_TRADEROUTES)
	LUAAPIEXTN(GetTradeRoute, table, iRouteIndex);
#endif
	static int lSelectedUnit_SpeculativePopupTradeRoute_Display(lua_State* L);
	static int lSelectedUnit_SpeculativePopupTradeRoute_Hide(lua_State* L);

	static int lMouseoverUnit(lua_State* L);

	static int lGetNumArchaeologySites(lua_State *L);
	static int lGetNumHiddenArchaeologySites(lua_State *L);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_EXTENSIONS)
	LUAAPIEXTN(ExitLeaderScreen, void);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(GetDllGuid, char*);
	LUAAPIEXTN(ReloadGameDataDefines, void);
	LUAAPIEXTN(ReloadCustomModOptions, void);
	LUAAPIEXTN(IsCustomModOption, bool, sOption);
	LUAAPIEXTN(GetCustomModOption, int, sOption);
	LUAAPIEXTN(SpewTestEvents, int, iLimit); // returns iSeconds, iMilliSeconds, iValue
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CIV4_FEATURES)
	LUAAPIEXTN(GetMinimumVoluntaryVassalTurns, int);
	LUAAPIEXTN(GetMinimumVassalTurns, int);
	LUAAPIEXTN(GetNumTurnsBetweenVassals, int);
	LUAAPIEXTN(GetMinimumVassalTax, int);
	LUAAPIEXTN(GetMaximumVassalTax, int);
	LUAAPIEXTN(GetMinimumVassalLiberateTurns, int);
	LUAAPIEXTN(GetMinimumVassalTaxTurns, int);
	LUAAPIEXTN(GetVassalageEnabledEra, int);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	LUAAPIEXTN(AnyoneHasBelief, bool, iBeliefType);
	LUAAPIEXTN(AnyoneHasBuilding, bool, iBuildingType);
	LUAAPIEXTN(AnyoneHasBuildingClass, bool, iBuildingClassType);
	LUAAPIEXTN(AnyoneHasAnyWonder, bool);
	LUAAPIEXTN(AnyoneHasWonder, bool, iBuildingType);
	LUAAPIEXTN(GetCivilizationPlayer, int, iCivilizationType);
	LUAAPIEXTN(AnyoneIsInEra, bool, iEraType);
	LUAAPIEXTN(AnyoneHasReachedEra, bool, iEraType);
	LUAAPIEXTN(AnyoneHasAnyNaturalWonder, bool);
	LUAAPIEXTN(AnyoneHasNaturalWonder, bool, iFeatureType);
	LUAAPIEXTN(AnyoneHasPolicy, bool, iPolicyType);
	LUAAPIEXTN(AnyoneHasTenet, bool, iPolicyType);
	LUAAPIEXTN(AnyoneHasPolicyBranch, bool, iPolicyBranchType);
	LUAAPIEXTN(AnyoneHasIdeology, bool, iPolicyBranchType);
	LUAAPIEXTN(AnyoneHasProject, bool, iProjectType);
	LUAAPIEXTN(AnyoneHasPantheon, bool);
	LUAAPIEXTN(AnyoneHasAnyReligion, bool);
	LUAAPIEXTN(AnyoneHasReligion, bool, iReligionType);
	LUAAPIEXTN(IsResolutionPassed, bool, iResolutionType, iChoice);
	LUAAPIEXTN(AnyoneHasTech, bool, iTechType);
	LUAAPIEXTN(AnyoneHasUnit, bool, iUnitType);
	LUAAPIEXTN(AnyoneHasUnitClass, bool, iUnitClassType);
#endif

#if defined(MOD_BALANCE_CORE)
	static int lFoundCorporation(lua_State* L);
	static int lCanFoundCorporation(lua_State* L);
	static int lIsCorporationFounded(lua_State* L);

	//Contracts
	static int lDoUpdateContracts(lua_State* L);
	static int lGetNumActiveContracts(lua_State* L);
	static int lGetNumInactiveContracts(lua_State* L);
	static int lGetNumAvailableContracts(lua_State* L);
	static int lGetNumUnavailableContracts(lua_State* L);
	static int lGetActiveContract(lua_State* L);
	static int lGetInactiveContract(lua_State* L);
	static int lIsContractActive(lua_State* L);
	static int lIsContractAvailable(lua_State* L);
	static int lSetContractUnits(lua_State* L);
	static int lGetContractUnits(lua_State* L);
	static int lGetInactiveContractUnitList(lua_State* L);
	static int lGetActiveContractUnitList(lua_State* L);

	LUAAPIEXTN(DoSpawnFreeCity, void);
#endif

#if defined(MOD_BATTLE_ROYALE)
	// Lua CSV Logging Functions
	static int lDeleteCSV(lua_State* L);
	static int lWriteCSV(lua_State* L);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	static int lIsPitbossHost(lua_State* L);
	static int lIsHost(lua_State* L);
	static int lGetTimeStringForYear(lua_State* L);
#endif
};

#endif //CVLUAGAME_H
