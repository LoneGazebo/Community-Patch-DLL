/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvAdvisorRecommender.h"
#include "CvGameCoreUtils.h"
#include "CvTechAI.h"

#include "LintFree.h"

CvAdvisorRecommender::CvAdvisorRecommender(void)
{
	Init();
}

CvAdvisorRecommender::~CvAdvisorRecommender(void)
{
	Uninit();
}

void CvAdvisorRecommender::Init(void)
{
	Reset();
}

void CvAdvisorRecommender::Uninit(void)
{
}

void CvAdvisorRecommender::Reset(void)
{
	ResetCity();
	ResetTechs();
}

void CvAdvisorRecommender::ResetCity()
{
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		m_aRecommendedBuilds[ui].m_eBuildableType = NOT_A_CITY_BUILDABLE;
		m_aRecommendedBuilds[ui].m_iIndex = -1;
		m_aRecommendedBuilds[ui].m_iTurnsToConstruct = -1;
	}

	m_aCityBuildables.clear();
	m_aFinalRoundBuildables.clear();
}

void CvAdvisorRecommender::UpdateCityRecommendations(CvCity* pCity)
{
	ResetCity();

	CvCityStrategyAI* pCityStrategy = pCity->GetCityStrategyAI();
	CvCityBuildable buildable;

	// buildings
	for(int iBuildingLoop = 0; iBuildingLoop < GC.GetGameBuildings()->GetNumBuildings(); iBuildingLoop++)
	{
		BuildingTypes eBuilding = (BuildingTypes)iBuildingLoop;
		if(!pCity->canConstruct(eBuilding))
		{
			continue;
		}

		buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
		buildable.m_iIndex = iBuildingLoop;
		buildable.m_iTurnsToConstruct = pCity->getProductionTurnsLeft(eBuilding, 0);

		int iFlavorWeight = pCityStrategy->GetBuildingProductionAI()->GetWeight(eBuilding);
		int iSaneWeight = pCityStrategy->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, iFlavorWeight, false, false);

		if (iSaneWeight > 0)
			m_aCityBuildables.push_back(buildable, iSaneWeight);
	}

	// units
	for(int iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{
		UnitTypes eUnit = (UnitTypes)iUnitLoop;
		if(!pCity->canTrain(eUnit))
		{
			continue;
		}

		buildable.m_eBuildableType = CITY_BUILDABLE_UNIT;
		buildable.m_iIndex = iUnitLoop;
		buildable.m_iTurnsToConstruct = pCity->getProductionTurnsLeft(eUnit, 0);
		
		int iFlavorWeight = pCityStrategy->GetUnitProductionAI()->GetWeight(eUnit);
		int iSaneWeight = pCityStrategy->GetUnitProductionAI()->CheckUnitBuildSanity(eUnit, false, iFlavorWeight, false, false);

		if (iSaneWeight > 0)
			m_aCityBuildables.push_back(buildable, iSaneWeight);
	}

	// projects
	for(int iProjectLoop = 0; iProjectLoop < GC.GetGameProjects()->GetNumProjects(); iProjectLoop++)
	{
		ProjectTypes eProject = (ProjectTypes)iProjectLoop;
		if(!pCity->canCreate(eProject))
		{
			continue;
		}

		buildable.m_eBuildableType = CITY_BUILDABLE_PROJECT;
		buildable.m_iIndex = iProjectLoop;
		buildable.m_iTurnsToConstruct = pCity->getProductionTurnsLeft(eProject, 0);
		
		int iFlavorWeight = pCityStrategy->GetProjectProductionAI()->GetWeight(eProject);
		int iSaneWeight = pCityStrategy->GetProjectProductionAI()->CheckProjectBuildSanity(eProject, iFlavorWeight);

		if (iSaneWeight > 0)
			m_aCityBuildables.push_back(buildable, iSaneWeight);
	}

	// reweigh by cost
	for(int iI = 0; iI < m_aCityBuildables.size(); iI++)
	{
		buildable = m_aCityBuildables.GetElement(iI);

		// Compute the new weight and change it
		int iNewWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_aCityBuildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
		m_aCityBuildables.SetWeight(iI, iNewWeight);
	}

	m_aCityBuildables.StableSortItems();

	// move top buildables into final round!
	for(int i = 0; i < NUM_ADVISOR_TYPES; i++)
	{
		// if index is out of bounds
		if(i >= m_aCityBuildables.size())
		{
			break;
		}

		m_aFinalRoundBuildables.push_back(m_aCityBuildables.GetElement(i), m_aCityBuildables.GetWeight(i));
	}

	//todo: find best buildable for each advisor, not best advisor for buildable?
	for(int i = 0; i < m_aFinalRoundBuildables.size(); i++)
	{
		m_aFinalRoundBuildables.StableSortItems();

		buildable = m_aFinalRoundBuildables.GetElement(0);
		int iScore = m_aFinalRoundBuildables.GetWeight(0);

		AdvisorTypes eAvailableAdvisor = FindUnassignedAdvisorForBuildable(pCity->getOwner(), buildable);
		if(eAvailableAdvisor != NO_ADVISOR_TYPE)
		{
			m_aRecommendedBuilds[eAvailableAdvisor] = buildable;
		}

		m_aFinalRoundBuildables.SetWeight(0, iScore / 2);
	}
}

