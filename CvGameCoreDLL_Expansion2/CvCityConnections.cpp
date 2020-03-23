/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

/// Serialization read
void CvCityConnections::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	size_t nItems;
	kStream >> nItems;
	m_plotsWithConnectionToCapital.clear();
	for (size_t i = 0; i < nItems; i++)
	{
		int temp;
		kStream >> temp;
		m_plotsWithConnectionToCapital.push_back(temp);
	}

	kStream >> nItems;
	m_connectionState.clear();
	for (size_t i = 0; i < nItems; i++)
	{
		int origin;
		kStream >> origin;
		size_t nItems2;
		kStream >> nItems2;
		for (size_t j = 0; j < nItems2; j++)
		{
			pair<int, int> destination;
			int temp3;
			kStream >> destination;
			kStream >> temp3;
			m_connectionState[origin][destination] = (CityConnectionTypes)temp3;
		}
	}

	// read in city locations
	kStream >> m_plotIdsToConnect;
}

/// Serialization write
void CvCityConnections::Write(FDataStream& kStream) const
{
	// Version number to maintain backwards compatibility
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_plotsWithConnectionToCapital.size();
	for (PlotIndexStore::const_iterator it=m_plotsWithConnectionToCapital.begin(); it!=m_plotsWithConnectionToCapital.end(); ++it)
	{
		kStream << *(it);
	}

	kStream << m_connectionState.size();
	for (AllCityConnectionStore::const_iterator it=m_connectionState.begin(); it!=m_connectionState.end(); ++it)
	{
		kStream << it->first;
		kStream << it->second.size();
		for (AllCityConnectionStore::value_type::second_type::const_iterator it2=it->second.begin(); it2!=it->second.end(); ++it2)
		{
			kStream << it2->first;
			kStream << it2->second;
		}
	}

	// read in city locations
	kStream << m_plotIdsToConnect;
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

		//citadels etc
		const PlotIndexContainer& vPlots = GET_PLAYER(ePlayer).GetPlots();
		for (size_t j=0; j<vPlots.size(); j++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(vPlots[j]);

			if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
				continue;
						
			//ignore plots which are not exposed
			if (!pLoopPlot->IsBorderLand(m_pPlayer->GetID()))
				continue;

			CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(pLoopPlot->getImprovementType());
			if (pImprovementInfo && pImprovementInfo->GetDefenseModifier() >= 50)
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
			pStartCity->SetRouteToCapitalConnected(false);
			pStartCity->SetIndustrialRouteToCapitalConnected(false);
		}

		ReachablePlots roadPlots, railroadPlots, waterPlots;
		SPathFinderUserData data(m_pPlayer->GetID(),PT_CITY_CONNECTION_LAND, ROUTE_ROAD);
		if (!pStartCity->IsBlockaded(false))
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
		if (bStartCityAllowsWater && !pStartCity->IsBlockaded(true))
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
				if (pEndCity->IsBlockaded(false))
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
				if (pEndCity->IsBlockaded(false))
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

				if (!bEndCityAllowsWater || pEndCity->IsBlockaded(true))
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
	int iCityLoopA, iCityLoopB;
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
	std::sort(lastState.begin(),lastState.end());
	lastState.erase( std::unique(lastState.begin(),lastState.end()), lastState.end() );
	std::sort(newState.begin(),newState.end());
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

