/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvTacticalAI.h"
#include "CvTacticalAnalysisMap.h"
#include "CvGameCoreUtils.h"
#include "CvAStar.h"
#include "CvEconomicAI.h"
#include "CvEnumSerialization.h"
#include "CvUnitCombat.h"
#include "CvGrandStrategyAI.h"
#include "cvStopWatch.h"
#include "CvMilitaryAI.h"
#include "CvTypes.h"
#if defined(MOD_BALANCE_CORE_MILITARY)
#include "CvDiplomacyAI.h"
#include "CvBarbarians.h"
#include "CvUnitMovement.h"
#endif

#include <iomanip>
#include <sstream>
#include <cmath>
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE_MILITARY)
//for easier debugging
#ifdef VPDEBUG
#define TACTDEBUG 1
#endif

int gCurrentUnitToTrack = 0;
const unsigned char TACTICAL_COMBAT_MAX_TARGET_DISTANCE = 4; //not larger than 4, not smaller than 3
const int TACTICAL_COMBAT_CITADEL_BONUS = 67; //larger than 60 to override firstline/secondline difference
const int TACTICAL_COMBAT_IMPOSSIBLE_SCORE = -1000;
const int TACTSIM_UNIQUENESS_CHECK_GENERATIONS = 3; //higher means check more siblings for permutations
const int TACTSIM_BREADTH_FIRST_GENERATIONS = 3; //switch to depth-first later

//global memory for tactical simulation
CvTactPosStorage gTactPosStorage(8000);
TCachedMovePlots gReachablePlotsLookup;
TCachedRangeAttackPlots gRangeAttackPlotsLookup;
vector<int> gLandEnemies, gSeaEnemies, gCitadels, gNewlyVisiblePlots;
vector<STacticalAssignment> gPossibleMoves;
vector<STacticalAssignment> gOverAllChoices;
vector<STacticalAssignment> gMovesToAdd;
PlayerTypes eLastTactSimPlayer = NO_PLAYER;

//just some statistics
unsigned long gMovePlotsCacheHit = 0, gMovePlotsCacheMiss = 0;
unsigned long gAttackPlotsCacheHit = 0, gAttackPlotsCacheMiss = 0;
unsigned long gAttackCacheHit = 0, gAttackCacheMiss = 0;
unsigned long giEquivalentPos = 0, giDifferentPos = 0;
unsigned long giValidEndPos = 0, giInvalidEndPos = 0;
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

bool CvTacticalTarget::IsReadyForCapture() const
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
bool CvTacticalTarget::IsTargetStillAlive(PlayerTypes eAttackingPlayer) const
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
			bRtnValue = true;
	}
	else
	{
		bRtnValue = true; //unknown type, assume still valid
	}

	return bRtnValue;
}

/// This target make sense for this domain of unit/zone?
bool CvTacticalTarget::IsTargetValidInThisDomain(DomainTypes eDomain) const
{
	switch(GetTargetType())
	{
	case AI_TACTICAL_TARGET_NONE:
		return false;

	case AI_TACTICAL_TARGET_DEFENSIVE_BASTION:
	case AI_TACTICAL_TARGET_BARBARIAN_CAMP:
	case AI_TACTICAL_TARGET_IMPROVEMENT:
	case AI_TACTICAL_TARGET_IMPROVEMENT_TO_DEFEND:
	case AI_TACTICAL_TARGET_TRADE_UNIT_LAND:
	case AI_TACTICAL_TARGET_CITADEL:
	case AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE:
	case AI_TACTICAL_TARGET_GOODY:
		return eDomain == DOMAIN_LAND;

	case AI_TACTICAL_TARGET_BLOCKADE_POINT:
	case AI_TACTICAL_TARGET_BOMBARDMENT_ZONE:
	case AI_TACTICAL_TARGET_EMBARKED_CIVILIAN:
	case AI_TACTICAL_TARGET_TRADE_UNIT_SEA:
		return eDomain == DOMAIN_SEA;

	case AI_TACTICAL_TARGET_CITY:
	case AI_TACTICAL_TARGET_CITY_TO_DEFEND:
	case AI_TACTICAL_TARGET_LOW_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_HIGH_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_VERY_HIGH_PRIORITY_CIVILIAN:
	case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
	case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
	case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
		return true;
	}

	return false;
}

template<typename FocusArea, typename Visitor>
void CvFocusArea::Serialize(FocusArea& focusArea, Visitor& visitor)
{
	visitor(focusArea.m_iX);
	visitor(focusArea.m_iY);
	visitor(focusArea.m_iRadius);
	visitor(focusArea.m_iLastTurn);
}

FDataStream& operator<<(FDataStream& saveTo, const CvFocusArea& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvFocusArea::Serialize(readFrom, serialVisitor);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvFocusArea& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvFocusArea::Serialize(writeTo, serialVisitor);
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

	m_tacticalMap.Reset(m_pPlayer ? m_pPlayer->GetID() : NO_PLAYER);

	// Initialize AI constants from XML
	m_iRecruitRange = /*8*/ GD_INT_GET(AI_TACTICAL_RECRUIT_RANGE);
	m_iLandBarbarianRange = max(1, GC.getGame().getHandicapInfo().getBarbarianLandTargetRange());
	m_iSeaBarbarianRange = max(1, GC.getGame().getHandicapInfo().getBarbarianSeaTargetRange());
}

/// Deallocate memory created in initialize
void CvTacticalAI::Uninit()
{
}

///
template<typename TacticalAI, typename Visitor>
void CvTacticalAI::Serialize(TacticalAI& tacticalAI, Visitor& visitor)
{
	visitor(tacticalAI.m_focusAreas);
	visitor(tacticalAI.m_tacticalMap);
}

/// Serialization read
void CvTacticalAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvTacticalAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvTacticalAI& tacticalAI)
{
	tacticalAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvTacticalAI& tacticalAI)
{
	tacticalAI.Write(stream);
	return stream;
}

/// Mark all the units that will be under tactical AI control this turn
void CvTacticalAI::RecruitUnits()
{
	int iLoop = 0;
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

		// Never want immobile/dead units, explorers, ones that have already moved or automated human units
		if(!pLoopUnit->canUseForTacticalAI())
			continue;

		// reset mission AI so we don't see stale information (debugging only)
		pLoopUnit->SetMissionAI(NO_MISSIONAI,NULL,NULL);

		// we want all combat ready air units, except nukes (those go through operational AI)
		if (pLoopUnit->getDomainType() == DOMAIN_AIR)
		{
			//rebasing is done in homeland AI
			if (!ShouldRebase(pLoopUnit))
				m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
		// Now down to land and sea units
		else
		{
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
			CvString msg = CvString::format("using %s %d at %d,%d for tactical ai. power is %d\n", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY(), pUnit->GetPower() );
			LogTacticalMessage( msg );
		}
	}
#endif
}

/// Update the AI for units
void CvTacticalAI::Update()
{
	UpdateVisibility();
	DropOldFocusAreas();
	FindTacticalTargets();

	//do this after updating the target list!
	RecruitUnits();

	// Loop through each dominance zone assigning moves
	ProcessDominanceZones();
}

/// Clear up memory usage
void CvTacticalAI::CleanUp()
{
	m_AllTargets.clear();
	m_ZoneTargets.clear();
}

/// Add a temporary focus of attention around a short-term target
void CvTacticalAI::AddFocusArea(CvPlot* pPlot, int iRadius, int iDuration)
{
	if (!pPlot)
		return;

	CvFocusArea zone;
	zone.m_iX = pPlot->getX();
	zone.m_iY = pPlot->getY();
	zone.m_iRadius = iRadius;
	zone.m_iLastTurn = GC.getGame().getGameTurn() + iDuration;

	m_focusAreas.push_back(zone);
}

/// Remove a temporary focus of attention we no longer need to track
void CvTacticalAI::DeleteFocusArea(CvPlot* pPlot)
{
	std::vector<CvFocusArea> zonesCopy(m_focusAreas);
	m_focusAreas.clear();

	// Copy back to original vector any whose coords don't match
	for(unsigned int iI = 0; iI < zonesCopy.size(); iI++)
		if(zonesCopy[iI].m_iX != pPlot->getX() || zonesCopy[iI].m_iY != pPlot->getY())
			m_focusAreas.push_back(zonesCopy[iI]);
}

/// Remove focus zones that have expired
void CvTacticalAI::DropOldFocusAreas()
{
	std::vector<CvFocusArea> zonesCopy(m_focusAreas);
	m_focusAreas.clear();

	// Copy back to original vector any that haven't expired
	for(unsigned int iI = 0; iI < zonesCopy.size(); iI++)
		if(zonesCopy[iI].m_iLastTurn >= GC.getGame().getGameTurn())
			m_focusAreas.push_back(zonesCopy[iI]);
}

/// Is this a city that an operation just deployed in front of?
bool CvTacticalAI::IsInFocusArea(const CvPlot* pPlot) const
{
	for(unsigned int iI = 0; iI < m_focusAreas.size(); iI++)
		if(plotDistance(pPlot->getX(),pPlot->getY(),m_focusAreas[iI].m_iX,m_focusAreas[iI].m_iY)<=m_focusAreas[iI].m_iRadius)
			return true;

	return false;
}

/// Setup knowledge of other players' seen plots
void CvTacticalAI::UpdateVisibility()
{
	const TeamTypes eTeam = m_pPlayer->getTeam();

	CvPlot* pLoopPlot;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		pLoopPlot->ResetKnownVisibility();
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		if (!pLoopPlot->isRevealed(eTeam))
			continue;

		UpdateVisibilityFromBorders(pLoopPlot);

		if (!pLoopPlot->isVisible(eTeam))
			continue;

		UpdateVisibilityFromUnits(pLoopPlot);
	}
}

/// Check if there are any units owned by other players in this tile and what they can see
void CvTacticalAI::UpdateVisibilityFromUnits(CvPlot* pPlot)
{
	if (!pPlot)
		return;

	const TeamTypes ePlayerTeam = m_pPlayer->getTeam();

	if (pPlot->getNumUnits() > 0)
	{
		CvUnit* pLoopUnit;
		TeamTypes eLoopUnitTeam;

		for (int iI = 0; iI < pPlot->getNumUnits(); iI++)
		{
			pLoopUnit = pPlot->getUnitByIndex(iI);
			eLoopUnitTeam = pLoopUnit->getTeam();

			const PlayerTypes eMinorCivAlly = GET_TEAM(eLoopUnitTeam).isMinorCiv() ? GET_PLAYER(pLoopUnit->getOwner()).GetMinorCivAI()->GetAlly() : NO_PLAYER;
			TeamTypes eMinorCivAllyTeam = eMinorCivAlly != NO_PLAYER ? GET_PLAYER(eMinorCivAlly).getTeam() : NO_TEAM;

			if (eMinorCivAllyTeam == ePlayerTeam)
				eMinorCivAllyTeam = NO_TEAM;

			if (eLoopUnitTeam != ePlayerTeam && !pLoopUnit->isInvisible(ePlayerTeam, false))
			{
				pPlot->ChangeKnownAdjacentSight(eLoopUnitTeam, eMinorCivAllyTeam, pLoopUnit->visibilityRange(), pLoopUnit->getFacingDirection(true));
			}
		}
	}

	if (pPlot->IsTradeUnitRoute())
	{
		PlotIndexContainer aiTradeUnitsAtPlot = m_pPlayer->GetTrade()->GetOpposingTradeUnitsAtPlot(pPlot, false);
		for (PlotIndexContainer::iterator it = aiTradeUnitsAtPlot.begin(); it != aiTradeUnitsAtPlot.end(); ++it)
		{
			PlayerTypes eTradeUnitOwner = GC.getGame().GetGameTrade()->GetOwnerFromID(*it);

			if (eTradeUnitOwner != NO_PLAYER)
				pPlot->IncreaseKnownVisibilityCount(GET_PLAYER(eTradeUnitOwner).getTeam(), NO_TEAM);
		}
	}
}

void CvTacticalAI::UpdateVisibilityFromBorders(CvPlot* pPlot)
{
	const TeamTypes ePlayerTeam = m_pPlayer->getTeam();
	const TeamTypes ePlotTeam = pPlot->getTeam();
	const PlayerTypes eMinorCivAlly = GET_TEAM(ePlotTeam).isMinorCiv() ? GET_PLAYER(pPlot->getOwner()).GetMinorCivAI()->GetAlly() : NO_PLAYER;
	TeamTypes eMinorCivAllyTeam = eMinorCivAlly != NO_PLAYER ? GET_PLAYER(eMinorCivAlly).getTeam() : NO_TEAM;

	if (eMinorCivAllyTeam == ePlayerTeam)
		eMinorCivAllyTeam = NO_TEAM;

	if (ePlotTeam != NO_TEAM && ePlotTeam != ePlayerTeam && GET_TEAM(ePlayerTeam).isHasMet(ePlotTeam))
	{
		pPlot->ChangeKnownAdjacentSight(ePlotTeam, eMinorCivAllyTeam, GD_INT_GET(PLOT_VISIBILITY_RANGE), NO_DIRECTION);
	}
}

// PRIVATE METHODS

/// Make lists of everything we might want to target with the tactical AI this turn
void CvTacticalAI::FindTacticalTargets()
{
	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();

	bool bNoBarbsAllowedYet = GC.getGame().getGameTurn() < GC.getGame().GetBarbarianReleaseTurn();
	vector<PlayerTypes> vUnfriendlyMajors = m_pPlayer->GetUnfriendlyMajors();

	// Look at every tile on map
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		bool bValidPlot = pLoopPlot->isRevealed(m_pPlayer->getTeam());

		// Make sure I am not a barbarian who can not move into owned territory this early in the game
		if (m_pPlayer->isBarbarian() && bNoBarbsAllowedYet && pLoopPlot->isOwned())
			continue;

		if (bValidPlot)
		{
			//camps are typically revealed but not visible; also the camp might since have been cleared but we don't know yet - so check if it is owned now
			bool bSuspectedBarbCamp = pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT) && !pLoopPlot->isOwned();

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
					//minors ignore barbarians until they are close to their borders
					if (!m_pPlayer->isMinorCiv() || !pUnit->isBarbarian() || pUnit->plot()->isAdjacentTeam(m_pPlayer->getTeam()))
					{
						//note that the HIGH/MEDIUM/LOW classification is changed later in IdentifyPriorityTargets
						newTarget.SetTargetType(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT);
						newTarget.SetUnitPtr(pUnit);
						newTarget.SetAuxIntData(50);
						m_AllTargets.push_back(newTarget);
					}
				}
				// ... unprotected enemy civilian?
				else if (pLoopPlot->isEnemyUnit(m_pPlayer->GetID(),false,true) && !pLoopPlot->isNeutralUnit(m_pPlayer->GetID(),true,true))
				{
					for (int iUnitLoop = 0; iUnitLoop < pLoopPlot->getNumUnits(); iUnitLoop++)
					{
						CvUnit* pUnit = pLoopPlot->getUnitByIndex(iUnitLoop);

						//barbarians do not attack civilians before the first city was founded.
						if (!m_pPlayer->isBarbarian() || GET_PLAYER(pUnit->getOwner()).GetNumCitiesFounded() > 0)
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

				// ... barbarian camp? doesn't matter if it has a unit inside
				// note that minors ignore barb camps, cannot clear them anyway
				if (bSuspectedBarbCamp && (m_pPlayer->isMajorCiv() || m_pPlayer->isBarbarian()))
				{
					int iBaseScore = pLoopPlot->isVisible(m_pPlayer->getTeam()) ? 100 : 50;
					newTarget.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
					newTarget.SetAuxIntData(iBaseScore - m_pPlayer->GetCityDistancePathLength(pLoopPlot));
					m_AllTargets.push_back(newTarget);
				}

				// ... unpopped goody hut? (ancient ruins)
				if(!m_pPlayer->isMinorCiv() && pLoopPlot->isRevealedGoody(m_pPlayer->getTeam()))
				{
					int iBaseScore = pLoopPlot->isVisible(m_pPlayer->getTeam()) ? 100 : 50;
					newTarget.SetTargetType(AI_TACTICAL_TARGET_GOODY);
					newTarget.SetAuxIntData(iBaseScore - m_pPlayer->GetCityDistancePathLength(pLoopPlot));
					m_AllTargets.push_back(newTarget);
				}

				// Or citadels (for pillaging!)
				if (atWar(m_pPlayer->getTeam(), pLoopPlot->getTeam()) &&
					pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()) != NO_IMPROVEMENT &&
					GC.getImprovementInfo(pLoopPlot->getRevealedImprovementType(m_pPlayer->getTeam()))->GetNearbyEnemyDamage() > /*10 in CP, 5 in VP*/ GD_INT_GET(ENEMY_HEAL_RATE) &&
					!pLoopPlot->IsImprovementPillaged())
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
					ResourceTypes eResource = pLoopPlot->getResourceType(m_pPlayer->getTeam());
					int iExtraScore = 0;
					//does this make a difference in the end?
					if (m_pPlayer->isBarbarian() || m_pPlayer->GetPlayerTraits()->IsWarmonger())
						iExtraScore = 20;

					ResourceUsageTypes eResourceUsage = (eResource != NO_RESOURCE) ? GC.getResourceInfo(eResource)->getResourceUsage() : RESOURCEUSAGE_BONUS;
					if (eResourceUsage == RESOURCEUSAGE_STRATEGIC)
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE);
						newTarget.SetAuxIntData(80+iExtraScore);
					}
					else if (eResourceUsage == RESOURCEUSAGE_LUXURY)
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE);
						newTarget.SetAuxIntData(40+iExtraScore);
					}
					else
					{
						newTarget.SetTargetType(AI_TACTICAL_TARGET_IMPROVEMENT);
						newTarget.SetAuxIntData(5+iExtraScore);
					}

					m_AllTargets.push_back(newTarget);
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
				if (m_pPlayer->GetID() == pLoopPlot->getOwner() && pLoopPlot->getDomain()==DOMAIN_LAND &&
					(pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false) >= 30 || pLoopPlot->IsChokePoint()) &&
					(!vUnfriendlyMajors.empty() && pLoopPlot->IsBorderLand(m_pPlayer->GetID(), vUnfriendlyMajors))
					)
				{
					newTarget.SetTargetType(AI_TACTICAL_TARGET_DEFENSIVE_BASTION);
					int iValue = pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false);
					if (pLoopPlot->IsChokePoint())
						iValue *= 3;

					newTarget.SetAuxIntData(iValue);
					m_AllTargets.push_back(newTarget);
				}

				// ... friendly improvement?
				if (m_pPlayer->GetID() == pLoopPlot->getOwner() &&
					pLoopPlot->getImprovementType() != NO_IMPROVEMENT &&
					!pLoopPlot->IsImprovementPillaged() && !pLoopPlot->isGoody())
				{
					if (pLoopPlot->getOwningCity() != NULL && !vUnfriendlyMajors.empty() && pLoopPlot->getOwningCity()->isBorderCity(vUnfriendlyMajors))
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
							iWeight = max(1, iWeight - m_pPlayer->GetCityDistancePathLength(pLoopPlot));

							//try to support the troops
							if (pOwningCity->getDamage() > 0 || pOwningCity->isUnderSiege())
								iWeight *= 2;

							if (iWeight > 0)
							{
								newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_POINT);
								newTarget.SetAuxIntData(iWeight);
								m_NavalBlockadePoints.push_back(newTarget);
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

	if (GC.getLogging() && GC.getAILogging())
		DumpTacticalTargets();
}

/// Don't allow adjacent tiles to both be sentry points
void CvTacticalAI::PrioritizeNavalTargetsAndAddToMainList()
{
	// First, sort the sentry points by priority
	std::stable_sort(m_NavalBlockadePoints.begin(), m_NavalBlockadePoints.end());
	CvTacticalTarget newTarget;
	// Loop through all points in copy
	for (unsigned int iI = 0; iI < m_NavalBlockadePoints.size(); iI++)
	{
		// Is the target of an appropriate type?
		CvPlot* pPlot = GC.getMap().plot(m_NavalBlockadePoints[iI].GetTargetX(), m_NavalBlockadePoints[iI].GetTargetY());
		if (pPlot != NULL)
		{
			//Only keep top 10 targets.
			if (pPlot->getImprovementType() != NO_IMPROVEMENT || iI < 10)
			{
				newTarget.SetTargetType(AI_TACTICAL_TARGET_BLOCKADE_POINT);
				newTarget.SetTargetX(pPlot->getX());
				newTarget.SetTargetY(pPlot->getY());
				newTarget.SetDominanceZone(GetTacticalAnalysisMap()->GetDominanceZoneID(pPlot->GetPlotIndex()));
				newTarget.SetAuxIntData(m_NavalBlockadePoints[iI].GetAuxIntData());
				m_AllTargets.push_back(newTarget);
			}
		}
	}
	m_NavalBlockadePoints.clear();
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
		//high prio goes first
		AssignGlobalHighPrioMoves();

		//then confront the enemy in each tactical zone
		for(int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
		{
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByIndex(iI);

			if(pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && !pZone->IsWater())
			{
				PlotEmergencyPurchases(pZone);
				PlotDefensiveAirlifts(pZone);
			}

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvCity* pZoneCity = pZone->GetZoneCity();
				strLogString.Format("Zone %d, %s, city of %s, posture %s",  
					pZone ? pZone->GetZoneID() : -1, pZone->IsWater() ? "water" : "land",
					pZoneCity ? pZoneCity->getNameNoSpace().c_str() : "none", 
					postureNames[pZone->GetPosture()]);
				LogTacticalMessage(strLogString);
			}

			ExtractTargetsForZone(pZone);

			switch (pZone->GetPosture())
			{
			case TACTICAL_POSTURE_NONE:
				break; //no posture assigned so do nothing; TODO: Maybe this should be unreachable?
			case TACTICAL_POSTURE_WITHDRAW: //give up
				PlotWithdrawMoves(pZone);
				break;
			case TACTICAL_POSTURE_HEDGEHOG: //defend
				PlotHedgehogMoves(pZone);
				break;
			case TACTICAL_POSTURE_ATTRITION: //low risk attacks on units
				PlotAttritionAttacks(pZone);
				break;
			case TACTICAL_POSTURE_EXPLOIT_FLANKS: //try to kill enemy units
				PlotExploitFlanksMoves(pZone);
				break;
			case TACTICAL_POSTURE_STEAMROLL: //attack everything including cities
				PlotSteamrollMoves(pZone);
				break;
			case TACTICAL_POSTURE_SURGICAL_CITY_STRIKE: //go for the city first
				PlotSurgicalCityStrikeMoves(pZone);
				break;
			case TACTICAL_POSTURE_COUNTERATTACK: //concentrated fire on enemy units
				PlotCounterattackMoves(pZone);
				break;
			}
		}

		//second pass: bring in reinforcements
		for (int iI = 0; iI < GetTacticalAnalysisMap()->GetNumZones(); iI++)
			PlotReinforcementMoves(GetTacticalAnalysisMap()->GetZoneByIndex(iI));

		//now mid prio moves like capturing barb camps, pillaging
		AssignGlobalMidPrioMoves();

		//finally arrange our remaining idle units for defense
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

	//try to make sure our most important fortifications have units in them
	PlotGarrisonMoves(1, true);
	PlotBastionMoves(1, true);
}

/// Choose which tactics to run and assign units to it
void CvTacticalAI::AssignGlobalMidPrioMoves()
{
	ExtractTargetsForZone(NULL);

	//air sweeps / attacks are already done during zone attacks, this is just for the remaining units
	PlotAirPatrolMoves();

	//score some goodies
	PlotGrabGoodyMoves();
	PlotCivilianAttackMoves();

	//make sure our frontline cities and fortresses have a garrison
	//garrisons sometimes make a sortie so we have to get them back
	PlotGarrisonMoves(3, false);
	PlotBastionMoves(2, false);

	//now all attacks are done, try to move any unprocessed units out of harm's way
	PlotMovesToSafety(true);

	//try again now that other blocking units might have moved
	PlotHealMoves(false);

	//harass the enemy (plundering also happens during combat sim ...)
	PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, true);
	PlotPlunderTradeUnitMoves(DOMAIN_LAND);
	PlotPlunderTradeUnitMoves(DOMAIN_SEA);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, true);
	PlotPillageMoves(AI_TACTICAL_TARGET_CITADEL, false);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, false);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, false);
	PlotBlockadeMoves();
}

/// Choose which tactics to run and assign units to it
void CvTacticalAI::AssignGlobalLowPrioMoves()
{
	ExtractTargetsForZone(NULL);

	//defense preparation for next turn
	PlotGuardImprovementMoves(2);

	//do this last after the units in need have already moved
	PlotNavalEscortMoves();

	//civilians move out of harms way last, when all potential defenders are set in place
	PlotMovesToSafety(false);
}

/// Choose which tactics to run and assign units to it (barbarian version)
void CvTacticalAI::AssignBarbarianMoves()
{
	//even barbarians like their camps
	PlotBarbarianCampDefense();

	//barbarians don't have tactical zones, they just attack everything that moves
	PlotBarbarianAttacks();
	PlotCivilianAttackMoves();

	//barbarians like to plunder as well
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE, true);
	PlotPillageMoves(AI_TACTICAL_TARGET_IMPROVEMENT, true);
	PlotPlunderTradeUnitMoves(DOMAIN_LAND);
	PlotPlunderTradeUnitMoves(DOMAIN_SEA);

	//normal roaming to find targets
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
			if (!pZone || (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && !pCity->isInDangerOfFalling()))
			{
				//try again with the water zone
				pZone = GetTacticalAnalysisMap()->GetZoneByCity(pCity, true);

				if (!pZone || (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY && !pCity->isInDangerOfFalling()))
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
				int iExpectedDamagePerTurn = ComputeTotalExpectedDamage(*pTarget);
				//actual siege will typically be longer because not all units actually attack the city each turn
				int iMaxSiegeTurns = 13; //do we even need a limit here or is this handled through the tactical posture?

				//assume the city heals each turn ...
				if ( (iExpectedDamagePerTurn - /*20 in CP, 8 in VP*/ GD_INT_GET(CITY_HIT_POINTS_HEALED_PER_TURN))*iMaxSiegeTurns < iRequiredDamage )
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

						//don't use too many far-away melee units
						//the combat sim can't deal with too many units
						//make sure there are ranged units in there as well
						if (iMeleeCount > 3 && m_CurrentMoveUnits.size()>9 && !pPlot->isAdjacent(pUnit->plot()))
							m_CurrentMoveUnits[iI].SetAttackStrength(-1);
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
					DeleteFocusArea(pPlot);

				//do we have embarked units we need to put ashore
				if (FindEmbarkedUnitsAroundTarget(pPlot, 4))
					ExecuteLandingOperation(pPlot);
			}
		}
	}
}

/// Assign a unit to capture an undefended barbarian camp
void CvTacticalAI::PlotGrabGoodyMoves()
{
	ClearCurrentMoveUnits(AI_TACTICAL_GOODY);

	//allow a fairly big range so we can clear islands as well unless we're at war and need the units otherwise
	//note the barbarians are excluded from that check
	int iRange = m_pPlayer->IsAtWarAnyMajor() ? 6 : 11;

	//ruins first
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_GOODY); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvUnit* pUnit =	FindUnitForThisMove(AI_TACTICAL_GOODY,pPlot,iRange);
		if (pUnit)
		{
			ExecuteMoveToPlot(pUnit, pPlot, false);

			if (pUnit->canMove())
				//can use this unit for other stuff, reset the tactmove to avoid spamming the log
				pUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
			else
				UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving %s %d to grab a goody, X: %d, Y: %d", pUnit->getName().c_str(), pUnit->GetID(), pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
	}

	//then barb camps, occupied or not
	ClearCurrentMoveUnits(AI_TACTICAL_BARBARIAN_CAMP);
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_BARBARIAN_CAMP); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (FindUnitsForHarassing(pPlot,iRange,-1,-1,DOMAIN_LAND,false,true,3))
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

void CvTacticalAI::ExecuteBarbarianTheft()
{
	vector<CvUnit*> vUsedUnits;
	for (std::list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		CvCity* pCity = pUnit->plot()->GetAdjacentCity();
		if (pCity)
		{
			if (CvBarbarians::DoTakeOverCityState(pCity) || CvBarbarians::DoStealFromCity(pUnit, pCity))
				vUsedUnits.push_back(pUnit);
		}
	}
	//have to do this in two steps to keep our iterator happy
	for (size_t i=0; i<vUsedUnits.size(); i++)
	{
		vUsedUnits[i]->finishMoves();
		UnitProcessed(vUsedUnits[i]->GetID());
	}
}

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

/// Assign a group of units to attack each unit we think we can destroy
void CvTacticalAI::ExecuteDestroyUnitMoves(AITacticalTargetType targetType, bool bMustBeAbleToKill, eAggressionLevel aggLvl)
{
	vector<STargetWithTwoScoresTiebreak> targets;

	// See how many moves of this type we can execute
	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(targetType, aggLvl); pTarget!=NULL; pTarget = GetNextZoneTarget(aggLvl))
	{
		// See what units we have who can reach targets this turn
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvUnit* pDefender = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
		if (pDefender && !pDefender->isDelayedDeath())
		{
			//if we're not about to make a kill, stay away from zones we are supposed to avoid
			if (!bMustBeAbleToKill)
			{
				CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pPlot);
				if (pZone && pZone->GetPosture() == TACTICAL_POSTURE_WITHDRAW)
					continue;
			}

			FindUnitsWithinStrikingDistance(pPlot);
			FindCitiesWithinStrikingDistance(pPlot);

			int iExpectedDamage = ComputeTotalExpectedDamage(*pTarget) + ComputeTotalExpectedCityBombardDamage(pDefender);
			if (iExpectedDamage>0)
			{
				int iRequiredDamage = pDefender->GetCurrHitPoints();

				//when in doubt attack the unit with fewer neighboring enemies first (can happen especially in naval combat)
				int iTiebreak = pPlot->countMatchingAdjacentPlots(NO_DOMAIN,NO_PLAYER,NO_PLAYER,NO_PLAYER) - pPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(), NO_DOMAIN);

				//also prefer to attack enemies we have a higher chance of killing (this should include garrisons while 'units within striking distance' ignores those)
				iTiebreak += m_pPlayer->GetPossibleAttackers(*pPlot, m_pPlayer->getTeam()).size();

				pTarget->SetAuxIntData(iRequiredDamage);
				targets.push_back(STargetWithTwoScoresTiebreak(pTarget,pPlot,iExpectedDamage-iRequiredDamage,iTiebreak) );
			}
		}
	}

	//we want to attacks the one we can do most damage to first
	std::stable_sort(targets.begin(), targets.end());
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

		//update this, some units might already have been used for other targets
		int iExpectedDamage = ComputeTotalExpectedDamage(*targets[i].pTarget) + ComputeTotalExpectedCityBombardDamage(pDefender);
		if (bMustBeAbleToKill)
		{
			if (iExpectedDamage < pDefender->GetCurrHitPoints())
				continue;
		}
		else
		{
			if (iExpectedDamage == 0)
				continue;
		}

		// Put in any attacks where we'll inflict at least equal damage
		if (GC.getLogging() && GC.getAILogging())
		{
			const char* task = bMustBeAbleToKill ? "killing" : "damaging";
			CvString strLogString;
			CvString strPlayerName = GET_PLAYER(pDefender->getOwner()).getCivilizationShortDescription();
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pDefender->getUnitType());
			CvString strTemp = (pkUnitInfo != NULL) ? pkUnitInfo->GetDescription() : "Unknown Unit Type";

			switch (targetType)
			{
			case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
				strLogString.Format("Looking at %s high priority %s, X: %d, Y: %d, ", task, strTemp.GetCString(), pDefender->getX(), pDefender->getY());
				break;
			case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
				strLogString.Format("Looking at %s medium priority %s, X: %d, Y: %d, ", task, strTemp.GetCString(), pDefender->getX(), pDefender->getY());
				break;
			case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
				strLogString.Format("Looking at %s low priority %s, X: %d, Y: %d, ", task, strTemp.GetCString(), pDefender->getX(), pDefender->getY());
				break;
			default:
				UNREACHABLE(); // Unsupported `targetType`.
			}
			strLogString += strPlayerName;
			LogTacticalMessage(strLogString);
		}

		//mark the target no matter if the attack succeeds
		targets[i].pTarget->SetLastAggLvl(aggLvl);

		//return true if target was killed
		if (ExecuteAttackWithCitiesAndGarrisons(pDefender))
			continue;

		ExecuteAttackWithUnits(targets[i].pPlot, aggLvl);
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
			// try to flee or hide
			int iDangerLevel = pUnit->GetDanger();
			if(iDangerLevel > 0 || pUnit->plot()->IsKnownVisibleToEnemy(m_pPlayer->GetID()))
			{
				bool bAddUnit = false;
				if(bCombatUnits)
				{
					if (!pUnit->IsCombatUnit() || pUnit->IsGarrisoned() || pUnit->getArmyID() != -1)
						continue;

					//if danger is high or we took a lot of damage last turn
					//counterintuitively, barbarians always flee, because if we get here it means we did not attack this turn!
					if(iDangerLevel>pUnit->GetMaxHitPoints() || pUnit->isProjectedToDieNextTurn() || pUnit->isBarbarian())
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
				}

				if(bAddUnit)
					m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
			}
		}
	}

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		ExecuteMovesToSafestPlot(pUnit);
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
	ExecuteBarbarianTheft();
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false, AL_BRAVEHEART);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false, AL_BRAVEHEART);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, AL_BRAVEHEART);
	ExecuteCaptureCityMoves();
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

		if (FindUnitsForHarassing(pPlot,1,GD_INT_GET(MAX_HIT_POINTS)/2,-1,eDomain,false,false,1))
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
	for (size_t i=0; i<m_pPlayer->getNumAIOperations(); i++)
	{
		CvAIOperation* pOp = m_pPlayer->getAIOperationByIndex(i);
		if (!pOp->DoTurn())
			opsToKill.push_back(pOp->GetID());
	}

	//clean up - have to do this in two steps so the iterator does not get invalidated
	for (size_t i=0; i<opsToKill.size(); i++)
		m_pPlayer->getAIOperation(opsToKill[i])->Kill();
}

