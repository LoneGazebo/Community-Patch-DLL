/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaGame.cpp
//!  \brief     Private implementation to CvLuaGame.
//!
//!		This file includes the implementation for exposing the Game namespace
//!		to Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaCity.h"
#include "CvLuaGame.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#include "CvLuaLeague.h"

#include "../CvGame.h"
#include "../CvGameCoreUtils.h"
#include "../CvInternalGameCoreUtils.h"
#include "../CvGameTextMgr.h"
#include "../CvReplayMessage.h"
#include "../cvStopWatch.h"

#if defined(MOD_BATTLE_ROYALE)
#include "../CvLoggerCSV.h"
#endif

#define Method(func) RegisterMethod(L, l##func, #func);

//------------------------------------------------------------------------------
const char* CvLuaGame::GetInstanceName()
{
	return "Game";
}
//------------------------------------------------------------------------------
CvGame* CvLuaGame::GetInstance(lua_State* L, int idx)
{
	return &GC.getGame();
}
//------------------------------------------------------------------------------
void CvLuaGame::RegisterMembers(lua_State* L)
{
	Method(CanHandleAction);
	Method(HandleAction);
	Method(UpdateScore);
	Method(CycleCities);
	Method(CycleUnits);
	Method(CyclePlotUnits);

	Method(SelectionListMove);
	Method(SelectionListGameNetMessage);
	Method(SelectedCitiesGameNetMessage);
	Method(CityPushOrder);
	Method(CityPurchaseUnit);
	Method(CityPurchaseBuilding);
	Method(CityPurchaseProject);

	Method(GetProductionPerPopulation);

	Method(GetAdjustedPopulationPercent);
	Method(GetAdjustedLandPercent);

	Method(GetUnitedNationsCountdown);
	Method(SetUnitedNationsCountdown);

	Method(CountCivPlayersAlive);
	Method(CountCivPlayersEverAlive);
	Method(CountCivTeamsAlive);
	Method(CountCivTeamsEverAlive);
	Method(CountHumanPlayersAlive);
	Method(CountHumanPlayersEverAlive);

	Method(CountTotalCivPower);
	Method(CountTotalNukeUnits);
	Method(CountKnownTechNumTeams);

	Method(GoldenAgeLength);
	Method(VictoryDelay);
	Method(GetImprovementUpgradeTime);
	Method(CanTrainNukes);

	Method(GetCurrentEra);

	Method(GetDiploResponse);

	Method(GetActiveTeam);
	Method(GetActiveCivilizationType);
	Method(IsNetworkMultiPlayer);
	Method(IsGameMultiPlayer);
	Method(IsTeamGame);

	Method(ReviveActivePlayer);

	Method(GetNumHumanPlayers);
	Method(GetNumSequentialHumans);
	Method(GetGameTurn);
	Method(SetGameTurn);
	Method(GetTurnYear);
	Method(GetGameTurnYear);
	Method(GetTurnString);
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(GetDateString);
#endif

	Method(GetElapsedGameTurns);
	Method(GetMaxTurns);
	Method(SetMaxTurns);
	Method(ChangeMaxTurns);
	Method(GetMaxCityElimination);
	Method(SetMaxCityElimination);
	Method(GetNumAdvancedStartPoints);
	Method(SetNumAdvancedStartPoints);
	Method(GetStartTurn);
	Method(GetWinningTurn);
	Method(GetStartYear);
	Method(SetStartYear);
	Method(GetEstimateEndTurn);
	Method(SetEstimateEndTurn);
	Method(GetTurnSlice);
	Method(GetMinutesPlayed);
	Method(GetTargetScore);
	Method(SetTargetScore);

	Method(IsStaticTutorialActive);
	Method(SetStaticTutorialActive);
	Method(IsEverRightClickMoved);
	Method(SetEverRightClickMoved);

	Method(IsTutorialLogging);
	Method(IsTutorialDebugging);
	Method(GetTutorialLevel);

	Method(HasAdvisorMessageBeenSeen);
	Method(SetAdvisorMessageHasBeenSeen);

	Method(SetAdvisorBadAttackInterrupt);
	Method(SetAdvisorCityAttackInterrupt);

	Method(GetAllowRClickMovementWhileScrolling);

	Method(GetNumGameTurnActive);
	Method(CountNumHumanGameTurnActive);
	Method(GetNumCities);
	Method(GetNumCivCities);
	Method(GetTotalPopulation);

	Method(GetNoNukesCount);
	Method(IsNoNukes);
	Method(ChangeNoNukesCount);
	Method(GetNukesExploded);
	Method(ChangeNukesExploded);

	Method(GetMaxPopulation);
	Method(GetInitPopulation);
	Method(GetInitLand);
	Method(GetInitTech);
	Method(GetInitWonders);
	Method(GetNumWorldWonders);
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(IsWorldWonderClass);
#endif

	Method(GetAIAutoPlay);
	Method(SetAIAutoPlay);

	Method(IsScoreDirty);
	Method(SetScoreDirty);
	Method(IsCircumnavigated);
	Method(MakeCircumnavigated);

	Method(DoFromUIDiploEvent);

	Method(IsDebugMode);
	Method(SetDebugMode);
	Method(ToggleDebugMode);
	Method(UpdateFOW);

	Method(GetPitbossTurnTime);
	Method(SetPitbossTurnTime);
	Method(IsHotSeat);
	Method(IsPbem);
	Method(IsPitboss);
	Method(IsSimultaneousTeamTurns);

	Method(IsFinalInitialized);

	Method(GetActivePlayer);
	Method(SetActivePlayer);
	Method(GetPausePlayer);
	Method(SetPausePlayer);
	Method(IsPaused);
	Method(GetBestLandUnit);
	Method(GetBestLandUnitCombat);

	Method(GetFaithCost);

	Method(GetWinner);
	Method(GetVictory);
	Method(SetWinner);
	Method(GetGameState);
	Method(SetGameState);

	Method(GetVotesNeededForDiploVictory);
	Method(IsUnitedNationsActive);

	Method(GetNumVictoryVotesTallied);
	Method(GetVoteCast);
	Method(GetPreviousVoteCast);
	Method(GetNumVotesForTeam);
	Method(SetNumVotesForTeam);
	Method(ChangeNumVotesForTeam);

	Method(GetHandicapType);
	Method(GetCalendar);
	Method(GetStartEra);
	Method(GetGameSpeedType);
	Method(GetRankPlayer);
	Method(GetPlayerRank);
	Method(GetPlayerScore);
	Method(GetRankTeam);
	Method(GetTeamRank);
	Method(GetTeamScore);
	Method(IsOption);
	Method(SetOption);
	Method(IsMPOption);
	Method(GetUnitCreatedCount);
	Method(GetUnitClassCreatedCount);
	Method(IsUnitClassMaxedOut);
	Method(GetBuildingClassCreatedCount);
	Method(IsBuildingClassMaxedOut);

	Method(GetProjectCreatedCount);
	Method(IsProjectMaxedOut);

	Method(IsVictoryValid);
	Method(SetVictoryValid);
	Method(IsSpecialUnitValid);
	Method(MakeSpecialUnitValid);
	Method(IsNukesValid);
	Method(MakeNukesValid);
	Method(IsInAdvancedStart);

	Method(SetName);
	Method(GetName);
	Method(Rand);
	Method(CalculateSyncChecksum);
	Method(CalculateOptionsChecksum);

	Method(GetReplayMessage);
	Method(GetReplayMessages);
	Method(GetNumReplayMessages);
	Method(GetReplayInfo);

	Method(SaveReplay);

	Method(AddPlayer);

	Method(SetPlotExtraYield);
	Method(ChangePlotExtraCost);

	Method(IsCivEverActive);
	Method(IsLeaderEverActive);
	Method(IsUnitEverActive);
	Method(IsBuildingEverActive);

	Method(CanDoControl);
	Method(DoControl);

	Method(DoMinorPledgeProtection);
	Method(DoMinorGoldGift);
	Method(DoMinorGiftGold);
	Method(DoMinorGiftTileImprovement);
	Method(DoMinorBullyGold);
	Method(DoMinorBullyUnit);
	Method(DoMinorBuyout);
#if defined(MOD_BALANCE_CORE)
	Method(DoMinorMarriage);
#endif

	Method(GetBestWondersPlayer);
	Method(GetBestPoliciesPlayer);
	Method(GetBestGreatPeoplePlayer);

	Method(IsEverAttackedTutorial);

	Method(GameplayDiplomacyAILeaderMessage);

	Method(GetResourceUsageType);

#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
	Method(GetNumResourceTotalRequiredForUnit);
#endif

	Method(GetNumResourceRequiredForUnit);
	Method(GetNumResourceRequiredForBuilding);

	Method(IsCombatWarned);
	Method(SetCombatWarned);

	Method(GetAdvisorCounsel);
	Method(SetAdvisorRecommenderCity);

	Method(IsUnitRecommended);
	Method(IsBuildingRecommended);
	Method(IsProjectRecommended);

	Method(SetAdvisorRecommenderTech);
	Method(IsTechRecommended);

	Method(GetVariableCitySizeFromPopulation);

	Method(GetResearchAgreementCost);

	Method(GetCustomOption);

	Method(GetNumCitiesPolicyCostMod);
	Method(GetNumCitiesTechCostMod);
	Method(GetNumCitiesTourismCostMod);

	Method(GetBuildingYieldChange);
	Method(GetBuildingYieldModifier);
#if defined(MOD_BALANCE_CORE)
	Method(GetBuildingCorporateGPChange);
	Method(GetPovertyHappinessChangeBuilding);
	Method(GetDefenseHappinessChangeBuilding);
	Method(GetUnculturedHappinessChangeBuilding);
	Method(GetIlliteracyHappinessChangeBuilding);
	Method(GetMinorityHappinessChangeBuilding);
	Method(GetPovertyHappinessChangeBuildingGlobal);
	Method(GetDefenseHappinessChangeBuildingGlobal);
	Method(GetUnculturedHappinessChangeBuildingGlobal);
	Method(GetIlliteracyHappinessChangeBuildingGlobal);
	Method(GetMinorityHappinessChangeBuildingGlobal);
	Method(GetPromiseDuration);
	Method(GetCorporationFounder);
	Method(GetCorporationHeadquarters);
	Method(GetNumCorporationsFounded);
	Method(GetNumAvailableCorporations);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	Method(GetGreatestPlayerResourceMonopoly);
#endif
#endif

	Method(GetWorldNumCitiesUnhappinessPercent);

	Method(GetDealDuration);
	Method(GetRelationshipDuration);
	Method(GetPeaceDuration);

	Method(GetUnitUpgradesTo);

	Method(GetCombatPrediction);

	Method(GetTimeString);

	Method(GetMinimumFaithNextPantheon);
	Method(SetMinimumFaithNextPantheon);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
	Method(IsInSomeReligion);
#endif
	Method(GetAvailablePantheonBeliefs);
	Method(GetAvailableFounderBeliefs);
	Method(GetAvailableFollowerBeliefs);
	Method(GetAvailableEnhancerBeliefs);
	Method(GetAvailableBonusBeliefs);
	Method(GetAvailableReformationBeliefs);

	Method(GetNumFollowers);
	Method(GetNumCitiesFollowing);

	Method(GetBeliefsInReligion);
	Method(GetNumReligionsStillToFound);
#if defined(MOD_BALANCE_CORE)
	Method(BeliefIsInReligion);
#endif
	Method(GetNumReligionsFounded);
	Method(GetHolyCityForReligion);
	Method(GetReligionName);
	Method(GetFounderBenefitsReligion);

	Method(FoundPantheon);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
	Method(EnhancePantheon);
#endif
	Method(FoundReligion);
	Method(EnhanceReligion);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE)
	Method(AddReformation);
#endif
	Method(SetHolyCity);
	Method(GetFounder);
	Method(SetFounder);
#if defined(MOD_BALANCE_CORE)
	Method(SetFoundYear);
	Method(GetFoundYear);
#endif

	Method(GetTurnsBetweenMinorCivElections);
	Method(GetTurnsUntilMinorCivElection);

	Method(GetNumActiveLeagues);
	Method(GetNumLeaguesEverFounded);
	Method(GetLeague);
	Method(GetActiveLeague);
#if defined(MOD_BALANCE_CORE)
	Method(DoEnactResolution);
	Method(DoRepealResolution);
	Method(IsBeliefValid);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(IsAchievementUnlocked);
	Method(GetSteamStat);
#endif

	Method(IsProcessingMessages);

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(CreateGreatWork);
#endif
	Method(GetGreatWorkTooltip);
	Method(GetGreatWorkName);
	Method(GetGreatWorkType);
	Method(GetGreatWorkClass);
	Method(GetGreatWorkArtist);
	Method(GetGreatWorkEra);
	Method(GetGreatWorkEraAbbreviation);
	Method(GetGreatWorkEraShort);
	Method(GetGreatWorkCreator);
	Method(GetGreatWorkController);
	Method(GetGreatWorkCurrentThemingBonus);
	Method(GetArtifactName);

	Method(GetNumFreePolicies);

	Method(GetLongestCityConnectionPlots);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_TRADEROUTES)
	Method(GetTradeRoute);
