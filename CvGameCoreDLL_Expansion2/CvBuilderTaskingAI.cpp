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
#include "CvEconomicAI.h"
#include "CvGameCoreEnumSerialization.h" //toString(const YieldTypes& v)
#include "CvTypes.h"

// include after all other headers
#include "LintFree.h"

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
	m_eRemoveRouteBuild = GetRemoveRoute();
	m_eRouteBuild = NO_BUILD; //will be updated each turn!

	m_aDirectives.clear();
	m_bLogging = GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging();

	// special case code so the Dutch don't remove marshes
	m_bKeepMarshes = false;
	// special case code so Brazil doesn't remove jungle
	m_bKeepJungle = false;
#if defined(MOD_BALANCE_CORE)
	// special case to evaluate plots adjacent to friendly
	m_bEvaluateAdjacent = false;
	m_bNoPermanentsAdjacentCity = false;
#endif
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
				else if (pkImprovementInfo->GetFeatureMakesValid(FEATURE_JUNGLE))
				{
					m_bKeepJungle = true;
				}
				if (pkImprovementInfo->IsAdjacentCity())
				{
					m_bNoPermanentsAdjacentCity = true;
				}
			}
		}
#if defined(MOD_BALANCE_CORE)
		if(pkImprovementInfo->IsInAdjacentFriendly())
		{
			//Is this required by a civ? If so, block it to all others.
			if(pkImprovementInfo->IsSpecificCivRequired())
			{
				CivilizationTypes eCiv = pkImprovementInfo->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					m_bEvaluateAdjacent = true;
				}
			}
			//Is this created by a GP? If so, block it to workers (GPs have their own tests for this kind of thing).
			else if(!pkImprovementInfo->IsCreatedByGreatPerson())
			{
				m_bEvaluateAdjacent = true;
			}
			//GP? Oh, is it our special unit, and is also a GP (i.e. Venice)? This sucks, but we need to drill down and find our UU, and our build.
			else
			{
				CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(m_pPlayer->getCivilizationType());
				if(pkInfo)
				{
					// Loop through all units
					for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
					{
						// Is this one overridden for our civ?
						if(pkInfo->isCivilizationUnitOverridden(iI))
						{
							UnitTypes eCivilizationUnit = static_cast<UnitTypes>(pkInfo->getCivilizationUnits(iI));
							if(eCivilizationUnit != NO_UNIT)
							{
								CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eCivilizationUnit);
								if(pkUnitEntry && pkUnitEntry->GetBuilds((BuildTypes)i))
								{
									m_bEvaluateAdjacent = true;
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

/// Uninit
void CvBuilderTaskingAI::Uninit(void)
{
	m_pPlayer = NULL;
	m_bLogging = false;

	m_eRepairBuild = NO_BUILD;
	m_eFalloutFeature = NO_FEATURE;
	m_eFalloutRemove = NO_BUILD;

	m_aDirectives.clear();

	m_bKeepMarshes = false;
	m_bKeepJungle = false;
	m_bEvaluateAdjacent = false;
	m_bNoPermanentsAdjacentCity = false;
}

/// Serialization read
void CvBuilderTaskingAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Serialization write
void CvBuilderTaskingAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// Update
void CvBuilderTaskingAI::Update(void)
{
	UpdateRoutePlots();

	m_eRouteBuild = GetBuildRoute();

	int iLoop;
	for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
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
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
				case YIELD_TOURISM:
					strYield = "tourism    ";
					break;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
				case YIELD_GOLDEN_AGE_POINTS:
					strYield = "goldenage  ";
					break;
#endif
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

	return pPlot->calculateNatureYield(eYield, NO_PLAYER, NULL);
}

void CvBuilderTaskingAI::ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, RouteTypes eRoute, int iNetGoldTimes100)
{
	// for quests we might be targeting a city state ...
	bool bSamePlayer = (pTargetCity->getOwner() == pPlayerCapital->getOwner());

	// if we already have a connection, bail out
	bool bIndustrialRoute = (GC.getGame().GetIndustrialRoute() == eRoute);
	if(bIndustrialRoute)
	{
		if (m_pPlayer->GetCityConnections()->AreCitiesDirectlyConnected(pPlayerCapital, pTargetCity, CvCityConnections::CONNECTION_RAILROAD))
			return;
	}
	else if (m_pPlayer->GetCityConnections()->AreCitiesDirectlyConnected(pPlayerCapital, pTargetCity, CvCityConnections::CONNECTION_ROAD))
	{
		return;
	}

	if(pTargetCity->IsRazing())
	{
		return;
	}

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if(!pRouteInfo)
	{
		return;
	}

	// build a path between the two cities
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eRoute);
	SPath path = GC.GetStepFinder().GetPath(pPlayerCapital->getX(), pPlayerCapital->getY(), pTargetCity->getX(), pTargetCity->getY(), data);

	// if no path, try again with harbors allowed
	if(!path)
	{
		data.ePathType = PT_BUILD_ROUTE_MIXED;
		path = GC.GetStepFinder().GetPath(pPlayerCapital->getX(), pPlayerCapital->getY(), pTargetCity->getX(), pTargetCity->getY(), data);

		//still no path? then give up
		if (!path)
			return;
	}

	bool bHuman = m_pPlayer->isHuman();
	// go through the route to see how long it is and how many plots already have roads
	int iRoadLength = 0;
	int iPlotsNeeded = 0;
	int iWildPlots = 0; //plots not under our control ... dangerous

	for (size_t i=0; i<path.vPlots.size(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if(!pPlot)
			break;

		//don't count the cities themselves
		if (pPlot->isCity())
			continue;

		iRoadLength++;

		if(pPlot->getRouteType() < eRoute || pPlot->IsRoutePillaged())
			iPlotsNeeded++;

		if (pPlot->getOwner()!=m_pPlayer->GetID())
			iWildPlots++;
	}

	//don't build through the wilderness
	if (iWildPlots>3 && bSamePlayer && !bHuman)
		return;

	//maybe a lighthouse is just as good?
	if (pPlayerCapital->isCoastal() && pTargetCity->isCoastal() && pPlayerCapital->isMatchingArea(pTargetCity->plot()))
		if (iNetGoldTimes100<500 && !bHuman)
			return;

	//see if the new route makes sense economically
	int iValue = -1;
	if(!bSamePlayer)
	{
		//this is for a quest ... normal considerations don't apply
		iValue = min(GC.getMINOR_CIV_ROUTE_QUEST_WEIGHT() / max(1, iPlotsNeeded), MAX_SHORT);
	}
	else
	{
		int iMaintenancePerTile = pRouteInfo->GetGoldMaintenance()*(100+m_pPlayer->GetImprovementGoldMaintenanceMod());
		int iGoldForRoute = m_pPlayer->GetTreasury()->GetCityConnectionRouteGoldTimes100(pTargetCity);

		//route has side benefits also (movement, village gold, trade route range, religion spread)
		int iSideBenefits = iRoadLength * 25;
		// give an additional bump if we're almost done (don't get distracted)
		if (iPlotsNeeded<=3)
			iSideBenefits += 2000;

		//assume one unhappiness is worth .5 gold per turn per city
		iSideBenefits += pTargetCity->getUnhappinessFromConnection() * m_pPlayer->IsEmpireUnhappy() ? 200 : 100;

		if(bIndustrialRoute)
		{
			iSideBenefits += (pTargetCity->getYieldRate(YIELD_PRODUCTION, false) * GC.getINDUSTRIAL_ROUTE_PRODUCTION_MOD());
		}

		int iProfit = iGoldForRoute - (iRoadLength*iMaintenancePerTile);
		iProfit += iSideBenefits;

		if (!bHuman && (iProfit < 0 || (iProfit + iNetGoldTimes100 < 0)))
			return;

		iValue = iProfit;
	}

	for (size_t i=0; i<path.vPlots.size(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if(!pPlot)
			break;

		if(pPlot->getRouteType() >= eRoute && !pPlot->IsRoutePillaged())
			continue;
		
		//don't build roads if our trait gives the same benefit
		if (m_pPlayer->getBestRoute() == ROUTE_ROAD && m_pPlayer->GetPlayerTraits()->IsRiverTradeRoad() || m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus())
		{
			if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (pPlot->getFeatureType() == FEATURE_FOREST || pPlot->getFeatureType() == FEATURE_JUNGLE))
				continue;
			
			if (m_pPlayer->GetPlayerTraits()->IsRiverTradeRoad() && pPlot->isRiver())
				continue;
		}

		// mark nodes and reset values
		pPlot->SetBuilderAIScratchPadTurn(GC.getGame().getGameTurn());
		pPlot->SetBuilderAIScratchPadPlayer(m_pPlayer->GetID());
		pPlot->SetBuilderAIScratchPadValue(iValue);
		pPlot->SetBuilderAIScratchPadRoute(eRoute);
	}
}

void CvBuilderTaskingAI::ConnectCitiesForShortcuts(CvCity* pCity1, CvCity* pCity2, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if(pCity1->getOwner() != pCity2->getOwner())
		return;

	// if we *don't* already have a basic land connection, bail out
	SPath existingPath;
	if (!m_pPlayer->IsCityConnectedToCity(pCity1, pCity2, ROUTE_ROAD, true, &existingPath))
		return;

	// build a path between the two cities - this will tend to re-use existing routes, unless the new path is much shorter
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eRoute);
	SPath newPath = GC.GetStepFinder().GetPath(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY(), data);

	//this cannot really happen, but anyway
	if(!newPath)
		return;

	//now compare if the new path is shorter or better than the existing path. 
	//don't use the normalized distance though because we have two different pathfinders here, so it's not quite comparable
	if (newPath.vPlots.size() < existingPath.vPlots.size() || eRoute>ROUTE_ROAD )
	{
		int iGameTurn = GC.getGame().getGameTurn();
		for (size_t i=0; i<newPath.vPlots.size(); i++)
		{
			CvPlot* pPlot = newPath.get(i);
			if(pPlot->getRouteType() >= eRoute && !pPlot->IsRoutePillaged())
				continue;

			if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (pPlot->getFeatureType() == FEATURE_FOREST || pPlot->getFeatureType() == FEATURE_JUNGLE))
				continue;

			if (m_pPlayer->GetPlayerTraits()->IsRiverTradeRoad() && pPlot->isRiver())
				continue;

			// mark nodes and reset values
			pPlot->SetBuilderAIScratchPadTurn(iGameTurn);
			pPlot->SetBuilderAIScratchPadPlayer(m_pPlayer->GetID());
			pPlot->SetBuilderAIScratchPadValue(10);
			pPlot->SetBuilderAIScratchPadRoute(eRoute);
		}
	}
}

//helper function. don't panic if a a plot isn't marked for a short time
bool WantRoute(CvPlot* pPlot, PlayerTypes ePlayer, int iMaxAge)
{
	return (pPlot->GetBuilderAIScratchPadTurn() >= (GC.getGame().getGameTurn() - iMaxAge) && 
		pPlot->GetBuilderAIScratchPadPlayer() == ePlayer &&
		pPlot->GetBuilderAIScratchPadRoute() != NO_ROUTE
		);
}

void CvBuilderTaskingAI::ConnectCitiesForScenario(CvCity* pCity1, CvCity* pCity2, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if(pCity1->getOwner() != pCity2->getOwner())
		return;

	// if we already have a connection, bail out
	if (m_pPlayer->IsCityConnectedToCity(pCity1, pCity2, eRoute, true))
		return;

	// build a path between the two cities
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eRoute);
	SPath path = GC.GetStepFinder().GetPath(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY(), data);

	//  if no path, then bail!
	if(!path)
		return;

	for (int i=0; i<path.length(); i++)
	{
		CvPlot* pPlot = path.get(i);

		if(!pPlot)
			break;

		if(pPlot->getRouteType() == eRoute && !pPlot->IsRoutePillaged())
			continue;

		// if we already know about this plot, continue on
		if(WantRoute(pPlot,m_pPlayer->GetID(),1))
			continue;

		// mark nodes and reset values
		pPlot->SetBuilderAIScratchPadTurn(GC.getGame().getGameTurn());
		pPlot->SetBuilderAIScratchPadPlayer(m_pPlayer->GetID());
		pPlot->SetBuilderAIScratchPadValue( pPlot->IsAdjacentToRoute() ? 150 : 100);
		pPlot->SetBuilderAIScratchPadRoute(eRoute);
	}
}

void CvBuilderTaskingAI::ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, RouteTypes eRoute, int iNetGoldTimes100)
{
	// don't connect cities from different owners
	if (pOriginCity->getOwner() != pTargetPlot->getOwner())
		return;

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if (!pRouteInfo)
		return;

	// build a path between the two cities
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eRoute);
	SPath path = GC.GetStepFinder().GetPath(pOriginCity->getX(), pOriginCity->getY(), pTargetPlot->getX(), pTargetPlot->getY(), data);

	//  if no path, then bail!
	if (!path)
		return;

	int iCost = pRouteInfo->GetGoldMaintenance()*(100 + m_pPlayer->GetImprovementGoldMaintenanceMod());
	iCost *= path.length();
	bool bTooExpensive = false;
	if (iNetGoldTimes100 - iCost <= 6)
		bTooExpensive = true;

	for (int i = 0; i<path.length(); i++)
	{
		CvPlot* pPlot = path.get(i);

		if (!pPlot)
			break;

		if (pPlot->getOwner() != m_pPlayer->GetID())
			break;

		//we set this to keep the route alive.
		pPlot->SetStrategicRoute(m_pPlayer->getTeam(), true);

		//and this to see if we actually build it
		if (bTooExpensive)
			continue;

		if (pPlot->getRouteType() == eRoute && !pPlot->IsRoutePillaged())
			continue;

		// if we already know about this plot, continue on
		if(WantRoute(pPlot,m_pPlayer->GetID(),1))
			continue;

		// mark nodes and reset values
		pPlot->SetBuilderAIScratchPadTurn(GC.getGame().getGameTurn());
		pPlot->SetBuilderAIScratchPadPlayer(m_pPlayer->GetID());
		pPlot->SetBuilderAIScratchPadValue(10);
		pPlot->SetBuilderAIScratchPadRoute(eRoute);
	}
}
/// Looks at city connections and marks plots that can be added as routes by EvaluateBuilder
void CvBuilderTaskingAI::UpdateRoutePlots(void)
{
	// updating plots that are part of the road network
	CvCityConnections* pCityConnections = m_pPlayer->GetCityConnections();

	// if there are fewer than 2 cities, we don't need to run this function
	std::vector<int> plotsToConnect = pCityConnections->GetPlotsToConnect();
	if(plotsToConnect.size() < 2)
	{
		return;
	}

	RouteTypes eBestRoute = m_pPlayer->getBestRoute();
	if(eBestRoute == NO_ROUTE)
	{
		return;
	}

	//reset this each turn, as our improvements may change
	for (PlotIndexContainer::const_iterator it = m_pPlayer->GetPlots().begin(); it != m_pPlayer->GetPlots().end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		pPlot->SetStrategicRoute(m_pPlayer->getTeam(), false);
	}

	// find a builder, if I don't have a builder, bail!
	int iBuilderCount = 0;
	int iLoopUnit;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		if(pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			iBuilderCount++;

			//mark the plot if one of our builders is already active there
			const MissionData* pMission = pLoopUnit->GetHeadMissionData();
			if (pMission && pMission->eMissionType==CvTypes::getMISSION_BUILD() && pMission->iData2==BuilderDirective::BUILD_ROUTE)
			{
				CvPlot* pPlot = pLoopUnit->plot();
				pPlot->SetBuilderAIScratchPadTurn(GC.getGame().getGameTurn());
				pPlot->SetBuilderAIScratchPadPlayer(m_pPlayer->GetID());
				pPlot->SetBuilderAIScratchPadValue(200);
				pPlot->SetBuilderAIScratchPadRoute(eBestRoute);
			}
		}
	}

	// If there's no builder, bail!
	if(iBuilderCount==0)
	{
		return;
	}

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
#if defined(MOD_BALANCE_CORE)
		if(GC.getBuildInfo(eBuild)->getTechObsolete() != NO_TECH)
		{
			bool bHasTech = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechObsolete());
			if(bHasTech)
			{
				continue;
			}
		}
