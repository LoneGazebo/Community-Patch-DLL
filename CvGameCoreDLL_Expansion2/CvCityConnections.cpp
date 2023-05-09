/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvCityConnections.h"
#include "CvPlayer.h"
#include "CvAStar.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "FStlContainerSerialization.h"

// must be included after all other headers
#include "LintFree.h"

/// Constructor
CvCityConnections::CvCityConnections(void)
{
	Reset();
}

/// Destructor
CvCityConnections::~CvCityConnections(void)
{
	Reset();
}

/// Init
void CvCityConnections::Init(CvPlayer* pPlayer)
{
	Reset();

	m_pPlayer = pPlayer;

	m_aBuildingsAllowWaterRoutes.clear();
	CvBuildingXMLEntries* pkBuildingEntries = GC.GetGameBuildings();
	for(int i = 0; i < pkBuildingEntries->GetNumBuildings(); i++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(i);
		CvBuildingEntry* pkBuildingInfo = pkBuildingEntries->GetEntry(eBuilding);
		if(pkBuildingInfo)
		{
			if(pkBuildingInfo->AllowsWaterRoutes())
			{
				m_aBuildingsAllowWaterRoutes.push_back(eBuilding);
			}
		}
	}
}

/// Uninit
void CvCityConnections::Reset(void)
{
	m_pPlayer = NULL;
	m_plotsWithConnectionToCapital.clear();
	m_connectionState.clear();
	m_plotIdsToConnect.clear();
	m_bDirty = true;
}

template<typename CityConnections, typename Visitor>
void CvCityConnections::Serialize(CityConnections& cityConnections, Visitor& visitor)
{
	visitor(cityConnections.m_plotsWithConnectionToCapital);
	visitor(cityConnections.m_connectionState);
	visitor(cityConnections.m_plotIdsToConnect);
}

/// Serialization read
void CvCityConnections::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvCityConnections::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& loadFrom, CvCityConnections::CityConnectionTypes& writeTo)
{
	int value = 0;
	loadFrom >> value;
	writeTo = static_cast<CvCityConnections::CityConnectionTypes>(value);
	return loadFrom;
}
FDataStream& operator<<(FDataStream& saveTo, const CvCityConnections::CityConnectionTypes& readFrom)
{
	int value = static_cast<int>(readFrom);
	saveTo << value;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvCityConnections& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}
FDataStream& operator<<(FDataStream& saveTo, const CvCityConnections& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}

/// Update - called from within CvPlayer
void CvCityConnections::Update(void)
{
	//important, do this first to avoid endless recursion!
	m_bDirty = false;

	if(m_pPlayer->isBarbarian())
		return;

	UpdatePlotsToConnect();
	UpdateRouteInfo();
}

/// Update the city ids to the correct ones
void CvCityConnections::UpdatePlotsToConnect(void)
{
	m_plotIdsToConnect.clear();

	bool bIsIndustrial = GET_TEAM(m_pPlayer->getTeam()).GetBestPossibleRoute() == GC.getGame().GetIndustrialRoute();
	bool bHaveGoldToSpare = m_pPlayer->GetTreasury()->CalculateBaseNetGoldTimes100() > 1000;

	vector<PlayerTypes> vTeamPlayers = GET_TEAM(m_pPlayer->getTeam()).getPlayers();
	for (size_t i = 0; i < vTeamPlayers.size(); i++)
	{
		PlayerTypes ePlayer = vTeamPlayers[i];

		//cities
		int iLoop;
		for(CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			int iPlotIndex = pLoopCity->plot()->GetPlotIndex();
			m_plotIdsToConnect.push_back(iPlotIndex);
		}

		//select strategically important points
		//this logic is similar to tactical target selection (AI_TACTICAL_TARGET_DEFENSIVE_BASTION, AI_TACTICAL_TARGET_CITADEL)
		vector<PlayerTypes> vUnfriendlyMajors = GET_PLAYER(ePlayer).GetUnfriendlyMajors();
		const PlotIndexContainer& vPlots = GET_PLAYER(ePlayer).GetPlots();
		for (size_t j=0; j<vPlots.size(); j++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(vPlots[j]);
			if (pLoopPlot->isWater())
				continue;

			if (pLoopPlot->isCity())
				continue;

			//ignore plots that are owned by a city we are razing
			if (pLoopPlot->getOwningCity() && pLoopPlot->getOwningCity()->IsRazing())
				continue;

			if (pLoopPlot->isImpassable(m_pPlayer->getTeam()))
				continue;

			//ignore plots which are not exposed
			if (vUnfriendlyMajors.empty() || !pLoopPlot->IsBorderLand(m_pPlayer->GetID(), vUnfriendlyMajors))
				continue;

			//natural defenses
			if (pLoopPlot->defenseModifier(m_pPlayer->getTeam(), false, false) >= 25 || pLoopPlot->IsChokePoint())
			{
				m_plotIdsToConnect.push_back(pLoopPlot->GetPlotIndex());
			}
			else if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				//citadels and forts
				CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(pLoopPlot->getImprovementType());
				if (pImprovementInfo && pImprovementInfo->GetDefenseModifier() >= 20)
					m_plotIdsToConnect.push_back(pLoopPlot->GetPlotIndex());
			}

			//in industrial era, AI becomes much more generous with routes ...
			if (bIsIndustrial && bHaveGoldToSpare && pLoopPlot->IsAdjacentOwnedByTeamOtherThan(m_pPlayer->getTeam(), false, true))
				m_plotIdsToConnect.push_back(pLoopPlot->GetPlotIndex());
		}
	}

	//quests
	for (int i=MAX_MAJOR_CIVS; i<MAX_CIV_PLAYERS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;
		if(ShouldConnectToOtherPlayer(ePlayer))
		{
			CvCity* pOtherCapital = GET_PLAYER(ePlayer).getCapitalCity();
			if(pOtherCapital)
			{
				int iPlotIndex = pOtherCapital->plot()->GetPlotIndex();
				m_plotIdsToConnect.push_back(iPlotIndex);
			}
		}
	}
}