/// Assigns units to pillage enemy improvements
void CvTacticalAI::PlotPillageMoves(AITacticalTargetType eTarget, bool bImmediate)
{
	ClearCurrentMoveUnits(AI_TACTICAL_PILLAGE);

	int iBaseDamage = /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT);
	CvString szTargetName = "";
	if(GC.getLogging() && GC.getAILogging())
	{
		if (eTarget == AI_TACTICAL_TARGET_CITADEL)
		{
			szTargetName = "Citadel";
			iBaseDamage = 0; //also undamaged units may pillage this
		}
		else if (eTarget == AI_TACTICAL_TARGET_IMPROVEMENT_RESOURCE)
		{
			szTargetName = "Improved Resource";
			iBaseDamage = 0; //also undamaged units may pillage this
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

		int iMinDamage = iBaseDamage;
		CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pPlot);
		if (pZone)
		{
			//do not move in to pillage when we are fleeing from the zone (but we may pillage while withdrawing)
			if (pZone->GetPosture() == TACTICAL_POSTURE_WITHDRAW)
				continue;

			if (pZone->IsWater())
				iMinDamage = 0;
			else if (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
				iMinDamage /= 2;
		}

		// Don't do it if an enemy unit became visible in the meantime
		if (pPlot->getVisibleEnemyDefender(m_pPlayer->GetID()) != NULL)
			continue;

		if (bImmediate)
		{
			// try paratroopers first, not because they are more effective, just because it looks cooler...
			if (eTarget != AI_TACTICAL_TARGET_IMPROVEMENT && FindParatroopersWithinStrikingDistance(pPlot,true))
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
			else if (FindUnitsForHarassing(pPlot, 0, GD_INT_GET(MAX_HIT_POINTS) / 3, GD_INT_GET(MAX_HIT_POINTS) - iMinDamage, DOMAIN_LAND, true, false, 1))
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
		else if (FindUnitsForHarassing(pPlot, 2, GD_INT_GET(MAX_HIT_POINTS) / 2, GD_INT_GET(MAX_HIT_POINTS) - iMinDamage, DOMAIN_LAND, false, false, 1))
		{
			//be careful when sending out single units ...
			CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pPlot);
			if (!pZone || pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
				continue;

			CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;
			if (pUnit && pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
			{
				ExecuteMoveToPlot(pUnit, pPlot, true, CvUnit::MOVEFLAG_NO_EMBARK | CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER);

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
		if (FindUnitsForHarassing(pPlot, 4, GD_INT_GET(MAX_HIT_POINTS)/2, -1, DOMAIN_SEA, false, false, 1))
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
		if(FindUnitsForHarassing(pPlot,1,GD_INT_GET(MAX_HIT_POINTS)/2,-1,DOMAIN_LAND,false,false,1))
		{
			for (size_t i = 0; i < m_CurrentMoveUnits.size(); i++)
			{
				CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[i].GetID());
				if (!pUnit || !pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_ATTACK))
					continue;
				//don't allow humans to use civilians as bait to lure units out of camps
				if (pUnit->GetDanger() == 0 || pUnit->plot()->getImprovementType()!=(ImprovementTypes)GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
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
						default:
							UNREACHABLE(); // Unsupported `eTargetType`.
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

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (!pUnit || !pUnit->canUseForTacticalAI())
			continue;

		if (pUnit->shouldHeal(bFirstPass))
			m_CurrentMoveUnits.push_back(CvTacticalUnit(pUnit->GetID()));
	}

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
			else if (currentDefender->IsCanAttackRanged())
			{
				//don't leave camp
				TacticalAIHelpers::PerformRangedOpportunityAttack(currentDefender);
				currentDefender->PushMission(CvTypes::getMISSION_SKIP());
			}
			else
			{
				//capture unprotected civilians if we can return to camp in the same turn
				CvPlot** neighbors = GC.getMap().getNeighborsShuffled(pPlot);
				for (int i = 0; i < 6; i++)
				{
					CvPlot* pNeighbor = neighbors[i];
					if (!pNeighbor)
						continue;

					if (!pNeighbor->isEnemyUnit(m_pPlayer->GetID(), true, true, true, true) &&
						pNeighbor->isEnemyUnit(m_pPlayer->GetID(), false, true, true, true) &&
						currentDefender->TurnsToReachTarget(pPlot,0,1)==0)
					{
						currentDefender->PushMission(CvTypes::getMISSION_MOVE_TO(), pNeighbor->getX(), pNeighbor->getY());
						currentDefender->PushMission(CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY());
						break;
					}
				}

				//melee may attack but never leave camp (because they won't return then)
				TacticalAIHelpers::PerformOpportunityAttack(currentDefender,false);
				currentDefender->PushMission(CvTypes::getMISSION_SKIP());
			}

			UnitProcessed(currentDefender->GetID());
		}
		else if (FindUnitsForHarassing(pPlot,5,-1,-1,DOMAIN_LAND,false,false,1))
		{
			CvUnit* pUnit = (m_CurrentMoveUnits.size() > 0) ? m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID()) : 0;
			ExecuteMoveToPlot(pUnit,pPlot);
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving to protect camp, X: %d, Y: %d", pTarget->GetTargetX(), pTarget->GetTargetY());
				LogTacticalMessage(strLogString);
			}
		}
	}
}

/// Make a defensive move to garrison a city
void CvTacticalAI::PlotGarrisonMoves(int iNumTurnsAway, bool bEmergencyOnly)
{
	ClearCurrentMoveUnits(AI_TACTICAL_GARRISON);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_CITY_TO_DEFEND); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		CvCity* pCity = pPlot->getPlotCity();
		if(!pCity)
			continue;

		if (bEmergencyOnly)
		{
			if (!pCity->isUnderSiege())
				continue;
		}
		else
		{
			// it's possible that the city did not perform a ranged attack this turn yet although enemies are present
			// this depends on the tactical posture ... so let's try again here as a safety net
			CvUnit* pEnemyPlot = pCity->getBestRangedStrikeTarget();
			if (pEnemyPlot)
				pCity->rangeStrike(pEnemyPlot->getX(), pEnemyPlot->getY());
		}

		//note that garrisons do not need to be "recruited" into tactical AI
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

			//note: ranged garrisons are also used in ExecuteDestroyUnit etc if they can hit a target without moving
			//here we have more advanced logic and allow some movement if it's safe (and it's not our last stand)
			TacticalAIHelpers::PerformOpportunityAttack(pGarrison, iEnemyCount < 2 && m_pPlayer->getNumCities() > 1 );

			//no need to call SetProcessed() because the unit was never in currentTurnUnits
			//do not call finishMoves() else the garrison will not heal!
			pGarrison->PushMission(CvTypes::getMISSION_SKIP());
			pGarrison->SetTurnProcessed(true);

			//don't try to find a better garrison while under siege
			if (pCity->isUnderSiege())
				continue;
		}

		//prefer ranged land units as garrisons ...
		bool bWantGarrison = (pGarrison == NULL || !pGarrison->isNativeDomain(pPlot) || !pGarrison->IsCanAttackRanged());
		if ( bWantGarrison && pCity->NeedsGarrison() )
		{
			// Grab units that make sense for this move type
			CvUnit* pUnit = FindUnitForThisMove(AI_TACTICAL_GARRISON, pPlot, iNumTurnsAway);
			if (pUnit)
			{
				//move out old garrison if necessary
				if (pGarrison && pUnit->CanSafelyReachInXTurns(pPlot, 0))
				{
					CvPlot* pFreePlot = pCity->GetPlotForNewUnit(pGarrison->getUnitType(), false);
					if (pFreePlot)
						//do not set it processed, it should be considered for other tactical moves later
						ExecuteMoveToPlot(pGarrison, pFreePlot, false);
				}

				//if the old garrison did not move out this will fail (possibly also for other reasons)
				int iTurnsLeft = ExecuteMoveToPlot(pUnit, pPlot);

				//if everything went according to plan ...
				if (iTurnsLeft != INT_MAX)
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Unit %d, moving to garrison, X: %d, Y: %d, Priority: %d, Turns Away: %d", 
							pUnit->GetID(), pTarget->GetTargetX(), pTarget->GetTargetY(), pTarget->GetAuxIntData(), iTurnsLeft);
						LogTacticalMessage(strLogString);
					}

					//just in case we're doing this with enemy arounds and have movement left
					TacticalAIHelpers::PerformOpportunityAttack(pUnit);

					//the new garrison came from the tactical AI current turn units, so need to mark it
					UnitProcessed(pUnit->GetID());
				}
			}
		}
	}
}

/// Establish a defensive bastion adjacent to a city
void CvTacticalAI::PlotBastionMoves(int iNumTurnsAway, bool bEmergencyOnly)
{
	ClearCurrentMoveUnits(AI_TACTICAL_GUARD);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_DEFENSIVE_BASTION); pTarget!=NULL; pTarget = GetNextZoneTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
		if (bEmergencyOnly && !TacticalAIHelpers::IsCloseToContestedBorder(m_pPlayer,pPlot))
			continue;

		// Grab units that make sense for this move type
		CvUnit* pUnit = FindUnitForThisMove(AI_TACTICAL_GUARD, pPlot, iNumTurnsAway);

		//move may fail if the plot is already occupied (can happen if another unit moved there during this turn)
		if (pUnit && ExecuteMoveToPlot(pUnit, pPlot, true, CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER)==0)
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
		if (pUnit && ExecuteMoveToPlot(pUnit, pPlot, true, CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER)==0)
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
void CvTacticalAI::PlotAirPatrolMoves()
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
				}
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAirPatrolMoves();
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

	// Sometimes buying a unit is useless
	bool bWantUnits = true;
	if (pCity->getDamage() * 2 > pCity->GetMaxHitPoints() && MOD_BALANCE_CORE_UNIT_CREATION_DAMAGED)
		bWantUnits = false;

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
			else if (bWantUnits)
			{
				//in water zones buy naval melee
				if (pZone->IsWater() && pCity->isCoastal())
					m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_ATTACK_SEA, pCity);
				else
					//otherwise buy defensive land units
					if (!MOD_AI_UNIT_PRODUCTION)
						m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(GC.getGame().randRangeExclusive(0, 5, CvSeeder(pCity->plot()->GetPseudoRandomSeed())) < 2 ? UNITAI_COUNTER : UNITAI_DEFENSE, pCity);
					else //AI Unit Production : Counter is AA only now
						m_pPlayer->GetMilitaryAI()->BuyEmergencyUnit(UNITAI_DEFENSE, pCity);
			}
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
void CvTacticalAI::PlotNavalEscortMoves()
{
	ClearCurrentMoveUnits(AI_TACTICAL_ESCORT);

	std::vector<CvUnit*> vTargetUnits;
	int iLoop = 0;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit->isEmbarked())
			vTargetUnits.push_back(pLoopUnit);
	}

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->canUseForTacticalAI() && !pUnit->shouldHeal(false))
		{
			// Am I a naval combat unit?
			if(pUnit->getDomainType() == DOMAIN_SEA && pUnit->IsCanAttack())
			{
				//any embarked unit close by?
				int iMaxDist = pUnit->getMoves();
				for (size_t i=0; i<vTargetUnits.size(); i++)
				{
					if (plotDistance(*pUnit->plot(),*vTargetUnits[i]->plot())<=iMaxDist)
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
		ExecuteEscortEmbarkedMoves(vTargetUnits);
	}
}

// PLOT MOVES FOR ZONE TACTICAL POSTURES

/// Win an attrition campaign with bombardments
void CvTacticalAI::PlotAttritionAttacks(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_ATTRITION);

	//todo: the targets are sorted in a very rough "how bad can they hit us" order
	//but we should probably sort them in a "how bad can we hit them" order
	//ExecuteAttritionAttacks() considers only the given target for city/garrison attacks

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteAttritionAttacks(*pTarget);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteAttritionAttacks(*pTarget);

	for (CvTacticalTarget* pTarget = GetFirstZoneTarget(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT); pTarget!=NULL && pTarget->IsTargetStillAlive(m_pPlayer->GetID()); pTarget = GetNextZoneTarget())
		ExecuteAttritionAttacks(*pTarget);

	//don't expose our units for city attacks here; if we are likely to succeed we don't call PlotAttritionAttacks
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

	//don't expose our units for city attacks here; if we are likely to succeed we don't call PlotExploitFlanksMoves
}

/// We have more overall strength than enemy, defeat his army first
void CvTacticalAI::PlotSteamrollMoves(CvTacticalDominanceZone* /*pZone*/)
{
	ClearCurrentMoveUnits(AI_TACTICAL_STEAMROLL);

	// See if there are any kill attacks we can make.
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, true, AL_HIGH);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, true);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, true);

	// We have superiority, so let's attack high prio targets even with bad odds
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false, AL_HIGH);
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
void CvTacticalAI::PlotHedgehogMoves(CvTacticalDominanceZone* pZone)
{
	ClearCurrentMoveUnits(AI_TACTICAL_HEDGEHOG);

	// Be careful with our units, we don't have so many
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT, false, AL_LOW);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT, false, AL_LOW);
	ExecuteDestroyUnitMoves(AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT, false, AL_LOW);

	// exception : early reinforcement before attacks in other zones are considered
	PlotReinforcementMoves(pZone);
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
			// Recent, healthy deployments need to attack!
			if (pUnit->IsRecentlyDeployedFromOperation() && (pUnit->GetCurrHitPoints() > pUnit->GetMaxHitPoints()/2))
				continue;

			// Am I in the current dominance zone?
			// Units in other dominance zones need to fend for themselves, depending on their own posture
			CvTacticalDominanceZone* pUnitZone = GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot());
			if (pUnitZone != pZone)
				continue;

			// However, zones might overlap borders, so double check that we don't give up our border forts
			if (pZone->GetTerritoryType() != TACTICAL_TERRITORY_FRIENDLY)
			{
				if (TacticalAIHelpers::IsPlayerCitadel(pUnit->plot(), pUnit->getOwner()))
					continue;

				if (pUnit->plot()->IsFriendlyTerritory(pUnit->getOwner()) && !pUnit->plot()->IsAdjacentOwnedByEnemy(pUnit->getTeam()))
					continue;
			}

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
	ClearCurrentMoveUnits(AI_TACTICAL_REINFORCE);

	//sometimes there is nothing to do ...
	if (!pTargetZone || pTargetZone->GetPosture() == TACTICAL_POSTURE_WITHDRAW)
		return;

	//don't try to reinforce wilderness zones
	CvCity* pZoneCity = pTargetZone->GetZoneCity();
	if (!pZoneCity)
		return;
	
	//don't try to reinforce neutral zones if there are no enemies
	if (pZoneCity->getTeam() != m_pPlayer->getTeam() && pTargetZone->GetTotalEnemyUnitCount()==0)
		return;

	//sometimes we do not need further reinforcement - should we check whether we still need siege units specifically?
	if (pTargetZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY && pTargetZone->GetRangedDominanceFlag(100) == TACTICAL_DOMINANCE_FRIENDLY)
		return;

	//sometimes it's pointless, too far out
	if (!pTargetZone->HasNeighborZone(m_pPlayer->GetID()) && pTargetZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
		return;

	// we want units which are somewhat close (so we don't deplete other combat zones) 
	// do not set a player - that way we can traverse unrevealed plots and foreign territory
	SPathFinderUserData data(NO_PLAYER, PT_ARMY_MIXED, NO_PLAYER, GetRecruitRange());
	CvPlot* pTargetPlot = pZoneCity->plot();

	ReachablePlots relevantPlots = GC.GetStepFinder().GetPlotsInReach(pTargetPlot, data);

	int iMoveUnitsAlreadyInZone = 0;
	for (ReachablePlots::iterator it = relevantPlots.begin(); it != relevantPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->iPlotIndex );
		for (int i = 0; i < pPlot->getNumUnits(); i++)
		{
			CvUnit* pUnit = pPlot->getUnitByIndex(i);
			if (pUnit->getOwner()==m_pPlayer->GetID() && pUnit->canUseForTacticalAI())
			{
				CvTacticalDominanceZone* pUnitZone = GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot());
				if (pUnitZone && pUnitZone != pTargetZone)
				{
					//we should not pull units from zones which need them
					if (pUnitZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && pUnitZone->GetPosture() != TACTICAL_POSTURE_WITHDRAW)
						if (!pPlot->isCity() || pPlot->getPlotCity()->isInDangerOfFalling())
							if (!pUnit->isEmbarked()) //cannot fight if embarked, so we can take it!
								continue;
				}

				// Carriers have special moves
				if (pUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA)
					continue;

				// Do not move siege units into enemy dominated zones ... wait until we have some cover!
				if (pUnit->AI_getUnitAIType() == UNITAI_CITY_BOMBARD && pTargetZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
					continue;

				//don't send generals / admirals across the map in an uncontrolled and potentially unescorted manner
				//it's enough if we recruit them for armies or combat sim
				if (pUnit->IsGreatGeneral() || pUnit->IsGreatAdmiral())
					continue;
				//conversely, don't leave generals out in the cold
				if (pUnit->IsCoveringFriendlyCivilian())
				{
					//a bit weird to just push a mission and not make an explicit plot/execute pair for this but should be ok
					pUnit->PushMission(CvTypes::getMISSION_SKIP());
					UnitProcessed(pUnit->GetID());
					continue;
				}

				// Proper domain of unit?
				// Note that coastal cities have two zones, so we will call this method twice
				if ((pTargetZone->IsWater() && pUnit->getDomainType() == DOMAIN_SEA) ||
					(!pTargetZone->IsWater() && pUnit->getDomainType() == DOMAIN_LAND))
				{
					// don't use near-dead units to attack ... misuse the flag here to be more careful when attacking
					if (pUnit->shouldHeal(pTargetZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY))
						continue;

					//don't run away if there's other work to do (will eventually be handled by ExecuteAttackWithUnits)
					bool bHaveFarTarget = false;
					vector<pair<CvPlot*,bool>> altTargets = TacticalAIHelpers::GetTargetsInRange(pUnit, false, false);
					for (size_t j = 0; j < altTargets.size(); j++)
						bHaveFarTarget |= (plotDistance(*altTargets[j].first, *pTargetPlot) > TACTICAL_COMBAT_MAX_TARGET_DISTANCE);

					if (bHaveFarTarget)
						continue;

					CvTacticalUnit unit(pUnit->GetID());
					unit.SetMovesToTarget(it->iPathLength);
					m_CurrentMoveUnits.push_back(unit);

					if (m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot()) == pTargetZone)
						iMoveUnitsAlreadyInZone++;
				}
			}
		}
	}

	//if we only have a single unit to work with in total, this is a case for pillage moves or the like
	if (pTargetZone->GetTotalFriendlyUnitCount() + m_CurrentMoveUnits.size() - iMoveUnitsAlreadyInZone < 2)
		return;

	if (m_CurrentMoveUnits.size() > 0)
	{
		vector<CvUnit*> vUnits;
		for (size_t i = 0; i < m_CurrentMoveUnits.size(); i++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit( m_CurrentMoveUnits[i].GetID() );
			if (pUnit->canUseNow())
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
			CvPlot* pSafePlot = pUnit->GetDanger()>pUnit->healRate(NULL) ? TacticalAIHelpers::FindSafestPlotInReach(pUnit, true) : NULL;
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
	if(pPlotDefender && pPlotDefender->getArmyID() == -1 && pPlotDefender->getDomainType() == pCivilian->getDomainType() && pPlotDefender->AI_getUnitAIType() != UNITAI_CITY_BOMBARD)
	{
		int iSlot = pThisArmy->RemoveUnit(pEscort->GetID(),true);
		if (iSlot>=0)
		{
			pThisArmy->AddUnit(pPlotDefender->GetID(), iSlot, true);
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("SingleHexOperationMoves: Switched escort to get things going.");
				GET_PLAYER(pCivilian->getOwner()).GetTacticalAI()->LogTacticalMessage(strLogString);
			}

			return pPlotDefender;
		}
		else
			CUSTOMLOG("SwitchEscort: Failed to remove unit from army!");
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
							ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(),true,CvUnit::MOVEFLAG_APPROX_TARGET_RING1|CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER);
							ExecuteMoveToPlot(pEscort, pOperation->GetTargetPlot(),true,CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

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
						if (ExecuteMoveToPlot(pEscort, pCivilian->plot())>0)
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
					//both must move
					CvPlot* pMuster = pOperation->GetMusterPlot();
					ExecuteMoveToPlot(pCivilian, pMuster);
					ExecuteMoveToPlot(pEscort, pOperation->GetMusterPlot(),true,pEscort->canMoveInto(*pMuster)?0:CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
				}

				if(pOperation->GetOperationState()!=AI_OPERATION_STATE_ABORTED && GC.getLogging() && GC.getAILogging())
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
		int iMoveFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER;
		//if necessary and possible, avoid plots where our escort cannot follow
		if (pEscort && !pOperation->GetTargetPlot()->isNeutralUnit(pEscort->getOwner(),true,true))
			iMoveFlags |= CvUnit::MOVEFLAG_DONT_STACK_WITH_NEUTRAL;
	
		// the escort leads the way
		bool bPathFound = false;
		CvString strLogString;
		if(pEscort)
		{
			//the target plot may be a city, so we need to check if the escort can actually go there
			//but the civilian uses the same flags so dump the escort when we're already there
			CvPlot* pTargetPlot = pOperation->GetTargetPlot();
			if (!pEscort->canMoveInto(*pTargetPlot) && !pEscort->plot()->isAdjacent(pTargetPlot))
				iMoveFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

			bool bHavePathEscort = pEscort->GeneratePath(pOperation->GetTargetPlot(), iMoveFlags);
			if(bHavePathEscort)
			{
				CvPlot* pCommonPlot = pEscort->GetPathEndFirstTurnPlot();
				//need to check if civilian can enter because of unrevealed tiles in path
				if(pCommonPlot != NULL && pCivilian->canMoveInto(*pCommonPlot,CvUnit::MOVEFLAG_DESTINATION))
				{
					int iTurns = INT_MAX;
					bool bHavePathCivilian = pCivilian->GeneratePath(pCommonPlot, iMoveFlags, 5, &iTurns);
					if (bHavePathCivilian)
					{
						bPathFound = true;

						if (iTurns > 0)
							//escort seems to be faster than the civilian, slow down
							pCommonPlot = pCivilian->GetPathEndFirstTurnPlot();

						//we know they can stack
						ExecuteMoveToPlot(pEscort, pCommonPlot);
						ExecuteMoveToPlot(pCivilian, pCommonPlot);

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
				//civilian leads the way since escort seems to be blocked
				//but maybe we can at least find a way for this turn
				bool bHavePathCivilian = pCivilian->GeneratePath(pOperation->GetTargetPlot(), iMoveFlags);
				if(bHavePathCivilian)
				{
					CvPlot* pCommonPlot = pCivilian->GetPathEndFirstTurnPlot();
					if(pCommonPlot != NULL)
					{
						int iTurns = INT_MAX;
						if (!pEscort->canMoveInto(*pCommonPlot))
							iMoveFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

						bool bHavePathEscort = pEscort->GeneratePath(pCommonPlot, iMoveFlags, 5, &iTurns);
						if (bHavePathEscort)
						{
							bPathFound = true;

							if (iTurns > 0)
								//civilian seems to be faster than the escort, slow down
								pCommonPlot = pEscort->GetPathEndFirstTurnPlot();

							//we know they can stack
							ExecuteMoveToPlot(pEscort, pCommonPlot);
							ExecuteMoveToPlot(pCivilian, pCommonPlot);
						}
						else
						{
							//our escort can't move into the next path plot. maybe it's blocked by a friendly unit?
							CvUnit* pNewEscort = SwitchEscort(pCivilian,pCommonPlot,pEscort,pThisArmy);
							if (pNewEscort)
							{
								ExecuteMoveToPlot(pCivilian, pCommonPlot);
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
				if (ExecuteMoveToPlot(pCivilian, pOperation->GetTargetPlot(), true, CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED)==INT_MAX)
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
			bool bHavePathCivilian = pCivilian->GeneratePath(pOperation->GetTargetPlot(), iMoveFlags);
			if(bHavePathCivilian)
			{
				CvPlot* pTurnTarget = pCivilian->GetPathEndFirstTurnPlot();
				if(pTurnTarget != NULL)
				{
					if (pCivilian->GetDanger(pTurnTarget) == INT_MAX)
					{
						CvPlot* pAlternativeTarget = TacticalAIHelpers::FindSafestPlotInReach(pCivilian, true, true);
						if (pAlternativeTarget)
							pTurnTarget = pAlternativeTarget;
					}
					else
					{
						//maybe we can find ourselves an escort!
						CvUnit* pDefender = pTurnTarget->getBestDefender(m_pPlayer->GetID());
						if (pDefender && pDefender->getArmyID() == -1 && pDefender->getDomainType() == pCivilian->getDomainType())
						{
							pThisArmy->AddUnit(pDefender->GetID(), 1, true);
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("SingleHexOperationMoves: Grabbed an escort along the way.");
							}
							pDefender->SetTurnProcessed(true);
						}
					}

					ExecuteMoveToPlot(pCivilian, pTurnTarget);
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

	//this may force detours, but whatever
	if (CheckForEnemiesNearArmy(pThisArmy))
	{
		//try to keep our units together, do not move on while there are enemies around, it's too dangerous
		pThisTurnTarget = pThisArmy->GetCenterOfMass(true);
		pOperation->LogOperationSpecialMessage("Contact with enemy!");
	}

	// RECRUITING
	if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE || 
		pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		// This is where we try to gather. Don't use the center of mass here, it may drift anywhere 
		ExecuteGatherMoves(pThisArmy,pThisTurnTarget);
	}

	// MOVING TO TARGET
	else if(pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		//if this operation has a specific target player
		if (pOperation->GetEnemy() != NO_PLAYER)
		{
			//getting too close to another enemy?
			if (GC.getGame().GetClosestCityDistanceInPlots(pThisTurnTarget) < 3)
			{
				PlayerTypes eCityOwner = GC.getGame().GetClosestCityOwnerByPlots(pThisTurnTarget);
				if (eCityOwner != pOperation->GetEnemy() && m_pPlayer->IsAtWarWith(eCityOwner))
					pOperation->SetToAbort(AI_ABORT_TOO_DANGEROUS);
			}
		}

		//try to arrange the units somewhat closer to the target
		ExecuteGatherMoves(pThisArmy,pThisTurnTarget);
	}
}

//workaround to make units from disbanded armies accessible to tactical AI in the same turn
void CvTacticalAI::AddCurrentTurnUnit(CvUnit * pUnit)
{
	if (pUnit && pUnit->canMove())
		m_CurrentTurnUnits.push_back( pUnit->GetID() );
}

//make sure our units come in a defined order (important for reproducability, don't want to sort pointers!)
struct PrSortByUnitId
{
	bool operator()(const CvUnit* lhs, const CvUnit* rhs) const { return lhs->GetID() < rhs->GetID(); }
};

/// Queues up attacks on enemy units on or adjacent to army's desired center
bool CvTacticalAI::CheckForEnemiesNearArmy(CvArmyAI* pArmy)
{
	if (!pArmy)
		return false;

	set<CvUnit*, PrSortByUnitId> ourUnitsInitial;
	CvUnit* pUnit = pArmy->GetFirstUnit();
	while (pUnit)
	{
		if (!pUnit->canUseNow() || pUnit->GetCurrHitPoints()<pUnit->GetMaxHitPoints() / 2)
		{
			pUnit = pArmy->GetNextUnit(pUnit);
			continue;
		}

		//who can attack us?
		vector<CvUnit*> vEnemyAttackers = m_pPlayer->GetPossibleAttackers(*pUnit->plot(), m_pPlayer->getTeam());
		if (vEnemyAttackers.empty())
		{
			pUnit = pArmy->GetNextUnit(pUnit);
			continue;
		}

		//this unit can be attacked, remember it
		ourUnitsInitial.insert(pUnit);

		for (size_t i = 0; i < vEnemyAttackers.size(); i++)
		{
			//now here's the trick, also include our non-army units which happen to be around
			vector<CvUnit*> vOurAttackersAndAllies = GET_PLAYER(vEnemyAttackers[i]->getOwner()).GetPossibleAttackers(*vEnemyAttackers[i]->plot(), vEnemyAttackers[i]->getTeam());
			for (size_t j = 0; j < vOurAttackersAndAllies.size(); j++)
				if (vOurAttackersAndAllies[j]->getOwner()==m_pPlayer->GetID())
					ourUnitsInitial.insert(vOurAttackersAndAllies[j]);
		}

		pUnit = pArmy->GetNextUnit(pUnit);
	}

	if (ourUnitsInitial.empty())
		return false;

	//now that we have a set of units find the center of mass
	int x = 0;
	int y = 0;
	set<CvPlot*, PrSortByPlotIndex> allEnemyPlots;
	for (set<CvUnit*, PrSortByUnitId>::iterator it = ourUnitsInitial.begin(); it != ourUnitsInitial.end(); ++it)
	{
		x += (*it)->getX();
		y += (*it)->getY();

		vector<CvUnit*> vEnemyAttackers = m_pPlayer->GetPossibleAttackers(*(*it)->plot(), m_pPlayer->getTeam());
		for (size_t i = 0; i < vEnemyAttackers.size(); i++)
			allEnemyPlots.insert(vEnemyAttackers[i]->plot());
	}
	x = (x * 100) / ourUnitsInitial.size();
	y = (y * 100) / ourUnitsInitial.size();

	//now find the closest enemy plot to our center of mass
	CvPlot* pCoM = GC.getMap().plot((x + 50) / 100, (y + 50) / 100);
	CvPlot* pClosestEnemyPlot = NULL;
	int iMinDist = INT_MAX;
	for (set<CvPlot*, PrSortByPlotIndex>::iterator it = allEnemyPlots.begin(); it != allEnemyPlots.end(); ++it)
	{
		int iDist = plotDistance(*pCoM, *(*it));
		if (iDist < iMinDist)
		{
			pClosestEnemyPlot = *it;
			iMinDist = iDist;
		}
	}

	//ignore units which are VERY far out; combat sim will ignore the "worst" units if necessary
	vector<CvUnit*> ourUnitsFinal;
	for (set<CvUnit*, PrSortByUnitId>::iterator it = ourUnitsInitial.begin(); it != ourUnitsInitial.end(); ++it)
	{
		if (plotDistance(*pCoM, *(*it)->plot()) < 7)
			ourUnitsFinal.push_back(*it);
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("Performing opportunity attack with army %d and friends", pArmy->GetID());
		LogTacticalMessage(strMsg);
	}

	//we probably didn't see all enemy units, so doublecheck ...
	//but take care to pick the right zone in the right domain
	CvCity* pClosestEnemyCity = GC.getGame().GetClosestCityByPlots(pClosestEnemyPlot, NO_PLAYER);
	CvTacticalDominanceZone* pZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(pClosestEnemyCity,pArmy->GetType()!=ARMY_TYPE_LAND);
	if (pZone && pZone->GetZoneCity() && pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
		return false;

	int iCount = 0;
	bool bSuccess = false;
	do
	{
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestUnitAssignments(ourUnitsFinal, pClosestEnemyPlot, AL_MEDIUM, gTactPosStorage);
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
		if (pUnit->canUseNow()) //ignore units used during CheckForEnemiesNearArmy
			vUnits.push_back(pUnit);

		pUnit = pArmy->GetNextUnit(pUnit);
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("Gathering army %d", pArmy->GetID());
		for (size_t i = 0; i < pArmy->GetNumFormationEntries(); i++)
		{
			CvArmyFormationSlot* slot = pArmy->GetSlotStatus(i);
			strMsg += CvString::format("; unit %d", slot->GetUnitID());
		}
		LogTacticalMessage(strMsg);
	}

	//we used to pass the army's target plot as a fallback target
	//but for sneak attacks the target plot may be unreachable
	//so we just go step by step
	PositionUnitsAroundTarget(vUnits, pTurnTarget);
}

// ROUTINES TO PROCESS AND SORT TARGETS

/// Mark units that can damage key items as priority targets
void CvTacticalAI::IdentifyPriorityTargets()
{
	//calculate this only once
	std::map<int,ReachablePlots> unitMovePlots;

	// Loop through each of our cities
	int iCityLoop = 0;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		// Compile a list of units that can attack it this turn and what their expected damage is
		vector<CvTacticalTarget*> possibleAttackers;
		int iExpectedTotalDamage = 0;

		//todo: should really use FindUnitsWithinStrikingDistance() here but it does not re-use the moveplots ...
		for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextUnitTarget())
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
			if(pEnemyUnit)
			{
				int iExpectedDamage = 0;

				if(pEnemyUnit->IsCanAttackRanged())
				{
					if (pEnemyUnit->canRangeStrikeAt(pLoopCity->getX(), pLoopCity->getY()))
						//can attack directly
						iExpectedDamage = pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false, 0, NULL, NULL, true, true);
					else
					{
						//have to move first
						ReachablePlots plots;
						std::map<int,ReachablePlots>::iterator it = unitMovePlots.find(pEnemyUnit->GetID());
						if (it==unitMovePlots.end())
						{
							plots = pEnemyUnit->GetAllPlotsInReachThisTurn(true, true, false);
							unitMovePlots[pEnemyUnit->GetID()] = plots;
						}
						else
							plots = it->second;

						//start from the outside
						for (int i=pEnemyUnit->GetRange(); i>0 && iExpectedDamage==0; i--)
						{
							std::vector<CvPlot*> vPlots = TacticalAIHelpers::GetPlotsForRangedAttack(pLoopCity->plot(),pEnemyUnit,i,false);

							for (std::vector<CvPlot*>::iterator it = vPlots.begin(); it != vPlots.end() && iExpectedDamage == 0; ++it)
								if (plots.find((*it)->GetPlotIndex()) != plots.end())
									iExpectedDamage = pEnemyUnit->GetRangeCombatDamage(NULL, pLoopCity, false, 0, NULL, NULL, true, true);
						}
					}
				}
				else
				{
					//melee always needs to move
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

		for (size_t i = 0; i < possibleAttackers.size(); i++)
		{
			CvPlot* pPlot = GC.getMap().plot( possibleAttackers[i]->GetTargetX(), possibleAttackers[i]->GetTargetY() );
			CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(m_pPlayer->GetID());

			//the siege units are the dangerous ones! cities should target them first
			//for counterattack with units the tactical combat simulation chooses attacks based on estimated damage
			if (pEnemyUnit && pEnemyUnit->AI_getUnitAIType()==UNITAI_CITY_BOMBARD)
				possibleAttackers[i]->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			else
				possibleAttackers[i]->SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
		}
	}
}

/// Mark units that can damage our barbarian camps as priority targets
void CvTacticalAI::IdentifyPriorityBarbarianTargets()
{
	if (!m_pPlayer->isBarbarian())
	{
		for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextUnitTarget())
		{
			CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());
			if (pPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
				pTarget->SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
		}
	}
	else
	{
		for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
			if(pLoopPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
			{
				for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextUnitTarget())
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

void CvTacticalAI::DumpTacticalTargets()
{
	for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextUnitTarget())
	{
		const char* prio = NULL;
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
		strMsg.Format("Enemy %s, id %d at (%d,%d) with prio %s, score %d, damage %d", pUnit->getName().c_str(), pUnit->GetID(), pTarget->GetTargetX(), pTarget->GetTargetY(),
			prio, pTarget->GetAuxIntData(), pUnit->getDamage());
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

		// Don't consider units that are already higher than low priority
		if(m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT)
		{
			//units which are in the front line should be medium at least
			if (pUnit->plot()->GetNumSpecificPlayerUnitsAdjacent(m_pPlayer->GetID())>0)
				m_AllTargets[iI].SetTargetType(AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT);
		}
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
	int iSuppressionRange = 2;
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
	int iMaxRadius = GetTacticalAnalysisMap()->GetMaxZoneRadius();

	m_ZoneTargets.clear();
	for(TacticalList::iterator it = m_AllTargets.begin(); it != m_AllTargets.end(); ++it)
	{
		//domain check
		if (pZone)
		{
			DomainTypes eDomain = pZone->IsWater() ? DOMAIN_SEA : DOMAIN_LAND;
			if (!it->IsTargetValidInThisDomain(eDomain))
				continue;
		}

		//zone match
		if(pZone == NULL || it->GetDominanceZone() == pZone->GetZoneID())
		{
			m_ZoneTargets.push_back(*it);
			continue;
		}

		//zone boundaries are arbitrary sometimes so include neighboring tiles as well for smaller zones
		if (pZone && plotDistance(pZone->GetCenterX(), pZone->GetCenterY(), it->GetTargetX(), it->GetTargetY()) <= iMaxRadius)
		{
			m_ZoneTargets.push_back(*it);
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
	//ignore visibility here so the AI doesn't go naively after revealed but invisible camps
	//and then a unit is stuck there without being able to attack
	if (pTargetPlot->isEnemyUnit(m_pPlayer->GetID(), true, false))
	{
		int nGoodAttackers = 0;
		vector<CvUnit*> vUnits;
		for (size_t i = 0; i < m_CurrentMoveUnits.size(); i++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[i].GetID());
			if (!pUnit)
				continue;

			//need at least one good attacker
			if (TacticalAIHelpers::IsAttackNetPositive(pUnit, pTargetPlot))
				nGoodAttackers++;

			vUnits.push_back(pUnit);

			//don't use too many units
			if (nGoodAttackers > 2)
				break;
		}

		//just get into position, we will attack next turn when in place
		if (nGoodAttackers>1)
			PositionUnitsAroundTarget(vUnits, pTargetPlot);
	}
	else
	{
		for (size_t i = 0; i < m_CurrentMoveUnits.size(); i++)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[i].GetID());
			if (!pUnit)
				continue;

			//try to get into position
			//some of the camps the player has revealed may since have been cleared ... but we need to check
			//if the camp has been cleared there might be a neutral unit in the plot and our pathfinding could fail without the approximate flag!
			ExecuteMoveToPlot(pUnit, pTargetPlot, false, CvUnit::MOVEFLAG_APPROX_TARGET_RING1|CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER);

			if (pUnit->canMove())
			{
				//capture the camp if it still exists - if there is an enemy then we'll attack later
				if (pTargetPlot->GetNumCombatUnits()==0)
					ExecuteMoveToPlot(pUnit, pTargetPlot, false);

				//can use this unit for other moves, reset the tactmove to avoid spamming the log
				pUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
			}
			else
				UnitProcessed(pUnit->GetID());

			if (pUnit->plot() == pTargetPlot)
				break;
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
				ExecuteMoveToPlot(pUnit, pTargetPlot, false, CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED);

				//now that the neighbor plots are revealed, maybe it's better to retreat?
				CvPlot* pSafePlot = NULL;
				if (!TacticalAIHelpers::IsOtherPlayerCitadel(pUnit->plot(), m_pPlayer->GetID(), true))
				{
					int iVal = pUnit->GetCurrHitPoints() + /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT);
					if (pUnit->IsGainsXPFromPillaging())
						iVal += /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT);

					if ((pUnit->GetDanger() > iVal) && !pUnit->hasFreePillageMove())
						pSafePlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
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
					//it's a ranged attack but it uses the move mission ... air units are strange
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("%s ATTACK: %s %d attacks target X: %d, Y: %d", pUnit->isSuicide() ? "MISSILE":"AIR" , pUnit->getName().c_str(), pUnit->GetID(), pBestTarget->getX(), pBestTarget->getY());
						LogTacticalMessage(strMsg);
					}
				}
				iCount++;
			}

			if (pUnit->getNumAttacks() - pUnit->getNumAttacksMadeThisTurn() == 0)
				UnitProcessed(m_CurrentMoveUnits[iI].GetID());
		}
	}
}

/// Queues up attacks on enemy units on or adjacent to army's desired center
CvPlot* CvTacticalAI::FindAirTargetNearTarget(CvUnit* pUnit, CvPlot* pApproximateTargetPlot)
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
			m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_CITY)
		{
			//make sure it is close to our actual target plot
			if (pApproximateTargetPlot)
			{
				int iTargetDistance = plotDistance(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY(), pApproximateTargetPlot->getX(), pApproximateTargetPlot->getY());
				if (iTargetDistance > 3)
					continue;
			}

			int iDistance = plotDistance(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY(), pUnit->getX(), pUnit->getY());
			if (iDistance <= iRange)
			{
				CvPlot* pTestPlot = GC.getMap().plot(m_AllTargets[iI].GetTargetX(), m_AllTargets[iI].GetTargetY());
				if (pTestPlot == NULL)
					continue;

				//don't beat a dead horse
				CvCity *pCity = pTestPlot->getPlotCity();
				if (pCity && pCity->getDamage() > pCity->GetMaxHitPoints() - 10)
					continue;

				CvUnit* pDefender = pUnit->rangeStrikeTarget(*pTestPlot, true);
				if (!pDefender)
					continue;

				int iValue = 0;
				if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT)
					iValue += 23;
				if (m_AllTargets[iI].GetTargetType() == AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT)
					iValue += 11;

				if (pUnit->AI_getUnitAIType() == UNITAI_MISSILE_AIR)
				{
					//ignore the city when attacking!
					iValue = pUnit->GetAirCombatDamage(pDefender, NULL, false);
					//bonus for a kill
					if (pDefender->GetCurrHitPoints() <= iValue)
						iValue += 21;
					//bonus for hitting units in cities, can only do that with missiles
					if (pDefender->plot()->isCity())
						iValue += 17;
				}
				else
				{
					//use distance as tiebreaker
					iValue = pUnit->GetAirCombatDamage(pDefender, pCity, false) - iDistance * 3;

					if (pCity != NULL)
					{
						iValue /= 2; //prefer attacking units ...
						iValue -= pCity->GetAirStrikeDefenseDamage(pUnit, false);
					}
					else
						iValue -= pDefender->GetAirStrikeDefenseDamage(pUnit, false);

					if (pTestPlot->GetBestInterceptor(pUnit->getOwner(),NULL,false,true) != NULL)
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
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Starting air sweep with %s %d before attack on X: %d, Y: %d", pUnit->getName().c_str(), pUnit->GetID(), pTargetPlot->getX(), pTargetPlot->getY());
				LogTacticalMessage(strMsg);
			}
		}
	}
}

bool CvTacticalAI::ExecuteSpotterMove(const vector<CvUnit*>& vUnits, CvPlot* pTargetPlot)
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
		case UNITAI_SKIRMISHER:
		case UNITAI_ATTACK_SEA:
		case UNITAI_SUBMARINE:
		case UNITAI_ATTACK:
		case UNITAI_COUNTER:
			vCandidates.push_back(pUnit);
			break;
		default:
			break; // Not a candidate.
		}
	}

	vector<OptionWithScore<pair<CvUnit*, CvPlot*>>> vOptions;

	for (size_t i = 0; i < vCandidates.size(); i++)
	{
		CvUnit* pUnit = vCandidates[i];
		int iFlags = CvUnit::MOVEFLAG_NO_EMBARK;

		//move into ring 2 unless we are already there and still can't see the target
		iFlags |= plotDistance(*pTargetPlot, *pUnit->plot()) > 2 ? CvUnit::MOVEFLAG_APPROX_TARGET_RING2 : CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

		if (pUnit->GeneratePath(pTargetPlot, iFlags, 2))
		{
			//try to see if we have a plot we can reach this turn and see the target
			const CvPathNodeArray& path = pUnit->GetLastPath();
			for (size_t i = 0; i < path.size(); i++)
			{
				if (path[i].m_iMoves==0) //want some movement left to retreat if required
					break;

				CvPlot* pPathPlot = GC.getMap().plotUnchecked(path[i].m_iX, path[i].m_iY);
				if (pPathPlot->canSeePlot(pTargetPlot, pUnit->getTeam(), pUnit->visibilityRange(), NO_DIRECTION))
				{
					//or should we use danger as the sorting criterion?
					vOptions.push_back(OptionWithScore<pair<CvUnit*, CvPlot*>>(make_pair(pUnit,pPathPlot),path[i].m_iMoves));
				}
			}
		}
	}

	if (vOptions.empty())
		return false;

	std::stable_sort(vOptions.begin(), vOptions.end());
	ExecuteMoveToPlot(vOptions.front().option.first, vOptions.front().option.second, false, CvUnit::MOVEFLAG_NO_EMBARK);
	return true;
}

