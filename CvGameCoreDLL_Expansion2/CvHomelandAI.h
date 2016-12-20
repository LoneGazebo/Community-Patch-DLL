/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_HOMELAND_AI_H
#define CIV5_HOMELAND_AI_H

#if defined(MOD_BALANCE_CORE_MILITARY)
#define UPGRADE_THIS_TURN_PRIORITY_BOOST 5000
#define UPGRADE_IN_TERRITORY_PRIORITY_BOOST 2000
#else
#define UPGRADE_THIS_TURN_PRIORITY_BOOST 1000
#define UPGRADE_IN_TERRITORY_PRIORITY_BOOST 500
#endif
enum AIHomelandTargetType
{
    AI_HOMELAND_TARGET_NONE,
    AI_HOMELAND_TARGET_CITY,
    AI_HOMELAND_TARGET_SENTRY_POINT,
	AI_HOMELAND_TARGET_SENTRY_POINT_NAVAL,
    AI_HOMELAND_TARGET_FORT,
    AI_HOMELAND_TARGET_NAVAL_RESOURCE,
    AI_HOMELAND_TARGET_HOME_ROAD,
    AI_HOMELAND_TARGET_ANCIENT_RUIN,
	AI_HOMELAND_TARGET_ANTIQUITY_SITE,
	AI_HOMELAND_TARGET_UNASSIGNED,
};

// Object stored in the list of move priorities (m_MovePriorityList)
class CvHomelandMove
{
public:
	CvHomelandMove()
	{
		m_eMoveType = AI_HOMELAND_MOVE_NONE;
		m_iPriority = 0;
	}

	bool operator<(const CvHomelandMove& move) const
	{
		return (m_iPriority > move.m_iPriority);
	}

	AIHomelandMove m_eMoveType;
	int m_iPriority;
};

// Object stored in the list of current move units (m_CurrentMoveUnits)
class CvHomelandUnit
{
public:
	CvHomelandUnit();

	bool operator<(const CvHomelandUnit& unit) const
	{
		return (GetMovesToTarget() < unit.GetMovesToTarget());
	}

	// Accessors
	void SetID(int iID)
	{
		m_iID = iID;
	};
	int GetID() const
	{
		return m_iID;
	};
	void SetMovesToTarget(int iMoves)
	{
		m_iMovesToTarget = iMoves;
	};
	int GetMovesToTarget() const
	{
		return m_iMovesToTarget;
	};
	void SetTarget(CvPlot* pPlot)
	{
		m_pTarget = pPlot;
	};
	CvPlot* GetTarget() const
	{
		return m_pTarget;
	};

	// Stores extra integer data
	//   For potential upgradeable units stores the unit type since that's a convenient way to sort them
	int GetAuxIntData() const
	{
		return m_iAuxData;
	}
	void SetAuxIntData(int iAuxData)
	{
		m_iAuxData = iAuxData;
	}

private:
	int m_iID;
	int m_iAuxData;
	int m_iMovesToTarget;

	CvPlot* m_pTarget;
};

#if defined(MOD_BALANCE_CORE_MILITARY)
//a simple wrapper around std::vector so we can log/break on certain units being added (in a central place)
class CHomelandUnitArray
{
public:
	CHomelandUnitArray() : m_owner(NULL), m_currentHomelandMove(AI_HOMELAND_MOVE_UNASSIGNED) {}

	std::vector<CvHomelandUnit>::iterator begin() { return m_vec.begin(); }
	std::vector<CvHomelandUnit>::iterator end() { return m_vec.end(); }
	std::vector<CvHomelandUnit>::size_type size() const { return m_vec.size(); }
	bool empty() { return m_vec.empty(); }
	std::vector<CvHomelandUnit>::reference operator[](std::vector<CvHomelandUnit>::size_type _Pos) { return m_vec[_Pos]; }
	std::vector<CvHomelandUnit>::iterator erase(std::vector<CvHomelandUnit>::const_iterator _Where) { return m_vec.erase(_Where); }
	void push_back(const CvHomelandUnit& unit);
	void clear() { m_vec.clear(); }
	void setPlayer(CvPlayer* pOwner) { m_owner=pOwner; }
	void setCurrentHomelandMove(AIHomelandMove move) { m_currentHomelandMove=move; }

