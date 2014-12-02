/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "FFastVector.h"
#include "CvGameCoreUtils.h"
#include "CvAStar.h"
#include "CvEconomicAI.h"
#include "CvEnumSerialization.h"
#include "CvUnitCombat.h"
#include "cvStopWatch.h"
#include "CvMilitaryAI.h"
#include "CvTypes.h"
#if defined(MOD_BALANCE_CORE_MILITARY)
#include "CvDiplomacyAI.h"
#endif

#include "LintFree.h"

CvTacticalUnit::CvTacticalUnit() :
	m_iID(0)
	, m_iAttackStrength(0)
	, m_iHealthPercent(0)
	, m_iMovesToTarget(0)
	, m_iExpectedTargetDamage(0)
	, m_iExpectedSelfDamage(0)
{
}

//=====================================
// CvTacticalMoveXMLEntry
//=====================================
/// Constructor
CvTacticalMoveXMLEntry::CvTacticalMoveXMLEntry(void)
{
	m_iPriority = -1;
	m_iOffenseWeight = 0;
	m_iOffenseWeight = 0;
	m_bOperationsCanRecruit = false;
}

/// Destructor
CvTacticalMoveXMLEntry::~CvTacticalMoveXMLEntry(void)
{
}

/// Load from XML (or cache)
bool CvTacticalMoveXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	const char* szMoveType = kResults.GetText("Type");
	m_eMoveType = (TacticalAIMoveTypes)GC.getInfoTypeForString(szMoveType);

	m_iPriority = kResults.GetInt("Priority");

	m_iOffenseWeight = kResults.GetInt("OffenseFlavorWeight");
	m_iDefenseWeight = kResults.GetInt("DefenseFlavorWeight");

	m_bOperationsCanRecruit = kResults.GetInt("OperationsCanRecruit");
	m_bDominanceZoneMove = kResults.GetInt("DominanceZoneMove");

	return true;
}
//=====================================
// CvTacticalMoveXMLEntries
//=====================================
/// Constructor
CvTacticalMoveXMLEntries::CvTacticalMoveXMLEntries(void)
{

}

/// Destructor
CvTacticalMoveXMLEntries::~CvTacticalMoveXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of AIStrategy entries
std::vector<CvTacticalMoveXMLEntry*>& CvTacticalMoveXMLEntries::GetTacticalMoveEntries()
{
	return m_paTacticalMoveEntries;
}

/// Number of defined AIStrategies
int CvTacticalMoveXMLEntries::GetNumTacticalMoves()
{
	return m_paTacticalMoveEntries.size();
}

/// Get a specific entry
CvTacticalMoveXMLEntry* CvTacticalMoveXMLEntries::GetEntry(int index)
{
	return m_paTacticalMoveEntries[index];
}

/// Clear entries
void CvTacticalMoveXMLEntries::DeleteArray()
{
	for(std::vector<CvTacticalMoveXMLEntry*>::iterator it = m_paTacticalMoveEntries.begin(); it != m_paTacticalMoveEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paTacticalMoveEntries.clear();
}

//=====================================
// CvTacticalTarget
//=====================================
/// Still a living target?
bool CvTacticalTarget::IsTargetStillAlive(PlayerTypes m_eAttackingPlayer)
{
	bool bRtnValue = false;

	AITacticalTargetType eType = GetTargetType();
	if(eType == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
	        eType == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
	        eType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
	{
		CvUnit* pUnit;
		CvPlot* pPlot;
		pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		pUnit = pPlot->getVisibleEnemyDefender(m_eAttackingPlayer);
		if(pUnit != NULL && !pUnit->isDelayedDeath())
		{
			bRtnValue = true;
		}
	}
	else if(eType == AI_TACTICAL_TARGET_CITY)
	{
		CvCity* pCity;
		CvPlot* pPlot;
		pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		pCity = pPlot->getPlotCity();
		if(pCity != NULL && pCity->getOwner() == GetTargetPlayer())
		{
			bRtnValue = true;
		}
	}
	return bRtnValue;
}

/// This target make sense for this domain of unit/zone?
bool CvTacticalTarget::IsTargetValidInThisDomain(DomainTypes eDomain)
{
	bool bRtnValue = false;

	switch(GetTargetType())
	{
	case AI_TACTICAL_TARGET_DEFENSIVE_BASTION:
	case AI_TACTICAL_TARGET_BARBARIAN_CAMP:
	case AI_TACTICAL_TARGET_IMPROVEMENT:
	case AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND:
	case AI_TACTICAL_TARGET_ANCIENT_RUINS:
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND:
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT:
	case AI_TACTICAL_TARGET_CITADEL:
	case AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE:
		bRtnValue = (eDomain == DOMAIN_LAND);
		break;

	case AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT:
	case AI_TACTICAL_TARGET_BOMBARDMENT_ZONE:
	case AI_TACTICAL_TARGET_EMBARKED_CIVILIAN:
	case AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT:
	case AI_TACTICAL_TARGET_TRADE_UNIT_SEA:
	case AI_TACTICAL_TARGET_TRADE_UNIT_SEA_PLOT:
		bRtnValue = (eDomain == DOMAIN_SEA);
		break;

	case AI_TACTICAL_TARGET_CITY:
	case AI_TACTICAL_TARGET_CITY_TO_DEFEND:
	case AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
	case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
	case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
		bRtnValue = true;
		break;
	}

	return bRtnValue;
}

FDataStream& operator<<(FDataStream& saveTo, const CvTemporaryZone& readFrom)
{
	uint uiVersion = 1;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << (int)readFrom.GetTargetType();
	saveTo << readFrom.GetLastTurn();
	saveTo << readFrom.GetX();
	saveTo << readFrom.GetY();
	saveTo << (int)readFrom.IsNavalInvasion();
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvTemporaryZone& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iTemp;
	loadFrom >> iTemp;
	writeTo.SetTargetType((AITacticalTargetType)iTemp);
	loadFrom >> iTemp;
	writeTo.SetLastTurn(iTemp);
	loadFrom >> iTemp;
	writeTo.SetX(iTemp);
	loadFrom >> iTemp;
	writeTo.SetY(iTemp);
	loadFrom >> iTemp;
	writeTo.SetNavalInvasion(iTemp != 0);
	return loadFrom;
}

//=====================================
// CvTacticalAI
//=====================================

/// Constructor
CvTacticalAI::CvTacticalAI(void)
{
	m_iCurrentZoneIndex = -1;
}

/// Destructor
CvTacticalAI::~CvTacticalAI(void)
{
	Uninit();
}

/// Initialize
void CvTacticalAI::Init(CvPlayer* pPlayer)
{
	// Store off the pointer to the objects we need elsewhere in the game engine
	m_pPlayer = pPlayer;

	Reset();

	m_pMap = GC.getGame().GetTacticalAnalysisMap();

	// Initialize AI constants from XML
	m_iRecruitRange = GC.getAI_TACTICAL_RECRUIT_RANGE();
	m_iLandBarbarianRange = GC.getGame().getHandicapInfo().getBarbarianLandTargetRange();
	m_iSeaBarbarianRange = GC.getGame().getHandicapInfo().getBarbarianSeaTargetRange();
	m_iRepositionRange = GC.getAI_TACTICAL_REPOSITION_RANGE();
	m_iDeployRadius = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
	m_iRandomRange = GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS();
	m_fFlavorDampening = GC.getAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION();

	// cache TypeInfos rather than doing a hash map look up of the string every time it is being used
	m_CachedInfoTypes[eTACTICAL_UNASSIGNED] = GC.getInfoTypeForString("TACTICAL_UNASSIGNED");
	m_CachedInfoTypes[eTACTICAL_POSTURE_SIT_AND_BOMBARD] = GC.getInfoTypeForString("TACTICAL_POSTURE_SIT_AND_BOMBARD");
	m_CachedInfoTypes[eTACTICAL_POSTURE_ATTRIT_FROM_RANGE] = GC.getInfoTypeForString("TACTICAL_POSTURE_ATTRIT_FROM_RANGE");
	m_CachedInfoTypes[eTACTICAL_POSTURE_EXPLOIT_FLANKS] = GC.getInfoTypeForString("TACTICAL_POSTURE_EXPLOIT_FLANKS");
	m_CachedInfoTypes[eTACTICAL_POSTURE_STEAMROLL] = GC.getInfoTypeForString("TACTICAL_POSTURE_STEAMROLL");
	m_CachedInfoTypes[eTACTICAL_POSTURE_SURGICAL_CITY_STRIKE] = GC.getInfoTypeForString("TACTICAL_POSTURE_SURGICAL_CITY_STRIKE");
	m_CachedInfoTypes[eTACTICAL_POSTURE_COUNTERATTACK] = GC.getInfoTypeForString("TACTICAL_POSTURE_COUNTERATTACK");
	m_CachedInfoTypes[eTACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY] = GC.getInfoTypeForString("TACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY");
	m_CachedInfoTypes[eTACTICAL_CAPTURE_CITY] = GC.getInfoTypeForString("TACTICAL_CAPTURE_CITY");
	m_CachedInfoTypes[eTACTICAL_DAMAGE_CITY] = GC.getInfoTypeForString("TACTICAL_DAMAGE_CITY");
	m_CachedInfoTypes[eTACTICAL_DESTROY_HIGH_UNIT] = GC.getInfoTypeForString("TACTICAL_DESTROY_HIGH_UNIT");
	m_CachedInfoTypes[eTACTICAL_DESTROY_MEDIUM_UNIT] = GC.getInfoTypeForString("TACTICAL_DESTROY_MEDIUM_UNIT");
	m_CachedInfoTypes[eTACTICAL_DESTROY_LOW_UNIT] = GC.getInfoTypeForString("TACTICAL_DESTROY_LOW_UNIT");
	m_CachedInfoTypes[eTACTICAL_TO_SAFETY] = GC.getInfoTypeForString("TACTICAL_TO_SAFETY");
	m_CachedInfoTypes[eTACTICAL_ATTRIT_HIGH_UNIT] = GC.getInfoTypeForString("TACTICAL_ATTRIT_HIGH_UNIT");
	m_CachedInfoTypes[eTACTICAL_ATTRIT_MEDIUM_UNIT] = GC.getInfoTypeForString("TACTICAL_ATTRIT_MEDIUM_UNIT");
	m_CachedInfoTypes[eTACTICAL_ATTRIT_LOW_UNIT] = GC.getInfoTypeForString("TACTICAL_ATTRIT_LOW_UNIT");
	m_CachedInfoTypes[eTACTICAL_REPOSITION] = GC.getInfoTypeForString("TACTICAL_REPOSITION");
	m_CachedInfoTypes[eTACTICAL_BARBARIAN_CAMP] = GC.getInfoTypeForString("TACTICAL_BARBARIAN_CAMP");
	m_CachedInfoTypes[eTACTICAL_PILLAGE] = GC.getInfoTypeForString("TACTICAL_PILLAGE");
	m_CachedInfoTypes[eTACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN] = GC.getInfoTypeForString("TACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN");
	m_CachedInfoTypes[eTACTICAL_SAFE_BOMBARDS] = GC.getInfoTypeForString("TACTICAL_SAFE_BOMBARDS");
	m_CachedInfoTypes[eTACTICAL_HEAL] = GC.getInfoTypeForString("TACTICAL_HEAL");
	m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS] = GC.getInfoTypeForString("TACTICAL_ANCIENT_RUINS");
	m_CachedInfoTypes[eTACTICAL_GARRISON_TO_ALLOW_BOMBARD] = GC.getInfoTypeForString("TACTICAL_GARRISON_TO_ALLOW_BOMBARD");
	m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE] = GC.getInfoTypeForString("TACTICAL_GARRISON_ALREADY_THERE");
	m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE] = GC.getInfoTypeForString("TACTICAL_BASTION_ALREADY_THERE");
	m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE] = GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE");
	m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN] = GC.getInfoTypeForString("TACTICAL_GARRISON_1_TURN");
	m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN] = GC.getInfoTypeForString("TACTICAL_BASTION_1_TURN");
	m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_1_TURN] = GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_1_TURN");
	m_CachedInfoTypes[eTACTICAL_AIR_INTERCEPT] = GC.getInfoTypeForString("TACTICAL_AIR_INTERCEPT");
	m_CachedInfoTypes[eTACTICAL_AIR_SWEEP] = GC.getInfoTypeForString("TACTICAL_AIR_SWEEP");
	m_CachedInfoTypes[eTACTICAL_POSTURE_HEDGEHOG] = GC.getInfoTypeForString("TACTICAL_POSTURE_HEDGEHOG");
	m_CachedInfoTypes[eTACTICAL_POSTURE_WITHDRAW] = GC.getInfoTypeForString("TACTICAL_POSTURE_WITHDRAW");
	m_CachedInfoTypes[eTACTICAL_POSTURE_SHORE_BOMBARDMENT] = GC.getInfoTypeForString("TACTICAL_POSTURE_SHORE_BOMBARDMENT");
	m_CachedInfoTypes[eTACTICAL_CLOSE_ON_TARGET] = GC.getInfoTypeForString("TACTICAL_CLOSE_ON_TARGET");
	m_CachedInfoTypes[eTACTICAL_MOVE_OPERATIONS] = GC.getInfoTypeForString("TACTICAL_MOVE_OPERATIONS");
	m_CachedInfoTypes[eTACTICAL_EMERGENCY_PURCHASES] = GC.getInfoTypeForString("TACTICAL_EMERGENCY_PURCHASES");
	m_CachedInfoTypes[eTACTICAL_ESCORT_EMBARKED_UNIT] = GC.getInfoTypeForString("TACTICAL_ESCORT_EMBARKED_UNIT");
	m_CachedInfoTypes[eTACTICAL_PLUNDER_TRADE_UNIT] = GC.getInfoTypeForString("TACTICAL_PLUNDER_TRADE_UNIT");
	m_CachedInfoTypes[eTACTICAL_PARK_ON_TRADE_ROUTE] = GC.getInfoTypeForString("TACTICAL_PARK_ON_TRADE_ROUTE");
	m_CachedInfoTypes[eTACTICAL_DEFENSIVE_AIRLIFT] = GC.getInfoTypeForString("TACTICAL_DEFENSIVE_AIRLIFT");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL] = GC.getInfoTypeForString("TACTICAL_PILLAGE_CITADEL");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE] = GC.getInfoTypeForString("TACTICAL_PILLAGE_RESOURCE");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL_NEXT_TURN] = GC.getInfoTypeForString("TACTICAL_PILLAGE_CITADEL_NEXT_TURN");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE_NEXT_TURN] = GC.getInfoTypeForString("TACTICAL_PILLAGE_RESOURCE_NEXT_TURN");
	m_CachedInfoTypes[eTACTICAL_PILLAGE_NEXT_TURN] = GC.getInfoTypeForString("TACTICAL_PILLAGE_NEXT_TURN");
	m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT] = GC.getInfoTypeForString("MUPOSITION_CIVILIAN_SUPPORT");
	m_CachedInfoTypes[eMUPOSITION_NAVAL_ESCORT] = GC.getInfoTypeForString("MUPOSITION_NAVAL_ESCORT");
	m_CachedInfoTypes[eMUPOSITION_BOMBARD] = GC.getInfoTypeForString("MUPOSITION_BOMBARD");
	m_CachedInfoTypes[eMUPOSITION_FRONT_LINE] = GC.getInfoTypeForString("MUPOSITION_FRONT_LINE");

}

/// Deallocate memory created in initialize
void CvTacticalAI::Uninit()
{
}

/// Reset variables
void CvTacticalAI::Reset()
{
	m_MovePriorityTurn = -1;
	m_pMap = NULL;
}

/// Serialization read
void CvTacticalAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	int phony = 0;
	kStream >> phony;
	kStream >> m_TempZones;
}

/// Serialization write
void CvTacticalAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
	int phony = 0;
	kStream << phony;
	kStream << m_TempZones;
}

/// Mark all the units that will be under tactical AI control this turn
void CvTacticalAI::CommandeerUnits()
{
	UnitHandle pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();

	// Loop through our units
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Never want immobile/dead units, explorers, ones that have already moved or automated human units
		if(pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath() || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN ||  pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || !pLoopUnit->canMove() || pLoopUnit->isHuman())
		{
			continue;
		}

		// We want ALL the barbarians and air units (that are combat ready)
		else if(pLoopUnit->isBarbarian() || (pLoopUnit->getDomainType() == DOMAIN_AIR && pLoopUnit->getDamage() < 50 && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pLoopUnit.pointer())))
		{
			pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}

		// Now down to land and sea units ... in these groups our unit must have a base combat strength ... or be a great general
		else if(!pLoopUnit->IsCombatUnit() && !pLoopUnit->IsGreatGeneral())
		{
			continue;
		}

		else
		{
			// Is this one in an operation we can't interrupt?
			int iArmyID = pLoopUnit->getArmyID();
			const CvArmyAI* army = m_pPlayer->getArmyAI(iArmyID);
			if(iArmyID != FFreeList::INVALID_INDEX && NULL != army && !army->CanTacticalAIInterruptUnit(pLoopUnit->GetID()))
			{
				pLoopUnit->setTacticalMove(NO_TACTICAL_MOVE);
			}

			else
			{
				// Non-zero danger value, near enemy, or deploying out of an operation?
				int iDanger = m_pPlayer->GetPlotDanger(*(pLoopUnit->plot()));
				if(iDanger > 0 || NearVisibleEnemy(pLoopUnit, m_iRecruitRange) ||
				        pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
				{
					pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
					m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
				}
				else if (pLoopUnit->canParadrop(pLoopUnit->plot(),false))
				{
					pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
					m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
				}
			}
		}
	}
}

/// Set up for a turn of tactical moves
void CvTacticalAI::DoTurn()
{
}

/// Update the AI for units
void CvTacticalAI::Update()
{
	AI_PERF_FORMAT("AI-perf.csv", ("Tactical AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	FindTacticalTargets();

	// Loop through each dominance zone assigning moves
	ProcessDominanceZones();
}

// TEMPORARY DOMINANCE ZONES

/// Retrieve first temporary dominance zone (follow with calls to GetNextTemporaryZone())
CvTemporaryZone* CvTacticalAI::GetFirstTemporaryZone()
{
	CvTemporaryZone* pRtnValue = NULL;

	m_iCurrentTempZoneIndex = 0;
	if((int)m_TempZones.size() > m_iCurrentTempZoneIndex)
	{
		pRtnValue = &m_TempZones[m_iCurrentTempZoneIndex];
	}

	return pRtnValue;
}

/// Retrieve next temporary dominance zone, NULL if no more (should follow a call to GetFirstTemporaryZone())
CvTemporaryZone* CvTacticalAI::GetNextTemporaryZone()
{
	CvTemporaryZone* pRtnValue = NULL;

	m_iCurrentTempZoneIndex++;
	if((int)m_TempZones.size() > m_iCurrentTempZoneIndex)
	{
		pRtnValue = &m_TempZones[m_iCurrentTempZoneIndex];
	}

	return pRtnValue;
}

/// Add a temporary dominance zone around a short-term target
void CvTacticalAI::AddTemporaryZone(CvTemporaryZone zone)
{
	m_TempZones.push_back(zone);
}

/// Remove a temporary dominance zone we no longer need to track
void CvTacticalAI::DeleteTemporaryZone(CvPlot* pPlot)
{
	FStaticVector<CvTemporaryZone, SAFE_ESTIMATE_NUM_TEMP_ZONES, true, c_eCiv5GameplayDLL, 0> zonesCopy;

	// Copy zones over
	zonesCopy = m_TempZones;

	m_TempZones.clear();

	// Copy back to original vector any whose coords don't match
	for(unsigned int iI = 0; iI < zonesCopy.size(); iI++)
	{
		if(zonesCopy[iI].GetX() != pPlot->getX() ||
		        zonesCopy[iI].GetY() != pPlot->getY())
		{
			m_TempZones.push_back(zonesCopy[iI]);
		}
	}
}

/// Remove temporary zones that have expired
void CvTacticalAI::DropObsoleteZones()
{
	FStaticVector<CvTemporaryZone, SAFE_ESTIMATE_NUM_TEMP_ZONES, true, c_eCiv5GameplayDLL, 0> zonesCopy;

	// Copy zones over
	zonesCopy = m_TempZones;

	m_TempZones.clear();

	// Copy back to original vector any that haven't expired
	for(unsigned int iI = 0; iI < zonesCopy.size(); iI++)
	{
		if(zonesCopy[iI].GetLastTurn() >= GC.getGame().getGameTurn())
		{
			m_TempZones.push_back(zonesCopy[iI]);
		}
	}
}

/// Is this a city that an operation just deployed in front of?
bool CvTacticalAI::IsTemporaryZoneCity(CvCity* pCity)
{
	for(unsigned int iI = 0; iI < m_TempZones.size(); iI++)
	{
		if(m_TempZones[iI].GetX() == pCity->getX() &&
		        m_TempZones[iI].GetY() == pCity->getY() &&
		        m_TempZones[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
		{
			return true;
		}
	}
	return false;
}

/// Reset all data on queued attacks for a new turn
void CvTacticalAI::InitializeQueuedAttacks()
{
	m_QueuedAttacks.clear();
	m_iCurrentSeriesID = 0;
}

/// Queue up the attack - return TRUE if first attack on this target
bool CvTacticalAI::QueueAttack(void* pAttacker, CvTacticalTarget* pTarget, bool bRanged, bool bCityAttack)
{
	bool bRtnValue = true;
	CvQueuedAttack attack;
	int iSeriesID;
	CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
	CvPlot* pFromPlot;

	// Can we find this target in the queue, if so what is its series ID
	int iTempSeriesID = PlotAlreadyTargeted(pTargetPlot);

	if(iTempSeriesID == -1)
	{
		m_iCurrentSeriesID++;
		iSeriesID = m_iCurrentSeriesID;
	}
	else
	{
		iSeriesID = iTempSeriesID;
		bRtnValue = false;
	}

	attack.SetAttacker(pAttacker);
	attack.SetTarget(pTarget);
	attack.SetSeriesID(iSeriesID);
	attack.SetRanged(bRanged);
	attack.SetCityAttack(bCityAttack);
	m_QueuedAttacks.push_back(attack);

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		CvString strTemp;
		if(bCityAttack)
		{
			strTemp = ((CvCity*)pAttacker)->getName();
			pFromPlot = ((CvCity*)pAttacker)->plot();
		}
		else
		{
			CvUnit* pkAttackingUnit = static_cast<CvUnit*>(pAttacker);
			strTemp = pkAttackingUnit->getUnitInfo().GetDescription();
			pFromPlot = ((CvUnit*)pAttacker)->plot();
		}
		strMsg.Format("Queued attack with %s, To X: %d, To Y: %d, From X: %d, From Y: %d", strTemp.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY(), pFromPlot->getX(), pFromPlot->getY());
		LogTacticalMessage(strMsg);
	}

	return bRtnValue;
}

/// Pushes the mission to launch an attack and logs this activity
void CvTacticalAI::LaunchAttack(void* pAttacker, CvTacticalTarget* pTarget, bool bFirstAttack, bool bRanged, bool bCityAttack)
{
	CvCity* pCity = NULL;
	UnitHandle pUnit;

	if(bCityAttack)
	{
		pCity = (CvCity*)pAttacker;
	}
	else
	{
		pUnit = UnitHandle((CvUnit*)pAttacker);
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		CvString strTemp;
		int iX, iY;

		if(bCityAttack)
		{
			strTemp = pCity->getName();
			iX = pCity->getX();
			iY = pCity->getY();
		}
		else
		{
			strTemp = pUnit->getUnitInfo().GetDescription();
			iX = pUnit->getX();
			iY = pUnit->getY();
		}

		if(bRanged)
		{
			if(bFirstAttack)
			{
				strMsg.Format("Made initial ranged attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
			else
			{
				strMsg.Format("Made follow-on ranged attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}
		else
		{
			if(bFirstAttack)
			{
				strMsg.Format("Made initial attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
			else
			{
				strMsg.Format("Made follow-on attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), iX, iY, pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}

		LogTacticalMessage(strMsg);
	}

	// Attack AFTER logging since our attacker could die
	if(bCityAttack)
	{
		pCity->doTask(TASK_RANGED_ATTACK, pTarget->GetTargetX(), pTarget->GetTargetY(), 0);
	}
	else
	{
		CvAssertMsg(pUnit->getMoves() > 0, "Tactical AI is going to push a mission on the queue to attack, but the unit is out of moves.");
		CvAssertMsg(!pUnit->isOutOfAttacks(), "Tactical AI is going to push a mission on the queue to attack, but the unit is out of attacks.");

		bool bSendAttack = pUnit->getMoves() > 0 && !pUnit->isOutOfAttacks();
		if(bSendAttack)
		{
			if(bRanged && pUnit->getDomainType() != DOMAIN_AIR)	// Air attack is ranged, but it goes through the 'move to' mission.
			{
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}
			//else if (pUnit->canNuke(NULL)) // NUKE tactical attack (ouch)
			//{
			//	pUnit->PushMission(CvTypes::getMISSION_NUKE(), pTarget->GetTargetX(), pTarget->GetTargetY());
			//}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}

		// Make sure we did make an attack, if not we should take out this unit from the queue
		if(!bSendAttack || (!pUnit->isOutOfAttacks() && !pUnit->isFighting()))
		{
			pUnit->SetTurnProcessed(false);
			CombatResolved(pAttacker, false, bCityAttack);
		}
	}
}

/// Handles notification that an attack has been resolved (so can queue another)
void CvTacticalAI::CombatResolved(void* pAttacker, bool bVictorious, bool bCityAttack)
{
	int iSeriesID = 0;
	bool bFoundIt = false;

	CvCity* pCity = NULL;
	UnitHandle pUnit;

	if(bCityAttack)
	{
		pCity = (CvCity*)pAttacker;
	}
	else
	{
		pUnit = UnitHandle((CvUnit*)pAttacker);
	}

	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator nextToErase, nextInList;
		nextToErase = m_QueuedAttacks.begin();

		// Find first attack with this unit/city
		while(nextToErase != m_QueuedAttacks.end())
		{
			if(nextToErase->GetAttacker() == pAttacker)
			{
				iSeriesID = nextToErase->GetSeriesID();
				bFoundIt = true;
				break;
			}
			nextToErase++;
		}

		// Couldn't find it ... could have been an accidental attack moving to deploy near a target
		// So safe to ignore these
		if(!bFoundIt)
		{
			return;
		}

		// If this attacker gets multiple attacks, release him to be processed again
		if(!bCityAttack && (pUnit->canMoveAfterAttacking() || !pUnit->isOutOfAttacks()) && pUnit->getMoves() > 0)
		{
			pUnit->SetTurnProcessed(false);
		}

		// If victorious, dump follow-up attacks
		if(bVictorious)
		{
			bool bFirst = true;
			while(nextToErase != m_QueuedAttacks.end() && nextToErase->GetSeriesID() == iSeriesID)
			{
				// Only the first unit being erased is done for the turn
				if(!bFirst && !nextToErase->IsCityAttack())
				{
					UnitHandle pAbortUnit((CvUnit*)nextToErase->GetAttacker());
					pAbortUnit->SetTurnProcessed(false);
				}
				nextToErase = m_QueuedAttacks.erase(nextToErase);
				bFirst = false;
			}
		}

		// Otherwise look for a follow-up attack
		else
		{
			nextInList = nextToErase;
			nextInList++;

			// Matching IDs, we have a follow-up attack
			if(nextInList != m_QueuedAttacks.end() && nextInList->GetSeriesID() == nextToErase->GetSeriesID())
			{
				// Calling LaunchAttack can be recursive if the launched combat is resolved immediately.
				// We'll make a copy of the iterator's contents before erasing.  This is not technically needed because
				// the current queue is a std::list and iterators don't invalidate on erase, but we'll be safe, in case
				// the container type changes.
				CvQueuedAttack kAttack(*nextInList);
				m_QueuedAttacks.erase(nextToErase);
				LaunchAttack(kAttack.GetAttacker(), kAttack.GetTarget(), false /*bFirst*/, kAttack.IsRanged(), kAttack.IsCityAttack());
			}
			else
			{
				m_QueuedAttacks.erase(nextToErase);
			}
		}
	}

	return;
}

/// Do we already have a queued attack running on this plot? Return series ID if yes, -1 if no.
int CvTacticalAI::PlotAlreadyTargeted(CvPlot* pPlot)
{
	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			if(it->GetTarget()->GetTargetX() == pPlot->getX() &&
			        it->GetTarget()->GetTargetY() == pPlot->getY())
			{
				return it->GetSeriesID();
			}
		}
	}
	return -1;
}

/// Is this unit waiting to get its turn to attack?
bool CvTacticalAI::IsInQueuedAttack(const CvUnit* pUnit)
{
	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			if(it->GetAttacker() == pUnit)
			{
				return true;
			}
		}
	}
	return false;
}

/// Is this unit waiting to get its turn to attack?
bool CvTacticalAI::IsCityInQueuedAttack(const CvCity* pAttackCity)
{
	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			if(it->IsCityAttack() && it->GetAttacker() == (void*)pAttackCity)
			{
				return true;
			}
		}
	}
	return false;
}

/// Is this unit waiting to get its turn to attack?
int CvTacticalAI::NearXQueuedAttacks(const CvPlot* pPlot, const int iRange)
{
	int iNearbyAttacks = 0;

	if(m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for(it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			int iDistance;
			iDistance = plotDistance(pPlot->getX(), pPlot->getY(), it->GetTarget()->GetTargetX(), it->GetTarget()->GetTargetY());
			if(iDistance <= iRange)
			{
				iNearbyAttacks++;
			}
		}
	}
	return iNearbyAttacks;
}

// PRIVATE METHODS

/// Establish postures for each dominance zone (taking into account last posture)
void CvTacticalAI::UpdatePostures()
{
	CvTacticalDominanceZone* pZone;
	AITacticalPosture eLastPosture, eNewPosture;

	m_NewPostures.clear();

	// Loop through all the zones we have this turn
	for(int iI = 0; iI < m_pMap->GetNumZones(); iI++)
	{
		pZone = m_pMap->GetZone(iI);

		// Check to make sure we want to use this zone
		if(UseThisDominanceZone(pZone))
		{
			int iCityID = -1;
			if(pZone->GetClosestCity() != NULL)
			{
				iCityID = pZone->GetClosestCity()->GetID();
			}

			eLastPosture = FindPosture(pZone);
			eNewPosture = SelectPosture(pZone, eLastPosture);
			m_NewPostures.push_back(CvTacticalPosture(pZone->GetOwner(), pZone->IsWater(), iCityID, eNewPosture));

			if(GC.getLogging() && GC.getAILogging() && eNewPosture != AI_TACTICAL_POSTURE_NONE)
			{
				CvString szPostureMsg;
				szPostureMsg.Format("Zone ID: %d, ", pZone->GetDominanceZoneID());
				switch(eNewPosture)
				{
				case AI_TACTICAL_POSTURE_SIT_AND_BOMBARD:
					szPostureMsg += "Sit and Bombard";
					break;
				case AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE:
					szPostureMsg += "Attrit from Range";
					break;
				case AI_TACTICAL_POSTURE_EXPLOIT_FLANKS:
					szPostureMsg += "Exploit Flanks";
					break;
				case AI_TACTICAL_POSTURE_STEAMROLL:
					szPostureMsg += "Steamroll";
					break;
				case AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE:
					szPostureMsg += "Surgical City Strike";
					break;
				case AI_TACTICAL_POSTURE_HEDGEHOG:
					szPostureMsg += "Hedgehog";
					break;
				case AI_TACTICAL_POSTURE_COUNTERATTACK:
					szPostureMsg += "Counterattack";
					break;
				case AI_TACTICAL_POSTURE_WITHDRAW:
					szPostureMsg += "Withdraw";
					break;
				case AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT:
					szPostureMsg += "Shore Bombardment";
					break;
				}
				LogTacticalMessage(szPostureMsg);
			}
		}
	}

	// New postures become current ones
	m_Postures.clear();
	m_Postures = m_NewPostures;
}

/// Select a posture for a specific zone
AITacticalPosture CvTacticalAI::SelectPosture(CvTacticalDominanceZone* pZone, AITacticalPosture eLastPosture)
{
	AITacticalPosture eChosenPosture = AI_TACTICAL_POSTURE_NONE;
	int iDominancePercentage = GC.getGame().GetTacticalAnalysisMap()->GetDominancePercentage();
	eTacticalDominanceFlags eRangedDominance = TACTICAL_DOMINANCE_EVEN;
	eTacticalDominanceFlags eUnitCountDominance = TACTICAL_DOMINANCE_EVEN;
	int iRatio;

	// Compute who is dominant in various areas
	//   Ranged strength
	if(pZone->GetEnemyRangedStrength() <= 0)
	{
		eRangedDominance = TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		iRatio = (pZone->GetFriendlyRangedStrength() * 100) / pZone->GetEnemyRangedStrength();
		if(iRatio > 100 + iDominancePercentage)
		{
			eRangedDominance = TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if(iRatio < 100 - iDominancePercentage)
		{
			eRangedDominance = TACTICAL_DOMINANCE_ENEMY;
		}
	}

	//   Number of units
	if(pZone->GetEnemyUnitCount() <= 0)
	{
		eUnitCountDominance = TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		iRatio = (pZone->GetFriendlyUnitCount()  * 100) / pZone->GetEnemyUnitCount();
		if(iRatio > 100 + iDominancePercentage)
		{
			eUnitCountDominance = TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if(iRatio < 100 - iDominancePercentage)
		{
			eUnitCountDominance = TACTICAL_DOMINANCE_ENEMY;
		}
	}

	// Choice based on whose territory this is
	switch(pZone->GetTerritoryType())
	{
	case TACTICAL_TERRITORY_ENEMY:
	{
		bool bTemporaryZone = false;
		CvCity *pClosestCity = pZone->GetClosestCity();

		// Always push again for a few turns after each operation reaches destination
		if (pClosestCity)
		{
			bTemporaryZone = m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pClosestCity);
		}

		// Withdraw if enemy dominant overall or we have nothing but ranged left
		if(!bTemporaryZone && (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY || pZone->GetFriendlyRangedUnitCount() == pZone->GetFriendlyUnitCount()))
		{
			eChosenPosture = AI_TACTICAL_POSTURE_WITHDRAW;
		}

		// Temporary zone: want Steamroll of Surgical Strike so we close in on city
		else if (bTemporaryZone)
		{
			if (pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
			}
			else
			{
				eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
			}
		}

		// Destroy units then assault - for first time need dominance in total strength but not enemy dominance in ranged units OR just double total strength
		else if(pZone->GetEnemyUnitCount() > 0 && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY &&
		        (eRangedDominance != TACTICAL_DOMINANCE_ENEMY || pZone->GetFriendlyStrength() > pZone->GetEnemyStrength() * 2))
		{
			eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_STEAMROLL && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
		}

		// Sit and bombard - for first time need dominance in ranged strength and total unit count
		else if(eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SIT_AND_BOMBARD;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_SIT_AND_BOMBARD && eRangedDominance != TACTICAL_DOMINANCE_ENEMY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SIT_AND_BOMBARD;
		}

		// Go right after the city - need tactical dominance
		else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
		}

		// Exploit flanks - for first time need dominance in unit count
		else if(eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 1)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 1)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}

		// Default for this zone
		else
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
		}
		break;
	}

	case TACTICAL_TERRITORY_NEUTRAL:
	case TACTICAL_TERRITORY_NO_OWNER:
	{
		if(eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE && eRangedDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}

		// Exploit flanks - for first time need dominance in unit count
		else if(eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}

		// Default for this zone
		else
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}
		break;

		break;
	}
	case TACTICAL_TERRITORY_FRIENDLY:
	{
		if(eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetFriendlyRangedUnitCount() > 1)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE && pZone->GetFriendlyRangedUnitCount() > 1 && eRangedDominance != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		}

		// Exploit flanks - for first time need dominance in unit count
		else if(eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 0)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}

		// Counterattack - for first time must be stronger or even with enemy having a ranged advantage
		else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY ||
		        pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_EVEN && eRangedDominance == TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_COUNTERATTACK;
		}

		//                 - less stringent if continuing this from a previous turn
		else if(eLastPosture == AI_TACTICAL_POSTURE_COUNTERATTACK && pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_ENEMY)
		{
			eChosenPosture = AI_TACTICAL_POSTURE_COUNTERATTACK;
		}

		// Default for this zone
		else
		{
			eChosenPosture = AI_TACTICAL_POSTURE_HEDGEHOG;
		}
		break;
	}
	case TACTICAL_TERRITORY_TEMP_ZONE:
	{
		// Land or water?
		if(pZone->IsWater())
		{
			eChosenPosture = AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT;
		}
		else
		{
			// Should be a barbarian camp
			eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
		}
	}
	}

	return eChosenPosture;
}

/// Find last posture for a specific zone
AITacticalPosture CvTacticalAI::FindPosture(CvTacticalDominanceZone* pZone)
{
	if(pZone != NULL)
	{
		for(unsigned int iI = 0; iI < m_Postures.size(); iI++)
		{
			if(m_Postures[iI].GetPlayer() == pZone->GetOwner() &&
			        m_Postures[iI].IsWater() == pZone->IsWater())
			{
				int iCityID = -1;
				if(pZone->GetClosestCity() != NULL)
				{
					iCityID = pZone->GetClosestCity()->GetID();
				}
				if(m_Postures[iI].GetCityID() == iCityID)
				{
					return m_Postures[iI].GetPosture();
				}
			}
		}
	}

	return AI_TACTICAL_POSTURE_NONE;
}

