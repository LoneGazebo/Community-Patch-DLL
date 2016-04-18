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
#if defined(MOD_BALANCE_CORE_SETTLER)
	, m_iPrevPlotIdx1(0)
	, m_iPrevPlotIdx2(0)
#endif
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

#if defined(MOD_BALANCE_CORE_MILITARY)
	//needed for better debugging - can't use ID here because it's not set yet!
	m_CurrentMoveHighPriorityUnits.setPlayer(pPlayer);
	m_CurrentMoveUnits.setPlayer(pPlayer);
#endif
}

/// Deallocate memory created in initialize
void CvHomelandAI::Uninit()
{
}

/// Reset variables
void CvHomelandAI::Reset()
{
}

/// Serialization read
void CvHomelandAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	int iDummy;
	kStream >> iDummy;
}

/// Serialization write
void CvHomelandAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	//dummy value
	kStream << 0L;
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
#if defined(MOD_BALANCE_CORE_MILITARY)
			//don't use units which were assigned a tactical move this turn!
			if ( pLoopUnit->hasCurrentTacticalMove() )
			{
				CvString msg = CvString::format("warning: homeland AI unit %d has a current tactical move (%s at %d,%d)", 
										pLoopUnit->GetID(), pLoopUnit->getName().c_str(), pLoopUnit->getX(), pLoopUnit->getY() );
				LogHomelandMessage( msg );

				/*
				//if we skip the units, we have to end their turn, else the AI turn will never end! (in fact it is terminated after 10 turn slices without movement ...)
				pLoopUnit->finishMoves();
				pLoopUnit->SetTurnProcessed(true);
				continue;
				*/
			}
#endif

			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
		}
	}

#if defined(MOD_CORE_DEBUGGING)
	if (MOD_CORE_DEBUGGING)
	{
		for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			UnitHandle pUnit = m_pPlayer->getUnit(*it);
			CvString msg = CvString::format("current turn homeland unit %s %d at %d,%d\n", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY() );
			LogHomelandMessage( msg );
		}
	}
#endif
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

CvPlot* CvHomelandAI::GetBestExploreTarget(const CvUnit* pUnit, int nMinCandidates) const
{
	if (!pUnit)
		return NULL;

	CvEconomicAI* pEconomicAI = m_pPlayer->GetEconomicAI();

	const std::vector<SPlotWithScore>& vExplorePlots = pEconomicAI->GetExplorationPlots( pUnit ? pUnit->getDomainType() : DOMAIN_LAND );
	if (vExplorePlots.empty())
		return NULL;

	int iBestPlotScore = 100; //limits initial search range to 10 turns
	CvPlot* pBestPlot = NULL;

	//sort by distance to capital or to unit
	int iRefX = pUnit ? pUnit->getX() : m_pPlayer->getCapitalCity()->getX();
	int iRefY = pUnit ? pUnit->getY() : m_pPlayer->getCapitalCity()->getY();

	std::vector< std::pair<int,SPlotWithScore> > vPlotsByDistance;
	for(uint ui = 0; ui < vExplorePlots.size(); ui++)
	{
		if (pUnit->getDomainType() == DOMAIN_SEA)
		{
			//sea units can typically not leave their area - catch this case, 
			//else there will be a long and useless pathfinding operation
			if (pUnit->plot()->getArea()!=vExplorePlots[ui].pPlot->getArea())
				continue;
		}

		if(vExplorePlots[ui].pPlot == pUnit->plot())
			continue;

		//make sure our unit can actually go there
		if (!IsValidExplorerEndTurnPlot(pUnit, vExplorePlots[ui].pPlot))
			continue;

		//Ignore plots with units in them
		if(vExplorePlots[ui].pPlot->getNumUnits() > 0)
			continue;

		//don't send multiple explorers to the same target
		if (m_pPlayer->IsPlotTargetedForExplorer(vExplorePlots[ui].pPlot, pUnit))
			continue;

		int iDistX = abs( vExplorePlots[ui].pPlot->getX() - iRefX );
		int iDistY = abs( vExplorePlots[ui].pPlot->getY() - iRefY );
		int iDist2 = (iDistX*iDistX)+(iDistY*iDistY);

		vPlotsByDistance.push_back( std::make_pair( (iDist2*100)/vExplorePlots[ui].score, vExplorePlots[ui]) );
	}

	//sorts ascending by the first element of the iterator ... which is our distance. nice.
	std::stable_sort(vPlotsByDistance.begin(), vPlotsByDistance.end());

	int iValidCandidates = 0;
	for (size_t idx=0; idx<vPlotsByDistance.size(); idx++)
	{
		//after looking at the N closest candidates
		//if we found something, bail
		if (pBestPlot && iValidCandidates>nMinCandidates)
			break;

		CvPlot* pEvalPlot = vPlotsByDistance[idx].second.pPlot;
		int iRating = vPlotsByDistance[idx].second.score;

		//discourage embarking
		if (pUnit->getDomainType()==DOMAIN_LAND && pEvalPlot->isWater())
			iRating /= 2;

		//try to explore close to our cities first to find potential settle spots
		int iCityDistance = m_pPlayer->GetCityDistance(pEvalPlot);
		iRating = max(1, iRating-iCityDistance); 

		//reverse the score calculation below to get an upper bound on the distance
		//minus one because we want to do better
		int iMaxDistance = (1000*iRating) / max(1,iBestPlotScore) - 1;

		//is there a chance we can reach the plot within the required number of turns? (assuming no roads)
		if( sqrt((float)vPlotsByDistance[idx].first) > (iMaxDistance*pUnit->baseMoves()) )
			continue;

		SPathFinderUserData data(pUnit,CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE,iMaxDistance);
		bool bCanFindPath = GC.GetPathFinder().GeneratePath(iRefX, iRefY, pEvalPlot->getX(), pEvalPlot->getY(), data);

		if(!bCanFindPath)
			continue;

		int iDistance = GC.GetPathFinder().GetPathLength();
		int iPlotScore = (1000 * iRating) / max(1,iDistance);

		iValidCandidates++;

		if (iPlotScore>iBestPlotScore)
		{
			CvPlot* pEndTurnPlot = GC.GetPathFinder().GetPathEndTurnPlot();

			if(pEndTurnPlot == pUnit->plot())
			{
				pBestPlot = NULL;
				iBestPlotScore = iPlotScore;
			}
			else if(IsValidExplorerEndTurnPlot(pUnit, pEndTurnPlot))
			{
				pBestPlot = pEndTurnPlot;
				iBestPlotScore = iPlotScore;
			}
		}
	}

	return pBestPlot;
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

#if defined(MOD_AI_SMART_UPGRADES)
	// On even turns upgrades acquire more priority in order to be able to upgrade air units.
	int iTurnUpgradePriority = MOD_AI_SMART_UPGRADES ? (GC.getGame().getGameTurn() % 2) * 50 : 0;
#endif

	// Find required flavor values
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			iFlavorDefense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorDefense = (int)(iFlavorDefense * m_fFlavorDampening);
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			iFlavorOffense = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorOffense = (int)(iFlavorOffense * m_fFlavorDampening);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iFlavorExpand = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_RECON")
		{
			iFlavorExplore = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			iFlavorExplore = (int)(iFlavorExplore * m_fFlavorDampening);
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
#if defined(AUI_HOMELAND_FIX_ESTABLISH_HOMELAND_PRIORITIES_MISSING_FLAVORS)
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_TILE_IMPROVEMENT")
		{
			iFlavorImprove = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_NAVAL_TILE_IMPROVEMENT")
		{
			iFlavorNavalImprove = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
#endif // AUI_HOMELAND_FIX_ESTABLISH_HOMELAND_PRIORITIES_MISSING_FLAVORS
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
#if defined(MOD_BALANCE_CORE)
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GARRISON();
#else
			if(m_pPlayer->GetPlayerPolicies()->HasPolicyEncouragingGarrisons())
			{
				iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SENTRY() + 1;
			}

			else
			{
				iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GARRISON();
			}
#endif
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
#if defined(MOD_BALANCE_CORE)
				if(m_pPlayer->IsAtWarAnyMajor())
				{
					iPriority *= iFlavorDefense;
				}
				break;
#endif
			case AI_HOMELAND_MOVE_HEAL:
#if defined(MOD_BALANCE_CORE)
				iPriority += iFlavorDefense;
				break;
#endif
			case AI_HOMELAND_MOVE_TO_SAFETY:
#if defined(MOD_BALANCE_CORE)
				if(m_pPlayer->IsAtWarAnyMajor())
				{
					iPriority *= iFlavorImprove;
				}
				break;
#endif
			case AI_HOMELAND_MOVE_MOBILE_RESERVE:
#if defined(MOD_BALANCE_CORE)
				iPriority += iFlavorDefense;
				break;
#endif
			case AI_HOMELAND_MOVE_SENTRY:
#if defined(MOD_BALANCE_CORE)
				if(m_pPlayer->IsAtWarAnyMajor())
				{
					iPriority /= max (2, iFlavorDefense);
				}
				break;
#endif
			case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
			case AI_HOMELAND_MOVE_GENERAL_GARRISON:
			case AI_HOMELAND_MOVE_ADMIRAL_GARRISON:
#if defined(MOD_BALANCE_CORE)
				iPriority += iFlavorDefense;
				break;
#endif
			case AI_HOMELAND_MOVE_AIRCRAFT_TO_THE_FRONT:
#if defined(MOD_BALANCE_CORE)
				if(m_pPlayer->IsAtWarAnyMajor())
				{
					iPriority *= iFlavorOffense;
				}
				break;
#endif
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

			//if (m_pPlayer->isMajorCiv())
			//	OutputDebugString( CvString::format("Player %d, Move %s, Prio %d\n",m_pPlayer->GetID(),homelandMoveNames[iI],iPriority).c_str() );
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
				if(m_pPlayer->GetID() == pCity->getOwner() && !pCity->HasGarrison())
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_CITY);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData((void*)pCity);
					newTarget.SetAuxIntData(pCity->getThreatValue());
					m_TargetedCities.push_back(newTarget);
				}
			}
			// ... naval resource?
			else if(pLoopPlot->isWater() && pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
			{
#if defined(MOD_BALANCE_CORE)
				ResourceTypes eNonObsoleteResource = pLoopPlot->getResourceType(eTeam);
				if(eNonObsoleteResource != NO_RESOURCE)
				{
					if(pLoopPlot->getOwner() == m_pPlayer->GetID())
					{
#else
				ResourceTypes eNonObsoleteResource = pLoopPlot->getNonObsoleteResourceType(eTeam);

				if(eNonObsoleteResource != NO_RESOURCE)
				{
					CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
					if(NULL != pWorkingCity && pWorkingCity->getOwner() == m_pPlayer->GetID())
					{
#endif
						// Find proper improvement
						BuildTypes eBuild;
						for(int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							// we've already checked for pLoopPlot is water and builds on sea tiles are a Resource Trade no need for an additional check, we just need to know if we can build it or not - Iamblichos
							eBuild = ((BuildTypes)iJ);
							if(eBuild != NO_BUILD)
							{
								newTarget.SetTargetType(AI_HOMELAND_TARGET_NAVAL_RESOURCE);
								newTarget.SetTargetX(pLoopPlot->getX());
								newTarget.SetTargetY(pLoopPlot->getY());
								newTarget.SetAuxData(pLoopPlot);
								newTarget.SetAuxIntData(eBuild);
								m_TargetedNavalResources.push_back(newTarget);								
								//why break, just build all builds available that we can.
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
#if defined(MOD_BALANCE_CORE)
			else if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()))
			{
				if(pLoopPlot->getOwner() != NO_PLAYER)
				{
					if(pLoopPlot->getOwner() == m_pPlayer->GetID() || !m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoDiggingPromise(pLoopPlot->getOwner()))
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData(pLoopPlot);
						m_TargetedAntiquitySites.push_back(newTarget);
					}
				}
				else
				{

#else
			else if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()) && 
				!(pLoopPlot->getOwner() != NO_PLAYER && m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoDiggingPromise(pLoopPlot->getOwner())))
			{
#endif
				newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedAntiquitySites.push_back(newTarget);
#if defined(MOD_BALANCE_CORE)
				}
#endif
			}
#if defined(MOD_BALANCE_CORE)
			// ... possible sentry point?
			else if(pLoopPlot->getOwner() == m_pPlayer->GetID() && !pLoopPlot->isWater() && pLoopPlot->isValidMovePlot(m_pPlayer->GetID()) && !pLoopPlot->isCity())
			{
				ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
				ImprovementTypes eCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");
				
				if((eFort != NO_IMPROVEMENT) && (eCitadel != NO_IMPROVEMENT) && (pLoopPlot->getImprovementType () != NO_IMPROVEMENT) && ((pLoopPlot->getImprovementType() == eFort) || (pLoopPlot->getImprovementType() == eCitadel)))
				{
					int iWeight = 100000;

					newTarget.SetTargetType(AI_HOMELAND_TARGET_FORT);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					newTarget.SetAuxIntData(iWeight);
					m_TargetedSentryPoints.push_back(newTarget);
				}
				else
				{
					int iWeight = pLoopPlot->GetDefenseBuildValue(m_pPlayer->GetID());
					if(iWeight > 0)
					{
						if(pLoopPlot->getResourceType(eTeam) != NO_RESOURCE)
						{
							iWeight += 25;
						}
						if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
						{
							iWeight += 25;
						}
						if(pLoopPlot->isHills())
						{
							iWeight += 25;
						}
						if(pLoopPlot->getNumUnits() > 0)
						{
							CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
							if((pUnit != NULL) && pUnit->IsCivilianUnit())
							{
								iWeight += 25;
							}
						}
#if !defined(MOD_CORE_REDUCE_RANDOMNESS)
						iWeight += GC.getGame().getJonRandNum(25, "Roll for randomness");
#endif
						newTarget.SetTargetType(AI_HOMELAND_TARGET_SENTRY_POINT);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData(pLoopPlot);
						newTarget.SetAuxIntData(iWeight);
						m_TargetedSentryPoints.push_back(newTarget);
					}
				}
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
			// ... empty barb camp?
			else if(pLoopPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT() && pLoopPlot->getNumDefenders(BARBARIAN_PLAYER) <= 0)
			{
				CvUnit* pTargetUnit = pLoopPlot->getUnitByIndex(0);
				if(pTargetUnit && !pTargetUnit->isDelayedDeath() && atWar(eTeam, pTargetUnit->getTeam()) && !pTargetUnit->IsCanDefend())
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					m_TargetedAncientRuins.push_back(newTarget);
				}
				else
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					m_TargetedAncientRuins.push_back(newTarget);
				}
			}
#else
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

						CvCity* pFriendlyCity = m_pPlayer->GetClosestCity(*pLoopPlot, 5 /*i SearchRadius */);
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
#endif
			// ... road segment in friendly territory?
#if defined(MOD_BALANCE_CORE)
			else if(pLoopPlot->isRoute() && pLoopPlot->getOwner() == m_pPlayer->GetID())
			{
				//Let's weight them based on defense and danger - this should make us muster in more tactically - responsible places
				int iWeight = pLoopPlot->defenseModifier(eTeam, true);
				iWeight += m_pPlayer->GetPlotDanger(*pLoopPlot);
				if(iWeight > 0)
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_HOME_ROAD);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					newTarget.SetAuxIntData(iWeight);
					m_TargetedHomelandRoads.push_back(newTarget);
				}
			}
#else
			else if(pLoopPlot->getTeam() == eTeam && pLoopPlot->isRoute())
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_HOME_ROAD);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxData(pLoopPlot);
				m_TargetedHomelandRoads.push_back(newTarget);
			}
