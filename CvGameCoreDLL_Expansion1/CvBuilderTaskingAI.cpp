/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvBuilderTaskingAI.h"
#include "CvPlayer.h"
#include "CvTraitClasses.h"
#include "CvAStar.h"
#include "CvImprovementClasses.h"
#include "CvCityConnections.h"

// include after all other headers
#include "LintFree.h"

CvWeightedVector<BuilderDirective, 100, true> CvBuilderTaskingAI::m_aDirectives;
FStaticVector<int, SAFE_ESTIMATE_NUM_EXTRA_PLOTS, true, c_eCiv5GameplayDLL, 0> CvBuilderTaskingAI::m_aiNonTerritoryPlots; // plots that we need to evaluate that are outside of our territory

/// Constructor
CvBuilderTaskingAI::CvBuilderTaskingAI(void)
{
	Uninit();
}

/// Destructor
CvBuilderTaskingAI::~CvBuilderTaskingAI(void)
{
	Uninit();
}

/// Init
void CvBuilderTaskingAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_eRepairBuild = GetRepairBuild();
	m_eFalloutFeature = GetFalloutFeature();
	m_eFalloutRemove = GetFalloutRemove();

	m_aiNonTerritoryPlots.clear();
	m_bLogging = GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging();
	m_iNumCities = -1;
	m_pTargetPlot = NULL;

	// special case code so the Dutch don't remove marshes
	m_bKeepMarshes = false;
	for(int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(!pkBuild)
		{
			continue;
		}
		
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
		{
			continue;
		}

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo == NULL)
		{
			continue;
		}

		if(pkImprovementInfo->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkImprovementInfo->GetRequiredCivilization();
			if(eCiv == pPlayer->getCivilizationType())
			{
				if(pkImprovementInfo->GetFeatureMakesValid(FEATURE_MARSH))
				{
					m_bKeepMarshes = true;
				}
			}
		}
	}
}

/// Uninit
void CvBuilderTaskingAI::Uninit(void)
{
	m_eRepairBuild = NO_BUILD;
	m_pPlayer = NULL;
	m_bLogging = false;
	m_iNumCities = -1;
	m_pTargetPlot = NULL;
}

/// Serialization read
void CvBuilderTaskingAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_eRepairBuild;

	uint uiNumSlots;

	// non-territory plots
	kStream >> uiNumSlots;
	m_aiNonTerritoryPlots.resize(uiNumSlots);

	for(uint ui = 0; ui < uiNumSlots; ui++)
	{
		kStream >> m_aiNonTerritoryPlots[ui];
	}

	if (uiVersion >= 2)
	{
		kStream >> m_bKeepMarshes;
	}
	else
	{
		m_bKeepMarshes = false;
	}
	m_iNumCities = -1; //Force everyone to do an CvBuilderTaskingAI::Update() after loading	
	m_pTargetPlot = NULL;		//Force everyone to recalculate current yields after loading.
}

/// Serialization write
void CvBuilderTaskingAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

	kStream << m_eRepairBuild;

	// non-territory plots
	kStream << m_aiNonTerritoryPlots.size();
	for(uint ui = 0; ui < m_aiNonTerritoryPlots.size(); ui++)
	{
		kStream << m_aiNonTerritoryPlots[ui];
	}

	kStream << m_bKeepMarshes;
}

/// Update
void CvBuilderTaskingAI::Update(void)
{
	UpdateRoutePlots();
	m_iNumCities = m_pPlayer->getNumCities();

	int iLoop;
	CvCity* pCity;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		pCity->GetCityStrategyAI()->UpdateBestYields();
	}

	if(m_bLogging)
	{
		bool bShowOutput = m_pPlayer->isHuman();

		if(m_pPlayer->IsEmpireUnhappy())
		{
			CvString str = "// Empire Unhappy! //";
			LogInfo(str, m_pPlayer, bShowOutput);
		}

		// show crisis states
		CvCity* pLoopCity;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			CvString str;
			str += "// ";
			CvString strCityName;
			strCityName = pLoopCity->getName();
			str += strCityName;
			str += " \\\\";

			LogInfo(str, m_pPlayer, bShowOutput);

			for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
			{
				//double fYield = pLoopCity->GetCityStrategyAI()->GetYieldAverage((YieldTypes)ui);
				//double fYieldDeficient = pLoopCity->GetCityStrategyAI()->GetDeficientYieldValue((YieldTypes)ui);
				CvString strYield;
				switch(ui)
				{
				case YIELD_FOOD:
					strYield = "food       ";
					break;
				case YIELD_PRODUCTION:
					strYield = "production ";
					break;
				case YIELD_SCIENCE:
					strYield = "science    ";
					break;
				case YIELD_GOLD:
					strYield = "gold       ";
					break;
				case YIELD_CULTURE:
					strYield = "culture    ";
					break;
				case YIELD_FAITH:
					strYield = "faith      ";
					break;
				}

				CvString strNumbers;
				strNumbers.Format("%d, %d", pLoopCity->GetCityStrategyAI()->GetBestYieldAverageTimes100((YieldTypes)ui), pLoopCity->GetCityStrategyAI()->GetYieldDeltaTimes100((YieldTypes)ui));

				//int iYieldAdjusted = (int)workerround(fYield * 100);
				//int iYieldDeficientAdjacent = (int)workerround(fYieldDeficient * 100);

				//strNumbers.Format("%d / %d", iYieldAdjusted, iYieldDeficientAdjacent);
				strYield += strNumbers;

				if(ui == pLoopCity->GetCityStrategyAI()->GetFocusYield())
				{
					strYield += " *";
				}

				//if (iYieldAdjusted < iYieldDeficientAdjacent)
				//{
				//if (GetDeficientYield(pLoopCity, false) != GetDeficientYield(pLoopCity, true))
				//{
				//	strYield += "  Problem, but happiness over is overriding it";
				//}
				//else
				//{
				//	strYield += "  PROBLEM!!";
				//}
				//}
				LogInfo(strYield, m_pPlayer, bShowOutput);
			}

			str = "\\\\ end ";
			str += strCityName;
			str += " //";
			LogInfo(str, m_pPlayer, bShowOutput);
		}
	}
}

int GetPlotYield(CvPlot* pPlot, YieldTypes eYield)
{
	if(pPlot->getTerrainType() == NO_TERRAIN)
	{
		return 0;
	}

	return pPlot->calculateNatureYield(eYield, NO_TEAM);;
}

