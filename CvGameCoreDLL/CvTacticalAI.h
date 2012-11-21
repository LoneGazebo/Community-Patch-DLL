/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TACTICAL_AI_H
#define CIV5_TACTICAL_AI_H

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
	int GetPriority() const {return m_iPriority;};
	void SetPriority(int iPriority) {m_iPriority = iPriority;};
	int GetOffenseFlavorWeight() const {return m_iOffenseWeight;};
	void SetOffenseFlavorWeight(int iWeight) {m_iOffenseWeight = iWeight;};
	int GetDefenseFlavorWeight() const {return m_iDefenseWeight;};
	void SetDefenseFlavorWeight(int iWeight) {m_iDefenseWeight = iWeight;};
	int CanRecruitForOperations() const {return m_bOperationsCanRecruit;};
	void SetRecruitForOperations(bool bAvailable) {m_bOperationsCanRecruit = bAvailable;};
	int IsDominanceZoneMove() const {return m_bDominanceZoneMove;};
	void SetDominanceZoneMove(bool bByPosture) {m_bDominanceZoneMove = bByPosture;};

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
	CvTacticalMoveXMLEntry *GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvTacticalMoveXMLEntry*> m_paTacticalMoveEntries;
};

enum AITacticalMission
{
	AI_TACTICAL_MISSION_NONE = -1,
	AI_TACTICAL_MISSION_ATTACK_STATIONARY_TARGET,
    AI_TACTICAL_MISSION_PILLAGE_ENEMY_IMPROVEMENTS,
	// Phasing this out so other types deleted (as unused)
};
FDataStream & operator<<(FDataStream &, const AITacticalMission &);
FDataStream & operator>>(FDataStream &, AITacticalMission &);

// STL "find_if" predicate
class UnitIDMatch
{
public:
	UnitIDMatch (int iUnitID)
		: m_iMatchingUnitID(iUnitID)
	{}

	bool operator() (int element) const
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
	}

	bool operator<(const CvTacticalMove& move) const
	{
		return (m_iPriority > move.m_iPriority);
	}

	TacticalAIMoveTypes m_eMoveType;
	int m_iPriority;
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
	void SetID(int iID) {m_iID = iID;};
	int GetID() const {return m_iID;};
	void SetHealthPercent(int curHitPoints, int maxHitPoints)
	{
		CvAssert (maxHitPoints != 0);
		m_iHealthPercent = curHitPoints * 100 / maxHitPoints;
	}
	int GetHealthPercent() const {return m_iHealthPercent;};
	void SetAttackStrength(int iAttackStrength) {m_iAttackStrength = iAttackStrength;};
	int GetAttackStrength() const {return m_iAttackStrength;};
	void SetExpectedTargetDamage(int iExpectedDamage) {m_iExpectedTargetDamage = iExpectedDamage;};
	int GetExpectedTargetDamage() const {return m_iExpectedTargetDamage;};
	void SetExpectedSelfDamage(int iExpectedDamage) {m_iExpectedSelfDamage = iExpectedDamage;};
	int GetExpectedSelfDamage() const {return m_iExpectedSelfDamage;};
	void SetMovesToTarget(int iMoves) {m_iMovesToTarget = iMoves;};
	int GetMovesToTarget() const {return m_iMovesToTarget;};

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
	void SetID(int iID) {m_iID = iID;};
	int GetID() const {return m_iID;};
	void SetExpectedTargetDamage(int iExpectedDamage) {m_iExpectedTargetDamage = iExpectedDamage;};
	int GetExpectedTargetDamage() const {return m_iExpectedTargetDamage;};

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
		m_iTargetX = -1;
		m_iTargetY = -1;
		m_eTargetPlayer = NO_PLAYER;
		m_iDominanceZoneID = -1;
		m_pAuxData = NULL;
		m_iAuxData = 0;
	};
	inline bool operator<(const CvTacticalTarget& target) const
	{
		return (m_iAuxData > target.m_iAuxData);
	};
	inline AITacticalTargetType GetTargetType() {return m_eTargetType;}
	inline void SetTargetType(AITacticalTargetType eTargetType) {m_eTargetType = eTargetType;}
	inline int GetTargetX() {return m_iTargetX;}
	inline void SetTargetX(int iValue) {m_iTargetX = iValue;}
	inline int GetTargetY() {return m_iTargetY;}
	inline void SetTargetY(int iValue) {m_iTargetY = iValue;}
	inline PlayerTypes GetTargetPlayer() {return m_eTargetPlayer;}
	inline void SetTargetPlayer(PlayerTypes eValue) {m_eTargetPlayer = eValue;}
	inline int GetDominanceZone() const {return m_iDominanceZoneID;};
	inline void SetDominanceZone(int iZone) {m_iDominanceZoneID = iZone;};
	bool IsTargetStillAlive(PlayerTypes m_eAttackingPlayer);
    bool IsTargetValidInThisDomain(DomainTypes eDomain);

	// AuxData is used for a pointer to the actual target object (CvUnit, CvCity, etc.)
	//    (for improvements & barbarian camps this is set to the plot).
	inline void *GetAuxData() {return m_pAuxData;}
	inline void SetAuxData(void *pAuxData) {m_pAuxData = pAuxData;}

	// Stores required damage for offensive targets
	// For defensive items used to SORT targets in priority order
	//    Set to the weight for defensive bastions
	//    Set to the danger for cities to be garrisoned
	inline int GetAuxIntData() {return m_iAuxData;}
	inline void SetAuxIntData(int iAuxData) {m_iAuxData = iAuxData;}

