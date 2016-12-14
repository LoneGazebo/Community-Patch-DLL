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
		if(pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath())
		{
			continue;
		}

		//units we don't know how to handle here or which should have been processed in tactical AI
		if(pLoopUnit->getArmyID()!=-1 || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN || !pLoopUnit->canMove())
		{
			pLoopUnit->finishMoves();
			pLoopUnit->SetTurnProcessed(true);
			continue;
		}

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

#if defined(MOD_CORE_DEBUGGING)
	if (MOD_CORE_DEBUGGING)
	{
		for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(*it);
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

	//no homeland for barbarians
	if(m_pPlayer->GetID() == BARBARIAN_PLAYER)
		return;

	if (m_pPlayer->isHuman())
		FindAutomatedUnits();
	else
		RecruitUnits();

	// Make sure we have a unit to handle
	if(!m_CurrentTurnUnits.empty())
	{
		// Start by establishing the priority order for moves this turn
		EstablishHomelandPriorities();

		// Put together lists of places we may want to move toward
		FindHomelandTargets();

		// Loop through each move assigning units when available
		AssignHomelandMoves();
	}
}

CvPlot* CvHomelandAI::GetBestExploreTarget(const CvUnit* pUnit, int nMinCandidates, int iMaxTurns) const
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

	//see where our scout can go within the allowed turns
	ReachablePlots reachablePlots;
	if (pUnit)
	{
		SPathFinderUserData data(pUnit, CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE, iMaxTurns);
		data.ePathType = PT_UNIT_REACHABLE_PLOTS;
		reachablePlots = GC.GetPathFinder().GetPlotsInReach(iRefX, iRefY, data);
	}

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
		int iCityDistance = m_pPlayer->GetCityDistanceInEstimatedTurns(pEvalPlot);
		iRating = max(1, iRating-iCityDistance); 

		//reverse the score calculation below to get an upper bound on the distance
		//minus one because we want to do better
		int iMaxDistance = (1000*iRating) / max(1,iBestPlotScore) - 1;

		//is there a chance we can reach the plot within the required number of turns? (assuming no roads)
		if( sqrt((float)vPlotsByDistance[idx].first) > (iMaxDistance*pUnit->baseMoves()) )
			continue;

		ReachablePlots::iterator it = reachablePlots.find(pEvalPlot->GetPlotIndex());
		if (it==reachablePlots.end())
			continue;

		int iDistance = it->iTurns;
		int iPlotScore = (1000 * iRating) / max(1,iDistance);

		iValidCandidates++;

		if (iPlotScore>iBestPlotScore)
		{
			pBestPlot = pEvalPlot;
			iBestPlotScore = iPlotScore;
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
#if defined(MOD_BALANCE_CORE)
	int iFlavorNaval = 0;
	int iFlavorNavalMelee = 0;
#endif

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
#if defined(MOD_BALANCE_CORE)
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_NAVAL")
		{
			iFlavorNaval = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_NAVAL_RECON")
		{
			iFlavorNavalMelee = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
		}
#endif
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
			if(m_pPlayer->GetPlayerPolicies()->HasPolicyEncouragingGarrisons() || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
			{
				iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_GARRISON() * 2;
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
#if defined(MOD_BALANCE_CORE)
		case AI_HOMELAND_MOVE_SENTRY_NAVAL:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SENTRY();
			break;
#endif
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
#if defined(MOD_AI_SECONDARY_SETTLERS)
		case AI_HOMELAND_MOVE_SECONDARY_SETTLER:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SECONDARY_SETTLER();
			break;
#endif
#if defined(MOD_AI_SECONDARY_WORKERS)
		case AI_HOMELAND_MOVE_SECONDARY_WORKER:
			iPriority = GC.getAI_HOMELAND_MOVE_PRIORITY_SECONDARY_WORKER();
			break;
#endif
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
					iPriority /= min (4, max(1, iFlavorDefense));
				}
				break;
			case AI_HOMELAND_MOVE_SENTRY_NAVAL:
				if(m_pPlayer->IsAtWarAnyMajor())
				{
					iPriority /= min (4, max(1, iFlavorNaval));
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
				iPriority += iFlavorExplore;
			case AI_HOMELAND_MOVE_EXPLORE_SEA:
				iPriority += iFlavorExplore;
				iPriority += iFlavorNavalMelee;
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
#if defined(MOD_BALANCE_CORE)
	m_TargetedNavalSentryPoints.clear();
	m_TargetedHomeUnassignedPlots.clear();

	CvTacticalAnalysisMap* pTactMap = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap();
	CvTacticalDominanceZone *pZone;
#endif

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
				ResourceTypes eNonObsoleteResource = pLoopPlot->getResourceType(eTeam);
				if(eNonObsoleteResource != NO_RESOURCE)
				{
					if(pLoopPlot->getOwner() == m_pPlayer->GetID())
					{
						// Find proper improvement
						BuildTypes eBuild;
						for(int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							eBuild = ((BuildTypes)iJ);
							if(eBuild == NO_BUILD)
								continue;

							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if(!pkBuildInfo)
								continue;

							ImprovementTypes eNavalImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
							if(eNavalImprovement == NO_IMPROVEMENT)
								continue;

							if(GC.getImprovementInfo(eNavalImprovement)->IsImprovementResourceTrade(pLoopPlot->getResourceType()))
							{
								newTarget.SetTargetType(AI_HOMELAND_TARGET_NAVAL_RESOURCE);
								newTarget.SetTargetX(pLoopPlot->getX());
								newTarget.SetTargetY(pLoopPlot->getY());
								newTarget.SetAuxData(pLoopPlot);
								newTarget.SetAuxIntData(eBuild);
								m_TargetedNavalResources.push_back(newTarget);								
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
			else if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()))
			{
				if( pLoopPlot->getOwner() == NO_PLAYER ||
					pLoopPlot->getOwner() == m_pPlayer->GetID() || 
					!m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoDiggingPromise(pLoopPlot->getOwner()) )
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxData(pLoopPlot);
					m_TargetedAntiquitySites.push_back(newTarget);
				}
			}
			// ... possible sentry point?
			else if(pLoopPlot->getOwner() == m_pPlayer->GetID() && !pLoopPlot->isWater() && 
				pLoopPlot->isValidMovePlot(m_pPlayer->GetID()) && m_pPlayer->GetCityDistanceInEstimatedTurns(pLoopPlot)>1)
			{
				ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
				ImprovementTypes eCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");
				
				if( (eFort != NO_IMPROVEMENT && pLoopPlot->getImprovementType() == eFort) ||
					(eCitadel != NO_IMPROVEMENT && pLoopPlot->getImprovementType() == eCitadel) )
				{
					int iWeight = 100000 + pLoopPlot->defenseModifier(m_pPlayer->getTeam(),false,false);

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
#if defined(MOD_BALANCE_CORE)
			// ... possible naval sentry point?
			if (pLoopPlot->isWater() &&
				pLoopPlot->isValidMovePlot(m_pPlayer->GetID()))
			{
				CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
				if (pWorkingCity != NULL && pWorkingCity->getOwner() == m_pPlayer->GetID() && pWorkingCity->isCoastal())
				{
					if (!m_pPlayer->IsCityAlreadyTargeted(pWorkingCity, DOMAIN_SEA, 50))
					{
						int iDistance = m_pPlayer->GetCityDistanceInEstimatedTurns(pLoopPlot);

						if (iDistance > 3)
							continue;

						int iWeight = 0;
						if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
						{
							iWeight += 50;
						}

						if (iDistance == 3)
						{
							iWeight += 50;
						}
						else if (iDistance == 2)
						{
							iWeight += 100;
						}
						else if (iDistance == 1)
						{
							iWeight += 25;
						}
						if (pLoopPlot->getNumUnits() > 0)
						{
							CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
							if (pUnit != NULL)
							{
								if (pUnit->IsCivilianUnit())
								{
									iWeight += 25;
								}
								else if (pUnit->isEmbarked() && pUnit->getDomainType() != DOMAIN_SEA)
								{
									iWeight += 50;
								}
							}
						}


						if (m_pPlayer->getNumCities() > 1 && pWorkingCity->GetThreatRank() != -1)
						{
							//More cities = more threat.
							int iThreat = (m_pPlayer->getNumCities() - pWorkingCity->GetThreatRank()) * 10;
							if (iThreat > 0)
							{
								iWeight += iThreat;
							}
						}
						if (m_pPlayer->IsAtWar())
						{
							if (pWorkingCity->isInDangerOfFalling() || pWorkingCity->isUnderSiege() || pWorkingCity->IsBlockaded(true))
							{
								iWeight *= 10;
							}
							if (pWorkingCity->IsBastion())
							{
								iWeight *= 5;
							}
							if (pWorkingCity->getDamage() > 0)
							{
								iWeight *= 2;
							}
						}
						if (iWeight > 0)
						{
							newTarget.SetTargetType(AI_HOMELAND_TARGET_SENTRY_POINT_NAVAL);
							newTarget.SetTargetX(pLoopPlot->getX());
							newTarget.SetTargetY(pLoopPlot->getY());
							newTarget.SetAuxData(pLoopPlot);
							newTarget.SetAuxIntData(iWeight);
							m_TargetedNavalSentryPoints.push_back(newTarget);
						}
					}
				}
			}
#endif
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
			// ... road segment in friendly territory?
			else if(pLoopPlot->isRoute() && pLoopPlot->getOwner() == m_pPlayer->GetID())
			{
				//Let's weight them based on defense and danger - this should make us muster in more tactically - responsible places
				int iWeight = pLoopPlot->defenseModifier(eTeam, false, false);
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
#if defined(MOD_BALANCE_CORE)
			//Let's make a list of all other plots.
			if (pLoopPlot->getOwner() == m_pPlayer->GetID() && !pLoopPlot->isWater() && !pLoopPlot->isImpassable() && pLoopPlot->getWorkingCity() != NULL)
			{
				pZone = pTactMap->GetZoneByCity(pLoopPlot->getWorkingCity(), false);
				if (pZone)
				{
					int iValue = (10000 - pZone->GetFriendlyMeleeUnitCount() - pZone->GetFriendlyRangedUnitCount());
					if (iValue > 0)
					{
						newTarget.SetTargetType(AI_HOMELAND_TARGET_UNASSIGNED);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxData(pLoopPlot);
						newTarget.SetAuxIntData(iValue);
						m_TargetedHomeUnassignedPlots.push_back(newTarget);
					}
				}
			}
#endif
		}
	}

	// Post-processing on targets
	EliminateAdjacentSentryPoints();
#if defined(MOD_BALANCE_CORE)
	EliminateAdjacentNavalSentryPoints();
	EliminateAdjacentUnassignedPoints();
#endif
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
#if defined(MOD_BALANCE_CORE)
		case AI_HOMELAND_MOVE_SENTRY_NAVAL:
			PlotSentryNavalMoves();
			break;
#endif
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
			//this is for embassies - diplomatic missions are handled via AI operation
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
#if defined(MOD_AI_SECONDARY_SETTLERS)
		case AI_HOMELAND_MOVE_SECONDARY_SETTLER:
			PlotOpportunisticSettlementMoves();
			break;
#endif
#if defined(MOD_AI_SECONDARY_SETTLERS)
		case AI_HOMELAND_MOVE_SECONDARY_WORKER:
			PlotWorkerMoves(true);
			PlotWorkerSeaMoves(true);
			break;
#endif
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && !pUnit->isHuman())
		{
			if(m_pPlayer->getNumCities() == 0)
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
						ExecuteMoveToTarget(pGarrison, pTarget, 0);
#if defined(MOD_BALANCE_CORE)
						TacticalAIHelpers::PerformRangedAttackWithoutMoving(pGarrison);
						pGarrison->finishMoves();
						UnitProcessed(pGarrison->GetID());
#endif
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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

			int iDangerLevel = m_pPlayer->GetPlotDanger(*pPlot,pUnit);
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
				else if(pUnit->getDamage()>0)
				{
#if defined(MOD_BALANCE_CORE)
					int iTurnDamage = pUnit->plot()->getTurnDamage(pUnit->ignoreTerrainDamage(), pUnit->ignoreFeatureDamage(), pUnit->extraTerrainDamage(), pUnit->extraFeatureDamage());
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
					else if(iTurnDamage > 0 && (pUnit->getDamage() > pUnit->GetCurrHitPoints()))
					{
						bAddUnit = true;
					}
#endif
					// Everyone else flees at more than 70% damage
					else if(MOD_AI_SMART_FLEE_FROM_DANGER && (((pUnit->getDamage()*100)/pUnit->GetMaxHitPoints())>70))
					{
						bAddUnit = true;
					}
					// Everyone else flees at less than 50% combat strength (works for zero CS also)
					else if(pUnit->GetBaseCombatStrengthConsideringDamage() * 2 < pUnit->GetBaseCombatStrength())
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
				else if (!pUnit->isBarbarian() && pUnit->getArmyID() == -1 && !pUnit->IsRecentlyDeployedFromOperation())
				{
					if(iDangerLevel > pUnit->GetCurrHitPoints()*1.5)
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
				CvUnit *pReserve = GetBestUnitToReachTarget(pTarget, 15);
				if(pReserve)
				{
					ExecuteMoveToTarget(pReserve, pTarget, 0);
#if defined(MOD_BALANCE_CORE)
					TacticalAIHelpers::PerformRangedAttackWithoutMoving(pReserve);
					pReserve->finishMoves();
					UnitProcessed(pReserve->GetID());
#endif

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
	if (m_pPlayer->IsAtWar())
		return;

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
				CvUnit *pSentry = GetBestUnitToReachTarget(pTarget, 6);
				if(pSentry)
				{
					ExecuteMoveToTarget(pSentry, pTarget, 0);
#if defined(MOD_BALANCE_CORE)
					TacticalAIHelpers::PerformRangedAttackWithoutMoving(pSentry);
					pSentry->finishMoves();
					UnitProcessed(pSentry->GetID());
#endif
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
#if defined(MOD_BALANCE_CORE)
void CvHomelandAI::PlotSentryNavalMoves()
{
	// Do we have any targets of this type?
	if(!m_TargetedNavalSentryPoints.empty())
	{
		// See how many moves of this type we can execute
		for(unsigned int iI = 0; iI < m_TargetedNavalSentryPoints.size(); iI++)
		{
			AI_PERF_FORMAT("Homeland-perf.csv", ("PlotNavalSentryMoves, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

			CvPlot* pTarget = GC.getMap().plot(m_TargetedNavalSentryPoints[iI].GetTargetX(), m_TargetedNavalSentryPoints[iI].GetTargetY());

			FindUnitsForThisMove(AI_HOMELAND_MOVE_SENTRY_NAVAL, (iI == 0)/*bFirstTime*/);

			if(m_CurrentMoveHighPriorityUnits.size() + m_CurrentMoveUnits.size() > 0)
			{
				CvUnit *pSentry = GetBestUnitToReachTarget(pTarget, 15);
				if(pSentry)
				{
					if(pSentry->plot() == pTarget)
					{
						//Remove all status if not fortified so we can see if it is possible to fortify.
						if(pSentry->canSentry(pSentry->plot()))
						{
							pSentry->PushMission(CvTypes::getMISSION_ALERT());
						}
						else
						{
							pSentry->PushMission(CvTypes::getMISSION_SKIP());
						}
#if defined(MOD_BALANCE_CORE)
						TacticalAIHelpers::PerformRangedAttackWithoutMoving(pSentry);
						pSentry->finishMoves();
						UnitProcessed(pSentry->GetID());
#endif
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s %d AT NAVAL sentry point, chillin, X: %d, Y: %d, Priority: %d", pSentry->getName().c_str(), pSentry->GetID(), m_TargetedNavalSentryPoints[iI].GetTargetX(), m_TargetedNavalSentryPoints[iI].GetTargetY(), m_TargetedNavalSentryPoints[iI].GetAuxIntData());
							LogHomelandMessage(strLogString);
						}
					}
					else
					{
						ExecuteMoveToTarget(pSentry, pTarget, 0);
#if defined(MOD_BALANCE_CORE)
						TacticalAIHelpers::PerformRangedAttackWithoutMoving(pSentry);
						pSentry->finishMoves();
						UnitProcessed(pSentry->GetID());
#endif
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving %s %d to NAVAL sentry point, X: %d, Y: %d, Priority: %d", pSentry->getName().c_str(), pSentry->GetID(), m_TargetedNavalSentryPoints[iI].GetTargetX(), m_TargetedNavalSentryPoints[iI].GetTargetY(), m_TargetedNavalSentryPoints[iI].GetAuxIntData());
							LogHomelandMessage(strLogString);
						}
					}
				}
			}
		}
	}
}
#endif

#if defined(MOD_AI_SECONDARY_SETTLERS)
void CvHomelandAI::PlotOpportunisticSettlementMoves()
{
	if(!MOD_AI_SECONDARY_SETTLERS)
		return;

	ClearCurrentMoveUnits();
	const char* szCiv = m_pPlayer->getCivilizationTypeKey();

	int iMinHappiness = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MIN_HAPPINESS", 5);
	int iMinTurnsSinceLastCity = gCustomMods.getCivOption(szCiv, "SECONDARY_SETTLERS_MIN_TURNS_SINCE_LAST_CITY", 10);

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

	if (m_pPlayer->GetHappiness() <= iMinHappiness) {
		return;
	}
	
	if (m_pPlayer->GetTurnsSinceSettledLastCity() <= iMinTurnsSinceLastCity) {
		return;
	}
	
	// Make a list of all combat units that can do this.
	MoveUnitsArray PossibleSettlerUnits;
	int iTurnDistance = INT_MAX;

	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it) 
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->IsCombatUnit() && (pUnit->isFound() || pUnit->IsFoundAbroad() || pUnit->IsFoundLate() || pUnit->IsFoundMid()))
		{
			//fake this, the distance check is irrelevant here
			ReachablePlots turnsFromMuster;
			turnsFromMuster.insert( SMovePlot(pUnit->plot()->GetPlotIndex()) );

			if(OperationalAIHelpers::IsUnitSuitableForRecruitment(pUnit,pUnit->plot(),turnsFromMuster,pUnit->plot(),false,false,iTurnDistance))
			{
				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				unit.SetAuxIntData(plotDistance(pUnit->getX(), pUnit->getY(), iCapitalX, iCapitalY));
				PossibleSettlerUnits.push_back(unit);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp;
					strLogString.Format("%s (%d): found secondary settler, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
	}
	
	// Sort them by proximity to the capital
	std::stable_sort(PossibleSettlerUnits.begin(), PossibleSettlerUnits.end(), HomelandAIHelpers::CvHomelandUnitAuxIntSort);
	
	if (PossibleSettlerUnits.size() > 0) 
	{
		MoveUnitsArray::iterator settlerUnitIt;
		for (settlerUnitIt = PossibleSettlerUnits.begin(); settlerUnitIt != PossibleSettlerUnits.end(); ++settlerUnitIt) 
		{
			CvUnit* pUnit = m_pPlayer->getUnit(settlerUnitIt->GetID());
			if(pUnit)
			{
				pUnit->AI_setUnitAIType(UNITAI_SETTLE);
				break;
			}
		}
	}
	PossibleSettlerUnits.clear();
}
#endif

/// Find something for all workers to do
#if defined(MOD_AI_SECONDARY_WORKERS)
void CvHomelandAI::PlotWorkerMoves(bool bSecondary)
#else
void CvHomelandAI::PlotWorkerMoves()
#endif
{
	if(bSecondary && !MOD_AI_SECONDARY_WORKERS)
		return;

	ClearCurrentMoveUnits();

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
#if defined(MOD_BALANCE_CORE)
void CvHomelandAI::PlotWorkerSeaMoves(bool bSecondary)
#else
void CvHomelandAI::PlotWorkerSeaMoves()
#endif
{
	ClearCurrentMoveUnits();
	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
			else if(MOD_AI_SECONDARY_WORKERS && bSecondary && !m_pPlayer->IsAtWar() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->getUnitInfo().GetCombat() > 0)
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
		CvUnit* pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
		int iTargetIndex = -1;
		int iTargetMoves = MAX_INT;

		// See how many moves of this type we can execute
		for (unsigned int iI = 0; iI < m_TargetedNavalResources.size(); iI++)
		{
			// See what units we have who can reach target this turn
			CvPlot* pTarget = GC.getMap().plot(m_TargetedNavalResources[iI].GetTargetX(), m_TargetedNavalResources[iI].GetTargetY());

			if (!pTarget->getArea()==pUnit->getArea())
				continue;

#if defined(MOD_AI_SECONDARY_WORKERS)
			if(MOD_AI_SECONDARY_WORKERS && bSecondary)
			{
				if(pUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA  ||
			   pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_BUILD)
				{
					if (!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
						continue;

					if (m_pPlayer->GetPlotDanger(*pTarget,pUnit)>0)
						continue;
				}
				else if(MOD_AI_SECONDARY_WORKERS && !pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
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
			else
			{
#endif
				if (!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
					continue;

				if (m_pPlayer->GetPlotDanger(*pTarget,pUnit)>0)
					continue;
#if defined(MOD_AI_SECONDARY_WORKERS)
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
			if (pUnit->at(iTargetX, iTargetY))
			{
				pUnit->PushMission(CvTypes::getMISSION_BUILD(), m_TargetedNavalResources[iTargetIndex].GetAuxIntData(), -1, 0, false, false, MISSIONAI_BUILD, pTarget);
				bResult = true;
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), iTargetX, iTargetY);
				if(pUnit->plot() == pTarget && pUnit->canMove())
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), m_TargetedNavalResources[iTargetIndex].GetAuxIntData(), -1, 0, false, false, MISSIONAI_BUILD, pTarget);
					bResult = true;
				}
				else
				{
					pUnit->finishMoves();
				}

				// Delete this unit from those we have to move
				UnitProcessed(pUnit->GetID());
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
	ClearCurrentMoveUnits();

	// Loop through all remaining units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && pUnit->IsCombatUnit() && pUnit->getDomainType() != DOMAIN_AIR)
		{
			if(pUnit->plot()->getOwner() != m_pPlayer->GetID())
				continue;

			CvHomelandUnit unit;
			unit.SetID(pUnit->GetID());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
#if defined(MOD_BALANCE_CORE)
		if(m_pPlayer->IsAtWarAnyMajor())
		{
			ExecuteAggressivePatrolMoves();
		}
		else
		{
#endif
		ExecutePatrolMoves();
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}
}
#if defined(MOD_BALANCE_CORE)
void CvHomelandAI::ExecuteAggressivePatrolMoves()
{
//check what kind of units we have
	int iUnitsSea = 0, iUnitsLand = 0;
	for(MoveUnitsArray::iterator itUnit = m_CurrentMoveUnits.begin(); itUnit != m_CurrentMoveUnits.end(); ++itUnit)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(itUnit->GetID());
		if (pUnit && pUnit->IsCombatUnit())
		{
			if (pUnit->getDomainType()==DOMAIN_SEA)
				iUnitsSea++;
			if (pUnit->getDomainType()==DOMAIN_LAND)
				iUnitsLand++;
		}
	}
	
	//get the most exposed cities and their surrounding plots
	std::vector<CvPlot*> vLandTargets, vWaterTargets;
	if (iUnitsLand>0)
		vLandTargets = HomelandAIHelpers::GetAggressivePatrolTargets(m_pPlayer->GetID(), false, iUnitsLand);
	if (iUnitsSea>0)
		vWaterTargets = HomelandAIHelpers::GetAggressivePatrolTargets(m_pPlayer->GetID(), true, iUnitsSea);

	SPathFinderUserData data(m_pPlayer->GetID(),PT_GENERIC_REACHABLE_PLOTS,-1,23);
	std::map<CvPlot*,ReachablePlots> mapReachablePlots;
	for (size_t i=0; i<vLandTargets.size(); i++)
		mapReachablePlots[vLandTargets[i]] = GC.GetStepFinder().GetPlotsInReach(vLandTargets[i],data);
	for (size_t i=0; i<vWaterTargets.size(); i++)
		//the stepfinder works for both land and water, so do the work only if necessary
		if (mapReachablePlots.find(vWaterTargets[i])==mapReachablePlots.end())
				mapReachablePlots[vWaterTargets[i]] = GC.GetStepFinder().GetPlotsInReach(vWaterTargets[i],data);

	//for each unit, check which city is closest
	for(MoveUnitsArray::iterator itUnit = m_CurrentMoveUnits.begin(); itUnit != m_CurrentMoveUnits.end(); ++itUnit)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(itUnit->GetID());
		if(!pUnit || pUnit->IsCivilianUnit() || pUnit->getDomainType()==DOMAIN_AIR)
			continue;

		//the target we're looking at depends on the domain of the unit
		std::vector<CvPlot*>& vTargets = (pUnit->getDomainType()==DOMAIN_SEA) ? vWaterTargets : vLandTargets;

		int iBestTurns = INT_MAX;
		CvPlot* pBestTarget = NULL;
		for (size_t i=0; i<vTargets.size(); i++)
		{
			ReachablePlots::const_iterator itPlot = mapReachablePlots[vTargets[i]].find(pUnit->plot()->GetPlotIndex());
			if (itPlot!=mapReachablePlots[vTargets[i]].end() && itPlot->iTurns<iBestTurns)
			{
				//try not to create a unit carpet without any space to move
				for(int iJ = 0; iJ < RING5_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = iterateRingPlots(vTargets[i], iJ);

					if(pLoopPlot == NULL)
						continue;

					if (pLoopPlot->getDomain() != pUnit->getDomainType())
						continue;

					//Don't patrol into cities.
					if(pLoopPlot->isCity())
						continue;

					//Lots of adjacent units? Ignore.
					if(pLoopPlot->GetNumFriendlyUnitsAdjacent(m_pPlayer->getTeam(), pUnit->getDomainType(), pUnit) > 3)
					{
						continue;
					}

					if (pUnit->canMoveInto(*vTargets[i]) && pLoopPlot->getDomain()==pUnit->getDomainType())
					{
						iBestTurns = itPlot->iTurns;
						pBestTarget = GC.getMap().plotByIndexUnchecked(itPlot->iPlotIndex);
					}
				}
			}
		}

		if(pBestTarget)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;

				strTemp = pUnit->getUnitInfo().GetDescription();
				strLogString.Format("%s (%d) aggressively patrolling to, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestTarget->getX(), pBestTarget->getY(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}

			if (pBestTarget != pUnit->plot())
			{
				//use the exact target location - GetPatrolTarget makes sure there is a free spot
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());
			}
			else
			{
				if (pUnit->canFortify(pUnit->plot()))
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
			}
		}
	}
}
#endif
/// When nothing better to do, distribute units across cities
void CvHomelandAI::ExecutePatrolMoves()
{
	//check what kind of units we have
	int iUnitsSea = 0, iUnitsLand = 0;
	for(MoveUnitsArray::iterator itUnit = m_CurrentMoveUnits.begin(); itUnit != m_CurrentMoveUnits.end(); ++itUnit)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(itUnit->GetID());
		if (pUnit && pUnit->IsCombatUnit())
		{
			if (pUnit->getDomainType()==DOMAIN_SEA)
				iUnitsSea++;
			if (pUnit->getDomainType()==DOMAIN_LAND)
				iUnitsLand++;
		}
	}
	
	//get the most exposed cities and their surrounding plots
	std::vector<CvPlot*> vLandTargets, vWaterTargets;
	if (iUnitsLand>0)
		vLandTargets = HomelandAIHelpers::GetPatrolTargets(m_pPlayer->GetID(), false);
	if (iUnitsSea>0)
		vWaterTargets = HomelandAIHelpers::GetPatrolTargets(m_pPlayer->GetID(), true);

	SPathFinderUserData data(m_pPlayer->GetID(),PT_GENERIC_REACHABLE_PLOTS,-1,23);
	std::map<CvPlot*,ReachablePlots> mapReachablePlots;
	for (size_t i=0; i<vLandTargets.size(); i++)
		mapReachablePlots[vLandTargets[i]] = GC.GetStepFinder().GetPlotsInReach(vLandTargets[i],data);
	for (size_t i=0; i<vWaterTargets.size(); i++)
		//the stepfinder works for both land and water, so do the work only if necessary
		if (mapReachablePlots.find(vWaterTargets[i])==mapReachablePlots.end())
				mapReachablePlots[vWaterTargets[i]] = GC.GetStepFinder().GetPlotsInReach(vWaterTargets[i],data);

	//for each unit, check which city is closest
	for(MoveUnitsArray::iterator itUnit = m_CurrentMoveUnits.begin(); itUnit != m_CurrentMoveUnits.end(); ++itUnit)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(itUnit->GetID());
		if(!pUnit || pUnit->IsCivilianUnit() || pUnit->getDomainType()==DOMAIN_AIR)
			continue;

		//the target we're looking at depends on the domain of the unit
		std::vector<CvPlot*>& vTargets = (pUnit->getDomainType()==DOMAIN_SEA) ? vWaterTargets : vLandTargets;

		CvPlot* pBestTarget = NULL;
		for (size_t i=0; i<vTargets.size(); i++)
		{
			ReachablePlots::const_iterator itPlot = mapReachablePlots[vTargets[i]].find(pUnit->plot()->GetPlotIndex());
			if (itPlot!=mapReachablePlots[vTargets[i]].end())
			{
				//try not to create a unit carpet without any space to move
				for(int iJ = 0; iJ < RING4_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = iterateRingPlots(vTargets[i], iJ);

					if(pLoopPlot == NULL)
						continue;

					if (pLoopPlot->getDomain() != pUnit->getDomainType())
						continue;

					//Don't patrol into cities.
					if(pLoopPlot->isCity())
						continue;

					//Lots of adjacent units? Ignore.
					if(pLoopPlot->GetNumFriendlyUnitsAdjacent(m_pPlayer->getTeam(), pUnit->getDomainType(), pUnit) > 3)
					{
						continue;
					}

					if (pUnit->canMoveInto(*vTargets[i]) && pLoopPlot->getDomain()==pUnit->getDomainType())
					{
						pBestTarget = GC.getMap().plotByIndexUnchecked(itPlot->iPlotIndex);
						break;
					}
				}
			}
		}

		if(pBestTarget)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;

				strTemp = pUnit->getUnitInfo().GetDescription();
				strLogString.Format("%s (%d) patrolling to, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestTarget->getX(), pBestTarget->getY(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}

			if (pBestTarget != pUnit->plot())
			{
				//use the exact target location - GetPatrolTarget makes sure there is a free spot
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestTarget->getX(), pBestTarget->getY());
				pUnit->finishMoves();
				UnitProcessed(pUnit->GetID());
			}
			else
			{
				if (pUnit->canFortify(pUnit->plot()))
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
			}
		}
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);

		if(pUnit && !pUnit->isHuman() && pUnit->getArmyID() == -1)
		{
#if defined(MOD_BALANCE_CORE)
			//Let's only worry about units in our land.
			if(pUnit->plot()->getOwner() != m_pPlayer->GetID())
				continue;

			//And on land.
			if(pUnit->isEmbarked() && pUnit->getDomainType() != DOMAIN_SEA)
				continue;
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
			CvUnit* pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
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
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[0].GetID());
#if defined(MOD_BALANCE_CORE)
		if(!pUnit)
			return;
#endif

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
#if !defined(MOD_BALANCE_CORE)
		if(pUnit->plot()->getOwner() == pUnit->getOwner())
#endif
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
#if !defined(MOD_BALANCE_CORE)
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
				if(MoveToEmptySpaceNearTarget(pUnit, pUpgradeCity->plot(), DOMAIN_LAND, 23))
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
#endif
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
					ExecuteMoveToTarget(pIndy, pTarget, CvUnit::MOVEFLAG_IGNORE_DANGER);
#if defined(MOD_BALANCE_CORE)
					TacticalAIHelpers::PerformRangedAttackWithoutMoving(pIndy);
					pIndy->finishMoves();
					UnitProcessed(pIndy->GetID());
#endif
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
#if defined(MOD_BALANCE_CORE)
			if(pUnit->AI_getUnitAIType() == UNITAI_ENGINEER || (pUnit->getUnitInfo().GetUnitAIType(UNITAI_ENGINEER) && !m_pPlayer->IsAtWar()))
#else
			if(pUnit->AI_getUnitAIType() == UNITAI_ENGINEER)
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->getDomainType() == DOMAIN_AIR)
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
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
	vector<CvCity *> aNeedAirliftCities;
	CvCity *pLoopCity;
	int iCityLoop;
	for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		//Let's see if we have airlifts.
		if (pLoopCity->CanAirlift())
		{
			//Let's see if there are cities that need an airlift.
			if (pLoopCity->isUnderSiege() || pLoopCity->isInDangerOfFalling())
			{
				aNeedAirliftCities.push_back(pLoopCity);
			}
			aAirliftCities.push_back(pLoopCity);
		}
	}
	if (aAirliftCities.size() < 2 || aNeedAirliftCities.size() <= 0)
	{
		return;
	}

	// Create list of empty land plots we own adjacent to airlift cities that are not endangered
	vector<CvPlot *> aAirliftPlots;
	CvTacticalAnalysisMap* pTactMap = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap();

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
					CvUnit* pBestDefender = pLoopPlot->getBestDefender(m_pPlayer->GetID());
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

			CvUnit *pUnit = GetBestUnitToReachTarget(pTarget, 12);
			if(pUnit)
			{
				ExecuteMoveToTarget(pUnit,pTarget,0, true);

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

/// Log that we couldn't find assignments for some units - this is the catchall, last-resort handler that makes sure we can end the turn
void CvHomelandAI::ReviewUnassignedUnits()
{
	ClearCurrentMoveUnits();
	// Loop through all remaining units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			//safety check. in case we have a unit with queued missions, 
			//don't finish its moves, else it won't be able to execute the mission
			if (pUnit->GetLengthMissionQueue()>0)
			{
				pUnit->SetTurnProcessed(true);
				continue;
			}

			pUnit->setHomelandMove(AI_HOMELAND_MOVE_UNASSIGNED);
			if (pUnit->getDomainType() == DOMAIN_LAND)
			{
				if (pUnit->getMoves() > 0)
				{
					CvHomelandUnit unit;
					unit.SetID(pUnit->GetID());
					m_CurrentMoveUnits.push_back(unit);
				}
				else
				{
					pUnit->SetTurnProcessed(true);
					pUnit->finishMoves();
				}
			}
			else if(pUnit->getDomainType() == DOMAIN_SEA)
			{
				if(pUnit->plot()->getOwner() == pUnit->getOwner())
				{
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
						CvPlot* pPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTestCity->plot());
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

					bool bStuck = true;
					if(pBestPlot != NULL)
					{
						if(MoveToEmptySpaceNearTarget(pUnit, pBestPlot, DOMAIN_SEA, 42))
						{
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
							bStuck = false;
							continue;
						}
					}
					//Stuck and not at home? Scrap it.
					if(bStuck)
					{
						CvString strTemp;
						CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
						if(pkUnitInfo)
						{
							strTemp = pkUnitInfo->GetDescription();
							CvString strLogString;
							strLogString.Format("Unassigned %s %d SCRAPPED, at X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
						pUnit->scrap();
						continue;
					}
				}
			}
		}
	}
	if (m_CurrentMoveUnits.size() > 0)
	{
		ExecuteUnassignedUnitMoves();
	}
}
#if defined(MOD_BALANCE_CORE)
void CvHomelandAI::ExecuteUnassignedUnitMoves()
{
	MoveUnitsArray::iterator it;
	for (it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
		{
			continue;
		}

		CvPlot* pTarget = FindUnassignedTarget(pUnit);
		if (pTarget)
		{
			if (MoveToEmptySpaceNearTarget(pUnit, pTarget, DOMAIN_LAND, 10))
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("UnitID: %d moved to unassigned plot, X: %d, Y: %d, from X: %d Y: %d", pUnit->GetID(), pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
		pUnit->finishMoves();
		UnitProcessed(pUnit->GetID());
	}
}
#endif
/// Creates cities for AI civs on first turn
void CvHomelandAI::ExecuteFirstTurnSettlerMoves()
{
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
#if defined(MOD_BALANCE_CORE_SETTLER_MOVE)
			if( MOD_BALANCE_CORE_SETTLER_MOVE && GC.getSETTLER_MOVE_ON_START() > 0 && m_pPlayer->isMajorCiv())
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
						int iRandomDirection = GC.getGame().getSmallFakeRandNum(NUM_DIRECTION_TYPES, iI);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit || !pUnit->canMove())
		{
			continue;
		}

		//performance: if we have a leftover path and the target is a good, then reuse it!
		const CvPathNodeArray& lastPath = pUnit->GetPathNodeArray();
		if (!lastPath.empty())
		{
			CvPlot* pDestPlot = lastPath.GetFinalPlot();
			const std::vector<SPlotWithScore>& vExplorePlots = m_pPlayer->GetEconomicAI()->GetExplorationPlots(pUnit->getDomainType());

			SPlotWithScore dummy(pDestPlot,0);
			if ( std::find( vExplorePlots.begin(),vExplorePlots.end(),dummy ) != vExplorePlots.end() )
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pDestPlot->getX(), pDestPlot->getY(), CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE, false, false, MISSIONAI_EXPLORE, pDestPlot);
				if(!pUnit->canMove())
					UnitProcessed(pUnit->GetID());
				continue;
			}
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
		ReachablePlots eligiblePlots;
		TacticalAIHelpers::GetAllPlotsInReachThisTurn(pUnit, pUnit->plot(), eligiblePlots, true, true, pUnit->isEmbarked());
		for (ReachablePlots::iterator tile=eligiblePlots.begin(); tile!=eligiblePlots.end(); ++tile)
		{
			CvPlot* pEvalPlot = GC.getMap().plotByIndexUnchecked(tile->iPlotIndex);

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
			if (pEnemyUnit && TacticalAIHelpers::KillUnitIfPossible(pUnit,pEnemyUnit))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("%s killed a weak enemy, at X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}

				//we just moved, eligiblePlots are no longer valid
				break;
			}

			//do this after the enemy unit check
			if (!IsValidExplorerEndTurnPlot(pUnit, pEvalPlot))
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

				int iRandom = GC.getGame().getSmallFakeRandNum(47,*pEvalPlot);
				int iTotalScore = iScoreBase+iScoreExtra+iScoreBonus+iRandom;

				//careful with plots that are too dangerous
				int iAcceptableDanger = pUnit->GetCurrHitPoints()/2;
				int iDanger = m_pPlayer->GetPlotDanger(*pEvalPlot,pUnit);
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
		if(!pUnit->canMove() || pUnit->isOutOfAttacks())
		{
			UnitProcessed(pUnit->GetID());
			continue;
		}

		//if we didn't find a worthwhile plot among our adjacent plots, check the global targets
		if (!pBestPlot && pUnit->movesLeft() > 0)
		{
			//check at least 5 candidates
			pBestPlot = GetBestExploreTarget(pUnit, 5, 8);

			//if nothing found, retry with larger distance - but this is slow
			if (!pBestPlot)
				pBestPlot = GetBestExploreTarget(pUnit, 5, 23);

			//if still nothing found, retry with even larger distance - but this is sloooooow
			if (!pBestPlot)
				pBestPlot = GetBestExploreTarget(pUnit, 5, 42);

			//verify that we don't move into danger ...
			if (pBestPlot)
			{
				if (pUnit->GeneratePath(pBestPlot, CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE))
				{
					CvPlot* pEndTurnPlot = pUnit->GetPathEndFirstTurnPlot();
					if (pUnit->GetDanger(pEndTurnPlot) > pUnit->GetCurrHitPoints() / 2)
					{
						//move to safety instead
						pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
					}
				}
				else
					pBestPlot = 0;
			}
		}

		if(pBestPlot && pBestPlot != pUnit->plot())
		{
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE, false, false, MISSIONAI_EXPLORE, pBestPlot);

			// Only mark as done if out of movement - we'll do a second pass later
			if (!pUnit->canMove())
				UnitProcessed(pUnit->GetID());

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp = pUnit->getUnitInfo().GetDescription();
				if (bFoundNearbyExplorePlot)
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
				//don't call UnitProcessed, it seems it can cause a hang
				m_CurrentTurnUnits.remove(pUnit->GetID());
				pUnit->SetAutomateType(NO_AUTOMATE);
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
						if(pUnit->GeneratePath(pLoopCity->plot(),CvUnit::MOVEFLAG_APPROX_TARGET_RING2,23))
						{
							pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pLoopCity->getX(), pLoopCity->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING2);
							pUnit->finishMoves();
							UnitProcessed(pUnit->GetID());
							bFoundWayHome = true;
							break;
						}
					}

					if(!bFoundWayHome)
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			CvPlot* pPlot = pUnit->plot();

			AI_PERF_FORMAT("Homeland-ExecuteWorkerMoves-perf.csv", ("ExecuteWorkerMoves, Turn %03d, %s, Unit %d, at x=%d, y=%d", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription(), pUnit->GetID(), pUnit->getX(), pUnit->getY()) );

			//fallout also counts as danger, so set the threshold a bit higher
			if(pPlot && pUnit->GetDanger(pPlot)>pUnit->GetCurrHitPoints()/2 && !pPlot->getBestDefender(m_pPlayer->GetID()))
			{
#if defined(MOD_AI_SECONDARY_WORKERS)
				if(MoveCivilianToSafety(pUnit, false, bSecondary))
#else
				if(MoveCivilianToSafety(pUnit))
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
			bool bActionPerformed = ExecuteWorkerMove(pUnit, bSecondary);
#else
			bool bActionPerformed = ExecuteWorkerMove(pUnit);
#endif
			if(bActionPerformed)
			{
				continue;
			}

#if defined(MOD_AI_SECONDARY_WORKERS)
			// Only move primary workers (actual civilian workers/workboats) or embarked secondary workers (combat units) to safety
			if (bSecondary && !pUnit->isEmbarked())
			{
				continue;
			}
#endif

			// if there's nothing else to do, move to the safest spot nearby
			if (pUnit->GetDanger()>0)
				MoveCivilianToSafety(pUnit, true /*bIgnoreUnits*/);
			else
				MoveCivilianToGarrison(pUnit);

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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
			if (pUnit->GetDanger()>0)
			{
				CvPlot* pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit,true);
				if (pBestPlot)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), CvUnit::MOVEFLAG_IGNORE_DANGER);
					pUnit->finishMoves();
					UnitProcessed(pUnit->GetID());
					continue;
				}
			}
			//Remove all status if not fortified so we can see if it is possible to fortify.
			if(pUnit->getFortifyTurns() <= 0)
			{
				pUnit->SetActivityType(NO_ACTIVITY);
			}
			if(pUnit->canFortify(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
				pUnit->SetFortifiedThisTurn(true);
			}
			else
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			}
			UnitProcessed(pUnit->GetID());
		}
	}
}

typedef CvWeightedVector<CvPlot*, 100, true> WeightedPlotVector;

/// Moves units to the hex with the lowest danger
void CvHomelandAI::ExecuteMovesToSafestPlot()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if(pUnit)
		{
			//so easy
			CvPlot* pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit,true);

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

//	---------------------------------------------------------------------------
/// Find one unit to move to target, starting with high priority list
void CvHomelandAI::ExecuteMoveToTarget(CvUnit* pUnit, CvPlot* pTarget, int iFlags, bool bFinishMoves)
{
	if (!pUnit || !pTarget)
		return;

	AI_PERF_FORMAT("Homeland-ExecuteMove-perf.csv", ("ExecuteMoveToTarget, %d, %d, Turn %03d, %s", pTarget->getX(), pTarget->getY(), GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	if(pUnit->plot() == pTarget)
	{
		//Remove all status if not fortified so we can see if it is possible to fortify.
		if(pUnit->getFortifyTurns() <= 0)
		{
			pUnit->SetActivityType(NO_ACTIVITY);
		}
		if (pUnit->canFortify(pUnit->plot()))
		{
			pUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			pUnit->SetFortifiedThisTurn(true);
		}
		else
			pUnit->PushMission(CvTypes::getMISSION_SKIP());

#if defined(MOD_BALANCE_CORE)
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Best unit fortified at X: %d, Y: %d", pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
#endif
	}
	else
	{
		// Best units have already had a full path check to the target, so just add the move
		MoveToTargetButDontEndTurn(pUnit, pTarget, iFlags);
	}

	//don't actually call finish moves, it will prevent healing
	if(bFinishMoves)
		UnitProcessed(pUnit->GetID());
}

/// Great writer moves
void CvHomelandAI::ExecuteWriterMoves()
{
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST:
			ExecuteCultureBlast(pUnit);
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

					MoveCivilianToSafety(pUnit);
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
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX,&iTurns))
						{
							// In less than one turn?
							if (iTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
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
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								pUnit->finishMoves();
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Writer toward Great Work city at, X: %d, Y: %d", pTargetCity->getX(),  pTargetCity->getY());
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
			MoveCivilianToSafety(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit);
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

					MoveCivilianToSafety(pUnit);
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
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX,&iTurns))
						{
							// In less than one turn?
							if (iTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
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
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								pUnit->finishMoves();
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Artist toward Great Work city at, X: %d, Y: %d", pTargetCity->getX(),  pTargetCity->getY());
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
			MoveCivilianToSafety(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
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

					MoveCivilianToSafety(pUnit);
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
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX,&iTurns))
						{
							// In less than one turn?
							if (iTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
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
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								pUnit->finishMoves();
								UnitProcessed(pUnit->GetID());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Musician toward Great Work city at, X: %d, Y: %d", pTargetCity->getX(),  pTargetCity->getY());
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
			MoveCivilianToSafety(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit);
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
			if (!ExecuteWorkerMove(pUnit))
				MoveCivilianToSafety(pUnit);
			break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			if (!ExecuteWorkerMove(pUnit))
				MoveCivilianToSafety(pUnit);
			break;
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit);
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

				MoveCivilianToSafety(pUnit);
			}
			else
			{
				bool bForceWonderCity = true;

				// Are we less than 25% done building the most desired wonder chosen by the city specialization AI?
				pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
				if(pWonderCity)
				{
#if defined(MOD_BALANCE_CORE)
					if(pUnit->AI_getUnitAIType() == UNITAI_ENGINEER)
					{
#endif
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
							}

							// No, then move there
							else
							{
								CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
								if(pEng)
								{
									ExecuteMoveToTarget(pEng, pWonderCity->plot(), 0, true);

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
#if defined(MOD_BALANCE_CORE)
					}
					else if(pUnit->IsCombatUnit())
					{
						CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
						if(pEng)
						{
							ExecuteMoveToTarget(pEng, pWonderCity->plot(), 0, true);
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Moving %s as garrison to city to boost production at specialization wonder city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
								LogHomelandMessage(strLogString);
							}
							continue;
						}
					}
#endif
				}

				if(bForceWonderCity)
				{
					pWonderCity = pCityToBuildAt;

					if(pWonderCity)
					{
#if defined(MOD_BALANCE_CORE)
						if(pUnit->AI_getUnitAIType() == UNITAI_ENGINEER)
						{
#endif
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
								CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
								if(pEng)
								{
									ExecuteMoveToTarget(pEng, pWonderCity->plot(), 0, true);

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
#if defined(MOD_BALANCE_CORE)
						}
						else if(pUnit->IsCombatUnit())
						{
							CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
							if(pEng)
							{
								ExecuteMoveToTarget(pEng, pWonderCity->plot(), 0, true);

								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving %s as garrison to boost wonder in city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
									LogHomelandMessage(strLogString);
								}
								continue;
							}
						}
#endif
					}
				}
			}
		}
		break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		//Handled by economic AI
		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		if (eDirective==GREAT_PEOPLE_DIRECTIVE_USE_POWER)
			continue;

		if (eDirective == GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT)
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
				else if( pUnit->CanReachInXTurns(pTarget,INT_MAX) )
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
				else if (pUnit->GetDanger()>0)
				{
					ExecuteMoveToTarget(pUnit, pTarget, CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY, true);

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

		//either no directive or trying to build embassy but walked into danger
		if (pUnit->canMove())
		{
			MoveCivilianToSafety(pUnit);
			UnitProcessed(pUnit->GetID());
			pUnit->finishMoves();

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving Great Diplomat to safety.");
				LogHomelandMessage(strLogString);
			}
		}
	}
}

