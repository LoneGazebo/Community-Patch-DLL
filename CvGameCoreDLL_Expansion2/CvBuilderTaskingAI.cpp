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

	m_bLogging = GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging();

	// if we get a unique improvement that needs a certain feature, leave that feature until tech is researched
	for (int i = 0; i < NUM_FEATURE_TYPES; i++)
	{
		m_aiSaveFeatureUntilTech[i] = NO_TECH;
	}

	// special case to evaluate plots adjacent to city
	m_iSaveCityAdjacentUntilTech = NO_TECH;

	CivilizationTypes eCiv = m_pPlayer->getCivilizationType();

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

		if (pkImprovementInfo->IsSpecificCivRequired())
		{
			CivilizationTypes eRequiredCiv = pkImprovementInfo->GetRequiredCivilization();
			if (eRequiredCiv == eCiv)
			{
				for (int i = 0; i < NUM_FEATURE_TYPES; i++)
				{
					if (pkImprovementInfo->GetFeatureMakesValid(i))
					{
						m_aiSaveFeatureUntilTech[i] = (TechTypes)pkBuild->getTechPrereq();
					}
				}
				if (pkImprovementInfo->IsAdjacentCity())
				{
					m_iSaveCityAdjacentUntilTech = (TechTypes)pkBuild->getTechPrereq();
				}
			}
		}
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

	for (int i = 0; i < NUM_FEATURE_TYPES; i++)
		m_aiSaveFeatureUntilTech[i] = NO_TECH;
	m_iSaveCityAdjacentUntilTech = NO_TECH;
}

template<typename BuilderTaskingAI, typename Visitor>
void CvBuilderTaskingAI::Serialize(BuilderTaskingAI& builderTaskingAI, Visitor& visitor)
{
	visitor(builderTaskingAI.m_routeNeededPlots);
	visitor(builderTaskingAI.m_canalWantedPlots);
	visitor(builderTaskingAI.m_mainRoutePlots);
	visitor(builderTaskingAI.m_shortcutRoutePlots);
	visitor(builderTaskingAI.m_strategicRoutePlots);
	visitor(builderTaskingAI.m_aiSaveFeatureUntilTech);
	visitor(builderTaskingAI.m_iSaveCityAdjacentUntilTech);
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
	UpdateImprovementPlots();

	if(m_bLogging)
	{
		bool bShowOutput = m_pPlayer->isHuman();

		if(m_pPlayer->IsEmpireUnhappy())
		{
			CvString str = "// Empire Unhappy! //";
			LogInfo(str, m_pPlayer, bShowOutput);
		}

		// show crisis states
		int iLoop = 0;
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

void CvBuilderTaskingAI::ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, BuildTypes eBuild, RouteTypes eRoute)
{
	if (pPlayerCapital->getOwner() != m_pPlayer->GetID())
		return;

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
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eBuild,eRoute,true);
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

	// for quests we might be targeting a city state ...
	bool bTargetingMinor = GET_TEAM(pTargetCity->getTeam()).isMinorCiv();

	// There's no reason for building railroads for city state quests
	if (bTargetingMinor && eRoute == ROUTE_RAILROAD)
		return;

	// go through the route to see how long it is and how many plots already have roads
	int iRoadLength = 0;
	int iRoadMaintenanceLength = 0;
	int iNumRoadsNeededToBuild = 0;

	for (size_t i = 1; i < path.vPlots.size() - 1; i++)
	{
		CvPlot* pPlot = path.get(i);
		if(!pPlot)
			break;

		//don't count the cities themselves
		if (pPlot->isCity())
			continue;

		iRoadLength++;

		if (!GetSameRouteBenefitFromTrait(pPlot, eRoute))
		{
			iRoadMaintenanceLength++;

			if (pPlot->getRouteType() < eRoute || pPlot->IsRoutePillaged())
				iNumRoadsNeededToBuild++;
		}
	}

	//see if the new route makes sense economically
	int iValue = -1;
	if(bTargetingMinor)
	{
		//this is for a quest ... normal considerations don't apply
		iValue = min(/*1000*/ GD_INT_GET(MINOR_CIV_ROUTE_QUEST_WEIGHT) / max(1, iNumRoadsNeededToBuild), MAX_SHORT);
	}
	else
	{
		bool bHasCityConnection = m_pPlayer->IsCityConnectedToCity(pPlayerCapital, pTargetCity, eRoute);

		//route has side benefits also (movement, village gold, trade route range, religion spread)
		iValue = /*750*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_ROUTES);

		// Focus on almost completed roads first
		if (iNumRoadsNeededToBuild <= 40 && iNumRoadsNeededToBuild > 0)
			iValue += 1600 / (iNumRoadsNeededToBuild * iNumRoadsNeededToBuild);

		//assume one unhappiness is worth gold per turn per city
		iValue += !bHasCityConnection ? pTargetCity->GetUnhappinessFromIsolation() * (m_pPlayer->IsEmpireUnhappy() ? 300 : 100) : 0;

		if(GC.getGame().GetIndustrialRoute() == eRoute)
		{
			if (!bHasCityConnection)
			{
				iValue += pTargetCity->getYieldRate(YIELD_PRODUCTION, false) * /*25 in CP, 0 in VP*/ GD_INT_GET(INDUSTRIAL_ROUTE_PRODUCTION_MOD);

#if defined(MOD_BALANCE_CORE)
				// Target city would get a production and gold boost from a train station.
				for (int iBuildingIndex = 0; iBuildingIndex < GC.getNumBuildingInfos(); iBuildingIndex++)
				{
					BuildingTypes eBuilding = (BuildingTypes)iBuildingIndex;
					CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding == NULL)
						continue;

					bool bRequiresRail = pkBuilding->IsRequiresRail();
					if (!bRequiresRail)
						continue;

					int iProductionYield = pTargetCity->getYieldRate(YIELD_PRODUCTION, false);
					int iGoldYield = pTargetCity->getYieldRate(YIELD_GOLD, false);
					int iProductionYieldRateModifier = pkBuilding->GetYieldModifier(YIELD_PRODUCTION);
					int iGoldYieldRateModifier = pkBuilding->GetYieldModifier(YIELD_GOLD);

					if (pTargetCity->HasBuilding(eBuilding))
					{
						iValue += 100 * iProductionYield * iProductionYieldRateModifier / (100 + iProductionYieldRateModifier);
						iValue += 100 * iGoldYield * iGoldYieldRateModifier / (100 + iGoldYieldRateModifier);
					}
					else if (m_pPlayer->canConstruct(eBuilding) || eBuilding == pTargetCity->getProductionBuilding())
					{
						iValue += iProductionYield * iProductionYieldRateModifier;
						iValue += iGoldYield * iGoldYieldRateModifier;
					}
				}
#endif
			}
		}
	}

	for (size_t i=0; i<path.vPlots.size(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if(!pPlot)
			break;

		if(pPlot->isCity())
			continue;

		//remember it
		if (AddRoutePlot(pPlot, eRoute, iValue))
			m_mainRoutePlots.insert(pPlot->GetPlotIndex());
	}
}

