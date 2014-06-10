/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvAdvisorCounsel.h"
#include "CvGameCoreUtils.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvDiplomacyAI.h"
#include "CvMinorCivAI.h"
#include "CvTypes.h"

// must be included after all other headers
#include "LintFree.h"

#define NUM_COUNSEL_SLOTS 50

CvAdvisorCounsel::CvAdvisorCounsel(void)
{
	Init();
}

CvAdvisorCounsel::~CvAdvisorCounsel(void)
{
	Uninit();
}

void CvAdvisorCounsel::Init(void)
{
	m_aCounsel.resize(NUM_COUNSEL_SLOTS);
	Reset();
}

void CvAdvisorCounsel::Uninit(void)
{
	m_aCounsel.clear();
}

void CvAdvisorCounsel::Reset(void)
{
	for(uint ui = 0; ui < m_aCounsel.size(); ui++)
	{
		m_aCounsel[ui].m_eAdvisor = NO_ADVISOR_TYPE;
		m_aCounsel[ui].m_iValue = -1;
	}
}

bool CounselSort(const CvAdvisorCounsel::Counsel Counsel1, const CvAdvisorCounsel::Counsel Counsel2)
{
	// if the advisors are different types
	if(Counsel1.m_eAdvisor != Counsel2.m_eAdvisor)
	{
		// adjust the advisor values so that empty values will be moved to the end of the list
		AdvisorTypes eCounselAdvisor1 = Counsel1.m_eAdvisor;
		AdvisorTypes eCounselAdvisor2 = Counsel2.m_eAdvisor;
		if(eCounselAdvisor1 == NO_ADVISOR_TYPE)
		{
			eCounselAdvisor1 = NUM_ADVISOR_TYPES;
		}

		if(eCounselAdvisor2 == NO_ADVISOR_TYPE)
		{
			eCounselAdvisor2 = NUM_ADVISOR_TYPES;
		}

		return eCounselAdvisor1 < eCounselAdvisor2;
	}

	return Counsel1.m_iValue > Counsel2.m_iValue;
}

