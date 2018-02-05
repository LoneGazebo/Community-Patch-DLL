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
#include "CvBarbarians.h"
#endif

#include <sstream>
#include <cmath>
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE_MILITARY)
//for easier debugging
int g_currentUnitToTrack = 0;
#endif

CvTacticalUnit::CvTacticalUnit() :
	m_iID(0)
	, m_iAttackStrength(0)
	, m_iHealthPercent(0)
	, m_iMovesToTarget(0)
	, m_iExpectedTargetDamage(0)
	, m_iExpectedSelfDamage(0)
#ifdef MOD_AI_SMART_AIR_TACTICS 
	, m_pInterceptor(0)
#endif
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
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_TACTICAL_MOVE) ? m_paTacticalMoveEntries[index] : NULL;
#else
	return m_paTacticalMoveEntries[index];
#endif
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
		if(pCity != NULL && pCity->getOwner() == GetTargetPlayer())
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
	case AI_TACTICAL_TARGET_ANCIENT_RUINS:
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND:
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT:
	case AI_TACTICAL_TARGET_CITADEL:
	case AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE:
		bRtnValue = (eDomain == DOMAIN_LAND);
		break;

	case AI_TACTICAL_TARGET_BLOCKADE_POINT:
	case AI_TACTICAL_TARGET_BOMBARDMENT_ZONE:
	case AI_TACTICAL_TARGET_EMBARKED_CIVILIAN:
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

	//do this after setting the player pointer!
	Reset();

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
	m_CachedInfoTypes[eTACTICAL_BASTION_2_TURN] = GC.getInfoTypeForString("TACTICAL_BASTION_2_TURN");
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

#if defined(MOD_BALANCE_CORE_MILITARY)
	m_CurrentMoveUnits.setPlayer(pPlayer);
#endif
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
	m_HealingUnits.clear();
	kStream >> iCount;
	for (int i=0; i<iCount; i++)
	{
		int iTemp;
		kStream >> iTemp;
		m_HealingUnits.insert(iTemp);
	}
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
	kStream << m_HealingUnits.size();
	for (std::set<int>::iterator it=m_HealingUnits.begin(); it!=m_HealingUnits.end(); ++it)
		kStream << *it;

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
void CvTacticalAI::CommandeerUnits()
{
	int iLoop;
	m_CurrentTurnUnits.clear();

	// Loop through our units
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// debugging hook
		if (g_currentUnitToTrack == pLoopUnit->GetID())
		{
			pLoopUnit->DumpDangerInNeighborhood();
		}

		//LogTacticalMessage( CvString::format("looking to recruit %s %d at (%d,%d) with %d hp",
		//	pLoopUnit->getName().c_str(),pLoopUnit->GetID(),pLoopUnit->getX(),pLoopUnit->getY(),pLoopUnit->GetCurrHitPoints()).c_str() );

		// reset mission AI so we don't see stale information (debugging only)
		// careful with explorers though, for performance reasons their mission is persistent
		if (pLoopUnit->GetMissionAIType()!=MISSIONAI_EXPLORE)
			pLoopUnit->SetMissionAI(NO_MISSIONAI,NULL,NULL);

		//if we cannot heal in the capital, we can heal nowhere ...
		CvCity* pCapital = m_pPlayer->getCapitalCity();
		bool bCanHeal = pCapital ? pLoopUnit->healRate( pCapital->plot() ) > 0 : false;
		bool bHasTarget = TacticalAIHelpers::IsCaptureTargetAdjacent(pLoopUnit) || (TacticalAIHelpers::GetFirstEnemyUnitInRange(pLoopUnit,true)!=NULL);

		// is the unit healing?
		if (m_HealingUnits.find( pLoopUnit->GetID() ) != m_HealingUnits.end())
		{
			if ( pLoopUnit->getDamage()>30 && bCanHeal && !bHasTarget )
				//need to continue healing
				continue;
			else
				//done healing
				m_HealingUnits.erase( pLoopUnit->GetID() );
		}
		else if (bCanHeal && !bHasTarget)
		{
			//does it need healing? unless barbarian or japanese!
			if ((pLoopUnit->getDamage()>80 && !m_pPlayer->isBarbarian()) || 
				(pLoopUnit->isProjectedToDieNextTurn() && !m_pPlayer->GetPlayerTraits()->IsFightWellDamaged() && !pLoopUnit->IsStrongerDamaged()))
			{
				CvPlot* pHealPlot = TacticalAIHelpers::FindSafestPlotInReach(pLoopUnit, true, true);
				if (pHealPlot)
				{
					//need to start healing
					m_HealingUnits.insert( pLoopUnit->GetID() );
					continue;
				}
			}
		}

		// Never want immobile/dead units, explorers, ones that have already moved or automated human units
		if(pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath() || !pLoopUnit->canMove() || pLoopUnit->isHuman() )
			continue;

		if (pLoopUnit->getDomainType() == DOMAIN_AIR)
		{
			//we want all combat ready air units, except nukes (those go through operational AI)
			if (pLoopUnit->getDamage() > 50 || ShouldRebase(pLoopUnit) || pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_ICBM)
				continue;

			if (pLoopUnit->getTacticalMove() == NO_TACTICAL_MOVE)
				pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
		else if (pLoopUnit->isBarbarian()) // We want ALL the barbarians
		{
			if (pLoopUnit->getTacticalMove() == NO_TACTICAL_MOVE)
				pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
		// these should be handled by homeland AI
		else if ( pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN || 
					pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE ||
					pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA )
		{
			continue;
		}
		// Now down to land and sea units ... in these groups our unit must have a base combat strength ... or be a great general
		else
		{
			//reject all civilians we cannot use
			if (!pLoopUnit->IsCombatUnit() && !pLoopUnit->IsGreatGeneral() && !pLoopUnit->IsGreatAdmiral() && !pLoopUnit->IsCityAttackSupport())
				continue;

			 //if it's a general or admiral and not a field commander, we don't want it
			if( pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral() || pLoopUnit->IsCityAttackSupport())
			{
				GreatPeopleDirectiveTypes eDirective = pLoopUnit->GetGreatPeopleDirective();
				if (eDirective == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
				{
					continue;
				}
			}

			bool bNearVisibleEnemy = NearVisibleEnemy(pLoopUnit, GetRecruitRange());
			// Is this one in an operation we can't interrupt?
			int iArmyID = pLoopUnit->getArmyID();
			const CvArmyAI* pArmy = m_pPlayer->getArmyAI(iArmyID);
			if(iArmyID != -1 && pArmy && !pArmy->CanTacticalAIInterruptUnit(pLoopUnit->GetID()))
			{
				//those units will be moved as part of the army moves. don't touch them afterwards
				pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_MOVE_OPERATIONS]);
			}
			else
			{
				// Non-zero danger value or near enemy, paratrooper, or recently deployed out of an operation?
				int iDanger = pLoopUnit->GetDanger();
				if (iDanger > 0 || bNearVisibleEnemy || pLoopUnit->IsRecentlyDeployedFromOperation() || pLoopUnit->canParadrop(pLoopUnit->plot(), false))
				{
					if (iArmyID != -1)
					{
						//part of an army but we can still use it for tactical AI if it has moves left
						pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_MOVE_OPERATIONS]);
					}
					else
					{
						if (pLoopUnit->getTacticalMove() == NO_TACTICAL_MOVE)
							pLoopUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);
					}

					m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
				}
			}
		}
	}

#if defined(MOD_CORE_DEBUGGING)
	if (MOD_CORE_DEBUGGING)
	{
		for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
			CvString msg = CvString::format("current turn tactical unit %s %d at %d,%d\n", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY() );
			LogTacticalMessage( msg );
		}
	}
#endif
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

	//do this after updating the target list!
	CommandeerUnits();

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
	
	CvTacticalDominanceZone* pZone;
	CvCity* pClosestCity = NULL;

	// Loop through all the zones we have this turn
	for(int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
	{
		pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);

		// Check to make sure we want to use this zone
		if(UseThisDominanceZone(pZone))
		{
			pClosestCity = pZone->GetZoneCity();

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
	std::vector<CvTemporaryZone> zonesCopy;

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
	std::vector<CvTemporaryZone> zonesCopy;

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

// PRIVATE METHODS

/// Establish postures for each dominance zone (taking into account last posture)
void CvTacticalAI::UpdatePostures()
{
	CvTacticalDominanceZone* pZone;
	AITacticalPosture eLastPosture, eNewPosture;

	std::vector<CvTacticalPosture> newPostures;

	// Loop through all the zones we have this turn
	for(int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
	{
		pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);

		// Check to make sure we want to use this zone
		if(UseThisDominanceZone(pZone))
		{
			int iCityID = -1;
			if(pZone->GetZoneCity() != NULL)
			{
				iCityID = pZone->GetZoneCity()->GetID();
			}

			eLastPosture = FindPosture(pZone);
			eNewPosture = SelectPosture(pZone, eLastPosture);
			newPostures.push_back(CvTacticalPosture(pZone->GetOwner(), pZone->IsWater(), iCityID, eNewPosture));

			if(GC.getLogging() && GC.getAILogging() && eNewPosture != AI_TACTICAL_POSTURE_NONE)
			{
				CvString szPostureMsg;
				szPostureMsg.Format("Zone ID: %d, %s, %s, ", pZone ? pZone->GetZoneID() : -1, pZone->IsWater() ? "Water" : "Land", pZone->GetZoneCity() ? pZone->GetZoneCity()->getName().c_str() : "none");

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
	//eTacticalDominanceFlags eUnitCountDominance = pZone->IsWater() ? pZone->GetNavalUnitCountDominanceFlag(iDominancePercentage) : pZone->GetUnitCountDominanceFlag(iDominancePercentage);

	// Choice based on whose territory this is
	switch(pZone->GetTerritoryType())
	{
	case TACTICAL_TERRITORY_ENEMY:
	case TACTICAL_TERRITORY_TEMP_ZONE:
	{
		// Default for this zone
		eChosenPosture = (pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE) ? AI_TACTICAL_POSTURE_STEAMROLL : AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE;
		CvCity *pClosestCity = pZone->GetZoneCity();

		// Temporary zone: Attack if possible, if defenses are strong try to wear them down first
		if (pClosestCity && IsTemporaryZoneCity(pClosestCity))
		{
			if (pZone->GetEnemyMeleeStrength() > pZone->GetFriendlyMeleeStrength())
			{
				eChosenPosture = (eOverallDominance != TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_SIT_AND_BOMBARD : AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
			}
			else
			{
				eChosenPosture = (eOverallDominance != TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_STEAMROLL : AI_TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
			}
		}
		// Withdraw if enemy dominant overall or we are vulnerable to counterattacks
		else if (eOverallDominance == TACTICAL_DOMINANCE_ENEMY || (pZone->GetEnemyMeleeStrength()>pZone->GetFriendlyMeleeStrength() && pZone->GetFriendlyRangedStrength()>pZone->GetFriendlyMeleeStrength()))
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
				eChosenPosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_ATTRIT_FROM_RANGE : AI_TACTICAL_POSTURE_SIT_AND_BOMBARD;
			}
			else
			{
				//if we have ranged dominance, keep our risk lower
				eChosenPosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? AI_TACTICAL_POSTURE_STEAMROLL : AI_TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}
		}
		else if (eOverallDominance == TACTICAL_DOMINANCE_FRIENDLY)
		{
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

		if (eOverallDominance >= TACTICAL_DOMINANCE_ENEMY || (pZone->GetOverallEnemyStrength() > 0 && pZone->GetZoneCity() != NULL && pZone->GetZoneCity()->IsBastion()))
		{
			eChosenPosture = AI_TACTICAL_POSTURE_HEDGEHOG;
		}	

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
			move.m_name = pkTacticalMoveInfo->GetType();

			if(move.m_iPriority >= 0)
			{
				m_MovePriorityList.push_back(move);
			}
		}
	}

	// Now sort the moves in priority order
	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());

#if defined(MOD_CORE_DEBUGGING)
	if(MOD_CORE_DEBUGGING)
	{
		//Debugging: Check order of tactical moves ... this is independent of the player
		if (GC.getLogging() && GC.getAILogging() && m_pPlayer->GetID()==1 && GC.getGame().getGameTurn()==1)
		{
			for (size_t i=0; i<m_MovePriorityList.size(); i++)
			{
				OutputDebugString( CvString::format("Turn %03d - Player %02d - Move %02d: %s - Prio %d%s - %s\n", 
					GC.getGame().getGameTurn(), m_pPlayer->GetID(), 
					m_MovePriorityList[i].m_eMoveType, GC.getTacticalMoveInfo(m_MovePriorityList[i].m_eMoveType)->GetType(), 
					m_MovePriorityList[i].m_iPriority,
					GC.getTacticalMoveInfo(m_MovePriorityList[i].m_eMoveType)->CanRecruitForOperations() ? "" : " (not interruptible)",
					GC.getTacticalMoveInfo(m_MovePriorityList[i].m_eMoveType)->IsDominanceZoneMove() ? "dominance zone move" : "" ).c_str() );
			}
		}
	}
#endif

}


/// Choose which tactics the barbarians should emphasize this turn
void CvTacticalAI::EstablishBarbarianPriorities()
{
	int iPriority = 0;
	m_MovePriorityList.clear();

	// Loop through each possible tactical move (other than "none" or "unassigned")
	for(int iI = 0; iI < NUM_AI_BARBARIAN_MOVES; iI++)
	{
		if((AIBarbarianTacticalMove)iI == AI_TACTICAL_BARBARIAN_NONE)
			continue;

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
		case AI_TACTICAL_BARBARIAN_BLOCKADE_RESOURCE:
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
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
			iPriority += GC.getGame().getSmallFakeRandNum(GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS(),iI);
#else
			iPriority += GC.getGame().getJonRandNum(GC.getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS(), "Tactical AI Move Priority");
#endif

			// Store off this move and priority
			CvTacticalMove move;
			move.m_eMoveType = (TacticalAIMoveTypes)iI;
			move.m_iPriority = iPriority;
			move.m_name = barbarianMoveNames[iI];

			m_MovePriorityList.push_back(move);
		}
	}

	// Now sort the moves in priority order
	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());

#if defined(MOD_BALANCE_CORE_MILITARY)
	if(MOD_BALANCE_CORE_MILITARY_LOGGING)
	{
		//Debugging: Check order of tactical moves ...
		if (GC.getAILogging() && GC.getGame().getGameTurn()==1)
			for (size_t i=0; i<m_MovePriorityList.size(); i++)
				OutputDebugString( CvString::format("Turn %03d - Barbarian Move %s - Prio %d\n", 
					GC.getGame().getGameTurn(), barbarianMoveNames[m_MovePriorityList[i].m_eMoveType], m_MovePriorityList[i].m_iPriority).c_str() );
	}
#endif
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
	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		bValidPlot = false;

#if defined(MOD_BALANCE_CORE_MILITARY)
		if (pLoopPlot->isRevealed(m_pPlayer->getTeam()))
#else
		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
#endif
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
		}

		if (bValidPlot)
		{
			newTarget.Clear();
			newTarget.SetTargetX(pLoopPlot->getX());
			newTarget.SetTargetY(pLoopPlot->getY());
			newTarget.SetDominanceZone(GetTacticalAnalysisMap()->GetCell(iI)->GetDominanceZone());

			bEnemyDominatedPlot = GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pLoopPlot);

			// Have a ...
			// ... friendly city?
			CvCity* pCity = pLoopPlot->getPlotCity();
			if (pCity != NULL)
			{
				if (m_pPlayer->GetID() == pCity->getOwner() && (pCity->IsBastion() || pCity->isUnderSiege() || pCity->isInDangerOfFalling()))
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
					newTarget.SetAuxData((void*)pCity);
					newTarget.SetAuxIntData(pCity->getThreatValue());
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy city
				else if (atWar(m_pPlayer->getTeam(), pCity->getTeam()))
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITY);
					newTarget.SetTargetPlayer(pCity->getOwner());
					newTarget.SetAuxData((void*)pCity);
#if defined(MOD_BALANCE_CORE)
					//barbarians don't care about cities so much
					newTarget.SetAuxIntData( m_pPlayer->isBarbarian() ? 20 : 100);
#endif
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

					newTarget.SetTargetPlayer(pUnit->getOwner());
					newTarget.SetAuxData((void*)pUnit);
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
							newTarget.SetTargetPlayer(pUnit->getOwner());
							newTarget.SetAuxData((void*)pUnit);
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

				// ... undefended camp? But just because there's no visible defender doesn't mean it's undefended
				if (pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) == GC.getBARBARIAN_CAMP_IMPROVEMENT() && !m_pPlayer->isMinorCiv())
				{
					int iBaseScore = pLoopPlot->isVisible(m_pPlayer->getTeam()) ? 50 : 30;

					newTarget.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
					newTarget.SetTargetPlayer(BARBARIAN_PLAYER);
					newTarget.SetAuxData((void*)pLoopPlot);
					newTarget.SetAuxIntData(iBaseScore);
					m_AllTargets.push_back(newTarget);
				}

				// ... goody hut?
				if (m_pPlayer->isMajorCiv() && pLoopPlot->isGoody())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_ANCIENT_RUINS);
					newTarget.SetAuxData((void*)pLoopPlot);
#if defined(MOD_BALANCE_CORE)
					newTarget.SetAuxIntData(150);
#endif
					m_AllTargets.push_back(newTarget);
				}

				// Or citadels!
#if defined(MOD_BALANCE_CORE_MILITARY)
				if (atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) != NO_IMPROVEMENT &&
					GC.getImprovementInfo(pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()))->GetNearbyEnemyDamage() > 0)
#else
				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					GC.getImprovementInfo(pLoopPlot->getImprovementType())->GetNearbyEnemyDamage() > 0)
#endif
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_CITADEL);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void*)pLoopPlot);
#if defined(MOD_BALANCE_CORE)
					newTarget.SetAuxIntData(80);
#endif
					m_AllTargets.push_back(newTarget);
				}

				// ... enemy improvement?
#if defined(MOD_BALANCE_CORE_MILITARY)
				if (atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) != NO_IMPROVEMENT &&
					!pLoopPlot->IsImprovementPillaged())
#else
				else if(atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					!pLoopPlot->IsImprovementPillaged())
#endif
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
						if (m_pPlayer->isBarbarian() && pLoopPlot->isWater())
						{
							continue;
						}
						else
						{
							newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE);
							newTarget.SetTargetPlayer(pLoopPlot->getOwner());
							newTarget.SetAuxData((void*)pLoopPlot);
#if defined(MOD_BALANCE_CORE)
							newTarget.SetAuxIntData(40);
#endif
							m_AllTargets.push_back(newTarget);
						}
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
						newTarget.SetTargetPlayer(pLoopPlot->getOwner());
						newTarget.SetAuxData((void*)pLoopPlot);
#if defined(MOD_BALANCE_CORE)
						newTarget.SetAuxIntData(5);
#endif
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... enemy trade route? (city connection - not caravan)
				// checking for city connection is not enough, some people (iroquois) don't need roads, so there isn't anything to pillage 
				if (atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRouteType() != NO_ROUTE && !pLoopPlot->IsRoutePillaged() && pLoopPlot->IsCityConnection())
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
					newTarget.SetTargetPlayer(pLoopPlot->getOwner());
					newTarget.SetAuxData((void*)pLoopPlot);
					newTarget.SetAuxIntData(10 - bEnemyDominatedPlot ? 2 : 0 );
					m_AllTargets.push_back(newTarget);
				}

				// ... trade unit
#if defined(MOD_BALANCE_CORE_MILITARY)
				if (pLoopPlot->isVisible(m_pPlayer->getTeam()) && pPlayerTrade->ContainsEnemyTradeUnit(pLoopPlot))
#else
				else if (pPlayerTrade->ContainsEnemyTradeUnit(pLoopPlot))
#endif
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
#if defined(MOD_BALANCE_CORE)
					newTarget.SetAuxIntData(35);
#endif
					m_AllTargets.push_back(newTarget);
				}

				// ... defensive bastion?
				if (m_pPlayer->GetID() == pLoopPlot->getOwner() &&
					(pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false) >= 20 || pLoopPlot->IsChokePoint()) && 
					pLoopPlot->getBestDefender(m_pPlayer->GetID())==NULL)
				{
					CvCity* pDefenseCity = pLoopPlot->getWorkingCity();
					if ((pDefenseCity && (pDefenseCity->IsBastion() || pDefenseCity->isUnderSiege())) || pLoopPlot->IsChokePoint())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
						newTarget.SetAuxData((void*)pLoopPlot);
#if defined(MOD_BALANCE_CORE_MILITARY)
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
#else
						newTarget.SetAuxIntData(pDefenseCity->getThreatValue() + m_pPlayer->GetPlotDanger(*pLoopPlot));
#endif
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... friendly improvement?
				if (m_pPlayer->GetID() == pLoopPlot->getOwner() &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					!pLoopPlot->IsImprovementPillaged() && !pLoopPlot->isGoody() &&
					pLoopPlot->getBestDefender(m_pPlayer->GetID()))
				{
					if (pLoopPlot->getWorkingCity() != NULL && pLoopPlot->getWorkingCity()->IsBastion())
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
						newTarget.SetAuxData((void*)pLoopPlot);
#if defined(MOD_BALANCE_CORE)
						newTarget.SetAuxIntData(1);
#endif
						m_AllTargets.push_back(newTarget);
					}
				}

				// ... trade plot (for getting units to park on trade routes to try to get them to plunder enemy trade routes)
				if (pLoopPlot->isVisible(m_pPlayer->getTeam()) &&
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
#if defined(MOD_BALANCE_CORE)
					newTarget.SetAuxIntData(15);
#endif
					m_AllTargets.push_back(newTarget);
				}
#if defined(MOD_BALANCE_CORE)
				//Enemy water plots?
				if (pLoopPlot->isWater() && atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()))
				{
					CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
					if (pWorkingCity != NULL && pWorkingCity->isCoastal())
					{
						int iDistance = GET_PLAYER(pWorkingCity->getOwner()).GetCityDistanceInPlots(pLoopPlot);
						if (iDistance > 3 || pLoopPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(),DOMAIN_SEA)>2)
							continue;

						int iWeight = (iDistance>1) ? 10 : 0;
						if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
							iWeight += 10;

						if (pWorkingCity->isInDangerOfFalling() || pWorkingCity->isUnderSiege() || (pWorkingCity->isCoastal() && pWorkingCity->IsBlockaded(true)))
							iWeight *= 2;

						if (pWorkingCity->IsBastion())
							iWeight *= 2;

						if (pWorkingCity->getDamage() > 0)
							iWeight *= 2;

						if (iWeight > 0)
						{
							newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_POINT);

							newTarget.SetAuxData((void*)pLoopPlot);
							newTarget.SetAuxIntData(iWeight/10);
							m_NavalTargets.push_back(newTarget);
						}
					}
				}
#endif
			}
		}
	}

	// POST-PROCESSING ON TARGETS
#if defined(MOD_BALANCE_CORE)
	//DumpTacticalTargets("pre");
#endif

	// Mark enemy units threatening our cities (or camps) as priority targets
	IdentifyPriorityTargets();
	IdentifyPriorityBarbarianTargets();

	// Also add some priority targets that we'd like to hit just because of their unit type (e.g. catapults)
	IdentifyPriorityTargetsByType();

	// make sure high prio units have the higher scores
	UpdateTargetScores();

#if defined(MOD_BALANCE_CORE)
	//DumpTacticalTargets("post");

	//Let's clean up our naval target list.
	PrioritizeNavalTargets();
#endif

	// Sort remaining targets by aux data (if used for that target type)
	std::stable_sort(m_AllTargets.begin(), m_AllTargets.end());

#if defined(MOD_CORE_DEBUGGING)
	if(MOD_CORE_DEBUGGING)
	{
		// mark the targets in the tactical map
		for (TacticalList::const_iterator i = m_AllTargets.begin(); i != m_AllTargets.end(); ++i)
			GetTacticalAnalysisMap()->GetCell( GC.getMap().plotNum( i->GetTargetX(), i->GetTargetY() ) )->SetTargetType( i->GetTargetType() ); 

		GetTacticalAnalysisMap()->Dump();
	}
#endif

}
#if defined(MOD_BALANCE_CORE)
/// Don't allow adjacent tiles to both be sentry points
void CvTacticalAI::PrioritizeNavalTargets()
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
			if (pPlot->getResourceType(m_pPlayer->getTeam()) != NO_RESOURCE)
			{
				newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_POINT);
				newTarget.SetTargetX(pPlot->getX());
				newTarget.SetTargetY(pPlot->getY());
				newTarget.SetDominanceZone(GetTacticalAnalysisMap()->GetCell(pPlot->GetPlotIndex())->GetDominanceZone());
				newTarget.SetAuxData((void*)pPlot);
				newTarget.SetAuxIntData(m_NavalTargets[iI].GetAuxIntData());
				m_AllTargets.push_back(newTarget);
			}
			//Only keep top 10 targets.
			else if (iI <= 10)
			{
				newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_POINT);
				newTarget.SetTargetX(pPlot->getX());
				newTarget.SetTargetY(pPlot->getY());
				newTarget.SetDominanceZone(GetTacticalAnalysisMap()->GetCell(pPlot->GetPlotIndex())->GetDominanceZone());
				newTarget.SetAuxData((void*)pPlot);
				newTarget.SetAuxIntData(m_NavalTargets[iI].GetAuxIntData());
				m_AllTargets.push_back(newTarget);
			}
		}
	}
	m_NavalTargets.clear();
}
#endif
void CvTacticalAI::ProcessDominanceZones()
{
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
		for(std::vector<CvTacticalMove>::iterator it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
		{
			CvTacticalMove move = *it;

			if(move.m_iPriority >= 0)
			{
				CvTacticalMoveXMLEntry* pkTacticalMoveInfo = GC.getTacticalMoveInfo(move.m_eMoveType);
				if(pkTacticalMoveInfo && pkTacticalMoveInfo->IsDominanceZoneMove())
				{
					for(int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
					{
						m_iCurrentZoneIndex = iI;
						CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);

						//no friendly presence -> nothing to do
						if (pZone->GetOverallFriendlyStrength() == 0)
							continue;
						//no enemy presence -> nothing to do
						if (pZone->GetOverallEnemyStrength() == 0)
							continue;

						AITacticalPosture ePosture = FindPosture(pZone);
						CvString strMoveName = (CvString)pkTacticalMoveInfo->GetType();
						CvTacticalMove moveToPassOn = move;

						// Is this move of the right type for this zone?
						bool bMatch = false;
						if(ePosture != AI_TACTICAL_POSTURE_WITHDRAW && strMoveName == "TACTICAL_CLOSE_ON_TARGET")
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
						else if(pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && strMoveName == "TACTICAL_EMERGENCY_PURCHASES")
						{
							bMatch = true;
						}
						else if(pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && strMoveName == "TACTICAL_DEFENSIVE_AIRLIFT")
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


							if (GC.getLogging() && GC.getAILogging() && pZone)
							{
								CvString strLogString;
								CvCity* pZoneCity = pZone->GetZoneCity();
								CvTacticalMoveXMLEntry* pkMoveInfo = GC.getTacticalMoveInfo(moveToPassOn.m_eMoveType);
								strLogString.Format("Zone %d, %s, using move %s, city of %s, %s",  
									pZone ? pZone->GetZoneID() : -1, pZone->IsWater() ? "water" : "land",
									pkMoveInfo ? pkMoveInfo->GetType() : "unknown", pZoneCity ? pZoneCity->getName().c_str() : "none", postureNames[ePosture]);
								LogTacticalMessage(strLogString);
								
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

	//failsafe
	ReviewUnassignedUnits();
}

/// Choose which tactics to run and assign units to it
void CvTacticalAI::AssignTacticalMove(CvTacticalMove move)
{
#pragma warning ( push )
#pragma warning ( disable : 6011 ) // Dereferencing NULL pointer
	AI_PERF_FORMAT("AI-perf-tact.csv", ("Move Type: %s (%d), Turn %03d, %s", GC.getTacticalMoveInfo(move.m_eMoveType)->GetType(), (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
#pragma warning ( pop )

#if defined(MOD_BALANCE_CORE_MILITARY)
	m_CurrentMoveUnits.setCurrentTacticalMove(move);
#endif

	//the order in which the moves are listed here is not at all the order in which they are executed!

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
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true, true);
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
		//can't capture those, so let's shoot at them
		PlotDamageCivilianMoves(AI_TACTICAL_TARGET_EMBARKED_CIVILIAN);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_REPOSITION])
	{
		PlotBlockadeMoves();
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
	else if (move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN])
	{
		PlotBastionMoves(1);
	}
	else if (move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_2_TURN])
	{
		PlotBastionMoves(2);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE])
	{
		PlotGuardImprovementMoves(0);
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN])
	{
		PlotGarrisonMoves(1);
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
		//important
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
		PlotCloseOnTarget();
	}
	else if(move.m_eMoveType == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_MOVE_OPERATIONS])
	{
		//this is in fact a very important move
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
	// Proceed in priority order
	for(std::vector<CvTacticalMove>::iterator it = m_MovePriorityList.begin(); it != m_MovePriorityList.end(); it++)
	{
		CvTacticalMove move = *it;

#if defined(MOD_BALANCE_CORE_MILITARY)
		//if all units have moved we can stop - this is ok here because barbarians don't have operations
		if (m_CurrentTurnUnits.empty())
			break;

		//debugging
		m_CurrentMoveUnits.setCurrentTacticalMove(move);
#endif

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
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true, true);
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_MEDIUM_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true, true);
			break;
		case AI_TACTICAL_BARBARIAN_DESTROY_LOW_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true, true);
			break;
		case AI_TACTICAL_BARBARIAN_MOVE_TO_SAFETY:
			PlotMovesToSafety(true /*bCombatUnits*/);
			PlotMovesToSafety(false /*bCombatUnits*/);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_HIGH_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false, true);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_MEDIUM_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false, true);
			//barbarians attack civilians, they are barbarians!
			PlotDamageCivilianMoves(AI_TACTICAL_TARGET_EMBARKED_CIVILIAN);
			break;
		case AI_TACTICAL_BARBARIAN_ATTRIT_LOW_PRIORITY_UNIT:
			PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, true);
			//barbarians attack civilian, they are barbarians!
			PlotDamageCivilianMoves(AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN);
			PlotDamageCivilianMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN);
			PlotDamageCivilianMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN);
			PlotDamageCivilianMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN);
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE:
			PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true/*bFirstPass*/);
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_CITADEL:
			PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, true/*bFirstPass*/);
			break;
		case AI_TACTICAL_BARBARIAN_PILLAGE_NEXT_TURN:
			PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, false/*bFirstPass*/);
			break;
		case AI_TACTICAL_BARBARIAN_BLOCKADE_RESOURCE:
			PlotBlockadeMoves();
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
			PlotPlunderTradeUnitMoves(DOMAIN_LAND);
			PlotPlunderTradeUnitMoves(DOMAIN_SEA);
			break;
		}
	}
}

/// Assign a group of units to take down each city we can capture
bool CvTacticalAI::PlotCaptureCityMoves()
{
	bool bAttackMade = false;

	// See how many moves of this type we can execute
	CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while(pTarget != NULL)
	{
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
				CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, true);

				if (!pZone || (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && !IsTemporaryZoneCity(pCity) && !pCity->isInDangerOfFalling()))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Zone %d, City of %s, is in enemy dominated zone - won't try to capture, X: %d, Y: %d, ",
							pZone ? pZone->GetZoneID() : -1, pCity->getName().c_str(), pCity->getX(), pCity->getY());
						LogTacticalMessage(strLogString);
					}

					pTarget = GetNextZoneTarget();
					continue;
				}
			}

			// Do we have enough firepower to destroy it?
			int iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
			pTarget->SetAuxIntData(iRequiredDamage);
			// If we have the city already down to minimum, don't use ranged... Only try to capture.
			bool bNoRangedUnits = (iRequiredDamage <= 1);
			// Always recruit both naval and land based forces if available!
			if(FindUnitsWithinStrikingDistance(pPlot, bNoRangedUnits, false))
			{
				int iExpectedDamage = ComputeTotalExpectedDamage(pTarget, pPlot);

				if(iExpectedDamage < iRequiredDamage)
				{
					if (GC.getLogging() && GC.getAILogging() && pZone)
					{
						CvString strLogString;
						strLogString.Format("Zone %d, too early for capture of %s, required damage %d, expected damage %d", 
							pZone ? pZone->GetZoneID() : -1, pCity->getName().c_str(), iRequiredDamage, iExpectedDamage);
						LogTacticalMessage(strLogString);
					}
					pTarget = GetNextZoneTarget();
					continue;
				}

				//see whether we have melee units for capturing
				int iRangedCount = 0, iMeleeCount = 0;
				for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
				{
					CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
					if (!pUnit || !pUnit->canMove())
						continue;

					// Are we a melee unit
					if (pUnit->IsCanAttackRanged())
						iRangedCount++;
					else
						iMeleeCount++;
				}

				if (iMeleeCount == 0)
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Zone %d, no melee units to capture %s", pZone ? pZone->GetZoneID() : -1, pCity->getName().c_str());
						LogTacticalMessage(strLogString);
					}
					pTarget = GetNextZoneTarget();
					continue;
				}

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Zone %d, attempting capture of %s, required damage %d, expected damage %d",
						pZone ? pZone->GetZoneID() : -1, pCity->getName().c_str(), iRequiredDamage, iExpectedDamage);
					LogTacticalMessage(strLogString);
				}

				bAttackMade = ExecuteAttackWithUnits(pPlot, AL_HIGH);

				// Did it work?  If so, don't need a temporary dominance zone if had one here
				if (pPlot->getOwner() == m_pPlayer->GetID())
				{
					DeleteTemporaryZone(pPlot);
					pZone->SetNavalInvasion(false);
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
	bool bAttackMade = false;

	// See how many moves of this type we can execute
	CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(pPlot != NULL && pPlot->isCity() )
		{
			m_CurrentMoveCities.clear();
			CvCity* pCity = pPlot->getPlotCity();

			//first try the land zone
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, false);

			//does it look good there?
			if (!pZone || (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && !IsTemporaryZoneCity(pCity)))
			{
				//try again with the water zone
				CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, true);

				if (!pZone || (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && !IsTemporaryZoneCity(pCity)))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Zone %d, City of %s, is in enemy dominated zone - won't try to damage, X: %d, Y: %d, ",
							pZone ? pZone->GetZoneID() : -1, pCity->getName().c_str(), pCity->getX(), pCity->getY());
						LogTacticalMessage(strLogString);
					}

					pTarget = GetNextZoneTarget();
					continue;
				}
			}

			int iRequiredDamage = pCity->GetMaxHitPoints() - pCity->getDamage();
			pTarget->SetAuxIntData(iRequiredDamage);
			// If we have the city already down to minimum, don't use ranged... Only try to capture.
			bool bNoRangedUnits = (iRequiredDamage <= 1);
			// ideally we should check unit danger, respectively if it can survive an attack
			// always recruit both naval and land based forces if available!
			if(FindUnitsWithinStrikingDistance(pPlot, bNoRangedUnits, false))
			{
				int iExpectedDamage = ComputeTotalExpectedDamage(pTarget, pPlot);

				//Let's encourage sneak attack potency
				int iRequiredDamageDivisor = IsTemporaryZoneCity(pCity) ? 15 : 10;

				// Don't want to hammer away to try and take down a city for more than X turns
				if ((iExpectedDamage - GC.getCITY_HIT_POINTS_HEALED_PER_TURN()) < (iRequiredDamage / iRequiredDamageDivisor))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Zone %d, Siege of %s is pointless, required damage %d, expected damage %d", 
							pZone ? pZone->GetZoneID() : -1, pCity->getName().c_str(), iRequiredDamage, iExpectedDamage);
						LogTacticalMessage(strLogString);
					}

					pTarget = GetNextZoneTarget();
					continue;
				}

				//see whether we need to preserve melee units for capturing
				int iRangedCount = 0, iMeleeCount = 0;
				for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
				{
					CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
					if (!pUnit || !pUnit->canMove())
						continue;

					// Are we a melee unit
					if (pUnit->IsCanAttackRanged())
						iRangedCount++;
					else
						iMeleeCount++;
				}

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Zone %d, Laying siege to %s, required damage %d, expected damage %d",
						pZone ? pZone->GetZoneID() : -1, pCity->getName().c_str(), iRequiredDamage, iExpectedDamage);
					LogTacticalMessage(strLogString);
				}

				// Fire away!
				bAttackMade = ExecuteAttackWithUnits(pPlot, iMeleeCount<3 ? AL_MEDIUM : AL_HIGH);

				//do we have embarked units we need to put ashore
				if (FindEmbarkedUnitsAroundTarget(pPlot, 4))
					ExecuteLandingOperation(pPlot);
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
	CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindClosestUnit(pPlot,4,false))
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

