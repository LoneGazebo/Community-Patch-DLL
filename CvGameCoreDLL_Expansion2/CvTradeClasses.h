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

#include "CvAStar.h"

struct TradeConnectionPlot
{
	int m_iX;
	int m_iY;
};

typedef std::vector<TradeConnectionPlot> TradeConnectionPlotList;

struct TradeConnection
{
	//default constructor
	TradeConnection()
	{
		m_iID = -1;
		m_iDestID = -1;
		m_iDestX = -1;
		m_iDestY = -1;
		m_eDestOwner = NO_PLAYER;
		m_iOriginID = -1;
		m_iOriginX = -1;
		m_iOriginY = -1;
		m_eOriginOwner = NO_PLAYER;
		m_eDomain = NO_DOMAIN;
		m_eConnectionType = NUM_TRADE_CONNECTION_TYPES;
		m_iTradeUnitLocationIndex = -1;
		m_bTradeUnitMovingForward = false;
		m_iSpeedFactor = 100;
		m_iCircuitsCompleted = 0;
		m_iCircuitsToComplete = 0;
		m_iTurnRouteComplete = 0;
		m_bTradeUnitRecalled = false;
		m_aPlotList.clear();
		m_unitID = -1;

		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			m_aiOriginYields[ui] = 0;
			m_aiDestYields[ui] = 0;
		}
	}

	void SetCities(const CvCity* pOriginCity, const CvCity* pDestCity)
	{
		m_iOriginID = pOriginCity->GetID();
		m_iOriginX = pOriginCity->getX();
		m_iOriginY = pOriginCity->getY();
		m_eOriginOwner = pOriginCity->getOwner();
		m_iDestID = pDestCity->GetID();
		m_iDestX = pDestCity->getX();
		m_iDestY = pDestCity->getY();
		m_eDestOwner = pDestCity->getOwner();
	}

	int GetMovementSpeed();

	int m_iID;
	int m_iOriginID;
	int m_iOriginX;
	int m_iOriginY;
	int m_iDestID;
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
	int m_iSpeedFactor; //move faster on this route?
	int m_iCircuitsCompleted;
	int m_iCircuitsToComplete;
	int m_iTurnRouteComplete;
	bool m_bTradeUnitRecalled;
	int m_aiOriginYields[NUM_YIELD_TYPES];
	int m_aiDestYields[NUM_YIELD_TYPES];
};

typedef std::vector<TradeConnection> TradeConnectionList;

typedef std::map<int,std::map<int,SPath>> TradePathLookup;

class CvGameTrade
{
public:
	CvGameTrade(void);
	~CvGameTrade(void);

	void Init (void);
	void Uninit (void);
	void Reset (void);

	void DoTurn (void);

	bool CanCreateTradeRoute (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath);
	bool CanCreateTradeRoute(PlayerTypes eOriginPlayer, PlayerTypes eDestPlayer, DomainTypes eDomainRestriction);
	bool CreateTradeRoute (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, int& iRouteID);

	bool IsValidTradeRoutePath (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, SPath* pPathOut=NULL, bool bWarCheck = true);
	int GetValidTradeRoutePathLength(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, SPath* pPathOut = NULL, bool bWarCheck = true);
	bool IsDestinationExclusive(const TradeConnection& kTradeConnection);
	bool IsConnectionInternational (const TradeConnection& kTradeConnection);

	bool IsCityConnectedToPlayer (CvCity* pCity, PlayerTypes eOtherPlayer, bool bOnlyOwnedByCityOwner);
	bool IsPlayerConnectedToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer, bool bFirstPlayerOnly = false);
	int CountNumPlayerConnectionsToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer);

	bool CitiesHaveTradeConnection (CvCity* pFirstCity, CvCity* pSecondCity);

	int GetNumTimesOriginCity (CvCity* pCity, bool bOnlyInternational);
	int GetNumTimesDestinationCity (CvCity* pCity, bool bOnlyInternational);

	void CopyPathIntoTradeConnection (const SPath& path, TradeConnection* pTradeConnection);

	int GetDomainModifierTimes100 (DomainTypes eDomain);

	int GetEmptyTradeRouteIndex (void);
	bool IsTradeRouteIndexEmpty (int iIndex);
	bool EmptyTradeRoute (int iIndex);
#if defined(MOD_BALANCE_CORE)
	void UpdateTradePlots();
	int GetTradeRouteTurns(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, int* piCircuitsToComplete = NULL);
