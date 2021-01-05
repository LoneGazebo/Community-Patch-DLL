/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
int CvBuildingProductionAI::CheckBuildingBuildSanity(BuildingTypes eBuilding, int iValue,
	int iNumLandConnection, int iNumSeaConnection, int iGPT, 
	bool bInterruptBuildings, bool bNationalWonderCheck, bool bFreeBuilding, bool bIgnoreSituational)
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

	//Bonus additive. All values below will be added to this and combined with real value at end.
	int iBonus = 0;

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

		if (iValue > 300)
		{
			iValue = 300;
		}

		// we want this? ramp it up!
		if (kPlayer.GetCitySpecializationAI()->GetNextWonderDesired() == eBuilding)
		{
			if (isNationalWonderClass(kBuildingClassInfo))
			{
				//cap this at an extreme value.
				iBonus += min(1000, m_pCity->getPopulation() * 50);
			}
			else
			{
				//cap this at an extreme value.
				iBonus += min(1500, m_pCity->getPopulation() * 100);
			}
		}

		if (isWorldWonderClass(kBuildingClassInfo) && !bFreeBuilding && !bIgnoreSituational)
		{
			iValue += (kPlayer.GetPlayerTraits()->GetWonderProductionModifier() + kPlayer.getWonderProductionModifier());

			// Adjust weight for this wonder down based on number of other players currently working on it
			int iNumOthersConstructing = 0;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
				
				if (eLoopPlayer == kPlayer.GetID())
					continue;

				if (GET_PLAYER(eLoopPlayer).isAlive() && GET_TEAM(kPlayer.getTeam()).isHasMet(GET_PLAYER(eLoopPlayer).getTeam()) && GET_PLAYER(eLoopPlayer).getBuildingClassMaking((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType()) > 0)
				{
					iNumOthersConstructing++;
				}
			}
			iBonus -= iNumOthersConstructing * 150;

			//probably early game, so if we haven't started yet, we're probably not going to win this one.
			if (kPlayer.getNumCities() == 1 && !bIsVenice)
			{
				iBonus -= iNumOthersConstructing * 100;
			}
		}
	}
	else
	{
		//Sanitize...
		if (iValue > 100)
		{
			iValue = 100;
		}
		if (iValue <= 50)
		{
			iValue = 50;
		}
	}


	////////////////
	////QUESTS
	////////////////
	if (!bIgnoreSituational)
	{
		//Minor Check
		for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
			if (eMinor != NO_PLAYER)
			{
				CvPlayer* pMinor = &GET_PLAYER(eMinor);
				if (pMinor)
				{
					CvMinorCivAI* pMinorCivAI = pMinor->GetMinorCivAI();
					if (pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(m_pCity->getOwner(), MINOR_CIV_QUEST_BUILD_X_BUILDINGS))
					{
						if ((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_BUILD_X_BUILDINGS) == eBuilding)
						{
							iBonus += 10;
						}
						if ((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER) == eBuilding)
						{
							iBonus += 10;
						}
						if ((BuildingTypes)pMinorCivAI->GetQuestData1(m_pCity->getOwner(), MINOR_CIV_QUEST_CONSTRUCT_WONDER) == eBuilding)
						{
							iBonus += 10;
						}
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
				iBonus += 10;
			}
		}
	}

	//No Land trade connections?
	if(pkBuildingInfo->GetTradeRouteLandDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteLandGoldBonus() > 0 || pkBuildingInfo->GetLandTourismEnd() > 0)
	{	
		if (!bFreeBuilding && iNumLandConnection <= 0)
		{
			if (pkBuildingInfo->GetTradeRouteLandDistanceModifier() > 0)
			{
				iBonus -= 50;
			}
			else
			{
				iBonus -= 10;
			}
		}
		else
		{
			//Higher value the higher the number of routes.
			iBonus += iNumLandConnection;
			if(kPlayer.GetPlayerTraits()->GetLandTradeRouteRangeBonus() > 0)
			{
				iBonus += 10;
			}
		}
	}

	bool bGoodforHappiness = false;
	bool bGoodforGPT = false;

	//No Sea Trade Connections?
	if (pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0 || pkBuildingInfo->GetTradeRouteSeaGoldBonus() > 0 || pkBuildingInfo->GetSeaTourismEnd() > 0 || pkBuildingInfo->AllowsWaterRoutes())
	{
		//we know the city must be coastal. but if this is a lake and there are no other cities reachable via water, then it makes no sense to build this
		if (!m_pCity->plot()->isCoastalLand(-1) && GC.getGame().GetGameTrade()->GetAllPotentialTradeRoutesFromCity(m_pCity,true).empty())
			return 0;

		if (pkBuildingInfo->AllowsWaterRoutes()) //this is not about trade routes but city connections
		{
			CvCity* pCapital = kPlayer.getCapitalCity();
			if(iNumSeaConnection <= 0 && m_pCity->IsRouteToCapitalConnected())
			{
				iBonus -= 50;
			}
			else if(pCapital != NULL && pCapital->getArea() != m_pCity->getArea())
			{
				iBonus += 10 * max(1, m_pCity->getPopulation());
			}
			else
			{
				iBonus += 5 * max(1, m_pCity->getPopulation());
			}

			//Higher value the higher the number of routes.
			iBonus += iNumSeaConnection;
			if(kPlayer.GetPlayerTraits()->GetSeaTradeRouteRangeBonus() > 0 || kPlayer.getTradeRouteSeaDistanceModifier() != 0)
			{
				iBonus += 5 * max(1, iNumSeaConnection);
			}

			int iUnhappyConnection = m_pCity->getUnhappinessFromConnection();
			if (iUnhappyConnection > 0)
			{
				iBonus += (iUnhappyConnection * 10);
				bGoodforHappiness = true;
				bGoodforGPT = true;
			}
		}
		else
		{
			if (iNumSeaConnection <= 0 && !bFreeBuilding)
			{
				if (pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0)
				{
					iBonus -= 10;
				}
				else
				{
					iBonus -= 5;
				}
			}
			//Higher value the higher the number of routes.
			iBonus += iNumSeaConnection;
			if (kPlayer.GetPlayerTraits()->GetSeaTradeRouteRangeBonus() > 0 || kPlayer.getTradeRouteSeaDistanceModifier() != 0)
			{
				iBonus += 5 * max(1, iNumSeaConnection);
			}
		}
	}

	if(pkBuildingInfo->IsAddsFreshWater() && !m_pCity->plot()->isFreshWater())
	{
		iBonus += 25;
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
		if (iResource <= 0 && !bFreeBuilding)
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
				iNumOwned = (100 - (iNumOwned * 5));
				iBonus += max(0, iNumOwned);
			}
		}
	}

	
	/////////
	////Happiness (CBP)
	////////

	
	if(pkBuildingInfo->GetHappiness() > 0 || pkBuildingInfo->GetHappinessPerCity() > 0 || pkBuildingInfo->GetHappinessPerXPolicies() > 0 || pkBuildingInfo->GetUnmoddedHappiness() > 0)
	{
		iBonus += kPlayer.GetUnhappiness() * 10;
		bGoodforHappiness = true;
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
					bGoodforHappiness = true;
				}
			}
		}
	}

	int iPoverty = 0;
	int iIlliteracy = 0;
	int iCrime = 0;
	int iBoredom = 0;
	int iReligion = 0;
	int iEmpire = 0;
	if (MOD_BALANCE_CORE_HAPPINESS)
	{
		bool bTested = false;
		//Only checking for buildings that matter, and only once per test (based on prioritization of city happiness logic.
		iEmpire = pkBuildingInfo->GetEmpireNeedsModifier() + pkBuildingInfo->GetEmpireNeedsModifierGlobal();
		iPoverty = pkBuildingInfo->GetPovertyHappinessChangeBuilding() + pkBuildingInfo->GetPovertyHappinessChangeBuildingGlobal();
		iIlliteracy = pkBuildingInfo->GetIlliteracyHappinessChangeBuilding() + pkBuildingInfo->GetIlliteracyHappinessChangeBuildingGlobal();
		iCrime = pkBuildingInfo->GetDefenseHappinessChangeBuilding() + pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal();
		iBoredom = pkBuildingInfo->GetUnculturedHappinessChangeBuilding() + pkBuildingInfo->GetUnculturedHappinessChangeBuildingGlobal();
		iReligion = pkBuildingInfo->GetMinorityHappinessChangeBuilding() + pkBuildingInfo->GetMinorityHappinessChangeBuildingGlobal();

		if (iEmpire < 0)
		{
			int iUnhappyEmpire = m_pCity->getEmpireSizeMod();
			if (iUnhappyEmpire > 0)
			{
				iEmpire *= -1;
				iBonus += (iUnhappyEmpire * iEmpire * 10);
				bGoodforHappiness = true;
			}
		}
		else if (iPoverty < 0)
		{
			int iUnhappyGold = m_pCity->getUnhappinessFromGold();
			if (iUnhappyGold > 0)
			{
				iPoverty *= -1;
				bTested = true;
				iBonus += (iUnhappyGold * iPoverty);
				bGoodforHappiness = true;
				bGoodforGPT = true;
			}
		}
		else if (!bTested && (iCrime < 0))
		{
			int iUnhappyDefense = m_pCity->getUnhappinessFromDefense();
			if (iUnhappyDefense > 0)
			{
				iCrime *= -1;
				bTested = true;
				iBonus += (iUnhappyDefense * iCrime);
				bGoodforHappiness = true;
			}
		}
		else if (!bTested && (iReligion < 0))
		{
			int iUnhappyReligion = m_pCity->getUnhappinessFromReligion();
			if (iUnhappyReligion > 0)
			{
				iReligion *= -1;
				bTested = true;
				iBonus += (iUnhappyReligion * iReligion);
				bGoodforHappiness = true;
			}
		}
		else if (!bTested && (iBoredom < 0))
		{
			int iUnhappyCulture = m_pCity->getUnhappinessFromCulture();
			if (iUnhappyCulture > 0)
			{
				iBoredom *= -1;
				bTested = true;
				iBonus += (iUnhappyCulture * iBoredom);
				bGoodforHappiness = true;
			}
		}
		else if (!bTested && (iIlliteracy < 0))
		{
			int iUnhappyScience = m_pCity->getUnhappinessFromScience();
			if (iUnhappyScience > 0)
			{
				iIlliteracy *= -1;
				bTested = true;
				iBonus += (iUnhappyScience * iIlliteracy);
				bGoodforHappiness = true;
			}
		}
	}

	////////
	////Specific Concepts
	///////

	//Defense Needs
	int iDefense = 0;
	if (pkBuildingInfo->GetNukeModifier() < 0)
	{
		iDefense += (pkBuildingInfo->GetNukeModifier() * -1);
	}
	if (pkBuildingInfo->GetNukeInterceptionChance() > 0)
	{
		iDefense += (pkBuildingInfo->GetNukeInterceptionChance());
	}
	if (pkBuildingInfo->GetGlobalDefenseModifier() > 0)
	{
		iDefense += pkBuildingInfo->GetGlobalDefenseModifier() / 2;
	}
	if (pkBuildingInfo->GetDefenseModifier() > 0)
	{
		iDefense += (pkBuildingInfo->GetDefenseModifier() / 50);
	}
	if (pkBuildingInfo->CityRangedStrikeRange() > 0)
	{
		iDefense += 25 * pkBuildingInfo->CityRangedStrikeRange();
	}
	if (pkBuildingInfo->CityIndirectFire() > 0)
	{
		iDefense += 25;
	}
	if (pkBuildingInfo->CityRangedStrikeModifier() > 0)
	{
		iDefense += pkBuildingInfo->CityRangedStrikeModifier();
	}
	if (pkBuildingInfo->GetBuildingDefenseModifier() > 0)
	{
		iDefense += pkBuildingInfo->GetBuildingDefenseModifier() / 25;
	}
	if (pkBuildingInfo->GetExtraCityHitPoints() > 0)
	{
		iDefense += (pkBuildingInfo->GetExtraCityHitPoints() / 25);
	}
	if (m_pCity->isCoastal())
	{
		AICityStrategyTypes eStrategyLakeBound = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_LAKEBOUND");
		if (eStrategyLakeBound != NO_ECONOMICAISTRATEGY)
		{
			if (!m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyLakeBound))
			{
				if (pkBuildingInfo->GetBorderObstacleWater() > 0)
				{
					iDefense += 25;
				}
			}
		}
	}
	else
	{
		if (pkBuildingInfo->GetBorderObstacleLand() > 0)
		{
			iDefense += 25;
		}
		if (pkBuildingInfo->IsBorderObstacle())
		{
			iDefense += 25;
		}
	}
	int iDefenseMod = 100;
	if (m_pCity->isBorderCity() || m_pCity->isCoastal(12) )
	{
		if (kPlayer.IsAtWarAnyMajor())
			iDefenseMod += 1000;
		else
			iDefenseMod += 500;
	}
	else if (kPlayer.IsAtWarAnyMajor())
		iDefenseMod += 150;
	bool bDesperate = !bIgnoreSituational && (m_pCity->isInDangerOfFalling() || m_pCity->isUnderSiege());

	if (bDesperate || m_pCity->IsPuppet())
		iDefenseMod += 1000;

	iDefense *= iDefenseMod;
	iDefense /= 100;

	iBonus += iDefense;

	//desperate, and this ISN'T a defensive building?
	if (bDesperate && iDefense <= 0)
		iBonus -= 500;

	if (iBonus > 0 && (pkBuildingInfo->GetCitySupplyModifier() > 0 || pkBuildingInfo->GetCitySupplyModifierGlobal() > 0))
	{
		int iSupply = max(1,kPlayer.GetNumUnitsSupplied());
		int iDemand = kPlayer.GetMilitaryAI()->GetRecommendedMilitarySize();
		int iGrowth = pkBuildingInfo->GetCitySupplyModifier() + (pkBuildingInfo->GetCitySupplyModifierGlobal() * kPlayer.getNumCities());

		int iPercent = (iDemand * 100) / iSupply + max (0, iGrowth);

		//Closer we get to cap, more we want this.
		iBonus *= (100 + iPercent);
		iBonus /= 100;
	}

	if (iBonus > 0 &&  (pkBuildingInfo->GetCitySupplyFlat() > 0 || pkBuildingInfo->GetCitySupplyFlatGlobal() > 0))
	{
		int iSupply = max(1,kPlayer.GetNumUnitsSupplied());
		int iDemand = kPlayer.GetMilitaryAI()->GetRecommendedMilitarySize();
		int iGrowth = (pkBuildingInfo->GetCitySupplyFlat() + (pkBuildingInfo->GetCitySupplyFlatGlobal() * kPlayer.getNumCities()));

		int iPercent = (iDemand * 100) / iSupply + max (0, iGrowth);

		//Closer we get to cap, more we want this.
		iBonus *= (100 + iPercent);
		iBonus /= 100;
	}
	