CvCityConnections::CityConnectionTypes CvCityConnections::GetConnectionState(const CvCity* pCityA, const CvCity* pCityB) const
{
	if(!pCityA || !pCityB)
		return CONNECTION_NONE;

	if (pCityA==pCityB)
		return CONNECTION_NONE;
	
	AllCityConnectionStore::const_iterator it1 = m_connectionState.find(pCityA->GetID());
	if (it1==m_connectionState.end())
		return CONNECTION_NONE;
	
	pair<int, int> destination(pCityB->getOwner(), pCityB->GetID());
	AllCityConnectionStore::value_type::second_type::const_iterator it2 = it1->second.find(destination);
	if (it2==it1->second.end())
		return CONNECTION_NONE;

	return it2->second;
}

bool CvCityConnections::AreCitiesDirectlyConnected(const CvCity * pCityA, const CvCity * pCityB, CityConnectionTypes eConnectionType)
{
	if (m_bDirty)
		Update();

	return (GetConnectionState(pCityA,pCityB) & eConnectionType) > 0;
}

const CvCityConnections::SingleCityConnectionStore& CvCityConnections::GetDirectConnectionsFromCity(const CvCity* pOrigin)
{
	if (m_bDirty)
		Update();

	AllCityConnectionStore::const_iterator it = m_connectionState.find(pOrigin->GetID());
	if (it!=m_connectionState.end())
		return it->second;

	return dummy;
}

