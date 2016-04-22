﻿/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvSiteEvaluationClasses.h"
#include "CvImprovementClasses.h"
#include "CvCitySpecializationAI.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"
#if defined(MOD_BALANCE_CORE)
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#endif

// include this after all other headers!
#include "LintFree.h"

//=====================================
// CvCitySiteEvaluator
//=====================================
/// Constructor
CvCitySiteEvaluator::CvCitySiteEvaluator(void)
{
	m_iExpansionIndex = 12;
	m_iGrowthIndex = 13;
}

/// Destructor
CvCitySiteEvaluator::~CvCitySiteEvaluator(void)
{
}

/// Initialize
void CvCitySiteEvaluator::Init()
{
	// Set up city ring multipliers
	m_iRingModifier[0] = 1;   // Items under city get handled separately
	m_iRingModifier[1] = /*6*/ GC.getCITY_RING_1_MULTIPLIER();
	m_iRingModifier[2] = /*3*/ GC.getCITY_RING_2_MULTIPLIER();
	m_iRingModifier[3] = /*2*/ GC.getCITY_RING_3_MULTIPLIER();
	m_iRingModifier[4] = /*1*/ GC.getCITY_RING_4_MULTIPLIER();
	m_iRingModifier[5] = /*1*/ GC.getCITY_RING_5_MULTIPLIER();
	m_iRingModifier[6] = 0;
	m_iRingModifier[7] = 0;

	m_iGrowthIndex = GC.getInfoTypeForString("FLAVOR_GROWTH");
	m_iExpansionIndex = GC.getInfoTypeForString("FLAVOR_EXPANSION");
	m_iNavalIndex = GC.getInfoTypeForString("FLAVOR_NAVAL");

	m_iBrazilMultiplier = 1000;	//fertility boost from jungles
	m_iSpainMultiplier = 55000;	//fertility boost from natural wonders
	m_iMorrocoMultiplier = 1000; //fertility boost from desert
#if defined(MOD_BALANCE_CORE)
	m_iFranceMultiplier = 1000; //fertility boost from resources
#endif
	m_iNetherlandsMultiplier = 2000; //fertility boost from marshes and flood plains
	m_iIncaMultiplier = 500; //fertility boost for hill tiles surrounded my mountains

#if defined(MOD_BALANCE_CORE_SETTLER)
	for (int i=0; i<NUM_SITE_EVALUATION_FACTORS; i++)
		m_iFlavorMultiplier[i]=1;
#endif

}

/// Is it valid for this player to found a city here?
#if defined(MOD_BALANCE_CORE)
bool CvCitySiteEvaluator::CanFound(const CvPlot* pPlot, const CvPlayer* pPlayer, bool bIgnoreDistanceToExistingCities, const CvUnit* pUnit) const
#else
bool CvCitySiteEvaluator::CanFound(CvPlot* pPlot, const CvPlayer* pPlayer, bool bIgnoreDistanceToExistingCities) const
#endif
{
	CvAssert(pPlot);
	if(!pPlot)
		return false;

	CvPlot* pLoopPlot(NULL);
	bool bValid(false);

	// Used to have a Python hook: CANNOT_FOUND_CITY_CALLBACK

	if(GC.getGame().isFinalInitialized())
	{
		if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer && pPlayer->isHuman())
		{
			if(pPlayer->getNumCities() > 0)
			{
				return false;
			}
		}
	}

	if (pUnit)
	{
		if(!pUnit->canEnterTerrain(*pPlot))
		{
			return false;
		}
	}
	else if(!pPlot->isValidMovePlot(pPlayer ? pPlayer->GetID() : NO_PLAYER ))
	{
		return false;
	}

	if(pPlot->IsNaturalWonder())
		return false;

	//need at least one conventionally accessible plot around the city
	bool bAccessible = false;
	for (int i = 0; i < RING1_PLOTS; i++)
	{
		CvPlot* pNeighbor = iterateRingPlots(pPlot, i);
		if (pNeighbor && !pNeighbor->isImpassable(BARBARIAN_TEAM))
			bAccessible = true;
	}
	if (!bAccessible)
		return false;

	if(pPlot->getFeatureType() != NO_FEATURE)
	{
		if(GC.getFeatureInfo(pPlot->getFeatureType())->isNoCity())
		{
			return false;
		}
	}

	if(pPlayer)
	{
		if(pPlot->isOwned() && (pPlot->getOwner() != pPlayer->GetID()))
		{
			return false;
		}
	}

	CvTerrainInfo* pTerrainInfo = GC.getTerrainInfo(pPlot->getTerrainType());

	if(!bValid)
	{
		if(pTerrainInfo->isFound())
		{
			bValid = true;
		}
	}

	if(!bValid)
	{
		if(pTerrainInfo->isFoundCoast())
		{
			if(pPlot->isCoastalLand())
			{
				bValid = true;
			}
		}
	}

	if(!bValid)
	{
		if(pTerrainInfo->isFoundFreshWater())
		{
			if(pPlot->isFreshWater_cached())
			{
				bValid = true;
			}
		}
	}

	// Used to have a Python hook: CAN_FOUND_CITIES_ON_WATER_CALLBACK

	if(pPlot->isWater())
	{
		return false;
	}

	if(!bValid)
	{
		return false;
	}