bool CvTacticalAI::ExecuteAttackWithCitiesAndGarrisons(CvUnit* pDefender)
{
	// Start by applying damage from city bombards
	for (unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
	{
		CvCity* pCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		if (!pCity)
			continue;

		if (pCity->canRangeStrikeAt(pDefender->getX(), pDefender->getY()) && !pCity->isMadeAttack())
		{
			pCity->doTask(TASK_RANGED_ATTACK, pDefender->getX(), pDefender->getY(), false);
			if (pDefender->GetCurrHitPoints() < 1)
				return true;
		}

		//special handling for ranged garrisons as they are ignored by tactical AI
		//note: melee garrisons are handled in PlotGarrisonMoves()
		CvUnit* pGarrison = pCity->GetGarrisonedUnit();
		if (pGarrison)
		{
			if (pGarrison->shouldHeal(true))
			{
				pGarrison->PushMission(CvTypes::getMISSION_SKIP());
				pGarrison->SetTurnProcessed(true);
			}
			else
			{
				//may have multiple attacks ...
				while (pGarrison->canRangeStrikeAt(pDefender->getX(), pDefender->getY()))
					pGarrison->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pDefender->getX(), pDefender->getY());

				//if we have attacks left, we can try again in PlotGarrisonMoves() with other target
				if (pGarrison->isOutOfAttacks())
					pGarrison->SetTurnProcessed(true);
			}
		}

		if (pDefender->GetCurrHitPoints() < 1)
			return true;
	}

	//not killed
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
		if (m_CurrentMoveUnits[i].GetAttackStrength()>=0) //sometimes we mark units as unnecessary
			vUnits.push_back( m_pPlayer->getUnit( m_CurrentMoveUnits[i].GetID() ) );

	//try to improve visibility
	if (!ExecuteSpotterMove(vUnits,pTargetPlot))
		return false;

	//first handle air units (including missiles)
	ExecuteAirSweep(pTargetPlot);
	ExecuteAirAttack(pTargetPlot);

	//did the air attack already kill the enemy?
	if (pTargetPlot->getBestDefender(NO_PLAYER, m_pPlayer->GetID(), NULL, true) == NULL && !pTargetPlot->isCity())
		return true;

#if defined(MOD_CORE_DEBUGGING)
	if (MOD_CORE_DEBUGGING)
		LogTacticalMessage(CvString::format("trying attack on %d:%d, agg level %d", pTargetPlot->getX(), pTargetPlot->getY(), eAggLvl));
#endif

	int iCount = 0;
	bool bSuccess = false;
	do
	{
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestUnitAssignments(vUnits, pTargetPlot, eAggLvl, gTactPosStorage);
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
bool CvTacticalAI::PositionUnitsAroundTarget(const vector<CvUnit*>& vUnits, CvPlot* pTarget)
{
	//try to improve visibility. however, if the target is too far away this may fail ... in that case we chance it
	ExecuteSpotterMove(vUnits, pTarget);

	if (MOD_CORE_DEBUGGING)
		LogTacticalMessage(CvString::format("seeking defensive positioning around %d:%d", pTarget->getX(), pTarget->getY()));

	//first round: in case there are enemies around, do a combat simulation
	int iCount = 0;
	bool bSuccess = false;
	do
	{
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestUnitAssignments(vUnits, pTarget, AL_NONE, gTactPosStorage);
		if (vAssignments.empty())
			break;
		
		//true if everything went according to plan. if not, repeat!
		bSuccess = TacticalAIHelpers::ExecuteUnitAssignments(m_pPlayer->GetID(), vAssignments);
		iCount++;
	}
	while (!bSuccess && iCount < 4);

	//sometimes tactsim cannot use all units, eg if they are too far out
	vector<CvUnit*> farout;
	bool bHaveNavalEscort = false;
	for (vector<CvUnit*>::const_iterator it = vUnits.begin(); it != vUnits.end(); ++it)
	{
		CvUnit* pUnit = *it;

		//also include units we already moved ...
		if (pUnit->IsCombatUnit() && pUnit->getDomainType() == DOMAIN_SEA)
			bHaveNavalEscort = true;

		if (pUnit->TurnProcessed())
			continue;
		
		if (bSuccess && plotDistance(*pTarget, *pUnit->plot()) <= TACTICAL_COMBAT_MAX_TARGET_DISTANCE)
			continue; //do not end the turn ... we may want to shuffle them around later

		farout.push_back(pUnit);
	}

	//we want to move the civilians last so they have a better chance of getting cover
	struct PrSortCombatFirst
	{
		bool operator()(const CvUnit* lhs, const CvUnit* rhs) const { return (lhs->IsCombatUnit() ? 0 : 1) < (rhs->IsCombatUnit() ? 0 : 1); }
	};
	std::stable_sort(farout.begin(), farout.end(), PrSortCombatFirst());

	//second round: move in as long as there is no danger and we're still far away
	for (vector<CvUnit*>::const_iterator it = farout.begin(); it != farout.end(); ++it)
	{
		//lots of flags ...
		CvUnit* pUnit = *it;
		int	iFlags = CvUnit::MOVEFLAG_NO_STOPNODES | CvUnit::MOVEFLAG_APPROX_TARGET_RING2;
		if (pUnit->isNativeDomain(pTarget)) //don't embark if we don't have to
			iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
		if (pUnit->IsCivilianUnit())
			iFlags |= (CvUnit::MOVEFLAG_DONT_STACK_WITH_NEUTRAL | CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER);
		if (!bHaveNavalEscort && pUnit->getDomainType()==DOMAIN_LAND)
			iFlags |= CvUnit::MOVEFLAG_NO_EMBARK;

		//since we know the unit was far out originally, this is guaranteed to be actual movement
		if (!pUnit->GeneratePath(pTarget, iFlags, GetRecruitRange()))
			continue;

		//we are not here to fight or flee, let other moves take over
		int iDanger = pUnit->GetDanger(pUnit->GetPathEndFirstTurnPlot());
		int iDangerLimit = pUnit->IsCanAttackWithMove() ? pUnit->GetCurrHitPoints() / 2 : 0;
		//generals and siege should not even be in fog danger
		if (iDanger > iDangerLimit)
			continue;

		//embark only when it's safe
		CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->GetPathEndFirstTurnPlot());
		if (pZone && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY && !pUnit->isEmbarked())
			iFlags |= CvUnit::MOVEFLAG_NO_EMBARK;

		ExecuteMoveToPlot(pUnit, pTarget, true, iFlags);
	}

	//third round: if the unit is in an army (no tactical moves) and did not move yet, move it to safety now
	for (vector<CvUnit*>::const_iterator it = vUnits.begin(); it != vUnits.end(); ++it)
	{
		CvUnit* pUnit = *it;
		//don't move in further if we're already close
		if (pUnit->TurnProcessed() || pUnit->getArmyID() == -1)
			continue;

		//only flee if we're in danger
		if (pUnit->GetDanger() > pUnit->healRate(NULL) || (!pUnit->IsCombatUnit() && pUnit->plot()->getNumDefenders(pUnit->getOwner()) == 0))
			ExecuteMovesToSafestPlot(pUnit);
		else
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->SetTurnProcessed(true);
		}
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
		bool operator<(const SAssignment& rhs) const { return iScore>rhs.iScore; }
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
			if (pUnit->IsCanAttackRanged())
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

			if (bAttack && pUnit->IsCanAttackWithMove())
			{
				//check if attack makes sense
				if (TacticalAIHelpers::IsAttackNetPositive(pUnit,pEvalPlot))
				{
					choices.push_back( SAssignment(pUnit,pEvalPlot,pUnit->GetMaxHitPoints()+1,true) );
				}
			}
			else if (!bAttack)
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
	std::stable_sort(choices.begin(),choices.end());
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
	for (int i = 0; i < iNumPlots - 1; i++)
	{
		int iSwapIndex = GC.getGame().randRangeExclusive(0, iNumPlots - i, CvSeeder(m_CurrentMoveUnits.size()).mix(i));
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

			if (pUnit->IsCanAttackRanged())
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
void CvTacticalAI::ExecuteMovesToSafestPlot(CvUnit* pUnit)
{
	if (!pUnit)
		return;

	//see if we can do damage before retreating
	if (pUnit->canMoveAfterAttacking() && pUnit->getMoves()>GD_INT_GET(MOVE_DENOMINATOR) && pUnit->canRangeStrike())
		TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit,true);

	//so easy
	CvPlot* pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true, true);
	if (pBestPlot != NULL)
	{
		//check if we need to bump somebody else
		CvUnit* pBumpUnit = pUnit->GetPotentialUnitToPushOut(*pBestPlot);
		if (pBumpUnit)
		{
			if (pUnit->PushBlockingUnitOutOfPlot(*pBestPlot))
			{
				UnitProcessed(pUnit->GetID());
				return;
			}
		}

		//pillage before retreat, if we have movement points to spare
		if (pUnit->getMoves()>GD_INT_GET(MOVE_DENOMINATOR) && pBestPlot->isAdjacent(pUnit->plot()) && pUnit->shouldPillage(pUnit->plot()))
			pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

		//typical citadel case
		if (pUnit->plot() == pBestPlot)
		{
			if (pUnit->GetCurrHitPoints()>pUnit->GetMaxHitPoints()/2)
				TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit);

			//make sure the unit stays put!
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
		}
		else
		{
			//try to do some damage if we have movement points to spare
			if (pBestPlot->isAdjacent(pUnit->plot()) && pUnit->getMoves() > GD_INT_GET(MOVE_DENOMINATOR) && pUnit->canRangeStrike() && pUnit->canMoveAfterAttacking())
				TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit, true);

			// Move to the lowest danger value found
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), 0, false, false, MISSIONAI_TACTMOVE);

			//see if we can do damage after retreating
			if (pUnit->canMove() && pUnit->canRangeStrike())
				TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit, true);

			//pillage after retreat, if we have movement points to spare
			if (pUnit->shouldPillage(pUnit->plot()))
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
		}

		UnitProcessed(pUnit->GetID());
	}
	else if (pUnit->canHeal(pUnit->plot()) && pUnit->GetDanger()<pUnit->GetCurrHitPoints())
	{
		//do nothing and hope for the best
		pUnit->PushMission(CvTypes::getMISSION_SKIP());
		UnitProcessed(pUnit->GetID());
	}
	else //no good plot found
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
			strLogString.Format("Failed to find destination moving %s to safety from, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
			LogTacticalMessage(strLogString);
		}

		//try to go home
		if(pUnit->plot()->getOwner() != pUnit->getOwner())
		{
			CvCity* pClosestCity = m_pPlayer->GetClosestCityByPathLength(pUnit->plot());
			if (m_pPlayer->isMinorCiv())
				pClosestCity = m_pPlayer->getCapitalCity();

			CvPlot* pMovePlot = pClosestCity ? pClosestCity->plot() : NULL;
			if(pMovePlot != NULL)
				MoveToEmptySpaceNearTarget(pUnit,pMovePlot,DOMAIN_LAND,42,true);
			else
				pUnit->PushMission(CvTypes::getMISSION_SKIP());

			UnitProcessed(pUnit->GetID());
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
					pArmy->RemoveUnit(pUnit->GetID(), false);
			}
		}

		//ranged attack before fleeing for fast units
		if (pUnit->canMoveAfterAttacking() && pUnit->getMoves() > 3*GD_INT_GET(MOVE_DENOMINATOR) && pUnit->canRangeStrike())
			TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit);

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
			else if (pUnit->GetDamageAoEFortified() > 0 && pUnit->canFortify(pUnit->plot()) &&
				pUnit->getDamage() >= pUnit->healRate(pUnit->plot()) && pUnit->GetDanger() < pUnit->GetCurrHitPoints() &&
				pUnit->plot()->GetNumEnemyUnitsAdjacent(pUnit->getTeam(), pUnit->getDomainType()) > 0)
			{
				//units with area damage if fortified should fortify as much as possible if near enemies
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				UnitProcessed(pUnit->GetID());
				continue;
			}

			//land unit on land should move away from danger
			if (pUnit->GetDanger() > 0 && !pUnit->isEmbarked() && !pUnit->IsGarrisoned())
			{
				//unless we can eliminate the danger!
				std::vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pUnit->plot(),m_pPlayer->getTeam());
				if (vAttackers.size() == 1 && TacticalAIHelpers::KillLoneEnemyIfPossible(pUnit, vAttackers[0]))
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
				if (vAttackers.size() == 1 && TacticalAIHelpers::KillLoneEnemyIfPossible(pUnit, vAttackers[0]))
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
				CvUnit* pPushUnit = pUnit->GetPotentialUnitToPushOut(*pBetterPlot);
				if (pPushUnit)
					pUnit->PushBlockingUnitOutOfPlot(*pBetterPlot);
				else //plot should be free
					ExecuteMoveToPlot(pUnit, pBetterPlot);
			}
			else
				//this is required to flush the previous mission!
				pUnit->PushMission(CvTypes::getMISSION_SKIP());

			UnitProcessed(pUnit->GetID());
		}
		//no safe plot to heal ...
		else if (!bFirstPass && pUnit->getDomainType() != DOMAIN_AIR && pUnit->GetDanger() > /*10*/ GD_INT_GET(NEUTRAL_HEAL_RATE))
		{
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

			//at least try to flee if we're not needed
			if (!pUnit->IsCoveringFriendlyCivilian())
			{
				pBetterPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
				if (pBetterPlot && pBetterPlot != pUnit->plot())
					ExecuteMoveToPlot(pUnit, pBetterPlot);
			}

			if (!pUnit->canMove())
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
				if(pPlot && (pPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT) || pPlot->isCity()))
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
				bool bTargetIsUnit = pBestPlot->isEnemyUnit(BARBARIAN_PLAYER, true, true);

				if (!bTargetIsUnit && pBestPlot->isEnemyUnit(BARBARIAN_PLAYER, false, true))
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
					bMoved = pUnit->at(pBestPlot->getX(), pBestPlot->getY());
				}
				//if we're already adjacent, we cannot move closer
				//in fact, since we apparently did not attack this turn, we should flee (handled later)
				else if (!bTargetIsUnit || plotDistance(*pBestPlot,*pUnit->plot()) > 1 )
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
				if(!pBestPlot)
					continue;

				//no naval pillaging, it's just too annoying
				//same logic as above, if we're already at the target we should flee instead of staying
				bool bTargetIsUnit = pBestPlot->isEnemyUnit(BARBARIAN_PLAYER, true, true);
				if (!bTargetIsUnit || plotDistance(*pBestPlot, *pUnit->plot()) > 1)
				{
					if (MoveToEmptySpaceNearTarget(pUnit, pBestPlot, DOMAIN_SEA, 12))
					{
						TacticalAIHelpers::PerformOpportunityAttack(pUnit, true);
						UnitProcessed(m_CurrentMoveUnits[iI].GetID());
					}
				}
			}
		}
	}
}

/// Move unit to a specific tile, return turns remaining
int CvTacticalAI::ExecuteMoveToPlot(CvUnit* pUnit, CvPlot* pTarget, bool bSetProcessed, int iFlags)
{
	int iResult = INT_MAX; //impossible

	if(!pUnit || !pTarget)
		return iResult;

	//for inspection in GUI
	pUnit->SetMissionAI(MISSIONAI_TACTMOVE, pTarget, NULL);

	// Unit already at target plot?
	if(pTarget == pUnit->plot() && pUnit->canEndTurnAtPlot(pTarget))
	{
		iResult = 0;

		TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit);
		pUnit->PushMission(CvTypes::getMISSION_SKIP());
	}
	else if (pUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION) || (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING1) || (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING2))
	{
		int iTurns = INT_MAX;
		if (pUnit->GeneratePath(pTarget,iFlags,INT_MAX,&iTurns))
		{
			//pillage if it makes sense and we have movement points to spare
			if (pUnit->shouldPillage(pUnit->plot(), true) && pUnit->GetMovementPointsAtCachedTarget()>=GD_INT_GET(MOVE_DENOMINATOR))
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
			iResult = iTurns - 1;

			bool bAlreadyThere = false;
			if (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING2)
				bAlreadyThere = (plotDistance(*pUnit->plot(),*pTarget)<3);
			else if  (iFlags&CvUnit::MOVEFLAG_APPROX_TARGET_RING1)
				bAlreadyThere = (plotDistance(*pUnit->plot(),*pTarget)<2);
			else
				bAlreadyThere = pUnit->at(pTarget->getX(), pTarget->getY());

			if (!bAlreadyThere && pUnit->canMove()) //typically because of MOVEFLAG_ABORT_IN_DANGER and newly revealed enemies ...
			{
				pTarget = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
				if (pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), 0 /*no approximate flags*/, false, false, MISSIONAI_TACTMOVE, pTarget);
					UnitProcessed(pUnit->GetID());
				}
			}
		}
		//maybe units are blocking our way? 
		else if (pUnit->GeneratePath(pTarget,iFlags|CvUnit::MOVEFLAG_IGNORE_STACKING_SELF,INT_MAX,&iTurns))
		{
			//already close? try to push the other unit out
			if (iTurns == 0)
			{
				CvUnit* pPushUnit = pUnit->GetPotentialUnitToPushOut(*pTarget);
				if (pPushUnit && pUnit->PushBlockingUnitOutOfPlot(*pTarget))
					iResult = 0;
			}
			else
			{
				//try to find a good plot in the direction of the target and hope the block clears
				CvPlot* pWorkaround = pUnit->GetLastValidDestinationPlotInCachedPath();
				if (pWorkaround)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), iFlags, false, false, MISSIONAI_TACTMOVE, pTarget);
					if (bSetProcessed || !pUnit->canMove())
						UnitProcessed(pUnit->GetID());
					iResult = iTurns-1;
				}
			}
		}

		if(iResult==0 && pUnit->canMove())
			TacticalAIHelpers::PerformRangedOpportunityAttack(pUnit);
	}

	if (bSetProcessed)
		UnitProcessed(pUnit->GetID());

	return iResult;
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
				CvString strTemp;
				CvString strLogString;
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

			//safety check
			if (pUnit->GetDanger(pTarget) <= pUnit->GetCurrHitPoints())
			{
				//make sure we have a valid path
				if (!pUnit->GeneratePath(pTarget))
					continue;

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
void CvTacticalAI::ExecuteAirPatrolMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			if(pUnit->canAirPatrol(NULL))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Starting air patrol at, X: %d, Y: %d with %s %d", pUnit->getX(), pUnit->getY(), pUnit->getName().c_str(), pUnit->GetID());
					LogTacticalMessage(strLogString);
				}

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
bool CvTacticalAI::ExecuteAttritionAttacks(CvTacticalTarget& kTarget)
{
	//mark the target no matter if the attack succeeds
	kTarget.SetLastAggLvl(AL_LOW);

	//todo: how can we make sure that city and units focus fire on the same targets?
	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());
	CvUnit* pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if(pDefender)
	{
		if(pDefender->GetCurrHitPoints() < 1)
			return false;

		// If this is a unit target we might also be able to hit it with a city
		bool bCityCanAttack = FindCitiesWithinStrikingDistance(pTargetPlot);
		if (bCityCanAttack && ExecuteAttackWithCitiesAndGarrisons(pDefender))
			return true;

		// Make attacks - this includes melee attacks but only very safe ones
		if (FindUnitsWithinStrikingDistance(pTargetPlot) && ComputeTotalExpectedDamage(kTarget)>0)
			return ExecuteAttackWithUnits(pTargetPlot, AL_LOW);
	}

	return false;
}

/// Take a multi-hex attack on an enemy unit this turn
bool CvTacticalAI::ExecuteFlankAttack(CvTacticalTarget& kTarget)
{
	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());

	bool bLoneBarbInCamp = (pTargetPlot->getRevealedImprovementType(m_pPlayer->getTeam()) == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT)) &&
		(pTargetPlot->GetNumEnemyUnitsAdjacent(m_pPlayer->getTeam(), NO_DOMAIN) == 0);
	eAggressionLevel aggLevel = bLoneBarbInCamp ? AL_HIGH : AL_MEDIUM;

	//mark the target no matter if the attack succeeds
	kTarget.SetLastAggLvl(aggLevel);

	// Make attacks
	CvUnit* pDefender = pTargetPlot->getVisibleEnemyDefender(m_pPlayer->GetID());
	if (pDefender && FindUnitsWithinStrikingDistance(pTargetPlot) && ComputeTotalExpectedDamage(kTarget)>0)
		return ExecuteAttackWithUnits(pTargetPlot, aggLevel);

	return false;
}

/// Move units out of current dominance zone
void CvTacticalAI::ExecuteWithdrawMoves()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(!pUnit)
			continue;
		
		// Allow withdraw to neighboring tactical zone which seems safe
		CvTacticalDominanceZone* pZone = GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot());
		if (!pZone)
			continue;

		//todo: if we withdraw one unit, make sure we withdraw any neighboring units as well .. don't want to leave anyone behind!
		int iBestScore = 0;
		CvPlot* pTargetPlot = NULL;
		for (std::vector<int>::const_iterator it = pZone->GetNeighboringZones().begin(); it != pZone->GetNeighboringZones().end(); ++it)
		{
			CvTacticalDominanceZone* pNextZone = GetTacticalAnalysisMap()->GetZoneByID(*it);
			if (pNextZone && pNextZone->GetZoneCity() && pNextZone->IsWater() == (pUnit->getDomainType() == DOMAIN_SEA))
			{
				int iScore = pNextZone->getHospitalityScore();
				int iTurns = pUnit->TurnsToReachTarget(pTargetPlot, CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER, 12);
				if (iTurns == INT_MAX)
					continue;

				iScore = (iScore * 100) / (iTurns + 1);

				if (iScore > iBestScore && pUnit->CanSafelyReachInXTurns(pTargetPlot, 12))
				{
					pTargetPlot = GC.getMap().plot(pNextZone->GetCenterX(), pNextZone->GetCenterY());
					iBestScore = iScore;
				}
			}
		}

		if (!pTargetPlot)
		{
			// Compute moves to nearest city and use as sort criteria
			CvCity* pNearestCity = m_pPlayer->isMinorCiv() ? m_pPlayer->getCapitalCity() : m_pPlayer->GetClosestCityByPathLength(pUnit->plot());
			if (pNearestCity)
				pTargetPlot = pNearestCity->plot();
		}

		if (pTargetPlot)
		{
			MoveToEmptySpaceNearTarget(pUnit, pTargetPlot, pUnit->getDomainType(), 12, true);
			UnitProcessed(m_CurrentMoveUnits[iI].GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("%s %d withdrew from (%d,%d) towards (%d,%d)", 
					pUnit->getName().GetCString(),pUnit->GetID(),pUnit->getX(),pUnit->getY(),pTargetPlot->getX(),pTargetPlot->getY());
				LogTacticalMessage(strLogString);
			}
		}
		else
		{
			if (pUnit->shouldPillage(pUnit->plot(), true))
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());

			//now move all units which didn't find a path to a city
			ExecuteMovesToSafestPlot(pUnit);
		}
	}

}