#endif

		//cache this
		int iNetGoldTimes100 = m_pPlayer->GetTreasury()->CalculateBaseNetGoldTimes100();

		for(uint uiFirstCityIndex = 0; uiFirstCityIndex < plotsToConnect.size(); uiFirstCityIndex++)
		{
			for(uint uiSecondCityIndex = uiFirstCityIndex + 1; uiSecondCityIndex < plotsToConnect.size(); uiSecondCityIndex++)
			{
				CvPlot* pFirstPlot = GC.getMap().plotByIndexUnchecked(plotsToConnect[uiFirstCityIndex]);
				CvPlot* pSecondPlot = GC.getMap().plotByIndexUnchecked(plotsToConnect[uiSecondCityIndex]);

				// get the two cities
				CvCity* pFirstCity  = pFirstPlot->getPlotCity();
				CvCity* pSecondCity  = pSecondPlot->getPlotCity();
				CvCity* pPlayerCapitalCity = NULL;
				CvCity* pTargetCity = NULL;

				if (pFirstCity && !pSecondCity)
				{
					ConnectPointsForStrategy(pFirstCity, pSecondPlot, eBestRoute, iNetGoldTimes100);
					continue;
				}

				if (!pFirstCity && pSecondCity)
				{
					ConnectPointsForStrategy(pSecondCity, pFirstPlot, eBestRoute, iNetGoldTimes100);
					continue;
				}

				//we don't do citadel to citadel. or should we?
				if (!pFirstCity && !pSecondCity)
					continue;

				bool bConnectOnlyCapitals = (bool)GC.getCITY_CONNECTIONS_CONNECT_TO_CAPITAL();
				if (bConnectOnlyCapitals)
				{
					// only need to build roads to the capital for the money and happiness
					if(!pFirstCity->isCapital() && !pSecondCity->isCapital() && iNetGoldTimes100>0)
					{
						//if we already have a connection to the capital, it may be possible to have a much shorter route for a direct connection
						//thus improving unit movement and gold bonus from villages
						ConnectCitiesForShortcuts(pFirstCity, pSecondCity, eBestRoute);
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

					ConnectCitiesToCapital(pPlayerCapitalCity, pTargetCity, eRoute, iNetGoldTimes100);
				}
				else
				{
					ConnectCitiesForScenario(pFirstCity, pSecondCity, eBestRoute);
				}
			}
		}
	}
}