	typedef std::vector<CvHomelandUnit>::iterator iterator; 

private:
	std::vector<CvHomelandUnit> m_vec;
	CvPlayer* m_owner;
	AIHomelandMove m_currentHomelandMove;
};

#endif


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvHomelandTarget
//!  \brief		A target of opportunity for the Homeland AI this turn
//
//!  Key Attributes:
//!  - Arises during processing of CvHomelandAI::FindHomelandTargets()
//!  - Targets are reexamined each turn (so shouldn't need to be serialized)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHomelandTarget
{
public:

	CvHomelandTarget()
		: m_eTargetType(AI_HOMELAND_TARGET_CITY)
#if defined(MOD_BALANCE_CORE)
		, m_iTargetX(INVALID_PLOT_COORD)
		, m_iTargetY(INVALID_PLOT_COORD)
#else
		, m_iTargetX(-1)
		, m_iTargetY(-1)
#endif
		, m_pAuxData(NULL)
		, m_iAuxData(0)
	{
	};

	bool operator<(const CvHomelandTarget& target) const
	{
		return (m_iAuxData > target.m_iAuxData);
	}
	inline AIHomelandTargetType GetTargetType()
	{
		return m_eTargetType;
	}
	inline void SetTargetType(AIHomelandTargetType eTargetType)
	{
		m_eTargetType = eTargetType;
	}
	inline int GetTargetX()
	{
		return m_iTargetX;
	}
	inline void SetTargetX(int iValue)
	{
		m_iTargetX = iValue;
	}
	inline int GetTargetY()
	{
		return m_iTargetY;
	}
	inline void SetTargetY(int iValue)
	{
		m_iTargetY = iValue;
	}

	// AuxData is used for a pointer to the actual target object (CvUnit, CvCity, etc.)
	//    (for naval improvements this is set to target plot).
	inline void* GetAuxData()
	{
		return m_pAuxData;
	}
	inline void SetAuxData(void* pAuxData)
	{
		m_pAuxData = pAuxData;
	}

	// Used to SORT homeland targets in priority order
	//    Set to the BuildType for improvement targets
	//    Set to the weight for sentry points
	//    Set to the danger for cities to be garrisoned
	inline int GetAuxIntData()
	{
		return m_iAuxData;
	}
	inline void SetAuxIntData(int iAuxData)
	{
		m_iAuxData = iAuxData;
	}

private:
	AIHomelandTargetType m_eTargetType;
	int m_iTargetX;
	int m_iTargetY;
	void* m_pAuxData;
	int m_iAuxData;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvHomelandAI
//!  \brief		A player's AI to control units that are in reserve protecting their lands
//
//!  Key Attributes:
//!  - Handles moves for all military units not recruited by the tactical or operational AI
//!  - Also handles moves for workers and explorers (and settlers on the first turn)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHomelandAI
{
public:
	CvHomelandAI(void);
	~CvHomelandAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();

	// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Public turn update routines
	void RecruitUnits();
	void FindAutomatedUnits();
	void DoTurn();
	void Update();

	// Public exploration routines
	CvPlot* GetBestExploreTarget(const CvUnit* pUnit, int nMinCandidatesToCheck, int iMaxTurns) const;
	bool ExecuteSpecialExploreMove(CvUnit* pUnit, CvPlot* pPlot);
#if defined(MOD_BALANCE_CORE)
	bool FindTestArchaeologistPlotPrimer(CvUnit* pUnit);
#endif
	// Public logging
	void LogHomelandMessage(const CvString& strMsg);
	void LogPatrolMessage(const CvString& strMsg, CvUnit* pPatrolUnit);

#if defined(MOD_BALANCE_CORE)
	bool MoveCivilianToGarrison(CvUnit* pUnit);
#endif
#if defined(MOD_AI_SECONDARY_WORKERS)
	bool MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits = false, bool bSecondary = false);
#else
	bool MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits = false);
#endif

