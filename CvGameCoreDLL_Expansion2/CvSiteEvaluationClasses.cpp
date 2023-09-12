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
#if defined(MOD_BALANCE_CORE)
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#endif

// include this after all other headers!
#include "LintFree.h"

//=====================================
// CvCitySiteEvaluator
//=====================================
/// Destructor
CvCitySiteEvaluator::~CvCitySiteEvaluator(void)
{
}

/// Initialize
CvCitySiteEvaluator::CvCitySiteEvaluator()
{
	// Set up city ring multipliers
	m_iRingModifier[0] = 1;   // Items under city get handled separately
	m_iRingModifier[1] = /*6*/ GD_INT_GET(CITY_RING_1_MULTIPLIER);
	m_iRingModifier[2] = /*6*/ GD_INT_GET(CITY_RING_2_MULTIPLIER);
	m_iRingModifier[3] = /*3*/ GD_INT_GET(CITY_RING_3_MULTIPLIER);
	m_iRingModifier[4] = /*1*/ GD_INT_GET(CITY_RING_4_MULTIPLIER);
	m_iRingModifier[5] = /*1*/ GD_INT_GET(CITY_RING_5_MULTIPLIER);
	m_iRingModifier[6] = 0;
	m_iRingModifier[7] = 0;
	m_iRingModifier[8] = 0;
	m_iRingModifier[9] = 0;
	m_iRingModifier[10] = 0;
	m_iRingModifier[11] = 0;

	m_iGrowthIndex = GC.getInfoTypeForString("FLAVOR_GROWTH");
	m_iExpansionIndex = GC.getInfoTypeForString("FLAVOR_EXPANSION");
	m_iNavalIndex = GC.getInfoTypeForString("FLAVOR_NAVAL");

	for (int i=0; i<NUM_SITE_EVALUATION_FACTORS; i++)
		m_iFlavorMultiplier[i]=1;
}

/// Is it valid for this player to found a city here?
bool CvCitySiteEvaluator::CanFoundCity(const CvPlot* pPlot, const CvPlayer* pPlayer, bool bIgnoreDistanceToExistingCities) const
{
	CvAssert(pPlot);
	if(!pPlot)
		return false;

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

	// Used to have a Python hook: CAN_FOUND_CITIES_ON_WATER_CALLBACK

	if(pPlot->isWater())
	{
		return false;
	}

	if(!pPlot->isValidMovePlot(pPlayer ? pPlayer->GetID() : NO_PLAYER ))
	{
		return false;
	}

	if (pPlot->IsNaturalWonder())
	{
		return false;
	}

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

		if(pPlot->IsAdjacentOwnedByTeamOtherThan(pPlayer->getTeam()))
		{
			return false;
		}

		// Has the AI agreed to not settle here?
		if (!pPlayer->isHuman() && pPlayer->isMajorCiv())
		{
			vector<PlayerTypes> vNoSettlePlayers = pPlayer->GetDiplomacyAI()->GetPlayersWithNoSettlePolicy();
			for (size_t i = 0; i < vNoSettlePlayers.size(); i++)
			{
				int iDistanceToOtherPlayer = GET_PLAYER(vNoSettlePlayers[i]).GetCityDistanceInPlots(pPlot);
				if (iDistanceToOtherPlayer <= GET_PLAYER(vNoSettlePlayers[i]).GetDiplomacyAI()->GetExpansionBickerRange())
					return false;
			}
		}
	}

	CvTerrainInfo* pTerrainInfo = GC.getTerrainInfo(pPlot->getTerrainType());
	bool bValid = pTerrainInfo->isFound();

	if(!bValid)
	{
		if(pTerrainInfo->isFoundCoast())
		{
			//be careful in the pre-game, the cache might not be set up yet
			if(pPlot->isCoastalLand(-1,GC.getGame().getElapsedGameTurns()>0))
			{
				bValid = true;
			}
		}
	}

	if(!bValid)
	{
		if(pTerrainInfo->isFoundFreshWater())
		{
			if(pPlot->isFreshWater())
			{
				bValid = true;
			}
		}
	}

	if(!bValid)
	{
		return false;
	}

#if defined(MOD_BALANCE_CORE)
	if(!bIgnoreDistanceToExistingCities)
	{
		int iMinDist = /*3*/ GD_INT_GET(MIN_CITY_RANGE);

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

		int iDistanceToExisting = GC.getGame().GetClosestCityDistanceInPlots(pPlot);
		//watch out, it's <=
		if (iDistanceToExisting <= iMinDist)
			return false;
	}
#endif

	return true;
}

