/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvUnitProductionAI.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#if defined(MOD_BALANCE_CORE)
#include "CvMilitaryAI.h"
#include "CvEconomicAI.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"
#endif
// include this after all other headers
#include "LintFree.h"


/// Constructor
CvUnitProductionAI::CvUnitProductionAI(CvCity* pCity,  CvUnitXMLEntries* pUnits):
	m_pCity(pCity),
	m_pUnits(pUnits)
{
}

/// Destructor
CvUnitProductionAI::~CvUnitProductionAI(void)
{
}

/// Clear out AI local variables
void CvUnitProductionAI::Reset()
{
	ASSERT_DEBUG(m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");

	m_UnitAIWeights.clear();

	// Loop through reading each one and add an entry with 0 weight to our vector
	if(m_pUnits)
	{
		for(int i = 0; i < m_pUnits->GetNumUnits(); i++)
		{
			m_UnitAIWeights.push_back(i, 0);
		}
	}
}

template<typename UnitProductionAI, typename Visitor>
void CvUnitProductionAI::Serialize(UnitProductionAI& unitProductionAI, Visitor& visitor)
{
	visitor(unitProductionAI.m_UnitAIWeights);
}

/// Serialization read
void CvUnitProductionAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvUnitProductionAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& loadFrom, CvUnitProductionAI& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}
FDataStream& operator<<(FDataStream& saveTo, const CvUnitProductionAI& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvUnitProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	if (iWeight==0)
		return;

	// Loop through all units
	for(int iUnit = 0; iUnit < m_pUnits->GetNumUnits(); iUnit++)
	{
		CvUnitEntry* entry = m_pUnits->GetEntry(iUnit);
		if(entry)
		{
			// Set its weight by looking at unit's weight for this flavor and using iWeight multiplier passed in
			m_UnitAIWeights.IncreaseWeight(iUnit, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}

/// Retrieve sum of weights on one item
int CvUnitProductionAI::GetWeight(UnitTypes eUnit)
{
	//dirty hack to allow barbarian cities to build units ... barbarians don't use flavors, so their weights are all zero
	if (m_pCity->getOwner() == BARBARIAN_PLAYER)
		return 1;

	return m_UnitAIWeights.GetWeight(eUnit);
}

/// Recommend highest-weighted unit
UnitTypes CvUnitProductionAI::RecommendUnit(UnitAITypes eUnitAIType, bool bAllowStrategicResource)
{
	if(eUnitAIType <= NO_UNITAI)
	{
		return NO_UNIT;
	}

	// Reset list of all the possible units
	m_Buildables.clear();

	// Loop through adding the available units
	for(int iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			// Make sure it matches the requested unit AI type
			if (eUnitAIType != NO_UNITAI && !pkUnitInfo->GetUnitAIType(eUnitAIType))
				continue;

			if (!bAllowStrategicResource && pkUnitInfo->GetResourceType() != NO_RESOURCE)
				continue;

			// Make sure this unit can be built now
			if(m_pCity->canTrain(eUnit, (m_pCity->isProductionUnit() && eUnit == m_pCity->getProductionUnit())))
			{
				// Update weight based on turns to construct
				int iTurnsLeft = m_pCity->getProductionTurnsLeft(eUnit, 0);
				int iTempWeight = m_UnitAIWeights.GetWeight(eUnit);
				if (iTempWeight == 0)
				{
					const int iNumFlavors = GC.getNumFlavorTypes();
					for (int iFlavorLoop = 0; iFlavorLoop < iNumFlavors; iFlavorLoop++)
					{
						iTempWeight += pkUnitInfo->GetFlavorValue(iFlavorLoop);
					}
				}

				int iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(iTempWeight, iTurnsLeft);
				if(iWeight > 0)
					m_Buildables.push_back(iUnitLoop, iWeight);
			}
		}

	}

	// Sort items and grab the first one
	if(m_Buildables.size() > 0)
	{
		m_Buildables.StableSortItems();
		LogPossibleBuilds(eUnitAIType);
		return (UnitTypes)m_Buildables.GetElement(0);
	}

	// Unless we didn't find any
	else
	{
		return NO_UNIT;
	}
}

#if defined(MOD_BALANCE_CORE)
int CvUnitProductionAI::CheckUnitBuildSanity(UnitTypes eUnit, bool bForOperation, int iTempWeight, bool bForPurchase, bool bFree)
{
	//value to be added
	int iBonus = 0;

	if(iTempWeight < 1)
		return SR_IMPOSSIBLE;

	//this seems to work well to bring the raw flavor weight into a sensible range [0 ... 200]
	iTempWeight = sqrti(10 * iTempWeight);

	CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
	if (!pkUnitEntry)
		return SR_IMPOSSIBLE;

	bool bCombat = (pkUnitEntry->GetCombat() > 0 || pkUnitEntry->GetRangedCombat() > 0);

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());
	if (!bFree && bForPurchase && !m_pCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_GOLD))
	{
		return SR_IMPOSSIBLE;
	}

	if (!bFree && !bForPurchase && m_pCity->IsPuppet())
	{
		return SR_IMPOSSIBLE;
	}

	bool bDesperate = m_pCity->isUnderSiege();
	if (!bFree && bDesperate && !bCombat)
	{
		return SR_STRATEGY;
	}

	// can we build an upgraded version of this unit in any of our cities?
	if (!kPlayer.GetUnitClassTrainingAllowedAnywhere().empty())
	{
		const set<UnitClassTypes>& sUnitClassTrainingAllowed = kPlayer.GetUnitClassTrainingAllowedAnywhere();
		for (set<UnitClassTypes>::const_iterator it = sUnitClassTrainingAllowed.begin(); it != sUnitClassTrainingAllowed.end(); ++it)
		{
			if (pkUnitEntry->GetUpgradeUnitClass(*it))
			{
				return SR_STRATEGY;
			}
		}
	}

	if (!bDesperate && !bFree)
	{
		if (bCombat && !pkUnitEntry->IsNoMaintenance() && !pkUnitEntry->IsTrade())
		{
			int iGoldSpentOnUnits = kPlayer.GetTreasury()->GetExpensePerTurnUnitMaintenance();
			int iAverageGoldPerUnit = iGoldSpentOnUnits / (max(1, kPlayer.getNumUnits()));
			int iTurnsLeft = kPlayer.getTurnsToBankruptcy(iAverageGoldPerUnit);

			if (iTurnsLeft < 9)
				return SR_MAINTENANCE;
			else if (iTurnsLeft < 17)
				iBonus -= iAverageGoldPerUnit*25;
		}
	}

	//don't build land/sea units if there's no place to put them
	if (bCombat && m_pCity->HasGarrison() && pkUnitEntry->GetDomainType() != DOMAIN_AIR)
	{
		CvPlot* pCenter = m_pCity->plot();
		bool bHavePlot = false;
		for (int i = RING0_PLOTS; i < RING3_PLOTS && !bHavePlot; i++)
		{
			CvPlot* pNeighbor = iterateRingPlots(pCenter, i);
			if (pNeighbor && pNeighbor->isValidMovePlot(m_pCity->getOwner()) && pNeighbor->GetNumCombatUnits() == 0)
			{
				if (pNeighbor->isWater() && pkUnitEntry->GetDomainType() == DOMAIN_SEA)
					bHavePlot=true;

				if (!pNeighbor->isWater() && pkUnitEntry->GetDomainType() == DOMAIN_LAND)
					bHavePlot=true;
			}
		}

		if (!bHavePlot)
			return SR_IMPOSSIBLE;
	}

	int iNumLandUnits = kPlayer.getNumMilitaryLandUnits() + kPlayer.GetNumUnitsInProduction(DOMAIN_LAND, true);
	int iNumSeaUnits = kPlayer.getNumMilitarySeaUnits() + kPlayer.GetNumUnitsInProduction(DOMAIN_SEA, true);
	int iNumTotalUnits = iNumLandUnits + iNumSeaUnits;
	int iNumTotalUnitsToSupply = iNumTotalUnits - kPlayer.getNumUnitsSupplyFree();

	//only war with majors count
	bool bAtWar = (kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0);
	if (!bFree && kPlayer.isMinorCiv())
	{
		if (bCombat)
		{
			if (iNumTotalUnits > max(4, ((kPlayer.GetCurrentEra() + 3) * max(1, kPlayer.getNumCities()))))
				return SR_UNITSUPPLY;

			if (pkUnitEntry->GetDomainType() == DOMAIN_SEA)
			{
				if (iNumSeaUnits <= 2)
					iBonus += max(0, 300 - (iNumSeaUnits * 100));

				int iLimit = kPlayer.GetMilitaryAI()->GetRecommendNavySize();
				if (iNumSeaUnits < iLimit)
				{
					iBonus += (iLimit - iNumSeaUnits) * 100;
					if (kPlayer.GetMinorCivAI()->IsRecentlyBulliedByAnyMajor())
					{
						iBonus += 100;
					}
				}
				else
					return SR_UNITSUPPLY;
			}
			else if (pkUnitEntry->GetDomainType() == DOMAIN_LAND)
			{
				if (iNumLandUnits <= 2)
					iBonus += max(0, 800 - (iNumLandUnits * 100));

				int iLimit = kPlayer.GetMilitaryAI()->GetRecommendLandArmySize();
				if (iNumLandUnits < iLimit)
				{
					iBonus += (iLimit - iNumLandUnits) * 150;
					if (kPlayer.GetMinorCivAI()->IsRecentlyBulliedByAnyMajor())
					{
						iBonus += 250;
					}
				}
				else
					return SR_UNITSUPPLY;
			}
		}
	}

	//Let's check this against supply to keep our military numbers lean.
	int iScale = 0;
	if (bCombat && !bFree)
	{
		int iSupply = max(1, kPlayer.GetNumUnitsSupplied());
		int iDemand = iNumTotalUnitsToSupply;
		if (bAtWar || bForOperation)
		{
			//hard limit, don't go too far into negative supply
			if (iSupply <= iDemand - 3)
				return SR_UNITSUPPLY;

			//reduce bonus once we're over the limit
			iScale = MapToPercent(iDemand, iSupply + 3, iSupply - 1);
		}
		else
		{
			//reduce bonus once we're approaching the limit
			iScale = MapToPercent(iDemand, iSupply, (iSupply * 2) / 3);

			//don't exceed the limit
			if (iSupply <= iDemand)
				return SR_UNITSUPPLY;
		}
	}

	//Are we alone?
	DomainTypes eDomain = pkUnitEntry->GetDomainType();
	if (!bFree && bCombat)
	{
		CvLandmass* pLM = GC.getMap().getLandmassById(m_pCity->plot()->getLandmass());
		if(pLM != NULL && pLM->getNumTiles() <= 3)
		{
			if (eDomain == DOMAIN_LAND && m_pCity->HasGarrison() && !bForOperation)
			{
				return SR_USELESS;
			}
		}
	}

	
	//////////////////
	//DOMAINS AND LOCATION CHECKS
	////////////////////////

	// sanity check for building ships on isolated small inland seas (not lakes)
	// if needed allow workboats...
	if (eDomain == DOMAIN_SEA && pkUnitEntry->GetCombat() > 0) 
	{
		int iWaterTiles = 0;
		int iNumUnitsofMine = 0;
		int iNumUnitsOther = 0;
		int iNumCitiesofMine = 0;
		int iNumCitiesOther = 0;

		//todo: alternatively check whether we can create a sea trade route to another player?
		std::vector<int> landmasses = m_pCity->plot()->getAllAdjacentLandmasses();
		for (std::vector<int>::iterator it=landmasses.begin(); it!=landmasses.end(); ++it)
		{
			CvLandmass* pkLandmass = GC.getMap().getLandmassById(*it);
			if (pkLandmass->isWater())
			{
				iWaterTiles += pkLandmass->getNumTiles();
				iNumUnitsofMine += pkLandmass->getUnitsPerPlayer(m_pCity->getOwner());
				iNumUnitsOther += pkLandmass->getNumUnits()-iNumUnitsofMine;
				iNumCitiesofMine += pkLandmass->getCitiesPerPlayer(m_pCity->getOwner());
				iNumCitiesOther += pkLandmass->getNumCities()-iNumCitiesofMine;
			}
		}

		int iFactor = /*6*/ GD_INT_GET(AI_CONFIG_MILITARY_TILES_PER_SHIP);
		//Are we mustering a naval attack here?
		bool bOperationalOverride = (bForOperation && kPlayer.IsMusterCityForOperation(m_pCity, true));
		bool bTooManyUnits = (iNumUnitsofMine * iFactor > iWaterTiles);
		bool bNoEnemies = (iNumUnitsOther < 1 && iNumCitiesOther < 1);

		if (!bFree && !bOperationalOverride && (bTooManyUnits || bNoEnemies))
		{
			return SR_USELESS;
		}
	}
	if(!kPlayer.isMinorCiv())
	{
		/////////
		//RESOURCE CHECKS
		////////////
		//Check for special unlimited units from specific resources (not a strategic check)
		if(pkUnitEntry->GetResourceType() != NO_RESOURCE)
		{
			if(kPlayer.getNumResourceAvailable((ResourceTypes)pkUnitEntry->GetResourceType(), false) > 0)
			{
				iBonus += 5 * kPlayer.getNumResourceAvailable((ResourceTypes)pkUnitEntry->GetResourceType(), false);
			}
			else
			{
				return SR_IMPOSSIBLE;
			}
		}
		//Check for specific resource usage by units.
		if(pkUnitEntry->GetSpaceshipProject() == NO_PROJECT)
		{
			if(kPlayer.GetDiplomacyAI()->IsGoingForSpaceshipVictory())
			{
				//Aluminum Check
				static ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
				if(pkUnitEntry->GetResourceQuantityRequirement(eAluminumResource) > 0)
				{
					if(kPlayer.getNumResourceAvailable(eAluminumResource, false) <= (kPlayer.GetNumAluminumStillNeededForSpaceship() + kPlayer.GetNumAluminumStillNeededForCoreCities()))
					{
						return SR_STRATEGY;
					}
				}
			}

			ResourceTypes eNeededResource = (ResourceTypes)pkUnitEntry->GetResourceType();
			if (eNeededResource != NO_RESOURCE)
			{	
				//if we have a lot of the resource, we should spend it ... the units are usually good
				iBonus += max(40, (kPlayer.getNumResourceAvailable(eNeededResource, false) - pkUnitEntry->GetResourceQuantityRequirement(eNeededResource)) * 40);
			}
		}

		///////////////
		//UNIT TYPE CHECKS
		//////////////////////
		//Virtually force this.
		if (m_pCity->IsUnitInvestment((UnitClassTypes)pkUnitEntry->GetUnitClassType()))
		{
			iBonus += 500;
		}

		//Sanity check for buildable support units.
		if (!bFree && pkUnitEntry->IsCityAttackSupport() && !bForOperation)
		{
			int iTotalAlready = kPlayer.GetNumUnitsWithUnitAI(pkUnitEntry->GetDefaultUnitAIType(), false);
			int iNum = kPlayer.GetNumUnitsWithUnitAI(UNITAI_CITY_BOMBARD, true);
			if (iNum < iTotalAlready)
			{
				return SR_USELESS;
			}
			else
			{
				iBonus += 10 * (5 - iTotalAlready);
			}
		}

		//Suicide units? Hmm...
		if(pkUnitEntry->IsSuicide())
		{
			//Nukes!!!
			if(pkUnitEntry->GetNukeDamageLevel() > 0)
			{
				iBonus += 100;
			}
			//Cruise Missiles? Only if we don't have any nukes lying around...
			else if(pkUnitEntry->GetRangedCombat() > 0 && kPlayer.getNumNukeUnits() > 0)
			{
				iBonus -= 50;
			}
			else
			{
				iBonus += 25;
			}
		}


		//Policy unlock or move-on-purchase? These are usually cheap and good, so get em!
		if(pkUnitEntry->GetPolicyType() != NO_POLICY || pkUnitEntry->CanMoveAfterPurchase())
		{
			if(bCombat && bAtWar)
			{
				int iGoldCost = m_pCity->GetPurchaseCost(eUnit);
				int iSurplus = ((kPlayer.GetTreasury()->GetGold() - iGoldCost) / 10);
				if(iSurplus > 0)
				{
					//Bonus based on difference in gold - the more money we have, the more we want this!
					iBonus += min(250, iSurplus);
				}
			}
			else if(bCombat && !bAtWar)
			{
				int iGoldCost = m_pCity->GetPurchaseCost(eUnit);
				int iSurplus = ((kPlayer.GetTreasury()->GetGold() - iGoldCost) / 25);
				if(iSurplus > 0)
				{
					//Bonus based on difference in gold - the more money we have, the more we want this!
					iBonus += min(200, iSurplus);
				}
			}
			else if(!bCombat)
			{
				int iGoldCost = m_pCity->GetPurchaseCost(eUnit);
				int iSurplus = ((kPlayer.GetTreasury()->GetGold() - iGoldCost) / 50);
				if(iSurplus > 0)
				{
					//Bonus based on difference in gold - the more money we have, the more we want this!
					iBonus += min(150, iSurplus);
				}
			}
		}

		//Carriers? Only if we need them.
		if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_CARRIER_SEA)
		{
			if (kPlayer.isMinorCiv())
				return SR_USELESS;

			if (!bForOperation && MilitaryAIHelpers::IsTestStrategy_NeedAirCarriers(&kPlayer)==false)
				return SR_BALANCE;
		}

		//Need Explorers?
		if (eDomain == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE)
		{
			int iExplorersNeeded = kPlayer.GetEconomicAI()->GetExplorersNeeded();
			int iExplorersHave = kPlayer.GetNumUnitsWithUnitAI(UNITAI_EXPLORE, true);
			if (m_pCity->isProductionUnit() && m_pCity->getProductionUnit() == eUnit)
				iExplorersHave--;

			int iExploreBonus = iExplorersNeeded - iExplorersHave;
			if (iExploreBonus > 0)
			{
				iBonus += iExploreBonus * 500;
			}
		}

		//Need Sea Explorers?
		if (EconomicAIHelpers::IsPotentialNavalExplorer(pkUnitEntry->GetDefaultUnitAIType()))
		{
			int iExplorersNeeded = kPlayer.GetEconomicAI()->GetNavalExplorersNeeded(); //this will always return at least one!
			int iExplorersHave = kPlayer.GetNumUnitsWithUnitAI(UNITAI_EXPLORE_SEA, true);
			int iExplorersIdle = kPlayer.GetNumUnitsWithUnitAI(pkUnitEntry->GetDefaultUnitAIType(), true);

			if (iExplorersNeeded > iExplorersHave + iExplorersIdle)
				iBonus += 500;

		}
		//Naval Units Critically Needed?
		if (eDomain == DOMAIN_SEA)
		{
			if(bCombat)
			{
				int iCurrent = kPlayer.GetMilitaryAI()->GetNumNavalUnits();
				int iDesired = kPlayer.GetMilitaryAI()->GetRecommendNavySize();
				int iValue = iDesired - iCurrent;

				// German UA encourages producing lots of units to gift to City-States
				if (iValue > 0 && kPlayer.GetPlayerTraits()->GetMinorInfluencePerGiftedUnit() > 0)
					iBonus += 75;

				iValue *= max(1, (int)kPlayer.GetCurrentEra());

				if (iCurrent * 2 < iDesired)
					iValue *= 2;

				if (iValue > 0 && !kPlayer.isBarbarian())
				{
					//emphasize navy if there is nobody to attack over land
					if (MilitaryAIHelpers::IsTestStrategy_NeedNavalUnitsCritical(&kPlayer))
						iValue *= 5;
					else if (MilitaryAIHelpers::IsTestStrategy_NeedNavalUnits(&kPlayer))
						iValue *= 2;

					const CivsList& warPlayers = kPlayer.GetPlayersAtWarWith();
					vector<PlayerTypes> vUnfriendlyMajors = kPlayer.GetUnfriendlyMajors();
					int iNumPlayers = (int)warPlayers.size();
					int iWarValue = 0;
					for (size_t i=0; i<warPlayers.size(); i++)
					{
						PlayerTypes eLoopPlayer = warPlayers[i];

						if (kPlayer.GetMilitaryAI()->GetWarType(eLoopPlayer) == WARTYPE_SEA)
						{
							if (std::find(vUnfriendlyMajors.begin(), vUnfriendlyMajors.end(), eLoopPlayer) != vUnfriendlyMajors.end())
								iWarValue += 4;
							else
								iWarValue += 2;
						}
						else
						{
							iWarValue += 1;
						}
					}

					iWarValue /= max(1, iNumPlayers);

					if (kPlayer.GetMilitaryAI()->GetWarType() == WARTYPE_SEA)
					{
						iWarValue += 10;
					}

					if (iWarValue > 0)
					{
						iValue *= iWarValue;
					}
				}
				iBonus += iValue;
			}
		}
		//Land Units Critically Needed?
		else if (eDomain == DOMAIN_LAND)
		{
			if (bCombat)
			{
				int iCurrent = kPlayer.GetMilitaryAI()->GetNumLandUnits();
				int iDesired = kPlayer.GetMilitaryAI()->GetRecommendLandArmySize();
				int iValue = iDesired - iCurrent;

				// German UA encourages producing lots of units to gift to City-States
				if (iValue > 0 && kPlayer.GetPlayerTraits()->GetMinorInfluencePerGiftedUnit() > 0)
					iBonus += 150;

				iValue *= max(1, (int)kPlayer.GetCurrentEra());

				if (iValue > 0 && !kPlayer.isBarbarian())
				{
					const CivsList& warPlayers = kPlayer.GetPlayersAtWarWith();
					vector<PlayerTypes> vUnfriendlyMajors = kPlayer.GetUnfriendlyMajors();
					int iNumPlayers = (int)warPlayers.size();
					int iWarValue = 0;
					for (size_t i=0; i<warPlayers.size(); i++)
					{
						PlayerTypes eLoopPlayer = warPlayers[i];

						if (kPlayer.GetMilitaryAI()->GetWarType(eLoopPlayer) == WARTYPE_LAND)
						{
							if (std::find(vUnfriendlyMajors.begin(), vUnfriendlyMajors.end(), eLoopPlayer) != vUnfriendlyMajors.end())
								iWarValue += 4;
							else
								iWarValue += 2;
						}
						else
						{
							iWarValue += 1;
						}
					}

					iWarValue /= max(1, iNumPlayers);

					if (kPlayer.GetMilitaryAI()->GetWarType() == WARTYPE_LAND)
					{
						iWarValue += 15;
					}

					if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_DEFENSE || (!MOD_AI_UNIT_PRODUCTION && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_COUNTER) || pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ATTACK)
					{
						CvUnit* pLoopUnit2 = NULL;
						for (int iUnitLoop = 0; iUnitLoop < m_pCity->plot()->getNumUnits(); iUnitLoop++)
						{
							pLoopUnit2 = m_pCity->plot()->getUnitByIndex(iUnitLoop);
							if (pLoopUnit2->isFound() && pLoopUnit2->getArmyID() == -1)
							{
								iWarValue += 2;
							}
							else if (pLoopUnit2->isFound() && pLoopUnit2->getArmyID() != -1)
							{
								iWarValue += 1;
							}
						}
					}

					if (iWarValue > 0)
					{
						iValue *= iWarValue;
					}
				}
				iBonus += iValue;
			}
		}

		if (eDomain == DOMAIN_AIR)
		{
			//if we can build an airforce do so, independent of other players
			//just take care that it's approximately evenly split between attack and defense
			int ourBombers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_ATTACK_AIR, false);
			int ourFighters = kPlayer.GetNumUnitsWithUnitAI(UNITAI_DEFENSE_AIR, false);
			int emptySlots = m_pCity->GetMaxAirUnits() - m_pCity->plot()->countNumAirUnits(kPlayer.getTeam(), true);

			switch (pkUnitEntry->GetDefaultUnitAIType())
			{
			case UNITAI_ATTACK_AIR:
			{
				if (ourBombers * 2 > ourFighters * 3)
					return SR_BALANCE;

				iBonus += 2000 + emptySlots*100;
				break;
			}
			case UNITAI_DEFENSE_AIR:
			{
				if (ourFighters * 2 > ourBombers * 3)
					return SR_BALANCE;

				iBonus += 2000 + emptySlots*100;
				break;
			}
			default: //missiles etc
				iBonus += 1000;
			}
		}

		//Air defense needed?
		if ((eDomain == DOMAIN_LAND && pkUnitEntry->GetAirInterceptRange() > 0) || pkUnitEntry->GetBaseLandAirDefense() > 20)
		{
			int iNeedAir = 0;
			int iNumAA = kPlayer.GetMilitaryAI()->GetNumAAUnits();
			int iOurAir =  + kPlayer.GetNumUnitsWithUnitAI(UNITAI_ATTACK_AIR, false) + kPlayer.GetNumUnitsWithUnitAI(UNITAI_DEFENSE_AIR, false);
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

				if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && kPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && 
					(kPlayer.GetProximityToPlayer(eLoopPlayer) == PLAYER_PROXIMITY_NEIGHBORS || GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam())))
				{
					int iTheirAir = GET_PLAYER(eLoopPlayer).GetNumUnitsWithUnitAI(UNITAI_DEFENSE_AIR, false) + GET_PLAYER(eLoopPlayer).GetNumUnitsWithUnitAI(UNITAI_ATTACK_AIR, false);

					//they have no air units? Ignore!
					if (iTheirAir == 0)
						continue;

					//not enough AA?
					if (iNumAA < kPlayer.getNumCities())
						iNeedAir += kPlayer.getNumCities();

					//they're superior to us overall? Get on it!
					if (iTheirAir >= iOurAir)
						iNeedAir += kPlayer.getNumCities();
				}
			}

			iBonus += iNeedAir * 50;
		}
	
		/////////////
		//GRAND STRATEGY CHECKS
		//////////////////
		static EconomicAIStrategyTypes eStrategyConquest = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CONQUEST");
		if(pkUnitEntry->GetCombat() > 0)
		{
			if(eStrategyConquest != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
			{
				iBonus += 50;
			}
		}

		int iProductionBonus = kPlayer.GetUnitClassProductionModifier((UnitClassTypes)pkUnitEntry->GetUnitClassType());
		if (iProductionBonus > 0)
		{
			iBonus += (iProductionBonus / 5);
		}

		//Diplomatic Units!
		if (MOD_BALANCE_VP && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_MESSENGER)
		{
			if (!kPlayer.HasMetValidMinorCiv())
				return SR_USELESS;

			//There's a diplomat waiting here? Abort!
			if (m_pCity->plot()->getNumUnitsOfAIType(UNITAI_MESSENGER, m_pCity->getOwner()) > 0)
			{
				return SR_USELESS;
			}

			//There's 2 sitting around? Abort!
			if (kPlayer.GetNumUnitsWithUnitAI(UNITAI_MESSENGER, true) > 3)
			{
				return SR_USELESS;
			}

			int iInfluence = 0;
			//Promotion Bonus
			for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				PromotionTypes ePromotion = (PromotionTypes)iI;
				if (pkUnitEntry->GetFreePromotions(iI))
					iInfluence += GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence();

				if (kPlayer.GetFreePromotionCount((PromotionTypes)iI))
					iInfluence += GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence();
			}

			//should not iterate all promotion types ... this way is faster
			vector<PromotionTypes> freePromotions = m_pCity->getFreePromotions();
			for (size_t iI = 0; iI < freePromotions.size(); iI++)
			{
				iInfluence += GC.getPromotionInfo(freePromotions[iI])->GetDiploMissionInfluence();
			}

			static EconomicAIStrategyTypes eNeedDiplomats = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS");
			static EconomicAIStrategyTypes eNeedDiplomatsCrit = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");
			if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedDiplomats))
			{
				iInfluence *= 2;
			}
			else if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedDiplomatsCrit))
			{
				iInfluence *= 3;
			}
			else
			{
				return SR_USELESS;
			}

			if (MOD_BALANCE_VP)
			{
				static ResourceTypes ePaper = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_PAPER", true);
				if (ePaper != NO_RESOURCE && kPlayer.getResourceFromCSAlliances(ePaper) > 0)
				{
					iInfluence *= 2;
				}
			}

			iBonus += iInfluence;
		}

		if(pkUnitEntry->GetSpaceshipProject() != NO_PROJECT)
		{
			// if we're an AI player going for spaceship victory, spaceship production is controlled in AI_doSpaceshipProduction, overriding normal AI production selection. other cities should not start building spaceship parts on their own
			if (!kPlayer.isHuman() && !kPlayer.isMinorCiv() && kPlayer.GetDiplomacyAI()->IsGoingForSpaceshipVictory())
			{
				return SR_STRATEGY;
			}

			static EconomicAIStrategyTypes eStrategySS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP");
			if (eStrategySS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategySS))
			{
				iBonus += 10000;
			}
			else
			{
				iBonus += 1000;
			}
			if(kPlayer.GetDiplomacyAI()->IsCloseToSpaceshipVictory())
			{
				iBonus += 10000;
			}
			else
			{
				iBonus += 2500;
			}

			if(m_pCity->getSpaceProductionModifier() > 0)
			{
				iBonus += (m_pCity->getSpaceProductionModifier() * 100);
			}
			//Don't zero out because of this penalty.
			if (iBonus <= 0)
				iBonus = 1;
		}

		/////////
		//RELIGION CHECKS
		////////////
		if(pkUnitEntry->GetCombat() > 0)
		{
			int iReligiousBonus = 0;
			ReligionTypes eReligion = GET_PLAYER(m_pCity->getOwner()).GetReligions()->GetStateReligion();

			if (eReligion != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pCity->getOwner());
				if (pReligion)
				{
					CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
					for (size_t iI = 0; iI < pReligion->m_Beliefs.GetBeliefList().size(); iI++)
					{
						const BeliefTypes eBelief = (BeliefTypes) pReligion->m_Beliefs.GetBeliefList()[iI];
						CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
						if (pEntry && pReligion->m_Beliefs.HasBelief(eBelief) && pReligion->m_Beliefs.IsBeliefValid(eBelief, eReligion, m_pCity->getOwner()))
						{
							if (pEntry->GetFaithFromKills() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK)))
							{
								iReligiousBonus += (pEntry->GetFaithFromKills() / 5);
							}
							if (pEntry->GetCombatModifierEnemyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_SKIRMISHER) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
							{
								iReligiousBonus += (pEntry->GetCombatModifierEnemyCities());
							}
							if (pEntry->GetCombatModifierFriendlyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
							{
								iReligiousBonus += (pEntry->GetCombatModifierFriendlyCities());
							}
							if (pEntry->GetCombatVersusOtherReligionOwnLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
							{
								iReligiousBonus += (pEntry->GetCombatVersusOtherReligionOwnLands());
							}
							if (pEntry->GetCombatVersusOtherReligionTheirLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_SKIRMISHER) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
							{
								iReligiousBonus += (pEntry->GetCombatVersusOtherReligionTheirLands());
							}
							for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
							{
								const YieldTypes eYield = static_cast<YieldTypes>(iI);
								if (eYield != NO_YIELD)
								{
									if (pEntry->GetYieldFromKills(eYield) > 0)
									{
										iReligiousBonus += (pEntry->GetYieldFromKills(eYield) / 5);
									}
								}
							}
						}
					}
				}
			}
			iBonus += iReligiousBonus;
		}

		//////////////
		//CIVILIAN CHECKS
		//////////////////////

		if (pkUnitEntry->IsTrade())
		{
			//No if we're small.
			if (m_pCity->getPopulation() <= 4)
			{
				return SR_USELESS;
			}
			
			if (pkUnitEntry->GetDomainType() == DOMAIN_LAND)
			{
				iBonus += m_pCity->GetTradePriorityLand() * 10;
			}
			else
			{
				iBonus += m_pCity->GetTradePrioritySea() * 10;
			}

			//Less often if at war.
			if (bAtWar)
			{
				iBonus -= 100;
			}

			if (kPlayer.GetPlayerTraits()->IsDiplomat())
			{
				iBonus += 750;
			}
			else
			{
				iBonus += 700;
			}
		}

		if (pkUnitEntry->GetGoldenAgeTurns() != 0)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsTourism() || kPlayer.GetPlayerTraits()->IsExpansionist())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->GetBaseBeakersTurnsToCount() != 0)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsNerd())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->IsCultureBoost() != 0)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsTourism())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_MESSENGER)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsDiplomat())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->GetBaseGold() != 0)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsDiplomat())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->IsCanBuyCityState() != 0)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsDiplomat())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->GetBaseHurry() != 0)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsSmaller() || kPlayer.GetPlayerTraits()->IsWarmonger())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_GENERAL)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsExpansionist() || kPlayer.GetPlayerTraits()->IsWarmonger())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ADMIRAL)
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsDiplomat() || kPlayer.GetPlayerTraits()->IsWarmonger())
			{
				iBonus += 100;
			}
		}
		if (pkUnitEntry->IsGreatWorkUnit())
		{
			iBonus += 100;
			if (kPlayer.GetPlayerTraits()->IsTourism())
			{
				iBonus += 100;
			}
		}
	}

	//Settlers? Let's see...
	if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
	{
		//Don't build a settler if we're about to grow.
		if (m_pCity->getFoodTurnsLeft() <= 1)
			return SR_STRATEGY;

		//this checks war state, grand strategy and more!
		static EconomicAIStrategyTypes eNoMoreExpand = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
		if (GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eNoMoreExpand))
			return SR_STRATEGY;

		AICityStrategyTypes eEnoughSettlers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_SETTLERS");
		if (m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eEnoughSettlers))
			return SR_BALANCE;

		int iFlavorExpansion = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));

		//we already checked ECONOMICAISTRATEGY_ENOUGH_EXPANSION, so we know we have a good settle plot
		//but let's bump the prio based on the quality of the plot
		int iQuality = kPlayer.GetSettlePlotQualityMeasure(kPlayer.GetBestSettlePlot(NULL));
		if (iQuality>0)
			iFlavorExpansion += iQuality;

		// If we are running "ECONOMICAISTRATEGY_EARLY_EXPANSION"
		if (kPlayer.IsEarlyExpansionPhase())
			//strategies affect unit flavors but unfortunately we largely ignore "pre" score from the flavor system
			iFlavorExpansion += 100;

		AICityStrategyTypes eFeeder = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEW_CONTINENT_FEEDER");
		if (m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eFeeder))
			iFlavorExpansion += 50;

		if (kPlayer.GetPlayerTraits()->IsExpansionWLTKD())
			iFlavorExpansion += 10;

		//if we're even more unhappy then ECONOMICAISTRATEGY_ENOUGH_EXPANSION will kick in
		if (kPlayer.IsEmpireUnhappy())
			iFlavorExpansion -= 10;

		//if already we have more than 2 cities, let's try to get non-capital cities to be our settler-makers
		if (m_pCity->isCapital() && kPlayer.getNumCities() > 2)
		{
			iFlavorExpansion -= 25;
			//but this bonus may counteract
			iFlavorExpansion += kPlayer.getCapitalSettlerProductionModifier();
		}

		//sometimes it's just not right
		if (iFlavorExpansion <= 0)
			return SR_STRATEGY;

		//if we got this far we want to expand, so let's bump the number of times we've skipped this.
		iFlavorExpansion += kPlayer.GetMilitaryAI()->GetNumberOfTimesSettlerBuildSkippedOver() * 10;

		//bring it into a sensible range
		iBonus += iFlavorExpansion*23;
	}

	if(!kPlayer.isMinorCiv())
	{
		//Archaeologists? Only if we have digs nearby.
		if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ARCHAEOLOGIST)
		{
			if(kPlayer.isMinorCiv())
			{
				return SR_IMPOSSIBLE;
			}
			static EconomicAIStrategyTypes eWantArch = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_ARCHAEOLOGISTS");
			if(!kPlayer.GetEconomicAI()->IsUsingStrategy(eWantArch))
			{
				return SR_BALANCE;
			}
			else
			{
				int iNumArch = kPlayer.GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true);
				if (iNumArch <= 0)
				{
					iBonus += 5000;
				}
				
				if (kPlayer.GetDiplomacyAI()->IsGoingForCultureVictory())
				{
					iBonus += 5000;
				}

				if(kPlayer.GetArchaeologicalDigTourism() > 0)
				{
					iBonus += 2500;
				}
				for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					//Simplification - errata yields not worth considering.
					if ((YieldTypes)iI > YIELD_CULTURE_LOCAL && !MOD_BALANCE_CORE_JFD)
						break;

					const YieldTypes eYield = static_cast<YieldTypes>(iI);
					if(eYield != NO_YIELD)
					{
						if(kPlayer.GetPlayerTraits()->GetArtifactYieldChanges(eYield) > 0)
						{
							iBonus += 500;
						}
					}
				}
			}
		}
	}

	//Make sure we need naval workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER_SEA)
	{
		CvTacticalDominanceZone* pZone = kPlayer.GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(m_pCity,true);
		if (pZone && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
			return SR_STRATEGY;

		//There's a worker waiting here? Abort!
		if(m_pCity->plot()->getNumUnitsOfAIType(UNITAI_WORKER_SEA, m_pCity->getOwner()) > 0)
		{
			return SR_USELESS;
		}
		static AICityStrategyTypes eNoNavalWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_NAVAL_TILE_IMPROVEMENT");
		if(eNoNavalWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNoNavalWorkers))
		{
			return SR_USELESS;
		}
		else
		{
			int iUnimprovedAround = 0;
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = (ResourceTypes)iResourceLoop;
				if (eResourceLoop != NO_RESOURCE)
				{
					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResourceLoop);
					if (pkResourceInfo != NULL && pkResourceInfo->isTerrain(TERRAIN_COAST))
					{
						if (!kPlayer.NeedWorkboatToImproveResource(eResourceLoop))
							continue;

						iUnimprovedAround += m_pCity->GetNumResourceLocal(eResourceLoop);
					}
				}
			}
			iBonus += (1000 * iUnimprovedAround * (m_pCity->getPopulation() + kPlayer.GetCurrentEra()));

			//additional loop to help coastal and non-coastal cities nearby.
			int iCityLoop = 0;
			for (CvCity* pLoopCity = kPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iCityLoop))
			{
				if (pLoopCity->HasSharedAreaWith(m_pCity,true,false) && m_pCity != pLoopCity)
				{
					static AICityStrategyTypes eNeedNavalWorker = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");
					if (eNeedNavalWorker != NO_AICITYSTRATEGY && pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedNavalWorker))
						iBonus += (250 * (pLoopCity->getPopulation() + kPlayer.GetCurrentEra()));
				}
			}
		}
	}
	//Make sure we need workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER)
	{
		CvTacticalDominanceZone* pZone = kPlayer.GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(m_pCity,false);
		if (pZone && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
			return SR_STRATEGY;

		//There's a worker waiting here? Abort!
		if (m_pCity->plot()->getNumUnitsOfAIType(UNITAI_WORKER, m_pCity->getOwner()) > 0)
			return SR_USELESS;

		int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true);
		int iMissingBuilders = kPlayer.getCitiesNeedingTerrainImprovements() + 1 - iNumBuilders; //one for roads
		int iScale = (iMissingBuilders > 2) ? 3 : 1; //escalate if many are missing

		static AICityStrategyTypes eNoWorkers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_TILE_IMPROVERS");
		if (eNoWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNoWorkers))
		{
			return SR_USELESS;
		}
		static AICityStrategyTypes eWantWorkers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_WANT_TILE_IMPROVERS");
		if (eWantWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eWantWorkers))
		{
			iBonus += (200 * iMissingBuilders * iScale);
		}
		static AICityStrategyTypes eNeedWorkers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");
		if (eNeedWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedWorkers))
		{
			iBonus += (500 * iMissingBuilders * iScale);
		}
	}
	
	if (!kPlayer.isMinorCiv())
	{
		//////////////////
		//WAR BOOSTERS
		////////////////////////
		int iWarBooster = 0;
		if (bCombat)
		{
			vector<PlayerTypes> enemies = kPlayer.GetPlayersAtWarWith();
			if (kPlayer.GetMilitaryAI()->GetWarType() == WARTYPE_LAND && eDomain == DOMAIN_LAND)
			{
				iWarBooster += enemies.size();
			}
			else if (kPlayer.GetMilitaryAI()->GetWarType() == WARTYPE_SEA && eDomain == DOMAIN_SEA)
			{
				iWarBooster += enemies.size();
			}

			iBonus += iWarBooster * 100;
		}
		if (bAtWar)
		{
			if (bCombat)
			{
				iBonus += m_pCity->getThreatValue();
			}
			else
			{
				iBonus -= m_pCity->getThreatValue();
			}
		}
		if (bCombat && bForOperation)
		{
			if (kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver() > 0)
			{
				iBonus += (kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver() * 100);
			}
		}

		/////////////////////
		// EXPERIENCE BOOSTERS
		/////////////////////
		if (bCombat && kPlayer.getNumCities() > 1)
		{
			//Let's try to build our units in our best cities only.
			if (m_pCity == kPlayer.GetBestMilitaryCity((UnitCombatTypes)pkUnitEntry->GetUnitCombatType()))
			{
				iBonus += 25 * kPlayer.getNumCities();
			}

			//Let's try to build our units in our best cities only.
			if (m_pCity == kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, pkUnitEntry->GetDomainType()))
			{
				iBonus += 25 * kPlayer.getNumCities();
			}

		}

		//Promotion Bonus
		//disabled for performance, bonus is very small, doesn't matter in the end

		/*
		int iPromotionBonus = 0;
		for(int iI = 0; iI < GC.getNumPromotionInfos() && bCombat; iI++)
		{
			const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkPromotionInfo)
			{
				if (pkUnitEntry->GetFreePromotions(iI))
				{
					if (!pkPromotionInfo->IsLostWithUpgrade())
					{ 
						iPromotionBonus += 5;
					}
				}
				if(kPlayer.IsFreePromotion(ePromotion))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iPromotionBonus += 5;
					}
				}
				if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitClass(iI, pkUnitEntry->GetUnitClassType()))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iPromotionBonus += 5;
					}
				}
				if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(iI, pkUnitEntry->GetUnitCombatType()))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iPromotionBonus += 5;
					}
				}
			}
		}
		
		if (iPromotionBonus != 0)
		{
			iBonus += iPromotionBonus;
		}
		*/
	
		//Uniques? They're generally good enough to spam.
		if(kPlayer.getCivilizationInfo().isCivilizationUnitOverridden(pkUnitEntry->GetUnitClassType()))
		{
			iBonus += 100;

			if (pkUnitEntry->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON", true)) // reduce the bonus for recons, they aren't that spammable
				iBonus -= 80;
		}

		//For an operation? Build it!
		if(bCombat)
		{
			if(bForOperation)
			{
				iBonus += 500;
			}

			//slight bump in value for ranged units, to counteract some other elements.
			if (pkUnitEntry->GetRangedCombat() > 0)
			{
				iBonus += pkUnitEntry->GetRangedCombat();
			}

			if (kPlayer.IsUnderrepresentedUnitType(pkUnitEntry->GetDefaultUnitAIType()))
			{
				iBonus += 200;
			}

			if (eDomain == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_EXPLORE)
			{
				if (m_pCity->GetGarrisonedUnit() == NULL)
				{
					iBonus += 25;
				}
			}
		}
	}
	//Tiny army? Eek!
	if (iNumLandUnits <= (kPlayer.getNumCities() * 2) && pkUnitEntry->GetDomainType() == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_EXPLORE)
	{
		if(bCombat)
		{
			iBonus += 300;
		}
		//Fewer civilians til we rectify this!
		else
		{
			iBonus -= 300;
		}
	}
	
	if (bDesperate)
	{
		if (bCombat)
			iBonus += 500;
		else
			iBonus -= 500;
	}

	if (bCombat && pkUnitEntry->GetDomainType() == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_EXPLORE)
	{
		static MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		static MilitaryAIStrategyTypes eStrategyBarbsCritical = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS_CRITICAL");
		
		if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyBarbs))
		{
			iBonus += 250;
		}
		else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyBarbsCritical))
		{
			iBonus += 500;
		}
	}

	if (bCombat)
	{
		UnitTypes eCurrentUnit = m_pCity->getProductionUnit();
		//making a combat unit?
		if (eCurrentUnit != NO_UNIT)
		{
			//working on this unit? finish up if at war.
			if (eUnit == eCurrentUnit && bAtWar)
			{
				iBonus += 100;
			}
			//a little boost if not at war (to finish)
			else if (eUnit == eCurrentUnit && !bAtWar)
			{
				iBonus += 50;
			}
			// looking at another unit? let's not switch.
			else if (GC.getUnitInfo(eCurrentUnit)->GetCombat() > 0 || GC.getUnitInfo(eCurrentUnit)->GetRangedCombat() > 0)
			{
				iBonus -= 100;
			}
		}
	}

	// Add a boost to score proportional to instant yield gains when the unit is completed
	int iInstantYieldImportanceScale = 1; // Relative importance of instant yields obtained to score bonus
	int iInstantYieldBonus = 0;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		const YieldTypes eYield = static_cast<YieldTypes>(iI);
		if (eYield != NO_YIELD)
		{
			int iYieldAmountOnComplete = GC.getUnitInfo(eUnit)->GetYieldOnCompletion(eYield);
			if (iYieldAmountOnComplete > 0)
			{
				iInstantYieldBonus += (iYieldAmountOnComplete * iInstantYieldImportanceScale);
			}
		}
	}

	iBonus += iInstantYieldBonus;

	//Make sure that free units are given some kind of value.
	if (bFree && iBonus <= iTempWeight)
	{
		iBonus = iTempWeight;
	}

	if (!kPlayer.isMinorCiv() && !pkUnitEntry->IsNoSupply())
	{
		//Let's check this against supply to keep our military numbers lean.
		if (bCombat && !bFree)
		{
			iBonus *= iScale;
			iBonus /= 100;
		}
	}

	//prioritize besieged cities for purchase
	if (bForPurchase && bCombat && bDesperate)
	{
		iBonus += 250;
	}

	// MOD_AI_UNIT_PRODUCTION - NEW STRATEGIES -- consider if we have too much or too few of a unit
	if (bCombat && MOD_AI_UNIT_PRODUCTION)
	{
		static MilitaryAIStrategyTypes eStrategyArcher = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_ARCHER");
		static MilitaryAIStrategyTypes eStrategyEnoughArcher = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_ARCHER");
		static MilitaryAIStrategyTypes eStrategySiege = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_SIEGE");
		static MilitaryAIStrategyTypes eStrategyEnoughSiege = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_SIEGE");
		static MilitaryAIStrategyTypes eStrategySkirmisher = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_SKIRMISHER");
		static MilitaryAIStrategyTypes eStrategyEnoughSkirmisher = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_SKIRMISHER");
		static MilitaryAIStrategyTypes eStrategyMobile = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_MOBILE");
		static MilitaryAIStrategyTypes eStrategyEnoughMobile = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_MOBILE");
		static MilitaryAIStrategyTypes eStrategyNavalMelee = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_MELEE");
		//static MilitaryAIStrategyTypes eStrategyEnoughNavalMelee = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_NAVAL_MELEE");
		static MilitaryAIStrategyTypes eStrategyNavalRanged = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_RANGED");
		static MilitaryAIStrategyTypes eStrategyEnoughNavalRanged = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_NAVAL_RANGED");
		static MilitaryAIStrategyTypes eStrategySubmarine = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_SUBMARINE");
		static MilitaryAIStrategyTypes eStrategyEnoughSubmarine = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_SUBMARINE");
		static MilitaryAIStrategyTypes eStrategyBomber = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_BOMBER");
		static MilitaryAIStrategyTypes eStrategyEnoughBomber = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_BOMBER");
		static MilitaryAIStrategyTypes eStrategyFighter = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_FIGHTER");
		static MilitaryAIStrategyTypes eStrategyEnoughFighter = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_FIGHTER");

		if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_RANGED)
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyArcher))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughArcher))
				iBonus -= 100;
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_CITY_BOMBARD)
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategySiege))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughSiege))
				iBonus -= 100;
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SKIRMISHER)
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategySkirmisher))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughSkirmisher))
				iBonus -= 100;
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_FAST_ATTACK) // mounted & armor
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyMobile))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughMobile))
				iBonus -= 100;
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ATTACK_SEA) // naval melee
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyNavalMelee))
				iBonus += 100;
			/*			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughNavalMelee)) -- do Enough discourage this, as ranged may be restricted by resource
							iBonus -= 100;
			*/
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ASSAULT_SEA) // naval ranged
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyNavalRanged))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughNavalRanged))
				iBonus -= 100;
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SUBMARINE)
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategySubmarine))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughSubmarine))
				iBonus -= 100;
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ATTACK_AIR)
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyBomber))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughBomber))
				iBonus -= 100;
		}
		else if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_DEFENSE_AIR)
		{
			if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyFighter))
				iBonus += 100;
			else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyEnoughFighter))
				iBonus -= 100;
		}
	}


	/////
	///WEIGHT
	//////

	if (iBonus < 0)
		return SR_UNKNOWN;

	//iValue modified by iBonus
	iTempWeight += iBonus;

	if (m_pCity->IsOccupied() && !m_pCity->IsNoOccupiedUnhappiness())
		iTempWeight /= 5;

	return max(1,iTempWeight);
}
#endif


/// Log all potential builds
void CvUnitProductionAI::LogPossibleBuilds(UnitAITypes eUnitAIType)
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
		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvString strOutBuf = strBaseString;

			CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(m_Buildables.GetElement(iI));

			CvString strDesc = (pUnitEntry != NULL)? pUnitEntry->GetDescription() : "Unknown Unit";
			if(eUnitAIType == NO_UNITAI)
			{
				CvString strTemp;
				strTemp.Format("Unit, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
				strOutBuf += strTemp;
			}
			else
			{
				CvString strTempString;
				getUnitAIString(strTempString, eUnitAIType);

				CvString strTemp;
				strTemp.Format("Special request for unit of type: %s, %s, %d", strTempString.GetCString(), strDesc.GetCString(), m_Buildables.GetWeight(iI));
				strOutBuf += strTemp;
			}

			pLog->Msg(strOutBuf);
		}
	}
}
