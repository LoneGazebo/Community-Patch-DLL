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
	template<typename TradeConnectionPlotT, typename Visitor>
	static void Serialize(TradeConnectionPlotT& tradeConnectionPlot, Visitor& visitor);

	int m_iX;
	int m_iY;
};

FDataStream& operator<<(FDataStream&, const TradeConnectionPlot&);
FDataStream& operator>>(FDataStream&, TradeConnectionPlot&);

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

	template<typename TradeConnectionT, typename Visitor>
	static void Serialize(TradeConnectionT& tradeConnection, Visitor& visitor);

	bool isValid() const
	{
		return m_iDestID != -1 && m_eDestOwner != NO_PLAYER && m_iOriginID != -1 && m_eOriginOwner != NO_PLAYER;
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

FDataStream& operator<<(FDataStream&, const TradeConnection&);
FDataStream& operator>>(FDataStream&, TradeConnection&);

typedef std::vector<TradeConnection> TradeConnectionList;

//org plot index -> dst plot index -> path
typedef std::map<int,std::map<int,SPath>> TradePathLookup;

class CvGameTrade
{
public:
	CvGameTrade(void);
	~CvGameTrade(void);

	void Init (void);
	void Uninit (void);
	void Reset (void);

	template<typename GameTrade, typename Visitor>
	static void Serialize(GameTrade& gameTrade, Visitor& visitor);

	void DoTurn (void);

	bool CanCreateTradeRoute (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath);
	bool CanCreateTradeRoute(PlayerTypes eOriginPlayer, PlayerTypes eDestPlayer, DomainTypes eDomainRestriction);
	bool CreateTradeRoute (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, int& iRouteID);

	bool IsValidTradeRoutePath (CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, SPath* pPathOut=NULL, bool bWarCheck = true);
	int GetValidTradeRoutePathLength(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, SPath* pPathOut = NULL, bool bWarCheck = true);
	bool IsDestinationExclusive(const TradeConnection& kTradeConnection);
	bool IsConnectionInternational (const TradeConnection& kConnection);

	bool IsCityConnectedToPlayer (CvCity* pCity, PlayerTypes eOtherPlayer, bool bOnlyOwnedByCityOwner);
	bool IsPlayerConnectedToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer, bool bFirstPlayerOnly = false);
	int CountNumPlayerConnectionsToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer, bool bFirstPlayerOnly = false);

	bool CitiesHaveTradeConnection (CvCity* pFirstCity, CvCity* pSecondCity);

	int GetNumTimesOriginCity (CvCity* pCity, bool bOnlyInternational);
	int GetNumTimesDestinationCity (CvCity* pCity, bool bOnlyInternational);

	void CopyPathIntoTradeConnection (const SPath& path, TradeConnection* pTradeConnection);

	int GetDomainModifierTimes100 (DomainTypes eDomain);

	int GetEmptyTradeRouteIndex (void);
	bool IsTradeRouteIndexEmpty (int iIndex);
	bool ClearTradeRoute (int iIndex);
#if defined(MOD_BALANCE_CORE)
	void UpdateTradePlots();
	int GetTradeRouteTurns(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, int* piCircuitsToComplete = NULL);
#endif
	void ClearAllCityTradeRoutes (CvPlot* pPlot, bool bIncludeTransits = false); // called when a city is captured or traded
	void ClearAllCivTradeRoutes (PlayerTypes ePlayer, bool bFromEmbargo = false); // called from world congress code
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

	bool IsRecalledUnit (int iIndex); // has the unit been recalled
	void RecallUnit (int iIndex, bool bImmediate = false); // recall a trade unit
	void EndTradeRoute (int iIndex); // end a trade route

	// trade unit movement
	bool MoveUnit (int iIndex); // move a trade unit along its path for all its movement points
	bool StepUnit (int iIndex); // move a trade unit a single step along its path (called by MoveUnit)

	void DisplayTemporaryPopupTradeRoute(int iDestX, int iDestY, TradeConnectionType type, DomainTypes eDomain);
	void HideTemporaryPopupTradeRoute(int iPlotX, int iPlotY, TradeConnectionType type) const;

	CvString GetLogFileName() const;
	void LogTradeMsg(CvString& strMsg) const;

	const vector<int>& GetTradeConnectionsForPlayer(PlayerTypes ePlayer) const;
	size_t GetNumTradeConnections() const { return m_aTradeConnections.size(); }
	const TradeConnection& GetTradeConnection(size_t iIndex) const { return m_aTradeConnections[iIndex]; }
	void SetOriginYields(int iConnection, int iYield, int iValue) { m_aTradeConnections[iConnection].m_aiOriginYields[iYield]=iValue; }
	void SetDestYields(int iConnection, int iYield, int iValue) { m_aTradeConnections[iConnection].m_aiDestYields[iYield]=iValue; }

	const std::map<int, SPath>& GetAllPotentialTradeRoutesFromCity(CvCity* pOrigin, bool bWater);
	bool HavePotentialTradePath(bool bWater, CvCity* pOriginCity, CvCity* pDestCity, SPath* pPathOut=NULL);
	void UpdateTradePathCache(PlayerTypes ePlayer1);
	void InvalidateTradePathCache();
	void InvalidateTradePathCache(PlayerTypes ePlayer);
	void InvalidateTradePathTeamCache(TeamTypes eTeam);

	void SetLongestPotentialTradeRoute(int iValue, int iCityIndex, DomainTypes eDomain);
	int GetLongestPotentialTradeRoute(int iCityIndex, DomainTypes eDomain);

