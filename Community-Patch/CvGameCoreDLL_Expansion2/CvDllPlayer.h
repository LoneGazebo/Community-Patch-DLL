/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllPlayer : public ICvPlayer3
{
public:
	CvDllPlayer(_In_ CvPlayerAI* pPlayer);
	~CvDllPlayer();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvPlayerAI* GetInstance();

	void DLLCALL Init(PlayerTypes eID);
	void DLLCALL SetupGraphical();
	void DLLCALL LaterInit();
	void DLLCALL Uninit();
	bool DLLCALL IsHuman() const;
	bool DLLCALL IsBarbarian() const;
	const char* DLLCALL GetName() const;
	const char* const DLLCALL GetNickName() const;
	ArtStyleTypes DLLCALL GetArtStyleType() const;
	ICvUnit1* DLLCALL GetFirstReadyUnit();
	bool DLLCALL HasBusyUnit() const;
	bool DLLCALL HasBusyUnitOrCity() const;
	int DLLCALL GetScore(bool bFinal = false, bool bVictory = false) const;
	ICvPlot1* DLLCALL GetStartingPlot() const;
	ICvCity1* DLLCALL GetCapitalCity();
	bool DLLCALL IsHasLostCapital() const;
	void DLLCALL SetStartTime(uint uiStartTime);
	bool DLLCALL IsMinorCiv() const;
	bool DLLCALL IsAlive() const;
	bool DLLCALL IsEverAlive() const;
	bool DLLCALL IsTurnActive() const;
	void DLLCALL SetTurnActive(bool bNewValue, bool bDoTurn = true);
	bool DLLCALL IsSimultaneousTurns() const;
	PlayerTypes DLLCALL GetID() const;
	HandicapTypes DLLCALL GetHandicapType() const;
	CivilizationTypes DLLCALL GetCivilizationType() const;
	LeaderHeadTypes DLLCALL GetLeaderType() const;
	const char* DLLCALL GetLeaderTypeKey() const;
	EraTypes DLLCALL GetCurrentEra() const;
	TeamTypes DLLCALL GetTeam() const;
	PlayerColorTypes DLLCALL GetPlayerColor() const;
	bool DLLCALL IsOption(PlayerOptionTypes eIndex) const;
	ICvCity1* DLLCALL FirstCity(int* pIterIdx, bool bRev = false);
	ICvCity1* DLLCALL NextCity(int* pIterIdx, bool bRev = false);
	int DLLCALL GetNumCities() const;
	ICvCity1* DLLCALL GetCity(int iID);
	ICvUnit1* DLLCALL GetUnit(int iID);
	ICvUnit1* DLLCALL FirstUnit(int* pIterIdx, bool bRev = false);
	ICvUnit1* DLLCALL NextUnit(int* pIterIdx, bool bRev = false);
	int DLLCALL GetPlotDanger(ICvPlot1* pPlot) const;
	int DLLCALL GetCityDistanceHighwaterMark() const;
	void DLLCALL Disconnected();
	void DLLCALL Reconnected();
	void DLLCALL SetBusyUnitUpdatesRemaining(int iUpdateCount);
	bool DLLCALL HasUnitsThatNeedAIUpdate() const;

	int DLLCALL GetNumPolicyBranchesFinished() const;

	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream) const;

	FAutoArchive& DLLCALL GetSyncArchive();

	int DLLCALL GetGold() const;
	int DLLCALL CalculateBaseNetGold();
	const char* DLLCALL GetEmbarkedGraphicOverride();

	MinorCivTypes DLLCALL GetMinorCivType() const;
	ICvDiplomacyAI1* DLLCALL GetDiplomacyAI();
	ICvDealAI1* DLLCALL GetDealAI();
	bool DLLCALL MayNotAnnex();

	bool DLLCALL AddDiplomacyRequest(PlayerTypes ePlayerID, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1);
	void DLLCALL ActiveDiplomacyRequestComplete();

	int DLLCALL AddNotification(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData = -1);
	void DLLCALL ActivateNotification(int iLookupIndex);
	void DLLCALL DismissNotification(int iLookupIndex, bool bUserInvoked);
	bool DLLCALL MayUserDismissNotification(int iLookupIndex);
	void DLLCALL RebroadcastNotifications(void);

	// Techs
	bool DLLCALL GetCurrentResearchTech(TechTypes* pkTech, int *pkTurnsLeft) const;

	bool DLLCALL HasTurnTimerExpired();

	bool DLLCALL IsOptionKey(PlayerOptionTypes eOptionID) const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvPlayerAI* m_pPlayer;
};