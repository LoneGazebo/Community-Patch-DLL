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

	// special case to evaluate plots adjacent to friendly
	m_bEvaluateAdjacent = false;
	m_bMayPutGPTINextToCity = true;

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
					m_bMayPutGPTINextToCity = false; //keep those plots available
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
	m_bMayPutGPTINextToCity = true;
}

template<typename BuilderTaskingAI, typename Visitor>
void CvBuilderTaskingAI::Serialize(BuilderTaskingAI& builderTaskingAI, Visitor& visitor)
{
	visitor(builderTaskingAI.m_routeNeededPlots);
	visitor(builderTaskingAI.m_routeWantedPlots);
	visitor(builderTaskingAI.m_canalWantedPlots);
}

/// Serialization read
void CvBuilderTaskingAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvBuilderTaskingAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvBuilderTaskingAI& builderTaskingAI)
{
	builderTaskingAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvBuilderTaskingAI& builderTaskingAI)
{
	builderTaskingAI.Write(stream);
	return stream;
}

/// Update
void CvBuilderTaskingAI::Update(void)
{
	UpdateRoutePlots();
	UpdateCanalPlots();

	m_eRouteBuild = GetBuildRoute();

	if(m_bLogging)
	{
		bool bShowOutput = m_pPlayer->isHuman();

		if(m_pPlayer->IsEmpireUnhappy())
		{
			CvString str = "// Empire Unhappy! //";
			LogInfo(str, m_pPlayer, bShowOutput);
		}

		// show crisis states
		int iLoop;
		for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			CvString str("// ");
			str += pLoopCity->getName();
			str += " // missing ";

			YieldTypes eMissing = pLoopCity->GetCityStrategyAI()->GetMostDeficientYield();
			switch(eMissing)
			{
			case YIELD_FOOD:
				str += "food";
				break;
			case YIELD_PRODUCTION:
				str += "production";
				break;
			case YIELD_SCIENCE:
				str += "science";
				break;
			case YIELD_GOLD:
				str += "gold";
				break;
			case YIELD_CULTURE:
				str += "culture";
				break;
			case YIELD_FAITH:
				str += "faith";
				break;
			default:
				str += "unknown";
			}

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

	//see if the new route makes sense economically
	int iValue = -1;
	if(!bSamePlayer)
	{
		//this is for a quest ... normal considerations don't apply
		iValue = min(/*1000*/ GD_INT_GET(MINOR_CIV_ROUTE_QUEST_WEIGHT) / max(1, iPlotsNeeded), MAX_SHORT);
	}
	else
	{
		int iMaintenancePerTile = pRouteInfo->GetGoldMaintenance()*(100+m_pPlayer->GetImprovementGoldMaintenanceMod());
		int iGoldForRoute = m_pPlayer->GetTreasury()->GetCityConnectionRouteGoldTimes100(pTargetCity);

		//route has side benefits also (movement, village gold, trade route range, religion spread)
		int iSideBenefits = 500 + iRoadLength * 100;

		// give an additional bump if we're almost done (don't get distracted and leave half-finished roads)
		if (iPlotsNeeded > 0 && iPlotsNeeded < 3)
			iSideBenefits += 20000;

		//assume one unhappiness is worth gold per turn per city
		iSideBenefits += pTargetCity->GetUnhappinessFromIsolation() * (m_pPlayer->IsEmpireUnhappy() ? 200 : 100);

		if(GC.getGame().GetIndustrialRoute() == eRoute)
		{
			iSideBenefits += (pTargetCity->getYieldRate(YIELD_PRODUCTION, false) * /*25 in CP, 0 in VP*/ GD_INT_GET(INDUSTRIAL_ROUTE_PRODUCTION_MOD));
		}

		int iProfit = iGoldForRoute - (iRoadLength*iMaintenancePerTile) + iSideBenefits;

		if (!bHuman && iPlotsNeeded>3 && (iProfit < 0 || (iProfit + iNetGoldTimes100 < 0)))
			return;

		iValue = iProfit;
	}

	for (size_t i=0; i<path.vPlots.size(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if(!pPlot)
			break;

		if(pPlot->isCity())
			continue;

		//don't build roads if our trait gives the same benefit
		if (m_pPlayer->getBestRoute() == ROUTE_ROAD && m_pPlayer->GetPlayerTraits()->IsRiverTradeRoad() || m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus())
		{
			if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (pPlot->getFeatureType() == FEATURE_FOREST || pPlot->getFeatureType() == FEATURE_JUNGLE))
				continue;
			
			if (m_pPlayer->GetPlayerTraits()->IsRiverTradeRoad() && pPlot->isRiver())
				continue;
		}

		//remember it
		AddRoutePlot(pPlot, eRoute, iValue);
	}
}

void CvBuilderTaskingAI::ConnectCitiesForShortcuts(CvCity* pCity1, CvCity* pCity2, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if(pCity1->getOwner() != pCity2->getOwner())
		return;

	// don't connect razing cities
	if (pCity1->IsRazing() || pCity2->IsRazing())
		return;

	CvCity* pPlayerCapital = m_pPlayer->getCapitalCity();
	// only build a shortcut if both cities have a connection to the capital (including sea routes)
	if (!m_pPlayer->IsCityConnectedToCity(pCity1, pPlayerCapital, eRoute, false) || !m_pPlayer->IsCityConnectedToCity(pCity2, pPlayerCapital, eRoute, false))
		return;

	// build a path between the two cities - this will tend to re-use existing routes, unless the new path is much shorter
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eRoute);
	SPath newPath = GC.GetStepFinder().GetPath(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY(), data);

	// cities are not on the same landmass? then give up
	if(!newPath)
		return;

	for (size_t i=0; i<newPath.vPlots.size(); i++)
	{
		CvPlot* pPlot = newPath.get(i);
		if(pPlot->isCity())
			continue;

		if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (pPlot->getFeatureType() == FEATURE_FOREST || pPlot->getFeatureType() == FEATURE_JUNGLE))
			continue;

		if (m_pPlayer->GetPlayerTraits()->IsRiverTradeRoad() && pPlot->isRiver())
			continue;

		//remember it
		AddRoutePlot(pPlot, eRoute, 10);
	}
}