void CvBuilderTaskingAI::ConnectCities(CvCity* pPlayerCapital, CvCity* pTargetCity, RouteTypes eRoute)
{
	bool bMajorMinorConnection = false;
	if(pTargetCity->getOwner() != pPlayerCapital->getOwner())
	{
		bMajorMinorConnection = true;
	}

	bool bIndustrialRoute = false;
	if(GC.getGame().GetIndustrialRoute() == eRoute)
	{
		bIndustrialRoute = true;
	}

	// if we already have a connection, bail out
	if(bIndustrialRoute && pTargetCity->IsIndustrialRouteToCapital())
	{
		return;
	}
	else if(m_pPlayer->IsCapitalConnectedToCity(pTargetCity, eRoute))
	{
		return;
	}

	int iGoldForRoute = 0;
	if(!bMajorMinorConnection)
	{
		iGoldForRoute = m_pPlayer->GetTreasury()->GetRouteGoldTimes100(pTargetCity) / 100;
	}

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if(!pRouteInfo)
	{
		return;
	}
	int iMaintenancePerTile = pRouteInfo->GetGoldMaintenance();
	if(iMaintenancePerTile < 0)  // div by zero check
	{
		return;
	}

	// build a path between the two cities
	int iPathfinderFlags = m_pPlayer->GetID();
	int iRouteValue = eRoute + 1;
	// assuming that there are fewer than 256 players
	iPathfinderFlags |= (iRouteValue << 8);
	bool bFoundPath = GC.GetBuildRouteFinder().GeneratePath(pPlayerCapital->plot()->getX(), pPlayerCapital->plot()->getY(), pTargetCity->plot()->getX(), pTargetCity->plot()->getY(), iPathfinderFlags);

	//  if no path, then bail!
	if(!bFoundPath)
	{
		return;
	}

	// walk the path
	CvPlot* pPlot = NULL;

	// go through the route to see how long it is and how many plots already have roads
	int iRoadLength = 0;
	int iPlotsNeeded = 0;
	CvAStarNode* pNode = GC.GetBuildRouteFinder().GetLastNode();
	while(pNode)
	{
		pPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
		pNode = pNode->m_pParent;
		if(!pPlot)
		{
			break;
		}

		CvCity* pCity = pPlot->getPlotCity();
		if(NULL != pCity && pCity->getTeam() == m_pPlayer->getTeam())
		{
			continue;
		}

		if(pPlot->getRouteType() == eRoute && !pPlot->IsRoutePillaged())
		{
			// if this is already a trade route or someone else built it, we can count is as free
			if(pPlot->IsTradeRoute(m_pPlayer->GetID()) || pPlot->GetPlayerResponsibleForRoute() != m_pPlayer->GetID())
			{
				continue;
			}
			iRoadLength++;
		}
		else
		{
			iRoadLength++;
			iPlotsNeeded++;
		}
	}

	// This is very odd
	if(iRoadLength <= 0 || iPlotsNeeded <= 0)
	{
		return;
	}


	short sValue = -1;
	int iProfit = iGoldForRoute - (iRoadLength * iMaintenancePerTile);
	if(bIndustrialRoute)
	{
		if(iProfit >= 0)
		{
			sValue = MAX_SHORT;
		}
		else if(m_pPlayer->calculateGoldRate() + iProfit >= 0)
		{
			sValue = pTargetCity->getYieldRate(YIELD_PRODUCTION) * GC.getINDUSTRIAL_ROUTE_PRODUCTION_MOD();
		}
		else
		{
			return;
		}
	}
	else if(bMajorMinorConnection)
	{
		sValue = min(GC.getMINOR_CIV_ROUTE_QUEST_WEIGHT() / iPlotsNeeded, MAX_SHORT);
	}
	else // normal route
	{
		// is this route worth building?
		if(iProfit < 0)
		{
			return;
		}

		int iValue = (iGoldForRoute * 100) / (iRoadLength * (iMaintenancePerTile + 1));
		iValue = (iValue * iRoadLength) / iPlotsNeeded;
		sValue = min(iValue, MAX_SHORT);
	}

	pPlot = NULL;
	pNode = GC.GetBuildRouteFinder().GetLastNode();

	int iGameTurn = GC.getGame().getGameTurn();

	while(pNode)
	{
		pPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);
		pNode = pNode->m_pParent;

		if(!pPlot)
		{
			break;
		}

		if(pPlot->getRouteType() == eRoute && !pPlot->IsRoutePillaged())
		{
			continue;
		}

		// if we already know about this plot, continue on
		if(pPlot->GetBuilderAIScratchPadTurn() == iGameTurn && pPlot->GetBuilderAIScratchPadPlayer() == m_pPlayer->GetID())
		{
			if(sValue > pPlot->GetBuilderAIScratchPadValue())
			{
				pPlot->SetBuilderAIScratchPadValue(sValue);
				pPlot->SetBuilderAIScratchPadRoute(eRoute);
			}
			continue;
		}

		// mark nodes and reset values
		pPlot->SetBuilderAIScratchPadTurn(iGameTurn);
		pPlot->SetBuilderAIScratchPadPlayer(m_pPlayer->GetID());
		pPlot->SetBuilderAIScratchPadValue(sValue);
		pPlot->SetBuilderAIScratchPadRoute(eRoute);

		// add nodes that are not in territory to extra list
		// minors should not build out of their borders when they are doing a major/minor connection, only when connecting their two cities
		if(!(m_pPlayer->isMinorCiv() && bMajorMinorConnection))
		{
			if(pPlot->getOwner() != m_pPlayer->GetID())
			{
				m_aiNonTerritoryPlots.push_back(GC.getMap().plotNum(pPlot->getX(), pPlot->getY()));
			}
		}
	}
}

/// Looks at city connections and marks plots that can be added as routes by EvaluateBuilder
void CvBuilderTaskingAI::UpdateRoutePlots(void)
{
	m_aiNonTerritoryPlots.clear();

	// if there are fewer than 2 cities, we don't need to run this function
	if(m_pPlayer->GetCityConnections()->GetNumConnectableCities() < 2)
	{
		return;
	}

	RouteTypes eBestRoute = m_pPlayer->getBestRoute();
	if(eBestRoute == NO_ROUTE)
	{
		return;
	}

	// find a builder, if I don't have a builder, bail!
	CvUnit* pBuilder = NULL;
	CvUnit* pLoopUnit;
	int iLoopUnit;
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		if(pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			pBuilder = pLoopUnit;
			break;
		}
	}

	// If there's no builder, bail!
	if(!pBuilder)
	{
		return;
	}

	// updating plots that are part of the road network
	CvCityConnections* pCityConnections = m_pPlayer->GetCityConnections();

	for(int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(!pkBuild)
		{
			continue;
		}

		RouteTypes eRoute = (RouteTypes)pkBuild->getRoute();
		if(eRoute == NO_ROUTE)
		{
			continue;
		}

		if(GC.getBuildInfo(eBuild)->getTechPrereq() != NO_TECH)
		{
			bool bHasTech = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechPrereq());
			if(!bHasTech)
			{
				continue;
			}
		}

		for(uint uiFirstCityIndex = 0; uiFirstCityIndex < pCityConnections->m_aiCityPlotIDs.size(); uiFirstCityIndex++)
		{
			for(uint uiSecondCityIndex = uiFirstCityIndex + 1; uiSecondCityIndex < pCityConnections->m_aiCityPlotIDs.size(); uiSecondCityIndex++)
			{
				// get the two cities
				CvCity* pFirstCity  = pCityConnections->GetCityFromIndex(uiFirstCityIndex);
				CvCity* pSecondCity = pCityConnections->GetCityFromIndex(uiSecondCityIndex);
				CvCity* pPlayerCapitalCity = NULL;
				CvCity* pTargetCity = NULL;

				if(!pFirstCity || !pSecondCity)
				{
					continue;
				}

				// only need to build roads to the capital
				if(!pFirstCity->isCapital() && !pSecondCity->isCapital())
				{
					continue;
				}

				if(pFirstCity->isCapital() && pFirstCity->getOwner() == m_pPlayer->GetID())
				{
					pPlayerCapitalCity = pFirstCity;
					pTargetCity = pSecondCity;
				}
				else
				{
					pPlayerCapitalCity = pSecondCity;
					pTargetCity = pFirstCity;
				}

				ConnectCities(pPlayerCapitalCity, pTargetCity, eRoute);
			}
		}
	}
}

int CorrectWeight(int iWeight)
{
	if(iWeight < -1000)
	{
		return MAX_INT;
	}
	else
	{
		return iWeight;
	}
}