private:

	typedef CHomelandUnitArray MoveUnitsArray;

	// Internal turn update routines - commandeered unit processing
	void EstablishHomelandPriorities();
	void FindHomelandTargets();
	void AssignHomelandMoves();

	// Routines to manage identifying and implementing homeland moves
	void PlotExplorerMoves();
	void PlotExplorerSeaMoves();
	void PlotFirstTurnSettlerMoves();
	void PlotGarrisonMoves(bool bCityStateOnly = false);
	void PlotHealMoves();
	void PlotMovesToSafety();
	void PlotMobileReserveMoves();
#if defined(MOD_AI_SECONDARY_SETTLERS)
	void PlotOpportunisticSettlementMoves();
#endif
	void PlotSentryMoves();
#if defined(MOD_BALANCE_CORE)
	void PlotSentryNavalMoves();
#endif
#if defined(MOD_AI_SECONDARY_WORKERS)
	void PlotWorkerMoves(bool bSecondary = false);
#else
	void PlotWorkerMoves();
#endif
#if defined(MOD_BALANCE_CORE)
	void PlotWorkerSeaMoves(bool bSecondary = false);
#else
	void PlotWorkerSeaMoves();
#endif
	void PlotPatrolMoves();
	void PlotUpgradeMoves();
	void PlotAncientRuinMoves();
	void PlotWriterMoves();
	void PlotArtistMoves();
	void PlotMusicianMoves();
	void PlotScientistMoves();
	void PlotEngineerMoves();
	void PlotGeneralMoves();
	void PlotMerchantMoves();
#if defined(MOD_DIPLOMACY_CITYSTATES)
	void PlotDiplomatMoves();
	void PlotMessengerMoves();
#endif
	void PlotProphetMoves();
	void PlotAdmiralMoves();
	void PlotMissionaryMoves();
	void PlotInquisitorMoves();
	void PlotSSPartAdds();
	void PlotSSPartMoves();
	void PlotTreasureMoves();
	void PlotAircraftMoves();
	void PlotTradeUnitMoves();
	void PlotArchaeologistMoves();
	void PlotAirliftMoves();
	void ReviewUnassignedUnits();
#if defined(MOD_BALANCE_CORE)
	void ExecuteUnassignedUnitMoves();
#endif

	// Routines to execute homeland moves
	void ExecuteFirstTurnSettlerMoves();
#if defined(MOD_BALANCE_CORE)
	void ExecuteExplorerMoves(bool bSecondPass);
#else
	void ExecuteExplorerMoves();
#endif

#if defined(MOD_AI_SECONDARY_WORKERS)
	void ExecuteWorkerMoves(bool bSecondary = false);
#else
	void ExecuteWorkerMoves();
#endif
	void ExecuteMovesToSafestPlot();
	void ExecuteMoveToTarget(CvUnit* pUnit, CvPlot* pTarget, int iFlags, bool bFinishMoves = false);

	void ExecuteHeals();
	void ExecuteWriterMoves();
	void ExecuteArtistMoves();
	void ExecuteMusicianMoves();
	void ExecuteScientistMoves();
	void ExecuteEngineerMoves();
	void ExecuteGeneralMoves();
#if defined(MOD_DIPLOMACY_CITYSTATES)
	void ExecuteDiplomatMoves();
	void ExecuteMessengerMoves();
#endif
	void ExecuteMerchantMoves();
	void ExecuteProphetMoves();
	void ExecuteAdmiralMoves();
	void ExecuteMissionaryMoves();
	void ExecuteInquisitorMoves();
	void ExecuteSSPartAdds();
	void ExecuteSSPartMoves();
	void ExecuteTreasureMoves();
#if defined(MOD_AI_SMART_AIR_TACTICS)
	void ExecuteAircraftInterceptions();
#endif
	void ExecuteAircraftMoves();
	void ExecuteTradeUnitMoves();
	void ExecuteArchaeologistMoves();
	void ExecutePatrolMoves();
#if defined(MOD_BALANCE_CORE)
	void ExecuteAggressivePatrolMoves();
#endif

	// Internal low-level utility routines
	void EliminateAdjacentSentryPoints();
