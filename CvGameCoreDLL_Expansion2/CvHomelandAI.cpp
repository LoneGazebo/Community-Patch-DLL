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
#include "CvHomelandAI.h"
#include "CvAStar.h"
#include "CvEconomicAI.h"
#include "CvWonderProductionAI.h"
#include "CvCitySpecializationAI.h"
#include "CvGrandStrategyAI.h"
#include "cvStopWatch.h"
#include "CvTypes.h"

#if defined(MOD_AI_SMART_AIR_TACTICS)
#include "CvMilitaryAI.h"
#endif

// must be included after all other headers
#include "LintFree.h"

CvHomelandUnit::CvHomelandUnit() :
	m_iID(0)
	, m_iAuxData(0)
	, m_iMovesToTarget(0)
	, m_pTarget(NULL)
{
}

//=====================================
// CvHomelandAI
//=====================================

/// Constructor
CvHomelandAI::CvHomelandAI(void)
{
}

/// Destructor
CvHomelandAI::~CvHomelandAI(void)
{
	Uninit();
}

/// Initialize
void CvHomelandAI::Init(CvPlayer* pPlayer)
{
	// Store off the pointer to the objects we need elsewhere in the game engine
	m_pPlayer = pPlayer;

	Reset();

	// Initialize AI constants from XML
	m_iRandomRange = GC.getAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS();
	m_iDefensiveMoveTurns = GC.getAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS();
	m_iUpgradeMoveTurns = GC.getAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS();
	m_fFlavorDampening = GC.getAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION();
}

/// Deallocate memory created in initialize
void CvHomelandAI::Uninit()
{
}

/// Reset variables
void CvHomelandAI::Reset()
{
	m_MovePriorityTurn = -1;
	m_CurrentBestMoveUnit = NULL;
	m_CurrentBestMoveHighPriorityUnit = NULL;
	m_iCurrentBestMoveUnitTurns = MAX_INT;
	m_iCurrentBestMoveHighPriorityUnitTurns = MAX_INT;
}

/// Serialization read
void CvHomelandAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_MovePriorityTurn;
}

/// Serialization write
void CvHomelandAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_MovePriorityTurn;
}

/// Mark all the units that will be under tactical AI control this turn
void CvHomelandAI::RecruitUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();

	// Loop through our units
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Never want immobile/dead units or ones that have already moved
		if(!pLoopUnit->TurnProcessed() && !pLoopUnit->isDelayedDeath() && pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN && pLoopUnit->canMove())
		{
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
	}
}

/// Mark all the units that will be under tactical AI control this turn
void CvHomelandAI::FindAutomatedUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();

	// Loop through our units
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if(pLoopUnit->IsAutomated() && !pLoopUnit->TurnProcessed() && pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN && pLoopUnit->canMove())
		{
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
	}
}

/// Set up for a turn of tactical moves
void CvHomelandAI::DoTurn()
{
}

/// Update the AI for units
void CvHomelandAI::Update()
{
	AI_PERF_FORMAT("AI-perf.csv", ("Homeland AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()));

	// Make sure we have a unit to handle
	if(!m_CurrentTurnUnits.empty())
	{
		// Make sure the economic plots are up-to-date, it has a caching system in it.
		CvEconomicAI* pEconomicAI = m_pPlayer->GetEconomicAI();
		pEconomicAI->UpdatePlots();

		// Start by establishing the priority order for moves this turn
		EstablishHomelandPriorities();

		// Put together lists of places we may want to move toward
		FindHomelandTargets();

		// Loop through each move assigning units when available
		AssignHomelandMoves();
	}
}

typedef std::pair<int, CvPlot*> DistancePlotEntry;
typedef FFastVector<DistancePlotEntry> DistanceSortedPlotArray;
//	----------------------------------------------------------------------------
//	Sort function for the distances
static bool SortUnitDistance(const DistancePlotEntry& kEntry1, const DistancePlotEntry& kEntry2)
{
	return kEntry1.first < kEntry2.first;
}

bool CvHomelandAI::IsAnyValidExploreMoves(const CvUnit* pUnit) const
{
	CvEconomicAI* pEconomicAI = m_pPlayer->GetEconomicAI();
	FFastVector<int>& aiExplorationPlots = pEconomicAI->GetExplorationPlots();
	FFastVector<int>& aiExplorationPlotRatings = pEconomicAI->GetExplorationPlotRatings();

	if (aiExplorationPlots.size() > 0)
	{
		int iUnitX = pUnit->getX();
		int iUnitY = pUnit->getY();

		// Filter the list with some quick checks, then add the rest to a list that we can sort by distance
		DistanceSortedPlotArray aDistanceList;
		aDistanceList.reserve( aiExplorationPlots.size() );

		for(uint ui = 0; ui < aiExplorationPlots.size(); ui++)
		{
			int iPlot = aiExplorationPlots[ui];
			if(iPlot < 0)
			{
				continue;
			}

			CvPlot* pEvalPlot = GC.getMap().plotByIndex(iPlot);
			if(!pEvalPlot)
			{
				continue;
			}

			if(aiExplorationPlotRatings[ui] == 0)
			{
				continue;
			}

			int iDistX = abs( pEvalPlot->getX() - iUnitX );
			int iDistY = abs( pEvalPlot->getY() - iUnitY );

			aDistanceList.push_back(std::pair<int, CvPlot*>((iDistX*iDistX)+(iDistY*iDistY), pEvalPlot));

		}

		if (aDistanceList.size())
		{
			std::sort(aDistanceList.begin(), aDistanceList.end(), SortUnitDistance);

			for (DistanceSortedPlotArray::const_iterator itr = aDistanceList.begin(); itr != aDistanceList.end(); ++itr)
			{
				CvPlot* pEvalPlot = (*itr).second;
				if(!IsValidExplorerEndTurnPlot(pUnit, pEvalPlot))
				{
					continue;
				}
				// hitting the path finder, may not be the best idea. . .
				bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit, iUnitX, iUnitY, pEvalPlot->getX(), pEvalPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if(!bCanFindPath)
				{
					continue;
				}

				return true;
			}
		}
	}


	return false;
}

// PRIVATE METHODS

/// Choose which moves to emphasize this turn
void CvHomelandAI::EstablishHomelandPriorities()
{
	int iPriority = 0;
	int iFlavorDefense = 0;
	int iFlavorOffense = 0;
	int iFlavorExpand = 0;
	int iFlavorImprove = 0;
	int iFlavorNavalImprove = 0;
	int iFlavorExplore = 0;
	int iFlavorGold = 0;
	int iFlavorScience = 0;
	int iFlavorWonder = 0;
	int iFlavorMilitaryTraining = 0;
	int iFlavorCulture = 0;

	m_MovePriorityList.clear();
	m_MovePriorityTurn = GC.getGame().getGameTurn();

#if defined(MOD_AI_SMART_UPGRADES)
	// On even turns upgrades acquire more priority in order to be able to upgrade air units.
	int iTurnUpgradePriority = MOD_AI_SMART_UPGRADES ? (m_MovePriorityTurn % 2) * 50 : 0;
#endif

	// Find required flavor values
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			iFlavorDefense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorDefense = (int)((double)iFlavorDefense * m_fFlavorDampening);
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			iFlavorOffense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorOffense = (int)((double)iFlavorOffense * m_fFlavorDampening);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iFlavorExpand = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_RECON")
		{
			iFlavorExplore = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorExplore = (int)((double)iFlavorExplore * m_fFlavorDampening);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GOLD")
		{
			iFlavorGold = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
		{
			iFlavorScience = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_WONDER")
		{
			iFlavorWonder = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
		{
			iFlavorMilitaryTraining = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CULTURE")
		{
			iFlavorCulture = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
	}

	// Loop through each possible homeland move (other than "none" or "unassigned")
	for(int iI = AI_HOMELAND_MOVE_UNASSIGNED + 1; iI < NUM_AI_HOMELAND_MOVES; iI++)
	{
		// Set base value
		switch((AIHomelandMove)iI)
		{
		case AI_HOMELAND_MOVE_EXPLORE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_EXPLORE();
			break;
		case AI_HOMELAND_MOVE_EXPLORE_SEA:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA();
			break;
		case AI_HOMELAND_MOVE_SETTLE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SETTLE();
			break;
		case AI_HOMELAND_MOVE_GARRISON:
			// Garrisons must beat out sentries if policies encourage garrisoning
			if(m_pPlayer->GetPlayerPolicies()->HasPolicyEncouragingGarrisons())
			{
				iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SENTRY() + 1;
			}
			else
			{
				iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GARRISON();
			}
			break;
		case AI_HOMELAND_MOVE_HEAL:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_HEAL();
			break;
		case AI_HOMELAND_MOVE_TO_SAFETY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY();
			break;
		case AI_HOMELAND_MOVE_MOBILE_RESERVE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE();
			break;
		case AI_HOMELAND_MOVE_SENTRY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SENTRY();
			break;
		case AI_HOMELAND_MOVE_WORKER:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_WORKER();
			break;
		case AI_HOMELAND_MOVE_WORKER_SEA:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA();
			break;
		case AI_HOMELAND_MOVE_PATROL:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_PATROL();
			break;
		case AI_HOMELAND_MOVE_UPGRADE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_UPGRADE();
			break;
		case AI_HOMELAND_MOVE_ANCIENT_RUINS:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS();
			break;
		case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE();
			break;
		case AI_HOMELAND_MOVE_WRITER:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_WRITER();
			break;
		case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ARTIST();
			break;
		case AI_HOMELAND_MOVE_MUSICIAN:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_MUSICIAN();
			break;
		case AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH();
			break;
		case AI_HOMELAND_MOVE_ENGINEER_HURRY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY();
			break;
#if defined(MOD_DIPLOMACY_CITYSTATES)
		case AI_HOMELAND_MOVE_DIPLOMAT_EMBASSY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_DIPLOMAT();
			break;
		case AI_HOMELAND_MOVE_MESSENGER:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_MESSENGER();
			break;
#endif
		case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON();
			break;
		case AI_HOMELAND_MOVE_ADMIRAL_GARRISON:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON();
			break;
		case AI_HOMELAND_MOVE_PROPHET_RELIGION:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION();
			break;
		case AI_HOMELAND_MOVE_MISSIONARY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_MISSIONARY();
			break;
		case AI_HOMELAND_MOVE_INQUISITOR:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_INQUISITOR();
			break;
		case AI_HOMELAND_MOVE_SPACESHIP_PART:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART();
			break;
		case AI_HOMELAND_MOVE_ADD_SPACESHIP_PART:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART();
			break;
		case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT();
			break;
		case AI_HOMELAND_MOVE_TREASURE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_TREASURE();
			break;
		case AI_HOMELAND_MOVE_TRADE_UNIT:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT();
			break;
		case AI_HOMELAND_MOVE_ARCHAEOLOGIST:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST();
			break;
		case AI_HOMELAND_MOVE_AIRLIFT:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_AIRLIFT();
			break;
		}

		// Make sure base priority is not negative
		if(iPriority >= 0)
		{
			// Now add in the appropriate flavor value
			switch((AIHomelandMove)iI)
			{
				// Defensive moves
			case AI_HOMELAND_MOVE_GARRISON:
			case AI_HOMELAND_MOVE_HEAL:
			case AI_HOMELAND_MOVE_TO_SAFETY:
			case AI_HOMELAND_MOVE_MOBILE_RESERVE:
			case AI_HOMELAND_MOVE_SENTRY:
			case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			case AI_HOMELAND_MOVE_ADMIRAL_GARRISON:
			case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			case AI_HOMELAND_MOVE_TREASURE:

				// Here so they remain same priority relative to AI_HOMELAND_MOVE_TO_SAFETY
			case AI_HOMELAND_MOVE_MISSIONARY:
			case AI_HOMELAND_MOVE_INQUISITOR:
			case AI_HOMELAND_MOVE_PROPHET_RELIGION:
			case AI_HOMELAND_MOVE_SPACESHIP_PART:
			case AI_HOMELAND_MOVE_ADD_SPACESHIP_PART:
				iPriority += iFlavorDefense;
				break;

				// Other miscellaneous types
			case AI_HOMELAND_MOVE_EXPLORE:
			case AI_HOMELAND_MOVE_EXPLORE_SEA:
				iPriority += iFlavorExplore;
				break;

			case AI_HOMELAND_MOVE_SETTLE:
				iPriority += iFlavorExpand;
				break;

			case AI_HOMELAND_MOVE_WORKER:
				iPriority += iFlavorImprove;
				break;

			case AI_HOMELAND_MOVE_WORKER_SEA:
				iPriority += iFlavorNavalImprove;
				break;

			case AI_HOMELAND_MOVE_UPGRADE:
#if defined(MOD_AI_SMART_UPGRADES)
				iPriority += (iFlavorMilitaryTraining + iTurnUpgradePriority);
#else
				iPriority += iFlavorMilitaryTraining;
#endif
				break;

			case AI_HOMELAND_MOVE_WRITER:
				iPriority += iFlavorCulture;
				break;

			case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
				iPriority += iFlavorCulture;
				break;

			case AI_HOMELAND_MOVE_MUSICIAN:
				iPriority += iFlavorCulture;
				break;

			case AI_HOMELAND_MOVE_ANCIENT_RUINS:
				iPriority += iFlavorExplore;
				break;

			case AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH:
				iPriority += iFlavorScience;
				break;

			case AI_HOMELAND_MOVE_ENGINEER_HURRY:
				iPriority += iFlavorWonder;
				break;

			case AI_HOMELAND_MOVE_TRADE_UNIT:
				iPriority += iFlavorGold;
				break;

#if defined(MOD_DIPLOMACY_CITYSTATES)
			case AI_HOMELAND_MOVE_DIPLOMAT_EMBASSY:
				iPriority += iFlavorCulture;
				break;
			case AI_HOMELAND_MOVE_MESSENGER:
				iPriority += iFlavorCulture;
				break;
#endif
			}

			// Store off this move and priority
			CvHomelandMove move;
			move.m_eMoveType = (AIHomelandMove)iI;
			move.m_iPriority = iPriority;
			m_MovePriorityList.push_back(move);
		}
	}

	// Now sort the moves in priority order
	std::stable_sort(m_MovePriorityList.begin(), m_MovePriorityList.end());
}

/// Make lists of everything we might want to target with the homeland AI this turn
void CvHomelandAI::FindHomelandTargets()
{
	int iI;
	CvPlot* pLoopPlot;
	CvHomelandTarget newTarget;

	// Clear out target lists since we rebuild them each turn
	m_TargetedCities.clear();
	m_TargetedSentryPoints.clear();
	m_TargetedForts.clear();
	m_TargetedNavalResources.clear();
	m_TargetedHomelandRoads.clear();
	m_TargetedAncientRuins.clear();
	m_TargetedAntiquitySites.clear();

	TeamTypes eTeam = m_pPlayer->getTeam();

	// Look at every tile on map
	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for(iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{
			// Have a ...
			// ... friendly city?
			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity != NULL)
			{
				if(m_pPlayer->GetID() == pCity->getOwner())
				{
					// Don't send another unit if the tactical AI already sent a garrison here
					UnitHandle pUnit = pLoopPlot->getBestDefender(m_pPlayer->GetID());
					if(!pUnit || !pUnit->isUnderTacticalControl())
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_CITY);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData((void*)pCity);
						newTarget.SetAuxIntData(pCity->getThreatValue());
						m_TargetedCities.push_back(newTarget);
					}
				}
			}

			// ... naval resource?
			else if(pLoopPlot->isWater() &&
			        pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
			{
				ResourceTypes eNonObsoleteResource = pLoopPlot->getNonObsoleteResourceType(eTeam);
				if(eNonObsoleteResource != NO_RESOURCE)
				{
					CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
					if(NULL != pWorkingCity && pWorkingCity->getOwner() == m_pPlayer->GetID())
					{
						// Find proper improvement
						BuildTypes eBuild;
						for(int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							eBuild = ((BuildTypes)iJ);
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if(pkBuildInfo && pkBuildInfo->getImprovement() != NO_IMPROVEMENT && pkBuildInfo->IsWater())
							{
								if(GC.getImprovementInfo((ImprovementTypes) GC.getBuildInfo(eBuild)->getImprovement())->IsImprovementResourceTrade(eNonObsoleteResource))
								{
									newTarget.SetTargetType(AI_HOMELAND_TARGET_NAVAL_RESOURCE);
									newTarget.SetTargetX(pLoopPlot->getX());
									newTarget.SetTargetY(pLoopPlot->getY());
									newTarget.SetAuxData(pLoopPlot);
									newTarget.SetAuxIntData((int)eBuild);
									m_TargetedNavalResources.push_back(newTarget);
									break;
								}
							}
						}
					}
				}
			}

			// ... unpopped goody hut?
			else if(!m_pPlayer->isMinorCiv() && pLoopPlot->isGoody())
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedAncientRuins.push_back(newTarget);
			}

			// ... antiquity site?
			else if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()) && 
				!(pLoopPlot->getOwner() != NO_PLAYER && m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoDiggingPromise(pLoopPlot->getOwner())))
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedAntiquitySites.push_back(newTarget);
			}

			// ... enemy civilian (or embarked) unit?
			else if(pLoopPlot->isVisibleOtherUnit(m_pPlayer->GetID()))
			{
				CvUnit* pTargetUnit = pLoopPlot->getUnitByIndex(0);
				if(!pTargetUnit->isDelayedDeath() && atWar(eTeam, pTargetUnit->getTeam()) && !pTargetUnit->IsCanDefend())
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					m_TargetedAncientRuins.push_back(newTarget);
				}
			}

			// ... possible sentry point? (must be empty or only have friendly units)
			else if(!pLoopPlot->isWater() && (!pLoopPlot->isUnit() || pLoopPlot->getNumDefenders(m_pPlayer->GetID()) > 0))
			{
				// Must be at least adjacent to our land
				if(pLoopPlot->getOwner() == m_pPlayer->GetID() ||
				        (pLoopPlot->isAdjacentTeam(eTeam, true /*bLandOnly*/) && pLoopPlot->getOwner() == NO_PLAYER))
				{
					// See how many outside plots are nearby to monitor
					int iOutsidePlots = pLoopPlot->GetNumAdjacentDifferentTeam(eTeam, true /*bIgnoreWater*/);

					if(iOutsidePlots > 0)
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_SENTRY_POINT);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData(pLoopPlot);

						// Get weight for this sentry point
						int iWeight = iOutsidePlots * 100;
						iWeight += pLoopPlot->defenseModifier(eTeam, true);
						iWeight += m_pPlayer->GetPlotDanger(*pLoopPlot);

						CvCity* pFriendlyCity = m_pPlayer->GetClosestFriendlyCity(*pLoopPlot, 5 /*i SearchRadius */);
						if(pFriendlyCity && pFriendlyCity->getOwner() == m_pPlayer->GetID())
						{
							iWeight += pFriendlyCity->getThreatValue() * pFriendlyCity->getPopulation() / 50;
							if(pFriendlyCity->isCapital())
							{
								iWeight = (iWeight * GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()) / 100;
							}
						}

						if(pLoopPlot->isHills())
						{
							iWeight *= 2;
						}
						if(pLoopPlot->isCoastalLand())
						{
							iWeight /= 2;
						}

						newTarget.SetAuxIntData(iWeight);
						m_TargetedSentryPoints.push_back(newTarget);
					}
				}
			}

			// ... road segment in friendly territory?
			else if(pLoopPlot->getTeam() == eTeam && pLoopPlot->isRoute())
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_HOME_ROAD);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedHomelandRoads.push_back(newTarget);
			}
		}
	}

	// Post-processing on targets
	EliminateAdjacentSentryPoints();
	EliminateAdjacentHomelandRoads();
	std::stable_sort(m_TargetedCities.begin(), m_TargetedCities.end());
}

