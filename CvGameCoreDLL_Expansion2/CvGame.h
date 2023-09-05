/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// game.h

#ifndef CIV5_GAME_H
#define CIV5_GAME_H

#include "CvRandom.h"
#include "FTimer.h"
#include "CvPreGame.h"
#include <CvLocalization.h>
#include "CvDistanceMap.h"
#include "CvDealClasses.h"
#include "CvEnumMap.h"

class CvPlot;
class CvCity;
class CvReplayMessage;
class CvReplayInfo;
class CvSiteEvaluatorForSettler;
class CvCitySiteEvaluator;
class IStartPositioner;
class CvGameReligions;
class CvGameCulture;
class CvGameLeagues;
class CvGameTrade;
class CvTacticalAnalysisMap;
class CvAdvisorCounsel;
class CvAdvisorRecommender;
#if defined(MOD_BALANCE_CORE)
class CvGameCorporations;
class CvGameContracts;
#endif

unsigned long hash32(uint input);

/// Helper type for producing seed values.
struct NODISCARD CvSeeder {
	uint value;

	inline CvSeeder() : value(0) {}
	template<typename T> explicit inline CvSeeder(T value) : value(hash32(static_cast<uint>(value))) {}
	inline CvSeeder(const CvSeeder& other) : value(other.value) {}

	inline static CvSeeder fromRaw(uint rawValue)
	{
		CvSeeder newSeed;
		newSeed.value = rawValue;
		return newSeed;
	}

	template<typename T> inline CvSeeder& mixRawAssign(T otherValue)
	{
		value ^= static_cast<uint>(otherValue) + 0x9e3779b9 + (value << 6) + (value >> 2);
		return *this;
	}

	template<typename T> inline CvSeeder& mixAssign(T otherValue)
	{
		return mixRawAssign(hash32(static_cast<uint>(otherValue)));
	}

	// `mixAssign` is specialized for mixing two seeders.
	// This is done to avoid rehashing the value within the seeder because it is presumably already hashed.
	template<> inline CvSeeder& mixAssign<CvSeeder>(CvSeeder otherSeed)
	{
		return mixRawAssign(otherSeed.value);
	}

	template<typename T> inline CvSeeder mixRaw(T otherValue) const
	{
		CvSeeder newSeed = *this;
		newSeed.mixRawAssign(otherValue);
		return newSeed;
	}

	template<typename T> inline CvSeeder mix(T otherValue) const
	{
		CvSeeder newSeed = *this;
		newSeed.mixAssign(otherValue);
		return newSeed;
	}

	inline operator uint()
	{
		return value;
	}
};

class CvGameInitialItemsOverrides
{
public:
	CvGameInitialItemsOverrides();

	std::vector<bool> GrantInitialFreeTechsPerTeam;
	std::vector<bool> GrantInitialGoldPerPlayer;
	std::vector<bool> GrantInitialCulturePerPlayer;
	std::vector<bool> ClearResearchQueuePerPlayer;
	std::vector<bool> GrantInitialUnitsPerPlayer;
};

class CvGame
{
public:
	CvGame();
	~CvGame();

	void init(HandicapTypes eHandicap);
	bool init2();

	void InitPlayers();

	void reset(HandicapTypes eHandicap, bool bConstructorCall = false);
	void uninit();

	void DoGameStarted();

	void update();
	void updateScore(bool bForce = false);

	int GetMapScoreMod() const;

	void updateCitySight(bool bIncrement);
	void updateSelectionList();
	void updateTestEndTurn();

	CvUnit* getPlotUnit(CvPlot* pPlot, int iIndex);
	void getPlotUnits(CvPlot* pPlot, std::vector<CvUnit*>& plotUnits);

	void cycleCities(bool bForward = true, bool bAdd = false);
	void cycleUnits(bool bClear, bool bForward = true, bool bWorkers = false);
	bool cyclePlotUnits(CvPlot* pPlot, bool bForward = true, bool bAuto = false, int iCount = -1);