//helper function. don't panic if a a plot isn't marked for a short time
bool CvBuilderTaskingAI::WantRouteAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	RoutePlotContainer::const_iterator it = m_routeWantedPlots.find(pPlot->GetPlotIndex());
	return (it != m_routeWantedPlots.end());
}

bool CvBuilderTaskingAI::NeedRouteAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	RoutePlotContainer::const_iterator it = m_routeNeededPlots.find(pPlot->GetPlotIndex());
	return (it != m_routeNeededPlots.end());
}

bool CvBuilderTaskingAI::WantCanalAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	//do not check IsWaterAreaSeparator() but wait until there are cities on both sides
	set<int>::const_iterator it = m_canalWantedPlots.find(pPlot->GetPlotIndex());
	return (it != m_canalWantedPlots.end());
}

void CvBuilderTaskingAI::AddRoutePlot(CvPlot* pPlot, RouteTypes eRoute, int iValue)
{
	if (!pPlot)
		return;

	// if we already know about this plot, continue on
	if( GetRouteValue(pPlot)>=iValue )
		return;

	//if it is the right route, add to needed plots
	if (pPlot->getRouteType() == eRoute)
		m_routeNeededPlots[pPlot->GetPlotIndex()] = make_pair(eRoute, iValue);
	else
		//if no matching route, add to wanted plots
		m_routeWantedPlots[pPlot->GetPlotIndex()] = make_pair(eRoute, iValue);
}

int CvBuilderTaskingAI::GetRouteValue(CvPlot* pPlot)
{
	if (!pPlot)
		return false;

	int iCreateValue = 0;
	int iKeepValue = 0;
	RoutePlotContainer::const_iterator it;

	it = m_routeNeededPlots.find(pPlot->GetPlotIndex());
	if (it != m_routeNeededPlots.end())
		iKeepValue = it->second.second;

	it = m_routeWantedPlots.find(pPlot->GetPlotIndex());
	if (it != m_routeWantedPlots.end())
		iCreateValue = it->second.second;

	return max(iKeepValue,iCreateValue);
}