void CvTacticalAI::PlotDamageCivilianMoves(AITacticalTargetType targetType)
{
	// See how many moves of this type we can execute
	CvTacticalTarget* pTarget = GetFirstZoneTarget(targetType);
	while(pTarget != NULL)
	{
		// See what units we have who can reach targets this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvUnit* pVictim = pPlot->getUnitByIndex(0);
		if(pVictim && m_pPlayer->IsAtWarWith(pVictim->getOwner()))
		{
			if(FindUnitsWithinStrikingDistance(pPlot))
			{
				// First loop is ranged units only
				for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
				{
					CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
					// Only ranged units, otherwise this would be a capture move
					if(!pUnit || !pUnit->IsCanAttackRanged())
						continue;

					// Are we in range and do we have LOS to the target?
					if(pUnit->canEverRangeStrikeAt(pPlot->getX(), pPlot->getY()))
					{
						if( pUnit->getDomainType() != DOMAIN_AIR)
							pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pTarget->GetTargetX(), pTarget->GetTargetY());
						else
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->GetTargetX(), pTarget->GetTargetY());

						if (!pUnit->canMove())
							UnitProcessed(pUnit->GetID());

						if (pVictim->isDelayedDeath())
							break;
					}
				}
			}
		}

		pTarget = GetNextZoneTarget();
	}
}

/// Assign a group of units to attack each unit we think we can destroy
void CvTacticalAI::PlotDestroyUnitMoves(AITacticalTargetType targetType, bool bMustBeAbleToKill, bool bAttackAtPoorOdds)
{
	vector<SPlotWithTwoScoresTiebreak> targets;

	// See how many moves of this type we can execute
	CvTacticalTarget* pTarget = GetFirstZoneTarget(targetType);
	while (pTarget != NULL)
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
				int iTiebreak = 6 - pPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(), NO_DOMAIN) - pPlot->getNumAdjacentNonvisible(m_pPlayer->getTeam());

				targets.push_back( SPlotWithTwoScoresTiebreak(pPlot,iExpectedDamage-iRequiredDamage,iTiebreak) );
			}
		}

		pTarget = GetNextZoneTarget();
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
			
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				CvString strTemp = pCity->getName();
				strMsg.Format("Made ranged attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), pCity->getX(), pCity->getY(), pDefender->getX(), pDefender->getY());
				LogTacticalMessage(strMsg);
			}

			pCity->doTask(TASK_RANGED_ATTACK, pDefender->getX(), pDefender->getY(), 0);

			if (pDefender->GetCurrHitPoints() < 1)
				break;
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

			ExecuteAttackWithUnits(targets[i].pPlot, bAttackAtPoorOdds ? AL_HIGH : AL_MEDIUM);
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

				ExecuteAttackWithUnits(targets[i].pPlot, bAttackAtPoorOdds ? AL_HIGH : AL_MEDIUM);
			}
		}
	}
}

/// Moved endangered units to safe hexes
void CvTacticalAI::PlotMovesToSafety(bool bCombatUnits)
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Danger value of plot must be greater than 0
			int iDangerLevel = pUnit->GetDanger();
			if(iDangerLevel > 0)
			{
				bool bAddUnit = false;
				if(bCombatUnits)
				{
					// If under 100% health, might flee to safety
					if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
					{
#if defined(MOD_BALANCE_CORE)
						int iDamage = pUnit->plot()->getTurnDamage(pUnit->ignoreTerrainDamage(), pUnit->ignoreFeatureDamage(), pUnit->extraTerrainDamage(), pUnit->extraFeatureDamage());
#endif
						if(pUnit->isBarbarian())
						{
							// Barbarian combat units - only naval units flee (but they flee if have taken ANY damage)
							if(pUnit->getDomainType() == DOMAIN_SEA)
							{
								bAddUnit = true;
							}
						}

#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
						else if(iDamage > 0 && (((pUnit->getDamage()*100)/pUnit->GetMaxHitPoints())>50))
						{
							bAddUnit = true;
						}
						// Everyone else flees at more than 70% damage
						else if(MOD_AI_SMART_FLEE_FROM_DANGER && (((pUnit->getDamage()*100)/pUnit->GetMaxHitPoints())>70) )
						{
							bAddUnit = true;
						}
						// Everyone else flees at less than or equal to 50% combat strength
						else if(pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
						{
							bAddUnit = true;
						}
						// Everyone flees under (heavy) enemy fire
						else if(pUnit->isProjectedToDieNextTurn())
						{
							bAddUnit = true;
						}

						//But not if we're in a city!
						if(pUnit->IsGarrisoned())
						{
							bAddUnit = false;
						}
						if(pUnit->getArmyID() != -1)
						{
							bAddUnit = false;
						}

#else
						// Everyone else flees at less than or equal to 50% combat strength
						else if(pUnit->IsUnderEnemyRangedAttack() || pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
						{
							bAddUnit = true;
						}
#endif
					}

					// Also flee if danger is really high in current plot (but not if we're barbarian)
					//Not if we're operational units!
					else if(!pUnit->isBarbarian() && pUnit->getArmyID() == -1 && !pUnit->IsRecentlyDeployedFromOperation())
					{
						if(iDangerLevel*1.5 > pUnit->GetCurrHitPoints())
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if (!pUnit->canMove())
				continue;
			
			if (pUnit->hasCurrentTacticalMove())
				continue;

			// Never use this (default) move for Great Admirals or Generals
			if (pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral() || pUnit->IsCityAttackSupport())
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
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
			CvUnit* pUnit = m_pPlayer->getUnit(*it);

			// Find any civilians we may have "acquired" from the civs
			if(!pUnit->IsCombatUnit())
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
void CvTacticalAI::PlotPlunderTradeUnitMoves(DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = (eDomain == DOMAIN_LAND) ? AI_TACTICAL_TARGET_TRADE_UNIT_LAND : AI_TACTICAL_TARGET_TRADE_UNIT_SEA;

	CvTacticalTarget* pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		if (FindUnitsCloseToPlot(pPlot,1,10,100,eDomain,false))
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

/// Process units that we recruited out of operational moves.
void CvTacticalAI::PlotOperationalArmyMoves()
{
	std::vector<int> opsToKill;

	// move all units in operations
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

	for(CvTacticalTarget* pTarget = GetFirstZoneTarget(eTarget); pTarget != NULL; pTarget = GetNextZoneTarget())
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		// Don't do it if an enemy unit became visible in the meantime
		if (pPlot->getVisibleEnemyDefender(m_pPlayer->GetID()) != NULL)
			continue;
		if (eTarget != AI_TACTICAL_TARGET_CITADEL && m_pPlayer->isEnemyCombatUnitAdjacent(pPlot,true))
			continue;

		//no targeted pillage on water (opportunistic pillage is handled elsewhere)
		if (pPlot->isWater())
			continue;

		// try paratroopers first, not because they are more effective, just because it looks cooler...
		if (bImmediate && pPlot->isVisible(m_pPlayer->getTeam()) && FindParatroopersWithinStrikingDistance(pPlot))
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
		else if (bImmediate && FindUnitsCloseToPlot(pPlot,0,GC.getMAX_HIT_POINTS()/3,GC.getMAX_HIT_POINTS()-iMinDamage,DOMAIN_LAND,true))
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
		else if (!bImmediate && FindUnitsCloseToPlot(pPlot,2,GC.getMAX_HIT_POINTS()/3,GC.getMAX_HIT_POINTS()-iMinDamage,DOMAIN_LAND,true))
		{
			CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;

			if (pUnit && pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
				ExecuteMoveToPlot(pUnit,pPlot,false,CvUnit::MOVEFLAG_NO_EMBARK);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving toward %s for pillage, X: %d, Y: %d", szTargetName.GetCString(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
	}
}

/// Assigns units to sit on trade routes it knows about and wait for enemy trade units
void CvTacticalAI::PlotPlunderTradePlotMoves (DomainTypes eDomain)
{
	AITacticalTargetType eTargetType = (eDomain == DOMAIN_LAND) ? AI_TACTICAL_TARGET_TRADE_UNIT_LAND_PLOT : AI_TACTICAL_TARGET_TRADE_UNIT_SEA_PLOT;

	CvTacticalTarget* pTarget = GetFirstZoneTarget(eTargetType);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsCloseToPlot(pPlot,1,10,100,eDomain,false))
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
			if(pUnit)
			{
				ExecuteMoveToPlot(pUnit, pPlot, true, CvUnit::MOVEFLAG_NO_EMBARK);
				if (pUnit->canPillage(pUnit->plot()) && pUnit->getDamage() > 0)
				{
					pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Pillaging during a plunder mission with %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						LogTacticalMessage(strMsg, false);
					}
				}
			
				// Delete this unit from those we have to move
				UnitProcessed(pUnit->GetID());
			}
		}

		pTarget = GetNextZoneTarget();
	}
}

/// Move barbarian ships to disrupt usage of water improvements
void CvTacticalAI::PlotBlockadeMoves()
{
	CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BLOCKADE_POINT);
	while(pTarget != NULL)
	{
		// See what units we have who can reach target this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if(FindUnitsCloseToPlot(pPlot, 4, 10, 100, DOMAIN_SEA,false))
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
		if(FindUnitsCloseToPlot(pPlot,1,10,100,DOMAIN_LAND,false))
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
		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}

	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
	while(pTarget != NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()))
	{
		ExecuteSafeBombards(*pTarget);
		pTarget = GetNextZoneTarget();
	}
}

/// Assigns units to heal
void CvTacticalAI::PlotHealMoves()
{
	list<int>::iterator it;
	m_CurrentMoveUnits.clear();

	//not all units go through ExecuteHeals
	std::set<int> passiveAggressive;

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit == NULL)
			continue;

		//the really bad cases have already been added in CommandeerUnits
		//now we need to take care of the units which are damaged and have nothing else to do

		CvPlot* pUnitPlot = pUnit->plot();
		if (pUnitPlot == NULL)
			continue;
		int iAcceptableDamage = 20;

		//allow some more damage outside of our borders
		if (pUnitPlot->getOwner() != pUnit->getOwner())
		{
			iAcceptableDamage = 40;
			if (pUnit->getDamage() > iAcceptableDamage && pUnit->getArmyID() == -1)
			{
				if (pUnit->pillage())
				{
					if (!pUnit->canMove())
					{
						UnitProcessed(pUnit->GetID());
						continue;
					}
				}
			}
		}


		if (pUnit->getDamage() > iAcceptableDamage && pUnit->getArmyID() == -1 && FindNearbyTarget(pUnit, 5) == NULL)
			m_HealingUnits.insert(pUnit->GetID());

		//units with area damage if fortified should fortify as much as possible if near enemies
		if (pUnit->GetDamageAoEFortified() > 0 && pUnit->canFortify(pUnitPlot) &&
			pUnit->getDamage() >= pUnit->healRate(pUnitPlot) && pUnit->GetDanger()<pUnit->GetCurrHitPoints() &&
			pUnitPlot->IsEnemyCityAdjacent(pUnit->getTeam(),NULL)==false &&
			pUnitPlot->GetNumEnemyUnitsAdjacent(pUnit->getTeam(), pUnit->getDomainType()) > 0)
		{
			passiveAggressive.insert(pUnit->GetID());
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
		}
	}

	//can't do this directly, otherwise we invalidate the iterator for m_CurrentTurnUnits
	for (set<int>::iterator it = passiveAggressive.begin(); it != passiveAggressive.end(); ++it)
		UnitProcessed(*it);

	if(m_HealingUnits.size() > 0)
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

		//for the barbarian player AI_TACTICAL_TARGET_BARBARIAN_CAMP does not automatically mean the camp is empty of _barbarian_ defenders (check is only for enemy units)
		CvUnit* currentDefender = pPlot->getBestDefender(BARBARIAN_PLAYER);
		if (currentDefender)
		{
			if (currentDefender->CanUpgradeRightNow(true))
			{
				currentDefender->DoUpgrade(true);
				UnitProcessed(currentDefender->GetID());
			}
			else
			{
				TacticalAIHelpers::PerformOpportunityAttack(currentDefender);
				if (currentDefender->canFortify(pPlot))
				{
					currentDefender->PushMission(CvTypes::getMISSION_FORTIFY());
					currentDefender->SetFortifiedThisTurn(true);
				}
				else
					currentDefender->PushMission(CvTypes::getMISSION_SKIP());
				UnitProcessed(currentDefender->GetID());
			}

			//that's a hack but that's the way it is
			currentDefender->setTacticalMove( (TacticalAIMoveTypes)AI_TACTICAL_BARBARIAN_CAMP_DEFENSE );
		}
		else if(FindClosestUnit(pPlot,5,false,false,0,false,false,false,NULL,1))
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
		pTarget = GetNextZoneTarget();
	}
}

/// Make a defensive move to garrison a city
void CvTacticalAI::PlotGarrisonMoves(int iNumTurnsAway, bool bMustAllowRangedAttack)
{
	CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
	while(pTarget != NULL)
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvCity* pCity = pPlot->getPlotCity();

		if(!pCity)
			continue;

		//an explorer can be a transient garrison but we don't want to keep it here
		CvUnit* pGarrison = pCity->GetGarrisonedUnit();
		if (pGarrison && pGarrison->AI_getUnitAIType()!=UNITAI_EXPLORE)
		{
			//ranged garrisons are used in ExecuteSafeBombards. 
			//special handling only needed for melee garrisons here. note: garrison will not advance
			TacticalAIHelpers::PerformOpportunityAttack(pGarrison);

			//can we maybe capture a lone civilian
			for (int i=RING0_PLOTS; i<RING2_PLOTS; i++)
			{
				CvPlot* pNeighbor = iterateRingPlots( pPlot,i );
				if (!pNeighbor)
					continue;

				if (!pNeighbor->isEnemyUnit(m_pPlayer->GetID(), true, true) &&
					pNeighbor->isEnemyUnit(m_pPlayer->GetID(), false, true) &&
					pGarrison->TurnsToReachTarget(pNeighbor, 0, 1) < 2 &&
					pGarrison->GetDanger(pNeighbor) == 0 &&
					pGarrison->GetDanger() == 0 &&
					pCity->getDamage() == 0)
				{
					pGarrison->PushMission(CvTypes::getMISSION_MOVE_TO(), pNeighbor->getX(), pNeighbor->getY());
					if (pGarrison->canMove()) //try to move back
						pGarrison->PushMission(CvTypes::getMISSION_MOVE_TO(), pCity->getX(), pCity->getY());
					break;
				}
			}

			if (pGarrison->canFortify(pGarrison->plot()))
			{
				pGarrison->PushMission(CvTypes::getMISSION_FORTIFY());
				pGarrison->SetFortifiedThisTurn(true);
			}
			else
				pGarrison->PushMission(CvTypes::getMISSION_SKIP());

			//do not call finishMoves() else the garrison will not heal!
			UnitProcessed(pGarrison->GetID());
		}
		else if ( !pCity->isInDangerOfFalling() )
		{
			// Grab units that make sense for this move type
			FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE], pPlot, iNumTurnsAway, bMustAllowRangedAttack);

			CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;

			if (ExecuteMoveToPlot(pUnit, pPlot))
			{
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
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("No unit for garrison in %s at (%d:%d)", pCity->getName().c_str(), pTarget->GetTargetX(), pTarget->GetTargetY());
					LogTacticalMessage(strLogString);
				}
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
		CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;

		//move may fail if the plot is already occupied (can happen if another unit moved there during this turn)
		if (ExecuteMoveToPlot(pUnit, pPlot))
		{
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
	CvTacticalTarget* pTarget;
	pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
	while(pTarget != NULL)
	{
		// Grab units that make sense for this move type
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		FindUnitsForThisMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE], pPlot, iNumTurnsAway);
		CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;

		//move may fail if the plot is already occupied (can happen if another unit moved there during this turn)
		if (ExecuteMoveToPlot(pUnit, pPlot))
		{
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
		CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;

		//move may fail if the plot is already occupied (can happen if another unit moved there during this turn)
		if (ExecuteMoveToPlot(pUnit, pPlot,true))
		{
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Am I eligible to intercept?
#if defined(MOD_AI_SMART_AIR_TACTICS)
			// we only commandeered units which won't be rebased
			if(pUnit->canAirPatrol(NULL))
			{
				CvPlot* pUnitPlot = pUnit->plot();
				if (MOD_AI_SMART_AIR_TACTICS)
				{
					int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), false/*bCountFighters*/, true/*bCountBombers*/);
					int iNumNearbyFighters = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), true/*bCountFighters*/, false/*bCountBombers*/);
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
#else
			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit))
			{
				CvPlot* pUnitPlot = pUnit->plot();

#endif
					CvCity* pCity = pUnitPlot->getPlotCity();
					pZone = NULL;

					if (pCity)
						pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, false);

					int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, m_iRecruitRange, false/*bCountFighters*/, true/*bCountBombers*/);

					// On a carrier or in a city where we are not dominant and near some enemy bombers?
					if (!pCity || !pZone || pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
#if defined(MOD_UNITS_MAX_HP)
		if(pUnit && (pUnit->getDamage() * 2) < pUnit->GetMaxHitPoints())
#else
		if(pUnit && (pUnit->getDamage() * 2) < GC.getMAX_HIT_POINTS())
#endif
		{
			// Am I eligible to air sweep and have a target?
#if defined(MOD_BALANCE_CORE)
			// we have only units here which won't be rebased
			if(pUnit->canAirSweep() && m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit) != NULL)
#else
			if(pUnit->canAirSweep() && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit) && m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit) != NULL)
#endif
			{
				CvPlot* pUnitPlot = pUnit->plot();
				CvCity* pCity = pUnitPlot->getPlotCity();
				pZone = NULL;

				// On a carrier or in a city where we are dominant?
				if (pCity)
					pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, false);

				if (!pCity || !pZone || pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
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
	if(m_pPlayer->isMinorCiv())
		return;

	// Is this a dominance zone where we're defending a city?
	CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);
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
	CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);
	pCity = pZone->GetZoneCity();
	if(pCity && pCity->getOwner() == m_pPlayer->GetID() && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && pCity->CanAirlift()&& pZone->GetTotalEnemyUnitCount() > 0)
	{
		// Loop through all recruited units
		for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
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

#if defined(MOD_BALANCE_CORE_MILITARY)
	std::vector<CvUnit*> vEmbarkedUnits;
	int iLoop = 0;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit->isEmbarked() && pLoopUnit->getDomainType() != DOMAIN_SEA)
			vEmbarkedUnits.push_back(pLoopUnit);
	}

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
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
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);
						break;
					}
				}
			}
		}
	}
#else
	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Am I a naval combat unit?
#if defined(MOD_BALANCE_CORE)
			if(pUnit->getDomainType() == DOMAIN_SEA && pUnit->IsCombatUnit() && pUnit->getArmyID() == -1)
#else
			if(pUnit->getDomainType() == DOMAIN_SEA && pUnit->IsCombatUnit())
#endif
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}
#endif

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

	CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);
	if(pZone->GetZoneCity() != NULL)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetZoneCity()->getOwner());
		target.SetTargetX(pZone->GetZoneCity()->plot()->getX());
		target.SetTargetY(pZone->GetZoneCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetZoneID());
		pTarget = GC.getMap().plot(target.GetTargetX(), target.GetTargetY());

		// Attack primary target (city)
		ExecuteSafeBombards(target);
		if(target.IsTargetStillAlive(m_pPlayer->GetID()))
			PlotDamageCityMoves();

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
					pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
					ExecuteSafeBombards(m_ZoneTargets[iI]);
				}
			}
		}

		PlotCloseOnTarget();
	}
}

/// Win an attrition campaign with bombardments
void CvTacticalAI::PlotAttritFromRangeMoves()
{
	CvPlot* pTarget;

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
				pTarget = GC.getMap().plot(m_ZoneTargets[iI].GetTargetX(), m_ZoneTargets[iI].GetTargetY());
				ExecuteSafeBombards(m_ZoneTargets[iI]);
			}
		}
	}
}

/// Defeat enemy units by using our advantage in numbers
void CvTacticalAI::PlotExploitFlanksMoves()
{
	m_TempTargets.clear();

	// Loop through unit targets finding attack for this turn
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			ExecuteFlankAttack(m_ZoneTargets[iI]);
		}
	}
}

/// We have more overall strength than enemy, defeat his army first
void CvTacticalAI::PlotSteamrollMoves()
{
	m_TempTargets.clear();

	//try capture first! will only do something if we have enough firepower.
	PlotCaptureCityMoves();

	// See if there are any kill attacks we can make.
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		CvTacticalTarget kTarget = m_ZoneTargets[iI];
		if(!kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
			continue;

		if(kTarget.GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT || kTarget.GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
			ExecuteSafeBombards(kTarget);
	}

	// We have superiority, so a let's attack high prio targets even with bad odds
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);

	// See if it is time to go after the city
	PlotDamageCityMoves();

	// Bring in more units
	PlotCloseOnTarget();
}

/// We should be strong enough to take out the city before the enemy can whittle us down with ranged attacks
void CvTacticalAI::PlotSurgicalCityStrikeMoves()
{
	CvTacticalTarget target;
	CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);

	if(pZone->GetZoneCity() != NULL)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetZoneCity()->getOwner());
		target.SetTargetX(pZone->GetZoneCity()->plot()->getX());
		target.SetTargetY(pZone->GetZoneCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetZoneID());

		//try capture first! will only do something if we have enough firepower.
		PlotCaptureCityMoves();

		if(target.IsTargetStillAlive(m_pPlayer->GetID()))
			PlotDamageCityMoves();

		// Take any other really good attacks we've set up
		// Don't need to be able to kill them - if we ignore them they might attack our siege units
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false);
		PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false);

		PlotCloseOnTarget();
	}
}

/// Build a defensive shell around this city
void CvTacticalAI::PlotHedgehogMoves()
{
	// Attack priority unit targets
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		CvTacticalTarget kTarget = m_ZoneTargets[iI];
		if(!kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
			continue;

		if(kTarget.GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT || kTarget.GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
			ExecuteSafeBombards(kTarget);
	}

	// But after best attacks are exhausted, go right to playing defense
	CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);
	if(pZone->GetZoneCity() != NULL)
	{
		CvTacticalTarget target;
		target.SetTargetType(AI_TACTICAL_TARGET_CITY);
		target.SetTargetPlayer(pZone->GetZoneCity()->getOwner());
		target.SetTargetX(pZone->GetZoneCity()->plot()->getX());
		target.SetTargetY(pZone->GetZoneCity()->plot()->getY());
		target.SetDominanceZone(pZone->GetZoneID());
		ExecuteCloseOnTarget(target, pZone, false);
	}
}

/// Try to push back the invader
void CvTacticalAI::PlotCounterattackMoves()
{
	// Attack priority unit targets
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	PlotDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		CvTacticalTarget kTarget = m_ZoneTargets[iI];
		if(!kTarget.IsTargetStillAlive(m_pPlayer->GetID()))
			continue;

		if(kTarget.GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT || kTarget.GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
			ExecuteSafeBombards(kTarget);
	}

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

	CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);

	// Loop through all recruited units
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
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
			if(pUnitZone == pZone)
			{
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
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
	for(unsigned int iI = 0; iI < m_ZoneTargets.size(); iI++)
	{
		// Is the target of an appropriate type?
		if(m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
		        m_ZoneTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			if(m_ZoneTargets[iI].IsTargetStillAlive(m_pPlayer->GetID()))
			{
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
				ExecuteSafeBombards(m_ZoneTargets[iI]);
			}
		}
	}
}

/// Close units in on primary target of this dominance zone
void CvTacticalAI::PlotCloseOnTarget()
{
	CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);
	if (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
		return;

	// Flank attacks done; if in an enemy zone, close in on target
	CvTacticalTarget target;
	if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
	{
		target.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		target.SetTargetPlayer(BARBARIAN_PLAYER);
		target.SetTargetX(pZone->GetCenterX());
		target.SetTargetY(pZone->GetCenterY());
		target.SetDominanceZone(pZone->GetZoneID());

		ExecuteCloseOnTarget(target, pZone, true);
	}
	else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY && pZone->GetZoneCity() != NULL)
	{
		bool bCanSeeCity = pZone->GetZoneCity()->plot()->isVisible(m_pPlayer->getTeam());

		// If we can't see the city, be careful advancing on it.  We want to be sure we're not heavily outnumbered
		//Exception for temporary targets - we need to press the offensive here.
		if(IsTemporaryZoneCity(pZone->GetZoneCity()) || bCanSeeCity || pZone->GetOverallFriendlyStrength()*3 > pZone->GetOverallEnemyStrength()*2)
		{
			target.SetTargetType(AI_TACTICAL_TARGET_CITY);
			target.SetTargetPlayer(pZone->GetZoneCity()->getOwner());
			target.SetTargetX(pZone->GetZoneCity()->plot()->getX());
			target.SetTargetY(pZone->GetZoneCity()->plot()->getY());
			target.SetDominanceZone(pZone->GetZoneID());

			ExecuteCloseOnTarget(target, pZone, true);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
#if defined(MOD_BALANCE_CORE)
			//in any case: homeland AI may use recruit the units if it wants to
			pUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_UNASSIGNED]);

			//If didn't JUST deploy, reset that so that the AI can grab it.
			if(pUnit->GetDeployFromOperationTurn() > (GC.getGame().getGameTurn() + 1))
			{
				pUnit->SetDeployFromOperationTurn(-1);
			}

			//barbarians have no homeland AI, so they end their turn here
			if (pUnit->isBarbarian())
			{
				MissionTypes eMission = pUnit->canFortify(pUnit->plot()) ? CvTypes::getMISSION_FORTIFY() : CvTypes::getMISSION_SKIP();
				pUnit->PushMission(eMission);
				if (pUnit->canFortify(pUnit->plot()))
					pUnit->SetFortifiedThisTurn(true);

				pUnit->SetTurnProcessed(true);
			}
			else if ( pUnit->getDomainType()==DOMAIN_LAND )
			{
				//tactical reposition failed ... but well, maybe homeland AI can use the unit
				/*
				CvString missionInfo = (pUnit->getTacticalMove()==NO_TACTICAL_MOVE) ? "no tactical move" : GC.getTacticalMoveInfo(pUnit->getTacticalMove())->GetType();
			
				OutputDebugString( CvString::format( "unassigned %s tactical unit %s %d at %d,%d (last move: %s)\n", 
					m_pPlayer->getCivilizationAdjective(), pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY(), missionInfo.c_str() ).c_str() ); 
				*/
			}
			else if ( pUnit->getDomainType()==DOMAIN_AIR )
			{
				//homeland doesn't handle air units except for rebasing
				//units to be rebased are passed over for tactical AI, the rest simply does nothing
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				pUnit->SetTurnProcessed(true);
			}
			else if ( pUnit->getDomainType()==DOMAIN_SEA )
			{
				//don't know if homeland AI does anything with the navy ...
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				pUnit->SetTurnProcessed(true);
			}
#else
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
#endif
		}
	}
}

// OPERATIONAL AI SUPPORT FUNCTIONS

CvUnit* SwitchEscort(CvUnit* pCivilian, CvUnit* pEscort, CvArmyAI* pThisArmy)
{
	CvUnit* pPlotDefender = pCivilian->plot()->getBestDefender(pCivilian->getOwner());

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
	CvUnit* pEscort = pThisArmy->GetNextUnit();
	//additional escorts
	std::vector<CvUnit*> vExtraEscorts;
	CvUnit* pExtraEscort = pThisArmy->GetNextUnit();
	while (pExtraEscort)
	{
		vExtraEscorts.push_back(pExtraEscort);
		pExtraEscort = pThisArmy->GetNextUnit(); 
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
			//civilian and escort have not yet met up
			if(pEscort)
			{
				//civilian is already there
				if(pCivilian->plot() == pOperation->GetMusterPlot())
				{
					//another military unit is blocking our escort ... find another muster plot
					if(pCivilian->plot()->GetNumCombatUnits() > 0)
					{
						CvUnit* pNewEscort = SwitchEscort(pCivilian,pEscort,pThisArmy);
						if (!pNewEscort)
						{
							//Let's have them move forward, see if that clears things up.
							ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(),false,CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
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
						//check if the civilian is in danger
						if ( pCivilian->GetDanger() > 0 )
						{
							//try to move to safety
							CvPlot* pBetterPlot = TacticalAIHelpers::FindSafestPlotInReach(pCivilian,true);
							if (pBetterPlot)
							{
								ExecuteMoveToPlot(pCivilian,pBetterPlot);
								pOperation->SetMusterPlot(pBetterPlot);
							}
						}

						//move escort towards civilian
						if (!ExecuteMoveToPlot(pEscort, pCivilian->plot()))
						{
							//d'oh. escort cannot reach us
							CvUnit* pNewEscort = SwitchEscort(pCivilian,pEscort,pThisArmy);

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
				if(pCivilian->plot() == pOperation->GetMusterPlot())
				{
					//check if the civilian is in danger
					if ( pCivilian->GetDanger()>0 )
					{
						//try to move to safety
						CvPlot* pBetterPlot = TacticalAIHelpers::FindSafestPlotInReach(pCivilian,true);
						if (pBetterPlot)
						{
							ExecuteMoveToPlot(pCivilian,pBetterPlot);

							//update this so we can advance to next stage
							pOperation->SetMusterPlot(pBetterPlot);
						}
					}
					UnitProcessed(pCivilian->GetID());
				}
				else
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

		int iMoveFlags = CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_DONT_STACK_WITH_NEUTRAL;
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
									pAltPlot = pCivilian->GetPathNodeArray().GetFirstPlot();
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
				CvPlot* pCommonPlot = pCivilian->GetPathEndFirstTurnPlot();
				if(pCommonPlot != NULL)
				{
					if (pCivilian->GetDanger(pCommonPlot) == INT_MAX)
						pCommonPlot = TacticalAIHelpers::FindSafestPlotInReach(pCivilian, true, false, true);

					bSaveMoves = (pCommonPlot == pOperation->GetTargetPlot());
					ExecuteMoveToPlot(pCivilian, pCommonPlot, bSaveMoves);
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
	if (!pOperation || pOperation->GetMusterPlot()==NULL)
		return;

	m_OperationUnits.clear();
	m_GeneralsToMove.clear();

	ClearEnemiesNearArmy(pThisArmy);

	// RECRUITING
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE || 
		pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		// This is where we try to gather. Don't use the center of mass here, it may drift anywhere 
		pThisArmy->SetXY(pOperation->GetMusterPlot()->getX(), pOperation->GetMusterPlot()->getY());

		// Request moves for all units
		for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
			if(pSlot->GetUnitID() != NO_UNIT)
			{
				CvUnit* pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if(pUnit && pUnit->canMove())
				{
					// Great general or admiral?
					if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral() || pUnit->IsCityAttackSupport())
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

		ExecuteGatherMoves(pThisArmy);
	}

	// MOVING TO TARGET
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		// Update army's current location
		CvPlot* pCOM = pThisArmy->GetCenterOfMass();
		if (!pCOM)
		{
			pOperation->SetToAbort(AI_ABORT_LOST_PATH);
			return;
		}
		pThisArmy->SetXY(pCOM->getX(), pCOM->getY());

		//where do we want to go
		CvPlot* pThisTurnTarget = pOperation->ComputeTargetPlotForThisTurn(pThisArmy);
		if(pThisTurnTarget == NULL)
		{
			pOperation->SetToAbort(AI_ABORT_LOST_PATH);
			return;
		}

		// Request moves for all units
		for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
			if(pSlot->GetUnitID() != NO_UNIT)
			{
				CvUnit* pUnit = m_pPlayer->getUnit(pSlot->GetUnitID());
				if(pUnit && pUnit->canMove())
				{
					// Great general?
					if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral() || pUnit->IsCityAttackSupport())
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

		if (pThisArmy->GetDomainType() == DOMAIN_SEA)
		{
			if (pOperation->GetMoveType() == AI_OPERATION_MOVETYPE_ESCORT || pOperation->GetMoveType() == AI_OPERATION_MOVETYPE_COMBAT_ESCORT)
			{
				ExecuteNavalFormationEscortMoves(pThisArmy, pThisTurnTarget);
			}
			else
			{
				ExecuteNavalFormationMoves(pThisArmy, pThisTurnTarget);
			}
		}
		else
		{
			ExecuteFormationMoves(pThisArmy, pThisTurnTarget);
		}
	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral(pThisArmy);
	}
}

/// Queues up attacks on enemy units on or adjacent to army's desired center
void CvTacticalAI::ClearEnemiesNearArmy(CvArmyAI* pArmy)
{
	if (!pArmy)
		return;

	bool bInDanger = false;
	vector<CvUnit*> vUnits;
	CvUnit* pUnit = pArmy->GetFirstUnit();
	while (pUnit)
	{
		if (pUnit->GetDanger() > 0)
			bInDanger = true;
		vUnits.push_back(pUnit);
		pUnit = pArmy->GetNextUnit();
	}

	if (!bInDanger)
		return;

	//just because we can, make a unique set of enemy units
	set<CvUnit*> allEnemies;
	for (size_t i = 0; i < vUnits.size(); i++)
	{
		vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*vUnits[i]->plot());
		allEnemies.insert(vAttackers.begin(), vAttackers.end());
	}

	//find the closest pair of units
	int iMinDist = INT_MAX;
	CvUnit* pClosestEnemy = NULL;
	for (set<CvUnit*>::iterator it = allEnemies.begin(); it != allEnemies.end(); ++it)
	{
		for (size_t i = 0; i < vUnits.size(); i++)
		{
			int iDistance = plotDistance( *vUnits[i]->plot(), *(*it)->plot() );
			if (iDistance < iMinDist)
			{
				iMinDist = iDistance;
				pClosestEnemy = *it;
			}
		}
	}

	//don't get sidetracked
	if (iMinDist>3 || pClosestEnemy==NULL)
		return;
	
	vector<STacticalAssignment> vAssignments;
	int iCount = 0;
	do
	{
		TacticalAIHelpers::FindBestAssignmentsForUnits(vUnits, pClosestEnemy->plot(), AL_HIGH, 3, 23, vAssignments);
		iCount++;
	}
	while (!vAssignments.empty() && !TacticalAIHelpers::ExecuteUnitAssignments(m_pPlayer->GetID(), vAssignments) && iCount < 4);
}

/// Store off a new unit that needs to move as part of an operational AI formation
void CvTacticalAI::MoveWithFormation(CvUnit* pUnit, MultiunitPositionTypes ePosition)
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
	CvPlot* pTarget = pArmy->Plot();
	AITacticalTargetType eTargetType;

	// Gathering - treat everyone as a melee unit; don't need ranged in the rear yet
	int iUnits = m_OperationUnits.size();
	CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());
	if(!pOperation || iUnits==0)
		return;

	// Range around target based on number of units we need to place
	int iRange = pOperation->GetGatherTolerance(pArmy,pTarget);

	// Try one time with computed range
	bool bFoundEnoughDeploymentPlots = false;
	if (ScoreDeploymentPlots(pTarget, pArmy, iUnits, 0, iRange))
	{
		// Did we enough plots with some to spare?
		if (m_TempTargets.size() > (unsigned)iUnits+1)
		{
			bFoundEnoughDeploymentPlots = true;
		}
		else
		{
			m_TempTargets.clear();
		}
	}

	if (!bFoundEnoughDeploymentPlots)
	{
		//try again with additional space
		if (!ScoreDeploymentPlots(pTarget, pArmy, iUnits, 0, iRange++))
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

	// see where our units can go
	std::map<int,ReachablePlots> unitMovePlots;
	for(opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		CvUnit *pOpUnit = m_pPlayer->getUnit(it->GetUnitID());
		SPathFinderUserData data(pOpUnit,CvUnit::MOVEFLAG_IGNORE_STACKING,5);
		data.ePathType = PT_UNIT_REACHABLE_PLOTS;
		unitMovePlots[it->GetUnitID()] = GC.GetPathFinder().GetPlotsInReach(pOpUnit->plot(), data);
	}

	int iUnitsToPlace = iUnits;
	for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
	{
		eTargetType = m_TempTargets[iI].GetTargetType();

		CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
		
		// Don't use if there's already a unit not in the army here
		CvUnit* pUnitAlreadyThere = pLoopPlot->getBestDefender(NO_PLAYER);
		if (!pUnitAlreadyThere || pUnitAlreadyThere->getArmyID() == pArmy->GetID())
		{
			if(FindClosestOperationUnit(pLoopPlot, unitMovePlots, true /*bIncludeRanged*/, false /*bMustBeRangedUnit*/, false))
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

		// Loop through all units available to operation and move them blindly
		for (opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
		{
			CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
			if (!pLoopUnit || !pLoopUnit->canMove())
				continue;

			if (MoveToEmptySpaceNearTarget(pLoopUnit,pTarget,NO_DOMAIN,23))
				UnitProcessed(pLoopUnit->GetID());
			else
				pArmy->RemoveUnit(pLoopUnit->GetID());

		}
	}
}

/// Complete moves for all units requested through calls to MoveWithFormation()
void CvTacticalAI::ExecuteFormationMoves(CvArmyAI* pArmy, CvPlot *pTurnTarget)
{
	if(m_OperationUnits.size() ==  0 || !pArmy)
		return;

	CvPlot* pCurrent = pArmy->Plot();
	if (!pCurrent)
		return;

	int iMeleeUnits = 0;
	int iRangedUnits = 0;
	std::map<int,ReachablePlots> unitMovePlots;
	for(opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
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

		// see where our units can go
		SPathFinderUserData data(pOpUnit,CvUnit::MOVEFLAG_IGNORE_STACKING,2);
		data.ePathType = PT_UNIT_REACHABLE_PLOTS;
		unitMovePlots[it->GetUnitID()] = GC.GetPathFinder().GetPlotsInReach(pOpUnit->plot(), data);
	}

#if defined(MOD_BALANCE_CORE)
	//the direction we are coming from
	DirectionTypes eFromDir = estimateDirection(pTurnTarget->getX(),pTurnTarget->getY(),pCurrent->getX(),pCurrent->getY());
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("Trying to move army %d from (%d:%d), to (%d:%d)",pArmy->GetID(),pCurrent->getX(),pCurrent->getY(),pTurnTarget->getX(),pTurnTarget->getY());
		LogTacticalMessage(strMsg);
	}
#endif

	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iMeleeUnits+iRangedUnits);
	if (pArmy->GetDomainType() == DOMAIN_SEA)
		iRange++;

	// See if we have enough places to put everyone
	if(!ScoreDeploymentPlots(pTurnTarget, pArmy, iMeleeUnits, iRangedUnits, iRange))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy along formation's path", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
		return;
	}

	// Compute the moves to get the best deployment
	std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());

	// First loop for melee units who should be out front
	int iMeleeUnitsToPlace = iMeleeUnits;
	bool bDone = false;
	if(iMeleeUnitsToPlace > 0)
	{
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

			// Don't use if there's already someone here
			if (!pLoopPlot->getBestDefender(NO_PLAYER))
			{
				//find a unit that's a little bit further back, and then try to move it to this good plot
				CvPlot* pIdealUnitPlot = plotDirection(pLoopPlot->getX(),pLoopPlot->getY(),eFromDir);
				if(FindClosestOperationUnit( pIdealUnitPlot, unitMovePlots, false /*bIncludeRanged*/, false /*bMustBeRangedUnit*/, false))
				{
					CvUnit* pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying melee unit (first pass) %d (%s), to %d,%d, from %d,%d", pInnerUnit->GetID(), pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
						LogTacticalMessage(strMsg);
					}
					ExecuteMoveToPlot(pInnerUnit, pLoopPlot);
					iMeleeUnitsToPlace--;
				}
			}
			if (iMeleeUnitsToPlace == 0)
			{
				bDone = true;
			}
		}
	}

	// Second loop for ranged units
	int iRangedUnitsToPlace = iRangedUnits;
	if(iRangedUnitsToPlace > 0)
	{
		bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				// Don't use if there's already someone here
				if (!pLoopPlot->getBestDefender(NO_PLAYER))
				{
					//find a unit that's a little bit further back, and then try to move it to this good plot
					CvPlot* pIdealUnitPlot = plotDirection(pLoopPlot->getX(),pLoopPlot->getY(),eFromDir);
					if(FindClosestOperationUnit( pIdealUnitPlot, unitMovePlots, true /*bIncludeRanged*/, true /*bMustBeRangedUnit*/, false))
					{
						CvUnit* pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying ranged unit (first pass) %d (%s), to %d,%d, from %d,%d", pInnerUnit->GetID(), pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
							LogTacticalMessage(strMsg);
						}
						ExecuteMoveToPlot(pInnerUnit, pLoopPlot);
						iRangedUnitsToPlace--;
					}
				}
			}
			if (iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}
	}

	// Third loop for all units we couldn't put in an ideal spot
	for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

		// Don't use if there's already someone here
		if (!pLoopPlot->getBestDefender(NO_PLAYER))
		{
			//find a unit that's a little bit further back, and then try to move it to this good plot
			CvPlot* pIdealUnitPlot = plotDirection(pLoopPlot->getX(),pLoopPlot->getY(),eFromDir);
			bool bSuitableForRanged = (pLoopPlot->isCity() && pLoopPlot->IsFriendlyTerritory(m_pPlayer->GetID())) ||
										pLoopPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(), pLoopPlot->getDomain(), NULL, false)==0;

			if(FindClosestOperationUnit( pIdealUnitPlot, unitMovePlots, bSuitableForRanged /*bIncludeRanged*/, false /*bMustBeRangedUnit*/, false))
			{
				CvUnit* pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Deploying third wave unit %d (%s), second pass, to %d,%d, from %d,%d", pInnerUnit->GetID(), pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
					LogTacticalMessage(strMsg);
				}

				ExecuteMoveToPlot(pInnerUnit, pLoopPlot);
				if(!pInnerUnit->isRanged())
					iMeleeUnitsToPlace--;
				else
					iRangedUnitsToPlace--;
			}
		}