CvUnit* CvBuilderTaskingAI::FindBestWorker(const std::map<CvUnit*, ReachablePlots>& allWorkersReachablePlots, const CvPlot* pTarget) const
{
	if (!pTarget)
		return NULL;

	int iBestTurnsAway = INT_MAX;
	CvUnit* pBestWorker = NULL;

	for (std::map<CvUnit*, ReachablePlots>::const_iterator itUnit = allWorkersReachablePlots.begin(); itUnit != allWorkersReachablePlots.end(); ++itUnit)
	{
		int iTurns = INT_MAX;
		ReachablePlots::const_iterator itPlot = itUnit->second.find(pTarget->GetPlotIndex());
		if (itPlot != itUnit->second.end())
			iTurns = itPlot->iTurns;
		else
			continue;

		if (iTurns < iBestTurnsAway)
		{
			iBestTurnsAway = iTurns;
			pBestWorker = itUnit->first;
		}
	}

	return pBestWorker;
}

/// Use the flavor settings to determine what the worker should do
BuilderDirective CvBuilderTaskingAI::EvaluateBuilder(CvUnit* pUnit, const map<CvUnit*,ReachablePlots>& allWorkersReachablePlots)
{
	m_aDirectives.clear();

	// check for no brainer bail-outs
	// if the builder is already building something
	if (pUnit->getBuildType() != NO_BUILD && pUnit->GetDanger() < pUnit->GetCurrHitPoints() / 2)
	{
		BuilderDirective nd;
		nd.m_eDirective = BuilderDirective::BUILD_IMPROVEMENT;
		nd.m_eBuild = pUnit->getBuildType();
		nd.m_sX = pUnit->getX();
		nd.m_sY = pUnit->getY();
		nd.m_sMoveTurnsAway = 0;
		return nd;
	}

	// go through all the plots this unit can reach
	map<CvUnit*, ReachablePlots>::const_iterator thisUnitPlots = allWorkersReachablePlots.find(pUnit);
	if (thisUnitPlots == allWorkersReachablePlots.end())
		return BuilderDirective();

	for(ReachablePlots::const_iterator it=thisUnitPlots->second.begin(); it!=thisUnitPlots->second.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(it->iPlotIndex);
		int iMoveTurnsAway = it->iTurns;

		if(!ShouldBuilderConsiderPlot(pUnit, pPlot))
		{
			continue;
		}

		CvUnit* pBestWorker = FindBestWorker(allWorkersReachablePlots,pPlot);
		if (pBestWorker != pUnit)
		{
			continue;
		}

		//in our own plots we can build anything
		if (pPlot->getOwner() == pUnit->getOwner())
		{
			UpdateCurrentPlotYields(pPlot);

			AddRouteDirectives(pUnit, pPlot, iMoveTurnsAway);
			AddImprovingResourcesDirectives(pUnit, pPlot, iMoveTurnsAway);
			AddImprovingPlotsDirectives(pUnit, pPlot, iMoveTurnsAway);
			if (pUnit->AI_getUnitAIType() == UNITAI_WORKER)
			{
				AddChopDirectives(pUnit, pPlot, iMoveTurnsAway);
				AddScrubFalloutDirectives(pUnit, pPlot, iMoveTurnsAway);
				AddRepairTilesDirectives(pUnit, pPlot, iMoveTurnsAway);
				AddRemoveRouteDirectives(pUnit, pPlot, iMoveTurnsAway);
			}
		}
		else if (m_bEvaluateAdjacent && !pPlot->isOwned() && pPlot->isAdjacentPlayer(pUnit->getOwner()))
		{
			UpdateCurrentPlotYields(pPlot);

			//some special improvements and roads
			AddImprovingPlotsDirectives(pUnit, pPlot, iMoveTurnsAway);
			AddRouteDirectives(pUnit, pPlot, iMoveTurnsAway);
		}
		else
		{
			//only roads
			AddRouteDirectives(pUnit, pPlot, iMoveTurnsAway);
		}
	}

	sort(m_aDirectives.begin(), m_aDirectives.end());
	LogDirectives(pUnit);

	//nothing found?
	if (m_aDirectives.empty())
		return BuilderDirective();

	return m_aDirectives.front().option;
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

	if (pPlot->IsImprovementPillaged())
		return;

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
		if(pkImprovementInfo == NULL)
			continue;

#if defined(MOD_BALANCE_CORE)
		//Check for test below.
		if(pkImprovementInfo->IsSpecificCivRequired() && m_pPlayer->getCivilizationType() != pkImprovementInfo->GetRequiredCivilization())
			continue;

#endif

		if (pkImprovementInfo->IsExpandedImprovementResourceTrade(eResource))
		{
			if(eImprovement == eExistingPlotImprovement)
			{
				if(pkImprovementInfo->IsAdjacentCity())
				{
					//break if adjacent already here.
					break;
				}
				else
				{
					// this plot already has the appropriate improvement to use the resource
					continue;
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
				continue;
			}

			BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE;
			int iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS();

			//slightly downward value for bonus resources relative to lux/strat
			if (pkResource->getResourceUsage() == RESOURCEUSAGE_BONUS)
			{
				iWeight *= 2;
				iWeight /= 3;
			}

			//if we're building a great person improvement, putting it on a resource is the last resort ...
			if (pkImprovementInfo->IsCreatedByGreatPerson())
				iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS();

			iWeight = GetBuildCostWeight(iWeight, pPlot, eBuild);

			// this is to deal with when the plot is already improved with another improvement that doesn't enable the resource
			int iInvestedImprovementTime = 0;
			bool bPrevImprovementConnects = false;
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
						CvImprovementEntry* pkExistingImprovementInfo = GC.getImprovementInfo((ImprovementTypes)pkBuild2->getImprovement());
						bPrevImprovementConnects = pkExistingImprovementInfo && pkExistingImprovementInfo->IsExpandedImprovementResourceTrade(eResource);
						eExistingBuild = eBuild2;
						break;
					}
				}

				if(eExistingBuild != NO_BUILD)
				{
					iInvestedImprovementTime = pPlot->getBuildTime(eExistingBuild, m_pPlayer->GetID());
				}
			}

			int iBuildTimeWeight = GetBuildTimeWeight(pUnit, pPlot, eBuild, DoesBuildHelpRush(pUnit, pPlot, eBuild), iInvestedImprovementTime);
			iWeight += iBuildTimeWeight;

			if (!bPrevImprovementConnects && !pkImprovementInfo->IsCreatedByGreatPerson())
			{
				int iResourceWeight = GetResourceWeight(eResource, eImprovement, pPlot->getNumResource());
				iWeight += iResourceWeight;
			}

#if defined(MOD_BALANCE_CORE)
			iWeight = min(iWeight,0x7FFF);
			iWeight = iWeight / (iMoveTurnsAway*iMoveTurnsAway + 1);
#endif

			int iScore = ScorePlotBuild(pPlot, eImprovement, eBuild);
			iScore = min(iScore,0x7FFF);

			if(iScore > 0)
			{
				iWeight += iScore;
			}

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

			m_aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
		}
	}
}