void CvBuilderTaskingAI::UpdateCanalPlots()
{
	//not needed every turn ... simple performance optimization
	if ((GC.getGame().getGameTurn() + m_pPlayer->GetID()) % 7 == 0)
		return;

	//only majors build canals
	if (m_pPlayer->isMinorCiv())
		return;

	m_canalWantedPlots.clear();

	vector<CvPlot*> vDestPlots;
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		int iCity;
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		for (CvCity* pDestCity = kLoopPlayer.firstCity(&iCity); pDestCity != NULL; pDestCity = kLoopPlayer.nextCity(&iCity))
			vDestPlots.push_back(pDestCity->plot());
	}

	int iOriginCityLoop;
	for (CvCity* pOriginCity = m_pPlayer->firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = m_pPlayer->nextCity(&iOriginCityLoop))
	{
		SPathFinderUserData data(m_pPlayer->GetID(), PT_TRADE_WATER);
		//this is the important flag
		data.iFlags = CvUnit::MOVEFLAG_PRETEND_CANALS;
		//doesn't really matter but we don't want to underestimate the real range, so use the real range
		data.iMaxNormalizedDistance = m_pPlayer->GetTrade()->GetTradeRouteRange(DOMAIN_SEA, pOriginCity);

		//get all paths
		map<CvPlot*, SPath> waterpaths = GC.GetStepFinder().GetMultiplePaths(pOriginCity->plot(), vDestPlots, data);
		for (map<CvPlot*, SPath>::iterator it = waterpaths.begin(); it != waterpaths.end(); ++it)
		{
			//the paths may contain not-yet-existing canals but the path cost for them is very high
			//so they should only be used if there really is no other way.
			SPath& currentPath = it->second;
			for (int i = 0; i < currentPath.length(); i++)
			{
				CvPlot* pCheck = currentPath.get(i);

				if (pCheck->isWater() || pCheck->isCity())
					continue;

				//existing canal in use, remember that so we don't replace it by something else
				if (pCheck->IsImprovementPassable())
				{
					if (pCheck->getOwner() == m_pPlayer->GetID())
						m_canalWantedPlots.insert(pCheck->GetPlotIndex());
				}
				else
				{
					//this is where we must build a new canal
					if (pCheck->getOwner() == m_pPlayer->GetID())
						m_canalWantedPlots.insert(pCheck->GetPlotIndex());
				}
			}
		}
	}
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

		//remember it
		AddRoutePlot(pPlot, eRoute, 100);
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

	//and this to see if we actually build it
	int iCost = pRouteInfo->GetGoldMaintenance()*(100 + m_pPlayer->GetImprovementGoldMaintenanceMod());
	iCost *= path.length();
	if (iNetGoldTimes100 - iCost <= 6)
		return;

	for (int i = 0; i<path.length(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if (!pPlot)
			break;

		if (pPlot->getOwner() != m_pPlayer->GetID())
			break;

		// remember the plot
		AddRoutePlot(pPlot, eRoute, 54);

		// for citadels also put routes on the neighboring plots ...
		if (TacticalAIHelpers::IsPlayerCitadel(pPlot, m_pPlayer->GetID()))
			for (int i = RING0_PLOTS; i < RING1_PLOTS; i++)
				AddRoutePlot(iterateRingPlots(pPlot, i), eRoute, 42);
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

	m_routeNeededPlots.clear();
	m_routeWantedPlots.clear();

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

				bool bConnectOnlyCapitals = /*1*/ GD_INT_GET(CITY_CONNECTIONS_CONNECT_TO_CAPITAL) > 0;
				if (bConnectOnlyCapitals)
				{
					// only need to build roads to the capital for the money and happiness
					if (!pFirstCity->isCapital() && !pSecondCity->isCapital() && iNetGoldTimes100 > 0)
					{
						//if we already have a connection to the capital, it may be possible to have a much shorter route for a direct connection
						//thus improving unit movement and gold bonus from villages
						ConnectCitiesForShortcuts(pFirstCity, pSecondCity, eBestRoute);
					}
					else
					{
						if (pFirstCity->isCapital() && pFirstCity->getOwner() == m_pPlayer->GetID())
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

	int iBestScore = INT_MAX;
	CvUnit* pBestWorker = NULL;

	for (std::map<CvUnit*, ReachablePlots>::const_iterator itUnit = allWorkersReachablePlots.begin(); itUnit != allWorkersReachablePlots.end(); ++itUnit)
	{
		ReachablePlots::const_iterator itPlot = itUnit->second.find(pTarget->GetPlotIndex());
		if (itPlot == itUnit->second.end())
			continue;

		int iTurns = itPlot->iPathLength;
		//-1 means the plot is already targeted by another worker
		if (iTurns < 0)
			return itUnit->first;

		//use distance as a tiebreaker
		int iScore = iTurns * 100 + plotDistance(*itUnit->first->plot(), *pTarget);
		if (iScore < iBestScore)
		{
			iBestScore = iScore;
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
		int iMoveTurnsAway = it->iPathLength;

		if(!ShouldBuilderConsiderPlot(pUnit, pPlot))
		{
			continue;
		}

		CvUnit* pBestWorker = FindBestWorker(allWorkersReachablePlots,pPlot);
		if (pBestWorker != pUnit)
		{
			continue;
		}

		//action may depend on city
		CvCity* pWorkingCity = pPlot->getEffectiveOwningCity();

		//in our own plots we can build anything
		if (pPlot->getOwner() == pUnit->getOwner())
		{
			UpdateCurrentPlotYields(pPlot);

			AddRouteDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			AddImprovingResourcesDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			AddImprovingPlotsDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			if (pUnit->AI_getUnitAIType() == UNITAI_WORKER)
			{
				AddChopDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
				AddScrubFalloutDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
				AddRepairTilesDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
				AddRemoveRouteDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			}
		}
		else if (m_bEvaluateAdjacent && !pPlot->isOwned() && pPlot->isAdjacentPlayer(pUnit->getOwner()))
		{
			UpdateCurrentPlotYields(pPlot);

			//some special improvements and roads
			AddImprovingPlotsDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			AddRouteDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			if (pUnit->AI_getUnitAIType() == UNITAI_WORKER)
			{
				// May want to repair and remove road tiles outside of our territory
				AddRepairTilesDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
				AddRemoveRouteDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			}
		}
		else
		{
			//only roads
			AddRouteDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			if (pUnit->AI_getUnitAIType() == UNITAI_WORKER)
			{
				// May want to repair and remove road tiles outside of our territory
				AddRepairTilesDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
				AddRemoveRouteDirectives(pUnit, pPlot, pWorkingCity, iMoveTurnsAway);
			}
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
void CvBuilderTaskingAI::AddImprovingResourcesDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pCity, int iMoveTurnsAway)
{
	// check to see if a resource is here. If not, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource == NO_RESOURCE)
		return;

	ImprovementTypes eExistingPlotImprovement = pPlot->getImprovementType();
	if (eExistingPlotImprovement != NO_IMPROVEMENT)
	{
		// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
		if (pPlot->HasSpecialImprovement() || (GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION) && GET_PLAYER(pUnit->getOwner()).isHuman()))
			return;

		if (pPlot->IsImprovementPillaged())
			return;
	}

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	CvResourceInfo* pkResource = GC.getResourceInfo(eResource);

	// loop through the build types to find one that we can use
	for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild == NULL)
			continue;

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
			continue;

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo == NULL)
			continue;

#if defined(MOD_BALANCE_CORE)
		//Check for test below.
		if(pkImprovementInfo->IsSpecificCivRequired() && m_pPlayer->getCivilizationType() != pkImprovementInfo->GetRequiredCivilization())
			continue;
#endif

		if (pkImprovementInfo->IsConnectsResource(eResource))
		{
			if(eImprovement == eExistingPlotImprovement)
				continue;

			if(!pUnit->canBuild(pPlot, eBuild))
				continue;

			BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE;
			int iWeight = /*300*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS);

			//slightly downward value for bonus resources relative to lux/strat
			if (pkResource->getResourceUsage() == RESOURCEUSAGE_BONUS)
			{
				iWeight *= 2;
				iWeight /= 3;
			}
			else if (pkImprovementInfo->IsCreatedByGreatPerson())
			{
				//if we're building a great person improvement, putting it on a lux/strat resource is the last resort ...
				iWeight = /*100*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS);
			}

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
						bPrevImprovementConnects = pkExistingImprovementInfo && pkExistingImprovementInfo->IsConnectsResource(eResource);
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

			iWeight = min(iWeight,0x7FFF);
			iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);
			iWeight -= plotDistance(*pUnit->plot(),*pPlot); //tiebreaker in case of multiple equal options

			int iScore = ScorePlotBuild(pPlot, eImprovement, eBuild);
			iScore = min(iScore,0x7FFF);

			if(iScore > 0)
			{
				if (pCity && pCity->GetCityCitizens()->IsWorkingPlot(pPlot))
					iScore *= 2;
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
void CvBuilderTaskingAI::AddImprovingPlotsDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pCity, int iMoveTurnsAway)
{
	ImprovementTypes eExistingImprovement = pPlot->getImprovementType();

	if (eExistingImprovement != NO_IMPROVEMENT)
	{
		// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
		if (pPlot->HasSpecialImprovement() || (GET_PLAYER(pUnit->getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION) && GET_PLAYER(pUnit->getOwner()).isHuman()))
		{
			if (m_bLogging)
			{
				CvString strTemp;
				strTemp.Format("Weight,Improvement Blocked by Special Improvement,%s,,,,%i, %i", GC.getImprovementInfo(pPlot->getImprovementType())->GetType(), pPlot->getX(), pPlot->getY());
				LogInfo(strTemp, m_pPlayer);
			}
			return;
		}
		if (pPlot->IsImprovementPillaged())
			return;
	}

	if(!m_bEvaluateAdjacent && !pPlot->isWithinTeamCityRadius(pUnit->getTeam()))
		return;

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
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

	if (!m_bEvaluateAdjacent && pPlot->getOwningCityID() == -1)
		return;

	// loop through the build types to find one that we can use
	for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild == NULL)
			continue;

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
			continue;

		CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);

		// for bonus resources, check to see if this is the improvement that connects it
		if(eResource != NO_RESOURCE)
		{
			if (!pImprovement->IsConnectsResource(eResource))
			{
				if(m_bLogging){
					CvString strTemp;
					strTemp.Format("Weight,!pImprovement->IsConnectsResource(eResource),%s,%i,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), eResource, pPlot->getX(), pPlot->getY());
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
			if(bWillRemoveForestOrJungle && eResource == NO_RESOURCE)
			{
				if(m_bLogging){
					CvString strTemp;
					strTemp.Format("Weight,Keep Forests,%s,,,,%i, %i", GC.getBuildInfo(eBuild)->GetType(), pPlot->getX(), pPlot->getY());
					LogInfo(strTemp, m_pPlayer);
				}
				continue;
			}
		}

		int iScore = ScorePlotBuild(pPlot, eImprovement, eBuild);
		if (pCity && pCity->GetCityCitizens()->IsWorkingPlot(pPlot))
			iScore *= 2;

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
		int iWeight = /*100*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS);
		iWeight = GetBuildCostWeight(iWeight, pPlot, eBuild);

		int iBuildTimeWeight = GetBuildTimeWeight(pUnit, pPlot, eBuild, DoesBuildHelpRush(pUnit, pPlot, eBuild));
		iWeight += iBuildTimeWeight;

		if (bWillRemoveForestOrJungle)
		{
			if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus())
				iWeight /= 100;
		}

		if (eFeature != NO_FEATURE && pkBuild->isFeatureRemove(eFeature))
		{
			CvCity* pCity = pPlot->getEffectiveOwningCity();
			if (pCity)
			{
				int iWeightPenalty = 0;
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					YieldTypes eYield = (YieldTypes)iI;
					if (pCity->GetYieldPerXFeature(eFeature, eYield) > 0)
						iWeightPenalty++;

					if (pCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield) > 0)
						iWeightPenalty++;

					if (pCity->GetYieldPerXFeatureFromReligion(eFeature, eYield) > 0)
						iWeightPenalty++;

					if (pCity->GetYieldPerTurnFromUnimprovedFeatures(eFeature, eYield) > 0)
						iWeightPenalty++;

					if (pCity->GetFeatureExtraYield(eFeature, eYield) > 0)
						iWeightPenalty++;
				}

				iWeight /= max(1, iWeightPenalty);
			}
		}

		iWeight = min(iWeight,0x7FFF);
		iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);
		iWeight -= plotDistance(*pUnit->plot(), *pPlot); //tiebreaker in case of multiple equal options

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
void CvBuilderTaskingAI::AddRemoveRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* /*pCity*/, int iMoveTurnsAway)
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
	if (WantRouteAtPlot(pPlot))
		return;

	// keep routes which are needed
	if (NeedRouteAtPlot(pPlot))
		return;

	// don't remove routes which we did not create
	if (pPlot->GetPlayerResponsibleForRoute() != NO_PLAYER && pPlot->GetPlayerResponsibleForRoute() != m_pPlayer->GetID())
		return;

	//don't touch master's roads!
	if (pPlot->GetPlayerResponsibleForRoute() != NO_PLAYER && pPlot->GetPlayerResponsibleForRoute() != m_pPlayer->GetID())
		if (GET_TEAM(m_pPlayer->getTeam()).IsVassal(GET_PLAYER(pPlot->GetPlayerResponsibleForRoute()).getTeam()))
			return;

	//we want to be aggressive with this because of the cost.
	int iWeight = /*250*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_ROUTES)/3;

	//if in debt, bump it up.
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	if(m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
		iWeight *= 3;

	BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::REMOVE_ROAD;

	iWeight = GetBuildCostWeight(iWeight, pPlot, m_eRemoveRouteBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eRemoveRouteBuild, false);
	iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);
	iWeight -= plotDistance(*pUnit->plot(), *pPlot); //tiebreaker in case of multiple equal options

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
void CvBuilderTaskingAI::AddRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* /*pCity*/, int iMoveTurnsAway)
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
	if (!WantRouteAtPlot(pPlot))
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

	int iWeight = /*750*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_ROUTES);
	BuilderDirective::BuilderDirectiveType eDirectiveType = BuilderDirective::BUILD_ROUTE;

	iWeight = GetBuildCostWeight(iWeight, pPlot, m_eRouteBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eRouteBuild, false);
	iWeight += GetRouteValue(pPlot);
	iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);
	iWeight -= plotDistance(*pUnit->plot(), *pPlot); //tiebreaker in case of multiple equal options

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
void CvBuilderTaskingAI::AddChopDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pCity, int iMoveTurnsAway)
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

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	// check to see if a resource is here. If so, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource != NO_RESOURCE)
	{
		return;
	}

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

	int iWeight = GetBuildCostWeight(/*1000*/ GD_INT_GET(BUILDER_TASKING_BASELINE_REPAIR), pPlot, eChopBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, eChopBuild, false);
	iWeight *= iProduction; // times the amount that the plot produces from the chopping

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
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*3*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD);
				}
				break;
			case YIELD_PRODUCTION:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_PRODUCTION")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*2*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION);
				}
				break;
			case YIELD_GOLD:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GOLD")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*2*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD);
				}
				break;
			case YIELD_SCIENCE:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*2*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE);
				}
				break;
			case YIELD_CULTURE:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CULTURE")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*3*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE);
				}
				break;
			case YIELD_FAITH:
				break;
			case YIELD_TOURISM:
				break;
			case YIELD_GOLDEN_AGE_POINTS:
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

	iWeight = min(iWeight, 0x7FFF);
	iWeight /= (iMoveTurnsAway * iMoveTurnsAway + 1);
	iWeight -= plotDistance(*pUnit->plot(), *pPlot); //tiebreaker in case of multiple equal options

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

