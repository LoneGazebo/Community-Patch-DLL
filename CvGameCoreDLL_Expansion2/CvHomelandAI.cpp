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
	m_CurrentTurnUnits.clear();
	m_automatedTargetPlots.clear();

	// Loop through our units
	int iLoop;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Sanity check
		if (pLoopUnit->IsGreatGeneral() && pLoopUnit->plot()->getNumDefenders(pLoopUnit->getOwner()) == 0 && pLoopUnit->GetDanger()>99)
			OutputDebugString( CvString::format("undefended general found at %d:%d!\n",pLoopUnit->getX(),pLoopUnit->getY()).c_str());

		// Never want units that have already moved or zombies
		if (pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath())
		{
			//if there is an unfinished mission but the unit can still move, that means there is a problem ...
			if (pLoopUnit->GetLengthMissionQueue() > 0 && pLoopUnit->canMove())
				pLoopUnit->ClearMissionQueue();

			continue;
		}

		//don't touch armies or units which are out of moves
		if(!pLoopUnit->canMove() || pLoopUnit->getArmyID()!=-1)
		{
			pLoopUnit->SetTurnProcessed(true);
			continue;
		}

#if defined(VPDEBUG)
		if ( pLoopUnit->hasCurrentTacticalMove() )
		{
			CvString msg = CvString::format("warning: homeland AI unit %d has a current tactical move (%s at %d,%d)", 
									pLoopUnit->GetID(), pLoopUnit->getName().c_str(), pLoopUnit->getX(), pLoopUnit->getY() );
			LogHomelandMessage( msg );
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
			if (!pUnit)
				continue;
			CvString msg = CvString::format("current turn homeland unit %s %d at %d,%d\n", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY() );
			LogHomelandMessage( msg );
		}
	}
#endif
}

/// Mark all the units that will be under tactical AI control this turn
void CvHomelandAI::FindAutomatedUnits()
{
	int iLoop;

	m_CurrentTurnUnits.clear();
	m_automatedTargetPlots.clear();

	// Loop through our units
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if(!pLoopUnit->IsAutomated() || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN)
			continue;

		if (pLoopUnit->GetMissionAIPlot())
			m_automatedTargetPlots[pLoopUnit->AI_getUnitAIType()].push_back( std::make_pair(pLoopUnit->GetID(),pLoopUnit->GetMissionAIPlot()->GetPlotIndex()) );
	
		if (!pLoopUnit->TurnProcessed() && pLoopUnit->canMove())
			m_CurrentTurnUnits.push_back(pLoopUnit->GetID());
	}
}

/// Update the AI for units
void CvHomelandAI::Update()
{
	AI_PERF_FORMAT("AI-perf.csv", ("Homeland AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()));

	//no homeland for barbarians
	if(m_pPlayer->GetID() == BARBARIAN_PLAYER)
	{
		int iLoop;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{
			if (!pLoopUnit->TurnProcessed())
				pLoopUnit->SetTurnProcessed(true);
		}
		return;
	}

	if (m_pPlayer->isHuman())
		FindAutomatedUnits();
	else
		RecruitUnits();

	// Make sure we have a unit to handle
	if(!m_CurrentTurnUnits.empty())
	{
		// Put together lists of places we may want to move toward
		FindHomelandTargets();

		//so that workers know where to build roads
		m_pPlayer->GetBuilderTaskingAI()->Update();

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
			CvPlot* pCurPlot = pUnit->plot();
			if (pCurPlot->isCity())
			{
				if (!pCurPlot->getPlotCity()->isMatchingArea(vExplorePlots[ui].pPlot))
					continue;
			}
			else
			{
				if (pCurPlot->getArea() != vExplorePlots[ui].pPlot->getArea())
					continue;
			}
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
		SPathFinderUserData data(pUnit, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE, iMaxTurns);
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

/// Make lists of everything we might want to target with the homeland AI this turn
void CvHomelandAI::FindHomelandTargets()
{
	CvHomelandTarget newTarget;

	// Clear out target lists since we rebuild them each turn
	m_TargetedCities.clear();
	m_TargetedSentryPoints.clear();
	m_TargetedNavalResources.clear();
	m_TargetedAncientRuins.clear();
	m_TargetedAntiquitySites.clear();
	m_TargetedNavalSentryPoints.clear();

	TeamTypes eTeam = m_pPlayer->getTeam();

	// Look at every tile on map
	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();

	for(int iI = 0; iI < iNumPlots; iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);

		if(pLoopPlot->isVisible(m_pPlayer->getTeam()))
		{
			// Have a ...
			// ... friendly city?
			CvCity* pCity = pLoopPlot->getPlotCity();
			if(pCity != NULL && m_pPlayer->GetID() == pCity->getOwner())
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_CITY);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				newTarget.SetAuxIntData(pCity->getThreatValue());
				m_TargetedCities.push_back(newTarget);
			}
			// ... naval resource?
			if(pLoopPlot->isWater() && pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
			{
				ResourceTypes eNonObsoleteResource = pLoopPlot->getResourceType(eTeam);
				if(eNonObsoleteResource != NO_RESOURCE)
				{
					if(pLoopPlot->getOwner() == m_pPlayer->GetID())
					{
						// Find proper improvement
						for(int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							BuildTypes eBuild = ((BuildTypes)iJ);
							if(eBuild == NO_BUILD)
								continue;

							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if(!pkBuildInfo)
								continue;

							ImprovementTypes eNavalImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
							if(eNavalImprovement == NO_IMPROVEMENT)
								continue;

							if (GC.getImprovementInfo(eNavalImprovement)->IsExpandedImprovementResourceTrade(pLoopPlot->getResourceType()))
							{
								newTarget.SetTargetType(AI_HOMELAND_TARGET_NAVAL_RESOURCE);
								newTarget.SetTargetX(pLoopPlot->getX());
								newTarget.SetTargetY(pLoopPlot->getY());
								newTarget.SetAuxIntData(eBuild);
								m_TargetedNavalResources.push_back(newTarget);
								//once is enough ... apparently multiple builds are valid?
								break;
							}
						}
					}
				}
			}
			// ... unpopped goody hut?
			if(!m_pPlayer->isMinorCiv() && pLoopPlot->isRevealedGoody(m_pPlayer->getTeam()))
			{
				newTarget.SetTargetType(AI_HOMELAND_TARGET_ANCIENT_RUIN);
				newTarget.SetTargetX(pLoopPlot->getX());
				newTarget.SetTargetY(pLoopPlot->getY());
				m_TargetedAncientRuins.push_back(newTarget);
			}
			// ... antiquity site?
			if((pLoopPlot->getResourceType(eTeam) == GC.getARTIFACT_RESOURCE() || pLoopPlot->getResourceType(eTeam) == GC.getHIDDEN_ARTIFACT_RESOURCE()))
			{
				if( pLoopPlot->getOwner() == NO_PLAYER ||
					pLoopPlot->getOwner() == m_pPlayer->GetID() || 
					!m_pPlayer->GetDiplomacyAI()->IsPlayerBadTheftTarget(pLoopPlot->getOwner(), THEFT_TYPE_ARTIFACT, pLoopPlot) )
				{
					newTarget.SetTargetType(AI_HOMELAND_TARGET_ANTIQUITY_SITE);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					m_TargetedAntiquitySites.push_back(newTarget);
				}
			}
			// ... possible sentry point?
			if( !pLoopPlot->isWater() && 
				pLoopPlot->getOwner() == m_pPlayer->GetID() &&
				pLoopPlot->isValidMovePlot(m_pPlayer->GetID()) && 
				pLoopPlot->IsBorderLand(m_pPlayer->GetID()))
			{
				if (pLoopPlot->isRevealedFortification(m_pPlayer->getTeam()))
				{
					int iWeight = 100000 + pLoopPlot->defenseModifier(m_pPlayer->getTeam(),false,false);

					newTarget.SetTargetType(AI_HOMELAND_TARGET_FORT);
					newTarget.SetTargetX(pLoopPlot->getX());
					newTarget.SetTargetY(pLoopPlot->getY());
					newTarget.SetAuxIntData(iWeight);
					m_TargetedSentryPoints.push_back(newTarget);
				}
				else
				{
					//todo: shouldn't we be looking at visibility here?
					int iWeight = pLoopPlot->GetDefenseBuildValue(m_pPlayer->GetID());
					if(iWeight > 0)
					{
						if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
							iWeight += 10;
						if(pLoopPlot->isHills())
							iWeight += 25;

						if(pLoopPlot->getNumUnits() > 0)
						{
							CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
							if(pUnit && pUnit->IsCivilianUnit())
								iWeight += 100;
							}

						newTarget.SetTargetType(AI_HOMELAND_TARGET_SENTRY_POINT);
						newTarget.SetTargetX(pLoopPlot->getX());
						newTarget.SetTargetY(pLoopPlot->getY());
						newTarget.SetAuxIntData(iWeight);
						m_TargetedSentryPoints.push_back(newTarget);
					}
				}
			}
			// ... possible naval sentry point?
			if (pLoopPlot->isWater() && pLoopPlot->isValidMovePlot(m_pPlayer->GetID()))
			{
				CvCity* pOwningCity = pLoopPlot->getOwningCity();
				if (pOwningCity != NULL && pOwningCity->getOwner() == m_pPlayer->GetID() && pOwningCity->isCoastal())
				{
					int iSuspiciousNeighbors = pLoopPlot->GetNumAdjacentDifferentTeam(m_pPlayer->getTeam(), DOMAIN_SEA, true);
					if (iSuspiciousNeighbors > 0)
					{
						int iWeight = m_pPlayer->GetCityDistanceInEstimatedTurns(pLoopPlot) + iSuspiciousNeighbors*2;
						if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
							iWeight += 23;

						if (m_pPlayer->getNumCities() > 1 && pOwningCity->GetCoastalThreatRank() != -1)
						{
							//More cities = more threat.
							int iThreat = (m_pPlayer->getNumCities() - pOwningCity->GetCoastalThreatRank()) * 10;
							if (iThreat > 0)
								iWeight += iThreat;
						}

						if (iWeight > 0)
						{
							newTarget.SetTargetType(AI_HOMELAND_TARGET_SENTRY_POINT_NAVAL);
							newTarget.SetTargetX(pLoopPlot->getX());
							newTarget.SetTargetY(pLoopPlot->getY());
							newTarget.SetAuxIntData(iWeight);
							m_TargetedNavalSentryPoints.push_back(newTarget);
						}
					}
				}
			}
		}
	}

	//we also want to guard our workers!
	int iUnitLoop = 0;
	for (const CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		//a bit redundant, but let's play it safe
		if (pLoopUnit->IsWork() && 
			pLoopUnit->getDomainType() == DOMAIN_LAND && 
			pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER && 
			pLoopUnit->isNativeDomain(pLoopUnit->plot()) &&
			pLoopUnit->plot()->IsAdjacentOwnedByTeamOtherThan(m_pPlayer->getTeam(),true))
		{
			newTarget.SetTargetType(AI_HOMELAND_TARGET_WORKER);
			newTarget.SetTargetX(pLoopUnit->getX());
			newTarget.SetTargetY(pLoopUnit->getY());
			newTarget.SetAuxIntData(123);
			m_TargetedSentryPoints.push_back(newTarget);
		}
	}

	// Post-processing on targets
	EliminateAdjacentSentryPoints();
	EliminateAdjacentNavalSentryPoints();
	std::stable_sort(m_TargetedCities.begin(), m_TargetedCities.end());
}

/// Choose which moves to run and assign units to it
void CvHomelandAI::AssignHomelandMoves()
{
	//most of these functions are very specific, so their order is not so important ...

	//call it twice in case the unit reaches the end of it's sight with moves left
	PlotExplorerMoves(false);
	PlotExplorerMoves(true);

	//call it twice in case the unit reaches the end of it's sight with moves left
	PlotExplorerSeaMoves(false);
	PlotExplorerSeaMoves(true);

	//military only
	PlotUpgradeMoves();
	PlotGarrisonMoves();
	PlotSentryMoves();
	PlotSentryNavalMoves();

	//civilian and military
	PlotHealMoves();
	PlotMovesToSafety();
	PlotAncientRuinMoves();

	//military again
	PlotAircraftRebase();
	PlotPatrolMoves();

	//civilians again
	PlotFirstTurnSettlerMoves();
	PlotWorkerMoves();
	PlotWorkerSeaMoves();
	PlotWriterMoves();
	PlotArtistMoves();
	PlotMusicianMoves();
	PlotScientistMoves();
	PlotEngineerMoves();
	PlotMerchantMoves();
	PlotGeneralMoves();
	PlotAdmiralMoves();
	PlotProphetMoves();
	PlotMissionaryMoves();
	PlotInquisitorMoves();

#if defined(MOD_DIPLOMACY_CITYSTATES)
			//this is for embassies - diplomatic missions are handled via AI operation
	if (MOD_DIPLOMACY_CITYSTATES)
	{
		PlotDiplomatMoves();
		PlotMessengerMoves();
	}
#endif

	PlotSSPartMoves();

	PlotTreasureMoves();
	PlotTradeUnitMoves();
	PlotArchaeologistMoves();

#if defined(MOD_AI_SECONDARY_SETTLERS)
	PlotOpportunisticSettlementMoves();
	PlotWorkerMoves(true);
	PlotWorkerSeaMoves(true);
#endif

	ReviewUnassignedUnits();
}

/// Get units with explore AI and plan their moves
void CvHomelandAI::PlotExplorerMoves(bool bSecondPass)
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_EXPLORE);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE ||
				(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->GetAutomateType() == AUTOMATE_EXPLORE))
			{
				//this is stupid but we need extra code for scout healing 
				if (pUnit->shouldHeal())
				{
					CvPlot* pPlot = TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
					if (!pPlot)
						pPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);

					if (pPlot)
					{
						ExecuteMoveToTarget(pUnit, pPlot, 0, true);
						if (pUnit->canMove() && pUnit->shouldPillage(pUnit->plot()))
							pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
						continue;
					}
				}

				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteExplorerMoves();
	}

	if(bSecondPass)
	{
		for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
			if (pUnit && pUnit->GetDanger()==0) //don't freeze in danger in case pathfinding failed ...
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				UnitProcessed(pUnit->GetID());
			}
		}
	}
}