void CvCityConnections::UpdateRouteInfo(void)
{
	if(m_pPlayer->getNumCities() <= 1)
		return;

	//allow mods to set connectivity as well - this is a bit strange, there is no check for industrial, and direct connection makes no sense
	bool bCallDirectEvents = false;
	bool bCallIndirectEvents = false;
	if (MOD_EVENTS_CITY_CONNECTIONS) 
	{
		// Events to determine if we support alternative direct and/or indirect route types
		if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CityConnections, m_pPlayer->GetID(), true) == GAMEEVENTRETURN_TRUE) {
			bCallDirectEvents = true;
		}

		if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CityConnections, m_pPlayer->GetID(), false) == GAMEEVENTRETURN_TRUE) {
			bCallIndirectEvents = true;
		}
	}

	m_connectionState.clear();

	for (PlotIndexStore::iterator it = m_plotIdsToConnect.begin(); it != m_plotIdsToConnect.end(); ++it)
	{
		CvCity* pStartCity = GC.getMap().plotByIndexUnchecked( *it )->getPlotCity();
		
		//special code for noncity connections
		if (pStartCity == NULL)
			continue;

		//we assume no city is connected to the capital
		if (pStartCity->getOwner()==m_pPlayer->GetID())
		{
			pStartCity->SetRouteToCapitalConnected(false, true);
			pStartCity->SetIndustrialRouteToCapitalConnected(false);
		}

		ReachablePlots roadPlots, railroadPlots, waterPlots;
		SPathFinderUserData data(m_pPlayer->GetID(),PT_CITY_CONNECTION_LAND, ROUTE_ROAD);
		if (!pStartCity->IsBlockaded(DOMAIN_LAND))
		{
			roadPlots= GC.GetStepFinder().GetPlotsInReach( pStartCity->getX(),pStartCity->getY(), data);
			data.iTypeParameter = ROUTE_RAILROAD;
			railroadPlots = GC.GetStepFinder().GetPlotsInReach( pStartCity->getX(),pStartCity->getY(), data);
		}

		// See if we have a harbor / lighthouse, whatever
		bool bStartCityAllowsWater = false;
		for(int i = 0; i < (int)m_aBuildingsAllowWaterRoutes.size(); i++)
			if(pStartCity->GetCityBuildings()->GetNumActiveBuilding(m_aBuildingsAllowWaterRoutes[i]) > 0)
				bStartCityAllowsWater = true;
		if (bStartCityAllowsWater && !pStartCity->IsBlockaded(DOMAIN_SEA))
		{
			data.iTypeParameter = NO_ROUTE;
			data.ePathType = PT_CITY_CONNECTION_WATER;
			waterPlots = GC.GetStepFinder().GetPlotsInReach( pStartCity->getX(),pStartCity->getY(), data);
		}

		//start with an empty map
		SingleCityConnectionStore localConnections;

		for (ReachablePlots::iterator it = roadPlots.begin(); it != roadPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->iPlotIndex );
			if (pPlot->isCity() && pPlot!=pStartCity->plot())
			{
				CvCity* pEndCity = pPlot->getPlotCity();
				if (pEndCity->IsBlockaded(DOMAIN_LAND))
					continue;
				
				pair<int, int> destination( pEndCity->getOwner(),pEndCity->GetID() );
				SingleCityConnectionStore::iterator lala = localConnections.find(destination);
				if ( lala == localConnections.end() )
					localConnections.insert( std::make_pair(destination,CONNECTION_NONE) );

				//a bit ugly but what can you do
				lala = localConnections.find(destination);
				lala->second = (CityConnectionTypes) (lala->second + CONNECTION_ROAD);
			}
		}

		for (ReachablePlots::iterator it = railroadPlots.begin(); it != railroadPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->iPlotIndex );
			if (pPlot->isCity() && pPlot!=pStartCity->plot())
			{
				CvCity* pEndCity = pPlot->getPlotCity();
				if (pEndCity->IsBlockaded(DOMAIN_LAND))
					continue;
				
				pair<int, int> destination( pEndCity->getOwner(),pEndCity->GetID() );
				SingleCityConnectionStore::iterator lala = localConnections.find(destination);
				if ( lala == localConnections.end() )
					localConnections.insert( std::make_pair(destination,CONNECTION_NONE) );

				//a bit ugly but what can you do
				lala = localConnections.find(destination);
				lala->second = (CityConnectionTypes) (lala->second + CONNECTION_RAILROAD);
			}
		}

		for (ReachablePlots::iterator it = waterPlots.begin(); it != waterPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->iPlotIndex );
			if (pPlot->isCity() && pPlot!=pStartCity->plot())
			{
				CvCity* pEndCity = pPlot->getPlotCity();

				// See if we have a harbor / lighthouse, whatever
				bool bEndCityAllowsWater = false;
				for(int i = 0; i < (int)m_aBuildingsAllowWaterRoutes.size(); i++)
					if(pEndCity->GetCityBuildings()->GetNumActiveBuilding(m_aBuildingsAllowWaterRoutes[i]) > 0)
						bEndCityAllowsWater = true;

				if (!bEndCityAllowsWater || pEndCity->IsBlockaded(DOMAIN_SEA))
					continue;
				
				pair<int, int> destination( pEndCity->getOwner(),pEndCity->GetID() );
				SingleCityConnectionStore::iterator lala = localConnections.find(destination);
				if ( lala == localConnections.end() )
					localConnections.insert( std::make_pair(destination,CONNECTION_NONE) );

				//a bit ugly but what can you do
				lala = localConnections.find(destination);
				lala->second = (CityConnectionTypes) (lala->second + CONNECTION_HARBOR);
			}
		}

		//save the result
		m_connectionState[pStartCity->GetID()] = localConnections;
	}

	//for any cities which are not linked now, check what lua says
	int iCityLoopA = 0, iCityLoopB = 0;
	for(CvCity* pCityA = m_pPlayer->firstCity(&iCityLoopA); pCityA != NULL; pCityA = m_pPlayer->nextCity(&iCityLoopA))
	{
		for(CvCity* pCityB = m_pPlayer->firstCity(&iCityLoopB); pCityB != NULL; pCityB = m_pPlayer->nextCity(&iCityLoopB))
		{
			//maybe check the upper half of the matrix only? is there a requirement that connections must be symmetrical?
			if (pCityA==pCityB)
				continue;

			bool bLuaRouteFound = false;
			if (!AreCitiesDirectlyConnected(pCityA,pCityB,CONNECTION_ANY_LAND) && bCallDirectEvents)
			{
				// Event to determine if connected by an alternative direct route type
				bLuaRouteFound = (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CityConnected, m_pPlayer->GetID(), pCityA->getX(), pCityA->getY(), pCityB->getX(), pCityB->getY(), true) == GAMEEVENTRETURN_TRUE);
			}

			if (!AreCitiesDirectlyConnected(pCityA,pCityB,CONNECTION_HARBOR) && bCallIndirectEvents)
			{
				// Event to determine if connected by an alternative indirect route type
				bLuaRouteFound = (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CityConnected, m_pPlayer->GetID(), pCityA->getX(), pCityA->getY(), pCityB->getX(), pCityB->getY(), false) == GAMEEVENTRETURN_TRUE);
			}

			if (bLuaRouteFound)
			{
				AllCityConnectionStore::iterator it = m_connectionState.find(pCityA->GetID());
				SingleCityConnectionStore localConnections;
				if (it!=m_connectionState.end())
					localConnections = it->second;

				//no matter whether "direct" or "indirect", we pretend it's a harbor
				//this works as is with the capital connection check below
				localConnections.insert( make_pair( make_pair(pCityB->getOwner(),pCityB->GetID()),CONNECTION_HARBOR));

				//don't forget to save it
				m_connectionState[pCityA->GetID()] = localConnections;
			}
		}
	}

	//finally, see how far we can go from the capital over mixed routes
	PlotIndexStore previousPlotsWithConnection = m_plotsWithConnectionToCapital;
	m_plotsWithConnectionToCapital.clear();

	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if (pCapital)
	{
		//need to check those later
		std::vector<CvCity*> vConnectedCities;
		std::vector<CvCity*> vDisconnectedCities;

		//Let's check for road first (railroad also counts as road)
		//Very important to set up m_connectionState for direct connections first!
		SPathFinderUserData data(m_pPlayer->GetID(), PT_CITY_CONNECTION_MIXED, ROUTE_ROAD);
		ReachablePlots capitalRoadConnectedPlots = GC.GetStepFinder().GetPlotsInReach( pCapital->getX(),pCapital->getY(), data);
		for (ReachablePlots::iterator it = capitalRoadConnectedPlots.begin(); it != capitalRoadConnectedPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->iPlotIndex );

			//if it's one of our own cities, set the connection flag - also for the capital itself
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity && pCity->getOwner() == m_pPlayer->GetID())
			{
				pCity->SetRouteToCapitalConnected(true);
				vConnectedCities.push_back(pCity);
			}
		}
		
		//Set industrial routes as needed.
		if ( GET_TEAM(m_pPlayer->getTeam()).GetBestPossibleRoute()==GC.getGame().GetIndustrialRoute() )
		{
			//with water and railroad only 
			data.iTypeParameter = ROUTE_RAILROAD;
			ReachablePlots capitalRailroadConnectedPlots = GC.GetStepFinder().GetPlotsInReach( pCapital->getX(),pCapital->getY(), data);
			for (ReachablePlots::iterator it = capitalRailroadConnectedPlots.begin(); it != capitalRailroadConnectedPlots.end(); ++it)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->iPlotIndex );

				//if it's one of our own cities, set the connection flag - also for the capital itself
				CvCity* pCity = pPlot->getPlotCity();
				if (pCity && pCity->getOwner()==m_pPlayer->GetID())
					pCity->SetIndustrialRouteToCapitalConnected(true);
			}
		}

		//Now set up the city connection flags for the plots with a route
		data.ePathType = PT_CITY_CONNECTION_LAND;
		data.iTypeParameter = ROUTE_ROAD;
		for (size_t i = 0; i < vConnectedCities.size(); i++)
		{
			for (size_t j = i + 1; j < vConnectedCities.size(); j++)
			{
				//find the shortest path between any two connected cities
				SPath path = GC.GetStepFinder().GetPath(vConnectedCities[i]->plot(), vConnectedCities[j]->plot(), data);
				for (int k = 0; k < path.length(); k++)
				{
					CvPlot* pPlot = path.get(k);
					if (pPlot && !pPlot->isWater() && !pPlot->isCity()) //should be only land, but doesn't hurt to check
						m_plotsWithConnectionToCapital.push_back(pPlot->GetPlotIndex());
				}
			}
		}

		int iCityLoop = 0;
		for (CvCity* pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop))
		{
			bool bDisconnected = true;
			for (size_t i = 0; i < vConnectedCities.size(); i++)
			{
				if (vConnectedCities[i] == pCity)
				{
					bDisconnected = false;
					break;
				}
			}
			if (bDisconnected)
				vDisconnectedCities.push_back(pCity);
		}
		if (vDisconnectedCities.size() > 0)
		{
			for (size_t i = 0; i < vDisconnectedCities.size(); i++)
			{
				vDisconnectedCities[i]->SetRouteToCapitalConnected(false);
				vDisconnectedCities[i]->SetIndustrialRouteToCapitalConnected(false);
			}
		}
	}

	//now set the plot flags for bonus yields
	CheckPlotRouteStateChanges(previousPlotsWithConnection,m_plotsWithConnectionToCapital);
}