private:
	AITacticalTargetType m_eTargetType;
	int m_iTargetX;
	int m_iTargetY;
	PlayerTypes m_eTargetPlayer;
	void *m_pAuxData;
	int m_iAuxData;
	int m_iDominanceZoneID;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvQueuedAttack
//!  \brief		A planned attack waiting to execute
//
//!  Key Attributes:
//!  - Arises during processing of CvTacticalAI::ExecuteAttacks() or ProcessUnit()
//!  - Created by calling QueueFirstAttack() or QueueSubsequentAttack()
//!  - Combat animation system calls back into tactical AI when animation completes with call CombatResolved()
//!  - This callback signals it is time to execute the next attack
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvQueuedAttack
{
public:
	CvQueuedAttack()
	{
	  m_pAttacker = NULL;
	  m_iSeriesID = 0;
	  m_bRanged = false;
	  m_bCityAttack = false;
	};
	CvQueuedAttack(const CvQueuedAttack& rhs) : m_pAttacker(rhs.m_pAttacker), m_kTarget(rhs.m_kTarget), m_iSeriesID(rhs.m_iSeriesID), m_bRanged(rhs.m_bRanged), m_bCityAttack(rhs.m_bCityAttack) {}
	void *GetAttacker() {return m_pAttacker;}
	void SetAttacker(void *pAttacker) {m_pAttacker = pAttacker;}
	CvTacticalTarget *GetTarget() {return &m_kTarget;}
	void SetTarget(CvTacticalTarget *pTarget) {m_kTarget = *pTarget;}
	int GetSeriesID() {return m_iSeriesID;}
	void SetSeriesID(int id) {m_iSeriesID = id;}
	bool IsRanged() {return m_bRanged;}
	void SetRanged(bool bRanged) {m_bRanged = bRanged;}
	bool IsCityAttack() {return m_bCityAttack;}
	void SetCityAttack(bool bCityAttack) {m_bCityAttack = bCityAttack;}

private:
	void *m_pAttacker;
	CvTacticalTarget m_kTarget;  // Needs to be a copy since original CvTacticalTarget is gone before a follow-on attack is made
	int m_iSeriesID;
	bool m_bRanged;
	bool m_bCityAttack;
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
	AI_TACTICAL_POSTURE_SIT_AND_BOMBARD,
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

	PlayerTypes GetPlayer() const {return (PlayerTypes)(m_iPlayerWaterInfo / 2);};
	bool IsWater() {return m_iPlayerWaterInfo & 0x1;};
	int GetCityID() {return m_iCityID;};
	AITacticalPosture GetPosture() {return m_ePosture;};

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
		m_eTargetType = AI_TACTICAL_TARGET_NONE;
		m_iLastTurn = 0;
	};

	int GetX() const {return m_iX;};
	void SetX(int iX) {m_iX = iX;};
	int GetY() const {return m_iY;};
	void SetY(int iY) {m_iY = iY;};
	AITacticalTargetType GetTargetType() const {return m_eTargetType;};
	void SetTargetType(AITacticalTargetType eType) {m_eTargetType = eType;};
	int GetLastTurn() const {return m_iLastTurn;};
	void SetLastTurn(int iTurn) {m_iLastTurn = iTurn;};
	bool IsNavalInvasion() const {return m_bIsNavalInvasion;};
	void SetNavalInvasion(bool bIsNavalInvasion) {m_bIsNavalInvasion = bIsNavalInvasion;};