/// Get units with explore AI and plan their moves
void CvHomelandAI::PlotExplorerSeaMoves(bool bSecondPass)
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_EXPLORE_SEA);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit)
		{
			if(pUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA ||
				(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_EXPLORE))
			{
				//this is stupid but we need extra code for scout healing 
				if (pUnit->shouldHeal())
				{
					CvPlot* pPlot = TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
					if (pPlot)
					{
						ExecuteMoveToTarget(pUnit, pPlot, 0, true);
						if (pUnit->canMove() && pUnit->shouldPillage(pUnit->plot()))
							pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
						continue;
					}
				}

				CvHomelandUnit unit;
				unit.SetID(pUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
			}
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteExplorerMoves();
	}

	if(bSecondPass)
	{
		for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
			if (pUnit)
			{
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
				UnitProcessed(pUnit->GetID());
			}
		}
	}
}

/// Get our first city built
void CvHomelandAI::PlotFirstTurnSettlerMoves()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_SETTLE);

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
void CvHomelandAI::PlotGarrisonMoves()
{
	// Grab units that make sense for this move type
	FindUnitsForThisMove(AI_HOMELAND_MOVE_GARRISON);

	for(unsigned int iI = 0; iI < m_TargetedCities.size(); iI++)
	{
		CvPlot* pTarget = GC.getMap().plot(m_TargetedCities[iI].GetTargetX(), m_TargetedCities[iI].GetTargetY());
		CvCity* pCity = pTarget->getPlotCity();
		if (!pCity)
			continue;

		if (pCity->HasGarrison())
		{
			//nothing to do really
			CvUnit* pGarrison = pCity->GetGarrisonedUnit();
			if (!pGarrison->TurnProcessed())
				UnitProcessed(pGarrison->GetID());
		}
		else
		{
			//try to find a new garrison
			CvUnit *pGarrison = GetBestUnitToReachTarget(pTarget, GC.getAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS());
			if(pGarrison)
			{
				ExecuteMoveToTarget(pGarrison, pTarget, 0);
				UnitProcessed(pGarrison->GetID());

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


/// Find out which units would like to heal
void CvHomelandAI::PlotHealMoves()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_HEAL);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		//this is very simple, we know there are no enemies around, else tactical AI would have kicked in
		if(pUnit && !pUnit->isHuman() && pUnit->IsHurt())
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

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteHeals();
	}
}

/// Moved endangered units to safe hexes
void CvHomelandAI::PlotMovesToSafety()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_TO_SAFETY);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (!pUnit)
			continue;

		int iDangerLevel = pUnit->GetDanger();
		if (iDangerLevel == 0)
			continue;

		// civilian always ready to flee
		if (pUnit->IsCivilianUnit())
		{
			//allow workers to clean fallout (at home)
			if (pUnit->plot()->getOwner() == pUnit->getOwner() &&
				pUnit->plot()->getFeatureType() == FEATURE_FALLOUT && 
				iDangerLevel < pUnit->GetCurrHitPoints())
				continue;
		}
		else
		{
			//land barbarians don't flee
			if (pUnit->isBarbarian() && pUnit->getDomainType() == DOMAIN_LAND)
				continue;
		}

			CvHomelandUnit unit;
			unit.SetID(pUnit->GetID());
			m_CurrentMoveUnits.push_back(unit);
		}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteMovesToSafestPlot();
	}
}

/// Send units to sentry points around borders
void CvHomelandAI::PlotSentryMoves()
{
	FindUnitsForThisMove(AI_HOMELAND_MOVE_SENTRY);

	// Do we have any targets of this type?
	for(unsigned int iI = 0; iI < m_TargetedSentryPoints.size(); iI++)
	{
		AI_PERF_FORMAT("Homeland-perf.csv", ("PlotSentryMoves, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

		CvPlot* pTarget = GC.getMap().plot(m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY());

		if(m_CurrentMoveUnits.size() > 0)
		{
			CvUnit *pSentry = GetBestUnitToReachTarget(pTarget, 6);
			if (!pSentry)
				continue;

			if (pSentry->atPlot(*pTarget))
			{
				//check our immediate neighbors if we can increase our visibility
				int iBestCount = 0;
				CvPlot* pBestNeighbor = NULL;
				for (int i = RING0_PLOTS; i < RING1_PLOTS; i++)
				{
					CvPlot* pNeighbor = iterateRingPlots(pSentry->plot(), i);
					if (!pNeighbor || !pSentry->canMoveInto(*pNeighbor,CvUnit::MOVEFLAG_DESTINATION))
						continue;

					int iCount = TacticalAIHelpers::CountAdditionallyVisiblePlots(pSentry, pNeighbor);
					if (iCount > iBestCount)
					{
						iBestCount = iCount;
						pBestNeighbor = pNeighbor;
					}
				}

				if (pBestNeighbor)
					pSentry->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestNeighbor->getX(), pBestNeighbor->getY());
			}

			ExecuteMoveToTarget(pSentry, pTarget, 0, true);
			UnitProcessed(pSentry->GetID());

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Moving %s %d to sentry point, X: %d, Y: %d, Priority: %d", pSentry->getName().c_str(), pSentry->GetID(), m_TargetedSentryPoints[iI].GetTargetX(), m_TargetedSentryPoints[iI].GetTargetY(), m_TargetedSentryPoints[iI].GetAuxIntData());
				LogHomelandMessage(strLogString);
			}
		}
	}
}

#if defined(MOD_BALANCE_CORE)
void CvHomelandAI::PlotSentryNavalMoves()
{
	FindUnitsForThisMove(AI_HOMELAND_MOVE_SENTRY_NAVAL);

	// Do we have any targets of this type?
	if(!m_TargetedNavalSentryPoints.empty())
	{
		// See how many moves of this type we can execute
		for(unsigned int iI = 0; iI < m_TargetedNavalSentryPoints.size(); iI++)
		{
			AI_PERF_FORMAT("Homeland-perf.csv", ("PlotNavalSentryMoves, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

			CvPlot* pTarget = GC.getMap().plot(m_TargetedNavalSentryPoints[iI].GetTargetX(), m_TargetedNavalSentryPoints[iI].GetTargetY());
			if(m_CurrentMoveUnits.size() > 0)
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
						UnitProcessed(pSentry->GetID());

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("%s %d AT NAVAL sentry point, chillin, X: %d, Y: %d, Priority: %d", pSentry->getName().c_str(), pSentry->GetID(), m_TargetedNavalSentryPoints[iI].GetTargetX(), m_TargetedNavalSentryPoints[iI].GetTargetY(), m_TargetedNavalSentryPoints[iI].GetAuxIntData());
							LogHomelandMessage(strLogString);
						}
					}
					else
					{
						ExecuteMoveToTarget(pSentry, pTarget, 0, true);
						UnitProcessed(pSentry->GetID());

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

	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_SECONDARY_SETTLER);
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
	CHomelandUnitArray PossibleSettlerUnits;
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it) 
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit && pUnit->IsCombatUnit() && (pUnit->isFound() || pUnit->IsFoundAbroad() || pUnit->IsFoundLate() || pUnit->IsFoundMid()))
		{
			//fake this, the distance check is irrelevant here
			ReachablePlots turnsFromMuster;
			turnsFromMuster.insert( SMovePlot(pUnit->plot()->GetPlotIndex()) );

			if(OperationalAIHelpers::IsUnitSuitableForRecruitment(pUnit,pUnit->plot(),turnsFromMuster,pUnit->plot(),false,false))
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
		CHomelandUnitArray::iterator settlerUnitIt;
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

	ClearCurrentMoveUnits(bSecondary ? AI_HOMELAND_MOVE_SECONDARY_WORKER : AI_HOMELAND_MOVE_WORKER);

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
					
#if defined(MOD_BALANCE_CORE)
						if (pkBuild && !pkBuild->IsWater() && (pUnit->getUnitInfo().GetBuilds((BuildTypes)i) && !GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsNoBuild((BuildTypes)i) || m_pPlayer->GetPlayerTraits()->HasUnitClassCanBuild(i, pUnit->getUnitInfo().GetUnitClassType())))
#else
						if (pkBuild && !pkBuild->IsWater() && (pUnit->getUnitInfo().GetBuilds((BuildTypes)i) || m_pPlayer->GetPlayerTraits()->HasUnitClassCanBuild(i, pUnit->getUnitInfo().GetUnitClassType())))
#endif
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
		ExecuteWorkerMoves();
	}
}