void CvCityConnections::SetDirty(void)
{
	m_bDirty = true;
}

std::vector<int> CvCityConnections::GetPlotsToConnect()
{
	if (m_bDirty)
		Update();

	return m_plotIdsToConnect;
}

bool CvCityConnections::ShouldConnectToOtherPlayer(PlayerTypes eOtherPlayer)
{
	bool result = false;

	// shouldn't be able to connect to yourself
	if(m_pPlayer->GetID() == eOtherPlayer)
	{
		return false;
	}

	if(!GET_PLAYER(eOtherPlayer).isAlive())
	{
		return false;
	}

	if(GET_PLAYER(eOtherPlayer).isBarbarian())
	{
		return false;
	}

	CvPlayer* pOtherPlayer = &(GET_PLAYER(eOtherPlayer));

	// only majors and minors should connect to each other at this point.
	bool bMajorMinor = m_pPlayer->isMinorCiv() != pOtherPlayer->isMinorCiv();
	if(!bMajorMinor)
	{
		return false;
	}

	if(m_pPlayer->isMinorCiv())
	{
		CvPlayer* pMajorCiv = pOtherPlayer;

		// If the major is a human, don't decide a connection to a minor is desirable on their behalf
		if(pMajorCiv->isHuman())
		{
			return false;
		}

		if(!m_pPlayer->GetMinorCivAI()->IsActiveQuestForPlayer(pMajorCiv->GetID(), MINOR_CIV_QUEST_ROUTE))
		{
			return false;
		}

		result = true;
	}
	else // player is a major
	{
		CvPlayer* pMinorPlayer = pOtherPlayer;
		if(!pMinorPlayer->isAlive())
		{
			return false;
		}

		if (!m_pPlayer->isHuman()&& !m_pPlayer->GetDiplomacyAI()->IsWantToRouteConnectToMinor(pMinorPlayer->GetID()))
		{
			return false;
		}

		if(!pMinorPlayer->GetMinorCivAI()->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_ROUTE))
		{
			return false;
		}

		result = true;
	}

	return result;
}

