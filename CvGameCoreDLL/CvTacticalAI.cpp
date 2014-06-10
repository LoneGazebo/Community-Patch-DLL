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
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
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
CvTacticalMoveXMLEntry *CvTacticalMoveXMLEntries::GetEntry(int index)
{
	return m_paTacticalMoveEntries[index];
}

/// Clear entries
void CvTacticalMoveXMLEntries::DeleteArray()
{
	for (std::vector<CvTacticalMoveXMLEntry*>::iterator it = m_paTacticalMoveEntries.begin(); it != m_paTacticalMoveEntries.end(); ++it)
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
	if (eType == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
		eType == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		eType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
	{
		CvUnit *pUnit;
		CvPlot *pPlot;
		pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		pUnit = pPlot->getVisibleEnemyDefender(m_eAttackingPlayer);
		if (pUnit != NULL && !pUnit->isDelayedDeath())
		{
			bRtnValue = true;
		}
	}
	else if (eType == AI_TACTICAL_TARGET_CITY)
	{
		CvCity *pCity;
		CvPlot *pPlot;
		pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		pCity = pPlot->getPlotCity();
		if (pCity != NULL && pCity->getOwner() == GetTargetPlayer())
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

	switch (GetTargetType())
	{
		case AI_TACTICAL_TARGET_DEFENSIVE_BASTION:
		case AI_TACTICAL_TARGET_BARBARIAN_CAMP:
		case AI_TACTICAL_TARGET_IMPROVEMENT:
		case AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND:
		case AI_TACTICAL_TARGET_ANCIENT_RUINS:
			bRtnValue = (eDomain == DOMAIN_LAND);
			break;

		case AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT:
		case AI_TACTICAL_TARGET_BOMBARDMENT_ZONE:
		case AI_TACTICAL_TARGET_EMBARKED_CIVILIAN:
		case AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT:
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

FDataStream & operator<<(FDataStream & saveTo, const CvTemporaryZone & readFrom)
{
	uint uiVersion = 1;
	saveTo << uiVersion;

	saveTo << (int)readFrom.GetTargetType();
	saveTo << readFrom.GetLastTurn();
	saveTo << readFrom.GetX();
	saveTo << readFrom.GetY();
	saveTo << (int)readFrom.IsNavalInvasion();
	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, CvTemporaryZone & writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;

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
CvTacticalAI::CvTacticalAI (void)
{
	m_iCurrentZoneIndex = -1;
}

/// Destructor
CvTacticalAI::~CvTacticalAI (void)
{
	Uninit();
}

/// Initialize
void CvTacticalAI::Init(CvPlayer *pPlayer)
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
	m_iTempZoneRadius = GC.getAI_TACTICAL_MAP_TEMP_ZONE_RADIUS();
	m_iDeployRadius = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
	m_iRandomRange = GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS();
	m_fFlavorDampening = GC.getAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION();
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
	for (pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Never want immobile/dead units, explorers, ones that have already moved or automated human units
 		if (pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath() || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN ||  pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || !pLoopUnit->canMove() || pLoopUnit->isHuman())
		{
			continue;
		}

		// We want ALL the barbarians and air units
		else if (pLoopUnit->isBarbarian() || pLoopUnit->getDomainType() == DOMAIN_AIR)
		{
			pLoopUnit->setTacticalMove((TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_UNASSIGNED"));
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}

		// Now down to land and sea units ... in these groups our unit must have a base combat strength ... or be a great general
		else if (!pLoopUnit->IsCombatUnit() && !pLoopUnit->IsGreatGeneral())
		{
			continue;
		}

		else
		{
			// Is this one in an operation we can't interrupt?
			int iArmyID = pLoopUnit->getArmyID();
			const CvArmyAI * army = m_pPlayer->getArmyAI(iArmyID);
			if (iArmyID != FFreeList::INVALID_INDEX && NULL != army && !army->CanTacticalAIInterruptUnit(pLoopUnit->GetID()))
			{
				pLoopUnit->setTacticalMove(NO_TACTICAL_MOVE);
			}

			else
			{
				// Non-zero danger value, near enemy, or deploying out of an operation?
				int iDanger = m_pPlayer->GetPlotDanger(*(pLoopUnit->plot()));
				if (iDanger > 0 || NearVisibleEnemy(pLoopUnit, m_iRecruitRange) ||
					pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
				{
					pLoopUnit->setTacticalMove((TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_UNASSIGNED"));
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
CvTemporaryZone *CvTacticalAI::GetFirstTemporaryZone()
{
	CvTemporaryZone *pRtnValue = NULL;

	m_iCurrentTempZoneIndex = 0;
	if ((int)m_TempZones.size() > m_iCurrentTempZoneIndex)
	{
		pRtnValue = &m_TempZones[m_iCurrentTempZoneIndex];
	}

	return pRtnValue;
}

/// Retrieve next temporary dominance zone, NULL if no more (should follow a call to GetFirstTemporaryZone())
CvTemporaryZone *CvTacticalAI::GetNextTemporaryZone()
{
	CvTemporaryZone *pRtnValue = NULL;

	m_iCurrentTempZoneIndex++;
	if ((int)m_TempZones.size() > m_iCurrentTempZoneIndex)
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
void CvTacticalAI::DeleteTemporaryZone(CvPlot *pPlot)
{
	FStaticVector<CvTemporaryZone, SAFE_ESTIMATE_NUM_TEMP_ZONES, true, c_eCiv5GameplayDLL, 0> zonesCopy;

	// Copy zones over
	zonesCopy = m_TempZones;

	m_TempZones.clear();

	// Copy back to original vector any whose coords don't match
	for (unsigned int iI = 0; iI < zonesCopy.size(); iI++)
	{
		if (zonesCopy[iI].GetX() != pPlot->getX() ||
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
	for (unsigned int iI = 0; iI < zonesCopy.size(); iI++)
	{
		if (zonesCopy[iI].GetLastTurn() >= GC.getGame().getGameTurn())
		{
			m_TempZones.push_back(zonesCopy[iI]);
		}
	}
}

/// Is this a city that an operation just deployed in front of?
bool CvTacticalAI::IsTemporaryZoneCity(CvCity *pCity)
{
	for (unsigned int iI = 0; iI < m_TempZones.size(); iI++)
	{
		if (m_TempZones[iI].GetX() == pCity->getX() &&
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
bool CvTacticalAI::QueueAttack (void *pAttacker, CvTacticalTarget *pTarget, bool bRanged, bool bCityAttack)
{
	bool bRtnValue = true;
	CvQueuedAttack attack;
	int iSeriesID;
	CvPlot *pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
	CvPlot *pFromPlot;

	// Can we find this target in the queue, if so what is its series ID
	int iTempSeriesID = PlotAlreadyTargeted(pTargetPlot);

	if (iTempSeriesID == -1)
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

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		CvString strTemp;
		if (bCityAttack)
		{
			strTemp = ((CvCity *)pAttacker)->getName();
			pFromPlot = ((CvCity *)pAttacker)->plot();
		}
		else
		{
			CvUnit* pkAttackingUnit = static_cast<CvUnit*>(pAttacker);
			strTemp = pkAttackingUnit->getUnitInfo().GetDescription();
			pFromPlot = ((CvUnit *)pAttacker)->plot();
		}
		strMsg.Format ("Queued attack with %s, To X: %d, To Y: %d, From X: %d, From Y: %d", strTemp.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY(), pFromPlot->getX(), pFromPlot->getY());
		LogTacticalMessage(strMsg);
	}

	return bRtnValue;
}

/// Pushes the mission to launch an attack and logs this activity
void CvTacticalAI::LaunchAttack (void *pAttacker, CvTacticalTarget *pTarget, bool bFirstAttack, bool bRanged, bool bCityAttack)
{
	CvCity *pCity = NULL;
	UnitHandle pUnit;

	if (bCityAttack)
	{
		pCity = (CvCity *)pAttacker;
	}
	else
	{
		pUnit = UnitHandle((CvUnit *)pAttacker);
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		CvString strTemp;

		if (bCityAttack)
		{
			strTemp = pCity->getName();
		}
		else
		{
			strTemp = pUnit->getUnitInfo().GetDescription();
		}

		if (bRanged)
		{
			if (bFirstAttack)
			{
				strMsg.Format ("Made initial ranged attack with %s, X: %d, Y: %d", strTemp.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}
			else
			{
				strMsg.Format ("Made follow-on ranged attack with %s, X: %d, Y: %d", strTemp.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}
		else
		{
			if (bFirstAttack)
			{
				strMsg.Format ("Made initial attack with %s, X: %d, Y: %d", strTemp.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}
			else
			{
				strMsg.Format ("Made follow-on attack with %s, X: %d, Y: %d", strTemp.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
			}
		}

		LogTacticalMessage(strMsg);
	}

	// Attack AFTER logging since our attacker could die
	if (bCityAttack)
	{
		pCity->doTask(TASK_RANGED_ATTACK, pTarget->GetTargetX(), pTarget->GetTargetY(), 0);
	}
	else
	{
		CvAssertMsg(pUnit->getMoves() > 0, "Tactical AI is going to push a mission on the queue to attack, but the unit is out of moves.");
		CvAssertMsg(!pUnit->isOutOfAttacks(), "Tactical AI is going to push a mission on the queue to attack, but the unit is out of attacks.");

		bool bSendAttack = pUnit->getMoves() > 0 && !pUnit->isOutOfAttacks();
		if (bSendAttack)
		{
			if (bRanged && pUnit->getDomainType() != DOMAIN_AIR)	// Air attack is ranged, but it goes through the 'move to' mission.
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
		if (!bSendAttack || (!pUnit->isOutOfAttacks() && !pUnit->isFighting()))
		{
			pUnit->SetTurnProcessed(false);
			CombatResolved(pAttacker, false, bCityAttack);
		}
	}
}

/// Handles notification that an attack has been resolved (so can queue another)
void CvTacticalAI::CombatResolved (void *pAttacker, bool bVictorious, bool bCityAttack)
{
	int iSeriesID = 0;
	bool bFoundIt = false;

	UnitHandle pUnit;

	if (!bCityAttack)
	{
		pUnit = UnitHandle((CvUnit *)pAttacker);
	}

	if (m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator nextToErase, nextInList;
		nextToErase = m_QueuedAttacks.begin();

		// Find first attack with this unit/city
		while (nextToErase != m_QueuedAttacks.end())
		{
			if (nextToErase->GetAttacker() == pAttacker)
			{
				iSeriesID = nextToErase->GetSeriesID();
				bFoundIt = true;
				break;
			}
			nextToErase++;
		}

		// Couldn't find it ... could have been an accidental attack moving to deploy near a target
		// So safe to ignore these
		if (!bFoundIt)
		{
			return;
		}

		// If this attacker gets multiple attacks, release him to be processed again
		if (!bCityAttack && pUnit->canMoveAfterAttacking() && pUnit->getMoves() > 0)
		{
			pUnit->SetTurnProcessed(false);
		}

		// If victorious, dump follow-up attacks
		if (bVictorious)
		{
			bool bFirst = true;
			while (nextToErase != m_QueuedAttacks.end() && nextToErase->GetSeriesID() == iSeriesID)
			{
				// Only the first unit being erased is done for the turn
				if (!bFirst && !nextToErase->IsCityAttack())
				{
					UnitHandle pAbortUnit((CvUnit *)nextToErase->GetAttacker());
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
			if (nextInList != m_QueuedAttacks.end() && nextInList->GetSeriesID() == nextToErase->GetSeriesID())
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
int CvTacticalAI::PlotAlreadyTargeted (CvPlot *pPlot)
{
	if (m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			if (it->GetTarget()->GetTargetX() == pPlot->getX() &&
				it->GetTarget()->GetTargetY() == pPlot->getY())
			{
				return it->GetSeriesID();
			}
		}
	}
	return -1;
}

/// Is this unit waiting to get its turn to attack?
bool CvTacticalAI::IsInQueuedAttack(const CvUnit *pUnit)
{
	if (m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			if (it->GetAttacker() == pUnit)
			{
				return true;
			}
		}
	}
	return false;
}

/// Is this unit waiting to get its turn to attack?
bool CvTacticalAI::IsCityInQueuedAttack (const CvCity *pAttackCity)
{
	if (m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			if (it->IsCityAttack() && it->GetAttacker() == (void *)pAttackCity)
			{
				return true;
			}
		}
	}
	return false;
}

/// Is this unit waiting to get its turn to attack?
int CvTacticalAI::NearXQueuedAttacks (const CvPlot *pPlot, const int iRange)
{
	int iNearbyAttacks = 0;

	if (m_QueuedAttacks.size() > 0)
	{
		std::list<CvQueuedAttack>::iterator it;
		for (it = m_QueuedAttacks.begin(); it != m_QueuedAttacks.end(); it++)
		{
			int iDistance;
			iDistance = plotDistance(pPlot->getX(), pPlot->getY(), it->GetTarget()->GetTargetX(), it->GetTarget()->GetTargetY());
			if (iDistance <= iRange)
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
	CvTacticalDominanceZone *pZone;
	AITacticalPosture eLastPosture, eNewPosture;

	m_NewPostures.clear();

	// Loop through all the zones we have this turn
	for (int iI = 0; iI < m_pMap->GetNumZones(); iI++)
	{
		pZone = m_pMap->GetZone(iI);

		// Check to make sure we want to use this zone
		if (UseThisDominanceZone(pZone))
		{
			int iCityID = -1;
			if (pZone->GetClosestCity() != NULL)
			{
				iCityID = pZone->GetClosestCity()->GetID();
			}

			eLastPosture = FindPosture(pZone);
			eNewPosture = SelectPosture(pZone, eLastPosture);
			m_NewPostures.push_back(CvTacticalPosture(pZone->GetOwner(), pZone->IsWater(), iCityID, eNewPosture));

			if (GC.getLogging() && GC.getAILogging() && eNewPosture != AI_TACTICAL_POSTURE_NONE)
			{
				CvString szPostureMsg;
				szPostureMsg.Format("Zone ID: %d, ", pZone->GetDominanceZoneID());
				switch (eNewPosture)
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
AITacticalPosture CvTacticalAI::SelectPosture(CvTacticalDominanceZone *pZone, AITacticalPosture eLastPosture)
{
	AITacticalPosture eChosenPosture = AI_TACTICAL_POSTURE_NONE;
	int iDominancePercentage = GC.getGame().GetTacticalAnalysisMap()->GetDominancePercentage();
	eTacticalDominanceFlags eRangedDominance = TACTICAL_DOMINANCE_EVEN;
	eTacticalDominanceFlags eUnitCountDominance = TACTICAL_DOMINANCE_EVEN;
	int iRatio;

	// Compute who is dominant in various areas
	//   Ranged strength
	if (pZone->GetEnemyRangedStrength() <= 0)
	{
		eRangedDominance = TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		iRatio = (pZone->GetFriendlyRangedStrength() * 100) / pZone->GetEnemyRangedStrength();
		if (iRatio > 100 + iDominancePercentage)
		{
			eRangedDominance = TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if (iRatio < 100 - iDominancePercentage)
		{
			eRangedDominance = TACTICAL_DOMINANCE_ENEMY;
		}
	}

	//   Number of units
	if (pZone->GetEnemyUnitCount() <= 0)
	{
		eUnitCountDominance = TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		iRatio = (pZone->GetFriendlyUnitCount()  * 100) / pZone->GetEnemyUnitCount();
		if (iRatio > 100 + iDominancePercentage)
		{
			eUnitCountDominance = TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if (iRatio < 100 - iDominancePercentage)
		{
			eUnitCountDominance = TACTICAL_DOMINANCE_ENEMY;
		}
	}

	// Choice based on whose territory this is
	switch (pZone->GetTerritoryType())
	{
	case TACTICAL_TERRITORY_ENEMY:
		{
			// Always withdraw if enemy dominant overall
			if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY || pZone->GetFriendlyRangedUnitCount() == pZone->GetFriendlyUnitCount())
			{
				eChosenPosture = AI_TACTICAL_POSTURE_WITHDRAW;
			}

			// Destroy units then assault - for first time need dominance in total strength but not enemy dominance in ranged units OR just double total strength
			else if (pZone->GetEnemyUnitCount() > 0 && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY &&
				     (eRangedDominance != TACTICAL_DOMINANCE_ENEMY || pZone->GetFriendlyStrength() > pZone->GetEnemyStrength() * 2))
			{
				eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_STEAMROLL && pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
			}

			// Sit and bombard - for first time need dominance in ranged strength and total unit count
			else if (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_SIT_AND_BOMBARD;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_SIT_AND_BOMBARD && eRangedDominance != TACTICAL_DOMINANCE_ENEMY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_SIT_AND_BOMBARD;
			}

			// Go right after the city - need tactical dominance
			else if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
			}

			// Exploit flanks - for first time need dominance in unit count
			else if (eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 1)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 1)
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
			if (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE && eRangedDominance != TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
			}

			// Exploit flanks - for first time need dominance in unit count
			else if (eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 0)
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
			if (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetFriendlyRangedUnitCount() > 1)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE && pZone->GetFriendlyRangedUnitCount() > 1 && eRangedDominance != TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
			}

			// Exploit flanks - for first time need dominance in unit count
			else if (eUnitCountDominance == TACTICAL_DOMINANCE_FRIENDLY && pZone->GetEnemyUnitCount() > 0)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && eUnitCountDominance != TACTICAL_DOMINANCE_ENEMY && pZone->GetEnemyUnitCount() > 0)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}

			// Counterattack - for first time must be stronger or even with enemy having a ranged advantage
			else if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY ||
				     pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_EVEN && eRangedDominance == TACTICAL_DOMINANCE_ENEMY)
			{
				eChosenPosture = AI_TACTICAL_POSTURE_COUNTERATTACK;
			}

			//                 - less stringent if continuing this from a previous turn
			else if (eLastPosture == AI_TACTICAL_POSTURE_COUNTERATTACK && pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_ENEMY)
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
			if (pZone->IsWater())
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
AITacticalPosture CvTacticalAI::FindPosture(CvTacticalDominanceZone *pZone)
{
	if (pZone != NULL)
	{
		for (unsigned int iI = 0; iI < m_Postures.size(); iI++)
		{
			if (m_Postures[iI].GetPlayer() == pZone->GetOwner() &&
				m_Postures[iI].IsWater() == pZone->IsWater())
			{
				int iCityID = -1;
				if (pZone->GetClosestCity() != NULL)
				{
					iCityID = pZone->GetClosestCity()->GetID();
				}
				if (m_Postures[iI].GetCityID() == iCityID)
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
	for (int iI = 0; iI < GC.getNumTacticalMoveInfos(); iI++)
	{
		const TacticalAIMoveTypes eTacticalAIMove = static_cast<TacticalAIMoveTypes>(iI);
		CvTacticalMoveXMLEntry* pkTacticalMoveInfo = GC.getTacticalMoveInfo(eTacticalAIMove);
		if(pkTacticalMoveInfo)
		{
			move.m_eMoveType = eTacticalAIMove;
			move.m_iPriority = pkTacticalMoveInfo->GetPriority();

			if (move.m_iPriority >= 0)
			{
				m_MovePriorityList.push_back (move);
			}
		}
	}

	// Now sort the moves in priority order
	std::stable_sort (m_MovePriorityList.begin(), m_MovePriorityList.end());
}


/// Choose which tactics the barbarians should emphasize this turn
void CvTacticalAI::EstablishBarbarianPriorities()
{
	int iPriority = 0;

	// Only establish priorities once per turn
	if (GC.getGame().getGameTurn() <= m_MovePriorityTurn)
	{
		return;
	}

	m_MovePriorityList.clear();
	m_MovePriorityTurn = GC.getGame().getGameTurn();

	// Loop through each possible tactical move (other than "none" or "unassigned")
	for (int iI = 0; iI < NUM_AI_BARBARIAN_MOVES; iI++)
	{
		// Set base value
		switch ((AIBarbarianTacticalMove)iI)
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
		}

		// Make sure base priority is not negative
		if (iPriority >= 0)
		{
			// Finally, add a random die roll to each priority
			iPriority += GC.getGame().getJonRandNum(GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS(), "Tactical AI Move Priority");

			// Store off this move and priority
			CvTacticalMove move;
			move.m_eMoveType = (TacticalAIMoveTypes)iI;
			move.m_iPriority = iPriority;
			m_MovePriorityList.push_back (move);
		}
	}

	// Now sort the moves in priority order
	std::stable_sort (m_MovePriorityList.begin(), m_MovePriorityList.end());
}


/// Make lists of everything we might want to target with the tactical AI this turn
void CvTacticalAI::FindTacticalTargets()
{
	int iI;
	CvPlot *pLoopPlot;
	CvTacticalTarget newTarget;
	bool bValidPlot;
	bool bEnemyDominatedPlot;

	// Clear out target list since we rebuild it each turn
	m_AllTargets.clear();

	bool bBarbsAllowedYet = GC.getGame().getGameTurn() >= GC.getGame().GetBarbarianReleaseTurn();

	// Look at every tile on map
	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		bValidPlot = false;

		if (pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{
			// Make sure I am not a barbarian who can not move into owned territory this early in the game
			if (!m_pPlayer->isBarbarian() || bBarbsAllowedYet)
			{
				bValidPlot = true;
			}
			else
			{
				if (!pLoopPlot->isOwned())
				{
					bValidPlot = true;
				}
			}
			if (bValidPlot)
			{
				if (PlotAlreadyTargeted(pLoopPlot) != -1)
				{
					bValidPlot = false;
				}
			}
		}

		if (bValidPlot)
		{
			newTarget.Clear();
			newTarget.SetTargetX(pLoopPlot->getX());
			newTarget.SetTargetY(pLoopPlot->getY());
			newTarget.SetDominanceZone(GC.getGame().GetTacticalAnalysisMap()->GetCell(iI)->GetDominanceZone());

			bEnemyDominatedPlot = GC.getGame().GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pLoopPlot);

			// Have a ...
			// ... friendly city?
			CvCity* pCity = pLoopPlot->getPlotCity();
			if (pCity != NULL)
			{
				if (m_pPlayer->GetID() == pCity->getOwner())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
					newTarget.SetAuxData((void *)pCity);
					newTarget.SetAuxIntData(pCity->getThreatValue());
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy city
				else if (atWar (m_pPlayer->getTeam(), pCity->getTeam()))
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY);
					newTarget.SetTargetPlayer(pCity->getOwner());
					newTarget.SetAuxData((void *)pCity);
					m_AllTargets.push_back(newTarget);
				}
			}
			else
			{
				// ... enemy unit?
				CvUnit* pUnit = pLoopPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if (pUnit != NULL)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
					newTarget.SetTargetPlayer(pUnit->getOwner());
					newTarget.SetAuxData((void *)pUnit);
					newTarget.SetAuxIntData(pUnit->getDamage());
					m_AllTargets.push_back(newTarget);
				}

				// ... undefended camp?
				else if (pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
					newTarget.SetTargetPlayer(BARBARIAN_PLAYER);
					newTarget.SetAuxData((void *)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... goody hut?
				else if (!m_pPlayer->isMinorCiv() && pLoopPlot->isGoody())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_ANCIENT_RUINS);
					newTarget.SetAuxData((void *)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy resource improvement?
				else if (atWar (m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					!pLoopPlot->IsImprovementPillaged() &&
					pLoopPlot->getResourceType() != NO_RESOURCE
					&& !bEnemyDominatedPlot)
				{
					ResourceTypes eResource = pLoopPlot->getResourceType();
					ResourceUsageTypes eRUT = GC.getResourceInfo(eResource)->getResourceUsage();

					// On land, civs only target improvements built on resources
					if (eRUT == RESOURCEUSAGE_STRATEGIC || eRUT == RESOURCEUSAGE_LUXURY || pLoopPlot->isWater() || m_pPlayer->isBarbarian())
					{
						// Barbarians can't target naval improvements
						if (m_pPlayer->isBarbarian() && pLoopPlot->isWater())
						{
							continue;
						}
						else
						{
							newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
							newTarget.SetTargetPlayer(pLoopPlot->getOwner());
							newTarget.SetAuxData((void *)pLoopPlot);
							m_AllTargets.push_back(newTarget);
						}
					}
				}

				// Or citadels!
				else if (atWar (m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					     pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					     GC.getImprovementInfo(pLoopPlot->getImprovementType())->GetNearbyEnemyDamage() > 0)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void *)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy trade route?
				else if (atWar (m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRouteType() != NO_ROUTE && !pLoopPlot->IsRoutePillaged() && pLoopPlot->IsTradeRoute() && !bEnemyDominatedPlot)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void *)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy civilian (or embarked) unit?
				else if (pLoopPlot->isVisibleOtherUnit(m_pPlayer->GetID()))
				{
					CvUnit *pTargetUnit = pLoopPlot->getUnitByIndex(0);
					if (!pTargetUnit->isDelayedDeath() && atWar (m_pPlayer->getTeam(), pTargetUnit->getTeam()) && !pTargetUnit->IsCanDefend())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
						newTarget.SetTargetPlayer(pTargetUnit->getOwner());
						newTarget.SetAuxData((void *)pTargetUnit);

						if (pTargetUnit->isEmbarked())
						{
							if (pTargetUnit->IsCombatUnit())
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
							if (IsVeryHighPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
							}
							else if (IsHighPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
							}
							else if (IsMediumPriorityCivilianTarget(&newTarget))
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
							}
						}
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... defensive bastion?
				else if (m_pPlayer->GetID() == pLoopPlot->getOwner() &&
					pLoopPlot->defenseModifier(m_pPlayer->getTeam(), true) > 0 &&
					m_pPlayer->GetPlotDanger(*pLoopPlot) > 0)
				{
					CvCity *pDefenseCity = pLoopPlot->GetAdjacentFriendlyCity(m_pPlayer->getTeam(), true/*bLandOnly*/);
					if (pDefenseCity)
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
						newTarget.SetAuxData((void *)pLoopPlot);
						newTarget.SetAuxIntData(pDefenseCity->getThreatValue() + m_pPlayer->GetPlotDanger(*pLoopPlot));
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... friendly improvement?
				else if (m_pPlayer->GetID() == pLoopPlot->getOwner() &&
					     pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					     !pLoopPlot->IsImprovementPillaged() && !pLoopPlot->isGoody())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
					newTarget.SetAuxData((void *)pLoopPlot);
					m_AllTargets.push_back(newTarget);
				}

				//// ... empty water tile near naval resource for barbarians?
				//else if (m_pPlayer->isBarbarian() && pLoopPlot->isWater())
				//{
				//	int iNumNearbyResources = 0;

				//	// We're allowed to blockade from 2 tiles away, but let's just move adjacent
				//	int iRange = 1;
				//	for (int iDX = -iRange; iDX <= iRange; iDX++)
				//	{
				//		for (int iDY = -iRange; iDY <= iRange; iDY++)
				//		{
				//			CvPlot *pPlot = plotXYWithRangeCheck(pLoopPlot->getX(), pLoopPlot->getY(), iDX, iDY, iRange);

				//			if (pPlot != NULL && pPlot->isWater())
				//			{
				//				if (pPlot->getResourceType(m_pPlayer->getTeam()) != NO_RESOURCE && pPlot->isOwned())
				//				{
				//					iNumNearbyResources++;
				//				}
				//			}
				//		}
				//	}
				//	if (iNumNearbyResources > 0)
				//	{
				//		newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT);
				//		newTarget.SetAuxData((void *)pLoopPlot);
				//		newTarget.SetAuxIntData(iNumNearbyResources);

				//		// Store in their own list for now until we can reduce the number of these
				//		m_NavalResourceBlockadePoints.push_back(newTarget);
				//	}
				//}
			}
		}
	}

	// POST-PROCESSING ON TARGETS

	// Mark enemy units threatening our cities (or camps) as priority targets
	if (m_pPlayer->isBarbarian())
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
	CvTacticalDominanceZone *pZone;
	FStaticVector<CvTacticalMove, 100, false, c_eCiv5GameplayDLL >::iterator it;

	// Barbarian processing is straightforward -- just one big list of priorites and everything is considered at once
	if (m_pPlayer->isBarbarian())
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
		for (it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
		{
			CvTacticalMove move = *it;

			if (move.m_iPriority >= 0)
			{
				CvTacticalMoveXMLEntry* pkTacticalMoveInfo = GC.getTacticalMoveInfo(move.m_eMoveType);
				if (pkTacticalMoveInfo && pkTacticalMoveInfo->IsDominanceZoneMove())
				{
					for (int iI = 0; iI < m_pMap->GetNumZones(); iI++)
					{
						m_iCurrentZoneIndex = iI;
						pZone = m_pMap->GetZone(iI);
						AITacticalPosture ePosture = FindPosture(pZone);
						CvString strMoveName = (CvString)pkTacticalMoveInfo->GetType();

						// Is this move of the right type for this zone?
						bool bMatch = false;
						if (strMoveName == "TACTICAL_CLOSE_ON_TARGET")   // This one okay for all zones
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_WITHDRAW && strMoveName == "TACTICAL_POSTURE_WITHDRAW")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_SIT_AND_BOMBARD && strMoveName == "TACTICAL_POSTURE_SIT_AND_BOMBARD")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE && strMoveName == "TACTICAL_POSTURE_ATTRIT_FROM_RANGE")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_EXPLOIT_FLANKS && strMoveName == "TACTICAL_POSTURE_EXPLOIT_FLANKS")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_STEAMROLL && strMoveName == "TACTICAL_POSTURE_STEAMROLL")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE && strMoveName == "TACTICAL_POSTURE_SURGICAL_CITY_STRIKE")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_HEDGEHOG && strMoveName == "TACTICAL_POSTURE_HEDGEHOG")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_COUNTERATTACK && strMoveName == "TACTICAL_POSTURE_COUNTERATTACK")
						{
							bMatch = true;
						}
						else if (ePosture == AI_TACTICAL_POSTURE_SHORE_BOMBARDMENT && strMoveName == "TACTICAL_POSTURE_SHORE_BOMBARDMENT")
						{
							bMatch = true;
						}
						else if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && strMoveName == "TACTICAL_EMERGENCY_PURCHASES")
						{
							bMatch = true;
						}

						if (bMatch)
						{
							if (!UseThisDominanceZone(pZone))
							{
								continue;
							}

							ExtractTargetsForZone(pZone);

							// Must have some moves to continue or it must be land around an enemy city (which we always want to process because
							// we might have an operation targeting it)
							if (m_ZoneTargets.size() <= 0 && pZone->GetTerritoryType() != TACTICAL_TERRITORY_TEMP_ZONE && (pZone->GetTerritoryType() != TACTICAL_TERRITORY_ENEMY || pZone->IsWater()))
							{
								continue;
							}

							AssignTacticalMove(move);
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

	if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_MOVE_NONCOMBATANTS_TO_SAFETY"))
	{
		PlotMovesToSafety(false /*bCombatUnits*/);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_CAPTURE_CITY"))
	{
		PlotCaptureCityMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_DAMAGE_CITY"))
	{
		PlotDamageCityMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_DESTROY_HIGH_UNIT"))
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_DESTROY_MEDIUM_UNIT"))
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_DESTROY_LOW_UNIT"))
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_TO_SAFETY"))
	{
		PlotMovesToSafety(true /*bCombatUnits*/);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTRIT_HIGH_UNIT"))
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTRIT_MEDIUM_UNIT"))
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTRIT_LOW_UNIT"))
	{
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_REPOSITION"))
	{
		PlotRepositionMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_BARBARIAN_CAMP"))
	{
		PlotBarbarianCampMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_PILLAGE"))
	{
		PlotPillageMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTACK_VERY_HIGH_PRIORITY_CIVILIAN"))
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN"))
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN"))
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN"))
	{
		PlotCivilianAttackMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_SAFE_BOMBARDS"))
	{
		PlotSafeBombardMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_HEAL"))
	{
		PlotHealMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ANCIENT_RUINS"))
	{
		PlotAncientRuinMoves(1);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GARRISON_TO_ALLOW_BOMBARD"))
	{
		PlotGarrisonMoves(1, true /*bMustAllowRangedAttack*/);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GARRISON_ALREADY_THERE"))
	{
		PlotGarrisonMoves(0);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_BASTION_ALREADY_THERE"))
	{
		PlotBastionMoves(0);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE"))
	{
		PlotGuardImprovementMoves(0);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GARRISON_1_TURN"))
	{
		PlotGarrisonMoves(1);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_BASTION_1_TURN"))
	{
		PlotBastionMoves(1);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_1_TURN"))
	{
		PlotGuardImprovementMoves(1);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_AIR_INTERCEPT"))
	{
		PlotAirInterceptMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_SIT_AND_BOMBARD"))
	{
		PlotSitAndBombardMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_ATTRIT_FROM_RANGE"))
	{
		PlotAttritFromRangeMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_EXPLOIT_FLANKS"))
	{
		PlotExploitFlanksMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_STEAMROLL"))
	{
		PlotSteamrollMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_SURGICAL_CITY_STRIKE"))
	{
		PlotSurgicalCityStrikeMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_HEDGEHOG"))
	{
		PlotHedgehogMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_COUNTERATTACK"))
	{
		PlotCounterattackMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_WITHDRAW"))
	{
		PlotWithdrawMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_POSTURE_SHORE_BOMBARDMENT"))
	{
		PlotShoreBombardmentMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_CLOSE_ON_TARGET"))
	{
		PlotCloseOnTarget();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_MOVE_OPERATIONS"))
	{
		PlotOperationalArmyMoves();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_EMERGENCY_PURCHASES"))
	{
		PlotEmergencyPurchases();
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTACK_EMBARKED_MILITARY_UNIT"))
	{
		PlotEmbarkedUnitRunoverMoves(AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ATTACK_EMBARKED_CIVILIAN"))
	{
		PlotEmbarkedUnitRunoverMoves(AI_TACTICAL_TARGET_EMBARKED_CIVILIAN);
	}
}

/// Choose which tactics to run and assign units to it (barbarian version)
void CvTacticalAI::AssignBarbarianMoves()
{
	FStaticVector<CvTacticalMove, 100, false, c_eCiv5GameplayDLL >::iterator it;

	// Proceed in priority order
	for (it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
	{
		CvTacticalMove move = *it;

		AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Move: %d, Turn %03d, %s", (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

		switch (move.m_eMoveType)
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
			PlotPillageMoves();
			break;
		case AI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE:
//			PlotBlockadeImprovementMoves();
			break;
		case AI_TACTICAL_BARBARIAN_CIVILIAN_ATTACK:
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
			PlotCivilianAttackMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
			PlotEmbarkedUnitRunoverMoves(AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT);
			PlotEmbarkedUnitRunoverMoves(AI_TACTICAL_TARGET_EMBARKED_CIVILIAN);
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
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while (pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (pPlot != NULL)
		{
			m_CurrentMoveCities.clear();
			if (FindUnitsWithinStrikingDistance(pPlot, 1, false /* bNoRangedUnits */))
			{
				// Do we have enough firepower to destroy it?
				CvCity *pCity = pPlot->getPlotCity();
				if (pCity != NULL)
				{
					iRequiredDamage = GC.getMAX_CITY_HIT_POINTS() - pCity->getDamage();
					pTarget->SetAuxIntData(iRequiredDamage);
					if (ComputeTotalExpectedDamage(pTarget, pPlot) >= iRequiredDamage)
					{
						// Log result
						if (bLog)
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
						if (pPlot->getOwner() == m_pPlayer->GetID())
						{
							DeleteTemporaryZone(pPlot);
						}
					}
				}
			}
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
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while (pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (pPlot != NULL)
		{
			m_CurrentMoveCities.clear();
			if (FindUnitsWithinStrikingDistance(pPlot, 1, false /* bNoRangedUnits */))
			{
				CvCity *pCity = pPlot->getPlotCity();
				if (pCity != NULL)
				{
					iRequiredDamage = GC.getMAX_CITY_HIT_POINTS() - pCity->getDamage();
					pTarget->SetAuxIntData(iRequiredDamage);

					// Don't want to hammer away to try and take down a city for more than 4 turns
					if (ComputeTotalExpectedDamage(pTarget, pPlot) > (iRequiredDamage / 4))
					{
						// Log result
						if (bLog)
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
		}
		pTarget = GetNextZoneTarget();
	}
	return bAttackMade;
}

/// Assign a unit to capture an undefended barbarian camp
void CvTacticalAI::PlotBarbarianCampMoves()
{
	bool bLog = GC.getLogging() && GC.getAILogging();
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	while (pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsWithinStrikingDistance(pPlot, 1, false /* bNoRangedUnits */, false /*bNavalOnly*/, true /*bMustMoveThrough*/))
		{
			// Queue best one up to capture it
			ExecuteBarbarianCampMove(pPlot);
			if (bLog)
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
	while (pTarget != NULL)
	{
		bool bUnitCanAttack = false;
		bool bCityCanAttack = false;

		// See what units we have who can reach targets this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		UnitHandle pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
		if (pDefender)
		{
			bUnitCanAttack = FindUnitsWithinStrikingDistance(pPlot, 1, false /* bNoRangedUnits */);
			bCityCanAttack = FindCitiesWithinStrikingDistance(pPlot);
			if (bUnitCanAttack || bCityCanAttack)
			{
				iExpectedDamage = ComputeTotalExpectedDamage(pTarget, pPlot);
				iExpectedDamage += ComputeTotalExpectedBombardDamage(pDefender);
				iRequiredDamage = pDefender->GetCurrHitPoints();
				pTarget->SetAuxIntData(iRequiredDamage);

				if (!bMustBeAbleToKill)
				{
					// Put in any attacks where we'll inflict at least equal damage
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;

						CvString strPlayerName = GET_PLAYER(pDefender->getOwner()).getCivilizationShortDescription();

						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pDefender->getUnitType());
						CvString strTemp = (pkUnitInfo != NULL)? pkUnitInfo->GetDescription() : "Unknown Unit Type";

						switch (targetType)
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
					if (bAttackAtPoorOdds)
					{
						ExecuteAttack(pTarget, pPlot, false, false);
					}
					// If we can at least knock the defender to half strength with our combined efforts, go ahead even if each individual attack isn't favorable
					else
					{
						bool bMustInflictWhatWeTake = true;
						if (iExpectedDamage >= iRequiredDamage / 2)
						{
							bMustInflictWhatWeTake = false;
						}
						ExecuteAttack(pTarget, pPlot, bMustInflictWhatWeTake, true);
					}
				}

				// Do we have enough firepower to destroy it?
				else
				{
					if (iExpectedDamage > iRequiredDamage)
					{
						// If so, execute enough moves to destroy it
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString, strTemp, strPlayerName;
							strPlayerName = GET_PLAYER(pDefender->getOwner()).getCivilizationShortDescription();
							strTemp = GC.getUnitInfo(pDefender->getUnitType())->GetDescription();
							switch (targetType)
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
void CvTacticalAI::PlotMovesToSafety (bool bCombatUnits)
{
	int iDangerLevel;

	list<int>::iterator it;
	m_CurrentMoveUnits.clear();

	// Loop through all recruited units
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Danger value of plot must be greater than 0
			CvPlot *pPlot = pUnit->plot();

			iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
			if (iDangerLevel > 0)
			{
				bool bAddUnit = false;
				if (bCombatUnits)
				{
					// If under 100% health, might flee to safety
					if (pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
					{
						if (pUnit->isBarbarian())
						{
							// Barbarian combat units - only naval units flee (but they flee if have taken ANY damage)
							if (pUnit->getDomainType() == DOMAIN_SEA)
							{
								bAddUnit = true;
							}
						}

						// Everyone else flees at less than or equal to 50% combat strength
						else if (pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
						{
							bAddUnit = true;
						}
					}

					// Also flee if danger is really high in current plot (but not if we're barbarian)
					else if (!pUnit->isBarbarian())
					{
						int iAcceptableDanger;
						iAcceptableDanger = pUnit->GetBaseCombatStrengthConsideringDamage() * 100;
						if (iDangerLevel > iAcceptableDanger)
						{
							bAddUnit = true;
						}
					}
				}
				else
				{
					// Civilian (or embarked) units always flee from danger
					if (!pUnit->IsCanDefend())
					{
						bAddUnit = true;
					}
				}

				if (bAddUnit)
				{
					// Just one unit involved in this move to execute
					CvTacticalUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
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
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			unit.SetID(pUnit->GetID());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteRepositionMoves();
	}
}

/// Move barbarians across the map
void CvTacticalAI::PlotBarbarianMove(bool bAggressive)
{
	list<int>::iterator it;
	CvTacticalUnit unit;

	if (m_pPlayer->isBarbarian())
	{
		m_CurrentMoveUnits.clear();

		// Loop through all recruited units
		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);
			if (pUnit)
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}

		if (m_CurrentMoveUnits.size() > 0)
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

	if (m_pPlayer->isBarbarian())
	{
		m_CurrentMoveUnits.clear();

		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);

			// Find any civilians we may have "acquired" from the civs
			if (!pUnit->IsCanAttack())
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}

		if (m_CurrentMoveUnits.size() > 0)
		{
			ExecuteBarbarianCivilianEscortMove();
		}
	}
}

/// Process units that we recruited out of operational moves.  Haven't used them, so let them go ahead with those moves
void CvTacticalAI::PlotOperationalArmyMoves()
{
	list<int>::iterator it;

	// Update all operations (moved down - previously was in the PlayerAI object)
	CvAIOperation *nextOp = NULL;
	nextOp = m_pPlayer->getFirstAIOperation();
	while (nextOp != NULL)
	{
		if (nextOp->GetLastTurnMoved() < GC.getGame().getGameTurn())
		{
			switch (nextOp->GetMoveType())
			{
			case AI_OPERATION_MOVETYPE_SINGLE_HEX:
				PlotSingleHexOperationMoves((CvAIEscortedOperation *)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_ENEMY_TERRITORY:
				PlotEnemyTerritoryOperationMoves((CvAIEnemyTerritoryOperation *)nextOp);
				break;

			case AI_OPERATION_MOVETYPE_NAVAL_ESCORT:
				PlotNavalEscortOperationMoves((CvAINavalEscortedOperation *)nextOp);
			    break;

			case AI_OPERATION_MOVETYPE_FREEFORM_NAVAL:
				PlotFreeformNavalOperationMoves((CvAINavalOperation *)nextOp);
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
		while (nextOp != NULL)
		{
			if (nextOp->DoDelayedDeath())
			{
				bKilledSomething = true;
				break;
			}
			nextOp = m_pPlayer->getNextAIOperation();
		}
		// hack
	} while (bKilledSomething);
}

/// Assigns units to pillage enemy improvements
void CvTacticalAI::PlotPillageMoves()
{
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_IMPROVEMENT);
	while (pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsWithinStrikingDistance(pPlot, 0, false /* bNoRangedUnits */, false /*bNavalOnly*/, true /*bMustMoveThrough*/))
		{
			// Queue best one up to capture it
			ExecutePillage(pPlot);

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Pillaging improvement, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		// No one can reach it this turn, what about next turn?
		else if (FindUnitsWithinStrikingDistance(pPlot, 2, false /* bNoRangedUnits */, false /*bNavalOnly*/, true /*bMustMoveThrough*/))
		{
			ExecuteMoveToTarget(pPlot);

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving toward improvement for pillage, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}

		pTarget = GetNextZoneTarget();
	}
}

/// Move barbarian ships to disrupt usage of water improvements
void CvTacticalAI::PlotBlockadeImprovementMoves()
{
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BLOCKADE_RESOURCE_POINT);
	while (pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsWithinStrikingDistance(pPlot, 2, false /* bNoRangedUnits */, true /* bNavalOnly */, true /*bMustMoveThrough*/))
		{
			// Queue best one up to capture it
			ExecuteNavalBlockadeMove(pPlot);

			if (GC.getLogging() && GC.getAILogging())
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
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while (pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsWithinStrikingDistance(pPlot, 1, false /* bNoRangedUnits */, false /*bNavalOnly*/, true /*bMustMoveThrough*/))
		{
			// Queue best one up to capture it
			ExecuteCivilianCapture(pPlot);

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				switch (eTargetType)
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

/// Assigns units to run over embarked units
void CvTacticalAI::PlotEmbarkedUnitRunoverMoves(AITacticalTargetType eTargetType)
{
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(eTargetType);
	while (pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsWithinStrikingDistance(pPlot, 1, false /* bNoRangedUnits */, true /*bNavalOnly*/, true /*bMustMoveThrough*/))
		{
			// Queue best one up to capture it
			ExecuteCivilianCapture(pPlot);

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				switch (eTargetType)
				{
				case AI_TACTICAL_TARGET_EMBARKED_CIVILIAN:
					strLogString.Format("Running over embarked civilian, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
					break;
				case AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT:
					strLogString.Format("Running over embarked military, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
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
	CvTacticalTarget *pTarget;

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
	while (pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot *pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
	while (pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot *pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
	while (pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot *pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		m_pMap->SetTargetBombardCells(pTargetPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_EMBARKED_MILITARY_UNIT);
	while (pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		CvPlot *pTargetPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
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
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Am I under 100% health and not embarked or already in a city?
			if (pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints() && !pUnit->isEmbarked() && !pUnit->plot()->isCity())
			{
				// If I'm a naval unit I need to be in friendly territory
				if (pUnit->getDomainType() != DOMAIN_SEA || pUnit->plot()->IsFriendlyTerritory(m_pPlayer->GetID()))
				{
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Healing at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strLogString);
					}
				}
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteHeals();
	}
}

/// Assigns a barbarian to go protect an undefended camp
void CvTacticalAI::PlotCampDefenseMoves()
{
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	while (pTarget != NULL)
	{
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsWithinStrikingDistance(pPlot, 1, true /* bNoRangedUnits */, false /*bNavalOnly*/, true /*bMustMoveThrough*/))
		{
			ExecuteMoveToPlot(pPlot);

			if (GC.getLogging() && GC.getAILogging())
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
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
	while (pTarget != NULL)
	{
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvCity *pCity = pPlot->getPlotCity();

		if (pCity && pCity->GetLastTurnGarrisonAssigned() < GC.getGame().getGameTurn())
		{
			// Grab units that make sense for this move type
			FindUnitsForThisMove((TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GARRISON_ALREADY_THERE"), pPlot, iNumTurnsAway, bMustAllowRangedAttack);

			if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				ExecuteMoveToTarget(pPlot, true /*bGarrisonIfPossible*/);
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Garrison, X: %d, Y: %d, Priority: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iNumTurnsAway);
					if (bMustAllowRangedAttack)
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
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
	while (pTarget != NULL)
	{
		// Grab units that make sense for this move type
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_BASTION_ALREADY_THERE"), pPlot, iNumTurnsAway);

		if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);
			if (GC.getLogging() && GC.getAILogging())
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
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
	while (pTarget != NULL)
	{
		// Grab units that make sense for this move type
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_BASTION_ALREADY_THERE"), pPlot, iNumTurnsAway);

		if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);
			if (GC.getLogging() && GC.getAILogging())
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
	CvTacticalTarget *pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_ANCIENT_RUINS);
	while (pTarget != NULL)
	{
		// Grab units that make sense for this move type
		CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ANCIENT_RUINS"), pPlot, iNumTurnsAway);

		if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMoveToTarget(pPlot);

			if (GC.getLogging() && GC.getAILogging())
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

	// Loop through all recruited units
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Am I eligible to intercept?
			if (pUnit->canAirPatrol(NULL))
			{
				CvPlot* pUnitPlot = pUnit->plot();
				CvCity* pCity = pUnitPlot->getPlotCity();

				if (pCity && m_pPlayer->GetPlotDanger(*pUnitPlot) > 0)
				{
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Ready to intercept enemy air units at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strLogString);
					}
				}
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAirInterceptMoves();
	}
}

/// Spend money to buy defenses
void CvTacticalAI::PlotEmergencyPurchases()
{
	CvCity *pCity;
	UnitHandle pCityDefender;
	bool bRangedUnitInCity = false;
	CvUnit *pUnit;

	if (m_pPlayer->isMinorCiv())
	{
		return;
	}

	// Is this a dominance zone where we're defending a city?
	CvTacticalDominanceZone *pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	pCity = pZone->GetClosestCity();
	if (pCity && pCity->getOwner() == m_pPlayer->GetID() && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
	{
		// Make sure the city isn't about to fall.  Test by seeing if there are high priority unit targets
		for (unsigned int iI = 0; iI < m_ZoneTargets.size() && !pCity->isCapital(); iI++)
		{
			if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				return;   // Abandon hope for this city; save our money to use elsewhere
			}
		}

		m_pPlayer->GetMilitaryAI()->BuyEmergencyBuilding(pCity);

		// Unit already in our city?
		pCityDefender = pCity->plot()->getBestDefender(m_pPlayer->GetID());
		if (pCityDefender)
		{
			bRangedUnitInCity = pCityDefender->IsCanAttackRanged();
		}

		if (!bRangedUnitInCity)
		{
			pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_CITY_BOMBARD, pCity);
			if (!pUnit)
			{
				pUnit = m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_RANGED, pCity);
			}
		}
	}
}

// PLOT MOVES FOR ZONE TACTICAL POSTURES

/// Win an attrition campaign with bombardments
void CvTacticalAI::PlotSitAndBombardMoves()
{
	// Find primary target for zone (i.e. enemy city)
	CvTacticalTarget target;
	CvPlot *pTarget;
	bool bAttackUnderway;
	bool bAttackMade;

	CvTacticalDominanceZone *pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	if (pZone->GetClosestCity() != NULL)
	{
		// Turn off moves we don't want to mix with this posture
		TurnOffMove((TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_DAMAGE_CITY"));

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
		if (target.IsTargetStillAlive(m_pPlayer->GetID()))
		{
			bAttackMade = PlotCaptureCityMoves();
			if (bAttackMade)
			{
				bAttackUnderway = true;
			}
			if (target.IsTargetStillAlive(m_pPlayer->GetID()))
			{
				bAttackMade = ExecuteProtectedBombards(target, bAttackUnderway);
				if (bAttackMade)
				{
					bAttackUnderway = true;
				}
				if (target.IsTargetStillAlive(m_pPlayer->GetID()))
				{
					PlotCaptureCityMoves();
				}
			}
		}

		// Attack ancillary target (nearby units)
		for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
		{
			// Is the target of an appropriate type?
			if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{
					m_pMap->ClearDynamicFlags();
					pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
					m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

					bAttackUnderway = ExecuteSafeBombards(m_ZoneTargets[iI]);
					if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
					{
						ExecuteProtectedBombards(m_ZoneTargets[iI], bAttackUnderway);
					}
				}
			}
		}
	}
}

/// Win an attrition campaign with bombardments
void CvTacticalAI::PlotAttritFromRangeMoves()
{
	CvPlot *pTarget;
	bool bAttackUnderway;

	// Find unit targets
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				bAttackUnderway = ExecuteSafeBombards(m_ZoneTargets[iI]);
				if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
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
	m_TempTargets.clear();

	// Loop through unit targets finding attack for this turn
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			m_pMap->ClearDynamicFlags();
			CvPlot* pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			m_pMap->SetTargetFlankBonusCells(pTarget);

			bool bAttackMade = ExecuteFlankAttack(m_ZoneTargets[iI]);

			// No attack, see if we can set one up on this target later
			if(!bAttackMade)
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
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}

	// See if there are any other anti-unit attacks we can make.
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	// See if it is time to go after the city
	bAttackMade = PlotCaptureCityMoves();

	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, true);

	if (!bAttackMade)
	{
		PlotDamageCityMoves();
	}
}

/// We should be strong enough to take out the city before the enemy can whittle us down with ranged attacks
void CvTacticalAI::PlotSurgicalCityStrikeMoves()
{
	CvTacticalTarget target;
	bool bAttackUnderway;

	CvTacticalDominanceZone *pZone = m_pMap->GetZone(m_iCurrentZoneIndex);
	if (pZone->GetClosestCity() != NULL)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetClosestCity()->getOwner());
		target.SetTargetX(pZone->GetClosestCity()->plot()->getX());
		target.SetTargetY(pZone->GetClosestCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());
		bAttackUnderway = PlotCaptureCityMoves();

		// Any unit targets adjacent to city?
		for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
		{
			if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if (plotDistance(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY(), target.GetTargetX(), target.GetTargetY()) == 1)
				{
					if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
					{
						ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
					}
				}
			}
		}

		if (target.IsTargetStillAlive(m_pPlayer->GetID()))
		{
			PlotDamageCityMoves();
		}

		// Take any other really good attacks we've set up
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
	}
}

/// Build a defensive shell around this city
void CvTacticalAI::PlotHedgehogMoves()
{
	// Attack priority unit targets
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
		{
			if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}

	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);

	// But after best attacks are exhausted, go right to playing defense
	CvTacticalDominanceZone *pZone = m_pMap->GetZone(m_iCurrentZoneIndex);
	if (pZone->GetClosestCity() != NULL)
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
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
		{
			if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				ExecutePriorityAttacksOnUnitTarget(m_ZoneTargets[iI]);
			}
		}
	}
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);

	// Now low priority targets
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
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

	CvTacticalDominanceZone *pZone = m_pMap->GetZone(m_iCurrentZoneIndex);

	// Loop through all recruited units
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Proper domain of unit?
			if (pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{
				// Am I in the current dominance zone?
				int iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
				if (m_pMap->GetCell(iPlotIndex)->GetDominanceZone() == m_iCurrentZoneIndex)
				{
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}

				// Or am I within tactical recruit range of center of zone and in danger and not in friendly territory?
				else if (pZone->GetClosestCity() != NULL)
				{
					if (plotDistance(pZone->GetClosestCity()->getX(), pZone->GetClosestCity()->getY(), pUnit->getX(), pUnit->getY()) <= m_iRecruitRange)
					{
						CvPlot *pPlot = pUnit->plot();
						if (m_pPlayer->GetPlotDanger(*pPlot) > 0)
						{
							if (pUnit->plot()->getOwner() != pUnit->getOwner())
							{
								unit.SetID(pUnit->GetID());
								m_CurrentMoveUnits.push_back(unit);
							}
						}
					}
				}

				// Or we are withdrawing from unowned lands zone and this is a plot in danger
				else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER)
				{
					CvPlot *pPlot = pUnit->plot();
					if (m_pPlayer->GetPlotDanger(*pPlot) > 0)
					{
						if (pUnit->plot()->getOwner() != pUnit->getOwner())
						{
							unit.SetID(pUnit->GetID());
							m_CurrentMoveUnits.push_back(unit);
						}
					}
				}
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWithdrawMoves();
	}
}

/// Bombard any enemy targets we can get to on shore
void CvTacticalAI::PlotShoreBombardmentMoves()
{
	CvPlot *pTarget;

	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
				m_pMap->ClearDynamicFlags();
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

				ExecuteSafeBombards(m_ZoneTargets[iI]);
			}
		}
	}
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
		{
			if (m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
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
void CvTacticalAI::PlotCloseOnTarget()
{
	// Flank attacks done; if in an enemy zone, close in on target
	CvTacticalTarget target;
	CvTacticalDominanceZone *pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
	if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE && pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_ENEMY)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		target.SetTargetPlayer(BARBARIAN_PLAYER);
		target.SetTargetX(pZone->GetTempZoneCenter()->getX());
		target.SetTargetY(pZone->GetTempZoneCenter()->getY());
		target.SetDominanceZone(pZone->GetDominanceZoneID());

		ExecuteCloseOnTarget(target, pZone);
	}
	else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY && pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_ENEMY && pZone->GetClosestCity() != NULL)
	{
		// If we can't see the city, be careful advancing on it.  We want to be sure we're not heavily outnumbered
		if (pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_NOT_VISIBLE || pZone->GetFriendlyStrength() > (pZone->GetEnemyStrength() / 2))
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
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Barbarians and air units aren't handled by the operational or homeland AIs
			if (pUnit->isBarbarian() || pUnit->getDomainType() == DOMAIN_AIR)
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				pUnit->SetTurnProcessed(true);

				if (GC.getLogging() && GC.getAILogging())
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
void CvTacticalAI::PlotSingleHexOperationMoves(CvAIEscortedOperation *pOperation)
{
	int iUnitID;
	UnitHandle pCivilian;
	UnitHandle pEscort;
	CvPlot *pCivilianMove;
	CvPlot *pEscortMove;
	bool bSaveMoves = false;

	// Simplification - assume only 1 army per operation now
	if (pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());
	iUnitID = pThisArmy->GetFirstUnitID();
	if (iUnitID != -1)
	{
		pCivilian = m_pPlayer->getUnit(iUnitID);
	}

	if (!pCivilian)
	{
		return;
	}

	// ESCORT AND CIVILIAN MEETING UP
	if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE ||
		pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
		if (!pEscort || pEscort->TurnProcessed())
		{
			// Escort died or was poached for other tactical action, operation will clean itself up when call CheckOnTarget()
			return;
		}

		// Check to make sure escort can get to civilian
		if (pEscort->GeneratePath(pCivilian->plot()))
		{
			// He can, so have civilian remain in place
			ExecuteMoveToPlot(pCivilian, pCivilian->plot());

			if (pThisArmy->GetNumSlotsFilled() > 1)
			{
				if (pEscort)
				{
					// Move escort over
					ExecuteMoveToPlot(pEscort, pCivilian->plot());
					if (GC.getLogging() && GC.getAILogging())
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
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pPlot = plotDirection(pCivilian->getX(), pCivilian->getY(), ((DirectionTypes)iI));
				if (pPlot != NULL)
				{
					// Has to be somewhere we can move and be empty of other units
					if (pEscort->canEnterTerrain(*pPlot) && pEscort->canEnterTerritory(pPlot->getTeam()))
					{
						if (pPlot->getNumUnits() == 0)
						{
							if (pEscort->GeneratePath(pPlot) && pCivilian->GeneratePath(pPlot))
							{
								ExecuteMoveToPlot(pEscort, pPlot);
								ExecuteMoveToPlot(pCivilian, pPlot);
								if (GC.getLogging() && GC.getAILogging())
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
			if (GC.getLogging() && GC.getAILogging())
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
		if (pCivilian->plot() != pOperation->GetTargetPlot())
		{
			// Look at where we'd move this turn taking units into consideration
			int iFlags = 0;
			if (pThisArmy->GetNumSlotsFilled() > 1)
			{
				iFlags = MOVE_UNITS_IGNORE_DANGER;
			}

			// Handle case of no path found at all for civilian
			if (!pCivilian->GeneratePath(pOperation->GetTargetPlot(), iFlags, false /*bReuse*/))
			{
				pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
				pCivilian->finishMoves();
				pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());
				if (pEscort)
				{
					pEscort->finishMoves();
				}
				if (GC.getLogging() && GC.getAILogging())
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
				if (pThisArmy->GetNumSlotsFilled() == 1)
				{
					ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
					if (GC.getLogging() && GC.getAILogging())
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
					pEscort = m_pPlayer->getUnit(pThisArmy->GetNextUnitID());

					// See if escort can move to the same location in one turn
					if (TurnsToReachTarget(pEscort, pCivilianMove) <= 1)
					{
						ExecuteMoveToPlot(pEscort, pCivilianMove);
						ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
						if (GC.getLogging() && GC.getAILogging())
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
						CvTacticalAnalysisCell *pCell = GC.getGame().GetTacticalAnalysisMap()->GetCell(pCivilianMove->GetPlotIndex());
						UnitHandle pBlockingUnit = pCivilianMove->getBestDefender(m_pPlayer->GetID());

						// See if friendly blocking unit is ending the turn there, or if no blocking unit (which indicates this is somewhere civilian
						// can move that escort can't -- like minor civ territory), then find a new path based on moving the escort
						if (pCell->IsFriendlyTurnEndTile() || !pBlockingUnit)
						{
							if (!pEscort->GeneratePath(pOperation->GetTargetPlot(), 0, false /*bReuse*/))
							{
								pOperation->RetargetCivilian(pCivilian.pointer(), pThisArmy);
								pCivilian->finishMoves();
								pEscort->finishMoves();
								if (GC.getLogging() && GC.getAILogging())
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
								if (TurnsToReachTarget(pCivilian, pEscortMove) <= 1)
								{
									ExecuteMoveToPlot(pEscort, pEscortMove);
									ExecuteMoveToPlot(pCivilian, pEscortMove, bSaveMoves);
									if (GC.getLogging() && GC.getAILogging())
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
									if (GC.getLogging() && GC.getAILogging())
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
							if (ExecuteMoveOfBlockingUnit(pBlockingUnit))
							{
								ExecuteMoveToPlot(pEscort, pCivilianMove);
								ExecuteMoveToPlot(pCivilian, pCivilianMove, bSaveMoves);
								if (GC.getLogging() && GC.getAILogging())
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
								if (GC.getLogging() && GC.getAILogging())
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
void CvTacticalAI::PlotEnemyTerritoryOperationMoves(CvAIEnemyTerritoryOperation *pOperation)
{
	// Simplification - assume only 1 army per operation now
	if (pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());

	m_OperationUnits.clear();
	m_GeneralsToMove.clear();
	pThisArmy->UpdateCheckpointTurns();

	// RECRUITING
	if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		// If no estimate for when recruiting will end, let the rest of the AI use these units
		if (pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					// See if we are just able to get to muster point in time.  If so, time for us to head over there
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit && !pUnit->TurnProcessed())
					{
						// Great general?
						if (pUnit->IsGreatGeneral())
						{
							if (pUnit->getMoves() > 0)
							{
								CvOperationUnit operationUnit;
								operationUnit.SetUnitID(pUnit->GetID());
								operationUnit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_CIVILIAN_SUPPORT"));
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
								if (pSlot->HasStartedOnOperation())
								{
									MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
								}

								else
								{
									// See if we are just able to get to muster point in time.  If so, time for us to head over there
									int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true /*bReusePaths*/, true, true);
									if (iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
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
			ExecuteFormationMoves(pThisArmy);
		}
	}

	// GATHERING FORCES
	else if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		ClearEnemiesNearArmy(pThisArmy);

		// Request moves for all units
		for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
			if (pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if (pUnit && !pUnit->TurnProcessed())
				{
					// Great general?
					if (pUnit->IsGreatGeneral())
					{
						if (pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_CIVILIAN_SUPPORT"));
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
		ExecuteFormationMoves(pThisArmy);
	}

	// MOVING TO TARGET
	else if (pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		// Update army's current location
		CvPlot *pThisTurnTarget;
		pThisTurnTarget = pOperation->ComputeCenterOfMassForTurn(pThisArmy);
		if (pThisTurnTarget == NULL)
		{
			return;
		}

		pThisArmy->SetXY(pThisTurnTarget->getX(), pThisTurnTarget->getY());
		ClearEnemiesNearArmy(pThisArmy);

		// Request moves for all units
		for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
			if (pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if (pUnit && !pUnit->TurnProcessed())
				{
					// Great general?
					if (pUnit->IsGreatGeneral())
					{
						if (pUnit->getMoves() > 0)
						{
							CvOperationUnit operationUnit;
							operationUnit.SetUnitID(pUnit->GetID());
							operationUnit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_CIVILIAN_SUPPORT"));
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
		ExecuteFormationMoves(pThisArmy);
	}

	if (m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral(pThisArmy);
	}
}

/// Move a large army to its destination against an enemy target
void CvTacticalAI::PlotNavalEscortOperationMoves(CvAINavalEscortedOperation *pOperation)
{
	CvUnit *pCivilian = 0, *pEscort = 0;
	int iUnitID;

	// Simplification - assume only 1 army per operation now
	if (pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());
	iUnitID = pThisArmy->GetFirstUnitID();
	if (iUnitID != -1)
	{
		pCivilian = m_pPlayer->getUnit(iUnitID);

		iUnitID = pThisArmy->GetNextUnitID();
		if (iUnitID != -1)
		{
			pEscort = m_pPlayer->getUnit(iUnitID);
			if (pEscort->getDomainType() != DOMAIN_LAND)
			{
				pEscort = NULL;   // Second unit wasn't the escort
			}
		}
	}

	if (!pCivilian && pOperation->IsCivilianRequired())
	{
		return;
	}

	m_OperationUnits.clear();
	pThisArmy->UpdateCheckpointTurns();

	// RECRUITING
	if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		// If no estimate for when recruiting will end, let the rest of the AI use these units
		if (pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					// See if we are just able to get to muster point in time.  If so, time for us to head over there
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit && !pUnit->TurnProcessed())
					{
						const CvFormationSlotEntry& thisSlotEntry = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex())->getFormationSlotEntry(iI);

						// Continue moving to target
						if (pSlot->HasStartedOnOperation())
						{
							MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
						}

						else
						{
							// See if we are just able to get to muster point in time.  If so, time for us to head over there
							int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true /*bReusePaths*/, true, true);
							if (iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
							{
								pSlot->SetStartedOnOperation(true);
								MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
							}
						}
					}
				}
			}
			ExecuteNavalFormationMoves(pThisArmy, pOperation->GetMusterPlot());
		}
	}

	// GATHERING FORCES
	else if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		ExecuteFleetMoveToTarget (pThisArmy, pOperation->GetMusterPlot());
	}

	// MOVING TO TARGET with a civilian
	else if (pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION && pOperation->IsCivilianRequired())
	{
		// Update army's current location
		CvPlot *pThisTurnTarget;
		pThisTurnTarget = pThisArmy->GetCenterOfMass();
		if (pThisTurnTarget != NULL)
		{
			pThisArmy->SetXY(pThisTurnTarget->getX(), pThisTurnTarget->getY());
		}

		// Settler at the target?
		if (!pCivilian->plot()->isWater() && pCivilian->plot() == pThisArmy->GetGoalPlot())
		{
			if (pCivilian->canFound(pCivilian->plot()))
			{
				pCivilian->PushMission(CvTypes::getMISSION_FOUND());
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("City founded, At X=%d, At Y=%d", pCivilian->plot()->getX(), pCivilian->plot()->getY());
					pOperation->LogOperationSpecialMessage(strMsg);
				}
				pOperation->SetToAbort(AI_ABORT_SUCCESS);
			}
		}

		// Is our settler and his escort both one move from the destination?  If so, targeted moves to finish up
		else if (TurnsToReachTarget(pCivilian, pOperation->GetTargetPlot()) <= 1 &&
			     (!pEscort || TurnsToReachTarget(pEscort, pOperation->GetTargetPlot()) <= 1))
		{
			ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(), true);
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strTemp;
				CvString strLogString;
				strTemp = GC.getUnitInfo(pCivilian->getUnitType())->GetDescription();
				strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
				LogTacticalMessage(strLogString);
			}

			if (pEscort)
			{
				ExecuteMoveToPlot(pEscort, pOperation->GetTargetPlot(), false);
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp;
					CvString strLogString;
					strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
					strLogString.Format("Moving %s to target, X: %d, Y: %d", strTemp.GetCString(), pEscort->getX(), pEscort->getY());
					LogTacticalMessage(strLogString);
				}
			}

			// Move naval escorts in close
			for (int iI = 2; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit && !pUnit->TurnProcessed())
					{
						MoveToEmptySpaceNearTarget(pUnit, pOperation->GetTargetPlot(), false /*bLand */);
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strTemp;
							CvString strLogString;
							strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
							strLogString.Format("Moving %s near target, X: %d, Y: %d", strTemp.GetCString(), pCivilian->getX(), pCivilian->getY());
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
			CvPlot *pBestPlot = NULL;

			// Request moves for all units
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit && !pUnit->TurnProcessed())
					{
						const CvFormationSlotEntry& thisSlotEntry = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex())->getFormationSlotEntry(iI);
						MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
					}
				}
			}

			// Plot one turn of movement for the first unit in the army we find out in the water
			UnitHandle pUnitAtSea = pThisArmy->GetFirstUnitInDomain(DOMAIN_SEA);
			if (!pUnitAtSea)
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
				int iFlags;
				if (pThisArmy->GetUnitsOfType((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_NAVAL_ESCORT")) > 0)
				{
					iFlags = MOVE_UNITS_IGNORE_DANGER;
				}

				// Goal should be a water tile one hex shy of our target
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot *pAdjacentPlot = plotDirection(pThisArmy->GetGoalX(), pThisArmy->GetGoalY(), ((DirectionTypes)iI));
					if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
					{
						if (pUnitAtSea->plot() == pAdjacentPlot)
						{
							pBestPlot = pAdjacentPlot;
							break;
						}
						else
						{
							// Using step finder could get tripped up by ocean hexes (since they are in the area but not valid movement targets for coastal vessels.  Watch this!
							int iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_pPlayer->GetID(), pOperation->GetEnemy(), pUnitAtSea->plot(), pAdjacentPlot);
							if (iDistance > 0 && iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								int iDistanceToMove = min (4, iDistance);
								PlayerTypes eEnemy = pOperation->GetEnemy();
								pBestPlot = GC.getStepFinder().GetXPlotsFromEnd(m_pPlayer->GetID(), eEnemy, pUnitAtSea->plot(), pAdjacentPlot, (iDistance - iDistanceToMove));
							}
						}
					}
				}

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
					ExecuteNavalFormationMoves(pThisArmy, pBestPlot);
				}
			}
		}
	}

	// MOVING TO TARGET as an army
	else if (pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION && !pOperation->IsCivilianRequired())
	{
		int iBestDistance = MAX_INT;
		CvPlot *pBestPlot = NULL;

		// Request moves for all units
		for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
			if (pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if (pUnit && !pUnit->TurnProcessed())
				{
					const CvFormationSlotEntry& thisSlotEntry = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex())->getFormationSlotEntry(iI);
					MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
				}
			}
		}

		// Plot one turn of movement for the first unit in the army we find out in the water
		UnitHandle pUnitAtSea = pThisArmy->GetFirstUnitInDomain(DOMAIN_SEA);
		if (!pUnitAtSea)
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
			int iFlags;
			if (pThisArmy->GetUnitsOfType((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_NAVAL_ESCORT")) > 0)
			{
				iFlags = MOVE_UNITS_IGNORE_DANGER;
			}

			// Goal should be a water tile one hex shy of our target
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pAdjacentPlot = plotDirection(pThisArmy->GetGoalX(), pThisArmy->GetGoalY(), ((DirectionTypes)iI));
				if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
				{
					if (pUnitAtSea->plot() == pAdjacentPlot)
					{
						pBestPlot = pAdjacentPlot;
						break;
					}
					else
					{
						// Using step finder could get tripped up by ocean hexes (since they are in the area but not valid movement targets for coastal vessels.  Watch this!
						int iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_pPlayer->GetID(), pOperation->GetEnemy(), pUnitAtSea->plot(), pAdjacentPlot);
						if (iDistance > 0 && iDistance < iBestDistance)
						{
							iBestDistance = iDistance;
							int iDistanceToMove = min (4, iDistance);
							PlayerTypes eEnemy = pOperation->GetEnemy();
							pBestPlot = GC.getStepFinder().GetXPlotsFromEnd(m_pPlayer->GetID(), eEnemy, pUnitAtSea->plot(), pAdjacentPlot, (iDistance - iDistanceToMove));
						}
					}
				}
			}

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
				// Update army's current location
				pThisArmy->SetXY(pBestPlot->getX(), pBestPlot->getY());

				ExecuteNavalFormationMoves(pThisArmy, pBestPlot);
			}
		}
	}
}

/// Move a naval force that is roaming for targets
void CvTacticalAI::PlotFreeformNavalOperationMoves(CvAINavalOperation *pOperation)
{
	// Simplification - assume only 1 army per operation now
	if (pOperation->GetFirstArmyID() == -1)
	{
		return;
	}
	CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(pOperation->GetFirstArmyID());

	// RECRUITING
	if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		// If no estimate for when recruiting will end, let the rest of the AI use these units
		if (pThisArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return;
		}
		else
		{
			for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
			{
				CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
				if (pSlot->GetUnitID() != NO_UNIT)
				{
					// See if we are just able to get to muster point in time.  If so, time for us to head over there
					UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
					if (pUnit && !pUnit->TurnProcessed())
					{
						CvMultiUnitFormationInfo* pkMultiUnitFormationInfo = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
						if(pkMultiUnitFormationInfo)
						{
							const CvFormationSlotEntry& thisSlotEntry = pkMultiUnitFormationInfo->getFormationSlotEntry(iI);

							// Continue moving to target
							if (pSlot->HasStartedOnOperation())
							{
								MoveWithFormation(pUnit, thisSlotEntry.m_ePositionType);
							}
							else
							{
								// See if we are just able to get to muster point in time.  If so, time for us to head over there
								int iTurns = TurnsToReachTarget(pUnit, pOperation->GetMusterPlot(), true /*bReusePaths*/, true, true);
								if (iTurns + GC.getGame().getGameTurn() >= pThisArmy->GetTurnAtNextCheckpoint())
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
	else if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		ExecuteFleetMoveToTarget (pThisArmy, pOperation->GetMusterPlot());
	}

	// MOVING TO TARGET
	else if (pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		// Check danger on our ships; move to safety if necessary
		m_CurrentMoveUnits.clear();
		for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
			if (pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if (pUnit && !pUnit->TurnProcessed())
				{
					CvPlot *pPlot = pUnit->plot();
					int iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
					if (iDangerLevel > pUnit->GetPower())
					{
						CvTacticalUnit unit;
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);
					}
				}
			}
		}
		if (m_CurrentMoveUnits.size() > 0)
		{
			ExecuteMovesToSafestPlot();
		}

		if (!PlotEscortNavalOperationMoves(pThisArmy))
		{
			PlotCloseOnEnemyNavalUnitMoves(pThisArmy);
		}
	}
}

/// Queues up attacks on enemy units on or adjacent to army's desired center
void CvTacticalAI::ClearEnemiesNearArmy(CvArmyAI *pArmy)
{
	bool bEnemyNear = false;
	CvPlot *pPlot;
	int iRange = 1;
	bool bAttackUnderway;
	bool bAttackMade = false;
	UnitHandle pUnit;

	// Loop through all appropriate targets to see if any is of concern
	for (unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			// Is this unit near enough?
			if (plotDistance(pArmy->GetX(), pArmy->GetY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY()) <= iRange)
			{
				bEnemyNear = true;
				break;
			}
		}
	}

	if (bEnemyNear)
	{
		// Add units from army to tactical AI for this turn
		pUnit = pArmy->GetFirstUnit();
		while (pUnit)
		{
			if (!pUnit->TurnProcessed() && !pUnit->isDelayedDeath() && pUnit->canMove())
			{
				if (std::find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), pUnit->GetID()) == m_CurrentTurnUnits.end())
				{
					m_CurrentTurnUnits.push_back(pUnit->GetID());
				}
			}
			pUnit = pArmy->GetNextUnit();
		}

		// Now attack these targets
		for (unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
		{
			// Is the target of an appropriate type?
			if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
				m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
				m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if (m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
				{
					// Is this unit near enough?
					if (plotDistance(pArmy->GetX(), pArmy->GetY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY()) <= iRange)
					{
						pPlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());

						m_pMap->ClearDynamicFlags();
						m_pMap->SetTargetBombardCells(pPlot, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

						bAttackUnderway = ExecuteSafeBombards(m_AllTargets[iI]);
						if (m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
						{
							bAttackMade = ExecuteProtectedBombards(m_AllTargets[iI], bAttackUnderway);
						}
						if (bAttackMade)
						{
							bAttackUnderway = true;
						}
						if (m_AllTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
						{
							UnitHandle pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
							if (pDefender)
							{
								m_AllTargets[iI].SetAuxIntData(pDefender->GetCurrHitPoints());
								m_CurrentMoveCities.clear();
								if (FindUnitsWithinStrikingDistance(pPlot, 1, false /* bNoRangedUnits */))
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
	if (pUnit->getMoves() > 0)
	{
		CvOperationUnit operationUnit;
		operationUnit.SetUnitID(pUnit->GetID());
		operationUnit.SetPosition(ePosition);
		m_OperationUnits.push_back(operationUnit);
	}
}

/// Complete moves for all units requested through calls to MoveWithFormation()
void CvTacticalAI::ExecuteFormationMoves(CvArmyAI *pArmy)
{
	CvPlot *pTarget;
	UnitHandle pUnit;
	int iMeleeUnits;
	int iRangedUnits;
	AITacticalTargetType eTargetType;
	CvPlot *pLoopPlot;

	if (m_OperationUnits.size() == 0)
	{
		return;
	}

	pTarget = pArmy->Plot();
	iMeleeUnits = pArmy->GetUnitsOfType((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_FRONT_LINE"));
	iRangedUnits = pArmy->GetUnitsOfType((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_BOMBARD"));

	// Range around target based on number of units we need to place
	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iMeleeUnits + iRangedUnits);

	// See if we have enough places to put everyone
	if (!ScoreDeploymentPlots(pTarget, pArmy, iMeleeUnits, iRangedUnits, iRange) &&
		!ScoreDeploymentPlots(pTarget, pArmy, iMeleeUnits, iRangedUnits, 3))
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy near target", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
	}

	// Compute the moves to get the best deployment
	else
	{
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();
		bool bDone = false;

		int iRangedUnitsToPlace = iRangedUnits;
		int iMeleeUnitsToPlace = iMeleeUnits;

		for (unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			// DECREMENT COUNTERS OF UNITS NEEDED
			// High priority = target for ranged unit
			bool bSkipThisOne = false;
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				if (iRangedUnitsToPlace > 0)
				{
					iRangedUnitsToPlace--;
				}
				else
				{
					iMeleeUnitsToPlace--;
				}
			}
			else
			{
				if (iMeleeUnitsToPlace > 0)
				{
					iMeleeUnitsToPlace--;
				}
				else
				{
					bSkipThisOne = true;
				}
			}
			if (!bSkipThisOne)
			{
				pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if (FindClosestOperationUnit(pLoopPlot, (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT), false /*bIncludeGenerals*/))
				{
					for (unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
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
			if (iMeleeUnitsToPlace == 0 && iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}

		// Now ready to make the assignments
		AssignDeployingUnits(iMeleeUnits + iRangedUnits);

		PerformChosenMoves();

		// Log if someone in army didn't get a move assigned (how do we address this in the future?)
		if (m_ChosenBlocks.size() < (unsigned int)(iMeleeUnits + iRangedUnits))
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format ("No army deployment move for %d units", iMeleeUnits + iRangedUnits - m_ChosenBlocks.size());
				LogTacticalMessage(strMsg);
			}
		}
	}
}

/// Pick best hexes for deploying our army (based on safety, terrain, and keeping a tight formation). Returns false if insufficient free plots.
bool CvTacticalAI::ScoreDeploymentPlots(CvPlot *pTarget, CvArmyAI *pArmy, int iNumMeleeUnits, int iNumRangedUnits, int iRange)
{
	int iDX, iDY;
	int iScore;
	CvPlot *pPlot;
	CvTacticalAnalysisCell *pCell;
	bool bSafeForDeployment;
	int iNumSafePlotsFound = 0;
	int iNumDeployPlotsFound = 0;
	CvTacticalTarget target;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pPlot = plotXY(pTarget->getX(), pTarget->getY(), iDX, iDY);
			if (pPlot != NULL)
			{
				bSafeForDeployment = true;

				int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
				if (iPlotDistance <= iRange)
				{
					int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					CvAIOperation *pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());

					bool bValid = false;
					if (pOperation->IsMixedLandNavalOperation() && pCell->CanUseForOperationGathering())
					{
						bValid = true;
					}

					else if (pOperation->IsAllNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true /*bWater*/))
					{
						bValid = true;
					}

					else if ((!pOperation->IsAllNavalOperation() && !pOperation->IsMixedLandNavalOperation()) && pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/))
					{
						bValid = true;
					}

					if (pOperation->IsMixedLandNavalOperation() || pOperation->IsAllNavalOperation())
					{
						if (!pArmy->IsAllOceanGoing() && pCell->IsOcean())
						{
							bValid = false;
						}
					}

					if (bValid)
					{
						// Skip this plot if friendly unit that isn't in this army
						CvUnit *pFriendlyUnit;
						pFriendlyUnit = pCell->GetFriendlyMilitaryUnit();
						if (pFriendlyUnit != NULL)
						{
							if (pFriendlyUnit->getArmyID() != pArmy->GetID())
							{
								continue;
							}
						}

						iNumDeployPlotsFound++;
						iScore = 600 - (iPlotDistance * 100);
						if (pCell->IsSubjectToAttack())
						{
							iScore -= 100;
							bSafeForDeployment = false;
						}
						else
						{
							iNumSafePlotsFound++;
						}
						if (pCell->IsEnemyCanMovePast())
						{
							iScore -= 100;
						}
						if (pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
						{
							iScore += 100;
						}
						else
						{
							iScore += pCell->GetDefenseModifier() * 2;
						}

						pCell->SetSafeForDeployment(bSafeForDeployment);
						pCell->SetDeploymentScore(iScore);

						// Save this in our list of potential targets
						target.SetTargetX(pPlot->getX());
						target.SetTargetY(pPlot->getY());
						target.SetAuxIntData(iScore);

						// A bit of a hack -- use high priority targets to indicate safe plots for ranged units
						if (bSafeForDeployment)
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
	if (iNumSafePlotsFound < iNumRangedUnits || iNumDeployPlotsFound < (iNumMeleeUnits + iNumRangedUnits))
	{
		return false;
	}

	return true;
}

/// Complete moves for all units requested through calls to MoveWithFormation()
void CvTacticalAI::ExecuteNavalFormationMoves(CvArmyAI *pArmy, CvPlot *pTurnTarget)
{
	UnitHandle pUnit;
	int iNavalUnits;
	int iEscortedUnits;
	AITacticalTargetType eTargetType;
	CvPlot *pLoopPlot;

	if (m_OperationUnits.size() == 0)
	{
		return;
	}

	iNavalUnits = pArmy->GetUnitsOfType((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_NAVAL_ESCORT"));
	iEscortedUnits = m_OperationUnits.size() - iNavalUnits;

	// Range around turn target based on number of units we need to place
	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iNavalUnits + iEscortedUnits);

	// See if we have enough places to put everyone
	if (!ScoreDeploymentPlots(pTurnTarget, pArmy, iNavalUnits, iEscortedUnits, iRange) &&
		!ScoreDeploymentPlots(pTurnTarget, pArmy, iNavalUnits, iEscortedUnits, 3))
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy near turn target", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
	}

	// Compute the moves to get the best deployment
	else
	{
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();
		bool bDone = false;

		int iEscortedUnitsToPlace = iEscortedUnits;
		int iNavalUnitsToPlace = iNavalUnits;

		for (unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			eTargetType = m_TempTargets[iI].GetTargetType();

			// DECREMENT COUNTERS OF UNITS NEEDED
			// High priority = target for escorted unit
			bool bSkipThisOne = false;
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				if (iEscortedUnitsToPlace > 0)
				{
					iEscortedUnitsToPlace--;
				}
				else
				{
					iNavalUnitsToPlace--;
				}
			}
			else
			{
				if (iNavalUnitsToPlace > 0)
				{
					iNavalUnitsToPlace--;
				}
				else
				{
					bSkipThisOne = true;
				}
			}
			if (!bSkipThisOne)
			{
				pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if (FindClosestOperationUnit(pLoopPlot, (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT), true /*bIncludeGenerals*/))
				{
					for (unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
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
			if (iNavalUnitsToPlace == 0 && iEscortedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}

		// Now ready to make the assignments
		AssignDeployingUnits(iNavalUnits + iEscortedUnits);

		PerformChosenMoves();

		// Log if someone in army didn't get a move assigned (how do we address this in the future?)
		if (m_ChosenBlocks.size() < (unsigned int)(iNavalUnits + iEscortedUnits))
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format ("No naval deployment move for %d units", iNavalUnits + iEscortedUnits - m_ChosenBlocks.size());
				LogTacticalMessage(strMsg);
			}
		}
	}
}

/// Help protect a nearby naval operation (returns true if found one to support)
bool CvTacticalAI::PlotEscortNavalOperationMoves(CvArmyAI *pArmy)
{
	CvPlot *pTarget = NULL;
	int iOperationID;
	int iArmyID;
	CvArmyAI *pArmyToEscort;

	// Only one colonization operation at a time, so don't have to search for more than one
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_COLONIZE, &iOperationID))
	{
		CvAIOperation *pOperation = m_pPlayer->getAIOperation(iOperationID);
		iArmyID = pOperation->GetFirstArmyID();
		if (iArmyID != -1)
		{
			pArmyToEscort = m_pPlayer->getArmyAI(iArmyID);
			if (pArmyToEscort)
			{
				pTarget = pArmyToEscort->GetCenterOfMass();
			}
		}
	}

	if (pTarget)
	{
		ExecuteFleetMoveToTarget (pArmy, pTarget);
		return true;
	}

	return false;
}

/// Close on nearest enemy unit to our army's center of mass
void CvTacticalAI::PlotCloseOnEnemyNavalUnitMoves(CvArmyAI *pArmy)
{
	CvPlot *pBestTarget = NULL;
	int iBestTurns = MAX_INT;
	int iTurns;
	CvTacticalTarget target;
	UnitHandle pInitialUnit;

	pInitialUnit = pArmy->GetFirstUnit();
	if (pInitialUnit)
	{
		// Loop through all unit targets looking for the closest enemy naval unit
		for (unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
		{
			target = m_AllTargets[iI];

			if (target.GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
				target.GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
				target.GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				if (target.IsTargetStillAlive(m_pPlayer->GetID()))
				{
					CvPlot *pPlot = GC.getMap().plot(target.GetTargetX(), target.GetTargetY());
					if (pPlot->isWater())
					{
						iTurns = TurnsToReachTarget(pInitialUnit, pPlot, true /*bReusePaths*/, true, true);
						if (iTurns < iBestTurns)
						{
							iBestTurns = iTurns;
							pBestTarget = pPlot;
						}
					}
				}
			}
		}
	}

	if (pBestTarget)
	{
		ExecuteFleetMoveToTarget (pArmy, pBestTarget);
	}
}

/// Move a squadron of naval units to a target
void CvTacticalAI::ExecuteFleetMoveToTarget(CvArmyAI *pArmy, CvPlot *pTarget)
{
	// Request moves for all units
	for (int iI = 0; iI < pArmy->GetNumFormationEntries(); iI++)
	{
		CvArmyFormationSlot *pSlot = pArmy->GetFormationSlot(iI);
		if (pSlot->GetUnitID() != NO_UNIT)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
			if (pUnit && !pUnit->TurnProcessed())
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
	CvCity *pLoopCity;
	int iCityLoop;
	FFastVector<CvTacticalTarget> possibleAttackers;
	int iExpectedDamage;
	int iExpectedTotalDamage;

	// Loop through each of our cities
	for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		// Compile a list of units that can attack it this turn and what their expected damage is
		possibleAttackers.clear();
		iExpectedTotalDamage = 0;

		CvTacticalTarget *pTarget;
		pTarget = GetFirstUnitTarget();
		while (pTarget != NULL)
		{
			CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if (pEnemyUnit)
			{
				iExpectedDamage = 0;

				if (pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) > pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL))
				{
					if (plotDistance(pEnemyUnit->getX(), pEnemyUnit->getY(), pLoopCity->getX(), pLoopCity->getY()) <= pEnemyUnit->GetRange())
					{
						if (pEnemyUnit->canEverRangeStrikeAt(pLoopCity->getX(), pLoopCity->getY()))
						{
							iExpectedDamage = pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false);
						}
					}
				}
				else if (CanReachInXTurns(pEnemyUnit, pLoopCity->plot(), 1))
				{
					int iAttackerStrength = pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL);
					int iDefenderStrength = pLoopCity->getStrengthValue();
					CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pLoopCity->getOwner(), pLoopCity->getX(), pLoopCity->getY(), pEnemyUnit->getX(), pEnemyUnit->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if (pFireSupportUnit != NULL)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pEnemyUnit.pointer(), NULL, false);
					}
					iExpectedDamage = pEnemyUnit->getCombatDamage(iAttackerStrength, iDefenderStrength, pEnemyUnit->getDamage() + iDefenderFireSupportCombatDamage, /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ true);
				}
				if (iExpectedDamage > 0)
				{
					iExpectedTotalDamage += iExpectedDamage;
					possibleAttackers.push_back(*pTarget);
				}
			}
			pTarget = GetNextUnitTarget();
		}

		// If they can take the city down and they are a melee unit, then they are a high priority target
		if (iExpectedTotalDamage > (GC.getMAX_CITY_HIT_POINTS() - pLoopCity->getDamage()))
		{
			unsigned int iAttackerIndex = 0;

			// Loop until we've found all the attackers in the unit target list
			pTarget = GetFirstUnitTarget();
			while (iAttackerIndex < possibleAttackers.size() && pTarget != NULL)
			{
				// Match based on X, Y
				if (pTarget->GetTargetX() == possibleAttackers[iAttackerIndex].GetTargetX() &&
					pTarget->GetTargetY() == possibleAttackers[iAttackerIndex].GetTargetY())
				{
					CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
					if (pEnemyUnit && pEnemyUnit->IsCanAttackRanged())
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
		else if (possibleAttackers.size() > 0)
		{
			unsigned int iAttackerIndex = 0;

			// Loop until we've found all the attackers in the unit target list
			pTarget = GetFirstUnitTarget();
			while (iAttackerIndex < possibleAttackers.size() && pTarget != NULL)
			{
				// Match based on X, Y
				if (pTarget->GetTargetX() == possibleAttackers[iAttackerIndex].GetTargetX() &&
					pTarget->GetTargetY() == possibleAttackers[iAttackerIndex].GetTargetY())
				{
					if (pTarget->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
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
	CvPlot *pLoopPlot;
	CvTacticalTarget *pTarget;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		if (pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			pTarget = GetFirstUnitTarget();
			while (pTarget != NULL)
			{
				bool bPriorityTarget = false;

				// Skip if already a priority target (because was able to strike another camp)
				if (pTarget->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
				{
					CvPlot *pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					UnitHandle pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
					if (pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) > pEnemyUnit->GetMaxAttackStrength(NULL, pLoopPlot, NULL))
					{
						if (plotDistance(pEnemyUnit->getX(), pEnemyUnit->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= pEnemyUnit->GetRange())
						{
							if (pEnemyUnit->canEverRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
							{
								bPriorityTarget = true;
							}
						}
					}
					else if (CanReachInXTurns(pEnemyUnit, pLoopPlot, 1))
					{
						bPriorityTarget = true;
					}
					if (bPriorityTarget)
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
	for (unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Don't consider units that are already medium priority
		if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			// Ranged units will always be medium priority targets
			CvUnit *pUnit = (CvUnit *)m_AllTargets[iI].GetAuxData();
			if (pUnit->IsCanAttackRanged())
			{
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
			}
		}

		// Don't consider units that are already high priority
		if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			// Units defending citadels will always be high priority targets
			CvUnit *pUnit = (CvUnit *)m_AllTargets[iI].GetAuxData();
			ImprovementTypes eImprovement = pUnit->plot()->getImprovementType();
			if (pUnit->plot()->getOwner() == pUnit->getOwner() &&
				eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
			{
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			}
		}
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
	for (it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;

		// Is it adjacent to a point in the main list?
		for (it2 = m_NavalResourceBlockadePoints.begin(); it2 != m_NavalResourceBlockadePoints.end(); ++it2)
		{
			if (plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) <= 2)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if (!bFoundAdjacent)
		{
			m_NavalResourceBlockadePoints.push_back(*it);
		}
	}

	// Now copy all points into main target list
	for (it = m_NavalResourceBlockadePoints.begin(); it != m_NavalResourceBlockadePoints.end(); ++it)
	{
		m_AllTargets.push_back(*it);
	}
}

/// Sift through the target list and find just those that apply to the dominance zone we are currently looking at
void CvTacticalAI::ExtractTargetsForZone(CvTacticalDominanceZone *pZone /* Pass in NULL for all zones */)
{
	TacticalList::iterator it;

	m_ZoneTargets.clear();
	for (it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
	{
		bool bValid = false;

		if (pZone == NULL)
		{
			bValid = true;
		}
		else
		{
			DomainTypes eDomain = DOMAIN_LAND;
			if (pZone->IsWater())
			{
				eDomain = DOMAIN_SEA;
			}
			bValid = it->IsTargetValidInThisDomain(eDomain);
		}
		if (bValid)
		{
			if (pZone == NULL || it->GetDominanceZone() == pZone->GetDominanceZoneID())
			{
				m_ZoneTargets.push_back(*it);
			}

			// Not obviously in this zone, but if within 2 of city we want them anyway
			else
			{
				CvCity *pCity;
				pCity = pZone->GetClosestCity();
				if (pCity)
				{
					if (plotDistance(pCity->getX(), pCity->getY(), it->GetTargetX(), it->GetTargetY()) <= 2)
					{
						m_ZoneTargets.push_back(*it);
					}
				}
			}
		}
	}
}

/// Find the first target of a requested type in current dominance zone (call after ExtractTargetsForZone())
CvTacticalTarget *CvTacticalAI::GetFirstZoneTarget(AITacticalTargetType eType)
{
	m_eCurrentTargetType = eType;
	m_iCurrentTargetIndex = 0;

	while (m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if (m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
		{
			return &m_ZoneTargets[m_iCurrentTargetIndex];
		}
		m_iCurrentTargetIndex++;
	}

	return NULL;
}

/// Find the next target of a requested type in current dominance zone (call after GetFirstZoneTarget())
CvTacticalTarget *CvTacticalAI::GetNextZoneTarget()
{
	m_iCurrentTargetIndex++;

	while (m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if (m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
		{
			return &m_ZoneTargets[m_iCurrentTargetIndex];
		}
		m_iCurrentTargetIndex++;
	}

	return NULL;
}

/// Find the first unit target (in full list of targets -- NOT by zone)
CvTacticalTarget *CvTacticalAI::GetFirstUnitTarget()
{
	m_iCurrentUnitTargetIndex = 0;

	while (m_iCurrentUnitTargetIndex < (int)m_AllTargets.size())
	{
		if (m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
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
CvTacticalTarget *CvTacticalAI::GetNextUnitTarget()
{
	m_iCurrentUnitTargetIndex++;

	while (m_iCurrentUnitTargetIndex < (int)m_AllTargets.size())
	{
		if (m_AllTargets[m_iCurrentUnitTargetIndex].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
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
void CvTacticalAI::ExecuteBarbarianCampMove(CvPlot *pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if (pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}

/// Capture an undefended civilian
void CvTacticalAI::ExecuteCivilianCapture(CvPlot *pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if (pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());

		pUnit->SetTacticalAIPlot(NULL);
	}
}

/// Pillage an undefended improvement
void CvTacticalAI::ExecutePillage(CvPlot *pTargetPlot)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if (pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
		pUnit->finishMoves();

		// Delete this unit from those we have to move
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}

/// Attack a defended space
void CvTacticalAI::ExecuteAttack(CvTacticalTarget *pTarget, CvPlot *pTargetPlot, bool bInflictWhatWeTake, bool bMustSurviveAttack)
{
	void *pFirstAttacker = NULL;
	bool bFirstAttackRanged = false;
	bool bFirstAttackCity = false;

	if (PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return;
	}

	// How much damage do we still need to inflict?
	int iDamageRemaining = (pTarget->GetAuxIntData() * (100 + GC.getAI_TACTICAL_OVERKILL_PERCENT())) / 100;

	// Start by applying damage from city bombards
	for (unsigned int iI = 0; iI < m_CurrentMoveCities.size() && iDamageRemaining > 0; iI++)
	{
		CvCity *pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		if (pCity != NULL)
		{
			if (QueueAttack(pCity, pTarget, true /*bRanged*/, true /*bCity*/))
			{
				pFirstAttacker = (void *)pCity;
				bFirstAttackRanged = true;
				bFirstAttackCity = true;
			}

			// Subtract off expected damage
			iDamageRemaining -= m_CurrentMoveCities[iI].GetExpectedTargetDamage();
		}
	}

	// First loop is ranged units only
	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size() && iDamageRemaining > 0; iI++)
	{
		if (!bInflictWhatWeTake || m_CurrentMoveUnits[iI].GetExpectedTargetDamage() >= m_CurrentMoveUnits[iI].GetExpectedSelfDamage())
		{
			UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
			if (pUnit)
			{
				if (pUnit->getMoves() > 0)
				{
					if (!bMustSurviveAttack || ((m_CurrentMoveUnits[iI].GetExpectedSelfDamage() + pUnit->getDamage()) < pUnit->GetMaxHitPoints()))
					{
						// Are we a ranged unit
						if (pUnit->IsCanAttackRanged())
						{
							// Are we in range?
							if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= pUnit->GetRange())
							{
								// Do we have LOS to the target?
								if (pUnit->canEverRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
								{
									// Do we need to set up to make a ranged attack?
									if (pUnit->canSetUpForRangedAttack(NULL))
									{
										pUnit->setSetUpForRangedAttack(true);
										if (GC.getLogging() && GC.getAILogging())
										{
											CvString strTemp;
											CvString strMsg;
											strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
											strMsg.Format ("Set up %s for ranged attack", strTemp.GetCString());
											LogTacticalMessage(strMsg);
										}
										if (!pUnit->canMove())
										{
											pUnit->SetTacticalAIPlot(NULL);
											UnitProcessed(pUnit->GetID());
										}
									}

									// Can we hit it with a ranged attack?  If so, that gets first priority
									if (pUnit->canMove() && pUnit->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
									{
										// Queue up this attack
										if (QueueAttack((void *)pUnit.pointer(), pTarget, true /*bRanged*/, false /*bCity*/))
										{
											pFirstAttacker = (void *)pUnit.pointer();
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
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format ("Not attacking with unit. We'll destroy ourself.");
							LogTacticalMessage(strMsg);
						}
					}
				}
			}
		}
		else
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format ("Not attacking with unit. Can't generate a good damage ratio.");
				LogTacticalMessage(strMsg);
			}
		}
	}

	// If target is city, want to get in one melee attack, so set damage remaining to 1
	if (pTarget->GetTargetType() == AI_TACTICAL_TARGET_CITY && iDamageRemaining < 1)
	{
		iDamageRemaining = 1;
	}

	// Second loop are only melee units
	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size() && iDamageRemaining > 0; iI++)
	{
		if (!bInflictWhatWeTake || m_CurrentMoveUnits[iI].GetExpectedTargetDamage() >= m_CurrentMoveUnits[iI].GetExpectedSelfDamage())
		{
			UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
			if (pUnit)
			{
				if (pUnit->getMoves() > 0 && (!bMustSurviveAttack || ((m_CurrentMoveUnits[iI].GetExpectedSelfDamage() + pUnit->getDamage()) < pUnit->GetMaxHitPoints())))
				{
					// Are we a melee unit
					if (!pUnit->IsCanAttackRanged())
					{
						// Queue up this attack
						if (QueueAttack((void *)pUnit.pointer(), pTarget, false /*bRanged*/, false /*bCity*/))
						{
							pFirstAttacker = (void *)pUnit.pointer();
						}
						pUnit->SetTacticalAIPlot(NULL);
						UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false /*bMarkTacticalMap*/);

						// Subtract off expected damage
						iDamageRemaining -= m_CurrentMoveUnits[iI].GetExpectedTargetDamage();
					}
				}
				else
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Not attacking with unit. We'll destroy ourself.");
						LogTacticalMessage(strMsg);
					}
				}
			}
		}
		else
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format ("Not attacking with unit. Can't generate a good damage ratio.");
				LogTacticalMessage(strMsg);
			}
		}
	}

	// Start up first attack
	if (pFirstAttacker)
	{
		LaunchAttack(pFirstAttacker, pTarget, true /*bFirst*/, bFirstAttackRanged, bFirstAttackCity);
	}
}

/// Execute moving units to a better location
void CvTacticalAI::ExecuteRepositionMoves()
{
	CvPlot* pBestPlot = NULL;
	CvString strTemp;

	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			strTemp = pUnit->getUnitInfo().GetDescription();

			// LAND MOVES
			if (pUnit->getDomainType() == DOMAIN_LAND)
			{
				pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange);

				if (pBestPlot)
				{
					if (MoveToEmptySpaceNearTarget(pUnit, pBestPlot, (pUnit->getDomainType()==DOMAIN_LAND)))
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());
						if (GC.getLogging() && GC.getAILogging())
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

/// Moves units to the hex with the lowest danger
void CvTacticalAI::ExecuteMovesToSafestPlot()
{
	int iDanger;

	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			CvPlot *pBestPlot = NULL;
			int iLowestDanger = MAX_INT;

			int iRange = pUnit->getUnitInfo().GetMoves();
			bool bResultHasZeroDangerMove = false;
			bool bResultInTerritory = false;
			bool bResultInCity = false;
			bool bResultInCover = false;

			// For each plot within movement range of the fleeing unit
			for (int iX = -iRange; iX <= iRange; iX++)
			{
				for (int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot *pPlot = GC.getMap().plot(pUnit->getX() + iX, pUnit->getY() + iY);
					if (pPlot == NULL)
					{
						continue;
					}

					// Can't be a plot with another player's unit in it or another of our unit of same type
					if (pPlot->getNumUnits() > 0)
					{
						IDInfo* pUnitNode = pPlot->headUnitNode();
						if (pUnitNode)
						{
							CvUnit *pFirstUnit = ::getUnit(*pUnitNode);
							if (pFirstUnit)
							{
								if (pFirstUnit->getOwner() != pUnit->getOwner())
								{
									continue;
								}

								else if (pUnit->AreUnitsOfSameType(*pFirstUnit))
								{
									continue;
								}
							}
						}
					}

					if (!CanReachInXTurns(pUnit, pPlot, 1))
					{
						continue;
					}

					//   prefer being in a city with the lowest danger value
					//   prefer being in a plot with no danger value
					//   prefer being under a unit with the lowest danger value
					//   prefer being in your own territory with the lowest danger value
					//   prefer the lowest danger value

					iDanger = m_pPlayer->GetPlotDanger(*pPlot);
					bool bIsZeroDanger = (iDanger <= 0);
					bool bIsInCity = pPlot->isFriendlyCity(*pUnit, false);
					bool bIsInCover = (pPlot->getNumDefenders(m_pPlayer->GetID()) > 0);
					bool bIsInTerritory = (pPlot->getTeam() == m_pPlayer->getTeam());

					bool bUpdateBestValue = false;

					if (bIsInCity)
					{
						if (!bResultInCity || iDanger < iLowestDanger)
						{
							bUpdateBestValue = true;
						}
					}
					else if (bIsZeroDanger)
					{
						if (!bResultInCity)
						{
							if (bResultHasZeroDangerMove)
							{
								if (bIsInTerritory && !bResultInTerritory)
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
					else if (bIsInCover)
					{
						if (!bResultInCity && !bResultHasZeroDangerMove)
						{
							if (!bResultInCover || iDanger < iLowestDanger)
							{
								bUpdateBestValue = true;
							}
						}
					}
					else if (bIsInTerritory)
					{
						if (!bResultInCity && !bResultInCover && !bResultHasZeroDangerMove)
						{
							if (!bResultInTerritory || iDanger < iLowestDanger)
							{
								bUpdateBestValue = true;
							}
						}
					}
					// if we have no good home, head to the lowest danger value
					else if (!bResultInCity && !bResultInCover && !bResultInTerritory && !bResultHasZeroDangerMove)
					{
						if (iDanger < iLowestDanger)
						{
							bUpdateBestValue = true;
						}
					}

					if (bUpdateBestValue)
					{
						pBestPlot = pPlot;
						iLowestDanger = iDanger;

						bResultInTerritory = bIsInTerritory;
						bResultInCity      = bIsInCity;
						bResultInCover     = bIsInCover;
						bResultHasZeroDangerMove = bIsZeroDanger;
					}
				}
			}

			if (pBestPlot != NULL)
			{
				// Move to the lowest danger value found
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_UNITS_IGNORE_DANGER);
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("Moving %s to safety, X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}
}

/// Heal chosen units
void CvTacticalAI::ExecuteHeals()
{
	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			if (pUnit->isFortifyable())
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

	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			if (pUnit->isBarbarian())
			{
				strTemp = pUnit->getUnitInfo().GetDescription();

				// LAND MOVES
				if (pUnit->getDomainType() == DOMAIN_LAND)
				{
					AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Land Move, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

					if (bAggressive)
					{
						pBestPlot = FindBestBarbarianLandMove(pUnit);
					}
					else
					{
						pBestPlot = FindPassiveBarbarianLandMove(pUnit);
					}

					if (pBestPlot && MoveToEmptySpaceNearTarget(pUnit, pBestPlot))
					{
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());
						if (GC.getLogging() && GC.getAILogging())
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
						if (GC.getLogging() && GC.getAILogging())
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
					if (pCurrentDestination == NULL ||
						pCurrentDestination == pUnit->plot())
					{
						pBestPlot = FindBestBarbarianSeaMove(pUnit);
					}

					// Otherwise just keep moving there (assuming a path is available)
					else
					{
						if (TurnsToReachTarget(pUnit, pCurrentDestination, true /*bReusePaths*/) != MAX_INT)
						{
							pBestPlot = pCurrentDestination;
						}
						else
						{
							pBestPlot = FindBestBarbarianSeaMove(pUnit);
						}
					}

					if (pBestPlot != NULL)
					{
						pUnit->SetTacticalAIPlot(pBestPlot);
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
						pUnit->finishMoves();
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());

						if (GC.getLogging() && GC.getAILogging())
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

						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("No target for %s at position, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
			}
		}
	}
}

/// Move Barbarian civilian to a camp (with escort if possible)
void CvTacticalAI::ExecuteBarbarianCivilianEscortMove()
{
	UnitHandle pCivilian;
	UnitHandle pEscort;
	CvPlot *pTarget;
	CvPlot *pCurrent;
	CvPlot *pCivilianMove;
	CvPlot *pEscortMove;
	CvUnit *pLoopUnit;

	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		pCivilian = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pCivilian)
		{
			pTarget = FindNearbyTarget(pCivilian, MAX_INT, AI_TACTICAL_TARGET_BARBARIAN_CAMP, pCivilian.pointer());
			if (pTarget)
			{
				// If we're not there yet, we have work to do
				pCurrent = pCivilian->plot();
				if (pCurrent == pTarget)
				{
					pCivilian->finishMoves();
					UnitProcessed(pCivilian->GetID());
				}
				else
				{
					if (pCurrent->getNumUnits() > 1)
					{
						for (int iJ = 0; iJ < pCurrent->getNumUnits(); iJ++)
						{
							pLoopUnit = pCurrent->getUnitByIndex(iJ);
							if (pLoopUnit->GetID() != pCivilian->GetID() &&
								pLoopUnit->getOwner() == pCivilian->getOwner())
							{
								pEscort = UnitHandle(pLoopUnit);
								break;
							}
						}
					}

					// Look at where we'd move this turn taking units into consideration
					int iFlags = 0;
					if (pEscort)
					{
						iFlags = MOVE_UNITS_IGNORE_DANGER;
					}

					// Handle case of no path found at all for civilian
					if (!pCivilian->GeneratePath(pTarget, iFlags, false /*bReuse*/))
					{
						pCivilian->finishMoves();
						if (pEscort)
						{
							pEscort->finishMoves();
						}
						if (GC.getLogging() && GC.getAILogging())
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
						if (pCivilianMove == pTarget)
						{
							// See which defender is stronger
							UnitHandle pCampDefender = pCivilianMove->getBestDefender(m_pPlayer->GetID());
							if (!pCampDefender || (pEscort && pEscort->GetPower() > pCampDefender->GetPower()))
							{
								if (pEscort && (!pCampDefender || ExecuteMoveOfBlockingUnit(pCampDefender)))
								{
									ExecuteMoveToPlot(pEscort, pCivilianMove);
									ExecuteMoveToPlot(pCivilian, pCivilianMove);
									if (GC.getLogging() && GC.getAILogging())
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
									if (GC.getLogging() && GC.getAILogging())
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
								if (GC.getLogging() && GC.getAILogging())
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
						else if (!pEscort)
						{
							ExecuteMoveToPlot(pCivilian, pCivilianMove);
							if (GC.getLogging() && GC.getAILogging())
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
							if (TurnsToReachTarget(pEscort, pCivilianMove) <= 1)
							{
								ExecuteMoveToPlot(pEscort, pCivilianMove);
								ExecuteMoveToPlot(pCivilian, pCivilianMove);
								if (GC.getLogging() && GC.getAILogging())
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
								if (!pBlockingUnit || pBlockingUnit->getMoves() == 0)
								{
									if (!pEscort->GeneratePath(pTarget, 0, false /*bReuse*/))
									{
										pCivilian->finishMoves();
										pEscort->finishMoves();
										if (GC.getLogging() && GC.getAILogging())
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
										if (TurnsToReachTarget(pCivilian, pEscortMove) <= 1)
										{
											ExecuteMoveToPlot(pEscort, pEscortMove);
											ExecuteMoveToPlot(pCivilian, pEscortMove);
											if (GC.getLogging() && GC.getAILogging())
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
											if (GC.getLogging() && GC.getAILogging())
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
									if (ExecuteMoveOfBlockingUnit(pBlockingUnit))
									{
										ExecuteMoveToPlot(pEscort, pCivilianMove);
										ExecuteMoveToPlot(pCivilian, pCivilianMove);
										if (GC.getLogging() && GC.getAILogging())
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
										if (GC.getLogging() && GC.getAILogging())
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
void CvTacticalAI::ExecuteMoveToPlot (CvPlot *pTarget, bool bSaveMoves)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if (pUnit)
	{
		ExecuteMoveToPlot(pUnit, pTarget, bSaveMoves);
	}
}

/// Move unit to protect a specific tile (retrieve unit from first entry in m_CurrentMoveUnits)
void CvTacticalAI::ExecuteMoveToPlot (UnitHandle pUnit, CvPlot *pTarget, bool bSaveMoves)
{
	// Unit already at target plot?
	if (pTarget == pUnit->plot())
	{
		// Fortify if possible
		if (pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
		}
		else
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			if (!bSaveMoves)
			{
				pUnit->finishMoves();
			}
		}
	}

	else
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), MOVE_UNITS_IGNORE_DANGER);
		if (!bSaveMoves)
		{
			pUnit->finishMoves();
		}
	}

	UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());
}

/// Find an adjacent hex to move a blocking unit to
bool CvTacticalAI::ExecuteMoveOfBlockingUnit(UnitHandle pBlockingUnit)
{
	if (!pBlockingUnit->canMove() || IsInQueuedAttack(pBlockingUnit.pointer()))
	{
		return false;
	}

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot *pPlot = plotDirection(pBlockingUnit->getX(), pBlockingUnit->getY(), ((DirectionTypes)iI));
		if (pPlot != NULL)
		{
			// Has to be somewhere we can move and be empty of other units/enemy cities
			if (!pPlot->getVisibleEnemyDefender(m_pPlayer->GetID()) && !pPlot->isEnemyCity(*pBlockingUnit.pointer()) && pBlockingUnit->GeneratePath(pPlot))
			{
				ExecuteMoveToPlot(pBlockingUnit, pPlot);
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp, strLogString;
					strTemp = pBlockingUnit->getUnitInfo().GetDescription();
					strLogString.Format("Moving blocking %s out of way, X: %d, Y: %d", strTemp.GetCString(), pBlockingUnit->getX(), pBlockingUnit->getY());
					LogTacticalMessage(strLogString);
				}
				return true;
			}
		}
	}
	return false;
}

/// Move unit to protect a specific tile
void CvTacticalAI::ExecuteNavalBlockadeMove (CvPlot *pTarget)
{
	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if (pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
		pUnit->finishMoves();
		UnitProcessed(m_CurrentMoveUnits[0].GetID());
		pUnit->SetTacticalAIPlot(NULL);
	}
}

/// Find one unit to move to target, starting with high priority list
void CvTacticalAI::ExecuteMoveToTarget(CvPlot *pTarget, bool bGarrisonIfPossible)
{
	std::vector<CvTacticalUnit>::iterator it;

	// Start with high priority list
	for (it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		// Don't move high priority unit if regular priority unit is closer
		if (m_CurrentMoveUnits.size() > 0 && m_CurrentMoveUnits.begin()->GetMovesToTarget() < it->GetMovesToTarget())
		{
			break;
		}

		if (pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
			UnitProcessed(it->GetID());
			return;
		}

		else if (bGarrisonIfPossible && pUnit->plot() == pTarget && pUnit->CanGarrison(pTarget))
		{
			pUnit->PushMission(CvTypes::getMISSION_GARRISON());
			pUnit->finishMoves();
			UnitProcessed(it->GetID());
			return;
		}

		else if (pUnit && it->GetMovesToTarget() < MAX_INT)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(it->GetID());
			return;
		}
	}

	// Then regular priority
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		if (pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
			UnitProcessed(it->GetID());
			return;
		}

		else if (pUnit && it->GetMovesToTarget() < MAX_INT)
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
	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			if (pUnit->canAirPatrol(NULL))
			{
				pUnit->PushMission(CvTypes::getMISSION_AIRPATROL());
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			}
			UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false /*bMarkTacticalMap*/);
		}
	}
}

/// Bombard enemy units from plots they can't reach (return true if some attack made)
bool CvTacticalAI::ExecuteSafeBombards(CvTacticalTarget &kTarget)
{
	CvTacticalUnit unit;
	void *pFirstAttacker = NULL;
	UnitHandle pDefender;
	int iRequiredDamage = 0;
	bool bCityCanAttack = false;
	bool bFirstAttackCity = false;

	CvPlot *pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	if (PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return false;
	}

	// Get required damage on unit target
	pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if (pDefender)
	{
		iRequiredDamage = pDefender->GetCurrHitPoints();

		// If this is a unit target we might also be able to hit it with a city
		bCityCanAttack = FindCitiesWithinStrikingDistance(pTargetPlot);
		if (bCityCanAttack)
		{
			ComputeTotalExpectedBombardDamage(pDefender);

			// Start by applying damage from city bombards
			for (unsigned int iI = 0; iI < m_CurrentMoveCities.size() && iRequiredDamage > 0; iI++)
			{
				CvCity *pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
				if (pCity != NULL)
				{
					if (QueueAttack(pCity, &kTarget, true /*bRanged*/, true /*bCity*/))
					{
						pFirstAttacker = (void *)pCity;
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
		CvCity *pCity = pTargetPlot->getPlotCity();
		if (pCity != NULL)
		{
			iRequiredDamage = GC.getMAX_CITY_HIT_POINTS() - pCity->getDamage();

			// Can't eliminate a city with ranged fire, so don't target one if that low on health
			if (iRequiredDamage <= 1)
			{
				return false;
			}
		}
	}

	// Need to keep hitting target?
	if (iRequiredDamage <= 0)
	{
		return false;
	}

	// For each of our ranged units, see if they are already in a plot that can bombard that can't be attacked.
	// If so, bombs away!
	m_CurrentMoveUnits.clear();
	list<int>::iterator it;
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->IsCanAttackRanged() && !pUnit->isOutOfAttacks())
		{
			int iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
			CvTacticalAnalysisCell *pCell = GC.getGame().GetTacticalAnalysisMap()->GetCell(iPlotIndex);
			if (pCell->IsWithinRangeOfTarget() && !pCell->IsSubjectToAttack() && IsExpectedToDamageWithRangedAttack(pUnit, pTargetPlot))
			{
				if (pUnit->canSetUpForRangedAttack(NULL))
				{
					pUnit->setSetUpForRangedAttack(true);
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Set up %s for ranged attack", pUnit->getName().GetCString());
						LogTacticalMessage(strMsg, false);
					}
				}

				if (pUnit->canMove() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Making a safe bombard (no move) with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
							kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}

					if (QueueAttack((void *)pUnit.pointer(), &kTarget, true /*bRanged*/, false /*bCity*/))
					{
						pFirstAttacker = (void *)pUnit.pointer();
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
	for (currMoveIt = m_CurrentMoveUnits.begin(); currMoveIt != m_CurrentMoveUnits.end(); ++currMoveIt)
	{
		UnitProcessed(currMoveIt->GetID());
	}

	// For each plot that we can bombard from that the enemy can't attack, try and move a ranged unit there.
	// If so, make that move and mark that tile as blocked with our unit.  If unit has movement left, queue up an attack
	int iDX, iDY;
	CvPlot *pLoopPlot;
	int iPlotIndex;
	CvTacticalAnalysisCell *pCell;

	for (int iTurnsToReach = 0; iTurnsToReach < 2; iTurnsToReach++)
	{
		int iRange = m_pMap->GetBestFriendlyRange();

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot = plotXY(kTarget.GetTargetX(), kTarget.GetTargetY(), iDX, iDY);
				if (pLoopPlot != NULL)
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
					if (iDistance > 0 && iDistance <= iRange)
					{
						iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
						pCell = m_pMap->GetCell(iPlotIndex);
						if (pCell->IsRevealed() && pCell->CanUseForOperationGathering())
						{
							if (pCell->IsWithinRangeOfTarget() && !pCell->IsSubjectToAttack())
							{
								bool bHaveLOS = pLoopPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
								if (FindClosestUnit(pLoopPlot, iTurnsToReach, true/*bMustBeRangedUnit*/, iDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/, pTargetPlot))
								{
									UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
									if (pUnit)
									{
										// Check for presence of unmovable friendly units
										UnitHandle pBlockingUnit = pLoopPlot->getBestDefender(m_pPlayer->GetID());
										if (!pBlockingUnit || ExecuteMoveOfBlockingUnit(pBlockingUnit))
										{
											pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlot->getX(), pLoopPlot->getY());

											if (GC.getLogging() && GC.getAILogging())
											{
												CvString strMsg;
												strMsg.Format ("Moving closer for safe bombard with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
													kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
												LogTacticalMessage(strMsg, false);
											}

											UnitProcessed(m_CurrentMoveUnits[0].GetID());

											if (pUnit->canSetUpForRangedAttack(NULL))
											{
												pUnit->setSetUpForRangedAttack(true);
												if (GC.getLogging() && GC.getAILogging())
												{
													CvString strMsg;
													strMsg.Format ("Set up %s for ranged attack", pUnit->getName().GetCString());
													LogTacticalMessage(strMsg, false);
												}
											}

											if (pUnit->canMove() && !pUnit->isOutOfAttacks() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
											{
												if (GC.getLogging() && GC.getAILogging())
												{
													CvString strMsg;
													strMsg.Format ("Making a safe bombard (half move) with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
														kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
													LogTacticalMessage(strMsg, false);
												}

												if (QueueAttack((void *)pUnit.pointer(), &kTarget, true /*bRanged*/, false /*bCityAttack*/))
												{
													pFirstAttacker = (void *)pUnit.pointer();
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
	if (pFirstAttacker)
	{
		LaunchAttack(pFirstAttacker, &kTarget, true /*bFirst*/, true /*bFirstAttackRanged*/, bFirstAttackCity);
		return true;
	}
	return false;
}

/// Bombard an enemy target from plots we can protect from enemy attack (return true if some attack made)
bool CvTacticalAI::ExecuteProtectedBombards(CvTacticalTarget &kTarget, bool bAttackUnderway)
{
	bool bAttackMade = true;
	bool bAtLeastOneAttackInitiated = false;

	while (bAttackMade && kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		bAttackMade = ExecuteOneProtectedBombard(kTarget);
		if (bAttackMade)
		{
			bAttackUnderway = true;
			bAtLeastOneAttackInitiated = true;
		}
	}
	return bAtLeastOneAttackInitiated;
}

/// Bombard an enemy target from a single plot we can protect from enemy attack (return true if some attack made)
bool CvTacticalAI::ExecuteOneProtectedBombard(CvTacticalTarget &kTarget)
{
	UnitHandle pFirstAttacker;
	int iDX, iDY;
	CvPlot *pAttackPlot;
	int iPlotIndex;
	CvTacticalAnalysisCell *pCell;
	int iPriority;
	int iNumTurns;
	int iNumUnitsRequiredToCover;

	CvPlot *pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	if (PlotAlreadyTargeted(pTargetPlot) != -1)
	{
		return false;
	}

	CvCity *pCity = pTargetPlot->getPlotCity();
	if (pCity != NULL)
	{
		int iRequiredDamage = GC.getMAX_CITY_HIT_POINTS() - pCity->getDamage();

		// Can't eliminate a city with ranged fire, so don't target one if that low on health
		if (iRequiredDamage <= 1)
		{
			return false;
		}
	}

	int iRange = m_pMap->GetBestFriendlyRange();
	m_TempTargets.clear();

	// Build a list of all plots that have LOS to target where no enemy unit is adjacent
	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pAttackPlot = plotXY(kTarget.GetTargetX(), kTarget.GetTargetY(), iDX, iDY);
			if (pAttackPlot != NULL)
			{
				int iPlotDistance = plotDistance(pAttackPlot->getX(), pAttackPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if (iPlotDistance > 0 && iPlotDistance <= iRange)
				{
					iPlotIndex = GC.getMap().plotNum(pAttackPlot->getX(), pAttackPlot->getY());
					pCell = m_pMap->GetCell(iPlotIndex);
					if (pCell->IsRevealed() && pCell->CanUseForOperationGathering())
					{
						if (pCell->IsWithinRangeOfTarget())
						{
							// Check for adjacent enemy unit
							CvPlot *pLoopPlot;
							bool bNoEnemyAdjacent = true;
							for (int iI = 0; iI < NUM_DIRECTION_TYPES && bNoEnemyAdjacent; iI++)
							{
								pLoopPlot = plotDirection(pAttackPlot->getX(), pAttackPlot->getY(), ((DirectionTypes)iI));
								if (pLoopPlot != NULL)
								{
									iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
									if (m_pMap->GetCell(iPlotIndex)->GetEnemyMilitaryUnit() != NULL)
									{
										bNoEnemyAdjacent = false;
									}
								}
							}

							if (bNoEnemyAdjacent)
							{
								// Do we have a unit that can get off a bombard from here THIS turn
								iNumTurns = -1;
								bool bHaveLOS = pAttackPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
								if (FindClosestUnit(pAttackPlot, 0, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/, pTargetPlot))
								{
									iNumTurns = 0;
								}
								// What about next turn?
								else if (FindClosestUnit(pAttackPlot, 1, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/, pTargetPlot))
								{
									iNumTurns = 1;
								}

								// If found a unit that could get here, see if we can cover the hex from enemy attack
								if (iNumTurns >= 0)
								{
									if (CanCoverFromEnemy(pAttackPlot, iNumUnitsRequiredToCover, m_CurrentMoveUnits[0].GetID()))
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
	if (m_TempTargets.size() == 0)
	{
		return false;
	}

	// Sort potential spots
	std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());

	// Have to rebuild blocking position info for this specific spot
	pAttackPlot = GC.getMap().plot(m_TempTargets[0].GetTargetX(), m_TempTargets[0].GetTargetY());
	int iPlotDistance = plotDistance(pAttackPlot->getX(), pAttackPlot->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
	bool bHaveLOS = pAttackPlot->canSeePlot(pTargetPlot, m_pPlayer->getTeam(), iRange, NO_DIRECTION);
	if (FindClosestUnit(pAttackPlot, 0, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/, pTargetPlot) ||
		FindClosestUnit(pAttackPlot, 1, true /*bMustBeRangedUnit*/, iPlotDistance, !bHaveLOS, false/*bMustBeMeleeUnit*/, pTargetPlot))
	{
		int iAttackingUnitID = m_CurrentMoveUnits[0].GetID();
		if (pAttackPlot && CanCoverFromEnemy(pAttackPlot, iNumUnitsRequiredToCover, iAttackingUnitID))
		{
			// Make each blocking move
			for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
			{
				CvUnit *pChosenUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
				if (pChosenUnit)
				{
					pChosenUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					pChosenUnit->finishMoves();
					UnitProcessed(pChosenUnit->GetID());
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Covering a protected bombard with %s, X: %d, Y: %d", pChosenUnit->getName().GetCString(),
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
						LogTacticalMessage(strMsg, false);
					}
				}
			}

			// Then move the attacking unit
			CvUnit *pUnit = m_pPlayer->getUnit(iAttackingUnitID);
			if (pUnit)
			{
				if (pAttackPlot != pUnit->plot())
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pAttackPlot->getX(), pAttackPlot->getY());

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Moving closer for protected bombard with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
							pAttackPlot->getX(), pAttackPlot->getY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}
				}

				UnitProcessed(iAttackingUnitID);

				if (pUnit->canSetUpForRangedAttack(NULL))
				{
					pUnit->setSetUpForRangedAttack(true);
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Set up %s for ranged attack", pUnit->getName().GetCString());
						LogTacticalMessage(strMsg, false);
					}
				}

				if (pUnit->canMove() && !pUnit->isOutOfAttacks() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Making a protected bombard with %s, Target X: %d, Target Y: %d, At X: %d, At Y: %d", pUnit->getName().GetCString(),
							kTarget.GetTargetX(), kTarget.GetTargetY(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}

					if (QueueAttack((void *)pUnit, &kTarget, true /*bRanged*/, false /*bCityAttack*/))
					{
						pFirstAttacker = pUnit;
					}
				}
			}
		}

		if (pFirstAttacker)
		{
			LaunchAttack((void *)pFirstAttacker.pointer(), &kTarget, true /*bFirst*/, true /*bFirstAttackRanged*/, false /*bCityAttack*/);
			return true;
		}
	}

	return false;
}

/// Take a multi-hex attack on an enemy unit this turn
bool CvTacticalAI::ExecuteFlankAttack(CvTacticalTarget &kTarget)
{
	CvTacticalAnalysisCell *pCell;
	CvPlot *pLoopPlot;
	int iPlotIndex;
	int iPossibleFlankHexes = 0;
	CvTacticalTarget target;

	// Count number of possible flank attack spaces around target
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(kTarget.GetTargetX(), kTarget.GetTargetY(), ((DirectionTypes)iI));
		if (pLoopPlot != NULL)
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);
			if (pCell->IsHelpsProvidesFlankBonus())
			{
				iPossibleFlankHexes++;
			}
		}
	}

	// If more than 1, find how many we can fill with units
	if (iPossibleFlankHexes > 1)
	{
		int iFillableHexes = 0;
		int iNumAttackers = 0;
		m_TempTargets.clear();
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(kTarget.GetTargetX(), kTarget.GetTargetY(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL)
			{
				iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
				pCell = m_pMap->GetCell(iPlotIndex);
				if (pCell->IsHelpsProvidesFlankBonus())
				{
					if (FindClosestUnit(pLoopPlot, 0, false /*bMustBeRangedUnit*/, 0 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
					{
						iFillableHexes++;
						iNumAttackers++;
						target.SetTargetX(pLoopPlot->getX());
						target.SetTargetY(pLoopPlot->getY());
						m_TempTargets.push_back(target);
					}
					// What about next turn?
					else if (FindClosestUnit(pLoopPlot, 1, false /*bMustBeRangedUnit*/, 0 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
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
		if ((iNumAttackers >= 1 && iFillableHexes >= 3) || iNumAttackers >= 2)
		{
			// Compute best way to fill the hexes
			m_PotentialBlocks.clear();
			for (unsigned int iI = 0; iI < m_TempTargets.size(); iI++)
			{
				pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if (!FindClosestUnit(pLoopPlot, 1, false /*bMustBeRangedUnit*/, 0 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
				{
					iFillableHexes--;
					if (iFillableHexes < 2)
					{
						return false;
					}
				}
				else
				{
					// Save off the units that could get here
					for (unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
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

			if (AssignFlankingUnits(iFillableHexes))
			{
				// Make moves up into hexes
				for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
				{
					CvUnit *pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
					if (pUnit)
					{
						if (pUnit->plot() == m_ChosenBlocks[iI].GetPlot())
						{
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format ("Already in a flanking position with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
									m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
								LogTacticalMessage(strMsg, false);
							}
						}
						else
						{
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format ("Moving into a flanking position with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
									m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
								LogTacticalMessage(strMsg, false);
							}

							if (pUnit->getMoves() <= 0)
							{
								UnitProcessed(pUnit->GetID());
							}
						}
					}
				}

				// Make attacks
				CvPlot *pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
				UnitHandle pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if (pDefender)
				{
					kTarget.SetAuxIntData(pDefender->GetCurrHitPoints());
					m_CurrentMoveCities.clear();
					if (FindUnitsWithinStrikingDistance(pTargetPlot, 1, false /* bNoRangedUnits */))
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
void CvTacticalAI::ExecuteCloseOnTarget(CvTacticalTarget &kTarget, CvTacticalDominanceZone *pZone)
{
	CvOperationUnit unit;
	int iDistance;
	CvPlot *pTargetPlot;
	int iRangedUnits = 0;
	int iMeleeUnits = 0;
	int iGenerals = 0;
	list<int>::iterator it;

	pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	m_OperationUnits.clear();
	m_GeneralsToMove.clear();

	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// If not naval invasion, proper domain of unit?
			if (pZone->IsNavalInvasion() ||
				(pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND))
			{
				// Find units really close to target or somewhat close that just came out of an operation
				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if (iDistance <= m_iTempZoneRadius || (iDistance <= m_iRecruitRange && pUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn()))
				{
					unit.SetUnitID(pUnit->GetID());

					if (pUnit->IsCanAttackRanged())
					{
						unit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_BOMBARD"));
						iRangedUnits++;
						m_OperationUnits.push_back(unit);

					}
					else if (pUnit->IsGreatGeneral())
					{
						unit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_CIVILIAN_SUPPORT"));
						iGenerals++;
						m_GeneralsToMove.push_back(unit);
					}
					else
					{
						unit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_FRONT_LINE"));
						iMeleeUnits++;
						m_OperationUnits.push_back(unit);
					}
				}
			}
		}
	}

	// If have any units to move...
	if (m_OperationUnits.size() > 0)
	{
		int iRangedUnitPlots = ScoreCloseOnPlots(pTargetPlot, !pZone->IsNavalInvasion() /* Land only unless invasion */);

		// Compute the moves to get the best deployment
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();
		bool bDone = false;

		int iRangedUnitsToPlace = iRangedUnits;
		int iMeleeUnitsToPlace = iMeleeUnits;

		for (unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();

			// DECREMENT COUNTERS OF UNITS NEEDED
			// High priority = target for ranged unit
			bool bSkipThisOne = false;
			bool bSafeForRanged = false;
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				if (iRangedUnitsToPlace > 0)
				{
					iRangedUnitsToPlace--;
					bSafeForRanged = true;
				}
				else
				{
					iMeleeUnitsToPlace--;
				}
			}
			else
			{
				if (iMeleeUnitsToPlace > 0)
				{
					iMeleeUnitsToPlace--;
				}
				else
				{
					// If we don't have enough places for ranged units, put them in after the melee units (so further from target)
					if (iRangedUnitPlots < iRangedUnits)
					{
						iRangedUnitsToPlace--;
						bSafeForRanged = true;
					}
					else
					{
						bSkipThisOne = true;
					}
				}
			}
			if (!bSkipThisOne)
			{
				CvPlot *pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if (FindClosestOperationUnit(pLoopPlot, bSafeForRanged, false /*bIncludeGenerals*/))
				{
					for (unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
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
			if (iMeleeUnitsToPlace == 0 && iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}

		// Now ready to make the assignments
		AssignDeployingUnits(iMeleeUnits + - iMeleeUnitsToPlace + iRangedUnits - iRangedUnitsToPlace);

		PerformChosenMoves();
	}

	if (m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral();
	}
}

/// Deploy defensive forces around a friendly city
void CvTacticalAI::ExecuteHedgehogDefense(CvTacticalTarget &kTarget, CvTacticalDominanceZone *pZone)
{
	CvOperationUnit unit;
	int iDistance;
	CvPlot *pTargetPlot;
	int iRangedUnits = 0;
	int iMeleeUnits = 0;
	list<int>::iterator it;

	pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	m_OperationUnits.clear();

	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Proper domain of unit?
			if (pZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA || !pZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{
				// Find units really close to target or somewhat close that just came out of an operation
				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if (iDistance <= m_iTempZoneRadius)
				{
					unit.SetUnitID(pUnit->GetID());

					if (pUnit->IsCanAttackRanged())
					{
						unit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_BOMBARD"));
						iRangedUnits++;
					}
					else
					{
						unit.SetPosition((MultiunitPositionTypes)GC.getInfoTypeForString("MUPOSITION_FRONT_LINE"));
						iMeleeUnits++;
					}
					m_OperationUnits.push_back(unit);
				}
			}
		}
	}

	// If have any units to move...
	if (m_OperationUnits.size() > 0)
	{
		ScoreHedgehogPlots(pTargetPlot);

		// Compute the moves to get the best deployment
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();
		bool bDone = false;

		int iRangedUnitsToPlace = iRangedUnits;
		int iMeleeUnitsToPlace = iMeleeUnits;

		for (unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();

			// DECREMENT COUNTERS OF UNITS NEEDED
			// High priority = target for ranged unit
			bool bSkipThisOne = false;
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				if (iRangedUnitsToPlace > 0)
				{
					iRangedUnitsToPlace--;
				}
				else
				{
					iMeleeUnitsToPlace--;
				}
			}
			else
			{
				if (iMeleeUnitsToPlace > 0)
				{
					iMeleeUnitsToPlace--;
				}
				else
				{
					bSkipThisOne = true;
				}
			}
			if (!bSkipThisOne)
			{
				CvPlot *pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if (FindClosestOperationUnit(pLoopPlot, (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT), false /*bIncludeGenerals*/))
				{
					for (unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
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
			if (iMeleeUnitsToPlace == 0 && iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}

		// Now ready to make the assignments
		AssignDeployingUnits(iMeleeUnits + - iMeleeUnitsToPlace + iRangedUnits - iRangedUnitsToPlace);

		PerformChosenMoves();
	}
}

/// Bombard and flank attacks (whatever is applicable) against a unit target
void CvTacticalAI::ExecutePriorityAttacksOnUnitTarget(CvTacticalTarget &kTarget)
{
	bool bAttackUnderway = false;
	bool bAttackMade = false;
	CvPlot *pTarget;

	pTarget = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());

	// Try to find a bombard first
	m_pMap->ClearDynamicFlags();
	m_pMap->SetTargetBombardCells(pTarget, m_pMap->GetBestFriendlyRange(), m_pMap->CanIgnoreLOS());

	if (kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		bAttackUnderway = ExecuteSafeBombards(kTarget);
	}

	if (kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		ExecuteProtectedBombards(kTarget, bAttackUnderway);
	}

	// Then try for a flank attack
	if (kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
	{
		m_pMap->ClearDynamicFlags();
		m_pMap->SetTargetFlankBonusCells(pTarget);

		bAttackMade = ExecuteFlankAttack(kTarget);
		if (bAttackMade)
		{
			bAttackUnderway = true;
		}
	}
}

/// Move units out of current dominance zone
void CvTacticalAI::ExecuteWithdrawMoves()
{
	CvCity *pNearestCity;
	int iTurnsToReachTarget;

	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			// Compute moves to nearest city and use as sort criteria
			pNearestCity = GC.getMap().findCity(pUnit->getX(), pUnit->getY(), m_pPlayer->GetID(), NO_TEAM, true /* bSameArea */);
			if (pNearestCity != NULL)
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
	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			// Compute moves to nearest city and use as sort criteria
			pNearestCity = GC.getMap().findCity(pUnit->getX(), pUnit->getY(), m_pPlayer->GetID(), NO_TEAM, true /* bSameArea */);
			if (pNearestCity != NULL)
			{
				MoveToEmptySpaceNearTarget(pUnit, pNearestCity->plot(), (pUnit->getDomainType()==DOMAIN_LAND));

				pUnit->finishMoves();
				UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());

				if (GC.getLogging() && GC.getAILogging())
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

/// Disable a move (probably because it is incompatible with a posture chosen)
void CvTacticalAI::TurnOffMove(TacticalAIMoveTypes eType)
{
	FStaticVector<CvTacticalMove, 100, false, c_eCiv5GameplayDLL >::iterator it;

	for (it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
	{
		if (it->m_eMoveType == eType)
		{
			it->m_iPriority = -1;
			return;
		}
	}
}

/// Finds both high and normal priority units we can use for this move (returns true if at least 1 unit found)
bool CvTacticalAI::FindUnitsForThisMove(TacticalAIMoveTypes eMove, CvPlot *pTarget, int iNumTurnsAway /* = -1 if any distance okay */, bool bRangedOnly)
{
	UnitHandle pLoopUnit;
	bool rtnValue = false;

	list<int>::iterator it;
	m_CurrentMoveUnits.clear();
	m_CurrentMoveHighPriorityUnits.clear();

	// Loop through all units available to tactical AI this turn
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if (pLoopUnit && pLoopUnit->getDomainType() != DOMAIN_AIR && pLoopUnit->IsCombatUnit())
		{
			// Make sure domain matches
			if (pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
				pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				continue;
			}

			bool bSuitableUnit = false;
			bool bHighPriority = false;

			if (eMove == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GARRISON_ALREADY_THERE") ||
				eMove == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GARRISON_1_TURN"))
			{
				// Want to put ranged units in cities to give them a ranged attack
				if (pLoopUnit->isRanged())
				{
					bSuitableUnit = true;
					bHighPriority = true;
				}

				else if (bRangedOnly)
				{
					continue;
				}

				// Don't put units with a combat strength boosted from promotions in cities, these boosts are ignored
				if (pLoopUnit->getDefenseModifier() == 0 &&
					pLoopUnit->getAttackModifier() == 0 &&
					pLoopUnit->getExtraCombatPercent() == 0)
				{
					bSuitableUnit = true;
				}
			}

			else if (eMove == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE") ||
				eMove == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_GUARD_IMPROVEMENT_1_TURN") ||
				eMove == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_BASTION_ALREADY_THERE") ||
				eMove == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_BASTION_1_TURN"))
			{
				// No ranged units or units without defensive bonuses as plot defenders
				if (!pLoopUnit->isRanged() && !pLoopUnit->noDefensiveBonus())
				{
					bSuitableUnit = true;

					// Units with defensive promotions are especially valuable
					if (pLoopUnit->getDefenseModifier() > 0 || pLoopUnit->getExtraCombatPercent() > 0)
					{
						bHighPriority = true;
					}
				}
			}

			else if (eMove == (TacticalAIMoveTypes)GC.getInfoTypeForString("TACTICAL_ANCIENT_RUINS"))
			{
				// Fast movers are top priority
				if (pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
				{
					bSuitableUnit = true;
					bHighPriority = true;
				}
				else if (pLoopUnit->IsCanAttack())
				{
					bSuitableUnit = true;
				}
			}

			if (bSuitableUnit)
			{
				// Is it even possible for the unit to reach in the number of requested turns (ignoring roads and RR)
				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());
				if (pLoopUnit->maxMoves() > 0)
				{
					int iMovesPerTurn = pLoopUnit->maxMoves() / GC.getMOVE_DENOMINATOR();
					int iLeastTurns = (iDistance + iMovesPerTurn - 1) / iMovesPerTurn;
					if (iNumTurnsAway == -1 || iLeastTurns <= iNumTurnsAway)
					{
						// If unit was suitable, and close enough, add it to the proper list
						int iMoves = TurnsToReachTarget(pLoopUnit, pTarget);
						if (iMoves != MAX_INT && (iNumTurnsAway == -1 ||
							(iNumTurnsAway == 0 && pLoopUnit->plot() == pTarget) || iMoves <= iNumTurnsAway))
						{
							CvTacticalUnit unit;
							unit.SetID(pLoopUnit->GetID());
							unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
							unit.SetMovesToTarget(iMoves);

							if (bHighPriority)
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
bool CvTacticalAI::FindUnitsWithinStrikingDistance (CvPlot *pTarget, int iNumTurnsAway, bool bNoRangedUnits, bool bNavalOnly, bool bMustMoveThrough)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if (pLoopUnit)
		{
			if (!bNavalOnly || pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				// don't use non-combat units
				if (!pLoopUnit->IsCanAttack())
				{
					continue;
				}

				if (pLoopUnit->isOutOfAttacks())
				{
					continue;
				}

				// *** Need to make this smarter and account for units that can move up on their targets and then make a ranged attack,
				//     all in the same turn. ***
				if (!bNoRangedUnits && !bMustMoveThrough && pLoopUnit->IsCanAttackRanged())
				{
					// Are we in range?
					if (plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY()) <= pLoopUnit->GetRange())
					{
						// Do we have LOS to the target?
						if (pLoopUnit->canEverRangeStrikeAt(pTarget->getX(), pTarget->getY()))
						{
							// Will we do any damage
							if (IsExpectedToDamageWithRangedAttack(pLoopUnit, pTarget))
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
				}
				else
				{
					if (CanReachInXTurns(pLoopUnit, pTarget, iNumTurnsAway))
					{
						CvTacticalUnit unit;
						unit.SetID(pLoopUnit->GetID());
						unit.SetAttackStrength(pLoopUnit->GetMaxAttackStrength(NULL, NULL, NULL));
						unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
						m_CurrentMoveUnits.push_back(unit);
						rtnValue = true;
					}
				}
			}
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort (m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Fills m_CurrentMoveCities with all cities within bombard range of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindCitiesWithinStrikingDistance (CvPlot *pTargetPlot)
{
	list<int>::iterator it;
	CvCity *pLoopCity;
	int iLoop;

	bool rtnValue = false;
	m_CurrentMoveCities.clear();

	// Loop through all of our cities
	for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()) && !IsCityInQueuedAttack(pLoopCity))
		{
			CvTacticalCity city;
			city.SetID(pLoopCity->GetID());
			m_CurrentMoveCities.push_back(city);
			rtnValue = true;
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort (m_CurrentMoveCities.begin(), m_CurrentMoveCities.end());

	return rtnValue;
}


/// Fills m_CurrentMoveUnits with all units within X turns of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestUnit (CvPlot *pTarget, int iNumTurnsAway, bool bMustBeRangedUnit, int iRangeRequired, bool bNeedsIgnoreLOS, bool bMustBeMeleeUnit, CvPlot *pRangedAttackTarget)
{
	list<int>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);
		if (pLoopUnit)
		{
			bool bValidUnit = true;

			// don't use non-combat units (but consider embarked for now)
			if (pLoopUnit->getUnitInfo().GetCombat() == 0)
			{
				bValidUnit = false;
			}

			else if (bMustBeRangedUnit && ((pTarget->isWater() && pLoopUnit->getDomainType() == DOMAIN_LAND) ||
										  (!pTarget->isWater() && !pTarget->isCity() && pLoopUnit->getDomainType() == DOMAIN_SEA)))
			{
				bValidUnit = false;
			}

			else if (bMustBeRangedUnit && !pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			else if (bMustBeRangedUnit && pLoopUnit->GetRange() < iRangeRequired)
			{
				bValidUnit = false;
			}

			else if (bMustBeRangedUnit && !pLoopUnit->canEverRangeStrikeAt(pRangedAttackTarget->getX(), pRangedAttackTarget->getY()))
			{
				bValidUnit = false;
			}

			else if (bMustBeRangedUnit && pLoopUnit->isOutOfAttacks())
			{
				bValidUnit = false;
			}

			else if (pRangedAttackTarget != NULL && bMustBeRangedUnit && !IsExpectedToDamageWithRangedAttack(pLoopUnit, pRangedAttackTarget))
			{
				bValidUnit = false;
			}

			else if (bNeedsIgnoreLOS && !pLoopUnit->IsRangeAttackIgnoreLOS())
			{
				bValidUnit = false;
			}

			else if (bMustBeMeleeUnit && pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());
			if (iNumTurnsAway == 0 && iDistance > (m_iRecruitRange / 2) || iNumTurnsAway == 1 && iDistance > m_iRecruitRange)
			{
				bValidUnit = false;
			}

			if (bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false /*bReusePaths*/, false /*bIgnoreUnits*/, (iNumTurnsAway==0));
				if (iTurns <= iNumTurnsAway)
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
	std::stable_sort (m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Fills m_CurrentMoveUnits with all units in operation that can get to target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestOperationUnit(CvPlot *pTarget, bool bSafeForRanged, bool bIncludeGenerals)
{
	FStaticVector<CvOperationUnit, SAFE_ESTIMATE_NUM_MULTIUNITFORMATION_ENTRIES, true, c_eCiv5GameplayDLL, 0>::iterator it;
	UnitHandle pLoopUnit;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to operation
	for (it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (pLoopUnit)
		{
			bool bValidUnit = true;

			if (!bSafeForRanged && pLoopUnit->IsCanAttackRanged())
			{
				bValidUnit = false;
			}

			if (!bIncludeGenerals && pLoopUnit->IsGreatGeneral())
			{
				bValidUnit = false;
			}

			if (bValidUnit)
			{
				int iTurns = TurnsToReachTarget(pLoopUnit, pTarget, false /*bReusePaths*/, false /*bIgnoreUnits*/);

				if (iTurns != MAX_INT)
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
	std::stable_sort (m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Estimates the damage we can apply to a target
int CvTacticalAI::ComputeTotalExpectedDamage(CvTacticalTarget *pTarget, CvPlot *pTargetPlot)
{
	int rtnValue = 0;
	int iExpectedDamage;
	int iExpectedSelfDamage;

	// Loop through all units who can reach the target
	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pAttacker = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());

		// Is target a unit?
		switch (pTarget->GetTargetType())
		{
		case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
			{
				UnitHandle pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if (pDefender)
				{
					if (pAttacker->IsCanAttackRanged())
					{
						iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender.pointer(), NULL, false);
						iExpectedSelfDamage = 0;
					}
					else
					{
						int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
						int iDefenderStrength = pDefender->GetMaxDefenseStrength(pTargetPlot, pAttacker.pointer());
						UnitHandle pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pDefender->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
						int iDefenderFireSupportCombatDamage = 0;
						if (pFireSupportUnit)
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
				CvCity *pCity = pTargetPlot->getPlotCity();
				if (pCity != NULL)
				{
					if (pAttacker->IsCanAttackRanged() && pAttacker->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) > pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL))
					{
						iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, false);
						iExpectedSelfDamage = 0;
					}
					else
					{
						int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
						int iDefenderStrength = pCity->getStrengthValue();
						CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pCity->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
						int iDefenderFireSupportCombatDamage = 0;
						if (pFireSupportUnit != NULL)
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
	for (unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
	{
		CvCity *pAttackingCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		iExpectedDamage = pAttackingCity->rangeCombatDamage(pTarget.pointer(), NULL, false);
		m_CurrentMoveCities[iI].SetExpectedTargetDamage(iExpectedDamage);
		rtnValue += iExpectedDamage;
	}

	return rtnValue;
}

bool CvTacticalAI::IsExpectedToDamageWithRangedAttack(UnitHandle pAttacker, CvPlot *pTargetPlot)
{
	int iExpectedDamage = 0;

	if (pTargetPlot->isCity())
	{
		CvCity *pCity = pTargetPlot->getPlotCity();
		iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, /*bIncludeRand*/ false);
	}
	else
	{
		UnitHandle pDefender = pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID());
		if (pDefender)
		{
			iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender.pointer(), NULL, false);
		}
	}

	return iExpectedDamage > 0;
}


/// Move up to our target avoiding our own units if possible
bool CvTacticalAI::MoveToEmptySpaceNearTarget(UnitHandle pUnit, CvPlot *pTarget, bool bLand)
{
	CvPlot *pLoopPlot;

	// Look at spaces adjacent to target
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iI));
		if (pLoopPlot != NULL && pLoopPlot->isWater() != bLand)
		{
			// Must be currently empty of friendly combat units
			if (!pLoopPlot->getBestDefender(m_pPlayer->GetID()))
			{
				// Enemies too
				if (!pLoopPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID()))
				{
					// And if it is a city, make sure we are friends with them, else we will automatically attack
					if (pLoopPlot->getPlotCity() == NULL || pLoopPlot->isFriendlyCity(*pUnit, false))
					{
						// Find a path to this space
						if (pUnit->GeneratePath(pLoopPlot))
						{
							// Go ahead with mission
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlot->getX(), pLoopPlot->getY());
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/// Find a multi-turn target for a land barbarian to wander towards
CvPlot *CvTacticalAI::FindBestBarbarianLandMove(UnitHandle pUnit)
{
	CvPlot *pBestMovePlot = FindNearbyTarget(pUnit, m_iLandBarbarianRange);
	if (pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	return pBestMovePlot;
}

/// Find a multi-turn target for a land barbarian to wander towards
CvPlot *CvTacticalAI::FindPassiveBarbarianLandMove(UnitHandle pUnit)
{
	CvPlot *pBestMovePlot = NULL;
	int iBestValue;
	int iValue;

	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	for (unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Is this target a camp?
		if (m_AllTargets[iI].GetTargetType()==AI_TACTICAL_TARGET_BARBARIAN_CAMP)
		{
			iValue = plotDistance(pUnit->getX(), pUnit->getY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
			if (iValue < iBestValue)
			{
				iBestValue = iValue;
				pBestMovePlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
			}
		}
	}

	if (pBestMovePlot == NULL)
	{
		pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	return pBestMovePlot;
}

/// Find a multi-turn target for a sea barbarian to wander towards
CvPlot *CvTacticalAI::FindBestBarbarianSeaMove(UnitHandle pUnit)
{
	CvPlot *pBestMovePlot = NULL;
	int iBestValue;
	int iValue;
	CvPlot *pPlot;
	CvTacticalTarget *pTarget;
	int iMovementRate;

	iMovementRate = pUnit->baseMoves();
	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	// Loop through all unit targets to find the closest
	pTarget = GetFirstUnitTarget();
	while (pTarget != NULL)
	{
		// Is this unit nearby enough?
		if (plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY()) < m_iSeaBarbarianRange)
		{
			pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if (pUnit->getArea() == pPlot->getArea())
			{
				iValue = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/);
				if (iValue < iBestValue)
				{
					iBestValue = iValue;
					pBestMovePlot = pPlot;
				}
			}
		}
		pTarget = GetNextUnitTarget();
	}

	// No units to pick on, so sail to a tile adjacent to the second closest barbarian camp
	if (pBestMovePlot == NULL)
	{
		CvPlot *pNearestCamp = NULL;
		int iBestCampDistance = MAX_INT;

		// Start by finding the very nearest camp
		pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		while (pTarget != NULL)
		{
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY());
			if (iDistance < iBestCampDistance)
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
		for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
		{
			pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
			while (pTarget != NULL)
			{
				CvPlot *pCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
				if (pCamp != pNearestCamp)
				{
					pPlot = plotDirection(pCamp->getX(), pCamp->getY(), ((DirectionTypes)jJ));
					if (pPlot && pPlot->isWater())
					{
						int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());

						// Optimization
						if (iDistance < iMovementRate * iBestValue && iDistance < (m_iSeaBarbarianRange * 3))
						{
							iValue = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/);
							if (iValue < iBestValue)
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
	if (pBestMovePlot == NULL)
	{
		// Now looking for BEST score
		iBestValue = 0;
		int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
		for (int iX = -iMovementRange; iX <= iMovementRange; iX++)
		{
			for (int iY = -iMovementRange; iY <= iMovementRange; iY++)
			{
				CvPlot* pConsiderPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
				if (!pConsiderPlot)
				{
					continue;
				}

				if (pUnit->atPlot(*pConsiderPlot))
				{
					continue;
				}

				if (!pConsiderPlot->isRevealed(pUnit->getTeam()))
				{
					continue;
				}

				if (pConsiderPlot->area() != pUnit->area())
				{
					continue;
				}

				if (!CanReachInXTurns(pUnit, pConsiderPlot, 1))
				{
					continue;
				}

				// Value them based on their explore value
				DomainTypes eDomain = pUnit->getDomainType();
				iValue = CvEconomicAI::ScoreExplorePlot(pConsiderPlot, pUnit->getTeam(), pUnit->getUnitInfo().GetBaseSightRange(), eDomain);

				// Add special value for being near enemy lands
				if (pConsiderPlot->isAdjacentOwned())
				{
					iValue += 100;
				}
				else if (pConsiderPlot->isOwned())
				{
					iValue += 200;
				}

				// If still have no value, score equal to distance from my current plot
				if (iValue == 0)
				{
					iValue = plotDistance(pUnit->getX(), pUnit->getY(), pConsiderPlot->getX(), pConsiderPlot->getY());
				}

				if (iValue > iBestValue)
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
CvPlot *CvTacticalAI::FindBarbarianExploreTarget(UnitHandle pUnit)
{
	CvPlot *pBestMovePlot = NULL;
	int iBestValue;
	int iValue;

	// Now looking for BEST score
	iBestValue = 0;
	int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();
	for (int iX = -iMovementRange; iX <= iMovementRange; iX++)
	{
		for (int iY = -iMovementRange; iY <= iMovementRange; iY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iMovementRange);
			if (!pPlot)
			{
				continue;
			}

			if (pUnit->atPlot(*pPlot))
			{
				continue;
			}

			if (!pPlot->isRevealed(pUnit->getTeam()))
			{
				continue;
			}

			if (pPlot->area() != pUnit->area())
			{
				continue;
			}

			if (!CanReachInXTurns(pUnit, pPlot, 1))
			{
				continue;
			}

			// Value them based on their explore value
			DomainTypes eDomain = pUnit->getDomainType();
			iValue = CvEconomicAI::ScoreExplorePlot(pPlot, pUnit->getTeam(), pUnit->getUnitInfo().GetBaseSightRange(), eDomain);

			// Add special value for popping up on hills or near enemy lands
			if (pPlot->isAdjacentOwned())
			{
				iValue += 100;
			}
			else if (pPlot->isOwned())
			{
				iValue += 200;
			}

			// If still have no value, score equal to distance from my current plot
			if (iValue == 0)
			{
				iValue = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
			}

			if (iValue > iBestValue)
			{
				pBestMovePlot = pPlot;
				iBestValue = iValue;
			}
		}
	}
	return pBestMovePlot;
}

/// Find a multi-turn target for a land unit to wander towards
CvPlot *CvTacticalAI::FindNearbyTarget(UnitHandle pUnit, int iRange, AITacticalTargetType eType, CvUnit *pNoLikeUnit)
{
	CvPlot *pBestMovePlot = NULL;
	int iBestValue;
	int iValue;
	CvPlot *pPlot;

	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	// Loop through all appropriate targets to find the closest
	for (unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		bool bTypeMatch = false;
		if (eType == AI_TACTICAL_TARGET_NONE)
		{
			if (m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY ||
				m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_IMPROVEMENT)
			{
				bTypeMatch = true;
			}
		}
		else if (m_ZoneTargets[iI].GetTargetType() ==  eType)
		{
			bTypeMatch = true;
		}

		// Is this unit near enough?
		if (bTypeMatch)
		{
			pPlot = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
			if (iDistance == 0)
			{
				return pPlot;
			}
			else if (iDistance < iRange)
			{
				if (pUnit->getArea() == pPlot->getArea())
				{
					if (!pNoLikeUnit || pPlot->getNumFriendlyUnitsOfType(pNoLikeUnit) == 0)
					{
						iValue = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/);
						if (iValue < iBestValue)
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
bool CvTacticalAI::NearVisibleEnemy (UnitHandle pUnit, int iRange)
{
	UnitHandle pLoopUnit;
	CvCity* pLoopCity = NULL;
	int iLoop;

	// Loop through enemies
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive() && atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
		{
			// Loop through their units
			for (pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				// Make sure this tile is visible to us
				if (pLoopUnit->plot()->isVisible(m_pPlayer->getTeam()))
				{
					// Check distance
					if (plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
					{
						return true;
					}
				}
			}

			// Loop through their cities
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				// Make sure this tile is visible to us
				if (pLoopCity->plot()->isVisible(m_pPlayer->getTeam()))
				{
					// Check distance
					if (plotDistance(pLoopCity->getX(), pLoopCity->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
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
bool CvTacticalAI::CanCoverFromEnemy(CvPlot *pPlot, int &iNumUnitsRequiredToCover, int iAttackingUnitID)
{
	CvPlot *pLoopPlot;
	int iPlotIndex;

	iNumUnitsRequiredToCover = 0;

	// Can't melee attack at sea so those hexes are always covered
	if (pPlot->isWater())
	{
		return true;
	}

	// Find all the hexes we need to cover
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		// Don't need to cover a water hex
		if (pLoopPlot != NULL && !pLoopPlot->isWater())
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			if (m_pMap->GetCell(iPlotIndex)->IsEnemyCanMovePast() && !m_pMap->GetCell(iPlotIndex)->IsFriendlyTurnEndTile())
			{
				iNumUnitsRequiredToCover++;
			}
		}
	}

	if (iNumUnitsRequiredToCover == 0)
	{
		m_ChosenBlocks.clear();
		return true;
	}
	else
	{
		if (iNumUnitsRequiredToCover > (int)m_CurrentTurnUnits.size())
		{
			return false;
		}

		// Have some unit that can cover each hex this turn?
		m_PotentialBlocks.clear();
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL && !pLoopPlot->isWater())
			{
				iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
				if (m_pMap->GetCell(iPlotIndex)->IsEnemyCanMovePast() && !m_pMap->GetCell(iPlotIndex)->IsFriendlyTurnEndTile())
				{
					if (!FindClosestUnit(pLoopPlot, 1, false /*bMustBeRangedUnit*/, 2 /*iRangeRequired*/, false /*bNeedsIgnoreLOS*/, true /*bMustBeMeleeUnit*/))
					{
						return false;
					}
					else
					{
						// Save off the units that could get here
						for (unsigned int iJ = 0; iJ < m_CurrentMoveUnits.size(); iJ++)
						{
							if (m_CurrentMoveUnits[iJ].GetID() != iAttackingUnitID)
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

	while (bChoseOne)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

		// Loop through potential blocks looking for assignments we MUST make (only one possibility)
		for (unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
		{
			CvBlockingUnit temp = m_PotentialBlocks[iI];

			if (temp.GetNumChoices() == 1)
			{
				m_NewlyChosen.push_back(temp);
				bChoseOne = true;
			}
		}

		if (bChoseOne)
		{
			// Do we have the same unit in m_NewlyChosen twice?
			if (HaveDuplicateUnit())
			{
				return false;   // Not going to work
			}
			else
			{
				// Copy to final list
				for (unsigned int jJ = 0; jJ < m_NewlyChosen.size(); jJ++)
				{
					m_ChosenBlocks.push_back(m_NewlyChosen[jJ]);
				}

				RemoveChosenUnits();

				// Do we have enough units left to cover everything?
				if (NumUniqueUnitsLeft() < (iNumUnitsRequiredToCover - (int)m_ChosenBlocks.size()))
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

	m_TemporaryBlocks.clear();
	m_ChosenBlocks.clear();

	// Loop through potential blocks looking for assignments we MUST make (only one possibility)
	while (bChoseOne)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

		for (unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
		{
			if (m_PotentialBlocks[iI].GetNumChoices() == 1)
			{
				m_NewlyChosen.push_back(m_PotentialBlocks[iI]);
				bChoseOne = true;
			}
		}

		if (bChoseOne)
		{
			// Do we have the same unit in m_NewlyChosen twice?
			if (HaveDuplicateUnit())
			{
				return false;   // Not going to work
			}
			else
			{
				// Copy to final list
				for (unsigned int jJ = 0; jJ < m_NewlyChosen.size(); jJ++)
				{
					m_ChosenBlocks.push_back(m_NewlyChosen[jJ]);
				}

				RemoveChosenUnits();

				// Do we have enough units left to cover everything?
				if (NumUniqueUnitsLeft() < (iNumUnitsRequiredToDeploy - (int)m_ChosenBlocks.size()))
				{
					return false;
				}
			}
		}
	}

	// Pick closest unit for highest priority assignment until all processed
	while (m_PotentialBlocks.size() > 0)
	{
		bChoseOne = false;
		m_NewlyChosen.clear();

		m_NewlyChosen.push_back(m_PotentialBlocks[0]);
		m_ChosenBlocks.push_back(m_PotentialBlocks[0]);

		// Don't copy the other entries for this hex so pass in the number of choices here
		RemoveChosenUnits(m_PotentialBlocks[0].GetNumChoices());

		// Do we have enough units left to cover everything?
		if (NumUniqueUnitsLeft() < (iNumUnitsRequiredToDeploy - (int)m_ChosenBlocks.size()))
		{
			return false;
		}
	}

	return true;
}

/// Make and log selected movements
void CvTacticalAI::PerformChosenMoves()
{
	// Make moves up into hexes, starting with units already close to their final destination
	std::stable_sort(m_ChosenBlocks.begin(), m_ChosenBlocks.end(), TacticalAIHelpers::CvBlockingUnitDistanceSort);

	// First loop through is for units that have a unit moving into their hex.  They need to leave first!
	for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit *pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if (pUnit)
		{
			if (pUnit->plot() != m_ChosenBlocks[iI].GetPlot() && IsInChosenMoves(pUnit->plot()) && m_ChosenBlocks[iI].GetPlot()->getNumFriendlyUnitsOfType(pUnit) == 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format ("Deploying %s (to get out of way), To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
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
	for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit *pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if (pUnit)
		{
			if (pUnit->plot() == m_ChosenBlocks[iI].GetPlot())
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					if (pUnit->canFortify(pUnit->plot()))
					{
						strMsg.Format ("Already in position, will fortify with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					}
					else
					{
						strMsg.Format ("Already in position, no move for %s, X: %d, Y: %d", pUnit->getName().GetCString(),
							m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					}
					LogTacticalMessage(strMsg);
				}
				m_ChosenBlocks[iI].SetNumChoices(-1);
			}
			else
			{
				// Someone we didn't move above?
				if (m_ChosenBlocks[iI].GetNumChoices() != -1)
				{
					CvPlot *pPlotBeforeMove = pUnit->plot();
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Deploying %s, To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
							pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
							pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
						LogTacticalMessage(strMsg);
					}

					// Use number of choices field to indicate already moved
					if (pPlotBeforeMove != pUnit->plot())
					{
						m_ChosenBlocks[iI].SetNumChoices(-1);
					}
				}
			}
		}
	}

	// Third loop is for units we still haven't been able to move (other units must be blocking their target for this turn)
	for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit *pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if (pUnit)
		{
			// Someone we didn't move above?
			if (m_ChosenBlocks[iI].GetNumChoices() != -1)
			{
				if (MoveToEmptySpaceNearTarget (UnitHandle(pUnit), m_ChosenBlocks[iI].GetPlot()))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format ("Deploying %s to space near target, To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
							pUnit->getName().GetCString(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
							pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
						LogTacticalMessage(strMsg);
					}
				}
			}
		}
	}

	// Finish moves for all units
	for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit *pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if (!pUnit->isDelayedDeath())
		{
			if (pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			}
			else
			{
				pUnit->finishMoves();
			}
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Move a great general with an operation
void CvTacticalAI::MoveGreatGeneral(CvArmyAI *pArmyAI)
{
	UnitHandle pGeneral;
	int iRange;
	CvPlot *pEvalPlot = NULL;
	CvPlot *pBestPlot = NULL;
	int iBestScore = -1;

	for (unsigned int iI = 0; iI < m_GeneralsToMove.size(); iI++)
	{
		pGeneral = m_pPlayer->getUnit(m_GeneralsToMove[iI].GetUnitID());

		if (pGeneral)
		{
			iRange = pGeneral->maxMoves() * 3;  // Enough to make a decent road move
			for (int iX = -iRange; iX <= iRange; iX++)
			{
				for (int iY = -iRange; iY <= iRange; iY++)
				{
					pEvalPlot = plotXYWithRangeCheck(pGeneral->getX(), pGeneral->getY(), iX, iY, iRange);
					if (!pEvalPlot)
					{
						continue;
					}

					if (CanReachInXTurns(pGeneral, pEvalPlot, 1))
					{
						int iScore = ScoreGreatGeneralPlot(pGeneral, pEvalPlot, pArmyAI);

						if (iScore > iBestScore && iScore > 0)
						{
							iBestScore = iScore;
							pBestPlot = pEvalPlot;
						}
					}

				}
			}

			if (pBestPlot != NULL)
			{
				ExecuteMoveToPlot(pGeneral, pBestPlot);

				if (GC.getLogging() && GC.getAILogging())
				{
					int iDistToOperationCenter = -1;

					if (pArmyAI)
					{
						CvPlot *pCOM = pArmyAI->GetCenterOfMass();
						if (pCOM)
						{
							iDistToOperationCenter = plotDistance(pBestPlot->getX(), pBestPlot->getY(), pCOM->getX(), pCOM->getY());
						}
					}

					CvString strMsg;
					strMsg.Format ("Deploying %s, To X: %d, To Y: %d, At X: %d, At Y: %d, Plot Score: %d, Dist from COM: %d",
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
bool CvTacticalAI::IsInChosenMoves(CvPlot *pPlot)
{
	for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		if (m_ChosenBlocks[iI].GetPlot() == pPlot)
		{
			return true;
		}
	}

	return false;
}

/// Were we forced to select the same unit to block twice?
bool CvTacticalAI::HaveDuplicateUnit()
{
	for (unsigned int iI = 0; iI < m_NewlyChosen.size()- 1; iI++)
	{
		for (unsigned int jJ = iI + 1; jJ < m_NewlyChosen.size(); jJ++)
		{
			if (m_NewlyChosen[iI].GetUnitID() == m_NewlyChosen[jJ].GetUnitID())
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

	for (unsigned int iI = iStartIndex; iI < m_TemporaryBlocks.size(); iI++)
	{
		bool bCopyIt = true;

		CvBlockingUnit block = m_TemporaryBlocks[iI];

		// Loop through chosen array looking for occurrences of this unit
		for (unsigned int jJ = 0; jJ < m_NewlyChosen.size() && bCopyIt; jJ++)
		{
			if (block.GetUnitID() == m_NewlyChosen[jJ].GetUnitID())
			{
				bCopyIt = false;
			}
		}

		if (bCopyIt)
		{
			m_PotentialBlocks.push_back(block);
		}
	}

	// Rebuild number of choices
	for (unsigned int iI = 0; iI < m_PotentialBlocks.size(); iI++)
	{
		int iNumFound = 0;
		CvPlot *pPlot = m_PotentialBlocks[iI].GetPlot();

		for (unsigned int jJ = 0; jJ < m_PotentialBlocks.size(); jJ++)
		{
			if (pPlot == m_PotentialBlocks[jJ].GetPlot())
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

	if (m_PotentialBlocks.size() < 2)
	{
		return m_PotentialBlocks.size();
	}

	// Copy data over and sort it so in unit ID order
	m_TemporaryBlocks.clear();
	m_TemporaryBlocks = m_PotentialBlocks;
	std::stable_sort (m_TemporaryBlocks.begin(), m_TemporaryBlocks.end());

	iCurrentID = m_TemporaryBlocks[0].GetUnitID();
	for (unsigned int iI = 1; iI < m_TemporaryBlocks.size(); iI++)
	{
		if (m_TemporaryBlocks[iI].GetUnitID() != iCurrentID)
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

	if (iBlocksToCreate == 0)
	{
		return true;
	}

	FAssertMsg (iBlocksToCreate <= NUM_DIRECTION_TYPES, "More than NUM_DIRECTION_TYPES hexes to block. Will cause array overflows and performance issues!");

	if (iBlocksToCreate > 0 && iBlocksToCreate < NUM_DIRECTION_TYPES)
	{
		m_NewlyChosen.clear();

		// Set up indexes pointing to the possible choices
		int iLevel = 0;
		int iCurIndex = 0;
		while (iLevel < iBlocksToCreate)
		{
			iFirst[iLevel] = iCurIndex;
			int iNumChoices = m_PotentialBlocks[iCurIndex].GetNumChoices();

			FAssertMsg (iNumChoices > 0, "Invalid number of tactical AI move choices. Will cause array overflows and performance issues!");
			FAssertMsg (iNumChoices + iCurIndex <= (int)m_PotentialBlocks.size(), "Invalid number of tactical AI move choices. Will cause array overflows and performance issues!");

			iLast[iLevel] = iCurIndex + iNumChoices - 1;
			iCurIndex = iLast[iLevel] + 1;
			iLevel++;
		}

		for (int iI = 0; iI < iBlocksToCreate; iI++)
		{
			iCurrent[iI] = iFirst[iI];
		}

		// Loop through each possibility
		bool bDone = false;
		while (!bDone)
		{
			m_TemporaryBlocks.clear();

			// Create this choice
			for (int iI = 0; iI < iBlocksToCreate; iI++)
			{
				if (iI >= (int)m_PotentialBlocks.size())
				{
					FAssertMsg (false, "Invalid fast vector index - show Ed");
				}
				m_TemporaryBlocks.push_back(m_PotentialBlocks[iCurrent[iI]]);
			}

			iScore = ScoreAssignments(iNumUnitsDesired != iNumUnitsAcceptable /*bCanLeaveOpenings*/);

			// If best so far, save it off
			if (iScore > iBestScore)
			{
				m_NewlyChosen.clear();
				for (unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
				{
					if (m_TemporaryBlocks[iI].GetDistanceToTarget() != MAX_INT)
					{
						m_NewlyChosen.push_back(m_TemporaryBlocks[iI]);
					}
				}
				iBestScore = iScore;
			}

			// Increment proper index
			bool bIncrementDone = false;
			for (int iLevelIndex = iBlocksToCreate - 1; !bIncrementDone && iLevelIndex >= 0 && iLevelIndex < NUM_DIRECTION_TYPES; iLevelIndex--)
			{
				// See if at end of line for this index
				if (iCurrent[iLevelIndex] + 1 > iLast[iLevelIndex])
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

			if (!bIncrementDone)
			{
				bDone = true;
			}
		}

		// Copy final choices into output
		for (unsigned int iI = 0; iI < m_NewlyChosen.size(); iI++)
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
	for (unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
	{
		for (unsigned int jJ = iI + 1; jJ < m_TemporaryBlocks.size(); jJ++)
		{
			if (m_TemporaryBlocks[iI].GetUnitID() == m_TemporaryBlocks[jJ].GetUnitID())
			{
				if (!bCanLeaveOpenings)
				{
					return -1;
				}
				else
				{
					// "Clear" the move with greater distance by setting it to MAX_INT distance
					if (m_TemporaryBlocks[iI].GetDistanceToTarget() < m_TemporaryBlocks[jJ].GetDistanceToTarget())
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
	for (unsigned int iI = 0; iI < m_TemporaryBlocks.size(); iI++)
	{
		if (m_TemporaryBlocks[iI].GetDistanceToTarget() != MAX_INT)
		{
			iScore += (10000 - (m_TemporaryBlocks[iI].GetDistanceToTarget() * 1000));
			iScore += m_pPlayer->getUnit(m_TemporaryBlocks[iI].GetUnitID())->GetPower();
		}
	}

	return iScore;
}

/// Pick best hexes for closing in on an enemy city. Returns number of ranged unit plots found
int CvTacticalAI::ScoreCloseOnPlots(CvPlot *pTarget, bool bLandOnly)
{
	int iScore;
	CvPlot *pPlot;
	CvTacticalAnalysisCell *pCell;
	bool bChoiceBombardSpot;
	bool bSafeFromAttack;
	CvTacticalTarget target;
	int iRtnValue = 0;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

	for (int jJ = 0; jJ < NUM_CITY_PLOTS; jJ++)
	{
		pPlot = plotCity(pTarget->getX(), pTarget->getY(), jJ);

		if (pPlot != NULL)
		{
			bChoiceBombardSpot = false;
			bSafeFromAttack = true;

			int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);

			if ((bLandOnly && pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/)) || (!bLandOnly && pCell->CanUseForOperationGathering()))
			{
				bool bCloseEnough = false;
				for (unsigned int iI = 0; iI < m_OperationUnits.size() && !bCloseEnough; iI++)
				{
					UnitHandle pUnit = m_pPlayer->getUnit(m_OperationUnits[iI].GetUnitID());
					if (pUnit)
					{
						if (plotDistance(pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY()) <= m_iDeployRadius)
						{
							bCloseEnough = true;
						}
					}
				}

				if (bCloseEnough)
				{
					iScore = 600 - (iPlotDistance * 100);

					// Top priority is hexes to bombard from (within range but not adjacent)
					if (pCell->IsWithinRangeOfTarget() && iPlotDistance > 1)
					{
						bChoiceBombardSpot = true;
						iRtnValue++;
					}

					if (pCell->IsSubjectToAttack())
					{
						iScore -= 30;
						bSafeFromAttack = false;
					}

					if (pCell->IsEnemyCanMovePast())
					{
						iScore -= 30;
					}
					if (pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
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
					if (bChoiceBombardSpot)
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
void CvTacticalAI::ScoreHedgehogPlots(CvPlot *pTarget)
{
	int iScore;
	CvPlot *pPlot;
	CvTacticalAnalysisCell *pCell;
	bool bChoiceBombardSpot;
	bool bSafeFromAttack;
	CvTacticalTarget target;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

	for (int jJ = 0; jJ < NUM_CITY_PLOTS; jJ++)
	{
		pPlot = plotCity(pTarget->getX(), pTarget->getY(), jJ);

		if (pPlot != NULL)
		{
			bChoiceBombardSpot = false;
			bSafeFromAttack = true;

			int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			pCell = m_pMap->GetCell(iPlotIndex);

			if (pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/))
			{
				iScore = 600 - (iPlotDistance * 150);

				if (pCell->IsSubjectToAttack())
				{
					iScore += 100;
					bSafeFromAttack = false;
				}
				if (pCell->IsEnemyCanMovePast())
				{
					iScore += 50;
				}
				if (pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
				{
					iScore += 100;
				}
				else
				{
					iScore += pCell->GetDefenseModifier() * 4;
				}

				pCell->SetSafeForDeployment(bSafeFromAttack);
				pCell->SetDeploymentScore(iScore);

				// Save this in our list of potential targets
				target.SetTargetX(pPlot->getX());
				target.SetTargetY(pPlot->getY());
				target.SetAuxIntData(iScore);

				// A bit of a hack -- use high priority targets to indicate good plots for ranged units
				if (bSafeFromAttack)
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
int CvTacticalAI::ScoreGreatGeneralPlot(UnitHandle pGeneral, CvPlot *pTarget, CvArmyAI *pArmyAI)
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
	if (pTarget->isCity() && pTarget->getOwner() != ePlayer)
	{
		return 0;
	}

	// Another player's unit here?
	CvUnit *pUnit = pTarget->getUnitByIndex(0);
	if (pUnit && pUnit->getOwner() != ePlayer)
	{
		return 0;
	}

	// Danger value
	iDangerValue = m_pPlayer->GetPlotDanger(*pTarget);

	// Friendly city here?
	if (pTarget->isCity())
	{
		bFriendlyCity = true;
		CvCity *pCity = pTarget->getPlotCity();

		iFriendlyCityStrength = pCity->getStrengthValue() * 100;
		iFriendlyCityStrength *= (GC.getMAX_CITY_HIT_POINTS() - pCity->getDamage());
		iFriendlyCityStrength /= GC.getMAX_CITY_HIT_POINTS();

		if (iDangerValue > (iFriendlyCityStrength * 2))
		{
			iDangerDivisor = 5;
		}
		else if (iDangerValue > iFriendlyCityStrength)
		{
			iDangerDivisor = 3;
		}
		else if (iDangerValue > (iFriendlyCityStrength / 2))
		{
			iDangerDivisor = 2;
		}
	}

	// Friendly unit here?
	pBestDefender = pTarget->getBestDefender(m_pPlayer->GetID());
	if (pBestDefender && pBestDefender->GetID() != pGeneral->GetID())
	{
		iFriendlyUnitBasePower = pBestDefender->GetBaseCombatStrengthConsideringDamage() * 250;

		if (iDangerValue > (iFriendlyUnitBasePower * 2))
		{
			iDangerDivisor = 5;
		}
		else if (iDangerValue > iFriendlyUnitBasePower)
		{
			iDangerDivisor = 3;
		}
		else if (iDangerValue > (iFriendlyUnitBasePower / 2))
		{
			iDangerDivisor = 2;
		}

		iFriendlyUnitFinalPower = iFriendlyUnitBasePower;

		// Deemphasize if hasn't finished moves yet
		if (IsInQueuedAttack(pBestDefender.pointer()) && !pBestDefender->isRanged())
		{
			// Melee unit: might take losses attacking, so reduce the desirability
			iFriendlyUnitFinalPower /= 3;
		}

		else if (pBestDefender->getMoves() > 0)
		{
			// Could move anywhere, not a good place to go
			iFriendlyUnitFinalPower /= 25;
		}

		// Emphasize member of this operation
		if (pArmyAI && (pBestDefender->getArmyID() != -1) &&(pBestDefender->getArmyID() == pArmyAI->GetID()))
		{
			iFriendlyUnitFinalPower *= 5;
		}
	}

	// Distance to center of army (if still under operational AI)
	if (pArmyAI)
	{
		CvPlot *pCOM = pArmyAI->GetCenterOfMass();
		if (pCOM)
		{
			iDistToOperationCenter = plotDistance(pTarget->getX(), pTarget->getY(), pCOM->getX(), pCOM->getY());
		}
	}

	// Near an attack we already have planned?
	iNearbyQueuedAttacks = NearXQueuedAttacks(pTarget, 2);

	// COMPUTE SCORE
	//  Entering a city
	if (bFriendlyCity)
	{
		iScore = iFriendlyCityStrength;

		// If part of an army moving to target, don't hide in a city
		if (pArmyAI)
		{
			iScore = 1000;
		}
	}
	// Stacking with a unit
	else if (iFriendlyUnitFinalPower > 0)
	{
		iScore = iFriendlyUnitFinalPower;
	}
	// Moving to an empty tile
	else
	{
		iScore = 10;
	}

	if (iNearbyQueuedAttacks > 0)
	{
		iScore *= (5 * iNearbyQueuedAttacks);
	}

	if (iDangerValue > 0)
	{
		iScore /= iDangerDivisor;
	}

	if (iDistToOperationCenter < 20)
	{
		// Anywhere near center is really good
		if (iDistToOperationCenter <= 3)
		{
			iScore += 5000000;
		}

		// Sliding scale further off
		else
		{
			iScore += (250000 * (20 - iDistToOperationCenter));
		}
	}

	return iScore;
}

/// Remove a unit that we've allocated from list of units to move this turn
void CvTacticalAI::UnitProcessed (int iID, bool bMarkTacticalMap)
{
	UnitHandle pUnit;
	int iPlotIndex;
	CvTacticalAnalysisCell *pCell;

	m_CurrentTurnUnits.remove(iID);
	pUnit = m_pPlayer->getUnit(iID);

	CvAssert (pUnit);
	pUnit->SetTurnProcessed(true);

	if (bMarkTacticalMap)
	{
		CvTacticalAnalysisMap *pMap = GC.getGame().GetTacticalAnalysisMap();
		if (pMap->IsBuilt())
		{
			iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
			pCell = pMap->GetCell(iPlotIndex);
			pCell->SetFriendlyTurnEndTile(true);
		}
	}
}

/// Do we want to process moves for this dominance zone?
bool CvTacticalAI::UseThisDominanceZone(CvTacticalDominanceZone *pZone)
{
	bool bIsOurCapital = false;
	int iCityID = -1;
	if (pZone->GetClosestCity() != NULL)
	{
		iCityID = pZone->GetClosestCity()->GetID();
		bIsOurCapital = (pZone->GetClosestCity() == m_pPlayer->getCapitalCity());
	}

	return (bIsOurCapital || pZone->GetRangeClosestEnemyUnit() <= (m_iRecruitRange / 2) ||
		(pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_NO_UNITS_PRESENT && pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_NOT_VISIBLE));
}


/// Is this civilian target of the highest priority?
bool CvTacticalAI::IsVeryHighPriorityCivilianTarget(CvTacticalTarget *pTarget)
{
	bool bRtnValue = false;
	CvUnit *pUnit = (CvUnit *)pTarget->GetAuxData();
	if (pUnit)
	{
		if (pUnit->AI_getUnitAIType() == UNITAI_GENERAL)
		{
			bRtnValue = true;
		}
	}
	return bRtnValue;
}

/// Is this civilian target of high priority?
bool CvTacticalAI::IsHighPriorityCivilianTarget(CvTacticalTarget *pTarget)
{
	bool bRtnValue = false;
	CvUnit *pUnit = (CvUnit *)pTarget->GetAuxData();
	if (pUnit)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
		int iEstimatedEndTurn = GC.getGame().getEstimateEndTurn();
		if (pkUnitInfo)
		{
			// Priorities defined in XML
			if (pkUnitInfo->GetCivilianAttackPriority() == CIVILIAN_ATTACK_PRIORITY_HIGH)
			{
				bRtnValue = true;
			}
			else if (pkUnitInfo->GetCivilianAttackPriority() == CIVILIAN_ATTACK_PRIORITY_HIGH_EARLY_GAME_ONLY)
			{
				if (GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3))
				{
					bRtnValue =  true;
				}
			}
			if (!bRtnValue && pUnit->AI_getUnitAIType() == UNITAI_SETTLE)
			{
				//Settlers
				if (GET_PLAYER(pUnit->getOwner()).getNumCities() < 5) //small player?
				{
					bRtnValue = true;
				}
				else if (GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3)) //early game?
				{
					bRtnValue =  true;
				}
				else if (GET_PLAYER(pUnit->getOwner()).IsCramped()) //player really needs to expand?
				{
					bRtnValue = true;
				}
			}
			if (!bRtnValue && m_pPlayer->isBarbarian())
			{
				bRtnValue = true; //always high priority for barbs
			}
		}
	}
	return bRtnValue;
}

/// Is this civilian target of medium priority?
bool CvTacticalAI::IsMediumPriorityCivilianTarget(CvTacticalTarget *pTarget)
{
	bool bRtnValue = false;
	CvUnit *pUnit = (CvUnit *)pTarget->GetAuxData();
	if (pUnit)
	{
		int iEstimatedEndTurn = GC.getGame().getEstimateEndTurn();
		if (pUnit->isEmbarked() && !pUnit->IsCombatUnit()) //embarked civilians
		{
			bRtnValue = true;
		}
		else if (pUnit->AI_getUnitAIType() == UNITAI_SETTLE)
		{
			if (GC.getGame().getGameTurn() >= (iEstimatedEndTurn / 3))
			{
				bRtnValue =  true;
			}
		}
		else if (pUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			if (GC.getGame().getGameTurn() < (iEstimatedEndTurn / 3)) //early game?
			{
				bRtnValue =  true;
			}
		}
	}
	return bRtnValue;
}

/// Log current status of the operation
void CvTacticalAI::LogTacticalMessage(CvString& strMsg, bool bSkipLogDominanceZone)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strTemp;
		FILogFile *pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strTemp = "no zone, ";
		if (!bSkipLogDominanceZone)
		{
			CvTacticalDominanceZone *pZone = GC.getGame().GetTacticalAnalysisMap()->GetZone(m_iCurrentZoneIndex);
			if (pZone != NULL)
			{
				strTemp.Format ("Zone ID: %d, ", pZone->GetDominanceZoneID());
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
	if (GC.getPlayerAndCityAILogSplit())
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
	switch (eMission)
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

FDataStream & operator<<(FDataStream & saveTo, const AITacticalMission & readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, AITacticalMission & writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AITacticalMission>(v);
	return loadFrom;
}

// HELPER FUNCTIONS

/// Sort CvBlockingUnit by a non-standard criteria
bool TacticalAIHelpers::CvBlockingUnitDistanceSort (CvBlockingUnit obj1, CvBlockingUnit obj2)
{
	return obj1.GetDistanceToTarget() < obj2.GetDistanceToTarget();
}