#endif
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

#if defined(MOD_BALANCE_CORE_MILITARY)
		m_CurrentMoveUnits.setCurrentHomelandMove(move.m_eMoveType);
		m_CurrentMoveHighPriorityUnits.setCurrentHomelandMove(move.m_eMoveType);
#endif

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
#if defined(MOD_BALANCE_CORE)
		ExecuteExplorerMoves(false);
		ExecuteExplorerMoves(true);
#else
		ExecuteExplorerMoves();
		ExecuteExplorerMoves();
#endif
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
#if defined(MOD_BALANCE_CORE)
		ExecuteExplorerMoves(false);
		ExecuteExplorerMoves(true);
#else
		ExecuteExplorerMoves();
		ExecuteExplorerMoves();
#endif
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
#if defined(MOD_BALANCE_CORE_SETTLER_MOVE)
				if(pUnit->canFound(NULL))
#else
				if(pUnit->canFound(pUnit->plot()))
#endif
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
					bGoingToSettle = true;
				}
			}
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

			//don't move into a doomed city
			if(pCity && !pCity->HasGarrison() && !pCity->isInDangerOfFalling() )
			{
				// Grab units that make sense for this move type
				FindUnitsForThisMove(AI_HOMELAND_MOVE_GARRISON, (iI == 0)/*bFirstTime*/);

				if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
				{
					CvUnit *pGarrison = GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns);
					if(pGarrison)
					{
						ExecuteMoveToTarget(pGarrison, pTarget);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving %s %d to garrison, X: %d, Y: %d, Priority: %d", pGarrison->getName().c_str(), pGarrison->GetID(), m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY(), m_TargetedCities[iI].GetAuxIntData());
							LogHomelandMessage(strLogString);
						}
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
			int iHealingLimit = pUnit->GetMaxHitPoints() * 9 / 10;

			if (MOD_AI_SMART_HEALING) 
			{
				CvPlot* unitPlot = pUnit->plot();
				if (!unitPlot->isCity() && (unitPlot->getOwner() != pUnit->getOwner()) && m_pPlayer->GetPlotDanger(*unitPlot) > 0)
				{
					iHealingLimit /= 2;
				}
			}

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
#if defined(MOD_BALANCE_CORE)
			bool bFallout = false;
			if (pPlot != NULL)
			{
				if (!pUnit->IsCanDefend() && pPlot->getFeatureType() == FEATURE_FALLOUT && (pUnit->ignoreFeatureDamage() || (pUnit->getDamage() <= (pUnit->GetMaxHitPoints() / 2))))
				{
					bFallout = true;
				}
			}
			else
				continue;

			int iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot,pUnit.pointer());
#else
			int iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot);