/// Choose which tactics to emphasize this turn
void CvTacticalAI::EstablishTacticalPriorities()
{
	CvTacticalMove move;

	m_MovePriorityList.clear();

	// Loop through each possible tactical move
	for(int iI = 0; iI < GC.getNumTacticalMoveInfos(); iI++)
	{
		const TacticalAIMoveTypes eTacticalAIMove = static_cast<TacticalAIMoveTypes>(iI);
		CvTacticalMoveXMLEntry* pkTacticalMoveInfo = GC.getTacticalMoveInfo(eTacticalAIMove);
		if(pkTacticalMoveInfo)
		{
			move.m_eMoveType = eTacticalAIMove;
			move.m_iPriority = pkTacticalMoveInfo->GetPriority();

			if(move.m_iPriority >= 0)
			{
				m_MovePriorityList.push_back(move);
			}
		}
	}

	// Now sort the moves in priority order
	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());
}


/// Choose which tactics the barbarians should emphasize this turn
void CvTacticalAI::EstablishBarbarianPriorities()
{
	int iPriority = 0;

	// Only establish priorities once per turn
	if(GC.getGame().getGameTurn() <= m_MovePriorityTurn)
	{
		return;
	}

	m_MovePriorityList.clear();
	m_MovePriorityTurn = GC.getGame().getGameTurn();

	// Loop through each possible tactical move (other than "none" or "unassigned")
	for(int iI = 0; iI < NUM_AI_BARBARIAN_MOVES; iI++)
	{
		// Set base value
		switch((AIBarbarianTacticalMove)iI)
		{
		case AI_TACTICAL_BARBARIAN_CAPTURE_CITY:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY();
			break;
		case AI_TACTICAL_BARBARIAN_DAMAGE_CITY:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_HIGH_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_MEDIUM_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_LOW_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_MOVE_TO_SAFETY:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY();
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_HIGH_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_MEDIUM_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_LOW_PRIORITY_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT();
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE();
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_CITADEL:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL();
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_NEXT_TURN:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN();
			break;
		case AI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE();
			break;
		case AI_TACTICAL_BARBARIAN_CIVILIAN_ATTACK:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK();
			break;
		case AI_TACTICAL_BARBARIAN_AGGRESSIVE_MOVE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE();
			break;
		case AI_TACTICAL_BARBARIAN_PASSIVE_MOVE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE();
			break;
		case AI_TACTICAL_BARBARIAN_CAMP_DEFENSE:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE();
			break;
		case AI_TACTICAL_BARBARIAN_DESPERATE_ATTACK:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK();
			break;
		case AI_TACTICAL_BARBARIAN_ESCORT_CIVILIAN:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN();
			break;
		case AI_TACTICAL_BARBARIAN_PLUNDER_TRADE_UNIT:
			iPriority = GC.getAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT();
			break;
		}

		// Make sure base priority is not negative
		if(iPriority >= 0)
		{
			// Finally, add a random die roll to each priority
			iPriority += GC.getGame().getJonRandNum(GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS(), "Tactical AI Move Priority");

			// Store off this move and priority
			CvTacticalMove move;
			move.m_eMoveType = (TacticalAIMoveTypes)iI;
			move.m_iPriority = iPriority;
			m_MovePriorityList.push_back(move);
		}
	}

	// Now sort the moves in priority order
	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());
}


/// Make lists of everything we might want to target with the tactical AI this turn
void CvTacticalAI::FindTacticalTargets()
{
	int iI;
	CvPlot* pLoopPlot;
	CvTacticalTarget newTarget;
	bool bValidPlot;
	bool bEnemyDominatedPlot;
	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();

	// Clear out target list since we rebuild it each turn
	m_AllTargets.clear();

	bool bBarbsAllowedYet = GC.getGame().getGameTurn() >= GC.getGame().GetBarbarianReleaseTurn();

	// Look at every tile on map
	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		bValidPlot = false;

		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{
			// Make sure I am not a barbarian who can not move into owned territory this early in the game
			if(!m_pPlayer->isBarbarian() || bBarbsAllowedYet)
			{
				bValidPlot = true;
			}
			else
			{
				if(!pLoopPlot->isOwned())
				{
					bValidPlot = true;
				}
			}
			if(bValidPlot)
			{
				if(PlotAlreadyTargeted(pLoopPlot) != -1)
				{
					bValidPlot = false;
				}
			}
		}

		if(bValidPlot)
		{
			newTarget.Clear();
			newTarget.SetTargetX(pLoopPlot->getX());
			newTarget.SetTargetY(pLoopPlot->getY());
			newTarget.SetDominanceZone(GC.getGame().GetTacticalAnalysisMap()->GetCell(iI)->GetDominanceZone());

			bEnemyDominatedPlot = GC.getGame().GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pLoopPlot);

			// Have a ...
			// ... friendly city?
			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity != NULL)
			{
				if(m_pPlayer->GetID() == pCity->getOwner())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
					newTarget.SetAuxData((void*)pCity);
					newTarget.SetAuxIntData(pCity->getThreatValue());
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy city
				else if(atWar(m_pPlayer->getTeam(), pCity->getTeam()))
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY);
					newTarget.SetTargetPlayer(pCity->getOwner());
					newTarget.SetAuxData((void*)pCity);
					m_AllTargets.push_back(newTarget);
				}
			}
			else
			{
				// ... enemy unit?
				CvUnit* pUnit = pLoopPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if(pUnit != NULL)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
					newTarget.SetTargetPlayer(pUnit->getOwner());
					newTarget.SetAuxData((void*)pUnit);
					newTarget.SetAuxIntData(pUnit->getDamage());
					m_AllTargets.push_back(newTarget);
				}

				// ... undefended camp?
				else if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
					newTarget.SetTargetPlayer(BARBARIAN_PLAYER);
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... goody hut?
				else if(!m_pPlayer->isMinorCiv() && pLoopPlot->isGoody())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_ANCIENT_RUINS);
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// Or citadels!
				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					GC.getImprovementInfo(pLoopPlot->getImprovementType())->GetNearbyEnemyDamage() > 0)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITADEL);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy improvement?
				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
				        pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
				        !pLoopPlot->IsImprovementPillaged())
				{
					ResourceUsageTypes eRUT = (ResourceUsageTypes)-1;
					ResourceTypes eResource = pLoopPlot->getResourceType();
					if (eResource != NO_RESOURCE)
					{
						eRUT = GC.getResourceInfo(eResource)->getResourceUsage();
					}

					// On land, civs prioritize improvements built on resources
					if (eRUT == RESOURCEUSAGE_STRATEGIC || eRUT == RESOURCEUSAGE_LUXURY || pLoopPlot->isWater() || m_pPlayer->isBarbarian())
					{
						// Barbarians can't target naval improvements
						if(m_pPlayer->isBarbarian() && pLoopPlot->isWater())
						{
							continue;
						}
						else
						{
							newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE);
							newTarget.SetTargetPlayer(pLoopPlot->getOwner());
							newTarget.SetAuxData((void*)pLoopPlot);
							m_AllTargets.push_back(newTarget);
						}
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
						newTarget.SetTargetPlayer(pLoopPlot->getOwner());
						newTarget.SetAuxData((void*)pLoopPlot);
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... enemy trade route?
				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
				        pLoopPlot->getRouteType() != NO_ROUTE && !pLoopPlot->IsRoutePillaged() && pLoopPlot->IsTradeRoute()/* && !bEnemyDominatedPlot*/)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy civilian (or embarked) unit?
				else if(pLoopPlot->isVisibleOtherUnit(m_pPlayer->GetID()))
				{
					CvUnit* pTargetUnit = pLoopPlot->getUnitByIndex(0);
					if(!pTargetUnit->isDelayedDeath() && atWar(m_pPlayer->getTeam(), pTargetUnit->getTeam()) && !pTargetUnit->IsCanDefend())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
						newTarget.SetTargetPlayer(pTargetUnit->getOwner());
						newTarget.SetAuxData((void*)pTargetUnit);

						if(pTargetUnit->isEmbarked())
						{
							if(pTargetUnit->IsCombatUnit())
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT);
							}
							else
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_EMBARKED_CIVILIAN);
							}
						}
						else
						{
							if(IsVeryHighPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
							}
							else if(IsHighPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
							}
							else if(IsMediumPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
							}
						}
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... trade unit
				else if (pPlayerTrade->ContainsEnemyTradeUnit(pLoopPlot))
				{
					if (pLoopPlot->isWater())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_SEA);
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_LAND);
					}

					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... defensive bastion?
				else if(m_pPlayer->GetID() == pLoopPlot->getOwner() &&
				        pLoopPlot->defenseModifier(m_pPlayer->getTeam(), true) > 0 &&
				        m_pPlayer->GetPlotDanger(*pLoopPlot) > 0)
				{
					CvCity* pDefenseCity = pLoopPlot->GetAdjacentFriendlyCity(m_pPlayer->getTeam(), true/*bLandOnly*/);
					if(pDefenseCity)
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
						newTarget.SetAuxData((void*)pLoopPlot);
						newTarget.SetAuxIntData(pDefenseCity->getThreatValue() + m_pPlayer->GetPlotDanger(*pLoopPlot));
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... friendly improvement?
				else if(m_pPlayer->GetID() == pLoopPlot->getOwner() &&
				        pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
				        !pLoopPlot->IsImprovementPillaged() && !pLoopPlot->isGoody())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... trade plot (for getting units to park on trade routes to try to get them to plunder enemy trade routes)
				else if (pLoopPlot->isVisible(m_pPlayer->getTeam()) && 
						 pPlayerTrade->ContainsEnemyTradePlot(pLoopPlot))
				{
					if (pLoopPlot->isWater())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_SEA_PLOT);
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT);
					}

					newTarget.SetAuxData((void*)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}
			}
		}
	}

	// POST-PROCESSING ON TARGETS

	// Mark enemy units threatening our cities (or camps) as priority targets
	if(m_pPlayer->isBarbarian())
	{
		IdentifyPriorityBarbarianTargets();
	}
	else
	{
		IdentifyPriorityTargets();
	}

	// Also add some priority targets that we'd like to hit just because of their unit type (e.g. catapults)
	IdentifyPriorityTargetsByType();

	// Remove extra targets
	EliminateNearbyBlockadePoints();

	// Sort remaining targets by aux data (if used for that target type)
	std::stable_sort(m_AllTargets.begin(), m_AllTargets.end());
}

void CvTacticalAI::ProcessDominanceZones()
{
	CvTacticalDominanceZone* pZone;
	FStaticVector<CvTacticalMove, 256, true, c_eCiv5GameplayDLL >::iterator it;

	// Barbarian processing is straightforward -- just one big list of priorites and everything is considered at once
	if(m_pPlayer->isBarbarian())
	{
		EstablishBarbarianPriorities();
		ExtractTargetsForZone(NULL);
		AssignBarbarianMoves();
	}

	else
	{
		EstablishTacticalPriorities();
		UpdatePostures();

		// Proceed in priority order
		for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
		{
			CvTacticalMove move = *it;

			if(move.m_iPriority >= 0)
			{
				CvTacticalMoveXMLEntry* pkTacticalMoveInfo = GC.getTacticalMoveInfo(move.m_eMoveType);
				if(pkTacticalMoveInfo && pkTacticalMoveInfo->IsDominanceZoneMove())
				{
					for(int iI = 0; iI < m_pMap->GetNumZones(); iI++)
					{
						m_iCurrentZoneIndex = iI;
						pZone = m_pMap->GetZone(iI);
						AITacticalPosture ePosture = FindPosture(pZone);
						CvString strMoveName = (CvString)pkTacticalMoveInfo->GetType();
						CvTacticalMove moveToPassOn = move;

						// Is this move of the right type for this zone?
						bool bMatch = false;
						if(strMoveName == "TACTICAL_CLOSE_ON_TARGET")    // This one okay for all zones
						{
							bMatch = true;
						}
						else if(ePosture == AI_TACTICAL_POSTURE_WITHDRAW && strMoveName == "TACTICAL_POSTURE_WITHDRAW")
						{
							bMatch = true;
						}
						else if(ePosture == AI_TACTICAL_POSTURE_HEDGEHOG && strMoveName == "TACTICAL_POSTURE_HEDGEHOG")
						{
							bMatch = true;
						}
						else if(ePosture == AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT && strMoveName == "TACTICAL_POSTURE_SHORE_BOMBARDMENT")
						{
							bMatch = true;
						}
						else if(pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && strMoveName == "TACTICAL_EMERGENCY_PURCHASES")
						{
							bMatch = true;
						}
						else if(pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && strMoveName == "TACTICAL_DEFENSIVE_AIRLIFT")
						{
							bMatch = true;
						}
						else if (strMoveName == "TACTICAL_OFFENSIVE_POSTURE_MOVES")
						{
							// If this is an offensive posture move, map us to the detailed move type.  Why?  Well we want these offensive moves
							// processed in order of zone priority. This allows us to set them all to the same priority but then break them out
							// into individual move types that get passed into AssignTacticalMove()
							bMatch = true;
							
							switch (ePosture)
							{
							case AI_TACTICAL_POSTURE_SIT_AND_BOMBARD:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SIT_AND_BOMBARD];
								break;
							case AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_ATTRIT_FROM_RANGE];
								break;
							case AI_TACTICAL_POSTURE_EXPLOIT_FLANKS:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_EXPLOIT_FLANKS];
								break;
							case AI_TACTICAL_POSTURE_STEAMROLL:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_STEAMROLL];
								break;
							case AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SURGICAL_CITY_STRIKE];
								break;
							case AI_TACTICAL_POSTURE_COUNTERATTACK:
								moveToPassOn.m_eMoveType = (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_COUNTERATTACK];
								break;
							default:
								bMatch = false;
								break;
							}
						}

						if(bMatch)
						{
							if(!UseThisDominanceZone(pZone))
							{
								continue;
							}

							ExtractTargetsForZone(pZone);

							// Must have some moves to continue or it must be land around an enemy city (which we always want to process because
							// we might have an operation targeting it)
							if(m_ZoneTargets.size() <= 0 && pZone->GetTerritoryType() != TACTICAL_TERRITORY_TEMP_ZONE && (pZone->GetTerritoryType() != TACTICAL_TERRITORY_ENEMY || pZone->IsWater()))
							{
								continue;
							}

							AssignTacticalMove(moveToPassOn);
						}
					}
				}
				else
				{
					ExtractTargetsForZone(NULL);
					AssignTacticalMove(move);
				}
			}
		}
	}
}

/// Choose which tactics to run and assign units to it
void CvTacticalAI::AssignTacticalMove(CvTacticalMove move)
{
#pragma warning ( push )
#pragma warning ( disable : 6011 ) // Dereferencing NULL pointer
	AI_PERF_FORMAT("AI-perf-tact.csv", ("Move Type: %s (%d), Turn %03d, %s", GC.getTacticalMoveInfo(move.m_eMoveType)->GetType(), (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
#pragma warning ( pop )

	if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY])
	{
		PlotMovesToSafety(false /*bCombatUnits*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_CAPTURE_CITY])
	{
		PlotCaptureCityMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DAMAGE_CITY])
	{
		PlotDamageCityMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DESTROY_HIGH_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DESTROY_MEDIUM_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DESTROY_LOW_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_TO_SAFETY])
	{
		PlotMovesToSafety(true /*bCombatUnits*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTRIT_HIGH_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTRIT_MEDIUM_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTRIT_LOW_UNIT])
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_REPOSITION])
	{
		PlotRepositionMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BARBARIAN_CAMP])
	{
		PlotBarbarianCampMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, true /*bFirstPass*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true /*bFirstPass*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, true /*bFirstPass*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_CITADEL_NEXT_TURN])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, false /*bFirstPass*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_RESOURCE_NEXT_TURN])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, false /*bFirstPass*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PILLAGE_NEXT_TURN])
	{
		PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, false /*bFirstPass*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PLUNDER_TRADE_UNIT])
	{
		PlotPlunderTradeUnitMoves(DOMAIN_LAND);
		PlotPlunderTradeUnitMoves(DOMAIN_SEA);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_PARK_ON_TRADE_ROUTE])
	{
		PlotPlunderTradePlotMoves(DOMAIN_LAND);
		PlotPlunderTradePlotMoves(DOMAIN_SEA);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN])
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_SAFE_BOMBARDS])
	{
		PlotSafeBombardMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_HEAL])
	{
		PlotHealMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS])
	{
		PlotAncientRuinMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_TO_ALLOW_BOMBARD])
	{
		PlotGarrisonMoves(1, true /*bMustAllowRangedAttack*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE])
	{
		PlotGarrisonMoves(0);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE])
	{
		PlotBastionMoves(0);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE])
	{
		PlotGuardImprovementMoves(0);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN])
	{
		PlotGarrisonMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN])
	{
		PlotBastionMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_1_TURN])
	{
		PlotGuardImprovementMoves(1);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_AIR_INTERCEPT])
	{
		PlotAirInterceptMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_AIR_SWEEP])
	{
		PlotAirSweepMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SIT_AND_BOMBARD])
	{
		PlotSitAndBombardMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_ATTRIT_FROM_RANGE])
	{
		PlotAttritFromRangeMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_EXPLOIT_FLANKS])
	{
		PlotExploitFlanksMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_STEAMROLL])
	{
		PlotSteamrollMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SURGICAL_CITY_STRIKE])
	{
		PlotSurgicalCityStrikeMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_HEDGEHOG])
	{
		PlotHedgehogMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_COUNTERATTACK])
	{
		PlotCounterattackMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_WITHDRAW])
	{
		PlotWithdrawMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SHORE_BOMBARDMENT])
	{
		PlotShoreBombardmentMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_CLOSE_ON_TARGET])
	{
		PlotCloseOnTarget(true /*bCheckDominance*/);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_MOVE_OPERATIONS])
	{
		PlotOperationalArmyMoves();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_EMERGENCY_PURCHASES])
	{
		PlotEmergencyPurchases();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DEFENSIVE_AIRLIFT])
	{
		PlotDefensiveAirlifts();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ESCORT_EMBARKED_UNIT])
	{
		PlotEscortEmbarkedMoves();
	}
}

/// Choose which tactics to run and assign units to it (barbarian version)
void CvTacticalAI::AssignBarbarianMoves()
{
	FStaticVector<CvTacticalMove, 256, true, c_eCiv5GameplayDLL >::iterator it;

	// Proceed in priority order
	for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
	{
		CvTacticalMove move = *it;

		AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Move: %d, Turn %03d, %s", (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

		switch(move.m_eMoveType)
		{
		case AI_TACTICAL_BARBARIAN_CAPTURE_CITY:
			PlotCaptureCityMoves();
			break;
		case AI_TACTICAL_BARBARIAN_DAMAGE_CITY:
			PlotDamageCityMoves();
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_HIGH_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_MEDIUM_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_LOW_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
			break;
		case AI_TACTICAL_BARBARIAN_MOVE_TO_SAFETY:
			PlotMovesToSafety(true /*bCombatUnits*/);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_HIGH_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_MEDIUM_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_LOW_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE:
			PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true/*bFirstPass*/);
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_CITADEL:
			PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, true/*bFirstPass*/);
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_NEXT_TURN:
			PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, false/*bFirstPass*/);
			PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, false/*bFirstPass*/);
			break;
		case AI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE:
//			PlotBlockadeImprovementMoves();
			break;
		case AI_TACTICAL_BARBARIAN_CIVILIAN_ATTACK:
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
			break;
		case AI_TACTICAL_BARBARIAN_CAMP_DEFENSE:
			PlotCampDefenseMoves();
			break;
		case AI_TACTICAL_BARBARIAN_AGGRESSIVE_MOVE:
			PlotBarbarianMove(true /*bAggressive*/);
			break;
		case AI_TACTICAL_BARBARIAN_PASSIVE_MOVE:
			PlotBarbarianMove(false /*bAggressive*/);
			break;
		case AI_TACTICAL_BARBARIAN_DESPERATE_ATTACK:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, true);
			break;
		case AI_TACTICAL_BARBARIAN_ESCORT_CIVILIAN:
			PlotBarbarianCivilianEscortMove();
			break;
		case AI_TACTICAL_BARBARIAN_PLUNDER_TRADE_UNIT:
			PlotBarbarianPlunderTradeUnitMove(DOMAIN_LAND);
			PlotBarbarianPlunderTradeUnitMove(DOMAIN_SEA);
			break;
		}
	}

	ReviewUnassignedUnits();
}

/// Assign a group of units to take down each city we can capture
bool CvTacticalAI::PlotCaptureCityMoves()
{
	int iRequiredDamage;
	bool bAttackMade = false;
	bool bLog = GC.getLogging() && GC.getAILogging();

	// See how many moves of this type we can execute
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(pPlot != NULL)
		{
			m_CurrentMoveCities.clear();

#if defined(MOD_AI_SMART_MELEE_TACTICS)
			if (MOD_AI_SMART_MELEE_TACTICS)
			{
				CvCity* pCity = pPlot->getPlotCity();
			
				if(pCity != NULL)
				{
					//If don't have units to actually conquer, get out.
					if(!FindUnitsWithinStrikingDistance(pPlot, 1, 0, true /*bNoRangedUnits*/, false /*bNavalOnly*/, false /*bMustMoveThrough*/, true /*bIncludeBlockedUnits*/))
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString("No units to capture city");
							LogTacticalMessage(strLogString);
						}
						return bAttackMade;
					}

					// Do we have enough firepower to destroy it?
					iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
					pTarget->SetAuxIntData(iRequiredDamage);
					// If we have the city already down to minimum, don't use ranged... Only try to capture.
					// This will never be true, as the city heals between turns!!!
					bool bNoRangedUnits = (iRequiredDamage <= 1);
					if (bNoRangedUnits) CUSTOMLOG("City attack without ranged units");
					if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, bNoRangedUnits, false /*bNavalOnly*/, false /*bMustMoveThrough*/, true /*bIncludeBlockedUnits*/))
					{
						if(ComputeTotalExpectedDamage(pTarget, pPlot) >= (iRequiredDamage / 2)) // risky
						{
							// If so, execute enough moves to take it
							ExecuteAttack(pTarget, pPlot, false, false);
							bAttackMade = true;

							// Did it work?  If so, don't need a temporary dominance zone if had one here
							if(pPlot->getOwner() == m_pPlayer->GetID())
							{
								DeleteTemporaryZone(pPlot);
							}
						}
					}
				}
			}
			else
			{
#endif
				if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false /*bNoRangedUnits*/, false /*bNavalOnly*/, false /*bMustMoveThrough*/, true /*bIncludeBlockedUnits*/))
				{
					// Do we have enough firepower to destroy it?
					CvCity* pCity = pPlot->getPlotCity();
					if(pCity != NULL)
					{
						iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
						pTarget->SetAuxIntData(iRequiredDamage);
						if(ComputeTotalExpectedDamage(pTarget, pPlot) >= (iRequiredDamage / 2)) // risky
						{
							// Log result
							if(bLog)
							{
								CvString strPlayerName, strCityName, strLogString, strTemp;
								strPlayerName = GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
								strCityName = pCity->getName();
								strLogString.Format("Attacking city of ");
								strLogString += strCityName;
								strTemp.Format(" to capture, X: %d, Y: %d,", pCity->getX(), pCity->getY());
								strLogString += strTemp + strPlayerName;
								LogTacticalMessage(strLogString);
							}

							// If so, execute enough moves to take it
							ExecuteAttack(pTarget, pPlot, false, false);
							bAttackMade = true;

							// Did it work?  If so, don't need a temporary dominance zone if had one here
							if(pPlot->getOwner() == m_pPlayer->GetID())
							{
								DeleteTemporaryZone(pPlot);
							}
						}
					}
				}
#if defined(MOD_AI_SMART_MELEE_TACTICS)
			}
#endif
		}
		pTarget = GetNextZoneTarget();
	}
	return bAttackMade;
}

/// Assign a group of units to take down each city we can capture
bool CvTacticalAI::PlotDamageCityMoves()
{
	int iRequiredDamage;
	bool bAttackMade = false;
	bool bLog = GC.getLogging() && GC.getAILogging();

	// See how many moves of this type we can execute
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(pPlot != NULL)
		{
			m_CurrentMoveCities.clear();

#if defined(MOD_AI_SMART_MELEE_TACTICS)
			if (MOD_AI_SMART_MELEE_TACTICS)
			{
				CvCity* pCity = pPlot->getPlotCity();

				if(pCity != NULL)
				{
					//If don't have units nearby to actually conquer, and bad dominance flag, get out.
					if(!FindUnitsWithinStrikingDistance(pPlot, 2, 0, true /*bNoRangedUnits*/, false /*bNavalOnly*/, false /*bMustMoveThrough*/, true /*bIncludeBlockedUnits*/))
					{
						CvTacticalDominanceZone* pZone;
						pZone = m_pMap->GetZoneByCity(pCity, false);
						if (pZone != NULL)
						{
							if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
							{
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString("Pulling back from city, no melee support for conquering");
									LogTacticalMessage(strLogString);
								}
								return bAttackMade;
							}
						}
						else
						{
							return bAttackMade;
						}
					}

					iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
					pTarget->SetAuxIntData(iRequiredDamage);
					// If we have the city already down to minimum, don't use ranged... Only try to capture.
					// This will never be true, as the city heals between turns!!!
					bool bNoRangedUnits = (iRequiredDamage <= 1);
					if (bNoRangedUnits) CUSTOMLOG("City attack without ranged units");
					if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, bNoRangedUnits, false /*bNavalOnly*/, false /*bMustMoveThrough*/, true /*bIncludeBlockedUnits*/))
					{
						int iExpectedDamage = ComputeTotalExpectedDamage(pTarget, pPlot);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Laying siege to city, required damage %d, expected damage %d", iRequiredDamage, iExpectedDamage);
							LogTacticalMessage(strLogString);
						}

						// Don't want to hammer away to try and take down a city for more than 8 turns
						if(ComputeTotalExpectedDamage(pTarget, pPlot) > (iRequiredDamage / 8))
						{
							// If so, execute enough moves to take it
							ExecuteAttack(pTarget, pPlot, false, true);
							bAttackMade = true;
						}
					}
				}
			}
			else
			{
#endif
				if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false /*bNoRangedUnits*/, false /*bNavalOnly*/, false /*bMustMoveThrough*/, true /*bIncludeBlockedUnits*/))
				{
					CvCity* pCity = pPlot->getPlotCity();
					if(pCity != NULL)
					{
						iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
						pTarget->SetAuxIntData(iRequiredDamage);

						// Don't want to hammer away to try and take down a city for more than 8 turns
						if(ComputeTotalExpectedDamage(pTarget, pPlot) > (iRequiredDamage / 8))
						{
							// Log result
							if(bLog)
							{
								CvString strPlayerName, strCityName, strLogString, strTemp;
								strPlayerName = GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
								strCityName = pCity->getName();
								strLogString.Format("Attacking city of ");
								strLogString += strCityName;
								strTemp.Format(" to damage, X: %d, Y: %d,", pCity->getX(), pCity->getY());
								strLogString += strTemp + strPlayerName;
								LogTacticalMessage(strLogString);
							}

							// If so, execute enough moves to take it
							ExecuteAttack(pTarget, pPlot, false, true);
							bAttackMade = true;
						}
					}
				}
#if defined(MOD_AI_SMART_MELEE_TACTICS)
			}
#endif
		}
		pTarget = GetNextZoneTarget();
	}
	return bAttackMade;
}

/// Assign a unit to capture an undefended barbarian camp
void CvTacticalAI::PlotBarbarianCampMoves()
{
	bool bLog = GC.getLogging() && GC.getAILogging();
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false /* bNoRangedUnits */, false /*bNavalOnly*/, false /*bMustMoveThrough*/, false /*bIncludeBlockedUnits*/, false /*bWillPillage*/, true /*bTargetUndefended*/))
		{
			// Queue best one up to capture it
			ExecuteBarbarianCampMove(pPlot);
			if(bLog)
			{
				CvString strLogString;
				strLogString.Format("Removing barbarian camp, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
				LogTacticalMessage(strLogString);
			}
			DeleteTemporaryZone(pPlot);
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Assign a group of units to attack each unit we think we can destroy
void CvTacticalAI::PlotDestroyUnitMoves(AITacticalTargetType targetType, bool bMustBeAbleToKill, bool bAttackAtPoorOdds)
{
	int iRequiredDamage;
	int iExpectedDamage;

	// See how many moves of this type we can execute
	CvTacticalTarget* pTarget = GetFirstZoneTarget(targetType);
	while(pTarget != NULL)
	{
		bool bUnitCanAttack = false;
		bool bCityCanAttack = false;

		// See what units we have who can reach targets this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		UnitHandle pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
		if(pDefender)
		{
			bUnitCanAttack = FindUnitsWithinStrikingDistance(pPlot, 1, 0, false /* bNoRangedUnits */);
			bCityCanAttack = FindCitiesWithinStrikingDistance(pPlot);
			if(bUnitCanAttack || bCityCanAttack)
			{
				iExpectedDamage = ComputeTotalExpectedDamage(pTarget, pPlot);
				iExpectedDamage += ComputeTotalExpectedBombardDamage(pDefender);
				iRequiredDamage = pDefender->GetCurrHitPoints();
				pTarget->SetAuxIntData(iRequiredDamage);

				if(!bMustBeAbleToKill)
				{
					// Put in any attacks where we'll inflict at least equal damage
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;

						CvString strPlayerName = GET_PLAYER(pDefender->getOwner()).getCivilizationShortDescription();

						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pDefender->getUnitType());
						CvString strTemp = (pkUnitInfo != NULL)? pkUnitInfo->GetDescription() : "Unknown Unit Type";

						switch(targetType)
						{
						case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
							strLogString.Format("Looking at damaging high priority %s, X: %d, Y: %d,", strTemp.GetCString(),
							                    pDefender->getX(), pDefender->getY());
							break;
						case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
							strLogString.Format("Looking at damaging medium priority %s, X: %d, Y: %d,", strTemp.GetCString(),
							                    pDefender->getX(), pDefender->getY());
							break;
						case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
							strLogString.Format("Looking at damaging low priority %s, X: %d, Y: %d,", strTemp.GetCString(),
							                    pDefender->getX(), pDefender->getY());
							break;
						}
						strLogString += strPlayerName;
						LogTacticalMessage(strLogString);
					}

					// Attack no matter what
					if(bAttackAtPoorOdds)
					{
						ExecuteAttack(pTarget, pPlot, false, false);
					}
					// If we can at least knock the defender to 40% strength with our combined efforts, go ahead even if each individual attack isn't favorable
					else
					{
						bool bMustInflictWhatWeTake = true;
						if (iExpectedDamage >= (iRequiredDamage * 40) / 100)
						{
							bMustInflictWhatWeTake = false;
						}
						ExecuteAttack(pTarget, pPlot, bMustInflictWhatWeTake, true);
					}
				}

				// Do we have enough firepower to destroy it?
				else
				{
					if(iExpectedDamage > iRequiredDamage)
					{
						// If so, execute enough moves to destroy it
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString, strTemp, strPlayerName;
							strPlayerName = GET_PLAYER(pDefender->getOwner()).getCivilizationShortDescription();
							strTemp = GC.getUnitInfo(pDefender->getUnitType())->GetDescription();
							switch(targetType)
							{
							case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
								strLogString.Format("Looking at killing high priority %s, X: %d, Y: %d,", strTemp.GetCString(),
								                    pDefender->getX(), pDefender->getY());
								break;
							case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
								strLogString.Format("Looking at killing medium priority %s, X: %d, Y: %d,", strTemp.GetCString(),
								                    pDefender->getX(), pDefender->getY());
								break;
							case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
								strLogString.Format("Looking at killing low priority %s, X: %d, Y: %d,", strTemp.GetCString(),
								                    pDefender->getX(), pDefender->getY());
								break;
							}
							strLogString += strPlayerName;
							LogTacticalMessage(strLogString);
						}
						ExecuteAttack(pTarget, pPlot, false, (targetType != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT));
					}
				}
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Moved endangered units to safe hexes
void CvTacticalAI::PlotMovesToSafety(bool bCombatUnits)
{
	int iDangerLevel;

	list<int>::iterator it;
	m_CurrentMoveUnits.clear();

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Danger value of plot must be greater than 0
			CvPlot* pPlot = pUnit->plot();

			iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
			if(iDangerLevel > 0)
			{
				bool bAddUnit = false;
				if(bCombatUnits)
				{
					// If under 100% health, might flee to safety
					if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
					{
						if(pUnit->isBarbarian())
						{
							// Barbarian combat units - only naval units flee (but they flee if have taken ANY damage)
							if(pUnit->getDomainType() == DOMAIN_SEA)
							{
								bAddUnit = true;
							}
						}

						// Everyone else flees if under enemy fire or if at less than or equal to 50% combat strength
						else if (pUnit->IsUnderEnemyRangedAttack() || pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
						{
							bAddUnit = true;
						}
#if defined(MOD_BALANCE_CORE_MILITARY)
						else if(MOD_BALANCE_CORE_MILITARY && pUnit->isEmbarked())
						{
							//Are you embarked, and 50% injured, and alone, and in enemy territory? Bail out!
							if(pUnit->plot()->getOwner() != pUnit->getOwner() && !pUnit->IsFriendlyUnitAdjacent(true) && (pUnit->GetCurrHitPoints() <= (pUnit->GetMaxHitPoints() / 2)))
							{
								bAddUnit = true;
							}
						}
#endif
					}

					// Also flee if danger is really high in current plot (but not if we're barbarian)
					else if(!pUnit->isBarbarian())
					{
						int iAcceptableDanger;
						iAcceptableDanger = pUnit->GetBaseCombatStrengthConsideringDamage() * 100;
						if(iDangerLevel > iAcceptableDanger)
						{
							bAddUnit = true;
						}
					}
				}
				else
				{
					// Civilian (or embarked) units always flee from danger
					if(!pUnit->IsCanDefend())
					{
						bAddUnit = true;
					}
				}

				if(bAddUnit)
				{
					// Just one unit involved in this move to execute
					CvTacticalUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMovesToSafestPlot();
	}
}

/// Move units to a better location
void CvTacticalAI::PlotRepositionMoves()
{
	list<int>::iterator it;
	CvTacticalUnit unit;

	m_CurrentMoveUnits.clear();

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Never use this (default) move for Great Admirals or Generals
			if (pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
			{
				continue;
			}

			unit.SetID(pUnit->GetID());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteRepositionMoves();
	}
}

/// Move barbarians across the map
void CvTacticalAI::PlotBarbarianMove(bool bAggressive)
{
	list<int>::iterator it;
	CvTacticalUnit unit;

	if(m_pPlayer->isBarbarian())
	{
		m_CurrentMoveUnits.clear();

		// Loop through all recruited units
		for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);
			if(pUnit)
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}

		if(m_CurrentMoveUnits.size() > 0)
		{
			ExecuteBarbarianMoves(bAggressive);
		}
	}
}

/// Escort captured civilians back to barbarian camps
void CvTacticalAI::PlotBarbarianCivilianEscortMove()
{
	list<int>::iterator it;
	CvTacticalUnit unit;

	if(m_pPlayer->isBarbarian())
	{
		m_CurrentMoveUnits.clear();

		for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);

			// Find any civilians we may have "acquired" from the civs
			if(!pUnit->IsCanAttack())
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}

		if(m_CurrentMoveUnits.size() > 0)
		{
			ExecuteBarbarianCivilianEscortMove();
		}
	}
}

/// Plunder trade routes
void CvTacticalAI::PlotBarbarianPlunderTradeUnitMove(DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = AI_TACTICAL_TARGET_NONE;
	bool bNavalOnly = false;
	if (eDomain == DOMAIN_LAND)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_LAND;
	}
	else if (eDomain == DOMAIN_SEA)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_SEA;
		bNavalOnly = true;
	}

	CvAssertMsg(eTargetType != AI_TACTICAL_TARGET_NONE, "Undefined domain type for target");
	if (eTargetType == AI_TACTICAL_TARGET_NONE)
	{
		return;
	}

	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		if (FindUnitsWithinStrikingDistance(pPlot, 0, 0, false /* bNoRangedUnits */, bNavalOnly, true /*bMustMoveThrough*/, false /*bIncludeBlockedUnits*/, false /*bWillPillage*/))
		{
			// Queue best one up to capture it
			ExecutePlunderTradeUnit(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Plundering trade unit, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		pTarget = GetNextZoneTarget();
	}
}

