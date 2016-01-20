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
		for(int i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
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
		for(int i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
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
		int iNumBuildings = m_pCityBuildings->GetBuildings()->GetNumBuildings();
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

	CvBuildingXMLEntries* pkBuildings = m_pCityBuildings->GetBuildings();

	// Loop through all buildings
	for(int iBuilding = 0; iBuilding < m_pCityBuildings->GetBuildings()->GetNumBuildings(); iBuilding++)
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
int CvBuildingProductionAI::CheckBuildingBuildSanity(BuildingTypes eBuilding, int iValue,int iNumLandConnection, int iNumSeaConnection)
{
	if(m_pCity == NULL)
		return 0;

	if(GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
		return iValue;

	if(eBuilding == NO_BUILDING)
		return 0;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
	bool bIsVenice = GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsNoAnnexing();
	if(m_pCity->IsPuppet())
	{
		if(isWorldWonderClass(kBuildingClassInfo) || isTeamWonderClass(kBuildingClassInfo) || ((isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo)) && !bIsVenice))
		{
			return 0;
		}
	}
	
	//Let's not send uniques through this - they're generally good enough to spam.
	if((BuildingTypes)GET_PLAYER(m_pCity->getOwner()).getCivilizationInfo().isCivilizationBuildingOverridden(eBuilding))
	{
		iValue *= 10;
		return iValue;
	}

	//Skip if null
	if(pkBuildingInfo == NULL)
		return 0;

	////////////
	///GEOGRAPHY
	////////////
	CvArea* pArea = GC.getMap().getArea(m_pCity->getArea());
	if(pArea == NULL)
		return 0;

	//Airlift
	if(pkBuildingInfo->IsAirlift())
	{
		if(!m_pCity->isCapital() && GET_PLAYER(m_pCity->getOwner()).getCapitalCity() != NULL)
		{
			if(m_pCity->getArea() != GET_PLAYER(m_pCity->getOwner()).getCapitalCity()->getArea())	
			{
				iValue *= 5;
			}
		}
	}

	//Defense Needs
	if(m_pCity->IsBastion())
	{
		if(pkBuildingInfo->GetDefenseModifier() > 0)
		{
			iValue *= pkBuildingInfo->GetDefenseModifier();
		}
	}
	else if(!m_pCity->IsBastion() && pkBuildingInfo->GetDefenseModifier() > 0)
	{
		iValue /= 10;
	}

	//No Land trade connections?
	if(pkBuildingInfo->GetLandTourismEnd() > 0 || pkBuildingInfo->GetTradeRouteLandDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteLandGoldBonus() > 0)
	{	
		if(iNumLandConnection == 0)
		{
			return 0;
		}
		else
		{
			//Higher value the higher the number of routes.
			iValue *= 100;
			iValue /= max(1, (100 - iNumLandConnection));
		}
	}

	//No Sea Trade Connections?
	if(pkBuildingInfo->GetSeaTourismEnd() > 0 || pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteSeaGoldBonus() > 0)
	{
		if(iNumSeaConnection == 0)
		{
			return 0;
		}
		else
		{
			//Higher value the higher the number of routes.
			iValue *= 100;
			iValue /= max(1, (100 - iNumSeaConnection));
		}
	}
	if(pkBuildingInfo->IsAddsFreshWater() && m_pCity->plot()->isFreshWater())
	{
		iValue /= 2;
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
			iValue *= 100;
			iValue /= max(1, (120 - iResource));
		}
	}

	
	/////////
	////Happiness (CBP)
	////////
	if(MOD_BALANCE_CORE_HAPPINESS)
	{
		// find flavors
		FlavorTypes eFlavorScience = NO_FLAVOR;
		FlavorTypes eFlavorGold = NO_FLAVOR;
		FlavorTypes eFlavorCulture = NO_FLAVOR;
		FlavorTypes eFlavorDefense = NO_FLAVOR;
		FlavorTypes eFlavorReligion = NO_FLAVOR;
		for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
		{
			if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
			{
				eFlavorScience = (FlavorTypes)iFlavorLoop;
			}
			else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GOLD")
			{
				eFlavorGold = (FlavorTypes)iFlavorLoop;
			}
			else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CITY_DEFENSE")
			{
				eFlavorDefense = (FlavorTypes)iFlavorLoop;
			}
			else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CULTURE")
			{
				eFlavorCulture = (FlavorTypes)iFlavorLoop;
			}
			else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_RELIGION")
			{
				eFlavorReligion = (FlavorTypes)iFlavorLoop;
			}
		}
		if(m_pCity->getUnhappinessFromCulture() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorCulture) > 0) || (pkBuildingInfo->GetUnculturedHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetUnculturedHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (m_pCity->getUnhappinessFromCulture() * 10);
		}
		if(m_pCity->getUnhappinessFromGold() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorGold) > 0) || (pkBuildingInfo->GetPovertyHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetPovertyHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (m_pCity->getUnhappinessFromGold() * 10);
		}
		if(m_pCity->getUnhappinessFromDefense() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorDefense) > 0) || (pkBuildingInfo->GetDefenseHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (m_pCity->getUnhappinessFromDefense() * 10);
		}
		if(m_pCity->getUnhappinessFromScience() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorScience) > 0) || (pkBuildingInfo->GetIlliteracyHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetIlliteracyHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (m_pCity->getUnhappinessFromScience() * 10);
		}
		if(m_pCity->getUnhappinessFromReligion() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorReligion) > 0) || (pkBuildingInfo->GetMinorityHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetMinorityHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (m_pCity->getUnhappinessFromReligion() * 10);
		}
	}
	if(GET_PLAYER(m_pCity->getOwner()).IsEmpireUnhappy())
	{
		if(pkBuildingInfo->GetHappiness() > 0 || pkBuildingInfo->GetHappinessPerCity() > 0 || pkBuildingInfo->GetHappinessPerXPolicies() > 0 || pkBuildingInfo->GetUnmoddedHappiness() > 0)
		{
			iValue *= 10;
		}
	}

	////////
	////Specific Concepts
	///////

	if(pkBuildingInfo->GetLandmarksTourismPercent() > 0)
	{
		int iFromWonders = m_pCity->GetCityCulture()->GetCultureFromWonders();
		int iFromNaturalWonders = m_pCity->GetCityCulture()->GetCultureFromNaturalWonders();
		int iFromImprovements = m_pCity->GetCityCulture()->GetYieldFromImprovements(YIELD_CULTURE);

		int iTest = (iFromWonders + iFromNaturalWonders + iFromImprovements);
		iTest = range(iTest,0,99);

		//Higher value the higher the number of routes.
		iValue *= 100;
		iValue /= (100-iTest);
	}
	if(pkBuildingInfo->GetGreatWorksTourismModifier() > 0)
	{
		int iWorks = m_pCity->GetCityCulture()->GetNumGreatWorks() * GC.getBASE_TOURISM_PER_GREAT_WORK();
		iWorks = range(iWorks,0,99);

		//Higher value the higher the number of works.
		iValue *= 100;
		iValue /= (100-iWorks);
	}

	//Faith Bonus for Faith Civs
	if(pkBuildingInfo && pkBuildingInfo->GetYieldChange(YIELD_FAITH) > 0)
	{
		if(GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsUniqueBeliefsOnly() || GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsBonusReligiousBelief() || GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsReconquista() || GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsPopulationBoostReligion() || GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->GetFaithFromKills() > 0 || GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsFaithFromUnimprovedForest())
		{
			iValue *= 25;
		}
	}
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	if(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	{
		const BuildingClassTypes eBuildingClass = (BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType());
		if(m_pCity->IsBuildingInvestment(eBuildingClass))
		{
			iValue *= 100;
		}
	}
#endif

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
				if(pkBuildingInfo->GetDomainFreeExperience(eTestDomain) > 0)
				{
					if(m_pCity->getDomainFreeExperience(eTestDomain) > 0)
					{
						iValue *= m_pCity->getDomainFreeExperience(eTestDomain);
					}
				}		
				if(pkBuildingInfo->GetDomainFreeExperiencePerGreatWork(DOMAIN_LAND) > 0)
				{
					if(m_pCity->getDomainFreeExperienceFromGreatWorks(eTestDomain) > 0)
					{
						iValue *= m_pCity->getDomainFreeExperienceFromGreatWorks(eTestDomain);
					}
				}
				if(pkBuildingInfo->GetDomainProductionModifier(DOMAIN_AIR) > 0)
				{
					if(m_pCity->getDomainProductionModifier(eTestDomain) > 0)
					{
						iValue *= m_pCity->getDomainProductionModifier(eTestDomain);
					}
				}
			}
		}
	}
	
	/////////////
	//Grand Strategies
	///////////////
	// == Grand Strategy ==
	AIGrandStrategyTypes eGrandStrategy = GET_PLAYER(m_pCity->getOwner()).GetGrandStrategyAI()->GetActiveGrandStrategy();
	bool bSeekingDiploVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS");
	bool bSeekingConquestVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	bool bSeekingCultureVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	bool bSeekingScienceVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");

	//Let's look at building special traits.
	if(bSeekingDiploVictory)
	{
		if(isWorldWonderClass(kBuildingClassInfo))
		{
			iValue *= 15;
		}
		if(isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
		{
			iValue *= 10;
		}
		// Don't build the UN if you aren't going for the diplo victory
		if(pkBuildingInfo->IsDiplomaticVoting())
		{
			iValue *= 10;
		}
		if(pkBuildingInfo->GetSingleVotes() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetExtraLeagueVotes() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetEspionageModifier() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetGlobalEspionageModifier() > 0)
		{
			iValue *= 3;
		}
		if(pkBuildingInfo->GetSpyRankChange() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetInstantSpyRankChange() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetExtraSpies() > 0)
		{
			iValue *= 3;
		}
		if(pkBuildingInfo->GetMinorFriendshipChange() > 0)
		{
			iValue *= 3;
		}
#if defined(MOD_DIPLOMACY_CITYSTATES)
		if(MOD_DIPLOMACY_CITYSTATES)
		{
			if(pkBuildingInfo->GetCapitalsToVotes() > 0)
			{
				iValue *= 2;
			}
			if(pkBuildingInfo->GetRAToVotes() > 0)
			{
				iValue *= 2;
			}
			if(pkBuildingInfo->GetDoFToVotes() > 0)
			{
				iValue *= 2;
			}
			if(pkBuildingInfo->GetFaithToVotes() > 0)
			{
				iValue *= 2;
			}
			if(pkBuildingInfo->GetDPToVotes() > 0)
			{
				iValue *= 2;
			}
			if(pkBuildingInfo->GetGPExpendInfluence() > 0)
			{
				iValue *= 2;
			}
			UnitCombatTypes eUnitCombat = (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_DIPLOMACY", true);
			if(eUnitCombat != NO_UNITCOMBAT)
			{
				if(pkBuildingInfo->GetUnitCombatProductionModifier((int)eUnitCombat) > 0)
				{
					iValue *= 3;
				}
			}
		}
#endif
	}
	//Let's look at building special traits.
	if(bSeekingConquestVictory)
	{
		if(isWorldWonderClass(kBuildingClassInfo))
		{
			iValue *= 5;
		}
		if(isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetAirModifier() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetAlwaysHeal() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetBorderObstacleCity() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetDefenseHappinessChangeBuilding() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetDefenseModifier() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetExtraCityHitPoints() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetFreeExperience() > 0)
		{
			iValue *= 4;
		}
		PromotionTypes eFreePromotion = (PromotionTypes) pkBuildingInfo->GetFreePromotion();
		if(eFreePromotion != NO_PROMOTION)
		{
			iValue *= 4;
		}
		PromotionTypes eFreeTrainedPromotion = (PromotionTypes) pkBuildingInfo->GetTrainedFreePromotion();
		if(eFreeTrainedPromotion != NO_PROMOTION)
		{
			iValue *= 4;
		}
		if(pkBuildingInfo->GetGlobalDefenseModifier() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetGlobalFreeExperience() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetGreatGeneralRateModifier() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetHappiness() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetUnmoddedHappiness() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetInstantMilitaryIncrease() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetNukeModifier() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetUnitUpgradeCostMod() > 0)
		{
			iValue *= 3;
		}
		if(pkBuildingInfo->IsBorderObstacle())
		{
			iValue *= 2;
		}
		for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatClassInfo = GC.getUnitCombatClassInfo(eUnitCombatClass);
			if(pkUnitCombatClassInfo)
			{
				if(pkBuildingInfo->GetUnitCombatProductionModifier(eUnitCombatClass) > 0)
				{
					iValue *= 2;
				}
			}
		}
	}
	if(bSeekingScienceVictory)
	{
		if(isWorldWonderClass(kBuildingClassInfo))
		{
			iValue *= 15;
		}
		if(isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
		{
			iValue *= 10;
		}
		if(pkBuildingInfo->GetFreeTechs() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetGlobalSpaceProductionModifier() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetGreatScientistBeakerModifier() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetSpaceProductionModifier() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetMedianTechPercentChange() > 0)
		{
			iValue *= 5;
		}
	}
	if(bSeekingCultureVictory)
	{
		if(isWorldWonderClass(kBuildingClassInfo))
		{
			iValue *= 20;
		}
		if(isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
		{
			iValue *= 10;
		}
		if(pkBuildingInfo->GetCultureRateModifier() > 0)
		{
			iValue *= 3;
		}
		if(pkBuildingInfo->GetGlobalCultureRateModifier() > 0)
		{
			iValue *= 4;
		}
		if(pkBuildingInfo->GetGreatPeopleRateChange() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetEventTourism() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetFreeGreatPeople() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetFreeGreatWork() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetFreePolicies() > 0)
		{
			iValue *= 5;
		}
		if(pkBuildingInfo->GetGlobalGreatPeopleRateModifier() > 0)
		{
			iValue *= 4;
		}
		if(pkBuildingInfo->GetGreatPeopleRateModifier() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetGreatWorkCount() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetGreatWorksTourismModifier() > 0)
		{
			iValue *= 3;
		}
		if(pkBuildingInfo->GetLandmarksTourismPercent() > 0)
		{
			iValue *= 3;
		}
		if(pkBuildingInfo->GetLandTourismEnd() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetSeaTourismEnd() > 0)
		{
			iValue *= 2;
		}
		if(pkBuildingInfo->GetTechEnhancedTourism() > 0)
		{
			iValue *= 3;
		}
	}

	////////////
	///YIELD BONUSES
	//////////
	//Let's look at yield bonuses.
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		const YieldTypes eYield = static_cast<YieldTypes>(iI);
		if(eYield == NO_YIELD)
			continue;

		if(pkBuildingInfo->GetYieldChange(eYield) > 0)
		{
			iValue *= pkBuildingInfo->GetYieldChange(eYield);
		}
		if(pkBuildingInfo->GetYieldChangePerPop(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldChangePerPop(eYield);
		}
		if(pkBuildingInfo->GetYieldChangePerReligion(eYield) > 0)
		{
			iValue *= pkBuildingInfo->GetYieldChangePerReligion(eYield);
		}
		if(pkBuildingInfo->GetYieldFromConstruction(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldFromConstruction(eYield);
		}
		if(pkBuildingInfo->GetYieldFromDeath(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldFromDeath(eYield);
		}
		if(pkBuildingInfo->GetYieldFromGPExpend(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldFromGPExpend(eYield);
		}
		if(pkBuildingInfo->GetYieldFromTech(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldFromTech(eYield);
		}
		if(pkBuildingInfo->GetYieldFromVictory(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldFromVictory(eYield);
		}
		if(pkBuildingInfo->GetYieldFromWLTKD(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldFromWLTKD(eYield);
		}
		if(pkBuildingInfo->GetYieldModifier(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetYieldModifier(eYield);
		}
		if(pkBuildingInfo->GetThemingYieldBonus(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetThemingYieldBonus(eYield);
		}
		if(pkBuildingInfo->GetInstantYield(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetInstantYield(eYield);
		}
		if(pkBuildingInfo->GetGrowthExtraYield(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetGrowthExtraYield(eYield);
		}
		if(pkBuildingInfo->GetGlobalYieldModifier(eYield) > 0)
		{
			iValue += pkBuildingInfo->GetGlobalYieldModifier(eYield);
		}
		if(m_pCity->plot()->isRiver() && pkBuildingInfo->GetRiverPlotYieldChange(eYield) > 0)
		{
			iValue *= pkBuildingInfo->GetRiverPlotYieldChange(eYield);
		}
		if(m_pCity->plot()->isCoastalLand() && pkBuildingInfo->GetSeaPlotYieldChange(eYield) > 0)
		{
			iValue *= pkBuildingInfo->GetSeaPlotYieldChange(eYield);
		}
		if(pkBuildingInfo->GetAreaYieldModifier(eYield) > 0)
		{
			iValue *= pkBuildingInfo->GetAreaYieldModifier(eYield);
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
				if(pkBuildingInfo->GetBuildingClassYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield) > 0)
				{
					iValue *= pkBuildingInfo->GetBuildingClassYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield);
				}
				if(pkBuildingInfo->GetBuildingClassLocalYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield) > 0)
				{
					iValue += pkBuildingInfo->GetBuildingClassLocalYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield);
				}
			}
		}
		int iNumTerrainInfos = GC.getNumTerrainInfos();
		for(int iI = 0; iI < iNumTerrainInfos; iI++)
		{
			TerrainTypes eTerrain = (TerrainTypes)iI;
			if(eTerrain == NO_TERRAIN)
				continue;

			if(m_pCity->GetNumTerrainWorked(eTerrain) > 0)
			{
				if(pkBuildingInfo->GetYieldPerXTerrain(eTerrain, eYield) > 0)
				{
					iValue *= (pkBuildingInfo->GetYieldPerXTerrain(eTerrain, eYield) / m_pCity->GetNumTerrainWorked(eTerrain));
				}
				if(pkBuildingInfo->GetTerrainYieldChange(eTerrain, eYield) > 0)
				{
					iValue *= m_pCity->GetNumTerrainWorked(eTerrain);
				}
			
			}
		}
	}
	//////////////
	///Puppet Controls
	/////////////////////
	if(pkBuildingInfo->GetGoldMaintenance() > 0)
	{
		iValue /= pkBuildingInfo->GetGoldMaintenance();
		if(m_pCity->IsPuppet())
		{
			EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
			if(eStrategyLosingMoney != NO_ECONOMICAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
			{
				return 0;
			}
			iValue /= max(2, (pkBuildingInfo->GetGoldMaintenance() * 2));
		}
	}

	/////////
	//RELIGION CHECKS
	////////////
	ReligionTypes eReligion = GET_PLAYER(m_pCity->getOwner()).GetReligions()->GetReligionInMostCities();
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
						iValue *= 2;
					}
					if(pEntry->GetBuildingClassTourism((int)pkBuildingInfo->GetBuildingClassType()))
					{
						iValue *= 2;
					}
					if(pkBuildingInfo->GetGreatWorkSlotType() != NO_GREAT_WORK_SLOT)
					{
						if(pEntry->GetGreatWorkYieldChange(pkBuildingInfo->GetGreatWorkSlotType()))
						{
							iValue *= 10;
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
									iValue += pEntry->GetSpecialistYieldChange(pkBuildingInfo->GetSpecialistType(), eYield);
								}
								if(pEntry->GetBuildingClassYieldChange((int)pkBuildingInfo->GetBuildingClassType(), eYield))
								{
									iValue += pEntry->GetBuildingClassYieldChange((int)pkBuildingInfo->GetBuildingClassType(), eYield);
								}
							}
						}
						if(pEntry->GetYieldChangeAnySpecialist(pkBuildingInfo->GetSpecialistType()))
						{
							iValue += pEntry->GetYieldChangeAnySpecialist(pkBuildingInfo->GetSpecialistType());
						}
					}
					if(pEntry->GetWonderProductionModifier() && isWorldWonderClass(kBuildingClassInfo))
					{
						iValue *= 10;
					}
				}
			}
		}
	}
	//////
	//WAR
	///////
	if(GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0)
	{
		iValue /= max(1, GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetNumberCivsAtWarWith(false) * 4);
	}

	/////
	///WEIGHT
	//////
	iValue /= max(1, (m_pCity->getProductionTurnsLeft(eBuilding, 0) / 4));

	return iValue;
}
#endif