/// Choose which moves to run and assign units to it
void CvHomelandAI::AssignHomelandMoves()
{
	FStaticVector< CvHomelandMove, 64, true, c_eCiv5GameplayDLL >::iterator it;

	// Proceed in priority order
	for(it = m_MovePriorityList.begin(); it != m_MovePriorityList.end() && !m_CurrentTurnUnits.empty(); ++it)
	{
		CvHomelandMove move = *it;

		AI_PERF_FORMAT("AI-perf-tact.csv", ("Homeland Move: %d, Turn %03d, %s", (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

		switch(move.m_eMoveType)
		{
		case AI_HOMELAND_MOVE_EXPLORE:
			PlotExplorerMoves();
			break;
		case AI_HOMELAND_MOVE_EXPLORE_SEA:
			PlotExplorerSeaMoves();
			break;
		case AI_HOMELAND_MOVE_SETTLE:
			PlotFirstTurnSettlerMoves();
			break;
		case AI_HOMELAND_MOVE_GARRISON:
			PlotGarrisonMoves();
			break;
		case AI_HOMELAND_MOVE_HEAL:
			PlotHealMoves();
			break;
		case AI_HOMELAND_MOVE_TO_SAFETY:
			PlotMovesToSafety();
			break;
		case AI_HOMELAND_MOVE_MOBILE_RESERVE:
			PlotMobileReserveMoves();
			break;
		case AI_HOMELAND_MOVE_SENTRY:
			PlotSentryMoves();
			break;
		case AI_HOMELAND_MOVE_WORKER:
			PlotWorkerMoves();
			break;
		case AI_HOMELAND_MOVE_WORKER_SEA:
			PlotWorkerSeaMoves();
			break;
		case AI_HOMELAND_MOVE_PATROL:
			PlotPatrolMoves();
			break;
		case AI_HOMELAND_MOVE_UPGRADE:
			PlotUpgradeMoves();
			break;
		case AI_HOMELAND_MOVE_ANCIENT_RUINS:
			PlotAncientRuinMoves();
			break;
		case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			PlotGarrisonMoves(true /*bCityStateOnly*/);
			break;
		case AI_HOMELAND_MOVE_WRITER:
			PlotWriterMoves();
			break;
		case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
			PlotArtistMoves();
			break;
		case AI_HOMELAND_MOVE_MUSICIAN:
			PlotMusicianMoves();
			break;
		case AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH:
			PlotScientistMoves();
			break;
		case AI_HOMELAND_MOVE_ENGINEER_HURRY:
			PlotEngineerMoves();
			break;
		case AI_HOMELAND_MOVE_MERCHANT_TRADE:
			PlotMerchantMoves();
			break;
#if defined(MOD_DIPLOMACY_CITYSTATES)
		case AI_HOMELAND_MOVE_DIPLOMAT_EMBASSY:
			if (MOD_DIPLOMACY_CITYSTATES) PlotDiplomatMoves();
			break;
		case AI_HOMELAND_MOVE_MESSENGER:
			if (MOD_DIPLOMACY_CITYSTATES) PlotMessengerMoves();
			break;
#endif
		case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			PlotGeneralMoves();
			break;
		case AI_HOMELAND_MOVE_ADMIRAL_GARRISON:
			PlotAdmiralMoves();
			break;
		case AI_HOMELAND_MOVE_PROPHET_RELIGION:
			PlotProphetMoves();
			break;
		case AI_HOMELAND_MOVE_MISSIONARY:
			PlotMissionaryMoves();
			break;
		case AI_HOMELAND_MOVE_INQUISITOR:
			PlotInquisitorMoves();
			break;
		case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			PlotAircraftMoves();
			break;
		case AI_HOMELAND_MOVE_ADD_SPACESHIP_PART:
			PlotSSPartAdds();
			break;
		case AI_HOMELAND_MOVE_SPACESHIP_PART:
			PlotSSPartMoves();
			break;
		case AI_HOMELAND_MOVE_TREASURE:
			PlotTreasureMoves();
			break;
		case AI_HOMELAND_MOVE_TRADE_UNIT:
			PlotTradeUnitMoves();
			break;
		case AI_HOMELAND_MOVE_ARCHAEOLOGIST:
			PlotArchaeologistMoves();
			break;
		case AI_HOMELAND_MOVE_AIRLIFT:
			PlotAirliftMoves();
			break;
		}
	}

	ReviewUnassignedUnits();
}

/// Get units with explore AI and plan their moves
void CvHomelandAI::PlotExplorerMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE ||
			        pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_EXPLORE)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		// Execute twice so explorers who can reach the end of their sight can move again
		ExecuteExplorerMoves();
		ExecuteExplorerMoves();
	}
}

/// Get units with explore AI and plan their moves
void CvHomelandAI::PlotExplorerSeaMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA ||
			        pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_EXPLORE)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		// Execute twice so explorers who can reach the end of their sight can move again
		ExecuteExplorerMoves();
		ExecuteExplorerMoves();
	}
}

/// Get our first city built
void CvHomelandAI::PlotFirstTurnSettlerMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		bool bGoingToSettle = false;
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman())
		{
			if(m_pPlayer->getNumCities() == 0 && m_CurrentMoveUnits.size() == 0)
			{
				if(pUnit->canFound(pUnit->plot()))
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
					bGoingToSettle = true;
				}
#if defined(MOD_BALANCE_CORE_SETTLER)
				else if(m_pPlayer->isMinorCiv())
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
					bGoingToSettle = true;
				}
#endif
			}
		}

		// If we find a settler that isn't in an operation, let's keep him in place
		if(!bGoingToSettle && pUnit->isFound() && pUnit->getArmyID() == FFreeList::INVALID_INDEX)
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->finishMoves();
		}
	}

	if(!m_CurrentMoveUnits.empty())
	{
		ExecuteFirstTurnSettlerMoves();
	}
}

/// Send units to garrison cities
void CvHomelandAI::PlotGarrisonMoves(bool bCityStateOnly)
{
	// City state garrisoning has a separate priority, so only do it when the flag is on
	if(bCityStateOnly != m_pPlayer->isMinorCiv())
	{
		return;
	}

	// Do we have any targets of this type?
	if(m_TargetedCities.size() > 0)
	{
		for(unsigned int iI = 0; iI < m_TargetedCities.size(); iI++)
		{
			CvPlot* pTarget = GC.getMap().plot(m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY());
			CvCity* pCity = pTarget->getPlotCity();

			if(pCity && pCity->GetLastTurnGarrisonAssigned() < GC.getGame().getGameTurn())
			{
				// Grab units that make sense for this move type
				FindUnitsForThisMove(AI_HOMELAND_MOVE_GARRISON, (iI == 0)/*bFirstTime*/);

				if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
				{
					if(GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns))
					{
						ExecuteMoveToTarget(pTarget);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving to garrison, X: %d, Y: %d, Priority: %d", m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY(), m_TargetedCities[iI].GetAuxIntData());
							LogHomelandMessage(strLogString);
						}

						pCity->SetLastTurnGarrisonAssigned(GC.getGame().getGameTurn());
					}
				}
			}
		}
	}
}

/// Find out which units would like to heal
void CvHomelandAI::PlotHealMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman())
		{
#if defined(MOD_AI_SMART_HEALING)
			int iHealingLimit = pUnit->GetMaxHitPoints();

			if (MOD_AI_SMART_HEALING) {
				CvPlot* unitPlot = pUnit->plot();
				int iDangerLevel = m_pPlayer->GetPlotDanger(*unitPlot);

				if (iDangerLevel > 0)
				{
					// Set the health limit to 75%
					iHealingLimit = ((pUnit->GetMaxHitPoints() * 3) / 4);
				}
			}
#endif

#if defined(MOD_AI_SMART_HEALING)
			// Am I under my health limit and not at sea or already in a city?
			if(pUnit->GetCurrHitPoints() < iHealingLimit && !pUnit->isEmbarked() && !pUnit->plot()->isCity())
#else
			// Am I under 100% health and not at sea or already in a city?
			if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints() && !pUnit->isEmbarked() && !pUnit->plot()->isCity())
