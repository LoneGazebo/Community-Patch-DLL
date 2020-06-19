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
	CvAssertMsg(m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");

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

/// Serialization read
void CvUnitProductionAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	int iWeight;

	// Reset vector
	m_UnitAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	if(m_pUnits)
	{
		for(int i = 0; i < m_pUnits->GetNumUnits(); i++)
		{
			m_UnitAIWeights.push_back(i, 0);
		}

		int iNumEntries;
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
		{
			bool bValid = true;
			iType = CvInfosSerializationHelper::ReadHashed(kStream, &bValid);
			if(iType != -1 || !bValid)
			{
				kStream >> iWeight;
				if(iType != -1)
				{
					m_UnitAIWeights.IncreaseWeight(iType, iWeight);
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Unit Type not found");
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
		}
	}
	else
	{
		CvAssertMsg(m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");
	}
}

/// Serialization write
void CvUnitProductionAI::Write(FDataStream& kStream) const
{
	FStringFixedBuffer(sTemp, 64);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	if(m_pUnits)
	{
		int iNumUnits = m_pUnits->GetNumUnits();
		kStream << iNumUnits;

		// Loop through writing each entry
		for(int iI = 0; iI < iNumUnits; iI++)
		{
			const UnitTypes eUnit = static_cast<UnitTypes>(iI);
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
			if(pkUnitInfo)
			{
				CvInfosSerializationHelper::WriteHashed(kStream, pkUnitInfo);
				kStream << m_UnitAIWeights.GetWeight(iI);
			}
			else
			{
				kStream << (int)0;
			}
		}
	}
	else
	{
		CvAssertMsg(m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");
	}
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
	return m_UnitAIWeights.GetWeight(eUnit);
}

/// Recommend highest-weighted unit
#if defined(MOD_BALANCE_CORE)
UnitTypes CvUnitProductionAI::RecommendUnit(UnitAITypes eUnitAIType, bool bUsesStrategicResource)
#else
UnitTypes CvUnitProductionAI::RecommendUnit(UnitAITypes eUnitAIType)
#endif
{
	int iUnitLoop;
	int iWeight;
	int iTurnsLeft;

	if(eUnitAIType <= NO_UNITAI)
	{
		return NO_UNIT;
	}

	// Reset list of all the possible units
	m_Buildables.clear();

	// Loop through adding the available units
	for(iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
#if defined(MOD_BALANCE_CORE)
			bool bBad = false;
			if(!bUsesStrategicResource)
			{
				ResourceTypes eResource;
				for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
				{
					eResource = (ResourceTypes) iResourceLoop;
					int iNumResource = pkUnitInfo->GetResourceQuantityRequirement(eResource);
					if (iNumResource > 0)
					{
						bBad = true;
						break;
					}
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
					if (MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
					{
						iNumResource = pkUnitInfo->GetResourceQuantityTotal(eResource);
						if (iNumResource > 0)
						{
							bBad = true;
							break;
						}
					}
#endif
				}
			}
			if(bBad)
			{
				continue;
			}
#endif
			// Make sure this unit can be built now
			if(m_pCity->canTrain(eUnit, (m_pCity->isProductionUnit() && eUnit == m_pCity->getProductionUnit())))
			{
				// Make sure it matches the requested unit AI type
				if(eUnitAIType == NO_UNITAI || pkUnitInfo->GetUnitAIType(eUnitAIType))
				{
					// Update weight based on turns to construct
					iTurnsLeft = m_pCity->getProductionTurnsLeft(eUnit, 0);
					int iTempWeight = m_UnitAIWeights.GetWeight(eUnit);
					if (iTempWeight == 0)
					{
						const int iNumFlavors = GC.getNumFlavorTypes();
						for (int iFlavorLoop = 0; iFlavorLoop < iNumFlavors; iFlavorLoop++)
						{
							iTempWeight += pkUnitInfo->GetFlavorValue(iFlavorLoop);
						}
					}
					iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(iTempWeight, iTurnsLeft);
#if defined(MOD_BALANCE_CORE)
					if(iWeight > 0)
					{
#endif
					m_Buildables.push_back(iUnitLoop, iWeight);
#if defined(MOD_BALANCE_CORE)
					}
#endif
				}
			}
		}

	}

	// Sort items and grab the first one
	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
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
int CvUnitProductionAI::CheckUnitBuildSanity(UnitTypes eUnit, bool bForOperation, CvArmyAI* pArmy, int iTempWeight, int iGPT, int iWaterRoutes, int iLandRoutes, bool bForPurchase, bool bFree, bool bInterruptBuildings)
{
	bool bOperationalOverride = false;
	CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
	bool bCombat = (pkUnitEntry->GetCombat() > 0 || pkUnitEntry->GetRangedCombat() > 0);

	if(iTempWeight <= 0)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	//Sanitize...
	if (kPlayer.GetPlayerTraits()->IsNoAnnexing() && m_pCity->isCapital())
	{
		if (iTempWeight > 250)
		{
			iTempWeight = 250;
		}
	}
	else
	{
		if (iTempWeight > 100)
		{
			iTempWeight = 100;
		}
	}

	if (!pkUnitEntry)
		return 0;

	if (!bFree && bForPurchase && !m_pCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_GOLD))
	{
		return 0;
	}

	if (!bFree && !bForPurchase && m_pCity->IsPuppet())
	{
		return 0;
	}

	bool bDesperate = m_pCity->isInDangerOfFalling() || m_pCity->isUnderSiege();

	if (!bFree && bDesperate && !bCombat)
	{
		return 0;
	}

	//don't build land/sea units if there's no place to put them
	if (bCombat && m_pCity->HasGarrison() && pkUnitEntry->GetDomainType() != DOMAIN_AIR)
	{
		int iFreePlots = 0;
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(m_pCity->plot());
		for (int i = 0; i < 6; i++)
		{
			CvPlot* pNeighbor = aNeighbors[i];
			if (pNeighbor && pNeighbor->isValidMovePlot(m_pCity->getOwner()) && pNeighbor->GetNumCombatUnits() == 0)
			{
				if (pNeighbor->isWater() && pkUnitEntry->GetDomainType() == DOMAIN_SEA)
					iFreePlots++;

				if (!pNeighbor->isWater() && pkUnitEntry->GetDomainType() == DOMAIN_LAND)
					iFreePlots++;
			}
		}

		if (iFreePlots == 0)
			return 0;
	}

	//% Value that will modify the base value.
	int iBonus = 0;

	//only war with majors count
	bool bAtWar = (kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0);
	if (!bFree && kPlayer.isMinorCiv())
	{
		if (bCombat)
		{
			if (kPlayer.getNumMilitaryUnits() > max(4, ((kPlayer.GetCurrentEra() + 3) * max(1, kPlayer.getNumCities()))))
				return 0;

			if (pkUnitEntry->GetDomainType() == DOMAIN_SEA && kPlayer.getCapitalCity() != NULL && kPlayer.getCapitalCity()->isCoastal())
			{
				int iNumUnits = kPlayer.GetNumUnitsWithDomain(DOMAIN_SEA, true);
				if (iNumUnits <= 2)
					iBonus += max(0, 300 - (iNumUnits * 100));

				int iNumCities = kPlayer.getNumCities();
				int iEra = (kPlayer.GetCurrentEra() + 1) * iNumCities;
				if (iNumUnits <= iEra)
				{
					iBonus += (iEra - iNumUnits) * 100;
					if (kPlayer.GetMinorCivAI()->IsRecentlyBulliedByAnyMajor())
					{
						iBonus += 100;
					}
				}
				else
					return 0;
			}
			else
			{
				int iNumUnits = kPlayer.GetNumUnitsWithDomain(DOMAIN_LAND, true);
				if (iNumUnits <= 2)
					iBonus += max(0, 800 - (iNumUnits * 100));

				int iNumCities = kPlayer.getNumCities();
				int iEra = (kPlayer.GetCurrentEra() + 2) * iNumCities;

				if (iNumUnits <= iEra)
				{
					iBonus += (iEra - iNumUnits) * 150;
					if (kPlayer.GetMinorCivAI()->IsRecentlyBulliedByAnyMajor())
					{
						iBonus += 250;
					}
				}
				else
					return 0;
			}
		}
	}

	//Let's check this against supply to keep our military numbers lean.
	int iScale = 0;
	if (bCombat && !bFree)
	{
		int iSupply = max(1, kPlayer.GetNumUnitsSupplied());
		int iDemand = kPlayer.GetNumUnitsToSupply();
		if (bAtWar || bForOperation)
		{
			//hard limit, don't go too far into negative supply
			if (iSupply <= iDemand - 3)
				return -1;

			//reduce bonus once we're over the limit
			iScale = MapToPercent(iDemand, iSupply + 3, iSupply - 1);
		}
		else
		{
			//reduce bonus once we're approaching the limit
			iScale = MapToPercent(iDemand, iSupply, (iSupply * 2) / 3);

			//don't exceed the limit
			if (iSupply <= iDemand)
				return -1;
		}
	}

	//Are we alone?
	DomainTypes eDomain = (DomainTypes) pkUnitEntry->GetDomainType();
	bool bAlone = true;
	if(kPlayer.isMinorCiv())
	{
		bAlone = false;
	}
	else if (!bFree && bCombat)
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && kPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				if (kPlayer.GetProximityToPlayer(eLoopPlayer) >= PLAYER_PROXIMITY_CLOSE)
				{
					bAlone = false;
					break;
				}
			}
		}
		CvArea* pArea = GC.getMap().getArea(m_pCity->getArea());
		if(pArea != NULL)
		{
			if(pArea->getNumTiles() <= 3)
			{
				if (eDomain == DOMAIN_LAND)
				{
					if(m_pCity->GetGarrisonedUnit() != NULL)
					{
						return 0;
					}
				}
			}
		}
	}

	
	//////////////////
	//DOMAINS AND LOCATION CHECKS
	////////////////////////

	// sanity check for building ships on isolated small inland seas (not lakes)
	if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_WORKER_SEA) // if needed allow workboats...
	{
		int iWaterTiles = 0;
		int iNumUnitsofMine = 0;
		int iNumUnitsOther = 0;
		int iNumCitiesofMine = 0;
		int iNumCitiesOther = 0;

		std::vector<int> areas = m_pCity->plot()->getAllAdjacentAreas();
		for (std::vector<int>::iterator it=areas.begin(); it!=areas.end(); ++it)
		{
			CvArea* pkArea = GC.getMap().getArea(*it);
			if (pkArea->isWater())
			{
				iWaterTiles += pkArea->getNumTiles();
				iNumUnitsofMine += pkArea->getUnitsPerPlayer(m_pCity->getOwner());
				iNumUnitsOther += pkArea->getNumUnits()-iNumUnitsofMine;
				iNumCitiesofMine += pkArea->getCitiesPerPlayer(m_pCity->getOwner());
				iNumCitiesOther += pkArea->getNumCities()-iNumCitiesofMine;
			}
		}

		int iFactor = GC.getAI_CONFIG_MILITARY_TILES_PER_SHIP();
		//Are we mustering a naval attack here?
		if(bForOperation && !kPlayer.IsMusterCityAlreadyTargeted(m_pCity, DOMAIN_SEA))
		{
			bOperationalOverride = true;
		}
		if (!bFree && !bOperationalOverride && ((iNumUnitsofMine * iFactor > iWaterTiles) || ((iNumUnitsOther == 0 && iNumCitiesOther == 0))))
		{
			return 0;
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
				return 0;
			}
		}
		//Check for specific resource usage by units.
		if(pkUnitEntry->GetSpaceshipProject() == NO_PROJECT)
		{
			int iResourceBonus = 0;
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
				if (eResourceLoop != NO_RESOURCE)
				{
					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResourceLoop);
					if(pkResourceInfo != NULL)
					{	
						//Aluminum Check
						ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
						if(eResourceLoop == eAluminumResource)
						{
							if(pkUnitEntry->GetResourceQuantityRequirement(iResourceLoop) > 0)
							{
								//We need at least 4 aluminum to get off the planet, so let's save that much if we've got the Apollo.
								if(kPlayer.getNumResourceAvailable(eResourceLoop, false) <= 5)
								{
									return 0;
								}
							}
						}
						//we should really be building as many resource units as we can...
						if(pkUnitEntry->GetResourceQuantityRequirement(iResourceLoop) > 0)
						{
							iResourceBonus += max(40, (kPlayer.getNumResourceAvailable(eResourceLoop, false) - pkUnitEntry->GetResourceQuantityRequirement(iResourceLoop)) * 40);
						}
					}
				}
			}
			iBonus += iResourceBonus;
		}

		//Let's look at what this can build (if a combat unit).
		if (bCombat)
		{
			int iBuildBonus = 0;
			for (int i = 0; i < GC.getNumBuildInfos(); i++)
			{
				CvBuildInfo* pkBuild = GC.getBuildInfo((BuildTypes)i);
					
				if (pkBuild && (pkUnitEntry->GetBuilds((BuildTypes)i) && !kPlayer.GetPlayerTraits()->IsNoBuild((BuildTypes)i) || kPlayer.GetPlayerTraits()->HasUnitClassCanBuild(i, pkUnitEntry->GetUnitClassType())))
				{
					iBuildBonus++;
				}
			}
			iBonus += iBuildBonus * 10;
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
		if (!bFree && pkUnitEntry->IsCityAttackSupport())
		{
			int iTotalAlready = kPlayer.GetNumUnitsWithUnitAI(pkUnitEntry->GetDefaultUnitAIType());
			int iNum = kPlayer.GetNumUnitsWithUnitAI(UNITAI_CITY_BOMBARD, true, false);
			if (iNum < iTotalAlready)
			{
				return 0;
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
		if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_CARRIER_SEA || (pkUnitEntry->GetCargoSpace() > 0 && pkUnitEntry->GetDomainType() == DOMAIN_SEA))
		{
			if (kPlayer.isMinorCiv())
				return 0;

			int iSlots = kPlayer.GetMilitaryAI()->GetNumFreeCarrier();
			//No planes, or a surplus of carriers? No carriers
			if (iSlots == -1 || (iSlots > 0))
			{
				return 0;
			}
			//No slots at all? Let's make one.
			else if (iSlots == 0)
			{
				iBonus += 1000;
			}
			//If we have more planes than slots (negative), we need this!
			else
			{
				iBonus += (-10 * iSlots);
			}
		}
		//Need Explorers?
		if (eDomain == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE)
		{
			int iExplore = kPlayer.GetEconomicAI()->GetExplorersNeeded();
			if(iExplore > 0)
			{
				iBonus += iExplore * 2;
			}
		}
		//Need Sea Explorers?
		if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA || pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ATTACK_SEA)
		{
			int iExplore = kPlayer.GetEconomicAI()->GetNavalExplorersNeeded();
			if(iExplore > 0)
			{
				iBonus += iExplore * 2;
			}
		}
		//Naval Units Critically Needed?
		if (eDomain == DOMAIN_SEA)
		{
			if(bCombat)
			{
				int iCurrent = kPlayer.GetMilitaryAI()->GetNumNavalUnits();
				int iDesired = kPlayer.GetMilitaryAI()->GetRecommendNavySize();
				int iValue = iDesired - iCurrent;

				iValue *= 1 + kPlayer.GetCurrentEra();

				int iNumCoastal = 0;
				int iLoop;
				for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
				{
					if (pCity != NULL && pCity->isCoastal())
					{
						iNumCoastal++;
					}
				}
				if (iCurrent <= iNumCoastal)
					iValue *= 2;

				if (iValue > 0)
				{
					if (bAlone)
					{
						iValue *= 5;
					}
					int iWarValue = 0;
					int iNumPlayers = 0;
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

						if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID())
						{
							if (!GET_PLAYER(eLoopPlayer).isAlive())
								continue;

							iNumPlayers++;

							bool bPotentialTargetOrThreat = kPlayer.isMajorCiv() ? kPlayer.GetDiplomacyAI()->IsPotentialMilitaryTargetOrThreat(eLoopPlayer) : false;

							if (kPlayer.GetMilitaryAI()->GetWarType(eLoopPlayer) == WARTYPE_SEA)
							{
								if (GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) || bPotentialTargetOrThreat)
									iWarValue += 4;
								else
									iWarValue += 2;
							}
							else
							{
								iWarValue += 1;
							}
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

				iValue *= max(1, (int)kPlayer.GetCurrentEra());

				if (iValue > 0)
				{
					if (bAlone)
					{
						iValue /= 2;
						if (iValue <= 0)
							iValue = 1;
					}
					int iWarValue = 0;
					int iNumPlayers = 0;
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

						if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID())
						{
							if (!GET_PLAYER(eLoopPlayer).isAlive())
								continue;

							iNumPlayers++;

							bool bPotentialTargetOrThreat = kPlayer.isMajorCiv() ? kPlayer.GetDiplomacyAI()->IsPotentialMilitaryTargetOrThreat(eLoopPlayer) : false;

							if (kPlayer.GetMilitaryAI()->GetWarType(eLoopPlayer) == WARTYPE_LAND)
							{
								if (GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) || bPotentialTargetOrThreat)
									iWarValue += 4;
								else
									iWarValue += 2;
							}
							else
							{
								iWarValue += 1;
							}
						}
					}

					iWarValue /= max(1, iNumPlayers);

					if (kPlayer.GetMilitaryAI()->GetWarType() == WARTYPE_LAND)
					{
						iWarValue += 15;
					}

					if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_DEFENSE || pkUnitEntry->GetDefaultUnitAIType() == UNITAI_COUNTER || pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ATTACK)
					{
						CvUnit* pLoopUnit2;
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
		//Air Units Critically Needed?
		if (eDomain == DOMAIN_AIR)
		{
			bool bBomber = pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ATTACK_AIR;

			int iNeedAir = 0;

			int iOurAir = bBomber ? kPlayer.GetNumUnitsWithUnitAI(UNITAI_ATTACK_AIR) : kPlayer.GetNumUnitsWithUnitAI(UNITAI_DEFENSE_AIR);
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

				if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && kPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && (kPlayer.GetProximityToPlayer(eLoopPlayer) == PLAYER_PROXIMITY_NEIGHBORS || GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam())))
				{
					int iTheirAir = bBomber ? GET_PLAYER(eLoopPlayer).GetNumUnitsWithUnitAI(UNITAI_DEFENSE_AIR) : GET_PLAYER(eLoopPlayer).GetNumUnitsWithUnitAI(UNITAI_ATTACK_AIR);

					//they have no air units? Bombers!
					if (iTheirAir == 0 && bBomber)
						iNeedAir += 10;

					//no fighters for city defense versus their bombers? eek?
					if (iTheirAir > 0 && iOurAir <= kPlayer.getNumCities() && !bBomber)
						iNeedAir += 5;

					if (iTheirAir >= iOurAir)
						iNeedAir++;
				}
			}

			iBonus += iNeedAir * 100;

			int iMaxAircraft = m_pCity->GetMaxAirUnits();
			if (iMaxAircraft > 0)
			{
				int iAir = m_pCity->plot()->countNumAirUnits(kPlayer.getTeam(), true);
				if (iOurAir <= 0)
				{
					iBonus += 500 * iMaxAircraft;
				}
				else
				{
					iBonus += (iMaxAircraft - iAir) * 250;
				}
				if (m_pCity->GetGarrisonedUnit() == NULL)
				{
					iBonus -= 50;
				}
			}
		}

		//Air defense needed?
		if (eDomain == DOMAIN_LAND && pkUnitEntry->GetAirInterceptRange() > 0 || pkUnitEntry->GetBaseLandAirDefense() > 20)
		{
			int iNumAA = kPlayer.GetMilitaryAI()->GetNumAAUnits();
			int iNumCities = kPlayer.getNumCities();
			
			iBonus += ((iNumCities - iNumAA) * 5);
		}
	
		/////////////
		//GRAND STRATEGY CHECKS
		//////////////////
		EconomicAIStrategyTypes eStrategyConquest = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CONQUEST");				
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
		