	void selectionListMove(CvPlot* pPlot, bool bShift);
	void selectionListGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, int iFlags = 0, bool bAlt = false, bool bShift = false);
	void selectedCitiesGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false);
	void cityPushOrder(CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt = false, bool bShift = false, bool bCtrl = false);
	void CityPurchase(CvCity* pCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield);

	void selectUnit(CvUnit* pUnit, bool bClear, bool bToggle = false, bool bSound = false);
	void mouseoverUnit(CvUnit* pUnit, bool bEnter);
	void selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt) const;
	void selectAll(CvPlot* pPlot) const;
	void SelectSettler(void);

	bool selectionListIgnoreBuildingDefense();

	bool canHandleAction(int iAction, CvPlot* pPlot = NULL, bool bTestVisible = false);
	void handleAction(int iAction);

	bool canDoControl(ControlTypes eControl);
	void doControl(ControlTypes eControl);

	int getProductionPerPopulation(HurryTypes eHurry);

	int getAdjustedPopulationPercent(VictoryTypes eVictory) const;
	int getAdjustedLandPercent(VictoryTypes eVictory) const;

	int countCivPlayersAlive() const;
	int countCivPlayersEverAlive() const;
	int countCivTeamsAlive() const;
	int countCivTeamsEverAlive() const;
	int countHumanPlayersAlive() const;
	int countHumanPlayersEverAlive() const;
	int countSeqHumanTurnsUntilPlayerTurn(PlayerTypes playerID) const;

	int countMajorCivsAlive() const;
	int countMajorCivsEverAlive() const;
	int countTotalCivPower();
	int countTotalNukeUnits();
	int countKnownTechNumTeams(TechTypes eTech);

	int goldenAgeLength(int iManualLength = -1) const;
	int victoryDelay(VictoryTypes eVictory) const;

	int getImprovementUpgradeTimeMod(ImprovementTypes eImprovement, const CvPlot* pPlot = NULL) const;
	int getImprovementUpgradeTime(ImprovementTypes eImprovement, const CvPlot* pPlot = NULL) const;

	bool canTrainNukes() const;
	EraTypes getCurrentEra() const;
	void UpdateGameEra();

	TeamTypes getActiveTeam() const;
	CivilizationTypes getActiveCivilizationType() const;

	bool isReallyNetworkMultiPlayer() const;
	bool isNetworkMultiPlayer() const;
	bool isGameMultiPlayer() const;
	bool isTeamGame() const;

	void ReviveActivePlayer();

	int getNumHumanPlayers();
	int GetNumMajorCivsEver(bool bOnlyStart = false) const;
	int GetNumMinorCivsEver(bool bOnlyStart = false) const;
	int GetNumMinorCivsAlive();
	int getNumHumansInHumanWars(PlayerTypes ignorePlayer = NO_PLAYER);
	int getNumSequentialHumans(PlayerTypes ignorePlayer = NO_PLAYER);

	int getGameTurn() const;
	void setGameTurn(int iNewValue);
	void incrementGameTurn();
	int getTurnYear(int iGameTurn) const;
	int getGameTurnYear();

	int getElapsedGameTurns() const;
	void incrementElapsedGameTurns();

	int getMaxTurns() const;
	void setMaxTurns(int iNewValue) const;
	void changeMaxTurns(int iChange);

	int getMaxCityElimination() const;
	void setMaxCityElimination(int iNewValue) const;

	int getNumAdvancedStartPoints() const;
	void setNumAdvancedStartPoints(int iNewValue) const;

	int getStartTurn() const;
	void setStartTurn(int iNewValue);

	int GetWinningTurn() const;
	void SetWinningTurn(int iNewValue);

	int getStartYear() const;
	void setStartYear(int iNewValue);

	int getEstimateEndTurn() const;
	void setEstimateEndTurn(int iNewValue);
	int getDefaultEstimateEndTurn() const;
	void setDefaultEstimateEndTurn(int iNewValue);

	int getTurnSlice() const;
	int getMinutesPlayed() const;
	void setTurnSlice(int iNewValue);
	void changeTurnSlice(int iChange);

	void resetTurnTimer(bool resetGameTurnStart = true);
	int getMaxTurnLen() const;

	bool IsStaticTutorialActive() const;
	void SetStaticTutorialActive(bool bStaticTutorialActive);

	bool HasAdvisorMessageBeenSeen(_In_z_ const char* szAdvisorMessageName);
	void SetAdvisorMessageHasBeenSeen(_In_z_ const char* szAdvisorMessageName, bool bSeen);

	bool IsCityScreenBlocked();
	bool CanOpenCityScreen(PlayerTypes eOpener, CvCity* pCity);

	int getTargetScore() const;
	void setTargetScore(int iNewValue) const;

	int getNumGameTurnActive();
	int countNumHumanGameTurnActive();
	void changeNumGameTurnActive(int iChange, const std::string& why);

	int getNumCities() const;
	int getNumCivCities() const;
	void changeNumCities(int iChange);

	int getTotalPopulation() const;
	void changeTotalPopulation(int iChange);

	int getTotalEconomicValue() const;
	void setTotalEconomicValue(int iValue);

	int getHighestEconomicValue() const;
	void setHighestEconomicValue(int iValue);

	int getMedianEconomicValue() const;
	void setMedianEconomicValue(int iValue);

	int getNoNukesCount() const;
	bool isNoNukes() const;
	void changeNoNukesCount(int iChange);

	int getNukesExploded() const;
	void changeNukesExploded(int iChange);

	int getMaxPopulation() const;
	int getInitPopulation() const;
	int getInitLand() const;
	int getInitTech() const;
	int getInitWonders() const;
	void initScoreCalculation();

	int getAIAutoPlay() const;
	void setAIAutoPlay(int iNewValue, PlayerTypes eReturnPlayer);
	void changeAIAutoPlay(int iChange);

	unsigned int getInitialTime() const;
	void setInitialTime(unsigned int uiNewValue);

	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);

	TechTypes getOceanPassableTech() const;
	bool isCircumnavigated() const;
	void makeCircumnavigated();
	bool circumnavigationAvailable() const;

	void DoFromUIDiploEvent(FromUIDiploEventTypes eEvent, PlayerTypes eAIPlayer, int iArg1, int iArg2);

	// Diplomacy Victory
	void DoInitDiploVictory();
	void DoUpdateDiploVictory();
	int GetVotesNeededForDiploVictory() const;
	void SetVotesNeededForDiploVictory(int iValue);
	bool IsUnitedNationsActive();

	// DEPRECATED
	int GetUnitedNationsCountdown() const;
	void SetUnitedNationsCountdown(int iValue);
	void ChangeUnitedNationsCountdown(int iChange);
	int GetNumVictoryVotesTallied() const;
	void SetNumVictoryVotesTallied(int iValue, bool bPreliminaryVote);
	void ChangeNumVictoryVotesTallied(int iChange, bool bPreliminaryVote);
	int GetNumVictoryVotesExpected() const;
	void SetNumVictoryVotesExpected(int iValue);
	TeamTypes GetVoteCast(TeamTypes eVotingTeam) const;
	void SetVoteCast(TeamTypes eVotingTeam, TeamTypes eVote, bool bPreliminaryVote = false);
	TeamTypes GetPreviousVoteCast(TeamTypes eVotingTeam) const;
	void SetPreviousVoteCast(TeamTypes eVotingTeam, TeamTypes eVotingTarget);
	int GetNumVotesForTeam(TeamTypes eTeam) const;
	void SetNumVotesForTeam(TeamTypes eTeam, int iValue);
	void ChangeNumVotesForTeam(TeamTypes eTeam, int iChange);
	// End DEPRECATED

	Localization::String GetDiploResponse(const char* szLeader, const char* szResponse, const Localization::String& strOptionalKey1, const Localization::String& strOptionalKey2);

	bool isDebugMode() const;
	void setDebugMode(bool bDebugMode);
	void toggleDebugMode();
	void updateDebugModeCache();

	void setFOW(bool bMode);
	bool getFOW() const;

