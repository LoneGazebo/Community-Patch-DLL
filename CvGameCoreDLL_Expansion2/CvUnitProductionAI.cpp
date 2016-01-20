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

	if(!GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsNoAnnexing() && m_pCity->IsPuppet())
	{
		return 0;
	}
	
	if(GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
	{
		int iNumUnits = GET_PLAYER(m_pCity->getOwner()).getNumMilitaryUnits();
		if(iNumUnits >= (8 * GET_PLAYER(m_pCity->getOwner()).getNumCities()))
		{
			return 0;
		}
	}
	else
	{
		if(GET_PLAYER(m_pCity->getOwner()).GetNumUnitsOutOfSupply() > 0)
		{
			return 0;
		}
	}
	
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
			if(bForOperation && !GET_PLAYER(m_pCity->getOwner()).IsMusterCityAlreadyTargeted(m_pCity, DOMAIN_SEA))
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
	//Building land units on a tiny island? Only if we lack a garrison.
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

	/////////
	//RESOURCE CHECKS
	////////////
	//Check for special unlimited units from specific resources (not a strategic check)
	if(pkUnitEntry->GetResourceType() != NO_RESOURCE)
	{
		if(GET_PLAYER(m_pCity->getOwner()).getNumResourceAvailable((ResourceTypes)pkUnitEntry->GetResourceType(), false) > 0)
		{
			iTempWeight *= 3;
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
				ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
				if(eApolloProgram != NO_PROJECT)
				{
					if(GET_TEAM(m_pCity->getTeam()).getProjectCount(eApolloProgram) > 0)
					{
						ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
						if(eResourceLoop == eAluminumResource)
						{
							//We need at least 4 aluminum to get off the planet, so let's save that much if we've got the Apollo.
							if(GET_PLAYER(m_pCity->getOwner()).getNumResourceAvailable(eResourceLoop, false) <= 4)
							{
								if(pkUnitEntry->GetResourceQuantityRequirement(eResourceLoop) > 0)
								{
									return 0;
								}
							}
						}
					}
				}
			}
		}
	}
	///////////////
	//UNIT TYPE CHECKS
	//////////////////////

	//Carriers? Only if we need them.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_CARRIER_SEA)
	{
		MilitaryAIStrategyTypes eNeedCarriers = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_AIR_CARRIER");
		if(eNeedCarriers != NO_ECONOMICAISTRATEGY)
		{
			if(!GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eNeedCarriers))
			{
				return 0;
			}
			else
			{
				iTempWeight *= 3;
			}
		}
	}
	//Need Explorers?
	if (eDomain == DOMAIN_LAND && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE)
	{
		EconomicAIStrategyTypes eNeedRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON");	
		if(eNeedRecon != NO_ECONOMICAISTRATEGY)
		{
			if(GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eNeedRecon))
			{
				iTempWeight *= 10;
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
			if(GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eNeedRecon))
			{
				iTempWeight *= 10;
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
		if(eNeedBoatsCritical != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eNeedBoatsCritical))
		{
			iTempWeight *= 10;
		}
		MilitaryAIStrategyTypes eNeedBoats = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS");
		if(eNeedBoats != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eNeedBoats))
		{
			iTempWeight *= 2;
		}
	}
	//Land Units Critically NeedeD?
	if (eDomain == DOMAIN_LAND && (pkUnitEntry->GetCombat() > 0))
	{
		MilitaryAIStrategyTypes eNeedLandCritical = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");
		if(eNeedLandCritical != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eNeedLandCritical))
		{
			iTempWeight *= 10;
		}
		MilitaryAIStrategyTypes eNeedLand = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE");
		if(eNeedLand != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eNeedLand))
		{
			iTempWeight *= 2;
		}
		MilitaryAIStrategyTypes eBarb = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		if(eBarb != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eBarb))
		{
			iTempWeight *= 2;
		}
		MilitaryAIStrategyTypes eBarbCrit = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS_CRITICAL");
		if(eBarbCrit != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eBarbCrit))
		{
			iTempWeight *= 5;
		}
	}
	//Air Units Critically Needed?
	if (eDomain == DOMAIN_AIR)
	{
		MilitaryAIStrategyTypes eNeedAir = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_AIR");
		if(eNeedAir != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eNeedAir))
		{
			iTempWeight *= 10;
		}
	}
	//Air Defense Critically Needed?
	MilitaryAIStrategyTypes eNeedAirDefense = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_ANTIAIR");
	if(eNeedAirDefense != NO_MILITARYAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eNeedAirDefense))
	{
		//Promotion Bonus
		for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if(pkUnitEntry->GetFreePromotions(iI))
			{
				PromotionTypes ePromotion = (PromotionTypes) iI;

				if(GC.getPromotionInfo(ePromotion)->GetInterceptChanceChange() > 0)
				{
					iTempWeight *= 10;
				}
			}
		}
	}

	//Sanity check for city-attack only units.
	if(pkUnitEntry->IsCityAttackOnly())
	{
		if(pArmy != NULL && pArmy->GetGoalPlot() != NULL)
		{
			if(!pArmy->GetGoalPlot()->isCity() || ((pArmy->GetGoalPlot()->getOwner() != m_pCity->getOwner()) && (pArmy->GetGoalPlot()->getOwner() != NO_PLAYER)))
			{
				return 0;
			}
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if(MOD_DIPLOMACY_CITYSTATES && (pkUnitEntry->GetUnitAIType(UNITAI_MESSENGER) || pkUnitEntry->GetUnitAIType(UNITAI_DIPLOMAT)))
	{
		EconomicAIStrategyTypes eNeedDiplomats = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS");	
		if(eNeedDiplomats != NO_ECONOMICAISTRATEGY)
		{
			if(GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eNeedDiplomats))
			{
				iTempWeight *= 5;
			}
			else
			{
				iTempWeight /= 2;
			}
			EconomicAIStrategyTypes eNeedDiplomatsCrit = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");	
			if(eNeedDiplomatsCrit != NO_ECONOMICAISTRATEGY)
			{
				if(GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eNeedDiplomatsCrit))
				{
					iTempWeight *= 10;
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
		if(eStrategyConquest != NO_ECONOMICAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
		{
			iTempWeight *= 10;
		}
	}

	EconomicAIStrategyTypes eStrategyUNGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_DIPLOMACY");	
	if(eStrategyUNGS != NO_ECONOMICAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategyUNGS))
	{
		//Promotion Bonus
		for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if(pkUnitEntry->GetFreePromotions(iI))
			{
				PromotionTypes ePromotion = (PromotionTypes) iI;

				if(GC.getPromotionInfo(ePromotion)->GetTradeMissionInfluenceModifier() > 0)
				{
					iTempWeight *= GC.getPromotionInfo(ePromotion)->GetTradeMissionInfluenceModifier();
				}
			}
		}
	}

	EconomicAIStrategyTypes eStrategySS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP");	
	if(eStrategyUNGS != NO_ECONOMICAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategySS))
	{
		if(pkUnitEntry->GetSpaceshipProject() != NO_PROJECT)
		{
			iTempWeight *= 100;
		}
	}

	/////////
	//RELIGION CHECKS
	////////////
	if(pkUnitEntry->GetCombat() > 0)
	{
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
						if(pEntry->GetFaithFromKills() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK)))
						{
							iTempWeight *= 2;
						}
						if(pEntry->GetCombatModifierEnemyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
						{
							iTempWeight *= 2;
						}
						if(pEntry->GetCombatModifierFriendlyCities() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
						{
							iTempWeight *= 2;
						}
						if(pEntry->GetCombatVersusOtherReligionOwnLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_DEFENSE) || pkUnitEntry->GetUnitAIType(UNITAI_COUNTER)))
						{
							iTempWeight *= 2;
						}
						if(pEntry->GetCombatVersusOtherReligionTheirLands() > 0 && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_RANGED) || pkUnitEntry->GetUnitAIType(UNITAI_FAST_ATTACK) || pkUnitEntry->GetUnitAIType(UNITAI_CITY_BOMBARD)))
						{
							iTempWeight *= 2;
						}
						for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							const YieldTypes eYield = static_cast<YieldTypes>(iI);
							if(eYield != NO_YIELD)
							{
								if(pEntry->GetYieldFromKills(eYield) > 0)
								{
									iTempWeight *= 3;
									iTempWeight /= 2;
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

	//Settlers? Let's see...
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
	{
		if(GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
		{
			return 0;
		}
		EconomicAIStrategyTypes eStrategyEnoughSettlers = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
		bool bEnoughSettlers = false;
		if(eStrategyEnoughSettlers != NO_ECONOMICAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategyEnoughSettlers))
		{
			iTempWeight = 0;
		}
		else
		{
			AICityStrategyTypes eStrategyEnoughSettlers2 = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_SETTLERS");
			if(eStrategyEnoughSettlers2 != NO_AICITYSTRATEGY)
			{
				bEnoughSettlers = m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyEnoughSettlers2);
			}
		}
		bool bForceSettler = false;
		
		EconomicAIStrategyTypes eStrategyExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_REALLY_EXPAND_TO_OTHER_CONTINENTS");
		EconomicAIStrategyTypes eExpandLikeCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY");
		AICityStrategyTypes eFeederCity = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEW_CONTINENT_FEEDER");

		if (eStrategyExpandToOtherContinents != NO_ECONOMICAISTRATEGY)
		{
			if (GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategyExpandToOtherContinents))
			{
				bEnoughSettlers = false;
				bForceSettler = true;
			}
		}
		else if (eExpandLikeCrazy != NO_ECONOMICAISTRATEGY)
		{
			if (GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eExpandLikeCrazy))
			{
				bEnoughSettlers = false;
				bForceSettler = true;
			}
		}
		if(eFeederCity != NO_AICITYSTRATEGY)
		{
			if(m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eFeederCity))
			{
				bEnoughSettlers = false;
				bForceSettler = true;
			}
		}
		int iBestArea;
		int iSecondBestArea;
		int iNumAreas;
		iNumAreas = GET_PLAYER(m_pCity->getOwner()).GetBestSettleAreas(GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
		if(iNumAreas == 0)
		{
			bEnoughSettlers = true;
			bForceSettler = false;
		}
		if(GET_PLAYER(m_pCity->getOwner()).getNumCities() <= 1)
		{
			bEnoughSettlers = false;
			bForceSettler = true;
		}

		if(GET_PLAYER(m_pCity->getOwner()).isBarbarian() || GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsNoAnnexing() || GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
		{
			bEnoughSettlers = true;
			bForceSettler = false;
		}
		if(bEnoughSettlers)
		{
			return 0;
		}
		if(bForceSettler)
		{
			iTempWeight *= 100;
		}
	}

	//Archaeologists? Only if we have digs nearby.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_ARCHAEOLOGIST)
	{
		if(GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
		{
			return 0;
		}
		EconomicAIStrategyTypes eWantArch = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_ARCHAEOLOGISTS");
		if(eWantArch != NO_ECONOMICAISTRATEGY)
		{
			if(!GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eWantArch))
			{
				return 0;
			}
			else
			{
				iTempWeight *= 5;
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
	}
	//Make sure we need workers in this city.
	if(pkUnitEntry->GetDefaultUnitAIType() == UNITAI_WORKER)
	{
		AICityStrategyTypes eNoWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_ENOUGH_TILE_IMPROVERS");
		if(eNoWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNoWorkers))
		{
			return 0;
		}
		AICityStrategyTypes eWantWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_WANT_TILE_IMPROVERS");
		if(eWantWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eWantWorkers))
		{
			iTempWeight *= 2;
		}
		AICityStrategyTypes eNeedWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");
		if(eNeedWorkers != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedWorkers))
		{
			iTempWeight *= 2;
		}
	}
	//////////////////
	//WAR BOOSTERS
	////////////////////////
	int iSneakies = 0;
	if(!GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
	{
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				if(GET_PLAYER(m_pCity->getOwner()).GetDiplomacyAI()->IsWantsSneakAttack(eLoopPlayer))
				{
					iSneakies++;
				}
			}
		}
		if(GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0)
		{
			iSneakies += (GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetNumberCivsAtWarWith(false) * 5);
		}
	}
	iTempWeight *= max(1, iSneakies);

	if(pkUnitEntry->GetCombat() > 0)
	{
		if(!GET_PLAYER(m_pCity->getOwner()).isMinorCiv() && GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver() > 0)
		{
			iTempWeight *= GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver();
		}

		if(GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetWarType() == 1 && pkUnitEntry->GetDomainType() == DOMAIN_LAND)
		{
			iTempWeight *= 3;
		}
		if(GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->GetWarType() == 2 && pkUnitEntry->GetDomainType() == DOMAIN_SEA)
		{
			iTempWeight *= 3;
		}
	}
	
	//XP and Production Mods
	if(!m_pCity->IsPuppet() || GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsNoAnnexing())
	{
		for (int iDomainLoop = 0; iDomainLoop < NUM_DOMAIN_TYPES; iDomainLoop++)
		{
			DomainTypes eTestDomain = (DomainTypes)iDomainLoop;
			if(eTestDomain != NO_DOMAIN)
			{
				if(pkUnitEntry->GetDomainType() == eTestDomain)
				{
					if(m_pCity->getDomainFreeExperience(eTestDomain) > 0)
					{
						iTempWeight *= max(1, (m_pCity->getDomainFreeExperience(eTestDomain) / 10));
					}
					if(m_pCity->getDomainFreeExperienceFromGreatWorks(eTestDomain) > 0)
					{
						iTempWeight *= max(1, (m_pCity->getDomainFreeExperienceFromGreatWorks(eTestDomain) / 10));
					}
					if(m_pCity->getDomainProductionModifier(eTestDomain) > 0)
					{
						iTempWeight *= max(1, (m_pCity->getDomainProductionModifier(eTestDomain) / 10));
					}
				}
			}
		}
	}
	
	//Promotion Bonus
	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			if(m_pCity->isFreePromotion(ePromotion))
			{
				if((pkUnitEntry->GetUnitCombatType() != NO_UNITCOMBAT) && pkPromotionInfo->GetUnitCombatClass(pkUnitEntry->GetUnitCombatType()))
				{
					iTempWeight *= 10;
				}
			}
		}
	}
	
	//Uniques? They're generally good enough to spam.
	if((UnitTypes)GET_PLAYER(m_pCity->getOwner()).getCivilizationInfo().isCivilizationUnitOverridden(eUnit))
	{
		iTempWeight *= 5;
	}

	//Debt is worth considering.
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	if(eStrategyLosingMoney != NO_ECONOMICAISTRATEGY && GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
	{
		iTempWeight /= 25;
	}

	/////
	///WEIGHT
	//////
	iTempWeight /= max(1, (m_pCity->getProductionTurnsLeft(eUnit, 0) / 3));

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