void CvBuilderTaskingAI::ConnectCitiesForShortcuts(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if(pCity1->getOwner() != pCity2->getOwner())
		return;

	// don't connect razing cities
	if (pCity1->IsRazing() || pCity2->IsRazing())
		return;

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if (!pRouteInfo)
	{
		return;
	}

	// build a path between the two cities - this will tend to re-use existing routes, unless the new path is much shorter
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eBuild,eRoute,true);
	SPath newPath = GC.GetStepFinder().GetPath(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY(), data);

	// cities are not on the same landmass? then give up
	if(!newPath)
		return;

	// go through the route to see how long it is and how many plots already have roads
	int iRoadLength = 0;
	int iRoadMaintenanceLength = 0;

	for (size_t i = 1; i < newPath.vPlots.size() - 1; i++)
	{
		CvPlot* pPlot = newPath.get(i);
		if (!pPlot)
			break;

		//don't count the cities themselves
		if (pPlot->isCity())
		{
			// if we are going via a city, that isn't one of the two cities we are connecting, then this shortcut is not needed
			if (pPlot->getOwner() == m_pPlayer->GetID())
				return;
			continue;
		}

		iRoadLength++;

		if (!GetSameRouteBenefitFromTrait(pPlot, eRoute))
			iRoadMaintenanceLength++;
	}

	//route has side benefits also (movement, village gold, trade route range, religion spread)
	int iValue = /*750*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_ROUTES);

	for (size_t i=0; i<newPath.vPlots.size(); i++)
	{
		CvPlot* pPlot = newPath.get(i);
		if (!pPlot)
			break;

		if (pPlot->isCity())
			continue;

		//remember it
		if (AddRoutePlot(pPlot, eRoute, iValue))
			m_shortcutRoutePlots.insert(pPlot->GetPlotIndex());
	}
}

void CvBuilderTaskingAI::ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, BuildTypes eBuild, RouteTypes eRoute, int iNetGoldTimes100)
{
	// don't connect cities from different owners
	if (pOriginCity->getOwner() != pTargetPlot->getOwner())
		return;

	// don't connect razing cities
	if (pOriginCity->IsRazing())
		return;

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if (!pRouteInfo)
		return;

	// build a path between the two cities
	SPathFinderUserData data(m_pPlayer->GetID(), PT_BUILD_ROUTE, eBuild, eRoute, false);
	SPath path = GC.GetStepFinder().GetPath(pOriginCity->getX(), pOriginCity->getY(), pTargetPlot->getX(), pTargetPlot->getY(), data);

	//  if no path, then bail!
	if (!path)
		return;

	// go through the route to see how long it is and how many plots already have roads
	int iRoadMaintenanceLength = 0;

	for (size_t i = 1; i < path.vPlots.size(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if (!pPlot)
			break;

		if (pPlot->getOwner() != m_pPlayer->GetID())
			return;

		//don't count the cities themselves
		if (pPlot->isCity())
		{
			// strategic routes never need to go through our cities
			if (pPlot->getOwner() == m_pPlayer->GetID())
				return;
			continue;
		}

		if (!GetSameRouteBenefitFromTrait(pPlot, eRoute))
			iRoadMaintenanceLength++;
	}

	//and this to see if we actually build it
	int iCost = pRouteInfo->GetGoldMaintenance() * (100 + m_pPlayer->GetImprovementGoldMaintenanceMod());
	iCost *= iRoadMaintenanceLength + 5; // add extra to account for ring tiles around fort

	int iGoldDelta = iNetGoldTimes100 - iCost;

	if (iGoldDelta < 0)
		return;

	// Need a decent gold buffer to build strategic routes, but don't remove them unless we are actually losing gold
	int iValue = iGoldDelta >= 30 ? 100 : 0;

	for (int i = 1; i < path.length(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if (!pPlot)
			break;

		// remember the plot
		if (AddRoutePlot(pPlot, eRoute, iValue))
			m_strategicRoutePlots.insert(pPlot->GetPlotIndex());
	}

	// Build a ring around the target plot
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(path.get(path.length() - 1));
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];

		if (pAdjacentPlot->isWater())
			continue;

		if (pAdjacentPlot->isCity())
			continue;

		if (pAdjacentPlot->getOwner() != m_pPlayer->GetID())
			continue;

		if (!pAdjacentPlot->isValidMovePlot(m_pPlayer->GetID()))
			continue;

		// Add these routes after the main route is completed
		if (AddRoutePlot(pAdjacentPlot, eRoute, iValue - 1))
			m_strategicRoutePlots.insert(pAdjacentPlot->GetPlotIndex());
	}
}

bool CvBuilderTaskingAI::NeedRouteAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	RoutePlotContainer::const_iterator it = m_routeNeededPlots.find(pPlot->GetPlotIndex());
	return (it != m_routeNeededPlots.end());
}

RouteTypes CvBuilderTaskingAI::GetRouteTypeNeededAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return NO_ROUTE;
	RoutePlotContainer::const_iterator it = m_routeNeededPlots.find(pPlot->GetPlotIndex());
	if (it != m_routeNeededPlots.end())
		return it->second.first;
	return NO_ROUTE;
}

bool CvBuilderTaskingAI::WantCanalAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	//do not check IsWaterAreaSeparator() but wait until there are cities on both sides
	set<int>::const_iterator it = m_canalWantedPlots.find(pPlot->GetPlotIndex());
	return (it != m_canalWantedPlots.end());
}

bool CvBuilderTaskingAI::AddRoutePlot(CvPlot* pPlot, RouteTypes eRoute, int iValue)
{
	if (!pPlot)
		return false;

	if (iValue <= 0 || eRoute == NO_ROUTE)
		return false;

	RouteTypes eOldRoute = NO_ROUTE;
	int iOldValue = 0;

	RoutePlotContainer::const_iterator it;

	it = m_routeNeededPlots.find(pPlot->GetPlotIndex());
	if (it != m_routeNeededPlots.end())
	{
		eOldRoute = it->second.first;
		iOldValue = it->second.second;
	}

	// if we already want a better route, ignore this
	if (eOldRoute > eRoute)
		return false;

	// if we wanted a lower tech route, ignore the old value
	if (eOldRoute < eRoute)
		m_routeNeededPlots[pPlot->GetPlotIndex()] = make_pair(eRoute, iValue);
	else
		m_routeNeededPlots[pPlot->GetPlotIndex()] = make_pair(eRoute, max(iOldValue, iValue));

	//if it is the right route, add to needed plots
	return true;
}

int CvBuilderTaskingAI::GetRouteValue(CvPlot* pPlot)
{
	if (!pPlot)
		return false;

	int iRouteValue = 0;
	RoutePlotContainer::const_iterator it;

	it = m_routeNeededPlots.find(pPlot->GetPlotIndex());
	if (it != m_routeNeededPlots.end())
		iRouteValue = it->second.second;

	return iRouteValue;
}

set<int> CvBuilderTaskingAI::GetMainRoutePlots() const
{
	return m_mainRoutePlots;
}

set<int> CvBuilderTaskingAI::GetShortcutRoutePlots() const
{
	return m_shortcutRoutePlots;
}

set<int> CvBuilderTaskingAI::GetStrategicRoutePlots() const
{
	return m_strategicRoutePlots;
}

bool CvBuilderTaskingAI::GetSameRouteBenefitFromTrait(CvPlot* pPlot, RouteTypes eRoute) const
{
	if (eRoute == ROUTE_ROAD)
	{
		if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (pPlot->getFeatureType() == FEATURE_FOREST || pPlot->getFeatureType() == FEATURE_JUNGLE))
		{
			if (MOD_BALANCE_VP || pPlot->getTeam() == m_pPlayer->getTeam())
				return true;
		}
		else if (m_pPlayer->GetPlayerTraits()->IsRiverTradeRoad() && pPlot->isRiver())
			return true;
	}
	return false;
}

bool CvBuilderTaskingAI::MayWantVillageOnPlot(CvPlot* pPlot) const
{
	FeatureTypes eFeature = pPlot->getFeatureType();

	if (eFeature != NO_FEATURE && m_aiSaveFeatureUntilTech[eFeature] != NO_TECH)
		return false;

	if (m_iSaveCityAdjacentUntilTech != NO_TECH && pPlot->IsAdjacentCity())
		return false;

	return true;
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
		int iCity = 0;
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		for (CvCity* pDestCity = kLoopPlayer.firstCity(&iCity); pDestCity != NULL; pDestCity = kLoopPlayer.nextCity(&iCity))
			vDestPlots.push_back(pDestCity->plot());
	}

	int iOriginCityLoop = 0;
	for (CvCity* pOriginCity = m_pPlayer->firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = m_pPlayer->nextCity(&iOriginCityLoop))
	{
		SPathFinderUserData data(m_pPlayer->GetID(), PT_TRADE_WATER);
		//this is the important flag
		data.iFlags = CvUnit::MOVEFLAG_PRETEND_CANALS;
		//doesn't really matter but we don't want to underestimate the real range, so use the real range
		data.iMaxNormalizedDistance = m_pPlayer->GetTrade()->GetTradeRouteRange(DOMAIN_SEA, pOriginCity);

		//get all paths
		map<int, SPath> waterpaths = GC.GetStepFinder().GetMultiplePaths(pOriginCity->plot(), vDestPlots, data);
		for (map<int, SPath>::iterator it = waterpaths.begin(); it != waterpaths.end(); ++it)
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


void CvBuilderTaskingAI::ConnectCitiesForScenario(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if(pCity1->getOwner() != pCity2->getOwner())
		return;

	// if we already have a connection, bail out
	if (m_pPlayer->IsCityConnectedToCity(pCity1, pCity2, eRoute, true))
		return;

	// build a path between the two cities
	SPathFinderUserData data(m_pPlayer->GetID(),PT_BUILD_ROUTE,eBuild,eRoute,false);
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

vector<BuilderDirective> CvBuilderTaskingAI::GetDirectives()
{
	return m_directives;
}

BuilderDirective CvBuilderTaskingAI::GetAssignedDirective(CvUnit* pUnit)
{
	int iUnitID = pUnit->GetID();

	if (m_assignedDirectives.find(iUnitID) != m_assignedDirectives.end())
	{
		return m_assignedDirectives[iUnitID];
	}

	return BuilderDirective();
}

void CvBuilderTaskingAI::SetAssignedDirective(CvUnit* pUnit, BuilderDirective eDirective)
{
	m_assignedDirectives[pUnit->GetID()] = eDirective;
}

bool CvBuilderTaskingAI::EvaluateBuilder(CvUnit* pUnit, BuilderDirective eDirective)
{
	CvPlot* pTargetPlot = GC.getMap().plot(eDirective.m_sX, eDirective.m_sY);
	if (!pUnit->canBuild(pTargetPlot, eDirective.m_eBuild))
		return false;

	if (!ShouldBuilderConsiderPlot(pUnit, pTargetPlot))
		return false;

	return true;
}

int CvBuilderTaskingAI::GetBuilderNumTurnsAway(CvUnit* pUnit, BuilderDirective eDirective, int iMaxDistance)
{
	if (iMaxDistance < 0)
		return INT_MAX;

	CvPlot* pStartPlot = pUnit->plot();
	CvPlot* pTargetPlot = GC.getMap().plot(eDirective.m_sX, eDirective.m_sY);

	if (pStartPlot == pTargetPlot)
		return 0;

	SPathFinderUserData data(pUnit, CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER, iMaxDistance);

	if (pUnit->IsCombatUnit())
		data.iMaxTurns = min(3, iMaxDistance);

	SPath path = GC.GetPathFinder().GetPath(pUnit->plot(), pTargetPlot, data);

	if (!!path)
		return path.iTotalTurns;
	else
		return INT_MAX;
}


int CvBuilderTaskingAI::GetTurnsToBuild(CvUnit* pUnit, BuilderDirective eDirective, CvPlot* pPlot)
{

	BuildTypes eBuild = eDirective.m_eBuild;
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iBuildLeft = pPlot->getBuildTime(eBuild, ePlayer);
	if (iBuildLeft == 0)
		return 0;

	int iBuildRate = pUnit->workRate(true);

	if (iBuildRate == 0)
	{
		//this means it will take forever under current circumstances
		return INT_MAX;
	}

	iBuildLeft -= pPlot->getBuildProgress(eBuild);
	iBuildLeft = std::max(0, iBuildLeft);

	int iTurnsLeft = (iBuildLeft / iBuildRate);
	//round up
	if (iTurnsLeft * iBuildRate < iBuildLeft)
		iTurnsLeft++;

	return iTurnsLeft;
}

//returns true if sucessful, false otherwise
bool CvBuilderTaskingAI::ExecuteWorkerMove(CvUnit* pUnit, BuilderDirective aDirective)
{
	bool bSuccessful = false;

	if (aDirective.m_eDirectiveType != BuilderDirective::NUM_DIRECTIVES)
	{
		switch (aDirective.m_eDirectiveType)
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
			if (pUnit->getX() == aDirective.m_sX && pUnit->getY() == aDirective.m_sY)
				eMission = CvTypes::getMISSION_BUILD();

			if (GC.getLogging() && GC.GetBuilderAILogging())
			{
				// Open the log file
				CvString strFileName = "BuilderTaskingLog.csv";
				FILogFile* pLog = NULL;
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

				switch (aDirective.m_eDirectiveType)
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
					if (aDirective.m_eDirectiveType == BuilderDirective::REPAIR)
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
					else if (aDirective.m_eDirectiveType == BuilderDirective::BUILD_ROUTE)
					{
						strLog += "Building route,";
					}
					else if (aDirective.m_eDirectiveType == BuilderDirective::BUILD_IMPROVEMENT || aDirective.m_eDirectiveType == BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE)
					{
						strLog += "Building improvement,";
					}
					else if (aDirective.m_eDirectiveType == BuilderDirective::CHOP)
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

			if (eMission == CvTypes::getMISSION_MOVE_TO())
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), aDirective.m_sX, aDirective.m_sY,
					CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED, false, false, MISSIONAI_BUILD, pPlot);

				//do we have movement left?
				if (pUnit->getMoves() > 0)
					eMission = CvTypes::getMISSION_BUILD();
				else
					bSuccessful = true;
			}

			if (eMission == CvTypes::getMISSION_BUILD())
			{
				// check to see if we already have this mission as the unit's head mission
				const MissionData* pkMissionData = pUnit->GetHeadMissionData();
				if (pkMissionData == NULL || pkMissionData->eMissionType != eMission || pkMissionData->iData1 != aDirective.m_eBuild)
					pUnit->PushMission(CvTypes::getMISSION_BUILD(), aDirective.m_eBuild, aDirective.m_eDirectiveType, 0, false, false, MISSIONAI_BUILD, pPlot);

				CvAssertMsg(!pUnit->ReadyToMove(), "Worker did not do their mission this turn. Could cause game to hang.");
				bSuccessful = true;
			}

		}
		break;
		}
	}

	if (bSuccessful)
		SetAssignedDirective(pUnit, aDirective);

	return bSuccessful;
}

/// Looks at city connections and marks plots that can be added as routes by AddRouteDirective
void CvBuilderTaskingAI::UpdateRoutePlots(void)
{
	// updating plots that are part of the road network
	CvCityConnections* pCityConnections = m_pPlayer->GetCityConnections();

	m_routeNeededPlots.clear();
	m_mainRoutePlots.clear();
	m_shortcutRoutePlots.clear();
	m_strategicRoutePlots.clear();

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

	for(int i = GC.getNumBuildInfos() - 1; i >= 0; i--)
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
					ConnectPointsForStrategy(pFirstCity, pSecondPlot, eBuild, eRoute, iNetGoldTimes100);
					continue;
				}

				if (!pFirstCity && pSecondCity)
				{
					ConnectPointsForStrategy(pSecondCity, pFirstPlot, eBuild, eRoute, iNetGoldTimes100);
					continue;
				}

				//we don't do citadel to citadel. or should we?
				if (!pFirstCity && !pSecondCity)
					continue;

				bool bConnectOnlyCapitals = /*1*/ GD_INT_GET(CITY_CONNECTIONS_CONNECT_TO_CAPITAL) > 0;
				if (bConnectOnlyCapitals)
				{
					// only need to build roads to the capital for the money and happiness
					if (!pFirstCity->isCapital() && !pSecondCity->isCapital())
					{
						//if we already have a connection to the capital, it may be possible to have a much shorter route for a direct connection
						//thus improving unit movement and gold bonus from villages
						ConnectCitiesForShortcuts(pFirstCity, pSecondCity, eBuild, eRoute);
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

						ConnectCitiesToCapital(pPlayerCapitalCity, pTargetCity, eBuild, eRoute);
					}
				}
				else
				{
					ConnectCitiesForScenario(pFirstCity, pSecondCity, eBuild, eBestRoute);
				}
			}
		}
	}
}