/// Use the flavor settings to determine what the worker should do
bool CvBuilderTaskingAI::EvaluateBuilder(CvUnit* pUnit, BuilderDirective* paDirectives, UINT uaDirectives, bool bOnlyKeepBest, bool bOnlyEvaluateWorkersPlot)
{
	// number of cities has changed mid-turn, so we need to re-evaluate what workers should do
	if(m_pPlayer->getNumCities() != m_iNumCities)
	{
		Update();
	}

	CvAssertMsg(uaDirectives > 0, "Need more than one directive");

	for(uint ui = 0; ui < uaDirectives; ui++)
	{
		paDirectives[ui].m_eDirective = BuilderDirective::NUM_DIRECTIVES;
	}

	m_aDirectives.clear();

	// check for no brainer bail-outs
	// if the builder is already building something
	if(pUnit->getBuildType() != NO_BUILD)
	{
		paDirectives[0].m_eDirective = BuilderDirective::BUILD_IMPROVEMENT;
		paDirectives[0].m_eBuild = pUnit->getBuildType();
		paDirectives[0].m_sX = pUnit->getX();
		paDirectives[0].m_sY = pUnit->getY();
		//inDirective.m_sGoldCost = 0;
		paDirectives[0].m_sMoveTurnsAway = 0;
		return true;
	}

	m_aiPlots.clear();
	if(bOnlyEvaluateWorkersPlot)
	{
		// can't build on plots others own
		PlayerTypes eOwner = pUnit->plot()->getOwner();
		if(eOwner == m_pPlayer->GetID())
		{
			m_aiPlots.push_back(pUnit->plot()->GetPlotIndex());
		}
	}
	else
	{
		m_aiPlots = m_pPlayer->GetPlots();
	}

	// go through all the plots the player has under their control
	for(uint uiPlotIndex = 0; uiPlotIndex < m_aiPlots.size(); uiPlotIndex++)
	{
		// when we encounter the first plot that is invalid, the rest of the list will be invalid
		if(m_aiPlots[uiPlotIndex] == -1)
		{
			if(m_bLogging)
			{
				CvString strLog = "end of plot list";
				LogInfo(strLog, m_pPlayer);
			}
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(m_aiPlots[uiPlotIndex]);

		if(!ShouldBuilderConsiderPlot(pUnit, pPlot))
		{
			continue;
		}

		// distance weight
		// find how many turns the plot is away
		int iMoveTurnsAway = FindTurnsAway(pUnit, pPlot);
		if(iMoveTurnsAway < 0)
		{
			if(m_bLogging)
			{
				CvString strLog;
				strLog.Format("unitx: %d unity: %d, plotx: %d ploty: %d, can't find path", pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
				LogInfo(strLog, m_pPlayer);
			}

			continue;
		}

		UpdateCurrentPlotYields(pPlot);

		//AddRepairDirectives(pUnit, pPlot, iMoveTurnsAway);
		AddRouteDirectives(pUnit, pPlot, iMoveTurnsAway);
		AddImprovingResourcesDirectives(pUnit, pPlot, iMoveTurnsAway);
		AddImprovingPlotsDirectives(pUnit, pPlot, iMoveTurnsAway);
		AddChopDirectives(pUnit, pPlot, iMoveTurnsAway);
		AddScrubFalloutDirectives(pUnit, pPlot, iMoveTurnsAway);
		// only AIs have permission to remove roads
		if(!m_pPlayer->isHuman())
		{
			//AddRemoveUselessRoadDirectives(pUnit, pPlot, iMoveTurnsAway);
		}
	}

	// we need to evaluate the tiles outside of our territory to build roads
	for(uint ui = 0; ui < m_aiNonTerritoryPlots.size(); ui++)
	{
		//FAssertMsg(!m_pPlayer->isMinorCiv(), "MinorCivs should have no nonterritory plots");
		CvPlot* pPlot = GC.getMap().plotByIndex(m_aiNonTerritoryPlots[ui]);
		CvAssertMsg(pPlot != NULL, "Plot should not be NULL");
		if(!pPlot)
			continue;

		if(bOnlyEvaluateWorkersPlot)
		{
			if(pPlot != pUnit->plot())
			{
				continue;
			}
		}

		if(!ShouldBuilderConsiderPlot(pUnit, pPlot))
		{
			continue;
		}

		// distance weight
		// find how many turns the plot is away
		int iMoveTurnsAway = FindTurnsAway(pUnit, pPlot);
		if(iMoveTurnsAway < 0)
		{
			if(m_bLogging)
			{
				CvString strLog;
				strLog.Format("unitx: %d unity: %d, plotx: %d ploty: %d, Evaluating out of territory plot, can't find path", pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
				LogInfo(strLog, m_pPlayer);
			}

			continue;
		}

		if(m_bLogging)
		{
			CvString strLog;
			strLog.Format("x: %d y: %d, Evaluating out of territory plot", pPlot->getX(), pPlot->getY());
			LogInfo(strLog, m_pPlayer);
		}

		//AddRepairDirectives(pUnit, pPlot, iMoveTurnsAway);
		AddRouteDirectives(pUnit, pPlot, iMoveTurnsAway);
	}

	m_aDirectives.SortItems();

	int iBestWeight = 0;

	int iAssignIndex = 0;
	for(int i = 0; i < m_aDirectives.size(); i++)
	{
		// If this target was far away, we only estimated the time to get there.  We need to be sure we have a real path there
		CvPlot* pTarget = GC.getMap().plot(m_aDirectives.GetElement(i).m_sX, m_aDirectives.GetElement(i).m_sY);
		CvAssertMsg(pTarget != NULL, "Not expecting the target to be NULL");
		if(!pTarget)
			continue;

#if 0	// KWG: We are now always doing a raw estimate when gathering the directives
		int iPlotDistance = plotDistance(pUnit->getX(), pUnit->getY(), pTarget->getX(), pTarget->getY());
		if(iPlotDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE())
#endif
		{
			if(TurnsToReachTarget(pUnit, pTarget) == MAX_INT)
			{
				// No path, need to pick a new directive
				continue;
			}
		}

		if(iBestWeight == 0)
		{
			iBestWeight = m_aDirectives.GetWeight(i);
		}

		if(bOnlyKeepBest)
		{
			int iWeight = m_aDirectives.GetWeight(i);
			if(iWeight < iBestWeight * 3 / 4)
			{
				break;
			}
		}

		BuilderDirective directive = m_aDirectives.GetElement(i);
		paDirectives[iAssignIndex] = directive;
		iAssignIndex++;

		// if we shouldn't copy over any more directives, then break
		if(iAssignIndex >= (int)uaDirectives)
		{
			break;
		}
	}

	if(m_bLogging)
	{
		if(m_aDirectives.size() > 0)
		{
			//LogFlavors(NO_FLAVOR);
		}

		LogDirectives(pUnit);
	}

	//if (m_aDirectives.size() > 0 && iAssignIndex > 0)
	if(iAssignIndex > 0)
	{
		if(m_bLogging)
		{
			LogDirective(paDirectives[0], pUnit, -1, true /*bChosen*/);
		}
		return true;
	}

	return false;
}


/// Evaluating a plot to see if we can build resources there
void CvBuilderTaskingAI::AddImprovingResourcesDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	ImprovementTypes eExistingPlotImprovement = pPlot->getImprovementType();

	// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
	if(eExistingPlotImprovement != NO_IMPROVEMENT && pPlot->HasSpecialImprovement() && !pPlot->IsImprovementPillaged())
	{
		return;
	}

	// check to see if a resource is here. If not, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource == NO_RESOURCE)
	{
		return;
	}

	CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
	if(pkResource == NULL || pkResource->getResourceUsage() == RESOURCEUSAGE_BONUS)
	{
		// evaluate bonus resources as normal improvements
		return;
	}

	// loop through the build types to find one that we can use
	BuildTypes eBuild;
	BuildTypes eOriginalBuild;
	int iBuildIndex;
	for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild == NULL)
			continue;

		eOriginalBuild = eBuild;
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
		{
			continue;
		}

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo == NULL || !pkImprovementInfo->IsImprovementResourceTrade(eResource))
		{
			continue;
		}

		if(eImprovement == eExistingPlotImprovement)
		{
			if(pPlot->IsImprovementPillaged())
			{
				eBuild = m_eRepairBuild;
			}
			else
			{
				// this plot already has the appropriate improvement to use the resource
				break;
			}
		}
		else
		{
			// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
			if (eExistingPlotImprovement != NO_IMPROVEMENT && pPlot->HasSpecialImprovement())
				continue;
		}

		if(!pUnit->canBuild(pPlot, eBuild))
		{
			break;
		}

		BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE;
		int iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS();
		if(eBuild == m_eRepairBuild)
		{
			eDirectiveType = BuilderDirective::REPAIR;
			iWeight = GC.getBUILDER_TASKING_BASELINE_REPAIR();
		}

		iWeight = GetBuildCostWeight(iWeight, pPlot, eBuild);

		// this is to deal with when the plot is already improved with another improvement that doesn't enable the resource
		int iInvestedImprovementTime = 0;
		if(eExistingPlotImprovement != NO_IMPROVEMENT)
		{
			BuildTypes eExistingBuild = NO_BUILD;
			BuildTypes eBuild2 = NO_BUILD;
			for(int iBuildIndex2 = 0; iBuildIndex2 < GC.getNumBuildInfos(); iBuildIndex2++)
			{
				eBuild2 = (BuildTypes)iBuildIndex2;
				CvBuildInfo* pkBuild2 = GC.getBuildInfo(eBuild2);
				if(pkBuild2 && pkBuild2->getImprovement() == eExistingPlotImprovement)
				{
					eExistingBuild = eBuild2;
					break;
				}
			}

			if(eExistingBuild != NO_BUILD)
			{
				iInvestedImprovementTime = pPlot->getBuildTime(eExistingBuild, m_pPlayer->GetID());
			}
		}

		int iBuildTimeWeight = GetBuildTimeWeight(pUnit, pPlot, eBuild, DoesBuildHelpRush(pUnit, pPlot, eBuild), iInvestedImprovementTime + iMoveTurnsAway);
		iWeight += iBuildTimeWeight;
		iWeight = CorrectWeight(iWeight);

		iWeight += GetResourceWeight(eResource, eImprovement, pPlot->getNumResource());
		iWeight = CorrectWeight(iWeight);

		UpdateProjectedPlotYields(pPlot, eBuild);
		int iScore = ScorePlot();
		if(iScore > 0)
		{
			iWeight *= iScore;
			iWeight = CorrectWeight(iWeight);
		}

		{
			CvCity* pLogCity = NULL;
			int iProduction = pPlot->getFeatureProduction(eBuild, pUnit->getOwner(), &pLogCity);
			if(DoesBuildHelpRush(pUnit, pPlot, eBuild))
			{
				iWeight += iProduction; // a nominal benefit for choosing this production

				if(m_bLogging)
				{
					CvString strLog;
					strLog.Format("Helps rush, %d", iProduction);
					LogInfo(strLog, m_pPlayer);
				}
			}
		}

		if(iWeight <= 0)
		{
			continue;
		}

		BuilderDirective directive;
		directive.m_eDirective = eDirectiveType;
		directive.m_eBuild = eBuild;
		directive.m_eResource = eResource;
		directive.m_sX = pPlot->getX();
		directive.m_sY = pPlot->getY();
		//directive.m_iGoldCost = m_pPlayer->getBuildCost(pPlot, eBuild);
		directive.m_sMoveTurnsAway = iMoveTurnsAway;

		if(m_bLogging)
		{
			CvString strTemp;
			strTemp.Format("%d, Build Time Weight, %d, Weight, %d", pUnit->GetID(), iBuildTimeWeight, iWeight);
			LogInfo(strTemp, m_pPlayer);
		}

		m_aDirectives.push_back(directive, iWeight);
	}
}