void CvBuilderTaskingAI::AddRepairTilesDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pWorkingCity, int iMoveTurnsAway)
{
	if (!pPlot)
	{
		return;
	}

	bool isOwned = pPlot->isOwned();
	bool isOwnedByUs = pPlot->getOwner() == pUnit->getOwner();
	// If it's owned by someone else, ignore it
	if (isOwned && !isOwnedByUs)
	{
		return;
	}
	// If it's owned by us, but it's being razed, ignore it (check actual owning city instead of working city)
	if (isOwnedByUs && pPlot->getOwningCity()->IsRazing())
	{
		return;
	}
	bool isPillagedRouteWeWantToRepair = NeedRouteAtPlot(pPlot) && pPlot->IsRoutePillaged();
	// If it's not owned by us, and it's not a route we want to repair, ignore it
	if (!isOwned && !isPillagedRouteWeWantToRepair)
	{
		return;
	}

	// if city owning this plot is being razed, and it's not a route we want to repair, ignore this plot
	if (pWorkingCity && pWorkingCity->IsRazing() && !isPillagedRouteWeWantToRepair)
	{
		return;
	}

	//nothing pillaged here? hmm...
	if (!pPlot->IsImprovementPillaged() && !pPlot->IsRoutePillaged())
	{
		return;
	}

	int iWeight = GetBuildCostWeight(100 * /*1000*/ GD_INT_GET(BUILDER_TASKING_BASELINE_REPAIR), pPlot, m_eRepairBuild);
	iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eRepairBuild, false);
	iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);
	iWeight -= plotDistance(*pUnit->plot(), *pPlot); //tiebreaker in case of multiple equal options

	if (pPlot->isRevealedFortification(m_pPlayer->getTeam()))
		iWeight *= 10;

	if (pWorkingCity && pWorkingCity->GetCityCitizens()->IsWorkingPlot(pPlot))
		iWeight *= 2;

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
void CvBuilderTaskingAI::AddScrubFalloutDirectives(CvUnit* pUnit, CvPlot* pPlot, CvCity* pCity, int iMoveTurnsAway)
{
	if(m_eFalloutFeature == NO_FEATURE || m_eFalloutRemove == NO_BUILD)
	{
		return;
	}

	if (pPlot->getTeam() != pUnit->getTeam())
	{
		return;
	}

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	if(pPlot->getFeatureType() == m_eFalloutFeature && pUnit->canBuild(pPlot, m_eFalloutRemove))
	{
		int iWeight = GetBuildCostWeight(1000 * /*20000*/ GD_INT_GET(BUILDER_TASKING_BASELINE_SCRUB_FALLOUT), pPlot, m_eFalloutRemove);
		iWeight += GetBuildTimeWeight(pUnit, pPlot, m_eFalloutRemove, false);
		iWeight /= (iMoveTurnsAway*iMoveTurnsAway + 1);
		iWeight -= plotDistance(*pUnit->plot(), *pPlot); //tiebreaker in case of multiple equal options

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
		// As embarked workers can now build fishing boats, we need to consider coastal plots
		if(pPlot->isDeepWater())
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

	//check if we could be captured
	vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pPlot, pUnit->getTeam());
	bool bMayStay = vAttackers.empty() || (vAttackers.size() == 1 && pPlot->getBestDefender(pUnit->getOwner()) != NULL);
	if (!bMayStay)
	{
		if(GC.getLogging() && GC.getAILogging() && m_bLogging)
		{
			CvString strLog;
			strLog.Format("plotX: %d plotY: %d, danger: %d, bailing due to potential attackers", pPlot->getX(), pPlot->getY(), m_pPlayer->GetPlotDanger(*pPlot, true));
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLog);
		}
		return false;
	}

	//if there is fallout, try to scrub it in spite of the danger
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

	//danger check is not enough - we don't want to be adjacent to enemy territory for example
	if (pPlot->isVisibleToEnemy(pUnit->getOwner()))
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
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
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
	CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
	if (!pkResource)
		return 0;

	int iWeight = 0;

	for (int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		int iResourceFlavor = pkResource->getFlavorValue((FlavorTypes)i);
		int iPersonalityFlavorValue = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)i);
		int iResult = iResourceFlavor * iPersonalityFlavorValue;

		if (iResult > 0)
		{
			iWeight += iResult;
		}

		int iImprovementFlavor = 1;
		if (eImprovement != NO_IMPROVEMENT)
		{
			iImprovementFlavor = GC.getImprovementInfo(eImprovement)->GetFlavorValue(i);
		}

		int iUsableByCityWeight = iPersonalityFlavorValue * iImprovementFlavor;
		if (iUsableByCityWeight > 0)
		{
			iWeight += iUsableByCityWeight;
		}
	}

	// if the empire is unhappy (or close to it) and this is a luxury resource the player doesn't have, provide a super bonus to getting it
	if (pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
	{
		int iModifier = pkResource->getHappiness() * /*750*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE);

		if (m_pPlayer->getNumResourceAvailable(eResource) > 0)
			iModifier /= 10; 

		iWeight *= iModifier;
	}
	else if (pkResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
	{
		if (m_pPlayer->IsResourceCityTradeable(eResource))
		{
			// measure quantity
			int iMultiplyingAmount = iQuantity * 200;

			// if we don't have any currently available
			if (m_pPlayer->getNumResourceAvailable(eResource) == 0)
				iMultiplyingAmount *= 4;

			iWeight *= iMultiplyingAmount;
		}
	}

	return iWeight;
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

	if(eImprovement == NO_IMPROVEMENT || eBuild == NO_BUILD)
		return -1;

	CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
	if(!pImprovement)
		return -1;

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

		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			int iYieldDelta = m_aiProjectedPlotYields[ui] - m_aiCurrentPlotYields[ui] * 100;
			if(iYieldDelta >= 0)
			{
				iYieldScore += m_aiProjectedPlotYields[ui]; // provide a nominal score to plots that improve anything
			}
			else if(iYieldDelta < 0)
			{
				iYieldScore += iYieldDelta;
			}
		}
			
		//Because this evaluates territory outside of our base territory, let's cut this down a bit.
		iYieldScore -= iSmallBuff;
	}

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
			if (iYieldDelta > 0 && pCityStrategy->GetMostDeficientYield() == (YieldTypes)ui)
				iYieldScore += iYieldDelta * 5;
			else
				iYieldScore += iYieldDelta;
		}

		if (pCity->isCapital() || pCity->IsOriginalCapital()) // this is our capital and needs emphasis
		{
			iYieldScore += iSmallBuff;
		}

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

		//Existing improvement? Let's may not put great person improvements here.
		if(pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if(pPlot->HasSpecialImprovement())
			{
				iSecondaryScore -= iBigBuff;
			}
		}
		else
		{
			if (pImprovement->IsCreatedByGreatPerson())
			{
				//good plot doesn't have a resource and is not adjacent to another team
				if (pPlot->getResourceType(pCity->getTeam()) == NO_RESOURCE)
				{
					if ((m_bMayPutGPTINextToCity || !pPlot->IsAdjacentCity()) && !pPlot->IsAdjacentOwnedByTeamOtherThan(pCity->getTeam()))
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
		if(eResourceFromImprovement != NO_RESOURCE)
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

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes) iI;

		// Simplification - errata yields not worth considering.
		if (eYield > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		if (pImprovement->GetYieldChange(iI) > 0)
		{
			switch (eYield)
			{
			case NO_YIELD:
				UNREACHABLE();
			case YIELD_FOOD:
				iYieldScore += pImprovement->GetYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FOOD);

				if (MOD_IMPROVEMENTS_EXTENSIONS)
				{
					CvFeatureInfo* pFeature = GC.getFeatureInfo(pImprovement->GetCreatedFeature());
					if (pFeature)
					{
						iYieldScore += pFeature->getYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FOOD);
					}
				}

				break;
			case YIELD_PRODUCTION:
				iYieldScore += pImprovement->GetYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_PRODUCTION);

				if (MOD_IMPROVEMENTS_EXTENSIONS)
				{
					CvFeatureInfo* pFeature = GC.getFeatureInfo(pImprovement->GetCreatedFeature());
					if (pFeature)
					{
						iYieldScore += pFeature->getYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_PRODUCTION);
					}
				}

				break;
			case YIELD_GOLD:
				iYieldScore += pImprovement->GetYieldChange(iI) * /*40*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_GOLD);

				if (MOD_IMPROVEMENTS_EXTENSIONS)
				{
					CvFeatureInfo* pFeature = GC.getFeatureInfo(pImprovement->GetCreatedFeature());
					if (pFeature)
					{
						iYieldScore += pFeature->getYieldChange(iI) * /*40*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_GOLD);
					}
				}

				break;
			case YIELD_SCIENCE:
				iYieldScore += pImprovement->GetYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_SCIENCE);

				if (MOD_IMPROVEMENTS_EXTENSIONS)
				{
					CvFeatureInfo* pFeature = GC.getFeatureInfo(pImprovement->GetCreatedFeature());
					if (pFeature)
					{
						iYieldScore += pFeature->getYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_SCIENCE);
					}
				}

				break;
			case YIELD_CULTURE:
				iYieldScore += pImprovement->GetYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_CULTURE);

				if (MOD_IMPROVEMENTS_EXTENSIONS)
				{
					CvFeatureInfo* pFeature = GC.getFeatureInfo(pImprovement->GetCreatedFeature());
					if (pFeature)
					{
						iYieldScore += pFeature->getYieldChange(iI) * /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_CULTURE);
					}
				}

				break;
			case YIELD_FAITH:
				iYieldScore += pImprovement->GetYieldChange(iI) * /*150*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FAITH);

				if (MOD_IMPROVEMENTS_EXTENSIONS)
				{
					CvFeatureInfo* pFeature = GC.getFeatureInfo(pImprovement->GetCreatedFeature());
					if (pFeature)
					{
						iYieldScore += pFeature->getYieldChange(iI) * /*150*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FAITH);
					}
				}

				break;
			case YIELD_TOURISM:
			case YIELD_GOLDEN_AGE_POINTS:
			case YIELD_GREAT_GENERAL_POINTS:
			case YIELD_GREAT_ADMIRAL_POINTS:
			case YIELD_POPULATION:
			case YIELD_CULTURE_LOCAL:
			case YIELD_JFD_HEALTH:
			case YIELD_JFD_DISEASE:
			case YIELD_JFD_CRIME:
			case YIELD_JFD_LOYALTY:
			case YIELD_JFD_SOVEREIGNTY:
				break; // TODO: These yields have no baseline.
			}
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
			iYieldScore += (100 * pPlot->ComputeYieldFromAdjacentResource(*pImprovement, eYield, GET_PLAYER(m_pPlayer->GetID()).getTeam()));
		}
		if (iAdjacentFeatureValue > 0)
		{
			iYieldScore += (100 * pPlot->ComputeYieldFromAdjacentFeature(*pImprovement, eYield));
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
				CvCity* pCity = pPlot->getEffectiveOwningCity();
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
				CvCity* pCity = pPlot->getEffectiveOwningCity();
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

	//City adjacenct improvement? Ramp it up - other stuff can move somewhere else
	if(pImprovement->IsAdjacentCity() && pPlot->IsAdjacentCity())
	{
		iSecondaryScore += iBigBuff;
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
					if (m_pPlayer->GetMonopolyPercent(eResource) > 0 && m_pPlayer->GetMonopolyPercent(eResource) <= /*50*/ GD_INT_GET(GLOBAL_RESOURCE_MONOPOLY_THRESHOLD))
						iYieldScore += iBigBuff + m_pPlayer->GetMonopolyPercent(eResource);
				}
				else
				{
					if (m_pPlayer->GetMonopolyPercent(eResource) > 0 && m_pPlayer->GetMonopolyPercent(eResource) <= /*25*/ GD_INT_GET(STRATEGIC_RESOURCE_MONOPOLY_THRESHOLD))
						iYieldScore += iBigBuff + m_pPlayer->GetMonopolyPercent(eResource);
					else if (m_pPlayer->GetMonopolyPercent(eResource) >= /*25*/ GD_INT_GET(STRATEGIC_RESOURCE_MONOPOLY_THRESHOLD) && m_pPlayer->GetMonopolyPercent(eResource) <= /*50*/ GD_INT_GET(GLOBAL_RESOURCE_MONOPOLY_THRESHOLD))
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

	//Fort test.
	static ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
	if (eFort != NO_IMPROVEMENT && eImprovement == eFort && pPlot->canBuild(eBuild, m_pPlayer->GetID()))
	{
		//Is this a good spot for a defensive building?
		if(eResource == NO_RESOURCE)
		{
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				//looking to build a fort
				int iFortScore = pPlot->GetDefenseBuildValue(m_pPlayer->GetID());
				if (MOD_GLOBAL_PASSABLE_FORTS && WantCanalAtPlot(pPlot))
					iFortScore += iBigBuff*3;

				if(iFortScore==0)
					return -1;

				if (iFortScore > iSecondaryScore)
					iSecondaryScore = iFortScore;
			}
		}
	}
	//Looking to build something else on top of a fort? It'd better be good.
	else if(eImprovement != eFort && pPlot->getImprovementType() == eFort)
	{
		//looking to replace a fort - score might have changed because the border moved!
		int iFortScore = pPlot->GetDefenseBuildValue(m_pPlayer->GetID());
		if (MOD_GLOBAL_PASSABLE_FORTS && WantCanalAtPlot(pPlot))
			iFortScore += iBigBuff*3;

		if (iFortScore * 4 > iSecondaryScore * 3)
			return -1;
	}

	return iYieldScore + iSecondaryScore;
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
	return static_cast<FeatureTypes>(GD_INT_GET(NUKE_FEATURE));
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
void CvBuilderTaskingAI::LogInfo(const CvString& strNewLogStr, CvPlayer* pPlayer, bool /*bWriteToOutput*/)
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

