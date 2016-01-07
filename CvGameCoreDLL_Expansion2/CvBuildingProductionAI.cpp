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

// include after all other headers
#include "LintFree.h"

/// Constructor
CvBuildingProductionAI::CvBuildingProductionAI(CvCity* pCity, CvCityBuildings* pCityBuildings):
	m_pCity(pCity),
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
int CvBuildingProductionAI::CheckBuildingBuildSanity(CvCity* pCity, BuildingTypes eBuilding, int iValue)
{
	if(pCity == NULL)
		return 0;

	if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
		return iValue;

	if(eBuilding == NO_BUILDING)
		return 0;
	
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Let's not send uniques through this - they're generally good enough to spam.
	if((BuildingTypes)GET_PLAYER(pCity->getOwner()).getCivilizationInfo().isCivilizationBuildingOverridden(eBuilding))
	{
		iValue *= 2;
		return iValue;
	}

	//Skip if null
	if(pkBuildingInfo == NULL)
		return 0;

	CvPlayerTrade* pTrade = GET_PLAYER(pCity->getOwner()).GetTrade();

	////////////
	///GEOGRAPHY
	////////////
	CvArea* pArea = GC.getMap().getArea(pCity->getArea());
	if(pArea == NULL)
		return 0;

	//Defense Needs
	if(pCity->IsBastion())
	{
		if(pkBuildingInfo->GetDefenseModifier() > 0)
		{
			iValue *= pkBuildingInfo->GetDefenseModifier();
		}
	}
	else if(!pCity->IsBastion() && pkBuildingInfo->GetDefenseModifier() > 0)
	{
		iValue /= 25;
	}
	
	//Caravan stuff

	//No Land trade connections?
	if(pkBuildingInfo->GetLandTourismEnd() > 0 || pkBuildingInfo->GetTradeRouteLandDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteLandGoldBonus() > 0)
	{
		int iLandTrade = pTrade->GetNumPotentialConnections(pCity, DOMAIN_LAND, false);
		if(iLandTrade == 0)
		{
			return 0;
		}
		else
		{
			//Higher value the higher the number of routes.
			iValue *= 100;
			iValue /= max(1, (110 - iLandTrade));
		}
	}

	//No Sea Trade Connections?
	if(pkBuildingInfo->GetSeaTourismEnd() > 0 || pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteSeaGoldBonus() > 0)
	{
		int iSeaTrade = pTrade->GetNumPotentialConnections(pCity, DOMAIN_SEA, false);
		if(iSeaTrade == 0)
		{
			return 0;
		}
		else
		{
			//Higher value the higher the number of routes.
			iValue *= 100;
			iValue /= max(1, (110 - iSeaTrade));
		}
	}
	if(pkBuildingInfo->IsAddsFreshWater() && pCity->plot()->isFreshWater())
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
				if(pCity->GetNumResourceLocal(eResource) > 0)
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
		if(pCity->getUnhappinessFromCulture() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorCulture) > 0) || (pkBuildingInfo->GetUnculturedHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetUnculturedHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (pCity->getUnhappinessFromCulture() * 10);
		}
		if(pCity->getUnhappinessFromGold() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorGold) > 0) || (pkBuildingInfo->GetPovertyHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetPovertyHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (pCity->getUnhappinessFromGold() * 10);
		}
		if(pCity->getUnhappinessFromDefense() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorDefense) > 0) || (pkBuildingInfo->GetDefenseHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (pCity->getUnhappinessFromDefense() * 10);
		}
		if(pCity->getUnhappinessFromScience() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorScience) > 0) || (pkBuildingInfo->GetIlliteracyHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetIlliteracyHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (pCity->getUnhappinessFromScience() * 10);
		}
		if(pCity->getUnhappinessFromMinority() > 0 && ((pkBuildingInfo->GetFlavorValue(eFlavorReligion) > 0) || (pkBuildingInfo->GetMinorityHappinessChangeBuilding() != 0) || (pkBuildingInfo->GetMinorityHappinessChangeBuildingGlobal() != 0)))
		{
			iValue *= (pCity->getUnhappinessFromMinority() * 10);
		}
	}

	////////
	////Specific Concepts
	///////

	if(pkBuildingInfo->GetLandmarksTourismPercent() > 0)
	{
		int iFromWonders = pCity->GetCityCulture()->GetCultureFromWonders();
		int iFromNaturalWonders = pCity->GetCityCulture()->GetCultureFromNaturalWonders();
		int iFromImprovements = pCity->GetCityCulture()->GetYieldFromImprovements(YIELD_CULTURE);

		int iTest = (iFromWonders + iFromNaturalWonders + iFromImprovements);
		iTest = range(iTest,0,99);

		//Higher value the higher the number of routes.
		iValue *= 100;
		iValue /= (100-iTest);
	}
	if(pkBuildingInfo->GetGreatWorksTourismModifier() > 0)
	{
		int iWorks = pCity->GetCityCulture()->GetNumGreatWorks() * GC.getBASE_TOURISM_PER_GREAT_WORK();
		iWorks = range(iWorks,0,99);

		//Higher value the higher the number of works.
		iValue *= 100;
		iValue /= (100-iWorks);
	}

	//Faith Bonus for Faith Civs
	if(pkBuildingInfo && pkBuildingInfo->GetYieldChange(YIELD_FAITH) > 0)
	{
		if(GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsUniqueBeliefsOnly() || GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsBonusReligiousBelief() || GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsReconquista() || GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsPopulationBoostReligion() || GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->GetFaithFromKills() > 0 || GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsFaithFromUnimprovedForest())
		{
			iValue *= 25;
		}
	}
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	if(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	{
		const BuildingClassTypes eBuildingClass = (BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType());
		if(pCity->IsBuildingInvestment(eBuildingClass))
		{
			iValue *= 100;
		}
	}
#endif

	return iValue;
}
#endif