/// Setup flavor multipliers - call once per player before PlotFoundValue() or PlotFertilityValue()
void CvSiteEvaluatorForSettler::ComputeFlavorMultipliers(const CvPlayer* pPlayer)
{
	// Set all to 1 as base value
	for(int iI = 0; iI < NUM_SITE_EVALUATION_FACTORS; iI++)
	{
		m_iFlavorMultiplier[iI] = 1;
	}

	// Find out if player has a desired next city specialization
	CvCitySpecializationXMLEntry* pkCitySpecializationEntry = NULL;

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
int CvSiteEvaluatorForSettler::PlotFoundValue(CvPlot* pPlot, const CvPlayer* pPlayer, const std::vector<int>& ignorePlots, bool bCoastOnly, CvString* pDebug)
{
	CvAssert(pPlot);
	if(!pPlot)
		return -1;

	// Is there any reason this site doesn't work for a settler?
	// First must be on coast if settling a new continent
	CvArea* pArea = pPlot->area();
	if(pArea && pPlayer)
	{
		bool bIsCoastal = pPlot->isCoastalLand();
		int iNumAreaCities = pArea->getCitiesPerPlayer(pPlayer->GetID());
		if(bCoastOnly && !bIsCoastal && iNumAreaCities == 0)
		{
			return -1;
		}
	}

	// Make sure this player can even build a city here
	// This does not check visibility!
	if(!CanFoundCity(pPlot, pPlayer, false))
	{
		if(pDebug)
			*pDebug = "cannot found";
		return -1;
	}

	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;

	// AI does not settle on Antiquity Sites
	ResourceTypes ePlotResource = pPlot->getResourceType(eTeam);
	if (ePlotResource == (ResourceTypes)GD_INT_GET(ARTIFACT_RESOURCE) ||
		ePlotResource == (ResourceTypes)GD_INT_GET(HIDDEN_ARTIFACT_RESOURCE))
	{
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
	int iFloodPlainsCount = 0;
	int iIncaHillsCount = 0;

	int iLakeCount = 0;
	int iResourceLuxuryCount = 0;
	int iResourceStrategicCount = 0;
	int iResourceBonusCount = 0;

	//currently just for debugging
	int iTotalFoodValue = 0;
	int iTotalHappinessValue = 0;
	int iTotalProductionValue = 0;
	int iTotalGoldValue = 0;
	int iTotalScienceValue = 0;
	int iTotalFaithValue = 0;
	int iTotalTradeValue = 0;
	int iTotalStrategicValue = 0;

	//use a slightly negative base value to discourage settling in bad lands
	int iDefaultPlotValue = -100;

	int iBorderlandRange = /*5*/ GD_INT_GET(AI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT);

	bool bIsAlmostCoast = false;

	std::vector<SPlotWithScore> workablePlots;
	workablePlots.reserve(49);

	int nFoodPlots = 0;
	int nHammerPlots = 0;
	int nWaterPlots = 0;
	int nGoodPlots = 0;
	int iRange = pPlayer ? max(2,min(5,pPlayer->getWorkPlotDistance())) : 3;
	for (int iI=0; iI<RING_PLOTS[iRange]; iI++)
	{
		//do not check fog of war!
		CvPlot* pLoopPlot = iterateRingPlots(pPlot, iI);
		if (!pLoopPlot)
		{
			//AI never settle at the edge of the map
			if (iI < RING1_PLOTS)
				return -1;
			else
				continue;
		}

		//ignore some plots (typically enemy or close to enemy)
		if (plotDistance(*pLoopPlot,*pPlot)>1) //but only if we can't instantly claim them
			if (ignorePlots.size()==GC.getMap().numPlots() && ignorePlots[pLoopPlot->GetPlotIndex()] > 0)
				continue;

		int iDistance = plotDistance(*pPlot,*pLoopPlot);
		int iRingModifier = m_iRingModifier[iDistance];

		//not only our cities, also other player's cities!
		int iExistingCityDistance = GC.getGame().GetClosestCityDistanceInPlots(pLoopPlot);
		int iExistingFriendlyCityDistance = pPlayer ? pPlayer->GetCityDistanceInPlots(pLoopPlot) : 5;

		//count the tile only if the city will be able to work it (doesn't need to be passable)
		if (iExistingCityDistance<2  || pLoopPlot->isBeingWorked()) 
			iRingModifier = 0;
		else if (iExistingCityDistance==2)
			//this plot will likely be contested between the two cities, reduce its value
			iRingModifier = 1;

		int iPlotValue = iDefaultPlotValue;
		if (iRingModifier>0)
		{
			int iFoodValue = ComputeFoodValue(pLoopPlot, pPlayer) * /*12*/ GD_INT_GET(SETTLER_FOOD_MULTIPLIER);
			int iProductionValue = ComputeProductionValue(pLoopPlot, pPlayer) * /*8*/ GD_INT_GET(SETTLER_PRODUCTION_MULTIPLIER);
			int	iGoldValue = ComputeGoldValue(pLoopPlot, pPlayer) * /*8*/ GD_INT_GET(SETTLER_GOLD_MULTIPLIER);
			int iScienceValue = ComputeScienceValue(pLoopPlot, pPlayer) * /*5*/ GD_INT_GET(SETTLER_SCIENCE_MULTIPLIER);
			int	iFaithValue = ComputeFaithValue(pLoopPlot, pPlayer) * /*4*/ GD_INT_GET(SETTLER_FAITH_MULTIPLIER);
			//this is about strategic placement, not resources
			int iStrategicValue = ComputeStrategicValue(pLoopPlot, iDistance) * /*3*/ GD_INT_GET(SETTLER_STRATEGIC_MULTIPLIER);

			int iTradeValue = 0;
			int iHappinessValue = 0;
			if (pLoopPlot->getOwner() == NO_PLAYER) // there is no benefit if we already own these tiles
			{
				iHappinessValue = ComputeHappinessValue(pLoopPlot, pPlayer) * /*8*/ GD_INT_GET(SETTLER_HAPPINESS_MULTIPLIER);
				iTradeValue = ComputeTradeableResourceValue(pLoopPlot, pPlayer) * /*3*/ GD_INT_GET(SETTLER_RESOURCE_MULTIPLIER);

				//our existing city will catch those eventually
				if (iExistingFriendlyCityDistance<=3)
				{
					iHappinessValue /= 2;
					iTradeValue /= 2;
				}
			}

			iTotalFoodValue += iFoodValue;
			iTotalHappinessValue += iHappinessValue;
			iTotalProductionValue += iProductionValue;
			iTotalGoldValue += iGoldValue;
			iTotalScienceValue += iScienceValue;
			iTotalFaithValue += iFaithValue;
			iTotalTradeValue += iTradeValue;
			iTotalStrategicValue += iStrategicValue;

			//it's a bit awkward, happiness and trade value should have a higher modifier in ring 0 ...
			iPlotValue += iRingModifier * ( iFoodValue + iProductionValue + iGoldValue + iScienceValue + iFaithValue + iTradeValue + iHappinessValue ) + iStrategicValue;

			// need at least some food close by
			if (iDistance > 0 && iDistance < 3)
			{
				if (iFoodValue > 0)
					nFoodPlots++;
				// and some hammers
				if (iProductionValue > 0)
					nHammerPlots++;
				//natural wonders and super features like atolls
				if (pLoopPlot->getFeatureType() != NO_FEATURE)
				{
					CvFeatureInfo* pFeatureInfo = GC.getFeatureInfo(pLoopPlot->getFeatureType());
					if (pFeatureInfo->IsNaturalWonder())
						nGoodPlots++;
					if (pFeatureInfo->isAddsFreshWater())
						nGoodPlots++;
					int iTotalFeatureYield = 0;
					for (int i=0; i<YIELD_TOURISM; i++)
						iTotalFeatureYield += pFeatureInfo->getYieldChange((YieldTypes)i);
					if (iTotalFeatureYield >= 3)
						nGoodPlots++;
				}

			}
		}

		// for the central plot
		if (iDistance==0)
			if (pDebug) vQualifiersPositive.push_back( CvString::format("raw plot value %d", iPlotValue).c_str() );

		// avoid this
		if (iDistance==1 && !pPlot->isCoastalLand() && pLoopPlot->isCoastalLand())
			bIsAlmostCoast = true;

		// we don't want to be too exposed
		if (iDistance == 1 && pLoopPlot->isWater() && !pLoopPlot->isLake())
			nWaterPlots++;

		// if this tile is a NW boost the value
		if (pLoopPlot->IsNaturalWonder() && iPlotValue>0)
			iPlotValue *= 3; //yields will improve in later eras!

		// lower value a lot if we or somebody else already own this tile
		if (iPlotValue > 0 && pLoopPlot->getOwner() != NO_PLAYER)
			iPlotValue /= 2;

		// add this plot into the total
		workablePlots.push_back( SPlotWithScore(pLoopPlot,iPlotValue) );

		// some civ-specific checks
		FeatureTypes ePlotFeature = pLoopPlot->getFeatureType();
		ImprovementTypes ePlotImprovement = pLoopPlot->getImprovementType();

		if (ePlotFeature == FEATURE_FOREST && iDistance>0)
		{
			++iIroquoisForestCount;
			if (iDistance == 1 && ePlotImprovement == NO_IMPROVEMENT)
				++iCelticForestCount;
		}
		else if (ePlotFeature == FEATURE_JUNGLE && iDistance>0)
		{
			++iBrazilJungleCount;
		}
		else if (ePlotFeature == FEATURE_MARSH)
		{
			++iWetlandsCount;
		}
		else if (ePlotFeature == FEATURE_FLOOD_PLAINS)
		{
			++iFloodPlainsCount;
		}

		if (pLoopPlot->IsNaturalWonder())
		{
			++iNaturalWonderCount;
		}
		if (pLoopPlot->isLake())
		{
			++iLakeCount;
		}
		if (pLoopPlot->isHills())
		{
			++iIncaHillsCount;
		}

		ResourceTypes eResource = pLoopPlot->getResourceType(eTeam);
		if(eResource != NO_RESOURCE)
		{
			switch (GC.getResourceInfo(eResource)->getResourceUsage())
			{
			case RESOURCEUSAGE_LUXURY:
				++iResourceLuxuryCount;
				break;
			case RESOURCEUSAGE_STRATEGIC:
				++iResourceStrategicCount;
				break;
			case RESOURCEUSAGE_BONUS:
				++iResourceBonusCount;
				break;
			}
		}

		if (pLoopPlot->getTerrainType() == TERRAIN_DESERT && eResource == NO_RESOURCE)
		{
			++iDesertCount;
		}

		if (pLoopPlot->isMountain() && pPlayer && pPlayer->GetPlayerTraits()->IsMountainPass())
		{
			int iAdjacentMountains = pLoopPlot->GetNumAdjacentMountains();
			//give the bonus if it's hills, with additional if bordered by mountains
			iCivModifier += (iAdjacentMountains+1) * m_iIncaMultiplier;
			if (pDebug) vQualifiersPositive.push_back("(C) incan mountains");
		}
	}

	//take into account only the best 80% of the plots - in the near term the city will not work all plots anyways
	std::stable_sort( workablePlots.begin(), workablePlots.end() );
	size_t iIrrelevantPlots = workablePlots.size()*20/100;
	for (size_t idx=iIrrelevantPlots; idx<workablePlots.size(); idx++)
		iTotalPlotValue += workablePlots[idx].score;

	//hard cutoffs
	if (iTotalPlotValue < 0)
		return 0;
	if (nFoodPlots < 5)
		return 0;
	if (nHammerPlots < 4)
		return 0;
	if (iResourceLuxuryCount < 2 && iResourceStrategicCount < 2 && iResourceBonusCount < 2 && nGoodPlots < 2)
		return 0;

	//civ-specific bonuses
	if (pPlayer)
	{
		if (pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest())
		{
			iCivModifier += iCelticForestCount * m_iCelticMultiplier;
			if (pDebug) vQualifiersPositive.push_back("(C) forest");
		}
		else if (pPlayer->GetPlayerTraits()->GetNaturalWonderYieldModifier() > 0)
		{
			iCivModifier += iNaturalWonderCount * m_iSpainMultiplier;
			if (pDebug) vQualifiersPositive.push_back("(C) natural wonders");
		}

		// Custom code for Brazil
		static ImprovementTypes eBrazilImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_BRAZILWOOD_CAMP", true);  
		if(eBrazilImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eBrazilImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					if (pkEntry->GetFeatureMakesValid(FEATURE_JUNGLE))
						iCivModifier += iBrazilJungleCount * m_iBrazilMultiplier;
					else if (pkEntry->GetFeatureMakesValid(FEATURE_FOREST))
						iCivModifier += iIroquoisForestCount * m_iBrazilMultiplier;
						
					if (pDebug) vQualifiersPositive.push_back("(C) jungle");
				}
			}
		}

		// Custom code for Morocco
		static ImprovementTypes eMoroccoImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_KASBAH", true);
		if(eMoroccoImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eMoroccoImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired() && !pkEntry->IsAdjacentCity())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					iCivModifier += iDesertCount * m_iMorrocoMultiplier;
					if (pDebug) vQualifiersPositive.push_back("(C) desert");
				}
			}
		}

		// Custom code for France
		static ImprovementTypes eFranceImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CHATEAU", true);
		if(eFranceImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eFranceImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired() && pkEntry->IsAdjacentLuxury())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					iCivModifier += iResourceLuxuryCount * m_iFranceMultiplier;
					if (pDebug) vQualifiersPositive.push_back("(C) luxury");
				}
			}
		}

		//Custom code for Netherlands
		static ImprovementTypes ePolderImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_POLDER", true);
		if(ePolderImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(ePolderImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					if(pkEntry->IsFreshWaterMakesValid())
					{
						iCivModifier += iWetlandsCount * m_iNetherlandsMultiplier;
						iCivModifier += iFloodPlainsCount * m_iNetherlandsMultiplier;
					}
					else if(pkEntry->GetFeatureMakesValid(FEATURE_MARSH))
						iCivModifier += iWetlandsCount * m_iNetherlandsMultiplier;

					if(pkEntry->IsAdjacentLake())
						iCivModifier += (iLakeCount * m_iNetherlandsMultiplier);
					if (pDebug) vQualifiersPositive.push_back("(C) wetlands");
				}
			}
		}

		// Custom code for Inca
		static ImprovementTypes eIncaImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_TERRACE_FARM", true);
		if(eIncaImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkEntry = GC.getImprovementInfo(eIncaImprovement);
			if(pkEntry != NULL && pkEntry->IsSpecificCivRequired())
			{
				CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
				if(eCiv == pPlayer->getCivilizationType())
				{
					iCivModifier += (iIncaHillsCount * m_iIncaMultiplier);
					if (pDebug) vQualifiersPositive.push_back("(C) hills");
				}
			}
		}
	}

	// Finally, look at the city plot itself
	if (pPlot->IsNaturalWonder())
	{
		iValueModifier += (iTotalPlotValue * /*-10*/ GD_INT_GET(BUILD_ON_RESOURCE_PERCENT)) / 100;
		if (pDebug) vQualifiersNegative.push_back("(V) city on natural wonder");
	}

	if ( iTotalProductionValue > 4 * iTotalFoodValue )
	{
		iValueModifier -= 20 * iTotalPlotValue / 100;
		if (pDebug) vQualifiersNegative.push_back("(V) unbalanced yields (lacking food)");
	}

	//food has a higher weight than production anyway, so set a high threshold here
	if (iTotalFoodValue > 12 * iTotalProductionValue)
	{
		iValueModifier -= 20 * iTotalPlotValue / 100;
		if (pDebug) vQualifiersNegative.push_back("(V) unbalanced yields (lacking hammers)");
	}

	if (pPlot->isRiver())
	{
		iValueModifier += (iTotalPlotValue * /*35*/ GD_INT_GET(BUILD_ON_RIVER_PERCENT)) / 100;
		if(pPlayer && pPlayer->GetPlayerTraits()->IsRiverTradeRoad())
			iValueModifier += (iTotalPlotValue * /*35*/ GD_INT_GET(BUILD_ON_RIVER_PERCENT)) / 100;
		if (pDebug) vQualifiersPositive.push_back("(V) river");
	}

	if (bIsAlmostCoast)
	{
		iValueModifier -= 20 * iTotalPlotValue / 100;
		if (pDebug) vQualifiersNegative.push_back("(V) almost coast");
	}

	if (pPlot->isCoastalLand(/*10*/ GD_INT_GET(MIN_WATER_SIZE_FOR_OCEAN)))
	{
		if (nWaterPlots <= 3)
		{
			iValueModifier += (iTotalPlotValue * /*40*/ GD_INT_GET(SETTLER_BUILD_ON_COAST_PERCENT)) / 100;
			if (pDebug) vQualifiersPositive.push_back("(V) coast but not too exposed");
		}

		if (pPlayer)
		{
			int iNavalFlavor = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)m_iNavalIndex);
			// we really like the coast (England, Norway, Polynesia, Carthage, etc.)
			if (iNavalFlavor > 7 || pPlayer->getCivilizationInfo().isCoastalCiv())
			{
				iValueModifier += (iTotalPlotValue * /*40*/ GD_INT_GET(SETTLER_BUILD_ON_COAST_PERCENT)) / 100;
			}
		}
	}

	//Is this a chokepoint?
	if(pPlot->IsChokePoint())
	{
		iStratModifier += (iTotalPlotValue * /*10*/ GD_INT_GET(CHOKEPOINT_STRATEGIC_VALUE)) / 100;
		if (pDebug) vQualifiersPositive.push_back("(S) chokepoint");
	}
	//Can we create a canal here?
	if(pPlot->IsWaterAreaSeparator())
	{
		iStratModifier += (iTotalPlotValue * /*10*/ GD_INT_GET(CHOKEPOINT_STRATEGIC_VALUE)) / 100;
		if (pDebug) vQualifiersPositive.push_back("(S) landbridge");
	}

	// AI only (and not for initial city)
	if (pPlayer && !pPlayer->isHuman() && pPlayer->getNumCities()>0)
	{
		//check if this location can be defended (from majors)
		int iOwnCityDistance = pPlayer->GetCityDistancePathLength(pPlot);
		PlayerTypes eNeighbor = NO_PLAYER;

		CvCity* pClosestCity = GC.getGame().GetClosestCityByPathLength(pPlot,true);
		if (pClosestCity && pClosestCity->getOwner() != pPlayer->GetID())
		{
			eNeighbor = pClosestCity->getOwner();
		}
		else if (pPlayer->GetNumCitiesFounded() < 4) //early game
		{
			//if we're still inside our sphere of influence, see if we can make a strategic claim to territory
			DirectionTypes eDirectionFromCapital = directionXY(pPlayer->getCapitalCity()->plot(), pPlot);

			//go two steps outward
			CvPlot* pOutwardPlot = pPlot->getNeighboringPlot(eDirectionFromCapital);
			if (pOutwardPlot)
				pOutwardPlot = pOutwardPlot->getNeighboringPlot(eDirectionFromCapital);

			if (pOutwardPlot)
			{
				CvCity* pClosestCity2 = GC.getGame().GetClosestCityByPathLength(pOutwardPlot, true);
				if (pClosestCity2 && pClosestCity2->getOwner() != pPlayer->GetID())
					eNeighbor = pClosestCity2->getOwner();
			}
		}
		
		//todo: do we want the check the map area as well?
		if (eNeighbor != NO_PLAYER && pPlayer->GetDiplomacyAI()->GetMilitaryStrengthComparedToUs(eNeighbor)!=NO_STRENGTH_VALUE)
		{
			int strengthModifiers[NUM_STRENGTH_VALUES] = { 100, 100, 100, 50, -50, -100, -100 };
			int iModifierPercent = strengthModifiers[pPlayer->GetDiplomacyAI()->GetMilitaryStrengthComparedToUs(eNeighbor)];

			CvPlayer& kNeighbor = GET_PLAYER(eNeighbor);
			int iEnemyDistance = kNeighbor.GetCityDistancePathLength(pPlot);
			//are we getting *very* close to the enemy?
			int iThreshold = min(iOwnCityDistance - 1, iBorderlandRange);
			if (iEnemyDistance < iThreshold)
			{
				//be annoying to our enemies
				if (pPlayer->GetDiplomacyAI()->GetCivApproach(eNeighbor) <= CIV_APPROACH_GUARDED || pPlayer->GetDiplomacyAI()->GetCivOpinion(eNeighbor) <= CIV_OPINION_COMPETITOR)
					iModifierPercent += 50;
			}
			else if (pPlayer->GetNumCitiesFounded() > 3)
			{
				//be nice to our friends, but only once we have our base cities
				if (pPlayer->GetDiplomacyAI()->GetCivApproach(eNeighbor) == CIV_APPROACH_FRIENDLY || pPlayer->GetDiplomacyAI()->GetCivOpinion(eNeighbor) >= CIV_OPINION_FRIEND)
					iModifierPercent -= 50;
			}

			//personality also plays a role
			if (pPlayer->GetDiplomacyAI()->GetBoldness() > 6)
				iModifierPercent += 30;
			if (pPlayer->GetDiplomacyAI()->GetBoldness() < 4)
				iModifierPercent -= 30;

			//finally
			int iAdjustedEffect =  (/*30*/ GD_INT_GET(BALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE) * iModifierPercent) / 100;
			iStratModifier += (iTotalPlotValue * iAdjustedEffect) / 100;

			if (pDebug)
			{
				if (iAdjustedEffect > 0)
					vQualifiersPositive.push_back("(S) landgrab");
				if (iAdjustedEffect < 0)
					vQualifiersNegative.push_back("(S) hard to defend");
			}
		}

		// where is our personal sweet spot?
		// this is handled in plots, not in turns
		int iMinDistance = /*3*/ GD_INT_GET(MIN_CITY_RANGE);
		if(pPlayer->isMinorCiv())
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
			iValueModifier -= (20*iTotalPlotValue) / 100;
			if (pDebug) vQualifiersNegative.push_back("(V) too close to existing friendly city");
		}

		int iSweetMin = /*4*/ GD_INT_GET(SETTLER_DISTANCE_DROPOFF_MODIFIER);
		int iSweetMax = iSweetMin+1;

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
		int iCityDistance = GC.getGame().GetClosestCityDistanceInPlots(pPlot);
		if (iCityDistance >= iSweetMin && iOwnCityDistance <= iSweetMax)
		{
			iValueModifier += (iTotalPlotValue*20)/100; //make this a small bonus, there is a separate distance check anyway
			if (pDebug) vQualifiersPositive.push_back("(V) optimal distance to existing cities");
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
int CvCitySiteEvaluator::PlotFertilityValue(CvPlot* pPlot, bool bIncludeCoast)
{
	int rtnValue = 0;
	if( (!pPlot->isWater() && pPlot->isValidMovePlot(NO_PLAYER)) || (bIncludeCoast && pPlot->isShallowWater() ) )
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

vector<int> CvCitySiteEvaluator::GetAllCitySiteValues(const CvPlayer* pPlayer)
{
	vector<int> vValues;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
		int iValue = PlotFoundValue(pPlot, pPlayer, vector<int>());

		if (iValue > 0)
			vValues.push_back(iValue);
	}

	return vValues;
}

// PROTECTED METHODS (can be overridden in derived classes)

/// Value of plot for providing food
int CvCitySiteEvaluator::ComputeFoodValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	// From tile yield
	int rtnValue = pPlot->calculateNatureYield(YIELD_FOOD, pPlayer ? pPlayer->GetID() : NO_PLAYER, NULL);

	// assume a farm or similar on suitable terrain ... should be build sooner or later. value averages out with other improvements
	if (((pPlot->getTerrainType()==TERRAIN_GRASS || pPlot->getTerrainType()==TERRAIN_PLAINS) && pPlot->getFeatureType() == NO_FEATURE) || pPlot->getFeatureType() == FEATURE_FLOOD_PLAINS)
		rtnValue += 1;

	//Help with island settling - assume a lighthouse
	if(pPlot->isShallowWater())
		rtnValue += 1;

	// From resource
	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;
	ResourceTypes eResource = pPlot->getResourceType(eTeam);
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

/// Value of plot for providing hammers
int CvCitySiteEvaluator::ComputeProductionValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = pPlot->calculateNatureYield(YIELD_PRODUCTION, pPlayer ? pPlayer->GetID() : NO_PLAYER, NULL);

	// assume a mine or similar in friendly climate. don't run off into the snow
	if (pPlot->isHills() && (pPlot->getTerrainType()==TERRAIN_GRASS || pPlot->getTerrainType()==TERRAIN_PLAINS || pPlot->getTerrainType()==TERRAIN_TUNDRA) && pPlot->getFeatureType() == NO_FEATURE)
		rtnValue += 1;

	// From resource
	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;
	ResourceTypes eResource = pPlot->getResourceType(eTeam);
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
	int rtnValue = pPlot->calculateNatureYield(YIELD_GOLD, pPlayer ? pPlayer->GetID() : NO_PLAYER, NULL);

	// From resource
	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;
	ResourceTypes eResource = pPlot->getResourceType(eTeam);
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
	int rtnValue = pPlot->calculateNatureYield(YIELD_SCIENCE, pPlayer ? pPlayer->GetID() : NO_PLAYER, NULL);

	// From resource
	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;
	ResourceTypes eResource = pPlot->getResourceType(eTeam);
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
	int rtnValue = pPlot->calculateNatureYield(YIELD_FAITH, pPlayer ? pPlayer->GetID() : NO_PLAYER, NULL);

	// From resource
	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;
	ResourceTypes eResource = pPlot->getResourceType(eTeam);
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

/// Value of plot for providing Happiness
int CvCitySiteEvaluator::ComputeHappinessValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	// From resource
	TeamTypes eTeam = pPlayer ? pPlayer->getTeam() : NO_TEAM;

	ResourceTypes eResource = pPlot->getResourceType(eTeam);
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

/// Value of plot for providing tradeable resources
int CvCitySiteEvaluator::ComputeTradeableResourceValue(CvPlot* pPlot, const CvPlayer* pPlayer)
{
	int rtnValue = 0;

	// If we already own this Tile then we already have access to the Strategic Resource
	if(!pPlot || pPlot->isOwned())
	{
		return rtnValue;
	}

	TeamTypes eTeam = NO_TEAM;
	if(pPlayer != NULL)
	{
		eTeam = pPlayer->getTeam();
	}

	ResourceTypes eResource = pPlot->getResourceType(eTeam);

	if(eResource != NO_RESOURCE)
	{
		ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eResource)->getResourceUsage();

		// Multiply number of tradeable resources by flavor value
		if(eResourceUsage == RESOURCEUSAGE_LUXURY || eResourceUsage == RESOURCEUSAGE_STRATEGIC)
		{
			rtnValue += pPlot->getNumResource() * m_iFlavorMultiplier[SITE_EVALUATION_RESOURCES];

			//Since there aren't 'multiple' luxuries on a tile, increase this value.
			if(eResourceUsage == RESOURCEUSAGE_LUXURY)
				rtnValue *= 3;

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
int CvCitySiteEvaluator::ComputeStrategicValue(CvPlot* pPlot, int iPlotsFromCity)
{
	int rtnValue = 0;
	if (!pPlot) return rtnValue;

	//Some features and terrain types are useful strategically. (Or really bad)
	if (pPlot->getOwner() == NO_PLAYER)
	{
		if (iPlotsFromCity <= 3 && (pPlot->getFeatureType() == FEATURE_ICE))
		{
			rtnValue += /*-34*/ GD_INT_GET(BALANCE_BAD_TILES_STRATEGIC_VALUE);
		}
		if (iPlotsFromCity <= 3 && pPlot->isFreshWater())
		{
			rtnValue += /*5*/ GD_INT_GET(BALANCE_FRESH_WATER_STRATEGIC_VALUE);
		}
		if (iPlotsFromCity <= 3 && pPlot->isCoastalLand())
		{
			rtnValue += /*3*/ GD_INT_GET(BALANCE_COAST_STRATEGIC_VALUE);
		}
	}

	if (pPlot->isHills())
	{
		// Hills in first ring are useful for defense and production
		if (iPlotsFromCity == 1)
			rtnValue += /*2*/ GD_INT_GET(HILL_STRATEGIC_VALUE);
		else if (iPlotsFromCity == 0)
			rtnValue += /*24*/ GD_INT_GET(HILL_STRATEGIC_VALUE) * 12; //good for defense
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

	return rtnValue * m_iFlavorMultiplier[SITE_EVALUATION_STRATEGIC];
}

//=====================================
// CvSiteEvaluatorForSettler
//=====================================
/// Constructor
CvSiteEvaluatorForSettler::CvSiteEvaluatorForSettler(void)
{
	m_iBrazilMultiplier = 1000; //fertility boost from jungles and/or forest
	m_iSpainMultiplier = 5000; //fertility boost from natural wonders
	m_iCelticMultiplier = 2000; //fertility boost from unimproved forest
	m_iMorrocoMultiplier = 1000; //fertility boost from desert
	m_iFranceMultiplier = 1000; //fertility boost from resources
	m_iNetherlandsMultiplier = 2000; //fertility boost from marshes and/or flood plains
	m_iIncaMultiplier = 100; //fertility boost for hill tiles surrounded my mountains
}

/// Destructor
CvSiteEvaluatorForSettler::~CvSiteEvaluatorForSettler(void)
{
}

/// Overridden - ignore flavors for initial site selection
void CvCitySiteEvaluator::ComputeFlavorMultipliers(const CvPlayer*)
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
int CvCitySiteEvaluator::PlotFoundValue(CvPlot* pPlot, const CvPlayer*, const std::vector<int>&, bool, CvString*)
{
	int rtnValue = 0;

	if(!pPlot) 
		return rtnValue;

	if(!CanFoundCity(pPlot, NULL, false))
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
				rtnValue += iRingModifier * ComputeFoodValue(pLoopPlot, NULL) * /*6*/ GD_INT_GET(START_AREA_FOOD_MULTIPLIER);
				rtnValue += iRingModifier * ComputeHappinessValue(pLoopPlot, NULL) * /*12*/ GD_INT_GET(START_AREA_HAPPINESS_MULTIPLIER);
				rtnValue += iRingModifier * ComputeProductionValue(pLoopPlot, NULL) * /*8*/ GD_INT_GET(START_AREA_PRODUCTION_MULTIPLIER);
				rtnValue += iRingModifier * ComputeGoldValue(pLoopPlot, NULL) * /*2*/ GD_INT_GET(START_AREA_GOLD_MULTIPLIER);
				rtnValue += iRingModifier * ComputeScienceValue(pLoopPlot, NULL) * /*1*/ GD_INT_GET(START_AREA_SCIENCE_MULTIPLIER);
				rtnValue += iRingModifier * ComputeFaithValue(pLoopPlot, NULL) * /*1*/ GD_INT_GET(START_AREA_FAITH_MULTIPLIER);
				rtnValue += iRingModifier * ComputeTradeableResourceValue(pLoopPlot, NULL) * /*1*/ GD_INT_GET(START_AREA_RESOURCE_MULTIPLIER);
				rtnValue += iRingModifier * ComputeStrategicValue(pLoopPlot, iDistance) * /*1*/ GD_INT_GET(START_AREA_STRATEGIC_MULTIPLIER);
			}
		}
	}

	if(rtnValue < 0) 
		rtnValue = 0;

	// Finally, look at the city plot itself and use it as an overall multiplier
	if(pPlot->getResourceType() != NO_RESOURCE)
	{
		rtnValue += rtnValue * /*-10*/ GD_INT_GET(BUILD_ON_RESOURCE_PERCENT) / 100;
	}

	if(pPlot->isRiver())
	{
		rtnValue += rtnValue * /*35*/ GD_INT_GET(BUILD_ON_RIVER_PERCENT) / 100;
	}

	if(pPlot->isCoastalLand(-1,false))
	{
		rtnValue += rtnValue * /*20*/ GD_INT_GET(START_AREA_BUILD_ON_COAST_PERCENT) / 100;
	}

	return rtnValue;
}