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
int CvUnitProductionAI::CheckUnitBuildSanity(UnitTypes eUnit, bool bForOperation, CvArmyAI* pArmy, int iTempWeight)
{
	bool bOperationalOverride = false;
	CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
	if (!pkUnitEntry)
		return 0;

	if(iTempWeight == 0)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if(m_pCity->IsPuppet())
	{
		return 0;
	}
	
	if(kPlayer.isMinorCiv())
	{
		int iNumUnits = kPlayer.getNumMilitaryUnits();
		if(iNumUnits >= (6 * kPlayer.getNumCities()))
		{
			return 0;
		}
	}
	else
	{
		if(pkUnitEntry->GetCombat() > 0)
		{
			if(kPlayer.GetNumUnitsOutOfSupply() > 0)
			{
				return 0;
			}
		}
	}

	//% Value that will modify the base value.
	int iBonus = 0;
	
	//////////////////
	//DOMAINS AND LOCATION CHECKS
	////////////////////////

	// sanity check for building ships on isolated small inland seas (not lakes)
	DomainTypes eDomain = (DomainTypes) pkUnitEntry->GetDomainType();
	if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_WORKER_SEA) // if needed allow workboats...
	{
		CvArea* pBiggestNearbyBodyOfWater = m_pCity->waterArea();
		if (pBiggestNearbyBodyOfWater)
		{
			int iWaterTiles = pBiggestNearbyBodyOfWater->getNumTiles();
			int iNumUnitsofMine = pBiggestNearbyBodyOfWater->getUnitsPerPlayer(m_pCity->getOwner());
			int iNumUnitsOther = pBiggestNearbyBodyOfWater->getNumUnits()-iNumUnitsofMine;
			int iNumCitiesofMine = pBiggestNearbyBodyOfWater->getCitiesPerPlayer(m_pCity->getOwner());
			int iNumCitiesOther = pBiggestNearbyBodyOfWater->getNumCities()-iNumCitiesofMine;

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
		else // this should never happen, but...
			return 0;
	}
	//Are we alone?
	if(pkUnitEntry->GetCombat() > 0)
	{
		bool bAlone = true;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				if(GET_PLAYER(eLoopPlayer).GetProximityToPlayer(kPlayer.GetID()) > PLAYER_PROXIMITY_CLOSE)
				{
					bAlone = false;
					break;
				}
			}
		}
		if(bAlone)
		{
			if(eDomain == DOMAIN_LAND)
			{
				iBonus -= 50;
			}
			else if(eDomain == DOMAIN_SEA)
			{
				iBonus += 50;
			}
		}
	}
				
	//Building land units on a tiny island? Only if we lack a garrison.
	if(pkUnitEntry->GetCombat() > 0)
	{
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
		EconomicAIStrategyTypes eStrategyIslandStart = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ISLAND_START");
		if(eStrategyIslandStart != NO_ECONOMICAISTRATEGY)
		{
			if(kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyIslandStart))
			{
				if(eDomain == DOMAIN_LAND)
				{
					iBonus -= 50;
				}
				else if(eDomain == DOMAIN_SEA)
				{
					iBonus += 50;
				}
			}
		}
	}

	/////////
	//RESOURCE CHECKS
	////////////
	//Check for special unlimited units from specific resources (not a strategic check)
	if(pkUnitEntry->GetResourceType() != NO_RESOURCE)
	{
		if(kPlayer.getNumResourceAvailable((ResourceTypes)pkUnitEntry->GetResourceType(), false) > 0)
		{
			iBonus += 33;
		}
		else
		{
			return 0;
		}
	}
	//Check for specific resource usage by units.
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
					if(pkUnitEntry->GetResourceQuantityRequirement(eResourceLoop) > 0)
					{
						//We need at least 5 aluminum to get off the planet, so let's save that much if we've got the Apollo.
						if(kPlayer.getNumResourceAvailable(eResourceLoop, false) <= 5)
						{
							return 0;
						}
					}
				}
				if(pkUnitEntry->GetResourceQuantityRequirement(iResourceLoop) > 0)
				{
					iBonus += (kPlayer.getNumResourceAvailable(eResourceLoop, false) * 5);
				}
			}
		}
	}

	//Let's look at what this can build (if a combat unit).
	for (int i = 0; i < GC.getNumBuildInfos(); i++) 
	{
		CvBuildInfo* pkBuild = GC.getBuildInfo((BuildTypes)i);
					
		if (pkBuild && (pkUnitEntry->GetBuilds((BuildTypes)i) || kPlayer.GetPlayerTraits()->HasUnitClassCanBuild(i, pkUnitEntry->GetUnitClassType())))
		{
			if(pkUnitEntry->GetCombat() > 0)
			{
				iBonus += 25;
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
			iBonus += 100;
		}
	}
#endif

	//Sanity check for buildable support units.
	if(pkUnitEntry->IsCityAttackSupport())
	{
		int iNum = kPlayer.GetNumUnitsWithUnitAI(UNITAI_CITY_BOMBARD, true, true);
		if(iNum >= kPlayer.getNumCities())
		{
			return 0;
		}
		else
		{
			iBonus += 50;
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
	}


	//Policy unlock or move-on-purchase? These are usually cheap and good, so get em!
	if(pkUnitEntry->GetPolicyType() != NO_POLICY || pkUnitEntry->CanMoveAfterPurchase())
	{
		int iGoldCost = m_pCity->GetPurchaseCost(eUnit);
		if((kPlayer.GetTreasury()->GetGold() - iGoldCost) > 0)
		{
			//Bonus based on difference in gold - the more money we have, the more we want this!
			iBonus += ((kPlayer.GetTreasury()->GetGold() - iGoldCost) * 10);
		}
		//Esp if at war!
		if(kPlayer.IsAtWarAnyMajor())
		{
			iBonus += 50;
		}
	}

	//Carriers? Only if we need them.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_CARRIER_SEA)
	{
		MilitaryAIStrategyTypes eNeedCarriers = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_AIR_CARRIER");
		if(eNeedCarriers != NO_ECONOMICAISTRATEGY)
		{
			if(kPlayer.GetMilitaryAI()->IsUsingStrategy(eNeedCarriers))
			{
				iBonus += 75;
			}
			else
			{
				return 0;
			}
		}
	}
	//Need Explorers?
	if (eDomain == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE)
	{
		EconomicAIStrategyTypes eNeedRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON");	
		if(eNeedRecon != NO_ECONOMICAISTRATEGY)
		{
			if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedRecon))
			{
				iBonus += 100;
			}
			else
			{
				return 0;
			}
		}
	}
	//Need Sea Explorers?
	if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
	{
		EconomicAIStrategyTypes eNeedRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");	
		if(eNeedRecon != NO_ECONOMICAISTRATEGY)
		{
			if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedRecon))
			{
				iBonus += 100;
			}
			else
			{
				return 0;
			}
		}
	}
	//Naval Units Critically Needed?
	if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_WORKER_SEA)
	{
		MilitaryAIStrategyTypes eNeedBoatsCritical = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS_CRITICAL");
		if(eNeedBoatsCritical != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eNeedBoatsCritical))
		{
			iBonus += 50;
		}
		MilitaryAIStrategyTypes eNeedBoats = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS");
		if(eNeedBoats != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eNeedBoats))
		{
			iBonus += 25;
		}
	}
	//Land Units Critically Needed?
	if (eDomain == DOMAIN_LAND)
	{
		MilitaryAIStrategyTypes eNeedLandCritical = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");
		if(eNeedLandCritical != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eNeedLandCritical))
		{
			if(pkUnitEntry->GetCombat() > 0)
			{
				iBonus += 50;
			}
			else
			{
				iBonus -= 25;
			}
		}
		MilitaryAIStrategyTypes eNeedLand = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE");
		if(eNeedLand != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eNeedLand))
		{
			if(pkUnitEntry->GetCombat() > 0)
			{
				iBonus += 15;
			}
			else
			{
				iBonus -= 15;
			}
		}
		MilitaryAIStrategyTypes eBarb = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		if(eBarb != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBarb))
		{
			if(pkUnitEntry->GetCombat() > 0)
			{
				iBonus += 20;
			}
			else
			{
				iBonus -= 20;
			}
		}
		MilitaryAIStrategyTypes eBarbCrit = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS_CRITICAL");
		if(eBarbCrit != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBarbCrit))
		{
			if(pkUnitEntry->GetCombat() > 0)
			{
				iBonus += 40;
			}
			else
			{
				iBonus -= 40;
			}
		}
	}
	//Air Units Critically Needed?
	if (eDomain == DOMAIN_AIR)
	{
		MilitaryAIStrategyTypes eNeedAir = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_AIR");
		if(eNeedAir != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eNeedAir))
		{
			iBonus += 75;
		}
	}
	//Air Defense Critically Needed?
	MilitaryAIStrategyTypes eNeedAirDefense = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_ANTIAIR");
	if(eNeedAirDefense != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eNeedAirDefense))
	{
		for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if(pkUnitEntry->GetFreePromotions(iI))
			{
				PromotionTypes ePromotion = (PromotionTypes) iI;

				if(GC.getPromotionInfo(ePromotion)->GetInterceptChanceChange() > 0)
				{
					iBonus += 75;
				}
			}
		}
	}