/// Process units that we recruited out of operational moves.  Haven't used them, so let them go ahead with those moves
void CvTacticalAI::PlotOperationalArmyMoves()
{
	list<int>::iterator it;

	// Update all operations (moved down - previously was in the PlayerAI object)
	CvAIOperation* nextOp = NULL;
	nextOp = m_pPlayer->getFirstAIOperation();
	while(nextOp != NULL)
	{
		if(nextOp->GetLastTurnMoved() < GC.getGame().getGameTurn())
		{
			switch(nextOp->GetMoveType())
			{
			case AI_OPERATION_MOVETYPE_SINGLE_HEX:
				PlotSingleHexOperationMoves((CvAIEscortedOperation*)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_ENEMY_TERRITORY:
				PlotEnemyTerritoryOperationMoves((CvAIEnemyTerritoryOperation*)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_NAVAL_ESCORT:
				PlotNavalEscortOperationMoves((CvAINavalEscortedOperation*)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_FREEFORM_NAVAL:
				PlotFreeformNavalOperationMoves((CvAINavalOperation*)nextOp);
				break;
			}

			nextOp->SetLastTurnMoved(GC.getGame().getGameTurn());
			nextOp->CheckOnTarget();
		}


		nextOp = m_pPlayer->getNextAIOperation();
	}

	bool bKilledSomething;
	do
	{
		bKilledSomething = false;
		nextOp = m_pPlayer->getFirstAIOperation();
		while(nextOp != NULL)
		{
			if(nextOp->DoDelayedDeath())
			{
				bKilledSomething = true;
				break;
			}
			nextOp = m_pPlayer->getNextAIOperation();
		}
		// hack
	}
	while(bKilledSomething);
}

/// Assigns units to pillage enemy improvements
void CvTacticalAI::PlotPillageMoves(AITacticalTargetType eTarget, bool bFirstPass)
{
	int iPillageHeal = GC.getPILLAGE_HEAL_AMOUNT();

	CvString szTargetName = "";
	if(GC.getLogging() && GC.getAILogging())
	{
		if (eTarget == AI_TACTICAL_TARGET_CITADEL)
		{
			szTargetName = "Citadel";
		}
		else if (eTarget == AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE)
		{
			szTargetName = "Improved Resource";
		}
		else
		{
			szTargetName = "Improvement";
		}
	}

	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTarget);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		// try paratroopers first, not because they are more effective, just because it looks cooler...
		if (bFirstPass && FindParatroopersWithinStrikingDistance(pPlot))
		{
			// Queue best one up to capture it
			ExecuteParadropPillage(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Paratrooping in to pillage %s, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}

		}

		else if (bFirstPass && FindUnitsWithinStrikingDistance(pPlot, 0, iPillageHeal, false /* bNoRangedUnits */, false /*bNavalOnly*/, true /*bMustMoveThrough*/, false /*bIncludeBlockedUnits*/, true /*bWillPillage*/))
		{
			// Queue best one up to capture it
			ExecutePillage(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Pillaging %s, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		// No one can reach it this turn, what about next turn?
		else if(!bFirstPass && FindUnitsWithinStrikingDistance(pPlot, 2, iPillageHeal, false /* bNoRangedUnits */, false /*bNavalOnly*/, false /*bMustMoveThrough*/, false /*bIncludeBlockedUnits*/, true /*bWillPillage*/))
		{
			ExecuteMoveToTarget(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving toward %s for pillage, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		pTarget = GetNextZoneTarget();
	}
}

/// Assigns units to sit on trade routes it knows about and wait for enemy trade units
void CvTacticalAI::PlotPlunderTradePlotMoves (DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = AI_TACTICAL_TARGET_NONE;
	bool bNavalOnly = false;
	if (eDomain == DOMAIN_LAND)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT;
	}
	else if (eDomain == DOMAIN_SEA)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_SEA_PLOT;
		bNavalOnly = true;
	}

	CvAssertMsg(eTargetType != AI_TACTICAL_TARGET_NONE, "Undefined domain type for target");
	if (eTargetType == AI_TACTICAL_TARGET_NONE)
	{
		return;
	}

	for (uint ui = 0; ui < m_CurrentMoveUnits.size(); ui++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[ui].GetID());
		if (!pUnit)
		{
			continue;
		}

		if (pUnit->getDomainType() != eDomain)
		{
			continue;
		}

		int iClosestTarget = MAX_INT;
		CvPlot* pClosestPlot = NULL;
		CvTacticalTarget* pChosenTarget = NULL;
		CvTacticalTarget* pTarget = GetFirstZoneTarget(eTargetType);
		while(pTarget != NULL)
		{
			if (pTarget->GetAuxIntData() == 0)
			{
				int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY());
				if(iDistance < iClosestTarget)
				{
					pClosestPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					iClosestTarget = iDistance;
					pChosenTarget = pTarget;
				}
			}

			pTarget = GetNextZoneTarget();
		}

		if (pClosestPlot)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pClosestPlot->getX(), pClosestPlot->getY());
			pUnit->finishMoves();
			
			// Delete this unit from those we have to move
			UnitProcessed(m_CurrentMoveUnits[ui].GetID());

			// flag target as used
			pChosenTarget->SetAuxIntData(1);
		}
	}
}

/// Assigns units to plunder trade units it sees 
void CvTacticalAI::PlotPlunderTradeUnitMoves (DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = AI_TACTICAL_TARGET_NONE;
	bool bNavalOnly = false;
	if (eDomain == DOMAIN_LAND)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_LAND;
	}
	else if (eDomain == DOMAIN_SEA)
	{
		eTargetType = AI_TACTICAL_TARGET_TRADE_UNIT_SEA;
		bNavalOnly = true;
	}

	CvAssertMsg(eTargetType != AI_TACTICAL_TARGET_NONE, "Undefined domain type for target");
	if (eTargetType == AI_TACTICAL_TARGET_NONE)
	{
		return;
	}

	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		if (FindUnitsWithinStrikingDistance(pPlot, 0, 0, false /* bNoRangedUnits */, bNavalOnly, true /*bMustMoveThrough*/, false /*bIncludeBlockedUnits*/, false /*bWillPillage*/))
		{
			// Queue best one up to capture it
			ExecutePlunderTradeUnit(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Plundering trade unit, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		pTarget = GetNextZoneTarget();
	}
}

/// Move barbarian ships to disrupt usage of water improvements
void CvTacticalAI::PlotBlockadeImprovementMoves()
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsWithinStrikingDistance(pPlot, 2, 0, false /* bNoRangedUnits */, true /* bNavalOnly */, false /*bMustMoveThrough*/))
		{
			// Queue best one up to capture it
			ExecuteNavalBlockadeMove(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Blockading naval resource(s) with move to, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Assigns units to capture undefended civilians
void CvTacticalAI::PlotCivilianAttackMoves(AITacticalTargetType eTargetType)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false /* bNoRangedUnits */, false /*bNavalOnly*/, false /*bMustMoveThrough*/, false /*bIncludeBlockedUnits*/, false /*bWillPillage*/, true /*bTargetUndefended*/))
		{
			// Queue best one up to capture it
			ExecuteCivilianCapture(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				switch(eTargetType)
				{
				case AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking very high priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				case AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking high priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking medium priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				case AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN:
					strLogString.Format("Attacking low priority civilian, X: %d, Y: %d", pTarget->GetTargetX(),
					                    pTarget->GetTargetY());
					break;
				}
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Find all targets that we can bombard easily
void CvTacticalAI::PlotSafeBombardMoves()
{
	CvTacticalTarget* pTarget;

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot* pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}
}

/// Assigns units to heal
void CvTacticalAI::PlotHealMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Am I under 100% health and not embarked or already in a city?
			if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints() && !pUnit->isEmbarked() && !pUnit->plot()->isCity())
			{
				// If I'm a naval unit I need to be in friendly territory
				if(pUnit->getDomainType() != DOMAIN_SEA || pUnit->plot()->IsFriendlyTerritory(m_pPlayer->GetID()))
				{
					if (!pUnit->IsUnderEnemyRangedAttack())
					{
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Healing at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteHeals();
	}
}

/// Assigns a barbarian to go protect an undefended camp
void CvTacticalAI::PlotCampDefenseMoves()
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	while(pTarget != NULL)
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
#if defined(MOD_BALANCE_CORE_MILITARY)
		if(MOD_BALANCE_CORE_MILITARY && pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			if(pPlot->getNumUnits() > 0)
			{
				CvUnit* pUnit = pPlot->getUnitByIndex(0);
				if(pUnit->IsCanAttackRanged())
				{
					int iRange = pUnit->GetRange();
					if(iRange > 0)
					{
						for(int iX = -iRange; iX <= iRange; iX++)
						{
							for(int iY = -iRange; iY <= iRange; iY++)
							{
								CvPlot* pConsiderPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iRange);
								if(pConsiderPlot != NULL)
								{
									if(pConsiderPlot->getNumUnits() > 0)
									{
										if(pConsiderPlot->getUnitByIndex(0)->getOwner() != pUnit->getOwner())
										{
											if(pUnit->canEverRangeStrikeAt(pConsiderPlot->getX(), pConsiderPlot->getY()))
											{
												pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pConsiderPlot->getX(), pConsiderPlot->getY());
												UnitProcessed(pUnit->GetID());
												if(GC.getLogging() && GC.getAILogging())
												{
													CvString strLogString;
													strLogString.Format("Ranged unit attacking nearby enemy to protect camp, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
													LogTacticalMessage(strLogString);
												}
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
#endif
		if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, true /* bNoRangedUnits */, false /*bNavalOnly*/, false /*bMustMoveThrough*/))
		{
			ExecuteMoveToPlot(pPlot);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving to protect camp, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Make a defensive move to garrison a city
void CvTacticalAI::PlotGarrisonMoves(int iNumTurnsAway, bool bMustAllowRangedAttack)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
	while(pTarget != NULL)
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvCity* pCity = pPlot->getPlotCity();

		if(pCity && pCity->GetLastTurnGarrisonAssigned() < GC.getGame().getGameTurn())
		{
			// Grab units that make sense for this move type
			FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE], pPlot, iNumTurnsAway, bMustAllowRangedAttack);

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				ExecuteMoveToTarget(pPlot);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Garrison, X: %d, Y: %d, Priority: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iNumTurnsAway);
					if(bMustAllowRangedAttack)
					{
						strLogString += ", Allows bombard";
					}
					LogTacticalMessage(strLogString);
				}
				pCity->SetLastTurnGarrisonAssigned(GC.getGame().getGameTurn());
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Establish a defensive bastion adjacent to a city
void CvTacticalAI::PlotBastionMoves(int iNumTurnsAway)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
	while(pTarget != NULL)
	{
		// Grab units that make sense for this move type
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE], pPlot, iNumTurnsAway);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Bastion, X: %d, Y: %d, Priority: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Make a defensive move to guard an improvement
void CvTacticalAI::PlotGuardImprovementMoves(int iNumTurnsAway)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
	while(pTarget != NULL)
	{
		// Grab units that make sense for this move type
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE], pPlot, iNumTurnsAway);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Guard Improvement, X: %d, Y: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Pop goody huts nearby
void CvTacticalAI::PlotAncientRuinMoves(int iNumTurnsAway)
{
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_ANCIENT_RUINS);
	while(pTarget != NULL)
	{
		// Grab units that make sense for this move type
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS], pPlot, iNumTurnsAway);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving to goody hut, X: %d, Y: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
		pTarget = GetNextZoneTarget();
	}
}

/// Set fighters to intercept
void CvTacticalAI::PlotAirInterceptMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;
#if defined(MOD_AI_SMART_AIR_TACTICS)
	std::vector<CvPlot*> checkedPlotList;
#endif
	CvTacticalDominanceZone *pZone;

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Am I eligible to intercept?
			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit.pointer()))
			{
				CvPlot* pUnitPlot = pUnit->plot();
#if defined(MOD_AI_SMART_AIR_TACTICS)
				if (MOD_AI_SMART_AIR_TACTICS)
				{
					int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange()/*m_iRecruitRange*/, false/*bCountFighters*/, true/*bCountBombers*/);
					int iNumNearbyFighters = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange()/*m_iRecruitRange*/, true/*bCountFighters*/, false/*bCountBombers*/);
					int iNumPlotNumAlreadySet = SamePlotFound(checkedPlotList, pUnitPlot);

					if (iNumNearbyBombers == 1)
					{
						// To at least intercept once if only one bomber found.
						iNumNearbyBombers++;
					}
					int maxInterceptorsWanted = ((iNumNearbyBombers / 2) + (iNumNearbyFighters / 4));

					if (iNumPlotNumAlreadySet < maxInterceptorsWanted)
					{
						checkedPlotList.push_back(pUnitPlot);
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Ready to intercept enemy air units at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
				else
				{
#endif
					CvCity* pCity = pUnitPlot->getPlotCity();
					pZone = NULL;

					if (pCity)
					{
						pZone = m_pMap->GetZoneByCity(pCity, false);
					}
					int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, m_iRecruitRange, false/*bCountFighters*/, true/*bCountBombers*/);

					// On a carrier or in a city where we are not dominant and near some enemy bombers?
					if (!pCity || !pZone || pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
					{
						if (iNumNearbyBombers > 0)
						{
							unit.SetID(pUnit->GetID());
							m_CurrentMoveUnits.push_back(unit);

							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Ready to intercept enemy air units at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strLogString);
							}
						}
					}
#if defined(MOD_AI_SMART_AIR_TACTICS)
				}
#endif
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAirInterceptMoves();
	}
}

/// Set fighters to air sweep
void CvTacticalAI::PlotAirSweepMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;
	CvTacticalDominanceZone *pZone;

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && (pUnit->getDamage() * 2) < GC.getMAX_HIT_POINTS())
		{
			// Am I eligible to air sweep and have a target?
			if(pUnit->canAirSweep() && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit.pointer()) && m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit.pointer()) != NULL)
			{
				CvPlot* pUnitPlot = pUnit->plot();
				CvCity* pCity = pUnitPlot->getPlotCity();
				pZone = NULL;

				// On a carrier or in a city where we are dominant?
				if (pCity)
				{
					pZone = m_pMap->GetZoneByCity(pCity, false);
				}
				if (!pCity || !pZone || pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				{
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Ready to air sweep enemy air units at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strLogString);
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAirSweepMoves();
	}
}

/// Spend money to buy defenses
void CvTacticalAI::PlotEmergencyPurchases()
{
	CvCity* pCity;
	UnitHandle pCityDefender;
	CvUnit* pUnit;

	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	// Is this a dominance zone where we're defending a city?
	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	pCity = pZone->GetClosestCity();
	if(pCity && pCity->getOwner() == m_pPlayer->GetID() && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
	{
		// Make sure the city isn't about to fall.  Test by seeing if there are high priority unit targets
		for(unsigned int iI = 0; iI < m_ZoneTargets.size() && !pCity->isCapital(); iI++)
		{
			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				return;   // Abandon hope for this city; save our money to use elsewhere
			}
		}

		m_pPlayer->GetMilitaryAI()->BuyEmergencyBuilding(pCity);

		// If two defenders, assume already have land and sea and skip this city
		if (pCity->plot()->getNumDefenders(m_pPlayer->GetID()) < 2)
		{
			bool bBuyNavalUnit = false;
			bool bBuyLandUnit = false;

			pCityDefender = pCity->plot()->getBestDefender(m_pPlayer->GetID());
			if (!pCityDefender)
			{
				bBuyLandUnit = true;
				if (pCity->isCoastal())
				{
					bBuyNavalUnit = true;
				}
			}
			else
			{
				if (pCityDefender->getDomainType() == DOMAIN_LAND)
				{
					if (pCity->isCoastal())
					{
						bBuyNavalUnit = true;
					}
				}
				else
				{
					bBuyLandUnit = true;
				}
			}

			if (bBuyLandUnit)
			{
				pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_CITY_BOMBARD, pCity);
				if(!pUnit)
				{
					pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_RANGED, pCity);
				}
			}

			if (bBuyNavalUnit)
			{
				pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_ASSAULT_SEA, pCity);
				if (pUnit)
				{
					// Bought one, don't need to buy melee naval later
					bBuyNavalUnit = false;
				}
			}

			// Always can try to buy air units
			pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_ATTACK_AIR, pCity);
			if (!pUnit)
			{
				pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_DEFENSE_AIR, pCity);
			}

			// Melee naval if didn't buy Ranged naval, (or not)
			//if (bBuyNavalUnit)
			//{
			//	pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_ATTACK_SEA, pCity);
			//}
		}
	}
}

/// Spend money to buy defenses
void CvTacticalAI::PlotDefensiveAirlifts()
{
	vector<int> aProcessedUnits;
	list<int>::iterator it;
	CvTacticalUnit unit;
	CvCity* pCity;

	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	// Is this a dominance zone where we're defending a city and that city has an airport?
	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	pCity = pZone->GetClosestCity();
	if(pCity && pCity->getOwner() == m_pPlayer->GetID() && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && pCity->CanAirlift()&& pZone->GetEnemyUnitCount() > 0)
	{
		// Loop through all recruited units
		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);
			if(pUnit)
			{
				// If there a hex adjacent to city they can airlift to?
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot *pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));
					if (pLoopPlot != NULL)
					{
						if (pUnit->canAirliftAt(pUnit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
						{
							pUnit->airlift(pLoopPlot->getX(), pLoopPlot->getY());

							aProcessedUnits.push_back(pUnit->GetID());

							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Airlifting %s to city of %s, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pCity->getName().GetCString(), pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strLogString, false);
							}

							break;
						}
					}
				}
			}
		}
	}

	// Mark units processed
	vector<int>::const_iterator unitIt;
	for (unitIt = aProcessedUnits.begin(); unitIt != aProcessedUnits.end(); unitIt++)
	{
		UnitProcessed(*unitIt);
	}
}

/// Move naval units over top of unprotected embarked units
void CvTacticalAI::PlotEscortEmbarkedMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Am I a naval combat unit?
			if(pUnit->getDomainType() == DOMAIN_SEA && pUnit->IsCombatUnit())
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteEscortEmbarkedMoves();
	}
}

// PLOT MOVES FOR ZONE TACTICAL POSTURES

/// Win an attrition campaign with bombardments
void CvTacticalAI::PlotSitAndBombardMoves()
{
	// Find primary target for zone (i.e. enemy city)
	CvTacticalTarget target;
	CvPlot* pTarget;
	bool bAttackUnderway;
	bool bAttackMade;

	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	if(pZone->GetClosestCity() != NULL)
	{
		// Turn off moves we don't want to mix with this posture
		TurnOffMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_DAMAGE_CITY]);

		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
		target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
		target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());

		m_pMap->ClearDynamicFlags();
		pTarget = GC.getMap().plot(target.GetTargetX(), target.GetTargetY());
		m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		// Attack primary target (city)
		bAttackUnderway = ExecuteSafeBombards(target);
		if(target.IsTargetStillAlive(m_pPlayer->GetID()))
		{
			bAttackMade = PlotCaptureCityMoves();
			if(bAttackMade)
			{
				bAttackUnderway = true;
			}
			if(target.IsTargetStillAlive(m_pPlayer->GetID()))
			{
				bAttackMade = ExecuteProtectedBombards(target, bAttackUnderway);
				if(bAttackMade)
				{
					bAttackUnderway = true;
				}
				if(target.IsTargetStillAlive(m_pPlayer->GetID()))
				{
					PlotCaptureCityMoves();
				}
			}
		}

		// Attack ancillary target (nearby units)
		for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
		{
			// Is the target of an appropriate type?
			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{
					m_pMap->ClearDynamicFlags();
					pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
					m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

					bAttackUnderway = ExecuteSafeBombards(m_ZoneTargets[iI]);
					if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
					{
						ExecuteProtectedBombards(m_ZoneTargets[iI], bAttackUnderway);
					}
				}
			}
		}

		PlotCloseOnTarget(true /*bCheckDominance*/);
	}
}

/// Win an attrition campaign with bombardments
void CvTacticalAI::PlotAttritFromRangeMoves()
{
	CvPlot* pTarget;
	bool bAttackUnderway;

	// Find unit targets
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				bAttackUnderway = ExecuteSafeBombards(m_ZoneTargets[iI]);
				if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{
					ExecuteProtectedBombards(m_ZoneTargets[iI], bAttackUnderway);
				}
			}
		}
	}
}

/// Defeat enemy units by using our advantage in numbers
void CvTacticalAI::PlotExploitFlanksMoves()
{
	CvPlot* pTarget;
	bool bAttackUnderway = false;
	bool bAttackMade;

	m_TempTargets.clear();

	// Loop through unit targets finding attack for this turn
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			m_pMap->ClearDynamicFlags();
			pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			m_pMap->SetTargetFlankBonusCells(pTarget);

			bAttackMade = ExecuteFlankAttack(m_ZoneTargets[iI]);
			if(bAttackMade)
			{
				bAttackUnderway = true;
			}

			// No attack, see if we can set one up on this target later
			else
			{
				m_TempTargets.push_back(m_ZoneTargets[iI]);
			}
		}
	}
}

/// We have more overall strength than enemy, defeat his army first
void CvTacticalAI::PlotSteamrollMoves()
{
	m_TempTargets.clear();
	bool bAttackMade;

	// Loop through unit targets finding advantageous attacks for this turn
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}

	// See if there are any other anti-unit attacks we can make.
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (MOD_BALANCE_CORE_MILITARY) 
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	}
#else
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
#endif
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	// See if it is time to go after the city
	bAttackMade = PlotCaptureCityMoves();

	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, true);

	if(!bAttackMade)
	{
		PlotDamageCityMoves();
	}

	PlotCloseOnTarget(false /*bCheckDominance*/);
}

/// We should be strong enough to take out the city before the enemy can whittle us down with ranged attacks
void CvTacticalAI::PlotSurgicalCityStrikeMoves()
{
	CvTacticalTarget target;
	bool bAttackUnderway;

	CvTacticalDominanceZone* pZone = m_pMap->GetZone(m_iCurrentZoneIndex);
	if(pZone->GetClosestCity() != NULL)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
		target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
		target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());
		bAttackUnderway = PlotCaptureCityMoves();

		// Any unit targets adjacent to city?
		for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
		{
			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if(plotDistance(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY(), target.GetTargetX(), target.GetTargetY()) == 1)
				{
					if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
					{
						ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
					}
				}
			}
		}

		if(target.IsTargetStillAlive(m_pPlayer->GetID()))
		{
			PlotDamageCityMoves();
		}

		// Take any other really good attacks we've set up
#if defined(MOD_BALANCE_CORE_MILITARY)
		if (MOD_BALANCE_CORE_MILITARY) 
		{
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
		}
#else
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
#endif
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

		PlotCloseOnTarget(false /*bCheckDominance*/);
	}
}

/// Build a defensive shell around this city
void CvTacticalAI::PlotHedgehogMoves()
{
	// Attack priority unit targets
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (MOD_BALANCE_CORE_MILITARY) 
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	}
#else
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
#endif

	// But after best attacks are exhausted, go right to playing defense
	CvTacticalDominanceZone* pZone = m_pMap->GetZone(m_iCurrentZoneIndex);
	if(pZone->GetClosestCity() != NULL)
	{
		CvTacticalTarget target;
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
		target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
		target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());
		ExecuteHedgehogDefense(target, pZone);
	}
}

/// Try to push back the invader
void CvTacticalAI::PlotCounterattackMoves()
{
	// Attack priority unit targets
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);

	// Now low priority targets
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	// Now targets we can't destroy
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
}

/// Withdraw out of current dominance zone
void CvTacticalAI::PlotWithdrawMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	CvTacticalUnit unit;

	CvTacticalDominanceZone* pZone = m_pMap->GetZone(m_iCurrentZoneIndex);

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Proper domain of unit?
			if(pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{
				// Am I in the current dominance zone?
				int iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
				if(m_pMap->GetCell(iPlotIndex)->GetDominanceZone() == m_pMap->GetZone(m_iCurrentZoneIndex)->GetDominanceZoneID())
				{
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}

				// Or am I within tactical recruit range of center of zone and in danger and not in friendly territory?
				else if(pZone->GetClosestCity() != NULL)
				{
					if(plotDistance(pZone->GetClosestCity()->getX(), pZone->GetClosestCity()->getY(), pUnit->getX(), pUnit->getY()) <= m_iRecruitRange)
					{
						CvPlot* pPlot = pUnit->plot();
						if(m_pPlayer->GetPlotDanger(*pPlot) > 0)
						{
							if(pUnit->plot()->getOwner() != pUnit->getOwner())
							{
								unit.SetID(pUnit->GetID());
								m_CurrentMoveUnits.push_back(unit);
							}
						}
					}
				}

				// Or we are withdrawing from unowned lands zone and this is a plot in danger
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER)
				{
					CvPlot* pPlot = pUnit->plot();
					if(m_pPlayer->GetPlotDanger(*pPlot) > 0)
					{
						if(pUnit->plot()->getOwner() != pUnit->getOwner())
						{
							unit.SetID(pUnit->GetID());
							m_CurrentMoveUnits.push_back(unit);
						}
					}
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWithdrawMoves();
	}
}

/// Bombard any enemy targets we can get to on shore
void CvTacticalAI::PlotShoreBombardmentMoves()
{
	CvPlot* pTarget;

	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				ExecuteSafeBombards(m_ZoneTargets[iI]);
			}
		}
	}
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				ExecuteSafeBombards(m_ZoneTargets[iI]);
			}
		}
	}
}

/// Close units in on primary target of this dominance zone
void CvTacticalAI::PlotCloseOnTarget(bool bCheckDominance)
{
	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	if (bCheckDominance && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
	{
		return;
	}

	// Flank attacks done; if in an enemy zone, close in on target
	CvTacticalTarget target;
	if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		target.SetTargetPlayer(BARBARIAN_PLAYER);
		target.SetTargetX(pZone->GetTempZoneCenter()->getX());
		target.SetTargetY(pZone->GetTempZoneCenter()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());

		ExecuteCloseOnTarget(target, pZone);
	}
	else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY && pZone->GetClosestCity() != NULL)
	{
		bool bCanSeeCity = pZone->GetClosestCity()->plot()->isVisible(m_pPlayer->getTeam());

		// If we can't see the city, be careful advancing on it.  We want to be sure we're not heavily outnumbered
		if(!bCanSeeCity || pZone->GetFriendlyStrength() > (pZone->GetEnemyStrength() / 2))
		{
			target.SetTargetType(AI_TACTICAL_TARGET_CITY);
			target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
			target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
			target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
			target.SetDominanceZone(pZone->GetDominanceZoneID());

			ExecuteCloseOnTarget(target, pZone);
		}
	}
}

/// Log that we couldn't find assignments for some units
void CvTacticalAI::ReviewUnassignedUnits()
{
	list<int>::iterator it;

	// Loop through all remaining units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Barbarians and air units aren't handled by the operational or homeland AIs
			if(pUnit->isBarbarian() || pUnit->getDomainType() == DOMAIN_AIR)
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				pUnit->SetTurnProcessed(true);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Unassigned %s at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}
}

// OPERATIONAL AI SUPPORT FUNCTIONS

/// Move a single stack (civilian plus escort) to its destination
void CvTacticalAI::PlotSingleHexOperationMoves(CvAIEscortedOperation* pOperation)
{
	int iUnitID;
	UnitHandle pCivilian;
	UnitHandle pEscort;
	CvPlot* pCivilianMove;
	CvPlot* pEscortMove;
	bool bSaveMoves = false;

	// Simplification - assume only 1 army per operation now
	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());
	iUnitID = pThisArmy->GetFirstUnitID();
	if(iUnitID != -1)
	{
		pCivilian = m_pPlayer->getUnit(iUnitID);
	}

	if(!pCivilian)
	{
		return;
	}

#if defined(MOD_BALANCE_CORE)
	//may be null!
	pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
#endif
	// ESCORT AND CIVILIAN MEETING UP
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE ||
	        pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
#if !defined(MOD_BALANCE_CORE)
		pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
#endif
		if(!pEscort || pEscort->TurnProcessed())
		{
			// Escort died or was poached for other tactical action, operation will clean itself up when call CheckOnTarget()
			return;
		}

		// Check to make sure escort can get to civilian
		if(pEscort->GeneratePath(pCivilian->plot()))
		{
			// He can, so have civilian remain in place
			ExecuteMoveToPlot(pCivilian, pCivilian->plot());

			if(pThisArmy->GetNumSlotsFilled() > 1)
			{
				if(pEscort)
				{
					// Move escort over
					ExecuteMoveToPlot(pEscort, pCivilian->plot());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strTemp;
						CvString strLogString;
						strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
						strLogString.Format("Moving escorting %s to civilian for operation, Civilian X: %d, Civilian Y: %d, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY(), pEscort->getX(), pEscort->getY());
						LogTacticalMessage(strLogString);
					}
				}
			}
		}

		// Find a new place to meet up, look at all hexes adjacent to civilian
		else
		{
			for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pPlot = plotDirection(pCivilian->getX(), pCivilian->getY(), ((DirectionTypes)iI));
				if(pPlot != NULL)
				{
					// Has to be somewhere we can move and be empty of other units
					if(pEscort->canEnterTerrain(*pPlot) && pEscort->canEnterTerritory(pPlot->getTeam()))
					{
						if(pPlot->getNumUnits() == 0)
						{
							if(pEscort->GeneratePath(pPlot) && pCivilian->GeneratePath(pPlot))
							{
								ExecuteMoveToPlot(pEscort, pPlot);
								ExecuteMoveToPlot(pCivilian, pPlot);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strTemp;
									CvString strLogString;
									strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
									strLogString.Format("Moving escorting %s to open hex, Open hex X: %d, Open hex Y: %d, X: %d, Y: %d", strTemp.GetCString(), pPlot->getX(), pPlot->getY(), pEscort->getX(), pEscort->getY());
									LogTacticalMessage(strLogString);
									strTemp = GC.getUnitInfo(pCivilian->getUnitType())->GetDescription();
									strLogString.Format("Moving %s to open hex, Open hex X: %d, Open hex Y: %d, X: %d, Y: %d", strTemp.GetCString(), pPlot->getX(), pPlot->getY(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
								return;
							}
						}
					}
				}
			}

			// Didn't find an alternative, must abort operation
			pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
			pCivilian->finishMoves();
			pEscort->finishMoves();
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strTemp;
				CvString strLogString;
				strLogString.Format("Retargeting civilian escort operation. No empty tile adjacent to civilian to meet.");
				LogTacticalMessage(strLogString);
			}
		}
	}

	// MOVING TO TARGET
	else
	{
		// If we're not there yet, we have work to do (otherwise CheckOnTarget() will finish operation for us)
		if(pCivilian->plot() != pOperation->GetTargetPlot())
		{
#if defined(MOD_BALANCE_CORE)
			//todo: make sure to stay away from danger
			bool bDanger = pCivilian->SentryAlert(true);
			if (bDanger)
			{
				CvString msg = CvString::format("%s settler in danger at (%d,%d)", GET_PLAYER(pCivilian->getOwner()).getCivilizationAdjective(), pCivilian->getX(), pCivilian->getY() );
				if (pEscort && pEscort->plot()!=pCivilian->plot())
					msg += CvString::format(", escort at (%d,%d)", pEscort->getX(), pEscort->getY() );
				msg += "\n";
				OutputDebugString( msg );
#if defined(MOD_BALANCE_CORE_SETTLER)
				if (!pEscort || pEscort->plot()!=pCivilian->plot())
					m_pPlayer->GetHomelandAI()->MoveCivilianToSafety(pCivilian.pointer());
#endif
			}
#endif
			// Look at where we'd move this turn taking units into consideration
			int iFlags = 0;
			if(pThisArmy->GetNumSlotsFilled() > 1)
			{
				iFlags = MOVE_UNITS_IGNORE_DANGER;
			}

			// Handle case of no path found at all for civilian
			if(!pCivilian->GeneratePath(pOperation->GetTargetPlot(), iFlags, false /*bReuse*/))
			{
				pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
				pCivilian->finishMoves();
#if !defined(MOD_BALANCE_CORE)
				pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
#endif
				if(pEscort)
				{
					pEscort->finishMoves();
				}
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
					LogTacticalMessage(strLogString);
				}
			}

			else
			{
				pCivilianMove = pCivilian->GetPathEndTurnPlot();
				bSaveMoves = (pCivilianMove == pOperation->GetTargetPlot());
				if(pThisArmy->GetNumSlotsFilled() == 1)
				{
					ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						CvString strTemp;
						strTemp = pCivilian->getUnitInfo().GetDescription();
						strLogString.Format("Moving %s without escort to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY());
						LogTacticalMessage(strLogString);
					}
				}

				else
				{
#if !defined(MOD_BALANCE_CORE)
					pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
#endif
					// See if escort can move to the same location in one turn
					if(TurnsToReachTarget(pEscort, pCivilianMove) <= 1)
					{
						ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
						ExecuteMoveToPlot(pEscort, pCivilianMove);
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							CvString strTemp;
							strTemp = pCivilian->getUnitInfo().GetDescription();
							strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
							LogTacticalMessage(strLogString);
							strTemp = pEscort->getUnitInfo().GetDescription();
							strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
							LogTacticalMessage(strLogString);
						}
					}

					else
					{
						CvTacticalAnalysisCell* pCell = GC.getGame().GetTacticalAnalysisMap()->GetCell(pCivilianMove->GetPlotIndex());
						UnitHandle pBlockingUnit = pCivilianMove->getBestDefender(m_pPlayer->GetID());

						// See if friendly blocking unit is ending the turn there, or if no blocking unit (which indicates this is somewhere civilian
						// can move that escort can't -- like minor civ territory), then find a new path based on moving the escort
						if(pCell->IsFriendlyTurnEndTile() || !pBlockingUnit)
						{
							if(!pEscort->GeneratePath(pOperation->GetTargetPlot(), 0, false /*bReuse*/))
							{
								pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
								pCivilian->finishMoves();
								pEscort->finishMoves();
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
									LogTacticalMessage(strLogString);
								}
							}
							else
							{
								pEscortMove = pCivilian->GetPathEndTurnPlot();
								bSaveMoves = (pEscortMove == pOperation->GetTargetPlot());

								// See if civilian can move to the same location in one turn
								if(TurnsToReachTarget(pCivilian, pEscortMove) <= 1)
								{
									ExecuteMoveToPlot(pEscort, pEscortMove);
									ExecuteMoveToPlot(pCivilian, pEscortMove, bSaveMoves);
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										CvString strTemp;
										strTemp = pEscort->getUnitInfo().GetDescription();
										strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
										LogTacticalMessage(strLogString);
										strTemp = pCivilian->getUnitInfo().GetDescription();
										strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
										LogTacticalMessage(strLogString);
									}
								}
								else
								{
									// Didn't find an alternative, retarget operation
									pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
									pCivilian->finishMoves();
									pEscort->finishMoves();
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Retargeting civilian escort operation. Too many blocking units.");
										LogTacticalMessage(strLogString);
									}
								}
							}
						}
						// Looks like we should be able to move the blocking unit out of the way
						else
						{
							if(ExecuteMoveOfBlockingUnit(pBlockingUnit))
							{
								ExecuteMoveToPlot(pEscort, pCivilianMove);
								ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									CvString strTemp;
									strTemp = pEscort->getUnitInfo().GetDescription();
									strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
									LogTacticalMessage(strLogString);
									strTemp = pCivilian->getUnitInfo().GetDescription();
									strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
							}
							else
							{

								// Didn't find an alternative, try retargeting operation
								pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
								pCivilian->finishMoves();
								pEscort->finishMoves();
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Retargeting civilian escort operation. Could not move blocking unit.");
									LogTacticalMessage(strLogString);
								}
							}
						}
					}
				}
			}
		}
	}
}

/// Move a large army to its destination against an enemy target
void CvTacticalAI::PlotEnemyTerritoryOperationMoves(CvAIEnemyTerritoryOperation* pOperation)
{
	// Simplification - assume only 1 army per operation now
	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());

	m_OperationUnits.clear();
	m_GeneralsToMove.clear();
	m_ParatroopersToMove.clear();
	pThisArmy->UpdateCheckpointTurns();

	// RECRUITING
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		// If no estimate for when recruiting will end, let the rest of the AI use these units
		if(pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{
					// See if we are just able to get to muster point in time.  If so, time for us to head over there
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						// Great general?
						if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
						{
							if(pUnit->getMoves() > 0)
							{
								CvOperationUnit operationUnit;
								operationUnit.SetUnitID(pUnit->GetID());
								operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
								m_GeneralsToMove.push_back(operationUnit);
							}
						}

						else
						{
							CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
							if(pkMultiUnitFormationInfo)
							{
								const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);

								// Continue moving to target
								if(pSlot->HasStartedOnOperation())
								{
									MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
								}

								else
								{
									// See if we are just able to get to muster point in time.  If so, time for us to head over there
									int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true /*bReusePaths*/, true, true);
									if(iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
									{
										pSlot->SetStartedOnOperation(true);
										MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
									}
								}
							}
						}
					}
				}
			}
			ExecuteGatherMoves(pThisArmy);
		}
	}

	// GATHERING FORCES
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		ClearEnemiesNearArmy(pThisArmy);

		// Request moves for all units
		for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
			if(pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if(pUnit && !pUnit->TurnProcessed())
				{
					// Great general or admiral?
					if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
					{
						if(pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
							m_GeneralsToMove.push_back(operationUnit);
						}
					}

					else
					{
						pSlot->SetStartedOnOperation(true);
						CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if(pkMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);
							MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
						}
					}
				}
			}
		}
			ExecuteGatherMoves(pThisArmy);
	}

	// MOVING TO TARGET
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		// Update army's current location
		CvPlot* pThisTurnTarget;
		CvPlot* pClosestCurrentCOMonPath = NULL;
		pThisTurnTarget = pOperation->ComputeCenterOfMassForTurn(pThisArmy, &pClosestCurrentCOMonPath);
		if(pThisTurnTarget == NULL)
		{
			pOperation->SetToAbort(AI_ABORT_LOST_PATH);
			return;
		}

		pThisArmy->SetXY(pThisTurnTarget->getX(), pThisTurnTarget->getY());
		ClearEnemiesNearArmy(pThisArmy);

		// Request moves for all units
		for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
			if(pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if(pUnit && !pUnit->TurnProcessed())
				{
					// Great general?
					if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
					{
						if(pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
							m_GeneralsToMove.push_back(operationUnit);
						}
					}
					else
					{
						CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if(pkMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);
							MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
						}
					}
				}
			}
		}
		ExecuteFormationMoves(pThisArmy, pClosestCurrentCOMonPath);
	}

	if(m_ParatroopersToMove.size() > 0)
	{
		//MoveParatroopers(pThisArmy);
	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral(pThisArmy);
	}
}