#endif
			if(iDangerLevel > 0)
			{
				bool bAddUnit = false;

				// If civilian (or embarked unit) always ready to flee
				// slewis - 4.18.2013 - Problem here is that a combat unit that is a boat can get stuck in a city hiding from barbarians on the land
				if(!pUnit->IsCanDefend())
				{
#if defined(MOD_BALANCE_CORE)
					if(!bFallout)
					{
#endif
					if (pUnit->IsAutomated() && pUnit->GetBaseCombatStrength() > 0)
					{
						// then this is our special case
					}
					else
					{
						bAddUnit = true;
					}
#if defined(MOD_BALANCE_CORE)
					}
#endif
				}

				// Also may be true if a damaged combat unit
				else if(pUnit->GetCurrHitPoints() < pUnit->GetMaxHitPoints())
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
#if defined(MOD_BALANCE_CORE)
					else if(iDamage > 0 && (((pUnit->getDamage()*100)/pUnit->GetMaxHitPoints())>50))
					{
						bAddUnit = true;
					}
#endif
					// Everyone else flees at more than 70% damage
					else if(MOD_AI_SMART_FLEE_FROM_DANGER && (((pUnit->getDamage()*100)/pUnit->GetMaxHitPoints())>70))
					{
						bAddUnit = true;
					}
					// Everyone else flees at less than or equal to 50% combat strength
					else if(pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
					{
						bAddUnit = true;
					}
#if defined(MOD_CORE_PER_TURN_DAMAGE)
					// Everyone flees under (heavy) enemy fire
					else if(pUnit->isProjectedToDieNextTurn())
					{
						bAddUnit = true;
					}
#endif
#else
					// Everyone else flees at less than or equal to 50% combat strength
					else if(pUnit->IsUnderEnemyRangedAttack() || pUnit->GetBaseCombatStrengthConsideringDamage() * 2 <= pUnit->GetBaseCombatStrength())
					{
						bAddUnit = true;
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
				CvUnit *pReserve = GetBestUnitToReachTarget(pTarget, MAX_INT);
				if(pReserve)
				{
					ExecuteMoveToTarget(pReserve, pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving %s %d to mobile reserve muster pt, X: %d, Y: %d", pReserve->getName().c_str(), pReserve->GetID(), m_TargetedHomelandRoads[iI].GetTargetX(), m_TargetedHomelandRoads[iI].GetTargetY());
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
#if defined(MOD_BALANCE_CORE)
	PlotWorkerMoves(true);
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
				CvUnit *pSentry = GetBestUnitToReachTarget(pTarget, 4);
				if(pSentry)
				{
					ExecuteMoveToTarget(pSentry, pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving %s %d to sentry point, X: %d, Y: %d, Priority: %d", pSentry->getName().c_str(), pSentry->GetID(), m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY(), m_TargetedSentryPoints[iI].GetAuxIntData());
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

	int iCapitalX = 0;
	int iCapitalY = 0;
	if(m_pPlayer->getCapitalCity() != NULL)
	{
		iCapitalX = m_pPlayer->getCapitalCity()->getX();
		iCapitalY = m_pPlayer->getCapitalCity()->getY();
	}
	else
	{
		return;
	}

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
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it) {
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

		MoveUnitsArray::iterator settlerUnitIt;
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

					aBestPlots.push_back(pPlot, pPlot->getFoundValue(m_pPlayer->GetID()) );
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
					bool bCanFindPath = pUnit->GeneratePath(pFoundPlot, 0, INT_MAX, &iPathTurns);

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
			if(m_pPlayer->IsAtWar())
			{
				bSecondary = false;
			}
			if(!bUseSecondaryUnit && bSecondary)
			{
				if(pUnit->getUnitInfo().GetCombat() > 0 && pUnit->getDomainType() == DOMAIN_LAND)
				{
					for (int i = 0; i < GC.getNumBuildInfos(); i++) 
					{
						CvBuildInfo* pkBuild = GC.getBuildInfo((BuildTypes)i);
					
						if (pkBuild && !pkBuild->IsWater() && (pUnit->getUnitInfo().GetBuilds((BuildTypes)i) || m_pPlayer->GetPlayerTraits()->HasUnitClassCanBuild(i, pUnit->getUnitInfo().GetUnitClassType())))
						{
							bUseSecondaryUnit = true;
							break;
						}
					}
				}
			}
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
#if defined(MOD_AI_SECONDARY_WORKERS)
			else if(!m_pPlayer->IsAtWar() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->getUnitInfo().GetCombat() > 0)
			{
				bool bUseSecondaryUnit = (pUnit->AI_getUnitAIType() != UNITAI_WORKER_SEA) && (pUnit->getUnitInfo().GetUnitAIType(UNITAI_WORKER_SEA));
				if(!bUseSecondaryUnit)
				{
					for (int i = 0; i < GC.getNumBuildInfos(); i++) 
					{
						CvBuildInfo* pkBuild = GC.getBuildInfo((BuildTypes)i);
					
						if (pkBuild && pkBuild->IsWater() && (pUnit->getUnitInfo().GetBuilds((BuildTypes)i) || m_pPlayer->GetPlayerTraits()->HasUnitClassCanBuild(i, pUnit->getUnitInfo().GetUnitClassType())))
						{
							bUseSecondaryUnit = true;
							break;
						}
					}
				}
				if(bUseSecondaryUnit)
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
			}
#endif
		}
	}

	MoveUnitsArray::iterator moveUnitIt;
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

			if (!pTarget->getArea()==pUnit->getArea())
				continue;

#if defined(MOD_BALANCE_CORE)
			if(pUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA  ||
			   pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_BUILD)
			{
#endif
				if (!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
					continue;

				if (m_pPlayer->GetPlotDanger(*pTarget,pUnit.pointer())>0)
					continue;
#if defined(MOD_BALANCE_CORE)
			}
			else
			{
				if(!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
				{
					BuildTypes eBuildCheck1 = (BuildTypes)GC.getInfoTypeForString("BUILD_FISHING_BOATS");
					if(eBuildCheck1 == (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData())
					{
						BuildTypes eBuildCheck2 = (BuildTypes)GC.getInfoTypeForString("BUILD_FISHING_BOATS_NO_KILL");
						if (eBuildCheck2 != NO_BUILD)
						{
							m_TargetedNavalResources[iI].SetAuxIntData(eBuildCheck2);
							if (!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
								continue;
						}
						else
						{
							continue;
						}
					}
					else
					{
						continue;
					}
				}
			}
#endif

			int iMoves = pUnit->TurnsToReachTarget(pTarget, false, false, iTargetMoves);
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
#if defined(MOD_BALANCE_CORE)
				pUnit->finishMoves();
				// Delete this unit from those we have to move
				UnitProcessed(m_CurrentMoveUnits.begin()->GetID());
#endif
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
				if(pUnit->canMove() && pUnit->canFortify(pUnit->plot()) && !pUnit->isEmbarked())
				{
					pUnit->setHomelandMove(AI_HOMELAND_MOVE_PATROL);
					pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
					pUnit->SetTurnProcessed(true);
					pUnit->finishMoves();
				}
			}
			else
			{
#endif
			CvPlot* pTarget = HomelandAIHelpers::GetPatrolTarget(pUnit->plot(),pUnit->getOwner(),23);
			if(pTarget && pUnit->GeneratePath(pTarget,CvUnit::MOVEFLAG_APPROXIMATE_TARGET,10))
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
#if defined(MOD_BALANCE_CORE_MILITARY)
			}
#endif
		}
	}
	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecutePatrolMoves();
	}
}

/// Find units that we can upgrade
void CvHomelandAI::PlotUpgradeMoves()
{
	MoveUnitsArray::iterator moveUnitIt;
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
		if(pUnit && !pUnit->isHuman() && (MOD_AI_SMART_UPGRADES || pUnit->getArmyID() == -1))
		{
			// Also eligible to upgrade if army is not moving/at destination
			if (MOD_AI_SMART_UPGRADES)
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
#else
		if(pUnit && !pUnit->isHuman() && pUnit->getArmyID() == -1)
		{
#endif
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
#if defined(MOD_BALANCE_CORE)
							//Don't use all of our Aluminum!
							ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
							if(eResource == eAluminumResource)
							{
								iNumResource += 5;
							}
#endif
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

#if defined(MOD_BALANCE_CORE)
						//Add in experience - we should promote veterans.
#if defined(MOD_UNITS_XP_TIMES_100)
						iPriority += pUnit->getExperienceTimes100();
#else
						iPriority += pUnit->getExperience();
#endif
#endif

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
#if defined(MOD_BALANCE_CORE)
		//Why 1/3?
		iFlavorMilitaryTraining = max(1 , iFlavorMilitaryTraining);
#else
		iFlavorMilitaryTraining = max(1,iFlavorMilitaryTraining/3);
#endif
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
#if defined(MOD_BALANCE_CORE)
		if(iNumUpgraded > iFlavorMilitaryTraining)
#else
		if(iNumUpgraded > 0)
#endif
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
#if defined(MOD_BALANCE_CORE)
				if(m_pPlayer->IsAtWar())
				{
					iCurrentFlavorMilitaryTraining *= 50;
				}
#endif
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
				CvUnit *pIndy = GetBestUnitToReachTarget(pTarget, m_iDefensiveMoveTurns);
				if(pIndy)
				{
					ExecuteMoveToTarget(pIndy, pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving %s %d to goody hut (non-explorer), X: %d, Y: %d", pIndy->getName().c_str(), pIndy->GetID(), m_TargetedAncientRuins[iI].GetTargetX(), m_TargetedAncientRuins[iI].GetTargetY());
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
#if defined(MOD_BALANCE_CORE)
			if(pUnit->AI_getUnitAIType() == UNITAI_MESSENGER || (pUnit->IsAutomated() && pUnit->GetAutomateType() == AUTOMATE_DIPLOMAT))
#else
			if(pUnit->AI_getUnitAIType() == UNITAI_MESSENGER)
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
#if defined(MOD_BALANCE_CORE)
			if(pUnit->AI_getUnitAIType() == UNITAI_MISSIONARY || (pUnit->IsAutomated() && pUnit->GetAutomateType() == AUTOMATE_MISSIONARY))
#else
			if(pUnit->AI_getUnitAIType() == UNITAI_MISSIONARY)
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
#if defined(MOD_BALANCE_CORE_MILITARY)
			if(pUnit->IsGreatGeneral() || pUnit->IsCityAttackSupport())
#else
			if(pUnit->AI_getUnitAIType() == UNITAI_GENERAL)
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
#if defined(MOD_BALANCE_CORE_MILITARY)
			if(pUnit->IsGreatAdmiral())
#else
			if(pUnit->AI_getUnitAIType() == UNITAI_ADMIRAL)
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
#if defined(MOD_BALANCE_CORE)
			if(pUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST || (pUnit->IsAutomated() && pUnit->GetAutomateType() == AUTOMATE_ARCHAEOLOGIST))
#else
			if(pUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST)
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
	CvTacticalAnalysisMap* pTactMap = GC.getGame().GetTacticalAnalysisMap();
	if (pTactMap->GetCurrentPlayer()!=m_pPlayer->GetID())
		pTactMap->RefreshDataForNextPlayer(m_pPlayer);

	CvTacticalDominanceZone *pZone;
	vector<CvCity *>::const_iterator it;
	for (it = aAirliftCities.begin(); it != aAirliftCities.end(); it++)
	{
		pZone = pTactMap->GetZoneByCity(*it, false);
		if (pZone && (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY || pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_VISIBLE))
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pLoopPlot = plotDirection((*it)->getX(), (*it)->getY(), ((DirectionTypes)iI));
				if (pLoopPlot != NULL && !pLoopPlot->isWater() && pLoopPlot->isValidMovePlot(m_pPlayer->GetID()) && pLoopPlot->getOwner() == m_pPlayer->GetID())
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

			CvUnit *pUnit = GetBestUnitToReachTarget(pTarget, MAX_INT);
			if(pUnit)
			{
				ExecuteMoveToTarget(pUnit,pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving %s %d to airlift origin plot, X: %d, Y: %d", pUnit->getName().c_str(), pUnit->GetID(), pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

/// Log that we couldn't find assignments for some units
void CvHomelandAI::ReviewUnassignedUnits()
{
#if defined(MOD_AI_SECONDARY_WORKERS)
	if (MOD_AI_SECONDARY_WORKERS && !m_pPlayer->isMinorCiv()) {
		// Find any units with a secondary role of worker and see if there is anything close to build
		PlotWorkerMoves(true);
	}
#endif
	// Loop through all remaining units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
#if defined(MOD_BALANCE_CORE)
			if(pUnit->getDomainType() == DOMAIN_LAND)
			{
				if(!pUnit->isEmbarked() && pUnit->plot()->getOwner()==pUnit->getOwner())
				{
#if !defined(MOD_CORE_REDUCE_RANDOMNESS)
					CvPlot* pLoopPlotSearch = NULL;
					for (int iI = 0; iI < 3; iI++)
					{
						int iRandomDirection = GC.getGame().getJonRandNum(NUM_DIRECTION_TYPES,"random unassigned move");
						pLoopPlotSearch = plotDirection(pUnit->plot()->getX(), pUnit->plot()->getY(), ((DirectionTypes)iRandomDirection));
						if (pLoopPlotSearch != NULL)
						{
							if (m_pPlayer->GetPlotDanger(*pLoopPlotSearch,pUnit.pointer())>20)
								continue;

							if(pLoopPlotSearch->isWater())
								continue;

							if(pUnit->canMoveInto(*pLoopPlotSearch,CvUnit::MOVEFLAG_DESTINATION) && pUnit->canEnterTerrain(*pLoopPlotSearch) )
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlotSearch->getX(), pLoopPlotSearch->getY());
								break;
							}
						}
					}
#endif
					if(pUnit->canFortify(pUnit->plot()))
					{
						pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
						pUnit->SetTurnProcessed(true);
						pUnit->finishMoves();
					}
					else
					{
						pUnit->PushMission(CvTypes::getMISSION_SKIP());
						pUnit->SetTurnProcessed(true);
						pUnit->finishMoves();
					}

					CvString strTemp;
					CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
					if(pkUnitInfo)
					{
						strTemp = pkUnitInfo->GetDescription();
						CvString strLogString;
						strLogString.Format("Unassigned %s %d wandering around at home, at X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
					continue;
				}
				else
				{
					//We really need to do something with this unit - let's bring it home if we can.
					CvCity* pBestCity = m_pPlayer->GetClosestCity( pUnit->plot() );
					if(pBestCity != NULL)
					{
						MoveToEmptySpaceNearTarget(pUnit.pointer(), pBestCity->plot());
						pUnit->SetTurnProcessed(true);
						pUnit->finishMoves();
						CvString strTemp;
						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
						if(pkUnitInfo)
						{
							strTemp = pkUnitInfo->GetDescription();
							CvString strLogString;
							strLogString.Format("Unassigned %s %d moving homeward, at X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
						continue;
					}
				}
			}
			else if(pUnit->getDomainType() == DOMAIN_SEA)
			{
				if(pUnit->plot()->getOwner() == pUnit->getOwner())
				{
#if !defined(MOD_CORE_REDUCE_RANDOMNESS)
					CvPlot* pLoopPlotSearch = NULL;
					for (int iI = 0; iI < 3; iI++)
					{
						int iRandomDirection = GC.getGame().getJonRandNum(NUM_DIRECTION_TYPES,"random unassigned move");
						pLoopPlotSearch = plotDirection(pUnit->plot()->getX(), pUnit->plot()->getY(), ((DirectionTypes)iRandomDirection));
						if (pLoopPlotSearch != NULL)
						{
							if (m_pPlayer->GetPlotDanger(*pLoopPlotSearch,pUnit.pointer())>20)
								continue;

							if(pUnit->canMoveInto(*pLoopPlotSearch,CvUnit::MOVEFLAG_DESTINATION) && pUnit->canEnterTerrain(*pLoopPlotSearch))
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlotSearch->getX(), pLoopPlotSearch->getY());
								break;
							}
						}
					}
#endif
					pUnit->SetTurnProcessed(true);
					pUnit->finishMoves();

					CvString strTemp;
					CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
					if(pkUnitInfo)
					{
						strTemp = pkUnitInfo->GetDescription();
						CvString strLogString;
						strLogString.Format("Unassigned %s %d at home, at X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
					continue;
				}
				else
				{
					//We really need to do something with this unit - let's bring it home if we can.
					CvPlot* pBestPlot = NULL;
					int iBestDistance = MAX_INT;
					int iLoop;
					for(CvCity* pTestCity = m_pPlayer->firstCity(&iLoop); pTestCity != NULL; pTestCity = m_pPlayer->nextCity(&iLoop))
					{
						CvPlot* pPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pTestCity->plot(), NULL);
						if(pPlot != NULL)
						{
							if(pPlot->getArea() != pUnit->getArea())
							{
								continue;
							}
							int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY());

							if(iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								pBestPlot = pPlot;
							}
						}
					}

					if(pBestPlot != NULL)
					{
						MoveToEmptySpaceNearTarget(pUnit.pointer(), pBestPlot, false);
						pUnit->SetTurnProcessed(true);
						pUnit->finishMoves();
						CvString strTemp;
						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
						if(pkUnitInfo)
						{
							strTemp = pkUnitInfo->GetDescription();
							CvString strLogString;
							strLogString.Format("Unassigned %s %d wandering homeward, at, X: %d, Y: %d - to X: %d, Y: %d.", strTemp.GetCString(), pUnit->GetID(), pUnit->getX(), pUnit->getY(), pBestPlot->getX(), pBestPlot->getY());
							LogHomelandMessage(strLogString);
						}
						continue;
					}
				}
			}
			pUnit->SetTurnProcessed(true);
			pUnit->finishMoves();
#else
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
#endif
		}
	}
}

/// Creates cities for AI civs on first turn
void CvHomelandAI::ExecuteFirstTurnSettlerMoves()
{
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
#if defined(MOD_BALANCE_CORE_SETTLER_MOVE)
			if( MOD_BALANCE_CORE_SETTLER_MOVE && GC.getSETTLER_MOVE_ON_START() > 0 )
			{
				int iInitialPlotValue = 0;
				int iAdjacentValue = 0;
				CvPlot* pBestAdjacentPlot = NULL;
				//Let's check for a river estuary - those are always good
				if(pUnit->plot()->isFreshWater_cached() && pUnit->plot()->isCoastalLand() && pUnit->canFound(pUnit->plot()))
				{
					pUnit->PushMission(CvTypes::getMISSION_FOUND());
					UnitProcessed(pUnit->GetID());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Founded city at starting location as it is great, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
					break;
				}
				else
				{
					iInitialPlotValue = pUnit->canFound(pUnit->plot()) ? pUnit->plot()->getFoundValue(m_pPlayer->GetID()) : 0;

					for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));
						if(pAdjacentPlot != NULL && pUnit->canFound(pAdjacentPlot))
						{
							iAdjacentValue = pAdjacentPlot->getFoundValue(m_pPlayer->GetID());
							if(iAdjacentValue > iInitialPlotValue*1.05f) //should be at least five percent better to justify the hassle
							{
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("%s settler found better initial plot: %d vs %d\n", m_pPlayer->getCivilizationAdjective(), iAdjacentValue, iInitialPlotValue);
									LogHomelandMessage(strLogString);
								}
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
							}
							break;
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
							}
							break;
						}
					}
					else if(pUnit->canFound(pUnit->plot()))
					{
						pUnit->PushMission(CvTypes::getMISSION_FOUND());
						UnitProcessed(pUnit->GetID());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Founded city because this is the best we can do, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
						break;
					}

					//apparently no good plot around. move in a random direction to explore
					CvPlot* pLoopPlotSearch = NULL;
					for (int iI = 0; iI < 3; iI++)
					{
						int iRandomDirection = GC.getGame().getJonRandNum(NUM_DIRECTION_TYPES,"random settler move");
						pLoopPlotSearch = plotDirection(pUnit->plot()->getX(), pUnit->plot()->getY(), ((DirectionTypes)iRandomDirection));
						if (pLoopPlotSearch != NULL)
						{
							if(pLoopPlotSearch != NULL && pUnit->canMoveOrAttackInto(*pLoopPlotSearch,CvUnit::MOVEFLAG_DESTINATION))
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopPlotSearch->getX(), pLoopPlotSearch->getY());
								break;
							}
						}
					}
					if(pLoopPlotSearch != NULL && pUnit->plot() == pLoopPlotSearch && pUnit->canFound(pLoopPlotSearch))
					{
						pUnit->PushMission(CvTypes::getMISSION_FOUND());
						UnitProcessed(pUnit->GetID());
					}
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Things aren't looking good for us! Scramble to X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
					break;
				}
			}
			else
			{
				//default behavior
				pUnit->PushMission(CvTypes::getMISSION_FOUND());
				UnitProcessed(pUnit->GetID());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Founded city at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
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

/// Moves units to explore the map
void CvHomelandAI::ExecuteExplorerMoves(bool bSecondPass)
{
	bool bFoundNearbyExplorePlot = false;

	MoveUnitsArray::iterator it;
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
			PlayerTypes ePlotOwner = NO_PLAYER;
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pLoopPlotSearch = plotDirection(pUnit->plot()->getX(), pUnit->plot()->getY(), ((DirectionTypes)iI));
				if (pLoopPlotSearch != NULL)
				{
					PlayerTypes eLoopPlotOwner = pLoopPlotSearch->getOwner();
					if (eLoopPlotOwner != NO_PLAYER)
					{
						if (!GET_TEAM(pUnit->getTeam()).isAtWar(GET_PLAYER(eLoopPlotOwner).getTeam()))
						{
							if(GET_PLAYER(eLoopPlotOwner).isMinorCiv())
							{
								ePlotOwner = eLoopPlotOwner;
								break;
							}
						}
					}
				}
			}
			if(ePlotOwner != NO_PLAYER)
			{
				fRewardFactor *= 100;
			}
			else
			{
				fRewardFactor /= 10;
			}

			if (fRewardFactor >= 0.5f)
			{
				pUnit->PushMission(CvTypes::getMISSION_SELL_EXOTIC_GOODS());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("%s used Sell Exotic Goods, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}

		int iUnitX = pUnit->getX();
		int iUnitY = pUnit->getY();

		const CvPlot* pCurPlot = GC.getMap().plot( iUnitX, iUnitY );
		CvPlot* pBestPlot = NULL;
		int iBestPlotScore = 0;
		
		//first check our immediate neighborhood (ie the tiles we can reach within one turn)
		TacticalAIHelpers::ReachablePlotSet eligiblePlots;
		TacticalAIHelpers::GetAllPlotsInReach(pUnit.pointer(), pUnit->plot(), eligiblePlots, true /*checkTerritory*/);
		for (TacticalAIHelpers::ReachablePlotSet::iterator tile=eligiblePlots.begin(); tile!=eligiblePlots.end(); ++tile)
		{
			CvPlot* pEvalPlot = GC.getMap().plotByIndexUnchecked(tile->first);

			if(!pEvalPlot)
				continue;

			if(pEvalPlot->isCity() && (pEvalPlot->getOwner() != pUnit->getOwner()))
				continue;

			//don't embark to reach a close-range target
			if(pUnit->getDomainType()==DOMAIN_LAND && pEvalPlot->isWater())
				continue;
			//Ignore land for sea explorers
			if(pUnit->getDomainType()==DOMAIN_SEA && !pEvalPlot->isWater())
				continue;

			//see if we can make an easy kill
			CvUnit* pEnemyUnit = pEvalPlot->getVisibleEnemyDefender(pUnit->getOwner());
			if (pEnemyUnit)
			{
				if (TacticalAIHelpers::KillUnitIfPossible(pUnit.pointer(),pEnemyUnit))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						CvString strTemp = pUnit->getUnitInfo().GetDescription();
						strLogString.Format("%s killed a weak enemy, at X: %d, Y: %d, From X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}

					// Only mark as done if out of movement
					if(pUnit->getMoves() <= 0)
					{
						pUnit->finishMoves();
						UnitProcessed(pUnit->GetID());
						break;
					}
				}
				else
					continue;
			}

			//do this after the enemy unit check
			if (!IsValidExplorerEndTurnPlot(pUnit.pointer(), pEvalPlot))
				continue;

			//get contributions from yet-to-be revealed plots (and goodies)
			int iScoreBase = CvEconomicAI::ScoreExplorePlot2(pEvalPlot, m_pPlayer, pUnit->getDomainType(), pUnit->isEmbarked());
			if(iScoreBase > 0)
			{
				int iScoreBonus = pEvalPlot->GetExplorationBonus(m_pPlayer, pCurPlot);
				int iScoreExtra = 0;

				//hill plots are good for defense and view - do not add this in ScoreExplorePlot2
				if(pEvalPlot->isHills())
					iScoreExtra += 25;

				//resources on water are always near land
				if(pUnit->getDomainType() == DOMAIN_SEA && pEvalPlot->isWater() && (pEvalPlot->getResourceType() != NO_RESOURCE || pEvalPlot->getFeatureType() != NO_FEATURE))
					iScoreExtra += 25;

				//We should always be moving.
				if( pEvalPlot == pUnit->plot())
					iScoreExtra -= 50;

				//If a known minor, let's not wander around if we don't have open borders.
				if(pEvalPlot->getOwner() != NO_PLAYER)
				{
					if(GET_PLAYER(pEvalPlot->getOwner()).isMinorCiv() && GET_TEAM(m_pPlayer->getTeam()).isHasMet( GET_PLAYER(pEvalPlot->getOwner()).getTeam() ) )
					{
						if(!GET_PLAYER(pEvalPlot->getOwner()).GetMinorCivAI()->IsPlayerHasOpenBorders(m_pPlayer->GetID()))
						{
							if(pUnit->getDomainType() == DOMAIN_LAND)
							{
								iScoreExtra -= 25;
							}
						}
					}
				}

				if(pUnit->canSellExoticGoods(pEvalPlot))
				{
					float fRewardFactor = pUnit->calculateExoticGoodsDistanceFactor(pEvalPlot);
					if (fRewardFactor >= 0.75f)
					{
						iScoreExtra += 150;
					}
					else if (fRewardFactor >= 0.5f)
					{
						iScoreExtra += 75;
					}
				}

				int iRandom = GC.getGame().getJonRandNum(50,"explore target tiebreak");
				int iTotalScore = iScoreBase+iScoreExtra+iScoreBonus+iRandom;

				//careful with plots that are too dangerous
				int iAcceptableDanger = pUnit->GetCurrHitPoints()/2;
				int iDanger = m_pPlayer->GetPlotDanger(*pEvalPlot,pUnit.pointer());
				if(iDanger > iAcceptableDanger)
					continue;
				if(iDanger > iAcceptableDanger/2)
					iTotalScore /= 2;

				if(iTotalScore > iBestPlotScore)
				{
					//make sure we can actually reach it - shouldn't happen, but sometimes does because of blocks
					if (!pUnit->CanReachInXTurns(pEvalPlot,1))
						continue;

					pBestPlot = pEvalPlot;
					iBestPlotScore = iTotalScore;
					bFoundNearbyExplorePlot = true;
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strTemp = pUnit->getUnitInfo().GetDescription();
						CvString msg = CvString::format("Checking plots for scouting unit %s is %d, %d with base %d, extra %d, bonus %d\n",
							strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY(), iScoreBase, iScoreExtra, iScoreBonus);
						LogHomelandMessage(msg);
					}
				}
			}
		}

		//if we made an opportunity attack, we're done
		if(!pUnit->canMove())
			continue;

		//if we didn't find a worthwhile plot among our adjacent plots, check the global targets
		if(!pBestPlot && pUnit->movesLeft() > 0)
			//check at least 5 candidates
			pBestPlot = GetBestExploreTarget(pUnit.pointer(),5);

		//make sure we're not in an endless loop
		if(pBestPlot)
		{
			CvAssertMsg(!pUnit->atPlot(*pBestPlot), "Exploring unit is already at the best place to explore");

			if ( (pBestPlot->GetPlotIndex() == it->GetPrevPlot1()) && (pCurPlot->GetPlotIndex() == it->GetPrevPlot2()) )
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
					CvString msg = CvString::format("Warning: Explorer %s is maybe caught in a loop at %d,%d\n", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage( msg );
				}

				//reset
				pBestPlot = NULL;
			}
		}

		if(pBestPlot && pBestPlot != pUnit->plot())
		{
			it->PushPrevPlot( pCurPlot->GetPlotIndex() );
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE, false, false, MISSIONAI_EXPLORE, pBestPlot);

			// Only mark as done if out of movement
			if(pUnit->getMoves() <= 0)
			{
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp = pUnit->getUnitInfo().GetDescription();
				if(bFoundNearbyExplorePlot)
				{			
					strLogString.Format("%s Explored to nearby target, To X: %d, Y: %d, From X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY(), iUnitX, iUnitY);
				}
				else
				{
					strLogString.Format("%s Explored to distant target, To X: %d, Y: %d, From X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY(), iUnitX, iUnitY);
				}
				LogHomelandMessage(strLogString);
			}
		}
		else //no target
		{
			if(pUnit->isHuman())
			{
				CvString strTemp = pUnit->getUnitInfo().GetDescription();
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("%s Explorer (human) found no target, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
				pUnit->SetAutomateType(NO_AUTOMATE);
				UnitProcessed(pUnit->GetID());
				continue;
			}
			else
			{
				// go home or disband
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("%s Explorer found no target, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}

				//in case it was non-native scout, reset the unit AI
				pUnit->AI_setUnitAIType((UnitAITypes)pUnit->getUnitInfo().GetDefaultUnitAIType());

				//nothing to explore and at home?
				bool bDisband = false;
				if (pUnit->plot()->getOwner()==pUnit->getOwner())
					bDisband = (pUnit->AI_getUnitAIType()==UNITAI_EXPLORE);

				if (!bDisband)
				{
					CvCity* pLoopCity = NULL;
					int iLoop;
					bool bFoundWayHome = false;
					for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						if(pUnit->GeneratePath(pLoopCity->plot(),CvUnit::MOVEFLAG_APPROXIMATE_TARGET | CvUnit::MOVEFLAG_IGNORE_STACKING))
						{
							bFoundWayHome = true;
							break;
						}
					}

					if(bFoundWayHome)
					{
						MoveToEmptySpaceNearTarget(pUnit.pointer(),pLoopCity->plot());
						pUnit->finishMoves();
						UnitProcessed(pUnit->GetID());
					}
					else
						bDisband = true;
				}

				if (bDisband)
				{
					//don't disband in the beginning of the game ...  
					if (GC.getGame().getGameTurn()<50)
						pUnit->PushMission( CvTypes::getMISSION_SKIP() );
					else
					{
						CvString strLogString;
						strLogString.Format("UnitID: %d Disbanding explorer, X: %d, Y: %d", pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);

						UnitProcessed(pUnit->GetID());
						pUnit->kill(true);
						m_pPlayer->GetEconomicAI()->IncrementExplorersDisbanded();
					}
				}
			}
		}

		if(bSecondPass)
		{
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
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

	MoveUnitsArray::iterator it;
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

/// Heal chosen units
void CvHomelandAI::ExecuteHeals()
{
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			if(pUnit->canFortify(pUnit->plot()))
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

typedef CvWeightedVector<CvPlot*, 100, true> WeightedPlotVector;

/// Moves units to the hex with the lowest danger
void CvHomelandAI::ExecuteMovesToSafestPlot()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			//so easy
			CvPlot* pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit.pointer(),true);

			if(pBestPlot != NULL)
			{
				// Move to the lowest danger value found
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER);
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
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		CvPlot* pTarget = it->GetTarget();
		if(pUnit && pTarget)
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_APPROXIMATE_TARGET);
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
		}
	}
}

//	---------------------------------------------------------------------------
/// Find one unit to move to target, starting with high priority list
void CvHomelandAI::ExecuteMoveToTarget(CvUnit* pUnit, CvPlot* pTarget)
{
	if (!pUnit || !pTarget)
		return;

	AI_PERF_FORMAT("Homeland-ExecuteMove-perf.csv", ("ExecuteMoveToTarget, %d, %d, Turn %03d, %s", pTarget->getX(), pTarget->getY(), GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	if(pUnit->plot() == pTarget)
	{
		if (pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
		}
		else
			pUnit->PushMission(CvTypes::getMISSION_SKIP());

		UnitProcessed(pUnit->GetID());
#if defined(MOD_BALANCE_CORE)
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Best unit fortified at X: %d, Y: %d", pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
#endif
		return;
	}
	else
	{
		// Best units have already had a full path check to the target, so just add the move
		MoveToUsingSafeEmbark(pUnit, pTarget, true);
		pUnit->finishMoves();
		UnitProcessed(pUnit->GetID());
		return;
	}
}

/// Great writer moves
void CvHomelandAI::ExecuteWriterMoves()
{
	MoveUnitsArray::iterator it;
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
#if defined(MOD_BALANCE_CORE)
					UnitProcessed(pUnit->GetID());
					pUnit->finishMoves();
					continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
						pUnit->finishMoves();
						continue;
#endif
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						CvPlot *pBestTarget = NULL;
						int iBestDistance = INT_MAX;
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_APPROXIMATE_TARGET,INT_MAX,&iTurns))
						{
							if (iTurns==0)
								pBestTarget = pTargetCity->plot();
							else
								for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
								{
									CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
									if(pAdjacentPlot != NULL && pAdjacentPlot->isValidMovePlot(pUnit->getOwner()) )
									{
										int iDistance = plotDistance(pTargetCity->getX(), pTargetCity->getY(), pUnit->getX(), pUnit->getY());
										if (iDistance < iBestDistance)
										{
											pBestTarget = pAdjacentPlot;
											iBestDistance = iDistance;
										}
									}
								}
						}

						if (pBestTarget)
						{
							// In less than one turn?
							if (iTurns == 0)
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
#if defined(MOD_BALANCE_CORE)
								pUnit->finishMoves();
								continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
								continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
							UnitProcessed(pUnit->GetID());
							pUnit->finishMoves();
							continue;
#endif
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
	MoveUnitsArray::iterator it;
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
#if defined(MOD_BALANCE_CORE)
					UnitProcessed(pUnit->GetID());
					pUnit->finishMoves();
					continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
						pUnit->finishMoves();
						continue;
#endif
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						CvPlot *pBestTarget = NULL;
						int iBestDistance = INT_MAX;
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_APPROXIMATE_TARGET,INT_MAX,&iTurns))
						{
							if (iTurns==0)
								pBestTarget = pTargetCity->plot();
							else
								for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
								{
									CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
									if(pAdjacentPlot != NULL && pAdjacentPlot->isValidMovePlot(pUnit->getOwner()) )
									{
										int iDistance = plotDistance(pTargetCity->getX(), pTargetCity->getY(), pUnit->getX(), pUnit->getY());
										if (iDistance < iBestDistance)
										{
											pBestTarget = pAdjacentPlot;
											iBestDistance = iDistance;
										}
									}
								}
						}

						if (pBestTarget)
						{
							// In less than one turn?
							if (iTurns == 0)
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
#if defined(MOD_BALANCE_CORE)
								continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
								continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
							UnitProcessed(pUnit->GetID());
							pUnit->finishMoves();
							continue;
#endif
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
	MoveUnitsArray::iterator it;
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
#if defined(MOD_BALANCE_CORE)
					UnitProcessed(pUnit->GetID());
					pUnit->finishMoves();
					continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
						pUnit->finishMoves();
						continue;
#endif
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						CvPlot *pBestTarget = NULL;
						int iBestDistance = INT_MAX;
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_APPROXIMATE_TARGET,INT_MAX,&iTurns))
						{
							if (iTurns==0)
								pBestTarget = pTargetCity->plot();
							else
								for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
								{
									CvPlot* pAdjacentPlot = plotDirection(pTargetCity->getX(), pTargetCity->getY(), ((DirectionTypes)iI));
									if(pAdjacentPlot != NULL && pAdjacentPlot->isValidMovePlot(pUnit->getOwner()) )
									{
										int iDistance = plotDistance(pTargetCity->getX(), pTargetCity->getY(), pUnit->getX(), pUnit->getY());
										if (iDistance < iBestDistance)
										{
											pBestTarget = pAdjacentPlot;
											iBestDistance = iDistance;
										}
									}
								}
						}

						if (pBestTarget)
						{
							// In less than one turn?
							if (iTurns == 0)
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
#if defined(MOD_BALANCE_CORE)
								continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
								continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
							UnitProcessed(pUnit->GetID());
							pUnit->finishMoves();
							continue;
#endif
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
	MoveUnitsArray::iterator it;
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

	MoveUnitsArray::iterator it;
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
			BuildingTypes eNextWonderDesired = m_pPlayer->GetWonderProductionAI()->ChooseWonderForGreatEngineer(iNextWonderWeight, pCityToBuildAt);

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
						iTurnsToTarget = pUnit->TurnsToReachTarget(pWonderCity->plot(), true);
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
#if defined(MOD_BALANCE_CORE)
								pUnit->finishMoves();
								continue;
#endif
							}

							// No, then move there
							else
							{
								CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), MAX_INT);
								if(pEng)
								{
									ExecuteMoveToTarget(pEng, pWonderCity->plot());

									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Moving %s to city specialization wonder city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
										LogHomelandMessage(strLogString);
									}
#if defined(MOD_BALANCE_CORE)
									continue;
#endif
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
						iTurnsToTarget = pUnit->TurnsToReachTarget(pWonderCity->plot(), true);

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
#if defined(MOD_BALANCE_CORE)
								pUnit->finishMoves();
								continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
								UnitProcessed(pUnit->GetID());
								pUnit->finishMoves();
								continue;
#endif
							}
						}

						// No, then move there
						else
						{
							CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), MAX_INT);
							if(pEng)
							{
								ExecuteMoveToTarget(pEng, pWonderCity->plot());

								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving %s for free wonder to city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
									LogHomelandMessage(strLogString);
								}