#if defined(MOD_BALANCE_CORE)
		if (iRangedUnitsToPlace == 0 && iMeleeUnitsToPlace == 0)
#else
		if (iRangedUnitsToPlace == 0)
#endif
		{
			bDone = true;
		}
	}

	// Log if someone in army didn't get a move assigned
#if defined(MOD_BALANCE_CORE)
	if (iRangedUnitsToPlace > 0 || iMeleeUnitsToPlace > 0)
#else
	if (iRangedUnitsToPlace > 0)
#endif
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
#if defined(MOD_BALANCE_CORE)
			strMsg.Format("No army deployment move for %d ranged units, %d melee units.", iRangedUnitsToPlace, iMeleeUnitsToPlace);
#else
			strMsg.Format("No army deployment move for %d ranged units", iRangedUnitsToPlace);
#endif
			LogTacticalMessage(strMsg);
		}

		// Loop through all units available to operation and move them blindly
		for (opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
		{
			CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
			if (!pLoopUnit || !pLoopUnit->canMove())
				continue;

			MoveToEmptySpaceNearTarget(pLoopUnit,pTurnTarget,NO_DOMAIN,12);
		}
	}
}

bool CvTacticalAI::ScoreDeploymentPlots(CvPlot* pTarget, CvArmyAI* pArmy, int iNumMeleeUnits, int iNumRangedUnits, int iRange)
{
	int iScore;
	int iNumSafePlotsFound = 0;
	int iNumDeployPlotsFound = 0;
	CvTacticalTarget target;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

	if (!pArmy || !pTarget)
		return false;

	for(int iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(int iDY = -(iRange); iDY <= iRange; iDY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pTarget->getX(), pTarget->getY(), iDX, iDY, iRange);
			if (!pPlot)
				continue;

			bool bSafeForDeployment = true;
			bool bForcedToUseWater = false;

			int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			CvTacticalAnalysisCell* pCell = GetTacticalAnalysisMap()->GetCell(iPlotIndex);
			CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());

			bool bValid = false;
			if(pOperation->IsNavalOperation() && pCell->CanUseForOperationGatheringCheckWater(true /*bWater*/))
			{
				bValid = true;
			}
			else if(!pOperation->IsNavalOperation() && (pCell->CanUseForOperationGatheringCheckWater(false /*bWater*/) || GC.getMap().GetAIMapHint() & ciMapHint_Naval))
			{
				bValid = true;
				if (pCell->IsWater())
				{
					bForcedToUseWater = true;
				}
			}

			if(pOperation->IsNavalOperation())
			{
				if(!pArmy->IsAllOceanGoing() && pCell->IsOcean())
				{
					bValid = false;
				}
			}

			if (!bValid)
				continue;

			// Skip this plot if friendly unit that isn't in this army
			CvUnit* pFriendlyUnit = pPlot->getBestDefender(pArmy->GetOwner());
			if(pFriendlyUnit)
			{
				if(pFriendlyUnit->getArmyID() != pArmy->GetID())
				{
					continue;
				}
			}

			if (pPlot->isCity())
			{
				continue;
			}

			if (bForcedToUseWater)
			{
				iScore = 100 - (iPlotDistance * 10);
			}
			else
			{
				iScore = 1000 - (iPlotDistance * 100);
			}

			//when in doubt, stay out of sight
			if(pCell->IsVisibleToEnemy())
				iScore -= 8;

			//avoid enemies right now
			if(pCell->IsSubjectToAttack())
				iScore -= 100;

			//be careful with ranged units here
			if (pCell->IsVisibleToEnemy() && pCell->IsSubjectToAttack())
				bSafeForDeployment = false;

			//todo: maybe avoid "slow" plot? but ending the turn there is usually good.
			//need to know a unit's previous position before making the call ...
			iScore += pPlot->defenseModifier(m_pPlayer->getTeam(),false,false) * 2;
	
			pCell->SetSafeForDeployment(bSafeForDeployment);
			pCell->SetDeploymentScore(iScore);

			// Save this in our list of potential targets
			target.SetTargetX(pPlot->getX());
			target.SetTargetY(pPlot->getY());
			target.SetAuxIntData(iScore);

			// A bit of a hack -- use high priority targets to indicate safe plots for ranged units
			if(bSafeForDeployment)
			{
				iNumSafePlotsFound++;
				iNumDeployPlotsFound++;
				target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
				pCell->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			}
			else
			{
				iNumDeployPlotsFound++;
				target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
				pCell->SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
			}

			m_TempTargets.push_back(target);
		}
	}

	// Make sure we found enough
	if(iNumRangedUnits > 0)
	{
		if(iNumSafePlotsFound < iNumRangedUnits && iNumDeployPlotsFound < (iNumMeleeUnits + iNumRangedUnits))
		{
			return false;
		}
	}
	else if(iNumDeployPlotsFound < iNumMeleeUnits)
	{
		return false;
	}

	return true;
}

void CvTacticalAI::ExecuteNavalFormationEscortMoves(CvArmyAI* pArmy, CvPlot* pTurnTarget)
{

	if (m_OperationUnits.size() == 0)
	{
		return;
	}

#if defined(MOD_BALANCE_CORE)
	//Let's make sure we aren't excluding units here - assume 2 moves per turn (with embarkation changes ...)
	int iMaxTurns = max(5, pArmy->GetFurthestUnitDistance(pTurnTarget) / 2 + 1);
#endif

	int iNavalUnits = 0;
	int iEscortedUnits = 0;
	std::map<int, ReachablePlots> unitMovePlots;
	for(opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
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

		// see where our units can go
		SPathFinderUserData data(pOpUnit, CvUnit::MOVEFLAG_IGNORE_STACKING, iMaxTurns);
		data.ePathType = PT_UNIT_REACHABLE_PLOTS;
		unitMovePlots[it->GetUnitID()] = GC.GetPathFinder().GetPlotsInReach(pOpUnit->plot(), data);
	}

	CvAIOperation* pOperation = m_pPlayer->getAIOperation(pArmy->GetOperationID());
	if (!pOperation)
		return;

	// Range around target based on number of units we need to place
	int iRange = pOperation->GetGatherTolerance(pArmy, pTurnTarget);

	// See if we have enough places to put everyone
	if (!ScoreDeploymentPlots(pTurnTarget, pArmy, iNavalUnits, iEscortedUnits, iRange) &&
		!ScoreDeploymentPlots(pTurnTarget, pArmy, iNavalUnits, iEscortedUnits, iRange + 1))
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy near turn target", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
	}
	// Compute moves forescorted units first
	else
	{
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();
		bool bDone = false;
		int iMostUnitsToPlace = iEscortedUnits;

		for (unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
			if (FindClosestNavalOperationUnit(pLoopPlot, unitMovePlots, true))
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
				iMostUnitsToPlace--;
				if (iMostUnitsToPlace == 0)
				{
					bDone = true;
				}
			}
		}

		// Now ready to make the assignments
		AssignDeployingUnits(iEscortedUnits - iMostUnitsToPlace);

		PerformChosenMoves();


		// Log if someone in army didn't get a move assigned (how do we address this in the future?)
		if (m_ChosenBlocks.size() < (unsigned int)iEscortedUnits)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("No naval deployment move for %d units in first pass", iEscortedUnits - m_ChosenBlocks.size());
				LogTacticalMessage(strMsg);
			}
		}

		if (iNavalUnits > 0)
		{
			// Now repeat for the naval escorts, using new target plots of where the embarked units ended up.
			m_TempTargets.clear();
			CvTacticalTarget temp;

			for (opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
			{
				CvUnit *pOpUnit = m_pPlayer->getUnit(it->GetUnitID());
				if (pOpUnit)
				{
					if (pOpUnit->getDomainType() == DOMAIN_LAND && pOpUnit->isEmbarked())
					{
						temp.SetTargetX(pOpUnit->plot()->getX());
						temp.SetTargetY(pOpUnit->plot()->getY());
						m_TempTargets.push_back(temp);
					}
				}
			}
			//Not enough plots? Add in our chosen plots from the above function.
			if (m_TempTargets.size() < (unsigned int)iNavalUnits)
			{
				for (unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
				{
					temp.SetTargetX(m_ChosenBlocks[iI].GetPlot()->getX());
					temp.SetTargetY(m_ChosenBlocks[iI].GetPlot()->getY());
					m_TempTargets.push_back(temp);
				}
			}

			m_PotentialBlocks.clear();
			bDone = false;
			int iLeastUnitsToPlace = iNavalUnits;

			for (unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if (FindClosestNavalOperationUnit(pLoopPlot, unitMovePlots, false))
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
					iLeastUnitsToPlace--;
					if (iLeastUnitsToPlace == 0)
					{
						bDone = true;
					}
				}
			}

			// Now ready to make the assignments
			AssignDeployingUnits(iNavalUnits - iLeastUnitsToPlace);
			PerformChosenMoves();

			if (m_ChosenBlocks.size() < (unsigned int)iNavalUnits)
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("No naval deployment move for %d units in second pass", iNavalUnits - m_ChosenBlocks.size());
					LogTacticalMessage(strMsg);
				}
			}
		}
	}
}
/// Complete moves for all units requested through calls to MoveWithFormation()
void CvTacticalAI::ExecuteNavalFormationMoves(CvArmyAI* pArmy, CvPlot* pTurnTarget)
{
	if(m_OperationUnits.size() == 0 || !pArmy)
	{
		return;
	}

	CvPlot* pCurrent = pArmy->Plot();
	if (!pCurrent)
		return;

	int iMeleeUnits = 0;
	int iRangedUnits = 0;
	std::map<int,ReachablePlots> unitMovePlots;
	for(opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
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

		// see where our units can go
		SPathFinderUserData data(pOpUnit,CvUnit::MOVEFLAG_IGNORE_STACKING,2);
		data.ePathType = PT_UNIT_REACHABLE_PLOTS;
		unitMovePlots[it->GetUnitID()] = GC.GetPathFinder().GetPlotsInReach(pOpUnit->plot(), data);
	}

#if defined(MOD_BALANCE_CORE)
	//the direction we are coming from
	DirectionTypes eFromDir = estimateDirection(pTurnTarget->getX(),pTurnTarget->getY(),pCurrent->getX(),pCurrent->getY());
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("Trying to move navy %d from (%d:%d), to (%d:%d)",pArmy->GetID(),pCurrent->getX(),pCurrent->getY(),pTurnTarget->getX(),pTurnTarget->getY());
		LogTacticalMessage(strMsg);
	}
#endif

	int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iMeleeUnits + iRangedUnits);
	if (pArmy->GetDomainType() == DOMAIN_SEA)
		iRange++;

	// See if we have enough places to put everyone
	if (!ScoreDeploymentPlots(pTurnTarget, pArmy, iMeleeUnits, iRangedUnits, iRange))
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Operation aborting. Army ID: %d. Not enough spaces to deploy along formation's path", pArmy->GetID());
			LogTacticalMessage(strLogString);
		}
		m_pPlayer->getAIOperation(pArmy->GetOperationID())->SetToAbort(AI_ABORT_NO_ROOM_DEPLOY);
		return;
	}

	// Compute the moves to get the best deployment
	std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());

	// First loop for melee units who should be out front
	int iMeleeUnitsToPlace = iMeleeUnits;
	bool bDone = false;
	if(iMeleeUnitsToPlace > 0)
	{
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

			// Don't use if there's already someone here
			if (!pLoopPlot->getBestDefender(NO_PLAYER))
			{
				//find a unit that's a little bit further back, and then try to move it to this good plot
				CvPlot* pIdealUnitPlot = plotDirection(pLoopPlot->getX(),pLoopPlot->getY(),eFromDir);
				if(FindClosestOperationUnit( pIdealUnitPlot, unitMovePlots, false /*bIncludeRanged*/, false /*bMustBeRangedUnit*/, false))
				{
					CvUnit* pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying melee unit (first pass) %d (%s), to %d,%d, from %d,%d", pInnerUnit->GetID(), pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
						LogTacticalMessage(strMsg);
					}
					ExecuteMoveToPlot(pInnerUnit, pLoopPlot);
					iMeleeUnitsToPlace--;
				}
			}
			if (iMeleeUnitsToPlace == 0)
			{
				bDone = true;
			}
		}
	}

	// Second loop for ranged units
	int iRangedUnitsToPlace = iRangedUnits;
	if(iRangedUnitsToPlace > 0)
	{
		bDone = false;
		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				// Don't use if there's already someone here
				if (!pLoopPlot->getBestDefender(NO_PLAYER))
				{
					//find a unit that's a little bit further back, and then try to move it to this good plot
					CvPlot* pIdealUnitPlot = plotDirection(pLoopPlot->getX(),pLoopPlot->getY(),eFromDir);
					if(FindClosestOperationUnit( pIdealUnitPlot, unitMovePlots, true /*bIncludeRanged*/, true /*bMustBeRangedUnit*/, false))
					{
						CvUnit* pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying ranged unit (first pass) %d (%s), to %d,%d, from %d,%d", pInnerUnit->GetID(), pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
							LogTacticalMessage(strMsg);
						}
						ExecuteMoveToPlot(pInnerUnit, pLoopPlot);
						iRangedUnitsToPlace--;
					}
				}
			}
			if (iRangedUnitsToPlace == 0)
			{
				bDone = true;
			}
		}
	}

	// Third loop for all units we couldn't put in an ideal spot
	for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());

		// Don't use if there's already someone here
		if (!pLoopPlot->getBestDefender(NO_PLAYER))
		{
			//find a unit that's a little bit further back, and then try to move it to this good plot
			CvPlot* pIdealUnitPlot = plotDirection(pLoopPlot->getX(),pLoopPlot->getY(),eFromDir);
			if(FindClosestOperationUnit( pIdealUnitPlot, unitMovePlots, true /*bIncludeRanged*/, false /*bMustBeRangedUnit*/, false))
			{
				CvUnit* pInnerUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Deploying third wave unit %d (%s), second pass, to %d,%d, from %d,%d", pInnerUnit->GetID(), pInnerUnit->getName().GetCString(), pLoopPlot->getX(), pLoopPlot->getY(), pInnerUnit->getX(), pInnerUnit->getY());
					LogTacticalMessage(strMsg);
				}

				ExecuteMoveToPlot(pInnerUnit, pLoopPlot);
				if(!pInnerUnit->isRanged())
					iMeleeUnitsToPlace--;
				else
					iRangedUnitsToPlace--;
			}
		}
#if defined(MOD_BALANCE_CORE)
		if (iRangedUnitsToPlace == 0 && iMeleeUnitsToPlace == 0)
#else
		if (iRangedUnitsToPlace == 0)
#endif
		{
			bDone = true;
		}
	}

	// Log if someone in army didn't get a move assigned
#if defined(MOD_BALANCE_CORE)
	if (iRangedUnitsToPlace > 0 || iMeleeUnitsToPlace > 0)
#else
	if (iRangedUnitsToPlace > 0)
#endif
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
#if defined(MOD_BALANCE_CORE)
			strMsg.Format("No army deployment move for %d ranged units, %d melee units.", iRangedUnitsToPlace, iMeleeUnitsToPlace);
#else
			strMsg.Format("No army deployment move for %d ranged units", iRangedUnitsToPlace);
#endif
			LogTacticalMessage(strMsg);
		}

		// Loop through all units available to operation and move them blindly
		for (opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
		{
			CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
			if (!pLoopUnit || !pLoopUnit->canMove())
				continue;

			MoveToEmptySpaceNearTarget(pLoopUnit,pTurnTarget,DOMAIN_SEA,12);
		}
	}
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

	//calculate this only once
	std::map<int,ReachablePlots> unitMovePlots;

	// Loop through each of our cities
	for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		// Compile a list of units that can attack it this turn and what their expected damage is
#if defined(MOD_BALANCE_CORE)
		if(pLoopCity == NULL)
			continue;
#endif
		possibleAttackers.clear();
		iExpectedTotalDamage = 0;

		CvTacticalTarget* pTarget = GetFirstUnitTarget();
		while(pTarget != NULL)
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if(pEnemyUnit)
			{
				iExpectedDamage = 0;

				if(pEnemyUnit->IsCanAttackRanged() && pEnemyUnit->canEverRangeStrikeAt(pLoopCity->getX(), pLoopCity->getY()))
				{
					//maybe take into account that ranged units can move? but should be okay
					iExpectedDamage = pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false, 0);
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
						int iAttackerStrength = pEnemyUnit->GetMaxAttackStrength(NULL, pLoopCity->plot(), NULL);
						int iDefenderStrength = pLoopCity->getStrengthValue();

						int iDefenderFireSupportCombatDamage = 0;
						CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pLoopCity->getOwner(), pLoopCity->getX(), pLoopCity->getY(), pEnemyUnit->getX(), pEnemyUnit->getY());
						if(pFireSupportUnit != NULL)
							iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pEnemyUnit, NULL, false);

						iExpectedDamage = pEnemyUnit->getCombatDamage(iAttackerStrength, iDefenderStrength, pEnemyUnit->getDamage() + iDefenderFireSupportCombatDamage, /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ true);
					}
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
					CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
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

	if (!m_pPlayer->isBarbarian())
	{
		pTarget = GetFirstUnitTarget();
		while(pTarget != NULL)
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if (pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			pTarget = GetNextUnitTarget();
		}
	}
	else
	{
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
						CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
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
						else if(pEnemyUnit->CanReachInXTurns(pLoopPlot, 1))
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
}

void CvTacticalAI::DumpTacticalTargets(const char* hint)
{
	CvTacticalTarget* pTarget = GetFirstUnitTarget();
	while(pTarget != NULL)
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

		CvUnit* pUnit = (CvUnit*)(pTarget->GetAuxData());
		CvString strMsg;
		strMsg.Format("Enemy unit (%s) at (%d,%d) with prio %s, score %d (%s with %d hp)", hint, pTarget->GetTargetX(), pTarget->GetTargetY(), 
			prio, pTarget->GetAuxIntData(), pUnit->getName().c_str(), pUnit->GetCurrHitPoints());
		LogTacticalMessage(strMsg);

		pTarget = GetNextUnitTarget();
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
			// Don't consider units that are already higher than low priority
			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				//units which are in the front line should be medium at least
				CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();
				if (pUnit->plot()->GetNumSpecificPlayerUnitsAdjacent(m_pPlayer->GetID())>0)
					m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
			}

			// Don't consider units that are already high priority
			if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
					m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
			{
				// Units defending forts will always be high priority targets
				CvUnit* pUnit = (CvUnit*)m_AllTargets[iI].GetAuxData();

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
	TacticalList::iterator it;
	for(it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
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
				//if the target is damage, give it a further boost
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
				//if the target is damage, give it a further boost
				it->SetAuxIntData( it->GetAuxIntData() + pUnit->getDamage()/10 );
			}
		}
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
			DomainTypes eDomain = pZone->IsWater() ? DOMAIN_SEA : DOMAIN_LAND;
			bValid = it->IsTargetValidInThisDomain(eDomain);
		}

		if(bValid)
		{
			if(pZone == NULL || it->GetDominanceZone() == pZone->GetZoneID())
			{
				m_ZoneTargets.push_back(*it);
			}

			// Not obviously in this zone, but if within 2 of city we want them anyway
			else
			{
				CvCity* pCity = pZone->GetZoneCity();
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
	CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());

		// Delete this unit from those we have to move
		if (!pUnit->canMove())
			UnitProcessed(pUnit->GetID());
	}
}

/// Capture an undefended civilian
void CvTacticalAI::ExecuteCivilianCapture(CvPlot* pTargetPlot)
{
	// Move first one to target
	CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY(), CvUnit::MOVEFLAG_NO_EMBARK);

		// Delete this unit from those we have to move
		if (!pUnit->canMove())
			UnitProcessed(m_CurrentMoveUnits[0].GetID());
	}
}

/// Pillage an undefended improvement
void CvTacticalAI::ExecutePillage(CvPlot* pTargetPlot)
{
	// Move first one to target
	CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
	if(pUnit)
	{
		if(pUnit->canMoveInto(*pTargetPlot, CvUnit::MOVEFLAG_DESTINATION ))
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
			pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
		}
		else
			MoveToEmptySpaceNearTarget(pUnit,pTargetPlot,DOMAIN_LAND,23);

		// Delete this unit from those we have to move
		if (!pUnit->canMove())
			UnitProcessed(pUnit->GetID());
	}
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
				}
				iCount++;
			}

			if (pUnit->getNumAttacks() - pUnit->getNumAttacksMadeThisTurn() == 0)
				UnitProcessed(m_CurrentMoveUnits[iI].GetID(), false /*bMarkTacticalMap*/);

			if (bDone)
				break;
		}
	}
}

/// Queues up attacks on enemy units on or adjacent to army's desired center
CvPlot* CvTacticalAI::FindAirTargetNearTarget(CvUnit* pUnit, CvPlot* pTargetPlot)
{
	int iRange = pUnit->GetRange();
	int iBestValue = 0;
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
					iValue = pUnit->GetAirCombatDamage(pDefender, pCity, false) * 2 - iTargetDistance * 3;

					if (pCity != NULL)
						iValue -= pCity->GetAirStrikeDefenseDamage(pUnit, false) * 3;
					else if (pDefender != NULL)
						iValue -= pDefender->GetAirStrikeDefenseDamage(pUnit, false) * 3;

					if (GetProbableInterceptor(pTargetPlot) != NULL)
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
					UnitProcessed(m_CurrentAirSweepUnits[iI].GetID(), false /*bMarkTacticalMap*/);

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
#endif
}

#ifdef MOD_CORE_NEW_DEPLOYMENT_LOGIC
bool CvTacticalAI::ExecuteAttackWithUnits(CvPlot* pTargetPlot, eAggressionLevel eAggLvl)
{
	//first handle air units
	ExecuteAirSweep(pTargetPlot);
	ExecuteAirAttack(pTargetPlot);

	//did the air attack already kill the enemy?
	if (pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID(), NULL, true) == NULL && !pTargetPlot->isCity())
		return true;

	//evaluate many possible unit assignments around the target plot and choose the best one
	//will not necessarily attack only the target plot when other targets are present!

	//todos:
	//
	// - embarked units are ignored. too much hassle
	// - defensive variant (simulate possible enemy moves?)
	// - find out most dangerous enemy unit (cf IdentifyPriorityTargets)
	// - consider possible kills in unit position score (ie ranged unit doesn't need to move if it can kill the attacker)
	// - consider enemy units which would be revealed during combat for following danger calculations 
	// - track tile visibility changes to avoid parking ranged units next to invisible tiles

	vector<STacticalAssignment> vAssignments;
	vector<CvUnit*> vUnits;
	for (size_t i=0; i<m_CurrentMoveUnits.size(); i++)
		vUnits.push_back( m_CurrentMoveUnits.getUnit(i) );

	int iCount = 0;
	bool bSuccess = false;
	do
	{
		TacticalAIHelpers::FindBestAssignmentsForUnits(vUnits, pTargetPlot, eAggLvl, 3, 23, vAssignments);
		if (vAssignments.empty())
			break;
		
		//true if everything went according to plan. if not, repeat!
		bSuccess = TacticalAIHelpers::ExecuteUnitAssignments(m_pPlayer->GetID(), vAssignments);
		iCount++;
	}
	while (!bSuccess && iCount < 4);

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
					choices.push_back( SAssignment(pUnit,pEvalPlot,101,true) );
				}
			}
			else
			{
				//check danger
				int iScore = 100 - pUnit->GetDanger(pEvalPlot) + iBonus;
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
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
bool IsGoodPlotForStaging(CvPlayer* pPlayer, CvPlot* pCandidate, bool bWater)
{
	if (!pPlayer || !pCandidate)
		return false;

	//we want a plot that is
	//- not too close and not too far from our cities
	//- is not a route (don't block roads)
	//- doesn't have too many other units around (don't build impenetrable walls of units)

	if (pCandidate->getNumUnits()>0)
		return false;

	int iCityDistance = pPlayer->GetCityDistanceInEstimatedTurns(pCandidate);
	if (iCityDistance<2 || iCityDistance>3)
		return false;

	if (pCandidate->getRouteType()!=NO_ROUTE)
		return false;

	int iNeighboringUnits = pCandidate->GetNumSpecificPlayerUnitsAdjacent(pPlayer->GetID(),NULL,NULL,true);
	if (iNeighboringUnits>2)
		return false;

	if (pCandidate->countPassableNeighbors(bWater?DOMAIN_SEA:DOMAIN_LAND)<3)
		return false;

	return true;
}
#endif

/// Execute moving units to a better location
void CvTacticalAI::ExecuteRepositionMoves()
{
	CvPlot* pBestPlot = NULL;
	CvString strTemp;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			strTemp = pUnit->getUnitInfo().GetDescription();

			// LAND MOVES
			if(pUnit->getDomainType() == DOMAIN_LAND)
			{
				//defensive only - don't send lonesome units into danger
				pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange*2, AI_TACTICAL_TARGET_CITY_TO_DEFEND);
				if (!pBestPlot)
					pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange, AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
				if (!pBestPlot)
					pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange, AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);

				if(pBestPlot)
				{
					if(MoveToEmptySpaceNearTarget(pUnit, pBestPlot, pUnit->getDomainType(), 12))
					{
						UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());
						
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s moving to empty space near defensive target (RepositionMoves), X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
							                    pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}
				//do not automatically end the turn ... unit might be useful for homeland. at least homeland patrol move will apply and find a nice spot for it
			}
#if defined(MOD_BALANCE_CORE_MILITARY)
			else if (pUnit->getDomainType() == DOMAIN_SEA)
			{
				bool bMoveMade = false;

				//defensive only - don't send lonesome units into danger
				pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange*2, AI_TACTICAL_TARGET_CITY_TO_DEFEND);
				if (!pBestPlot)
					pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange, AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND);
				if (!pBestPlot)
					pBestPlot = FindNearbyTarget(pUnit, m_iRepositionRange, AI_TACTICAL_TARGET_DEFENSIVE_BASTION);

				if(pBestPlot)
				{
					if(MoveToEmptySpaceNearTarget(pUnit, pBestPlot, DOMAIN_SEA, 12))
					{
						bMoveMade = true;

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s moving to empty space near defensive naval target (RepositionMoves), X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
												pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
					}
				}

				//stay put if we're in a good place already
				if (IsGoodPlotForStaging(m_pPlayer,pUnit->plot(),pUnit->getDomainType()==DOMAIN_SEA))
					bMoveMade=true;

				if (!bMoveMade)
				{
					ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
					for (ReachablePlots::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
					{
						CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

						if ( IsGoodPlotForStaging(m_pPlayer,pLoopPlot,pUnit->getDomainType()==DOMAIN_SEA) )
						{
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlot->getX(), pLoopPlot->getY(), 0, false, false, MISSIONAI_BUILD);
							TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);
							bMoveMade = true;

							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("%s moving to empty space to avoid being in the way (RepositionMoves), X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
													pLoopPlot->getX(), pLoopPlot->getY(), pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strLogString);
							}
							break;
						}
					}

					//nothing found in neighborhood? move to closest city
					if (!bMoveMade)
					{
						CvCity* pTargetCity = m_pPlayer->GetClosestCity(pUnit->plot(),INT_MAX,true);
						if (pTargetCity)
						{
							bMoveMade = MoveToEmptySpaceNearTarget(pUnit, pTargetCity->plot(), DOMAIN_SEA, 12);

							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("%s cannot move to empty space to avoid being in the way,  moving to city instead (RepositionMoves), X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(),
									pTargetCity->getX(), pTargetCity->getY(), pUnit->getX(), pUnit->getY());
								LogTacticalMessage(strLogString);
							}
						}
					}
				
				}
				if(bMoveMade)
				{
					//the new homeland patrol moves can handle naval units also, but the simple logic above seems good enough, so end their turn
					pUnit->PushMission(CvTypes::getMISSION_SKIP());
					UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());
				}
			}
#endif
		}
	}
}

/// Moves units to the hex with the lowest danger
void CvTacticalAI::ExecuteMovesToSafestPlot()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit && pUnit->canMove())
		{
			//see if we can do damage before retreating
			if (pUnit->canMoveAfterAttacking() && pUnit->getMoves()>1 && pUnit->canRangeStrike())
				TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);

			//so easy
			CvPlot* pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit,true,false,true);
			if(pBestPlot != NULL)
			{
				CvUnit* pSwapUnit = pUnit->GetPotentialUnitToSwapWith(*pBestPlot);
				//melee units are there to soak damage ...
				int iDangerLimit = pSwapUnit ? (pSwapUnit->isRanged() ? pSwapUnit->GetCurrHitPoints() : (3 * pSwapUnit->GetCurrHitPoints()) / 2) : 0;
				if (pSwapUnit && pSwapUnit->GetDanger(pUnit->plot()) < iDangerLimit)
				{
					pSwapUnit->SetActivityType(ACTIVITY_AWAKE);
					pUnit->PushMission(CvTypes::getMISSION_SWAP_UNITS(), pBestPlot->getX(), pBestPlot->getY());
				}
				else
				{
					//pillage before retreat, if we have movement points to spare
					if (pBestPlot->isAdjacent(pUnit->plot()) && pUnit->getMoves() > GC.getMOVE_DENOMINATOR())
						if (pUnit->canPillage(pUnit->plot()) && pUnit->getDamage() > 0)
							pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

					//try to do some damage if we have movement points to spare
					if (pBestPlot->isAdjacent(pUnit->plot()) && pUnit->getMoves() > GC.getMOVE_DENOMINATOR() && pUnit->canRangeStrike())
						TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);

					// Move to the lowest danger value found
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_TACTMOVE);

					//see if we can do damage after retreating
					if (pUnit->canMove() && pUnit->canRangeStrike())
						TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);

					//pillage after retreat, if we have movement points to spare
					if (pUnit->canMove() && pUnit->canPillage(pUnit->plot()) && pUnit->getDamage() > 0)
						pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
				}

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

#if defined(MOD_BALANCE_CORE)
				//not in an army and not at home? we really need to do something
				if(pUnit->getArmyID() == -1 && pUnit->plot()->getOwner() != pUnit->getOwner())
				{
					CvCity* pClosestCity = m_pPlayer->GetClosestCityByEstimatedTurns(pUnit->plot());
					CvPlot* pMovePlot = pClosestCity ? pClosestCity->plot() : NULL;
					if(pMovePlot != NULL)
					{
						MoveToEmptySpaceNearTarget(pUnit,pMovePlot,DOMAIN_LAND,42);
						pUnit->SetTurnProcessed(true);
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
						pUnit->SetTurnProcessed(true);
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
void CvTacticalAI::ExecuteHeals()
{
	std::vector<int> killedUnits;
	for (std::set<int>::iterator it=m_HealingUnits.begin(); it!=m_HealingUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit( *it );
		CvPlot* pBetterPlot = NULL;
		if(!pUnit)
		{
			//unit does not seem to exist anymore
			killedUnits.push_back(*it);
			continue;
		}

		//need to split from army?
		if (pUnit->getArmyID() != -1)
		{
			CvArmyAI* pArmy = m_pPlayer->getArmyAI(pUnit->getArmyID());
			if (pArmy && pArmy->GetArmyAIState() != ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
			{
				pArmy->RemoveUnit(pUnit->GetID());
			}
		}

		//ranged attack before fleeing
		if (pUnit->canMoveAfterAttacking() && pUnit->getMoves()>1 && pUnit->canRangeStrike())
			TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);

		//find a suitable spot for healing
		if (pUnit->getDomainType() == DOMAIN_LAND)
		{
			//why not pillage some tiles?
			if (pUnit->canPillage(pUnit->plot()))
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

			//land unit on land should move away from danger
			if (pUnit->GetDanger() > 0 && !pUnit->isEmbarked() && !pUnit->IsGarrisoned())
			{
				//unless we can eliminate the danger!
				std::vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pUnit->plot());
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
				else //flee
				{
					pBetterPlot = TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
				}
			}
			else
			{
				pBetterPlot = TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
			}
		}
		else if (pUnit->getDomainType()==DOMAIN_SEA)
		{
			if (pUnit->GetDanger()>0 || !pUnit->canHeal(pUnit->plot()))
			{
				std::vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pUnit->plot());
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
				else //flee
				{
					pBetterPlot = TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
				}
			}
		}

		//now finally do something
		if (pBetterPlot && pBetterPlot != pUnit->plot())
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
		else if (!pBetterPlot && pUnit->getDomainType()!=DOMAIN_AIR)
		{
			//apparently no chance to heal? try again
			pBetterPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit,true);
			if (pBetterPlot && pBetterPlot != pUnit->plot())
				ExecuteMoveToPlot(pUnit, pBetterPlot);
		}

		//why not pillage more tiles?
		if (pUnit->canMove() && pUnit->canPillage(pUnit->plot()))
			pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

		//finish this up
		if (pUnit->canHeal(pUnit->plot()))
			pUnit->PushMission(CvTypes::getMISSION_HEAL());
		else if (pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
		}
		else
			pUnit->PushMission(CvTypes::getMISSION_SKIP());

		pUnit->setTacticalMove((TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_HEAL]);
		UnitProcessed( *it );
	}

	//erase the zombies
	for (std::vector<int>::iterator i2=killedUnits.begin(); i2!=killedUnits.end(); ++i2)
		m_HealingUnits.erase(*i2);
}

