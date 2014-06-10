/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
	m_iRingModifier[0] = 0;   // Items under city get handled separately
	m_iRingModifier[1] = /*12*/ GC.getCITY_RING_1_MULTIPLIER();
	m_iRingModifier[2] = /*6*/ GC.getCITY_RING_2_MULTIPLIER();
	m_iRingModifier[3] = /*2*/ GC.getCITY_RING_3_MULTIPLIER();

	m_iGrowthIndex = GC.getInfoTypeForString("FLAVOR_GROWTH");
	m_iExpansionIndex = GC.getInfoTypeForString("FLAVOR_EXPANSION");
}

/// Is it valid for this player to found a city here?
bool CvCitySiteEvaluator::CanFound(CvPlot *pPlot, const CvPlayer *pPlayer, bool bTestVisible) const
{
	CvAssert(pPlot);
	if(!pPlot)
		return false;


	CvPlot* pLoopPlot(NULL);
	bool bValid(false);
	int iRange(0), iDX(0), iDY(0);

	// Used to have a Python hook: CANNOT_FOUND_CITY_CALLBACK

	if (GC.getGame().isFinalInitialized())
	{
		if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer && pPlayer->isHuman())
		{
			if (pPlayer->getNumCities() > 0)
			{
				return false;
			}
		}
	}

	if (pPlot->isImpassable())
	{
		return false;
	}

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		if (GC.getFeatureInfo(pPlot->getFeatureType())->isNoCity())
		{
			return false;
		}
	}

	if (pPlayer)
	{
		if (pPlot->isOwned() && (pPlot->getOwner() != pPlayer->GetID()))
		{
			return false;
		}
	}

	CvTerrainInfo* pTerrainInfo = GC.getTerrainInfo(pPlot->getTerrainType());

	if (!bValid)
	{
		if (pTerrainInfo->isFound())
		{
			bValid = true;
		}
	}

	if (!bValid)
	{
		if (pTerrainInfo->isFoundCoast())
		{
			if (pPlot->isCoastalLand())
			{
				bValid = true;
			}
		}
	}

	if (!bValid)
	{
		if (pTerrainInfo->isFoundFreshWater())
		{
			if (pPlot->isFreshWater())
			{
				bValid = true;
			}
		}
	}

	// Used to have a Python hook: CAN_FOUND_CITIES_ON_WATER_CALLBACK

	if (pPlot->isWater())
	{
		return false;
	}

	if (!bValid)
	{
		return false;
	}

	if (!bTestVisible)
	{
		// look at same land mass
		iRange = GC.getMIN_CITY_RANGE();

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isCity())
					{
						if (pLoopPlot->getLandmass() == pPlot->getLandmass())
						{
							return false;
						}
						else if (hexDistance(iDX, iDY) < iRange) // one less for off shore
						{
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}

/// Setup flavor multipliers - call once per player before PlotFoundValue() or PlotFertilityValue()
void CvCitySiteEvaluator::ComputeFlavorMultipliers(CvPlayer *pPlayer)
{
	// Set all to 0
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_iFlavorMultiplier[iI] = 0;
	}

	m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] = 0;

	// Find out if player has a desired next city specialization
	CitySpecializationTypes eNextSpecialization = pPlayer->GetCitySpecializationAI()->GetNextSpecializationDesired();
	CvCitySpecializationXMLEntry* pkCitySpecializationEntry = NULL;
	if(eNextSpecialization != NO_CITY_SPECIALIZATION)
		pkCitySpecializationEntry = GC.getCitySpecializationInfo(eNextSpecialization);


	for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		const FlavorTypes eFlavor = static_cast<FlavorTypes>(iFlavorLoop);
		const CvString& strFlavor = GC.getFlavorTypes(eFlavor);
		if (strFlavor == "FLAVOR_GROWTH" ||
			strFlavor == "FLAVOR_EXPANSION")
		{
			m_iFlavorMultiplier[YIELD_FOOD] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if (pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_FOOD] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if (strFlavor == "FLAVOR_GOLD" ||
				 strFlavor == "FLAVOR_TILE_IMPROVEMENT")
		{
			m_iFlavorMultiplier[YIELD_GOLD] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if (pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_GOLD] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if (strFlavor == "FLAVOR_PRODUCTION" ||
				 strFlavor == "FLAVOR_WONDER")
		{
			m_iFlavorMultiplier[YIELD_PRODUCTION] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
			if (pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_PRODUCTION] += pkCitySpecializationEntry->GetFlavorValue(eFlavor);
			}
		}
		else if (strFlavor == "FLAVOR_SCIENCE")
		{
			// Doubled since only one flavor related to science
			m_iFlavorMultiplier[YIELD_SCIENCE] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if (pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[YIELD_SCIENCE] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
		else if (strFlavor == "FLAVOR_HAPPINESS")
		{
			// Doubled since only one flavor related to Happiness
			m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] += pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor) * 2;
			if (pkCitySpecializationEntry)
			{
				m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS] += pkCitySpecializationEntry->GetFlavorValue(eFlavor) * 2;
			}
		}
	}

	// Make sure none are negative
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (m_iFlavorMultiplier[iI] < 0)
		{
			m_iFlavorMultiplier[iI] = 0;
		}
	}

	// Set tradable resources and strategic value to times 10 (so multiplying this by the number of map gives a number from 1 to 100)
	m_iFlavorMultiplier[SITE_EVALUATION_RESOURCES] = 10;
	m_iFlavorMultiplier[SITE_EVALUATION_STRATEGIC] = 10;
}