#if defined(MOD_BALANCE_CORE)
								continue;
#endif
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
	MoveUnitsArray::iterator it;
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
			CvPlot* pTarget = GET_PLAYER(m_pPlayer->GetID()).ChooseDiplomatTargetPlot(pUnit);
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
#if defined(MOD_BALANCE_CORE)
					UnitProcessed(pUnit->GetID());
					pUnit->finishMoves();
					continue;
#endif
				}
				else if( pUnit->CanReachInXTurns(pTarget,0) )
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
#if defined(MOD_BALANCE_CORE)
					UnitProcessed(pUnit->GetID());
					pUnit->finishMoves();
					continue;
#endif
				}
				else
				{
					ExecuteMoveToTarget(pUnit.pointer(), pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great Diplomat moving to city-state, currently at X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
						LogHomelandMessage(strLogString);
					}
#if defined(MOD_BALANCE_CORE)
					continue;
#endif
				}
			}
		}
		break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			//Handled by economic AI
		break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit.pointer());
#if defined(MOD_BALANCE_CORE)
		UnitProcessed(pUnit->GetID());
		pUnit->finishMoves();
#endif
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
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}
		
		//Do trade mission
		CvPlot* pTarget = GET_PLAYER(m_pPlayer->GetID()).ChooseMessengerTargetPlot(pUnit);
		if(pTarget)
		{
			if(((pUnit->plot() == pTarget) || (pUnit->plot()->getOwner() == pTarget->getOwner())) && pUnit->canMove() && pUnit->canTrade(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_TRADE());
				PlayerTypes ePlayer = pUnit->plot()->getOwner();
				if(ePlayer != NO_PLAYER)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Diplomatic Unit finishing Diplomatic Mission at %s", GET_PLAYER(ePlayer).getCivilizationShortDescription());
						LogHomelandMessage(strLogString);
					}
				}
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				continue;
#endif
			}
			else if( pUnit->CanReachInXTurns(pTarget,0) )
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());
				
				if(((pUnit->plot() == pTarget) || (pUnit->plot()->getOwner() == pTarget->getOwner())) && pUnit->canMove() && pUnit->canTrade(pUnit->plot()))
				{
					pUnit->PushMission(CvTypes::getMISSION_TRADE());
					PlayerTypes ePlayer = pUnit->plot()->getOwner();
					if(ePlayer != NO_PLAYER)
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Diplomatic Unit moving to finish Diplomatic Mission at %s", GET_PLAYER(ePlayer).getCivilizationShortDescription());
							LogHomelandMessage(strLogString);
						}
					}
				}
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				pUnit->finishMoves();
				continue;