void CvCityConnections::CheckPlotRouteStateChanges(PlotIndexStore& lastState, PlotIndexStore& newState)
{
	//make sure the input is sorted and unique
	std::stable_sort(lastState.begin(),lastState.end());
	lastState.erase( std::unique(lastState.begin(),lastState.end()), lastState.end() );
	std::stable_sort(newState.begin(),newState.end());
	newState.erase( std::unique(newState.begin(),newState.end()), newState.end() );

	PlotIndexStore addedPlots( newState.size() );
	PlotIndexStore removedPlots( lastState.size() );

	PlotIndexStore::iterator lastAdded = std::set_difference(newState.begin(),newState.end(),lastState.begin(),lastState.end(),addedPlots.begin());
	PlotIndexStore::iterator lastRemoved = std::set_difference(lastState.begin(),lastState.end(),newState.begin(),newState.end(),removedPlots.begin());

	addedPlots.resize( lastAdded - addedPlots.begin() );
	removedPlots.resize( lastRemoved - removedPlots.begin() );

	for (PlotIndexStore::iterator it = removedPlots.begin(); it!=removedPlots.end(); ++it)
	{
		// indicate removed route
		CvPlot* pPlot = GC.getMap().plotByIndex( *it );
		pPlot->SetCityConnection(m_pPlayer->GetID(), false);
	}

	for (PlotIndexStore::iterator it = addedPlots.begin(); it!=addedPlots.end(); ++it)
	{
		// broadcast new connected trade route
		CvPlot* pPlot = GC.getMap().plotByIndex( *it );
		pPlot->SetCityConnection(m_pPlayer->GetID(), true);
	}
}

