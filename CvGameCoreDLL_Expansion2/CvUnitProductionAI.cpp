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
				}
			}
			if(bBad)
			{
				continue;
			}
#endif
			// Make sure this unit can be built now
			if(m_pCity->canTrain(eUnit))
			{
				// Make sure it matches the requested unit AI type
				if(eUnitAIType == NO_UNITAI || pkUnitInfo->GetUnitAIType(eUnitAIType))
				{
					// Update weight based on turns to construct
					iTurnsLeft = m_pCity->getProductionTurnsLeft(eUnit, 0);
					iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_UnitAIWeights.GetWeight(eUnit), iTurnsLeft);
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
int CvUnitProductionAI::CheckUnitBuildSanity(UnitTypes eUnit, bool bForOperation, CvArmyAI* pArmy, int iTempWeight, int iGPT, int iWaterRoutes, int iLandRoutes, bool bForPurchase)
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
		if (iTempWeight > 2000)
		{
			iTempWeight = 2000;
		}
	}
	else
	{
		if (iTempWeight > 1600)
		{
			iTempWeight = 1600;
		}
	}

	if (!pkUnitEntry)
		return 0;

	if (bForPurchase && !m_pCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_GOLD))
	{
		return 0;
	}

	if (!bForPurchase && m_pCity->IsPuppet())
	{
		return 0;
	}

	if ((m_pCity->isUnderSiege() || m_pCity->isInDangerOfFalling()) && !bCombat)
	{
		return 0;
	}

	//% Value that will modify the base value.
	int iBonus = 0;

	bool bAtWar = false;
	if(kPlayer.isMinorCiv())
	{
		if (bCombat)
		{
			int iNumUnits = kPlayer.getNumMilitaryUnits();
			int iNumCities = kPlayer.getNumCities();
			int iEra = (kPlayer.GetCurrentEra() + 1) * iNumCities;
			if (iNumUnits >= iEra)
			{
				return 0;
			}
			else
			{
				iBonus += (iEra - iNumUnits) * 50;
			}
		}
	}
	else
	{
		bAtWar = (kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0);
		if(bCombat && kPlayer.GetNumUnitsOutOfSupply() > 0)
		{
			return 0;
		}
	}

	//Are we alone?
	DomainTypes eDomain = (DomainTypes) pkUnitEntry->GetDomainType();
	bool bAlone = true;
	if(kPlayer.isMinorCiv())
	{
		bAlone = false;
	}
	if(bCombat && !kPlayer.isMinorCiv())
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				if(GET_PLAYER(eLoopPlayer).GetProximityToPlayer(kPlayer.GetID()) >= PLAYER_PROXIMITY_CLOSE)
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
		if (!bOperationalOverride && ((iNumUnitsofMine * iFactor > iWaterTiles) || ((iNumUnitsOther==0 && iNumCitiesOther==0))))
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
				iBonus += 50 * kPlayer.getNumResourceAvailable((ResourceTypes)pkUnitEntry->GetResourceType(), false);
			}
			else
			{
				return 0;
			}
		}
		//Check for specific resource usage by units.
		if(pkUnitEntry->GetSpaceshipProject() == NO_PROJECT)
		{
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
								if(kPlayer.getNumResourceAvailable(eResourceLoop, false) <= 4)
								{
									return 0;
								}
							}
						}
						if(pkUnitEntry->GetResourceQuantityRequirement(iResourceLoop) > 0)
						{
							iBonus += (50 * kPlayer.getNumResourceAvailable(eResourceLoop, false));
						}
					}
				}
			}
		}

		//Let's look at what this can build (if a combat unit).
		if (bCombat)
		{
			for (int i = 0; i < GC.getNumBuildInfos(); i++)
			{
				CvBuildInfo* pkBuild = GC.getBuildInfo((BuildTypes)i);
					
				if (pkBuild && (pkUnitEntry->GetBuilds((BuildTypes)i) || kPlayer.GetPlayerTraits()->HasUnitClassCanBuild(i, pkUnitEntry->GetUnitClassType())))
				{
					iBonus += 10;
				}
			}
		}

		///////////////
		//UNIT TYPE CHECKS
		//////////////////////
#if defined(MOD_BALANCE_CORE_UNIT_INVESTMENTS)
		if(MOD_BALANCE_CORE_UNIT_INVESTMENTS)
		{
			//Virtually force this.
			const UnitClassTypes eUnitClass = (UnitClassTypes)(pkUnitEntry->GetUnitClassType());
			if(m_pCity->IsUnitInvestment(eUnitClass))
			{
				iBonus += 500;
			}
		}