private:
	int m_iX;
	int m_iY;
	AITacticalTargetType m_eTargetType;
	int m_iLastTurn;
	bool m_bIsNavalInvasion;
};

FDataStream & operator<<(FDataStream &, const CvTemporaryZone &);
FDataStream & operator>>(FDataStream &, CvTemporaryZone &);

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
	CvPlot *GetPlot() const {return m_pPlot;};
	void SetPlot(CvPlot *pPlot) {m_pPlot = pPlot;};
	int GetUnitID() const {return m_iUnitID;};
	void SetUnitID(int iID) {m_iUnitID = iID;};
	int GetNumChoices() const {return m_iNumChoices;};
	void SetNumChoices(int iChoices) {m_iNumChoices = iChoices;};
	int GetDistanceToTarget() const {return m_iDistanceToTarget;};
	void SetDistanceToTarget(int iDistanceToTarget) {m_iDistanceToTarget = iDistanceToTarget;};

private:
	CvPlot *m_pPlot;
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
	int GetUnitID() const {return m_iUnitID;};
	void SetUnitID(int iID) {m_iUnitID = iID;};
	int GetPosition() const {return m_ePosition;};
	void SetPosition(MultiunitPositionTypes eType) {m_ePosition = eType;};

private:
	int m_iUnitID;
	MultiunitPositionTypes m_ePosition;
};

typedef FStaticVector<CvTacticalTarget, 100, false, c_eCiv5GameplayDLL > TacticalList;

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
	void Init(CvPlayer *pPlayer);
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
	CvTemporaryZone *GetFirstTemporaryZone();
	CvTemporaryZone *GetNextTemporaryZone();
	void AddTemporaryZone(CvTemporaryZone zone);
	void DeleteTemporaryZone(CvPlot *pPlot);
	void DropObsoleteZones();
	bool IsTemporaryZoneCity(CvCity *pCity);

	// Public routines to handle multiple unit attacks
	void InitializeQueuedAttacks();
	bool QueueAttack (void *pAttacker, CvTacticalTarget *pTarget, bool bRanged, bool bCityAttack);
	void LaunchAttack (void *pAttacker, CvTacticalTarget *pTarget, bool bFirstAttack, bool bRanged, bool bCityAttack);
	void CombatResolved (void *pAttacker, bool bVictorious, bool bCityAttack=false);
	int PlotAlreadyTargeted (CvPlot *pPlot);
	bool IsInQueuedAttack (const CvUnit *pAttacker);
	bool IsCityInQueuedAttack (const CvCity *pAttackCity);
	int NearXQueuedAttacks (const CvPlot *pPlot, const int iRange);

	// Public logging
	void LogTacticalMessage(CvString& strMsg, bool bSkipLogDominanceZone = true);