/// Move naval units on top of embarked units in danger
void CvTacticalAI::ExecuteEscortEmbarkedMoves(std::vector<CvUnit*> vTargets)
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			CvUnit* pBestTarget = NULL;
			int iHighestDanger = -1;
			int iBestMoveFlag = 0;

			// Loop through all my embarked units that are: alone and within range
			for (size_t i=0; i<vTargets.size(); ++i)
			{
				CvUnit* pTarget = vTargets[i];
				int iMoveFlag = pUnit->CanStackUnitAtPlot(pTarget->plot()) ? CvUnit::MOVEFLAG_IGNORE_DANGER : CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
				
				// Can this unit get to the embarked unit in two moves?
				int iTurns = pUnit->TurnsToReachTarget(pTarget->plot(),iMoveFlag,1);
				if (iTurns <= 1)
				{
					//note: civilian in danger have INT_MAX
					int iDanger = pTarget->GetDanger();
					if (iDanger > iHighestDanger)
					{
						iHighestDanger = iDanger;
						pBestTarget = pTarget;
						iBestMoveFlag = iMoveFlag;
					}
				}
			}

			if (pBestTarget)
			{
				ExecuteMoveToPlot(pUnit, pBestTarget->plot(), true, iBestMoveFlag);

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
	if (pUnit->isBarbarian() && pUnit->plot()->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
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
	bool bIsRanged = pUnit->IsCanAttackRanged();

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

CvUnit* CvTacticalAI::FindUnitForThisMove(AITacticalMove eMove, CvPlot* pTarget, int iNumTurnsAway /* = -1 if any distance okay */)
{
	static UnitCombatTypes eReconType = (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON", true);

	m_CurrentMoveUnits.clear();
	std::vector<OptionWithScore<CvUnit*>> possibleUnits;

	// Loop through all units available to tactical AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); it++)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && pLoopUnit->getDomainType() != DOMAIN_AIR && pLoopUnit->IsCombatUnit() && !pLoopUnit->TurnProcessed())
		{
			// Mod option: only recon units can claim ruins
			// Leaving this code here because A) this option can be turned off (is by default in Community Patch Only), and
			// B) even though most explorers aren't available to tactical AI, some secondary explorer units with the Reconnaissance promotion, like Conquistadors, can make use of it
			// Economic AI still places a high value on goody huts for AI explorers, so they'll still be prioritized; see EconomicAIHelpers::ScoreExplorePlot()
			if (MOD_BALANCE_CORE_GOODY_RECON_ONLY && eMove == AI_TACTICAL_GOODY && pTarget->isGoody())
			{
				if (pLoopUnit->getUnitCombatType() != eReconType && !pLoopUnit->IsGainsXPFromScouting())
					continue;
			}

			//note that garrisons are not recruited into m_CurrentTurnUnits in the first place
			if(!pLoopUnit->canMove() || !pLoopUnit->IsCanAttack() || !pLoopUnit->canMoveInto(*pTarget,CvUnit::MOVEFLAG_DESTINATION))
				continue;

			if(pLoopUnit->AI_getUnitAIType()==UNITAI_EXPLORE || pLoopUnit->getArmyID() != -1)
				continue;

			if (pLoopUnit->IsCoveringFriendlyCivilian())
				continue;

			//performance optimization ... careful because zero is a valid turn value
			if(iNumTurnsAway>1 && plotDistance(*pLoopUnit->plot(),*pTarget)>5*iNumTurnsAway)
				continue;

			int iExtraScore = 0;
			if(eMove == AI_TACTICAL_GARRISON)
			{
				// Do not pull units out of important citadels
				CvPlot* pUnitPlot = pLoopUnit->plot();
				if (TacticalAIHelpers::IsPlayerCitadel(pUnitPlot, m_pPlayer->GetID()) && pUnitPlot->IsBorderLand(m_pPlayer->GetID()))
					continue;

				// Want to put ranged units in cities to give them a ranged attack (but siege units should be used for offense)
				switch (pLoopUnit->AI_getUnitAIType())
				{
				case UNITAI_RANGED:
					iExtraScore += 30 + pTarget->getPlotCity()->getGarrisonRangedAttackModifier();
					break;
				case UNITAI_DEFENSE_AIR:
				case UNITAI_DEFENSE:
					iExtraScore += 20;
					break;
				default:
					//nothing
					break;
				}

				if (pLoopUnit->canFortify(pTarget))
					iExtraScore += 17;

				//naval garrisons cannot attack inside cities and don't increase city strength...
				if (!pLoopUnit->isNativeDomain(pTarget))
					continue;

				// Don't put units with a defense boosted from promotions in cities, these boosts are ignored
				iExtraScore -= pLoopUnit->getDefenseModifier();
			}
			else if (eMove == AI_TACTICAL_GUARD)
			{
				// Heal first, guards might be attacked
				if (pLoopUnit->shouldHeal(false))
					continue;

				// No siege units as plot defenders
				if (pLoopUnit->AI_getUnitAIType()==UNITAI_CITY_BOMBARD)
					continue;

				// Ranged units are ok only in citadels
				if (!TacticalAIHelpers::IsPlayerCitadel(pTarget, m_pPlayer->GetID()))
				{
					if (pLoopUnit->IsCanAttackRanged())
						continue;
					if (pLoopUnit->getExtraVisibilityRange() > 0)
						iExtraScore += 23;
				}

				//these can do in a pinch
				if (pLoopUnit->noDefensiveBonus() || !pLoopUnit->canFortify(pTarget))
					iExtraScore -= 21;

				// Units with defensive promotions are especially valuable
				if(pLoopUnit->getDefenseModifier() > 0 || pLoopUnit->getExtraRangedDefenseModifier() > 0)
					iExtraScore += 31;
			}
			else if(eMove == AI_TACTICAL_GOODY)
			{
				// Fast movers are top priority
				if (pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK) || pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_SKIRMISHER))
					iExtraScore += 31;
			}

			//otherwise collect and sort
			int iTurns = pLoopUnit->TurnsToReachTarget(pTarget, CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER, (iNumTurnsAway == -1 ? MAX_INT : iNumTurnsAway));
			if(iTurns != MAX_INT)
			{
				//tricky to make a good score avoiding ties ...
				int iScore = 1000 + iExtraScore - 20 * iTurns - plotDistance(*pTarget,*pLoopUnit->plot());
				possibleUnits.push_back( OptionWithScore<CvUnit*>(pLoopUnit, iScore));
			}
		}
	}

	if (possibleUnits.empty())
		return NULL;
	else
	{
		std::stable_sort(possibleUnits.begin(), possibleUnits.end());
		CheckDebugTrigger(possibleUnits.front().option->GetID());
		return possibleUnits.front().option;
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

		// Don't pull barbarian units out of camps to attack.
		if(pLoopUnit->isBarbarian() && (pLoopUnit->plot()->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT)))
			continue;

		// Some units can't enter cities
		if (pLoopUnit->isNoCapture() && bIsCityTarget)
			continue;

		// Don't bother with pathfinding if we're very far away
		if (plotDistance(*pLoopUnit->plot(),*pTarget) > pLoopUnit->baseMoves(false)*4)
			continue;

		//if it's a fighter plane, don't use it here, we need it for interceptions / sweeps
		if (pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_DEFENSE_AIR)
			continue;

		if (pLoopUnit->IsCoveringFriendlyCivilian())
			continue;

		//note that garrisoned units are *not* recruited into m_CurrentTurnUnits!
		//also note that units in citadels are recruited, but the combat sim knows about their importance

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
					CvUnit::MOVEFLAG_IGNORE_STACKING_SELF | CvUnit::MOVEFLAG_NO_EMBARK, 0);

				//start from the outside
				for (int i=pLoopUnit->GetRange(); i>0 && !bCanReach; i--)
				{
					std::vector<CvPlot*> vPlots = TacticalAIHelpers::GetPlotsForRangedAttack(pTarget,pLoopUnit,i,false);

					for (std::vector<CvPlot*>::iterator it=vPlots.begin(); it!=vPlots.end() && !bCanReach; ++it)
						bCanReach = (reachablePlots.find( (*it)->GetPlotIndex() ) != reachablePlots.end());
				}
			}
		}
		else //melee. enough if we can get adjacent to the target
		{
			int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF;
			bCanReach = (pLoopUnit->TurnsToReachTarget(pTarget, iFlags, 0) <= 0);
		}

		//include units which are very close even if they cannot do anything right now
		//but the combat sim should have control over them to move them out of the way
		if (!bCanReach && plotDistance(*pLoopUnit->plot(),*pTarget)>2)
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
		int iAttackStrength = 0;

		if (pLoopCity->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()) && !pLoopCity->isMadeAttack())
			iAttackStrength += pLoopCity->getStrengthValue(true);

		//garrison may have a larger range than the city ...
		if (pLoopCity->HasGarrison())
		{
			CvUnit* pGarrison = pLoopCity->GetGarrisonedUnit();
			if (pGarrison->canRangeStrikeAt(pTargetPlot->getX(), pTargetPlot->getY()))
				iAttackStrength += pGarrison->GetBaseRangedCombatStrength();
		}

		if (iAttackStrength>0)
		{
			CvTacticalCity city;
			city.SetID(pLoopCity->GetID());
			city.SetExpectedTargetDamage(iAttackStrength);
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
bool CvTacticalAI::FindUnitsForHarassing(CvPlot* pTarget, int iNumTurnsAway, int iMinHitpoints, int iMaxHitpoints, DomainTypes eDomain, bool bMustHaveMovesLeft, bool bAllowEmbarkation, int iMaxNumUnits)
{
	m_CurrentMoveUnits.clear();
	//need to convert turns to max path length here, zero turns away is also valid!
	SPathFinderUserData data(m_pPlayer->GetID(), PT_ARMY_MIXED, NO_PLAYER, (iNumTurnsAway+1)*3);
	ReachablePlots relevantPlots = GC.GetStepFinder().GetPlotsInReach(pTarget, data);

	//plots are ordered by turns to reach!
	for (ReachablePlots::iterator it = relevantPlots.begin(); it != relevantPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
		CvUnit* pLoopUnit = pPlot->getBestDefender(m_pPlayer->GetID());
		if (pLoopUnit)
		{
			//these units are too fragile for the moves we have in mind
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_CITY_BOMBARD || pLoopUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA)
				continue;

			if (!pLoopUnit->canUseForTacticalAI())
				continue;

			if (pLoopUnit->IsCoveringFriendlyCivilian())
				continue;

			if (pLoopUnit->isBarbarian() && pLoopUnit->plot()->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
				continue;

			if (eDomain != NO_DOMAIN && pLoopUnit->getDomainType() != eDomain)
				continue;

			if(iMinHitpoints>0 && pLoopUnit->GetCurrHitPoints()<iMinHitpoints)
				continue;

			if(iMaxHitpoints>0 && pLoopUnit->GetCurrHitPoints()>iMaxHitpoints)
				continue;

			if (pLoopUnit->GetDanger(pTarget) > pLoopUnit->GetCurrHitPoints())
				continue;

			//don't use garrisons if there is an enemy around. the garrison may still attack when we do garrison moves!
			if (pLoopUnit->IsGarrisoned() && pLoopUnit->GetGarrisonedCity()->NeedsGarrison())
				continue;

			//this should be a low-risk thing so don't get our units killed
			int iFlags = CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER;
			if (bAllowEmbarkation)
				iFlags |= CvUnit::MOVEFLAG_NO_EMBARK;
			if (pTarget->isEnemyUnit(m_pPlayer->GetID(), true, true) && !pLoopUnit->IsCanAttackWithMove())
				iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
			if (bMustHaveMovesLeft)
				iFlags |= CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN;

			int iTurnsCalculated = pLoopUnit->TurnsToReachTarget(pTarget, iFlags, iNumTurnsAway);
			if (iTurnsCalculated <= iNumTurnsAway)
			{
				CvTacticalUnit unit(pLoopUnit->GetID());
				unit.SetAttackStrength(1 + iNumTurnsAway - iTurnsCalculated);
				m_CurrentMoveUnits.push_back(unit);
				//pathfinding is expensive, don't return more than needed
				if (m_CurrentMoveUnits.size() >= (size_t)iMaxNumUnits)
					break;
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
int CvTacticalAI::ComputeTotalExpectedDamage(const CvTacticalTarget& kTarget)
{
	CvPlot* pTargetPlot = GC.getMap().plot(kTarget.GetTargetX(), kTarget.GetTargetY());

	int rtnValue = 0;
	int iMeleeCount = 0;

	// Loop through all units who can reach the target
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pAttacker = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());

		// Is target a unit?
		switch(kTarget.GetTargetType())
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
				if (iDamage > iSelfDamage/5 && pAttacker->GetCurrHitPoints() > iSelfDamage/2) //exclude only the most extreme suicides, we will sort out the details during combat sim
				{
					m_CurrentMoveUnits[iI].SetExpectedTargetDamage(iDamage);
					m_CurrentMoveUnits[iI].SetExpectedSelfDamage(iSelfDamage);

					//if we have a lot of melee units, don't assume they all can be executed at once
					if (!pAttacker->IsCanAttackRanged() && !pAttacker->canMoveAfterAttacking())
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
		default:
		UNREACHABLE(); // Other target types cannot be damaged.
		}
	}

	//sort by expected damage to target!
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end(), TacticalAIHelpers::SortByExpectedTargetDamageDescending);

	return rtnValue;
}

/// Estimates the bombard damage we can apply to a target
int CvTacticalAI::ComputeTotalExpectedCityBombardDamage(CvUnit* pTarget)
{
	int iExpectedDamage = 0;

	// Now loop through all the cities that can bombard it
	for(unsigned int iI = 0; iI < m_CurrentMoveCities.size(); iI++)
	{
		CvCity* pAttackingCity = m_pPlayer->getCity(m_CurrentMoveCities[iI].GetID());
		
		iExpectedDamage += pAttackingCity->rangeCombatDamage(pTarget);

		if (pAttackingCity->HasGarrison())
		{
			CvUnit* pGarrison = pAttackingCity->GetGarrisonedUnit();
			if (pGarrison->canRangeStrikeAt(pTarget->getX(), pTarget->getY()))
			{
				int iUnitDamage = pGarrison->GetRangeCombatDamage(pTarget, NULL, false, 0, NULL, NULL, true, true);
				//assume same damage for multiple attacks ...
				iExpectedDamage += iUnitDamage * pGarrison->getNumAttacks();
			}
		}

	}
	
	return iExpectedDamage;
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
	int iMaxTurns = m_iLandBarbarianRange;
	
	// combat units look at all offensive targets within x turns
	if (pUnit->IsCanDefend())
	{
		pBestMovePlot = FindNearbyTarget(pUnit, iMaxTurns, true);

		// alternatively explore
		if (pBestMovePlot == NULL)
			pBestMovePlot = FindBarbarianExploreTarget(pUnit);
	}

	// by default go back to camp or so
	if (pBestMovePlot == NULL)
	{
		if (!pUnit->IsCanDefend())
			iMaxTurns = 23;

		pBestMovePlot = FindNearbyTarget(pUnit, iMaxTurns, false);
	}

	return pBestMovePlot;
}

/// Find a multi-turn target for a sea barbarian to wander towards
CvPlot* CvTacticalAI::FindBestBarbarianSeaTarget(CvUnit* pUnit)
{
	CvPlot* pBestMovePlot = NULL;
	int iBestValue = MAX_INT;

	SPathFinderUserData data(pUnit, 0, m_iSeaBarbarianRange);
	ReachablePlots movePlots = GC.GetPathFinder().GetPlotsInReach(pUnit->plot(), data);

	// Loop through all unit targets to find the closest
	for (CvTacticalTarget* pTarget = GetFirstUnitTarget(); pTarget!=NULL; pTarget = GetNextUnitTarget())
	{
		CvPlot* pPlot = GC.getMap().plot(pTarget->GetTargetX(), pTarget->GetTargetY());

		ReachablePlots::const_iterator itPlot = movePlots.find(pPlot->GetPlotIndex());
		if (itPlot != movePlots.end() && itPlot->iPathLength < iBestValue)
		{
			iBestValue = itPlot->iPathLength;
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
						int iValue = it->iPathLength;
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

		//ignore cities
		if (!pConsiderPlot->isRevealed(pUnit->getTeam()) || pConsiderPlot->isCity())
			continue;

		//even barbarians consider danger sometimes
		if (pUnit->GetDanger(pConsiderPlot) > pUnit->GetCurrHitPoints())
			continue;

		int iValue = 0;
		for (int i = 0; i < RING1_PLOTS; i++)
		{
			CvPlot* pNeighbor = iterateRingPlots(pConsiderPlot, i);
			if (!pNeighbor)
				continue;

			if (!pNeighbor->isRevealed(pUnit->getTeam()))
				iValue += 3;
			else if (!pNeighbor->isVisible(pUnit->getTeam()) && pNeighbor->isOwned())
				iValue += 2;
		}

		// disembark if possible
		if (pUnit->isEmbarked() && pUnit->isNativeDomain(pConsiderPlot))
			iValue += 100;

		if (iValue > iBestValue)
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
		if (pUnitPlot->getPlotCity()->isInDangerOfFalling(true))
			return true;

		if (pUnit->shouldHeal(true) && m_pPlayer->GetPlotDanger(pUnitPlot->getPlotCity())>0)
			return true;
	}
	else
	{
		CvUnit *pCarrier = pUnit->getTransportUnit();
		if (pCarrier && pCarrier->isProjectedToDieNextTurn())
			return true;

		if (pUnit->shouldHeal(true) && pCarrier->GetDanger(pUnitPlot)>0)
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
		default:
			UNREACHABLE(); // Unit type cannot rebase.
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
			if (!pPlot->isWater() && pUnit->IsCanAttackRanged() && pUnit->getDomainType() == DOMAIN_SEA)
			{
				pPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pPlot);
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
	std::stable_sort(candidates.begin(), candidates.end());
	std::reverse(candidates.begin(), candidates.end());

	for (size_t i=0; i<candidates.size(); i++)
	{
		CvPlot* pPlot = candidates[i].option;
		if ( pUnit->TurnsToReachTarget(pPlot,CvUnit::MOVEFLAG_APPROX_TARGET_RING1|CvUnit::MOVEFLAG_IGNORE_STACKING_SELF|CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER,iMaxTurns) < INT_MAX )
			return pPlot;
	}

	return NULL;
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

	//try and upgrade units even if tactical AI is using them
	if (pUnit->CanUpgradeRightNow(false) && !pUnit->IsHurt())
	{
		CvUnit* pNewUnit = pUnit->DoUpgrade();
		if (pNewUnit)
			pNewUnit->SetTurnProcessed(true);
	}
}

/// Is this civilian target of the highest priority?
bool CvTacticalAI::IsVeryHighPriorityCivilianTarget(CvTacticalTarget* pTarget)
{
	CvUnit* pUnit = pTarget->GetUnitPtr();
	return pUnit->IsCombatSupportUnit();
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

FILogFile* CvTacticalAI::GetLogFile()
{
	return LOGFILEMGR.GetLog(GetLogFileName(m_pPlayer->getCivilizationShortDescription()), FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite);
}

/// Log current status of the operation
void CvTacticalAI::LogTacticalMessage(const CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;

		CvString strPlayerName(m_pPlayer->getCivilizationShortDescription());
		strPlayerName.Replace(' ', '_'); //no spaces!

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		strOutBuf = strBaseString + strMsg;

		FILogFile* pLog = GetLogFile();
		if (pLog)
			pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvTacticalAI::GetLogFileName(const CvString& playerName) const
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
	//no visibility check, when we call this we already know there is an enemy unit ...
	CvUnit* pTargetUnit = pTargetPlot->getBestDefender( NO_PLAYER, pUnit->getOwner(), pUnit, false, true);

	int iDamageDealt = 0;
	int iDamageReceived = 1;
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

			//we have just a single attacker, avoid enemy clusters
			if (pTestPlot->GetNumEnemyUnitsAdjacent(pUnit->getTeam(), NO_DOMAIN) > 0)
				continue;

			//there might be stacking issues so do another pathfinder check
			if (pUnit->TurnsToReachTarget(pTestPlot, CvUnit::MOVEFLAG_ATTACK | CvUnit::MOVEFLAG_NO_EMBARK, 1) > 0)
				continue;

			if (iDamageDealt >= pEnemy->GetCurrHitPoints())
			{
				if (iDamageReceived < pUnit->GetCurrHitPoints())
					iDamageDealt += 30; //bonus for a kill, but no suicide
				iDamageReceived = max(0, iDamageReceived - pUnit->getHPHealedIfDefeatEnemy());
			}

			//if the attacker is (almost) unhurt, we can be a bit more aggressive and assume we'll heal up next turn
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

	std::stable_sort(meleeTargets.begin(), meleeTargets.end());

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
	if (pUnit->canMove() && !pUnit->atPlot(*pOrigin)) //try to move back to the original plot (if we advanced)
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
	if (bIsAirUnit || pUnit->IsGarrisoned())
		bAllowMovement = false;

	if (bAllowMovement)
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Performing ranged opportunity attack with %s at (%d:%d)", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
			GET_PLAYER(pUnit->getOwner()).GetTacticalAI()->LogTacticalMessage(strMsg);
		}

		//no loop needed, there is only one unit anyway
		vector<STacticalAssignment> vAssignments = TacticalAIHelpers::FindBestUnitAssignments(vector<CvUnit*>(1, pUnit), pUnit->plot(), AL_LOW, gTactPosStorage);
		return TacticalAIHelpers::ExecuteUnitAssignments(pUnit->getOwner(), vAssignments);
	}
	else
	{
		int iMaxDamage = 0;
		CvPlot* pBestTarget = NULL;

		int iRange = max(1,min(5,pUnit->GetRange()));
		for (int i=RING0_PLOTS; i<RING_PLOTS[iRange]; i++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pBasePlot, i);
			if (!pLoopPlot || pLoopPlot->isCity())
				continue;

			if (!pUnit->canRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
				continue;

			//don't blindly attack the first one we find, check how much damage we can do
			CvUnit* pOtherUnit = pLoopPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(), pUnit, true /*testWar*/);
			if (pOtherUnit && !pOtherUnit->isDelayedDeath())
			{
				int iDamage = bIsAirUnit ? pUnit->GetAirCombatDamage(pOtherUnit, NULL, false) : 
											pUnit->GetRangeCombatDamage(pOtherUnit, NULL, false, 0) +  pUnit->GetRangeCombatSplashDamage(pOtherUnit->plot());

				//kill bonus
				if (iDamage >= pOtherUnit->GetCurrHitPoints())
					iDamage += 30;

				if (iDamage > iMaxDamage)
				{
					pBestTarget = pLoopPlot;
					iMaxDamage = iDamage;
				}
			}
		}

		if (!pBestTarget)
			return false;

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Performing ranged opportunity attack with %s at (%d:%d) on (%d:%d)",
				pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY(), pBestTarget->getX(), pBestTarget->getY());
			GET_PLAYER(pUnit->getOwner()).GetTacticalAI()->LogTacticalMessage(strMsg);
		}

		pUnit->PushMission(bIsAirUnit ? CvTypes::getMISSION_MOVE_TO() : CvTypes::getMISSION_RANGE_ATTACK(), pBestTarget->getX(), pBestTarget->getY());
		return true;
	}
}

CvPlot* TacticalAIHelpers::FindSafestPlotInReach(const CvUnit* pUnit, bool bAllowEmbark, bool bConsiderPush)
{
	//use rebase moves for aircraft!
	if (!pUnit || pUnit->getDomainType() == DOMAIN_AIR)
		return NULL;

	vector<OptionWithScore<CvPlot*>> aCityList;
	vector<OptionWithScore<CvPlot*>> aZeroDangerList;
	vector<OptionWithScore<CvPlot*>> aCoverList;
	vector<OptionWithScore<CvPlot*>> aDangerList;

	//special behavior for cities and citadels - don't run even if there is a "safe" plot somewhere else
	CvPlot* pCurrentPlot = pUnit->plot();
	bool bIsInCityOrCitadelNow = (pCurrentPlot->isFriendlyCity(*pUnit) && !pCurrentPlot->getPlotCity()->isInDangerOfFalling()) || 
								(pUnit->IsCombatUnit() && TacticalAIHelpers::IsPlayerCitadel(pCurrentPlot, pUnit->getOwner()));
	if (bIsInCityOrCitadelNow && !pUnit->isProjectedToDieNextTurn() && pUnit->canEndTurnAtPlot(pCurrentPlot))
		if (pUnit->AI_getUnitAIType()!=UNITAI_CITY_BOMBARD || pUnit->GetDanger(pCurrentPlot)<pUnit->GetCurrHitPoints())
			return pCurrentPlot;

	//for current plot
	int iCurrentHealRate = pUnit->healRate(pCurrentPlot);

	//don't run if we are needed
	if (pUnit->IsCoveringFriendlyCivilian() && pUnit->GetDanger(pCurrentPlot)<pUnit->GetCurrHitPoints()*2)
		return pCurrentPlot;

	//embarkation allowed for now, we sort it out below
	ReachablePlots eligiblePlots = pUnit->GetAllPlotsInReachThisTurn(); 
	for (ReachablePlots::iterator it = eligiblePlots.begin(); it != eligiblePlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		// don't attack though
		if (pPlot->getNumVisibleEnemyDefenders(pUnit) > 0 || pPlot->isEnemyCity(*pUnit))
			continue;

		int iFlags = CvUnit::MOVEFLAG_DESTINATION;
		// allow capturing civilians!
		if (pPlot->isVisibleEnemyUnit(pUnit))
			iFlags |= CvUnit::MOVEFLAG_ATTACK;

		//if we cannot move in because one of our own units is blocking us
		if (!pUnit->canMoveInto(*pPlot, iFlags) && pUnit->canMoveInto(*pPlot, iFlags | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF))
		{
			if (!bConsiderPush || !pUnit->CanPushOutUnitHere(*pPlot))
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
		int iCityDistance = kPlayer.GetCityDistancePathLength(pPlot);

		bool bIsZeroDanger = (iDanger <= 0);
		bool bIsInTerritory = (pPlot->getTeam() == kPlayer.getTeam());
		// citadels have low danger but not zero. so we need to make sure we're not abandoning them too easily
		bool bIsInCityOrCitadel = (pPlot->isFriendlyCity(*pUnit) && !pPlot->getPlotCity()->isInDangerOfFalling()) ||
			(pUnit->IsCombatUnit() && TacticalAIHelpers::IsPlayerCitadel(pPlot, pUnit->getOwner()));

		//civilians and embarked units want cover
		bool bIsInCover = false;
		if (pUnit->IsCivilianUnit() || !pUnit->isNativeDomain(pPlot))
		{
			CvUnit* pDefender = pPlot->getBestDefender(pUnit->getOwner());
			if (pDefender && pDefender != pUnit && !pDefender->isProjectedToDieNextTurn() && pDefender->GetDanger()<pDefender->GetCurrHitPoints())
			{
				bIsInCover = true;
				//otherwise we will get only INT_MAX for civilians
				iDanger = pDefender->GetDanger(pPlot);
			}
		}

		bool bWrongDomain = pPlot->needsEmbarkation(pUnit);
		bool bWouldEmbark = bWrongDomain && !pUnit->isEmbarked();

		//avoid overflow further down and useful handling for civilians
		if (iDanger == INT_MAX)
			iDanger = 10000;

		//map 144 to 144, everything above is not so important
		int iScore = (iDanger > 144) ? 12 * sqrti(iDanger) : iDanger;

		//we can't heal after moving and lose fortification bonus, so the current plot gets a bonus (respectively all others a penalty)
		if (pPlot != pUnit->plot() && iCurrentHealRate > 0)
			iScore += iCurrentHealRate - 3; //everything else equal it looks stupid to stand around while being shot at

		//heal rate is higher here and danger lower
		if (!bIsInTerritory)
			iScore += 12;

		//try to hide - if there are few enemy units, this might be a tiebreaker
		if (!pPlot->IsKnownVisibleToEnemy(pUnit->getOwner()))
			iScore -= iScore / 4;

		//try to go avoid borders
		if (pPlot->IsAdjacentOwnedByEnemy(pUnit->getTeam()))
			iScore += iScore / 20;

		//don't stay here, try to get away even if it means temporarily moving to a higher danger plot
		if (pPlot->IsEnemyCityAdjacent(pUnit->getTeam(), NULL))
			iScore += iScore / 10;

		//naval units should avoid enemy coast, never know what's hiding there
		if (pUnit->getDomainType() == DOMAIN_SEA)
			iScore += pPlot->countMatchingAdjacentPlots(DOMAIN_LAND, NO_PLAYER, pUnit->getOwner(), NO_PLAYER) * 7;

		if (!bIsInCityOrCitadel)
		{
			//try to go where our friends are
			int iFriendlyUnitsAdjacent = pPlot->GetNumFriendlyUnitsAdjacent(pUnit->getTeam(), NO_DOMAIN, true, pUnit);
			//don't go where our foes are
			int iEnemyUnitsAdjacent = pPlot->GetNumEnemyUnitsAdjacent(pUnit->getTeam(), pUnit->getDomainType());
			iScore += (iEnemyUnitsAdjacent - iFriendlyUnitsAdjacent) * 13;

			//use city distance as tiebreaker
			iScore = iScore * 10 + iCityDistance;
		}

		//discourage water tiles for land units
		//note that zero danger status has already been established, this is only for sorting now
		if (bWrongDomain)
			iScore += 250;

		if(bIsInCityOrCitadel)
		{
			aCityList.push_back( OptionWithScore<CvPlot*>(pPlot,iScore) );
		}
		else if(bIsInCover) //mostly relevant for civilians
		{
			aCoverList.push_back( OptionWithScore<CvPlot*>(pPlot,iScore) );
		}
		else if(bIsZeroDanger)
		{
			//if danger is zero, look at distance to closest owned city instead
			//idea: could also look at number of plots reachable from pPlot to avoid dead ends
			aZeroDangerList.push_back( OptionWithScore<CvPlot*>(pPlot, bIsInTerritory ? iCityDistance : iCityDistance*2) );
		}
		else if(!bWouldEmbark || bAllowEmbark)
		{
			aDangerList.push_back( OptionWithScore<CvPlot*>(pPlot,iScore) );
		}
	}

	//high scores are bad, we sort descending
	std::stable_sort(aCityList.begin(), aCityList.end());
	std::stable_sort(aCoverList.begin(), aCoverList.end());
	std::stable_sort(aZeroDangerList.begin(), aZeroDangerList.end());
	std::stable_sort(aDangerList.begin(), aDangerList.end());

	// Now that we've gathered up our lists of destinations, pick the most promising one
	if (aCityList.size()>0)
		return aCityList.back().option;
	else if (aCoverList.size() > 0)
	{
		CvPlot* pPlot = aCoverList.back().option;
		CvUnit* pDefender = pPlot->getBestDefender(pUnit->getOwner());
		if (pDefender && pDefender != pUnit)
		{
			//taking cover only works if the defender will not move away!
			//since we move civilians only after the combat units have moved it should be safe to pin the defender here (AI players only!)
			if (!pDefender->TurnProcessed() && !pDefender->isHuman())
			{
				TacticalAIHelpers::PerformRangedOpportunityAttack(pDefender, false);
				pDefender->PushMission(CvTypes::getMISSION_SKIP());
				pDefender->SetTurnProcessed(true);
			}
		}
		return pPlot;
	}
	else if (aZeroDangerList.size()>0)
		return aZeroDangerList.back().option;
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

	int iCityDistance = pPlayer->GetCityDistancePathLength(pCandidate);
	if (iCityDistance>5)
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

bool TacticalAIHelpers::IsCloseToContestedBorder(CvPlayer* pPlayer, CvPlot* pPlot)
{
	bool bResult = false;

	for (int i = RING1_PLOTS; i < RING2_PLOTS; i++)
	{
		CvPlot* pTestPlot = iterateRingPlots(pPlot, i);
		if (pTestPlot && pPlayer->IsAtWarWith(pTestPlot->getOwner()) && pTestPlot->getDomain() == DOMAIN_LAND)
		{
			CvTacticalDominanceZone* pZone = pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pTestPlot);
			if (pZone && pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				continue;

			bResult = true;
			break;
		}
	}

	return bResult;
}

CvPlot* TacticalAIHelpers::FindClosestSafePlotForHealing(CvUnit* pUnit, bool bConservative)
{
	if (!pUnit)
		return NULL;

	//first see if the current plot is good
	if (pUnit->GetDanger() == 0 && pUnit->canHeal(pUnit->plot(), true))
		return pUnit->plot();

	std::vector<SPlotWithScore> vCandidates;
	ReachablePlots eligiblePlots = pUnit->GetAllPlotsInReachThisTurn(); //embarkation allowed for now, we sort it out below
	for (ReachablePlots::iterator it = eligiblePlots.begin(); it != eligiblePlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		if (pPlot->isEnemyUnit(pUnit->getOwner(), true, true))
			continue;

		bool bPillage = (it->iMovesLeft > 0) && pUnit->shouldPillage(pPlot, false);
		//don't check movement, don't need to heal right now
		if (pUnit->getDomainType() == DOMAIN_LAND)
		{
			if (!pUnit->canHeal(pPlot, true))
				continue;

			//don't mess with (ranged) garrisons if enemies are around
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity && pCity->HasGarrison() && pCity->isUnderSiege() && pCity->GetGarrisonedUnit()->IsCanAttackRanged())
				continue;
		}
		else
		{
			//naval units usually must pillage to heal ...
			if (!bPillage && !pUnit->canHeal(pPlot, true))
				continue;
		}

		//can we stay there?
		if (!pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
		{
			if (pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF))
			{
				//todo: we should maybe choose the target plot based on whether we can make a good swap?
				if (!pUnit->CanPushOutUnitHere(*pPlot))
					continue;
			}
			else
				continue;
		}

		int iDanger = pUnit->GetDanger(pPlot);
		int iHealRate = pUnit->healRate(pPlot);
		int nFriends = pPlot->GetNumFriendlyUnitsAdjacent(pUnit->getTeam(), NO_DOMAIN, true, pUnit);

		//sometimes we want to ignore pillage health, it's a one-time effect and may lead into dead ends
		if (!bConservative && bPillage)
			iHealRate += /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT);

		//the method is called findSafePlot; this should be safe enough
		if (iDanger < iHealRate * nFriends)
		{
			int iScore = iHealRate * 2 + nFriends * 3 - iDanger;
			//tiebreaker
			iScore -= GET_PLAYER(pUnit->getOwner()).GetCityDistancePathLength(pPlot);

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

std::vector<CvPlot*> TacticalAIHelpers::GetPlotsForRangedAttack(const CvPlot* pTarget, const CvUnit* pUnit, int iRange, bool bCheckOccupied)
{
	std::vector<CvPlot*> vPlots;

	if (!pTarget || !pUnit)
		return vPlots;

	// Aircraft and special promotions make us ignore LOS
	bool bIgnoreLOS = pUnit->IsRangeAttackIgnoreLOS() || pUnit->getDomainType()==DOMAIN_AIR;
	// Can only bombard in domain? (used for Subs' torpedo attack)
	bool bOnlyInDomain = pUnit->getUnitInfo().IsRangeAttackOnlyInDomain();

	const vector<CvPlot*>& vCandidates = GC.getMap().GetPlotsAtRangeX(pTarget, iRange, false, !bIgnoreLOS);

	//filter and take only the half closer to origin
	CvPlot* pRefPlot = pUnit->plot();
	if(pRefPlot == NULL)
		return vPlots;

	int iRefDist = plotDistance(*pRefPlot,*pTarget);
	std::vector<SPlotWithScore> vIntermediate;
	for (size_t i=0; i<vCandidates.size(); i++)
	{
		if(vCandidates[i] == NULL)
			continue;

		int iDistance = plotDistance(*pRefPlot,*(vCandidates[i]));
		if (iDistance>iRefDist && iRefDist>iRange)
			continue;

		if (!vCandidates[i]->isRevealed(pUnit->getTeam()))
			continue;

		//this concerns not only embarked units but also ships in harbor!
		if (!pUnit->isNativeDomain(vCandidates[i]))
			continue;

		if (bCheckOccupied && vCandidates[i]!=pRefPlot && vCandidates[i]->getBestDefender(NO_PLAYER))
			continue;

		if (bOnlyInDomain)
		{
			//subs can only attack within their (water) area or adjacent cities
			if (pRefPlot->getArea() != vCandidates[i]->getArea())
			{
				CvCity *pCity = vCandidates[i]->getPlotCity();
				if (!pCity || !pCity->HasAccessToArea(pRefPlot->getArea()))
					continue;
			}
		}

		vIntermediate.push_back( SPlotWithScore(vCandidates[i],iDistance) );
	}

	//sort by increasing distance
	std::stable_sort(vIntermediate.begin(), vIntermediate.end());

	for (size_t i=0; i<vIntermediate.size(); i++)
		vPlots.push_back(vIntermediate[i].pPlot);

	return vPlots;
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
			// ignore interception for quick and dirty mode ...
			CvUnit* pInterceptor = bQuickAndDirty ? NULL : pDefenderPlot->GetBestInterceptor(pAttacker->getOwner(), pAttacker, false, true);
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

		int iAttackerStrength = pAttacker->GetMaxAttackStrength(pAttackerPlot, pDefenderPlot, pDefender, bIgnoreUnitAdjacencyBoni, bQuickAndDirty, iExtraDefenderDamage);
		//do not override defender flanking/general bonus (it is known during combat simulation)
		int iDefenderStrength = pDefender->GetMaxDefenseStrength(pDefenderPlot, pAttacker, pAttackerPlot, false, bQuickAndDirty, iDamage);

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

bool TacticalAIHelpers::KillLoneEnemyIfPossible(CvUnit* pOurUnit, CvUnit* pEnemyUnit)
{
	if (!pOurUnit || !pEnemyUnit || pEnemyUnit->isDelayedDeath())
		return false;

	//aircraft are different
	if (pOurUnit->getDomainType()==DOMAIN_AIR || pEnemyUnit->getDomainType()==DOMAIN_AIR)
		return false;

	//see how the attack would go
	int iDamageDealt = 0;
	int iDamageReceived = 0;
	iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pEnemyUnit, pOurUnit, pEnemyUnit->plot(), pOurUnit->plot(), iDamageReceived);

	//is it worth it? (take into account some randomness ...)
	if ( iDamageDealt-3 > pEnemyUnit->GetCurrHitPoints() && pOurUnit->GetCurrHitPoints()-iDamageReceived > 23 )
	{
		if (pOurUnit->IsCanAttackRanged())
		{
			//can we attack directly
			if (pOurUnit->canRangeStrikeAt(pEnemyUnit->getX(),pEnemyUnit->getY()))
			{
				pOurUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(),pEnemyUnit->getX(),pEnemyUnit->getY());
				return true;
			}
			else if (pOurUnit->canRangeStrike())
			{
				//need to move and shoot
				bool bIgnoreLOS = pOurUnit->IsRangeAttackIgnoreLOS();
				const vector<CvPlot*>& vAttackPlots = GC.getMap().GetPlotsAtRangeX(pEnemyUnit->plot(), pOurUnit->GetRange(), false, !bIgnoreLOS);
				for (std::vector<CvPlot*>::const_iterator it = vAttackPlots.begin(); it != vAttackPlots.end(); ++it)
				{
					if (pOurUnit->TurnsToReachTarget(*it, CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN, 1) == 0 && pOurUnit->canEverRangeStrikeAt(pEnemyUnit->getX(), pEnemyUnit->getY(), *it, false))
					{
						pOurUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), (*it)->getX(), (*it)->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER);
						//sometimes the unit takes an unexpected path
						if (pOurUnit->atPlot(**it))
							pOurUnit->PushMission(CvTypes::getMISSION_RANGE_ATTACK(), pEnemyUnit->getX(), pEnemyUnit->getY());
						else
							OutputDebugString("pathfinding issue ...\n");
						return true;
					}
				}
			}
		}
		else //melee
		{
			if (pOurUnit->TurnsToReachTarget(pEnemyUnit->plot(),0,1)==0)
			{
				pOurUnit->PushMission(CvTypes::getMISSION_MOVE_TO(),pEnemyUnit->getX(),pEnemyUnit->getY());
				return true;
			}
		}
	}

	return false;
}

bool TacticalAIHelpers::IsSuicideMeleeAttack(const CvUnit * pAttacker, CvPlot * pTarget)
{
	//todo: add special code for air attacks!
	if (!pAttacker || !pTarget || pAttacker->IsCanAttackRanged() || pAttacker->getDomainType()==DOMAIN_AIR)
		return false;

	int iDamageDealt = 0;
	int iDamageReceived = 0;

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

bool TacticalAIHelpers::CanKillTarget(const CvUnit* pAttacker, CvPlot* pTarget)
{
	if (!pAttacker || !pTarget)
		return false;

	CvCity* pTargetCity = pTarget->getPlotCity();
	if (pTargetCity)
	{
		if (!pAttacker->isEnemy(pTargetCity->getTeam()))
			return false;

		//shortcut
		if (pTargetCity->isInDangerOfFalling(true))
			return true;

		//see how an attack would go just in case
		int iDamageDealt = 0;
		int iDamageReceived = 0;
		iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pTargetCity, pAttacker, pAttacker->plot(), iDamageReceived, true, 0, true);

		//ranged units can't capture themselves so we check for an adjacent melee unit
		if (pAttacker->IsCanAttackRanged())
		{
			int iNumMelee = pTarget->GetNumFriendlyUnitsAdjacent(pAttacker->getTeam(), NO_DOMAIN, false);
			if (iNumMelee > 0)
			{
				//assume our melee friends also do some damage
				return iDamageDealt + iNumMelee*iDamageDealt/4 + pTargetCity->getDamage() >= pTargetCity->GetMaxHitPoints();
			}
			else
				return false;
		}
		else
		{
			//some melee units cannot capture either
			if (pAttacker->isNoCapture())
				return false;

			return iDamageDealt + pTargetCity->getDamage() >= pTargetCity->GetMaxHitPoints();
		}
	}

	CvUnit* pDefender = pTarget->getVisibleEnemyDefender(pAttacker->getOwner());
	if (pDefender)
	{
		//see how the attack would go
		int iDamageDealt = 0;
		int iDamageReceived = 0;
		iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pDefender, pAttacker, pDefender->plot(), pAttacker->plot(), iDamageReceived, true, 0, true);
		//no suicide attacks ...
		return iDamageDealt >= pDefender->GetCurrHitPoints() && (iDamageReceived == 0 || iDamageReceived < pAttacker->GetCurrHitPoints()-3);
	}

	return false;
}

vector<pair<CvPlot*, bool>> TacticalAIHelpers::GetTargetsInRange(const CvUnit * pUnit, bool bMustBeAbleToKill, bool bIncludeCivilians)
{
	vector<pair<CvPlot*, bool>> result;
	if (!pUnit)
		return result;

	ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
	for (ReachablePlots::iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		//only melee attacks here - ranged attacks are checked below
		bool bMilitaryTarget = (pPlot->isEnemyCity(*pUnit) && !pUnit->isNoCapture()) || pPlot->isEnemyUnit(pUnit->getOwner(), true, true);
		if (bMilitaryTarget && !pUnit->IsCanAttackRanged())
		{
			bool bCanKill = CanKillTarget(pUnit,pPlot);
			if (bMustBeAbleToKill && !bCanKill)
				continue;

			result.push_back( make_pair(pPlot,bCanKill) );
		}
		else if (bIncludeCivilians && pPlot->isEnemyUnit(pUnit->getOwner(), false, true))
			//we know the civilian is unescorted!
			result.push_back(make_pair(pPlot, true));
		else if (pPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
			//unoccupied barb camp?
			result.push_back(make_pair(pPlot, true));
	}

	if (pUnit->IsCanAttackRanged())
	{
		//for ranged every tile we can enter with movement left is a base for attack
		std::set<int> attackableTiles = TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit,reachablePlots,true,false);
		for (std::set<int>::iterator attackTile=attackableTiles.begin(); attackTile!=attackableTiles.end(); ++attackTile)
		{
			CvPlot* pAttackTile = GC.getMap().plotByIndexUnchecked(*attackTile);
			bool bCanKill = CanKillTarget(pUnit,pAttackTile);

			if (bMustBeAbleToKill && !bCanKill)
				continue;

			result.push_back(make_pair(pAttackTile, bCanKill));
		}
	}

	return result;
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
				CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
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
		const vector<CvPlot*>& vPlots = GC.getMap().GetPlotsAtRangeX(pTestPlot, iRange, true, true);
		for (size_t i = 0; i < vPlots.size(); i++)
			if (vPlots[i] && !vPlots[i]->isVisible(pUnit->getTeam())) //we already know that would have line of sight
				iCount++;
	}
	
	return iCount;
}

//convenience
bool isCombatUnit(eUnitMovementStrategy eMoveType) { return eMoveType == MS_FIRSTLINE || eMoveType == MS_SECONDLINE || eMoveType == MS_THIRDLINE; }
bool isEmbarkedUnit(eUnitMovementStrategy eMoveType) { return eMoveType == MS_EMBARKED; }
bool isSupportUnit(eUnitMovementStrategy eMoveType) { return eMoveType == MS_SUPPORT; }

//Unbelievable bad logic but taken like this from CvUnitCombat
bool AttackEndsTurn(const CvUnit* pUnit, int iNumAttacksLeft)
{
	return !pUnit->canMoveAfterAttacking() && !pUnit->isRangedSupportFire() && iNumAttacksLeft<2;
}