void CvHomelandAI::ExecuteMessengerMoves()
{
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
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
			else if( pUnit->CanReachInXTurns(pTarget,INT_MAX) )
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
					ExecuteMoveToTarget(pUnit,pTarget, 0, true);

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
				MoveCivilianToSafety(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE:
			ExecuteGoldenAgeMove(pUnit);
			break;
		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
			// handled by economic AI
			break;
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			if (!ExecuteWorkerMove(pUnit))
				MoveCivilianToSafety(pUnit);
			break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = pUnit->GetGreatPeopleDirective();
		switch(eDirective)
		{
		case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			if (!ExecuteWorkerMove(pUnit))
				MoveCivilianToSafety(pUnit);
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
				CvCity* pTarget = m_pPlayer->GetReligionAI()->ChooseProphetConversionCity(false, pUnit, &iTargetTurns);
				if(pTarget)
				{
					if(iTargetTurns==0)
					{
						pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

						if(pUnit->canMove())
							pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Move to spread religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
							LogHomelandMessage(strLogString);
						}

						UnitProcessed(pUnit->GetID());
					}
					else
					{
						ExecuteMoveToTarget(pUnit, pTarget->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1, true);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
				else
					MoveCivilianToSafety(pUnit);
			}
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
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
	if (!MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	{
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
	if (!MOD_GLOBAL_BREAK_CIVILIAN_1UPT) 
	{
#endif
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eMyReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
		const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
		if(pMyReligion)
		{
			pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
			if(pHolyCityPlot != NULL)
			{
				pHolyCity = pHolyCityPlot->getPlotCity();
				if(pHolyCity && (pHolyCity->getOwner() == m_pPlayer->GetID()))
				{
					bKeepHolyCityClear = true;
				}
			}
		}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	}
#endif

	std::vector<CvPlot*> vPlotsToAvoid;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
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

			CvPlot* pTargetPlot = GET_PLAYER(m_pPlayer->GetID()).FindBestGreatGeneralTargetPlot(pUnit, vPlotsToAvoid, iValue);
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
					if (MoveToTargetButDontEndTurn(pUnit, pTargetPlot, 0))
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
			ExecuteGoldenAgeMove(pUnit);
			continue;
		}

#if defined(MOD_BALANCE_CORE_MILITARY)
		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND && !pUnit->IsCityAttackSupport())
		{
			int iBestScore = 0;
			CvPlot* pBestPlot = 0;
			int iAura = 0;
			//this directive should normally be handled in tactical AI (operation moves, close on target or hedgehog)
			//we could use ScoreGreatGeneralPlot() here, but maybe a different algorithm is a good idea
			ReachablePlots reachablePlots;
			TacticalAIHelpers::GetAllPlotsInReachThisTurn(pUnit,pUnit->plot(),reachablePlots,true,true,false);
			for (ReachablePlots::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
			{
				CvPlot* pCandidate = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
				//plot needs to have a defender, but no be adjacent to the enemy
				CvUnit* pDefender = pCandidate->getBestDefender(pUnit->getOwner());
				if (!pDefender || (pDefender->GetNumEnemyUnitsAdjacent()>0 && !pCandidate->isCity()))
					continue;

				if(pUnit->IsNearGreatGeneral(iAura, pCandidate, pUnit)) //near another general
					continue;

				//we want to have many neighboring units in danger, but our plot should be relatively safe
				//(look at the danger for the defender, the general danger is zero unless the defender is projected to die)
				int iGeneralDanger = m_pPlayer->GetPlotDanger(*pCandidate,pDefender);
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

					CvUnit* pSupportedUnit = pLoopPlot->getBestDefender(pUnit->getOwner());
					if (!pSupportedUnit)
						continue;

					iSupportedDanger += m_pPlayer->GetPlotDanger(*pLoopPlot,pSupportedUnit);
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
				CvUnit* pDefender = pBestPlot->getBestDefender(pUnit->getOwner());
				if (pDefender && pDefender->canMove())
				{
					pDefender->finishMoves();
					UnitProcessed(pDefender->GetID());
				}
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
			ReachablePlots reachablePlots;
			TacticalAIHelpers::GetAllPlotsInReachThisTurn(pUnit,pUnit->plot(),reachablePlots,true,true,false);
			for (ReachablePlots::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
			{

				int iScore = 0;
				CvPlot* pCandidate = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
				//plot needs to have a defender, but no be adjacent to the enemy
				CvUnit* pDefender = pCandidate->getBestDefender(pUnit->getOwner());
				if (!pDefender)
					continue;

				if(pUnit->IsNearCityAttackSupport(pCandidate, pUnit)) //near another general
					continue;

				//we want to have many neighboring units in danger, but our plot should be relatively safe
				//(look at the danger for the defender, the general danger is zero unless the defender is projected to die)
				int iGeneralDanger = m_pPlayer->GetPlotDanger(*pCandidate,pDefender);
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

					CvUnit* pSupportedUnit = pLoopPlot->getBestDefender(pUnit->getOwner());
					if (!pSupportedUnit)
						continue;

					if(pLoopPlot->IsEnemyCityAdjacent(pUnit->getTeam(),NULL))
						iScore += 5000;

					iSupportedDanger += m_pPlayer->GetPlotDanger(*pLoopPlot,pSupportedUnit);
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
				CvUnit* pDefender = pBestPlot->getBestDefender(pUnit->getOwner());
				if (pDefender && pDefender->canMove())
				{
					pDefender->finishMoves();
					UnitProcessed(pDefender->GetID());
					continue;
				}
			}
		}
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
		if( pUnit->GetGreatPeopleDirective() == NO_GREAT_PEOPLE_DIRECTIVE_TYPE || 
			(pUnit->IsCityAttackSupport() && pUnit->canRecruitFromTacticalAI() && !pUnit->IsRecentlyDeployedFromOperation()))
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
					MoveToTargetButDontEndTurn(pUnit,pChosenCity->plot(),0);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			ExecuteGoldenAgeMove(pUnit);
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns; //todo: don't pick the same target for all missionaries
		CvCity* pTarget = m_pPlayer->GetReligionAI()->ChooseMissionaryTargetCity(pUnit, &iTargetTurns);
		if(pTarget)
		{
			if(iTargetTurns==0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(),CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

				if(pUnit->canMove())
					pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Move to spread religion, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}

				UnitProcessed(pUnit->GetID());
			}
			else
			{
				ExecuteMoveToTarget(pUnit,pTarget->plot(),CvUnit::MOVEFLAG_APPROX_TARGET_RING1, true);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}
		}
#if defined(MOD_BALANCE_CORE)
		else
		{
			if(pUnit->isHuman())
			{
				pUnit->SetAutomateType(NO_AUTOMATE);
			}
			else
			{
				MoveCivilianToGarrison(pUnit);
				// slewis - this was removed because a unit would eat all its moves. So if it didn't do anything this turn, it wouldn't be able to work 
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				if (!m_pPlayer->isHuman())
				{
					pUnit->finishMoves();
				}		
			}
			UnitProcessed(pUnit->GetID());
			continue;
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns;
		CvCity* pTarget = m_pPlayer->GetReligionAI()->ChooseInquisitorTargetCity(pUnit, &iTargetTurns);
		if(pTarget)
		{
			if(iTargetTurns == 0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

				if(pUnit->canMove())
					pUnit->PushMission(CvTypes::getMISSION_REMOVE_HERESY());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Move to remove heresy, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}

				UnitProcessed(pUnit->GetID());
			}
			else
			{
				ExecuteMoveToTarget(pUnit, pTarget->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1, true);

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
		MoveUnitsArray::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
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
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
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
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
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
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());

		if(pUnit)
		{
			// Am I eligible to intercept?
#if defined(MOD_AI_SMART_AIR_TACTICS) && defined(MOD_BALANCE_CORE_MILITARY)
			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetTacticalAI()->ShouldRebase(pUnit))
#else
			if(pUnit->canAirPatrol(NULL) && !m_pPlayer->GetMilitaryAI()->WillAirUnitRebase(pUnit))
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
			CvCity* pRefCity = m_pPlayer->GetClosestCityByEstimatedTurns(pLoopUnit->plot());
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
					SPath path = GC.GetStepFinder().GetPath(pUnit->getX(),pUnit->getY(),it->pPlot->getX(),it->pPlot->getY(),data);
					if (!!path)
					{
						pNewBase = PathHelpers::GetPathFirstPlot(path);
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
				SPath path = GC.GetStepFinder().GetPath(pUnit->getX(),pUnit->getY(),it->pPlot->getX(),it->pPlot->getY(),data);
				if (!!path)
				{
					pNewBase = PathHelpers::GetPathFirstPlot(path);
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

#if defined(MOD_BALANCE_CORE)
bool CvHomelandAI::MoveCivilianToGarrison(CvUnit* pUnit)
{
	WeightedPlotVector aBestPlotList;
	aBestPlotList.clear();
	int iLoopCity;
	for(CvCity *pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity != NULL)
		{
			if(pLoopCity->isInDangerOfFalling())
				continue;

			CvPlot* pLoopPlot = pLoopCity->plot();

			if(!pLoopPlot->isValidMovePlot(pUnit->getOwner(), true))
			{
				continue;
			}

#if defined(MOD_GLOBAL_STACKING_RULES)
			if (pLoopPlot->getMaxFriendlyUnitsOfType(pUnit) >= pLoopPlot->getUnitLimit())
#else
			if (pLoopPlot->getMaxFriendlyUnitsOfType(pUnit) < GC.getPLOT_UNIT_LIMIT())
#endif
			{
				continue;
			}

			//Flat value.
			int iValue = pLoopCity->getPopulation();

			if(pLoopPlot->getArea() != pUnit->getArea())
			{
				iValue /= 2;
			}
			int iFirstUnitID;
			int iNumFriendlies = pLoopPlot->getNumUnitsOfAIType(pUnit->AI_getUnitAIType(), iFirstUnitID) * 10;

			iValue -= iNumFriendlies;

			//Add back in our value if this is our plot.
			if(pLoopPlot == pUnit->plot())
			{
				iValue += 10;
			}
			
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
					strLogString.Format("%s (%d) tried to spread out but is at the best spot, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
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
					strLogString.Format("%s (%d) tried to spread out but cannot hold in current location, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
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
				strLogString.Format("%s (%d) spreading out, X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY());
				LogHomelandMessage(strLogString);
			}

			MoveToTargetButDontEndTurn(pUnit, pBestPlot, 0);
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
			strLogString.Format("%s (%d) tried to spread out but couldn't find a good place to go", strTemp.GetCString(), pUnit->GetID());
			LogHomelandMessage(strLogString);
		}
	}


	return false;
}
#endif
/// Fleeing to safety for civilian units
#if defined(MOD_AI_SECONDARY_WORKERS)
bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits, bool bSecondary)
#else
bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit, bool bIgnoreUnits)
#endif
{
	WeightedPlotVector aBestPlotList;
	ReachablePlots reachablePlots;
	TacticalAIHelpers::GetAllPlotsInReachThisTurn(pUnit,pUnit->plot(),reachablePlots,true,true,true);
	for (ReachablePlots::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
	{
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

			if(!pLoopPlot->isValidMovePlot(pUnit->getOwner(),!pUnit->isRivalTerritory()))
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

			MoveToTargetButDontEndTurn(pUnit, pBestPlot, 0);
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

	TradeConnectionList aTradeConnections;
	pkTradeAI->GetPrioritizedTradeRoutes(aTradeConnections,true);

	//don't start all routes from the same city, try to spread them out
	//note this is only for this turn - next turn it's a new game
	std::set<int> originCities;
	
	//stats to decide whether to disband a unit
	int iWaterRoutes = 0;
	int iLandRoutes = 0;
#if defined(MOD_BALANCE_CORE)
	int iDupes = 0;
#endif
	//for the N best trade routes, find a suitable unit
	//check at least the 8 best routes, at max 3 times the number of free trade units
	uint nRoutesToCheck = MIN(aTradeConnections.size(),MAX(m_CurrentMoveUnits.size()*3,8u));
	for (uint ui = 0; ui < nRoutesToCheck; ui++)
	{
		CvPlot* pOriginPlot = GC.getMap().plot(aTradeConnections[ui].m_iOriginX, aTradeConnections[ui].m_iOriginY);

		//stats
		if (aTradeConnections[ui].m_eDomain==DOMAIN_SEA)
			iWaterRoutes++;
		if (aTradeConnections[ui].m_eDomain==DOMAIN_LAND)
			iLandRoutes++;

#if defined(MOD_BALANCE_CORE)	
		//if we already made 3 routes from here this turn, then skip it
		if (originCities.find(aTradeConnections[ui].m_iOriginID)!=originCities.end())
			iDupes++;

		if(iDupes >= 3)
			continue;
#else
		//if we already made a route from here this turn, then skip it
		if (originCities.find(aTradeConnections[ui].m_iOriginID)!=originCities.end())
			continue;
#endif

		//we don't really care about the distance but not having to re-base is good
		CvUnit *pBestUnit = NULL;
		int iBestDistance = INT_MAX;

		MoveUnitsArray::iterator it;
		for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
#if defined(MOD_BALANCE_CORE)
			if(!pUnit || !pUnit->canMove() || pUnit->TurnProcessed() || pUnit->IsAutomated())
#else
			if(!pUnit || !pUnit->canMove())
#endif
				continue;

			if (aTradeConnections[ui].m_eDomain != pUnit->getDomainType())
				continue;

			if (pUnit->canMakeTradeRouteAt(pOriginPlot, aTradeConnections[ui].m_iDestX, aTradeConnections[ui].m_iDestY, aTradeConnections[ui].m_eConnectionType))
			{
				int iDistance = plotDistance(*pOriginPlot,*pUnit->plot());
				if (iDistance < iBestDistance)
				{
					pBestUnit = pUnit;
					iDistance = MIN(iDistance,iBestDistance);

					//can't get any better
					if (iDistance==0)
						break;
				}
			}
		}

		if (pBestUnit)
		{
			CvCity* pOriginCity = CvGameTrade::GetOriginCity(aTradeConnections[ui]);
			CvCity* pDestCity = CvGameTrade::GetDestCity(aTradeConnections[ui]);

			//remember this city
			originCities.insert(pOriginCity->GetID());

			if (pOriginPlot != pBestUnit->plot())
			{
				pBestUnit->PushMission(CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY(), pOriginPlot->getX(), pOriginPlot->getY());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Changing trade route home city from %s to %s", pOriginCity->getName().c_str(), pDestCity->getName().c_str());
					LogHomelandMessage(strLogString);
				}
			}
			else
			{
				pBestUnit->PushMission(CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE(), pDestCity->plot()->GetPlotIndex(), aTradeConnections[ui].m_eConnectionType);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					switch (aTradeConnections[ui].m_eConnectionType)
					{
					case TRADE_CONNECTION_FOOD:
						strLogString.Format("Establishing food trade route from %s to %s", pOriginCity->getName().c_str(), pDestCity->getName().c_str());
						break;
					case TRADE_CONNECTION_INTERNATIONAL:
						strLogString.Format("Establishing gold trade route from %s to %s", pOriginCity->getName().c_str(), pDestCity->getName().c_str());
						break;
					case TRADE_CONNECTION_PRODUCTION:
						strLogString.Format("Establishing production trade route from %s to %s", pOriginCity->getName().c_str(), pDestCity->getName().c_str());
						break;
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
					case TRADE_CONNECTION_WONDER_RESOURCE:
						strLogString.Format("Establishing wonder trade route from %s to %s", pOriginCity->getName().c_str(), pDestCity->getName().c_str());
						break;
#endif
					}

					LogHomelandMessage(strLogString);
				}
				UnitProcessed(pBestUnit->GetID());
			}
		}
	}

	//unassigned trade units?
	MoveUnitsArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit || !pUnit->canMove() || pUnit->TurnProcessed())
			continue;

		bool bKill = false;

		if ( pUnit->getDomainType()==DOMAIN_SEA )
			bKill = ((iWaterRoutes+2<iLandRoutes) || (iWaterRoutes == 0));

		if ( pUnit->getDomainType()==DOMAIN_LAND )
			bKill = ((iLandRoutes+2<iWaterRoutes) || (iLandRoutes == 0));

		if (bKill)
		{
			if(GC.getLogging() && GC.getAILogging())
			{	
				CvString strLogString;
				strLogString.Format("Disbanding %s trade unit %d because no suitable target",pUnit->getDomainType()==DOMAIN_SEA?"sea":"land",pUnit->GetID());
				LogHomelandMessage(strLogString);
			}
			pUnit->kill(true);
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
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Archaeologist moving to site at, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
				LogHomelandMessage(strLogString);
			}

			if(pUnit->plot() == pTarget)
			{
				BuildTypes eBuild = (BuildTypes)GC.getInfoTypeForString("BUILD_ARCHAEOLOGY_DIG");
				pUnit->PushMission(CvTypes::getMISSION_BUILD(), eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Archaeologist creating dig at, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
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
		//Let's make sure that we exclude forts here. If the first is a sentry, then we can make the check.
		if((it->GetTargetType() == AI_HOMELAND_TARGET_FORT))
		{
			m_TargetedSentryPoints.push_back(*it);
		}
		else
		{
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
}
#if defined(MOD_BALANCE_CORE)
/// Don't allow adjacent tiles to both be sentry points
void CvHomelandAI::EliminateAdjacentNavalSentryPoints()
{
	// First, sort the sentry points by priority
	std::stable_sort(m_TargetedNavalSentryPoints.begin(), m_TargetedNavalSentryPoints.end());

	// Create temporary copy of list
	std::vector<CvHomelandTarget> tempPoints;
	tempPoints = m_TargetedNavalSentryPoints;

	// Clear out main list
	m_TargetedNavalSentryPoints.clear();

	// Loop through all points in copy
	std::vector<CvHomelandTarget>::iterator it, it2;
	for(it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;
		// Is it adjacent to a point in the main list?
		for(it2 = m_TargetedNavalSentryPoints.begin(); it2 != m_TargetedNavalSentryPoints.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}
		if(!bFoundAdjacent)
		{
			m_TargetedNavalSentryPoints.push_back(*it);
		}
	}
}
void CvHomelandAI::EliminateAdjacentUnassignedPoints()
{
	// First, sort the sentry points by priority
	std::stable_sort(m_TargetedHomeUnassignedPlots.begin(), m_TargetedHomeUnassignedPlots.end());

	// Create temporary copy of list
	std::vector<CvHomelandTarget> tempPoints;
	tempPoints = m_TargetedHomeUnassignedPlots;

	// Clear out main list
	m_TargetedHomeUnassignedPlots.clear();

	// Loop through all points in copy
	std::vector<CvHomelandTarget>::iterator it, it2;
	for (it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		bool bFoundAdjacent = false;
		// Is it adjacent to a point in the main list?
		for (it2 = m_TargetedHomeUnassignedPlots.begin(); it2 != m_TargetedHomeUnassignedPlots.end(); ++it2)
		{
			if (plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				bFoundAdjacent = true;
				break;
			}
		}
		if (!bFoundAdjacent)
		{
			m_TargetedHomeUnassignedPlots.push_back(*it);
		}
	}
}
#endif

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
		int nAdjacent = 0;

		// Is it adjacent to a point in the main list?
		for(it2 = m_TargetedHomelandRoads.begin(); it2 != m_TargetedHomelandRoads.end(); ++it2)
		{
			if(plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
			{
				nAdjacent++;
				if (nAdjacent>1)
					break;
			}
		}

		//just make sure some road plots remain free so other units can move
		if(nAdjacent<2)
		{
			m_TargetedHomelandRoads.push_back(*it);
		}
	}
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
			CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
			if(pLoopUnit && !pLoopUnit->isHuman())
			{
				// Civilians or units in armies aren't useful for any of these moves
				if(!pLoopUnit->canMove() || !pLoopUnit->IsCombatUnit() || pLoopUnit->getArmyID() != -1)
				{
					continue;
				}
				if(eMove != AI_HOMELAND_MOVE_SENTRY_NAVAL && pLoopUnit->getDomainType()!=DOMAIN_LAND)
					continue;

				bool bSuitableUnit = false;
				bool bHighPriority = false;

				switch(eMove)
				{
				case AI_HOMELAND_MOVE_GARRISON:
				case AI_HOMELAND_MOVE_GARRISON_CITY_STATE:
					//Don't poach garrisons for garrisons.
					if(pLoopUnit->IsGarrisoned())
						continue;

					if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE)
						continue;

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
					if(pLoopUnit->isFortifyable() && !pLoopUnit->IsGarrisoned() && !pLoopUnit->noDefensiveBonus() && (UnitAITypes)pLoopUnit->getUnitInfo().GetDefaultUnitAIType() != UNITAI_EXPLORE)
					{
						bSuitableUnit = true;

						// Units with extra defense are especially valuable
						if((pLoopUnit->getDefenseModifier() > 0) || (pLoopUnit->getExtraRangedDefenseModifier() > 0) || (pLoopUnit->getExtraVisibilityRange() > 0))
						{
							bHighPriority = true;
						}
					}
					break;
#if defined(MOD_BALANCE_CORE)
				case AI_HOMELAND_MOVE_SENTRY_NAVAL:
					// No ranged units as sentries
					if(pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit() && (UnitAITypes)pLoopUnit->getUnitInfo().GetDefaultUnitAIType() != UNITAI_EXPLORE_SEA)
					{
						bSuitableUnit = true;

						// Units with extra defense are especially valuable
						if(!pLoopUnit->isRanged() || (pLoopUnit->getDefenseModifier() > 0) || (pLoopUnit->getExtraRangedDefenseModifier() > 0) || (pLoopUnit->getExtraVisibilityRange() > 0))
						{
							bHighPriority = true;
						}
					}
					break;
#endif

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
	// Sort by raw distance
	std::stable_sort(kMoveUnits.begin(), kMoveUnits.end());

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
		CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetID());
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
					pBestUnit = pLoopUnit;
					iMinTurns = iMoves;
					break;
				}
				if (iMoves <= iDistance && iMoves <= iMaxTurns && iMoves < iMinTurns)
#else
				if (iDistance == 0 || (iMoves <= iDistance && iMoves <= iMaxTurns && iMoves < iMinTurns))
#endif
				{
					pBestUnit = pLoopUnit;
					iMinTurns = iMoves;
					break;
				}

				if (iMoves < iMinTurns)
				{
					pBestUnit = pLoopUnit;
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

	CvUnit* pLoopUnit;
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
			
			if (pLoopUnit->plot() == pTarget)
			{
				return pLoopUnit;
			}
			else
			{
				// Make sure we can move into the destination.  The path finder will do a similar check near the beginning, but it is best to get this out of the way before then
				if (!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION))
				{
					it->SetMovesToTarget(MAX_INT);
					continue;
				}

				int iPlotDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iTargetX, iTargetY);
				it->SetMovesToTarget(iPlotDistance);
			}
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

			if (pLoopUnit->plot() == pTarget)
			{
				return pLoopUnit;
			}
			else
			{
				// Make sure we can move into the destination.  The path finder will do a similar check near the beginning, but it is best to get this out of the way before then
				if (!pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION))
				{
					it->SetMovesToTarget(MAX_INT);
					continue;
				}

				int iPlotDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iTargetX, iTargetY);
				it->SetMovesToTarget(iPlotDistance);
			}
		}
	}

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
		if (pUnit->plot()==pTarget)
		{
			pBestTarget = pTarget;
			break;
		}

		//ignore if another digger is already near
		bool bIgnore = false;
		for (int i=0; i<RING1_PLOTS; i++)
		{
			int iUnitID = 0;
			CvPlot* pTest = iterateRingPlots(pTarget,i);
			if (pTest && pTest->getNumUnitsOfAIType(UNITAI_ARCHAEOLOGIST,iUnitID)>0)
			{
				if (iUnitID != pUnit->GetID())
				{
					bIgnore = true;
					break;
				}
			}
		}

		if (bIgnore)
			continue;

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

#if defined(MOD_BALANCE_CORE)
/// Find best target for this unassigned unit
CvPlot* CvHomelandAI::FindUnassignedTarget(CvUnit *pUnit)
{
	CvPlot *pBestTarget = NULL;
	int iBestTurns = 8;

	// Reverse the logic from most of the Homeland moves; for this we'll loop through units and find the best targets for them (instead of vice versa)
	std::vector<CvHomelandTarget>::iterator it;
	for (it = m_TargetedHomeUnassignedPlots.begin(); it != m_TargetedHomeUnassignedPlots.end(); it++)
	{
		CvPlot* pTarget = GC.getMap().plot(it->GetTargetX(), it->GetTargetY());
		if (pUnit->plot() == pTarget)
		{
			pBestTarget = pTarget;
			break;
		}

		int iTurns = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY());
		if (iTurns < iBestTurns)
		{
			pBestTarget = pTarget;
			iBestTurns = iTurns;
		}
	}

	// Erase this site from future contention
	if (pBestTarget)
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("UnitID: %d Found unassigned plot for move, X: %d, Y: %d, from X: %d Y: %d", pUnit->GetID(), pBestTarget->getX(), pBestTarget->getY(), pUnit->getX(), pUnit->getY());
			LogHomelandMessage(strLogString);
		}
		for (it = m_TargetedHomeUnassignedPlots.begin(); it != m_TargetedHomeUnassignedPlots.end(); it++)
		{
			if (it->GetTargetX() == pBestTarget->getX() && it->GetTargetY() == pBestTarget->getY())
			{
				m_TargetedHomeUnassignedPlots.erase(it);
				break;
			}
		}
	}
	return pBestTarget;
}
#endif