/// Move barbarian to a new location
void CvTacticalAI::ExecuteBarbarianMoves(bool bAggressive)
{
	CvPlot* pBestPlot = NULL;
	CvString strTemp;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->isBarbarian())
			{
				strTemp = pUnit->getUnitInfo().GetDescription();

				// LAND MOVES
				if(pUnit->getDomainType() == DOMAIN_LAND)
				{
					AI_PERF_FORMAT("AI-perf-tact.csv", ("Barb Land Move, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

#if defined(MOD_BALANCE_CORE)
					CvPlot* pPlot = pUnit->plot();
					if(pPlot && (pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT() || pPlot->isCity()))
					{
						pUnit->setTacticalMove((TacticalAIMoveTypes)AI_TACTICAL_BARBARIAN_CAMP_DEFENSE);
						if (pUnit->canFortify(pPlot))
						{
							pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
							pUnit->SetFortifiedThisTurn(true);
						}
						else
							pUnit->PushMission(CvTypes::getMISSION_SKIP());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Pinning %s to camp/city at X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
							LogTacticalMessage(strLogString);
						}
						continue;
					}
#endif

#if defined(AUI_TACTICAL_EXECUTE_BARBARIAN_MOVES_CIVILIANS_MOVE_PASSIVELY)
					if(bAggressive && pUnit->IsCanDefend() && pUnit->GetCurrHitPoints()>pUnit->GetDanger())
#else
					if(bAggressive)
#endif
					{
						pBestPlot = FindBestBarbarianLandMove(pUnit);
					}
					else
					{
						pBestPlot = FindPassiveBarbarianLandMove(pUnit);
					}

					if(pBestPlot && MoveToEmptySpaceNearTarget(pUnit,pBestPlot,DOMAIN_LAND,12))
					{
#if defined(MOD_BALANCE_CORE)
						if(pUnit->getMoves() > 0 && pUnit->canPillage(pUnit->plot()))
						{
							pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strMsg;
								strMsg.Format("Pillaging during a barbarian move order. %s, X: %d, Y: %d", pUnit->getName().GetCString(),
									pUnit->plot()->getX(), pUnit->plot()->getY());
								LogTacticalMessage(strMsg, false);
							}
						}
#endif
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

					pBestPlot = FindBestBarbarianSeaMove(pUnit);

					if(pBestPlot && MoveToEmptySpaceNearTarget(pUnit,pBestPlot,DOMAIN_SEA,12))
					{
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
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());

						if(GC.getLogging() && GC.getAILogging())
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
	CvUnit* pCivilian = NULL;
	CvUnit* pEscort = NULL;
	CvPlot* pTarget = NULL;
	CvPlot* pCurrent = NULL;
	CvPlot* pCivilianMove = NULL;
	CvPlot* pEscortMove = NULL;
	CvUnit* pLoopUnit = NULL;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		pCivilian = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pCivilian)
		{
			//for the barbarian player AI_TACTICAL_TARGET_BARBARIAN_CAMP does not automatically mean the camp is empty of _barbarian_ defenders (check is only for enemy units)
			pTarget = FindNearbyTarget(pCivilian, 23, AI_TACTICAL_TARGET_BARBARIAN_CAMP);
			if(pTarget)
			{
				// If we're not there yet, we have work to do
				pCurrent = pCivilian->plot();
				if(pCurrent == pTarget)
				{
					UnitProcessed(pCivilian->GetID());
				}
				else
				{
#if defined(MOD_BALANCE_CORE)
					int iLoop;
					int iBestDistance = 4;
					ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();
					for(pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
					{
						if(!pLoopUnit || pLoopUnit->IsCivilianUnit() || pLoopUnit->plot()->getArea()!=pCivilian->plot()->getArea())
							continue;

						if(pLoopUnit->getDomainType() == pCivilian->getDomainType() || pLoopUnit->getArea() != pCivilian->getArea())
							continue;

						if(pLoopUnit->plot()->getImprovementType()==eCamp)
							continue;

						if(pLoopUnit->plot() == pCivilian->plot())
						{
							pEscort = pLoopUnit;
							break;
						}
						else
						{
							int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pCivilian->getX(), pCivilian->getY());
							if(iDistance < iBestDistance)
							{
								pEscort = pLoopUnit;
								iBestDistance = iDistance;
							}
						}
					}
#else

					if(pCurrent->getNumUnits() > 1)
					{
						for(int iJ = 0; iJ < pCurrent->getNumUnits(); iJ++)
						{
							pLoopUnit = pCurrent->getUnitByIndex(iJ);
							if(pLoopUnit->GetID() != pCivilian->GetID() &&
							        pLoopUnit->getOwner() == pCivilian->getOwner())
							{
								pEscort = CvUnit*(pLoopUnit);
								break;
							}
						}
					}
#endif

					// Handle case of no path found at all for civilian
					if(!pCivilian->GeneratePath(pTarget, 0, INT_MAX, NULL, true))
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Civilian cannot reach target, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
							LogTacticalMessage(strLogString);
						}
						return;
					}
					else
					{
						pCivilianMove = pCivilian->GetPathEndFirstTurnPlot();

						// Can we reach our target this turn?
						if (pCivilianMove == pTarget)
						{
							// See which defender is stronger
							CvUnit* pCampDefender = pCivilianMove->getBestDefender(m_pPlayer->GetID());
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
						else if (!pEscort)
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
						else if (pCivilianMove)
						{
							// See if escort can move to the same location in one turn
							if(pEscort->TurnsToReachTarget(pCivilianMove) <= 1)
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
								CvUnit* pBlockingUnit = pCivilianMove->getBestDefender(m_pPlayer->GetID());

								// See if friendly blocking unit is ending the turn there, or if no blocking unit (which indicates this is somewhere civilian
								// can move that escort can't), then find a new path based on moving the escort
								if(!pBlockingUnit || pBlockingUnit->getMoves() == 0)
								{
									if(!pEscort->GeneratePath(pTarget))
									{
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strLogString;
											strLogString.Format("Escort cannot move with civilian, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
											LogTacticalMessage(strLogString);
										}
										return;
									}
									else
									{
										pEscortMove = pCivilian->GetPathEndFirstTurnPlot();

										// See if civilian can move to the same location in one turn
										if(pCivilian->TurnsToReachTarget(pEscortMove) <= 1)
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
											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strLogString;
												strLogString.Format("Civilian cannot move with escort. Too many blocking units.");
												LogTacticalMessage(strLogString);
											}
											return;
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
										if(GC.getLogging() && GC.getAILogging())
										{
											CvString strLogString;
											strLogString.Format("Could not move blocking unit for escorted civilian.");
											LogTacticalMessage(strLogString);
										}
										return;
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

/// Move unit to a specific tile (unit passed explicitly)
bool CvTacticalAI::ExecuteMoveToPlot(CvUnit* pUnit, CvPlot* pTarget, bool bSaveMoves, int iFlags)
{
	bool bResult = false;

	if(!pUnit || !pTarget)
		return false;

	// Unit already at target plot?
	if(pTarget == pUnit->plot())
	{
		bResult = true;

		// Fortify if possible
		if (!TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit))
		{
			if( pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
		}
		else
			pUnit->PushMission(CvTypes::getMISSION_SKIP());

		//don't call finish moves, otherwise we won't heal!
	}
	else if (pUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION) || (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING1) || (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING2))
	{
		//don't attack accidentally
		iFlags |= CvUnit::MOVEFLAG_NO_ATTACKING;

		if (pUnit->GeneratePath(pTarget,iFlags))
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), iFlags, false, false, MISSIONAI_TACTMOVE, pTarget);
			bResult = true;

			//for inspection in GUI
			pUnit->SetMissionAI(MISSIONAI_TACTMOVE,pTarget,NULL);
		}

		if(!bSaveMoves && bResult)
			TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit);
	}

	if (bResult)
		UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());

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
	for (unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (pUnit)
		{
			TacticalAIHelpers::PerformOpportunityAttack(pUnit,true);

			//safety check
			if (pUnit->GetDanger() > pUnit->GetCurrHitPoints())
				continue;

			//don't get killed on the way
			if (!pUnit->GeneratePath(pTarget, CvUnit::MOVEFLAG_APPROX_TARGET_RING1, INT_MAX, NULL, true))
				continue;

			if (pUnit->GetDanger(pUnit->GetPathEndFirstTurnPlot()) > pUnit->GetCurrHitPoints())
				continue;

			if (pUnit->canMove())
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

			if (pUnit->canMove())
				TacticalAIHelpers::PerformOpportunityAttack(pUnit,true);

			if (pUnit->canMove() && pUnit->canPillage(pUnit->plot()))
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

			UnitProcessed(m_CurrentMoveUnits[0].GetID());
			pUnit->SetTacticalAIPlot(NULL);

			return;
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
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirSweep())
			{
				CvPlot *pTarget = m_pPlayer->GetMilitaryAI()->GetBestAirSweepTarget(pUnit);
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
				
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					CvString strTemp = pCity->getName();
					strMsg.Format("Made ranged attack with %s, at X: %d, Y: %d, vs. target at X: %d, Y: %d", strTemp.GetCString(), pCity->getX(), pCity->getY(), pDefender->getX(), pDefender->getY());
					LogTacticalMessage(strMsg);
				}

				pCity->doTask(TASK_RANGED_ATTACK, pDefender->getX(), pDefender->getY(), 0);

				// Need to keep hitting the target?
				if (pDefender->GetCurrHitPoints() < 1)
					return true;
			}
		}

		//special handling for ranged garrison as it is ignored by FindUnitsWithinStrikingDistance()
		//note: melee garrison is handled in PlotGarrisonMoves()
		for(std::list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
			if(pUnit->IsGarrisoned() && pUnit->canRangeStrikeAt(kTarget.GetTargetX(), kTarget.GetTargetY()))
			{
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), kTarget.GetTargetX(), kTarget.GetTargetY());

				//this invalidates our iterator so exit the loop immediately
				UnitProcessed(pUnit->GetID());
				break;
			}
		}

		// Need to keep hitting the target?
		if(pDefender->GetCurrHitPoints() <= 0)
			return true;

		// Make attacks - this includes melee attacks but only very safe ones
		if(FindUnitsWithinStrikingDistance(pTargetPlot))
			ExecuteAttackWithUnits(pTargetPlot, AL_LOW);
	}

	return true;
}

/// Take a multi-hex attack on an enemy unit this turn
bool CvTacticalAI::ExecuteFlankAttack(CvTacticalTarget& kTarget)
{
#ifdef MOD_CORE_NEW_DEPLOYMENT_LOGIC
	// Make attacks
	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	CvUnit* pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if(pDefender && FindUnitsWithinStrikingDistance(pTargetPlot))
		ExecuteAttackWithUnits(pTargetPlot, AL_MEDIUM);
#endif
	return false;
}

/// Move forces in toward our target
void CvTacticalAI::ExecuteCloseOnTarget(CvTacticalTarget& kTarget, CvTacticalDominanceZone* pZone, bool bOffensive)
{
	int iTacticalRadius = GetTacticalAnalysisMap()->GetTacticalRange();
	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());

	//cities have two zones ...
	CvTacticalDominanceZone* pTargetZone = NULL, *pTargetZoneWater = NULL;
	if (pTargetPlot->isCity())
	{
		pTargetZone = GetTacticalAnalysisMap()->GetZoneByCity(pTargetPlot->getPlotCity(),false);
		pTargetZoneWater = GetTacticalAnalysisMap()->GetZoneByCity(pTargetPlot->getPlotCity(),true);

		//failsafe 
		if (!pTargetZone)
			pTargetZone = pTargetZoneWater;
	}
	else
		pTargetZone = GetTacticalAnalysisMap()->GetZoneByPlot(pTargetPlot);

	m_OperationUnits.clear();
	m_GeneralsToMove.clear();

	if (!pTargetZone)
		return;

	vector<CvOperationUnit> landMelee, landRanged, navalMelee, navalRanged;
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->IsGarrisoned() && !pUnit->isDelayedDeath() && pUnit->canMove())
		{
			// Proper domain of unit?
			if ((pTargetZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA) ||
				(!pTargetZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND) ||
				(pTargetZoneWater && pUnit->getDomainType() == DOMAIN_SEA) ||
				(pTargetZoneWater && pTargetZoneWater->IsNavalInvasion()) ||
				(pZone && pZone->IsNavalInvasion()) ) //might be temporary zone, so check separately!
			{	
				// Find units really close to target or somewhat close that just came out of an operation
				int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), kTarget.GetTargetX(), kTarget.GetTargetY());
				if(iDistance <= iTacticalRadius || (iDistance <= iTacticalRadius*2 && pUnit->IsRecentlyDeployedFromOperation()))
				{
					//don't run away if there's work to do (will eventually be handled by ExecuteAttackWithUnits)
					if (pUnit->plot()->GetNumEnemyUnitsAdjacent(pUnit->getTeam(),pUnit->getDomainType()) > 1)
						continue;

					CvOperationUnit unit;
					unit.SetUnitID(pUnit->GetID());

					if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral() || pUnit->IsCityAttackSupport())
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_CIVILIAN_SUPPORT]);
						m_GeneralsToMove.push_back(unit);
					}
					else if(pUnit->IsCanAttackRanged() && pUnit->GetRange() > 1)
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_BOMBARD]);
						if (pUnit->getDomainType()==DOMAIN_SEA)
							navalRanged.push_back(unit);
						else
							landRanged.push_back(unit);
					}
					else
					{
						unit.SetPosition((MultiunitPositionTypes)m_CachedInfoTypes[eMUPOSITION_FRONT_LINE]);
						if (pUnit->getDomainType() == DOMAIN_SEA)
							navalMelee.push_back(unit);
						else
							landMelee.push_back(unit);
					}
				}
			}
		}
	}

	m_OperationUnits.insert(m_OperationUnits.end(), navalMelee.begin(), navalMelee.end());
	m_OperationUnits.insert(m_OperationUnits.end(), landMelee.begin(), landMelee.end());
	// only take ranged units if there are also melee units
	// todo: check whether there are already melee units present in the target area
	// note: melee also contains hit and run units ... but that should work out

	if (navalMelee.size() > 0 || (pTargetZone && pTargetZone->GetFriendlyMeleeStrength()>pTargetZone->GetFriendlyRangedStrength()))
		m_OperationUnits.insert(m_OperationUnits.end(), navalRanged.begin(), navalRanged.end());
	if (landMelee.size() > 0 || (pTargetZone && pTargetZone->GetFriendlyMeleeStrength()>pTargetZone->GetFriendlyRangedStrength()))
		m_OperationUnits.insert(m_OperationUnits.end(), landRanged.begin(), landRanged.end());

	// If have any units to move...
	if(m_OperationUnits.size() > 0)
	{
		// see where our units can go
		std::map<int,ReachablePlots> unitMovePlots;
		for(opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
		{
			CvUnit *pOpUnit = m_pPlayer->getUnit(it->GetUnitID());
			SPathFinderUserData data(pOpUnit,CvUnit::MOVEFLAG_IGNORE_STACKING,2);
			data.ePathType = PT_UNIT_REACHABLE_PLOTS;
			unitMovePlots[it->GetUnitID()] = GC.GetPathFinder().GetPlotsInReach(pOpUnit->plot(), data);
		}

		if (bOffensive)
			ScoreCloseOnPlots(pTargetPlot, unitMovePlots);
		else
			ScoreHedgehogPlots(pTargetPlot, unitMovePlots);

		// Compute the moves to get the best deployment
		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();

		// First loop for ranged unit spots
		int iUnitsToPlace = navalRanged.size() + landRanged.size();
		int iUnitsRemaining = iUnitsToPlace;
		bool bDone = (iUnitsRemaining==0);

		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			AITacticalTargetType eTargetType = m_TempTargets[iI].GetTargetType();
			if (eTargetType == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(FindClosestOperationUnit(pLoopPlot, unitMovePlots, true /*bIncludeRanged*/, true /*bMustBeRangedUnit*/, true))
				{
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());

						//try to prevent embarkation - unit can still embark to go somewhere else, but shouldn't actually target the plot
						CvUnit *pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[jJ].GetID());
						if (!pUnit->isNativeDomain(pLoopPlot))
							continue;

						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}

					iUnitsRemaining--;
					if(iUnitsRemaining == 0)
						bDone = true;
				}
			}
		}
		AssignDeployingUnits(iUnitsToPlace - iUnitsRemaining);
		PerformChosenMoves();

		// Second loop for everyone else (including remaining ranged units)
		if (bOffensive)
			ScoreCloseOnPlots(pTargetPlot, unitMovePlots);
		else
			ScoreHedgehogPlots(pTargetPlot, unitMovePlots);

		std::stable_sort(m_TempTargets.begin(), m_TempTargets.end());
		m_PotentialBlocks.clear();

		iUnitsToPlace = navalMelee.size() + landMelee.size() + iUnitsRemaining;
		iUnitsRemaining = iUnitsToPlace;
		bDone = (iUnitsRemaining==0);

		for(unsigned int iI = 0; iI < m_TempTargets.size() && !bDone; iI++)
		{
			//don't care about prio/safety now
			if (true)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(m_TempTargets[iI].GetTargetX(), m_TempTargets[iI].GetTargetY());
				if(FindClosestOperationUnit(pLoopPlot, unitMovePlots, true /*bIncludeRanged*/, false /*bMustBeRangedUnit*/, true))
				{
					for(unsigned int jJ = 0; jJ < m_CurrentMoveUnits.size(); jJ++)
					{
						CvBlockingUnit block;
						block.SetUnitID(m_CurrentMoveUnits[jJ].GetID());
						block.SetPlot(pLoopPlot);
						block.SetNumChoices((int)m_CurrentMoveUnits.size());

						//try to prevent embarkation - unit can still embark to go somewhere else, but shouldn't actually target the plot
						CvUnit *pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[jJ].GetID());
						if (!pUnit->isNativeDomain(pLoopPlot))
							continue;

						block.SetDistanceToTarget(m_CurrentMoveUnits[jJ].GetMovesToTarget());
						m_PotentialBlocks.push_back(block);
					}

					iUnitsRemaining--;
					if(iUnitsRemaining == 0)
						bDone = true;
				}
			}
		}

		AssignDeployingUnits(iUnitsToPlace-iUnitsRemaining);
		PerformChosenMoves();
	}

	if(m_GeneralsToMove.size() > 0)
	{
		MoveGreatGeneral();
	}
}

/// Move units out of current dominance zone
void CvTacticalAI::ExecuteWithdrawMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			bool bMoveMade = false;
			CvPlot* pTargetPlot = NULL;

			// Allow withdraw to neighboring tactical zone which seems safe
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot());
			if (!pZone)
				continue;

			for (std::vector<int>::const_iterator it = pZone->GetNeighboringZones().begin(); it != pZone->GetNeighboringZones().end(); ++it)
			{
				CvTacticalDominanceZone* pNextZone = GetTacticalAnalysisMap()->GetZoneByID(*it);
				if (pNextZone && pNextZone->GetZoneCity() && pNextZone->IsWater()==(pUnit->getDomainType()==DOMAIN_SEA) &&
					(pNextZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY || pNextZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_EVEN))
				{
					pTargetPlot = GC.getMap().plot(pNextZone->GetCenterX(), pNextZone->GetCenterY());
					if (pUnit->CanReachInXTurns(pTargetPlot,5))
					{
						bMoveMade = MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, pUnit->getDomainType(), 12);
						break;
					}
				}
			}

			if (!bMoveMade)
			{
				// Compute moves to nearest city and use as sort criteria
				CvCity* pNearestCity = m_pPlayer->GetClosestCityByEstimatedTurns(pUnit->plot());
				if (pNearestCity)
					pTargetPlot = pNearestCity->plot();

				if (pUnit->CanReachInXTurns(pTargetPlot, 5))
					bMoveMade = pUnit->IsCivilianUnit() ?
						ExecuteMoveToPlot(pUnit, pTargetPlot) :
						MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, pUnit->getDomainType(), 12);
			}

			if (bMoveMade)
			{
				UnitProcessed(m_CurrentMoveUnits[iI].GetID(), pUnit->IsCombatUnit());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s %d withdrew from (%d,%d) towards (%d,%d)", 
						pUnit->getName().GetCString(),pUnit->GetID(),pUnit->getX(),pUnit->getY(),pTargetPlot->getX(),pTargetPlot->getY());
					LogTacticalMessage(strLogString, false);
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
						LogTacticalMessage(strLogString, false);
					}
				}

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
			iCurrentAttack = pUnit->GetMaxRangedCombatStrength(pTargetUnit, pTargetCity, true, true, plotTarget, pMoveTile);
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
	list<int>::iterator it;
	CvUnit* pLoopUnit;
	m_CurrentAirSweepUnits.clear();
	int interceptionsOnPlot = m_pPlayer->GetMilitaryAI()->GetMaxPossibleInterceptions(pTarget, true);

	// Loop through all units available to tactical AI this turn
	for (it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end() && interceptionsOnPlot > 0; ++it)
	{
		pLoopUnit = m_pPlayer->getUnit(*it);

		if (pLoopUnit)
		{
			// Is an air unit.
			if (pLoopUnit->getDomainType() == DOMAIN_AIR && pLoopUnit->canMove())
			{
				// Is able to sweep at target
				if (pLoopUnit->canAirSweepAt(pTarget->getX(), pTarget->getY()))
				{

					int iAttackStrength = pLoopUnit->GetMaxAttackStrength(pLoopUnit->plot(), pTarget, GetProbableInterceptor(pTarget));
					// Mod to air sweep strength
					iAttackStrength *= (100 + pLoopUnit->GetAirSweepCombatModifier());
					iAttackStrength /= 100;
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetAttackStrength(iAttackStrength);
					unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
					m_CurrentAirSweepUnits.push_back(unit);

					interceptionsOnPlot--;

					// We also remove the unit from m_CurrentMoveUnits
					std::vector<CvTacticalUnit>::iterator itM;

					for (itM = m_CurrentMoveUnits.begin(); itM != m_CurrentMoveUnits.end(); itM++)
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

	std::stable_sort(m_CurrentAirSweepUnits.begin(), m_CurrentAirSweepUnits.end());
}

CvUnit* CvTacticalAI::GetProbableInterceptor(CvPlot* pTargetPlot) const
{
	int iLoopUnit;
	CvUnit* pBestUnit = NULL;
	CvUnit* pLoopUnit;

	// Loop through all the players
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kPlayer.isAlive() && kPlayer.GetID() != m_pPlayer->GetID())
		{
			if (atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
			{
				// Loop through their units looking for intercept capable units
				iLoopUnit = 0;
				for (pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
				{
					// Must be able to intercept this turn
					if (!pLoopUnit->isDelayedDeath() && pLoopUnit->canAirDefend() && !pLoopUnit->isInCombat() && !pLoopUnit->isOutOfInterceptions())
					{
						// Must either be a non-air Unit, or an air Unit that hasn't moved this turn and is on intercept duty
						if ((pLoopUnit->getDomainType() != DOMAIN_AIR) || !(pLoopUnit->hasMoved() && pLoopUnit->GetActivityType() == ACTIVITY_INTERCEPT))
						{
							// Test range
							if (plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= (pLoopUnit->getUnitInfo().GetAirInterceptRange() + pLoopUnit->GetExtraAirInterceptRange())) // JJ: Added consideration for extra intercept range from promotions
							{
								if (pLoopUnit->currInterceptionProbability() > 0)
								{
									if (pBestUnit)
									{
										if (pLoopUnit->currInterceptionProbability() > pBestUnit->currInterceptionProbability())
										{
											pBestUnit = pLoopUnit;
										}
										else if (pLoopUnit->currInterceptionProbability() == pBestUnit->currInterceptionProbability() &&
											pLoopUnit->GetBaseCombatStrengthConsideringDamage() > pBestUnit->GetBaseCombatStrengthConsideringDamage())
										{
											pBestUnit = pLoopUnit;
										}
									}
									else
									{
										pBestUnit = pLoopUnit;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}
#endif

/// Finds both high and normal priority units we can use for this move (returns true if at least 1 unit found)
bool CvTacticalAI::FindUnitsForThisMove(TacticalAIMoveTypes eMove, CvPlot* pTarget, int iNumTurnsAway /* = -1 if any distance okay */, bool bRangedOnly)
{
	bool rtnValue = false;

	list<int>::iterator it;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->getDomainType() != DOMAIN_AIR && pLoopUnit->IsCombatUnit())
		{
			// Make sure domain matches
			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
			        pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				continue;
			}

#if defined(MOD_BALANCE_CORE)
			if(!pLoopUnit->canMove() || !pLoopUnit->IsCanAttack() || pLoopUnit->GetDanger(pTarget)>pLoopUnit->GetCurrHitPoints())
			{
				continue;
			}
#endif

			bool bSuitableUnit = false;
			bool bHighPriority = false;

			if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GARRISON_1_TURN])
			{
				// Want to put ranged units in cities to give them a ranged attack (but siege units should be used for offense)
				if (pLoopUnit->isRanged() && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_CITY_BOMBARD)==false)
					bHighPriority = true;
				else if(bRangedOnly)
					continue;

				//Let's not pull out garrisons to do this.
				if(pLoopUnit->IsGarrisoned() || pLoopUnit->AI_getUnitAIType()==UNITAI_EXPLORE)
					continue;

				if(pLoopUnit->IsHurt())
					bHighPriority = true;

				// Don't put units with a defense boosted from promotions in cities, these boosts are ignored
				if(pLoopUnit->getDefenseModifier() == 0)
					bSuitableUnit = true;
			}

			else if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_GUARD_IMPROVEMENT_1_TURN] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_ALREADY_THERE] ||
			        eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_1_TURN] ||
					eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_BASTION_2_TURN])
			{
				// No ranged units or units without defensive bonuses as plot defenders
				if(!pLoopUnit->isRanged() && !pLoopUnit->noDefensiveBonus())
				{
					bSuitableUnit = true;

					// Units with defensive promotions are especially valuable
					if(pLoopUnit->getDefenseModifier() > 0 || pLoopUnit->getExtraRangedDefenseModifier() > 0)
					{
						bHighPriority = true;
					}
					if (pLoopUnit->getExtraVisibilityRange() > 0)
						bHighPriority = true;
				}

				//Let's not pull out garrisons to do this.
				if(pLoopUnit->IsGarrisoned() || pLoopUnit->AI_getUnitAIType()==UNITAI_EXPLORE)
					continue;
			}
			else if(eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_ANCIENT_RUINS])
			{
				// Fast movers are top priority
				if(pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
					bHighPriority = true;

				bSuitableUnit = true;
			}
#if defined(MOD_BALANCE_CORE)
			else if (eMove == (TacticalAIMoveTypes)m_CachedInfoTypes[eTACTICAL_POSTURE_SHORE_BOMBARDMENT])
			{
				if (pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit() && pLoopUnit->plot()->getOwner() != pTarget->getOwner())
				{
					if (pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_ASSAULT_SEA))
					{
						bHighPriority = true;
					}

					bSuitableUnit = true;
				}
			}
#endif

			if(bSuitableUnit)
			{
				int iMoves = pLoopUnit->TurnsToReachTarget(pTarget, false, false, (iNumTurnsAway == -1 ? MAX_INT : iNumTurnsAway));
				if(iMoves != MAX_INT && (iNumTurnsAway == -1 ||
						                    (iNumTurnsAway == 0 && pLoopUnit->plot() == pTarget) || iMoves <= iNumTurnsAway))
				{
					CvTacticalUnit unit;
					unit.SetID(pLoopUnit->GetID());
					unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
					unit.SetMovesToTarget(iMoves);
					unit.SetAttackStrength( bHighPriority ? 40-iMoves : 20-iMoves ); //high prio units should come first after sorting
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}

	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());
	return rtnValue;
}

/// Fills m_CurrentMoveUnits with all units within X turns of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindUnitsWithinStrikingDistance(CvPlot* pTarget, bool bNoRangedUnits, bool bImmediateStrike)
{
	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	bool bIsCityTarget = pTarget->isCity();
	bool bAirUnitsAdded = false;
	CvUnit* pDefender = pTarget->getBestDefender(NO_PLAYER, m_pPlayer->GetID());

	//todo: check if defender can be damaged at all or if an attacker would die?
	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(!pLoopUnit || pLoopUnit->isDelayedDeath())
			continue;

		//Don't grab civilians for this!
		if (pLoopUnit->IsCivilianUnit() && !pLoopUnit->IsGreatGeneral())
			continue;

		// To effectively skip all ranged units...
		if (bNoRangedUnits && pLoopUnit->IsCanAttackRanged())
			continue;

		//ignore most embarked units for attacking, that usually makes no sense
		bool bAmphibiousAttackPossible = pLoopUnit->plot()->isAdjacent(pTarget) && !pLoopUnit->isRanged() && pTarget->isCoastalLand();
		if(!pLoopUnit->canMove() || (pLoopUnit->isEmbarked() && !bAmphibiousAttackPossible) )
			continue;

		//Respect domain for melee units and subs
		if (!pLoopUnit->IsCanAttackRanged() || pLoopUnit->getUnitInfo().IsRangeAttackOnlyInDomain())
			if (pLoopUnit->getDomainType() != DOMAIN_AIR && !bIsCityTarget && pTarget->getDomain() != pLoopUnit->getDomainType())
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

		// Only time ranged units aren't selected for city attack is when it needs to be captured
		if (pLoopUnit->isNoCapture() && bIsCityTarget && bNoRangedUnits)
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
			if (pLoopUnit->canEverRangeStrikeAt(pTarget->getX(), pTarget->getY()))
				bCanReach = true;
			else if (pLoopUnit->canMove() && pLoopUnit->getDomainType()!=DOMAIN_AIR)
			{
				//note that we also take units which can reach an attack plot but can only attack next turn. that's ok.
				ReachablePlots reachablePlots = TacticalAIHelpers::GetAllPlotsInReach(pLoopUnit, pLoopUnit->plot(),
					CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_NO_EMBARK, bImmediateStrike ? 2 : 0);

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
			bCanReach = (pLoopUnit->TurnsToReachTarget(pTarget, iFlags, 2) < (bImmediateStrike?1:3));
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
				CvTacticalUnit unit;
				unit.SetID(pLoopUnit->GetID());
				if (bIsCityTarget)
					unit.SetAttackStrength(pLoopUnit->GetMaxRangedCombatStrength(NULL, pTarget->getPlotCity(), true, true));
				else
					unit.SetAttackStrength(pLoopUnit->GetMaxRangedCombatStrength(pDefender, NULL, true, true));

				unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
				m_CurrentMoveUnits.push_back(unit);
				rtnValue = true;

				if (pLoopUnit->getDomainType()==DOMAIN_AIR)
					bAirUnitsAdded = true;
			}
		}
		else //melee
		{
			int iAttackStrength = pLoopUnit->GetMaxAttackStrength(NULL, pTarget, bIsCityTarget ? NULL : pDefender);

			CvTacticalUnit unit;
			unit.SetID(pLoopUnit->GetID());
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
	if (m_CurrentMoveUnits.size()>0)
		OutputDebugString( CvString::format("player %d recruited %d units for possible attack on %d:%d (zone idx %d)\n",m_pPlayer->GetID(),m_CurrentMoveUnits.size(),pTarget->getX(),pTarget->getY(),m_iCurrentZoneIndex ).c_str() );
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
		if(pLoopCity->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()) && !pLoopCity->isMadeAttack())
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


bool CvTacticalAI::FindEmbarkedUnitsAroundTarget(CvPlot* pTarget, int iMaxDistance)
{
	if (!pTarget)
		return false;

	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->IsCombatUnit() && pLoopUnit->isEmbarked() && plotDistance(*pLoopUnit->plot(),*pTarget)<=iMaxDistance )
		{
			CvTacticalUnit unit;
			unit.SetID(pLoopUnit->GetID());
			unit.SetAttackStrength(pLoopUnit->GetBaseCombatStrengthConsideringDamage());
			unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
			m_CurrentMoveUnits.push_back(unit);
			rtnValue = true;
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}


/// Fills m_CurrentMoveUnits with all paratrooper units (available to jump) to the target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindParatroopersWithinStrikingDistance(CvPlot* pTarget)
{
	list<int>::iterator it;
	CvUnit* pLoopUnit;

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
#ifdef AUI_TACTICAL_FIX_FIND_PARATROOPER_WITHIN_STRIKING_DISTANCE_MELEE_STRENGTH
			unit.SetAttackStrength(pLoopUnit->GetMaxAttackStrength(NULL, pTarget, (pTarget->getPlotCity() != NULL ? NULL : pTarget->getBestDefender(NO_PLAYER, m_pPlayer->GetID()))));
#else
			unit.SetAttackStrength(pLoopUnit->GetMaxAttackStrength(NULL, NULL, NULL));
#endif // AUI_TACTICAL_FIX_FIND_PARATROOPER_WITHIN_STRIKING_DISTANCE_MELEE_STRENGTH
			unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
			m_CurrentMoveUnits.push_back(unit);
			rtnValue = true;
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

bool CvTacticalAI::FindUnitsCloseToPlot(CvPlot* pTarget, int iNumTurnsAway, int iMinHitpoints, int iMaxHitpoints, DomainTypes eDomain, bool bMustPillage)
{
	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->IsCombatUnit()) //ignore generals and the like!
		{
			if (bMustPillage && !pLoopUnit->canPillage(pTarget))
				continue;

			if (eDomain != NO_DOMAIN && pLoopUnit->getDomainType() != eDomain)
				continue;

			if(iMinHitpoints>0 && pLoopUnit->GetCurrHitPoints()<iMinHitpoints)
				continue;

			if(iMaxHitpoints>0 && pLoopUnit->GetCurrHitPoints()>iMaxHitpoints)
				continue;

			//performance optimization
			if (plotDistance(*pTarget, *pLoopUnit->plot())>(iNumTurnsAway + 1) * 3)
				continue;

			int iTurnsCalculated = -1;
			if (pLoopUnit->CanReachInXTurns(pTarget, iNumTurnsAway, false /*bIgnoreUnits*/, false /*bAllowEmbark*/, &iTurnsCalculated))
			{
				CvTacticalUnit unit;
				unit.SetID(pLoopUnit->GetID());

				//sorting is by the product of these two values
				if (bMustPillage)
				{
					unit.SetAttackStrength(3 - iTurnsCalculated);
					unit.SetHealthPercent(pLoopUnit->getDamage(),100); //fake it
				}
				else
				{
					unit.SetAttackStrength(10 - iTurnsCalculated);
					unit.SetHealthPercent(pLoopUnit->GetCurrHitPoints(), pLoopUnit->GetMaxHitPoints());
				}

				m_CurrentMoveUnits.push_back(unit);
				rtnValue = true;

				//won't get any better
				if (iTurnsCalculated==0 && pLoopUnit->getDamage()>=GC.getPILLAGE_HEAL_AMOUNT())
					break;
			}
		}
	}

	// Now sort them in the order we'd like them to attack
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
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

/// Fills m_CurrentMoveUnits with all units within X turns of a target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestUnit(CvPlot* pTarget, int iNumTurnsAway, bool bMustHaveHalfHP, 
	bool bMustBeRangedUnit, int iRangeRequired, bool bNeedsIgnoreLOS, bool bMustBeMeleeUnit, 
	bool bIgnoreUnits, CvPlot* pRangedAttackTarget, int iMaxUnits)
{
	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	if (pTarget == NULL)
		return false;

	//temporary container - don't add to m_CurrentMoveUnits directly as it would overwrite the unit's tactical move
	std::vector< std::pair<int, int> > vUnitsByDistance;

	// Loop through all units available to tactical AI this turn
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if (!pLoopUnit || !pLoopUnit->canMove())
			continue;

		// don't use non-combat units (but consider embarked for now)
		if (pLoopUnit->getUnitInfo().GetCombat() == 0)
			continue;
#if defined(MOD_UNITS_MAX_HP)
		if (bMustHaveHalfHP && (pLoopUnit->getDamage() * 2 > pLoopUnit->GetMaxHitPoints()))
#else
		if (bMustHaveHalfHP && (pLoopUnit->getDamage() * 2 > GC.getMAX_HIT_POINTS()))
#endif
			continue;
		if (bMustBeRangedUnit && ((pTarget->isWater() && pLoopUnit->getDomainType() == DOMAIN_LAND) ||
			(!pTarget->isWater() && !pTarget->isCoastalLand() && pLoopUnit->getDomainType() == DOMAIN_SEA)))
			continue;

		if (bMustBeRangedUnit && !pLoopUnit->IsCanAttackRanged())
			continue;

		if (bMustBeRangedUnit && pLoopUnit->GetRange() < iRangeRequired)
			continue;

		if (bMustBeRangedUnit && !pLoopUnit->canEverRangeStrikeAt(pRangedAttackTarget->getX(), pRangedAttackTarget->getY()))
			continue;

		if (bMustBeRangedUnit && pLoopUnit->isOutOfAttacks())
			continue;

		if (pRangedAttackTarget != NULL && bMustBeRangedUnit && !IsExpectedToDamageWithRangedAttack(pLoopUnit, pRangedAttackTarget))
			continue;

		if (bNeedsIgnoreLOS && !pLoopUnit->IsRangeAttackIgnoreLOS())
			continue;

		if (bMustBeMeleeUnit && pLoopUnit->IsCanAttackRanged())
			continue;

		if (bMustBeMeleeUnit && !pTarget->isCity() && pLoopUnit->IsCityAttackSupport())
			continue;

		//barbarians stay in camp ...
		if (GET_PLAYER(pLoopUnit->getOwner()).isBarbarian() && pLoopUnit->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			continue;

		int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTarget->getX(), pTarget->getY());
		if (iDistance > GetRecruitRange())
			continue;

		vUnitsByDistance.push_back(std::make_pair(iDistance, pLoopUnit->GetID()));
	}

	//default sort is by ascending first member of the pair, then by the second - important we don't store pointers here!
	std::stable_sort(vUnitsByDistance.begin(), vUnitsByDistance.end());

	for (std::vector< std::pair<int, int> >::iterator it = vUnitsByDistance.begin(); it != vUnitsByDistance.end(); ++it)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(it->second);

		int iTurns = pLoopUnit->TurnsToReachTarget(pTarget, bIgnoreUnits, (iNumTurnsAway == 0), iNumTurnsAway);
		if (iTurns <= iNumTurnsAway)
		{
			CvTacticalUnit unit;
			unit.SetID(pLoopUnit->GetID());
			unit.SetAttackStrength(1000 - iTurns);
			unit.SetHealthPercent(10, 10);
			unit.SetMovesToTarget(iTurns);
			m_CurrentMoveUnits.push_back(unit);
			rtnValue = true;

			if (m_CurrentMoveUnits.size() == iMaxUnits)
				break;
		}
	}

	return rtnValue;
}