#endif
			{
				// If I'm a naval unit I need to be in friendly territory
				if(pUnit->getDomainType() != DOMAIN_SEA || pUnit->plot()->IsFriendlyTerritory(m_pPlayer->GetID()))
				{
					if (!pUnit->IsUnderEnemyRangedAttack())
					{
						CvHomelandUnit unit;
						unit.SetID(pUnit->GetID());
						m_CurrentMoveUnits.push_back(unit);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							CvString strTemp;

							strTemp = pUnit->getUnitInfo().GetDescription();
							strLogString.Format("%s healing at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
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

/// Moved endangered units to safe hexes
void CvHomelandAI::PlotMovesToSafety()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			// Danger value of plot must be greater than 0
			CvPlot* pPlot = pUnit->plot();

			int iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
			if(iDangerLevel > 0)
			{
				bool bAddUnit = false;

				// If civilian (or embarked unit) always ready to flee
				// slewis - 4.18.2013 - Problem here is that a combat unit that is a boat can get stuck in a city hiding from barbarians on the land
				if(!pUnit->IsCanDefend())
				{
					if (pUnit->IsAutomated() && pUnit->GetBaseCombatStrength() > 0)
					{
						// then this is our special case
					}
					else
					{
						bAddUnit = true;
					}
				}

				// Also may be true if a damaged combat unit
				else if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
				{
					if(pUnit->isBarbarian())
					{
						// Barbarian combat units - only naval units flee (but they flee if have taken ANY damage)
						if(pUnit->getDomainType() == DOMAIN_SEA)
						{
							bAddUnit = true;
						}
					}

					// Everyone else flees at less than or equal to 50% combat strength
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
					else if(MOD_AI_SMART_FLEE_FROM_DANGER || pUnit->IsUnderEnemyRangedAttack() || pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
#else
					else if(pUnit->IsUnderEnemyRangedAttack() || pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
#endif
					{
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
						if (MOD_AI_SMART_FLEE_FROM_DANGER) {
							// Use the combat modifier as parameter.
							int iWoundedDamageMultiplier = /*33*/ GC.getWOUNDED_DAMAGE_MULTIPLIER();
							int alteredCombatStrength = ((pUnit->GetBaseCombatStrengthConsideringDamage() * (100 + iWoundedDamageMultiplier)) / 100);
							if(pUnit->IsUnderEnemyRangedAttack() || alteredCombatStrength <= pUnit->GetBaseCombatStrength())
							{
								bAddUnit = true;
							}
						}
						else
#endif
							bAddUnit = true;
					}
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

				if(bAddUnit)
				{
					// Just one unit involved in this move to execute
					CvHomelandUnit unit;
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

/// Send units to roads for quick movement to face any threat
void CvHomelandAI::PlotMobileReserveMoves()
{
	// Do we have any targets of this type?
	if(!m_TargetedHomelandRoads.empty())
	{
		// Prioritize them (LATER)

		// See how many moves of this type we can execute
		for(unsigned int iI = 0; iI < m_TargetedHomelandRoads.size(); iI++)
		{
			CvPlot* pTarget = GC.getMap().plot(m_TargetedHomelandRoads[iI].GetTargetX(), m_TargetedHomelandRoads[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_MOBILE_RESERVE, (iI == 0)/*bFirstTime*/);

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, MAX_INT))
				{
					ExecuteMoveToTarget(pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to mobile reserve muster pt, X: %d, Y: %d", m_TargetedHomelandRoads[iI].GetTargetX(), m_TargetedHomelandRoads[iI].GetTargetY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}

/// Send units to sentry points around borders
void CvHomelandAI::PlotSentryMoves()
{
#if defined(MOD_AI_SECONDARY_WORKERS)
	if (MOD_AI_SECONDARY_WORKERS && !m_pPlayer->isMinorCiv()) {
		// Find any units with a secondary role of worker and see if there is anything close to build
		PlotWorkerMoves(true);
	}
#endif
	
	// Do we have any targets of this type?
	if(!m_TargetedSentryPoints.empty())
	{
		// Prioritize them (LATER)

		// See how many moves of this type we can execute
		for(unsigned int iI = 0; iI < m_TargetedSentryPoints.size(); iI++)
		{
			AI_PERF_FORMAT("Homeland-perf.csv", ("PlotSentryMoves, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

			CvPlot* pTarget = GC.getMap().plot(m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_SENTRY, (iI == 0)/*bFirstTime*/);

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, MAX_INT))
				{
					ExecuteMoveToTarget(pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to sentry point, X: %d, Y: %d, Priority: %d", m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY(), m_TargetedSentryPoints[iI].GetAuxIntData());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}

#if defined(MOD_AI_SECONDARY_SETTLERS)
typedef CvWeightedVector<CvPlot*, 1, true> WeightedFoundPlotVector;

void CvHomelandAI::PlotOpportunisticSettlementMoves()
{
	const char* szCiv = m_pPlayer->getCivilizationTypeKey();

	int iMinHappiness = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MIN_HAPPINESS", 5);
	int iMinTurnsSinceLastCity = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MIN_TURNS_SINCE_LAST_CITY", 10);
	int iMinRevealedTilesThisLandmass = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MIN_REVEALED_TILES", 10);
	int iMaxDistance = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MAX_DISTANCE", 10);
	int iMaxPlotsToConsider = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MAX_PLOTS_CONSIDER", 3);
	int iMaxSettleDistance = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MAX_SETTLE_DISTANCE", 1);
	int iMaxTravelDistance = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MAX_TRAVEL_DISTANCE", 3);

	int iCapitalX = m_pPlayer->getCapitalCity()->getX();
	int iCapitalY = m_pPlayer->getCapitalCity()->getY();

	CvUnit* pLoopUnit;

	if (m_pPlayer->GetHappiness() <= iMinHappiness) {
		return;
	}
	
	if (m_pPlayer->GetTurnsSinceSettledLastCity() <= iMinTurnsSinceLastCity) {
		return;
	}
	
	int iLoop;
	for (pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop)) {
		if (!pLoopUnit->IsCombatUnit() && pLoopUnit->isFound()) {
			return;
		}
	}
	
	// Make a list of all combat units that are on a landmass of a suitable size and could found a city at their current plot
	MoveUnitsArray PossibleSettlerUnits;
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it) {
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit) {
			if (pUnit->IsCombatUnit() && GC.getMap().getArea(pUnit->getArea())->getNumRevealedTiles(m_pPlayer->getTeam()) > iMinRevealedTilesThisLandmass && pUnit->canFound(pUnit->plot())) {
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				unit.SetAuxIntData(plotDistance(pUnit->getX(), pUnit->getY(), iCapitalX, iCapitalY));
				PossibleSettlerUnits.push_back(unit);
			}
		}
	}
	
	// Sort them by proximity to the capital
	std::stable_sort(PossibleSettlerUnits.begin(), PossibleSettlerUnits.end(), HomelandAIHelpers::CvHomelandUnitAuxIntSort);
	
	if (PossibleSettlerUnits.size() > 0) {
		// CUSTOMLOG("%i possible units for opportunistic settlement", PossibleSettlerUnits.size());
		CvMap& kMap = GC.getMap();
		TeamTypes eUnitTeam = m_pPlayer->getTeam();

		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator settlerUnitIt;
		for (settlerUnitIt = PossibleSettlerUnits.begin(); settlerUnitIt != PossibleSettlerUnits.end(); ++settlerUnitIt) {
			UnitHandle pUnit = m_pPlayer->getUnit(settlerUnitIt->GetID());
			int iArea = pUnit->getArea();
			int iUnitX = pUnit->getX();
			int iUnitY = pUnit->getY();
			// CUSTOMLOG("  ... for unit at (%i, %i)", iUnitX, iUnitY);

			// Find the best locations on the landmass (within X tiles of the unit)
			WeightedFoundPlotVector aBestPlots;
			aBestPlots.reserve((iMaxDistance+1) * 2);

			for (int iPlotX = iUnitX-iMaxDistance; iPlotX != iUnitX+iMaxDistance; iPlotX++) {
				for (int iPlotY = iUnitY-iMaxDistance; iPlotY != iUnitY+iMaxDistance; iPlotY++) {
					CvPlot* pPlot = kMap.plot(iPlotX, iPlotY);
					if (!pPlot) continue;
					if (pPlot->getArea() != iArea) continue;
					if (!pPlot->isRevealed(eUnitTeam)) continue;
					if (!m_pPlayer->canFound(iPlotX, iPlotY)) continue;

					aBestPlots.push_back(pPlot, m_pPlayer->AI_foundValue(iPlotX, iPlotY, -1, false));
				}
			}

			if (aBestPlots.size() > 0) {
				int iMaxPlots = std::min(iMaxPlotsToConsider, aBestPlots.size());
				// CUSTOMLOG("  ... found %i possible plots, considering the first %i", aBestPlots.size(), iMaxPlots);

				aBestPlots.SortItems();

				// For the first N locations, can the unit reach it in less than one turn (ie move to the plot and found the city as one turn)
				for (int i = 0; i < iMaxPlots; ++i ) {
					int iPathTurns;
					CvPlot* pFoundPlot = aBestPlots.GetElement(i);
					// CUSTOMLOG("  ... possible city plot at (%i, %i)", pFoundPlot->getX(), pFoundPlot->getY());
					bool bCanFindPath = pUnit->GeneratePath(pFoundPlot, MOVE_TERRITORY_NO_UNEXPLORED, true, &iPathTurns);

					if (bCanFindPath) {
						// CUSTOMLOG("    ... is %i moves away", iPathTurns)
						
						if (iPathTurns <= iMaxSettleDistance) {
							// CUSTOMLOG("    ... here comes the city!")
							// If so, move to the plot, found the city and bail out of this method
							pUnit->setXY(pFoundPlot->getX(), pFoundPlot->getY());
							pUnit->PushMission(CvTypes::getMISSION_FOUND());
							UnitProcessed(pUnit->GetID());
							return;
						} else if (iPathTurns <= iMaxTravelDistance) {
							// CUSTOMLOG("    ... moving towards the city site!")
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pFoundPlot->getX(), pFoundPlot->getY());
							UnitProcessed(pUnit->GetID());
						}
					}
				}
			}
		}
	}
}
#endif

/// Find something for all workers to do
#if defined(MOD_AI_SECONDARY_WORKERS)
void CvHomelandAI::PlotWorkerMoves(bool bSecondary)
#else
void CvHomelandAI::PlotWorkerMoves()
#endif
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
#if defined(MOD_AI_SECONDARY_WORKERS)
			bool bUsePrimaryUnit = (pUnit->AI_getUnitAIType() == UNITAI_WORKER || pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_BUILD);
			bool bUseSecondaryUnit = (pUnit->AI_getUnitAIType() != UNITAI_WORKER && (pUnit->getUnitInfo().GetUnitAIType(UNITAI_WORKER) || pUnit->getUnitInfo().GetUnitAIType(UNITAI_WORKER_SEA)) && pUnit->getDomainType() == DOMAIN_LAND);

			if((!bSecondary && bUsePrimaryUnit) || (bSecondary && bUseSecondaryUnit))
#else
			if(pUnit->AI_getUnitAIType() == UNITAI_WORKER  ||
			        pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_BUILD)
#endif
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
#if defined(MOD_AI_SECONDARY_WORKERS)
		ExecuteWorkerMoves(bSecondary);
#else
		ExecuteWorkerMoves();
#endif
	}
}

/// Send out work boats to harvest resources
void CvHomelandAI::PlotWorkerSeaMoves()
{
	ClearCurrentMoveUnits();
	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA  ||
			   pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_BUILD)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator moveUnitIt;
	for(moveUnitIt = m_CurrentMoveUnits.begin(); moveUnitIt != m_CurrentMoveUnits.end(); ++moveUnitIt)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
		int iTargetIndex = -1;
		int iTargetMoves = MAX_INT;

		// See how many moves of this type we can execute
		for (unsigned int iI = 0; iI < m_TargetedNavalResources.size(); iI++)
		{
			// See what units we have who can reach target this turn
			CvPlot* pTarget = GC.getMap().plot(m_TargetedNavalResources[iI].GetTargetX(), m_TargetedNavalResources[iI].GetTargetY());

			if (!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
			{
				continue;
			}

			int iMoves = TurnsToReachTarget(pUnit.pointer(), pTarget);
			if (iMoves < iTargetMoves)
			{
				iTargetMoves = iMoves;
				iTargetIndex = iI;
			}
		}

		if (iTargetIndex != -1)
		{
			// Queue best one up to capture it
			int iTargetX = m_TargetedNavalResources[iTargetIndex].GetTargetX();
			int iTargetY = m_TargetedNavalResources[iTargetIndex].GetTargetY();
			CvPlot* pTarget = GC.getMap().plot(iTargetX, iTargetY);

			bool bResult = false;
			if(pUnit->UnitPathTo(iTargetX, iTargetY, 0) > 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), iTargetX, iTargetY);
				if(pUnit->plot() == pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), m_TargetedNavalResources[iTargetIndex].GetAuxIntData(), -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
					bResult = true;
				}
				else
				{
					pUnit->finishMoves();
				}

				// Delete this unit from those we have to move
				UnitProcessed(m_CurrentMoveUnits.begin()->GetID());
			}
			else
			{
				if(pUnit->plot() == pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), m_TargetedNavalResources[iTargetIndex].GetAuxIntData(), -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
					bResult = true;
				}
			}
		
			if (bResult)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Harvesting naval resource at, X: %d, Y: %d", m_TargetedNavalResources[iTargetIndex].GetTargetX(), m_TargetedNavalResources[iTargetIndex].GetTargetY());
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving toward naval resource at, X: %d, Y: %d", m_TargetedNavalResources[iTargetIndex].GetTargetX(), m_TargetedNavalResources[iTargetIndex].GetTargetY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// When nothing better to do, have units patrol to an adjacent tiles
void CvHomelandAI::PlotPatrolMoves()
{
#if defined(MOD_AI_SECONDARY_SETTLERS)
	if (MOD_AI_SECONDARY_SETTLERS && !m_pPlayer->isMinorCiv()) {
		// Find any units with a secondary role of settler and check for opportunistic city founding
		PlotOpportunisticSettlementMoves();
	}
#endif
	
	ClearCurrentMoveUnits();

	// Loop through all remaining units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman() && pUnit->getDomainType() != DOMAIN_AIR && !pUnit->isTrade())
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (MOD_BALANCE_CORE_MILITARY) 
	{
			//Patrolling is pointless.
			if(pUnit->canFortify(pUnit->plot()) && !pUnit->isEmbarked())
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			}
		}
	}
	}
#else
			CvPlot* pTarget = FindPatrolTarget(pUnit.pointer());
			if(pTarget)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				unit.SetTarget(pTarget);
				m_CurrentMoveUnits.push_back(unit);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;

					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("%s (%d) patrolling to, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->GetID(), pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
#endif
	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecutePatrolMoves();
	}
}

/// Find units that we can upgrade
void CvHomelandAI::PlotUpgradeMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator moveUnitIt;
	ResourceTypes eResource;
	int iNumResource;
	int iNumResourceInUnit;
	bool bMissingResource;

	ClearCurrentMoveUnits();
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		// Don't try and upgrade a human player's unit or one already recruited for an operation
		UnitHandle pUnit = m_pPlayer->getUnit(*it);

#if defined(MOD_AI_SMART_UPGRADES)
		if(MOD_AI_SMART_UPGRADES && pUnit && !pUnit->isHuman())
		{
			int iArmyId = pUnit->getArmyID();
			if (iArmyId != -1)
			{
				CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(iArmyId);

				if (pThisArmy)
				{
					if((pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION) || (pThisArmy->GetArmyAIState() == ARMYAISTATE_AT_DESTINATION))
					{
						continue;
					}
				}
			}
		}
#endif

#if defined(MOD_AI_SMART_UPGRADES)
		if(pUnit && !pUnit->isHuman() && (!MOD_AI_SMART_UPGRADES || pUnit->getArmyID() == -1))
#else
		if(pUnit && !pUnit->isHuman() && pUnit->getArmyID() == -1)
#endif
		{
			// Can this unit be upgraded?
			UnitTypes eUpgradeUnitType = pUnit->GetUpgradeUnitType();
			if(eUpgradeUnitType != NO_UNIT)
			{
				// Tech requirement
				TechTypes ePrereqTech = (TechTypes) GC.getUnitInfo(eUpgradeUnitType)->GetPrereqAndTech();
				if(ePrereqTech == NO_TECH || GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
				{
					// Resource requirement
					bMissingResource = false;
					for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bMissingResource; iResourceLoop++)
					{
						eResource = (ResourceTypes) iResourceLoop;
						iNumResource = GC.getUnitInfo(eUpgradeUnitType)->GetResourceQuantityRequirement(eResource);
						if (iNumResource > 0)
						{
							iNumResourceInUnit = pUnit->getUnitInfo().GetResourceQuantityRequirement(eResource);
							if (m_pPlayer->getNumResourceAvailable(eResource) + iNumResourceInUnit < iNumResource)
							{
								bMissingResource = true;
							}
						}
					}

					if(!bMissingResource)
					{
						CvHomelandUnit unit;
						unit.SetID(pUnit->GetID());

						// Initial priority: units with lowest power first
						int iPriority = UPGRADE_THIS_TURN_PRIORITY_BOOST - GC.getUnitInfo(pUnit->getUnitType())->GetPower();

						// Priority is boosted if can upgrade immediately
						if(pUnit->CanUpgradeRightNow(false))
						{
							iPriority += UPGRADE_THIS_TURN_PRIORITY_BOOST;
						}

						// Alternatively, priority boosted a lesser amount if in friendly territory
						else if(pUnit->getOwner() == pUnit->plot()->getOwner())
						{
							iPriority += UPGRADE_IN_TERRITORY_PRIORITY_BOOST;
						}

						unit.SetAuxIntData(iPriority);
						m_CurrentMoveUnits.push_back(unit);
					}
				}
			}
		}
	}

	// No units found, cancel request for money if we have one in
	if(m_CurrentMoveUnits.size() == 0)
	{
		if(!m_pPlayer->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_UNIT_UPGRADE))
		{
			m_pPlayer->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE);
		}
	}

	else
	{
		// Sort results so highest priority is first
		std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end(), HomelandAIHelpers::CvHomelandUnitAuxIntReverseSort);

		int iFlavorMilitaryTraining = 0;
		for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && iFlavorMilitaryTraining == 0; iFlavorLoop++)
		{
			if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
			{
				iFlavorMilitaryTraining = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop);
			}
		}

		iFlavorMilitaryTraining = max(1,iFlavorMilitaryTraining/3);
		int iBonusUpgrades = max(0,GC.getGame().getHandicapInfo().GetID() - 5); // more at the higher difficulties (the AI should have more money to spend)
		iFlavorMilitaryTraining += iBonusUpgrades;

		// Try to find a unit that can upgrade immediately
		int iNumUpgraded = 0;
		for(moveUnitIt = m_CurrentMoveUnits.begin(); moveUnitIt != m_CurrentMoveUnits.end(); ++moveUnitIt)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
			if(pUnit->CanUpgradeRightNow(false))
			{
				CvUnit* pNewUnit = pUnit->DoUpgrade();
				UnitProcessed(pUnit->GetID());
				UnitProcessed(pNewUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp1, strTemp2;
					strTemp1 = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strTemp2 = GC.getUnitInfo(pNewUnit->getUnitType())->GetDescription();
					strLogString.Format("Upgrading unit from type %s to type %s, X: %d, Y: %d", strTemp1.GetCString(), strTemp2.GetCString(), pNewUnit->getX(), pNewUnit->getY());
					LogHomelandMessage(strLogString);
				}

				iNumUpgraded++;
				if(iNumUpgraded >= iFlavorMilitaryTraining)
				{
					return; // Only upgrade iFlavorMilitaryTraining units per turn
				}
			}
		}

		if(iNumUpgraded > 0)
		{
			return;
		}

		// Couldn't do all upgrades this turn, get ready for highest priority unit to upgrade
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());

		int iAmountRequired = pUnit->upgradePrice(pUnit->GetUpgradeUnitType());
		bool bRequiresGold = (iAmountRequired > 0);
		int iGoldPriority = 0;

		if(bRequiresGold)
		{
			// Find priority of this financial request
			int iCurrentFlavorMilitaryTraining = 0;
			for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && iCurrentFlavorMilitaryTraining == 0; iFlavorLoop++)
			{
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
				{
					iCurrentFlavorMilitaryTraining = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
				}
			}
			iGoldPriority = GC.getAI_GOLD_PRIORITY_UPGRADE_BASE();
			iGoldPriority += GC.getAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT() * iCurrentFlavorMilitaryTraining;
		}

		// Start saving
		if(bRequiresGold)
		{
			m_pPlayer->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE);
			m_pPlayer->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE, iAmountRequired, iGoldPriority);
		}

		// Already in friendly territory
		if(pUnit->plot()->getOwner() == pUnit->getOwner())
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;

				strTemp = pUnit->getUnitInfo().GetDescription();
				strLogString.Format("Need gold for %s upgrade, GOLD: Available = %d, Needed = %d, Priority = %d",
				                    strTemp.GetCString(), m_pPlayer->GetTreasury()->GetGold(), iAmountRequired, iGoldPriority);
				LogHomelandMessage(strLogString);
			}
		}

		else
		{
			// Move top priority unit toward closest city
			CvCity* pUpgradeCity = NULL;
			int iBestDistance = MAX_INT;
			for(unsigned int iI = 0; iI < m_TargetedCities.size(); iI++)
			{
				CvPlot* pTarget = GC.getMap().plot(m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY());
				CvCity* pCity = pTarget->getPlotCity();

				int iDistance = plotDistance(pCity->getX(), pCity->getY(), pUnit->getX(), pUnit->getY());

				if(iDistance < iBestDistance)
				{
					iBestDistance = iDistance;
					pUpgradeCity = pCity;
				}
			}

			if(pUpgradeCity)
			{
				if(MoveToEmptySpaceNearTarget(pUnit.pointer(), pUpgradeCity->plot()))
				{
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						CvString strTemp;
						strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
						strLogString.Format("Moving %s for upgrade at %s, GOLD: Available = %d, Needed = %d, Priority = %d, Dist = %d",
						                    strTemp.GetCString(), pUpgradeCity->getName().GetCString(),
						                    m_pPlayer->GetTreasury()->GetGold(), iAmountRequired, iGoldPriority, iBestDistance);
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}

	return;
}

/// Pop goody huts nearby
void CvHomelandAI::PlotAncientRuinMoves()
{
	// Do we have any targets of this type?
	if(!m_TargetedAncientRuins.empty())
	{
		// Prioritize them (LATER)

		// See how many moves of this type we can execute
		for(unsigned int iI = 0; iI < m_TargetedAncientRuins.size(); iI++)
		{
			CvPlot* pTarget = GC.getMap().plot(m_TargetedAncientRuins[iI].GetTargetX(), m_TargetedAncientRuins[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_ANCIENT_RUINS, (iI == 0)/*bFirstTime*/);

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns))
				{
					ExecuteMoveToTarget(pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to goody hut (non-explorer), X: %d, Y: %d", m_TargetedAncientRuins[iI].GetTargetX(), m_TargetedAncientRuins[iI].GetTargetY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}

/// Find moves for great writers
void CvHomelandAI::PlotWriterMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_WRITER)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWriterMoves();
	}
}

/// Find moves for great artists
void CvHomelandAI::PlotArtistMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_ARTIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteArtistMoves();
	}
}

/// Find moves for great musicians
void CvHomelandAI::PlotMusicianMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_MUSICIAN)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMusicianMoves();
	}
}

/// Find moves for great scientists
void CvHomelandAI::PlotScientistMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_SCIENTIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteScientistMoves();
	}
}

/// Find moves for great engineers
void CvHomelandAI::PlotEngineerMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_ENGINEER)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteEngineerMoves();
	}
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
/// Find moves for great diplomats
void CvHomelandAI::PlotDiplomatMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_DIPLOMAT)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteDiplomatMoves();
	}
}
/// Find moves for messengers
void CvHomelandAI::PlotMessengerMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_MESSENGER)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMessengerMoves();
	}
}
#endif

/// Find moves for great merchants
void CvHomelandAI::PlotMerchantMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_MERCHANT)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMerchantMoves();
	}
}

/// Find moves for prophets
void CvHomelandAI::PlotProphetMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_PROPHET)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteProphetMoves();
	}
}

/// Find moves for missionaries
void CvHomelandAI::PlotMissionaryMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_MISSIONARY)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMissionaryMoves();
	}
}

/// Find moves for inquisitors
void CvHomelandAI::PlotInquisitorMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_INQUISITOR)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteInquisitorMoves();
	}
}

/// Find moves for spaceship parts
void CvHomelandAI::PlotSSPartMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_SPACESHIP_PART)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteSSPartMoves();
	}
}

/// Find spaceship parts ready to add to spaceship
void CvHomelandAI::PlotSSPartAdds()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_SPACESHIP_PART)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteSSPartAdds();
	}
}

/// Find moves for treasures
void CvHomelandAI::PlotTreasureMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_TREASURE)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteTreasureMoves();
	}
}

/// Find moves for great generals
void CvHomelandAI::PlotGeneralMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_GENERAL)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteGeneralMoves();
	}
}

/// Find moves for great admirals
void CvHomelandAI::PlotAdmiralMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_ADMIRAL)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAdmiralMoves();
	}
}

/// Send units to cities near the front (or carriers)
void CvHomelandAI::PlotAircraftMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->getDomainType() == DOMAIN_AIR && pUnit->getDamage() < 50)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
#if defined(MOD_AI_SMART_AIR_TACTICS)
		if (MOD_AI_SMART_AIR_TACTICS)
			ExecuteAircraftInterceptions();
#endif
		ExecuteAircraftMoves();
	}
}

/// Send trade units on their way
void CvHomelandAI::PlotTradeUnitMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_TRADE_UNIT)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteTradeUnitMoves();
	}
}

/// Send archaeologists to safe sites
void CvHomelandAI::PlotArchaeologistMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteArchaeologistMoves();
	}
}