#if defined(MOD_BALANCE_CORE)
	if(pPlayer && pPlot->IsAdjacentOwnedByOtherTeam(pPlayer->getTeam()))
	{
		return false;
	}

	if(!bIgnoreDistanceToExistingCities)
	{
		int iMinDist(0), iDX(0), iDY(0);
		// look at same land mass

		iMinDist = /*3*/ GC.getMIN_CITY_RANGE();
		if(pPlayer && pPlayer->isMinorCiv())
		{
			if(GC.getMap().getWorldInfo().getMinDistanceCityStates() > 0)
			{
				iMinDist = GC.getMap().getWorldInfo().getMinDistanceCityStates();
			}
		}
		else if(GC.getMap().getWorldInfo().getMinDistanceCities() > 0)
		{
			iMinDist = GC.getMap().getWorldInfo().getMinDistanceCities();
		}

		for(iDX = -(iMinDist); iDX <= iMinDist; iDX++)
		{
			for(iDY = -(iMinDist); iDY <= iMinDist; iDY++)
			{
				pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iMinDist);

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->isCity())
					{
						if(pLoopPlot->getLandmass() == pPlot->getLandmass())
						{
							return false; //we know that distance <= iMinDist here
						}
						else if(hexDistance(iDX, iDY) < iMinDist)  // one less for off shore
						{
							return false;
						}
					}
				}
			}
		}
	}
#endif

	return true;
}

