/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_PROJECT_PRODUCTION_AI_H
#define CIV5_PROJECT_PRODUCTION_AI_H

class CvCity;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvProjectProductionAI
//!  \brief		Handles project production decisions for one city
//
//!  Key Attributes:
//!  - Object created by CvCityStrategyAI (so one per city)
//!  - Uses flavor-based weighting choices (similar to one used for policies and techs)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProjectProductionAI
{
public:
	CvProjectProductionAI(CvCity* pCity);
	~CvProjectProductionAI(void);

	// Initialization
	void Reset();

	// Serialization routines
	template<typename ProjectProductionAI, typename Visitor>
	static void Serialize(ProjectProductionAI& projectProductionAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Establish/retrieve weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(ProjectTypes eProject);

	// Recommend highest-weighted Project
	ProjectTypes RecommendProject();
#if defined(MOD_BALANCE_CORE)
	int CheckProjectBuildSanity(ProjectTypes eProject, int iTempWeight);
#endif

	// Logging
	void LogPossibleBuilds();

private:

	// Private data
	CvCity* m_pCity;
	CvWeightedVector<int> m_ProjectAIWeights;
	CvWeightedVector<int> m_Buildables;
};

FDataStream& operator>>(FDataStream&, CvProjectProductionAI&);
FDataStream& operator<<(FDataStream&, const CvProjectProductionAI&);

#endif //CIV5_PROJECT_PRODUCTION_AI_H
