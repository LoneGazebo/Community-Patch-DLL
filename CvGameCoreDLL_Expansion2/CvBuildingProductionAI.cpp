/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvBuildingProductionAI.h"
#include "CvInfosSerializationHelper.h"
#if defined(MOD_BALANCE_CORE)
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvCitySpecializationAI.h"
#include "CvEconomicAI.h"
#include "CvGrandStrategyAI.h"
#include "CvMilitaryAI.h"
#endif
// include after all other headers
#include "LintFree.h"

/// Constructor
CvBuildingProductionAI::CvBuildingProductionAI(CvCity* m_pCity, CvCityBuildings* pCityBuildings):
	m_pCity(m_pCity),
	m_pCityBuildings(pCityBuildings)
{
}

/// Destructor
CvBuildingProductionAI::~CvBuildingProductionAI(void)
{
}

/// Clear out AI local variables
void CvBuildingProductionAI::Reset()
{
	CvAssertMsg(m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");

	m_BuildingAIWeights.clear();

	// Loop through reading each one and add an entry with 0 weight to our vector
	if(m_pCityBuildings)
	{
		for(int i = 0; i < m_pCityBuildings->GetPossibleBuildings()->GetNumBuildings(); i++)
		{
			m_BuildingAIWeights.push_back(i, 0);
		}
	}
}

/// Serialization read
void CvBuildingProductionAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	// Reset vector
	m_BuildingAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	if(m_pCityBuildings)
	{
		for(int i = 0; i < m_pCityBuildings->GetPossibleBuildings()->GetNumBuildings(); i++)
		{
			m_BuildingAIWeights.push_back(i, 0);
		}

		int iNumEntries;
		FStringFixedBuffer(sTemp, 64);
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
		{
			bool bValid = true;
			iType = CvInfosSerializationHelper::ReadHashed(kStream, &bValid);
			if(iType != -1 || !bValid)
			{
				int iWeight;
				kStream >> iWeight;
				if(iType != -1)
				{
					m_BuildingAIWeights.IncreaseWeight(iType, iWeight);
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Building Type not found");
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
		}
	}
	else
	{
		CvAssertMsg(m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");
	}
}

/// Serialization write
void CvBuildingProductionAI::Write(FDataStream& kStream)
{
	CvAssertMsg(m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	if(m_pCityBuildings)
	{
		int iNumBuildings = m_pCityBuildings->GetPossibleBuildings()->GetNumBuildings();
		kStream << iNumBuildings;

		// Loop through writing each entry
		for(int iI = 0; iI < iNumBuildings; iI++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				CvInfosSerializationHelper::WriteHashed(kStream, pkBuildingInfo);
				kStream << m_BuildingAIWeights.GetWeight(iI);
			}
			else
			{
				kStream << (int)0;
			}
		}
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvBuildingProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	if (iWeight==0)
		return;

	CvBuildingXMLEntries* pkBuildings = m_pCityBuildings->GetPossibleBuildings();

	// Loop through all buildings
	for(int iBuilding = 0; iBuilding < pkBuildings->GetNumBuildings(); iBuilding++)
	{
		CvBuildingEntry* entry = pkBuildings->GetEntry(iBuilding);
		if(entry)
		{
			// Set its weight by looking at building's weight for this flavor and using iWeight multiplier passed in
			m_BuildingAIWeights.IncreaseWeight(iBuilding, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}

/// Retrieve sum of weights on one item
int CvBuildingProductionAI::GetWeight(BuildingTypes eBuilding)
{
	return m_BuildingAIWeights.GetWeight(eBuilding);
}

/// Recommend highest-weighted building
BuildingTypes CvBuildingProductionAI::RecommendBuilding()
{
	int iBldgLoop;
	int iWeight;
	int iTurnsLeft;

	// Reset list of all the possible buildings
	m_Buildables.clear();

	// Loop through adding the available buildings
	for(iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		// Make sure this building can be built now
		if(m_pCity->canConstruct((BuildingTypes)iBldgLoop))
		{
			// Update weight based on turns to construct
			iTurnsLeft = m_pCity->getProductionTurnsLeft((BuildingTypes) iBldgLoop, 0);
			iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_BuildingAIWeights.GetWeight((BuildingTypes)iBldgLoop), iTurnsLeft);
			m_Buildables.push_back(iBldgLoop, iWeight);
		}
	}

	// Sort items and grab the first one
	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleBuilds();
		return (BuildingTypes)m_Buildables.GetElement(0);
	}

	// Unless we didn't find any
	else
	{
		return NO_BUILDING;
	}
}

/// Log all potential builds
void CvBuildingProductionAI::LogPossibleBuilds()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		// Find the name of this civ and city
		CvString playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		CvString cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		// Dump out the weight of each buildable item
		CvBuildingXMLEntries* pGameBuildings = GC.GetGameBuildings();
		if(pGameBuildings != NULL)
		{
			for(int iI = 0; iI < m_Buildables.size(); iI++)
			{
				CvBuildingEntry* pBuildingEntry = pGameBuildings->GetEntry(m_Buildables.GetElement(iI));;
				if(pBuildingEntry != NULL)
				{
					CvString strTemp;
					strTemp.Format("Building, %s, %d", pBuildingEntry->GetDescription(), m_Buildables.GetWeight(iI));
					CvString strOutBuf = strBaseString + strTemp;
					pLog->Msg(strOutBuf);
				}
			}
		}
	}
}
#if defined(MOD_BALANCE_CORE)
/// Do all building sanity stuff here.
int CvBuildingProductionAI::CheckBuildingBuildSanity(BuildingTypes eBuilding, int iValue, int iNumLandConnection, int iNumSeaConnection)
{
	if(m_pCity == NULL)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if(kPlayer.isMinorCiv())
		return iValue;

	if(eBuilding == NO_BUILDING)
		return 0;

	if(iValue == 0)
		return 0;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Skip if null
	if(pkBuildingInfo == NULL)
		return 0;

	const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
	bool bIsVenice = kPlayer.GetPlayerTraits()->IsNoAnnexing();
	if(m_pCity->IsPuppet() && !bIsVenice)
	{
		if(isWorldWonderClass(kBuildingClassInfo) || isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
		{
			return 0;
		}
	}
	else if(m_pCity->IsPuppet() && bIsVenice)
	{
		if(isWorldWonderClass(kBuildingClassInfo) || isTeamWonderClass(kBuildingClassInfo))
		{
			return 0;
		}
	}
	if(isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
	{
		//Should never be in first 3 things built.
		if(m_pCity->GetCityBuildings()->GetNumBuildings() <= 3)
		{
			return 0;
		}
	}
	
	//Let's not send uniques through this - they're generally good enough to spam.
	if((BuildingTypes)kPlayer.getCivilizationInfo().isCivilizationBuildingOverridden(eBuilding))
	{
		iValue *= 20;
		return iValue;
	}

	
	//Bonus % additive. All values below will be added to this and combined with real value at end.
	int iBonus = 0;


	////////////////
	////QUESTS
	////////////////

	//Minor Check
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
		if(eMinor != NO_PLAYER)
		{
			CvPlayer* pMinor = &GET_PLAYER(eMinor);
			if(pMinor)
			{
				CvMinorCivAI* pMinorCivAI = pMinor->GetMinorCivAI();
				if(pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(m_pCity->getOwner(), MINOR_CIV_QUEST_BUILD_X_BUILDINGS))
				{
					if((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_BUILD_X_BUILDINGS) == eBuilding)
					{
						iBonus += 50;
					}
					if((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER) == eBuilding)
					{
						iBonus += 50;
					}
					if((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_CONSTRUCT_WONDER) == eBuilding)
					{
						iBonus += 50;
					}
				}
			}
		}
	}


	////////////
	///GEOGRAPHY
	////////////
	CvArea* pArea = GC.getMap().getArea(m_pCity->getArea());
	if(pArea == NULL)
		return 0;

	//Airlift
	if(pkBuildingInfo->IsAirlift())
	{
		if(!m_pCity->isCapital() && kPlayer.getCapitalCity() != NULL)
		{
			if(m_pCity->getArea() != kPlayer.getCapitalCity()->getArea())	
			{
				iBonus += 25;
			}
		}
	}

	//Defense Needs
	int iDefense = 0;
	if(pkBuildingInfo->GetNukeModifier() > 0)
	{
		iDefense += (pkBuildingInfo->GetNukeModifier() / 2);
	}
	if(pkBuildingInfo->GetGlobalDefenseModifier() > 0)
	{
		iDefense += pkBuildingInfo->GetGlobalDefenseModifier();
	}
	if(pkBuildingInfo->GetDefenseModifier() > 0)
	{
		iDefense += (pkBuildingInfo->GetDefenseModifier() / 5);
	}
	if(pkBuildingInfo->GetExtraCityHitPoints() > 0)
	{
		iDefense += (pkBuildingInfo->GetExtraCityHitPoints() / 2);
	}
	if(m_pCity->plot()->isCoastalLand())
	{
		if(pkBuildingInfo->GetBorderObstacleWater() > 0)
		{
			iDefense += 50;
		}
	}
	else
	{
		if(pkBuildingInfo->GetBorderObstacleCity() > 0)
		{
			iDefense += 50;
		}
		if(pkBuildingInfo->IsBorderObstacle())
		{
			iDefense += 100;
		}
	}
	if(m_pCity->IsBastion())
	{
		iDefense *= 3;
	}
	else
	{
		iDefense /= 3;
	}
	iBonus += iDefense;

	//No Land trade connections?
	if(pkBuildingInfo->GetTradeRouteLandDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteLandGoldBonus() > 0)
	{	
		if(iNumLandConnection == 0)
		{
			return 0;
		}
		else
		{
			//Higher value the higher the number of routes.
			iBonus += (iNumLandConnection * 10);
			if(kPlayer.GetPlayerTraits()->GetLandTradeRouteRangeBonus() > 0)
			{
				iBonus += 25;
			}
		}
	}

	//No Sea Trade Connections?
	if(pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteSeaGoldBonus() > 0)
	{
		CvCity* pCapital = kPlayer.getCapitalCity();
		if(iNumSeaConnection == 0 && m_pCity->IsConnectedToCapital())
		{
			return 0;
		}
		else if(iNumSeaConnection == 0 && pCapital != NULL && pCapital->getArea() != m_pCity->getArea())
		{
			iBonus += 50;
		}
		else
		{
			//Higher value the higher the number of routes.
			iBonus += (iNumSeaConnection * 10);
			if(kPlayer.GetPlayerTraits()->GetSeaTradeRouteRangeBonus() > 0)
			{
				iBonus += 25;
			}
		}
	}

	if(pkBuildingInfo->IsAddsFreshWater() && m_pCity->plot()->isFreshWater())
	{
		iBonus += 50;
	}
	
	if(isWorldWonderClass(kBuildingClassInfo))
	{
		if(m_pCity->getPopulation() <= 8 && !m_pCity->isCapital())
		{
			iBonus -= 100;
		}
		else
		{
			iBonus += (kPlayer.GetPlayerTraits()->GetWonderProductionModifier() + kPlayer.getWonderProductionModifier());
		}
	}

	if(pkBuildingInfo->IsExtraLuxuries())
	{
		int iResource = 0;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if(pkResource && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				if(m_pCity->GetNumResourceLocal(eResource) > 0)
				{
					iResource++;
				}
			}
		}
		iResource *= 5;
		if(iResource <= 0)
		{
			return 0;
		}
		else
		{
			iBonus += iResource;
		}
	}

	///Free Resources
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource)
		{
			//Building uses resources? Not if we're a puppet, thanks!
			if(pkBuildingInfo->GetResourceQuantityRequirement(eResource) > 0 && m_pCity->IsPuppet())
			{
				return 0;
			}
			if(pkBuildingInfo->GetResourceQuantity(eResource) > 0)
			{
				//Decrease value based on # we own.
				int iNumOwned = kPlayer.getNumResourceAvailable(eResource, false);
				iBonus += (20 - iNumOwned);
			}
		}
	}

	
	/////////
	////Happiness (CBP)
	////////
	if(kPlayer.IsEmpireUnhappy())
	{
		if(pkBuildingInfo->GetHappiness() > 0 || pkBuildingInfo->GetHappinessPerCity() > 0 || pkBuildingInfo->GetHappinessPerXPolicies() > 0 || pkBuildingInfo->GetUnmoddedHappiness() > 0)
		{
			iBonus += kPlayer.GetUnhappiness();
		}
		int iNumBuildingInfos = GC.getNumBuildingInfos();
		for(int iI = 0; iI < iNumBuildingInfos; iI++)
		{
			const BuildingTypes eBuildingLoop = static_cast<BuildingTypes>(iI);
			if(eBuildingLoop == NO_BUILDING)
				continue;

			CvBuildingEntry* pkLoopBuilding = GC.getBuildingInfo(eBuildingLoop);
			if(pkLoopBuilding)
			{
				if(pkBuildingInfo->GetBuildingClassHappiness(pkLoopBuilding->GetBuildingClassType()) > 0)
				{
					iBonus += (kPlayer.getBuildingClassCount((BuildingClassTypes)pkLoopBuilding->GetBuildingClassType()) * 5);
				}
			}
		}
	}

	FlavorTypes eFlavorScience = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE");
	FlavorTypes eFlavorGold = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD");
	FlavorTypes eFlavorCulture = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE");
	FlavorTypes eFlavorDefense = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_CITY_DEFENSE");
	FlavorTypes eFlavorReligion = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION");

	bool bTested = false;
	//Only checking for buildings that matter, and only once per test (based on prioritization of city happiness logic.
	if((pkBuildingInfo->GetFlavorValue(eFlavorGold) > 0) || (pkBuildingInfo->GetPovertyHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetPovertyHappinessChangeBuildingGlobal() != 0))
	{
		int iUnhappyGold = m_pCity->getUnhappinessFromGold();
		if(iUnhappyGold > 0)
		{
			bTested = true;
			iBonus += (iUnhappyGold * 10);
		}
	}
	else if(!bTested && ((pkBuildingInfo->GetFlavorValue(eFlavorDefense) > 0) || (pkBuildingInfo->GetDefenseHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal() != 0)))
	{
		int iUnhappyDefense = m_pCity->getUnhappinessFromDefense();
		if(iUnhappyDefense > 0)
		{
			bTested = true;
			iBonus += (iUnhappyDefense * 10);
		}
	}
	else if(!bTested && ((pkBuildingInfo->GetFlavorValue(eFlavorReligion) > 0) || (pkBuildingInfo->GetMinorityHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetMinorityHappinessChangeBuildingGlobal() != 0)))
	{
		int iUnhappyReligion = m_pCity->getUnhappinessFromReligion();
		if(iUnhappyReligion > 0)
		{
			bTested = true;
			iBonus += (iUnhappyReligion * 10);
		}
	}
	else if(!bTested && ((pkBuildingInfo->GetFlavorValue(eFlavorCulture) > 0) || (pkBuildingInfo->GetUnculturedHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetUnculturedHappinessChangeBuildingGlobal() != 0)))
	{
		int iUnhappyCulture = m_pCity->getUnhappinessFromCulture();
		if(iUnhappyCulture > 0)
		{
			bTested = true;
			iBonus += (iUnhappyCulture * 10);
		}
	}
	else if(!bTested && ((pkBuildingInfo->GetFlavorValue(eFlavorScience) > 0) || (pkBuildingInfo->GetIlliteracyHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetIlliteracyHappinessChangeBuildingGlobal() != 0)))
	{
		int iUnhappyScience = m_pCity->getUnhappinessFromScience();
		if(iUnhappyScience > 0)
		{
			bTested = true;
			iBonus += (iUnhappyScience * 10);
		}
	}

	////////
	////Specific Concepts
	///////

#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	if(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	{
		//Virtually force this.
		const BuildingClassTypes eBuildingClass = (BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType());
		if(m_pCity->IsBuildingInvestment(eBuildingClass))
		{
			iBonus += 100;
		}
	}
#endif
	//Courthouse? Let's get it ASAP.
	if(pkBuildingInfo->IsNoOccupiedUnhappiness())
	{
		if(m_pCity->IsOccupied() && !m_pCity->IsNoOccupiedUnhappiness())
		{
			iBonus += 500;
		}
	}

	///////////////////
	//Military Stuff
	////////////////////////
	
	//Domain mods are good, and should be stacked.
	if(!m_pCity->IsPuppet() || bIsVenice)
	{
		for (int iDomainLoop = 0; iDomainLoop < NUM_DOMAIN_TYPES; iDomainLoop++)
		{
			DomainTypes eTestDomain = (DomainTypes)iDomainLoop;
			if(eTestDomain != NO_DOMAIN)
			{
				if(pkBuildingInfo->GetDomainFreeExperience(eTestDomain) > 0 || pkBuildingInfo->GetDomainFreeExperiencePerGreatWork(eTestDomain))
				{
					if(m_pCity->getDomainFreeExperience(eTestDomain) > 0)
					{
						iBonus += (m_pCity->getDomainFreeExperience(eTestDomain) +  pkBuildingInfo->GetDomainFreeExperiencePerGreatWork(eTestDomain)) * 2;
					}
					else
					{
						iBonus += m_pCity->getDomainFreeExperience(eTestDomain) +  pkBuildingInfo->GetDomainFreeExperiencePerGreatWork(eTestDomain);
					}
				}		
				if(pkBuildingInfo->GetDomainProductionModifier(eTestDomain) > 0)
				{
					if(m_pCity->getDomainProductionModifier(eTestDomain) > 0)
					{
						iBonus += m_pCity->getDomainProductionModifier(eTestDomain) * 2;
					}
					else
					{
						iBonus += m_pCity->getDomainProductionModifier(eTestDomain);
					}
				}
				if(pkBuildingInfo->GetDomainFreeExperiencePerGreatWorkGlobal(eTestDomain) > 0)
				{
					if(m_pCity->getDomainFreeExperience(eTestDomain) > 0)
					{
						iBonus += (m_pCity->getDomainFreeExperience(eTestDomain) +  pkBuildingInfo->GetDomainFreeExperiencePerGreatWorkGlobal(eTestDomain)) * 2;
					}
					else
					{
						iBonus += m_pCity->getDomainFreeExperience(eTestDomain) +  pkBuildingInfo->GetDomainFreeExperiencePerGreatWorkGlobal(eTestDomain);
					}
				}
			}
		}
	}
	//Unitcombat Bonuses should stack too.
	if(!m_pCity->IsPuppet() || bIsVenice)
	{
		for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatClassInfo = GC.getUnitCombatClassInfo(eUnitCombatClass);
			if(pkUnitCombatClassInfo)
			{
				if(pkBuildingInfo->GetUnitCombatProductionModifier(eUnitCombatClass) > 0)
				{
					iBonus += m_pCity->getUnitCombatProductionModifier(eUnitCombatClass);
				}
				if(pkBuildingInfo->GetUnitCombatFreeExperience(eUnitCombatClass) > 0)
				{
					iBonus += m_pCity->getUnitCombatFreeExperience(eUnitCombatClass);
				}
			}
		}
	}
	
	//////////////////////
	///BASE BONUSES
	/////////////////////
	int iBaseValue = CityStrategyAIHelpers::GetBuildingBasicValue(m_pCity, eBuilding);

	iBonus += iBaseValue;

	/////////////
	//Grand Strategies
	///////////////

	//Acts as modifier for value.
	int iGSValue = CityStrategyAIHelpers::GetBuildingGrandStrategyValue(m_pCity, eBuilding, kPlayer.GetID());

	iBonus += iGSValue;

	//////////////////////
	///POLICY BONUSES
	/////////////////////
	int iPolicyValue = CityStrategyAIHelpers::GetBuildingPolicyValue(m_pCity, eBuilding);

	iBonus += iPolicyValue;

	////////////
	///YIELD BONUSES
	//////////
	//Let's look at yield bonuses.
	int iYieldValue = 0;
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		const YieldTypes eYield = static_cast<YieldTypes>(iI);
		if(eYield == NO_YIELD)
			continue;

		iYieldValue = CityStrategyAIHelpers::GetBuildingYieldValue(m_pCity, eBuilding, eYield);

		int iYieldTrait = CityStrategyAIHelpers::GetBuildingTraitValue(m_pCity, eYield, eBuilding);

		//Help with poverty
		if(eYield == YIELD_GOLD)
		{
			int iGPT = (int)kPlayer.GetTreasury()->AverageIncome(10);
			if(iGPT < 0)
			{
				iYieldValue += (iGPT * -5);
			}
		}

		iBonus += iYieldValue;
		iBonus += iYieldTrait;
	}

	//////////////
	///Puppet Controls
	/////////////////////
	if(pkBuildingInfo->GetGoldMaintenance() > 0)
	{
		iBonus /= pkBuildingInfo->GetGoldMaintenance();
		if(m_pCity->IsPuppet())
		{
			iBonus /= max(2, (pkBuildingInfo->GetGoldMaintenance() * 2));
		}
	}

	/////////
	//RELIGION CHECKS
	////////////
	ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
	if (eReligion == NO_RELIGION)
	{
		eReligion = kPlayer.GetReligions()->GetReligionInMostCities();
	}
	if(eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pCity->getOwner());
		if(pReligion)
		{
			CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
			const int iNumBeliefs = pkBeliefs->GetNumBeliefs();
			for(int iI = 0; iI < iNumBeliefs; iI++)
			{
				const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
				CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
				if(pEntry && pReligion->m_Beliefs.HasBelief(eBelief))
				{
					if(pEntry->GetBuildingClassHappiness((int)pkBuildingInfo->GetBuildingClassType()))
					{
						iBonus += (pEntry->GetBuildingClassHappiness((int)pkBuildingInfo->GetBuildingClassType()) * 10);
					}
					if(pEntry->GetBuildingClassTourism((int)pkBuildingInfo->GetBuildingClassType()))
					{
						iBonus *= (pEntry->GetBuildingClassTourism((int)pkBuildingInfo->GetBuildingClassType()) * 10);
					}
					if(pkBuildingInfo->GetGreatWorkSlotType() != NO_GREAT_WORK_SLOT)
					{
						if(pEntry->GetGreatWorkYieldChange(pkBuildingInfo->GetGreatWorkSlotType()))
						{
							iBonus *= (pEntry->GetGreatWorkYieldChange(pkBuildingInfo->GetGreatWorkSlotType()) * 5);
						}
					}
					if(pkBuildingInfo->GetSpecialistType() != NO_SPECIALIST)
					{
						for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							const YieldTypes eYield = static_cast<YieldTypes>(iI);
							if(eYield != NO_YIELD)
							{
								if(pEntry->GetSpecialistYieldChange(pkBuildingInfo->GetSpecialistType(), eYield))
								{
									iBonus += (pEntry->GetSpecialistYieldChange(pkBuildingInfo->GetSpecialistType(), eYield) * 5);
								}
								if(pEntry->GetBuildingClassYieldChange((int)pkBuildingInfo->GetBuildingClassType(), eYield))
								{
									iBonus += (pEntry->GetBuildingClassYieldChange((int)pkBuildingInfo->GetBuildingClassType(), eYield) * 5);
								}
								if(m_pCity->GetCityCitizens()->GetTotalSpecialistCount() <= 0 && pEntry->GetYieldChangeAnySpecialist(eYield) > 0)
								{
									iBonus += (pEntry->GetYieldChangeAnySpecialist(eYield) * 5);
								}
							}
						}
					}
					if(pEntry->GetWonderProductionModifier() && isWorldWonderClass(kBuildingClassInfo))
					{
						iBonus += pEntry->GetWonderProductionModifier();
					}
				}
			}
		}
	}
	//////
	//WAR
	///////
	//Fewer buildings while at war.
	if(kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0)
	{
		iBonus -= (kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) * 25);
		if(kPlayer.GetMilitaryAI()->GetMostThreatenedCity(0) == m_pCity && kPlayer.getNumCities() > 1)
		{
			iBonus -= 300;
		}
		else if(kPlayer.GetMilitaryAI()->GetMostThreatenedCity(1) == m_pCity)
		{
			iBonus -= 200;
		}
		else if(kPlayer.GetMilitaryAI()->GetMostThreatenedCity(2) == m_pCity)
		{
			iBonus -= 200;
		}
	}
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
		{
			if(kPlayer.GetDiplomacyAI()->GetWarState(eLoopPlayer) < WAR_STATE_STALEMATE)
			{
				iBonus -= 100;
			}
		}
	}

	///////
	///Era Difference
	/////////

	TechTypes eTech = (TechTypes)pkBuildingInfo->GetPrereqAndTech();
	if(eTech != NO_TECH)
	{
		CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eTech);
		if(pEntry)
		{
			EraTypes eEra = (EraTypes)pEntry->GetEra();
			if(eEra != NO_ERA && eEra < kPlayer.GetCurrentEra())
			{
				iBonus *= ((kPlayer.GetCurrentEra() + 1) - eEra);
			}
		}
	}

	/////
	///WEIGHT
	//////
	//Let's reduce the value by turns to build (to keep us from taking dozens of turn on high-value things).
	if(pkBuildingInfo->GetGoldMaintenance() > 0)
	{
		int iGPT = (int)kPlayer.GetTreasury()->AverageIncome(10);
		if(iGPT < 0)
		{
			//Every -1 GPT = -6% bonus
			iBonus += (iGPT * 6);
		}
	}
	
	iValue *= (iBonus + 100);
	iValue /= 100;

	return iValue;
}
#endif