#if defined(MOD_DIPLOMACY_CITYSTATES)
	if(MOD_DIPLOMACY_CITYSTATES && (pkUnitEntry->GetUnitAIType(UNITAI_MESSENGER) || pkUnitEntry->GetUnitAIType(UNITAI_DIPLOMAT)))
	{
		EconomicAIStrategyTypes eNeedDiplomats = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS");	
		if(eNeedDiplomats != NO_ECONOMICAISTRATEGY)
		{
			if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedDiplomats))
			{
				iBonus += 100;
			}
			EconomicAIStrategyTypes eNeedDiplomatsCrit = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");	
			if(eNeedDiplomatsCrit != NO_ECONOMICAISTRATEGY)
			{
				if(kPlayer.GetEconomicAI()->IsUsingStrategy(eNeedDiplomatsCrit))
				{
					iBonus += 250;
				}
			}
		}
	}
#endif
	
	/////////////
	//GRAND STRATEGY CHECKS
	//////////////////
	EconomicAIStrategyTypes eStrategyConquest = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CONQUEST");				
	if(pkUnitEntry->GetCombat() > 0)
	{
		if(eStrategyConquest != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
		{
			iBonus += 25;
		}
	}

	EconomicAIStrategyTypes eStrategyUNGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_DIPLOMACY");	
	if(eStrategyUNGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyUNGS))
	{
		//Promotion Bonus
		for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if(pkUnitEntry->GetFreePromotions(iI))
			{
				PromotionTypes ePromotion = (PromotionTypes) iI;

				if(GC.getPromotionInfo(ePromotion)->GetTradeMissionInfluenceModifier() > 0)
				{
					iBonus += GC.getPromotionInfo(ePromotion)->GetTradeMissionInfluenceModifier();
				}
			}
		}
	}

	EconomicAIStrategyTypes eStrategySS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP");	
	if(eStrategyUNGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategySS))
	{
		if(pkUnitEntry->GetSpaceshipProject() != NO_PROJECT)
		{
			if(m_pCity->getSpaceProductionModifier() > 0)
			{
				iBonus += (m_pCity->getSpaceProductionModifier() * 3);
			}
			else
			{
				iBonus -= 33;
			}
		}
	}

	/////////
	//RELIGION CHECKS
	////////////
	if(pkUnitEntry->GetCombat() > 0)
	{
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
							iBonus += (pEntry->GetFaithFromKills() / 15);
						}
						if(pEntry->GetCombatModifierEnemyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
						{
							iBonus += (pEntry->GetCombatModifierEnemyCities() / 2);
						}
						if(pEntry->GetCombatModifierFriendlyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
						{
							iBonus += (pEntry->GetCombatModifierFriendlyCities() / 2);
						}
						if(pEntry->GetCombatVersusOtherReligionOwnLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
						{
							iBonus += (pEntry->GetCombatVersusOtherReligionOwnLands() / 2);
						}
						if(pEntry->GetCombatVersusOtherReligionTheirLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
						{
							iBonus += (pEntry->GetCombatVersusOtherReligionTheirLands() / 2);
						}
						for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							const YieldTypes eYield = static_cast<YieldTypes>(iI);
							if(eYield != NO_YIELD)
							{
								if(pEntry->GetYieldFromKills(eYield) > 0)
								{
									iBonus += (pEntry->GetYieldFromKills(eYield) / 10);
								}
							}
						}
					}
				}
			}
		}
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
		if(!m_pCity->isCapital())
		{
			if(m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == YIELD_GOLD)
			{
				iBonus += 75;
			}
			else
			{
				iBonus += 25;
			}
		}
		if(m_pCity->isCapital())
		{
			if(m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == YIELD_GOLD)
			{
				iBonus += 150;
			}
			else
			{
				iBonus += 50;
			}
		}
	}

	//Settlers? Let's see...
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
	{
		//No escorts? No trip.
		if(kPlayer.getNumMilitaryUnits() <= 3)
		{
			iBonus -= 500;
		}
		if(kPlayer.isMinorCiv())
		{
			return 0;
		}
		//Don't build a settler if we're about to grow.
		if(m_pCity->getFoodTurnsLeft() <= 3)
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
		if(kPlayer.getNumCities() <= 1)
		{
			iBonus += 200;
		}
		else
		{
			EconomicAIStrategyTypes eStrategyEnoughSettlers = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
			if(eStrategyEnoughSettlers != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyEnoughSettlers))
			{
				return 0;
			}
			AICityStrategyTypes eStrategyEnoughSettlers2 = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_SETTLERS");
			if(eStrategyEnoughSettlers2 != NO_AICITYSTRATEGY)
			{
				if(m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyEnoughSettlers2))
				{
					return 0;
				}
			}

			EconomicAIStrategyTypes eStrategyExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_REALLY_EXPAND_TO_OTHER_CONTINENTS");
			EconomicAIStrategyTypes eExpandLikeCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY");
			AICityStrategyTypes eFeederCity = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEW_CONTINENT_FEEDER");

			if (eStrategyExpandToOtherContinents != NO_ECONOMICAISTRATEGY)
			{
				if (kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyExpandToOtherContinents))
				{
					iBonus += 75;
				}
			}
			else if (eExpandLikeCrazy != NO_ECONOMICAISTRATEGY)
			{
				if (kPlayer.GetEconomicAI()->IsUsingStrategy(eExpandLikeCrazy))
				{
					iBonus += 150;
				}
			}
			if(eFeederCity != NO_AICITYSTRATEGY)
			{
				if(m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eFeederCity))
				{
					iBonus += 50;
				}
			}
			
			if(kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE) > 1)
			{
				iBonus /= 100;
			}
			if(kPlayer.getSettlerProductionModifier() > 0)
			{
				iBonus += (kPlayer.getSettlerProductionModifier() / 2);
			}
			if(m_pCity->isCapital() && kPlayer.getCapitalSettlerProductionModifier() > 0)
			{
				iBonus += (kPlayer.getCapitalSettlerProductionModifier() / 2);
			}
			
			int iBestArea;
			int iSecondBestArea;
			int iNumAreas;
			iNumAreas = kPlayer.GetBestSettleAreas(kPlayer.GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
			if(iNumAreas == 0)
			{
				return 0;
			}
			else
			{
				iBonus += (75 * max(1, iNumAreas));
			}
		}
	}

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
				AIGrandStrategyTypes eGrandStrategy = kPlayer.GetGrandStrategyAI()->GetActiveGrandStrategy();
				bool bSeekingCultureVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
				
				iBonus += 200;
				if(bSeekingCultureVictory)
				{
					iBonus += 100;
				}

				if(kPlayer.GetArchaeologicalDigTourism() > 0)
				{
					iBonus += 300;
				}
			}
		}
	}
	//Make sure we need naval workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER_SEA)
	{
		AICityStrategyTypes eNoNavalWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_NAVAL_TILE_IMPROVEMENT");
		if(eNoNavalWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNoNavalWorkers))
		{
			return 0;
		}
		else
		{
			iBonus += 100;
		}
	}
	//Make sure we need workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER)
	{
		if(kPlayer.getNumMilitaryUnits() <= 3)
		{
			iBonus -= 300;
		}
		AICityStrategyTypes eNoWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_TILE_IMPROVERS");
		if(eNoWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNoWorkers))
		{
			return 0;
		}
		AICityStrategyTypes eWantWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_WANT_TILE_IMPROVERS");
		if(eWantWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eWantWorkers))
		{
			iBonus += 100;
		}
		AICityStrategyTypes eNeedWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");
		if(eNeedWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedWorkers))
		{
			iBonus += 200;
		}
	}
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
				if(kPlayer.GetDiplomacyAI()->GetWarState(eLoopPlayer) < WAR_STATE_STALEMATE)
				{
					iBonus += 300;
				}
			}
		}
	}
	if(kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0)
	{
		if(kPlayer.GetMilitaryAI()->GetMostThreatenedCity(0) == m_pCity)
		{
			iBonus += 200;
		}
		if(pkUnitEntry->GetCombat() > 0)
		{
			iBonus += (kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) * 100);
		}
		else
		{
			iBonus -= (kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false) * 50);
		}
	}
	if(pkUnitEntry->GetCombat() > 0)
	{
		if(!kPlayer.isMinorCiv() && kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver() > 0)
		{
			iBonus += (kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver() * 25);
		}

		if(kPlayer.GetMilitaryAI()->GetWarType() == 1 && pkUnitEntry->GetDomainType() == DOMAIN_LAND)
		{
			iBonus += 35;
		}
		else if(kPlayer.GetMilitaryAI()->GetWarType() == 1 && pkUnitEntry->GetDomainType() == DOMAIN_SEA)
		{
			iBonus -= 25;
		}
		if(kPlayer.GetMilitaryAI()->GetWarType() == 2 && pkUnitEntry->GetDomainType() == DOMAIN_SEA)
		{
			iBonus += 35;
		}
		else if(kPlayer.GetMilitaryAI()->GetWarType() == 2 && pkUnitEntry->GetDomainType() == DOMAIN_LAND)
		{
			iBonus -= 25;
		}
	}
	
	/////////////////////
	// EXPERIENCE BOOSTERS
	/////////////////////
	if(pkUnitEntry->GetCombat() > 0)
	{
		//Let's try to build our units in our best cities only. More cities we have, the more this matters.
		if(m_pCity == kPlayer.GetBestMilitaryCity((UnitCombatTypes)pkUnitEntry->GetUnitCombatType()))
		{
			iBonus += (30 * kPlayer.getNumCities());
		}
		//Discourage bad cities.
		else
		{
			iBonus -= (15 * kPlayer.getNumCities());
		}
		//Let's try to build our units in our best cities only. More cities we have, the more this matters.
		if(m_pCity == kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, (DomainTypes)pkUnitEntry->GetDomainType()))
		{
			iBonus += (30 * kPlayer.getNumCities());
		}
		//Discourage bad cities.
		else
		{
			iBonus -= (15 * kPlayer.getNumCities());
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
				if((pkUnitEntry->GetUnitCombatType() != NO_UNITCOMBAT) && pkPromotionInfo->GetUnitCombatClass(pkUnitEntry->GetUnitCombatType()))
				{
					iBonus += 25;
				}
			}
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitClass(iI, pkUnitEntry->GetUnitClassType()))
			{
				if((pkUnitEntry->GetUnitCombatType() != NO_UNITCOMBAT) && pkPromotionInfo->GetUnitCombatClass(pkUnitEntry->GetUnitCombatType()))
				{
					iBonus += 50;
				}
			}
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(iI, pkUnitEntry->GetUnitCombatType()))
			{
				if((pkUnitEntry->GetUnitCombatType() != NO_UNITCOMBAT) && pkPromotionInfo->GetUnitCombatClass(pkUnitEntry->GetUnitCombatType()))
				{
					iBonus += 25;
				}
			}
		}
	}
	
	//Uniques? They're generally good enough to spam.
	if((UnitTypes)kPlayer.getCivilizationInfo().isCivilizationUnitOverridden(eUnit))
	{
		iBonus += 50;
	}

	//Have an army that needs this unit? Boost it quite a bit.
	if(pkUnitEntry->GetCombat() > 0)
	{
		if(pArmy != NULL && pArmy->GetGoalPlot() != NULL)
		{
			iBonus += 100;
		}
	}
	//For an operation? Build it!
	if(pkUnitEntry->GetCombat() > 0)
	{
		if(bForOperation)
		{
			iBonus += 100;
		}
		//////Let's get the land military unit AI type we have the least of and boost the lowest type.
		if(kPlayer.GetArmyDiversity() == (int)pkUnitEntry->GetDefaultUnitAIType())
		{
			iBonus += 33;
		}
	}
	//Tiny army? Eek!
	if(kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 2))
	{
		if(pkUnitEntry->GetCombat() > 0)
		{
			iBonus += 66;
		}
		//Fewer civilians til we rectify this!
		else
		{
			iBonus -= 66;
		}
	}
	//Debt is worth considering.
	if(pkUnitEntry->GetCombat() > 0 && !pkUnitEntry->IsNoMaintenance())
	{
		int iGPT = (int)kPlayer.GetTreasury()->AverageIncome(10);
		if(iGPT < 0)
		{
			//Every -1 GPT = -12% bonus
			iBonus += (iGPT * 12);
		}
	}
	
	iTempWeight *= (iBonus + 100);
	iTempWeight /= 100;

	/////
	///WEIGHT
	//////

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