/// Fills m_CurrentMoveUnits with all units in operation that can get to target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestOperationUnit(CvPlot* pTarget, const std::map<int,ReachablePlots>& movePlots, 
	bool bIncludeRanged, bool bRangedOnly, bool bCombatExpected)
{
	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	if (pTarget == NULL)
		return false;

	// Loop through all units available to operation
	for (opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		int iPenalty = 0;

		CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (!pLoopUnit || !pLoopUnit->canMove())
			continue;

		if (pLoopUnit->IsCanAttackRanged())
		{
			if (!bIncludeRanged)
				continue;

			//we don't want enemies close by
			if (pTarget->GetNumEnemyUnitsAdjacent(pLoopUnit->getTeam(), pLoopUnit->getDomainType(), NULL, false) > 0)
				iPenalty = 2;
		}
		else //melee
		{
			if (bRangedOnly)
				continue;

			//we want enemies close by
			if (pTarget->GetNumEnemyUnitsAdjacent(pLoopUnit->getTeam(),pLoopUnit->getDomainType())==0)
				iPenalty = 1;
		}
		
		//avoid embarkation if we need to fight!
		if (bCombatExpected && !pLoopUnit->isNativeDomain(pTarget))
			continue;

		std::map<int,ReachablePlots>::const_iterator itUnit = movePlots.find(pLoopUnit->GetID());
		if (itUnit!=movePlots.end())
		{
			std::map<int,ReachablePlots>::value_type::second_type::const_iterator itPlot = itUnit->second.find(pTarget->GetPlotIndex());
			if (itPlot!=itUnit->second.end())
			{
				CvTacticalUnit unit;
				unit.SetID(pLoopUnit->GetID());
				unit.SetAttackStrength(1000 - iPenalty - itPlot->iTurns);
				unit.SetHealthPercent(10, 10);
				unit.SetMovesToTarget(itPlot->iTurns);
				m_CurrentMoveUnits.push_back(unit);
				rtnValue = true;
			}
		}
	}

	// Now sort them by turns to reach
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Fills m_CurrentMoveUnits with all units in naval operation that can get to target (returns TRUE if 1 or more found)
bool CvTacticalAI::FindClosestNavalOperationUnit(CvPlot* pTarget, const std::map<int,ReachablePlots>& movePlots, bool bEscortedUnits)
{
	bool rtnValue = false;
	m_CurrentMoveUnits.clear();

	// Loop through all units available to operation
	std::vector< std::pair<int, int> > vUnitsByDistance;
	for(opUnitIt it = m_OperationUnits.begin(); it != m_OperationUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetUnitID());
		if (!pLoopUnit || !pLoopUnit->canMove())
			continue;

		if(bEscortedUnits && (!pLoopUnit->IsGreatAdmiral() && pLoopUnit->getDomainType() != DOMAIN_LAND))
			continue;

		if(!bEscortedUnits && (pLoopUnit->getDomainType() != DOMAIN_SEA || pLoopUnit->IsGreatAdmiral()))
			continue;

		std::map<int,ReachablePlots>::const_iterator itUnit = movePlots.find(pLoopUnit->GetID());
		if (itUnit!=movePlots.end())
		{
			std::map<int,ReachablePlots>::value_type::second_type::const_iterator itPlot = itUnit->second.find(pTarget->GetPlotIndex());
			if (itPlot!=itUnit->second.end())
			{
				int iTurns = itPlot->iTurns;

				CvTacticalUnit unit;
				unit.SetID(pLoopUnit->GetID());
				unit.SetAttackStrength(1000-iTurns);
				unit.SetHealthPercent(10,10);
				unit.SetMovesToTarget(iTurns);
				m_CurrentMoveUnits.push_back(unit);
				rtnValue = true;
			}
		}
	}

	// Now sort them by turns to reach
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

#if defined(MOD_AI_SMART_AIR_TACTICS)

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
			if(pDefender)
			{
#ifdef AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_AIR_UNITS
				if (pAttacker->getDomainType() == DOMAIN_AIR)
				{
					iExpectedDamage = pAttacker->GetAirCombatDamage(pDefender, NULL, false);
					iExpectedSelfDamage = pDefender->GetAirStrikeDefenseDamage(pAttacker, false);
				}
				else
#endif
				if(pAttacker->IsCanAttackRanged())
				{
					iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender, NULL, false);
					iExpectedSelfDamage = 0;
				}
				else
				{
#ifdef AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_MELEE
					int iAttackerStrength;
					if (plotDistance(pAttacker->getX(), pAttacker->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > 1)
					{
						iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, pDefender);
					}
					else
					{
						iAttackerStrength = pAttacker->GetMaxAttackStrength(pAttacker->plot(), pTargetPlot, pDefender);
					}
#else
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
#endif // AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_MELEE
					int iDefenderStrength = pDefender->GetMaxDefenseStrength(pTargetPlot, pAttacker);
					CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pDefender->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit)
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pAttacker, NULL, false);

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
#ifdef AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_MELEE
				if (pAttacker->IsCanAttackRanged() && pAttacker->GetMaxRangedCombatStrength(NULL, /*pCity*/ pCity, true, true) > pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL))
#else
				if(pAttacker->IsCanAttackRanged() && pAttacker->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) > pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL))
#endif
				{
					iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, false);
					iExpectedSelfDamage = 0;
				}
				else
				{
#ifdef AUI_TACTICAL_FIX_COMPUTE_EXPECTED_DAMAGE_MELEE
					int iAttackerStrength;
					if (plotDistance(pAttacker->getX(), pAttacker->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > 1)
					{
						iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
					}
					else
					{
						iAttackerStrength = pAttacker->GetMaxAttackStrength(pAttacker->plot(), pTargetPlot, NULL);
					}
#else
					int iAttackerStrength = pAttacker->GetMaxAttackStrength(NULL, pTargetPlot, NULL);
#endif
					int iDefenderStrength = pCity->getStrengthValue();
					CvUnit* pFireSupportUnit = CvUnitCombat::GetFireSupportUnit(pCity->getOwner(), pTargetPlot->getX(), pTargetPlot->getY(), pAttacker->getX(), pAttacker->getY());
					int iDefenderFireSupportCombatDamage = 0;
					if(pFireSupportUnit != NULL)
					{
						iDefenderFireSupportCombatDamage = pFireSupportUnit->GetRangeCombatDamage(pAttacker, NULL, false);
					}

					iExpectedDamage = pAttacker->getCombatDamage(iAttackerStrength, iDefenderStrength, pAttacker->getDamage() + iDefenderFireSupportCombatDamage, /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ true);
#ifdef AUI_TACTICAL_FIX_NO_CAPTURE
					int iMaxRangedCityDamage = pCity->GetMaxHitPoints() - pCity->getDamage() - 1;
					if (pAttacker->isNoCapture() && iExpectedDamage > iMaxRangedCityDamage)
						iExpectedDamage = iMaxRangedCityDamage;
#endif
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
		iExpectedDamage = pAttacker->GetRangeCombatDamage(NULL, pCity, /*bIncludeRand*/ false, 0, NULL, NULL, false);
	}
	else
	{
		CvUnit* pDefender = pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID());
		if(pDefender)
		{
			iExpectedDamage = pAttacker->GetRangeCombatDamage(pDefender, NULL, false, 0, NULL, NULL, false);
		}
	}

	return iExpectedDamage >= iMinDamage;
}

/// Move up close to our target avoiding our own units if possible
bool CvTacticalAI::MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, DomainTypes eDomain, int iMaxTurns)
{
	if (!pUnit || !pTarget)
		return false;

	int iFlags = CvUnit::MOVEFLAG_NO_ATTACKING;
	//can we move there directly? if not try to move to an adjacent plot
	if (!pUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION))
		iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
	if (eDomain==pTarget->getDomain())
		iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
	int iTurns = pUnit->TurnsToReachTarget(pTarget,iFlags,iMaxTurns);

	//if not possible, try again with more leeway
	if (iTurns==INT_MAX)
	{
		iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_NO_ATTACKING;
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
		{
			if( pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
		}

		UnitProcessed(pUnit->GetID(), pUnit->IsCombatUnit());
		return true;
	}

	return false;
}

/// Find a multi-turn target for a land barbarian to wander towards
CvPlot* CvTacticalAI::FindBestBarbarianLandMove(CvUnit* pUnit)
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

	// if nothing to explore, go back to camp
	if (pBestMovePlot == NULL)
	{
		pBestMovePlot = FindNearbyTarget(pUnit, m_iLandBarbarianRange, AI_TACTICAL_TARGET_BARBARIAN_CAMP);
	}

	return pBestMovePlot;
}

/// Find a multi-turn target for a land barbarian to wander towards
CvPlot* CvTacticalAI::FindPassiveBarbarianLandMove(CvUnit* pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue;
	int iValue;

	iBestValue = MAX_INT;
	pBestMovePlot = NULL;

	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		// Is this target a camp?
		if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_BARBARIAN_CAMP || m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY_TO_DEFEND)
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
CvPlot* CvTacticalAI::FindBestBarbarianSeaMove(CvUnit* pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue = MAX_INT;
	CvTacticalTarget* pTarget;

	SPathFinderUserData data(pUnit, 0, m_iSeaBarbarianRange/3); //assume 3 moves per turn
	data.ePathType = PT_UNIT_REACHABLE_PLOTS;
	ReachablePlots movePlots = GC.GetPathFinder().GetPlotsInReach(pUnit->plot(), data);

	// Loop through all unit targets to find the closest
	pTarget = GetFirstUnitTarget();
	while(pTarget != NULL)
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		ReachablePlots::const_iterator itPlot = movePlots.find(pPlot->GetPlotIndex());
		if (itPlot != movePlots.end() && itPlot->iTurns < iBestValue)
		{
			iBestValue = itPlot->iTurns;
			pBestMovePlot = pPlot;
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

			pTarget = GetNextZoneTarget();
		}

		// Try to sail to the second closest camp - this should result in patrolling behavior
		pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
		while(pTarget != NULL)
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
			pTarget = GetNextZoneTarget();
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

		if (iValue > iBestValue || (iValue == iBestValue && GC.getGame().getSmallFakeRandNum(3,*pConsiderPlot)==0))
		{
			pBestMovePlot = pConsiderPlot;
			iBestValue = iValue;
		}
	}

	return pBestMovePlot;
}

/// Scan nearby tiles for a trade route to sit and gank from
CvPlot* CvTacticalAI::FindBarbarianGankTradeRouteTarget(CvUnit* pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestDistance = INT_MAX;
	int iRange = m_iLandBarbarianRange/2+1;

	for(int iX = -iRange; iX <= iRange; iX++)
	{
		for(int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iX, iY, iRange);
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

			if(!pUnit->CanReachInXTurns( pPlot, 2))
			{
				continue;
			}

			int iDistance = plotDistance(*pPlot,*pUnit->plot());
			if(pPlot->IsTradeUnitRoute() && iDistance<iBestDistance)
			{
				pBestMovePlot = pPlot;
				iBestDistance = iDistance;
			}
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

		if (IsUnitHealing(pUnit->GetID()) && m_pPlayer->GetPlotDanger(*pUnitPlot,pUnitPlot->getPlotCity())>0)
			return true;
	}
	else
	{
		CvUnit *pCarrier = pUnit->getTransportUnit();
		if (pCarrier && pCarrier->isProjectedToDieNextTurn())
			return true;

		if (IsUnitHealing(pUnit->GetID()) && pCarrier->GetDanger(pUnitPlot)>0)
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

/// Find a multi-turn target for a land unit to wander towards
CvPlot* CvTacticalAI::FindNearbyTarget(CvUnit* pUnit, int iRange, AITacticalTargetType eType, bool bAllowDefensiveTargets)
{
	if (pUnit == NULL)
		return NULL;

	CvPlot* pBestMovePlot = NULL;
	int iBestValue = 0;
	int iMaxTurns = iRange/2+3;

	// fill this in once we have our first match
	ReachablePlots reachablePlots;

	// Loop through all appropriate targets to find the closest
	for(unsigned int iI = 0; iI < m_AllTargets.size(); iI++)
	{
		CvTacticalTarget target = m_AllTargets[iI];

		// Is the target of an appropriate type?
		bool bTypeMatch = false;
		if(eType == AI_TACTICAL_TARGET_NONE)
		{
			if(target.GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT ||
			    target.GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT ||
			    target.GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT ||
			    target.GetTargetType() == AI_TACTICAL_TARGET_CITY ||
				target.GetTargetType() == AI_TACTICAL_TARGET_IMPROVEMENT)
			{
				bTypeMatch = true;
			}
			
			//only majors go after barb camps
			if(target.GetTargetType() == AI_TACTICAL_TARGET_BARBARIAN_CAMP && !m_pPlayer->isMinorCiv() && !m_pPlayer->isBarbarian())
			{
				bTypeMatch = true;
			}

			if (bAllowDefensiveTargets || m_pPlayer->isMinorCiv())
			{
				if(target.GetTargetType() == AI_TACTICAL_TARGET_CITY_TO_DEFEND ||
					target.GetTargetType() == AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND ||
					target.GetTargetType() == AI_TACTICAL_TARGET_DEFENSIVE_BASTION)
				{
					bTypeMatch = true;
				}
			}
		}
		else if(target.GetTargetType() ==  eType)
		{
			bTypeMatch = true;
		}

		// Is this unit near enough?
		if(bTypeMatch)
		{
			CvPlot* pPlot = GC.getMap().plot(target.GetTargetX(), target.GetTargetY());
			if(!pPlot)
				continue;

			//shortcut, may happen often
			if (pUnit->plot()==pPlot)
				return pPlot;

			//can't do anything if we would need to embark
			if (pPlot->needsEmbarkation(pUnit))
				continue;

			//Ranged naval unit? Let's get a water plot (naval melee can enter cities, don't care for others)
			if(!pPlot->isWater() && pUnit->isRanged() && pUnit->getDomainType() == DOMAIN_SEA)
			{
				pPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pPlot);
				if(!pPlot)
					continue;
			}
	
			//see how far the unit can go
			if (reachablePlots.empty())
			{
				SPathFinderUserData data(pUnit,0,iMaxTurns);
				data.ePathType = PT_UNIT_REACHABLE_PLOTS;
				reachablePlots = GC.GetPathFinder().GetPlotsInReach(pUnit->plot(), data);
			}

			//how long would it take to go to the target
			ReachablePlots::iterator it = reachablePlots.find(pPlot->GetPlotIndex());
			if (it==reachablePlots.end())
				continue;

			int iValue = target.GetAuxIntData() / max(1,it->iTurns);
			if( iValue > iBestValue || (iValue == iBestValue && GC.getGame().getSmallFakeRandNum(3,*pPlot)==0) )
			{
				pBestMovePlot = pPlot;
				iBestValue = iValue;
			}
		}
	}

	return pBestMovePlot;
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
void CvTacticalAI::PerformChosenMoves()
{
	// Make moves up into hexes, starting with units already close to their final destination
	std::stable_sort(m_ChosenBlocks.begin(), m_ChosenBlocks.end(), TacticalAIHelpers::SortBlockingUnitByDistanceAscending);

	// First loop through is for units that have a unit moving into their hex.  They need to leave first!
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if(pUnit)
		{
			if(pUnit->plot() != m_ChosenBlocks[iI].GetPlot() && IsInChosenMoves(pUnit->plot()) && m_ChosenBlocks[iI].GetPlot()->getMaxFriendlyUnitsOfType(pUnit) == 0)
			{
				if (ExecuteMoveToPlot(pUnit, m_ChosenBlocks[iI].GetPlot(), true))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying %s %d (to get out of way), To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
									  pUnit->getName().GetCString(), pUnit->GetID(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
									  pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
						LogTacticalMessage(strMsg);
					}

					// Use number of choices field to indicate already moved
					m_ChosenBlocks[iI].SetNumChoices(-1);
				}
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
					ExecuteMoveToPlot(pUnit, m_ChosenBlocks[iI].GetPlot(), true);

					// Use number of choices field to indicate already moved
					if(pPlotBeforeMove != pUnit->plot())
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying %s %d, To X: %d, To Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
										  pUnit->getName().GetCString(), pUnit->GetID(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
										  pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
							LogTacticalMessage(strMsg);
						}
						m_ChosenBlocks[iI].SetNumChoices(-1);
					}
				}
			}
		}
	}

	// Third loop is for units we still haven't been able to move (other units must be blocking their target for this turn)
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());
		if(pUnit)
		{
			// Someone we didn't move above?
			if(m_ChosenBlocks[iI].GetNumChoices() != -1)
			{
				CvPlot* pPlotBeforeMove = pUnit->plot();
				if(ExecuteMoveToPlot(pUnit, m_ChosenBlocks[iI].GetPlot(), true))
				{
					if(pPlotBeforeMove != pUnit->plot())
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Deploying %s %d to space near target, Target X: %d, Target Y: %d, At X: %d, At Y: %d, Distance Before Move: %d",
								pUnit->getName().GetCString(), pUnit->GetID(), m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(),
								pUnit->getX(), pUnit->getY(), m_ChosenBlocks[iI].GetDistanceToTarget());
							LogTacticalMessage(strMsg);
						}
						m_ChosenBlocks[iI].SetNumChoices(-1);
					}
				}
#if defined(MOD_BALANCE_CORE)
				else if(m_ChosenBlocks[iI].GetPlot() != NULL && (!m_pPlayer->isBarbarian()))
				{
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(), ((DirectionTypes)iDirectionLoop));
						if (pAdjacentPlot)
						{
							if(pAdjacentPlot != NULL && pAdjacentPlot != m_ChosenBlocks[iI].GetPlot())
							{
								if(pAdjacentPlot->isCity() && pAdjacentPlot->getOwner() != pUnit->getOwner())
									continue;
								if(pUnit->getDomainType()==DOMAIN_LAND && pAdjacentPlot->isWater())
									continue;

								CvUnit* pFriendlyUnit = pAdjacentPlot->getUnitByIndex(0);
								if(pFriendlyUnit == NULL)
								{
									if (pUnit->TurnsToReachTarget(pAdjacentPlot, false /*bIgnoreUnits*/, true /*bIgnoreStacking*/, 1) <= 1)
									{
										// Move up there
										pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pAdjacentPlot->getX(), pAdjacentPlot->getY());
										if(pPlotBeforeMove != pUnit->plot())
										{
											m_ChosenBlocks[iI].SetNumChoices(-1);

											if(GC.getLogging() && GC.getAILogging())
											{
												CvString strTemp;
												CvString strMsg;
												strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
												strMsg.Format("Moving %s for proper organization, x: %d, y: %d (PerformChosenMoves)", strTemp.GetCString(), pAdjacentPlot->getX(), pAdjacentPlot->getY());
												LogTacticalMessage(strMsg);
											}
										}
										break;
									}
								}
								else if(pFriendlyUnit != NULL && pFriendlyUnit->getOwner() == pUnit->getOwner() && (pFriendlyUnit->getMoves() > 0))
								{
									CvPlot* pFriendlyPlot = pFriendlyUnit->plot();
									if(pUnit->GetDanger(pFriendlyPlot) > pUnit->GetDanger() && !pUnit->isRanged() && pFriendlyUnit->isRanged())
									{
										if(pUnit->CanSwapWithUnitHere(*pFriendlyPlot))
										{
											// Move up there
											pFriendlyUnit->SetActivityType(ACTIVITY_AWAKE);
											pUnit->PushMission(CvTypes::getMISSION_SWAP_UNITS(), pFriendlyUnit->getX(), pFriendlyUnit->getY());
											if(pPlotBeforeMove != pUnit->plot())
											{
												m_ChosenBlocks[iI].SetNumChoices(-1);

												if(GC.getLogging() && GC.getAILogging())
												{
													CvString strTemp;
													CvString strTemp2;
													CvString strMsg;
													strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
													strTemp2 = GC.getUnitInfo(pFriendlyUnit->getUnitType())->GetDescription();
													strMsg.Format("Moving %s and repositioning %s in its place for proper organization, x: %d, y: %d (PerformChosenMoves)", strTemp2.GetCString(), strTemp.GetCString(), pFriendlyPlot->getX(), pFriendlyPlot->getY());
													LogTacticalMessage(strMsg);
												}
											}
											break;
										}
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

	// Finish moves for all units
	for(unsigned int iI = 0; iI < m_ChosenBlocks.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_ChosenBlocks[iI].GetUnitID());

		if(!pUnit->isDelayedDeath() && pUnit->canMove())
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
			//check neighboring plots if we can steal a civilian
			CvPlot** aPlotsToCheck = GC.getMap().getNeighborsShuffled(pUnit->plot());
			for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
			{
				const CvPlot* pNeighborPlot = aPlotsToCheck[iCount];
				if (pNeighborPlot && !pNeighborPlot->isCity())
				{
					CvUnit* pUnit2 = pNeighborPlot->getUnitByIndex(0);
					if(pUnit2 && pUnit2->IsCivilianUnit() && GET_TEAM(pUnit2->getTeam()).isAtWar(pUnit->getTeam()))
					{
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pNeighborPlot->getX(), pNeighborPlot->getY());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Already in position at %d, %d, but %s found an enemy civilan to capture at X: %d, Y: %d", 
								m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY(), pUnit->getName().GetCString(), pNeighborPlot->getX(), pNeighborPlot->getY() );
							LogTacticalMessage(strMsg);
						}
						break;
					}
				}
			}

			//see if we can hit an enemy
			TacticalAIHelpers::PerformOpportunityAttack(pUnit,true);

			if(pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Already in position, will fortify with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
						m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					LogTacticalMessage(strMsg);
				}
			}
			else if(pUnit->getDamage() > 0 && pUnit->canHeal(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_HEAL());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Already in position, will heal up with %s, X: %d, Y: %d", pUnit->getName().GetCString(),
						m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					LogTacticalMessage(strMsg);
				}
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Already in position, no move for %s, X: %d, Y: %d", pUnit->getName().GetCString(),
						m_ChosenBlocks[iI].GetPlot()->getX(), m_ChosenBlocks[iI].GetPlot()->getY());
					LogTacticalMessage(strMsg);
				}
			}
		}

		UnitProcessed(pUnit->GetID());
	}
}

/// Move a great general with an operation
void CvTacticalAI::MoveGreatGeneral(CvArmyAI* pArmyAI)
{
	for (unsigned int iI = 0; iI < m_GeneralsToMove.size(); iI++)
	{
		CvPlot* pBestPlot = NULL;
		int iBestScore = -1;
		CvUnit* pGeneral = m_pPlayer->getUnit(m_GeneralsToMove[iI].GetUnitID());

		if(!pGeneral)
			continue;

		//Should we consider using our heal?
		if(pGeneral->canRepairFleet(pGeneral->plot()))
		{
			int iInjured = 0;
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pLoopPlot = plotDirection(pGeneral->plot()->getX(), pGeneral->plot()->getY(), ((DirectionTypes)iI));
				if (pLoopPlot != NULL && pLoopPlot->getNumUnits() > 0)
				{
					CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
					if(pUnit != NULL && pUnit->getOwner() == pGeneral->getOwner() && (pUnit->GetCurrHitPoints() <= (pUnit->GetMaxHitPoints() / 2)))
					{
						iInjured++;
					}
				}
			}
			CvUnit* pUnit = pGeneral->plot()->getUnitByIndex(0);
			if(pUnit != NULL && pUnit->getOwner() == pGeneral->getOwner() && (pUnit->GetCurrHitPoints() <= (pUnit->GetMaxHitPoints() / 2)))
			{
				iInjured++;
			}
			//A lot of injured ships around us
			if(iInjured >= 3)
			{
				pGeneral->PushMission(CvTypes::getMISSION_REPAIR_FLEET());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Expending for REPAIR FLEET ACTION - Great Admiral - %s, At X: %d, At Y: %d",
						pGeneral->getName().GetCString(), pGeneral->getX(), pGeneral->getY());
					LogTacticalMessage(strMsg);
				}
				UnitProcessed(pGeneral->GetID());
				continue;
			}
		}

		CvPlot* pArmyCOM = pArmyAI ? pArmyAI->GetCenterOfMass() : NULL;

		ReachablePlots reachablePlots = pGeneral->GetAllPlotsInReachThisTurn(true, true, false);
		for (ReachablePlots::const_iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
		{
			CvPlot* pEvalPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
			if (!pEvalPlot)
				continue;

			int iScore = ScoreGreatGeneralPlot(pGeneral, pEvalPlot);

			if (pArmyCOM && iScore>0)
			{
				//try to stay with the army center
				int iDistance = plotDistance(pEvalPlot->getX(), pEvalPlot->getY(), pArmyCOM->getX(), pArmyCOM->getY());
				iScore *= MapToPercent(iDistance,6,0);
			}

			if(iScore > iBestScore)
			{
				iBestScore = iScore;
				pBestPlot = pEvalPlot;
			}
		}

		if (pBestPlot == NULL)
		{
			//nothing near us, check all possible defenders
			int iHighestScore = 0;
			int iUnitLoop=0;
			for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))				
			{
				if (!pLoopUnit->IsCombatUnit())
					continue;

				CvPlot* pLoopPlot = pLoopUnit->plot();
				int iScore = ScoreGreatGeneralPlot(pGeneral,pLoopPlot);

				//we don't want to adjust our position too much
				int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pGeneral->getX(), pGeneral->getY());
				iScore *= MapToPercent(iDistance,15,2);

				if (iScore>iHighestScore)
				{
					pBestPlot = pLoopPlot;
					iHighestScore = iScore;
				}
			}
		}

		//ok, one last attempt
		if(pBestPlot == NULL)
		{
			//try to go to a city
			CvCity* pCity = m_pPlayer->GetMilitaryAI()->GetMostThreatenedCity();
			if(!pCity)
				pCity = m_pPlayer->GetClosestCityByEstimatedTurns(pGeneral->plot());

			if(pCity != NULL)
			{
				pBestPlot = pCity->plot();

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("No good plot, so %s is moving to a threatened city, To X: %d, To Y: %d, At X: %d, At Y: %d",
									pGeneral->getName().GetCString(), pBestPlot->getX(), pBestPlot->getY(),
									pGeneral->getX(), pGeneral->getY());
					LogTacticalMessage(strMsg);
				}
			}
		}

		if(pBestPlot != NULL)
		{
			pGeneral->GeneratePath(pBestPlot, NULL, INT_MAX, NULL, true);
			CvPlot *pMovePlot = pGeneral->GetPathEndFirstTurnPlot();
			if(pMovePlot != NULL)
			{
				CvUnit* pDefender = pMovePlot->getBestDefender(m_pPlayer->GetID());
				if(pDefender && !pDefender->isProjectedToDieNextTurn())
				{
					ExecuteMoveToPlot(pGeneral, pMovePlot);
					UnitProcessed(pGeneral->GetID());

					//defender must stay here now, whether he wants to or not
					if(pDefender)
					{
						TacticalAIHelpers::PerformRangedOpportunityAttack(pDefender);
						pDefender->PushMission(CvTypes::getMISSION_SKIP());
						UnitProcessed(pDefender->GetID());
					}

					//if we get here, pathfinding failed or the target plot was not acceptable
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Deploying %s %d to tactically assist troops. To X: %d, To Y: %d, At X: %d, At Y: %d",
							pGeneral->getName().GetCString(), pGeneral->GetID(), pBestPlot->getX(), pBestPlot->getY(),
							pGeneral->getX(), pGeneral->getY());
						LogTacticalMessage(strMsg);
					}

					continue;
				}
			}

			//if we get here, pathfinding failed or the target plot was not acceptable
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Deploying %s %d to assist troops, but couldn't find a safe plot. Going to flee. To X: %d, To Y: %d, At X: %d, At Y: %d",
								pGeneral->getName().GetCString(), pGeneral->GetID(), pBestPlot->getX(), pBestPlot->getY(),
								pGeneral->getX(), pGeneral->getY());
				LogTacticalMessage(strMsg);

				CvPlot* pSafestPlot = TacticalAIHelpers::FindSafestPlotInReach(pGeneral,true);
				if(pSafestPlot != NULL)
				{
					pGeneral->PushMission(CvTypes::getMISSION_MOVE_TO(), pSafestPlot->getX(), pSafestPlot->getY());
					UnitProcessed(pGeneral->GetID());						
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

/// Pick best hexes for deployment around a target. Returns number of ranged unit plots found
int CvTacticalAI::ScoreCloseOnPlots(CvPlot* pTarget, const std::map<int,ReachablePlots>& unitMovePlots)
{
	int iRtnValue = 0;

	// We'll store the hexes we've found here
	m_TempTargets.clear();
	GetTacticalAnalysisMap()->ClearDynamicFlags();
	if (!pTarget)
		return 0;

	ReachablePlots distFromTarget;
	SPathFinderUserData data(m_pPlayer->GetID(), PT_GENERIC_REACHABLE_PLOTS, -1, m_iDeployRadius/2);
	distFromTarget = GC.GetStepFinder().GetPlotsInReach(pTarget, data);

	//we want to be close to the target
	for (ReachablePlots::iterator it = distFromTarget.begin(); it != distFromTarget.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
		if (!pPlot)
			continue;

		//don't walk into traps
		int iEnemies = pPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(),pTarget->getDomain());
		int iInvisible = pPlot->getNumAdjacentNonvisible(m_pPlayer->getTeam());
		if (iEnemies>2 || iInvisible>3)
			continue;

		CvTacticalAnalysisCell* pCell = GetTacticalAnalysisMap()->GetCell(it->iPlotIndex);
		//if an enemy unit is revealed in the course of the turn, this might return true incorrectly!
		if (!pCell->CanUseForOperationGathering())
			continue;

		int iScore = 10000 - (100 + it->iTurns * 100 - it->iMovesLeft / 10);

		//don't know the unit here, so use the generic danger calculation (but danger is only a secondary factor)
		iScore -= m_pPlayer->GetPlotDanger(*pPlot)*10;

		//try to avoid isolated plots
		int nFreeNeighbors = 0;
		CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pPlot);
		for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
		{
			if (!aPlotsToCheck[iI] || !aPlotsToCheck[iI]->isVisible(m_pPlayer->getTeam()))
				continue;

			CvTacticalAnalysisCell* pAdjCell = GetTacticalAnalysisMap()->GetCell(aPlotsToCheck[iI]->GetPlotIndex());
			if (pAdjCell && pAdjCell->CanUseForOperationGatheringCheckWater(pCell->IsWater()))
				nFreeNeighbors++;
		}
		//don't add too much else we'll never engage the enemy ... 
		iScore += nFreeNeighbors * 40;

		//see how far it is for the units
		int iTurnsAway = INT_MAX;
		for ( std::map<int,ReachablePlots>::const_iterator itUnit = unitMovePlots.begin(); itUnit!=unitMovePlots.end(); ++itUnit)
		{
			std::map<int,ReachablePlots>::value_type::second_type::const_iterator itPlot = itUnit->second.find(pPlot->GetPlotIndex());
			if (itPlot!=itUnit->second.end())
				iTurnsAway = min(iTurnsAway,itPlot->iTurns);
		}
		iScore -= iTurnsAway * 100;

		// Unescorted civilian?
		if (pPlot->isEnemyUnit(m_pPlayer->GetID(),false,true) && !pPlot->isEnemyUnit(m_pPlayer->GetID(),true,true))
			iScore += 500;

		bool bHasLOS = pPlot->canSeePlot(pTarget, m_pPlayer->getTeam(), 3, NO_DIRECTION);
		if (bHasLOS)
			iScore += 80;

		// Top priority is hexes to bombard from (within range but not adjacent)
		bool bChoiceBombardSpot = false;
		int iPlotDistance = plotDistance(*pPlot,*pTarget);
		pCell->SetTargetDistance(iPlotDistance);
		if (iPlotDistance > 1 && pPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(), pPlot->getDomain(), NULL, false)==0 && iInvisible<2)
			bChoiceBombardSpot = true;

		pCell->SetHasLOS(bHasLOS);
		pCell->SetSafeForDeployment(bChoiceBombardSpot);
		pCell->SetDeploymentScore(iScore);

		// Save this in our list of potential targets
		CvTacticalTarget target;
		target.SetTargetX(pPlot->getX());
		target.SetTargetY(pPlot->getY());
		target.SetAuxIntData(iScore);

		// A bit of a hack -- use high priority targets to indicate good plots for ranged units
		if (bChoiceBombardSpot)
		{
			target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			iRtnValue++;
		}
		else
		{
			target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
		}

		m_TempTargets.push_back(target);
	}

	return iRtnValue;
}

int CvTacticalAI::ScoreHedgehogPlots(CvPlot* pTarget, const std::map<int,ReachablePlots>& unitMovePlots)
{
	int iRtnValue = 0;

	// We'll store the hexes we've found here
	m_TempTargets.clear();

	for (int iDX = -m_iDeployRadius; iDX <= m_iDeployRadius; iDX++)
	{
		for (int iDY = -m_iDeployRadius; iDY <= m_iDeployRadius; iDY++)
		{
			CvPlot* pPlot = plotXYWithRangeCheck(pTarget->getX(), pTarget->getY(), iDX, iDY, m_iDeployRadius);

			if (!pPlot)
				continue;

			//exclude plots in the wrong area
			if (pTarget->isCity())
			{
				CvCity* pCity = pTarget->getPlotCity();
				if (!pCity->isMatchingArea(pPlot))
					continue;
			}

			int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
			CvTacticalAnalysisCell* pCell = GetTacticalAnalysisMap()->GetCell(iPlotIndex);

			if (pCell->CanUseForOperationGathering())
			{
				bool bChoiceBombardSpot = false;
				int iDistance = plotDistance(pTarget->getX(), pTarget->getY(), pPlot->getX(), pPlot->getY());

				//don't know the unit here, so use the generic danger calculation
				int	iScore = 10000 - m_pPlayer->GetPlotDanger(*pPlot);
				
				//we want to be close to the target 
				iScore -= iDistance * 100;

				bool bMyCity = (pPlot->isCity() && pPlot->getOwner()==m_pPlayer->GetID());
				if (bMyCity)
					iScore += 1000;

				//see how far it is
				int iTurnsAway = INT_MAX;
				for ( std::map<int,ReachablePlots>::const_iterator itUnit = unitMovePlots.begin(); itUnit!=unitMovePlots.end(); ++itUnit)
				{
					std::map<int,ReachablePlots>::value_type::second_type::const_iterator itPlot = itUnit->second.find(pPlot->GetPlotIndex());
					if (itPlot!=itUnit->second.end())
						iTurnsAway = min(iTurnsAway,itPlot->iTurns);
				}
				iScore -= iTurnsAway * 100;

				//we want to be able to attack a lot of plots
				if (pPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(), pPlot->getDomain())>0 && !bMyCity)
				{
					//good plot for melee
					iScore += pPlot->countPassableNeighbors(pPlot->getDomain(), NULL) * 30;
				}
				else
				{
					//good plot for ranged
					bChoiceBombardSpot = true;

					std::vector<CvPlot*> vAttackablePlots;
					pPlot->GetPlotsAtRangeX(2, true, true, vAttackablePlots);
					iScore += vAttackablePlots.size() * 30;
				}

				pCell->SetSafeForDeployment(bChoiceBombardSpot);
				pCell->SetDeploymentScore(iScore);
				pCell->SetTargetDistance(iDistance);

				// Save this in our list of potential targets
				CvTacticalTarget target;
				target.SetTargetX(pPlot->getX());
				target.SetTargetY(pPlot->getY());
				target.SetAuxIntData(iScore);

				// A bit of a hack -- use high priority targets to indicate good plots for ranged units
				if (bChoiceBombardSpot)
				{
					target.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
					iRtnValue++;
				}
				else
				{
					target.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
				}

				m_TempTargets.push_back(target);
			}
		}
	}
	return iRtnValue;
}