/// Send spare units to cities that can airlift
void CvHomelandAI::PlotAirliftMoves()
{
	if (m_pPlayer->isHuman())
	{
		return;
	}

	// Need at least 2 cities with airports
	vector<CvCity *> aAirliftCities;
	CvCity *pLoopCity;
	int iCityLoop;
	for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity->CanAirlift())
		{
			aAirliftCities.push_back(pLoopCity);
		}
	}
	if (aAirliftCities.size() < 2)
	{
		return;
	}

	// Create list of empty land plots we own adjacent to airlift cities that are not endangered
	vector<CvPlot *> aAirliftPlots;
	CvTacticalAnalysisMap* pMap = GC.getGame().GetTacticalAnalysisMap();
	CvTacticalDominanceZone *pZone;
	vector<CvCity *>::const_iterator it;
	for (it = aAirliftCities.begin(); it != aAirliftCities.end(); it++)
	{
		pZone = pMap->GetZoneByCity(*it, false);
		if (pZone && (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY || pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_VISIBLE))
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pLoopPlot = plotDirection((*it)->getX(), (*it)->getY(), ((DirectionTypes)iI));
				if (pLoopPlot != NULL && !pLoopPlot->isWater() && !pLoopPlot->isMountain() && pLoopPlot->getOwner() == m_pPlayer->GetID())
				{
					UnitHandle pBestDefender = pLoopPlot->getBestDefender(m_pPlayer->GetID());
					if (pBestDefender)
					{
						if (std::find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), pBestDefender->GetID()) != m_CurrentTurnUnits.end())
						{
							pBestDefender->finishMoves();
							UnitProcessed(pBestDefender->GetID());
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Remaining in place for future airlift - %s, X: %d, Y: %d", pBestDefender->getName().GetCString(), pBestDefender->getX(), pBestDefender->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
					else
					{
						aAirliftPlots.push_back(pLoopPlot);
					}
				}
			}
		}
	}

	vector<CvPlot *>::const_iterator plotIt;
	for (plotIt = aAirliftPlots.begin(); plotIt != aAirliftPlots.end(); plotIt++)
	{
		FindUnitsForThisMove(AI_HOMELAND_MOVE_MOBILE_RESERVE, true);

		if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
		{
			CvPlot *pTarget = *plotIt;

			if (GetBestUnitToReachTarget(pTarget, MAX_INT))
			{
				ExecuteMoveToTarget(pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to airlift origin plot, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// Log that we couldn't find assignments for some units
void CvHomelandAI::ReviewUnassignedUnits()
{
	// Loop through all remaining units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->SetTurnProcessed(true);

			CvString strTemp;
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
			if(pkUnitInfo)
			{
				strTemp = pkUnitInfo->GetDescription();
				CvString strLogString;
				strLogString.Format("Unassigned %s at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}

/// Creates cities for AI civs on first turn
void CvHomelandAI::ExecuteFirstTurnSettlerMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
#if defined(MOD_BALANCE_CORE_SETTLER)
			if (MOD_BALANCE_CORE_SETTLER) 
			{
				//Let's not try this too much. Two turns is enough.
				if(GC.getGame().getGameTurn() <= 2)
				{
					int iInitialPlotValue = 0;
					int iAdjacentValue = 0;
					CvPlot* pBestAdjacentPlot = NULL;
					//Let's check for a river, coast and mountains within one hex. If all there, break, you fool!
					if(pUnit->plot()->isFreshWater() && pUnit->plot()->isCoastalLand() && (pUnit->plot()->GetNumAdjacentMountains() > 0))
					{
						pUnit->PushMission(CvTypes::getMISSION_FOUND());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Founded city at starting location as it is great, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							break;
						}
					}
					else if(!pUnit->plot()->isFreshWater() || !pUnit->plot()->isCoastalLand() || (pUnit->plot()->GetNumAdjacentMountains() <= 0))
					{
						iInitialPlotValue = GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pUnit->plot(), m_pPlayer, NO_YIELD, false);

						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL && pUnit->canFound(pAdjacentPlot))
							{
								iAdjacentValue = GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pAdjacentPlot, m_pPlayer, NO_YIELD, false);
								if(iAdjacentValue > iInitialPlotValue)
								{
									iInitialPlotValue = iAdjacentValue;
									pBestAdjacentPlot = pAdjacentPlot;
								}
							}
						}
						if(pBestAdjacentPlot != NULL)
						{
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestAdjacentPlot->getX(), pBestAdjacentPlot->getY());
							if(pUnit->plot() == pBestAdjacentPlot && (pUnit->getMoves() > 0))
							{
								pUnit->PushMission(CvTypes::getMISSION_FOUND());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Founded city at adjacent site, as it is superior. X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									break;
								}
							}
							//Couldn't get there and found in one move? That's okay - it is better to lose a turn or two early on than to be in a bad spot.
							else
							{
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moved to superior starting site. Wish me luck! X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									break;
								}
							}
						}
						else
						{
							pUnit->PushMission(CvTypes::getMISSION_FOUND());
							UnitProcessed(pUnit->GetID());
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Founded city at this location as it is the best we can do, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
								LogHomelandMessage(strLogString);
								break;
							}
						}
					}
					else
					{
						pUnit->PushMission(CvTypes::getMISSION_FOUND());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Founded city at base spot as the adjacent test was invalid, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
				else
				{
					int iInitialPlotValue = 0;
					int iAdjacentValue = 0;
					CvPlot* pBestAdjacentPlot = NULL;
					if(pUnit->canFound(pUnit->plot()))
					{
						pUnit->PushMission(CvTypes::getMISSION_FOUND());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Founded city because we are out of time, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							break;
						}
					}
					else
					{
						iInitialPlotValue = GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pUnit->plot(), m_pPlayer, NO_YIELD, false);

						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL)
							{
								iAdjacentValue = GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pAdjacentPlot, m_pPlayer, NO_YIELD, false);
								if(iAdjacentValue > iInitialPlotValue)
								{
									iInitialPlotValue = iAdjacentValue;
									pBestAdjacentPlot = pAdjacentPlot;
								}
							}
						}
						if(pBestAdjacentPlot != NULL)
						{
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestAdjacentPlot->getX(), pBestAdjacentPlot->getY());
							if(pUnit->plot() == pBestAdjacentPlot && (pUnit->getMoves() > 0))
							{
								if(pUnit->canFound(pBestAdjacentPlot))
								{
									pUnit->PushMission(CvTypes::getMISSION_FOUND());
									UnitProcessed(pUnit->GetID());
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Founded city at adjacent site, as we are out of time. X: %d, Y: %d", pUnit->getX(), pUnit->getY());
										LogHomelandMessage(strLogString);
										break;
									}
								}
								else
								{
									UnitProcessed(pUnit->GetID());
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Moved to superior starting site. Trying to hurry! X: %d, Y: %d", pUnit->getX(), pUnit->getY());
										LogHomelandMessage(strLogString);
										break;
									}
								}
							}
						}
						else
						{
							for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
							{
								CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));
								if(pAdjacentPlot != NULL && !pAdjacentPlot->isWater() && !pAdjacentPlot->isImpassable())
								{
									pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pAdjacentPlot->getX(), pAdjacentPlot->getY());
									UnitProcessed(pUnit->GetID());
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Things aren't looking good for us! Scramble! X: %d, Y: %d", pUnit->getX(), pUnit->getY());
										LogHomelandMessage(strLogString);
										break;
									}
									break;
								}
							}
						}
					}
				}
			}
#else
			pUnit->PushMission(CvTypes::getMISSION_FOUND());
			UnitProcessed(pUnit->GetID());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Founded city at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
#endif
		}
	}
}

#define PATH_PLAN_LAST
typedef CvWeightedVector<CvPlot*, 100, true> WeightedPlotVector;

/// Moves units to explore the map
void CvHomelandAI::ExecuteExplorerMoves()
{
	bool bFoundNearbyExplorePlot = false;
	CvEconomicAI* pEconomicAI = m_pPlayer->GetEconomicAI();

	pEconomicAI->UpdatePlots();

#if defined(PATH_PLAN_LAST)
	WeightedPlotVector aBestPlotList;
	aBestPlotList.reserve(100);
#endif

	CvTwoLayerPathFinder& kPathFinder = GC.getPathFinder();
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit || pUnit->TurnProcessed())
		{
			continue;
		}

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(pUnit->getOwner());
			args->Push(pUnit->GetID());

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "UnitGetSpecialExploreTarget", args.get(), bResult);

			if(bResult)
			{
				continue;
			}
		}

		if (!m_pPlayer->isHuman() && pUnit->CanStartMission(CvTypes::getMISSION_SELL_EXOTIC_GOODS(), -1, -1))
		{
			// Far enough from home to get a good reward?
			float fRewardFactor = pUnit->calculateExoticGoodsDistanceFactor(pUnit->plot());
			if (fRewardFactor >= 0.5f)
			{
				pUnit->PushMission(CvTypes::getMISSION_SELL_EXOTIC_GOODS());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("UnitID: %d used Sell Exotic Goods, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}

		int iUnitX = pUnit->getX();
		int iUnitY = pUnit->getY();

		CvPlot* pkStepPlot = NULL;
		CvPlot* pGoodyPlot = pEconomicAI->GetUnitTargetGoodyPlot(pUnit.pointer(), &pkStepPlot);
		if (pGoodyPlot)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("UnitID: %d has goody target, X: %d, Y: %d", pUnit->GetID(), pGoodyPlot->getX(), pGoodyPlot->getY());
				LogHomelandMessage(strLogString);
			}
		}

		if(pGoodyPlot && (pGoodyPlot->isGoody(m_pPlayer->getTeam()) || (pGoodyPlot->HasBarbarianCamp()) && !pGoodyPlot->isVisibleEnemyDefender(pUnit.pointer())))
		{
			bool bCanFindPath = false;
			if (pkStepPlot)	// Do we already have our first step point?
			{
				if (IsValidExplorerEndTurnPlot(pUnit.pointer(), pkStepPlot))
					bCanFindPath = true;

				// The economic AI should recalculate next time through, but just in case, let's say that we've used the step plot
				pEconomicAI->ClearUnitTargetGoodyStepPlot(pUnit.pointer());
			}

			if (!pkStepPlot || !bCanFindPath)
			{
				bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if(bCanFindPath)
				{
					pkStepPlot = kPathFinder.GetPathEndTurnPlot();
				}
			}

			if (bCanFindPath)
			{
				if(pkStepPlot)
				{
					CvAssert(!pUnit->atPlot(*pkStepPlot));
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Moving to goody hut, X: %d, Y: %d, from X: %d Y: %d", pUnit->GetID(), pkStepPlot->getX(), pkStepPlot->getY(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pkStepPlot->getX(), pkStepPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pkStepPlot);
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d No end turn plot to goody from, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}

				continue;

			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("UnitID: %d Can't find path to goody from, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}

		CvPlot* pBestPlot = NULL;
		int iBestPlotScore = 0;

#if defined(PATH_PLAN_LAST)
		aBestPlotList.clear();
#endif
		TeamTypes eTeam = pUnit->getTeam();
		int iBaseSightRange = pUnit->getUnitInfo().GetBaseSightRange();

		int iMovementRange = pUnit->movesLeft() / GC.getMOVE_DENOMINATOR();

		// if iMovesLeft value is greater than zero but not enough to be a positive integer when divided by move_denominator
		// then just give them an extra movement. Who could it hurt, right?
		if (pUnit->movesLeft() > 0 && iMovementRange == 0)
		{
			iMovementRange = 1;
		}

		for(int iX = -iMovementRange; iX <= iMovementRange; iX++)
		{
			for(int iY = -iMovementRange; iY <= iMovementRange; iY++)
			{
				CvPlot* pEvalPlot = plotXYWithRangeCheck(iUnitX, iUnitY, iX, iY, iMovementRange);
				if(!pEvalPlot)
				{
					continue;
				}

				if(!IsValidExplorerEndTurnPlot(pUnit.pointer(), pEvalPlot))
				{
					continue;
				}

#if !defined(PATH_PLAN_LAST)
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), pUnit->getX(), pUnit->getY(), pEvalPlot->getX(), pEvalPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if(!bCanFindPath)
				{
					continue;
				}

				CvAStarNode* pNode = kPathFinder.GetLastNode();
				int iDistance = pNode->m_iData2;
				if(iDistance > 1)
				{
					continue;
				}
#endif

				DomainTypes eDomain = pUnit->getDomainType();
				int iScore = CvEconomicAI::ScoreExplorePlot(pEvalPlot, eTeam, iBaseSightRange, eDomain);
				if(iScore > 0)
				{
					if (eDomain == DOMAIN_LAND)
					{
						if (pEvalPlot->isHills())
						{
							iScore += 50;
						}
						if (pUnit->IsEmbarkAllWater() && !pEvalPlot->isShallowWater())
						{
							iScore += 200;
						}
					}
					else if (eDomain == DOMAIN_SEA)
					{
						if(pUnit->canSellExoticGoods(pEvalPlot))
						{
							float fRewardFactor = pUnit->calculateExoticGoodsDistanceFactor(pEvalPlot);
							if (fRewardFactor >= 0.75f)
							{
								iScore += 150;
							}
							else if (fRewardFactor >= 0.5f)
							{
								iScore += 75;
							}
						}

						if(pEvalPlot->isAdjacentToLand())
						{
							iScore += 200;
						}
					}

#if defined(PATH_PLAN_LAST)
					aBestPlotList.push_back(pEvalPlot, iScore);
#endif
				}

#if !defined(PATH_PLAN_LAST)
				if(iScore > iBestPlotScore)
				{
					pBestPlot = pEvalPlot;
					iBestPlotScore = iScore;
					bFoundNearbyExplorePlot = true;
				}
#endif
			}
		}

#if defined(PATH_PLAN_LAST)
		uint uiListSize;
		if ((uiListSize = aBestPlotList.size()) > 0)
		{
			aBestPlotList.SortItems();	// highest score will be first.
			for (uint i = 0; i < uiListSize; ++i )	
			{
				CvPlot* pPlot = aBestPlotList.GetElement(i);
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pPlot->getX(), pPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if(!bCanFindPath)
				{
					continue;
				}

				CvAStarNode* pNode = GC.getPathFinder().GetLastNode();
				int iDistance = pNode->m_iData2;
				if(iDistance <= 1)
				{
					pBestPlot = pPlot;
					break;
				}
			}
		}
#endif

		if(!pBestPlot && iMovementRange > 0)
		{
			FFastVector<int>& aiExplorationPlots = pEconomicAI->GetExplorationPlots();
			if (aiExplorationPlots.size() > 0)
			{
				FFastVector<int>& aiExplorationPlotRatings = pEconomicAI->GetExplorationPlotRatings();

				aBestPlotList.clear();
				aBestPlotList.reserve(aiExplorationPlots.size());

				iBestPlotScore = 0;

				for(uint ui = 0; ui < aiExplorationPlots.size(); ui++)
				{
					int iPlot = aiExplorationPlots[ui];
					if(iPlot < 0)
					{
						continue;
					}

					CvPlot* pEvalPlot = GC.getMap().plotByIndex(iPlot);
					if(!pEvalPlot)
					{
						continue;
					}

					int iPlotScore = 0;

					if(!IsValidExplorerEndTurnPlot(pUnit.pointer(), pEvalPlot))
					{
						continue;
					}

					int iRating = aiExplorationPlotRatings[ui];

#if defined(PATH_PLAN_LAST)
					int iDistance = plotDistance(iUnitX, iUnitY, pEvalPlot->getX(), pEvalPlot->getY());
					int iEstimateTurns = iDistance / iMovementRange;
					if(iEstimateTurns == 0)
					{
						iPlotScore = 1000 * iRating;
					}
					else
					{
						iPlotScore = (1000 * iRating) / iEstimateTurns;
					}

					aBestPlotList.push_back(pEvalPlot, iPlotScore);
#else
					// hitting the path finder, may not be the best idea. . .
					bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pEvalPlot->getX(), pEvalPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
					if(!bCanFindPath)
					{
						continue;
					}

					CvAStarNode* pNode = GC.getPathFinder().GetLastNode();
					int iDistance = pNode->m_iData2;
					if(iDistance == 0)
					{
						iPlotScore = 1000 * iRating;
					}
					else
					{
						iPlotScore = (1000 * iRating) / iDistance;
					}

					if(iPlotScore > iBestPlotScore)
					{
						CvPlot* pEndTurnPlot = GC.getPathFinder().GetPathEndTurnPlot();
						if(pEndTurnPlot == pUnit->plot())
						{
							pBestPlot = NULL;
							iBestPlotScore = iPlotScore;
						}
						else if(IsValidExplorerEndTurnPlot(pUnit.pointer(), pEndTurnPlot))
						{
							pBestPlot = pEndTurnPlot;
							iBestPlotScore = iPlotScore;
						}
						else
						{
							// not a valid destination
							continue;
						}
					}
#endif
				}

#if defined(PATH_PLAN_LAST)
				if ((uiListSize = aBestPlotList.size()) > 0)
				{
					aBestPlotList.SortItems();		// Highest score will be first.
					for (uint i = 0; i < uiListSize ; ++i )	
					{
						CvPlot* pPlot = aBestPlotList.GetElement(i);
						bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pPlot->getX(), pPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
						if(!bCanFindPath)
						{
							continue;
						}

						CvPlot* pEndTurnPlot = GC.getPathFinder().GetPathEndTurnPlot();
						if(pEndTurnPlot == pUnit->plot())
						{
							continue;
						}
						else if(IsValidExplorerEndTurnPlot(pUnit.pointer(), pEndTurnPlot))
						{
							pBestPlot = pEndTurnPlot;
							break;
						}
						else
						{
							// not a valid destination
							continue;
						}
					}
				}
#endif
			}
		}

		if(pBestPlot)
		{
			CvAssertMsg(!pUnit->atPlot(*pBestPlot), "Exploring unit is already at the best place to explore");
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pBestPlot);

			// Only mark as done if out of movement
			if(pUnit->getMoves() <= 0)
			{
				UnitProcessed(pUnit->GetID());
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				if(bFoundNearbyExplorePlot)
				{
					strLogString.Format("UnitID: %d Explored to nearby target, To X: %d, Y: %d, From X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY(), iUnitX, iUnitY);
				}
				else
				{
					strLogString.Format("UnitID: %d Explored to distant target, To X: %d, Y: %d, From X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY(), iUnitX, iUnitY);
				}
				LogHomelandMessage(strLogString);
			}
		}
		else
		{
			if(pUnit->isHuman())
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("UnitID: %d Explorer (human) found no target, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
				pUnit->SetAutomateType(NO_AUTOMATE);
				UnitProcessed(pUnit->GetID());
			}
			else
			{
				// If this is a land explorer and there is no ignore unit path to a friendly city, then disband him
				if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Explorer (AI) found no target, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}

					CvCity* pLoopCity;
					int iLoop;
					bool bFoundPath = false;
					for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						if(GC.getIgnoreUnitsPathFinder().DoesPathExist(*(pUnit), pUnit->plot(), pLoopCity->plot()))
						{
							bFoundPath = true;
							break;
						}
					}
					if(!bFoundPath)
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Disbanding explorer, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);

						UnitProcessed(pUnit->GetID());
						pUnit->kill(true);
						m_pPlayer->GetEconomicAI()->IncrementExplorersDisbanded();
					}
				}
				else if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Sea explorer (AI) found no target, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}