/// Move a large army to its destination against an enemy target
void CvTacticalAI::PlotNavalEscortOperationMoves(CvAINavalEscortedOperation* pOperation)
{
	CvUnit* pCivilian = 0, *pEscort = 0;
	int iUnitID;

	// Simplification - assume only 1 army per operation now
	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());
	iUnitID = pThisArmy->GetFirstUnitID();
	if(iUnitID != -1)
	{
		pCivilian = m_pPlayer->getUnit(iUnitID);

		iUnitID = pThisArmy->GetNextUnitID();
		if(iUnitID != -1)
		{
			pEscort = m_pPlayer->getUnit(iUnitID);
			if(pEscort->getDomainType() != DOMAIN_LAND)
			{
				pEscort = NULL;   // Second unit wasn't the escort
			}
		}
	}

	if(!pCivilian && pOperation->IsCivilianRequired())
	{
		return;
	}

	m_OperationUnits.clear();
	pThisArmy->UpdateCheckpointTurns();

	// RECRUITING
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		// If no estimate for when recruiting will end, let the rest of the AI use these units
		if(pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{
					// See if we are just able to get to muster point in time.  If so, time for us to head over there
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						CvMultiUnitFormationInfo* pkUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if (pkUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkUnitFormationInfo->getFormationSlotEntry(iI);

							// Continue moving to target
							if(pSlot->HasStartedOnOperation())
							{
								MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
							}

							else
							{
								// See if we are just able to get to muster point in time.  If so, time for us to head over there
								int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true /*bReusePaths*/, true, true);
								if(iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
								{
									pSlot->SetStartedOnOperation(true);
									MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
								}
							}
						}
					}
				}
			}
			ExecuteNavalFormationMoves(pThisArmy, pOperation->GetMusterPlot());
		}
	}

	// GATHERING FORCES
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		ExecuteFleetMoveToTarget(pThisArmy, pOperation->GetMusterPlot());
	}

	// MOVING TO TARGET with a civilian
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION && pOperation->IsCivilianRequired())
	{
		// Update army's current location
		CvPlot* pThisTurnTarget;
		pThisTurnTarget = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
		if(pThisTurnTarget != NULL)
		{
			pThisArmy->SetXY(pThisTurnTarget->getX(), pThisTurnTarget->getY());
		}

		// Settler at the target?
		if(!pCivilian->plot()->isWater() && pCivilian->plot() == pThisArmy->GetGoalPlot())
		{
			if(pCivilian->canFound(pCivilian->plot()))
			{
				pCivilian->PushMission(CvTypes::getMISSION_FOUND());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("City founded, At X=%d, At Y=%d", pCivilian->plot()->getX(), pCivilian->plot()->getY());
					pOperation->LogOperationSpecialMessage(strMsg);
				}
				pOperation->SetToAbort(AI_ABORT_SUCCESS);
			}
		}

		// Is our settler and his escort both one move from the destination?  If so, targeted moves to finish up
		else if(TurnsToReachTarget(pCivilian, pOperation->GetTargetPlot()) <= 1 &&
		        (!pEscort || TurnsToReachTarget(pEscort, pOperation->GetTargetPlot()) <= 1))
		{
			ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(), true);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strTemp;
				CvString strLogString;
				strTemp = GC.getUnitInfo(pCivilian->getUnitType())->GetDescription();
				strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
				LogTacticalMessage(strLogString);
			}

			if(pEscort)
			{
				ExecuteMoveToPlot(pEscort, pOperation->GetTargetPlot(), false);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp;
					CvString strLogString;
					strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
					strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
					LogTacticalMessage(strLogString);
				}
			}

			// Move naval escorts in close
			for(int iI = 2; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						MoveToEmptySpaceNearTarget(pUnit, pOperation->GetTargetPlot(), false /*bLand */);
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strTemp;
							CvString strLogString;
							strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
							strLogString.Format("Moving %s near target, Now at X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}

		// Still further away, move as formation
		else
		{
			int iBestDistance = MAX_INT;
			CvPlot* pBestPlot = NULL;

			// Request moves for all units
			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						const CvFormationSlotEntry& thisSlotEntry = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex())->getFormationSlotEntry(iI);
						MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
					}
				}
			}

			// Plot one turn of movement for the first unit in the army we find out in the water
			UnitHandle pUnitAtSea = pThisArmy->GetFirstUnitInDomain(DOMAIN_SEA);
			if(!pUnitAtSea)
			{
				pOperation->RetargetCivilian(pCivilian, pThisArmy);
				for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
					if(pSlot->GetUnitID() != NO_UNIT)
					{
						UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
						if(pUnit)
						{
							pUnit->finishMoves();
						}
					}
				}
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Retargeting naval escort operation (no unit in water!), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
					LogTacticalMessage(strLogString);
				}
			}
			else
			{
				int iFlags;
				if(pThisArmy->GetUnitsOfType((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_NAVAL_ESCORT]) > 0)
				{
					iFlags = MOVE_UNITS_IGNORE_DANGER;
				}

				// Goal should be a water tile one hex shy of our target
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot* pAdjacentPlot = plotDirection(pThisArmy->GetGoalX(), pThisArmy->GetGoalY(), ((DirectionTypes)iI));
					if(pAdjacentPlot != NULL && pAdjacentPlot->isWater())
					{
						if(pUnitAtSea->plot() == pAdjacentPlot)
						{
							pBestPlot = pAdjacentPlot;
							break;
						}
						else
						{
							// Using step finder could get tripped up by ocean hexes (since they are in the area but not valid movement targets for coastal vessels.  Watch this!
							int iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_pPlayer->GetID(), pOperation->GetEnemy(), pUnitAtSea->plot(), pAdjacentPlot);
							if(iDistance > 0 && iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								int iDistanceToMove = min(4, iDistance);
								PlayerTypes eEnemy = pOperation->GetEnemy();
								pBestPlot = GC.getStepFinder().GetXPlotsFromEnd(m_pPlayer->GetID(), eEnemy, pUnitAtSea->plot(), pAdjacentPlot, (iDistance - iDistanceToMove), false);
							}
						}
					}
				}

				if(pBestPlot == NULL)
				{
					pOperation->RetargetCivilian(pCivilian, pThisArmy);
					for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
					{
						CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
						if(pSlot->GetUnitID() != NO_UNIT)
						{
							UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
							if(pUnit)
							{
								pUnit->finishMoves();
							}
						}
					}
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Retargeting naval escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
						LogTacticalMessage(strLogString);
					}
				}
				else
				{
					ExecuteNavalFormationMoves(pThisArmy, pBestPlot);
				}
			}
		}
	}

	// MOVING TO TARGET as an army
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION && !pOperation->IsCivilianRequired())
	{
		int iBestDistance = MAX_INT;
		int iDistance;
		CvPlot *pBestPlot = NULL;
		int iSlowestMovementRate = MAX_INT;
		UnitHandle pClosestUnitAtSea;
		pBestPlot = m_pPlayer->GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pThisArmy->GetGoalPlot(), pThisArmy);

		// Error handling: couldn't find path to plot next to target
		if (pBestPlot == NULL)
		{
			pOperation->RetargetCivilian(pCivilian, pThisArmy);
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit) 
					{
						pUnit->finishMoves();
					}
				}
			}
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Retargeting naval escort operation (path lost to target), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
				LogTacticalMessage(strLogString);
			}
		}

		else
		{
			// Request moves for all units, getting the slowest movement rate and the closest unit
			iBestDistance = MAX_INT;
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit && !pUnit->TurnProcessed())
					{
						CvMultiUnitFormationInfo* pMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if (pMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pMultiUnitFormationInfo->getFormationSlotEntry(iI);
							MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);

							int iMoves = pUnit->getMoves() / GC.getMOVE_DENOMINATOR();
							if (iMoves < iSlowestMovementRate)
							{
								iSlowestMovementRate = iMoves;
							}

							// At sea?
							iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_pPlayer->GetID(), pOperation->GetEnemy(), pUnit->plot(), pBestPlot);
							if (iDistance > 0 && iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								pClosestUnitAtSea = pUnit;
							}
						}
					}
				}
			}	

			// Error handling: no one at sea, retarget
			if (!pClosestUnitAtSea)
			{
				pOperation->RetargetCivilian(pCivilian, pThisArmy);
				for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
					if (pSlot->GetUnitID() != NO_UNIT)
					{
						UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
						if (pUnit) 
						{
							pUnit->finishMoves();
						}
					}
				}
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Retargeting naval escort operation (no unit in water!), X: %d, Y: %d", pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY());
					LogTacticalMessage(strLogString);
				}
			}
			else
			{
				// If not close yet, find best plot for this turn's movement along path to ultimate best plot
				if (iBestDistance > iSlowestMovementRate)
				{
					pBestPlot = GC.getStepFinder().GetXPlotsFromEnd(m_pPlayer->GetID(), pOperation->GetEnemy(), pClosestUnitAtSea->plot(), pBestPlot, (iBestDistance - iSlowestMovementRate), true);					
				}
				if (pBestPlot)
				{
					pThisArmy->SetXY(pBestPlot->getX(), pBestPlot->getY());
					ExecuteNavalFormationMoves(pThisArmy, pBestPlot);
				}
				else
				{
					CvAssertMsg (false, "Naval operation logic error.");
				}
			}
		}
	}
}

/// Move a naval force that is roaming for targets
void CvTacticalAI::PlotFreeformNavalOperationMoves(CvAINavalOperation* pOperation)
{
	// Simplification - assume only 1 army per operation now
	if(pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());

	m_OperationUnits.clear();
	pThisArmy->UpdateCheckpointTurns();

	// RECRUITING
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		// If no estimate for when recruiting will end, let the rest of the AI use these units
		if(pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
				if(pSlot->GetUnitID() != NO_UNIT)
				{
					// See if we are just able to get to muster point in time.  If so, time for us to head over there
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if(pUnit && !pUnit->TurnProcessed())
					{
						CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if(pkMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);

							// Continue moving to target
							if(pSlot->HasStartedOnOperation())
							{
								MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
							}
							else
							{
								// See if we are just able to get to muster point in time.  If so, time for us to head over there
								int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true /*bReusePaths*/, true, true);
								if(iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
								{
									pSlot->SetStartedOnOperation(true);
									MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
								}
							}
						}
					}
				}
			}
			ExecuteNavalFormationMoves(pThisArmy, pOperation->GetMusterPlot());
		}
	}

	// GATHERING FORCES
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		// Get them moving to target without delay
		pOperation->ArmyInPosition(pThisArmy);
		ExecuteFleetMoveToTarget(pThisArmy, pOperation->GetTargetPlot());
	}

	// MOVING TO TARGET
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		// Get them moving to target without delay
		pOperation->ArmyInPosition(pThisArmy);
		ExecuteFleetMoveToTarget(pThisArmy, pOperation->GetTargetPlot());
	}
}

/// Queues up attacks on enemy units on or adjacent to army's desired center
void CvTacticalAI::ClearEnemiesNearArmy(CvArmyAI* pArmy)
{
	bool bEnemyNear = false;
	CvPlot* pPlot;
	int iRange = 1;
	bool bAttackUnderway;
	bool bAttackMade = false;
	UnitHandle pUnit;

	// Loop through all appropriate targets to see if any is of concern
	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT
#if defined(MOD_BALANCE_CORE_MILITARY)
				|| m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY
#endif
				)
		{
			// Is this unit near enough?
			if(plotDistance(pArmy->GetX(), pArmy->GetY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY()) <= iRange)
			{
				bEnemyNear = true;
				break;
			}
		}
	}

	if(bEnemyNear)
	{
		// Add units from army to tactical AI for this turn
		pUnit = pArmy->GetFirstUnit();
		while(pUnit)
		{
			if(!pUnit->TurnProcessed() && !pUnit->isDelayedDeath() && pUnit->canMove())
			{
				if(std::find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), pUnit->GetID()) == m_CurrentTurnUnits.end())
				{
					m_CurrentTurnUnits.push_back(pUnit->GetID());
				}
			}
			pUnit = pArmy->GetNextUnit();
		}

		// Now attack these targets
		for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
		{
			// Is the target of an appropriate type?
			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT
#if defined(MOD_BALANCE_CORE_MILITARY)
					|| m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY
#endif					
					)
			{
				if(m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{
					// Is this unit near enough?
					if(plotDistance(pArmy->GetX(), pArmy->GetY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY()) <= iRange)
					{
						pPlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());

						m_pMap->ClearDynamicFlags();
						m_pMap->SetTargetBombardCells(pPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

						bAttackUnderway = ExecuteSafeBombards(m_AllTargets[iI]);
						if(m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
						{
							bAttackMade = ExecuteProtectedBombards(m_AllTargets[iI], bAttackUnderway);
						}
						if(bAttackMade)
						{
							bAttackUnderway = true;
						}
						if(m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
						{
							UnitHandle pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
							if(pDefender)
							{
								m_AllTargets[iI].SetAuxIntData(pDefender->GetCurrHitPoints());
								m_CurrentMoveCities.clear();
#if defined(MOD_BALANCE_CORE)
								if(FindUnitsWithinStrikingDistance(pPlot, 1, 0))
#else
								if(FindUnitsWithinStrikingDistance(pPlot, 1, 0, false /* bNoRangedUnits */))
#endif
								{
									ComputeTotalExpectedDamage(&m_AllTargets[iI], pPlot);
									ExecuteAttack(&m_AllTargets[iI], pPlot, true /*bInflictWhatWeTake*/, true /*bMustSurviveAttack*/);
								}
							}
						}
					}
				}
			}
		}
	}
}

/// Store off a new unit that needs to move as part of an operational AI formation
void CvTacticalAI::MoveWithFormation(UnitHandle pUnit, MultiunitPositionTypes ePosition)
{
	if(pUnit->getMoves() > 0)
	{
		CvOperationUnit operationUnit;
		operationUnit.SetUnitID(pUnit->GetID());
		operationUnit.SetPosition(ePosition);
		m_OperationUnits.push_back(operationUnit);
	}
}

/// Gather all units requested through calls to MoveWithFormation() to army's location
void CvTacticalAI::ExecuteGatherMoves(CvArmyAI* pArmy)
{
	CvPlot* pTarget;
	UnitHandle pUnit;
	int iUnits;
	AITacticalTargetType eTargetType;
	CvPlot* pLoopPlot;

	if(m_OperationUnits.size() == 0)
	{
		return;
	}

	pTarget = pArmy->Plot();

	// Gathering - treat everyone as a melee unit; don't need ranged in the rear yet
	iUnits = m_OperationUnits.size();

	// Range around target based on number of units we need to place
	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iUnits);

	// Try one time with computed range
	bool bFoundEnoughDeploymentPlots = false;
	if (ScoreDeploymentPlots(pTarget, pArmy, iUnits, 0, iRange))
	{
		// Did we get twice as many possible plots as units?
		if (m_TempTargets.size() >= (unsigned)(iUnits * 2))
		{
			bFoundEnoughDeploymentPlots = true;
		}
		else
		{
			m_TempTargets.clear();
			iRange = 3;
		}
	}

	if (!bFoundEnoughDeploymentPlots)
	{
		if (!ScoreDeploymentPlots(pTarget, pArmy, iUnits, 0, iRange))
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy near target", pArmy->GetID());
				LogTacticalMessage(strLogString);
			}
			m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
			return;
		}
	}

	// Compute the moves to get the best deployment
	std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
	m_PotentialBlocks.clear();
	bool bDone = false;

	int iUnitsToPlace = iUnits;

	for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
	{
		eTargetType = m_TempTargets[iI].GetTargetType();

		pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
		
		// Don't use if there's already a unit not in the army here
		UnitHandle pUnitAlreadyThere = pLoopPlot->getBestDefender(NO_PLAYER);
		if (!pUnitAlreadyThere || pUnitAlreadyThere->getArmyID() == pArmy->GetID())
		{
			if(FindClosestOperationUnit(pLoopPlot, true /*bSafeForRanged*/, false /*bMustBeRangedUnit*/))
			{
				for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
				{
					CvBlockingUnit block;
					block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
					block.SetPlot(pLoopPlot);
					block.SetNumChoices((int)m_CurrentMoveUnits.size());
					block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
					m_PotentialBlocks.push_back(block);
				}
				iUnitsToPlace--;
				if(iUnitsToPlace == 0)
				{
					bDone = true;
				}	
			}
		}
	}

	// Now ready to make the assignments
	AssignDeployingUnits(iUnits - iUnitsToPlace);

	PerformChosenMoves();

	// Log if someone in army didn't get a move assigned (how do we address this in the future?)
	if(m_ChosenBlocks.size() < (unsigned int)(iUnits))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("No gather move for %d units", iUnits - m_ChosenBlocks.size());
			LogTacticalMessage(strMsg);
		}
	}
}

/// Complete moves for all units requested through calls to MoveWithFormation()
void CvTacticalAI::ExecuteFormationMoves(CvArmyAI* pArmy, CvPlot *pClosestCurrentCOMonPath)
{
	CvPlot* pTarget;
	UnitHandle pUnit;
	AITacticalTargetType eTargetType;
	CvPlot* pLoopPlot;
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;

	if(m_OperationUnits.size() == 0)
	{
		return;
	}

	pTarget = pArmy->Plot();

	int iMeleeUnits = 0;
	int iRangedUnits = 0;
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		CvUnit *pOpUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (pOpUnit->IsCanAttackRanged())
		{
			iRangedUnits++;
		}
		else
		{
			iMeleeUnits++;
		}
	}

	// See if we have enough places to put everyone
	if(!ScoreFormationPlots(pArmy, pTarget, pClosestCurrentCOMonPath, iMeleeUnits + iRangedUnits))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy along formation's path", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
	}

	// Compute the moves to get the best deployment
	else
	{
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());

		// First loop for melee units who should be out front
		int iMeleeUnitsToPlace = iMeleeUnits;
		bool bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

			// Don't use if there's already someone here
			if (!pLoopPlot->getBestDefender(NO_PLAYER))
			{
				if(FindClosestOperationUnit(pLoopPlot, false /*bSafeForRanged*/, false /*bMustBeRangedUnit*/))
				{
					UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
					bool bMoveWasSafe;
					MoveToUsingSafeEmbark(pInnerUnit, pLoopPlot, bMoveWasSafe);
					pInnerUnit->finishMoves();
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying melee unit, %s, To X: %d, To Y: %d, At X: %d, At Y: %d", pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
						LogTacticalMessage(strMsg);
					}
					iMeleeUnitsToPlace--;
				}
			}
			if (iMeleeUnitsToPlace == 0)
			{
				bDone = true;
			}
		}

		// Log if someone in army didn't get a move assigned
		if (iMeleeUnitsToPlace > 0)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("No army deployment move for %d melee units", iMeleeUnitsToPlace);
				LogTacticalMessage(strMsg);
			}
		}

		// Second loop for ranged units
		int iRangedUnitsToPlace = iRangedUnits;
		bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				// Don't use if there's already someone here
				if (!pLoopPlot->getBestDefender(NO_PLAYER))
				{
					if(FindClosestOperationUnit(pLoopPlot, true /*bSafeForRanged*/, true /*bMustBeRangedUnit*/))
					{
						UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
						bool bMoveWasSafe;
						MoveToUsingSafeEmbark(pInnerUnit, pLoopPlot, bMoveWasSafe);
						pInnerUnit->finishMoves();
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying ranged unit, %s, To X: %d, To Y: %d, At X: %d, At Y: %d", pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
							LogTacticalMessage(strMsg);
						}
						iRangedUnitsToPlace--;
					}
				}
			}
			if (iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}

		// Third loop for ranged units we couldn't put in an ideal spot
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

			// Don't use if there's already someone here
			if (!pLoopPlot->getBestDefender(NO_PLAYER))
			{
				if(FindClosestOperationUnit(pLoopPlot, true /*bSafeForRanged*/, true /*bMustBeRangedUnit*/))
				{
					UnitHandle pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
					bool bMoveWasSafe;
					MoveToUsingSafeEmbark(pInnerUnit, pLoopPlot, bMoveWasSafe);
					pInnerUnit->finishMoves();
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying ranged unit (Pass 2), %s, To X: %d, To Y: %d, At X: %d, At Y: %d", pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
						LogTacticalMessage(strMsg);
					}
					iRangedUnitsToPlace--;
				}
			}
			if (iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}

		// Log if someone in army didn't get a move assigned
		if (iRangedUnitsToPlace > 0)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("No army deployment move for %d ranged units", iRangedUnitsToPlace);
				LogTacticalMessage(strMsg);
			}
		}
	}
}

/// Pick best hexes for deploying our army (based on safety, terrain, and keeping a tight formation). Returns false if insufficient free plots.
bool CvTacticalAI::ScoreDeploymentPlots(CvPlot* pTarget, CvArmyAI* pArmy, int iNumMeleeUnits, int iNumRangedUnits, int iRange)
{
	int iDX, iDY;
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	bool bSafeForDeployment;
	bool bForcedToUseWater;
	int iNumSafePlotsFound = 0;
	int iNumDeployPlotsFound = 0;
	CvTacticalTarget target;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pPlot = plotXY(pTarget->getX(), pTarget->getY(), iDX, iDY);
			if(pPlot != NULL)
			{
				bSafeForDeployment = true;
				bForcedToUseWater = false;

				int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
				if(iPlotDistance <= iRange)
				{
					int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());

					bool bValid = false;
					if(pOperation->IsMixedLandNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true /*bWater*/))
					{
						bValid = true;
					}

					else if(pOperation->IsAllNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true /*bWater*/))
					{
						bValid = true;
					}

					else if((!pOperation->IsAllNavalOperation() && !pOperation->IsMixedLandNavalOperation()) && (pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/) || GC.getMap().GetAIMapHint() & 1))
					{
						bValid = true;
						if (pCell->IsWater())
						{
							bForcedToUseWater = true;
						}
					}

					if(pOperation->IsMixedLandNavalOperation() || pOperation->IsAllNavalOperation())
					{
						if(!pArmy->IsAllOceanGoing() && pCell->IsOcean())
						{
							bValid = false;
						}
					}

					if(bValid)
					{
						// Skip this plot if friendly unit that isn't in this army
						CvUnit* pFriendlyUnit;
						pFriendlyUnit = pCell->GetFriendlyMilitaryUnit();
						if(pFriendlyUnit != NULL)
						{
							if(pFriendlyUnit->getArmyID() != pArmy->GetID())
							{
								continue;
							}
						}

						iNumDeployPlotsFound++;
						iScore = 600 - (iPlotDistance * 100);
						if(pCell->IsSubjectToAttack())
						{
							iScore -= 100;
							bSafeForDeployment = false;
						}
						else
						{
							iNumSafePlotsFound++;
						}
						if(pCell->IsEnemyCanMovePast())
						{
							iScore -= 100;
						}
						if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
						{
							iScore += 100;
						}
						else
						{
							iScore += pCell->GetDefenseModifier() * 2;
						}
						if (bForcedToUseWater)
						{
							iScore = 10;
						}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
						if(MOD_DIPLOMACY_CIV4_FEATURES && pPlot->IsAvoidMovement(m_pPlayer->GetID()))
						{
							iScore -= 100;
						}
#endif

						pCell->SetSafeForDeployment(bSafeForDeployment);
						pCell->SetDeploymentScore(iScore);

						// Save this in our list of potential targets
						target.SetTargetX(pPlot->getX());
						target.SetTargetY(pPlot->getY());
						target.SetAuxIntData(iScore);

						// A bit of a hack -- use high priority targets to indicate safe plots for ranged units
						if(bSafeForDeployment)
						{
							target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
						}
						else
						{
							target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
						}

						m_TempTargets.push_back(target);
					}
				}
			}
		}
	}

	// Make sure we found enough
	if(iNumSafePlotsFound < iNumRangedUnits || iNumDeployPlotsFound < (iNumMeleeUnits + iNumRangedUnits))
	{
		return false;
	}

	return true;
}

/// Pick best hexes for deploying our army (mostly based on keeping a tight formation that is moving toward the target). Returns false if insufficient free plots.
bool CvTacticalAI::ScoreFormationPlots(CvArmyAI* pArmy, CvPlot* pForwardTarget, CvPlot *pCurrentCOM, int iNumUnits)
{
	int iDX, iDY;
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	int iNumDeployPlotsFound = 0;
	CvTacticalTarget target;
	bool bForcedToUseWater;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

	int iRange = 3;
	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pPlot = plotXY(pForwardTarget->getX(), pForwardTarget->getY(), iDX, iDY);
			if(pPlot != NULL)
			{
				bForcedToUseWater = false;

				int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pForwardTarget->getX(), pForwardTarget->getY());
				if(iPlotDistance <= iRange)
				{
					int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());

					bool bValid = false;
					if(pOperation->IsMixedLandNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true /*bWater*/))
					{
						bValid = true;
					}

					else if(pOperation->IsAllNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true /*bWater*/))
					{
						bValid = true;
					}

					else if((!pOperation->IsAllNavalOperation() && !pOperation->IsMixedLandNavalOperation()) && (pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/) || GC.getMap().GetAIMapHint() & 1))
					{
						bValid = true;
						if (pCell->IsWater())
						{
							bForcedToUseWater = true;
						}
					}

					if(pOperation->IsMixedLandNavalOperation() || pOperation->IsAllNavalOperation())
					{
						if(!pArmy->IsAllOceanGoing() && pCell->IsOcean())
						{
							bValid = false;
						}
					}

					if(bValid)
					{
						// Skip this plot if friendly unit that isn't in this army
						CvUnit* pFriendlyUnit;
						pFriendlyUnit = pCell->GetFriendlyMilitaryUnit();
						if(pFriendlyUnit != NULL)
						{
							if(pFriendlyUnit->getArmyID() != pArmy->GetID())
							{
								continue;
							}
						}

						iNumDeployPlotsFound++;

						// Score plots close to COM and turn target highest
						int iPlotDistanceFromCOM;
						if (pForwardTarget == pCurrentCOM)
						{
							iPlotDistanceFromCOM = iPlotDistance;
						}
						else
						{
							iPlotDistanceFromCOM = plotDistance(pPlot->getX(), pPlot->getY(), pCurrentCOM->getX(), pCurrentCOM->getY());
						}

						iScore = 800 - (iPlotDistance * 100) - (iPlotDistanceFromCOM * 100);

						if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
						{
							iScore += 100;
						}
						else
						{
							iScore += pCell->GetDefenseModifier() * 2;
						}
						if (bForcedToUseWater)
						{
							iScore = 10;
						}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
						if(MOD_DIPLOMACY_CIV4_FEATURES && pPlot->IsAvoidMovement(m_pPlayer->GetID()))
						{
							iScore -= 100;
						}
#endif

						// Safe if closer to rear, or if forward target and COM are the same
						bool bSafeForDeployment = iPlotDistanceFromCOM < iPlotDistance || pForwardTarget == pCurrentCOM;
						pCell->SetSafeForDeployment(bSafeForDeployment);

						// Reduce score if safe for ranged (so these don't get picked first)
						if (bSafeForDeployment)
						{
							iScore -= 200;
						}

						pCell->SetDeploymentScore(iScore);

						// Save this in our list of potential targets
						target.SetTargetX(pPlot->getX());
						target.SetTargetY(pPlot->getY());
						target.SetAuxIntData(iScore);
						// A bit of a hack -- use high priority targets to indicate safe plots for ranged units
						if(bSafeForDeployment)
						{
							target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
						}
						else
						{
							target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
						}
						m_TempTargets.push_back(target);
					}
				}
			}
		}
	}

	// Make sure we found enough
	if(iNumDeployPlotsFound < iNumUnits)
	{
		return false;
	}

	return true;
}

/// Complete moves for all units requested through calls to MoveWithFormation()
void CvTacticalAI::ExecuteNavalFormationMoves(CvArmyAI* pArmy, CvPlot* pTurnTarget)
{
	UnitHandle pUnit;
	int iMostUnits;
	int iLeastUnits;
	CvPlot* pLoopPlot;
	bool bMoreEscorted = true;
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;

	if(m_OperationUnits.size() == 0)
	{
		return;
	}

	int iNavalUnits = 0;
	int iEscortedUnits = 0;
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		CvUnit *pOpUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (pOpUnit)
		{
			if (pOpUnit->getDomainType() == DOMAIN_LAND)
			{
				iEscortedUnits++;
			}
			else
			{
				if (pOpUnit->IsGreatAdmiral())
				{
					iEscortedUnits++;
				}
				else
				{
					iNavalUnits++;
				}
			}
		}
	}

	iMostUnits = max(iNavalUnits, iEscortedUnits);
	iLeastUnits = min(iNavalUnits, iEscortedUnits);
	if(iNavalUnits > iEscortedUnits)
	{
		bMoreEscorted = false;
	}

	// Range around turn target based on number of units we need to place
	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iMostUnits);

	// See if we have enough places to put everyone
	if(!ScoreDeploymentPlots(pTurnTarget, pArmy, iMostUnits, 0, iRange) &&
	        !ScoreDeploymentPlots(pTurnTarget, pArmy, iMostUnits, 0, 3))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy near turn target", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
	}

	// Compute moves for whomever has more units first
	else
	{
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();
		bool bDone = false;
		int iMostUnitsToPlace = iMostUnits;

		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
			if(FindClosestNavalOperationUnit(pLoopPlot, bMoreEscorted))
			{
				for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
				{
					CvBlockingUnit block;
					block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
					block.SetPlot(pLoopPlot);
					block.SetNumChoices((int)m_CurrentMoveUnits.size());
					block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
					m_PotentialBlocks.push_back(block);
				}
				iMostUnitsToPlace--;
				if(iMostUnitsToPlace == 0)
				{
					bDone = true;
				}
			}
		}

		// Now ready to make the assignments
		AssignDeployingUnits(iMostUnits - iMostUnitsToPlace);
		PerformChosenMoves(2);

		// Log if someone in army didn't get a move assigned (how do we address this in the future?)
		if(m_ChosenBlocks.size() < (unsigned int)iMostUnits)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("No naval deployment move for %d units in first pass", iMostUnits - m_ChosenBlocks.size());
				LogTacticalMessage(strMsg);
			}
		}
		
		if(iLeastUnits > 0)
		{
			// Now repeat for the other type of units, using the same target plots
			m_TempTargets.clear();
			CvTacticalTarget temp;

			for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
			{
				temp.SetTargetX(m_ChosenBlocks[iI].GetPlot()->getX());
				temp.SetTargetY(m_ChosenBlocks[iI].GetPlot()->getY());
				m_TempTargets.push_back(temp);
			}

			m_PotentialBlocks.clear();
			bDone = false;
			int iLeastUnitsToPlace = iLeastUnits;

			for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
			{
				pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(FindClosestNavalOperationUnit(pLoopPlot, !bMoreEscorted))
				{
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}
					iLeastUnitsToPlace--;
					if(iLeastUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}

			// Now ready to make the assignments
			AssignDeployingUnits(iLeastUnits - iLeastUnitsToPlace);
			PerformChosenMoves(2);

			if(m_ChosenBlocks.size() < (unsigned int)iLeastUnits)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("No naval deployment move for %d units in second pass", iLeastUnits - m_ChosenBlocks.size());
					LogTacticalMessage(strMsg);
				}
			}
		}
	}
}

/// Help protect a nearby naval operation (returns true if found one to support)
bool CvTacticalAI::PlotEscortNavalOperationMoves(CvArmyAI* pArmy)
{
	CvPlot* pTarget = NULL;
	int iOperationID;
	int iArmyID;
	CvArmyAI* pArmyToEscort;

	// Only one colonization operation at a time, so don't have to search for more than one
	if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_COLONIZE, &iOperationID))
	{
		CvAIOperation* pOperation = m_pPlayer->getAIOperation(iOperationID);
		iArmyID = pOperation->GetFirstArmyID();
		if(iArmyID != -1)
		{
			pArmyToEscort = m_pPlayer->getArmyAI(iArmyID);
			if(pArmyToEscort)
			{
				pTarget = pArmyToEscort->GetCenterOfMass(DOMAIN_SEA);
			}
		}
	}
	if(pTarget)
	{
		ExecuteFleetMoveToTarget(pArmy, pTarget);
		return true;
	}

	return false;
}

/// Move a squadron of naval units to a target
void CvTacticalAI::ExecuteFleetMoveToTarget(CvArmyAI* pArmy, CvPlot* pTarget)
{
	// Request moves for all units
	for(int iI = 0; iI < pArmy->GetNumFormationEntries(); iI++)
	{
		CvArmyFormationSlot* pSlot = pArmy->GetFormationSlot(iI);
		if(pSlot->GetUnitID() != NO_UNIT)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
			if(pUnit && !pUnit->TurnProcessed())
			{
				pSlot->SetStartedOnOperation(true);

				CvMultiUnitFormationInfo* pkMultiUnitFormation = GC.getMultiUnitFormationInfo(pArmy->GetFormationIndex());
				if(pkMultiUnitFormation)
				{
					const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormation->getFormationSlotEntry(iI);
					MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
				}
			}
		}
	}
	ExecuteNavalFormationMoves(pArmy, pTarget);
}

// ROUTINES TO PROCESS AND SORT TARGETS

/// Mark units that can damage key items as priority targets
void CvTacticalAI::IdentifyPriorityTargets()
{
	CvCity* pLoopCity;
	int iCityLoop;
	FFastVector<CvTacticalTarget> possibleAttackers;
	int iExpectedDamage;
	int iExpectedTotalDamage;

	// Loop through each of our cities
	for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		// Compile a list of units that can attack it this turn and what their expected damage is
		possibleAttackers.clear();
		iExpectedTotalDamage = 0;

		CvTacticalTarget* pTarget;
		pTarget = GetFirstUnitTarget();
		while(pTarget != NULL)
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if(pEnemyUnit)
			{
				iExpectedDamage = 0;

				if(pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) > pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL))
				{
					if(plotDistance(pEnemyUnit->getX(), pEnemyUnit->getY(), pLoopCity->getX(), pLoopCity->getY()) <= pEnemyUnit->GetRange())
					{
						if(pEnemyUnit->canEverRangeStrikeAt(pLoopCity->getX(), pLoopCity->getY()))
						{
							iExpectedDamage = pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false);
						}
					}
				}
				else if(CanReachInXTurns(pEnemyUnit, pLoopCity->plot(), 1))
				{
					int iAttackerStrength = pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL);
					int iDefenderStrength = pLoopCity->getStrengthValue();
					CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pLoopCity->getOwner(), pLoopCity->getX(), pLoopCity->getY(), pEnemyUnit->getX(), pEnemyUnit->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit != NULL)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pEnemyUnit.pointer(), NULL, false);
					}
					iExpectedDamage = pEnemyUnit->getCombatDamage(iAttackerStrength, iDefenderStrength, pEnemyUnit->getDamage() + iDefenderFireSupportCombatDamage, /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ true);
				}
				if(iExpectedDamage > 0)
				{
					iExpectedTotalDamage += iExpectedDamage;
					possibleAttackers.push_back(*pTarget);
				}
			}
			pTarget = GetNextUnitTarget();
		}

		// If they can take the city down and they are a melee unit, then they are a high priority target
		if(iExpectedTotalDamage > (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()))
		{
			unsigned int iAttackerIndex = 0;

			// Loop until we've found all the attackers in the unit target list
			pTarget = GetFirstUnitTarget();
			while(iAttackerIndex < possibleAttackers.size() && pTarget != NULL)
			{
				// Match based on X, Y
				if(pTarget->GetTargetX() == possibleAttackers[iAttackerIndex].GetTargetX() &&
				        pTarget->GetTargetY() == possibleAttackers[iAttackerIndex].GetTargetY())
				{
					CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
					if(pEnemyUnit && pEnemyUnit->IsCanAttackRanged())
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
					}
					else
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
					iAttackerIndex++;
				}
				pTarget = GetNextUnitTarget();
			}
		}

		// If they can damage a city they are a medium priority target
		else if(possibleAttackers.size() > 0)
		{
			unsigned int iAttackerIndex = 0;

			// Loop until we've found all the attackers in the unit target list
			pTarget = GetFirstUnitTarget();
			while(iAttackerIndex < possibleAttackers.size() && pTarget != NULL)
			{
				// Match based on X, Y
				if(pTarget->GetTargetX() == possibleAttackers[iAttackerIndex].GetTargetX() &&
				        pTarget->GetTargetY() == possibleAttackers[iAttackerIndex].GetTargetY())
				{
					if(pTarget->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
					}
					iAttackerIndex++;
				}
				pTarget = GetNextUnitTarget();
			}
		}
	}
}