/// Evaluating a plot to determine what improvement could be best there
void CvBuilderTaskingAI::AddImprovingPlotsDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	ImprovementTypes eExistingImprovement = pPlot->getImprovementType();

	// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
	if(eExistingImprovement != NO_IMPROVEMENT && pPlot->HasSpecialImprovement() && !pPlot->IsImprovementPillaged())
	{
		return;
	}

	// if it's not within a city radius
	if(!pPlot->isWithinTeamCityRadius(pUnit->getTeam()))
	{
		return;
	}

	// check to see if a non-bonus resource is here. if so, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource != NO_RESOURCE)
	{
		if(GC.getResourceInfo(eResource)->getResourceUsage() != RESOURCEUSAGE_BONUS)
		{
			return;
		}
	}

	// celtic rule: if this is a forest tile next to a city, do not improve this tile with a normal improvement
	if (m_pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest() && eExistingImprovement == NO_IMPROVEMENT)
	{
		CvCity* pNextCity = pPlot->GetAdjacentCity();
		if (pNextCity && pNextCity->getOwner() == m_pPlayer->GetID())
		{
			if (pPlot->getFeatureType() == FEATURE_FOREST)
			{
				return;
			}
		}
	}

	CvCity* pCity = GetWorkingCity(pPlot);
	if(!pCity)
	{
		return;
	}

	// loop through the build types to find one that we can use
	BuildTypes eBuild;
	BuildTypes eOriginalBuildType;
	int iBuildIndex;
	for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		eBuild = (BuildTypes)iBuildIndex;
		eOriginalBuildType = eBuild;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild == NULL)
		{
			continue;
		}

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
		{
			continue;
		}

		// if this improvement has a defense modifier, ignore it for now
		CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
		if(pImprovement->GetDefenseModifier() > 0)
		{
			continue;
		}

		// for bonus resources, check to see if this is the improvement that connects it
		if(eResource != NO_RESOURCE)
		{
			if(!pImprovement->IsImprovementResourceTrade(eResource))
			{
				continue;
			}
		}

		if(eImprovement == pPlot->getImprovementType())
		{
			if(pPlot->IsImprovementPillaged())
			{
				eBuild = m_eRepairBuild;
			}
			else
			{
				continue;
			}
		}
		else
		{
			// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
			if (eExistingImprovement != NO_IMPROVEMENT)
			{
				if (pPlot->HasSpecialImprovement() || GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION))
				{
					continue;
				}
			}
		}

		// Only check to make sure our unit can build this after possibly switching this to a repair build in the block of code above
		if(!pUnit->canBuild(pPlot, eBuild))
		{
			continue;
		}

		bool bWillRemoveForestOrJungle = false;
		FeatureTypes eFeature = pPlot->getFeatureType();
		if(eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE)
		{
			if(pkBuild->isFeatureRemove(eFeature))
			{
				bWillRemoveForestOrJungle = true;
			}
		}

		// special case for Dutch
		if (m_bKeepMarshes && eFeature == FEATURE_MARSH)
		{
			if (pkBuild->isFeatureRemove(FEATURE_MARSH))
			{
				continue;
			}
		}

		if(GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_LEAVE_FORESTS))
		{
			if(eFeature != NO_FEATURE)
			{
				if(pkBuild->isFeatureRemove(eFeature))
				{
					continue;
				}
			}
		}

		UpdateProjectedPlotYields(pPlot, eBuild);
		int iScore = ScorePlot();

		// if we're going backward, bail out!
		if(iScore <= 0)
		{
			continue;
		}

		BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_IMPROVEMENT;
		int iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS();
		if(eBuild == m_eRepairBuild)
		{
			eDirectiveType = BuilderDirective::REPAIR;
			iWeight = GC.getBUILDER_TASKING_BASELINE_REPAIR();
		}
		else if(pImprovement->GetYieldChange(YIELD_CULTURE) > 0)
		{
			iWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_CULTURE() * GC.getImprovementInfo(eImprovement)->GetYieldChange(YIELD_CULTURE);
			int iAdjacentCulture = pImprovement->GetCultureAdjacentSameType();

			if(iAdjacentCulture > 0)
			{
				iScore *= (1 + pPlot->ComputeCultureFromAdjacentImprovement(*pImprovement, eImprovement));
			}
		}

		iWeight = GetBuildCostWeight(iWeight, pPlot, eBuild);
		int iBuildTimeWeight = GetBuildTimeWeight(pUnit, pPlot, eBuild, DoesBuildHelpRush(pUnit, pPlot, eBuild), iMoveTurnsAway);
		iWeight += iBuildTimeWeight;
		iWeight *= iScore;

		if(m_pPlayer->GetPlayerTraits()->IsMoveFriendlyWoodsAsRoad() && bWillRemoveForestOrJungle)
		{
			iWeight = iWeight / 4;
		}

		iWeight = CorrectWeight(iWeight);

		BuilderDirective directive;

		directive.m_eDirective = eDirectiveType;
		directive.m_eBuild = eBuild;
		directive.m_eResource = NO_RESOURCE;
		directive.m_sX = pPlot->getX();
		directive.m_sY = pPlot->getY();
		directive.m_sMoveTurnsAway = iMoveTurnsAway;

		if(m_bLogging)
		{
			CvString strTemp;
			//strTemp.Format("Build Time Weight, %d, Weight, %d", iBuildTimeWeight, iWeight);
			strTemp.Format("Weight, %d", iWeight);
			LogInfo(strTemp, m_pPlayer);
		}

		m_aDirectives.push_back(directive, iWeight);
	}
}

/// Adds a directive if the unit can construct a road in the plot
void CvBuilderTaskingAI::AddRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	RouteTypes eBestRouteType = m_pPlayer->getBestRoute();

	// if the player can't build a route, bail out!
	if(eBestRouteType == NO_ROUTE)
	{
		return;
	}

	if(pPlot->getRouteType() == eBestRouteType && !pPlot->IsRoutePillaged())
	{
		return;
	}

	// the plot was not flagged this turn, so ignore
	bool bShouldRoadThisTile = (pPlot->GetBuilderAIScratchPadTurn() == GC.getGame().getGameTurn()) && (pPlot->GetBuilderAIScratchPadPlayer() == pUnit->getOwner());
	if(!bShouldRoadThisTile)
	{
		return;
	}

	// find the route build
	BuildTypes eRouteBuild = NO_BUILD;
	if(pPlot->IsRoutePillaged())
	{
		eRouteBuild = m_eRepairBuild;
	}
	else
	{
		RouteTypes eRoute = pPlot->GetBuilderAIScratchPadRoute();
		for(int i = 0; i < GC.getNumBuildInfos(); i++)
		{
			BuildTypes eBuild = (BuildTypes)i;
			CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
			if(pkBuild && pkBuild->getRoute() == eRoute)
			{
				eRouteBuild = eBuild;
				break;
			}
		}
	}

	if(eRouteBuild == NO_BUILD)
	{
		return;
	}

	CvUnitEntry& kUnitInfo = pUnit->getUnitInfo();
	if(!kUnitInfo.GetBuilds(eRouteBuild))
	{
		return;
	}

	if(GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_LEAVE_FORESTS))
	{
		FeatureTypes eFeature = pPlot->getFeatureType();
		if(eFeature != NO_FEATURE)
		{
			CvBuildInfo* pkBuild = GC.getBuildInfo(eRouteBuild);
			if(pkBuild && pkBuild->isFeatureRemove(eFeature))
			{
				return;
			}
		}
	}

	int iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_ROUTES();
	BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_ROUTE;
	if(eRouteBuild == m_eRepairBuild)
	{
		iWeight = GC.getBUILDER_TASKING_BASELINE_REPAIR();
		eDirectiveType = BuilderDirective::REPAIR;
	}

	// int iTurnsAway = FindTurnsAway(pUnit, pPlot);
	iWeight = iWeight / (iMoveTurnsAway/*iTurnsAway*/ + 1);
	iWeight = GetBuildCostWeight(iWeight, pPlot, eRouteBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, eRouteBuild, false, iMoveTurnsAway);
	iWeight *= pPlot->GetBuilderAIScratchPadValue();
	iWeight = CorrectWeight(iWeight);

	BuilderDirective directive;
	directive.m_eDirective = eDirectiveType;
	directive.m_eBuild = eRouteBuild;
	directive.m_eResource = NO_RESOURCE;
	directive.m_sX = pPlot->getX();
	directive.m_sY = pPlot->getY();
	directive.m_sMoveTurnsAway = iMoveTurnsAway;

	if(m_bLogging)
	{
		CvString strTemp;
		strTemp.Format("AddRouteDirectives, adding, x: %d y: %d, Weight, %d", pPlot->getX(), pPlot->getY(), iWeight);
		LogInfo(strTemp, m_pPlayer);
	}

	m_aDirectives.push_back(directive, iWeight);
}

