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
				CvBuildingEntry* pBuildingEntry = pGameBuildings->GetEntry(m_Buildables.GetElement(iI));
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
int CvBuildingProductionAI::CheckBuildingBuildSanity(BuildingTypes eBuilding, int iValue, int iNumLandConnection, int iNumSeaConnection, int iGPT, bool bInterruptBuildings, bool bNationalWonderCheck)
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
	if ((m_pCity->IsPuppet() && !bIsVenice) || (m_pCity->getPopulation() <= 6 && !m_pCity->isCapital()))
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
		if(isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
		{
			if(kBuildingClassInfo.getMaxPlayerInstances() == 1)
			{
				return 0;
			}
		}
	}
	if (isNationalWonderClass(kBuildingClassInfo) || isWorldWonderClass(kBuildingClassInfo))
	{
		if (!bNationalWonderCheck)
		{
			if (!m_pCity->IsBestForWonder((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType()))
			{
				return 0;
			}
		}
		if (iValue > 750)
		{
			iValue = 750;
		}
		int iNumCivsAlreadyBuilding = kPlayer.GetNumCivsConstructingWonder(eBuilding);
		if (iNumCivsAlreadyBuilding > 0)
		{
			iValue -= (150 * iNumCivsAlreadyBuilding);
		}
	}
	else
	{
		//Sanitize...
		if (iValue > 350)
		{
			iValue = 350;
		}
	}

	//Bonus % additive. All values below will be added to this and combined with real value at end.
	int iBonus = 0;

	//////
	//WAR
	///////
	//Fewer buildings while at war.
	int iNumWar = kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false);
	if(iNumWar > 0 && pkBuildingInfo->GetDefenseModifier() <= 0 && !m_pCity->IsPuppet())
	{
		if(kPlayer.getNumCities() > 1 && m_pCity->GetThreatRank() != -1)
		{
			//More cities = more threat.
			int iThreat = (kPlayer.getNumCities() - m_pCity->GetThreatRank());
			if(iThreat > 0)
			{
				iBonus -= iThreat * 2;
			}
		}
		if(m_pCity->IsBastion())
		{
			iBonus -= (iNumWar * 100);
		}
		if( m_pCity->IsBlockaded(true) || m_pCity->IsBlockaded(false))
		{
			iBonus -= (iNumWar * 150);
		}
	}
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
		{
			if(kPlayer.GetDiplomacyAI()->GetWarState(eLoopPlayer) < WAR_STATE_STALEMATE)
			{
				iBonus -= 50;
			}
		}
	}
	//Tiny army? Eek!
	if(kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 3))
	{
		iBonus -= 250;
	}

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
						iBonus += 100;
					}
					if((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER) == eBuilding)
					{
						iBonus += 100;
					}
					if((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_CONSTRUCT_WONDER) == eBuilding)
					{
						iBonus += 100;
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
	if(pkBuildingInfo->GetNukeModifier() < 0)
	{
		iDefense += (pkBuildingInfo->GetNukeModifier() * -1);
	}
	if(pkBuildingInfo->GetGlobalDefenseModifier() > 0)
	{
		iDefense += pkBuildingInfo->GetGlobalDefenseModifier() / 5;
	}
	if(pkBuildingInfo->GetDefenseModifier() > 0)
	{
		iDefense += (pkBuildingInfo->GetDefenseModifier() / 5);
	}
#if defined(MOD_BALANCE_CORE)
	if(pkBuildingInfo->GetBuildingDefenseModifier() > 0)
	{
		iDefense += pkBuildingInfo->GetBuildingDefenseModifier() / 5;
	}
#endif
	if(pkBuildingInfo->GetExtraCityHitPoints() > 0)
	{
		iDefense += (pkBuildingInfo->GetExtraCityHitPoints() / 5);
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
			iDefense += 100;
		}
		if(pkBuildingInfo->IsBorderObstacle())
		{
			iDefense += 100;
		}
	}
	if(m_pCity->IsBastion())
	{
		iDefense *= 10;
	}
	iBonus += iDefense;

	//No Land trade connections?
	if(pkBuildingInfo->GetTradeRouteLandDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteLandGoldBonus() > 0 || pkBuildingInfo->GetLandTourismEnd() > 0)
	{	
		if(iNumLandConnection <= 0)
		{
			if (pkBuildingInfo->GetTradeRouteLandDistanceModifier() > 0)
			{
				iBonus -= 100;
			}
			else
			{
				iBonus -= 25;
			}
		}
		else
		{
			//Higher value the higher the number of routes.
			iBonus += iNumLandConnection;
			if(kPlayer.GetPlayerTraits()->GetLandTradeRouteRangeBonus() > 0)
			{
				iBonus += 50;
			}
		}
	}

	bool bGoodforGPTHappiness = false;

	//No Sea Trade Connections?
	if(pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteSeaGoldBonus() > 0 || pkBuildingInfo->GetSeaTourismEnd() > 0)
	{
		CvCity* pCapital = kPlayer.getCapitalCity();
		if(pkBuildingInfo->AllowsWaterRoutes())
		{
			if(iNumSeaConnection <= 0 && m_pCity->IsRouteToCapitalConnected())
			{
				iBonus -= 100;
			}
			else if(iNumSeaConnection <= 0 && pCapital != NULL && pCapital->getArea() != m_pCity->getArea())
			{
				iBonus += 50;
			}

			//Higher value the higher the number of routes.
			iBonus += iNumSeaConnection;
			if(kPlayer.GetPlayerTraits()->GetSeaTradeRouteRangeBonus() > 0)
			{
				iBonus += 50;
			}

			int iUnhappyConnection = m_pCity->getUnhappinessFromConnection();
			if (iUnhappyConnection > 0)
			{
				iBonus += (iUnhappyConnection * 50);
				bGoodforGPTHappiness = true;
			}
		}
		else
		{
			if(iNumSeaConnection <= 0)
			{
				if (pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0)
				{
					iBonus -= 100;
				}
				else
				{
					iBonus -= 25;
				}
			}
			//Higher value the higher the number of routes.
			iBonus += iNumSeaConnection;
			if(kPlayer.GetPlayerTraits()->GetSeaTradeRouteRangeBonus() > 0)
			{
				iBonus += 50;
			}
		}
	}

	if(pkBuildingInfo->IsAddsFreshWater() && m_pCity->plot()->isFreshWater())
	{
		iBonus += 25;
	}
	
	if(isWorldWonderClass(kBuildingClassInfo))
	{
		iBonus += (kPlayer.GetPlayerTraits()->GetWonderProductionModifier() + kPlayer.getWonderProductionModifier());
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
		iResource *= 10;
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
				iBonus += (50 - iNumOwned);
			}
		}
	}

	
	/////////
	////Happiness (CBP)
	////////

	
	if(pkBuildingInfo->GetHappiness() > 0 || pkBuildingInfo->GetHappinessPerCity() > 0 || pkBuildingInfo->GetHappinessPerXPolicies() > 0 || pkBuildingInfo->GetUnmoddedHappiness() > 0)
	{
		iBonus += kPlayer.GetUnhappiness() * 50;
		bGoodforGPTHappiness = true;
	}
	if (kPlayer.IsEmpireUnhappy())
	{
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
					iBonus += (kPlayer.getBuildingClassCount((BuildingClassTypes)pkLoopBuilding->GetBuildingClassType()) * 25);
					bGoodforGPTHappiness = true;
				}
			}
		}
	}

	if (MOD_BALANCE_CORE_HAPPINESS)
	{
		bool bTested = false;
		//Only checking for buildings that matter, and only once per test (based on prioritization of city happiness logic.
		int iPoverty = pkBuildingInfo->GetPovertyHappinessChangeBuilding() + pkBuildingInfo->GetPovertyHappinessChangeBuildingGlobal();
		int iIlliteracy = pkBuildingInfo->GetIlliteracyHappinessChangeBuilding() + pkBuildingInfo->GetIlliteracyHappinessChangeBuildingGlobal();
		int iDefense = pkBuildingInfo->GetDefenseHappinessChangeBuilding() + pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal();
		int iBoredom = pkBuildingInfo->GetUnculturedHappinessChangeBuilding() + pkBuildingInfo->GetUnculturedHappinessChangeBuildingGlobal();
		int iReligion = pkBuildingInfo->GetMinorityHappinessChangeBuilding() + pkBuildingInfo->GetMinorityHappinessChangeBuildingGlobal();
		if (iPoverty < 0)
		{
			int iUnhappyGold = m_pCity->getUnhappinessFromGold();
			if (iUnhappyGold > 0)
			{
				iPoverty *= -1;
				bTested = true;
				iBonus += (iUnhappyGold * max(2, iPoverty));
				bGoodforGPTHappiness = true;
			}
		}
		else if (!bTested && (iDefense < 0))
		{
			int iUnhappyDefense = m_pCity->getUnhappinessFromDefense();
			if (iUnhappyDefense > 0)
			{
				iDefense *= -1;
				bTested = true;
				iBonus += (iUnhappyDefense * max(2, iDefense));
				bGoodforGPTHappiness = true;
			}
		}
		else if (!bTested && (iReligion < 0))
		{
			int iUnhappyReligion = m_pCity->getUnhappinessFromReligion();
			if (iUnhappyReligion > 0)
			{
				iReligion *= -1;
				bTested = true;
				iBonus += (iUnhappyReligion * max(2, iReligion));
				bGoodforGPTHappiness = true;
			}
		}
		else if (!bTested && (iBoredom < 0))
		{
			int iUnhappyCulture = m_pCity->getUnhappinessFromCulture();
			if (iUnhappyCulture > 0)
			{
				iBoredom *= -1;
				bTested = true;
				iBonus += (iUnhappyCulture * max(2, iBoredom));
				bGoodforGPTHappiness = true;
			}
		}
		else if (!bTested && (iIlliteracy < 0))
		{
			int iUnhappyScience = m_pCity->getUnhappinessFromScience();
			if (iUnhappyScience > 0)
			{
				iIlliteracy *= -1;
				bTested = true;
				iBonus += (iUnhappyScience * max(2, iIlliteracy));
				bGoodforGPTHappiness = true;
			}
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
			iBonus += 500;
		}
	}