#endif
			}
			else
			{
				if(pUnit->plot()->getOwner() == pTarget->getOwner() && pUnit->canMove() && pUnit->canTrade(pUnit->plot()))
				{
					pUnit->PushMission(CvTypes::getMISSION_TRADE());
					PlayerTypes ePlayer = pUnit->plot()->getOwner();
					if(ePlayer != NO_PLAYER)
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Diplomatic Unit finishing Diplomatic Mission at %s", GET_PLAYER(ePlayer).getCivilizationShortDescription());
							LogHomelandMessage(strLogString);
						}
					}
					UnitProcessed(pUnit->GetID());
					continue;
				}
				else
				{
					ExecuteMoveToTarget(pUnit.pointer(),pTarget);

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Diplomatic Unit moving to city-state, currently at X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
						LogHomelandMessage(strLogString);
					}
#if defined(MOD_BALANCE_CORE)
					UnitProcessed(pUnit->GetID());
					continue;
#endif
				}
			}
		}
		//Dangerous?
		else
		{
			if(pUnit->isHuman())
			{
				pUnit->SetAutomateType(NO_AUTOMATE);
				UnitProcessed(pUnit->GetID());
				continue;
			}
			else
			{
				MoveCivilianToSafety(pUnit.pointer());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving Messenger to safety.");
					LogHomelandMessage(strLogString);
				}
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				pUnit->finishMoves();
				continue;
			}
#endif

		}
	}
}
#endif

void CvHomelandAI::ExecuteMerchantMoves()
{
	MoveUnitsArray::iterator it;
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
#if defined(MOD_BALANCE_CORE)
			UnitProcessed(pUnit->GetID());
			pUnit->finishMoves();
			continue;
#endif
			break;
		}
	}
}

void CvHomelandAI::ExecuteProphetMoves()
{
	MoveUnitsArray::iterator it;
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
#if defined(MOD_BALANCE_CORE)
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great Prophet founding a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
					LogHomelandMessage(strLogString);
				}
#else
				CvString strLogString;
				strLogString.Format("Great Prophet founding a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
				LogHomelandMessage(strLogString);
#endif

				pUnit->DoFoundReligion();
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				pUnit->finishMoves();
				continue;
#endif
			}

			// Can I enhance a religion?
			else if(pUnit->CanEnhanceReligion(pUnit->plot()))
			{
#if defined(MOD_BALANCE_CORE)
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
				strLogString.Format("Great Prophet enhancing a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
				LogHomelandMessage(strLogString);
				}
#else
				CvString strLogString;
				strLogString.Format("Great Prophet enhancing a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
				LogHomelandMessage(strLogString);
#endif

				pUnit->DoEnhanceReligion();
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				pUnit->finishMoves();
				continue;
#endif
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
						int iTurns = pUnit->TurnsToReachTarget(pTarget);
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
						ExecuteMoveToTarget(pUnit.pointer(), pTarget);

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
	MoveUnitsArray::iterator it;
	CvPlot* pHolyCityPlot = NULL;
	CvCity* pHolyCity = NULL;

	// Do we have an Apollo program to stay clear of?
	bool bHaveApolloInCapital = false;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	if (!MOD_GLOBAL_BREAK_CIVILIAN_1UPT) {
#endif
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			bHaveApolloInCapital = true;
		}
	}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	}
#endif
	// Do we have a holy city to stay clear of?
	bool bKeepHolyCityClear = false;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	if (!MOD_GLOBAL_BREAK_CIVILIAN_1UPT) {
#endif
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
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	}
#endif

	std::vector<CvPlot*> vPlotsToAvoid;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
			continue;

#if defined(MOD_BALANCE_CORE_MILITARY)
		//might have been used in tactical AI already
		if (!pUnit->canMove())
			continue;
#endif

		// this is for the citadel/culture bomb
		if (pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
		{
			int iValue = 0;

			CvPlot* pTargetPlot = GET_PLAYER(m_pPlayer->GetID()).FindBestGreatGeneralTargetPlot(pUnit.pointer(), vPlotsToAvoid, iValue);
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
						UnitProcessed(pUnit->GetID());
						pUnit->finishMoves();
					}
					else
					{
						//give up
						pUnit->SetGreatPeopleDirective(NO_GREAT_PEOPLE_DIRECTIVE_TYPE);
					}
				}
				else
				{
					//continue moving to target
					if (MoveToUsingSafeEmbark(pUnit, pTargetPlot, true))
					{
						vPlotsToAvoid.push_back(pTargetPlot);
						pUnit->finishMoves();
						UnitProcessed(pUnit->GetID());

						//just for debugging
						pUnit->SetMissionAI(MISSIONAI_BUILD,pTargetPlot,NULL);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Great general moving to build citadel at, X: %d, Y: %d, current location, X: %d, Y: %d", 
								pTargetPlot->getX(), pTargetPlot->getY(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
					else
						pUnit->SetGreatPeopleDirective(NO_GREAT_PEOPLE_DIRECTIVE_TYPE);
				}
			}
#if defined(MOD_BALANCE_CORE)
			else
			{
				//no target for citadel
				pUnit->SetGreatPeopleDirective(NO_GREAT_PEOPLE_DIRECTIVE_TYPE);
			}
#endif
		}


		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			ExecuteGoldenAgeMove(pUnit.pointer());
			continue;
		}

#if defined(MOD_BALANCE_CORE_MILITARY)
		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND && !pUnit->IsCityAttackSupport())
		{
			int iBestScore = 0;
			CvPlot* pBestPlot = 0;

			//this directive should normally be handled in tactical AI (operation moves, close on target or hedgehog)
			//we could use ScoreGreatGeneralPlot() here, but maybe a different algorithm is a good idea
			TacticalAIHelpers::ReachablePlotSet reachablePlots;
			TacticalAIHelpers::GetAllPlotsInReach(pUnit.pointer(),pUnit->plot(),reachablePlots,true,true);
			for (TacticalAIHelpers::ReachablePlotSet::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
			{
				CvPlot* pCandidate = GC.getMap().plotByIndexUnchecked(it->first);
				//plot needs to have a defender, but no be adjacent to the enemy
				UnitHandle pDefender = pCandidate->getBestDefender(pUnit->getOwner());
				if (!pDefender || (pDefender->GetNumEnemyUnitsAdjacent()>0 && !pCandidate->isCity()))
					continue;

				if(pUnit->IsNearGreatGeneral(pCandidate, pUnit.pointer())) //near another general
					continue;

				//we want to have many neighboring units in danger, but our plot should be relatively safe
				//(look at the danger for the defender, the general danger is zero unless the defender is projected to die)
				int iGeneralDanger = m_pPlayer->GetPlotDanger(*pCandidate,pDefender.pointer());
				//careful with overflow, we expect a lot of INT_MAX value here ...
				iGeneralDanger = min(100000,iGeneralDanger);

				//for each candidate plot, look at the neighbors
				int iSupportedDanger = iGeneralDanger;
				CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pCandidate);
				for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
				{
					CvPlot* pLoopPlot = aPlotsToCheck[iCount];
					if (!pLoopPlot)
						continue;

					UnitHandle pSupportedUnit = pLoopPlot->getBestDefender(pUnit->getOwner());
					if (!pSupportedUnit)
						continue;

					iSupportedDanger += m_pPlayer->GetPlotDanger(*pLoopPlot,pSupportedUnit.pointer());
				}

				int iScore = (100*iSupportedDanger)/(iGeneralDanger+1);
				if (iScore>iBestScore && iScore>100)
				{
					iBestScore=iScore;
					pBestPlot=pCandidate;
				}
			}

			if (pBestPlot)
			{
				//we know we can reach it in one turn
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());
				//make sure our defender doesn't run away
				UnitHandle pDefender = pBestPlot->getBestDefender(pUnit->getOwner());
				if (pDefender->canMove())
					pDefender->finishMoves();
				UnitProcessed(pDefender->GetID());
			}
			else
			{
				//no units around ... move to city
				pUnit->SetGreatPeopleDirective(NO_GREAT_PEOPLE_DIRECTIVE_TYPE);
			}
		}
		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND && pUnit->IsCityAttackSupport() && m_pPlayer->IsAtWar())
		{
			int iBestScore = 0;
			CvPlot* pBestPlot = 0;

			//this directive should normally be handled in tactical AI (operation moves, close on target or hedgehog)
			//we could use ScoreGreatGeneralPlot() here, but maybe a different algorithm is a good idea
			TacticalAIHelpers::ReachablePlotSet reachablePlots;
			TacticalAIHelpers::GetAllPlotsInReach(pUnit.pointer(),pUnit->plot(),reachablePlots,true,true);
			for (TacticalAIHelpers::ReachablePlotSet::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
			{

				int iScore = 0;
				CvPlot* pCandidate = GC.getMap().plotByIndexUnchecked(it->first);
				//plot needs to have a defender, but no be adjacent to the enemy
				UnitHandle pDefender = pCandidate->getBestDefender(pUnit->getOwner());
				if (!pDefender)
					continue;

				if(pUnit->IsNearCityAttackSupport(pCandidate, pUnit.pointer())) //near another general
					continue;

				//we want to have many neighboring units in danger, but our plot should be relatively safe
				//(look at the danger for the defender, the general danger is zero unless the defender is projected to die)
				int iGeneralDanger = m_pPlayer->GetPlotDanger(*pCandidate,pDefender.pointer());
				//careful with overflow, we expect a lot of INT_MAX value here ...
				iGeneralDanger = min(100000,iGeneralDanger);

				//for each candidate plot, look at the neighbors
				int iSupportedDanger = iGeneralDanger;
				CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pCandidate);
				for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
				{
					CvPlot* pLoopPlot = aPlotsToCheck[iCount];
					if (!pLoopPlot)
						continue;

					UnitHandle pSupportedUnit = pLoopPlot->getBestDefender(pUnit->getOwner());
					if (!pSupportedUnit)
						continue;

					if(pLoopPlot->IsEnemyCityAdjacent(pUnit->getTeam(),NULL))
						iScore += 5000;

					iSupportedDanger += m_pPlayer->GetPlotDanger(*pLoopPlot,pSupportedUnit.pointer());
				}

				//don't forget adjacency bonus for the candidate plot itself
				if(pCandidate->IsEnemyCityAdjacent(pUnit->getTeam(),NULL))
					iScore += 5000;

				CvCity* pClosestEnemyCity = m_pPlayer->GetTacticalAI()->GetNearestTargetCity(pUnit->plot());
				if(pClosestEnemyCity != NULL)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Found a nearby city target for our Siege Tower: X: %d, Y: %d", pClosestEnemyCity->getX(), pClosestEnemyCity->getY());
						LogHomelandMessage(strLogString);
					}
					iScore += (1000 - (plotDistance(pCandidate->getX(), pCandidate->getY(), pClosestEnemyCity->getX(), pClosestEnemyCity->getY()) * 5));
				}

				iScore += (100*iSupportedDanger)/(iGeneralDanger+1);
				if (iScore>iBestScore && iScore>100)
				{
					iBestScore=iScore;
					pBestPlot=pCandidate;
				}
			}

			if (pBestPlot)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Siege Tower moving towards: X: %d, Y: %d", pBestPlot->getX(), pBestPlot->getY());
					LogHomelandMessage(strLogString);
				}
				//we know we can reach it in one turn
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());
				//make sure our defender doesn't run away
				UnitHandle pDefender = pBestPlot->getBestDefender(pUnit->getOwner());
				if (pDefender->canMove())
					pDefender->finishMoves();
				UnitProcessed(pDefender->GetID());
				continue;
			}
		}
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
		if( pUnit->GetGreatPeopleDirective() == NO_GREAT_PEOPLE_DIRECTIVE_TYPE || 
			(pUnit->IsCityAttackSupport() && pUnit->canRecruitFromTacticalAI() && ((pUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS()) < GC.getGame().getGameTurn())) )
		{
			int iUnitLoop = 0;
			int iTotalGenerals = 0;
			for (const CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
			{
				if (pLoopUnit != NULL && (pLoopUnit->IsGreatGeneral() || pLoopUnit->IsCityAttackSupport()))
				{
					iTotalGenerals++;
				}
			}

			// Score cities to move to
			CvCity* pLoopCity;
			int iLoopCity = 0;
			CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> weightedCityList;
			for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
			{
				int iNumCommanders = 0;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
				if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) {
#endif
				if (bHaveApolloInCapital && pLoopCity->isCapital())
				{
					continue;
				}

				if (bKeepHolyCityClear && pLoopCity == pHolyCity)
				{
					continue;
				}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
				}
#endif
				// Don't go here if a different general or admiral is already present
				bool bSkipCity = false;
				CvPlot* pTarget = pLoopCity->plot();
				for(int iUnitLoop = 0; iUnitLoop < pTarget->getNumUnits(); iUnitLoop++)
				{
					CvUnit *pLoopUnit = pTarget->getUnitByIndex(iUnitLoop);
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
					if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) 
					{
#endif
						if(pLoopUnit->IsGreatGeneral() && pLoopUnit->GetID() != pUnit->GetID())
						{
							bSkipCity = true;
							break;
						}
						else if(pLoopUnit->IsGreatAdmiral() && pLoopUnit->GetID() != pUnit->GetID())
						{
							bSkipCity = true;
							break;
						}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
					}
#endif
					if(pLoopUnit->IsGreatGeneral() && pLoopUnit->GetID() != pUnit->GetID())
					{
						iNumCommanders++;
					}
				}

				if(iNumCommanders > 0)
				{
					bSkipCity = true;
				}
				if(iTotalGenerals > m_pPlayer->getNumCities())
				{
					bSkipCity = false;
				}

				if(bSkipCity)
				{
					continue;
				}

				// Weight is current hitpoints
				int iWeight = (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage());

				// Subtract off the distance
				iWeight -= plotDistance(pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY())*10;
				
				if (iWeight>0)
				{
					if(pLoopCity->HasGarrison())
					{
						iWeight *= 2;
					}
					if(iNumCommanders > 0)
					{
						//discourage more than one general in a city
						iWeight /= (iNumCommanders + 1);
					}

					weightedCityList.push_back(pLoopCity, iWeight);
				}
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
						strLogString.Format("Great General still in most favored city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
						LogHomelandMessage(strLogString);
					}
				}
				// Move normally to this city
				else if(pChosenCity)
				{
					MoveToUsingSafeEmbark(pUnit,pChosenCity->plot(),true);
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving Great General to city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
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
						strLogString.Format("No place to move Great General at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}
#else

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
					int iTurns = pUnit->TurnsToReachTarget(pTarget);
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
#endif
		}		
	}
}