#if defined(MOD_DIPLOMACY_CITYSTATES)	
		//Diplomatic Units!
		if(MOD_DIPLOMACY_CITYSTATES &&  pkUnitEntry->GetDefaultUnitAIType() == UNITAI_MESSENGER)
		{
			if (!kPlayer.HasMetValidMinorCiv())
				return 0;

			//There's a diplomat waiting here? Abort!
			if (m_pCity->plot()->getNumUnitsOfAIType(UNITAI_MESSENGER, m_pCity->getOwner()) > 0)
			{
				return 0;
			}

			//There's 2 sitting around? Abort!
			if (kPlayer.GetNumUnitsWithUnitAI(UNITAI_MESSENGER, true, false) > 3)
			{
				return 0;
			}

			int iInfluence = 0;
			//Promotion Bonus
			for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				if(pkUnitEntry->GetFreePromotions(iI))
				{
					PromotionTypes ePromotion = (PromotionTypes) iI;

					if(GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence() != 0)
					{
						iInfluence += GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence();
					}
				}
				if (kPlayer.GetFreePromotionCount((PromotionTypes)iI))
				{
					PromotionTypes ePromotion = (PromotionTypes)iI;

					if (GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence() != 0)
					{
						iInfluence += GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence();
					}
				}

				if (m_pCity->isFreePromotion((PromotionTypes)iI))
				{
					PromotionTypes ePromotion = (PromotionTypes)iI;

					if (GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence() != 0)
					{
						iInfluence += GC.getPromotionInfo(ePromotion)->GetDiploMissionInfluence();
					}
				}
			}
			EconomicAIStrategyTypes eNeedDiplomats = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS");
			EconomicAIStrategyTypes eNeedDiplomatsCrit = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");
			if(eNeedDiplomats != NO_ECONOMICAISTRATEGY && eNeedDiplomatsCrit != NO_ECONOMICAISTRATEGY)
			{
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
					return 0;
				}
			}
			if (MOD_DIPLOMACY_CITYSTATES)
			{
				ResourceTypes ePaper = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_PAPER", true);
				if (ePaper != NO_RESOURCE && kPlayer.getResourceFromCSAlliances(ePaper) > 0)
				{
					iInfluence *= 2;
				}
			}

			iBonus += iInfluence;
		}