/// Send out work boats to harvest resources
#if defined(MOD_BALANCE_CORE)
void CvHomelandAI::PlotWorkerSeaMoves(bool bSecondary)
#else
void CvHomelandAI::PlotWorkerSeaMoves()
#endif
{
	ClearCurrentMoveUnits(bSecondary ? AI_HOMELAND_MOVE_SECONDARY_WORKER : AI_HOMELAND_MOVE_WORKER_SEA);

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
					
#if defined(MOD_BALANCE_CORE)
						if (pkBuild && pkBuild->IsWater() && (pUnit->getUnitInfo().GetBuilds((BuildTypes)i) && !GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsNoBuild((BuildTypes)i) || m_pPlayer->GetPlayerTraits()->HasUnitClassCanBuild(i, pUnit->getUnitInfo().GetUnitClassType())))
#else
						if (pkBuild && pkBuild->IsWater() && (pUnit->getUnitInfo().GetBuilds((BuildTypes)i) || m_pPlayer->GetPlayerTraits()->HasUnitClassCanBuild(i, pUnit->getUnitInfo().GetUnitClassType())))
#endif
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

	for(CHomelandUnitArray::iterator moveUnitIt = m_CurrentMoveUnits.begin(); moveUnitIt != m_CurrentMoveUnits.end(); ++moveUnitIt)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
		int iTargetIndex = -1;
		int iTargetMoves = MAX_INT;

		// See how many moves of this type we can execute
		for (unsigned int iI = 0; iI < m_TargetedNavalResources.size(); iI++)
		{
			// See what units we have who can reach target this turn
			CvPlot* pTarget = GC.getMap().plot(m_TargetedNavalResources[iI].GetTargetX(), m_TargetedNavalResources[iI].GetTargetY());

			//boat may be in city so water area is only adjacent
			if (!pTarget->shareAdjacentArea(pUnit->plot()))
				continue;

#if defined(MOD_AI_SECONDARY_WORKERS)
			if(MOD_AI_SECONDARY_WORKERS && bSecondary)
			{
				if(pUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA ||
					(pUnit->IsAutomated() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->GetAutomateType() == AUTOMATE_BUILD))
				{
					if (!pUnit->canBuild(pTarget, (BuildTypes)m_TargetedNavalResources[iI].GetAuxIntData()))
						continue;

					if (pUnit->GetDanger(pTarget)>20)
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

				if (pUnit->GetDanger(pTarget)>20)
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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_PATROL);

	// Loop through all remaining units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && pUnit->IsCombatUnit() && pUnit->getDomainType() != DOMAIN_AIR && !pUnit->IsGarrisoned() && pUnit->AI_getUnitAIType() != UNITAI_CITY_BOMBARD)
		{
			CvHomelandUnit unit;
			unit.SetID(pUnit->GetID());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
		ExecutePatrolMoves();
}

void CvHomelandAI::ExecutePatrolMoves()
{
	//check what kind of units we have
	int iUnitsSea = 0, iUnitsLand = 0;
	for(CHomelandUnitArray::iterator itUnit = m_CurrentMoveUnits.begin(); itUnit != m_CurrentMoveUnits.end(); ++itUnit)
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
	
	//get the most exposed cities and the most threatening neighbor cities
	vector<SPatrolTarget> vLandTargets, vWaterTargets;
	if (iUnitsLand>0)
		vLandTargets = HomelandAIHelpers::GetPatrolTargets(m_pPlayer->GetID(), false, iUnitsLand);
	if (iUnitsSea>0)
		vWaterTargets = HomelandAIHelpers::GetPatrolTargets(m_pPlayer->GetID(), true, iUnitsSea);

	int iUnitMoveRange = m_pPlayer->isMinorCiv() ? 5 : 9; //determines how far a unit can move for a patrol
	SPathFinderUserData data(m_pPlayer->GetID(),PT_GENERIC_REACHABLE_PLOTS,-1,iUnitMoveRange);
	std::map<CvPlot*,ReachablePlots> mapReachablePlots;
	for (size_t i=0; i<vLandTargets.size(); i++)
		mapReachablePlots[vLandTargets[i].pTarget] = GC.GetStepFinder().GetPlotsInReach(vLandTargets[i].pTarget,data);
	for (size_t i=0; i<vWaterTargets.size(); i++)
		//the stepfinder works for both land and water, so do the work only if necessary
		if (mapReachablePlots.find(vWaterTargets[i].pTarget)==mapReachablePlots.end())
				mapReachablePlots[vWaterTargets[i].pTarget] = GC.GetStepFinder().GetPlotsInReach(vWaterTargets[i].pTarget,data);

	//for each unit, check which city is closest
	for(CHomelandUnitArray::iterator itUnit = m_CurrentMoveUnits.begin(); itUnit != m_CurrentMoveUnits.end(); ++itUnit)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(itUnit->GetID());
		if(!pUnit || pUnit->IsCivilianUnit() || pUnit->getDomainType()==DOMAIN_AIR)
			continue;

		//the target we're looking at depends on the domain of the unit
		vector<SPatrolTarget>& vTargets = (pUnit->getDomainType()==DOMAIN_SEA) ? vWaterTargets : vLandTargets;
		CvTacticalDominanceZone* pCurrentZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pUnit->plot());

		int iHighestThreat = 0;
		int iCurrentDistance = INT_MAX;
		CvPlot* pBestCity = NULL;
		CvPlot* pWorstEnemy = NULL;
		for (size_t i=0; i<vTargets.size(); i++)
		{
			//see if there's a target in range
			ReachablePlots::const_iterator itPlot = mapReachablePlots[vTargets[i].pTarget].find(pUnit->plot()->GetPlotIndex());
			if (itPlot!=mapReachablePlots[vTargets[i].pTarget].end())
			{
				//stay in the current zone if it's one of the targets
				CvTacticalDominanceZone* pTargetZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(vTargets[i].pTarget);
				if (pTargetZone == pCurrentZone)
				{
					pBestCity = vTargets[i].pTarget;
					pWorstEnemy = vTargets[i].pWorstEnemy;
					break;
				}

				//otherwise move to the zone with the highest threat score, using distance as tiebreaker
				if (vTargets[i].iThreatLevel > iHighestThreat || (vTargets[i].iThreatLevel == iHighestThreat  && itPlot->iNormalizedDistanceRaw < iCurrentDistance))
				{
					pBestCity = vTargets[i].pTarget;
					pWorstEnemy = vTargets[i].pWorstEnemy;
					iHighestThreat = vTargets[i].iThreatLevel;
					iCurrentDistance = itPlot->iNormalizedDistanceRaw;
				}
			}
		}

		CvPlot* pBestPlot = NULL;
		if (pBestCity)
		{
			//try not to create a unit carpet without any space to move
			int iMaxDistance = pUnit->isRanged() ? 3 : 4;
			for (int iJ = RING1_PLOTS; iJ < RING_PLOTS[iMaxDistance]; iJ++)
			{
				CvPlot* pLoopPlot = iterateRingPlots(pBestCity, iJ);
				if(pLoopPlot == NULL)
					continue;

				//must be to the right side of our city if we know where the threat comes from
				if (pWorstEnemy && plotDistance(*pLoopPlot, *pWorstEnemy) > plotDistance(*pBestCity, *pWorstEnemy))
					continue;

				//avoid confrontations with other players, dead ends etc
				if (!TacticalAIHelpers::IsGoodPlotForStaging(m_pPlayer, pLoopPlot, pUnit->getDomainType()))
					continue;

				//naturally
				if (!pUnit->canMoveInto(*pLoopPlot, CvUnit::MOVEFLAG_DESTINATION))
					continue;

				//if we get here it's good enough
				pBestPlot = pLoopPlot;
				break;
			}
		}

		if(pBestPlot)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp;

				strTemp = pUnit->getUnitInfo().GetDescription();
				strLogString.Format("%s (%d) patrolling to, X: %d, Y: %d, Current X: %d, Current Y: %d", strTemp.GetCString(), pUnit->GetID(), pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}

			if (pBestPlot == pUnit->plot() && !pUnit->IsHurt())
			{
				//check our immediate neighbors if we can increase our visibility
				int iBestCount = 0;
				CvPlot* pBestNeighbor = NULL;
				for (int i = RING0_PLOTS; i < RING1_PLOTS; i++)
				{
					CvPlot* pNeighbor = iterateRingPlots(pUnit->plot(), i);
					if (!pNeighbor || !pUnit->canMoveInto(*pNeighbor,CvUnit::MOVEFLAG_DESTINATION) || !pUnit->isNativeDomain(pNeighbor))
						continue;

					int iCount = TacticalAIHelpers::CountAdditionallyVisiblePlots(pUnit, pNeighbor);
					if (iCount > iBestCount)
					{
						iBestCount = iCount;
						pBestNeighbor = pNeighbor;
					}
				}

				if (pBestNeighbor)
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestNeighbor->getX(), pBestNeighbor->getY());
			}

			if (pUnit->canMove() && pUnit->plot() != pBestPlot)
				//use the exact target location - GetPatrolTarget makes sure there is a free spot
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());

			if (pUnit->canMove())
				pUnit->PushMission(CvTypes::getMISSION_SKIP());

			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Find units that we can upgrade
void CvHomelandAI::PlotUpgradeMoves()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_UPGRADE);
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		// Don't try and upgrade a human player's unit or one already recruited for an operation
		CvUnit* pUnit = m_pPlayer->getUnit(*it);

		if(pUnit && !pUnit->isHuman() && pUnit->getArmyID() == -1)
		{
			//Let's only worry about units in our land.
			if(pUnit->plot()->getOwner() != m_pPlayer->GetID())
				continue;

			//And not embarked.
			if(!pUnit->isNativeDomain(pUnit->plot()))
				continue;

			// Can this unit be upgraded?
			UnitTypes eUpgradeUnitType = pUnit->GetUpgradeUnitType();
			if(eUpgradeUnitType != NO_UNIT)
			{
				// Tech requirement
				TechTypes ePrereqTech = (TechTypes) GC.getUnitInfo(eUpgradeUnitType)->GetPrereqAndTech();
				if(ePrereqTech == NO_TECH || GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
				{
					// Resource requirement
					bool bMissingResource = false;
					if (!m_pPlayer->isMinorCiv() && !m_pPlayer->isBarbarian())
					{
						for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bMissingResource; iResourceLoop++)
						{
							ResourceTypes eResource = (ResourceTypes)iResourceLoop;
							int iNumResource = GC.getUnitInfo(eUpgradeUnitType)->GetResourceQuantityRequirement(eResource);
							if (iNumResource > 0)
							{
#if defined(MOD_BALANCE_CORE)
								//Don't use all of our Aluminum, keep some for spaceship parts
								ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
								if (eResource == eAluminumResource)
								{
									iNumResource += 5;
								}
#endif
								int iNumResourceInUnit = pUnit->getUnitInfo().GetResourceQuantityRequirement(eResource);
								if (m_pPlayer->getNumResourceAvailable(eResource) + iNumResourceInUnit < iNumResource)
								{
									bMissingResource = true;
								}
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
								if (MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
								{
									int iResourceTotal = GC.getUnitInfo(eUpgradeUnitType)->GetResourceQuantityTotal(eResource);
									if (m_pPlayer->getNumResourceTotal(eResource) < iResourceTotal || m_pPlayer->getNumResourceAvailable(eResource) + iNumResourceInUnit < 0)
									{
										bMissingResource = true;
									}
								}
#endif
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

						//Extra emphasis on air/sea units
						if (pUnit->getDomainType() != DOMAIN_LAND)
							iPriority *= 2;

						//Add in experience - we should promote veterans.
#if defined(MOD_UNITS_XP_TIMES_100)
						iPriority += pUnit->getExperienceTimes100();
#else
						iPriority += pUnit->getExperience();
#endif

						unit.SetAuxIntData(max(1, iPriority));
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

		CvUnit* pFirstNonUpgradedUnit = NULL;
		// Try to find a unit that can upgrade immediately
		for(CHomelandUnitArray::iterator moveUnitIt = m_CurrentMoveUnits.begin(); moveUnitIt != m_CurrentMoveUnits.end(); ++moveUnitIt)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(moveUnitIt->GetID());
			if(pUnit->CanUpgradeRightNow(false))
			{
				CvUnit* pNewUnit = pUnit->DoUpgrade();
				UnitProcessed(pUnit->GetID());
				if (pNewUnit)
				{
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
				}
			}
			else if (pFirstNonUpgradedUnit == NULL)
				pFirstNonUpgradedUnit = pUnit;
		}

		//return if there is nothing left to do
		if (pFirstNonUpgradedUnit == NULL)
			return;

		// Couldn't do all upgrades this turn, get ready for highest priority unit to upgrade
		int iAmountRequired = pFirstNonUpgradedUnit->upgradePrice(pFirstNonUpgradedUnit->GetUpgradeUnitType());

		// Find priority of this financial request
		int iCurrentFlavorMilitaryTraining = 0;
		for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && iCurrentFlavorMilitaryTraining == 0; iFlavorLoop++)
		{
			if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_MILITARY_TRAINING")
			{
				iCurrentFlavorMilitaryTraining = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)iFlavorLoop);
			}
			if(m_pPlayer->IsAtWar())
			{
				iCurrentFlavorMilitaryTraining *= 50;
			}
		}
		int iGoldPriority = GC.getAI_GOLD_PRIORITY_UPGRADE_BASE();
		iGoldPriority += GC.getAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT() * iCurrentFlavorMilitaryTraining;

		m_pPlayer->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE);
		m_pPlayer->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_UNIT_UPGRADE, iAmountRequired, iGoldPriority);

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			CvString strTemp;

			strTemp = pFirstNonUpgradedUnit->getUnitInfo().GetDescription();
			strLogString.Format("Need gold for %s upgrade, GOLD: Available = %d, Needed = %d, Priority = %d",
				                strTemp.GetCString(), m_pPlayer->GetTreasury()->GetGold(), iAmountRequired, iGoldPriority);
			LogHomelandMessage(strLogString);
		}
	}

	return;
}

/// Pop goody huts nearby
void CvHomelandAI::PlotAncientRuinMoves()
{
	FindUnitsForThisMove(AI_HOMELAND_MOVE_ANCIENT_RUINS);

	// Do we have any targets of this type?
	if(!m_TargetedAncientRuins.empty())
	{
		// See how many moves of this type we can execute
		for(unsigned int iI = 0; iI < m_TargetedAncientRuins.size(); iI++)
		{
			CvPlot* pTarget = GC.getMap().plot(m_TargetedAncientRuins[iI].GetTargetX(), m_TargetedAncientRuins[iI].GetTargetY());
				CvUnit *pIndy = GetBestUnitToReachTarget(pTarget, GC.getAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS());
				if(pIndy)
				{
					ExecuteMoveToTarget(pIndy, pTarget, CvUnit::MOVEFLAG_IGNORE_DANGER);
					if (pIndy->canMove())
						pIndy->PushMission(CvTypes::getMISSION_SKIP());

					UnitProcessed(pIndy->GetID());

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

/// Find moves for great writers
void CvHomelandAI::PlotWriterMoves()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_WRITER);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_ARTIST_GOLDEN_AGE);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_MUSICIAN);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_SCIENTIST_FREE_TECH);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_ENGINEER_HURRY);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_DIPLOMAT_EMBASSY);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_MESSENGER);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_MERCHANT_TRADE);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_PROPHET_RELIGION);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_MISSIONARY);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_INQUISITOR);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_SPACESHIP_PART);

	// Loop through all recruited units
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if(pUnit && pUnit->AI_getUnitAIType() == UNITAI_SPACESHIP_PART)
		{
			CvHomelandUnit unit;
			unit.SetID(pUnit->GetID());
			m_CurrentMoveUnits.push_back(unit);
		}
	}

	if(m_CurrentMoveUnits.size() > 0)
	{
		ExecuteSSPartMoves();
	}
}

/// Find moves for treasures
void CvHomelandAI::PlotTreasureMoves()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_TREASURE);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_GENERAL_GARRISON);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_ADMIRAL_GARRISON);

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
void CvHomelandAI::PlotAircraftRebase()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_AIRCRAFT_REBASE);

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
		ExecuteAircraftMoves();
	}
}

