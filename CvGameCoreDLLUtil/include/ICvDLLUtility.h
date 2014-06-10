#pragma once

#ifndef ICvDLLUtility_h
#define ICvDLLUtility_h

#include "LinkedList.h"
#include <string>
#include <FireWorks/FMemoryStream.h>

#include "CvDllInterfaces.h"
//
// abstract interface for utility functions used by DLL
// Copyright 2010 Firaxis Games
//
class ICvEngineScriptSystem1;
class CvDLLInterfaceIFaceBase;

class ICvCombatInfo1;
class ICvCity1;
class ICvDeal1;
class ICvPlot1;
class ICvRandom1;
class ICvUnit1;
class CvUnit;

class CvReplayInfo;


// {B044BCED-E001-42ef-9A42-03A8B09C1362}
static const GUID guidICvEngineUtility1 = 
{ 0xb044bced, 0xe001, 0x42ef, { 0x9a, 0x42, 0x3, 0xa8, 0xb0, 0x9c, 0x13, 0x62 } };

// {B4F5594D-BBA4-4a4c-9BBD-CEDF4CD10F2D}
static const GUID guidICvEngineUtility2 = 
{ 0xb4f5594d, 0xbba4, 0x4a4c, { 0x9b, 0xbd, 0xce, 0xdf, 0x4c, 0xd1, 0xf, 0x2d } };

// {F95CB893-6D19-4584-A741-6F64B855C7E9}
static const GUID guidICvEngineUtility3 = 
{ 0xf95cb893, 0x6d19, 0x4584, { 0xa7, 0x41, 0x6f, 0x64, 0xb8, 0x55, 0xc7, 0xe9 } };

// {DD8A29A2-E752-4bf7-9EE0-42B8155F175F}
static const GUID guidICvEngineUtility4 = 
{ 0xdd8a29a2, 0xe752, 0x4bf7, { 0x9e, 0xe0, 0x42, 0xb8, 0x15, 0x5f, 0x17, 0x5f } };


typedef FFastVector<int, true, c_eMPoolTypeGame> CvPlotIndexVector;