bool CvAdvisorRecommender::IsUnitRecommended(UnitTypes eUnit, AdvisorTypes eAdvisor)
{
	switch (m_aRecommendedBuilds[eAdvisor].m_eBuildableType)
	{
	case CITY_BUILDABLE_UNIT:
	case CITY_BUILDABLE_UNIT_FOR_OPERATION:
	case CITY_BUILDABLE_UNIT_FOR_ARMY:
		return static_cast<UnitTypes>(m_aRecommendedBuilds[eAdvisor].m_iIndex) == eUnit;
	case NOT_A_CITY_BUILDABLE:
	case CITY_BUILDABLE_BUILDING:
	case CITY_BUILDABLE_PROJECT:
	case CITY_BUILDABLE_PROCESS:
		break;
	}
	return false;
}

bool  CvAdvisorRecommender::IsBuildingRecommended(BuildingTypes eBuilding, AdvisorTypes eAdvisor)
{
	return m_aRecommendedBuilds[eAdvisor].m_eBuildableType == CITY_BUILDABLE_BUILDING && (BuildingTypes)m_aRecommendedBuilds[eAdvisor].m_iIndex == eBuilding;
}

bool  CvAdvisorRecommender::IsProjectRecommended(ProjectTypes eProject, AdvisorTypes eAdvisor)
{
	return m_aRecommendedBuilds[eAdvisor].m_eBuildableType == CITY_BUILDABLE_PROJECT && (ProjectTypes)m_aRecommendedBuilds[eAdvisor].m_iIndex == eProject;
}

void CvAdvisorRecommender::ResetTechs()
{
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		m_aRecommendedTechs[ui] = NO_TECH;
	}

	m_aResearchableTechs.clear();
	m_aFinalRoundTechs.clear();
}