/// Moves units to explore the map
#if defined(MOD_AI_SECONDARY_WORKERS)
void CvHomelandAI::ExecuteWorkerMoves(bool bSecondary)
#else
void CvHomelandAI::ExecuteWorkerMoves()
#endif
{
	CvString strLogString;

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			CvPlot* pPlot = pUnit->plot();

			AI_PERF_FORMAT("Homeland-ExecuteWorkerMoves-perf.csv", ("ExecuteWorkerMoves, Turn %03d, %s, Unit %d, at x=%d, y=%d", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription(), pUnit->GetID(), pUnit->getX(), pUnit->getY()) );

			if(pPlot && m_pPlayer->IsPlotUnderImmediateThreat(*pPlot) && !pPlot->getBestDefender(m_pPlayer->GetID()))
			{
#if defined(MOD_AI_SECONDARY_WORKERS)
				if(MoveCivilianToSafety(pUnit.pointer(), false, bSecondary))
#else
				if(MoveCivilianToSafety(pUnit.pointer()))
#endif
				{
					if(GC.getLogging() && GC.GetBuilderAILogging())
					{
						// Open the log file
						CvString strFileName = "BuilderTaskingLog.csv";
						FILogFile* pLog;
						pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

						// write in data
						CvString strLog;
						CvString strTemp;

						CvString strPlayerName;
						strPlayerName = m_pPlayer->getCivilizationShortDescription();
						strLog += strPlayerName;
						strLog += ",";

						strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
						strLog += strTemp;

						strTemp.Format("%d,", pUnit->GetID()); // unit id
						strLog += strTemp;

						strLog += "1st Safety,";

						pLog->Msg(strLog);
					}

					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
					continue;
				}
			}

#if defined(MOD_AI_SECONDARY_WORKERS)
			bool bActionPerformed = ExecuteWorkerMove(pUnit.pointer(), bSecondary);
#else
			bool bActionPerformed = ExecuteWorkerMove(pUnit.pointer());
#endif
			if(bActionPerformed)
			{
				continue;
			}

			// if there's nothing else to do, move to the safest spot nearby
#if defined(MOD_AI_SECONDARY_WORKERS)
			// Only move primary workers (actual civilian workers/workboats) or embarked secondary workers (combat units) to safety
			if (bSecondary && !pUnit.pointer()->isEmbarked())
			{
				continue;
			}

			if(MoveCivilianToSafety(pUnit.pointer(), true /*bIgnoreUnits*/), bSecondary)
#else
			if(MoveCivilianToSafety(pUnit.pointer(), true /*bIgnoreUnits*/))
#endif
			{
				if(GC.getLogging() && GC.GetBuilderAILogging())
				{
					// Open the log file
					CvString strFileName = "BuilderTaskingLog.csv";
					FILogFile* pLog;
					pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

					// write in data
					CvString strLog;
					CvString strTemp;

					CvString strPlayerName;
					strPlayerName = m_pPlayer->getCivilizationShortDescription();
					strLog += strPlayerName;
					strLog += ",";

					strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
					strLog += strTemp;

					strTemp.Format("%d,", pUnit->GetID()); // unit id
					strLog += strTemp;

					strLog += "2nd Safety,";

					pLog->Msg(strLog);
				}

				// slewis - this was removed because a unit would eat all its moves. So if it didn't do anything this turn, it wouldn't be able to work 
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				if (!m_pPlayer->isHuman())
				{
					pUnit->finishMoves();
				}
				UnitProcessed(pUnit->GetID());
				continue;
			}

			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Send work boats to deploy on naval resources (return true if improvement built)
bool CvHomelandAI::ExecuteWorkerSeaMoves(CvHomelandTarget target, CvPlot* pTarget)
{
	bool bRtnValue = false;

	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID());
	if(pUnit)
	{
		if(pUnit->UnitPathTo(pTarget->getX(), pTarget->getY(), 0) > 0)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			if(pUnit->plot() == pTarget)
			{
				pUnit->PushMission(CvTypes::getMISSION_BUILD(), (int)target.GetAuxIntData(), -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
				bRtnValue = true;
			}
			else
			{
				pUnit->finishMoves();
			}

			// Delete this unit from those we have to move
			UnitProcessed(m_CurrentMoveUnits.begin()->GetID());
		}
	}
	return bRtnValue;
}

/// Heal chosen units
void CvHomelandAI::ExecuteHeals()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
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
			UnitProcessed(it->GetID());
		}
	}
}

/// Moves units to the hex with the lowest danger
void CvHomelandAI::ExecuteMovesToSafestPlot()
{
	int iDanger;

	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		WeightedPlotVector aBestPlotList;
		aBestPlotList.reserve(100);

		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			CvPlot* pBestPlot = NULL;

			int iRange = pUnit->getUnitInfo().GetMoves();

			// For each plot within movement range of the fleeing unit
			for(int iX = -iRange; iX <= iRange; iX++)
			{
				for(int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot* pPlot = GC.getMap().plot(pUnit->getX() + iX, pUnit->getY() + iY);
					if(pPlot == NULL)
					{
						continue;
					}

					if (plotDistance(pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY()) > iRange)
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
					bool bIsInCover = (pPlot->getNumDefenders(m_pPlayer->GetID()) > 0) && !pUnit->IsCanDefend();
					bool bIsInTerritory = (pPlot->getTeam() == m_pPlayer->getTeam());
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
					bool bNeedEmbark = ((pUnit->getDomainType() == DOMAIN_LAND) && (!pUnit->plot()->isWater()) && (pPlot->isWater()));
#endif

					#define MAX_DANGER_VALUE	100000
					#define PREFERENCE_LEVEL(x, y) (x * MAX_DANGER_VALUE) + ((MAX_DANGER_VALUE - 1) - y)

					CvAssert(iDanger < MAX_DANGER_VALUE);

					int iScore;
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
					// Lower preference if water and uncovered.
					if(MOD_AI_SMART_FLEE_FROM_DANGER && bNeedEmbark && !bIsInCover)
					{
						iScore = PREFERENCE_LEVEL(0, iDanger);
					}
					else
#endif
					if(bIsInCity)
					{
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
						iScore = PREFERENCE_LEVEL((MOD_AI_SMART_FLEE_FROM_DANGER ? 6 : 5), iDanger);
#else
						iScore = PREFERENCE_LEVEL(5, iDanger);
#endif
					}
					else if(bIsZeroDanger)
					{
						if (bIsInTerritory)
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
							iScore = PREFERENCE_LEVEL((MOD_AI_SMART_FLEE_FROM_DANGER ? 5 : 4), iDanger);
#else
							iScore = PREFERENCE_LEVEL(4, iDanger);
#endif
						else
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
							iScore = PREFERENCE_LEVEL((MOD_AI_SMART_FLEE_FROM_DANGER ? 4 : 3), iDanger);
#else
							iScore = PREFERENCE_LEVEL(3, iDanger);
#endif
					}
					else if(bIsInCover)
					{
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
						iScore = PREFERENCE_LEVEL((MOD_AI_SMART_FLEE_FROM_DANGER ? 3 : 2), iDanger);
#else
						iScore = PREFERENCE_LEVEL(2, iDanger);
#endif
					}
					else if(bIsInTerritory)
					{
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
						iScore = PREFERENCE_LEVEL((MOD_AI_SMART_FLEE_FROM_DANGER ? 2 : 1), iDanger);
#else
						iScore = PREFERENCE_LEVEL(1, iDanger);
#endif
					}
					// if we have no good home, head to the lowest danger value
					else 
					{
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
						iScore = PREFERENCE_LEVEL((MOD_AI_SMART_FLEE_FROM_DANGER ? 1 : 0), iDanger);
#else
						iScore = PREFERENCE_LEVEL(0, iDanger);
#endif
					}

					aBestPlotList.push_back(pPlot, iScore);
				}
			}

			aBestPlotList.SortItems();

			// Now loop through the sorted score list and go to the best one we can reach in one turn.
			// #define EXECUTEMOVESTOSAFESTPLOT_FAILURE_LIMIT
			#ifdef EXECUTEMOVESTOSAFESTPLOT_FAILURE_LIMIT
			int iFailureLimit = 10;
			#endif
			uint uiListSize;
			if ((uiListSize = aBestPlotList.size()) > 0)
			{
				aBestPlotList.SortItems();	// highest score will be first.
				for (uint i = 0; i < uiListSize; ++i )	
				{
					CvPlot* pPlot = aBestPlotList.GetElement(i);

					if(CanReachInXTurns(pUnit, pPlot, 1))
					{
						pBestPlot = pPlot;
						break;
					}
					#ifdef EXECUTEMOVESTOSAFESTPLOT_FAILURE_LIMIT
					else
					{
						if (iFailureLimit-- == 0)
							break;
					}
					#endif
				}
			}

			if(pBestPlot != NULL)
			{
				// Move to the lowest danger value found
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_UNITS_IGNORE_DANGER);
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("Moving %s to safety, X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// Patrol with chosen units
void CvHomelandAI::ExecutePatrolMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		CvPlot* pTarget = it->GetTarget();
		if(pUnit && pTarget)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
		}
	}
}

//	---------------------------------------------------------------------------
/// Find one unit to move to target, starting with high priority list
void CvHomelandAI::ExecuteMoveToTarget(CvPlot* pTarget)
{
	AI_PERF_FORMAT("Homeland-ExecuteMove-perf.csv", ("ExecuteMoveToTarget, %d, %d, Turn %03d, %s", pTarget->getX(), pTarget->getY(), GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	// Do we have a pre-calcuated 'best' unit?
	CvUnit *pBestUnit = NULL;
	if (m_CurrentBestMoveHighPriorityUnit)
	{
		// Don't move high priority unit if regular priority unit is closer
		if (m_CurrentBestMoveUnit && m_iCurrentBestMoveUnitTurns < m_iCurrentBestMoveHighPriorityUnitTurns)
			pBestUnit = m_CurrentBestMoveUnit;
		else
			pBestUnit = m_CurrentBestMoveHighPriorityUnit;
	}
	else
		pBestUnit = m_CurrentBestMoveUnit;

	if (pBestUnit)
	{
		if(pBestUnit->plot() == pTarget && pBestUnit->canFortify(pBestUnit->plot()))
		{
			pBestUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pBestUnit->SetFortifiedThisTurn(true);
			UnitProcessed(pBestUnit->GetID());
			return;
		}
		else
		{
			// Best units have already had a full path check to the target, so just add the move
			pBestUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pBestUnit->finishMoves();
			UnitProcessed(pBestUnit->GetID());
			return;
		}
	}

	MoveUnitsArray::iterator it;

	// Start with high priority list
	for(it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		// Don't move high priority unit if regular priority unit is closer
		if(m_CurrentMoveUnits.size() > 0 && m_CurrentMoveUnits.begin()->GetMovesToTarget() < it->GetMovesToTarget())
		{
			break;
		}

		if(pUnit)
		{
			if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
				UnitProcessed(it->GetID());
				return;
			}

			else if(it->GetMovesToTarget() < GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE() || TurnsToReachTarget(pUnit, pTarget) != MAX_INT)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(it->GetID());
				return;
			}
		}
	}

	// Then regular priority
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			if(pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
				UnitProcessed(it->GetID());
				return;
			}

			else if(it->GetMovesToTarget() < GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE() || TurnsToReachTarget(pUnit, pTarget) != MAX_INT)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(it->GetID());
				return;
			}
		}
	}
}

/// Great writer moves
void CvHomelandAI::ExecuteWriterMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST:
			ExecuteCultureBlast(pUnit.pointer());
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			{
				// Do we want to create a Great Work?
				GreatWorkType eGreatWorkType = pUnit->GetGreatWork();
				CvCity* pTargetCity = m_pPlayer->GetEconomicAI()->GetBestGreatWorkCity(pUnit->plot(), eGreatWorkType);

				// No?  Just move to safety...
				if (pTargetCity == NULL)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great writer can't find a Great Work slot. Running to safety");
						LogHomelandMessage(strLogString);
					}

					MoveCivilianToSafety(pUnit.pointer());
				}
				else
				{
					// Already at (or adjacent to) target?
					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							continue;
						}
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						CvPlot *pBestTarget = NULL;
						int iBestTurns = MAX_INT;
						int iTurns;
						iTurns = TurnsToReachTarget(pUnit, pTargetCity->plot());
						if (iTurns < iBestTurns)
						{
							pBestTarget = pTargetCity->plot();
						}
						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL)
							{
								iTurns = TurnsToReachTarget(pUnit, pAdjacentPlot);
								if (iTurns < iBestTurns)
								{
									pBestTarget = pAdjacentPlot;
									iBestTurns = iTurns;
								}
							}
						}

						if (pBestTarget)
						{
							// In less than one turn?
							if (iBestTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}

							// In multiple moves
							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->finishMoves();
								UnitProcessed(it->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Writer toward Great Work city at, X: %d, Y: %d", pBestTarget->getX(),  pBestTarget->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}
						else
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Could not find a target for Great Writer at, X: %d, Y: %d", pUnit->getX(),  pUnit->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

/// Expends an artist to start a golden age
void CvHomelandAI::ExecuteArtistMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			{
				// Do we want to create a Great Work?
				GreatWorkType eGreatWorkType = pUnit->GetGreatWork();
				CvCity* pTargetCity = m_pPlayer->GetEconomicAI()->GetBestGreatWorkCity(pUnit->plot(), eGreatWorkType);

				// No?  Just move to safety...
				if (pTargetCity == NULL)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great artist can't find a Great Work slot. Running to safety");
						LogHomelandMessage(strLogString);
					}

					MoveCivilianToSafety(pUnit.pointer());
				}
				else
				{
					// Already at (or adjacent to) target?
					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							continue;
						}
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						CvPlot *pBestTarget = NULL;
						int iBestTurns = MAX_INT;
						int iTurns;
						iTurns = TurnsToReachTarget(pUnit, pTargetCity->plot());
						if (iTurns < iBestTurns)
						{
							pBestTarget = pTargetCity->plot();
						}
						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL)
							{
								iTurns = TurnsToReachTarget(pUnit, pAdjacentPlot);
								if (iTurns < iBestTurns)
								{
									pBestTarget = pAdjacentPlot;
									iBestTurns = iTurns;
								}
							}
						}

						if (pBestTarget)
						{
							// In less than one turn?
							if (iBestTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}

							// In multiple moves
							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->finishMoves();
								UnitProcessed(it->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Artist toward Great Work city at, X: %d, Y: %d", pBestTarget->getX(),  pBestTarget->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}
						else
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Could not find a target for Great Artist at, X: %d, Y: %d", pUnit->getX(),  pUnit->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

/// Expends an artist to start a golden age
void CvHomelandAI::ExecuteMusicianMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST:
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			{
				// Do we want to create a Great Work?
				GreatWorkType eGreatWorkType = pUnit->GetGreatWork();
				CvCity* pTargetCity = m_pPlayer->GetEconomicAI()->GetBestGreatWorkCity(pUnit->plot(), eGreatWorkType);

				// No?  Just move to safety...
				if (pTargetCity == NULL)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great musician can't find a Great Work slot. Running to safety");
						LogHomelandMessage(strLogString);
					}

					MoveCivilianToSafety(pUnit.pointer());
				}
				else
				{
					// Already at (or adjacent to) target?
					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
							continue;
						}
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						CvPlot *pBestTarget = NULL;
						int iBestTurns = MAX_INT;
						int iTurns;
						iTurns = TurnsToReachTarget(pUnit, pTargetCity->plot());
						if (iTurns < iBestTurns)
						{
							pBestTarget = pTargetCity->plot();
						}
						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL)
							{
								iTurns = TurnsToReachTarget(pUnit, pAdjacentPlot);
								if (iTurns < iBestTurns)
								{
									pBestTarget = pAdjacentPlot;
									iBestTurns = iTurns;
								}
							}
						}

						if (pBestTarget)
						{
							// In less than one turn?
							if (iBestTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}

							// In multiple moves
							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
								pUnit->finishMoves();
								UnitProcessed(it->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Musician toward Great Work city at, X: %d, Y: %d", pBestTarget->getX(),  pBestTarget->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}
						else
						{
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Could not find a target for Great Musician at, X: %d, Y: %d", pUnit->getX(),  pUnit->getY());
								LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

/// Expends a scientist to gain a free tech
void CvHomelandAI::ExecuteScientistMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			pUnit->PushMission(CvTypes::getMISSION_DISCOVER());
			UnitProcessed(pUnit->GetID());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Great scientist discovered free tech at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
			break;
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

/// Expends an engineer to gain a wonder (or a very emergency build)
void CvHomelandAI::ExecuteEngineerMoves()
{
	CvCity* pWonderCity;
	int iTurnsToTarget;

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
		{
			// Do we want to build any wonder?
			int iNextWonderWeight;
			CvCity* pCityToBuildAt = 0;
			BuildingTypes eNextWonderDesired = m_pPlayer->GetWonderProductionAI()->ChooseWonderForGreatEngineer(false, iNextWonderWeight, pCityToBuildAt);

			// No?  Just move to safety...
			if(eNextWonderDesired == NO_BUILDING)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great builder can't find a good wonder to build. Running to safety");
					LogHomelandMessage(strLogString);
				}

				MoveCivilianToSafety(pUnit.pointer());
			}
			else
			{
				bool bForceWonderCity = true;

				// Are we less than 25% done building the most desired wonder chosen by the city specialization AI?
				pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
				if(pWonderCity)
				{
					int iProductionSoFar = pWonderCity->getProduction();
					int iProductionRemaining = pWonderCity->getProductionNeeded(eNextWonderDesired);

					if(pWonderCity->getProductionBuilding() == eNextWonderDesired && iProductionSoFar * 3 < iProductionRemaining)
					{
						// If engineer can move to city before half done
						int iTurnsRemaining = pWonderCity->getProductionTurnsLeft();
						iTurnsToTarget = TurnsToReachTarget(pUnit, pWonderCity->plot(), true /*bReusePaths*/, true);
						if(iTurnsToTarget * 3 < iTurnsRemaining)
						{
							bForceWonderCity = false;

							// Already at target and the wonder is underway?
							if(pWonderCity->getProductionBuilding() == eNextWonderDesired && iTurnsToTarget == 0 && pUnit->plot() == pWonderCity->plot())
							{
								pUnit->PushMission(CvTypes::getMISSION_HURRY());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer hurrying wonder chosen by city specialization AI at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}

							// No, then move there
							else
							{
								if(GetBestUnitToReachTarget(pWonderCity->plot(), MAX_INT))
								{
									ExecuteMoveToTarget(pWonderCity->plot());

									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Moving Great Engineer to city specialization wonder city at, X: %d, Y: %d", pWonderCity->getX(),  pWonderCity->getY());
										LogHomelandMessage(strLogString);
									}
								}
							}
						}
					}
				}

				if(bForceWonderCity)
				{
					pWonderCity = pCityToBuildAt;

					if(pWonderCity)
					{
						iTurnsToTarget = TurnsToReachTarget(pUnit, pWonderCity->plot(), false /*bReusePaths*/, true);

						// Already at target?
						if(iTurnsToTarget == 0 && pUnit->plot() == pWonderCity->plot())
						{
							// Switch production
							pWonderCity->pushOrder(ORDER_CONSTRUCT, eNextWonderDesired, -1, false, false, false);

							if (pWonderCity->getProductionTurnsLeft() > 1)
							{
								// Rush it
								pUnit->PushMission(CvTypes::getMISSION_HURRY());
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer hurrying free wonder at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}
							else
							{
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer not needed to hurry 1-turn wonder at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
									continue;
								}
							}
						}

						// No, then move there
						else
						{
							if(GetBestUnitToReachTarget(pWonderCity->plot(), MAX_INT))
							{
								ExecuteMoveToTarget(pWonderCity->plot());

								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Engineer for free wonder to city at, X: %d, Y: %d", pWonderCity->getX(),  pWonderCity->getY());
									LogHomelandMessage(strLogString);
								}
							}
						}

					}
				}
			}
		}
		break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