#endif
	Method(SelectedUnit_SpeculativePopupTradeRoute_Display);
	Method(SelectedUnit_SpeculativePopupTradeRoute_Hide);
	Method(MouseoverUnit);

	Method(GetNumArchaeologySites);
	Method(GetNumHiddenArchaeologySites);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_EXTENSIONS)
	Method(ExitLeaderScreen);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(GetDllGuid);
	Method(ReloadGameDataDefines);
	Method(ReloadCustomModOptions);
	Method(IsCustomModOption);
	Method(GetCustomModOption);
	Method(SpewTestEvents);
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CIV4_FEATURES)
	Method(GetMinimumVassalTurns);
	Method(GetNumTurnsBetweenVassals);
	Method(GetMinimumVoluntaryVassalTurns);
	Method(GetMinimumVassalTax);
	Method(GetMaximumVassalTax);
	Method(GetMinimumVassalLiberateTurns);
	Method(GetMinimumVassalTaxTurns);
	Method(GetVassalageEnabledEra);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(AnyoneHasBelief);
	Method(AnyoneHasBuilding);
	Method(AnyoneHasBuildingClass);
	Method(AnyoneHasAnyWonder);
	Method(AnyoneHasWonder);
	Method(GetCivilizationPlayer);
	Method(AnyoneIsInEra);
	Method(AnyoneHasReachedEra);
	Method(AnyoneHasAnyNaturalWonder);
	Method(AnyoneHasNaturalWonder);
	Method(AnyoneHasPolicy);
	Method(AnyoneHasTenet);
	Method(AnyoneHasPolicyBranch);
	Method(AnyoneHasIdeology);
	Method(AnyoneHasProject);
	Method(AnyoneHasPantheon);
	Method(AnyoneHasAnyReligion);
	Method(AnyoneHasReligion);
	Method(IsResolutionPassed);
	Method(AnyoneHasTech);
	Method(AnyoneHasUnit);
	Method(AnyoneHasUnitClass);
#endif

#if defined(MOD_BALANCE_CORE)
	Method(FoundCorporation);
	Method(CanFoundCorporation);
	Method(IsCorporationFounded);

	//Contracts
	Method(DoUpdateContracts);
	Method(GetNumActiveContracts);
	Method(GetNumInactiveContracts);
	Method(GetNumAvailableContracts);
	Method(GetNumUnavailableContracts);
	Method(GetActiveContract);
	Method(GetInactiveContract);
	Method(IsContractActive);
	Method(IsContractAvailable);
	Method(SetContractUnits);
	Method(GetContractUnits);
	Method(GetInactiveContractUnitList);
	Method(GetActiveContractUnitList);

	Method(DoSpawnFreeCity);
#endif

#if defined(MOD_BATTLE_ROYALE)
	Method(DeleteCSV);
	Method(WriteCSV);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(IsPitbossHost);
	Method(IsHost);
	Method(GetTimeStringForYear);
#endif

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------
// bool CanHandleAction(int iAction, CvPlot* pPlot, bool bTestVisible)
int CvLuaGame::lCanHandleAction(lua_State* L)
{
	const int iAction = lua_tointeger(L, 1);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2, false);
	const bool bTestVisible = luaL_optbool(L, 3, 0);
	const bool bResult = GetInstance()->canHandleAction(iAction, pkPlot, bTestVisible);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void HandleAction(iAction)
int CvLuaGame::lHandleAction(lua_State* L)
{
	const int iAction = lua_tointeger(L, 1);
	GetInstance()->handleAction(iAction);
	return 0;
}
//------------------------------------------------------------------------------
// void updateScore(bool bForce);
int CvLuaGame::lUpdateScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::updateScore);
}
//------------------------------------------------------------------------------
// void cycleCities(bool bForward, bool bAdd);
int CvLuaGame::lCycleCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::cycleCities);
}
//------------------------------------------------------------------------------
// void cycleUnits(bool bClear, bool bForward, bool bWorkers);
int CvLuaGame::lCycleUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::cycleUnits);
}
//------------------------------------------------------------------------------
// bool cyclePlotUnits(CyPlot* pPlot, bool bForward, bool bAuto, int iCount);
int CvLuaGame::lCyclePlotUnits(lua_State* L)
{
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 1);
	const bool bForward = lua_toboolean(L, 2);
	const bool bAuto = lua_toboolean(L, 3);
	const int iCount = lua_tointeger(L, 4);

	const bool bResult = GetInstance()->cyclePlotUnits(pkPlot, bForward, bAuto, iCount);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