/// Setup flavor multipliers - call once per player before PlotFoundValue() or PlotFertilityValue()
void CvCitySiteEvaluator::ComputeFlavorMultipliers(CvPlayer* pPlayer)
{
	// Set all to 0
	for(int iI = 0; iI < NUM_SITE_EVALUATION_FACTORS; iI++)
	{
		m_iFlavorMultiplier[iI] = 0;
	}

	// Find out if player has a desired next city specialization
	CvCitySpecializationXMLEntry* pkCitySpecializationEntry = NULL;

	//disable this, it leads to strange results
	/*
	CitySpecializationTypes eNextSpecialization = pPlayer->GetCitySpecializationAI()->GetNextSpecializationDesired();
	if(eNextSpecialization != NO_CITY_SPECIALIZATION)
		pkCitySpecializationEntry = GC.getCitySpecializationInfo(eNextSpecialization);
	*/

	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		const FlavorTypes eFlavor = static_cast<FlavorTypes>(iFlavorLoop);
		const CvString& strFlavor = GC.getFlavorTypes(eFlavor);
		if(strFlavor == "FLAVOR_GROWTH" ||
		        strFlavor == "FLAVOR_EXPANSION")
		{
			m_iFlavorMultiplier[YIELD_FOOD] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_FOOD] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if(strFlavor == "FLAVOR_GOLD" ||
		        strFlavor == "FLAVOR_TILE_IMPROVEMENT")
		{
			m_iFlavorMultiplier[YIELD_GOLD] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_GOLD] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if(strFlavor == "FLAVOR_PRODUCTION" ||
		        strFlavor == "FLAVOR_WONDER")
		{
			m_iFlavorMultiplier[YIELD_PRODUCTION] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_PRODUCTION] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if(strFlavor == "FLAVOR_SCIENCE")
		{
			// Doubled since only one flavor related to science
			m_iFlavorMultiplier[YIELD_SCIENCE] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_SCIENCE] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
		else if(strFlavor == "FLAVOR_HAPPINESS")
		{
			// Doubled since only one flavor related to Happiness
			m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if(pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
		else if(strFlavor == "FLAVOR_RELIGION")
		{
			// Doubled since only one flavor related to faith
			m_iFlavorMultiplier[YIELD_FAITH] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if (pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_FAITH] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
	}

	// Make sure none are negative
	for(int iI = 0; iI < NUM_SITE_EVALUATION_FACTORS; iI++)
	{
		if(m_iFlavorMultiplier[iI] < 0)
		{
			m_iFlavorMultiplier[iI] = 0;
		}
	}

	// Set tradable resources and strategic value to times 10 (so multiplying this by the number of map gives a number from 1 to 100)
	m_iFlavorMultiplier[SITE_EVALUATION_RESOURCES] = 10;
	m_iFlavorMultiplier[SITE_EVALUATION_STRATEGIC] = 10;

	// Important: To make sure the resulting scores are comparable, we need to L1 normalize this vector (to 100)
	float fSum = 0;
	for (int i=0; i<NUM_SITE_EVALUATION_FACTORS; i++)
		fSum += m_iFlavorMultiplier[i];
	for (int i=0; i<NUM_SITE_EVALUATION_FACTORS; i++)
		m_iFlavorMultiplier[i] = int(0.5f + m_iFlavorMultiplier[i]/fSum * 100);
}

/// Retrieve the relative value of this plot (including plots that would be in city radius)
int CvCitySiteEvaluator::PlotFoundValue(CvPlot* pPlot, const CvPlayer* pPlayer, YieldTypes eYield, bool /*bCoastOnly*/, CvString* pDebug)
{
	CvAssert(pPlot);
	if(!pPlot)
		return -1;

	// Make sure this player can even build a city here
	if(!CanFound(pPlot, pPlayer, false))
	{
		if(pDebug)
			*pDebug = "cannot found";
		return -1;
	}

	//for debugging
	std::vector<std::string> vQualifiersPositive;
	std::vector<std::string> vQualifiersNegative;

	//total
	int iTotalPlotValue = 0;
	int iValueModifier = 0; //general modifiers
	int iCivModifier = 0; //civ-specific modifiers
	int iStratModifier = 0; //strategic modifiers

	int iCelticForestCount = 0;
	int iIroquoisForestCount = 0;
	int iBrazilJungleCount = 0;
	int iNaturalWonderCount = 0;
	int iDesertCount = 0;
	int iWetlandsCount = 0;

	int iLakeCount = 0;
	int iLuxuryCount = 0;

	//currently just for debugging
	int iTotalFoodValue = 0;
	int iTotalHappinessValue = 0;
	int iTotalProductionValue = 0;
	int iTotalGoldValue = 0;
	int iTotalScienceValue = 0;
	int iTotalFaithValue = 0;
	int iTotalResourceValue = 0;
	int iTotalStrategicValue = 0;

	//use a slightly negative base value to discourage settling in bad lands
	int iDefaultPlotValue = -100;

	int iBorderlandRange = 6;
	int iCapitalArea = NULL;

	bool bIsAlmostCoast = false;
	bool bIsInca = false;
	int iAdjacentMountains = 0;

	std::vector<SPlotWithScore> workablePlots;
	workablePlots.reserve(49);

	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;
	PlayerTypes ePlayer = pPlayer ? pPlayer->GetID() : NO_PLAYER;

	if (pPlayer)
	{
		if ( pPlayer->getCapitalCity() )
		{
			iCapitalArea = pPlayer->getCapitalCity()->getArea();
		}

		// Custom code for Inca ideal terrace farm locations
		ImprovementTypes eIncaImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_TERRACE_FARM", true);  
		if(eIncaImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eIncaImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					bIsInca = true;
				}
			}
		}
	}


	int iRange = pPlayer ? pPlayer->getWorkPlotDistance() : 3;
	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				if (iDistance <= iRange)
				{
					int iRingModifier = m_iRingModifier[iDistance];

					//not only our cities, also other player's cities!
					int iExistingCityDistance = GC.getGame().GetClosestCityDistance(pLoopPlot);

					//count the tile only if the city will be able to work it
					if ( !pLoopPlot->isValidMovePlot(pPlayer->GetID()) || pLoopPlot->getWorkingCity()!=NULL || iExistingCityDistance<=2 ) 
						iRingModifier = 0;

					if (iExistingCityDistance==3)
						//this plot will likely be contested between the two cities, reduce its value
						iRingModifier /= 2;

					int iPlotValue = iDefaultPlotValue;
					if (iRingModifier>0)
					{
						int iFoodValue = 0;
						int iHappinessValue = 0;
						int iProductionValue = 0;
						int iGoldValue = 0;
						int iScienceValue = 0;
						int iFaithValue = 0;
						int iResourceValue = 0;
						int iStrategicValue = 0;

						if (eYield == NO_YIELD || eYield == YIELD_FOOD)
							iFoodValue = ComputeFoodValue(pLoopPlot, pPlayer) * /*15*/ GC.getSETTLER_FOOD_MULTIPLIER();

						if (eYield == NO_YIELD || eYield == YIELD_PRODUCTION)
							iProductionValue = ComputeProductionValue(pLoopPlot, pPlayer) * /*3*/ GC.getSETTLER_PRODUCTION_MULTIPLIER();

						if (eYield == NO_YIELD || eYield == YIELD_GOLD)
							iGoldValue = ComputeGoldValue(pLoopPlot, pPlayer) * /*2*/ GC.getSETTLER_GOLD_MULTIPLIER();

						if (eYield == NO_YIELD || eYield == YIELD_SCIENCE)
							iScienceValue = ComputeScienceValue(pLoopPlot, pPlayer) * /*1*/ GC.getSETTLER_SCIENCE_MULTIPLIER();

						if (eYield == NO_YIELD || eYield == YIELD_FAITH)
							iFaithValue = ComputeFaithValue(pLoopPlot, pPlayer) * /*1*/ GC.getSETTLER_FAITH_MULTIPLIER();

						if (pLoopPlot->getOwner() == NO_PLAYER) // there is no benefit if we already own these tiles
						{
							iHappinessValue = ComputeHappinessValue(pLoopPlot, pPlayer) * /*6*/ GC.getSETTLER_HAPPINESS_MULTIPLIER();
							iResourceValue = ComputeTradeableResourceValue(pLoopPlot, pPlayer) * /*1*/ GC.getSETTLER_RESOURCE_MULTIPLIER();
							if (iDistance)
								iStrategicValue = ComputeStrategicValue(pLoopPlot, pPlayer, iDistance) * /*1*/ GC.getSETTLER_STRATEGIC_MULTIPLIER();  // the ring is included in the computation
						}

						iTotalFoodValue += iFoodValue;
						iTotalHappinessValue += iHappinessValue;
						iTotalProductionValue += iProductionValue;
						iTotalGoldValue += iGoldValue;
						iTotalScienceValue += iScienceValue;
						iTotalFaithValue += iFaithValue;
						iTotalResourceValue += iResourceValue;
						iTotalStrategicValue += iStrategicValue;

						iPlotValue += iRingModifier * ( iFoodValue + iHappinessValue + iProductionValue + iGoldValue + iScienceValue + iFaithValue + iResourceValue ) + iStrategicValue;
					}

					// for the central plot
					if (iDistance==0)
						vQualifiersPositive.push_back( CvString::format("raw plot value %d", iPlotValue).c_str() );

					if (iDistance==1 && !pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()) && pLoopPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
						bIsAlmostCoast = true;

					// if this tile is a NW boost the value just so that we force the AI to claim them (if we can work it)
					if (pLoopPlot->IsNaturalWonder() && iPlotValue>0)
						iPlotValue *= 15;

					// lower value a lot if we already own this tile
					if (iPlotValue > 0 && pLoopPlot->getOwner() == ePlayer && ePlayer != NO_PLAYER)
						iPlotValue /= 2;

					// add this plot into the total
					workablePlots.push_back( SPlotWithScore(pLoopPlot,iPlotValue) );

					// some civ-specific checks
					FeatureTypes ePlotFeature = pLoopPlot->getFeatureType();
					ImprovementTypes ePlotImprovement = pLoopPlot->getImprovementType();
					ResourceTypes ePlotResource = pLoopPlot->getResourceType();

					if (ePlotFeature == FEATURE_FOREST)
					{
						if (iDistance <= 5)
						{
							++iIroquoisForestCount;
							if (iDistance == 1)
								if (ePlotImprovement == NO_IMPROVEMENT)
									++iCelticForestCount;
						}
					}
					else if (ePlotFeature == FEATURE_JUNGLE)
					{
						if (iDistance <= iRange)
							++iBrazilJungleCount;
					}
					else if (ePlotFeature == FEATURE_MARSH || ePlotFeature == FEATURE_FLOOD_PLAINS)
					{
						if (iDistance <= iRange)
							++iWetlandsCount;
					}

					if (pLoopPlot->IsNaturalWonder())
					{
						if (iDistance <= iRange)
							++iNaturalWonderCount;
					}

					if (pLoopPlot->isLake())
					{
						if (iDistance <= iRange)
							++iLakeCount;
					}
					if (pLoopPlot->getResourceType(NO_TEAM) != NO_RESOURCE)
					{
						ResourceTypes eResource = pLoopPlot->getResourceType(eTeam);
						if(eResource != NO_RESOURCE && GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
						{
							if (iDistance <= iRange)
							{
								++iLuxuryCount;
							}
						}
					}

					if (pLoopPlot->getTerrainType() == TERRAIN_DESERT)
					{
						if (iDistance <= iRange)
						{
							if (ePlotResource == NO_RESOURCE)
							{
								++iDesertCount;
							}
						}
					}

					if (bIsInca)
					{
						if (pLoopPlot->isHills() && iDistance <= iRange)
						{
							iAdjacentMountains = pLoopPlot->GetNumAdjacentMountains();
							if (iAdjacentMountains > 0 && iAdjacentMountains < 6)
							{
								//give the bonus if it's hills, with additional if bordered by mountains
								iCivModifier += (iAdjacentMountains+1) * m_iIncaMultiplier;
								vQualifiersPositive.push_back("(C) incan hills");
							}
						}
					}
				}
			}
		}
	}	

	//take into account only the best 70% of the plots - in the near term the city will not work all plots anyways
	std::stable_sort( workablePlots.begin(), workablePlots.end() );
	size_t iIrrelevantPlots = workablePlots.size()*30/100;
	for (size_t idx=iIrrelevantPlots; idx<workablePlots.size(); idx++)
	{
		SPlotWithScore& ref = workablePlots[idx];
		iTotalPlotValue += ref.score;
	}

	if (iTotalPlotValue<0)
		return 0;
	
	//civ-specific bonuses
	if (pPlayer)
	{
		if (pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest())
		{
			if (iCelticForestCount >= 3)
			{
				iCivModifier += 2 * 1000 * m_iFlavorMultiplier[YIELD_FAITH];
				vQualifiersPositive.push_back("(C) much forest");
			}
			else if (iCelticForestCount >= 1)
			{
				iCivModifier += 1 * 1000 * m_iFlavorMultiplier[YIELD_FAITH];
				vQualifiersPositive.push_back("(C) some forest");
			}
		}
		else if (pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus())
		{
			iCivModifier += iIroquoisForestCount * 10;	
			vQualifiersPositive.push_back("(C) forested");
		}
		else if (pPlayer->GetPlayerTraits()->GetNaturalWonderYieldModifier() > 0)	//ie: Spain
		{
			iCivModifier += iNaturalWonderCount * m_iSpainMultiplier;	
			vQualifiersPositive.push_back("(C) natural wonders");
		}

		// Custom code for Brazil
		ImprovementTypes eBrazilImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_BRAZILWOOD_CAMP", true);  
		if(eBrazilImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eBrazilImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					iCivModifier += iBrazilJungleCount * m_iBrazilMultiplier;
					vQualifiersPositive.push_back("(C) jungle");
				}
			}
		}

		// Custom code for Morocco
		ImprovementTypes eMoroccoImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_KASBAH", true);  
		if(eMoroccoImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eMoroccoImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired() && !pkEntry->IsAdjacentCity())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					iCivModifier += iDesertCount * m_iMorrocoMultiplier;
					vQualifiersPositive.push_back("(C) desert");
				}
			}
		}

		// Custom code for France
		ImprovementTypes eFranceImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CHATEAU", true);  
		if(eFranceImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eFranceImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired() && pkEntry->IsAdjacentLuxury())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					iCivModifier += iLuxuryCount * m_iFranceMultiplier;
					vQualifiersPositive.push_back("(C) luxury");
				}
			}
		}

		//Custom code for Netherlands
		ImprovementTypes ePolderImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_POLDER", true);  
		if(ePolderImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(ePolderImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					iCivModifier += iWetlandsCount * m_iNetherlandsMultiplier;
					if(pkEntry->IsAdjacentLake())
					{
						iCivModifier += (iLakeCount * m_iNetherlandsMultiplier);
					}
					vQualifiersPositive.push_back("(C) wetlands");
				}
			}
		}
	}

	// Finally, look at the city plot itself
	if (pPlot->getResourceType(eTeam) != NO_RESOURCE)
	{
		iValueModifier += (int)iTotalPlotValue * /*-50*/ GC.getBUILD_ON_RESOURCE_PERCENT() / 100;
		vQualifiersNegative.push_back("(V) city on resource");
	}

	if (pPlot->IsNaturalWonder())
	{
		iValueModifier += (int)iTotalPlotValue * /*-50*/ GC.getBUILD_ON_RESOURCE_PERCENT() / 100;
		vQualifiersNegative.push_back("(V) city on natural wonder");
	}

	if ( iTotalFoodValue>5*iTotalProductionValue || iTotalProductionValue > 2*iTotalFoodValue )
	{
		iValueModifier -= (int)iTotalPlotValue * 10 / 100;
		vQualifiersNegative.push_back("(V) unbalanced yields");
	}

	if (pPlot->isRiver())
	{
		iValueModifier += (int)iTotalPlotValue * /*15*/ GC.getBUILD_ON_RIVER_PERCENT() / 100;
		if(pPlayer && pPlayer->GetPlayerTraits()->IsRiverTradeRoad())
			iValueModifier += (int)iTotalPlotValue * /*15*/ GC.getBUILD_ON_RIVER_PERCENT() / 100 * 2;
		vQualifiersPositive.push_back("(V) river");
	}

	if (bIsAlmostCoast)
	{
		iValueModifier -= (iTotalPlotValue * 25) / 100;
		vQualifiersNegative.push_back("(V) almost coast");
	}

	CvArea* pArea = pPlot->area();
	int iGoodTiles = 1;
	if(pArea != NULL)
	{
		iGoodTiles = max(1,(pArea->getNumUnownedTiles() - pArea->GetNumBadPlots()));
	}

	//Island maps need a little more loose restriction here.
	if(GC.getMap().GetAIMapHint() & ciMapHint_NavalOffshore)
	{
		if (pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			if(pArea != NULL && iGoodTiles > 0)
			{
				iValueModifier += (iTotalPlotValue * /*40*/ GC.getSETTLER_BUILD_ON_COAST_PERCENT()) / 100;
				vQualifiersPositive.push_back("(V) coast");

				if (pPlayer)
				{
					int iNavalFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iNavalIndex);
					if (iNavalFlavor > 7)
					{
						iValueModifier += (iTotalPlotValue * /*40*/ GC.getSETTLER_BUILD_ON_COAST_PERCENT()) / 100;
					}
					if (pPlayer->getCivilizationInfo().isCoastalCiv()) // we really like the coast (England, Norway, Polynesia, Carthage, etc.)
					{
						iValueModifier += iTotalPlotValue;
					}
				}
			}
		}
		else
		{
			if(iGoodTiles <= 3)
			{
				iValueModifier -= (iTotalPlotValue * 40) / 100;
				vQualifiersNegative.push_back("(V) not enough good plots");
			}
			else if(iGoodTiles <= 6)
			{
				iValueModifier -= (iTotalPlotValue * 20) / 100;
				vQualifiersNegative.push_back("(V) few good plots");
			}
		}
	}
	else
	{
		if (pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			if(pArea != NULL && iGoodTiles > 3)
			{
				iValueModifier += (iTotalPlotValue * /*40*/ GC.getSETTLER_BUILD_ON_COAST_PERCENT()) / 100;
				vQualifiersPositive.push_back("(V) coast");

				if (pPlayer)
				{
					int iNavalFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iNavalIndex);
					if (iNavalFlavor > 7)
					{
						iValueModifier += (iTotalPlotValue * /*40*/ GC.getSETTLER_BUILD_ON_COAST_PERCENT()) / 100;
					}
					if (pPlayer->getCivilizationInfo().isCoastalCiv()) // we really like the coast (England, Norway, Polynesia, Carthage, etc.)
					{
						iValueModifier += iTotalPlotValue;
					}
				}
			}
			else if(pArea != NULL && iGoodTiles == 1)
			{
				iValueModifier -= (iTotalPlotValue * 40) / 100;
				vQualifiersNegative.push_back("(V) coast on 1-tile island is not great");
			}
			else
			{
				iValueModifier -= (iTotalPlotValue * 25) / 100;
				vQualifiersNegative.push_back("(V) coast on small island");
			}
		}
		else
		{
			if(iGoodTiles <= 5)
			{
				iValueModifier -= (iTotalPlotValue * 40) / 100;
				vQualifiersNegative.push_back("(V) not enough good plots");
			}
			else if(iGoodTiles <= 10)
			{
				iValueModifier -= (iTotalPlotValue * 20) / 100;
				vQualifiersNegative.push_back("(V) few good plots");
			}
		}
	}

	//Is this a chokepoint?
	if(pPlot->IsChokePoint())
	{
		iStratModifier += (iTotalPlotValue * /*100*/ GC.getBALANCE_CHOKEPOINT_STRATEGIC_VALUE()) / 100;
		vQualifiersPositive.push_back("(S) chokepoint");

		//each landbride is a chokepoint, but not every chokepoint is a landbridge
		if(pPlot->IsLandbridge(12,54))
		{
			iStratModifier += (iTotalPlotValue * /*100*/ GC.getBALANCE_CHOKEPOINT_STRATEGIC_VALUE()) / 100;
			vQualifiersPositive.push_back("(S) landbridge");
		}
	}

	//Check for strategic landgrab
	int iOwnCityDistance = pPlayer ? pPlayer->GetCityDistance(pPlot) : INT_MAX;
	int iOtherCityDistance = INT_MAX;

	//check if the closest city is our or somebody else's
	CvCity* pClosestCity = GC.getGame().GetClosestCity(pPlot);
	if( pClosestCity && (!pPlayer || pClosestCity->getOwner()!=pPlayer->GetID()) )
	{
		iOtherCityDistance = GC.getGame().GetClosestCityDistance(pPlot);

		PlayerTypes eOtherPlayer = (PlayerTypes) pClosestCity->getOwner();
		PlayerProximityTypes eProximity = GET_PLAYER(eOtherPlayer).GetProximityToPlayer(pPlayer->GetID());
		if(eProximity >= PLAYER_PROXIMITY_CLOSE && iOtherCityDistance<=iBorderlandRange)
		{
			//Neighbor must not be too strong
			if ( pPlayer->GetMilitaryMight() > GET_PLAYER(eOtherPlayer).GetMilitaryMight()*1.4f )
			{
				iStratModifier += (iTotalPlotValue * /*50*/ GC.getBALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE()) / 100;
				vQualifiersPositive.push_back("(S) landgrab");
			}
			else if ( pPlayer->GetMilitaryMight() < GET_PLAYER(eOtherPlayer).GetMilitaryMight()*0.8f )
			{
				iStratModifier -= (iTotalPlotValue * /*50*/ GC.getBALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE()) / 100;
				vQualifiersNegative.push_back("(S) too dangerous");
			}
		}
	}

	// where is our personal sweet spot?
	int iMinDistance = /*3*/ GC.getMIN_CITY_RANGE();
	if(pPlayer && pPlayer->isMinorCiv())
	{
		if(GC.getMap().getWorldInfo().getMinDistanceCityStates() > 0)
		{
			iMinDistance = GC.getMap().getWorldInfo().getMinDistanceCityStates();
		}
	}
	else if(GC.getMap().getWorldInfo().getMinDistanceCities() > 0)
	{
		iMinDistance = GC.getMap().getWorldInfo().getMinDistanceCities();
	}

	if (iOwnCityDistance <= iMinDistance)
	{
		//this case should be handled by the distance check in CanFound() also
		iValueModifier -= iTotalPlotValue / 2;
		vQualifiersNegative.push_back("(V) too close to existing friendly city");
	}

	// AI only
	if (pPlayer && !pPlayer->isHuman())
	{
		int iSweetMin = GC.getSETTLER_DISTANCE_DROPOFF_MODIFIER();
		int iSweetMax = GC.getSETTLER_DISTANCE_DROPOFF_MODIFIER()+1;

		//check our preferred balance between tall and wide
		int iGrowthFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iGrowthIndex);
		if (iGrowthFlavor > 5)
		{
			iSweetMax++;
			iSweetMin++;
		}
		else if (iGrowthFlavor < 5)
		{
			iSweetMax--;
			iSweetMin--;
		}

		int iExpansionFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iExpansionIndex);
		if (iExpansionFlavor > 5)
		{
			iSweetMax++;
			iSweetMin++;
		}
		else if (iExpansionFlavor < 5)
		{
			iSweetMax--;
			iSweetMin--;
		}

		if(iSweetMin < iMinDistance)
			iSweetMin = iMinDistance;

		//this affects both friendly and other cities
		if (min(iOwnCityDistance,iOtherCityDistance) >= iSweetMin && max(iOwnCityDistance,iOtherCityDistance) <= iSweetMax) 
		{
			iValueModifier += (iTotalPlotValue*20)/100; //make this a small bonus, there is a separate distance check anyway
			vQualifiersPositive.push_back("(V) optimal distance to existing cities");
		}

		//boldness comes into play when there are enemy cities around
		int iBoldness = pPlayer->GetDiplomacyAI()->GetBoldness();
		if (iBoldness > 5)
			iSweetMin--;
		if (iBoldness < 5)
			iSweetMin++;

		if (iOtherCityDistance<=iSweetMin)
		{
			iStratModifier -= iTotalPlotValue / 2;
			vQualifiersNegative.push_back("(S) too close to existing enemy city");
		}
	}

	//logging logging logging
	if (pDebug)
	{
		pDebug->Format("%d,%d,%d,%d", iTotalPlotValue, iValueModifier, iStratModifier, iCivModifier);
		for (size_t i=0; i<vQualifiersPositive.size();i++)
		{
			pDebug->append(",positive: ");
			pDebug->append(vQualifiersPositive[i].c_str());
		}
		for (size_t i=0; i<vQualifiersNegative.size();i++)
		{
			pDebug->append(",negative: ");
			pDebug->append(vQualifiersNegative[i].c_str());
		}
	}

	return max(0,iTotalPlotValue + iValueModifier + iStratModifier + iCivModifier);
}