/// Retrieve the relative value of this plot (including plots that would be in city radius)
int CvCitySiteEvaluator::PlotFoundValue(CvPlot *pPlot, CvPlayer *pPlayer, YieldTypes eYield, bool)
{
	CvAssert(pPlot);
	if(!pPlot)
		return 0;

	// Make sure this player can even build a city here
	if (!CanFound(pPlot, pPlayer, false))
	{
		return 0;
	}

	int rtnValue = 0;

	int iFoodValue = 0;
	int iHappinessValue = 0;
	int iProductionValue = 0;
	int iGoldValue = 0;
	int iScienceValue = 0;
	int iResourceValue = 0;
	int iStrategicValue = 0;

	int iTotalFoodValue = 0;
	int iTotalHappinessValue = 0;
	int iTotalProductionValue = 0;
	int iTotalGoldValue = 0;
	int iTotalScienceValue = 0;
	int iTotalResourceValue = 0;
	int iTotalStrategicValue = 0;

	bool bMyCityAtRing3 = false;
	bool bMyCityAtRing4 = false;
	bool bMyCityAtRing5 = false;
	bool bEnemyCityAtRing3 = false;
	bool bEnemyCityAtRing4 = false;
	bool bEnemyCityAtRing5 = false;

	for (int iDX = -5; iDX <= 5; iDX++)
	{
		for (int iDY = -5; iDY <= 5; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
				if (iDistance <= 5)
				{
					if ((pLoopPlot->getOwner() == NO_PLAYER) || (pLoopPlot->getOwner() == pPlayer->GetID()))
					{
						bool bIsMyCity = pLoopPlot->isCity();
						// See if there are other cities nearby
						if (iDistance == 3 && bIsMyCity)
							bMyCityAtRing3 = true;
						if (iDistance == 4 && bIsMyCity)
							bMyCityAtRing4 = true;
						if (iDistance == 5 && bIsMyCity)
							bMyCityAtRing5 = true;

						// Skip the city plot itself for now
						if (iDistance != 0 && iDistance <= NUM_CITY_RINGS)
						{
							int iRingModifier = m_iRingModifier[iDistance];

							iFoodValue = 0;
							iProductionValue = 0;
							iGoldValue = 0;
							iScienceValue = 0;
							iHappinessValue = 0;
							iResourceValue = 0;
							iStrategicValue = 0;

							if (eYield == NO_YIELD || eYield == YIELD_FOOD)
							{
								iFoodValue = iRingModifier * ComputeFoodValue(pLoopPlot, pPlayer) * /*15*/ GC.getSETTLER_FOOD_MULTIPLIER();
							}
							if (eYield == NO_YIELD || eYield == YIELD_PRODUCTION)
							{
								iProductionValue = iRingModifier * ComputeProductionValue(pLoopPlot, pPlayer) * /*3*/ GC.getSETTLER_PRODUCTION_MULTIPLIER();
							}
							if (eYield == NO_YIELD || eYield == YIELD_GOLD)
							{
								iGoldValue = iRingModifier * ComputeGoldValue(pLoopPlot, pPlayer) * /*2*/ GC.getSETTLER_GOLD_MULTIPLIER();
							}
							if (eYield == NO_YIELD || eYield == YIELD_SCIENCE)
							{
								iScienceValue = iRingModifier * ComputeScienceValue(pLoopPlot, pPlayer) * /*1*/ GC.getSETTLER_SCIENCE_MULTIPLIER();
							}
							if (eYield == NO_YIELD)
							{
								iHappinessValue = iRingModifier * ComputeHappinessValue(pLoopPlot, pPlayer) * /*6*/ GC.getSETTLER_HAPPINESS_MULTIPLIER();
								iResourceValue = iRingModifier * ComputeTradeableResourceValue(pLoopPlot, pPlayer) * /*1*/ GC.getSETTLER_RESOURCE_MULTIPLIER();
								iStrategicValue = ComputeStrategicValue(pLoopPlot, pPlayer, iDistance) * /*1*/ GC.getSETTLER_STRATEGIC_MULTIPLIER();  // the ring is included in the computation
							}

							iTotalFoodValue += iFoodValue;
							iTotalHappinessValue += iHappinessValue;
							iTotalProductionValue += iProductionValue;
							iTotalGoldValue += iGoldValue;
							iTotalScienceValue += iScienceValue;
							iTotalResourceValue += iResourceValue;
							iTotalStrategicValue += iStrategicValue;

							int iPlotValue = iFoodValue + iHappinessValue + iProductionValue + iGoldValue + iScienceValue + iResourceValue + iStrategicValue;

							// if this tile is a NW boost the value just so that we force the AI to claim them
							if (pLoopPlot->IsNaturalWonder())
							{
								iPlotValue += iPlotValue * 2 + 10;
							}

							// lower value if we already own this tile
							if (pLoopPlot->getOwner() == pPlayer->GetID())
							{
								iPlotValue *= 2;
								iPlotValue /= 3;
							}

							// add this plot into the total
							rtnValue += iPlotValue;

						}
					}
					else // this tile is owned by someone else
					{
						bool bIsEnemyCity = pLoopPlot->isCity();
						// See if there are other cities nearby
						if (bIsEnemyCity)
						{
							if (iDistance == 3)
								bEnemyCityAtRing3 = true;
							if (iDistance == 4)
								bEnemyCityAtRing4 = true;
							if (iDistance == 5)
								bEnemyCityAtRing5 = true;
						}
					}
				}
			}
		}
	}

	if (rtnValue < 0) rtnValue = 0;

	// Finally, look at the city plot itself and use it as an overall multiplier
	if (pPlot->getResourceType(pPlayer->getTeam()) != NO_RESOURCE)
	{
		rtnValue += (int)rtnValue * /*-50*/ GC.getBUILD_ON_RESOURCE_PERCENT() / 100;
	}

	if (pPlot->isRiver())
	{
		rtnValue += (int)rtnValue * /*15*/ GC.getBUILD_ON_RIVER_PERCENT() / 100;
	}

	if (pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		rtnValue += (int)rtnValue * /*10*/ GC.getSETTLER_BUILD_ON_COAST_PERCENT() / 100;
	}

	// Nearby Cities?

	// Human
	if (pPlayer != NULL && pPlayer->isHuman())
	{
		if (bMyCityAtRing3)
			rtnValue /= 2;
	}
	// AI
	else
	{
		int iGrowthFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iGrowthIndex);

		if (iGrowthFlavor > 7)
		{
			if (bMyCityAtRing5)
			{
				rtnValue *= 3;
				rtnValue /= 2;
			}
			else if (!bMyCityAtRing4)
			{
				rtnValue /= 2;
			}
		}
		else
		{
			if (bMyCityAtRing4)
			{
				rtnValue *= 3;
				rtnValue /= 2;
			}
			else if (!bMyCityAtRing5)
			{
				rtnValue /= 2;
			}
		}

		// use boldness to decide if we want to push close to enemies
		int iBoldness = pPlayer->GetDiplomacyAI()->GetBoldness();
		if (iBoldness <= 4)
		{
			if (bEnemyCityAtRing4)
			{
				rtnValue /= 4;
			}
			else if (bEnemyCityAtRing5)
			{
				rtnValue /= 2;
			}
		}
		else if (iBoldness >= 6)
		{
			if (bEnemyCityAtRing4 || bEnemyCityAtRing5)
			{
				rtnValue *= 3;
				rtnValue /= 2;
			}
		}
		else
		{
			if (bEnemyCityAtRing4)
			{
				rtnValue *= 2;
				rtnValue /= 3;
			}
		}

	}

	return rtnValue;
}