int NumAttacksForUnit(int iMovesLeft, int iMaxAttacks)
{
	return max(0, min( (iMovesLeft+GD_INT_GET(MOVE_DENOMINATOR)-1)/GD_INT_GET(MOVE_DENOMINATOR), iMaxAttacks ));
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

void CAttackCache::clear()
{
	attackStats.clear();
}

void CAttackCache::storeAttack(int iAttackerId, int iAttackerPlot, int iDefenderId, int iPrevDamage, int iDamageDealt, int iDamageTaken)
{
	//known attacker
	for (size_t i = 0; i < attackStats.size(); i++)
	{
		if (attackStats[i].first == iAttackerId)
		{
			attackStats[i].second.push_back(SAttackStats(iAttackerPlot, iDefenderId, iPrevDamage, iDamageDealt, iDamageTaken));
			return;
		}
	}

	//unknown attacker? add a new entry
	vector<SAttackStats> data(1, SAttackStats(iAttackerPlot, iDefenderId, iPrevDamage, iDamageDealt, iDamageTaken));
	attackStats.push_back( make_pair(iAttackerId,data) );
}

bool CAttackCache::findAttack(int iAttackerId, int iAttackerPlot, int iDefenderId, int iPrevDamage, int& iDamageDealt, int& iDamageTaken) const
{
	for (size_t i = 0; i < attackStats.size(); i++)
	{
		if (attackStats[i].first == iAttackerId)
		{
			const vector<SAttackStats>& data = attackStats[i].second;
			for (size_t i = 0; i < data.size(); i++)
			{
				if (data[i].iAttackerPlot == iAttackerPlot &&
					data[i].iDefenderId == iDefenderId &&
					data[i].iDefenderPrevDamage == iPrevDamage)
				{
					iDamageDealt = data[i].iAttackerDamageDealt;
					iDamageTaken = data[i].iAttackerDamageTaken;
					gAttackCacheHit++;
					return true;
				}
			}
		}
	}

	iDamageDealt = 0;
	iDamageTaken = 0;
	gAttackCacheMiss++;
	return false;
}

//note that the score returned from this function is not multiplied by 10 yet
bool ScoreAttack(const CvTacticalPlot& tactPlot, const CvUnit* pUnit, const CvTacticalPlot& assumedPlot, eAggressionLevel eAggLvl, float fAggBias, CAttackCache& cache, STacticalAssignment& result)
{
	if (eAggLvl == AL_NONE)
	{
		result.iScore = -INT_MAX;
		return false;
	}

	bool bIsKill = false;
	int iDamageDealt = 0;
	int iDamageReceived = 0; //always zero for ranged attack
	int iExtraScore = 0; //splash damage and other bonuses

	//the damage calculation doesn't know about hypothetical flanking units, so we ignore it here and add it ourselves 
	int iPrevDamage = tactPlot.getDamage();
	int iPrevHitPoints = 0;

	const CvPlot* pUnitPlot = assumedPlot.getPlot();
	const CvPlot* pTestPlot = tactPlot.getPlot();
	bool bScoreReduction = false;
	bool bBelowHpLimitAfterMeleeAttack = false;

	//AL_NONE, AL_LOW, AL_MEDIUM, AL_HIGH, AL_BRAVEHEART
	//braveheart allows attacks for which you need luck to survive
	int hpLimit[5] = {80,60,40,20,-5};

	if (tactPlot.isEnemyCity()) //a plot can be both a city and a unit - in that case we would attack the city
	{
		CvCity* pEnemy = pTestPlot->getPlotCity();
		if (!pEnemy)
		{
			result.iScore = -INT_MAX;
			return false;
		}

		//first try the cache
		if (!cache.findAttack(pUnit->GetID(),pUnitPlot->GetPlotIndex(), pEnemy->GetID(), iPrevDamage, iDamageDealt, iDamageReceived))
		{
			iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pEnemy, pUnit, pUnitPlot, iDamageReceived, true, iPrevDamage, true);
			cache.storeAttack(pUnit->GetID(),pUnitPlot->GetPlotIndex(), pEnemy->GetID(), iPrevDamage, iDamageDealt, iDamageReceived);
		}

		//no suicide. note: should consider self-damage from previous attacks here ... blitz
		if (iDamageReceived>0 && pUnit->GetCurrHitPoints() - iDamageReceived < hpLimit[eAggLvl])
			bBelowHpLimitAfterMeleeAttack = true;

		iExtraScore = pUnit->GetRangeCombatSplashDamage(pTestPlot) + (pUnit->GetCityAttackPlunderModifier() / 50);
		iPrevHitPoints = pEnemy->GetMaxHitPoints() - pEnemy->getDamage() - iPrevDamage;

		//but we don't want our melee units to die so take into account self damage and counterattacks
		if (!pUnit->IsCanAttackRanged())
		{
			//if we have multiple units encircling the city, try to take into account their attacks as well
			//easiest way is to consider damage from last turn. so ideally ranged units start attacking and melee joins in later
			float fRemainingTurnsOnCity = iPrevHitPoints / (max(iDamageDealt*fAggBias, pEnemy->getDamageTakenLastTurn()*1.0f) + 1);
			//if the city cannot heal, be even more aggressive
			if (pEnemy->IsBlockaded(NO_DOMAIN))
				fRemainingTurnsOnCity = max(0.f, fRemainingTurnsOnCity - 1);

			//consider that we have other units around which can soak damage
			int iCounterattackDamage = pEnemy->canRangeStrike() ? pEnemy->rangeCombatDamage(pUnit, false, pUnitPlot, true) * (pEnemy->HasGarrison() ? 2 : 1) : 0;
			float fScaledCounterattackDamage = iCounterattackDamage / fAggBias;
			float fRemainingTurnsOnAttacker = pUnit->GetCurrHitPoints() / (iDamageReceived + fScaledCounterattackDamage + 1);

			//no attack if it's too early yet
			bool bGoodFirstAttack = !pUnit->IsHurt() && iDamageDealt > iDamageReceived && iDamageReceived < 23;
			if (fRemainingTurnsOnAttacker < fRemainingTurnsOnCity && !bGoodFirstAttack)
			{
				result.iScore = -INT_MAX;
				return false;
			}
		}

		//city blockaded? not 100% accurate, but anyway
		if (tactPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH, -1) == pTestPlot->countPassableNeighbors(NO_DOMAIN))
		{
			iDamageDealt *= max(100 + /*0 in CP, 20 in VP*/ GD_INT_GET(BLOCKADED_CITY_ATTACK_MODIFIER), 0);
			iDamageDealt /= 100;
		}

		//prefer ranged attacks over melee attacks (except if it's a kill, see below)
		if (iDamageReceived > 0)
			bScoreReduction = true;

		//special: if there are multiple enemy units around, try to attack those first
		if (tactPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH) > 1 && pUnit->AI_getUnitAIType() != UNITAI_CITY_BOMBARD)
			bScoreReduction = true;
	}
	else if (tactPlot.isEnemyCombatUnit())
	{
		CvUnit* pEnemy = tactPlot.getEnemyUnit();
		if (!pEnemy)
		{
			result.iScore = -INT_MAX;
			return false;
		}

		//first use the cache
		if (!cache.findAttack(pUnit->GetID(),pUnitPlot->GetPlotIndex(), pEnemy->GetID(), iPrevDamage, iDamageDealt, iDamageReceived))
		{
			//use the quick and dirty method ... and don't check for general bonus etc (their position isn't official yet - we handle that below)
			iDamageDealt = TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pEnemy, pUnit, pTestPlot, pUnitPlot, iDamageReceived, true, iPrevDamage, true);
			cache.storeAttack(pUnit->GetID(),pUnitPlot->GetPlotIndex(), pEnemy->GetID(), iPrevDamage, iDamageDealt, iDamageReceived);
		}

		iExtraScore = pUnit->GetRangeCombatSplashDamage(pTestPlot);
		iPrevHitPoints = pEnemy->GetCurrHitPoints() - iPrevDamage;

		//should consider self-damage from previous attacks here ... blitz
		if (iDamageReceived > 0 && pUnit->GetCurrHitPoints() - iDamageReceived < hpLimit[eAggLvl])
			bBelowHpLimitAfterMeleeAttack = true;

		//problem is flanking bonus affects combat strength, not damage, so the effect is nonlinear. anyway just assume 10% per adjacent unit
		if (!pUnit->IsCanAttackRanged()) //only for melee
		{
			//it works both ways!
			//note that this can go quite wrong if we're facing multiple enemy players!
			int iDelta = tactPlot.getNumAdjacentFriendlies(DomainForUnit(pUnit), assumedPlot.getPlotIndex()) - assumedPlot.getNumAdjacentEnemies(DomainForUnit(pUnit));
			iDamageDealt += (iDelta*iDamageDealt) / 10;
			iDamageReceived -= (iDelta*iDamageReceived) / 10;
		}

		//repeat attacks may give extra bonus
		if (iPrevDamage > 0)
		{
			int iBonus = pUnit->getMultiAttackBonus() + GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
			if (iBonus > 0) //the bonus affects attack strength, so the effect is hard to predict ...
				iDamageDealt += (iBonus * (iDamageDealt+iPrevDamage)) / 100;
		}

		//don't be as aggressive when attacking embarked units
		if (!pEnemy->IsCanAttack())
			fAggBias /= 2;

		//don't be distracted by attacks on barbarians when there are real enemies around
		if (pEnemy->getOwner() == BARBARIAN_PLAYER)
			bScoreReduction = true;
	}

	//fake general bonus
	if (assumedPlot.hasSupportBonus(-1))
	{
		iDamageDealt += iDamageDealt / 10;
		iDamageReceived -= iDamageReceived / 10;
	}

	//bonus for a kill
	//also consider near-kills as kills
	//if it doesn't work out we can try again
	//better than assuming it's not a kill and having a melee unit end up in a bad place
	if (iDamageDealt >= iPrevHitPoints - 1)
	{
		bIsKill = true;

		//don't hand out points for over-killing (but make an allowance for randomness)
		iDamageDealt = min(iDamageDealt, iPrevHitPoints + 10);

		if (tactPlot.isEnemyCity()) //city capture
		{
			bScoreReduction = false;

			//ranged units can't capture, so discourage the attack
			if (pUnit->IsCanAttackRanged())
			{
				result.eAssignmentType = A_RANGEATTACK; //not a kill!
				//don't continue flogging a dead horse
				if (iPrevHitPoints < 2)
					iDamageDealt = 0;
			}
			else
			{
				result.eAssignmentType = A_MELEEKILL;
			}
		}
		else //enemy unit killed
		{
			if (pTestPlot->getNumUnits() > 1 && !pTestPlot->isNeutralUnit(pUnit->getOwner(), false, false))
				iExtraScore += 20; //even more points for a double kill

			if (pUnit->getHPHealedIfDefeatEnemy() > 0)
				iDamageReceived = max(iDamageReceived - pUnit->getHPHealedIfDefeatEnemy(), -pUnit->getDamage()); //may turn negative, but can't heal more than current damage

			if (pUnit->IsCanAttackRanged())
				result.eAssignmentType = A_RANGEKILL;
			else
			{
				if (pUnitPlot->isFortification(pUnit->getTeam()))
					result.eAssignmentType = A_MELEEKILL_NO_ADVANCE;
				else
					result.eAssignmentType = A_MELEEKILL;
			}
		}
	}
	else
	{
		//note that we ignore melee attacks with advancing here (heavy charge promotion)
		//because it's too much chance involved. instead we abort the execution later if necessary and restart
		result.eAssignmentType = pUnit->IsCanAttackRanged() ? A_RANGEATTACK : A_MELEEATTACK;
	}

	//for melee units we check if the damage received is worth it ...
	if (iDamageReceived > 0)
	{
		float fAggFactor = /*100*/ GD_INT_GET(COMBAT_AI_OFFENSE_DAMAGEWEIGHT) / 100.f;
		switch (eAggLvl)
		{
		case AL_LOW:
			//want to do more damage than we take
			fAggFactor *= 0.7f;
			break;
		case AL_MEDIUM:
			//accept slightly more damage than we deal
			fAggFactor *= 1.1f;
			break;
		case AL_HIGH:
			//we check whether we can survive a counterattack in ScoreTurnEnd
			fAggFactor *= 2.3f;
			break;
		case AL_BRAVEHEART:
			//basically suicide
			fAggFactor *= 4.2f;
			break;
		default:
			result.iScore = -INT_MAX;
			return false;
		}

		//bias depends on the ratio of friendly to enemy units
		int iScaledDamage = int(iDamageDealt*fAggBias*fAggFactor + 0.5f);
		int iDamageDelta = iScaledDamage - iDamageReceived;
		bool bVoluntaryCancel = (bBelowHpLimitAfterMeleeAttack && iDamageDelta < 0 && result.eAssignmentType != A_MELEEKILL && result.eAssignmentType != A_MELEEKILL_NO_ADVANCE);
		bool bSuicideCancel = (pUnit->GetCurrHitPoints() - iDamageReceived < 1) && eAggLvl != AL_BRAVEHEART;
		if (bVoluntaryCancel || bSuicideCancel)
		{
			result.iScore = -INT_MAX;
			return false;
		}
	}

	//finally the almighty score
	//add previous damage again and again to make concentrated fire attractive
	//todo: consider pEnemy->getUnitInfo().GetProductionCost() and pEnemy->GetBaseCombatStrength()
	//todo: normalize damage done by max hp to balance between city attacks and unit attacks?
	//todo: score by net damage? so subtract damage taken?
	result.iScore = iDamageDealt + iExtraScore;
	if (bScoreReduction)
		result.iScore /= 2;

	result.iSelfDamage = iDamageReceived;
	result.iDamage = iDamageDealt;

	return bIsKill;
}

bool TacticalAIHelpers::IsPlayerCitadel(const CvPlot* pPlot, PlayerTypes ePlayer)
{
	if (!pPlot || ePlayer==NO_PLAYER || pPlot->getOwner() != ePlayer)
		return false;

	// Citadel here?
	ImprovementTypes eImprovement = pPlot->getImprovementType();
	if (eImprovement != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
	{
		CvImprovementEntry* pInfo = GC.getImprovementInfo(eImprovement);
		if (pInfo->GetNearbyEnemyDamage() <= /*10 in CP, 5 in VP*/ GD_INT_GET(ENEMY_HEAL_RATE))
			return false;

		if (pInfo->GetDefenseModifier() < 50)
			return false;

		return true;
	}
	
	return false;
}

bool TacticalAIHelpers::IsOtherPlayerCitadel(const CvPlot* pPlot, PlayerTypes ePlayer, bool bCheckWar)
{
	if (!pPlot || ePlayer==NO_PLAYER || !pPlot->isOwned())
		return false;

	// Citadel here?
	ImprovementTypes eImprovement = pPlot->getImprovementType();
	if (eImprovement != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
	{
		CvImprovementEntry* pInfo = GC.getImprovementInfo(eImprovement);
		if (pInfo->GetNearbyEnemyDamage() <= /*10 in CP, 5 in VP*/ GD_INT_GET(ENEMY_HEAL_RATE))
			return false;

		if (pInfo->GetDefenseModifier() < 50)
			return false;

		if (bCheckWar)
			return GET_PLAYER(ePlayer).IsAtWarWith(pPlot->getOwner());
		else
			return true;
	}
	
	return false;
}

int TacticalAIHelpers::SentryScore(const CvPlot * pPlot, PlayerTypes ePlayer)
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	int iScore = pPlot->defenseModifier(eTeam, false, false);

	const vector<CvPlot*>& possibleEnemyPlots = GC.getMap().GetPlotsAtRangeX(pPlot, 2, true, true);
	for (size_t i = 0; i < possibleEnemyPlots.size(); i++)
	{
		//there may be a sentinel null pointer
		if (possibleEnemyPlots[i] == NULL)
			continue;

		//don't need to cover a plot multiple times ... but don't want the sentry unit to shuffle around too much
		if (possibleEnemyPlots[i]->getVisibilityCount(eTeam) > 1)
			continue;

		//really we should consider whether the enemy can move there, not our team
		if (!possibleEnemyPlots[i]->isValidMovePlot(ePlayer, false))
			continue;

		iScore += 37; //less than a good defense bonus ...
	}

	return iScore;
}

STacticalAssignment ScorePlotForPillageMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition)
{
	//default action is do nothing and invalid score (not -INT_MAX to to prevent overflows!)
	STacticalAssignment result(unit.iPlotIndex,movePlot.iPlotIndex,unit.iUnitID,movePlot.iMovesLeft,unit.eMoveStrategy,TACTICAL_COMBAT_IMPOSSIBLE_SCORE,A_MOVE);

	//the plot we're checking right now
	const CvPlot* pTestPlot = testPlot.getPlot();
	const CvUnit* pUnit = unit.pUnit;

	if (pUnit->shouldPillage(pTestPlot, true) && !assumedPosition.unitHasAssignmentOfType(unit.iUnitID, A_PILLAGE) && !assumedPosition.plotHasAssignmentOfType(movePlot.iPlotIndex,A_PILLAGE))
	{
		result.eAssignmentType = A_PILLAGE;

		if (TacticalAIHelpers::IsOtherPlayerCitadel(pTestPlot, assumedPosition.getPlayer(), true))
			result.iScore = 500;
		else if (pTestPlot->getResourceType(pUnit->getTeam()) != NO_RESOURCE && GC.getResourceInfo(pTestPlot->getResourceType(pUnit->getTeam()))->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
			result.iScore = 200;
		else if (pUnit->getDamage() >= /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT))
			result.iScore = 100;

		if (pUnit->IsGainsXPFromPillaging())
			result.iScore += 50;

		if (!pUnit->hasFreePillageMove())
			result.iRemainingMoves -= min(result.iRemainingMoves, GD_INT_GET(MOVE_DENOMINATOR));
	}
	
	return result;
}

int ScorePlotForVisibility(const CvUnit* pUnit, const CvTacticalPlot& testPlot)
{
	int iVisiblityScore = 0;

	if (pUnit->IsCanAttackRanged())
	{
		//ignore range 1, we can always see those plots!
		//also ignore range 4+, this is too far and we don't have those plots cached

		if (pUnit->GetRange() > 1)
			iVisiblityScore += testPlot.getNumVisiblePlotsRange2();
		if (pUnit->GetRange() > 2)
			iVisiblityScore += testPlot.getNumVisiblePlotsRange3();
	}
	return iVisiblityScore;
}

int ScorePlotForPotentialAttacks(const CvUnit* pUnit, const CvTacticalPlot& testPlot, CvTacticalPlot::eTactPlotDomain eRelevantDomain, int iNumAttacks, const CvTacticalPosition& assumedPosition)
{
	int iBestAttackScore = 0;

	//check how much damage we can do to the enemies around this plot
	//works for both melee and ranged
	int iMaxRange = pUnit->IsCanAttackRanged() ? pUnit->GetRange() : 1;
	for (int iRange = testPlot.getEnemyDistance(eRelevantDomain); iRange <= iMaxRange; iRange++)
	{
		const vector<CvPlot*>& vAttackPlots = GC.getMap().GetPlotsAtRangeX(testPlot.getPlot(), iRange, true, !pUnit->IsRangeAttackIgnoreLOS());
		for (size_t iCount = 0; iCount < vAttackPlots.size(); iCount++)
		{
			CvPlot* pLoopPlot = vAttackPlots[iCount];
			if (!pLoopPlot || !assumedPosition.isAttackablePlot(pLoopPlot->GetPlotIndex()))
				continue;

			//we already know it's a valid plot with an enemy unit (or city)
			const CvTacticalPlot& targetPlot = assumedPosition.getTactPlot(pLoopPlot->GetPlotIndex());
			if (!targetPlot.isValid() || !targetPlot.isEnemy())
				continue;

			//even if we don't want to attack now we might want to attack next turn
			eAggressionLevel level = assumedPosition.getAggressionLevel() == AL_NONE ? AL_LOW : assumedPosition.getAggressionLevel();

			//we don't care for damage here but let's reuse the scoring function
			STacticalAssignment temp;
			//note here we don't care whether it's a kill or not - don't want to double dip for the move and the kill
			//also, if the attack would be next turn, chance is the enemy will flee, so it never happens
			ScoreAttack(targetPlot, pUnit, testPlot, level, assumedPosition.getAggressionBias(), gTactPosStorage.getCache(), temp);
			iBestAttackScore = max(temp.iScore, iBestAttackScore);
		}
	}

	//for simplicity assume we will get the same score for multiple attacks
	if (iNumAttacks>0)
		//we want the real attacks to have higher scores than movement to guarantee they are picked so divide by 2!
		return iNumAttacks * iBestAttackScore / 2;
	else
		//if we can't do it this turn then reduce the score even further because the attack is still hypothetical
		return iBestAttackScore / 4;
}


bool isKillAssignment(eUnitAssignmentType eAssignmentType)
{
	return eAssignmentType == A_MELEEKILL ||
		eAssignmentType == A_MELEEKILL_NO_ADVANCE ||
		eAssignmentType == A_RANGEKILL;
}

int ScoreTurnEnd(const CvUnit* pUnit, eUnitAssignmentType eLastAssignment, const CvTacticalPlot& testPlot, int iMovesLeft, CvTacticalPlot::eTactPlotDomain eRelevantDomain, int iSelfDamage, 
							const CvTacticalPosition& assumedPosition, eUnitMoveEvalMode evalMode)
{
	int iResult = 0;

	//the danger value reflects any defensive terrain bonuses
	//but unfortunately danger is not very useful here
	// * ZOC is unclear during simulation
	// * freshly revealed enemy units are not considered
	int	iDanger = pUnit->GetDanger(testPlot.getPlot(), assumedPosition.getKilledEnemies(), iSelfDamage);
	int iNumAdjFriendlies = (evalMode==EM_FINAL) ? testPlot.getNumAdjacentFriendliesEndTurn(eRelevantDomain) : testPlot.getNumAdjacentFriendlies(eRelevantDomain, -1);
	bool bIsFrontlineCitadelOrCity = (TacticalAIHelpers::IsPlayerCitadel(testPlot.getPlot(), assumedPosition.getPlayer()) || testPlot.getPlot()->isCity()) && testPlot.getEnemyDistance() < 3;

	//don't do it if it's a death trap (unless there is no other choice ...)
	int iNumAdjEnemies = testPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH);
	if (iNumAdjEnemies > 3 || (iNumAdjEnemies == 3 && assumedPosition.getAggressionBias() < 1))
		if (!bIsFrontlineCitadelOrCity)
			return INT_MAX;

	//can happen with garrisons, catch this case as it messes up the math
	if (iDanger == INT_MAX)
		iDanger = 10 * pUnit->GetMaxHitPoints();

	//avoid these plots
	if (testPlot.isNextToEnemyCitadel())
	{
		//the citadel damage is included in the danger value, but since it's guaranteed to happen give it additional emphasis
		//since we only check turn end after the fact, it might happen we check a lot of invalid positions if we put a hard exclusion here!
		iResult -= 50;
	}

	//unseen enemies might be hiding behind the edge, so assume danger there
	if (testPlot.isEdgePlot())
	{
		iDanger = max(pUnit->GetMaxHitPoints() / 2, iDanger);

		//siege units (with limited visibility) should not move there unless covered (the -1 is important)
		if (pUnit->visibilityRange()<2 && testPlot.getNumAdjacentFriendlies(DomainForUnit(pUnit), -1) < 2)
			return INT_MAX;
	}

	if (iDanger > 0)
	{
		//avoid extreme danger, except in citadels
		if (!bIsFrontlineCitadelOrCity && assumedPosition.getAggressionLevel() != AL_BRAVEHEART)
		{
			int iRemainingHP = pUnit->GetCurrHitPoints() - iSelfDamage;

			//the minimum amount of hitpoint we want a standalone unit to have for the expected counterattacks
			int iMagicNumber = (eLastAssignment == A_MELEEKILL || eLastAssignment == A_MELEEKILL_NO_ADVANCE || eLastAssignment == A_RANGEKILL) ? 23 : 42;

			//this is a bit cryptic to avoid integer truncation. consider danger/hp is the overkill factor.
			//if the overkill factor is high, we need more hitpoints
			int iLowHealthThreshold = (iMagicNumber * iDanger) / max(1, iRemainingHP);

			//if there is nothing we would cover or that covers us or we are low on health, don't do it
			if (iRemainingHP*max(iNumAdjFriendlies,1) < iLowHealthThreshold)
				return INT_MAX;

			//extra careful with siege units / carriers
			if (pUnit->AI_getUnitAIType() == UNITAI_CITY_BOMBARD || pUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA)
			{
				if (iNumAdjEnemies > 0 && iDanger > pUnit->GetMaxHitPoints())
					return INT_MAX;
				if (iNumAdjFriendlies == 0 && iDanger > pUnit->GetMaxHitPoints())
					return INT_MAX;
			}
		}


		//if we have friends around assume they will absorb some damage
		//of course the enemy will tend to focus fire, but then raw danger does not consider ZoC
		//todo: do not share damage for frontline units?
		iDanger /= max(iNumAdjFriendlies,1);

		//make it relative to current hitpoints
		int iScaledDanger = (iDanger * /*100*/ GD_INT_GET(COMBAT_AI_OFFENSE_DANGERWEIGHT)) / max(1, pUnit->GetCurrHitPoints());

		//danger values can get very high if there are many enemy units around, so try to normalize this a bit
		//this maps 225 to 225, higher values get flattened
		if (iScaledDanger > 225)
			iScaledDanger = 15*sqrti(iScaledDanger);

		//try to be more careful with highly promoted units
		iScaledDanger += (pUnit->getExperienceTimes100() - GET_PLAYER(assumedPosition.getPlayer()).GetAvgUnitExp100()) / 200;

		//penalty for high danger plots (should this be personality dependent?)
		iResult -= iScaledDanger;
	}

	//minor bonus for staying put and healing
	//don't add too much else it overrides the firstline/secondline order
	if (iMovesLeft == pUnit->maxMoves())
	{
		if (pUnit->IsHurt())
			iResult++; //cannot fortify, only heal
		if (testPlot.getEnemyDistance(eRelevantDomain) < 3 && !pUnit->noDefensiveBonus())
			iResult++; //fortification bonus!
	}

	//try to stay together, in pairs at least
	if (iNumAdjFriendlies > 0)
	{
		//carrier really want to be surrounded
		if (pUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA)
			iResult += iNumAdjFriendlies*11;
		else
			iResult+=7;
	}
	//when in doubt, stay under air cover
	if (testPlot.hasAirCover())
		iResult+=3;
	//when in doubt, hide from the enemy
	if (!testPlot.getPlot()->IsKnownVisibleToEnemy(pUnit->getOwner()))
		iResult++;

	//try to occupy enemy citadels!
	if (TacticalAIHelpers::IsOtherPlayerCitadel(testPlot.getPlot(), assumedPosition.getPlayer(), true))
		iResult += 11;

	//also occupy our own citadels
	if (bIsFrontlineCitadelOrCity)
	{
		if (pUnit->GetRange() > 1 || testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_LAND, -1)==0 || testPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_LAND)>0)
			iResult += TACTICAL_COMBAT_CITADEL_BONUS;
		else
			iResult += TACTICAL_COMBAT_CITADEL_BONUS/2;
	}

	//try not to be a sitting duck (faster than isNativeDomain but not entirely accurate)
	if (pUnit->getDomainType() != testPlot.getPlot()->getDomain())
		iResult-=3;

	//sometimes danger is zero, but maybe we're wrong, so look at plot defense too
	iResult += testPlot.getPlot()->defenseModifier(pUnit->getTeam(),false,false) / 5;

	//todo: take into account mobility at the proposed plot
	//todo: take into account ZOC when ending the turn

	return iResult;
}

STacticalAssignment ScorePlotForCombatUnitOffensiveMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, 
											const CvTacticalPosition& assumedPosition, eUnitMoveEvalMode evalMode) 
{
	//default action is do nothing and invalid score (not -INT_MAX to to prevent overflows!)
	STacticalAssignment result(unit.iPlotIndex,movePlot.iPlotIndex,unit.iUnitID,movePlot.iMovesLeft,unit.eMoveStrategy,TACTICAL_COMBAT_IMPOSSIBLE_SCORE,A_MOVE);

	//the plot we're checking right now
	const CvPlot* pTestPlot = testPlot.getPlot();
	const CvUnit* pUnit = unit.pUnit;

	//can we put a combat unit here?
	if (testPlot.isBlockedByNonSimUnit(DomainForUnit(pUnit)))
	{
		//double check - the blocking flag is not 100% reliable in cities
		if (!pTestPlot->isCoastalCityOrPassableImprovement(assumedPosition.getPlayer(),true,true) || pUnit->plot()!=pTestPlot)
			return result;
	}

	//cannot deal with enemies here, only friendly/empty plots
	if (testPlot.isEnemy())
		return result;

	//different contributions
	int iDamageScore = 0;
	int iDangerScore = 0;
	int iPlotScore = 0;

	//ranged attacks are cross-domain
	CvTacticalPlot::eTactPlotDomain eRelevantDomain = pUnit->IsCanAttackRanged() ? CvTacticalPlot::TD_BOTH : pTestPlot->isWater() ? CvTacticalPlot::TD_SEA : CvTacticalPlot::TD_LAND;

	//lookup desirability by unit strategy / enemy distance
	//even for intermediate plots, so as not to bias against them
	int iPlotTypeScores[5][5] = {
		{ -1,-1,-1,-1,-1 }, //none (should not occur)
		{  1, 12, 6, 1, -1 }, //firstline (note that it's ok to evaluate the score in an enemy plot for a firstline unit -> meleekill) 
		{ -1, 6, 12, 2, -1 }, //secondline
		{ -1, 1, 8, 12, -1 }, //thirdline
		{ -1, 1, 8,  8, -1 }, //support (can happen for damaged melee units)
	};
	iPlotScore = iPlotTypeScores[unit.eMoveStrategy][testPlot.getEnemyDistance(eRelevantDomain)];

	//if we made a kill, assume we are in a good plot
	//this is very important because enemy distance changes and we might end up with -1
	if (isKillAssignment(unit.eLastAssignment))
		iPlotScore = max(6, iPlotScore);

	if (testPlot.isEnemyCivilian()) //unescorted civilian
	{
		if (movePlot.iMovesLeft > 0 || testPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_LAND) == 0)
		{
			CvUnit* pCivilian = pTestPlot->getUnitByIndex(0); //there should be only one ...
			if (pCivilian)
			{
				//workers are not so important ...
				iPlotScore += (pCivilian->AI_getUnitAIType() == UNITAI_WORKER) ? 5 : 20;
				result.eAssignmentType = A_CAPTURE; //important so that the next assignment can be a move again
			}
		}
	}

	//many considerations are only relevant if we end the turn here (critical for skirmishers which can move after attacking ...)
	//we only consider this when explicitly ending the turn!
	if (evalMode != EM_INTERMEDIATE)
	{
		result.eAssignmentType = A_FINISH;

		// unit giving extra strength to an adjacent city?
		if (pUnit->GetAdjacentCityDefenseMod() > 0 && (pTestPlot->IsAdjacentCity(pUnit->getTeam()) || (pTestPlot->isCity() && pTestPlot->getTeam() == pUnit->getTeam())))
		{
			iPlotScore += pUnit->GetAdjacentCityDefenseMod();
		}

		iDangerScore = ScoreTurnEnd(pUnit, unit.eLastAssignment, testPlot, movePlot.iMovesLeft, eRelevantDomain, unit.iSelfDamage, assumedPosition, evalMode);

		if (iDangerScore == INT_MAX)
			return result; //don't do it

		//self damage is not included in danger calculation ...
		iDangerScore -= unit.iSelfDamage;
	}
	else
	{
		//how much damage could we do with our next moves?
		//check this only during simulation; don't use it for initial/final score as it decreases with enemies killed!
		int iMaxAttacks = NumAttacksForUnit(movePlot.iMovesLeft, unit.iAttacksLeft);
		iDamageScore = ScorePlotForPotentialAttacks(pUnit, testPlot, eRelevantDomain, iMaxAttacks, assumedPosition);

		//some indication of danger as a tiebreaker - final danger will be checked later
		int	iDanger = pUnit->GetDanger(testPlot.getPlot(), assumedPosition.getKilledEnemies(), unit.iSelfDamage);
		int iRemainingHP = pUnit->GetCurrHitPoints() - unit.iSelfDamage;
		int iOverkillFactor = min(3, iDanger / max(1, iRemainingHP));
		iDangerScore -= iOverkillFactor;

		//give a bonus for occupying a citadel even if it's just intermediate for now
		//but we want our units to take turns soaking damage, so we have to incentivise moving in.
		//bonus should be larger than 60 to override the difference between firstline/secondline base score.
		if (TacticalAIHelpers::IsPlayerCitadel(testPlot.getPlot(), assumedPosition.getPlayer()) && testPlot.getEnemyDistance() < 3)
		{
			if (pUnit->GetRange()>1 || testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_LAND, unit.iPlotIndex) == 0 || testPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_LAND) > 0)
	 			iDangerScore += TACTICAL_COMBAT_CITADEL_BONUS;
			else
				iDangerScore += TACTICAL_COMBAT_CITADEL_BONUS/2;
		}
		else if (TacticalAIHelpers::IsOtherPlayerCitadel(testPlot.getPlot(), assumedPosition.getPlayer(), true))
		{
			if (movePlot.iMovesLeft>0) //can pillage this turn
				iDangerScore += TACTICAL_COMBAT_CITADEL_BONUS*2;
			else
				iDangerScore += TACTICAL_COMBAT_CITADEL_BONUS;
		}
	}

	//consider visibility as well
	iDangerScore += ScorePlotForVisibility(pUnit, testPlot);

	//final score
	//danger values (typically negative!) are mostly useful as tiebreaker
	result.iScore = iDamageScore * 10 + iPlotScore * 10 + iDangerScore;

	//often there are multiple identical units which could move into a plot (eg in naval battles)
	//in that case we want to prefer the one which has more movement points left to make the movement animation look better
	result.iScore += result.iRemainingMoves / GD_INT_GET(MOVE_DENOMINATOR);

	return result;
}

STacticalAssignment ScorePlotForCombatUnitDefensiveMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, 
											const CvTacticalPosition& assumedPosition, eUnitMoveEvalMode evalMode)
{
	//default action is do nothing and invalid score (not -INT_MAX to to prevent overflows!)
	STacticalAssignment result(unit.iPlotIndex,movePlot.iPlotIndex,unit.iUnitID,movePlot.iMovesLeft,unit.eMoveStrategy,TACTICAL_COMBAT_IMPOSSIBLE_SCORE,A_MOVE);

	//the plot we're checking right now
	const CvPlot* pTestPlot = testPlot.getPlot();
	const CvUnit* pUnit = unit.pUnit;

	//can we put a combat unit here?
	if (testPlot.isBlockedByNonSimUnit(DomainForUnit(pUnit)))
	{
		//double check - the blocking flag is not 100% reliable in cities
		if (!pTestPlot->isCoastalCityOrPassableImprovement(assumedPosition.getPlayer(), true, true) || pUnit->plot() != pTestPlot)
			return result;
	}

	//cannot deal with enemies here, but we can capture civilians!
	if (testPlot.isEnemyCombatUnit()||testPlot.isEnemyCity())
		return result;

	//different contributions
	int iDamageScore = 0;
	int iDangerScore = 0;
	int iPlotScore = 0;

	//ranged attacks are cross-domain
	CvTacticalPlot::eTactPlotDomain eRelevantDomain = pUnit->IsCanAttackRanged() ? CvTacticalPlot::TD_BOTH : pTestPlot->isWater() ? CvTacticalPlot::TD_SEA : CvTacticalPlot::TD_LAND;

	//lookup score by unit strategy / enemy distance
	int iPlotTypeScores[5][5] = {
		{ -1,-1,-1,-1,-1 }, //none (should not occur)
		{ -1, 12, 6, 4, 1 }, //firstline
		{ -1, 1, 12, 8, 1 }, //secondline
		{ -1, 1, 8, 12, 1 }, //thirdline
		{ -1, 1, 8, 8, -1 }, //support (can happen for damaged melee units)
	};
	iPlotScore = iPlotTypeScores[unit.eMoveStrategy][testPlot.getEnemyDistance(eRelevantDomain)];

	//we use defensive moves also for gathering armies when there are no enemies around ...
	const CvTacticalPlot& targetPlot = assumedPosition.getTactPlot( assumedPosition.getTarget()->GetPlotIndex() );
	if (targetPlot.isValid() && !targetPlot.isEnemy())
	{
		//assume we want to defend the target, ie be ready for an attack from the "outside"
		//move close to the (friendly) target and weigh the target distance to be competitive with the enemy distance
		int iPlotDistance = plotDistance(*assumedPosition.getTarget(),*pTestPlot);
		iPlotScore += 3*(3 - iPlotDistance);
	}
	else
	{
		//how much damage could we do with our next moves?
		//for defensive moves we don't expect to kill any enemies, so consider it for all modes (initial, intermediate and final)
		int iMaxAttacks = NumAttacksForUnit(movePlot.iMovesLeft, unit.iAttacksLeft);
		iDamageScore = ScorePlotForPotentialAttacks(pUnit, testPlot, eRelevantDomain, iMaxAttacks, assumedPosition);
	}

	if (evalMode!=EM_INTERMEDIATE)
	{
		result.eAssignmentType = A_FINISH;
		iDangerScore = ScoreTurnEnd(pUnit, unit.eLastAssignment, testPlot, movePlot.iMovesLeft, eRelevantDomain, unit.iSelfDamage, assumedPosition, evalMode);

		// unit giving extra strength to an adjacent city?
		if (pUnit->GetAdjacentCityDefenseMod() > 0 && (pTestPlot->IsAdjacentCity(pUnit->getTeam()) || (pTestPlot->isCity() && pTestPlot->getTeam() == pUnit->getTeam())))
		{
			iPlotScore += pUnit->GetAdjacentCityDefenseMod();
		}

		if (iDangerScore == INT_MAX)
			return result; //don't do it
	}
	else
	{
		//some indication of danger as a tiebreaker - final danger will be checked later
		int	iDanger = pUnit->GetDanger(testPlot.getPlot(), assumedPosition.getKilledEnemies(), unit.iSelfDamage);
		int iRemainingHP = pUnit->GetCurrHitPoints() - unit.iSelfDamage;
		int iOverkillFactor = min(20, 5*iDanger / max(1, iRemainingHP));
		iDangerScore -= iOverkillFactor;
	}

	if (testPlot.isEnemyCivilian()) //unescorted civilian
	{
		if (movePlot.iMovesLeft > 0 || testPlot.getNumAdjacentEnemies(CvTacticalPlot::TD_LAND) == 0)
		{
			CvUnit* pCivilian = pTestPlot->getUnitByIndex(0); //there should be only one ...
			if (pCivilian)
			{
				//workers are not so important ...
				iPlotScore += (pCivilian->AI_getUnitAIType() == UNITAI_WORKER) ? 5 : 20;
				result.eAssignmentType = A_CAPTURE; //important so that the next assigment can be a move again
			}
		}
	}

	//consider visibility as well
	iDangerScore += ScorePlotForVisibility(pUnit, testPlot);

	//final score. dangerscore is typically negative!
	result.iScore = iPlotScore*10 + iDamageScore*10 + iDangerScore;

	//often there are multiple identical units which could move into a plot
	//in that case we want to prefer the one which has more movement points left
	result.iScore += result.iRemainingMoves / GD_INT_GET(MOVE_DENOMINATOR);

	return result;
}

