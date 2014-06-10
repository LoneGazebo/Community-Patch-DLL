/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
CvHomelandAI::CvHomelandAI (void)
{
}

/// Destructor
CvHomelandAI::~CvHomelandAI (void)
{
	Uninit();
}

/// Initialize
void CvHomelandAI::Init(CvPlayer *pPlayer)
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

	kStream >> m_MovePriorityTurn;
}

/// Serialization write
void CvHomelandAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_MovePriorityTurn;
}

/// Mark all the units that will be under tactical AI control this turn
void CvHomelandAI::RecruitUnits()
{
	CvUnit *pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();

	// Loop through our units
	for (pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Never want immobile/dead units or ones that have already moved
		if (!pLoopUnit->TurnProcessed() && !pLoopUnit->isDelayedDeath() && pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN && pLoopUnit->canMove())
		{
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
	}
}

/// Mark all the units that will be under tactical AI control this turn
void CvHomelandAI::FindAutomatedUnits()
{
	CvUnit *pLoopUnit;
	int iLoop;

	m_CurrentTurnUnits.clear();

	// Loop through our units
	for (pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit->IsAutomated() && !pLoopUnit->TurnProcessed() && pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN && pLoopUnit->canMove())
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
	if (!m_CurrentTurnUnits.empty())
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

	m_MovePriorityList.clear();
	m_MovePriorityTurn = GC.getGame().getGameTurn();

	// Find required flavor values
	for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			iFlavorDefense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorDefense = (int)((double)iFlavorDefense * m_fFlavorDampening);
		}
		if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			iFlavorOffense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorOffense = (int)((double)iFlavorOffense * m_fFlavorDampening);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iFlavorExpand = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_RECON")
		{
			iFlavorExplore = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorExplore = (int)((double)iFlavorExplore * m_fFlavorDampening);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GOLD")
		{
			iFlavorGold = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
		{
			iFlavorScience = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_WONDER")
		{
			iFlavorWonder = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
		{
			iFlavorMilitaryTraining = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
	}

	// Loop through each possible homeland move (other than "none" or "unassigned")
	for (int iI = AI_HOMELAND_MOVE_UNASSIGNED + 1; iI < NUM_AI_HOMELAND_MOVES; iI++)
	{
		// Set base value
		switch ((AIHomelandMove)iI)
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
			if (m_pPlayer->GetPlayerPolicies()->HasPolicyEncouragingGarrisons())
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
		case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ARTIST_GOLDEN_AGE();
			break;
		case AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH();
			break;
		case AI_HOMELAND_MOVE_ENGINEER_HURRY:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY();
			break;
		case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON();
			break;
		case AI_HOMELAND_MOVE_SPACESHIP_PART:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART();
			break;
		case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT();
			break;
		case AI_HOMELAND_MOVE_TREASURE:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_TREASURE();
			break;
		}

		// Make sure base priority is not negative
		if (iPriority >= 0)
		{
			// Now add in the appropriate flavor value
			switch ((AIHomelandMove)iI)
			{
				// Defensive moves
			case AI_HOMELAND_MOVE_GARRISON:
			case AI_HOMELAND_MOVE_HEAL:
			case AI_HOMELAND_MOVE_TO_SAFETY:
			case AI_HOMELAND_MOVE_MOBILE_RESERVE:
			case AI_HOMELAND_MOVE_SENTRY:
			case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
			case AI_HOMELAND_MOVE_TREASURE:
			case AI_HOMELAND_MOVE_SPACESHIP_PART:   // Here so it remains lower priority than AI_HOMELAND_MOVE_TO_SAFETY
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
				iPriority += iFlavorMilitaryTraining;
				break;

			case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
				iPriority += iFlavorGold;
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
			}

			// Store off this move and priority
			CvHomelandMove move;
			move.m_eMoveType = (AIHomelandMove)iI;
			move.m_iPriority = iPriority;
			m_MovePriorityList.push_back (move);
		}
	}

	// Now sort the moves in priority order
	std::stable_sort (m_MovePriorityList.begin(), m_MovePriorityList.end());
}

/// Make lists of everything we might want to target with the homeland AI this turn
void CvHomelandAI::FindHomelandTargets()
{
	int iI;
	CvPlot *pLoopPlot;
	CvHomelandTarget newTarget;

	// Clear out target lists since we rebuild them each turn
	m_TargetedCities.clear();
	m_TargetedSentryPoints.clear();
	m_TargetedForts.clear();
	m_TargetedNavalResources.clear();
	m_TargetedHomelandRoads.clear();
	m_TargetedAncientRuins.clear();

	TeamTypes eTeam = m_pPlayer->getTeam();

	// Look at every tile on map
	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	for (iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if (pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{
			// Have a ...
			// ... friendly city?
			CvCity* pCity = pLoopPlot->getPlotCity();
			if (pCity != NULL)
			{
				if (m_pPlayer->GetID() == pCity->getOwner())
				{
					// Don't send another unit if the tactical AI already sent a garrison here
					UnitHandle pUnit = pLoopPlot->getBestDefender(m_pPlayer->GetID());
					if (!pUnit || !pUnit->isUnderTacticalControl())
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_CITY);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData((void *)pCity);
						newTarget.SetAuxIntData(pCity->getThreatValue());
						m_TargetedCities.push_back(newTarget);
					}
				}
			}

			// ... naval resource?
			else if (pLoopPlot->isWater() &&
				     pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
			{
				ResourceTypes eNonObsoleteResource = pLoopPlot->getNonObsoleteResourceType(eTeam);
				if (eNonObsoleteResource != NO_RESOURCE)
				{
					CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
					if (NULL != pWorkingCity && pWorkingCity->getOwner() == m_pPlayer->GetID())
					{
						// Find proper improvement
						BuildTypes eBuild;
						for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							eBuild = ((BuildTypes)iJ);
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if (pkBuildInfo && pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
							{
								if (GC.getImprovementInfo((ImprovementTypes) GC.getBuildInfo(eBuild)->getImprovement())->IsImprovementResourceTrade(eNonObsoleteResource))
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
			else if (!m_pPlayer->isMinorCiv() && pLoopPlot->isGoody())
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedAncientRuins.push_back(newTarget);
			}

			// ... enemy civilian (or embarked) unit?
			else if (pLoopPlot->isVisibleOtherUnit(m_pPlayer->GetID()))
			{
				CvUnit *pTargetUnit = pLoopPlot->getUnitByIndex(0);
				if (!pTargetUnit->isDelayedDeath() && atWar(eTeam, pTargetUnit->getTeam()) && !pTargetUnit->IsCanDefend())
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					m_TargetedAncientRuins.push_back(newTarget);
				}
			}

			// ... possible sentry point? (must be empty or only have friendly units)
			else if (!pLoopPlot->isWater() && (!pLoopPlot->isUnit() || pLoopPlot->getNumDefenders(m_pPlayer->GetID()) > 0))
			{
				// Must be at least adjacent to our land
				if (pLoopPlot->getOwner() == m_pPlayer->GetID() ||
					(pLoopPlot->isAdjacentTeam(eTeam, true /*bLandOnly*/) && pLoopPlot->getOwner() == NO_PLAYER))
				{
					// See how many outside plots are nearby to monitor
					int iOutsidePlots = pLoopPlot->GetNumAdjacentDifferentTeam(eTeam, true /*bIgnoreWater*/);

					if (iOutsidePlots > 0)
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_SENTRY_POINT);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData(pLoopPlot);

						// Get weight for this sentry point
						int iWeight = iOutsidePlots * 100;
						iWeight += pLoopPlot->defenseModifier(eTeam, true);
						iWeight += m_pPlayer->GetPlotDanger(*pLoopPlot);

						CvCity *pFriendlyCity = m_pPlayer->GetClosestFriendlyCity(*pLoopPlot, 5 /*i SearchRadius */);
						if (pFriendlyCity && pFriendlyCity->getOwner() == m_pPlayer->GetID())
						{
							iWeight += pFriendlyCity->getThreatValue() * pFriendlyCity->getPopulation() / 50;
							if (pFriendlyCity->isCapital())
							{
								iWeight = (iWeight * GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()) / 100;
							}
						}

						if (pLoopPlot->isHills())
						{
							iWeight *= 2;
						}
						if (pLoopPlot->isCoastalLand())
						{
							iWeight /= 2;
						}

						newTarget.SetAuxIntData(iWeight);
						m_TargetedSentryPoints.push_back(newTarget);
					}
				}
			}

			// ... road segment in friendly territory?
			else if (pLoopPlot->getTeam() == eTeam && pLoopPlot->isRoute())
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
	for (it = m_MovePriorityList.begin(); it != m_MovePriorityList.end() && !m_CurrentTurnUnits.empty(); ++it)
	{
		CvHomelandMove move = *it;

		AI_PERF_FORMAT("AI-perf-tact.csv", ("Homeland Move: %d, Turn %03d, %s", (int)move.m_eMoveType, GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

		switch (move.m_eMoveType)
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
			case AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE:
				PlotArtistMoves();
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
			case AI_HOMELAND_MOVE_GENERAL_GARRISON:
				PlotGeneralMoves();
				break;
			case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
				PlotAircraftMoves();
				break;
			case AI_HOMELAND_MOVE_SPACESHIP_PART:
				PlotSSPartMoves();
				break;
			case AI_HOMELAND_MOVE_TREASURE:
				PlotTreasureMoves();
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
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_EXPLORE ||
				pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_EXPLORE)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
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
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA ||
				pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_EXPLORE)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
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
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		bool bGoingToSettle = false;
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && !pUnit->isHuman())
		{
			if (m_pPlayer->getNumCities() == 0 && m_CurrentMoveUnits.size() == 0)
			{
				if (pUnit->canFound(pUnit->plot()))
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
					bGoingToSettle = true;
				}
			}
		}

		// If we find a settler that isn't in an operation, let's keep him in place
		if (!bGoingToSettle && pUnit->isFound() && pUnit->getArmyID() == FFreeList::INVALID_INDEX)
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			pUnit->finishMoves();
		}
	}

	if (!m_CurrentMoveUnits.empty())
	{
		ExecuteFirstTurnSettlerMoves();
	}
}

/// Send units to garrison cities
void CvHomelandAI::PlotGarrisonMoves(bool bCityStateOnly)
{
	// City state garrisoning has a separate priority, so only do it when the flag is on
	if (bCityStateOnly != m_pPlayer->isMinorCiv())
	{
		return;
	}

	// Do we have any targets of this type?
	if (m_TargetedCities.size() > 0)
	{
		for (unsigned int iI = 0; iI < m_TargetedCities.size(); iI++)
		{
			CvPlot *pTarget = GC.getMap().plot(m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY());
			CvCity *pCity = pTarget->getPlotCity();

			if (pCity && pCity->GetLastTurnGarrisonAssigned() < GC.getGame().getGameTurn())
			{
				// Grab units that make sense for this move type
				FindUnitsForThisMove(AI_HOMELAND_MOVE_GARRISON, (iI == 0)/*bFirstTime*/);

				if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
				{
					if(GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns))
					{
						ExecuteMoveToTarget(pTarget, true /*bGarrisonIfPossible*/);

						if (GC.getLogging() && GC.getAILogging())
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
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && !pUnit->isHuman())
		{
			// Am I under 100% health and not at sea or already in a city?
			if (pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints() && !pUnit->isEmbarked() && !pUnit->plot()->isCity())
			{
				// If I'm a naval unit I need to be in friendly territory
				if (pUnit->getDomainType() != DOMAIN_SEA || pUnit->plot()->IsFriendlyTerritory(m_pPlayer->GetID()))
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);

					if (GC.getLogging() && GC.getAILogging())
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

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteHeals();
	}
}

/// Moved endangered units to safe hexes
void CvHomelandAI::PlotMovesToSafety ()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Danger value of plot must be greater than 0
			CvPlot *pPlot = pUnit->plot();

			int iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
			if (iDangerLevel > 0)
			{
				bool bAddUnit = false;

				// If civilian (or embarked unit) always ready to flee
				if (!pUnit->IsCanDefend())
				{
					bAddUnit = true;
				}

				// Also may be true if a damaged combat unit
				else if (pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
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

				if (bAddUnit)
				{
					// Just one unit involved in this move to execute
					CvHomelandUnit unit;
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

/// Send units to roads for quick movement to face any threat
void CvHomelandAI::PlotMobileReserveMoves()
{
	// Do we have any targets of this type?
	if (!m_TargetedHomelandRoads.empty())
	{
		// Prioritize them (LATER)

		// See how many moves of this type we can execute
		for (unsigned int iI = 0; iI < m_TargetedHomelandRoads.size(); iI++)
		{
			CvPlot *pTarget = GC.getMap().plot(m_TargetedHomelandRoads[iI].GetTargetX(), m_TargetedHomelandRoads[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_MOBILE_RESERVE, (iI == 0)/*bFirstTime*/);

			if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, MAX_INT))
				{
					ExecuteMoveToTarget(pTarget);

					if (GC.getLogging() && GC.getAILogging())
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
	// Do we have any targets of this type?
	if (!m_TargetedSentryPoints.empty())
	{
		// Prioritize them (LATER)

		// See how many moves of this type we can execute
		for (unsigned int iI = 0; iI < m_TargetedSentryPoints.size(); iI++)
		{
			AI_PERF_FORMAT("Homeland-perf.csv", ("PlotSentryMoves, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
			CvPlot *pTarget = GC.getMap().plot(m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_SENTRY, (iI == 0)/*bFirstTime*/);

			if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, MAX_INT))
				{
					ExecuteMoveToTarget(pTarget);

					if (GC.getLogging() && GC.getAILogging())
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

/// Find something for all workers to do
void CvHomelandAI::PlotWorkerMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_WORKER  ||
				pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_BUILD)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteWorkerMoves();
	}
}

/// Send out work boats to harvest resources
void CvHomelandAI::PlotWorkerSeaMoves()
{
	// Do we have any targets of this type?
	if (!m_TargetedNavalResources.empty())
	{
		// Prioritize them (LATER)

		// See how many moves of this type we can execute
		for (unsigned int iI = 0; iI < m_TargetedNavalResources.size(); iI++)
		{
			// See what units we have who can reach target this turn
			CvPlot *pTarget = GC.getMap().plot(m_TargetedNavalResources[iI].GetTargetX(), m_TargetedNavalResources[iI].GetTargetY());
			if (FindWorkersInSameArea(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
			{
				// Queue best one up to capture it
				if (ExecuteWorkerSeaMoves(m_TargetedNavalResources[iI], pTarget))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Harvesting naval resource at, X: %d, Y: %d", m_TargetedNavalResources[iI].GetTargetX(), m_TargetedNavalResources[iI].GetTargetY());
						LogHomelandMessage(strLogString);
					}
				}
				else
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving toward naval resource at, X: %d, Y: %d", m_TargetedNavalResources[iI].GetTargetX(), m_TargetedNavalResources[iI].GetTargetY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}

/// When nothing better to do, have units patrol to an adjacent tiles
void CvHomelandAI::PlotPatrolMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all remaining units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && !pUnit->isHuman() && pUnit->getDomainType() != DOMAIN_AIR)
		{
			CvPlot *pTarget = FindPatrolTarget(pUnit.pointer());
			if (pTarget)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				unit.SetTarget(pTarget);
				m_CurrentMoveUnits.push_back(unit);
				if (GC.getLogging() && GC.getAILogging())
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

	if (m_CurrentMoveUnits.size() > 0)
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
	bool bMissingResource;

	ClearCurrentMoveUnits();
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		// Don't try and upgrade a human player's unit or one already recruited for an operation
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && !pUnit->isHuman() && pUnit->getArmyID() == -1)
		{
			// Can this unit be upgraded?
			UnitTypes eUpgradeUnitType = pUnit->GetUpgradeUnitType();
			if (eUpgradeUnitType != NO_UNIT)
			{
				// Tech requirement
				TechTypes ePrereqTech = (TechTypes) GC.getUnitInfo(eUpgradeUnitType)->GetPrereqAndTech();
				if (ePrereqTech == NO_TECH || GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
				{
					// Resource requirement
					bMissingResource = false;
					for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bMissingResource; iResourceLoop++)
					{
						eResource = (ResourceTypes) iResourceLoop;
						iNumResource = GC.getUnitInfo(eUpgradeUnitType)->GetResourceQuantityRequirement(eResource);
						if (iNumResource > 0)
						{
							if (m_pPlayer->getNumResourceAvailable(eResource) < iNumResource)
							{
								bMissingResource = true;
							}
						}
					}

					if (!bMissingResource)
					{
						CvHomelandUnit unit;
						unit.SetID(pUnit->GetID());

						// Initial priority: units with lowest power first
						int iPriority = UPGRADE_THIS_TURN_PRIORITY_BOOST - GC.getUnitInfo(pUnit->getUnitType())->GetPower();

						// Priority is boosted if can upgrade immediately
						if (pUnit->CanUpgradeRightNow(false))
						{
							iPriority += UPGRADE_THIS_TURN_PRIORITY_BOOST;
						}

						// Alternatively, priority boosted a lesser amount if in friendly territory
						else if (pUnit->getOwner() == pUnit->plot()->getOwner())
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
	if (m_CurrentMoveUnits.size() == 0)
	{
		if (!m_pPlayer->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_UNIT_UPGRADE))
		{
			m_pPlayer->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE);
		}
	}

	else
	{
		// Sort results so highest priority is first
		std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end(), HomelandAIHelpers::CvHomelandUnitAuxIntReverseSort);

		int iFlavorMilitaryTraining = 0;
		for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && iFlavorMilitaryTraining == 0; iFlavorLoop++)
		{
			if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
			{
				iFlavorMilitaryTraining = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop);
			}
		}

		iFlavorMilitaryTraining = max(1,iFlavorMilitaryTraining/3);

		// Try to find a unit that can upgrade immediately
		int iNumUpgraded = 0;
		for (moveUnitIt = m_CurrentMoveUnits.begin(); moveUnitIt != m_CurrentMoveUnits.end(); ++moveUnitIt)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
			if (pUnit->CanUpgradeRightNow(false))
			{
				CvUnit *pNewUnit = pUnit->DoUpgrade();
				UnitProcessed(pUnit->GetID());
				UnitProcessed(pNewUnit->GetID());

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp1, strTemp2;
					strTemp1 = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
					strTemp2 = GC.getUnitInfo(pNewUnit->getUnitType())->GetDescription();
					strLogString.Format("Upgrading unit from type %s to type %s, X: %d, Y: %d", strTemp1.GetCString(), strTemp2.GetCString(), pNewUnit->getX(), pNewUnit->getY());
					LogHomelandMessage(strLogString);
				}

				iNumUpgraded++;
				if (iNumUpgraded >= iFlavorMilitaryTraining)
				{
					return; // Only upgrade iFlavorMilitaryTraining units per turn
				}
			}
		}

		if (iNumUpgraded > 0)
		{
			return;
		}

		// Couldn't do all upgrades this turn, get ready for highest priority unit to upgrade
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());

		int iAmountRequired = pUnit->upgradePrice(pUnit->GetUpgradeUnitType());
		bool bRequiresGold = (iAmountRequired > 0);
		int iGoldPriority = 0;

		if (bRequiresGold)
		{
			// Find priority of this financial request
			int iCurrentFlavorMilitaryTraining = 0;
			for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && iCurrentFlavorMilitaryTraining == 0; iFlavorLoop++)
			{
				if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
				{
					iCurrentFlavorMilitaryTraining = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
				}
			}
			iGoldPriority = GC.getAI_GOLD_PRIORITY_UPGRADE_BASE();
			iGoldPriority += GC.getAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT() * m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iCurrentFlavorMilitaryTraining);
		}

		// Start saving
		if (bRequiresGold)
		{
			m_pPlayer->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE);
			m_pPlayer->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE, iAmountRequired, iGoldPriority);
		}

		// Already in friendly territory
		if (pUnit->plot()->getOwner() == pUnit->getOwner())
		{
			if (GC.getLogging() && GC.getAILogging())
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
			CvCity *pUpgradeCity = NULL;
			int iBestDistance = MAX_INT;
			for (unsigned int iI = 0; iI < m_TargetedCities.size(); iI++)
			{
				CvPlot *pTarget = GC.getMap().plot(m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY());
				CvCity *pCity = pTarget->getPlotCity();

				int iDistance = plotDistance(pCity->getX(), pCity->getY(), pUnit->getX(), pUnit->getY());

				if (iDistance < iBestDistance)
				{
					iBestDistance = iDistance;
					pUpgradeCity = pCity;
				}
			}

			if (pUpgradeCity)
			{
				if (MoveToEmptySpaceNearTarget(pUnit.pointer(), pUpgradeCity->plot()))
				{
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if (GC.getLogging() && GC.getAILogging())
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
	if (!m_TargetedAncientRuins.empty())
	{
		// Prioritize them (LATER)

		// See how many moves of this type we can execute
		for (unsigned int iI = 0; iI < m_TargetedAncientRuins.size(); iI++)
		{
			CvPlot *pTarget = GC.getMap().plot(m_TargetedAncientRuins[iI].GetTargetX(), m_TargetedAncientRuins[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_ANCIENT_RUINS, (iI == 0)/*bFirstTime*/);

			if (m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				if(GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns))
				{
					ExecuteMoveToTarget(pTarget);

					if (GC.getLogging() && GC.getAILogging())
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

/// Find moves for great artists
void CvHomelandAI::PlotArtistMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_ARTIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteArtistMoves();
	}
}

/// Find moves for great scientists
void CvHomelandAI::PlotScientistMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_SCIENTIST)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteScientistMoves();
	}
}

/// Find moves for great engineers
void CvHomelandAI::PlotEngineerMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_ENGINEER)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteEngineerMoves();
	}
}

/// Find moves for great merchants
void CvHomelandAI::PlotMerchantMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_MERCHANT)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMerchantMoves();
	}
}

/// Find moves for spaceship parts
void CvHomelandAI::PlotSSPartMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_SPACESHIP_PART)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteSSPartMoves();
	}
}

/// Find moves for treasures
void CvHomelandAI::PlotTreasureMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_TREASURE)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteTreasureMoves();
	}
}