/// Retrieve the relative fertility of this plot (alone)
int CvCitySiteEvaluator::PlotFertilityValue(CvPlot *pPlot)
{
	int rtnValue = 0;

	if (!pPlot->isWater() && !pPlot->isImpassable())
	{
		rtnValue += ComputeFoodValue(pPlot, NULL);
		rtnValue += ComputeProductionValue(pPlot, NULL);
		rtnValue += ComputeGoldValue(pPlot, NULL);
		rtnValue += ComputeScienceValue(pPlot, NULL);
		rtnValue += ComputeTradeableResourceValue(pPlot, NULL);
	}

	if (rtnValue < 0) rtnValue = 0;

	return rtnValue;
}

/// How strong a city site can we find nearby for this type of yield?
int CvCitySiteEvaluator::BestFoundValueForSpecificYield(CvPlayer *pPlayer, YieldTypes eYield)
{
	pPlayer; eYield;
	return 0;
}

// PROTECTED METHODS (can be overridden in derived classes)

/// Value of plot for providing food
int CvCitySiteEvaluator::ComputeFoodValue(CvPlot *pPlot, CvPlayer *pPlayer)
{
	int rtnValue = 0;

	// From tile yield
	if (pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FOOD, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_FOOD, pPlayer->getTeam());
	}

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if (pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if (eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_FOOD);

		CvImprovementEntry *pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if (pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_FOOD);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_FOOD];
}