void CvHomelandAI::ExecuteDiplomatMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}
		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();

		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
		{
			int iTargetTurns;
			CvPlot* pTarget = GET_PLAYER(m_pPlayer->GetID()).ChooseDiplomatTargetPlot(pUnit, &iTargetTurns);
			BuildTypes eBuild = (BuildTypes)GC.getInfoTypeForString("BUILD_EMBASSY");
			if(pTarget)
			{
				if(pUnit->plot() == pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great Diplomat creating Embassy at %s", pUnit->plot()->GetAdjacentCity()->getName().c_str());
					LogHomelandMessage(strLogString);
					}
				}
				else if(iTargetTurns < 1)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

					if(pUnit->plot() == pTarget && pUnit->canMove())
					{
						pUnit->PushMission(CvTypes::getMISSION_BUILD(), eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Great Diplomat moving to create Embassy at %s", pUnit->plot()->GetAdjacentCity()->getName().c_str());
							LogHomelandMessage(strLogString);
						}
					}
				}
				else
				{
					m_CurrentBestMoveHighPriorityUnit = NULL;
					m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
					ExecuteMoveToTarget(pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great Diplomat moving to city-state, currently at X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
		break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			//Handled by economic AI
		break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
		MoveCivilianToSafety(pUnit.pointer());
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Moving Great Diplomat to safety.");
			LogHomelandMessage(strLogString);
		}
		break;
		}
	}
}
void CvHomelandAI::ExecuteMessengerMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}
		
		//Do trade mission
		int iTargetTurns;
		CvPlot* pTarget = GET_PLAYER(m_pPlayer->GetID()).ChooseMessengerTargetPlot(pUnit, &iTargetTurns);
		if(pTarget)
		{
			if(pUnit->plot() == pTarget && pUnit->canMove() && pUnit->canTrade(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_TRADE());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Diplomatic Unit finishing Diplomatic Mission at %s", pUnit->plot()->GetAdjacentCity()->getName().c_str());
					LogHomelandMessage(strLogString);
				}
			}
			else if(iTargetTurns < 1)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				
				if(pUnit->plot() == pTarget && pUnit->canMove())
				{
					pUnit->PushMission(CvTypes::getMISSION_TRADE());
					
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Diplomatic Unit moving to finish Diplomatic Mission at %s", pUnit->plot()->GetAdjacentCity()->getName().c_str());
						LogHomelandMessage(strLogString);
					}
				}
			}
			else
			{
				m_CurrentBestMoveHighPriorityUnit = NULL;
				m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
				ExecuteMoveToTarget(pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Diplomatic Unit moving to city-state, currently at X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
		//Dangerous?
		else
		{
			MoveCivilianToSafety(pUnit.pointer());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving Messenger to safety.");
				LogHomelandMessage(strLogString);
			}
		}
	}
}
#endif

void CvHomelandAI::ExecuteMerchantMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			// handled by economic AI
			break;
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

void CvHomelandAI::ExecuteProphetMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			ExecuteWorkerMove(pUnit.pointer());
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:

			// Can I found a religion?
			if(pUnit->CanFoundReligion(pUnit->plot()))
			{
				CvString strLogString;
				strLogString.Format("Great Prophet founding a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
				LogHomelandMessage(strLogString);

				pUnit->DoFoundReligion();
			}

			// Can I enhance a religion?
			else if(pUnit->CanEnhanceReligion(pUnit->plot()))
			{
				CvString strLogString;
				strLogString.Format("Great Prophet enhancing a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
				LogHomelandMessage(strLogString);

				pUnit->DoEnhanceReligion();
			}

			// Move to closest city without a civilian in it
			else
			{
				CvCity* pLoopCity;
				int iLoopCity = 0;
				int iBestTurns = MAX_INT;
				CvPlot* pBestCityPlot = NULL;
				for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
				{
					bool bSkipCity = false;

					CvPlot* pTarget = pLoopCity->plot();
					for(int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
					{
						// Don't go here if a civilian is already present
						if(!pTarget->getUnitByIndex(iUnitLoop)->IsCombatUnit())
						{
							bSkipCity = true;
							break;
						}
					}

					if(!bSkipCity)
					{
						int iTurns = TurnsToReachTarget(pUnit, pTarget);
						if(iTurns < iBestTurns)
						{
							iBestTurns = iTurns;
							pBestCityPlot = pTarget;
						}
					}
				}

				if(pBestCityPlot)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestCityPlot->getX(), pBestCityPlot->getY());
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving Great Prophet to nearest city without civilian, X: %d, Y: %d", pBestCityPlot->getX(), pBestCityPlot->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("No place to move Great Prophet at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}

			break;

		case GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION:
			{
				int iTargetTurns;
				CvPlot* pTarget = m_pPlayer->GetReligionAI()->ChooseProphetTargetPlot(pUnit, &iTargetTurns);
				if(pTarget)
				{
					if(pUnit->plot() == pTarget)
					{
						pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Spreading religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
							LogHomelandMessage(strLogString);
						}
					}
					else if(iTargetTurns < 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

						if(pUnit->plot() == pTarget && pUnit->canMove())
						{
							pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Move to spread religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
								LogHomelandMessage(strLogString);
							}
						}
						else
						{
							CvAssertMsg(false, "Internal error with Prophet spread religion AI move, contact Ed.");
						}
					}
					else
					{
						
						m_CurrentBestMoveHighPriorityUnit = NULL;
						m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
						ExecuteMoveToTarget(pTarget);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
			}
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
			break;
		}
	}
}

/// Moves a great general into an important city to aid its defense
void CvHomelandAI::ExecuteGeneralMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	CvPlot* pHolyCityPlot = NULL;
	CvCity* pHolyCity = NULL;

	// Do we have an Apollo program to stay clear of?
	bool bHaveApolloInCapital = false;
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			bHaveApolloInCapital = true;
		}
	}

	// Do we have a holy city to stay clear of?
	bool bKeepHolyCityClear = false;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eMyReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
	const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
	if(pMyReligion)
	{
		pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
		pHolyCity = pHolyCityPlot->getPlotCity();
		if(pHolyCity && (pHolyCity->getOwner() == m_pPlayer->GetID()))
		{
			bKeepHolyCityClear = true;
		}
	}

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		// this is for the citadel/culture bomb
		if (pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
		{
			int iValue = 0;
			CvPlot* pTargetPlot = GET_PLAYER(m_pPlayer->GetID()).FindBestArtistTargetPlot(pUnit.pointer(), iValue);
			if(pTargetPlot)
			{
				if(pUnit->plot() == pTargetPlot)
				{
					// find the great general improvement
					BuildTypes eSelectedBuildType = NO_BUILD;
					BuildTypes eBuild;
					int iBuildIndex;
					for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
					{
						eBuild = (BuildTypes)iBuildIndex;
						CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
						if(pkBuild == NULL)
							continue;
						
						if(!pUnit->canBuild(pTargetPlot, eBuild))
						{
							continue;
						}
	
						eSelectedBuildType = eBuild;
						break;
					}

					CvAssertMsg(eSelectedBuildType != NO_BUILD, "Great General trying to build something it doesn't qualify for");
					if (eSelectedBuildType != NO_BUILD)
					{
						pUnit->PushMission(CvTypes::getMISSION_BUILD(), eSelectedBuildType, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTargetPlot);
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Great General culture bombed/citadel'd at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
				else
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great general moving to culture bomb/citadel at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
				continue;
			}
		}


		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			ExecuteGoldenAgeMove(pUnit.pointer());
			continue;
		}

		// if we already built the Apollo Program we don't want the general in the capital because it'll block spaceship parts

		// Already in a friendly city?
		CvPlot* pUnitPlot =  pUnit->plot();
		if(pUnitPlot->isFriendlyCity(*pUnit, false) && (!bHaveApolloInCapital || !pUnitPlot->getPlotCity()->isCapital()) && (!bKeepHolyCityClear || pUnitPlot != pHolyCityPlot))
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Great General remaining as garrison for %s, X: %d, Y: %d", pUnit->plot()->getPlotCity()->getName().GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}

		else
		{
			CvCity* pLoopCity;
			int iLoopCity = 0;
			int iBestTurns = MAX_INT;
			CvPlot* pBestCityPlot = NULL;
			for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
			{
				if(bHaveApolloInCapital && pLoopCity->isCapital())
				{
					continue;
				}

				if(bKeepHolyCityClear && pLoopCity == pHolyCity)
				{
					continue;
				}

				bool bSkipCity = false;

				CvPlot* pTarget = pLoopCity->plot();
				for(int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
				{
					// Don't go here if a general or admiral is already present
					if(pTarget->getUnitByIndex(iUnitLoop)->AI_getUnitAIType() == UNITAI_GENERAL)
					{
						bSkipCity = true;
						break;
					}
					else if(pTarget->getUnitByIndex(iUnitLoop)->AI_getUnitAIType() == UNITAI_ADMIRAL)
					{
						bSkipCity = true;
						break;
					}
				}

				if(!bSkipCity)
				{
					int iTurns = TurnsToReachTarget(pUnit, pTarget);
					if(iTurns < iBestTurns)
					{
						iBestTurns = iTurns;
						pBestCityPlot = pTarget;
					}
				}
			}

			if(pBestCityPlot)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestCityPlot->getX(), pBestCityPlot->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving Great General to city garrison, X: %d, Y: %d", pBestCityPlot->getX(), pBestCityPlot->getY());
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("No place to move Great General at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// Moves a great admiral into an important coastal city to aid its defense
void CvHomelandAI::ExecuteAdmiralMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	CvPlot* pHolyCityPlot = NULL;
	CvCity* pHolyCity = NULL;

	// Do we have an Apollo program to stay clear of?
	bool bHaveApolloInCapital = false;
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			bHaveApolloInCapital = true;
		}
	}

	// Do we have a holy city to stay clear of?
	bool bKeepHolyCityClear = false;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eMyReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
	const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
	if(pMyReligion)
	{
		pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
		pHolyCity = pHolyCityPlot->getPlotCity();
		if(pHolyCity && pHolyCity->isCoastal() && pHolyCity->getOwner() == m_pPlayer->GetID())
		{
			bKeepHolyCityClear = true;
		}
	}

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			ExecuteGoldenAgeMove(pUnit.pointer());
			continue;
		}

		bool bNotAtFriendlyCity = !pUnit->plot()->isCity() || pUnit->plot()->getOwner() != pUnit->getOwner();

			// Score cities to move to
		CvCity* pLoopCity;
		int iLoopCity = 0;
		CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> weightedCityList;
		for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			if (bHaveApolloInCapital && pLoopCity->isCapital())
			{
				continue;
			}

			if (bKeepHolyCityClear && pLoopCity == pHolyCity)
			{
				continue;
			}

			if (!pLoopCity->isCoastal())
			{
				continue;
			}

			// Don't go here if a different general or admiral is already present
			bool bSkipCity = false;
			CvPlot* pTarget = pLoopCity->plot();
			for(int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
			{
				CvUnit *pLoopUnit = pTarget->getUnitByIndex(iUnitLoop);
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_GENERAL && pLoopUnit->GetID() != pUnit->GetID())
				{
					bSkipCity = true;
					break;
				}
				else if(pLoopUnit->AI_getUnitAIType() == UNITAI_ADMIRAL && pLoopUnit->GetID() != pUnit->GetID())
				{
					bSkipCity = true;
					break;
				}
			}
			if(bSkipCity)
			{
				continue;
			}

			int iTurns = TurnsToReachTarget(pUnit, pLoopCity->plot());

			// Don't go here if I'm not in a city currently and this city is not reachable by normal movement
			if (bNotAtFriendlyCity)
			{
				if (iTurns == MAX_INT)
				{
					continue;
				}
			}

			// Weight is size of largest adjacent ocean
			int iWeight = pTarget->GetSizeLargestAdjacentWater();

			// If this city is damaged, divide weight by the damage level
			if (pLoopCity->getDamage() > 0)
			{
				iWeight /= pLoopCity->getDamage();
			}

			// Subtract off turns to reach
			if (iTurns != MAX_INT)
			{
				iWeight -= iTurns;
			}

			weightedCityList.push_back(pLoopCity, iWeight);
		}

		weightedCityList.SortItems();
		if (weightedCityList.size() > 0)
		{
			CvCity *pChosenCity = weightedCityList.GetElement(0);

			// Am I already in chosen city?
			if (pUnit->plot() == pChosenCity->plot())
			{
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great Admiral still in most favored city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
					LogHomelandMessage(strLogString);
				}
			}
			
			// Am I currently in a different friendly city?
			else if (!bNotAtFriendlyCity)
			{
				pUnit->PushMission(CvTypes::getMISSION_CHANGE_ADMIRAL_PORT(), pChosenCity->getX(), pChosenCity->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Transferring Great Admiral to city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
					LogHomelandMessage(strLogString);
				}
			}

			// Move normally to this city
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pChosenCity->getX(), pChosenCity->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving Great Admiral normally to city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
		else
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("No place to move Great Admiral at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}

// Get a missionary to the best city, then spread the word
void CvHomelandAI::ExecuteMissionaryMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns;
		CvPlot* pTarget = m_pPlayer->GetReligionAI()->ChooseMissionaryTargetPlot(pUnit, &iTargetTurns);
		if(pTarget)
		{
			if(pUnit->plot() == pTarget)
			{
				pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Spreading religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}
			else if(iTargetTurns < 1)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

				if(pUnit->plot() == pTarget && pUnit->canMove())
				{
					pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Move to spread religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					CvAssertMsg(false, "Internal error with Missionary AI move, contact Ed.");
				}
			}
			else
			{
				m_CurrentBestMoveHighPriorityUnit = NULL;
				m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
				ExecuteMoveToTarget(pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

// Get a inquisitor to the best city
void CvHomelandAI::ExecuteInquisitorMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns;
		CvPlot* pTarget = m_pPlayer->GetReligionAI()->ChooseInquisitorTargetPlot(pUnit, &iTargetTurns);
		if(pTarget)
		{
			if(pUnit->plot() == pTarget)
			{
				pUnit->PushMission(CvTypes::getMISSION_REMOVE_HERESY());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Removing heresy, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}
			else if(iTargetTurns < 1)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

				if(pUnit->plot() == pTarget && pUnit->canMove())
				{
					pUnit->PushMission(CvTypes::getMISSION_REMOVE_HERESY());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Move to remove heresy, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					CvAssertMsg(false, "Internal error with Inquisitor AI move, contact Ed.");
				}
			}
			else
			{
				m_CurrentBestMoveHighPriorityUnit = NULL;
				m_CurrentBestMoveUnit = m_pPlayer->getUnit(it->GetID());
				ExecuteMoveToTarget(pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to plot adjacent to heresy removal city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// Moves a spaceship part to the capital (or into the spaceship if already there)
void CvHomelandAI::ExecuteSSPartMoves()
{
	CvCity* pLoopCity;
	CvCity* pCapitalCity = NULL;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if(pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if(!pUnit)
			{
				continue;
			}

			if(pUnit->plot() != pCapitalCity->plot())
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Moving %s to spaceship - now at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// Moves a spaceship part into the spaceship if already there
void CvHomelandAI::ExecuteSSPartAdds()
{
	CvCity* pLoopCity;
	CvCity* pCapitalCity = NULL;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if(pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if(!pUnit)
			{
				continue;
			}

			if(pUnit->plot() == pCapitalCity->plot())
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Adding %s to spaceship, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}

				pUnit->PushMission(CvTypes::getMISSION_SPACESHIP());
				UnitProcessed(pUnit->GetID());
			}

			else if (TurnsToReachTarget(pUnit, pCapitalCity->plot()) == 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
				pUnit->PushMission(CvTypes::getMISSION_SPACESHIP());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("Moving %s to spaceship and adding it directly in", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// Moves a treasure to the capital
void CvHomelandAI::ExecuteTreasureMoves()
{
	CvCity* pLoopCity;
	CvCity* pCapitalCity = NULL;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if(pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if(!pUnit)
			{
				continue;
			}

			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;
				strTemp = pUnit->getUnitInfo().GetDescription();
				strLogString.Format("Moving %s to capital - now at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}

#if defined(MOD_AI_SMART_AIR_TACTICS)
// Similar to interception moves on tacticalAI, grant some interceptions based on number of enemies
void CvHomelandAI::ExecuteAircraftInterceptions()
{
	FStaticVector<CvHomelandUnit, 64, true, c_eCiv5GameplayDLL>::iterator it;
	std::vector<CvPlot*> checkedPlotList;

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		if(pUnit)
		{
			// Am I eligible to intercept?
			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit.pointer()))
			{
				CvPlot* pUnitPlot = pUnit->plot();
				int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange()/*m_iRecruitRange*/, false/*bCountFighters*/, true/*bCountBombers*/);
				int iNumNearbyFighters = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange()/*m_iRecruitRange*/, true/*bCountFighters*/, false/*bCountBombers*/);
				int iNumPlotNumAlreadySet = m_pPlayer->GetTacticalAI()->SamePlotFound(checkedPlotList, pUnitPlot);

				if (iNumNearbyBombers == 1)
				{
					// To at least intercept once if only one bomber found.
					iNumNearbyBombers++;
				}

				int maxInterceptorsWanted = ((iNumNearbyBombers / 2) + (iNumNearbyFighters / 4));

				if (iNumPlotNumAlreadySet < maxInterceptorsWanted)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strTemp;
						strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
						CvString strLogString;
						strLogString.Format("(%d of %d) - Ready to intercept enemy air units at, X: %d, Y: %d", iNumPlotNumAlreadySet, maxInterceptorsWanted, pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}

					checkedPlotList.push_back(pUnitPlot);
					pUnit->PushMission(CvTypes::getMISSION_AIRPATROL());
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
				}
			}
		}
	}
}
#endif

/// Moves an aircraft into an important city near the front to aid its defense (or offense)
void CvHomelandAI::ExecuteAircraftMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		CvPlot* pUnitPlot = pUnit->plot();
		CvUnit* pTransportUnit = NULL;
		CvPlot* pBestPlot = NULL;
		int iMostDangerous = 0;

		// first look for open carrier slots
		int iLoopUnit = 0;
		for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			CvPlot* pLoopUnitPlot = pLoopUnit->plot();

			if(pLoopUnit->getDamage() > (GC.getMAX_HIT_POINTS() / 5))  // this might not be a good place to land
			{
				continue;
			}

			if(pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pLoopUnitPlot->getX(),pLoopUnitPlot->getY()))
			{
				continue;
			}

			if(!pUnit->canLoadUnit(*pLoopUnit, *pLoopUnitPlot))
			{
				continue;
			}

			int iPlotDanger = m_pPlayer->GetPlotDanger(*pLoopUnitPlot);
			if (pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
			{
				iPlotDanger += 5000;
			}
			if(iPlotDanger >= iMostDangerous)
			{
				iMostDangerous = iPlotDanger;
				pBestPlot = pLoopUnitPlot;
				pTransportUnit = pLoopUnit;
			}

		}

		CvCity* pLoopCity;
		int iLoopCity = 0;
		for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			CvPlot* pTarget = pLoopCity->plot();

			if(pLoopCity->getDamage() > (pLoopCity->GetMaxHitPoints() / 5))
			{
				continue;
			}

			if(pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot,pTarget->getX(),pTarget->getY()))
			{
				continue;
			}

			if (pLoopCity->IsRazing())
			{
				continue;
			}

			int iPlotDanger = m_pPlayer->GetPlotDanger(*pTarget);
			if(iPlotDanger >= iMostDangerous)
			{
				iMostDangerous = iPlotDanger;
				pBestPlot = pTarget;
				pTransportUnit = NULL;
			}
		}

		if(pBestPlot && pBestPlot != pUnitPlot)
		{
			pUnit->PushMission(CvTypes::getMISSION_REBASE(), pBestPlot->getX(), pBestPlot->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp, strTemp2;
				strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
				if (pTransportUnit)
				{
					strTemp2 = GC.getUnitInfo(pTransportUnit->getUnitType())->GetDescription();
				}

				if (pBestPlot->getPlotCity())
				{
					strLogString.Format("Rebasing %s to city garrison, X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY());
				}
				else
				{
					strLogString.Format("Rebasing %s onto %s, X: %d, Y: %d", strTemp.GetCString(), strTemp2.GetCString(), pBestPlot->getX(), pBestPlot->getY());
				}
				LogHomelandMessage(strLogString);
			}
		}
		else
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strTemp;
				strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
				CvString strLogString;
				strLogString.Format("No better place to move %s at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}


/// Fleeing to safety for civilian units
#if defined(MOD_AI_SECONDARY_WORKERS)
bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits, bool bSecondary)
#else
bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits)
#endif
{
	int iSearchRange = pUnit->SearchRange(1);

	// Collecting all the possibilities first.
	WeightedPlotVector aBestPlotList;
	aBestPlotList.reserve( ((iSearchRange * 2) + 1) * 2 );

	for(int iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for(int iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iDX, iDY, iSearchRange);
			if(!pLoopPlot)
			{
				continue;
			}

#if defined(MOD_AI_SECONDARY_WORKERS)
			if(!pUnit->PlotValid(pLoopPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
#else
			if(!pUnit->PlotValid(pLoopPlot))
#endif
			{
				continue;
			}

			if(pLoopPlot->isVisibleEnemyUnit(pUnit))
			{
				continue;
			}

			int iValue = 0;
			if(pLoopPlot->getOwner() != NO_PLAYER && GET_PLAYER(pLoopPlot->getOwner()).getTeam() == m_pPlayer->getTeam())
			{
				// if this is within our territory, provide a minor benefit
				iValue += 1;
			}

#if defined(MOD_AI_SECONDARY_WORKERS)
			if (bSecondary && pUnit->getDomainType() == DOMAIN_LAND && pLoopPlot->isWater()) {
				// being embarked is NOT safe!
				iValue -= 100;
			}
#endif

			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity && pCity->getTeam() == pUnit->getTeam())
			{
				iValue += pCity->getStrengthValue() * (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDamage());
			}
			else if(!bIgnoreUnits)
			{
				IDInfo* pUnitNode = pLoopPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if(pLoopUnit && pLoopUnit->getOwner() == pUnit->getOwner())
					{
						if(pLoopUnit->IsCanDefend())
						{
							if(pLoopUnit != pUnit)
							{
								if(pLoopUnit->isWaiting() || !(pLoopUnit->canMove()))
								{
									iValue += pLoopUnit->GetMaxDefenseStrength(pLoopPlot, NULL) * pLoopUnit->GetCurrHitPoints();
								}
							}
						}
					}
				}
			}

			iValue -= m_pPlayer->GetPlotDanger(*pLoopPlot);

			aBestPlotList.push_back(pLoopPlot, iValue);
		}
	}

	// Now loop through the sorted score list and go to the best one we can reach in one turn.
	CvPlot* pBestPlot = NULL;
	uint uiListSize;
	if ((uiListSize = aBestPlotList.size()) > 0)
	{
		aBestPlotList.SortItems();	// highest score will be first.
		for (uint i = 0; i < uiListSize; ++i )	
		{
			CvPlot* pPlot = aBestPlotList.GetElement(i);

			int iPathTurns;
			if(!pUnit->GeneratePath(pPlot, MOVE_UNITS_IGNORE_DANGER, true, &iPathTurns))
			{
				continue;
			}

			// if we can't get there this turn, forget it
			if(iPathTurns > 1)
			{
				continue;
			}

			pBestPlot = pPlot;
			break;
		}
	}

	if(pBestPlot != NULL)
	{
		if(pUnit->atPlot(*pBestPlot))
		{
			if (pUnit->canHold(pBestPlot))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("%s (%d) tried to move to safety but is at the best spot, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage(strLogString);
				}

				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				return true;
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strLogString.Format("%s (%d) tried to move to safety but cannot hold in current location, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage(strLogString);
				}
				pUnit->SetAutomateType(NO_AUTOMATE);
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;
				strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
				strLogString.Format("%s (%d) moving to safety, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
				LogHomelandMessage(strLogString);
			}

			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), MOVE_UNITS_IGNORE_DANGER);
			return true;
		}
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = pUnit->getUnitInfo().GetDescription();
			strLogString.Format("%s (%d) tried to move to a safe point but couldn't find a good place to go", strTemp.GetCString(), pUnit->GetID());
			LogHomelandMessage(strLogString);
		}
	}


	return false;
}