#ifdef EA_EVENT_GAME_SAVE
	void SetGameEventsSaveGame(bool bNewValue)
	{
		m_bSavedOnce = bNewValue;
	};
#endif

	int getPitbossTurnTime() const;
	void setPitbossTurnTime(int iHours);

	bool isHotSeat() const;
	bool isPbem() const;
	bool isPitboss() const;
	bool isSimultaneousTeamTurns() const;

	bool isFinalInitialized() const;
	void setFinalInitialized(bool bNewValue);

	bool IsWaitingForBlockingInput() const
	{
		return (m_eWaitDiploPlayer != NO_PLAYER);
	};
	void SetWaitingForBlockingInput(PlayerTypes ePlayer)
	{
		m_eWaitDiploPlayer = ePlayer;
	};

	bool getPbemTurnSent() const;
	void setPbemTurnSent(bool bNewValue);

	bool getHotPbemBetweenTurns() const;
	void setHotPbemBetweenTurns(bool bNewValue);

	bool isPlayerOptionsSent() const;
	void sendPlayerOptions(bool bForce = false);

	PlayerTypes getActivePlayer() const;
	void setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat = false, bool bAutoplaySwitch = false);

	const CvHandicapInfo& getHandicapInfo() const;
	HandicapTypes getHandicapType() const;
	void setHandicapType(HandicapTypes eHandicap);

	PlayerTypes getPausePlayer();
	bool isPaused();
	void setPausePlayer(PlayerTypes eNewValue);

	int GetStartingMilitaryRating() const;
	int GetMinimumHumanMilitaryRating() const;
	int ComputeRatingStrengthAdjustment(PlayerTypes ePlayer, PlayerTypes ePerceivingPlayer) const;
	int ComputeAverageMajorMilitaryRating(PlayerTypes ePerceivingPlayer, PlayerTypes eExcludedPlayer = NO_PLAYER) const;

	// Diplomacy AI Options (all except the first two are configurable in DiploAIOptions.sql)
	// Also consolidates some checks from various game options, for simplicity.
	bool IsVictoryCompetitionEnabled() const;
	bool IsEndgameAggressionEnabled() const;
	bool IsNoPrimaryVictoryPursuitRandomization() const;
	bool IsNoSecondaryVictoryPursuitRandomization() const;
	bool IsNuclearGandhiEnabled() const;
	bool IsAllWarBribesDisabled() const;
	bool IsAIWarBribesDisabled() const;
	bool IsAICityTradingHumanOnly() const;
	bool IsAICityTradingDisabled() const;
	bool IsAllCityTradingDisabled() const;
	bool IsInsultMessagesDisabled() const; // Only affects AI messages sent to humans
	bool IsComplimentMessagesDisabled() const; // Only affects AI messages sent to humans
	bool IsNoFakeOpinionModifiers() const;
	bool IsShowHiddenOpinionModifiers() const;
	bool IsShowAllOpinionValues() const;
	bool IsShowBaseHumanOpinion() const;

	// Advanced Options
	bool IsHideOpinionTable() const;
	bool IsHumanPermanentForAITemporaryTradingAllowed() const;
	bool IsPermanentForTemporaryTradingAllowed() const;
	bool IsFriendshipRequestsDisabled() const; // Only affects AI messages sent to humans
	bool IsGiftOffersDisabled() const; // Only affects AI messages sent to humans
	bool IsCoopWarRequestsWithHumansDisabled() const; // Only affects AI messages sent to humans
	bool IsCoopWarRequestsDisabled() const;
	bool IsHelpRequestsDisabled() const; // Only affects AI messages sent to humans
	bool IsTradeOffersDisabled(bool bIncludeRenewals = false) const; // Only affects AI messages sent to humans
	bool IsPeaceOffersDisabled() const; // Only affects AI messages sent to humans
	bool IsDemandsDisabled() const; // Only affects AI messages sent to humans
	bool IsIndependenceRequestsDisabled() const; // Only affects AI messages sent to humans
	bool IsAllDiploStatementsDisabled() const; // Only affects AI messages sent to humans
	bool IsAIPassiveMode() const;
	bool IsAIPassiveTowardsHumans() const;
	bool CanPlayerAttemptDominationVictory(PlayerTypes ePlayer, PlayerTypes eMakePeacePlayer, bool bCheckEliminationPossible) const;
	bool WouldMakingPeacePreventDominationVictory(PlayerTypes ePlayer, PlayerTypes eMakePeacePlayer) const;
	bool IsAIAggressiveMode() const;
	bool IsAIAggressiveTowardsHumans() const;

	// Debug Mode Options
	bool IsDiploDebugModeEnabled() const;
	bool IsAIMustAcceptHumanDiscussRequests() const;

	UnitTypes getBestLandUnit();
	int getBestLandUnitCombat();
	void setBestLandUnit(UnitTypes eNewValue);

	int GetFaithCost(CvUnitEntry *pkUnit) const;

	TeamTypes getWinner() const;
	VictoryTypes getVictory() const;
	void setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory);

	void LogTurnScores() const;
	void LogGameResult(const char* victoryTypeText, const char* victoryCivText) const;
#if defined(MOD_BALANCE_CORE)
	bool isVictoryRandomizationDone() const;
	void setVictoryRandomizationDone(bool bValue);