void CvAdvisorRecommender::UpdateTechRecommendations(PlayerTypes ePlayer)
{
	ResetTechs();
	CvPlayerTechs* pPlayerTechs = GET_PLAYER(ePlayer).GetPlayerTechs();
	CvTechAI* pPlayerTechAI = pPlayerTechs->GetTechAI();

	// Loop through adding the researchable techs
	for(int iTechLoop = 0; iTechLoop < pPlayerTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;
		if(pPlayerTechs->CanResearch(eTech))
		{
			m_aResearchableTechs.push_back(iTechLoop, pPlayerTechAI->GetWeight(eTech));
		}
	}

	for(int iI = 0; iI < m_aResearchableTechs.size(); iI++)
	{
		TechTypes eTech = (TechTypes) m_aResearchableTechs.GetElement(iI);
		int iTurnsLeft = pPlayerTechs->GetResearchTurnsLeft(eTech, true);

		// reweight by turns left
		double fTotalCostFactor = /*0.2f*/ GD_FLOAT_GET(AI_RESEARCH_WEIGHT_BASE_MOD) + (iTurnsLeft * /*0.035f*/ GD_FLOAT_GET(AI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT));
		double fWeightDivisor = pow((double)iTurnsLeft, fTotalCostFactor);

		// if the tech is free, then we don't want inverse the weighting. More expensive techs = better.
		int iNewWeight = 0;
		if(GET_PLAYER(ePlayer).GetNumFreeTechs() == 0)
		{
			iNewWeight = int(double(m_aResearchableTechs.GetWeight(iI)) / fWeightDivisor);
		}
		else
		{
			iNewWeight = m_aResearchableTechs.GetWeight(iI) * max(iTurnsLeft / 2, 1);
		}

		// Now actually change the weight
		m_aResearchableTechs.SetWeight(iI, iNewWeight);
	}

	m_aResearchableTechs.StableSortItems();

	// move techs into final round!
	for(int i = 0; i < NUM_ADVISOR_TYPES; i++)
	{
		// if index is out of bounds
		if(i >= m_aResearchableTechs.size())
		{
			break;
		}

		m_aFinalRoundTechs.push_back(m_aResearchableTechs.GetElement(i), m_aResearchableTechs.GetWeight(i));
	}

	for(int i = 0; i < m_aFinalRoundTechs.size(); i++)
	{
		m_aFinalRoundTechs.StableSortItems();

		TechTypes eTech = (TechTypes)m_aFinalRoundTechs.GetElement(0);
		int iScore = m_aFinalRoundTechs.GetWeight(0);

		AdvisorTypes eAvailableAdvisor = FindUnassignedAdvisorForTech(ePlayer, eTech);
		if(eAvailableAdvisor != NO_ADVISOR_TYPE)
		{
			m_aRecommendedTechs[eAvailableAdvisor] = eTech;
		}

		m_aFinalRoundTechs.SetWeight(0, iScore / 2);
	}
}

bool CvAdvisorRecommender::IsTechRecommended(TechTypes eTech, AdvisorTypes eAdvisor)
{
	return (m_aRecommendedTechs[eAdvisor] == eTech);
}