// Get a trade unit and send it to a city!
void CvHomelandAI::ExecuteTradeUnitMoves()
{
	CvTradeAI* pkTradeAI = m_pPlayer->GetTradeAI();

	static TradeConnectionList aTradeConnections; // slewis - added static to work around the stack limit
	pkTradeAI->PrioritizeTradeRoutes(aTradeConnections);

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iOriginPlotIndex = MAX_INT;
		int iDestPlotIndex = MAX_INT;
		TradeConnectionType eTradeConnectionType = NUM_TRADE_CONNECTION_TYPES;
		bool bDisband = false;
		bool bSuccess = m_pPlayer->GetTradeAI()->ChooseTradeUnitTargetPlot(pUnit, iOriginPlotIndex, iDestPlotIndex, eTradeConnectionType, bDisband, aTradeConnections);
		if (bSuccess)
		{
			if (bDisband)
			{
				pUnit->kill(true);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Disbanding unit because we want different domains for our trade units");
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				CvPlot* pOriginPlot = GC.getMap().plotByIndex(iOriginPlotIndex);
				if (pOriginPlot != pUnit->plot())
				{
					pUnit->PushMission(CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY(), pOriginPlot->getX(), pOriginPlot->getY());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvPlot* pPlot = GC.getMap().plotByIndex(iDestPlotIndex);
						CvString strLogString;
						strLogString.Format("Changing trade route home city, X: %d, Y: %d", pPlot->getX(), pPlot->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					pUnit->PushMission(CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE(), iDestPlotIndex, eTradeConnectionType);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvPlot* pPlot = GC.getMap().plotByIndex(iDestPlotIndex);
						CvString strLogString;

						switch (eTradeConnectionType)
						{
						case TRADE_CONNECTION_FOOD:
							strLogString.Format("Establishing trade route, X: %d, Y: %d, food", pPlot->getX(), pPlot->getY());
							break;
						case TRADE_CONNECTION_INTERNATIONAL:
							strLogString.Format("Establishing trade route, X: %d, Y: %d, gold", pPlot->getX(), pPlot->getY());
							break;
						case TRADE_CONNECTION_PRODUCTION:
							strLogString.Format("Establishing trade route, X: %d, Y: %d, production", pPlot->getX(), pPlot->getY());
							break;
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
						case TRADE_CONNECTION_WONDER_RESOURCE:
							strLogString.Format("Establishing trade route, X: %d, Y: %d, wonder resource", pPlot->getX(), pPlot->getY());
							break;
#endif
						}

						LogHomelandMessage(strLogString);
					}
				}
			}
		}
		else
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->finishMoves();			
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Trade unit idling");
				LogHomelandMessage(strLogString);
			}
		}
	}
}

// Get an archaeologist and send it to an antiquity site
void CvHomelandAI::ExecuteArchaeologistMoves()
{
#if defined(MOD_AI_SMART_ARCHAEOLOGISTS)
	int iUnassignedArchaeologists = 0;
#endif

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		CvPlot* pTarget = FindArchaeologistTarget(pUnit);
		if (pTarget)
		{
			BuildTypes eBuild = (BuildTypes)GC.getInfoTypeForString("BUILD_ARCHAEOLOGY_DIG");
			if(pUnit->UnitPathTo(pTarget->getX(), pTarget->getY(), 0) > 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Archaeologist moving to site at, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}

				if(pUnit->plot() == pTarget)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Archaeologist creating dig at, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					pUnit->finishMoves();
				}
			}
			// Delete this unit from those we have to move
			UnitProcessed(pUnit->GetID());
		}
#if defined(MOD_AI_SMART_ARCHAEOLOGISTS)
		else
		{
			iUnassignedArchaeologists++;
		}
#endif
	}

#if defined(MOD_AI_SMART_ARCHAEOLOGISTS)
	// Unassigned archaeologists due to no valid targets, check against the possible targets left and scrap one if there are too many.
	if (MOD_AI_SMART_ARCHAEOLOGISTS && iUnassignedArchaeologists > 0)
	{
		int iPossibleSites = ((m_TargetedAntiquitySites.size() / 5) + 1);

		if (iUnassignedArchaeologists > iPossibleSites)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits.end()->GetID());

			if (pUnit)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Idle Archaeologist scrapped at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}

				pUnit->scrap();
			}
		}
	}
#endif
}

/// Don't allow adjacent tiles to both be sentry points
void CvHomelandAI::EliminateAdjacentSentryPoints()
{
	// First, sort the sentry points by priority
	std::stable_sort(m_TargetedSentryPoints.begin(), m_TargetedSentryPoints.end());

	// Create temporary copy of list
	std::vector<CvHomelandTarget> tempPoints;
	tempPoints = m_TargetedSentryPoints;

	// Clear out main list
	m_TargetedSentryPoints.clear();

	// Loop through all points in copy
	std::vector<CvHomelandTarget>::iterator it, it2;
	for(it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;

		// Is it adjacent to a point in the main list?
		for(it2 = m_TargetedSentryPoints.begin(); it2 != m_TargetedSentryPoints.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if(!bFoundAdjacent)
		{
			m_TargetedSentryPoints.push_back(*it);
		}
	}
}

/// Don't allow adjacent tiles to both be mobile reserve muster points
void CvHomelandAI::EliminateAdjacentHomelandRoads()
{
	// Create temporary copy of list
	std::vector<CvHomelandTarget> tempPoints;
	tempPoints = m_TargetedHomelandRoads;

	// Clear out main list
	m_TargetedHomelandRoads.clear();

	// Loop through all points in copy
	std::vector<CvHomelandTarget>::iterator it, it2;
	for(it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;

		// Is it adjacent to a point in the main list?
		for(it2 = m_TargetedHomelandRoads.begin(); it2 != m_TargetedHomelandRoads.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if(!bFoundAdjacent)
		{
			m_TargetedHomelandRoads.push_back(*it);
		}
	}
}

/// Fills m_CurrentMoveUnits with all units in same area
bool CvHomelandAI::FindWorkersInSameArea(CvPlot* pTarget, BuildTypes eBuild)
{
	bool rtnValue = false;
	ClearCurrentMoveUnits();

	// Loop through all units available to homeland AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit)
		{
			// Civilians only
			if(pLoopUnit->IsCanAttack())
			{
				continue;
			}

			// Can this worker make the right improvement?
			if(pLoopUnit->canBuild(pTarget, eBuild))
			{
				CvHomelandUnit unit;
				int iMoves = TurnsToReachTarget(pLoopUnit.pointer(), pTarget);

				if(iMoves != MAX_INT)
				{
					unit.SetID(pLoopUnit->GetID());
					unit.SetMovesToTarget(iMoves);
					m_CurrentMoveUnits.push_back(unit);
					rtnValue = true;
				}
			}
		}
	}

	// Now sort them in the order we'd like them to move
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Finds both high and normal priority units we can use for this homeland move (returns true if at least 1 unit found)
bool CvHomelandAI::FindUnitsForThisMove(AIHomelandMove eMove, bool bFirstTime)
{
	bool rtnValue = false;

	if(bFirstTime)
	{
		ClearCurrentMoveUnits();
		ClearCurrentMoveHighPriorityUnits();

		// Loop through all units available to homeland AI this turn
		for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			UnitHandle pLoopUnit = m_pPlayer->getUnit(*it);
			if(pLoopUnit && !pLoopUnit->isHuman())
			{
				// Civilians aren't useful for any of these moves
				if(!pLoopUnit->IsCombatUnit())
				{
					continue;
				}

				// Scouts aren't useful unless recon is entirely shut off
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE && m_pPlayer->GetEconomicAI()->GetReconState() != RECON_STATE_ENOUGH)
				{
					continue;
				}

				bool bSuitableUnit = false;
				bool bHighPriority = false;

				switch(eMove)
				{
				case AI_HOMELAND_MOVE_GARRISON:
				case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
					// Want to put ranged units in cities to give them a ranged attack
					if(pLoopUnit->isRanged() && !pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_CITY_BOMBARD))
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}

					// Don't use non-combatants
					else if(pLoopUnit->IsCanAttack())
					{
						// Don't put units with a combat strength boosted from promotions in cities, these boosts are ignored
						if(pLoopUnit->getDefenseModifier() == 0 &&
						        pLoopUnit->getAttackModifier() == 0 &&
						        pLoopUnit->getExtraCombatPercent() == 0)
						{
							bSuitableUnit = true;
						}
					}
					break;

				case AI_HOMELAND_MOVE_SENTRY:
					// No ranged units as sentries
					if(!pLoopUnit->isRanged() && !pLoopUnit->noDefensiveBonus())
					{
						bSuitableUnit = true;

						// Units with extra sight are especially valuable
						if(pLoopUnit->getExtraVisibilityRange() > 0)
						{
							bHighPriority = true;
						}
					}
					else if(pLoopUnit->noDefensiveBonus() && pLoopUnit->getExtraVisibilityRange() > 0)
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}
					break;

				case AI_HOMELAND_MOVE_MOBILE_RESERVE:
					// Ranged units are excellent in the mobile reserve as are fast movers
					if(pLoopUnit->isRanged() || pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}
					else if(pLoopUnit->IsCanAttack())
					{
						bSuitableUnit = true;
					}
					break;

				case AI_HOMELAND_MOVE_ANCIENT_RUINS:
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
					break;
				}

				// If unit was suitable, add it to the proper list
				if(bSuitableUnit)
				{
					CvHomelandUnit unit;
					unit.SetID(pLoopUnit->GetID());

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

	// Second time through, just make sure all units are still in list of current units to process
	else
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL > tempList;
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		std::list<int>::iterator it2;

		// Normal priority units
		tempList = m_CurrentMoveUnits;
		ClearCurrentMoveUnits();
		for(it = tempList.begin(); it != tempList.end(); ++it)
		{
			it2 = find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), (int)it->GetID());
			if(it2 != m_CurrentTurnUnits.end())
			{
				m_CurrentMoveUnits.push_back(*it);
				rtnValue = true;
			}
		}

		// High priority units
		tempList = m_CurrentMoveHighPriorityUnits;
		ClearCurrentMoveHighPriorityUnits();
		for(it = tempList.begin(); it != tempList.end(); ++it)
		{
			it2 = find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), (int)it->GetID());
			if(it2 != m_CurrentTurnUnits.end())
			{
				m_CurrentMoveHighPriorityUnits.push_back(*it);
				rtnValue = true;
			}
		}
	}

	return rtnValue;
}