/// Moves a great admiral into an important coastal city to aid its defense
void CvHomelandAI::ExecuteAdmiralMoves()
{
	MoveUnitsArray::iterator it;
	CvPlot* pHolyCityPlot = NULL;
	CvCity* pHolyCity = NULL;
	// Do we have an Apollo program to stay clear of?
	bool bHaveApolloInCapital = false;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) {
#endif
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			bHaveApolloInCapital = true;
		}
	}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	}
#endif
	// Do we have a holy city to stay clear of?
	bool bKeepHolyCityClear = false;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) {
#endif
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
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	}
#endif
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
#if defined(MOD_BALANCE_CORE_MILITARY)
		//if he's a commander but not in an army, put him up in a city for a while
		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
		{
			if(pUnit->canGetFreeLuxury())
			{
				pUnit->PushMission(CvTypes::getMISSION_FREE_LUXURY());
				UnitProcessed(pUnit->GetID());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("%s got a free luxury for this civ, X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
				continue;
			}
			else
			{
				CvCity* pLoopCity;
				int iLoopCity = 0;
				CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> weightedCityList;
				for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
				{
					if (!pLoopCity->isCoastal())
					{
						continue;
					}

					int iTurns = pUnit->TurnsToReachTarget(pLoopCity->plot());
					int iWeight = pLoopCity->plot()->GetSizeLargestAdjacentWater();

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
				
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pChosenCity->getX(), pChosenCity->getY());
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving Great Admiral normally to city of %s, X: %d, Y: %d", pChosenCity->getName().GetCString(), pChosenCity->getX(), pChosenCity->getY());
						LogHomelandMessage(strLogString);
					}
					continue;
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
					continue;
				}
			}

		}

		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND)
		{
			//this should have been handled in tactical AI - so reset him
			if (pUnit->canMove())
				pUnit->SetGreatPeopleDirective(NO_GREAT_PEOPLE_DIRECTIVE_TYPE);
		}


		//if he's a commander but not in an army, put him up in a city for a while
		if(pUnit->GetGreatPeopleDirective() == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
		{
#endif

		bool bNotAtFriendlyCity = !pUnit->plot()->isCity() || pUnit->plot()->getOwner() != pUnit->getOwner();

			// Score cities to move to
		CvCity* pLoopCity;
		int iLoopCity = 0;
		CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> weightedCityList;
		for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
			int iNumCommanders = 0;
#endif
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
			if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) {
#endif
			if (bHaveApolloInCapital && pLoopCity->isCapital())
			{
				continue;
			}

			if (bKeepHolyCityClear && pLoopCity == pHolyCity)
			{
				continue;
			}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
			}
#endif
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
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
				if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) {
#endif
#if defined(MOD_BALANCE_CORE_MILITARY)
				if(pLoopUnit->IsGreatGeneral() && pLoopUnit->GetID() != pUnit->GetID())
				{
					bSkipCity = true;
					break;
				}
				else if(pLoopUnit->IsGreatAdmiral() && pLoopUnit->GetID() != pUnit->GetID())
				{
					bSkipCity = true;
					break;
				}
#else
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
#endif
#if defined(MOD_BALANCE_CORE_MILITARY)
				}
				if(pLoopUnit->IsGreatAdmiral() && pLoopUnit->GetID() != pUnit->GetID())
				{
					iNumCommanders++;
				}
#endif
			}
#if defined(MOD_BALANCE_CORE_MILITARY)
			int iUnitLoop = 0;
			int iTotalAdmirals = 0;
			for (const CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
			{
				if (pLoopUnit != NULL && pLoopUnit->IsGreatAdmiral())
				{
					iTotalAdmirals++;
				}
			}
			if(iNumCommanders > 0)
			{
				bSkipCity = true;
			}
			if(iTotalAdmirals > m_pPlayer->getNumCities())
			{
				bSkipCity = false;
			}
			if(bSkipCity)
			{
				continue;
			}
			int iTurns = pUnit->TurnsToReachTarget(pLoopCity->plot(),false,false,23);

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
			if(pLoopCity->HasGarrison())
			{
				iWeight *= 2;
			}
			if(iNumCommanders > 0)
			{
				iWeight /= (iNumCommanders + 1);
			}

#else
			if(bSkipCity)
			{
				continue;
			}

			int iTurns = pUnit->TurnsToReachTarget(pLoopCity->plot());

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
#endif

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
#if defined(MOD_BALANCE_CORE)
				continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
				continue;
#endif
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
#if defined(MOD_BALANCE_CORE)	
				continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
			continue;
#endif
		}
#if defined(MOD_BALANCE_CORE_MILITARY)
		}
#endif
	}
}

// Get a missionary to the best city, then spread the word
void CvHomelandAI::ExecuteMissionaryMoves()
{
	MoveUnitsArray::iterator it;
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
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				continue;
#endif
			}
			else
			{
				ExecuteMoveToTarget(pUnit.pointer(),pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				continue;
#endif
			}
		}
#if defined(MOD_BALANCE_CORE)
		else
		{
			if(pUnit->isHuman())
			{
				pUnit->SetAutomateType(NO_AUTOMATE);
				UnitProcessed(pUnit->GetID());
				continue;
			}
		}
#endif
	}
}

// Get a inquisitor to the best city
void CvHomelandAI::ExecuteInquisitorMoves()
{
	MoveUnitsArray::iterator it;
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
#if defined(MOD_BALANCE_CORE)
				UnitProcessed(pUnit->GetID());
				pUnit->finishMoves();
				continue;
#endif
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
#if defined(MOD_BALANCE_CORE)
					UnitProcessed(pUnit->GetID());
					pUnit->finishMoves();
					continue;
#endif
				}
				else
				{
					CvAssertMsg(false, "Internal error with Inquisitor AI move, contact Ed.");
				}
			}
			else
			{
				ExecuteMoveToTarget(pUnit.pointer(), pTarget);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to plot adjacent to heresy removal city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
#if defined(MOD_BALANCE_CORE)
				continue;
#endif
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
		MoveUnitsArray::iterator it;
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
		MoveUnitsArray::iterator it;
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

			else if (pUnit->TurnsToReachTarget(pCapitalCity->plot()) == 0)
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
		MoveUnitsArray::iterator it;
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
	MoveUnitsArray::iterator it;
	std::vector<CvPlot*> checkedPlotList;

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		UnitHandle pUnit = m_pPlayer->getUnit(it->GetID());

		if(pUnit)
		{
			// Am I eligible to intercept?
#if defined(MOD_AI_SMART_AIR_TACTICS) && defined(MOD_BALANCE_CORE_MILITARY)
			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetTacticalAI()->ShouldRebase(pUnit.pointer()))
#else
			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit.pointer()))
#endif
			{
				CvPlot* pUnitPlot = pUnit->plot();
				int iNumNearbyBombers = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), false/*bCountFighters*/, true/*bCountBombers*/);
				int iNumNearbyFighters = m_pPlayer->GetMilitaryAI()->GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), true/*bCountFighters*/, false/*bCountBombers*/);
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
	MoveUnitsArray::iterator it;

	if (m_CurrentMoveUnits.empty())
		return;
	
	int iTargetRange = 6; //basic aircraft and guided missile range
	int nAirUnitsInCarriers = 0;
	int nAirUnitsInCities = 0;
	int nSlotsInCarriers = 0;
	int nAirUnitsOffensive = 0;
	int nAirUnitsDefensive = 0;

	if (GC.getGame().GetTacticalAnalysisMap()->GetCurrentPlayer() != m_pPlayer->GetID())
	{
		OutputDebugString("ExecuteAircraftMoves: Tactical map not up to date!\n");
		return;
	}

	//in general we want to go to conflict zones but not if we are in danger of losing the base
	//unfortunately it may be necessary to do the rebasing in multiple steps if the distance is too far
	std::vector<SPlotWithScore> vPotentialBases;
	std::map<int,int> scoreLookup;

	//for pathfinding we need to assign the carriers to cities
	int iLoopCity = 0;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		pLoopCity->ClearAttachedUnits();

	//check the carriers and their potential cargo
	int iLoopUnit = 0;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		CvPlot* pLoopUnitPlot = pLoopUnit->plot();

		//first some statistics
		if(pLoopUnit->getDomainType() == DOMAIN_AIR)
		{
			switch (pLoopUnit->getUnitInfo().GetDefaultUnitAIType())
			{
			case UNITAI_DEFENSE_AIR:
					nAirUnitsDefensive++;
					break;
			case UNITAI_ATTACK_AIR:
			case UNITAI_ICBM:
			case UNITAI_MISSILE_AIR:
					nAirUnitsOffensive++;
					break;
			}

			if (pLoopUnit->getTransportUnit()!=NULL)
				nAirUnitsInCarriers++;
			else if (pLoopUnit->plot()->isCity())
				nAirUnitsInCities++;
		}
		else if (pLoopUnit->isAircraftCarrier())
		{
			nSlotsInCarriers += pLoopUnit->cargoSpace();

			//for simplicity we don't do carrier to carrier rebasing, only carrier to city
			CvCity* pRefCity = m_pPlayer->GetClosestCity(pLoopUnit->plot());
			if (pRefCity)
			{
				pRefCity->AttachUnit(pLoopUnit);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Linking %s (%d) at %d,%d to %s at %d,%d (for rebase)", 
						pLoopUnit->getName().c_str(), pLoopUnit->GetID(), pLoopUnit->getX(), pLoopUnit->getY(),
						pRefCity->getName().c_str(), pRefCity->getX(), pRefCity->getY());
					LogHomelandMessage(strLogString);
				}
			}

			int iScore = HomelandAIHelpers::ScoreAirBase(pLoopUnitPlot,m_pPlayer->GetID(), iTargetRange );
			vPotentialBases.push_back( SPlotWithScore( pLoopUnitPlot, iScore) );
			scoreLookup[pLoopUnitPlot->GetPlotIndex()] = iScore;
		}
	}

	//check the cities
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		CvPlot* pTarget = pLoopCity->plot();

		int iScore = HomelandAIHelpers::ScoreAirBase(pTarget,m_pPlayer->GetID(), iTargetRange );
		vPotentialBases.push_back( SPlotWithScore( pTarget, iScore ) );
		scoreLookup[pTarget->GetPlotIndex()] = iScore;
	}

	//then decide what to do with our units
	//combat-ready units want to go to the hotspots
	//healing units want to go to the backwaters
	std::vector<CvUnit*> vCombatReadyUnits, vHealingUnits;

	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
			continue;

		//tactical AI commandeers some units, others are healing, some are up for rebasing
		if(!m_pPlayer->GetTacticalAI()->ShouldRebase(pUnit))
		{
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
			UnitProcessed(pUnit->GetID());
			continue;
		}

		//only combat-ready units for now
		if(m_pPlayer->GetTacticalAI()->IsUnitHealing(it->GetID()))
			vHealingUnits.push_back(pUnit);
		else
			vCombatReadyUnits.push_back(pUnit);
	}

	//todo: make sure the units are not blocking each other because of max capacity
	//todo: sort also by plot distance? right now there are many targets with the same score

	//for now we simply try to move units to the most desirable base
	//lowest score first for now
	std::stable_sort(vPotentialBases.begin(),vPotentialBases.end());

	for (size_t i=0; i<vHealingUnits.size(); ++i)
	{
		CvUnit* pUnit = vHealingUnits[i];

		//do we need to move (to make space for combat ready units ...)
		if (scoreLookup[pUnit->plot()->GetPlotIndex()]>0)
		{
			CvPlot* pNewBase = NULL;

			//boring places come first!
			for (std::vector<SPlotWithScore>::iterator it=vPotentialBases.begin(); it!=vPotentialBases.end(); ++it)
			{
				//unsuitable
				if (it->score<0)
					continue;

				//healing in cities only
				if (!pUnit->canRebase(NULL) || !(it->pPlot->isCity()))
					continue;

				//apparently we're already in the best possible base?
				if (pUnit->plot() == it->pPlot)
					continue;

				//see if we can rebase there directly
				if (pUnit->canRebaseAt(pUnit->plot(),it->pPlot->getX(),it->pPlot->getY()))
				{
					pNewBase = it->pPlot;
					break;
				}
				else
				{
					//need to use the pathfinder
					SPathFinderUserData data(pUnit, 0, 6);
					data.ePathType = PT_AIR_REBASE;
					if (GC.GetStepFinder().GeneratePath(pUnit->getX(),pUnit->getY(),it->pPlot->getX(),it->pPlot->getY(),data))
					{
						pNewBase = GC.GetStepFinder().GetPathFirstPlot();
						break;
					}
				}
			}

			if (pNewBase)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Rebasing %s (%d) from %d,%d to %d,%d for healing (score %d)", 
						pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY(),
						pNewBase->getX(), pNewBase->getY(), scoreLookup[pNewBase->GetPlotIndex()]);
					LogHomelandMessage(strLogString);
				}

				pUnit->PushMission(CvTypes::getMISSION_REBASE(), pNewBase->getX(), pNewBase->getY());
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Failed to rebase %s (%d) at %d,%d for healing", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Don't need to rebase %s (%d) at %d,%d for healing", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}

		//that's it for this turn, whether we rebased successfully or not
		pUnit->PushMission(CvTypes::getMISSION_SKIP());
		UnitProcessed(pUnit->GetID());
	}

	//exciting places first - we should have some space there now
	std::reverse(vPotentialBases.begin(),vPotentialBases.end());
	for (size_t i=0; i<vCombatReadyUnits.size(); ++i)
	{
		CvUnit* pUnit = vCombatReadyUnits[i];
		CvPlot* pNewBase = NULL;
		for (std::vector<SPlotWithScore>::iterator it=vPotentialBases.begin(); it!=vPotentialBases.end(); ++it)
		{
			//unsuitable - we want to fight!
			if (it->score<0)
				continue;

			//make sure the unit fits the destination (ie missile to cruiser, fighter to carrier)
			if (!pUnit->canRebase(NULL) || !pUnit->canLoad(*(it->pPlot)))
				continue;

			//apparently we're already in the best possible base?
			if (scoreLookup[pUnit->plot()->GetPlotIndex()]>=it->score )
				break;

			//sometimes you just don't fit in
			if (!HomelandAIHelpers::IsGoodUnitMix(it->pPlot,pUnit))
				continue;

			//see if we can rebase there directly
			if (pUnit->canRebaseAt(pUnit->plot(),it->pPlot->getX(),it->pPlot->getY()))
			{
				pNewBase = it->pPlot;
				break;
			}
			else
			{
				//need to use the pathfinder
				SPathFinderUserData data(pUnit, 0, 6);
				data.ePathType = PT_AIR_REBASE;
				if (GC.GetStepFinder().GeneratePath(pUnit->getX(),pUnit->getY(),it->pPlot->getX(),it->pPlot->getY(),data))
				{
					pNewBase = GC.GetStepFinder().GetPathFirstPlot();
					break;
				}
			}
		}

		if (pNewBase)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Rebasing %s (%d) from %d,%d to %d,%d for combat (score %d)", 
					pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY(),
					pNewBase->getX(), pNewBase->getY(), scoreLookup[pNewBase->GetPlotIndex()]);
				LogHomelandMessage(strLogString);
			}

			pUnit->PushMission(CvTypes::getMISSION_REBASE(), pNewBase->getX(), pNewBase->getY());
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Failed to rebase %s (%d) at %d,%d for combat", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
		}

		//that's it for this turn, whether we rebased successfully or not
		pUnit->PushMission(CvTypes::getMISSION_SKIP());
		UnitProcessed(pUnit->GetID());
	}
}