/// Use the flavor settings to determine what to do
void CvBuilderTaskingAI::UpdateImprovementPlots()
{
	vector<OptionWithScore<BuilderDirective>> aDirectives;

	PlayerTypes ePlayer = m_pPlayer->GetID();

	// Check which builds we can build first
	vector<BuildTypes> aPossibleBuilds;
	for (int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;

		if (!m_pPlayer->canBuild(NULL, eBuild))
			continue;

		int iLoopUnit = 0;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			if (pLoopUnit->canBuild(NULL, eBuild))
			{
				aPossibleBuilds.push_back(eBuild);
				break;
			}
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);

		if (!ShouldAnyBuilderConsiderPlot(pPlot))
			continue;

		int iExistingImprovementValue = 0;

		// Evaluate the improvement currently on the tile
		if (pPlot->getImprovementType() != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
		{
			ImprovementTypes eImprovement = pPlot->getImprovementType();

			int iScore = ScorePlotBuild(pPlot, eImprovement, NO_BUILD);

			// No build time
			iScore += iScore / 2;

			// No build cost

			iScore = min(iScore, 0x7FFF);

			iExistingImprovementValue = iScore;
		}

		//action may depend on city
		CvCity* pWorkingCity = pPlot->getEffectiveOwningCity();

		if (pPlot->getOwner() == ePlayer)
		{
			UpdateCurrentPlotYields(pPlot);

			AddRouteDirective(aDirectives, pPlot);
			AddImprovingResourcesDirective(aDirectives, pPlot, pWorkingCity, aPossibleBuilds, iExistingImprovementValue);
			AddImprovingPlotsDirective(aDirectives, pPlot, pWorkingCity, aPossibleBuilds, iExistingImprovementValue);
			AddChopDirectives(aDirectives, pPlot, pWorkingCity);
			AddScrubFalloutDirectives(aDirectives, pPlot, pWorkingCity);
			AddRepairTilesDirectives(aDirectives, pPlot, pWorkingCity);
			AddRemoveRouteDirective(aDirectives, pPlot);
		}
		else if (!pPlot->isOwned() && pPlot->isAdjacentPlayer(m_pPlayer->GetID()))
		{
			//some special improvements and roads
			AddImprovingPlotsDirective(aDirectives, pPlot, pWorkingCity, aPossibleBuilds, iExistingImprovementValue);
			AddRouteDirective(aDirectives, pPlot);
			// May want to repair and remove road tiles outside of our territory
			AddRepairTilesDirectives(aDirectives, pPlot, pWorkingCity);
			AddRemoveRouteDirective(aDirectives, pPlot);
		}
		else if (NeedRouteAtPlot(pPlot) || pPlot->GetPlayerResponsibleForRoute() == m_pPlayer->GetID())
		{
			//only roads
			AddRouteDirective(aDirectives, pPlot);
			// May want to repair and remove road tiles outside of our territory
			AddRepairTilesDirectives(aDirectives, pPlot, pWorkingCity);
			AddRemoveRouteDirective(aDirectives, pPlot);
		}
	}

	std::stable_sort(aDirectives.begin(), aDirectives.end());
	LogDirectives(aDirectives);

	m_directives.clear();
	m_assignedDirectives.clear();
	for (vector<OptionWithScore<BuilderDirective>>::iterator it = aDirectives.begin(); it != aDirectives.end(); ++it)
		m_directives.push_back((*it).option);
}

/// Evaluating a plot to see if we can build resources there
void CvBuilderTaskingAI::AddImprovingResourcesDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pCity, const vector<BuildTypes> aBuildsToConsider, int iMinValue)
{
	// check to see if a resource is here. If not, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource == NO_RESOURCE)
		return;

	ImprovementTypes eExistingPlotImprovement = pPlot->getImprovementType();
	if (eExistingPlotImprovement != NO_IMPROVEMENT)
	{
		// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
		if (pPlot->HasSpecialImprovement() || (m_pPlayer->isOption(PLAYEROPTION_SAFE_AUTOMATION) && m_pPlayer->isHuman()))
		{
			if (m_bLogging)
			{
				CvString strTemp;
				strTemp.Format(
					"%Weight,Improvement Blocked by Special Improvement,%s,%i,%i",
					GC.getImprovementInfo(pPlot->getImprovementType())->GetType(),
					pPlot->getX(),
					pPlot->getY()
				);
				LogInfo(strTemp, m_pPlayer);
			}
			return;
		}
	}

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	// loop through the build types to find one that we can use
	for(vector<BuildTypes>::const_iterator it = aBuildsToConsider.begin(); it != aBuildsToConsider.end(); ++it)
	{
		BuildTypes eBuild = *it;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild == NULL)
			continue;

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
			continue;

		if (eImprovement == eExistingPlotImprovement)
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
			if (!m_pPlayer->canBuild(pPlot, eBuild))
				continue;

			int iScore = ScorePlotBuild(pPlot, eImprovement, eBuild);

			iScore += GetBuildTimeWeight(iScore, pPlot, eBuild);
			iScore += GetBuildCostWeight(pPlot, eBuild);

			iScore = min(iScore, 0x7FFF);

			if (iScore <= 0)
			{
				continue;
			}

			BuilderDirective directive(BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE, eBuild, eResource, pPlot->getX(), pPlot->getY(), iScore);

			if (m_bLogging)
			{
				CvString strTemp;
				strTemp.Format(
					"Weight,Directive Score Added,%s,%i,%i,%d",
					GC.getBuildInfo(eBuild)->GetType(),
					directive.m_sX,
					directive.m_sY,
					iScore
				);
				LogInfo(strTemp, m_pPlayer);
			}

			if (iScore > iMinValue)
				aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iScore));
		}
	}
}