/// See if there is an adjacent plot we can wander to
CvPlot* CvHomelandAI::FindPatrolTarget(CvUnit* pUnit)
{
	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pUnit->PlotValid(pAdjacentPlot))
			{
				if(!(pAdjacentPlot->isVisibleEnemyUnit(pUnit)))
				{
					if(pUnit->GeneratePath(pAdjacentPlot, 0, true))
					{
						iValue = (1 + GC.getGame().getJonRandNum(10000, "AI Patrol"));

						// Prefer wandering in our own territory
						if(pAdjacentPlot->getOwner() == pUnit->getOwner())
						{
							iValue += 10000;
						}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
						if(MOD_DIPLOMACY_CIV4_FEATURES && pAdjacentPlot->IsAvoidMovement(m_pPlayer->GetID()))
						{
							iValue -= 100;
						}
#endif

						if(GC.getLogging() && GC.getAILogging()){
							CvString strLogString;
							strLogString.Format("Adjacent Patrol Plot Score, %i, %i, %i", iValue, pAdjacentPlot->getX(), pAdjacentPlot->getY());
							LogPatrolMessage(strLogString, pUnit);
						}

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pAdjacentPlot;
						}
					}
					else
					{
						if(GC.getLogging() && GC.getAILogging()){
							CvString strLogString;
							strLogString.Format("Adjacent Patrol Plot !GeneratePath(), , %i, %i", pAdjacentPlot->getX(), pAdjacentPlot->getY());
							LogPatrolMessage(strLogString, pUnit);
						}
					}
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging()){
						CvString strLogString;
						strLogString.Format("Adjacent Patrol Plot !isVisibleEnemyUnit(), , %i, %i", pAdjacentPlot->getX(), pAdjacentPlot->getY());
						LogPatrolMessage(strLogString, pUnit);
					}
				}
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging()){
					CvString strLogString;
					strLogString.Format("Adjacent Patrol Plot not valid, , %i, %i", pAdjacentPlot->getX(), pAdjacentPlot->getY());
					LogPatrolMessage(strLogString, pUnit);
				}
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging()){
				CvString strLogString;
				strLogString.Format("Adjacent Patrol Plot not found");
				LogPatrolMessage(strLogString, pUnit);
			}
		}
	}

	if(pBestPlot != NULL)
	{
		if(GC.getLogging() && GC.getAILogging()){
			CvString strLogString;
			strLogString.Format("Patrol Target FOUND, %i, %i, %i", iBestValue, pBestPlot->getX(), pBestPlot->getY());
			LogPatrolMessage(strLogString, pUnit);
		}

		CvAssert(!pUnit->atPlot(*pBestPlot));
		return pBestPlot;
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging()){
			CvString strLogString;
			strLogString.Format("Patrol Target NOT FOUND");
			LogPatrolMessage(strLogString, pUnit);
		}
	}

	return NULL;
}

//	---------------------------------------------------------------------------
/// Get the closest 
bool CvHomelandAI::GetClosestUnitByTurnsToTarget(CvHomelandAI::MoveUnitsArray &kMoveUnits, CvPlot* pTarget, int iMaxTurns, CvUnit** ppClosestUnit, int* piClosestTurns)
{
	CvUnit* pBestUnit = NULL;
	int iMinTurns = MAX_INT;
	MoveUnitsArray::iterator it;

	int iFailedPaths = 0;
	// If we see this many failed pathing attempts, we assume no unit can get to the target
	const int MAX_FAILED_PATHS = 2;
	// If the last failed pathing attempt was this far (raw distance) from the target, we assume no one can reach the target, even if we have not reached MAX_FAILED_PATHS
	const int EARLY_OUT_FAILED_PATH_DISTANCE = 12;

	// Now go through and figure out the actual number of turns, and as a result, even if it can get there at all.
	// We will try and do as few as possible by stopping if we find a unit that can make it in one turn.
	for(it = kMoveUnits.begin(); it != kMoveUnits.end(); ++it)
	{
		UnitHandle pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if (pLoopUnit)
		{
			int iDistance = it->GetMovesToTarget();	// Raw distance
			if (iDistance == MAX_INT)
				continue;
			int iMoves = TurnsToReachTarget(pLoopUnit.pointer(), pTarget);
			it->SetMovesToTarget(iMoves);
			// Did we make it at all?
			if (iMoves != MAX_INT)
			{
				// Reasonably close?
				if (iDistance == 0 || (iMoves <= iDistance && iMoves <= iMaxTurns && iMoves < iMinTurns))
				{
					pBestUnit = pLoopUnit.pointer();
					iMinTurns = iMoves;
					break;
				}

				if (iMoves < iMinTurns)
				{
					pBestUnit = pLoopUnit.pointer();
					iMinTurns = iMoves;
				}

				// Were we far away?  If so, this is probably the best we are going to do
				if (iDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE())
					break;
			}
			else
			{
				++iFailedPaths;
				if (iFailedPaths >= MAX_FAILED_PATHS)
					break;
				if (iDistance >= EARLY_OUT_FAILED_PATH_DISTANCE)
					break;
			}
		}
	}

	*ppClosestUnit = pBestUnit;
	*piClosestTurns = iMinTurns;
	return pBestUnit != NULL;
}

//	---------------------------------------------------------------------------
/// Compute the best unit to reach a target in the current normal and high priority move list
bool CvHomelandAI::GetBestUnitToReachTarget(CvPlot* pTarget, int iMaxTurns)
{
	AI_PERF_FORMAT("Homeland-GetBest-perf.csv", ("GetBestUnitToReachTarget, %d, %d, Turn %03d, %s", pTarget->getX(), pTarget->getY(), GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	UnitHandle pLoopUnit;
	MoveUnitsArray::iterator it;

	// Get the raw distance for all units
	int iTargetX = pTarget->getX();
	int iTargetY = pTarget->getY();
	
	// Normal priority units
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if(pLoopUnit)
		{
			// Make sure domain matches
			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
				pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}

			// Make sure we can move into the destination.  The path finder will do a similar check near the beginning, but it is best to get this out of the way before then
			if(!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}

			int iPlotDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iTargetX, iTargetY);
			it->SetMovesToTarget(iPlotDistance);
		}
	}

	// High priority units
	for(it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if(pLoopUnit)
		{
			// Make sure domain matches
			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
				pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}

			// Make sure we can move into the destination.  The path finder will do a similar check near the beginning, but it is best to get this out of the way before then
			if(!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}

			int iPlotDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iTargetX, iTargetY);
			it->SetMovesToTarget(iPlotDistance);
		}
	}

	// Sort by raw distance
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());
	std::stable_sort(m_CurrentMoveHighPriorityUnits.begin(), m_CurrentMoveHighPriorityUnits.end());

	// Find the one with the best true moves distance
	GetClosestUnitByTurnsToTarget(m_CurrentMoveUnits, pTarget, iMaxTurns, &m_CurrentBestMoveUnit, &m_iCurrentBestMoveUnitTurns);
	GetClosestUnitByTurnsToTarget(m_CurrentMoveHighPriorityUnits, pTarget, iMaxTurns, &m_CurrentBestMoveHighPriorityUnit, &m_iCurrentBestMoveHighPriorityUnitTurns);

	return m_CurrentBestMoveHighPriorityUnit != NULL || m_CurrentBestMoveUnit != NULL;
}

/// Move up to our target avoiding our own units if possible
bool CvHomelandAI::MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, bool bLand)
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
					// Find a path to this space
					if(pUnit->GeneratePath(pLoopPlot))
					{
						// Go ahead with mission
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlot->getX(), pLoopPlot->getY());
						return true;
					}
				}
			}
		}
	}
	return false;
}

/// Choose a city to build the next wonder
CvCity* CvHomelandAI::ChooseBestFreeWonderCity(BuildingTypes eWonder, UnitHandle pEngineer)
{
	CvCity* pBestCity = NULL;
	CvCity* pLoopCity;
	int iLoop;
	int iBestTurns = MAX_INT;
	int iTurns;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canConstruct(eWonder))
		{
			iTurns = TurnsToReachTarget(pEngineer, pLoopCity->plot(), true /*bReusePaths*/);
			if(iTurns < iBestTurns)
			{
				pBestCity = pLoopCity;
				iBestTurns = iTurns;
			}
		}
	}

	return pBestCity;
}

/// Find best target for this archaeologist
CvPlot* CvHomelandAI::FindArchaeologistTarget(CvUnit *pUnit)
{
	CvPlot *pBestTarget = NULL;
	int iBestTurns = MAX_INT;

	// Reverse the logic from most of the Homeland moves; for this we'll loop through units and find the best targets for them (instead of vice versa)
	std::vector<CvHomelandTarget>::iterator it;
	for (it = m_TargetedAntiquitySites.begin(); it != m_TargetedAntiquitySites.end(); it++)
	{
		CvPlot* pTarget = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
		if (m_pPlayer->GetPlotDanger(*pTarget) == 0)
		{
			int iTurns = TurnsToReachTarget(pUnit, pTarget);

			if (iTurns < iBestTurns)
			{
				pBestTarget = pTarget;
				iBestTurns = iTurns;
			}
		}
	}

	// Erase this site from future contention
	if (pBestTarget)
	{
		for (it = m_TargetedAntiquitySites.begin(); it != m_TargetedAntiquitySites.end(); it++)
		{
			if (it->GetTargetX() == pBestTarget->getX() && it->GetTargetY() == pBestTarget->getY())
			{
				m_TargetedAntiquitySites.erase(it);
				break;
			}
		}
	}

	return pBestTarget;
}

/// Find the plot where we want to pillage
/// Log current status of the operation
void CvHomelandAI::LogHomelandMessage(CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

void CvHomelandAI::LogPatrolMessage( CvString& strMsg, CvUnit* pPatrolUnit )
{
	if(!pPatrolUnit || !(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	// Open the log file
	CvString strFileName = "PlayerHomelandAIPatrolLog.csv";
	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

	CvString strLog, strTemp;

	CvString strPlayerName;
	strPlayerName = m_pPlayer->getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
	strLog += strTemp;

	strTemp.Format("%s (%d),", pPatrolUnit->getUnitInfo().GetDescription(), pPatrolUnit->GetID());
	strLog += strTemp;

	strLog += strMsg;
	pLog->Msg(strLog);
}

/// Remove a unit that we've allocated from list of units to move this turn
void CvHomelandAI::UnitProcessed(int iID)
{
	CvUnit* pUnit;

	m_CurrentTurnUnits.remove(iID);
	pUnit = m_pPlayer->getUnit(iID);

	CvAssert(pUnit);
	if(!pUnit) return;

	pUnit->SetTurnProcessed(true);
}

#if defined(MOD_AI_SECONDARY_WORKERS)
bool CvHomelandAI::ExecuteWorkerMove(CvUnit* pUnit, bool bSecondary)
#else
bool CvHomelandAI::ExecuteWorkerMove(CvUnit* pUnit)
#endif
{
#if defined(MOD_AI_SECONDARY_WORKERS)
	// if (bSecondary) CUSTOMLOG("ExecuteWorkerMove(secondary) for %s at (%i, %i)", pUnit->getName().c_str(), pUnit->plot()->getX(), pUnit->plot()->getY());
#endif
	const UINT ciDirectiveSize = 1;
	BuilderDirective aDirective[ ciDirectiveSize ];

	// evaluator
#if defined(MOD_AI_SECONDARY_WORKERS)
	bool bHasDirective = m_pPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pUnit, aDirective, ciDirectiveSize, false, false, bSecondary);
#else
	bool bHasDirective = m_pPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pUnit, aDirective, ciDirectiveSize);
#endif
	if(bHasDirective)
	{
		switch(aDirective[0].m_eDirective)
		{
		case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
		case BuilderDirective::BUILD_IMPROVEMENT:
		case BuilderDirective::REPAIR:
		case BuilderDirective::BUILD_ROUTE:
		case BuilderDirective::CHOP:
		case BuilderDirective::REMOVE_ROAD:
		{
			CvPlot* pPlot = GC.getMap().plot(aDirective[0].m_sX, aDirective[0].m_sY);
			MissionTypes eMission = NO_MISSION;
			if(pUnit->getX() == aDirective[0].m_sX && pUnit->getY() == aDirective[0].m_sY)
			{
#if defined(MOD_AI_SECONDARY_WORKERS)
				// if (bSecondary) CUSTOMLOG("  ... build %i", ((int) aDirective[0].m_eDirective));
#endif
				eMission = CvTypes::getMISSION_BUILD();
			}
			else
			{
#if defined(MOD_AI_SECONDARY_WORKERS)
				// if (bSecondary) CUSTOMLOG("  ... move to (%i, %i)", ((int) aDirective[0].m_sX), ((int) aDirective[0].m_sY));
#endif
				eMission = CvTypes::getMISSION_MOVE_TO();
			}

			if(GC.getLogging() && GC.GetBuilderAILogging())
			{
				// Open the log file
				CvString strFileName = "BuilderTaskingLog.csv";
				FILogFile* pLog;
				pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

				// write in data
				CvString strLog;
				CvString strTemp;

				CvString strPlayerName;
				strPlayerName = m_pPlayer->getCivilizationShortDescription();
				strLog += strPlayerName;
				strLog += ",";

				strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
				strLog += strTemp;

				strTemp.Format("%d,", pUnit->GetID()); // unit id
				strLog += strTemp;

				switch(aDirective[0].m_eDirective)
				{
				case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
					strLog += "On resource,";
					break;
				case BuilderDirective::BUILD_IMPROVEMENT:
					strLog += "On plot,";
					break;
				case BuilderDirective::REPAIR:
					strLog += "Repairing,";
					break;
				case BuilderDirective::BUILD_ROUTE:
					strLog += "Building route,";
					break;
				case BuilderDirective::CHOP:
					strLog += "Removing resource for production,";
					break;
				case BuilderDirective::REMOVE_ROAD:
					strLog += "Removing road,";
					break;
				}

				if(eMission == CvTypes::getMISSION_BUILD())
				{
					if(aDirective[0].m_eDirective == BuilderDirective::REPAIR)
					{
						if(pPlot->IsImprovementPillaged())
						{
							strLog += "Repairing improvement";
						}
						else
						{
							strLog += "Repairing route";
						}
					}
					else if(aDirective[0].m_eDirective == BuilderDirective::BUILD_ROUTE)
					{
						strLog += "Building route,";
					}
					else if(aDirective[0].m_eDirective == BuilderDirective::BUILD_IMPROVEMENT || aDirective[0].m_eDirective == BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE)
					{
						strLog += "Building improvement,";
					}
					else if(aDirective[0].m_eDirective == BuilderDirective::CHOP)
					{
						strLog += "Removing feature for production,";
					}
					else
					{
						strLog += "Removing road,";
					}
				}
				else
				{
					strLog += "Moving to location,";
				}

				pLog->Msg(strLog);
			}

			if(eMission == CvTypes::getMISSION_BUILD())
			{
				// check to see if we already have this mission as the unit's head mission
				bool bPushMission = true;
				const MissionData* pkMissionData = pUnit->GetHeadMissionData();
				if(pkMissionData != NULL)
				{
					if(pkMissionData->eMissionType == eMission && pkMissionData->iData1 == aDirective[0].m_eBuild)
					{
						bPushMission = false;
					}
				}

				if(bPushMission)
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), aDirective[0].m_eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);
				}

				CvAssertMsg(!pUnit->ReadyToMove(), "Worker did not do their mission this turn. Could cause game to hang.");
				if(pUnit->ReadyToMove())
				{
					pUnit->finishMoves();
				}
				UnitProcessed(pUnit->GetID());
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), aDirective[0].m_sX, aDirective[0].m_sY, 0, false, false, MISSIONAI_BUILD, pPlot);
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());
			}

			return true;
		}
		break;
		}
	}
	else
	{
		if(GC.getLogging() && GC.GetBuilderAILogging())
		{
			CvString strFileName = "BuilderTaskingLog.csv";
			FILogFile* pLog;
			pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
			pLog->Msg("builder has no directive");
		}
	}

	return false;
}

bool CvHomelandAI::ExecuteCultureBlast(CvUnit* pUnit)
{
	if (pUnit->canGivePolicies(pUnit->plot()))
	{
		pUnit->PushMission(CvTypes::getMISSION_GIVE_POLICIES());
		UnitProcessed(pUnit->GetID());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = pUnit->getUnitInfo().GetDescription();
			strLogString.Format("%s blasted culture with unit at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
	}
	else
	{
		MoveCivilianToSafety(pUnit);
	}

	return true;
}

bool CvHomelandAI::ExecuteGoldenAgeMove(CvUnit* pUnit)
{
	if (pUnit->canGoldenAge(pUnit->plot()))
	{
		pUnit->PushMission(CvTypes::getMISSION_GOLDEN_AGE());
		UnitProcessed(pUnit->GetID());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = pUnit->getUnitInfo().GetDescription();
			strLogString.Format("%s started golden age with unit at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
	}
	else
	{
		MoveCivilianToSafety(pUnit);
	}

	return true;
}

bool CvHomelandAI::IsValidExplorerEndTurnPlot(const CvUnit* pUnit, CvPlot* pPlot) const
{
	if(pUnit->atPlot(*pPlot))
	{
		return false;
	}

	if(!pPlot->isRevealed(pUnit->getTeam()))
	{
		return false;
	}

	DomainTypes eDomain = pUnit->getDomainType();

	if(pPlot->area() != pUnit->area())
	{
		if(!pUnit->CanEverEmbark())
		{
			if(!(eDomain == DOMAIN_SEA && pPlot->isWater()))
			{
				return false;
			}
		}
	}

	// don't let the auto-explore end it's turn in a city
	CvCity* pCity = pPlot->getPlotCity();
	if(pCity && pCity->getOwner() != pUnit->getOwner())
	{
		return false;
	}

	if(!pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
	{
		return false;
	}

	return true;
}

/// Move an exploring unit to a designated target (special function exposed to Lua)
bool CvHomelandAI::ExecuteSpecialExploreMove(CvUnit* pUnit, CvPlot* pTargetPlot)
{
	bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
	if(bCanFindPath)
	{
		CvPlot* pPlot = GC.getPathFinder().GetPathEndTurnPlot();
		if(pPlot)
		{
			CvAssert(!pUnit->atPlot(*pPlot));
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("UnitID: %d Moving to script explore target, X: %d, Y: %d, from X: %d Y: %d", pUnit->GetID(), pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pPlot);
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			return true;
		}
	}
	return false;
}

/// Build log filename
CvString CvHomelandAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PlayerHomelandAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PlayerHomelandAILog.csv";
	}

	return strLogName;
}

//	---------------------------------------------------------------------------
void CvHomelandAI::ClearCurrentMoveUnits()
{
	m_CurrentMoveUnits.clear();
	m_CurrentBestMoveUnit = NULL;
	m_iCurrentBestMoveUnitTurns = MAX_INT;
}

//	---------------------------------------------------------------------------
void CvHomelandAI::ClearCurrentMoveHighPriorityUnits()
{
	m_CurrentMoveHighPriorityUnits.clear();
	m_CurrentBestMoveHighPriorityUnit = NULL;
	m_iCurrentBestMoveHighPriorityUnitTurns = MAX_INT;
}
//	---------------------------------------------------------------------------
CvUnit* m_CurrentBestHighPriorityMoveUnit;
int m_iCurrentBestHighPriorityMoveUnitTurns;

// HELPER FUNCTIONS

/// Sort CvHomelandUnit by a non-standard criteria
bool HomelandAIHelpers::CvHomelandUnitAuxIntSort(CvHomelandUnit obj1, CvHomelandUnit obj2)
{
	return obj1.GetAuxIntData() < obj2.GetAuxIntData();
}

bool HomelandAIHelpers::CvHomelandUnitAuxIntReverseSort(CvHomelandUnit obj1, CvHomelandUnit obj2)
{
	return obj1.GetAuxIntData() > obj2.GetAuxIntData();
}