/// Determines if the builder should "chop" the feature in the tile
void CvBuilderTaskingAI::AddChopDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	// if it's not within a city radius
	if(!pPlot->isWithinTeamCityRadius(pUnit->getTeam()))
	{
		return;
	}

	if(pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		return;
	}

	if(GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_LEAVE_FORESTS))
	{
		return;
	}

	// check to see if a resource is here. If so, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource != NO_RESOURCE)
	{
		return;
	}

	CvCity* pCity = GetWorkingCity(pPlot);
	if(!pCity)
	{
		return;
	}

	if(pCity->GetCityCitizens()->IsWorkingPlot(pPlot))
	{
		return;
	}

	FeatureTypes eFeature = pPlot->getFeatureType();
	if(eFeature == NO_FEATURE)
	{
		// no feature in this tile, so bail
		return;
	}

	// celtic rule: if this is a forest tile next to a city, do not chop the trees
	if (m_pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest())
	{
		CvCity* pAdjacentCity = pPlot->GetAdjacentCity();
		if (pAdjacentCity && pAdjacentCity->getOwner() == m_pPlayer->GetID())
		{
			if (eFeature == FEATURE_FOREST)
			{
				return;
			}
		}
	}

	BuildTypes eChopBuild = NO_BUILD;
	for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(NULL != pkBuild && pkBuild->getImprovement() == NO_IMPROVEMENT && pkBuild->isFeatureRemove(eFeature) && pkBuild->getFeatureProduction(eFeature) > 0 && pUnit->canBuild(pPlot, eBuild))
		{
			eChopBuild = eBuild;
			break;
		}
	}

	if(eChopBuild == NO_BUILD)
	{
		// we couldn't find a build that removed the feature without a production benefit
		return;
	}

	pCity = NULL;
	int iProduction = pPlot->getFeatureProduction(eChopBuild, pUnit->getOwner(), &pCity);

	if(!DoesBuildHelpRush(pUnit, pPlot, eChopBuild))
	{
		return;
	}

	int iWeight = GC.getBUILDER_TASKING_BASELINE_REPAIR();
	//int iTurnsAway = FindTurnsAway(pUnit, pPlot);
	iWeight = iWeight / (iMoveTurnsAway/*iTurnsAway*/ + 1);
	iWeight = GetBuildCostWeight(iWeight, pPlot, eChopBuild);
	int iBuildTimeWeight = GetBuildTimeWeight(pUnit, pPlot, eChopBuild, false, iMoveTurnsAway);
	iWeight += iBuildTimeWeight;
	iWeight *= iProduction; // times the amount that the plot produces from the chopping

	int iYieldDifferenceWeight = 0;
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		// calculate natural yields
		int iPreviousYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->getTeam());
		int iNewYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->getTeam(), true /*bIgnoreFeature*/);
		int iDeltaYield = iNewYield - iPreviousYield;

		if(iDeltaYield == 0)
		{
			continue;
		}

		for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
		{
			switch(ui)
			{
			case YIELD_FOOD:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GROWTH")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * GC.getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD();
				}
				break;
			case YIELD_PRODUCTION:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_PRODUCTION")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * GC.getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION();
				}
				break;
			case YIELD_GOLD:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GOLD")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * GC.getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD();
				}
				break;
			case YIELD_SCIENCE:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * GC.getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE();
				}
				break;
			case YIELD_CULTURE:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CULTURE")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * GC.getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE();
				}
				break;
			case YIELD_FAITH:
				//if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
				//{
				//	iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * GC.getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE();
				//}
				break;
			}
		}
	}

	// if we are going backwards, bail
	if(iYieldDifferenceWeight < 0)
	{
		return;
	}

	iWeight += iYieldDifferenceWeight;

	if (m_pPlayer->GetPlayerTraits()->IsMoveFriendlyWoodsAsRoad() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		iWeight = iWeight / 4;
	}
	if (m_pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest() && eFeature == FEATURE_FOREST)
	{
		iWeight = iWeight / 4;
	}
	// this doesn't actually help them, but adds some historical flavor
	if (m_pPlayer->GetPlayerTraits()->IsEmbarkedAllWater() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		iWeight = iWeight * 2;
	}

	iWeight = CorrectWeight(iWeight);

	if(iWeight > 0)
	{
		BuilderDirective directive;
		directive.m_eDirective = BuilderDirective::CHOP;
		directive.m_eBuild = eChopBuild;
		directive.m_eResource = NO_RESOURCE;
		directive.m_sX = pPlot->getX();
		directive.m_sY = pPlot->getY();
		//directive.m_iGoldCost = m_pPlayer->getBuildCost(pPlot, eChopBuild);
		directive.m_sMoveTurnsAway = iMoveTurnsAway;

		if(m_bLogging)
		{
			CvString strTemp;
			strTemp.Format("BuildTimeWeight, %d, Weight, %d", iBuildTimeWeight, iWeight);
			LogInfo(strTemp, m_pPlayer);
		}

		m_aDirectives.push_back(directive, iWeight);
	}
	else
	{
		if(m_bLogging)
		{
			LogInfo("Add chop directives, Weight is zero!", m_pPlayer);
		}
	}
}

void CvBuilderTaskingAI::AddRemoveUselessRoadDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	// if it's not within a city radius
	if(!pPlot->isWithinTeamCityRadius(pUnit->getTeam()))
	{
		return;
	}

	// don't try to remove the route under the city
	if(pPlot->isCity())
	{
		return;
	}

	// nothing here to remove
	if(pPlot->getRouteType() == NO_ROUTE)
	{
		return;
	}

	// flagged this turn means this is a valid route plot
	if(pPlot->GetBuilderAIScratchPadTurn() == GC.getGame().getGameTurn() && pPlot->GetBuilderAIScratchPadPlayer() == pUnit->getOwner())
	{
		return;
	}

	BuildTypes eRemoveRouteBuild = NO_BUILD;
	for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(NULL != pkBuild && pkBuild->IsRemoveRoute() && pUnit->canBuild(pPlot, eBuild))
		{
			eRemoveRouteBuild = eBuild;
			break;
		}
	}


	if(eRemoveRouteBuild == NO_BUILD)
	{
		return;
	}

	// evaluate if removing the road helps anything
	ImprovementTypes eImprovement = pPlot->getImprovementType();
	BuildTypes eBuild = NO_BUILD;
	if(eImprovement != NO_IMPROVEMENT)
	{
		eBuild = GetBuildTypeFromImprovement(eImprovement);
	}

	//int iWeight = GetWeightFromPlotYields(m_pPlayer, pPlot, eBuild, NULL, NO_YIELD, 100, NO_ROUTE) - GetWeightFromPlotYields(m_pPlayer, pPlot, eBuild, NULL, NO_YIELD, 10);
	int iWeight = 0;
	iWeight = CorrectWeight(iWeight);

	if(iWeight > 0)
	{
		BuilderDirective directive;
		directive.m_eDirective = BuilderDirective::REMOVE_ROAD;
		directive.m_eBuild = eRemoveRouteBuild;
		directive.m_eResource = NO_RESOURCE;
		directive.m_sX = pPlot->getX();
		directive.m_sY = pPlot->getY();
		directive.m_sMoveTurnsAway = iMoveTurnsAway;

		m_aDirectives.push_back(directive, iWeight);
	}

}

