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

template<typename BuildingProductionAI, typename Visitor>
void CvBuildingProductionAI::Serialize(BuildingProductionAI& buildingProductionAI, Visitor& visitor)
{
	visitor(buildingProductionAI.m_BuildingAIWeights);
}

/// Serialization read
void CvBuildingProductionAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvBuildingProductionAI::Write(FDataStream& kStream) const
{
	CvAssertMsg(m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");

	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& loadFrom, CvBuildingProductionAI& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}
FDataStream& operator<<(FDataStream& saveTo, const CvBuildingProductionAI& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
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
	int iNumLandConnection, int iNumSeaConnection, bool bNationalWonderCheck, bool bFreeBuilding, bool bIgnoreSituational)
{
	if(m_pCity == NULL)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if(kPlayer.isMinorCiv())
		return iValue;

	if(eBuilding == NO_BUILDING)
		return 0;

	if(iValue < 1)
		return 0;

	//this seems to work well to bring the raw flavor weight into a sensible range [0 ... 200]
	iValue = sqrti(10 * iValue);

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
		return 0;

	//Bonus additive. All values below will be added to this and combined with real value at end.
	int iBonus = 0;

	const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
	if (m_pCity->getPopulation() <= 6 && !m_pCity->isCapital())
	{
		if(isWorldWonderClass(kBuildingClassInfo) || isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
		{
			return 0;
		}
	}

	if (isNationalWonderClass(kBuildingClassInfo) || isWorldWonderClass(kBuildingClassInfo))
	{
		if (isWorldWonderClass(kBuildingClassInfo))
		{
			// Specific check to block non-founder AIs from building Hagia Sophia, Borobudur, Cathedral of St. Basil
			ReligionTypes eOurReligion = kPlayer.GetReligions()->GetStateReligion(false);
			if (eOurReligion == NO_RELIGION)
			{
				bool bCanFoundReligion = GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() > 0 || kPlayer.GetPlayerTraits()->IsAlwaysReligion();
				bool bSpawnsProphet = false;

				for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
				{
					const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
					CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
					if (pkUnitInfo && pkBuildingInfo->GetNumFreeUnits(iUnitLoop) > 0)
					{
						// Don't need Great Prophets if we can't found a religion
						if (pkUnitInfo->IsFoundReligion())
						{
							if (!bCanFoundReligion)
								return 0;

							bSpawnsProphet = true;
							break;
						}
						// Don't need Missionaries or Inquisitors if we aren't a founder
						else if (pkUnitInfo->IsSpreadReligion() || pkUnitInfo->IsRemoveHeresy())
						{
							return 0;
						}
					}
				}
				if (!bSpawnsProphet)
				{
					// None of this is useful if we don't have a religion.
					if (pkBuildingInfo->GetExtraMissionaryStrength() > 0 || pkBuildingInfo->GetExtraMissionarySpreads() > 0 || pkBuildingInfo->GetExtraMissionarySpreadsGlobal() > 0 || pkBuildingInfo->IsReformation())
					{
						return 0;
					}
				}
			}
		}

		if (!bNationalWonderCheck)
		{
			if (!m_pCity->IsBestForWonder(pkBuildingInfo->GetBuildingClassType()))
			{
				return 0;
			}
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

				if (GET_PLAYER(eLoopPlayer).isAlive() && GET_TEAM(kPlayer.getTeam()).isHasMet(GET_PLAYER(eLoopPlayer).getTeam()) && GET_PLAYER(eLoopPlayer).getBuildingClassMaking(pkBuildingInfo->GetBuildingClassType()) > 0)
				{
					iNumOthersConstructing++;
				}
			}
			iBonus -= iNumOthersConstructing * 150;

			//probably early game, so if we haven't started yet, we're probably not going to win this one.
			if (kPlayer.getNumCities() == 1)
			{
				iBonus -= iNumOthersConstructing * 100;
			}
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

	//Airlift
	if(pkBuildingInfo->IsAirlift())
	{
		if(!m_pCity->isCapital() && kPlayer.getCapitalCity() != NULL)
		{
			if(!m_pCity->HasSharedAreaWith(kPlayer.getCapitalCity(),true,true))	
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

	//this is not about trade routes but city connections
	if (pkBuildingInfo->AllowsWaterRoutes()) 
	{
		CvCity* pCapital = kPlayer.getCapitalCity();
		if(iNumSeaConnection <= 0 && m_pCity->IsRouteToCapitalConnected())
		{
			iBonus -= 50;
		}
		else if(pCapital != NULL && !pCapital->HasSharedAreaWith(m_pCity,true,true))
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

	//bonus to sea trade
	if (pkBuildingInfo->GetTradeRouteSeaDistanceModifier() > 0)
	{
		//can we even have sea trade here
		if (m_pCity->plot()->isCoastalLand() && GC.getGame().GetGameTrade()->GetAllPotentialTradeRoutesFromCity(m_pCity, true).size() > 0)
			iBonus += 10;
		else
			iBonus -= 100;
	}

	if (pkBuildingInfo->GetTradeRouteSeaGoldBonus() > 0 || pkBuildingInfo->GetSeaTourismEnd() > 0)
	{
		//Higher value the higher the number of routes.
		iBonus += 5 * iNumSeaConnection;
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
			//Building uses resources? Not if we're a puppet or automated, thanks! 
			if(pkBuildingInfo->GetResourceQuantityRequirement(eResource) > 0 && (CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity) || m_pCity->isHumanAutomated()))
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
		iDefense += (pkBuildingInfo->GetDefenseModifier() / 25);
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
		iDefense += (pkBuildingInfo->GetExtraCityHitPoints() / 10);
	}
	if (m_pCity->isCoastal())
	{
		AICityStrategyTypes eStrategyLakeBound = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_LAKEBOUND");
		if (!m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyLakeBound))
		{
			if (pkBuildingInfo->GetBorderObstacleWater() > 0)
			{
				iDefense += 25;
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
		if (pkBuildingInfo->GetDeepWaterTileDamage() > 0)
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
	if (bDesperate)
		iDefenseMod += 1000;

	//do not build any buildings at all when about to be captured
	if (m_pCity->isInDangerOfFalling())
		return 0;

	if (iDefense == 0 && m_pCity->isUnderSiege() && !bIgnoreSituational)
		//do not build any non-defensive buildings when under siege
		return 0;

	iDefense *= iDefenseMod;
	iDefense /= 100;

	iBonus += iDefense;

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


	//unfortunately we have to loop through all buildings in the game to do this. Sigh...
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		int iNumNeeded = kPlayer.getBuildingClassPrereqBuilding((BuildingTypes)iI, pkBuildingInfo->GetBuildingClassType());
		//need in all?
		if (iNumNeeded > 0)
		{
			int iNumHave = kPlayer.getNumBuildings(eBuilding);
			iBonus += iNumNeeded * max(1, iNumHave) * 100;
		}
	}

	//unlocks a unit?
	for (int i = 0; i < GC.getNumUnitInfos(); i++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(i);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if (pkUnitInfo && pkUnitInfo->GetBuildingClassRequireds(pkBuildingInfo->GetBuildingClassType()))
		{
			if (pkUnitInfo->GetPrereqAndTech() == NO_TECH)
				iBonus += 1000;
			else if (kPlayer.HasTech((TechTypes)pkUnitInfo->GetPrereqAndTech()))
				iBonus += 1000;
			else
				iBonus += 50;
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

	///////////////////
	//Military Stuff
	////////////////////////

	//Domain mods are good, and should be stacked.
	if (!CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity))
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
	if (!CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity))
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
	int iAvgGPT = kPlayer.GetTreasury()->AverageIncome100(10) / 100;
	YieldTypes eFocusYield = m_pCity->GetCityCitizens()->GetFocusTypeYield(m_pCity->GetCityCitizens()->GetFocusType());

	int iTotalYield = 0;
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

		int iFlatYield = 0;
		int iYieldValue = CityStrategyAIHelpers::GetBuildingYieldValue(m_pCity, eBuilding, eYield, iFlatYield);
		iTotalYield += iFlatYield;

		int iYieldTrait = CityStrategyAIHelpers::GetBuildingTraitValue(m_pCity, eYield, eBuilding, iYieldValue);
		int iHappinessReduction = pkBuildingInfo->GetUnhappinessNeedsFlatReduction(eYield);

		if ((iYieldValue > 0) || (iYieldTrait > 0) || (iHappinessReduction > 0))
		{
			switch (eYield)
			{
			case YIELD_GOLD:
				if (iAvgGPT < 0)
				{
					iYieldValue += (iAvgGPT * -10);
					iYieldTrait += (iAvgGPT * -10);
					bGoodforHappiness = true;
					bGoodforGPT = true;
				}
				if (iHappinessReduction > 0)
				{
					iYieldValue += iHappinessReduction * 100;
					bGoodforHappiness = true;
					bGoodforGPT = true;
				}
				break;
			case YIELD_SCIENCE:
				if (iHappinessReduction > 0)
				{
					iYieldValue += iHappinessReduction * 100;
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
				if (iHappinessReduction > 0)
				{
					iYieldValue += iHappinessReduction * 100;

					bGoodforHappiness = true;
				}

				//explicit check for stonehenge.
				if (pkBuildingInfo->GetInstantYield(YIELD_FAITH) > 0 && kPlayer.GetReligions()->HasCreatedPantheon() && kPlayer.GetCurrentEra() <= 3)
					iReligion /= 50;

				break;
			case YIELD_CULTURE:
				if (iHappinessReduction > 0)
				{
					iYieldValue += iHappinessReduction * 100;
					bGoodforHappiness = true;
				}
				break;
			case YIELD_PRODUCTION:
				if (bSmall)
				{
					iYieldValue *= 4;
					iYieldTrait *= 4;
				}
				if (iHappinessReduction > 0)
				{
					iYieldValue += iHappinessReduction * 100;
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

				if (iHappinessReduction > 0)
				{
					iYieldValue += iHappinessReduction * 100;
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
		}

		iBonus += iYieldValue;
		iBonus += iYieldTrait;
	}

	//////////////
	///Maintenance
	/////////////////////
	if (pkBuildingInfo->GetGoldMaintenance() > 0 && !bCourthouse && !bGoodforGPT)
	{
		//careful if it would bankrupt us
		if (kPlayer.getTurnsToBankruptcy(pkBuildingInfo->GetGoldMaintenance()) < 9)
		{
			return 0;
		}

		/*
		// it would be useful to have a check whether a building is worth building at all (eg skip hotel in a city without culture)
		// however yields are not everything (eg growth bonus, resources etc). so it's difficult to write down a good condition ...
		if (iTotalYield < pkBuildingInfo->GetGoldMaintenance() * 3 && pkBuildingInfo->GetDefenseModifier() == 0)
		{
			return 0;
		}
		*/
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
		if (iNumWar > 0 && pkBuildingInfo->GetDefenseModifier() <= 0 && !CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity) && !bFreeBuilding && !kPlayer.IsEmpireVeryUnhappy())
		{
			WarPenalty += 50 + m_pCity->getThreatValue();

			if (bDesperate)
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

	int iEra = 0; //default
	TechTypes eTech = (TechTypes)pkBuildingInfo->GetPrereqAndTech();
	if (eTech != NO_TECH)
	{
		CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eTech);
		if (pEntry)
		{
			EraTypes eEra = (EraTypes)pEntry->GetEra();
			if (eEra != NO_ERA)
				iEra = pEntry->GetEra();
		}
	}
	int iEraValue =  max(1, kPlayer.GetCurrentEra() - iEra);
	iBonus += (200 * iEraValue);

	//Unlocks another building?
	int iPrereqChain = kPlayer.GetChainLength(eBuilding);
	if (iPrereqChain > 0)
		iBonus += iPrereqChain * 100 * iEraValue;

	//UB?
	if (kPlayer.getCivilizationInfo().isCivilizationBuildingOverridden(pkBuildingInfo->GetBuildingClassType()))
	{
		// scale off with pop so UB will not be the first building to build in a fresh city
		if (pkBuildingInfo->IsNoOccupiedUnhappiness())
			iBonus += max(15, m_pCity->getPopulation()) * 25;
		else
			iBonus += m_pCity->getPopulation() * 25;
	}

	/////
	///WEIGHT
	//////
	if (iBonus < 1)
		return 0;

	//iValue is the compunded value of the items.
	return iValue + iBonus;
}
#endif