//stacking with combat units is allowed here!
STacticalAssignment ScorePlotForNonFightingUnitMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, 
											const CvTacticalPosition& assumedPosition, eUnitMoveEvalMode evalMode)
{
	//default action is do nothing and invalid score (not -INT_MAX to to prevent overflows!)
	STacticalAssignment result(unit.iPlotIndex,movePlot.iPlotIndex,unit.iUnitID,movePlot.iMovesLeft,unit.eMoveStrategy,TACTICAL_COMBAT_IMPOSSIBLE_SCORE,A_MOVE);
	int iScore = 0;
		
	//the plot we're checking right now
	const CvPlot* pTestPlot = testPlot.getPlot();
	const CvUnit* pUnit = unit.pUnit;

	//cannot deal with enemies here, only friendly/empty plots
	if (testPlot.isEnemy())
		return result;

	//check distance to target if gathering (not attacking)
	const CvTacticalPlot& targetPlot = assumedPosition.getTactPlot( assumedPosition.getTarget()->GetPlotIndex() );
	if (targetPlot.isValid() && !targetPlot.isEnemy())
	{
		//can be treacherous with impassable terrain in between but everything else is much more complex
		int iPlotDistance = plotDistance(*assumedPosition.getTarget(),*pTestPlot);
		iScore += 3 - iPlotDistance;
	}

	//generals and admirals
	if (unit.eMoveStrategy == MS_SUPPORT)
	{
		//check distance to enemy in any case
		switch (testPlot.getEnemyDistance())
		{
		case 0:
			return result; //don't ever go there, wouldn't work anyway
			break;
		case 1:
			iScore = pTestPlot->isCity() ? 13 : 2; //dangerous to end the turn, avoid
			break;
		case 2:
			iScore = 23; //good for defense support, good for attack support, but risky
			break;
		case 3:
			iScore = 17; //good for defense support, not so good for attack support
			break;
		default:
			iScore = 5; //usual case for gathering moves, otherwise not really interesting
			break;
		}

		if (evalMode != EM_INTERMEDIATE)
		{
			//we want one of our own combat units covering us (either sim or non sim)
			if (testPlot.isCombatEndTurn())
			{
				bool bHaveSimCover = false;
				const vector<STacticalUnit>& units = testPlot.getUnitsAtPlot();
				for (size_t i = 0; i < units.size(); i++)
				{
					if (isCombatUnit(units[i].eMoveType))
					{
						//we have to make sure our protector will stay ...
						CvUnit* pDefender = GET_PLAYER(assumedPosition.getPlayer()).getUnit(units[i].iUnitID);
						if (!assumedPosition.lastAssignmentIsAfterRestart(units[i].iUnitID) && !pDefender->shouldHeal(false))
						{
							bHaveSimCover = true;
							break;
						}
					}
				}
				if (!bHaveSimCover)
				{
					bool bHaveRealCover = false;
					CvUnit* pBestDefender = pTestPlot->getBestDefender(assumedPosition.getPlayer());
					bool bDefenderIsGood = pBestDefender && pBestDefender->TurnProcessed() && !pBestDefender->isProjectedToDieNextTurn() && pBestDefender->GetDanger() < pBestDefender->GetCurrHitPoints();
					if (pTestPlot->isFriendlyCity(*pUnit) || bDefenderIsGood)
						bHaveRealCover = true;

					//don't do it
					if (!bHaveRealCover)
						return result;
				}
			}
			else if (testPlot.isNicePlotForCitadel() && pUnit->IsGreatGeneral() && movePlot.iMovesLeft > 0)
			{
				result.eAssignmentType = A_USE_POWER;
				result.iRemainingMoves = 0;
				iScore += 100;
			}
			else if (!pTestPlot->isFriendlyCity(*pUnit)) //cities are considered safe
				return result;

			//surrounding cover is also good
			int iFriends = (evalMode==EM_FINAL) ? testPlot.getNumAdjacentFriendliesEndTurn(CvTacticalPlot::TD_BOTH) : testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH, -1);
			iScore += iFriends;

			//when in doubt prefer the high ground - looks cooler
			if (pTestPlot->isHills() || pTestPlot->isMountain())
				iScore++;

			//try not to be a sitting duck (faster than isNativeDomain but not entirely accurate)
			if (pUnit->getDomainType() != pTestPlot->getDomain())
				iScore -= 3;
		}

		//points for supported units (count only the first ring for performance ...)
		int iFriends = testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH, -1);
		if (testPlot.hasFriendlyCombatUnit())
			iFriends++;

		//supported units count double plus one extra if we have covered
		iScore += iFriends*2;
		if (testPlot.hasFriendlyCombatUnit())
			iScore++;

		//avoid overlap. this works only because we ignore our own aura when calling this function!
		if (testPlot.hasSupportBonus(unit.iPlotIndex) && movePlot.iPlotIndex!=unit.iPlotIndex)
			iScore /= 2;
	}
	//plain embarked units
	else if (unit.eMoveStrategy == MS_EMBARKED)
	{
		//check distance to enemy in any case
		switch (testPlot.getEnemyDistance())
		{
		case 0:
			return result; //don't ever go there, wouldn't work anyway
			break;
		case 1:
			iScore = 2; //dangerous, only in emergencies
			break;
		default:
			iScore = 23; //embarked units don't care about getting close to enemies
			break;
		}

		//can we put a combat unit here?
		if (testPlot.isBlockedByNonSimUnit(DomainForUnit(pUnit)))
		{
			//double check - the blocking flag is not 100% reliable in cities
			if (!pTestPlot->isCoastalCityOrPassableImprovement(assumedPosition.getPlayer(), true, true) || pUnit->plot() != pTestPlot)
				return result;
		}

		if (evalMode!=EM_INTERMEDIATE)
		{
			//catch the case with infinite danger
			int iDanger = min(1000, pUnit->GetDanger(pTestPlot, assumedPosition.getKilledEnemies(), 0));

			//embarked units have limited vision! but if we don't expect combat, we can afford to be less careful
			if (testPlot.isEdgePlot())
				iDanger += (assumedPosition.getAggressionLevel() == AL_NONE) ? 20 : 50;

			//we want friends around us
			int iFriends = (evalMode==EM_FINAL) ? testPlot.getNumAdjacentFriendliesEndTurn(CvTacticalPlot::TD_BOTH) : testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH, -1);
			iScore -= iDanger / (iFriends + 1);
		}
	}

	//scale to be in range with actual fighting units
	result.iScore = iScore * 10;

	//often there are multiple identical units which could move into a plot
	//in that case we want to prefer the one which has more movement points left
	result.iScore += result.iRemainingMoves / GD_INT_GET(MOVE_DENOMINATOR);

	return result;
}

STacticalAssignment ScorePlotForRangedAttack(const SUnitStats& unit, const CvTacticalPlot& assumedUnitPlot, const CvTacticalPlot& enemyPlot, const CvTacticalPosition& assumedPosition)
{
	STacticalAssignment newAssignment(unit.iPlotIndex,enemyPlot.getPlotIndex(),unit.iUnitID,unit.iMovesLeft,unit.eMoveStrategy,-1,A_RANGEATTACK);

	//even if we don't want to attack now we might want to attack next turn
	eAggressionLevel level = assumedPosition.getAggressionLevel() == AL_NONE ? AL_LOW : assumedPosition.getAggressionLevel();

	//received damage is zero here but still use the correct unit number ratio so as not to distort scores
	bool bIsKill = ScoreAttack(enemyPlot, unit.pUnit, assumedUnitPlot, level, assumedPosition.getAggressionBias(), gTactPosStorage.getCache(), newAssignment);
	if (newAssignment.iScore < 0)
		return newAssignment;

	//times 10 to match with ScorePlotForCombatUnitOffensive()
	newAssignment.iScore *= 10;

	//what happens next?
	if (AttackEndsTurn(unit.pUnit, unit.iAttacksLeft))
	{
		//if we have movement to spare, we should be able to disengage before attacking?
		if (newAssignment.iRemainingMoves > GD_INT_GET(MOVE_DENOMINATOR) && assumedUnitPlot.getEnemyDistance()==1 && unit.eMoveStrategy != MS_FIRSTLINE)
			newAssignment.iScore /= 2;

		newAssignment.iRemainingMoves = 0;
	}
	else
	{
		newAssignment.iRemainingMoves -= min(newAssignment.iRemainingMoves, GD_INT_GET(MOVE_DENOMINATOR));

		//a bonus the further we can disengage after attacking
		newAssignment.iScore += (newAssignment.iRemainingMoves * 2) / GD_INT_GET(MOVE_DENOMINATOR);
	}

	//flat bonus for a kill
	if (bIsKill)
		newAssignment.iScore += 1000;

	//a slight boost for attacking the "real" target
	if ( enemyPlot.getPlotIndex()==assumedPosition.getTarget()->GetPlotIndex() )
		newAssignment.iScore += 2; 

	return newAssignment;
}

STacticalAssignment ScorePlotForMeleeAttack(const SUnitStats& unit, const CvTacticalPlot& assumedUnitPlot, const CvTacticalPlot& enemyPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition)
{
	//default action is invalid
	STacticalAssignment result(unit.iPlotIndex, movePlot.iPlotIndex, unit.iUnitID, unit.iMovesLeft, unit.eMoveStrategy, -INT_MAX, A_MELEEATTACK);

	//the plot we're checking right now
	const CvPlot* pEnemyPlot = enemyPlot.getPlot();

	//sanity check
	if (!enemyPlot.isValid() || !assumedUnitPlot.isValid())
		return result;

	//this is only for melee attacks - ranged attacks are handled separately
	const CvUnit* pUnit = unit.pUnit;
	if (!enemyPlot.isEnemy() || pUnit->IsCanAttackRanged())
		return result;

	//how often can we attack this turn (depending on moves left on the unit)
	int iMaxAttacks = min(unit.iAttacksLeft, (unit.iMovesLeft + GD_INT_GET(MOVE_DENOMINATOR) - 1) / GD_INT_GET(MOVE_DENOMINATOR));
	if (iMaxAttacks == 0)
		return result;

	//check how much damage we could do
	bool bIsKill = ScoreAttack(enemyPlot, pUnit, assumedUnitPlot, assumedPosition.getAggressionLevel(), assumedPosition.getAggressionBias(), gTactPosStorage.getCache(), result);
	if (result.iScore < 0)
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
		int iAttackCost = max(iMoveCost, GD_INT_GET(MOVE_DENOMINATOR));
		result.iRemainingMoves -= min(unit.iMovesLeft, iAttackCost);
	}

	//don't break formation if there are many enemies around
	if (result.eAssignmentType == A_MELEEKILL && enemyPlot.getNumAdjacentEnemies(DomainForUnit(pUnit)) > 3)
	{
		result.iScore = -INT_MAX;
		return result;
	}

	//bring it into the same range as movement (add 8 so we're always better than just finishing the turn on a frontline plot)
	result.iScore = (result.iScore + 8) * 10;

	//strongly discourage melee attack on cities if the real target is something else (capturing is ok)
	if (result.eAssignmentType != A_MELEEKILL && enemyPlot.isEnemyCity() && assumedPosition.getTarget() != pEnemyPlot)
		result.iScore /= 4;

	//a slight boost for attacking the "real" target or a citadel
	if (pEnemyPlot == assumedPosition.getTarget() || TacticalAIHelpers::IsOtherPlayerCitadel(pEnemyPlot, assumedPosition.getPlayer(), true))
		result.iScore += 3; 

	//combo bonus
	if (result.eAssignmentType == A_MELEEKILL && enemyPlot.isEnemyCivilian())
		result.iScore += 5;

	//flat bonus for a kill
	if (bIsKill)
		result.iScore += 1000;

	return result;
}