/// Evaluating a plot to determine what improvement could be best there
void CvBuilderTaskingAI::AddImprovingPlotsDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	ImprovementTypes eExistingImprovement = pPlot->getImprovementType();

	// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
	if(eExistingImprovement != NO_IMPROVEMENT && pPlot->HasSpecialImprovement() && !pPlot->IsImprovementPillaged())
		return;

	if(!m_bEvaluateAdjacent && !pPlot->isWithinTeamCityRadius(pUnit->getTeam()))
		return;

	// check to see if a non-bonus resource is here. if so, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource != NO_RESOURCE && GC.getResourceInfo(eResource)->getResourceUsage() != RESOURCEUSAGE_BONUS)
		return;

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

	if (pPlot->IsImprovementPillaged())
		return;

	if (!m_bEvaluateAdjacent)
	{
		CvCity* pCity = getOwningCity(pPlot);
		if(!pCity)
		{
			return;
		}
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

		CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);

		// if this improvement has a defense modifier, ignore it for now
		//if(pImprovement->GetDefenseModifier() > 0)
		//{
		//	continue;
		//}

		// for bonus resources, check to see if this is the improvement that connects it
		if(eResource != NO_RESOURCE)
		{
			if (!pImprovement->IsExpandedImprovementResourceTrade(eResource))
			{
				if(m_bLogging){
					CvString strTemp;
					strTemp.Format("Weight,!pImprovement->IsExpandedImprovementResourceTrade(eResource),%s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), eResource, pPlot->getX(), pPlot->getY());
					LogInfo(strTemp, m_pPlayer);
				}
				continue;
			}
		}

		if(eImprovement == pPlot->getImprovementType())
		{
			if(m_bLogging){
				CvString strTemp;
				strTemp.Format("Weight,eImprovement == pPlot->getImprovementType(),%s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), eImprovement, pPlot->getX(), pPlot->getY());
				LogInfo(strTemp, m_pPlayer);
			}
			continue;
		}
		else
		{
			// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
			if (eExistingImprovement != NO_IMPROVEMENT)
			{
				if (pPlot->HasSpecialImprovement() || (GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION) && GET_PLAYER(pUnit->getOwner()).isHuman()))
				{
					if(m_bLogging){
						CvString strTemp;
						strTemp.Format("Weight,Improvement Type Blocked by Special Improvement,%s,,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), pPlot->getX(), pPlot->getY());
						LogInfo(strTemp, m_pPlayer);
					}
					continue;
				}
			}
		}

		// Only check to make sure our unit can build this after possibly switching this to a repair build in the block of code above
		if(!pUnit->canBuild(pPlot, eBuild))
		{
			if(m_bLogging){
				CvString strTemp;
				strTemp.Format("Weight,!pUnit->canBuild(),%s,,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), pPlot->getX(), pPlot->getY());
				LogInfo(strTemp, m_pPlayer);
			}
			continue;
		}

		bool bWillRemoveForestOrJungle = false;
		FeatureTypes eFeature = pPlot->getFeatureType();
		if(eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE)
		{
			if (pkBuild->isFeatureRemove(eFeature))
			{
				bWillRemoveForestOrJungle = true;
			}
		}

		// special case for Dutch
		if (m_bKeepMarshes && eFeature == FEATURE_MARSH)
		{
			if (pkBuild->isFeatureRemove(FEATURE_MARSH))
			{
				if(m_bLogging){
					CvString strTemp;
					strTemp.Format("Weight,Marsh Remove,%s,,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), pPlot->getX(), pPlot->getY());
					LogInfo(strTemp, m_pPlayer);
				}
				continue;
			}
		}

		// special case for Brazil
		if (m_bKeepJungle && eFeature == FEATURE_JUNGLE)
		{
			if (pkBuild->isFeatureRemove(FEATURE_JUNGLE))
			{
				if(m_bLogging){
					CvString strTemp;
					strTemp.Format("Weight,Jungle Remove,%s,,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), pPlot->getX(), pPlot->getY());
					LogInfo(strTemp, m_pPlayer);
				}
				if (pPlot->getResourceType(m_pPlayer->getTeam()) == NO_RESOURCE)
				{
					continue;
				}
			}
		}

		if(GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_LEAVE_FORESTS) && GET_PLAYER(pUnit->getOwner()).isHuman())
		{
			if(eFeature != NO_FEATURE && eResource == NO_RESOURCE)
			{
				if(pkBuild->isFeatureRemove(eFeature))
				{
					if(m_bLogging){
						CvString strTemp;
						strTemp.Format("Weight,Keep Forests,%s,,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), pPlot->getX(), pPlot->getY());
						LogInfo(strTemp, m_pPlayer);
					}
					continue;
				}
			}
		}

		int iScore = ScorePlotBuild(pPlot, eImprovement, eBuild);
		iScore = min(iScore,0x7FFF);

		// if we're going backward, bail out!
		if(iScore <= 0)
		{
			if(m_bLogging){
				CvString strTemp;
				strTemp.Format("Weight,Negative Score,%s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), iScore, pPlot->getX(), pPlot->getY());
				LogInfo(strTemp, m_pPlayer);
			}
			continue;
		}

		BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_IMPROVEMENT;
		int iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS();
#if !defined(MOD_API_UNIFIED_YIELDS)
		else if(pImprovement->GetYieldChange(YIELD_CULTURE) > 0)
		{
			iWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_CULTURE() * GC.getImprovementInfo(eImprovement)->GetYieldChange(YIELD_CULTURE);
			int iAdjacentCulture = pImprovement->GetCultureAdjacentSameType();

			if(iAdjacentCulture > 0)
			{
				iScore *= (1 + pPlot->ComputeCultureFromAdjacentImprovement(*pImprovement, eImprovement));
			}
		}
#endif
		iWeight = GetBuildCostWeight(iWeight, pPlot, eBuild);

		int iBuildTimeWeight = GetBuildTimeWeight(pUnit, pPlot, eBuild, DoesBuildHelpRush(pUnit, pPlot, eBuild));
		iWeight += iBuildTimeWeight;

		if(m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && bWillRemoveForestOrJungle)
			iWeight /= 100;

#if defined(MOD_BALANCE_CORE)
		iWeight = min(iWeight,0x7FFF);
		iWeight = iWeight / (iMoveTurnsAway*iMoveTurnsAway + 1);
#endif

		//overflow danger here
		iWeight += iScore;

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
			strTemp.Format("Weight,Directive Score Added,%s,,,,%i, %i, %i, %d", GC.getBuildInfo(eBuild)->GetType(), directive.m_sX, directive.m_sY, directive.m_sMoveTurnsAway, iWeight);
			LogInfo(strTemp, m_pPlayer);
		}

		m_aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
	}
}

/// Adds a directive if the unit can construct a road in the plot
void CvBuilderTaskingAI::AddRemoveRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	//minors stay out
	if (m_pPlayer->isMinorCiv())
		return;

	//humans don't get a route cache, so ignore them.
	if (m_pPlayer->isHuman())
		return;

	//can we even remove routes?
	CvUnitEntry& kUnitInfo = pUnit->getUnitInfo();
	if (m_eRemoveRouteBuild==NO_BUILD || !kUnitInfo.GetBuilds(m_eRemoveRouteBuild))
		return;

	// if the player can't build a route, bail out!
	RouteTypes eBestRouteType = m_pPlayer->getBestRoute();
	if (eBestRouteType == NO_ROUTE)
		return;

	if (pPlot->getRouteType() == NO_ROUTE)
		return;

	if (pPlot->isCity())
		return;

	// the plot was flagged recently, so ignore
	if (WantRoute(pPlot,pUnit->getOwner(),5))
		return;

	if (pPlot->IsStrategicRoute(m_pPlayer->getTeam()))
		return;

	if (pPlot->IsCityConnection(m_pPlayer->GetID()))
		return;

	//don't touch master's roads!
	if (pPlot->GetPlayerResponsibleForRoute() != NO_PLAYER && pPlot->GetPlayerResponsibleForRoute() != m_pPlayer->GetID())
		if (GET_TEAM(m_pPlayer->getTeam()).IsVassal(GET_PLAYER(pPlot->GetPlayerResponsibleForRoute()).getTeam()))
			return;

	//we want to be aggressive with this because of the cost.
	int iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_ROUTES()/3;

	//if in debt, bump it up.
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	if(m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
		iWeight *= 3;

	BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::REMOVE_ROAD;

	iWeight = GetBuildCostWeight(iWeight, pPlot, m_eRemoveRouteBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eRemoveRouteBuild, false);
	iWeight = iWeight / (iMoveTurnsAway*iMoveTurnsAway + 1);

	BuilderDirective directive;
	directive.m_eDirective = eDirectiveType;
	directive.m_eBuild = m_eRemoveRouteBuild;
	directive.m_eResource = NO_RESOURCE;
	directive.m_sX = pPlot->getX();
	directive.m_sY = pPlot->getY();
	directive.m_sMoveTurnsAway = iMoveTurnsAway;

	if (m_bLogging)
	{
		CvString strTemp;
		strTemp.Format("RemoveRouteDirectives, adding, x: %d y: %d, Weight, %d", pPlot->getX(), pPlot->getY(), iWeight);
		LogInfo(strTemp, m_pPlayer);
	}

	m_aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
}

/// Adds a directive if the unit can construct a road in the plot
void CvBuilderTaskingAI::AddRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	// if the player can't build a route, bail out!
	RouteTypes eBestRouteType = m_pPlayer->getBestRoute();
	if(eBestRouteType == NO_ROUTE)
		return;

	// can we even build the desired route
	CvUnitEntry& kUnitInfo = pUnit->getUnitInfo();
	if(m_eRouteBuild == NO_BUILD || !kUnitInfo.GetBuilds(m_eRouteBuild))
		return;

	// no matter if pillaged or not
	if(pPlot->getRouteType() == eBestRouteType)
		return;

	// the plot was not flagged recently, so ignore
	if (!WantRoute(pPlot,pUnit->getOwner(),1))
		return;

	if(GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_LEAVE_FORESTS))
	{
		FeatureTypes eFeature = pPlot->getFeatureType();
		if(eFeature != NO_FEATURE)
		{
			CvBuildInfo* pkBuild = GC.getBuildInfo(m_eRouteBuild);
			if(pkBuild && pkBuild->isFeatureRemove(eFeature))
			{
				return;
			}
		}
	}

	int iWeight = GC.getBUILDER_TASKING_BASELINE_BUILD_ROUTES();
	BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_ROUTE;

	iWeight = GetBuildCostWeight(iWeight, pPlot, m_eRouteBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eRouteBuild, false);
	iWeight += pPlot->GetBuilderAIScratchPadValue();
	iWeight = iWeight / (iMoveTurnsAway*iMoveTurnsAway + 1);

	BuilderDirective directive;
	directive.m_eDirective = eDirectiveType;
	directive.m_eBuild = m_eRouteBuild;
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

	m_aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
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

	if (pPlot->IsImprovementPillaged())
		return;

	// check to see if a resource is here. If so, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource != NO_RESOURCE)
	{
		return;
	}

	CvCity* pCity = getOwningCity(pPlot);
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
#if defined(MOD_BALANCE_CORE)
	//Don't cut down city connections!
	if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		if(pPlot->IsCityConnection(m_pPlayer->GetID()))
		{
			return;
		}
	}
#endif

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

	int iWeight = GetBuildCostWeight(GC.getBUILDER_TASKING_BASELINE_REPAIR(), pPlot, eChopBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, eChopBuild, false);
	iWeight *= iProduction; // times the amount that the plot produces from the chopping

#if defined(MOD_BALANCE_CORE)
	iWeight = iWeight / (iMoveTurnsAway*iMoveTurnsAway + 1);
#endif

	int iYieldDifferenceWeight = 0;
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		// calculate natural yields
		int iPreviousYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->GetID(), NULL);
		int iNewYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->GetID(), NULL, true /*bIgnoreFeature*/);
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
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
			case YIELD_TOURISM:
				break;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
			case YIELD_GOLDEN_AGE_POINTS:
				break;