/// Support function to pick best hex for a great general to move to
int CvTacticalAI::ScoreGreatGeneralPlot(CvUnit* pGeneral, CvPlot* pLoopPlot)
{
	int iAura = 0;

	if(pLoopPlot == NULL || !pLoopPlot->isValidMovePlot(m_pPlayer->GetID()) || !pLoopPlot->isRevealed(m_pPlayer->getTeam()))
		return 0;

	if(pLoopPlot->isWater() && pGeneral->getDomainType() == DOMAIN_LAND)
		return 0;

	if(!pLoopPlot->isWater() && pGeneral->getDomainType() == DOMAIN_SEA)
		return 0;

	if(!pGeneral->IsCityAttackSupport() && pGeneral->IsNearGreatGeneral(iAura, pLoopPlot, pGeneral)) //near another general
		return 0;

	if(pGeneral->IsCityAttackSupport() && pGeneral->IsNearCityAttackSupport(pLoopPlot, pGeneral)) // Near another sapper
		return 0;

	const CvUnit* pDefender = pLoopPlot->getBestDefender(m_pPlayer->GetID());
	CvCity* pPlotCity = pLoopPlot->getPlotCity();
	if (pLoopPlot->getOwner() != NO_PLAYER && m_pPlayer->IsAtWarWith(pLoopPlot->getOwner()))
	{
		if (!pDefender)
			return 0;
	}
	else
	{
		if (pDefender && pDefender->isProjectedToDieNextTurn() && pPlotCity == NULL)
			return 0;
	}

	//if we're in a besieged city, chances are we cannot escape
	if ( pPlotCity && pPlotCity->isInDangerOfFalling() )
	{
		CvPlot* pEscapePlot = TacticalAIHelpers::FindSafestPlotInReach(pGeneral,true,true);
		if (pEscapePlot) //the city plot is bad if we can escape
			return 0;
	}

	//avoid the front line
	int iBaseMultiplier = 3;
	if (!pGeneral->IsCityAttackSupport() && pLoopPlot->GetAdjacentCity() == NULL)
	{
		if(pLoopPlot->GetNumEnemyUnitsAdjacent(pGeneral->getTeam(),pGeneral->getDomainType()) > 0)
			iBaseMultiplier = 1;
	}

	int iTotalScore = 0;
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pLoopPlot);
	for(int iCount=0; iCount < NUM_DIRECTION_TYPES; iCount++)
	{
		const CvPlot* pNeighborPlot = aPlotsToCheck[iCount];
		if(!pNeighborPlot)
			continue;

		CvUnit* pSupportedUnit = pNeighborPlot->getBestDefender(m_pPlayer->GetID());

		int iMultiplier = iBaseMultiplier;
		if (pSupportedUnit && (pSupportedUnit->getDomainType() == pGeneral->getDomainType()))
		{
			if(pGeneral->IsCityAttackSupport())
			{
				if(pSupportedUnit->IsEnemyCityAdjacent())
				{
					iMultiplier *= 10;
				}
				if(!pSupportedUnit->isRanged())
				{
					iMultiplier *= 5;
				}
				//recent operation
				if(pSupportedUnit->IsRecentlyDeployedFromOperation())
					iMultiplier++;

				//many friendly units around
				if(pSupportedUnit->GetNumOwningPlayerUnitsAdjacent(pDefender)>1)
					iMultiplier++;
			}
			else
			{
				//if enemies are nearby the general is more useful
				if(pSupportedUnit->GetNumEnemyUnitsAdjacent() > 0)
					iMultiplier++;
				if(pSupportedUnit->IsEnemyCityAdjacent())
					iMultiplier++;
				//recent operation
				if(pSupportedUnit->IsRecentlyDeployedFromOperation())
					iMultiplier++;
				//many friendly units around
				if(pSupportedUnit->GetNumOwningPlayerUnitsAdjacent(pDefender)>2)
					iMultiplier++;
			}

			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot( pSupportedUnit->plot() );
			if (pZone && pZone->GetZoneCity()!= NULL && pZone->GetOwner() != m_pPlayer->GetID())
			{
				CvCity *pClosestCity = pZone->GetZoneCity();
				if (pClosestCity && m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pClosestCity))
				{
					iMultiplier *= 10;
				}
			}

			iTotalScore += iMultiplier*pSupportedUnit->GetPower();
			//Same army? Emphasize.
			if(pGeneral->getArmyID() != -1)
			{
				if(pSupportedUnit->getArmyID() == pGeneral->getArmyID())
				{
					iTotalScore *= 10;
				}
				else
				{
					iTotalScore /= 5;
				}
			}
		}
	}

	if(!pGeneral->IsCityAttackSupport() && pDefender)
	{
		int iDefenderPower = pDefender->GetPower();
		if(pLoopPlot->isCity())
			iDefenderPower *= 2;
		iTotalScore += iDefenderPower;
	}

	CvCity* pClosestEnemyCity = m_pPlayer->GetTacticalAI()->GetNearestTargetCity(pGeneral->plot());
	if(pClosestEnemyCity && pDefender)
		iTotalScore += (10000 - (plotDistance(pDefender->getX(), pDefender->getY(), pClosestEnemyCity->getX(), pClosestEnemyCity->getY()) * 3));

	return iTotalScore;
}


/// Remove a unit that we've allocated from list of units to move this turn
void CvTacticalAI::UnitProcessed(int iID, bool bMarkTacticalMap)
{
	m_CurrentTurnUnits.remove(iID);
	CvUnit* pUnit = m_pPlayer->getUnit(iID);

#if defined(MOD_BALANCE_CORE)
	if (!pUnit)
		return;
#endif

	pUnit->SetTurnProcessed(true);

	if(bMarkTacticalMap)
	{
		CvTacticalAnalysisMap* pMap = GetTacticalAnalysisMap();
		if(pMap->IsUpToDate())
		{
			int iPlotIndex = GC.getMap().plotNum(pUnit->getX(), pUnit->getY());
			CvTacticalAnalysisCell* pCell = pMap->GetCell(iPlotIndex);
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
	if(pZone->GetZoneCity() != NULL)
	{
		iCityID = pZone->GetZoneCity()->GetID();
		bIsOurCapital = (pZone->GetZoneCity() == m_pPlayer->getCapitalCity());
		bTempZone = IsTemporaryZoneCity(pZone->GetZoneCity());
	}
	bool bWeHaveUnitsNearEnemy = pZone->GetTotalFriendlyUnitCount() > 0 && pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY;
	bool bTheyHaveUnitsNearUs = pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_NO_UNITS_VISIBLE && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY;
	bool bBothHaveUnits = pZone->GetTotalFriendlyUnitCount() > 0 && pZone->GetTotalEnemyUnitCount() > 0;

	return (bTempZone || bIsOurCapital || bWeHaveUnitsNearEnemy || bTheyHaveUnitsNearUs || bBothHaveUnits);
}


/// Is this civilian target of the highest priority?
bool CvTacticalAI::IsVeryHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	bool bRtnValue = false;
	CvUnit* pUnit = (CvUnit*)pTarget->GetAuxData();
	if(pUnit)
	{
#if defined(MOD_BALANCE_CORE_MILITARY)
		if(pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral() || pUnit->IsCityAttackSupport())
#else
		if(pUnit->AI_getUnitAIType() == UNITAI_GENERAL || pUnit->AI_getUnitAIType() == UNITAI_ADMIRAL)
#endif
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
void CvTacticalAI::LogTacticalMessage(const CvString& strMsg, bool bSkipLogDominanceZone)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite );

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		if(!bSkipLogDominanceZone)
		{
			CvString strTemp = "no zone, ";
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(m_iCurrentZoneIndex);
			if(pZone != NULL)
			{
				strTemp.Format("Zone ID: %d, ", pZone->GetZoneID());
			}
			strBaseString += strTemp;
		}

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
bool TacticalAIHelpers::SortBlockingUnitByDistanceAscending(const CvBlockingUnit& obj1, const CvBlockingUnit& obj2)
{
	return obj1.GetDistanceToTarget() < obj2.GetDistanceToTarget();
}

bool TacticalAIHelpers::SortByExpectedTargetDamageDescending(const CvTacticalUnit& obj1, const CvTacticalUnit& obj2)
{
	return obj1.GetExpectedTargetDamage()*2-obj1.GetExpectedSelfDamage() > obj2.GetExpectedTargetDamage()*2-obj2.GetExpectedSelfDamage();
}

bool CvTacticalAI::IsUnitHealing(int iUnitID) const
{
	return m_HealingUnits.find(iUnitID) != m_HealingUnits.end();
}

ReachablePlots TacticalAIHelpers::GetAllPlotsInReach(const CvUnit* pUnit, const CvPlot* pStartPlot, int iFlags, int iMinMovesLeft, int iStartMoves, const set<int>& plotsToIgnoreForZOC)
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

int TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, const CvPlot* pBasePlot, std::set<int>& resultSet, bool bOnlyWithEnemy, bool bIgnoreVisibility)
{
	if (!pUnit || !pBasePlot)
		return false;

	resultSet.clear();
	int iRange = min(5,max(1,pUnit->GetRange()));

	for(int i=1; i<RING_PLOTS[iRange]; i++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pBasePlot,i);
		if (!pLoopPlot)
			continue;

		if (!bOnlyWithEnemy || pLoopPlot->isEnemyCity(*pUnit) || pLoopPlot->isEnemyUnit(pUnit->getOwner(),true,!bIgnoreVisibility))
			if (pUnit->canEverRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY(), pBasePlot, bIgnoreVisibility))
				resultSet.insert(pLoopPlot->GetPlotIndex());
	}

	return (int)resultSet.size();
}

int TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(const CvUnit* pUnit, ReachablePlots& basePlots, std::set<int>& resultSet, bool bOnlyWithEnemy, bool bIgnoreVisibility)
{
	resultSet.clear();

	if (!pUnit || !pUnit->IsCanAttackRanged())
		return false;

	int iRange = min(5,max(1,pUnit->GetRange()));

	for (ReachablePlots::iterator base=basePlots.begin(); base!=basePlots.end(); ++base)
	{
		CvPlot* pBasePlot = GC.getMap().plotByIndexUnchecked( base->iPlotIndex );

		int iPlotMoves = base->iMovesLeft;
		if (iPlotMoves<=0)
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

	return (int)resultSet.size();
}

bool TacticalAIHelpers::IsAttackNetPositive(CvUnit* pUnit, const CvPlot* pTargetPlot)
{
	if (!pUnit || !pTargetPlot)
		return false;

	//target can be city or a unit
	CvCity* pTargetCity = pTargetPlot->getPlotCity();
	CvUnit* pTargetUnit = pTargetPlot->getVisibleEnemyDefender( pUnit->getOwner() );

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

//attack the target plot with the given unit if possible this turn. if no target given, pick the best one
bool TacticalAIHelpers::PerformOpportunityAttack(CvUnit* pUnit, bool bAllowDisengage)
{
	if (!pUnit || !pUnit->IsCanAttack())
		return false;

	//cannot attack if no attacks left
	int iAttacksLeft = pUnit->getNumAttacks() - pUnit->getNumAttacksMadeThisTurn();
	if (iAttacksLeft == 0)
		return false;

	//for ranged we have a readymade method
	if (pUnit->IsCanAttackRanged())
		return TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit, bAllowDisengage);

	//melee needs more work
	vector<CvUnit*> vEnemies = pUnit->plot()->GetAdjacentEnemyUnits(pUnit->getTeam(), pUnit->getDomainType());
	if (vEnemies.empty())
		return false;

	vector<SPlotWithScore> targets;
	for (size_t i = 0; i < vEnemies.size(); i++)
	{
		int iDamageDealt = 0, iDamageReceived = 0;
		iDamageDealt = GetSimulatedDamageFromAttackOnUnit(vEnemies[i], pUnit, vEnemies[i]->plot(), pUnit->plot(), iDamageReceived);
		if (iDamageDealt == vEnemies[i]->GetCurrHitPoints())
		{
			iDamageDealt += 20; //bonus for a kill!
			if (pUnit->getHPHealedIfDefeatEnemy() > 0)
				iDamageReceived = max(0, iDamageReceived - pUnit->getHPHealedIfDefeatEnemy());
		}

		int iScore = (1000 * iDamageDealt) / (iDamageReceived + 10);
		if (iScore > 1000 * (int)vEnemies.size() ) //threshold depends on number of enemies around us!
			targets.push_back( SPlotWithScore(vEnemies[i]->plot(),iScore) );
	}

	if (!targets.empty())
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Performing melee opportunity attack on (%d:%d) with %s at (%d:%d)",
				targets.front().pPlot->getX(), targets.front().pPlot->getY(), pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
			GET_PLAYER(pUnit->getOwner()).GetTacticalAI()->LogTacticalMessage(strMsg);
		}

		std::sort(targets.begin(), targets.end());
		std::reverse(targets.begin(), targets.end());
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), targets.front().pPlot->getX(), targets.front().pPlot->getY());
		return true;
	}

	return false;
}

//see if we can hit anything from our current plot - with or without moving
bool TacticalAIHelpers::PerformRangedOpportunityAttack(CvUnit* pUnit, bool bAllowDisengage)
{
	if (!pUnit || !pUnit->IsCanAttackRanged() || !pUnit->canMove())
		return false;

	int iRange = max(1,min(5,pUnit->GetRange()));
	CvPlot* pBasePlot = pUnit->plot();
	bool bIsAirUnit = pUnit->getDomainType()==DOMAIN_AIR;
	if (bIsAirUnit)
		bAllowDisengage = false;
	
	int iMaxDamage = 0;
	CvPlot* pBestTarget = NULL;

	for (int i=RING0_PLOTS; i<RING_PLOTS[iRange]; i++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pBasePlot, i);
		if (!pLoopPlot)
			continue;

		if (pLoopPlot->isCity() && atWar( GET_PLAYER(pUnit->getOwner()).getTeam(), GET_PLAYER(pLoopPlot->getOwner()).getTeam() ) )
		{
			//prefer attacking units if present
			if (pBestTarget==NULL)
				pBestTarget = pLoopPlot;
			continue;
		}

		CvUnit* pOtherUnit = pLoopPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(), pUnit, true /*testWar*/);

		//don't blindly attack the first one we find, check how much damage we can do
		if (pOtherUnit && !pOtherUnit->isDelayedDeath() && pUnit->canEverRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
		{
			int iDamage = bIsAirUnit ? pUnit->GetAirCombatDamage(pOtherUnit, NULL, false) : 
										pUnit->GetRangeCombatDamage(pOtherUnit, NULL, false, 0) +  pUnit->GetRangeCombatSplashDamage(pOtherUnit->plot());
			//prefer attacking units over cities
			if (iDamage>iMaxDamage || pBestTarget->isCity())
			{
				pBestTarget = pLoopPlot;
				iMaxDamage = iDamage;
			}
		}
	}

	if (!pBestTarget)
		return false;

	if (bAllowDisengage)
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Performing ranged opportunity attack with disengagement on (%d:%d) with %s at (%d:%d)",
				pBestTarget->getX(), pBestTarget->getY(), pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
			GET_PLAYER(pUnit->getOwner()).GetTacticalAI()->LogTacticalMessage(strMsg);
		}

		//no loop needed, there is only one unit anyway
		vector<STacticalAssignment> vAssignments;
		TacticalAIHelpers::FindBestAssignmentsForUnits(vector<CvUnit*>(1, pUnit), pBestTarget, AL_LOW, 4, 23, vAssignments);
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
bool TacticalAIHelpers::CountDeploymentPlots(TeamTypes eTeam, const CvPlot* pTarget, int iNumUnits, int iRange)
{
	int iDX, iDY;
	CvPlot* pPlot;
	int iNumDeployPlotsFound = 0;

	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pPlot = plotXY(pTarget->getX(), pTarget->getY(), iDX, iDY);
			if(pPlot != NULL)
			{
				int iPlotDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());
				if(iPlotDistance <= iRange)
				{
					if(!pPlot->isValidMovePlot(NO_PLAYER))
					{
						continue;
					}
					if(!GET_TEAM(eTeam).canEmbark() && (pPlot->isShallowWater() || pPlot->isWater()))
					{
						continue;
					}
					else if(!GET_TEAM(eTeam).canEmbarkAllWaterPassage() && (pPlot->isWater()))
					{
						continue;
					}
					if(pPlot->getNumUnits() > 0)
					{
						CvUnit* pUnit = pPlot->getUnitByIndex(0);
						if(pUnit && pUnit->IsCombatUnit())
						{
							continue;
						}
					}

					iNumDeployPlotsFound++;
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

CvPlot* TacticalAIHelpers::FindSafestPlotInReach(const CvUnit* pUnit, bool bAllowEmbark, bool bLowDangerOnly, bool bConsiderSwap)
{
	CvWeightedVector<CvPlot*, 8, true> aCityList;
	CvWeightedVector<CvPlot*, 8, true> aZeroDangerList;
	CvWeightedVector<CvPlot*, 8, true> aCoverList;
	CvWeightedVector<CvPlot*, 8, true> aDangerList;

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

		CvPlayer& kPlayer = GET_PLAYER(pUnit->getOwner());
		int iDanger = pUnit->GetDanger(pPlot);

		int iCityDistance = kPlayer.GetCityDistanceInEstimatedTurns(pPlot);
		//when in doubt, prefer to move, even in the wrong direction - being stuck looks stupid
		if (pUnit->atPlot(*pPlot) && iDanger>GC.getENEMY_HEAL_RATE())
			iCityDistance+=2;

		bool bIsZeroDanger = (iDanger <= 0);
		bool bIsInCity = pPlot->isFriendlyCity(*pUnit, false);
		bool bIsInCover = (pPlot->getNumDefenders(pUnit->getOwner()) > 0) && !pUnit->IsCanDefend(pPlot); // only move to cover if I'm defenseless here
		bool bIsInTerritory = (pPlot->getTeam() == kPlayer.getTeam());

		bool bWrongDomain = pPlot->needsEmbarkation(pUnit);
		bool bWouldEmbark = bWrongDomain && !pUnit->isEmbarked();

		//avoid overflow further down and useful handling for civilians
		if (iDanger == INT_MAX)
			iDanger = 10000;

		//we can't heal after moving and lose fortification bonus, so the current plot gets a bonus (respectively all others a penalty)
		if (pPlot != pUnit->plot() && pUnit->canHeal(pUnit->plot()))
			iDanger++;

		//try to hide - if there are few enemy units, this might be a tiebreaker
		//this is cheating a bit, really we need to check if the plot is visible for the enemy units visible to us
		if (pPlot->isVisibleToEnemy(pUnit->getOwner()))
			iDanger+=10;

		//don't stay here, try to get away even if it means temporarily moving to a higher danger plot
		if (pPlot->IsEnemyCityAdjacent(pUnit->getTeam(),NULL))
			iDanger+=20;

		//use city distance as tiebreaker
		int iScore = iDanger * 10 + iCityDistance;

		//discourage water tiles for land units
		//note that zero danger status has already been established, this is only for sorting now
		if (bWrongDomain)
			iScore += 500;

		if(bIsInCity)
		{
			if (!pPlot->getPlotCity()->isInDangerOfFalling())
				aCityList.push_back(pPlot, iScore);
		}
		else if(bIsZeroDanger)
		{
			//if danger is zero, look at distance to closest owned city instead
			//idea: could also look at number of plots reachable from pPlot to avoid dead ends
			aZeroDangerList.push_back(pPlot, bIsInTerritory ? iCityDistance : iCityDistance*2 );
		}
		else if(bIsInCover) //mostly relevant for civilians - when in doubt go home
		{
			aCoverList.push_back(pPlot, iScore - iCityDistance);
		}
		else if( (!bLowDangerOnly || iDanger<pUnit->GetCurrHitPoints()) && (!bWouldEmbark || bAllowEmbark) )
		{
			aDangerList.push_back(pPlot, iScore);
		}
	}

	//this makes the highest weight come first!
	aCityList.SortItems();
	aZeroDangerList.SortItems();
	aCoverList.SortItems();
	aDangerList.SortItems();

	// Now that we've gathered up our lists of destinations, pick the most promising one
	if (aCityList.size()>0)
		return aCityList.GetElement( aCityList.size()-1 );
	else if (aZeroDangerList.size()>0)
		return aZeroDangerList.GetElement(  aZeroDangerList.size()-1 );
	else if (aCoverList.size()>0)
		return aCoverList.GetElement( aCoverList.size()-1 );
	else if (aDangerList.size()>0)
		return aDangerList.GetElement( aDangerList.size()-1 );

	return NULL;
}

void CTacticalUnitArray::push_back(const CvTacticalUnit& unit)
{
	m_vec.push_back(unit);

	CvUnit* pUnit = m_owner ? m_owner->getUnit( unit.GetID() ) : NULL;

	if (pUnit)
	{
		//not a nice design to use a global variable here, but it's easier than modifying the code in 30 places
		pUnit->setTacticalMove( m_currentTacticalMove.m_eMoveType );

		if (unit.GetID()==g_currentUnitToTrack)
		{
			CvPlayer& owner = GET_PLAYER(pUnit->getOwner());
			OutputDebugString( CvString::format("turn %03d: using %s %s %d for tactical move %s. hitpoints %d, pos (%d,%d), danger %d\n", 
				GC.getGame().getGameTurn(), owner.getCivilizationAdjective(), pUnit->getName().c_str(), g_currentUnitToTrack,
				pUnit->isBarbarian() ? barbarianMoveNames[m_currentTacticalMove.m_eMoveType] : GC.getTacticalMoveInfo(m_currentTacticalMove.m_eMoveType)->GetType(), 
				pUnit->GetCurrHitPoints(), pUnit->getX(), pUnit->getY(), pUnit->GetDanger() ) );
		}
	}
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

		//don't check movement, don't need to heal right now
		if (!pUnit->canHeal(pPlot, false, true))
			continue;

		//can we stay there?
		if (!pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
		{
			//can we swap?
			CvUnit* pSwapUnit = pUnit->GetPotentialUnitToSwapWith(*pPlot);
			//melee units are there to soak damage ...
			int iDangerLimit = pSwapUnit ? (pSwapUnit->isRanged() ? pSwapUnit->GetCurrHitPoints() : (3 * pSwapUnit->GetCurrHitPoints()) / 2) : 0;

			if (!pSwapUnit || pSwapUnit->GetDanger(pUnit->plot()) > iDangerLimit)
				continue;
		}

		if (pUnit->GetDanger(pPlot) < pUnit->healRate(pPlot))
		{
			int iScore = pUnit->healRate(pPlot) - pUnit->GetDanger(pPlot) / 5;
			//higher distance = bad
			iScore -= GET_PLAYER(pUnit->getOwner()).GetCityDistanceInEstimatedTurns(pPlot);
			//friendly combat unit nearby = good
			if (pPlot->GetNumFriendlyUnitsAdjacent(pUnit->getTeam(), NO_DOMAIN, pUnit) > 0)
				iScore++;

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

	std::vector<CvPlot*> vCandidates;
	pTarget->GetPlotsAtRangeX(iRange, false, !bIgnoreLOS, vCandidates);

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

		if(bOnlyInDomain)
			//subs can only attack within their (water) area or adjacent cities
			if (pRefPlot->getArea() != vCandidates[i]->getArea())
			{
				CvCity *pCity = vCandidates[i]->getPlotCity();
				if (!pCity || !pCity->isAdjacentToArea(pRefPlot->getArea()))
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
int TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(CvCity* pCity, const CvUnit* pAttacker, CvPlot* pAttackerPlot, int& iAttackerDamage, bool bIgnoreUnitAdjacencyBoni, int iExtraDefenderDamage)
{
	if (!pAttacker || !pCity || pAttacker->isDelayedDeath() || pAttacker->IsDead())
		return 0;
		
	int iDamage = 0;
	if (pAttacker->IsCanAttackRanged())
	{
		if (pAttacker->getDomainType() == DOMAIN_AIR)
			iDamage += pAttacker->GetAirCombatDamage(NULL, pCity, false);
		else
			iDamage += pAttacker->GetRangeCombatDamage(NULL, pCity, false, iExtraDefenderDamage, NULL, pAttackerPlot, bIgnoreUnitAdjacencyBoni);

		iAttackerDamage = 0;
	}
	else
	{
		if (pAttacker->isRangedSupportFire())
			iDamage += pAttacker->GetRangeCombatDamage(NULL, pCity, false, iExtraDefenderDamage, NULL, pAttackerPlot, bIgnoreUnitAdjacencyBoni);

		//just assume the unit can attack from it's current location - modifiers might be different, but thats acceptable
		iDamage += pAttacker->getCombatDamage(
			pAttacker->GetMaxAttackStrength(pAttackerPlot, pCity->plot(), NULL, bIgnoreUnitAdjacencyBoni),
			pCity->getStrengthValue(), 
			pAttacker->getDamage(), false, false, true);

		iAttackerDamage = pAttacker->getCombatDamage(
			pCity->getStrengthValue(),
			pAttacker->GetMaxAttackStrength(pAttackerPlot, pCity->plot(), NULL),
			pCity->getDamage() + iExtraDefenderDamage, false, true, false);

		//if both would die, the attacking unit survives!
		if (pCity->getDamage() + iExtraDefenderDamage + iDamage >= pCity->GetMaxHitPoints() && pAttacker->getDamage() + iAttackerDamage >= pAttacker->GetMaxHitPoints())
			iAttackerDamage = pAttacker->GetMaxHitPoints() - pAttacker->getDamage() - 1;
	}

	return iDamage;
}

//helper function for unit threat calculation
int TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(const CvUnit* pDefender, const CvUnit* pAttacker, CvPlot* pDefenderPlot, CvPlot* pAttackerPlot, int& iAttackerDamage, bool bIgnoreUnitAdjacencyBoni, int iExtraDefenderDamage)
{
	if (!pAttacker || !pDefender || pDefender->isDelayedDeath() || pDefender->IsDead() || pAttacker->isDelayedDeath() || pAttacker->IsDead())
		return 0;
		
	int iDamage = 0;
	if (pAttacker->IsCanAttackRanged())
	{
		if (pAttacker->getDomainType() == DOMAIN_AIR)
		{
			CvUnit* pInterceptor = pAttacker->GetBestInterceptor(*pDefenderPlot, pDefender, false, true);
			// Always assume interception is successful
			iAttackerDamage = pInterceptor ? pInterceptor->GetInterceptionDamage(pDefender, false, pDefenderPlot, pAttackerPlot) : 0;

			iDamage += pAttacker->GetAirCombatDamage(pDefender, NULL, false, iAttackerDamage, pDefenderPlot, pAttackerPlot);
		}
		else
		{
			iDamage += pAttacker->GetRangeCombatDamage(pDefender, NULL, false, iExtraDefenderDamage, pDefenderPlot, pAttackerPlot, bIgnoreUnitAdjacencyBoni);
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
			iDamage += pAttacker->GetRangeCombatDamage(pDefender, NULL, false, iExtraDefenderDamage, pDefenderPlot, pAttackerPlot, bIgnoreUnitAdjacencyBoni);

		//just assume the unit can attack from its current location - modifiers might be different, but thats acceptable
		iDamage += pAttacker->getCombatDamage(
			pAttacker->GetMaxAttackStrength(pAttackerPlot, pDefenderPlot, pDefender, bIgnoreUnitAdjacencyBoni),
			pDefender->GetMaxDefenseStrength(pDefenderPlot, pAttacker), //do not override defender flanking/general bonus
			pAttacker->getDamage(), false, false, false);

		iAttackerDamage = pDefender->getCombatDamage(
			pDefender->GetMaxAttackStrength(pDefenderPlot, pAttackerPlot, pAttacker), //do not override defender flanking/general bonus
			pAttacker->GetMaxDefenseStrength(pAttackerPlot, pDefender, bIgnoreUnitAdjacencyBoni),
			pDefender->getDamage() + iExtraDefenderDamage, false, false, false);
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
	if ( iDamageDealt > pDefender->GetCurrHitPoints() && iDamageReceived < 3*pAttacker->GetCurrHitPoints()/4 )
	{
		if (pAttacker->isRanged())
		{
			//can we attack directly
			if (pAttacker->canRangeStrikeAt(pDefender->getX(),pDefender->getY()))
			{
				pAttacker->PushMission(CvTypes::getMISSION_RANGE_ATTACK(),pDefender->getX(),pDefender->getY());
				return true;
			}
			
			//need to move and shoot
			std::vector<CvPlot*> vAttackPlots;
			bool bIgnoreLOS = pAttacker->IsRangeAttackIgnoreLOS();
			pDefender->plot()->GetPlotsAtRangeX(pAttacker->GetRange(), false, !bIgnoreLOS, vAttackPlots);
			for (std::vector<CvPlot*>::iterator it=vAttackPlots.begin(); it!=vAttackPlots.end(); ++it)
			{
				if (pAttacker->TurnsToReachTarget(*it,false,false,1)==0)
				{
					pAttacker->PushMission(CvTypes::getMISSION_MOVE_TO(),(*it)->getX(),(*it)->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER);
					pAttacker->PushMission(CvTypes::getMISSION_RANGE_ATTACK(),pDefender->getX(),pDefender->getY());
					return true;
				}
			}
		}
		else //melee
		{
			if (pAttacker->TurnsToReachTarget(pDefender->plot(),false,false,1)<=1)
			{
				pAttacker->PushMission(CvTypes::getMISSION_MOVE_TO(),pDefender->getX(),pDefender->getY());
				return true;
			}
		}
	}

	return false;
}

bool TacticalAIHelpers::IsCaptureTargetAdjacent(CvUnit * pUnit)
{
	if (pUnit && pUnit->IsCombatUnit() && !pUnit->isNoCapture())
	{
		//don't really check all reachable plots, just the neighbors
		CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pUnit->plot());
		for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pPlot = aPlotsToCheck[iI];

			if (pPlot == NULL)
				continue;

			if (!pUnit->isRanged())
			{
				CvCity* pNeighboringCity = pPlot->getPlotCity();
				if (pNeighboringCity && GET_PLAYER(pUnit->getOwner()).IsAtWarWith(pNeighboringCity->getOwner()) && pNeighboringCity->isInDangerOfFalling())
					return true;
			}

			if (pPlot->getImprovementType()==GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				CvUnit* pDefender = pPlot->getBestDefender(BARBARIAN_PLAYER);
				if (!pDefender || TacticalAIHelpers::IsAttackNetPositive(pUnit,pPlot))
					return true;
			}
		}
	}

	return false;
}

CvPlot* TacticalAIHelpers::GetFirstEnemyUnitInRange(CvUnit * pUnit, bool bMustBeAbleToKill)
{
	if (!pUnit)
		return NULL;

	ReachablePlots plots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
	for (ReachablePlots::iterator it = plots.begin(); it != plots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		//works for melee
		if (pPlot->isEnemyUnit(pUnit->getOwner(), true, true))
		{
			if (bMustBeAbleToKill)
			{
				//see how the attack would go
				CvUnit* pDefender = pPlot->getVisibleEnemyDefender(pUnit->getOwner());
				int iDamageDealt = 0, iDamageReceived = 0;
				iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pUnit, pDefender->plot(), pUnit->plot(), iDamageReceived);

				if (iDamageDealt > iDamageReceived && iDamageDealt > pDefender->GetCurrHitPoints() && pUnit->GetCurrHitPoints() - iDamageReceived > pUnit->GetMaxHitPoints() / 2)
					return pPlot;
			}
			else
				return pPlot;
		}

		//special for ranged units who can't enter enemy occupied plots
		//quite inefficient so only check the first ring
		if (it->iMovesLeft > 0 && pUnit->isRanged())
		{
			for (int i = RING0_PLOTS; i < RING1_PLOTS; i++)
			{
				CvPlot* pPlot2 = iterateRingPlots(pPlot, i);
				if (pPlot2 && pPlot2->isEnemyUnit(pUnit->getOwner(), true, true))
				{
					if (bMustBeAbleToKill)
					{
						//see how the attack would go
						CvUnit* pDefender = pPlot2->getVisibleEnemyDefender(pUnit->getOwner());
						int iDamageDealt = 0, iDamageReceived = 0;
						iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pUnit, pDefender->plot(), pUnit->plot(), iDamageReceived);

						if (iDamageDealt > pDefender->GetCurrHitPoints())
							return pPlot2;
					}
					else
						return pPlot2;
				}
			}
		}
	}

	return NULL;
}

#if defined(MOD_CORE_NEW_DEPLOYMENT_LOGIC) 

//Unbelievable bad logic but taken like this from CvUnitCombat
bool AttackEndsTurn(const CvUnit* pUnit, int iNumAttacksLeft)
{
	if(!pUnit->canMoveAfterAttacking() && !pUnit->isRangedSupportFire() && iNumAttacksLeft<1)
		return true;

	return false;
}

int NumAttacksForUnit(int iMovesLeft, int iMaxAttacks)
{
	return max(0, min( (iMovesLeft+GC.getMOVE_DENOMINATOR()-1)/GC.getMOVE_DENOMINATOR(), iMaxAttacks ));
}

void ScoreAttack(const CvTacticalPlot& tactPlot, CvUnit* pUnit, const CvTacticalPlot& assumedPlot, eAggressionLevel eAggLvl, float fUnitNumberRatio, STacticalAssignment& result)
{
	int iDamageDealt = 0;
	int iDamageReceived = 0; //always zero for ranged attack
	int iExtraDamage = 0; //splash damage and other bonuses

	//the damage calculation doesn't know about hypothetical flanking units, so we ignore it here and add it ourselves 
	int iPrevDamage = tactPlot.getDamage();
	int iPrevHitPoints = 0;

	CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(assumedPlot.getPlotIndex());
	CvPlot* pTestPlot = GC.getMap().plotByIndexUnchecked(tactPlot.getPlotIndex());

	bool bFlankModifierOffensive = false;
	bool bFlankModifierDefensive = false;

	if (tactPlot.isEnemyCity()) //a plot can be both a city and a unit - in that case we would attack the city
	{
		CvCity* pEnemy = pTestPlot->getPlotCity();
		iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pEnemy, pUnit, pUnitPlot, iDamageReceived, true, iPrevDamage);
		iExtraDamage = pUnit->GetRangeCombatSplashDamage(pTestPlot);
		iPrevHitPoints = pEnemy->GetMaxHitPoints() - pEnemy->getDamage() - iPrevDamage;

		//city blockaded? not 100% accurate, but anyway
		if (tactPlot.getNumAdjacentFriendlies()==pTestPlot->countPassableNeighbors(NO_DOMAIN))
			iDamageDealt += iDamageDealt/5;

		//special: if there are multiple enemy units around, try to attack those first
		if (tactPlot.getNumAdjacentEnemies()>1)
			iDamageDealt /= 2;
	}
	else if (tactPlot.isEnemyCombatUnit())
	{
		CvUnit* pEnemy = pTestPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(), pUnit, true, true); //ignore the official visibility
		if (!pEnemy)
		{
			OutputDebugString("expected enemy unit not present!\n");
			result.iScore = -1;
			return;
		}
		iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pEnemy, pUnit, pTestPlot, pUnitPlot, iDamageReceived, true, iPrevDamage);
		iExtraDamage = pUnit->GetRangeCombatSplashDamage(pTestPlot);
		iPrevHitPoints = pEnemy->GetCurrHitPoints() - iPrevDamage;

		bFlankModifierDefensive = pEnemy->GetFlankAttackModifier() > 0;
		bFlankModifierOffensive = pUnit->GetFlankAttackModifier() > 0;
	}

	//fake general bonus
	if (assumedPlot.hasSupportBonus())
	{
		iDamageDealt += iDamageDealt/10;
		iDamageReceived -= iDamageReceived/10;
	}
	
	//problem is flanking affects combat strength, not damage, so the effect is nonlinear. anyway just assume 10%
	if (bFlankModifierOffensive && tactPlot.getNumAdjacentFriendlies()>1 ) //we need a second friendly. the first one is us!
	{
		iDamageDealt += iDamageDealt/10;
		iDamageReceived -= iDamageReceived/10;
	}

	//flanking bonus for enemy. if both apply we simply assume they cancel each other
	if (bFlankModifierDefensive && tactPlot.getNumAdjacentEnemies()>0 )
	{
		iDamageDealt -= iDamageDealt/10;
		iDamageReceived += iDamageReceived/10;
	}

	//bonus for a kill
	if (iDamageDealt >= iPrevHitPoints)
	{
		if (tactPlot.isEnemyCity()) //city capture
		{
			//ranged units can't capture, so discourage the attack
			if (pUnit->isRanged())
			{
				result.eType = STacticalAssignment::A_RANGEATTACK; //not a kill!
				//don't continue flogging a dead horse
				if ( iPrevHitPoints < 2 )
					iDamageDealt = 0;
			}
			else
			{
				iDamageDealt = 108; //more than any unit except if it's a perfect kill
				iExtraDamage += 100; //capturing a city is important
				result.eType = STacticalAssignment::A_MELEEKILL;
			}
		}
		else //enemy unit killed
		{
			//don't hand out points for over-killing (but make an allowance for randomness)
			iDamageDealt = min(iDamageDealt, iPrevHitPoints + 10);

			iExtraDamage += 30;
			if (pTestPlot->isEnemyUnit(pUnit->getOwner(),false,false))
				iExtraDamage += 20; //even more points for a double kill
			if (pUnit->getHPHealedIfDefeatEnemy() > 0)
				iDamageReceived = max(0, iDamageReceived - pUnit->getHPHealedIfDefeatEnemy());

			result.eType = pUnit->isRanged() ? STacticalAssignment::A_RANGEATTACK : STacticalAssignment::A_MELEEATTACK;
		}
	}
	else
		result.eType = pUnit->isRanged() ? STacticalAssignment::A_RANGEATTACK : STacticalAssignment::A_MELEEATTACK;

	//finally the almighty score
	float fAggFactor = -1;
	int iHPbelowHalf = pUnit->GetMaxHitPoints()/2 - (pUnit->GetCurrHitPoints() - iDamageReceived);
	switch (eAggLvl)
	{
	case AL_LOW:
		fAggFactor = 0.5f;
		if ( iHPbelowHalf>0 )
			iExtraDamage -= 4 * min(iDamageReceived,iHPbelowHalf);
		break;
	case AL_MEDIUM:
		fAggFactor = 1.1f;
		if ( iHPbelowHalf>0 )
			iExtraDamage -= 2 * min(iDamageReceived,iHPbelowHalf);
		break;
	case AL_HIGH:
		if ( iDamageReceived < pUnit->GetCurrHitPoints() || fUnitNumberRatio>1 )
			fAggFactor = 4.2f;
		break;
	}

	//if the score is negative we don't do it. add previous damage again and again to make concentrated fire attractive
	//todo: consider pEnemy->getUnitInfo().GetProductionCost() and pEnemy->GetBaseCombatStrength()
	if (fAggFactor>0 && iDamageReceived<pUnit->GetCurrHitPoints())
	{
		result.iScore = int(iDamageDealt*fUnitNumberRatio*fAggFactor+0.5) - iDamageReceived + iExtraDamage; 
		result.iDamage = iDamageDealt;
	}
	else
		result.iScore = -1;
}