private:

	// Internal turn update routines - commandeered unit processing
	void UpdatePostures();
	AITacticalPosture SelectPosture(CvTacticalDominanceZone *pZone, AITacticalPosture eLastPosture);
	AITacticalPosture FindPosture(CvTacticalDominanceZone *pZone);
	void EstablishTacticalPriorities();
	void EstablishBarbarianPriorities();
	void FindTacticalTargets();
	void ProcessDominanceZones();
	void AssignTacticalMove(CvTacticalMove move);
	void AssignBarbarianMoves();

	// Routines to manage identifying and implementing tactical moves
	bool PlotCaptureCityMoves();
	bool PlotDamageCityMoves();
	void PlotBarbarianCampMoves();
	void PlotDestroyUnitMoves(AITacticalTargetType movePriorit, bool bMustBeAbleToKill, bool bAttackAtPoorOdds=false);
	void PlotMovesToSafety(bool bCombatUnits);
	void PlotRepositionMoves();
	void PlotOperationalArmyMoves();
	void PlotPillageMoves();
	void PlotBlockadeImprovementMoves();
	void PlotCivilianAttackMoves(AITacticalTargetType eTargetType);
	void PlotSafeBombardMoves();
	void PlotHealMoves();
	void PlotCampDefenseMoves();
	void PlotBarbarianMove(bool bAggressive);
	void PlotBarbarianCivilianEscortMove();
	void PlotGarrisonMoves(int iTurnsToArrive, bool bMustAllowRangedAttack=false);
	void PlotBastionMoves(int iTurnsToArrive);
	void PlotGuardImprovementMoves(int iTurnsToArrive);
	void PlotAncientRuinMoves(int iNumTurnsAway);
	void PlotAirInterceptMoves();
	void PlotSitAndBombardMoves();
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
	void PlotEmbarkedUnitRunoverMoves(AITacticalTargetType eTargetType);
	void ReviewUnassignedUnits();

	// Operational AI support functions
	void PlotSingleHexOperationMoves(CvAIEscortedOperation *pOperation);
	void PlotEnemyTerritoryOperationMoves(CvAIEnemyTerritoryOperation *pOperation);
	void PlotNavalEscortOperationMoves(CvAINavalEscortedOperation *pOperation);
	void PlotFreeformNavalOperationMoves(CvAINavalOperation *pOperation);
	void ClearEnemiesNearArmy(CvArmyAI *pArmy);
	void MoveWithFormation(UnitHandle pUnit, MultiunitPositionTypes ePosition);
	void ExecuteFormationMoves(CvArmyAI *pArmy);
	bool ScoreDeploymentPlots(CvPlot *pTarget, CvArmyAI *pArmy, int iNumMeleeUnits, int iNumRangedUnits, int iDeployRange);
	void ExecuteNavalFormationMoves(CvArmyAI *pArmy, CvPlot *pTurnTarget);
	bool PlotEscortNavalOperationMoves(CvArmyAI *pArmy);
	void PlotCloseOnEnemyNavalUnitMoves(CvArmyAI *pArmy);
	void ExecuteFleetMoveToTarget(CvArmyAI *pArmy, CvPlot *pTarget);

	// Routines to process and sort targets
	void IdentifyPriorityTargets();
	void IdentifyPriorityBarbarianTargets();
	void IdentifyPriorityTargetsByType();
	void EliminateNearbyBlockadePoints();
	void ExtractTargetsForZone(CvTacticalDominanceZone *pZone /* Pass in NULL for all zones */);
	CvTacticalTarget *GetFirstZoneTarget(AITacticalTargetType eType);
	CvTacticalTarget *GetNextZoneTarget();
	CvTacticalTarget *GetFirstUnitTarget();
	CvTacticalTarget *GetNextUnitTarget();

	// Routines to execute a mission
	void ExecuteBarbarianCampMove(CvPlot *pTargetPlot);
	void ExecuteCivilianCapture(CvPlot *pTargetPlot);
	void ExecutePillage(CvPlot *pTargetPlot);
	void ExecuteAttack(CvTacticalTarget *target, CvPlot *pTargetPlot, bool bInflictWhatWeTake=false, bool bMustSurviveAttack=true);
	void ExecuteRepositionMoves();
	void ExecuteMovesToSafestPlot();
	void ExecuteHeals();
	void ExecuteBarbarianMoves(bool bAggressive);
	void ExecuteBarbarianCivilianEscortMove();
	void ExecuteMoveToPlot(CvPlot *pTarget, bool bSaveMoves=false);
	void ExecuteMoveToPlot(UnitHandle pUnit, CvPlot *pTarget, bool bSaveMoves = false);
	bool ExecuteMoveOfBlockingUnit(UnitHandle pUnit);
	void ExecuteNavalBlockadeMove(CvPlot *pTarget);
	void ExecuteMoveToTarget(CvPlot *pTarget, bool bGarrisonIfPossible=false);
	void ExecuteAirInterceptMoves();
	bool ExecuteSafeBombards(CvTacticalTarget &kTarget);
	bool ExecuteProtectedBombards(CvTacticalTarget &kTarget, bool bAttackUnderway);
	bool ExecuteOneProtectedBombard(CvTacticalTarget &kTarget);
	bool ExecuteFlankAttack(CvTacticalTarget &kTarget);
	void ExecuteCloseOnTarget(CvTacticalTarget &kTarget, CvTacticalDominanceZone *pZone);
	void ExecuteHedgehogDefense(CvTacticalTarget &kTarget, CvTacticalDominanceZone *pZone);
	void ExecutePriorityAttacksOnUnitTarget(CvTacticalTarget &kTarget);
	void ExecuteWithdrawMoves();

	// Internal low-level utility routines
	void TurnOffMove(TacticalAIMoveTypes eType);
	bool FindUnitsForThisMove(TacticalAIMoveTypes eMove, CvPlot *pTargetPlot, int iNumTurnsAway=0, bool bRangedOnly=false);
	bool FindUnitsWithinStrikingDistance(CvPlot *pTargetPlot, int iNumTurnsAway, bool bNoRangedUnits=false, bool bNavalOnly=false, bool bMustMoveThrough=false);
	bool FindCitiesWithinStrikingDistance(CvPlot *pTargetPlot);
	bool FindClosestUnit(CvPlot *pTargetPlot, int iNumTurnsAway, bool bMustBeRangedUnit=false, int iRangeRequired=2, bool bNeedsIgnoreLOS=false, bool bMustBeMeleeUnit=false, CvPlot *pRangedAttackTarget=NULL);
	bool FindClosestOperationUnit(CvPlot *pTargetPlot, bool bSafeForRanged, bool bIncludeGenerals);
	int ComputeTotalExpectedDamage(CvTacticalTarget *target, CvPlot *pTargetPlot);
	int ComputeTotalExpectedBombardDamage(UnitHandle pTarget);
	bool IsExpectedToDamageWithRangedAttack(UnitHandle pAttacker, CvPlot *pTarget);
	bool MoveToEmptySpaceNearTarget(UnitHandle pUnit, CvPlot *pTargetPlot, bool bLand=true);
	CvPlot *FindBestBarbarianLandMove(UnitHandle pUnit);
	CvPlot *FindPassiveBarbarianLandMove(UnitHandle pUnit);
	CvPlot *FindBestBarbarianSeaMove(UnitHandle pUnit);
	CvPlot *FindBarbarianExploreTarget(UnitHandle pUnit);
	CvPlot *FindNearbyTarget(UnitHandle pUnit, int iRange, AITacticalTargetType eType = AI_TACTICAL_TARGET_NONE, CvUnit *pNoLikeUnit = NULL);
	bool NearVisibleEnemy(UnitHandle pUnit, int iRange);
	void UnitProcessed(int iID, bool bMarkTacticalMap=true);
	bool UseThisDominanceZone(CvTacticalDominanceZone *pZone);
	bool IsVeryHighPriorityCivilianTarget(CvTacticalTarget *pTarget);
	bool IsHighPriorityCivilianTarget(CvTacticalTarget *pTarget);
	bool IsMediumPriorityCivilianTarget(CvTacticalTarget *pTarget);

	// Blocking position functions
	bool CanCoverFromEnemy(CvPlot *pPlot, int &iNumUnitsRequiredToCover, int iAttackingUnitID);
	bool AssignCoveringUnits(int iNumUnitsRequiredToCover);
	bool AssignFlankingUnits(int iNumUnitsRequiredToFlank);
	bool AssignDeployingUnits(int iNumUnitsRequiredToDeploy);
	void PerformChosenMoves();
	void MoveGreatGeneral(CvArmyAI *pArmyAI = NULL);
	bool HaveDuplicateUnit();
	void RemoveChosenUnits(int iStartIndex = 0);
	int NumUniqueUnitsLeft();
	bool IsInChosenMoves(CvPlot *pPlot);
	bool ChooseRemainingAssignments(int iNumUnitsDesired, int iNumUnitsAcceptable);
	int ScoreAssignments(bool bCanLeaveOpenings);
	int ScoreCloseOnPlots(CvPlot *pTarget, bool bLandOnly);
	void ScoreHedgehogPlots(CvPlot *pTarget);
	int ScoreGreatGeneralPlot(UnitHandle pGeneral, CvPlot *pTarget, CvArmyAI *pArmyAI);

	// Logging functions
	CvString GetLogFileName(CvString& playerName) const;
	CvString GetTacticalMissionName(AITacticalMission eMission) const;

	// Class data
	CvPlayer *m_pPlayer;
	CvTacticalAnalysisMap *m_pMap;
	list<int> m_CurrentTurnUnits;
	std::vector<CvTacticalUnit> m_CurrentMoveUnits;
	std::vector<CvTacticalUnit> m_CurrentMoveHighPriorityUnits;
	std::vector<CvTacticalCity> m_CurrentMoveCities;
    FStaticVector<CvTacticalMove, 100, false, c_eCiv5GameplayDLL > m_MovePriorityList;
	int m_MovePriorityTurn;

	// Data for multi-unit attacks - not serialized, cleared out for each turn
	std::list<CvQueuedAttack> m_QueuedAttacks;
	int m_iCurrentSeriesID;

	// Lists of targets for the turn
	TacticalList m_AllTargets;
	TacticalList m_ZoneTargets;
	TacticalList m_NavalResourceBlockadePoints;
	FStaticVector<CvTacticalTarget, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0> m_TempTargets;

	// Targeting ranges (pulled in from GlobalAIDefines.XML)
	int m_iRecruitRange;
	int m_iLandBarbarianRange;
	int m_iSeaBarbarianRange;
	int m_iRepositionRange;
	int m_iTempZoneRadius;
	int m_iDeployRadius;
	int m_iRandomRange;
	double m_fFlavorDampening;

	// Dominance zone info
	int m_iCurrentZoneIndex;
	int m_eCurrentTargetType;
	int m_iCurrentTargetIndex;
	int m_iCurrentUnitTargetIndex;
	FStaticVector<CvTacticalPosture, SAFE_ESTIMATE_NUM_CITIES, true, c_eCiv5GameplayDLL, 0> m_Postures;
	FStaticVector<CvTacticalPosture, SAFE_ESTIMATE_NUM_CITIES, true, c_eCiv5GameplayDLL, 0> m_NewPostures;
	int m_iCurrentTempZoneIndex;
	FStaticVector<CvTemporaryZone, SAFE_ESTIMATE_NUM_TEMP_ZONES, true, c_eCiv5GameplayDLL, 0> m_TempZones;

	// Blocking (and flanking) position data
	FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0> m_PotentialBlocks;
	FStaticVector<CvBlockingUnit, SAFE_ESTIMATE_NUM_BLOCKING_UNITS, true, c_eCiv5GameplayDLL, 0> m_TemporaryBlocks;
	FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0> m_ChosenBlocks;
	FStaticVector<CvBlockingUnit, NUM_CITY_PLOTS, true, c_eCiv5GameplayDLL, 0> m_NewlyChosen;

	// Operational AI support data
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0> m_OperationUnits;
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0> m_GeneralsToMove;
};

namespace TacticalAIHelpers
{
	bool CvBlockingUnitDistanceSort (CvBlockingUnit obj1, CvBlockingUnit obj2);
}

#endif //CIV5_TACTICAL_AI_H