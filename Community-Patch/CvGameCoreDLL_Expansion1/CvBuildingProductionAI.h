/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_BUILDING_PRODUCTION_AI_H
#define CIV5_BUILDING_PRODUCTION_AI_H

#include "CvWeightedVector.h"

// Allocate array for 50% more buildings than we have now (92)
//   May want to tune this number closer to shipping, though safe enough
//   given that each entry is only 8 bytes
#define SAFE_ESTIMATE_NUM_BUILDINGS 135

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
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Establish/retrieve weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(BuildingTypes eBuilding);

	// Recommend highest-weighted building
	BuildingTypes RecommendBuilding();

	// Logging
	void LogPossibleBuilds();

private:

	// Private data
	CvCity* m_pCity;
	CvCityBuildings* m_pCityBuildings;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> m_BuildingAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> m_Buildables;
};

#endif //CIV5_BUILDING_PRODUCTION_AI_H