bool IsEnemyCitadel(CvPlot* pPlot, TeamTypes eMyTeam)
{
	if (!pPlot || eMyTeam==NO_TEAM)
		return false;

	ImprovementTypes eImprovement = pPlot->getImprovementType();

	// Citadel here?
	if (eImprovement != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
	{
		int iDamage = GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage();

		return (iDamage != 0) && (pPlot->getOwner() != NO_PLAYER) && GET_TEAM(eMyTeam).isAtWar(pPlot->getTeam());
	}
	
	return false;
}

STacticalAssignment ScorePlotForCombatUnit(const SUnitStats unit, SMovePlot plot, const CvTacticalPosition& assumedPosition, bool bForRangedAttack)
{
	//default action is invalid
	STacticalAssignment result(unit.iPlotIndex,plot.iPlotIndex,unit.iUnitID,plot.iMovesLeft,unit.isCombatUnit(),-1,STacticalAssignment::A_FINISH);

	CvPlayer& kPlayer = GET_PLAYER(assumedPosition.getPlayer());
	CvUnit* pUnit = kPlayer.getUnit(unit.iUnitID);
	if (!pUnit)
		return result;
		
	//how often can we attack this turn
	int iMaxAttacks = unit.iAttacksLeft;
	if (unit.iMovesLeft == 0)
		iMaxAttacks = 0;

	//the plot we're checking right now
	CvPlot* pCurrentPlot = GC.getMap().plotByIndexUnchecked(plot.iPlotIndex);
	const CvTacticalPlot& currentPlot = assumedPosition.getTactPlot(plot.iPlotIndex);
	if (!currentPlot.isValid())
		return result;

	//note: assumed unit plot may be invalid if the unit is initially far away from the target
	CvPlot* pAssumedUnitPlot = GC.getMap().plotByIndexUnchecked(unit.iPlotIndex);
	CvTacticalPlot assumedUnitPlot = assumedPosition.getTactPlot(unit.iPlotIndex);
	if (!assumedUnitPlot.isValid()) //create a temporary so that ScoreAttack works
		assumedUnitPlot = CvTacticalPlot(pAssumedUnitPlot,assumedPosition.getPlayer());

	//this is only for melee attacks - ranged attacks are handled separately
	if (currentPlot.isEnemy())
	{
		//don't attack cities if the real target is something else
		if (currentPlot.isEnemyCity() && assumedPosition.getTarget()!=pCurrentPlot)
			return result;

		int iAttacksHereThisTurn = NumAttacksForUnit(unit.iMovesLeft,iMaxAttacks);
		//assume a single attack. if we have multiple attacks this will be called again
		if (iAttacksHereThisTurn > 0)
		{
			//check how much damage we could do - this is a melee attack, works only if we're adjacent
			int iDistance = plotDistance(*pAssumedUnitPlot,*pCurrentPlot);
			if (iDistance == 1)
			{
				ScoreAttack(currentPlot, pUnit, assumedUnitPlot, assumedPosition.getAggressionLevel(), assumedPosition.getUnitNumberRatio(), result);
				if (result.iScore<0)
					return result;

				//what happens next?
				if (AttackEndsTurn(pUnit,iMaxAttacks-1))
					result.iRemainingMoves = 0;
				else
					result.iRemainingMoves -= min(result.iRemainingMoves, GC.getMOVE_DENOMINATOR());

				//don't break formation if there are many enemies around
				if (result.eType == STacticalAssignment::A_MELEEKILL && currentPlot.getNumAdjacentEnemies()>3)
					return result;

				if ( pCurrentPlot==assumedPosition.getTarget() || IsEnemyCitadel(pCurrentPlot,kPlayer.getTeam()) )
					result.iScore += 8; //a slight boost for attacking the "real" target or a citadel
				else
					result.iScore += 5; //a bonus to make attacks more competive with moves - each frontline plot gets a +8

				//combo bonus
				if (result.eType==STacticalAssignment::A_MELEEKILL && currentPlot.isEnemyCivilian())
					result.iScore += 5;
			}
		}
	}
	else if (currentPlot.isEnemyCivilian()) //unescorted civilian
	{
		if (plot.iMovesLeft > 0 || currentPlot.getNumAdjacentEnemies()==0)
		{
			CvUnit* pCivilian = pCurrentPlot->getUnitByIndex(0); //there should be only one ...
			if (pCivilian)
			{
				//workers are not so important ...
				result.iScore = (pCivilian->AI_getUnitAIType() == UNITAI_WORKER) ? 5 : 20;
				result.eType = STacticalAssignment::A_CAPTURE;
				result.iRemainingMoves = plot.iMovesLeft;
			}
		}
	}
	else //empty plot or friendly unit. the latter case will be handled by the isMoveBlockedByOtherUnit check
	{
		//check if this is actual movement or whether we simply end the turn
		if (plot.iPlotIndex != unit.iPlotIndex || bForRangedAttack)
		{
			result.eType = STacticalAssignment::A_MOVE;

			//prevent two moves in a row, that is inefficient and can lead to "shuttling" behavior
			//we only consider plots which can be reached in one turn anyway. parthian moves still work.
			if (!bForRangedAttack && unit.eLastAssignment == STacticalAssignment::A_MOVE)
				return result;
		}

		//stay on target. hard cutoff!
		int iPlotDistance = plotDistance(*assumedPosition.getTarget(),*pCurrentPlot);
		if (iPlotDistance>3)
			return result;

		//don't do it if it's a death trap
		if (currentPlot.getNumAdjacentEnemies()>3)
			return result;

		//lookup by unit strategy / plot type
		int iDistanceScores[5][5] = {
			{ -1,-1,-1,-1,-1 }, //none (should not occur)
			{ -1,-1, 8, 2, 1 }, //firstline
			{ -1,-1, 2, 8, 1 }, //secondline
			{ -1,-1, 1, 4, 8 }, //thirdline
			{ -1,-1, 1, 1, 1 }, //support (should not occur)
		};

		int iDistanceScore = iDistanceScores[unit.eStrategy][currentPlot.getType()];
		//parthian moves: no use staying close after we made our attack
		if (unit.iMovesLeft>0 && iMaxAttacks==0)
			iDistanceScore = currentPlot.getNumAdjacentEnemies() > 0 ? -1 : 2;

		if (iDistanceScore<0)
			return result;

		//check all plots we could possibly attack from here
		std::vector<int> vDamageRatios;
		std::vector<CvPlot*> vAttackPlots;

		//works for both melee and ranged
		int iMaxRange = pUnit->isRanged() ? pUnit->GetRange() : 1;
		for (int iRange=1; iRange<iMaxRange+1; iRange++)
		{
			//performance optimization
			if (iRange==1 && currentPlot.getType()!=CvTacticalPlot::TP_FRONTLINE)
				continue;

			pCurrentPlot->GetPlotsAtRangeX(iRange,true,!pUnit->IsRangeAttackIgnoreLOS(),vAttackPlots);
			for(size_t iCount=0; iCount<vAttackPlots.size(); iCount++)
			{
				CvPlot* pLoopPlot = vAttackPlots[iCount];
				if (!pLoopPlot)
					continue;

				const CvTacticalPlot& enemyPlot = assumedPosition.getTactPlot(pLoopPlot->GetPlotIndex());
				if (enemyPlot.isValid() && enemyPlot.isEnemy())
				{
					//don't attack cities if the real target is something else
					if (enemyPlot.isEnemyCity() && assumedPosition.getTarget()!=pLoopPlot)
						continue;

					//we don't care for damage here but let's reuse the scoring function
					STacticalAssignment temp; 
					ScoreAttack(enemyPlot, pUnit, assumedUnitPlot, assumedPosition.getAggressionLevel(), assumedPosition.getUnitNumberRatio(), temp);

					if (temp.iScore>0)
					{
						//don't break formation if there are many enemies around
						if (temp.eType == STacticalAssignment::A_MELEEKILL && enemyPlot.getNumAdjacentEnemies()>3)
							continue;

						vDamageRatios.push_back(temp.iScore);
					}
					else 
						//for melee units the attack might be cancelled because of bad damage ratio. 
						//nevertheless we need to cover our ranged units. so add a token amount.
						vDamageRatios.push_back(4); //4 so that something remains after the /4 below
				}
			}
		}

		int iDamageScore = 0;
		if (!vDamageRatios.empty())
		{
			//the best target comes last
			std::sort(vDamageRatios.begin(), vDamageRatios.end());

			//how often can we attack?
			size_t iAttacksHereThisTurn = (size_t)NumAttacksForUnit(plot.iMovesLeft, iMaxAttacks);
			if (result.eType == STacticalAssignment::A_FINISH)
				iAttacksHereThisTurn = 0;

			while (vDamageRatios.size()<iAttacksHereThisTurn)
				vDamageRatios.push_back( vDamageRatios.back() );

			//this is a bit tricky, for simplicity assume we attack each taget only once
			//important: we also score targets which can only be attacked next turn!
			std::reverse(vDamageRatios.begin(), vDamageRatios.end());
			for (size_t i = 0; i<vDamageRatios.size(); i++)
			{
				if (i < iAttacksHereThisTurn)
					iDamageScore += vDamageRatios[i];
				else if (i < iAttacksHereThisTurn+pUnit->getNumAttacks())
					//if we can't attack now, we don't like it as much
					iDamageScore += vDamageRatios[i] / 4;
			}

			//ranged specialties (don't check range because it's about skirmishers as well)
			if (pUnit->isRanged())
			{
				if (iAttacksHereThisTurn>0)
				{
					//try to stay away from enemies if we can attack from afar
					if (currentPlot.getNumAdjacentEnemies()>0 && iMaxRange>1)
						iDamageScore /= 4;

					//ranged units are vulnerable without melee
					if (currentPlot.getNumAdjacentFirstlineFriendlies()==0)
						iDamageScore /= 4;
				}
				else if (currentPlot.getNumAdjacentEnemies()>0 || currentPlot.getNumAdjacentFirstlineFriendlies()==0)
					//ranged units don't like to park next to enemies at all if we cannot attack
					iDamageScore = 0;
			}
			else //melee
			{
				//we exclude plots with >3 enemies around anyway. three is a corner case
				if (currentPlot.getNumAdjacentEnemies() == 3)
					iDamageScore /= 2;
			}

			//isolated unit? bad. chicken and egg problem for the algorithm though.
			if (currentPlot.getNumAdjacentFriendlies()==0)
				iDamageScore /= 2;
		}

		//temporary score
		result.iScore = iDamageScore + iDistanceScore;

		//when in doubt use the plot with better sight (maybe check seeFromLevel instead?)
		if (pCurrentPlot->isHills() || pCurrentPlot->isMountain())
			result.iScore++;

		//does it make sense to pillage here?
		if (result.eType == STacticalAssignment::A_FINISH && unit.iMovesLeft > 0 && pUnit->canPillage(pCurrentPlot) && pUnit->getDamage()>10)
		{
			//can only pillage once per turn
			if (!assumedPosition.unitHasAssignmentOfType(unit.iUnitID, STacticalAssignment::A_PILLAGE))
			{
				if (pUnit->getDamage() > GC.getPILLAGE_HEAL_AMOUNT())
					result.iScore += 10;
				else if (IsEnemyCitadel(pCurrentPlot, kPlayer.getTeam()))
					result.iScore += 50;
				else if (pUnit->IsGainsXPFromPillaging())
					result.iScore += 25;

				result.iRemainingMoves -= min(result.iRemainingMoves, GC.getMOVE_DENOMINATOR());
				result.eType = STacticalAssignment::A_PILLAGE;
			}
		}

		//minor bonus for staying put and healing
		if (result.eType == STacticalAssignment::A_FINISH && unit.iMovesLeft == pUnit->maxMoves())
			result.iScore++;
	}

	//will we end the turn here?
	bool bEndTurn = (result.iToPlotIndex == unit.iPlotIndex) || result.iRemainingMoves == 0;

	//try to close the lines (todo: make sure the friendlies intend to stay there ...)
	if (bEndTurn && currentPlot.getNumAdjacentFriendlies()>0)
		result.iScore++;

	//the danger value reflects any defensive terrain bonuses
	int iDanger = bEndTurn ? pUnit->GetDanger(pCurrentPlot,assumedPosition.getKilledEnemies()) : 0;

	//can happen with garrisons
	if (iDanger==INT_MAX)
	{
		result.iScore = 1;
		return result;
	}

	//extra careful if we're over the magical threshold
	if (iDanger > pUnit->GetCurrHitPoints())
		iDanger += 10;

	//try to be more careful with highly promoted units
	iDanger += (pUnit->getExperienceTimes100()-kPlayer.GetAvgUnitExp100()) / 1000;

	//penalty for high danger plots
	//todo: take into account self damage from previous attacks
	int iDangerScore = (iDanger*30)/(pUnit->GetCurrHitPoints()+1);

	//todo: take into account mobility at the proposed plot
	//todo: take into account ZOC when ending the turn

	//adjust the score - danger values are mostly useless but maybe useful as tiebreaker 
	//add a flat base value so that bad moves are not automatically invalid - sometimes all moves are bad
	result.iScore = result.iScore*10 - iDangerScore + 20;
	return result;
}

//stacking with combat units is allowed here!
STacticalAssignment ScorePlotForSupportUnit(const SUnitStats unit, const SMovePlot plot, const CvTacticalPosition& assumedPosition)
{
	//default action
	STacticalAssignment result(unit.iPlotIndex, plot.iPlotIndex, unit.iUnitID, plot.iMovesLeft, unit.isCombatUnit(), -1, STacticalAssignment::A_FINISH);

	//the plot we're checking right now
	const CvTacticalPlot& tactPlot = assumedPosition.getTactPlot(plot.iPlotIndex);
	if (!tactPlot.isValid())
		return result;

	//todo: take into account sight / mobility from plot

	int iScore = 0;
	switch (tactPlot.getType())
	{
	case CvTacticalPlot::TP_ENEMY:
		iScore = -1; //don't ever go there, wouldn't work anyway
		break;
	case CvTacticalPlot::TP_FRONTLINE:
		iScore = tactPlot.isFriendlyCombatUnit() ? 2 : -1; //dangerous, only in emergencies
		break;
	case CvTacticalPlot::TP_SECONDLINE:
	case CvTacticalPlot::TP_THIRDLINE:
		if (tactPlot.isFriendlyCombatUnit())
			iScore = 24; //nice. different to first line is worth more than two adjacent units
		else if (tactPlot.getNumAdjacentFriendlies() > 0)
			iScore = 3; //dangerous
		else
			iScore = -1; //nobody around? don't go there
		break;
	case CvTacticalPlot::TP_FARAWAY:
		iScore = 1;	//possible but not really useful
		break;
	default:
		iScore = 10;
	}

	if (plot.iPlotIndex != unit.iPlotIndex)
	{
		result.eType = STacticalAssignment::A_MOVE;
		result.iRemainingMoves = plot.iMovesLeft;
	}

	//points for supported units
	//todo: make sure they intend to stay there. movement phase and attack phase?
	if (iScore>0)
	{
		iScore += 10*tactPlot.getNumAdjacentFriendlies();

		//avoid overlap. this works only because we eliminated our own aura before calling this function!
		if (tactPlot.hasSupportBonus())
			iScore/=2;

		//if we intend to stay there, there must be a defender which intends to stay
		if (plot.iMovesLeft == 0 || plot.iPlotIndex == unit.iPlotIndex)
		{
			CvUnit* pUnit = GET_PLAYER(assumedPosition.getPlayer()).getUnit(unit.iUnitID);
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(plot.iPlotIndex);
			
			if (!pPlot->isFriendlyCity(*pUnit,true)) //check only applies outside of cities
			{
				STacticalAssignment defenderAssignment = assumedPosition.findBlockingUnitAtPlot(tactPlot.getPlotIndex());
				if (defenderAssignment.iUnitID == 0 || defenderAssignment.iRemainingMoves > 0)
					iScore = 0;
			}
		}
	}

	result.iScore = iScore;
	return result;
}

CvTacticalPlot::CvTacticalPlot(const CvPlot* plot, PlayerTypes ePlayer)
{
	bBlockedByEnemyCity = false;
	bBlockedByEnemyCombatUnit = false;
	bEnemyCivilianPresent = false;
	bBlockedByFriendlyCombatUnit = false;
	bSupportUnitPresent = false;
	bValid = false;
	iDamageDealt = 0;
	eType = TP_FARAWAY;
	pPlot = NULL;
	nEnemyCombatUnitsAdjacent = 0;
	nFriendlyCombatUnitsAdjacent = 0;
	nFriendlyFirstlineUnitsAdjacent = 0;
	nSupportUnitsAdjacent = 0;

	setInitialState(plot,ePlayer);
}

void CvTacticalPlot::setInitialState(const CvPlot* plot, PlayerTypes ePlayer)
{
	//initial values are "real", will be updated later on
	if (plot && ePlayer!=NO_PLAYER)
	{
		bValid=true;
		pPlot=plot;
		nEnemyCombatUnitsAdjacent = pPlot->GetNumEnemyUnitsAdjacent(GET_PLAYER(ePlayer).getTeam(),pPlot->getDomain());
		nFriendlyCombatUnitsAdjacent = pPlot->GetNumFriendlyUnitsAdjacent(GET_PLAYER(ePlayer).getTeam(),pPlot->getDomain());
		nFriendlyFirstlineUnitsAdjacent = 0; //don't know this yet

		//embarked units don't count as combat unit ...
		CvUnit* pDefender = pPlot->getBestDefender(ePlayer);
		bBlockedByFriendlyCombatUnit = (pDefender != NULL && !pDefender->isEmbarked());

		bBlockedByEnemyCombatUnit = pPlot->isEnemyUnit(ePlayer,true,false); //visibility is checked elsewhere!
		bBlockedByEnemyCity = (pPlot->isCity() && GET_PLAYER(ePlayer).IsAtWarWith(pPlot->getOwner()));
		bEnemyCivilianPresent = !bBlockedByEnemyCombatUnit && pPlot->isEnemyUnit(ePlayer,false,false); //visibility is checked elsewhere!

		//general handling is a bit awkward
		bSupportUnitPresent = (pPlot->getNumUnitsOfAIType(UNITAI_GENERAL,ePlayer) + pPlot->getNumUnitsOfAIType(UNITAI_ADMIRAL,ePlayer))>0;
		nSupportUnitsAdjacent = 0;
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
		for (int i = 0; i < 6; i++)
		{
			CvPlot* pNeighbor = aNeighbors[i];
			if (pNeighbor && (pPlot->getNumUnitsOfAIType(UNITAI_GENERAL,ePlayer)+pPlot->getNumUnitsOfAIType(UNITAI_ADMIRAL,ePlayer))>0)
				nSupportUnitsAdjacent++;
		}

		//plot type is set later
	}
}

void CvTacticalPlot::changeNeighboringUnitCount(CvTacticalPosition& currentPosition, bool bCombat, int iChange)
{
	if (!pPlot)
		return;

	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for (int i = 0; i < 6; i++)
	{
		CvPlot* pNeighbor = aNeighbors[i];
		if (pNeighbor)
		{
			CvTacticalPlot& tactPlot = currentPosition.getTactPlot(pNeighbor->GetPlotIndex());
			if (tactPlot.isValid())
			{
				if (bCombat)
					tactPlot.nFriendlyCombatUnitsAdjacent += iChange;
				else
					tactPlot.nSupportUnitsAdjacent += iChange;
			}
		}
	}
}

void CvTacticalPlot::friendlyUnitMovingIn(CvTacticalPosition& currentPosition, bool bFriendlyUnitIsCombat)
{
	bBlockedByEnemyCombatUnit = false;
	bBlockedByEnemyCity = false;
	bEnemyCivilianPresent = false;

	if (bFriendlyUnitIsCombat)
		bBlockedByFriendlyCombatUnit = true;

	if (!bFriendlyUnitIsCombat) //multiple support units per plot should not happen ...
	{
		bSupportUnitPresent = true;
		changeNeighboringUnitCount(currentPosition, false, +1);
	}
}

void CvTacticalPlot::friendlyUnitMovingOut(CvTacticalPosition& currentPosition, bool bFriendlyUnitIsCombat)
{
	if (bFriendlyUnitIsCombat)
		bBlockedByFriendlyCombatUnit = false;

	if (!bFriendlyUnitIsCombat)
	{
		bSupportUnitPresent = false;
		changeNeighboringUnitCount(currentPosition, false, -1);
	}
}

void CvTacticalPlot::enemyUnitRangeKill()
{
	bBlockedByEnemyCombatUnit = false;
}

void CvTacticalPlot::findType(const CvTacticalPosition& currentPosition, set<int>& outstandingUpdates)
{
	eTactPlotType eOldType = eType;
	nFriendlyCombatUnitsAdjacent = 0;
	nEnemyCombatUnitsAdjacent = 0;
	nFriendlyFirstlineUnitsAdjacent = 0;
	
	bool bEnemyCityAdjacent = false;
	int iFL = 0, iSL = 0, iTL = 0;
	
	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for (int i = 0; i < 6; i++)
	{
		CvPlot* pNeighbor = aNeighbors[i];
		if (pNeighbor)
		{
			const CvTacticalPlot& tactPlot = currentPosition.getTactPlot(pNeighbor->GetPlotIndex());
			if (tactPlot.isValid())
			{
				if (tactPlot.bBlockedByFriendlyCombatUnit)
					nFriendlyCombatUnitsAdjacent++;
				if (tactPlot.bBlockedByEnemyCombatUnit)
					nEnemyCombatUnitsAdjacent++;
				if (tactPlot.bBlockedByEnemyCity)
					bEnemyCityAdjacent=true;

				if (tactPlot.getType() == TP_FRONTLINE)
					iFL++;
				if (tactPlot.getType() == TP_SECONDLINE)
					iSL++;
				if (tactPlot.getType() == TP_THIRDLINE)
					iTL++;

				//let's hope it's the right kind of unit
				if (tactPlot.getType() == TP_FRONTLINE && tactPlot.bBlockedByFriendlyCombatUnit)
					nFriendlyFirstlineUnitsAdjacent++;
			}
		}
	}

	if (bBlockedByEnemyCombatUnit || bBlockedByEnemyCity)
		eType = TP_ENEMY;
	else if (nEnemyCombatUnitsAdjacent>0 || bEnemyCityAdjacent)
		eType = TP_FRONTLINE;
	else if (iFL>0)
		eType = TP_SECONDLINE;
	else if (iSL>0)
		eType = TP_THIRDLINE;
	else
		eType = TP_FARAWAY;
	
	//we we changed our status, maybe our neighbors need to switch too
	if (eType != eOldType)
	{
		for (int i = 0; i < 6; i++)
		{
			CvPlot* pNeighbor = aNeighbors[i];
			if (pNeighbor)
				outstandingUpdates.insert( pNeighbor->GetPlotIndex() );
		}
	}
}

bool CvTacticalPosition::getRangeAttackPlotsForUnit(int iUnit, set<int>& out) const
{
	map<int,set<int>>::const_iterator localResult = rangeAttackPlotLookup.find(iUnit);
	if (localResult != rangeAttackPlotLookup.end())
	{
		out = localResult->second;
		return true;
	}
	else if (parentPosition && parentPosition->getRangeAttackPlotsForUnit(iUnit, out))
		return true;

	return false;
}

bool CvTacticalPosition::getReachablePlotsForUnit(int iUnit, ReachablePlots& out) const
{
	map<int,ReachablePlots>::const_iterator localResult = reachablePlotLookup.find(iUnit);
	if (localResult != reachablePlotLookup.end())
	{
		out = localResult->second;
		return true;
	}
	else if (parentPosition && parentPosition->getReachablePlotsForUnit(iUnit, out))
		return true;

	return false;
}

vector<STacticalAssignment> CvTacticalPosition::getPreferredAssignmentsForUnit(SUnitStats unit, int nMaxCount) const
{
	vector<STacticalAssignment> possibleMoves;

	CvUnit* pUnit = GET_PLAYER(getPlayer()).getUnit(unit.iUnitID);
	if (!pUnit)
		return vector<STacticalAssignment>();

	//moves and melee attacks
	ReachablePlots reachablePlots;
	getReachablePlotsForUnit(unit.iUnitID,reachablePlots);

	for (ReachablePlots::const_iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
	{
		STacticalAssignment move;
		switch (unit.eStrategy)
		{
		case SUnitStats::MS_FIRSTLINE:
		case SUnitStats::MS_SECONDLINE:
		case SUnitStats::MS_THIRDLINE:
			move = ScorePlotForCombatUnit(unit, *it, *this, false); //ranged attacks handled below
			break;
		case SUnitStats::MS_SUPPORT:
			move = ScorePlotForSupportUnit(unit, *it, *this);
			break;
		}

		if (move.iScore>0)
			possibleMoves.push_back( move );
	}

	//ranged attacks
	if (unit.iAttacksLeft>0 && unit.iMovesLeft>0)
	{
		//in case we need to stay here after attacking
		SUnitStats unitAfterAttack(unit);
		unitAfterAttack.iMovesLeft = 0;
		ReachablePlots::iterator it = reachablePlots.find(unit.iPlotIndex);

		//this should definitely not happen
		if (it == reachablePlots.end())
			return vector<STacticalAssignment>();

		SMovePlot unitPlot = *it;
		int endTurnMoveScore = ScorePlotForCombatUnit(unitAfterAttack, unitPlot, *this, true).iScore;

		set<int> rangeAttackPlots;
		getRangeAttackPlotsForUnit(unit.iUnitID, rangeAttackPlots);
		for (set<int>::const_iterator it=rangeAttackPlots.begin(); it!=rangeAttackPlots.end(); ++it)
		{
			STacticalAssignment move(unit.iPlotIndex,*it,unit.iUnitID,unit.iMovesLeft,unit.isCombatUnit(),-1,STacticalAssignment::A_FINISH);

			if (unit.isCombatUnit())
			{
				//the plot we're checking right now
				const CvTacticalPlot& assumedUnitPlot = getTactPlot(unit.iPlotIndex);
				const CvTacticalPlot& currentPlot = getTactPlot(*it);

				//don't attack cities if the real target is something else
				if (currentPlot.isEnemyCity() && getTarget()->GetPlotIndex() != currentPlot.getPlotIndex())
					continue;

				if (currentPlot.isValid() && currentPlot.isEnemy() && assumedUnitPlot.isValid()) //still alive?
				{
					//received damage is zero here but still use the correct unit number ratio so as not to distort scores
					ScoreAttack(currentPlot, pUnit, assumedUnitPlot, getAggressionLevel(), getUnitNumberRatio(), move);
					if (move.iScore<0)
						continue;

					//what happens next?
					if (AttackEndsTurn(pUnit,unit.iAttacksLeft-1))
						move.iRemainingMoves = 0;
					else
						move.iRemainingMoves -= min(move.iRemainingMoves, GC.getMOVE_DENOMINATOR());

					//if we would need to stay here but it's a bad idea, then don't do the attack
					if (move.iRemainingMoves == 0 && endTurnMoveScore < 0 && getAggressionLevel()<AL_HIGH)
						continue;

					if ( *it==getTarget()->GetPlotIndex() )
						move.iScore += 3; //a slight boost for attacking the "real" target

					//times 10 to match with ScorePlotForCombatUnit() and always better than ending the turn!
					move.iScore = move.iScore*10 + endTurnMoveScore;
				}

				//generate a move for each possible attack. if we have multiple attacks, we will call this function again
				if (move.iScore>0)
					possibleMoves.push_back( move );
			}
		}

		//optimization: if we added a range attack then the end turn move will be generated automatically and the explicit end turn move is not needed
		if (!possibleMoves.empty() && (possibleMoves.back().eType==STacticalAssignment::A_RANGEATTACK || possibleMoves.back().eType == STacticalAssignment::A_RANGEKILL))
			//end turn should always be first because of the ordering inside reachablePlots
			if (possibleMoves.front().eType==STacticalAssignment::A_FINISH)
				possibleMoves.erase(possibleMoves.begin());
	}

	//note: we don't filter out blocked moves. the unit will be considered blocked in this case, even if we have valid but bad moves
	std::sort(possibleMoves.begin(),possibleMoves.end());

	if (possibleMoves.size()>(size_t)nMaxCount)
		possibleMoves.erase( possibleMoves.begin()+nMaxCount, possibleMoves.end() );
	
	//don't return moves which are known to be suboptimal
	while (!possibleMoves.empty() && possibleMoves.back().iScore < possibleMoves.front().iScore/4)
		possibleMoves.pop_back();

	return possibleMoves;
}

bool CvTacticalPosition::makeNextAssignments(int iMaxBranches, int iMaxAssignmentsPerBranch)
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

	vector<STacticalAssignment> overAllChoices;
	map<int,vector<STacticalAssignment>> choicePerUnit;
	map<int,vector<STacticalAssignment>> newAssignments;
	
	for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
	{
		vector<STacticalAssignment> thisUnitChoices;
		CvTacticalPlot& currentPlot = getTactPlot(itUnit->iPlotIndex);

		//remove / restore aura for unbiased placement
		if (itUnit->eStrategy==SUnitStats::MS_SUPPORT)
		{
			currentPlot.changeNeighboringUnitCount(*this, false, -1);
			thisUnitChoices = getPreferredAssignmentsForUnit(*itUnit,8);
			currentPlot.changeNeighboringUnitCount(*this, false, +1);
		}
		else
		{
			currentPlot.changeNeighboringUnitCount(*this, true, -1);
			thisUnitChoices = getPreferredAssignmentsForUnit(*itUnit, 8);
			currentPlot.changeNeighboringUnitCount(*this, true, +1);
		}

		//oops we're blocked with no valid move
		if (thisUnitChoices.empty())
			thisUnitChoices.push_back( STacticalAssignment(itUnit->iPlotIndex,itUnit->iPlotIndex,itUnit->iUnitID,0,itUnit->isCombatUnit(),0,STacticalAssignment::A_BLOCKED) );

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
		if (itMove->eType==STacticalAssignment::A_BLOCKED)
			continue;

		if (isMoveBlockedByOtherUnit(*itMove))
		{
			int iUnitID = findBlockingUnitAtPlot(itMove->iToPlotIndex).iUnitID;
			if (iUnitID==0 || iUnitID==itMove->iUnitID)
			{
				OutputDebugString("invalid block!\n");
				continue; //should not happen!
			}

			//find best non-blocked move for blocking unit (search only one level deep)
			vector<STacticalAssignment> blockingUnitChoices = choicePerUnit[ iUnitID ];
			for (vector<STacticalAssignment>::iterator itMove2 = blockingUnitChoices.begin(); itMove2 != blockingUnitChoices.end(); ++itMove2)
			{
				if (!isMoveBlockedByOtherUnit(*itMove2) && itMove2->eType==STacticalAssignment::A_MOVE)
				{
					//add the move to make space, then add the original move
					movesToAdd.push_back(*itMove2);
					movesToAdd.push_back(*itMove);
					break;
				}
			}

			//oops we're blocked with no valid move
			if (movesToAdd.empty())
				movesToAdd.push_back( STacticalAssignment(itMove->iFromPlotIndex,itMove->iFromPlotIndex,itMove->iUnitID,0,itMove->bIsCombat,0,STacticalAssignment::A_BLOCKED) );
		}
		else
			movesToAdd.push_back(*itMove);

		//try to add some more moves
		//	note that this is a performance / memory optimization to keep the tree smaller
		//	however in many cases the assignments are not independent in the sense that they influence each others' score
		//	so it would be better to do the moves one by one
		findCompatibleMoves(movesToAdd,overAllChoices,iMaxAssignmentsPerBranch);

		if (!movesToAdd.empty())
		{
			//check if we already did something equivalent
			bool bHaveEquivalent = false;
			for (map<int,vector<STacticalAssignment>>::iterator itPrevMoves = newAssignments.begin(); itPrevMoves != newAssignments.end(); ++itPrevMoves)
				//not a perfect check but good enough
				if (movesAreEquivalent(movesToAdd,itPrevMoves->second))
				{
					bHaveEquivalent = true;
					break;
				}

			if (bHaveEquivalent)
				continue;

			CvTacticalPosition* pNewChild = addChild();
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

			if (pNewChild)
			{
				//check some candidates for redundancy
				bool bRedundant = false;
				if (parentPosition)
				{
					const vector<CvTacticalPosition*>& siblings = parentPosition->getChildren();
					for (vector<CvTacticalPosition*>::const_iterator itSibling = siblings.begin(); itSibling != siblings.end(); ++itSibling)
					{
						if (*itSibling == this)
							continue;

						const vector<CvTacticalPosition*>& nephews = (*itSibling)->getChildren();
						for (vector<CvTacticalPosition*>::const_iterator itNephew = nephews.begin(); itNephew != nephews.end(); ++itNephew)
						{
							//not a perfect check but good enough
							if (movesAreEquivalent(pNewChild->getAssignments(),(*itNephew)->getAssignments()))
							{
								bRedundant = true;
								break;
							}
						}

						if (bRedundant)
						{
							removeChild(pNewChild);
							pNewChild = NULL;
							break;
						}
					}
				}
			}

			if (pNewChild)
			{
				//remember for later
				newAssignments[iNewBranches] = movesToAdd;
				iNewBranches++;
			}
		}
	}
	
	return (iNewBranches>0);
}

//assumption is that A is executed before B
bool CvTacticalPosition::movesAreCompatible(const STacticalAssignment& A, const STacticalAssignment& B) const
{
	bool AisPlotChange = A.eType == STacticalAssignment::A_MOVE || A.eType == STacticalAssignment::A_MELEEKILL || A.eType == STacticalAssignment::A_CAPTURE;
	bool BisPlotChange = B.eType == STacticalAssignment::A_MOVE || B.eType == STacticalAssignment::A_MELEEKILL || B.eType == STacticalAssignment::A_CAPTURE;

	bool AisAttack = A.eType == STacticalAssignment::A_MELEEATTACK || A.eType == STacticalAssignment::A_RANGEATTACK;
	bool BisAttack = B.eType == STacticalAssignment::A_MELEEATTACK || B.eType == STacticalAssignment::A_RANGEATTACK;

	bool AisKill = A.eType == STacticalAssignment::A_MELEEKILL || A.eType == STacticalAssignment::A_RANGEKILL;
	bool BisKill = B.eType == STacticalAssignment::A_MELEEKILL || B.eType == STacticalAssignment::A_RANGEKILL;

	if (A.iUnitID == B.iUnitID)
		return false;

	//don't combine kills with anything, they change ZOC
	if (AisKill || BisKill)
		return false;

	//don't combine attacks, even different targets. otherwise we might miss a kill
	//the algorithm is greedy otherwise and doesn't get to check sequential attacks on the same target if it can spend the attacks right away
	if (AisAttack && BisAttack)
		return false;

	//do not combine voluntary end turns with anything except other endturns
	if ( (A.eType == STacticalAssignment::A_FINISH || B.eType == STacticalAssignment::A_FINISH) && (A.eType != B.eType))
		return false;

	if (A.bIsCombat && B.bIsCombat)
	{
		if (AisPlotChange && BisPlotChange)
			//do moves sequentially if we have only a few units. otherwise the adjacency bonus stuff doesn't work.
			//if we have many units, the runtime gets too long, and we can probably afford a suboptimal solution.
			if (availableUnits.size()<8 || A.iToPlotIndex == B.iToPlotIndex)
				return false;

		if (AisPlotChange && !BisPlotChange)
			if (A.iToPlotIndex == B.iFromPlotIndex)
				return false;

		if (!AisPlotChange && BisPlotChange)
			if (A.iFromPlotIndex == B.iToPlotIndex)
				return false;
	}

	//note that some combinations are guaranteed not to occur (hopefully)
	//- move out from the same plot (for combat units)
	//- move into plot before attack / kill
	//- ...

	return true;
}

bool CvTacticalPosition::movesAreEquivalent(const vector<STacticalAssignment>& seqA, const vector<STacticalAssignment>& seqB) const
{
	if (seqA.size()!=seqB.size())
		return false;

	struct PrAssignmentSortById
	{
		//sort by unit id and movetype as tiebreaker
		bool operator()(const STacticalAssignment& lhs, const STacticalAssignment& rhs) const 
			{ return (lhs.iUnitID < rhs.iUnitID) || (lhs.iUnitID == rhs.iUnitID && lhs.eType < rhs.eType); } 
	};
	struct PrAssignmentIsEqual
	{
		bool operator()(const STacticalAssignment& lhs, const STacticalAssignment& rhs) const 
			{ return (lhs.iUnitID == rhs.iUnitID) && (lhs.iFromPlotIndex == rhs.iFromPlotIndex) && (lhs.iToPlotIndex == rhs.iToPlotIndex) && (lhs.eType == rhs.eType); } 
	};

	//make local copies for sorting
	vector<STacticalAssignment> A = seqA;
	vector<STacticalAssignment> B = seqB;
	sort(A.begin(), A.end(), PrAssignmentSortById());
	sort(B.begin(), B.end(), PrAssignmentSortById());

	return equal(A.begin(), A.end(), B.begin(), PrAssignmentIsEqual());
}

void CvTacticalPosition::findCompatibleMoves(vector<STacticalAssignment>& chosen, const vector<STacticalAssignment>& choice, size_t nMaxCombinedMoves) const
{
	if (chosen.empty())
		return;

	for (vector<STacticalAssignment>::const_iterator itChoice = choice.begin(); itChoice != choice.end(); ++itChoice)
	{
		//don't combine good moves with crap moves
		if (itChoice->iScore < chosen.front().iScore / 3)
			break; //choice is sorted!

		if (isMoveBlockedByOtherUnit(*itChoice))
		{
			int iBlockingUnitID = findBlockingUnitAtPlot(itChoice->iToPlotIndex).iUnitID;
			if (iBlockingUnitID==0 || iBlockingUnitID==itChoice->iUnitID)
				continue; //shouldn't happen

			//check if one of the already chosen assignments solves the block
			bool bSolved = false;
			for (vector<STacticalAssignment>::const_iterator itChosen = chosen.begin(); itChosen != chosen.end(); ++itChosen)
				if (itChosen->eType == STacticalAssignment::A_MOVE && itChosen->iFromPlotIndex == itChoice->iToPlotIndex && itChosen->iUnitID == iBlockingUnitID)
				{
					bSolved = true;
					break;
				}

			if (!bSolved)
				continue;
		}

		bool bAllCompatible = true;
		for (vector<STacticalAssignment>::const_iterator itChosen = chosen.begin(); itChosen != chosen.end(); ++itChosen)
		{
			if (!movesAreCompatible(*itChosen,*itChoice))
			{
				bAllCompatible = false;
				break;
			}
		}

		if (bAllCompatible)
		{
			chosen.push_back(*itChoice);
			if (chosen.size()==nMaxCombinedMoves)
				return;
		}
	}
}

bool CvTacticalPosition::isMoveBlockedByOtherUnit(const STacticalAssignment& move) const
{
	//only movement can be blocked
	if (move.eType != STacticalAssignment::A_MOVE)
		return false;

	//find out if there was a unit there initially and if it's still there
	const CvTacticalPlot& tactPlot = getTactPlot(move.iToPlotIndex);
	if (tactPlot.isValid())
		return tactPlot.isFriendlyCombatUnit();

	return false; //no info, assume it's ok
}

STacticalAssignment CvTacticalPosition::findBlockingUnitAtPlot(int iPlotIndex) const
{
	//start with the last move!
	for (int i=(int)assignedMoves.size()-1; i>=0; i--)
	{
		//only combat units are blocking
		if (!assignedMoves[i].bIsCombat)
			continue;

		//there was a unit there but it moved away
		if (assignedMoves[i].iFromPlotIndex == iPlotIndex)
		{
			if (assignedMoves[i].eType == STacticalAssignment::A_MOVE ||
				assignedMoves[i].eType == STacticalAssignment::A_MELEEKILL)
			{
				return STacticalAssignment();
			}
		}

		//a unit moved into this plot
		if (assignedMoves[i].iToPlotIndex == iPlotIndex)
		{
			if (assignedMoves[i].eType == STacticalAssignment::A_INITIAL ||
				assignedMoves[i].eType == STacticalAssignment::A_FINISH ||
				assignedMoves[i].eType == STacticalAssignment::A_MOVE ||
				assignedMoves[i].eType == STacticalAssignment::A_MELEEKILL)
			{
				return assignedMoves[i];
			}
		}
	}

	return STacticalAssignment();
}

//are there units left or all enemies gone?
bool CvTacticalPosition::isComplete() const
{ 
	return killedEnemies.size()==nTotalEnemies;
}

//check that we're not just shuffling around units
bool CvTacticalPosition::isOffensive() const
{ 
	for (vector<STacticalAssignment>::const_iterator it=assignedMoves.begin(); it!=assignedMoves.end(); ++it)
		if (it->eType==STacticalAssignment::A_MELEEATTACK ||
			it->eType==STacticalAssignment::A_MELEEKILL ||
			it->eType==STacticalAssignment::A_RANGEATTACK ||
			it->eType==STacticalAssignment::A_RANGEKILL ||
			it->eType==STacticalAssignment::A_CAPTURE)
			return true;

	//note: a pillage move alone does not count as offensive!
	return false;
}

void CvTacticalPosition::updateTacticalPlotTypes(int iStartPlot)
{
	set<int> outstanding;

	if (iStartPlot<0)
	{
		//none given? use all ...
		for (map<int,int>::iterator it = tacticalPlotLookup.begin(); it != tacticalPlotLookup.end(); ++it)
			outstanding.insert( it->first );
	}
	else
	{
		CvTacticalPlot& tactPlot = getTactPlot(iStartPlot);
		if (tactPlot.isValid())
			tactPlot.findType(*this,outstanding);
	}

	//now this one update may cause neighbors to switch as well
	while (!outstanding.empty())
	{
		int index = *outstanding.begin();
		outstanding.erase(outstanding.begin());
		CvTacticalPlot& tactPlot = getTactPlot(index);
		if (tactPlot.isValid())
			tactPlot.findType(*this,outstanding);
	}
}

CvTacticalPosition::CvTacticalPosition(PlayerTypes player, eAggressionLevel eAggLvl, CvPlot* pTarget) : 
	ePlayer(player), dummyPlot(NULL,NO_PLAYER), pTargetPlot(pTarget), eAggression(eAggLvl), iTotalScore(0), nTotalEnemies(0), parentPosition(NULL), iID(g_siTacticalPositionCount++)
{
}

CvTacticalPosition::CvTacticalPosition(const CvTacticalPosition& parent) : dummyPlot(NULL,NO_PLAYER)
{
	ePlayer = parent.ePlayer;
	eAggression = parent.eAggression;
	fUnitNumberRatio = parent.fUnitNumberRatio;
	pTargetPlot = parent.pTargetPlot;
	iTotalScore = parent.iTotalScore;
	iScoreOverParent = 0;
	nTotalEnemies = parent.nTotalEnemies;
	parentPosition = &parent;
	iID = g_siTacticalPositionCount++;

	//childPositions stays empty!
	//reachablePlotLookup stays empty for now
	//rangeAttackPlotLookup stays empty for now

	//copied from parent, modified when addAssignment is called
	tacticalPlotLookup = parent.tacticalPlotLookup;
	tactPlots = parent.tactPlots;
	availableUnits = parent.availableUnits;
	assignedMoves = parent.assignedMoves;
	freedPlots = parent.freedPlots;
	killedEnemies = parent.killedEnemies;
}

bool CvTacticalPosition::removeChild(CvTacticalPosition* pChild)
{
	vector<CvTacticalPosition*>::iterator it = find(childPositions.begin(), childPositions.end(), pChild);
	if (it!=childPositions.end())
	{
		delete pChild;
		childPositions.erase(it);
	}

	return false;
}

CvTacticalPosition* CvTacticalPosition::addChild()
{
	CvTacticalPosition* newPosition = new CvTacticalPosition(*this);
	if (newPosition)
		childPositions.push_back(newPosition);
	
	return newPosition;
}

void CvTacticalPosition::getPlotsWithChangedVisibility(const STacticalAssignment& assignment, vector<int>& madeVisible) const
{
	madeVisible.clear();

	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(assignment.iUnitID);
	CvPlot* pNewPlot = GC.getMap().plotByIndexUnchecked(assignment.iToPlotIndex);

	for (int i=0; i<RING5_PLOTS; i++)
	{
		CvPlot* pTestPlot = iterateRingPlots(pTargetPlot,i); //iterate around the target plot, not the unit plot
		if (!pTestPlot)
			continue;

		if (pTestPlot->getVisibilityCount(pUnit->getTeam())==0)
		{
			if (pNewPlot->canSeePlot(pTestPlot, pUnit->getTeam(), pUnit->visibilityRange(), pUnit->getFacingDirection(true)))
				madeVisible.push_back(pTestPlot->GetPlotIndex());
		}
	}
}

bool CvTacticalPosition::unitHasAssignmentOfType(int iUnit, STacticalAssignment::eAssignmentType move) const
{
	for (vector<STacticalAssignment>::const_iterator it = assignedMoves.begin(); it != assignedMoves.end(); ++it)
		if (it->iUnitID == iUnit && it->eType == move)
			return true;

	return false;
}

bool CvTacticalPosition::addAssignment(STacticalAssignment newAssignment)
{
	//if we killed an enemy ZOC will change
	bool bRecomputeAllMoves = false;
	bool bVisibilityChange = false;
	int iUnitEndTurnPlot = -1;
	CvUnit* pEnemy = NULL; //in case of a kill
	
	vector<SUnitStats>::iterator itUnit = find_if(availableUnits.begin(), availableUnits.end(), PrMatchingUnit(newAssignment.iUnitID));
	if (itUnit == availableUnits.end())
		return false;
	
	CvTacticalPlot& oldTactPlot = getTactPlot(newAssignment.iFromPlotIndex);
	CvTacticalPlot& newTactPlot = getTactPlot(newAssignment.iToPlotIndex);
	//tactical plots are only touched for "real" moves. blocked units may be on invalid plots.
	if (newAssignment.eType != STacticalAssignment::A_BLOCKED)
	{
		if (!newTactPlot.isValid())
			return false;
		//a unit may start out on an invalid plot (eg. too far away)
		if (!oldTactPlot.isValid() && itUnit->eLastAssignment!=STacticalAssignment::A_INITIAL)
			return false;
	}

	//i know what you did last summer!
	itUnit->eLastAssignment = newAssignment.eType;
	
	//now for real
	switch (newAssignment.eType)
	{
	case STacticalAssignment::A_MOVE:
	case STacticalAssignment::A_CAPTURE:
		{
			int iUnitID;

			//plausi checks
			iUnitID = newAssignment.bIsCombat ? findBlockingUnitAtPlot(newAssignment.iFromPlotIndex).iUnitID : 0;
			if (iUnitID != 0 && iUnitID != newAssignment.iUnitID)
				OutputDebugString("inconsistent origin\n");

			iUnitID = newAssignment.bIsCombat ? findBlockingUnitAtPlot(newAssignment.iToPlotIndex).iUnitID : 0;
			if (iUnitID != 0)
				OutputDebugString("inconsistent destination\n");

			if (newAssignment.iRemainingMoves > itUnit->iMovesLeft)
				OutputDebugString("inconsistent moves!\n");
		}

		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iPlotIndex = newAssignment.iToPlotIndex;
		bVisibilityChange = true;
		oldTactPlot.friendlyUnitMovingOut(*this, itUnit->isCombatUnit());
		newTactPlot.friendlyUnitMovingIn(*this, itUnit->isCombatUnit());
		if (newAssignment.iRemainingMoves==0)
			iUnitEndTurnPlot = newAssignment.iToPlotIndex;
		break;
	case STacticalAssignment::A_RANGEATTACK:
	case STacticalAssignment::A_MELEEATTACK:
		if (itUnit->iAttacksLeft<=0)
			OutputDebugString("inconsistent number of attacks\n");
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iAttacksLeft--;
		newTactPlot.setDamage(newTactPlot.getDamage() + newAssignment.iDamage);
		if (newAssignment.iRemainingMoves==0)
			iUnitEndTurnPlot = newAssignment.iFromPlotIndex;
		break;
	case STacticalAssignment::A_RANGEKILL:
		if (itUnit->iAttacksLeft<=0)
			OutputDebugString("inconsistent number of attacks\n");
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iAttacksLeft--;
		newTactPlot.enemyUnitRangeKill();
		updateTacticalPlotTypes(newAssignment.iToPlotIndex);
		bRecomputeAllMoves = true; //ZOC changed
		pEnemy = GC.getMap().plotByIndexUnchecked(newAssignment.iToPlotIndex)->getVisibleEnemyDefender(ePlayer);
		if (pEnemy)
		{
			freedPlots.insert(newAssignment.iToPlotIndex);
			killedEnemies.insert(pEnemy->GetID());
		}
		if (newAssignment.iRemainingMoves==0)
			iUnitEndTurnPlot = newAssignment.iFromPlotIndex;
		break;
	case STacticalAssignment::A_MELEEKILL:
		if (itUnit->iAttacksLeft<=0)
			OutputDebugString("inconsistent number of attacks\n");
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iAttacksLeft--;
		itUnit->iPlotIndex = newAssignment.iToPlotIndex;
		bVisibilityChange = true;

		pEnemy = GC.getMap().plotByIndexUnchecked(newAssignment.iToPlotIndex)->getVisibleEnemyDefender(ePlayer);
		if (newTactPlot.isEnemyCity() && !pEnemy)
			killedEnemies.insert(0); //put an invalid unit ID as a placeholder so that isComplete() works
		else if (pEnemy) //should always be true, else we wouldn't be here
		{
			freedPlots.insert(newAssignment.iToPlotIndex);
			killedEnemies.insert(pEnemy->GetID());
		}

		oldTactPlot.friendlyUnitMovingOut(*this, itUnit->isCombatUnit());
		newTactPlot.friendlyUnitMovingIn(*this, itUnit->isCombatUnit());
		updateTacticalPlotTypes(newAssignment.iToPlotIndex);
		bRecomputeAllMoves = true; //ZOC changed
		if (newAssignment.iRemainingMoves==0)
			iUnitEndTurnPlot = newAssignment.iToPlotIndex;
		break;
	case STacticalAssignment::A_PILLAGE:
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		break;
	case STacticalAssignment::A_FINISH:
	case STacticalAssignment::A_BLOCKED:
		itUnit->iMovesLeft = 0;
		iUnitEndTurnPlot = newAssignment.iToPlotIndex;
		//todo: mark end turn plot? as opposed to transient blocks
		break;
	}

	vector<int> newlyVisiblePlots;
	if (bVisibilityChange)
	{
		//may need to add some new tactical plots - ideally we should reconsider all queued assignments afterwards
		//the next round of assignments will take into account the new plots in any case
		getPlotsWithChangedVisibility(newAssignment, newlyVisiblePlots);
		for (size_t i=0; i<newlyVisiblePlots.size(); i++)
		{
			//since it was invisible before, we know there are no friendly units around
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(newlyVisiblePlots[i]);
			if (pPlot && !pPlot->isNeutralUnit(ePlayer,true,false)) //can't use the official visiblity here 
			{
				addTacticalPlot(pPlot);
				updateTacticalPlotTypes(pPlot->GetPlotIndex());
			}
		}
	}

	int iFlags = CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_NO_EMBARK | CvUnit::MOVEFLAG_IGNORE_DANGER;
	if (bRecomputeAllMoves)
	{
		for (vector<SUnitStats>::iterator itUnit2 = availableUnits.begin(); itUnit2 != availableUnits.end(); ++itUnit2)
		{
			CvUnit* pUnit2 = GET_PLAYER(ePlayer).getUnit(itUnit2->iUnitID);
			CvPlot* pStartPlot = GC.getMap().plotByIndexUnchecked(itUnit2->iPlotIndex);

			ReachablePlots reachablePlots =  TacticalAIHelpers::GetAllPlotsInReach(pUnit2, pStartPlot, iFlags, 0, itUnit2->iMovesLeft, freedPlots);
			reachablePlotLookup[itUnit2->iUnitID] = reachablePlots;

			set<int> rangeAttackPlots;
			TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit2,pStartPlot,rangeAttackPlots, true, true);
			rangeAttackPlotLookup[itUnit2->iUnitID] = rangeAttackPlots;
		}
	}
	else if (itUnit->iMovesLeft>0) //otherwise iterator is invalid
	{
		CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(itUnit->iUnitID);
		CvPlot* pStartPlot = GC.getMap().plotByIndexUnchecked(itUnit->iPlotIndex);
		ReachablePlots reachablePlots =	TacticalAIHelpers::GetAllPlotsInReach(pUnit, pStartPlot, iFlags, 0, itUnit->iMovesLeft, freedPlots);
		reachablePlotLookup[itUnit->iUnitID] = reachablePlots;

		set<int> rangeAttackPlots;
		TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit,pStartPlot,rangeAttackPlots, true, true);
		rangeAttackPlotLookup[itUnit->iUnitID] = rangeAttackPlots;
	}
	else if (itUnit->iMovesLeft==0)
	{
		reachablePlotLookup[itUnit->iUnitID] = ReachablePlots();
		rangeAttackPlotLookup[itUnit->iUnitID] = set<int>();
	}

	//finally store it
	assignedMoves.push_back(newAssignment);

	//intermediate moves don't affect the final score, but only this generation's score
	iScoreOverParent += newAssignment.iScore;
	if (newAssignment.eType != STacticalAssignment::A_MOVE)
		iTotalScore += newAssignment.iScore;

	//increasing our visibility is good
	iScoreOverParent += newlyVisiblePlots.size();
	iTotalScore += newlyVisiblePlots.size();

	if (newAssignment.eType == STacticalAssignment::A_BLOCKED)
		availableUnits.erase(itUnit);
	else
	{
		//are we done or can we do further moves with this unit?
		if (iUnitEndTurnPlot > 0)
		{
			//since we will end the turn here, add the score of the position to the total
			int iEndTurnScore = newAssignment.bIsCombat ?
				ScorePlotForCombatUnit(*itUnit, SMovePlot(iUnitEndTurnPlot), *this, false).iScore :
				ScorePlotForSupportUnit(*itUnit, SMovePlot(iUnitEndTurnPlot), *this).iScore;
			iTotalScore += iEndTurnScore;

			//add an explicit end turn if necessary
			if (newAssignment.eType != STacticalAssignment::A_FINISH)
				assignedMoves.push_back(STacticalAssignment(iUnitEndTurnPlot, iUnitEndTurnPlot, newAssignment.iUnitID, 0, newAssignment.bIsCombat, iEndTurnScore, STacticalAssignment::A_FINISH));

			//now we can invalidate the iterator!
			availableUnits.erase(itUnit);
		}
	}

	return true;
}