/// Value of plot for providing Happiness
int CvCitySiteEvaluator::ComputeHappinessValue(CvPlot *pPlot, CvPlayer *pPlayer)
{
	int rtnValue = 0;

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if (pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if (eResource != NO_RESOURCE)
	{
		// Add a bonus if adds Happiness
		if (!pPlot->isOwned())
		{
			rtnValue += GC.getResourceInfo(eResource)->getHappiness();
		}

		// If we don't have this resource yet, increase it's value
		if (pPlayer)
		{
			if (pPlayer->getNumResourceTotal(eResource) == 0)
				rtnValue *= 5;
		}
	}

	return rtnValue * m_iFlavorMultiplier[SITE_EVALUATION_HAPPINESS];
}

/// Value of plot for providing hammers
int CvCitySiteEvaluator::ComputeProductionValue(CvPlot *pPlot, CvPlayer *pPlayer)
{
	int rtnValue = 0;

	// From tile yield
	if (pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_PRODUCTION, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_PRODUCTION, pPlayer->getTeam());
	}

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if (pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if (eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_PRODUCTION);

		CvImprovementEntry *pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if (pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_PRODUCTION);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_PRODUCTION];
}

/// Value of plot for providing gold
int CvCitySiteEvaluator::ComputeGoldValue(CvPlot *pPlot, CvPlayer *pPlayer)
{
	int rtnValue = 0;

	// From tile yield
	if (pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_GOLD, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_GOLD, pPlayer->getTeam());
	}

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if (pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if (eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_GOLD);

		CvImprovementEntry *pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if (pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_GOLD);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_GOLD];
}