// Everything means less than zero, hey
void CvBuilderTaskingAI::AddScrubFalloutDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	if(m_eFalloutFeature == NO_FEATURE || m_eFalloutRemove == NO_BUILD)
	{
		return;
	}

	CvCity* pCity = GetWorkingCity(pPlot);
	if(!pCity)
	{
		return;
	}

	if(pPlot->getFeatureType() == m_eFalloutFeature && pUnit->canBuild(pPlot, m_eFalloutRemove))
	{
		int iWeight = GC.getBUILDER_TASKING_BASELINE_REPAIR();
		//int iTurnsAway = FindTurnsAway(pUnit, pPlot);
		iWeight = iWeight / (iMoveTurnsAway/*iTurnsAway*/ + 1);
		iWeight = GetBuildCostWeight(iWeight, pPlot, m_eFalloutRemove);
		int iBuildTimeWeight = GetBuildTimeWeight(pUnit, pPlot, m_eFalloutRemove, false, iMoveTurnsAway);
		iWeight += iBuildTimeWeight;

		BuilderDirective directive;
		directive.m_eDirective = BuilderDirective::CHOP;
		directive.m_eBuild = m_eFalloutRemove;
		directive.m_eResource = NO_RESOURCE;
		directive.m_sX = pPlot->getX();
		directive.m_sY = pPlot->getY();
		directive.m_sMoveTurnsAway = iMoveTurnsAway;
		m_aDirectives.push_back(directive, iWeight);
	}
}


/// Evaluates all the circumstances to determine if the builder can and should evaluate the given plot
bool CvBuilderTaskingAI::ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot)
{
	// if plot is impassable, bail!
	if(pPlot->isImpassable() || pPlot->isMountain())
	{
		if(m_bLogging)
		{
			CvString strLog;
			strLog.Format("x: %d y: %d,,Impassable tile. Toss out", pPlot->getX(), pPlot->getY());
			LogInfo(strLog, m_pPlayer);
		}
		return false;
	}

	// can't build on plots others own (unless inside a minor)
	PlayerTypes eOwner = pPlot->getOwner();
	if(eOwner != NO_PLAYER && eOwner != m_pPlayer->GetID() && !GET_PLAYER(eOwner).isMinorCiv())
	{
		return false;
	}

	// workers should not be able to work in plots that do not match their default domain
	switch(pUnit->getDomainType())
	{
	case DOMAIN_LAND:
		if(pPlot->isWater())
		{
			return false;
		}
		break;
	case DOMAIN_SEA:
		if(!pPlot->isWater())
		{
			return false;
		}
		break;
	default:
		break;
	}

	// need more planning for amphibious units
	// we should include here the ability for work boats to cross to other areas with cities
	if(pPlot->area() != pUnit->area())
	{
		bool bCanCrossToNewArea = false;

		if(pUnit->getDomainType() == DOMAIN_SEA)
		{
			if(pPlot->isAdjacentToArea(pUnit->area()))
			{
				bCanCrossToNewArea = true;
			}
		}
		else
		{
			if(pUnit->CanEverEmbark())
			{
				bCanCrossToNewArea = true;
			}
		}

		if(!bCanCrossToNewArea)
		{
			if(m_bLogging)
			{
				CvString strLog;
				strLog.Format("unitx: %d unity: %d, plotx: %d ploty: %d, plot area: %d, unit area: %d,,Plot areas don't match and can't embark", pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY(), pPlot->area(), pUnit->area());
				if(pPlot->isWater() == pUnit->plot()->isWater())
				{
					strLog += ",This is weird";
				}
				else
				{
					strLog += ",This is normal";
				}
				LogInfo(strLog, m_pPlayer);
			}
			return false;
		}
	}

	// check to see if someone already has a mission here
	if(pUnit->GetMissionAIPlot() != pPlot)
	{
		if(m_pPlayer->AI_plotTargetMissionAIs(pPlot, MISSIONAI_BUILD) > 0)
		{
			if(m_bLogging)
			{
				CvString strLog;
				strLog.Format("x: %d y: %d,,Somebody has a mission here, ", pPlot->getX(), pPlot->getY());
				LogInfo(strLog, m_pPlayer, true);
			}

			return false;
		}
	}

	if(m_pPlayer->GetPlotDanger(*pPlot) > 0)
	{
		if(m_bLogging)
		{
			CvString strLog;
			strLog.Format("plotX: %d plotY: %d, danger: %d,, bailing due to danger", pPlot->getX(), pPlot->getY(), m_pPlayer->GetPlotDanger(*pPlot));
			LogInfo(strLog, m_pPlayer, true);
		}

		return false;
	}

	if(!pUnit->atPlot(*pPlot) && pPlot->getNumFriendlyUnitsOfType(pUnit) >= GC.getPLOT_UNIT_LIMIT())
	{
		if(m_bLogging)
		{
			CvString strLog;
			strLog.Format("plotX: %d plotY: %d,, this tile is full with another unit. bailing!", pPlot->getX(), pPlot->getY());
			LogInfo(strLog, m_pPlayer, true);
		}

		return false;
	}

	return true;
}

/// Determines if the builder can get to the plot. Returns -1 if no path can be found, otherwise it returns the # of turns to get there
int CvBuilderTaskingAI::FindTurnsAway(CvUnit* pUnit, CvPlot* pPlot)
{
	// If this plot is far away, we'll just use its distance as an estimate of the time to get there (to avoid hitting the path finder)
	// We'll be sure to check later to make sure we have a real path before we execute this
	if(pUnit->getDomainType() == DOMAIN_LAND && pUnit->plot()->area() != pPlot->area() && !pUnit->CanEverEmbark())
	{
		return -1;
	}

	int iPlotDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
#if 1
	// Always return the raw distance
	return iPlotDistance;
#else
	if(iPlotDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE())
	{
		return iPlotDistance;
	}
	else
	{
		int iResult = TurnsToReachTarget(pUnit, pPlot);
		if(iResult == MAX_INT)
		{
			return -1;
		}

		return iResult;
	}
#endif
}

/// Get the weight determined by the cost of building the item
int CvBuilderTaskingAI::GetBuildCostWeight(int iWeight, CvPlot* pPlot, BuildTypes eBuild)
{
	int iBuildCost = m_pPlayer->getBuildCost(pPlot, eBuild);
	if(iBuildCost > 0)
	{
		iWeight = (iWeight * 100) / iBuildCost;
	}
	else
	{
		//if (m_bLogging)
		//{
		//	LogInfo("Build cost is zero", m_pPlayer);
		//}
		iWeight = (iWeight * 100);
	}
	return iWeight;
}

/// Get the weight determined by the building time of the item
int CvBuilderTaskingAI::GetBuildTimeWeight(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild, bool bIgnoreFeatureTime, int iAdditionalTime)
{
	// if we need to repair this plot, replace the build with a repair build
	if((GC.getBuildInfo(eBuild)->getImprovement() != NO_IMPROVEMENT && pPlot->IsImprovementPillaged()) || (GC.getBuildInfo(eBuild)->getRoute() != NO_ROUTE && pPlot->IsRoutePillaged()))
	{
		// find a repair directive to replace
		// find the repair build
		eBuild = m_eRepairBuild;
	}

	int iBuildTimeNormal = pPlot->getBuildTime(eBuild, m_pPlayer->GetID());
	int iBuildTurnsLeft = pPlot->getBuildTurnsLeft(eBuild, m_pPlayer->GetID(), pUnit->workRate(true), pUnit->workRate(true));
	int iBuildTime = min(iBuildTimeNormal, iBuildTurnsLeft);
	if(iBuildTime <= 0)
	{
		iBuildTime = 1;
	}

	if(bIgnoreFeatureTime)
	{
		if(pPlot->getFeatureType() != NO_FEATURE)
		{
			iBuildTime -= GC.getBuildInfo(eBuild)->getFeatureTime(pPlot->getFeatureType());
		}
	}

	iBuildTime += iAdditionalTime;

	return 10000 / iBuildTime;
}