/// Retrieve the relative fertility of this plot (alone)
int CvCitySiteEvaluator::PlotFertilityValue(CvPlot* pPlot, bool bAllPlots)
{
	int rtnValue = 0;
	if( bAllPlots || (!pPlot->isWater() && pPlot->isValidMovePlot(NO_PLAYER)) )
	{
		rtnValue += ComputeFoodValue(pPlot, NULL);
		rtnValue += ComputeProductionValue(pPlot, NULL);
		rtnValue += ComputeGoldValue(pPlot, NULL);
		rtnValue += ComputeScienceValue(pPlot, NULL);
		rtnValue += ComputeTradeableResourceValue(pPlot, NULL);
	}

	if(rtnValue < 0) rtnValue = 0;

	return rtnValue;
}

/// How strong a city site can we find nearby for this type of yield?
int CvCitySiteEvaluator::BestFoundValueForSpecificYield(CvPlayer* pPlayer, YieldTypes eYield)
{
	pPlayer;
	eYield;
	return 0;
}

// PROTECTED METHODS (can be overridden in derived classes)

/// Value of plot for providing food
int CvCitySiteEvaluator::ComputeFoodValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	// From tile yield
	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FOOD, NO_PLAYER);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FOOD, pPlayer->GetID());
	}

#if defined(MOD_BALANCE_CORE_SETTLER)
	// assume a farm or similar on suitable terrain ... should be build sooner or later. value averages out with other improvements ...
	if (MOD_BALANCE_CORE_SETTLER && (pPlot->getTerrainType()==TERRAIN_GRASS || pPlot->getTerrainType()==TERRAIN_PLAINS || pPlot->getFeatureType()==FEATURE_FLOOD_PLAINS))
		rtnValue += 1;

	//Help with island settling - assume a lighthouse
	if(pPlot->isShallowWater())
	{
		rtnValue += 1;
	}