#endif

		//Sanity check for buildable support units.
		if(pkUnitEntry->IsCityAttackSupport())
		{
			int iNum = kPlayer.GetNumUnitsWithUnitAI(UNITAI_CITY_BOMBARD, true, false);
			if(iNum >= kPlayer.getNumCities())
			{
				return 0;
			}
			else
			{
				iBonus += 20;
			}
		}

		//Suicide units? Hmm...
		if(pkUnitEntry->IsSuicide())
		{
			//Nukes!!!
			if(pkUnitEntry->GetNukeDamageLevel() > 0)
			{
				iBonus += 50;
			}
			//Cruise Missiles? Only if we don't have any nukes lying around...
			else if(pkUnitEntry->GetRangedCombat() > 0 && kPlayer.getNumNukeUnits() > 0)
			{
				iBonus -= 50;
			}
		}


		//Policy unlock or move-on-purchase? These are usually cheap and good, so get em!
		if(pkUnitEntry->GetPolicyType() != NO_POLICY || pkUnitEntry->CanMoveAfterPurchase())
		{
			if(bCombat && bAtWar)
			{
				int iGoldCost = m_pCity->GetPurchaseCost(eUnit);
				int iSurplus = ((kPlayer.GetTreasury()->GetGold() - iGoldCost) / 50);
				if(iSurplus > 0)
				{
					//Bonus based on difference in gold - the more money we have, the more we want this!
					iBonus += iSurplus;
				}
			}
			else if(bCombat && !bAtWar)
			{
				int iGoldCost = m_pCity->GetPurchaseCost(eUnit);
				int iSurplus = ((kPlayer.GetTreasury()->GetGold() - iGoldCost) / 100);
				if(iSurplus > 0)
				{
					//Bonus based on difference in gold - the more money we have, the more we want this!
					iBonus += iSurplus;
				}
			}
			else if(!bCombat)
			{
				int iGoldCost = m_pCity->GetPurchaseCost(eUnit);
				int iSurplus = ((kPlayer.GetTreasury()->GetGold() - iGoldCost) / 100);
				if(iSurplus > 0)
				{
					//Bonus based on difference in gold - the more money we have, the more we want this!
					iBonus += iSurplus;
				}
			}
		}

		//Carriers? Only if we need them.
		if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_CARRIER_SEA)
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
				iBonus += 500;
			}
			//If we have more planes than slots (negative), we need this!
			else
			{
				iBonus += (-3 * iSlots);
			}
		}
		//Need Explorers?
		if (eDomain == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE)
		{
			int iExplore = kPlayer.GetEconomicAI()->GetExplorersNeeded();
			if(iExplore > 0)
			{
				iBonus += iExplore * 60;
			}
		}
		//Need Sea Explorers?
		if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
		{
			int iExplore = kPlayer.GetEconomicAI()->GetNavalExplorersNeeded();
			if(iExplore > 0)
			{
				iBonus += iExplore * 15;
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
				
				if(bAtWar)
				{
					if(kPlayer.GetMilitaryAI()->GetWarType() == 2)
					{
						iValue *= 8;
					}
					else
					{
						iValue *= 4;
					}
				}
				else if(bAlone)
				{
					iValue *= 4;
				}
				if(iValue > 0)
				{
					iBonus += iValue;
				}
			}
		}
		//Land Units Critically Needed?
		else if (eDomain == DOMAIN_LAND)
		{
			if(bCombat)
			{
				int iCurrent = kPlayer.GetMilitaryAI()->GetNumLandUnits();
				int iDesired = kPlayer.GetMilitaryAI()->GetRecommendLandArmySize();
				int iValue = iDesired - iCurrent;
				if(bAtWar)
				{
					if(kPlayer.GetMilitaryAI()->GetWarType() == 1)
					{
						iValue *= 8;
					}
					else
					{
						iValue *= 4;
					}
				}
				else if(bAlone)
				{
					iValue /= 4;
				}
				if(iValue > 0)
				{
					iBonus += iValue;
				}
			}
		}
		//Air Units Critically Needed?
		if (eDomain == DOMAIN_AIR)
		{
			int iAircraft = kPlayer.GetMilitaryAI()->GetNumFreeCargo();
			if(iAircraft <= 0)
			{
				return 0;
			}
			else
			{
				iBonus += (25 * iAircraft);
			}
		}

		//Air defense needed?
		if(eDomain == DOMAIN_LAND && pkUnitEntry->GetAirInterceptRange() > 0)
		{
			int iNumAA = kPlayer.GetMilitaryAI()->GetNumAAUnits();
			int iNumCities = kPlayer.getNumCities();
			
			iBonus += ((iNumCities - iNumAA) * 10);
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
			iBonus += (iProductionBonus / 2);
		}
		
