/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//  PURPOSE: Advisors recommend things in the city production chooser and tech chooser
//
#pragma once
#ifndef CIV5_ADVISOR_RECOMMENDER_H
#define CIV5_ADVISOR_RECOMMENDER_H

class CvAdvisorRecommender
{
public:
	CvAdvisorRecommender();
	~CvAdvisorRecommender();

	void Init();
	void Uninit();
	void Reset();

	// city recommendations
	void ResetCity();
	void UpdateCityRecommendations(CvCity* pCity);
	bool IsUnitRecommended(UnitTypes eUnit, AdvisorTypes eAdvisor);
	bool IsBuildingRecommended(BuildingTypes eBuilding, AdvisorTypes eAdvisor);
	bool IsProjectRecommended(ProjectTypes eProject, AdvisorTypes eAdvisor);

	// tech recommendations
	void ResetTechs();
	void UpdateTechRecommendations(PlayerTypes ePlayer);
	bool IsTechRecommended(TechTypes eTech, AdvisorTypes eAdvisor);

	int AdvisorInterestInFlavor(AdvisorTypes eAdvisor, FlavorTypes eFlavor);
	AdvisorTypes FindUnassignedAdvisorForTech(PlayerTypes ePlayer, TechTypes eTech);
	AdvisorTypes FindUnassignedAdvisorForBuildable(PlayerTypes ePlayer, CvCityBuildable& buildable);

	CvCityBuildable m_aRecommendedBuilds[NUM_ADVISOR_TYPES];
	TechTypes m_aRecommendedTechs[NUM_ADVISOR_TYPES];

	CvWeightedVector<int, 80, true> m_aResearchableTechs;
	CvWeightedVector<int, NUM_ADVISOR_TYPES, true> m_aFinalRoundTechs;

	CvWeightedVector<CvCityBuildable, (50), true> m_aCityBuildables;
	CvWeightedVector<CvCityBuildable, NUM_ADVISOR_TYPES, true> m_aFinalRoundBuildables;
};

#endif	//CIV5_ADVISOR_COUNSEL_H