#endif
#if defined(MOD_BUGFIX_MINOR)
	void ClearAllCityTradeRoutes (CvPlot* pPlot, bool bIncludeTransits = false); // called when a city is captured or traded
#else
	void ClearAllCityTradeRoutes (CvPlot* pPlot); // called when a city is captured or traded
#endif
	void ClearAllCivTradeRoutes (PlayerTypes ePlayer); // called from world congress code
	void ClearAllCityStateTradeRoutes (void); // called from world congress code
#if defined(MOD_BALANCE_CORE)
	void ClearAllCityStateTradeRoutesSpecial(void); // called from world congress code
	void ClearTradePlayerToPlayer(PlayerTypes ePlayer, PlayerTypes eToPlayer); // called from world congress code
#endif
	void CancelTradeBetweenTeams (TeamTypes eTeam1, TeamTypes eTeam2);

	void DoAutoWarPlundering(TeamTypes eTeam1, TeamTypes eTeam2); // when war is declared, both sides plunder each others trade routes for cash!

	int GetIndexFromID (int iID);
	PlayerTypes GetOwnerFromID (int iID);
	PlayerTypes GetDestFromID (int iID);
	
	int GetIndexFromUnitID(int iUnitID, PlayerTypes eOwner);
	bool IsUnitIDUsed (int iUnitID);
	const TradeConnection* GetConnectionFromIndex(int iIndex) const;

	static CvCity* GetOriginCity(const TradeConnection& kTradeConnection);
	static CvCity* GetDestCity(const TradeConnection& kTradeConnection);

	void ResetTechDifference ();
	void BuildTechDifference ();
	int GetTechDifference (PlayerTypes ePlayer, PlayerTypes ePlayer2);

	void ResetPolicyDifference();
	void BuildPolicyDifference();
	int GetPolicyDifference(PlayerTypes ePlayer, PlayerTypes ePlayer2);

	void CreateTradeUnitForRoute (int iIndex); // Create the trade unit vis unit
	CvUnit* GetTradeUnitForRoute(int iIndex);
#if defined(MOD_API_TRADEROUTES)
	bool IsRecalledUnit (int iIndex); // has the unit been recalled
	void RecallUnit (int iIndex, bool bImmediate = false); // recall a trade unit
	void EndTradeRoute (int iIndex); // end a trade route
#endif
	// trade unit movement
	bool MoveUnit (int iIndex); // move a trade unit along its path for all its movement points
	bool StepUnit (int iIndex); // move a trade unit a single step along its path (called by MoveUnit)

	void DisplayTemporaryPopupTradeRoute(int iPlotX, int iPlotY, TradeConnectionType type, DomainTypes eDomain);
	void HideTemporaryPopupTradeRoute(int iPlotX, int iPlotY, TradeConnectionType type);

	CvString GetLogFileName() const;
	void LogTradeMsg(CvString& strMsg);

	size_t GetNumTradeConnections() const { return m_aTradeConnections.size(); }
	const TradeConnection& GetTradeConnection(size_t iIndex) const { return m_aTradeConnections[iIndex]; }
	void SetOriginYields(int iConnection, int iYield, int iValue) { m_aTradeConnections[iConnection].m_aiOriginYields[iYield]=iValue; }
	void SetDestYields(int iConnection, int iYield, int iValue) { m_aTradeConnections[iConnection].m_aiDestYields[iYield]=iValue; }

	bool HavePotentialTradePath(bool bWater, CvCity* pOriginCity, CvCity* pDestCity, SPath* pPathOut=NULL);
	void UpdateTradePathCache(uint iOriginPlayer);
	void InvalidateTradePathCache(uint iPlayer);

protected:

	TradeConnectionList m_aTradeConnections;

	TradePathLookup m_aPotentialTradePathsLand;
	TradePathLookup m_aPotentialTradePathsWater;
	std::map<uint,int> m_lastTradePathUpdate;

	int m_iNextID; // used to assign IDs to trade routes to avoid confusion when some are disrupted in multiplayer
	int m_aaiTechDifference[MAX_MAJOR_CIVS][MAX_MAJOR_CIVS];
	int m_aaiPolicyDifference[MAX_MAJOR_CIVS][MAX_MAJOR_CIVS];

	struct {
		int iPlotX, iPlotY;
		TradeConnectionType type;
	} m_CurrentTemporaryPopupRoute;

	friend FDataStream& operator>>(FDataStream&, CvGameTrade&);
	friend FDataStream& operator<<(FDataStream&, const CvGameTrade&);
};