/// Mark units that can damage our barbarian camps as priority targets
void CvTacticalAI::IdentifyPriorityBarbarianTargets()
{
	CvPlot* pLoopPlot;
	CvTacticalTarget* pTarget;

	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			pTarget = GetFirstUnitTarget();
			while(pTarget != NULL)
			{
				bool bPriorityTarget = false;

				// Skip if already a priority target (because was able to strike another camp)
				if(pTarget->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
				{
					CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
					if(pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) > pEnemyUnit->GetMaxAttackStrength(NULL, pLoopPlot, NULL))
					{
						if(plotDistance(pEnemyUnit->getX(), pEnemyUnit->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= pEnemyUnit->GetRange())
						{
							if(pEnemyUnit->canEverRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
							{
								bPriorityTarget = true;
							}
						}
					}
					else if(CanReachInXTurns(pEnemyUnit, pLoopPlot, 1))
					{
						bPriorityTarget = true;
					}
					if(bPriorityTarget)
					{
						pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
				}
				pTarget = GetNextUnitTarget();
			}
		}
	}
}

/// Mark units that we'd like to make opportunity attacks on because of their unit type (e.g. catapults)
void CvTacticalAI::IdentifyPriorityTargetsByType()
{
	// Look through all the enemies we can see
	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Don't consider units that are already medium priority
		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			// Ranged units will always be medium priority targets
			CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();
			if(pUnit->IsCanAttackRanged())
			{
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
			}
		}

		// Don't consider units that are already high priority
		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			// Units defending citadels will always be high priority targets
			CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();
			ImprovementTypes eImprovement = pUnit->plot()->getImprovementType();
			if(pUnit->plot()->getOwner() == pUnit->getOwner() &&
			        eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
			{
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			}
		}
#if defined(MOD_BALANCE_CORE_MILITARY)
		if (MOD_BALANCE_CORE_MILITARY) 
		{
			// Don't consider units that are already high priority
			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
					m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				// Units defending forts will always be high priority targets
				CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();
				ImprovementTypes eImprovement = pUnit->plot()->getImprovementType();
				if(eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetDefenseModifier() > 0)
				{
					m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
				}
				TerrainTypes eTerrain = pUnit->plot()->getTerrainType();
				//Units in weak terrain will be a high target.
				if(GC.getTerrainInfo(eTerrain)->getDefenseModifier() < 0)
				{
					m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
				}
				//Is a unit below 1/4 health? If so, make it a high-priority target.
				if(pUnit && pUnit->getOwner() != m_pPlayer->GetID())
				{
					int iDamage = 0;
					iDamage = pUnit->GetMaxHitPoints() - pUnit->GetCurrHitPoints();
					if(iDamage != 0 && (iDamage <= (pUnit->GetMaxHitPoints() / 4)))
					{
						m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
				}
			}
			// Don't consider units that are already medium priority
			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
					m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();
				//Units in defensive terrain will be more of a target.
				TerrainTypes eTerrain = pUnit->plot()->getTerrainType();
				if(GC.getTerrainInfo(eTerrain)->getDefenseModifier() > 0)
				{
					m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
				}
			}
		}
#endif
	}
}

/// Don't allow tiles within 2 to both be blockade points
void CvTacticalAI::EliminateNearbyBlockadePoints()
{
	// First, sort the sentry points by priority
	std::stable_sort(m_NavalResourceBlockadePoints.begin(), m_NavalResourceBlockadePoints.end());

	// Create temporary copy of list
	TacticalList tempPoints;
	tempPoints = m_NavalResourceBlockadePoints;

	// Clear out main list
	m_NavalResourceBlockadePoints.clear();

	// Loop through all points in copy
	TacticalList::iterator it, it2;
	for(it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;

		// Is it adjacent to a point in the main list?
		for(it2 = m_NavalResourceBlockadePoints.begin(); it2 != m_NavalResourceBlockadePoints.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) <= 2)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if(!bFoundAdjacent)
		{
			m_NavalResourceBlockadePoints.push_back(*it);
		}
	}

	// Now copy all points into main target list
	for(it = m_NavalResourceBlockadePoints.begin(); it != m_NavalResourceBlockadePoints.end(); ++it)
	{
		m_AllTargets.push_back(*it);
	}
}

/// Sift through the target list and find just those that apply to the dominance zone we are currently looking at
void CvTacticalAI::ExtractTargetsForZone(CvTacticalDominanceZone* pZone /* Pass in NULL for all zones */)
{
	TacticalList::iterator it;

	m_ZoneTargets.clear();
	for(it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
	{
		bool bValid = false;

		if(pZone == NULL)
		{
			bValid = true;
		}
		else
		{
			DomainTypes eDomain = DOMAIN_LAND;
			if(pZone->IsWater())
			{
				eDomain = DOMAIN_SEA;
			}
			bValid = it->IsTargetValidInThisDomain(eDomain);
		}
		if(bValid)
		{
			if(pZone == NULL || it->GetDominanceZone() == pZone->GetDominanceZoneID())
			{
				m_ZoneTargets.push_back(*it);
			}

			// Not obviously in this zone, but if within 2 of city we want them anyway
			else
			{
				CvCity* pCity;
				pCity = pZone->GetClosestCity();
				if(pCity)
				{
					if(plotDistance(pCity->getX(), pCity->getY(), it->GetTargetX(), it->GetTargetY()) <= 2)
					{
						m_ZoneTargets.push_back(*it);
					}
				}
			}
		}
	}
}

/// Find the first target of a requested type in current dominance zone (call after ExtractTargetsForZone())
CvTacticalTarget* CvTacticalAI::GetFirstZoneTarget(AITacticalTargetType eType)
{
	m_eCurrentTargetType = eType;
	m_iCurrentTargetIndex = 0;

	while(m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if(m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
		{
			return &m_ZoneTargets[m_iCurrentTargetIndex];
		}
		m_iCurrentTargetIndex++;
	}

	return NULL;
}

/// Find the next target of a requested type in current dominance zone (call after GetFirstZoneTarget())
CvTacticalTarget* CvTacticalAI::GetNextZoneTarget()
{
	m_iCurrentTargetIndex++;

	while(m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if(m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
		{
			return &m_ZoneTargets[m_iCurrentTargetIndex];
		}
		m_iCurrentTargetIndex++;
	}

	return NULL;
}

/// Find the first unit target (in full list of targets -- NOT by zone)
CvTacticalTarget* CvTacticalAI::GetFirstUnitTarget()
{
	m_iCurrentUnitTargetIndex = 0;

	while(m_iCurrentUnitTargetIndex < (int)m_AllTargets.size())
	{
		if(m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			return &m_AllTargets[m_iCurrentUnitTargetIndex];
		}
		m_iCurrentUnitTargetIndex++;
	}

	return NULL;
}

/// Find the next unit target (call after GetFirstUnitTarget())
CvTacticalTarget* CvTacticalAI::GetNextUnitTarget()
{
	m_iCurrentUnitTargetIndex++;

	while(m_iCurrentUnitTargetIndex < (int)m_AllTargets.size())
	{
		if(m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			return &m_AllTargets[m_iCurrentUnitTargetIndex];
		}
		m_iCurrentUnitTargetIndex++;
	}

	return NULL;
}

// ROUTINES TO EXECUTE A MISSION

/// Capture the gold from a barbarian camp
void CvTacticalAI::ExecuteBarbarianCampMove(CvPlot* pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}

/// Capture an undefended civilian
void CvTacticalAI::ExecuteCivilianCapture(CvPlot* pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());

		pUnit->SetTacticalAIPlot(NULL);
	}
}

/// Pillage an undefended improvement
void CvTacticalAI::ExecutePillage(CvPlot* pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}

/// Pillage an undefended improvement
void CvTacticalAI::ExecutePlunderTradeUnit(CvPlot* pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PLUNDER_TRADE_ROUTE());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}

/// Paradrop in to pillage an undefended improvement
void CvTacticalAI::ExecuteParadropPillage(CvPlot* pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_PARADROP(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}

/// Attack a defended space
void CvTacticalAI::ExecuteAttack(CvTacticalTarget* pTarget, CvPlot* pTargetPlot, bool bInflictWhatWeTake, bool bMustSurviveAttack)
{
	void* pFirstAttacker = NULL;
	bool bFirstAttackRanged = false;
	bool bFirstAttackCity = false;

	if(PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return;
	}

	// How much damage do we still need to inflict?
	int iDamageRemaining = (pTarget->GetAuxIntData() * (100 + GC.getAI_TACTICAL_OVERKILL_PERCENT())) / 100;

#if defined(MOD_AI_SMART_AIR_TACTICS)
	if (MOD_AI_SMART_AIR_TACTICS) {
		// Start by sending possible air sweeps
		for(unsigned int iI = 0; iI < m_CurrentAirUnits.size(); iI++)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentAirUnits[iI].GetID());

			if(pUnit)
			{
				if(pUnit->canAirSweep())
				{
					if (pTarget)
					{
						pUnit->PushMission(CvTypes::getMISSION_AIR_SWEEP(), pTargetPlot->getX(), pTargetPlot->getY());
						UnitProcessed(m_CurrentAirUnits[iI].GetID(), false /*bMarkTacticalMap*/);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Sending %s to air sweep prior to attack to Target X: %d, Y: %d", pUnit->getName().GetCString(), pTargetPlot->getX(), pTargetPlot->getY());
							LogTacticalMessage(strMsg);
						}
					}
				}
			}
		}
	}
#endif

	// Start by applying damage from city bombards
	for(unsigned int iI = 0; iI < m_CurrentMoveCities.size() && iDamageRemaining > 0; iI++)
	{
		CvCity* pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		if(pCity != NULL)
		{
			if(QueueAttack(pCity, pTarget, true /*bRanged*/, true /*bCity*/))
			{
				pFirstAttacker = (void*)pCity;
				bFirstAttackRanged = true;
				bFirstAttackCity = true;
			}

			// Subtract off expected damage
			iDamageRemaining -= m_CurrentMoveCities[iI].GetExpectedTargetDamage();
		}
	}

#if defined(MOD_AI_SMART_MELEE_TACTICS)
	// Moved up from below
	// Make a list of adjacent plots
	std::vector<CvPlot *> plotList;
	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
	{
		CvPlot* pAdjacentPlot = plotDirection(pTargetPlot->getX(), pTargetPlot->getY(), ((DirectionTypes)iDirectionLoop));
		if (pAdjacentPlot)
		{
			int iPlotIndex = GC.getMap().plotNum(pAdjacentPlot->getX(), pAdjacentPlot->getY());
			CvTacticalAnalysisCell *pCell = m_pMap->GetCell(iPlotIndex);
			if(pAdjacentPlot != NULL && pAdjacentPlot->getNumDefenders(pTarget->GetTargetPlayer()) == 0 && !pCell->IsEnemyCity())
			{
				plotList.push_back(pAdjacentPlot);
			}
		}
	}
#endif

#if defined(MOD_AI_SMART_MELEE_TACTICS)
	if (MOD_AI_SMART_MELEE_TACTICS)
	{
		// Loop for melee units just to reposition.
		for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
		{
			if(!bInflictWhatWeTake || m_CurrentMoveUnits[iI].GetExpectedTargetDamage() >= m_CurrentMoveUnits[iI].GetExpectedSelfDamage())
			{
				UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());

				if(pUnit)
				{
					if(pUnit->getMoves() > 0 && (!bMustSurviveAttack || ((m_CurrentMoveUnits[iI].GetExpectedSelfDamage() + pUnit->getDamage()) < pUnit->GetMaxHitPoints())))
					{
						// Are we a melee unit
						if(!pUnit->IsCanAttackRanged())
						{
							bool unitHasMoved = false;

							// Not adjacent
							if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > 1)
							{
								// Find spaces adjacent to target we can move into with MP left
								std::vector<CvPlot*>::iterator it;

								for (it = plotList.begin(); it != plotList.end(); it++)
								{
									if (TurnsToReachTarget(pUnit, *it, false /*bReusePaths*/, false /*bIgnoreUnits*/, true /*bIgnoreStacking*/) == 0)
									{
										// Move up there
										pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), (*it)->getX(), (*it)->getY());
										plotList.erase(it);
										unitHasMoved = true;
										break;
									}
								}

								// Didn't find an open hex
								if (!unitHasMoved)
								{
									// Move up if we can
									MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, (pUnit->getDomainType() == DOMAIN_LAND));
								}
							}
						}
					}
				}
			}
		}
	}
#endif

	// First loop is ranged units only
#if defined(MOD_AI_SMART_MELEE_TACTICS)
	int iHpLimit = (MOD_AI_SMART_MELEE_TACTICS) ? 1 : 0;
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size() && iDamageRemaining > iHpLimit; iI++)
#else
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size() && iDamageRemaining > 0; iI++)
#endif
	{
		if(!bInflictWhatWeTake || m_CurrentMoveUnits[iI].GetExpectedTargetDamage() >= m_CurrentMoveUnits[iI].GetExpectedSelfDamage())
		{
			UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
			if(pUnit)
			{
				if(pUnit->getMoves() > 0)
				{
#if defined(MOD_AI_SMART_RANGED_UNITS)
					if (MOD_AI_SMART_RANGED_UNITS)
					{
						// Are we a ranged unit
						if(pUnit->IsCanAttackRanged())
						{
							bool bQueueTryRangedAttack = false;

							// Are we in range and do we have LOS to the target?
							if((plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= pUnit->GetRange()) && (pUnit->canEverRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY())))
							{
								bQueueTryRangedAttack = true;
							}
							else
							{
								// Not in range/ not LOS: then lets try to reposition unit.
								CvPlot* repositionPlot = GetBestRepositionPlot(pUnit, pTargetPlot);

								if (repositionPlot != NULL)
								{
									pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), repositionPlot->getX(), repositionPlot->getY());
									bQueueTryRangedAttack = true;
								}
							}

							if (bQueueTryRangedAttack)
							{
								// Do we have LOS to the target?
								if(pUnit->canEverRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
								{
									// Do we need to set up to make a ranged attack?
									if(pUnit->canSetUpForRangedAttack(NULL))
									{
										pUnit->setSetUpForRangedAttack(true);

										if(!pUnit->canMove())
										{
											pUnit->SetTacticalAIPlot(NULL);
											UnitProcessed(pUnit->GetID());
										}
									}

									// Can we hit it with a ranged attack?  If so, that gets first priority
									if(pUnit->canMove() && pUnit->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
									{
										// Queue up this attack
										if(QueueAttack((void*)pUnit.pointer(), pTarget, true /*bRanged*/, false /*bCity*/))
										{
											pFirstAttacker = (void*)pUnit.pointer();
											bFirstAttackRanged = true;
										}

										pUnit->SetTacticalAIPlot(NULL);
										UnitProcessed(m_CurrentMoveUnits[iI].GetID());
										// Subtract off expected damage
										iDamageRemaining -= m_CurrentMoveUnits[iI].GetExpectedTargetDamage();
									}
								}
							}
						}
					}
					else
					{
#endif
						if(!bMustSurviveAttack || ((m_CurrentMoveUnits[iI].GetExpectedSelfDamage() + pUnit->getDamage()) < pUnit->GetMaxHitPoints()))
						{
							// Are we a ranged unit
							if(pUnit->IsCanAttackRanged())
							{
								// Are we in range?
								if(plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= pUnit->GetRange())
								{
									// Do we have LOS to the target?
									if(pUnit->canEverRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
									{
										// Do we need to set up to make a ranged attack?
										if(pUnit->canSetUpForRangedAttack(NULL))
										{
											pUnit->setSetUpForRangedAttack(true);
											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strTemp;
												CvString strMsg;
												strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
												strMsg.Format("Set up %s for ranged attack", strTemp.GetCString());
												LogTacticalMessage(strMsg);
											}
											if(!pUnit->canMove())
											{
												pUnit->SetTacticalAIPlot(NULL);
												UnitProcessed(pUnit->GetID());
											}
										}

										// Can we hit it with a ranged attack?  If so, that gets first priority
										if(pUnit->canMove() && pUnit->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
										{
											// Queue up this attack
											if(QueueAttack((void*)pUnit.pointer(), pTarget, true /*bRanged*/, false /*bCity*/))
											{
												pFirstAttacker = (void*)pUnit.pointer();
												bFirstAttackRanged = true;
											}
											pUnit->SetTacticalAIPlot(NULL);
											UnitProcessed(m_CurrentMoveUnits[iI].GetID());

											// Subtract off expected damage
											iDamageRemaining -= m_CurrentMoveUnits[iI].GetExpectedTargetDamage();
										}
									}
								}
							}
						}
						else
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Not attacking with unit. We'll destroy ourself, %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strMsg);
							}
						}
#if defined(MOD_AI_SMART_RANGED_UNITS)
					}
#endif
				}
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Not attacking with unit. Can't generate a good damage ratio.");
				LogTacticalMessage(strMsg);
			}
		}
	}

	// If target is city, want to get in one melee attack, so set damage remaining to 1
	if(pTarget->GetTargetType() == AI_TACTICAL_TARGET_CITY && iDamageRemaining < 1)
	{
		iDamageRemaining = 1;
	}

#if !defined(MOD_AI_SMART_MELEE_TACTICS)
	// Make a list of adjacent plots
	std::vector<CvPlot *> plotList;
	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
	{
		CvPlot* pAdjacentPlot = plotDirection(pTargetPlot->getX(), pTargetPlot->getY(), ((DirectionTypes)iDirectionLoop));
		if (pAdjacentPlot)
		{
			int iPlotIndex = GC.getMap().plotNum(pAdjacentPlot->getX(), pAdjacentPlot->getY());
			CvTacticalAnalysisCell *pCell = m_pMap->GetCell(iPlotIndex);
			if(pAdjacentPlot != NULL && pAdjacentPlot->getNumDefenders(pTarget->GetTargetPlayer()) == 0 && !pCell->IsEnemyCity())
			{
				plotList.push_back(pAdjacentPlot);
			}
		}
	}
#endif

	// Second loop are only melee units
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size() && iDamageRemaining > 0; iI++)
	{
		if(!bInflictWhatWeTake || m_CurrentMoveUnits[iI].GetExpectedTargetDamage() >= m_CurrentMoveUnits[iI].GetExpectedSelfDamage())
		{
			UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
			if(pUnit)
			{
				if(pUnit->getMoves() > 0 && (!bMustSurviveAttack || ((m_CurrentMoveUnits[iI].GetExpectedSelfDamage() + pUnit->getDamage()) < pUnit->GetMaxHitPoints())))
				{
					// Are we a melee unit
					if(!pUnit->IsCanAttackRanged())
					{
						bool bQueueAttackNow = false;
						
						// Already adjacent?
#if defined(MOD_AI_SMART_MELEE_TACTICS)
						if (MOD_AI_SMART_MELEE_TACTICS)
						{
							bQueueAttackNow = plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= 1;
						}
						else
						{
#endif
							if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= 1)
							{
								bQueueAttackNow = true;
							}
							else
							{
								// Find spaces adjacent to target we can move into with MP left
								std::vector<CvPlot *>::iterator it;
								for (it = plotList.begin(); it != plotList.end(); it++)
								{
									if (TurnsToReachTarget(pUnit, *it, false /*bReusePaths*/, false /*bIgnoreUnits*/, true /*bIgnoreStacking*/) == 0)
									{
										// Move up there
										pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), (*it)->getX(), (*it)->getY());
										bQueueAttackNow = true;

										plotList.erase(it);
										break;
									}
								}

								// Didn't find an open hex
								if (!bQueueAttackNow)
								{
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strMsg;
										strMsg.Format("Not enough free hexes next for all melee units to attack, Target X: %d, Y: %d", pTargetPlot->getX(), pTargetPlot->getY());
										LogTacticalMessage(strMsg);
									}
								
									// Move up if we can
									if (MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, (pUnit->getDomainType() == DOMAIN_LAND)))
									{
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strMsg;
											strMsg.Format("Moving adjacent for attack next turn, Now at X: %d, Y: %d", pUnit->getX(), pUnit->getY());
											LogTacticalMessage(strMsg);
										}
									}
								}
							}
#if defined(MOD_AI_SMART_MELEE_TACTICS)
						}
#endif

						if (bQueueAttackNow)
						{
							// Queue up this attack
							if(QueueAttack((void*)pUnit.pointer(), pTarget, false /*bRanged*/, false /*bCity*/))
							{
								pFirstAttacker = (void*)pUnit.pointer();
							}
							pUnit->SetTacticalAIPlot(NULL);
							UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false /*bMarkTacticalMap*/);

							// Subtract off expected damage
							iDamageRemaining -= m_CurrentMoveUnits[iI].GetExpectedTargetDamage();
						}
					}
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Not attacking with unit. We'll destroy ourself, %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg);
					}
				}
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Not attacking with unit. Can't generate a good damage ratio.");
				LogTacticalMessage(strMsg);
			}
		}
	}

	// Start up first attack
	if(pFirstAttacker)
	{
		LaunchAttack(pFirstAttacker, pTarget, true /*bFirst*/, bFirstAttackRanged, bFirstAttackCity);
	}
}

/// Execute moving units to a better location
void CvTacticalAI::ExecuteRepositionMoves()
{
	CvPlot* pBestPlot = NULL;
	CvString strTemp;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			strTemp = pUnit->getUnitInfo().GetDescription();

			// LAND MOVES
			if(pUnit->getDomainType() == DOMAIN_LAND)
			{
				pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange);

				if(pBestPlot)
				{
					if(MoveToEmptySpaceNearTarget(pUnit, pBestPlot, (pUnit->getDomainType()==DOMAIN_LAND)))
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s moved to empty space near target, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
							                    pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}
	}
}

#define PATH_PLAN_LAST
#if defined(PATH_PLAN_LAST)
typedef CvWeightedVector<CvPlot*, 1, true> WeightedPlotVector;
//	---------------------------------------------------------------------------
//	Return the first reachable plot in the weighted plot list.
//	It is assumed that the list has yet to be sorted and will do so.
static CvPlot* GetReachablePlot(UnitHandle pUnit, WeightedPlotVector& aPlots, int iTurns)
{
	uint uiListSize;
	if ((uiListSize = aPlots.size()) > 0)
	{
		aPlots.SortItems();
		for (uint i = uiListSize; i--; )		// Go backward, the CvWeightedVector sorts highest to lowest and we want the least dangerous location
		{
			CvPlot* pPlot = aPlots.GetElement(i);
			if(CanReachInXTurns(pUnit, pPlot, iTurns))
			{
				return pPlot;
			}
		}
	}
	return NULL;
}
#endif

/// Moves units to the hex with the lowest danger
void CvTacticalAI::ExecuteMovesToSafestPlot()
{
	int iDanger;

#if defined(PATH_PLAN_LAST)
	// Maybe make these part of the class and just reuse?
	WeightedPlotVector aCityList;
	aCityList.reserve(10);
	WeightedPlotVector aZeroDangerList;
	aZeroDangerList.reserve(100);
	WeightedPlotVector aCoverList;
	aCoverList.reserve(100);
	WeightedPlotVector aDangerList;
	aDangerList.reserve(100);
#endif

	TeamTypes ePlayerTeam = m_pPlayer->getTeam();
	PlayerTypes ePlayerID = m_pPlayer->GetID();

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
#if defined(PATH_PLAN_LAST)
			aCityList.clear();
			aZeroDangerList.clear();
			aCoverList.clear();
			aDangerList.clear();
#endif
			CvPlot* pBestPlot = NULL;

			int iRange = pUnit->getUnitInfo().GetMoves();
#if !defined(PATH_PLAN_LAST)
			int iLowestDanger = MAX_INT;
			bool bResultHasZeroDangerMove = false;
			bool bResultInTerritory = false;
			bool bResultInCity = false;
			bool bResultInCover = false;
#endif

			CvMap& kMap = GC.getMap();
			int iUnitX = pUnit->getX();
			int iUnitY = pUnit->getY();
			DomainTypes eUnitDomain = pUnit->getDomainType();
			bool bIsSeaUnit = eUnitDomain == DOMAIN_SEA;

			// For each plot within movement range of the fleeing unit
			for(int iX = -iRange; iX <= iRange; iX++)
			{
				for(int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot* pPlot = kMap.plot(iUnitX + iX, iUnitY + iY);
					if(pPlot == NULL)
					{
						continue;
					}

					// Can't be a plot with another player's unit in it or another of our unit of same type
					if(pPlot->getNumUnits() > 0)
					{
						IDInfo* pUnitNode = pPlot->headUnitNode();
						if(pUnitNode)
						{
							CvUnit* pFirstUnit = ::getUnit(*pUnitNode);
							if(pFirstUnit)
							{
								if(pFirstUnit->getOwner() != pUnit->getOwner())
								{
									continue;
								}

								else if(pUnit->AreUnitsOfSameType(*pFirstUnit))
								{
									continue;
								}
							}
						}
					}

					// Also filter out sea units targeting land plots
					if (bIsSeaUnit && !pPlot->isWater())
					{
						continue;
					}

#if !defined(PATH_PLAN_LAST)
					if(!CanReachInXTurns(pUnit, pPlot, 1))
					{
						continue;
					}
#endif
					//   prefer being in a city with the lowest danger value
					//   prefer being in a plot with no danger value
					//   prefer being under a unit with the lowest danger value
					//   prefer being in your own territory with the lowest danger value
					//   prefer the lowest danger value

					iDanger = m_pPlayer->GetPlotDanger(*pPlot);
					bool bIsZeroDanger = (iDanger <= 0);
					bool bIsInCity = pPlot->isFriendlyCity(*pUnit, false);
					bool bIsInCover = (pPlot->getNumDefenders(ePlayerID) > 0) && !pUnit->IsCanDefend(pPlot); // only move to cover if I'm defenseless here
					bool bIsInTerritory = (pPlot->getTeam() == ePlayerTeam);
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
					bool bNeedEmbark = ((eUnitDomain == DOMAIN_LAND) && (!pUnit->plot()->isWater()) && (pPlot->isWater()));
#endif

#if defined(PATH_PLAN_LAST)
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
					// Enormous danger on water plot, embarking as last option.
					if(MOD_AI_SMART_FLEE_FROM_DANGER && bNeedEmbark && !bIsInCover)
					{
						int combatMod = pUnit->GetBaseCombatStrength() == 0 ? 100 : pUnit->GetBaseCombatStrength();
						iDanger = min(combatMod * 900, 99999);
						aDangerList.push_back(pPlot, iDanger);
					}
					else
#endif
					if(bIsInCity)
					{
						aCityList.push_back(pPlot, iDanger);
					}
					else
					if(bIsZeroDanger)
					{
						aZeroDangerList.push_back(pPlot, (bIsInTerritory)?0:1);
					}
					else
					if(bIsInCover)
					{
						aCoverList.push_back(pPlot, iDanger);
					}
					else
					{
						aDangerList.push_back(pPlot, iDanger);
					}
#endif

#if !defined(PATH_PLAN_LAST)
					bool bUpdateBestValue = false;

					if(bIsInCity)
					{
						if(!bResultInCity || iDanger < iLowestDanger)
						{
							bUpdateBestValue = true;
						}
					}
					else if(bIsZeroDanger)
					{
						if(!bResultInCity)
						{
							if(bResultHasZeroDangerMove)
							{
								if(bIsInTerritory && !bResultInTerritory)
								{
									bUpdateBestValue = true;
								}
							}
							else
							{
								bUpdateBestValue = true;
							}
						}
					}
					else if(bIsInCover)
					{
						if(!bResultInCity && !bResultHasZeroDangerMove)
						{
							if(!bResultInCover || iDanger < iLowestDanger)
							{
								bUpdateBestValue = true;
							}
						}
					}
					else if(bIsInTerritory)
					{
						if(!bResultInCity && !bResultInCover && !bResultHasZeroDangerMove)
						{
							if(!bResultInTerritory || iDanger < iLowestDanger)
							{
								bUpdateBestValue = true;
							}
						}
					}
					// if we have no good home, head to the lowest danger value
					else if(!bResultInCity && !bResultInCover && !bResultInTerritory && !bResultHasZeroDangerMove)
					{
						if(iDanger < iLowestDanger)
						{
							bUpdateBestValue = true;
						}
					}

					if(bUpdateBestValue)
					{
						pBestPlot = pPlot;
						iLowestDanger = iDanger;

						bResultInTerritory = bIsInTerritory;
						bResultInCity      = bIsInCity;
						bResultInCover     = bIsInCover;
						bResultHasZeroDangerMove = bIsZeroDanger;
					}
#endif
				}
			}

#if defined(PATH_PLAN_LAST)
			// Now that we've gathered up our lists of destinations, go through them in order and pick the first one we can reach.
			// This minimizes accessing the pathfinder.
			if ((pBestPlot = GetReachablePlot(pUnit, aCityList, 1)) == NULL)
				if ((pBestPlot = GetReachablePlot(pUnit, aZeroDangerList, 1)) == NULL)
					if ((pBestPlot = GetReachablePlot(pUnit, aCoverList, 1)) == NULL)
						pBestPlot = GetReachablePlot(pUnit, aDangerList, 1);
#endif

			if(pBestPlot != NULL)
			{
				// Move to the lowest danger value found
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_UNITS_IGNORE_DANGER);
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("Moving %s to safety, To X: %d, Y: %d, From X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("Failed to find destination moving %s to safety from, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}
}

/// Heal chosen units
void CvTacticalAI::ExecuteHeals()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->isFortifyable())
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			}
			UnitProcessed(m_CurrentMoveUnits[iI].GetID());
		}
	}
}

/// Move barbarian to a new location
void CvTacticalAI::ExecuteBarbarianMoves(bool bAggressive)
{
	CvPlot* pBestPlot = NULL;
	CvPlot* pCurrentDestination;
	CvString strTemp;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->isBarbarian())
			{
				strTemp = pUnit->getUnitInfo().GetDescription();

				// LAND MOVES
				if(pUnit->getDomainType() == DOMAIN_LAND)
				{
					AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Land Move, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

					if(bAggressive)
					{
						pBestPlot = FindBestBarbarianLandMove(pUnit);
					}
					else
					{
						pBestPlot = FindPassiveBarbarianLandMove(pUnit);
					}

					if(pBestPlot && MoveToEmptySpaceNearTarget(pUnit, pBestPlot))
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s moved to empty space near target, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
							                    pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
					else
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("No target for %s at position, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}

				// NAVAL MOVES
				else
				{

					AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Naval Move, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

					// Do I still have a destination from a previous turn?
					pCurrentDestination = pUnit->GetTacticalAIPlot();

					// Compute a new destination if I don't have one or am already there
					if(pCurrentDestination == NULL ||
					        pCurrentDestination == pUnit->plot())
					{
						pBestPlot = FindBestBarbarianSeaMove(pUnit);
					}

					// Otherwise just keep moving there (assuming a path is available)
					else
					{
						if(TurnsToReachTarget(pUnit, pCurrentDestination, true /*bReusePaths*/) != MAX_INT)
						{
							pBestPlot = pCurrentDestination;
						}
						else
						{
							pBestPlot = FindBestBarbarianSeaMove(pUnit);
						}
					}

					if(pBestPlot != NULL)
					{
						pUnit->SetTacticalAIPlot(pBestPlot);
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving %s to new position, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
							                    pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
					else
					{
						pUnit->SetTacticalAIPlot(NULL);
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("No target for %s at position, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
#if defined(MOD_BALANCE_CORE_BARBARIAN_THEFT)
				//Let's make Barbs scarier. If they end their move next to a city, let's have them pillage some Gold from the City. If they end their turn in owned land, let's give them a smaller chance to do so.
				int iGold = 0;
				if(pUnit && MOD_BALANCE_CORE_BARBARIAN_THEFT)
				{	
					if(pUnit->plot()->GetAdjacentCity() != NULL)
					{
						CvCity* pCity = pUnit->plot()->GetAdjacentCity();
						if(pCity && pCity->getOwner() != NO_PLAYER && !GET_PLAYER(pCity->getOwner()).isMinorCiv() && !GET_PLAYER(pCity->getOwner()).isBarbarian())
						{
							//Can steal up to 20%
							iGold = (GET_PLAYER(pCity->getOwner()).GetTreasury()->GetGold() *  GC.getGame().getJonRandNum(20, "Barbarian Random Gold Theft") / 100);
							{
								if(iGold > 5)
								{
									GET_PLAYER(pCity->getOwner()).GetTreasury()->ChangeGold(-iGold);

									Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY_DETAILED");
									strMessage << iGold;
									strMessage << pCity->getNameKey();
									Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY");
									strSummary << pCity->getNameKey();

									CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
									if(pNotification)
									{
										pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
									}
								}
							}
						}
					}
					else if(pUnit->plot()->getOwner() != NO_PLAYER)
					{
						PlayerTypes pPlayer = pUnit->plot()->getOwner();
						if(pPlayer && pPlayer != NO_PLAYER && !GET_PLAYER(pPlayer).isMinorCiv() && !GET_PLAYER(pPlayer).isBarbarian())
						{
							//Can steal up to 10%
							iGold = (GET_PLAYER(pPlayer).GetTreasury()->GetGold() *  GC.getGame().getJonRandNum(10, "Barbarian Random Gold Theft") / 100);
							{
								if(iGold > 5)
								{
									GET_PLAYER(pPlayer).GetTreasury()->ChangeGold(-iGold);

									Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_DETAILED");
									strMessage << iGold;
									Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT");

									CvNotifications* pNotification = GET_PLAYER(pPlayer).GetNotifications();
									if(pNotification)
									{
										pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pUnit->getX(), pUnit->getY(), pPlayer);
									}
								}
							}
						}
					}
				}
#endif
			}
		}
	}
}

/// Move Barbarian civilian to a camp (with escort if possible)
void CvTacticalAI::ExecuteBarbarianCivilianEscortMove()
{
	UnitHandle pCivilian;
	UnitHandle pEscort;
	CvPlot* pTarget;
	CvPlot* pCurrent;
	CvPlot* pCivilianMove;
	CvPlot* pEscortMove;
	CvUnit* pLoopUnit;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		pCivilian = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pCivilian)
		{
			pTarget = FindNearbyTarget(pCivilian, MAX_INT, AI_TACTICAL_TARGET_BARBARIAN_CAMP, pCivilian.pointer());
			if(pTarget)
			{
				// If we're not there yet, we have work to do
				pCurrent = pCivilian->plot();
				if(pCurrent == pTarget)
				{
					pCivilian->finishMoves();
					UnitProcessed(pCivilian->GetID());
				}
				else
				{
					if(pCurrent->getNumUnits() > 1)
					{
						for(int iJ = 0; iJ < pCurrent->getNumUnits(); iJ++)
						{
							pLoopUnit = pCurrent->getUnitByIndex(iJ);
							if(pLoopUnit->GetID() != pCivilian->GetID() &&
							        pLoopUnit->getOwner() == pCivilian->getOwner())
							{
								pEscort = UnitHandle(pLoopUnit);
								break;
							}
						}
					}

					// Look at where we'd move this turn taking units into consideration
					int iFlags = 0;
					if(pEscort)
					{
						iFlags = MOVE_UNITS_IGNORE_DANGER;
					}

					// Handle case of no path found at all for civilian
					if(!pCivilian->GeneratePath(pTarget, iFlags, false /*bReuse*/))
					{
						pCivilian->finishMoves();
						if(pEscort)
						{
							pEscort->finishMoves();
						}
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pTarget->getX(), pTarget->getY());
							LogTacticalMessage(strLogString);
						}
					}

					else
					{
						pCivilianMove = pCivilian->GetPathEndTurnPlot();

						// Can we reach our target this turn?
						if(pCivilianMove == pTarget)
						{
							// See which defender is stronger
							UnitHandle pCampDefender = pCivilianMove->getBestDefender(m_pPlayer->GetID());
							if(!pCampDefender || (pEscort && pEscort->GetPower() > pCampDefender->GetPower()))
							{
								if(pEscort && (!pCampDefender || ExecuteMoveOfBlockingUnit(pCampDefender)))
								{
									ExecuteMoveToPlot(pEscort, pCivilianMove);
									ExecuteMoveToPlot(pCivilian, pCivilianMove);
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strTemp;
										CvString strLogString;
										strTemp = pEscort->getUnitInfo().GetDescription();
										strLogString.Format("Moving escorting %s to take over camp defense, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
										LogTacticalMessage(strLogString);
										strTemp = pCivilian->getUnitInfo().GetDescription();
										strLogString.Format("Moving %s to camp, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
										LogTacticalMessage(strLogString);
									}
								}
								else
								{
									ExecuteMoveToPlot(pCivilian, pCivilianMove);
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strTemp;
										CvString strLogString;
										strTemp = pCivilian->getUnitInfo().GetDescription();
										strLogString.Format("Moving %s to camp, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
										LogTacticalMessage(strLogString);
									}
								}
							}
							else
							{
								ExecuteMoveToPlot(pCivilian, pCivilianMove);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strTemp;
									CvString strLogString;
									strTemp = pCivilian->getUnitInfo().GetDescription();
									strLogString.Format("Moving %s to camp, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
							}
						}

						// Can't reach target and don't have escort...
						else if(!pEscort)
						{
							ExecuteMoveToPlot(pCivilian, pCivilianMove);
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strTemp;
								CvString strLogString;
								strTemp = pCivilian->getUnitInfo().GetDescription();
								strLogString.Format("Moving %s without escort to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY());
								LogTacticalMessage(strLogString);
							}
						}

						// Can't reach target and DO have escort...
						else
						{
							// See if escort can move to the same location in one turn
							if(TurnsToReachTarget(pEscort, pCivilianMove) <= 1)
							{
								ExecuteMoveToPlot(pEscort, pCivilianMove);
								ExecuteMoveToPlot(pCivilian, pCivilianMove);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strTemp;
									CvString strLogString;
									strTemp = pEscort->getUnitInfo().GetDescription();
									strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
									LogTacticalMessage(strLogString);
									strTemp = pCivilian->getUnitInfo().GetDescription();
									strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
									LogTacticalMessage(strLogString);
								}
							}

							else
							{
								UnitHandle pBlockingUnit = pCivilianMove->getBestDefender(m_pPlayer->GetID());

								// See if friendly blocking unit is ending the turn there, or if no blocking unit (which indicates this is somewhere civilian
								// can move that escort can't), then find a new path based on moving the escort
								if(!pBlockingUnit || pBlockingUnit->getMoves() == 0)
								{
									if(!pEscort->GeneratePath(pTarget, 0, false /*bReuse*/))
									{
										pCivilian->finishMoves();
										pEscort->finishMoves();
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strLogString;
											strLogString.Format("Retargeting civilian escort operation (path lost to target), X: %d, Y: %d", pTarget->getX(), pTarget->getY());
											LogTacticalMessage(strLogString);
										}
									}
									else
									{
										pEscortMove = pCivilian->GetPathEndTurnPlot();

										// See if civilian can move to the same location in one turn
										if(TurnsToReachTarget(pCivilian, pEscortMove) <= 1)
										{
											ExecuteMoveToPlot(pEscort, pEscortMove);
											ExecuteMoveToPlot(pCivilian, pEscortMove);
											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strTemp;
												CvString strLogString;
												strTemp = pEscort->getUnitInfo().GetDescription();
												strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
												LogTacticalMessage(strLogString);
												strTemp = pCivilian->getUnitInfo().GetDescription();
												strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
												LogTacticalMessage(strLogString);
											}
										}
										else
										{
											pCivilian->finishMoves();
											pEscort->finishMoves();
											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strLogString;
												strLogString.Format("Retargeting civilian escort operation. Too many blocking units.");
												LogTacticalMessage(strLogString);
											}
										}
									}
								}

								// Looks like we should be able to move the blocking unit out of the way
								else
								{
									if(ExecuteMoveOfBlockingUnit(pBlockingUnit))
									{
										ExecuteMoveToPlot(pEscort, pCivilianMove);
										ExecuteMoveToPlot(pCivilian, pCivilianMove);
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strTemp;
											CvString strLogString;
											strTemp = pEscort->getUnitInfo().GetDescription();
											strLogString.Format("Moving escorting %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
											LogTacticalMessage(strLogString);
											strTemp = pCivilian->getUnitInfo().GetDescription();
											strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
											LogTacticalMessage(strLogString);
										}
									}
									else
									{
										pCivilian->finishMoves();
										pEscort->finishMoves();
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strLogString;
											strLogString.Format("Retargeting civilian escort operation. Could not move blocking unit.");
											LogTacticalMessage(strLogString);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

/// Move unit to protect a specific tile (retrieve unit from first entry in m_CurrentMoveUnits)
void CvTacticalAI::ExecuteMoveToPlot(CvPlot* pTarget, bool bSaveMoves)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		ExecuteMoveToPlot(pUnit, pTarget, bSaveMoves);
	}
}

/// Move unit to protect a specific tile (retrieve unit from first entry in m_CurrentMoveUnits)
void CvTacticalAI::ExecuteMoveToPlot(UnitHandle pUnit, CvPlot* pTarget, bool bSaveMoves)
{
	// Unit already at target plot?
	if(pTarget == pUnit->plot())
	{
		// Fortify if possible
		if(pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
		}
		else
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			if(!bSaveMoves)
			{
				pUnit->finishMoves();
			}
		}
	}

	else
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), MOVE_UNITS_IGNORE_DANGER);
		if(!bSaveMoves)
		{
			pUnit->finishMoves();
		}
	}

	UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());
}

