/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvTacticalAI.h"
#include "CvTacticalAnalysisMap.h"
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
#include "CvBarbarians.h"
#include "CvUnitMovement.h"
#endif

#include <sstream>
#include <cmath>
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE_MILITARY)
//for easier debugging
#ifdef VPDEBUG
#define TACTDEBUG 1
#endif
int gCurrentUnitToTrack = 0;
int gTacticalCombatDebugOutput = 0;
int TACTICAL_COMBAT_MAX_TARGET_DISTANCE = 4; //not larger than 4, not smaller than 3
CvTactPosStorage gTactPosStorage(7000);
TCachedMovePlots gReachablePlotsLookup;
TCachedRangeAttackPlots gRangeAttackPlotsLookup;
#endif

void CheckDebugTrigger(int iUnitID)
{
	if (iUnitID == gCurrentUnitToTrack)
	{
		//put a breakpoint here if required
		OutputDebugString("match\n");
	}
}

//=====================================
// CvTacticalTarget
//=====================================

bool CvTacticalTarget::IsReadyForCapture()
{
	AITacticalTargetType eType = GetTargetType();
	if(eType == AI_TACTICAL_TARGET_CITY)
	{
		CvPlot *pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		CvCity *pCity = pPlot->getPlotCity();
		if(pCity != NULL)
		{
			//if there's only one hitpoint left
			int iCurHp = pCity->GetMaxHitPoints() - pCity->getDamage();
			return iCurHp<=1;
		}
	}

	return false;
}

/// Still a living target?
bool CvTacticalTarget::IsTargetStillAlive(PlayerTypes eAttackingPlayer)
{
	bool bRtnValue = false;

	AITacticalTargetType eType = GetTargetType();
	if(eType == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
	        eType == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
	        eType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
	{
		CvPlot* pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		CvUnit* pUnit = pPlot->getVisibleEnemyDefender(eAttackingPlayer);
		if(pUnit != NULL && !pUnit->isDelayedDeath())
		{
			bRtnValue = true;
		}
	}
	else if(eType == AI_TACTICAL_TARGET_CITY)
	{
		CvPlot *pPlot = GC.getMap().plot(m_iTargetX, m_iTargetY);
		CvCity *pCity = pPlot->getPlotCity();
		if(pCity != NULL && pCity->getOwner() != eAttackingPlayer && GET_PLAYER(eAttackingPlayer).IsAtWarWith(pCity->getOwner()))
		{
			bRtnValue = true;
		}
	}
	else
	{
		bRtnValue = true; //unknown type, assume still valid
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
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND:
	case AI_TACTICAL_TARGET_CITADEL:
	case AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE:
		bRtnValue = (eDomain == DOMAIN_LAND);
		break;

	case AI_TACTICAL_TARGET_BLOCKADE_POINT:
	case AI_TACTICAL_TARGET_BOMBARDMENT_ZONE:
	case AI_TACTICAL_TARGET_EMBARKED_CIVILIAN:
	case AI_TACTICAL_TARGET_TRADE_UNIT_SEA:
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

	saveTo << readFrom.GetLastTurn();
	saveTo << readFrom.GetX();
	saveTo << readFrom.GetY();
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvTemporaryZone& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iTemp;
	loadFrom >> iTemp;
	writeTo.SetLastTurn(iTemp);
	loadFrom >> iTemp;
	writeTo.SetX(iTemp);
	loadFrom >> iTemp;
	writeTo.SetY(iTemp);
	return loadFrom;
}

//=====================================
// CvTacticalAI
//=====================================

/// Constructor
CvTacticalAI::CvTacticalAI(void)
{
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

	//do this after setting the player pointer!
	Reset();

	// Initialize AI constants from XML
	m_iRecruitRange = GC.getAI_TACTICAL_RECRUIT_RANGE();
	m_iDeployRadius = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
	m_iLandBarbarianRange = std::max(1, GC.getGame().getHandicapInfo().getBarbarianLandTargetRange());
	if (MOD_BALANCE_CORE_DIFFICULTY)
		m_iSeaBarbarianRange = std::max(1, GC.getGame().getHandicapInfo().getBarbarianSeaTargetRange());
	else
		m_iSeaBarbarianRange = std::max(1, GC.getGame().getHandicapInfo().getBarbarianSeaTargetRange() / 2);
}

/// Deallocate memory created in initialize
void CvTacticalAI::Uninit()
{
}

/// Reset variables
void CvTacticalAI::Reset()
{
	m_tacticalMap.Init(m_pPlayer ? m_pPlayer->GetID() : NO_PLAYER);
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
	kStream >> m_tacticalMap;

#if defined(MOD_BALANCE_CORE)
	int iCount;
	m_Postures.clear();
	kStream >> iCount;
	for (int i=0; i<iCount; i++)
	{
		int iPlayer, iCityID, iPosture;
		bool bWater;
		kStream >> iPlayer;
		kStream >> iCityID;
		kStream >> iPosture;
		kStream >> bWater;
		CvTacticalPosture temp( (PlayerTypes)iPlayer, bWater, iCityID, (AITacticalPosture)iPosture );
		m_Postures.push_back(temp);
	}
#endif
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
	kStream << m_tacticalMap;

#if defined(MOD_BALANCE_CORE)
	kStream << m_Postures.size();
	for(unsigned int iI = 0; iI < m_Postures.size(); iI++)
	{
		kStream << m_Postures[iI].GetPlayer();
		kStream << m_Postures[iI].GetCityID();
		kStream << m_Postures[iI].GetPosture();
		kStream << m_Postures[iI].IsWater();
	}
#endif
}

/// Mark all the units that will be under tactical AI control this turn
void CvTacticalAI::RecruitUnits()
{
	int iLoop;
	m_CurrentTurnUnits.clear();

	// Loop through our units
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// debugging hook
		if (gCurrentUnitToTrack == pLoopUnit->GetID())
			pLoopUnit->DumpDangerInNeighborhood();

		// we reset this every turn in order to spot units falling through the cracks
		// todo: ideally we have some persistency and prefer to assign the same tasks across turns ...
		pLoopUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
		pLoopUnit->setHomelandMove(AI_HOMELAND_MOVE_NONE);

		//LogTacticalMessage( CvString::format("looking to recruit %s %d at (%d,%d) with %d hp",
		//	pLoopUnit->getName().c_str(),pLoopUnit->GetID(),pLoopUnit->getX(),pLoopUnit->getY(),pLoopUnit->GetCurrHitPoints()).c_str() );

		// reset mission AI so we don't see stale information (debugging only)
		// careful with explorers though, for performance reasons their mission is persistent
		if (pLoopUnit->GetMissionAIType()!=MISSIONAI_EXPLORE)
			pLoopUnit->SetMissionAI(NO_MISSIONAI,NULL,NULL);

		// Never want immobile/dead units, explorers, ones that have already moved or automated human units
		if(pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath() || !pLoopUnit->canMove() || pLoopUnit->isHuman() )
			continue;

		// we want all combat ready air units, except nukes (those go through operational AI)
		if (pLoopUnit->getDomainType() == DOMAIN_AIR)
		{
			if (pLoopUnit->getDamage() > 50 || ShouldRebase(pLoopUnit) || pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_ICBM)
				continue;

			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
		// we want ALL the barbarians
		else if (pLoopUnit->isBarbarian()) 
		{
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
		// explorers should be handled by homeland AI
		else if ( pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN || 
					pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE ||
					pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA )
		{
			continue;
		}
		// Now down to land and sea units
		else
		{
			//no civilians with a few exceptions
			if (!pLoopUnit->IsCombatUnit())
			{
				 //if it's a general or admiral and not a field commander, we don't want it
				if( pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral() || pLoopUnit->IsCityAttackSupport())
				{
					GreatPeopleDirectiveTypes eDirective = pLoopUnit->GetGreatPeopleDirective();
					if (eDirective == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
						continue;
				}
				else
					continue;
			}

			//finally, these are our combat units
			if (pLoopUnit->getArmyID() != -1)
				//army units will be moved as part of the army moves
				pLoopUnit->setTacticalMove(AI_TACTICAL_OPERATION);
			else
				m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
	}

#if defined(MOD_CORE_DEBUGGING)
	if (MOD_CORE_DEBUGGING)
	{
		for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
			if (!pUnit)
				continue;
			CvString msg = CvString::format("current turn tactical unit %s %d at %d,%d\n", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY() );
			LogTacticalMessage( msg );
		}
	}
#endif
}

/// Update the AI for units
void CvTacticalAI::Update()
{
	AI_PERF_FORMAT("AI-perf.csv", ("Tactical AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	FindTacticalTargets();

	//do this after updating the target list!
	RecruitUnits();

	// Loop through each dominance zone assigning moves
	ProcessDominanceZones();
}

#if defined(MOD_BALANCE_CORE)
CvCity* CvTacticalAI::GetNearestTargetCity(CvPlot* pPlot)
{
	CvCity* pBestCity = NULL;
	int iBestValue = 0;
	if(pPlot == NULL)
		return pBestCity;
	
	// Loop through all the zones we have this turn
	for(int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
	{
		CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);

		// Check to make sure we want to use this zone
		if(UseThisDominanceZone(pZone))
		{
			CvCity* pClosestCity = pZone->GetZoneCity();

			if(pClosestCity != NULL && pClosestCity->getOwner() != m_pPlayer->GetID() && GET_TEAM(m_pPlayer->getTeam()).isAtWar(pClosestCity->getTeam()))
			{
				int iValue = 100;
				int iDistance = plotDistance(pClosestCity->getX(), pClosestCity->getY(), pPlot->getX(), pPlot->getY());
				iValue -= (iDistance * 3);

				if(pZone->GetOverallFriendlyStrength() > 0)
				{
					iValue += pZone->GetOverallFriendlyStrength();
				}
				if(IsTemporaryZoneCity(pClosestCity))
				{
					iValue *= 5;
				}
				if(iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestCity = pClosestCity;
				}
			}
		}
	}
	return pBestCity;
}
#endif

// TEMPORARY DOMINANCE ZONES

/// Add a temporary dominance zone around a short-term target
void CvTacticalAI::AddTemporaryZone(CvPlot* pPlot, int iDuration)
{
	if (!pPlot)
		return;

	CvTemporaryZone zone;
	zone.SetX(pPlot->getX());
	zone.SetY(pPlot->getY());
	zone.SetLastTurn( GC.getGame().getGameTurn() + iDuration );

	m_TempZones.push_back(zone);
}

/// Remove a temporary dominance zone we no longer need to track
void CvTacticalAI::DeleteTemporaryZone(CvPlot* pPlot)
{
	std::vector<CvTemporaryZone> zonesCopy(m_TempZones);
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
	std::vector<CvTemporaryZone> zonesCopy(m_TempZones);
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
		if(m_TempZones[iI].GetX() == pCity->getX() && m_TempZones[iI].GetY() == pCity->getY() )
			return true;

	return false;
}

// PRIVATE METHODS

/// Establish postures for each dominance zone (taking into account last posture)
void CvTacticalAI::UpdatePostures()
{
	std::vector<CvTacticalPosture> newPostures;

	// Loop through all the zones we have this turn
	for(int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
	{
		CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);

		// Check to make sure we want to use this zone
		if(UseThisDominanceZone(pZone))
		{
			int iCityID = -1;
			if(pZone->GetZoneCity() != NULL)
			{
				iCityID = pZone->GetZoneCity()->GetID();
			}

			AITacticalPosture eLastPosture = FindPosture(pZone);
			AITacticalPosture eNewPosture = SelectPosture(pZone, eLastPosture);
			newPostures.push_back(CvTacticalPosture(pZone->GetOwner(), pZone->IsWater(), iCityID, eNewPosture));

			if(GC.getLogging() && GC.getAILogging() && eNewPosture != AI_TACTICAL_POSTURE_NONE)
			{
				CvString szPostureMsg;
				szPostureMsg.Format("Zone ID: %d, %s, %s, ", pZone ? pZone->GetZoneID() : -1, pZone->IsWater() ? "Water" : "Land", pZone->GetZoneCity() ? pZone->GetZoneCity()->getName().c_str() : "none");

				switch(eNewPosture)
				{
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
				}
				LogTacticalMessage(szPostureMsg);
			}
		}
	}

	// New postures become current ones
	m_Postures = newPostures;
}

/// Select a posture for a specific zone
AITacticalPosture CvTacticalAI::SelectPosture(CvTacticalDominanceZone* pZone, AITacticalPosture /*eLastPosture*/)
{
	AITacticalPosture eChosenPosture = AI_TACTICAL_POSTURE_NONE;
	if (!pZone)
		return eChosenPosture;

	int iDominancePercentage = GetTacticalAnalysisMap()->GetDominancePercentage();
	eTacticalDominanceFlags eOverallDominance = pZone->GetOverallDominanceFlag(); //this one is precomputed
	eTacticalDominanceFlags eRangedDominance = pZone->IsWater() ? pZone->GetNavalRangedDominanceFlag(iDominancePercentage) : pZone->GetRangedDominanceFlag(iDominancePercentage);

	//are our forces mostly ranged and the enemy has a lot of melee units?
	bool bInDangerOfCounterattack = pZone->IsWater() ? pZone->GetEnemyNavalStrength() > 2 * pZone->GetFriendlyNavalStrength() && pZone->GetFriendlyNavalRangedStrength() > 2 * pZone->GetFriendlyNavalStrength() :
									pZone->GetEnemyMeleeStrength() > 2 * pZone->GetFriendlyMeleeStrength() && pZone->GetFriendlyRangedStrength() > 2 * pZone->GetFriendlyMeleeStrength();

	bool bLandRangedMayKillFleet = false;
	if (pZone->IsWater())
	{
		CvTacticalDominanceZone* pLandZone = GetTacticalAnalysisMap()->GetZoneByCity(pZone->GetZoneCity(), false);
		if (pLandZone && pLandZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && pLandZone->GetEnemyRangedStrength() > pZone->GetFriendlyNavalRangedStrength())
			bLandRangedMayKillFleet = true;
	}

	// Choice based on whose territory this is
	switch(pZone->GetTerritoryType())
	{
	case TACTICAL_TERRITORY_ENEMY:
	case TACTICAL_TERRITORY_TEMP_ZONE:
	{
		// Default for this zone
		eChosenPosture = (pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE) ? AI_TACTICAL_POSTURE_STEAMROLL : AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;

		// Temporary zone: Attack if possible, if defenses are strong try to wear them down first
		CvCity *pClosestCity = pZone->GetZoneCity();
		if (pClosestCity && IsTemporaryZoneCity(pClosestCity))
		{
			if (pZone->GetEnemyMeleeStrength() > pZone->GetFriendlyMeleeStrength())
			{
				eChosenPosture = (eOverallDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE : AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}
			else
			{
				eChosenPosture = (eOverallDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE : AI_TACTICAL_POSTURE_STEAMROLL;
			}
		}
		// Withdraw if enemy dominant overall or we are vulnerable to counterattacks
		else if (eOverallDominance == TACTICAL_DOMINANCE_ENEMY || bInDangerOfCounterattack)
		{
			//try to grab it ...
			if (pClosestCity && pClosestCity->isInDangerOfFalling())
				eChosenPosture = AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
			else
				eChosenPosture = AI_TACTICAL_POSTURE_WITHDRAW;
		}
		else if (eOverallDominance == TACTICAL_DOMINANCE_EVEN)
		{
			if (pZone->GetEnemyMeleeStrength() > pZone->GetFriendlyMeleeStrength())
			{
				//if we have ranged dominance, keep our risk lower
				eChosenPosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE : AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}
			else
			{
				//if we have ranged dominance and melee dominance, go all in
				eChosenPosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_STEAMROLL : AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}
		}
		else if (eOverallDominance == TACTICAL_DOMINANCE_FRIENDLY)
		{
			if (bLandRangedMayKillFleet)
				eChosenPosture = AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			else
				eChosenPosture = AI_TACTICAL_POSTURE_STEAMROLL;
		}
		break;
	}

	case TACTICAL_TERRITORY_NEUTRAL:
	case TACTICAL_TERRITORY_NO_OWNER:
	{
		// Default for this zone
		eChosenPosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE : AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;

		if (eOverallDominance == TACTICAL_DOMINANCE_ENEMY)
		{
			if (pZone->GetEnemyMeleeStrength() > pZone->GetFriendlyMeleeStrength())
			{
				//if we have ranged dominance, keep our risk lower
				eChosenPosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE : AI_TACTICAL_POSTURE_WITHDRAW;
			}
			else
			{
				//if we have ranged dominance, keep our risk lower
				eChosenPosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_EXPLOIT_FLANKS : AI_TACTICAL_POSTURE_WITHDRAW;
			}
		}

		break;
	}
	case TACTICAL_TERRITORY_FRIENDLY:
	{
		// Default for this zone
		eChosenPosture = AI_TACTICAL_POSTURE_COUNTERATTACK;

		if (eOverallDominance == TACTICAL_DOMINANCE_ENEMY)
			eChosenPosture = AI_TACTICAL_POSTURE_HEDGEHOG;

		break;
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
				if(pZone->GetZoneCity() != NULL)
				{
					iCityID = pZone->GetZoneCity()->GetID();
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

/// Make lists of everything we might want to target with the tactical AI this turn
void CvTacticalAI::FindTacticalTargets()
{
	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();

	// Clear out target list since we rebuild it each turn
	m_AllTargets.clear();
	m_ZoneTargets.clear();

	int iReleaseTurn = (GC.getGame().GetBarbarianReleaseTurn() * GC.getGame().getGameSpeedInfo().getTrainPercent()) / 100;
	bool bNoBarbsAllowedYet = GC.getGame().getGameTurn() < iReleaseTurn;

	// Look at every tile on map
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		bool bValidPlot = pLoopPlot->isRevealed(m_pPlayer->getTeam());

		// Make sure I am not a barbarian who can not move into owned territory this early in the game
		if (m_pPlayer->isBarbarian())
		{
			if (bNoBarbsAllowedYet && pLoopPlot->isOwned())
				bValidPlot = false;
		}

		if (bValidPlot)
		{
			CvTacticalTarget newTarget;
			newTarget.SetTargetX(pLoopPlot->getX());
			newTarget.SetTargetY(pLoopPlot->getY());
			newTarget.SetDominanceZone(GetTacticalAnalysisMap()->GetDominanceZoneID(iI));

			// Have a ...
			// ... friendly city?
			CvCity* pCity = pLoopPlot->getPlotCity();
			if (pCity != NULL)
			{
				if (m_pPlayer->GetID() == pCity->getOwner())
				{
					CvTacticalDominanceZone* pLandZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, false);
					CvTacticalDominanceZone* pWaterZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, true);
					int iBorderScore = (pLandZone ? pLandZone->GetBorderScore(DOMAIN_LAND) : 0) + (pWaterZone ? pWaterZone->GetBorderScore(DOMAIN_SEA) : 0);
					if (iBorderScore > 0)
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
						newTarget.SetAuxIntData(iBorderScore);
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... enemy city
				else if (atWar(m_pPlayer->getTeam(), pCity->getTeam()))
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY);
					//barbarians don't care about cities much compared to normal players
					newTarget.SetAuxIntData( m_pPlayer->isBarbarian() ? 20 : 100);
					m_AllTargets.push_back(newTarget);
				}
			}
			else
			{
				// ... enemy combat unit?
				CvUnit* pUnit = pLoopPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if (pUnit != NULL)
				{
					if (pUnit->IsCivilianUnit())
						//if it can defend and it's a civilian, it must be embarked ...
						newTarget.SetTargetType(AI_TACTICAL_TARGET_EMBARKED_CIVILIAN);
					else
						//note that the HIGH/MEDIUM/LOW classification is changed later in IdentifyPriorityTargets
						newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);

					newTarget.SetUnitPtr(pUnit);
					newTarget.SetAuxIntData(50);
					m_AllTargets.push_back(newTarget);
				}
				// ... unprotected enemy civilian?
				else if (pLoopPlot->isEnemyUnit(m_pPlayer->GetID(),false,true) && !pLoopPlot->isNeutralUnit(m_pPlayer->GetID(),true,true))
				{
					for (int iUnitLoop = 0; iUnitLoop < pLoopPlot->getNumUnits(); iUnitLoop++)
					{
						CvUnit* pUnit = pLoopPlot->getUnitByIndex(iUnitLoop);
						//a bunch of redundant checks, but you never know
						if (pUnit && pUnit->IsCivilianUnit() && m_pPlayer->IsAtWarWith(pUnit->getOwner()))
						{
							newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
							newTarget.SetUnitPtr(pUnit);
							newTarget.SetAuxIntData(25);

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
							m_AllTargets.push_back(newTarget);
						}
					}
				}

				// ... barbarian camp? typically revealed but not visible, so don't even check for defenders
				// (also the camp might since have been cleared but we don't know yet - so check if it is owned now)
				// minors ignore barb camps, cannot clear them anyway
				if (pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) == GC.getBARBARIAN_CAMP_IMPROVEMENT() && 
					!pLoopPlot->isOwned() && (m_pPlayer->isMajorCiv() || m_pPlayer->isBarbarian()))
				{
					int iBaseScore = pLoopPlot->isVisible(m_pPlayer->getTeam()) ? 50 : 30;
					newTarget.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
					newTarget.SetAuxIntData(iBaseScore);
					m_AllTargets.push_back(newTarget);
				}

				// Or citadels!
				if (atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) != NO_IMPROVEMENT &&
					GC.getImprovementInfo(pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()))->GetNearbyEnemyDamage() > 0)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITADEL);
					newTarget.SetAuxIntData(80);
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy improvement?
				if (atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) != NO_IMPROVEMENT &&
					!pLoopPlot->IsImprovementPillaged())
				{
					ResourceTypes eResource = pLoopPlot->getResourceType();
					int iExtraScore = 0;
					//does this make a difference in the end?
					if (m_pPlayer->isBarbarian() || m_pPlayer->GetPlayerTraits()->IsWarmonger())
						iExtraScore = 20;

					if (eResource != NO_RESOURCE)
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE);
						newTarget.SetAuxIntData(40+iExtraScore);
						m_AllTargets.push_back(newTarget);
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
						newTarget.SetAuxIntData(5+iExtraScore);
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... enemy trade route? (city connection - not caravan)
				// checking for city connection is not enough, some people (iroquois) don't need roads, so there isn't anything to pillage 
				if (atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRevealedRouteType(m_pPlayer->getTeam()) != NO_ROUTE && 
					!pLoopPlot->IsRoutePillaged() && pLoopPlot->IsCityConnection() &&
					!GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pLoopPlot))
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
					newTarget.SetAuxIntData(10);
					m_AllTargets.push_back(newTarget);
				}

				// ... trade unit
				if (pLoopPlot->isVisible(m_pPlayer->getTeam()) && pPlayerTrade->ContainsEnemyTradeUnit(pLoopPlot))
				{
					newTarget.SetTargetType( pLoopPlot->isWater() ? AI_TACTICAL_TARGET_TRADE_UNIT_SEA : AI_TACTICAL_TARGET_TRADE_UNIT_LAND);
					newTarget.SetAuxIntData(35);
					m_AllTargets.push_back(newTarget);
				}

				// ... defensive bastion?
				if (m_pPlayer->GetID() == pLoopPlot->getOwner() && pLoopPlot->IsBorderLand( m_pPlayer->GetID() ) &&
					(pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false) >= 30 || pLoopPlot->IsChokePoint())
					)
				{
					CvCity* pDefenseCity = pLoopPlot->getOwningCity();
					if ((pDefenseCity && (pDefenseCity->isBorderCity() || pDefenseCity->isUnderSiege())) || pLoopPlot->IsChokePoint())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
						if (pDefenseCity)
						{
							int iValue = pDefenseCity->getThreatValue() + pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false);
							if (pDefenseCity->isUnderSiege())
							{
								iValue *= 5;
							}
							if (pLoopPlot->IsChokePoint() || pDefenseCity->isInDangerOfFalling())
							{
								iValue *= 10;
							}
							newTarget.SetAuxIntData(iValue);
						}
						else
						{
							int iValue = pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false);
							if (pLoopPlot->IsChokePoint())
							{
								iValue *= 10;
							}
							newTarget.SetAuxIntData(iValue);
						}
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... friendly improvement?
				if (m_pPlayer->GetID() == pLoopPlot->getOwner() &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					!pLoopPlot->IsImprovementPillaged() && !pLoopPlot->isGoody())
				{
					if (pLoopPlot->getOwningCity() != NULL && pLoopPlot->getOwningCity()->isBorderCity())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
						newTarget.SetAuxIntData(1);
						m_AllTargets.push_back(newTarget);
					}
				}

#if defined(MOD_BALANCE_CORE)
				//Enemy water plots?
				if (pLoopPlot->isRevealed(m_pPlayer->getTeam()) && pLoopPlot->isWater() && atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()))
				{
					CvCity* pOwningCity = pLoopPlot->getOwningCity();
					if (pOwningCity != NULL && pLoopPlot->isValidMovePlot(m_pPlayer->GetID(),true))
					{
						int iDistance = GET_PLAYER(pOwningCity->getOwner()).GetCityDistanceInPlots(pLoopPlot);
						//we want to stay for a while, so stay out of danger as far as possible
						if (iDistance < 4 && m_pPlayer->GetPossibleAttackers(*pLoopPlot,NO_TEAM).empty())
						{
							//try to stay away from land
							int iWeight = pLoopPlot->GetSeaBlockadeScore(m_pPlayer->GetID());

							//prefer close targets
							iWeight = max(1, iWeight - m_pPlayer->GetCityDistanceInEstimatedTurns(pLoopPlot));

							//try to support the troops
							if (pOwningCity->getDamage() > 0 || pOwningCity->isUnderSiege())
								iWeight *= 2;

							if (iWeight > 0)
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_POINT);
								newTarget.SetAuxIntData(iWeight);
								m_NavalTargets.push_back(newTarget);
							}
						}
					}
				}
#endif
			}
		}
	}

	// POST-PROCESSING ON TARGETS

	// Mark enemy units threatening our cities (or camps) as priority targets
	IdentifyPriorityTargets();
	IdentifyPriorityBarbarianTargets();

	// Also add some priority targets that we'd like to hit just because of their unit type (e.g. catapults)
	IdentifyPriorityTargetsByType();

	// make sure high prio units have the higher scores
	UpdateTargetScores();

	//Let's clean up our naval target list.
	PrioritizeNavalTargetsAndAddToMainList();

	// since the combat simulation considers a whole area, we don't need to attack for individual units
	SortTargetListAndDropUselessTargets();
}

/// Don't allow adjacent tiles to both be sentry points
void CvTacticalAI::PrioritizeNavalTargetsAndAddToMainList()
{
	// First, sort the sentry points by priority
	std::stable_sort(m_NavalTargets.begin(), m_NavalTargets.end());
	CvTacticalTarget newTarget;
	// Loop through all points in copy
	for (unsigned int iI = 0; iI < m_NavalTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		CvPlot* pPlot = GC.getMap().plot(m_NavalTargets[iI].GetTargetX(), m_NavalTargets[iI].GetTargetY());
		if (pPlot != NULL)
		{
			//Only keep top 10 targets.
			if (pPlot->getImprovementType() != NO_IMPROVEMENT || iI < 10)
			{
				newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_POINT);
				newTarget.SetTargetX(pPlot->getX());
				newTarget.SetTargetY(pPlot->getY());
				newTarget.SetDominanceZone(GetTacticalAnalysisMap()->GetDominanceZoneID(pPlot->GetPlotIndex()));
				newTarget.SetAuxIntData(m_NavalTargets[iI].GetAuxIntData());
				m_AllTargets.push_back(newTarget);
			}
		}
	}
	m_NavalTargets.clear();
}

void CvTacticalAI::ProcessDominanceZones()
{
	// Barbarian processing is straightforward -- just one big list of priorites and everything is considered at once
	if(m_pPlayer->isBarbarian())
	{
		ExtractTargetsForZone(NULL);
		AssignBarbarianMoves();
	}
	else
	{
		UpdatePostures();
		AssignGlobalHighPrioMoves();

		for(int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
		{
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);
			if(!pZone || !UseThisDominanceZone(pZone))
				continue;

			if(pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
			{
				PlotEmergencyPurchases(pZone);
				PlotDefensiveAirlifts(pZone);
			}

			AITacticalPosture ePosture = FindPosture(pZone);
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvCity* pZoneCity = pZone->GetZoneCity();
				strLogString.Format("Zone %d, %s, city of %s, posture %s",  
					pZone ? pZone->GetZoneID() : -1, pZone->IsWater() ? "water" : "land",
					pZoneCity ? pZoneCity->getNameNoSpace().c_str() : "no_name_city", 
					postureNames[ePosture]);
				LogTacticalMessage(strLogString);
			}

			ExtractTargetsForZone(pZone);
			switch (ePosture)
			{
			case AI_TACTICAL_POSTURE_WITHDRAW: //give up
				PlotWithdrawMoves(pZone);
				break;
			case AI_TACTICAL_POSTURE_HEDGEHOG: //defend
				PlotHedgehogMoves(pZone);
				break;
			case AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE: //low risk attacks on units
				PlotAttritFromRangeMoves(pZone);
				break;
			case AI_TACTICAL_POSTURE_EXPLOIT_FLANKS: //try to kill enemy units
				PlotExploitFlanksMoves(pZone);
				break;
			case AI_TACTICAL_POSTURE_STEAMROLL: //attack everything
				PlotSteamrollMoves(pZone);
				break;
			case AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE: //go for the city
				PlotSurgicalCityStrikeMoves(pZone);
				break;
			case AI_TACTICAL_POSTURE_COUNTERATTACK: //concentrated fire on enemy attackers
			default:
				PlotCounterattackMoves(pZone);
				break;
			}
		}

		//second pass: bring in reinforcements
		for (int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
		{
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);
			if (!pZone || !UseThisDominanceZone(pZone))
				continue;

			PlotReinforcementMoves(pZone);
		}

		AssignGlobalLowPrioMoves();
	}

	//failsafe
	ReviewUnassignedUnits();
}

/// Choose which tactics to run and assign units to it
void CvTacticalAI::AssignGlobalHighPrioMoves()
{
	ExtractTargetsForZone(NULL);

	//make some space near the frontline
	PlotHealMoves(true);
	//move armies first
	PlotOperationalArmyMoves();
}

/// Choose which tactics to run and assign units to it
void CvTacticalAI::AssignGlobalLowPrioMoves()
{
	ExtractTargetsForZone(NULL);

	//air attacks are already done during zone attacks, this is just for the remaining units
	PlotAirInterceptMoves();
	//do extra sweeps _after_ setting up the necessary interceptors for defense
	PlotAirSweepMoves();

	//score some goodies
	PlotCaptureBarbCamp();

	//now all attacks are done, try to move any unprocessed units out of harm's way
	PlotMovesToSafety(true);
	PlotEscortEmbarkedMoves();

	//try again now that other blocking units might have moved
	PlotHealMoves(false);

	//harass the enemy (plundering also happens during combat sim ...)
	PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, true);
	PlotPlunderTradeUnitMoves(DOMAIN_LAND);
	PlotPlunderTradeUnitMoves(DOMAIN_SEA);
	PlotCivilianAttackMoves();
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, true);
	PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, false);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, false);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, false);
	PlotBlockadeMoves();

	//pure defense
	PlotGarrisonMoves(4);
	PlotBastionMoves(2);
	PlotGuardImprovementMoves(1);
	PlotRepositionMoves();

	//civilians move out of harms way last, when all potential defenders are set in place
	PlotMovesToSafety(false);
}

/// Choose which tactics to run and assign units to it (barbarian version)
void CvTacticalAI::AssignBarbarianMoves()
{
	//barbarians don't have tactical zones, they just attack everything that moves
	PlotBarbarianAttacks();
	PlotCivilianAttackMoves();

	//barbarians like to plunder as well
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, true);
	PlotPlunderTradeUnitMoves(DOMAIN_LAND);
	PlotPlunderTradeUnitMoves(DOMAIN_SEA);

	//normal roaming unless in camp
	PlotBarbarianCampDefense();
	PlotBarbarianRoaming();

	//safety comes last for the barbarians ...
	PlotMovesToSafety(true /*bCombatUnits*/);
	PlotMovesToSafety(false /*bCombatUnits*/);
}

/// Assign a group of units to take down each city we can capture
void CvTacticalAI::ExecuteCaptureCityMoves()
{
	// See how many moves of this type we can execute
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY, AL_HIGH); pTarget!=NULL; pTarget = GetNextZoneTarget(AL_HIGH))
	{
		//mark the target whether the attack happened or not - we won't have a better chance this turn
		pTarget->SetLastAggLvl(AL_HIGH);

		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(pPlot != NULL && pPlot->isCity())
		{
			m_CurrentMoveCities.clear();
			CvCity* pCity = pPlot->getPlotCity();

			//first try the land zone
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, false);

			//does it look good there?
			if (!pZone || (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && !IsTemporaryZoneCity(pCity) && !pCity->isInDangerOfFalling()))
			{
				//try again with the water zone
				pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, true);

				if (!pZone || (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && !IsTemporaryZoneCity(pCity) && !pCity->isInDangerOfFalling()))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Zone %d, City of %s, is in enemy dominated zone - won't try to capture, X: %d, Y: %d, ",
							pZone ? pZone->GetZoneID() : -1, pCity->getNameNoSpace().c_str(), pCity->getX(), pCity->getY());
						LogTacticalMessage(strLogString);
					}

					continue;
				}
			}

			// Always recruit both naval and land based forces if available!
			if(FindUnitsWithinStrikingDistance(pPlot))
			{
				int iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
				int iExpectedDamagePerTurn = ComputeTotalExpectedDamage(pTarget, pPlot);
				//actual siege will typically be longer because not all units actually attack the city each turn
				int iMaxSiegeTurns = IsTemporaryZoneCity(pCity) ? 12 : 8;

				//assume the city heals each turn ...
				if ( (iExpectedDamagePerTurn - GC.getCITY_HIT_POINTS_HEALED_PER_TURN())*iMaxSiegeTurns < iRequiredDamage )
				{
					if (GC.getLogging() && GC.getAILogging() && pZone)
					{
						CvString strLogString;
						strLogString.Format("Zone %d, too early for attacking %s, required damage %d, expected max damage per turn %d", 
							pZone ? pZone->GetZoneID() : -1, pCity->getNameNoSpace().c_str(), iRequiredDamage, iExpectedDamagePerTurn);
						LogTacticalMessage(strLogString);
					}

					continue;
				}

				//see whether we have melee units for capturing
				int iMeleeCount = 0;
				for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
				{
					CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
					if (!pUnit || !pUnit->canMove())
						continue;

					// Are we a melee unit
					if (!pUnit->IsCanAttackRanged())
					{
						iMeleeCount++;

						//don't use too many melee units
						//the combat sim can't deal with too many units
						//make sure there are ranged units in there as well
						if (iMeleeCount > 3 && m_CurrentMoveUnits.size()>9)
							m_CurrentMoveUnits[iI].SetAttackStrength(0);
					}
				}

				if (iMeleeCount == 0 && iRequiredDamage <= 1)
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Zone %d, no melee units to capture %s", pZone ? pZone->GetZoneID() : -1, pCity->getNameNoSpace().c_str());
						LogTacticalMessage(strLogString);
					}

					continue;
				}

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Zone %d, attempting capture of %s, required damage %d, expected max damage per turn %d",
						pZone ? pZone->GetZoneID() : -1, pCity->getNameNoSpace().c_str(), iRequiredDamage, iExpectedDamagePerTurn);
					LogTacticalMessage(strLogString);
				}

				//finally do the attack. be a bit more careful if we have few melee units
				ExecuteAttackWithUnits(pPlot, iMeleeCount>2 ? AL_HIGH : AL_MEDIUM);

				// Did it work?  If so, don't need a temporary dominance zone if had one here
				if (pPlot->getOwner() == m_pPlayer->GetID())
				{
					DeleteTemporaryZone(pPlot);
					if (pZone)
					{
						pZone->SetNavalInvasion(false); // just a precaution
						pZone->SetPillageZone(false);
					}

				}

				//do we have embarked units we need to put ashore
				if (FindEmbarkedUnitsAroundTarget(pPlot, 4))
					ExecuteLandingOperation(pPlot);
			}
		}
	}
}

/// Assign a unit to capture an undefended barbarian camp
void CvTacticalAI::PlotCaptureBarbCamp()
{
	ClearCurrentMoveUnits(AI_TACTICAL_GOODY);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsForHarassing(pPlot,4,-1,-1,DOMAIN_LAND,false))
		{
			ExecuteBarbarianCampMove(pPlot);
			if( GC.getLogging() && GC.getAILogging() )
			{
				CvString strLogString;
				strLogString.Format("Trying to remove barbarian camp, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
				LogTacticalMessage(strLogString);
			}
		}
	}
}

/// Assign a group of units to attack each unit we think we can destroy
void CvTacticalAI::ExecuteDestroyUnitMoves(AITacticalTargetType targetType, bool bMustBeAbleToKill, bool bAttackAtPoorOdds)
{
	struct STargetWithTwoScoresTiebreak
	{
		STargetWithTwoScoresTiebreak(CvTacticalTarget* pTarget_, CvPlot* pPlot_, int score1_, int score2_) : pTarget(pTarget_), pPlot(pPlot_), score1(score1_), score2(score2_) {}

		bool operator<(const STargetWithTwoScoresTiebreak& other) const
		{
			return (score1<other.score1) || (score1 == other.score1 && score2<other.score2);
		}

		CvTacticalTarget* pTarget;
		CvPlot* pPlot;
		int score1, score2;
	};

	vector<STargetWithTwoScoresTiebreak> targets;

	eAggressionLevel aggLvl = bAttackAtPoorOdds ? AL_HIGH : AL_MEDIUM;

	// See how many moves of this type we can execute
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(targetType, aggLvl); pTarget!=NULL; pTarget = GetNextZoneTarget(aggLvl))
	{
		bool bUnitCanAttack = false;
		bool bCityCanAttack = false;

		// See what units we have who can reach targets this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvUnit* pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
		if (pDefender && !pDefender->isDelayedDeath())
		{
			bUnitCanAttack = FindUnitsWithinStrikingDistance(pPlot);
			bCityCanAttack = FindCitiesWithinStrikingDistance(pPlot);

			if (bUnitCanAttack || bCityCanAttack)
			{
				int iExpectedDamage = ComputeTotalExpectedDamage(pTarget, pPlot) + ComputeTotalExpectedCityBombardDamage(pDefender);
				int iRequiredDamage = pDefender->GetCurrHitPoints();
				pTarget->SetAuxIntData(iRequiredDamage);

				//when in doubt attack the unit with fewer neighboring enemies first (can happen especially in naval combat)
				int iTiebreak = 6 - pPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(), NO_DOMAIN) - pPlot->countMatchingAdjacentPlots(NO_DOMAIN,NO_PLAYER,NO_PLAYER,m_pPlayer->GetID());

				targets.push_back(STargetWithTwoScoresTiebreak(pTarget,pPlot,iExpectedDamage-iRequiredDamage,iTiebreak) );
			}
		}
	}

	//we want to attacks the one we can do most damage to first
	std::sort(targets.begin(), targets.end());
	std::reverse(targets.begin(), targets.end());

	for (size_t i=0; i<targets.size(); i++)
	{
		CvUnit* pDefender = targets[i].pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
		if (!pDefender)
			continue;

		//this is still a bit suboptimal, we might move units which are better positioned to attack another target
		//on the other hand, ExecuteAttackWithUnits() considers other targets in the vicinity anyway
		FindUnitsWithinStrikingDistance(targets[i].pPlot);
		FindCitiesWithinStrikingDistance(targets[i].pPlot);

		// Start by applying damage from city bombards
		for(unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
		{
			CvCity* pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
			if(!pCity)
				continue;

			pCity->doTask(TASK_RANGED_ATTACK, pDefender->getX(), pDefender->getY(), 0);

			if (pDefender->GetCurrHitPoints() < 1)
				break;
		}

		if (pDefender->GetCurrHitPoints() < 1)
			continue;

		//special handling for ranged garrison as it is ignored by FindUnitsWithinStrikingDistance()
		//note: melee garrison and advanced logic is handled in PlotGarrisonMoves()
		for(std::list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
			if(pUnit && pUnit->IsGarrisoned() && pUnit->canRangeStrikeAt(pDefender->getX(), pDefender->getY()))
			{
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pDefender->getX(), pDefender->getY());

				//this invalidates our iterator so exit the loop immediately
				UnitProcessed(pUnit->GetID());
				break;
			}
		}

		if (pDefender->GetCurrHitPoints() < 1)
			continue;

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

			//mark the target no matter if the attack succeeds
			targets[i].pTarget->SetLastAggLvl(aggLvl);

			ExecuteAttackWithUnits(targets[i].pPlot, aggLvl);
		}
		// Do we have enough firepower to destroy it?
		else
		{
			if(targets[i].score1>0) //score1 is the overkill number
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

				//mark the target no matter if the attack succeeds
				targets[i].pTarget->SetLastAggLvl(aggLvl);

				ExecuteAttackWithUnits(targets[i].pPlot, aggLvl);
			}
		}
	}
}

/// Moved endangered units to safe hexes
void CvTacticalAI::PlotMovesToSafety(bool bCombatUnits)
{
	ClearCurrentMoveUnits(AI_TACTICAL_SAFETY);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && pUnit->canUseForTacticalAI())
		{
			// Danger value of plot must be greater than 0
			int iDangerLevel = pUnit->GetDanger();
			if(iDangerLevel > 0)
			{
				bool bAddUnit = false;
				if(bCombatUnits)
				{
					if (!pUnit->IsCombatUnit() || pUnit->IsGarrisoned() || pUnit->getArmyID() != -1)
						continue;

					if(pUnit->isBarbarian())
					{
						// Barbarian combat units - only naval units flee (but they flee if have taken ANY damage)
						if(pUnit->getDomainType() == DOMAIN_SEA && pUnit->getDamage()>0)
						{
							bAddUnit = true;
						}
					}
					//if danger is quite high or unit is already damaged
					else if(iDangerLevel>pUnit->GetMaxHitPoints()/2 || (pUnit->getDamage()*2)>pUnit->GetMaxHitPoints())
					{
						bAddUnit = true;
					}
					else if(pUnit->isProjectedToDieNextTurn())
					{
						bAddUnit = true;
					}
				}
				else
				{
					// Civilian (or embarked) units always flee from danger
					if(!pUnit->IsCanDefend())
					{
						bAddUnit = true;
					}
#if defined(MOD_BALANCE_CORE)
					//GGs and GAs need to stay in the DANGER ZONE, but only if there are units near it to support it.
					int iUnits = 0;
					if (pUnit->IsGreatAdmiral() || pUnit->IsGreatGeneral())
					{
						//can't use garrison check here. while non-combat units may be in a city, they are not the garrison
						if(pUnit->plot()->isCity())
						{
							bAddUnit = false;
						}
						else if(pUnit->plot()->getNumDefenders(pUnit->getOwner()) > 0)
						{
							CvUnit* pUnit2 = pUnit->plot()->getUnitByIndex(0);
							//Are we under a relatively healthy unit? Alright, let's drill down and see what's around us.
							if(pUnit2 != NULL && pUnit2->GetCurrHitPoints() > (pUnit2->GetMaxHitPoints() / 2))
							{
								// If there a hex adjacent to city they can airlift to?
								for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
								{
									CvPlot *pLoopPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));
									if (pLoopPlot != NULL)
									{
										if(pLoopPlot->getNumDefenders(pUnit->getOwner()) > 0)
										{
											iUnits++;
										}
									}
								}
							}
						}
					}
					//Two or more units nearby or we just got here? We're staying in the DANGER ZONE.
					if(iUnits > 1 || pUnit->IsRecentlyDeployedFromOperation())
					{
						bAddUnit = false;
					}
#endif
				}

				if(bAddUnit)
					m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
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
	ClearCurrentMoveUnits(AI_TACTICAL_REPOSITION);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->canUseForTacticalAI()) //important, don't move units which have been processed already even if they have movement left!
		{
			// Never use this (default) move for Great Admirals or Generals
			if (pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral() || pUnit->IsCityAttackSupport())
				continue;

			m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteRepositionMoves();
	}
}

/// Move barbarians across the map
void CvTacticalAI::PlotBarbarianRoaming()
{
	if (!m_pPlayer->isBarbarian())
		return;

	ClearCurrentMoveUnits(AI_TACTICAL_BARBARIAN_ROAM);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->canUseForTacticalAI())
			m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
	}

	if(m_CurrentMoveUnits.size() > 0)
		ExecuteBarbarianRoaming();
}

//attack military units and civilians without regard for tactical zones
void CvTacticalAI::PlotBarbarianAttacks()
{
	//the Execute* functions are generic, need to set the current tactical move before calling them
	ClearCurrentMoveUnits(AI_TACTICAL_BARBARIAN_HUNT);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, true);
}

/// Plunder trade routes
void CvTacticalAI::PlotPlunderTradeUnitMoves(DomainTypes eDomain)
{
	ClearCurrentMoveUnits(AI_TACTICAL_PLUNDER);

	AITacticalTargetType eTargetType = (eDomain == DOMAIN_LAND) ? AI_TACTICAL_TARGET_TRADE_UNIT_LAND : AI_TACTICAL_TARGET_TRADE_UNIT_SEA;

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(eTargetType); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		if (FindUnitsForHarassing(pPlot,1,GC.getMAX_HIT_POINTS()/2,-1,eDomain,false))
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
	}
}

/// Process units that we recruited out of operational moves.
void CvTacticalAI::PlotOperationalArmyMoves()
{
	//just so that UnitProcessed() sets the right flags
	ClearCurrentMoveUnits(AI_TACTICAL_OPERATION);

	// move all units in operations
	std::vector<int> opsToKill;
	CvAIOperation* nextOp = m_pPlayer->getFirstAIOperation();
	while(nextOp != NULL)
	{
		if (!nextOp->DoTurn())
			opsToKill.push_back(nextOp->GetID());

		nextOp = m_pPlayer->getNextAIOperation();
	}

	//clean up - have to do this in two steps so the iterator does not get invalidated
	for (size_t i=0; i<opsToKill.size(); i++)
		m_pPlayer->getAIOperation(opsToKill[i])->Kill();
}

/// Assigns units to pillage enemy improvements
void CvTacticalAI::PlotPillageMoves(AITacticalTargetType eTarget, bool bImmediate)
{
	ClearCurrentMoveUnits(AI_TACTICAL_PILLAGE);

	int iMinDamage = GC.getPILLAGE_HEAL_AMOUNT();

	CvString szTargetName = "";
	if(GC.getLogging() && GC.getAILogging())
	{
		if (eTarget == AI_TACTICAL_TARGET_CITADEL)
		{
			szTargetName = "Citadel";
			iMinDamage = 0; //also undamaged units may pillage this
		}
		else if (eTarget == AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE)
		{
			szTargetName = "Improved Resource";
			iMinDamage = 0; //also undamaged units may pillage this
		}
		else if (eTarget == AI_TACTICAL_TARGET_IMPROVEMENT)
		{
			szTargetName = "Improvement";
		}
	}

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(eTarget); pTarget != NULL; pTarget = GetNextZoneTarget())
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pPlot);
		if (pZone)
		{
			if (pZone->IsNavalInvasion() || pZone->IsWater())
				iMinDamage = 0;
			else if (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
				iMinDamage /= 2;
		}

		// Don't do it if an enemy unit became visible in the meantime
		if (pPlot->getVisibleEnemyDefender(m_pPlayer->GetID()) != NULL)
			continue;
		if (eTarget != AI_TACTICAL_TARGET_CITADEL)
		{
			CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				if (aNeighbors[iI] && aNeighbors[iI]->isEnemyUnit(m_pPlayer->GetID(), true, true) && aNeighbors[iI]->getDomain()==pPlot->getDomain())
					continue;
		}

		if (bImmediate)
		{
			// try paratroopers first, not because they are more effective, just because it looks cooler...
			if (FindParatroopersWithinStrikingDistance(pPlot,true))
			{
				// Queue best one up to capture it
				ExecuteParadropPillage(pPlot);

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Paratrooping in to pillage %s, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
					LogTacticalMessage(strLogString);
				}

			}
			else if (FindUnitsForHarassing(pPlot, 0, GC.getMAX_HIT_POINTS() / 3, GC.getMAX_HIT_POINTS() - iMinDamage, DOMAIN_LAND, true))
			{
				if (ExecutePillage(pPlot))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Pillaging %s, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
						LogTacticalMessage(strLogString);
					}
				}
			}
		}
		else if (FindUnitsForHarassing(pPlot, 2, GC.getMAX_HIT_POINTS() / 2, GC.getMAX_HIT_POINTS() - iMinDamage, DOMAIN_LAND, false))
		{
			//be careful when sending out single units ...
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pPlot);
			if (!pZone || pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
				continue;

			CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;
			if (pUnit && pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
			{
				ExecuteMoveToPlot(pUnit, pPlot, false, CvUnit::MOVEFLAG_NO_EMBARK | CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER);

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving toward %s for pillage, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
					LogTacticalMessage(strLogString);
				}

				UnitProcessed(pUnit->GetID());
			}
		}
	}
}

/// Move barbarian ships to disrupt usage of water improvements
void CvTacticalAI::PlotBlockadeMoves()
{
	ClearCurrentMoveUnits(AI_TACTICAL_BLOCKADE);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BLOCKADE_POINT); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsForHarassing(pPlot, 4, GC.getMAX_HIT_POINTS()/2, -1, DOMAIN_SEA, false))
		{
			// Queue best one up to capture it
			ExecuteNavalBlockadeMove(pPlot);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving into enemy territory for a naval blockade with move to, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
	}
}

void CvTacticalAI::PlotCivilianAttackMoves()
{
	ClearCurrentMoveUnits(AI_TACTICAL_CAPTURE);
	ExecuteCivilianAttackMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
	ExecuteCivilianAttackMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
	ExecuteCivilianAttackMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
	ExecuteCivilianAttackMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
}

/// Assigns units to capture undefended civilians
void CvTacticalAI::ExecuteCivilianAttackMoves(AITacticalTargetType eTargetType)
{
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(eTargetType); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsForHarassing(pPlot,1,GC.getMAX_HIT_POINTS()/2,-1,DOMAIN_LAND,false))
		{
			for (size_t i = 0; i < m_CurrentMoveUnits.size(); i++)
			{
				CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[i].GetID());
				if (!pUnit || !pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_ATTACK))
					continue;
				//don't allow humans to use civilians as bait to lure units out of camps
				if (pUnit->GetDanger() == 0 || pUnit->plot()->getImprovementType()!=(ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY(), CvUnit::MOVEFLAG_NO_EMBARK);

					// Delete this unit from those we have to move
					if (!pUnit->canMove())
						UnitProcessed(m_CurrentMoveUnits[i].GetID());

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

					break;
				}
			}
		}
	}
}

/// Assigns units to heal
void CvTacticalAI::PlotHealMoves(bool bFirstPass)
{
	ClearCurrentMoveUnits(AI_TACTICAL_HEAL);

	//not all units go through ExecuteHeals
	std::set<int> passiveAggressive;

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (!pUnit || !pUnit->canUseForTacticalAI())
			continue;

		if (pUnit->shouldHeal(bFirstPass))
			m_CurrentMoveUnits.push_back( CvTacticalUnit(pUnit->GetID()) );
		else
		{
			//units with area damage if fortified should fortify as much as possible if near enemies
			if (pUnit->GetDamageAoEFortified() > 0 && pUnit->canFortify(pUnit->plot()) &&
				pUnit->getDamage() >= pUnit->healRate(pUnit->plot()) && pUnit->GetDanger() < pUnit->GetCurrHitPoints() &&
				pUnit->plot()->GetNumEnemyUnitsAdjacent(pUnit->getTeam(), pUnit->getDomainType()) > 0)
			{
				passiveAggressive.insert(pUnit->GetID());
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			}
		}
	}

	//can't do this directly, otherwise we invalidate the iterator for m_CurrentTurnUnits
	for (set<int>::iterator it = passiveAggressive.begin(); it != passiveAggressive.end(); ++it)
		UnitProcessed(*it);

	if(m_CurrentMoveUnits.size() > 0)
		ExecuteHeals(bFirstPass);
}

/// Assigns a barbarian to go protect an undefended camp
void CvTacticalAI::PlotBarbarianCampDefense()
{
	ClearCurrentMoveUnits(AI_TACTICAL_BARBARIAN_CAMP);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		//for the barbarian player AI_TACTICAL_TARGET_BARBARIAN_CAMP does not automatically mean the camp is empty of _barbarian_ defenders (check is only for enemy units)
		CvUnit* currentDefender = pPlot->getBestDefender(BARBARIAN_PLAYER);
		if (currentDefender)
		{
			if (currentDefender->CanUpgradeRightNow(true) && !currentDefender->IsHurt())
			{
				CvUnit* pNewUnit = currentDefender->DoUpgrade(true);
				if (pNewUnit)
					UnitProcessed(pNewUnit->GetID());
			}
			else if (currentDefender->isRanged())
			{
				//don't leave camp
				TacticalAIHelpers::PerformOpportunityAttack(currentDefender);
				currentDefender->PushMission(CvTypes::getMISSION_SKIP());
			}
			else
			{
				//melee may leave camp depending on how many enemies are around
				int iStrongEnemyCount = 0;
				int iWeakEnemyCount = 0;
				for (int i = RING0_PLOTS; i < RING3_PLOTS; i++)
				{
					CvPlot* pNeighbor = iterateRingPlots(pPlot, i);
					if (pNeighbor && pNeighbor->isEnemyUnit(m_pPlayer->GetID(), true, true))
					{
						CvUnit* pEnemy = pNeighbor->getBestDefender(NO_PLAYER, m_pPlayer->GetID());
						//if the enemy ignores zoc, he might capture the camp behind our back 
						if (pEnemy->GetBaseCombatStrength() < currentDefender->GetBaseCombatStrength() && !pEnemy->IsIgnoreZOC())
							iWeakEnemyCount++;
						else
							iStrongEnemyCount++;
					}
				}

				TacticalAIHelpers::PerformOpportunityAttack(currentDefender,iWeakEnemyCount<2 && iStrongEnemyCount==0);
				currentDefender->PushMission(CvTypes::getMISSION_SKIP());
			}

			UnitProcessed(currentDefender->GetID());
		}
		else if(FindUnitsForHarassing(pPlot,5,-1,-1,DOMAIN_LAND,false))
		{
			CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;
			ExecuteMoveToPlot(pUnit,pPlot);
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving to protect camp, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
	}
}

/// Make a defensive move to garrison a city
void CvTacticalAI::PlotGarrisonMoves(int iNumTurnsAway)
{
	ClearCurrentMoveUnits(AI_TACTICAL_GARRISON);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY_TO_DEFEND); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvCity* pCity = pPlot->getPlotCity();

		if(!pCity)
			continue;

		//an explorer can be a transient garrison but we don't want to keep it here
		CvUnit* pGarrison = pCity->GetGarrisonedUnit();
		if (pGarrison)
		{
			if (pGarrison->CanUpgradeRightNow(false) && !pGarrison->IsHurt())
			{
				CvUnit* pNewUnit = pGarrison->DoUpgrade();
				if (pNewUnit)
					UnitProcessed(pNewUnit->GetID());
			}

			//sometimes we have an accidental garrison ...
			if (pGarrison->AI_getUnitAIType() == UNITAI_EXPLORE || pGarrison->isDelayedDeath() || pGarrison->TurnProcessed() || pGarrison->getArmyID()!=-1)
				continue;

			//first check how many enemies are around
			int iEnemyCount = 0;
			for (int i = RING0_PLOTS; i < RING3_PLOTS; i++)
			{
				CvPlot* pNeighbor = iterateRingPlots(pPlot, i);
				if (pNeighbor && pNeighbor->isEnemyUnit(m_pPlayer->GetID(), true, true))
					iEnemyCount++;
			}

			//note: ranged garrisons are also used in ExecuteSafeBombards if they can hit a target without moving
			//here we have more advanced logic and allow some movement if it's safe
			TacticalAIHelpers::PerformOpportunityAttack(pGarrison, iEnemyCount < 2);

			//do not call finishMoves() else the garrison will not heal!
			pGarrison->PushMission(CvTypes::getMISSION_SKIP());
			UnitProcessed(pGarrison->GetID());
		}
		else if ( !pCity->isInDangerOfFalling() )
		{
			// Grab units that make sense for this move type
			CvUnit* pUnit = FindUnitForThisMove(AI_TACTICAL_GARRISON, pPlot, iNumTurnsAway);

			if (pUnit && ExecuteMoveToPlot(pUnit, pPlot))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Garrison, X: %d, Y: %d, Priority: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iNumTurnsAway);
					LogTacticalMessage(strLogString);
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("No unit for garrison in %s at (%d:%d)", pCity->getNameNoSpace().c_str(), pTarget->GetTargetX(), pTarget->GetTargetY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}
}

/// Establish a defensive bastion adjacent to a city
void CvTacticalAI::PlotBastionMoves(int iNumTurnsAway)
{
	ClearCurrentMoveUnits(AI_TACTICAL_GUARD);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_DEFENSIVE_BASTION); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		// Grab units that make sense for this move type
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvUnit* pUnit = FindUnitForThisMove(AI_TACTICAL_GUARD, pPlot, iNumTurnsAway);

		//move may fail if the plot is already occupied (can happen if another unit moved there during this turn)
		if (pUnit && ExecuteMoveToPlot(pUnit, pPlot))
		{
			if (pUnit->CanUpgradeRightNow(false) && !pUnit->IsHurt())
			{
				CvUnit* pNewUnit = pUnit->DoUpgrade();
				if (pNewUnit)
					UnitProcessed(pNewUnit->GetID());
			}
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Bastion, X: %d, Y: %d, Priority: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
	}
}

/// Make a defensive move to guard an improvement
void CvTacticalAI::PlotGuardImprovementMoves(int iNumTurnsAway)
{
	ClearCurrentMoveUnits(AI_TACTICAL_GUARD);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		// Grab units that make sense for this move type
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvUnit* pUnit = FindUnitForThisMove(AI_TACTICAL_GUARD, pPlot, iNumTurnsAway);

		//move may fail if the plot is already occupied (can happen if another unit moved there during this turn)
		if (pUnit && ExecuteMoveToPlot(pUnit, pPlot))
		{
			if (pUnit->CanUpgradeRightNow(false) && !pUnit->IsHurt())
			{
				CvUnit* pNewUnit = pUnit->DoUpgrade();
				if (pNewUnit)
					UnitProcessed(pNewUnit->GetID());
			}
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Guard Improvement, X: %d, Y: %d, Turns Away: %d", pTarget->GetTargetX(), pTarget->GetTargetY(), iNumTurnsAway);
				LogTacticalMessage(strLogString);
			}
		}
	}
}

/// Set fighters to intercept
void CvTacticalAI::PlotAirInterceptMoves()
{
	ClearCurrentMoveUnits(AI_TACTICAL_AIRPATROL);

	std::vector<CvPlot*> checkedPlotList;

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->getDomainType()==DOMAIN_AIR && pUnit->canUseForTacticalAI())
		{
			// Am I eligible to intercept? We only commandeered units which won't be rebased
			if(pUnit->canAirPatrol(NULL))
			{
				CvPlot* pUnitPlot = pUnit->plot();
				int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), false/*bCountFighters*/, true/*bCountBombers*/);
				int iNumNearbyFighters = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), true/*bCountFighters*/, false/*bCountBombers*/);
				int iNumPlotNumAlreadySet = std::count(checkedPlotList.begin(), checkedPlotList.end(), pUnitPlot);

				// To at least intercept once if only one bomber found.
				if (iNumNearbyBombers == 1)
					iNumNearbyBombers++;

				// TODO: we should not just use any interceptor but the best one (depending on promotions etc)
				int maxInterceptorsWanted = (iNumNearbyBombers / 2) + (iNumNearbyFighters / 4);
				if (iNumPlotNumAlreadySet < maxInterceptorsWanted)
				{
					checkedPlotList.push_back(pUnitPlot);

					m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
	
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Ready to intercept enemy air units at, X: %d, Y: %d with %s %d", pUnit->getX(), pUnit->getY(), pUnit->getName().c_str(), pUnit->GetID());
						LogTacticalMessage(strLogString);
					}
				}
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
	ClearCurrentMoveUnits(AI_TACTICAL_AIRSWEEP);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
#if defined(MOD_UNITS_MAX_HP)
		if(pUnit && (pUnit->getDamage() * 2) < pUnit->GetMaxHitPoints() && pUnit->getDomainType() == DOMAIN_AIR && pUnit->canUseForTacticalAI())
#else
		if(pUnit && (pUnit->getDamage() * 2) < GC.getMAX_HIT_POINTS())
#endif
		{
			// Am I eligible to air sweep and have a target?
			// we have only units here which won't be rebased
			if(pUnit->canAirSweep() && m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit) != NULL)
			{
				CvPlot* pUnitPlot = pUnit->plot();
				CvCity* pCity = pUnitPlot->getPlotCity();
				CvTacticalDominanceZone* pZone = NULL;

				// On a carrier or in a city where we are dominant?
				if (pCity)
					pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, false);

				if (!pCity || !pZone || pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				{
					m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Ready to air sweep enemy air units at, X: %d, Y: %d with %s %d", pUnit->getX(), pUnit->getY(), pUnit->getName().c_str(), pUnit->GetID());
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
void CvTacticalAI::PlotEmergencyPurchases(CvTacticalDominanceZone* pZone)
{
	if(m_pPlayer->isMinorCiv())
		return;
	if (!pZone)
		return;

	CvCity* pCity = pZone->GetZoneCity();

	// Only in our own cities
	if (!pCity || pCity->getOwner() != m_pPlayer->GetID())
		return;

	// Done waste money if there's no hope
	if (pCity->isInDangerOfFalling())
		return;

	// If we need additional units - ignore the supply limit here, we're probably losing units anyway
	if(pZone->GetOverallDominanceFlag()>TACTICAL_DOMINANCE_FRIENDLY)
	{
		if(!MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
			m_pPlayer->GetMilitaryAI()->BuyEmergencyBuilding(pCity);

		if(!MOD_BALANCE_CORE_UNIT_INVESTMENTS)
		{
			//only buy ranged if there's no garrison
			//otherwise it will be placed outside of the city and most probably die instantly
			if (!pCity->HasGarrison())
				m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_RANGED, pCity);

			//in water zones buy naval melee
			if (pZone->IsWater() && pCity->isCoastal())
				m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_ATTACK_SEA, pCity);
			else
				//otherwise buy defensive land units
				m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(GC.getGame().getSmallFakeRandNum(5,*pCity->plot())<2?UNITAI_COUNTER:UNITAI_DEFENSE, pCity);
		}
	}
}

/// Spend money to buy defenses
void CvTacticalAI::PlotDefensiveAirlifts(CvTacticalDominanceZone* pZone)
{
	if(m_pPlayer->isMinorCiv())
		return;

	if (!pZone)
		return;

	ClearCurrentMoveUnits(AI_TACTICAL_AIRLIFT);

	CvCity* pCity = pZone->GetZoneCity();
	if (pCity && pCity->getOwner() == m_pPlayer->GetID() && pCity->CanAirlift())
	{
		// Loop through all recruited units
		for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
			if (pUnit && pUnit->canUseForTacticalAI())
			{
				// If there a hex adjacent to city they can airlift to?
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot *pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));
					if (pLoopPlot != NULL && pUnit->canAirliftAt(pUnit->plot(), pLoopPlot->getX(), pLoopPlot->getY()))
					{
						CvTacticalUnit unit(pUnit->GetID());
						//evil hack ...
						unit.SetAttackStrength( pLoopPlot->GetPlotIndex() );
						m_CurrentMoveUnits.push_back( unit );
					}
				}
			}
		}
	}

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Airlifting %s to city of %s, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pCity->getNameNoSpace().GetCString(), pUnit->getX(), pUnit->getY());
				LogTacticalMessage(strLogString);
			}

			CvPlot* pTarget = GC.getMap().plotByIndexUnchecked( m_CurrentMoveUnits[iI].GetAttackStrength() );
			pUnit->airlift(pTarget->getX(), pTarget->getY());
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Move naval units over top of unprotected embarked units
void CvTacticalAI::PlotEscortEmbarkedMoves()
{
	ClearCurrentMoveUnits(AI_TACTICAL_ESCORT);

	std::vector<CvUnit*> vEmbarkedUnits;
	int iLoop = 0;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit->isEmbarked() && pLoopUnit->getDomainType() != DOMAIN_SEA)
			vEmbarkedUnits.push_back(pLoopUnit);
	}

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->canUseForTacticalAI())
		{
			// Am I a naval combat unit?
			if(pUnit->getDomainType() == DOMAIN_SEA && pUnit->IsCombatUnit())
			{
				//any embarked unit close by?
				int iMaxDist = pUnit->baseMoves()*2;
				for (size_t i=0; i<vEmbarkedUnits.size(); i++)
				{
					if (plotDistance(*pUnit->plot(),*vEmbarkedUnits[i]->plot())<=iMaxDist)
					{
						m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
						break;
					}
				}
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
void CvTacticalAI::PlotAttritFromRangeMoves(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_ATTRITION);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteSafeBombards(*pTarget);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteSafeBombards(*pTarget);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteSafeBombards(*pTarget);

	//if there is a city, don' t forget to bombard it as well
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteSafeBombards(*pTarget);
}

/// Defeat enemy units by using our advantage in numbers
void CvTacticalAI::PlotExploitFlanksMoves(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_FLANKATTACK);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteFlankAttack(*pTarget);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteFlankAttack(*pTarget);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteFlankAttack(*pTarget);

	//if there is a city, don' t forget to bombard it as well
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteFlankAttack(*pTarget);
}

/// We have more overall strength than enemy, defeat his army first
void CvTacticalAI::PlotSteamrollMoves(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_STEAMROLL);

	// See if there are any kill attacks we can make.
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	// We have superiority, so let's attack high prio targets even with bad odds
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);

	// Now go after the city
	ExecuteCaptureCityMoves();
}

/// We should be strong enough to take out the city before the enemy can whittle us down with ranged attacks
void CvTacticalAI::PlotSurgicalCityStrikeMoves(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_SURGICAL_STRIKE);

	// Attack the city first
	ExecuteCaptureCityMoves();

	// Take any other really good attacks we've set up
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
}

/// Build a defensive shell around this city
void CvTacticalAI::PlotHedgehogMoves(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_HEDGEHOG);

	// Attack priority unit targets
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
}

/// Try to push back the invader
void CvTacticalAI::PlotCounterattackMoves(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_COUNTERATTACK);

	// Attack priority unit targets
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	// Now targets we can't destroy
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);
}

/// Withdraw out of current dominance zone
void CvTacticalAI::PlotWithdrawMoves(CvTacticalDominanceZone* pZone)
{
	if (!pZone)
		return;

	ClearCurrentMoveUnits(AI_TACTICAL_WITHDRAW);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->canUseForTacticalAI())
		{
			//Recent, healthy deployments need to attack!
			if (pUnit->IsRecentlyDeployedFromOperation() && (pUnit->GetCurrHitPoints() > pUnit->GetMaxHitPoints()/2))
				continue;

			//Army men don't retreat!
			if (pUnit->getArmyID() != -1)
				continue;

			//special moves for support units
			if (pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
				continue;

			// Am I in the current dominance zone?
			// Units in other dominance zones need to fend for themselves, depending on their own posture
			CvTacticalDominanceZone* pUnitZone = GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot());
			if (pUnitZone == pZone)
				m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWithdrawMoves();
	}
}

/// Close units in on primary target of this dominance zone
void CvTacticalAI::PlotReinforcementMoves(CvTacticalDominanceZone* pTargetZone)
{
	ClearCurrentMoveUnits(AI_TACTICAL_CLOSE_ON_TARGET);

	if (!pTargetZone)
		return;

	//sometimes there is nothing to do ...
	if (pTargetZone->GetTerritoryType() != TACTICAL_TERRITORY_ENEMY && pTargetZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
		return;

	int iAlreadyInZone = 0;
	CvPlot* pTargetPlot = GC.getMap().plot(pTargetZone->GetCenterX(), pTargetZone->GetCenterY());
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && pUnit->canUseForTacticalAI())
		{
			// Proper domain of unit?
			if ((pTargetZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA) ||
				(!pTargetZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND) ||
				(pTargetZone->IsNavalInvasion()) )
			{	
				// we want units which are somewhat close (so we don't deplete other combat zones) 
				int iMaxTurns = GetTacticalAnalysisMap()->GetTacticalRangeTurns();

				// rough distance check first
				if (plotDistance(*pUnit->plot(), *pTargetPlot) > iMaxTurns * 2)
					continue;

				//don't run away if there's other work to do (will eventually be handled by ExecuteAttackWithUnits)
				CvPlot* pAlternativeTarget = TacticalAIHelpers::GetFirstTargetInRange(pUnit, false, false);
				if (pAlternativeTarget && plotDistance(*pAlternativeTarget,*pTargetPlot)>TACTICAL_COMBAT_MAX_TARGET_DISTANCE)
					continue;

				//finally detailed pathfinding
				int iTurns = 0;
				if (pUnit->GeneratePath(pTargetPlot, CvUnit::MOVEFLAG_APPROX_TARGET_RING2|CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN, iMaxTurns, &iTurns, true))
				{
					CvPlot* pEndTurnPlot = pUnit->GetPathEndFirstTurnPlot();
					if (pEndTurnPlot && pUnit->GetDanger(pEndTurnPlot) < pUnit->GetCurrHitPoints())
					{
						CvTacticalUnit unit(pUnit->GetID());
						unit.SetMovesToTarget(iTurns);
						m_CurrentMoveUnits.push_back(unit);

						if (m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot()) == pTargetZone)
							iAlreadyInZone++;
					}
				}
			}
		}
	}

	//if we only have a single unit to work with in total, this is a case for pillage moves or the like
	if (pTargetZone->GetTotalFriendlyUnitCount() + m_CurrentMoveUnits.size() - iAlreadyInZone < 2)
		return;

	if (m_CurrentMoveUnits.size() > 0)
	{
		vector<CvUnit*> vUnits;
		for (size_t i = 0; i < m_CurrentMoveUnits.size(); i++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit( m_CurrentMoveUnits[i].GetID() );
			if (!pUnit->isDelayedDeath() && pUnit->canMove())
				vUnits.push_back(pUnit);
		}

		PositionUnitsAroundTarget(vUnits,pTargetPlot);
	}
}

/// Log that we couldn't find assignments for some units
void CvTacticalAI::ReviewUnassignedUnits()
{
	// Loop through all remaining units.
	// Do not call UnitProcessed() from here as it may invalidate our iterator
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->canUseForTacticalAI())
		{
			//don't overwrite army moves ... everything else is fair game
			if (pUnit->getArmyID()==-1)
				pUnit->setTacticalMove(AI_TACTICAL_UNASSIGNED);

			//there shouldn't be any danger but just in case
			CvPlot* pSafePlot = pUnit->GetDanger()>0 ? TacticalAIHelpers::FindSafestPlotInReach(pUnit, true) : NULL;
			if (pSafePlot)
			{
				if (pUnit->CanUpgradeRightNow(false) && !pUnit->IsHurt())
				{
					CvUnit* pNewUnit = pUnit->DoUpgrade();
					if (pNewUnit)
						pNewUnit->SetTurnProcessed(true);
				}
				else
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pSafePlot->getX(), pSafePlot->getY());

				if (!pUnit->canMove())
					pUnit->SetTurnProcessed(true);
			}

			//do not skip the turn, finish moves, set the turn processed flag for units which we didn't move
			//homeland AI will take care of them!
		}
	}
}

// OPERATIONAL AI SUPPORT FUNCTIONS

CvUnit* SwitchEscort(CvUnit* pCivilian, CvPlot* pNewEscortPlot, CvUnit* pEscort, CvArmyAI* pThisArmy)
{
	CvUnit* pPlotDefender = pNewEscortPlot->getBestDefender(pCivilian->getOwner());

	//Maybe we just make this guy our new escort, eh?
	if(pPlotDefender && pPlotDefender->getArmyID() == -1 && pPlotDefender->getDomainType() == pCivilian->getDomainType())
	{
		pThisArmy->RemoveUnit(pEscort->GetID());
		pThisArmy->AddUnit(pPlotDefender->GetID(), 1);
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("SingleHexOperationMoves: Switched escort to get things going.");
			GET_PLAYER(pCivilian->getOwner()).GetTacticalAI()->LogTacticalMessage(strLogString);
		}

		return pPlotDefender;
	}

	return NULL;
}

/// Move a single stack (civilian plus escort) to its destination
void CvTacticalAI::PlotArmyMovesEscort(CvArmyAI* pThisArmy)
{
	if (!pThisArmy)
		return;

	CvAIOperation* pOperation = GET_PLAYER(pThisArmy->GetOwner()).getAIOperation(pThisArmy->GetOperationID());
	if (!pOperation)
		return;

	//the unit to be escorted is always the first one
	CvUnit* pCivilian = pThisArmy->GetFirstUnit();
	//the second unit would be the first escort
	CvUnit* pEscort = pThisArmy->GetNextUnit(pCivilian);
	//additional escorts
	std::vector<CvUnit*> vExtraEscorts;
	CvUnit* pExtraEscort = pThisArmy->GetNextUnit(pEscort);
	while (pExtraEscort)
	{
		vExtraEscorts.push_back(pExtraEscort);
		pExtraEscort = pThisArmy->GetNextUnit(pExtraEscort); 
	}

	// No civilian? that's a problem
	if(!pCivilian || !pCivilian->IsCivilianUnit())
	{
		return;
	}

	// ESCORT AND CIVILIAN MEETING UP
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE || 
		pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		pThisArmy->SetXY(pCivilian->getX(), pCivilian->getY());

		// Check to make sure escort can get to civilian
		if(pOperation->GetMusterPlot() != NULL)
		{
			//check if the civilian is in danger
			if ( pCivilian->GetDanger() > 0 )
			{
				//try to move to safety
				CvPlot* pBetterPlot = TacticalAIHelpers::FindSafestPlotInReach(pCivilian,true);
				if (pBetterPlot)
				{
					ExecuteMoveToPlot(pCivilian,pBetterPlot);
					return;
				}
			}

			//civilian and escort have not yet met up
			if(pEscort)
			{
				//civilian is already there
				if(pCivilian->plot() == pOperation->GetMusterPlot())
				{
					//another military unit is blocking our escort ... find another muster plot
					if(pCivilian->plot()->GetNumCombatUnits() > 0)
					{
						CvUnit* pNewEscort = SwitchEscort(pCivilian,pCivilian->plot(),pEscort,pThisArmy);
						if (pNewEscort)
							pOperation->CheckTransitionToNextStage();
						else //did not switch
						{
							//Let's have them move forward, see if that clears things up.
							ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(),false,CvUnit::MOVEFLAG_APPROX_TARGET_RING1|CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER);
							ExecuteMoveToPlot(pEscort, pOperation->GetTargetPlot(),false,CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

							//try again next turn
							pOperation->SetMusterPlot(pCivilian->plot());

							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("SingleHexOperationMoves: Forced movement to get things going.");
								LogTacticalMessage(strLogString);
							}
						}
					}
					else
					{
						//move escort towards civilian
						if (!ExecuteMoveToPlot(pEscort, pCivilian->plot()))
						{
							//d'oh. escort cannot reach us
							CvUnit* pNewEscort = SwitchEscort(pCivilian,pCivilian->plot(),pEscort,pThisArmy);

							if (pEscort==pNewEscort)
								pOperation->SetToAbort(AI_ABORT_LOST_PATH);

							return;	
						}

						UnitProcessed(pCivilian->GetID());
					}
				}
				else
				{
					// Civilian is not yet there - both must move
					if (!ExecuteMoveToPlot(pCivilian, pOperation->GetMusterPlot(),false))
						pOperation->SetToAbort(AI_ABORT_LOST_PATH);
					// use approximate pathfinding to avoid embarked stacking problems - but only for one of them, else they will never come close
					else if (!ExecuteMoveToPlot(pEscort, pOperation->GetMusterPlot(),false,CvUnit::MOVEFLAG_APPROX_TARGET_RING1))
						pOperation->SetToAbort(AI_ABORT_LOST_PATH);
				}

				if(pOperation->GetOperationState()!=AI_ABORT_LOST_PATH && GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp;
					CvString strLogString;
					strTemp = GC.getUnitInfo(pEscort->getUnitType())->GetDescription();
					strLogString.Format("Moving escorting %s to civilian for operation, Civilian X: %d, Civilian Y: %d, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY(), pEscort->getX(), pEscort->getY());
					LogTacticalMessage(strLogString);
				}
			}
			else
			{
				//no escort
				if (pCivilian->plot() == pOperation->GetMusterPlot())
					pOperation->CheckTransitionToNextStage();
				else if (pCivilian->GetDanger(pOperation->GetMusterPlot())<INT_MAX)
					//continue moving. if this should fail, we just freeze and wait for better times
					ExecuteMoveToPlot(pCivilian,pOperation->GetMusterPlot());
			}
		}
	}

	// MOVING TO TARGET ... or really close
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION ||
		pThisArmy->GetArmyAIState() == ARMYAISTATE_AT_DESTINATION)
	{
		// are we there yet?
		if(pCivilian->plot() == pOperation->GetTargetPlot())
			return;

		int iMoveFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY;
		//if necessary and possible, avoid plots where our escort cannot follow
		if (pEscort && !pOperation->GetTargetPlot()->isNeutralUnit(pEscort->getOwner(),true,true))
			iMoveFlags |= CvUnit::MOVEFLAG_DONT_STACK_WITH_NEUTRAL;

		pThisArmy->SetXY(pCivilian->getX(), pCivilian->getY());
	
		// the escort leads the way
		bool bHavePathEscort = false;
		bool bPathFound = false;
		bool bSaveMoves = true;
		CvString strLogString;
		if(pEscort)
		{
			bHavePathEscort = pEscort->GeneratePath(pOperation->GetTargetPlot(), iMoveFlags, INT_MAX, NULL, true);
			if(bHavePathEscort)
			{
				CvPlot* pCommonPlot = pEscort->GetPathEndFirstTurnPlot();
				//need to check if civilian can enter because of unrevealed tiles in path
				if(pCommonPlot != NULL && pCivilian->canMoveInto(*pCommonPlot,CvUnit::MOVEFLAG_DESTINATION))
				{
					int iTurns = INT_MAX;
					bool bHavePathCivilian = pCivilian->GeneratePath(pCommonPlot, iMoveFlags, 5, &iTurns, true);
					if (bHavePathCivilian)
					{
						bSaveMoves = (pCommonPlot == pOperation->GetTargetPlot());
						bPathFound = true;

						if (iTurns > 1)
						{
							//strange, escort seems to be faster than the civilian, let's hope it's better the other way around
							CvPlot* pAltPlot = pCivilian->GetPathEndFirstTurnPlot();
							if (pAltPlot != NULL)
							{
								if (pEscort->canMoveInto(*pAltPlot, CvUnit::MOVEFLAG_DESTINATION))
									pCommonPlot = pAltPlot;
								else
								{
									pAltPlot = pCivilian->GetPathFirstPlot();
									if (pEscort->canMoveInto(*pAltPlot, CvUnit::MOVEFLAG_DESTINATION))
										pCommonPlot = pAltPlot;
								}
							}
						}

						//civilian goes first - we know by now that this must work
						ExecuteMoveToPlot(pCivilian, pCommonPlot, bSaveMoves);

						//watch out: in CP embarked units cannot stack ...
						if (pEscort->canMoveInto(*pCivilian->plot(), CvUnit::MOVEFLAG_DESTINATION))
						{
							ExecuteMoveToPlot(pEscort, pCivilian->plot(), bSaveMoves);
						}
						else
						{
							if (pCivilian->plot()->isAdjacent(pEscort->plot()))
								//approximate move wouldn't make any progress - go straight to the target
								ExecuteMoveToPlot(pEscort, pOperation->GetTargetPlot(), bSaveMoves, CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
							else
								//escort tries to stay close
								ExecuteMoveToPlot(pEscort, pCivilian->plot(), bSaveMoves, CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
						}

						if (GC.getLogging() && GC.getAILogging())
						{
							strLogString.Format("%s now at (%d,%d). Moving towards (%d,%d) with escort %s. escort leading.",
								pCivilian->getName().c_str(), pCivilian->getX(), pCivilian->getY(),
								pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY(), pEscort->getName().c_str());
						}
					}
				}
			}
			else
			{
				bool bHavePathCivilian = pCivilian->GeneratePath(pOperation->GetTargetPlot(), iMoveFlags, INT_MAX, NULL, true);
				if(bHavePathCivilian)
				{
					CvPlot* pCommonPlot = pCivilian->GetPathEndFirstTurnPlot();
					if(pCommonPlot != NULL)
					{
						int iTurns = INT_MAX;
						bool bHavePathEscort = pEscort->GeneratePath(pCommonPlot, iMoveFlags, 5, &iTurns);
						if (bHavePathEscort)
						{
							bSaveMoves = (pCommonPlot == pOperation->GetTargetPlot());
							bPathFound = true;

							if (iTurns>1)
								//strange, civilian seems to be faster than the civilian, let's hope it's better the other way around
								pCommonPlot = pEscort->GetPathEndFirstTurnPlot();

							ExecuteMoveToPlot(pEscort, pCommonPlot, bSaveMoves);
							//Did he actually move?
							if (pEscort->plot() != pCivilian->plot())
							{
								//avoid problems with embarked stacking
								if ( pEscort->plot()->needsEmbarkation(pCivilian) )
								{
									if ( pEscort->plot()->isAdjacent( pCivilian->plot() ) )
										//approximate move wouldn't make any progress - go straight to the target
										ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(), bSaveMoves);
									else
										//simply stay close
										ExecuteMoveToPlot(pCivilian, pEscort->plot(), bSaveMoves, CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								}
								else
									ExecuteMoveToPlot(pCivilian, pEscort->plot(), bSaveMoves);

								if (GC.getLogging() && GC.getAILogging())
								{
									strLogString.Format("%s now at (%d,%d). Moving towards (%d,%d) with escort %s. Civilian leading.",
										pCivilian->getName().c_str(), pCivilian->getX(), pCivilian->getY(),
										pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY(), pEscort->getName().c_str());
								}
							}
							else
							{
								bPathFound = false;
							}
						}
						else
						{
							//our escort can't move into the next path plot. maybe it's blocked by a friendly unit?
							CvUnit* pNewEscort = SwitchEscort(pCivilian,pCommonPlot,pEscort,pThisArmy);
							if (pNewEscort)
							{
								ExecuteMoveToPlot(pCivilian, pCommonPlot, bSaveMoves);
								pNewEscort->PushMission(CvTypes::getMISSION_SKIP());
								UnitProcessed(pNewEscort->GetID());
							}
						}
					}
				}
			}
			
			if(!bPathFound)
			{
				//we have a problem, apparently civilian and escort must split up
				//use a special flag here to make sure we're not stuck in a dead end with limited sight (can happen with embarked units)
				if (!ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(), false, CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED))
				{
					pOperation->SetToAbort(AI_ABORT_LOST_PATH);
					strLogString.Format("%s stuck at (%d,%d), cannot find safe path to target. aborting.", 
						pCivilian->getName().c_str(), pCivilian->getX(), pCivilian->getY() );
				}

				//try to stay close
				MoveToEmptySpaceNearTarget(pEscort, pCivilian->plot(), pCivilian->plot()->getDomain(), 12);
				strLogString.Format("%s at (%d,%d) separated from escort %s at (%d,%d)", 
					pCivilian->getName().c_str(), pCivilian->getX(), pCivilian->getY(), 
					pEscort->getName().c_str(), pEscort->getX(), pEscort->getY() );
			}
		}
		else //no escort
		{
			bool bHavePathCivilian = pCivilian->GeneratePath(pOperation->GetTargetPlot(), iMoveFlags, INT_MAX, NULL, true);
			if(bHavePathCivilian)
			{
				CvPlot* pTurnTarget = pCivilian->GetPathEndFirstTurnPlot();
				if(pTurnTarget != NULL)
				{
					if (pCivilian->GetDanger(pTurnTarget) == INT_MAX)
					{
						CvPlot* pAlternativeTarget = TacticalAIHelpers::FindSafestPlotInReach(pCivilian, true, false, true);
						if (pAlternativeTarget)
							pTurnTarget = pAlternativeTarget;
					}

					bSaveMoves = (pTurnTarget == pOperation->GetTargetPlot());
					ExecuteMoveToPlot(pCivilian, pTurnTarget, bSaveMoves);
					if(GC.getLogging() && GC.getAILogging())
					{
						strLogString.Format("%s now at (%d,%d). Moving normally towards (%d,%d) without escort.",  pCivilian->getName().c_str(), pCivilian->getX(), pCivilian->getY(), pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY() );
					}
				}
			}
			else
			{
				if (MoveToEmptySpaceNearTarget(pCivilian, pOperation->GetTargetPlot(), DOMAIN_LAND, INT_MAX))
				{
					if(GC.getLogging() && GC.getAILogging())
						strLogString.Format("%s now at (%d,%d). Moving to empty space near target (%d,%d) without escort.",  pCivilian->getName().c_str(), pCivilian->getX(), pCivilian->getY(), pOperation->GetTargetPlot()->getX(), pOperation->GetTargetPlot()->getY() );
				}
				else
				{
					pOperation->SetToAbort(AI_ABORT_LOST_PATH);
					if(GC.getLogging() && GC.getAILogging())
						strLogString.Format("%s at (%d,%d). Aborted operation. No path to target for civilian.",  pCivilian->getName().c_str(), pCivilian->getX(), pCivilian->getY() );
				}
			}
		}

		// now we're done
		UnitProcessed(pCivilian->GetID());
		if (pEscort)
			UnitProcessed(pEscort->GetID());

		// logging
		if(GC.getLogging() && GC.getAILogging())
		{
			LogTacticalMessage(strLogString);
		}
	}

	//move any additional escorts near the civilian
	for (size_t i=0; i<vExtraEscorts.size(); i++)
	{
		CvUnit* pUnit = vExtraEscorts[i];
		MoveToEmptySpaceNearTarget( pUnit, pCivilian->plot(), NO_DOMAIN, 23 );
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strTemp;
			CvString strLogString;
			strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
			strLogString.Format("Moving additional escorting %s to civilian for operation, Civilian X: %d, Civilian Y: %d, X: %d, Y: %d", strTemp.GetCString(), pCivilian->plot()->getX(), pCivilian->plot()->getY(), pUnit->getX(), pUnit->getY());
			LogTacticalMessage(strLogString);
		}
		UnitProcessed(pUnit->GetID());
	}
}

/// Move a large army to its destination against an enemy target
void CvTacticalAI::PlotArmyMovesCombat(CvArmyAI* pThisArmy)
{
	if (!pThisArmy)
		return;

	CvAIOperation* pOperation = GET_PLAYER(pThisArmy->GetOwner()).getAIOperation(pThisArmy->GetOperationID());
	if (!pOperation || pOperation->GetMusterPlot() == NULL)
		return;

	//where do we want to go
	CvPlot* pThisTurnTarget = pOperation->ComputeTargetPlotForThisTurn(pThisArmy);
	if (pThisTurnTarget == NULL)
	{
		pOperation->SetToAbort(AI_ABORT_LOST_PATH);
		return;
	}

	if (pOperation->GetEnemy() != NO_PLAYER)
	{
		if (GET_TEAM(GET_PLAYER(pOperation->GetEnemy()).getTeam()).IsVassalOfSomeone())
		{
			pOperation->SetToAbort(AI_ABORT_WAR_STATE_CHANGE);
			return;
		}
	}

	// RECRUITING
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE || 
		pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		//no matter if successful or not
		CheckForEnemiesNearArmy(pThisArmy);

		// This is where we try to gather. Don't use the center of mass here, it may drift anywhere 
		ExecuteGatherMoves(pThisArmy,pThisTurnTarget);
	}

	// MOVING TO TARGET
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		//sneak attack about to move into soon-to-be-enemy territory? can happen with open borders ...
		if (!pOperation->IsShowOfForce() && !m_pPlayer->IsAtWarWith(pOperation->GetEnemy()) &&
			pOperation->GetEnemy() != NO_PLAYER && pThisTurnTarget->getTeam() == GET_PLAYER(pOperation->GetEnemy()).getTeam())
		{
			m_pPlayer->GetDiplomacyAI()->DeclareWar(pOperation->GetEnemy());
		}

		//no matter if successful or not
		CheckForEnemiesNearArmy(pThisArmy);

		//try to arrage the units somewhat closer to the target
		ExecuteGatherMoves(pThisArmy,pThisTurnTarget);
	}
}

//workaround to make units from disbanded armies accessible to tactical AI in the same turn
void CvTacticalAI::AddCurrentTurnUnit(CvUnit * pUnit)
{
	if (pUnit)
		m_CurrentTurnUnits.push_back( pUnit->GetID() );
}

/// Queues up attacks on enemy units on or adjacent to army's desired center
bool CvTacticalAI::CheckForEnemiesNearArmy(CvArmyAI* pArmy)
{
	if (!pArmy)
		return false;

	//make a unique set of enemy units
	set<CvPlot*> allEnemyPlots;
	vector<CvUnit*> vUnitsInitial, vUnitsFinal;
	CvUnit* pUnit = pArmy->GetFirstUnit();
	while (pUnit)
	{
		if (pUnit->isDelayedDeath() || pUnit->GetCurrHitPoints()<pUnit->GetMaxHitPoints()/2)
		{
			pUnit = pArmy->GetNextUnit(pUnit);
			continue;
		}

		//check for enemy units, also around the immediate neighbor plots to be sure
		for (int i = 0; i < RING1_PLOTS; i++)
		{
			CvPlot* pTestPlot = iterateRingPlots(pUnit->plot(), i);
			if (!pTestPlot)
				continue;
			//combat units
			vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pTestPlot,m_pPlayer->getTeam());
			for (size_t i = 0; i < vAttackers.size(); i++)
				allEnemyPlots.insert(vAttackers[i]->plot());
		}

		//if the closest city belongs to the enemy, make sure we don't ignore it
		CvCity* pClosestCity = GC.getGame().GetClosestCityByPlots(pUnit->plot(), NO_PLAYER);
		if (pClosestCity && m_pPlayer->IsAtWarWith(pClosestCity->getOwner()))
			allEnemyPlots.insert(pClosestCity->plot());

		vUnitsInitial.push_back(pUnit);
		pUnit = pArmy->GetNextUnit(pUnit);
	}

	//find the closest pair
	int iMinDistGlobal = INT_MAX;
	CvPlot* pClosestEnemyPlot = NULL;
	for (size_t i = 0; i < vUnitsInitial.size(); i++)
	{
		int iMinDistForThisUnit = INT_MAX;
		for (set<CvPlot*>::iterator it = allEnemyPlots.begin(); it != allEnemyPlots.end(); ++it)
		{
			int iDistance = plotDistance(*vUnitsInitial[i]->plot(), **it);
			if (iDistance < iMinDistGlobal)
			{
				iMinDistGlobal = iDistance;
				pClosestEnemyPlot = *it;
			}

			//don't include units which are far away (should really use the pathfinder ...)
			if (iDistance < iMinDistForThisUnit)
				iMinDistForThisUnit = iDistance;
		}

		if (iMinDistForThisUnit < 5)
			vUnitsFinal.push_back(vUnitsInitial[i]);
	}

	//don't get sidetracked
	if (iMinDistGlobal>3 || pClosestEnemyPlot==NULL)
		return false;

	//do we have additional units around?
	if (FindUnitsWithinStrikingDistance(pClosestEnemyPlot))
		for (size_t i=0; i<m_CurrentMoveUnits.size(); i++)
			vUnitsFinal.push_back( m_pPlayer->getUnit( m_CurrentMoveUnits[i].GetID() ) );

	//we probably didn't see all enemy units, so doublecheck ...
	CvPlot* pTargetPlot = pClosestEnemyPlot;
	CvTacticalDominanceZone* pZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pClosestEnemyPlot);
	if (pZone && pZone->GetZoneCity() && pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
		return false;

	int iCount = 0;
	bool bSuccess = false;
	do
	{
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestOffensiveAssignment(vUnitsFinal, pTargetPlot, AL_LOW, gTactPosStorage);
		if (vAssignments.empty())
			break;

		//restarts might happen when new enemies become visible
		bSuccess = TacticalAIHelpers::ExecuteUnitAssignments(m_pPlayer->GetID(), vAssignments);
		iCount++;
	}
	while (!bSuccess && iCount < 4);

	return bSuccess;
}

void CvTacticalAI::ExecuteGatherMoves(CvArmyAI * pArmy, CvPlot * pTurnTarget)
{
	if (!pArmy || !pTurnTarget)
		return;

	vector<CvUnit*> vUnits;
	CvUnit* pUnit = pArmy->GetFirstUnit();
	while (pUnit)
	{
		if (!pUnit->isDelayedDeath() && pUnit->canMove())
			vUnits.push_back(pUnit);

		pUnit = pArmy->GetNextUnit(pUnit);
	}

	PositionUnitsAroundTarget(vUnits, pTurnTarget);

	// Update army's current location
	CvPlot* pCoM = pArmy->GetCenterOfMass();
	if (!pCoM)
	{
		CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());
		if (pOperation)
			pOperation->SetToAbort(AI_ABORT_LOST_PATH);
	}
	else
		pArmy->SetXY(pCoM->getX(),pCoM->getY());
}

// ROUTINES TO PROCESS AND SORT TARGETS

/// Mark units that can damage key items as priority targets
void CvTacticalAI::IdentifyPriorityTargets()
{
	//calculate this only once
	std::map<int,ReachablePlots> unitMovePlots;

	// Loop through each of our cities
	int iCityLoop;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		// Compile a list of units that can attack it this turn and what their expected damage is
		vector<CvTacticalTarget*> possibleAttackers;
		int iExpectedTotalDamage = 0;

		for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextZoneTarget())
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if(pEnemyUnit)
			{
				int iExpectedDamage = 0;

				if(pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->canRangeStrikeAt(pLoopCity->getX(), pLoopCity->getY()))
				{
					//maybe take into account that ranged units can move? but should be okay, siege units are slow
					iExpectedDamage = pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false, 0, NULL, NULL, true, true);
				}
				else
				{
					ReachablePlots plots;
					std::map<int,ReachablePlots>::iterator it = unitMovePlots.find(pEnemyUnit->GetID());
					if (it==unitMovePlots.end())
					{
						plots = pEnemyUnit->GetAllPlotsInReachThisTurn(true, true, false);
						unitMovePlots[pEnemyUnit->GetID()] = plots;
					}
					else
						plots = it->second;

					ReachablePlots::iterator plot = plots.find( pLoopCity->plot()->GetPlotIndex() );
					if ( plot != plots.end() )
					{
						if (pEnemyUnit->isRangedSupportFire()) //plot is likely incorrect but this is just an estimation
							iExpectedDamage += pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false, 0, 
													pLoopCity->plot(), pEnemyUnit->plot(), true);

						int iAttackerStrength = pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL, true, true);
						int iDefenderStrength = pLoopCity->getStrengthValue(false,pEnemyUnit->ignoreBuildingDefense());
						iExpectedDamage += pEnemyUnit->getCombatDamage(iAttackerStrength, iDefenderStrength, /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ true);
					}
				}

				if(iExpectedDamage > 0)
				{
					iExpectedTotalDamage += iExpectedDamage;
					possibleAttackers.push_back(pTarget);
				}
			}
		}

		// If they can take the city down and they are a melee unit, then they are a high priority target
		if (iExpectedTotalDamage > (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()))
		{
			for (size_t i = 0; i < possibleAttackers.size(); i++)
			{
				CvPlot* pPlot = GC.getMap().plot( possibleAttackers[i]->GetTargetX(), possibleAttackers[i]->GetTargetY() );
				CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
				if (pEnemyUnit && pEnemyUnit->IsCanAttackRanged())
					possibleAttackers[i]->SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
				else
					possibleAttackers[i]->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			}
		}
		// If they can damage a city they are a medium priority target
		else if(possibleAttackers.size() > 0)
		{
			for (size_t i = 0; i < possibleAttackers.size(); i++)
				if(possibleAttackers[i]->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
					possibleAttackers[i]->SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
		}
	}
}

/// Mark units that can damage our barbarian camps as priority targets
void CvTacticalAI::IdentifyPriorityBarbarianTargets()
{
	if (!m_pPlayer->isBarbarian())
	{
		for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextZoneTarget())
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if (pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
		}
	}
	else
	{
		for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
			if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextZoneTarget())
				{
					bool bPriorityTarget = false;

					// Skip if already a priority target (because was able to strike another camp)
					if(pTarget->GetTargetType() != AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
					{
						CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
						CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
						if(pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->GetMaxRangedCombatStrength(NULL, NULL, true) > pEnemyUnit->GetMaxAttackStrength(NULL, pLoopPlot, NULL))
						{
							if(plotDistance(pEnemyUnit->getX(), pEnemyUnit->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= pEnemyUnit->GetRange())
							{
								if(pEnemyUnit->canRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
								{
									bPriorityTarget = true;
								}
							}
						}
						else if(pEnemyUnit->TurnsToReachTarget(pLoopPlot,CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN,1)<1)
						{
							bPriorityTarget = true;
						}
						if(bPriorityTarget)
						{
							pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
						}
					}
				}
			}
		}
	}
}

void CvTacticalAI::DumpTacticalTargets(const char* hint)
{
	for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		const char* prio;
		switch (pTarget->GetTargetType())
		{
		case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
			prio = "low";
			break;
		case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
			prio = "med";
			break;
		case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
			prio = "hi";
			break;
		default:
			continue;
		}

		CvUnit* pUnit = pTarget->GetUnitPtr();
		CvString strMsg;
		strMsg.Format("Enemy unit (%s) at (%d,%d) with prio %s, score %d (%s with %d hp)", hint, pTarget->GetTargetX(), pTarget->GetTargetY(), 
			prio, pTarget->GetAuxIntData(), pUnit->getName().c_str(), pUnit->GetCurrHitPoints());
		LogTacticalMessage(strMsg);
	}
}

/// Mark units that we'd like to make opportunity attacks on because of their unit type (e.g. catapults)
void CvTacticalAI::IdentifyPriorityTargetsByType()
{
	// Look through all the enemies we can see
	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		CvUnit* pUnit = m_AllTargets[iI].GetUnitPtr();
		if (!pUnit)
			continue;

		// Don't consider units that are already medium priority
		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			// Ranged units will always be medium priority targets
			if(pUnit->IsCanAttackRanged())
			{
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
			}
		}

		// Carriers are always high prio
		if (pUnit->hasCargo())
			m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);

		// Don't consider units that are already high priority
		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			// Units defending citadels will always be high priority targets
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
			// Don't consider units that are already higher than low priority
			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				//units which are in the front line should be medium at least
				if (pUnit->plot()->GetNumSpecificPlayerUnitsAdjacent(m_pPlayer->GetID())>0)
					m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
			}

			// Don't consider units that are already high priority
			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
					m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				// Units defending forts will always be high priority targets
				ImprovementTypes eImprovement = pUnit->plot()->getImprovementType();
				if(eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetDefenseModifier() >= 25)
				{
					m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
				}

				//Is a unit below 1/4 health? If so, make it a high-priority target.
				if(pUnit->getOwner() != m_pPlayer->GetID())
				{
					if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints() / 4)
					{
						m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					}
				}
			}
		}
#endif
	}
}

/// after the high/mid/low prio unit target types are set, adjust the score so that the sorting works correctly
void CvTacticalAI::UpdateTargetScores()
{
	for(TacticalList::iterator it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());

		//try to attack target (no matter the type) close to us first
		if (pPlot->IsFriendlyUnitAdjacent(m_pPlayer->getTeam(), true))
			it->SetAuxIntData(it->GetAuxIntData() + 8);

		//try to attack targets close to our cities first
		if (m_pPlayer->GetCityDistanceInPlots(pPlot)<4)
			it->SetAuxIntData(it->GetAuxIntData() + 3);

		if(it->GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
		{
			//initially all targets were low prio, if it's higher now we give it a boost
			it->SetAuxIntData( it->GetAuxIntData() + 25 );

			CvPlot* pPlot = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
			CvUnit* pUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if (pUnit)
			{
				//if the target is damaged, give it a further boost
				it->SetAuxIntData( it->GetAuxIntData() + pUnit->getDamage()/10 );
			}
		}

		if(it->GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
		{
			//initially all targets were low prio, if it's higher now we give it a boost
			it->SetAuxIntData( it->GetAuxIntData() + 50 );

			CvPlot* pPlot = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
			CvUnit* pUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if (pUnit)
			{
				//if the target is damaged, give it a further boost
				it->SetAuxIntData( it->GetAuxIntData() + pUnit->getDamage()/10 );
			}
		}
	}
}

void CvTacticalAI::SortTargetListAndDropUselessTargets()
{
	// Important: Sort all targets by aux data (if used for that target type)
	std::stable_sort(m_AllTargets.begin(), m_AllTargets.end());

	TacticalList reducedTargetList;

	//now in the sorted list we can suppress adjacent non-maximum targets
	int iSuppressionRange = 1; //could even try 2 ...
	for (TacticalList::iterator it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
	{
		bool bBetterTargetAdjacent = false;

		//do this only for enemy units in the same domain
		if (it->GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			it->GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			it->GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT )
		{
			for (TacticalList::iterator it2 = reducedTargetList.begin(); it2 != reducedTargetList.end(); ++it2)
			{
				//trick: land zones have id > 0, water zones id < 0. so negative product means domain mismatch
				if (it->GetDominanceZone() * it2->GetDominanceZone() < 0)
					continue;

				//if close to one of our cities, make sure we're not dropping it
				CvPlot* pPlot = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
				if (m_pPlayer->GetCityDistanceInPlots(pPlot) < 4)
					continue;

				if (it2->GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
					it2->GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
					it2->GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
				{
					if (it2->GetAuxIntData() >= it->GetAuxIntData() && plotDistance(it2->GetTargetX(), it2->GetTargetY(), it->GetTargetX(), it->GetTargetY()) <= iSuppressionRange)
					{
						bBetterTargetAdjacent = true;
						break;
					}
				}
			}
		}

		if (!bBetterTargetAdjacent)
			reducedTargetList.push_back(*it);
	}

	m_AllTargets = reducedTargetList;
}

void CvTacticalAI::ClearCurrentMoveUnits(AITacticalMove eNewMove)
{
	m_CurrentAirSweepUnits.clear();
	m_CurrentMoveCities.clear();
	m_CurrentMoveUnits.clear();
	m_CurrentMoveUnits.setCurrentTacticalMove(eNewMove);
}

/// Sift through the target list and find just those that apply to the dominance zone we are currently looking at
void CvTacticalAI::ExtractTargetsForZone(CvTacticalDominanceZone* pZone /* Pass in NULL for all zones */)
{
	m_ZoneTargets.clear();
	for(TacticalList::iterator it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
	{
		bool bValid = false;

		if(pZone == NULL)
		{
			bValid = true;
		}
		else
		{
			DomainTypes eDomain = pZone->IsWater() ? DOMAIN_SEA : DOMAIN_LAND;
			bValid = it->IsTargetValidInThisDomain(eDomain);
		}

		if(bValid)
		{
			if(pZone == NULL || it->GetDominanceZone() == pZone->GetZoneID())
			{
				m_ZoneTargets.push_back(*it);
				continue;
			}

			// Not obviously in this zone, but if within 2 of city we want them anyway
			CvCity* pCity = pZone->GetZoneCity();
			if(pCity && plotDistance(pCity->getX(), pCity->getY(), it->GetTargetX(), it->GetTargetY()) <= 2)
			{
				m_ZoneTargets.push_back(*it);
				continue;
			}

			// Last chance, adjacent to the current zone is also ok, zone boundaries are approximate only
			CvPlot* pCheck = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
			for (int i = RING0_PLOTS; i < RING1_PLOTS; i++)
			{
				CvPlot* pNeighbor = iterateRingPlots(pCheck, i);
				if (pNeighbor)
				{
					CvTacticalDominanceZone* pNeighborZone = GetTacticalAnalysisMap()->GetZoneByPlot(pNeighbor);
					if (pNeighborZone && pNeighborZone->GetZoneID() == pZone->GetZoneID())
					{
						m_ZoneTargets.push_back(*it);
						continue;
					}
				}
			}
		}
	}
}

/// Find the first target of a requested type in current dominance zone (call after ExtractTargetsForZone())
CvTacticalTarget* CvTacticalAI::GetFirstZoneTarget(AITacticalTargetType eType, eAggressionLevel eMaxLvl)
{
	m_eCurrentTargetType = eType;
	m_iCurrentTargetIndex = 0;

	while(m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if (m_ZoneTargets[m_iCurrentTargetIndex].GetLastAggLvl() <= eMaxLvl)
		{
			if (m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
			{
				return &m_ZoneTargets[m_iCurrentTargetIndex];
			}
		}
		m_iCurrentTargetIndex++;
	}

	return NULL;
}

/// Find the next target of a requested type in current dominance zone (call after GetFirstZoneTarget())
CvTacticalTarget* CvTacticalAI::GetNextZoneTarget(eAggressionLevel eMaxLvl)
{
	m_iCurrentTargetIndex++;

	while(m_iCurrentTargetIndex < (int)m_ZoneTargets.size())
	{
		if (m_ZoneTargets[m_iCurrentTargetIndex].GetLastAggLvl() <= eMaxLvl)
		{
			if (m_eCurrentTargetType == AI_TACTICAL_TARGET_NONE || m_ZoneTargets[m_iCurrentTargetIndex].GetTargetType() == m_eCurrentTargetType)
			{
				return &m_ZoneTargets[m_iCurrentTargetIndex];
			}
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
	size_t iMaxUnits = min(2u,m_CurrentMoveUnits.size());
	for (size_t i=0; i<iMaxUnits; i++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[i].GetID());
		if (!pUnit)
			continue;

		//a little bit of AI cheating here.
		//the problem is that sometimes the camp is too strong to attack with a single unit, it needs an army to take it out
		//so if we have a single unit it moves in, does nothing, reposition moves take it away, next turn it comes back etc.
		//so we peek at the unit in the camp even if it's invisible and see if it's even worth going there

		if (!TacticalAIHelpers::IsAttackNetPositive(pUnit, pTargetPlot))
			continue;

		//guarded camp?
		if (pTargetPlot->isEnemyUnit(m_pPlayer->GetID(), true, false))
		{
			ExecuteMoveToPlot(pUnit, pTargetPlot, false, CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN);
		}
		else //empty camp, move in and move on
		{
			ExecuteMoveToPlot(pUnit, pTargetPlot, true);
		}
	}
}

/// Pillage an undefended improvement
bool CvTacticalAI::ExecutePillage(CvPlot* pTargetPlot)
{
	for (size_t i = 0; i < m_CurrentMoveUnits.size(); i++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[i].GetID());
		if (pUnit && pUnit->canMoveInto(*pTargetPlot, CvUnit::MOVEFLAG_DESTINATION))
		{
			if (pUnit->shouldPillage(pTargetPlot))
			{
				ExecuteMoveToPlot(pUnit, pTargetPlot, true, CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED);

				//now that the neighbor plots are revealed, maybe it's better to retreat?
				CvPlot* pSafePlot = NULL;
				if (!TacticalAIHelpers::IsEnemyCitadel(pUnit->plot(), m_pPlayer->GetID()))
				{
					int iVal = pUnit->GetCurrHitPoints() + GC.getPILLAGE_HEAL_AMOUNT();
					if (pUnit->IsGainsXPFromPillaging())
						iVal += GC.getPILLAGE_HEAL_AMOUNT();

					if ((pUnit->GetDanger() > iVal) && !pUnit->hasFreePillageMove())
						pSafePlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true, false, false);
				}

				if (pSafePlot)
				{
					//better go somewhere else
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pSafePlot->getX(), pSafePlot->getY());
					if (!pUnit->canMove())
						UnitProcessed(pUnit->GetID());
					//no use trying this target again
					return false;
				}
				else
				{
					//proceed
					pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
					if (!pUnit->canMove())
						UnitProcessed(pUnit->GetID());
					//done
					return true;
				}
			}
		}
	}

	return false;
}

/// Pillage an undefended improvement
void CvTacticalAI::ExecutePlunderTradeUnit(CvPlot* pTargetPlot)
{
	// Move first one to target
	CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		if(pUnit->canMoveInto(*pTargetPlot, CvUnit::MOVEFLAG_DESTINATION ))
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
			pUnit->PushMission(CvTypes::getMISSION_PLUNDER_TRADE_ROUTE());
		}
		else
			MoveToEmptySpaceNearTarget(pUnit,pTargetPlot,NO_DOMAIN,23);

		//only end the turn if we can't move anymore
		if (!pUnit->canMove())
			UnitProcessed(pUnit->GetID());
	}
}

/// Paradrop in to pillage an undefended improvement
void CvTacticalAI::ExecuteParadropPillage(CvPlot* pTargetPlot)
{
	// Move first one to target
	CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_PARADROP(), pTargetPlot->getX(), pTargetPlot->getY());
		pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

		// Delete this unit from those we have to move
		if (!pUnit->canMove())
			UnitProcessed(pUnit->GetID());
	}
}

void CvTacticalAI::ExecuteAirAttack(CvPlot* pTargetPlot)
{
	if (!pTargetPlot)
		return;

	// Do air attacks, ignore all other units
	bool bDone = false;
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());

		if (pUnit && pUnit->getDomainType()==DOMAIN_AIR) //this includes planes and missiles. no nukes.
		{
			int iCount = 0; //failsafe
			while (pUnit->canMove() && pUnit->GetCurrHitPoints() > 30 && iCount < pUnit->getNumAttacks())
			{
				CvPlot* pBestTarget = FindAirTargetNearTarget(pUnit, pTargetPlot);
				if (pBestTarget != NULL)
				{
					//this won't change
					CvCity *pCity = pBestTarget->getPlotCity();

					//this is a bit ugly ... oh well
					CvUnit* pDefender = pBestTarget->getVisibleEnemyDefender(m_pPlayer->GetID());
					bool bHaveUnitTarget = (pDefender != NULL && !pDefender->isDelayedDeath());
					bool bHaveCityTarget = (pCity != NULL && pCity->getDamage() < pCity->GetMaxHitPoints());

					if (!bHaveUnitTarget && !bHaveCityTarget)
					{
						bDone = true;
						break;
					}

					//it's a ranged attack but it uses the move mission ... air units are strange
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("AIR ATTACK: %s %d attacks Target X: %d, Y: %d", pUnit->getName().c_str(), pUnit->GetID(), pBestTarget->getX(), pBestTarget->getY());
						LogTacticalMessage(strMsg);
					}
				}
				iCount++;
			}

			if (pUnit->getNumAttacks() - pUnit->getNumAttacksMadeThisTurn() == 0)
				UnitProcessed(m_CurrentMoveUnits[iI].GetID());

			if (bDone)
				break;
		}
	}
}

/// Queues up attacks on enemy units on or adjacent to army's desired center
CvPlot* CvTacticalAI::FindAirTargetNearTarget(CvUnit* pUnit, CvPlot* pTargetPlot)
{
	int iRange = pUnit->GetRange();
	int iBestValue = -INT_MAX;
	CvPlot* pBestPlot = NULL;

	// Loop through all appropriate targets to see if any is of concern
	for (unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN ||
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
		{
			//make sure it is close to our actual target plot
			int iTargetDistance = plotDistance(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY(), pTargetPlot->getX(), pTargetPlot->getY());
			if (iTargetDistance > 3)
				continue;

			int iDistance = plotDistance(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY(), pUnit->getX(), pUnit->getY());
			if (iDistance < iRange)
			{
				CvPlot* pTestPlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
				if (pTestPlot == NULL)
					continue;

				CvCity *pCity = pTestPlot->getPlotCity();
				CvUnit* pDefender = pUnit->rangeStrikeTarget(*pTestPlot, true);

				bool bHaveUnitTarget = (pDefender != NULL && !pDefender->isDelayedDeath());
				bool bHaveCityTarget = (pCity != NULL && pCity->getDamage() < pCity->GetMaxHitPoints());
				if (!bHaveUnitTarget && !bHaveCityTarget)
					continue;

				int iValue = 0;
				if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
					iValue += 20;
				if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
					iValue += 10;

				if (pUnit->AI_getUnitAIType() == UNITAI_MISSILE_AIR)
				{
					//we need a unit to attack, if it's a civilian only generals are of interest
					if (!pDefender)
						continue;

					//ignore the city when attacking!
					int iDamage = pUnit->GetAirCombatDamage(pDefender, NULL, false);

					if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN)
						iValue = 50; //flat default value - always a kill
					else
					{
						iValue = iDamage;
						//bonus for a kill
						if (pDefender->GetCurrHitPoints() <= iDamage)
							iValue += 20;
						//bonus for hitting units in cities, can only do that with missiles
						if (pDefender->plot()->isCity())
							iValue += 10;
					}
				}
				else
				{
					//no civilian attacks here, it's inefficient
					if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN)
						continue;

					//use distance from actual target as tiebreaker
					iValue = pUnit->GetAirCombatDamage(pDefender, pCity, false) - iTargetDistance * 3;

					if (pCity != NULL)
						iValue -= pCity->GetAirStrikeDefenseDamage(pUnit, false);
					else if (pDefender != NULL)
						iValue -= pDefender->GetAirStrikeDefenseDamage(pUnit, false);

					if (pTargetPlot->GetBestInterceptor(pUnit->getOwner(),NULL,false,true) != NULL)
						iValue /= 2;
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pTestPlot;
				}
			}			
		}
	}

	return pBestPlot;
}

void CvTacticalAI::ExecuteAirSweep(CvPlot* pTargetPlot)
{
	if (!pTargetPlot)
		return;

#if defined(MOD_AI_SMART_AIR_TACTICS)
	if (MOD_AI_SMART_AIR_TACTICS) {
		// Start by sending possible air sweeps
		for(unsigned int iI = 0; iI < m_CurrentAirSweepUnits.size(); iI++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentAirSweepUnits[iI].GetID());

			if(pUnit && pUnit->canMove())
			{
				if(pUnit->canAirSweep())
				{
					pUnit->PushMission(CvTypes::getMISSION_AIR_SWEEP(), pTargetPlot->getX(), pTargetPlot->getY());
					UnitProcessed(m_CurrentAirSweepUnits[iI].GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Sending %s %d to air sweep prior to attack to Target X: %d, Y: %d", pUnit->getName().c_str(), pUnit->GetID(), pTargetPlot->getX(), pTargetPlot->getY());
						LogTacticalMessage(strMsg);
					}
				}
			}
		}
	}
#endif
}

bool CvTacticalAI::ExecuteSpotterMove(vector<CvUnit*> vUnits, CvPlot* pTargetPlot)
{
	if (pTargetPlot->isVisible(m_pPlayer->getTeam()))
		return true; //nothing to do

	//else find a suitable unit
	vector<CvUnit*> vCandidates;
	for (size_t i = 0; i < vUnits.size(); i++)
	{
		CvUnit* pUnit = vUnits[i];

		// we want fast units or tanks
		// (unitai defense includes ranged units ... don't use it here)
		switch (pUnit->AI_getUnitAIType())
		{
		case UNITAI_FAST_ATTACK:
		case UNITAI_ATTACK_SEA:
		case UNITAI_ATTACK:
		case UNITAI_COUNTER:
			vCandidates.push_back(pUnit);
			break;
		}
	}

	//first pass, try to go close and retreat
	for (size_t i=0; i<vCandidates.size();i++)
	{
		CvUnit* pUnit = vCandidates[i];
		int iFlags = CvUnit::MOVEFLAG_NO_EMBARK;

		//move into ring 2 unless we are already there and still can't see the target
		iFlags |= plotDistance(*pTargetPlot,*pUnit->plot())>2 ? CvUnit::MOVEFLAG_APPROX_TARGET_RING2 : CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

		// do we have some spare movement points to be able to retreat?
		// (retreat would be handled elsewhere)
		if (pUnit->TurnsToReachTarget(pTargetPlot,iFlags|CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN,1)==0)
		{
			ExecuteMoveToPlot(pUnit, pTargetPlot, true, iFlags);
			return pTargetPlot->isVisible(m_pPlayer->getTeam());
		}
	}

	//second pass, try to go close and stay (hoping that danger isn't too high)
	for (size_t i=0; i<vCandidates.size();i++)
	{
		CvUnit* pUnit = vCandidates[i];

		int iFlags = CvUnit::MOVEFLAG_NO_EMBARK;

		//move into ring 2 unless we are already there and still can't see the target
		iFlags |= plotDistance(*pTargetPlot,*pUnit->plot())>2 ? CvUnit::MOVEFLAG_APPROX_TARGET_RING2 : CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

		if (pUnit->GetDanger(pTargetPlot)<pUnit->GetCurrHitPoints()/2 && pUnit->TurnsToReachTarget(pTargetPlot,iFlags,1)==0)
		{
			ExecuteMoveToPlot(pUnit, pTargetPlot, true, iFlags);
			return pTargetPlot->isVisible(m_pPlayer->getTeam());
		}
	}

	return false;
}

//evaluate many possible unit assignments around the target plot and choose the best one
//will not necessarily attack only the target plot when other targets are present!
//todos:
// - find out most dangerous enemy unit (cf IdentifyPriorityTargets)
// - consider possible kills in unit position score (ie ranged unit doesn't need to move if it can kill the attacker so will be safe from retaliation)
bool CvTacticalAI::ExecuteAttackWithUnits(CvPlot* pTargetPlot, eAggressionLevel eAggLvl)
{
	vector<CvUnit*> vUnits;
	for (size_t i=0; i<m_CurrentMoveUnits.size(); i++)
		if (m_CurrentMoveUnits[i].GetAttackStrength()>0) //sometimes we mark units as not desired
			vUnits.push_back( m_pPlayer->getUnit( m_CurrentMoveUnits[i].GetID() ) );

	//try to improve visibility
	if (!ExecuteSpotterMove(vUnits,pTargetPlot))
		return false;

	//first handle air units
	ExecuteAirSweep(pTargetPlot);
	ExecuteAirAttack(pTargetPlot);

	//did the air attack already kill the enemy?
	if (pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID(), NULL, true) == NULL && !pTargetPlot->isCity())
		return true;

	int iCount = 0;
	bool bSuccess = false;
	do
	{
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestOffensiveAssignment(vUnits, pTargetPlot, eAggLvl, gTactPosStorage);
		if (vAssignments.empty())
			break;
		
		//true if everything went according to plan. if not, repeat!
		bSuccess = TacticalAIHelpers::ExecuteUnitAssignments(m_pPlayer->GetID(), vAssignments);
		iCount++;
	}
	while (!bSuccess && iCount < 4);

	return bSuccess;
}

//target can be friendly, neutral or hostile
bool CvTacticalAI::PositionUnitsAroundTarget(vector<CvUnit*> vUnits, CvPlot* pTargetPlot)
{
	//try to improve visibility
	if (!ExecuteSpotterMove(vUnits,pTargetPlot))
		return false;

	int iCount = 0;
	bool bSuccess = false;
	do
	{
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestDefensiveAssignment(vUnits, pTargetPlot, gTactPosStorage);
		if (vAssignments.empty())
			break;
		
		//true if everything went according to plan. if not, repeat!
		bSuccess = TacticalAIHelpers::ExecuteUnitAssignments(m_pPlayer->GetID(), vAssignments);
		iCount++;
	}
	while (!bSuccess && iCount < 4);

	//in case not all units were moved ...
	for (vector<CvUnit*>::iterator it = vUnits.begin(); it != vUnits.end(); ++it)
	{
		CvUnit* pUnit = *it;
		if (pUnit->TurnProcessed())
			continue;

		//don't go directly to the target, instead find one of our units which is close to the target
		CvPlot* pClosestUnit = NULL;
		//we only want units on our side of the target!
		int iClosestDistance = plotDistance(*pTargetPlot, *pUnit->plot());
		for (int i = RING0_PLOTS; i < RING4_PLOTS; i++)
		{
			CvPlot* pTestPlot = iterateRingPlots(pTargetPlot, i);
			//ignore the current plot of the unit, we want to make progress
			if (!pTestPlot || pTestPlot == pUnit->plot())
				continue;

			//the > is important, it allows the garrison as target in case of a friendly city
			int iDistance = plotDistance(*pTestPlot, *pUnit->plot());
			if (iDistance > iClosestDistance)
				continue;

			CvUnit* pFriendlyUnit = pTestPlot->getBestDefender(m_pPlayer->GetID());
			if ((pFriendlyUnit && pFriendlyUnit->getDomainType()==pUnit->getDomainType()) || pTestPlot->isFriendlyCity(*pUnit))
			{
				iClosestDistance = iDistance;
				pClosestUnit = pTestPlot;
			}
		}

		//don't move into danger blindly
		if (!pClosestUnit)
			continue;

		//don't leave a general out in the cold
		if (pUnit->GetDanger() == INT_MAX)
		{
			CvPlot* pSafePlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
			if (pSafePlot)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pSafePlot->getX(), pSafePlot->getY());
				UnitProcessed(pUnit->GetID());
				continue;
			}
		}

		//civilians can stack ...
		int iFlags = 0;
		if (pUnit->IsCombatUnit())
			iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;

		CvPlot* pOldPlot = pUnit->plot();
		ExecuteMoveToPlot(pUnit, pClosestUnit, true, iFlags);

		//important: only mark the unit as processed if we made a move!
		//if we're stuck it should be used for other purposes
		if (pUnit->plot() != pOldPlot)
			UnitProcessed(pUnit->GetID());
	}

	return bSuccess;
}

void CvTacticalAI::ExecuteLandingOperation(CvPlot* pTargetPlot)
{
	if (!pTargetPlot)
		return;

	struct SAssignment
	{
		SAssignment( CvUnit* unit, CvPlot* plot, int score, bool isAttack ) : pUnit(unit), pPlot(plot), iScore(score), bAttack(isAttack) {}
		CvUnit* pUnit;
		CvPlot* pPlot;
		int iScore;
		bool bAttack;
		bool operator<(const SAssignment& rhs) { return iScore>rhs.iScore; }
	};

	struct PrPlotMatch
	{
		PrPlotMatch(CvPlot* refPlot) : pRefPlot(refPlot) {}
		CvPlot* pRefPlot;
		bool operator()(const SAssignment& other) { return pRefPlot==other.pPlot; } 
	};

	struct PrUnitMatch
	{
		PrUnitMatch(CvUnit* refUnit) : pRefUnit(refUnit) {}
		CvUnit* pRefUnit;
		bool operator()(const SAssignment& other) { return pRefUnit==other.pUnit; } 
	};

	vector<SAssignment> choices;
	for (size_t i=0; i<m_CurrentMoveUnits.size(); i++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[i].GetID());
		if (!pUnit)
			continue;

		//first check our immediate neighborhood (ie the tiles we can reach within one turn)
		ReachablePlots eligiblePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
		for (ReachablePlots::iterator tile=eligiblePlots.begin(); tile!=eligiblePlots.end(); ++tile)
		{
			CvPlot* pEvalPlot = GC.getMap().plotByIndexUnchecked(tile->iPlotIndex);
			if (!pEvalPlot || !pEvalPlot->isCoastalLand())
				continue;
			
			int iBonus = plotDistance(*pEvalPlot,*pTargetPlot) * (-10);
			if (pUnit->isRanged())
			{
				if (pEvalPlot->getArea()!=pTargetPlot->getArea() && plotDistance(*pEvalPlot,*pTargetPlot)>pUnit->GetRange())
					continue;

				if (pEvalPlot->isHills())
					iBonus += 20;
			}
			else
			{
				if (pEvalPlot->getArea()!=pTargetPlot->getArea())
					continue;
			}

			bool bAttack = pEvalPlot->isEnemyCity(*pUnit);
			CvUnit* pDefender = pEvalPlot->getBestDefender(NO_PLAYER);
			if (pDefender)
			{
				if ( m_pPlayer->IsAtWarWith(pDefender->getOwner()) )
					bAttack = true;
				else
					continue; //must be a neutral unit or one of ours
			}

			if (bAttack)
			{
				//check if attack makes sense
				if (TacticalAIHelpers::IsAttackNetPositive(pUnit,pEvalPlot))
				{
					choices.push_back( SAssignment(pUnit,pEvalPlot,pUnit->GetMaxHitPoints()+1,true) );
				}
			}
			else
			{
				//check danger
				int iScore = pUnit->GetMaxHitPoints() - pUnit->GetDanger(pEvalPlot) + iBonus;
				if (iScore>0)
					choices.push_back( SAssignment(pUnit,pEvalPlot,iScore,false) );
			}
		}
	}

	//prefer non-isolated plots
	for (vector<SAssignment>::iterator it=choices.begin(); it!=choices.end(); ++it)
	{
		for (vector<SAssignment>::iterator it2=choices.begin(); it2!=choices.end(); ++it2)
		{
			if (it2!=it && plotDistance(*(it2->pPlot),*(it->pPlot))==1)
				it2->iScore += 10;
		}

		if (it->pPlot->IsFriendlyUnitAdjacent(m_pPlayer->getTeam(),true))
			it->iScore += 10;
	}

	//ok let's go
	sort(choices.begin(),choices.end());
	while (!choices.empty())
	{
		SAssignment next = choices.front();

		vector<SAssignment>::iterator last;
		last = remove_if( choices.begin(), choices.end(), PrPlotMatch(next.pPlot) ); choices.erase(last,choices.end());
		last = remove_if( choices.begin(), choices.end(), PrUnitMatch(next.pUnit) ); choices.erase(last,choices.end());

		next.pUnit->PushMission( CvTypes::getMISSION_MOVE_TO(), next.pPlot->getX(), next.pPlot->getY() );
		if (!next.pUnit->canMove()) //not all units end their turn after disembark - they can still be used for other moves!
			UnitProcessed(next.pUnit->GetID());
	}

	//note that it's possible some units were not moved because of conflicts
}

/// Execute moving units to a better location
void CvTacticalAI::ExecuteRepositionMoves()
{
	//don't be too predictable
	int shuffledIndex[RING4_PLOTS - RING1_PLOTS];
	for (int i = RING1_PLOTS; i < RING4_PLOTS; i++)
		shuffledIndex[i-RING1_PLOTS] = i;

	int iNumPlots = RING4_PLOTS - RING1_PLOTS;
	for (int i = 0; i < iNumPlots; i++)
	{
		int iSwapIndex = GC.getGame().getSmallFakeRandNum(iNumPlots - i, m_CurrentMoveUnits.size() + i);
		std::swap<int>(shuffledIndex[i], shuffledIndex[iSwapIndex]);
	}

	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (!pUnit)
			continue;

		//any cities we can reinforce?
		CvPlot* pTarget = FindNearbyTarget(pUnit, 12, false);
		if (!pTarget)
			continue;

		//already close ...
		if (plotDistance(*pTarget, *pUnit->plot()) < 5 && TacticalAIHelpers::IsGoodPlotForStaging(m_pPlayer, pUnit->plot(), pUnit->getDomainType()))
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			UnitProcessed(m_CurrentMoveUnits[iI].GetID());
			continue;
		}

		//find a good spot
		for (int i = 0; i<iNumPlots; i++)
		{
			CvPlot* pTestPlot = iterateRingPlots(pTarget, shuffledIndex[i]);
			if (!pTestPlot)
				continue;

			if (pUnit->isRanged())
				if (pTestPlot->IsAdjacentOwnedByTeamOtherThan(m_pPlayer->getTeam()))
					continue;

			//staging is not fighting ...
			if (pUnit->GetDanger(pTestPlot) > pUnit->GetCurrHitPoints()/5)
				continue;

			if (TacticalAIHelpers::IsGoodPlotForStaging(m_pPlayer, pTestPlot, pUnit->getDomainType()))
			{
				if(GC.getLogging() && GC.getAILogging() && m_pPlayer->isMajorCiv())
				{
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
					CvString strLogString;
					strLogString.Format("%s moving to reinforce city at, X: %d, Y: %d, Current X: %d, Current Y: %d", 
						strTemp.GetCString(), pTestPlot->getX(), pTestPlot->getY(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}

				ExecuteMoveToPlot(pUnit, pTestPlot);
				UnitProcessed(m_CurrentMoveUnits[iI].GetID());
				break;
			}
		}
	}
}

/// Moves units to the hex with the lowest danger
void CvTacticalAI::ExecuteMovesToSafestPlot()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit && pUnit->canUseForTacticalAI())
		{
			//see if we can do damage before retreating
			if (pUnit->canMoveAfterAttacking() && pUnit->getMoves()>1 && pUnit->canRangeStrike())
				TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);

			//so easy
			CvPlot* pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit,true,false,true);

			bool bRetreated = false;
			if (pBestPlot != NULL)
			{
				CvUnit* pSwapUnit = pUnit->GetPotentialUnitToSwapWith(*pBestPlot);

				//melee units are there to soak damage ...
				int iDangerLimit = pSwapUnit ? (pSwapUnit->isRanged() ? pSwapUnit->GetCurrHitPoints() : (3 * pSwapUnit->GetCurrHitPoints()) / 2) : 0;
				if (pSwapUnit && pSwapUnit->GetDanger(pUnit->plot()) < iDangerLimit)
				{
					pSwapUnit->SetActivityType(ACTIVITY_AWAKE);
					pUnit->PushMission(CvTypes::getMISSION_SWAP_UNITS(), pBestPlot->getX(), pBestPlot->getY());
					bRetreated = true;
				}
				else
				{
					//need to reconsider if there's a unit blocking our retreat
					if (pSwapUnit != NULL)
						pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);

					if (pBestPlot != NULL)
					{
						//pillage before retreat, if we have movement points to spare
						if (pUnit->getMoves()>GC.getMOVE_DENOMINATOR() && pBestPlot->isAdjacent(pUnit->plot()) && pUnit->shouldPillage(pUnit->plot()))
						{
							pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Flee: pillage with %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strMsg);
							}
						}

						//try to do some damage if we have movement points to spare
						if (pBestPlot->isAdjacent(pUnit->plot()) && pUnit->getMoves() > GC.getMOVE_DENOMINATOR() && pUnit->canRangeStrike() && pUnit->canMoveAfterAttacking())
							TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit, true);

						// Move to the lowest danger value found
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_TACTMOVE);
						bRetreated = true;

						//see if we can do damage after retreating
						if (pUnit->canMove() && pUnit->canRangeStrike())
							TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit, true);

						//pillage after retreat, if we have movement points to spare
						if (pUnit->shouldPillage(pUnit->plot()))
						{
							pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Flee: pillage with %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strMsg);
							}
						}
					}
				}
			}

			if (bRetreated)
			{
				UnitProcessed(pUnit->GetID());

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

#if defined(MOD_BALANCE_CORE)
				//not in an army and not at home? we really need to do something
				if(pUnit->getArmyID() == -1 && pUnit->plot()->getOwner() != pUnit->getOwner())
				{
					CvCity* pClosestCity = m_pPlayer->GetClosestCityByEstimatedTurns(pUnit->plot());
					if (m_pPlayer->isMinorCiv())
						pClosestCity = m_pPlayer->getCapitalCity();

					CvPlot* pMovePlot = pClosestCity ? pClosestCity->plot() : NULL;
					if(pMovePlot != NULL)
					{
						MoveToEmptySpaceNearTarget(pUnit,pMovePlot,DOMAIN_LAND,42,true);
						UnitProcessed(pUnit->GetID());
						CvString strTemp;
						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
						if(pkUnitInfo)
						{
							strTemp = pkUnitInfo->GetDescription();
							CvString strLogString;
							strLogString.Format("Fleeing %s wandering home, at X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
					else
					{
						pUnit->PushMission(CvTypes::getMISSION_SKIP());
						UnitProcessed(pUnit->GetID());
						CvString strTemp;
						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
						if(pkUnitInfo)
						{
							strTemp = pkUnitInfo->GetDescription();
							CvString strLogString;
							strLogString.Format("Fleeing %s stuck, at X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
#endif
			}
		}
	}
}

/// Heal chosen units
void CvTacticalAI::ExecuteHeals(bool bFirstPass)
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (!pUnit)
			continue;

		CvPlot* pBetterPlot = NULL;

		//need to split from army?
		if (pUnit->getArmyID() != -1)
		{
			CvArmyAI* pArmy = m_pPlayer->getArmyAI(pUnit->getArmyID());
			if (pArmy)
			{
				//Don't do this for civilan operations!
				CvAIOperation* AIOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());
				if (AIOperation && AIOperation->IsCivilianOperation())
					continue;

				if (pArmy->GetArmyAIState() != ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
					pArmy->RemoveUnit(pUnit->GetID());
			}
		}

		//ranged attack before fleeing
		if (pUnit->canMoveAfterAttacking() && pUnit->getMoves()>1 && pUnit->canRangeStrike())
			TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,pUnit->getMoves()>2);

		//find a suitable spot for healing
		if (pUnit->getDomainType() == DOMAIN_LAND)
		{
			//why not pillage some tiles?
			if (pUnit->shouldPillage(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Heal: pillage with %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strMsg);
				}
			}

			//land unit on land should move away from danger
			if (pUnit->GetDanger() > 0 && !pUnit->isEmbarked() && !pUnit->IsGarrisoned())
			{
				//unless we can eliminate the danger!
				std::vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pUnit->plot(),m_pPlayer->getTeam());
				if (vAttackers.size() == 1 && TacticalAIHelpers::KillUnitIfPossible(pUnit, vAttackers[0]))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Healing unit %s (%d) counterattacked pursuer at X: %d, Y: %d",
							pUnit->getName().GetCString(), pUnit->GetID(), vAttackers[0]->getX(), vAttackers[0]->getY());
						LogTacticalMessage(strLogString);
					}
				}
			}

			pBetterPlot = TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
		}
		else if (pUnit->getDomainType()==DOMAIN_SEA)
		{
			if (pUnit->GetDanger()>0 || !pUnit->canHeal(pUnit->plot()))
			{
				std::vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pUnit->plot(),m_pPlayer->getTeam());
				//try to turn the tables on him
				if (vAttackers.size() == 1 && TacticalAIHelpers::KillUnitIfPossible(pUnit, vAttackers[0]))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Healing unit %s (%d) counterattacked pursuer at X: %d, Y: %d",
							pUnit->getName().GetCString(), pUnit->GetID(), vAttackers[0]->getX(), vAttackers[0]->getY());
						LogTacticalMessage(strLogString);
					}
				}
				else
				{
					//why not pillage some tiles?
					if (pUnit->shouldPillage(pUnit->plot()))
					{
						pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Heal: pillage with %s before move, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strMsg);
						}
					}
				}

				pBetterPlot = TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
			}
		}

		//now finally do something
		if (pBetterPlot)
		{
			if (pBetterPlot != pUnit->plot())
			{
				CvUnit* pSwapUnit = pUnit->GetPotentialUnitToSwapWith(*pBetterPlot);
				int iDangerLimit = pSwapUnit ? (pSwapUnit->isRanged() ? pSwapUnit->GetCurrHitPoints() : (3 * pSwapUnit->GetCurrHitPoints()) / 2) : 0;
				if (pSwapUnit && pSwapUnit->GetDanger(pUnit->plot()) < iDangerLimit)
				{
					pSwapUnit->SetActivityType(ACTIVITY_AWAKE);
					pUnit->PushMission(CvTypes::getMISSION_SWAP_UNITS(), pBetterPlot->getX(), pBetterPlot->getY());
				}
				else //plot should be free
					ExecuteMoveToPlot(pUnit, pBetterPlot, true);
			}
		}
		else //no safe plot to heal ...
		{
			if (!bFirstPass && pUnit->getDomainType() != DOMAIN_AIR && pUnit->GetDanger() > 0)
			{
				//at least try to flee
				pBetterPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
				if (pBetterPlot && pBetterPlot != pUnit->plot())
					ExecuteMoveToPlot(pUnit, pBetterPlot);
			}
		}

		//why not pillage more tiles?
		if (pUnit->shouldPillage(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
			
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Heal: pillage with %s after move, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
				LogTacticalMessage(strMsg);
			}
		}

		//finish this up
		if (pUnit->canHeal(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_HEAL());
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Move barbarian to faraway targets 
void CvTacticalAI::ExecuteBarbarianRoaming()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit && pUnit->isBarbarian()) //combat and captured civilians both
		{
			CvString strTemp = pUnit->getUnitInfo().GetDescription();

			// LAND MOVES
			if(pUnit->getDomainType() == DOMAIN_LAND)
			{
				CvPlot* pPlot = pUnit->plot();
				if(pPlot && (pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT() || pPlot->isCity()))
				{
					pUnit->PushMission(CvTypes::getMISSION_SKIP());
					UnitProcessed(pUnit->GetID());
					//do it this way to avoid a warning
					pUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
					pUnit->setTacticalMove(AI_TACTICAL_BARBARIAN_CAMP);
					continue;
				}

				//where to?
				CvPlot* pBestPlot = FindBestBarbarianLandTarget(pUnit);
				if(!pBestPlot)
					continue;
					
				//civilian to capture?
				bool bMoved = false;
				if (pBestPlot->isEnemyUnit(BARBARIAN_PLAYER, false, true))
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
					bMoved = pUnit->at(pBestPlot->getX(), pBestPlot->getY());
				}
				else
					bMoved = MoveToEmptySpaceNearTarget(pUnit, pBestPlot, DOMAIN_LAND, 12);

				if (bMoved)
				{
					if (pUnit->shouldPillage(pUnit->plot()))
						pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

					TacticalAIHelpers::PerformOpportunityAttack(pUnit, true);
					UnitProcessed(m_CurrentMoveUnits[iI].GetID());
				}
			}
			// NAVAL MOVES
			else
			{
				CvPlot* pBestPlot = FindBestBarbarianSeaTarget(pUnit);
				//no naval pillaging, it's just too annoying
				if (MoveToEmptySpaceNearTarget(pUnit, pBestPlot, DOMAIN_SEA, 12))
				{
					TacticalAIHelpers::PerformOpportunityAttack(pUnit, true);
					UnitProcessed(m_CurrentMoveUnits[iI].GetID());
				}
			}
		}
	}
}

/// Move unit to a specific tile (unit passed explicitly)
bool CvTacticalAI::ExecuteMoveToPlot(CvUnit* pUnit, CvPlot* pTarget, bool bSaveMoves, int iFlags)
{
	bool bResult = false;

	if(!pUnit || !pTarget)
		return false;

	// Unit already at target plot?
	if(pTarget == pUnit->plot() && pUnit->canEndTurnAtPlot(pTarget))
	{
		bResult = true;

		TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit);
		pUnit->PushMission(CvTypes::getMISSION_SKIP());

		//don't call finish moves, otherwise we won't heal!
		if (!bSaveMoves)
			UnitProcessed(pUnit->GetID());
	}
	else if (pUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION) || (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING1) || (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING2))
	{
		if (pUnit->GeneratePath(pTarget,iFlags,INT_MAX,NULL,true))
		{
			//pillage if it makes sense and we have movement points to spare
			if (pUnit->shouldPillage(pUnit->plot(), true, pUnit->GetMovementPointsAtCachedTarget()))
			{
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
				
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Move To Plot: pillage with %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strMsg);
				}
			}

			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), iFlags, false, false, MISSIONAI_TACTMOVE, pTarget);

			if (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING2)
				bResult = (plotDistance(*pUnit->plot(),*pTarget)<3);
			else if  (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING1)
				bResult = (plotDistance(*pUnit->plot(),*pTarget)<2);
			else
				bResult = pUnit->at(pTarget->getX(), pTarget->getY());

			if (!bResult && pUnit->canMove()) //typically because of MOVEFLAG_ABORT_IN_DANGER
			{
				pTarget = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
				if (pTarget)
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), 0 /*no approximate flags*/, false, false, MISSIONAI_TACTMOVE, pTarget);
			}

			//for inspection in GUI
			pUnit->SetMissionAI(MISSIONAI_TACTMOVE,pTarget,NULL);

			if (!bSaveMoves || !pUnit->canMove())
				UnitProcessed(pUnit->GetID());
		}
		//maybe units are blocking our way? try to find a good plot in the direction of the target and hope the block clears
		else if (pUnit->GeneratePath(pTarget,iFlags|CvUnit::MOVEFLAG_IGNORE_STACKING,INT_MAX,NULL,true))
		{
			CvPlot* pWorkaround = pUnit->GetLastValidDestinationPlotInCachedPath();
			if (pWorkaround)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), iFlags, false, false, MISSIONAI_TACTMOVE, pTarget);
				//for inspection in GUI
				pUnit->SetMissionAI(MISSIONAI_TACTMOVE,pTarget,NULL);
				if (!bSaveMoves || !pUnit->canMove())
					UnitProcessed(pUnit->GetID());
			}
		}

		if(bResult && pUnit->canMove())
			TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit);
	}

	return bResult;
}

/// Find an adjacent hex to move a blocking unit to
bool CvTacticalAI::ExecuteMoveOfBlockingUnit(CvUnit* pBlockingUnit, CvPlot* pPreferredDirection)
{
	if(!pBlockingUnit->canMove())
	{
		return false;
	}

	CvPlot* pOldPlot = pBlockingUnit->plot();

	std::vector<SPlotWithScore> vCandidates;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pPlot = plotDirection(pBlockingUnit->getX(), pBlockingUnit->getY(), ((DirectionTypes)iI));
		if(pPlot != NULL)
		{
			if (pPreferredDirection)
				vCandidates.push_back( SPlotWithScore(pPlot,plotDistance(pPreferredDirection->getX(),pPreferredDirection->getY(),pPlot->getX(),pPlot->getY())) );
			else
				vCandidates.push_back( SPlotWithScore(pPlot,0) );
		}
	}

	std::stable_sort(vCandidates.begin(),vCandidates.end());

	for (std::vector<SPlotWithScore>::iterator it=vCandidates.begin(); it!=vCandidates.end(); ++it)
	{
		CvPlot* pPlot = it->pPlot;

		// Don't embark for one of these moves
		if (!pOldPlot->isWater() && pPlot->isWater() && pBlockingUnit->getDomainType() == DOMAIN_LAND)
		{
			continue;
		}

		// Has to be somewhere we can move and be empty of other units/enemy cities
		if(!pPlot->getVisibleEnemyDefender(m_pPlayer->GetID()) && !pPlot->isEnemyCity(*pBlockingUnit) && pBlockingUnit->GeneratePath(pPlot))
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
	return false;
}

/// Move unit to protect a specific tile
void CvTacticalAI::ExecuteNavalBlockadeMove(CvPlot* pTarget)
{
	if (!pTarget)
		return;

	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit && pUnit->canUseForTacticalAI())
		{
			//see if we can harrass the enemy first
			if (pUnit->shouldPillage(pUnit->plot()))
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
			TacticalAIHelpers::PerformOpportunityAttack(pUnit, true);

			//safety check
			if (pUnit->GetDanger(pTarget) <= pUnit->GetCurrHitPoints())
			{
				if (pUnit->GetDanger(pUnit->GetPathEndFirstTurnPlot()) <= pUnit->GetCurrHitPoints())
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Naval blockade at %d:%d with %s at %d:%d", pTarget->getX(), pTarget->getY(), pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg);
					}

					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

					//see if we can harrass the enemy now
					TacticalAIHelpers::PerformOpportunityAttack(pUnit, true);
					if (pUnit->shouldPillage(pUnit->plot()))
						pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

					UnitProcessed(pUnit->GetID());

					//one is enough?
					break;
				}
			}
		}
	}
}

/// Set up fighters to intercept enemy air units
void CvTacticalAI::ExecuteAirInterceptMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirPatrol(NULL))
			{
				pUnit->PushMission(CvTypes::getMISSION_AIRPATROL());
				UnitProcessed(m_CurrentMoveUnits[iI].GetID());
			}
		}
	}
}

/// Set up fighters to air sweep to suppress enemy air units/AA
void CvTacticalAI::ExecuteAirSweepMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirSweep())
			{
				CvPlot *pTarget = m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit);
				if (pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_AIR_SWEEP(), pTarget->getX(), pTarget->getY());
					pUnit->finishMoves();
					UnitProcessed(m_CurrentMoveUnits[iI].GetID());
				}
			}
		}
	}
}

/// Bombard enemy units from plots they can't reach (return true if some attack made)
bool CvTacticalAI::ExecuteSafeBombards(CvTacticalTarget& kTarget)
{
	//mark the target no matter if the attack succeeds
	kTarget.SetLastAggLvl(AL_LOW);

	//todo: how can we make sure that city and units focus fire on the same targets?
	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	CvUnit* pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if(pDefender)
	{
		if(pDefender->GetCurrHitPoints() <= 0)
			return false;

		// If this is a unit target we might also be able to hit it with a city
		bool bCityCanAttack = FindCitiesWithinStrikingDistance(pTargetPlot);
		if(bCityCanAttack)
		{
			// Start by applying damage from city bombards
			for(unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
			{
				CvCity* pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
				if(!pCity)
					continue;

				pCity->doTask(TASK_RANGED_ATTACK, pDefender->getX(), pDefender->getY(), 0);

				// Need to keep hitting the target?
				if (pDefender->GetCurrHitPoints() < 1)
					return true;
			}
		}

		//special handling for ranged garrison as it is ignored by FindUnitsWithinStrikingDistance()
		//note: melee garrison and advanced logic is handled in PlotGarrisonMoves()
		for(std::list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
			if(pUnit && pUnit->IsGarrisoned() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
			{
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), kTarget.GetTargetX(), kTarget.GetTargetY());

				//this invalidates our iterator so exit the loop immediately
				UnitProcessed(pUnit->GetID());
				break;
			}
		}

		// Need to keep hitting the target?
		if(pDefender->GetCurrHitPoints() < 1)
			return true;

		// Make attacks - this includes melee attacks but only very safe ones
		if (FindUnitsWithinStrikingDistance(pTargetPlot))
		{
			return ExecuteAttackWithUnits(pTargetPlot, AL_LOW);
		}
	}

	return false;
}

/// Take a multi-hex attack on an enemy unit this turn
bool CvTacticalAI::ExecuteFlankAttack(CvTacticalTarget& kTarget)
{
	//mark the target no matter if the attack succeeds
	kTarget.SetLastAggLvl(AL_MEDIUM);

	// Make attacks
	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	CvUnit* pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if (pDefender && FindUnitsWithinStrikingDistance(pTargetPlot))
		return ExecuteAttackWithUnits(pTargetPlot, AL_MEDIUM);

	return false;
}

/// Move units out of current dominance zone
void CvTacticalAI::ExecuteWithdrawMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit && pUnit->GetDanger()>0)
		{
			bool bMoveMade = false;
			CvPlot* pTargetPlot = NULL;

			// Allow withdraw to neighboring tactical zone which seems safe
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot());
			if (!pZone)
				continue;

			//todo: if we withdraw one unit, make sure we withdraw any neighboring units as well .. don't want to leave anyone behind!

			for (std::vector<int>::const_iterator it = pZone->GetNeighboringZones().begin(); it != pZone->GetNeighboringZones().end(); ++it)
			{
				CvTacticalDominanceZone* pNextZone = GetTacticalAnalysisMap()->GetZoneByID(*it);
				if (pNextZone && pNextZone->GetZoneCity() && pNextZone->IsWater()==(pUnit->getDomainType()==DOMAIN_SEA) &&
					(pNextZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY || pNextZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_EVEN))
				{
					pTargetPlot = GC.getMap().plot(pNextZone->GetCenterX(), pNextZone->GetCenterY());
					if (pUnit->CanSafelyReachInXTurns(pTargetPlot,12))
					{
						bMoveMade = MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, pUnit->getDomainType(), 12, true);
						break;
					}
				}
			}

			if (!bMoveMade)
			{
				// Compute moves to nearest city and use as sort criteria
				CvCity* pNearestCity = m_pPlayer->GetClosestCityByEstimatedTurns(pUnit->plot());
				if (m_pPlayer->isMinorCiv())
					pNearestCity = m_pPlayer->getCapitalCity();

				if (pNearestCity)
					pTargetPlot = pNearestCity->plot();

				if (pUnit->CanSafelyReachInXTurns(pTargetPlot, 12))
					bMoveMade = pUnit->IsCivilianUnit() ?
						ExecuteMoveToPlot(pUnit, pTargetPlot) :
						MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, pUnit->getDomainType(), 12, true);
			}

			if (bMoveMade)
			{
				UnitProcessed(m_CurrentMoveUnits[iI].GetID());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s %d withdrew from (%d,%d) towards (%d,%d)", 
						pUnit->getName().GetCString(),pUnit->GetID(),pUnit->getX(),pUnit->getY(),pTargetPlot->getX(),pTargetPlot->getY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}

	//now move all units which didn't find a path to a city
	ExecuteMovesToSafestPlot();
}

/// Move naval units on top of embarked units in danger
void CvTacticalAI::ExecuteEscortEmbarkedMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			CvPlot *pBestTarget = NULL;
			int iHighestDanger = -1;

			// Loop through all my embarked units that are: alone and within range
			CvUnit* pLoopUnit;
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
				else if (plotDistance(*pUnit->plot(), *pLoopUnit->plot()) > pUnit->baseMoves()*2)
				{
					continue;
				}
				else if(pLoopUnit->plot()->getBestDefender(m_pPlayer->GetID()))
				{
					continue;
				}
				//Ignore guys that can still move.
				else if(pLoopUnit->getMoves() > 0)
				{
					continue;
				}
			
				CvPlot *pTarget = pLoopUnit->plot();
				// Can this unit get to the embarked unit in two moves?
				int iTurns = pUnit->TurnsToReachTarget(pTarget);
				if (iTurns <= 1)
				{
					//note: civilian in danger have INT_MAX
					int iDanger = pUnit->GetDanger(pTarget);
					if (iDanger > iHighestDanger)
					{
						iHighestDanger = iDanger;
						pBestTarget = pTarget;
					}
				}
			}

			if (pBestTarget)
			{
				ExecuteMoveToPlot(pUnit, pBestTarget, true);

				//If we can shoot while doing this, do it!
				if (TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("%s escort opportunity range attack, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strLogString);
					}
				}

				UnitProcessed(m_CurrentMoveUnits[iI].GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s escorted embarked unit at, Current X: %d, Current Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
					LogTacticalMessage(strLogString);
				}
			}
		}
	}
}

// Get best plot of the array of possible plots, based on plot danger.
CvPlot* CvTacticalAI::GetBestRepositionPlot(CvUnit* pUnit, CvPlot* plotTarget, int iAcceptableDanger)
{
	//safety: barbarians don't leave camp
	if (pUnit->isBarbarian() && pUnit->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		return NULL;

	//don't pull units out of cities for repositioning
	if (pUnit->IsGarrisoned())
		return NULL;

	ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
	if (reachablePlots.empty())
		return NULL;

	CvCity* pTargetCity = plotTarget->getPlotCity();
	CvUnit* pTargetUnit = NULL;
	if (!pTargetCity)
		pTargetUnit = plotTarget->getBestDefender(NO_PLAYER, m_pPlayer->GetID());

	//done with the preparation, now start for real
	std::vector<SPlotWithTwoScoresL2> vStats;
	int iHighestAttack = 0;
	int iLowestDanger = INT_MAX;
	bool bIsRanged = pUnit->isRanged();

	for (ReachablePlots::iterator moveTile=reachablePlots.begin(); moveTile!=reachablePlots.end(); ++moveTile)
	{
		CvPlot* pMoveTile = GC.getMap().plotByIndexUnchecked(moveTile->iPlotIndex);

		//already occupied?
		if (!pUnit->canMoveInto(*pMoveTile,CvUnit::MOVEFLAG_DESTINATION ))
			continue;

		bool bBetterPass = false;
		if (bIsRanged)
		{
			//don't fly too close to the sun ...
			if ( pUnit->GetRange()>1 && plotDistance(*pMoveTile,*plotTarget)<2 )
				bBetterPass = true;
		}

		int iCurrentDanger = pUnit->GetDanger(pMoveTile);

		int iCurrentAttack = 0; //these methods take into account embarkation so we don't have to check for it
		if (bIsRanged && pUnit->canEverRangeStrikeAt(plotTarget->getX(),plotTarget->getY(),pMoveTile,false))
			iCurrentAttack = pUnit->GetMaxRangedCombatStrength(pTargetUnit, pTargetCity, true, pMoveTile, plotTarget);
		else if (!bIsRanged && (pUnit->GetNumEnemyUnitsAdjacent()>0 || pMoveTile->IsFriendlyUnitAdjacent(pUnit->getTeam(),true)) )
			iCurrentAttack = pUnit->GetMaxAttackStrength(pMoveTile, plotTarget, pTargetUnit);

		if (bBetterPass)
			iCurrentAttack /= 2;

		if (iCurrentDanger<=iAcceptableDanger && iCurrentAttack>0)
		{
			vStats.push_back( SPlotWithTwoScoresL2(pMoveTile,iCurrentAttack,iCurrentDanger) );

			iHighestAttack = max( iHighestAttack, iCurrentAttack );
			iLowestDanger = min( iLowestDanger, iCurrentDanger );
		}
	}

	//we want to find the best combination of attack potential and danger
	float fBestScore = 0;
	CvPlot* pBestRepositionPlot = NULL;
	for (std::vector<SPlotWithTwoScoresL2>::iterator it=vStats.begin(); it!=vStats.end(); ++it)
	{
		//be conservative: danger counts twice as much as attack strength
		float fScore = it->score1 / float(iHighestAttack) + 2 * float(iLowestDanger) / it->score2;

		if (fScore > fBestScore)
		{
			pBestRepositionPlot = it->pPlot;
			fBestScore = fScore;
		}
	}

	return pBestRepositionPlot;
}

#ifdef MOD_AI_SMART_AIR_TACTICS
//AMS: Fills m_CurrentAirSweepUnits with all units able to sweep at target plot.
void CvTacticalAI::FindAirUnitsToAirSweep(CvPlot* pTarget)
{
	m_CurrentAirSweepUnits.clear();
	int interceptionsOnPlot = pTarget->GetInterceptorCount(m_pPlayer->GetID(),NULL,false,true);

	// Loop through all units available to tactical AI this turn
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end() && interceptionsOnPlot > 0; ++it)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if (pLoopUnit && pLoopUnit->canUseForTacticalAI())
		{
			// Is an air unit.
			if (pLoopUnit->getDomainType() == DOMAIN_AIR && pLoopUnit->canMove())
			{
				// Is able to sweep at target
				if (pLoopUnit->canAirSweepAt(pTarget->getX(), pTarget->getY()))
				{
					int iAttackStrength = pLoopUnit->GetMaxRangedCombatStrength(pTarget->GetBestInterceptor(pLoopUnit->getOwner(),pLoopUnit,false,true),NULL,true,NULL,pTarget);
					// Mod to air sweep strength
					iAttackStrength *= (100 + pLoopUnit->GetAirSweepCombatModifier());
					iAttackStrength /= 100;
					CvTacticalUnit unit(pLoopUnit->GetID());
					unit.SetAttackStrength(iAttackStrength);
					unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
					m_CurrentAirSweepUnits.push_back(unit);

					interceptionsOnPlot--;
				}
			}
		}
	}

	std::stable_sort(m_CurrentAirSweepUnits.begin(), m_CurrentAirSweepUnits.end());
}
#endif

CvUnit* CvTacticalAI::FindUnitForThisMove(AITacticalMove eMove, CvPlot* pTarget, int iNumTurnsAway /* = -1 if any distance okay */)
{
	m_CurrentMoveUnits.clear();

	struct SUnitWithScore
	{
		CvUnit* unit;
		int score;
		SUnitWithScore(CvUnit* ptr, int i) : unit(ptr), score(i) {}
		bool operator<(const SUnitWithScore& rhs) const { return score<rhs.score; } //sort ascending
	};

	std::vector<SUnitWithScore> possibleUnits;

	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->getDomainType() != DOMAIN_AIR && pLoopUnit->IsCombatUnit() && !pLoopUnit->TurnProcessed())
		{
			// Make sure domain matches
			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
		        pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				continue;
			}

			if(!pLoopUnit->canMove() || !pLoopUnit->IsCanAttack())
			{
				continue;
			}

			if(pLoopUnit->IsGarrisoned() || pLoopUnit->AI_getUnitAIType()==UNITAI_EXPLORE || pLoopUnit->getArmyID() != -1)
			{
				continue;
			}

			//performance optimization ... careful because zero is a valid turn value
			if(iNumTurnsAway>1 && plotDistance(*pLoopUnit->plot(),*pTarget)>3*iNumTurnsAway)
			{
				continue;
			}

			int iExtraScore = 0;

			if(eMove == AI_TACTICAL_GARRISON)
			{
				// Want to put ranged units in cities to give them a ranged attack (but siege units should be used for offense)
				if (pLoopUnit->isRanged() && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_CITY_BOMBARD)==false)
					iExtraScore += 30;

				// Don't put units with a defense boosted from promotions in cities, these boosts are ignored
				iExtraScore -= pLoopUnit->getDefenseModifier();
			}

			else if(eMove == AI_TACTICAL_GUARD)
			{
				// No ranged units or units without defensive bonuses as plot defenders
				if (pLoopUnit->isRanged() || pLoopUnit->noDefensiveBonus())
					continue;

				// Units with defensive promotions are especially valuable
				if(pLoopUnit->getDefenseModifier() > 0 || pLoopUnit->getExtraRangedDefenseModifier() > 0)
					iExtraScore += 30;

				if (pLoopUnit->getExtraVisibilityRange() > 0)
					iExtraScore += 30;
			}
			else if(eMove == AI_TACTICAL_GOODY)
			{
				// Fast movers are top priority
				if(pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
					iExtraScore += 30;
			}

			//if we have a suitable unit in place already then use it
			if (pLoopUnit->atPlot(*pTarget))
				return pLoopUnit;

			//otherwise collect and sort
			int iTurns = pLoopUnit->TurnsToReachTarget(pTarget, false, false, (iNumTurnsAway == -1 ? MAX_INT : iNumTurnsAway));
			if(iTurns != MAX_INT)
			{
				int iScore = 100 - 10 * iTurns + iExtraScore;
				possibleUnits.push_back(SUnitWithScore(pLoopUnit, iScore));
			}
		}
	}

	if (possibleUnits.empty())
		return NULL;
	else
	{
		std::stable_sort(possibleUnits.begin(), possibleUnits.end());
		CheckDebugTrigger(possibleUnits.back().unit->GetID());
		return possibleUnits.back().unit;
	}
}

/// Fills m_CurrentMoveUnits with all units within X turns of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindUnitsWithinStrikingDistance(CvPlot* pTarget)
{
	m_CurrentMoveUnits.clear();

	bool rtnValue = false;
	bool bIsCityTarget = pTarget->isCity();
	bool bAirUnitsAdded = false;
	CvUnit* pDefender = pTarget->getBestDefender(NO_PLAYER, m_pPlayer->GetID());

	//todo: check if defender can be damaged at all or if an attacker would die?
	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(!pLoopUnit || !pLoopUnit->canUseForTacticalAI())
			continue;

		//Don't grab civilians for this!
		if (pLoopUnit->IsCivilianUnit() && !pLoopUnit->IsGreatGeneral())
			continue;

		//ignore most embarked units for attacking, that usually makes no sense
		bool bAmphibiousAttackPossible = pLoopUnit->plot()->isAdjacent(pTarget) && !pLoopUnit->isRanged() && pTarget->isCoastalLand();
		if(!pLoopUnit->canMove() || (pLoopUnit->isEmbarked() && !bAmphibiousAttackPossible) )
			continue;

		//Respect domain for melee units and subs
		if (!pLoopUnit->IsCanAttackRanged() || pLoopUnit->getUnitInfo().IsRangeAttackOnlyInDomain())
			if (pLoopUnit->getDomainType() != DOMAIN_AIR && !bIsCityTarget && pTarget->getDomain() != pLoopUnit->getDomainType())
				continue;

		// some naval units can only fire on coastal land tiles.
		if (pLoopUnit->getUnitInfo().IsCoastalFireOnly() && pTarget->getDomain() == DOMAIN_LAND && !pTarget->isCoastalLand())
			continue;
				
		// there are no other civilians here
		if (!bIsCityTarget && (pLoopUnit->IsCityAttackSupport() || pLoopUnit->IsGreatAdmiral() || pLoopUnit->IsGreatAdmiral()))
			continue;

		// Garrisons have special moves
		if (pLoopUnit->IsGarrisoned())
			continue;

		// Don't pull barbarian units out of camps to attack.
		if(pLoopUnit->isBarbarian() && (pLoopUnit->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT()))
			continue;

		// Some units can't enter cities
		if (pLoopUnit->isNoCapture() && bIsCityTarget)
			continue;

		// Don't bother with pathfinding if we're very far away
		if (plotDistance(*pLoopUnit->plot(),*pTarget) > pLoopUnit->baseMoves()*4)
			continue;

		//if it's a fighter plane, don't use it here, we need it for interceptions / sweeps
		if (pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_DEFENSE_AIR)
			continue;

		bool bCanReach = false;
		if ( pLoopUnit->IsCanAttackRanged() )
		{
			//can attack without moving ... for aircraft and other long-range units
			if (pLoopUnit->canRangeStrikeAt(pTarget->getX(), pTarget->getY()))
				bCanReach = true;
			else if (pLoopUnit->canMove() && pLoopUnit->getDomainType()!=DOMAIN_AIR)
			{
				//note that we also take units which can reach an attack plot but can only attack next turn. that's ok.
				ReachablePlots reachablePlots = TacticalAIHelpers::GetAllPlotsInReachThisTurn(pLoopUnit, pLoopUnit->plot(),
					CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_NO_EMBARK, 0);

				//start from the outside
				for (int i=pLoopUnit->GetRange(); i>0 && !bCanReach; i--)
				{
					std::vector<CvPlot*> vPlots;
					TacticalAIHelpers::GetPlotsForRangedAttack(pTarget,pLoopUnit,pLoopUnit->GetRange(),false,vPlots);

					for (std::vector<CvPlot*>::iterator it=vPlots.begin(); it!=vPlots.end() && !bCanReach; ++it)
						bCanReach = (reachablePlots.find( (*it)->GetPlotIndex() ) != reachablePlots.end());
				}
			}
		}
		else //melee. enough if we can get adjacent to the target
		{
			int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_IGNORE_STACKING;
			bCanReach = (pLoopUnit->TurnsToReachTarget(pTarget, iFlags, 0) <= 0);
		}

		if (!bCanReach)
			continue;

		if(pLoopUnit->IsCanAttackRanged())
		{
			// Will we do a significant amount of damage
			int iTargetHitpoints = pDefender ? pDefender->GetCurrHitPoints() : 0;
			if(IsExpectedToDamageWithRangedAttack(pLoopUnit, pTarget, MIN(iTargetHitpoints/20, 3)))
			{
				//first-line ranged and air
				CvTacticalUnit unit(pLoopUnit->GetID());
				if (bIsCityTarget)
					unit.SetAttackStrength(pLoopUnit->GetMaxRangedCombatStrength(NULL, pTarget->getPlotCity(), true, NULL, NULL, true, true));
				else
					unit.SetAttackStrength(pLoopUnit->GetMaxRangedCombatStrength(pDefender, NULL, true, NULL, NULL, true, true));

				unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
				m_CurrentMoveUnits.push_back(unit);
				rtnValue = true;

				if (pLoopUnit->getDomainType()==DOMAIN_AIR)
					bAirUnitsAdded = true;
			}
		}
		else //melee
		{
			int iAttackStrength = pLoopUnit->GetMaxAttackStrength(NULL, pTarget, bIsCityTarget ? NULL : pDefender, true, true);

			CvTacticalUnit unit(pLoopUnit->GetID());
			unit.SetAttackStrength(iAttackStrength);
			unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
			m_CurrentMoveUnits.push_back(unit);
			rtnValue = true;
		}
	}

	// As we have air units on the attack targets we should also check possible air sweeps
	if (bAirUnitsAdded)
		FindAirUnitsToAirSweep(pTarget);
	else
		m_CurrentAirSweepUnits.clear();

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());
	//if (m_CurrentMoveUnits.size()>0)
	//	OutputDebugString( CvString::format("player %d recruited %d units for possible attack on %d:%d (zone id %d)\n",m_pPlayer->GetID(),m_CurrentMoveUnits.size(),pTarget->getX(),pTarget->getY(),m_iCurrentZoneID ).c_str() );
	return rtnValue;
}

/// Fills m_CurrentMoveCities with all cities within bombard range of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindCitiesWithinStrikingDistance(CvPlot* pTargetPlot)
{
	m_CurrentMoveCities.clear();

	// Loop through all of our cities
	int iLoop;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()) && !pLoopCity->isMadeAttack())
		{
			CvTacticalCity city;
			city.SetID(pLoopCity->GetID());
			m_CurrentMoveCities.push_back(city);
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveCities.begin(), m_CurrentMoveCities.end());

	return !m_CurrentMoveCities.empty();
}


bool CvTacticalAI::FindEmbarkedUnitsAroundTarget(CvPlot* pTarget, int iMaxDistance)
{
	m_CurrentMoveUnits.clear();

	if (!pTarget)
		return false;

	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->canUseForTacticalAI() && pLoopUnit->IsCombatUnit() && pLoopUnit->isEmbarked() && plotDistance(*pLoopUnit->plot(),*pTarget)<=iMaxDistance )
		{
			CvTacticalUnit unit(pLoopUnit->GetID());
			unit.SetAttackStrength(pLoopUnit->GetBaseCombatStrength());
			unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return m_CurrentMoveUnits.size()>0;
}


/// Fills m_CurrentMoveUnits with all paratrooper units (available to jump) to the target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindParatroopersWithinStrikingDistance(CvPlot* pTarget, bool bCheckDanger)
{
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->canUseForTacticalAI() && 
			pLoopUnit->canParadropAt(pLoopUnit->plot(), pTarget->getX(), pTarget->getY()) &&
			(!bCheckDanger || pLoopUnit->GetDanger(pTarget) < pLoopUnit->GetCurrHitPoints()))
		{
			CvTacticalUnit unit(pLoopUnit->GetID());
			unit.SetAttackStrength(pLoopUnit->GetBaseCombatStrength());
			unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return m_CurrentMoveUnits.size()>0;
}

//find units for pillaging, plundering, blockading, etc
bool CvTacticalAI::FindUnitsForHarassing(CvPlot* pTarget, int iNumTurnsAway, int iMinHitpoints, int iMaxHitpoints, DomainTypes eDomain, bool bMustHaveMovesLeft)
{
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->canUseForTacticalAI() && pLoopUnit->IsCombatUnit()) //ignore generals and the like!
		{
			//these units are too fragile for the moves we have in mind
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_CITY_BOMBARD || pLoopUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA)
				continue;

			if (pLoopUnit->IsGarrisoned() || pLoopUnit->getArmyID()!=-1)
				continue;

			if (pLoopUnit->isBarbarian() && pLoopUnit->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				continue;

			if (eDomain != NO_DOMAIN && pLoopUnit->getDomainType() != eDomain)
				continue;

			if (pLoopUnit->GetDanger(pTarget) > pLoopUnit->GetCurrHitPoints())
				continue;

			if(iMinHitpoints>0 && pLoopUnit->GetCurrHitPoints()<iMinHitpoints)
				continue;

			if(iMaxHitpoints>0 && pLoopUnit->GetCurrHitPoints()>iMaxHitpoints)
				continue;

			//performance optimization
			if (plotDistance(*pTarget, *pLoopUnit->plot())>(iNumTurnsAway + 1) * 3)
				continue;

			int iFlags = CvUnit::MOVEFLAG_NO_EMBARK;
			if (bMustHaveMovesLeft)
				iFlags |= CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN;

			int iTurnsCalculated = pLoopUnit->TurnsToReachTarget(pTarget, iFlags, iNumTurnsAway);
			if (iTurnsCalculated <= iNumTurnsAway)
			{
				CvTacticalUnit unit(pLoopUnit->GetID());
				unit.SetAttackStrength(1 + iNumTurnsAway - iTurnsCalculated);
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return m_CurrentMoveUnits.size()>0;
}

// search radius for units depending on map size and game turn
int CvTacticalAI::GetRecruitRange() const
{
	int iResult = m_iRecruitRange;
	//add some for duration
	iResult += (4*GC.getGame().getGameTurn()) / max(400, GC.getGame().getMaxTurns());
	//add some for map size
	if (GC.getMap().getWorldSize() == WORLDSIZE_LARGE)
		iResult += 1;
	if (GC.getMap().getWorldSize() == WORLDSIZE_HUGE)
		iResult += 2;

	return iResult;
}

/// Estimates the damage we can apply to a target
int CvTacticalAI::ComputeTotalExpectedDamage(CvTacticalTarget* pTarget, CvPlot* pTargetPlot)
{
	int rtnValue = 0;
	int iMeleeCount = 0;

	// Loop through all units who can reach the target
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pAttacker = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());

		// Is target a unit?
		switch(pTarget->GetTargetType())
		{
		case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
		case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
		{
			CvUnit* pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if (pDefender)
			{
				int iSelfDamage = 0;
				//attacker plot will likely change but this is just an estimation anyway
				int iDamage = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pAttacker, pTargetPlot, pAttacker->plot(), iSelfDamage, true, 0, true);
				if (iDamage > iSelfDamage) //exclude suicidal melee attacks
				{
					m_CurrentMoveUnits[iI].SetExpectedTargetDamage(iDamage);
					m_CurrentMoveUnits[iI].SetExpectedSelfDamage(iSelfDamage);

					//if we have a lot of melee units, don't assume they all can be executed at once
					if (!pAttacker->isRanged() && !pAttacker->canMoveAfterAttacking())
					{
						if (iMeleeCount<3)
							rtnValue += iDamage;
						iMeleeCount++;
					}
					else
						rtnValue += iDamage;
				}
			}
		}
		break;

		case AI_TACTICAL_TARGET_CITY:
		{
			CvCity* pCity = pTargetPlot->getPlotCity();
			if(pCity != NULL)
			{
				int iSelfDamage = 0;
				//attacker plot will likely change but this is just an estimation anyway
				int iDamage = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pCity, pAttacker, pAttacker->plot(), iSelfDamage, true, 0, true);
				if (iDamage > iSelfDamage || (iDamage*2 > iSelfDamage && pAttacker->GetCurrHitPoints()-iSelfDamage > pAttacker->GetMaxHitPoints()/2)) //exclude suicidal melee attacks
				{
					//if the city has a garrison, it will absorb part of the damage
					//let's pretend it doesn't die, this is just an estimation after all
					if (pCity->HasGarrison())
					{
						CvUnit* pGarrison = pCity->GetGarrisonedUnit();
						if (pGarrison)
						{
							int iGarrisonShare = (iDamage*2*pGarrison->GetMaxHitPoints()) / (pCity->GetMaxHitPoints()+2*pGarrison->GetMaxHitPoints());
							iDamage -= iGarrisonShare;
						}
					}

					m_CurrentMoveUnits[iI].SetExpectedTargetDamage(iDamage);
					m_CurrentMoveUnits[iI].SetExpectedSelfDamage(iSelfDamage);
					rtnValue += iDamage;
				}
			}
		}
		break;
		}
	}

	//sort by expected damage to target!
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end(), TacticalAIHelpers::SortByExpectedTargetDamageDescending);

	return rtnValue;
}

/// Estimates the bombard damage we can apply to a target
int CvTacticalAI::ComputeTotalExpectedCityBombardDamage(CvUnit* pTarget)
{
	int rtnValue = 0;
	int iExpectedDamage;

	// Now loop through all the cities that can bombard it
	for(unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
	{
		CvCity* pAttackingCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		iExpectedDamage = pAttackingCity->rangeCombatDamage(pTarget, NULL, false);
		m_CurrentMoveCities[iI].SetExpectedTargetDamage(iExpectedDamage);
		rtnValue += iExpectedDamage;
	}

	return rtnValue;
}

bool CvTacticalAI::IsExpectedToDamageWithRangedAttack(CvUnit* pAttacker, CvPlot* pTargetPlot, int iMinDamage)
{
	int iExpectedDamage = 0;

	if(pTargetPlot->isCity())
	{
		CvCity* pCity = pTargetPlot->getPlotCity();
		iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, /*bIncludeRand*/ false, 0, NULL, NULL, true, true);
	}
	else
	{
		CvUnit* pDefender = pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID());
		if(pDefender)
		{
			iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender, NULL, false, 0, NULL, NULL, true, true);
		}
	}

	return iExpectedDamage >= iMinDamage;
}

/// Move up close to our target avoiding our own units if possible
bool CvTacticalAI::MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, DomainTypes eDomain, int iMaxTurns, bool bMustBeSafePath)
{
	if (!pUnit || !pTarget)
		return false;

	int iFlags = 0;
	//can we move there directly? if not try to move to an adjacent plot
	if (!pUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION))
		iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
	if (eDomain==pTarget->getDomain())
		iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
	if (bMustBeSafePath)
		iFlags |= CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER;

	int iTurns = pUnit->TurnsToReachTarget(pTarget,iFlags,iMaxTurns);

	//if not possible, try again with more leeway
	if (iTurns==INT_MAX)
	{
		iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2;
		if (eDomain==pTarget->getDomain())
			iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
		iTurns = pUnit->TurnsToReachTarget(pTarget,iFlags,iMaxTurns);
	}

	if (iTurns <= iMaxTurns)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), iFlags);

		//for inspection in GUI
		pUnit->SetMissionAI(MISSIONAI_TACTMOVE,pTarget,NULL);

		TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);

		//don't call finish moves, otherwise we won't heal!
		if (pUnit->canMove())
			pUnit->PushMission(CvTypes::getMISSION_SKIP());

		return true;
	}

	return false;
}

/// Find a multi-turn target for a land barbarian to wander towards
CvPlot* CvTacticalAI::FindBestBarbarianLandTarget(CvUnit* pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	
	// combat units look at all offensive targets within x turns
	if (pUnit->IsCanDefend())
	{
		pBestMovePlot = FindNearbyTarget(pUnit, m_iLandBarbarianRange, true);

		// alternatively explore
		if (pBestMovePlot == NULL)
			pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	// by default go back to camp or so
	if (pBestMovePlot == NULL)
		pBestMovePlot = FindNearbyTarget(pUnit, m_iLandBarbarianRange, false);

	return pBestMovePlot;
}

/// Find a multi-turn target for a sea barbarian to wander towards
CvPlot* CvTacticalAI::FindBestBarbarianSeaTarget(CvUnit* pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue = MAX_INT;

	SPathFinderUserData data(pUnit, 0, m_iSeaBarbarianRange);
	data.ePathType = PT_UNIT_REACHABLE_PLOTS;
	ReachablePlots movePlots = GC.GetPathFinder().GetPlotsInReach(pUnit->plot(), data);

	// Loop through all unit targets to find the closest
	for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		ReachablePlots::const_iterator itPlot = movePlots.find(pPlot->GetPlotIndex());
		if (itPlot != movePlots.end() && itPlot->iTurns < iBestValue)
		{
			iBestValue = itPlot->iTurns;
			pBestMovePlot = pPlot;
		}
	}

	// No units to pick on, so sail to a tile adjacent to the second closest barbarian camp
	if(pBestMovePlot == NULL)
	{
		CvPlot* pNearestCamp = NULL;
		int iBestCampDistance = MAX_INT;

		// Start by finding the very nearest camp
		for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP); pTarget!=NULL; pTarget = GetNextZoneTarget())
		{
			CvPlot* pCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if (pCamp->isAdjacentToShallowWater())
			{
				int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->GetTargetX(), pTarget->GetTargetY());
				if (iDistance < iBestCampDistance)
				{
					pNearestCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
					iBestCampDistance = iDistance;
				}
			}
		}

		// Try to sail to the second closest camp - this should result in patrolling behavior
		for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP); pTarget!=NULL; pTarget = GetNextZoneTarget())
		{
			CvPlot* pCamp = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if(pCamp != pNearestCamp && pCamp->isAdjacentToShallowWater())
			{
				for (ReachablePlots::const_iterator it = movePlots.begin(); it != movePlots.end(); ++it)
				{
					CvPlot* pTestPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
					if (plotDistance(*pTestPlot, *pCamp) == 1)
					{
						int iValue = it->iTurns;
						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestMovePlot = pTestPlot;
						}
					}
				}
			}
		}
	}

	// No obvious target ... next try
	if (pBestMovePlot == NULL)
		pBestMovePlot = FindBarbarianExploreTarget(pUnit);

	return pBestMovePlot;
}

/// Scan nearby tiles for the best choice, borrowing code from the explore AI
CvPlot* CvTacticalAI::FindBarbarianExploreTarget(CvUnit* pUnit)
{
	CvPlot* pBestMovePlot = 0;
	int iBestValue = 0;

	ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
	for (ReachablePlots::iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
	{
		CvPlot* pConsiderPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		if (pUnit->atPlot(*pConsiderPlot))
			continue;

		if (!pConsiderPlot->isRevealed(pUnit->getTeam()))
			continue;

		// Value them based on their explore value
#if defined(MOD_CORE_ALTERNATIVE_EXPLORE_SCORE)
		int iValue = EconomicAIHelpers::ScoreExplorePlot2(pConsiderPlot, m_pPlayer, pUnit->getDomainType(), false);
#else
		int iValue = CvEconomicAI::ScoreExplorePlot(pConsiderPlot, pUnit->getTeam(), pUnit->getUnitInfo().GetBaseSightRange(), eDomain);
#endif

		// disembark if possible
		if (pUnit->isNativeDomain(pConsiderPlot))
		{
			iValue += 200;
		}

		// Add special value enemy lands
		if (pConsiderPlot->isAdjacentOwned() || pConsiderPlot->isOwned())
		{
			iValue += 100;
		}

		//magic knowledge - gravitate towards cities
		int iCityDistance = GC.getGame().GetClosestCityDistanceInTurns(pConsiderPlot);
		if (iCityDistance<10)
			iValue += (10 - iCityDistance);

		// If still have no value, score equal to distance from my current plot
		if (iValue == 0)
			iValue = plotDistance(pUnit->getX(), pUnit->getY(), pConsiderPlot->getX(), pConsiderPlot->getY());

		if (iValue > iBestValue || (!GC.getGame().isGameMultiPlayer() && iValue == iBestValue && GC.getGame().getSmallFakeRandNum(3, *pConsiderPlot) == 0))
		{
			pBestMovePlot = pConsiderPlot;
			iBestValue = iValue;
		}
	}

	return pBestMovePlot;
}

#if defined(MOD_BALANCE_CORE_MILITARY)
/// Do we want to move this air unit to a new base?
bool CvTacticalAI::ShouldRebase(CvUnit* pUnit) const
{
	if (!pUnit || pUnit->getDomainType()!=DOMAIN_AIR)
		return false;

	CvPlot* pUnitPlot = pUnit->plot();
	if (!pUnitPlot)
		return false;

	// Is this unit in a base in danger?
	if (pUnitPlot->isCity())
	{
		if (pUnitPlot->getPlotCity()->isInDangerOfFalling())
			return true;

		if (pUnit->shouldHeal() && m_pPlayer->GetPlotDanger(pUnitPlot->getPlotCity())>0)
			return true;
	}
	else
	{
		CvUnit *pCarrier = pUnit->getTransportUnit();
		if (pCarrier && pCarrier->isProjectedToDieNextTurn())
			return true;

		if (pUnit->shouldHeal() && pCarrier->GetDanger(pUnitPlot)>0)
			return true;
	}

	bool bIsNeeded = false;
	if (!m_pPlayer->GetPlayersAtWarWith().empty())
	{
		switch (pUnit->getUnitInfo().GetDefaultUnitAIType())
		{
		case UNITAI_DEFENSE_AIR:
			// Is this a fighter that doesn't have any useful missions nearby
			{
				int iNumNearbyEnemyAirUnits = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), true /*bCountFighters*/, true /*bCountBombers*/);
				if (iNumNearbyEnemyAirUnits > 0  || m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit))
				{
					bIsNeeded = true;
				}
			}
			break;
		case UNITAI_ATTACK_AIR:
		case UNITAI_ICBM:
		case UNITAI_MISSILE_AIR:
			//Is this a bomber or a missile that lacks useful target?
			{
				//check for targets in tactical map
				for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
				{
					// If it's a nuke, we only want city targets
					if (pUnit->canNuke())
					{
						if (m_AllTargets[iI].GetTargetType() != AI_TACTICAL_TARGET_CITY)
							continue;
						//if the city is already weak, no point in nuking it
						CvPlot* pTargetPlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
						if (pTargetPlot->getPlotCity()->isInDangerOfFalling())
							continue;
					}

					// Is the target of an appropriate type?
					if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
							m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
							m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
					{
						// Is this target near enough?
						if(plotDistance(pUnit->getX(), pUnit->getY(), m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY()) <= pUnit->GetRange())
						{
							bIsNeeded = true;
							break;
						}
					}
				}
			}
			break;
		}

	}

	return !bIsNeeded;
}
#endif

// Find a faraway target for a unit to wander towards
// Can be either a specific type or any offensive type
// Returns the closest matching target that is reachable for the unit
CvPlot* CvTacticalAI::FindNearbyTarget(CvUnit* pUnit, int iMaxTurns, bool bOffensive)
{
	if (pUnit == NULL)
		return NULL;

	vector<OptionWithScore<CvPlot*>> candidates;

	// Loop through all appropriate targets to find the closest
	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		CvTacticalTarget target = m_AllTargets[iI];

		// Is the target of an appropriate type?
		bool bTypeMatch = false;
		if(bOffensive)
		{
			if (target.GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
				target.GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
				target.GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
				target.GetTargetType() == AI_TACTICAL_TARGET_CITY)
			{
				bTypeMatch = !TacticalAIHelpers::IsSuicideMeleeAttack(pUnit, GC.getMap().plotUnchecked(target.GetTargetX(), target.GetTargetY()));
			}

			if (target.GetTargetType() == AI_TACTICAL_TARGET_IMPROVEMENT ||
				target.GetTargetType() == AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE ||
				(target.GetTargetType() == AI_TACTICAL_TARGET_TRADE_UNIT_LAND && pUnit->getDomainType()==DOMAIN_LAND) ||
				(target.GetTargetType() == AI_TACTICAL_TARGET_TRADE_UNIT_SEA && pUnit->getDomainType()==DOMAIN_SEA) ||
 				target.GetTargetType() == AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN ||
 				target.GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN ||
 				target.GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN ||
				target.GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN )
			{
				bTypeMatch = true;
			}
		}
		else //defensive targets
		{
			if (target.GetTargetType() == AI_TACTICAL_TARGET_CITY_TO_DEFEND ||
				(pUnit->isBarbarian() && target.GetTargetType() == AI_TACTICAL_TARGET_BARBARIAN_CAMP))
			{
				bTypeMatch = true;
			}
		}

		// Is this unit near enough?
		if (bTypeMatch)
		{
			CvPlot* pPlot = GC.getMap().plot(target.GetTargetX(), target.GetTargetY());
			if (!pPlot)
				continue;

			if (plotDistance(target.GetTargetX(), target.GetTargetY(),pUnit->getX(),pUnit->getY()) > iMaxTurns*3)
				continue;

			//can't do anything if we would need to embark
			if (pPlot->needsEmbarkation(pUnit))
				continue;

			//Ranged naval unit? Let's get a water plot (naval melee can enter cities, don't care for others)
			if (!pPlot->isWater() && pUnit->isRanged() && pUnit->getDomainType() == DOMAIN_SEA)
			{
				pPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pPlot);
				if (!pPlot)
					continue;
			}
	
			//shortcut, may happen often (do this after the domain checks so don't accidentally get stuck in the wrong domain)
			if (pUnit->plot() == pPlot)
				return pPlot;

			candidates.push_back(OptionWithScore<CvPlot*>(pPlot, plotDistance(*pPlot, *pUnit->plot())));
		}
	}

	//second round. default sort order is descending
	std::sort(candidates.begin(), candidates.end());
	std::reverse(candidates.begin(), candidates.end());

	for (size_t i=0; i<candidates.size(); i++)
	{
		CvPlot* pPlot = candidates[i].option;
		if ( pUnit->TurnsToReachTarget(pPlot,0,iMaxTurns) < INT_MAX )
			return pPlot;
	}

	return NULL;
}


/// Am I within range of an enemy?
bool CvTacticalAI::NearVisibleEnemy(CvUnit* pUnit, int iRange)
{
	int iLoop;

	// Loop through enemies
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
		{
			// Loop through their units
			for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
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
			for(CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
#if defined(MOD_BALANCE_CORE_MILITARY)
				// Cities can't move, revealed is good enough
				if(pLoopCity->plot()->isRevealed(m_pPlayer->getTeam()))
#else
				// Make sure this tile is visible to us
				if(pLoopCity->plot()->isVisible(m_pPlayer->getTeam()))
#endif
				{
					// Check distance
					if(plotDistance(pLoopCity->getX(), pLoopCity->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
					{
						return true;
					}
				}
			}
		}
		
		//additionally check barb camps
		if(kPlayer.isBarbarian())
		{
			for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
				if(pLoopPlot == NULL)
					continue;

				if(pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) != GC.getBARBARIAN_CAMP_IMPROVEMENT())
					continue;

				if(plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Remove a unit that we've allocated from list of units to move this turn
void CvTacticalAI::UnitProcessed(int iID)
{
	m_CurrentTurnUnits.remove(iID);

	CvUnit* pUnit = m_pPlayer->getUnit(iID);
	if (!pUnit)
		return;

	if (iID==gCurrentUnitToTrack)
	{
		CvPlayer& owner = GET_PLAYER(pUnit->getOwner());
		OutputDebugString( CvString::format("turn %03d: used %s %s %d for tactical move %s. hitpoints %d, pos (%d,%d), danger %d\n", 
			GC.getGame().getGameTurn(), owner.getCivilizationAdjective(), pUnit->getName().c_str(), gCurrentUnitToTrack,
			tacticalMoveNames[m_CurrentMoveUnits.getCurrentTacticalMove()], 
			pUnit->GetCurrHitPoints(), pUnit->getX(), pUnit->getY(), pUnit->GetDanger() ) );
	}

	pUnit->setTacticalMove(m_CurrentMoveUnits.getCurrentTacticalMove());
	pUnit->SetTurnProcessed(true);
}

/// Do we want to process moves for this dominance zone?
bool CvTacticalAI::UseThisDominanceZone(CvTacticalDominanceZone* pZone)
{
	bool bTempZone = false;
	if(pZone->GetZoneCity() != NULL)
		bTempZone = IsTemporaryZoneCity(pZone->GetZoneCity());

	bool bWeHaveUnitsNearEnemy = pZone->GetTotalFriendlyUnitCount() > 0 && pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY;
	bool bTheyHaveUnitsNearUs = pZone->GetTotalEnemyUnitCount() > 0 && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY;
	bool bBothHaveUnits = pZone->GetTotalFriendlyUnitCount() > 0 && pZone->GetTotalEnemyUnitCount() > 0;

	return (bTempZone || bWeHaveUnitsNearEnemy || bTheyHaveUnitsNearUs || bBothHaveUnits);
}

/// Is this civilian target of the highest priority?
bool CvTacticalAI::IsVeryHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = pTarget->GetUnitPtr();
	if(pUnit)
	{
		if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral() || pUnit->IsCityAttackSupport())
			bRtnValue = true;
	}
	return bRtnValue;
}

/// Is this civilian target of high priority?
bool CvTacticalAI::IsHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = pTarget->GetUnitPtr();
	if (pUnit && pUnit->IsCivilianUnit())
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
				if(!pUnit->isBarbarian() && pUnit->plot()->GetAdjacentCity() != NULL)
				{
					if(pUnit->plot()->GetAdjacentCity()->getOwner() == m_pPlayer->GetID())
					{
						bRtnValue = false;
					}
				}
				if(!pUnit->isBarbarian())
				{
					//Is he trying to lure us to come out of a garrison?
					for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot *pLoopPlot = plotDirection(pUnit->plot()->getX(), pUnit->plot()->getY(), ((DirectionTypes)iI));
						if (pLoopPlot != NULL)
						{
							if((pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false) > 10) && pLoopPlot->getNumUnits() > 0)
							{
								if(pLoopPlot->getUnitByIndex(0)->getOwner() == m_pPlayer->GetID())
								{
									bRtnValue = false;
								}
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
	CvUnit* pUnit = pTarget->GetUnitPtr();
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
						if((pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false) > 15) && pLoopPlot->getNumUnits() > 0)
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
void CvTacticalAI::LogTacticalMessage(const CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		strPlayerName.Replace(' ', '_'); //no spaces
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite );

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
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

// HELPER FUNCTIONS
bool TacticalAIHelpers::SortByExpectedTargetDamageDescending(const CvTacticalUnit& obj1, const CvTacticalUnit& obj2)
{
	return obj1.GetExpectedTargetDamage()*2-obj1.GetExpectedSelfDamage() > obj2.GetExpectedTargetDamage()*2-obj2.GetExpectedSelfDamage();
}

ReachablePlots TacticalAIHelpers::GetAllPlotsInReachThisTurn(const CvUnit* pUnit, const CvPlot* pStartPlot, int iFlags, int iMinMovesLeft, int iStartMoves, const PlotIndexContainer& plotsToIgnoreForZOC)
{
	if (!pStartPlot)
		return ReachablePlots();

	if (!plotsToIgnoreForZOC.empty())
		iFlags |= CvUnit::MOVEFLAG_SELECTIVE_ZOC;

	SPathFinderUserData data(pUnit, iFlags, 1);
	data.ePathType = PT_UNIT_REACHABLE_PLOTS;
	data.iMinMovesLeft = iMinMovesLeft;
	if (iStartMoves>-1) //overwrite this only if we have a sane value
		data.iStartMoves = iStartMoves;
	data.plotsToIgnoreForZOC = plotsToIgnoreForZOC;

	return GC.GetPathFinder().GetPlotsInReach(pStartPlot->getX(), pStartPlot->getY(), data);
}

vector<int> TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, const CvPlot* pBasePlot, bool bOnlyWithEnemy, bool bIgnoreVisibility)
{
	vector<int> resultSet;

	if (!pUnit || !pBasePlot)
		return resultSet;

	int iRange = min(5,max(1,pUnit->GetRange()));
	for(int i=1; i<RING_PLOTS[iRange]; i++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pBasePlot,i);
		if (!pLoopPlot)
			continue;

		if (!bOnlyWithEnemy || pLoopPlot->isEnemyCity(*pUnit) || pLoopPlot->isEnemyUnit(pUnit->getOwner(),true,!bIgnoreVisibility))
			if (pUnit->canEverRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY(), pBasePlot, bIgnoreVisibility))
				resultSet.push_back(pLoopPlot->GetPlotIndex());
	}

	return resultSet;
}

std::set<int> TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, ReachablePlots& basePlots, bool bOnlyWithEnemy, bool bIgnoreVisibility)
{
	std::set<int> resultSet;

	if (!pUnit || !pUnit->IsCanAttackRanged())
		return resultSet;

	int iRange = min(5,max(1,pUnit->GetRange()));
	for (ReachablePlots::iterator base=basePlots.begin(); base!=basePlots.end(); ++base)
	{
		CvPlot* pBasePlot = GC.getMap().plotByIndexUnchecked( base->iPlotIndex );

		int iPlotMoves = base->iMovesLeft;
		if (iPlotMoves<=0)
			continue;

		//can't shoot if embarked
		if (!pUnit->isNativeDomain(pBasePlot))
			continue;

		//we have enough moves for an attack ...
		for(int i=1; i<RING_PLOTS[iRange]; i++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pBasePlot,i);

			//if the plot is already know to be attackable, don't check again
			//the reverse is not true: from another base plot the attack might work!
			if (!pLoopPlot || resultSet.find(pLoopPlot->GetPlotIndex())!=resultSet.end())
				continue;

			if (!bOnlyWithEnemy || pLoopPlot->isEnemyCity(*pUnit) || pLoopPlot->isEnemyUnit(pUnit->getOwner(),true,!bIgnoreVisibility))
				if (pUnit->canEverRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY(), pBasePlot, bIgnoreVisibility))
					resultSet.insert(pLoopPlot->GetPlotIndex());
		}
	}

	return resultSet;
}

bool TacticalAIHelpers::IsAttackNetPositive(CvUnit* pUnit, const CvPlot* pTargetPlot)
{
	if (!pUnit || !pTargetPlot)
		return false;

	//target can be city or a unit
	CvCity* pTargetCity = pTargetPlot->getPlotCity();
	CvUnit* pTargetUnit = pTargetPlot->getBestDefender( NO_PLAYER, pUnit->getOwner(), pUnit);

	int iDamageDealt = 0, iDamageReceived = 1;
	if (pTargetCity)
	{
		//+2 to make sure it's positive if city has zero hitpoints left
		iDamageDealt = GetSimulatedDamageFromAttackOnCity(pTargetCity, pUnit, pUnit->plot(), iDamageReceived) + 2;
		return (iDamageDealt > iDamageReceived || iDamageDealt == pTargetCity->GetMaxHitPoints()-pTargetCity->getDamage());
	}
	else if (pTargetUnit)
	{
		iDamageDealt = GetSimulatedDamageFromAttackOnUnit(pTargetUnit, pUnit, pTargetUnit->plot(), pUnit->plot(), iDamageReceived);
		return (iDamageDealt > iDamageReceived || iDamageDealt == pTargetUnit->GetCurrHitPoints());
	}

	return false;
}

//see if there is a possible target around the unit
bool TacticalAIHelpers::PerformOpportunityAttack(CvUnit* pUnit, bool bAllowMovement)
{
	if (!pUnit || !pUnit->IsCanAttack() || !pUnit->canMove())
		return false;

	//for ranged we have a readymade method
	if (pUnit->IsCanAttackRanged())
		return TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit, bAllowMovement);

	//where can we go
	CvPlot* pOrigin = pUnit->plot();
	vector<CvPlot*> testPlots;
	if (bAllowMovement)
	{
		ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
		for (ReachablePlots::const_iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
		{
			CvPlot *pTestPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
			testPlots.push_back(pTestPlot);
		}
	}
	else
	{
		//simply check all adjacent plots. if the unit is a garrison or the attack is not a kill, it will not advance after attacking
		for (int i = RING0_PLOTS; i < RING1_PLOTS; i++)
		{
			CvPlot* pTestPlot = iterateRingPlots(pOrigin, i);
			if (pTestPlot && pUnit->canMoveInto(*pTestPlot,CvUnit::MOVEFLAG_ATTACK))
				testPlots.push_back(pTestPlot);
		}
	}

	//what can we do?
	vector<SPlotWithScore> meleeTargets;
	int iScoreThreshold = 0;
	for (size_t i = 0; i < testPlots.size(); i++)
	{
		CvPlot* pTestPlot = testPlots[i];
		if (!pTestPlot || pTestPlot->isCity())
			continue;

		//attack an enemy?
		if (pTestPlot->isEnemyUnit(pUnit->getOwner(), true, true) && !pUnit->isOutOfAttacks())
		{
			CvUnit* pEnemy = pTestPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(), pUnit, true);
			int iDamageReceived = 0;
			int iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pEnemy, pUnit, pTestPlot, pOrigin, iDamageReceived);

			//no suicide
			if (iDamageReceived >= pUnit->GetCurrHitPoints())
				continue;

			if (iDamageDealt >= pEnemy->GetCurrHitPoints())
			{
				if (iDamageReceived < pUnit->GetCurrHitPoints())
					iDamageDealt += 30; //bonus for a kill, but no suicide
				iDamageReceived = max(0, iDamageReceived - pUnit->getHPHealedIfDefeatEnemy());
			}

			//if the garrison is (almost) unhurt, we can be a bit more aggressive and assume we'll heal up next turn
			int iHealRate = pUnit->healRate(pUnit->plot());
			if (pUnit->getDamage() < iHealRate/2 && iDamageReceived < pUnit->GetMaxHitPoints()/2 && bAllowMovement)
				iDamageReceived = max(0, iDamageReceived - iHealRate);

			int iScore = (1000 * iDamageDealt) / (iDamageReceived + 10);
			meleeTargets.push_back(SPlotWithScore(pTestPlot, iScore));

			//increase the threshold for each new enemy we find
			iScoreThreshold += 1000;
		}
		//maybe capture a civilian?
		else if (pTestPlot->isEnemyUnit(pUnit->getOwner(), false, true) &&
			pUnit->GetDanger(pTestPlot) == 0 &&
			pUnit->GetDanger() == 0)
		{
			meleeTargets.push_back(SPlotWithScore(pTestPlot, 10 - plotDistance(*pTestPlot, *pUnit->plot())));
		}
	}

	//nothing to do?
	if (meleeTargets.empty())
		return false;

	std::sort(meleeTargets.begin(), meleeTargets.end());

	//we will never do attacks with negative scores!
	if (meleeTargets.back().score < iScoreThreshold)
		return false;

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("Performing melee opportunity attack on (%d:%d) with %s at (%d:%d)",
			meleeTargets.front().pPlot->getX(), meleeTargets.front().pPlot->getY(), pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
		GET_PLAYER(pUnit->getOwner()).GetTacticalAI()->LogTacticalMessage(strMsg);
	}

	pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), meleeTargets.back().pPlot->getX(), meleeTargets.back().pPlot->getY());
	if (pUnit->canMove()) //try to move back to the original plot
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pOrigin->getX(), pOrigin->getY());

	return true;
}

//see if we can hit anything from our current plot - with or without moving
bool TacticalAIHelpers::PerformRangedOpportunityAttack(CvUnit* pUnit, bool bAllowMovement)
{
	if (!pUnit || !pUnit->IsCanAttackRanged() || !pUnit->canMove())
		return false;

	CvPlot* pBasePlot = pUnit->plot();
	bool bIsAirUnit = pUnit->getDomainType()==DOMAIN_AIR;
	if (bIsAirUnit)
		bAllowMovement = false;
	
	int iMaxDamage = 0;
	CvPlot* pBestTarget = NULL;

	int iRange = max(1,min(5,pUnit->GetRange()));
	if (bAllowMovement)
		iRange = 4; //this just says how far we look for possible targets in this case

	for (int i=RING0_PLOTS; i<RING_PLOTS[iRange]; i++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pBasePlot, i);
		if (!pLoopPlot || pLoopPlot->isCity())
			continue;

		CvUnit* pOtherUnit = pLoopPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(), pUnit, true /*testWar*/);

		//don't blindly attack the first one we find, check how much damage we can do
		if (pOtherUnit && !pOtherUnit->isDelayedDeath() && pUnit->canRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
		{
			int iDamage = bIsAirUnit ? pUnit->GetAirCombatDamage(pOtherUnit, NULL, false) : 
										pUnit->GetRangeCombatDamage(pOtherUnit, NULL, false, 0) +  pUnit->GetRangeCombatSplashDamage(pOtherUnit->plot());

			if (iDamage < pOtherUnit->GetCurrHitPoints())
			{
				//if we're not killing the enemy, don't use a garrison
				if (iDamage > iMaxDamage && !pUnit->IsGarrisoned())
				{
					pBestTarget = pLoopPlot;
					iMaxDamage = iDamage;
				}

			}
			else
			{
				//bonus for a kill
				iDamage += 30;
				if (iDamage > iMaxDamage)
				{
					pBestTarget = pLoopPlot;
					iMaxDamage = iDamage;
				}

			}
		}
	}

	if (!pBestTarget)
		return false;

	if (bAllowMovement)
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Performing ranged opportunity attack with disengagement on (%d:%d) with %s at (%d:%d)",
				pBestTarget->getX(), pBestTarget->getY(), pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
			GET_PLAYER(pUnit->getOwner()).GetTacticalAI()->LogTacticalMessage(strMsg);
		}

		//no loop needed, there is only one unit anyway
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestOffensiveAssignment(vector<CvUnit*>(1, pUnit), pBestTarget, AL_LOW, gTactPosStorage);
		return TacticalAIHelpers::ExecuteUnitAssignments(pUnit->getOwner(), vAssignments);
	}
	else
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Performing ranged opportunity attack without moving on (%d:%d) with %s at (%d:%d)",
				pBestTarget->getX(), pBestTarget->getY(), pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
			GET_PLAYER(pUnit->getOwner()).GetTacticalAI()->LogTacticalMessage(strMsg);
		}

		pUnit->PushMission(bIsAirUnit ? CvTypes::getMISSION_MOVE_TO() : CvTypes::getMISSION_RANGE_ATTACK(), pBestTarget->getX(), pBestTarget->getY());
		return true;
	}
}

///Returns false if insufficient free plots around the target
int TacticalAIHelpers::CountDeploymentPlots(const CvPlot* pTarget, int iRange, TeamTypes eTeam, bool bForNavalOp)
{
	int iNumDeployPlotsFound = 0;
	bool bAllowDeepWater = GET_TEAM(eTeam).canEmbarkAllWaterPassage();

	iRange = max(1, min(5, iRange));
	for (int i = 0; i < RING_PLOTS[iRange]; i++)
	{
		CvPlot* pPlot = iterateRingPlots(pTarget, i);
		if (!pPlot)
			continue;

		if(pPlot->isImpassable(eTeam))
			continue;

		if (pPlot->isOwned() && pPlot->getTeam() != eTeam)
			continue;

		if (pPlot->isWater())
					{
			if (!bForNavalOp)
						continue;
			else if(!bAllowDeepWater && pPlot->isDeepWater())
						continue;
					}
		else if (bForNavalOp)
						continue;

					iNumDeployPlotsFound++;
	}

	return iNumDeployPlotsFound;
}

CvPlot* TacticalAIHelpers::FindSafestPlotInReach(const CvUnit* pUnit, bool bAllowEmbark, bool bLowDangerOnly, bool bConsiderSwap)
{
	vector<OptionWithScore<CvPlot*>> aCityList;
	vector<OptionWithScore<CvPlot*>> aZeroDangerList;
	vector<OptionWithScore<CvPlot*>> aCoverList;
	vector<OptionWithScore<CvPlot*>> aDangerList;

	//for current plot
	int iCurrentHealRate = pUnit->healRate(pUnit->plot());

	ReachablePlots eligiblePlots = pUnit->GetAllPlotsInReachThisTurn(); //embarkation allowed for now, we sort it out below
	for (ReachablePlots::iterator it=eligiblePlots.begin(); it!=eligiblePlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		// don't attack though
		if (pPlot->getNumVisibleEnemyDefenders(pUnit) > 0)
			continue;

		// allow capturing civilians!
		int iFlags = CvUnit::MOVEFLAG_DESTINATION;
		if (pPlot->isVisibleEnemyUnit(pUnit))
			iFlags |= CvUnit::MOVEFLAG_ATTACK;

		if (!pUnit->canMoveInto(*pPlot, iFlags))
		{
			if (!bConsiderSwap || !pUnit->CanSwapWithUnitHere(*pPlot))
				continue;
		}

		//   prefer being in a city with the lowest danger value
		//   prefer being in a plot with no danger value
		//   prefer being under a unit with the lowest danger value
		//   prefer being in your own territory with the lowest danger value
		//   prefer the lowest danger value

		// plot danger is a bit unreliable, so we need extra checks
		CvPlayer& kPlayer = GET_PLAYER(pUnit->getOwner());
		int iDanger = pUnit->GetDanger(pPlot);
		//use both measures here because estimated turns often isn't granular enough
		int iCityDistance = kPlayer.GetCityDistanceInEstimatedTurns(pPlot) + kPlayer.GetCityDistanceInPlots(pPlot);

		bool bIsZeroDanger = (iDanger <= 0);
		bool bIsInCity = pPlot->isFriendlyCity(*pUnit);
		bool bIsInTerritory = (pPlot->getTeam() == kPlayer.getTeam());

		//taking cover only works if the defender will not move away!
		CvUnit* pDefender = pPlot->getBestDefender(pUnit->getOwner());
		bool bIsInCover = pDefender && pDefender->TurnProcessed() && !pUnit->IsCanDefend(pPlot); // only move to cover if I'm defenseless here

		bool bWrongDomain = pPlot->needsEmbarkation(pUnit);
		bool bWouldEmbark = bWrongDomain && !pUnit->isEmbarked();

		//avoid overflow further down and useful handling for civilians
		if (iDanger == INT_MAX)
			iDanger = 10000;

		//we can't heal after moving and lose fortification bonus, so the current plot gets a bonus (respectively all others a penalty)
		if (pPlot != pUnit->plot() && iCurrentHealRate>0)
			iDanger += iCurrentHealRate-3; //everything else equal it looks stupid to stand around while being shot at

		//heal rate is higher here and danger lower
		if (!bIsInTerritory)
			iDanger += 12;

		//try to hide - if there are few enemy units, this might be a tiebreaker
		//this is cheating a bit, really we need to check if the plot is visible for the enemy units visible to us
		if (pPlot->isVisibleToEnemy(pUnit->getOwner()))
			iDanger += 9;

		//try to go avoid borders
		if (pPlot->IsAdjacentOwnedByEnemy(pUnit->getTeam()))
			iDanger += 7;

		//don't stay here, try to get away even if it means temporarily moving to a higher danger plot
		if (pPlot->IsEnemyCityAdjacent(pUnit->getTeam(),NULL))
			iDanger += 23;

		//naval units should avoid enemy coast, never know what's hiding there
		if (pUnit->getDomainType() == DOMAIN_SEA)
			iDanger += pPlot->countMatchingAdjacentPlots(DOMAIN_LAND, NO_PLAYER, pUnit->getOwner(), NO_PLAYER) * 7;

		//try to go where our friends are
		int iFriendlyUnitsAdjacent = pPlot->GetNumFriendlyUnitsAdjacent(pUnit->getTeam(), NO_DOMAIN);
		//don't go where our foes are
		int iEnemyUnitsAdjacent = pPlot->GetNumEnemyUnitsAdjacent(pUnit->getTeam(), pUnit->getDomainType(), NULL, true);

		iDanger += (iEnemyUnitsAdjacent-iFriendlyUnitsAdjacent) * 9;
		
		//use city distance as tiebreaker
		int iScore = iDanger * 10 + iCityDistance;

		//discourage water tiles for land units
		//note that zero danger status has already been established, this is only for sorting now
		if (bWrongDomain)
			iScore += 500;

		if(bIsInCity)
		{
			if (!pPlot->getPlotCity()->isInDangerOfFalling())
				aCityList.push_back( OptionWithScore<CvPlot*>(pPlot,iScore) );
		}
		else if(bIsZeroDanger)
		{
			//if danger is zero, look at distance to closest owned city instead
			//idea: could also look at number of plots reachable from pPlot to avoid dead ends
			aZeroDangerList.push_back( OptionWithScore<CvPlot*>(pPlot, bIsInTerritory ? iCityDistance : iCityDistance*2) );
		}
		else if(bIsInCover) //mostly relevant for civilians
		{
			aCoverList.push_back( OptionWithScore<CvPlot*>(pPlot,iScore) );
		}
		else if( (!bLowDangerOnly || iDanger<pUnit->GetCurrHitPoints()) && (!bWouldEmbark || bAllowEmbark) )
		{
			aDangerList.push_back( OptionWithScore<CvPlot*>(pPlot,iScore) );
		}
	}

	sort(aCityList.begin(), aCityList.end());
	sort(aZeroDangerList.begin(), aZeroDangerList.end());
	sort(aCoverList.begin(), aCoverList.end());
	sort(aDangerList.begin(), aDangerList.end());

	// Now that we've gathered up our lists of destinations, pick the most promising one
	if (aCityList.size()>0)
		return aCityList.back().option;
	else if (aZeroDangerList.size()>0)
		return aZeroDangerList.back().option;
	else if (aCoverList.size()>0)
		return aCoverList.back().option;
	else if (aDangerList.size()>0)
		return aDangerList.back().option;

	return NULL;
}

void CTacticalUnitArray::push_back(const CvTacticalUnit& unit)
{
	CheckDebugTrigger(unit.GetID());
	m_vec.push_back(unit);
}

bool TacticalAIHelpers::IsGoodPlotForStaging(CvPlayer* pPlayer, CvPlot* pCandidate, DomainTypes eDomain)
{
	if (!pPlayer || !pCandidate)
		return false;

	if (pCandidate->getBestDefender(pPlayer->GetID())!=NULL)
		return false;

	if (eDomain != NO_DOMAIN && pCandidate->getDomain() != eDomain)
		return false;

	int iCityDistance = pPlayer->GetCityDistanceInEstimatedTurns(pCandidate);
	if (iCityDistance>4)
		return false;

	if (pCandidate->getRouteType()!=NO_ROUTE)
		return false;

	int iFriendlyCombatUnitsAdjacent = 0;
	int iPassableNeighbors = 0;
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pNeighbor = plotDirection(pCandidate->getX(), pCandidate->getY(), ((DirectionTypes)iI));
		if (!pNeighbor)
			continue;

		//don't want to provoke other players, stay away from their units
		if (pNeighbor->isNeutralUnit(pPlayer->GetID(),true,true,true) || pNeighbor->isEnemyUnit(pPlayer->GetID(),true,true,true))
			return false;

		//stay away from their borders as well
		if (pNeighbor->isOwned() && pNeighbor->getTeam() != pPlayer->getTeam())
			if (GET_PLAYER(pNeighbor->getOwner()).isMajorCiv())
				return false;

		if (pNeighbor->getBestDefender(pPlayer->GetID()) != NULL)
			iFriendlyCombatUnitsAdjacent++;

		if (eDomain == NO_DOMAIN || eDomain == pNeighbor->getDomain())
			if (!pNeighbor->isImpassable(pPlayer->getTeam()))
				iPassableNeighbors++;
	}

	//don't build a wall of units
	if (iFriendlyCombatUnitsAdjacent>3)
		return false;

	//don't move into dead ends
	if (iPassableNeighbors<3)
		return false;

	return true;
}

CvPlot* TacticalAIHelpers::FindClosestSafePlotForHealing(CvUnit* pUnit)
{
	if (!pUnit)
		return NULL;

	//first see if the current plot is good
	if (pUnit->GetDanger() == 0 && pUnit->canHeal(pUnit->plot(), false, true))
		return pUnit->plot();

	std::vector<SPlotWithScore> vCandidates;
	ReachablePlots eligiblePlots = pUnit->GetAllPlotsInReachThisTurn(); //embarkation allowed for now, we sort it out below
	for (ReachablePlots::iterator it = eligiblePlots.begin(); it != eligiblePlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		if (pPlot->isEnemyUnit(pUnit->getOwner(), true, true))
			continue;

		bool bPillage = (it->iMovesLeft > 0) && pUnit->shouldPillage(pPlot, false, it->iMovesLeft);
		//don't check movement, don't need to heal right now
		if (pUnit->getDomainType() == DOMAIN_LAND)
		{
			if (!pUnit->canHeal(pPlot, false, true))
				continue;
		}
		else
		{
			//naval units usually must pillage to heal ...
			if (!bPillage && !pUnit->canHeal(pPlot, false, true))
				continue;
		}

		//can we stay there?
		if (!pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
		{
			//can we swap?
			CvUnit* pSwapUnit = pUnit->GetPotentialUnitToSwapWith(*pPlot);
			//melee units are there to soak damage ...
			int iDangerLimit = pSwapUnit ? (pSwapUnit->isRanged() ? pSwapUnit->GetCurrHitPoints() : (3 * pSwapUnit->GetCurrHitPoints()) / 2) : 0;
			if (!pSwapUnit || !pSwapUnit->isNativeDomain(pUnit->plot()) || pSwapUnit->GetDanger(pUnit->plot()) > iDangerLimit)
				continue;
		}

		int iDanger = pUnit->GetDanger(pPlot);
		if (iDanger <= pUnit->healRate(pPlot)) //ignore pillage health here, it's a one-time effect and may lead into dead ends
		{
			int iScore = pUnit->healRate(pPlot) - iDanger / 5;
			//friendly combat unit nearby = good
			iScore += pPlot->GetNumFriendlyUnitsAdjacent(pUnit->getTeam(), NO_DOMAIN, pUnit);
			//can pillage = good
			if (bPillage)
				iScore += GC.getPILLAGE_HEAL_AMOUNT();
			//higher distance = bad
			iScore -= GET_PLAYER(pUnit->getOwner()).GetCityDistanceInEstimatedTurns(pPlot);
			//todo: check distance to enemy cities as well?

			vCandidates.push_back(SPlotWithScore(pPlot, iScore));
		}
	}

	if (!vCandidates.empty())
	{
		std::stable_sort(vCandidates.begin(), vCandidates.end());
		return vCandidates.back().pPlot;
	}

	return NULL;
}

bool TacticalAIHelpers::GetPlotsForRangedAttack(const CvPlot* pTarget, const CvUnit* pUnit, int iRange, bool bCheckOccupied, std::vector<CvPlot*>& vPlots)
{
	vPlots.clear();

	if (!pTarget || !pUnit)
		return false;

	// Aircraft and special promotions make us ignore LOS
	bool bIgnoreLOS = pUnit->IsRangeAttackIgnoreLOS() || pUnit->getDomainType()==DOMAIN_AIR;
	// Can only bombard in domain? (used for Subs' torpedo attack)
	bool bOnlyInDomain = pUnit->getUnitInfo().IsRangeAttackOnlyInDomain();
	bool bIsCoastalOnly = pUnit->getUnitInfo().IsCoastalFireOnly();

	std::vector<CvPlot*> vCandidates = GC.getMap().GetPlotsAtRange(pTarget, iRange, false, !bIgnoreLOS);

	//filter and take only the half closer to origin
	CvPlot* pRefPlot = pUnit->plot();
	if(pRefPlot == NULL)
		return false;

	int iRefDist = plotDistance(*pRefPlot,*pTarget);
	std::vector<SPlotWithScore> vIntermediate;
	for (size_t i=0; i<vCandidates.size(); i++)
	{
		if((vCandidates[i]) == NULL)
			continue;

		int iDistance = plotDistance(*pRefPlot,*(vCandidates[i]));
		if (iDistance>iRefDist && iRefDist>iRange)
			continue;

		if (!vCandidates[i]->isRevealed(pUnit->getTeam()))
			continue;

		if (!pUnit->isNativeDomain(vCandidates[i]))
			continue;

		if (bCheckOccupied && vCandidates[i]!=pRefPlot && vCandidates[i]->getBestDefender(NO_PLAYER))
			continue;

		//Respect domains.
		if (pUnit->getDomainType() != vCandidates[i]->getDomain())
			continue;

		if (bOnlyInDomain)
		{
			//subs can only attack within their (water) area or adjacent cities
			if (pRefPlot->getArea() != vCandidates[i]->getArea())
			{
				CvCity *pCity = vCandidates[i]->getPlotCity();
				if (!pCity || !pCity->isAdjacentToArea(pRefPlot->getArea()))
					continue;
			}
		}
		if (bIsCoastalOnly)
		{
			if (vCandidates[i]->getDomain() == DOMAIN_LAND && pRefPlot->getArea() != vCandidates[i]->getArea())
				if (!vCandidates[i]->isCoastalLand())
					continue;
		}

		vIntermediate.push_back( SPlotWithScore(vCandidates[i],iDistance) );
	}

	//sort by increasing distance
	std::stable_sort(vIntermediate.begin(), vIntermediate.end());

	for (size_t i=0; i<vIntermediate.size(); i++)
		vPlots.push_back(vIntermediate[i].pPlot);

	return true;
}

//helper function for city threat calculation
int TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(const CvCity* pCity, const CvUnit* pAttacker, const CvPlot* pAttackerPlot, int& iAttackerDamage, 
													bool bIgnoreUnitAdjacencyBoni, int iExtraDefenderDamage, bool bQuickAndDirty)
{
	if (!pAttacker || !pCity || pAttacker->isDelayedDeath() || pAttacker->IsDead())
		return 0;
		
	int iDamage = 0;
	if (pAttacker->IsCanAttackRanged())
	{
		if (pAttacker->getDomainType() == DOMAIN_AIR)
			iDamage += pAttacker->GetAirCombatDamage(NULL, pCity, false);
		else
			iDamage += pAttacker->GetRangeCombatDamage(NULL, pCity, false, iExtraDefenderDamage, NULL, pAttackerPlot, bIgnoreUnitAdjacencyBoni, bQuickAndDirty);

		iAttackerDamage = 0; //what about interceptions?
	}
	else
	{
		if (pAttacker->isRangedSupportFire())
			iDamage += pAttacker->GetRangeCombatDamage(NULL, pCity, false, iExtraDefenderDamage, NULL, pAttackerPlot, bIgnoreUnitAdjacencyBoni, bQuickAndDirty);

		//just assume the unit can attack from its current location - modifiers might be different, but thats acceptable
		iDamage += pAttacker->getCombatDamage(
			pAttacker->GetMaxAttackStrength(pAttackerPlot, pCity->plot(), NULL, bIgnoreUnitAdjacencyBoni, bQuickAndDirty),
			pCity->getStrengthValue(), //not affected by assumed extra damage
			false, false, true);

		//counterattack for self damage
		iAttackerDamage = pAttacker->getCombatDamage(
			pCity->getStrengthValue(), //not affected by assumed extra damage
			pAttacker->GetMaxAttackStrength(pAttackerPlot, pCity->plot(), NULL, bIgnoreUnitAdjacencyBoni, bQuickAndDirty),
			false, true, false);

		//if both would die, the attacking unit survives!
		if (pCity->getDamage() + iExtraDefenderDamage + iDamage >= pCity->GetMaxHitPoints() && pAttacker->getDamage() + iAttackerDamage >= pAttacker->GetMaxHitPoints())
			iAttackerDamage = pAttacker->GetMaxHitPoints() - pAttacker->getDamage() - 1;
	}

	return iDamage;
}

//helper function for unit threat calculation
int TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(const CvUnit* pDefender, const CvUnit* pAttacker, 
				const CvPlot* pDefenderPlot, const CvPlot* pAttackerPlot, int& iAttackerDamage, 
				bool bIgnoreUnitAdjacencyBoni, int iExtraDefenderDamage, bool bQuickAndDirty)
{
	if (!pAttacker || !pDefender || pDefender->isDelayedDeath() || pDefender->IsDead() || pAttacker->isDelayedDeath() || pAttacker->IsDead())
		return 0;
		
	int iDamage = 0;
	if (pAttacker->IsCanAttackRanged())
	{
		if (pAttacker->getDomainType() == DOMAIN_AIR)
		{
			CvUnit* pInterceptor = pDefenderPlot->GetBestInterceptor(pAttacker->getOwner(), pAttacker, false, true);
			// assume interception is successful - do this before the actual attack
			iAttackerDamage = pInterceptor ? pInterceptor->GetInterceptionDamage(pDefender, false, pDefenderPlot) : 0;

			if (pAttacker->GetCurrHitPoints() - iAttackerDamage > 0)
			{
				iDamage += pAttacker->GetAirCombatDamage(pDefender, NULL, false, iAttackerDamage, pDefenderPlot, pAttackerPlot, bQuickAndDirty);
				iAttackerDamage += pDefender->GetAirStrikeDefenseDamage(pAttacker, false, pDefenderPlot);
			}
		}
		else
		{
			iDamage += pAttacker->GetRangeCombatDamage(pDefender, NULL, false, iExtraDefenderDamage, 
							pDefenderPlot, pAttackerPlot, bIgnoreUnitAdjacencyBoni, bQuickAndDirty);
			iAttackerDamage = 0;
		}
	}
	else
	{
		//for melee attack check whether the attacker can actually go where the defender is
		//the defender might only be there hypothetically - so an empty plot is a valid target 
		if (pDefenderPlot && !pAttacker->canMoveOrAttackInto(*pDefenderPlot))
			return 0;

		if (pAttacker->isRangedSupportFire())
			iDamage += pAttacker->GetRangeCombatDamage(pDefender, NULL, false, iExtraDefenderDamage, 
							pDefenderPlot, pAttackerPlot, bIgnoreUnitAdjacencyBoni, bQuickAndDirty);

		int iAttackerStrength = pAttacker->GetMaxAttackStrength(pAttackerPlot, pDefenderPlot, pDefender, bIgnoreUnitAdjacencyBoni, bQuickAndDirty);
		//do not override defender flanking/general bonus (it is known during combat simulation)
		int iDefenderStrength = pDefender->GetMaxDefenseStrength(pDefenderPlot, pAttacker, pAttackerPlot, false, bQuickAndDirty); 

		//hack. we modify defender strength again by the modifier for assumed extra damage
		iDefenderStrength += (pDefender->GetDamageCombatModifier(true, iExtraDefenderDamage) * iDefenderStrength) / 100;

		//just assume the unit can attack from its current location - modifiers might be different, but thats acceptable
		iDamage += pAttacker->getCombatDamage(
			iAttackerStrength,
			iDefenderStrength,
			false, false, false);

		//note: this is just the self-damage of the attacker! if the defender strikes back, we typically get other strength values because the plots are switched!
		iAttackerDamage = pDefender->getCombatDamage(
			iDefenderStrength,
			iAttackerStrength,
			false, false, false);
	}

	return iDamage;
}

bool TacticalAIHelpers::KillUnitIfPossible(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (!pAttacker || !pDefender || pDefender->isDelayedDeath())
		return false;

	//aircraft are different
	if (pAttacker->getDomainType()==DOMAIN_AIR || pDefender->getDomainType()==DOMAIN_AIR)
		return false;

	//see how the attack would go
	int iDamageDealt = 0, iDamageReceived = 0;
	iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pAttacker, pDefender->plot(), pAttacker->plot(), iDamageReceived);

	//is it worth it?
	if ( iDamageDealt+7 > pDefender->GetCurrHitPoints() && pAttacker->GetCurrHitPoints()-iDamageReceived > GC.getMAX_HIT_POINTS()/2 )
	{
		if (pAttacker->isRanged())
		{
			//can we attack directly
			if (pAttacker->canRangeStrikeAt(pDefender->getX(),pDefender->getY()))
			{
				pAttacker->PushMission(CvTypes::getMISSION_RANGE_ATTACK(),pDefender->getX(),pDefender->getY());
				return true;
			}
			else if (pAttacker->canRangeStrike())
			{
				//need to move and shoot
				bool bIgnoreLOS = pAttacker->IsRangeAttackIgnoreLOS();
				std::vector<CvPlot*> vAttackPlots = GC.getMap().GetPlotsAtRange(pDefender->plot(), pAttacker->GetRange(), false, !bIgnoreLOS);
				for (std::vector<CvPlot*>::iterator it = vAttackPlots.begin(); it != vAttackPlots.end(); ++it)
				{
					if (pAttacker->TurnsToReachTarget(*it, CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN, 1) == 0 && pAttacker->canEverRangeStrikeAt(pDefender->getX(), pDefender->getY(), *it, false))
					{
						pAttacker->PushMission(CvTypes::getMISSION_MOVE_TO(), (*it)->getX(), (*it)->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER);
						pAttacker->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pDefender->getX(), pDefender->getY());
						return true;
					}
				}
			}
		}
		else //melee
		{
			if (pAttacker->TurnsToReachTarget(pDefender->plot(),0,1)==0)
			{
				pAttacker->PushMission(CvTypes::getMISSION_MOVE_TO(),pDefender->getX(),pDefender->getY());
				return true;
			}
		}
	}

	return false;
}

bool TacticalAIHelpers::IsSuicideMeleeAttack(CvUnit * pAttacker, CvPlot * pTarget)
{
	//todo: add special code for air attacks!
	if (!pAttacker || !pTarget || pAttacker->isRanged() || pAttacker->getDomainType()==DOMAIN_AIR)
		return false;

	int iDamageDealt = 0, iDamageReceived = 0;

	//if we're not adjacent we don't know the plot the attacker will use in the end
	CvPlot* pAttackerPlot = NULL;
	if (pAttacker->plot()->isAdjacent(pTarget))
		pAttackerPlot = pAttacker->plot();

	//unit attack or city attack?
	if (pTarget->isCity())
	{
		iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pTarget->getPlotCity(), pAttacker, pAttackerPlot, iDamageReceived);
	}
	else
	{
		CvUnit* pDefender = pTarget->getBestDefender(NO_PLAYER, pAttacker->getOwner(), pAttacker);
		if (pDefender)
			iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pAttacker, pTarget, pAttackerPlot, iDamageReceived);
		else
			return false;
	}

	//add some margin for randomness
	return (iDamageReceived+3 >= pAttacker->GetCurrHitPoints());
}

CvPlot* TacticalAIHelpers::GetFirstTargetInRange(const CvUnit * pUnit, bool bMustBeAbleToKill, bool bIncludeCivilians)
{
	if (!pUnit)
		return NULL;

	ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
	for (ReachablePlots::iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		//this can only ever be true for melee units - ranged attacks are checked below
		if (!pUnit->IsCanAttackRanged() && pPlot->isEnemyUnit(pUnit->getOwner(), true, true))
		{
			if (bMustBeAbleToKill)
			{
				//see how the attack would go
				CvUnit* pDefender = pPlot->getVisibleEnemyDefender(pUnit->getOwner());
				int iDamageDealt = 0, iDamageReceived = 0;
				iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pUnit, pDefender->plot(), pUnit->plot(), iDamageReceived, true, 0, true);

				if (iDamageDealt > iDamageReceived && iDamageDealt >= pDefender->GetCurrHitPoints() && pUnit->GetCurrHitPoints() - iDamageReceived > pUnit->GetMaxHitPoints() / 2)
					return pPlot;
			}
			else
				return pPlot;
		}
		else if (bIncludeCivilians && pPlot->isEnemyUnit(pUnit->getOwner(), false, true))
			return pPlot;

		//unoccupied barb camp?
		if (pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			return pPlot;

		//could this unit capture a city?
		if (!pUnit->isNoCapture())
		{
			CvCity* pNeighboringCity = pPlot->getPlotCity();
			if (pNeighboringCity && GET_PLAYER(pUnit->getOwner()).IsAtWarWith(pNeighboringCity->getOwner()))
			{
				if (!bMustBeAbleToKill || pNeighboringCity->isInDangerOfFalling())
					return pPlot;
			}
		}
	}

	if (pUnit->IsCanAttackRanged())
	{
		//for ranged every tile we can enter with movement left is a base for attack
		std::set<int> attackableTiles = TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit,reachablePlots,true,false);
		for (std::set<int>::iterator attackTile=attackableTiles.begin(); attackTile!=attackableTiles.end(); ++attackTile)
		{
			CvPlot* pAttackTile = GC.getMap().plotByIndexUnchecked(*attackTile);
			if (bMustBeAbleToKill && !pAttackTile->isCity())
			{
				//see how the attack would go
				CvUnit* pDefender = pAttackTile->getVisibleEnemyDefender(pUnit->getOwner());
				if (!pDefender)
					continue; //shouldn't happen

				int iDamageDealt = 0, iDamageReceived = 0;
				iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pUnit, pDefender->plot(), pUnit->plot(), iDamageReceived, true, 0, true);

				if (iDamageDealt >= pDefender->GetCurrHitPoints())
					return pAttackTile;
			}
			else
				return pAttackTile;
		}
	}

	return NULL;
}

pair<int, int> TacticalAIHelpers::EstimateLocalUnitPower(const ReachablePlots& plotsToCheck, TeamTypes eTeamA, TeamTypes eTeamB, bool bMustBeVisibleToBoth)
{
	if (plotsToCheck.empty())
		return make_pair(0, 0);

	int iTeamAPower = 0;
	int iTeamBPower = 0;

	for (ReachablePlots::const_iterator it = plotsToCheck.begin(); it != plotsToCheck.end(); ++it)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
		if (!pLoopPlot)
			continue;

		if (bMustBeVisibleToBoth && !(pLoopPlot->isVisible(eTeamA) && pLoopPlot->isVisible(eTeamB)))
			continue;

		// If there are Units here, loop through them
		if (pLoopPlot->getNumUnits() > 0)
		{
			IDInfo* pUnitNode = pLoopPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// Is a combat unit
				if (pLoopUnit && (pLoopUnit->IsCombatUnit() || pLoopUnit->getDomainType() == DOMAIN_AIR))
				{
					int iScale = pLoopUnit->isNativeDomain(pLoopPlot) ? 1 : 2;

					if (pLoopUnit->getTeam() == eTeamA)
						iTeamAPower += pLoopUnit->GetPower() / iScale;
					if (pLoopUnit->getTeam() == eTeamB)
						iTeamBPower += pLoopUnit->GetPower() / iScale;
				}
			}
		}
	}

	return pair<int, int>(iTeamAPower,iTeamBPower);
}

//could we see additional plot when the unit moves to the test plot?
int TacticalAIHelpers::CountAdditionallyVisiblePlots(CvUnit * pUnit, CvPlot * pTestPlot)
{
	if (!pUnit || !pTestPlot)
		return 0;

	int iCount = 0;
	for (int iRange = 2; iRange <= pUnit->visibilityRange(); iRange++)
	{
		const vector<CvPlot*>& vPlots = GC.getMap().GetPlotsAtRange(pTestPlot, iRange, true, true);
		for (size_t i = 0; i < vPlots.size(); i++)
			if (!vPlots[i]->isVisible(pUnit->getTeam())) //we already know that would have line of sight
				iCount++;
	}
	
	return iCount;
}

//Unbelievable bad logic but taken like this from CvUnitCombat
bool AttackEndsTurn(const CvUnit* pUnit, int iNumAttacksLeft)
{
	if(!pUnit->canMoveAfterAttacking() && !pUnit->isRangedSupportFire() && iNumAttacksLeft<2)
		return true;

	return false;
}

int NumAttacksForUnit(int iMovesLeft, int iMaxAttacks)
{
	return max(0, min( (iMovesLeft+GC.getMOVE_DENOMINATOR()-1)/GC.getMOVE_DENOMINATOR(), iMaxAttacks ));
}

CvTacticalPlot::eTactPlotDomain DomainForUnit(const CvUnit* pUnit)
{
	if (!pUnit)
		return CvTacticalPlot::TD_BOTH;

	switch (pUnit->getDomainType())
	{
	case DOMAIN_LAND:
		return CvTacticalPlot::TD_LAND;
	case DOMAIN_SEA:
		return CvTacticalPlot::TD_SEA;
	default:
		return CvTacticalPlot::TD_BOTH;
	}
}

//note that the score returned from this function is not multiplied by 10 yet
void ScoreAttack(const CvTacticalPlot& tactPlot, const CvUnit* pUnit, const CvTacticalPlot& assumedPlot, eAggressionLevel eAggLvl, float fAggBias, STacticalAssignment& result)
{
	int iDamageDealt = 0;
	int iDamageReceived = 0; //always zero for ranged attack
	int iExtraScore = 0; //splash damage and other bonuses

	//the damage calculation doesn't know about hypothetical flanking units, so we ignore it here and add it ourselves 
	int iPrevDamage = tactPlot.getDamage();
	int iPrevHitPoints = 0;

	const CvPlot* pUnitPlot = assumedPlot.getPlot();
	const CvPlot* pTestPlot = tactPlot.getPlot();
	bool bBarbScoreReduction = false;

	if (tactPlot.isEnemyCity()) //a plot can be both a city and a unit - in that case we would attack the city
	{
		CvCity* pEnemy = pTestPlot->getPlotCity();
		iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(
			pEnemy, pUnit, pUnitPlot, iDamageReceived, true, iPrevDamage, true);
		iExtraScore = pUnit->GetRangeCombatSplashDamage(pTestPlot) + (pUnit->GetCityAttackPlunderModifier() / 50);
		iPrevHitPoints = pEnemy->GetMaxHitPoints() - pEnemy->getDamage() - iPrevDamage;

		//if we have multiple melee units encircling the city, try to take into account their attacks as well
		//otherwise nobody will make the first move
		int iMeleeAttackers = tactPlot.getNumAdjacentFirstlineFriendlies(CvTacticalPlot::TD_BOTH, assumedPlot.getPlotIndex());
		int iAssumedDamageFromOtherAttacks = iDamageDealt * iMeleeAttackers;

		int iRemainingTurnsOnCity = (iPrevHitPoints-iAssumedDamageFromOtherAttacks) / (iDamageDealt+1) + 1;
		int iRemainingTurnsOnAttacker = pUnit->GetCurrHitPoints() / (iDamageReceived+1) + 1;
		bool bAttackerWeak = (pUnit->getDamage() + iDamageReceived) * 2 > pUnit->GetMaxHitPoints();

		//should consider self-damage from previous attacks here ... blitz
		if (pUnit->GetCurrHitPoints() - iDamageReceived < 0 || (bAttackerWeak && iRemainingTurnsOnAttacker < iRemainingTurnsOnCity && iRemainingTurnsOnCity > 1) )
		{
			result.iScore = -INT_MAX;
			return;
		}

		//city blockaded? not 100% accurate, but anyway
		if (tactPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH,-1)==pTestPlot->countPassableNeighbors(NO_DOMAIN))
			iDamageDealt += iDamageDealt/5;

		//special: if there are multiple enemy units around, try to attack those first
		if (tactPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH)>1 && pUnit->AI_getUnitAIType()!=UNITAI_CITY_BOMBARD)
			iDamageDealt /= 2;
	}
	else if (tactPlot.isEnemyCombatUnit())
	{
		CvUnit* pEnemy = pTestPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(), pUnit, true, true); //ignore the official visibility
		if (!pEnemy)
		{
			OutputDebugString("expected enemy unit not present!\n");
			result.iScore = -INT_MAX;
			return;
		}

		//use the quick and dirty method ... and don't check for general bonus etc (their position isn't official yet - we handle that below)
		iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pEnemy, pUnit, pTestPlot, pUnitPlot, iDamageReceived, true, iPrevDamage, true);
		iExtraScore = pUnit->GetRangeCombatSplashDamage(pTestPlot);
		iPrevHitPoints = pEnemy->GetCurrHitPoints() - iPrevDamage;

		//should consider self-damage from previous attacks here ... blitz
		if (pUnit->GetCurrHitPoints() - iDamageReceived <= -1)
		{
			result.iScore = -INT_MAX;
			return;
		}

		//problem is flanking bonus affects combat strength, not damage, so the effect is nonlinear. anyway just assume 10% per adjacent unit
		if (!pUnit->isRanged()) //only for melee
		{
			//it works both ways!
			int iDelta = tactPlot.getNumAdjacentFriendlies(DomainForUnit(pUnit),-1) - assumedPlot.getNumAdjacentEnemies(DomainForUnit(pUnit));
			iDamageDealt += (iDelta*iDamageDealt)/10;
			iDamageReceived -= (iDelta*iDamageReceived)/10;
		}

		//repeat attacks may give extra bonus
		if (iPrevDamage > 0)
		{
			int iBonus = pUnit->getMultiAttackBonus() + GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
			if (iBonus > 0) //the bonus affects attack strength, so the effect is hard to predict ...
				iDamageDealt += iDamageDealt / 10;
		}

		//don't be as aggressive when attacking embarked units
		if (!pEnemy->IsCanAttack())
			fAggBias /= 2;

		//don't be distracted by attacks on barbarians when there are real enemies around
		if (pEnemy->isBarbarian())
			bBarbScoreReduction = true;
	}

	//fake general bonus
	if (assumedPlot.hasSupportBonus(-1))
	{
		iDamageDealt += iDamageDealt/10;
		iDamageReceived -= iDamageReceived/10;
	}

	//bonus for a kill
	if (iDamageDealt >= iPrevHitPoints)
	{
		//don't hand out points for over-killing (but make an allowance for randomness)
		iDamageDealt = min(iDamageDealt, iPrevHitPoints + 10);

		if (tactPlot.isEnemyCity()) //city capture
		{
			//ranged units can't capture, so discourage the attack
			if (pUnit->isRanged())
			{
				result.eAssignmentType = A_RANGEATTACK; //not a kill!
				//don't continue flogging a dead horse
				if ( iPrevHitPoints < 2 )
					iDamageDealt = 0;
			}
			else
			{
				iExtraScore += 300; //capturing a city is important
				result.eAssignmentType = A_MELEEKILL;
			}
		}
		else //enemy unit killed
		{
			iExtraScore += 30;
			if (pTestPlot->isEnemyUnit(pUnit->getOwner(),false,false))
				iExtraScore += 20; //even more points for a double kill
			if (pUnit->getHPHealedIfDefeatEnemy() > 0)
				iDamageReceived = max( iDamageReceived-pUnit->getHPHealedIfDefeatEnemy(), -pUnit->getDamage() ); //may turn negative, but can't heal more than current damage

			if (pUnit->isRanged())
				result.eAssignmentType = A_RANGEKILL;
			else
			{
				if (pUnitPlot->MeleeAttackerAdvances())
					result.eAssignmentType = A_MELEEKILL;
				else
					result.eAssignmentType = A_MELEEKILL_NO_ADVANCE;
			}
		}
	}
	else
		result.eAssignmentType = pUnit->isRanged() ? A_RANGEATTACK : A_MELEEATTACK;

	//for melee units we check if the damage received is worth it ...
	if (iDamageReceived > 0)
	{
		float fAggFactor = GC.getCOMBAT_AI_OFFENSE_DAMAGEWEIGHT() / 100.f;
		int iHPbelowHalf = pUnit->GetMaxHitPoints()/2 - (pUnit->GetCurrHitPoints() - iDamageReceived);
		switch (eAggLvl)
		{
		case AL_LOW:
			fAggFactor *= 0.5f+fAggBias/10; //bias is at least 0.9
			if ( iHPbelowHalf>0 )
				iExtraScore -= 4 * min(iDamageReceived,iHPbelowHalf);
			break;
		case AL_MEDIUM:
			fAggFactor *= 0.90f+fAggBias/10; //bias is at least 0.9
			if ( iHPbelowHalf>0 )
				iExtraScore -= 2 * min(iDamageReceived,iHPbelowHalf);
			break;
		case AL_HIGH:
			//don't care how much damage we do but we want to be able to survive a counterattack ...
			if ( (iDamageReceived+23) < pUnit->GetCurrHitPoints()*fAggBias )
				fAggFactor *= 2.3f;
			break;
		default:
			result.iScore = -INT_MAX;
			return;
		}

		int iScaledDamage = int(iDamageDealt*fAggBias*fAggFactor + 0.5f);
		if ( iScaledDamage - iDamageReceived + iExtraScore < 0)
		{
			result.iScore = -INT_MAX;
			return;
		}
	}

	//finally the almighty score
	//add previous damage again and again to make concentrated fire attractive
	//todo: consider pEnemy->getUnitInfo().GetProductionCost() and pEnemy->GetBaseCombatStrength()
	//todo: normalize damage done by max hp to balance between city attacks and unit attacks?
	result.iScore = iDamageDealt + iExtraScore;
	if (bBarbScoreReduction)
	{
		result.iScore *= 3;
		result.iScore /= 2;
	}

	result.iDamage = iDamageDealt;
	result.iSelfDamage = iDamageReceived;
}

bool TacticalAIHelpers::IsEnemyCitadel(const CvPlot* pPlot, PlayerTypes ePlayer)
{
	if (!pPlot || ePlayer==NO_PLAYER)
		return false;

	ImprovementTypes eImprovement = pPlot->getImprovementType();

	// Citadel here?
	if (eImprovement != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
	{
		int iDamage = GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage();

		return (iDamage != 0) && (pPlot->getOwner() != NO_PLAYER) && GET_PLAYER(ePlayer).IsAtWarWith(pPlot->getOwner());
	}
	
	return false;
}

STacticalAssignment ScorePlotForCombatUnitOffensiveMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition, bool bAllowPillage)
{
	//default action is do nothing and invalid score (not -INT_MAX to to prevent overflows!)
	STacticalAssignment result(unit.iPlotIndex,movePlot.iPlotIndex,unit.iUnitID,movePlot.iMovesLeft,unit.eStrategy,-100000,A_FINISH);

	//the plot we're checking right now
	const CvPlot* pTestPlot = testPlot.getPlot();
	const CvUnit* pUnit = unit.pUnit;

	//cannot deal with enemies here, only friendly/empty plots
	if (testPlot.isEnemy())
		return result;

	//is this actual movement or a finish move
	if (unit.iPlotIndex != movePlot.iPlotIndex)
	{
		//prevent two moves in a row, that is inefficient and can lead to "shuttling" behavior
		if (unit.eLastAssignment == A_MOVE || unit.eLastAssignment == A_MOVE_SWAP)
			return result;

		result.eAssignmentType = A_MOVE;
	}

	CvTacticalPlot::eTactPlotDomain eRelevantDomain = pUnit->isRanged() ? CvTacticalPlot::TD_BOTH : pTestPlot->isWater() ? CvTacticalPlot::TD_SEA : CvTacticalPlot::TD_LAND;

	//different contributions
	int iDamageScore = 0;
	int iDangerScore = 0;
	int iMiscScore = 0;

	//check how much damage we can do to the enemies around this plot
	//we only care about attacks in this turn, the rest is handled by the plot type score
	int iMaxAttacks = NumAttacksForUnit(movePlot.iMovesLeft, unit.iAttacksLeft);
	if (iMaxAttacks > 0)
	{
		std::vector<int> vDamageRatios;

		//works for both melee and ranged
		int iMaxRange = pUnit->isRanged() ? pUnit->GetRange() : 1;
		//ranged attacks are cross-domain
		for (int iRange = 1; iRange < iMaxRange + 1; iRange++)
		{
			//performance optimization
			if (iRange < testPlot.getEnemyDistance(eRelevantDomain))
				continue;

			std::vector<CvPlot*> vAttackPlots = GC.getMap().GetPlotsAtRange(pTestPlot, iRange, true, !pUnit->IsRangeAttackIgnoreLOS());
			for (size_t iCount = 0; iCount < vAttackPlots.size(); iCount++)
			{
				CvPlot* pLoopPlot = vAttackPlots[iCount];
				if (!pLoopPlot)
					continue;

				const CvTacticalPlot& enemyPlot = assumedPosition.getTactPlot(pLoopPlot->GetPlotIndex());
				if (enemyPlot.isValid() && enemyPlot.isEnemy())
				{
					//don't attack cities if the real target is something else
					if (enemyPlot.isEnemyCity() && assumedPosition.getTarget() != pLoopPlot && iMaxRange>1)
						continue;

					//we don't care for damage here but let's reuse the scoring function
					STacticalAssignment temp;
					ScoreAttack(enemyPlot, pUnit, testPlot, assumedPosition.getAggressionLevel(), assumedPosition.getAggressionBias(), temp);

					if (temp.iScore > 0)
					{
						//don't break formation if there are many enemies around
						if (temp.eAssignmentType == A_MELEEKILL && enemyPlot.getNumAdjacentEnemies(DomainForUnit(pUnit)) > 3)
							continue;

						vDamageRatios.push_back(temp.iScore);
					}
				}
			}
		}

		if (gTacticalCombatDebugOutput > 1)
		{
			OutputDebugString(CvString::format("pos %d: %s %d has %d attack targets at plot %d\n",
				assumedPosition.getID(), pUnit->getName().c_str(), unit.iUnitID, vDamageRatios.size(), movePlot.iPlotIndex).c_str());
		}

		//pick the best target
		if (!vDamageRatios.empty())
		{
			//the best target comes last
			std::sort(vDamageRatios.begin(), vDamageRatios.end());
			//for simplicity assume we will get the same score for multiple attacks
			//discount the score because the attack is still hypothetical
			//also we want the real attacks to have higher scores than movement to guarantee they are picked!
			iDamageScore += (iMaxAttacks * vDamageRatios.back()) / 2;
		}
	}

	//lookup desirability by unit strategy / enemy distance
	//even for intermediate plots, so as not to bias against them
	int iPlotTypeScores[5][5] = {
		{ -1,-1,-1,-1,-1 }, //none (should not occur)
		{  1, 10, 5, 1, -1 }, //firstline (note that it's ok to evaluate the score in an enemy plot for a firstline unit -> meleekill) 
		{ -1, 3, 10, 2, -1 }, //secondline
		{ -1, 1, 8, 10, -1 }, //thirdline
		{ -1, 1, 8,  8, -1 }, //support (should not occur here)
	};
	iMiscScore += iPlotTypeScores[unit.eStrategy][testPlot.getEnemyDistance(eRelevantDomain)];

	//note: it might happen we call this when all enemies are killed. 
	//in that case plot type FARAWAY is ok. score will still be positive because of bias.

	//when in doubt use the plot with better sight (maybe check seeFromLevel instead?)
	if (pTestPlot->isHills() || pTestPlot->isMountain())
		iMiscScore++;

	if (testPlot.isEnemyCivilian()) //unescorted civilian
	{
		if (movePlot.iMovesLeft > 0 || testPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_LAND) == 0)
		{
			CvUnit* pCivilian = pTestPlot->getUnitByIndex(0); //there should be only one ...
			if (pCivilian)
			{
				//workers are not so important ...
				iMiscScore += (pCivilian->AI_getUnitAIType() == UNITAI_WORKER) ? 5 : 20;
				result.eAssignmentType = A_CAPTURE; //important so that the next assigment can be a move again
			}
		}
	}

	//may instead of a plain finish we can pillage?
	if (bAllowPillage && movePlot.iPlotIndex == unit.iPlotIndex)
	{
		//would it make sense to pillage here?
		if (pUnit->shouldPillage(pTestPlot, false, movePlot.iMovesLeft) && !assumedPosition.unitHasAssignmentOfType(unit.iUnitID, A_PILLAGE))
		{
			//if it's a citadel we want to move there even if we cannot pillage right away and don't need the healing
			if (TacticalAIHelpers::IsEnemyCitadel(pTestPlot, assumedPosition.getPlayer()))
			{
				iMiscScore += 50;
				if (movePlot.iMovesLeft > 0) //if we can do it right away ...
				{
					iMiscScore += 50;
					if (!pUnit->hasFreePillageMove())
						result.iRemainingMoves -= min(result.iRemainingMoves, GC.getMOVE_DENOMINATOR());
					result.eAssignmentType = A_PILLAGE;
				}
			}
			//if it's an improvement we pillage to heal if we have moves to spare
			else if (pTestPlot->getImprovementType() != NO_IMPROVEMENT && (movePlot.iMovesLeft > 0 || pUnit->hasFreePillageMove()))
			{
				iMiscScore += 20;
				if (pUnit->IsGainsXPFromPillaging())
					iMiscScore += 10;
				if (!pUnit->hasFreePillageMove())
					result.iRemainingMoves -= min(result.iRemainingMoves, GC.getMOVE_DENOMINATOR());
				result.eAssignmentType = A_PILLAGE;
			}
		}
	}

	//many considerations are only relevant if we end the turn here (critical for skirmishers which can move after attacking ...)
	//we only consider this when explicitly ending the turn!
	if (movePlot.iMovesLeft == 0 || movePlot.iPlotIndex == unit.iPlotIndex) 
	{
		//if we don't plan on pillaging, then this is a plain finish assigment
		if (result.eAssignmentType != A_PILLAGE)
		{
			//minor bonus for staying put and healing
			//don't add too much else it overrides the firstline/secondline order
			if (unit.iMovesLeft == pUnit->maxMoves())
			{
				if (pUnit->IsHurt())
					iMiscScore ++; //cannot fortify, only heal
				if (!pUnit->noDefensiveBonus())
					iMiscScore ++; //fortification bonus!
			}
		}

		//try to close the lines (todo: make sure the friendlies intend to stay there ...)
		if (testPlot.getNumAdjacentFirstlineFriendlies(DomainForUnit(pUnit), unit.iPlotIndex) > 0)
			iMiscScore++;
		//when in doubt, stay under air cover
		if (testPlot.hasAirCover())
			iMiscScore++;
		//when in doubt, hide from the enemy
		if (!testPlot.isVisibleToEnemy() && unit.iAttacksLeft==0)
			iMiscScore++;

		//the danger value reflects any defensive terrain bonuses
		//but unfortunately danger is not very useful here
		// * ZOC is unclear during simulation
		// * freshly revealed enemy units are not considered
		int	iDanger = pUnit->GetDanger(pTestPlot, assumedPosition.getKilledEnemies());

		//if we're not alone, maybe not everybody will attack us
		int iSpreadFactor = pUnit->isRanged() ? 
			testPlot.getNumAdjacentFirstlineFriendlies(CvTacticalPlot::TD_BOTH,unit.iPlotIndex) + 1 : 
			testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH,unit.iPlotIndex) + 1;

		//can happen with garrisons, catch this case as is messes up the math
		if (iDanger == INT_MAX)
		{
			result.iScore = 1; //not impossible but strongly discouraged
			return result;
		}

		//don't do it if it's a death trap (unless there is no other choice ...)
		bool bAbort = false;
		bAbort |= (testPlot.getNumAdjacentEnemies(DomainForUnit(pUnit)) > 3);
		bAbort |= (assumedPosition.getAggressionBias() < 1 && testPlot.getNumAdjacentEnemies(DomainForUnit(pUnit)) == 3);
		bAbort |= (iSpreadFactor == 1 && iDanger > pUnit->GetCurrHitPoints());

		if (bAbort)
			return result;

		//avoid these plots
		if (testPlot.isNextToEnemyCitadel())
		{
			if (iDanger>=pUnit->GetCurrHitPoints())
				return result; //no suicide ...

			//the citadel damage is included in the danger value, but since it's guaranteed to happen give it additional emphasis
			iDanger += 50;
		}

		//unseen enemies might be hiding behind the edge, so assume danger there
		if (testPlot.isEdgePlot())
			iDanger = max( pUnit->GetMaxHitPoints()/2, iDanger);

		if (iDanger > 0)
		{
			//try to be more careful with highly promoted units
			iDanger += (pUnit->getExperienceTimes100() - GET_PLAYER(assumedPosition.getPlayer()).GetAvgUnitExp100()) / 200;

			//penalty for high danger plots
			iDangerScore = (iDanger * GC.getCOMBAT_AI_OFFENSE_DANGERWEIGHT()) / max(1,iSpreadFactor) / max(1,pUnit->GetCurrHitPoints() - unit.iSelfDamage);
		}

		//often there are multiple identical units which could move into a plot (eg in naval battles)
		//in that case we want to prefer the one which has more movement points left to make the movement animation look better
		iDamageScore -= result.iRemainingMoves / GC.getMOVE_DENOMINATOR();

		//sometimes danger is zero, but maybe we're wrong, so look at plot defense too
		iDamageScore += pTestPlot->defenseModifier(pUnit->getTeam(),false,false) / 10;

		//todo: take into account mobility at the proposed plot
		//todo: take into account ZOC when ending the turn
	}

	//final score - danger values are mostly useless but maybe useful as tiebreaker
	//add a flat base value so that bad moves are not automatically invalid - sometimes all moves are bad
	result.iScore = iDamageScore * 10 + iMiscScore * 10 - iDangerScore + GC.getCOMBAT_AI_OFFENSE_SCORE_BIAS();

	return result;
}

STacticalAssignment ScorePlotForCombatUnitDefensiveMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition)
{
	//since we're not going to attack, we will assume a single move per unit, so we will always end the turn on the test plot

	//default action is do nothing and invalid score (not -INT_MAX to to prevent overflows!)
	STacticalAssignment result(unit.iPlotIndex,movePlot.iPlotIndex,unit.iUnitID,movePlot.iMovesLeft,unit.eStrategy,-100000,A_FINISH);
		
	//the plot we're checking right now
	const CvPlot* pTestPlot = testPlot.getPlot();
	const CvUnit* pUnit = unit.pUnit;

	//cannot deal with enemies here, only friendly/empty plots
	if (testPlot.isEnemy())
		return result;

	//check if this is actual movement or whether we simply end the turn
	if (movePlot.iPlotIndex != unit.iPlotIndex)
	{
		//prevent two moves in a row, that is inefficient and can lead to "shuttling" behavior
		if (unit.eLastAssignment == A_MOVE || unit.eLastAssignment == A_MOVE_SWAP)
			return result;

		result.eAssignmentType = A_MOVE;
	}

	//simplification: don't embark if there is another embarked unit there, stacking is too complex to take care of then
	if (testPlot.hasFriendlyEmbarkedUnit() && !pUnit->isNativeDomain(pTestPlot))
		return result;

	//ranged attacks are cross-domain
	CvTacticalPlot::eTactPlotDomain eRelevantDomain = pUnit->isRanged() ? CvTacticalPlot::TD_BOTH : pTestPlot->isWater() ? CvTacticalPlot::TD_SEA : CvTacticalPlot::TD_LAND;

	//lookup score by unit strategy / enemy distance
	int iPlotTypeScores[5][5] = {
		{ -1,-1,-1,-1,-1 }, //none (should not occur)
		{ -1, 20, 6, 4, 1 }, //firstline
		{ -1, 1, 20, 8, 1 }, //secondline
		{ -1, 1, 8, 20, 1 }, //thirdline
		{ -1, 1, 4, 4, 1 }, //support (should not occur)
	};
	result.iScore = iPlotTypeScores[unit.eStrategy][testPlot.getEnemyDistance(eRelevantDomain)];

	const CvTacticalPlot& targetPlot = assumedPosition.getTactPlot( assumedPosition.getTarget()->GetPlotIndex() );
	if (targetPlot.isEnemy()) //we want to come close to prepare for a future attack and be ready for a counterattack from "inside"
	{
		//come close but not too close
		int iPlotDistance = plotDistance(*assumedPosition.getTarget(),*pTestPlot);
		if (iPlotDistance > TACTICAL_COMBAT_MAX_TARGET_DISTANCE)
			result.iScore = -5;
		else if (iPlotDistance < 2)
			result.iScore /= 2;
	}
	else //assume we want to defend the target, ie be ready for an attack from the "outside"
	{
		//move close to the (friendly) target
		int iPlotDistance = plotDistance(*assumedPosition.getTarget(),*pTestPlot);
		result.iScore += (3 - iPlotDistance);
	}

	//may instead of a plain finish we can pillage?
	if (movePlot.iPlotIndex == unit.iPlotIndex)
	{
		//would it make sense to pillage here?
		if (pUnit->shouldPillage(pTestPlot, false, movePlot.iMovesLeft) && !assumedPosition.unitHasAssignmentOfType(unit.iUnitID, A_PILLAGE))
		{
			//if it's a citadel we want to move there even if we cannot pillage right away and don't need the healing
			if (TacticalAIHelpers::IsEnemyCitadel(pTestPlot, assumedPosition.getPlayer()))
			{
				result.iScore  += 6;
				if (movePlot.iMovesLeft > 0) //if we can do it right away ...
				{
					result.iScore += 7;
					if (!pUnit->hasFreePillageMove())
						result.iRemainingMoves -= min(result.iRemainingMoves, GC.getMOVE_DENOMINATOR());
					result.eAssignmentType = A_PILLAGE;
				}
			}
			//if it's an improvement we pillage to heal if we have moves to spare
			else if (pTestPlot->getImprovementType() != NO_IMPROVEMENT && (movePlot.iMovesLeft > 0 || pUnit->hasFreePillageMove()))
			{
				result.iScore += 4;
				if (pUnit->IsGainsXPFromPillaging())
					result.iScore += 2;
				if (!pUnit->hasFreePillageMove())
					result.iRemainingMoves -= min(result.iRemainingMoves, GC.getMOVE_DENOMINATOR());
				result.eAssignmentType = A_PILLAGE;
			}
		}
	}

	//count all plots we could possibly attack from here (next turn since this is defensive placement)
	bool bEndTurn = (result.iToPlotIndex == unit.iPlotIndex) || result.iRemainingMoves == 0;
	if (bEndTurn)
	{
		if (pUnit->isRanged())
		{
			//careful with siege units, unseen enemies might be hiding behind the edge
			if (pUnit->isUnitAI(UNITAI_CITY_BOMBARD) && movePlot.iMovesLeft == 0 && testPlot.isEdgePlot())
				result.iScore -=2;

			if (pUnit->isNativeDomain(pTestPlot)) //if we could attack from here
			{
				//ignore the first ring
				for (int i = 2; i < pUnit->GetRange(); i++)
				{
					std::vector<CvPlot*> vAttackPlots = GC.getMap().GetPlotsAtRange(pTestPlot, pUnit->GetRange(), true, !pUnit->IsRangeAttackIgnoreLOS());
					result.iScore += vAttackPlots.size();
				}
			}

			//ranged units don't like to park next to enemies at all
			if (testPlot.getNumAdjacentEnemies(DomainForUnit(pUnit)) > 0 || 
				testPlot.getNumAdjacentFirstlineFriendlies(DomainForUnit(pUnit),unit.iPlotIndex) == 0)
				result.iScore -= 5; //no hard exclusion
		}

		//the danger value reflects any defensive terrain bonuses
		int iDanger = pUnit->GetDanger(pTestPlot, assumedPosition.getKilledEnemies());
		if (iDanger==INT_MAX)
		{
			//can happen with garrisons
			result.iScore = -10;
		}
		else if (iDanger>0)
		{
			//try to be more careful with highly promoted units
			iDanger += (pUnit->getExperienceTimes100() - GET_PLAYER(assumedPosition.getPlayer()).GetAvgUnitExp100()) / 1000;

			//if we're not alone, maybe not everybody will attack us
			int iSpreadFactor = pUnit->isRanged() ? testPlot.getNumAdjacentFirstlineFriendlies(CvTacticalPlot::TD_BOTH,unit.iPlotIndex) + 1 : testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH,unit.iPlotIndex) + 1;

			//use normalized danger for scoring (don't need to consider self damage here, we're not attacking)
			result.iScore -= (iDanger * GC.getCOMBAT_AI_DEFENSE_DANGERWEIGHT()) / max(1,iSpreadFactor) / (pUnit->GetCurrHitPoints() + 1);
		}

		//sometimes danger is zero, but maybe we're wrong, so look at plot defense too
		result.iScore += pTestPlot->defenseModifier(pUnit->getTeam(),false,false) / 10;

		//try to stay together
		if (testPlot.getNumAdjacentFriendlies(DomainForUnit(pUnit),unit.iPlotIndex) > 0)
			result.iScore++;

		//when in doubt, stay under air cover
		if (testPlot.hasAirCover())
			result.iScore++;

		//todo: take into account mobility at the proposed plot
		//todo: take into account ZOC when ending the turn
	}

	//minor bonus for staying put
	if (result.eAssignmentType == A_FINISH && movePlot.iMovesLeft == pUnit->maxMoves())
	{
		if (pUnit->IsHurt())
			result.iScore++; //for healing
		if (!pUnit->noDefensiveBonus())
			result.iScore++; //fortification bonus!
	}

	//tiebreakers to follow
	result.iScore *= 10;

	//often there are multiple identical units which could move into a plot
	//in that case we want to prefer the one which has more movement points left
	result.iScore += result.iRemainingMoves / GC.getMOVE_DENOMINATOR();

	//introduce some bias so that all valid scores are > 0
	result.iScore += GC.getCOMBAT_AI_DEFENSE_SCORE_BIAS();

	return result;
}

//stacking with combat units is allowed here!
STacticalAssignment ScorePlotForNonCombatUnitMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition)
{
	//default action is do nothing and invalid score (not -INT_MAX to to prevent overflows!)
	STacticalAssignment result(unit.iPlotIndex,movePlot.iPlotIndex,unit.iUnitID,movePlot.iMovesLeft,unit.eStrategy,-100000,A_FINISH);
		
	//the plot we're checking right now
	const CvPlot* pTestPlot = testPlot.getPlot();
	const CvUnit* pUnit = unit.pUnit;

	//cannot deal with enemies here, only friendly/empty plots
	if (testPlot.isEnemy())
		return result;

	if (movePlot.iPlotIndex != unit.iPlotIndex)
	{
		//prevent two moves in a row (also for generals, everything else is too complex)
		if (unit.eLastAssignment == A_MOVE)
			return result;

		result.eAssignmentType = A_MOVE;
	}

	int iScore = 0;
	switch (testPlot.getEnemyDistance())
	{
	case 0:
		return result; //don't ever go there, wouldn't work anyway
		break;
	case 1:
		iScore = 2; //dangerous, only in emergencies
		break;
	case 2:
	case 3:
		iScore = 23; //nice. different to first line is worth more than two adjacent units (check below)
		break;
	default:
		iScore = 2; //usual case for gathering moves, otherwise not really interesting
		break;
	}

	const CvTacticalPlot& targetPlot = assumedPosition.getTactPlot( assumedPosition.getTarget()->GetPlotIndex() );
	if (!targetPlot.isEnemy()) //gathering around a target
	{
		//can be treacherous with impassable terrain in between but everything else is much more complex
		int iPlotDistance = plotDistance(*assumedPosition.getTarget(),*pTestPlot);
		iScore = iScore + 3 - iPlotDistance;
	}

	//generals and admirals (as opposed to plain embarked units)
	if (unit.eStrategy == MS_SUPPORT)
	{
		//points for supported units (count only the first ring ...)
		iScore += 10 * testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH, -1);

		//avoid overlap. this works only because we ignore our own aura when calling this function!
		if (testPlot.hasSupportBonus(unit.iPlotIndex) && movePlot.iPlotIndex!=unit.iPlotIndex)
			iScore /= 2;
	}

	//we want one of our own combat units covering us
	//pass firstline, any combat movement strategy will do
	STacticalAssignment dummy(0,0,0,0,MS_FIRSTLINE);
	vector<STacticalAssignment> defenderAssignment = assumedPosition.findBlockingUnitsAtPlot(testPlot.getPlotIndex(),dummy);

	//todo: check if the defender is not embarked! 
	if ( (!defenderAssignment.empty() && defenderAssignment.front().iRemainingMoves == 0) || pTestPlot->isFriendlyCity(*pUnit) )
		iScore += 100;
	else
	{
		//danger is only relevant if there is no covering unit ...
		int iDanger = pUnit->GetDanger(pTestPlot, assumedPosition.getKilledEnemies());
		if (iDanger == INT_MAX)
			iScore -= 1000;
		else if (result.eMoveType==A_FINISH || result.iRemainingMoves==0) //only if we are going to end the turn here
		{
			if (testPlot.isEdgePlot()) //if we don't expect combat, we can afford to be less careful - embarked units have limited vision!
				iDanger += (assumedPosition.getAggressionLevel()==AL_NONE) ? 20 : 50;

			//if we're not alone, maybe not everybody will attack us
			iScore -= iDanger / (testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH, -1) + 1);
		}
	}

	result.iScore = iScore * 10;
	return result;
}

STacticalAssignment ScorePlotForRangedAttack(const SUnitStats& unit, const CvTacticalPlot& assumedUnitPlot, const CvTacticalPlot& enemyPlot, const CvTacticalPosition& assumedPosition)
{
	STacticalAssignment newAssignment(unit.iPlotIndex,enemyPlot.getPlotIndex(),unit.iUnitID,unit.iMovesLeft,unit.eStrategy,-1,A_FINISH);

	//received damage is zero here but still use the correct unit number ratio so as not to distort scores
	ScoreAttack(enemyPlot, unit.pUnit, assumedUnitPlot, assumedPosition.getAggressionLevel(), assumedPosition.getAggressionBias(), newAssignment);
	if (newAssignment.iScore < 0)
		return newAssignment;

	//what happens next?
	if (AttackEndsTurn(unit.pUnit, unit.iAttacksLeft))
	{
		//if we have movement to spare, we should be able to disengage before attacking?
		if (newAssignment.iRemainingMoves > GC.getMOVE_DENOMINATOR() && assumedUnitPlot.getEnemyDistance()==1 && unit.eStrategy != MS_FIRSTLINE)
			newAssignment.iScore /= 2;

		newAssignment.iRemainingMoves = 0;
	}
	else
		newAssignment.iRemainingMoves -= min(newAssignment.iRemainingMoves, GC.getMOVE_DENOMINATOR());

	//a bonus the further we can disengage after attacking
	newAssignment.iScore += (newAssignment.iRemainingMoves * 2) / GC.getMOVE_DENOMINATOR();

	//times 10 to match with ScorePlotForCombatUnitOffensive()
	newAssignment.iScore = newAssignment.iScore*10;

	//a slight boost for attacking the "real" target
	if ( enemyPlot.getPlotIndex()==assumedPosition.getTarget()->GetPlotIndex() )
		newAssignment.iScore += 2; 

	return newAssignment;
}

STacticalAssignment ScorePlotForMeleeAttack(const SUnitStats& unit, const CvTacticalPlot& assumedUnitPlot, const CvTacticalPlot& enemyPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition)
{
	//default action is invalid
	STacticalAssignment result(unit.iPlotIndex, movePlot.iPlotIndex, unit.iUnitID, unit.iMovesLeft, unit.eStrategy, -INT_MAX, A_FINISH);

	//the plot we're checking right now
	const CvPlot* pEnemyPlot = enemyPlot.getPlot();

	//sanity check
	if (!enemyPlot.isValid() || !assumedUnitPlot.isValid())
		return result;

	//this is only for melee attacks - ranged attacks are handled separately
	const CvUnit* pUnit = unit.pUnit;
	if (!enemyPlot.isEnemy() || pUnit->isRanged())
		return result;

	//how often can we attack this turn (depending on moves left on the unit)
	int iMaxAttacks = min(unit.iAttacksLeft, (unit.iMovesLeft + GC.getMOVE_DENOMINATOR() - 1) / GC.getMOVE_DENOMINATOR());
	if (iMaxAttacks == 0)
		return result;

	//check how much damage we could do
	ScoreAttack(enemyPlot, pUnit, assumedUnitPlot, assumedPosition.getAggressionLevel(), assumedPosition.getAggressionBias(), result);
	if (result.iScore < 0)
		return result;

	//don't melee attack cities if the real target is something else (capturing is ok)
	if (result.eAssignmentType != A_MELEEKILL && enemyPlot.isEnemyCity() && assumedPosition.getTarget() != pEnemyPlot)
		return result;

	//what happens next? capturing a city always ends the turn
	if (AttackEndsTurn(pUnit, iMaxAttacks) ||
		CvUnitMovement::IsSlowedByZOC(pUnit,assumedUnitPlot.getPlot(),pEnemyPlot,assumedPosition.getFreedPlots()) ||
		(enemyPlot.isEnemyCity() && result.eAssignmentType == A_MELEEKILL))
		//end turn cost will be checked in time, no need to panic yet
		result.iRemainingMoves = 0;
	else
	{
		int iMoveCost = unit.iMovesLeft - movePlot.iMovesLeft;
		int iAttackCost = max(iMoveCost, GC.getMOVE_DENOMINATOR());
		result.iRemainingMoves -= min(unit.iMovesLeft, iAttackCost);
	}

	//don't break formation if there are many enemies around
	if (result.eAssignmentType == A_MELEEKILL && enemyPlot.getNumAdjacentEnemies(DomainForUnit(pUnit)) > 3)
	{
		result.iScore = -INT_MAX;
		return result;
	}

	//a slight boost for attacking the "real" target or a citadel
	if (pEnemyPlot == assumedPosition.getTarget() || TacticalAIHelpers::IsEnemyCitadel(pEnemyPlot, assumedPosition.getPlayer()))
		result.iScore += 3; 

	//combo bonus
	if (result.eAssignmentType == A_MELEEKILL && enemyPlot.isEnemyCivilian())
		result.iScore += 5;

	//bring it into the same range as movement (add 8 so we're always better than just finishing the turn on a frontline plot)
	result.iScore = (result.iScore + 8) * 10;

	return result;
}

CvTacticalPlot::CvTacticalPlot(const CvPlot* plot, PlayerTypes ePlayer, const set<CvUnit*>& allOurUnits)
{
	if (!plot || ePlayer == NO_PLAYER)
	{
		pPlot = NULL;
		return;
	}

	//the most important thing
	pPlot = plot;

	//constant
	bHasAirCover = pPlot->HasAirCover(ePlayer);
	bIsVisibleToEnemy = pPlot->isVisibleToEnemy(ePlayer);
	bBlockedForCombatUnit = false;

	//updated if necessary
	bEnemyCivilianPresent = false;
	iDamageDealt = 0;

	//set initial state, update after every move
	aiFriendlyCombatUnitsAdjacent[TD_BOTH] = 0;
	aiFriendlyCombatUnitsAdjacent[TD_LAND] = 0;
	aiFriendlyCombatUnitsAdjacent[TD_SEA] = 0;
	aiFriendlyFirstlineUnitsAdjacent[TD_BOTH] = 0;
	aiFriendlyFirstlineUnitsAdjacent[TD_LAND] = 0;
	aiFriendlyFirstlineUnitsAdjacent[TD_SEA] = 0;
	nSupportUnitsAdjacent = 0;

	//these are all recomputed from scratch when necessary
	resetVolatileProperties();

	//concerning embarkation. this is complex because it allows combat units to stack, violating the 1UPT rule.
	//note that there are other exceptions as well, eg a fort can hold a naval unit and a land unit.
	//therefore we check for "native domain". we consider non-native domain units in the simulation but only allow moves into the native domain.
	//this means that 1UPT is still valid for all our simulated moves and we can ignore embarked defenders etc.

	//so here comes tricky logic to figure out whether we can use this plot
	bool bPlotBlocked = false;
	bool bSimUnitPresent = false;
	for (int i = 0; i < pPlot->getNumUnits(); i++)
	{
		CvUnit* pPlotUnit = pPlot->getUnitByIndex(i);

		//ignore zombies
		if (pPlotUnit->isDelayedDeath())
			continue;

		//enemies
		if (GET_PLAYER(ePlayer).IsAtWarWith(pPlotUnit->getOwner()))
		{
			//combat units and embarked civilians
			if (pPlotUnit->IsCanDefend())
			{
				//enemy distance for other plots will be set afterwards in refreshVolatilePlotProperties
				//but we need to set the zeros here!
				aiEnemyDistance[TD_BOTH] = 0;
				if (pPlotUnit->getDomainType() == DOMAIN_LAND)
					aiEnemyDistance[TD_LAND] = 0;
				else if (pPlotUnit->getDomainType() == DOMAIN_SEA)
					aiEnemyDistance[TD_SEA] = 0;
			}
			else
				//unembarked civilian
				bEnemyCivilianPresent = true;
		}
		//neutral units
		else if (ePlayer != pPlotUnit->getOwner())
		{
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
			if (MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
			{
				//be conservative and don't use the plot even if we maybe could put an embarked unit here
				if (pPlotUnit->IsCanDefend())
					bPlotBlocked = true;
			}
			else
#endif
				bPlotBlocked = true;
		}
		//owned units not included in sim
		else if (allOurUnits.find(pPlotUnit) == allOurUnits.end())
		{
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
			if (MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
			{
				//again stacking rules are too complex ... simply ignore the plot
				//IsCanDefend catches everybody but unembarked civilians
				//todo: figure out a simple way to allow generals into owned cities with a garrison
				if (pPlotUnit->IsCanDefend())
					bPlotBlocked = true;
			}
			else
#endif
				bPlotBlocked = true;
		}
		else
			bSimUnitPresent = true;
	}

	//annoying details ... embarked units can stack but combat units cannot
	if (bPlotBlocked)
	{
		bBlockedForCombatUnit = true;
		//we don't want to use it unless there's already on of our units there
		if (!bSimUnitPresent)
		{
			pPlot = NULL;
			return;
		}
	}

	if (pPlot->isCity() && GET_PLAYER(ePlayer).IsAtWarWith(pPlot->getOwner()))
	{
		aiEnemyDistance[TD_BOTH] = 0;
		aiEnemyDistance[TD_LAND] = 0;
		aiEnemyDistance[TD_SEA] = 0;
	}
}

void CvTacticalPlot::resetVolatileProperties()
{
	bEdgeOfTheKnownWorld = false;
	bAdjacentToEnemyCitadel = false;
	//set the distance to maximum but only if it's not a plot with an enemy
	if (aiEnemyDistance[TD_BOTH] != 0)
		aiEnemyDistance[TD_BOTH] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
	if (aiEnemyDistance[TD_LAND] != 0)
		aiEnemyDistance[TD_LAND] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
	if (aiEnemyDistance[TD_SEA] != 0)
		aiEnemyDistance[TD_SEA] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
	aiEnemyCombatUnitsAdjacent[TD_BOTH] = 0;
	aiEnemyCombatUnitsAdjacent[TD_LAND] = 0;
	aiEnemyCombatUnitsAdjacent[TD_SEA] = 0;
}

bool CvTacticalPlot::hasFriendlyCombatUnit() const
{
	for (size_t i = 0; i < vUnits.size(); i++)
		if (vUnits[i].isCombatUnit())
			return true;
	
	return false;
}

bool CvTacticalPlot::hasFriendlyEmbarkedUnit() const
{
	for (size_t i = 0; i < vUnits.size(); i++)
		if (vUnits[i].isEmbarkedUnit())
			return true;

	return false;
}

bool CvTacticalPlot::hasSupportBonus(int iIgnoreUnitPlot) const
{
	if (iIgnoreUnitPlot >= 0 && isValid())
	{
		CvPlot* pIgnorePlot = GC.getMap().plotByIndexUnchecked(iIgnoreUnitPlot);
		if (plotDistance(*pIgnorePlot, *pPlot) == 1)
		{
			if (nSupportUnitsAdjacent > 1)
				return true;
		}
		else if (nSupportUnitsAdjacent > 0)
			return true;
	}
	else if (nSupportUnitsAdjacent > 0)
		return true;

	for (size_t i = 0; i < vUnits.size(); i++)
		if (vUnits[i].isSupportUnit())
			return true;

	return false;
}

void CvTacticalPlot::changeNeighboringUnitCount(CvTacticalPosition& currentPosition, const STacticalAssignment& move, int iChange) const
{
	//we don't count embarked units
	if (!pPlot || move.isEmbarkedUnit())
		return;

	CvUnit* pUnit = GET_PLAYER(currentPosition.getPlayer()).getUnit(move.iUnitID);
	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for (int i = 0; i < 6; i++)
	{
		CvPlot* pNeighbor = aNeighbors[i];
		if (pNeighbor)
		{
			CvTacticalPlot& tactPlot = currentPosition.getTactPlotMutable(pNeighbor->GetPlotIndex());
			if (tactPlot.isValid())
			{
				if (move.isSupportUnit())
					tactPlot.nSupportUnitsAdjacent += iChange;
				else //embarked is already handled
				{
					CvTacticalPlot::eTactPlotDomain unitDomain = DomainForUnit(pUnit);
					tactPlot.aiFriendlyCombatUnitsAdjacent[unitDomain] += iChange;
					if (unitDomain!=TD_BOTH)
						tactPlot.aiFriendlyCombatUnitsAdjacent[TD_BOTH] += iChange;
					if (getEnemyDistance() == 1) //hoping that we'll only move appropriate units into frontline positions
					{
						tactPlot.aiFriendlyFirstlineUnitsAdjacent[unitDomain] += iChange;
						if (unitDomain!=TD_BOTH)
							tactPlot.aiFriendlyFirstlineUnitsAdjacent[TD_BOTH] += iChange;
					}
				}
			}
		}
	}
}

void CvTacticalPlot::friendlyUnitMovingIn(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment)
{
	//no more enemies here
	removeEnemyUnitIfPresent();
	bEnemyCivilianPresent = false;

	vUnits.push_back(assignment);

	changeNeighboringUnitCount(currentPosition, assignment, +1);
}

void CvTacticalPlot::friendlyUnitMovingOut(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment)
{
#ifdef TACTDEBUG
	if (vUnits.empty())
		OutputDebugString("invalid move\n");
#endif

	for (vector<STacticalAssignment>::iterator it = vUnits.begin(); it != vUnits.end(); ++it)
		if (it->iUnitID == assignment.iUnitID)
		{
			vUnits.erase(it);
			break;
		}

	changeNeighboringUnitCount(currentPosition, assignment, -1);
}

int CvTacticalPlot::getNumAdjacentFriendlies(eTactPlotDomain eDomain, int iIgnoreUnitPlot) const 
{
	if (iIgnoreUnitPlot >= 0 && isValid())
	{
		CvPlot* pIgnorePlot = GC.getMap().plotByIndexUnchecked(iIgnoreUnitPlot);
		if (plotDistance(*pIgnorePlot, *pPlot) == 1)
			return aiFriendlyCombatUnitsAdjacent[eDomain] -1;
	}

	return aiFriendlyCombatUnitsAdjacent[eDomain];
}

int CvTacticalPlot::getNumAdjacentFirstlineFriendlies(eTactPlotDomain eDomain, int iIgnoreUnitPlot) const 
{ 
	if (iIgnoreUnitPlot >= 0 && isValid())
	{
		CvPlot* pIgnorePlot = GC.getMap().plotByIndexUnchecked(iIgnoreUnitPlot);
		if (plotDistance(*pIgnorePlot, *pPlot) == 1)
			return aiFriendlyFirstlineUnitsAdjacent[eDomain] - 1;
	}

	return aiFriendlyFirstlineUnitsAdjacent[eDomain]; 
}

bool CvTacticalPlot::removeEnemyUnitIfPresent()
{
	bool bReturn = false;

	if (aiEnemyDistance[TD_BOTH] == 0)
	{
		aiEnemyDistance[TD_BOTH] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
		bReturn = true;
	}
	if (aiEnemyDistance[TD_LAND]==0)
	{
		aiEnemyDistance[TD_LAND] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
		bReturn = true;
	}
	if (aiEnemyDistance[TD_SEA]==0)
	{
		aiEnemyDistance[TD_SEA] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
		bReturn = true;
	}

	//need to call refreshVolatilePlotProperties if return value is true
	return bReturn;
}

unsigned char CvTacticalPlot::getEnemyDistance(eTactPlotDomain eDomain) const
{
	return aiEnemyDistance[eDomain];
}

void CvTacticalPlot::setEnemyDistance(eTactPlotDomain eDomain, int iDistance)
{
	aiEnemyDistance[eDomain] = static_cast<unsigned char>(iDistance);
}

bool CvTacticalPlot::checkEdgePlotsForSurprises(const CvTacticalPosition& currentPosition, vector<int>& landEnemies, vector<int>& seaEnemies)
{
	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for (int i = 0; i < 6; i++)
	{
		CvPlot* pNeighbor = aNeighbors[i];
		if (!pNeighbor)
			continue;

		const CvTacticalPlot& tactPlot = currentPosition.getTactPlot(pNeighbor->GetPlotIndex());
		//if the tactical plot is invalid, it's out of range or invisible on the main map.
		//these are the ones we need to check here
		if (!tactPlot.isValid())
		{
			//don't ignore enemy cities, we know they exist even if invisible
			if (pNeighbor->isCity() && GET_PLAYER(currentPosition.getPlayer()).IsAtWarWith(pNeighbor->getOwner()))
			{
				landEnemies.push_back(pNeighbor->GetPlotIndex());
			}
			else if (pNeighbor->isVisible(GET_PLAYER(currentPosition.getPlayer()).getTeam()))
			{
				CvUnit* pEnemy = pNeighbor->getBestDefender(NO_PLAYER, currentPosition.getPlayer(), NULL, true);
				if (pEnemy)
				{
					if (pEnemy->getDomainType() == DOMAIN_LAND)
						landEnemies.push_back(pNeighbor->GetPlotIndex());
					else if (pEnemy->getDomainType() == DOMAIN_SEA)
						seaEnemies.push_back(pNeighbor->GetPlotIndex());
				}
			}
			else
				//if the neighbor is invisible but passable be careful as well, enemies might be hiding there
				bEdgeOfTheKnownWorld = !pNeighbor->isImpassable();

			if (TacticalAIHelpers::IsEnemyCitadel(pNeighbor, currentPosition.getPlayer()))
				bAdjacentToEnemyCitadel = true;
		}

	}

	return bEdgeOfTheKnownWorld;
}

const vector<int>& CvTacticalPosition::getRangeAttackPlotsForUnit(const SUnitStats& unit) const
{
	static vector<int> emptyResult;

	TCachedRangeAttackPlots::const_iterator result = gRangeAttackPlotsLookup.find( make_pair(unit.iUnitID,unit.iPlotIndex) );
	if (result != gRangeAttackPlotsLookup.end())
		return result->second;

	return emptyResult;
}

const ReachablePlots& CvTacticalPosition::getReachablePlotsForUnit(const SUnitStats& unit) const
{
	static ReachablePlots emptyResult;

	SPathFinderStartPos key(unit, freedPlots);
	TCachedMovePlots::const_iterator result = gReachablePlotsLookup.find(key);
	if (result != gReachablePlotsLookup.end())
		return result->second;

	return emptyResult;
}

const STacticalAssignment ScorePlotForMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition, bool bAllowPillage)
{
	return (assumedPosition.getAggressionLevel() > AL_NONE) ? 
		ScorePlotForCombatUnitOffensiveMove(unit, testPlot, movePlot, assumedPosition, bAllowPillage) : 
		ScorePlotForCombatUnitDefensiveMove(unit, testPlot, movePlot, assumedPosition);
}

bool IsCombatUnit(const SUnitStats& unit)
{
	switch (unit.eStrategy)
	{
	case MS_FIRSTLINE:
	case MS_SECONDLINE:
	case MS_THIRDLINE:
		return true;
		break;
	default:
		return false;
	}
}

vector<STacticalAssignment> CvTacticalPosition::getPreferredAssignmentsForUnit(const SUnitStats& unit, int nMaxCount) const
{
	vector<STacticalAssignment> possibleMoves;

	const CvTacticalPlot& assumedUnitPlot = getTactPlot(unit.iPlotIndex);
	CvUnit* pUnit = GET_PLAYER(getPlayer()).getUnit(unit.iUnitID);
	if (!pUnit || !assumedUnitPlot.isValid())
		return vector<STacticalAssignment>();

	//what is the score for simply staying put
	//important not to pass zero moves left, otherwise we might ignore possible attacks
	STacticalAssignment refAssignment(ScorePlotForMove(unit, assumedUnitPlot, SMovePlot(unit.iPlotIndex,0,unit.iMovesLeft,0), *this, false));

	//check moves and melee attacks first
	const ReachablePlots& reachablePlots = getReachablePlotsForUnit(unit);
	for (ReachablePlots::const_iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
	{
		//the result of all this
		STacticalAssignment newAssignment;

		//the plot we're checking right now
		const CvTacticalPlot& testPlot = getTactPlot(it->iPlotIndex);

		//sanity check
		if (!testPlot.isValid())
			continue;

		if (IsCombatUnit(unit))
		{
			if (testPlot.isBlockedForCombatUnit())
				continue;

			if (testPlot.isEnemy())
			{
				//ranged attacks are handled separately below
				if (pUnit->isRanged() || eAggression == AL_NONE)
					continue;

				//for a melee attack we need to move to a defined adjacent plot first
				if (plotDistance(*assumedUnitPlot.getPlot(), *testPlot.getPlot()) > 1)
					continue;

				newAssignment = ScorePlotForMeleeAttack(unit,assumedUnitPlot,testPlot,*it,*this);
			}
			else
			{
				newAssignment = ScorePlotForMove(unit, testPlot, *it, *this, true);
				//may be invalid
				if (newAssignment.iScore < 0)
					continue;

				//important normalization step
				newAssignment.iScore -= refAssignment.iScore;
			}
		}
		else
		{
			//support units or embarked units
			newAssignment = ScorePlotForNonCombatUnitMove(unit, testPlot, *it, *this);
		}

		//score functions are biased so that only scores > 0 are interesting moves
		//still allow mildy negative moves here, maybe we want to do combo moves later!
		if (newAssignment.iScore > -100)
			possibleMoves.push_back(newAssignment);

		if (gTacticalCombatDebugOutput > 0)
		{
			stringstream ss;
			ss << "pos " << getID() << " unit " << unit.iUnitID << " moveto " << it->iPlotIndex << " score " << newAssignment.iScore << "\n";
			OutputDebugString(ss.str().c_str());
		}
	}

	//ranged attacks (also if aggression level is NONE!)
	if (pUnit->isRanged() && unit.iAttacksLeft>0 && unit.iMovesLeft>0)
	{
		const vector<int>& rangeAttackPlots = getRangeAttackPlotsForUnit(unit);
		for (vector<int>::const_iterator it=rangeAttackPlots.begin(); it!=rangeAttackPlots.end(); ++it)
		{
			//the plot we're checking right now
			const CvTacticalPlot& enemyPlot = getTactPlot(*it);

			//note: all valid plots are visible by definition
			if (enemyPlot.isValid() && enemyPlot.isEnemy())
			{
				STacticalAssignment newAssignment(ScorePlotForRangedAttack(unit,assumedUnitPlot,enemyPlot,*this));

				//sanity check
				bool bSuicide = false;
				if (newAssignment.iRemainingMoves == 0 && newAssignment.eAssignmentType != A_RANGEKILL)
				{
					int iDanger = pUnit->GetDanger(assumedUnitPlot.getPlot(), getKilledEnemies());
					int iSpreadFactor = assumedUnitPlot.getNumAdjacentFirstlineFriendlies(CvTacticalPlot::TD_BOTH, unit.iPlotIndex) + 1;
					if (iSpreadFactor == 1 && iDanger > pUnit->GetCurrHitPoints())
						bSuicide = true;
				}

				if (newAssignment.iScore > 0 && !bSuicide)
				{
					//if we're not looking to pick a fight, de-emphasize attacks
					if (eAggression == AL_NONE)
						newAssignment.iScore /= 3;

					possibleMoves.push_back(newAssignment);
				}
			}
		}

		//optimization: if we added a range attack then the end turn move will be generated automatically and the explicit end turn move is not needed
		if (!possibleMoves.empty() && (possibleMoves.back().eAssignmentType == A_RANGEATTACK || possibleMoves.back().eAssignmentType == A_RANGEKILL))
			//end turn should always be first because of the ordering inside reachablePlots
			if (possibleMoves.front().eAssignmentType == A_FINISH)
				possibleMoves.erase(possibleMoves.begin());
	}

	//note: we don't filter out blocked moves. the unit will be considered blocked in this case, even if we have valid but bad moves
	std::sort(possibleMoves.begin(),possibleMoves.end());

	if (possibleMoves.size()>(size_t)nMaxCount)
		possibleMoves.erase( possibleMoves.begin()+nMaxCount, possibleMoves.end() );

	return possibleMoves;
}

//if we have many units we won't look at all of them (for performance reasons)
//obviously needs plot types to be defined
void CvTacticalPosition::dropSuperfluousUnits(int iMaxUnitsToKeep)
{
	//nothing to do?
	if (availableUnits.size() <= (size_t)iMaxUnitsToKeep)
		return;

	//depending on distance to enemy
	int iPlotTypeScore[] = { 0,30,20,10,0 };

	//try to find out who is most relevant
	for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
	{
		const CvTacticalPlot& currentPlot = getTactPlot(itUnit->iPlotIndex);
		itUnit->iImportanceScore = iPlotTypeScore[ currentPlot.getEnemyDistance() ] + 
			currentPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH) + 
			itUnit->iMovesLeft/GC.getMOVE_DENOMINATOR() + getRangeAttackPlotsForUnit(*itUnit).size();

		if (pTargetPlot->isCity() && eAggression > AL_NONE && itUnit->eStrategy == MS_SECONDLINE)
			itUnit->iImportanceScore += 11; //we need siege units for attacking cities
	}

	std::sort(availableUnits.begin(), availableUnits.end());

	//simply consider those extra units as blocked.
	//since addAssignment will modify availableUnits, we copy the relevant units first
	vector<SUnitStats> unitsToDrop( availableUnits.begin()+iMaxUnitsToKeep, availableUnits.end() );
	for (vector<SUnitStats>::iterator itUnit = unitsToDrop.begin(); itUnit != unitsToDrop.end(); ++itUnit)
	{
		STacticalAssignment fakeBlock(itUnit->iPlotIndex,itUnit->iPlotIndex,itUnit->iUnitID,itUnit->iMovesLeft,itUnit->eStrategy,0,A_BLOCKED);
		addAssignment(fakeBlock);
	}
}

void CvTacticalPosition::addInitialAssignments()
{
	for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
	{
		const CvTacticalPlot& tactPlot = getTactPlot(itUnit->iPlotIndex);
		int iScore = ScorePlotForMove(*itUnit, tactPlot, SMovePlot( itUnit->iPlotIndex ), *this, false).iScore;
		addAssignment(STacticalAssignment(itUnit->iPlotIndex, itUnit->iPlotIndex, itUnit->iUnitID, itUnit->iMovesLeft, itUnit->eStrategy, max(-1000,iScore), A_INITIAL));
	}
}

bool CvTacticalPosition::makeNextAssignments(int iMaxBranches, int iMaxChoicesPerUnit, CvTactPosStorage& storage)
{
	/*
	abstract:
	get preferred plots for all units
	choose M best overall moves (combine as far as possible)
	create child positions
		assign moves
		update affected tact plots
		update unit reachable plots
	*/

	if (gTacticalCombatDebugOutput>100)
		dumpPlotStatus( CvString::format("d:\\temp\\plotstatus%06d.dmp",getID()).c_str() );

	//very important, lazy update
	updateMovePlotsIfRequired();

	vector<STacticalAssignment> overAllChoices;
	map<int,vector<STacticalAssignment>> choicePerUnit;

	for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
	{
		vector<STacticalAssignment> thisUnitChoices = getPreferredAssignmentsForUnit(*itUnit, iMaxChoicesPerUnit);

		//oops we're blocked with no valid move
		if (thisUnitChoices.empty())
			thisUnitChoices.push_back( STacticalAssignment(itUnit->iPlotIndex,itUnit->iPlotIndex,itUnit->iUnitID,0,itUnit->eStrategy,0,A_BLOCKED) );

		choicePerUnit[itUnit->iUnitID] = thisUnitChoices;
		overAllChoices.insert( overAllChoices.end(), thisUnitChoices.begin(), thisUnitChoices.end() );
	}

	int iNewBranches = 0;
	std::sort(overAllChoices.begin(), overAllChoices.end());
	for (vector<STacticalAssignment>::iterator itMove = overAllChoices.begin(); itMove != overAllChoices.end(); ++itMove)
	{
		vector<STacticalAssignment> movesToAdd;

		//don't check all possibilities, only the best ones
		if (iNewBranches == iMaxBranches)
			break;

		//don't start with a blocked unit, if that's the best we can do we have a problem
		//really we should check if there is anything else but blocks
		if (itMove->eAssignmentType==A_BLOCKED && overAllChoices.size()>1)
			continue;

		if (isMoveBlockedByOtherUnit(*itMove))
		{
			 //usually there is at most one, but sometimes two
			vector<STacticalAssignment> blocks = findBlockingUnitsAtPlot(itMove->iToPlotIndex, *itMove);
			set<int> blockMoveToPlots;
			for (vector<STacticalAssignment>::iterator itBlock = blocks.begin(); itBlock != blocks.end(); ++itBlock)
			{
				//find best non-blocked move for blocking unit (search only one level deep)
				vector<STacticalAssignment> blockingUnitChoices = choicePerUnit[itBlock->iUnitID];
				for (vector<STacticalAssignment>::iterator itMove2 = blockingUnitChoices.begin(); itMove2 != blockingUnitChoices.end(); ++itMove2)
				{
					if (itMove2->eAssignmentType == A_MOVE && blockMoveToPlots.find(itMove2->iToPlotIndex)==blockMoveToPlots.end())
					{
						//check if the combo has a positive score - but forcing out a similar unit is usually pointless
						int iBias = (itMove->eMoveType != itMove2->eMoveType) ? 0 : 10;
						if (itMove->iScore + itMove2->iScore < iBias)
							continue;

						//block wants to move into the plot currently occupied by this unit. bingo!
						if ( itMove2->iToPlotIndex == itMove->iFromPlotIndex && blocks.size()==1 ) //limit to one block to simplify the logic
						{
							if (blocks[0].eAssignmentType != A_MOVE_SWAP) //don't want to swap out a unit which was just swapped in
							{
								movesToAdd.push_back(*itMove);
								movesToAdd.back().eAssignmentType = A_MOVE_SWAP; //this one will actually move both units
								movesToAdd.push_back(*itMove2);
								movesToAdd.back().eAssignmentType = A_MOVE_SWAP_REVERSE; //this one is just bookkeeping
								//make sure a second block doesn't try to move into the same plot
								blockMoveToPlots.insert(itMove2->iToPlotIndex);
								break;
							}
						}
						else if (!isMoveBlockedByOtherUnit(*itMove2)) //free plot
						{
							//add the move to make space
							movesToAdd.push_back(*itMove2);
							//mark that this is a forced move so we're allowed to move back later
							movesToAdd.back().eAssignmentType = A_MOVE_FORCED;
							//make sure a second block doesn't try to move into the same plot
							blockMoveToPlots.insert(itMove2->iToPlotIndex);
							//now we can do the original move
							if (blockMoveToPlots.size() == blocks.size())
								movesToAdd.push_back(*itMove);
							break;
						}
					}
				}
			}

			//did we move all blocks out of the way?
			if (movesToAdd.size()-1 != blocks.size())
				continue;
		}
		//a plain finish move will have score 0
		else if (itMove->iScore >= 0)
		{
			//just do the original move
			movesToAdd.push_back(*itMove);
		}

		if (!movesToAdd.empty())
		{
			CvTacticalPosition* pNewChild = addChild(storage);
			if (!pNewChild)
				continue;

			for (vector<STacticalAssignment>::iterator itNewMove = movesToAdd.begin(); itNewMove != movesToAdd.end(); ++itNewMove)
			{
				if (!pNewChild->addAssignment(*itNewMove))
				{
					removeChild(pNewChild);
					pNewChild = NULL;
					break;
				}
			}

			//try to detect whether this new position is equivalent to one we already have
			if (parentPosition && pNewChild)
			{
				const vector<CvTacticalPosition*>& siblings = parentPosition->getChildren();
				for (size_t i = 0; i < siblings.size() && pNewChild; i++)
				{
					if (siblings[i] == this)
						continue;

					const vector<CvTacticalPosition*>& nephews = siblings[i]->getChildren();
					for (size_t j = 0; j < nephews.size(); j++)
					{
						//todo: cache the summaries ...
						if (pNewChild->isEquivalent(*nephews[j]))
						{
							removeChild(pNewChild);
							pNewChild = NULL;
							break;
						}
					}
				}
			}

			//count it only if we follow up on it
			if (pNewChild)
				iNewBranches++; 
		}
	}
	
	return (iNewBranches>0);
}

//lazy update of move plots
void CvTacticalPosition::updateMovePlotsIfRequired()
{
	if (movePlotUpdateFlag==-1)
	{
		for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
			updateMoveAndAttackPlotsForUnit(*itUnit);
	}
	else if (movePlotUpdateFlag>0)
	{
		for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
			if (itUnit->iUnitID == movePlotUpdateFlag)
				updateMoveAndAttackPlotsForUnit(*itUnit);
	}

	movePlotUpdateFlag = 0;
}

bool CvTacticalPosition::isMoveBlockedByOtherUnit(const STacticalAssignment& move) const
{
	//only movement can be blocked
	if (move.eAssignmentType != A_MOVE)
		return false;

	return !findBlockingUnitsAtPlot(move.iToPlotIndex, move).empty();
}

vector<STacticalAssignment> CvTacticalPosition::findBlockingUnitsAtPlot(int iPlotIndex, const STacticalAssignment& move) const
{
	vector<STacticalAssignment> result;
	const CvTacticalPlot& tactPlot = getTactPlot(iPlotIndex);
	const vector<STacticalAssignment>& units = tactPlot.getUnitsAtPlot();

	for (size_t i = 0; i < units.size(); i++)
	{
		if (move.isCombatUnit() && units[i].isCombatUnit())
			result.push_back(units[i]);
		if (move.isEmbarkedUnit() && units[i].isEmbarkedUnit())
			result.push_back(units[i]);
		if (move.isSupportUnit() && units[i].isSupportUnit())
			result.push_back(units[i]);
	}

	return result;
}

//can we stop now?
bool CvTacticalPosition::isComplete() const
{ 
	//simple - all enemies are gone (check for non-zero to make sure we aren't trivially complete)
	if (nEnemies>0 && nEnemies==killedEnemies.size())
		return true;

	//if this was an isolated target, we stop as soon as it has been killed (don't overcommit units)
	//if it's a cluster of targets we have to continue because there's only one tactical target for the whole cluster
	if (eAggression > AL_NONE && isIsolatedTarget)
		if (getTactPlot(pTargetPlot->GetPlotIndex()).getEnemyDistance() > 0)
			return true;

	//finally, if we can't move anymore we must be done
	return availableUnits.empty();
}

bool STacticalAssignment::isOffensive() const
{
	//note: a pillage move does not count as offensive!
	if (eAssignmentType == A_MELEEATTACK ||
		eAssignmentType == A_MELEEKILL ||
		eAssignmentType == A_MELEEKILL_NO_ADVANCE ||
		eAssignmentType == A_RANGEATTACK ||
		eAssignmentType == A_RANGEKILL ||
		eAssignmentType == A_CAPTURE)
	{
		return true;
	}

	return false;
}

bool CvTacticalPosition::canStayInPlotUntilNextTurn(SUnitStats unit, int& iNextTurnScore) const
{ 
	//copy the unit ... we want to modify it
	const CvTacticalPlot& tactPlot = getTactPlot(unit.iPlotIndex);

	//the new score for the next turn of combat (assuming the enemy doesn't move ...)
	//note: to make sure we take into account the damage from attacks in the next turn, pretend we have some movement points again
	unit.iMovesLeft = 60;
	unit.iAttacksLeft = 1;
	iNextTurnScore = ScorePlotForMove(unit, tactPlot, SMovePlot(unit.iPlotIndex, 0, unit.iMovesLeft, 0), *this, false).iScore;

	//note that the score may well be lower than the initial score if we killed an enemy!
	//so as long as it's positive, no problem. if it's negative, check if we're better than before
	if (iNextTurnScore < 0)
	{
		//first the reference
		int iInitialScore = 0;
		for (size_t i = 0; i < assignedMoves.size(); i++)
		{
			if (assignedMoves[i].iUnitID == unit.iUnitID && assignedMoves[i].eAssignmentType == A_INITIAL)
			{
				iInitialScore = assignedMoves[i].iScore;
				break;
			}
		}

		//initial score could have been even more negative
		iNextTurnScore -= iInitialScore;
	}

	return iNextTurnScore >= 0;
}

//see if all the plots where our units would end their turn are acceptable
//this is a deferred check because in the beginning it's not clear how many enemy units we can eliminate
bool CvTacticalPosition::addFinishMovesIfAcceptable()
{ 
	//only units we have moved can be in this array
	while (!notQuiteFinishedUnits.empty())
	{
		SUnitStats unit = notQuiteFinishedUnits.back();
		int iNextTurnScore = 0;
		if (canStayInPlotUntilNextTurn(unit,iNextTurnScore))
		{
			//blocked units may be used for other moves, but make sure we don't leave them in an impossible position
			if (unit.eLastAssignment != A_BLOCKED)
				assignedMoves.push_back(STacticalAssignment(unit.iPlotIndex, unit.iPlotIndex, unit.iUnitID, 0, unit.eStrategy, iNextTurnScore, A_FINISH));

			notQuiteFinishedUnits.pop_back();
		}
		else
			return false;
	}

	//also check units we didn't move but which might be important to block the enemy
	for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
	{
		const CvTacticalPlot& tactPlot = getTactPlot(itUnit->iPlotIndex);

		//ignore units without contact to the enemy, can use them for other tasks
		if (tactPlot.getEnemyDistance()>1)
			continue;

		int iNextTurnScore = 0;
		if (canStayInPlotUntilNextTurn(*itUnit,iNextTurnScore))
		{
			assignedMoves.push_back(STacticalAssignment(itUnit->iPlotIndex, itUnit->iPlotIndex, itUnit->iUnitID, 0, itUnit->eStrategy, iNextTurnScore, A_FINISH));
		}
		else
			return false;
	}

	return true;
}

bool CvTacticalPosition::hasOffensiveAssignments() const
{
	for (size_t i = 0; i < assignedMoves.size(); i++)
		if (assignedMoves[i].isOffensive())
			return true;

	return false;
}

//this influences how daring we'll be
void CvTacticalPosition::countEnemies()
{
	if (parentPosition != NULL)
		OutputDebugString("countEnemies() should only be called for the root position\n");

	//todo: should we count non-simulated friendlies as well?
	nOurUnits = availableUnits.size();
	nEnemies = 0;

	for (size_t i = 0; i < tactPlots.size(); i++)
	{
		if (tactPlots[i].isEnemy())
			nEnemies++; //units and cities
	}

	//for offense we need to know this
	if (eAggression > AL_NONE)
		isIsolatedTarget = (pTargetPlot->GetNumEnemyUnitsAdjacent(GET_PLAYER(ePlayer).getTeam(), NO_DOMAIN) == 0);
	else
		isIsolatedTarget = false;
}


void CvTacticalPosition::refreshVolatilePlotProperties()
{
	vector<int> landEnemies;
	vector<int> seaEnemies;
	vector<int> citadels;

	for (vector<CvTacticalPlot>::iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
	{
		if (it->getEnemyDistance(CvTacticalPlot::TD_LAND) == 0)
			landEnemies.push_back( it->getPlotIndex() );
		if (it->getEnemyDistance(CvTacticalPlot::TD_SEA) == 0)
			seaEnemies.push_back( it->getPlotIndex() );

		it->resetVolatileProperties();

		//include plots with enemies if they are just outside of the simulation range
		//we won't attack them but we won't ignore them either
		it->checkEdgePlotsForSurprises(*this,landEnemies,seaEnemies);

		if (TacticalAIHelpers::IsEnemyCitadel(it->getPlot(), getPlayer()) && !plotHasAssignmentOfType(it->getPlotIndex(), A_PILLAGE))
			citadels.push_back(it->getPlotIndex());
	}

	//distance transform pt1
	for (size_t i=0; i<landEnemies.size(); i++)
		for (vector<CvTacticalPlot>::iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
		{
			int iDistance = plotDistance(*GC.getMap().plotByIndexUnchecked(landEnemies[i]), *it->getPlot());
			if (iDistance < it->getEnemyDistance(CvTacticalPlot::TD_BOTH))
				it->setEnemyDistance(CvTacticalPlot::TD_BOTH, iDistance);
			if (iDistance < it->getEnemyDistance(CvTacticalPlot::TD_LAND))
				it->setEnemyDistance(CvTacticalPlot::TD_LAND, iDistance);
			if (iDistance == 1)
			{
				it->setNumAdjacentEnemies(CvTacticalPlot::TD_BOTH,it->getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH)+1);
				it->setNumAdjacentEnemies(CvTacticalPlot::TD_LAND,it->getNumAdjacentEnemies(CvTacticalPlot::TD_LAND)+1);
			}
		}

	//pt2
	for (size_t i=0; i<seaEnemies.size(); i++)
		for (vector<CvTacticalPlot>::iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
		{
			int iDistance = plotDistance(*GC.getMap().plotByIndexUnchecked(seaEnemies[i]), *it->getPlot());
			if (iDistance < it->getEnemyDistance(CvTacticalPlot::TD_BOTH))
				it->setEnemyDistance(CvTacticalPlot::TD_BOTH, iDistance);
			if (iDistance < it->getEnemyDistance(CvTacticalPlot::TD_SEA))
				it->setEnemyDistance(CvTacticalPlot::TD_SEA, iDistance);
			if (iDistance == 1)
			{
				it->setNumAdjacentEnemies(CvTacticalPlot::TD_BOTH,it->getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH)+1);
				it->setNumAdjacentEnemies(CvTacticalPlot::TD_SEA,it->getNumAdjacentEnemies(CvTacticalPlot::TD_SEA)+1);
			}
		}

	//citadels
	for (size_t i=0; i<citadels.size(); i++)
	{
		//iterate neighbors
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(GC.getMap().plotByIndexUnchecked(citadels[i]));
		for (int i = 0; i < 6; i++)
		{
			CvPlot* pNeighbor = aNeighbors[i];
			if (!pNeighbor)
				continue;

			CvTacticalPlot& tactPlot = getTactPlotMutable(pNeighbor->GetPlotIndex());
			if (tactPlot.isValid())
				tactPlot.setNextToEnemyCitadel(true);
		}
	}
}

CvTacticalPosition::CvTacticalPosition()
{
	ePlayer = NO_PLAYER;
	pTargetPlot = NULL;
	eAggression = AL_NONE;
	nOurUnits = 0;
	nEnemies = 0;
	isIsolatedTarget = false;
	iTotalScore = 0;
	iScoreOverParent = 0; 
	parentPosition = NULL;
	iID = 0;
	movePlotUpdateFlag = 0;
	//all the rest is default-initialized
}

void CvTacticalPosition::initFromScratch(PlayerTypes player, eAggressionLevel eAggLvl, CvPlot* pTarget)
{
	ePlayer = player;
	pTargetPlot = pTarget;
	eAggression = eAggLvl;
	nOurUnits = 0;
	nEnemies = 0;
	isIsolatedTarget = false;
	iTotalScore = 0;
	iScoreOverParent = 0; 
	parentPosition = NULL;
	iID = 1; //zero doesn't work here
	movePlotUpdateFlag = 0;

	childPositions.clear();
	tacticalPlotLookup.clear();
	tactPlots.clear();
	availableUnits.clear();
	notQuiteFinishedUnits.clear();
	assignedMoves.clear();
	summary.clear();
	freedPlots.clear();
	killedEnemies.clear();
}

void CvTacticalPosition::initFromParent(const CvTacticalPosition& parent)
{
	dummyPlot = parent.dummyPlot;
	ePlayer = parent.ePlayer;
	pTargetPlot = parent.pTargetPlot;
	eAggression = parent.eAggression;
	nOurUnits = parent.nOurUnits;
	nEnemies = parent.nEnemies;
	isIsolatedTarget = parent.isIsolatedTarget;
	iTotalScore = parent.iTotalScore;
	iScoreOverParent = 0;
	parentPosition = &parent;
	movePlotUpdateFlag = parent.movePlotUpdateFlag;

	//clever scheme to encode the tree structure into IDs
	//works only if the tree is not too wide or too deep
	if (parent.getID() < ULLONG_MAX / 10 - 10)
		iID = parent.getID() * 10 + parent.childPositions.size();
	else
		iID = INT_MAX;

	//childPositions stays empty!
	childPositions.clear();

	//these are cached locally
	tacticalPlotLookup.clear();
	tactPlots.clear();

	//copied from parent, modified when addAssignment is called
	availableUnits = parent.availableUnits;
	notQuiteFinishedUnits = parent.notQuiteFinishedUnits;
	assignedMoves = parent.assignedMoves;
	summary = parent.summary;
	freedPlots = parent.freedPlots;
	killedEnemies = parent.killedEnemies;
}

bool CvTacticalPosition::removeChild(CvTacticalPosition* pChild)
{
	//just unlink the child - do not delete it, the memory is allocated statically
	vector<CvTacticalPosition*>::iterator it = find(childPositions.begin(), childPositions.end(), pChild);
	if (it!=childPositions.end())
		childPositions.erase(it);

	return false;
}

CvTacticalPosition* CvTacticalPosition::addChild(CvTactPosStorage& storage)
{
	CvTacticalPosition* newPosition = storage.getNext();
	if (newPosition)
	{
		childPositions.push_back(newPosition); //this order is better for generating an ID for the child
		newPosition->initFromParent(*this);
	}
	return newPosition;
}

void CvTacticalPosition::getPlotsWithChangedVisibility(const STacticalAssignment& assignment, vector<int>& madeVisible) const
{
	madeVisible.clear();

	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(assignment.iUnitID);
	CvPlot* pNewPlot = GC.getMap().plotByIndexUnchecked(assignment.iToPlotIndex);

	for (int i=1; i<RING3_PLOTS; i++)
	{
		CvPlot* pTestPlot = iterateRingPlots(pNewPlot,i); //iterate around the new plot, not the old plot
		if (!pTestPlot)
			continue;

		//todo: check for distance to target? TACTICAL_COMBAT_MAX_TARGET_DISTANCE

		if (pTestPlot->getVisibilityCount(pUnit->getTeam())==0)
		{
			if (pNewPlot->canSeePlot(pTestPlot, pUnit->getTeam(), pUnit->visibilityRange(), pUnit->getFacingDirection(true)))
				madeVisible.push_back(pTestPlot->GetPlotIndex());
		}
	}
}

int gMoveCacheHit = 0, gMoveCacheMiss = 0;
int gAttackCacheHit = 0, gAttackCacheMiss = 0;

void CvTacticalPosition::updateMoveAndAttackPlotsForUnit(SUnitStats unit)
{
	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(unit.iUnitID);
	CvPlot* pStartPlot = GC.getMap().plotByIndexUnchecked(unit.iPlotIndex);

	TCachedMovePlots::iterator itP = gReachablePlotsLookup.find(SPathFinderStartPos(unit, freedPlots));
	if (itP != gReachablePlotsLookup.end())
		gMoveCacheHit++;
	else
	{
		gMoveCacheMiss++;

		//note: we allow (intermediate) embarkation here but filter out the non-native plots later (useful for denmark and lategame)
		int iMoveFlags = CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_DANGER;
		ReachablePlots reachablePlots = TacticalAIHelpers::GetAllPlotsInReachThisTurn(pUnit, pStartPlot, iMoveFlags, 0, unit.iMovesLeft, freedPlots);

		//need to know this if we're doing defensive positioning
		bool bTargetIsEnemy = pTargetPlot->isEnemyUnit(ePlayer, true, true) || pTargetPlot->isEnemyCity(*pUnit);
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);

		//try to save some memory here
		ReachablePlots reachablePlotsPruned;
		for (ReachablePlots::iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

			//note that if the unit is far away, it won't have any good plots and will be considered blocked
			if (plotDistance(*pPlot, *pTargetPlot) > TACTICAL_COMBAT_MAX_TARGET_DISTANCE)
				continue;

			if (unit.eStrategy == MS_ESCORTED_EMBARKED)
			{
				//we are embarked and stay embarked, so we can stack with combat units

				//only allow disembarking if it takes us closer to the target
				if (pUnit->isNativeDomain(pPlot) && plotDistance(*pPlot, *pTargetPlot) >= plotDistance(*pUnit->plot(), *pTargetPlot))
					continue;
			}
			else
			{
				//if we want to fight
				if (eAggression != AL_NONE)
				{
					//ignore all plots where we cannot fight. allow melee ships to capture cities though!
					if (!pUnit->isNativeDomain(pPlot) && !pPlot->isEnemyCity(*pUnit))
						continue;
				}
				else
				{
					//we don't want to fight so embarkation is ok if we don't need to stack with other (non-simulated) embarked units
					if (!pUnit->isNativeDomain(pPlot))
					{
						//for embarked units, every attacker is bad news
						if (bTargetIsEnemy || !kPlayer.GetPossibleAttackers(*pPlot, NO_TEAM).empty())
							continue;

						CvTacticalDominanceZone* pZone = GET_PLAYER(ePlayer).GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pPlot);
						if (pZone && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
							continue;
					}
				}
			}

			reachablePlotsPruned.insert(*it);
		}

		gReachablePlotsLookup[SPathFinderStartPos(unit, freedPlots)] = reachablePlotsPruned;
	}

	//simply ignore visibility here, later there's a check if there is a valid tactical plot for the targets
	if (eAggression > AL_NONE)
	{
		TCachedRangeAttackPlots::iterator itA = gRangeAttackPlotsLookup.find(make_pair(unit.iUnitID, unit.iPlotIndex));
		if (itA != gRangeAttackPlotsLookup.end())
			gAttackCacheHit++;
		else
		{
			gAttackCacheMiss++;
			vector<int> rangeAttackPlots = TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit, pStartPlot, true, true);
			gRangeAttackPlotsLookup[make_pair(unit.iUnitID, unit.iPlotIndex)] = rangeAttackPlots;
		}
	}
}

bool CvTacticalPosition::unitHasAssignmentOfType(int iUnitID, eUnitAssignmentType assignmentType) const
{
	for (vector<STacticalAssignment>::const_iterator it = assignedMoves.begin(); it != assignedMoves.end(); ++it)
		if (it->iUnitID == iUnitID && it->eAssignmentType == assignmentType)
			return true;

	return false;
}

bool CvTacticalPosition::plotHasAssignmentOfType(int iToPlotIndex, eUnitAssignmentType assignmentType) const
{
	for (vector<STacticalAssignment>::const_iterator it = assignedMoves.begin(); it != assignedMoves.end(); ++it)
		if (it->iToPlotIndex == iToPlotIndex && it->eAssignmentType == assignmentType)
			return true;

	return false;
}

//we should err on the side of caution here. better to allow two equivalent positions than to ignore a good one
const SAssignmentSummary& CvTacticalPosition::updateSummary(const STacticalAssignment& newAssignment)
{
	switch (newAssignment.eAssignmentType)
	{
	//these assigments change the unit's plot
	case A_INITIAL:
	case A_MOVE:
	case A_CAPTURE:
	case A_MOVE_FORCED:
	case A_MOVE_SWAP:
	case A_MOVE_SWAP_REVERSE:
		summary.unitPlots[newAssignment.iUnitID] = newAssignment.iToPlotIndex;
		break;

	//ignore those, they don't change the plot
	case A_FINISH:
	case A_BLOCKED:
	case A_RESTART:
	case A_PILLAGE:
		break;

	//attacks without plot change
	case A_MELEEATTACK:
	case A_RANGEATTACK:
	case A_RANGEKILL:
	case A_MELEEKILL_NO_ADVANCE:
		//note: the ordering between moves and attack is important for flanking bonuses. so we look at the damage here
		summary.attackedPlots[newAssignment.iToPlotIndex].push_back(newAssignment.iDamage);
		break;

	//attack with plot change
	case A_MELEEKILL:
		summary.attackedPlots[newAssignment.iToPlotIndex].push_back(newAssignment.iDamage);
		summary.unitPlots[newAssignment.iUnitID] = newAssignment.iToPlotIndex;
		break;
	}

	return summary;
}

static int giEquivalent = 0;
static int giDifferent = 0;

//try to detect simple permutations in the unit assigments which result in equivalent results
bool CvTacticalPosition::isEquivalent(const CvTacticalPosition & rhs) const
{
	if (assignedMoves.size() != rhs.assignedMoves.size())
		return false;

	if (assignedMoves.size() < 3)
		return false;

	if (summary == rhs.summary)
	{
		giEquivalent++;
		return true;
	}
	else
	{
		giDifferent++;
		return false;
	}
}

pair<int,int> CvTacticalPosition::doVisibilityUpdate(const STacticalAssignment& newAssignment)
{
	vector<int> newlyVisiblePlots;
	int nNewEnemies = 0;

	//may need to add some new tactical plots - ideally we should reconsider all queued assignments afterwards
	//the next round of assignments will take into account the new plots in any case
	getPlotsWithChangedVisibility(newAssignment, newlyVisiblePlots);
	for (size_t i=0; i<newlyVisiblePlots.size(); i++)
	{
		//since it was invisible before, we know there are no friendly units around
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(newlyVisiblePlots[i]);
		if (pPlot) //also create plots for neutral units - otherwise edgeOfTheKnownWorld is not correct
		{
			//can pass empty set of units - the plot was invisible before so we know there is none of our units there
			addTacticalPlot(pPlot,set<CvUnit*>()); 
			//if we found a new enemy unit, update some flags
			if ( tactPlots.back().getEnemyDistance()==0 )
				refreshVolatilePlotProperties();

			//we revealed a new enemy ... need to execute moves up to here, do a danger plot update and reconsider
			if (pPlot->isEnemyUnit(ePlayer, true, false))
				nNewEnemies++;
		}
	}

	return make_pair<int, int>( (int)newlyVisiblePlots.size(), nNewEnemies);
}

bool CvTacticalPosition::addAssignment(const STacticalAssignment& newAssignment)
{
	//if we killed an enemy ZOC will change
	bool bRecomputeAllMoves = false;
	//newly visible plots, newly visible enemies
	std::pair<int, int> visibilityResult(0, 0);
	
	vector<SUnitStats>::iterator itUnit = find_if(availableUnits.begin(), availableUnits.end(), PrMatchingUnit(newAssignment.iUnitID));
	if (itUnit == availableUnits.end())
		return false;

	//tactical plots are only touched for "real" moves. blocked units may be on invalid plots.
	if (newAssignment.eAssignmentType != A_BLOCKED)
	{
		if (!getTactPlotMutable(newAssignment.iToPlotIndex).isValid())
			return false;
		//a unit may start out on an invalid plot (eg. too far away)
		if (!getTactPlotMutable(newAssignment.iFromPlotIndex).isValid() && itUnit->eLastAssignment!=A_INITIAL)
			return false;
	}

	//i know what you did last summer!
	itUnit->eLastAssignment = newAssignment.eAssignmentType;

	//store the assignment
	assignedMoves.push_back(newAssignment);
	updateSummary(newAssignment);

	//now deal with the consequences
	switch (newAssignment.eAssignmentType)
	{
	case A_INITIAL:
		getTactPlotMutable(newAssignment.iToPlotIndex).friendlyUnitMovingIn(*this, newAssignment);
		break;
	case A_MOVE:
#ifdef TACTDEBUG
		{
			//plausi checks
			vector<STacticalAssignment> fromBlocks = findBlockingUnitsAtPlot(newAssignment.iFromPlotIndex, newAssignment);
			vector<STacticalAssignment> toBlocks = findBlockingUnitsAtPlot(newAssignment.iToPlotIndex, newAssignment);

			bool bFound = false;
			for (vector<STacticalAssignment>::iterator itBlock = fromBlocks.begin(); itBlock != fromBlocks.end(); ++itBlock)
				if (itBlock->iUnitID == newAssignment.iUnitID)
					bFound = true;

			if (!bFound)
				OutputDebugString("inconsistent origin\n");

			if (!toBlocks.empty())
				OutputDebugString("inconsistent destination\n");

			if (newAssignment.iRemainingMoves > itUnit->iMovesLeft)
				OutputDebugString("inconsistent moves!\n");
		}
#endif
		//fall through!
	case A_MOVE_FORCED:
	case A_CAPTURE:
	case A_MOVE_SWAP:
	case A_MOVE_SWAP_REVERSE:
	{
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iPlotIndex = newAssignment.iToPlotIndex;
		getTactPlotMutable(newAssignment.iFromPlotIndex).friendlyUnitMovingOut(*this, newAssignment);
		getTactPlotMutable(newAssignment.iToPlotIndex).friendlyUnitMovingIn(*this, newAssignment);

		visibilityResult = doVisibilityUpdate(newAssignment);
		//in case this was a move which revealed new plots, pretend it was a forced move so we can move again
		if (visibilityResult.first > 0 && newAssignment.eAssignmentType == A_MOVE)
			itUnit->eLastAssignment = A_MOVE_FORCED;
		break;
	}
	case A_RANGEATTACK:
	case A_MELEEATTACK:
	{
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iAttacksLeft--;
		itUnit->iSelfDamage += newAssignment.iSelfDamage;
		getTactPlotMutable(newAssignment.iToPlotIndex).changeDamage(newAssignment.iDamage);
		break;
	}
	case A_RANGEKILL:
	{
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iAttacksLeft--;
		getTactPlotMutable(newAssignment.iToPlotIndex).removeEnemyUnitIfPresent();
		refreshVolatilePlotProperties();
		bRecomputeAllMoves = true; //ZOC changed
		CvUnit* pEnemy = GC.getMap().plotByIndexUnchecked(newAssignment.iToPlotIndex)->getVisibleEnemyDefender(ePlayer);
		if (pEnemy)
		{
			freedPlots.push_back(newAssignment.iToPlotIndex);
			killedEnemies.push_back(pEnemy->GetID());
		}
		break;
	}
	case A_MELEEKILL_NO_ADVANCE:
	case A_MELEEKILL:
	{
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iAttacksLeft--;
		itUnit->iSelfDamage += newAssignment.iSelfDamage;

		if (newAssignment.eAssignmentType == A_MELEEKILL)
		{
			itUnit->iPlotIndex = newAssignment.iToPlotIndex; //this is because we're advancing
			getTactPlotMutable(newAssignment.iFromPlotIndex).friendlyUnitMovingOut(*this, newAssignment); //this is because we're advancing
			getTactPlotMutable(newAssignment.iToPlotIndex).friendlyUnitMovingIn(*this, newAssignment); //this implicitly removes the enemyUnit flag
			//do this before the distance update
			visibilityResult = doVisibilityUpdate(newAssignment);
		}
		else //NO_ADVANCE
			getTactPlotMutable(newAssignment.iToPlotIndex).removeEnemyUnitIfPresent();

		CvPlot* pFutureExEnemyPlot = GC.getMap().plotByIndexUnchecked(newAssignment.iToPlotIndex);
		CvUnit* pEnemy = pFutureExEnemyPlot->getVisibleEnemyDefender(ePlayer);
		if (pEnemy)
		{
			//killing a unit or a garrisoned city
			killedEnemies.push_back(pEnemy->GetID());
		}
		else if (pFutureExEnemyPlot->isCity())
		{
			//put an invalid unit ID as a placeholder so that isComplete() works
			killedEnemies.push_back(0);
		}

		freedPlots.push_back(newAssignment.iToPlotIndex);
		//important that we do the visibility update first!
		refreshVolatilePlotProperties();
		bRecomputeAllMoves = true; //ZOC changed
		break;
	}
	case A_PILLAGE:
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		if (TacticalAIHelpers::IsEnemyCitadel( GC.getMap().plotByIndexUnchecked(newAssignment.iToPlotIndex), getPlayer()))
			refreshVolatilePlotProperties();
		break;
	case A_FINISH:
	case A_BLOCKED:
		itUnit->iMovesLeft = 0;
		//todo: mark end turn plot? as opposed to transient blocks
		break;
	}

	//we don't update the moves plots lazily because it takes a while and we don't know yet if we will ever follow up on this position
	if (bRecomputeAllMoves)
	{
		movePlotUpdateFlag = -1; //need to update all
	}
	else if (itUnit->iMovesLeft>0)
	{
		//make sure we don't regress to a "lower" level
		if (movePlotUpdateFlag==0) 
			movePlotUpdateFlag = itUnit->iUnitID; //need to update only this one
		else if (movePlotUpdateFlag>0)
			movePlotUpdateFlag = -1; //need to update multiple units, simply do all
	}

	//forced moves don't even affect the score
	if (newAssignment.eAssignmentType != A_MOVE_FORCED && newAssignment.eAssignmentType != A_MOVE_SWAP_REVERSE)
	{
		iScoreOverParent += newAssignment.iScore;
		iTotalScore += newAssignment.iScore;
	}

	//when in doubt, increasing our visibility is good
	iScoreOverParent += visibilityResult.first;
	iTotalScore += visibilityResult.first;

	//are we done or can we do further moves with this unit?
	if (itUnit->iMovesLeft == 0)
	{
		if (itUnit->eLastAssignment != A_FINISH)
			notQuiteFinishedUnits.push_back(*itUnit);

		availableUnits.erase(itUnit);
	}

	//todo: should we stop the simulation? how to include this in position scoring?
	//don't do restarts if we have a lot of units, the simulation can take very long then
	//also don't do a restart if this was the last unit
	//any new enemies in sight?
	bool bRestartRequired = (visibilityResult.second > 0);
	if (bRestartRequired && availableUnits.size()<8 && availableUnits.size()>0)
		assignedMoves.push_back(STacticalAssignment( -1, -1, newAssignment.iUnitID, 0, newAssignment.eMoveType, 0, A_RESTART));

	return true;
}

bool CvTacticalPosition::haveTacticalPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return true; //this is a matter of definition ...

	if (tacticalPlotLookup.find(pPlot->GetPlotIndex()) != tacticalPlotLookup.end())
		return true;

	if (parentPosition && parentPosition->haveTacticalPlot(pPlot))
		return true;

	return false;
}

void CvTacticalPosition::addTacticalPlot(const CvPlot* pPlot, const set<CvUnit*>& allOurUnits)
{
	//don't check the official visibility here, we might want to create a tactplot that only became visible during simulation
	if (!pPlot)
		return;

	TTactPlotLookup::iterator it = tacticalPlotLookup.find(pPlot->GetPlotIndex());
	if (it != tacticalPlotLookup.end())
		return; //nothing to do

	if (parentPosition && parentPosition->haveTacticalPlot(pPlot))
		return; //nothing to do

	CvTacticalPlot newPlot(pPlot, ePlayer, allOurUnits);
	if (newPlot.isValid())
	{
		tacticalPlotLookup[pPlot->GetPlotIndex()] = (int)tactPlots.size();
		tactPlots.push_back(newPlot);
	}
}

bool CvTacticalPosition::addAvailableUnit(const CvUnit* pUnit)
{
	if (!pUnit || !pUnit->canMove())
		return false;

	eUnitMovementStrategy eStrategy = MS_NONE;

	//ok this is a bit involved
	//case a) we only have combat units and we want to fight. units should stay in their native domain so they can fight.
	//case b) we have escorted units which are embarked or need to embark and are not expected to fight. embarked units can stack with their escort but yet-unembarked units cannot stack.
	//later in updateMoveAndAttackPlotsForUnits we try and filter the reachable plots according to unit strategy
	//also, only land units can embark and but melee ships can move into certain land plots (cities) so it's tricky

	//dimensions
	// - attacking / defending
	// - combat unit / non-combat unit
	// - native domain target / non-native domain target

	if (eAggression == AL_NONE && !pUnit->isNativeDomain(pTargetPlot) && pUnit->CanEverEmbark())
	{
		// if the target is outside of our native domain, we will need to embark sooner or later
		if ( pUnit->isEmbarked() )
			eStrategy = MS_ESCORTED_EMBARKED; //already embarked, can stack
		else if (pUnit->IsCombatUnit())
			eStrategy = MS_THIRDLINE; //not yet embarked, don't stack
		else
			eStrategy = MS_SUPPORT;
	}
	else if (eAggression == AL_NONE && pUnit->isNativeDomain(pTargetPlot) && pUnit->isEmbarked())
	{
		// target is in native domain, but we are embarked and will need to disembark sooner or later
		eStrategy = MS_THIRDLINE; //do not stack
	}
	else
	{
		//normal combat units
		switch (pUnit->getUnitInfo().GetDefaultUnitAIType())
		{
			//front line units
		case UNITAI_ATTACK:
		case UNITAI_DEFENSE:
		case UNITAI_COUNTER:
		case UNITAI_PARADROP:
		case UNITAI_ATTACK_SEA:
		case UNITAI_RESERVE_SEA:
		case UNITAI_ESCORT_SEA:
		case UNITAI_FAST_ATTACK:
			//ranged units
		case UNITAI_RANGED:
		case UNITAI_CITY_BOMBARD:
		case UNITAI_ASSAULT_SEA:
			if (pUnit->GetRange() > 2)
				eStrategy = MS_THIRDLINE;
			else if (pUnit->GetRange() == 2)
				eStrategy = MS_SECONDLINE;
			else
			{
				//the unit AI type is unreliable, so we do this manually
				if (pUnit->isRanged())
					eStrategy = MS_SECONDLINE; //skirmishers are second line always
				else
					eStrategy = MS_FIRSTLINE; //regular melee
			}
			break;
		//carriers should stay back
		case UNITAI_CARRIER_SEA:
			eStrategy = MS_THIRDLINE;
			break;

		//explorers are an edge case, the visibility can be useful, so include them
		//they shouldn't fight if the odds are bad
		case UNITAI_EXPLORE:
		case UNITAI_EXPLORE_SEA:
			eStrategy = MS_FIRSTLINE;
			break;

		//combat support, stay out of danger
		case UNITAI_GENERAL:
		case UNITAI_ADMIRAL:
		case UNITAI_CITY_SPECIAL:
			eStrategy = MS_SUPPORT;
			break;

		//air units. ignore here, attack / rebase is handled elsewhere
		case UNITAI_ATTACK_AIR:
		case UNITAI_DEFENSE_AIR:
		case UNITAI_MISSILE_AIR:
		case UNITAI_ICBM:
		default:
			//skip the unit (other civilians as well)
			return false;
		}
	}

	//we will update the importance later, use 0 for now
	availableUnits.push_back( SUnitStats( pUnit, 0, eStrategy ) );

	movePlotUpdateFlag = -1; //lazy update of move plots later
	return true;
}

int CvTacticalPosition::countChildren() const
{
	int iCount = (int)childPositions.size();
	for (size_t i = 0; i < childPositions.size(); i++)
		iCount += childPositions[i]->countChildren();

	return iCount;
}

float CvTacticalPosition::getAggressionBias() const
{
	//avoid extreme ratios, use the sqrt
	float fUnitNumberRatio = sqrtf(nOurUnits / float(max(1,(int)nEnemies)));

	return max( 0.9f, fUnitNumberRatio ); //<1 indicates we're fewer but don't stop attacking because of that
}

std::ostream& operator<<(ostream& os, const CvPlot& p)
{
    os << "(" << p.getX() << "," << p.getY() << ")";
    return os;
}

ostream& operator << (ostream& out, const STacticalAssignment& arg)
{
	const char* eType = assignmentTypeNames[arg.eAssignmentType];
	//CvPlot* pFromPlot = GC.getMap().plotByIndexUnchecked( arg.iFromPlotIndex );
	//CvPlot* pToPlot = GC.getMap().plotByIndexUnchecked( arg.iToPlotIndex );
	out << arg.iUnitID << " " << eType << " from " << arg.iFromPlotIndex << " to " << arg.iToPlotIndex << " (" << arg.iScore << ")";
	return out;
}

void CvTacticalPosition::dumpChildren(ofstream& out) const
{
	out << "n" << (void*)this << " [ label = \"id " << (void*)this << ": score " << iTotalScore << ", " << availableUnits.size() << " units\" ";
	if (isComplete())
		out << " shape=box ";
	out << "];\n";

	size_t nAssignments = assignedMoves.size();
	for (size_t i = 0; i < childPositions.size(); i++)
	{
		out << "n" << (void*)this << " -> n" << (void*)childPositions[i] << " [ label = \"";

		size_t nAssignmentsChild = childPositions[i]->getAssignments().size();
		for (size_t j = nAssignments; j < nAssignmentsChild; j++)
		{
			const STacticalAssignment& assignment = childPositions[i]->getAssignments()[j];
			CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(assignment.iUnitID);
			out << pUnit->getName().c_str() << " " << assignment << "\\n";
		}
		out << "\" color=blue ];\n";
	}

	for (size_t i = 0; i < childPositions.size(); i++)
		childPositions[i]->dumpChildren(out);
}

void CvTacticalPosition::dumpPlotStatus(const char* fname) const
{
	ofstream out(fname);
	if (out)
	{
		out << "#x,y,terrain,isEnemy,isFriendly,nAdjEnemy,nAdjFriendly,nAdjFirstline,isEdge,hasSupport,eEnemyDist\n"; 
		for (vector<CvTacticalPlot>::const_iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
		{
			CvPlot* pPlot =  GC.getMap().plotByIndexUnchecked( it->getPlotIndex() );
			out << pPlot->getX() << "," << pPlot->getY() << "," << pPlot->getTerrainType() << "," << (it->isEnemy() ? 1 : 0) << "," << (it->hasFriendlyCombatUnit() ? 1 : 0) << "," 
				<< it->getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH) << "," << it->getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH,-1) << "," << it->getNumAdjacentFirstlineFriendlies(CvTacticalPlot::TD_BOTH,-1) << "," 
				<< it->isEdgePlot() << ","<< it->hasSupportBonus(-1) << "," << it->getEnemyDistance() << "\n";
		}
	}
	out.close();
}

void CvTacticalPosition::exportToDotFile(const char* fname) const
{
	std::ofstream out;
	out.open(fname);
	if (out)
	{
		out << "digraph tacticalmoves {\n";
		dumpChildren(out);
		out << "}\n";
	}
	out.close();
}

//warning: only keep the reference returned around if you know what you are doing!
//it may get invalidated by additional calls to this function!
CvTacticalPlot& CvTacticalPosition::getTactPlotMutable(int plotindex)
{
	TTactPlotLookup::const_iterator it = tacticalPlotLookup.find(plotindex);
	if (it!=tacticalPlotLookup.end())
		return tactPlots[it->second];

	if (parentPosition)
	{
		const CvTacticalPlot& parentResult = parentPosition->getTactPlot(plotindex);
		if (parentResult.isValid())
		{
			//now cache it locally so that we can modify it
			tacticalPlotLookup[plotindex] = (int)tactPlots.size();
			tactPlots.push_back(parentResult);
			return tactPlots.back();
		}
	}

	return dummyPlot;
}

const CvTacticalPlot& CvTacticalPosition::getTactPlot(int plotindex) const
{
	TTactPlotLookup::const_iterator it = tacticalPlotLookup.find(plotindex);
	if (it!=tacticalPlotLookup.end())
		return tactPlots[it->second];

	if (parentPosition)
		return parentPosition->getTactPlot(plotindex);

	//no caching? unclear what's better
	return dummyPlot;
}

struct PrPositionIsBetterHeap
{
	//sort by score of the last round of assignments - avoid bias for "deeper" positions
	bool operator()(const CvTacticalPosition* lhs, const CvTacticalPosition* rhs) const 
	{ 
		return lhs->getHeapScore() < rhs->getHeapScore(); //intended!
	}
};

struct PrPositionIsBetter
{
	//sort by cumulative score. only makes sense for "completed" positions
	bool operator()(const CvTacticalPosition* lhs, const CvTacticalPosition* rhs) const
	{ 
		return lhs->getScore() > rhs->getScore();
	}
};

//try to position our units around a target so that we are optimally prepared for counterattacks. target may be friendly or hostile.
vector<STacticalAssignment> TacticalAIHelpers::FindBestDefensiveAssignment(const vector<CvUnit*>& vUnits, CvPlot* pTarget, CvTactPosStorage& storage)
{
	/*
	abstract:
		agression level is zero here, so we do not plan any attacks. only movement.
		if target is friendly, we try to stay within N plots around it with melee units covering ranged units.
		if target is hostile, we try to come close but no closer than N plots with melee units covering ranged units.
	*/

	vector<STacticalAssignment> result;
	if (vUnits.empty() || vUnits.front()==NULL || pTarget==NULL)
		return result;

	//meta parameters depending on difficulty setting
	int iMaxCompletedPositions = GC.getGame().getHandicapType() < 2 ? 18 : 37;
	int iMaxBranches = GC.getGame().getHandicapType() < 2 ? 2 : 3;
	int iMaxChoicesPerUnit = GC.getGame().getHandicapType() < 2 ? 3 : 6;

	//set up the initial position
	PlayerTypes ePlayer = vUnits.front()->getOwner();
	storage.reset();
	gReachablePlotsLookup.clear();
	gRangeAttackPlotsLookup.clear();
	CvTacticalPosition* initialPosition = storage.getNext();
	if (!initialPosition)
		return result;
	initialPosition->initFromScratch(ePlayer, AL_NONE, pTarget);

	cvStopWatch timer("tactsim_defense",NULL,0,true);
	timer.StartPerfTest();

	//first pass: make sure there are no duplicates and other invalid inputs
	set<CvUnit*> ourUnits;
	bool bHaveCombatUnit = false;
	for (size_t i = 0; i < vUnits.size(); i++)
	{
		CvUnit* pUnit = vUnits[i];

		//ignore units on other islands, we can find better moves for them
		if (!pTarget->isSameOrAdjacentArea(pUnit->plot()) && pUnit->GetRange()<2)
			continue;

		//units outside of their native domain are a problem because they violate 1UPT
		//we treat embarked units non-combat units (see addAvailableUnit)
		if (pUnit && pUnit->canMove() && !pUnit->isDelayedDeath() && !pUnit->TurnProcessed())
		{
			ourUnits.insert(pUnit);
			if (pUnit->IsCombatUnit())
				bHaveCombatUnit = true;
		}
	}

	if (ourUnits.empty() || !bHaveCombatUnit)
		return result;

	//second pass, now that we know which units will be used, add them to the initial position
	for(set<CvUnit*>::iterator it=ourUnits.begin(); it!=ourUnits.end(); ++it)
	{
		CvUnit* pUnit = *it;
		if (initialPosition->addAvailableUnit(pUnit))
		{
			//make sure we know the immediate surroundings of every unit
			for (int j = 0; j < RING2_PLOTS; j++)
			{
				CvPlot* pPlot = iterateRingPlots(pUnit->plot(), j);
				if (pPlot && pPlot->isVisible(GET_PLAYER(ePlayer).getTeam()))
					initialPosition->addTacticalPlot(pPlot, ourUnits);
			}
		}
	}

	//create the tactical plots around the target (up to distance 5)
	//not equivalent to the union of all reachable plots: we need to consider unreachable enemies as well!
	//some units may have their initial plots outside of this range but that's ok
	for (int i = 0; i < RING_PLOTS[TACTICAL_COMBAT_MAX_TARGET_DISTANCE + 1]; i++)
	{
		CvPlot* pPlot = iterateRingPlots(pTarget, i);
		if (pPlot && pPlot->isVisible(GET_PLAYER(ePlayer).getTeam()))
			initialPosition->addTacticalPlot(pPlot,ourUnits);
	}

	//find out which plot is frontline, second line etc
	initialPosition->refreshVolatilePlotProperties();

	//now associate our units with their initial plots (after we know the plot types)
	initialPosition->addInitialAssignments();

	//this determines our max active units
	initialPosition->countEnemies();

	//if we have a lot of units, ignore the unimportant ones
	int iMaxActiveUnits = initialPosition->getNumEnemies() < 2 ? 6 : 9;
	initialPosition->dropSuperfluousUnits(iMaxActiveUnits);

	vector<CvTacticalPosition*> openPositionsHeap;
	vector<CvTacticalPosition*> completedPositions;

	//don't need to call make_heap for a single element
	openPositionsHeap.push_back(initialPosition);

	int iDiscardedPositions = 0;
	int iUsedPositions = 0;
	int iTopScore = 0;

	while (!openPositionsHeap.empty())
	{
		pop_heap( openPositionsHeap.begin(), openPositionsHeap.end(), PrPositionIsBetterHeap() );
		CvTacticalPosition* current = openPositionsHeap.back(); openPositionsHeap.pop_back();
		iUsedPositions++;

		//if we have a lot of open positions, don't waste time on candidates which looked good initially but are not anymore
		if (openPositionsHeap.size() > 1000 && current->getScore() < iTopScore / 2)
		{
			iDiscardedPositions++;
			continue;
		}

		//here the magic happens
		if (current->makeNextAssignments(iMaxBranches, iMaxChoicesPerUnit, storage))
		{
			for (vector<CvTacticalPosition*>::const_iterator it = current->getChildren().begin(); it != current->getChildren().end(); ++it)
			{
				CvTacticalPosition* newPos = *it;
				if (newPos->isComplete())
				{
					if (newPos->addFinishMovesIfAcceptable())
					{
						completedPositions.push_back(newPos);
						iTopScore = max(iTopScore, newPos->getScore());
					}
					else
						iDiscardedPositions++;
				}
				else
				{
					openPositionsHeap.push_back(newPos);
					iTopScore = max(iTopScore, newPos->getScore());
					push_heap(openPositionsHeap.begin(), openPositionsHeap.end(), PrPositionIsBetterHeap());
				}
			}
		}
		else
			iDiscardedPositions++;

		//at some point we have seen enough good positions to pick one
		if (completedPositions.size() > (size_t)iMaxCompletedPositions)
			break;

		//don't waste too much time/memory
		if (iUsedPositions + openPositionsHeap.size() == storage.getSizeLimit()-1)
			break;

		//be a good citizen and let the UI run in between ... stupid design
		if ( (iUsedPositions%1000)==999 && gDLL->HasGameCoreLock() )
		{
			gDLL->ReleaseGameCoreLock();
			Sleep(1);
			gDLL->GetGameCoreLock();
		}
	}

	if (!completedPositions.empty())
	{
		//need the predicate, else we sort the pointers by address!
		sort(completedPositions.begin(), completedPositions.end(), PrPositionIsBetter());
		result = completedPositions.front()->getAssignments();

		if (gTacticalCombatDebugOutput>10)
			completedPositions.front()->dumpPlotStatus("c:\\temp\\plotstatus_final.csv");
	}
	else if (!openPositionsHeap.empty())
	{
		//sort again with different predicate
		sort(openPositionsHeap.begin(), openPositionsHeap.end(), PrPositionIsBetter());
		result = openPositionsHeap.front()->getAssignments();

		if (gTacticalCombatDebugOutput>10)
			openPositionsHeap.front()->dumpPlotStatus("c:\\temp\\plotstatus_final.csv");
	}

	timer.EndPerfTest();

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("tactsim_defense %s, target %d:%d with %d units (agglvl %d). last dangerplots update at %d. tested %d, discarded %d, completed %d, open %d (%.2f ms).", 
			result.empty() ? "failed" : "success",
			pTarget->getX(), pTarget->getY(), ourUnits.size(), AL_NONE, GET_PLAYER(ePlayer).GetDangerPlotAge(), 
			iUsedPositions, iDiscardedPositions, completedPositions.size(), openPositionsHeap.size(), timer.GetDeltaInSeconds()*1000.f );

		GET_PLAYER(ePlayer).GetTacticalAI()->LogTacticalMessage(strMsg);

		if (timer.GetDeltaInSeconds() > 10 || vUnits.size()>20)
			OutputDebugString("warning, long running simulation\n"); //put a breakpoint here ...
	}

	return result;
}

//try to find a combination of unit actions (move, attack etc) which does maximal damage to the enemy while exposing us to limited risk
vector<STacticalAssignment> TacticalAIHelpers::FindBestOffensiveAssignment(
	const vector<CvUnit*>& vUnits, CvPlot* pTarget, eAggressionLevel eAggLvl, CvTactPosStorage& storage)
{
	/*
	abstract:

	create tactical plots
	add all units with reachable plots
 
	create open position
	while (pop open positions)
	 if (make next assignments)
	  add children to open positions
	 else
	  score position
	  if possible position (no unit left over)
	   add to closed positions

	return best closed position
	*/

	vector<STacticalAssignment> result;
	if (vUnits.empty() || vUnits.front()==NULL || pTarget==NULL)
		return result;

	//meta parameters depending on difficulty setting
	int iMaxCompletedPositions = GC.getGame().getHandicapType() < 2 ? 18 : 37;
	int iMaxBranches = GC.getGame().getHandicapType() < 2 ? 2 : 4;
	int iMaxChoicesPerUnit = GC.getGame().getHandicapType() < 2 ? 3 : 6;

	PlayerTypes ePlayer = vUnits.front()->getOwner();
	TeamTypes ourTeam = GET_PLAYER(ePlayer).getTeam();

	cvStopWatch timer("tactsim_offense",NULL,0,true);
	timer.StartPerfTest();

	//set up the initial position
	storage.reset();
	gReachablePlotsLookup.clear();
	gRangeAttackPlotsLookup.clear();
	CvTacticalPosition* initialPosition = storage.getNext();
	if (!initialPosition)
		return result;
	initialPosition->initFromScratch(ePlayer, eAggLvl, pTarget);

	//first pass: make sure there are no duplicates and other invalid inputs
	set<CvUnit*> ourUnits;
	for (size_t i = 0; i < vUnits.size(); i++)
	{
		CvUnit* pUnit = vUnits[i];

		//ignore units on other islands, we can find better moves for them
		if (!pTarget->isSameOrAdjacentArea(pUnit->plot()) && pUnit->GetRange()<2)
			continue;

		//units outside of their native domain are a problem because they violate 1UPT. 
		//we accept them only if they are alone in the plot and only allow movement into the native domain.
		if (pUnit && pUnit->canMove() && !pUnit->isDelayedDeath() && !pUnit->TurnProcessed())
			if (pUnit->isNativeDomain(pUnit->plot()) || pUnit->plot()->getNumUnits()==1)
				ourUnits.insert(vUnits[i]);
	}

	if (ourUnits.empty())
		return result;

	//second pass, now that we know which units will be used, add them to the initial position
	for(set<CvUnit*>::iterator it=ourUnits.begin(); it!=ourUnits.end(); ++it)
	{
		CvUnit* pUnit = *it;
		if (initialPosition->addAvailableUnit(pUnit))
		{
			//make sure we know the immediate surroundings of every unit
			for (int j = 0; j < RING2_PLOTS; j++)
			{
				CvPlot* pPlot = iterateRingPlots(pUnit->plot(), j);
				if (pPlot && pPlot->isVisible(ourTeam))
					initialPosition->addTacticalPlot(pPlot, ourUnits);
			}
		}
	}

	//create the tactical plots around the target (up to distance 5)
	//not equivalent to the union of all reachable plots: we need to consider unreachable enemies as well!
	//some units may have their initial plots outside of this range but that's ok
	for (int i = 0; i < RING_PLOTS[TACTICAL_COMBAT_MAX_TARGET_DISTANCE + 1]; i++)
	{
		CvPlot* pPlot = iterateRingPlots(pTarget, i);
		if (pPlot && pPlot->isVisible(ourTeam))
			initialPosition->addTacticalPlot(pPlot,ourUnits);
	}

	//find out which plot is frontline, second line etc
	initialPosition->refreshVolatilePlotProperties();

	//now associate our units with their initial plots (after we know the plot types)
	initialPosition->addInitialAssignments();

	//this determines how agressive we can be
	initialPosition->countEnemies();

	//if we have a lot of units, ignore the unimportant ones
	int iMaxActiveUnits = initialPosition->getNumEnemies() < 2 ? 6 : 9;
	initialPosition->dropSuperfluousUnits(iMaxActiveUnits);

	vector<CvTacticalPosition*> openPositionsHeap;
	vector<CvTacticalPosition*> completedPositions;
	vector<CvTacticalPosition*> blockedPositions;
	int iDiscardedPositions = 0;
	int iUsedPositions = 0;
	int iTopScore = 0;

	//don't need to call make_heap for a single element
	openPositionsHeap.push_back(initialPosition);

	while (!openPositionsHeap.empty())
	{
		pop_heap( openPositionsHeap.begin(), openPositionsHeap.end(), PrPositionIsBetterHeap() );
		CvTacticalPosition* current = openPositionsHeap.back(); openPositionsHeap.pop_back();
		iUsedPositions++;

		//if we have a lot of open positions, don't waste time on candidates which looked good initially but are not anymore
		if (openPositionsHeap.size() > 1000 && current->getScore() < iTopScore / 2)
		{
			iDiscardedPositions++;
			continue;
		}

		//here the magic happens
		if (current->makeNextAssignments(iMaxBranches, iMaxChoicesPerUnit, storage))
		{
			for (vector<CvTacticalPosition*>::const_iterator it = current->getChildren().begin(); it != current->getChildren().end(); ++it)
			{
				CvTacticalPosition* newPos = *it;
				if (newPos->isComplete())
				{
					if (newPos->hasOffensiveAssignments() && newPos->addFinishMovesIfAcceptable())
					{
						completedPositions.push_back(newPos);
						iTopScore = max(iTopScore, newPos->getScore());
					}
					else
						iDiscardedPositions++;
				}
				else
				{
					openPositionsHeap.push_back(newPos);
					iTopScore = max(iTopScore, newPos->getScore());
					push_heap(openPositionsHeap.begin(), openPositionsHeap.end(), PrPositionIsBetterHeap());
				}
			}
		}
		else
		{
			//apparently we're blocked from making further assignments, but maybe this position is still useful
			if (current->hasOffensiveAssignments() && current->addFinishMovesIfAcceptable())
			{
				blockedPositions.push_back(current);
				iTopScore = max(iTopScore, current->getScore());
			}
			else
				iDiscardedPositions++;
		}

		//at some point we have seen enough good positions to pick one
		if (completedPositions.size() >= (size_t)iMaxCompletedPositions)
			break;

		//don't waste too much time/memory
		if (iUsedPositions + openPositionsHeap.size() == storage.getSizeLimit())
			break;

		//be a good citizen and let the UI run in between ... stupid design
		if ( (iUsedPositions%1000)==999 && gDLL->HasGameCoreLock() )
		{
			gDLL->ReleaseGameCoreLock();
			Sleep(1);
			gDLL->GetGameCoreLock();
		}
	}

	//normally the score for a completed position should be higher than for a finished one
	//but if there are too many unassigned units it can tip
	if (!completedPositions.empty())
	{
		//need the predicate, else we sort the pointers by address!
		sort(completedPositions.begin(), completedPositions.end(), PrPositionIsBetter());
		result = completedPositions.front()->getAssignments();

		if (gTacticalCombatDebugOutput>10)
			completedPositions.front()->dumpPlotStatus("c:\\temp\\plotstatus_final.csv");
	}
	else
	{
		//last chance - take the best open / blocked position
		for (size_t i = 0; i < openPositionsHeap.size(); i++)
		{
			if (openPositionsHeap[i]->getScore() > iTopScore/2 && openPositionsHeap[i]->addFinishMovesIfAcceptable())
				blockedPositions.push_back(openPositionsHeap[i]);
		}

		if (!blockedPositions.empty())
		{
			sort(blockedPositions.begin(), blockedPositions.end(), PrPositionIsBetter());
			result = blockedPositions.front()->getAssignments();

			if (gTacticalCombatDebugOutput > 10)
				blockedPositions.front()->dumpPlotStatus("c:\\temp\\plotstatus_final.csv");
		}
	}

	//debugging
	timer.EndPerfTest();
	//if (timer.GetDeltaInSeconds()>0.1)
	//	OutputDebugString(CvString::format("--> tested %d possible positions for %d units in %.2fms\n", initialPosition->countChildren(), ourUnits.size(), timer.GetDeltaInSeconds()*1000).c_str());
	if (gTacticalCombatDebugOutput > 10) //if needed we can set the instruction pointer here
	{
		initialPosition->exportToDotFile("c:\\temp\\graph.dot");
		initialPosition->dumpPlotStatus("c:\\temp\\plotstatus_initial.csv");
	}

	//stringstream buffer;
	//for(size_t i=0; i<result.size(); i++)
	//	buffer << result[i] << "\n";
	//OutputDebugString( buffer.str().c_str() );


	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("tactsim_offense %s, target %d:%d with %d units (agglvl %d). last dangerplots update at %d. tested %d, discarded %d, completed %d, open %d (%.2f ms).", 
			result.empty() ? "failed" : "success",
			pTarget->getX(), pTarget->getY(), ourUnits.size(), eAggLvl, GET_PLAYER(ePlayer).GetDangerPlotAge(), 
			iUsedPositions, iDiscardedPositions, completedPositions.size(), openPositionsHeap.size(), timer.GetDeltaInSeconds()*1000.f );
		GET_PLAYER(ePlayer).GetTacticalAI()->LogTacticalMessage(strMsg);

		if (timer.GetDeltaInSeconds() > 10 || vUnits.size()>20)
			OutputDebugString("warning, long running simulation\n"); //put a breakpoint here ...
	}

	return result;
}

bool TacticalAIHelpers::ExecuteUnitAssignments(PlayerTypes ePlayer, const std::vector<STacticalAssignment>& vAssignments)
{
	//take the assigned moves one by one and try to execute them faithfully. 
	//may fail if a melee kill unexpectedly happens or does not happen

	for (size_t i = 0; i < vAssignments.size(); i++)
	{
		CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(vAssignments[i].iUnitID);
		//be extra careful with the unit here, if we capture cities and liberate them strange instakills can happen
		//so we need to guess whether the pointer is still valid
		if (!pUnit || pUnit->isDelayedDeath() || pUnit->plot()==NULL )
			continue;

		CvPlot* pFromPlot = GC.getMap().plotByIndexUnchecked(vAssignments[i].iFromPlotIndex);
		CvPlot* pToPlot = GC.getMap().plotByIndexUnchecked(vAssignments[i].iToPlotIndex);

		bool bPrecondition = false;
		bool bPostcondition = false;

		switch (vAssignments[i].eAssignmentType)
		{
		case A_INITIAL:
			continue; //skip this!
			break;
		case A_MOVE:
		case A_MOVE_FORCED:
		case A_CAPTURE:
			pUnit->ClearPathCache(); //make sure there's no stale path which coincides with our target
			bPrecondition = pUnit->canMove() && (pUnit->plot() == pFromPlot) && !(pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //no enemy
#ifdef TACTDEBUG
			if (bPrecondition)
			{
				//see if we can indeed reach the target plot this turn ... 
				pUnit->ClearPathCache(); pUnit->GeneratePath(pToPlot, CvUnit::MOVEFLAG_IGNORE_DANGER | CvUnit::MOVEFLAG_NO_STOPNODES, INT_MAX, NULL, true);
				if (pUnit->GetPathEndFirstTurnPlot() != pToPlot)
					OutputDebugString("ouch, pathfinding problem\n");
			}
#endif
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pToPlot->getX(), pToPlot->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER|CvUnit::MOVEFLAG_NO_STOPNODES, false, false, MISSIONAI_OPMOVE); 
			bPostcondition = (pUnit->plot() == pToPlot); //plot changed
			break;
		case A_MOVE_SWAP:
			pUnit->ClearPathCache(); //make sure there's no stale path which coincides with our target
			bPrecondition = (pUnit->plot() == pFromPlot) && !(pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //no enemy
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_SWAP_UNITS(), pToPlot->getX(), pToPlot->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER|CvUnit::MOVEFLAG_NO_STOPNODES, false, false, MISSIONAI_OPMOVE); 
			bPostcondition = (pUnit->plot() == pToPlot); //plot changed
			break;
		case A_MOVE_SWAP_REVERSE:
			//nothing to do, this is just a dummy which always occurs after MOVE_SWAP for bookkeeping
			bPrecondition = true;
			bPostcondition = true;
			break;
		case A_RANGEATTACK:
			bPrecondition = (pUnit->plot() == pFromPlot) && (pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy present
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pToPlot->getX(), pToPlot->getY());
			bPostcondition = pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit); //enemy should survive
			break;
		case A_RANGEKILL:
			bPrecondition = (pUnit->plot() == pFromPlot) && pToPlot->isEnemyUnit(ePlayer,true,true); //defending unit present. does not apply to cities
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pToPlot->getX(), pToPlot->getY());
			bPostcondition = !pToPlot->isEnemyUnit(ePlayer,true,true); //defending unit is gone
			break;
		case A_MELEEATTACK:
			bPrecondition = (pUnit->plot() == pFromPlot) && (pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy present
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pToPlot->getX(), pToPlot->getY());
			bPostcondition = (pUnit->plot() == pFromPlot) && (pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy still present
			break;
		case A_MELEEKILL:
		case A_MELEEKILL_NO_ADVANCE:
			bPrecondition = (pUnit->plot() == pFromPlot) && (pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy present
			//because of randomness in previous combat results, it may happen that we cannot actually kill the enemy
			if (bPrecondition)
			{
				int iDamageDealt = 0, iDamageReceived = 0;
				if (pToPlot->isEnemyCity(*pUnit))
				{
					CvCity* pCity = pToPlot->getPlotCity();
					iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pCity, pUnit, pUnit->plot(), iDamageReceived);
					if (iDamageDealt + 1 < (pCity->GetMaxHitPoints()-pCity->getDamage()))
						bPrecondition = false;
				}
				else
				{
					CvUnit* pEnemy = pToPlot->getBestDefender(NO_PLAYER, ePlayer, pUnit);
					iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pEnemy, pUnit, pEnemy->plot(), pUnit->plot(), iDamageReceived);
					if (iDamageDealt + 1 < pEnemy->GetCurrHitPoints())
						bPrecondition = false;
				}
			}
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pToPlot->getX(), pToPlot->getY());

			bPostcondition = !(pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy gone
			if (vAssignments[i].eAssignmentType == A_MELEEKILL)
				bPostcondition &= (pUnit->plot() == pToPlot); //advanced into enemy plot
			else
				bPostcondition &= (pUnit->plot() == pFromPlot); //still in the same plot
			break;
		case A_PILLAGE:
			pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
			bPrecondition = true;
			bPostcondition = true;
			break;
		case A_FINISH:
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			//this is the difference to a blocked unit, we may prevent anyone else from moving it
			if (!pUnit->shouldHeal(false))
				GET_PLAYER(ePlayer).GetTacticalAI()->UnitProcessed(pUnit->GetID());
			bPrecondition = true;
			bPostcondition = true;
			break;
		case A_BLOCKED:
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			//do not mark the unit as processed, it can be reused for other tasks!
			bPrecondition = true;
			bPostcondition = true;
			break;
		case A_RESTART:
			return false; //the previous move revealed a new enemy (which cause a danger update). restart the combat simulation with the remaining units.
			break;
		}

#ifdef TACTDEBUG
		//this is unexpected
		if (vAssignments[i].iRemainingMoves > 0 && !pUnit->canMove())
			OutputDebugString("ouch, inconsistent movement points\n");

		if (!bPrecondition || !bPostcondition)
		{
			stringstream out;
			out << "could not execute " << assignmentTypeNames[ vAssignments[i].eAssignmentType ] << (!bPrecondition?"":" (unexpected result)") << "\n";
			OutputDebugString(out.str().c_str());
			return false;
		}
#else
		if (!bPrecondition || !bPostcondition)
			return false;
#endif
	}

	return true;
}

const char* tacticalMoveNames[] =
{
	"T_NONE",

	"T_UNASSIGNED",
	"T_GUARD",
	"T_GARRISON",
	"T_OPERATION",

	"T_PILLAGE",
	"T_PLUNDER",
	"T_GOODY",

	"T_HEAL",
	"T_SAFETY",
	"T_REPOSITION",
	"T_ESCORT",

	"T_AIRSWEEP",
	"T_AIRPATROL",

	"T_HEDGEHOG",
	"T_COUNTERATTACK",
	"T_WITHDRAW",
	"T_CLOSE_ON_TARGET",
	"T_ATTRITION",
	"T_SURGICAL_STRIKE",
	"T_STEAMROLL",
	"T_FLANKATTACK",

	"T_AIRLIFT",
	"T_BLOCKADE",
	"T_CAPTURE",

	"B_CAMP",
	"B_ROAM",
	"B_HUNT",
};

const char* postureNames[] =
{
	"P_NONE",
    "P_WITHDRAW",
    "P_ATTRIT_FROM_RANGE",
    "P_EXPLOIT_FLANKS",
    "P_STEAMROLL",
    "P_SURGICAL_CITY_STRIKE",
    "P_HEDGEHOG",
    "P_COUNTERATTACK",
    "P_SHORE_BOMBARDMENT",
};

const char* assignmentTypeNames[] = 
{
	"INITIAL",
	"MOVE", 
	"MELEEATTACK", 
	"MELEEKILL", 
	"RANGEATTACK", 
	"RANGEKILL", 
	"FINISH",
	"BLOCKED",
	"PILLAGE",
	"CAPTURE",
	"FORCEDMOVE",
	"RESTART",
	"MELEEKILL_NOADVANCE",
	"SWAP",
	"SWAPREVERSE"
};