class ICvEngineUtility1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvEngineUtility1; }

	//This method has been deprecated due to the interface not being versioned.
	//You should use GC.GetEngineUserInterface() instead to obtain a pointer to ICvUserInterface.
	virtual DECLSPEC_DEPRECATED CvDLLInterfaceIFaceBase* getInterfaceIFace() = 0;

	virtual ICvEngineScriptSystem1* GetScriptSystem() = 0;
	
	virtual void reset() = 0;
	virtual void DoMapSetup(int iWorldNumPlots) = 0;

	virtual void DoTurn() = 0;
	virtual bool IsHost() const = 0;
	virtual bool IsPlayerConnected(PlayerTypes playerId) = 0;

	virtual Database::Connection* GetModsDatabase() = 0;
	virtual bool IsModActivated(_In_z_ const char* szModID) = 0;
	virtual bool IsModActivated(_In_z_ const char* szModID, int iVersion) = 0;

	virtual bool IsDLCValid(const GUID& kID) const = 0;

	virtual int getAssignedNetworkID(int iPlayerID) = 0;
	
	virtual bool IsPitbossHost() const = 0;
	virtual CvString GetPitbossSmtpHost() const = 0;
	virtual CvString GetPitbossSmtpLogin() const = 0;
	virtual CvString GetPitbossSmtpPassword() const = 0;
	virtual CvString GetPitbossEmail() const = 0;

	virtual void netMessageDebugLog(const std::string & logMessage) const = 0;
	virtual void sendPlayerInfo(PlayerTypes eActivePlayer) = 0;
	virtual void sendGameInfo(const CvString& szGameName, const CvString& szAdminPassword) = 0;
	virtual void sendPlayerOption(PlayerOptionTypes eOption, bool bValue) = 0;
	virtual void sendExtendedGame() = 0;
	virtual void SendFoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief) = 0;
	virtual void SendFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, int iCityX, int iCityY) = 0;
	virtual void SendEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, int iCityX, int iCityY) = 0;
	virtual void SendMoveSpy(PlayerTypes ePlayer, int iSpyIndex, int iTargetPlayer, int iTargetCity, bool bAsDiplomat) = 0;
	virtual void SendStageCoup(PlayerTypes eSpyPlayer, int iSpyIndex) = 0;
	virtual void SendFaithPurchase(PlayerTypes ePlayer, FaithPurchaseTypes eFaithPurchaseType, int iFaithPurchaseIndex) = 0;
	virtual void sendAutoMoves() = 0;
	virtual void sendTurnComplete() = 0;
	virtual bool HasSentTurnComplete() = 0;
	virtual bool HasReceivedTurnComplete(PlayerTypes ePlayer) = 0;
	virtual bool HasSentTurnAllComplete() = 0;
	virtual bool HasReceivedTurnAllComplete(PlayerTypes ePlayer) = 0;
	virtual bool HasReceivedTurnAllCompleteFromAllPlayers() = 0;
	virtual bool sendTurnUnready() = 0;
	virtual void sendPushMission(int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift) = 0;
	virtual void sendAutoMission(int iUnitID) = 0;
	virtual void sendDoCommand(int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt) = 0;
	virtual void sendPushOrder(int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl) = 0;
	virtual void sendPopOrder(int iCity, int iNum) = 0;
	virtual void sendSwapOrder(int iCity, int iNum0) = 0;
	virtual void sendPurchase(int iCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType) = 0;
	virtual void sendDoTask(int iCity, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl) = 0;
	virtual void sendResearch(TechTypes eTech, int iDiscover, PlayerTypes ePlayerToStealFrom, bool bShift) = 0;
	virtual void sendChat(const CvString& szChatString, ChatTargetTypes eTargetType, PlayerTypes toWho) = 0;
	virtual void sendPing(int iX, int iY) = 0;
	virtual void sendPause(int iPauseID = -1) = 0;
	virtual void sendChangeWar(TeamTypes iRivalTeam, bool bWar) = 0;
	virtual void sendIgnoreWarning(TeamTypes iRivalTeam) = 0;
	virtual void SendPledgeMinorProtection(PlayerTypes eMinor, bool bValue) = 0; //antonjs: DEPRECATED, use CvLuaGame::lDoMinorPledgeProtection instead
	virtual void SendMinorNoUnitSpawning(PlayerTypes eMinor, bool bValue) = 0;
	virtual void SendLiberateMinor(PlayerTypes eMinor, int iCityID) = 0;
	virtual void sendUpdatePolicies(bool bNOTPolicyBranch, int iID, bool bValue) = 0;
	virtual void SendDiploVote(PlayerTypes eVotePlayer) = 0;
	virtual void sendLaunch(PlayerTypes ePlayer, VictoryTypes eVictory) = 0;
	virtual void sendAdvancedStartAction(AdvancedStartActionTypes eAction, PlayerTypes ePlayer, int iX, int iY, int iData, bool bAdd) = 0;
	virtual void sendMinorCivQuestNoInterest(PlayerTypes eMinor, bool bPeace) = 0;
	virtual void sendMinorCivQuestCompleted(PlayerTypes eMinor, bool bPeace) = 0;
	virtual void sendMinorCivIntrusion(PlayerTypes eMinor, int iOptionChosen) = 0;
	virtual void sendMinorCivEnterTerritory(PlayerTypes eMinor) = 0;
	virtual void sendBarbarianRansom(int iOptionChosen, int iUnitID) = 0;
	virtual void sendGiftUnit(PlayerTypes eMinor, int iUnitID) = 0;
	virtual void sendReturnCivilian(bool bReturn, PlayerTypes ePlayer, int iUnitID) = 0;
	virtual void SendUpdateCityCitizens(int iCityID) = 0;
	virtual void SendSellBuilding(int iCityID, BuildingTypes eBuilding) = 0;
	virtual void SendRenameCity (int iCityID, CvString szNewName) = 0;
	virtual void SendRenameUnit (int iUnitID, CvString szNewName) = 0;
	virtual void sendPlayerHurry(HurryTypes eHurry) = 0;
	virtual void sendSwapUnits(int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift) = 0;
	virtual void sendCityBuyPlot (int iCityID, int iX, int iY) = 0;
	virtual void sendMinorPledgeProtection(PlayerTypes eMajor, PlayerTypes eMinor, bool bProtect, bool bPledgeNowBroken) = 0;
	virtual void sendMinorGiftGold(PlayerTypes eMinor, int iGold) = 0;
	virtual void sendMinorGiftTileImprovement(PlayerTypes eMajor, PlayerTypes eMinor, int iPlotX, int iPlotY) = 0;
	virtual void sendMinorBullyGold(PlayerTypes eBully, PlayerTypes eMinor, int iGold) = 0;
	virtual void sendMinorBullyUnit(PlayerTypes eBully, PlayerTypes eMinor, UnitTypes eUnitType) = 0;
	virtual void sendMinorBuyout(PlayerTypes eMajor, PlayerTypes eMinor) = 0;
	virtual void sendGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eUnit) = 0;
	virtual void sendMayaBonusChoice(PlayerTypes ePlayer, UnitTypes eUnit) = 0;
	virtual void sendFaithGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eUnit) = 0;
	virtual void sendSetCityAIFocus (int iCityID, CityAIFocusTypes eFocus) = 0;
	virtual void sendSetCityAvoidGrowth (int iCityID, bool bAvoidGrowth) = 0;
	virtual void sendUnitSyncCheck(PlayerTypes playerId, int unitId, FMemoryStream & syncArchiveData, const std::vector<std::pair<std::string, std::string> > &) const = 0;
	virtual void sendPlotSyncCheck(PlayerTypes playerId, short plotX, short plotY, FMemoryStream & syncArchiveData, const std::vector<std::pair<std::string, std::string> > &) const = 0;
	virtual void sendCitySyncCheck(PlayerTypes playerId, int cityId, FMemoryStream & syncArchiveData, const std::vector<std::pair<std::string, std::string> > &) const = 0;
	virtual void sendRandomNumberGeneratorSyncCheck(PlayerTypes playerId, ICvRandom1* pRandomNumberGenerator) const = 0;
	virtual void sendPlayerSyncCheck(PlayerTypes playerId, FMemoryStream & syncData, const std::vector<std::pair<std::string, std::string> > &) const = 0;
	virtual void sendFromUIDiploEvent(PlayerTypes eOtherPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2) = 0;
	virtual void sendNetDealAccepted(PlayerTypes fromPlayer, PlayerTypes toPlayer, ICvDeal1* pDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering) const = 0;
	virtual void sendNetDemandAccepted(PlayerTypes fromPlayer, PlayerTypes toPlayer, ICvDeal1* pDeal) const = 0;
	virtual void sendTurnReminder(PlayerTypes toPlayer) = 0;
	virtual void sendGoodyChoice(PlayerTypes ePlayerID, int iPlotX, int iPlotY, GoodyTypes eGoody, int iUnitID) = 0;
	virtual void sendSetSwappableGreatWork(PlayerTypes ePlayer, int iWorkClass, int iWorkIndex) = 0;
	virtual void sendSwapGreatWorks(PlayerTypes ePlayer1, int iWork1, PlayerTypes ePlayer2, int iWork2) = 0;
	virtual void sendMoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1, 
																											 int iCity2, int iBuildingClass2, int iWorkIndex2) = 0;
	virtual void netDisconnect() = 0;
	virtual void sendPlayerInitialAIProcessed() const = 0;
	virtual void hotJoinComplete() const = 0;
	virtual bool allAIProcessedThisTurn() const = 0;
	virtual int getMillisecsPerTurn() = 0;
	virtual float getSecsPerTurn() = 0;
	virtual int getTurnsPerSecond() = 0;
	virtual int getTurnsPerMinute() = 0;

	virtual bool CanAdvanceTurn() = 0;

	virtual void openSlot(PlayerTypes eID) = 0;
	virtual void closeSlot(PlayerTypes eID) = 0;

	virtual CvString getMapScriptName() = 0;
	virtual bool getTransferredMap() = 0;
	virtual bool isWBMapScript() = 0;
	virtual bool isWBMapNoPlayers() = 0;
	
	virtual CvString getPlayerName(int iIndex, uint uiForm = 0) = 0;
	virtual CvString getPlayerNameKey(int iIndex) = 0;
	virtual CvString getPlayerDisplayNickName(int iIndex) = 0;
	virtual CvString getCivDescription(int iIndex, uint uiForm = 0) = 0;
	virtual CvString getCivDescriptionKey(int iIndex) = 0;
	virtual CvString getCivShortDesc(int iIndex, uint uiForm = 0) = 0;
	virtual CvString getCivShortDescKey(int iIndex) = 0;
	virtual CvString getCivAdjective(int iIndex, uint uiForm = 0) = 0;
	virtual CvString getCivAdjectiveKey(int iIndex) = 0;
	virtual void stripSpecialCharacters(CvString& szName) = 0;

	virtual void InitGlobals() = 0;
	virtual void UninitGlobals() = 0;

	virtual void SetDone(bool bDone) = 0;
	virtual bool GetDone() = 0;
	virtual bool GetAutorun() = 0;

	virtual int GetAudioTagIndex(const char* szTag, int iScriptType = -1) = 0;

	virtual bool altKey() = 0;
	virtual bool shiftKey() = 0;
	virtual bool ctrlKey() = 0;

	typedef FFastList<char*, c_eMPoolTypeGame, 0> EnumeratedFilesList;
	virtual void EnumerateFiles(EnumeratedFilesList& files, 
								const char* szFolder, const size_t lenFolder, 
								const char* szPattern, const size_t lenPattern, 
								const bool bRecursive = false) = 0;

	virtual void ReleaseEnumeratedFilesList(EnumeratedFilesList &files) = 0;

	virtual void SaveGame(SaveGameTypes eSaveGame) = 0;
	virtual void LoadGame() = 0;
	virtual void AutoSave(bool bInitial = false, bool bPostTurn = false) = 0;
	virtual bool saveReplay() = 0;
	virtual void QuickSave() = 0;
	virtual void QuickLoad() = 0;
	virtual void sendPbemTurn(PlayerTypes ePlayer) = 0;
	virtual void getPassword(PlayerTypes ePlayer) = 0;
	virtual void RestartGame() = 0;

	virtual bool getPlayerOption(PlayerOptionTypes ePlayerOption) = 0;
	
	virtual const char* GetCacheFolderPath() = 0;

	virtual void GameplayEraChanged(PlayerTypes playerID, EraTypes newEra) = 0;

	virtual void GameplayBridgeChanged(bool bBridge, unsigned char newBridgeStyle) = 0;

	virtual void GameplayUnitCreated(_In_ ICvUnit1* pThisUnit) = 0;
	virtual void GameplayUnitMoved(_In_ ICvUnit1* pThisUnit, const CvPlotIndexVector& kPlots) = 0;
	virtual void GameplayUnitTeleported(_In_ ICvUnit1* pThisUnit, _In_ ICvPlot1* pThisPlot) = 0;
	virtual void GameplayUnitDestroyed(_In_ ICvUnit1* pThisUnit) = 0;
	virtual void GameplayUnitDestroyedInCombat(_In_ ICvUnit1* pUnit) = 0;
	virtual void GameplayUnitSetDamage(_In_ ICvUnit1* pThisUnit, int iDamage, int iPreviousDamage) = 0;
	virtual void GameplayUnitFortify(_In_ ICvUnit1* pThisUnit, bool bFortify) = 0;
	virtual void GameplayUnitWork(_In_ ICvUnit1* pThisUnit, int iWorkType) = 0;
	virtual void GameplayUnitParadrop(_In_ ICvUnit1* pThisUnit) = 0;
	virtual void GameplayUnitActivate(_In_ ICvUnit1* pThisUnit) = 0;
	virtual void GameplayUnitEmbark(_In_ ICvUnit1* pThisUnit, bool bEmbark) = 0;
	virtual void GameplayUnitGarrison(_In_ ICvUnit1* pThisUnit, bool bGarrison) = 0;
	virtual void GameplayUnitShouldDimFlag(_In_ ICvUnit1* pThisUnit, bool bDim) = 0;
	virtual void GameplayUnitMarkThreatening(_In_ ICvUnit1* pThisUnit, bool bMark) = 0;
	virtual void GameplayUnitVisibility(_In_ ICvUnit1* pThisUnit, bool bVisible, bool bChangeInvisibility = false, const float fBlendTime = 0.5f) = 0;
	virtual uint GameplayUnitCombat(_In_ ICvCombatInfo1* pCombatInfo) = 0;
	virtual void GameplayCityCreated(_In_ ICvCity1* pThisCity, EraTypes eCurrentEra) = 0;
	virtual void GameplayCityDestroyed(_In_ ICvCity1* pThisCity, PlayerTypes eNewOwner) = 0;
	virtual void GameplayCityCaptured(_In_ ICvCity1* pThisCity, PlayerTypes eNewOwner) = 0;
	virtual void GameplayCitySetDamage(_In_ ICvCity1* pThisCity, int iDamage, int iPreviousDamage) = 0;
	virtual void GameplayCityPopulationChanged(_In_ ICvCity1* pThisCity, int newPopulation) = 0;
	virtual uint GameplayCityCombat(_In_ ICvCombatInfo1* pInfo) = 0;
	virtual void GameplayUnitMissionEnd(_In_ ICvUnit1* pThisUnit) = 0;
	virtual void GameplayUnitRebased(_In_ ICvUnit1* pThisUnit, _In_ ICvPlot1* pOldPlot, _In_ ICvPlot1* pThisPlot) = 0;
	virtual void GameplayUnitResetAnimationState(_In_ ICvUnit1* pThisUnit) = 0;

	virtual void GameplayTechAcquired(TeamTypes eTeam, TechTypes eTech) = 0;

	virtual void GameplayFeatureChanged(_In_ ICvPlot1* pThisPlot, FeatureTypes newFeature) = 0;

	// the last set of functions will shortly be deprecated, this is the replacement
	virtual void GameplayPlotStateChange(_In_ const ICvPlot1* pThisPlot, const ResourceTypes eRevealedResource, const ImprovementTypes eRevealedImprovement, 
        const int iRevealedImprovementState, const RouteTypes eRevealedRoute, const int iRouteState) = 0;
    virtual void GameplayPlotIconStateChange(_In_ const ICvPlot1* pThisPlot, const ResourceTypes eRevealedResource, 
        const ImprovementTypes eRevealedImprovement, const int iRevealedImprovementState) = 0;

	virtual void GameplayNaturalWonderRevealed(_In_ ICvPlot1* pThisPlot) = 0;
	virtual void GameplayWallCreated(_In_ ICvPlot1* pThisPlot) = 0;
	virtual void GameplayDoFX(_In_opt_ ICvPlot1* pThisPlot, int iFXType = -1) = 0;

	virtual void GameplayFOWChanged(int x, int y, int eFOWType, bool bSetAll) = 0;

	virtual void GameplayYieldMightHaveChanged(_In_ ICvPlot1* pThisPlot) = 0;

	virtual void NotifyAILeadersInGame() = 0;
	virtual void NotifySpecificAILeaderInGame(PlayerTypes ePlayer) = 0;
	virtual void GameplayWarStateChanged(TeamTypes eTeam1, TeamTypes eTeam2, bool bWar) = 0;
	virtual void DoClearDiplomacyTradeTable() = 0;
	virtual void GameplayDiplomacyAILeaderMessage(PlayerTypes ePlayer, DiploUIStateTypes eDiploUIState, const char* szLeaderMessage, LeaderheadAnimationTypes eAction, int iData1 = -1) = 0;
	virtual void GameplayMetTeam(TeamTypes eMyTeam, TeamTypes eTheirTeam) = 0;
	virtual void NetMessageDebug(const std::string &) const = 0;

	virtual void endTurnTimerLength(float lengthInSeconds) = 0;
	virtual float endTurnTimerLength() const = 0;

	virtual void GameplayWonderCreated(PlayerTypes ePlayerID, _In_ ICvPlot1* pThisPlot, BuildingTypes eBuildingIndex, int iState) = 0;
	virtual void GameplayWonderEdited(PlayerTypes ePlayerID, _In_ ICvPlot1* pThisPlot, BuildingTypes eBuildingIndex, int iState) = 0;
	virtual void GameplayWonderRemoved(PlayerTypes ePlayerID, _In_ ICvPlot1* pThisPlot, BuildingTypes eBuildingIndex) = 0;

	virtual void GameplaySpaceshipCreated(_In_ ICvPlot1* pThisPlot, int iState) = 0;
	virtual void GameplaySpaceshipEdited(_In_ ICvPlot1* pThisPlot, int iState) = 0;
	virtual void GameplaySpaceshipRemoved(_In_ ICvPlot1* pThisPlot) = 0;

	virtual void GameplayOpenOptionsScreen() = 0;
	virtual void GameplaySearchForPediaEntry(const char * c) = 0;
	virtual void GameplayOpenInfoCorner(int iInfoCornerID) = 0;

	virtual void GameplayWorldAnchor(GenericWorldAnchorTypes eAnchor, bool bAdd, int iX, int iY, int iData1) = 0;

	virtual void GameplayMinimapUnitSelect(int iX, int iY) = 0;
	virtual void GameplayMinimapNotification(int iX, int iY, int iID) = 0;

	virtual void GameplayGoldenAgeStarted() = 0;
	virtual void GameplayGoldenAgeEnded() = 0;

	virtual	void GameplayTurnChanged(int iNewTurn) = 0;
	virtual	void GameplayActivePlayerChanged(PlayerTypes eNewPlayer) = 0;

	virtual void PublishActivePlayer(PlayerTypes ePlayer, PlayerTypes eOldPlayer) = 0;
	virtual void PublishNewGameTurn(int gameTurn) = 0;

	virtual void UnlockAchievement(EAchievement eAchievement) = 0;
	virtual bool IsAchievementUnlocked(EAchievement eAchievement) const = 0;

	virtual bool GetSteamStat(ESteamStat eStat, int32 * pStat) const = 0;
	virtual bool SetSteamStat(ESteamStat eStat, int32 nStat) = 0;

	virtual bool IncrementSteamStat(ESteamStat eStat) = 0;
	virtual bool IncrementSteamStatAndUnlock(ESteamStat eStat, int nThreshold, EAchievement eAchievement) = 0;

	virtual void SetAdvisorBadAttackInterrupt (bool bValue) = 0;
	virtual bool GetAdvisorBadAttackInterrupt (void) = 0;
	virtual void SetAdvisorCityAttackInterrupt (bool bValue) = 0;
	virtual bool GetAdvisorCityAttackInterrupt (void) = 0;

	virtual int  GetTutorialLevel (void) = 0;

	virtual void ReseatPlayer(PlayerTypes p, bool bClosePlayersOldSlot=false) const = 0;
	virtual bool IsNetPlayer(PlayerTypes p) const = 0;
	virtual void SendGameDoTurnProcessed() const = 0;

	virtual bool RecordVictoryInformation(int score) = 0;
	virtual bool RecordLeaderboardScore(int score) = 0;

	virtual bool TunerConnected() = 0;

	// Returns true if the tuner has been connected during this game session.
	// Use the CvGame version of this function to check the save as well.
	virtual bool TunerEverConnected() = 0;

    virtual void PublishEraChanges() = 0;

	virtual void RecordGameCoreMessages(bool bRecord) = 0;
	virtual bool IsProcessingGameCoreMessages() = 0;

	virtual	void GetGameCoreLock() = 0;
	virtual	void ReleaseGameCoreLock() = 0;
	virtual	bool TryGameCoreLock() = 0;
	virtual	bool HasGameCoreLock() = 0;
	virtual	bool IsGameCoreThread() = 0;
	virtual	bool IsGameCoreExecuting() = 0;
};