/// Find an adjacent hex to move a blocking unit to
bool CvTacticalAI::ExecuteMoveOfBlockingUnit(UnitHandle pBlockingUnit)
{
	if(!pBlockingUnit->canMove() || IsInQueuedAttack(pBlockingUnit.pointer()))
	{
		return false;
	}

	CvPlot* pOldPlot = pBlockingUnit->plot();

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pPlot = plotDirection(pBlockingUnit->getX(), pBlockingUnit->getY(), ((DirectionTypes)iI));
		if(pPlot != NULL)
		{
			// Don't embark for one of these moves
			if (!pOldPlot->isWater() && pPlot->isWater() && pBlockingUnit->getDomainType() == DOMAIN_LAND)
			{
				continue;
			}

			// Has to be somewhere we can move and be empty of other units/enemy cities
			if(!pPlot->getVisibleEnemyDefender(m_pPlayer->GetID()) && !pPlot->isEnemyCity(*pBlockingUnit.pointer()) && pBlockingUnit->GeneratePath(pPlot))
			{
				ExecuteMoveToPlot(pBlockingUnit, pPlot);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp, strLogString;
					strTemp = pBlockingUnit->getUnitInfo().GetDescription();
					strLogString.Format("Moving blocking %s out of way, Leaving X: %d, Y: %d, Now At X: %d, Y: %d", strTemp.GetCString(), pOldPlot->getX(), pOldPlot->getY(), pBlockingUnit->getX(), pBlockingUnit->getY());
					LogTacticalMessage(strLogString);
				}
				return true;
			}
		}
	}
	return false;
}

/// Move unit to protect a specific tile
void CvTacticalAI::ExecuteNavalBlockadeMove(CvPlot* pTarget)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
		pUnit->finishMoves();
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
		pUnit->SetTacticalAIPlot(NULL);
	}
}

/// Find one unit to move to target, starting with high priority list
void CvTacticalAI::ExecuteMoveToTarget(CvPlot* pTarget)
{
	std::vector<CvTacticalUnit>::iterator it;

	// Start with high priority list
	for(it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		// Don't move high priority unit if regular priority unit is closer
		if(m_CurrentMoveUnits.size() > 0 && m_CurrentMoveUnits.begin()->GetMovesToTarget() < it->GetMovesToTarget())
		{
			break;
		}

		if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
			UnitProcessed(it->GetID());
			return;
		}

		else if(pUnit && it->GetMovesToTarget() < MAX_INT)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(it->GetID());
			return;
		}
	}

	// Then regular priority
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
			UnitProcessed(it->GetID());
			return;
		}

		else if(pUnit && it->GetMovesToTarget() < MAX_INT)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(it->GetID());
			return;
		}
	}
}

/// Set up fighters to intercept enemy air units
void CvTacticalAI::ExecuteAirInterceptMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirPatrol(NULL))
			{
				pUnit->PushMission(CvTypes::getMISSION_AIRPATROL());
				UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false /*bMarkTacticalMap*/);
			}
		}
	}
}

/// Set up fighters to air sweep to suppress enemy air units/AA
void CvTacticalAI::ExecuteAirSweepMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirSweep())
			{
				CvPlot *pTarget = m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit.pointer());
				if (pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_AIR_SWEEP(), pTarget->getX(), pTarget->getY());
					UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false /*bMarkTacticalMap*/);
				}
			}
		}
	}
}

/// Bombard enemy units from plots they can't reach (return true if some attack made)
bool CvTacticalAI::ExecuteSafeBombards(CvTacticalTarget& kTarget)
{
	CvTacticalUnit unit;
	void* pFirstAttacker = NULL;
	UnitHandle pDefender;
	int iRequiredDamage = 0;
	bool bCityCanAttack = false;
	bool bFirstAttackCity = false;

	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	if(PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return false;
	}

	// Get required damage on unit target
	pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if(pDefender)
	{
		iRequiredDamage = pDefender->GetCurrHitPoints();

		// If this is a unit target we might also be able to hit it with a city
		bCityCanAttack = FindCitiesWithinStrikingDistance(pTargetPlot);
		if(bCityCanAttack)
		{
			ComputeTotalExpectedBombardDamage(pDefender);

			// Start by applying damage from city bombards
			for(unsigned int iI = 0; iI < m_CurrentMoveCities.size() && iRequiredDamage > 0; iI++)
			{
				CvCity* pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
				if(pCity != NULL)
				{
					if(QueueAttack(pCity, &kTarget, true /*bRanged*/, true /*bCity*/))
					{
						pFirstAttacker = (void*)pCity;
						bFirstAttackCity = true;
					}

					// Subtract off expected damage
					iRequiredDamage -= m_CurrentMoveCities[iI].GetExpectedTargetDamage();
				}
			}
		}
	}

	// Get required damage on city target
	else
	{
		CvCity* pCity = pTargetPlot->getPlotCity();
		if(pCity != NULL)
		{
			iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();

			// Can't eliminate a city with ranged fire, so don't target one if that low on health
			if(iRequiredDamage <= 1)
			{
				return false;
			}
		}
	}

	// Need to keep hitting target?
	if(iRequiredDamage <= 0)
	{
		return false;
	}

	// For each of our ranged units, see if they are already in a plot that can bombard that can't be attacked.
	// If so, bombs away!
	m_CurrentMoveUnits.clear();
	list<int>::iterator it;
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && pUnit->IsCanAttackRanged() && !pUnit->isOutOfAttacks())
		{
			int iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
			CvTacticalAnalysisCell* pCell = GC.getGame().GetTacticalAnalysisMap()->GetCell(iPlotIndex);
			if(pCell->IsWithinRangeOfTarget() && !pCell->IsSubjectToAttack() && IsExpectedToDamageWithRangedAttack(pUnit, pTargetPlot))
			{
				if(pUnit->canSetUpForRangedAttack(NULL))
				{
					pUnit->setSetUpForRangedAttack(true);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Set up %s for ranged attack", pUnit->getName().GetCString());
						LogTacticalMessage(strMsg, false);
					}
				}

				if(pUnit->canMove() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Making a safe bombard (no move) with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
						              kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}

					if(QueueAttack((void*)pUnit.pointer(), &kTarget, true /*bRanged*/, false /*bCity*/))
					{
						pFirstAttacker = (void*)pUnit.pointer();
					}

					// Save off ID so can be cleared from list to process for turn
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
			}
		}
	}

	// Clear out the units we just processed from the list for this turn
	std::vector<CvTacticalUnit>::iterator currMoveIt;
	for(currMoveIt = m_CurrentMoveUnits.begin(); currMoveIt != m_CurrentMoveUnits.end(); ++currMoveIt)
	{
		UnitProcessed(currMoveIt->GetID());
	}

	// For each plot that we can bombard from that the enemy can't attack, try and move a ranged unit there.
	// If so, make that move and mark that tile as blocked with our unit.  If unit has movement left, queue up an attack
	int iDX, iDY;
	CvPlot* pLoopPlot;
	int iPlotIndex;
	CvTacticalAnalysisCell* pCell;

	for(int iTurnsToReach = 0; iTurnsToReach <= 2; iTurnsToReach++)
	{
		int iRange = m_pMap->GetBestFriendlyRange();

		for(iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for(iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot = plotXY(kTarget.GetTargetX(), kTarget.GetTargetY(), iDX, iDY);
				if(pLoopPlot != NULL)
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
					if(iDistance > 0 && iDistance <= iRange)
					{
						iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
						pCell = m_pMap->GetCell(iPlotIndex);
						if(pCell->IsRevealed() && pCell->CanUseForOperationGathering())
						{
							if(pCell->IsWithinRangeOfTarget() && !pCell->IsSubjectToAttack())
							{
								bool bHaveLOS = pLoopPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
								if(FindClosestUnit(pLoopPlot, iTurnsToReach, false/*bMustHaveHalfHP*/, true/*bMustBeRangedUnit*/, iDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/, true/*bIgnoreUnits*/, pTargetPlot))
								{
									UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
									if(pUnit)
									{
										// Check for presence of unmovable friendly units
										UnitHandle pBlockingUnit = pLoopPlot->getBestDefender(m_pPlayer->GetID());
										if(!pBlockingUnit || ExecuteMoveOfBlockingUnit(pBlockingUnit))
										{
											pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlot->getX(), pLoopPlot->getY());

											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strMsg;
												strMsg.Format("Moving closer for safe bombard with %s, Target X: %d, Y: %d, Bombard From X: %d, Y: %d, Now At X: %d, Y: %d", pUnit->getName().GetCString(),
												              kTarget.GetTargetX(), kTarget.GetTargetY(), pLoopPlot->getX(), pLoopPlot->getY(), pUnit->getX(), pUnit->getY());
												LogTacticalMessage(strMsg, false);
											}

											UnitProcessed(m_CurrentMoveUnits[0].GetID());

											if(pUnit->canSetUpForRangedAttack(NULL))
											{
												pUnit->setSetUpForRangedAttack(true);
												if(GC.getLogging() && GC.getAILogging())
												{
													CvString strMsg;
													strMsg.Format("Set up %s for ranged attack", pUnit->getName().GetCString());
													LogTacticalMessage(strMsg, false);
												}
											}

											if(pUnit->canMove() && !pUnit->isOutOfAttacks() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
											{
												if(GC.getLogging() && GC.getAILogging())
												{
													CvString strMsg;
													strMsg.Format("Making a safe bombard (half move) with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
													              kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
													LogTacticalMessage(strMsg, false);
												}

												if(QueueAttack((void*)pUnit.pointer(), &kTarget, true /*bRanged*/, false /*bCityAttack*/))
												{
													pFirstAttacker = (void*)pUnit.pointer();
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Launch the initial attack plotted
	if(pFirstAttacker)
	{
		LaunchAttack(pFirstAttacker, &kTarget, true /*bFirst*/, true /*bFirstAttackRanged*/, bFirstAttackCity);
		return true;
	}
	return false;
}

/// Bombard an enemy target from plots we can protect from enemy attack (return true if some attack made)
bool CvTacticalAI::ExecuteProtectedBombards(CvTacticalTarget& kTarget, bool bAttackUnderway)
{
	bool bAttackMade = true;
	bool bAtLeastOneAttackInitiated = false;

	while(bAttackMade && kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		bAttackMade = ExecuteOneProtectedBombard(kTarget);
		if(bAttackMade)
		{
			bAttackUnderway = true;
			bAtLeastOneAttackInitiated = true;
		}
	}
	return bAtLeastOneAttackInitiated;
}

/// Bombard an enemy target from a single plot we can protect from enemy attack (return true if some attack made)
bool CvTacticalAI::ExecuteOneProtectedBombard(CvTacticalTarget& kTarget)
{
	UnitHandle pFirstAttacker;
	int iDX, iDY;
	CvPlot* pAttackPlot;
	int iPlotIndex;
	CvTacticalAnalysisCell* pCell;
	int iPriority;
	int iNumTurns;
	int iNumUnitsRequiredToCover;

	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	if(PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return false;
	}

	CvCity* pCity = pTargetPlot->getPlotCity();
	if(pCity != NULL)
	{
		int iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();

		// Can't eliminate a city with ranged fire, so don't target one if that low on health
		if(iRequiredDamage <= 1)
		{
			return false;
		}
	}

	int iRange = m_pMap->GetBestFriendlyRange();
	m_TempTargets.clear();

	// Build a list of all plots that have LOS to target where no enemy unit is adjacent
	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pAttackPlot = plotXY(kTarget.GetTargetX(), kTarget.GetTargetY(), iDX, iDY);
			if(pAttackPlot != NULL)
			{
				int iPlotDistance = plotDistance(pAttackPlot->getX(), pAttackPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if(iPlotDistance > 0 && iPlotDistance <= iRange)
				{
					iPlotIndex = GC.getMap().plotNum(pAttackPlot->getX(), pAttackPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					if(pCell->IsRevealed() && pCell->CanUseForOperationGathering())
					{
						if(pCell->IsWithinRangeOfTarget())
						{
							// Check for adjacent enemy unit
							CvPlot* pLoopPlot;
							bool bNoEnemyAdjacent = true;
							for(int iI = 0; iI < NUM_DIRECTION_TYPES && bNoEnemyAdjacent; iI++)
							{
								pLoopPlot = plotDirection(pAttackPlot->getX(), pAttackPlot->getY(), ((DirectionTypes)iI));
								if(pLoopPlot != NULL)
								{
									iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
									if(m_pMap->GetCell(iPlotIndex)->GetEnemyMilitaryUnit() != NULL)
									{
										bNoEnemyAdjacent = false;
									}
								}
							}

							if(bNoEnemyAdjacent)
							{
								// Do we have a unit that can get off a bombard from here THIS turn
								iNumTurns = -1;
								bool bHaveLOS = pAttackPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
								if(FindClosestUnit(pAttackPlot, 0, false/*bMustHaveHalfHP*/, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/,  false/*bIgnoreUnits*/, pTargetPlot))
								{
									iNumTurns = 0;
								}
								// What about next turn?
								else if(FindClosestUnit(pAttackPlot, 1, false/*bMustHaveHalfHP*/, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/,  false/*bIgnoreUnits*/,  pTargetPlot))
								{
									iNumTurns = 1;
								}

								// If found a unit that could get here, see if we can cover the hex from enemy attack
								if(iNumTurns >= 0)
								{
									if(CanCoverFromEnemy(pAttackPlot, iNumUnitsRequiredToCover, m_CurrentMoveUnits[0].GetID()))
									{
										CvTacticalTarget target;
										target.SetTargetX(pAttackPlot->getX());
										target.SetTargetY(pAttackPlot->getY());

										// How desirable is this move?
										// Set up math so having to allocate 3 extra units to defend is worse than waiting a turn to attack
										iPriority = 300 - (iNumTurns * 100);
										iPriority -= 40 * iNumUnitsRequiredToCover;
										target.SetAuxIntData(iPriority);
										m_TempTargets.push_back(target);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// No plots to shoot from?
	if(m_TempTargets.size() == 0)
	{
		return false;
	}

	// Sort potential spots
	std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());

	// Have to rebuild blocking position info for this specific spot
	pAttackPlot = GC.getMap().plot(m_TempTargets[0].GetTargetX(), m_TempTargets[0].GetTargetY());
	int iPlotDistance = plotDistance(pAttackPlot->getX(), pAttackPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
	bool bHaveLOS = pAttackPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
	if(FindClosestUnit(pAttackPlot, 0, false/*bMustHaveHalfHP*/, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/,  false/*bIgnoreUnits*/, pTargetPlot) ||
	        FindClosestUnit(pAttackPlot, 1, false/*bMustHaveHalfHP*/, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/,  false/*bIgnoreUnits*/, pTargetPlot))
	{
		int iAttackingUnitID = m_CurrentMoveUnits[0].GetID();
		if(pAttackPlot && CanCoverFromEnemy(pAttackPlot, iNumUnitsRequiredToCover, iAttackingUnitID))
		{
			// Make each blocking move
			for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
			{
				CvUnit* pChosenUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
				if(pChosenUnit)
				{
					if (pChosenUnit->plot() != m_ChosenBlocks[iI].GetPlot())
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Moving to cover a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
							LogTacticalMessage(strMsg, false);
						}
						pChosenUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					}
					else if (pChosenUnit->canPillage(pChosenUnit->plot()) && pChosenUnit->getDamage() > 0)
					{
						pChosenUnit->PushMission(CvTypes::getMISSION_PILLAGE());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Pillaging during a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
							LogTacticalMessage(strMsg, false);
						}
					}
					else if(pChosenUnit->canFortify(pChosenUnit->plot()))
					{
						pChosenUnit->PushMission(CvTypes::getMISSION_FORTIFY());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Fortifying during a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
							LogTacticalMessage(strMsg, false);
						}
					}
					else
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Sitting during a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
							LogTacticalMessage(strMsg, false);
						}
					}
					pChosenUnit->finishMoves();
					UnitProcessed(pChosenUnit->GetID());
				}
			}

			// Then move the attacking unit
			CvUnit* pUnit = m_pPlayer->getUnit(iAttackingUnitID);
			if(pUnit)
			{
				if(pAttackPlot != pUnit->plot())
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pAttackPlot->getX(), pAttackPlot->getY());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Moving closer for protected bombard with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
						              pAttackPlot->getX(), pAttackPlot->getY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}
				}

				UnitProcessed(iAttackingUnitID);

				if(pUnit->canSetUpForRangedAttack(NULL))
				{
					pUnit->setSetUpForRangedAttack(true);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Set up %s for ranged attack", pUnit->getName().GetCString());
						LogTacticalMessage(strMsg, false);
					}
				}

				if(pUnit->canMove() && !pUnit->isOutOfAttacks() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Making a protected bombard with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
						              kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}

					if(QueueAttack((void*)pUnit, &kTarget, true /*bRanged*/, false /*bCityAttack*/))
					{
						pFirstAttacker = pUnit;
					}
				}
			}
		}

		if(pFirstAttacker)
		{
			LaunchAttack((void*)pFirstAttacker.pointer(), &kTarget, true /*bFirst*/, true /*bFirstAttackRanged*/, false /*bCityAttack*/);
			return true;
		}
	}

	return false;
}

/// Take a multi-hex attack on an enemy unit this turn
bool CvTacticalAI::ExecuteFlankAttack(CvTacticalTarget& kTarget)
{
	CvTacticalAnalysisCell* pCell;
	CvPlot* pLoopPlot;
	int iPlotIndex;
	int iPossibleFlankHexes = 0;
	CvTacticalTarget target;

	// Count number of possible flank attack spaces around target
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(kTarget.GetTargetX(), kTarget.GetTargetY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL)
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);
			if(pCell->IsHelpsProvidesFlankBonus())
			{
				iPossibleFlankHexes++;
			}
		}
	}

	// If more than 1, find how many we can fill with units
	if(iPossibleFlankHexes > 1)
	{
		int iFillableHexes = 0;
		int iNumAttackers = 0;
		m_TempTargets.clear();
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(kTarget.GetTargetX(), kTarget.GetTargetY(), ((DirectionTypes)iI));
			if(pLoopPlot != NULL)
			{
				iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
				pCell = m_pMap->GetCell(iPlotIndex);
				if(pCell->IsHelpsProvidesFlankBonus())
				{
					if(FindClosestUnit(pLoopPlot, 0, true/*bMustHaveHalfHP*/, false /*bMustBeRangedUnit*/, 0 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
					{
						iFillableHexes++;
						iNumAttackers++;
						target.SetTargetX(pLoopPlot->getX());
						target.SetTargetY(pLoopPlot->getY());
						m_TempTargets.push_back(target);
					}
					// What about next turn?
					else if(FindClosestUnit(pLoopPlot, 1, false/*bMustHaveHalfHP*/, false /*bMustBeRangedUnit*/, 0 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
					{
						iFillableHexes++;
						target.SetTargetX(pLoopPlot->getX());
						target.SetTargetY(pLoopPlot->getY());
						m_TempTargets.push_back(target);
					}
				}
			}
		}

		// As long as we either get three hexes filled with one attacker, or two we can attack from, then this multi-hex attack is worth considering
		if((iNumAttackers >= 1 && iFillableHexes >= 3) || iNumAttackers >= 2)
		{
			// Compute best way to fill the hexes
			m_PotentialBlocks.clear();
			for(unsigned int iI = 0; iI < m_TempTargets.size(); iI++)
			{
				pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(!FindClosestUnit(pLoopPlot, 1, false/*bMustHaveHalfHP*/, false /*bMustBeRangedUnit*/, 0 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
				{
					iFillableHexes--;
					if(iFillableHexes < 2)
					{
						return false;
					}
				}
				else
				{
					// Save off the units that could get here
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}
				}
			}

			if(AssignFlankingUnits(iFillableHexes))
			{
				// Make moves up into hexes
				for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
				{
					CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
					if(pUnit)
					{
						if(pUnit->plot() == m_ChosenBlocks[iI].GetPlot())
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Already in a flanking position with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
								              m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
								LogTacticalMessage(strMsg, false);
							}
						}
						else
						{
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Moving into a flanking position with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
								              m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
								LogTacticalMessage(strMsg, false);
							}

							if(pUnit->getMoves() <= 0)
							{
								UnitProcessed(pUnit->GetID());
							}
						}
					}
				}

				// Make attacks
				CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
				UnitHandle pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if(pDefender)
				{
					kTarget.SetAuxIntData(pDefender->GetCurrHitPoints());
					m_CurrentMoveCities.clear();
					if(FindUnitsWithinStrikingDistance(pTargetPlot, 1, 0, false /* bNoRangedUnits */))
					{
						ComputeTotalExpectedDamage(&kTarget, pTargetPlot);
						ExecuteAttack(&kTarget, pTargetPlot, false/*bInflictWhatWeTake*/, true/*bMustSurviveAttack*/);
					}
				}
			}
		}
	}

	return false;
}

/// Move forces in toward our target
void CvTacticalAI::ExecuteCloseOnTarget(CvTacticalTarget& kTarget, CvTacticalDominanceZone* pZone)
{
	CvOperationUnit unit;
	int iDistance;
	CvPlot* pTargetPlot;
	int iRangedUnits = 0;
	int iMeleeUnits = 0;
	int iGenerals = 0;
	list<int>::iterator it;
	int iTacticalRadius = GC.getGame().GetTacticalAnalysisMap()->GetTacticalRange();

	pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	m_OperationUnits.clear();
	m_GeneralsToMove.clear();

	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// If not naval invasion, proper domain of unit?
			if(pZone->IsNavalInvasion() ||
			        (pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND))
			{	
				// Find units really close to target or somewhat close that just came out of an operation
				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if(iDistance <= iTacticalRadius || (iDistance <= (GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 3) && pUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn()))
				{
					unit.SetUnitID(pUnit->GetID());

					if(pUnit->IsCanAttackRanged())
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_BOMBARD]);
						iRangedUnits++;
						m_OperationUnits.push_back(unit);

					}
					else if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
						iGenerals++;
						m_GeneralsToMove.push_back(unit);
					}
					else
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_FRONT_LINE]);
						iMeleeUnits++;
						m_OperationUnits.push_back(unit);
					}
				}
			}
		}
	}

	// If have any units to move...
	if(m_OperationUnits.size() > 0)
	{
		/* Land only unless invasion or no enemy naval presence */
		bool bLandOnly = true;
		if (pZone->IsNavalInvasion() || pZone->GetEnemyNavalUnitCount() == 0)
		{
			bLandOnly = false;
		}
		ScoreCloseOnPlots(pTargetPlot, bLandOnly);

		// Compute the moves to get the best deployment
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();

		int iRangedUnitsToPlace = iRangedUnits;
		int iMeleeUnitsToPlace = iMeleeUnits;

		// First loop for ranged unit spots
		bool bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(FindClosestOperationUnit(pLoopPlot, true /*bSafeForRanged*/, true /*bMustBeRangedUnit*/))
				{
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}

					iRangedUnitsToPlace--;
					if(iRangedUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iRangedUnits - iRangedUnitsToPlace);
		PerformChosenMoves();

		// Second loop for everyone else (including remaining ranged units)
		m_PotentialBlocks.clear();
		iMeleeUnits += iRangedUnitsToPlace;
		iMeleeUnitsToPlace += iRangedUnitsToPlace;
		bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(FindClosestOperationUnit(pLoopPlot, true /*bSafeForRanged*/, false /*bMustBeRangedUnit*/))
				{
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}

					iMeleeUnitsToPlace--;
					if(iMeleeUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iMeleeUnits - iMeleeUnitsToPlace);
		PerformChosenMoves();
	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral();
	}
}

/// Deploy defensive forces around a friendly city
void CvTacticalAI::ExecuteHedgehogDefense(CvTacticalTarget& kTarget, CvTacticalDominanceZone* pZone)
{
	CvOperationUnit unit;
	int iDistance;
	CvPlot* pTargetPlot;
	int iRangedUnits = 0;
	int iMeleeUnits = 0;
	list<int>::iterator it;
	int iTacticalRadius = GC.getGame().GetTacticalAnalysisMap()->GetTacticalRange();

	pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	m_OperationUnits.clear();
	m_GeneralsToMove.clear();

	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Proper domain of unit?
			if(pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{
				// Find units really close to target or somewhat close that just came out of an operation
				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if(iDistance <= iTacticalRadius)
				{
					unit.SetUnitID(pUnit->GetID());

					if (pUnit->IsGreatGeneral())
					{
						if(pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
							m_GeneralsToMove.push_back(operationUnit);
						}
					}
					else if (pUnit->IsCanAttackRanged())
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_BOMBARD]);
						iRangedUnits++;
						m_OperationUnits.push_back(unit);
					}
					else
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_FRONT_LINE]);
						iMeleeUnits++;
						m_OperationUnits.push_back(unit);
					}
				}
			}
		}
	}

	// If have any units to move...
	if(m_OperationUnits.size() > 0)
	{
		ScoreHedgehogPlots(pTargetPlot);

		// Compute the moves to get the best deployment
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();

		int iRangedUnitsToPlace = iRangedUnits;
		int iMeleeUnitsToPlace = iMeleeUnits;

		// First loop for ranged unit spots
		bool bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(FindClosestOperationUnit(pLoopPlot, true /*bSafeForRanged*/, true /*bMustBeRangedUnit*/))
				{
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}

					iRangedUnitsToPlace--;
					if(iRangedUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iRangedUnits - iRangedUnitsToPlace);
		PerformChosenMoves();

		// Second loop for everyone else (including remaining ranged units)
		m_PotentialBlocks.clear();
		iMeleeUnits += iRangedUnitsToPlace;
		iMeleeUnitsToPlace += iRangedUnitsToPlace;
		bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(FindClosestOperationUnit(pLoopPlot, true /*bSafeForRanged*/, false /*bMustBeRangedUnit*/))
				{
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());
						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}

					iMeleeUnitsToPlace--;
					if(iMeleeUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}
		}
		AssignDeployingUnits(iMeleeUnits - iMeleeUnitsToPlace);
		PerformChosenMoves();
	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral();
	}
}

/// Bombard and flank attacks (whatever is applicable) against a unit target
void CvTacticalAI::ExecutePriorityAttacksOnUnitTarget(CvTacticalTarget& kTarget)
{
	bool bAttackUnderway = false;
	bool bAttackMade = false;
	CvPlot* pTarget;

	pTarget = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());

	// Try to find a bombard first
	m_pMap->ClearDynamicFlags();
	m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

	if(kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		bAttackUnderway = ExecuteSafeBombards(kTarget);
	}

	if(kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		ExecuteProtectedBombards(kTarget, bAttackUnderway);
	}

	// Then try for a flank attack
	if(kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		m_pMap->SetTargetFlankBonusCells(pTarget);

		bAttackMade = ExecuteFlankAttack(kTarget);
		if(bAttackMade)
		{
			bAttackUnderway = true;
		}
	}
}

/// Move units out of current dominance zone
void CvTacticalAI::ExecuteWithdrawMoves()
{
	CvCity* pNearestCity;
	int iTurnsToReachTarget;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			// Compute moves to nearest city and use as sort criteria
			pNearestCity = GC.getMap().findCity(pUnit->getX(), pUnit->getY(), m_pPlayer->GetID(), NO_TEAM, true /* bSameArea */);
			if(pNearestCity != NULL)
			{
				iTurnsToReachTarget = TurnsToReachTarget(pUnit, pNearestCity->plot());
				m_CurrentMoveUnits[iI].SetMovesToTarget(iTurnsToReachTarget);
				m_CurrentMoveUnits[iI].SetAttackStrength(1000-iTurnsToReachTarget);
				m_CurrentMoveUnits[iI].SetHealthPercent(10,10);
			}
		}
	}

	// Sort units by distance to city so closest ones move first
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	// Execute the moves
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			// Compute moves to nearest city and use as sort criteria
			pNearestCity = GC.getMap().findCity(pUnit->getX(), pUnit->getY(), m_pPlayer->GetID(), NO_TEAM, true /* bSameArea */);
			if(pNearestCity != NULL)
			{
				MoveToEmptySpaceNearTarget(pUnit, pNearestCity->plot(), (pUnit->getDomainType()==DOMAIN_LAND));

				pUnit->finishMoves();
				UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s withdrew toward %s, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pNearestCity->getName().GetCString(),
					                    pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString, false);
				}
			}
		}
	}
}

/// Move naval units on top of embarked units in danger
void CvTacticalAI::ExecuteEscortEmbarkedMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			CvPlot *pBestTarget = NULL;
			int iHighestDanger = -1;
			int iMovementRate = pUnit->getMoves() / GC.getMOVE_DENOMINATOR();

			// Loop through all my embarked units that are: alone, have already moved this turn, within range
			UnitHandle pLoopUnit;
			int iLoop;
			for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
			{
				if (pLoopUnit->getDomainType() != DOMAIN_LAND)
				{
					continue;
				}
				else if (!pLoopUnit->isEmbarked())
				{
					continue;
				}
				else if (pLoopUnit->plot()->getNumUnits() > 1)
				{
					continue;
				}
#if defined(MOD_BALANCE_CORE_MILITARY)
				//Even if it can move, we can still keep up this way. Otherwise naval units not already 'in' the group can hardly ever join.
#else
				else if (pLoopUnit->canMove())
				{
					continue;
				}
#endif
				else if (plotDistance(pUnit->getX(), pUnit->getY(), pLoopUnit->getX(), pLoopUnit->getY()) > iMovementRate)
				{
					continue;
				}

				// Can this unit get to the embarked unit in one move?
				CvPlot *pTarget = pLoopUnit->plot();
				if (TurnsToReachTarget(pUnit, pTarget) <= 1)
				{
					int iDanger = m_pPlayer->GetPlotDanger(*pTarget);
					if (iDanger > iHighestDanger)
					{
						iHighestDanger = iDanger;
						pBestTarget = pTarget;
					}
				}
			}

			if (pBestTarget)
			{
				ExecuteMoveToPlot(pUnit, pBestTarget);
				UnitProcessed(m_CurrentMoveUnits[iI].GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s escorted embarked unit at, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString, false);
				}
			}
		}
	}
}

#if defined(MOD_AI_SMART_RANGED_UNITS)
// Iterate through available plots and get the best one to later move at.
void CvTacticalAI::GetBestPlot(CvPlot*& outputPlot, vector<CvPlot*> plotsToCheck)
{
	int minDanger = 0;
	std::vector<CvPlot*>::iterator it;

	for (it = plotsToCheck.begin(); it != plotsToCheck.end(); it++)
	{
		CvPlot* pPlot = (*it);
		int currentDanger = m_pPlayer->GetPlotDanger(*pPlot);

		if (minDanger == 0 || (currentDanger < minDanger))
		{
			outputPlot = pPlot;
			minDanger = currentDanger;
		}
	}
}

// helper function to iterate vector that is of CvPlot Type.
bool CvTacticalAI::ContainsPlot(vector<CvPlot*> plotData, CvPlot* plotXy)
{
	bool methodResult = false;
	std::vector<CvPlot*>::iterator it;

	for (it = plotData.begin(); it != plotData.end(); it++)
	{
		if (((*it)->getX() == plotXy->getX()) && ((*it)->getY() == plotXy->getY()))
		{
			methodResult = true;
			break;
		}
	}

	return methodResult;
}

// Get best plot of the array of possible plots, based on plot danger.
CvPlot* CvTacticalAI::GetBestRepositionPlot(UnitHandle unitH, CvPlot* plotTarget)
{
	CvPlot* pBestRepositionPlot = NULL;
	std::vector<CvPlot*> movePlotList;
	unitH -> GetMovablePlotListOpt(movePlotList, plotTarget, false);

	if(movePlotList.size() >= 1)
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Ranged unit out of range to target has found %d, alternate tiles to move and fire", movePlotList.size());
			LogTacticalMessage(strMsg);
		}

		GetBestPlot(pBestRepositionPlot, movePlotList);
	}

	return pBestRepositionPlot;
}
#endif

/// Disable a move (probably because it is incompatible with a posture chosen)
void CvTacticalAI::TurnOffMove(TacticalAIMoveTypes eType)
{
	FStaticVector<CvTacticalMove, 256, true, c_eCiv5GameplayDLL >::iterator it;

	for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
	{
		if(it->m_eMoveType == eType)
		{
			it->m_iPriority = -1;
			return;
		}
	}
}

