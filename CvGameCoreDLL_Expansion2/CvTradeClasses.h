/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TRADE_CLASSES_H
#define CIV5_TRADE_CLASSES_H

// The map layer the trade units reside on
#define TRADE_UNIT_MAP_LAYER	1

struct TradeConnectionPlot
{
	int m_iX;
	int m_iY;
};

#define PROJECTED_MAX_TRADE_LENGTH 40
typedef FStaticVector<TradeConnectionPlot, PROJECTED_MAX_TRADE_LENGTH, false, c_eCiv5GameplayDLL> TradeConnectionPlotList;

struct TradeConnection
{
	int m_iID;
	int m_iOriginX;
	int m_iOriginY;
	int m_iDestX;
	int m_iDestY;
	PlayerTypes m_eOriginOwner;
	PlayerTypes m_eDestOwner;
	DomainTypes m_eDomain;
	TradeConnectionType m_eConnectionType;
	int m_iTradeUnitLocationIndex;
	bool m_bTradeUnitMovingForward;
	TradeConnectionPlotList m_aPlotList;
	int m_unitID;
	int m_iCircuitsCompleted;
	int m_iCircuitsToComplete;
	int m_iTurnRouteComplete;
	int m_aiOriginYields[NUM_YIELD_TYPES];
	int m_aiDestYields[NUM_YIELD_TYPES];
};

#define PROJECTED_MAX_TRADE_CONNECTIONS_PER_CIV 14
#define LIKELY_NUM_OF_PLAYERS 12
#define PROJECTED_MAX_TRADE_CONNECTIONS (PROJECTED_MAX_TRADE_CONNECTIONS_PER_CIV * LIKELY_NUM_OF_PLAYERS)

typedef FStaticVector<TradeConnection, PROJECTED_MAX_TRADE_CONNECTIONS, false, c_eCiv5GameplayDLL > TradeConnectionList;

class CvGameTrade
{
public:
	CvGameTrade(void);
	~CvGameTrade(void);

	void Init (void);
	void Uninit (void);
	void Reset (void);

	void DoTurn (void);

	bool CanCreateTradeRoute (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath = true);
	bool CanCreateTradeRoute(PlayerTypes eOriginPlayer, PlayerTypes eDestPlayer, DomainTypes eDomainRestriction);
	bool CreateTradeRoute (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, int& iRouteID);

	bool IsValidTradeRoutePath (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain);
	CvPlot* GetPlotAdjacentToWater (CvPlot* pTarget, CvPlot* pOrigin);

	bool IsDestinationExclusive(const TradeConnection& kTradeConnection);
	bool IsConnectionInternational (const TradeConnection& kTradeConnection);

	bool IsCityConnectedToPlayer (CvCity* pCity, PlayerTypes eOtherPlayer, bool bOnlyOwnedByCityOwner);
	bool IsPlayerConnectedToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer);
	int CountNumPlayerConnectionsToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer);

	bool IsCityConnectedToCity (CvCity* pFirstCity, CvCity* pSecondCity);
	bool IsCityConnectedFromCityToCity (CvCity* pOriginCity, CvCity* pDestCity);

	int GetNumTimesOriginCity (CvCity* pCity, bool bOnlyInternational);
	int GetNumTimesDestinationCity (CvCity* pCity, bool bOnlyInternational);

	void CopyPathIntoTradeConnection (CvAStarNode* pNode, TradeConnection* pTradeConnection);

	int GetDomainModifierTimes100 (DomainTypes eDomain);

	int GetEmptyTradeRouteIndex (void);
	bool IsTradeRouteIndexEmpty (int iIndex);
	bool EmptyTradeRoute (int iIndex);

	void ClearAllCityTradeRoutes (CvPlot* pPlot); // called when a city is captured or traded
	void ClearAllCivTradeRoutes (PlayerTypes ePlayer); // called from world congress code
	void ClearAllCityStateTradeRoutes (void); // called from world congress code
	void CancelTradeBetweenTeams (TeamTypes eTeam1, TeamTypes eTeam2);

	void DoAutoWarPlundering(TeamTypes eTeam1, TeamTypes eTeam2); // when war is declared, both sides plunder each others trade routes for cash!

	int GetNumTradeRoutesInPlot (CvPlot* pPlot);

	int GetIndexFromID (int iID);
	PlayerTypes GetOwnerFromID (int iID);
	PlayerTypes GetDestFromID (int iID);
	
	int GetIndexFromUnitID(int iUnitID, PlayerTypes eOwner);
	bool IsUnitIDUsed (int iUnitID);

	static CvCity* GetOriginCity(const TradeConnection& kTradeConnection);
	static CvCity* GetDestCity(const TradeConnection& kTradeConnection);

	void ResetTechDifference ();
	void BuildTechDifference ();
	int GetTechDifference (PlayerTypes ePlayer, PlayerTypes ePlayer2);

	void CreateVis (int iIndex); // Create the trade unit vis unit
	CvUnit* GetVis(int iIndex);
	// trade unit movement
	bool MoveUnit (int iIndex); // move a trade unit along its path for all its movement points
	bool StepUnit (int iIndex); // move a trade unit a single step along its path (called by MoveUnit)

	void DisplayTemporaryPopupTradeRoute(int iPlotX, int iPlotY, TradeConnectionType type, DomainTypes eDomain);
	void HideTemporaryPopupTradeRoute(int iPlotX, int iPlotY, TradeConnectionType type);

	CvString GetLogFileName() const;
	void LogTradeMsg(CvString& strMsg);

	TradeConnectionList m_aTradeConnections;
	int m_iNextID; // used to assign IDs to trade routes to avoid confusion when some are disrupted in multiplayer

	int m_aaiTechDifference[MAX_MAJOR_CIVS][MAX_MAJOR_CIVS];

	struct {
		int iPlotX, iPlotY;
		TradeConnectionType type;
	} m_CurrentTemporaryPopupRoute;
};