CvTacticalPlot::CvTacticalPlot(const CvPlot* plot, PlayerTypes ePlayer, const vector<CvUnit*>& allOurUnits) :
	pPlot(NULL) //important, invalid by default
{
	if (!plot || ePlayer == NO_PLAYER)
		return;

	//minor players ignore barb camps and the units inside
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	if (plot->getRevealedImprovementType(kPlayer.getTeam()) == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT) && kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		return;

	//the most important thing
	pPlot = plot;

	//constant
	bfBlockedByNonSimCombatUnit = 0;
	bHasAirCover = pPlot->HasAirCover(ePlayer);
	bIsVisibleToEnemy = pPlot->IsKnownVisibleToEnemy(kPlayer.GetID());

	//updated once at the beginning
	nVisiblePlotsNearEnemyRange2 = 0;
	nVisiblePlotsNearEnemyRange3 = 0;
	//maybe only plant citadels if we're not winning anyhow or if we have many generals? kPlayer.GetDiplomacyAI()->GetStateAllWars()
	bMightWantCitadel = kPlayer.IsNicePlotForCitadel(pPlot);

	//updated if necessary
	bEdgeOfTheKnownWorldUnknown = true;
	bEnemyCivilianPresent = false;
	bEdgeOfTheKnownWorld = false;
	bAdjacentToEnemyCitadel = false;
	pEnemyCombatUnit = NULL;

	//set only once
	bFriendlyDefenderEndTurn = false;

	iDamageDealt = 0;

	//set initial state, update after every move
	aiFriendlyCombatUnitsAdjacent[TD_BOTH] = 0;
	aiFriendlyCombatUnitsAdjacent[TD_LAND] = 0;
	aiFriendlyCombatUnitsAdjacent[TD_SEA] = 0;
	aiFriendlyCombatUnitsAdjacentEndTurn[TD_BOTH] = 0;
	aiFriendlyCombatUnitsAdjacentEndTurn[TD_LAND] = 0;
	aiFriendlyCombatUnitsAdjacentEndTurn[TD_SEA] = 0;
	nSupportUnitsAdjacent = 0;

	aiEnemyDistance[TD_BOTH] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
	aiEnemyDistance[TD_LAND] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
	aiEnemyDistance[TD_SEA] = TACTICAL_COMBAT_MAX_TARGET_DISTANCE;
	aiEnemyCombatUnitsAdjacent[TD_BOTH] = 0;
	aiEnemyCombatUnitsAdjacent[TD_LAND] = 0;
	aiEnemyCombatUnitsAdjacent[TD_SEA] = 0;

	//enemy distance alone is not enough
	//there may be multiple enemy combat units but we only care about the best defender
	//also ignore the official visibility, we only add tactical plots if we can see them in the sim
	pEnemyCombatUnit = pPlot->getBestDefender(NO_PLAYER,ePlayer,NULL,true,true);

	//concerning embarkation. this is complex because it allows combat units to stack, violating the 1UPT rule.
	//note that there are other exceptions as well, eg a fort can hold a naval unit and a land unit.
	//therefore we check for "native domain". we consider non-native domain units in the simulation but only allow moves into the native domain.
	//this means that 1UPT is still valid for all our simulated moves and we can ignore embarked defenders etc.

	//so here comes tricky logic to figure out whether we can use this plot
	for (int i = 0; i < pPlot->getNumUnits(); i++)
	{
		CvUnit* pPlotUnit = pPlot->getUnitByIndex(i);

		//ignore zombies
		if (pPlotUnit->isDelayedDeath())
			continue;

		//enemies
		if (GET_PLAYER(ePlayer).IsAtWarWith(pPlotUnit->getOwner()))
		{
			//combat units (embarked or not)
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
				//civilian
				bEnemyCivilianPresent = true;
		}
		//neutral units
		else if (ePlayer != pPlotUnit->getOwner())
		{
			//check if we can use the plot for combat units
			if (pPlotUnit->IsCanDefend())
			{
				if (pPlotUnit->getDomainType() == DOMAIN_LAND)
				{
					bfBlockedByNonSimCombatUnit |= 1;
					bfBlockedByNonSimCombatUnit |= 4;
				}
				else if (pPlotUnit->getDomainType() == DOMAIN_SEA)
				{
					bfBlockedByNonSimCombatUnit |= 2;
					bfBlockedByNonSimCombatUnit |= 4;
				}
			}
		}
		//owned units not included in sim
		else if (std::find(allOurUnits.begin(), allOurUnits.end(), pPlotUnit) == allOurUnits.end())
		{
			if (pPlotUnit->IsCanDefend())
			{
				if (pPlotUnit->getDomainType() == DOMAIN_LAND)
				{
					bfBlockedByNonSimCombatUnit |= 1;
					bfBlockedByNonSimCombatUnit |= 4;
				}
				else if (pPlotUnit->getDomainType() == DOMAIN_SEA)
				{
					bfBlockedByNonSimCombatUnit |= 2;
					bfBlockedByNonSimCombatUnit |= 4;
				}

				if (pPlotUnit->TurnProcessed())
					bFriendlyDefenderEndTurn = true;

				//rules for cities are complex so just don't try it
				if (pPlot->isCity())
					bfBlockedByNonSimCombatUnit = 7;
			}
		}
	}

	//important, not every enemy city has a garrison!
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

bool CvTacticalPlot::isBlockedByNonSimUnit(eTactPlotDomain eDomain) const
{
	if (eDomain == TD_BOTH)
		return bfBlockedByNonSimCombatUnit != 0;
	else if (eDomain == TD_LAND)
		return (bfBlockedByNonSimCombatUnit & 1) != 0;
	else if (eDomain == TD_SEA)
		return (bfBlockedByNonSimCombatUnit & 2) != 0;

	return false;
}

bool CvTacticalPlot::hasFriendlyCombatUnit() const
{
	for (size_t i = 0; i < vUnitsHere.size(); i++)
		if (isCombatUnit(vUnitsHere[i].eMoveType))
			return true;
	
	return false;
}

bool CvTacticalPlot::hasFriendlyEmbarkedUnit() const
{
	for (size_t i = 0; i < vUnitsHere.size(); i++)
		if (isEmbarkedUnit(vUnitsHere[i].eMoveType))
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

	for (size_t i = 0; i < vUnitsHere.size(); i++)
		if (isSupportUnit(vUnitsHere[i].eMoveType))
			return true;

	return false;
}

void CvTacticalPlot::setCombatUnitEndTurn(CvTacticalPosition& currentPosition, eTactPlotDomain unitDomain)
{
	//don't do this twice
	if (!pPlot || bFriendlyDefenderEndTurn)
		return;

	bFriendlyDefenderEndTurn = true;

	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for (int i = 0; i < 6; i++)
	{
		CvPlot* pNeighbor = aNeighbors[i];
		if (pNeighbor)
		{
			CvTacticalPlot& tactPlot = currentPosition.getTactPlotMutable(pNeighbor->GetPlotIndex());
			if (tactPlot.isValid())
			{
				tactPlot.aiFriendlyCombatUnitsAdjacentEndTurn[unitDomain]++;
				if (unitDomain != TD_BOTH)
					tactPlot.aiFriendlyCombatUnitsAdjacentEndTurn[TD_BOTH]++;

				if (tactPlot.aiFriendlyCombatUnitsAdjacentEndTurn[TD_BOTH] > 7)
					OutputDebugString("implausible amount of neighbors");
			}
		}
	}
}

void CvTacticalPlot::changeNeighboringUnitCount(CvTacticalPosition& currentPosition, const STacticalAssignment& move, int iChange) const
{
	//we don't count embarked units
	if (!pPlot || isEmbarkedUnit(move.eMoveType))
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
				if (isSupportUnit(move.eMoveType))
					tactPlot.nSupportUnitsAdjacent += iChange;
				else //embarked is already handled
				{
					CvTacticalPlot::eTactPlotDomain unitDomain = DomainForUnit(pUnit);
					tactPlot.aiFriendlyCombatUnitsAdjacent[unitDomain] += iChange;
					if (unitDomain != TD_BOTH)
						tactPlot.aiFriendlyCombatUnitsAdjacent[TD_BOTH] += iChange;

					if (tactPlot.aiFriendlyCombatUnitsAdjacent[TD_BOTH] > 7)
						OutputDebugString("implausible amount of neighbors");
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

	vUnitsHere.push_back( STacticalUnit(assignment.iUnitID,assignment.eMoveType));
	changeNeighboringUnitCount(currentPosition, assignment, +1);
}

void CvTacticalPlot::friendlyUnitMovingOut(CvTacticalPosition& currentPosition, const STacticalAssignment& assignment)
{
	for (vector<STacticalUnit>::iterator it = vUnitsHere.begin(); it != vUnitsHere.end(); it++)
	{
		if (assignment.iUnitID == it->iUnitID)
		{
			vUnitsHere.erase(it);
			changeNeighboringUnitCount(currentPosition, assignment, -1);
			return;
		}
	}

	OutputDebugString("invalid move\n");
}

//this is quite unreliable as it does not count non-simulated friendly units!
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

//this is quite unreliable as it does not count non-simulated friendly units!
int CvTacticalPlot::getNumAdjacentFriendliesEndTurn(eTactPlotDomain eDomain) const 
{ 
	return aiFriendlyCombatUnitsAdjacentEndTurn[eDomain]; 
}

bool CvTacticalPlot::removeEnemyUnitIfPresent()
{
	bool bReturn = false;
	pEnemyCombatUnit = NULL;

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
	//we only ever add plots, so if it's not a new plot and not an edge plot, nothing to do
	if (!bEdgeOfTheKnownWorld && !bEdgeOfTheKnownWorldUnknown)
		return false;

	//performance optimization ... only look at outward neighbors!
	CvPlot* pTarget = currentPosition.getTarget();
	int iRefDistance = plotDistance(pPlot->getX(), pPlot->getY(), pTarget->getX(), pTarget->getY());

	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
	for (int i = 0; i < 6; i++)
	{
		CvPlot* pNeighbor = aNeighbors[i];
		if (!pNeighbor)
			continue;

		int iNeighborDistance = plotDistance(pNeighbor->getX(), pNeighbor->getY(), pTarget->getX(), pTarget->getY());
		if (iNeighborDistance < iRefDistance)
			continue;

		//minor players ignore barb camps and the units inside
		CvPlayer& kPlayer = GET_PLAYER(currentPosition.getPlayer());
		if (pNeighbor->getRevealedImprovementType(kPlayer.getTeam()) == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT) && kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
			continue;

		const CvTacticalPlot& tactPlot = currentPosition.getTactPlot(pNeighbor->GetPlotIndex());
		//if the tactical plot is invalid, it's out of range or invisible on the main map.
		//these are the ones we need to check here
		if (!tactPlot.isValid())
		{
			//don't ignore enemy cities, we know they exist even if invisible
			if (pNeighbor->isCity() && GET_PLAYER(currentPosition.getPlayer()).IsAtWarWith(pNeighbor->getOwner()))
			{
				//poor man's deduplication
				if (landEnemies.empty() || landEnemies.back()!=pNeighbor->GetPlotIndex())
					landEnemies.push_back(pNeighbor->GetPlotIndex());
			}
			else if (pNeighbor->isVisible(GET_PLAYER(currentPosition.getPlayer()).getTeam()))
			{
				CvUnit* pEnemy = pNeighbor->getBestDefender(NO_PLAYER, currentPosition.getPlayer(), NULL, true);
				if (pEnemy)
				{
					if (pEnemy->getDomainType() == DOMAIN_LAND)
					{
						//poor man's deduplication
						if (landEnemies.empty() || landEnemies.back()!=pNeighbor->GetPlotIndex())
							landEnemies.push_back(pNeighbor->GetPlotIndex());
					}
					else if (pEnemy->getDomainType() == DOMAIN_SEA)
					{
						//poor man's deduplication
						if (seaEnemies.empty() || seaEnemies.back()!=pNeighbor->GetPlotIndex())
							seaEnemies.push_back(pNeighbor->GetPlotIndex());
					}
				}
			}
			else
				//if the neighbor is invisible but passable be careful as well, enemies might be hiding there
				bEdgeOfTheKnownWorld |= !pNeighbor->isImpassable();

			if (TacticalAIHelpers::IsOtherPlayerCitadel(pNeighbor, currentPosition.getPlayer(), true))
				bAdjacentToEnemyCitadel = true;
		}

	}

	bEdgeOfTheKnownWorldUnknown = false;
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

bool IsCombatUnit(const SUnitStats& unit)
{
	switch (unit.eMoveStrategy)
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

STacticalAssignment ScorePlotForMove(const SUnitStats& unit, const CvTacticalPlot& testPlot, const SMovePlot& movePlot, const CvTacticalPosition& assumedPosition, eUnitMoveEvalMode evalMode)
{
	if (IsCombatUnit(unit))
	{
		if (assumedPosition.getAggressionLevel() > AL_NONE)
			return ScorePlotForCombatUnitOffensiveMove(unit, testPlot, movePlot, assumedPosition, evalMode);
		else
			return ScorePlotForCombatUnitDefensiveMove(unit, testPlot, movePlot, assumedPosition, evalMode);
	}
	else
		return ScorePlotForNonFightingUnitMove(unit, testPlot, movePlot, assumedPosition, evalMode);
}

void CvTacticalPosition::getPreferredAssignmentsForUnit(const SUnitStats& unit, int nMaxCount) const
{
	//the challenge is that often a move can be good or bad depending on what our *other* units end up doing. so there are two strategies:
	//a) return as many moves as possible and check validity at the end.
	//b) return only "safe" moves in the sense that we dare to actually do them.
	//problem eg are generals, we want to move them close to the action but there might not be cover there or the cover might move away.
	//anyway experience shows if we choose an attractive but invalid move at the beginning of the sim this can poison everything
	//because so many positions can be generated from that we never get to examine at alternative beginnings in depth.
	//so we go with option B, even if it means we need to skip some daring moves.
	//on the other hand, the daring moves may still be possible later in the sim when they are not so daring anymore.
	//todo: try "fixup moves", meaning if a possible move is invalid, see if it can be made valid by moving another unit.
	gPossibleMoves.clear();

	const CvTacticalPlot& assumedUnitPlot = getTactPlot(unit.iPlotIndex);
	CvUnit* pUnit = GET_PLAYER(getPlayer()).getUnit(unit.iUnitID);
	if (!pUnit || !assumedUnitPlot.isValid())
		return;

	//what is the score for simply staying put and doing nothing?
	//important not to pass zero moves left, otherwise we might ignore possible attacks
	//do not score danger so we compare apples to apples below
	STacticalAssignment refAssignment = ScorePlotForMove(unit, assumedUnitPlot, SMovePlot(unit.iPlotIndex, 0, unit.iMovesLeft, 0), *this, EM_INTERMEDIATE);

	//check moves and melee attacks first
	const ReachablePlots& reachablePlots = getReachablePlotsForUnit(unit);
	for (ReachablePlots::const_iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
	{
		//the plot we're checking right now
		const CvTacticalPlot& testPlot = getTactPlot(it->iPlotIndex);

		//sanity check
		if (!testPlot.isValid())
			continue;

		if (IsCombatUnit(unit) && testPlot.isEnemy())
		{
			//ranged attacks are handled separately below
			if (pUnit->IsCanAttackRanged() || eAggression == AL_NONE)
				continue;

			//for a melee attack we need to move to a defined adjacent plot first
			if (plotDistance(*assumedUnitPlot.getPlot(), *testPlot.getPlot()) > 1)
				continue;

			STacticalAssignment newAssignment = ScorePlotForMeleeAttack(unit,assumedUnitPlot,testPlot,*it,*this);

			//attacks must have a positive score
			bool bCanEndTurnHere = (newAssignment.iRemainingMoves > 0) || couldEndTurnAfterThisAssignment(newAssignment);
			if (newAssignment.iScore > 0 && bCanEndTurnHere)
				gPossibleMoves.push_back(newAssignment);
		}
		else if (unit.iPlotIndex == it->iPlotIndex)
		{
			STacticalAssignment newAssignment = ScorePlotForPillageMove(unit, testPlot, *it, *this);
			//pillaging must have a positive score
			if (newAssignment.iScore > 0 && (newAssignment.iRemainingMoves>0 || couldEndTurnAfterThisAssignment(newAssignment)))
				gPossibleMoves.push_back(newAssignment);
			else if (refAssignment.iScore > TACTICAL_COMBAT_IMPOSSIBLE_SCORE)
			{
				int iBonus = 0;
				//give a bonus for potential fortifying/healing
				//redundant with ScoreTurnEnd but we have to make sure we consider these moves in the first place
				if (it->iMovesLeft == pUnit->maxMoves())
				{
					if (pUnit->getDamage() > /*10 in CP, 7 in VP*/ GD_INT_GET(FRIENDLY_HEAL_RATE) / 2)
					{
						//calling canHeal is too expensive, so fake it
						if (pUnit->getDomainType() != DOMAIN_SEA || testPlot.getPlot()->IsFriendlyTerritory(pUnit->getOwner()) || pUnit->isHealOutsideFriendly())
							iBonus += min(23, pUnit->getDamage());
					}
					if (pUnit->IsEverFortifyable())
						iBonus += (pUnit->GetDamageAoEFortified()>0) ? 37 : 23;
				}
				//same bonus is given for moving in ...
				if (TacticalAIHelpers::IsPlayerCitadel(testPlot.getPlot(), getPlayer()) && testPlot.getEnemyDistance() <= 3)
				{
					//bonus if this is not suicide or nobody is around to take over
					if (!pUnit->isProjectedToDieNextTurn() || testPlot.getNumAdjacentFriendlies(CvTacticalPlot::TD_LAND,-1) == 0)
	 					iBonus += TACTICAL_COMBAT_CITADEL_BONUS;
				}

				bool bCanEndTurnHere = couldEndTurnAfterThisAssignment(refAssignment);
				if (bCanEndTurnHere)
				{
					//subtract a little for wasted movement points to balance out what we added in the ref assignment
					gPossibleMoves.push_back(
						STacticalAssignment(unit.iPlotIndex, unit.iPlotIndex, unit.iUnitID, unit.iMovesLeft, unit.eMoveStrategy,
							iBonus - unit.iMovesLeft / GD_INT_GET(MOVE_DENOMINATOR), A_FINISH_TEMP));
				}
			}
		}
		else
		{
			//prevent two moves in a row, that is inefficient and can lead to "shuttling" behavior
			//note: if a move increases tile visibility we change it to MOVE_FORCED so we can move again!
			if (unit.eLastAssignment == A_MOVE || unit.eLastAssignment == A_MOVE_SWAP)
				continue;

			//make sure we have a chance to execute this move ... so skip it if there is an unmoveable block
			if (IsCombatUnit(unit))
			{
				int blocks = countBlockingUnitsAtPlot(testPlot.getPlotIndex(), unit.eMoveStrategy);
				if (blocks > 1 || (blocks == 1 && getAvailableUnitStats( getFirstBlockingUnitIDAtPlot(testPlot.getPlotIndex(), unit.eMoveStrategy) ) == NULL))
					continue;
			}

			STacticalAssignment newAssignment = ScorePlotForMove(unit, testPlot, *it, *this, EM_INTERMEDIATE);

			//may be invalid
			bool bCanEndTurnHereOrFlee = (newAssignment.iRemainingMoves>0) || couldEndTurnAfterThisAssignment(newAssignment);
			if (!bCanEndTurnHereOrFlee)
				continue;

			//score functions are biased so that only scores > 0 are interesting moves
			//still allow mildly negative moves here, maybe we want to do combo moves later!
			if (newAssignment.iScore > TACTICAL_COMBAT_IMPOSSIBLE_SCORE)
			{
				//important normalization step (after we check the cutoff in case ref score is negative!)
				newAssignment.iScore -= refAssignment.iScore;

				gPossibleMoves.push_back(newAssignment);
			}
		}
	}

	//ranged attacks (also if aggression level is NONE!)
	if (pUnit->IsCanAttackRanged() && unit.iAttacksLeft>0 && unit.iMovesLeft>0)
	{
		const vector<int>& rangeAttackPlots = getRangeAttackPlotsForUnit(unit);

		//what score for simply staying here and doing nothing? pass zero moves to ignore possible attacks
		STacticalAssignment positionScore = ScorePlotForMove(unit, assumedUnitPlot, SMovePlot(unit.iPlotIndex, 0, 0, 0), *this, EM_INTERMEDIATE);

		for (vector<int>::const_iterator it=rangeAttackPlots.begin(); it!=rangeAttackPlots.end(); ++it)
		{
			//the plot we're checking right now
			const CvTacticalPlot& enemyPlot = getTactPlot(*it);

			//note: all valid plots are visible by definition
			if (enemyPlot.isValid() && enemyPlot.isEnemy())
			{
				STacticalAssignment newAssignment(ScorePlotForRangedAttack(unit,assumedUnitPlot,enemyPlot,*this));

				bool bCanEndTurnHereOrFlee = (newAssignment.iRemainingMoves > 0) || couldEndTurnAfterThisAssignment(newAssignment);
				if (newAssignment.iScore > 0 && bCanEndTurnHereOrFlee)
				{
					//if we're not looking to pick a fight, de-emphasize attacks
					if (eAggression == AL_NONE)
						newAssignment.iScore /= 2;
					//discourage attacks on cities with non-siege units if the real target is something else
					if (enemyPlot.isEnemyCity() && getTarget() != enemyPlot.getPlot() && pUnit->GetRange()>1 && pUnit->AI_getUnitAIType()!=UNITAI_CITY_BOMBARD)
						newAssignment.iScore /= 2;

					gPossibleMoves.push_back(newAssignment);
				}
			}
		}
	}

	//we don't want the blocked moves for all units to have the same score
	//so use the number of alternatives as an indicator how urgent this is
	int iBlockedScore = max(0, 3 - (int)gPossibleMoves.size() / 3);

	//need to return in sorted order. note that we don't filter out bad (negative moves) they just are unlikely to get picked
	std::stable_sort(gPossibleMoves.begin(),gPossibleMoves.end());

	//don't return more than requested unless there is a tie
	if (gPossibleMoves.size() > (size_t)nMaxCount)
	{
		while (gPossibleMoves[nMaxCount].iScore == gPossibleMoves[nMaxCount-1].iScore && (size_t)nMaxCount<gPossibleMoves.size())
			nMaxCount++;

		gPossibleMoves.erase(gPossibleMoves.begin() + nMaxCount, gPossibleMoves.end());
	}

	//always add a 'do-nothing' move; this does not guarantee that it will get picked but it allows move-attack-flee for damaged units
	//order does not matter, the parent function will sort again ...
	gPossibleMoves.push_back(STacticalAssignment(unit.iPlotIndex, unit.iPlotIndex, unit.iUnitID, unit.iMovesLeft, unit.eMoveStrategy, iBlockedScore, A_BLOCKED));
}

//if we have many units we won't look at all of them (for performance reasons)
//obviously needs plot types to be defined
void CvTacticalPosition::dropSuperfluousUnits(int iMaxUnitsToKeep)
{
	//if we have weak units without a clear purpose we drop them
	if (iMaxUnitsToKeep > (int)availableUnits.size())
		iMaxUnitsToKeep = (int)availableUnits.size();

	//temporarily raise aggression level to make sure we consider all possible melee attacks (even those which only make sense after other attacks)
	eAggressionLevel actualLevel = getAggressionLevel();

	//very important before calling getPreferredAssignmentsForUnit()
	updateMovePlotsIfRequired();

	//get the best move for each unit
	gOverAllChoices.clear();
	bool bHaveSupport = false;
	for (size_t i = 0; i < availableUnits.size() && iMaxUnitsToKeep>0; i++)
	{
		//this always returns at least one move
		getPreferredAssignmentsForUnit(availableUnits[i], 1);

		int iScore = gPossibleMoves.front().iScore;

		//make sure to include a general if we have one
		if (availableUnits[i].eMoveStrategy == MS_SUPPORT)
		{
			if (bHaveSupport)
			{
				iScore = TACTICAL_COMBAT_IMPOSSIBLE_SCORE;
				iMaxUnitsToKeep--;
			}
			else
			{
				iScore += 1000;
				bHaveSupport = true;
			}
		}

		//this will be our sorting criterion
		availableUnits[i].iImportanceScore = iScore;
	}

	//reset aggression to the original value
	eAggression = actualLevel;

	std::stable_sort(availableUnits.begin(), availableUnits.end());

	//simply consider those extra units as blocked.
	//since addAssignment will modify availableUnits, we copy the relevant units first
	vector<SUnitStats> unitsToDrop( availableUnits.begin()+iMaxUnitsToKeep, availableUnits.end() );
	for (vector<SUnitStats>::iterator itUnit = unitsToDrop.begin(); itUnit != unitsToDrop.end(); ++itUnit)
	{
		vector<SUnitStats>::iterator toDrop = find_if(availableUnits.begin(), availableUnits.end(), PrMatchingUnit(itUnit->iUnitID));
		availableUnits.erase(toDrop);
		assignedMoves.push_back( STacticalAssignment(itUnit->iPlotIndex, itUnit->iPlotIndex, itUnit->iUnitID, itUnit->iMovesLeft, itUnit->eMoveStrategy, 0, A_BLOCKED) );
	}
}

void CvTacticalPosition::addInitialAssignments()
{
	//first pass. problem is scores might be off because officially we don't know where our units are yet
	for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
	{
		const CvTacticalPlot& tactPlot = getTactPlot(itUnit->iPlotIndex);
		if (tactPlot.isValid()) //failsafe
		{
			//we pretend the unit has zero moves, this means we do not score any possible attacks
			//this is important for symmetry with canStayInPlot()
			int iScore = ScorePlotForMove(*itUnit, tactPlot, SMovePlot(itUnit->iPlotIndex), *this, EM_INITIAL).iScore;
			addAssignment(STacticalAssignment(itUnit->iPlotIndex, itUnit->iPlotIndex, itUnit->iUnitID, itUnit->iMovesLeft, itUnit->eMoveStrategy, iScore, A_INITIAL));
		}
	}

	//second pass. fix up the scores with correct number of adjacent units
	for (vector<SUnitStats>::iterator itUnit = availableUnits.begin(); itUnit != availableUnits.end(); ++itUnit)
	{
		const CvTacticalPlot& tactPlot = getTactPlot(itUnit->iPlotIndex);
		if (tactPlot.isValid()) //failsafe
		{
			int iScore = ScorePlotForMove(*itUnit, tactPlot, SMovePlot(itUnit->iPlotIndex), *this, EM_INITIAL).iScore;
			STacticalAssignment* pInitial = getInitialAssignmentMutable(itUnit->iUnitID);
			if (pInitial)
				pInitial->iScore = iScore;
		}
	}
}

//finding a particular unit
struct PrIsMoveForUnit
{
	int iUnitID;
	PrIsMoveForUnit(int iID) : iUnitID(iID) {}
	bool operator()(const STacticalAssignment& other) { return iUnitID == other.iUnitID; }
};

bool CvTacticalPosition::makeNextAssignments(int iMaxBranches, int iMaxChoicesPerUnit, CvTactPosStorage& storage,
	vector<CvTacticalPosition*>& openPositionsHeap, vector<CvTacticalPosition*>& completedPositions, const PrPositionSortHeapGeneration& heapSort)
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

	//very important, lazy update
	updateMovePlotsIfRequired();

	gOverAllChoices.clear();
	for (size_t i=0; i<availableUnits.size(); i++)
	{
		getPreferredAssignmentsForUnit(availableUnits[i], iMaxChoicesPerUnit);
		gOverAllChoices.insert( gOverAllChoices.end(), gPossibleMoves.begin(), gPossibleMoves.end() );
	}

	//note that this is a stable sort, meaning in case of ties the original order is maintained
	//since we have a fixed cutoff, the simulation result depends on the order the moves were created in, ie the order of units
	std::stable_sort(gOverAllChoices.begin(), gOverAllChoices.end());
	for (size_t i=0; i<gOverAllChoices.size(); i++)
	{
		gMovesToAdd.clear();

		//easy case first, no conflict
		if (gOverAllChoices[i].eAssignmentType != A_MOVE)
		{
			//just do the original move
			gMovesToAdd.push_back(gOverAllChoices[i]);
		}
		else
		{
			 //possibly our target plot is still occupied ...
			int blocks = countBlockingUnitsAtPlot(gOverAllChoices[i].iToPlotIndex, gOverAllChoices[i].eMoveType);
			if (blocks == 0)
			{
				gMovesToAdd.push_back(gOverAllChoices[i]);
			}
			else if (blocks == 1)
			{
				//find best non-blocked move for blocking unit (search only one level deep)
				int blockID = getFirstBlockingUnitIDAtPlot(gOverAllChoices[i].iToPlotIndex, gOverAllChoices[i].eMoveType);
				for (size_t j = 0; j < gOverAllChoices.size(); j++)
				{
					if (gOverAllChoices[j].iUnitID==blockID && gOverAllChoices[j].eAssignmentType == A_MOVE)
					{
						//check if the combo has a positive score - but forcing out a similar unit is usually pointless
						int iBias = (gOverAllChoices[i].eMoveType != gOverAllChoices[j].eMoveType) ? 0 : 10;
						if (gOverAllChoices[i].iScore + gOverAllChoices[j].iScore < iBias)
							continue;

						//block wants to move into the plot currently occupied by this unit. bingo!
						if (gOverAllChoices[i].iFromPlotIndex == gOverAllChoices[j].iToPlotIndex)
						{
							const STacticalAssignment* pLA = getLatestAssignment(gOverAllChoices[j].iUnitID);
							if (pLA->eAssignmentType != A_MOVE_SWAP) //don't want to swap out a unit which was just swapped in
							{
								gMovesToAdd.push_back(gOverAllChoices[i]);
								gMovesToAdd.back().eAssignmentType = A_MOVE_SWAP; //this one will actually move both units
								gMovesToAdd.push_back(gOverAllChoices[j]);
								gMovesToAdd.back().eAssignmentType = A_MOVE_SWAP_REVERSE; //this one is just bookkeeping
								break;
							}
						}
						else if (!isMoveBlockedByOtherUnit(gOverAllChoices[j])) //free plot
						{
							//add the move to make space
							gMovesToAdd.push_back(gOverAllChoices[j]);
							//mark that this is a forced move so we're allowed to move back later
							gMovesToAdd.back().eAssignmentType = A_MOVE_FORCED;
							//now we can do the original move
							gMovesToAdd.push_back(gOverAllChoices[i]);
							break;
						}
					}
				}

				//did we find a block we can move?
				if (gMovesToAdd.size() != 2)
					continue;
			}
			else //too many blocks to sort out
				continue;
		}

		if (!gMovesToAdd.empty())
		{
			if (heapSort.bDepthFirst)
			{
				//maybe we have a block/finish move following our "primary" move? should be harmless to add them together. this reduces our total search depth
				while (i < gOverAllChoices.size() - 1 && (gOverAllChoices[i + 1].eAssignmentType == A_FINISH_TEMP || gOverAllChoices[i + 1].eAssignmentType == A_BLOCKED))
				{
					//if we don't have a move for the next unit yet
					if (std::find_if(gMovesToAdd.begin(), gMovesToAdd.end(), PrIsMoveForUnit(gOverAllChoices[i+1].iUnitID)) == gMovesToAdd.end())
					{
						gMovesToAdd.push_back(gOverAllChoices[++i]);
					}
					else
						break;
				}
			}

			//we need memory for the new child but we'll commit it only later after the uniqueness check
			CvTacticalPosition* pNewChild = storage.peekNext();
			if (!pNewChild)
				break;

			addChild(pNewChild);

			bool isConsistent = true;
			for (vector<STacticalAssignment>::iterator itNewMove = gMovesToAdd.begin(); itNewMove != gMovesToAdd.end(); ++itNewMove)
				isConsistent &= pNewChild->addAssignment(*itNewMove);

			//try to detect duplicates ...
			if (isConsistent && pNewChild->isUnique(TACTSIM_UNIQUENESS_CHECK_GENERATIONS))
			{
				//do we need to keep working on this one?
				if (pNewChild->isEarlyFinish() || pNewChild->isExhausted())
				{
					if (pNewChild->addFinishMovesIfAcceptable(pNewChild->isEarlyFinish()))
					{
						//good case, we're done
						completedPositions.push_back(pNewChild);
						storage.consumeOne();
						giValidEndPos++;
					}
					else
					{
						//position is illegal, forget about it so we can re-use the memory
						removeChild(pNewChild);
						giInvalidEndPos++;
					}
				}
				else
				{
					//also good case, keep this for the next round
					openPositionsHeap.push_back(pNewChild);
					push_heap(openPositionsHeap.begin(), openPositionsHeap.end(), heapSort);
					storage.consumeOne();
				}
			}
			else
				removeChild(pNewChild);
		}

		if (childPositions.size() >= (size_t)iMaxBranches)
			break;
	}

	//can happen we have no children if all were considered redundant or invalid
	//note that we also considered blocked moves for all children, but those also may turn out to be invalid if the unit doesn't have enough moves to flee 
	return !childPositions.empty();
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

	return countBlockingUnitsAtPlot(move.iToPlotIndex, move.eMoveType)!=0;
}

int CvTacticalPosition::countBlockingUnitsAtPlot(int iPlotIndex, eUnitMovementStrategy moveType) const
{
	int result = 0;
	const CvTacticalPlot& tactPlot = getTactPlot(iPlotIndex);
	const vector<STacticalUnit>& units = tactPlot.getUnitsAtPlot();

	for (size_t i = 0; i < units.size(); i++)
	{
		if (isCombatUnit(moveType) && isCombatUnit(units[i].eMoveType))
			result++;
		if (isEmbarkedUnit(moveType) && isEmbarkedUnit(units[i].eMoveType))
			result++;
		if (isSupportUnit(moveType) && isSupportUnit(units[i].eMoveType))
			result++;
	}

	return result;
}

int CvTacticalPosition::getFirstBlockingUnitIDAtPlot(int iPlotIndex, eUnitMovementStrategy moveType) const
{
	int result = -1;
	const CvTacticalPlot& tactPlot = getTactPlot(iPlotIndex);
	const vector<STacticalUnit>& units = tactPlot.getUnitsAtPlot();

	for (size_t i = 0; i < units.size(); i++)
	{
		if (isCombatUnit(moveType) && isCombatUnit(units[i].eMoveType))
			return units[i].iUnitID;
		if (isEmbarkedUnit(moveType) && isEmbarkedUnit(units[i].eMoveType))
			return units[i].iUnitID;
		if (isSupportUnit(moveType) && isSupportUnit(units[i].eMoveType))
			return units[i].iUnitID;
	}

	return result;
}

//can we stop now?
bool CvTacticalPosition::isEarlyFinish() const
{ 
	//simple - all enemies are gone (check for non-zero to make sure we aren't trivially complete)
	return nOriginalEnemies > 0 && nOriginalEnemies == killedEnemies.size();
}

//do we have to stop now
bool CvTacticalPosition::isExhausted() const
{
	return availableUnits.empty();
}

//see if all the plots where our units would end their turn are acceptable
//this is a deferred check because in the beginning it's not clear how many enemy units we can eliminate
bool CvTacticalPosition::addFinishMovesIfAcceptable(bool bEarlyFinish)
{ 
	int iBadUnitID = -1; //for debugging

	//only units which have exhausted their moves are in this array! if the sim was aborted, somebody else will hopefully pick up the pieces
	for (size_t i=0; i<notQuiteFinishedUnits.size(); i++)
	{
		const SUnitStats& unit = notQuiteFinishedUnits[i];
		const STacticalAssignment* pInitial = getInitialAssignment(unit.iUnitID);
		if (!pInitial)
			return false; //something wrong

		//if the unit is blocked but has movement left and can flee, let's assume that is ok
		if (unit.eLastAssignment == A_BLOCKED && unit.iMovesLeft>0)
			continue;

		//if we have a restart pending, that can also be ok if we're not planning to stay
		if (lastAssignmentIsAfterRestart(unit.iUnitID))
			continue;

		//make sure we don't leave a unit in an impossible position
		const CvTacticalPlot& tactPlot = getTactPlot(unit.iPlotIndex);
		int iNextTurnScore = ScorePlotForMove(unit, tactPlot, SMovePlot(unit.iPlotIndex), *this, EM_FINAL).iScore;
		if (iNextTurnScore > TACTICAL_COMBAT_IMPOSSIBLE_SCORE)
		{
			//if the score is acceptable, end their turn. unless the unit is blocked, then we may use them for other tasks
			if (unit.eLastAssignment != A_BLOCKED)
			{
				//handing out points for both the move and the finish is double-dipping ...
				const STacticalAssignment* lastMoveAssigment = getLatestMoveAssignment(unit.iUnitID);
				if (lastMoveAssigment)
					iTotalScore += max(0, iNextTurnScore - lastMoveAssigment->iScore);
				else
					iTotalScore += iNextTurnScore;

				assignedMoves.push_back(STacticalAssignment(unit.iPlotIndex, unit.iPlotIndex, unit.iUnitID, unit.iMovesLeft, unit.eMoveStrategy, iNextTurnScore, A_FINISH));
			}
		}
		else
		{
			iBadUnitID = unit.iUnitID;
			return false;
		}
	}

	//try to enforce some sort of sparsity, we should use only the minimum amount of units. 
	//so give a bonus for unmoved units. especially important in earlyFinish situations with many units.
	for (size_t i = 0; i < availableUnits.size(); i++)
	{
		const SUnitStats& unit = availableUnits[i];
		if (unit.eLastAssignment == A_INITIAL)
			iTotalScore += 30;
	}

	//scores look good and target was killed, we're done
	if (bEarlyFinish)
	{
		notQuiteFinishedUnits.clear();
		return true;
	}
	
	//second chance: did we kill anything (offensive) or at least improve the arrangement of our units (defensive)?
	//order is important here, need to add finish moves first!
	if (isKillOrImprovedPosition())
	{
		notQuiteFinishedUnits.clear();
		return true;
	}

	//do not clear notQuiteFinishedUnits for better debugging
	return false;
}

//although we try to pick "positive" moves only sometimes there are only bad choices
bool CvTacticalPosition::isKillOrImprovedPosition() const
{
	//if we made a kill, that is always good
	//on the other hand it messes up the enemy distance, so we cannot really compare before/after in that case!
	//note that regular attacks are checked below!
	for (size_t i = nFirstInterestingAssignment; i < assignedMoves.size(); i++)
		if (isKillAssignment(assignedMoves[i].eAssignmentType))
			return true;

	//find the original position to look up the unit move strategies
	const CvTacticalPosition* root = this;
	while (root->getParent())
		root = root->getParent();

	//if we did not make an attack, see if our units moved in the right way at least
	//note that this comparison only works because we know we did not kill an enemy, which would change enemyDistance!
	int iPositive = 0; //enemy distance change
	int iNegative = 0; //enemy distance change
	int iBefore = 0; //target distance change
	int iAfter = 0; //target distance change
	int iAttacksNoMove = 0;
	for (size_t i = nFirstInterestingAssignment; i < assignedMoves.size(); i++)
	{
		const STacticalAssignment& move = assignedMoves[i];
		if (move.eAssignmentType == A_FINISH)
		{
			//compare final to initial and see whether we came closer to the ideal distance
			const STacticalAssignment* initial = getInitialAssignment(move.iUnitID);
			const SUnitStats* unit = root->getAvailableUnitStats(move.iUnitID);
			const CvTacticalPlot& initialPlot = root->getTactPlot(initial->iFromPlotIndex);
			const CvTacticalPlot& finalPlot = getTactPlot(move.iFromPlotIndex);

			//only relevant in degenerate cases without enemies
			iBefore += plotDistance(*initialPlot.getPlot(), *root->getTarget());
			iAfter += plotDistance(*finalPlot.getPlot(), *root->getTarget());

			//which domain to use here? for simplicity assume firstline is melee and in-domain, everything else cross-domain
			CvTacticalPlot::eTactPlotDomain eRelevantDomain = unit->eMoveStrategy == MS_FIRSTLINE ? (finalPlot.getPlot()->isWater() ? CvTacticalPlot::TD_SEA : CvTacticalPlot::TD_LAND) : CvTacticalPlot::TD_BOTH;
			int iInitialDistance = initialPlot.getEnemyDistance(eRelevantDomain);
			int iFinalDistance = finalPlot.getEnemyDistance(eRelevantDomain);

			//occupying a citadel is always fine
			bool bIsStayingInFrontlineCitadel = (initialPlot.getPlotIndex() == finalPlot.getPlotIndex()) && 
				finalPlot.getEnemyDistance() < 3 && TacticalAIHelpers::IsPlayerCitadel(finalPlot.getPlot(), getPlayer());

			switch (unit->eMoveStrategy)
			{
			case MS_NONE:
				UNREACHABLE(); // Units are always supposed to be assigned a strategy.
			case MS_FIRSTLINE:
				if (bIsStayingInFrontlineCitadel)
					iPositive++; 
				else if (iInitialDistance != 1 && iFinalDistance == 1)
					iPositive++;
				else if (iInitialDistance == 1 && iFinalDistance != 1)
					iNegative++;
				break;
			case MS_SECONDLINE:
				if (bIsStayingInFrontlineCitadel)
					iPositive++;
				else if (iInitialDistance != 2 && iFinalDistance == 2)
					iPositive++;
				else if (iInitialDistance == 2 && iFinalDistance != 2)
					iNegative++;
				break;
			case MS_THIRDLINE:
				//thirdline is a bit different, ignore citadels
				if (iInitialDistance == 1 && iFinalDistance > 1)
					iPositive++;
				if (iInitialDistance > 1 && iFinalDistance == 1)
					iNegative++;
				break;
			case MS_SUPPORT:
				//ignore for now
				break;
			case MS_EMBARKED:
				//ignore for now
				break;
			}
		}
		else if (move.eAssignmentType == A_RANGEATTACK || move.eAssignmentType == A_MELEEATTACK)
			//we already checked for kills!
			iAttacksNoMove++;

		//note that RESTARTS are ignored here ... just hope that we still find good moves after the restart
	}
	
	//staying in place and bombarding is fine! note that retreating a damaged unit counts as positive because it should be MS_THIRDLINE
	return (iNegative < iPositive) || (iNegative == iPositive && iAttacksNoMove>0) || (eAggression==AL_NONE && iAfter<iBefore);
}

//this influences how daring we'll be
void CvTacticalPosition::countEnemiesAndCheckVisibility()
{
	if (parentPosition != NULL)
		CUSTOMLOG("should not happen");

	//todo: should we count non-simulated friendlies as well?
	nOurOriginalUnits = availableUnits.size();
	nOriginalEnemies = 0;

	for (size_t i = 0; i < tactPlots.size(); i++)
	{
		if (tactPlots[i].isEnemy())
		{
			nOriginalEnemies++; //units and cities
			enemyPlots.push_back(tactPlots[i].getPlotIndex());
		}

		//ignore range 1, we can always see those plots so they are boring
		//ignore range 4+, this is too far out and we don't have those plots cached
		const vector<CvPlot*>& vSeeToPlots2 = GC.getMap().GetPlotsAtRangeX(tactPlots[i].getPlot(), 2, true, true);
		const vector<CvPlot*>& vSeeToPlots3 = GC.getMap().GetPlotsAtRangeX(tactPlots[i].getPlot(), 3, true, true);
		int iVisiblityScore2 = 0;
		int iVisiblityScore3 = 0;

		//give a bonus to plots with high outward visibility, an enemy might come into range next turn!
		//however, we are only interested in the direction towards the enemy ... look at enemy distance as a proxy 
		for (size_t j = 0; j < vSeeToPlots2.size(); j++)
		{
			//null is used as a sentinel value
			if (vSeeToPlots2[j] == NULL)
				continue;

			const CvTacticalPlot& testPlot = getTactPlot(vSeeToPlots2[j]->GetPlotIndex());
			if (testPlot.isValid() && testPlot.getEnemyDistance() <= 3)
				iVisiblityScore2++;
		}
		for (size_t j = 0; j < vSeeToPlots3.size(); j++)
		{
			//null is used as a sentinel value
			if (vSeeToPlots3[j] == NULL)
				continue;

			const CvTacticalPlot& testPlot = getTactPlot(vSeeToPlots3[j]->GetPlotIndex());
			if (testPlot.isValid() && testPlot.getEnemyDistance() <= 3)
				iVisiblityScore3++;
		}

		tactPlots[i].setNumVisiblePlotsRange2(iVisiblityScore2);
		tactPlots[i].setNumVisiblePlotsRange3(iVisiblityScore3);
	}

	//need this to be sorted for binary search
	std::sort(enemyPlots.begin(), enemyPlots.end());
}

void CvTacticalPosition::refreshVolatilePlotProperties()
{
	//important, we are going to modify them all
	cacheAllTactPlotsLocally();

	gLandEnemies.clear();
	gSeaEnemies.clear();
	gCitadels.clear();

	for (vector<CvTacticalPlot>::iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
	{
		//need to check whether this is a city or actual units
		if (it->getEnemyDistance(CvTacticalPlot::TD_LAND) == 0 && it->isEnemyCombatUnit())
			gLandEnemies.push_back( it->getPlotIndex() );
		if (it->getEnemyDistance(CvTacticalPlot::TD_SEA) == 0 && it->isEnemyCombatUnit())
			gSeaEnemies.push_back( it->getPlotIndex() );

		it->resetVolatileProperties();

		//include plots with enemies if they are just outside of the simulation range
		//we won't attack them but we won't ignore them either
		it->checkEdgePlotsForSurprises(*this,gLandEnemies,gSeaEnemies);

		if (TacticalAIHelpers::IsOtherPlayerCitadel(it->getPlot(), getPlayer(), true) && !plotHasAssignmentOfType(it->getPlotIndex(), A_PILLAGE))
			gCitadels.push_back(it->getPlotIndex());
	}

	//distance transform pt1
	for (size_t i=0; i<gLandEnemies.size(); i++)
		for (vector<CvTacticalPlot>::iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
		{
			int iDistance = plotDistance(*GC.getMap().plotByIndexUnchecked(gLandEnemies[i]), *it->getPlot());
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
	for (size_t i=0; i<gSeaEnemies.size(); i++)
		for (vector<CvTacticalPlot>::iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
		{
			int iDistance = plotDistance(*GC.getMap().plotByIndexUnchecked(gSeaEnemies[i]), *it->getPlot());
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
	for (size_t i=0; i<gCitadels.size(); i++)
	{
		//iterate neighbors
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(GC.getMap().plotByIndexUnchecked(gCitadels[i]));
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

//need a default constructor for stl containers ...
CvTacticalPosition::CvTacticalPosition()
{
	initFromScratch(NO_PLAYER, AL_NONE, NULL);
}

void CvTacticalPosition::initFromScratch(PlayerTypes player, eAggressionLevel eAggLvl, CvPlot* pTarget)
{
	ePlayer = player;
	pTargetPlot = pTarget;
	eAggression = eAggLvl;
	nOurOriginalUnits = 0;
	nOriginalEnemies = 0;
	nFirstInterestingAssignment = 0;
	iTotalScore = 0;
	iScoreOverParent = 0; 
	parentPosition = NULL;
	iGeneration = 0;
	iID = 1; //zero doesn't work here
	movePlotUpdateFlag = 0;

	childPositions.clear();
	tactPlotLookup.clear();
	tactPlots.clear();
	availableUnits.clear();
	notQuiteFinishedUnits.clear();
	assignedMoves.clear();
	enemyPlots.clear();
	freedPlots.clear();
	killedEnemies.clear();
}

void CvTacticalPosition::initFromParent(const CvTacticalPosition& parent)
{
	dummyPlot = parent.dummyPlot;
	ePlayer = parent.ePlayer;
	pTargetPlot = parent.pTargetPlot;
	eAggression = parent.eAggression;
	nOurOriginalUnits = parent.nOurOriginalUnits;
	nOriginalEnemies = parent.nOriginalEnemies;
	nFirstInterestingAssignment = parent.nFirstInterestingAssignment;
	iTotalScore = parent.iTotalScore;
	iScoreOverParent = 0;
	parentPosition = &parent;
	movePlotUpdateFlag = parent.movePlotUpdateFlag;
	iGeneration = parent.iGeneration + 1;

	//clever scheme to encode the tree structure into IDs
	//works only if the tree is not too wide or too deep
	if (parent.getID() < ULLONG_MAX / 10 - 10)
		iID = parent.getID() * 10 + parent.childPositions.size();
	else
		iID = ULLONG_MAX;

	//childPositions stays empty!
	childPositions.clear();

	//these are cached locally
	tactPlotLookup.clear();
	tactPlots.clear();

	//copied from parent, modified when addAssignment is called
	assignedMoves = parent.assignedMoves;
	availableUnits = parent.availableUnits;
	notQuiteFinishedUnits = parent.notQuiteFinishedUnits;
	freedPlots = parent.freedPlots;
	enemyPlots = parent.enemyPlots;
	killedEnemies = parent.killedEnemies;
}

void CvTacticalPosition::wipe()
{
	//make sure the capacity of these containers doesn't grow too much over time
	vector<pair<int, size_t>>().swap(tactPlotLookup);
	vector<CvTacticalPlot>().swap(tactPlots);
	vector<STacticalAssignment>().swap(assignedMoves);
	vector<SUnitStats>().swap(availableUnits);
	vector<SUnitStats>().swap(notQuiteFinishedUnits);
	iID = 0; //easier debugging
}

bool CvTacticalPosition::removeChild(CvTacticalPosition* pChild)
{
	//just unlink the child - do not delete it, the memory is allocated statically
	vector<CvTacticalPosition*>::iterator it = find(childPositions.begin(), childPositions.end(), pChild);
	if (it!=childPositions.end())
		childPositions.erase(it);

	return false;
}

size_t CvTacticalPosition::addChild(CvTacticalPosition* pChild)
{
	if (pChild)
	{
		childPositions.push_back(pChild); //this order is better for generating an ID for the child
		pChild->initFromParent(*this);
	}
	return childPositions.size();
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

void CvTacticalPosition::updateMoveAndAttackPlotsForUnit(SUnitStats unit)
{
	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(unit.iUnitID);
	CvPlot* pStartPlot = GC.getMap().plotByIndexUnchecked(unit.iPlotIndex);

	TCachedMovePlots::iterator itP = gReachablePlotsLookup.find(SPathFinderStartPos(unit, freedPlots));
	if (itP != gReachablePlotsLookup.end())
		gMovePlotsCacheHit++;
	else
	{
		gMovePlotsCacheMiss++;

		//note: we allow (intermediate) embarkation here but filter out the non-native plots later (useful for denmark and lategame)
		int iMoveFlags = CvUnit::MOVEFLAG_IGNORE_STACKING_SELF | CvUnit::MOVEFLAG_IGNORE_DANGER;
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

			if (unit.eMoveStrategy == MS_EMBARKED)
			{
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
					//we don't want to fight so embarkation is ok if we're careful
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

			reachablePlotsPruned.insertNoIndex(*it);
		}

		reachablePlotsPruned.createIndex();
		gReachablePlotsLookup[SPathFinderStartPos(unit, freedPlots)] = reachablePlotsPruned;
	}

	//simply ignore visibility here, later there's a check if there is a valid tactical plot for the targets
	TCachedRangeAttackPlots::iterator itA = gRangeAttackPlotsLookup.find(make_pair(unit.iUnitID, unit.iPlotIndex));
	if (itA != gRangeAttackPlotsLookup.end())
		gAttackPlotsCacheHit++;
	else
	{
		gAttackPlotsCacheMiss++;
		vector<int> rangeAttackPlots = TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit, pStartPlot, true, true);
		gRangeAttackPlotsLookup[make_pair(unit.iUnitID, unit.iPlotIndex)] = rangeAttackPlots;
	}
}

bool CvTacticalPosition::unitHasAssignmentOfType(int iUnitID, eUnitAssignmentType assignmentType) const
{
	for (size_t i = nFirstInterestingAssignment; i < assignedMoves.size(); i++)
	{
		const STacticalAssignment& a = assignedMoves[i];
		if (a.iUnitID == iUnitID && a.eAssignmentType == assignmentType)
			return true;
	}

	return false;
}

bool CvTacticalPosition::plotHasAssignmentOfType(int iToPlotIndex, eUnitAssignmentType assignmentType) const
{
	for (size_t i = nFirstInterestingAssignment; i < assignedMoves.size(); i++)
	{
		const STacticalAssignment& a = assignedMoves[i];
		if (a.iToPlotIndex == iToPlotIndex && a.eAssignmentType == assignmentType)
			return true;
	}

	return false;
}

bool CvTacticalPosition::isAttackablePlot(int iPlotIndex) const
{
	return std::binary_search( enemyPlots.begin(), enemyPlots.end(), iPlotIndex );
}

pair<int,int> CvTacticalPosition::doVisibilityUpdate(const STacticalAssignment& newAssignment)
{
	int nNewEnemies = 0;

	//may need to add some new tactical plots - ideally we should reconsider all queued assignments afterwards
	//the next round of assignments will take into account the new plots in any case
	getPlotsWithChangedVisibility(newAssignment, gNewlyVisiblePlots);
	for (size_t i=0; i<gNewlyVisiblePlots.size(); i++)
	{
		//since it was invisible before, we know there are no friendly units around
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(gNewlyVisiblePlots[i]);
		if (pPlot) //also create plots for neutral units - otherwise edgeOfTheKnownWorld is not correct
		{
			//can pass empty set of units - the plot was invisible before so we know there is none of our units there
			if (addTacticalPlot(pPlot, vector<CvUnit*>()))
			{
				//we revealed a new enemy ... need to execute moves up to here, do a danger plot update and reconsider
				if (pPlot->isEnemyUnit(ePlayer, true, false))
					nNewEnemies++;

#if defined(MOD_CORE_DEBUGGING)
				if (MOD_CORE_DEBUGGING)
				{
					//make sure that the adjacent unit count is correct
					//normally it should because adjacent plots are visible from the beginning but ...
					CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(GC.getMap().plotByIndexUnchecked(pPlot->GetPlotIndex()));
					for (int i = 0; i < 6; i++)
					{
						CvPlot* pNeighbor = aNeighbors[i];
						if (!pNeighbor)
							continue;

						const CvTacticalPlot& neighborPlot = getTactPlot(pNeighbor->GetPlotIndex());
						if (neighborPlot.isValid())
						{
							const vector<STacticalUnit>& units = neighborPlot.getUnitsAtPlot();
							for (size_t j = 0; j < units.size(); j++)
								FAssert(!isCombatUnit(units[j].eMoveType));
						}
					}
				}
#endif
			}
		}
	}

	if (nNewEnemies>0)
		refreshVolatilePlotProperties();

	return make_pair( (int)gNewlyVisiblePlots.size(), nNewEnemies);
}

bool CvTacticalPosition::lastAssignmentIsAfterRestart(int iUnitID) const
{
	bool bHaveRestart = false;
	for (size_t i=nFirstInterestingAssignment; i<assignedMoves.size(); i++)
	{
		const STacticalAssignment& a = assignedMoves[i];
		if (a.eAssignmentType == A_RESTART)
		{
			bHaveRestart = true;
			continue;
		}

		if (bHaveRestart && a.iUnitID == iUnitID && a.eAssignmentType != A_FINISH && a.eAssignmentType != A_FINISH_TEMP)
			return true;
	}

	return false;
}

const SUnitStats * CvTacticalPosition::getAvailableUnitStats(int iUnitID) const
{
	for (vector<SUnitStats>::const_iterator it = availableUnits.begin(); it != availableUnits.end(); ++it)
		if (it->iUnitID == iUnitID)
			return &(*it);

	return NULL;
}

const STacticalAssignment* CvTacticalPosition::getInitialAssignment(int iUnitID) const
{
	for (vector<STacticalAssignment>::const_iterator it = assignedMoves.begin(); it != assignedMoves.end(); ++it)
		if (it->iUnitID == iUnitID && it->eAssignmentType == A_INITIAL)
			return &(*it);

	return NULL;
}

STacticalAssignment * CvTacticalPosition::getInitialAssignmentMutable(int iUnitID)
{
	for (vector<STacticalAssignment>::iterator it = assignedMoves.begin(); it != assignedMoves.end(); ++it)
		if (it->iUnitID == iUnitID && it->eAssignmentType == A_INITIAL)
			return &(*it);

	return NULL;
}

const STacticalAssignment* CvTacticalPosition::getLatestMoveAssignment(int iUnitID) const
{
	for (size_t i = assignedMoves.size()-1; i >= nFirstInterestingAssignment; i--)
		if (assignedMoves[i].iUnitID == iUnitID)
			//MOVE_SWAP_REVERSE and MOVE_FORCED can never occur alone, ignore them
			if (assignedMoves[i].eAssignmentType == A_MOVE || assignedMoves[i].eAssignmentType == A_MOVE_SWAP)
				return &(assignedMoves[i]);

	return NULL;
}

const STacticalAssignment* CvTacticalPosition::getLatestAssignment(int iUnitID) const
{
	for (vector<STacticalAssignment>::const_reverse_iterator it = assignedMoves.rbegin(); it != assignedMoves.rend(); ++it)
		if (it->iUnitID == iUnitID)
			return &(*it);

	return NULL;
}

STacticalAssignment * CvTacticalPosition::getLatestAssignmentMutable(int iUnitID)
{
	for (vector<STacticalAssignment>::reverse_iterator it = assignedMoves.rbegin(); it != assignedMoves.rend(); ++it)
		if (it->iUnitID == iUnitID)
			return &(*it);

	return NULL;
}

bool CvTacticalPosition::addAssignment(const STacticalAssignment& newAssignment)
{
	//if we killed an enemy ZOC will change
	bool bRecomputeAllMoves = false;
	//newly visible plots, newly visible enemies
	std::pair<int, int> visibilityResult(0, 0);
	
	vector<SUnitStats>::iterator itUnit = find_if(availableUnits.begin(), availableUnits.end(), PrMatchingUnit(newAssignment.iUnitID));
	if (itUnit == availableUnits.end() || itUnit->iPlotIndex != newAssignment.iFromPlotIndex)
		return false;

	//tactical plots are only touched for "real" moves. blocked units may be on invalid plots.
	//a unit may also start out on an invalid plot (eg. too far away)
	if (newAssignment.eAssignmentType != A_BLOCKED && itUnit->eLastAssignment != A_INITIAL)
	{
		if (!getTactPlotMutable(newAssignment.iToPlotIndex).isValid())
			return false;
		if (!getTactPlotMutable(newAssignment.iFromPlotIndex).isValid())
			return false;
	}

	//i know what you did last summer!
	itUnit->eLastAssignment = newAssignment.eAssignmentType;

	//store the assignment
	assignedMoves.push_back(newAssignment);

	//now deal with the consequences
	bool bAffectsScore = true;
	bool bEndOfSim = false;
	switch (newAssignment.eAssignmentType)
	{
	case A_INITIAL:
		getTactPlotMutable(newAssignment.iToPlotIndex).friendlyUnitMovingIn(*this, newAssignment);
		bAffectsScore = false;
		break;
	case A_MOVE_FORCED:
	case A_MOVE_SWAP_REVERSE:
		bAffectsScore = false;
		//fall through!
	case A_MOVE:
	case A_MOVE_SWAP:
	case A_CAPTURE:
	{
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		itUnit->iPlotIndex = newAssignment.iToPlotIndex;

		//do the visibility update first so all newly neighboring plots become part of the sim
		visibilityResult = doVisibilityUpdate(newAssignment);

		//now do the accounting for the neighbor plots
		getTactPlotMutable(newAssignment.iFromPlotIndex).friendlyUnitMovingOut(*this, newAssignment);
		getTactPlotMutable(newAssignment.iToPlotIndex).friendlyUnitMovingIn(*this, newAssignment);

		//in case this was a move which revealed new enemies, pretend it was a forced move so we can move again
		if (visibilityResult.second > 0 && newAssignment.eAssignmentType == A_MOVE)
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
			enemyPlots.erase(std::remove(enemyPlots.begin(), enemyPlots.end(), newAssignment.iToPlotIndex), enemyPlots.end());
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

			//do the visibility update first so all newly neighboring plots become part of the sim
			//do this before the distance update
			visibilityResult = doVisibilityUpdate(newAssignment);
			
			//now the accounting
			getTactPlotMutable(newAssignment.iFromPlotIndex).friendlyUnitMovingOut(*this, newAssignment); //this is because we're advancing
			getTactPlotMutable(newAssignment.iToPlotIndex).friendlyUnitMovingIn(*this, newAssignment); //this implicitly removes the enemyUnit flag
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
			//put an invalid unit ID as a placeholder so that isEarlyFinish() works
			killedEnemies.push_back(0);
		}

		freedPlots.push_back(newAssignment.iToPlotIndex);
		enemyPlots.erase(std::remove(enemyPlots.begin(), enemyPlots.end(), newAssignment.iToPlotIndex), enemyPlots.end());

		//important that we do the visibility update first!
		refreshVolatilePlotProperties();
		bRecomputeAllMoves = true; //ZOC changed
		break;
	}
	case A_PILLAGE:
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		if (TacticalAIHelpers::IsOtherPlayerCitadel( GC.getMap().plotByIndexUnchecked(newAssignment.iToPlotIndex), getPlayer(), true))
			refreshVolatilePlotProperties();
		break;
	case A_USE_POWER:
		itUnit->iMovesLeft = newAssignment.iRemainingMoves;
		bEndOfSim = true;
		break;
	case A_FINISH:
		OutputDebugString("this should not happen\n");
	case A_FINISH_TEMP:
	case A_BLOCKED:
		bAffectsScore = false;
		bEndOfSim = true;
		break;
	default:
		UNREACHABLE();
	}

	//we don't update the moves plots lazily because it takes a while and we don't know yet if we will ever follow up on this position
	if (bRecomputeAllMoves)
	{
		movePlotUpdateFlag = -1; //need to update all
	}
	else if (itUnit->iMovesLeft>0 && !bEndOfSim)
	{
		//make sure we don't regress to a "lower" level
		if (movePlotUpdateFlag==0) 
			movePlotUpdateFlag = itUnit->iUnitID; //need to update only this one
		else if (movePlotUpdateFlag>0)
			movePlotUpdateFlag = -1; //need to update multiple units, simply do all
	}

	//forced moves don't even affect the score
	if (bAffectsScore)
	{
		iScoreOverParent += newAssignment.iScore;
		iTotalScore += newAssignment.iScore;

		//when in doubt, increasing our visibility is good
		iScoreOverParent += visibilityResult.first;
		iTotalScore += visibilityResult.first;
	}

	//are we done or can we do further moves with this unit?
	if (itUnit->iMovesLeft == 0 || bEndOfSim)
	{
		//blocked units might still move away
		if (IsCombatUnit(*itUnit) && newAssignment.eAssignmentType != A_BLOCKED)
			getTactPlotMutable(itUnit->iPlotIndex).setCombatUnitEndTurn(*this,DomainForUnit(itUnit->pUnit));
		notQuiteFinishedUnits.push_back(*itUnit);
		availableUnits.erase(itUnit);
	}

	//todo: should we stop the simulation? how to include this in position scoring?
	//don't do restarts if we have a lot of units, the simulation can take very long then
	//also don't do a restart if this was the last unit
	//any new enemies in sight or borders changed?
	bool bCityCapture = (newAssignment.eAssignmentType == A_MELEEKILL && GC.getMap().plotByIndexUnchecked(newAssignment.iToPlotIndex)->isCity());
	bool bRestartRequired = (visibilityResult.second > 0) || bCityCapture;
	if (bRestartRequired && availableUnits.size()>0)
		assignedMoves.push_back(STacticalAssignment( -1, -1, newAssignment.iUnitID, 0, newAssignment.eMoveType, 0, A_RESTART));

	return true;
}

bool STacticalAssignment::operator==(const STacticalAssignment& rhs) const
{
	return memcmp(this, &rhs, sizeof(STacticalAssignment)) == 0;
}

//try to detect simple permutations in the unit assigments which result in equivalent results
const CvTacticalPosition* tacticalPositionIsEquivalent(const CvTacticalPosition* ref, const CvTacticalPosition* other)
{
	//self comparison is false by definition!
	if (ref == other)
		return NULL;

	//"ref" is the new position we are evaluating. the "other" may already have finish moves tacked on, so size may be larger!
	if (ref->getAssignments().size() > other->getAssignments().size())
		return NULL;

	//now check the scores (ignoring any extra moves in other)
	int iRefScore = 0;
	int iOtherScore = 0;
	for (size_t i = ref->getFirstInterestingAssignment(); i < ref->assignedMoves.size(); i++)
	{
		iRefScore += ref->assignedMoves[i].iScore;
		iOtherScore += other->assignedMoves[i].iScore;
	}
	if (iRefScore != iOtherScore)
		return NULL;

	//now check for simple (A.B -> B.A) and less simple (A.B.C -> C.A.B | B.C.A), (A.B.C.D -> D.A.B.C | C.D.A.B | B.C.D.A ) permutations
	size_t A = INT_MAX;
	size_t B = INT_MAX;
	size_t C = INT_MAX;
	size_t D = INT_MAX;
	bool mismatch = false;
	//the "other" may have more moves assigned but they should all be of type FINISH ...
	//for performance do the iteration in reverse; we expect the differences at the end
	for (size_t i = ref->assignedMoves.size()-1; i >= ref->getFirstInterestingAssignment(); i--)
	{
		//ignore matching elements
		if (ref->assignedMoves[i] == other->assignedMoves[i])
			continue;

		//remember where the differences occurred
		if (A == INT_MAX)
			A = i;
		else if (B == INT_MAX)
			B = i;
		else if (C == INT_MAX)
			C = i;
		else if (D == INT_MAX)
			D = i;

		//if we found two differences, check if the elements are flipped
		if (A != INT_MAX && B != INT_MAX)
		{
			//simple flip?
			if (C == INT_MAX)
			{
				if (ref->assignedMoves[A] == other->assignedMoves[B] && ref->assignedMoves[B] == other->assignedMoves[A])
				{
					//go on checking
					A = INT_MAX;
					B = INT_MAX;
				}
				else
				{
					//check for a three-element permutation before giving up
				}
			}
			else //C != INT_MAX
			{
				if (D == INT_MAX)
				{
					bool CAB = ref->assignedMoves[A] == other->assignedMoves[C] &&
						ref->assignedMoves[B] == other->assignedMoves[A] &&
						ref->assignedMoves[C] == other->assignedMoves[B];
					bool BCA = ref->assignedMoves[A] == other->assignedMoves[B] &&
						ref->assignedMoves[B] == other->assignedMoves[C] &&
						ref->assignedMoves[C] == other->assignedMoves[A];

					if (CAB || BCA)
					{
						//go on checking
						A = INT_MAX;
						B = INT_MAX;
						C = INT_MAX;
					}
					else
					{
						//check for a four-element permutation before giving up
					}
				}
				else //D != INT_MAX
				{
					bool DABC = ref->assignedMoves[A] == other->assignedMoves[D] &&
						ref->assignedMoves[B] == other->assignedMoves[A] &&
						ref->assignedMoves[C] == other->assignedMoves[B] &&
						ref->assignedMoves[D] == other->assignedMoves[C];
					bool CDAB = ref->assignedMoves[A] == other->assignedMoves[C] &&
						ref->assignedMoves[B] == other->assignedMoves[D] &&
						ref->assignedMoves[C] == other->assignedMoves[A] &&
						ref->assignedMoves[D] == other->assignedMoves[B];
					bool BCDA = ref->assignedMoves[A] == other->assignedMoves[B] &&
						ref->assignedMoves[B] == other->assignedMoves[C] &&
						ref->assignedMoves[C] == other->assignedMoves[D] &&
						ref->assignedMoves[D] == other->assignedMoves[A];

					if (DABC || CDAB || BCDA)
					{
						//go on checking
						A = INT_MAX;
						B = INT_MAX;
						C = INT_MAX;
						D = INT_MAX;
					}
					else
					{
						//real difference or more complex permutations
						mismatch = true;
						break;
					}
				}
			}
		}
	}

	//gotcha - there might be an "unfinished" mismatch!
	if (A != INT_MAX)
		mismatch = true;

	if (mismatch)
	{
		giDifferentPos++;
		return NULL;
	}
	else
	{
		giEquivalentPos++;
		return other;
	}
}

//try to detect whether this new position is equivalent to one we already have
const CvTacticalPosition* tacticalPositionIsEquivalentToAnyChild(const CvTacticalPosition* ref, const CvTacticalPosition* current)
{
	//go depth first
	const vector<CvTacticalPosition*>& children = current->getChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		const CvTacticalPosition* match = tacticalPositionIsEquivalentToAnyChild(ref, children[i]);
		if (match)
			return match;
	}

	return tacticalPositionIsEquivalent(ref, current);
}

bool CvTacticalPosition::isUnique(int levels) const
{
	//go up x levels
	const CvTacticalPosition* start = this;
	while (start->parentPosition && levels > 0)
	{
		start = start->parentPosition;
		levels--;
	}

	//then recurse downwards to all leaves
	const CvTacticalPosition* match = tacticalPositionIsEquivalentToAnyChild(this, start);
	return (match == NULL);
}

void CvTacticalPosition::setFirstInterestingAssignment(size_t i)
{
	nFirstInterestingAssignment = (unsigned char)i;
}

size_t CvTacticalPosition::getFirstInterestingAssignment() const
{
	return (size_t)nFirstInterestingAssignment;
}

struct PairCompareFirst
{
    bool operator() (const std::pair<int,size_t>& l, const std::pair<int,size_t>& r) const { return l.first < r.first; }
    bool operator() (const std::pair<int,int>& l, const std::pair<int,int>& r) const { return l.first < r.first; }
};

struct EqualRangeComparison
{
    bool operator() ( const pair<int,size_t> a, int b ) const { return a.first < b; }
    bool operator() ( int a, const pair<int,size_t> b ) const { return a < b.first; }
    bool operator() ( const pair<int,int> a, int b ) const { return a.first < b; }
    bool operator() ( int a, const pair<int,int> b ) const { return a < b.first; }
};

vector<CvTacticalPlot>::iterator CvTacticalPosition::findTactPlot(int iPlotIndex)
{
	typedef pair<vector<pair<int, size_t>>::iterator, vector<pair<int, size_t>>::iterator>  IteratorPair;
	IteratorPair it2 = equal_range(tactPlotLookup.begin(), tactPlotLookup.end(), iPlotIndex, EqualRangeComparison());
	if (it2.first != tactPlotLookup.end() && it2.first != it2.second)
		return tactPlots.begin() + it2.first->second;

	return tactPlots.end();
}

vector<CvTacticalPlot>::const_iterator CvTacticalPosition::findTactPlot(int iPlotIndex) const
{
	typedef pair<vector<pair<int, size_t>>::const_iterator, vector<pair<int, size_t>>::const_iterator>  IteratorPair;
	IteratorPair it2 = equal_range(tactPlotLookup.begin(), tactPlotLookup.end(), iPlotIndex, EqualRangeComparison());
	if (it2.first != tactPlotLookup.end() && it2.first != it2.second)
		return tactPlots.begin() + it2.first->second;

	return tactPlots.end();
}

bool CvTacticalPosition::findTactPlotRecursive(int iPlotIndex) const
{
	if (findTactPlot(iPlotIndex) != tactPlots.end())
		return true;

	if (parentPosition && parentPosition->findTactPlotRecursive(iPlotIndex))
		return true;

	return false;
}

bool CvTacticalPosition::addTacticalPlot(const CvPlot* pPlot, const vector<CvUnit*>& allOurUnits)
{
	//don't check the official visibility here, we might want to create a tactplot that only became visible during simulation
	if (!pPlot)
		return false;

	if (findTactPlotRecursive(pPlot->GetPlotIndex()))
		return false; //nothing to do

	CvTacticalPlot newPlot(pPlot, ePlayer, allOurUnits);
	if (newPlot.isValid())
	{
		pair<int, size_t> newEntry(pPlot->GetPlotIndex(), tactPlots.size());
		tactPlotLookup.insert(upper_bound(tactPlotLookup.begin(), tactPlotLookup.end(), newEntry, PairCompareFirst()), newEntry);
		tactPlots.push_back(newPlot);

#if defined(MOD_CORE_DEBUGGING)
		if (GC.getLogging() && GC.getAILogging() && MOD_CORE_DEBUGGING && iID==1) //log only initial position
		{
			CvString strMsg;
			strMsg.Format("added sim plot (%d:%d), %s, %s",
				pPlot->getX(), pPlot->getY(),
				newPlot.isEnemy() ? "enemy" : (newPlot.isBlockedByNonSimUnit(CvTacticalPlot::TD_BOTH) ? "blocked" : "available"),
				newPlot.isVisibleToEnemy() ? "enemy_can_see" : "enemy_cannot_see"
			);
			GET_PLAYER(ePlayer).GetTacticalAI()->LogTacticalMessage(strMsg);
		}
#endif

		return true;
	}

	return false;
}

bool CvTacticalPosition::addAvailableUnit(const CvUnit* pUnit)
{
	if (!pUnit || !pUnit->canMove() || !pUnit->canEndTurnAtPlot(pUnit->plot()))
		return false;

	eUnitMovementStrategy eStrategy = MS_NONE;

	//ok this is a bit involved
	//case a) we want to fight. units should stay in their native domain so they can fight.
	//case b) we don't want to fight. units may embark if the target is not their native domain
	//later in updateMoveAndAttackPlotsForUnits we try and filter the reachable plots according to unit strategy
	//also, only land units can embark and but melee ships can move into certain land plots (cities) so it's tricky

	//if were not looking to fight but about to embark then keep the unit away from enemies
	if (eAggression == AL_NONE && !pUnit->isNativeDomain(pTargetPlot) && pUnit->CanEverEmbark())
	{
		if (pUnit->IsCombatUnit())
			eStrategy = MS_EMBARKED;
		else
			eStrategy = MS_SUPPORT; //civilians can stack!
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
		case UNITAI_SKIRMISHER:
		case UNITAI_SUBMARINE:
			if (pUnit->GetRange() > 2 || (MOD_AI_UNIT_PRODUCTION && pUnit->canIntercept())) // MOD_AI_UNIT_PRODUCTION : AA to back
				eStrategy = MS_THIRDLINE;
			else if (pUnit->GetRange() == 2)
				eStrategy = MS_SECONDLINE;
			else
			{
				//the unit AI type is unreliable, so we do this manually
				if (pUnit->IsCanAttackRanged())
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

	//pull back our melee units if they have soaked enough damage
	if (eStrategy == MS_FIRSTLINE && pUnit->isProjectedToDieNextTurn())
		eStrategy = MS_THIRDLINE;

	//we will update the importance later, use 0 for now
	availableUnits.push_back( SUnitStats( pUnit, 0, eStrategy ) );

	movePlotUpdateFlag = -1; //lazy update of move plots later

#if defined(MOD_CORE_DEBUGGING)
	if (GC.getLogging() && GC.getAILogging() && MOD_CORE_DEBUGGING)
	{
		CvString strMsg;
		strMsg.Format("added sim unit %s, id %d, at (%d:%d), moves %d, damage %d, pathfinder count %d",
			pUnit->getName().c_str(),
			pUnit->GetID(),
			pUnit->getX(),
			pUnit->getY(),
			pUnit->getMoves(),
			pUnit->getDamage(),
			GC.GetPathFinder().GetCurrentGenerationID()
		);
		GET_PLAYER(ePlayer).GetTacticalAI()->LogTacticalMessage(strMsg);
	}
#endif

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
	float fUnitNumberRatio = sqrtf(nOurOriginalUnits / float(max(1,(int)nOriginalEnemies)));

	int iFlavorOffense = GET_PLAYER(ePlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	if (iFlavorOffense > 6)
		fUnitNumberRatio += 0.1f;

	return max( 0.9f, fUnitNumberRatio ); //<1 indicates we're fewer but don't stop attacking because of that
}

bool CvTacticalPosition::couldEndTurnAfterThisAssignment(const STacticalAssignment& assignment) const
{
	int iEndPlotIndex = -1;

	switch (assignment.eAssignmentType)
	{
	case A_MOVE:
	case A_MELEEKILL:
	case A_CAPTURE:
		//unit moves
		iEndPlotIndex = assignment.iToPlotIndex;
		break;
	case A_MELEEATTACK:
	case A_RANGEATTACK:
	case A_RANGEKILL:
	case A_PILLAGE:
	case A_FINISH_TEMP:
	case A_MELEEKILL_NO_ADVANCE:
		//unit stays in place
		iEndPlotIndex = assignment.iFromPlotIndex;
		break;
	case A_BLOCKED:	//not our problem ... fingers crossed
	case A_USE_POWER: //consumes the unit!
		return true;
	default:
		OutputDebugString("unexpected assignment type ...\n");
		return false;
	}

	const SUnitStats* unit = getAvailableUnitStats(assignment.iUnitID);
	const CvTacticalPlot& assumedUnitPlot = getTactPlot(iEndPlotIndex);
	CvUnit* pUnit = GET_PLAYER(getPlayer()).getUnit(assignment.iUnitID);
	if (!pUnit || !assumedUnitPlot.isValid())
		return false;

	return ScoreTurnEnd(pUnit, unit->eLastAssignment, assumedUnitPlot, 0, CvTacticalPlot::TD_BOTH, unit->iSelfDamage, *this, EM_FINAL) != INT_MAX;
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
	if (isEarlyFinish() || isExhausted())
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
		out << "#x,y,terrain,owner,isEnemy,isFriendly,nAdjEnemy,nAdjFriendly,nAdjFirstline,isEdge,hasSupport,iEnemyDist\n"; 
		for (vector<CvTacticalPlot>::const_iterator it = tactPlots.begin(); it != tactPlots.end(); ++it)
		{
			CvPlot* pPlot =  GC.getMap().plotByIndexUnchecked( it->getPlotIndex() );
			out << pPlot->getX() << "," << pPlot->getY() << "," << pPlot->getTerrainType() << "," << pPlot->getOwner() << "," << (it->isEnemy() ? 1 : 0) << "," << (it->hasFriendlyCombatUnit() ? 1 : 0) << "," 
				<< it->getNumAdjacentEnemies(CvTacticalPlot::TD_BOTH) << "," << it->getNumAdjacentFriendlies(CvTacticalPlot::TD_BOTH,-1) << "," << it->getNumAdjacentFriendliesEndTurn(CvTacticalPlot::TD_BOTH) << "," 
				<< (it->isEdgePlot() ? 1 : 0) << ","<< (it->hasSupportBonus(-1) ? 1 : 0) << "," << (int)(it->getEnemyDistance()) << "\n";
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
	vector<CvTacticalPlot>::iterator it = findTactPlot(plotindex);
	if (it != tactPlots.end())
		return *it;

	if (parentPosition)
	{
		const CvTacticalPlot& parentResult = parentPosition->getTactPlot(plotindex);
		if (parentResult.isValid())
		{
			//now cache it locally so that we can modify it
			tactPlotLookup.push_back( make_pair(plotindex, tactPlots.size()) );
			std::stable_sort(tactPlotLookup.begin(), tactPlotLookup.end(), PairCompareFirst() );
			//this is dangerous, may invalidate references if the vector is reallocated
			//we should really be storing pointers to plots, not the plots themselves ...
			tactPlots.push_back(parentResult);
			return tactPlots.back();
		}
	}

	return dummyPlot;
}

//make sure we have all tactical plots in our local cache
void CvTacticalPosition::cacheAllTactPlotsLocally()
{
	const CvTacticalPosition* current = parentPosition;
	while (current != NULL)
	{
		for (vector<CvTacticalPlot>::const_iterator it = current->tactPlots.begin(); it != current->tactPlots.end(); ++it)
		{
			int iIndex = it->getPlotIndex();
			if (findTactPlot(iIndex) == tactPlots.end())
			{
				pair<int, size_t> newEntry(iIndex, tactPlots.size());
				tactPlotLookup.insert(upper_bound(tactPlotLookup.begin(), tactPlotLookup.end(), newEntry, PairCompareFirst()), newEntry);
				tactPlots.push_back(*it);
			}
		}

		current = current->parentPosition;
	}
}

const CvTacticalPlot& CvTacticalPosition::getTactPlot(int plotindex) const
{
	vector<CvTacticalPlot>::const_iterator it = findTactPlot(plotindex);
	if (it != tactPlots.end())
		return *it;

	if (parentPosition)
		return parentPosition->getTactPlot(plotindex);

	//no caching? unclear what's better
	return dummyPlot;
}

//try to find a combination of unit actions (move, attack etc) which does maximal damage to the enemy while exposing us to minimal risk
vector<STacticalAssignment> TacticalAIHelpers::FindBestUnitAssignments(
	const vector<CvUnit*>& vUnits, CvPlot* pTarget, eAggressionLevel eAggLvl, CvTactPosStorage& storage)
{
	/*
	abstract:

	----
	create tactical plots
	add all units with reachable plots
 
	create open position
	while (pop open positions)
	 if (make next assignments)
	  add children to open positions
	 else if (completed and legal)
	   add to completed positions

	return best completed position
	----

	units are position according to their attack range (melee has range 1 for this purpose)
	distance to enemy is main criterion, danger is secondary
	ranged attack are always possible; melee attacks are accepted or not depending on aggression level
	final confirmation whether an assignement is acceptable happens only at the end

	if aggression level is zero, we do not plan any attacks. only movement.
		if target is friendly, we try to stay within N plots around it with melee units covering ranged units.
		if target is hostile, we try to come close but no closer than N plots with melee units covering ranged units.
	*/

	vector<STacticalAssignment> result;
	if (vUnits.empty() || vUnits.front()==NULL || pTarget==NULL)
		return result;

	//meta parameters depending on difficulty setting
	int iMaxBranches = range(GC.getGame().getHandicapInfo().getTacticalSimMaxBranches(),2,9); //cannot do more, else our ID scheme doesn't work
	int iMaxChoicesPerUnit = range(GC.getGame().getHandicapInfo().getTacticalSimMaxChoicesPerUnit(),2,9);
	int iMaxCompletedPositions = range(GC.getGame().getHandicapInfo().getTacticalSimMaxCompletedPositions(), 1, 4000);

	PlayerTypes ePlayer = vUnits.front()->getOwner();
	TeamTypes ourTeam = GET_PLAYER(ePlayer).getTeam();

	static vector<CvTacticalPosition*> openPositionsHeap;
	static vector<CvTacticalPosition*> completedPositions;

#if defined(MOD_CORE_DEBUGGING)
	if (false)
	{
		CvString strMsg = CvString::format("simulating assignments around %d:%d with %d units", pTarget->getX(), pTarget->getY(), vUnits.size());
		for (size_t i = 0; i < vUnits.size(); i++)
			strMsg += CvString::format("; %d", vUnits[i]->GetID());
		GET_PLAYER(ePlayer).GetTacticalAI()->LogTacticalMessage(strMsg);

#if defined(STACKWALKER)
		gStackWalker.SetLog(GET_PLAYER(ePlayer).GetTacticalAI()->GetLogFile());
		gStackWalker.ShowCallstack(5);
		gStackWalker.SetLog(NULL);
#endif
	}
#endif

	cvStopWatch timer("tactsim",NULL,0,true);
	timer.StartPerfTest();

	//clean up from the last run
	storage.reset(eLastTactSimPlayer!=ePlayer);
	eLastTactSimPlayer = ePlayer;
	gReachablePlotsLookup.clear();
	gRangeAttackPlotsLookup.clear();
	
	//set up the initial position
	CvTacticalPosition* initialPosition = storage.peekNext(); storage.consumeOne();
	if (!initialPosition)
		return result;
	initialPosition->initFromScratch(ePlayer, eAggLvl, pTarget);

	//first pass: make sure there are no duplicates and other invalid inputs
	vector<CvUnit*> ourUnits;
	for (size_t i = 0; i < vUnits.size(); i++)
	{
		CvUnit* pUnit = vUnits[i];

		//do not use a set for enforcing uniqueness - the iteration order would depend on memory address by default
		//unfortunately the simulation result sometimes seems to depend on the order of the units being processed ...
		if (std::find(ourUnits.begin(), ourUnits.end(), pUnit) != ourUnits.end())
			continue;

		//units outside of their native domain are a problem because they violate 1UPT. 
		//we accept them only if they are alone in the plot and only allow movement into the native domain.
		//exception: since we ignore garrisoned units for tactsim, we can still use units (ships) in cities if a (land) garrison is present
		if (pUnit && pUnit->canUseNow())
			if (pUnit->isNativeDomain(pUnit->plot()) || pUnit->plot()->getNumUnits()==1 || pUnit->plot()->isCity())
				ourUnits.push_back(vUnits[i]);
	}

	if (ourUnits.empty())
		return result;

	//create the tactical plots around the target (up to distance 5)
	//not equivalent to the union of all reachable plots: we need to consider unreachable enemies as well!
	//some units may have their initial plots outside of this range but that's ok, we'll fix it later
	for (int i = 0; i < RING_PLOTS[TACTICAL_COMBAT_MAX_TARGET_DISTANCE + 1]; i++)
	{
		CvPlot* pPlot = iterateRingPlots(pTarget, i);
		if (pPlot && pPlot->isVisible(ourTeam))
			initialPosition->addTacticalPlot(pPlot, ourUnits);
	}

	//second pass, now that we know which units will be used, add them to the initial position
	for(vector<CvUnit*>::iterator it=ourUnits.begin(); it!=ourUnits.end(); ++it)
	{
		CvUnit* pUnit = *it;
		if (initialPosition->addAvailableUnit(pUnit))
		{
			//make sure we know the immediate surroundings of every unit
			for (int j = 0; j < RING2_PLOTS; j++)
			{
				CvPlot* pPlot = iterateRingPlots(pUnit->plot(), j);
				if (!pPlot)
					continue;
				//there was a strange bug where the plot containing the unit or adjacent plots are not visible? wtf
				if (j<RING1_PLOTS || pPlot->isVisible(ourTeam))
					initialPosition->addTacticalPlot(pPlot, ourUnits);
			}
		}
	}

	//find out which plot is frontline, second line etc
	initialPosition->refreshVolatilePlotProperties();

	//now associate our units with their initial plots (after we know the plot types)
	initialPosition->addInitialAssignments();

	//number of enemies influences how aggressive we can be
	//note that for defensive positioning we do not require any enemies to be nearby
	initialPosition->countEnemiesAndCheckVisibility();

	//small performance optimization
	initialPosition->setFirstInterestingAssignment(initialPosition->getAssignments().size());

	//around 15 units everythings becomes slow so don't use more
	initialPosition->dropSuperfluousUnits(15);

	openPositionsHeap.clear();
	completedPositions.clear();
	size_t iUsedPositions = 0;

	//don't need to call make_heap for a single element
	openPositionsHeap.push_back(initialPosition);

	//initially we go breadth-first, later switch to depth-first
	CvTacticalPosition::PrPositionSortHeapGeneration heapSort(false);

	while (!openPositionsHeap.empty())
	{
		pop_heap( openPositionsHeap.begin(), openPositionsHeap.end(), heapSort);
		CvTacticalPosition* current = openPositionsHeap.back();

		//switch our strategy?
		if (!heapSort.bDepthFirst && current->getGeneration() > TACTSIM_BREADTH_FIRST_GENERATIONS)
		{
			heapSort.bDepthFirst = true;
			make_heap(openPositionsHeap.begin(), openPositionsHeap.end(), heapSort);
			//pick again after re-sorting
			continue;
		}
		else
			//go on with the selected position, remove it from the heap
			openPositionsHeap.pop_back();

		//just pick the "best" move in depth first mode
		int iMaxBranchesNow = heapSort.bDepthFirst ? 1 : iMaxBranches;
		//allow two moves per unit in case one is invalid
		int iMaxChoicesPerUnitNow = heapSort.bDepthFirst ? 2 : iMaxChoicesPerUnit;

		//here the magic happens!
		current->makeNextAssignments(iMaxBranchesNow, iMaxChoicesPerUnitNow, storage, openPositionsHeap, completedPositions, heapSort);

		int iOldPauseCount = iUsedPositions / 500;
		iUsedPositions += current->getChildren().size();
		int iNewPauseCount = iUsedPositions / 500;

		//at some point we have seen enough good positions to pick one
		if (completedPositions.size() > (size_t)iMaxCompletedPositions)
			break;

		//be a good citizen and let the UI run in between ... stupid design
		if (iOldPauseCount!=iNewPauseCount && gDLL->HasGameCoreLock())
		{
			gDLL->ReleaseGameCoreLock();
			Sleep(1);
			gDLL->GetGameCoreLock();
		}

		//did we run out of resources?
		//this typically happens if there are only invalid positions to be found ...
		if (storage.peekNext() == NULL)
		{
			timer.EndPerfTest();
			int iStartingUnits = initialPosition->getAvailableUnits().size();

			std::stringstream ss;
			ss << "warning: tactsim abandoned after, " << std::setprecision(3) << timer.GetDeltaInSeconds() << " s, " <<
				completedPositions.size() << " completed, " <<
				openPositionsHeap.size() << " open, " <<
				iUsedPositions << " processed, " <<
				iStartingUnits << " starting units, " <<
				current->getAvailableUnits().size() << " remaining units";
				CUSTOMLOG(ss.str().c_str());

			//too risky ...
			/*
			//see if we can just take an unfinished position to salvage the situation
			if (completedPositions.empty())
			{
				std::stable_sort(openPositionsHeap.begin(), openPositionsHeap.end(), CvTacticalPosition::PrPositionSortArrayTotalScore());
				for (size_t i = 0; i < openPositionsHeap.size(); i++)
				{
					if (openPositionsHeap[i]->addFinishMovesIfAcceptable(false))
						completedPositions.push_back(openPositionsHeap[i]);
				}

				//want at least 3, don't trust the results if we have less
				if (completedPositions.size() < 3)
					completedPositions.clear();
			}
			*/
			break;
		}
	}

	timer.EndPerfTest();

	//normally the score for a completed position should be higher than for a finished one
	//but if there are too many unassigned units it can tip
	if (!completedPositions.empty())
	{
		//need the predicate, else we sort the pointers by address!
		std::stable_sort(completedPositions.begin(), completedPositions.end(), CvTacticalPosition::PrPositionSortArrayTotalScore());
		result = completedPositions.front()->getAssignments();
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		if (true)
		{
			GET_PLAYER(ePlayer).GetTacticalAI()->LogTacticalMessage(CvString::format("tactsim around (%d:%d) finished in %d ms. started with %d units and %d enemies on %d plots. checked %d positions, %d completed.",
				pTarget->getX(),pTarget->getY(),int(timer.GetDeltaInSeconds()*1000), initialPosition->getAvailableUnits().size(), initialPosition->getNumEnemies(), initialPosition->getNumPlots(), iUsedPositions, completedPositions.size()));
		}

		//debug dump
#if defined(MOD_CORE_DEBUGGING)
		if (gCurrentUnitToTrack == -1)
		{
			ofstream out("c:\\temp\\positionscores.csv", std::ios::app);
			if (out)
			{
				for (int i = 0; i < storage.getSize(); i++)
				{
					CvTacticalPosition* pos = storage.first() + i;
					bool isComplete = pos->isEarlyFinish() || pos->isExhausted();
					out << pos->getID() << "," << pos->getScoreLastRound() << "," << pos->getScoreTotal() << "," << (isComplete ? 1:0) << ";";
				}
				out << std::endl;
			}
			out.close();
		}
#endif
	}

	return result;
}

bool TacticalAIHelpers::ExecuteUnitAssignments(PlayerTypes ePlayer, const std::vector<STacticalAssignment>& vAssignments)
{
	static const BuildTypes eCitadel = (BuildTypes)GC.getInfoTypeForString("BUILD_CITADEL");
	static const BuildTypes eOrdo = (BuildTypes)GC.getInfoTypeForString("BUILD_ORDO");

	//take the assigned moves one by one and try to execute them faithfully. 
	//may fail if a melee kill unexpectedly happens or does not happen

	for (size_t i = 0; i < vAssignments.size(); i++)
	{
		CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(vAssignments[i].iUnitID);
		//be extra careful with the unit here, if we capture cities and liberate them strange instakills can happen
		//so we need to guess whether the pointer is still valid
		if (!pUnit || pUnit->isDelayedDeath() || pUnit->plot()==NULL)
			continue;

		CvPlot* pFromPlot = GC.getMap().plotByIndexUnchecked(vAssignments[i].iFromPlotIndex);
		CvPlot* pToPlot = GC.getMap().plotByIndexUnchecked(vAssignments[i].iToPlotIndex);

		int iMoveflags = CvUnit::MOVEFLAG_IGNORE_DANGER | CvUnit::MOVEFLAG_NO_STOPNODES;
		bool bPrecondition = false;
		bool bPostcondition = false;

		switch (vAssignments[i].eAssignmentType)
		{
		case A_INITIAL:
		case A_FINISH_TEMP:
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
				pUnit->ClearPathCache(); 
				if (!pUnit->GeneratePath(pToPlot, iMoveflags) || pUnit->GetPathEndFirstTurnPlot() != pToPlot)
					OutputDebugString("ouch, pathfinding problem\n");
			}
#endif
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pToPlot->getX(), pToPlot->getY(), iMoveflags, false, false, MISSIONAI_OPMOVE);
			bPostcondition = (pUnit->plot() == pToPlot); //plot changed

#ifdef TACTDEBUG
			//check this only for moves, eg melee kills can fail this check because the pathfinder assumes attacks end the turn!
			if (vAssignments[i].iRemainingMoves != pUnit->getMoves() && bPostcondition)
				OutputDebugString("ouch, inconsistent movement points\n");
#endif
			break;
		case A_MOVE_SWAP:
			pUnit->ClearPathCache(); //make sure there's no stale path which coincides with our target
			bPrecondition = (pUnit->plot() == pFromPlot) && !(pToPlot->isEnemyUnit(ePlayer,true,true) || pToPlot->isEnemyCity(*pUnit)); //no enemy
			if (bPrecondition)
				pUnit->PushMission(CvTypes::getMISSION_SWAP_UNITS(), pToPlot->getX(), pToPlot->getY(), iMoveflags, false, false, MISSIONAI_OPMOVE);
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
				int iDamageDealt = 0;
				int iDamageReceived = 0;
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

			//because of randomness in previous combat results, it may happen that we cannot actually kill the enemy
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
		case A_USE_POWER:
			if (pUnit->canBuild(pUnit->plot(), eCitadel))
				pUnit->PushMission(CvTypes::getMISSION_BUILD(), eCitadel);
			else if (eOrdo != NO_BUILD && pUnit->canBuild(pUnit->plot(), eOrdo))
				pUnit->PushMission(CvTypes::getMISSION_BUILD(), eOrdo);
			else
				bPrecondition = false;
		case A_FINISH:
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			//this is the difference to a blocked unit, we prevent anyone else from moving it unless we want it to heal
			if (!pUnit->shouldHeal(false) || pUnit->getMoves()==0 || pUnit->isBarbarian()) //barbarians don't heal
				//important ... this allows civilian units to use this one as cover!
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
		//this can happen sometimes because of randomness or splash damage etc
		if (!bPrecondition || !bPostcondition)
		{
			//stringstream out;
			//out << "tactsim: could not execute " << assignmentTypeNames[ vAssignments[i].eAssignmentType ] << (bPrecondition?" (postcondition)":" (precondition)");
			//CUSTOMLOG(out.str().c_str());
			return false;
		}
#else
		if (!bPrecondition || !bPostcondition)
			return false;
#endif
	}

	return true;
}

void CvTactPosStorage::reset(bool bHard)
{ 
	//this is normally enough
	iCount = 0; attackCache.clear();

	//in a hard reset we recreate all stl containers from scratch
	//because their capacity tends to increase over time otherwise
	if (bHard)
	{
		//PrintMemoryInfo("before hard reset");

		for (int i = 0; i < iSize; i++)
			aPositions[i].wipe();

		//for some reason the memory usage is not affected immediately ... but the wiping works
		//PrintMemoryInfo("after hard reset");
	}
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
	"T_REINFORCE",
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
	"SWAPREVERSE",
	"USEPOWER",
	"FINISHTEMP"
};