int CvAdvisorRecommender::AdvisorInterestInFlavor(AdvisorTypes eAdvisor, FlavorTypes eFlavor)
{
	CvString strFlavorName = GC.getFlavorTypes(eFlavor);

	//todo: normalize weights!
	switch(eAdvisor)
	{
	case NO_ADVISOR_TYPE:
		break;
	case ADVISOR_ECONOMIC:
		if(strFlavorName == "FLAVOR_CITY_DEFENSE")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_GROWTH")
		{
			return 7;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_TILE_IMPROVEMENT")
		{
			return 7;
		}
		else if(strFlavorName == "FLAVOR_EXPANSION")
		{
			return 11;
		}
		else if(strFlavorName == "FLAVOR_GROWTH")
		{
			return 15;
		}
		else if(strFlavorName == "FLAVOR_TILE_IMPROVEMENT")
		{
			return 13;
		}
		else if(strFlavorName == "FLAVOR_INFRASTRUCTURE")
		{
			return 9;
		}
		else if(strFlavorName == "FLAVOR_PRODUCTION")
		{
			return 16;
		}
		else if(strFlavorName == "FLAVOR_GOLD")
		{
			return 23;
		}
		else if(strFlavorName == "FLAVOR_CULTURE")
		{
			return 3;
		}
		else if(strFlavorName == "FLAVOR_HAPPINESS")
		{
			return 9;
		}
		else if(strFlavorName == "FLAVOR_WONDER")
		{
			return 5;
		}
		else if(strFlavorName == "FLAVOR_WATER_CONNECTION")
		{
			return 17;
		}

		break;
	case ADVISOR_MILITARY:
		if(strFlavorName == "FLAVOR_OFFENSE")
		{
			return 17;
		}
		else if(strFlavorName == "FLAVOR_DEFENSE")
		{
			return 16;
		}
		else if(strFlavorName == "FLAVOR_CITY_DEFENSE")
		{
			return 9;
		}
		else if(strFlavorName == "FLAVOR_MILITARY_TRAINING")
		{
			return 7;
		}
		else if(strFlavorName == "FLAVOR_RECON")
		{
			return 3;
		}
		else if(strFlavorName == "FLAVOR_RANGED")
		{
			return 13;
		}
		else if(strFlavorName == "FLAVOR_MOBILE")
		{
			return 7;
		}
		else if(strFlavorName == "FLAVOR_NAVAL")
		{
			return 15;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_RECON")
		{
			return 3;
		}
		else if(strFlavorName == "FLAVOR_AIR")
		{
			return 17;
		}
		else if(strFlavorName == "FLAVOR_INFRASTRUCTURE")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_PRODUCTION")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_NUKE")
		{
			return 10;
		}
		//MOD_AI_UNIT_PRODUCTION : New flavors
		else if (MOD_AI_UNIT_PRODUCTION)
		{
			if (strFlavorName == "FLAVOR_ARCHER")
			{
				return 10;
			}
			else if (strFlavorName == "FLAVOR_SIEGE")
			{
				return 10;
			}
			else if (strFlavorName == "FLAVOR_SKIRMISHER")
			{
				return 10;
			}
			else if (strFlavorName == "FLAVOR_NAVAL_MELEE")
			{
				return 10;
			}
			else if (strFlavorName == "FLAVOR_NAVAL_RANGED")
			{
				return 10;
			}
			else if (strFlavorName == "FLAVOR_SUBMARINE")
			{
				return 7;
			}
			else if (strFlavorName == "FLAVOR_BOMBER")
			{
				return 10;
			}
			else if (strFlavorName == "FLAVOR_FIGHTER")
			{
				return 10;
			}
		}
		break;
	case ADVISOR_FOREIGN:
		if(strFlavorName == "FLAVOR_CITY_DEFENSE")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_RECON")
		{
			return 13;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_RECON")
		{
			return 11;
		}
		else if(strFlavorName == "FLAVOR_GREAT_PEOPLE")
		{
			return 5;
		}
		else if(strFlavorName == "FLAVOR_WONDER")
		{
			return 5;
		}
		else if(strFlavorName == "FLAVOR_RELIGION")
		{
			return 13;
		}
		else if(strFlavorName == "FLAVOR_DIPLOMACY")
		{
			return 17;
		}
		else if(strFlavorName == "FLAVOR_NUKE")
		{
			return 5;
		}
		break;
	case ADVISOR_SCIENCE:
		if(strFlavorName == "FLAVOR_CITY_DEFENSE")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_TILE_IMPROVEMENT")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_TILE_IMPROVEMENT")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_RECON")
		{
			return 11;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_RECON")
		{
			return 11;
		}
		else if(strFlavorName == "FLAVOR_AIR")
		{
			return 7;
		}
		else if(strFlavorName == "FLAVOR_SCIENCE")
		{
			return 13;
		}
		else if(strFlavorName == "FLAVOR_SPACESHIP")
		{
			return 17;
		}
		else if(strFlavorName == "FLAVOR_NUKE")
		{
			return 3;
		}
		break;
	}
	return 0;
}

AdvisorTypes CvAdvisorRecommender::FindUnassignedAdvisorForTech(PlayerTypes ePlayer, TechTypes eTech)
{
	int aiAdvisorValues[NUM_ADVISOR_TYPES];
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		aiAdvisorValues[ui] = 0;
	}

	CvTechEntry* pTechEntry = GET_PLAYER(ePlayer).GetPlayerTechs()->GetTechs()->GetEntry(eTech);
	if(pTechEntry == NULL)
	{
		//Should never happen.
		return NO_ADVISOR_TYPE;
	}

	for(int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		FlavorTypes eFlavor = (FlavorTypes)i;
		int iTechFlavorValue = pTechEntry->GetFlavorValue(eFlavor);
		if(iTechFlavorValue <= 0)
		{
			continue;
		}

		for(uint uiAdvisor = 0; uiAdvisor < NUM_ADVISOR_TYPES; uiAdvisor++)
		{
			AdvisorTypes eAdvisor = (AdvisorTypes)uiAdvisor;
			aiAdvisorValues[uiAdvisor] += iTechFlavorValue * AdvisorInterestInFlavor(eAdvisor, eFlavor);
		}
	}

	AdvisorTypes eWinningAdvisor = NO_ADVISOR_TYPE;
	int iHighestValue = 0;
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		// skip this advisor if already assigned
		if(m_aRecommendedTechs[ui] != NO_TECH)
		{
			continue;
		}

		if(aiAdvisorValues[ui] > iHighestValue)
		{
			eWinningAdvisor = (AdvisorTypes)ui;
			iHighestValue = aiAdvisorValues[ui];
		}
	}

	return eWinningAdvisor;
}

