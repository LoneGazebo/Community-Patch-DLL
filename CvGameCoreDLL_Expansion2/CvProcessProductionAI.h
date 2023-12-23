/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_PROCESS_PRODUCTION_AI_H
#define CIV5_PROCESS_PRODUCTION_AI_H

class CvCity;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvProcessProductionAI
//!  \brief		Handles process production decisions for one city
//
//!  Key Attributes:
//!  - Object created by CvCityStrategyAI (so one per city)
//!  - Uses flavor-based weighting choices (similar to one used for policies and techs)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProcessProductionAI
{
public:
	CvProcessProductionAI(CvCity* pCity);
	~CvProcessProductionAI(void);

	// Initialization
	void Reset();

	// Serialization routines
	template<typename ProcessProductionAI, typename Visitor>
	static void Serialize(ProcessProductionAI& processProductionAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Establish/retrieve weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(ProcessTypes eProject);

#if defined(MOD_BALANCE_CORE)
	int CheckProcessBuildSanity(ProcessTypes eProcess, int iTempWeight);
#endif

	// Logging
	void LogPossibleBuilds();

private:

	// Private data
	CvCity* m_pCity;
	CvWeightedVector<int> m_ProcessAIWeights;
	CvWeightedVector<int> m_Buildables;
};

FDataStream& operator>>(FDataStream&, CvProcessProductionAI&);
FDataStream& operator<<(FDataStream&, const CvProcessProductionAI&);

#endif //CIV5_PROJECT_PRODUCTION_AI_H