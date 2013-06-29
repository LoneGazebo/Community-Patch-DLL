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
	void Uninit(void);

	//// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void Update(void);
	void UpdateCityPlotIDs(void);
	void UpdateRouteInfo(void);

	void ResetRouteInfo(void);
	void ResetCityPlotIDs(void);

	bool IsEmpty(void);  // if there are no cities in the route list

	uint GetIndexFromCity(CvCity* pCity);
	CvCity* GetCityFromIndex(int iIndex);

	uint GetNumConnectableCities(void);

	bool ShouldConnectToOtherPlayer(PlayerTypes eMinor);

	CvPlayer* m_pPlayer;

	typedef enum RouteState
	{
	    HAS_ANY_ROUTE   = 0x1,
	    HAS_WATER_ROUTE = 0x2,
	    HAS_BEST_ROUTE  = 0x4,
	};

	struct RouteInfo
	{
		RouteInfo() :
			m_cRouteState(0)
			, m_cPassEval(0)
		{
		}

		char m_cRouteState;
		char m_cPassEval; // used to create the data, but should not be referenced as a value
	};

	uint m_uiRouteInfosDimension;
	RouteInfo* m_aRouteInfos;

	RouteInfo* GetRouteInfo(uint uiFirstCityIndex, uint uiSecondCityIndex);

	//typedef FStaticVector< RouteInfo, SAFE_ESTIMATE_NUM_CITIES, true, c_eCiv5GameplayDLL, 0 > RouteInfosRow;
	//FFastVector< RouteInfosRow, false, c_eCiv5GameplayDLL, 0 > m_aaRouteInfos;
	FStaticVector<int, SAFE_ESTIMATE_NUM_CITIES, true, c_eCiv5GameplayDLL, 0> m_aiCityPlotIDs;
	FStaticVector<BuildingTypes, 10, true, c_eCiv5GameplayDLL, 0> m_aBuildingsAllowWaterRoutes;

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	void UpdatePlotRouteStates(void);
	void BroadcastPlotRouteStateChanges(void);
	void ConnectPlotRoute(CvPlot* pPlot);

	void ResizeRouteInfo(uint uiNewSize);

	// these are used to update the engine
	typedef enum PlotRouteState
	{
	    NO_CONNECTION = 0x0,
	    CONNECTION = 0x1,
	    CONNECTION_LAST_TURN = 0x2
	};

	struct PlotRouteInfo
	{
		PlotRouteInfo() :
			m_iPlotIndex(-1)
			, m_bPlotRouteState(0)
		{
		}

		int m_iPlotIndex;
		byte m_bPlotRouteState;
	};

	FStaticVector<PlotRouteInfo, 100, true, c_eCiv5GameplayDLL, 0> m_aPlotRouteInfos;
};

#endif //CIV5_BUILDER_TASKING_AI_H