#endif
			}
		}
	}

	// if we are going backwards, bail
	if(iYieldDifferenceWeight < 0)
	{
		return;
	}

	iWeight += iYieldDifferenceWeight;

	if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		iWeight = iWeight / 10;
	}
	if (m_pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest() && eFeature == FEATURE_FOREST)
	{
		iWeight = iWeight / 10;
	}
	// this doesn't actually help them, but adds some historical flavor
	if (m_pPlayer->GetPlayerTraits()->IsEmbarkedAllWater() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		iWeight = iWeight * 2;
	}

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

		m_aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
	}
}

void CvBuilderTaskingAI::AddRepairTilesDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	// if it's not within a city radius
	if (pPlot->getOwner() != pUnit->getOwner())
	{
		return;
	}

	//nothing pillaged here? hmm...
	if (!pPlot->IsImprovementPillaged() && !pPlot->IsRoutePillaged())
	{
		return;
	}

	int iWeight = GetBuildCostWeight(GC.getBUILDER_TASKING_BASELINE_REPAIR() * 100, pPlot, m_eRepairBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eRepairBuild, false);
	iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);

	if (iWeight > 0)
	{
		BuilderDirective directive;
		directive.m_eDirective = BuilderDirective::REPAIR;
		directive.m_eBuild = m_eRepairBuild;
		directive.m_eResource = NO_RESOURCE;
		directive.m_sX = pPlot->getX();
		directive.m_sY = pPlot->getY();
		//directive.m_iGoldCost = m_pPlayer->getBuildCost(pPlot, eChopBuild);
		directive.m_sMoveTurnsAway = iMoveTurnsAway;

		m_aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
	}
}
// Everything means less than zero, hey
void CvBuilderTaskingAI::AddScrubFalloutDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway)
{
	if(m_eFalloutFeature == NO_FEATURE || m_eFalloutRemove == NO_BUILD)
	{
		return;
	}

	CvCity* pCity = getOwningCity(pPlot);
	if(!pCity)
	{
		return;
	}

	if(pPlot->getFeatureType() == m_eFalloutFeature && pUnit->canBuild(pPlot, m_eFalloutRemove))
	{
		int iWeight = GetBuildCostWeight(GC.getBUILDER_TASKING_BASELINE_SCRUB_FALLOUT() * 1000, pPlot, m_eFalloutRemove);
		iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eFalloutRemove, false);
		iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);

		BuilderDirective directive;
		directive.m_eDirective = BuilderDirective::CHOP;
		directive.m_eBuild = m_eFalloutRemove;
		directive.m_eResource = NO_RESOURCE;
		directive.m_sX = pPlot->getX();
		directive.m_sY = pPlot->getY();
		directive.m_sMoveTurnsAway = iMoveTurnsAway;
		m_aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
	}
}

/// Evaluates all the circumstances to determine if the builder can and should evaluate the given plot
bool CvBuilderTaskingAI::ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot)
{
	if (!pPlot || !pUnit)
		return false;

	if (pPlot->isCity())
		return false;

	//don't consider non-workable plots for GPs!
	if (pUnit->IsGreatPerson())
	{
		CvCity* pClosestCity = m_pPlayer->GetClosestCityByPlots(pPlot);
		if (pClosestCity && pClosestCity->getWorkPlotDistance() < m_pPlayer->GetCityDistanceInPlots(pPlot))
			return false;
	}

	//can't build in other major player's territory
	if (pPlot->isOwned() && pPlot->getOwner()!=pUnit->getOwner() && GET_PLAYER(pPlot->getOwner()).isMajorCiv())
		return false;

	// workers should not be able to work in plots that do not match their default domain
	switch(pUnit->getDomainType())
	{
	case DOMAIN_LAND:
#if defined(MOD_AI_SECONDARY_WORKERS)
		// As embarked workers can now build fishing boats, we need to consider coastal plots
		if(pPlot->isDeepWater())
#else
		if(pPlot->isWater())
#endif
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

	int iDanger = pUnit->GetDanger(pPlot);
	if(iDanger > pUnit->GetCurrHitPoints()/2 && pPlot->getBestDefender(pUnit->getOwner()) == NULL)
	{
		//if it's fallout, try to scrub it in spite of the danger
		if(pPlot->getFeatureType() == FEATURE_FALLOUT && !pUnit->ignoreFeatureDamage() && (pUnit->GetCurrHitPoints() < (pUnit->GetMaxHitPoints() / 2)))
		{
			if(GC.getLogging() && GC.getAILogging() && m_bLogging)
			{
				CvString strLog;
				strLog.Format("plotX: %d plotY: %d, danger: %d, bailing due to fallout", pPlot->getX(), pPlot->getY(), m_pPlayer->GetPlotDanger(*pPlot, true));
				m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLog);
			}
			return false;
		}
		else if(pPlot->getFeatureType() != FEATURE_FALLOUT)
		{
			if(GC.getLogging() && GC.getAILogging() && m_bLogging)
			{
				CvString strLog;
				strLog.Format("plotX: %d plotY: %d, danger: %d, bailing due to danger", pPlot->getX(), pPlot->getY(), m_pPlayer->GetPlotDanger(*pPlot, false));
				m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLog);
			}
			return false;
		}
	}

	//danger check is not enough - we don't want to be adjacent to enemy territory for example
	if (!m_pPlayer->isHuman() && pPlot->isVisibleToEnemy(pUnit->getOwner()))
		return false;

	if (!pUnit->canEndTurnAtPlot(pPlot))
	{
		if(m_bLogging)
		{
			CvString strLog;
			strLog.Format("plotX: %d plotY: %d,, this tile is full with another unit. bailing!", pPlot->getX(), pPlot->getY());
			LogInfo(strLog, m_pPlayer, true);
		}

		return false;
	}

	//Another unit already working here? Bail!
	const IDInfo* pUnitNode = pPlot->headUnitNode();
	const CvUnit* pLoopUnit = NULL;
	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit != pUnit)
		{
			if(pLoopUnit->IsWork() && pLoopUnit->getBuildType() != NO_BUILD)
			{
				return false;
			}
		}
	}

	return true;
}

/// Get the weight determined by the cost of building the item
int CvBuilderTaskingAI::GetBuildCostWeight(int iWeight, CvPlot* pPlot, BuildTypes eBuild)
{
	int iBuildCost = m_pPlayer->getBuildCost(pPlot, eBuild);
	if(iBuildCost > 0)
	{
		iWeight = (iWeight * 100) / iBuildCost;
		return iWeight;
	}
	
	return iWeight;
}

/// Get the weight determined by the building time of the item
int CvBuilderTaskingAI::GetBuildTimeWeight(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild, bool bIgnoreFeatureTime, int iAdditionalTime)
{
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

	return 10000 / ((iBuildTime*100) + iAdditionalTime);
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

		if(m_pPlayer->getNumResourceAvailable(eResource) > 0)
			iModifier /= 10; 

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
				iMultiplyingAmount *= 4;

			iWeight *= iMultiplyingAmount;
		}
	}

	return iWeight;
}