#endif
		if(pkUnitEntry->GetSpaceshipProject() != NO_PROJECT)
		{
			EconomicAIStrategyTypes eStrategySS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP");
			if (eStrategySS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategySS))
			{
				iBonus += 1000;
			}
			else
			{
				iBonus += 500;
			}
			if(kPlayer.GetDiplomacyAI()->IsCloseToSSVictory())
			{
				iBonus += 1000;
			}
			else if(kPlayer.GetDiplomacyAI()->IsCloseToCultureVictory())
			{
				iBonus += 500;
			}
			else if(kPlayer.GetDiplomacyAI()->IsCloseToDominationVictory())
			{
				iBonus += 350;
			}
			else if(kPlayer.GetDiplomacyAI()->IsCloseToDiploVictory())
			{
				iBonus += 400;
			}

			if(m_pCity->getSpaceProductionModifier() > 0)
			{
				iBonus += (m_pCity->getSpaceProductionModifier() * 10);
			}
			else
			{
				iBonus -= 100;
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
			ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(m_pCity->getOwner());
			if(eReligion == NO_RELIGION)
			{
				eReligion = GET_PLAYER(m_pCity->getOwner()).GetReligions()->GetReligionInMostCities();
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
						if(pEntry && pReligion->m_Beliefs.HasBelief(eBelief) && pReligion->m_Beliefs.IsBeliefValid(eBelief, eReligion, m_pCity->getOwner()))
						{
							if(pEntry->GetFaithFromKills() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK)))
							{
								iReligiousBonus += (pEntry->GetFaithFromKills() / 5);
							}
							if(pEntry->GetCombatModifierEnemyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
							{
								iReligiousBonus += (pEntry->GetCombatModifierEnemyCities());
							}
							if(pEntry->GetCombatModifierFriendlyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
							{
								iReligiousBonus += (pEntry->GetCombatModifierFriendlyCities());
							}
							if(pEntry->GetCombatVersusOtherReligionOwnLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
							{
								iReligiousBonus += (pEntry->GetCombatVersusOtherReligionOwnLands());
							}
							if(pEntry->GetCombatVersusOtherReligionTheirLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
							{
								iReligiousBonus += (pEntry->GetCombatVersusOtherReligionTheirLands());
							}
							for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
							{
								const YieldTypes eYield = static_cast<YieldTypes>(iI);
								if(eYield != NO_YIELD)
								{
									if(pEntry->GetYieldFromKills(eYield) > 0)
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
			if(m_pCity->getPopulation() <= 4)
			{
				return 0;
			}
			if(pkUnitEntry->GetDomainType() == DOMAIN_LAND)
			{
				iBonus += iLandRoutes;
			}
			else
			{
				iBonus += iWaterRoutes;
			}
			if(iGPT <= 0)
			{
				iBonus += (iGPT * iGPT * 100);
			}
			int iUnhappyGold = m_pCity->getUnhappinessFromGold();
			if (iUnhappyGold > 0)
			{
				iBonus += (iUnhappyGold * 10);
			}
			//Less often if at war.
			if (bAtWar && iGPT > 0)
			{
				iBonus -= 100;
			}

			if (kPlayer.GetPlayerTraits()->IsDiplomat())
			{
				int NumMissingTRs = kPlayer.GetTrade()->GetNumTradeRoutesPossible() - kPlayer.GetTrade()->GetNumTradeUnits(true);
				iBonus += 100 * NumMissingTRs;
			}
			else
			{
				int NumMissingTRs = kPlayer.GetTrade()->GetNumTradeRoutesPossible() - kPlayer.GetTrade()->GetNumTradeUnits(true);
				iBonus += 50 * NumMissingTRs;
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
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
		EconomicAIStrategyTypes eCanSettle = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_FOUND_CITY");
		if (EconomicAIHelpers::CannotMinorCiv(m_pCity->GetPlayer(), eCanSettle))
#else
		if(kPlayer.isMinorCiv())
#endif
		{
			return 0;
		}

		//There's a settler waiting here? Abort!
		if (m_pCity->plot()->getNumUnitsOfAIType(UNITAI_SETTLE, m_pCity->getOwner()) > 0)
		{
			return 0;
		}

		if (kPlayer.isBarbarian() || kPlayer.GetPlayerTraits()->IsNoAnnexing() || (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && kPlayer.isHuman()))
		{
			return 0;
		}
		if (kPlayer.IsEmpireSuperUnhappy())
		{
			return 0;
		}

		if (GC.getGame().getGameTurn() >= 50)
		{
			//Don't build a settler if we're about to grow.
			if (m_pCity->getFoodTurnsLeft() <= 1)
			{
				return 0;
			}

			EconomicAIStrategyTypes eNoMoreExpand = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
			if (GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eNoMoreExpand))
			{
				return 0;
			}

			//Or if we're small.
			if (m_pCity->getPopulation() <= 3)
			{
				return 0;
			}
		}

		//Already have an idle settler out? Ignore.
		int iNumSettlers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true, true);
		if(iNumSettlers>0)
		{
			return 0;
		}
	
		int iFlavorExpansion = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
		iFlavorExpansion -= kPlayer.getNumCities();

		if (GET_TEAM(kPlayer.getTeam()).canEmbarkAllWaterPassage())
		{
			// If we are running "ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS"
			EconomicAIStrategyTypes eExpandOther = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
			if (eExpandOther != NO_ECONOMICAISTRATEGY)
			{
				if (kPlayer.GetEconomicAI()->IsUsingStrategy(eExpandOther))
				{
					iFlavorExpansion += 10;
				}

				EconomicAIStrategyTypes eExpandOtherOffshore = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_OFFSHORE_EXPANSION_MAP");
				if (eExpandOtherOffshore != NO_ECONOMICAISTRATEGY)
				{
					if (kPlayer.GetEconomicAI()->IsUsingStrategy(eExpandOtherOffshore))
					{
						iFlavorExpansion += 5;
					}
				}
			}
		}

		// If we are running "ECONOMICAISTRATEGY_EARLY_EXPANSION"
		bool bRunningEarlyExpand = false;
		EconomicAIStrategyTypes eEarlyExpand = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
		if (eEarlyExpand != NO_ECONOMICAISTRATEGY)
		{
			if (kPlayer.GetEconomicAI()->IsUsingStrategy(eEarlyExpand))
			{
				iFlavorExpansion += 5;
				bRunningEarlyExpand = true;
			}
		}

		// If we are running "ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY"
		EconomicAIStrategyTypes eExpandCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY");
		if (eExpandCrazy != NO_ECONOMICAISTRATEGY)
		{
			if (kPlayer.GetEconomicAI()->IsUsingStrategy(eExpandCrazy))
			{
				iFlavorExpansion += 10;
			}
		}

		AICityStrategyTypes eFeeder = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEW_CONTINENT_FEEDER");
		if (eFeeder != NO_AICITYSTRATEGY)
		{
			if (m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eFeeder))
			{
				iFlavorExpansion += 10;
			}
		}	

		if(kPlayer.getSettlerProductionModifier() > 0)
		{
			iFlavorExpansion += 5;
		}
		if (kPlayer.GetPlayerTraits()->IsExpansionWLTKD())
		{
			iFlavorExpansion += 5;
		}
		if(m_pCity->isCapital() && kPlayer.getCapitalSettlerProductionModifier() > 0)
		{
			iFlavorExpansion += 5;
		}
		int iEraDifference = ((GC.getGame().getCurrentEra()+1) * 2) - kPlayer.getNumCities();
		if (iEraDifference > 0 && !kPlayer.GetPlayerTraits()->IsNoAnnexing())
		{
			iFlavorExpansion += 10 * iEraDifference;
		}
		if (kPlayer.IsAtWar())
		{
			iFlavorExpansion -= 10;
		}

		if (kPlayer.IsEmpireUnhappy())
		{
			iFlavorExpansion -= 15;

			if (kPlayer.IsEmpireVeryUnhappy())
			{
				iFlavorExpansion -= 15;
				
				if (kPlayer.IsEmpireSuperUnhappy())
					iFlavorExpansion -= 15;
			}
		}

		if(kPlayer.GetDiplomacyAI()->IsGoingForCultureVictory() && (kPlayer.GetNumCitiesFounded() > (kPlayer.GetDiplomacyAI()->GetBoldness())))
		{
			iFlavorExpansion -= 5;
		}

		// scale based on flavor and world size
		MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
		if(eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
		{
			iFlavorExpansion -= 10;
		}

		//check victory conditions
		if (!bRunningEarlyExpand)
		{
			AIGrandStrategyTypes eGrandStrategy = kPlayer.GetGrandStrategyAI()->GetActiveGrandStrategy();
			bool bSeekingCultureVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
			if (bSeekingCultureVictory)
			{
				iFlavorExpansion -= 10;
			}
			bool bSeekingSSVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");
			if (bSeekingSSVictory)
			{
				iFlavorExpansion -= 10;
			}
		}

		//bonuses for late founding units
		if (pkUnitEntry->IsFoundMid())
			iFlavorExpansion += 5;
		else if (pkUnitEntry->GetNumColonyFound() > 0)
			iFlavorExpansion += 10;
		else if (pkUnitEntry->IsFoundLate())
			iFlavorExpansion += 10;
		else if (pkUnitEntry->IsFoundAbroad())
			iFlavorExpansion += 10;

		if (iFlavorExpansion <= 0)
			return 0;

		//once we've expanded a single time, let's try to get non-capital cities to be our settler-makers.
		if (!m_pCity->isCapital())
			iFlavorExpansion += 5;
		else if (kPlayer.getNumCities() > 1)
			iFlavorExpansion -= 5;

		//todo: is 10 the right number here?
		if (m_pCity->isCapital() && iFlavorExpansion >= 10)
		{
			kPlayer.GetMilitaryAI()->BumpNumberOfTimesSettlerBuildSkippedOver();
		}

		//if we got this far we want to expand, so let's bump the number of times we've skipped this.
		iFlavorExpansion += kPlayer.GetMilitaryAI()->GetNumberOfTimesSettlerBuildSkippedOver();
		
		//Higher-level AI should expand more quickly.
		if (GC.getGame().getHandicapInfo().getAIDifficultyBonusBase() > 0)
		{
			iFlavorExpansion += GC.getGame().getHandicapInfo().getAIDifficultyBonusBase() * 10;
		}

		//250
		int iExpansionVal = GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER();
		iBonus += (iFlavorExpansion * iExpansionVal);
	}
	if(!kPlayer.isMinorCiv())
	{
		//Archaeologists? Only if we have digs nearby.
		if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ARCHAEOLOGIST)
		{
			if(kPlayer.isMinorCiv())
			{
				return 0;
			}
			EconomicAIStrategyTypes eWantArch = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_ARCHAEOLOGISTS");
			if(eWantArch != NO_ECONOMICAISTRATEGY)
			{
				if(!kPlayer.GetEconomicAI()->IsUsingStrategy(eWantArch))
				{
					return 0;
				}
				else
				{
					int iNumArch = kPlayer.GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true, false);
					if (iNumArch <= 0)
					{
						iBonus += 5000;
					}

					AIGrandStrategyTypes eGrandStrategy = kPlayer.GetGrandStrategyAI()->GetActiveGrandStrategy();
					bool bSeekingCultureVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
				
					if(bSeekingCultureVictory)
					{
						iBonus += 5000;
					}

					if(kPlayer.GetArchaeologicalDigTourism() > 0)
					{
						iBonus += 2500;
					}
					for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
					{
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
	}
	//Make sure we need naval workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER_SEA)
	{
		//There's a worker waiting here? Abort!
		if(m_pCity->plot()->getNumUnitsOfAIType(UNITAI_WORKER_SEA, m_pCity->getOwner()) > 0)
		{
			return 0;
		}
		AICityStrategyTypes eNoNavalWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_NAVAL_TILE_IMPROVEMENT");
		if(eNoNavalWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNoNavalWorkers))
		{
			return 0;
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
						iUnimprovedAround += m_pCity->GetNumResourceLocal(eResourceLoop);
					}
				}
			}
			iBonus += (1000 * iUnimprovedAround * (m_pCity->getPopulation() + kPlayer.GetCurrentEra()));

			//additional loop to help coastal and non-coastal cities nearby.
			int iCityLoop;
			for (CvCity* pLoopCity = kPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iCityLoop))
			{
				if (pLoopCity->getArea() == m_pCity->getArea())
				{
					AICityStrategyTypes eNeedNavalWorker = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");
					if (eNeedNavalWorker != NO_AICITYSTRATEGY && pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedNavalWorker))
						iBonus += (250 * (pLoopCity->getPopulation() + kPlayer.GetCurrentEra()));
				}
			}
		}
	}
	//Make sure we need workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER)
	{
		if(kPlayer.GetNumUnitsWithUnitAI(UNITAI_DEFENSE, true, false) <= 2)
		{
			iBonus += -100;
		}
		int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
		if(GC.getGame().getGameTurn() <= 25)
		{
			iBonus += -25;
		}
		else
		{
			if (iNumBuilders == 0)
			{
				if (kPlayer.isMinorCiv())
				{
					iBonus += 500 * (kPlayer.GetCurrentEra() + 1);
				}
				else
				{
					iBonus += 250 * (kPlayer.GetCurrentEra() + 1);
				}
			}
		}
		if (!kPlayer.isMinorCiv())
		{
			//There's a worker waiting here? Abort!
			if (m_pCity->plot()->getNumUnitsOfAIType(UNITAI_WORKER, m_pCity->getOwner()) > 0)
			{
				return 0;
			}
			int iCurrentNumCities = kPlayer.getNumCities();
			iCurrentNumCities -= iNumBuilders;
			if (iCurrentNumCities <= 0)
			{
				return 0;
			}
			AICityStrategyTypes eNoWorkers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_TILE_IMPROVERS");
			if (eNoWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNoWorkers))
			{
				return 0;
			}
			AICityStrategyTypes eWantWorkers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_WANT_TILE_IMPROVERS");
			if (eWantWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eWantWorkers))
			{
				iBonus += (750 * iCurrentNumCities);
			}
			AICityStrategyTypes eNeedWorkers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");
			if (eNeedWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedWorkers))
			{
				iBonus += (1250 * iCurrentNumCities);
			}

			if (!kPlayer.IsAtWar())
			{
				int iNumPillaged = m_pCity->GetNumPillagedPlots();
				if (iNumPillaged > 0)
				{
					iBonus += (500 * iNumPillaged);
				}
			}
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
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

				if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && kPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if (kPlayer.GetMilitaryAI()->GetWarType() == WARTYPE_LAND && eDomain == DOMAIN_LAND)
					{
						iWarBooster += 1;
					}
					else if (kPlayer.GetMilitaryAI()->GetWarType() == WARTYPE_SEA && eDomain == DOMAIN_SEA)
					{
						iWarBooster += 1;
					}
				}
			}
			iBonus += iWarBooster * 25;
		}
		if (bAtWar)
		{
			if (kPlayer.getNumCities() > 1 && m_pCity->GetThreatRank() != -1)
			{
				//More cities = more threat.
				int iThreat = (kPlayer.getNumCities() - m_pCity->GetThreatRank()) * 25;
				if (iThreat > 0)
				{
					if (bCombat)
					{
						iBonus += iThreat;
					}
					else
					{
						iBonus -= iThreat;
					}
				}
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
			if (m_pCity == kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, (DomainTypes)pkUnitEntry->GetDomainType()))
			{
				iBonus += 25 * kPlayer.getNumCities();
			}

		}
		//Promotion Bonus
		int iPromotionBonus = 0;
		for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkPromotionInfo)
			{
				if (pkUnitEntry->GetFreePromotions(iI))
				{
					if (!pkPromotionInfo->IsLostWithUpgrade())
					{ 
						iPromotionBonus += 25;
					}
				}
				if(kPlayer.IsFreePromotion(ePromotion))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iPromotionBonus += 25;
					}
				}
				if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitClass(iI, pkUnitEntry->GetUnitClassType()))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iPromotionBonus += 25;
					}
				}
				if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(iI, pkUnitEntry->GetUnitCombatType()))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iPromotionBonus += 25;
					}
				}
			}
		}
		
		if (iPromotionBonus != 0)
		{
			iBonus += iPromotionBonus;
		}
	
		//Uniques? They're generally good enough to spam.
		if(kPlayer.getCivilizationInfo().isCivilizationUnitOverridden(pkUnitEntry->GetUnitClassType()))
		{
			iBonus += 100;
		}

		//For an operation? Build it!
		if(bCombat)
		{
			if(pArmy != NULL && pArmy->GetGoalPlot() != NULL)
			{
				iBonus += 300;
			}
			else if(bForOperation)
			{
				iBonus += 300;
			}

			//slight bump in value for ranged units, to counteract some other elements.
			if (pkUnitEntry->GetRangedCombat() > 0)
			{
				iBonus += pkUnitEntry->GetRangedCombat();
			}

			if (kPlayer.GetDiversity(eDomain) == (int)pkUnitEntry->GetDefaultUnitAIType())
			{
				//special case for paradroppers, we don't want too many
				if (pkUnitEntry->GetDefaultUnitAIType() == UNITAI_PARADROP)
					iBonus += 25;
				else
					iBonus += 250;
			}

			if (eDomain == DOMAIN_LAND)
			{
				if (m_pCity->GetGarrisonedUnit() == NULL)
				{
					iBonus += 25;
				}
			}
			else if (eDomain == DOMAIN_SEA)
			{
				if (m_pCity->GetGarrisonedUnit() == NULL)
				{
					if (bForPurchase)
						return 0;

					iBonus -= 10;
				}
			}
		}
	}
	//Tiny army? Eek!
	if (kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 2) && pkUnitEntry->GetDomainType() == DOMAIN_LAND)
	{
		if(bCombat)
		{
			iBonus += 300;
		}
		//Fewer civilians til we rectify this!
		else
		{
			iBonus -= 100;
		}
	}
	
	if (bDesperate)
	{
		if (bCombat)
			iBonus += 500;
		else
			iBonus -= 500;
	}

	if (bCombat && pkUnitEntry->GetRangedCombat() <= 0 && pkUnitEntry->GetDomainType() == DOMAIN_LAND)
	{
		MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		MilitaryAIStrategyTypes eStrategyBarbsCritical = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS_CRITICAL");
		
		if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyBarbs))
		{
			iBonus += 25;
		}
		else if (kPlayer.GetMilitaryAI()->IsUsingStrategy(eStrategyBarbsCritical))
		{
			iBonus += 50;
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

	//prioritize siege cities for purchase
	if (bForPurchase && bCombat)
	{
		if (bDesperate)
			iBonus += 250;
	}

	if (!kPlayer.isMinorCiv())
	{
		//Debt is worth considering.
		bool bCloseToDebt = false;
		int iAverageGoldPerUnit = 0;

		if (bCombat && !pkUnitEntry->IsNoMaintenance() && !pkUnitEntry->IsTrade())
		{
			int iGoldSpentOnUnits = kPlayer.GetTreasury()->GetExpensePerTurnUnitMaintenance();
			iAverageGoldPerUnit = iGoldSpentOnUnits / (max(1, kPlayer.getNumUnits()));

			if (iGPT < iAverageGoldPerUnit * 3 && kPlayer.GetTreasury()->GetGold() <= iAverageGoldPerUnit * 20)
			{
				bCloseToDebt = true;
			}
		}
		if (bCloseToDebt && iAverageGoldPerUnit != 0)
		{
			iBonus += iAverageGoldPerUnit * -25;
			//At zero? Even more negative!
			if (kPlayer.GetTreasury()->GetGold() <= 0)
			{
				iBonus += -100;
			}
			if (iGPT <= 0)
			{
				iBonus += iGPT;
			}
		}
	}

	/////
	///WEIGHT
	//////

	if (bInterruptBuildings && bCombat)
		iBonus += 100;
	else if (bInterruptBuildings && !bCombat)
		iBonus -= 100;

	iTempWeight += iBonus;

	if (m_pCity->IsOccupied() && !m_pCity->IsNoOccupiedUnhappiness())
		iTempWeight /= 5;

	return iTempWeight;
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