class ICvEngineUtility2 : public ICvEngineUtility1
{
public:
	using ICvEngineUtility1::sendPurchase;
	static GUID DLLCALL GetInterfaceId() { return guidICvEngineUtility2; }
	virtual void sendPurchase(int iCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, int ePurchaseYield) = 0;
	virtual bool ReseatConnectedPlayer(PlayerTypes p) const = 0;
};

class ICvEngineUtility3 : public ICvEngineUtility2
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvEngineUtility3; }
	virtual void SendLeagueVoteEnact(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice) = 0;
	virtual void SendLeagueVoteRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice) = 0;
	virtual void SendLeagueVoteAbstain(LeagueTypes eLeague, PlayerTypes eVoter, int iNumVotes) = 0;
	virtual void SendLeagueProposeEnact(LeagueTypes eLeague, ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice) = 0;
	virtual void SendLeagueProposeRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eProposer) = 0;
	virtual void SendLeagueEditName(LeagueTypes eLeague, PlayerTypes ePlayer, const char* szCustomName) = 0;

	virtual void sendChangeIdeology() = 0;

	virtual void TradeVisuals_NewRoute(int iRoute, int iPlayer, TradeConnectionType type, int nPlots, int x[], int y[]) = 0;
	virtual void TradeVisuals_UpdateRouteDirection(int iRoute, bool bForwards) = 0;
	virtual void TradeVisuals_DestroyRoute(int iRoute, int iPlayer) = 0;
	virtual void TradeVisuals_ActivatePopupRoute(int iRoute) = 0;
	virtual void TradeVisuals_DeactivatePopupRoute() = 0;
	
	virtual void FlushTurnReminders() = 0;

	virtual void BeginSendBundle() = 0;
	virtual void EndSendBundle() = 0;

	virtual bool IsPlayerKicked(int iPlayerID) = 0;
	virtual bool IsPlayerHotJoining(int iPlayerID) = 0;

	virtual bool allAICivsProcessedThisTurn() const = 0;
	virtual void SendAICivsProcessed() const = 0;

	virtual bool HasReceivedAICivsProcessed(PlayerTypes ePlayer) const = 0;
	virtual int	 GetNumAICivsProcessed() const = 0;

	virtual void VerifyPlayerSlot(PlayerTypes ePlayer) const = 0;

	virtual bool ShouldValidateGameDatabase() const = 0;
};

class ICvEngineUtility4 : public ICvEngineUtility3
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvEngineUtility4; }

	virtual bool DLLCALL GetEvaluatedMapScriptPath(_In_z_ const char* szPath, _Inout_z_cap_(szEvaluatedPathSize) char* szEvaluatedPath, unsigned int szEvaluatedPathSize) const = 0;
};

#endif	// ICvDLLUtility_h