private:
	TradePathLookup& GetTradePathsCache(bool bWater);
	int GetTradePathsCacheUpdateCounter(PlayerTypes ePlayer);
	void SetTradePathsCacheUpdateCounter(PlayerTypes ePlayer, int iValue);

protected:
	
	// std::map<city index, std::map<DomainTypes, longest length>>
	std::map<int, std::map<DomainTypes, int>> m_aiLongestPotentialTradeRoute;
	TradeConnectionList m_aTradeConnections;
	TradePathLookup m_aPotentialTradePathsLand;
	TradePathLookup m_aPotentialTradePathsWater;
	TradePathLookup m_aPotentialTradePathsLandUi;
	TradePathLookup m_aPotentialTradePathsWaterUi;
	vector<int> m_lastTradePathUpdate;
	vector<int> m_lastTradePathUpdateUi;
	std::vector<vector<int>> m_routesPerPlayer;
	std::map<int, SPath> m_dummyTradePaths; //always empty, just for us to return a reference

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
	template<typename TradeConnectionWasPlunderedT, typename Visitor>
	static void Serialize(TradeConnectionWasPlunderedT& tradeConnectionWasPlundered, Visitor& visitor);

	TradeConnection m_kTradeConnection;
	int m_iTurnPlundered;
};

FDataStream& operator<<(FDataStream&, const TradeConnectionWasPlundered&);
FDataStream& operator>>(FDataStream&, TradeConnectionWasPlundered&);

typedef std::vector<TradeConnectionWasPlundered> TradeConnectionWasPlunderedList;

class CvPlayerTrade
{
public:
	CvPlayerTrade(void);
	~CvPlayerTrade(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);

	template<typename PlayerTrade, typename Visitor>
	static void Serialize(PlayerTrade& playerTrade, Visitor& visitor);

	// Functions invoked each player turn
	void DoTurn(void);
	void MoveUnits(void);

	int GetTradeConnectionBaseValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer) const;
	int GetTradeConnectionGPTValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer, bool bOriginCity);
	int GetTradeConnectionResourceValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionYourBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#if defined(MOD_BALANCE_CORE)
	int GetMinorCivGoldBonus(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#endif
	int GetTradeConnectionTheirBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer) const;
	int GetTradeConnectionExclusiveValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionPolicyValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield) const;
	int GetTradeConnectionOtherTraitValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionDomainValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
	int GetTradeConnectionRiverValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionDiplomatModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield);
#if defined(MOD_BALANCE_CORE)
	int GetTradeConnectionDistanceValueModifierTimes100(const TradeConnection& kTradeConnection) const;
	int GetTradeConnectionCorporationModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	int GetTradeConnectionOpenBordersModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
	int GetTradeConnectionPolicyModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