/// Get this city that can interact with this plot
CvCity* CvBuilderTaskingAI::getOwningCity(CvPlot* pPlot)
{
	CvCity* pCity = NULL;
	if(pPlot->getOwningCity())
	{
		pCity = pPlot->getOwningCity();
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

	//don't care about rush if this connects a resource.
	if (pPlot->getResourceType(pUnit->getTeam()) != NO_RESOURCE)
		return true;

	if(!(pCity->getOrderFromQueue(0)->bRush))
	{
		// this order should not be rushed
		return false;
	}

	return false;
}

int CvBuilderTaskingAI::ScorePlotBuild(CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild)
{
	UpdateProjectedPlotYields(pPlot, eBuild);

#if defined(MOD_BALANCE_CORE)
	if(eImprovement == NO_IMPROVEMENT)
	{
		return -1;
	}
	if(eBuild == NO_BUILD)
	{
		return -1;
	}

	CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
	if(!pImprovement)
	{
		return -1;
	}
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());

	//Some base value.
	int iScore = 0;
	int iYieldScore = 0;
	int iSecondaryScore = 0;

	int iBigBuff = 1000;
	int iMedBuff = 500;
	int iSmallBuff = 250;

	if (m_bEvaluateAdjacent && pPlot->getOwner() != m_pPlayer->GetID())
	{
		if (!pImprovement->IsInAdjacentFriendly())
		{
			CvCity* pCity = pPlot->getOwningCity();
			if (!pCity)
				return -1;
		}

		CvCity* pCapitalCity = m_pPlayer->getCapitalCity();
		if(pCapitalCity)
		{
			CvCityStrategyAI* pCapitalCityStrategy = pCapitalCity->GetCityStrategyAI();
			bool bAnyNegativeMultiplier = false;
			YieldTypes eFocusYield = pCapitalCityStrategy->GetFocusYield();
			if(!pCapitalCityStrategy)
			{
				return -1;
			}
			for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
			{
				int iMultiplier = pCapitalCityStrategy->GetYieldDeltaTimes100((YieldTypes)ui);
				//int iAbsMultiplier = abs(iMultiplier);
				int iYieldDelta = m_aiProjectedPlotYields[ui] - m_aiCurrentPlotYields[ui] * 100;

				// the multiplier being lower than zero means that we need more of this resource
				if(iMultiplier < 0)
				{
					bAnyNegativeMultiplier = true;
					if(iYieldDelta > 0)  // this would be an improvement to the yield
					{
						iYieldScore += m_aiProjectedPlotYields[ui];
					}
					else if(iYieldDelta < 0)  // the yield would go down
					{
						iYieldScore += iYieldDelta;
					}
				}
				else
				{
					if(iYieldDelta >= 0)
					{
						iYieldScore += m_aiProjectedPlotYields[ui]; // provide a nominal score to plots that improve anything
					}
					else if(iYieldDelta < 0)
					{
						iYieldScore += iYieldDelta;
					}
				}
			}
			if(!bAnyNegativeMultiplier && eFocusYield != NO_YIELD)
			{
				int iYieldDelta = m_aiProjectedPlotYields[eFocusYield] - m_aiCurrentPlotYields[eFocusYield];
				if(iYieldDelta > 0)
				{
					iYieldScore += m_aiProjectedPlotYields[eFocusYield];
				}
			}
			
			//Because this evaluates territory outside of our base territory, let's cut this down a bit.
			iYieldScore -= iSmallBuff;
		}
	}
#endif
	CvCity* pCity = pPlot->getOwningCity();
	//Great improvements are great!
	if (pImprovement->IsCreatedByGreatPerson() && pImprovement->GetCultureBombRadius() <= 0)
	{
		if (!m_bEvaluateAdjacent && !pCity)
			return 0;
	}
	if(pCity)
	{
		CvCityStrategyAI* pCityStrategy = pCity->GetCityStrategyAI();
		if(!pCityStrategy)
		{
			return -1;
		}
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			//int iAbsMultiplier = abs(iMultiplier);
			int iYieldDelta = (m_aiProjectedPlotYields[ui] - m_aiCurrentPlotYields[ui]) * 100;
			if (iYieldDelta > 0 && pCityStrategy->GetFocusYield() == (YieldTypes)ui)
				iYieldScore += iYieldDelta * 5;
			else
				iYieldScore += iYieldDelta;
		}

		if (pCity->isCapital() || pCity->IsOriginalCapital()) // this is our capital and needs emphasis
		{
			iYieldScore += iSmallBuff;
		}
#if defined(MOD_BALANCE_CORE)
		//Plots with resources on them need emphasis, especially if they offer happiness.
		if (!pImprovement->IsCreatedByGreatPerson() && pPlot->getResourceType(pCity->getTeam()) != NO_RESOURCE)
		{
			iScore += iMedBuff;
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(pPlot->getResourceType(pCity->getTeam()));
			if(pkResourceInfo)
			{
				if(pkResourceInfo->getHappiness() > 0)
				{
					iScore += iMedBuff;
				}
				if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
				{
					iScore += iMedBuff;
				}
			}
		}
		if(pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
			if(pkImprovementInfo && (pkImprovementInfo->IsAdjacentCity() || pkImprovementInfo->IsCreatedByGreatPerson() || (pkImprovementInfo->GetYieldChangePerEra(YIELD_CULTURE) > 0)))
			{
				iSecondaryScore -= iBigBuff;
			}
		}
		//Improvement? Let's not default to great person improvements here.
		else
		{
			if (pImprovement->IsCreatedByGreatPerson())
			{
				if (pPlot->getResourceType(pCity->getTeam()) == NO_RESOURCE)
				{
					if (!m_bNoPermanentsAdjacentCity || pPlot->IsAdjacentCity())
						iSecondaryScore += iBigBuff;
					else
						iSecondaryScore -= iSmallBuff;
				}
				else
				{
					iSecondaryScore -= iBigBuff;
				}
			}
		}

		//Improvement grants resource? Let's weight this based on flavors.
		ResourceTypes eResourceFromImprovement = (ResourceTypes)pImprovement->GetResourceFromImprovement();
		if(eResourceFromImprovement != NO_IMPROVEMENT)
		{
			CvResourceInfo* pkResource = GC.getResourceInfo(eResourceFromImprovement);
			int iResourceFlavor = 0;
			int iPersonalityFlavorValue = 0;
			int iResult = 0;
			if(pkResource != NULL)
			{
				for(int i = 0; i < GC.getNumFlavorTypes(); i++)
				{
					iResourceFlavor = pkResource->getFlavorValue((FlavorTypes)i);
					if(iResourceFlavor > 0)
					{
						iPersonalityFlavorValue = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)i);
						if(iPersonalityFlavorValue > 0)
						{
							iResult = iResourceFlavor + iPersonalityFlavorValue;
						}
					}
					if(iResult > 0)
					{
						iSecondaryScore += iResult;
					}
				}
			}
		}
	}
	//Unique improvement? Let's give this a lot of weight!
	if(pImprovement->IsSpecificCivRequired())
	{
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo)
		{
			CivilizationTypes eCiv = pkImprovementInfo->GetRequiredCivilization();
			if(eCiv == m_pPlayer->getCivilizationType())
			{
				if(!m_bEvaluateAdjacent || !pkImprovementInfo->IsInAdjacentFriendly())
				{
					iSecondaryScore += iBigBuff;
				}
				else if(m_bEvaluateAdjacent && pkImprovementInfo->IsInAdjacentFriendly() && (pPlot->getOwner() != m_pPlayer->GetID()))
				{
					bool bAdjacent = false;
					bool bAdjacentDesire = false;
					for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
					{
						if(pkImprovementInfo->GetYieldAdjacentSameType((YieldTypes)ui) > 0)
						{
							bAdjacentDesire = true;
							break;
						}
						else if(pkImprovementInfo->GetYieldAdjacentTwoSameType((YieldTypes)ui) > 0)
						{
							bAdjacentDesire = true;
							break;
						}
					}
					//If yield bonus is granted from it being adjacent...
					CvPlot* pAdjacentPlot;
					if(bAdjacentDesire)
					{
						for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes) iDirectionLoop));

							if(pAdjacentPlot != NULL)
							{
								if((pAdjacentPlot->getOwner() == m_pPlayer->GetID()) && (pAdjacentPlot->getImprovementType() == eImprovement))
								{
									//Is the plot outside our land, but we can build on it, and get an adjacency bonus? Let's capitalize on this.
									iSecondaryScore += iSmallBuff;
									if(m_bLogging)
									{
										CvString strTemp;
										strTemp.Format("Weight,Found a Tile outside our Territory for our UI with Adjacency Bonus,%s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), iScore, pPlot->getX(), pPlot->getY());
										LogInfo(strTemp, m_pPlayer);
									}
									bAdjacent = true;
									break;
								}
							}
						}
						if(!bAdjacent)
						{
							iSecondaryScore += iSmallBuff;
							if(m_bLogging)
							{
								CvString strTemp;
								strTemp.Format("Weight,Found a Tile outside our Territory for our UI with Adjacency Bonus, but no adjacent tile yet... %s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), iScore, pPlot->getX(), pPlot->getY());
								LogInfo(strTemp, m_pPlayer);
							}
						}

					}
					else
					{
						//Is the plot outside our land, but we can build on it? Let's do this after everything else.
						iSecondaryScore -= iSmallBuff;
						if(m_bLogging)
						{
							CvString strTemp;
							strTemp.Format("Weight,Found a Tile outside our Territory for our UI,%s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), iScore, pPlot->getX(), pPlot->getY());
							LogInfo(strTemp, m_pPlayer);
						}
					}
				}
				else
				{
					iSecondaryScore += iSmallBuff;
					if(m_bLogging)
					{
						CvString strTemp;
						strTemp.Format("Weight,Found a Tile inside our Territory for our UI,%s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), iScore, pPlot->getX(), pPlot->getY());
						LogInfo(strTemp, m_pPlayer);
					}
				}
			}
		}
	}