/// Send trade units on their way
void CvHomelandAI::PlotTradeUnitMoves()
{
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_TRADE_UNIT);

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
	ClearCurrentMoveUnits(AI_HOMELAND_MOVE_ARCHAEOLOGIST);

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

/// Log that we couldn't find assignments for some units - this is the catchall, last-resort handler that makes sure we can end the turn
void CvHomelandAI::ReviewUnassignedUnits()
{
	// Loop through all remaining units.
	for (list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(*it);
		if (pUnit)
		{
			// Do not call UnitProcessed() from inside this loop, it can invalidate the iterator!
			// Therefore we set the last move manually
			pUnit->setHomelandMove(AI_HOMELAND_MOVE_UNASSIGNED);

			//safety check in case we have a unit with a queued mission
			if (pUnit->GetLengthMissionQueue() > 0)
			{
				//unit can still move? then there is a problem with the mission, better delete it!
				if (pUnit->canMove())
					pUnit->ClearMissionQueue();
				else
				{
					pUnit->SetTurnProcessed(true);
					continue;
				}
			}

			if (pUnit->getDomainType() == DOMAIN_LAND)
			{
				if (pUnit->getMoves() > 0)
				{
					CvCity* pClosestCity = m_pPlayer->GetClosestCityByEstimatedTurns(pUnit->plot());
					if (pClosestCity && pUnit->GeneratePath(pClosestCity->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN, 23))
						ExecuteMoveToTarget(pUnit, pClosestCity->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN);
					else if (m_pPlayer->getCapitalCity() && pUnit->GeneratePath(m_pPlayer->getCapitalCity()->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN, 23) )
						ExecuteMoveToTarget(pUnit, m_pPlayer->getCapitalCity()->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN);
					else
						pUnit->PushMission(CvTypes::getMISSION_SKIP());
				}

				pUnit->SetTurnProcessed(true);
			}
			else if(pUnit->getDomainType() == DOMAIN_SEA)
			{
				if(pUnit->plot()->getOwner() == pUnit->getOwner())
				{
					pUnit->SetTurnProcessed(true);

					CvString strTemp;
					CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pUnit->getUnitType());
					if(pkUnitInfo)
					{
						strTemp = pkUnitInfo->GetDescription();
						CvString strLogString;
						strLogString.Format("Unassigned %s %d at home, at X: %d, Y: %d", strTemp.GetCString(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
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
							if (pPlot->getArea() != pUnit->getArea())
								continue;

							int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pUnit->getX(), pUnit->getY());
							if (iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								pBestPlot = pPlot;
							}
						}
					}

					if (pBestPlot != NULL)
					{
						if (MoveToTargetButDontEndTurn(pUnit, pBestPlot, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING2))
						{
							pUnit->SetTurnProcessed(true);

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
					//Stuck and not at home? Scrap it.
					if (GC.getGame().getGameTurn() - pUnit->getLastMoveTurn() > 7)
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

						if (pUnit->canScrap())
							pUnit->scrap();
						else
							pUnit->kill(true);
					}
				}
			}
		}
	}
}

/// Creates cities for AI civs on first turn
void CvHomelandAI::ExecuteFirstTurnSettlerMoves()
{
	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(pUnit)
		{
#if defined(MOD_BALANCE_CORE_SETTLER_MOVE)
			if( MOD_BALANCE_CORE_SETTLER_MOVE && GC.getSETTLER_MOVE_ON_START() > 0 && (m_pPlayer->isMajorCiv() || !pUnit->canFound(pUnit->plot())))
			{
				int iInitialPlotValue = 0;
				int iAdjacentValue = 0;
				CvPlot* pBestAdjacentPlot = NULL;
				//Let's check for a river estuary - those are always good
				if(pUnit->plot()->isFreshWater() && pUnit->plot()->isCoastalLand() && pUnit->canFound(pUnit->plot()))
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

					if (GC.getGame().getElapsedGameTurns()<3 || iInitialPlotValue==0) //first two turns or we're in a bad spot
					{
						for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iI));
							if(pAdjacentPlot != NULL && pUnit->canFound(pAdjacentPlot))
							{
								iAdjacentValue = pAdjacentPlot->getFoundValue(m_pPlayer->GetID());
								if(iAdjacentValue > iInitialPlotValue*1.1f) //should be at least ten percent better to justify the hassle
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
void CvHomelandAI::ExecuteExplorerMoves()
{
	bool bFoundNearbyExplorePlot = false;
	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit || !pUnit->canMove())
			continue;

		//should be the same everywhere so we can reuse paths
		int iMoveFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE | CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER;

		//performance: if we have a leftover path to a far-away (expensive) target an it's still good, then reuse it!
		if ( pUnit->GetMissionAIType()==MISSIONAI_EXPLORE && pUnit->GetMissionAIPlot() && plotDistance(*pUnit->plot(),*pUnit->GetMissionAIPlot())>10 )
		{
			CvPlot* pDestPlot = pUnit->GetMissionAIPlot();
			const std::vector<SPlotWithScore>& vExplorePlots = m_pPlayer->GetEconomicAI()->GetExplorationPlots(pUnit->getDomainType());

			SPlotWithScore dummy(pDestPlot,0);
			if ( std::find( vExplorePlots.begin(),vExplorePlots.end(),dummy ) != vExplorePlots.end() )
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pDestPlot->getX(), pDestPlot->getY(), 
					iMoveFlags, false, false, MISSIONAI_EXPLORE, pDestPlot);

				if (!pUnit->canMove())
				{
					UnitProcessed(pUnit->GetID());
					continue;
				}
			}
		}

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(pUnit->getOwner());
			args->Push(pUnit->GetID());

			bool bResult = false;
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

		CvPlot* pBestPlot = NULL;
		int iBestPlotScore = 0;
		
		//first check our immediate neighborhood (ie the tiles we can reach within one turn)
		//if the scout is already embarked, we need to allow it so we don't get stuck!
		int iMoveFlagsLocal = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY;
		if (!pUnit->isEmbarked())
			iMoveFlagsLocal |= CvUnit::MOVEFLAG_NO_EMBARK;

		ReachablePlots eligiblePlots = TacticalAIHelpers::GetAllPlotsInReachThisTurn(pUnit, pUnit->plot(), iMoveFlagsLocal);
		for (ReachablePlots::iterator tile=eligiblePlots.begin(); tile!=eligiblePlots.end(); ++tile)
		{
			CvPlot* pEvalPlot = GC.getMap().plotByIndexUnchecked(tile->iPlotIndex);

			if(!pEvalPlot)
				continue;

			//we can pass through a minor's territory but we don't want to stay there
			//this check shouldn't be necessary because of IsValidExplorerEndTurnPlot() but sometimes it is
			if(pEvalPlot->isOwned() && GET_PLAYER(pEvalPlot->getOwner()).isMinorCiv())
				continue;

			//don't embark to reach a close-range target
			if(pEvalPlot->needsEmbarkation(pUnit))
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

			//is there a lone civilian we can capture back (or kill if embarked)
			if (pEvalPlot->isEnemyUnit(pUnit->getOwner(), false, true, false) &&
				!pEvalPlot->isEnemyUnit(pUnit->getOwner(), true, true, false) &&
				pUnit->canMoveInto(*pEvalPlot,CvUnit::MOVEFLAG_ATTACK) &&
				pUnit->GetDanger(pEvalPlot) < pUnit->GetCurrHitPoints())
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pEvalPlot->getX(), pEvalPlot->getY());
				//we just moved, eligiblePlots are no longer valid
				break;
			}

			//do this after the enemy unit check
			if (!IsValidExplorerEndTurnPlot(pUnit, pEvalPlot))
				continue;

			//get contributions from yet-to-be revealed plots (and goodies)
			int iScoreBase = EconomicAIHelpers::ScoreExplorePlot2(pEvalPlot, m_pPlayer, pUnit->getDomainType(), pUnit->isEmbarked());
			if(iScoreBase > 0)
			{
				int iScoreBonus = pEvalPlot->GetExplorationBonus(m_pPlayer, pUnit);
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
				int iDanger = pUnit->GetDanger(pEvalPlot);
				if(iDanger > iAcceptableDanger)
					continue;
				if(iDanger > iAcceptableDanger/2)
					iTotalScore /= 2;

				if(iTotalScore > iBestPlotScore)
				{
					//make sure we can actually reach it - shouldn't happen, but sometimes does because of blocks
					if (pUnit->TurnsToReachTarget(pEvalPlot,false,false,1)>1)
						continue;

					pBestPlot = pEvalPlot;
					iBestPlotScore = iTotalScore;
					bFoundNearbyExplorePlot = true;
				}
			}
		}

		//if we made an opportunity attack, we're done
		if(!pUnit->canMove())
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
				//this  must be the same moveflags as above so we can reuse the path next turn
				if (pUnit->GeneratePath(pBestPlot, iMoveFlags, INT_MAX, NULL, true))
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
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				CvString strTemp = pUnit->getUnitInfo().GetDescription();
				strLogString.Format("%s Explored to %s target, To X: %d, Y: %d, From X: %d, Y: %d",
					strTemp.GetCString(), bFoundNearbyExplorePlot ? "nearby" : "distant", pBestPlot->getX(), pBestPlot->getY(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}

			//again same flags
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY(), iMoveFlags,	false, false, MISSIONAI_EXPLORE, pBestPlot);

			if (pUnit->canMove() && pUnit->IsGainsXPFromPillaging() && pUnit->canPillage(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_PILLAGE());
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					CvString strTemp = pUnit->getUnitInfo().GetDescription();
					strLogString.Format("%s pillaged a plot because we get XP, at X: %d, Y: %d", strTemp.GetCString(), pUnit->getX(), pUnit->getY());
					LogHomelandMessage(strLogString);
				}
			}

			// Only mark as done if out of movement - we'll do a second pass later
			if (!pUnit->canMove())
				UnitProcessed(pUnit->GetID());
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
	}
}

/// Moves units to improve plots
void CvHomelandAI::ExecuteWorkerMoves()
{
	// where can our workers go
	std::map<CvUnit*,ReachablePlots> allWorkersReachablePlots;

	//see what each worker can do in its immediate vicinity
	//if there is no work there, it will move towards the city which needs a worker most
	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
			continue;

		SPathFinderUserData data(pUnit, 0, 5);
		data.ePathType = PT_UNIT_REACHABLE_PLOTS;
		allWorkersReachablePlots[pUnit] = GC.GetPathFinder().GetPlotsInReach(pUnit->plot(), data);
	}

	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
			continue;

		//fallout also counts as danger, so set the threshold a bit higher than zero
		if(pUnit->GetDanger()>pUnit->GetCurrHitPoints()/2)
		{
			if(MoveCivilianToSafety(pUnit))
			{
				UnitProcessed(pUnit->GetID());
				continue;
			}
		}

		//how far around each worker should we be checking?
		int iTurnLimit = pUnit->IsGreatPerson() ? 12 : 5;

#if defined(MOD_AI_SECONDARY_WORKERS)
		if (MOD_AI_SECONDARY_WORKERS && pUnit->IsCombatUnit())
			iTurnLimit = gCustomMods.getOption("UNITS_LOCAL_WORKERS_COMBATLIMIT", 2);
#endif

		//is the unit still busy? if so, less time for movement
		int iBuildTimeLeft = 0;
		BuildTypes eBuild = pUnit->getBuildType();
		if (eBuild != NO_BUILD)
			iBuildTimeLeft = pUnit->plot()->getBuildTurnsLeft(eBuild, pUnit->getOwner(), 0, 0);

		if (iTurnLimit >= iBuildTimeLeft)
		{
			SPathFinderUserData data(pUnit, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY, iTurnLimit-iBuildTimeLeft);
			data.ePathType = PT_UNIT_REACHABLE_PLOTS;
			ReachablePlots plots = GC.GetPathFinder().GetPlotsInReach(pUnit->getX(), pUnit->getY(), data);
	
			// add offset for fair comparison
			for (ReachablePlots::iterator it2 = plots.begin(); it2 != plots.end(); ++it2)
				it2->iTurns += iBuildTimeLeft;
			
			allWorkersReachablePlots[pUnit] = plots;
		}
	}

	//may need this later
	map<CvCity*, int> mapCityNeed;
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		mapCityNeed[pLoopCity] = pLoopCity->GetTerrainImprovementNeed();

	//see if we have work to do
	for(std::map<CvUnit*,ReachablePlots>::iterator it = allWorkersReachablePlots.begin(); it != allWorkersReachablePlots.end(); ++it)
	{
		CvUnit* pUnit = it->first;
		//this checks for work in the immediate neighborhood of the workers
		CvPlot* pTarget = ExecuteWorkerMove(pUnit, allWorkersReachablePlots);
		if (pTarget)
		{
			//make sure no other worker tries to target the same plot
			it->second.clear();
			it->second.insert( SMovePlot(pTarget->GetPlotIndex(),-1,0,0) );
		}
		else
		{
			//find the city which is most in need of workers
			int iMaxNeed = -100;
			CvCity* pBestCity = NULL;
			for (map<CvCity*, int>::iterator it2 = mapCityNeed.begin(); it2 != mapCityNeed.end(); ++it2)
			{
				if (it2->second > iMaxNeed)
				{
					iMaxNeed = it2->second;
					pBestCity = it2->first;
				}
			}

			if (pBestCity && pUnit->GeneratePath(pBestCity->plot(),CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY,23))
			{
				ExecuteMoveToTarget(pUnit, pBestCity->plot(), 0);
				int iCurrentNeed = mapCityNeed[pBestCity];
				if (iCurrentNeed > 0)
					mapCityNeed[pBestCity] = iCurrentNeed / 2; //reduce the score for this city in case we have multiple workers to distribute
				else
					mapCityNeed[pBestCity]--; //in case all cities have all tiles improved, try spread the workers over all our cities
			}
			else if (pUnit->IsCivilianUnit())
			{
				MoveCivilianToGarrison(pUnit);
			}

			//simply ignore this unit for further building tasks 
			it->second.clear();
			UnitProcessed(pUnit->GetID());
		}
	}
}