#endif

	bool isVictoryAvailable(VictoryTypes eVictory) const;
	int GetNextAvailableVictoryCompetitionRank(VictoryTypes eVictory) const;
	void DoPlaceTeamInVictoryCompetition(VictoryTypes eNewVictory, TeamTypes eTeam);
	TeamTypes getTeamVictoryRank(VictoryTypes eNewVictory, int iRank) const;
	void setTeamVictoryRank(VictoryTypes eNewVictory, int iRank, TeamTypes eTeam);

	int GetWorldMilitaryStrengthAverage(PlayerTypes ePlayer, bool bIncludeMe, bool bIncludeOnlyKnown);

	int GetTotalReligionTechCost() const;
	void DoUpdateTotalReligionTechCost();
	int GetCachedWorldReligionTechProgress() const;
	void DoUpdateCachedWorldReligionTechProgress();

	TechTypes GetReligionTech() const;
	void SetReligionTech(TechTypes eTech);
	int GetResearchLeftToReligionTech(TeamTypes eTeam);
	int GetResearchLeftToTech(TeamTypes eTeam, TechTypes eTech);

	GameStateTypes getGameState();
	void setGameState(GameStateTypes eNewValue);

	const CvEraInfo& getStartEraInfo() const;
	EraTypes getStartEra() const;

	CalendarTypes getCalendar() const;

	const CvGameSpeedInfo& getGameSpeedInfo() const;
	GameSpeedTypes getGameSpeedType() const;

	int getEndTurnMessagesReceived(int iIndex);
	void incrementEndTurnMessagesReceived(int iIndex);

	PlayerTypes getRankPlayer(int iRank);
	void setRankPlayer(int iRank, PlayerTypes ePlayer);

	int getPlayerRank(PlayerTypes ePlayer);
	void setPlayerRank(PlayerTypes ePlayer, int iRank);

	int getPlayerScore(PlayerTypes ePlayer);
	void setPlayerScore(PlayerTypes ePlayer, int iScore);

	TeamTypes getRankTeam(int iRank);
	void setRankTeam(int iRank, TeamTypes eTeam);

	int getTeamRank(TeamTypes eTeam);
	void setTeamRank(TeamTypes eTeam, int iRank);

	int getTeamScore(TeamTypes eTeam) const;
	void setTeamScore(TeamTypes eTeam, int iScore);

	bool isOption(GameOptionTypes eIndex) const;
	bool isOption(const char* pszOption) const;
	void setOption(GameOptionTypes eIndex, bool bEnabled);
	void setOption(const char* pszOption, bool bEnabled);

	bool isMPOption(MultiplayerOptionTypes eIndex) const;
	void setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled);

	int getUnitCreatedCount(UnitTypes eIndex);
	void incrementUnitCreatedCount(UnitTypes eIndex);

	int getUnitClassCreatedCount(UnitClassTypes eIndex);
	bool isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra = 0);
	void incrementUnitClassCreatedCount(UnitClassTypes eIndex);

	int getBuildingClassCreatedCount(BuildingClassTypes eIndex);
	bool isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra = 0);
	void incrementBuildingClassCreatedCount(BuildingClassTypes eIndex);
	void decrementBuildingClassCreatedCount(BuildingClassTypes eIndex);

	int getProjectCreatedCount(ProjectTypes eIndex);
	bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra = 0);
	void incrementProjectCreatedCount(ProjectTypes eIndex, int iExtra = 1);

	bool isVictoryValid(VictoryTypes eIndex) const;
	void setVictoryValid(VictoryTypes eIndex, bool bValid);
	bool areNoVictoriesValid() const;

	bool isSpecialUnitValid(SpecialUnitTypes eIndex);
	void makeSpecialUnitValid(SpecialUnitTypes eIndex);

	bool isNukesValid() const;
	void makeNukesValid(bool bValid = true);

	bool isInAdvancedStart() const;

	const CvString& getName();
	void setName(const char* szName);

	bool isDestroyedCityName(CvString& szName) const;
	void addDestroyedCityName(const CvString& szName);

	bool isGreatPersonBorn(CvString& szName) const;
	void addGreatPersonBornName(const CvString& szName);
	void removeGreatPersonBornName(const CvString& szName);

	CvRandom& getMapRand();
	int getMapRandNum(int iNum, const char* pszLog);

	CvRandom& getJonRand();
	int getJonRandNum(int iNum, const char* pszLog);
	int getJonRandNumVA(int iNum, const char* pszLog, ...);
	int getAsyncRandNum(int iNum, const char* pszLog);

	/// Generates a pseudo-random 32-bit number using the game's current state and an extra seed.
	///
	/// The number returned by this function is derived from the following parameters and consistent for any unique set:
	/// - The game's map seed.
	/// - The game's current turn.
	/// - The provided extra seed.
	/// 
	/// Given that the returned number is consistent for any unique set of the above parameters, the user should make an effort to
	/// ensure that the provided extra seed is different for any number of consecutive calls that occur on a single turn.
	/// Failure to do so will lead to the same number being returned for each consecutive call on any given turn.
	/// 
	/// If this function is used to generate a number that is subsequently used to change the game state in manner that all peers
	/// of a multiplayer session must replicate to retain synchronization, then the extra seed must be guaranteed to be identical 
	/// for all peers of that multiplayer session. If this requirement is not met, then the multiplayer session will desynchronize.
	uint randCore(CvSeeder extraSeed) const;

	/// Generates a psuedo-random number using `randCore` and remaps the output into an exclusive range within `0` and `limit`.
	/// Specifically, if `x` is the returned unsigned integer, then `x` is guaranteed to satisfy the following:
	/// - `x >= 0`
	/// - `x < limit`
	///
	/// The following invariants must be satisfied for function to operate correctly:
	/// - `limit != 0`
	/// 
	/// All advisories documented on `randCore` apply to this function.
	uint urandLimitExclusive(uint limit, CvSeeder extraSeed) const;

	/// Generates a psuedo-random number using `randCore` and remaps the output into an inclusive range within `0` and `limit`.
	/// Specifically, if `x` is the returned unsigned integer, then `x` is guaranteed to satisfy the following:
	/// - `x >= 0`
	/// - `x <= limit`
	/// 
	/// All advisories documented on `randCore` apply to this function.
	uint urandLimitInclusive(uint limit, CvSeeder extraSeed) const;

	/// Generates a psuedo-random number using `randCore` and remaps the output into an exclusive range within `min` and `max`.
	/// Specifically, if `x` is the returned unsigned integer, then `x` is guaranteed to satisfy the following:
	/// - `x >= min`
	/// - `x < max`
	/// 
	/// The following invariants must be satisfied for the function to operate correctly:
	/// - `min < max`
	/// 
	/// All advisories documented on `randCore` apply to this function.
	uint urandRangeExclusive(uint min, uint max, CvSeeder extraSeed) const;

	/// Generates a psuedo-random number using `randCore` and remaps the output into an inclusive range within `min` and `max`.
	/// Specifically, if `x` is the returned unsigned integer, then `x` is guaranteed to satisfy the following:
	/// - `x >= min`
	/// - `x <= max`
	///
	/// The following invariants must be satisfied for the function to operate correctly:
	/// - `min <= max`
	/// 
	/// All advisories documented on `randCore` apply to this function.
	uint urandRangeInclusive(uint min, uint max, CvSeeder extraSeed) const;

	/// Generates a psuedo-random number using `randCore` and remaps the output into an exclusive range within `min` and `max`.
	/// Specifically, if `x` is the returned signed integer, then `x` is guaranteed to satisfy the following:
	/// - `x >= min`
	/// - `x < max`
	/// 
	/// The following invariants must be satisfied for the function to operate correctly:
	/// - `min < max`
	/// 
	/// All advisories documented on `randCore` apply to this function.
	int randRangeExclusive(int min, int max, CvSeeder extraSeed) const;

	/// Generates a psuedo-random number using `randCore` and remaps the output into an inclusive range within `min` and `max`.
	/// Specifically, if `x` is the returned signed integer, then `x` is guaranteed to satisfy the following:
	/// - `x >= min`
	/// - `x <= max`
	/// 
	/// The following invariants must be satisfied for the function to operate correctly:
	/// - `min <= max`
	/// 
	/// All advisories documented on `randCore` apply to this function.
	int randRangeInclusive(int min, int max, CvSeeder extraSeed) const;

	int calculateSyncChecksum();
	int calculateOptionsChecksum();