/// Evaluating a plot to determine what improvement could be best there
void CvBuilderTaskingAI::AddImprovingPlotsDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pCity, const vector<BuildTypes> aBuildsToConsider, int iMinValue)
{
	ImprovementTypes eExistingImprovement = pPlot->getImprovementType();

	if (eExistingImprovement != NO_IMPROVEMENT)
	{
		// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
		if (pPlot->HasSpecialImprovement() || (m_pPlayer->isOption(PLAYEROPTION_SAFE_AUTOMATION) && m_pPlayer->isHuman()))
		{
			if (m_bLogging)
			{
				CvString strTemp;
				strTemp.Format(
					"%Weight,Improvement Blocked by Special Improvement,%s,%i,%i",
					GC.getImprovementInfo(pPlot->getImprovementType())->GetType(),
					pPlot->getX(),
					pPlot->getY()
				);
				LogInfo(strTemp, m_pPlayer);
			}
			return;
		}
	}

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	// loop through the build types to find one that we can use
	for (vector<BuildTypes>::const_iterator it = aBuildsToConsider.begin(); it != aBuildsToConsider.end(); ++it)
	{
		BuildTypes eBuild = *it;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild == NULL)
			continue;

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
			continue;

		if (eImprovement == eExistingImprovement)
		{
			continue;
		}

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (pkImprovementInfo == NULL)
			continue;

		ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
		if (pkImprovementInfo->IsConnectsResource(eResource))
			continue;

		if (!m_pPlayer->canBuild(pPlot, eBuild))
			continue;

#if defined(MOD_BALANCE_CORE)
		//Check for test below.
		if (pkImprovementInfo->IsSpecificCivRequired() && m_pPlayer->getCivilizationType() != pkImprovementInfo->GetRequiredCivilization())
			continue;
#endif

		FeatureTypes eFeature = pPlot->getFeatureType();
		bool bWillRemoveFeature = eFeature != NO_FEATURE && pkBuild->isFeatureRemove(eFeature);

		if (bWillRemoveFeature)
		{
			if (m_aiSaveFeatureUntilTech[eFeature] != NO_TECH && !GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(m_aiSaveFeatureUntilTech[eFeature]))
			{
				if (eResource == NO_RESOURCE)
				{
					if (m_bLogging) {
						CvString strTemp;
						strTemp.Format(
							"Feature Remove Blocked,%s,%s,%i,%i",
							GC.getFeatureInfo(eFeature)->GetType(),
							GC.getBuildInfo(eBuild)->GetType(),
							pPlot->getX(),
							pPlot->getY()
						);
						LogInfo(strTemp, m_pPlayer);
					}
					continue;
				}
			}
		}

		if(m_pPlayer->isOption(PLAYEROPTION_LEAVE_FORESTS) && m_pPlayer->isHuman())
		{
			if(bWillRemoveFeature && eResource == NO_RESOURCE)
			{
				if (m_bLogging) {
					CvString strTemp;
					strTemp.Format(
						"Feature Remove Blocked,%s,%s,%i,%i",
						GC.getFeatureInfo(eFeature)->GetType(),
						GC.getBuildInfo(eBuild)->GetType(),
						pPlot->getX(),
						pPlot->getY()
					);
					LogInfo(strTemp, m_pPlayer);
				}
				continue;
			}
		}

		int iScore = ScorePlotBuild(pPlot, eImprovement, eBuild);

		iScore += GetBuildTimeWeight(iScore, pPlot, eBuild);
		iScore += GetBuildCostWeight(pPlot, eBuild);

		iScore = min(iScore, 0x7FFF);

		// if we're going backward, bail out!
		if(iScore <= 0)
		{
			continue;
		}

		if (bWillRemoveFeature && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
		{
			if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus())
			{
				if (GetRouteTypeNeededAtPlot(pPlot) == ROUTE_ROAD && (MOD_BALANCE_VP || pPlot->getTeam() == m_pPlayer->getTeam()))
					iScore /= 100;
				else
					iScore /= 10;
			}
		}

		BuilderDirective directive(BuilderDirective::BUILD_IMPROVEMENT, eBuild, NO_RESOURCE, pPlot->getX(), pPlot->getY(), iScore);

		if(m_bLogging)
		{
			CvString strTemp;
			strTemp.Format(
				"Weight,Directive Score Added,%s,%i,%i,%d", 
				GC.getBuildInfo(eBuild)->GetType(),
				directive.m_sX,
				directive.m_sY,
				iScore
			);
			LogInfo(strTemp, m_pPlayer);
		}

		if (iScore > iMinValue)
			aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iScore));
	}
}

/// Adds a directive if the unit can construct a road in the plot
void CvBuilderTaskingAI::AddRemoveRouteDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot)
{
	//minors stay out
	if (m_pPlayer->isMinorCiv())
		return;

	//can we even remove routes?
	if (m_eRemoveRouteBuild==NO_BUILD || !m_pPlayer->canBuild(pPlot,m_eRemoveRouteBuild))
		return;

	// If "Automated Workers Don't Replace Improvements" option is enabled, don't remove roads
	if (m_pPlayer->isOption(PLAYEROPTION_SAFE_AUTOMATION) && m_pPlayer->isHuman())
		return;

	// if the player can't build a route, bail out!
	if (m_pPlayer->getBestRoute() == NO_ROUTE)
		return;

	RouteTypes eRoute = pPlot->getRouteType();

	if (eRoute == NO_ROUTE)
		return;

	if (pPlot->isCity())
		return;

	RouteTypes eNeededRoute = GetRouteTypeNeededAtPlot(pPlot);

	// keep routes which are needed
	if (eNeededRoute >= eRoute && !GetSameRouteBenefitFromTrait(pPlot, eNeededRoute))
		return;

	// we don't need to remove pillaged routes
	if (pPlot->IsRoutePillaged())
		return;

	// don't remove routes which we do not pay maintenance for
	PlayerTypes eRouteResponsible = pPlot->GetPlayerResponsibleForRoute();
	if (eRouteResponsible != m_pPlayer->GetID())
		return;

	// don't remove routes that we pay maintenance for if our master built them
	if (eRouteResponsible == m_pPlayer->GetID() && GET_TEAM(m_pPlayer->getTeam()).IsVassal(GET_PLAYER(pPlot->GetPlayerThatBuiltRoute()).getTeam()))
		return;

	//we want to be aggressive with this because of the cost.
	int iWeight = /*500*/ GD_INT_GET(BUILDER_TASKING_BASELINE_BUILD_ROUTES);

	//if we are in debt, be more aggressive
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	if (m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
		iWeight *= 3;

	iWeight += GetBuildTimeWeight(iWeight, pPlot, m_eRemoveRouteBuild);
	iWeight += GetBuildCostWeight(pPlot, m_eRemoveRouteBuild);

	iWeight = min(iWeight, 0x7FFF);

	BuilderDirective directive(BuilderDirective::REMOVE_ROAD, m_eRemoveRouteBuild, NO_RESOURCE, pPlot->getX(), pPlot->getY(), iWeight);

	if (m_bLogging)
	{
		CvString strTemp;
		strTemp.Format(
			"RemoveRouteDirectives,%d,%d,%d",
			pPlot->getX(), 
			pPlot->getY(), 
			iWeight
		);
		LogInfo(strTemp, m_pPlayer);
	}

	aDirectives.push_back( OptionWithScore<BuilderDirective>(directive, iWeight));
}

/// Adds a directive if the unit can construct a road in the plot
void CvBuilderTaskingAI::AddRouteDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot)
{
	// the plot was not flagged recently, so ignore
	RouteTypes eRoute = GetRouteTypeNeededAtPlot(pPlot);
	if(eRoute == NO_ROUTE)
		return;

	if (GetSameRouteBenefitFromTrait(pPlot, eRoute))
		return;

	BuildTypes eRouteBuild = GetBuildRoute(eRoute);

	// can we even build the desired route
	m_pPlayer->canBuild(pPlot, eRouteBuild);
	if(eRouteBuild == NO_BUILD || !m_pPlayer->canBuild(pPlot, eRouteBuild))
		return;

	// no matter if pillaged or not
	if(pPlot->getRouteType() >= eRoute)
		return;

	if(m_pPlayer->isOption(PLAYEROPTION_LEAVE_FORESTS))
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

	int iWeight = GetRouteValue(pPlot);

	if (iWeight <= 0)
		return;

	iWeight += GetBuildTimeWeight(iWeight, pPlot, eRouteBuild);
	iWeight += GetBuildCostWeight(pPlot, eRouteBuild);

	iWeight = min(iWeight, 0x7FFF);

	BuilderDirective directive(BuilderDirective::BUILD_ROUTE, eRouteBuild, NO_RESOURCE, pPlot->getX(), pPlot->getY(), iWeight);

	if(m_bLogging)
	{
		CvString strTemp;
		strTemp.Format(
			"%i,AddRouteDirectives,%d,%d", 
			pPlot->getX(),
			pPlot->getY(),
			iWeight
		);
		LogInfo(strTemp, m_pPlayer);
	}

	aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iWeight));
}