#endif

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		//can we build an improvement on this resource? assume we will do it (natural yield is already considered)

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_FOOD);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_FOOD];
}

/// Value of plot for providing Happiness
int CvCitySiteEvaluator::ComputeHappinessValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		// Add a bonus if adds Happiness
		if(!pPlot->isOwned())
		{
			rtnValue += GC.getResourceInfo(eResource)->getHappiness();
		}

		// If we don't have this resource yet, increase it's value
		if(pPlayer)
		{
			if(pPlayer->getNumResourceTotal(eResource) == 0)
				rtnValue *= 5;
		}
	}

	return rtnValue * m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS];
}

/// Value of plot for providing hammers
int CvCitySiteEvaluator::ComputeProductionValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	// From tile yield
	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_PRODUCTION, NO_PLAYER);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_PRODUCTION, pPlayer->GetID());
	}

#if defined(MOD_BALANCE_CORE_SETTLER)
	// assume a mine or similar ...
	if (MOD_BALANCE_CORE_SETTLER && pPlot->isHills())
		rtnValue += 1;
#endif

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		//can we build an improvement on this resource? assume we will do it (natural yield is already considered)

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_PRODUCTION);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_PRODUCTION];
}

/// Value of plot for providing gold
int CvCitySiteEvaluator::ComputeGoldValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	// From tile yield
	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_GOLD, NO_PLAYER);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_GOLD, pPlayer->GetID());
	}

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		//can we build an improvement on this resource? assume we will do it (natural yield is already considered)

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_GOLD);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_GOLD];
}