#endif

	int GetTradeConnectionValueTimes100 (const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer);
	void ResetTradeStats();
	void UpdateTradeConnectionValues (void); // updates the all the values for the trade routes that go to and from this player

	int GetTradeValuesAtCityTimes100 (const CvCity* const pCity, YieldTypes eYield);

	int GetAllTradeValueTimes100 (YieldTypes eYield) const;
	int GetAllTradeValueFromPlayerTimes100 (YieldTypes eYield, PlayerTypes ePlayer) const;
	int GetTradeGPTLostFromWarTimes100(PlayerTypes ePlayer) const;

	bool IsConnectedToPlayer(PlayerTypes eOtherPlayer) const;

	bool CanCreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath = true) const;
	bool CanCreateTradeRoute(PlayerTypes eOtherPlayer, DomainTypes eDomain);
	bool CanCreateTradeRoute(DomainTypes eDomain) const;

	bool CreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType) const;

	const TradeConnection* GetTradeConnection(CvCity* pOriginCity, CvCity* pDestCity) const;
	int GetNumberOfCityStateTradeRoutes();
	int GetNumberOfCityStateTradeRoutesFromCity(CvCity* pCity) const;

	int GetNumberOfTradeRoutesFromCity(const CvCity* pCity);
	int GetNumberOfTradeRoutesCity(const CvCity* pCity);

	bool IsCityAlreadyConnectedByTrade(CvCity* pOtherCity) const;
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

	bool PlunderTradeRoute(int iTradeConnectionID, CvUnit* pUnit);
	void UpdateFurthestPossibleTradeRoute(DomainTypes eDomain, CvCity* pOriginCity, int iMaxRange);
	int GetTradeRouteRange (DomainTypes eDomain, CvCity* pOriginCity) const;
	int GetTradeRouteSpeed (DomainTypes eDomain) const;
	int GetTradeRouteTurnMod(CvCity* pOriginCity) const;

	uint GetNumTradeRoutesPossible (void) const;
	int GetNumTradeUnits (bool bIncludeBeingBuilt) const;
	int GetNumTradeUnitsRemaining (bool bIncludeBeingBuilt);

	int GetNumDifferentTradingPartners (void) const;
#if defined(MOD_BALANCE_CORE)
	int GetNumDifferentMajorCivTradingPartners(void) const;
#endif

	void UpdateTradeConnectionWasPlundered();
	void AddTradeConnectionWasPlundered(const TradeConnection& kTradeConnection);
	bool CheckTradeConnectionWasPlundered(const TradeConnection& kTradeConnection);

	static UnitTypes GetTradeUnit (DomainTypes eDomain, CvPlayer* pPlayer);

	void LogTradeMsg(CvString& strMsg) const;

	std::vector<CvString> GetPlotToolTips (CvPlot* pPlot);
	std::vector<CvString> GetPlotMouseoverToolTips (CvPlot* pPlot);

	TradeConnectionList m_aRecentlyExpiredConnections;
	TradeConnectionWasPlunderedList m_aTradeConnectionWasPlundered;

	CvPlayer* m_pPlayer;

	struct SPlayerTradeStats
	{
		int iTurnSliceBuilt;
		int iInternationalTRsOut;
		int iInternationalTRsIn;
		int iInternalTRs;
		int iMinorTRs;
		map<int, int> nRoutesFromCity; //city id, n outgoing routes
		map<int, int> nRoutesToCity; //city id, n outgoing routes

		void reset() { iTurnSliceBuilt = 0; iInternationalTRsOut = 0; iInternationalTRsIn = 0; iInternalTRs = 0; iMinorTRs = 0; nRoutesFromCity.clear(); nRoutesToCity.clear(); }
	};

	//some precomputed numbers for performance
	SPlayerTradeStats m_tradeStats;
	void UpdateTradeStats();
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

	template<typename TradeAI, typename Visitor>
	static void Serialize(TradeAI& tradeAI, Visitor& visitor);

	struct TRSortElement
	{
		TRSortElement() : m_iScore(0), m_iGoldScore(0), m_iCultureScore(0), m_iReligionScore(0), m_iScienceScore(0) {}

		TradeConnection m_kTradeConnection;
		int m_iScore;
		int m_iGoldScore;
		int m_iCultureScore;
		int m_iReligionScore;
		int m_iScienceScore;
	};

	void DoTurn(void);

	void GetAvailableTR(TradeConnectionList& aTradeConnectionList, bool bSkipExisting);
	void GetPrioritizedTradeRoutes(TradeConnectionList& aTradeConnectionList, bool bSkipExisting);

	TRSortElement ScoreInternationalTR(const TradeConnection& kTradeConnection, bool bHaveTourism) const;

	//generic method
	int ScoreInternalTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);

	//wrapper for different types of trade route
	int ScoreFoodTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);
	int ScoreProductionTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	int ScoreWonderTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList);
#endif
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	TRSortElement ScoreGoldInternalTR(const TradeConnection& kTradeConnection) const;
#endif

	int m_iRemovableValue;

	CvPlayer* m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvTradeAI&);
FDataStream& operator<<(FDataStream&, const CvTradeAI&);

#endif //CIV5_TRADE_CLASSES_H
