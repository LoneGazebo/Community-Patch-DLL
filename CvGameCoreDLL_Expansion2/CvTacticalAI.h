﻿/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TACTICAL_AI_H
#define CIV5_TACTICAL_AI_H

#include "CvAStar.h"

class FDataStream;

//for tactical combat
enum eAggressionLevel { AL_NONE, AL_LOW, AL_MEDIUM, AL_HIGH, AL_BRAVEHEART };
extern const unsigned char TACTICAL_COMBAT_MAX_TARGET_DISTANCE;

// STL "find_if" predicate
class UnitIDMatch
{
public:
	UnitIDMatch(int iUnitID)
		: m_iMatchingUnitID(iUnitID)
	{}

	bool operator()(int element) const
	{
		return (element == m_iMatchingUnitID);
	}

private:
	int m_iMatchingUnitID;
};

// Object stored in the list of current move units (m_CurrentMoveUnits)
class CvTacticalUnit
{
public:
	CvTacticalUnit(int iID) 
	{
		memset(this, 0, sizeof(CvTacticalUnit)); m_iID = iID;
	}

	bool operator<(const CvTacticalUnit& unit) const
	{
		return (GetAttackPriority() > unit.GetAttackPriority());
	}

	int GetID() const
	{
		return m_iID;
	};

	void SetHealthPercent(int curHitPoints, int maxHitPoints)
	{
		ASSERT_DEBUG(maxHitPoints != 0);
		m_iHealthPercent = curHitPoints * 100 / maxHitPoints;
	}
	int GetHealthPercent() const
	{
		return m_iHealthPercent;
	};
	void SetAttackStrength(int iAttackStrength)
	{
		m_iAttackStrength = iAttackStrength;
	};
	int GetAttackStrength() const
	{
		return m_iAttackStrength;
	};
	void SetExpectedTargetDamage(int iExpectedDamage)
	{
		m_iExpectedTargetDamage = iExpectedDamage;
	};
	int GetExpectedTargetDamage() const
	{
		return m_iExpectedTargetDamage;
	};
	void SetExpectedSelfDamage(int iExpectedDamage)
	{
		m_iExpectedSelfDamage = iExpectedDamage;
	};
	int GetExpectedSelfDamage() const
	{
		return m_iExpectedSelfDamage;
	};
	void SetMovesToTarget(int iMoves)
	{
		m_iMovesToTarget = iMoves;
	};
	int GetMovesToTarget() const
	{
		return m_iMovesToTarget;
	};

	void SetInterceptor(CvUnit* pInterceptor)
	{
		m_pInterceptor = pInterceptor;
	}
	CvUnit* GetInterceptor() const
	{
		return m_pInterceptor;
	};

	// Derived
	int GetAttackPriority() const
	{
		return m_iAttackStrength * m_iHealthPercent;
	}

private:
	int m_iID;
	int m_iAttackStrength;
	int m_iHealthPercent;
	int m_iMovesToTarget;
	int m_iExpectedTargetDamage;
	int m_iExpectedSelfDamage;
	CvUnit* m_pInterceptor;
};

// Object stored in the list of current move cities (m_CurrentMoveCities)
class CvTacticalCity
{
public:
	CvTacticalCity()
	{
		m_iID = 0;
		m_iExpectedTargetDamage = 0;
	}

	bool operator<(const CvTacticalCity& city) const
	{
		return (GetExpectedTargetDamage() > city.GetExpectedTargetDamage());
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
	void SetExpectedTargetDamage(int iExpectedDamage)
	{
		m_iExpectedTargetDamage = iExpectedDamage;
	};
	int GetExpectedTargetDamage() const
	{
		return m_iExpectedTargetDamage;
	};

private:
	int m_iID;
	int m_iExpectedTargetDamage;
};

//a simple wrapper around std::vector so we can log/break on certain units being added (in a central place)
class CTacticalUnitArray
{
public:
	CTacticalUnitArray() : m_eCurrentMoveType(AI_TACTICAL_MOVE_NONE) {}