/// Value of plot for providing science
int CvCitySiteEvaluator::ComputeScienceValue(CvPlot *pPlot, CvPlayer *pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot); if(!pPlot) return rtnValue;

	// From tile yield
	if (pPlayer == NULL)
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_SCIENCE, NO_TEAM);
	}
	else
	{
		rtnValue += pPlot->calculateNatureYield(YIELD_SCIENCE, pPlayer->getTeam());
	}

	// From resource
	TeamTypes eTeam = NO_TEAM;
	if (pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);
	if (eResource != NO_RESOURCE)
	{
		rtnValue += GC.getResourceInfo(eResource)->getYieldChange(YIELD_SCIENCE);

		CvImprovementEntry *pImprovement = GC.GetGameImprovements()->GetImprovementForResource(eResource);
		if (pImprovement)
		{
			rtnValue += pImprovement->GetImprovementResourceYield(eResource, YIELD_SCIENCE);
		}
	}

	return rtnValue * m_iFlavorMultiplier[YIELD_SCIENCE];
}

/// Value of plot for providing tradeable resources
int CvCitySiteEvaluator::ComputeTradeableResourceValue(CvPlot *pPlot, CvPlayer *pPlayer)
{
	int rtnValue = 0;

	CvAssert(pPlot); if(!pPlot) return rtnValue;

	// If we already own this Tile then we already have access to the Strategic Resource
	if (pPlot->isOwned())
	{
		return rtnValue;
	}

	TeamTypes eTeam = NO_TEAM;
	if (pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource;
	eResource = pPlot->getResourceType(eTeam);

	if (eResource != NO_RESOURCE)
	{
		ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eResource)->getResourceUsage();

		// Multiply number of tradeable resources by flavor value
		if (eResourceUsage == RESOURCEUSAGE_LUXURY || eResourceUsage == RESOURCEUSAGE_STRATEGIC)
		{
			rtnValue += pPlot->getNumResource() * m_iFlavorMultiplier[SITE_EVALUATION_RESOURCES];

			if (pPlayer)
			{
				// If we don't have this resource yet, increase it's value
				if (pPlayer->getNumResourceTotal(eResource) == 0)
					rtnValue *= 3;
			}
		}
	}

	return rtnValue;
}

/// Value of plot for providing strategic value
int CvCitySiteEvaluator::ComputeStrategicValue(CvPlot *pPlot, CvPlayer *pPlayer, int iPlotsFromCity)
{
	int rtnValue = 0;

	CvAssert(pPlot); if(!pPlot) return rtnValue;

	// Possible chokepoint if impassable terrain and exactly 2 plots from city
	if (iPlotsFromCity == 2 && pPlot->isImpassable())
	{
		rtnValue += /*5*/ GC.getCHOKEPOINT_STRATEGIC_VALUE();
	}

	// Hills in first ring are useful for defense and production
	if (iPlotsFromCity == 1 && pPlot->isHills())
	{
		rtnValue += /*3*/ GC.getHILL_STRATEGIC_VALUE();
	}

	// Some Features are less attractive to settle in, (e.g. Jungles, since it takes a while before you can clear them and they slow down movement)
	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		int iWeight = GC.getFeatureInfo(pPlot->getFeatureType())->getStartingLocationWeight();
		if (iWeight != 0 && iPlotsFromCity == 1)
		{
			rtnValue += iWeight;
		}
	}

	// Nearby City
	if (pPlayer != NULL && pPlot->isCity())
	{
//		if (pPlot->getOwner() == pPlayer->getID())
		{
			rtnValue += /*-1000*/ GC.getALREADY_OWNED_STRATEGIC_VALUE();
		}
	}

	// POSSIBLE FUTURE: Is there any way for us to know to grab land between us and another major civ?

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
int CvSiteEvaluatorForSettler::PlotFoundValue(CvPlot *pPlot, CvPlayer *pPlayer, YieldTypes eYield, bool bCoastOnly)
{
	CvAssert(pPlot); if(!pPlot) return 0;

	if (!CanFound(pPlot, pPlayer, true))
	{
		return 0;
	}

	// Is there any reason this site doesn't work for a settler?
	//
	// First must be on coast if settling a new continent
	bool bIsCoastal = pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN());
	CvArea *pArea = pPlot->area();
	CvAssert(pArea); if(!pArea) return 0;
	int iNumAreaCities = pArea->getCitiesPerPlayer(pPlayer->GetID());
	if (bCoastOnly && !bIsCoastal && iNumAreaCities == 0)
	{
		return 0;
	}

	// Seems okay for a settler, use base class to determine exact value
	else
	{
		return CvCitySiteEvaluator::PlotFoundValue(pPlot, pPlayer, eYield);
	}
}

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
	for (int iI = 0; iI < NUM_SITE_EVALUATION_FACTORS; iI++)
	{
		m_iFlavorMultiplier[iI] = 1;
	}
}