/// Determines if the builder should "chop" the feature in the tile
void CvBuilderTaskingAI::AddChopDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pCity)
{
	// if it's not within a city radius
	if(!pPlot->isWithinTeamCityRadius(m_pPlayer->getTeam()))
	{
		return;
	}

	if(pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		return;
	}

	if(m_pPlayer->isOption(PLAYEROPTION_LEAVE_FORESTS))
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

#if defined(MOD_BALANCE_CORE)
	//Don't cut down city connections!
	if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		if(NeedRouteAtPlot(pPlot))
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
		if(NULL != pkBuild && pkBuild->getImprovement() == NO_IMPROVEMENT && pkBuild->isFeatureRemove(eFeature) && pkBuild->getFeatureProduction(eFeature) > 0 && m_pPlayer->canBuild(pPlot, eBuild))
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
	int iProduction = pPlot->getFeatureProduction(eChopBuild, m_pPlayer->GetID(), &pCity);

	if(!DoesBuildHelpRush(pPlot, eChopBuild))
	{
		return;
	}

	int iWeight = /*1000*/ GD_INT_GET(BUILDER_TASKING_BASELINE_REPAIR);
	iWeight *= iProduction; // times the amount that the plot produces from the chopping

	int iYieldDifferenceWeight = 0;
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		// calculate natural yields
		int iPreviousYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->GetID(), pPlot->getFeatureType(), pPlot->getResourceType(m_pPlayer->getTeam()), NULL);
		int iNewYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->GetID(), NO_FEATURE /*eNewFeature*/, pPlot->getResourceType(m_pPlayer->getTeam()), NULL);
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

	iWeight += GetBuildTimeWeight(iWeight, pPlot, eChopBuild);
	iWeight += GetBuildCostWeight(pPlot, eChopBuild);

	iWeight = min(iWeight, 0x7FFF);

	if(iWeight > 0)
	{
		BuilderDirective directive(BuilderDirective::CHOP, eChopBuild, NO_RESOURCE, pPlot->getX(), pPlot->getY(), iWeight);

		aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iWeight));
	}
}

void CvBuilderTaskingAI::AddRepairTilesDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pWorkingCity)
{
	if (!pPlot)
	{
		return;
	}

	bool bImprovementPillaged = pPlot->getImprovementType() != NO_IMPROVEMENT && pPlot->IsImprovementPillaged();
	bool bRoutePillaged = pPlot->getRouteType() != NO_ROUTE && pPlot->IsRoutePillaged();

	//nothing pillaged here? hmm...
	if (!bImprovementPillaged && !bRoutePillaged)
	{
		return;
	}

	bool bRepairImprovement = false;
	if (bImprovementPillaged)
	{
		bRepairImprovement = true;

		if (pPlot->getOwner() == NO_PLAYER || pPlot->getOwner() != m_pPlayer->GetID())
		{
			bRepairImprovement = false;
		}
		if (pWorkingCity && pWorkingCity->IsRazing())
		{
			bRepairImprovement = false;
		}
	}

	bool bRepairRoute = false;
	if (bRoutePillaged && !bRepairImprovement)
	{
		bRepairRoute = true;

		RouteTypes eRouteNeeded = GetRouteTypeNeededAtPlot(pPlot);
		RouteTypes eRoute = pPlot->getRouteType();

		if (eRouteNeeded == NO_ROUTE)
		{
			// Don't repair roads that we don't need
			bRepairRoute = false;
		}
		else if (eRouteNeeded > eRoute)
		{
			// We want to replace the route with a better one
			bRepairRoute = false;
		}
		else if (GetSameRouteBenefitFromTrait(pPlot, eRoute))
		{
			// We don't need to repair the route since we get the same benefit from our civ trait
			bRepairRoute = false;
		}
	}

	// Nothing we want to repair here
	if (!bRepairImprovement && !bRepairRoute)
		return;

	int iWeight = 0;

	if (bRepairImprovement)
	{
		int iScore = ScorePlotBuild(pPlot, pPlot->getImprovementType(), m_eRepairBuild);

		if (iScore > 0)
		{
			iWeight = iScore;
		}
	}
	if (bRepairRoute)
	{
		int iRepairRouteValue = GetRouteValue(pPlot);

		if (iRepairRouteValue > iWeight)
			iWeight = iRepairRouteValue;
	}

	iWeight += GetBuildTimeWeight(iWeight, pPlot, m_eRepairBuild);
	iWeight += GetBuildCostWeight(pPlot, m_eRepairBuild);

	iWeight = min(iWeight, 0x7FFF);

	if (iWeight > 0)
	{
		BuilderDirective directive(BuilderDirective::REPAIR, m_eRepairBuild, NO_RESOURCE, pPlot->getX(), pPlot->getY(), iWeight);

		aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iWeight));
	}
}
// Everything means less than zero, hey
void CvBuilderTaskingAI::AddScrubFalloutDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pCity)
{
	if(m_eFalloutFeature == NO_FEATURE || m_eFalloutRemove == NO_BUILD)
	{
		return;
	}

	if (pPlot->getTeam() != m_pPlayer->getTeam())
	{
		return;
	}

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	if(pPlot->getFeatureType() == m_eFalloutFeature && m_pPlayer->canBuild(pPlot, m_eFalloutRemove))
	{
		int iWeight =/*20000*/ GD_INT_GET(BUILDER_TASKING_BASELINE_SCRUB_FALLOUT);
		iWeight += GetBuildTimeWeight(iWeight, pPlot, m_eFalloutRemove);
		iWeight += GetBuildCostWeight(pPlot, m_eFalloutRemove);

		BuilderDirective directive(BuilderDirective::CHOP, m_eFalloutRemove, NO_RESOURCE, pPlot->getX(), pPlot->getY(), iWeight);
		aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iWeight));
	}
}

/// Evaluates all the circumstances to determine if any builder should evaluate the given plot
bool CvBuilderTaskingAI::ShouldAnyBuilderConsiderPlot(CvPlot* pPlot)
{
	if (!pPlot)
		return false;

	if (pPlot->isCity())
		return false;

	if (!pPlot->isRevealed(m_pPlayer->getTeam()))
		return false;

	//can't build in other major player's territory (unless they are our vassal)
	if (pPlot->isOwned() && pPlot->getOwner() != m_pPlayer->GetID() && GET_PLAYER(pPlot->getOwner()).isMajorCiv() && !GET_TEAM(pPlot->getTeam()).IsVassal(m_pPlayer->getTeam()))
		return false;

	//check if we could be captured
	vector<CvUnit*> vAttackers = m_pPlayer->GetPossibleAttackers(*pPlot, m_pPlayer->getTeam());
	bool bMayStay = vAttackers.empty() || (vAttackers.size() == 1 && pPlot->getBestDefender(m_pPlayer->GetID()) != NULL);
	if (!bMayStay)
	{
		if (GC.getLogging() && GC.getAILogging() && m_bLogging)
		{
			CvString strLog;
			strLog.Format(
				"Bailing due to potential attackers,%d,%d,%d",
				pPlot->getX(),
				pPlot->getY(),
				m_pPlayer->GetPlotDanger(*pPlot, true)
			);
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLog);
		}
		return false;
	}

	return true;
}

/// Evaluates all the circumstances to determine if this builder can and should evaluate the given plot
bool CvBuilderTaskingAI::ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot)
{
	if (!pUnit)
		return false;

	//don't consider non-workable plots for GPs!
	if (pUnit->IsGreatPerson())
	{
		CvCity* pClosestCity = m_pPlayer->GetClosestCityByPlots(pPlot);
		if (pClosestCity && pClosestCity->getWorkPlotDistance() < m_pPlayer->GetCityDistanceInPlots(pPlot))
			return false;
	}

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

	//if there is fallout, try to scrub it in spite of the danger
	if(pPlot->getFeatureType() == FEATURE_FALLOUT && !pUnit->ignoreFeatureDamage() && (pUnit->GetCurrHitPoints() < (pUnit->GetMaxHitPoints() / 2)))
	{
		if(GC.getLogging() && GC.getAILogging() && m_bLogging)
		{
			CvString strLog;
			strLog.Format(
				"%i,Bailing due to fallout,%d,%d,%d", 
				pUnit->GetID(),
				pPlot->getX(),
				pPlot->getY(),
				m_pPlayer->GetPlotDanger(*pPlot, true)
			);
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLog);
		}
		return false;
	}

	if (!pUnit->canEndTurnAtPlot(pPlot))
	{
		if(m_bLogging)
		{
			CvString strLog;
			strLog.Format(
				"%i,This tile is full with another unit - bailing,%d,%d", 
				pUnit->GetID(),
				pPlot->getX(),
				pPlot->getY()
			);
			LogInfo(strLog, m_pPlayer, true);
		}

		return false;
	}

	return true;
}

/// Get the weight determined by the cost of building the item
int CvBuilderTaskingAI::GetBuildCostWeight(CvPlot* pPlot, BuildTypes eBuild)
{
	return -m_pPlayer->getBuildCost(pPlot, eBuild) / 10;
}

/// Get the weight determined by the building time of the item
int CvBuilderTaskingAI::GetBuildTimeWeight(int iWeight, CvPlot* pPlot, BuildTypes eBuild)
{
	int iBuildTimeNormal = pPlot->getBuildTime(eBuild, m_pPlayer->GetID());
	int iBuildProgress= pPlot->getBuildProgress(eBuild);
	int iBuildTurnsLeft = iBuildTimeNormal - iBuildProgress;
	int iBuildTime = min(iBuildTimeNormal, iBuildTurnsLeft);

	int iScaledBuildTime = iBuildTime / 300;

	if (2 * iScaledBuildTime <= iWeight && iScaledBuildTime > 0)
		return iWeight / (2 * iScaledBuildTime);
	else if (iScaledBuildTime <= 0)
		return iWeight / 2;
	else
		return 0;
}