/// Return the weight of this resource
int CvBuilderTaskingAI::GetResourceWeight(ResourceTypes eResource, ImprovementTypes eImprovement, int iQuantity)
{
	int iWeight = 0;
	CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
	if(pkResource == NULL)
	{
		return 0;
	}

	for(int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		int iResourceFlavor = pkResource->getFlavorValue((FlavorTypes)i);
		int iPersonalityFlavorValue = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)i);
		int iResult = iResourceFlavor * iPersonalityFlavorValue;

		if(iResult > 0)
		{
			iWeight += iResult;
		}

		int iImprovementFlavor = 1;
		if(eImprovement != NO_IMPROVEMENT)
		{
			iImprovementFlavor = GC.getImprovementInfo(eImprovement)->GetFlavorValue(i);
		}

		int iUsableByCityWeight = iPersonalityFlavorValue * iImprovementFlavor;
		if(iUsableByCityWeight > 0)
		{
			iWeight += iUsableByCityWeight;
		}
	}

	// if the empire is unhappy (or close to it) and this is a luxury resource the player doesn't have, provide a super bonus to getting it
	if(pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
	{
		int iModifier = GC.getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE() * pkResource->getHappiness();

		//if (m_pPlayer->IsEmpireUnhappy() || m_pPlayer->GetExcessHappiness() <= 2)
		//{
		//}
		if(m_pPlayer->getNumResourceAvailable(eResource) == 0)
		{
			// full bonus
		}
		else
		{
			iModifier = (iModifier * 3) / 4; // 3/4ths the awesome bonus, so that we pick up extra resources 
		}

		iWeight *= iModifier;
	}
	else if(pkResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC && pkResource->getTechCityTrade())
	{
		bool bHasTech = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes) pkResource->getTechCityTrade());
		if(bHasTech)
		{
			// measure quantity
			int iMultiplyingAmount = iQuantity * 200;

			// if we don't have any currently available
			if(m_pPlayer->getNumResourceAvailable(eResource) == 0)
			{
				// if we have some of the strategic resource, but all is used
				if(m_pPlayer->getNumResourceUsed(eResource) > 0)
				{
					iMultiplyingAmount *= 4;
				}
				else
				{
					// if we don't have any of it
					iMultiplyingAmount *= 4;
				}
			}

			iWeight *= iMultiplyingAmount;
		}
	}

	return iWeight;
}

/// Determine if an improvement will increase any of the outputs of the plot
bool CvBuilderTaskingAI::IsImprovementBeneficial(CvPlot* pPlot, const CvBuildInfo& kBuild, YieldTypes eYield, bool bIsBreakEvenOK)
{
	const ImprovementTypes eImprovement = (ImprovementTypes)kBuild.getImprovement();

	const FeatureTypes eFeature = pPlot->getFeatureType();

	bool bFeatureNeedsRemove = false;

	if(eFeature != NO_FEATURE)
	{
		if(kBuild.isFeatureRemove(eFeature))
		{
			bFeatureNeedsRemove = true;
		}
	}

	CvImprovementEntry* pkImprovementInfo = NULL;
	if(eImprovement != NO_IMPROVEMENT)
	{
		pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	}
	CvAssert(pkImprovementInfo);

	//This can technically happen if a build as passed in that doesn't contain an improvement..
	//Returning false isn't the ideal error approach, but there's nothing better to do atm.
	if(pkImprovementInfo == NULL)
	{
		return false;
	}

	CvRouteInfo* pkPlotRouteInfo = NULL;
	if(pPlot->getRouteType() != NO_ROUTE)
	{
		pkPlotRouteInfo = GC.getRouteInfo(pPlot->getRouteType());
	}


	int aiNaturalYieldTypes[NUM_YIELD_TYPES];
	int aiImprovedYieldTypes[NUM_YIELD_TYPES];

	// hacky solution for the great artist's landmark.
	// if an improvement generates culture, then it is beneficial
	if(pkImprovementInfo->GetYieldChange(YIELD_CULTURE) > 0)
	{
		return true;
	}

	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		// calculate natural yields
		aiNaturalYieldTypes[ui] = 0;
		aiNaturalYieldTypes[ui] = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->getTeam());

		// calculate improvement yields
		aiImprovedYieldTypes[ui] = 0;

		bool bIgnoreFeature = false;
		if(bFeatureNeedsRemove)
		{
			bIgnoreFeature = true;
		}

		aiImprovedYieldTypes[ui] = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->getTeam(), bIgnoreFeature);
		if(pkPlotRouteInfo)
		{
			aiImprovedYieldTypes[ui] += pkPlotRouteInfo->getYieldChange(ui);
		}
		aiImprovedYieldTypes[ui] += pPlot->calculateImprovementYieldChange(eImprovement, (YieldTypes)ui, m_pPlayer->GetID(), false /*bOptimal*/);
	}

	if(eYield == NO_YIELD)
	{
		// if any of the yields are increased from their natural state, then the improvement is considered a success
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			if(bIsBreakEvenOK)
			{
				if(aiImprovedYieldTypes[ui] >= aiNaturalYieldTypes[ui])
				{
					return true;
				}
			}
			else
			{
				if(aiImprovedYieldTypes[ui] > aiNaturalYieldTypes[ui])
				{
					return true;
				}
			}
		}
	}
	else
	{
		if(bIsBreakEvenOK)
		{
			if(aiImprovedYieldTypes[eYield] >= aiNaturalYieldTypes[eYield])
			{
				return true;
			}
		}
		else
		{
			if(aiImprovedYieldTypes[eYield] > aiNaturalYieldTypes[eYield])
			{
				return true;
			}
		}
	}

	return false;
}

/// Get this city that can interact with this plot
CvCity* CvBuilderTaskingAI::GetWorkingCity(CvPlot* pPlot)
{
	CvCity* pCity = NULL;
	if(pPlot->getWorkingCity())
	{
		pCity = pPlot->getWorkingCity();
	}
	else
	{
		CvCity* pLoopCity;
		int iLoop;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity->GetCityCitizens()->IsCanWork(pPlot))
			{
				pCity = pLoopCity;
				break;
			}
		}
	}

	return pCity;
}

/// Does this city want to rush a unit?
bool CvBuilderTaskingAI::DoesBuildHelpRush(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild)
{
	CvCity* pCity = NULL;
	int iProduction = pPlot->getFeatureProduction(eBuild, pUnit->getOwner(), &pCity);
	if(iProduction <= 0)
	{
		return false;
	}

	if(!pCity)
	{
		// this chop does not benefit any city
		return false;
	}

	if(pCity->getOrderQueueLength() <= 0)
	{
		// nothing in the build queue
		return false;
	}

	if(!(pCity->getOrderFromQueue(0)->bRush))
	{
		// this order should not be rushed
		return false;
	}

	return true;
}

int CvBuilderTaskingAI::ScorePlot()
{
	if(!m_pTargetPlot)
	{
		return -1;
	}

	CvCity* pCity = m_pTargetPlot->getWorkingCity();
	if(!pCity)
	{
		return -1;
	}

	CvCityStrategyAI* pCityStrategy = pCity->GetCityStrategyAI();
	if(!pCityStrategy)
	{
		return -1;
	}

	int iScore = 0;
	bool bAnyNegativeMultiplier = false;
	YieldTypes eFocusYield = pCityStrategy->GetFocusYield();
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		int iMultiplier = pCityStrategy->GetYieldDeltaTimes100((YieldTypes)ui);
		int iAbsMultiplier = abs(iMultiplier);
		int iYieldDelta = m_aiProjectedPlotYields[ui] - m_aiCurrentPlotYields[ui];

		// the multiplier being lower than zero means that we need more of this resource
		if(iMultiplier < 0)
		{
			bAnyNegativeMultiplier = true;
			if(iYieldDelta > 0)  // this would be an improvement to the yield
			{
				iScore += m_aiProjectedPlotYields[ui] * iAbsMultiplier;
			}
			else if(iYieldDelta < 0)  // the yield would go down
			{
				iScore += iYieldDelta * iAbsMultiplier;
			}
		}
		else
		{
			if(iYieldDelta >= 0)
			{
				iScore += m_aiProjectedPlotYields[ui]; // provide a nominal score to plots that improve anything
			}
			else if(iYieldDelta < 0)
			{
				iScore += iYieldDelta * iAbsMultiplier;
			}
		}
	}

	if(!bAnyNegativeMultiplier && eFocusYield != NO_YIELD)
	{
		int iYieldDelta = m_aiProjectedPlotYields[eFocusYield] - m_aiCurrentPlotYields[eFocusYield];
		if(iYieldDelta > 0)
		{
			iScore += m_aiProjectedPlotYields[eFocusYield] * 100;
		}
	}

	if (pCity->isCapital()) // this is our capital and needs emphasis
	{
		iScore *= 8;
	}
	else if (pCity->IsOriginalCapital()) // this was a particularly good city and needs a little emphasis
	{
		iScore *= 2;
	}

	return iScore;
}

BuildTypes CvBuilderTaskingAI::GetBuildTypeFromImprovement(ImprovementTypes eImprovement)
{
	for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);

		if(NULL != pkBuild && eImprovement == (ImprovementTypes)pkBuild->getImprovement())
		{
			return eBuild;
		}
	}

	return NO_BUILD;
}

BuildTypes CvBuilderTaskingAI::GetRepairBuild(void)
{
	for(int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);

		if(NULL != pkBuild && pkBuild->isRepair())
		{
			return eBuild;
		}
	}

	return NO_BUILD;
}