#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	if (MOD_BALANCE_CORE_BUILDING_INVESTMENTS && !bIgnoreSituational)
	{
		//Virtually force this.
		const BuildingClassTypes eBuildingClass = (BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType());
		if(m_pCity->IsBuildingInvestment(eBuildingClass))
		{
			iBonus += 1000;
		}
	}
#endif
	//Courthouse? Let's get it ASAP.
	bool bCourthouse = false;
	if(pkBuildingInfo->IsNoOccupiedUnhappiness())
	{
		if(m_pCity->IsOccupied() && !m_pCity->IsNoOccupiedUnhappiness())
		{
			//Extend based on population.
			iBonus += 5000 * m_pCity->getPopulation();
			bGoodforHappiness = true;
			bCourthouse = true;
		}
	}
	
	//Unlocks another building?
	int iNumNeeded;
	//unfortunately we have to loop through all buildings in the game to do this. Sigh...
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		iNumNeeded = kPlayer.getBuildingClassPrereqBuilding((BuildingTypes)iI, (BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), true);
		//need in all?
		if (iNumNeeded > 0)
		{
			int iNumHave = kPlayer.getNumBuildings(eBuilding);
			iBonus += iNumNeeded * max(1, iNumHave) * 10;
		}
	}

	//Corporations!
	if (pkBuildingInfo->IsCorp())
	{
		iBonus += 100 * (max(1, GET_PLAYER(m_pCity->getOwner()).GetCorporations()->GetNumOffices()) + max(1, GET_PLAYER(m_pCity->getOwner()).GetCorporations()->GetNumFranchises()));
	}
	if (pkBuildingInfo->GetBuildingClassInfo().getCorporationType() != NO_CORPORATION)
	{
		if(GET_PLAYER(m_pCity->getOwner()).GetCorporations()->GetFoundedCorporation() == pkBuildingInfo->GetBuildingClassInfo().getCorporationType())
		{
			iBonus += 250;
		}
	}
	if (pkBuildingInfo->GetBuildingClassInfo().IsHeadquarters())
	{
		if(GET_PLAYER(m_pCity->getOwner()).GetCorporations()->GetFoundedCorporation() == NO_CORPORATION)
		{
			iBonus += 1000;
		}
	}

	//Espionage!
	if (pkBuildingInfo->GetBlockBuildingDestruction() > 0)
	{
		iBonus += (m_pCity->getYieldRate(YIELD_PRODUCTION, false) / 100);
	}
	if (pkBuildingInfo->GetBlockWWDestruction() > 0)
	{
		iBonus += (m_pCity->getNumWorldWonders() * 25);
	}
	if (pkBuildingInfo->GetBlockUDestruction() > 0)
	{
		iBonus += (m_pCity->getYieldRate(YIELD_PRODUCTION, false) / 100);
	}
	if (pkBuildingInfo->GetBlockGPDestruction() > 0)
	{
		iBonus += m_pCity->getGreatPeopleRate() / 10;
	}
	if (pkBuildingInfo->GetBlockRebellion() > 0)
	{
		//Less happy = better!
		iBonus += (100 - kPlayer.GetHappiness());
	}
	if (pkBuildingInfo->GetBlockUnrest() > 0)
	{
		//Less happy = better!
		iBonus += (50 - kPlayer.GetHappiness());
	}
	if (pkBuildingInfo->GetBlockScience() > 0)
	{
		iBonus += (m_pCity->getYieldRate(YIELD_SCIENCE, false) / 100);
	}
	if (pkBuildingInfo->GetBlockGold() > 0)
	{
		iBonus += (m_pCity->getYieldRate(YIELD_GOLD, false) / 100);
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
#if defined(MOD_BALANCE_CORE)
				int iTempMod = 0;
#endif
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
#if defined(MOD_BALANCE_CORE)
				if (pkBuildingInfo->GetDomainFreeExperienceGlobal(eTestDomain) > 0)
				{
					iTempBonus += m_pCity->getDomainFreeExperience(eTestDomain) + kPlayer.GetDomainFreeExperience(eTestDomain) + pkBuildingInfo->GetDomainFreeExperienceGlobal(eTestDomain);
				}
				if(kPlayer.GetPlayerTraits()->GetDomainFreeExperienceModifier(eTestDomain) != 0)
				{
					iTempMod += kPlayer.GetPlayerTraits()->GetDomainFreeExperienceModifier(eTestDomain);
				}

				iTempBonus *= (100 + iTempMod);
				iTempBonus /= 100;
#endif
				if(iTempBonus > 0)
				{
					//Let's try to build our military buildings in our best cities only. More cities we have, the more this matters.
					if(m_pCity == kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, eTestDomain))
					{
						iBonus += iTempBonus;
					}
					//Discourage bad cities.
					else
					{
						iBonus += iTempBonus / 2;
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
						iBonus += iTempBonus;
					}
					//Discourage bad cities.
					else
					{
						iBonus += (iTempBonus / 2);
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

	iBonus += iGSValue * (kPlayer.GetCurrentEra() + 1);

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

	YieldTypes eFocusYield = m_pCity->GetCityCitizens()->GetFocusTypeYield(m_pCity->GetCityCitizens()->GetFocusType());

	bool bSmall = m_pCity->getPopulation() <= 10;
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		const YieldTypes eYield = static_cast<YieldTypes>(iI);
		if(eYield == NO_YIELD)
			continue;

		if (pkBuildingInfo->GetYieldChangePerReligion(eYield) > 0 && kPlayer.GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			iBonus = 0;
			break;
		}

		if(!MOD_BALANCE_CORE_JFD && eYield > YIELD_CULTURE_LOCAL)
			continue;

		iYieldValue = CityStrategyAIHelpers::GetBuildingYieldValue(m_pCity, eBuilding, eYield);

		int iYieldTrait = CityStrategyAIHelpers::GetBuildingTraitValue(m_pCity, eYield, eBuilding, iYieldValue);

		int iHappinessReduction = pkBuildingInfo->GetUnhappinessNeedsFlatReduction(eYield);

		if ((iYieldValue > 0) || (iYieldTrait > 0))
		{
			switch (eYield)
			{
			case YIELD_GOLD:
				if (iGPT <= 10)
				{
					iYieldValue += (iGPT * -10);
					iYieldTrait += (iGPT * -10);
					bGoodforHappiness = true;
					bGoodforGPT = true;
				}
				if (iPoverty > 0)
				{
					iYieldValue += iHappinessReduction * 50;
					iYieldValue += (iPoverty * 10);
					iYieldTrait += (iPoverty * 10);
					bGoodforHappiness = true;
					bGoodforGPT = true;
				}
				break;
			case YIELD_SCIENCE:
				if (iIlliteracy > 0)
				{
					iYieldValue += iHappinessReduction * 50;

					iYieldValue += (iIlliteracy * 10);
					iYieldTrait += (iIlliteracy * 10);
					bGoodforHappiness = true;
				}
				break;
			case YIELD_FAITH:

				if (kPlayer.isMinorCiv())
				{
					iYieldTrait = 0;
					iYieldValue /= 10;
					break;
				}
				if (iReligion > 0)
				{
					iYieldValue += iHappinessReduction * 50;

					iYieldValue += (iReligion * 10);
					iYieldTrait += (iReligion * 10);

					bGoodforHappiness = true;
				}

				//explicit check for stonehenge.
				if (pkBuildingInfo->GetInstantYield(YIELD_FAITH) > 0 && kPlayer.GetReligions()->HasCreatedPantheon() && kPlayer.GetCurrentEra() <= 3)
					iReligion /= 50;

				break;
			case YIELD_CULTURE:
				if (iBoredom > 0)
				{
					iYieldValue += iHappinessReduction * 50;

					iYieldValue += (iBoredom * 10);
					iYieldTrait += (iBoredom * 10);
					bGoodforHappiness = true;
				}
				break;
			case YIELD_PRODUCTION:
				if (bSmall)
				{
					iYieldValue *= 4;
					iYieldTrait *= 4;
				}
				if (iCrime > 0)
				{
					iYieldValue += iHappinessReduction * 50;

					iYieldValue += (iCrime * 10);
					iYieldTrait += (iCrime * 10);
					bGoodforHappiness = true;
				}
				break;
			case YIELD_FOOD:
				if (m_pCity->GetCityCitizens()->IsAvoidGrowth())
				{
					iYieldValue = 0;
					iYieldTrait = 0;
				}
				else if (bSmall)
					iYieldValue *= 10;

				if (iCrime > 0)
				{
					iYieldValue += iHappinessReduction * 50;

					iYieldValue += (iCrime * 10);
					iYieldTrait += (iCrime * 10);
					bGoodforHappiness = true;
				}
				break;
			case YIELD_TOURISM:
				if (bSmall)
				{
					iYieldValue /= 2;
					iYieldTrait /= 2;
				}
				break;
			case YIELD_GOLDEN_AGE_POINTS:
				if (bSmall)
				{
					iYieldValue /= 2;
					iYieldTrait /= 2;
				}
				break;
			}
			
			if (eFocusYield == eYield)
			{
				iYieldValue *= 5;
				iYieldTrait *= 5;
			}
			
			//Puppets should focus on yield buildings.
			if (m_pCity->IsPuppet())
			{
				iYieldValue *= 2;
				iYieldTrait *= 2;
			}
		}

		iBonus += iYieldValue;
		iBonus += iYieldTrait;
	}

	//////////////
	///Maintenance
	/////////////////////
	if (pkBuildingInfo->GetGoldMaintenance() > 0 && !bCourthouse)
	{
		if (!bGoodforGPT && iGPT <= pkBuildingInfo->GetGoldMaintenance())
		{
			iBonus -= (pkBuildingInfo->GetGoldMaintenance() * pkBuildingInfo->GetGoldMaintenance() * pkBuildingInfo->GetGoldMaintenance());

			if (iBonus <= 0)
				iBonus = 1;
		}
	}

	/////////
	//RELIGION CHECKS
	////////////
	int iReligionValue = CityStrategyAIHelpers::GetBuildingReligionValue(m_pCity, eBuilding, kPlayer.GetID());

	iBonus += iReligionValue;

	//////
	//WAR
	///////
	//Fewer buildings while at war.
	if (!bCourthouse && !bIgnoreSituational)
	{
		int iNumWar = kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false);

		int WarPenalty = 0;
		//Tiny army? Eek!
		if (kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 3))
		{
			WarPenalty += 75;
		}
		if (iNumWar > 0 && pkBuildingInfo->GetDefenseModifier() <= 0 && !m_pCity->IsPuppet() && !bFreeBuilding && !kPlayer.IsEmpireVeryUnhappy())
		{
			WarPenalty += 50 + m_pCity->getThreatValue();

			if (bDesperate)
			{
				WarPenalty += 25;
			}
			if ((m_pCity->isCoastal() && m_pCity->IsBlockaded(true)) || ((!m_pCity->isCoastal() && m_pCity->IsBlockaded(false))))
			{
				WarPenalty += 25;
			}

			int iCityLoop;
			for (CvCity* pLoopCity = kPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iCityLoop))
			{
				if (pLoopCity->isUnderSiege() || pLoopCity->isInDangerOfFalling())
				{
					WarPenalty += 25;
				}
				if ((pLoopCity->isCoastal() && pLoopCity->IsBlockaded(true)) || ((!pLoopCity->isCoastal() && pLoopCity->IsBlockaded(false))))
				{
					WarPenalty += 25;
				}
			}

			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

				if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if (kPlayer.GetDiplomacyAI()->GetWarState(eLoopPlayer) < WAR_STATE_STALEMATE)
					{
						WarPenalty += 25;
					}
				}
			}

		}
		iBonus *= (100 - min(99, WarPenalty));
		iBonus /= 100;
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
				int iEraValue = (kPlayer.GetCurrentEra() - eEra);
				if (iEraValue > 0)
				{
					iBonus += (250 * iEraValue);
				}
			}
			//No Era? Zero!
			else
			{
				int iEraValue = kPlayer.GetCurrentEra(); //was 8
				if (iEraValue > 0)
				{
					iBonus += (250 * iEraValue);
				}
			}
		}
	}
	else
	{
		int iEraValue = kPlayer.GetCurrentEra(); //was 8
		if (iEraValue > 0)
		{
			iBonus += (250 * iEraValue);
		}
	}

	//UB?
	if (kPlayer.getCivilizationInfo().isCivilizationBuildingOverridden(pkBuildingInfo->GetBuildingClassType()))
	{
		// scale off with pop so UB will not be the first building to build in a fresh city
		if (pkBuildingInfo->IsNoOccupiedUnhappiness())
			iBonus += max(15, m_pCity->getPopulation()) * 25;
		else
			iBonus += m_pCity->getPopulation() * 25;
	}

	//Danger? Prioritize units!
	if (!bFreeBuilding && !bIgnoreSituational && bDesperate && iDefense <= 0)
	{
		iBonus -= 1000;
	}

	if (m_pCity->isProductionBuilding())
	{
		if (m_pCity->getProductionBuilding() == eBuilding)
			iBonus += max(1, 1000 - m_pCity->getProductionTurnsLeft());
		else
			iBonus = (iBonus/2);
	}

	/////
	///WEIGHT
	//////
	//Greatly reduce this if we're under siege somewhere.
	if (bInterruptBuildings && !m_pCity->IsPuppet())
	{
		iBonus /= 10;
	}

	if (iBonus <= 0)
		return 1;

	//iValue is the compunded value of the items.
	iValue += iBonus;

	return iValue;
}
#endif