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
#include "CvLuaSupport.h"
#include "CvLuaCity.h"
#include "CvLuaGame.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"

#include "../CvGame.h"
#include "../CvGameCoreUtils.h"
#include "../CvInternalGameCoreUtils.h"
#include "../CvGameTextMgr.h"
#include "../CvReplayMessage.h"

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
	Method(GetMaxLand);
	Method(GetMaxTech);
	Method(GetMaxWonders);
	Method(GetInitPopulation);
	Method(GetInitLand);
	Method(GetInitTech);
	Method(GetInitWonders);
	Method(GetNumWorldWonders);

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

	Method(GetBestWondersPlayer);
	Method(GetBestPoliciesPlayer);
	Method(GetBestGreatPeoplePlayer);

	Method(IsEverAttackedTutorial);

	Method(GameplayDiplomacyAILeaderMessage);

	Method(GetResourceUsageType);

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

	Method(GetBuildingYieldChange);
	Method(GetBuildingYieldModifier);

	Method(GetWorldNumCitiesUnhappinessPercent);

	Method(GetDealDuration);
	Method(GetPeaceDuration);

	Method(GetUnitUpgradesTo);

	Method(GetCombatPrediction);

	Method(GetTimeString);

	Method(GetMinimumFaithNextPantheon);
	Method(SetMinimumFaithNextPantheon);

	Method(GetAvailablePantheonBeliefs);
	Method(GetAvailableFounderBeliefs)
	Method(GetAvailableFollowerBeliefs)
	Method(GetAvailableEnhancerBeliefs)
	Method(GetAvailableBonusBeliefs)

	Method(GetNumFollowers);
	Method(GetNumCitiesFollowing);

	Method(GetBeliefsInReligion);
	Method(GetNumReligionsStillToFound);
	Method(GetNumReligionsFounded);
	Method(GetHolyCityForReligion);
	Method(GetReligionName);
	Method(GetFounderBenefitsReligion);

	Method(FoundPantheon);
	Method(FoundReligion);
	Method(EnhanceReligion);
	Method(SetHolyCity);
	Method(GetFounder);
	Method(SetFounder);

	Method(GetTurnsBetweenMinorCivElections);
	Method(GetTurnsUntilMinorCivElection);

	Method(IsProcessingMessages)
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
	    = game.GetDiploResponse(szLeaderType, szResponseType);

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
	int iNumNukes = lua_tointeger(L, 2);
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
	int iNumNukes = lua_tointeger(L, 2);
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
//int getMaxLand();
int CvLuaGame::lGetMaxLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getMaxLand);
}
//------------------------------------------------------------------------------
//int getMaxTech();
int CvLuaGame::lGetMaxTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getMaxTech);
}
//------------------------------------------------------------------------------
//int getMaxWonders();
int CvLuaGame::lGetMaxWonders(lua_State* L)
{
	return BasicLuaMethod(L, &CvGame::getMaxWonders);
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
	const int rand_val = GetInstance()->getJonRand().get(max_num, strLog);

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
//------------------------------------------------------------------------------
int CvLuaGame::lGetAvailablePantheonBeliefs(lua_State* L)
{
	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailablePantheonBeliefs();
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
	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableFounderBeliefs();
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
	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableFollowerBeliefs();
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
	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableEnhancerBeliefs();
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
	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int idx = 1;

	std::vector<BeliefTypes> availableBeliefs = GC.getGame().GetGameReligions()->GetAvailableBonusBeliefs();
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
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumReligionsStillToFound(lua_State* L)
{
	int iRtnValue;
	iRtnValue = GC.getGame().GetGameReligions()->GetNumReligionsStillToFound();
	lua_pushinteger(L, iRtnValue);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaGame::lGetNumReligionsFounded(lua_State* L)
{
	int iRtnValue;
	iRtnValue = GC.getGame().GetGameReligions()->GetNumReligionsFounded();
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
		CvPlot* pkPlot = GC.getMap().plot(pkReligion->m_iHolyCityX, pkReligion->m_iHolyCityY);
		if(pkPlot)
		{
			pkHolyCity = pkPlot->getPlotCity();
		}
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

	CvReligionEntry* pkEntry = GC.getReligionInfo(eReligion);
	if(pkEntry != NULL)
	{
		lua_pushstring(L, pkEntry->GetDescriptionKey());
		return 1;
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

	GC.getGame().GetGameReligions()->EnhanceReligion(ePlayer, eReligion, eBelief1, eBelief2);

	return 0;
}
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
//------------------------------------------------------------------------------
int CvLuaGame::lIsProcessingMessages(lua_State* L)
{
	lua_pushboolean(L, gDLL->IsProcessingGameCoreMessages());
	return 1;
}