#if defined(MOD_API_UNIFIED_YIELDS)
	int iTempWeight = 0;
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes) iI;
		if(pImprovement->GetYieldChange(iI) > 0)
		{

			switch(eYield)
			{
				case YIELD_FOOD:
					iTempWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_FOOD() * GC.getImprovementInfo(eImprovement)->GetYieldChange(iI);
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
					if (MOD_IMPROVEMENTS_EXTENSIONS)
					{
						iTempWeight += GC.getBUILDER_TASKING_BASELINE_ADDS_FOOD() * GC.getFeatureInfo(GC.getImprovementInfo(eImprovement)->GetCreatedFeature())->getYieldChange(iI);
					}
#endif
					break;
				case YIELD_PRODUCTION:
					iTempWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_PRODUCTION() * GC.getImprovementInfo(eImprovement)->GetYieldChange(iI);
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
					if (MOD_IMPROVEMENTS_EXTENSIONS)
					{
						iTempWeight += GC.getBUILDER_TASKING_BASELINE_ADDS_PRODUCTION() * GC.getFeatureInfo(GC.getImprovementInfo(eImprovement)->GetCreatedFeature())->getYieldChange(iI);
					}
#endif
					break;
				case YIELD_GOLD:
					iTempWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_GOLD() * GC.getImprovementInfo(eImprovement)->GetYieldChange(iI);
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
					if (MOD_IMPROVEMENTS_EXTENSIONS)
					{
						iTempWeight += GC.getBUILDER_TASKING_BASELINE_ADDS_GOLD() * GC.getFeatureInfo(GC.getImprovementInfo(eImprovement)->GetCreatedFeature())->getYieldChange(iI);
					}
#endif
					break;
				case YIELD_SCIENCE:
					iTempWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_SCIENCE() * GC.getImprovementInfo(eImprovement)->GetYieldChange(iI);
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
					if (MOD_IMPROVEMENTS_EXTENSIONS)
					{
						iTempWeight += GC.getBUILDER_TASKING_BASELINE_ADDS_SCIENCE() * GC.getFeatureInfo(GC.getImprovementInfo(eImprovement)->GetCreatedFeature())->getYieldChange(iI);
					}
#endif
					break;
				case YIELD_CULTURE:
					iTempWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_CULTURE() * GC.getImprovementInfo(eImprovement)->GetYieldChange(iI);
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
					if (MOD_IMPROVEMENTS_EXTENSIONS)
					{
						iTempWeight += GC.getBUILDER_TASKING_BASELINE_ADDS_CULTURE() * GC.getFeatureInfo(GC.getImprovementInfo(eImprovement)->GetCreatedFeature())->getYieldChange(iI);
					}
#endif
					break;
				case YIELD_FAITH:
					iTempWeight = GC.getBUILDER_TASKING_BASELINE_ADDS_FAITH() * GC.getImprovementInfo(eImprovement)->GetYieldChange(iI);
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
					if (MOD_IMPROVEMENTS_EXTENSIONS)
					{
						iTempWeight += GC.getBUILDER_TASKING_BASELINE_ADDS_FAITH() * GC.getFeatureInfo(GC.getImprovementInfo(eImprovement)->GetCreatedFeature())->getYieldChange(iI);
					}
#endif
					break;
			}

			int iAdjacentValue = pImprovement->GetYieldAdjacentSameType(eYield);
			int iAdjacentTwoValue = pImprovement->GetYieldAdjacentTwoSameType(eYield);
			int iAdjacentOtherValue = 0;
			int iAdjacentResourceValue = 0;
			int iAdjacentTerrainValue = 0;
			int iAdjacentFeatureValue = 0;
			for(int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
			{
				ImprovementTypes eImprovement = (ImprovementTypes)iJ;
				if(eImprovement != NO_IMPROVEMENT)
				{
					iAdjacentOtherValue += pImprovement->GetAdjacentImprovementYieldChanges(eImprovement, eYield);
				}
			}
			for(int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
			{
				ResourceTypes eResource = (ResourceTypes)iJ;
				if(eResource != NO_RESOURCE)
				{
					iAdjacentResourceValue += pImprovement->GetAdjacentResourceYieldChanges(eResource, eYield);
				}
			}
			for(int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
			{
				TerrainTypes eTerrain = (TerrainTypes)iJ;
				if(eTerrain != NO_TERRAIN)
				{
					iAdjacentTerrainValue += pImprovement->GetAdjacentTerrainYieldChanges(eTerrain, eYield);
				}
			}
			for(int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
			{
				FeatureTypes eFeature = (FeatureTypes)iJ;
				if (eFeature != NO_FEATURE)
				{
					iAdjacentFeatureValue += pImprovement->GetAdjacentFeatureYieldChanges(eFeature, eYield);
				}
			}

			if(iAdjacentValue > 0)
			{
				iYieldScore += (100 * pPlot->ComputeYieldFromAdjacentImprovement(*pImprovement, eImprovement, eYield));
			}
			if(iAdjacentTwoValue > 0)
			{
				iYieldScore += (100 * pPlot->ComputeYieldFromTwoAdjacentImprovement(*pImprovement, eImprovement, eYield));
			}
			if(iAdjacentOtherValue > 0)
			{
				iYieldScore += (100 * pPlot->ComputeYieldFromOtherAdjacentImprovement(*pImprovement, eYield));
			}
			if(iAdjacentTerrainValue > 0)
			{
				iYieldScore += (100 * pPlot->ComputeYieldFromAdjacentTerrain(*pImprovement, eYield));
			}
			if(iAdjacentResourceValue > 0)
			{
				iYieldScore += (100 * pPlot->ComputeYieldFromAdjacentResource(*pImprovement, eYield));
			}
			if (iAdjacentFeatureValue > 0)
			{
				iYieldScore += (100 * pPlot->ComputeYieldFromAdjacentFeature(*pImprovement, eYield));
			}

		}
	}
	if(pImprovement->GetCultureBombRadius() > 0)
	{
		int iAdjacentGood = 0;
		for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes) iDirectionLoop));

			if(pAdjacentPlot != NULL && pAdjacentPlot->getOwner() != m_pPlayer->GetID())
			{
				iAdjacentGood++;
			}
		}
		if(iAdjacentGood > 0)
		{
			iSecondaryScore += 100 * iAdjacentGood;
		}
	}
#endif

#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
	if (MOD_IMPROVEMENTS_EXTENSIONS)
	{
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);

		// improvement spawns resource?
		int iResourceChance = GC.getImprovementInfo(eImprovement)->GetRandomResourceChance();
		if (iResourceChance > 0 && pPlot->getResourceType() == NO_RESOURCE)
		{
			iSecondaryScore += 5 * iResourceChance;
		}

		// if it's Iroquois building forests give it even more weight since it connects cities.
		if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (pkImprovementInfo->GetCreatedFeature() == FEATURE_FOREST || pkImprovementInfo->GetCreatedFeature() == FEATURE_JUNGLE))
		{
			iSecondaryScore += iBigBuff;
		}
	}