bool CvTacticalPosition::haveTacticalPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return true;

	return tacticalPlotLookup.find(pPlot->GetPlotIndex()) != tacticalPlotLookup.end();
}

void CvTacticalPosition::addTacticalPlot(const CvPlot* pPlot)
{
	if (!pPlot)
		return;

	map<int, int>::iterator it = tacticalPlotLookup.find(pPlot->GetPlotIndex());
	if (it == tacticalPlotLookup.end())
	{
		tacticalPlotLookup[pPlot->GetPlotIndex()] = (int)tactPlots.size();
		tactPlots.push_back( CvTacticalPlot(pPlot,ePlayer) );

		if (tactPlots.back().isEnemy())
			nTotalEnemies++;
	}
	else
	{
		//re-adding an existing plot sets it valid
		tactPlots[it->second].setValid(true);
	}
}

bool CvTacticalPosition::addAvailableUnit(const CvUnit* pUnit)
{
	if (!pUnit || !pUnit->canMove())
		return false;

	SUnitStats::eMovementStrategy eStrategy = SUnitStats::MS_NONE;
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
		case UNITAI_CARRIER_SEA:
			if (pUnit->GetRange()>2)
				eStrategy = SUnitStats::MS_THIRDLINE;
			else if (pUnit->GetRange()==2)
				eStrategy = SUnitStats::MS_SECONDLINE;
			else
				//this is also for melee
				eStrategy = SUnitStats::MS_FIRSTLINE;
			break;

		//explorers are an edge case, the visibility can be useful, so include them
		//they shouldn't fight if the odds are bad
		case UNITAI_EXPLORE:
		case UNITAI_EXPLORE_SEA:
			eStrategy = SUnitStats::MS_FIRSTLINE;
			break;

		//combat support, stay out of danger
		case UNITAI_GENERAL:
		case UNITAI_ADMIRAL:
		case UNITAI_CITY_SPECIAL:
			eStrategy = SUnitStats::MS_SUPPORT;
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

	availableUnits.push_back( SUnitStats( pUnit, eStrategy ) );
	assignedMoves.push_back( STacticalAssignment(pUnit->plot()->GetPlotIndex(),pUnit->plot()->GetPlotIndex(),
								pUnit->GetID(),pUnit->getMoves(),pUnit->IsCombatUnit(),0,STacticalAssignment::A_INITIAL) );

	ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false /*no embarkation*/);
	reachablePlotLookup[pUnit->GetID()] = reachablePlots; //store all of them, if there's no matching tactical plot they will be skipped later

	//for ranged units
	set<int> rangeAttackPlots;
	TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit,pUnit->plot(),rangeAttackPlots,true,false); //use official visibility here
	rangeAttackPlotLookup[pUnit->GetID()] = rangeAttackPlots;

	return true;
}

int CvTacticalPosition::countChildren() const
{
	int iCount = (int)childPositions.size();
	for (size_t i = 0; i < childPositions.size(); i++)
		iCount += childPositions[i]->countChildren();

	return iCount;
}

float CvTacticalPosition::getUnitNumberRatio() const
{
	return fUnitNumberRatio;
}

void CvTacticalPosition::updateUnitNumberRatio()
{
	fUnitNumberRatio = sqrt( availableUnits.size() / float(max(1u,nTotalEnemies)) );
	fUnitNumberRatio = max( 0.9f, fUnitNumberRatio ); //<1 indicates we're fewer but don't stop attacking because of that
}

std::ostream& operator<<(ostream& os, const CvPlot& p)
{
    os << "(" << p.getX() << "," << p.getY() << ")";
    return os;
}

ostream& operator << (ostream& out, const STacticalAssignment& arg)
{
	const char* eType = assignmentTypeNames[arg.eType];
	//CvPlot* pFromPlot = GC.getMap().plotByIndexUnchecked( arg.iFromPlotIndex );
	//CvPlot* pToPlot = GC.getMap().plotByIndexUnchecked( arg.iToPlotIndex );
	out << arg.iUnitID << " " << eType << " from " << arg.iFromPlotIndex << " to " << arg.iToPlotIndex << " (" << arg.iScore << ")";
	return out;
}

void CvTacticalPosition::dumpChildren(ofstream& out) const
{
	out << "n" << (void*)this << " [ label = \"id " << iID << "\nscore " << iTotalScore << ", " << availableUnits.size() << " units\" ];\n";

	size_t nAssignments = assignedMoves.size();
	for (size_t i = 0; i < childPositions.size(); i++)
	{
		out << "n" << (void*)this << " -> n" << (void*)childPositions[i] << " [ label = \"";

		size_t nAssignmentsChild = childPositions[i]->getAssignments().size();
		for (size_t j = nAssignments; j < nAssignmentsChild; j++)
		{
			STacticalAssignment& assignment = childPositions[i]->getAssignments()[j];
			CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(assignment.iUnitID);
			out << pUnit->getName().c_str() << " " << assignment << "\\n";
		}
		out << "\" ];\n";
	}

	for (size_t i = 0; i < childPositions.size(); i++)
		childPositions[i]->dumpChildren(out);
}

void CvTacticalPosition::dumpPlotStatus(const char* fname) const
{
	ofstream out(fname);
	if (out)
	{
		out << "#x,y,isEnemy,isFriendly,nAdjEnemy,nAdjFriendly,nAdjFirstline,hasSupport,type\n"; 
		for (vector<CvTacticalPlot>::const_iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
		{
			CvPlot* pPlot =  GC.getMap().plotByIndexUnchecked( it->getPlotIndex() );
			out << pPlot->getX() << "," << pPlot->getY() << "," << (it->isEnemy() ? 1 : 0) << "," << (it->isFriendlyCombatUnit() ? 1 : 0) << "," 
				<< it->getNumAdjacentEnemies() << "," << it->getNumAdjacentFriendlies() << "," << it->getNumAdjacentFirstlineFriendlies() << "," 
				<< it->hasSupportBonus() << "," << it->getType() << "\n";
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

CvTacticalPlot& CvTacticalPosition::getTactPlot(int plotindex)
{
	map<int,int>::const_iterator it = tacticalPlotLookup.find(plotindex);
	if (it!=tacticalPlotLookup.end())
		return tactPlots[it->second];

	return dummyPlot;
}

const CvTacticalPlot& CvTacticalPosition::getTactPlot(int plotindex) const
{
	map<int,int>::const_iterator it = tacticalPlotLookup.find(plotindex);
	if (it!=tacticalPlotLookup.end())
		return tactPlots[it->second];

	return dummyPlot;
}

bool TacticalAIHelpers::FindBestAssignmentsForUnits(const vector<CvUnit*>& vUnits, CvPlot* pTarget, eAggressionLevel eAggLvl, 
	int iMaxBranches, int iMaxFinishedPositions, vector<STacticalAssignment>& result)
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

	result.clear();
	if (vUnits.empty() || vUnits.front()==NULL)
		return false;

	PlayerTypes ePlayer = vUnits.front()->getOwner();

	cvStopWatch timer("tactical move checker",NULL,0,true);
	timer.StartPerfTest();

	//set up the initial position
	g_siTacticalPositionCount = 0;
	CvTacticalPosition* initialPosition = new CvTacticalPosition(ePlayer,eAggLvl,pTarget);

	//add all our units
	set<int> ourUnits;
	for(size_t i=0; i<vUnits.size(); i++)
	{
		CvUnit* pUnit = vUnits[i];
		//ignore embarked units, too difficult to get it right
		if (pUnit && !pUnit->isEmbarked() && pUnit->canMove() && !pUnit->isDelayedDeath()) 
			if (initialPosition->addAvailableUnit(pUnit))
				ourUnits.insert(pUnit->GetID());
	}

	//create the tactical plots around the target (up to distance 5)
	//not equivalent to the union of all reachable plots: we need to consider unreachable enemies as well!
	//some units may have their initial plots outside of this range but that's ok
	for (int i=0; i<RING5_PLOTS; i++)
	{
		CvPlot* pPlot = iterateRingPlots(pTarget,i);

		//for the inital setup we use the official visibility. skip plots with neutral combat units!
		if (pPlot && pPlot->isVisible( GET_PLAYER(ePlayer).getTeam() ) && !pPlot->isNeutralUnit(ePlayer,true,true))
		{
			//another twist. there may be "unrelated" friendly units standing around and blocking tiles
			//note: embarked units don't block the plot (although they are ignored for the attack)
			CvUnit* pDefender = pPlot->getBestDefender(ePlayer);
			if (!pDefender || pDefender->isEmbarked() || ourUnits.find(pDefender->GetID())!=ourUnits.end())
				initialPosition->addTacticalPlot(pPlot);
		}
	}

	//are there enemies?
	if (initialPosition->isComplete())
		return true;

	//this influences how daring we'll be
	initialPosition->updateUnitNumberRatio();

	//find out which plot is frontline, second line etc
	initialPosition->updateTacticalPlotTypes();
	if (0)
		initialPosition->dumpPlotStatus("c:\\temp\\plotstatus_initial.csv");

	vector<CvTacticalPosition*> openPositionsHeap;
	vector<CvTacticalPosition*> completedPositions;
	vector<CvTacticalPosition*> finishedPositions;
	vector<CvTacticalPosition*> discardedPositions;

	//don't need to call make_heap for a single element
	openPositionsHeap.push_back(initialPosition);
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

	int iMaxAssignmentsPerBranch = max(ourUnits.size() / 3, 1u);
	while (!openPositionsHeap.empty() && (int)finishedPositions.size()<iMaxFinishedPositions)
	{
		pop_heap( openPositionsHeap.begin(), openPositionsHeap.end(), PrPositionIsBetterHeap() );
		CvTacticalPosition* current = openPositionsHeap.back(); openPositionsHeap.pop_back();

		//done?
		if (current->isComplete())
		{
			completedPositions.push_back(current);
		}
		//here the magic happens
		else if (current->makeNextAssignments(iMaxBranches,iMaxAssignmentsPerBranch))
		{
			for (vector<CvTacticalPosition*>::const_iterator it = current->getChildren().begin(); it != current->getChildren().end(); ++it)
			{
				openPositionsHeap.push_back(*it);
				push_heap( openPositionsHeap.begin(), openPositionsHeap.end(), PrPositionIsBetterHeap() );
			}
		}
		//if we cannot add further moves, throw away positions without offensive moves
		else if (current->isOffensive())
			finishedPositions.push_back(current);
		else
			discardedPositions.push_back(current);

		if (openPositionsHeap.size()>5000 || discardedPositions.size()>5000)
		{
			OutputDebugString( "warning: terminating because of endless loop!\n" );
			break;
		}
	}

	//normally the score for a completed position should be higher than for a finished one
	//but if there are too many unassigned units it can tip
	if (!completedPositions.empty())
	{
		//need the predicate, else we sort the pointers by address!
		sort(completedPositions.begin(), completedPositions.end(), PrPositionIsBetter());
		result = completedPositions.front()->getAssignments();

		if (0)
			completedPositions.front()->dumpPlotStatus("c:\\temp\\plotstatus_final.csv");
	}
	else if (!finishedPositions.empty())
	{
		//need the predicate, else we sort the pointers by address!
		sort(finishedPositions.begin(), finishedPositions.end(), PrPositionIsBetter());
		result = finishedPositions.front()->getAssignments();

		if (0)
			finishedPositions.front()->dumpPlotStatus("c:\\temp\\plotstatus_final.csv");
	}

	//debugging
	timer.EndPerfTest();
	OutputDebugString(CvString::format("--> tested %d possible positions for %d units in %.2fms\n", initialPosition->countChildren(), ourUnits.size(), timer.GetDeltaInSeconds()*1000).c_str());
	if (0) //if needed we can set the instruction pointer here
		initialPosition->exportToDotFile("c:\\temp\\graph.dot");

	stringstream buffer;
	for(size_t i=0; i<result.size(); i++)
		buffer << result[i] << "\n";
	OutputDebugString( buffer.str().c_str() );

	//this deletes the whole tree with all child positions
	delete initialPosition;

	//if we had to bail because of memory issues
	if (result.empty() && vUnits.size()>10 && (openPositionsHeap.size()>5000 || discardedPositions.size()>5000))
	{
		OutputDebugString("retry with fewer units ...\n");
		vector<CvUnit*> vUnits2( vUnits.begin(), vUnits.begin()+vUnits.size()/2 );
		return FindBestAssignmentsForUnits(vUnits2, pTarget, eAggLvl, iMaxBranches, iMaxFinishedPositions, result);
	}

	return !result.empty();
}

bool TacticalAIHelpers::ExecuteUnitAssignments(PlayerTypes ePlayer, const std::vector<STacticalAssignment>& vAssignments)
{
	//take the assigned moves one by one and try to execute them faithfully. 
	//may fail if a melee kill unexpectedly happens or does not happen

	//todo: possible optimizations
	// - check if a unit is damaged, and don't move it so that it can heal if it's not actively attacking
	// - cancel execution if new enemy units are revealed during movement

	for (size_t i = 0; i < vAssignments.size(); i++)
	{
		CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(vAssignments[i].iUnitID);
		if (!pUnit)
			continue;

		CvPlot* pFromPlot = GC.getMap().plotByIndexUnchecked(vAssignments[i].iFromPlotIndex);
		CvPlot* pToPlot = GC.getMap().plotByIndexUnchecked(vAssignments[i].iToPlotIndex);

		bool bPrecondition = false;
		bool bPostcondition = false;

		switch (vAssignments[i].eType)
		{
		case STacticalAssignment::A_INITIAL:
			continue; //skip this!
			break;
		case STacticalAssignment::A_MOVE:
		case STacticalAssignment::A_CAPTURE:
			pUnit->ClearPathCache(); //make sure there's no stale path which coincides with our target
			bPrecondition = (pUnit->plot() == pFromPlot) && !(pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //no enemy
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pToPlot->getX(), pToPlot->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER, false, false, MISSIONAI_OPMOVE); //don't take any detours because of danger
			bPostcondition = (pUnit->plot() == pToPlot); //plot changed
			break;
		case STacticalAssignment::A_RANGEATTACK:
			bPrecondition = (pUnit->plot() == pFromPlot) && (pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy present
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pToPlot->getX(), pToPlot->getY());
			bPostcondition = pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit); //enemy should survive
			break;
		case STacticalAssignment::A_RANGEKILL:
			bPrecondition = (pUnit->plot() == pFromPlot) && pToPlot->isEnemyUnit(ePlayer,true,true); //defending unit present. does not apply to cities
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pToPlot->getX(), pToPlot->getY());
			bPostcondition = !pToPlot->isEnemyUnit(ePlayer,true,true); //defending unit is gone
			break;
		case STacticalAssignment::A_MELEEATTACK:
			bPrecondition = (pUnit->plot() == pFromPlot) && (pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy present
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pToPlot->getX(), pToPlot->getY());
			bPostcondition = (pUnit->plot() == pFromPlot) && (pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy still present
			break;
		case STacticalAssignment::A_MELEEKILL:
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
			bPostcondition = (pUnit->plot() == pToPlot) && !(pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //enemy gone
			break;
		case STacticalAssignment::A_PILLAGE:
			pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
			bPrecondition = true;
			bPostcondition = true;
			break;
		case STacticalAssignment::A_FINISH:
			if (pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			//this is the difference to a blocked unit, we prevent anyone else from moving it
			GET_PLAYER(ePlayer).GetTacticalAI()->UnitProcessed(pUnit->GetID());
			bPrecondition = true;
			bPostcondition = true;
			break;
		case STacticalAssignment::A_BLOCKED:
			if (pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			//do not mark the unit as processed, it can be reused for other tasks!
			bPrecondition = true;
			bPostcondition = true;
			break;
		}

		if (!bPrecondition || !bPostcondition)
		{
			stringstream out;
			out << "could not execute " << vAssignments[i] << (!bPrecondition?"":" (unexpected result)") << "\n";
			OutputDebugString(out.str().c_str());
			return false;
		}
	}

	return true;
}
#endif

int g_siTacticalPositionCount = 0;

const char* barbarianMoveNames[] =
{
	"B_CAPTURE_CITY",
	"B_DAMAGE_CITY",
	"B_DESTROY_HIGH_PRIO_UNIT",
	"B_DESTROY_MEDIUM_PRIO_UNIT",
	"B_DESTROY_LOW_PRIO_UNIT",
	"B_MOVE_TO_SAFETY",
	"B_ATTRIT_HIGH_PRIO_UNIT", //6
	"B_ATTRIT_MEDIUM_PRIO_UNIT",
	"B_ATTRIT_LOW_PRIO_UNIT",
	"B_PILLAGE",
	"B_BLOCKADE_RESOURCE",
	"B_CIVILIAN_ATTACK",
	"B_AGGRESSIVE_MOVE", //12
	"B_PASSIVE_MOVE",
	"B_CAMP_DEFENSE",
	"B_DESPERATE_ATTACK",
	"B_ESCORT_CIVILIAN",
	"B_PLUNDER_TRADE_UNIT",
	"B_PILLAGE_CITADEL",
	"B_PILLAGE_NEXT_TURN",
};

const char* postureNames[] =
{
    "P_WITHDRAW",
    "P_SIT_AND_BOMBARD",
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
	"ENDTURN",
	"BLOCKED",
	"PILLAGE",
	"CAPTURE"
};