#endif
	//Courthouse? Let's get it ASAP.
	if(pkBuildingInfo->IsNoOccupiedUnhappiness())
	{
		if(m_pCity->IsOccupied())
		{
			//Extend based on population.
			iBonus += 500 * m_pCity->getPopulation();
			bGoodforGPTHappiness = true;
		}
	}
	

	//Corporations!
	if (pkBuildingInfo->GetBuildingClassInfo().getCorporationType() != NO_CORPORATION)
	{
		if(GET_PLAYER(m_pCity->getOwner()).GetCorporations()->GetFoundedCorporation() == pkBuildingInfo->GetBuildingClassInfo().getCorporationType())
		{
			iBonus += 500;
		}
	}
	if (pkBuildingInfo->GetBuildingClassInfo().IsHeadquarters())
	{
		if(GET_PLAYER(m_pCity->getOwner()).GetCorporations()->GetFoundedCorporation() == NO_CORPORATION)
		{
			iBonus += 1000;
		}
	}

	///////////////////
	//Military Stuff
	////////////////////////

	//Domain mods are good, and should be stacked.
	if(bIsVenice || !m_pCity->IsPuppet())
	{
		for (int iDomainLoop = 0; iDomainLoop < NUM_DOMAIN_TYPES; iDomainLoop++)
		{
			DomainTypes eTestDomain = (DomainTypes)iDomainLoop;
			if(eTestDomain != NO_DOMAIN)
			{
				int iTempBonus = 0;
				if(pkBuildingInfo->GetDomainFreeExperience(eTestDomain) > 0 || pkBuildingInfo->GetDomainFreeExperiencePerGreatWork(eTestDomain))
				{
					iTempBonus += (m_pCity->getDomainFreeExperience(eTestDomain) + pkBuildingInfo->GetDomainFreeExperience(eTestDomain) + pkBuildingInfo->GetDomainFreeExperiencePerGreatWork(eTestDomain));
				}		
				if(pkBuildingInfo->GetDomainProductionModifier(eTestDomain) > 0)
				{
					iTempBonus += m_pCity->getDomainProductionModifier(eTestDomain) + pkBuildingInfo->GetDomainProductionModifier(eTestDomain);
				}
				if(pkBuildingInfo->GetDomainFreeExperiencePerGreatWorkGlobal(eTestDomain) > 0)
				{
					iTempBonus += (m_pCity->getDomainFreeExperience(eTestDomain) +  pkBuildingInfo->GetDomainFreeExperiencePerGreatWorkGlobal(eTestDomain));
				}
				if(iTempBonus > 0)
				{
					//Let's try to build our military buildings in our best cities only. More cities we have, the more this matters.
					if(m_pCity == kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, eTestDomain))
					{
						iBonus += (iTempBonus * 25);
					}
					//Discourage bad cities.
					else
					{
						iBonus += iTempBonus;
					}
				}
			}
		}
	}
	//Unitcombat Bonuses should stack too.
	if(bIsVenice || !m_pCity->IsPuppet())
	{
		for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatClassInfo = GC.getUnitCombatClassInfo(eUnitCombatClass);
			if(pkUnitCombatClassInfo)
			{
				int iTempBonus = 0;
				if(pkBuildingInfo->GetUnitCombatProductionModifier(eUnitCombatClass) > 0)
				{
					iTempBonus += m_pCity->getUnitCombatProductionModifier(eUnitCombatClass) + pkBuildingInfo->GetUnitCombatProductionModifier(eUnitCombatClass);
				}
				if(pkBuildingInfo->GetUnitCombatFreeExperience(eUnitCombatClass) > 0)
				{
					iTempBonus += m_pCity->getUnitCombatFreeExperience(eUnitCombatClass) + pkBuildingInfo->GetUnitCombatFreeExperience(eUnitCombatClass);
				}
				if(iTempBonus > 0)
				{
					//Let's try to build our production/experience buildings in our best cities only. More cities we have, the more this matters.
					if(m_pCity == kPlayer.GetBestMilitaryCity(eUnitCombatClass))
					{
						iBonus += (iTempBonus * 25);
					}
					//Discourage bad cities.
					else
					{
						iBonus += iTempBonus;
					}
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

		if(!MOD_BALANCE_CORE_JFD && eYield > YIELD_CULTURE_LOCAL)
			continue;

		iYieldValue = CityStrategyAIHelpers::GetBuildingYieldValue(m_pCity, eBuilding, eYield);

		int iYieldTrait = CityStrategyAIHelpers::GetBuildingTraitValue(m_pCity, eYield, eBuilding, iYieldValue);

		if((iYieldValue > 0) || (iYieldTrait > 0))
		{
			//Help with poverty
			if(eYield == YIELD_GOLD)
			{
				if(iGPT < 0)
				{
					iYieldValue += (iGPT * -25);
				}
				bGoodforGPTHappiness = true;
			}
			//Puppets should focus on yield buildings.
			if (m_pCity->IsPuppet())
			{
				iYieldValue *= 5;
				iYieldTrait *= 5;
			}
		}

		iBonus += iYieldValue;
		iBonus += iYieldTrait;
	}

	//////////////
	///Maintenance
	/////////////////////
	if(pkBuildingInfo->GetGoldMaintenance() > 0 && !bGoodforGPTHappiness)
	{
		//Maintenace getting close to our GPT? Let's minimize this.
		if (iGPT <= (pkBuildingInfo->GetGoldMaintenance() * 2))
		{
			iBonus -= (pkBuildingInfo->GetGoldMaintenance() * 5);
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
						iBonus += (pEntry->GetBuildingClassHappiness((int)pkBuildingInfo->GetBuildingClassType()) * 5);
					}
					if(pEntry->GetBuildingClassTourism((int)pkBuildingInfo->GetBuildingClassType()))
					{
						iBonus += (pEntry->GetBuildingClassTourism((int)pkBuildingInfo->GetBuildingClassType()) * 5);
					}
					if(pkBuildingInfo->GetGreatWorkSlotType() != NO_GREAT_WORK_SLOT)
					{
						if(pEntry->GetGreatWorkYieldChange(pkBuildingInfo->GetGreatWorkSlotType()))
						{
							iBonus += (pEntry->GetGreatWorkYieldChange(pkBuildingInfo->GetGreatWorkSlotType()) * 5);
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
									iBonus += (pEntry->GetYieldChangeAnySpecialist(eYield) * 2);
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
			if(eEra != NO_ERA)
			{
				int iEraValue = ((kPlayer.GetCurrentEra() * 5) - eEra);
				if (iEraValue > 0)
				{
					iBonus += (100 * iEraValue);
				}
			}
		}
	}
	else
	{
		int iEraValue = ((kPlayer.GetCurrentEra() * 5));
		if (iEraValue > 0)
		{
			iBonus += (100 * iEraValue);
		}
	}

	//UB?
	if (kPlayer.getCivilizationInfo().isCivilizationBuildingOverridden(pkBuildingInfo->GetBuildingClassType()))
	{
		iBonus *= 10;
	}

	/////
	///WEIGHT
	//////
	
	iValue *= (iBonus + 100);
	iValue /= 100;

	if(bInterruptBuildings && !m_pCity->IsPuppet())
	{
		iValue /= 5;
	}

	return iValue;
}
#endif