#if defined(MOD_DIPLOMACY_CITYSTATES)	
		//Diplomatic Units!
		if(MOD_DIPLOMACY_CITYSTATES &&  pkUnitEntry->GetUnitAIType(UNITAI_MESSENGER))
		{
			if (GC.getGame().GetNumMinorCivsAlive() <= 0)
				return 0;

			int iInfluence = 0;
			//Promotion Bonus
			for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				if(pkUnitEntry->GetFreePromotions(iI))
				{
					PromotionTypes ePromotion = (PromotionTypes) iI;

					if(GC.getPromotionInfo(ePromotion)->GetTradeMissionInfluenceModifier() > 0)
					{
						iInfluence = GC.getPromotionInfo(ePromotion)->GetTradeMissionInfluenceModifier();
						break;
					}
				}
			}
			EconomicAIStrategyTypes eNeedDiplomats = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS");
			EconomicAIStrategyTypes eNeedDiplomatsCrit = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");
			if(eNeedDiplomats != NO_ECONOMICAISTRATEGY && eNeedDiplomatsCrit != NO_ECONOMICAISTRATEGY)
			{
				if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedDiplomats))
				{
					iInfluence *= 5;
				}
				else if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedDiplomatsCrit))
				{
					iInfluence *= 15;
				}
				else
				{
					iInfluence /= 4;
				}
			}
			bool bAlwaysOne = false;
			if(kPlayer.GetPlayerTraits()->IsDiplomaticMarriage())
			{
				iInfluence *= 5;
				bAlwaysOne = true;
			}
			else if(kPlayer.GetPlayerTraits()->GetCityStateCombatModifier() > 0)
			{
				iInfluence *= 5;
				bAlwaysOne = true;
			}
			else if(kPlayer.GetPlayerTraits()->GetCityStateFriendshipModifier() > 0)
			{
				iInfluence *= 5;
				bAlwaysOne = true;
			}
			else if(kPlayer.GetPlayerTraits()->GetCityStateBonusModifier() > 0)
			{
				iInfluence *= 5;
				bAlwaysOne = true;
			}
			for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				const YieldTypes eYield = static_cast<YieldTypes>(iI);
				if(eYield != NO_YIELD)
				{
					if(kPlayer.GetPlayerTraits()->GetYieldFromCSAlly(eYield) > 0 || kPlayer.GetPlayerTraits()->GetYieldFromCSFriend(eYield) > 0)
					{
						iInfluence *= 5;
						bAlwaysOne = true;
						break;
					}
				}
			}
			if(bAlwaysOne && kPlayer.GetNumUnitsWithUnitAI(UNITAI_MESSENGER, true, true) <= 0)
			{
				iInfluence *= 10;
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
				iBonus -= 250;
			}
			else if(kPlayer.GetDiplomacyAI()->IsCloseToDominationVictory())
			{
				iBonus -= 250;
			}
			else if(kPlayer.GetDiplomacyAI()->IsCloseToDiploVictory())
			{
				iBonus -= 250;
			}

			if(pkUnitEntry->GetSpaceshipProject() != NO_PROJECT)
			{
				if(m_pCity->getSpaceProductionModifier() > 0)
				{
					iBonus += (m_pCity->getSpaceProductionModifier() * 10);
				}
				else
				{
					iBonus -= 250;
				}
			}
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
								iReligiousBonus += (pEntry->GetFaithFromKills());
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
										iReligiousBonus += (pEntry->GetYieldFromKills(eYield));
									}
								}
							}
						}
					}
				}
			}
			iReligiousBonus /= 5;
			iBonus += iReligiousBonus;
		}

		//////////////
		//CIVILIAN CHECKS
		//////////////////////

		if(pkUnitEntry->IsTrade())
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
				iBonus += (iGPT * -10);
			}
			int iUnhappyGold = m_pCity->getUnhappinessFromGold();
			if (iUnhappyGold > 0)
			{
				iBonus += (iUnhappyGold * 25);
			}
		}
	}

	//Settlers? Let's see...
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
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
		int iFirstUnitID = 0;
		//There's a settler waiting here? Abort!
		if(m_pCity->plot()->getNumUnitsOfAIType(UNITAI_SETTLE, iFirstUnitID) > 0)
		{
			return 0;
		}
		//Don't build a settler if we're about to grow.
		if(m_pCity->getFoodTurnsLeft() <= 2)
		{
			return 0;
		}
		//Or if we're small.
		if(m_pCity->getPopulation() <= 4)
		{
			return 0;
		}
		if(kPlayer.isBarbarian() || kPlayer.GetPlayerTraits()->IsNoAnnexing() || (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && kPlayer.isHuman()))
		{
			return 0;
		}
		if(kPlayer.IsEmpireSuperUnhappy())
		{
			return 0;
		}
		//Already have a settler out? Ignore.
		int iNumSettlers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true, true);
		if(iNumSettlers > 1)
		{
			return 0;
		}
		
		int iSettleValuation = m_pCity->GetNearbySettleSiteValue();
		if(iSettleValuation <= 0)
		{
			return 0;
		}
		//We have a good spot? Okay, let's see how important that is to us.
		else
		{
			int iFlavorExpansion = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));

			// If we are running "ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS"
			EconomicAIStrategyTypes eExpandOther = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
			if (eExpandOther != NO_ECONOMICAISTRATEGY)
			{
				if (kPlayer.GetEconomicAI()->IsUsingStrategy(eExpandOther))
				{
					iFlavorExpansion += 2;
				}
			}

			// If we are running "ECONOMICAISTRATEGY_EARLY_EXPANSION"
			EconomicAIStrategyTypes eEarlyExpand = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
			if (eEarlyExpand != NO_ECONOMICAISTRATEGY)
			{
				if (kPlayer.GetEconomicAI()->IsUsingStrategy(eEarlyExpand))
				{
					iFlavorExpansion += 3;
				}
			}

			// If we are running "ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY"
			EconomicAIStrategyTypes eExpandCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY");
			if (eExpandCrazy != NO_ECONOMICAISTRATEGY)
			{
				if (kPlayer.GetEconomicAI()->IsUsingStrategy(eExpandCrazy))
				{
					iFlavorExpansion += 3;
				}
			}

			if(kPlayer.getSettlerProductionModifier() > 0)
			{
				iFlavorExpansion += 1;
			}
			if(m_pCity->isCapital() && kPlayer.getCapitalSettlerProductionModifier() > 0)
			{
				iFlavorExpansion += 1;
			}

			if(kPlayer.IsEmpireUnhappy() && (kPlayer.GetNumCitiesFounded() > (kPlayer.GetDiplomacyAI()->GetBoldness())))
			{
				iFlavorExpansion -= 3;
			}
			if(kPlayer.GetDiplomacyAI()->IsGoingForCultureVictory() && (kPlayer.GetNumCitiesFounded() > (kPlayer.GetDiplomacyAI()->GetBoldness())))
			{
				iFlavorExpansion -= 2;
			}

			MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
			// scale based on flavor and world size
			if(eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
			{
				iFlavorExpansion -= 1;
			}
			int iNumCities = kPlayer.getNumCities();
			
			iFlavorExpansion -= iNumCities;
			
			if(iFlavorExpansion <= 0)
			{
				iFlavorExpansion = 1;
			}
			
			int iSettlerDesire = (iFlavorExpansion * iSettleValuation);
			iBonus += iSettlerDesire;
		}
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
					int iNumSettlers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true, false);
					if (iNumSettlers <= 0)
					{
						iBonus += 500;
					}

					AIGrandStrategyTypes eGrandStrategy = kPlayer.GetGrandStrategyAI()->GetActiveGrandStrategy();
					bool bSeekingCultureVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
				
					iBonus += 200;
					if(bSeekingCultureVictory)
					{
						iBonus += 400;
					}

					if(kPlayer.GetArchaeologicalDigTourism() > 0)
					{
						iBonus += 300;
					}
					for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
					{
						const YieldTypes eYield = static_cast<YieldTypes>(iI);
						if(eYield != NO_YIELD)
						{
							if(kPlayer.GetPlayerTraits()->GetArtifactYieldChanges(eYield) > 0)
							{
								iBonus += 200;
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
		int iFirstUnitID;
		//There's a worker waiting here? Abort!
		if(m_pCity->plot()->getNumUnitsOfAIType(UNITAI_WORKER_SEA, iFirstUnitID) > 0)
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
			iBonus += 500;
		}
	}
	//Make sure we need workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER)
	{
		if(kPlayer.GetNumUnitsWithUnitAI(UNITAI_DEFENSE, true, false) <= 2)
		{
			iBonus += -100;
		}
		if(GC.getGame().getGameTurn() <= 20)
		{
			iBonus += -200;
		}
		int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
		if(iNumBuilders == 0)
		{
			iBonus += 150;
		}
		if (!kPlayer.isMinorCiv())
		{
			int iFirstUnitID;
			//There's a worker waiting here? Abort!
			if (m_pCity->plot()->getNumUnitsOfAIType(UNITAI_WORKER, iFirstUnitID) > 0)
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
				iBonus += (50 * iCurrentNumCities);
			}
			AICityStrategyTypes eNeedWorkers = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");
			if (eNeedWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedWorkers))
			{
				iBonus += (150 * iCurrentNumCities);
			}
		}
	}
	
	if(!kPlayer.isMinorCiv())
	{
		//////////////////
		//WAR BOOSTERS
		////////////////////////
		if(pkUnitEntry->GetCombat() > 0)
		{
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if(kPlayer.GetDiplomacyAI()->GetWarState(eLoopPlayer) < WAR_STATE_STALEMATE || kPlayer.GetDiplomacyAI()->IsWantsSneakAttack(eLoopPlayer))
					{
						if(kPlayer.GetMilitaryAI()->GetWarType() == 1 && eDomain == DOMAIN_LAND)
						{
							iBonus += 200;
						}
						else if(kPlayer.GetMilitaryAI()->GetWarType() == 2 && eDomain == DOMAIN_SEA)
						{
							iBonus += 200;
						}
						else
						{
							iBonus += 100;
						}
					}
				}
			}
		}
		if(bAtWar)
		{
			if(kPlayer.getNumCities() > 1 && m_pCity->GetThreatRank() != -1)
			{
				//More cities = more threat.
				int iThreat = (kPlayer.getNumCities() - m_pCity->GetThreatRank()) * 75;
				if(iThreat > 0)
				{
					if(bCombat)
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
			if(kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver() > 0)
			{
				iBonus += (kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver() * 10);
			}
		}

		/////////////////////
		// EXPERIENCE BOOSTERS
		/////////////////////
		if(bCombat)
		{
			//Let's try to build our units in our best cities only.
			if(m_pCity == kPlayer.GetBestMilitaryCity((UnitCombatTypes)pkUnitEntry->GetUnitCombatType()))
			{
				iBonus += 150;
			}
			//Let's try to build our units in our best cities only. More cities we have, the more this matters.
			if(m_pCity == kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, (DomainTypes)pkUnitEntry->GetDomainType()))
			{
				iBonus += 150;
			}
		}
		//Promotion Bonus
		for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkPromotionInfo)
			{
				if(kPlayer.IsFreePromotion(ePromotion))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iBonus += 50;
					}
				}
				if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitClass(iI, pkUnitEntry->GetUnitClassType()))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iBonus += 50;
					}
				}
				if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(iI, pkUnitEntry->GetUnitCombatType()))
				{
					if(::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
					{
						iBonus += 50;
					}
				}
			}
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
				iBonus += 250;
			}
			else if(bForOperation)
			{
				iBonus += 250;
			}
			if (eDomain == DOMAIN_LAND)
			{
				//////Let's get the military unit AI type we have the least of and boost the lowest type.
				if(kPlayer.GetArmyDiversity() == (int)pkUnitEntry->GetDefaultUnitAIType())
				{
					iBonus += 250;
				}
			}
			else if (eDomain == DOMAIN_SEA)
			{
				//////Let's get the military unit AI type we have the least of and boost the lowest type.
				if (kPlayer.GetNavyDiversity() == (int)pkUnitEntry->GetDefaultUnitAIType())
				{
					iBonus += 250;
				}
			}
		}
	}
	//Tiny army? Eek!
	if(kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 2))
	{
		if(bCombat)
		{
			iBonus += 1000;
		}
		//Fewer civilians til we rectify this!
		else
		{
			iBonus -= 100;
		}
	}

	//Debt is worth considering.
	if(bCombat && !pkUnitEntry->IsNoMaintenance() && !pkUnitEntry->IsTrade())
	{
		if (iGPT < 0)
		{
			//Every -1 GPT = -100 penalty.
			if (!bAtWar)
			{
				iBonus += iGPT * 100;
				//At zero? Even more negative!
				if (kPlayer.GetTreasury()->GetGold() <= 0)
				{
					iBonus += -500;
				}
			}
		}
	}

	//Let's check this against supply to keep our military numbers lean.
	if(bCombat && !bAtWar)
	{
		int iSupply = kPlayer.GetNumUnitsSupplied();
		if(iSupply <= 0)
		{
			iSupply = 1;
		}
		int iDemand = kPlayer.getNumMilitaryUnits();
		int iPercent = (iDemand * 100) / iSupply;
		int iRemainder = (140 - iPercent);

		//Closer we get to cap over 40%, fewer units we should be making.
		iBonus *= iRemainder;
		iBonus /= 100;
	}

	/////
	///WEIGHT
	//////

	iTempWeight *= (iBonus + 100);
	iTempWeight /= 100;

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