#if defined(MOD_BALANCE_CORE)
	void debugSyncChecksum();

	PlayerTypes GetCorporationFounder( CorporationTypes eCorporation ) const;
	int GetNumCorporationsFounded() const;
#if defined (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	void UpdateGreatestPlayerResourceMonopoly(ResourceTypes eResource = NO_RESOURCE);
	int GetGreatestPlayerResourceMonopoly(ResourceTypes eResource) const;
	int GetGreatestPlayerResourceMonopolyValue(ResourceTypes eResource) const;
#endif
#endif

	void addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, const CvString& pszText, int iPlotX = -1, int iPlotY = -1);
	void clearReplayMessageMap();
	uint getNumReplayMessages() const;
	const CvReplayMessage* getReplayMessage(uint i) const;

	int CalculateMedianNumCities();
	int CalculateMedianNumPlots();
	int CalculateMedianNumWondersConstructed();

	void updateEconomicTotal();

	// Median Yield Per Pop Calculations
	void updateGlobalMedians();
	void SetMedianTechsResearched(int iValue);
	void SetBasicNeedsMedian(int iValue);
	void SetGoldMedian(int iValue);
	void SetScienceMedian(int iValue);
	void SetCultureMedian(int iValue);
	void DoGlobalMedianLogging() const;

	int GetMedianTechsResearched() const;
	int GetBasicNeedsMedian() const;
	int GetGoldMedian() const;
	int GetScienceMedian() const;
	int GetCultureMedian() const;

	void initSpyThreshold();
	int GetSpyThreshold() const;
#if defined(MOD_BALANCE_CORE_SPIES)
	void SetHighestSpyPotential();
#endif
	void DoBarbCountdown();

	void SetLastTurnCSAnnexed(int iValue);
	int GetLastTurnCSAnnexed() const;

	template<typename Game, typename Visitor>
	static void Serialize(Game& game, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;
	void readSaveGameDB(FDataStream& kStream);
	void writeSaveGameDB(FDataStream& kStream) const;
	void ReadSupportingClassData(FDataStream& kStream);
	void WriteSupportingClassData(FDataStream& kStream) const;

	void writeReplay(FDataStream& kStream);
	void saveReplay();

	void addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv);

	void testVictory();
	bool testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore = NULL) const;

#if defined(MOD_BALANCE_CORE)
	void doVictoryRandomization();
#endif

	int getPlotExtraYield(int iX, int iY, YieldTypes eYield) const;
	void setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iCost);
	void removePlotExtraYield(int iX, int iY);

	int getPlotExtraCost(int iX, int iY) const;
	void changePlotExtraCost(int iX, int iY, int iCost);
	void removePlotExtraCost(int iX, int iY);

	bool isCivEverActive(CivilizationTypes eCivilization) const;
	bool isLeaderEverActive(LeaderHeadTypes eLeader) const;
	bool isUnitEverActive(UnitTypes eUnit) const;
	bool isBuildingEverActive(BuildingTypes eBuilding) const;

	RouteTypes GetIndustrialRoute() const;
	void DoUpdateIndustrialRoute();

	ImprovementTypes GetBarbarianCampImprovementType();
	int GetBarbarianReleaseTurn() const;
	void SetBarbarianReleaseTurn(int iValue);

	UnitTypes GetRandomSpawnUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged);
	UnitTypes GetCompetitiveSpawnUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged, bool bIncludeShips, bool bNoResource = false, bool bIncludeOwnUUsOnly = false, bool bRandom = true);
	UnitTypes GetCsGiftSpawnUnitType(PlayerTypes ePlayer, bool bIncludeShips);

	UnitTypes GetRandomUniqueUnitType(bool bIncludeCivsInGame, bool bIncludeStartEra, bool bIncludeOldEras, bool bIncludeRanged, bool bCoastal, int iPlotX, int iPlotY);
	bool DoSpawnUnitsAroundTargetCity(PlayerTypes ePlayer, CvCity* pCity, int iNumber, bool bIncludeUUs, bool bIncludeShips, bool bNoResource, bool bIncludeOwnUUsOnly);

	CvSiteEvaluatorForSettler* GetSettlerSiteEvaluator();
	CvCitySiteEvaluator* GetStartSiteEvaluator();
	IStartPositioner* GetStartPositioner();
	CvGameDeals& GetGameDeals();
	CvGameReligions* GetGameReligions();
	CvGameCulture* GetGameCulture();
	CvGameLeagues* GetGameLeagues();
	CvGameTrade* GetGameTrade();
	CvString getDllGuid() const;