/// Heal chosen units
void CvHomelandAI::ExecuteHeals()
{
	CHomelandUnitArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if (!pUnit)
			continue;

		CvPlot* pBestPlot = pUnit->GetDanger()>0 ? TacticalAIHelpers::FindSafestPlotInReach(pUnit,true) : TacticalAIHelpers::FindClosestSafePlotForHealing(pUnit);
		if (pBestPlot && pBestPlot!=pUnit->plot())
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
		if (pUnit->canMove())
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
		UnitProcessed(pUnit->GetID());
	}
}

typedef CvWeightedVector<CvPlot*, 100, true> WeightedPlotVector;

/// Moves units to the hex with the lowest danger
void CvHomelandAI::ExecuteMovesToSafestPlot()
{
	for(unsigned int iI = 0; iI < m_CurrentMoveUnits.size(); iI++)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(m_CurrentMoveUnits[iI].GetID());
		if (!pUnit)
			continue;

		//a bit tricky: we know that we should be able to reach the plot
		//but maybe the mission is aborted (new enemy discovered etc)
		//can happen for AI civilians ...
		CvPlot* pBestPlot = NULL;
		for (int iLimit = 0; iLimit<9; iLimit++) //failsafe so we don't get stuck ...
		{
			pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);

			//can we move?
			if (!pBestPlot || pUnit->plot() == pBestPlot || !pUnit->canMove())
				break;

			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
		} 

		//important, else we can't end the turn
		UnitProcessed(pUnit->GetID());

		if(GC.getLogging() && GC.getAILogging() && pBestPlot)
		{
			CvString strLogString;
			CvString strTemp = GC.getUnitInfo(pUnit->getUnitType())->GetDescription();
			strLogString.Format("Moved %s to safety, X: %d, Y: %d", strTemp.GetCString(), pBestPlot->getX(), pBestPlot->getY());
			LogHomelandMessage(strLogString);
		}
	}
}

//	---------------------------------------------------------------------------
/// Find one unit to move to target, starting with high priority list
void CvHomelandAI::ExecuteMoveToTarget(CvUnit* pUnit, CvPlot* pTarget, int iFlags, bool bEndTurn)
{
	if (!pUnit || !pTarget)
		return;

	AI_PERF_FORMAT("Homeland-ExecuteMove-perf.csv", ("ExecuteMoveToTarget, %d, %d, Turn %03d, %s", pTarget->getX(), pTarget->getY(), GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	if(pUnit->plot() == pTarget && pUnit->canEndTurnAtPlot(pTarget))
	{
		pUnit->PushMission(CvTypes::getMISSION_SKIP());

#if defined(MOD_BALANCE_CORE)
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Best unit in position at X: %d, Y: %d", pUnit->getX(), pUnit->getY());
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
	if(bEndTurn)
		UnitProcessed(pUnit->GetID());
}

/// Great writer moves
void CvHomelandAI::ExecuteWriterMoves()
{
	CHomelandUnitArray::iterator it;
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
					UnitProcessed(pUnit->GetID());
					continue;
				}
				else
				{
					// Already at (or adjacent to) target?
					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
						UnitProcessed(pUnit->GetID());
						continue;
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX,&iTurns))
						{
							// In less than one turn?
							if (iTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
								continue;
							}

							// In multiple moves
							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), 
									CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Writer toward Great Work city at, X: %d, Y: %d", pTargetCity->getX(),  pTargetCity->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
								continue;
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
							UnitProcessed(pUnit->GetID());
							continue;
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
			UnitProcessed(pUnit->GetID());
			break;
		}
	}
}

/// Expends an artist to start a golden age
void CvHomelandAI::ExecuteArtistMoves()
{
	CHomelandUnitArray::iterator it;
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
					UnitProcessed(pUnit->GetID());
					continue;
				}
				else
				{
					// Already at (or adjacent to) target?
					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
						UnitProcessed(pUnit->GetID());
						continue;
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX,&iTurns))
						{
							// In less than one turn?
							if (iTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
							}

							// In multiple moves
							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), 
									CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Artist toward Great Work city at, X: %d, Y: %d", pTargetCity->getX(),  pTargetCity->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
								continue;
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
							UnitProcessed(pUnit->GetID());
							continue;
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
			UnitProcessed(pUnit->GetID());
			break;
		}
	}
}

/// Expends an artist to start a golden age
void CvHomelandAI::ExecuteMusicianMoves()
{
	CHomelandUnitArray::iterator it;
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
					UnitProcessed(pUnit->GetID());
					continue;
				}
				else
				{
					// Already at (or adjacent to) target?
					if (plotDistance(pUnit->getX(), pUnit->getY(), pTargetCity->getX(), pTargetCity->getY()) <= 1)
					{
						pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
						UnitProcessed(pUnit->GetID());
						continue;
					}

					// No, then move there
					else
					{
						// Find which plot (in or adjacent), we can reach in the fewest turns
						int iTurns = INT_MAX;
						if (pUnit->GeneratePath(pTargetCity->plot(),CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX,&iTurns))
						{
							// In less than one turn?
							if (iTurns == 0)
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								pUnit->PushMission(CvTypes::getMISSION_GREAT_WORK());
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving and creating Great Work at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
								continue;
							}

							// In multiple moves
							else
							{
								pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), 
									CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving Great Musician toward Great Work city at, X: %d, Y: %d", pTargetCity->getX(),  pTargetCity->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
								continue;
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
							UnitProcessed(pUnit->GetID());
							continue;
						}
					}
				}
			}	
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
			UnitProcessed(pUnit->GetID());
			break;
		}
	}
}

/// Expends a scientist to gain a free tech
void CvHomelandAI::ExecuteScientistMoves()
{
	CHomelandUnitArray::iterator it;
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
			{
				MoveCivilianToSafety(pUnit);
				UnitProcessed(pUnit->GetID());
			}
			break;
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
			UnitProcessed(pUnit->GetID());
			break;
		}
	}
}

/// Expends an engineer to gain a wonder (or a very emergency build)
void CvHomelandAI::ExecuteEngineerMoves()
{
	CvCity* pWonderCity;
	int iTurnsToTarget;

	CHomelandUnitArray::iterator it;
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
			//int iNextWonderWeight;
			CvCity* pCityToBuildAt = 0;
			BuildingTypes eNextWonderDesired = m_pPlayer->GetCitySpecializationAI()->GetNextWonderDesired();

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

				pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
				if(pWonderCity)
				{
					if(pUnit->AI_getUnitAIType() == UNITAI_ENGINEER)
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
								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Great Engineer hurrying wonder chosen by city specialization AI at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
								continue;
							}
						}

						// No, then move there
						else
						{
							CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
							if(pEng)
							{
								ExecuteMoveToTarget(pEng, pWonderCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY, true);

								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving %s to city specialization wonder city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
								continue;
							}
						}
#if defined(MOD_BALANCE_CORE)
					}
					else if(pUnit->IsCombatUnit())
					{
						CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
						if(pEng)
						{
							ExecuteMoveToTarget(pEng, pWonderCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY, true);
							if(GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Moving %s as garrison to city to boost production at specialization wonder city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
								LogHomelandMessage(strLogString);
							}
							UnitProcessed(pUnit->GetID());
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
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Great Engineer hurrying free wonder at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
										LogHomelandMessage(strLogString);
									}
									UnitProcessed(pUnit->GetID());
									continue;
								}
								else
								{
									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Great Engineer not needed to hurry 1-turn wonder at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
										LogHomelandMessage(strLogString);
									}
									UnitProcessed(pUnit->GetID());
									continue;
								}
							}
							// No, then move there
							else
							{
								CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
								if(pEng)
								{
									ExecuteMoveToTarget(pEng, pWonderCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY, true);

									if(GC.getLogging() && GC.getAILogging())
									{
										CvString strLogString;
										strLogString.Format("Moving %s for free wonder to city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
										LogHomelandMessage(strLogString);
									}
									UnitProcessed(pUnit->GetID());
									continue;
								}
							}
#if defined(MOD_BALANCE_CORE)
						}
						else if(pUnit->IsCombatUnit())
						{
							CvUnit *pEng = GetBestUnitToReachTarget(pWonderCity->plot(), 42);
							if(pEng)
							{
								ExecuteMoveToTarget(pEng, pWonderCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY, true);

								if(GC.getLogging() && GC.getAILogging())
								{
									CvString strLogString;
									strLogString.Format("Moving %s as garrison to boost wonder in city at, X: %d, Y: %d", pEng->getName().c_str(), pWonderCity->getX(),  pWonderCity->getY());
									LogHomelandMessage(strLogString);
								}
								UnitProcessed(pUnit->GetID());
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
	CHomelandUnitArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit || !pUnit->canMove())
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
			if (pTarget)
			{
				if (pUnit->plot() != pTarget)
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);

				if (pUnit->plot() == pTarget && pUnit->canMove())
				{
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), eBuild, -1, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pTarget);

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great Diplomat creating Embassy at %s", pUnit->plot()->GetAdjacentCity()->getName().c_str());
						LogHomelandMessage(strLogString);
					}
				}
			}
			else
				MoveCivilianToSafety(pUnit);
		}

		//no directive
		MoveCivilianToSafety(pUnit);

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Moving Great Diplomat to safety.");
			LogHomelandMessage(strLogString);
		}

		UnitProcessed(pUnit->GetID());
	}
}

void CvHomelandAI::ExecuteMessengerMoves()
{
	vector<int> vIgnoreCities;
	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit || !pUnit->canMove())
			continue;
		
		//Do trade mission
		CvPlot* pTarget = GET_PLAYER(m_pPlayer->GetID()).ChooseMessengerTargetPlot(pUnit,&vIgnoreCities);
		if(pTarget)
		{
			//not at target yet?
			if( ((pUnit->plot() != pTarget) && (pUnit->plot()->getOwner() != pTarget->getOwner())) || !pUnit->canTrade(pUnit->plot()) )
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);
				
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Diplomatic Unit moving to finish Diplomatic Mission at (%d:%d)", pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}
			}

			//now try to finish our mission
			if (((pUnit->plot() == pTarget) || (pUnit->plot()->getOwner() == pTarget->getOwner())) && pUnit->canMove() && pUnit->canTrade(pUnit->plot()))
			{
				pUnit->PushMission(CvTypes::getMISSION_TRADE());
				PlayerTypes ePlayer = pUnit->plot()->getOwner();
				if (ePlayer != NO_PLAYER)
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Diplomatic Unit finishing Diplomatic Mission at %s. Total Influence: %d, Ally is %s",
							GET_PLAYER(ePlayer).getCivilizationShortDescription(),
							GET_PLAYER(pTarget->getOwner()).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(m_pPlayer->GetID()),
							GET_PLAYER(pTarget->getOwner()).GetMinorCivAI()->GetAlly() != NO_PLAYER ? GET_PLAYER(GET_PLAYER(pTarget->getOwner()).GetMinorCivAI()->GetAlly()).getCivilizationShortDescription() : "No Ally");
						LogHomelandMessage(strLogString);
					}
				}

			}
		}
		//Dangerous?
		else
		{
			if(pUnit->isHuman())
			{
				pUnit->SetAutomateType(NO_AUTOMATE);
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
			}
		}

		UnitProcessed(pUnit->GetID());
	}
}
#endif