/// Return the weight of this resource
int CvBuilderTaskingAI::GetResourceWeight(ResourceTypes eResource, ImprovementTypes eImprovement, int iQuantity, int iAdditionalOwned)
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
			iWeight += iResult * 10;
		}

		int iImprovementFlavor = eImprovement != NO_IMPROVEMENT ? iImprovementFlavor = GC.getImprovementInfo(eImprovement)->GetFlavorValue(i) : 0;

		int iUsableByCityWeight = iPersonalityFlavorValue * iImprovementFlavor;
		if (iUsableByCityWeight > 0)
		{
			iWeight += iUsableByCityWeight * 10;
		}
	}

	// if the empire is unhappy (or close to it) and this is a luxury resource the player doesn't have, provide a super bonus to getting it
	if (pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
	{
		int iModifier = (pkResource->getHappiness() * /*750*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE));

		int iNumResourceAvailable = m_pPlayer->getNumResourceAvailable(eResource) + iAdditionalOwned;

		// We have plenty to spare
		if (iNumResourceAvailable > 1)
			iModifier /= 10;
		// We have one already
		else if (iNumResourceAvailable > 0)
			iModifier /= 2;

		iWeight += iModifier;
	}
	else if (pkResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
	{
		//if (m_pPlayer->IsResourceCityTradeable(eResource))
		//{
		// measure quantity
		int iModifier = iQuantity * 100;

		int iNumResourceAvailable = m_pPlayer->getNumResourceAvailable(eResource) + iAdditionalOwned;

		// We have plenty to spare
		if (iNumResourceAvailable > 5)
			iModifier /= 10;
		// We have some already
		else if (iNumResourceAvailable > 0)
			iModifier /= 2;

		iWeight += iModifier;
		//}
	}

	return iWeight;
}

/// Does this city want to rush a unit?
bool CvBuilderTaskingAI::DoesBuildHelpRush(CvPlot* pPlot, BuildTypes eBuild)
{
	CvCity* pCity = NULL;
	int iProduction = pPlot->getFeatureProduction(eBuild, m_pPlayer->GetID(), &pCity);
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
	if (pPlot->getResourceType(m_pPlayer->getTeam()) != NO_RESOURCE)
		return true;

	if(!(pCity->getOrderFromQueue(0)->bRush))
	{
		// this order should not be rushed
		return false;
	}

	return false;
}

int GetYieldBaseModifier(YieldTypes eYield)
{
	switch (eYield)
	{
	case NO_YIELD:
		UNREACHABLE();
	case YIELD_FOOD:
		return /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FOOD);
	case YIELD_PRODUCTION:
		return /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_PRODUCTION);
	case YIELD_GOLD:
		return /*40*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_GOLD);
	case YIELD_SCIENCE:
		return /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_SCIENCE);
	case YIELD_CULTURE:
		return /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_CULTURE);
	case YIELD_FAITH:
		return /*150*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FAITH);
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
		return 50;
	default:
		return 0;
	}
}

int CvBuilderTaskingAI::ScorePlotBuild(CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild, SBuilderState sState)
{
	const CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);

	if (eImprovement == NO_IMPROVEMENT && pPlot->IsImprovementPillaged() && pkBuildInfo->isRepair())
		eImprovement = pPlot->getImprovementType();

	const CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);

	const ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	const FeatureTypes eFeature = pPlot->getFeatureType();

	const ImprovementTypes eOldImprovement = pPlot->getImprovementType();
	const CvImprovementEntry* pkOldImprovementInfo = GC.getImprovementInfo(eOldImprovement);
	const ResourceTypes eResourceFromOldImprovement = pkOldImprovementInfo ? (ResourceTypes)pkOldImprovementInfo->GetResourceFromImprovement() : NO_RESOURCE;
	const FeatureTypes eFeatureFromOldImprovement = pkOldImprovementInfo ? pkOldImprovementInfo->GetCreatedFeature() : NO_FEATURE;

	const int iImprovementMaintenance = pkImprovementInfo ? pkImprovementInfo->GetGoldMaintenance() : 0;

	//Some base value.
	int iYieldScore = 0;
	int iBaseYieldScore = 0;
	int iSecondaryScore = 0;

	const int iBigBuff = 1000;
	const int iSmallBuff = 350;

	const CvCity* pOwningCity = pPlot->getEffectiveOwningCity();

	// If we are performing a culture bomb, find which city will be owning the plot
	bool bIsCultureBomb = pkImprovementInfo ? pkImprovementInfo->GetCultureBombRadius() > 0 : false;
	if (bIsCultureBomb && !pOwningCity)
	{
		int iBestCityDistance = INT_MAX;
		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			CvPlot* pPlot = pLoopCity->plot();
			if (pPlot)
			{
				int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

				if (iDistance < iBestCityDistance)
				{
					// If we can't work the tile, we don't care
					if (pLoopCity->IsWithinWorkRange(pPlot))
						pOwningCity = pLoopCity;

					iBestCityDistance = iDistance;
				}
			}
		}
	}

	const CvCityStrategyAI* pCityStrategy = pOwningCity ? pOwningCity->GetCityStrategyAI() : NULL;
	const CvReligion* pCityReligion = pOwningCity ? pOwningCity->GetCityReligions()->GetMajorityReligion() : NULL;
	
	// Do we have or will we build a road here?
	bool bWillBeCityConnectingRoad = true;
	const bool bIsWithinWorkRange = pOwningCity && pOwningCity->IsWithinWorkRange(pPlot);
	bool bCityIsSated = true;

	const ResourceTypes eResourceFromImprovement = pkImprovementInfo ? (ResourceTypes)pkImprovementInfo->GetResourceFromImprovement() : NO_RESOURCE;
	const FeatureTypes eFeatureFromImprovement = pkImprovementInfo ? pkImprovementInfo->GetCreatedFeature() : NO_FEATURE;

	bool bChangedFeatureState = (eFeatureFromImprovement != NO_FEATURE && eFeatureFromImprovement != eFeature)
		                     || (eImprovement != NO_IMPROVEMENT && eImprovement != eOldImprovement && eFeatureFromOldImprovement != NO_FEATURE && eFeatureFromImprovement != eFeatureFromOldImprovement)
		                     || (pkBuildInfo && pkBuildInfo->isFeatureRemove(eFeature));

	const FeatureTypes eNaturalFeature = eFeatureFromOldImprovement == NO_FEATURE ? eFeature : NO_FEATURE;
	const ResourceTypes eNaturalResource = eResourceFromOldImprovement == NO_RESOURCE ? eResource : NO_RESOURCE;

	int iExtraResource = 0;
	if ((pkImprovementInfo && pkImprovementInfo->IsConnectsResource(eResource)) || eResourceFromImprovement != NO_RESOURCE)
	{
		if (eResourceFromImprovement != NO_RESOURCE)
			iExtraResource = sState.mExtraResources[eResourceFromImprovement];
		else
			iExtraResource = sState.mExtraResources[eResource];
	}

	if (pOwningCity)
	{
		// A city is considered "sated" if all the plots they are working have an improvement on them
		int iWorkedUnimproved = 0;
		int iImprovementsPlanned = 0;

		std::vector<int> aWorkedPlots = pOwningCity->GetCityCitizens()->GetWorkedPlots();

		for (std::vector<int>::iterator it = aWorkedPlots.begin(); it != aWorkedPlots.end(); ++it)
		{
			CvPlot* pWorkedPlot = GC.getMap().plotByIndex(*it);

			if (!pWorkedPlot)
				continue;

			if (pWorkedPlot->isImpassable(m_pPlayer->getTeam()))
				continue;

			if (pWorkedPlot->IsNaturalWonder())
				continue;

			if (pWorkedPlot->isWater())
				continue;

			if (pWorkedPlot->getImprovementType() == NO_IMPROVEMENT || pWorkedPlot->IsImprovementPillaged())
				iWorkedUnimproved++;
		}

		if (iWorkedUnimproved > 0)
		{
			std::vector<int> aOwnedPlots = m_pPlayer->GetPlots();

			for (std::vector<int>::iterator it = aOwnedPlots.begin(); it != aOwnedPlots.end(); ++it)
			{
				CvPlot* pOwnedPlot = GC.getMap().plotByIndex(*it);

				if (pOwnedPlot->getEffectiveOwningCity() == pOwningCity && sState.mChangedPlotImprovements.find(pOwnedPlot->GetPlotIndex()) == sState.mChangedPlotImprovements.end())
					iImprovementsPlanned++;
			}
		}

		if (iWorkedUnimproved > iImprovementsPlanned)
			bCityIsSated = false;
	}

	if (bIsWithinWorkRange)
	{
		RouteTypes eRouteNeeded = GetRouteTypeNeededAtPlot(pPlot);
		bWillBeCityConnectingRoad = eRouteNeeded != NO_ROUTE; // TODO don't count strategic routes
		if (eRouteNeeded == ROUTE_ROAD)
		{
			int iPlotIndex = pPlot->GetPlotIndex();
			bool bCityConnectingRoad = m_mainRoutePlots.find(iPlotIndex) != m_mainRoutePlots.end() || m_shortcutRoutePlots.find(iPlotIndex) != m_shortcutRoutePlots.end();
			if (!bCityConnectingRoad)
			{
				bWillBeCityConnectingRoad = false;
			}
			else if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
			{
				if (MOD_BALANCE_VP || pPlot->getTeam() == m_pPlayer->getTeam())
				{
					bWillBeCityConnectingRoad = false;
				}
			}
		}

		RouteTypes eExpectedRoute = !pPlot->IsRoutePillaged() ? pPlot->getRouteType() : NO_ROUTE;

		if (pPlot->getRouteType() != NO_ROUTE && pPlot->IsRoutePillaged() && pkBuildInfo && pkBuildInfo->isRepair())
		{
			// Repairing a pillaged route
			eExpectedRoute = pPlot->getRouteType();
		}
		else if (pkBuildInfo && pkBuildInfo->getRoute() != NO_ROUTE)
		{
			// Building a route
			eExpectedRoute = (RouteTypes)pkBuildInfo->getRoute();
		}
		else
		{
			// If we are building an improvement, evaluate as if road network is completed
			if (eExpectedRoute == NO_ROUTE && bWillBeCityConnectingRoad)
				eExpectedRoute = eRouteNeeded;
			if (!bWillBeCityConnectingRoad)
				eExpectedRoute = NO_ROUTE;
		}

		if (eBuild != NO_BUILD)
			UpdateProjectedPlotYields(pPlot, eBuild, bWillBeCityConnectingRoad);
		else
			UpdateCurrentPlotYields(pPlot);
	}

	for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		YieldTypes eYield = (YieldTypes)ui;

		// Simplification - errata yields not worth considering.
		if (eYield > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		int iBasePlotYield = bIsWithinWorkRange ? pPlot->calculateNatureYield(eYield, m_pPlayer->GetID(), eNaturalFeature, eNaturalResource, pOwningCity) : 0;

		int iYieldBaseModifier = GetYieldBaseModifier(eYield);

		int iOldYieldTimes100 = bIsWithinWorkRange ? 100 * m_aiCurrentPlotYields[ui] : 0;
		int iNewYieldTimes100 = iOldYieldTimes100;

		if (bIsWithinWorkRange)
		{
			iNewYieldTimes100 = eBuild != NO_BUILD ? 100 * m_aiProjectedPlotYields[ui] : iNewYieldTimes100;

			// Assume we love the king day is active 85% of the time
			if (pkImprovementInfo)
			{
				if (pOwningCity->GetWeLoveTheKingDayCounter() == 0)
					iNewYieldTimes100 += 85 * pkImprovementInfo->GetWLTKDYieldChange(eYield);
				else
					iNewYieldTimes100 -= 15 * pkImprovementInfo->GetWLTKDYieldChange(eYield);
			}

			if (bChangedFeatureState)
			{
				// If we are creating a feature, check if city gets any extra yield gains
				if (eFeatureFromImprovement != NO_FEATURE)
				{
					// Give double value to this yield because we don't need to work the tile
					iNewYieldTimes100 += pOwningCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeatureFromImprovement, eYield) * 2;

					if (pCityReligion)
						iNewYieldTimes100 += pCityReligion->m_Beliefs.GetYieldPerXFeatureTimes100(eFeatureFromImprovement, eYield) * 2;
				}

				// If we are removing a feature, check if city gets any extra yield losses
				if (eFeature != NO_FEATURE)
				{
					// Give double value to this yield because we don't need to work the tile
					iNewYieldTimes100 -= pOwningCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield) * 2;

					if (pCityReligion)
						iNewYieldTimes100 -= pCityReligion->m_Beliefs.GetYieldPerXFeatureTimes100(eFeature, eYield) * 2;
				}
			}
		}

		// Special handling for vanilla celts
		if (!MOD_BALANCE_VP && eYield == YIELD_FAITH && m_pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest())
		{
			if (bChangedFeatureState && eFeature == FEATURE_FOREST && eOldImprovement == NO_IMPROVEMENT)
			{
				CvCity* pNextCity = pPlot->GetAdjacentCity();
				if (pNextCity && pNextCity->getOwner() == m_pPlayer->GetID())
				{
					int iAdjacentForests = 0;

					for (int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pCityAdjacentPlot = plotDirection(pNextCity->getX(), pNextCity->getY(), ((DirectionTypes)iDirectionLoop));
						int iAdjacentPlotIndex = pCityAdjacentPlot->GetPlotIndex();
						FeatureTypes eAdjacentFeature = sState.mChangedPlotFeatures.find(iAdjacentPlotIndex) != sState.mChangedPlotFeatures.end() ? sState.mChangedPlotFeatures[iAdjacentPlotIndex] : pCityAdjacentPlot->getFeatureType();
						if (pCityAdjacentPlot && eAdjacentFeature == eFeature && pCityAdjacentPlot->getImprovementType() == NO_IMPROVEMENT)
						{
							iAdjacentForests++;
						}
					}

					if (iAdjacentForests == 3 || (iAdjacentForests == 2 && MOD_ALTERNATE_CELTS) || iAdjacentForests == 1)
					{
						// Give double value to this yield because we don't need to work the tile
						iNewYieldTimes100 -= 200;
					}
				}
			}
		}

		if (eYield == YIELD_GOLD && iImprovementMaintenance != 0)
		{
			iNewYieldTimes100 -= 100 * iImprovementMaintenance;
		}

		// Bonuses yield that this improvement provides to adjacent improvements
		if (pOwningCity || (pkImprovementInfo && pkImprovementInfo->IsInAdjacentFriendly()))
		{
			CvPlot** pAdjacentPlots = GC.getMap().getNeighborsUnchecked(pPlot->GetPlotIndex());
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = pAdjacentPlots[iI];

				if (!pAdjacentPlot)
					continue;

				if (pAdjacentPlot->getOwner() != m_pPlayer->GetID())
					continue;

				int iAdjacentPlotIndex = pAdjacentPlot->GetPlotIndex();
				ImprovementTypes eAdjacentImprovement = sState.mChangedPlotImprovements.find(iAdjacentPlotIndex) != sState.mChangedPlotImprovements.end() ? sState.mChangedPlotImprovements[iAdjacentPlotIndex] : NO_IMPROVEMENT;

				if (eAdjacentImprovement == NO_IMPROVEMENT && !pAdjacentPlot->IsImprovementPillaged())
					eAdjacentImprovement = pAdjacentPlot->getImprovementType();

				if (eAdjacentImprovement == NO_IMPROVEMENT)
					continue;

				CvImprovementEntry* pkAdjacentImprovement = GC.getImprovementInfo(eAdjacentImprovement);
				if (!pkAdjacentImprovement)
					continue;

				CvCity* eAdjacentOwningCity = pAdjacentPlot->getEffectiveOwningCity();
				if (!eAdjacentOwningCity)
					continue;

				if (!eAdjacentOwningCity->IsWithinWorkRange(pAdjacentPlot))
					continue;

				if (eAdjacentOwningCity->IsRazing())
					continue;

				CvCityStrategyAI* eAdjacentCityStrategy = eAdjacentOwningCity->GetCityStrategyAI();
				if (!eAdjacentCityStrategy)
					continue;

				// Adjacency bonuses are applied to the city the other plot belongs to
				int iYieldMultiplier = eAdjacentCityStrategy->GetYieldModifierTimes100(eYield);
				int iYieldModifier = (iYieldBaseModifier * iYieldMultiplier) / 100;

				if (eAdjacentImprovement == eImprovement)
				{
					iYieldScore += pkAdjacentImprovement->GetYieldAdjacentSameType(eYield) * iYieldModifier;
					// Add half a yield if there's one adjacent when two are needed
					iYieldScore += (pkAdjacentImprovement->GetYieldAdjacentTwoSameType(eYield) * iYieldModifier) / 2;
				}
				else if (eOldImprovement != NO_IMPROVEMENT && eAdjacentImprovement == eOldImprovement)
				{
					iYieldScore -= pkAdjacentImprovement->GetYieldAdjacentSameType(eYield) * iYieldModifier;
					iYieldScore -= (pkAdjacentImprovement->GetYieldAdjacentTwoSameType(eYield) * iYieldModifier) / 2;
				}

				if (eImprovement != NO_IMPROVEMENT)
					iYieldScore += pkAdjacentImprovement->GetAdjacentImprovementYieldChanges(eImprovement, eYield) * iYieldModifier;
				if (eResourceFromImprovement != NO_RESOURCE)
					iYieldScore += pkAdjacentImprovement->GetAdjacentResourceYieldChanges(eResourceFromImprovement, eYield) * iYieldModifier;
				if (eFeatureFromImprovement != NO_FEATURE)
					iYieldScore += pkAdjacentImprovement->GetAdjacentFeatureYieldChanges(eFeatureFromImprovement, eYield) * iYieldModifier;

				if (eOldImprovement != NO_IMPROVEMENT)
				{
					iYieldScore -= pkAdjacentImprovement->GetAdjacentImprovementYieldChanges(eOldImprovement, eYield) * iYieldModifier;
					if (eResourceFromOldImprovement != NO_RESOURCE)
						iYieldScore -= pkAdjacentImprovement->GetAdjacentResourceYieldChanges(eResourceFromOldImprovement, eYield) * iYieldModifier;
					if (eFeatureFromOldImprovement != NO_FEATURE)
						iYieldScore -= pkAdjacentImprovement->GetAdjacentFeatureYieldChanges(eFeatureFromOldImprovement, eYield) * iYieldModifier;
				}
			}
		}

		int iYieldMultiplier = pCityStrategy ? pCityStrategy->GetYieldModifierTimes100(eYield) : 100;
		int iYieldModifier = (iYieldBaseModifier * iYieldMultiplier) / 100;

		if (bCityIsSated)
		{
			iYieldModifier *= 2;
			iYieldModifier /= 3;
		}

		iYieldScore += ((iNewYieldTimes100 - iOldYieldTimes100) * iYieldModifier) / 100;
		iBaseYieldScore += iBasePlotYield * iYieldModifier;
	}

	//Improvement grants or connects resource? Let's weight this based on flavors.
	if (pOwningCity && (eResourceFromImprovement != NO_RESOURCE || (eResource != NO_RESOURCE && pkImprovementInfo && pkImprovementInfo->IsConnectsResource(eResource) && !pkImprovementInfo->IsCreatedByGreatPerson())))
	{
		ResourceTypes eConnectedResource = eResourceFromImprovement != NO_RESOURCE ? eResourceFromImprovement : eResource;
		int iResourceWeight = GetResourceWeight(eConnectedResource, eImprovement, pkImprovementInfo->GetResourceQuantityFromImprovement(), iExtraResource);
		iSecondaryScore += iResourceWeight;

		CvResourceInfo* pkConnectedResource = GC.getResourceInfo(eConnectedResource);
		//amp up monopoly alloc!
		if (pkConnectedResource && pkConnectedResource->isMonopoly())
		{
			if (pkConnectedResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				if (m_pPlayer->GetMonopolyPercent(eConnectedResource) > 0 && m_pPlayer->GetMonopolyPercent(eConnectedResource) <= /*50*/ GD_INT_GET(GLOBAL_RESOURCE_MONOPOLY_THRESHOLD))
					iSecondaryScore += (iResourceWeight * m_pPlayer->GetMonopolyPercent(eConnectedResource)) / 25;
			}
			else if (pkConnectedResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
			{
				if (m_pPlayer->GetMonopolyPercent(eConnectedResource) > 0 && m_pPlayer->GetMonopolyPercent(eConnectedResource) <= /*50*/ GD_INT_GET(GLOBAL_RESOURCE_MONOPOLY_THRESHOLD))
					iSecondaryScore += (iResourceWeight * m_pPlayer->GetMonopolyPercent(eConnectedResource)) / 25;
			}
		}
	}

	// Avoid building improvements in certain spots
	if (pOwningCity && pkImprovementInfo && bIsWithinWorkRange)
	{
		bool bSaveCityAdjacent = m_iSaveCityAdjacentUntilTech != NO_TECH && pPlot->IsAdjacentCity();
		bool bBenefitsFromRoads = false;
		for (int iI = 0; iI <= YIELD_FAITH; iI++)
		{
			if (pkImprovementInfo->GetRouteYieldChanges(ROUTE_ROAD, iI) > 0 || pkImprovementInfo->GetRouteYieldChanges(ROUTE_RAILROAD, iI) > 0)
			{
				bBenefitsFromRoads = true;
				break;
			}
		}

		if (pkImprovementInfo->IsCreatedByGreatPerson())
		{
			// Great person improvements special considerations
			bool bGreatPersonAvoidLarge = false;
			if (bSaveCityAdjacent)
				bGreatPersonAvoidLarge = true;
			else if (eFeature != NO_FEATURE && m_aiSaveFeatureUntilTech[eFeature] != NO_TECH)
				bGreatPersonAvoidLarge = true;
			else if (pPlot->getResourceType(m_pPlayer->getTeam()) != NO_RESOURCE)
				bGreatPersonAvoidLarge = true;

			bool bGreatPersonAvoidSmall = false;
			if (eFeature != NO_FEATURE)
				bGreatPersonAvoidSmall = true;
			else if (pPlot->isRiver())
				bGreatPersonAvoidSmall = true;

			if (MOD_BALANCE_VP)
			{
				if (bWillBeCityConnectingRoad && !bBenefitsFromRoads)
					bGreatPersonAvoidSmall = true;
				else if (!bWillBeCityConnectingRoad && bBenefitsFromRoads)
					bGreatPersonAvoidSmall = true;
			}

			if (bGreatPersonAvoidLarge)
				iSecondaryScore -= iBigBuff;
			else if (bGreatPersonAvoidSmall)
				iSecondaryScore -= iSmallBuff;
		}
		else
		{
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			// Non-great person improvements considerations
			if ((eResource == NO_RESOURCE || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS) && bSaveCityAdjacent && !pkImprovementInfo->IsAdjacentCity())
				iSecondaryScore -= iSmallBuff;
			else if (eResource != NO_RESOURCE && pkImprovementInfo && !pkImprovementInfo->IsConnectsResource(eResource))
			{
				if (pkResourceInfo && pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_BONUS)
					iSecondaryScore -= iSmallBuff;
			}

			if (MOD_BALANCE_VP)
			{
				if (bWillBeCityConnectingRoad && !bBenefitsFromRoads)
					iSecondaryScore -= iSmallBuff;
				else if (!bWillBeCityConnectingRoad && bBenefitsFromRoads)
					iSecondaryScore -= iSmallBuff;
			}
		}
	}

