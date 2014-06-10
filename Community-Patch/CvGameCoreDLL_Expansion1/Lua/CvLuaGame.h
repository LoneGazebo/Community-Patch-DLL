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

	static int lGetUnitedNationsCountdown(lua_State* L);
	static int lSetUnitedNationsCountdown(lua_State* L);

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
	static int lGetMaxLand(lua_State* L);
	static int lGetMaxTech(lua_State* L);
	static int lGetMaxWonders(lua_State* L);
	static int lGetInitPopulation(lua_State* L);
	static int lGetInitLand(lua_State* L);
	static int lGetInitTech(lua_State* L);
	static int lGetInitWonders(lua_State* L);
	static int lGetNumWorldWonders(lua_State* L);

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
	static int lGetNumVictoryVotesTallied(lua_State* L);
	static int lGetVoteCast(lua_State* L);
	static int lGetPreviousVoteCast(lua_State* L);
	static int lGetNumVotesForTeam(lua_State* L);
	static int lSetNumVotesForTeam(lua_State* L);
	static int lChangeNumVotesForTeam(lua_State* L);

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

	static int lGetBestWondersPlayer(lua_State* L);
	static int lGetBestPoliciesPlayer(lua_State* L);
	static int lGetBestGreatPeoplePlayer(lua_State* L);

	static int lIsEverAttackedTutorial(lua_State* L);

	static int lGameplayDiplomacyAILeaderMessage(lua_State* L);

	static int lGetResourceUsageType(lua_State* L);

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

	static int lGetBuildingYieldChange(lua_State* L);
	static int lGetBuildingYieldModifier(lua_State* L);

	static int lGetWorldNumCitiesUnhappinessPercent(lua_State* L);

	static int lGetDealDuration(lua_State* L);
	static int lGetPeaceDuration(lua_State* L);

	static int lGetUnitUpgradesTo(lua_State* L);

	static int lGetCombatPrediction(lua_State* L);

	static int lGetTimeString(lua_State* L);

	static int lGetMinimumFaithNextPantheon(lua_State* L);
	static int lSetMinimumFaithNextPantheon(lua_State* L);

	static int lGetAvailablePantheonBeliefs(lua_State* L);
	static int lGetAvailableFounderBeliefs(lua_State* L);
	static int lGetAvailableFollowerBeliefs(lua_State* L);
	static int lGetAvailableEnhancerBeliefs(lua_State* L);
	static int lGetAvailableBonusBeliefs(lua_State* L);

	static int lGetNumFollowers(lua_State* L);
	static int lGetNumCitiesFollowing(lua_State* L);

	static int lGetBeliefsInReligion(lua_State* L);
	static int lGetNumReligionsFounded(lua_State* L);
	static int lGetNumReligionsStillToFound(lua_State* L);
	static int lGetHolyCityForReligion(lua_State* L);
	static int lGetReligionName(lua_State* L);
	static int lGetFounderBenefitsReligion(lua_State* L);

	static int lFoundPantheon(lua_State* L);
	static int lFoundReligion(lua_State* L);
	static int lEnhanceReligion(lua_State* L);
	static int lSetHolyCity(lua_State* L);
	static int lGetFounder(lua_State* L);
	static int lSetFounder(lua_State* L);

	static int lGetTurnsBetweenMinorCivElections(lua_State* L);
	static int lGetTurnsUntilMinorCivElection(lua_State* L);

	static int lIsProcessingMessages(lua_State* L);
};

#endif //CVLUAGAME_H