void CvBuilderTaskingAI::LogYieldInfo(const CvString& strNewLogStr, CvPlayer* pPlayer)
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
	case BuilderDirective::REMOVE_ROAD:
		strLog += "REMOVE_ROAD";
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

	const CvCity* pOwningCity = pPlot->getEffectiveOwningCity();
	if (pOwningCity)
	{
		ReligionTypes eMajority = pOwningCity->GetCityReligions()->GetReligiousMajority();
		BeliefTypes eSecondaryPantheon = pOwningCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();

		const CvReligion* pReligion = (eMajority != NO_RELIGION) ? GC.getGame().GetGameReligions()->GetReligion(eMajority, pOwningCity->getOwner()) : 0;
		const CvBeliefEntry* pBelief = (eSecondaryPantheon != NO_BELIEF) ? GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon) : 0;

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
		const CvReligion* pPantheon = NULL;
		BeliefTypes ePantheonBelief = NO_BELIEF;
		// Mod for civs keeping their pantheon belief forever
		if (MOD_RELIGION_PERMANENT_PANTHEON)
		{
			pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, pOwningCity->getOwner());
			ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(pOwningCity->getOwner());
		}
#endif

		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
#if defined(MOD_BALANCE_CORE)
			if ((YieldTypes)ui <= YIELD_FAITH)
			{
#endif
				m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, m_pPlayer->GetID(), pOwningCity, pReligion, pBelief);
				m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
				if (MOD_RELIGION_PERMANENT_PANTHEON)
				{
					if (GC.getGame().GetGameReligions()->HasCreatedPantheon(m_pPlayer->GetID()))
					{
						if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
						{
							if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eMajority, m_pPlayer->GetID()))) // check that the our religion does not have our belief, to prevent double counting
							{
								m_aiProjectedPlotYields[ui] += pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, m_pPlayer->GetID(), pOwningCity, pPantheon, NULL);
								m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);
							}
						}
					}
				}
#endif

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