/// Fleeing to safety for civilian units
#if defined(MOD_AI_SECONDARY_WORKERS)
bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits, bool bSecondary)
#else
bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits)
#endif
{
	WeightedPlotVector aBestPlotList;
	TacticalAIHelpers::ReachablePlotSet reachablePlots;
	TacticalAIHelpers::GetAllPlotsInReach(pUnit,pUnit->plot(),reachablePlots,true,true,true);
	for (TacticalAIHelpers::ReachablePlotSet::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
	{
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(it->first);

			if(!pLoopPlot->isValidMovePlot(pUnit->getOwner()))
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
				iValue += 5;
			}

#if defined(MOD_AI_SECONDARY_WORKERS)
			if (bSecondary && pUnit->getDomainType() == DOMAIN_LAND && pLoopPlot->isWater()) {
				// being embarked is NOT safe!
				iValue -= 125;
			}
#endif

			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity && pCity->getTeam() == pUnit->getTeam())
			{
				iValue += pCity->getStrengthValue() * (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDamage());
				if(pCity->getDamage() <= 0)
				{
					//We should always hide in cities if we can.
					iValue += 1;
					iValue *= 1000;
				}
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

			if (m_pPlayer->GetPlotDanger(*pLoopPlot,pUnit)==INT_MAX)
				continue;

			iValue -= m_pPlayer->GetPlotDanger(*pLoopPlot);
			aBestPlotList.push_back(pLoopPlot, iValue);
		}
	}

	// Now loop through the sorted score list and go to the best one we can reach in one turn.
	CvPlot* pBestPlot = NULL;
	//we already know that the plot is in reach
	if (aBestPlotList.size()>0)
	{
		aBestPlotList.SortItems(); //highest score will be first
		pBestPlot=aBestPlotList.GetElement(0);
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

			MoveToUsingSafeEmbark(pUnit, pBestPlot, true);
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

	MoveUnitsArray::iterator it;
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

	MoveUnitsArray::iterator it;
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
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());
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
#if defined(MOD_BALANCE_CORE)
		//Let's make sure that we exclude forts here. If the first is a sentry, then we can make the check.
		if((it->GetTargetType() == AI_HOMELAND_TARGET_FORT))
		{
			m_TargetedSentryPoints.push_back(*it);
		}
		else
		{
#endif
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
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}
}

/// Don't allow adjacent tiles to both be mobile reserve muster points
void CvHomelandAI::EliminateAdjacentHomelandRoads()
{
#if defined(MOD_BALANCE_CORE)
	// Let's just sort the list- no need to break off adjacent targets (otherwise we end up with muster points off roads, which is bad).
	std::stable_sort(m_TargetedHomelandRoads.begin(), m_TargetedHomelandRoads.end());
#else
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
#endif
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
				// Civilians or units in armies aren't useful for any of these moves
				if(!pLoopUnit->canMove() || !pLoopUnit->IsCombatUnit() || pLoopUnit->getArmyID() != -1)
				{
					continue;
				}
				if(pLoopUnit->getDomainType()!=DOMAIN_LAND)
					continue;

				bool bSuitableUnit = false;
				bool bHighPriority = false;

				switch(eMove)
				{
				case AI_HOMELAND_MOVE_GARRISON:
				case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
					// Want to put ranged units in cities to give them a ranged attack
					if(pLoopUnit->isRanged())
					{
						bSuitableUnit = true;
						bHighPriority = true;
					}

					// Don't use non-combatants
					else if(pLoopUnit->IsCanAttack())
					{
						// Don't put units with a combat strength boosted from promotions in cities, these boosts are ignored
						if(pLoopUnit->getDefenseModifier() == 0 )
						{
							bSuitableUnit = true;
						}
					}
					break;

				case AI_HOMELAND_MOVE_SENTRY:
					// No ranged units as sentries
					if(pLoopUnit->isFortifyable() && !pLoopUnit->noDefensiveBonus() && (UnitAITypes)pLoopUnit->getUnitInfo().GetDefaultUnitAIType() != UNITAI_EXPLORE)
					{
						bSuitableUnit = true;

						// Units with extra defense are especially valuable
						if((pLoopUnit->getDefenseModifier() > 0) || (pLoopUnit->getExtraRangedDefenseModifier() > 0) || (pLoopUnit->getExtraVisibilityRange() > 0))
						{
							bHighPriority = true;
						}
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
						bHighPriority = true;

					bSuitableUnit = true;
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
		MoveUnitsArray tempList;
		MoveUnitsArray::iterator it;
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

//	---------------------------------------------------------------------------
/// Get the closest 
bool CvHomelandAI::GetClosestUnitByTurnsToTarget(CvHomelandAI::MoveUnitsArray &kMoveUnits, CvPlot* pTarget, int iMaxTurns, CvUnit** ppClosestUnit, int* piClosestTurns)
{
	CvUnit* pBestUnit = NULL;
	int iMinTurns = iMaxTurns;
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
			
			int iMoves = pLoopUnit->TurnsToReachTarget(pTarget, false, false, iMinTurns);
			it->SetMovesToTarget(iMoves);
			// Did we make it at all?
			if (iMoves != MAX_INT)
			{
				// Reasonably close?
#if defined(MOD_BALANCE_CORE)
				if (iDistance == 0 || iMoves == 0)
				{
					pBestUnit = pLoopUnit.pointer();
					iMinTurns = iMoves;
					break;
				}
				if (iMoves <= iDistance && iMoves <= iMaxTurns && iMoves < iMinTurns)
#else
				if (iDistance == 0 || (iMoves <= iDistance && iMoves <= iMaxTurns && iMoves < iMinTurns))
#endif
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
CvUnit* CvHomelandAI::GetBestUnitToReachTarget(CvPlot* pTarget, int iMaxTurns)
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
			if(!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION ))
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
			if(!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION ))
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
	CvUnit *pHighPrio=0,*pNormalPrio=0;
	int iHighPrioMoves=INT_MAX,iNormalPrioMoves=INT_MAX;
	GetClosestUnitByTurnsToTarget(m_CurrentMoveHighPriorityUnits, pTarget, iMaxTurns, &pHighPrio, &iHighPrioMoves);
	GetClosestUnitByTurnsToTarget(m_CurrentMoveUnits, pTarget, iMaxTurns, &pNormalPrio, &iNormalPrioMoves);

	CvUnit* pBestUnit = NULL;
	if (pHighPrio)
	{
		// Don't move high priority unit if regular priority unit is closer
		if (pNormalPrio && iNormalPrioMoves < iHighPrioMoves)
			pBestUnit = pNormalPrio;
		else
			pBestUnit = pHighPrio;
	}
	else
		pBestUnit = pNormalPrio;

	return pBestUnit;
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
			if(!pTarget->isRevealed(m_pPlayer->getTeam()))
			{
				continue;
			}
			if(pTarget->getImprovementType() != NO_IMPROVEMENT && pUnit->IsAutomated() && pUnit->GetAutomateType() == AUTOMATE_ARCHAEOLOGIST)
			{
				if(GC.getImprovementInfo((ImprovementTypes) pTarget->getImprovementType())->IsCreatedByGreatPerson())
				{
					continue;
				}
			}

			int iTurns = pUnit->TurnsToReachTarget(pTarget, false, false, iBestTurns);
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
	bool bHasDirective = m_pPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pUnit, aDirective, ciDirectiveSize, false, false, bSecondary || pUnit->isHuman());
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

			if(eMission == CvTypes::getMISSION_MOVE_TO())
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), aDirective[0].m_sX, aDirective[0].m_sY, 0, false, false, MISSIONAI_BUILD, pPlot);

				//do we have movement left?
				if (pUnit->getMoves()>0)
					eMission = CvTypes::getMISSION_BUILD();
				else
				{
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
				}
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
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), aDirective[0].m_eBuild, aDirective[0].m_eDirective, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);
				}

				CvAssertMsg(!pUnit->ReadyToMove(), "Worker did not do their mission this turn. Could cause game to hang.");
				if(pUnit->ReadyToMove())
				{
					pUnit->finishMoves();
				}
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

	if(eDomain == DOMAIN_LAND)
	{
		if(!pUnit->CanEverEmbark() && pPlot->needsEmbarkation(pUnit))
		{
			return false;
		}
	}
	else if(eDomain == DOMAIN_SEA && !pPlot->isWater())
	{
		return false;
	}

	// don't let the auto-explore end it's turn in a city
	CvCity* pCity = pPlot->getPlotCity();
	if(pCity && pCity->getOwner() != pUnit->getOwner())
	{
		return false;
	}

	if(!pUnit->canMoveInto(*pPlot))
	{
		return false;
	}

	return true;
}

/// Move an exploring unit to a designated target (special function exposed to Lua)
bool CvHomelandAI::ExecuteSpecialExploreMove(CvUnit* pUnit, CvPlot* pTargetPlot)
{
	SPathFinderUserData data(pUnit,CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE | CvUnit::MOVEFLAG_IGNORE_DANGER);
	bool bCanFindPath = GC.GetPathFinder().GeneratePath(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY(), data);
	if(bCanFindPath)
	{
		CvPlot* pPlot = GC.GetPathFinder().GetPathEndTurnPlot();
		if(pPlot)
		{
			CvAssert(!pUnit->atPlot(*pPlot));
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("UnitID: %d Moving to script explore target, X: %d, Y: %d, from X: %d Y: %d", pUnit->GetID(), pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY(), CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE | CvUnit::MOVEFLAG_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pPlot);
			pUnit->finishMoves();
			UnitProcessed(pUnit->GetID());

			return true;
		}
	}
	return false;
}
#if defined(MOD_BALANCE_CORE)
bool CvHomelandAI::FindTestArchaeologistPlotPrimer(CvUnit *pUnit)
{
	if(pUnit->AI_getUnitAIType() != UNITAI_ARCHAEOLOGIST)
	{
		return false;
	}
	CvPlot* pLoopPlot;
	CvHomelandTarget newTarget;
	TeamTypes eTeam = m_pPlayer->getTeam();
	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	int iI;
	for(iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{
			// ... antiquity site?
			if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()))
			{
				if(pLoopPlot->getOwner() != NO_PLAYER)
				{
					if(pLoopPlot->getOwner() == m_pPlayer->GetID() || !m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoDiggingPromise(pLoopPlot->getOwner()))
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
		}
	}
	return false;
}
/// Find best target for this archaeologist
CvPlot* CvHomelandAI::FindTestArchaeologistPlot(CvUnit *pUnit)
{
	if(pUnit->AI_getUnitAIType() != UNITAI_ARCHAEOLOGIST)
	{
		return NULL;
	}
	CvPlot *pBestTarget = NULL;
	CvPlot* pLoopPlot;
	CvHomelandTarget newTarget;
	TeamTypes eTeam = m_pPlayer->getTeam();
	int iBestTurns = MAX_INT;
	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();
	int iI;
	for(iI = 0; iI < iNumPlots; iI++)
	{
		pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{
			// ... antiquity site?
			if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()))
			{
				if(pLoopPlot->getOwner() != NO_PLAYER)
				{
					if(pLoopPlot->getOwner() == m_pPlayer->GetID() || !m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoDiggingPromise(pLoopPlot->getOwner()))
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData(pLoopPlot);
						m_TargetedAntiquitySites.push_back(newTarget);
					}
				}
				else
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					m_TargetedAntiquitySites.push_back(newTarget);
				}
			}
		}
	}
	// Reverse the logic from most of the Homeland moves; for this we'll loop through units and find the best targets for them (instead of vice versa)
	std::vector<CvHomelandTarget>::iterator it;
	for (it = m_TargetedAntiquitySites.begin(); it != m_TargetedAntiquitySites.end(); it++)
	{
		CvPlot* pTarget = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
		if (m_pPlayer->GetPlotDanger(*pTarget) == 0)
		{
			if(!pTarget->isRevealed(m_pPlayer->getTeam()))
			{
				continue;
			}
			int iTurns = pUnit->TurnsToReachTarget(pTarget, false, false, iBestTurns);

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
	else
	{
		if(pUnit->isHuman())
		{
			pUnit->SetAutomateType(NO_AUTOMATE);
			UnitProcessed(pUnit->GetID());
		}
	}

	return pBestTarget;
}
#endif
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
}