/// Value of plot for providing science
int CvCitySiteEvaluator::ComputeScienceValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;

	// From tile yield
	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_SCIENCE, NO_PLAYER);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_SCIENCE, pPlayer->GetID());
	}

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		//can we build an improvement on this resource? assume we will do it (natural yield is already considered)

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_SCIENCE);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_SCIENCE];
}

/// Vale of plot for providing faith
int CvCitySiteEvaluator::ComputeFaithValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;

	// From tile yield
	if(pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FAITH, NO_PLAYER);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FAITH, pPlayer->GetID());
	}

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if(eResource != NO_RESOURCE)
	{
		//can we build an improvement on this resource? assume we will do it (natural yield is already considered)

		CvImprovementEntry* pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if(pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_FAITH);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_FAITH];
}


/// Value of plot for providing tradeable resources
int CvCitySiteEvaluator::ComputeTradeableResourceValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;

	// If we already own this Tile then we already have access to the Strategic Resource
	if(pPlot->isOwned())
	{
		return rtnValue;
	}

	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);

	if(eResource != NO_RESOURCE)
	{
		ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eResource)->getResourceUsage();

		// Multiply number of tradeable resources by flavor value
		if(eResourceUsage == RESOURCEUSAGE_LUXURY || eResourceUsage == RESOURCEUSAGE_STRATEGIC)
		{
			rtnValue += pPlot->getNumResource() * m_iFlavorMultiplier[SITE_EVALUATION_RESOURCES];

			if(pPlayer)
			{
				// If we don't have this resource yet, increase it's value
				if(pPlayer->getNumResourceTotal(eResource) == 0)
					rtnValue *= 3;
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
				if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES && (GC.getMap().getNumResources(eResource) > 0))
				{
					//Will this get us closer to a monopoly?
					if((((pPlot->getNumResource() + pPlayer->getNumResourceTotal(eResource, false) + pPlayer->getResourceExport(eResource)) * 100) / GC.getMap().getNumResources(eResource)) >= 30)
					{
						rtnValue *= 2;
					}
					else if((((pPlot->getNumResource() + pPlayer->getNumResourceTotal(eResource, false)) * 100) / GC.getMap().getNumResources(eResource)) >= 50)
					{
						rtnValue *= 10;
					}
				}
#endif
			}
		}
	}

	return rtnValue;
}

