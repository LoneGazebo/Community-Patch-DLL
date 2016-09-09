/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllGame : public ICvGame2
{
public:
	CvDllGame(CvGame* pGame);
	~CvDllGame();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	ICvPlayer1* DLLCALL GetPlayer(const PlayerTypes ePlayer);

	CvGame* GetInstance();

	PlayerTypes DLLCALL GetActivePlayer();
	TeamTypes DLLCALL GetActiveTeam();
	int DLLCALL GetGameTurn() const;

	void DLLCALL ChangeNumGameTurnActive(int iChange, const char* why);
	int DLLCALL CountHumanPlayersAlive() const;
	int DLLCALL CountNumHumanGameTurnActive();
	bool DLLCALL CyclePlotUnits(ICvPlot1* pPlot, bool bForward = true, bool bAuto = false, int iCount = -1);
	void DLLCALL CycleUnits(bool bClear, bool bForward = true, bool bWorkers = false);
	void DLLCALL DoGameStarted();
	void DLLCALL EndTurnTimerReset();
	void DLLCALL EndTurnTimerSemaphoreDecrement();
	void DLLCALL EndTurnTimerSemaphoreIncrement();
	int DLLCALL GetAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl);
	int DLLCALL IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl);
	ImprovementTypes DLLCALL GetBarbarianCampImprovementType();
	int DLLCALL GetElapsedGameTurns() const;
	bool DLLCALL GetFOW();
	ICvGameDeals1* DLLCALL GetGameDeals();
	GameSpeedTypes DLLCALL GetGameSpeedType() const;
	GameStateTypes DLLCALL GetGameState();
	HandicapTypes DLLCALL GetHandicapType() const;
	ICvRandom1* DLLCALL GetRandomNumberGenerator();
	int DLLCALL GetJonRandNum(int iNum, const char* pszLog);
	int DLLCALL GetMaxTurns() const;
	int DLLCALL GetNumGameTurnActive();
	PlayerTypes DLLCALL GetPausePlayer();
	bool DLLCALL GetPbemTurnSent() const;
	ICvUnit1* DLLCALL GetPlotUnit(ICvPlot1* pPlot, int iIndex);

	unsigned int DLLCALL GetVariableCitySizeFromPopulation(unsigned int nPopulation);
	VictoryTypes DLLCALL GetVictory() const;
	int DLLCALL GetVotesNeededForDiploVictory() const;
	TeamTypes DLLCALL GetWinner() const;
	int DLLCALL GetWinningTurn() const;
	void DLLCALL HandleAction(int iAction);
	bool DLLCALL HasTurnTimerExpired();
	bool DLLCALL HasTurnTimerExpired(PlayerTypes playerID);
	void DLLCALL TurnTimerSync(float fCurTurnTime, float fTurnStartTime);
	void DLLCALL GetTurnTimerData(float& fCurTurnTime, float& fTurnStartTime);
	void DLLCALL Init(HandicapTypes eHandicap);
	bool DLLCALL Init2();
	void DLLCALL InitScoreCalculation();
	void DLLCALL InitTacticalAnalysisMap(int iNumPlots);
	bool DLLCALL IsCityScreenBlocked();
	bool DLLCALL CanOpenCityScreen(PlayerTypes eOpener, ICvCity1* pCity);
	bool DLLCALL IsDebugMode() const;
	bool DLLCALL IsFinalInitialized() const;
	bool DLLCALL IsGameMultiPlayer() const;
	bool DLLCALL IsHotSeat() const;
	bool IsMPOption(MultiplayerOptionTypes eIndex) const;
	bool DLLCALL IsNetworkMultiPlayer() const;
	bool IsOption(GameOptionTypes eIndex) const;
	bool DLLCALL IsPaused();
	bool DLLCALL IsPbem() const;
	bool DLLCALL IsTeamGame() const;
	void DLLCALL LogGameState(bool bLogHeaders = false);
	void DLLCALL ResetTurnTimer();
	void DLLCALL SelectAll(ICvPlot1* pPlot);
	void DLLCALL SelectGroup(ICvUnit1* pUnit, bool bShift, bool bCtrl, bool bAlt);
	bool DLLCALL SelectionListIgnoreBuildingDefense();
	void DLLCALL SelectionListMove(ICvPlot1* pPlot, bool bShift);
	void DLLCALL SelectSettler(void);
	void DLLCALL SelectUnit(ICvUnit1* pUnit, bool bClear, bool bToggle = false, bool bSound = false);
	void DLLCALL SendPlayerOptions(bool bForce = false);
	void DLLCALL SetDebugMode(bool bDebugMode);
	void DLLCALL SetFinalInitialized(bool bNewValue);
	void DLLCALL SetInitialTime(unsigned int uiNewValue);
	void SetMPOption(MultiplayerOptionTypes eIndex, bool bEnabled);
	void DLLCALL SetPausePlayer(PlayerTypes eNewValue);
	void DLLCALL SetTimeStr(_Inout_z_cap_c_(256) char* szString, int iGameTurn, bool bSave);
	bool DLLCALL TunerEverConnected() const;
	void DLLCALL Uninit();
	void DLLCALL UnitIsMoving();
	void DLLCALL Update();
	void DLLCALL UpdateSelectionList();
	void DLLCALL UpdateTestEndTurn();

	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream) const;
	void DLLCALL ReadSupportingClassData(FDataStream& kStream);
	void DLLCALL WriteSupportingClassData(FDataStream& kStream) const;
	void DLLCALL WriteReplay(FDataStream& kStream) const;

	bool DLLCALL CanMoveUnitTo(ICvUnit1* pUnit, ICvPlot1* pPlot) const;

	void DLLCALL NetMessageStaticsReset();
	void DLLCALL SetLastTurnAICivsProcessed();

	bool DLLCALL GetGreatWorkAudio(int GreatWorkIndex, char* strSound, int length);

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvGame* m_pGame;
};