	std::vector<CvTacticalUnit>::const_iterator begin() const { return m_vec.begin(); }
	std::vector<CvTacticalUnit>::const_iterator end() const { return m_vec.end(); }
	std::vector<CvTacticalUnit>::iterator begin() { return m_vec.begin(); }
	std::vector<CvTacticalUnit>::iterator end() { return m_vec.end(); }
	std::vector<CvTacticalUnit>::size_type size() const { return m_vec.size(); }
	std::vector<CvTacticalUnit>::reference operator[](std::vector<CvTacticalUnit>::size_type _Pos) { return m_vec[_Pos]; }
	std::vector<CvTacticalUnit>::iterator erase(std::vector<CvTacticalUnit>::const_iterator _Where) { return m_vec.erase(_Where); }
	CvTacticalUnit& back() { return m_vec.back(); }
	void push_back(const CvTacticalUnit& unit);
	void clear() { m_vec.clear(); }
	void setCurrentTacticalMove(AITacticalMove move) { m_eCurrentMoveType = move; }
	AITacticalMove getCurrentTacticalMove() const { return m_eCurrentMoveType; }

private:
	std::vector<CvTacticalUnit> m_vec;
	AITacticalMove m_eCurrentMoveType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalTarget
//!  \brief		A target of opportunity for the tactical AI this turn
//
//!  Key Attributes:
//!  - Arises during processing of CvTacticalAI::FindTacticalTargets()
//!  - Targets are reexamined each turn (so shouldn't need to be serialized)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTacticalTarget
{
public:
	CvTacticalTarget()
	{
		Clear();
	};
	void Clear()
	{
		m_eTargetType = AI_TACTICAL_TARGET_NONE;
		m_iTargetX = INVALID_PLOT_COORD;
		m_iTargetY = INVALID_PLOT_COORD;
		m_iZoneID = -1;
		m_pUnit = NULL;
		m_iAuxData = 0;
		m_eLastAggLvl = AL_NONE;
	};
	inline bool operator<(const CvTacticalTarget& target) const
	{
		return (m_iAuxData > target.m_iAuxData);
	}
	inline AITacticalTargetType GetTargetType() const
	{
		return m_eTargetType;
	}
	void SetTargetType(AITacticalTargetType eTargetType)
	{
		m_eTargetType = eTargetType;
	}
	inline int GetTargetX() const
	{
		return m_iTargetX;
	}
	inline void SetTargetX(int iValue)
	{
		m_iTargetX = iValue;
	}
	inline int GetTargetY() const
	{
		return m_iTargetY;
	}
	inline void SetTargetY(int iValue)
	{
		m_iTargetY = iValue;
	}

	inline int GetDominanceZone() const
	{
		return m_iZoneID;
	};
	inline void SetDominanceZone(int iZone)
	{
		m_iZoneID = iZone;
	};

	bool IsTargetValidInThisDomain(DomainTypes eDomain) const;

	//if we already tried with a high agg level, no point in trying again with a lower level
	//chance that we have more units for an attack later in the turn is also slim
	void SetLastAggLevel(eAggressionLevel lvl) { m_eLastAggLvl = lvl; }
	eAggressionLevel GetLastAggLevel() const { return m_eLastAggLvl; }

	inline CvUnit* GetUnitPtr() const
	{
		return m_pUnit;
	}
	inline void SetUnitPtr(CvUnit* pUnit)
	{
		m_pUnit = pUnit;
	}

	// Stores required damage for offensive targets
	// For defensive items used to SORT targets in priority order
	//    Set to the weight for defensive bastions
	//    Set to the danger for cities to be garrisoned
	inline int GetAuxIntData() const
	{
		return m_iAuxData;
	}
	inline void SetAuxIntData(int iAuxData)
	{
		m_iAuxData = iAuxData;
	}

private:
	AITacticalTargetType m_eTargetType;
	int m_iTargetX;
	int m_iTargetY;
	CvUnit* m_pUnit;
	int m_iAuxData;
	int m_iZoneID;
	eAggressionLevel m_eLastAggLvl;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvFocusArea
//!  \brief		Location of a temporary focus of attention (like around a barbarian camp)
//
//!  Key Attributes:
//!  - Used to add dominance zones for short duration tactical strikes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvFocusArea
{
	template<typename FocusArea, typename Visitor>
	static void Serialize(FocusArea& focusArea, Visitor& visitor);

	int m_iX;
	int m_iY;
	int m_iRadius;
	int m_iLastTurn;
};

FDataStream& operator<<(FDataStream&, const CvFocusArea&);
FDataStream& operator>>(FDataStream&, CvFocusArea&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalAI
//!  \brief		A player's AI to control units as they fight out battles
//
//!  Key Attributes:
//!  - Handed units to control by the operational AI
//!  - Handles moves for these units until dead or objective completed
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTacticalAI
{
public:
	CvTacticalAI(void);
	~CvTacticalAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();

	// Serialization routines
	template<typename TacticalAI, typename Visitor>
	static void Serialize(TacticalAI& tacticalAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Public turn update routines
	void Update();
	void CleanUp();

	// temporary focus of attention
	void AddFocusArea(CvPlot* pPlot, int iRadius, int iDuration);
	void DeleteFocusArea(CvPlot* pPlot);
	void DropOldFocusAreas();
	bool IsInFocusArea(const CvPlot* pPlot) const;

	// Knowledge of other civs' vision
	void UpdateVisibility();
	void UpdateVisibilityFromUnits(CvPlot* pPlot);
	void UpdateVisibilityFromBorders(CvPlot* pPlot);

	// For air units
	bool ShouldRebase(CvUnit* pUnit) const;

	// Public logging
	FILogFile* GetLogFile();
	void LogTacticalMessage(const CvString& strMsg);

	// Other people want to know this too
	const vector<CvTacticalTarget>& GetTacticalTargets() const { return m_AllTargets; }
	CvTacticalAnalysisMap* GetTacticalAnalysisMap() { return &m_tacticalMap; }

	// Operational AI support functions
	void PlotArmyMovesEscort(CvArmyAI* pThisArmy);
	void PlotArmyMovesCombat(CvArmyAI* pThisArmy);
	void AddCurrentTurnUnit(CvUnit* pUnit);

	void UnitProcessed(int iID);
private:

	void RecruitUnits();

	// Internal turn update routines - commandeered unit processing
	void FindTacticalTargets();
	void PrioritizeNavalTargetsAndAddToMainList();
	void ProcessDominanceZones();
	void AssignGlobalHighPrioMoves();
	void AssignGlobalMidPrioMoves();
	void AssignGlobalLowPrioMoves();
	void AssignBarbarianMoves();

	// Routines to manage identifying and implementing tactical moves
	void ExecuteCaptureCityMoves();
	void PlotGrabGoodyMoves();
	void PlotMovesToSafety(bool bCombatUnits);
	void PlotOperationalArmyMoves();
	void PlotPillageMoves(AITacticalTargetType eTarget, bool bImmediate);
	void PlotPlunderTradeUnitMoves(DomainTypes eDomain);
	void PlotBlockadeMoves();
	void PlotCivilianAttackMoves();
	void ExecuteCivilianAttackMoves(AITacticalTargetType eTargetType);
	void PlotHealMoves(bool bFirstPass);
	void PlotBarbarianAttacks();
	void PlotBarbarianCampDefense();
	void PlotBarbarianRoaming();

///------------------------------
//	unify these?
///------------------------------
	void PlotGarrisonMoves(int iNumTurnsAway);
	void PlotBastionMoves(int iNumTurnsAway);
	void PlotGuardImprovementMoves(int iNumTurnsAway);
//--------------------------------

	void PlotAirPatrolMoves();

	void PlotAttritionAttacks(CvTacticalDominanceZone* pZone);
	void PlotExploitFlanksMoves(CvTacticalDominanceZone* pZone);
	void PlotSteamrollMoves(CvTacticalDominanceZone* pZone);
	void PlotSurgicalCityStrikeMoves(CvTacticalDominanceZone* pZone);
	void PlotHedgehogMoves(CvTacticalDominanceZone* pZone);
	void PlotCounterattackMoves(CvTacticalDominanceZone* pZone);
	void PlotWithdrawMoves(CvTacticalDominanceZone* pZone);
	void PlotReinforcementMoves(CvTacticalDominanceZone* pTargetZone);

	void PlotEmergencyPurchases(CvTacticalDominanceZone* pZone);
	void PlotDefensiveAirlifts(CvTacticalDominanceZone* pZone);

	void PlotNavalEscortMoves();
	void ReviewUnassignedUnits();

	// Operational AI support functions
	bool CheckForEnemiesNearArmy(CvArmyAI* pArmy);
	void ExecuteGatherMoves(CvArmyAI* pArmy, CvPlot* pTurnTarget);

	// Routines to process and sort targets
	void UpdateTargetScores();
	void SortTargetListAndDropUselessTargets();
	void DumpTacticalTargets();

	void ClearCurrentMoveUnits(AITacticalMove eNewMove);
	int ExtractTargetsForZone(CvTacticalDominanceZone* pZone /* Pass in NULL for all zones */);
	CvTacticalTarget* GetFirstZoneTarget(AITacticalTargetType eType, eAggressionLevel threshold = AL_BRAVEHEART);
	CvTacticalTarget* GetNextZoneTarget(eAggressionLevel threshold = AL_BRAVEHEART);

	// Routines to execute a mission
	void ExecuteBarbarianCampMove(CvPlot* pTargetPlot);
	void ExecuteBarbarianTheft();
	bool ExecutePillage(CvPlot* pTargetPlot);
	void ExecutePlunderTradeUnit(CvPlot* pTargetPlot);
	void ExecuteParadropPillage(CvPlot* pTargetPlot);
	void ExecuteLandingOperation(CvPlot* pTargetPlot);
	bool ExecuteSpotterMove(const vector<CvUnit*>& vUnits, CvPlot* pTargetPlot);
	bool ExecuteAttackWithUnits(CvPlot* pTargetPlot, eAggressionLevel eAggLvl);
	bool ExecuteAttackWithCities(CvUnit* pDefender);
	bool PositionUnitsAroundTarget(const vector<CvUnit*>& vUnits, CvPlot* pTarget);
	void ExecuteAirSweep(CvPlot* pTargetPlot);
	void ExecuteAirAttack(CvPlot* pTargetPlot);
	void ExecuteRepositionMoves();
	void ExecuteMovesToSafestPlot(CvUnit* pUnit);
	void ExecuteHeals(bool bFirstPass);
	void ExecuteBarbarianRoaming();
	int ExecuteMoveToPlot(CvUnit* pUnit, CvPlot* pTarget, bool bSetProcessed = true, int iFlags = 0);
	bool ExecuteMoveOfBlockingUnit(CvUnit* pBlockingUnit, CvPlot* pPreferredDirection=NULL);
	void ExecuteNavalBlockadeMove(CvPlot* pTarget);
	void ExecuteAirPatrolMoves();
	void ExecuteAirSweepMoves();
	bool ExecuteDestroyEnemyUnits(CvTacticalTarget& kTarget, eAggressionLevel aggLvl = AL_MEDIUM);
	void ExecuteWithdrawMoves();
	void ExecuteEscortEmbarkedMoves(std::vector<CvUnit*> vTargets);

	// Internal low-level utility routines
	CvPlot* GetBestRepositionPlot(CvUnit* pUnit, CvPlot* plotTarget, int iAcceptableDanger);
	CvUnit* FindUnitForThisMove(AITacticalMove eMove, CvPlot* pTargetPlot, int iNumTurnsAway=0);
	bool FindUnitsWithinStrikingDistance(CvPlot *pTargetPlot);
	bool FindUnitsForHarassing(CvPlot* pTarget, int iNumTurnsAway, int iMinHitpoints, int iMaxHitpoints, DomainTypes eDomain, bool bMustHaveMovesLeft, bool bAllowEmbarkation, int iMaxNumUnits=3, bool bPlunderTradeRoute=false);
	bool FindParatroopersWithinStrikingDistance(CvPlot *pTargetPlot, bool bCheckDanger);
	bool FindEmbarkedUnitsAroundTarget(CvPlot *pTargetPlot, int iMaxDistance);
	bool FindCitiesWithinStrikingDistance(CvPlot* pTargetPlot);
	CvPlot* FindAirTargetNearTarget(CvUnit* pUnit, CvPlot* pApproximateTargetPlot);

	int GetRecruitRange() const;

	void FindAirUnitsToAirSweep(CvPlot* pTarget);

	int ComputeTotalExpectedDamage(const CvTacticalTarget& target);
	int ComputeTotalExpectedCityBombardDamage(CvUnit* pTarget);
	bool IsExpectedToDamageWithRangedAttack(CvUnit* pAttacker, CvPlot* pTarget, int iMinDamage=0);

	bool MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTargetPlot, DomainTypes eDomain, int iMaxTurns, bool bMustBeSafePath = false);

	CvPlot* FindBestBarbarianLandTarget(CvUnit* pUnit);
	CvPlot* FindBestBarbarianSeaTarget(CvUnit* pUnit);
	CvPlot* FindBarbarianExploreTarget(CvUnit* pUnit);
	CvPlot* FindNearbyTarget(CvUnit* pUnit, int iMaxTurns, bool bOffensive);
	bool IsHighPriorityCivilianTarget(CvTacticalTarget* pTarget);

	// Logging functions
	CvString GetLogFileName(const CvString& playerName) const;

	// Class data - some of it only temporary, does not need to be persisted
	CvPlayer* m_pPlayer;

	CvTacticalAnalysisMap m_tacticalMap;
	std::list<int> m_CurrentTurnUnits;
	std::vector<CvTacticalUnit> m_CurrentAirSweepUnits;
	CTacticalUnitArray m_CurrentMoveUnits;
	std::vector<CvTacticalCity> m_CurrentMoveCities;

	// Lists of targets for the turn
	vector<CvTacticalTarget> m_AllTargets;
	vector<CvTacticalTarget*> m_ZoneTargets; //pointers only!
	vector<CvTacticalTarget> m_NavalBlockadePoints;

	// Targeting ranges (pulled in from GlobalAIDefines.XML)
	int m_iRecruitRange;
	int m_iLandBarbarianRange;
	int m_iSeaBarbarianRange;

	// Dominance zone info
	int m_eCurrentTargetType;
	int m_iCurrentTargetIndex;

	std::vector<CvFocusArea> m_focusAreas;
};

FDataStream& operator>>(FDataStream&, CvTacticalAI&);
FDataStream& operator<<(FDataStream&, const CvTacticalAI&);

enum CLOSED_ENUM eUnitMoveEvalMode { EM_INITIAL, EM_INTERMEDIATE, EM_FINAL };
enum CLOSED_ENUM eUnitMovementStrategy { MS_NONE,MS_FIRSTLINE,MS_SECONDLINE,MS_THIRDLINE,MS_SUPPORT,MS_EMBARKED }; //we should probably differentiate between regular ranged and siege ranged ...
enum CLOSED_ENUM eUnitAssignmentType { A_INITIAL, A_MOVE, A_MELEEATTACK, A_MELEEKILL, A_RANGEATTACK, A_RANGEKILL, A_FINISH,
							A_BLOCKED, A_PILLAGE, A_CAPTURE, A_MOVE_FORCED, A_RESTART, A_MELEEKILL_NO_ADVANCE, A_MOVE_SWAP, A_MOVE_SWAP_REVERSE, A_USE_POWER, A_FINISH_TEMP };

struct STacticalAssignment
{
	//todo: use smaller data types
	eUnitAssignmentType eAssignmentType;
	int iUnitID;
	int iScore;
	int iFromPlotIndex;
	int iToPlotIndex;
	int iRemainingMoves;
	eUnitMovementStrategy eMoveType;

	int iDamage; //just in case of attack, not set in constructor
	int iSelfDamage; //only relevant for melee ...
	int iKillOrNearKillId; //unit or city we might have killed (for danger estimation)

	//convenience constructor
	STacticalAssignment(int iFromPlot = 0, int iToPlot = 0, int iUnitID_ = 0, int iRemainingMoves_ = 0, eUnitMovementStrategy eMoveType_ = MS_NONE, int iScore_ = 0, eUnitAssignmentType eType_ = A_FINISH) :
		eAssignmentType(eType_), iUnitID(iUnitID_), iScore(iScore_), iFromPlotIndex(iFromPlot), iToPlotIndex(iToPlot), 
		iRemainingMoves(iRemainingMoves_), eMoveType(eMoveType_), iDamage(0), iSelfDamage(0), iKillOrNearKillId(-1) {}

	//sort descending
	bool operator<(const STacticalAssignment& rhs) const { return iScore>rhs.iScore; }
	bool operator==(const STacticalAssignment& rhs) const;
};

struct SComboMove
{
	STacticalAssignment a, b;
	bool operator<(const SComboMove& rhs) const { return a.iScore+b.iScore > rhs.a.iScore+rhs.b.iScore; }
	bool operator==(const SComboMove& rhs) const { return a == rhs.a && b == rhs.b; }
	bool addMove(const STacticalAssignment& move);
};

struct STacticalUnit
{
	int iUnitID;
	eUnitMovementStrategy eMoveType;

	//convenience constructor
	STacticalUnit(int iUnitID_ = 0, eUnitMovementStrategy eMoveType_ = MS_NONE) : iUnitID(iUnitID_), eMoveType(eMoveType_) {}
};

struct SUnitStats
{
	int iUnitID;
	int iPlotIndex;
	int iAttacksLeft;
	int iMovesLeft;
	int iImportanceScore;
	int iSelfDamage; //melee units take damage when attacking
	int iMaxMoves;
	eUnitAssignmentType eLastAssignment;
	eUnitMovementStrategy eMoveStrategy;
	const CvUnit* pUnit;

	//convenience constructor - do not use pUnit here because it's initialized last! (pUnit_ is ok)
	SUnitStats(const CvUnit* pUnit_, int iImportance, eUnitMovementStrategy eMoveStrategy_) :
		iUnitID(pUnit_->GetID()), iPlotIndex(pUnit_->plot()->GetPlotIndex()), iAttacksLeft(pUnit_->getNumAttacks() - pUnit_->getNumAttacksMadeThisTurn()), 
		iMovesLeft(pUnit_->getMoves()), iMaxMoves(pUnit_->maxMoves()), iImportanceScore(iImportance), iSelfDamage(0), eLastAssignment(A_INITIAL), eMoveStrategy(eMoveStrategy_), pUnit(pUnit_) { }
	//use with caution, this may lead to an inconsistent state
	SUnitStats(const CvUnit* pUnit_, int iUnit, int iPlot, int iAttacks, int iMoves, int iImportance, eUnitMovementStrategy eMoveStrategy_) :
		iUnitID(iUnit), iPlotIndex(iPlot), iAttacksLeft(iAttacks), iMovesLeft(iMoves), iMaxMoves(pUnit_->maxMoves()), iImportanceScore(iImportance), iSelfDamage(0),
		eLastAssignment(A_INITIAL), eMoveStrategy(eMoveStrategy_), pUnit(pUnit_) {}

	bool operator<(const SUnitStats& rhs) const { return iImportanceScore > rhs.iImportanceScore; } //sort descending by default
};

struct SPathFinderStartPos
{
	int iUnitID;
	int iPlotIndex;
	int iMovesLeft;
	PlotIndexContainer freedPlots;

	SPathFinderStartPos(const SUnitStats& startpoint, const PlotIndexContainer& noZocPlots) :
		iUnitID(startpoint.iUnitID), iPlotIndex(startpoint.iPlotIndex), iMovesLeft(startpoint.iMovesLeft), freedPlots(noZocPlots) {}

	bool operator==(const SPathFinderStartPos& rhs) const
	{
		return (iUnitID == rhs.iUnitID) && (iPlotIndex == rhs.iPlotIndex) && (iMovesLeft == rhs.iMovesLeft) && (freedPlots == rhs.freedPlots);
	}

	bool operator<(const SPathFinderStartPos& rhs) const
	{
		//unit id is primary feature
		if (iUnitID < rhs.iUnitID)
			return true;
		if (iUnitID > rhs.iUnitID)
			return false;
		//equal unit, check plot
		if (iPlotIndex < rhs.iPlotIndex)
			return true;
		if (iPlotIndex > rhs.iPlotIndex)
			return false;
		//equal plot, check moves
		if (iMovesLeft < rhs.iMovesLeft)
			return true;
		if (iMovesLeft > rhs.iMovesLeft)
			return false;
		//no good way to compare this ...
		if (freedPlots.size() < rhs.freedPlots.size())
			return true;
		if (freedPlots.size() > rhs.freedPlots.size())
			return false;
		for (size_t i = 0; i < freedPlots.size(); i++)
		{
			if (freedPlots[i] < rhs.freedPlots[i])
				return true;
			if (freedPlots[i] > rhs.freedPlots[i])
				return false;
		}

		return false;
	}
};

// specialized hash function for unordered_map keys
struct SPathFinderStartPosHash
{
	void hash_combine(size_t& hash, const size_t& extra) const
	{
		hash ^= extra + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	std::size_t operator() (const SPathFinderStartPos& key) const
	{
		std::size_t h0 = key.freedPlots.size();

		hash_combine(h0, tr1::hash<int>()(key.iUnitID));
		hash_combine(h0, tr1::hash<int>()(key.iPlotIndex));
		hash_combine(h0, tr1::hash<int>()(key.iMovesLeft));

		for(vector<int>::const_iterator i = key.freedPlots.begin(); i!=key.freedPlots.end(); ++i)
			hash_combine(h0, tr1::hash<int>()(*i));

		return h0;
	}
};

struct SIntPairHash
{
	void hash_combine(size_t& hash, const size_t& extra) const
	{
		hash ^= extra + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	std::size_t operator() (const pair<int, int>& key) const
	{
		std::size_t h0 = tr1::hash<int>()(key.first);
		hash_combine(h0, tr1::hash<int>()(key.second));
		return h0;
	}
};

typedef tr1::unordered_map<SPathFinderStartPos, ReachablePlots, SPathFinderStartPosHash> TCachedMovePlots;
typedef tr1::unordered_map<pair<int, int>, vector<int>, SIntPairHash> TCachedRangeAttackPlots; // (unit:plot) -> plots
typedef tr1::unordered_map<int, unsigned char> TUnitFlagLookup;

//forward
class CvTacticalPosition;
class CvTactPosStorage;

class CvTacticalPlot
{
public:
	enum eTactPlotDomain { TD_LAND, TD_SEA, TD_BOTH };

	CvTacticalPlot(const CvPlot* plot=NULL, PlayerTypes ePlayer=NO_PLAYER, const vector<CvUnit*>& allOurUnits= vector<CvUnit*>());
	bool operator!=(const CvTacticalPlot& rhs) const;

	const CvPlot* getPlot() const { return pPlot; }
	int getPlotIndex() const { return pPlot ? pPlot->GetPlotIndex() : -1; }
	bool isChokepoint() const { return pPlot ? pPlot->IsChokePoint() : false; }
	bool isAdjacent(const CvTacticalPlot& other) const { return pPlot ? pPlot->isAdjacent(other.pPlot) : false; }
	int getNumAdjacentEnemies(eTactPlotDomain eDomain) const { return aiEnemyCombatUnitsAdjacent[eDomain]; }
	void setNumAdjacentEnemies(eTactPlotDomain eDomain, int iValue) { aiEnemyCombatUnitsAdjacent[eDomain]=static_cast<unsigned char>(iValue); }
	int getNumAdjacentFriendlies(eTactPlotDomain eDomain, int iIgnoreUnitPlot) const;
	int getNumAdjacentFriendliesEndTurn(eTactPlotDomain eDomain) const;
	const vector<STacticalUnit>& getUnitsAtPlot() const { return vUnitsHere; }

	bool isEnemy(eTactPlotDomain eDomain = TD_BOTH) const { return aiEnemyDistance[eDomain]==0; }
	bool isEnemyCity() const { return isEnemy() && pPlot->isCity(); }
	bool isEnemyCivilian() const { return bEnemyCivilianPresent; }

	bool isEdgePlot() const { return bEdgeOfTheKnownWorld; }
	bool isNextToEnemyCitadel() const { return bAdjacentToEnemyCitadel; }
	void setNextToEnemyCitadel(bool bValue) { bAdjacentToEnemyCitadel = bValue; }
	bool hasAirCover() const { return bHasAirCover; }
	bool isVisibleToEnemy() const { return bIsVisibleToEnemy; }
	bool isBlockedByNonSimUnit(eTactPlotDomain eDomain = TD_BOTH, bool bMustBeFriendly = false) const;
	bool isNicePlotForCitadel() const { return bMightWantCitadel; }

	bool hasFriendlyCombatUnit() const;
	bool hasFriendlyEmbarkedUnit() const;
	bool hasSupportBonus(int iIgnoreUnitPlot) const;

	void changeDamage(int iDamage) { iDamageDealt += iDamage; }
	int getDamage() const { return iDamageDealt; }

	void resetVolatileProperties();
	//update fictional state
	void friendlyUnitMovingIn(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment);
	void friendlyUnitMovingOut(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment);
	bool removeEnemyUnitIfPresent();

	unsigned char getEnemyDistance(eTactPlotDomain eDomain = TD_BOTH) const;
	void setEnemyDistance(eTactPlotDomain eDomain, int iDistance);
	bool checkEdgePlotsForSurprises(const CvTacticalPosition& currentPosition, vector<int>& landEnemies, vector<int>& seaEnemies);
	bool isValid() const { return pPlot != NULL; }
	bool isEnemyCombatUnit() const { return pEnemyCombatUnit!=NULL; }
	CvUnit* getEnemyUnit() const { return pEnemyCombatUnit; }
	bool isCombatEndTurn() const { return bFriendlyDefenderEndTurn; }
	void changeNeighboringUnitCount(CvTacticalPosition& currentPosition, eUnitMovementStrategy moveType, eTactPlotDomain unitDomain, int iChange) const;
	void setCombatUnitEndTurn(CvTacticalPosition& currentPosition, eTactPlotDomain unitDomain, bool bOverride=false);

	int getNumVisiblePlotsRange2() const { return (int)nVisiblePlotsNearEnemyRange2; }
	int getNumVisiblePlotsRange3() const { return (int)nVisiblePlotsNearEnemyRange2; }
	void setNumVisiblePlotsRange2(int count) { nVisiblePlotsNearEnemyRange2 = (unsigned char)count; }
	void setNumVisiblePlotsRange3(int count) { nVisiblePlotsNearEnemyRange3 = (unsigned char)count; }

	bool hasCoverFromOtherUnits(const CvTacticalPosition& currentPosition) const;

protected:
	const CvPlot* pPlot; //null if invalid
	CvUnit* pEnemyCombatUnit; //there may also be enemy cities without garrison!
	vector<STacticalUnit> vUnitsHere; //which (simulated) units are in this plot?

	unsigned char aiEnemyDistance[3]; //distance to attack targets, not civilians. recomputed every time an enemy is killed or discovered
	unsigned char aiEnemyCombatUnitsAdjacent[3]; //recomputed every time an enemy is killed or discovered

	unsigned char aiFriendlyCombatUnitsAdjacent[3]; //for flanking. set initially and updated every time a unit moves
	unsigned char aiFriendlyCombatUnitsAdjacentEndTurn[3]; //ranged units need cover. updated every time a unit finishes
	unsigned char nSupportUnitsAdjacent; //for general bonus (not differentiated by domain)
	unsigned char iDamageDealt; //damage dealt to this plot in previous simulated attacks

	unsigned char nVisiblePlotsNearEnemyRange2; //what can we see from here
	unsigned char nVisiblePlotsNearEnemyRange3; //what can we see from here

	//set once and not changed afterwards
	unsigned char bfBlockedByNonSimCombatUnit; //bitfield per domain
	//avoid these
	bool bIsVisibleToEnemy:1;
	//avoid these
	bool bHasAirCover:1;
	//this is updated if the civilian is captured
	bool bEnemyCivilianPresent:1;
	//so generals know where to plant themselves
	bool bMightWantCitadel:1;
	//when a new plot is created we don't know its status yet
	bool bEdgeOfTheKnownWorldUnknown : 1;
	//neighboring plot is not part of sim and invisible, updated after adding a newly visible plot
	bool bEdgeOfTheKnownWorld : 1;
	//updated after pillage
	bool bAdjacentToEnemyCitadel:1;
	//updated on turn end
	bool bFriendlyDefenderEndTurn:1; 
};

struct SDefendStats
{
	int iDefenderPlot;
	int iDefenderPrevDamage;
	UnitIdContainer killedEnemies;
	int iDanger;

	SDefendStats(int iDefenderPlot_, int iDefenderPrevDamage_, const UnitIdContainer& killedEnemies_, int iDanger_)
	{
		iDefenderPlot = iDefenderPlot_;
		iDefenderPrevDamage = iDefenderPrevDamage_;
		killedEnemies = killedEnemies_;
		iDanger = iDanger_;
	}
};

struct SAttackStats
{
	int iAttackerPlot;
	int iDefenderId;
	int iDefenderPrevDamage;
	int iAttackerDamageDealt;
	int iAttackerDamageTaken;

	SAttackStats(int iAttackerPlot_, int iDefenderId_, int iDefenderPrevDamage_, int iAttackerDamageDealt_, int iAttackerDamageTaken_)
	{
		iAttackerPlot = iAttackerPlot_;
		iDefenderId = iDefenderId_;
		iDefenderPrevDamage = iDefenderPrevDamage_;
		iAttackerDamageDealt = iAttackerDamageDealt_;
		iAttackerDamageTaken = iAttackerDamageTaken_;
	}
};

class CDangerCache {
public:
	void clear();
	void storeDanger(int iDefenderId, int iDefenderPlot, int iPrevDamage, const UnitIdContainer& killedEnemies, int iDanger);
	bool findDanger(int iDefenderId, int iDefenderPlot, int iPrevDamage, const UnitIdContainer& killedEnemies, int& iDanger) const;
protected:
	//key is defender id
	vector<pair<int, vector<SDefendStats>>> dangerStats;
};

class CAttackCache {
public:
	void clear();
	void storeAttack(int iAttackerId, int iAttackerPlot, int iDefenderId, int iPrevDamage, int iDamageDealt, int iDamageTaken);
	bool findAttack(int iAttackerId, int iAttackerPlot, int iDefenderId, int iPrevDamage, int& iDamageDealt, int& iDamageTaken) const;
protected:
	//key is attacker id
	vector<pair<int, vector<SAttackStats>>> attackStats;
};

class CvTacticalPosition
{
protected:
	//for final sorting (does not include intermediate moves)
	int iTotalScore;
	//for heap sorting (included all moves made in the last generation)
	int iScoreOverParent;

	//moves already assigned to get into this position (complete, mostly redundant with parent)
	vector<STacticalAssignment> assignedMoves;

	//the positions we can reach by adding more moves
	vector<CvTacticalPosition*> childPositions;

	//so we can look up stuff we haven't cached locally
	const CvTacticalPosition* parentPosition;

	vector<SUnitStats> availableUnits; //units which still need an assignment
	vector<SUnitStats> notQuiteFinishedUnits; //unit which have no moves left and we need to do a deferred check if it's ok to stay in the plot

	typedef vector<pair<unsigned short, unsigned char>> TactPlotIndexByPlotIndex; //save some bits
	TactPlotIndexByPlotIndex tactPlotLookup; //map from plot index to storage index
	vector<CvTacticalPlot> tactPlots; //storage for tactical plots (complete, mostly redundant with parent)

	PlotIndexContainer enemyPlots; //plot indices for enemy units, to be checked for potential attacks
	PlotIndexContainer freedPlots; //plot indices for killed enemy units, to be ignored for ZOC
	UnitIdContainer killedEnemies; //enemy units which were killed, to be ignored for danger
	int movePlotUpdateFlagA; //zero for nothing to do, unit id for a specific unit, -1 for all units
	int movePlotUpdateFlagB; //zero for nothing to do, unit id for a specific unit, -1 for all units

	//set in constructor, constant afterwards
	PlayerTypes ePlayer;
	eAggressionLevel eAggression;
	unsigned char nOurOriginalUnits; //movable units included in sim (inherited from root)
	unsigned char nOriginalEnemies; //enemy units and cities. ignoring garrisons. not updated after sim-kills!
	unsigned char nFirstInterestingAssignment; //in case we want to skip INITIALs and BLOCKEDs
	CvPlot* pTargetPlot;

	//should be unique
	size_t iGeneration;
	unsigned long long iID;

	//dummy to avoid returning temporaries
	CvTacticalPlot dummyPlot;

	//------------
	const ReachablePlots& getReachablePlotsForUnit(const SUnitStats& unit) const;
	const vector<int>& getRangeAttackPlotsForUnit(const SUnitStats& unit) const;
	void getPreferredAssignmentsForUnit(const SUnitStats& unit, int nMaxCount) const;
	size_t addChild(CvTacticalPosition* pChild);
	bool removeChild(CvTacticalPosition* pChild);
	bool isMoveBlockedByOtherUnit(const STacticalAssignment& move) const;
	void getPlotsWithChangedVisibility(const STacticalAssignment& assignment, vector<int>& madeVisible) const;
	void updateMoveAndAttackPlotsForUnit(SUnitStats unit);
	vector<CvTacticalPlot>::iterator findTactPlot(int iPlotIndex);
	vector<CvTacticalPlot>::const_iterator findTactPlot(int iPlotIndex) const;

	//finding a particular unit
	struct PrMatchingUnit
	{
		int iUnitID;
		PrMatchingUnit(int iID) : iUnitID(iID) {}
		bool operator()(const SUnitStats& other) { return iUnitID == other.iUnitID; }
	};

	enum AddAssignmentResult
	{
		RESULT_NOOP,
		RESULT_ADDED,
		RESULT_ADDED_W_VIS_CHANGE,
		RESULT_NOT_ADDED
	};

public:
	struct PrPositionSortHeapGeneration
	{
		//this is quite tricky. initially we want to explore all sorts of moves, so go breadth-first
		//later on we want to finish the most promising positions quickly, so go depth-first
		bool bDepthFirst;

		PrPositionSortHeapGeneration(bool depthFirst) : bDepthFirst(depthFirst) {}

		//we will pop the *last* element from the heap later
		bool operator()(const CvTacticalPosition* lhs, const CvTacticalPosition* rhs) const
		{
			//within a generation always sort by (last round) score
			//continue where it seems most promising right now, ignore "historical" scores
			if (lhs->getGeneration() == rhs->getGeneration())
				return lhs->getScoreLastRound() < rhs->getScoreLastRound();

			if (bDepthFirst)
				//we want to pick the highest generation first
				return lhs->getGeneration() < rhs->getGeneration();
			else
				//we want to pick the lowest generation first
				return lhs->getGeneration() > rhs->getGeneration();
		}
	};

	struct PrPositionSortArrayTotalScore
	{
		bool operator()(const CvTacticalPosition* lhs, const CvTacticalPosition* rhs) const
		{
			if (lhs->getScoreTotal() == rhs->getScoreTotal())
				//equal ... try to use number of moves as a tiebraker
				return lhs->getAssignments().size() < rhs->getAssignments().size();
			
			//regular descending sort. only makes sense for "completed" positions!
			return lhs->getScoreTotal() > rhs->getScoreTotal();
		}
	};

	CvTacticalPosition();

	void initFromScratch(PlayerTypes player, eAggressionLevel eAggLvl, CvPlot* pTarget);
	void initFromParent(const CvTacticalPosition& parent);
	void wipe();

	bool isExhausted() const;
	bool isEarlyFinish() const;
	bool haveEnemies() const;
	bool addFinishMovesIfAcceptable(bool bEarlyFinish, int& iBadUnitID);
	bool isKillOrImprovedPosition() const;
	void countEnemiesAndCheckVisibility();
	void refreshVolatilePlotProperties(bool bInitial = false);
	void dropSuperfluousUnits(int iMaxUnitsToKeep);
	void addInitialAssignments();
	bool makeNextAssignments(int iMaxBranches, int iMaxChoicesPerUnit, CvTactPosStorage& storage, 
		vector<CvTacticalPosition*>& openPositionsHeap, vector<CvTacticalPosition*>& completedPositions, const PrPositionSortHeapGeneration& heapSort);
	void updateMovePlotsIfRequired();
	bool addTacticalPlot(const CvPlot* pPlot, const vector<CvUnit*>& allOurUnits);
	bool addAvailableUnit(const CvUnit* pUnit);
	const vector<SUnitStats>& getAvailableUnits() const { return availableUnits; }
	int countChildren() const;
	float getAggressionBias() const;
	bool canProbablyEndTurnAfterAssignment(const STacticalAssignment& assignment, const CvTacticalPlot& endPlot) const;
	int countBlockingUnitsAtPlot(int iPlotIndex, eUnitMovementStrategy moveType) const;
	int getFirstBlockingUnitIDAtPlot(int iPlotIndex, eUnitMovementStrategy moveType) const;
	pair<int,int> doVisibilityUpdate(const STacticalAssignment& newAssignment);
	bool lastAssignmentIsAfterRestart(int iUnitID) const;
	const SUnitStats* getAvailableUnitStats(int iUnitID) const;
	const STacticalAssignment* getInitialAssignment(int iUnitID) const;
	STacticalAssignment* getInitialAssignmentMutable(int iUnitID);
	const STacticalAssignment* getLatestAssignment(int iUnitID) const;
	STacticalAssignment* getLatestAssignmentMutable(int iUnitID);
	const STacticalAssignment* getLatestMoveAssignment(int iUnitID) const;
	bool unitHasAssignmentOfType(int iUnitID, eUnitAssignmentType assignmentType) const;
	bool plotHasAssignmentOfType(int iToPlotIndex, eUnitAssignmentType assignmentType) const;
	bool isAttackablePlot(int iPlotIndex) const;
	AddAssignmentResult addAssignment(const STacticalAssignment& newAssignment);
	bool isUnique(int levelsToCheck=2) const;
	void setFirstInterestingAssignment(size_t i);
	size_t getFirstInterestingAssignment() const;

	const CvTacticalPlot& getTactPlot(int plotindex) const; //get a reference to a local tact plot or higher up in the tree if we didn't modify it
	CvTacticalPlot& getTactPlotMutable(int plotindex); //this is dangerous! the reference returned by one call may become invalid when calling this a second time
	int getGeneration() const { return iGeneration; }

	CvPlot* getTarget() const { return pTargetPlot; }
	eAggressionLevel getAggressionLevel() const { return eAggression; }
	PlayerTypes getPlayer() const { return ePlayer; }
	int getScoreTotal() const { return iTotalScore; }
	int getScoreLastRound() const { return iScoreOverParent; }

	const CvTacticalPosition* getParent() const { return parentPosition; }
	const vector<CvTacticalPosition*>& getChildren() const { return childPositions; }
	const vector<STacticalAssignment>& getAssignments() const { return assignedMoves; }
	const UnitIdContainer& getKilledEnemies() const { return killedEnemies; }
	const PlotIndexContainer& getFreedPlots() const { return freedPlots; }
	int getNumEnemies() const { return nOriginalEnemies - killedEnemies.size(); }
	int getNumPlots() const { return (int)tactPlots.size(); }
	int getTotalNumFriendlyUnits() const { return (int)nOurOriginalUnits; }

	//sort descending cumulative score. only makes sense for "completed" positions
	bool operator<(const CvTacticalPosition& rhs) { return iTotalScore>rhs.iTotalScore; }

	//debugging
	unsigned long long getID() const { return iID; }
	void dumpPlotStatus(const char* fname) const;
	void exportToDotFile(const char* fname) const;
	void dumpChildren(ofstream& out) const;

	friend const CvTacticalPosition* tacticalPositionIsEquivalent(const CvTacticalPosition* ref, const CvTacticalPosition* other);
};

class CvTactPosStorage
{
public:
	CvTactPosStorage(int iPreallocationSize) : iCount(0), iSize(iPreallocationSize), aPositions(new CvTacticalPosition[iPreallocationSize]), attackCache() {}
	~CvTactPosStorage() { delete[] aPositions; }
	void reset(bool bHard);
	int getSizeLimit() const { return iSize; }
	int getSize() const { return iCount; }
	CvTacticalPosition* first() { return aPositions; }
	CvTacticalPosition* peekNext() { return (iCount < iSize) ? aPositions + iCount : NULL; }
	int consumeOne() { return iCount++; }
	CAttackCache& getAttackCache() { return attackCache; }
	CDangerCache& getDangerCache() { return dangerCache; }

protected:
	int iSize; //how many do we have
	int iCount; //how many are currently in use
	CvTacticalPosition* aPositions; //preallocated block of N positions
	CAttackCache attackCache; //performance optimization, unit strength calculation takes too long
	CDangerCache dangerCache; //performance optimization, unit strength calculation takes too long

private:
	//hide copy constructor and assignment operator
	CvTactPosStorage(const CvTactPosStorage& other);
	const CvTactPosStorage& operator=(const CvTactPosStorage& rhs);
};


namespace TacticalAIHelpers
{
	bool SortByExpectedTargetDamageDescending(const CvTacticalUnit& obj1, const CvTacticalUnit& obj2);

	ReachablePlots GetAllPlotsInReachThisTurn(const CvUnit* pUnit, const CvPlot* pStartPlot, int iFlags, int iMinMovesLeft=0, int iStartMoves=-1, const PlotIndexContainer& plotsToIgnoreForZOC=PlotIndexContainer());
	vector<int> GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, const CvPlot* pBasePlot, bool bOnlyWithEnemy, bool bIgnoreVisibility);
	std::set<int> GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, ReachablePlots& basePlots, bool bOnlyWithEnemy,  bool bIgnoreVisibility);

	bool PerformRangedOpportunityAttack(CvUnit* pUnit, bool bAllowMovement = false);
	bool PerformOpportunityAttack(CvUnit* pUnit, bool bAllowMovement = false);
	bool IsAttackNetPositive(CvUnit* pUnit, const CvPlot* pTarget);
	CvPlot* FindSafestPlotInReach(const CvUnit* pUnit, bool bAllowEmbark, bool bConsiderPush = false);
	CvPlot* FindClosestSafePlotForHealing(CvUnit* pUnit, bool bConservative = true);
	bool IsGoodPlotForStaging(CvPlayer* pPlayer, CvPlot* pCandidate, DomainTypes eDomain);
	bool IsCloseToContestedBorder(CvPlayer* pPlayer, CvPlot* pPlot);

	std::vector<CvPlot*> GetPlotsForRangedAttack(const CvPlot* pTarget, const CvUnit* pUnit, int iRange, bool bCheckOccupied);
	int GetSimulatedDamageFromAttackOnUnit(const CvUnit* pDefender, const CvUnit* pAttacker, const CvPlot* pDefenderPlot, const CvPlot* pAttackerPlot, int& iAttackerDamage, 
									bool bIgnoreUnitAdjacencyBoni=false, int iExtraDefenderDamage=0, bool bQuickAndDirty = false);
	int GetSimulatedDamageFromAttackOnCity(const CvCity* pCity, const CvUnit* pAttacker, const CvPlot* pAttackerPlot, int& iAttackerDamage, 
									bool bIgnoreUnitAdjacencyBoni=false, int iExtraDefenderDamage=0, bool bQuickAndDirty = false);
	bool KillLoneEnemyIfPossible(CvUnit* pOurUnit, CvUnit* pEnemyUnit);
	bool IsSuicideMeleeAttack(const CvUnit* pAttacker, CvPlot* pTarget);
	bool CanKillTarget(const CvUnit* pAttacker, CvPlot* pTarget);
	vector<pair<CvPlot*,bool>> GetTargetsInRange(const CvUnit* pUnit, bool bMustBeAbleToKill=false, bool bIncludeCivilians=true);
	pair<int, int> EstimateLocalUnitPower(const ReachablePlots& plotsToCheck, TeamTypes eTeamA, TeamTypes eTeamB, bool bMustBeVisibleToBoth);
	int CountAdditionallyVisiblePlots(CvUnit* pUnit, CvPlot* pTestPlot);
	bool IsPlayerCitadel(const CvPlot* pPlot, PlayerTypes ePlayer);
	bool IsOtherPlayerCitadel(const CvPlot* pPlot, PlayerTypes ePlayer, bool bCheckWar);
	int SentryScore(const CvPlot* pPlot, PlayerTypes ePlayer);

	bool FindAndExecuteBestUnitAssignments(vector<CvUnit*>& vUnits, CvPlot* pTarget, eAggressionLevel eAggLvl);
	vector<STacticalAssignment> FindBestUnitAssignments(const vector<CvUnit*>& vUnits, CvPlot* pTarget, eAggressionLevel eAggLvl, CvTactPosStorage& storage, set<int>& unuseableUnits);
	bool ExecuteUnitAssignments(PlayerTypes ePlayer, const vector<STacticalAssignment>& vAssignments);
}

extern const char* tacticalMoveNames[];
extern const char* postureNames[];
extern const char* assignmentTypeNames[];

#endif //CIV5_TACTICAL_AI_H