/// Value of plot for providing strategic value
int CvCitySiteEvaluator::ComputeStrategicValue(CvPlot* pPlot, const CvPlayer*, int iPlotsFromCity)
{
	int rtnValue = 0;

	CvAssert(pPlot);
	if(!pPlot) return rtnValue;

#if defined(MOD_BALANCE_CORE_SETTLER)
	if (MOD_BALANCE_CORE_SETTLER) 
	{
		//Some features and terrain types are useful strategically. (Or really bad)
		if(pPlot->getOwner() == NO_PLAYER)
		{
			if(iPlotsFromCity <= 3 && (pPlot->getFeatureType() == FEATURE_ICE))
			{
				rtnValue += /*-10*/ GC.getBALANCE_BAD_TILES_STRATEGIC_VALUE();
			}
			if(iPlotsFromCity <= 3 && pPlot->isFreshWater_cached())
			{
				rtnValue += /*2*/ GC.getBALANCE_FRESH_WATER_STRATEGIC_VALUE();
			}
			if(iPlotsFromCity <= 3 && pPlot->isCoastalLand())
			{
				rtnValue += /*2*/ GC.getBALANCE_COAST_STRATEGIC_VALUE();
			}
		}
	}
#endif

	// Hills in first ring are useful for defense and production
	if(iPlotsFromCity == 1 && pPlot->isHills())
	{
		rtnValue += /*3*/ GC.getHILL_STRATEGIC_VALUE();
	}

	// Some Features are less attractive to settle in, (e.g. Jungles, since it takes a while before you can clear them and they slow down movement)
	if(pPlot->getFeatureType() != NO_FEATURE)
	{
		int iWeight = GC.getFeatureInfo(pPlot->getFeatureType())->getStartingLocationWeight();
		if(iWeight != 0 && iPlotsFromCity == 1)
		{
			rtnValue += iWeight;
		}
	}

	rtnValue *= m_iFlavorMultiplier[SITE_EVALUATION_STRATEGIC];

	return rtnValue;
}

//=====================================
// CvSiteEvaluatorForSettler
//=====================================
/// Constructor
CvSiteEvaluatorForSettler::CvSiteEvaluatorForSettler(void)
{
}

/// Destructor
CvSiteEvaluatorForSettler::~CvSiteEvaluatorForSettler(void)
{
}