void CvHomelandAI::ExecuteMerchantMoves()
{
	CHomelandUnitArray::iterator it;
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
		{
			std::vector<CvPlot*> vDummy;
			bool bIsVenice = m_pPlayer->GetPlayerTraits()->IsNoAnnexing();
			BuildTypes eColonia = (BuildTypes)GC.getInfoTypeForString("BUILD_CUSTOMS_HOUSE_VENICE");
			//stupid distinction between Player and PlayerAI classes
			CvPlot* pTargetPlot = bIsVenice ? GET_PLAYER(m_pPlayer->GetID()).FindBestCultureBombPlot(pUnit, eColonia, vDummy, true) : NULL;
			if (pTargetPlot) //venetian merchant
			{
				ExecuteMoveToTarget(pUnit, pTargetPlot, 0, 0);
				if (pUnit->atPlot(*pTargetPlot) && pUnit->canMove())
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), eColonia);
			}
			else if (ExecuteWorkerMove(pUnit)) //regular merchant
			{
				UnitProcessed(pUnit->GetID());
			}
			else
			{
				MoveCivilianToSafety(pUnit);
				UnitProcessed(pUnit->GetID());
			}
			break;
		}
		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
			UnitProcessed(pUnit->GetID());
			break;
		}

	}
}

void CvHomelandAI::ExecuteProphetMoves()
{
	CHomelandUnitArray::iterator it;
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
			{
				MoveCivilianToSafety(pUnit);
				UnitProcessed(pUnit->GetID());
			}
			break;

		case GREAT_PEOPLE_DIRECTIVE_USE_POWER:

			// Can I found a religion?
			if(pUnit->CanFoundReligion(pUnit->plot()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great Prophet founding a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
					LogHomelandMessage(strLogString);
				}

				pUnit->DoFoundReligion();
				UnitProcessed(pUnit->GetID());
				continue;
			}

			// Can I enhance a religion?
			else if(pUnit->CanEnhanceReligion(pUnit->plot()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great Prophet enhancing a religion, X: %d, Y: %d", pUnit->plot()->getX(), pUnit->plot()->getY());
					LogHomelandMessage(strLogString);
				}

				pUnit->DoEnhanceReligion();
				UnitProcessed(pUnit->GetID());
				continue;
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
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestCityPlot->getX(), pBestCityPlot->getY(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);
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
				CvCity* pTarget = m_pPlayer->GetReligionAI()->ChooseProphetConversionCity(pUnit, &iTargetTurns);
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
						ExecuteMoveToTarget(pUnit, pTarget->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1, true);

						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Moving to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
					}
				}
				else
				{
					MoveCivilianToSafety(pUnit);
					UnitProcessed(pUnit->GetID());
				}
			}
			break;

		case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			MoveCivilianToSafety(pUnit);
			UnitProcessed(pUnit->GetID());
			break;
		}
	}
}

/// Moves a great general into an important city to aid its defense
void CvHomelandAI::ExecuteGeneralMoves()
{
	CHomelandUnitArray::iterator it;

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
	CvCity* pHolyCity = NULL;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	if (!MOD_GLOBAL_BREAK_CIVILIAN_1UPT) 
	{
#endif
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eMyReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
		const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
		if(pMyReligion)
		{
			pHolyCity = pMyReligion->GetHolyCity();
			if(pHolyCity && (pHolyCity->getOwner() == m_pPlayer->GetID()))
			{
				bKeepHolyCityClear = true;
			}
		}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
	}
#endif

	BuildTypes eCitadel = (BuildTypes)GC.getInfoTypeForString("BUILD_CITADEL");
	std::vector<CvPlot*> vPlotsToAvoid;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
			continue;

		// this is for the citadel/culture bomb
		if (pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
		{
			CvPlot* pTargetPlot = GET_PLAYER(m_pPlayer->GetID()).FindBestCultureBombPlot(pUnit, pUnit->isCultureBomb() ? NO_BUILD : eCitadel, vPlotsToAvoid, false);
			if(pTargetPlot)
			{
				if(pUnit->plot() != pTargetPlot)
				{
					//continue moving to target
					if (MoveToTargetButDontEndTurn(pUnit, pTargetPlot, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY))
					{
						vPlotsToAvoid.push_back(pTargetPlot);
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

				if(pUnit->plot() == pTargetPlot && pUnit->canMove())
				{
					if (pUnit->isCultureBomb())
					{
						//nothing to build
						pUnit->PushMission(CvTypes::getMISSION_CULTURE_BOMB());
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Great General culture bombed at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
							LogHomelandMessage(strLogString);
						}
						UnitProcessed(pUnit->GetID());
					}
					else
					{
						// find the great general improvement
						BuildTypes eSelectedBuildType = NO_BUILD;
						BuildTypes eBuild;
						int iBuildIndex;
						for (iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
						{
							eBuild = (BuildTypes)iBuildIndex;
							CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
							if (pkBuild == NULL)
								continue;

							if (!pUnit->canBuild(pTargetPlot, eBuild))
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
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Great General citadel'd at, X: %d, Y: %d", pUnit->getX(), pUnit->getY());
								LogHomelandMessage(strLogString);
							}
							UnitProcessed(pUnit->GetID());
						}
						else
						{
							//give up
							pUnit->SetGreatPeopleDirective(NO_GREAT_PEOPLE_DIRECTIVE_TYPE);
						}
					}
				}
			}
		}

		if(pUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND && !pUnit->IsCityAttackSupport())
		{
			int iBestScore = 0;
			CvPlot* pBestPlot = 0;

			//this directive should normally be handled in tactical AI (operation moves, close on target or hedgehog)
			ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
			for (ReachablePlots::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
			{
				CvPlot* pCandidate = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
				//plot needs to have a defender, but no be adjacent to the enemy
				CvUnit* pDefender = pCandidate->getBestDefender(pUnit->getOwner());
				if (!pDefender || (pDefender->GetNumEnemyUnitsAdjacent()>0 && !pCandidate->isCity()))
					continue;

				if(pUnit->IsNearGreatGeneral(pCandidate)) //near another general
					continue;

				//we want to have many neighboring units in danger, but our plot should be relatively safe
				//(look at the danger for the defender, the general danger is zero unless the defender is projected to die)
				int iGeneralDanger = pDefender->GetDanger(pCandidate);
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

					iSupportedDanger += pSupportedUnit->GetDanger(pLoopPlot);
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
				UnitProcessed(pUnit->GetID());
				//make sure our defender doesn't run away
				CvUnit* pDefender = pBestPlot->getBestDefender(pUnit->getOwner());
				if (pDefender && !pDefender->TurnProcessed())
				{
					TacticalAIHelpers::PerformRangedOpportunityAttack(pDefender);
					pDefender->PushMission(CvTypes::getMISSION_SKIP());
					//don't call UnitProcessed, it would overwrite the last move
					pDefender->SetTurnProcessed(true); 
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
			ReachablePlots reachablePlots = pUnit->GetAllPlotsInReachThisTurn(true, true, false);
			for (ReachablePlots::iterator it=reachablePlots.begin(); it!=reachablePlots.end(); ++it)
			{

				int iScore = 0;
				CvPlot* pCandidate = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
				//plot needs to have a defender, but no be adjacent to the enemy
				CvUnit* pDefender = pCandidate->getBestDefender(pUnit->getOwner());
				if (!pDefender)
					continue;

				if(pUnit->IsNearCityAttackSupport(pCandidate, pUnit)) //near another City Attack Support
					continue;

				//we want to have many neighboring units in danger, but our plot should be relatively safe
				//(look at the danger for the defender, the general danger is zero unless the defender is projected to die)
				int iGeneralDanger = pDefender->GetDanger(pCandidate);
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

					iSupportedDanger += pSupportedUnit->GetDanger(pLoopPlot);
				}

				//don't forget adjacency bonus for the candidate plot itself
				if(pCandidate->IsEnemyCityAdjacent(pUnit->getTeam(),NULL))
					iScore += 5000;

				CvCity* pClosestEnemyCity = m_pPlayer->GetTacticalAI()->GetNearestTargetCity(pUnit->plot());
				if(pClosestEnemyCity != NULL)
					iScore += (1000 - (plotDistance(pCandidate->getX(), pCandidate->getY(), pClosestEnemyCity->getX(), pClosestEnemyCity->getY()) * 5));

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
				UnitProcessed(pUnit->GetID());
				//make sure our defender doesn't run away
				CvUnit* pDefender = pBestPlot->getBestDefender(pUnit->getOwner());
				if (pDefender)
				{
					TacticalAIHelpers::PerformRangedOpportunityAttack(pDefender);
					pDefender->PushMission(CvTypes::getMISSION_SKIP());
					UnitProcessed(pDefender->GetID());
					continue;
				}
			}
		}

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
					MoveToTargetButDontEndTurn(pUnit,pChosenCity->plot(),CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);
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
					UnitProcessed(pUnit->GetID());
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
}

/// Moves a great admiral into an important coastal city to aid its defense
void CvHomelandAI::ExecuteAdmiralMoves()
{
	CHomelandUnitArray::iterator it;
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
	CvCity* pHolyCity = NULL;
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
	if (!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS) 
	{
#endif
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eMyReligion = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
		const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
		if(pMyReligion)
		{
			pHolyCity = pMyReligion->GetHolyCity();
			if (pHolyCity && pHolyCity->isCoastal() && pHolyCity->getOwner() == m_pPlayer->GetID())
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
				
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pChosenCity->getX(), pChosenCity->getY(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);
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
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pChosenCity->getX(), pChosenCity->getY(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);
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
	vector<pair<int,int>>& vBurnedTargets = m_automatedTargetPlots[UNITAI_MISSIONARY];

	CHomelandUnitArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns; //todo: don't pick the same target for all missionaries
		CvCity* pTarget = m_pPlayer->GetReligionAI()->ChooseMissionaryTargetCity(pUnit, vBurnedTargets, &iTargetTurns);
		if(pTarget)
		{
			vBurnedTargets.push_back( make_pair(pUnit->GetID(),pTarget->plot()->GetPlotIndex()));

			if(iTargetTurns==0)
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(),CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

				if(pUnit->canMove())
					pUnit->PushMission(CvTypes::getMISSION_SPREAD_RELIGION());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Move %d to spread religion, X: %d, Y: %d", pUnit->GetID(), pTarget->getX(), pTarget->getY());
					LogHomelandMessage(strLogString);
				}

				UnitProcessed(pUnit->GetID());
			}
			else
			{
				ExecuteMoveToTarget(pUnit, pTarget->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_APPROX_TARGET_RING1, true);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Moving %d to plot adjacent to conversion city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pUnit->GetID(), pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
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
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
			}
			UnitProcessed(pUnit->GetID());
		}
#endif
	}
}

// Get a inquisitor to the best city
void CvHomelandAI::ExecuteInquisitorMoves()
{
	vector<pair<int,int>>& vBurnedTargets = m_automatedTargetPlots[UNITAI_INQUISITOR];

	CHomelandUnitArray::iterator it;
	for(it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
		{
			continue;
		}

		int iTargetTurns;
		CvCity* pTarget = m_pPlayer->GetReligionAI()->ChooseInquisitorTargetCity(pUnit, vBurnedTargets, &iTargetTurns);
		if(pTarget)
		{
			vBurnedTargets.push_back( make_pair(pUnit->GetID(),pTarget->plot()->GetPlotIndex()));

			if (pUnit->CanRemoveHeresy(pTarget->plot()))
			{
				if (iTargetTurns == 0)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

					if (pUnit->canMove())
						pUnit->PushMission(CvTypes::getMISSION_REMOVE_HERESY());

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Move to remove heresy, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}

					UnitProcessed(pUnit->GetID());
				}
				else
				{
					ExecuteMoveToTarget(pUnit, pTarget->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN, true);

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to plot adjacent to heresy removal city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}
			else
			{
				if (iTargetTurns == 0)
				{
					pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1);

					if (pUnit->canMove())
						pUnit->PushMission(CvTypes::getMISSION_REMOVE_HERESY());

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Move to garrison against heresy, X: %d, Y: %d", pTarget->getX(), pTarget->getY());
						LogHomelandMessage(strLogString);
					}

					UnitProcessed(pUnit->GetID());
				}
				else
				{
					ExecuteMoveToTarget(pUnit, pTarget->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN, true);

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Moving to plot adjacent to garrison against heresy in city, X: %d, Y: %d, Currently at, X: %d, Y: %d", pTarget->getX(), pTarget->getY(), pUnit->getX(), pUnit->getY());
						LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}
}

/// Moves a spaceship part to the capital (or into the spaceship if already there)
void CvHomelandAI::ExecuteSSPartMoves()
{
	CvCity* pCapitalCity = m_pPlayer->getCapitalCity();
	if (pCapitalCity == NULL)
		return;

	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
			continue;

		ExecuteMoveToTarget(pUnit, pCapitalCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);

		if(pUnit->plot() == pCapitalCity->plot() && pUnit->canMove())
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
		}

		UnitProcessed(pUnit->GetID());
	}
}

/// Moves a treasure to the capital
void CvHomelandAI::ExecuteTreasureMoves()
{
	CvCity* pCapitalCity = m_pPlayer->getCapitalCity();
	if (pCapitalCity == NULL)
		return;

	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
		if(!pUnit)
			continue;

		ExecuteMoveToTarget(pUnit, pCapitalCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);
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

/// Moves an aircraft into an important city near the front to aid its defense (or offense)
void CvHomelandAI::ExecuteAircraftMoves()
{
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

	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
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
		if(pUnit->shouldHeal())
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
				if (!pUnit->canRebase() || !(it->pPlot->isCity()))
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
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
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
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
		}

		//that's it for this turn, whether we rebased successfully or not
		UnitProcessed(pUnit->GetID());
	}

	//exciting places first - we should have some space there now
	std::reverse(vPotentialBases.begin(),vPotentialBases.end());
	for (size_t i=0; i<vCombatReadyUnits.size(); ++i)
	{
		bool bBest = false;
		CvUnit* pUnit = vCombatReadyUnits[i];
		CvPlot* pNewBase = NULL;
		for (std::vector<SPlotWithScore>::iterator it=vPotentialBases.begin(); it!=vPotentialBases.end(); ++it)
		{
			//unsuitable - we want to fight!
			if (it->score<0)
				continue;

			//make sure the unit fits the destination (ie missile to cruiser, fighter to carrier)
			if (!pUnit->canRebase() || !pUnit->canLoad(*(it->pPlot)))
				continue;

			//apparently we're already in the best possible base?
			if (scoreLookup[pUnit->plot()->GetPlotIndex()] >= it->score)
			{
				bBest = true;
				break;
			}

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
				if (bBest)
					strLogString.Format("Rebase not needed for %s (%d) at %d,%d for combat", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
				else
					strLogString.Format("Failed to rebase %s (%d) at %d,%d for combat", pUnit->getName().c_str(), pUnit->GetID(), pUnit->getX(), pUnit->getY());
				LogHomelandMessage(strLogString);
			}
	
			//not strictly needed but looks cooler than doing nothing and we're ready for surprise attacks
			pUnit->PushMission(CvTypes::getMISSION_AIRPATROL());
		}

		//that's it for this turn, whether we rebased successfully or not
		UnitProcessed(pUnit->GetID());
	}
}