#if defined(MOD_BALANCE_CORE)
	CvGameCorporations* GetGameCorporations();
	CvGameContracts* GetGameContracts();
#endif

	int GetAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl);
	int IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl);

	void endTurnTimerSemaphoreIncrement();
	void endTurnTimerSemaphoreDecrement();
	void endTurnTimerReset();

	void DoMinorPledgeProtection(PlayerTypes eMajor, PlayerTypes eMinor, bool bProtect, bool bPledgeNowBroken = false);
	void DoMinorGiftGold(PlayerTypes eMinor, int iNumGold);
	void DoMinorGiftTileImprovement(PlayerTypes eMajor, PlayerTypes eMinor, int iPlotX, int iPlotY);
	void DoMinorBullyGold(PlayerTypes eBully, PlayerTypes eMinor);
	void DoMinorBullyUnit(PlayerTypes eBully, PlayerTypes eMinor);
	void DoMinorBuyout(PlayerTypes eMajor, PlayerTypes eMinor);
	void DoMinorBullyAnnex(PlayerTypes eMajor, PlayerTypes eMinor);
	void DoMinorMarriage(PlayerTypes eMajor, PlayerTypes eMinor);

	void DoDefensivePactNotification(PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer);
	void DoResearchAgreementNotification(PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer);

	int GetResearchAgreementCost(PlayerTypes ePlayer1, PlayerTypes ePlayer2) const;

	// Victory Stuff
	void DoTestConquestVictory();

	PlayerTypes GetBestWondersPlayer();
	void SetBestWondersPlayer(PlayerTypes ePlayer, int iWonderCount);
	PlayerTypes GetBestPoliciesPlayer();
	void SetBestPoliciesPlayer(PlayerTypes ePlayer, int iPolicyCount);
	PlayerTypes GetBestGreatPeoplePlayer();
	void SetBestGreatPeoplePlayer(PlayerTypes ePlayer, int iGreatPeopleCount);

	bool IsEndGameTechResearched() const;
	void SetEndGameTechResearched(bool bValue);
	// End Victory Stuff

	// Returns true if the tuner has ever been connected at any point during this game.
	bool TunerEverConnected() const;

	// slewis - Tutorial stuff
	bool IsEverAttackedTutorial() const;
	void SetEverAttackedTutorial(bool bValue);
	bool IsEverRightClickMoved() const;
	void SetEverRightClickMoved(bool bValue);
	bool IsCombatWarned() const;
	void SetCombatWarned(bool bValue);
	// end Tutorial stuff

	CvAdvisorCounsel* GetAdvisorCounsel();
	CvAdvisorRecommender* GetAdvisorRecommender();

	int GetTurnsBetweenMinorCivElections();
	int GetTurnsUntilMinorCivElection();

	void LogGameState(bool bLogHeaders = false) const;
	void unitIsMoving() const;

	void BuildProdModHelpText(CvString* toolTipSink, const char* strTextKey, int iMod, const char* strExtraKey = "") const;
	void BuildCannotPerformActionHelpText(CvString* toolTipSink, const char* strTextKey, const char* strExtraKey1 = "", const char* strExtraKey2 = "", int iValue = -666) const;

	bool allUnitAIProcessed() const;

	bool hasTurnTimerExpired(PlayerTypes playerID);
	void TurnTimerSync(float fCurTurnTime, float fTurnStartTime);
	void GetTurnTimerData(float& fCurTurnTime, float& fTurnStartTime);

	int GetDealDuration() const;
	int GetRelationshipDuration() const;
	int GetPeaceDuration() const;

	CombatPredictionTypes GetCombatPrediction(const CvUnit* pAttackingUnit, const CvUnit* pDefendingUnit);

	void NetMessageStaticsReset();
	void SetLastTurnAICivsProcessed();

	void TriggerArchaeologySiteCreation(bool bCheckInitialized);
	bool IsArchaeologyTriggered() const;
	int GetNumArchaeologySites() const;
	int GetNumHiddenArchaeologySites() const;

	TeamTypes GetTeamThatCircumnavigated() const;
	void SetTeamThatCircumnavigated(TeamTypes eNewValue);

	bool AnyoneHasBelief(BeliefTypes iBeliefType) const;
	bool AnyoneHasBuilding(BuildingTypes iBuildingType) const;
	bool AnyoneHasBuildingClass(BuildingClassTypes iBuildingClassType) const;
	bool AnyoneHasAnyWonder() const;
	bool AnyoneHasWonder(BuildingTypes iBuildingType) const;
	int GetCivilizationPlayer(CivilizationTypes iCivilizationType) const;
	bool AnyoneIsInEra(EraTypes iEraType) const;
	bool AnyoneHasReachedEra(EraTypes iEraType) const;
	bool AnyoneHasAnyNaturalWonder() const;
	bool AnyoneHasNaturalWonder(FeatureTypes iFeatureType) const;
	bool AnyoneHasPolicy(PolicyTypes iPolicyType) const;
	bool AnyoneHasTenet(PolicyTypes iPolicyType) const;
	bool AnyoneHasPolicyBranch(PolicyBranchTypes iPolicyBranchType) const;
	bool AnyoneHasIdeology(PolicyBranchTypes iPolicyBranchType) const;
	bool AnyoneHasProject(ProjectTypes iProjectType) const;
	bool AnyoneHasPantheon() const;
	bool AnyoneHasAnyReligion() const;
	bool AnyoneHasReligion(ReligionTypes iReligionType) const;
	bool IsResolutionPassed(ResolutionTypes iResolutionType, int iChoice) const;
	bool AnyoneHasTech(TechTypes iTechType) const;
	bool AnyoneHasUnit(UnitTypes iUnitType) const;
	bool AnyoneHasUnitClass(UnitClassTypes iUnitClassType) const;