#if defined(MOD_BALANCE_CORE)
	void EliminateAdjacentNavalSentryPoints();
	void EliminateAdjacentUnassignedPoints();
#endif
	void EliminateAdjacentHomelandRoads();
	bool FindUnitsForThisMove(AIHomelandMove eMove, bool bFirstTime);
	CvUnit* GetBestUnitToReachTarget(CvPlot* pTarget, int iMaxTurns);

	bool MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, DomainTypes eDomain, int iMaxTurns);
	bool MoveToTargetButDontEndTurn(CvUnit* pUnit, CvPlot* pTargetPlot, int iFlags);

	CvPlot* FindArchaeologistTarget(CvUnit *pUnit);
#if defined(MOD_BALANCE_CORE)
	CvPlot* FindUnassignedTarget(CvUnit *pUnit);
#endif
	void UnitProcessed(int iID);
#if defined(MOD_AI_SECONDARY_WORKERS)
	bool ExecuteWorkerMove(CvUnit* pUnit, bool bSecondary = false);
#else
	bool ExecuteWorkerMove(CvUnit* pUnit);
#endif
	bool ExecuteCultureBlast(CvUnit* pUnit);
	bool ExecuteGoldenAgeMove(CvUnit* pUnit);
	bool IsValidExplorerEndTurnPlot(const CvUnit* pUnit, CvPlot* pPlot) const;
	bool GetClosestUnitByTurnsToTarget(MoveUnitsArray &kMoveUnits, CvPlot* pTarget, int iMaxTurns, CvUnit** ppClosestUnit, int* piClosestTurns);
	void ClearCurrentMoveUnits();
	void ClearCurrentMoveHighPriorityUnits();

	// Logging functions
	CvString GetLogFileName(CvString& playerName) const;

	// Class data
	CvPlayer* m_pPlayer;
	std::list<int> m_CurrentTurnUnits;

	MoveUnitsArray m_CurrentMoveUnits;
	MoveUnitsArray m_CurrentMoveHighPriorityUnits;

	FStaticVector< CvHomelandMove, 35, true, c_eCiv5GameplayDLL > m_MovePriorityList;

	// Lists of targets for the turn
	std::vector<CvHomelandTarget> m_TargetedCities;
	std::vector<CvHomelandTarget> m_TargetedSentryPoints;
	std::vector<CvHomelandTarget> m_TargetedForts;
	std::vector<CvHomelandTarget> m_TargetedNavalResources;
	std::vector<CvHomelandTarget> m_TargetedHomelandRoads;
	std::vector<CvHomelandTarget> m_TargetedAncientRuins;
	std::vector<CvHomelandTarget> m_TargetedAntiquitySites;
#if defined(MOD_BALANCE_CORE)
	std::vector<CvHomelandTarget> m_TargetedNavalSentryPoints;
	std::vector<CvHomelandTarget> m_TargetedHomeUnassignedPlots;
#endif

	// Targeting ranges (pulled in from GlobalAIDefines.XML)
	int m_iRandomRange;
	int m_iDefensiveMoveTurns;
	int m_iUpgradeMoveTurns;
	double m_fFlavorDampening;
};

#if defined(MOD_BALANCE_CORE_MILITARY)
extern const char* homelandMoveNames[];
extern const char* directiveNames[];
#endif

namespace HomelandAIHelpers
{
bool CvHomelandUnitAuxIntSort(const CvHomelandUnit& obj1, const CvHomelandUnit& obj2);
bool CvHomelandUnitAuxIntReverseSort(const CvHomelandUnit& obj1, const CvHomelandUnit& obj2);

int ScoreAirBase(CvPlot* pAirBasePlot, PlayerTypes ePlayer, int iRange);
bool IsGoodUnitMix(CvPlot* pAirBasePlot, CvUnit* pUnit);
std::vector<CvPlot*> GetPatrolTargets(PlayerTypes ePlayer, bool bWater, int nMaxTargets = 5);

std::vector<CvPlot*> GetAggressivePatrolTargets(PlayerTypes ePlayer, bool bWater, int nMaxTargets = 5);

}

#endif //CIV5_HOMELAND_AI_H