﻿/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalMoveXMLEntry
//!  \brief		A single entry in the tactical move XML file
//
//!  Key Attributes:
//!  - Populated from XML\AI\CIV5TacticalMoves.xml
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTacticalMoveXMLEntry: public CvBaseInfo
{
public:
	CvTacticalMoveXMLEntry();
	virtual ~CvTacticalMoveXMLEntry();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions
	int GetPriority() const
	{
		return m_iPriority;
	};
	void SetPriority(int iPriority)
	{
		m_iPriority = iPriority;
	};
	int GetOffenseFlavorWeight() const
	{
		return m_iOffenseWeight;
	};
	void SetOffenseFlavorWeight(int iWeight)
	{
		m_iOffenseWeight = iWeight;
	};
	int GetDefenseFlavorWeight() const
	{
		return m_iDefenseWeight;
	};
	void SetDefenseFlavorWeight(int iWeight)
	{
		m_iDefenseWeight = iWeight;
	};
	int CanRecruitForOperations() const
	{
		return m_bOperationsCanRecruit;
	};
	void SetRecruitForOperations(bool bAvailable)
	{
		m_bOperationsCanRecruit = bAvailable;
	};
	int IsDominanceZoneMove() const
	{
		return m_bDominanceZoneMove;
	};
	void SetDominanceZoneMove(bool bByPosture)
	{
		m_bDominanceZoneMove = bByPosture;
	};

private:
	TacticalAIMoveTypes m_eMoveType;
	int m_iPriority;
	int m_iOffenseWeight;
	int m_iDefenseWeight;
	bool m_bOperationsCanRecruit;
	bool m_bDominanceZoneMove;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalMoveXMLEntries
//!  \brief		Game-wide information about tactical move priorities
//
//! Key Attributes:
//! - Populated from XML\AI\Civ5TacticalMoves.xml
//! - Contains an array of CvTacticalMoveXMLEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvTacticalAI class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTacticalMoveXMLEntries
{
public:
	CvTacticalMoveXMLEntries(void);
	~CvTacticalMoveXMLEntries(void);

	// Accessor functions
	std::vector<CvTacticalMoveXMLEntry*>& GetTacticalMoveEntries();
	int GetNumTacticalMoves();
	CvTacticalMoveXMLEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvTacticalMoveXMLEntry*> m_paTacticalMoveEntries;
};

//for tactical combat
enum eAggressionLevel { AL_NONE, AL_LOW, AL_MEDIUM, AL_HIGH };

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

// Object stored in the list of move priorities (m_MovePriorityList)
class CvTacticalMove
{
public:
	CvTacticalMove()
	{
		m_eMoveType = NO_TACTICAL_MOVE;
		m_iPriority = 0;
		m_name = 0;
	}

	bool operator<(const CvTacticalMove& move) const
	{
		return (m_iPriority > move.m_iPriority);
	}

	const char* m_name;
	TacticalAIMoveTypes m_eMoveType;
	int m_iPriority;
};

struct ZoneMoveWithPrio
{
	CvTacticalMove move;
	class CvTacticalDominanceZone* pZone; //forward
	int prio;

	bool operator<(const ZoneMoveWithPrio& other) const {
		return prio > other.prio; //> intended for descending sort
	}
};

// Object stored in the list of current move units (m_CurrentMoveUnits)
class CvTacticalUnit
{
public:
	CvTacticalUnit();

	bool operator<(const CvTacticalUnit& unit) const
	{
		return (GetAttackPriority() > unit.GetAttackPriority());
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
	void SetHealthPercent(int curHitPoints, int maxHitPoints)
	{
		CvAssert(maxHitPoints != 0);
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

#ifdef MOD_AI_SMART_AIR_TACTICS 
	void SetInterceptor(CvUnit* pInterceptor)
	{
		m_pInterceptor = pInterceptor;
	}
	CvUnit* GetInterceptor() const
	{
		return m_pInterceptor;
	};
#endif

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
#ifdef MOD_AI_SMART_AIR_TACTICS
	CvUnit* m_pInterceptor;
#endif
};

// Object stored in the list of current move cities (m_CurrentMoveCities)
class CvTacticalCity
{
public:
	CvTacticalCity()
	{
		m_iID = 0;
		m_iAttackStrength = 0;
		m_iExpectedTargetDamage = 0;
	}

	bool operator<(const CvTacticalCity& city) const
	{
		return (GetAttackPriority() > city.GetAttackPriority());
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

	// Derived
	int GetAttackPriority() const
	{
		return m_iAttackStrength;
	}

private:
	int m_iID;
	int m_iAttackStrength;
	int m_iExpectedTargetDamage;
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
#if defined(MOD_BALANCE_CORE)
		m_iTargetX = INVALID_PLOT_COORD;
		m_iTargetY = INVALID_PLOT_COORD;
#else
		m_iTargetX = -1;
		m_iTargetY = -1;
#endif
		m_eTargetPlayer = NO_PLAYER;
		m_iZoneID = -1;
		m_pAuxData = NULL;
		m_iAuxData = 0;
		m_eAggLvl = AL_NONE;
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
	inline PlayerTypes GetTargetPlayer() const
	{
		return m_eTargetPlayer;
	}
	inline void SetTargetPlayer(PlayerTypes eValue)
	{
		m_eTargetPlayer = eValue;
	}
	inline int GetDominanceZone() const
	{
		return m_iZoneID;
	};
	inline void SetDominanceZone(int iZone)
	{
		m_iZoneID = iZone;
	};

	bool IsReadyForCapture();
	bool IsTargetStillAlive(PlayerTypes eAttackingPlayer);
	bool IsTargetValidInThisDomain(DomainTypes eDomain);

	void SetLastAggLvl(eAggressionLevel lvl) { m_eAggLvl = lvl; }
	eAggressionLevel GetLastAggLvl() const { return m_eAggLvl; }

	// AuxData is used for a pointer to the actual target object (CvUnit, CvCity, etc.)
	//    (for improvements & barbarian camps this is set to the plot).
	inline void* GetAuxData()
	{
		return m_pAuxData;
	}
	inline void SetAuxData(void* pAuxData)
	{
		m_pAuxData = pAuxData;
	}

	// Stores required damage for offensive targets
	// For defensive items used to SORT targets in priority order
	//    Set to the weight for defensive bastions
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
	AITacticalTargetType m_eTargetType;
	int m_iTargetX;
	int m_iTargetY;
	PlayerTypes m_eTargetPlayer;
	void* m_pAuxData;
	int m_iAuxData;
	int m_iZoneID;
	eAggressionLevel m_eAggLvl;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTacticalPosture
//!  \brief		The posture an AI has adopted for fighting in a specific dominance zone
//
//!  Key Attributes:
//!  - Used to keep consistency in approach from turn-to-turn
//!  - Reevaluated by tactical AI each turn before units are moved for this zone
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum AITacticalPosture
{
    AI_TACTICAL_POSTURE_NONE = -1,
    AI_TACTICAL_POSTURE_WITHDRAW,
    AI_TACTICAL_POSTURE_SIT_AND_BOMBARD, //not used anymore
    AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE,
    AI_TACTICAL_POSTURE_EXPLOIT_FLANKS,
    AI_TACTICAL_POSTURE_STEAMROLL,
    AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE,
    AI_TACTICAL_POSTURE_HEDGEHOG,
    AI_TACTICAL_POSTURE_COUNTERATTACK,
    AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT,
};

class CvTacticalPosture
{
public:
	CvTacticalPosture(PlayerTypes ePlayer, bool bIsWater, int iCityID, AITacticalPosture ePosture)
	{
		m_iPlayerWaterInfo = (int)ePlayer * 2 + (int)bIsWater;
		m_iCityID = iCityID;
		m_ePosture = ePosture;
	}

	PlayerTypes GetPlayer() const
	{
		return (PlayerTypes)(m_iPlayerWaterInfo / 2);
	};
	bool IsWater()
	{
		return m_iPlayerWaterInfo & 0x1;
	};
	int GetCityID()
	{
		return m_iCityID;
	};
	AITacticalPosture GetPosture()
	{
		return m_ePosture;
	};

private:
	int m_iPlayerWaterInfo;
	int m_iCityID;
	AITacticalPosture m_ePosture;
};

#define SAFE_ESTIMATE_NUM_TEMP_ZONES 10

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTemporaryZone
//!  \brief		Location of a temporary dominance zone (like around a barbarian camp)
//
//!  Key Attributes:
//!  - Used to add dominance zones for short duration tactical strikes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTemporaryZone
{
public:
	CvTemporaryZone()
	{
		Clear();
	}
	void Clear()
	{
		m_iX = 0;
		m_iY = 0;
		m_iLastTurn = 0;
	};

	int GetX() const
	{
		return m_iX;
	};
	void SetX(int iX)
	{
		m_iX = iX;
	};
	int GetY() const
	{
		return m_iY;
	};
	void SetY(int iY)
	{
		m_iY = iY;
	};
	int GetLastTurn() const
	{
		return m_iLastTurn;
	};
	void SetLastTurn(int iTurn)
	{
		m_iLastTurn = iTurn;
	};

private:
	int m_iX;
	int m_iY;
	int m_iLastTurn;
};

FDataStream& operator<<(FDataStream&, const CvTemporaryZone&);
FDataStream& operator>>(FDataStream&, CvTemporaryZone&);

#define SAFE_ESTIMATE_NUM_BLOCKING_UNITS 25

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBlockingUnit
//!  \brief		Potential move of a unit to a hex to form a block keeping enemy away
//
//!  Key Attributes:
//!  - Used by CanCoverFromEnemy() to track moves we may want to make to block enemy
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBlockingUnit
{
public:
	CvBlockingUnit()
	{
		m_pPlot = NULL;
		m_iUnitID = 0;
		m_iNumChoices = 0;
		m_iDistanceToTarget = 0;
	};
	bool operator<(const CvBlockingUnit& rhs) const
	{
		return (m_iUnitID < rhs.m_iUnitID);
	};
	CvPlot* GetPlot() const
	{
		return m_pPlot;
	};
	void SetPlot(CvPlot* pPlot)
	{
		m_pPlot = pPlot;
	};
	int GetUnitID() const
	{
		return m_iUnitID;
	};
	void SetUnitID(int iID)
	{
		m_iUnitID = iID;
	};
	int GetNumChoices() const
	{
		return m_iNumChoices;
	};
	void SetNumChoices(int iChoices)
	{
		m_iNumChoices = iChoices;
	};
	int GetDistanceToTarget() const
	{
		return m_iDistanceToTarget;
	};
	void SetDistanceToTarget(int iDistanceToTarget)
	{
		m_iDistanceToTarget = iDistanceToTarget;
	};

private:
	CvPlot* m_pPlot;
	int m_iUnitID;
	int m_iNumChoices;
	int m_iDistanceToTarget;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvOperationUnit
//!  \brief		One unit moving with operational army currently being processed
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvOperationUnit
{
public:
	CvOperationUnit()
	{
		m_iUnitID = 0;
		m_ePosition = NO_MUPOSITION;
	};
	int GetUnitID() const
	{
		return m_iUnitID;
	};
	void SetUnitID(int iID)
	{
		m_iUnitID = iID;
	};
	int GetPosition() const
	{
		return m_ePosition;
	};
	void SetPosition(MultiunitPositionTypes eType)
	{
		m_ePosition = eType;
	};

private:
	int m_iUnitID;
	MultiunitPositionTypes m_ePosition;
};

enum TacticalAIInfoTypes
{
	eTACTICAL_UNASSIGNED,
	eTACTICAL_POSTURE_SIT_AND_BOMBARD,
	eTACTICAL_POSTURE_ATTRIT_FROM_RANGE,
	eTACTICAL_POSTURE_EXPLOIT_FLANKS,
	eTACTICAL_POSTURE_STEAMROLL,
	eTACTICAL_POSTURE_SURGICAL_CITY_STRIKE,
	eTACTICAL_POSTURE_COUNTERATTACK,
	eTACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY,
	eTACTICAL_CAPTURE_CITY,
	eTACTICAL_DAMAGE_CITY,
	eTACTICAL_DESTROY_HIGH_UNIT,
	eTACTICAL_DESTROY_MEDIUM_UNIT,
	eTACTICAL_DESTROY_LOW_UNIT,
	eTACTICAL_TO_SAFETY,
	eTACTICAL_ATTRIT_HIGH_UNIT,
	eTACTICAL_ATTRIT_MEDIUM_UNIT,
	eTACTICAL_ATTRIT_LOW_UNIT,
	eTACTICAL_REPOSITION,
	eTACTICAL_BARBARIAN_CAMP,
	eTACTICAL_PILLAGE,
	eTACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN,
	eTACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN,
	eTACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN,
	eTACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN,
	eTACTICAL_SAFE_BOMBARDS,
	eTACTICAL_HEAL,
	eTACTICAL_ANCIENT_RUINS,
	eTACTICAL_GARRISON_TO_ALLOW_BOMBARD,
	eTACTICAL_GARRISON_ALREADY_THERE,
	eTACTICAL_BASTION_ALREADY_THERE,
	eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE,
	eTACTICAL_GARRISON_1_TURN,
	eTACTICAL_BASTION_1_TURN,
	eTACTICAL_BASTION_2_TURN,
	eTACTICAL_GUARD_IMPROVEMENT_1_TURN,
	eTACTICAL_AIR_INTERCEPT,
	eTACTICAL_AIR_SWEEP,
	eTACTICAL_POSTURE_HEDGEHOG,
	eTACTICAL_POSTURE_WITHDRAW,
	eTACTICAL_POSTURE_SHORE_BOMBARDMENT,
	eTACTICAL_CLOSE_ON_TARGET,
	eTACTICAL_MOVE_OPERATIONS,
	eTACTICAL_EMERGENCY_PURCHASES,
	eTACTICAL_ESCORT_EMBARKED_UNIT,
	eTACTICAL_PLUNDER_TRADE_UNIT,
	eTACTICAL_PARK_ON_TRADE_ROUTE,
	eTACTICAL_DEFENSIVE_AIRLIFT,
	eTACTICAL_PILLAGE_CITADEL,
	eTACTICAL_PILLAGE_RESOURCE,
	eTACTICAL_PILLAGE_CITADEL_NEXT_TURN,
	eTACTICAL_PILLAGE_RESOURCE_NEXT_TURN,
	eTACTICAL_PILLAGE_NEXT_TURN,
	eMUPOSITION_CIVILIAN_SUPPORT,
	eMUPOSITION_NAVAL_ESCORT,
	eMUPOSITION_BOMBARD,
	eMUPOSITION_FRONT_LINE,
	eNUM_TACTICAL_INFOTYPES
};

typedef vector<CvTacticalTarget> TacticalList;

#if defined(MOD_BALANCE_CORE)
//a simple wrapper around std::vector so we can log/break on certain units being added (in a central place)
class CTacticalUnitArray
{
public:
	CTacticalUnitArray() : m_owner(NULL) {}

	std::vector<CvTacticalUnit>::const_iterator begin() const { return m_vec.begin(); }
	std::vector<CvTacticalUnit>::const_iterator end() const { return m_vec.end(); }
	std::vector<CvTacticalUnit>::iterator begin() { return m_vec.begin(); }
	std::vector<CvTacticalUnit>::iterator end() { return m_vec.end(); }
	std::vector<CvTacticalUnit>::size_type size() const { return m_vec.size(); }
	std::vector<CvTacticalUnit>::reference operator[](std::vector<CvTacticalUnit>::size_type _Pos) { return m_vec[_Pos]; }
	std::vector<CvTacticalUnit>::iterator erase(std::vector<CvTacticalUnit>::const_iterator _Where) { return m_vec.erase(_Where); }
	void push_back(const CvTacticalUnit& unit);
	void clear() { m_vec.clear(); }
	void setPlayer(CvPlayer* pOwner) { m_owner=pOwner; }
	void setCurrentTacticalMove(CvTacticalMove move) { m_currentTacticalMove=move; }
	CvUnit* getUnit(size_t i) const { return m_owner ? m_owner->getUnit( m_vec[i].GetID() ) : NULL; }
	PlayerTypes getOwner() const { return m_owner ? m_owner->GetID() : NO_PLAYER; }
private:
	std::vector<CvTacticalUnit> m_vec;
	CvPlayer* m_owner;
	CvTacticalMove m_currentTacticalMove;
};
#endif

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
	void Reset();

	// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Public turn update routines
	void CommandeerUnits();
	void DoTurn();
	void Update();

	// Temporary dominance zones
	void AddTemporaryZone(CvPlot* pPlot, int iDuration);
	void DeleteTemporaryZone(CvPlot* pPlot);
	void DropObsoleteZones();
	bool IsTemporaryZoneCity(CvCity* pCity);

#if defined(MOD_BALANCE_CORE)
	bool IsUnitHealing(int iUnitID) const;
	bool ShouldRebase(CvUnit* pUnit) const;
	CvCity* GetNearestTargetCity(CvPlot* pPlot);
#endif

	// Public logging
	void LogTacticalMessage(const CvString& strMsg, bool bSkipLogDominanceZone = true);

	// Other people want to know this too
	AITacticalPosture FindPosture(CvTacticalDominanceZone* pZone);
	const TacticalList& GetTacticalTargets() const { return m_AllTargets; }
	CvTacticalAnalysisMap* GetTacticalAnalysisMap() { return &m_tacticalMap; }

	// Operational AI support functions
	void PlotArmyMovesEscort(CvArmyAI* pThisArmy);
	void PlotArmyMovesCombat(CvArmyAI* pThisArmy);
	void AddCurrentTurnUnit(CvUnit* pUnit);

	void UnitProcessed(int iID);
private:

	// Internal turn update routines - commandeered unit processing
	void UpdatePostures();
	AITacticalPosture SelectPosture(CvTacticalDominanceZone* pZone, AITacticalPosture eLastPosture);
	void EstablishTacticalPriorities();
	void EstablishBarbarianPriorities();
	void FindTacticalTargets();
	void PrioritizeNavalTargetsAndAddToMainList();
	void ProcessDominanceZones();
	void AssignTacticalMove(CvTacticalMove move);
	void AssignBarbarianMoves();

	// Routines to manage identifying and implementing tactical moves
	void PlotCaptureCityMoves();
	void PlotBarbarianCampMoves();
	void PlotDamageCivilianMoves(AITacticalTargetType targetType);
	void PlotDestroyUnitMoves(AITacticalTargetType targetType, bool bMustBeAbleToKill, bool bAttackAtPoorOdds=false);
	void PlotMovesToSafety(bool bCombatUnits);
	void PlotRepositionMoves();
	void PlotOperationalArmyMoves();
	void PlotPillageMoves(AITacticalTargetType eTarget, bool bImmediate);
	void PlotPlunderTradeUnitMoves(DomainTypes eDomain);
	void PlotPlunderTradePlotMoves(DomainTypes eDomain); // squat on trade plots to try to scoop up trade units
	void PlotBlockadeMoves();
	void PlotCivilianAttackMoves(AITacticalTargetType eTargetType);
	void PlotSafeBombardMoves();
	void PlotHealMoves();
	void PlotCampDefenseMoves();
	void PlotBarbarianMove(bool bAggressive);
	void PlotBarbarianCivilianEscortMove();
	void PlotGarrisonMoves(int iTurnsToArrive);
	void PlotBastionMoves(int iTurnsToArrive);
	void PlotGuardImprovementMoves(int iTurnsToArrive);
	void PlotAncientRuinMoves(int iNumTurnsAway);
	void PlotAirInterceptMoves();
	void PlotAirSweepMoves();
	void PlotAttritFromRangeMoves();
	void PlotExploitFlanksMoves();
	void PlotSteamrollMoves();
	void PlotSurgicalCityStrikeMoves();
	void PlotHedgehogMoves();
	void PlotCounterattackMoves();
	void PlotWithdrawMoves();
	void PlotShoreBombardmentMoves();
	void PlotCloseOnTarget();
	void PlotEmergencyPurchases();
	void PlotDefensiveAirlifts();
	void PlotEscortEmbarkedMoves();
	void ReviewUnassignedUnits();

	// Operational AI support functions
	bool ClearEnemiesNearArmy(CvArmyAI* pArmy);
	void ExecuteGatherMoves(CvArmyAI* pArmy, CvPlot* pTurnTarget);

	// Routines to process and sort targets
	void IdentifyPriorityTargets();
	void IdentifyPriorityBarbarianTargets();
	void IdentifyPriorityTargetsByType();
	void UpdateTargetScores();
	void SortTargetListAndDropUselessTargets();
	void DumpTacticalTargets(const char* hint);
	void ExtractTargetsForZone(CvTacticalDominanceZone* pZone /* Pass in NULL for all zones */);
	CvTacticalTarget* GetFirstZoneTarget(AITacticalTargetType eType, eAggressionLevel eMaxLvl = AL_NONE);
	CvTacticalTarget* GetNextZoneTarget(eAggressionLevel eMaxLvl = AL_NONE);
	CvTacticalTarget* GetFirstUnitTarget();
	CvTacticalTarget* GetNextUnitTarget();

	// Routines to execute a mission
	void ExecuteBarbarianCampMove(CvPlot* pTargetPlot);
	bool ExecutePillage(CvPlot* pTargetPlot);
	void ExecutePlunderTradeUnit(CvPlot* pTargetPlot);
	void ExecuteParadropPillage(CvPlot* pTargetPlot);
	void ExecuteLandingOperation(CvPlot* pTargetPlot);
#ifdef MOD_CORE_NEW_DEPLOYMENT_LOGIC
	bool ExecuteSpotterMove(CvPlot* pTargetPlot);
	bool ExecuteAttackWithUnits(CvPlot* pTargetPlot, eAggressionLevel eAggLvl);
	bool PositionUnitsAroundTarget(CvPlot* pTargetPlot);
#endif
	void ExecuteAirSweep(CvPlot* pTargetPlot);
	void ExecuteAirAttack(CvPlot* pTargetPlot);
	CvPlot* FindAirTargetNearTarget(CvUnit* pUnit, CvPlot* pTargetPlot);
	void ExecuteRepositionMoves();
	void ExecuteMovesToSafestPlot();
	void ExecuteHeals();
	void ExecuteBarbarianMoves(bool bAggressive);
	void ExecuteBarbarianCivilianEscortMove();
	bool ExecuteMoveToPlot(CvUnit* pUnit, CvPlot* pTarget, bool bSaveMoves = false, int iFlags = 0);
	bool ExecuteMoveOfBlockingUnit(CvUnit* pUnit, CvPlot* pPreferredDirection=NULL);
	void ExecuteNavalBlockadeMove(CvPlot* pTarget);
	void ExecuteAirInterceptMoves();
	void ExecuteAirSweepMoves();
	bool ExecuteSafeBombards(CvTacticalTarget& kTarget);
	bool ExecuteFlankAttack(CvTacticalTarget& kTarget);
	void ExecuteCloseOnTarget(CvTacticalTarget& kTarget, CvTacticalDominanceZone* pZone);
	void ExecuteWithdrawMoves();
	void ExecuteEscortEmbarkedMoves();

	// Internal low-level utility routines
#if defined(MOD_AI_SMART_RANGED_UNITS)
	CvPlot* GetBestRepositionPlot(CvUnit* unitH, CvPlot* plotTarget, int iAcceptableDanger);
#endif
	CvUnit* FindUnitForThisMove(TacticalAIMoveTypes eMove, CvPlot* pTargetPlot, int iNumTurnsAway=0);
	bool FindUnitsWithinStrikingDistance(CvPlot *pTargetPlot, bool bNoRangedUnits=false, bool bImmediateStrike=true);
	bool FindUnitsForHarassing(CvPlot* pTarget, int iNumTurnsAway, int iMinHitpoints, int iMaxHitpoints, DomainTypes eDomain, bool bMustHaveMovesLeft);
	bool FindParatroopersWithinStrikingDistance(CvPlot *pTargetPlot, bool bCheckDanger);
	bool FindEmbarkedUnitsAroundTarget(CvPlot *pTargetPlot, int iMaxDistance);
	bool FindCitiesWithinStrikingDistance(CvPlot* pTargetPlot);

	int GetRecruitRange() const;

#if defined(MOD_AI_SMART_AIR_TACTICS)
	void FindAirUnitsToAirSweep(CvPlot* pTarget);
#endif

	int ComputeTotalExpectedDamage(CvTacticalTarget* target, CvPlot* pTargetPlot);
	int ComputeTotalExpectedCityBombardDamage(CvUnit* pTarget);
	bool IsExpectedToDamageWithRangedAttack(CvUnit* pAttacker, CvPlot* pTarget, int iMinDamage=0);

	bool MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTargetPlot, DomainTypes eDomain, int iMaxTurns, bool bMustBeSafePath = false);

	CvPlot* FindBestBarbarianLandMove(CvUnit* pUnit);
	CvPlot* FindPassiveBarbarianLandMove(CvUnit* pUnit);
	CvPlot* FindBestBarbarianSeaMove(CvUnit* pUnit);
	CvPlot* FindBarbarianExploreTarget(CvUnit* pUnit);
	CvPlot* FindBarbarianGankTradeRouteTarget(CvUnit* pUnit);
	CvPlot* FindNearbyTarget(CvUnit* pUnit, int iRange, AITacticalTargetType eType = AI_TACTICAL_TARGET_NONE, bool bAllowDefensiveTargets=false);
	bool NearVisibleEnemy(CvUnit* pUnit, int iRange);
	bool UseThisDominanceZone(CvTacticalDominanceZone* pZone);
	bool IsVeryHighPriorityCivilianTarget(CvTacticalTarget* pTarget);
	bool IsHighPriorityCivilianTarget(CvTacticalTarget* pTarget);
	bool IsMediumPriorityCivilianTarget(CvTacticalTarget* pTarget);

	// Logging functions
	CvString GetLogFileName(CvString& playerName) const;

	// Class data - some of it only temporary, does not need to be persisted
	CvPlayer* m_pPlayer;

	CvTacticalAnalysisMap m_tacticalMap;
	std::list<int> m_CurrentTurnUnits;

#if defined(MOD_AI_SMART_AIR_TACTICS)
	std::vector<CvTacticalUnit> m_CurrentAirSweepUnits;
#endif

	std::set<int> m_HealingUnits; //persistent!

	CTacticalUnitArray m_CurrentMoveUnits;
	std::vector<CvTacticalCity> m_CurrentMoveCities;
	std::vector<CvTacticalMove> m_MovePriorityList;

	// Lists of targets for the turn
	TacticalList m_AllTargets;
	TacticalList m_ZoneTargets;
	TacticalList m_NavalTargets;

	std::vector<CvTacticalPosture> m_Postures; //persistent!

	// Targeting ranges (pulled in from GlobalAIDefines.XML)
	int m_iRecruitRange;
	int m_iLandBarbarianRange;
	int m_iSeaBarbarianRange;
	int m_iRepositionRange;
	int m_iDeployRadius;
	int m_iRandomRange;
	double m_fFlavorDampening;

	// Dominance zone info
	int m_iCurrentZoneID;
	int m_eCurrentTargetType;
	int m_iCurrentTargetIndex;
	int m_iCurrentUnitTargetIndex;

	int m_iCurrentTempZoneIndex;
	std::vector<CvTemporaryZone> m_TempZones;
	std::vector<CvTacticalTarget> m_TempTargets;

	// Blocking (and flanking) position data
	std::vector<CvBlockingUnit> m_PotentialBlocks;
	std::vector<CvBlockingUnit> m_TemporaryBlocks;
	std::vector<CvBlockingUnit> m_ChosenBlocks;
	std::vector<CvBlockingUnit> m_NewlyChosen;

	// Operational AI support data
	int m_CachedInfoTypes[eNUM_TACTICAL_INFOTYPES];
};

#if defined(MOD_CORE_NEW_DEPLOYMENT_LOGIC)
enum eUnitMovementStrategy { MS_NONE,MS_FIRSTLINE,MS_SECONDLINE,MS_THIRDLINE,MS_SUPPORT,MS_ESCORTED_EMBARKED }; //we should probably differentiate between regular ranged and siege ranged ...
enum eUnitAssignmentType { A_INITIAL, A_MOVE, A_MELEEATTACK, A_MELEEKILL, A_RANGEATTACK, A_RANGEKILL, A_FINISH, 
							A_BLOCKED, A_PILLAGE, A_CAPTURE, A_MOVE_FORCED, A_RESTART, A_MELEEKILL_NO_ADVANCE, A_MOVE_SWAP, A_MOVE_SWAP_REVERSE };

struct STacticalAssignment
{
	eUnitAssignmentType eAssignmentType;
	int iUnitID;
	int iScore;
	int iFromPlotIndex;
	int iToPlotIndex;
	int iRemainingMoves;
	eUnitMovementStrategy eMoveType;

	int iDamage; //just in case of attack, not set in constructor
	int iSelfDamage; //only relevant for melee ...

	//convenience constructor
	explicit STacticalAssignment(int iFromPlot = 0, int iToPlot = 0, int iUnit = 0, int iRemainingMoves_= 0, eUnitMovementStrategy eMoveType_ = MS_NONE, int iScore_ = 0, eUnitAssignmentType eType_ = A_FINISH) :
		iFromPlotIndex(iFromPlot), iToPlotIndex(iToPlot), iUnitID(iUnit), iRemainingMoves(iRemainingMoves_), eMoveType(eMoveType_), iScore(iScore_), eAssignmentType(eType_), iDamage(0), iSelfDamage(0) {}

	//sort descending
	bool operator<(const STacticalAssignment& rhs) { return iScore>rhs.iScore; }

	//convenience
	bool isCombatUnit() const { return eMoveType == MS_FIRSTLINE || eMoveType == MS_SECONDLINE || eMoveType == MS_THIRDLINE; }
	bool isEmbarkedUnit() const { return eMoveType == MS_ESCORTED_EMBARKED; }
	bool isSupportUnit() const { return eMoveType == MS_SUPPORT; }
};

struct SUnitStats
{
	int iUnitID;
	int iPlotIndex;
	int iAttacksLeft;
	int iMovesLeft;
	int iImportanceScore;
	int iSelfDamage; //melee units take damage when attacking
	eUnitAssignmentType eLastAssignment;
	eUnitMovementStrategy eStrategy;

	//convenience constructor
	SUnitStats(const CvUnit* pUnit, int iImportance, eUnitMovementStrategy eStrategy_) :
		iUnitID(pUnit->GetID()), iPlotIndex(pUnit->plot()->GetPlotIndex()), iAttacksLeft(pUnit->getNumAttacks() - pUnit->getNumAttacksMadeThisTurn()), 
		iMovesLeft(pUnit->getMoves()), iImportanceScore(iImportance), iSelfDamage(0), eLastAssignment(A_INITIAL), eStrategy(eStrategy_) {}
	SUnitStats(int iUnit, int iPlot, int iAttacks, int iMoves, int iImportance, eUnitMovementStrategy eStrategy_) : 
		iUnitID(iUnit), iPlotIndex(iPlot), iAttacksLeft(iAttacks), iMovesLeft(iMoves), iImportanceScore(iImportance), iSelfDamage(0),
		eLastAssignment(A_INITIAL), eStrategy(eStrategy_) {}

	bool operator<(const SUnitStats& rhs) { return iImportanceScore > rhs.iImportanceScore; } //sort descending by default
};

//forward
class CvTacticalPosition;
class CvTactPosStorage;

class CvTacticalPlot
{
public:
	enum eTactPlotType { TP_FARAWAY, TP_ENEMY, TP_FRONTLINE, TP_SECONDLINE, TP_THIRDLINE, TP_BLOCKED_FRIENDLY, TP_BLOCKED_NEUTRAL };
	enum eTactPlotDomain { TD_LAND, TD_SEA, TD_BOTH };

	CvTacticalPlot(const CvPlot* plot=NULL, PlayerTypes ePlayer=NO_PLAYER, const set<CvUnit*>& allOurUnits=set<CvUnit*>());

	int getPlotIndex() const { return pPlot ? pPlot->GetPlotIndex() : -1; }
	bool isChokepoint() const { return pPlot ? pPlot->IsChokePoint() : false; }
	int getNumAdjacentEnemies() const { return nEnemyCombatUnitsAdjacent; }
	int getNumAdjacentFriendlies() const { return nFriendlyCombatUnitsAdjacent; }
	int getNumAdjacentFirstlineFriendlies() const { return nFriendlyFirstlineUnitsAdjacent; }
	const vector<STacticalAssignment>& getUnitsAtPlot() const { return vUnits; }

	bool isEnemy() const { return bBlockedByEnemyCity || bBlockedByEnemyCombatUnit; }
	bool isEnemyCity() const { return bBlockedByEnemyCity; }
	bool isEnemyCivilian() const { return bEnemyCivilianPresent; }
	bool isEnemyCombatUnit() const { return bBlockedByEnemyCombatUnit; }
	bool isEdgePlot() const { return bEdgeOfTheKnownWorld; }
	bool isNextToCitadel() const { return bAdjacentToEnemyCitadel; }
	bool hasAirCover() const { return bHasAirCover; }
	bool isOtherEmbarkedUnit() const { return bIsOtherEmbarkedUnit; }

	bool hasFriendlyCombatUnit() const;
	bool hasFriendlyEmbarkedUnit() const;
	bool hasSupportBonus() const;

	void setDamage(int iDamage) { iDamageDealt = iDamage; }
	int getDamage() const { return iDamageDealt; }

	void setInitialState(const CvPlot* plot, PlayerTypes ePlayer, const set<CvUnit*>& allOurUnits); //set initial state depending on current plot status
	//update fictional state
	void friendlyUnitMovingIn(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment);
	void friendlyUnitMovingOut(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment);
	void enemyUnitKilled();

	eTactPlotType getType(eTactPlotDomain eDomain = TD_BOTH) const { return eType[eDomain]; }
	void setType(eTactPlotDomain eDomain, eTactPlotType newType) { eType[eDomain] = newType; }
	void findType(eTactPlotDomain eDomain, const CvTacticalPosition& currentPosition, set<int>& outstandingUpdates);
	bool isValid() const { return pPlot != NULL; }
	void changeNeighboringUnitCount(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment, int iChange);
	bool isRelevant() const { return eType[TD_BOTH] != TP_BLOCKED_FRIENDLY && eType[TD_BOTH] != TP_BLOCKED_NEUTRAL; }

protected:
	const CvPlot* pPlot;
	vector<STacticalAssignment> vUnits; //which (simulated) units are in this plot?
	unsigned char nEnemyCombatUnitsAdjacent; //for figuring out the frontline
	unsigned char nFriendlyCombatUnitsAdjacent; //for flanking
	unsigned char nFriendlyFirstlineUnitsAdjacent; //ranged units need cover
	unsigned char nSupportUnitsAdjacent; //for general bonus

	//note that blocked by neutral cannot occur, we don't even create tactical plots in that case!
	bool bBlockedByEnemyCity:1;
	bool bBlockedByEnemyCombatUnit:1;
	bool bEnemyCivilianPresent:1;
	bool bEdgeOfTheKnownWorld:1; //neighboring plot is invisible
	bool bAdjacentToEnemyCitadel:1;
	bool bHasAirCover:1;
	bool bIsOtherEmbarkedUnit:1; //can we put an embarked unit there?

	eTactPlotType eType[3]; //land, sea, both
	unsigned char iDamageDealt;
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

	vector<SUnitStats> availableUnits; //which units do still need an assignment
	vector<CvTacticalPlot> tactPlots; //storage for tactical plots (complete, mostly redundant with parent)
	map<int, int> tacticalPlotLookup; //tactical plots don't store adjacency info, so we need to take a detour via CvPlot
	map<int,ReachablePlots> reachablePlotLookup; //reachable plots, only for those units where it's different from parent
	map<int,set<int>> rangeAttackPlotLookup; //plots for a potential ranged attack, only for those units where it's different from parent
	PlotIndexContainer freedPlots; //plot indices for killed enemy units, to be ignored for ZOC
	UnitIdContainer killedEnemies; //enemy units which were killed, to be ignored for danger
	int movePlotUpdateFlag; //zero for nothing to do, unit id for a specific unit, -1 for all units

	//set in constructor, constant afterwards
	PlayerTypes ePlayer;
	eAggressionLevel eAggression;
	unsigned char nOurUnits;
	unsigned char nTheirUnits; //also counts enemy cities ...
	CvPlot* pTargetPlot;
	bool isIsolatedTarget;

	//just for debugging, should be unique
	unsigned long long iID;

	//dummy to avoid returning temporaries
	CvTacticalPlot dummyPlot;

	//------------
	const ReachablePlots& getReachablePlotsForUnit(int iUnit) const;
	const set<int>& getRangeAttackPlotsForUnit(int iUnit) const;
	vector<STacticalAssignment> getPreferredAssignmentsForUnit(SUnitStats unit, int nMaxCount) const;
	CvTacticalPosition* addChild(CvTactPosStorage& storage);
	bool removeChild(CvTacticalPosition* pChild);
	bool addAssignment(STacticalAssignment newAssignment);
	bool isMoveBlockedByOtherUnit(const STacticalAssignment& move) const;
	void getPlotsWithChangedVisibility(const STacticalAssignment& assignment, vector<int>& madeVisible) const;
	void updateMoveAndAttackPlotsForUnit(SUnitStats unit);

	//finding a particular unit
	struct PrMatchingUnit
	{
		int iUnitID;
		PrMatchingUnit(int iID) : iUnitID(iID) {}
		bool operator()(const SUnitStats& other) { return iUnitID == other.iUnitID; }
	};

public:
	CvTacticalPosition();

	void initFromScratch(PlayerTypes player, eAggressionLevel eAggLvl, CvPlot* pTarget);
	void initFromParent(const CvTacticalPosition& parent); 

	bool isComplete() const;
	bool isOffensive() const;
	void updateTacticalPlotTypes(CvTacticalPlot::eTactPlotDomain eDomain, int iStartPlot = -1);
	void updateTacticalPlotTypes(int iStartPlot = -1);
	void dropSuperfluousUnits(int iMaxUnitsToKeep);
	void addInitialAssignments();
	bool makeNextAssignments(int iMaxBranches, int iMaxChoicesPerUnit, CvTactPosStorage& storage);
	void updateMovePlotsIfRequired();
	bool haveTacticalPlot(const CvPlot* pPlot) const;
	void addTacticalPlot(const CvPlot* pPlot, const set<CvUnit*>& allOurUnits);
	bool addAvailableUnit(const CvUnit* pUnit);
	int countChildren() const;
	float getAggressionBias() const;
	void countPlotTypes();
	vector<STacticalAssignment> findBlockingUnitsAtPlot(int iPlotIndex, const STacticalAssignment& move) const;
	bool unitHasAssignmentOfType(int iUnit, eUnitAssignmentType assignmentType) const;
	bool isEquivalent(const CvTacticalPosition& rhs) const;

	const CvTacticalPlot& getTactPlot(int plotindex) const;
	CvTacticalPlot& getTactPlot(int plotindex);

	CvPlot* getTarget() const { return pTargetPlot; }
	eAggressionLevel getAggressionLevel() const { return eAggression; }
	PlayerTypes getPlayer() const { return ePlayer; }
	int getScore() const { return iTotalScore; }
	int getHeapScore() const { return iScoreOverParent + iTotalScore/12; } //use total score mainly as tiebreaker

	const CvTacticalPosition* getParent() const { return parentPosition; }
	const vector<CvTacticalPosition*>& getChildren() const { return childPositions; }
	vector<STacticalAssignment> getAssignments() const { return assignedMoves; }
	const UnitIdContainer& getKilledEnemies() const { return killedEnemies; }
	const int getNumEnemies() const { return nTheirUnits - killedEnemies.size(); }

	//sort descending cumulative score. only makes sense for "completed" positions
	bool operator<(const CvTacticalPosition& rhs) { return iTotalScore>rhs.iTotalScore; }

	//debugging
	unsigned long long getID() const { return iID; }
	void dumpPlotStatus(const char* filename) const;
	void exportToDotFile(const char* filename) const;
	void dumpChildren(ofstream& out) const;
};

class CvTactPosStorage
{
public:
	CvTactPosStorage(int iPreallocationSize) : iCount(0), iSize(iPreallocationSize), aPositions(new CvTacticalPosition[iPreallocationSize]) {}
	~CvTactPosStorage() { delete[] aPositions; }
	void reset() { iCount = 0; }
	CvTacticalPosition* getNext()
	{
		if (iCount < iSize) 
		{ 
			return aPositions + iCount++; 
		}
		else 
			return NULL;
	}
protected:
	int iSize; //how many do we have
	int iCount; //how many are currently in use
	CvTacticalPosition* aPositions; //preallocated block of N positions
};

#endif

namespace TacticalAIHelpers
{
	bool SortBlockingUnitByDistanceAscending(const CvBlockingUnit& obj1, const CvBlockingUnit& obj2);
	bool SortByExpectedTargetDamageDescending(const CvTacticalUnit& obj1, const CvTacticalUnit& obj2);

	ReachablePlots GetAllPlotsInReachThisTurn(const CvUnit* pUnit, const CvPlot* pStartPlot, int iFlags, int iMinMovesLeft=0, int iStartMoves=-1, const PlotIndexContainer& plotsToIgnoreForZOC=PlotIndexContainer());
	std::set<int> GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, const CvPlot* pBasePlot, bool bOnlyWithEnemy, bool bIgnoreVisibility);
	std::set<int> GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, ReachablePlots& basePlots, bool bOnlyWithEnemy,  bool bIgnoreVisibility);

	bool PerformRangedOpportunityAttack(CvUnit* pUnit, bool bAllowMovement = false);
	bool PerformOpportunityAttack(CvUnit* pUnit, bool bAllowMovement = false);
	bool IsAttackNetPositive(CvUnit* pUnit, const CvPlot* pTarget);
	bool CountDeploymentPlots(TeamTypes eTeam, const CvPlot* pTarget, int iNumUnits, int iDeployRange);
	CvPlot* FindSafestPlotInReach(const CvUnit* pUnit, bool bAllowEmbark, bool bLowDangerOnly=false, bool bConsiderSwap=false);
	CvPlot* FindClosestSafePlotForHealing(CvUnit* pUnit);
	bool GetPlotsForRangedAttack(const CvPlot* pTarget, const CvUnit* pUnit, int iRange, bool bCheckOccupied, std::vector<CvPlot*>& vPlots);
	int GetSimulatedDamageFromAttackOnUnit(const CvUnit* pDefender, const CvUnit* pAttacker, CvPlot* pDefenderPlot, CvPlot* pAttackerPlot, int& iAttackerDamage, 
									bool bIgnoreUnitAdjacencyBoni=false, int iExtraDefenderDamage=0, bool bQuickAndDirty = false);
	int GetSimulatedDamageFromAttackOnCity(CvCity* pCity, const CvUnit* pAttacker, CvPlot* pAttackerPlot, int& iAttackerDamage, 
									bool bIgnoreUnitAdjacencyBoni=false, int iExtraDefenderDamage=0, bool bQuickAndDirty = false);
	bool KillUnitIfPossible(CvUnit* pAttacker, CvUnit* pDefender);
	CvPlot* GetFirstTargetInRange(CvUnit* pUnit, bool bMustBeAbleToKill=false, bool bIncludeCivilians=true);
	pair<int, int> EstimateLocalUnitPower(CvPlot* pOrigin, int iRangeInTurns, TeamTypes eTeamA, TeamTypes eTeamB, bool bMustBeVisibleToBoth);
	int CountAdditionallyVisiblePlots(CvUnit* pUnit, CvPlot* pTestPlot);

#if defined(MOD_CORE_NEW_DEPLOYMENT_LOGIC)
	vector<STacticalAssignment> FindBestOffensiveAssignment(const vector<CvUnit*>& vUnits, CvPlot* pTarget, eAggressionLevel eAggLvl, CvTactPosStorage& storage);
	vector<STacticalAssignment> FindBestDefensiveAssignment(const vector<CvUnit*>& vUnits, CvPlot* pTarget, CvTactPosStorage& storage);
	bool ExecuteUnitAssignments(PlayerTypes ePlayer, const vector<STacticalAssignment>& vAssignments);
#endif

}

extern const char* barbarianMoveNames[];
extern const char* postureNames[];
extern const char* assignmentTypeNames[];

#endif //CIV5_TACTICAL_AI_H