#if defined(MOD_BALANCE_CORE_JFD)	
	void SetContractUnits(ContractTypes eContract, UnitTypes eUnit, int iValue);
	int GetContractUnits(ContractTypes eContract, UnitTypes eUnit) const;
#endif
	//Function to determine city size from city population
	unsigned int GetVariableCitySizeFromPopulation(unsigned int nPopulation);

	int GetNextGlobalID() { ++m_iGlobalAssetCounterCurrentTurn; return m_iGlobalAssetCounterAllPreviousTurns + m_iGlobalAssetCounterCurrentTurn; }
	void RollOverAssetCounter() { m_iGlobalAssetCounterAllPreviousTurns += m_iGlobalAssetCounterCurrentTurn; m_iGlobalAssetCounterCurrentTurn = 0; }

	void SetClosestCityMapDirty();
	//assuming a typical unit
	int GetClosestCityDistancePathLength(  const CvPlot* pPlot, bool bMajorsOnly=false );
	CvCity* GetClosestCityByPathLength(  const CvPlot* pPlot, bool bMajorsOnly=false );
	int GetClosestCityDistancePathLength(  const CvPlot* pPlot, PlayerTypes ePlayer );
	CvCity* GetClosestCityByPathLength(  const CvPlot* pPlot, PlayerTypes ePlayer );

	int GetClosestCityDistanceInPlots(  const CvPlot* pPlot, bool bMajorsOnly=false );
	CvCity* GetClosestCityByPlots(  const CvPlot* pPlot, bool bMajorsOnly=false );
	int GetClosestCityDistanceInPlots(  const CvPlot* pPlot, PlayerTypes ePlayer );
	CvCity* GetClosestCityByPlots(  const CvPlot* pPlot, PlayerTypes ePlayer );

	PlayerTypes GetClosestCityOwnerByPlots(  const CvPlot* pPlot, bool bMajorsOnly=false );

	PlayerTypes GetPotentialFreeCityPlayer(CvCity* pCity = NULL);
	TeamTypes GetPotentialFreeCityTeam(CvCity* pCity = NULL);
	bool CreateFreeCityPlayer(CvCity* pCity, bool bJustChecking = false);
	MinorCivTypes GetAvailableMinorCivType();

	//------------------------------------------------------------
	PlayerTypes GetAutoPlayReturnPlayer() const { return m_eAIAutoPlayReturnPlayer;	}
	//------------------------------------------------------------
	//------------------------------------------------------------

	bool isFirstActivationOfPlayersAfterLoad() const;

protected:

	bool m_firstActivationOfPlayersAfterLoad;

	//for MP RNG we split this into two parts - everybody agrees on the previous turn but for the current turn races are possible
	int m_iGlobalAssetCounterAllPreviousTurns;
	int m_iGlobalAssetCounterCurrentTurn;

	int m_iEndTurnMessagesSent;
	int m_iElapsedGameTurns;
	int m_iStartTurn;
	int m_iWinningTurn;
	int m_iStartYear;
	int m_iEstimateEndTurn;
	int m_iDefaultEstimateEndTurn;
	int m_iTurnSlice;
	int m_iCutoffSlice;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iTotalEconomicValue;
	int m_iHighestEconomicValue;
	int m_iMedianEconomicValue;
	int m_iNoNukesCount;
	int m_iNukesExploded;
	int m_iMaxPopulation;
	int m_iInitPopulation;
	int m_iInitLand;
	int m_iInitTech;
	int m_iInitWonders;
	int m_iAIAutoPlay;
	int m_iTotalReligionTechCost;
	int m_iCachedWorldReligionTechProgress;
	int m_iUnitedNationsCountdown;
	int m_iNumVictoryVotesTallied;
	int m_iNumVictoryVotesExpected;
	int m_iVotesNeededForDiploVictory;
	int m_iMapScoreMod;
	int m_iNumMajorCivsAliveAtGameStart;
	int m_iNumMinorCivsAliveAtGameStart;

	unsigned int m_uiInitialTime;

	bool m_bScoreDirty;
	bool m_bCircumnavigated;
	bool m_bDebugMode;
	bool m_bDebugModeCache;
	bool m_bFinalInitialized;
	bool m_bPbemTurnSent;
	bool m_bHotPbemBetweenTurns;
	bool m_bPlayerOptionsSent;
	bool m_bNukesValid;
	bool m_bEndGameTechResearched;
	bool m_bTunerEverConnected;
	bool m_bDynamicTurnsSimultMode;		//if playing dynamic turn mode, are we currently running simultaneous turns?
	PlayerTypes m_eWaitDiploPlayer;
	TechTypes m_eTechAstronomy;

	bool m_bFOW;

#ifdef EA_EVENT_GAME_SAVE
	bool m_bSavedOnce;