#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
	// improvement spawns resource?
	if (pOwningCity)
	{
		int iResourceChance = pkImprovementInfo ? pkImprovementInfo->GetRandomResourceChance() : 0;
		if (iResourceChance > 0 && pPlot->getResourceType(m_pPlayer->getTeam()) == NO_RESOURCE)
		{
			iSecondaryScore += 5 * iResourceChance;
		}

		// if it's Iroquois building forests give it even more weight since it connects cities.
		if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeatureFromImprovement == FEATURE_FOREST || eFeatureFromImprovement == FEATURE_JUNGLE))
		{
			iSecondaryScore += iSmallBuff;
		}
	}
#endif

	//feature considerations...
	/*if (pOwningCity)
	{
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if (pkBuild)
		{
			//we remove this feature?
			if (pkBuild->isFeatureRemove(eFeature))
			{
				//how many do we have left?
				//we don't want to remove all features, we might need them later!
				int iNumFeatureRemaining = pOwningCity->CountFeature(eFeature);
				if (iNumFeatureRemaining <= 10)
				{
					iSecondaryScore -= (10 - iNumFeatureRemaining) * 100;
				}
			}
			else
			{
				//bump to encourage these in cities with lots of features
				//how many do we have left?
				if (pOwningCity)
				{
					int iNumFeatureRemaining = pOwningCity->CountFeature(eFeature);
					iSecondaryScore += iNumFeatureRemaining * 50;
				}
			}
		}
	}*/

	//Is this a good spot for a defensive building?
	bool bNewIsDefensive = pkImprovementInfo && (pkImprovementInfo->GetDefenseModifier() > 0 || pkImprovementInfo->GetNearbyEnemyDamage() > 0);
	bool bOldIsDefensive = pkOldImprovementInfo && (pkOldImprovementInfo->GetDefenseModifier() > 0 || pkOldImprovementInfo->GetNearbyEnemyDamage() > 0);
	if ((pOwningCity || (bIsCultureBomb && pPlot->isAdjacentPlayer(m_pPlayer->GetID()))) && (bNewIsDefensive || bOldIsDefensive))
	{
		int iDefenseBuildValue = pPlot->GetDefenseBuildValue(m_pPlayer->GetID(), eBuild, eImprovement, sState);
		iSecondaryScore += iDefenseBuildValue;
	}

	// Do we want a canal here?
	if ((pOwningCity || (bIsCultureBomb && pPlot->isAdjacentPlayer(m_pPlayer->GetID()))) && MOD_GLOBAL_PASSABLE_FORTS && pkImprovementInfo && pkImprovementInfo->IsMakesPassable() && WantCanalAtPlot(pPlot) && pkImprovementInfo->GetNearbyEnemyDamage() == 0)
		iSecondaryScore += iBigBuff * 3;

	// TODO flesh out culture bomb logic and move it from TacticalAI?
	// Currently this is only for human player recommendations.
	if (bIsCultureBomb && eBuild != NO_BUILD && (pOwningCity || pPlot->isAdjacentPlayer(m_pPlayer->GetID())))
	{
		int iAdjacentGood = 0;
		for (int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));

			if (pAdjacentPlot != NULL && pAdjacentPlot->getOwner() != m_pPlayer->GetID())
			{
				iAdjacentGood++;
			}
		}
		if (iAdjacentGood > 0)
		{
			iSecondaryScore += 1000 * iAdjacentGood;
		}
	}

	if (iYieldScore + iSecondaryScore <= 0)
		return iYieldScore + iSecondaryScore;
	return iYieldScore + iBaseYieldScore + iSecondaryScore;
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

