/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_CITY_CONNECTIONS_H
#define CIV5_CITY_CONNECTIONS_H

class CvPlayer;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCityConnections
//!  \brief		Maintains route connections for a city
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCityConnections
{
public:
	CvCityConnections(void);
	~CvCityConnections(void);

	void Init(CvPlayer* pPlayer);
	void Reset(void);

	//// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void Update(void);
	bool Empty(void);  // if there are no cities in the route list

	//we don't only connect or own cities but also some others (eg for quests)
	std::vector<int> GetPlotsToConnect() const { return m_cityPlotIDs; }

	enum CityConnectionTypes
	{
		CONNECTION_NONE = 0,
		CONNECTION_ROAD = 1,
		CONNECTION_RAILROAD = 2,
		CONNECTION_ANY_LAND = 3,
		CONNECTION_HARBOR = 4,
		CONNECTION_ANY = 7
	};
	typedef std::map<int,CityConnectionTypes> SingleCityConnectionStore;
	typedef std::map<int,SingleCityConnectionStore> AllCityConnectionStore;
	typedef std::vector<int> PlotIndexStore;

	bool AreCitiesDirectlyConnected( const CvCity* pCityA, const CvCity* pCityB, CityConnectionTypes eConnectionType ) const;
	SingleCityConnectionStore GetDirectConnectionsFromCity(const CvCity* pOrigin) const;

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	void UpdateCityPlotIDs(void);
	void UpdateRouteInfo(void);
	void CheckPlotRouteStateChanges(PlotIndexStore& lastState, PlotIndexStore& newState);
	CityConnectionTypes GetConnectionState( const CvCity* pCityA, const CvCity* pCityB ) const;
	bool ShouldConnectToOtherPlayer(PlayerTypes eMinor);

	AllCityConnectionStore m_connectionState;
	PlotIndexStore m_plotsWithConnectionToCapital;
	PlotIndexStore m_cityPlotIDs;
	std::vector<BuildingTypes> m_aBuildingsAllowWaterRoutes;
	CvPlayer* m_pPlayer;
};

#endif //CIV5_BUILDER_TASKING_AI_H