/// Finds both high and normal priority units we can use for this move (returns true if at least 1 unit found)
bool CvTacticalAI::FindUnitsForThisMove(TacticalAIMoveTypes eMove, CvPlot* pTarget, int iNumTurnsAway /* = -1 if any distance okay */, bool bRangedOnly)
{
	UnitHandle pLoopUnit;
	bool rtnValue = false;

	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	m_CurrentMoveHighPriorityUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->getDomainType() != DOMAIN_AIR && pLoopUnit->IsCombatUnit())
		{
			// Make sure domain matches
			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
			        pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				continue;
			}

			bool bSuitableUnit = false;
			bool bHighPriority = false;

			if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN])
			{
				// Want to put ranged units in cities to give them a ranged attack
				if(pLoopUnit->isRanged())
				{
					bSuitableUnit = true;
					bHighPriority = true;
				}

				else if(bRangedOnly)
				{
					continue;
				}

				// Don't put units with a combat strength boosted from promotions in cities, these boosts are ignored
				if(pLoopUnit->getDefenseModifier() == 0 &&
				        pLoopUnit->getAttackModifier() == 0 &&
				        pLoopUnit->getExtraCombatPercent() == 0)
				{
					bSuitableUnit = true;
				}
			}

			else if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_1_TURN] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN])
			{
				// No ranged units or units without defensive bonuses as plot defenders
				if(!pLoopUnit->isRanged() && !pLoopUnit->noDefensiveBonus())
				{
					bSuitableUnit = true;

					// Units with defensive promotions are especially valuable
					if(pLoopUnit->getDefenseModifier() > 0 || pLoopUnit->getExtraCombatPercent() > 0)
					{
						bHighPriority = true;
					}
				}
			}

			else if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS])
			{
				// Fast movers are top priority
				if(pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
				{
					bSuitableUnit = true;
					bHighPriority = true;
				}
				else if(pLoopUnit->IsCanAttack())
				{
					bSuitableUnit = true;
				}
			}
			if(bSuitableUnit)
			{
				// Is it even possible for the unit to reach in the number of requested turns (ignoring roads and RR)
				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());
				if(pLoopUnit->maxMoves() > 0)
				{
					int iMovesPerTurn = pLoopUnit->maxMoves() / GC.getMOVE_DENOMINATOR();
					int iLeastTurns = (iDistance + iMovesPerTurn - 1) / iMovesPerTurn;
					if(iNumTurnsAway == -1 || iLeastTurns <= iNumTurnsAway)
					{
						// If unit was suitable, and close enough, add it to the proper list
						int iMoves = TurnsToReachTarget(pLoopUnit, pTarget);
						if(iMoves != MAX_INT && (iNumTurnsAway == -1 ||
						                         (iNumTurnsAway == 0 && pLoopUnit->plot() == pTarget) || iMoves <= iNumTurnsAway))
						{
							CvTacticalUnit unit;
							unit.SetID(pLoopUnit->GetID());
							unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
							unit.SetMovesToTarget(iMoves);

							if(bHighPriority)
							{
								m_CurrentMoveHighPriorityUnits.push_back(unit);
							}
							else
							{
								m_CurrentMoveUnits.push_back(unit);
							}
							rtnValue = true;
						}
					}
				}
			}
		}
	}

	return rtnValue;
}

/// Fills m_CurrentMoveUnits with all units within X turns of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindUnitsWithinStrikingDistance(CvPlot* pTarget, int iNumTurnsAway, int iPreferredDamageLevel, bool bNoRangedUnits, bool bNavalOnly, bool bMustMoveThrough, bool bIncludeBlockedUnits, bool bWillPillage, bool bTargetUndefended)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	bool bIsCityTarget = pTarget->getPlotCity() != NULL;
#if defined(MOD_AI_SMART_AIR_TACTICS)
	bool bAirUnitsAdded = false;
#endif

	// Loop through all units available to tactical AI this turn
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit)
		{
			if(!bNavalOnly || pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				// don't use non-combat units
				if(!pLoopUnit->IsCanAttack())
				{
					continue;
				}

				if(pLoopUnit->isOutOfAttacks())
				{
					continue;
				}

				if (!bIsCityTarget && pLoopUnit->IsCityAttackOnly())
				{
					continue;
				}

				if (bWillPillage && !pLoopUnit->canPillage(pTarget))
				{
					continue;
				}

#if defined(MOD_AI_SMART_MELEE_TACTICS)
				if (MOD_AI_SMART_MELEE_TACTICS && bNoRangedUnits && pLoopUnit->IsCanAttackRanged())
				{
					// To effectively skip all ranged units...
					continue;
				}
#endif

				int iAttackStrength = pLoopUnit->GetMaxAttackStrength(NULL, NULL, NULL);

				// Looking for damaged units?  If so, recalculate attack strength
				if (iPreferredDamageLevel > 0)
				{
					int iDamage = pLoopUnit->getDamage();
					if (iDamage > iPreferredDamageLevel)
					{
						iAttackStrength *= 3;
					}
				}

				int iTurnsCalculated = -1;	// If CanReachInXTurns does an actual pathfind, save the result so we don't just do the same one again.
				if (bTargetUndefended && CanReachInXTurns(pLoopUnit, pTarget, iNumTurnsAway, false /*bIgnoreUnits*/, &iTurnsCalculated))
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(iAttackStrength / 2);
					unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}

#if defined(MOD_AI_SMART_RANGED_UNITS)
				else if((MOD_AI_SMART_RANGED_UNITS || !bNoRangedUnits) && !bWillPillage && pLoopUnit->IsCanAttackRanged())
#else
				else if(!bNoRangedUnits && !bWillPillage && pLoopUnit->IsCanAttackRanged())
#endif
				{
					// Don't use air units for air strikes if at or below half health
					if (pLoopUnit->getDomainType() != DOMAIN_AIR || (pLoopUnit->getDamage() * 2) < GC.getMAX_HIT_POINTS())
					{
#if defined(MOD_AI_SMART_RANGED_UNITS)
						if (MOD_AI_SMART_RANGED_UNITS)
						{
							// Are we in range or could be in range with movement?
							int getDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());

							if ((getDistance < pLoopUnit-> GetRangeWithMovement()) && (getDistance > pLoopUnit-> GetRange()))
							{
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strMsg;
									strMsg.Format("Unit %s going to be considered, out of pure range (%d) but in move+range (%d)", pLoopUnit->getName().GetCString(), pLoopUnit->GetRange(), pLoopUnit->GetRangeWithMovement());
									LogTacticalMessage(strMsg);
								}
							}

							if(getDistance < pLoopUnit-> GetRangeWithMovement())
							{
								// Will we do any damage
								if(IsExpectedToDamageWithRangedAttack(pLoopUnit, pTarget))
								{
									CvTacticalUnit unit;
									unit.SetID(pLoopUnit->GetID());
									unit.SetAttackStrength(100 * pLoopUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true));
									unit.SetHealthPercent(100, 100);  // Don't take damage from bombarding, so show as fully healthy
									m_CurrentMoveUnits.push_back(unit);
									rtnValue = true;

#if defined(MOD_AI_SMART_AIR_TACTICS)
									if (pLoopUnit->getDomainType() == DOMAIN_AIR)
									{
										bAirUnitsAdded = true;
									}
#endif
								}
							}
						}
						else
						{
#endif
							// Are we in range?
							if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()) <= pLoopUnit->GetRange())
							{
								// Do we have LOS to the target?
								if(pLoopUnit->canEverRangeStrikeAt(pTarget->getX(), pTarget->getY()))
								{
									// Will we do any damage
									if(IsExpectedToDamageWithRangedAttack(pLoopUnit, pTarget))
									{
										CvTacticalUnit unit;
										unit.SetID(pLoopUnit->GetID());

										// Want ranged units to attack first, so inflate this
										unit.SetAttackStrength(100 * pLoopUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true));
										unit.SetHealthPercent(100, 100);  // Don't take damage from bombarding, so show as fully healthy
										m_CurrentMoveUnits.push_back(unit);
										rtnValue = true;
									}
								}
							}
#if defined(MOD_AI_SMART_RANGED_UNITS)
						}
#endif
					}
				}
				else
				{
					if (bMustMoveThrough && (iTurnsCalculated == 0 || (iTurnsCalculated == -1 && CanReachInXTurns(pLoopUnit, pTarget, 0, false /*bIgnoreUnits*/, &iTurnsCalculated))))
					{
						CvTacticalUnit unit;
						unit.SetID(pLoopUnit->GetID());
						unit.SetAttackStrength(iAttackStrength);
						unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
						m_CurrentMoveUnits.push_back(unit);
						rtnValue = true;
					}

					else if ( (iTurnsCalculated != -1 && iTurnsCalculated <= iNumTurnsAway) || (iTurnsCalculated == -1 && CanReachInXTurns(pLoopUnit, pTarget, iNumTurnsAway, false /*bIgnoreUnits*/)) )
					{
						CvTacticalUnit unit;
						unit.SetID(pLoopUnit->GetID());
						unit.SetAttackStrength(iAttackStrength);
						unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
						m_CurrentMoveUnits.push_back(unit);
						rtnValue = true;
					}

					// Units that can make it when others get out of the way are also potentially useful, but give them half priority so they bring up the rear
					else if (bIncludeBlockedUnits && CanReachInXTurns(pLoopUnit, pTarget, iNumTurnsAway, true /*bIgnoreUnits*/))
					{
						CvTacticalUnit unit;
						unit.SetID(pLoopUnit->GetID());
						unit.SetAttackStrength(iAttackStrength / 2);
						unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
						m_CurrentMoveUnits.push_back(unit);
						rtnValue = true;
					}
				}
			}
		}
	}

#if defined(MOD_AI_SMART_AIR_TACTICS)
	// As we have air units on the attack targets we should also check possible air sweeps
	if (MOD_AI_SMART_AIR_TACTICS && bAirUnitsAdded)
	{
		FindAirUnitsToAirSweep(pTarget);
	}
	else
	{
		m_CurrentAirUnits.clear();
	}
#endif

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Fills m_CurrentMoveCities with all cities within bombard range of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindCitiesWithinStrikingDistance(CvPlot* pTargetPlot)
{
	list<int>::iterator it;
	CvCity* pLoopCity;
	int iLoop;

	bool rtnValue = false;
	m_CurrentMoveCities.clear();

	// Loop through all of our cities
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()) && !IsCityInQueuedAttack(pLoopCity))
		{
			CvTacticalCity city;
			city.SetID(pLoopCity->GetID());
			m_CurrentMoveCities.push_back(city);
			rtnValue = true;
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveCities.begin(), m_CurrentMoveCities.end());

	return rtnValue;
}


/// Fills m_CurrentMoveUnits with all paratrooper units (available to jump) to the target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindParatroopersWithinStrikingDistance(CvPlot* pTarget)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->canParadropAt(pLoopUnit->plot(), pTarget->getX(), pTarget->getY()))
		{
			CvTacticalUnit unit;
			unit.SetID(pLoopUnit->GetID());
			unit.SetAttackStrength(pLoopUnit->GetMaxAttackStrength(NULL, NULL, NULL));
			unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
			m_CurrentMoveUnits.push_back(unit);
			rtnValue = true;
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}


/// Fills m_CurrentMoveUnits with all units within X turns of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestUnit(CvPlot* pTarget, int iNumTurnsAway, bool bMustHaveHalfHP, bool bMustBeRangedUnit, int iRangeRequired, bool bNeedsIgnoreLOS, bool bMustBeMeleeUnit, bool bIgnoreUnits, CvPlot* pRangedAttackTarget)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit)
		{
			bool bValidUnit = true;

			// don't use non-combat units (but consider embarked for now)
			if(pLoopUnit->getUnitInfo().GetCombat() == 0)
			{
				bValidUnit = false;
			}

			else if (bMustHaveHalfHP && (pLoopUnit->getDamage() * 2 > GC.getMAX_HIT_POINTS()))
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && ((pTarget->isWater() && pLoopUnit->getDomainType() == DOMAIN_LAND) ||
			                              (!pTarget->isWater() && !pTarget->isCity() && pLoopUnit->getDomainType() == DOMAIN_SEA)))
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && !pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && pLoopUnit->GetRange() < iRangeRequired)
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && !pLoopUnit->canEverRangeStrikeAt(pRangedAttackTarget->getX(), pRangedAttackTarget->getY()))
			{
				bValidUnit = false;
			}

			else if(bMustBeRangedUnit && pLoopUnit->isOutOfAttacks())
			{
				bValidUnit = false;
			}

			else if(pRangedAttackTarget != NULL && bMustBeRangedUnit && !IsExpectedToDamageWithRangedAttack(pLoopUnit, pRangedAttackTarget))
			{
				bValidUnit = false;
			}

			else if(bNeedsIgnoreLOS && !pLoopUnit->IsRangeAttackIgnoreLOS())
			{
				bValidUnit = false;
			}

			else if(bMustBeMeleeUnit && pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());
			if(iNumTurnsAway == 0 && iDistance > (m_iRecruitRange / 2) || iNumTurnsAway == 1 && iDistance > m_iRecruitRange)
			{
				bValidUnit = false;
			}

			if(bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false /*bReusePaths*/, bIgnoreUnits, (iNumTurnsAway==0));
				if(iTurns <= iNumTurnsAway)
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(1000-iTurns);
					unit.SetHealthPercent(10,10);
					unit.SetMovesToTarget(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()));
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}

	// Now sort them by turns to reach
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Fills m_CurrentMoveUnits with all units in operation that can get to target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestOperationUnit(CvPlot* pTarget, bool bSafeForRanged, bool bMustBeRangedUnit)
{
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to operation
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
		if(pLoopUnit)
		{
			bool bValidUnit = true;

			if (pLoopUnit->hasMoved())
			{
				bValidUnit = false;
			}

			else if(!bSafeForRanged && pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			else if (bMustBeRangedUnit && !pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			if(bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false /*bReusePaths*/, false /*bIgnoreUnits*/);

				if(iTurns != MAX_INT)
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(1000-iTurns);
					unit.SetHealthPercent(10,10);
					unit.SetMovesToTarget(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()));
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}

	// Now sort them by turns to reach
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Fills m_CurrentMoveUnits with all units in naval operation that can get to target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestNavalOperationUnit(CvPlot* pTarget, bool bEscortedUnits)
{
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to operation
	for(it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
		if(pLoopUnit)
		{
			bool bValidUnit = true;

			if(bEscortedUnits && (!pLoopUnit->IsGreatAdmiral() && pLoopUnit->getDomainType() != DOMAIN_LAND))
			{
				bValidUnit = false;
			}

			if(!bEscortedUnits && (pLoopUnit->getDomainType() != DOMAIN_SEA || pLoopUnit->IsGreatAdmiral()))
			{
				bValidUnit = false;
			}

			if(bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false /*bReusePaths*/, true /*bIgnoreUnits*/);

				if(iTurns != MAX_INT)
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(1000-iTurns);
					unit.SetHealthPercent(10,10);
					unit.SetMovesToTarget(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()));
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}

	// Now sort them by turns to reach
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

#if defined(MOD_AI_SMART_AIR_TACTICS)
// Fills m_CurrentAirUnits with all units able to sweep at target plot.
bool CvTacticalAI::FindAirUnitsToAirSweep(CvPlot* pTarget)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;
	bool rtnValue = false;
	m_CurrentAirUnits.clear();
	int interceptionsOnPlot = -1;

	// Loop through all units available to tactical AI this turn
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);

		if(pLoopUnit)
		{
			// Is an air unit.
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				// Is able to sweep at target
				if (pLoopUnit->canAirSweepAt(pTarget->getX(), pTarget->getY()))
				{
					// Still not checked max insterceptions
					if (interceptionsOnPlot == -1)
					{
						interceptionsOnPlot = m_pPlayer->GetMilitaryAI()->GetMaxPossibleInterceptions(pTarget);
					}

					// Either not more interceptions answered or not possible enemy interceptions, we end here.
					if (interceptionsOnPlot == 0)
					{
						break;
					}

					int iAttackStrength = pLoopUnit->GetMaxAttackStrength(NULL, NULL, NULL);
					// Mod to air sweep strength
					iAttackStrength *= (100 + pLoopUnit->GetAirSweepCombatModifier());
					iAttackStrength /= 100;
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(iAttackStrength);
					unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
					m_CurrentAirUnits.push_back(unit);
					interceptionsOnPlot--;
					rtnValue = true;
					// We also remove the unit from m_CurrentMoveUnits
					std::vector<CvTacticalUnit>::iterator itM;

					for(itM = m_CurrentMoveUnits.begin(); itM != m_CurrentMoveUnits.end(); itM++)
					{
						if ((*itM).GetID() == pLoopUnit->GetID())
						{
							m_CurrentMoveUnits.erase(itM);
							break;
						}
					}
				}
			}
		}
	}

	std::stable_sort(m_CurrentAirUnits.begin(), m_CurrentAirUnits.end());
	return rtnValue;
}

// helper function to iterate vector that is of CvPlot Type.
int CvTacticalAI::SamePlotFound(vector<CvPlot*> plotData, CvPlot* plotXy)
{
	int methodResult = 0;
	std::vector<CvPlot*>::iterator it;

	for (it = plotData.begin(); it != plotData.end(); it++)
	{
		if (((*it)->getX() == plotXy->getX()) && ((*it)->getY() == plotXy->getY()))
		{
			methodResult++;
		}
	}

	return methodResult;
}
#endif

/// Estimates the damage we can apply to a target
int CvTacticalAI::ComputeTotalExpectedDamage(CvTacticalTarget* pTarget, CvPlot* pTargetPlot)
{
	int rtnValue = 0;
	int iExpectedDamage;
	int iExpectedSelfDamage;
#if defined(MOD_AI_SMART_RANGED_UNITS)
	int farAwayUnitDivisor;
#endif

	// Loop through all units who can reach the target
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pAttacker = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
#if defined(MOD_AI_SMART_RANGED_UNITS)
		// If the unit is out of initial range, may not reach successfully to target (we still not know), so value is halved.
		farAwayUnitDivisor = MOD_AI_SMART_RANGED_UNITS ? ((plotDistance(pAttacker->getX(), pAttacker->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > pAttacker-> GetRange()) ? 2 : 1) : 1;
#endif

		// Is target a unit?
		switch(pTarget->GetTargetType())
		{
		case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
		{
			UnitHandle pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if(pDefender)
			{
				if(pAttacker->IsCanAttackRanged())
				{
#if defined(MOD_AI_SMART_RANGED_UNITS)
					iExpectedDamage = ((pAttacker->GetRangeCombatDamage(pDefender.pointer(), NULL, false)) / farAwayUnitDivisor);
#else
					iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender.pointer(), NULL, false);
#endif
					iExpectedSelfDamage = 0;
				}
				else
				{
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
					int iDefenderStrength = pDefender->GetMaxDefenseStrength(pTargetPlot, pAttacker.pointer());
					UnitHandle pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pDefender->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pAttacker.pointer(), NULL, false);
					}
					iExpectedDamage = pAttacker->getCombatDamage(iAttackerStrength, iDefenderStrength, pAttacker->getDamage() + iDefenderFireSupportCombatDamage, /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);
					iExpectedSelfDamage = pDefender->getCombatDamage(iDefenderStrength, iAttackerStrength, pDefender->getDamage(), /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);
				}
				m_CurrentMoveUnits[iI].SetExpectedTargetDamage(iExpectedDamage);
				m_CurrentMoveUnits[iI].SetExpectedSelfDamage(iExpectedSelfDamage);
				rtnValue += iExpectedDamage;
			}
		}
		break;

		case AI_TACTICAL_TARGET_CITY:
		{
			CvCity* pCity = pTargetPlot->getPlotCity();
			if(pCity != NULL)
			{
				if(pAttacker->IsCanAttackRanged() && pAttacker->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) > pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL))
				{
#if defined(MOD_AI_SMART_RANGED_UNITS)
					iExpectedDamage = ((pAttacker->GetRangeCombatDamage(NULL, pCity, false)) / farAwayUnitDivisor);
#else
					iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, false);
#endif
					iExpectedSelfDamage = 0;
				}
				else
				{
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
					int iDefenderStrength = pCity->getStrengthValue();
					CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pCity->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit != NULL)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pAttacker.pointer(), NULL, false);
					}
					iExpectedDamage = pAttacker->getCombatDamage(iAttackerStrength, iDefenderStrength, pAttacker->getDamage() + iDefenderFireSupportCombatDamage, /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ true);
					iExpectedSelfDamage = pAttacker->getCombatDamage(iDefenderStrength, iAttackerStrength, pCity->getDamage(), /*bIncludeRand*/ false, /*bAttackerIsCity*/ true, /*bDefenderIsCity*/ false);
				}
				m_CurrentMoveUnits[iI].SetExpectedTargetDamage(iExpectedDamage);
				m_CurrentMoveUnits[iI].SetExpectedSelfDamage(iExpectedSelfDamage);
				rtnValue += iExpectedDamage;
			}
		}
		break;
		}
	}

	return rtnValue;
}

/// Estimates the bombard damage we can apply to a target
int CvTacticalAI::ComputeTotalExpectedBombardDamage(UnitHandle pTarget)
{
	int rtnValue = 0;
	int iExpectedDamage;

	// Now loop through all the cities that can bombard it
	for(unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
	{
		CvCity* pAttackingCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		iExpectedDamage = pAttackingCity->rangeCombatDamage(pTarget.pointer(), NULL, false);
		m_CurrentMoveCities[iI].SetExpectedTargetDamage(iExpectedDamage);
		rtnValue += iExpectedDamage;
	}

	return rtnValue;
}

bool CvTacticalAI::IsExpectedToDamageWithRangedAttack(UnitHandle pAttacker, CvPlot* pTargetPlot)
{
	int iExpectedDamage = 0;

	if(pTargetPlot->isCity())
	{
		CvCity* pCity = pTargetPlot->getPlotCity();
		iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, /*bIncludeRand*/ false);
	}
	else
	{
		UnitHandle pDefender = pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID());
		if(pDefender)
		{
			iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender.pointer(), NULL, false);
		}
	}

	return iExpectedDamage > 0;
}


/// Move up to our target avoiding our own units if possible
bool CvTacticalAI::MoveToEmptySpaceNearTarget(UnitHandle pUnit, CvPlot* pTarget, bool bLand)
{
	CvPlot* pLoopPlot;

	// Look at spaces adjacent to target
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL && pLoopPlot->isWater() != bLand)
		{
			// Must be currently empty of friendly combat units
			if(!pLoopPlot->getBestDefender(m_pPlayer->GetID()))
			{
				// Enemies too
				if(!pLoopPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID()))
				{
					// And if it is a city, make sure we are friends with them, else we will automatically attack
					if(pLoopPlot->getPlotCity() == NULL || pLoopPlot->isFriendlyCity(*pUnit, false))
					{
						// Find a path to this space
						if(pUnit->GeneratePath(pLoopPlot))
						{
							// Go ahead with mission
							bool bMoveWasSafe;
							MoveToUsingSafeEmbark(pUnit, pLoopPlot, bMoveWasSafe);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/// Move up to our target (this time within 2 spaces) avoiding our own units if possible
bool CvTacticalAI::MoveToEmptySpaceTwoFromTarget(UnitHandle pUnit, CvPlot* pTarget, bool bLand)
{
	CvPlot* pLoopPlot;

	// Look at spaces adjacent to target
#if defined(MOD_GLOBAL_CITY_WORKING)
	for(int iI = 0; iI < AVG_CITY_PLOTS; iI++)
#else
	for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
#endif
	{
		pLoopPlot = plotCity(pTarget->getX(), pTarget->getY(), iI);
		if(pLoopPlot != NULL && pLoopPlot->isWater() != bLand && plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pTarget->getX(), pTarget->getY() == 2))
		{
			// Must be currently empty of friendly combat units
			if(!pLoopPlot->getBestDefender(m_pPlayer->GetID()))
			{
				// Enemies too
				if(!pLoopPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID()))
				{
					// And if it is a city, make sure we are friends with them, else we will automatically attack
					if(pLoopPlot->getPlotCity() == NULL || pLoopPlot->isFriendlyCity(*pUnit, false))
					{
						// Find a path to this space
						if(pUnit->GeneratePath(pLoopPlot))
						{
							// Go ahead with mission
							bool bMoveWasSafe;
							MoveToUsingSafeEmbark(pUnit, pLoopPlot, bMoveWasSafe);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/// Low-level wrapper on CvUnit::PushMission() for move to missions that avoids embarking if dangerous. Returns true if any move made
bool CvTacticalAI::MoveToUsingSafeEmbark(UnitHandle pUnit, CvPlot* pTargetPlot, bool &bMoveWasSafe)
{
	bMoveWasSafe = true;

	// Move right away if not a land unit
	if (pUnit->getDomainType() != DOMAIN_LAND)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		return true;
	}

	// If a land unit, get path to target
	if(!pUnit->GeneratePath(pTargetPlot))
	{
		// No path this may happen if a unit has moved up and blocked our path to our target plot
		// If calling routine is moving a bunch of units like this it should retry these units
		bMoveWasSafe = false;
		return false;
	}

	else
	{
#if defined(MOD_BALANCE_CORE_MILITARY)
#else		
		CvPlot *pMovePlot = pUnit->GetPathEndTurnPlot();
		// On land?  If so go ahead and move there
		if (!pMovePlot->isWater())
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
			return true;
		}
#endif
		// How dangerous is this plot?
#if defined(MOD_BALANCE_CORE_MILITARY)
#else
		else
		{
#endif
		
			bool bDangerous = false;

			int iPlotIndex = GC.getMap().plotNum(pTargetPlot->getX(), pTargetPlot->getY());
			CvTacticalAnalysisCell *pCell = m_pMap->GetCell(iPlotIndex);
			if (pCell->IsSubjectToAttack())
			{
				// Enemy naval unit can definitely attack this plot
				bDangerous = true;
			}

			else
			{
				if (GC.getGame().GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pTargetPlot))
				{
					// Also dangerous in an enemy dominated naval zone
					bDangerous = true;
				}
			}

			// Not dangerous, proceed
			if (!bDangerous)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
				return true;	
			}

			// Dangerous - try to move just on land
			else
			{
				if(!pUnit->GeneratePath(pTargetPlot, CvUnit::MOVEFLAG_STAY_ON_LAND))
				{
#if defined(MOD_BALANCE_CORE_MILITARY)
					if (MOD_BALANCE_CORE_MILITARY) 
					{
						// No land path so just stay put and fortify until life improves for you.
						if(pUnit->canFortify(pUnit->plot()) && !pUnit->isEmbarked())
						{
							pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
						}
					}
#else
					// No land path so just risk move to sea
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
#endif
					// Hopefully the calling routine might be able to pull escort over to stack with this unit
					bMoveWasSafe = false;
					return true;	
				}
				else
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY(), CvUnit::MOVEFLAG_STAY_ON_LAND);
					return true;	
				}
			}
#if defined(MOD_BALANCE_CORE_MILITARY)
#else
		}
#endif
	}
}

/// Find a multi-turn target for a land barbarian to wander towards
CvPlot* CvTacticalAI::FindBestBarbarianLandMove(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = FindNearbyTarget(pUnit, m_iLandBarbarianRange);
	
	// move toward trade routes
	if (pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianGankTradeRouteTarget(pUnit);
	}

	// explore wander
	if(pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	return pBestMovePlot;
}

/// Find a multi-turn target for a land barbarian to wander towards
CvPlot* CvTacticalAI::FindPassiveBarbarianLandMove(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;

	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Is this target a camp?
		if(m_AllTargets[iI].GetTargetType()==AI_TACTICAL_TARGET_BARBARIAN_CAMP)
		{
			iValue = plotDistance(pUnit->getX(), pUnit->getY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
			if(iValue < iBestValue)
			{
				iBestValue = iValue;
				pBestMovePlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
			}
		}
	}

	if(pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	return pBestMovePlot;
}

/// Find a multi-turn target for a sea barbarian to wander towards
CvPlot* CvTacticalAI::FindBestBarbarianSeaMove(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;
	CvPlot* pPlot;
	CvTacticalTarget* pTarget;
	int iMovementRate;

	iMovementRate = pUnit->baseMoves();
	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	// Loop through all unit targets to find the closest
	pTarget = GetFirstUnitTarget();
	while(pTarget != NULL)
	{
		// Is this unit nearby enough?
		if(plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY()) < m_iSeaBarbarianRange)
		{
			pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if(pUnit->getArea() == pPlot->getArea())
			{
				iValue = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/);
				if(iValue < iBestValue)
				{
					iBestValue = iValue;
					pBestMovePlot = pPlot;
				}
			}
		}
		pTarget = GetNextUnitTarget();
	}

	// move toward trade routes
	if (pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianGankTradeRouteTarget(pUnit);
	}

	// No units to pick on, so sail to a tile adjacent to the second closest barbarian camp
	if(pBestMovePlot == NULL)
	{
		CvPlot* pNearestCamp = NULL;
		int iBestCampDistance = MAX_INT;

		// Start by finding the very nearest camp
		pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		while(pTarget != NULL)
		{
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY());
			if(iDistance < iBestCampDistance)
			{
				pNearestCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
				iBestCampDistance = iDistance;
			}
			pTarget = GetNextZoneTarget();
		}

		// The obvious way to do this next part is to plot moves to each naval tile adjacent to each camp ...
		// starting with the first camp and then proceeding to the final one.  But our optimization (to drop out
		// targets that are further from the closest we've found so far) might in worst case not help at all if we
		// check the closest camp last.  So instead we'll loop by DIRECTIONS first which should mean we pick up some plot
		// from a close camp early (and the optimization will help)
		for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
		{
			pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
			while(pTarget != NULL)
			{
				CvPlot* pCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
				if(pCamp != pNearestCamp)
				{
					pPlot = plotDirection(pCamp->getX(), pCamp->getY(), ((DirectionTypes)jJ));
					if(pPlot && pPlot->isWater())
					{
						int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());

						// Optimization
						if(iDistance < iMovementRate * iBestValue && iDistance < (m_iSeaBarbarianRange * 3))
						{
							iValue = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/);
							if(iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestMovePlot = pPlot;
							}
						}
					}
				}
				pTarget = GetNextZoneTarget();
			}
		}
	}

	// No obvious target, let's scan nearby tiles for the best choice, borrowing some of the code from the explore AI
	if(pBestMovePlot == NULL)
	{
		// Now looking for BEST score
		iBestValue = 0;
		int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
		for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
		{
			for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
			{
				CvPlot* pConsiderPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
				if(!pConsiderPlot)
				{
					continue;
				}

				if(pUnit->atPlot(*pConsiderPlot))
				{
					continue;
				}

				if(!pConsiderPlot->isRevealed(pUnit->getTeam()))
				{
					continue;
				}

				if(pConsiderPlot->area() != pUnit->area())
				{
					continue;
				}

				if(!CanReachInXTurns(pUnit, pConsiderPlot, 1))
				{
					continue;
				}

				// Value them based on their explore value
				DomainTypes eDomain = pUnit->getDomainType();
				iValue = CvEconomicAI::ScoreExplorePlot(pConsiderPlot, pUnit->getTeam(), pUnit->getUnitInfo().GetBaseSightRange(), eDomain);

				// Add special value for being near enemy lands
				if(pConsiderPlot->isAdjacentOwned())
				{
					iValue += 100;
				}
				else if(pConsiderPlot->isOwned())
				{
					iValue += 200;
				}

				// If still have no value, score equal to distance from my current plot
				if(iValue == 0)
				{
					iValue = plotDistance(pUnit->getX(), pUnit->getY(), pConsiderPlot->getX(), pConsiderPlot->getY());
				}

				if(iValue > iBestValue)
				{
					pBestMovePlot = pConsiderPlot;
					iBestValue = iValue;
				}
			}
		}
	}

	return pBestMovePlot;
}

/// Scan nearby tiles for the best choice, borrowing code from the explore AI
CvPlot* CvTacticalAI::FindBarbarianExploreTarget(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;

	// Now looking for BEST score
	iBestValue = 0;
	int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
	for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
	{
		for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
			if(!pPlot)
			{
				continue;
			}

			if(pUnit->atPlot(*pPlot))
			{
				continue;
			}

			if(!pPlot->isRevealed(pUnit->getTeam()))
			{
				continue;
			}

			if(pPlot->area() != pUnit->area())
			{
				continue;
			}

			if(!CanReachInXTurns(pUnit, pPlot, 1))
			{
				continue;
			}

			// Value them based on their explore value
			DomainTypes eDomain = pUnit->getDomainType();
			iValue = CvEconomicAI::ScoreExplorePlot(pPlot, pUnit->getTeam(), pUnit->getUnitInfo().GetBaseSightRange(), eDomain);

			// Add special value for popping up on hills or near enemy lands
			if(pPlot->isAdjacentOwned())
			{
				iValue += 100;
			}
			else if(pPlot->isOwned())
			{
				iValue += 200;
			}

			// If still have no value, score equal to distance from my current plot
			if(iValue == 0)
			{
				iValue = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
			}

			if(iValue > iBestValue)
			{
				pBestMovePlot = pPlot;
				iBestValue = iValue;
			}
		}
	}
	return pBestMovePlot;
}

/// Scan nearby tiles for a trade route to sit and gank from
CvPlot* CvTacticalAI::FindBarbarianGankTradeRouteTarget(UnitHandle pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;

	// Now looking for BEST score
	iBestValue = 0;
	int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
	for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
	{
		for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
			if(!pPlot)
			{
				continue;
			}

			if(pUnit->atPlot(*pPlot))
			{
				continue;
			}

			if(!pPlot->isRevealed(pUnit->getTeam()))
			{
				continue;
			}

			if(pPlot->area() != pUnit->area())
			{
				continue;
			}

			if(!CanReachInXTurns(pUnit, pPlot, 1))
			{
				continue;
			}

			iValue = GC.getGame().GetGameTrade()->GetNumTradeRoutesInPlot(pPlot);

			if(iValue > iBestValue)
			{
				pBestMovePlot = pPlot;
				iBestValue = iValue;
			}
		}
	}
	return pBestMovePlot;
}


/// Find a multi-turn target for a land unit to wander towards
CvPlot* CvTacticalAI::FindNearbyTarget(UnitHandle pUnit, int iRange, AITacticalTargetType eType, CvUnit* pNoLikeUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;
	CvPlot* pPlot;

	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	// Loop through all appropriate targets to find the closest
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		bool bTypeMatch = false;
		if(eType == AI_TACTICAL_TARGET_NONE)
		{
			if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY ||
			        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_IMPROVEMENT)
			{
				bTypeMatch = true;
			}
		}
		else if(m_ZoneTargets[iI].GetTargetType() ==  eType)
		{
			bTypeMatch = true;
		}

		// Is this unit near enough?
		if(bTypeMatch)
		{
			pPlot = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			if(iDistance == 0)
			{
				return pPlot;
			}
			else if(iDistance < iRange)
			{
				if(pUnit->getArea() == pPlot->getArea())
				{
					if(!pNoLikeUnit || pPlot->getNumFriendlyUnitsOfType(pNoLikeUnit) == 0)
					{
						iValue = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/);
					
#if defined(MOD_BALANCE_CORE_MILITARY)
						//Let's try to stay on roads and out of water, ok?
						if(pUnit->getDomainType() == DOMAIN_LAND)
						{
							if(!pPlot->isRoute() || pPlot->isWater())
							{
								iValue *= 5;
							}
						}
#endif
						if(iValue < iBestValue)
						{
							pBestMovePlot = pPlot;
							iBestValue = iValue;
						}
					}
				}
			}
		}
	}

	return pBestMovePlot;
}