// void selectionListMove(CyPlot* pPlot, bool bAlt, bool bShift, bool bCtrl);
int CvLuaGame::lSelectionListMove(lua_State* L)
{
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 1);
	const bool bShift = lua_toboolean(L, 3);

	GetInstance()->selectionListMove(pkPlot, bShift);
	return 0;
}
//------------------------------------------------------------------------------
// void selectionListGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, int iFlags = 0, bool bAlt = false, bool bShift = false);
int CvLuaGame::lSelectionListGameNetMessage(lua_State* L)
{
	const int eMessage	= lua_tointeger(L, 1);
	const int iData2	= luaL_optint(L, 2, -1);
	const int iData3	= luaL_optint(L, 3, -1);
	const int iData4	= luaL_optint(L, 4, -1);
	const int iFlags	= luaL_optint(L, 5, 0);
	const bool bAlt		= luaL_optbool(L, 6, 0);
	const bool bShift	= luaL_optbool(L, 7, 0);

	GetInstance()->selectionListGameNetMessage(eMessage, iData2, iData3, iData4, iFlags, bAlt, bShift);
	return 0;
}
//------------------------------------------------------------------------------
// void selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl);
int CvLuaGame::lSelectedCitiesGameNetMessage(lua_State* L)
{
	const int eMessage = lua_tointeger(L, 1);
	const int iData2 = luaL_optint(L, 2, -1);
	const int iData3 = luaL_optint(L, 3, -1);
	const int iData4 = luaL_optint(L, 4, -1);
	const bool bOption	= luaL_optbool(L, 5, 0);
	const bool bAlt		= luaL_optbool(L, 6, 0);
	const bool bShift	= luaL_optbool(L, 7, 0);
	const bool bCtrl	= luaL_optbool(L, 8, 0);
	GetInstance()->selectedCitiesGameNetMessage(eMessage, iData2, iData3, iData4, bOption, bAlt, bShift, bCtrl);
	return 0;
}
//------------------------------------------------------------------------------
// void cityPushOrder(CyCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);
int CvLuaGame::lCityPushOrder(lua_State* L)
{
	CvCity* pkCity			= CvLuaCity::GetInstance(L, 1);
	const OrderTypes eOrder	= (OrderTypes)lua_tointeger(L, 2);
	const int iData			= lua_tointeger(L, 3);
	const bool bAlt			= luaL_optbool(L, 4, 0);
	const bool bShift		= luaL_optbool(L, 5, 0);
	const bool bCtrl		= luaL_optbool(L, 6, 0);

	GetInstance()->cityPushOrder(pkCity, eOrder, iData, bAlt, bShift, bCtrl);
	return 0;
}
//------------------------------------------------------------------------------
// void cityPurchase(CyCity* pCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectTypes);
int CvLuaGame::lCityPurchaseUnit(lua_State* L)
{
	CvCity* pkCity = CvLuaCity::GetInstance(L, 1);
	const UnitTypes eUnitType = (UnitTypes)lua_tointeger(L, 2);
	const YieldTypes ePurchaseYield = (YieldTypes)lua_tointeger(L, 3);

	GetInstance()->CityPurchase(pkCity, eUnitType, NO_BUILDING, NO_PROJECT, ePurchaseYield);
	return 0;
}
//------------------------------------------------------------------------------
// void cityPurchase(CyCity* pCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectTypes);
int CvLuaGame::lCityPurchaseBuilding(lua_State* L)
{
	CvCity* pkCity = CvLuaCity::GetInstance(L, 1);
	const BuildingTypes eBuildingType = (BuildingTypes)lua_tointeger(L, 2);
	const YieldTypes ePurchaseYield = (YieldTypes)lua_tointeger(L, 3);

	GetInstance()->CityPurchase(pkCity, NO_UNIT, eBuildingType, NO_PROJECT, ePurchaseYield);
	return 0;
}
//------------------------------------------------------------------------------
// void cityPurchase(CyCity* pCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectTypes);
int CvLuaGame::lCityPurchaseProject(lua_State* L)
{
	CvCity* pkCity = CvLuaCity::GetInstance(L, 1);
	const ProjectTypes eProjectType = (ProjectTypes)lua_tointeger(L, 2);
	const YieldTypes ePurchaseYield = (YieldTypes)lua_tointeger(L, 3);

	GetInstance()->CityPurchase(pkCity, NO_UNIT, NO_BUILDING, eProjectType, ePurchaseYield);
	return 0;
}
//------------------------------------------------------------------------------
// int getProductionPerPopulation(HurryTypes eHurry);
int CvLuaGame::lGetProductionPerPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getProductionPerPopulation);
}
//------------------------------------------------------------------------------
// int getAdjustedPopulationPercent(VictoryTypes eVictory);
int CvLuaGame::lGetAdjustedPopulationPercent(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getAdjustedPopulationPercent);
}
//------------------------------------------------------------------------------
// int getAdjustedLandPercent(VictoryTypes eVictory);
int CvLuaGame::lGetAdjustedLandPercent(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getAdjustedLandPercent);
}
//------------------------------------------------------------------------------
// bool GetUnitedNationsCountdown() const;
int CvLuaGame::lGetUnitedNationsCountdown(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetUnitedNationsCountdown);
}
//------------------------------------------------------------------------------
//void SetUnitedNationsCountdown(int iNewValue);
int CvLuaGame::lSetUnitedNationsCountdown(lua_State* L)
{
	int iTurns = luaL_checkint(L, 1);
	GC.getGame().SetUnitedNationsCountdown(iTurns);
	return 0;
}
//------------------------------------------------------------------------------
// int CountCivPlayersAlive();
int CvLuaGame::lCountCivPlayersAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countCivPlayersAlive);
}
//------------------------------------------------------------------------------
// int CountCivPlayersEverAlive();
int CvLuaGame::lCountCivPlayersEverAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countCivPlayersEverAlive);
}
//------------------------------------------------------------------------------
// int CountCivTeamsAlive();
int CvLuaGame::lCountCivTeamsAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countCivTeamsAlive);
}
//------------------------------------------------------------------------------
// int CountCivTeamsEverAlive();
int CvLuaGame::lCountCivTeamsEverAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countCivTeamsEverAlive);
}
//------------------------------------------------------------------------------
//int countHumanPlayersAlive();
int CvLuaGame::lCountHumanPlayersAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countHumanPlayersAlive);
}
//------------------------------------------------------------------------------
//int countHumanPlayersEverAlive();
int CvLuaGame::lCountHumanPlayersEverAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countHumanPlayersEverAlive);
}
//------------------------------------------------------------------------------
//int countTotalCivPower();
int CvLuaGame::lCountTotalCivPower(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countTotalCivPower);
}
//------------------------------------------------------------------------------
//int countTotalNukeUnits();
int CvLuaGame::lCountTotalNukeUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countTotalNukeUnits);
}
//------------------------------------------------------------------------------
//int countKnownTechNumTeams(TechTypes eTech);
int CvLuaGame::lCountKnownTechNumTeams(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countKnownTechNumTeams);
}
//------------------------------------------------------------------------------
//int goldenAgeLength();
int CvLuaGame::lGoldenAgeLength(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::goldenAgeLength);
}
//------------------------------------------------------------------------------
//int victoryDelay(VictoryTypes eVictory);
int CvLuaGame::lVictoryDelay(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::victoryDelay);
}
//------------------------------------------------------------------------------
//int getImprovementUpgradeTime(ImprovementTypes eImprovement, CyPlot* pPlot);
int CvLuaGame::lGetImprovementUpgradeTime(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getImprovementUpgradeTime);
}
//------------------------------------------------------------------------------
//bool canTrainNukes();
int CvLuaGame::lCanTrainNukes(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::canTrainNukes);
}
//------------------------------------------------------------------------------
//EraTypes getCurrentEra();
int CvLuaGame::lGetCurrentEra(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getCurrentEra);
}
//------------------------------------------------------------------------------
//string GetDiploResponse(leaderType, responseType)
int CvLuaGame::lGetDiploResponse(lua_State* L)
{
	CvGame& game = GC.getGame();
	const char* szLeaderType = luaL_checkstring(L, 1);
	const char* szResponseType = luaL_checkstring(L, 2);

	Localization::String response
		= game.GetDiploResponse(szLeaderType, szResponseType, Localization::String::Empty, Localization::String::Empty);

	lua_pushstring(L, response.toUTF8());

	return 1;
}
//------------------------------------------------------------------------------
//int getActiveTeam();
int CvLuaGame::lGetActiveTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getActiveTeam);
}
//------------------------------------------------------------------------------
//CivilizationTypes getActiveCivilizationType();
int CvLuaGame::lGetActiveCivilizationType(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getActiveCivilizationType);
}
//------------------------------------------------------------------------------
//bool isNetworkMultiPlayer();
int CvLuaGame::lIsNetworkMultiPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isNetworkMultiPlayer);
}
//------------------------------------------------------------------------------
//bool isGameMultiPlayer();
int CvLuaGame::lIsGameMultiPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isGameMultiPlayer);
}
//------------------------------------------------------------------------------
//bool isTeamGame();
int CvLuaGame::lIsTeamGame(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isTeamGame);
}
//------------------------------------------------------------------------------
//void ReviveActivePlayer();
int CvLuaGame::lReviveActivePlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::ReviveActivePlayer);
}
//------------------------------------------------------------------------------
//int getNumHumanPlayers();
int CvLuaGame::lGetNumHumanPlayers(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNumHumanPlayers);
}
//------------------------------------------------------------------------------
//int getNumSequentialHumans();
int CvLuaGame::lGetNumSequentialHumans(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNumSequentialHumans);
}
//------------------------------------------------------------------------------
//int getGameTurn();
int CvLuaGame::lGetGameTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getGameTurn);
}
//------------------------------------------------------------------------------
//void setGameTurn(int iNewValue);
int CvLuaGame::lSetGameTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setGameTurn);
}
//------------------------------------------------------------------------------
//int getTurnYear(int iGameTurn);
int CvLuaGame::lGetTurnYear(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getTurnYear);
}
//------------------------------------------------------------------------------
//int getGameTurnYear();
int CvLuaGame::lGetGameTurnYear(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getGameTurnYear);
}
//------------------------------------------------------------------------------
//string GetTurnString();
int CvLuaGame::lGetTurnString(lua_State* L)
{
	CvGame& game = GC.getGame();
	CvString strString;
	CvGameTextMgr::setDateStr(strString,
		game.getGameTurn(),
		false,
		game.getCalendar(),
		game.getStartYear(),
		game.getGameSpeedType());

	lua_pushstring(L, strString);
	return 1;
}
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
int CvLuaGame::lGetDateString(lua_State* L)
{
    const int iTurn = lua_tointeger(L, 1);
    
    CvGame& game = GC.getGame();
    CvString strString;
    CvGameTextMgr::setDateStr(strString, iTurn, false, game.getCalendar(), game.getStartYear(), game.getGameSpeedType());
 
    lua_pushstring(L, strString);
    return 1;
}
#endif
//------------------------------------------------------------------------------
//int getElapsedGameTurns();
int CvLuaGame::lGetElapsedGameTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getElapsedGameTurns);
}
//------------------------------------------------------------------------------
//int getMaxTurns();
int CvLuaGame::lGetMaxTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getMaxTurns);
}
//------------------------------------------------------------------------------
//void setMaxTurns(int iNewValue);
int CvLuaGame::lSetMaxTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setMaxTurns);
}
//------------------------------------------------------------------------------
//void changeMaxTurns(int iChange);
int CvLuaGame::lChangeMaxTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::changeMaxTurns);
}
//------------------------------------------------------------------------------
//int getMaxCityElimination();
int CvLuaGame::lGetMaxCityElimination(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getMaxCityElimination);
}
//------------------------------------------------------------------------------
//void setMaxCityElimination(int iNewValue);
int CvLuaGame::lSetMaxCityElimination(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setMaxCityElimination);
}
//------------------------------------------------------------------------------
//int getNumAdvancedStartPoints();
int CvLuaGame::lGetNumAdvancedStartPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNumAdvancedStartPoints);
}
//------------------------------------------------------------------------------
//void setNumAdvancedStartPoints(int iNewValue);
int CvLuaGame::lSetNumAdvancedStartPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setNumAdvancedStartPoints);
}
//------------------------------------------------------------------------------
//int getStartTurn();
int CvLuaGame::lGetStartTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getStartTurn);
}
//------------------------------------------------------------------------------
//int GetWinningTurn();
int CvLuaGame::lGetWinningTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetWinningTurn);
}
//------------------------------------------------------------------------------
//int getStartYear();
int CvLuaGame::lGetStartYear(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getStartYear);
}
//------------------------------------------------------------------------------
//void setStartYear(int iNewValue);
int CvLuaGame::lSetStartYear(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setStartYear);
}
//------------------------------------------------------------------------------
//int getEstimateEndTurn();
int CvLuaGame::lGetEstimateEndTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getEstimateEndTurn);
}
//------------------------------------------------------------------------------
//void setEstimateEndTurn(int iNewValue);
int CvLuaGame::lSetEstimateEndTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setEstimateEndTurn);
}
//------------------------------------------------------------------------------
//int getTurnSlice();
int CvLuaGame::lGetTurnSlice(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getTurnSlice);
}
//------------------------------------------------------------------------------
//int getMinutesPlayed();
int CvLuaGame::lGetMinutesPlayed(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getMinutesPlayed);
}
//------------------------------------------------------------------------------
//int getTargetScore();
int CvLuaGame::lGetTargetScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getTargetScore);
}
//------------------------------------------------------------------------------
//void setTargetScore(int iNewValue);
int CvLuaGame::lSetTargetScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setTargetScore);
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsStaticTutorialActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::IsStaticTutorialActive);
}
//------------------------------------------------------------------------------
int CvLuaGame::lSetStaticTutorialActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::SetStaticTutorialActive);
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsEverRightClickMoved(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::IsEverRightClickMoved);
}
//------------------------------------------------------------------------------
int CvLuaGame::lSetEverRightClickMoved(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::SetEverRightClickMoved);
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsTutorialLogging(lua_State* L)
{
	lua_pushboolean(L, GC.GetTutorialLogging());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsTutorialDebugging(lua_State* L)
{
	lua_pushboolean(L, GC.GetTutorialDebugging());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetTutorialLevel(lua_State* L)
{
	lua_pushinteger(L, gDLL->GetTutorialLevel());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lHasAdvisorMessageBeenSeen(lua_State* L)
{
	const char* szAdvisorMessage = luaL_checkstring(L, 1);
	const bool bResult = GC.getGame().HasAdvisorMessageBeenSeen(szAdvisorMessage);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lSetAdvisorMessageHasBeenSeen(lua_State* L)
{
	const char* szAdvisorMessage = luaL_checkstring(L, 1);
	const bool bSeen = lua_toboolean(L, 2);
	GC.getGame().SetAdvisorMessageHasBeenSeen(szAdvisorMessage, bSeen);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lSetAdvisorBadAttackInterrupt(lua_State* L)
{
	bool bValue = lua_toboolean(L, 1);
	gDLL->SetAdvisorBadAttackInterrupt(bValue);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lSetAdvisorCityAttackInterrupt(lua_State* L)
{
	bool bValue = lua_toboolean(L, 1);
	gDLL->SetAdvisorCityAttackInterrupt(bValue);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetAllowRClickMovementWhileScrolling(lua_State* L)
{
	lua_pushboolean(L, GC.GetAllowRClickMovementWhileScrolling());
	return 1;
}

//------------------------------------------------------------------------------
//int getNumGameTurnActive();
int CvLuaGame::lGetNumGameTurnActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNumGameTurnActive);
}
//------------------------------------------------------------------------------
//int countNumHumanGameTurnActive();
int CvLuaGame::lCountNumHumanGameTurnActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::countNumHumanGameTurnActive);
}
//------------------------------------------------------------------------------
//int getNumCities();
int CvLuaGame::lGetNumCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNumCities);
}
//------------------------------------------------------------------------------
//int getNumCivCities();
int CvLuaGame::lGetNumCivCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNumCivCities);
}
//------------------------------------------------------------------------------
//int getTotalPopulation();
int CvLuaGame::lGetTotalPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getTotalPopulation);
}
//------------------------------------------------------------------------------
//int getNoNukesCount();
int CvLuaGame::lGetNoNukesCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNoNukesCount);
}
//------------------------------------------------------------------------------
//bool isNoNukes();
int CvLuaGame::lIsNoNukes(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isNoNukes);
}
//------------------------------------------------------------------------------
//void changeNoNukesCount(int iChange);
int CvLuaGame::lChangeNoNukesCount(lua_State* L)
{
#if defined(MOD_BUGFIX_LUA_API)
	int iNumNukes;
	if (lua_gettop(L) == 1)
		iNumNukes = lua_tointeger(L, 1); // The correct Game.ChangeNoNukesCount() usage
	else
		iNumNukes = lua_tointeger(L, 2); // The incorrect (but as used by the game core .lua files) Game:ChangeNoNukesCount() usage
#else
	int iNumNukes = lua_tointeger(L, 2);
#endif
	GC.getGame().changeNoNukesCount(iNumNukes);
	return 1;
}
//------------------------------------------------------------------------------
//int getNukesExploded();
int CvLuaGame::lGetNukesExploded(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNukesExploded);
}
//------------------------------------------------------------------------------
//void changeNukesExploded(int iChange);
int CvLuaGame::lChangeNukesExploded(lua_State* L)
{
#if defined(MOD_BUGFIX_LUA_API)
	int iNumNukes;
	if (lua_gettop(L) == 1)
		iNumNukes = lua_tointeger(L, 1); // The correct Game.ChangeNukesExploded() usage
	else
		iNumNukes = lua_tointeger(L, 2); // The incorrect (but as used by the game core .lua files) Game:ChangeNukesExploded() usage
#else
	int iNumNukes = lua_tointeger(L, 2);
#endif
	GC.getGame().changeNukesExploded(iNumNukes);
	return 1;
}
//------------------------------------------------------------------------------
//int getMaxPopulation();
int CvLuaGame::lGetMaxPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getMaxPopulation);
}
//------------------------------------------------------------------------------
//int getInitPopulation();
int CvLuaGame::lGetInitPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getInitPopulation);
}
//------------------------------------------------------------------------------
//int getInitLand();
int CvLuaGame::lGetInitLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getInitLand);
}
//------------------------------------------------------------------------------
//int getInitTech();
int CvLuaGame::lGetInitTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getInitTech);
}
//------------------------------------------------------------------------------
//int getInitWonders();
int CvLuaGame::lGetInitWonders(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getInitWonders);
}
//------------------------------------------------------------------------------
//int GetNumWorldWonders();
int CvLuaGame::lGetNumWorldWonders(lua_State* L)
{
	int iWonderCount = 0;

	// Loop through all players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);

		// Loop through all buildings, see if they're a world wonder
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				if(::isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
				{
					iWonderCount += kLoopPlayer.countNumBuildings(eBuilding);
				}
			}
		}
	}

	lua_pushinteger(L, iWonderCount);
	return 1;
}
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//bool isWorldWonderClass(const CvBuildingClassInfo& kBuildingClass);
int CvLuaGame::lIsWorldWonderClass(lua_State* L)
{
	const BuildingClassTypes eBuildingClass = (BuildingClassTypes) lua_tointeger(L, 1);
	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
	const bool bResult = ::isWorldWonderClass(*pkBuildingClassInfo);
	lua_pushboolean(L, bResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int getAIAutoPlay();
int CvLuaGame::lGetAIAutoPlay(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getAIAutoPlay);
}
//------------------------------------------------------------------------------
//void setAIAutoPlay(int iNewValue);
int CvLuaGame::lSetAIAutoPlay(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setAIAutoPlay);
}
//------------------------------------------------------------------------------
//bool isScoreDirty();
int CvLuaGame::lIsScoreDirty(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isScoreDirty);
}
//------------------------------------------------------------------------------
//void setScoreDirty(bool bNewValue);
int CvLuaGame::lSetScoreDirty(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setScoreDirty);
}
//------------------------------------------------------------------------------
//bool isCircumnavigated();
int CvLuaGame::lIsCircumnavigated(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isCircumnavigated);
}
//------------------------------------------------------------------------------
//void makeCircumnavigated();
int CvLuaGame::lMakeCircumnavigated(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::makeCircumnavigated);
}
//------------------------------------------------------------------------------
//void DoFromUIDiploEvent(FromUIDiploEventTypes eEvent, PlayerTypes eAIPlayer, int iArg1, int iArg2, int iArg3);
int CvLuaGame::lDoFromUIDiploEvent(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::DoFromUIDiploEvent);
}
//------------------------------------------------------------------------------
//bool isDebugMode();
int CvLuaGame::lIsDebugMode(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isDebugMode);
}
//------------------------------------------------------------------------------
//bool setDebugMode();
int CvLuaGame::lSetDebugMode(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setDebugMode);
}
//------------------------------------------------------------------------------
//void toggleDebugMode();
int CvLuaGame::lToggleDebugMode(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::toggleDebugMode);
}
//------------------------------------------------------------------------------
//void UpdateFOW();
int CvLuaGame::lUpdateFOW(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setFOW);
}
//------------------------------------------------------------------------------
//int getPitbossTurnTime();
int CvLuaGame::lGetPitbossTurnTime(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getPitbossTurnTime);
}
//------------------------------------------------------------------------------
//void setPitbossTurnTime(int iHours);
int CvLuaGame::lSetPitbossTurnTime(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setPitbossTurnTime);
}
//------------------------------------------------------------------------------
//bool isHotSeat();
int CvLuaGame::lIsHotSeat(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isHotSeat);
}
//------------------------------------------------------------------------------
//bool isPbem();
int CvLuaGame::lIsPbem(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isPbem);
}
//------------------------------------------------------------------------------
//bool isPitboss();
int CvLuaGame::lIsPitboss(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isPitboss);
}
//------------------------------------------------------------------------------
//bool isSimultaneousTeamTurns();
int CvLuaGame::lIsSimultaneousTeamTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isSimultaneousTeamTurns);
}
//------------------------------------------------------------------------------
//bool isFinalInitialized();
int CvLuaGame::lIsFinalInitialized(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isFinalInitialized);
}
//------------------------------------------------------------------------------
//PlayerTypes getActivePlayer();
int CvLuaGame::lGetActivePlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getActivePlayer);
}
//------------------------------------------------------------------------------
//void setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat);
int CvLuaGame::lSetActivePlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setActivePlayer);
}
//------------------------------------------------------------------------------
//int getPausePlayer();
int CvLuaGame::lGetPausePlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getPausePlayer);
}
//------------------------------------------------------------------------------
int CvLuaGame::lSetPausePlayer(lua_State* L)
{
	CvGame& game = GC.getGame();
	if(!game.isNetworkMultiPlayer())
	{
		int iPlayer = lua_tointeger(L, 1);
		game.setPausePlayer((PlayerTypes)iPlayer);
	}

	return 1;
}
//------------------------------------------------------------------------------
//bool isPaused();
int CvLuaGame::lIsPaused(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isPaused);
}
//------------------------------------------------------------------------------
//UnitTypes getBestLandUnit();
int CvLuaGame::lGetBestLandUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getBestLandUnit);
}
//------------------------------------------------------------------------------
//int getBestLandUnitCombat();
int CvLuaGame::lGetBestLandUnitCombat(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getBestLandUnitCombat);
}
//------------------------------------------------------------------------------
//int GetFaithCost();
int CvLuaGame::lGetFaithCost(lua_State* L)
{
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 1);
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	const int iResult = GC.getGame().GetFaithCost(pkUnitInfo);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//TeamTypes getWinner();
int CvLuaGame::lGetWinner(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getWinner);
}
//------------------------------------------------------------------------------
//VictoryTypes getVictory();
int CvLuaGame::lGetVictory(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getVictory);
}
//------------------------------------------------------------------------------
//void setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory);
int CvLuaGame::lSetWinner(lua_State* L)
{
	CUSTOMLOG("Calling setWinner from Lua: %i, %i", lua_tointeger(L, 1), lua_tointeger(L, 2));
	return BasicLuaMethod(L, &CvGame::setWinner);
}
//------------------------------------------------------------------------------
//GameStateTypes getGameState();
int CvLuaGame::lGetGameState(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getGameState);
}
//------------------------------------------------------------------------------
//void SetGameState(GameplayGameStateTypes gameState);
int CvLuaGame::lSetGameState(lua_State* L)
{
	int iGameState = luaL_checkint(L, 1);
	GameStateTypes eGameState = static_cast<GameStateTypes>(iGameState);

	GC.getGame().setGameState(eGameState);
	return 0;
}
//------------------------------------------------------------------------------
//int GetVotesNeededForDiploVictory();
int CvLuaGame::lGetVotesNeededForDiploVictory(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetVotesNeededForDiploVictory);
}
//------------------------------------------------------------------------------
//bool IsUnitedNationsActive();
int CvLuaGame::lIsUnitedNationsActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::IsUnitedNationsActive);
}
//------------------------------------------------------------------------------
//int GetNumVictoryVotesTallied();
int CvLuaGame::lGetNumVictoryVotesTallied(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetNumVictoryVotesTallied);
}
//------------------------------------------------------------------------------
//TeamTypes GetVoteCast();
int CvLuaGame::lGetVoteCast(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetVoteCast);
}
//------------------------------------------------------------------------------
//TeamTypes GetPreviousVoteCast();
int CvLuaGame::lGetPreviousVoteCast(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetPreviousVoteCast);
}
//------------------------------------------------------------------------------
//int GetNumVotesForTeam(TeamTypes eTeam);
int CvLuaGame::lGetNumVotesForTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetNumVotesForTeam);
}
//------------------------------------------------------------------------------
//int SetNumVotesForTeam(TeamTypes eTeam, int iValue);
int CvLuaGame::lSetNumVotesForTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::SetNumVotesForTeam);
}
//------------------------------------------------------------------------------
//int ChangeNumVotesForTeam(TeamTypes eTeam, int iChange);
int CvLuaGame::lChangeNumVotesForTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::ChangeNumVotesForTeam);
}
//------------------------------------------------------------------------------
//HandicapTypes getHandicapType();
int CvLuaGame::lGetHandicapType(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getHandicapType);
}
//------------------------------------------------------------------------------
//CalendarTypes getCalendar();
int CvLuaGame::lGetCalendar(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getCalendar);
}
//------------------------------------------------------------------------------
//EraTypes getStartEra();
int CvLuaGame::lGetStartEra(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getStartEra);
}
//------------------------------------------------------------------------------
//GameSpeedTypes getGameSpeedType();
int CvLuaGame::lGetGameSpeedType(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getGameSpeedType);
}
//------------------------------------------------------------------------------
//PlayerTypes getRankPlayer(int iRank);
int CvLuaGame::lGetRankPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getRankPlayer);
}
//------------------------------------------------------------------------------
//int getPlayerRank(PlayerTypes iIndex);
int CvLuaGame::lGetPlayerRank(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getPlayerRank);
}
//------------------------------------------------------------------------------
//int getPlayerScore(PlayerTypes iIndex);
int CvLuaGame::lGetPlayerScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getPlayerScore);
}
//------------------------------------------------------------------------------
//TeamTypes getRankTeam(int iRank);
int CvLuaGame::lGetRankTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getRankTeam);
}
//------------------------------------------------------------------------------
//int getTeamRank(TeamTypes iIndex);
int CvLuaGame::lGetTeamRank(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getTeamRank);
}
//------------------------------------------------------------------------------
//int getTeamScore(TeamTypes iIndex);
int CvLuaGame::lGetTeamScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getTeamScore);
}
//------------------------------------------------------------------------------
//bool isOption(GameOptionTypes eIndex);
//bool isOption(const char* pszOption);
int CvLuaGame::lIsOption(lua_State* L)
{
	CvGame& kGame = GC.getGame();
	bool bResult = false;
	if(lua_isnumber(L, 1))
		bResult = kGame.isOption((GameOptionTypes)lua_tointeger(L, 1));
	else
		bResult = kGame.isOption(lua_tostring(L, 1));

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setOption(GameOptionTypes eIndex, bool bEnabled);
//void setOption(const char *pszOption, bool bEnabled);
int CvLuaGame::lSetOption(lua_State* L)
{
	CvGame& kGame = GC.getGame();
	if(lua_isnumber(L, 1))
		kGame.setOption((GameOptionTypes)lua_tointeger(L, 1), lua_toboolean(L, 2));
	else
		kGame.setOption(lua_tostring(L, 1), lua_toboolean(L, 2));

	return 0;
}
//------------------------------------------------------------------------------
//bool isMPOption(MultiplayerOptionTypes eIndex);
int CvLuaGame::lIsMPOption(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isMPOption);
}
//------------------------------------------------------------------------------
//int getUnitCreatedCount(UnitTypes eIndex);
int CvLuaGame::lGetUnitCreatedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getUnitCreatedCount);
}
//------------------------------------------------------------------------------
//int getUnitClassCreatedCount(UnitClassTypes eIndex);
int CvLuaGame::lGetUnitClassCreatedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getUnitClassCreatedCount);
}
//------------------------------------------------------------------------------
//bool isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra);
int CvLuaGame::lIsUnitClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isUnitClassMaxedOut);
}
//------------------------------------------------------------------------------
//int getBuildingClassCreatedCount(BuildingClassTypes eIndex);
int CvLuaGame::lGetBuildingClassCreatedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getBuildingClassCreatedCount);
}
//------------------------------------------------------------------------------
//bool isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra);
int CvLuaGame::lIsBuildingClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isBuildingClassMaxedOut);
}
//------------------------------------------------------------------------------
//int getProjectCreatedCount(ProjectTypes eIndex);
int CvLuaGame::lGetProjectCreatedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getProjectCreatedCount);
}
//------------------------------------------------------------------------------
//bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra);
int CvLuaGame::lIsProjectMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isProjectMaxedOut);
}
//------------------------------------------------------------------------------
//bool isVictoryValid(VictoryTypes eIndex);
int CvLuaGame::lIsVictoryValid(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isVictoryValid);
}
//------------------------------------------------------------------------------
//bool setVictoryValid(VictoryTypes eIndex, bool bValid);
int CvLuaGame::lSetVictoryValid(lua_State* L)
{
	CvGame& game = GC.getGame();
	VictoryTypes eVictoryType = (VictoryTypes)lua_tointeger(L, 1);
	bool bValid = lua_toboolean(L, 2);
	game.setVictoryValid(eVictoryType, bValid);

	return true;
}
//------------------------------------------------------------------------------
//bool isSpecialUnitValid(SpecialUnitTypes eSpecialUnitType);
int CvLuaGame::lIsSpecialUnitValid(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isSpecialUnitValid);
}
//------------------------------------------------------------------------------
//void makeSpecialUnitValid(SpecialUnitTypes eSpecialUnitType);
int CvLuaGame::lMakeSpecialUnitValid(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::makeSpecialUnitValid);
}
//------------------------------------------------------------------------------
//bool isNukesValid();
int CvLuaGame::lIsNukesValid(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isNukesValid);
}
//------------------------------------------------------------------------------
//void makeNukesValid(bool bValid);
int CvLuaGame::lMakeNukesValid(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::makeNukesValid);
}
//------------------------------------------------------------------------------
//bool isInAdvancedStart();
int CvLuaGame::lIsInAdvancedStart(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isInAdvancedStart);
}
//------------------------------------------------------------------------------
//void setName(char* szName);
int CvLuaGame::lSetName(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setName);
}
//------------------------------------------------------------------------------
//wstring getName();
int CvLuaGame::lGetName(lua_State* L)
{
	luaL_error(L, "NYI");
	return 0;
}
//------------------------------------------------------------------------------
//int Rand(max_num, log);
int CvLuaGame::lRand(lua_State* L)
{
	const int max_num = luaL_checkinteger(L, 1);
	const char* strLog = luaL_checkstring(L, 2);
	const int rand_val = GetInstance()->getJonRandNum(max_num, strLog);

	lua_pushinteger(L, rand_val);
	return 1;
}
//------------------------------------------------------------------------------
//int calculateSyncChecksum();
int CvLuaGame::lCalculateSyncChecksum(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::calculateSyncChecksum);
}
//------------------------------------------------------------------------------
//int calculateOptionsChecksum();
int CvLuaGame::lCalculateOptionsChecksum(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::calculateOptionsChecksum);
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetReplayMessage(lua_State* L)
{
	CvGame& game = GC.getGame();
	const size_t nReplayMessages = game.getNumReplayMessages();

	unsigned int idx = luaL_checkint(L, 1);
	if(idx < nReplayMessages)
	{
		const CvReplayMessage* pMessage = game.getReplayMessage(idx);

		lua_createtable(L, 0, 5);
		const int t = lua_gettop(L);

		lua_pushinteger(L, pMessage->getPlayer());
		lua_setfield(L, t, "Player");

		lua_pushinteger(L, pMessage->getTurn());
		lua_setfield(L, t, "Turn");

		lua_pushinteger(L, pMessage->getType());
		lua_setfield(L, t, "Type");

		const CvString& text = pMessage->getText();
		if(text.GetLength() > 0)
		{
			lua_pushstring(L, text.c_str());
			lua_setfield(L, t, "Text");
		}

		const unsigned int nPlots = pMessage->getNumPlots();
		if(nPlots > 0)
		{
			lua_createtable(L, nPlots, 0);
			int plots_idx = 1;
			int plots_t = lua_gettop(L);

			for(unsigned int i = 0; i < nPlots; i++)
			{
				int iPlotX, iPlotY;
				if(pMessage->getPlot(i, iPlotX, iPlotY))
				{
					lua_createtable(L, 0, 2);
					lua_pushinteger(L, iPlotX);
					lua_setfield(L, -2, "X");
					lua_pushinteger(L, iPlotY);
					lua_setfield(L, -2, "Y");
					lua_rawseti(L, plots_t, plots_idx++);
				}
			}

			lua_setfield(L, t, "Plots");
		}

		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetReplayMessages(lua_State* L)
{
	CvGame& game = GC.getGame();
	const unsigned int nMessages = game.getNumReplayMessages();

	lua_createtable(L, nMessages, 0);
	const int messages_t = lua_gettop(L);
	int messages_idx = 1;

	for(unsigned int i = 0; i < nMessages; i++)
	{
		const CvReplayMessage* pMessage = game.getReplayMessage(i);

		lua_createtable(L, 0, 5);
		const int t = lua_gettop(L);

		lua_pushinteger(L, pMessage->getPlayer());
		lua_setfield(L, t, "Player");

		lua_pushinteger(L, pMessage->getTurn());
		lua_setfield(L, t, "Turn");

		lua_pushinteger(L, pMessage->getType());
		lua_setfield(L, t, "Type");

		const CvString& text = pMessage->getText();
		if(text.GetLength() > 0)
		{
			lua_pushstring(L, text.c_str());
			lua_setfield(L, t, "Text");
		}

		const unsigned int nPlots = pMessage->getNumPlots();
		if(nPlots > 0)
		{
			lua_createtable(L, nPlots, 0);
			int idx = 1;
			int plots_idx = lua_gettop(L);

			for(unsigned int j = 0; j < nPlots; j++)
			{
				int iPlotX, iPlotY;
				if(pMessage->getPlot(j, iPlotX, iPlotY))
				{
					lua_createtable(L, 0, 2);
					lua_pushinteger(L, iPlotX);
					lua_setfield(L, -2, "X");
					lua_pushinteger(L, iPlotY);
					lua_setfield(L, -2, "Y");
					lua_rawseti(L, plots_idx, idx++);
				}
			}

			lua_setfield(L, t, "Plots");
		}

		lua_rawseti(L, messages_t, messages_idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
//uint getNumReplayMessages();
int CvLuaGame::lGetNumReplayMessages(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getNumReplayMessages);
}
//------------------------------------------------------------------------------
//CyReplayInfo* getReplayInfo();
int CvLuaGame::lGetReplayInfo(lua_State* L)
{
	luaL_error(L, "NYI");
	return 0;
}
//------------------------------------------------------------------------------
//void saveReplay(int iPlayer);
int CvLuaGame::lSaveReplay(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::saveReplay);
}
//------------------------------------------------------------------------------
//void addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv);
int CvLuaGame::lAddPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::addPlayer);
}
//------------------------------------------------------------------------------
//void setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield);
int CvLuaGame::lSetPlotExtraYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::setPlotExtraYield);
}
//------------------------------------------------------------------------------
//void changePlotExtraCost(int iX, int iY, int iExtraCost);
int CvLuaGame::lChangePlotExtraCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::changePlotExtraCost);
}
//------------------------------------------------------------------------------
//bool isCivEverActive(CivilizationTypes eCivilization);
int CvLuaGame::lIsCivEverActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isCivEverActive);
}
//------------------------------------------------------------------------------
//bool isLeaderEverActive(LeaderHeadTypes eLeader);
int CvLuaGame::lIsLeaderEverActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isLeaderEverActive);
}
//------------------------------------------------------------------------------
//bool isUnitEverActive(UnitTypes eUnit);
int CvLuaGame::lIsUnitEverActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isUnitEverActive);
}
//------------------------------------------------------------------------------
//bool isBuildingEverActive(BuildingTypes eBuilding);
int CvLuaGame::lIsBuildingEverActive(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::isBuildingEverActive);
}
//------------------------------------------------------------------------------
//bool CanDoControl(ControlTypes eControl)
int CvLuaGame::lCanDoControl(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::canDoControl);
}
//------------------------------------------------------------------------------
//void DoControl(ControlTypes eControl)
int CvLuaGame::lDoControl(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::doControl);
}
//------------------------------------------------------------------------------
//void DoMinorPledgeProtection(int iMajorCivID, int iMinorCivID, bool bProtect);
int CvLuaGame::lDoMinorPledgeProtection(lua_State* L)
{
	const int iMajor = lua_tointeger(L, 1);
	const int iMinor = lua_tointeger(L, 2);
	const bool bProtect = lua_toboolean(L, 3);
	GC.getGame().DoMinorPledgeProtection((PlayerTypes)iMajor, (PlayerTypes)iMinor, bProtect);

	return 1;
}
//------------------------------------------------------------------------------
//void DoMinorGoldGift(int iMinorCivID, int iGold);
// Old name, kept here for backwards compatibility
int CvLuaGame::lDoMinorGoldGift(lua_State* L)
{
	return lDoMinorGiftGold(L);
}
//------------------------------------------------------------------------------
//void DoMinorGiftGold(int iMinorCivID, iGold);
int CvLuaGame::lDoMinorGiftGold(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::DoMinorGiftGold);
}
//------------------------------------------------------------------------------
//void DoMinorGiftTileImprovement(int iMajorCivID, int iMinorCivID, iPlotX, iPlotY);
int CvLuaGame::lDoMinorGiftTileImprovement(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::DoMinorGiftTileImprovement);
}
//------------------------------------------------------------------------------
//void DoMinorBullyGold(int iBullyCivID, int iMinorCivID);
int CvLuaGame::lDoMinorBullyGold(lua_State* L)
{
	const int iBully = lua_tointeger(L, 1);
	const int iMinor = lua_tointeger(L, 2);
	GC.getGame().DoMinorBullyGold((PlayerTypes)iBully, (PlayerTypes)iMinor);

	return 1;
}
//------------------------------------------------------------------------------
//void DoMinorBullyUnit(int iBullyCivID, int iMinorCivID);
int CvLuaGame::lDoMinorBullyUnit(lua_State* L)
{
	const int iBully = lua_tointeger(L, 1);
	const int iMinor = lua_tointeger(L, 2);
	GC.getGame().DoMinorBullyUnit((PlayerTypes)iBully, (PlayerTypes)iMinor);

	return 1;
}
//------------------------------------------------------------------------------
//void DoMinorBuyout(int iMajorCivID, int iMinorCivID);
int CvLuaGame::lDoMinorBuyout(lua_State* L)
{
	const int iMajor = lua_tointeger(L, 1);
	const int iMinor = lua_tointeger(L, 2);
	GC.getGame().DoMinorBuyout((PlayerTypes)iMajor, (PlayerTypes)iMinor);

	return 1;
}
#if defined(MOD_BALANCE_CORE)
int CvLuaGame::lDoMinorMarriage(lua_State* L)
{
	const int iMajor = lua_tointeger(L, 1);
	const int iMinor = lua_tointeger(L, 2);
	GC.getGame().DoMinorMarriage((PlayerTypes)iMajor, (PlayerTypes)iMinor);

	return 1;
}
#endif
//------------------------------------------------------------------------------
//void GetBestWondersPlayer();
int CvLuaGame::lGetBestWondersPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetBestWondersPlayer);
}
//------------------------------------------------------------------------------
//void GetBestPoliciesPlayer();
int CvLuaGame::lGetBestPoliciesPlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetBestPoliciesPlayer);
}
//------------------------------------------------------------------------------
//void GetBestGreatPeoplePlayer();
int CvLuaGame::lGetBestGreatPeoplePlayer(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetBestGreatPeoplePlayer);
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsEverAttackedTutorial(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::IsEverAttackedTutorial);
}
//------------------------------------------------------------------------------
int CvLuaGame::lGameplayDiplomacyAILeaderMessage(lua_State* L)
{
	const int iPlayer = lua_tointeger(L, 1);
	const int eMessage = lua_tointeger(L, 2);
	const int iData1 = lua_tointeger(L, 3);
	gDLL->GameplayDiplomacyAILeaderMessage((PlayerTypes) iPlayer, DIPLO_UI_STATE_DEFAULT_ROOT, "TEMP", (LeaderheadAnimationTypes) eMessage, iData1);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetResourceUsageType(lua_State* L)
{
	const ResourceTypes eResource = (ResourceTypes) lua_tointeger(L, 1);
	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	if (pkResourceInfo)
	{
		ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();
		lua_pushinteger(L, eUsage);
	}
	else
		lua_pushinteger(L, -1);		// NO_USAGE

	return 1;
}
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumResourceTotalRequiredForUnit(lua_State* L)
{
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 1);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if (pkUnitInfo == NULL)
	{
		luaL_error(L, "Unit row at ID %d is empty.", eUnit);
		return 0;
	}
	const int iNumNeeded = pkUnitInfo->GetResourceQuantityTotal(eResource);

	lua_pushinteger(L, iNumNeeded);

	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumResourceRequiredForUnit(lua_State* L)
{
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 1);
	const ResourceTypes eResource = (ResourceTypes) lua_tointeger(L, 2);

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		luaL_error(L, "Unit row at ID %d is empty.", eUnit);
		return 0;
	}
	const int iNumNeeded = pkUnitInfo->GetResourceQuantityRequirement(eResource);

	lua_pushinteger(L, iNumNeeded);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumResourceRequiredForBuilding(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);
	const ResourceTypes eResource = (ResourceTypes) luaL_checkint(L, 2);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);

	int iNumNeeded = 0;
	if(pkBuildingInfo && pkResourceInfo)
	{
		iNumNeeded = pkBuildingInfo->GetResourceQuantityRequirement(eResource);
	}

	lua_pushinteger(L, iNumNeeded);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lIsCombatWarned(lua_State* L)
{
	lua_pushboolean(L, GC.getGame().IsCombatWarned());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lSetCombatWarned(lua_State* L)
{
	GC.getGame().SetCombatWarned(lua_toboolean(L, 1));
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetAdvisorCounsel(lua_State* L)
{
	const PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	GC.getGame().GetAdvisorCounsel()->BuildCounselList(ePlayer);
	lua_createtable(L, 0, 0);
	int iTopLevelLua = lua_gettop(L);

	AdvisorTypes eCurrentAdvisorType = NUM_ADVISOR_TYPES;
	int iAdvisorTableLevel = -1;
	int iAdvisorIndex = 0;
	bool bTableOpen = false;
	for(uint ui = 0; ui < GC.getGame().GetAdvisorCounsel()->m_aCounsel.size(); ui++)
	{
		if(GC.getGame().GetAdvisorCounsel()->m_aCounsel[ui].m_eAdvisor != eCurrentAdvisorType)
		{
			// close out previous table
			if(eCurrentAdvisorType != NUM_ADVISOR_TYPES)
			{
				CvAssertMsg(bTableOpen, "Table should be open");
				lua_rawseti(L, iTopLevelLua, eCurrentAdvisorType);
				bTableOpen = false;
			}

			AdvisorTypes eNextAdvisorType = GC.getGame().GetAdvisorCounsel()->m_aCounsel[ui].m_eAdvisor;
			if(eNextAdvisorType == NO_ADVISOR_TYPE)
			{
				// no more valid advisor info. Bail out!
				break;
			}
			eCurrentAdvisorType = eNextAdvisorType;
			CvAssertMsg(!bTableOpen, "table should be open");
			lua_createtable(L, 0, 0);
			bTableOpen = true;
			iAdvisorTableLevel = lua_gettop(L);
			iAdvisorIndex = 0;
		}

		CvAssertMsg(bTableOpen, "Table should be open");
		lua_pushstring(L, GC.getGame().GetAdvisorCounsel()->m_aCounsel[ui].m_strTxtKey);
		lua_rawseti(L, iAdvisorTableLevel, iAdvisorIndex);
		iAdvisorIndex++;
	}

	if(bTableOpen)
	{
		lua_rawseti(L, iTopLevelLua, eCurrentAdvisorType);
	}

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lSetAdvisorRecommenderCity(lua_State* L)
{
	CvCity* pkCity = CvLuaCity::GetInstance(L, 1);
	GC.getGame().GetAdvisorRecommender()->UpdateCityRecommendations(pkCity);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lIsUnitRecommended(lua_State* L)
{
	UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 1);
	AdvisorTypes eAdvisor = (AdvisorTypes)lua_tointeger(L, 2);
	bool bResult = GC.getGame().GetAdvisorRecommender()->IsUnitRecommended(eUnit, eAdvisor);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lIsBuildingRecommended(lua_State* L)
{
	BuildingTypes eBuilding = (BuildingTypes)lua_tointeger(L, 1);
	AdvisorTypes eAdvisor = (AdvisorTypes)lua_tointeger(L, 2);
	bool bResult = GC.getGame().GetAdvisorRecommender()->IsBuildingRecommended(eBuilding, eAdvisor);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lIsProjectRecommended(lua_State* L)
{
	ProjectTypes eProject = (ProjectTypes)lua_tointeger(L, 1);
	AdvisorTypes eAdvisor = (AdvisorTypes)lua_tointeger(L, 2);
	bool bResult = GC.getGame().GetAdvisorRecommender()->IsProjectRecommended(eProject, eAdvisor);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lSetAdvisorRecommenderTech(lua_State* L)
{
	PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 1);
	GC.getGame().GetAdvisorRecommender()->UpdateTechRecommendations(ePlayer);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lIsTechRecommended(lua_State* L)
{
	TechTypes eTech = (TechTypes)lua_tointeger(L, 1);
	AdvisorTypes eAdvisor = (AdvisorTypes)lua_tointeger(L, 2);
	bool bResult = GC.getGame().GetAdvisorRecommender()->IsTechRecommended(eTech, eAdvisor);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetVariableCitySizeFromPopulation(lua_State* L)
{
	unsigned int nPopulation = (unsigned int)lua_tointeger(L, 1);
	unsigned int nSize = GC.getGame().GetVariableCitySizeFromPopulation(nPopulation);
	lua_pushinteger(L, nSize);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetResearchAgreementCost(lua_State* L)
{
	const PlayerTypes ePlayer1 = (PlayerTypes) lua_tointeger(L, 1);
	const PlayerTypes ePlayer2 = (PlayerTypes) lua_tointeger(L, 2);
	int iCost = GC.getGame().GetResearchAgreementCost(ePlayer1, ePlayer2);
	lua_pushinteger(L, iCost);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetCustomOption(lua_State* L)
{
	const char* szOptionName = luaL_checkstring(L, 1);
	int i = 0;
	if(CvPreGame::GetGameOption(szOptionName, i))
	{
		lua_pushinteger(L, i);
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumCitiesPolicyCostMod(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().getWorldInfo().GetNumCitiesPolicyCostMod());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumCitiesTourismCostMod(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().getWorldInfo().GetNumCitiesTourismCostMod());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumCitiesTechCostMod(lua_State* L)
{
	const PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	int iNormalValue = GC.getMap().getWorldInfo().GetNumCitiesTechCostMod();
	if (ePlayer != NO_PLAYER)
	{
		iNormalValue += GET_PLAYER(ePlayer).GetTechCostXCitiesModifier();
	}

	lua_pushinteger(L, iNormalValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetBuildingYieldChange(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);
	const YieldTypes eYield = (YieldTypes) luaL_checkint(L, 2);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvYieldInfo* pkYieldInfo = GC.getYieldInfo(eYield);

	int iYieldChange = 0;
	if(pkBuildingInfo && pkYieldInfo)
	{
		iYieldChange = pkBuildingInfo->GetYieldChange(eYield);
	}

	lua_pushinteger(L, iYieldChange);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetBuildingYieldModifier(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);
	const YieldTypes eYield = (YieldTypes) luaL_checkint(L, 2);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvYieldInfo* pkYieldInfo = GC.getYieldInfo(eYield);

	int iYieldModifier = 0;
	if(pkBuildingInfo && pkYieldInfo)
	{
		iYieldModifier= pkBuildingInfo->GetYieldModifier(eYield);
	}

	lua_pushinteger(L, iYieldModifier);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaGame::lGetPovertyHappinessChangeBuilding(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetPovertyHappinessChangeBuilding());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetDefenseHappinessChangeBuilding(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetDefenseHappinessChangeBuilding());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetUnculturedHappinessChangeBuilding(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetUnculturedHappinessChangeBuilding());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetIlliteracyHappinessChangeBuilding(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetIlliteracyHappinessChangeBuilding());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinorityHappinessChangeBuilding(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetMinorityHappinessChangeBuilding());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetPovertyHappinessChangeBuildingGlobal(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetPovertyHappinessChangeBuildingGlobal());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetDefenseHappinessChangeBuildingGlobal(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetUnculturedHappinessChangeBuildingGlobal(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetUnculturedHappinessChangeBuildingGlobal());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetIlliteracyHappinessChangeBuildingGlobal(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetIlliteracyHappinessChangeBuildingGlobal());
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinorityHappinessChangeBuildingGlobal(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pkBuildingInfo)
	{
		lua_pushinteger(L, pkBuildingInfo->GetMinorityHappinessChangeBuildingGlobal());
	}

	return 1;
}

//////
/// CORPORATIONS
//////
//------------------------------------------------------------------------------
int CvLuaGame::lGetBuildingCorporateGPChange(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes) luaL_checkint(L, 1);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	int iYieldChange = 0;
	if(pkBuildingInfo)
	{
		iYieldChange = pkBuildingInfo->GetGPRateModifierPerXFranchises();
	}

	lua_pushinteger(L, iYieldChange);
	return 1;
}
int CvLuaGame::lGetPromiseDuration(lua_State* L)
{
	int iTimeOutTurns = (GC.getEXPANSION_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;
	lua_pushinteger(L, iTimeOutTurns);
	return 1;
}

int CvLuaGame::lGetCorporationFounder(lua_State* L)
{
	const CorporationTypes eCorporation = (CorporationTypes) luaL_checkint(L, 1);
	int iReturn = (int) GC.getGame().GetCorporationFounder(eCorporation);
	lua_pushinteger(L, iReturn);
	return 1;
}

int CvLuaGame::lGetCorporationHeadquarters(lua_State* L)
{
	const CorporationTypes eCorporation = (CorporationTypes)luaL_checkint(L, 1);
	
	CvCity* pkCity = NULL;
	CvCorporation* pCorporation = GC.getGame().GetGameCorporations()->GetCorporation(eCorporation);
	if (pCorporation)
	{
		CvPlot* pHQPlot = GC.getMap().plot(pCorporation->m_iHeadquartersCityX, pCorporation->m_iHeadquartersCityY);
		if (pHQPlot)
		{
			pkCity = pHQPlot->getPlotCity();
		}
	}

	CvLuaCity::Push(L, pkCity);
	return 1;
}

int CvLuaGame::lGetNumCorporationsFounded(lua_State* L)
{
	int iReturn = GC.getGame().GetNumCorporationsFounded();
	lua_pushinteger(L, iReturn);
	return 1;
}

int CvLuaGame::lGetNumAvailableCorporations(lua_State* L)
{
	int iReturn = GC.getGame().GetGameCorporations()->GetNumAvailableCorporations();
	lua_pushinteger(L, iReturn);
	return 1;
}


#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
int CvLuaGame::lGetGreatestPlayerResourceMonopoly(lua_State* L)
{
	const ResourceTypes eResource = (ResourceTypes) luaL_checkint(L, 1);
	int iReturn = GC.getGame().GetGreatestPlayerResourceMonopoly(eResource);
	lua_pushinteger(L, iReturn);
	return 1;
}
#endif
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lGetWorldNumCitiesUnhappinessPercent(lua_State* L)
{
	lua_pushinteger(L, GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetDealDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetDealDuration);
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetRelationshipDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetRelationshipDuration);
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetPeaceDuration(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::GetPeaceDuration);
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetUnitUpgradesTo(lua_State* L)
{
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 1);

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		luaL_error(L, "Unit row at ID %d is empty.", eUnit);
		return 0;
	}

	for(int iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
	{
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes) iUnitClassLoop);
		if(pkUnitClassInfo != NULL)
		{
			if(pkUnitInfo->GetUpgradeUnitClass(iUnitClassLoop))	// Hardcoded to look at entry 0
			{
				lua_pushinteger(L, pkUnitClassInfo->getDefaultUnitIndex());
				return 1;
			}
		}
	}

	lua_pushinteger(L, -1);
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetCombatPrediction(lua_State* L)
{
	const CvUnit* pAttackingUnit = CvLuaUnit::GetInstance(L, 1);
	const CvUnit* pDefendingUnit = CvLuaUnit::GetInstance(L, 2);
	CombatPredictionTypes ePrediction = GC.getGame().GetCombatPrediction(pAttackingUnit, pDefendingUnit);
	lua_pushinteger(L, ePrediction);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetTimeString(lua_State* L)
{
	CvString timeString;

	CvGame& kGame = GC.getGame();
	CvGameTextMgr::setDateStr(timeString, kGame.getGameTurn(), true, kGame.getCalendar(), kGame.getStartYear(), kGame.getGameSpeedType());

	lua_pushstring(L, timeString.GetCString());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinimumFaithNextPantheon(lua_State* L)
{
	int iFaith = GC.getGame().GetGameReligions()->GetMinimumFaithNextPantheon();
	lua_pushinteger(L, iFaith);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lSetMinimumFaithNextPantheon(lua_State* L)
{
	GC.getGame().GetGameReligions()->SetMinimumFaithNextPantheon(lua_tointeger(L, 1));
	return 1;
}
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
//------------------------------------------------------------------------------
int CvLuaGame::lIsInSomeReligion(lua_State* L)
{
	BeliefTypes eBelief = (BeliefTypes)luaL_optint(L, 1, NO_BELIEF);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TRAITS_ANY_BELIEF)
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 2, NO_PLAYER);
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TRAITS_ANY_BELIEF)
	const bool bResult = GC.getGame().GetGameReligions()->IsInSomeReligion(eBelief, ePlayer);
#else
	const bool bResult = GC.getGame().GetGameReligions()->IsInSomeReligion(eBelief);
#endif
	lua_pushboolean(L, bResult);

	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lGetAvailablePantheonBeliefs(lua_State* L)
{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 1, GC.getGame().getActivePlayer());
#endif

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailablePantheonBeliefs(ePlayer);
#else
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailablePantheonBeliefs();
#endif
	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetAvailableFounderBeliefs(lua_State* L)
{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 1, NO_PLAYER);
	ReligionTypes eReligion = (ReligionTypes)luaL_optint(L, 2, NO_RELIGION);
#endif

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableFounderBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableFounderBeliefs();
#endif
	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetAvailableFollowerBeliefs(lua_State* L)
{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 1, NO_PLAYER);
	ReligionTypes eReligion = (ReligionTypes)luaL_optint(L, 2, NO_RELIGION);
#endif

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableFollowerBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableFollowerBeliefs();
#endif
	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetAvailableEnhancerBeliefs(lua_State* L)
{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 1, NO_PLAYER);
	ReligionTypes eReligion = (ReligionTypes)luaL_optint(L, 2, NO_RELIGION);
#endif

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableEnhancerBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableEnhancerBeliefs();
#endif
	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetAvailableBonusBeliefs(lua_State* L)
{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 1, NO_PLAYER);
	ReligionTypes eReligion = (ReligionTypes)luaL_optint(L, 2, NO_RELIGION);
#endif

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableBonusBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableBonusBeliefs();
#endif
	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetAvailableReformationBeliefs(lua_State* L)
{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 1, NO_PLAYER);
	ReligionTypes eReligion = (ReligionTypes)luaL_optint(L, 2, NO_RELIGION);
#endif

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableReformationBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableReformationBeliefs();
#endif
	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumFollowers(lua_State* L)
{
	int iRtnValue;
	ReligionTypes eReligion;
	eReligion = (ReligionTypes)lua_tointeger(L, 1);
	iRtnValue = GC.getGame().GetGameReligions()->GetNumFollowers(eReligion);
	lua_pushinteger(L, iRtnValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumCitiesFollowing(lua_State* L)
{
	int iRtnValue;
	ReligionTypes eReligion;
	eReligion = (ReligionTypes)lua_tointeger(L, 1);
	iRtnValue = GC.getGame().GetGameReligions()->GetNumCitiesFollowing(eReligion);
	lua_pushinteger(L, iRtnValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetBeliefsInReligion(lua_State* L)
{
	ReligionTypes eReligion;
	eReligion = (ReligionTypes)lua_tointeger(L, 1);

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	CvReligionBeliefs beliefs = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER)->m_Beliefs;
	for(int iI = 0; iI < beliefs.GetNumBeliefs(); iI++)
	{
		const BeliefTypes eBelief = beliefs.GetBelief(iI);
		lua_pushinteger(L, eBelief);
		lua_rawseti(L, t, idx++);
	}

	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaGame::lBeliefIsInReligion(lua_State* L)
{
	ReligionTypes eReligion;
	eReligion = (ReligionTypes)lua_tointeger(L, 1);
	BeliefTypes eBelief = (BeliefTypes)lua_tointeger(L, 2);
	CvGameReligions *pGameReligions = GC.getGame().GetGameReligions();
	const CvReligion *pReligion = pGameReligions->GetReligion(eReligion, NO_PLAYER);
	if(pReligion && pReligion->m_Beliefs.HasBelief(eBelief))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumReligionsStillToFound(lua_State* L)
{
	int iRtnValue;
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_RELIGION_LOCAL_RELIGIONS)
	const bool bIgnoreLocal = luaL_optbool(L, 1, false);
	PlayerTypes ePlayer = (PlayerTypes)luaL_optint(L, 2, NO_PLAYER);
	iRtnValue = GC.getGame().GetGameReligions()->GetNumReligionsStillToFound(bIgnoreLocal, ePlayer);
#else
	iRtnValue = GC.getGame().GetGameReligions()->GetNumReligionsStillToFound();
#endif
	lua_pushinteger(L, iRtnValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumReligionsFounded(lua_State* L)
{
	int iRtnValue;
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_RELIGION_LOCAL_RELIGIONS)
	const bool bIgnoreLocal	= luaL_optint(L, 1, 1);
	iRtnValue = GC.getGame().GetGameReligions()->GetNumReligionsFounded(bIgnoreLocal);
#else
	iRtnValue = GC.getGame().GetGameReligions()->GetNumReligionsFounded();
#endif
	lua_pushinteger(L, iRtnValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetHolyCityForReligion(lua_State* L)
{
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 2));
	const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, ePlayer);

	CvCity* pkHolyCity = NULL;
	if(pkReligion)
	{
		pkHolyCity = pkReligion->GetHolyCity();
	}

	if(pkHolyCity)
	{
		CvLuaCity::Push(L, pkHolyCity);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetReligionName(lua_State* L)
{
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
	if(pkReligion && strlen(pkReligion->m_szCustomName) > 0)
	{
		lua_pushstring(L, pkReligion->m_szCustomName);
		return 1;
	}
	
	if (eReligion == NO_RELIGION)
	{
		lua_pushstring(L, "No Religion");
		return 1;
	}
	else
	{
		CvReligionEntry* pkEntry = GC.getReligionInfo(eReligion);
		if (pkEntry != NULL)
		{
			lua_pushstring(L, pkEntry->GetDescriptionKey());
			return 1;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetFounderBenefitsReligion(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(ePlayer);
	lua_pushinteger(L, eReligion);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lFoundPantheon(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	const BeliefTypes eBelief = static_cast<BeliefTypes>(luaL_checkint(L, 2));

	GC.getGame().GetGameReligions()->FoundPantheon(ePlayer, eBelief);

	return 0;
}
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
//------------------------------------------------------------------------------
int CvLuaGame::lEnhancePantheon(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	const BeliefTypes eBelief = static_cast<BeliefTypes>(luaL_checkint(L, 2));
	const bool bNotify = luaL_optbool(L, 3, true);
	
	// If this player has created a (local) religion, we need to enhance that instead!
	ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(ePlayer);
	if (eReligion == NO_RELIGION) eReligion = RELIGION_PANTHEON;

	GC.getGame().GetGameReligions()->EnhanceReligion(ePlayer, eReligion, eBelief, NO_BELIEF, bNotify);

	return 0;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lFoundReligion(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 2));
	const char* szCustomName = lua_tostring(L, 3);
	const BeliefTypes eBelief1 = static_cast<BeliefTypes>(luaL_checkint(L, 4));
	const BeliefTypes eBelief2 = static_cast<BeliefTypes>(luaL_checkint(L, 5));
	const BeliefTypes eBelief3 = static_cast<BeliefTypes>(luaL_checkint(L, 6));
	const BeliefTypes eBelief4 = static_cast<BeliefTypes>(luaL_checkint(L, 7));
	CvCity* pkHolyCity	= CvLuaCity::GetInstance(L, 8);

	GC.getGame().GetGameReligions()->FoundReligion(ePlayer, eReligion, szCustomName, eBelief1, eBelief2, eBelief3, eBelief4, pkHolyCity);

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lEnhanceReligion(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 2));
	const BeliefTypes eBelief1 = static_cast<BeliefTypes>(luaL_checkint(L, 3));
	const BeliefTypes eBelief2 = static_cast<BeliefTypes>(luaL_checkint(L, 4));
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_RELIGION)
	const bool bNotify = luaL_optbool(L, 5, true);

	GC.getGame().GetGameReligions()->EnhanceReligion(ePlayer, eReligion, eBelief1, eBelief2, bNotify);
#else
	GC.getGame().GetGameReligions()->EnhanceReligion(ePlayer, eReligion, eBelief1, eBelief2);
#endif

	return 0;
}
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaGame::lAddReformation(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 2));
	const BeliefTypes eBelief = static_cast<BeliefTypes>(luaL_checkint(L, 3));

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvGameReligions* pkGameReligions = GC.getGame().GetGameReligions();
	if (!pkGameReligions->HasAddedReformationBelief(ePlayer) && kPlayer.GetReligions()->HasCreatedReligion() && kPlayer.GetReligions()->GetReligionCreatedByPlayer() == eReligion)
	{
		pkGameReligions->AddReformationBelief(ePlayer, eReligion, eBelief);
	}

	return 0;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lSetHolyCity(lua_State* L)
{
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	CvCity* pkHolyCity	= CvLuaCity::GetInstance(L, 2);

	GC.getGame().GetGameReligions()->SetHolyCity(eReligion, pkHolyCity);

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetFounder(lua_State* L)
{
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 2));

	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, ePlayer);

	lua_pushinteger(L, (int)pReligion->m_eFounder);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lSetFounder(lua_State* L)
{
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	const PlayerTypes eFounder = static_cast<PlayerTypes>(luaL_checkint(L, 2));

	GC.getGame().GetGameReligions()->SetFounder(eReligion, eFounder);

	return 0;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaGame::lSetFoundYear(lua_State* L)
{
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	const int iFoundYear = lua_tointeger(L, 2);

	GC.getGame().GetGameReligions()->SetFoundYear(eReligion, iFoundYear);

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetFoundYear(lua_State* L)
{
	const ReligionTypes eReligion = static_cast<ReligionTypes>(luaL_checkint(L, 1));

	const int iValue = GC.getGame().GetGameReligions()->GetFoundYear(eReligion);

	lua_pushinteger(L, iValue);
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lGetTurnsBetweenMinorCivElections(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetTurnsBetweenMinorCivElections());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetTurnsUntilMinorCivElection(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetTurnsUntilMinorCivElection());
	return 1;
}
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
int CvLuaGame::lIsAchievementUnlocked(lua_State* L)
{
	int iAchievement = lua_tointeger(L, 1);
	lua_pushboolean(L, gDLL->IsAchievementUnlocked((EAchievement) iAchievement));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetSteamStat(lua_State* L)
{
	int iSteamStat = lua_tointeger(L, 1);
	int32 iValue = 0;
	gDLL->GetSteamStat((ESteamStat) iSteamStat, &iValue);
	lua_pushinteger(L, iValue);
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumActiveLeagues(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetGameLeagues()->GetNumActiveLeagues());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumLeaguesEverFounded(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetGameLeagues()->GetNumLeaguesEverFounded());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetLeague(lua_State* L)
{
	const LeagueTypes eLeague = (LeagueTypes) lua_tointeger(L, 1);

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(eLeague);
	if (pLeague != NULL)
	{
		CvLuaLeague::Push(L, pLeague);
	}
	else
	{
		lua_pushnil(L);
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetActiveLeague(lua_State* L)
{
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague != NULL)
	{
		CvLuaLeague::Push(L, pLeague);
	}
	else
	{
		lua_pushnil(L);
	}
	return 1;
}
#if defined(MOD_BALANCE_CORE)
int CvLuaGame::lDoEnactResolution(lua_State* L)
{
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague != NULL)
	{
		const ResolutionTypes iResolutionType = static_cast<ResolutionTypes>(luaL_checkint(L, 1));
		const int iChoice = luaL_checkint(L, 2);
		PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 3);
		CvEnactProposal fakeProposal(GC.getGame().GetGameLeagues()->GenerateResolutionUniqueID(), iResolutionType, pLeague->GetID(), ePlayer, iChoice);
		
		pLeague->DoEnactResolutionPublic(&fakeProposal);
	}
	return 1;
}
int CvLuaGame::lDoRepealResolution(lua_State* L)
{
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague != NULL)
	{
		const ResolutionTypes iResolutionType = static_cast<ResolutionTypes>(luaL_checkint(L, 1));
		PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
		const int iValue = lua_tointeger(L, 3);
		CvActiveResolution* pResolution = pLeague->GetActiveResolution(iResolutionType, iValue);
		if (pResolution)
		{
			CvRepealProposal fakeProposal(pResolution, ePlayer);

			pLeague->DoRepealResolutionPublic(&fakeProposal);
		}
	}
	return 1;
}

int CvLuaGame::lIsBeliefValid(lua_State* L)
{
	const ReligionTypes iReligionType = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	const BeliefTypes iBeliefType = static_cast<BeliefTypes>(luaL_checkint(L, 2));
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3);
	bool bHolyCityOnly = lua_toboolean(L, 4);

	bool bResult = false;
	
	if (iReligionType != NO_RELIGION && iBeliefType != NO_BELIEF && pkCity != NULL)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(iReligionType, pkCity->getOwner());
		if (pReligion && pReligion->m_Beliefs.IsBeliefValid(iBeliefType, iReligionType, pkCity->getOwner(), pkCity, bHolyCityOnly))
		{
			bResult = true;
		}
	}
	
	lua_pushboolean(L, bResult);
	return 1;
}

#endif
//------------------------------------------------------------------------------
int CvLuaGame::lIsProcessingMessages(lua_State* L)
{
	lua_pushboolean(L, gDLL->IsProcessingGameCoreMessages());
	return 1;
}
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
int CvLuaGame::lCreateGreatWork(lua_State* L)
{
	GreatWorkType eType = (GreatWorkType)lua_tointeger(L, 1);
	PlayerTypes eOwner = (PlayerTypes)lua_tointeger(L, 2);
	EraTypes eEra = (EraTypes)lua_tointeger(L, 3);
	CvString szCreator = lua_tostring(L, 4);
	
	int iGWindex = 	GC.getGame().GetGameCulture()->CreateGreatWork(eType, CultureHelpers::GetGreatWorkClass(eType), eOwner, eEra, szCreator);

	lua_pushinteger(L, iGWindex);
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkTooltip(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	PlayerTypes eOwner = (PlayerTypes)lua_tointeger(L, 2);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		CvString szTooltip = GC.getGame().GetGameCulture()->GetGreatWorkTooltip(iIndex, eOwner);
		lua_pushstring(L, szTooltip);
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkName(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		CvString szTooltip = GC.getGame().GetGameCulture()->GetGreatWorkName(iIndex);
		lua_pushstring(L, szTooltip);
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkType(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		GreatWorkType eGreatWork = GC.getGame().GetGameCulture()->GetGreatWorkType(iIndex);
		lua_pushinteger(L, eGreatWork);
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkClass(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		GreatWorkClass eGreatWork = GC.getGame().GetGameCulture()->GetGreatWorkClass(iIndex);
		lua_pushinteger(L, eGreatWork);
		return 1;
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkArtist(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		CvString szTooltip = GC.getGame().GetGameCulture()->GetGreatWorkArtist(iIndex);
		lua_pushstring(L, szTooltip);
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkEra(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		CvString szTooltip = GC.getGame().GetGameCulture()->GetGreatWorkEra(iIndex);
		lua_pushstring(L, szTooltip);
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkEraAbbreviation(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		CvString szTooltip = GC.getGame().GetGameCulture()->GetGreatWorkEraAbbreviation(iIndex);
		lua_pushstring(L, szTooltip);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkEraShort(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		CvString szTooltip = GC.getGame().GetGameCulture()->GetGreatWorkEraShort(iIndex);
		lua_pushstring(L, szTooltip);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkCreator(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		PlayerTypes ePlayer = GC.getGame().GetGameCulture()->GetGreatWorkCreator(iIndex);
		lua_pushinteger(L, ePlayer);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkController(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		PlayerTypes ePlayer = GC.getGame().GetGameCulture()->GetGreatWorkController(iIndex);
		lua_pushinteger(L, ePlayer);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetGreatWorkCurrentThemingBonus(lua_State* L)
{
	const int iIndex = lua_tointeger(L, 1);
	if (iIndex < GC.getGame().GetGameCulture()->GetNumGreatWorks())
	{
		int iBonus = GC.getGame().GetGameCulture()->GetGreatWorkCurrentThemingBonus(iIndex);
		lua_pushinteger(L, iBonus);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetArtifactName(lua_State* L)
{
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 1, false);
	GreatWorkType eGreatWork = CultureHelpers::GetArtifact(pkPlot);
	CvString szName = CultureHelpers::GetGreatWorkName(eGreatWork);
	lua_pushstring(L, szName);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumFreePolicies(lua_State* L)
{
	const PolicyBranchTypes eBranch = static_cast<PolicyBranchTypes>(luaL_checkint(L, 1));
	lua_pushinteger(L, PolicyHelpers::GetNumFreePolicies(eBranch));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetLongestCityConnectionPlots(lua_State* L)
{
	CvPlot* pPlot1 = NULL;
	CvPlot* pPlot2 = NULL;

	int iFurthestPlotDistance = 0;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;

		CvCity* pFirstCity = NULL;
		CvCity* pSecondCity = NULL;
		int iLoop1;
		int iLoop2;

		SPathFinderUserData data(ePlayer, PT_CITY_CONNECTION_LAND, ROUTE_RAILROAD);

		for (pFirstCity = GET_PLAYER(ePlayer).firstCity(&iLoop1); pFirstCity != NULL; pFirstCity = GET_PLAYER(ePlayer).nextCity(&iLoop1))
		{
			CvPlot* pFirstCityPlot = pFirstCity->plot();

			for (pSecondCity = GET_PLAYER(ePlayer).firstCity(&iLoop2); pSecondCity != NULL; pSecondCity = GET_PLAYER(ePlayer).nextCity(&iLoop2))
			{
				// don't measure distance to same city
				if (pFirstCity == pSecondCity)
				{
					continue;
				}

				CvPlot* pSecondCityPlot = pSecondCity->plot();

				int iThisPlotDistance = plotDistance(pFirstCityPlot->getX(), pFirstCityPlot->getY(), pSecondCityPlot->getX(), pSecondCityPlot->getY());
				if (iThisPlotDistance > iFurthestPlotDistance)
				{
					if (GC.GetStepFinder().DoesPathExist(pFirstCityPlot, pSecondCityPlot, data))
					{
						// found a connection
						pPlot1 = pFirstCityPlot;
						pPlot2 = pSecondCityPlot;
						iFurthestPlotDistance = iThisPlotDistance;
					}
				}
			}
		}
	}

	CvLuaPlot::Push(L, pPlot1);
	CvLuaPlot::Push(L, pPlot2);
	return 2;
}

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_TRADEROUTES)
//------------------------------------------------------------------------------
// Most of this came from CvLuaPlayer::lGetTradeRoutes(lua_State* L)
int CvLuaGame::lGetTradeRoute(lua_State* L)
{
	const int iRouteIndex = lua_tointeger(L, 1);

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	if (!pTrade->IsTradeRouteIndexEmpty(iRouteIndex)) {
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(iRouteIndex));
		CvCity* pFromCity = GC.getMap().plot(pConnection->m_iOriginX, pConnection->m_iOriginY)->getPlotCity();
		CvPlayer* pFromPlayer = &GET_PLAYER(pFromCity->getOwner());
		CvCity* pToCity = GC.getMap().plot(pConnection->m_iDestX, pConnection->m_iDestY)->getPlotCity();
		CvPlayer* pToPlayer = &GET_PLAYER(pToCity->getOwner());

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lua_pushinteger(L, pConnection->m_eConnectionType);
		lua_setfield(L, t, "TradeConnectionType");
		lua_pushinteger(L, pConnection->m_eDomain);
		lua_setfield(L, t, "Domain");
		lua_pushinteger(L, pFromPlayer->getCivilizationType());
		lua_setfield(L, t, "FromCivilizationType");
		lua_pushinteger(L , pFromPlayer->GetID());
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
#if defined(MOD_BALANCE_CORE)
		int iToDelta = (pFromCity->GetBaseTourism() / 100) * pFromCity->GetCityCulture()->GetTourismMultiplier(pToPlayer->GetID(), true, true, false, true, true);
		int iFromDelta = (pToCity->GetBaseTourism() / 100) * pToCity->GetCityCulture()->GetTourismMultiplier(pFromPlayer->GetID(), true, true, false, true, true);
#else
		int iToDelta = pFromCity->GetBaseTourism() * pFromCity->GetCityCulture()->GetTourismMultiplier(pToPlayer->GetID(), true, true, false, true, true);
		int iFromDelta = pToCity->GetBaseTourism() * pToCity->GetCityCulture()->GetTourismMultiplier(pFromPlayer->GetID(), true, true, false, true, true);
#endif
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


		return 1;
	}

	return 0;
}
#endif

//------------------------------------------------------------------------------
int CvLuaGame::lSelectedUnit_SpeculativePopupTradeRoute_Display(lua_State* L)
{
	int iPlotX,iPlotY;
	TradeConnectionType type;
	DomainTypes eDomain;

	iPlotX = lua_tointeger(L,1);
	iPlotY = lua_tointeger(L,2);
	type = (TradeConnectionType)lua_tointeger(L,3);
	eDomain = (DomainTypes)lua_tointeger(L, 4);

	GC.getGame().GetGameTrade()->DisplayTemporaryPopupTradeRoute(iPlotX,iPlotY,type, eDomain);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lSelectedUnit_SpeculativePopupTradeRoute_Hide(lua_State* L)
{
	int iPlotX,iPlotY;
	TradeConnectionType type;
	
	iPlotX = lua_tointeger(L,1);
	iPlotY = lua_tointeger(L,2);
	type = (TradeConnectionType)lua_tointeger(L,3);
	GC.getGame().GetGameTrade()->HideTemporaryPopupTradeRoute(iPlotX,iPlotY,type);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lMouseoverUnit(lua_State* L)
{
	CvUnit* pUnit;
	bool bEnter;

	pUnit = CvLuaUnit::GetInstance(L, 1);
	bEnter = lua_toboolean(L, 2);

	GC.getGame().mouseoverUnit(pUnit, bEnter);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumArchaeologySites(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetNumArchaeologySites());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumHiddenArchaeologySites(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetNumHiddenArchaeologySites());
	return 1;
}

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_EXTENSIONS)
int CvLuaGame::lExitLeaderScreen(lua_State* L)
{
	CvPreGame::popGameType();
	return 0;
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
int CvLuaGame::lGetDllGuid(lua_State* L)
{
	CvString szDllGuid = GC.getGame().getDllGuid();
	lua_pushstring(L, szDllGuid);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lReloadGameDataDefines(lua_State* L)
{
	GC.cacheGlobals();
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lReloadCustomModOptions(lua_State* L)
{
	gCustomMods.reloadCache();
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsCustomModOption(lua_State* L)
{
	const char* szOption = luaL_checkstring(L, 1);
	lua_pushboolean(L, (gCustomMods.getOption(szOption, 0) == 1));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetCustomModOption(lua_State* L)
{
	const char* szOption = luaL_checkstring(L, 1);
	lua_pushinteger(L, gCustomMods.getOption(szOption, 0));
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lSpewTestEvents(lua_State* L)
{
	const int iLimit = luaL_optint(L, 1, 1000);

	int iValue = 0;
	cvStopWatch watch("lua test event",0,0,true);
	watch.StartPerfTest();

	if (iLimit > 0) {
		for (int i = 0; i < iLimit; ++i) {
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) {
				CvLuaArgsHandle args;
				args->Push(-1);
				args->Push(-1);
				args->Push(-1);
				args->Push(-1);

				bool bResult;
				LuaSupport::CallHook(pkScriptSystem, "TestEvent", args.get(), bResult);
			}
		}
	} else {
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) {
			CvLuaArgsHandle args;
			args->Push(-1);
			args->Push(-1);
			args->Push(-1);
			args->Push(-1);

			LuaSupport::CallAccumulator(pkScriptSystem, "TestAccumulator", args.get(), iValue);
		}
	}

	watch.EndPerfTest();

	int iSec = (int) watch.GetDeltaInSeconds();
	int iMSec = (int) ((watch.GetDeltaInSeconds() - iSec)*1000);

	lua_pushinteger(L, iSec);
	lua_pushinteger(L, iMSec);
	lua_pushinteger(L, iValue);
	return 3;
}
#endif


#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CIV4_FEATURES)
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinimumVoluntaryVassalTurns(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().getGameSpeedInfo().getMinimumVoluntaryVassalTurns());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinimumVassalTurns(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().getGameSpeedInfo().getMinimumVassalTurns());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumTurnsBetweenVassals(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().getGameSpeedInfo().getNumTurnsBetweenVassals());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinimumVassalTax(lua_State* L)
{
	lua_pushinteger(L, GC.getVASSALAGE_VASSAL_TAX_PERCENT_MINIMUM());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMaximumVassalTax(lua_State* L)
{
	lua_pushinteger(L, GC.getVASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinimumVassalLiberateTurns(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().getGameSpeedInfo().getMinimumVassalLiberateTurns());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetMinimumVassalTaxTurns(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().getGameSpeedInfo().getMinimumVassalTaxTurns());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetVassalageEnabledEra(lua_State* L)
{
	int iEra = -1;
	for(int iLoopEra = 0; iLoopEra < GC.getNumEraInfos(); iLoopEra++) {
		if(GC.getEraInfo(static_cast<EraTypes>(iLoopEra))->getVassalageEnabled()) {
			iEra = iLoopEra;
			break;
		}
	}
	lua_pushinteger(L, iEra);
	return 1;
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
int CvLuaGame::lAnyoneHasBelief(lua_State* L)
{
	const BeliefTypes iBeliefType = static_cast<BeliefTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasBelief(iBeliefType));
	return 1;
}

int CvLuaGame::lAnyoneHasBuilding(lua_State* L)
{
	const BuildingTypes iBuildingType = static_cast<BuildingTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasBuilding(iBuildingType));
	return 1;
}

int CvLuaGame::lAnyoneHasBuildingClass(lua_State* L)
{
	const BuildingClassTypes iBuildingClassType = static_cast<BuildingClassTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasBuildingClass(iBuildingClassType));
	return 1;
}

int CvLuaGame::lAnyoneHasAnyWonder(lua_State* L)
{
	lua_pushboolean(L, GC.getGame().AnyoneHasAnyWonder());
	return 1;
}

int CvLuaGame::lAnyoneHasWonder(lua_State* L)
{
	const BuildingTypes iBuildingType = static_cast<BuildingTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasWonder(iBuildingType));
	return 1;
}

int CvLuaGame::lGetCivilizationPlayer(lua_State* L)
{
	const CivilizationTypes iCivilizationType = static_cast<CivilizationTypes>(luaL_checkint(L, 1));
	lua_pushinteger(L, GC.getGame().GetCivilizationPlayer(iCivilizationType));
	return 1;
}

int CvLuaGame::lAnyoneIsInEra(lua_State* L)
{
	const EraTypes iEraType = static_cast<EraTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneIsInEra(iEraType));
	return 1;
}

int CvLuaGame::lAnyoneHasReachedEra(lua_State* L)
{
	const EraTypes iEraType = static_cast<EraTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasReachedEra(iEraType));
	return 1;
}

int CvLuaGame::lAnyoneHasAnyNaturalWonder(lua_State* L)
{
	lua_pushboolean(L, GC.getGame().AnyoneHasAnyNaturalWonder());
	return 1;
}

int CvLuaGame::lAnyoneHasNaturalWonder(lua_State* L)
{
	const FeatureTypes iFeatureType = static_cast<FeatureTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasNaturalWonder(iFeatureType));
	return 1;
}

int CvLuaGame::lAnyoneHasPolicy(lua_State* L)
{
	const PolicyTypes iPolicyType = static_cast<PolicyTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasPolicy(iPolicyType));
	return 1;
}

int CvLuaGame::lAnyoneHasTenet(lua_State* L)
{
	const PolicyTypes iPolicyType = static_cast<PolicyTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasTenet(iPolicyType));
	return 1;
}

int CvLuaGame::lAnyoneHasPolicyBranch(lua_State* L)
{
	const PolicyBranchTypes iPolicyBranchType = static_cast<PolicyBranchTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasPolicyBranch(iPolicyBranchType));
	return 1;
}

int CvLuaGame::lAnyoneHasIdeology(lua_State* L)
{
	const PolicyBranchTypes iPolicyBranchType = static_cast<PolicyBranchTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasIdeology(iPolicyBranchType));
	return 1;
}

int CvLuaGame::lAnyoneHasProject(lua_State* L)
{
	const ProjectTypes iProjectType = static_cast<ProjectTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasProject(iProjectType));
	return 1;
}

int CvLuaGame::lAnyoneHasPantheon(lua_State* L)
{
	lua_pushboolean(L, GC.getGame().AnyoneHasPantheon());
	return 1;
}

int CvLuaGame::lAnyoneHasAnyReligion(lua_State* L)
{
	lua_pushboolean(L, GC.getGame().AnyoneHasAnyReligion());
	return 1;
}

int CvLuaGame::lAnyoneHasReligion(lua_State* L)
{
	const ReligionTypes iReligionType = static_cast<ReligionTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasReligion(iReligionType));
	return 1;
}

int CvLuaGame::lIsResolutionPassed(lua_State* L)
{
	const ResolutionTypes iResolutionType = static_cast<ResolutionTypes>(luaL_checkint(L, 1));
	const int iChoice = luaL_checkint(L, 2);
	lua_pushboolean(L, GC.getGame().IsResolutionPassed(iResolutionType, iChoice));
	return 1;
}

int CvLuaGame::lAnyoneHasTech(lua_State* L)
{
	const TechTypes iTechType = static_cast<TechTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasTech(iTechType));
	return 1;
}

int CvLuaGame::lAnyoneHasUnit(lua_State* L)
{
	const UnitTypes iUnitType = static_cast<UnitTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasUnit(iUnitType));
	return 1;
}

int CvLuaGame::lAnyoneHasUnitClass(lua_State* L)
{
	const UnitClassTypes iUnitClassType = static_cast<UnitClassTypes>(luaL_checkint(L, 1));
	lua_pushboolean(L, GC.getGame().AnyoneHasUnitClass(iUnitClassType));
	return 1;
}
#endif

#if defined(MOD_BALANCE_CORE)
int CvLuaGame::lFoundCorporation(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	const CorporationTypes eCorporation = static_cast<CorporationTypes>(luaL_checkint(L, 2));
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3);
	GC.getGame().GetGameCorporations()->FoundCorporation(ePlayer, eCorporation, pkCity);
	return 1;
}

int CvLuaGame::lCanFoundCorporation(lua_State* L)
{
	const PlayerTypes ePlayer = static_cast<PlayerTypes>(luaL_checkint(L, 1));
	const CorporationTypes eCorporation = static_cast<CorporationTypes>(luaL_checkint(L, 2));
	bool bResult = GC.getGame().GetGameCorporations()->CanFoundCorporation(ePlayer, eCorporation);
	lua_pushboolean(L, bResult);
	return 1;
}

int CvLuaGame::lIsCorporationFounded(lua_State* L)
{
	const CorporationTypes eCorporation = static_cast<CorporationTypes>(luaL_checkint(L, 1));
	bool bResult = GC.getGame().GetGameCorporations()->IsCorporationFounded(eCorporation);
	lua_pushboolean(L, bResult);
	return 1;
}

int CvLuaGame::lDoUpdateContracts(lua_State* L)
{
	GC.getGame().GetGameContracts()->DoUpdateContracts();
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumActiveContracts(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetGameContracts()->GetNumActiveContracts());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumInactiveContracts(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetGameContracts()->GetNumInactiveContracts());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumAvailableContracts(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetGameContracts()->GetNumAvailableContracts());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumUnavailableContracts(lua_State* L)
{
	lua_pushinteger(L, GC.getGame().GetGameContracts()->GetNumUnavailableContracts());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsContractActive(lua_State* L)
{
	const ContractTypes eContract = (ContractTypes)lua_tointeger(L, 1);
	const bool bResult = GC.getGame().GetGameContracts()->IsContractActive(eContract);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lIsContractAvailable(lua_State* L)
{
	const ContractTypes eContract = (ContractTypes)lua_tointeger(L, 1);
	const bool bResult = GC.getGame().GetGameContracts()->IsContractAvailable(eContract);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetActiveContract(lua_State* L)
{
	const ContractTypes eContract = (ContractTypes)lua_tointeger(L, 1);

	const CvContract* pContract = GC.getGame().GetGameContracts()->GetActiveContract(eContract);

	lua_pushinteger(L, (int)pContract->m_eContract);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetInactiveContract(lua_State* L)
{
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 1);
	const CvContract* pContract = GC.getGame().GetGameContracts()->GetInactiveContract(eContract);

	lua_pushinteger(L, (int)pContract->m_eContract);
	return 1;
}
int CvLuaGame::lSetContractUnits(lua_State* L)
{
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 1);
	UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	int iValue = lua_tointeger(L, 3);
	GC.getGame().SetContractUnits(eContract, eUnit, iValue);
	return 0;
}
int CvLuaGame::lGetContractUnits(lua_State* L)
{
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 1);
	UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	int iResult = GC.getGame().GetContractUnits(eContract, eUnit);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetInactiveContractUnitList(lua_State* L)
{
	lua_createtable(L, 0, 0);
	int contractunits_idx = 1;
	
	for(int i = 0; i < GC.getNumContractInfos(); i++)
	{
		ContractTypes eContract = (ContractTypes)i;
		const CvContract* pContract = GC.getGame().GetGameContracts()->GetInactiveContract(eContract);
		if(pContract)
		{
			for(int j = 0; j < GC.getNumUnitInfos(); j++)
			{
				UnitTypes eUnit = (UnitTypes)j;
			
				if(eUnit == NO_UNIT)
					continue;

				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
				if(!pkUnitInfo)
					continue;
			
				int iNumContractUnits = GC.getGame().GetContractUnits(eContract, eUnit);

				if(iNumContractUnits <= 0)
					continue;

				lua_createtable(L, 0, 0);
				const int t = lua_gettop(L);

				lua_pushinteger(L, eContract);
				lua_setfield(L, t, "Contract");

				lua_pushinteger(L, eUnit);
				lua_setfield(L, t, "Unit");

				lua_pushinteger(L, iNumContractUnits);
				lua_setfield(L, t, "NumUnits");

				lua_rawseti(L, -2, contractunits_idx++);
			}
		}	
	}
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaGame::lGetActiveContractUnitList(lua_State* L)
{

	lua_createtable(L, 0, 0);
	int contractunits_idx = 1;

	for(int i = 0; i < GC.getNumContractInfos(); i++)
	{
		ContractTypes eContract = (ContractTypes)i;
		const CvContract* pContract = GC.getGame().GetGameContracts()->GetActiveContract(eContract);
		if(pContract)
		{
			for(int j = 0; j < GC.getNumUnitInfos(); j++)
			{
				UnitTypes eUnit = (UnitTypes)j;
			
				if(eUnit == NO_UNIT)
					continue;

				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
				if(!pkUnitInfo)
					continue;
			
				int iNumContractUnits = GC.getGame().GetContractUnits(eContract, eUnit);

				if(iNumContractUnits <= 0)
					continue;

				lua_createtable(L, 0, 0);
				const int t = lua_gettop(L);

				lua_pushinteger(L, eContract);
				lua_setfield(L, t, "Contract");

				lua_pushinteger(L, eUnit);
				lua_setfield(L, t, "Unit");

				lua_pushinteger(L, iNumContractUnits);
				lua_setfield(L, t, "NumUnits");

				lua_rawseti(L, -2, contractunits_idx++);
			}
		}	
	}
	return 1;
}

int CvLuaGame::lDoSpawnFreeCity(lua_State* L)
{
	CvCity* pkCity = CvLuaCity::GetInstance(L, 1);
	//CvString strCustomName = luaL_optstring(L, "NO_NAME", 2);
	GC.getGame().CreateFreeCityPlayer(pkCity);
	return 0;
}
#endif

#if defined(MOD_BATTLE_ROYALE)
int CvLuaGame::lDeleteCSV(lua_State * L)
{
	const char* szCSVFilename = lua_tostring(L, 2);

	CvLoggerCSV::DeleteCSV(szCSVFilename);

	return 1;
}

int CvLuaGame::lWriteCSV(lua_State * L)
{
	const char* szCSVFilename = lua_tostring(L, 2);
	const char* szCSVLine = lua_tostring(L, 3);

	CvLoggerCSV::WriteCSVLog(szCSVFilename, szCSVLine);

	return 1;
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
int CvLuaGame::lIsPitbossHost(lua_State* L)
{
	lua_pushboolean(L, gDLL->IsPitbossHost());
	return 1;
}

int CvLuaGame::lIsHost(lua_State* L)
{
	lua_pushboolean(L, gDLL->IsHost());
	return 1;
}

int CvLuaGame::lGetTimeStringForYear(lua_State* L)
{
	int year = lua_tointeger(L, 1);

	CvString timeString;

	CvGame& kGame = GC.getGame();
	CvGameTextMgr::setDateStr(timeString, year, true, kGame.getCalendar(), kGame.getStartYear(), kGame.getGameSpeedType());

	lua_pushstring(L, timeString.GetCString());
	return 1;
}
#endif