FeatureTypes CvBuilderTaskingAI::GetFalloutFeature(void)
{
	CvFeatureInfo* pFeatureInfo = NULL;
	for(int iFeature = 0; iFeature < GC.getNumFeatureInfos(); iFeature++)
	{
		pFeatureInfo = GC.getFeatureInfo((FeatureTypes)iFeature);
		if(!pFeatureInfo)
		{
			continue;
		}

		bool bIsFalloutFeature = true;
		for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			switch(iYield)
			{
			case YIELD_FOOD:
			case YIELD_PRODUCTION:
			case YIELD_GOLD:
			case YIELD_SCIENCE:
			case YIELD_CULTURE:
			case YIELD_FAITH:
				if(pFeatureInfo->getYieldChange(iYield) >= 0)
				{
					bIsFalloutFeature = false;
				}
				break;
			}
		}

		if(bIsFalloutFeature)
		{
			return (FeatureTypes)iFeature;
		}
	}
	return NO_FEATURE;
}

BuildTypes CvBuilderTaskingAI::GetFalloutRemove(void)
{
	FeatureTypes eFalloutFeature = m_eFalloutFeature;
	if(eFalloutFeature == NO_FEATURE)
	{
		eFalloutFeature = GetFalloutFeature();
	}

	if(eFalloutFeature == NO_FEATURE)
	{
		return NO_BUILD;
	}

	for(int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
	{
		BuildTypes eBuild = (BuildTypes)iBuild;
		CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);
		if(NULL != pBuildInfo && pBuildInfo->isFeatureRemove(eFalloutFeature))
		{
			return eBuild;
		}
	}

	return NO_BUILD;
}

/// Central logging repository!
void CvBuilderTaskingAI::LogInfo(CvString strNewLogStr, CvPlayer* pPlayer, bool /*bWriteToOutput*/)
{
	if(!(GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging()))
	{
		return;
	}

	// Open the log file
	CvString strFileName = "BuilderTaskingLog.csv";
	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

	CvString strLog, strTemp;

	CvString strPlayerName;
	strPlayerName = pPlayer->getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
	strLog += strTemp;
	strLog += strNewLogStr;
	pLog->Msg(strLog);
}

/// Log flavor information out
void CvBuilderTaskingAI::LogFlavors(FlavorTypes eFlavor)
{
	if(!m_bLogging)
	{
		return;
	}

	// Open the log file
	CvString strLog;
	CvString strTemp;

	// Dump out the setting for each flavor
	if(eFlavor == NO_FLAVOR)
	{
		for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
		{
			strLog.clear();
			strTemp.Format("Flavor, %s, %d,", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)iI));
			strLog += strTemp;
			LogInfo(strLog, m_pPlayer);
		}
	}
	else
	{
		strLog.clear();
		strTemp.Format("Flavor, %s, %d,", GC.getFlavorTypes(eFlavor).GetCString(), m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor));
		strLog += strTemp;
		LogInfo(strLog, m_pPlayer);
	}
}

/// Logs all the directives for the unit
void CvBuilderTaskingAI::LogDirectives(CvUnit* pUnit)
{
	if(!m_bLogging)
	{
		return;
	}

	if(m_aDirectives.size() > 0)
	{
		for(int i = 0; i < m_aDirectives.size(); i++)
		{
			LogDirective(m_aDirectives.GetElement(i), pUnit, m_aDirectives.GetWeight(i));
		}
	}
	else
	{
		CvString strLog;
		CvString strTemp;

		strTemp.Format("%d,", pUnit->GetID()); // unit id
		strLog += strTemp;

		strLog += "No directives!";
		LogInfo(strLog, m_pPlayer);
	}
}

void CvBuilderTaskingAI::LogDirective(BuilderDirective directive, CvUnit* pUnit, int iWeight, bool bChosen)
{
	if(!m_bLogging)
	{
		return;
	}

	CvString strLog;
	CvString strTemp;

	strTemp.Format("%d,", pUnit->GetID()); // unit id
	strLog += strTemp;

	strLog += "Evaluating,";

	switch(directive.m_eDirective)
	{
	case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
		strLog += "BUILD_IMPROVEMENT_ON_RESOURCE,";
		break;
	case BuilderDirective::BUILD_IMPROVEMENT:
		strLog += "BUILD_IMPROVEMENT,";
		break;
	case BuilderDirective::REPAIR:
		strLog += "REPAIR,";
		break;
	case BuilderDirective::BUILD_ROUTE:
		strLog += "BUILD_ROUTE,";
		break;
	case BuilderDirective::CHOP:
		strLog += "CHOP";
	}

	strLog += GC.getBuildInfo(directive.m_eBuild)->GetType();
	strLog += ",";

	if(directive.m_eResource != NO_RESOURCE)
	{
		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(directive.m_eResource);
		if(pkResourceInfo)
		{
			strLog += pkResourceInfo->GetType();
			strLog += ",";
			CvPlot* pPlot = GC.getMap().plot(directive.m_sX, directive.m_sY);
			strLog += pPlot->getNumResource();
			strLog += ",";
		}
	}
	else
	{
		strLog += ",";
	}

	if(directive.m_eDirective == BuilderDirective::REPAIR)
	{
		CvPlot* pPlot = GC.getMap().plot(directive.m_sX, directive.m_sY);
		if(pPlot->IsImprovementPillaged())
		{
			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
				if(pkImprovementInfo)
				{
					strLog += pkImprovementInfo->GetType();
				}
			}
		}
		else
		{
			if(pPlot->getRouteType() != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(pPlot->getRouteType());
				if(pkRouteInfo)
				{
					strLog += pkRouteInfo->GetType();
				}
			}
		}
	}
	else
	{
		CvPlot* pPlot = GC.getMap().plot(directive.m_sX, directive.m_sY);
		if(pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
			if(pkImprovementInfo)
			{
				strLog += pkImprovementInfo->GetType();
			}
		}
	}
	strLog += ",";

	strTemp.Format("%d,%d,", directive.m_sX, directive.m_sY);
	strLog += strTemp;

	strTemp.Format("%d,", directive.m_sMoveTurnsAway);
	strLog += strTemp;

	strTemp.Format("%d,", iWeight);
	strLog += strTemp;

	if(bChosen)
	{
		strLog += (", Chosen!");
	}

	LogInfo(strLog, m_pPlayer, GET_PLAYER(pUnit->getOwner()).isHuman());
}

// looks at the current plot to see what it's worth
void CvBuilderTaskingAI::UpdateCurrentPlotYields(CvPlot* pPlot)
{
	m_pTargetPlot = pPlot;
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		m_aiCurrentPlotYields[ui] = pPlot->getYield((YieldTypes)ui);
	}
}

// looks at the current plot assuming the build to see what it's worth
void CvBuilderTaskingAI::UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild)
{
	if(pPlot != m_pTargetPlot)
	{
		UpdateCurrentPlotYields(pPlot);
	}

	// determine if this feature removes a yield
	//FeatureTypes eFeature = pPlot->getFeatureType();
	//bool bFeatureNeedsRemove = false;
	//if (eFeature != NO_FEATURE && eBuild != NO_BUILD)
	//{
	//	if (GC.getBuildInfo(eBuild)->isFeatureRemove(eFeature))
	//	{
	//		bFeatureNeedsRemove = true;
	//	}
	//}

	//// calculate the natural yield of this tile (including if a feature is to be removed)
	//for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	//{
	//	m_aiProjectedPlotYields[ui] = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->getTeam(), bFeatureNeedsRemove);
	//}

	//ImprovementTypes eNewImprovement = NO_IMPROVEMENT;
	//RouteTypes eNewRoute = NO_ROUTE;

	//if (eBuild != NO_BUILD)
	//{
	//	eNewImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();
	//	eNewRoute = (RouteTypes)GC.getBuildInfo(eBuild)->getRoute();
	//}

	//// if there is no new route or improvement, we're done
	//if (eNewRoute == NO_ROUTE && eNewImprovement == NO_IMPROVEMENT)
	//{
	//	return;
	//}

	//ImprovementTypes eExistingImprovement = pPlot->getImprovementType();
	//RouteTypes eExistingRoute = pPlot->getRouteType();

	//// assign these to values to make the code simpler
	//ImprovementTypes eImprovement = NO_IMPROVEMENT;
	//RouteTypes eRoute = NO_ROUTE;

	//if (eNewImprovement != NO_IMPROVEMENT)
	//{
	//	eImprovement = eNewImprovement;
	//	eRoute = eExistingRoute;
	//}
	//else if (eNewRoute != NO_ROUTE)
	//{
	//	eImprovement = eExistingImprovement;
	//	eRoute = eNewRoute;
	//}

	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, m_pPlayer->GetID());

		//m_aiProjectedPlotYields[ui] += pPlot->calculateImprovementYieldChange(eImprovement, (YieldTypes)ui, m_pPlayer->GetID(), false /*bOptimal*/, eRoute);
		m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);
	}
}
