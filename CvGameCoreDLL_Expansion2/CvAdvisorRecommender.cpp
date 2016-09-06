/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);;

	CvCityBuildable buildable;
	int iWeight;

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

		iWeight = pCityStrategy->GetBuildingProductionAI()->GetWeight(eBuilding);
		m_aCityBuildables.push_back(buildable, iWeight);
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
		iWeight = pCityStrategy->GetUnitProductionAI()->GetWeight(eUnit);
		m_aCityBuildables.push_back(buildable, iWeight);
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
		iWeight = pCityStrategy->GetProjectProductionAI()->GetWeight(eProject);
		m_aCityBuildables.push_back(buildable, iWeight);
	}

	// reweigh by cost
	for(int iI = 0; iI < m_aCityBuildables.size(); iI++)
	{
		buildable = m_aCityBuildables.GetElement(iI);

		// Compute the new weight and change it
		int iNewWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_aCityBuildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
		m_aCityBuildables.SetWeight(iI, iNewWeight);
	}

	m_aCityBuildables.SortItems();

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

	for(int i = 0; i < m_aFinalRoundBuildables.size(); i++)
	{
		m_aFinalRoundBuildables.SortItems();

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
	if(m_aRecommendedBuilds[eAdvisor].m_eBuildableType == CITY_BUILDABLE_UNIT && (UnitTypes)m_aRecommendedBuilds[eAdvisor].m_iIndex == eUnit)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool  CvAdvisorRecommender::IsBuildingRecommended(BuildingTypes eBuilding, AdvisorTypes eAdvisor)
{
	if(m_aRecommendedBuilds[eAdvisor].m_eBuildableType == CITY_BUILDABLE_BUILDING && (BuildingTypes)m_aRecommendedBuilds[eAdvisor].m_iIndex == eBuilding)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool  CvAdvisorRecommender::IsProjectRecommended(ProjectTypes eProject, AdvisorTypes eAdvisor)
{
	if(m_aRecommendedBuilds[eAdvisor].m_eBuildableType == CITY_BUILDABLE_PROJECT && (ProjectTypes)m_aRecommendedBuilds[eAdvisor].m_iIndex == eProject)
	{
		return true;
	}
	else
	{
		return false;
	}
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

	RandomNumberDelegate fcn;
	int iTechLoop;

	// Use the synchronous random number generate
	// Asynchronous one would be:
	fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);
	//fcn = MakeDelegate (&GC.getGame(), &CvGame::getJonRandNum);

	// Loop through adding the researchable techs
	for(iTechLoop = 0; iTechLoop < pPlayerTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;
		if(pPlayerTechs->CanResearch(eTech))
		{
			m_aResearchableTechs.push_back(iTechLoop, pPlayerTechAI->GetWeight(eTech));
		}
	}

	// weigh by cost
	for(int iI = 0; iI < m_aResearchableTechs.size(); iI++)
	{
		TechTypes eTech = (TechTypes) m_aResearchableTechs.GetElement(iI);
		int iTurnsLeft = 0;

		iTurnsLeft = pPlayerTechs->GetResearchTurnsLeft(eTech, true);

		double fWeightDivisor;

		// 10 turns will add 0.02; 80 turns will add 0.16
		double fAdditionalTurnCostFactor = GC.getAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT() * iTurnsLeft;	// 0.015
		double fTotalCostFactor = GC.getAI_RESEARCH_WEIGHT_BASE_MOD() + fAdditionalTurnCostFactor;	// 0.15

		fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);

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

	m_aResearchableTechs.SortItems();

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
		m_aFinalRoundTechs.SortItems();

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

	switch(eAdvisor)
	{
	case ADVISOR_ECONOMIC:
		if(strFlavorName == "FLAVOR_CITY_DEFENSE")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_GROWTH")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_TILE_IMPROVEMENT")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_EXPANSION")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_GROWTH")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_TILE_IMPROVEMENT")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_INFRASTRUCTURE")
		{
			return 5;
		}
		else if(strFlavorName == "FLAVOR_PRODUCTION")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_GOLD")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_CULTURE")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_HAPPINESS")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_WONDER")
		{
			return 5;
		}
		else if(strFlavorName == "FLAVOR_WATER_CONNECTION")
		{
			return 5;
		}

		break;
	case ADVISOR_MILITARY:
		if(strFlavorName == "FLAVOR_OFFENSE")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_DEFENSE")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_CITY_DEFENSE")
		{
			return 5;
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
			return 10;
		}
		else if(strFlavorName == "FLAVOR_MOBILE")
		{
			return 7;
		}
		else if(strFlavorName == "FLAVOR_NAVAL")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_RECON")
		{
			return 3;
		}
		else if(strFlavorName == "FLAVOR_AIR")
		{
			return 10;
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
		break;
	case ADVISOR_FOREIGN:
		if(strFlavorName == "FLAVOR_CITY_DEFENSE")
		{
			return 1;
		}
		else if(strFlavorName == "FLAVOR_RECON")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_RECON")
		{
			return 10;
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
			return 10;
		}
		else if(strFlavorName == "FLAVOR_DIPLOMACY")
		{
			return 10;
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
			return 3;
		}
		else if(strFlavorName == "FLAVOR_NAVAL_RECON")
		{
			return 3;
		}
		else if(strFlavorName == "FLAVOR_AIR")
		{
			return 5;
		}
		else if(strFlavorName == "FLAVOR_SCIENCE")
		{
			return 10;
		}
		else if(strFlavorName == "FLAVOR_SPACESHIP")
		{
			return 10;
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
	int aiAdvisorValues[NUM_ADVISOR_TYPES];
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		aiAdvisorValues[ui] = 0;
	}

	CvBuildingEntry* pBuilding = NULL;
	CvUnitEntry*     pUnit     = NULL;
	CvProjectEntry*  pProject  = NULL;
	switch(buildable.m_eBuildableType)
	{
	case CITY_BUILDABLE_UNIT:
		pUnit = GC.getUnitInfo((UnitTypes)buildable.m_iIndex);
		break;
	case CITY_BUILDABLE_BUILDING:
		pBuilding = GC.getBuildingInfo((BuildingTypes)buildable.m_iIndex);
		break;
	case CITY_BUILDABLE_PROJECT:
		pProject = GC.getProjectInfo((ProjectTypes)buildable.m_iIndex);
		break;
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