#endif

	//feature considerations...
	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if (pkBuild)
		{
			//we remove this feature?
			if (pkBuild->isFeatureRemove(pPlot->getFeatureType()))
			{
				//how many do we have left?
				CvCity* pCity = pPlot->getOwningCity();
				if (pCity)
				{
					//we don't want to remove all features, we might need them later!
					int iNumFeatureRemaining = pCity->CountFeature(pPlot->getFeatureType());
					if (iNumFeatureRemaining <= 10)
					{
						iSecondaryScore -= (10 - iNumFeatureRemaining) * 100;
					}
				}
			}
			else
			{
				//bump to encourage these in cities with lots of features
				//how many do we have left?
				CvCity* pCity = pPlot->getOwningCity();
				if (pCity)
				{
					int iNumFeatureRemaining = pCity->CountFeature(pPlot->getFeatureType());
					iSecondaryScore += iNumFeatureRemaining * 50;
				}
			}
		}
	}

	//Let's get route things on routes, and not elsewhere.
	int iRouteScore = 0;
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes) iI;

		iRouteScore += pImprovement->GetRouteYieldChanges(ROUTE_RAILROAD, eYield) * 100;
		iRouteScore += pImprovement->GetRouteYieldChanges(ROUTE_ROAD, eYield) * 100;
	}
	if (iRouteScore > 0)
	{
		if (pPlot->IsCityConnection(m_pPlayer->GetID()) && (pPlot->IsRouteRailroad() || pPlot->IsRouteRoad()))
			iSecondaryScore += iRouteScore;
		else
			iSecondaryScore -= iRouteScore;
	}

	//City adjacenct improvement? Ramp it up!
	if(pImprovement->IsAdjacentCity() && pPlot->IsAdjacentCity())
	{
		iSecondaryScore += iBigBuff;
	}
	//Holy Sites should be built near Holy Cities.
	ImprovementTypes eHolySite = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_HOLY_SITE");
	if (eHolySite != NO_IMPROVEMENT && eHolySite == eImprovement)
	{
		ReligionTypes eReligion = m_pPlayer->GetReligions()->GetReligionCreatedByPlayer();
		if(eReligion != NO_RELIGION)
		{
			if(pPlot->getOwningCity() != NULL && pPlot->getOwningCity()->GetCityReligions()->IsHolyCityForReligion(eReligion))
			{
				iSecondaryScore += iBigBuff;
			}
		}
	}
	
	//Does this improvement connect a resource? Increase the score!
	if(eResource != NO_RESOURCE)
	{
		//for GPTI hooking up a resource is just a side benefit ... so no bonus
		if(pImprovement->IsImprovementResourceMakesValid(eResource) && !pImprovement->IsCreatedByGreatPerson())
		{
			iYieldScore += iBigBuff;
			//a new one? really buff it!!
			if (m_pPlayer->getNumResourceAvailable(eResource) <= 0)
				iYieldScore += iBigBuff;

			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			//amp up monopoly alloc!
			if (pkResource && pkResource->isMonopoly())
			{
				if (pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					if (m_pPlayer->GetMonopolyPercent(eResource) > 0 && m_pPlayer->GetMonopolyPercent(eResource) <= GC.getGLOBAL_RESOURCE_MONOPOLY_THRESHOLD())
						iYieldScore += iBigBuff + m_pPlayer->GetMonopolyPercent(eResource);
				}
				else
				{
					if (m_pPlayer->GetMonopolyPercent(eResource) > 0 && m_pPlayer->GetMonopolyPercent(eResource) <= GC.getSTRATEGIC_RESOURCE_MONOPOLY_THRESHOLD())
						iYieldScore += iBigBuff + m_pPlayer->GetMonopolyPercent(eResource);
					else if (m_pPlayer->GetMonopolyPercent(eResource) >= GC.getSTRATEGIC_RESOURCE_MONOPOLY_THRESHOLD() && m_pPlayer->GetMonopolyPercent(eResource) <= GC.getGLOBAL_RESOURCE_MONOPOLY_THRESHOLD())
						iYieldScore += iBigBuff + m_pPlayer->GetMonopolyPercent(eResource);
				}
			}
		}
	}
	//Do we have unimproved plots nearby? If so, let's not worry about replacing improvements right now.
	if(pPlot->getImprovementType() != NO_IMPROVEMENT && !pImprovement->IsAdjacentCity())
	{
		//first off, reduce the value, because this is time consuming. It had better be worth it!
		iYieldScore -= iMedBuff;

		//If our current improvement is obsolete, let's half it's value, so that the potential replacement is stronger.
		CvImprovementEntry* pOldImprovement = GC.getImprovementInfo(pPlot->getImprovementType());
		if((pOldImprovement->GetObsoleteTech() != NO_TECH) && GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)pOldImprovement->GetObsoleteTech()))
		{
			iYieldScore -= iMedBuff;
		}
		int iNumWorkedPlots = 0;
		int iNumImprovedPlots = 0;
		// Look at the city we'll be improving to see how essential another improvement is.
		if(pCity)
		{
			for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
			{
				ImprovementTypes eWorkingImprovement = (ImprovementTypes) iI;
				if(eWorkingImprovement != NO_IMPROVEMENT)
				{
					iNumImprovedPlots += pCity->GetNumImprovementWorked(eWorkingImprovement);
				}
			}
			iNumWorkedPlots = pCity->getPopulation();
			//if population is higher than # of plots, reduce value. Otherwise, increase it.
			//one will remove penalty above - more than that and it becomes useful.
			if (iNumWorkedPlots > iNumImprovedPlots)
			{
				iSecondaryScore -= ((iNumWorkedPlots - iNumImprovedPlots) * iSmallBuff);
			}
			else
			{
				iSecondaryScore += iSmallBuff * (iNumImprovedPlots - iNumWorkedPlots);
			}
		}
	}

	int iDefense = 0;
	//Fort test.
	ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
	if (eFort != NO_IMPROVEMENT && eImprovement == eFort && pPlot->canBuild(eBuild, m_pPlayer->GetID()))
	{
		//Is this a good spot for a defensive building?
		if(eResource == NO_RESOURCE)
		{
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				//looking to build a fort
				iDefense = pPlot->GetDefenseBuildValue(m_pPlayer->GetID());
				if(iDefense==0)
					return -1;

				if (iDefense > iSecondaryScore)
					iSecondaryScore = iDefense;
			}
		}
	}
	//Looking to build something else on top of a fort? It'd better be good.
	else if((eImprovement != eFort) && (pPlot->getImprovementType() != NO_IMPROVEMENT))
	{
		if(pPlot->getImprovementType() == eFort)
		{
			//looking to replace a fort
			iDefense = pPlot->GetDefenseBuildValue(m_pPlayer->GetID());
		}
		if ((iDefense * 3) > (iSecondaryScore * 2))
		{
			return -1;
		}
	}
#endif
	//if we're doing this solely for the secondary benefit, let's not overweight it. We need yields first and foremost.
	if (iYieldScore < iSecondaryScore)
		iSecondaryScore /= 2;

	iScore = iYieldScore + iSecondaryScore;

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
	return static_cast<FeatureTypes>(GC.getNUKE_FEATURE());
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

BuildTypes CvBuilderTaskingAI::GetRemoveRoute(void)
{
	// find the remove route build
	BuildTypes eRemoveRouteBuild = NO_BUILD;

	for (int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if (pkBuild && pkBuild->IsRemoveRoute())
		{
			eRemoveRouteBuild = eBuild;
			break;
		}
	}

	return eRemoveRouteBuild;
}

BuildTypes CvBuilderTaskingAI::GetBuildRoute(void)
{
	// find the route build
	BuildTypes eRouteBuild = NO_BUILD;
	RouteTypes eBestRoute = m_pPlayer->getBestRoute();

	for(int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild && pkBuild->getRoute() == eBestRoute)
		{
			eRouteBuild = eBuild;
			break;
		}
	}

	return eRouteBuild;
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

void CvBuilderTaskingAI::LogYieldInfo(CvString strNewLogStr, CvPlayer* pPlayer)
{
	if(!(GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging()))
	{
		return;
	}

	// Open the log file
	CvString strFileName = "BuilderTaskingYieldLog.csv";
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
		return;

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
		return;

	if(m_aDirectives.empty())
	{
		CvString strLog;
		CvString strTemp;

		strTemp.Format("%d,", pUnit->GetID()); // unit id
		strLog += strTemp;

		strLog += "No directives!";
		LogInfo(strLog, m_pPlayer);
	}
	else
	{
		for(size_t i = 0; i < m_aDirectives.size(); i++)
			LogDirective(m_aDirectives[i].option, pUnit, m_aDirectives[i].score);
	}
}

void CvBuilderTaskingAI::LogDirective(BuilderDirective directive, CvUnit* pUnit, int iWeight, bool bChosen)
{
	if(!m_bLogging)
		return;

	if (GC.getBuildInfo(directive.m_eBuild) == NULL)
		return;

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
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
#if defined(MOD_BALANCE_CORE)
		if((YieldTypes)ui <= YIELD_FAITH)
		{
#endif
		m_aiCurrentPlotYields[ui] = pPlot->getYield((YieldTypes)ui);

		if(m_bLogging){
			CvString strLog;
			YieldTypes yield = (YieldTypes) ui;
			strLog.Format("Plot Yield Update, %s, %i, %i, %i", FSerialization::toString(yield).c_str(), m_aiCurrentPlotYields[ui], pPlot->getX(), pPlot->getY());
			LogYieldInfo(strLog, m_pPlayer);
		}
#if defined(MOD_BALANCE_CORE)
		}
		else
			//zero it
			m_aiCurrentPlotYields[ui] = 0;
#endif
	}
}

// looks at the current plot assuming the build to see what it's worth
void CvBuilderTaskingAI::UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild)
{
	UpdateCurrentPlotYields(pPlot);

	const CvCity* pOwningCity = getOwningCity(pPlot);
	if (pOwningCity)
	{
		ReligionTypes eMajority = pOwningCity->GetCityReligions()->GetReligiousMajority();
		BeliefTypes eSecondaryPantheon = pOwningCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();

		const CvReligion* pReligion = (eMajority != NO_RELIGION) ? GC.getGame().GetGameReligions()->GetReligion(eMajority, pOwningCity->getOwner()) : 0;
		const CvBeliefEntry* pBelief = (eSecondaryPantheon != NO_BELIEF) ? GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon) : 0;

		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
#if defined(MOD_BALANCE_CORE)
			if ((YieldTypes)ui <= YIELD_FAITH)
			{
#endif
				m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, m_pPlayer->GetID(), pOwningCity, pReligion, pBelief);
				m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);

				if (m_bLogging){
					CvString strLog;
					YieldTypes yield = (YieldTypes)ui;
					strLog.Format("Plot Projected Yield Update, %s, %i, %i, %i", FSerialization::toString(yield).c_str(), m_aiProjectedPlotYields[ui], pPlot->getX(), pPlot->getY());
					LogYieldInfo(strLog, m_pPlayer);
				}
#if defined(MOD_BALANCE_CORE)
			}
			else
				//zero it!
				m_aiProjectedPlotYields[ui] = 0;
#endif
		}
	}
	else
	{
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
#if defined(MOD_BALANCE_CORE)
			if ((YieldTypes)ui <= YIELD_FAITH)
			{
#endif
				m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, m_pPlayer->GetID(), NULL, NULL, NULL);
				m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);

				if (m_bLogging){
					CvString strLog;
					YieldTypes yield = (YieldTypes)ui;
					strLog.Format("Plot Projected Yield Update, %s, %i, %i, %i", FSerialization::toString(yield).c_str(), m_aiProjectedPlotYields[ui], pPlot->getX(), pPlot->getY());
					LogYieldInfo(strLog, m_pPlayer);
				}
#if defined(MOD_BALANCE_CORE)
			}
			else
				//zero it!
				m_aiProjectedPlotYields[ui] = 0;
#endif
		}
	}
}
