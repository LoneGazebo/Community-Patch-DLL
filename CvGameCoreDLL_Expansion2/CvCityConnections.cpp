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
	m_cityPlotIDs.clear();
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
		int temp;
		kStream >> temp;
		size_t nItems2;
		kStream >> nItems2;
		for (size_t j = 0; j < nItems2; j++)
		{
			int temp2, temp3;
			kStream >> temp2;
			kStream >> temp3;
			m_connectionState[temp][temp2] = (CityConnectionTypes)temp3;
		}
	}

	// read in city locations
	kStream >> nItems;
	m_cityPlotIDs.clear();
	for(size_t i = 0; i < nItems; i++)
	{
		int iValue;
		kStream >> iValue;
		m_cityPlotIDs.push_back(iValue);
	}
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
	for (ConnectionStore::const_iterator it=m_connectionState.begin(); it!=m_connectionState.end(); ++it)
	{
		kStream << it->first;
		kStream << it->second.size();
		for (ConnectionStore::value_type::second_type::const_iterator it2=it->second.begin(); it2!=it->second.end(); ++it2)
		{
			kStream << it2->first;
			kStream << it2->second;
		}
	}

	// read in city locations
	kStream << m_cityPlotIDs.size();
	for (PlotIndexStore::const_iterator it=m_cityPlotIDs.begin(); it!=m_cityPlotIDs.end(); ++it)
	{
		kStream << *(it);
	}
}

/// Update - called from within CvPlayer
void CvCityConnections::Update(void)
{
	if(m_pPlayer->isBarbarian())
	{
		return;
	}

	UpdateCityPlotIDs();
	UpdateRouteInfo();
}

