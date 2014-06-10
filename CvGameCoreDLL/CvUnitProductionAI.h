/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_UNIT_PRODUCTION_AI_H
#define CIV5_UNIT_PRODUCTION_AI_H

// Allocate array for 50% more units than we have now (58)
//   May want to tune this number closer to shipping, though safe enough
//   given that each entry is only 8 bytes
#define SAFE_ESTIMATE_NUM_UNITS 90

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvUnitProductionAI
//!  \brief		Handles unit production decisions for one city
//
//!  Key Attributes:
//!  - Object created by CvCityStrategyAI (so one per city)
//!  - Uses flavor-based weighting choices (similar to one used for policies and techs)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitProductionAI
{
public:
	CvUnitProductionAI(CvCity *pCity, CvUnitXMLEntries *pUnits);
	~CvUnitProductionAI(void);

	// Initialization
	void Reset();

	// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Establish/retrieve weights for one flavor
	void AddFlavorWeights (FlavorTypes eFlavor, int iWeight);
	int GetWeight (UnitTypes eUnit);

	// Recommend highest-weighted unit
	UnitTypes RecommendUnit(UnitAITypes eUnitAIType = NO_UNITAI);

	// Logging
	void LogPossibleBuilds(UnitAITypes eUnitAIType = NO_UNITAI);

private:

	// Private data
	CvCity *m_pCity;
	CvUnitXMLEntries *m_pUnits;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_UNITS, true> m_UnitAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_UNITS, true> m_Buildables;
};

#endif //CIV5_UNIT_PRODUCTION_AI_H