/// Find moves for great generals
void CvHomelandAI::PlotGeneralMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->AI_getUnitAIType() == UNITAI_GENERAL)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteGeneralMoves();
	}
}

/// Send units to cities near the front (or carriers)
void CvHomelandAI::PlotAircraftMoves()
{
	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			if (pUnit->getDomainType() == DOMAIN_AIR)
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteAircraftMoves();
	}
}



/// Log that we couldn't find assignments for some units
void CvHomelandAI::ReviewUnassignedUnits()
{
	// Loop through all remaining units
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
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
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (pUnit)
		{
			pUnit->PushMission(CvTypes::getMISSION_FOUND());
			UnitProcessed(pUnit->GetID());
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Founded city at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}

#define PATH_PLAN_LAST
typedef CvWeightedVector<CvPlot*, 1, true> WeightedPlotVector;

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
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit || pUnit->TurnProcessed())
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
			if (LuaSupport::CallHook(pkScriptSystem, "UnitGetSpecialExploreTarget", args.get(), bResult))
			{
				if (bResult)
				{
					continue;
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
		if (pGoodyPlot && (pGoodyPlot->isGoody(m_pPlayer->getTeam()) || pGoodyPlot->HasBarbarianCamp()) && !pGoodyPlot->isVisibleEnemyDefender(pUnit.pointer()))
		{
			bool bCanFindPath = false;
			if (pkStepPlot)	// Do we already have our first step point?
			{
				if (IsValidExplorerEndTurnPlot(pUnit.pointer(), pkStepPlot))
					bCanFindPath = true;
				pEconomicAI->ClearUnitTargetGoodyStepPlot(pUnit.pointer());
			}
			if (!pkStepPlot || !bCanFindPath)
			{
				bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), iUnitX, iUnitY, pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
			if (bCanFindPath)
			{
					pkStepPlot = kPathFinder.GetPathEndTurnPlot();
				}
			}
			if (bCanFindPath)
			{
				if(pkStepPlot)
				{
					CvAssert(!pUnit->atPlot(*pkStepPlot));
					if (GC.getLogging() && GC.getAILogging())
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
		for (int iX = -iMovementRange; iX <= iMovementRange; iX++)
		{
			for (int iY = -iMovementRange; iY <= iMovementRange; iY++)
			{
				CvPlot* pEvalPlot = plotXYWithRangeCheck(iUnitX, iUnitY, iX, iY, iMovementRange);
				if (!pEvalPlot)
				{
					continue;
				}

				if (!IsValidExplorerEndTurnPlot(pUnit.pointer(), pEvalPlot))
				{
					continue;
				}

#if !defined(PATH_PLAN_LAST)
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit.pointer(), pUnit->getX(), pUnit->getY(), pEvalPlot->getX(), pEvalPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if (!bCanFindPath)
				{
					continue;
				}

				CvAStarNode* pNode = kPathFinder.GetLastNode();
				int iDistance = pNode->m_iData2;
				if (iDistance > 1)
				{
					continue;
				}
#endif

				DomainTypes eDomain = pUnit->getDomainType();
				int iScore = CvEconomicAI::ScoreExplorePlot(pEvalPlot, eTeam, iBaseSightRange, eDomain);
				if (iScore > 0)
				{
					if (eDomain == DOMAIN_LAND && pEvalPlot->isHills())
					{
						iScore += 50;
					}
					else if (eDomain == DOMAIN_SEA && pEvalPlot->isAdjacentToLand())
					{
						iScore += 200;
					}
					else if (eDomain == DOMAIN_LAND && pUnit->IsEmbarkAllWater() && !pEvalPlot->isShallowWater())
					{
						iScore += 200;
					}
#if defined(PATH_PLAN_LAST)
					aBestPlotList.push_back(pEvalPlot, iScore);
#endif
				}

#if !defined(PATH_PLAN_LAST)
				if (iScore > iBestPlotScore)
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
void CvHomelandAI::ExecuteWorkerMoves()
{
	CvString strLogString;

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (pUnit)
		{
			CvPlot* pPlot = pUnit->plot();

			if (pPlot && m_pPlayer->GetPlotDanger(*pPlot) > 0 && !pPlot->getBestDefender(m_pPlayer->GetID()))
			{
				if (MoveCivilianToSafety(pUnit.pointer()))
				{
					if (GC.getLogging() && GC.GetBuilderAILogging())
					{
						// Open the log file
						CvString strFileName = "BuilderTaskingLog.csv";
						FILogFile *pLog;
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

			bool bActionPerformed = ExecuteWorkerMove(pUnit.pointer());
			if (bActionPerformed)
			{
				continue;
			}

			// if there's nothing else to do, move to the safest spot nearby
			if (MoveCivilianToSafety(pUnit.pointer(), true /*bIgnoreUnits*/))
			{
				if (GC.getLogging() && GC.GetBuilderAILogging())
				{
					// Open the log file
					CvString strFileName = "BuilderTaskingLog.csv";
					FILogFile *pLog;
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

				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				if (!m_pPlayer->isHuman())
				{
					pUnit->finishMoves();
				}
				UnitProcessed(pUnit->GetID());
				continue;
			}

			// slewis - this was removed because a unit would eat all its moves. So if it didn't do anything this turn, it wouldn't be able to work 
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			if (!m_pPlayer->isHuman())
			{
				pUnit->finishMoves();
			}
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Send work boats to deploy on naval resources (return true if improvement built)
bool CvHomelandAI::ExecuteWorkerSeaMoves(CvHomelandTarget target, CvPlot *pTarget)
{
	bool bRtnValue = false;

	// Move first one to target
	UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits.begin()->GetID());
	if (pUnit)
	{
		if(pUnit->UnitPathTo(pTarget->getX(), pTarget->getY(), 0) > 0)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			if (pUnit->plot() == pTarget)
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
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
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

					#define MAX_DANGER_VALUE	100000
					#define PREFERENCE_LEVEL(x, y) (x * MAX_DANGER_VALUE) + ((MAX_DANGER_VALUE - 1) - y)

					CvAssert(iDanger < MAX_DANGER_VALUE);

					int iScore;
					if(bIsInCity)
					{
						iScore = PREFERENCE_LEVEL(5, iDanger);
					}
					else if(bIsZeroDanger)
					{
						if (bIsInTerritory)
							iScore = PREFERENCE_LEVEL(4, iDanger);
						else
							iScore = PREFERENCE_LEVEL(3, iDanger);
					}
					else if(bIsInCover)
					{
						iScore = PREFERENCE_LEVEL(2, iDanger);
					}
					else if(bIsInTerritory)
					{
						iScore = PREFERENCE_LEVEL(1, iDanger);
					}
					// if we have no good home, head to the lowest danger value
					else 
					{
						iScore = PREFERENCE_LEVEL(0, iDanger);
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
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		CvPlot *pTarget = it->GetTarget();
		if (pUnit && pTarget)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Find one unit to move to target, starting with high priority list
void CvHomelandAI::ExecuteMoveToTarget(CvPlot *pTarget, bool bGarrisonIfPossible)
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
		else if (bGarrisonIfPossible && pBestUnit->plot() == pTarget && pBestUnit->CanGarrison(pTarget))
		{
			pBestUnit->PushMission(CvTypes::getMISSION_GARRISON());
			pBestUnit->finishMoves();
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
	for (it = m_CurrentMoveHighPriorityUnits.begin(); it != m_CurrentMoveHighPriorityUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		// Don't move high priority unit if regular priority unit is closer
		if (m_CurrentMoveUnits.size() > 0 && m_CurrentMoveUnits.begin()->GetMovesToTarget() < it->GetMovesToTarget())
		{
			break;
		}

		if (pUnit)
		{
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
			else if (it->GetMovesToTarget() < GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE() || TurnsToReachTarget(pUnit, pTarget) != MAX_INT)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(it->GetID());
				return;
			}
		}
	}

	// Then regular priority
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (pUnit)
		{
			if (pUnit->plot() == pTarget && pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
				UnitProcessed(it->GetID());
				return;
			}

			else if (it->GetMovesToTarget() < GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE() || TurnsToReachTarget(pUnit, pTarget) != MAX_INT)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(it->GetID());
				return;
			}
		}
	}
}

/// Expends an artist to start a golden age
void CvHomelandAI::ExecuteArtistMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch (eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			{
				int iValue = 0;
				CvPlot* pTargetPlot = GET_PLAYER(m_pPlayer->GetID()).FindBestArtistTargetPlot(pUnit.pointer(), iValue);
				if (pTargetPlot)
				{
					if (pUnit->plot() == pTargetPlot)
					{
						pUnit->PushMission(CvTypes::getMISSION_CULTURE_BOMB());
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Great artist culture bombed at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
					else
					{
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
						pUnit->finishMoves();
						UnitProcessed(pUnit->GetID());

						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Great artist moving to culture bomb at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
			}
			break;
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
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


/// Expends a scientist to gain a free tech
void CvHomelandAI::ExecuteScientistMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch (eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit.pointer());
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			pUnit->PushMission(CvTypes::getMISSION_DISCOVER());
			UnitProcessed(pUnit->GetID());
			if (GC.getLogging() && GC.getAILogging())
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
	CvCity *pWonderCity;
	int iTurnsToTarget;

	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch (eDirective)
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
				BuildingTypes eNextWonderDesired = m_pPlayer->GetWonderProductionAI()->ChooseWonder(false /*bUseAsyncRandom*/, false /*bAdjustForOtherPlayers*/, iNextWonderWeight);

				// No?  Just move to safety...
				if (eNextWonderDesired == NO_BUILDING)
				{
					if (GC.getLogging() && GC.getAILogging())
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
					if (pWonderCity)
					{
						int iProductionSoFar = pWonderCity->getProduction();
						int iProductionRemaining = pWonderCity->getProductionNeeded(eNextWonderDesired);

						if (pWonderCity->getProductionBuilding() == eNextWonderDesired && iProductionSoFar * 3 < iProductionRemaining)
						{
							// If engineer can move to city before half done
							int iTurnsRemaining = pWonderCity->getProductionTurnsLeft();
							iTurnsToTarget = TurnsToReachTarget(pUnit, pWonderCity->plot(), true /*bReusePaths*/, true);
							if (iTurnsToTarget * 3 < iTurnsRemaining)
							{
								bForceWonderCity = false;

								// Already at target and the wonder is underway?
								if (pWonderCity->getProductionBuilding() == eNextWonderDesired && iTurnsToTarget == 0 && pUnit->plot() == pWonderCity->plot())
								{
									pUnit->PushMission(CvTypes::getMISSION_HURRY());
									UnitProcessed(pUnit->GetID());
									if (GC.getLogging() && GC.getAILogging())
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

										if (GC.getLogging() && GC.getAILogging())
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

					if (bForceWonderCity)
					{
						// Choose a city for our desired wonder
						if (!pWonderCity)
						{
							pWonderCity = ChooseBestFreeWonderCity (eNextWonderDesired, pUnit);
						}

						if (pWonderCity)
						{
							iTurnsToTarget = TurnsToReachTarget(pUnit, pWonderCity->plot(), false /*bReusePaths*/, true);

							// Already at target?
							if (iTurnsToTarget == 0 && pUnit->plot() == pWonderCity->plot())
							{
								// Switch production
								pWonderCity->pushOrder(ORDER_CONSTRUCT, eNextWonderDesired, -1, false, false, false);

								// Rush it
								pUnit->PushMission(CvTypes::getMISSION_HURRY());
								UnitProcessed(pUnit->GetID());
								if (GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer hurrying free wonder at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
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

									if (GC.getLogging() && GC.getAILogging())
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

void CvHomelandAI::ExecuteMerchantMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch (eDirective)
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

/// Moves a great general into an important city to aid its defense
void CvHomelandAI::ExecuteGeneralMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;

	bool bHaveApolloInCapital = false;
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	if (eApolloProgram != NO_PROJECT)
	{
		if (GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			bHaveApolloInCapital = true;
		}
	}


	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
		{
			continue;
		}

		if (pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			ExecuteGoldenAgeMove(pUnit.pointer());
			continue;
		}

		// if we already built the Apollo Program we don't want the general in the capital because it'll block spaceship parts


		// Already in a friendly city?
		CvPlot* pUnitPlot =  pUnit->plot();
		if (pUnitPlot->isFriendlyCity(*pUnit, false) && !bHaveApolloInCapital && pUnitPlot->getPlotCity()->isCapital())
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
			if (GC.getLogging() && GC.getAILogging())
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
			CvPlot *pBestCityPlot = NULL;
			for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
			{
				if (bHaveApolloInCapital && pLoopCity->isCapital())
				{
					continue;
				}

				bool bSkipCity = false;

				CvPlot *pTarget = pLoopCity->plot();
				for (int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
				{
					// Don't go here if a general is already present
					if (pTarget->getUnitByIndex(iUnitLoop)->AI_getUnitAIType() == UNITAI_GENERAL)
					{
						bSkipCity = true;
						break;
					}
				}

				if (!bSkipCity)
				{
					int iTurns = TurnsToReachTarget(pUnit, pTarget);
					if (iTurns < iBestTurns)
					{
						iBestTurns = iTurns;
						pBestCityPlot = pTarget;
					}
				}
			}

			if (pBestCityPlot)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestCityPlot->getX(), pBestCityPlot->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving Great General to city garrison, X: %d, Y: %d", pBestCityPlot->getX(), pBestCityPlot->getY());
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("No place to move Great General at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
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
	for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if (pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if (pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if (!pUnit)
			{
				continue;
			}

			if (pUnit->plot() == pCapitalCity->plot())
			{
				if (GC.getLogging() && GC.getAILogging())
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

			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());

				if (GC.getLogging() && GC.getAILogging())
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

/// Moves a treasure to the capital
void CvHomelandAI::ExecuteTreasureMoves()
{
	CvCity* pLoopCity;
	CvCity* pCapitalCity = NULL;
	int iLoopCity = 0;
	for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if (pLoopCity->isCapital())
		{
			pCapitalCity = pLoopCity;
			break;
		}
	}

	if (pCapitalCity != NULL)
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
			if (!pUnit)
			{
				continue;
			}

			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pCapitalCity->getX(), pCapitalCity->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if (GC.getLogging() && GC.getAILogging())
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

/// Moves an aircraft into an important city near the front to aid its defense (or offense)
void CvHomelandAI::ExecuteAircraftMoves()
{
	FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;

	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
		{
			continue;
		}

		CvPlot* pUnitPlot = pUnit->plot();

		CvPlot *pBestPlot = NULL;
		int iMostDangerous = 0;

		// first look for open carrier slots
		int iLoopUnit = 0;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			CvPlot* pLoopUnitPlot = pLoopUnit->plot();

			if (pLoopUnit->getDamage() > 3) // this might not be a good place to land
			{
				continue;
			}

			if (pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pLoopUnitPlot->getX(),pLoopUnitPlot->getY()))
			{
				continue;
			}

			if (!pUnit->canLoadUnit(*pLoopUnit, *pLoopUnitPlot))
			{
				continue;
			}

			int iPlotDanger = m_pPlayer->GetPlotDanger(*pLoopUnitPlot) + 5000;
			if (iPlotDanger > iMostDangerous)
			{
				iMostDangerous = iPlotDanger;
				pBestPlot = pLoopUnitPlot;
			}

		}

		CvCity* pLoopCity;
		int iLoopCity = 0;
		for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			CvPlot *pTarget = pLoopCity->plot();

			if (pLoopCity->getDamage() > 5)
			{
				continue;
			}

			if (pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot,pTarget->getX(),pTarget->getY()))
			{
				continue;
			}

			int iPlotDanger = m_pPlayer->GetPlotDanger(*pTarget);
			if (iPlotDanger > iMostDangerous)
			{
				iMostDangerous = iPlotDanger;
				pBestPlot = pTarget;
			}
		}

		if (pBestPlot && pBestPlot != pUnitPlot)
		{
			pUnit->PushMission(CvTypes::getMISSION_REBASE(), pBestPlot->getX(), pBestPlot->getY());
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Rebasing Aircraft to city garrison, X: %d, Y: %d", pBestPlot->getX(), pBestPlot->getY());
				LogHomelandMessage(strLogString);
			}
		}
		else
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("No better place to move Aircraft at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}
	}
}


/// Fleeing to safety for civilian units
bool CvHomelandAI::MoveCivilianToSafety (CvUnit *pUnit, bool bIgnoreUnits)
{
	int iSearchRange = pUnit->SearchRange(1);

	int iBestValue = -999999;
	CvPlot* pBestPlot = NULL;

	for (int iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for (int iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iDX, iDY, iSearchRange);
			if (!pLoopPlot)
			{
				continue;
			}

			if (!pUnit->PlotValid(pLoopPlot))
			{
				continue;
			}

			if (pLoopPlot->isVisibleEnemyUnit(pUnit))
			{
				continue;
			}

			int iPathTurns;
			if (!pUnit->GeneratePath(pLoopPlot, MOVE_UNITS_IGNORE_DANGER, true, &iPathTurns))
			{
				continue;
			}

			// if we can't get there this turn, forget it
			if (iPathTurns > 1)
			{
				continue;
			}

			int iValue = 0;
			if (pLoopPlot->getOwner() != NO_PLAYER && GET_PLAYER(pLoopPlot->getOwner()).getTeam() == m_pPlayer->getTeam()) {
				// if this is within our territory, provide a minor benefit
				iValue += 1;
			}

			CvCity* pCity = pLoopPlot->getPlotCity();
			if (pCity && pCity->getTeam() == pUnit->getTeam())
			{
				iValue += pCity->getStrengthValue() * (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDamage());
			}
			else if (!bIgnoreUnits)
			{
				IDInfo* pUnitNode = pLoopPlot->headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit && pLoopUnit->getOwner() == pUnit->getOwner())
					{
						if (pLoopUnit->IsCanDefend())
						{
							if (pLoopUnit != pUnit)
							{
								if (pLoopUnit->isWaiting() || !(pLoopUnit->canMove()))
								{
									iValue += pLoopUnit->GetMaxDefenseStrength(pLoopPlot, NULL) * pLoopUnit->GetCurrHitPoints();
								}
							}
						}
					}
				}
			}

			iValue -= m_pPlayer->GetPlotDanger(*pLoopPlot);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (pUnit->atPlot(*pBestPlot))
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;
				strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
				strLogString.Format("%s (%d) tried to move to safety but is at the best spot, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
				LogHomelandMessage(strLogString);
			}

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
			if (GC.getLogging() && GC.getAILogging())
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
		if (GC.getLogging() && GC.getAILogging())
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
	for (it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;

		// Is it adjacent to a point in the main list?
		for (it2 = m_TargetedSentryPoints.begin(); it2 != m_TargetedSentryPoints.end(); ++it2)
		{
			if (plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if (!bFoundAdjacent)
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
	for (it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;

		// Is it adjacent to a point in the main list?
		for (it2 = m_TargetedHomelandRoads.begin(); it2 != m_TargetedHomelandRoads.end(); ++it2)
		{
			if (plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}

		if (!bFoundAdjacent)
		{
			m_TargetedHomelandRoads.push_back(*it);
		}
	}
}

/// Fills m_CurrentMoveUnits with all units in same area
bool CvHomelandAI::FindWorkersInSameArea (CvPlot *pTarget, BuildTypes eBuild)
{
	bool rtnValue = false;
	ClearCurrentMoveUnits();

	// Loop through all units available to homeland AI this turn
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pLoopUnit = m_pPlayer->getUnit(*it);
		if (pLoopUnit)
		{
			// Civilians only
			if (pLoopUnit->IsCanAttack())
			{
				continue;
			}

			// Can this worker make the right improvement?
			if (pLoopUnit->canBuild(pTarget, eBuild))
			{
				CvHomelandUnit unit;
				int iMoves = TurnsToReachTarget(pLoopUnit.pointer(), pTarget);

				if (iMoves != MAX_INT)
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
	std::stable_sort (m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	return rtnValue;
}

/// Finds both high and normal priority units we can use for this homeland move (returns true if at least 1 unit found)
bool CvHomelandAI::FindUnitsForThisMove(AIHomelandMove eMove, bool bFirstTime)
{
	bool rtnValue = false;

	if (bFirstTime)
	{
		ClearCurrentMoveUnits();
		ClearCurrentMoveHighPriorityUnits();

		// Loop through all units available to homeland AI this turn
		for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			UnitHandle pLoopUnit = m_pPlayer->getUnit(*it);
			if (pLoopUnit && !pLoopUnit->isHuman())
			{
				// Civilians aren't useful for any of these moves
				if (!pLoopUnit->IsCombatUnit())
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

				switch (eMove)
				{
				case AI_HOMELAND_MOVE_GARRISON:
				case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
					// Want to put ranged units in cities to give them a ranged attack
					if (pLoopUnit->isRanged() && !pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_CITY_BOMBARD))
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}

					// Don't use non-combatants
					else if (pLoopUnit->IsCanAttack())
					{
						// Don't put units with a combat strength boosted from promotions in cities, these boosts are ignored
						if (pLoopUnit->getDefenseModifier() == 0 &&
							pLoopUnit->getAttackModifier() == 0 &&
							pLoopUnit->getExtraCombatPercent() == 0)
						{
							bSuitableUnit = true;
						}
					}
					break;

				case AI_HOMELAND_MOVE_SENTRY:
					// No ranged units as sentries
					if (!pLoopUnit->isRanged() && !pLoopUnit->noDefensiveBonus())
					{
						bSuitableUnit = true;

						// Units with extra sight are especially valuable
						if (pLoopUnit->getExtraVisibilityRange() > 0)
						{
							bHighPriority = true;
						}
					}
					else if (pLoopUnit->noDefensiveBonus() && pLoopUnit->getExtraVisibilityRange() > 0)
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}
					break;

				case AI_HOMELAND_MOVE_MOBILE_RESERVE:
					// Ranged units are excellent in the mobile reserve as are fast movers
					if (pLoopUnit->isRanged() || pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}
					else if (pLoopUnit->IsCanAttack())
					{
						bSuitableUnit = true;
					}
					break;

				case AI_HOMELAND_MOVE_ANCIENT_RUINS:
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
					break;
				}

				// If unit was suitable, add it to the proper list
				if (bSuitableUnit)
				{
					CvHomelandUnit unit;
					unit.SetID(pLoopUnit->GetID());

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

	// Second time through, just make sure all units are still in list of current units to process
	else
	{
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL > tempList;
		FStaticVector< CvHomelandUnit, 64, true, c_eCiv5GameplayDLL >::iterator it;
		std::list<int>::iterator it2;

		// Normal priority units
		tempList = m_CurrentMoveUnits;
		ClearCurrentMoveUnits();
		for (it = tempList.begin(); it != tempList.end(); ++it)
		{
			it2 = find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), (int)it->GetID());
			if (it2 != m_CurrentTurnUnits.end())
			{
				m_CurrentMoveUnits.push_back(*it);
				rtnValue = true;
			}
		}

		// High priority units
		tempList = m_CurrentMoveHighPriorityUnits;
		ClearCurrentMoveHighPriorityUnits();
		for (it = tempList.begin(); it != tempList.end(); ++it)
		{
			it2 = find(m_CurrentTurnUnits.begin(), m_CurrentTurnUnits.end(), (int)it->GetID());
			if (it2 != m_CurrentTurnUnits.end())
			{
				m_CurrentMoveHighPriorityUnits.push_back(*it);
				rtnValue = true;
			}
		}
	}

	return rtnValue;
}

/// See if there is an adjacent plot we can wander to
CvPlot *CvHomelandAI::FindPatrolTarget(CvUnit *pUnit)
{
	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pUnit->PlotValid(pAdjacentPlot))
			{
				if (!(pAdjacentPlot->isVisibleEnemyUnit(pUnit)))
				{
					if (pUnit->GeneratePath(pAdjacentPlot, 0, true))
					{
						iValue = (1 + GC.getGame().getJonRandNum(10000, "AI Patrol"));

						// Prefer wandering in our own territory
						if (pAdjacentPlot->getOwner() == pUnit->getOwner())
						{
							iValue += 10000;
						}

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pAdjacentPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		CvAssert(!pUnit->atPlot(*pBestPlot));
		return pBestPlot;
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
bool CvHomelandAI::MoveToEmptySpaceNearTarget(CvUnit *pUnit, CvPlot *pTarget, bool bLand)
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
	return false;
}

/// Choose a city to build the next wonder
CvCity *CvHomelandAI::ChooseBestFreeWonderCity(BuildingTypes eWonder, UnitHandle pEngineer)
{
	CvCity *pBestCity = NULL;
	CvCity* pLoopCity;
	int iLoop;
	int iBestTurns = MAX_INT;
	int iTurns;

	for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity->canConstruct(eWonder))
		{
			iTurns = TurnsToReachTarget(pEngineer, pLoopCity->plot(), true /*bReusePaths*/);
			if (iTurns < iBestTurns)
			{
				pBestCity = pLoopCity;
				iBestTurns = iTurns;
			}
		}
	}

	return pBestCity;
}

/// Find the plot where we want to pillage
/// Log current status of the operation
void CvHomelandAI::LogHomelandMessage(CvString& strMsg)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2;
		CvString strPlayerName;
		FILogFile *pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// Remove a unit that we've allocated from list of units to move this turn
void CvHomelandAI::UnitProcessed (int iID)
{
	CvUnit * pUnit;

	m_CurrentTurnUnits.remove(iID);
	pUnit = m_pPlayer->getUnit(iID);

	CvAssert (pUnit);
	if(!pUnit) return;

	pUnit->SetTurnProcessed(true);
}

bool CvHomelandAI::ExecuteWorkerMove(CvUnit* pUnit)
{
    const UINT ciDirectiveSize = 1;
	BuilderDirective aDirective[ ciDirectiveSize ];

	// evaluator
	bool bHasDirective = m_pPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pUnit, aDirective, ciDirectiveSize);
	if (bHasDirective)
	{
		switch (aDirective[0].m_eDirective)
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
				if (pUnit->getX() == aDirective[0].m_sX && pUnit->getY() == aDirective[0].m_sY)
				{
					eMission = CvTypes::getMISSION_BUILD();
				}
				else
				{
					eMission = CvTypes::getMISSION_MOVE_TO();
				}

				if (GC.getLogging() && GC.GetBuilderAILogging())
				{
					// Open the log file
					CvString strFileName = "BuilderTaskingLog.csv";
					FILogFile *pLog;
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

					switch (aDirective[0].m_eDirective)
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

					if (eMission == CvTypes::getMISSION_BUILD())
					{
						if (aDirective[0].m_eDirective == BuilderDirective::REPAIR)
						{
							if (pPlot->IsImprovementPillaged())
							{
								strLog += "Repairing improvement";
							}
							else
							{
								strLog += "Repairing route";
							}
						}
						else if (aDirective[0].m_eDirective == BuilderDirective::BUILD_ROUTE)
						{
							strLog += "Building route,";
						}
						else if (aDirective[0].m_eDirective == BuilderDirective::BUILD_IMPROVEMENT || aDirective[0].m_eDirective == BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE)
						{
							strLog += "Building improvement,";
						}
						else if (aDirective[0].m_eDirective == BuilderDirective::CHOP)
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

				if (eMission == CvTypes::getMISSION_BUILD())
				{
					// check to see if we already have this mission as the unit's head mission
					bool bPushMission = true;
					const MissionData* pkMissionData = pUnit->GetHeadMissionData();
					if (pkMissionData != NULL)
					{
						if (pkMissionData->eMissionType == eMission && pkMissionData->iData1 == aDirective[0].m_eBuild) {
							bPushMission = false;
						}
					}

					if (bPushMission)
					{
						pUnit->PushMission(CvTypes::getMISSION_BUILD(),   aDirective[0].m_eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);
					}

					CvAssertMsg(!pUnit->ReadyToMove(), "Worker did not do their mission this turn. Could cause game to hang.");
					if (pUnit->ReadyToMove())
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
		if (GC.getLogging() && GC.GetBuilderAILogging())
		{
			CvString strFileName = "BuilderTaskingLog.csv";
			FILogFile *pLog;
			pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
			pLog->Msg("builder has no directive");
		}
	}

	return false;
}

bool CvHomelandAI::ExecuteGoldenAgeMove(CvUnit* pUnit)
{
	// removed because golden ages are additive. Using them while a golden age is going on adds to the duration of the golden age

	//if (m_pPlayer->isGoldenAge())
	//{
	//	if (MoveCivilianToSafety(pUnit))
	//	{
	//		if (GC.getLogging() && GC.getAILogging())
	//		{
	//			CvString strLogString;
	//			CvString strTemp;
	//			strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
	//			strLogString.Format("Moving %s to safety at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
	//			LogHomelandMessage(strLogString);
	//		}
	//	}
	//	else
	//	{
	//		pUnit->PushMission(MISSION_SKIP);
	//	}
	//	pUnit->finishMoves();
	//}
	//else
	//{
		pUnit->PushMission(CvTypes::getMISSION_GOLDEN_AGE());
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;
			strTemp = pUnit->getUnitInfo().GetDescription();
			strLogString.Format("%s started golden age with unit at, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
	//}

	return true;
}

bool CvHomelandAI::IsValidExplorerEndTurnPlot (const CvUnit* pUnit, CvPlot* pPlot) const
{
	if (pUnit->atPlot(*pPlot))
	{
		return false;
	}

	if (!pPlot->isRevealed(pUnit->getTeam()))
	{
		return false;
	}

	DomainTypes eDomain = pUnit->getDomainType();

	if (pPlot->area() != pUnit->area())
	{
		if (!pUnit->CanEverEmbark())
		{
			if (!(eDomain == DOMAIN_SEA && pPlot->isWater()))
			{
				return false;
			}
		}
	}

	// don't let the auto-explore end it's turn in a city
	CvCity* pCity = pPlot->getPlotCity();
	if (pCity && pCity->getOwner() != pUnit->getOwner())
	{
		return false;
	}

	if (!pUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE))
	{
		return false;
	}

	return true;
}

/// Move an exploring unit to a designated target (special function exposed to Lua)
bool CvHomelandAI::ExecuteSpecialExploreMove(CvUnit *pUnit, CvPlot *pTargetPlot)
{
	bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
	if (bCanFindPath)
	{
		CvPlot* pPlot = GC.getPathFinder().GetPathEndTurnPlot();
		if(pPlot)
		{
			CvAssert(!pUnit->atPlot(*pPlot));
			if (GC.getLogging() && GC.getAILogging())
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
	if (GC.getPlayerAndCityAILogSplit())
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
bool HomelandAIHelpers::CvHomelandUnitAuxIntSort (CvHomelandUnit obj1, CvHomelandUnit obj2)
{
	return obj1.GetAuxIntData() < obj2.GetAuxIntData();
}

bool HomelandAIHelpers::CvHomelandUnitAuxIntReverseSort (CvHomelandUnit obj1, CvHomelandUnit obj2)
{
	return obj1.GetAuxIntData() > obj2.GetAuxIntData();
}