void CvHomelandAI::LogHomelandMessage(const CvString& strMsg)
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

void CvHomelandAI::LogPatrolMessage(const CvString& strMsg, CvUnit* pPatrolUnit )
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
	m_CurrentTurnUnits.remove(iID);

	CvUnit* pUnit = m_pPlayer->getUnit(iID);
	if(pUnit)
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
	if (!pPlot || !pUnit)
	{
		return false;
	}

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

	if(!pUnit->canMoveInto(*pPlot,CvUnit::MOVEFLAG_DESTINATION))
	{
		return false;
	}

	return true;
}

/// Move an exploring unit to a designated target (special function exposed to Lua)
bool CvHomelandAI::ExecuteSpecialExploreMove(CvUnit* pUnit, CvPlot* pTargetPlot)
{
	SPathFinderUserData data(pUnit,CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE | CvUnit::MOVEFLAG_IGNORE_DANGER);
	SPath path = GC.GetPathFinder().GetPath(pUnit->getX(), pUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY(), data);
	if(!!path)
	{
		CvPlot* pPlot = PathHelpers::GetPathEndFirstTurnPlot(path);
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

bool CvHomelandAI::MoveToTargetButDontEndTurn(CvUnit* pUnit, CvPlot* pTargetPlot, int iFlags)
{
	if(pUnit->GeneratePath(pTargetPlot,iFlags))
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY(), iFlags);
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
			//Remove all status if not fortified so we can see if it is possible to fortify.
			if(pUnit->getFortifyTurns() <= 0)
			{
				pUnit->SetActivityType(NO_ACTIVITY);
			}

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
bool CvHomelandAI::MoveToEmptySpaceNearTarget(CvUnit* pUnit, CvPlot* pTarget, DomainTypes eDomain, int iMaxTurns)
{
	if (!pUnit || !pTarget)
		return false;

	//nothing to do?
	if ((plotDistance(pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY()) < 3) &&
		(eDomain == NO_DOMAIN || pTarget->getDomain() == eDomain) &&
		(pUnit->plot()->isAdjacentToArea(pTarget->getArea())))
	{
		pUnit->PushMission(CvTypes::getMISSION_SKIP());
		return true;
	}

	int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2;
	if (eDomain == pTarget->getDomain())
		iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;

	int iTurns = pUnit->TurnsToReachTarget(pTarget, iFlags, iMaxTurns);
	if (iTurns <= iMaxTurns)
		return MoveToTargetButDontEndTurn(pUnit, pTarget, iFlags);

	return false;
}


int g_currentHomelandUnitToTrack = 0;

const char* homelandMoveNames[] =
{
	"H_MOVE_UNASSIGNED",
	"H_MOVE_EXPLORE",
	"H_MOVE_EXPLORE_SEA",
	"H_MOVE_SETTLE",
	"H_MOVE_GARRISON",
	"H_MOVE_HEAL",
	"H_MOVE_TO_SAFETY",
	"H_MOVE_MOBILE_RESERVE",
	"H_MOVE_SENTRY",
	"H_MOVE_WORKER",
	"H_MOVE_WORKER_SEA",
	"H_MOVE_PATROL",
	"H_MOVE_UPGRADE",
	"H_MOVE_ANCIENT_RUINS",
	"H_MOVE_GARRISON_CITY_STATE",
	"H_MOVE_WRITER",
	"H_MOVE_ARTIST_GOLDEN_AGE",
	"H_MOVE_MUSICIAN",
	"H_MOVE_SCIENTIST_FREE_TECH",
	"H_MOVE_MERCHANT_TRADE",
	"H_MOVE_ENGINEER_HURRY",
	"H_MOVE_GENERAL",
	"H_MOVE_ADMIRAL",
	"H_MOVE_SPACESHIP_PART",
	"H_MOVE_AIRCRAFT_REBASE",
	"H_MOVE_TREASURE",
	"H_MOVE_PROPHET_RELIGION",
	"H_MOVE_MISSIONARY",
	"H_MOVE_INQUISITOR",
	"H_MOVE_TRADE_UNIT",
	"H_MOVE_ARCHAEOLOGIST",
	"H_MOVE_ADD_SPACESHIP_PART",
	"H_MOVE_AIRLIFT",
	"H_MOVE_DIPLOMAT_EMBASSY",
	"H_MOVE_MESSENGER",
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
		CvUnit* pDefender = pBasePlot->getBestDefender(ePlayer);
		if(!pDefender || pDefender->isProjectedToDieNextTurn() || kPlayer.GetTacticalAI()->IsUnitHealing(pDefender->GetID()))  
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
	CvTacticalAnalysisMap* pTactMap = GET_PLAYER(ePlayer).GetTacticalAI()->GetTacticalAnalysisMap();
	CvTacticalDominanceZone* pZone = pTactMap->GetZoneByPlot( pBasePlot );
	if (!pZone)
	{
		//don't know what to do with this
		return iBaseScore;
	}

	const std::vector<int>& vNeighborZones = pZone->GetNeighboringZones();
	for (size_t i=0; i<vNeighborZones.size(); i++)
	{
		CvTacticalDominanceZone* pOtherZone = pTactMap->GetZoneByID( vNeighborZones[i] );
		if (pOtherZone && std::find(vFutureEnemies.begin(),vFutureEnemies.end(),pOtherZone->GetOwner())!=vFutureEnemies.end())
			iBaseScore += 1;
	}

	return iBaseScore;
}
//check all tactical zones to find the one we need to support most
std::vector<CvPlot*> HomelandAIHelpers::GetAggressivePatrolTargets(PlayerTypes ePlayer, bool bWater, int nMaxTargets)
{
	if (ePlayer==NO_PLAYER)
		return std::vector<CvPlot*>();

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	const std::vector<PlayerTypes>& vEnemies = kPlayer.GetPlayersAtWarWith();
	CvTacticalAnalysisMap* pTactMap = kPlayer.GetTacticalAI()->GetTacticalAnalysisMap();

	std::vector<SPlotWithScore> vTargets;
	for(int iI = 0; iI < pTactMap->GetNumZones(); iI++)
	{
		CvTacticalDominanceZone* pZone = pTactMap->GetZoneByIndex(iI);

		if (!pZone || pZone->IsWater()!=bWater)
			continue;

		PlayerTypes eOwner = pZone->GetOwner();
		if ((eOwner != ePlayer) || !GET_PLAYER(eOwner).IsAtWarWith(ePlayer))
			continue;

		//watch out, a city can occur multiple times (islands ...)
		CvCity* pZoneCity = pZone->GetZoneCity();
		if (!pZoneCity)
			continue;

		int iFriendlyPower = pZone->GetFriendlyStrength();
		int iEnemyPower = pZone->GetEnemyStrength();

		if(bWater)
		{
			iEnemyPower += pZone->GetEnemyNavalUnitCount() * 100;
		}

		iEnemyPower += (GET_PLAYER(ePlayer).getNumCities() - pZoneCity->GetThreatRank()) * 100;

		const std::vector<int>& vNeighborZones = pZone->GetNeighboringZones();
		for (size_t i=0; i<vNeighborZones.size(); i++)
		{
			CvTacticalDominanceZone* pOtherZone = pTactMap->GetZoneByID( vNeighborZones[i] );
			if (!pOtherZone)
				continue;

			//some base strength for zones with low visibility
			if (pOtherZone->GetOwner()!=ePlayer)
				iEnemyPower += (pOtherZone->GetOwner()!=NO_PLAYER) ? 10000 : 2000;

			if (std::find(vEnemies.begin(),vEnemies.end(),pOtherZone->GetOwner())!=vEnemies.end())
				iEnemyPower += pOtherZone->GetNeutralStrength();

			CvCity* pOtherZoneCity = pOtherZone->GetZoneCity();
			if (pOtherZoneCity)
			{
				iEnemyPower += (GET_PLAYER(ePlayer).getNumCities() - pOtherZoneCity->GetThreatRank()) * 100;
			}

			if(bWater)
			{
				iEnemyPower += pZone->GetEnemyNavalUnitCount() * 100;
			}

			//different domain counts less
			int iScale = (pOtherZone->IsWater() != pZone->IsWater()) ? 3 : 1;
			iEnemyPower += pOtherZone->GetEnemyStrength() / iScale;

			iFriendlyPower =+ pOtherZone->GetFriendlyStrength();
		}

		int iScore = (iEnemyPower*1000)/max(1,iFriendlyPower);
		vTargets.push_back( SPlotWithScore(pZoneCity->plot(),iScore) );
	}

	//sort descending!
	std::sort( vTargets.begin(), vTargets.end() );
	std::reverse( vTargets.begin(), vTargets.end() );

	std::vector<CvPlot*> vResult;
	for (size_t i=0; i<MIN(vTargets.size(),(size_t)nMaxTargets); i++)
		//copy the top N results, take care not to have duplicate cities in there
		if (std::find(vResult.begin(),vResult.end(),vTargets[i].pPlot)==vResult.end())
			vResult.push_back( vTargets[i].pPlot );

	return vResult;
}
//check all tactical zones to find the one we need to support most
std::vector<CvPlot*> HomelandAIHelpers::GetPatrolTargets(PlayerTypes ePlayer, bool bWater, int nMaxTargets)
{
	if (ePlayer==NO_PLAYER)
		return std::vector<CvPlot*>();

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	const std::vector<PlayerTypes>& vFutureEnemies = kPlayer.GetPlayersAtWarWithInFuture();
	CvTacticalAnalysisMap* pTactMap = kPlayer.GetTacticalAI()->GetTacticalAnalysisMap();

	std::vector<SPlotWithScore> vTargets;
	for(int iI = 0; iI < pTactMap->GetNumZones(); iI++)
	{
		CvTacticalDominanceZone* pZone = pTactMap->GetZoneByIndex(iI);
		if (!pZone || pZone->GetOwner()!=ePlayer || pZone->IsWater()!=bWater)
			continue;

		//watch out, a city can occur multiple times (islands ...)
		CvCity* pZoneCity = pZone->GetZoneCity();
		if (!pZoneCity)
			continue;

		int iFriendlyPower = pZone->GetFriendlyStrength();
		int iEnemyPower = pZone->GetEnemyStrength();

		if(bWater)
			iEnemyPower += pZone->GetEnemyNavalUnitCount() * 100;

		iEnemyPower += (GET_PLAYER(ePlayer).getNumCities() - pZoneCity->GetThreatRank()) * 100;

		const std::vector<int>& vNeighborZones = pZone->GetNeighboringZones();
		for (size_t i=0; i<vNeighborZones.size(); i++)
		{
			CvTacticalDominanceZone* pOtherZone = pTactMap->GetZoneByID( vNeighborZones[i] );
			if (!pOtherZone)
				continue;

			//some base strength for zones with low visibility
			if (pOtherZone->GetOwner()!=ePlayer)
				iEnemyPower += (pOtherZone->GetOwner()!=NO_PLAYER) ? 10000 : 2000;

			if (std::find(vFutureEnemies.begin(),vFutureEnemies.end(),pOtherZone->GetOwner())!=vFutureEnemies.end())
				iEnemyPower += pOtherZone->GetNeutralStrength();
			else
				iEnemyPower += pOtherZone->GetNeutralStrength()/2;

			CvCity* pOtherZoneCity = pOtherZone->GetZoneCity();
			if (pOtherZoneCity)
				iEnemyPower += (GET_PLAYER(ePlayer).getNumCities() - pOtherZoneCity->GetThreatRank()) * 100;

			if(bWater)
				iEnemyPower += pZone->GetEnemyNavalUnitCount() * 100;

			//different domain counts less
			int iScale = (pOtherZone->IsWater() != pZone->IsWater()) ? 3 : 1;
			iEnemyPower += pOtherZone->GetEnemyStrength() / iScale;

			iFriendlyPower =+ pOtherZone->GetFriendlyStrength();
		}

		int iScore = (iEnemyPower*1000)/max(1,iFriendlyPower);
		vTargets.push_back( SPlotWithScore(pZoneCity->plot(),iScore) );
	}

	//sort descending!
	std::sort( vTargets.begin(), vTargets.end() );
	std::reverse( vTargets.begin(), vTargets.end() );

	std::vector<CvPlot*> vResult;
	for (size_t i=0; i<MIN(vTargets.size(),(size_t)nMaxTargets); i++)
		//copy the top N results, take care not to have duplicate cities in there
		if (std::find(vResult.begin(),vResult.end(),vTargets[i].pPlot)==vResult.end())
			vResult.push_back( vTargets[i].pPlot );

	return vResult;
}