/// Value of this site for a civ starting location
int CvSiteEvaluatorForStart::PlotFoundValue(CvPlot *pPlot, CvPlayer *pPlayer, YieldTypes, bool)
{
	int rtnValue = 0;
	int iI;
	CvPlot *pLoopPlot(NULL);

	CvAssert(pPlot); if(!pPlot) return rtnValue;

	if (!CanFound(pPlot, pPlayer, false))
	{
		return rtnValue;
	}

	// Is there any reason this site doesn't work for a start location?
	//
	// Not on top of a goody hut
	if (pPlot->isGoody())
	{
		return 0;
	}

	// We have our own special method of scoring, so don't call the base class for that (like settler version does)
	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = plotCity(pPlot->getX(), pPlot->getY(), iI);

		// Too close to map edge?
		if (pLoopPlot == NULL)
		{
			return 0;
		}
		else
		{
			int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
			CvAssert (iDistance <= NUM_CITY_RINGS); if(iDistance > NUM_CITY_RINGS) continue;
			int iRingModifier = m_iRingModifier[iDistance];

			// Skip the city plot itself for now
			if (iDistance != 0)
			{
				rtnValue += iRingModifier * ComputeFoodValue(pLoopPlot, pPlayer) * /*6*/ GC.getSTART_AREA_FOOD_MULTIPLIER();
				rtnValue += iRingModifier * ComputeHappinessValue(pLoopPlot, pPlayer) * /*12*/ GC.getSTART_AREA_HAPPINESS_MULTIPLIER();
				rtnValue += iRingModifier * ComputeProductionValue(pLoopPlot, pPlayer) * /*8*/ GC.getSTART_AREA_PRODUCTION_MULTIPLIER();
				rtnValue += iRingModifier * ComputeGoldValue(pLoopPlot, pPlayer) * /*2*/ GC.getSTART_AREA_GOLD_MULTIPLIER();
				rtnValue += iRingModifier * ComputeScienceValue(pLoopPlot, pPlayer) * /*1*/ GC.getSTART_AREA_SCIENCE_MULTIPLIER();
				rtnValue += iRingModifier * ComputeTradeableResourceValue(pLoopPlot, pPlayer) * /*1*/ GC.getSTART_AREA_RESOURCE_MULTIPLIER();
				rtnValue += iRingModifier * ComputeStrategicValue(pLoopPlot, pPlayer, iDistance) * /*1*/ GC.getSTART_AREA_STRATEGIC_MULTIPLIER();
			}
		}
	}

	if (rtnValue < 0) rtnValue = 0;

	// Finally, look at the city plot itself and use it as an overall multiplier
		if (pPlot->getResourceType() != NO_RESOURCE)
	{
		rtnValue += rtnValue * GC.getBUILD_ON_RESOURCE_PERCENT() / 100;
	}

	if (pPlot->isRiver())
	{
		rtnValue += rtnValue * GC.getBUILD_ON_RIVER_PERCENT() / 100;
	}

	if (pPlot->isCoastalLand())
	{
		rtnValue += rtnValue * GC.getSTART_AREA_BUILD_ON_COAST_PERCENT() / 100;
	}

	return rtnValue;
}