/// Value of this site for a settler
#if defined(MOD_BALANCE_CORE_SETTLER)
int CvSiteEvaluatorForSettler::PlotFoundValue(CvPlot* pPlot, const CvPlayer* pPlayer, YieldTypes eYield, bool bCoastOnly, CvString* pDebug)
{
	CvAssert(pPlot);
	if(!pPlot)
	{
		return 0;
	}

	// Is there any reason this site doesn't work for a settler?
	//
	// First must be on coast if settling a new continent
	bool bIsCoastal = pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN());
	CvArea* pArea = pPlot->area();
	CvAssert(pArea);
	if(!pArea) 
	{
		return 0;
	}

	int iNumAreaCities = pArea->getCitiesPerPlayer(pPlayer->GetID());
	if(bCoastOnly && !bIsCoastal && iNumAreaCities == 0)
	{
		return 0;
	}

	// Seems okay for a settler, use base class to determine exact value
	else
	{
		// if the civ gets a benefit from settling on a new continent (ie: Indonesia) double the fertility of that plot
		if (pPlayer->GetPlayerTraits()->WillGetUniqueLuxury(pArea))
		{
			return CvCitySiteEvaluator::PlotFoundValue(pPlot, pPlayer, eYield, bCoastOnly, pDebug) * 2;
		}
		else
		{
			return CvCitySiteEvaluator::PlotFoundValue(pPlot, pPlayer, eYield, bCoastOnly, pDebug);
		}
	}
}
#else
int CvSiteEvaluatorForSettler::PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield, bool bCoastOnly)
{
	CvAssert(pPlot);
	if(!pPlot) return 0;

	if(!CanFound(pPlot, pPlayer, true))
	{
		return 0;
	}

	// Is there any reason this site doesn't work for a settler?
	//
	// First must be on coast if settling a new continent
	bool bIsCoastal = pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN());
	CvArea* pArea = pPlot->area();
	CvAssert(pArea);
	if(!pArea) return 0;
	int iNumAreaCities = pArea->getCitiesPerPlayer(pPlayer->GetID());
	if(bCoastOnly && !bIsCoastal && iNumAreaCities == 0)
	{
		return 0;
	}

	// Seems okay for a settler, use base class to determine exact value
	else
	{
		// if the civ gets a benefit from settling on a new continent (ie: Indonesia)
		// double the fertility of that plot
		int iLuxuryModifier = 0;
		if (pPlayer->GetPlayerTraits()->WillGetUniqueLuxury(pArea))
		{
			iLuxuryModifier = CvCitySiteEvaluator::PlotFoundValue(pPlot, pPlayer, eYield) * 2;
			return iLuxuryModifier;
		}
		else
		{
			return CvCitySiteEvaluator::PlotFoundValue(pPlot, pPlayer, eYield);
		}
	}
}
#endif

//=====================================
// CvSiteEvaluatorForStart
//=====================================
/// Constructor
CvSiteEvaluatorForStart::CvSiteEvaluatorForStart(void)
{
}

/// Destructor
CvSiteEvaluatorForStart::~CvSiteEvaluatorForStart(void)
{
}

/// Overridden - ignore flavors for initial site selection
void CvSiteEvaluatorForStart::ComputeFlavorMultipliers(CvPlayer*)
{
	// Set all to 1; we assign start position without considering flavors yet
	for(int iI = 0; iI < NUM_SITE_EVALUATION_FACTORS; iI++)
	{
		m_iFlavorMultiplier[iI] = 1;
	}
}


// *****
// ***** This method gets called pre-game if using a WB map (.civ5map), in which case pPlayer is NULL
// *****
/// Value of this site for a civ starting location
int CvSiteEvaluatorForStart::PlotFoundValue(CvPlot* pPlot, CvPlayer*, YieldTypes, bool)
{
	int rtnValue = 0;

	if(!pPlot) 
		return rtnValue;

	if(!CanFound(pPlot, NULL, false))
	{
		return rtnValue;
	}

	// Is there any reason this site doesn't work for a start location?
	//
	// Not on top of a goody hut
	if(pPlot->isGoody())
	{
		return 0;
	}

	// We have our own special method of scoring, so don't call the base class for that (like settler version does)
	for(int iI = 0; iI < RING3_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);

		// Too close to map edge?
		if(pLoopPlot == NULL)
		{
			return 0;
		}
		else
		{
			int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
			int iRingModifier = m_iRingModifier[iDistance];

			// Skip the city plot itself for now
			if(iDistance != 0)
			{
				rtnValue += iRingModifier * ComputeFoodValue(pLoopPlot, NULL) * /*6*/ GC.getSTART_AREA_FOOD_MULTIPLIER();
				rtnValue += iRingModifier * ComputeHappinessValue(pLoopPlot, NULL) * /*12*/ GC.getSTART_AREA_HAPPINESS_MULTIPLIER();
				rtnValue += iRingModifier * ComputeProductionValue(pLoopPlot, NULL) * /*8*/ GC.getSTART_AREA_PRODUCTION_MULTIPLIER();
				rtnValue += iRingModifier * ComputeGoldValue(pLoopPlot, NULL) * /*2*/ GC.getSTART_AREA_GOLD_MULTIPLIER();
				rtnValue += iRingModifier * ComputeScienceValue(pLoopPlot, NULL) * /*1*/ GC.getSTART_AREA_SCIENCE_MULTIPLIER();
				rtnValue += iRingModifier * ComputeFaithValue(pLoopPlot, NULL) * /*1*/ GC.getSTART_AREA_FAITH_MULTIPLIER();
				rtnValue += iRingModifier * ComputeTradeableResourceValue(pLoopPlot, NULL) * /*1*/ GC.getSTART_AREA_RESOURCE_MULTIPLIER();
				rtnValue += iRingModifier * ComputeStrategicValue(pLoopPlot, NULL, iDistance) * /*1*/ GC.getSTART_AREA_STRATEGIC_MULTIPLIER();
			}
		}
	}

	if(rtnValue < 0) 
		rtnValue = 0;

	// Finally, look at the city plot itself and use it as an overall multiplier
	if(pPlot->getResourceType() != NO_RESOURCE)
	{
		rtnValue += rtnValue * GC.getBUILD_ON_RESOURCE_PERCENT() / 100;
	}

	if(pPlot->isRiver())
	{
		rtnValue += rtnValue * GC.getBUILD_ON_RIVER_PERCENT() / 100;
	}

	if(pPlot->isCoastalLand())
	{
		rtnValue += rtnValue * GC.getSTART_AREA_BUILD_ON_COAST_PERCENT() / 100;
	}

	return rtnValue;
}