BuildTypes CvBuilderTaskingAI::GetBuildRoute(RouteTypes eRoute)
{
	// find the route build
	BuildTypes eRouteBuild = NO_BUILD;

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
	FILogFile* pLog = NULL;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

	CvString strLog;
	CvString strTemp;

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
	FILogFile* pLog = NULL;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

	CvString strLog;
	CvString strTemp;

	CvString strPlayerName;
	strPlayerName = pPlayer->getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
	strLog += strTemp;
	strLog += strNewLogStr;
	pLog->Msg(strLog);
}

/// Logs all the directives for the unit
void CvBuilderTaskingAI::LogDirectives(vector<OptionWithScore<BuilderDirective>> directives)
{
	if(!m_bLogging)
		return;

	if(directives.empty())
	{
		CvString strLog;
		CvString strTemp;

		strLog += "No directives!";
		LogInfo(strLog, m_pPlayer);
	}
	else
	{
		for(size_t i = 0; i < directives.size(); i++)
			LogDirective(directives[i].option, directives[i].score);
	}
}

void CvBuilderTaskingAI::LogDirective(BuilderDirective directive, int iWeight, bool bChosen)
{
	if(!m_bLogging)
		return;

	if (GC.getBuildInfo(directive.m_eBuild) == NULL)
		return;

	CvString strLog;

	strLog += "Evaluating,";

	switch(directive.m_eDirectiveType)
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
		strLog += "CHOP,";
		break;
	case BuilderDirective::REMOVE_ROAD:
		strLog += "REMOVE_ROAD,";
		break;
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

	if(directive.m_eDirectiveType == BuilderDirective::REPAIR)
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

	CvString strTemp;
	strTemp.Format("%d,%d,", directive.m_sX, directive.m_sY);
	strLog += strTemp;

	strTemp.Format("%d,", iWeight);
	strLog += strTemp;

	if(bChosen)
	{
		strLog += (", Chosen!");
	}

	LogInfo(strLog, m_pPlayer, m_pPlayer->isHuman());
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
void CvBuilderTaskingAI::UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild, bool bIsCityConnection)
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
			if ((YieldTypes)ui <= YIELD_GOLDEN_AGE_POINTS || MOD_BALANCE_CORE_JFD)
			{
#endif
				m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, bIsCityConnection, m_pPlayer->GetID(), pOwningCity, pReligion, pBelief);
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
								m_aiProjectedPlotYields[ui] += pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, bIsCityConnection, m_pPlayer->GetID(), pOwningCity, pPantheon, NULL);
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
			if ((YieldTypes)ui <= YIELD_GOLDEN_AGE_POINTS || MOD_BALANCE_CORE_JFD)
			{
#endif
				m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, bIsCityConnection, m_pPlayer->GetID(), NULL, NULL, NULL);
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