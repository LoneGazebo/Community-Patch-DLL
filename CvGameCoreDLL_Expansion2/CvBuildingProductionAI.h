/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_BUILDING_PRODUCTION_AI_H
#define CIV5_BUILDING_PRODUCTION_AI_H

#include "CvWeightedVector.h"

class CvCity;
class CvCityBuildings;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvBuildingProductionAI
//!  \brief		Handles building production decisions for one city
//
//!  Key Attributes:
//!  - Object created by CvCityStrategyAI (so one per city)
//!  - Uses flavor-based weighting choices (similar to one used for policies and techs)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingProductionAI
{
public:
	CvBuildingProductionAI(CvCity* pCity, CvCityBuildings* pCityBuildings);
	~CvBuildingProductionAI(void);

	// Initialization
	void Reset();

	// Serialization routines
	template<typename BuildingProductionAI, typename Visitor>
	static void Serialize(BuildingProductionAI& buildingProductionAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Establish/retrieve weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(BuildingTypes eBuilding);

	// Logging
	void LogPossibleBuilds();
	// Fast version with precomputed stats
	int CheckBuildingBuildSanity(BuildingTypes eBuilding, int iValue, const SPlotStats& plotStats, const std::vector<int>& allExistingBuildings,
		bool bNoBestWonderCityCheck = false, bool bFreeBuilding = false, bool bIgnoreSituational = false);
	// Slow version
	int CheckBuildingBuildSanity(BuildingTypes eBuilding, int iValue, 
		bool bNoBestWonderCityCheck = false, bool bFreeBuilding = false, bool bIgnoreSituational = false);

private:

	// Private data
	CvCity* m_pCity;
	CvCityBuildings* m_pCityBuildings;
	CvWeightedVector<int> m_BuildingAIWeights;
	CvWeightedVector<int> m_Buildables;
};

FDataStream& operator>>(FDataStream&, CvBuildingProductionAI&);
FDataStream& operator<<(FDataStream&, const CvBuildingProductionAI&);

#endif //CIV5_BUILDING_PRODUCTION_AI_H