/// Am I within range of an enemy?
bool CvTacticalAI::NearVisibleEnemy(UnitHandle pUnit, int iRange)
{
	UnitHandle pLoopUnit;
	CvCity* pLoopCity = NULL;
	int iLoop;

	// Loop through enemies
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
		{
			// Loop through their units
			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				// Make sure this tile is visible to us
				if(pLoopUnit->plot()->isVisible(m_pPlayer->getTeam()))
				{
					// Check distance
					if(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
					{
						return true;
					}
				}
			}

			// Loop through their cities
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				// Make sure this tile is visible to us
				if(pLoopCity->plot()->isVisible(m_pPlayer->getTeam()))
				{
					// Check distance
					if(plotDistance(pLoopCity->getX(), pLoopCity->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

// BLOCKING POSITION FUNCTIONS

/// Do I have available friendly units that can stop this hex from being attacked?
bool CvTacticalAI::CanCoverFromEnemy(CvPlot* pPlot, int& iNumUnitsRequiredToCover, int iAttackingUnitID)
{
	CvPlot* pLoopPlot;
	int iPlotIndex;

	iNumUnitsRequiredToCover = 0;

	// Can't melee attack at sea so those hexes are always covered
	if(pPlot->isWater())
	{
		return true;
	}

	// Find all the hexes we need to cover
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		// Don't need to cover a water hex
		if(pLoopPlot != NULL && !pLoopPlot->isWater())
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			if(m_pMap->GetCell(iPlotIndex)->IsEnemyCanMovePast() && !m_pMap->GetCell(iPlotIndex)->IsFriendlyTurnEndTile())
			{
				iNumUnitsRequiredToCover++;
			}
		}
	}

	if(iNumUnitsRequiredToCover == 0)
	{
		m_ChosenBlocks.clear();
		return true;
	}
	else
	{
		if(iNumUnitsRequiredToCover > (int)m_CurrentTurnUnits.size())
		{
			return false;
		}

		// Have some unit that can cover each hex this turn?
		m_PotentialBlocks.clear();
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if(pLoopPlot != NULL && !pLoopPlot->isWater())
			{
				iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
				if(m_pMap->GetCell(iPlotIndex)->IsEnemyCanMovePast() && !m_pMap->GetCell(iPlotIndex)->IsFriendlyTurnEndTile())
				{
					if(!FindClosestUnit(pLoopPlot, 1, false/*bMustHaveHalfHP*/, false /*bMustBeRangedUnit*/, 2 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
					{
						return false;
					}
					else
					{
						// Save off the units that could get here
						for(unsigned int iJ = 0; iJ < m_CurrentMoveUnits.size(); iJ++)
						{
							if(m_CurrentMoveUnits[iJ].GetID() != iAttackingUnitID)
							{
								CvBlockingUnit block;
								block.SetUnitID(m_CurrentMoveUnits[iJ].GetID());
								block.SetPlot(pLoopPlot);
								block.SetNumChoices((int)m_CurrentMoveUnits.size());
								block.SetDistanceToTarget(m_CurrentMoveUnits[iJ].GetMovesToTarget());
								m_PotentialBlocks.push_back(block);
							}
						}
					}
				}
			}
		}
	}

	// Now select exact covering units, making sure we didn't over commit a unit to covering more than one hex
	return AssignCoveringUnits(iNumUnitsRequiredToCover);
}

/// Uses information from m_PotentialBlocks to make final assignments to block a hex (returns false if not possible)
bool CvTacticalAI::AssignCoveringUnits(int iNumUnitsRequiredToCover)
{
	bool bChoseOne = true;

	m_TemporaryBlocks.clear();
	m_ChosenBlocks.clear();

	while(bChoseOne)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

		// Loop through potential blocks looking for assignments we MUST make (only one possibility)
		for(unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
		{
			CvBlockingUnit temp = m_PotentialBlocks[iI];

			if(temp.GetNumChoices() == 1)
			{
				m_NewlyChosen.push_back(temp);
				bChoseOne = true;
			}
		}

		if(bChoseOne)
		{
			// Do we have the same unit in m_NewlyChosen twice?
			if(HaveDuplicateUnit())
			{
				return false;   // Not going to work
			}
			else
			{
				// Copy to final list
				for(unsigned int jJ = 0; jJ < m_NewlyChosen.size(); jJ++)
				{
					m_ChosenBlocks.push_back(m_NewlyChosen[jJ]);
				}

				RemoveChosenUnits();

				// Do we have enough units left to cover everything?
				if(NumUniqueUnitsLeft() < (iNumUnitsRequiredToCover - (int)m_ChosenBlocks.size()))
				{
					return false;
				}
			}
		}
	}

	return ChooseRemainingAssignments(iNumUnitsRequiredToCover, iNumUnitsRequiredToCover);
}

/// Uses information from m_PotentialBlocks to make final assignments to put flanking unit around a target
bool CvTacticalAI::AssignFlankingUnits(int iNumUnitsRequiredToFlank)
{
	m_TemporaryBlocks.clear();
	m_ChosenBlocks.clear();

	return ChooseRemainingAssignments(iNumUnitsRequiredToFlank, 2/*iNumUnitsAcceptable*/);
}

/// Uses information from m_PotentialBlocks to make final assignments to put deploying unit on target
bool CvTacticalAI::AssignDeployingUnits(int iNumUnitsRequiredToDeploy)
{
	bool bChoseOne = true;
	bool bRtnValue = true;

	m_TemporaryBlocks.clear();
	m_ChosenBlocks.clear();

	// Loop through potential blocks looking for assignments we MUST make (only one possibility)
	while(bChoseOne)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

		for(unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
		{
			if(m_PotentialBlocks[iI].GetNumChoices() == 1)
			{
				m_NewlyChosen.push_back(m_PotentialBlocks[iI]);
				bChoseOne = true;
			}
		}

		if(bChoseOne)
		{
			// Do we have the same unit in m_NewlyChosen twice?
			if(HaveDuplicateUnit())
			{
				return false;   // Not going to work
			}
			else
			{
				// Copy to final list
				for(unsigned int jJ = 0; jJ < m_NewlyChosen.size(); jJ++)
				{
					m_ChosenBlocks.push_back(m_NewlyChosen[jJ]);
				}

				RemoveChosenUnits();

				// Do we have enough units left to cover everything?
				if(NumUniqueUnitsLeft() < (iNumUnitsRequiredToDeploy - (int)m_ChosenBlocks.size()))
				{
					return false;
				}
			}
		}
	}

	// Pick closest unit for highest priority assignment until all processed
	while(m_PotentialBlocks.size() > 0)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

		m_NewlyChosen.push_back(m_PotentialBlocks[0]);
		m_ChosenBlocks.push_back(m_PotentialBlocks[0]);

		// Don't copy the other entries for this hex so pass in the number of choices here
		RemoveChosenUnits(m_PotentialBlocks[0].GetNumChoices());

		// Do we have enough units left to cover everything?
		if(NumUniqueUnitsLeft() < (iNumUnitsRequiredToDeploy - (int)m_ChosenBlocks.size()))
		{
			// Used to abort here, but better if we get the moves in we can
			bRtnValue = false;
		}
	}

	return bRtnValue;
}

/// Make and log selected movements
void CvTacticalAI::PerformChosenMoves(int iFallbackMoveRange)
{
	// Make moves up into hexes, starting with units already close to their final destination
	std::stable_sort(m_ChosenBlocks.begin(), m_ChosenBlocks.end(), TacticalAIHelpers::CvBlockingUnitDistanceSort);

	// First loop through is for units that have a unit moving into their hex.  They need to leave first!
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if(pUnit)
		{
			if(pUnit->plot() != m_ChosenBlocks[iI].GetPlot() && IsInChosenMoves(pUnit->plot()) && m_ChosenBlocks[iI].GetPlot()->getNumFriendlyUnitsOfType(pUnit) == 0)
			{
				bool bMoveWasSafe;
				MoveToUsingSafeEmbark(pUnit, m_ChosenBlocks[iI].GetPlot(), bMoveWasSafe);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Deploying %s (to get out of way), To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
					              pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
					              pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
					LogTacticalMessage(strMsg);
				}

				// Use number of choices field to indicate already moved
				m_ChosenBlocks[iI].SetNumChoices(-1);
			}
		}
	}

	// Second loop is for units moving into their chosen spot normally
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if(pUnit)
		{
			if(pUnit->plot() == m_ChosenBlocks[iI].GetPlot())
			{
				m_ChosenBlocks[iI].SetNumChoices(-1);
			}
			else
			{
				// Someone we didn't move above?
				if(m_ChosenBlocks[iI].GetNumChoices() != -1)
				{
					CvPlot* pPlotBeforeMove = pUnit->plot();
					bool bMoveWasSafe;
					MoveToUsingSafeEmbark(pUnit, m_ChosenBlocks[iI].GetPlot(), bMoveWasSafe);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying %s, To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
						              pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
						              pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
						LogTacticalMessage(strMsg);
					}

					// Use number of choices field to indicate already moved
					if(pPlotBeforeMove != pUnit->plot())
					{
						m_ChosenBlocks[iI].SetNumChoices(-1);
					}
				}
			}
		}
	}

	// Third loop is for units we still haven't been able to move (other units must be blocking their target for this turn)
	if (iFallbackMoveRange > 0)
	{
		for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
			if(pUnit)
			{
				// Someone we didn't move above?
				if(m_ChosenBlocks[iI].GetNumChoices() != -1)
				{
					CvPlot* pPlotBeforeMove = pUnit->plot();
					if(MoveToEmptySpaceNearTarget(UnitHandle(pUnit), m_ChosenBlocks[iI].GetPlot(), (pUnit->getDomainType()==DOMAIN_LAND)))
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying %s to space near target, Target X: %d, Target Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
								pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
								pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
							LogTacticalMessage(strMsg);
						}

						if(pPlotBeforeMove != pUnit->plot())
						{
							m_ChosenBlocks[iI].SetNumChoices(-1);
						}
					}
				}
			}
		}
	}

	// Fourth loop let's unit end within 2 of target
	if (iFallbackMoveRange > 1)
	{
		for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
			if(pUnit)
			{
				// Someone we didn't move above?
				if(m_ChosenBlocks[iI].GetNumChoices() != -1)
				{
					if (MoveToEmptySpaceTwoFromTarget(UnitHandle(pUnit), m_ChosenBlocks[iI].GetPlot(), (pUnit->getDomainType()==DOMAIN_LAND)))
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying %s to space within 2 of target, Target X: %d, Target Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
								pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
								pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
							LogTacticalMessage(strMsg);
						}
					}
				}
			}
		}
	}

	// Finish moves for all units
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if(!pUnit->isDelayedDeath())
		{
			if (pUnit->getMoves() > 0)
			{
				if (pUnit->canPillage(pUnit->plot()) && pUnit->getDamage() > 0)
				{
					pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Already in position, will pillage with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
						LogTacticalMessage(strMsg);
					}

				}
				else if(pUnit->canFortify(pUnit->plot()))
				{
					pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Already in position, will fortify with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
						LogTacticalMessage(strMsg);
					}
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Already in position, no move for %s, X: %d, Y: %d", pUnit->getName().GetCString(),
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
						LogTacticalMessage(strMsg);
					}
				}
				pUnit->finishMoves();
			}
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Move a great general with an operation
void CvTacticalAI::MoveGreatGeneral(CvArmyAI* pArmyAI)
{
	UnitHandle pGeneral;
	int iRange;

	for (unsigned int iI = 0; iI < m_GeneralsToMove.size(); iI++)
	{
		CvPlot* pBestPlot = NULL;
		int iBestScore = -1;
		pGeneral = m_pPlayer->getUnit(m_GeneralsToMove[iI].GetUnitID());

		if(pGeneral)
		{
			iRange = (pGeneral->maxMoves() * 3) / GC.getMOVE_DENOMINATOR();  // Enough to make a decent road move
			for(int iX = -iRange; iX <= iRange; iX++)
			{
				for(int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot* pEvalPlot = NULL;
					pEvalPlot = plotXYWithRangeCheck(pGeneral->getX(), pGeneral->getY(), iX, iY, iRange);
					if(!pEvalPlot)
					{
						continue;
					}

					if(CanReachInXTurns(pGeneral, pEvalPlot, 1))
					{
						int iScore = ScoreGreatGeneralPlot(pGeneral, pEvalPlot, pArmyAI);

						if(iScore > iBestScore && iScore > 0)
						{
							iBestScore = iScore;
							pBestPlot = pEvalPlot;
						}
					}

				}
			}

			if(pBestPlot != NULL)
			{
				ExecuteMoveToPlot(pGeneral, pBestPlot);

				if(GC.getLogging() && GC.getAILogging())
				{
					int iDistToOperationCenter = -1;

					if(pArmyAI)
					{
						CvPlot* pCOM = pArmyAI->GetCenterOfMass(NO_DOMAIN);
						if(pCOM)
						{
							iDistToOperationCenter = plotDistance(pBestPlot->getX(), pBestPlot->getY(), pCOM->getX(), pCOM->getY());
						}
					}

					CvString strMsg;
					strMsg.Format("Deploying %s, To X: %d, To Y: %d, At X: %d, At Y: %d, Plot Score: %d, Dist from COM: %d",
					              pGeneral->getName().GetCString(), pBestPlot->getX(), pBestPlot->getY(),
					              pGeneral->getX(), pGeneral->getY(), iBestScore, iDistToOperationCenter);
					LogTacticalMessage(strMsg);
				}
			}
		}
	}

	return;
}

// Is one of the chosen moves to this plot?
bool CvTacticalAI::IsInChosenMoves(CvPlot* pPlot)
{
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		if(m_ChosenBlocks[iI].GetPlot() == pPlot)
		{
			return true;
		}
	}

	return false;
}

/// Were we forced to select the same unit to block twice?
bool CvTacticalAI::HaveDuplicateUnit()
{
	for(unsigned int iI = 0; iI < m_NewlyChosen.size()- 1; iI++)
	{
		for(unsigned int jJ = iI + 1; jJ < m_NewlyChosen.size(); jJ++)
		{
			if(m_NewlyChosen[iI].GetUnitID() == m_NewlyChosen[jJ].GetUnitID())
			{
				return true;
			}
		}
	}
	return false;
}

/// Pull the units we just assigned out of the list of potential assignments
void CvTacticalAI::RemoveChosenUnits(int iStartIndex)
{
	m_TemporaryBlocks.clear();
	m_TemporaryBlocks = m_PotentialBlocks;
	m_PotentialBlocks.clear();

	for(unsigned int iI = iStartIndex; iI < m_TemporaryBlocks.size(); iI++)
	{
		bool bCopyIt = true;

		CvBlockingUnit block = m_TemporaryBlocks[iI];

		// Loop through chosen array looking for occurrences of this unit
		for(unsigned int jJ = 0; jJ < m_NewlyChosen.size() && bCopyIt; jJ++)
		{
			if(block.GetUnitID() == m_NewlyChosen[jJ].GetUnitID())
			{
				bCopyIt = false;
			}
		}

		if(bCopyIt)
		{
			m_PotentialBlocks.push_back(block);
		}
	}

	// Rebuild number of choices
	for(unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
	{
		int iNumFound = 0;
		CvPlot* pPlot = m_PotentialBlocks[iI].GetPlot();

		for(unsigned int jJ = 0; jJ < m_PotentialBlocks.size(); jJ++)
		{
			if(pPlot == m_PotentialBlocks[jJ].GetPlot())
			{
				iNumFound++;
			}
		}
		m_PotentialBlocks[iI].SetNumChoices(iNumFound);
	}
}

/// How many units are left unassigned for a blocking position?
int CvTacticalAI::NumUniqueUnitsLeft()
{
	int iRtnValue = 1;
	int iCurrentID;

	if(m_PotentialBlocks.size() < 2)
	{
		return m_PotentialBlocks.size();
	}

	// Copy data over and sort it so in unit ID order
	m_TemporaryBlocks.clear();
	m_TemporaryBlocks = m_PotentialBlocks;
	std::stable_sort(m_TemporaryBlocks.begin(), m_TemporaryBlocks.end());

	iCurrentID = m_TemporaryBlocks[0].GetUnitID();
	for(unsigned int iI = 1; iI < m_TemporaryBlocks.size(); iI++)
	{
		if(m_TemporaryBlocks[iI].GetUnitID() != iCurrentID)
		{
			iRtnValue++;
			iCurrentID = m_TemporaryBlocks[iI].GetUnitID();
		}
	}

	return iRtnValue;
}

/// No clear cut blocking assignments left, have to make search possibilities and score most preferred
bool CvTacticalAI::ChooseRemainingAssignments(int iNumUnitsDesired, int iNumUnitsAcceptable)
{
	int iBestScore = 0;
	int iScore;
	int iCurrent[NUM_DIRECTION_TYPES];
	int iFirst[NUM_DIRECTION_TYPES];
	int iLast[NUM_DIRECTION_TYPES];

	int iBlocksToCreate = iNumUnitsDesired - (int)m_ChosenBlocks.size();

	if(iBlocksToCreate == 0)
	{
		return true;
	}

	FAssertMsg(iBlocksToCreate <= NUM_DIRECTION_TYPES, "More than NUM_DIRECTION_TYPES hexes to block. Will cause array overflows and performance issues!");

	if(iBlocksToCreate > 0 && iBlocksToCreate < NUM_DIRECTION_TYPES)
	{
		m_NewlyChosen.clear();

		// Set up indexes pointing to the possible choices
		int iLevel = 0;
		int iCurIndex = 0;
		while(iLevel < iBlocksToCreate)
		{
			iFirst[iLevel] = iCurIndex;
			int iNumChoices = m_PotentialBlocks[iCurIndex].GetNumChoices();

			FAssertMsg(iNumChoices > 0, "Invalid number of tactical AI move choices. Will cause array overflows and performance issues!");
			FAssertMsg(iNumChoices + iCurIndex <= (int)m_PotentialBlocks.size(), "Invalid number of tactical AI move choices. Will cause array overflows and performance issues!");

			iLast[iLevel] = iCurIndex + iNumChoices - 1;
			iCurIndex = iLast[iLevel] + 1;
			iLevel++;
		}

		for(int iI = 0; iI < iBlocksToCreate; iI++)
		{
			iCurrent[iI] = iFirst[iI];
		}

		// Loop through each possibility
		bool bDone = false;
		while(!bDone)
		{
			m_TemporaryBlocks.clear();

			// Create this choice
			for(int iI = 0; iI < iBlocksToCreate; iI++)
			{
				if(iI >= (int)m_PotentialBlocks.size())
				{
					FAssertMsg(false, "Invalid fast vector index - show Ed");
				}
				m_TemporaryBlocks.push_back(m_PotentialBlocks[iCurrent[iI]]);
			}

			iScore = ScoreAssignments(iNumUnitsDesired != iNumUnitsAcceptable /*bCanLeaveOpenings*/);

			// If best so far, save it off
			if(iScore > iBestScore)
			{
				m_NewlyChosen.clear();
				for(unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
				{
					if(m_TemporaryBlocks[iI].GetDistanceToTarget() != MAX_INT)
					{
						m_NewlyChosen.push_back(m_TemporaryBlocks[iI]);
					}
				}
				iBestScore = iScore;
			}

			// Increment proper index
			bool bIncrementDone = false;
			for(int iLevelIndex = iBlocksToCreate - 1; !bIncrementDone && iLevelIndex >= 0 && iLevelIndex < NUM_DIRECTION_TYPES; iLevelIndex--)
			{
				// See if at end of line for this index
				if(iCurrent[iLevelIndex] + 1 > iLast[iLevelIndex])
				{
					// Reset to first one and keep iterating
					iCurrent[iLevelIndex] = iFirst[iLevelIndex];
				}

				else
				{
					iCurrent[iLevelIndex]++;
					bIncrementDone = true;
				}
			}

			if(!bIncrementDone)
			{
				bDone = true;
			}
		}

		// Copy final choices into output
		for(unsigned int iI = 0; iI < m_NewlyChosen.size(); iI++)
		{
			m_ChosenBlocks.push_back(m_NewlyChosen[iI]);
		}

		return (int)m_ChosenBlocks.size() >= iNumUnitsAcceptable;
	}

	return false;
}

/// Score for this set of chosen blocks in m_TemporaryBlocks (-1 if illegal)
int CvTacticalAI::ScoreAssignments(bool bCanLeaveOpenings)
{
	int iScore = 0;

	// Any assignment appear twice?
	for(unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
	{
		for(unsigned int jJ = iI + 1; jJ < m_TemporaryBlocks.size(); jJ++)
		{
			if(m_TemporaryBlocks[iI].GetUnitID() == m_TemporaryBlocks[jJ].GetUnitID())
			{
				if(!bCanLeaveOpenings)
				{
					return -1;
				}
				else
				{
					// "Clear" the move with greater distance by setting it to MAX_INT distance
					if(m_TemporaryBlocks[iI].GetDistanceToTarget() < m_TemporaryBlocks[jJ].GetDistanceToTarget())
					{
						m_TemporaryBlocks[jJ].SetDistanceToTarget(MAX_INT);
					}
					else
					{
						m_TemporaryBlocks[iI].SetDistanceToTarget(MAX_INT);
					}
				}
			}
		}
	}

	// Legal, so let's score it
	for(unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
	{
		if(m_TemporaryBlocks[iI].GetDistanceToTarget() != MAX_INT)
		{
			iScore += (10000 - (m_TemporaryBlocks[iI].GetDistanceToTarget() * 1000));
			iScore += m_pPlayer->getUnit(m_TemporaryBlocks[iI].GetUnitID())->GetPower();
		}
	}

	return iScore;
}

/// Pick best hexes for closing in on an enemy city. Returns number of ranged unit plots found
int CvTacticalAI::ScoreCloseOnPlots(CvPlot* pTarget, bool bLandOnly)
{
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	bool bChoiceBombardSpot;
	bool bSafeFromAttack;
	CvTacticalTarget target;
	int iRtnValue = 0;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

#if defined(MOD_GLOBAL_CITY_WORKING)
	for(int jJ = 0; jJ < AVG_CITY_PLOTS; jJ++)
#else
	for(int jJ = 0; jJ < NUM_CITY_PLOTS; jJ++)
#endif
	{
		pPlot = plotCity(pTarget->getX(), pTarget->getY(), jJ);

		if(pPlot != NULL)
		{
			bChoiceBombardSpot = false;
			bSafeFromAttack = true;

			int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);

			if((bLandOnly && pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/)) || (!bLandOnly && pCell->CanUseForOperationGathering()))
			{
				bool bCloseEnough = false;
				for(unsigned int iI = 0; iI < m_OperationUnits.size() && !bCloseEnough; iI++)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(m_OperationUnits[iI].GetUnitID());
					if(pUnit)
					{
						if(plotDistance(pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY()) <= m_iDeployRadius)
						{
							bCloseEnough = true;
						}
					}
				}

				if(bCloseEnough)
				{
					iScore = 600 - (iPlotDistance * 100);

					// Top priority is hexes to bombard from (within range but not adjacent)
					if(pCell->IsWithinRangeOfTarget() && iPlotDistance > 1)
					{
						bChoiceBombardSpot = true;
						iRtnValue++;
					}
#if defined(MOD_BALANCE_CORE_MILITARY)
					else if(MOD_BALANCE_CORE_MILITARY){
						//The AI spends too much time positioning- better to attack than to shuffle to death.
						if(pCell->IsWithinRangeOfTarget() && iPlotDistance <= 1)
						{
							iScore += pCell->GetDefenseModifier();
						}
						//If we can bombard from friendly territory, this is good.
						else if(pCell->IsFriendlyTerritory() && pCell->IsWithinRangeOfTarget())
						{
							bChoiceBombardSpot = true;
							iRtnValue++;
							iScore += (pCell->GetDefenseModifier() * 2);
						}
					}
#endif
					if(pCell->IsSubjectToAttack())
					{
#if defined(MOD_BALANCE_CORE_MILITARY)
						iScore -= (30 - pCell->GetDefenseModifier());
#else
						iScore -= 30;
#endif
						bSafeFromAttack = false;
					}

					if(pCell->IsEnemyCanMovePast())
					{
#if defined(MOD_BALANCE_CORE_MILITARY)
						iScore -= (30 - pCell->GetDefenseModifier());
#else
						iScore -= 30;
#endif
					}
					if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
					{
						iScore += 100;
					}
					else
					{
						iScore += pCell->GetDefenseModifier();
					}

					pCell->SetSafeForDeployment(bChoiceBombardSpot || bSafeFromAttack);
					pCell->SetDeploymentScore(iScore);

					// Save this in our list of potential targets
					target.SetTargetX(pPlot->getX());
					target.SetTargetY(pPlot->getY());
					target.SetAuxIntData(iScore);

					// A bit of a hack -- use high priority targets to indicate good plots for ranged units
					if(bChoiceBombardSpot)
					{
						target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
					else
					{
						target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
					}

					m_TempTargets.push_back(target);
				}
			}
		}
	}
	return iRtnValue;
}

/// Pick best hexes for defending a friendly city. Returns false if insufficient free plots.
void CvTacticalAI::ScoreHedgehogPlots(CvPlot* pTarget)
{
	int iScore;
	CvPlot* pPlot;
	CvTacticalAnalysisCell* pCell;
	bool bChoiceBombardSpot;
	bool bSafeFromAttack;
	CvTacticalTarget target;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

#if defined(MOD_GLOBAL_CITY_WORKING)
	for(int jJ = 0; jJ < AVG_CITY_PLOTS; jJ++)
#else
	for(int jJ = 0; jJ < NUM_CITY_PLOTS; jJ++)
#endif
	{
		pPlot = plotCity(pTarget->getX(), pTarget->getY(), jJ);

		if(pPlot != NULL)
		{
			bChoiceBombardSpot = false;
			bSafeFromAttack = true;

			int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);

			if(pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/))
			{
				iScore = 600 - (iPlotDistance * 150);

				if(pCell->IsSubjectToAttack())
				{
					iScore += 100;
					bSafeFromAttack = false;
				}
				if(pCell->IsEnemyCanMovePast())
				{
					iScore += 50;
				}
				if(pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
				{
#if defined(MOD_BALANCE_CORE_MILITARY)
					//Let's prioritize filling up cities, okay?
					iScore += 1000;
					bChoiceBombardSpot = true;
#else
					iScore += 100;
#endif
				}
				else
				{
					iScore += pCell->GetDefenseModifier() * 4;
				}

#if defined(MOD_BALANCE_CORE_MILITARY)
				pCell->SetSafeForDeployment(bChoiceBombardSpot || bSafeFromAttack);
#else
				pCell->SetSafeForDeployment(bSafeFromAttack);
#endif
				pCell->SetDeploymentScore(iScore);

				// Save this in our list of potential targets
				target.SetTargetX(pPlot->getX());
				target.SetTargetY(pPlot->getY());
				target.SetAuxIntData(iScore);

				// A bit of a hack -- use high priority targets to indicate good plots for ranged units
				if(bSafeFromAttack)
				{
					target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
				}
				else
				{
					target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
				}

				m_TempTargets.push_back(target);
			}
		}
	}
}

/// Support function to pick best hex for a great general to move to
int CvTacticalAI::ScoreGreatGeneralPlot(UnitHandle pGeneral, CvPlot* pTarget, CvArmyAI* pArmyAI)
{
	// Returned value
	int iScore = 0;

	UnitHandle pBestDefender;
	PlayerTypes ePlayer = m_pPlayer->GetID();

	// Variables going into score
	bool bFriendlyCity = false;
	int iNearbyQueuedAttacks = 0;
	int iDangerValue = 0;
	int iDangerDivisor = 1;
	int iDistToOperationCenter = MAX_INT;
	int iFriendlyUnitBasePower = 0;
	int iFriendlyUnitFinalPower = 0;
	int iFriendlyCityStrength = 0;

	// GATHER DATA

	// Non-friendly city here?
	if(pTarget->isCity() && pTarget->getOwner() != ePlayer)
	{
		return 0;
	}

	// Another player's unit here?
	CvUnit* pUnit = pTarget->getUnitByIndex(0);
	if(pUnit && pUnit->getOwner() != ePlayer)
	{
		return 0;
	}

	// Another civilian of mine that has finished its moves?
	else if (pUnit && pUnit->GetID() != pGeneral->GetID() && !pUnit->IsCanAttack() && pUnit->hasMoved())
	{
		return 0;
	}

	// Danger value
	iDangerValue = m_pPlayer->GetPlotDanger(*pTarget);
	pBestDefender = pTarget->getBestDefender(m_pPlayer->GetID());

	// Friendly city here?
	if(pTarget->isCity())
	{
		bFriendlyCity = true;
		CvCity* pCity = pTarget->getPlotCity();

		iFriendlyCityStrength = pCity->getStrengthValue() * 100;
		iFriendlyCityStrength *= (pCity->GetMaxHitPoints() - pCity->getDamage());
		iFriendlyCityStrength /= pCity->GetMaxHitPoints();

		if(iDangerValue > (iFriendlyCityStrength * 2))
		{
			iDangerDivisor = 5;
		}
		else if(iDangerValue > iFriendlyCityStrength)
		{
			iDangerDivisor = 3;
		}
		else if(iDangerValue > (iFriendlyCityStrength / 2))
		{
			iDangerDivisor = 2;
		}
	}

	// Friendly unit here?
	else if(pBestDefender && pBestDefender->GetID() != pGeneral->GetID())
	{
		iFriendlyUnitBasePower = pBestDefender->GetBaseCombatStrengthConsideringDamage() * 250;

		if(iDangerValue > (iFriendlyUnitBasePower * 2))
		{
			iDangerDivisor = 5;
		}
		else if(iDangerValue > iFriendlyUnitBasePower)
		{
			iDangerDivisor = 3;
		}
		else if(iDangerValue > (iFriendlyUnitBasePower / 2))
		{
			iDangerDivisor = 2;
		}

		iFriendlyUnitFinalPower = iFriendlyUnitBasePower;

		// Deemphasize if hasn't finished moves yet
		if(IsInQueuedAttack(pBestDefender.pointer()) && !pBestDefender->isRanged())
		{
			// Melee unit: might take losses attacking, so reduce the desirability
			iFriendlyUnitFinalPower /= 3;
		}

		else if(pBestDefender->getMoves() > 0)
		{
			// Could move anywhere, not a good place to go
			iFriendlyUnitFinalPower /= 25;
		}

		// Emphasize member of this operation
		if(pArmyAI && (pBestDefender->getArmyID() != -1) &&(pBestDefender->getArmyID() == pArmyAI->GetID()))
		{
			iFriendlyUnitFinalPower *= 5;
		}
	}

	// No friendly city or unit
	else
	{	
#if defined(MOD_BALANCE_CORE_MILITARY)
		//GGs should never go alone if possible.
		iDangerDivisor = 10000;
#else
		iDangerDivisor = 1000;
#endif
	}

	// Distance to center of army (if still under operational AI)
	if(pArmyAI)
	{
		CvPlot* pCOM = pArmyAI->GetCenterOfMass(NO_DOMAIN);
		if(pCOM)
		{
			iDistToOperationCenter = plotDistance(pTarget->getX(), pTarget->getY(), pCOM->getX(), pCOM->getY());
		}
	}

	// Near an attack we already have planned?
	iNearbyQueuedAttacks = NearXQueuedAttacks(pTarget, 2);

	// COMPUTE SCORE
	//  Entering a city
	if(bFriendlyCity)
	{
		iScore = iFriendlyCityStrength;

		// If part of an army moving to target, don't hide in a city
		if(pArmyAI)
		{
			iScore = 1000;
		}
	}
	// Stacking with a unit
	else if(iFriendlyUnitFinalPower > 0)
	{
		iScore = iFriendlyUnitFinalPower;
	}
	// Moving to an empty tile
	else
	{
		iScore = 10;
	}

	if(iNearbyQueuedAttacks > 0)
	{
		iScore *= (5 * iNearbyQueuedAttacks);
	}

	if(iDistToOperationCenter < 20 && (iFriendlyUnitFinalPower > 0 || bFriendlyCity || iDangerValue == 0))
	{
		// Anywhere near center is really good
		if(iDistToOperationCenter <= 3)
		{
			iScore += 5000000;
		}

		// Sliding scale further off
		else
		{
			iScore += (250000 * (20 - iDistToOperationCenter));
		}
	}

	if(iDangerValue > 0)
	{
		iScore /= iDangerDivisor;
	}

	return iScore;
}

/// Remove a unit that we've allocated from list of units to move this turn
void CvTacticalAI::UnitProcessed(int iID, bool bMarkTacticalMap)
{
	UnitHandle pUnit;
	int iPlotIndex;
	CvTacticalAnalysisCell* pCell;

	m_CurrentTurnUnits.remove(iID);
	pUnit = m_pPlayer->getUnit(iID);

	CvAssert(pUnit);
	pUnit->SetTurnProcessed(true);

	if(bMarkTacticalMap)
	{
		CvTacticalAnalysisMap* pMap = GC.getGame().GetTacticalAnalysisMap();
		if(pMap->IsBuilt())
		{
			iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
			pCell = pMap->GetCell(iPlotIndex);
			pCell->SetFriendlyTurnEndTile(true);
		}
	}
}

/// Do we want to process moves for this dominance zone?
bool CvTacticalAI::UseThisDominanceZone(CvTacticalDominanceZone* pZone)
{
	bool bIsOurCapital = false;
	bool bTempZone = false;
	int iCityID = -1;
	if(pZone->GetClosestCity() != NULL)
	{
		iCityID = pZone->GetClosestCity()->GetID();
		bIsOurCapital = (pZone->GetClosestCity() == m_pPlayer->getCapitalCity());
		bTempZone = IsTemporaryZoneCity(pZone->GetClosestCity());
	}
	bool bWeHaveUnitsNearEnemy = pZone->GetFriendlyUnitCount() > 0 && pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY;
	bool bTheyHaveUnitsNearUs = pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_NO_UNITS_VISIBLE && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY;
	bool bBothHaveUnits = pZone->GetFriendlyUnitCount() > 0 && pZone->GetEnemyUnitCount() > 0;

	return (bTempZone || bIsOurCapital || bWeHaveUnitsNearEnemy || bTheyHaveUnitsNearUs || bBothHaveUnits);
}


/// Is this civilian target of the highest priority?
bool CvTacticalAI::IsVeryHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = (CvUnit*)pTarget->GetAuxData();
	if(pUnit)
	{
		if(pUnit->AI_getUnitAIType() == UNITAI_GENERAL || pUnit->AI_getUnitAIType() == UNITAI_ADMIRAL)
		{
			bRtnValue = true;
		}
	}
	return bRtnValue;
}

/// Is this civilian target of high priority?
bool CvTacticalAI::IsHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = (CvUnit*)pTarget->GetAuxData();
	if(pUnit)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
		int iEstimatedEndTurn = GC.getGame().getEstimateEndTurn();
		if(pkUnitInfo)
		{
			// Priorities defined in XML
			if(pkUnitInfo->GetCivilianAttackPriority() == CIVILIAN_ATTACK_PRIORITY_HIGH)
			{
				bRtnValue = true;
			}
			else if(pkUnitInfo->GetCivilianAttackPriority() == CIVILIAN_ATTACK_PRIORITY_HIGH_EARLY_GAME_ONLY)
			{
				if(GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3))
				{
					bRtnValue =  true;
				}
			}
			if(!bRtnValue && pUnit->AI_getUnitAIType() == UNITAI_SETTLE)
			{
				//Settlers
				if(GET_PLAYER(pUnit->getOwner()).getNumCities() < 5)  //small player?
				{
					bRtnValue = true;
				}
				else if(GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3))  //early game?
				{
					bRtnValue =  true;
				}
				else if(GET_PLAYER(pUnit->getOwner()).IsCramped())  //player really needs to expand?
				{
					bRtnValue = true;
				}
			}
			if(!bRtnValue && m_pPlayer->isBarbarian())
			{
				bRtnValue = true; //always high priority for barbs
			}
#if defined(MOD_BALANCE_CORE_MILITARY)
			if (MOD_BALANCE_CORE_MILITARY) 
			{
				//Let's not be too aggressive around our own cities.
				if(pUnit->plot()->GetAdjacentCity() != NULL)
				{
					if(pUnit->plot()->GetAdjacentCity()->getOwner() == m_pPlayer->GetID())
					{
						bRtnValue = false;
					}
				}
				//Is he trying to lure us to come out of a garrison?
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot *pLoopPlot = plotDirection(pUnit->plot()->getX(), pUnit->plot()->getY(), ((DirectionTypes)iI));
					if (pLoopPlot != NULL)
					{
						if((pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false) > 15) && pLoopPlot->getNumUnits() > 0)
						{
							if(pLoopPlot->getUnitByIndex(0)->getOwner() == m_pPlayer->GetID())
							{
								bRtnValue = false;
							}
						}
					}
				}
			}
#endif
		}
	}
	return bRtnValue;
}

/// Is this civilian target of medium priority?
bool CvTacticalAI::IsMediumPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = (CvUnit*)pTarget->GetAuxData();
	if(pUnit)
	{
		int iEstimatedEndTurn = GC.getGame().getEstimateEndTurn();
		if(pUnit->isEmbarked() && !pUnit->IsCombatUnit())  //embarked civilians
		{
			bRtnValue = true;
		}
		else if(pUnit->AI_getUnitAIType() == UNITAI_SETTLE)
		{
			if(GC.getGame().getGameTurn() >= (iEstimatedEndTurn / 3))
			{
				bRtnValue =  true;
			}
		}
		else if(pUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			if(GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3))  //early game?
			{
				bRtnValue =  true;
			}
		}
#if defined(MOD_BALANCE_CORE_MILITARY)
			if (MOD_BALANCE_CORE_MILITARY) 
			{
				//Let's not be too aggressive around our own cities.
				if(pUnit->plot()->GetAdjacentCity() != NULL)
				{
					if(pUnit->plot()->GetAdjacentCity()->getOwner() == m_pPlayer->GetID())
					{
						bRtnValue = false;
					}
				}
				//Is he trying to lure us to come out of a garrison?
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot *pLoopPlot = plotDirection(pUnit->plot()->getX(), pUnit->plot()->getY(), ((DirectionTypes)iI));
					if (pLoopPlot != NULL)
					{
						if((pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false) > 15) && pLoopPlot->getNumUnits() > 0)
						{
							if(pLoopPlot->getUnitByIndex(0)->getOwner() == m_pPlayer->GetID())
							{
								bRtnValue = false;
							}
						}
					}
				}
			}
#endif
	}
	return bRtnValue;
}

/// Log current status of the operation
void CvTacticalAI::LogTacticalMessage(CvString& strMsg, bool bSkipLogDominanceZone)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strTemp;
		FILogFile* pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strTemp = "no zone, ";
		if(!bSkipLogDominanceZone)
		{
			CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
			if(pZone != NULL)
			{
				strTemp.Format("Zone ID: %d, ", pZone->GetDominanceZoneID());
			}
		}
		strBaseString += strTemp;
		strOutBuf = strBaseString + strMsg;

		pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvTacticalAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PlayerTacticalAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PlayerTacticalAILog.csv";
	}

	return strLogName;
}

/// Build log filename
CvString CvTacticalAI::GetTacticalMissionName(AITacticalMission eMission) const
{
	CvString strMissionName;

	// Open the log file
	switch(eMission)
	{
	case AI_TACTICAL_MISSION_NONE:
		strMissionName = "No Tactic";
		break;
	case AI_TACTICAL_MISSION_ATTACK_STATIONARY_TARGET:
		strMissionName = "Attack Stationary Target";
		break;
	case AI_TACTICAL_MISSION_PILLAGE_ENEMY_IMPROVEMENTS:
		strMissionName = "Pillage Enemy Improvements";
	}

	return strMissionName;
}

FDataStream& operator<<(FDataStream& saveTo, const AITacticalMission& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, AITacticalMission& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AITacticalMission>(v);
	return loadFrom;
}

// HELPER FUNCTIONS

/// Sort CvBlockingUnit by a non-standard criteria
bool TacticalAIHelpers::CvBlockingUnitDistanceSort(CvBlockingUnit obj1, CvBlockingUnit obj2)
{
	return obj1.GetDistanceToTarget() < obj2.GetDistanceToTarget();
}