#if defined(MOD_BALANCE_CORE)
bool CvHomelandAI::MoveCivilianToGarrison(CvUnit* pUnit)
{
	WeightedPlotVector aBestPlotList;
	int iLoopCity;
	for(CvCity *pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isInDangerOfFalling())
			continue;

		CvPlot* pLoopPlot = pLoopCity->plot();
		if(!pUnit->canMoveInto(*pLoopPlot, CvUnit::MOVEFLAG_DESTINATION))
			continue;

		int iValue = 0; //default

		//see if there is work to do
		if (pUnit->AI_getUnitAIType() == UNITAI_WORKER)
			iValue += pLoopCity->countNumImprovablePlots(NO_IMPROVEMENT, DOMAIN_LAND) * 10;
		else
		{
			//try to spread out
			int iNumFriendlies = pLoopPlot->getNumUnitsOfAIType(pUnit->AI_getUnitAIType());
			if (pLoopPlot == pUnit->plot())
				iNumFriendlies--;
			iValue -= iNumFriendlies * 20;

			//stay close to the core
			if (m_pPlayer->getCapitalCity())
				iValue -= plotDistance(m_pPlayer->getCapitalCity()->getX(), m_pPlayer->getCapitalCity()->getY(), pLoopCity->getX(), pLoopCity->getY());
		}

		aBestPlotList.push_back(pLoopPlot, iValue);
	}

	CvPlot* pBestPlot = NULL;
	if (aBestPlotList.size()>0)
	{
		aBestPlotList.SortItems(); //highest score will be first
		pBestPlot=aBestPlotList.GetElement(0);

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

			MoveToTargetButDontEndTurn(pUnit, pBestPlot, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);
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
bool CvHomelandAI::MoveCivilianToSafety(CvUnit* pUnit)
{
	// Now loop through the sorted score list and go to the best one we can reach in one turn.
	CvPlot* pBestPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit,true);
	if(pBestPlot != NULL)
	{
		if(pUnit->atPlot(*pBestPlot))
		{
			//if the units is not in friendly territory and not in danger, try to bring it home ...
			if (!pBestPlot->IsFriendlyTerritory(m_pPlayer->GetID()) && pUnit->GetDanger() == 0 && m_pPlayer->getCapitalCity())
			{
				ExecuteMoveToTarget(pUnit, m_pPlayer->getCapitalCity()->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN | CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED, true);
				return true;
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
	TradeConnectionList aTradeConnections;
	CvTradeAI* pkTradeAI = m_pPlayer->GetTradeAI();
	pkTradeAI->GetPrioritizedTradeRoutes(aTradeConnections,true);
	
	//stats to decide whether to disband a unit
	int iWaterRoutes = 0;
	int iLandRoutes = 0;

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

		//we don't really care about the distance but not having to re-base is good
		CvUnit *pBestUnit = NULL;
		int iBestDistance = INT_MAX;
		for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
		{
			CvUnit* pUnit = m_pPlayer->getUnit(it->GetID());
			if(!pUnit || !pUnit->canMove() || pUnit->TurnProcessed() || pUnit->IsAutomated())
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

			if (pOriginPlot != pBestUnit->plot())
			{
				pBestUnit->PushMission(CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY(), pOriginPlot->getX(), pOriginPlot->getY());
				if(GC.getLogging() && GC.getAILogging() && pBestUnit->plot()->isCity())
				{
					CvString strLogString;
					strLogString.Format("Changing trade route home city from %s to %s", pBestUnit->plot()->getPlotCity()->getName().c_str(), pOriginCity->getName().c_str());
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
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
					case TRADE_CONNECTION_GOLD_INTERNAL:
						strLogString.Format("Establishing gold trade route (internal) from %s to %s", pOriginCity->getName().c_str(), pDestCity->getName().c_str());
						break;
#endif
					}

					LogHomelandMessage(strLogString);
				}
			}

			UnitProcessed(pBestUnit->GetID());
		}
	}

	//unassigned trade units?
	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
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

	CHomelandUnitArray::iterator it;
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
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTarget->getX(), pTarget->getY(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY);

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
	std::vector<CvHomelandTarget> tempPoints(m_TargetedSentryPoints);

	// Clear out main list
	m_TargetedSentryPoints.clear();

	// Loop through all points in copy
	for(std::vector<CvHomelandTarget>::iterator it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotUnchecked(it->GetTargetX(), it->GetTargetY());

		//always use fortifications and shadow potential enemies
		if(it->GetTargetType() == AI_HOMELAND_TARGET_FORT || pPlot->isNeutralUnitAdjacent(m_pPlayer->GetID(),true,true,true))
		{
			m_TargetedSentryPoints.push_back(*it);
		}
		else
		{
			// Is it adjacent to a point in the main list?
			bool bFoundAdjacent = false;
			for(std::vector<CvHomelandTarget>::iterator it2 = m_TargetedSentryPoints.begin(); it2 != m_TargetedSentryPoints.end(); ++it2)
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

/// Don't allow adjacent tiles to both be sentry points
void CvHomelandAI::EliminateAdjacentNavalSentryPoints()
{
	// First, sort the sentry points by priority
	std::stable_sort(m_TargetedNavalSentryPoints.begin(), m_TargetedNavalSentryPoints.end());

	// Create temporary copy of list
	std::vector<CvHomelandTarget> tempPoints(m_TargetedNavalSentryPoints);

	// Clear out main list
	m_TargetedNavalSentryPoints.clear();

	// Loop through all points in copy
	for(std::vector<CvHomelandTarget>::iterator it = tempPoints.begin(); it != tempPoints.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotUnchecked(it->GetTargetX(), it->GetTargetY());

		//always guard improvements and shadow potential enemies
		if (pPlot->getImprovementType() != NO_IMPROVEMENT|| pPlot->isNeutralUnitAdjacent(m_pPlayer->GetID(), true, true, true))
		{
			m_TargetedNavalSentryPoints.push_back(*it);
		}
		else
		{
			bool bFoundAdjacent = false;
			// Is it adjacent to a point in the main list?
			for (std::vector<CvHomelandTarget>::iterator it2 = m_TargetedNavalSentryPoints.begin(); it2 != m_TargetedNavalSentryPoints.end(); ++it2)
			{
				if (plotDistance(it->GetTargetX(), it->GetTargetY(), it2->GetTargetX(), it2->GetTargetY()) == 1)
				{
					bFoundAdjacent = true;
					break;
				}
			}
			if (!bFoundAdjacent)
			{
				m_TargetedNavalSentryPoints.push_back(*it);
			}
		}
	}
}

/// Finds both high and normal priority units we can use for this homeland move (returns true if at least 1 unit found)
bool CvHomelandAI::FindUnitsForThisMove(AIHomelandMove eMove)
{
	bool rtnValue = false;

	ClearCurrentMoveUnits(eMove);
			
	// Loop through all units available to homeland AI this turn
	for(list<int>::iterator it = m_CurrentTurnUnits.begin(); it != m_CurrentTurnUnits.end(); ++it)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(*it);
		if(pLoopUnit && !pLoopUnit->isHuman())
		{
			// Civilians or units in armies aren't useful for any of these moves
			if(!pLoopUnit->canMove() || !pLoopUnit->IsCombatUnit() || pLoopUnit->getArmyID() != -1)
				continue;

			bool bSuitableUnit = false;
			switch(eMove)
			{
			case AI_HOMELAND_MOVE_GARRISON:
				//Don't poach garrisons for garrisons.
				if(pLoopUnit->IsGarrisoned())
					continue;

				//don't use explorers
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE)
					continue;

				// Want to put ranged units in cities to give them a ranged attack
				if(pLoopUnit->isRanged())
					bSuitableUnit = true;

				break;

				case AI_HOMELAND_MOVE_SENTRY:
					// No ranged units as sentries
					if(pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->IsEverFortifyable() && !pLoopUnit->IsGarrisoned() && 
					( pLoopUnit->isUnitAI(UNITAI_DEFENSE) || pLoopUnit->isUnitAI(UNITAI_ATTACK) || pLoopUnit->isUnitAI(UNITAI_COUNTER) ) )
				{
					bSuitableUnit = true;
				}
				break;
			case AI_HOMELAND_MOVE_SENTRY_NAVAL:
				// No ranged units as sentries (that would be assault_sea)
				if(pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit() && pLoopUnit->isUnitAI(UNITAI_ATTACK_SEA))
				{
					bSuitableUnit = true;
				}
				break;
			case AI_HOMELAND_MOVE_ANCIENT_RUINS:
				// Fast movers are top priority
				if(pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_FAST_ATTACK))
					bSuitableUnit = true;
					break;
				}

			// If unit was suitable, add it to the proper list
			if(bSuitableUnit)
			{
				CvHomelandUnit unit;
				unit.SetID(pLoopUnit->GetID());
				m_CurrentMoveUnits.push_back(unit);
				rtnValue = true;
			}
		}
	}

	return rtnValue;
}

//	---------------------------------------------------------------------------
/// Compute the best unit to reach a target in the current normal and high priority move list
CvUnit* CvHomelandAI::GetBestUnitToReachTarget(CvPlot* pTarget, int iMaxTurns)
	{
	// Get the raw distance for all units
	int iTargetX = pTarget->getX();
	int iTargetY = pTarget->getY();

	// first check raw distance
	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if(pLoopUnit)
		{
			// Make sure domain matches
			if(pLoopUnit->getDomainType() == DOMAIN_SEA && !pTarget->isWater() ||
				pLoopUnit->getDomainType() == DOMAIN_LAND && pTarget->isWater())
			{
				it->SetMovesToTarget(MAX_INT);
				continue;
			}

			if (pLoopUnit->TurnProcessed() || !pLoopUnit->canMove())
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
				if (pLoopUnit->canMoveInto(*pTarget, CvUnit::MOVEFLAG_DESTINATION))
					it->SetMovesToTarget(plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iTargetX, iTargetY));
				else
					it->SetMovesToTarget(MAX_INT);
			}
		}
	}

	// Sort by raw distance
	std::stable_sort(m_CurrentMoveUnits.begin(), m_CurrentMoveUnits.end());

	CvUnit* pBestUnit = NULL;
	int iBestTurns = iMaxTurns;

	int iFailedPaths = 0;
	// If we see this many failed pathing attempts, we assume no unit can get to the target
	const int MAX_FAILED_PATHS = 2;

	// Now go through and figure out the actual number of turns, and as a result, even if it can get there at all.
	// We will try and do as few as possible by stopping if we find a unit that can make it in one turn.
	for(CHomelandUnitArray::iterator it = m_CurrentMoveUnits.begin(); it != m_CurrentMoveUnits.end(); ++it)
	{
		CvUnit* pLoopUnit = m_pPlayer->getUnit(it->GetID());
		if (pLoopUnit)
		{
			int iDistance = it->GetMovesToTarget();	// Raw distance
			if (iDistance == MAX_INT)
				continue;
			
			//pretent turns are "moves" here
			int iTurns = pLoopUnit->TurnsToReachTarget(pTarget, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY, iMaxTurns);
			it->SetMovesToTarget(iTurns);

			// Did we make it at all?
			if (iTurns == MAX_INT)
			{
				++iFailedPaths;
				if (iFailedPaths >= MAX_FAILED_PATHS)
					break;
				}

			// Take the first one ...
			if (iTurns < iBestTurns)
				{
					pBestUnit = pLoopUnit;
				iBestTurns = iTurns;
					break;
			}
			}
		}

	return pBestUnit;
}