FDataStream& operator>>(FDataStream&, CvGameTrade&);
FDataStream& operator<<(FDataStream&, const CvGameTrade&);

struct TradeConnectionWasPlundered
{
	TradeConnection m_kTradeConnection;
	int m_iTurnPlundered;
};

typedef FStaticVector<TradeConnectionWasPlundered, 10, false, c_eCiv5GameplayDLL > TradeConnectionWasPlunderedList;

class CvPlayerTrade
{
public:
	CvPlayerTrade(void);
	~CvPlayerTrade(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);

	// Functions invoked each player turn
	void DoTurn(void);
	void MoveUnits(void);

	int GetTradeConnectionBaseValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionGPTValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer, bool bOriginCity);
	int GetTradeConnectionResourceValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionYourBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionTheirBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionExclusiveValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionPolicyValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionOtherTraitValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionDomainValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionRiverValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionValueTimes100 (const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	void UpdateTradeConnectionValues (void); // updates the all the values for the trade routes that go to and from this player

	int GetTradeValuesAtCityTimes100 (const CvCity* const pCity, YieldTypes eYield);

	int GetAllTradeValueTimes100 (YieldTypes eYield);
	int GetAllTradeValueFromPlayerTimes100 (YieldTypes eYield, PlayerTypes ePlayer);

	bool IsConnectedToPlayer(PlayerTypes eOtherPlayer);

	bool CanCreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath = true);
	bool CanCreateTradeRoute(PlayerTypes eOtherPlayer, DomainTypes eDomain);
	bool CanCreateTradeRoute(DomainTypes eDomain);

	bool CreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType);

	TradeConnection* GetTradeConnection(CvCity* pOriginCity, CvCity* pDestCity);
	int GetNumberOfCityStateTradeRoutes();

	bool IsPreviousTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType);

	int GetNumPotentialConnections (CvCity* pFromCity, DomainTypes eDomain);

	std::vector<int> GetTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound, bool bExcludingMe, bool bOnlyWar);
	std::vector<int> GetTradePlotsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound, bool bExcludingMe, bool bOnlyWar);

	std::vector<int> GetOpposingTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound); // returns the ID of trade connections with units at that plot
	bool ContainsOpposingPlayerTradeUnit(const CvPlot* pPlot);

	std::vector<int> GetEnemyTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound); // returns the ID of trade connections with units at that plot
	bool ContainsEnemyTradeUnit(const CvPlot* pPlot);
	std::vector<int> GetEnemyTradePlotsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound); // returns the ID of trade connections that go through that plot
	bool ContainsEnemyTradePlot(const CvPlot* pPlot);

	bool PlunderTradeRoute(int iTradeConnectionID);

	int GetTradeRouteRange (DomainTypes eDomain, CvCity* pOriginCity);
	int GetTradeRouteSpeed (DomainTypes eDomain);

	uint GetNumTradeRoutesPossible (void);
	int GetNumTradeRoutesUsed (bool bContinueTraining);
	int GetNumTradeRoutesRemaining (bool bContinueTraining);

	int GetNumDifferentTradingPartners (void);

	void UpdateTradeConnectionWasPlundered();
	void AddTradeConnectionWasPlundered(const TradeConnection kTradeConnection);
	bool CheckTradeConnectionWasPlundered(const TradeConnection& kTradeConnection);

	static UnitTypes GetTradeUnit (DomainTypes eDomain);

	std::vector<CvString> GetPlotToolTips (CvPlot* pPlot);
	std::vector<CvString> GetPlotMouseoverToolTips (CvPlot* pPlot);

	TradeConnectionList m_aRecentlyExpiredConnections;
	TradeConnectionWasPlunderedList m_aTradeConnectionWasPlundered;

	CvPlayer* m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvPlayerTrade&);
FDataStream& operator<<(FDataStream&, const CvPlayerTrade&);

class CvTradeAI
{
public:
	CvTradeAI(void);
	~CvTradeAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);

	void DoTurn(void);

	void GetAvailableTR(TradeConnectionList& aTradeConnectionList);
	void PrioritizeTradeRoutes(TradeConnectionList& aTradeConnectionList);
	int	ScoreInternationalTR (const TradeConnection& kTradeConnection);
	int ScoreFoodTR(const TradeConnection& kTradeConnection, CvCity* pSmallestCity);
	int ScoreProductionTR (const TradeConnection& kTradeConnection, std::vector<CvCity*> aTargetCityList);

	bool ChooseTradeUnitTargetPlot(CvUnit* pUnit, int& iOriginPlotIndex, int& iDestPlotIndex, TradeConnectionType& eTradeConnectionType, bool& bDisband, const TradeConnectionList& aTradeConnections);

	int m_iRemovableValue;

	CvPlayer* m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvTradeAI&);
FDataStream& operator<<(FDataStream&, const CvTradeAI&);

#endif //CIV5_TRADE_CLASSES_H