//	---------------------------------------------------------------------------
void CvHomelandAI::ClearCurrentMoveHighPriorityUnits()
{
	m_CurrentMoveHighPriorityUnits.clear();
}
//	---------------------------------------------------------------------------

#if defined(MOD_BALANCE_CORE_MILITARY)

bool CvHomelandAI::MoveToUsingSafeEmbark(UnitHandle pUnit, CvPlot* pTargetPlot, bool bMustBeSafeOnLandToo)
{
	int iMoveFlags = CvUnit::MOVEFLAG_SAFE_EMBARK;
	if (!bMustBeSafeOnLandToo)
		iMoveFlags |= CvUnit::MOVEFLAG_IGNORE_DANGER;

	if(pUnit->GeneratePath(pTargetPlot,iMoveFlags))
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY(), iMoveFlags);
		return true;
	}
	else
	{
		// Embarked and in danger? We need to do something!
		if (pUnit->isEmbarked())
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER);
			return true;
		}
		else
		{
			// No safe path so just stay put and fortify until life improves for you.
			if (pUnit->canFortify(pUnit->plot()))
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			else
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			return false;
		}
	}
}
/// Move up to our target (this time within 2 spaces) avoiding our own units if possible
bool CvHomelandAI::MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, bool bLand)
{
	// Look at spaces adjacent to target
	for(int iI = RING0_PLOTS; iI < RING2_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pTarget->getX(), pTarget->getY(), iI);
		if(pLoopPlot != NULL && pLoopPlot->isWater() != bLand && 
			plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pTarget->getX(), pTarget->getY() <= 2))
		{
			if(pUnit->canMoveInto(*pLoopPlot,CvUnit::MOVEFLAG_DESTINATION ))
			{
				// And if it is a city, make sure we are friends with them, else we will automatically attack
				if(pLoopPlot->getPlotCity() == NULL || pLoopPlot->isFriendlyCity(*pUnit, false))
				{
					// Find a path to this space
					if(pUnit->GeneratePath(pLoopPlot))
					{
						// Go ahead with mission
						return MoveToUsingSafeEmbark(pUnit, pLoopPlot, false);
					}
				}
			}
		}
	}
	return false;
}


int g_currentHomelandUnitToTrack = 0;

const char* homelandMoveNames[] =
{
	"AI_HOMELAND_MOVE_UNASSIGNED",
	"AI_HOMELAND_MOVE_EXPLORE",
	"AI_HOMELAND_MOVE_EXPLORE_SEA",
	"AI_HOMELAND_MOVE_SETTLE",
	"AI_HOMELAND_MOVE_GARRISON",
	"AI_HOMELAND_MOVE_HEAL",
	"AI_HOMELAND_MOVE_TO_SAFETY",
	"AI_HOMELAND_MOVE_MOBILE_RESERVE",
	"AI_HOMELAND_MOVE_SENTRY",
	"AI_HOMELAND_MOVE_WORKER",
	"AI_HOMELAND_MOVE_WORKER_SEA",
	"AI_HOMELAND_MOVE_PATROL",
	"AI_HOMELAND_MOVE_UPGRADE",
	"AI_HOMELAND_MOVE_ANCIENT_RUINS",
	"AI_HOMELAND_MOVE_GARRISON_CITY_STATE",
	"AI_HOMELAND_MOVE_WRITER",
	"AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE",
	"AI_HOMELAND_MOVE_MUSICIAN",
	"AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH",
	"AI_HOMELAND_MOVE_MERCHANT_TRADE",
	"AI_HOMELAND_MOVE_ENGINEER_HURRY",
	"AI_HOMELAND_MOVE_GENERAL",
	"AI_HOMELAND_MOVE_ADMIRAL",
	"AI_HOMELAND_MOVE_SPACESHIP_PART",
	"AI_HOMELAND_MOVE_AIRCRAFT_REBASE",
	"AI_HOMELAND_MOVE_TREASURE",
	"AI_HOMELAND_MOVE_PROPHET_RELIGION",
	"AI_HOMELAND_MOVE_MISSIONARY",
	"AI_HOMELAND_MOVE_INQUISITOR",
	"AI_HOMELAND_MOVE_TRADE_UNIT",
	"AI_HOMELAND_MOVE_ARCHAEOLOGIST",
	"AI_HOMELAND_MOVE_ADD_SPACESHIP_PART",
	"AI_HOMELAND_MOVE_AIRLIFT",
	"AI_HOMELAND_MOVE_DIPLOMAT_EMBASSY",
	"AI_HOMELAND_MOVE_MESSENGER",
};

const char* directiveNames[] = 
{
	"GOLDEN_AGE",
	"USE_POWER",
	"CONSTRUCT_IMPROVEMENT",
	"SPREAD_RELIGION",
	"CULTURE_BLAST",
	"TOURISM_BLAST",
	"FIELD_COMMAND",
};

void CHomelandUnitArray::push_back(const CvHomelandUnit& unit)
{
	m_vec.push_back(unit);

	CvUnit* pUnit = m_owner ? m_owner->getUnit( unit.GetID() ) : NULL;

	if (pUnit)
	{
		//not a nice design to use a global variable here, but it's easier than modifying the code in 30 places
		pUnit->setHomelandMove( m_currentHomelandMove );

		if (unit.GetID()==g_currentHomelandUnitToTrack)
		{
			CvPlayer& owner = GET_PLAYER(pUnit->getOwner());
			OutputDebugString( CvString::format("turn %03d: using %s %s %d for homeland move %s. hitpoints %d, pos (%d,%d), danger %d\n", 
				GC.getGame().getGameTurn(), owner.getCivilizationAdjective(), pUnit->getName().c_str(), g_currentHomelandUnitToTrack,
				homelandMoveNames[(int)m_currentHomelandMove+1], 
				pUnit->GetCurrHitPoints(), pUnit->getX(), pUnit->getY(), owner.GetPlotDanger(*(pUnit->plot()),pUnit) ) );
		}
	}
}
#endif

// HELPER FUNCTIONS

/// Sort CvHomelandUnit by a non-standard criteria
bool HomelandAIHelpers::CvHomelandUnitAuxIntSort(const CvHomelandUnit& obj1, const CvHomelandUnit& obj2)
{
	return obj1.GetAuxIntData() < obj2.GetAuxIntData();
}

bool HomelandAIHelpers::CvHomelandUnitAuxIntReverseSort(const CvHomelandUnit& obj1, const CvHomelandUnit& obj2)
{
	return obj1.GetAuxIntData() > obj2.GetAuxIntData();
}

bool HomelandAIHelpers::IsGoodUnitMix(CvPlot* pBasePlot, CvUnit* pUnit)
{
	if (!pBasePlot)
		return false;

	int iOffensive = 0;
	int iDefensive = 0;

	// Loop through all units on this plot
	IDInfo* pUnitNode = pBasePlot->headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(*pUnitNode);

		switch (pLoopUnit->getUnitInfo().GetDefaultUnitAIType())
		{
		case UNITAI_DEFENSE_AIR:
			iDefensive++;
			break;
		case UNITAI_ATTACK_AIR:
		case UNITAI_ICBM:
		case UNITAI_MISSILE_AIR:
			iOffensive++;
			break;
		}

		pUnitNode = pBasePlot->nextUnitNode(pUnitNode);
	}

	//don't count free slots etc ...
	switch (pUnit->getUnitInfo().GetDefaultUnitAIType())
	{
	case UNITAI_DEFENSE_AIR:
		return iDefensive<iOffensive+3;
		break;
	case UNITAI_ATTACK_AIR:
	case UNITAI_ICBM:
	case UNITAI_MISSILE_AIR:
		return iOffensive<iDefensive+3;
		break;
	}

	return true;
}

int HomelandAIHelpers::ScoreAirBase(CvPlot* pBasePlot, PlayerTypes ePlayer, int iRange)
{
	if (!pBasePlot || iRange<0)
		return false;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	//if a carrier is in a city - bad luck :)
	bool bIsCarrier = !(pBasePlot->isCity());

	//see if we're not at war yet but war may be coming
	const std::vector<PlayerTypes>& vFutureEnemies = kPlayer.GetPlayersAtWarWithInFuture();

	int iBaseScore = 0;

	if (bIsCarrier)
	{
		UnitHandle pDefender = pBasePlot->getBestDefender(ePlayer);
		if(pDefender->isProjectedToDieNextTurn() || kPlayer.GetTacticalAI()->IsUnitHealing(pDefender->GetID()))  
			return -1;

		iBaseScore += 10;

		if (pDefender->getArmyID()!=-1)
			iBaseScore += 30;
	}
	else
	{
		CvCity* pCity = pBasePlot->getPlotCity();
		if(pCity->isInDangerOfFalling() || (pCity->IsRazing() && pCity->getPopulation()<3) )
			return -1;
	}

	//check current targets
	const TacticalList& allTargets = kPlayer.GetTacticalAI()->GetTacticalTargets();
	for(unsigned int iI = 0; iI < allTargets.size(); iI++)
	{
		// Is this target near enough?
		if(plotDistance(pBasePlot->getX(), pBasePlot->getY(), allTargets[iI].GetTargetX(), allTargets[iI].GetTargetY()) > iRange)
			continue;

		// Is the target of an appropriate type?
		switch (allTargets[iI].GetTargetType())
		{
		case AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT:
			iBaseScore += 10;
			break;
		case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
			iBaseScore += 10;
			break;
		case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
			iBaseScore += 5;
			break;
		case AI_TACTICAL_TARGET_CITY:
			iBaseScore += 5;
			break;
		}
	}

	//check if there are potential future enemies around
	CvTacticalAnalysisCell* pTactCell = GC.getGame().GetTacticalAnalysisMap()->GetCell( pBasePlot->GetPlotIndex() );
	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZoneByID( pTactCell->GetDominanceZone() );
	if (!pZone)
	{
		//don't know what to do with this
		return iBaseScore;
	}

	const std::vector<int>& vNeighborZones = pZone->GetNeighboringZones();
	for (size_t i=0; i<vNeighborZones.size(); i++)
	{
		CvTacticalDominanceZone* pOtherZone = GC.getGame().GetTacticalAnalysisMap()->GetZoneByID( vNeighborZones[i] );
		if (pOtherZone && std::find(vFutureEnemies.begin(),vFutureEnemies.end(),pOtherZone->GetOwner())!=vFutureEnemies.end())
			iBaseScore += 1;
	}

	return iBaseScore;
}


//check all tactical zones to find the one we need to support most 
CvPlot* HomelandAIHelpers::GetPatrolTarget(CvPlot* pOriginPlot, PlayerTypes ePlayer, int iRange)
{
	if (!pOriginPlot || iRange<0)
		return false;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	const std::vector<PlayerTypes>& vFutureEnemies = kPlayer.GetPlayersAtWarWithInFuture();
	CvTacticalAnalysisMap* pTactMap = GC.getGame().GetTacticalAnalysisMap();

	CvPlot* pBestTarget = 0;
	int iBestScore = 0;
	int iScore = 0;
	for(int iI = 0; iI < pTactMap->GetNumZones(); iI++)
	{
		CvTacticalDominanceZone* pZone = pTactMap->GetZone(iI);
		if (!pZone || pZone->GetOwner()!=ePlayer)
			continue;

		CvCity* pZoneCity = pZone->GetZoneCity();
		if (!pZoneCity)
			continue;

		int iDistance = plotDistance(*pOriginPlot,*pZoneCity->plot());
		if (iDistance>iRange)
			continue;

		int iFriendlyPower = pZone->GetFriendlyStrength();
		int iEnemyPower = pZone->GetEnemyStrength();

		const std::vector<int>& vNeighborZones = pZone->GetNeighboringZones();
		for (size_t i=0; i<vNeighborZones.size(); i++)
		{
			CvTacticalDominanceZone* pOtherZone = pTactMap->GetZoneByID( vNeighborZones[i] );
			if (!pOtherZone)
				continue;
			
			if (std::find(vFutureEnemies.begin(),vFutureEnemies.end(),pOtherZone->GetOwner())!=vFutureEnemies.end())
				iEnemyPower += pOtherZone->GetNeutralStrength();

			iEnemyPower += pOtherZone->GetEnemyStrength();
			iFriendlyPower =+ pOtherZone->GetFriendlyStrength();
		}

		iScore = (iEnemyPower*1000)/max(1,iFriendlyPower*iDistance);
		if (iScore>iBestScore)
		{
			iBestScore = iScore;
			pBestTarget = pZoneCity->plot();
		}
	}

	return pBestTarget;
}