/// Find best target for this archaeologist
CvPlot* CvHomelandAI::FindArchaeologistTarget(CvUnit *pUnit)
{
	CvPlot *pBestTarget = NULL;
	int iBestTurns = MAX_INT;

	BuildTypes eBuild = (BuildTypes)GC.getInfoTypeForString("BUILD_ARCHAEOLOGY_DIG");

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
			CvPlot* pTest = iterateRingPlots(pTarget,i);
			if (!pTest || pTest==pUnit->plot())
				continue;

			//other players' units are also a bad sign
			if (pTest->getNumUnitsOfAIType(UNITAI_ARCHAEOLOGIST,NO_PLAYER)>0)
			{
				bIgnore = true;
				break;
			}
		}

		if (bIgnore)
			continue;

		if (eBuild == NO_BUILD || !pUnit->canBuild(pTarget, eBuild))
			continue;

		if (pUnit->GetDanger(pTarget) == 0 && !m_pPlayer->IsAtWarWith(pTarget->getOwner()))
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

			int iTurns = pUnit->TurnsToReachTarget(pTarget, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY|CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN, iBestTurns);
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
		strPlayerName.Replace(' ', '_'); //no spaces
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
	if (pUnit)
	{
		pUnit->setHomelandMove(m_CurrentMoveUnits.getCurrentHomelandMove());
		pUnit->SetTurnProcessed(true);
	}
}

CvPlot* CvHomelandAI::ExecuteWorkerMove(CvUnit* pUnit)
{
	if (!pUnit)
		return NULL;

	//pretend there are no other workers ...
	std::map<CvUnit*,ReachablePlots> allWorkersReachablePlots;
	SPathFinderUserData data(pUnit, 0, 17);
	data.ePathType = PT_UNIT_REACHABLE_PLOTS;
	allWorkersReachablePlots[pUnit] = GC.GetPathFinder().GetPlotsInReach(pUnit->plot(), data);
	return ExecuteWorkerMove(pUnit, allWorkersReachablePlots);
}

//returns the target plot if sucessful, null otherwise
CvPlot* CvHomelandAI::ExecuteWorkerMove(CvUnit* pUnit, const map<CvUnit*,ReachablePlots>& allWorkersReachablePlots)
{
	// find work (considering all other workers as well)
	BuilderDirective aDirective = m_pPlayer->GetBuilderTaskingAI()->EvaluateBuilder(pUnit, allWorkersReachablePlots);
	if(aDirective.m_eDirective!=BuilderDirective::NUM_DIRECTIVES)
	{
		switch(aDirective.m_eDirective)
		{
		case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
		case BuilderDirective::BUILD_IMPROVEMENT:
		case BuilderDirective::REPAIR:
		case BuilderDirective::BUILD_ROUTE:
		case BuilderDirective::CHOP:
		case BuilderDirective::REMOVE_ROAD:
		{
			CvPlot* pPlot = GC.getMap().plot(aDirective.m_sX, aDirective.m_sY);
			MissionTypes eMission = CvTypes::getMISSION_MOVE_TO();
			if(pUnit->getX() == aDirective.m_sX && pUnit->getY() == aDirective.m_sY)
				eMission = CvTypes::getMISSION_BUILD();

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

				switch(aDirective.m_eDirective)
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
					if(aDirective.m_eDirective == BuilderDirective::REPAIR)
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
					else if(aDirective.m_eDirective == BuilderDirective::BUILD_ROUTE)
					{
						strLog += "Building route,";
					}
					else if(aDirective.m_eDirective == BuilderDirective::BUILD_IMPROVEMENT || aDirective.m_eDirective == BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE)
					{
						strLog += "Building improvement,";
					}
					else if(aDirective.m_eDirective == BuilderDirective::CHOP)
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
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), aDirective.m_sX, aDirective.m_sY, CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, pPlot);

				//do we have movement left?
				if (pUnit->getMoves()>0)
					eMission = CvTypes::getMISSION_BUILD();
				else
					UnitProcessed(pUnit->GetID());
			}

			if(eMission == CvTypes::getMISSION_BUILD())
			{
				// check to see if we already have this mission as the unit's head mission
				const MissionData* pkMissionData = pUnit->GetHeadMissionData();
				if(pkMissionData == NULL || pkMissionData->eMissionType != eMission || pkMissionData->iData1 != aDirective.m_eBuild)
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), aDirective.m_eBuild, aDirective.m_eDirective, 0, (pUnit->GetLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);

				CvAssertMsg(!pUnit->ReadyToMove(), "Worker did not do their mission this turn. Could cause game to hang.");
				UnitProcessed(pUnit->GetID());
			}

			return GC.getMap().plot(aDirective.m_sX, aDirective.m_sY);
		}
		break;
		}
	}

	return NULL;
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

	//some plot we can enter but we cannot stay there
	if (!pUnit->canEnterTerritory(pPlot->getTeam()))
	{
		return false;
	}

	// see if we can capture a civilian?
	int iFlags = CvUnit::MOVEFLAG_DESTINATION;
	if (!pPlot->isVisibleEnemyDefender(pUnit) && pPlot->isVisibleEnemyUnit(pUnit))
		iFlags |= CvUnit::MOVEFLAG_ATTACK;

	if(!pUnit->canMoveInto(*pPlot, iFlags))
	{
		return false;
	}

	return true;
}

/// Move an exploring unit to a designated target (special function exposed to Lua)
bool CvHomelandAI::ExecuteSpecialExploreMove(CvUnit* pUnit, CvPlot* pTargetPlot)
{
	int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_MAXIMIZE_EXPLORE | CvUnit::MOVEFLAG_IGNORE_DANGER;

	SPathFinderUserData data(pUnit,iFlags);
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
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY(), iFlags, false, false, MISSIONAI_EXPLORE, pPlot);
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

	CvHomelandTarget newTarget;
	TeamTypes eTeam = m_pPlayer->getTeam();
	CvMap& theMap = GC.getMap();
	int iNumPlots = theMap.numPlots();

	for(int iI = 0; iI < iNumPlots; iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);

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
void CvHomelandAI::ClearCurrentMoveUnits(AIHomelandMove eNextMove)
{
	m_CurrentMoveUnits.setCurrentHomelandMove(eNextMove);
	m_CurrentMoveUnits.clear();
}

#if defined(MOD_BALANCE_CORE_MILITARY)

bool CvHomelandAI::MoveToTargetButDontEndTurn(CvUnit* pUnit, CvPlot* pTargetPlot, int iFlags)
{
	if(pUnit->GeneratePath(pTargetPlot,iFlags,INT_MAX,NULL,true))
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY(), iFlags, false, false, MISSIONAI_HOMEMOVE, pTargetPlot);
		return true;
	}
	else
	{
		// Embarked and in danger? We need to do something!
		if (pUnit->isEmbarked())
		{
			pTargetPlot = TacticalAIHelpers::FindSafestPlotInReach(pUnit, true);
			if (pTargetPlot)
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pTargetPlot->getX(), pTargetPlot->getY());
			else
				pUnit->PushMission(CvTypes::getMISSION_SKIP());
		}
		else
		{
			// No safe path so just stay put and fortify until life improves for you.
			pUnit->PushMission(CvTypes::getMISSION_SKIP());
		}

		return false;
	}
}

const char* homelandMoveNames[] =
{
	"H_MOVE_NONE",
	"H_MOVE_UNASSIGNED",
	"H_MOVE_EXPLORE",
	"H_MOVE_EXPLORE_SEA",
	"H_MOVE_SETTLE",
	"H_MOVE_GARRISON",
	"H_MOVE_HEAL",
	"H_MOVE_TO_SAFETY",
	"H_MOVE_SENTRY",
	"H_MOVE_SENTRY_NAVAL",
	"H_MOVE_WORKER",
	"H_MOVE_WORKER_SEA",
	"H_MOVE_PATROL",
	"H_MOVE_UPGRADE",
	"H_MOVE_ANCIENT_RUINS",
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
	"H_MOVE_DIPLOMAT_EMBASSY",
	"H_MOVE_MESSENGER",
	"H_MOVE_SECONDARY_SETTLER",
	"H_MOVE_SECONDARY_WORKER",
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
		if(!pDefender || pDefender->IsHurt())  
			return -1;

		iBaseScore += 10;

		if (pDefender->getArmyID()!=-1)
			iBaseScore += 30;
	}
	else
	{
		CvCity* pCity = pBasePlot->getPlotCity();
		//careful in cities we might lose
		if(pCity->isInDangerOfFalling() || (GC.getGame().getGameTurn() - pCity->getGameTurnAcquired())<3)
			return -1;
		if (pCity->IsRazing() && pCity->getPopulation() < 3)
			return -1;

		if (GET_PLAYER(ePlayer).GetFlatDefenseFromAirUnits() != 0 && pCity->isUnderSiege())
		{
			iBaseScore += 2 * GET_PLAYER(ePlayer).GetFlatDefenseFromAirUnits();
		}

		if (GET_PLAYER(ePlayer).GetNeedsModifierFromAirUnits() != 0)
		{
			iBaseScore += GET_PLAYER(ePlayer).GetNeedsModifierFromAirUnits() * pCity->getUnhappinessAggregated() / 5;
		}
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
			iBaseScore += 12;
			break;
		case AI_TACTICAL_TARGET_MEDIUM_PRIORITY_UNIT:
			iBaseScore += 8;
			break;
		case AI_TACTICAL_TARGET_LOW_PRIORITY_UNIT:
			iBaseScore += 4;
			break;
		case AI_TACTICAL_TARGET_CITY:
			{
				//for nukes we want only cities, but ones we haven't recently nuked. for conventional air force cities are optional
				CvPlot* pTargetPlot = GC.getMap().plot(allTargets[iI].GetTargetX(), allTargets[iI].GetTargetY());
				if (!pTargetPlot->getPlotCity()->isInDangerOfFalling())
					iBaseScore += 20; 
				break;
			}
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
vector<SPatrolTarget> HomelandAIHelpers::GetPatrolTargets(PlayerTypes ePlayer, bool bWater, int nMaxTargets)
{
	if (ePlayer==NO_PLAYER)
		return vector<SPatrolTarget>();

	CvTacticalAnalysisMap* pTactMap = GET_PLAYER(ePlayer).GetTacticalAI()->GetTacticalAnalysisMap();

	vector<SPatrolTarget> vTargets;
	for(int iI = 0; iI < pTactMap->GetNumZones(); iI++)
	{
		CvTacticalDominanceZone* pZone = pTactMap->GetZoneByIndex(iI);
		if (!pZone || pZone->IsWater()!=bWater)
			continue;

		//ignore other players
		PlayerTypes eOwner = pZone->GetOwner();
		if (eOwner != ePlayer)
			continue;

		//watch out, a city can occur multiple times (islands ...)
		CvCity* pZoneCity = pZone->GetZoneCity();
		if (!pZoneCity)
			continue;

		CvCity* pWorstNeighborCity = NULL;
		int iBorderScore = pZone->GetBorderScore(bWater?DOMAIN_SEA:DOMAIN_LAND,&pWorstNeighborCity);

		if (iBorderScore>0)
			vTargets.push_back( SPatrolTarget(pZoneCity->plot(), pWorstNeighborCity ? pWorstNeighborCity->plot() : NULL, iBorderScore) );
	}

	//default sort is descending!
	std::sort( vTargets.begin(), vTargets.end() );

	vector<SPatrolTarget> vResult;
	for (size_t i=0; i<MIN(vTargets.size(),(size_t)nMaxTargets); i++)
		//copy the top N results, take care not to have duplicate cities in there
		if (std::find(vResult.begin(),vResult.end(),vTargets[i])==vResult.end())
			vResult.push_back( vTargets[i] );

	return vResult;
}

SPatrolTarget::SPatrolTarget()
{
	pTarget = NULL;
	pWorstEnemy = NULL;
	iThreatLevel = 0;
}

SPatrolTarget::SPatrolTarget(CvPlot * target, CvPlot * neighbor, int iThreat)
{
	pTarget = target;
	pWorstEnemy = neighbor;
	iThreatLevel = iThreat;
}

bool SPatrolTarget::operator<(const SPatrolTarget & rhs) const
{
	//sort descending!
	return iThreatLevel > rhs.iThreatLevel;
}

bool SPatrolTarget::operator==(const SPatrolTarget & rhs) const
{
	//ignore threat level for comparison
	return pTarget == rhs.pTarget && pWorstEnemy == rhs.pWorstEnemy;
}