FDataStream& operator>>(FDataStream&, CvGameTrade&);
FDataStream& operator<<(FDataStream&, const CvGameTrade&);

struct TradeConnectionWasPlundered
{
	TradeConnection m_kTradeConnection;
	int m_iTurnPlundered;
};

typedef std::vector<TradeConnectionWasPlundered> TradeConnectionWasPlunderedList;

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
#if defined(MOD_BALANCE_CORE)
	int GetMinorCivGoldBonus(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#endif
	int GetTradeConnectionTheirBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionExclusiveValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionPolicyValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionOtherTraitValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionDomainValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionRiverValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#if defined(MOD_BALANCE_CORE)
	int GetTradeConnectionDistanceValueModifierTimes100(const TradeConnection& kTradeConnection);
	int GetTradeConnectionCorporationModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionOpenBordersModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	int GetTradeConnectionPolicyModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#endif

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

	const TradeConnection* GetTradeConnection(CvCity* pOriginCity, CvCity* pDestCity);
	int GetNumberOfCityStateTradeRoutes();
#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetNumberOfInternalTradeRoutes();
	int GetNumberOfInternationalTradeRoutes(bool bOutgoing);
	int GetNumberOfTradeRoutes();
#endif

	bool IsPreviousTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType);

	int GetNumPotentialConnections(CvCity* pFromCity, DomainTypes eDomain, bool bNeedOnlyOne);

	std::vector<int> GetTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound, bool bExcludingMe, bool bOnlyWar);
	std::vector<int> GetTradePlotsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound, bool bExcludingMe, bool bOnlyWar);

	std::vector<int> GetOpposingTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound); // returns the ID of trade connections with units at that plot
	bool ContainsOpposingPlayerTradeUnit(const CvPlot* pPlot);

	std::vector<int> GetEnemyTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound); // returns the ID of trade connections with units at that plot
	bool ContainsEnemyTradeUnit(const CvPlot* pPlot);
	std::vector<int> GetEnemyTradePlotsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound); // returns the ID of trade connections that go through that plot
	bool ContainsEnemyTradePlot(const CvPlot* pPlot);

#if defined(MOD_API_EXTENSIONS)
	bool PlunderTradeRoute(int iTradeConnectionID, CvUnit* pUnit);
#else
	bool PlunderTradeRoute(int iTradeConnectionID);
#endif
	void UpdateFurthestPossibleTradeRoute(DomainTypes eDomain, CvCity* pOriginCity, int iMaxRange);
	int GetTradeRouteRange (DomainTypes eDomain, CvCity* pOriginCity);
	int GetTradeRouteSpeed (DomainTypes eDomain);

	uint GetNumTradeRoutesPossible (void);
	int GetNumTradeUnits (bool bIncludeBeingBuilt);
	int GetNumTradeUnitsRemaining (bool bIncludeBeingBuilt);

	int GetNumDifferentTradingPartners (void);
#if defined(MOD_BALANCE_CORE)
	int GetNumDifferentMajorCivTradingPartners(void);
#endif

	void UpdateTradeConnectionWasPlundered();
	void AddTradeConnectionWasPlundered(const TradeConnection& kTradeConnection);
	bool CheckTradeConnectionWasPlundered(const TradeConnection& kTradeConnection);

#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	static UnitTypes GetTradeUnit (DomainTypes eDomain, CvPlayer* pPlayer);
#else
	static UnitTypes GetTradeUnit (DomainTypes eDomain);
#endif

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

	void GetAvailableTR(TradeConnectionList& aTradeConnectionList, bool bSkipExisting);
	void GetPrioritizedTradeRoutes(TradeConnectionList& aTradeConnectionList, bool bSkipExisting);

	std::vector<int> ScoreInternationalTR(const TradeConnection& kTradeConnection, bool bHaveTourism);

	//generic method
	int ScoreInternalTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);

	//wrapper for different types of trade route
	int ScoreFoodTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);
	int ScoreProductionTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	int ScoreWonderTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);
#endif

	int m_iRemovableValue;

	CvPlayer* m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvTradeAI&);
FDataStream& operator<<(FDataStream&, const CvTradeAI&);

#endif //CIV5_TRADE_CLASSES_H