/// Update the city ids to the correct ones
void CvCityConnections::UpdateCityPlotIDs(void)
{
	m_cityPlotIDs.clear();

	for(uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if(GET_PLAYER(ePlayer).isBarbarian())
		{
			continue;
		}

		TeamTypes ePlayerTeam = GET_PLAYER(ePlayer).getTeam();
		TeamTypes eMyPlayerTeam = m_pPlayer->getTeam();

		if(ePlayerTeam == eMyPlayerTeam)
		{
			// player's city
			int iLoop;
			for(CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
			{
				CvAssertMsg(pLoopCity->plot(), "pLoopCity does not have a plot. What??");
				int iPlotIndex = pLoopCity->plot()->GetPlotIndex();
				m_cityPlotIDs.push_back(iPlotIndex);
			}
		}
		else if(ShouldConnectToOtherPlayer(ePlayer))
		{
			CvCity* pOtherCapital = GET_PLAYER(ePlayer).getCapitalCity();
			if(pOtherCapital)
			{
				int iPlotIndex = pOtherCapital->plot()->GetPlotIndex();
				m_cityPlotIDs.push_back(iPlotIndex);
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
	
	ConnectionStore::const_iterator it1 = m_connectionState.find(pCityA->GetID());
	if (it1==m_connectionState.end())
		return CONNECTION_NONE;
	
	ConnectionStore::value_type::second_type::const_iterator it2 = it1->second.find(pCityB->GetID());
	if (it2==it1->second.end())
		return CONNECTION_NONE;

	return it2->second;
}

bool CvCityConnections::AreCitiesConnected(const CvCity * pCityA, const CvCity * pCityB, CityConnectionTypes eConnectionType) const
{
	return (GetConnectionState(pCityA,pCityB) & eConnectionType) > 0;
}

void CvCityConnections::UpdateRouteInfo(void)
{
	m_connectionState.clear();

	for (PlotIndexStore::iterator it = m_cityPlotIDs.begin(); it != m_cityPlotIDs.end(); ++it)
	{
		CvCity* pStartCity = GC.getMap().plotByIndexUnchecked( *it )->getPlotCity();

		//we assume no city is connected to the capital
		if (pStartCity->getOwner()==m_pPlayer->GetID())
		{
			pStartCity->SetRouteToCapitalConnected(false);
			pStartCity->SetIndustrialRouteToCapitalConnected(false);
		}

		ReachablePlots roadPlots, railroadPlots, harborPlots;
		SPathFinderUserData data(m_pPlayer->GetID(),PT_CITY_CONNECTION_LAND, ROUTE_ROAD);
		if (!pStartCity->IsBlockaded(false))
		{
			roadPlots= GC.GetStepFinder().GetPlotsInReach( pStartCity->getX(),pStartCity->getY(), data, 0);
			data.iTypeParameter = ROUTE_RAILROAD;
			railroadPlots = GC.GetStepFinder().GetPlotsInReach( pStartCity->getX(),pStartCity->getY(), data, 0);
		}

		// See if we have a harbor
		bool bStartCityHasHarbor = false;
		for(int i = 0; i < (int)m_aBuildingsAllowWaterRoutes.size(); i++)
			if(pStartCity->GetCityBuildings()->GetNumActiveBuilding(m_aBuildingsAllowWaterRoutes[i]) > 0)
				bStartCityHasHarbor = true;

		if (bStartCityHasHarbor && !pStartCity->IsBlockaded(true))
		{
			data.iTypeParameter = NO_ROUTE;
			data.ePathType = PT_CITY_CONNECTION_WATER;
			harborPlots = GC.GetStepFinder().GetPlotsInReach( pStartCity->getX(),pStartCity->getY(), data, 0);
		}

		//start with an empty map
		std::map<int,CityConnectionTypes> localConnections;

		for (ReachablePlots::iterator it = roadPlots.begin(); it != roadPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->first );
			if (pPlot->isCity() && pPlot!=pStartCity->plot())
			{
				CvCity* pEndCity = pPlot->getPlotCity();
				if (pEndCity->IsBlockaded(false))
					continue;
				
				std::map<int,CityConnectionTypes>::iterator lala = localConnections.find(pEndCity->GetID());
				if ( lala == localConnections.end() )
					localConnections.insert( std::make_pair(pEndCity->GetID(),CONNECTION_NONE) );

				//a bit ugly but what can you do
				lala = localConnections.find(pEndCity->GetID());
				lala->second = (CityConnectionTypes) (lala->second + CONNECTION_ROAD);
			}
		}

		for (ReachablePlots::iterator it = railroadPlots.begin(); it != railroadPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->first );
			if (pPlot->isCity() && pPlot!=pStartCity->plot())
			{
				CvCity* pEndCity = pPlot->getPlotCity();
				if (pEndCity->IsBlockaded(false))
					continue;
				
				std::map<int,CityConnectionTypes>::iterator lala = localConnections.find(pEndCity->GetID());
				if ( lala == localConnections.end() )
					localConnections.insert( std::make_pair(pEndCity->GetID(),CONNECTION_NONE) );

				//a bit ugly but what can you do
				lala = localConnections.find(pEndCity->GetID());
				lala->second = (CityConnectionTypes) (lala->second + CONNECTION_RAILROAD);
			}
		}

		for (ReachablePlots::iterator it = harborPlots.begin(); it != harborPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->first );
			if (pPlot->isCity() && pPlot!=pStartCity->plot())
			{
				CvCity* pEndCity = pPlot->getPlotCity();

				// See if we have a harbor
				bool bEndCityHasHarbor = false;
				for(int i = 0; i < (int)m_aBuildingsAllowWaterRoutes.size(); i++)
					if(pEndCity->GetCityBuildings()->GetNumActiveBuilding(m_aBuildingsAllowWaterRoutes[i]) > 0)
						bEndCityHasHarbor = true;

				if (!bEndCityHasHarbor || pEndCity->IsBlockaded(true))
					continue;
				
				std::map<int,CityConnectionTypes>::iterator lala = localConnections.find(pEndCity->GetID());
				if ( lala == localConnections.end() )
					localConnections.insert( std::make_pair(pEndCity->GetID(),CONNECTION_NONE) );

				//a bit ugly but what can you do
				lala = localConnections.find(pEndCity->GetID());
				lala->second = (CityConnectionTypes) (lala->second + CONNECTION_HARBOR);
			}
		}

		//save the result
		m_connectionState[pStartCity->GetID()] = localConnections;
	}

	//finally, see how far we can go from the capital over mixed routes
	PlotIndexStore previousPlotsWithConnection = m_plotsWithConnectionToCapital;
	m_plotsWithConnectionToCapital.clear();

	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if (pCapital)
	{
		SPathFinderUserData data(m_pPlayer->GetID(), PT_CITY_CONNECTION_MIXED, ROUTE_ROAD);
		ReachablePlots capitalRoadConnectedPlots = GC.GetStepFinder().GetPlotsInReach( pCapital->getX(),pCapital->getY(), data, 0);
		for (ReachablePlots::iterator it = capitalRoadConnectedPlots.begin(); it != capitalRoadConnectedPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->first );
			if (!pPlot->isWater() && !pPlot->isCity()) //should be only land, but doesn't hurt to check
				m_plotsWithConnectionToCapital.push_back(pPlot->GetPlotIndex());

			//if it's one of our own cities, set the connection flag - also for the capital itself
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity && pCity->getOwner()==m_pPlayer->GetID())
				pCity->SetRouteToCapitalConnected(true);

		}

		data.iTypeParameter = ROUTE_RAILROAD;
		ReachablePlots capitalRailroadConnectedPlots = GC.GetStepFinder().GetPlotsInReach( pCapital->getX(),pCapital->getY(), data, 0);
		for (ReachablePlots::iterator it = capitalRailroadConnectedPlots.begin(); it != capitalRailroadConnectedPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked( it->first );

			//if it's one of our own cities, set the connection flag - also for the capital itself
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity && pCity->getOwner()==m_pPlayer->GetID())
				pCity->SetIndustrialRouteToCapitalConnected(true);

		}
	}

	//now set the plot flags for bonus yields
	CheckPlotRouteStateChanges(previousPlotsWithConnection,m_plotsWithConnectionToCapital);
}

/// if there are no cities in the route list
bool CvCityConnections::Empty(void)
{
	return m_cityPlotIDs.empty();
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

		// If the major is a human, don't decide a connection to a minor is desirable on their behalf
		if(m_pPlayer->isHuman())
		{
			return false;
		}

		if(!m_pPlayer->GetDiplomacyAI()->IsWantToRouteConnectToMinor(pMinorPlayer->GetID()))
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

void CvCityConnections::CheckPlotRouteStateChanges(const PlotIndexStore& lastState, const PlotIndexStore& newState)
{
	PlotIndexStore addedPlots( newState.size() );
	PlotIndexStore removedPlots( lastState.size() );

	PlotIndexStore::iterator lastAdded = std::set_difference(newState.begin(),newState.end(),lastState.begin(),lastState.end(),addedPlots.begin());
	PlotIndexStore::iterator lastRemoved =std::set_difference(lastState.begin(),lastState.end(),newState.begin(),newState.end(),removedPlots.begin());

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