void CvAdvisorCounsel::BuildCounselList(PlayerTypes ePlayer)
{
	Reset();

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	uint uiCounselIndex = 0;
	Localization::String strLoc;

	if(GET_PLAYER(ePlayer).getNumCities() == 0)
	{
		bool bSuccess;
		strLoc = Localization::Lookup("TXT_KEY_BUILD_A_CITY_MCFLY_ECONOMIC");
		bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_ECONOMIC, strLoc.toUTF8(), 99);
		CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
		uiCounselIndex++;

		strLoc = Localization::Lookup("TXT_KEY_BUILD_A_CITY_MCFLY_MILITARY");
		bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_MILITARY, strLoc.toUTF8(), 99);
		CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
		uiCounselIndex++;

		strLoc = Localization::Lookup("TXT_KEY_BUILD_A_CITY_MCFLY_SCIENCE");
		bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 99);
		CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
		uiCounselIndex++;

		strLoc = Localization::Lookup("TXT_KEY_BUILD_A_CITY_MCFLY_FOREIGN");
		bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_FOREIGN, strLoc.toUTF8(), 99);
		CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
		uiCounselIndex++;

		std::sort(&m_aCounsel[0], &m_aCounsel[0] + m_aCounsel.size(), CounselSort);
		return;
	}

	// go through economic strategies
	CvEconomicAI* pEconomicAI = GET_PLAYER(ePlayer).GetEconomicAI();

	for(int iStrategiesLoop = 0; iStrategiesLoop < pEconomicAI->GetEconomicAIStrategies()->GetNumEconomicAIStrategies(); iStrategiesLoop++)
	{
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) iStrategiesLoop;
		CvEconomicAIStrategyXMLEntry* pStrategy = pEconomicAI->GetEconomicAIStrategies()->GetEntry(iStrategiesLoop);

		if(pEconomicAI->IsUsingStrategy(eStrategy))
		{
			// if this strategy has an advisor set, then try to add it to the list
			if(pStrategy->GetAdvisor() != NO_ADVISOR_TYPE)
			{
				strLoc = Localization::Lookup(pStrategy->GetAdvisorCounselText());
				bool bSuccess = SetCounselEntry(uiCounselIndex, pStrategy->GetAdvisor(), strLoc.toUTF8(), pStrategy->GetAdvisorCounselImportance());
				CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
				if(!bSuccess)
				{
					break;
				}
				uiCounselIndex++;
			}
		}
	}

	// go through military strategies
	CvMilitaryAI* pMilitaryAI = GET_PLAYER(ePlayer).GetMilitaryAI();

	for(int iStrategiesLoop = 0; iStrategiesLoop < pMilitaryAI->GetMilitaryAIStrategies()->GetNumMilitaryAIStrategies(); iStrategiesLoop++)
	{
		MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) iStrategiesLoop;

		if(pMilitaryAI->IsUsingStrategy(eStrategy))
		{
			CvMilitaryAIStrategyXMLEntry* pStrategy = pMilitaryAI->GetMilitaryAIStrategies()->GetEntry(iStrategiesLoop);
			CvAssert(pStrategy != NULL);
			if(pStrategy)
			{
				// if this strategy has an advisor set, then try to add it to the list
				if(pStrategy->GetAdvisor() != NO_ADVISOR_TYPE)
				{
					strLoc = Localization::Lookup(pStrategy->GetAdvisorCounselText());
					bool bSuccess = SetCounselEntry(uiCounselIndex, pStrategy->GetAdvisor(), strLoc.toUTF8(), pStrategy->GetAdvisorCounselImportance());
					CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
					if(!bSuccess)
					{
						break;
					}
					uiCounselIndex++;
				}
			}
		}
	}

	// go through city strategies
	int iLoop;
	CvCity* pLoopCity;
	CvCityStrategyAI* pCityStrategyAI;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		pCityStrategyAI = pLoopCity->GetCityStrategyAI();
		for(int i = 0; i < pCityStrategyAI->GetAICityStrategies()->GetNumAICityStrategies(); i++)
		{
			AICityStrategyTypes eStrategy = (AICityStrategyTypes)i;
			CvAICityStrategyEntry* pStrategy = pCityStrategyAI->GetAICityStrategies()->GetEntry(i);
			if(pCityStrategyAI->IsUsingCityStrategy(eStrategy))
			{
				// if this strategy has an advisor set, then try to add it to the list
				if(pStrategy->GetAdvisor() != NO_ADVISOR_TYPE)
				{
					strLoc = Localization::Lookup(pStrategy->GetAdvisorCounselText());
					strLoc << pLoopCity->getNameKey();
					bool bSuccess = SetCounselEntry(uiCounselIndex, pStrategy->GetAdvisor(), strLoc.toUTF8(), pStrategy->GetAdvisorCounselImportance());
					CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
					if(!bSuccess)
					{
						break;
					}
					uiCounselIndex++;
				}
			}
		}
	}

	// find flavor science
	FlavorTypes eFlavorScience = NO_FLAVOR;
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
		{
			eFlavorScience = (FlavorTypes)iFlavorLoop;
			break;
		}
	}

	// cycle through all the science advisor tips
	// along with military recommendations
	TechTypes eMilitaryUnitTech = NO_TECH;
	UnitTypes eRecommendedUnit = NO_UNIT;
	int iMaxCombatValue = 0;
	TechTypes eMilitaryBuildingTech = NO_TECH;
	BuildingTypes eRecommendedBuilding = NO_BUILDING;
	CvPlayerTechs* pPlayerTechs = GET_PLAYER(ePlayer).GetPlayerTechs();
	for(int iTechLoop = 0; iTechLoop < pPlayerTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;
		if(pPlayerTechs->CanResearch(eTech) && pPlayerTechs->GetCurrentResearch() != eTech)
		{
			// go through recommended research for special units
			if(pPlayerTechs->GetCivTechPriority(eTech) > 1)
			{
				bool bUniquenessFound = false;
				if(pPlayerTechs->GetCivTechUniqueUnit(eTech) != NO_UNIT)
				{
					strLoc = Localization::Lookup("TXT_KEY_TECHSTRATEGY_CIV_BONUS_UNIT");
					strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
					strLoc << GC.getUnitInfo(pPlayerTechs->GetCivTechUniqueUnit(eTech))->GetTextKey();
					strLoc << GET_PLAYER(ePlayer).getCivilizationInfo().getAdjectiveKey();

					bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 2);
					CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
					if(!bSuccess)
					{
						break;
					}
					uiCounselIndex++;
					bUniquenessFound = true;
				}

				if(pPlayerTechs->GetCivTechUniqueBuilding(eTech) != NO_BUILDING)
				{
					strLoc = Localization::Lookup("TXT_KEY_TECHSTRATEGY_CIV_BONUS_BUILDING");
					strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
					strLoc << GC.getBuildingInfo(pPlayerTechs->GetCivTechUniqueBuilding(eTech))->GetTextKey();
					strLoc << GET_PLAYER(ePlayer).getCivilizationInfo().getAdjectiveKey();
					bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 2);
					CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
					if(!bSuccess)
					{
						break;
					}
					uiCounselIndex++;
					bUniquenessFound = true;
				}

				if(pPlayerTechs->GetCivTechUniqueImprovement(eTech) != NO_IMPROVEMENT)
				{
					strLoc = Localization::Lookup("TXT_KEY_TECHSTRATEGY_CIV_BONUS_IMPROVEMENT");
					strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
					strLoc << GC.getImprovementInfo(pPlayerTechs->GetCivTechUniqueImprovement(eTech))->GetTextKey();
					strLoc << GET_PLAYER(ePlayer).getCivilizationInfo().getAdjectiveKey();
					bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 2);
					CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
					if(!bSuccess)
					{
						break;
					}
					uiCounselIndex++;
					bUniquenessFound = true;
				}

				if(!bUniquenessFound)
				{
					// we don't have text for this
					strLoc = Localization::Lookup("TXT_KEY_TECHSTRATEGY_CIV_BONUS");
					strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
					strLoc << GET_PLAYER(ePlayer).getCivilizationInfo().getAdjectiveKey();
					bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 2);
					CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
					if(!bSuccess)
					{
						break;
					}
					uiCounselIndex++;
				}
			}

			// go through recommended research for resources
			if(pPlayerTechs->GetLocaleTechPriority(eTech) > 1)
			{
				// say we need this for a special resource
				strLoc = Localization::Lookup("TXT_KEY_TECHSTRATEGY_RESOURCE");
				strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
				strLoc << GC.getResourceInfo(pPlayerTechs->GetLocaleTechResource(eTech))->GetTextKey();
				bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 60);
				CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
				if(!bSuccess)
				{
					break;
				}
				uiCounselIndex++;
			}

			// look at available buildings
			int iNumBuildingInfos = GC.getNumBuildingInfos();
			BuildingTypes eBuilding1 = NO_BUILDING;
			BuildingTypes eBuilding2 = NO_BUILDING;
			BuildingTypes eBuilding3 = NO_BUILDING;
			for(int iI = 0; iI < iNumBuildingInfos; iI++)
			{
				BuildingTypes eBuilding = (BuildingTypes)iI;
				CvBuildingEntry* pBuilding = GC.getBuildingInfo(eBuilding);
				if(pBuilding && pBuilding->GetPrereqAndTech() == eTech && pBuilding->GetFlavorValue(eFlavorScience) > 0 && !GC.getGame().isBuildingClassMaxedOut((BuildingClassTypes)pBuilding->GetBuildingClassType()))
				{
					if(eBuilding1 == NO_BUILDING)
					{
						eBuilding1 = eBuilding;
					}
					else if(eBuilding2 == NO_BUILDING)
					{
						eBuilding2 = eBuilding;
					}
					else if(eBuilding3 == NO_BUILDING)
					{
						eBuilding3 = eBuilding;
					}
				}
			}

			if(eBuilding1 != NO_BUILDING)
			{
				if(eBuilding2 != NO_BUILDING)
				{
					if(eBuilding3 != NO_BUILDING)
					{
						strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_TECH_THAT_PROVIDES_SCIENCE_BUILDING_3");
						strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
						strLoc << GC.getBuildingInfo(eBuilding1)->GetTextKey();
						strLoc << GC.getBuildingInfo(eBuilding2)->GetTextKey();
						strLoc << GC.getBuildingInfo(eBuilding3)->GetTextKey();
					}
					else
					{
						strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_TECH_THAT_PROVIDES_SCIENCE_BUILDING_2");
						strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
						strLoc << GC.getBuildingInfo(eBuilding1)->GetTextKey();
						strLoc << GC.getBuildingInfo(eBuilding2)->GetTextKey();
					}
				}
				else
				{
					strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_TECH_THAT_PROVIDES_SCIENCE_BUILDING_1");
					strLoc << pPlayerTechs->GetTechs()->GetEntry(eTech)->GetTextKey();
					strLoc << GC.getBuildingInfo(eBuilding1)->GetTextKey();
				}

				bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 15);
				CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
				if(!bSuccess)
				{
					break;
				}
				uiCounselIndex++;
			}

			for(int i = 0; i < GC.getNumUnitInfos(); i++)
			{
				UnitTypes eUnitType = (UnitTypes)i;
				CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnitType);
				if(pUnitEntry)
				{
					if(pUnitEntry->GetPrereqAndTech() == eTech && pUnitEntry->GetUnitCombatType() != NO_UNITCOMBAT)
					{
						UnitTypes eCivUnit = ((UnitTypes)(GET_PLAYER(ePlayer).getCivilizationInfo().getCivilizationUnits(pUnitEntry->GetUnitClassType())));
						if(eUnitType != eCivUnit)
						{
							continue;
						}

						int iCombatValue = MAX(pUnitEntry->GetCombat(), pUnitEntry->GetRangedCombat());
						if(iCombatValue <= iMaxCombatValue)
						{
							continue;
						}

						eMilitaryUnitTech = eTech;
						eRecommendedUnit = eUnitType;
						iMaxCombatValue = iCombatValue;
					}
				}
			}

			for(int i = 0; i < GC.getNumBuildingInfos(); i++)
			{
				BuildingTypes eBuildingType = (BuildingTypes)i;
				CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuildingType);
				if(!pBuildingEntry || pBuildingEntry->GetPrereqAndTech() != eTech)
				{
					continue;
				}

				bool bProvidesExperience = false;
				for(int iDomain = 0; iDomain < NUM_DOMAIN_TYPES; iDomain++)
				{
					if(pBuildingEntry->GetDomainFreeExperience(iDomain) > 0)
					{
						bProvidesExperience = true;
					}
				}

				if(!bProvidesExperience)
				{
					continue;
				}

				eMilitaryBuildingTech = eTech;
				eRecommendedBuilding = eBuildingType;
			}
		}
	}

	if(eRecommendedUnit != NO_UNIT)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eRecommendedUnit);
		if(pkUnitInfo)
		{
			strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_RESEARCH_MILITARY_UNITS");
			strLoc << pPlayerTechs->GetTechs()->GetEntry(eMilitaryUnitTech)->GetTextKey();
			strLoc << pkUnitInfo->GetTextKey();

			bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_MILITARY, strLoc.toUTF8(), 15);
			DEBUG_VARIABLE(bSuccess);
			CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
			uiCounselIndex++;
		}
	}

	if(eRecommendedBuilding != NO_BUILDING)
	{
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eRecommendedBuilding);
		if(pkBuildingInfo)
		{
			strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_RESEARCH_MILITARY_BUILDING");
			strLoc << pPlayerTechs->GetTechs()->GetEntry(eMilitaryBuildingTech)->GetTextKey();
			strLoc << pkBuildingInfo->GetTextKey();

			bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_MILITARY, strLoc.toUTF8(), 15);
			DEBUG_VARIABLE(bSuccess);
			CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
			uiCounselIndex++;
		}
	}

	// evaluate the player's science yield according to population
	{
		int iRating = 1;
		int iScience = GET_PLAYER(ePlayer).GetScience();
		int iPopulation = GET_PLAYER(ePlayer).getTotalPopulation();
		int iRatio = 0;
		if(iPopulation > 0)
		{
			iRatio = (iScience * 100) / iPopulation;
		}

		CvTreasury* pTreasury = GET_PLAYER(ePlayer).GetTreasury();

		int iGold = pTreasury->GetGold();
		int iRevenue = GET_PLAYER(ePlayer).calculateGoldRate();

		if((iGold + iRevenue) > 0)
		{
			if(iRatio < 110)
			{
				strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_INFO_DOING_POORLY");
				strLoc << iScience;
				strLoc << iPopulation;
				iRating = 10;
			}
			else if(iRatio > 125)
			{
				strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_INFO_DOING_WELL");
				strLoc << iScience;
				strLoc << iPopulation;
				iRating = 5;
			}
			else
			{
				strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_INFO");
				strLoc << iScience;
				strLoc << iPopulation;
				iRating = 1;
			}
		}
		else
		{
			strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_BANKRUPT");
			iRating = 99;
		}

		bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), iRating);
		DEBUG_VARIABLE(bSuccess);
		CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
		uiCounselIndex++;
	}



	// diplomacy info
	// go through all the other major civs and build info on them
	// for each opposing major
	CvDiplomacyAI* pDiplomacyAI = GET_PLAYER(ePlayer).GetDiplomacyAI();
	for(int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; iPlayer++)
	{
		if(GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) || GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE))
		{
			continue;
		}

		// reset the ratings per player
		int iMessageRating = 0;
		AdvisorTypes eAdvisor = NO_ADVISOR_TYPE;

		PlayerTypes eOtherPlayer = (PlayerTypes)iPlayer;
		TeamTypes eOtherTeam = GET_PLAYER(eOtherPlayer).getTeam();

		// don't evaluate yourself
		if(eOtherPlayer == ePlayer)
		{
			continue;
		}

		// don't evaluate teammates
		if(eOtherTeam == eTeam)
		{
			continue;
		}

		// don't evaluate dead people
		if(!GET_PLAYER(eOtherPlayer).isAlive())
		{
			continue;
		}

		// don't evaluate players we haven't met
		if(!GET_TEAM(eTeam).isHasMet(eOtherTeam))
		{
			continue;
		}

		WarProjectionTypes eWarProjection = pDiplomacyAI->GetWarProjection(eOtherPlayer);
		StrengthTypes eMilitaryStrengthComparedToUs = pDiplomacyAI->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer);

		if(GET_TEAM(eTeam).isAtWar(eOtherTeam))
		{
			switch(eWarProjection)
			{
			case WAR_PROJECTION_DESTRUCTION:
			{
				//if (GET_TEAM(eTeam).isAtWar(eOtherTeam))
				//{
				int iRating = 99;
				// warn player to back out!
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_DESTRUCTION_WAR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
				//}
				//else
				//{
				//	int iRating = 50;
				//	// warn player to avoid
				//	if (iRating > iMessageRating)
				//	{
				//		iMessageRating = iRating;
				//		eAdvisor = ADVISOR_FOREIGN;
				//		strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_DESTRUCTION_PEACE");
				//		strLoc << GC.getCivilizationInfo(GET_PLAYER(eOtherPlayer).getCivilizationType())->GetTextKey();
				//	}
				//}
			}
			break;
			case WAR_PROJECTION_DEFEAT:
			{
				//if (GET_TEAM(eTeam).isAtWar(eOtherTeam))
				//{

				// warn player to back out!
				int iRating = 98;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_DEFEAT_WAR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}

				//}
				//else
				//{
				//	// warn player to avoid
				//	int iRating = 49;
				//	if (iRating > iMessageRating)
				//	{
				//		iMessageRating = iRating;
				//		eAdvisor = ADVISOR_MILITARY;
				//		strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_DEFEAT_PEACE");
				//		strLoc << GC.getCivilizationInfo(GET_PLAYER(eOtherPlayer).getCivilizationType())->GetTextKey();
				//	}
				//}
			}
			break;
			case WAR_PROJECTION_UNKNOWN:
			case WAR_PROJECTION_STALEMATE:
			{
				//if (GET_TEAM(eTeam).isAtWar(eOtherTeam))
				//{

				// even fight
				int iRating = 97;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_STALEMATE_WAR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}

				//}
				//else
				//{
				//	// no warning for other states
				//}
			}
			break;
			case WAR_PROJECTION_GOOD:
			{
				//if (GET_TEAM(eTeam).isAtWar(eOtherTeam))
				//{

				// things are going well!
				int iRating = 96;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_GOOD_WAR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}

				//}
				//else
				//{
				//	// we could rough them up
				//	int iRating = 48;
				//	if (iRating > iMessageRating)
				//	{
				//		iMessageRating = iRating;
				//		eAdvisor = ADVISOR_FOREIGN;
				//		strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_GOOD_PEACE");
				//		strLoc << GC.getCivilizationInfo(GET_PLAYER(eOtherPlayer).getCivilizationType())->GetTextKey();
				//	}
				//}
			}
			break;
			case WAR_PROJECTION_VERY_GOOD:
			{
				//if (GET_TEAM(eTeam).isAtWar(eOtherTeam))
				//{

				// things are going very well!
				int iRating = 97;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_VERY_GOOD_WAR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}

				//}
				//else
				//{
				//	// we could take these guys
				//	int iRating = 97;
				//	if (iRating > iMessageRating)
				//	{
				//		iMessageRating = iRating;
				//		eAdvisor = ADVISOR_FOREIGN;
				//		strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARPROJECTION_VERY_GOOD_PEACE");
				//		strLoc << GC.getCivilizationInfo(GET_PLAYER(eOtherPlayer).getCivilizationType())->GetTextKey();
				//	}
				//}
			}
			break;
			}
		}
		else // not at war
		{
			switch(eMilitaryStrengthComparedToUs)
			{
			case STRENGTH_PATHETIC:
			{
				int iRating = 60;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_PATHETIC");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case STRENGTH_WEAK:
			{
				int iRating = 50;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_WEAK");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case STRENGTH_POOR:
			{
				int iRating = 45;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_POOR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case STRENGTH_AVERAGE:
			{
				int iRating = 40;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_AVERAGE");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case STRENGTH_STRONG:
			{
				int iRating = 45;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_STRONG");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case STRENGTH_POWERFUL:
			{
				int iRating = 80;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_POWERFUL");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case STRENGTH_IMMENSE:
			{
				int iRating = 90;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_MILITARY;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_STRENGTH_COMPARED_TO_US_IMMENSE");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			}
		}

		// look at get military posture
		AggressivePostureTypes eMilitaryAggressivePostureTypes = pDiplomacyAI->GetMilitaryAggressivePosture(eOtherPlayer);
		if(!GET_TEAM(eTeam).isAtWar(eOtherTeam))
		{
			switch(eMilitaryAggressivePostureTypes)
			{
			case AGGRESSIVE_POSTURE_NONE:
				break;
			case AGGRESSIVE_POSTURE_LOW:
				break;
			case AGGRESSIVE_POSTURE_MEDIUM:
			{
				int iRating = 51;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_AGGRESSIVE_POSTURE_MEDIUM");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case AGGRESSIVE_POSTURE_HIGH:
			{
				int iRating = 75;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_AGGRESSIVE_POSTURE_HIGH");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case AGGRESSIVE_POSTURE_INCREDIBLE:
			{
				int iRating = 90;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MILITARY_AGGRESSIVE_POSTURE_INCREDIBLE");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			}
		}

		// look at expansion aggressive posture
		AggressivePostureTypes eExpansionAggressivePostureTypes = pDiplomacyAI->GetExpansionAggressivePosture(eOtherPlayer);
		if(!GET_TEAM(eTeam).isAtWar(eOtherTeam))
		{
			switch(eExpansionAggressivePostureTypes)
			{
			case AGGRESSIVE_POSTURE_NONE:
				break;
			case AGGRESSIVE_POSTURE_LOW:
				break;
			case AGGRESSIVE_POSTURE_MEDIUM:
			{
				int iRating = 41;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_EXPANSION_AGGRESSIVE_POSTURE_MEDIUM");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case AGGRESSIVE_POSTURE_HIGH:
			{
				int iRating = 65;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_EXPANSION_AGGRESSIVE_POSTURE_HIGH");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case AGGRESSIVE_POSTURE_INCREDIBLE:
			{
				int iRating = 80;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_EXPANSION_AGGRESSIVE_POSTURE_INCREDIBLE");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			}
		}

		ThreatTypes eWarmongerThreat = pDiplomacyAI->GetWarmongerThreat(eOtherPlayer);
		if(!GET_TEAM(eTeam).isAtWar(eOtherTeam))
		{
			switch(eWarmongerThreat)
			{
			case THREAT_NONE:
			{
				int iRating = 20;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARMONGER_THREAT_NONE");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case THREAT_MINOR:
			{
				int iRating = 25;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARMONGER_THREAT_MINOR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}

			break;
			case THREAT_MAJOR:
			{
				int iRating = 30;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARMONGER_THREAT_MAJOR");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case THREAT_SEVERE:
			{
				int iRating = 60;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARMONGER_THREAT_SEVERE");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			case THREAT_CRITICAL:
			{
				int iRating = 80;
				if(iRating > iMessageRating)
				{
					iMessageRating = iRating;
					eAdvisor = ADVISOR_FOREIGN;
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_WARMONGER_THREAT_CRITICAL");
					strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();
				}
			}
			break;
			}
		}

		if(eAdvisor != NO_ADVISOR_TYPE)
		{
			bool bSuccess = SetCounselEntry(uiCounselIndex, eAdvisor, strLoc.toUTF8(), iMessageRating);
			CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
			if(!bSuccess)
			{
				break;
			}
			uiCounselIndex++;
		}
	}

	// go through diplomatic list for competitions with other civs with minor civs
	for(int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; iPlayer++)
	{
		// reset the ratings per player
		int iMessageRating = 0;
		AdvisorTypes eAdvisor = NO_ADVISOR_TYPE;

		PlayerTypes eOtherPlayer = (PlayerTypes)iPlayer;
		TeamTypes eOtherTeam = GET_PLAYER(eOtherPlayer).getTeam();

		// don't evaluate yourself
		if(eOtherPlayer == ePlayer)
		{
			continue;
		}

		// don't evaluate teammates
		if(eOtherTeam == eTeam)
		{
			continue;
		}

		// don't evaluate dead people
		if(!GET_PLAYER(eOtherPlayer).isAlive())
		{
			continue;
		}

		// don't evaluate players we haven't met
		if(!GET_TEAM(eTeam).isHasMet(eOtherTeam))
		{
			continue;
		}

		if(GET_TEAM(eTeam).isAtWar(eOtherTeam))
		{
			continue;
		}

		DisputeLevelTypes eMinorCivDisputeLevel = pDiplomacyAI->GetMinorCivDisputeLevel(eOtherPlayer);
		switch(eMinorCivDisputeLevel)
		{

		case DISPUTE_LEVEL_NONE:
			break;

		case DISPUTE_LEVEL_WEAK:
		{
			iMessageRating = 20;

			DiploStatementTypes eStatement;
			int iMinorCiv = NO_PLAYER;
			pDiplomacyAI->DoMinorCivCompetitionStatement(eOtherPlayer, eStatement, iMinorCiv, true);

			if(iMinorCiv != NO_PLAYER)
			{
				eAdvisor = ADVISOR_FOREIGN;
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MINOR_CIV_DISPUTE_LEVEL_WEAK");

				strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();

				CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GET_PLAYER((PlayerTypes)iMinorCiv).GetMinorCivAI()->GetMinorCivType());
				if(pkMinorCivInfo != NULL)
					strLoc << pkMinorCivInfo->GetTextKey();
			}
		}
		break;
		case DISPUTE_LEVEL_STRONG:
		{
			iMessageRating = 30;
			DiploStatementTypes eStatement;
			int iMinorCiv = NO_PLAYER;
			pDiplomacyAI->DoMinorCivCompetitionStatement(eOtherPlayer, eStatement, iMinorCiv, true);

			if(iMinorCiv != NO_PLAYER)
			{
				eAdvisor = ADVISOR_FOREIGN;
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MINOR_CIV_DISPUTE_LEVEL_STRONG");

				strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();

				CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GET_PLAYER((PlayerTypes)iMinorCiv).GetMinorCivAI()->GetMinorCivType());
				if(pkMinorCivInfo != NULL)
					strLoc << pkMinorCivInfo->GetTextKey();
			}
		}
		break;
		case DISPUTE_LEVEL_FIERCE:
		{
			iMessageRating = 40;

			DiploStatementTypes eStatement;
			int iMinorCiv = NO_PLAYER;
			pDiplomacyAI->DoMinorCivCompetitionStatement(eOtherPlayer, eStatement, iMinorCiv, true);

			if(iMinorCiv != NO_PLAYER)
			{
				eAdvisor = ADVISOR_FOREIGN;
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_MINOR_CIV_DISPUTE_LEVEL_FIERCE");

				strLoc << GET_PLAYER(eOtherPlayer).getCivilizationInfo().GetTextKey();

				CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GET_PLAYER((PlayerTypes)iMinorCiv).GetMinorCivAI()->GetMinorCivType());
				if(pkMinorCivInfo != NULL)
					strLoc << pkMinorCivInfo->GetTextKey();
			}
		}
		break;
		}

		if(eAdvisor != NO_ADVISOR_TYPE)
		{
			bool bSuccess = SetCounselEntry(uiCounselIndex, eAdvisor, strLoc.toUTF8(), iMessageRating);
			CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
			if(!bSuccess)
			{
				break;
			}
			uiCounselIndex++;
		}
	}

	// evaluate our gold situation
	{
		int iMessageRating = 0;
		AdvisorTypes eAdvisor = ADVISOR_ECONOMIC;
		CvTreasury* pTreasury = GET_PLAYER(ePlayer).GetTreasury();

		int iGoldIncome = GET_PLAYER(ePlayer).calculateGoldRate();
		int iGold = pTreasury->GetGold();

		int iHighestCost = 0;
		enum
		{
		    COST_UNIT_MAINTENANCE,
		    COST_UNIT_SUPPLY,
		    COST_BUILDING_MAINTENANCE,
		    COST_IMPROVEMENT_MAINTENANCE,
		    COST_DIPLOMACY,
		    NUM_COSTS
		};
		int uiHighestCostCategory = NUM_COSTS;

		for(uint ui = 0; ui < NUM_COSTS; ui++)
		{
			switch(ui)
			{
			case COST_UNIT_MAINTENANCE:
				if(pTreasury->GetExpensePerTurnUnitMaintenance() > iHighestCost)
				{
					uiHighestCostCategory = ui;
					iHighestCost = pTreasury->GetExpensePerTurnUnitMaintenance();
				}
				break;
			case COST_UNIT_SUPPLY:
				if(pTreasury->GetExpensePerTurnUnitSupply() > iHighestCost)
				{
					uiHighestCostCategory = ui;
					iHighestCost = pTreasury->GetExpensePerTurnUnitSupply();
				}
				break;
			case COST_BUILDING_MAINTENANCE:
				if(pTreasury->GetBuildingGoldMaintenance() > iHighestCost)
				{
					uiHighestCostCategory = ui;
					iHighestCost = pTreasury->GetBuildingGoldMaintenance();
				}
				break;
			case COST_IMPROVEMENT_MAINTENANCE:
				if(pTreasury->GetImprovementGoldMaintenance() > iHighestCost)
				{
					uiHighestCostCategory = ui;
					iHighestCost = pTreasury->GetBuildingGoldMaintenance();
				}
				break;
			case COST_DIPLOMACY:
				if(pTreasury->GetGoldPerTurnFromDiplomacy() < -(iHighestCost))
				{
					uiHighestCostCategory = ui;
					iHighestCost = -(pTreasury->GetGoldPerTurnFromDiplomacy());
				}
				break;
			}
		}


		if(iGold <= 0)  // we're bankrupt
		{
			if(iGoldIncome <= 0)
			{
				//    if our income is negative or zero
				iMessageRating = 99;
				switch(uiHighestCostCategory)
				{
				case COST_UNIT_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_DOWN_UNIT_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_UNIT_SUPPLY:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_DOWN_UNIT_SUPPLY");
					strLoc << iHighestCost;
					break;
				case COST_BUILDING_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_DOWN_BUILDING_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_IMPROVEMENT_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_DOWN_DIPLOMACY");
					strLoc << iHighestCost;
					break;
				case COST_DIPLOMACY:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_DOWN_IMPROVEMENT_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				default:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_DOWN");
					break;
				}
			}
			else
			{
				iMessageRating = 95;
				switch(uiHighestCostCategory)
				{
				case COST_UNIT_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_UP_UNIT_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_UNIT_SUPPLY:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_UP_UNIT_SUPPLY");
					strLoc << iHighestCost;
					break;
				case COST_BUILDING_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_UP_BUILDING_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_IMPROVEMENT_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_UP_IMPROVEMENT_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_DIPLOMACY:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_UP_DIPLOMACY");
					strLoc << iHighestCost;
					break;
				default:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_BROKE_AND_GOING_UP");
					break;
				}
			}
		}
		else // we're not bankrupt!
		{
			if(iGoldIncome <= 0)
			{
				// we're going bankrupt
				if(iGoldIncome < 0 && iGold / abs(iGoldIncome) <= 5)
				{
					iMessageRating = 80;
					switch(uiHighestCostCategory)
					{
					case COST_UNIT_MAINTENANCE:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_GOING_BROKE_UNIT_MAINTENANCE");
						strLoc << iHighestCost;
						break;
					case COST_UNIT_SUPPLY:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_GOING_BROKE_UNIT_SUPPLY");
						strLoc << iHighestCost;
						break;
					case COST_BUILDING_MAINTENANCE:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_GOING_BROKE_BUILDING_MAINTENANCE");
						strLoc << iHighestCost;
						break;
					case COST_IMPROVEMENT_MAINTENANCE:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_GOING_BROKE_IMPROVEMENT_MAINTENANCE");
						strLoc << iHighestCost;
						break;
					case COST_DIPLOMACY:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_GOING_BROKE_DIPLOMACY");
						strLoc << iHighestCost;
						break;
					default:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_GOING_BROKE");
						break;
					}
				}
				else
				{
					iMessageRating = 75;
					switch(uiHighestCostCategory)
					{
					case COST_UNIT_MAINTENANCE:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_LOSING_MONEY_UNIT_MAINTENANCE");
						strLoc << iHighestCost;
						break;
					case COST_UNIT_SUPPLY:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_LOSING_MONEY_UNIT_SUPPLY");
						strLoc << iHighestCost;
						break;
					case COST_BUILDING_MAINTENANCE:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_LOSING_MONEY_BUILDING_MAINTENANCE");
						strLoc << iHighestCost;
						break;
					case COST_IMPROVEMENT_MAINTENANCE:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_LOSING_MONEY_IMPROVEMENT_MAINTENANCE");
						strLoc << iHighestCost;
						break;
					case COST_DIPLOMACY:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_LOSING_MONEY_DIPLOMACY");
						strLoc << iHighestCost;
						break;
					default:
						strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_LOSING_MONEY");
						break;
					}
				}
			}
			else if(iHighestCost > 0)
			{
				iMessageRating = 20;
				switch(uiHighestCostCategory)
				{
				case COST_UNIT_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_MAKING_MONEY_UNIT_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_UNIT_SUPPLY:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_MAKING_MONEY_UNIT_SUPPLY");
					strLoc << iHighestCost;
					break;
				case COST_BUILDING_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_MAKING_MONEY_BUILDING_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_IMPROVEMENT_MAINTENANCE:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_MAKING_MONEY_IMPROVEMENT_MAINTENANCE");
					strLoc << iHighestCost;
					break;
				case COST_DIPLOMACY:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_MAKING_MONEY_DIPLOMACY");
					strLoc << iHighestCost;
					break;
				default:
					strLoc = Localization::Lookup("TXT_KEY_FINANCESTRATEGY_MAKING_MONEY");
					break;
				}

			}
		}

		if(iHighestCost > 0)
		{
			bool bSuccess = SetCounselEntry(uiCounselIndex, eAdvisor, strLoc.toUTF8(), iMessageRating);
			DEBUG_VARIABLE(bSuccess);
			CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
			uiCounselIndex++;
		}
	}

	// finding minor civs that we are about to lose a friendship/ally status with
	for(uint ui = MAX_MAJOR_CIVS; ui < MAX_CIV_PLAYERS; ui++)
	{
		int iMessageRating = 0;
		AdvisorTypes eAdvisor = ADVISOR_FOREIGN;
		//MinorCivQuestTypes eQuest = NO_MINOR_CIV_QUEST_TYPE;
		PlayerTypes eEnemyPlayer = NO_PLAYER;

		PlayerTypes eMinorPlayer = (PlayerTypes)ui;

		TeamTypes eMinorTeam = GET_PLAYER(eMinorPlayer).getTeam();

		// don't evaluate players we haven't met
		if(!GET_TEAM(eTeam).isHasMet(eMinorTeam))
		{
			continue;
		}

		if(GET_TEAM(eTeam).isAtWar(eMinorTeam))
		{
			continue;
		}

		if(!GET_PLAYER(eMinorPlayer).isAlive())
		{
			if(!GET_PLAYER(eMinorPlayer).isEverAlive())
			{
				continue;
			}

			// see if we ganked their city
			PlayerTypes eCapturingPlayer = NO_PLAYER;

			for(uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
			{
				if(eCapturingPlayer != NO_PLAYER)
				{
					break;
				}

				PlayerTypes eCheckPlayer = (PlayerTypes)ui2;

				int iCityIndex;
				CvCity* pCapCity;
				for(pCapCity = GET_PLAYER(eCheckPlayer).firstCity(&iCityIndex); pCapCity != NULL; pCapCity = GET_PLAYER(eCheckPlayer).nextCity(&iCityIndex))
				{
					if(pCapCity->getOriginalOwner() == eMinorPlayer)
					{
						eCapturingPlayer = pCapCity->getOwner();
						break;
					}
				}
			}

			//antonjs: todo: don't show counsel to liberate if the CS was taken over peacefully with buyout (Austria UA)
			// don't mention it if we conquered the city-state
			if(eCapturingPlayer != NO_PLAYER && eCapturingPlayer != ePlayer)
			{
				iMessageRating = 10;
				CvMinorCivAI* pMinorCivAI = GET_PLAYER(eMinorPlayer).GetMinorCivAI();

				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_LIBERATE_CITY_STATE");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
				strLoc << GET_PLAYER(eCapturingPlayer).getCivilizationInfo().GetTextKey();
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();

				bool bSuccess = SetCounselEntry(uiCounselIndex, eAdvisor, strLoc.toUTF8(), iMessageRating);
				CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
				if(!bSuccess)
				{
					break;
				}
				uiCounselIndex++;
			}

			continue;
		}

		CvMinorCivAI* pMinorCivAI = GET_PLAYER(eMinorPlayer).GetMinorCivAI();


		if(pMinorCivAI->IsAllies(ePlayer) && pMinorCivAI->IsCloseToNotBeingAllies(ePlayer))
		{
			// warn the player they are about to not be an ally with a minor civ
			iMessageRating = 80;
			if(eEnemyPlayer != NO_PLAYER)
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_WAR_WITH_MAJOR");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
				strLoc << GET_PLAYER(eEnemyPlayer).getCivilizationInfo().GetTextKey();
			}
			/*
			else if (eQuest != NO_MINOR_CIV_QUEST_TYPE)
			{
				int iQuestData = pMinorCivAI->GetQuestData1(ePlayer, eQuest);

				switch (eQuest)
				{
				case MINOR_CIV_QUEST_ROUTE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_ROUTE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_KILL_CAMP:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_KILL_CAMP");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_CONNECT_RESOURCE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_CONNECT_RESOURCE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getResourceInfo((ResourceTypes)iQuestData)->GetTextKey();
					break;
				case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_CONSTRUCT_WONDER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getBuildingInfo((BuildingTypes)iQuestData)->GetDescription();
					break;
				case MINOR_CIV_QUEST_GREAT_PERSON:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_GREAT_PERSON");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getUnitInfo((UnitTypes)iQuestData)->GetDescription();
					break;
				case MINOR_CIV_QUEST_KILL_CITY_STATE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_KILL_CITY_STATE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getMinorCivInfo(GET_PLAYER((PlayerTypes)iQuestData).GetMinorCivAI()->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_FIND_PLAYER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_FIND_PLAYER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GET_PLAYER((PlayerTypes)iQuestData).getCivilizationInfo().GetTextKey();
					break;
				case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_FIND_NATURAL_WONDER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				default:
					CvAssertMsg(false, "No entry for this quest type!");
					break;
				}
			}
			else if (pMinorCivAI->GetTurnsSinceThreatenedByBarbarians() >= 0)
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY_QUEST_CLEAR_BARBARIANS");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
			}
			*/
			else
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_ALLY");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
			}
		}
		else if(pMinorCivAI->IsFriends(ePlayer) && pMinorCivAI->IsCloseToNotBeingFriends(ePlayer))
		{
			// warn the player they are about to not be an friend with a minor civ
			iMessageRating = 60;
			if(eEnemyPlayer != NO_PLAYER)
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_WAR_WITH_MAJOR");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
				strLoc << GET_PLAYER(eEnemyPlayer).getCivilizationInfo().GetTextKey();
			}
			/*
			else if (eQuest != NO_MINOR_CIV_QUEST_TYPE)
			{
				int iQuestData = pMinorCivAI->GetQuestData1(ePlayer, eQuest);

				switch (eQuest)
				{
				case MINOR_CIV_QUEST_ROUTE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_ROUTE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_KILL_CAMP:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_KILL_CAMP");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_CONNECT_RESOURCE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_CONNECT_RESOURCE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getResourceInfo((ResourceTypes)iQuestData)->GetTextKey();
					break;
				case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_CONSTRUCT_WONDER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getBuildingInfo((BuildingTypes)iQuestData)->GetDescription();
					break;
				case MINOR_CIV_QUEST_GREAT_PERSON:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_GREAT_PERSON");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getUnitInfo((UnitTypes)iQuestData)->GetDescription();
					break;
				case MINOR_CIV_QUEST_KILL_CITY_STATE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_KILL_CITY_STATE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getMinorCivInfo(GET_PLAYER((PlayerTypes)iQuestData).GetMinorCivAI()->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_FIND_PLAYER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_FIND_PLAYER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GET_PLAYER((PlayerTypes)iQuestData).getCivilizationInfo().GetTextKey();
					break;
				case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_FIND_NATURAL_WONDER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				default:
					CvAssertMsg(false, "No entry for this quest type!");
					break;
				}
			}
			else if (pMinorCivAI->GetTurnsSinceThreatenedByBarbarians() >= 0)
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP_QUEST_CLEAR_BARBARIANS");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
			}
			*/
			else
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_ALMOST_LOST_CITY_STATE_FRIENDSHIP");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
			}
		}
		else
		{
			iMessageRating = 20;
			if(eEnemyPlayer != NO_PLAYER)
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_WAR_WITH_MAJOR");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
				strLoc << GET_PLAYER(eEnemyPlayer).getCivilizationInfo().GetTextKey();
			}
			/*
			else if (eQuest != NO_MINOR_CIV_QUEST_TYPE)
			{
				int iQuestData = pMinorCivAI->GetQuestData1(ePlayer, eQuest);

				switch (eQuest)
				{
				case MINOR_CIV_QUEST_ROUTE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_ROUTE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_KILL_CAMP:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_KILL_CAMP");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_CONNECT_RESOURCE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_CONNECT_RESOURCE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getResourceInfo((ResourceTypes)iQuestData)->GetTextKey();
					break;
				case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_CONSTRUCT_WONDER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getBuildingInfo((BuildingTypes)iQuestData)->GetDescription();
					break;
				case MINOR_CIV_QUEST_GREAT_PERSON:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_GREAT_PERSON");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getUnitInfo((UnitTypes)iQuestData)->GetDescription();
					break;
				case MINOR_CIV_QUEST_KILL_CITY_STATE:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_KILL_CITY_STATE");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GC.getMinorCivInfo(GET_PLAYER((PlayerTypes)iQuestData).GetMinorCivAI()->GetMinorCivType())->GetTextKey();
					break;
				case MINOR_CIV_QUEST_FIND_PLAYER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_FIND_PLAYER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					strLoc << GET_PLAYER((PlayerTypes)iQuestData).getCivilizationInfo().GetTextKey();
					break;
				case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
					strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_FIND_NATURAL_WONDER");
					strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
					break;
				default:
					CvAssertMsg(false, "No entry for this quest type!");
					break;
				}
			}
			else if (pMinorCivAI->GetTurnsSinceThreatenedByBarbarians() >= 0)
			{
				strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE_QUEST_CLEAR_BARBARIANS");
				strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
			}
			*/
			else
			{
				iMessageRating = 0;
			}
			//else
			//{
			//	strLoc = Localization::Lookup("TXT_KEY_DIPLOSTRATEGY_BEFRIEND_CITY_STATE");
			//	strLoc << GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType())->GetTextKey();
			//}
		}

		if(iMessageRating > 0)
		{
			bool bSuccess = SetCounselEntry(uiCounselIndex, eAdvisor, strLoc.toUTF8(), iMessageRating);
			CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
			if(!bSuccess)
			{
				break;
			}
			uiCounselIndex++;
		}
	}

	// find the biggest city
	CvCity* pBiggestCity = NULL;
	int iMaxPopulation = 0;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if(pLoopCity->getPopulation() > iMaxPopulation)
		{
			pBiggestCity = pLoopCity;
			iMaxPopulation = pBiggestCity->getPopulation();
		}
	}

	if(pBiggestCity)
	{
		CvString strBuildingClass;
		for(uint uiLoop = 0; uiLoop < 4; uiLoop++)
		{
			// for every science improvement building
			switch(uiLoop)
			{
			case 0:
				strBuildingClass = "BUILDINGCLASS_LIBRARY";
				break;
			case 1:
				strBuildingClass = "BUILDINGCLASS_UNIVERSITY";
				break;
			case 2:
				strBuildingClass = "BUILDINGCLASS_PUBLIC_SCHOOL";
				break;
			case 3:
				strBuildingClass = "BUILDINGCLASS_LABORATORY";
				break;
			}

			BuildingClassTypes eScienceBuildingClass = NO_BUILDINGCLASS;
			for(int i = 0; i < GC.getNumBuildingClassInfos(); i++)
			{
				BuildingClassTypes eBuildingClass = (BuildingClassTypes)i;

				CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
				if(!pkBuildingClassInfo)
				{
					continue;
				}

				if(GC.getBuildingClassInfo(eBuildingClass)->GetType() == strBuildingClass)
				{
					eScienceBuildingClass = eBuildingClass;
					break;
				}
			}

			if(eScienceBuildingClass == NO_BUILDINGCLASS)
			{
				continue;
			}

			BuildingTypes eRecommendedResearchBuilding = NO_BUILDING;
			for(int i = 0; i < GC.getNumBuildingInfos(); i++)
			{
				BuildingTypes eBuildingType = (BuildingTypes)i;
				CvBuildingEntry* pkEntry = GC.getBuildingInfo(eBuildingType);
				if(!pkEntry || pkEntry->GetBuildingClassType() != eScienceBuildingClass)
				{
					continue;
				}

				if(pBiggestCity->canConstruct(eBuildingType))
				{
					eRecommendedResearchBuilding = eBuildingType;
					break;
				}
			}

			if(eRecommendedResearchBuilding != NO_BUILDING)
			{

				strLoc = Localization::Lookup("TXT_KEY_SCIENCESTRATEGY_RESEARCH_BUILDING_IN_BIG_CITY");
				strLoc << pBiggestCity->getNameKey();
				strLoc << GC.getBuildingInfo(eRecommendedResearchBuilding)->GetTextKey();
				strLoc << GC.getBuildingInfo(eRecommendedResearchBuilding)->GetTextKey();

				bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_SCIENCE, strLoc.toUTF8(), 20);
				CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
				if(!bSuccess)
				{
					break;
				}
				uiCounselIndex++;

				break;
			}
		}
	}

	CvPlotsVector& aiPlots = GET_PLAYER(ePlayer).GetPlots();
	ResourceTypes eRecommendedResource = NO_RESOURCE;
	CvPlot* pResourcePlot = NULL;
	int iNumOfResource = MAX_INT;
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{
		CvPlot* pPlot = NULL;
		// when we encounter the first plot that is invalid, the rest of the list will be invalid
		if(aiPlots[ui] == -1)
		{
			break;
		}

		pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if(!pPlot)
		{
			continue;
		}

		ResourceTypes eResource = pPlot->getResourceType(GET_PLAYER(ePlayer).getTeam());
		if(eResource == NO_RESOURCE)
		{
			continue;
		}

		if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY || GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
		{
			// if we don't have any of this resource
			if(GET_PLAYER(ePlayer).getNumResourceTotal(eResource) < iNumOfResource)
			{
				for(int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
				{
					BuildTypes eBuild = (BuildTypes)iBuild;
					CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
					if(pkBuild == NULL)
					{
						continue;
					}

					ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
					if(eImprovement == NO_IMPROVEMENT)
					{
						continue;
					}

					if(!GC.getImprovementInfo(eImprovement)->IsImprovementResourceTrade(eResource))
					{
						continue;
					}

					if(GET_PLAYER(ePlayer).canBuild(pPlot, eBuild))
					{
						// ensure there is a city working this tile
						if(pPlot->getWorkingCity())
						{
							// check for a worker in this plot
							bool bWorkerAlreadyOnIt = false;
							IDInfo* pPlotUnitNode = pPlot->headUnitNode();

							while(pPlotUnitNode != NULL)
							{
								CvUnit* pLoopUnit = ::getUnit(*pPlotUnitNode);
								pPlotUnitNode = pPlot->nextUnitNode(pPlotUnitNode);

								if(pLoopUnit->getOwner() != ePlayer)
								{
									continue;
								}

								// walk through missions
								int iNumMissions = pLoopUnit->GetLengthMissionQueue();
								for(int i = 0; i < iNumMissions; i++)
								{
									const MissionData* pMission = pLoopUnit->GetMissionData(i);
									if(pMission->eMissionType == CvTypes::getMISSION_BUILD() && pMission->iData1 == eBuild)
									{
										bWorkerAlreadyOnIt = true;
										break;
									}

								}

								if(bWorkerAlreadyOnIt)
								{
									break;
								}
							}

							if(!bWorkerAlreadyOnIt)
							{
								eRecommendedResource = eResource;
								pResourcePlot = pPlot;
								iNumOfResource = GET_PLAYER(ePlayer).getNumResourceTotal(eResource);
							}
						}
					}
				}
			}
		}
	}

	if(eRecommendedResource != NO_RESOURCE)
	{
		if(iNumOfResource == 0)
		{
			strLoc = Localization::Lookup("TXT_KEY_CITYIMPROVEMENTSTRATEGY_PLUG_UP_RESOURCES_HAVE_ZERO");
		}
		else
		{
			strLoc = Localization::Lookup("TXT_KEY_CITYIMPROVEMENTSTRATEGY_PLUG_UP_RESOURCES_HAVE_MORE");
		}
		strLoc << GC.getResourceInfo(eRecommendedResource)->GetTextKey();
		strLoc << pResourcePlot->getWorkingCity()->getNameKey();

		bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_ECONOMIC, strLoc.toUTF8(), 20);
		DEBUG_VARIABLE(bSuccess);
		CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
		uiCounselIndex++;
	}

	ResourceTypes eTradableResource = NO_RESOURCE;
	PlayerTypes eTargetPlayer = NO_PLAYER;
	TeamTypes ePlayerTeam = GET_PLAYER(ePlayer).getTeam();
	for(int i = 0; i < GC.getNumResourceInfos(); i++)
	{
		ResourceTypes eResource = (ResourceTypes)i;
		CvResourceInfo* pResourceInfo = GC.getResourceInfo(eResource);
		if (pResourceInfo == NULL)
		{
			continue;
		}

		if(pResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY && pResourceInfo->getResourceUsage() != RESOURCEUSAGE_STRATEGIC)
		{
			continue;
		}

		// not enough to trade
		if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource, false) <= 1)
		{
			continue;
		}

		for(int j = 0; j < MAX_MAJOR_CIVS; j++)
		{
			PlayerTypes eOtherPlayer = (PlayerTypes)j;
			TeamTypes eOtherTeam = GET_PLAYER(eOtherPlayer).getTeam();
			if(ePlayerTeam == eOtherTeam || !GET_PLAYER(eOtherPlayer).isAlive())
			{
				continue;
			}

			// if we haven't met or are at war
			if(!GET_TEAM(ePlayerTeam).isHasMet(eOtherTeam) || GET_TEAM(ePlayerTeam).isAtWar(eOtherTeam))
			{
				continue;
			}

			// other player already has some
			if(GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, true) != 0)
			{
				continue;
			}

			for(int k = 0; k < GC.getNumResourceInfos(); k++)
			{
				ResourceTypes eTradebackResource = (ResourceTypes)k;
				const CvResourceInfo* pkTradeResourceInfo = GC.getResourceInfo(eTradebackResource);
				if(pkTradeResourceInfo == NULL || (pkTradeResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY && pkTradeResourceInfo->getResourceUsage() != RESOURCEUSAGE_STRATEGIC))
				{
					continue;
				}

				// other player has some to trade
				if(GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eTradebackResource, false) == 0)
				{
					continue;
				}

				if(GET_PLAYER(ePlayer).getNumResourceAvailable(eTradebackResource, true) > 0)
				{
					continue;
				}

				if(eTradableResource != NO_RESOURCE)
				{
					ResourceUsageTypes eTradableResourceUsage = GC.getResourceInfo(eTradableResource)->getResourceUsage();
					ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eResource)->getResourceUsage();

					// prefer trading luxury to strategic
					if(eTradableResourceUsage == RESOURCEUSAGE_STRATEGIC && eResourceUsage == RESOURCEUSAGE_LUXURY)
					{
						eTradableResource = eResource;
						eTargetPlayer = eOtherPlayer;
					}
				}
				else
				{
					eTradableResource = eResource;
					eTargetPlayer = eOtherPlayer;
				}
			}
		}
	}

	if(eTradableResource != NO_RESOURCE)
	{
		ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eTradableResource)->getResourceUsage();
		if(eResourceUsage == RESOURCEUSAGE_LUXURY)
		{
			strLoc = Localization::Lookup("TXT_KEY_RESOURCESTRATEGY_TRADE_LUXURIES");
		}
		else if(eResourceUsage == RESOURCEUSAGE_STRATEGIC)
		{
			strLoc = Localization::Lookup("TXT_KEY_RESOURCESTRATEGY_TRADE_STRATEGIC");
		}
		else
		{
			CvAssertMsg(false, "Can't trade a bonus resource");
		}

		strLoc << GC.getResourceInfo(eTradableResource)->GetTextKey();
		strLoc << GET_PLAYER(eTargetPlayer).getCivilizationInfo().GetTextKey();

		bool bSuccess = SetCounselEntry(uiCounselIndex, ADVISOR_FOREIGN, strLoc.toUTF8(), 20);
		DEBUG_VARIABLE(bSuccess);
		CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
		uiCounselIndex++;
	}

	// search for each advisor type. If there isn't an entry of that type, add the default string
	bool abAdvisorUsed[NUM_ADVISOR_TYPES];
	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		abAdvisorUsed[ui] = false;
	}

	for(uint ui = 0; ui < m_aCounsel.size(); ui++)
	{
		if(m_aCounsel[ui].m_eAdvisor == NO_ADVISOR_TYPE || m_aCounsel[ui].m_eAdvisor == NUM_ADVISOR_TYPES)
		{
			continue;
		}
		abAdvisorUsed[m_aCounsel[ui].m_eAdvisor] = true;
	}

	for(uint ui = 0; ui < NUM_ADVISOR_TYPES; ui++)
	{
		if(!abAdvisorUsed[ui])
		{
			AdvisorTypes eAdvisorTypes = (AdvisorTypes)ui;
			switch(eAdvisorTypes)
			{
			case ADVISOR_SCIENCE:
				strLoc = Localization::Lookup("TXT_KEY_BASESTRATEGY_NOTHING_TO_SAY_SCIENCE");
				break;
			case ADVISOR_ECONOMIC:
				strLoc = Localization::Lookup("TXT_KEY_BASESTRATEGY_NOTHING_TO_SAY_ECONOMIC");
				break;
			case ADVISOR_MILITARY:
				strLoc = Localization::Lookup("TXT_KEY_BASESTRATEGY_NOTHING_TO_SAY_MILITARY");
				break;
			case ADVISOR_FOREIGN:
				strLoc = Localization::Lookup("TXT_KEY_BASESTRATEGY_NOTHING_TO_SAY_FOREIGN");
				break;
			}

			bool bSuccess = SetCounselEntry(uiCounselIndex, eAdvisorTypes, strLoc.toUTF8(), 0);
			DEBUG_VARIABLE(bSuccess);
			CvAssertMsg(bSuccess, "Unable to add counsel to list. Too many strategies running at once");
			uiCounselIndex++;
		}
	}

	// sort the list
	std::sort(&m_aCounsel[0], &m_aCounsel[0] + m_aCounsel.size(), CounselSort);
}

bool CvAdvisorCounsel::SetCounselEntry(uint uiIndex, AdvisorTypes eAdvisor, const char* strAdvisorCounsel, int iValue)
{
	if(uiIndex >= m_aCounsel.size())
	{
		return false;
	}

	m_aCounsel[uiIndex].m_eAdvisor = eAdvisor;
	strcpy_s(m_aCounsel[uiIndex].m_strTxtKey, strAdvisorCounsel);
	m_aCounsel[uiIndex].m_iValue = iValue;

	return true;
}