AdvisorTypes CvAdvisorRecommender::FindUnassignedAdvisorForBuildable(PlayerTypes /*ePlayer*/, CvCityBuildable& buildable)
{
	CvBuildingEntry* pBuilding = NULL;
	CvUnitEntry*     pUnit     = NULL;
	CvProjectEntry*  pProject  = NULL;
	switch(buildable.m_eBuildableType)
	{
	case CITY_BUILDABLE_UNIT:
	case CITY_BUILDABLE_UNIT_FOR_OPERATION:
	case CITY_BUILDABLE_UNIT_FOR_ARMY:
		pUnit = GC.getUnitInfo((UnitTypes)buildable.m_iIndex);
		break;
	case CITY_BUILDABLE_BUILDING:
		pBuilding = GC.getBuildingInfo((BuildingTypes)buildable.m_iIndex);
		break;
	case CITY_BUILDABLE_PROJECT:
		pProject = GC.getProjectInfo((ProjectTypes)buildable.m_iIndex);
		break;
	case NOT_A_CITY_BUILDABLE:
	case CITY_BUILDABLE_PROCESS:
		return NO_ADVISOR_TYPE;
	}

	int aiAdvisorValues[NUM_ADVISOR_TYPES];
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		aiAdvisorValues[ui] = 0;
	}

	for(int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		FlavorTypes eFlavor = (FlavorTypes)i;
		int iFlavorValue = 0;
		if(pBuilding)
		{
			iFlavorValue = pBuilding->GetFlavorValue(eFlavor);
		}
		else if(pUnit)
		{
			iFlavorValue = pUnit->GetFlavorValue(eFlavor);
		}
		else if(pProject)
		{
			iFlavorValue = pProject->GetFlavorValue(eFlavor);
		}

		if(iFlavorValue <= 0)
		{
			continue;
		}

		for(uint uiAdvisor = 0; uiAdvisor < NUM_ADVISOR_TYPES; uiAdvisor++)
		{
			AdvisorTypes eAdvisor = (AdvisorTypes)uiAdvisor;
			aiAdvisorValues[uiAdvisor] += iFlavorValue * AdvisorInterestInFlavor(eAdvisor, eFlavor);
		}
	}

	AdvisorTypes eWinningAdvisor = NO_ADVISOR_TYPE;
	int iHighestValue = 0;
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		// skip this advisor if already assigned
		if(m_aRecommendedBuilds[ui].m_eBuildableType != NOT_A_CITY_BUILDABLE)
		{
			continue;
		}

		if(aiAdvisorValues[ui] > iHighestValue)
		{
			eWinningAdvisor = (AdvisorTypes)ui;
			iHighestValue = aiAdvisorValues[ui];
		}
	}

	return eWinningAdvisor;

}