#endif

	// slewis - tutorial values
	bool m_bStaticTutorialActive;
	bool m_bTutorialEverAttacked;
	bool m_bEverRightClickMoved;
	bool m_bCombatWarned;
	std::tr1::unordered_set<size_t> m_AdvisorMessagesViewed;
	// slewis - tutorial values

	HandicapTypes m_eHandicap;
	PlayerTypes m_ePausePlayer;
	PlayerTypes m_eAIAutoPlayReturnPlayer;
	UnitTypes m_eBestLandUnit;
	TeamTypes m_eWinner;
	VictoryTypes m_eVictory;
	GameStateTypes m_eGameState;
	PlayerTypes m_eEventPlayer;
	PlayerTypes m_eBestWondersPlayer;
	PlayerTypes m_eBestPoliciesPlayer;
	PlayerTypes m_eBestGreatPeoplePlayer;
	TechTypes m_eReligionTech;
	RouteTypes m_eIndustrialRoute;
	EraTypes m_eGameEra;
	bool m_bArchaeologyTriggered;

	char /*TeamTypes*/ m_eTeamThatCircumnavigated;

	bool m_bVictoryRandomization;

	int m_iMedianTechsResearched;
	int m_iBasicNeedsMedian;
	int m_iGoldMedian;
	int m_iScienceMedian;
	int m_iCultureMedian;
	int m_iSpyThreshold;

	int m_iLastTurnCSSurrendered;
	CvEnumMap<ResourceTypes, PlayerTypes> m_aiGreatestMonopolyPlayer;

	CvString m_strScriptData;

	CvEnumMap<PlayerTypes, int> m_aiEndTurnMessagesReceived;
	CvEnumMap<PlayerTypes, int> m_aiRankPlayer;		// Ordered by rank...
	CvEnumMap<PlayerTypes, int> m_aiPlayerRank;		// Ordered by player ID...
	CvEnumMap<PlayerTypes, int> m_aiPlayerScore;	// Ordered by player ID...
	CvEnumMap<TeamTypes, int> m_aiRankTeam;			// Ordered by rank...
	CvEnumMap<TeamTypes, int> m_aiTeamRank;			// Ordered by team ID...
	CvEnumMap<TeamTypes, int> m_aiTeamScore;		// Ordered by team ID...

	CvEnumMap<UnitTypes, int> m_paiUnitCreatedCount;
	CvEnumMap<UnitClassTypes, int> m_paiUnitClassCreatedCount;
	CvEnumMap<BuildingClassTypes, int> m_paiBuildingClassCreatedCount;
	CvEnumMap<ProjectTypes, int> m_paiProjectCreatedCount;
	CvEnumMap<TeamTypes, TeamTypes> m_aiVotesCast;
	CvEnumMap<TeamTypes, TeamTypes> m_aiPreviousVotesCast;
	CvEnumMap<TeamTypes, int> m_aiNumVotesForTeam;

	CvEnumMap<TeamTypes, int> m_aiTeamCompetitionWinnersScratchPad;

	CvEnumMap<SpecialUnitTypes, bool> m_pabSpecialUnitValid;

	CvEnumMap<VictoryTypes, TeamTypes*> m_ppaaiTeamVictoryRank;
#if defined(MOD_BALANCE_CORE_JFD)
	CvEnumMap<ContractTypes, CvEnumMap<UnitTypes, int> > m_ppaiContractUnits;
#endif

	Database::Results* m_pDiploResponseQuery;

	std::vector<size_t> m_aszDestroyedCities;
	std::vector<size_t> m_aszGreatPeopleBorn;

	CvRandom m_mapRand;
	CvRandom m_jonRand;

	typedef std::vector<CvReplayMessage> ReplayMessageList;
	ReplayMessageList m_listReplayMessages;

	int m_iNumSessions;

	std::vector<PlotExtraYield> m_aPlotExtraYields;
	std::vector<PlotExtraCost> m_aPlotExtraCosts;

	int m_iEarliestBarbarianReleaseTurn;

	CvSiteEvaluatorForSettler* m_pSettlerSiteEvaluator;
	CvCitySiteEvaluator*	   m_pStartSiteEvaluator;
	IStartPositioner*          m_pStartPositioner;
	CvGameReligions*           m_pGameReligions;
	CvGameCulture*             m_pGameCulture;
	CvGameLeagues*             m_pGameLeagues;
	CvGameTrade*               m_pGameTrade;

	CvAdvisorCounsel*          m_pAdvisorCounsel;
	CvAdvisorRecommender*      m_pAdvisorRecommender;

	CvGameDeals                m_kGameDeals;

#if defined(MOD_BALANCE_CORE)
	CvGameCorporations*		   m_pGameCorporations;
	CvGameContracts*		   m_pGameContracts;
#endif

	//necessary because we only want to hide the mouseover of the most recently moused over unit -KS
	int                        m_iLastMouseoverUnitID;

	// CACHE: cache frequently used values

	FTimer  m_endTurnTimer;
	int     m_endTurnTimerSemaphore;
	int     m_lastTurnAICivsProcessed;
	FTimer  m_curTurnTimer;
	FTimer  m_timeSinceGameTurnStart;		//time since game turn started for human players
	float	m_fCurrentTurnTimerPauseDelta;	//
	bool    m_sentAutoMoves;
	bool	m_processPlayerAutoMoves;
	bool	m_bForceEndingTurn;

	CvDistanceMapByPathLength m_cityDistancePathLength;
	CvDistanceMapByPlots m_cityDistancePlots;

	//----------------------------------------------------------------

	void initDiplomacy();
	void initFreeState(CvGameInitialItemsOverrides& kOverrides) const;
	void initFreeUnits(CvGameInitialItemsOverrides& kOverrides);

	bool InitMap(CvGameInitialItemsOverrides& kGameInitialItemsOverrides);

	void assignStartingPlots();

	void setInitialItems(CvGameInitialItemsOverrides& kOverrides);
	void CheckGenerateArchaeology();
	void regenerateMap();

	void testExtendedGame();

	bool IsForceEndingTurn() const;
	void SetForceEndingTurn(bool bValue);

	void DoCacheMapScoreMod();

	void doTurn();

	void updateWar() const;
	void updateMoves();
	void updateTimers() const;
	void UpdatePlayers();
	void testAlive();

	void showEndGameSequence();

	void doUpdateCacheOnTurn();

	void CheckPlayerTurnDeactivate();

	void PopulateDigSite(CvPlot& kPlot, EraTypes eEra, GreatWorkArtifactClass eArtifact);
	void SpawnArchaeologySitesHistorically();
};

extern int gTactMovesCount[NUM_AI_TACTICAL_MOVES];
extern int gHomeMovesCount[NUM_AI_HOMELAND_MOVES];

#endif
