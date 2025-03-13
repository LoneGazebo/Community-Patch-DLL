/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvBuilderTaskingAI.h"
#include "CvPlayer.h"
#include "CvTraitClasses.h"
#include "CvAStar.h"
#include "CvImprovementClasses.h"
#include "CvCityConnections.h"
#include "CvEconomicAI.h"
#include "CvDiplomacyAI.h"
#include "CvGameCoreEnumSerialization.h" //toString(const YieldTypes& v)
#include "CvTypes.h"

// include after all other headers
#include "LintFree.h"

/// Constructor
CvBuilderTaskingAI::CvBuilderTaskingAI(void)
{
	Uninit();
}

/// Destructor
CvBuilderTaskingAI::~CvBuilderTaskingAI(void)
{
	Uninit();
}

/// Init
void CvBuilderTaskingAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_eRepairBuild = GetRepairBuild();
	m_eFalloutFeature = GetFalloutFeature();
	m_eFalloutRemove = GetFalloutRemove();
	m_eRemoveRouteBuild = GetRemoveRoute();

	m_bLogging = GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging();

	CivilizationTypes eCiv = m_pPlayer->getCivilizationType();

	for(int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(!pkBuild)
		{
			continue;
		}
		
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
		{
			continue;
		}

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pkImprovementInfo == NULL)
		{
			continue;
		}

		if (pkImprovementInfo->IsSpecificCivRequired())
		{
			CivilizationTypes eRequiredCiv = pkImprovementInfo->GetRequiredCivilization();
			if (eRequiredCiv == eCiv)
			{
				// Store this civ's unique improvements for special placement considerations as necessary
				m_uniqueImprovements.push_back(eImprovement);
			}
		}
	}
}

/// Uninit
void CvBuilderTaskingAI::Uninit(void)
{
	m_pPlayer = NULL;
	m_bLogging = false;

	m_eRepairBuild = NO_BUILD;
	m_eFalloutFeature = NO_FEATURE;
	m_eFalloutRemove = NO_BUILD;

	m_uniqueImprovements.clear();
}

template<typename BuilderTaskingAI, typename Visitor>
void CvBuilderTaskingAI::Serialize(BuilderTaskingAI& builderTaskingAI, Visitor& visitor)
{
	visitor(builderTaskingAI.m_bestRouteTypeAndValue);
	visitor(builderTaskingAI.m_plotRoutePurposes);
	visitor(builderTaskingAI.m_anyRoutePlanned);
	visitor(builderTaskingAI.m_canalWantedPlots);
	visitor(builderTaskingAI.m_uniqueImprovements);
}

/// Serialization read
void CvBuilderTaskingAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvBuilderTaskingAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& loadFrom, RoutePurpose& writeTo)
{
	return ReadBasicEnum<int>(loadFrom, writeTo);
}
FDataStream& operator<<(FDataStream& saveTo, const RoutePurpose& readFrom)
{
	return WriteBasicEnum<int>(saveTo, readFrom);
}

FDataStream& operator>>(FDataStream& stream, CvBuilderTaskingAI& builderTaskingAI)
{
	builderTaskingAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvBuilderTaskingAI& builderTaskingAI)
{
	builderTaskingAI.Write(stream);
	return stream;
}

/// Update
void CvBuilderTaskingAI::Update(void)
{
	SetupExtraXAdjacentPlots();
	UpdateRoutePlots();
	UpdateCanalPlots();
	UpdateImprovementPlots();

	if(m_bLogging)
	{
		bool bShowOutput = m_pPlayer->isHuman();

		if(m_pPlayer->IsEmpireUnhappy())
		{
			CvString str = "// Empire Unhappy! //";
			LogInfo(str, m_pPlayer, bShowOutput);
		}

		// show crisis states
		int iLoop = 0;
		for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			CvString str("// ");
			str += pLoopCity->getName();
			str += " // missing ";

			YieldTypes eMissing = pLoopCity->GetCityStrategyAI()->GetMostDeficientYield();
			switch(eMissing)
			{
			case YIELD_FOOD:
				str += "food";
				break;
			case YIELD_PRODUCTION:
				str += "production";
				break;
			case YIELD_SCIENCE:
				str += "science";
				break;
			case YIELD_GOLD:
				str += "gold";
				break;
			case YIELD_CULTURE:
				str += "culture";
				break;
			case YIELD_FAITH:
				str += "faith";
				break;
			default:
				str += "unknown";
			}

			LogInfo(str, m_pPlayer, bShowOutput);
		}
	}
}

//	---------------------------------------------------------------------------
int CvBuilderTaskingAI::GetMoveCostWithRoute(const CvPlot* pFromPlot, const CvPlot* pToPlot, RouteTypes eFromPlotRoute, RouteTypes eToPlotRoute)
{
	if (plotDistance(pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY()) != 1)
		return 0;

	if (!pFromPlot || !pToPlot)
		return 0;

	int iMoveDenominator = GD_INT_GET(MOVE_DENOMINATOR);
	int iRegularCost = 1;

	CvPlayerTraits* pTraits = m_pPlayer->GetPlayerTraits();

	TeamTypes eTeam = m_pPlayer->getTeam();
	CvTeam& kTeam = GET_TEAM(eTeam);

	//try to avoid calling directionXY too often
	bool bRiverCrossing = pFromPlot->getRiverCrossingCount() > 0 && pToPlot->getRiverCrossingCount() > 0 && pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot));
	FeatureTypes eToFeature = pToPlot->getFeatureType();
	CvFeatureInfo* pToFeatureInfo = (eToFeature > NO_FEATURE) ? GC.getFeatureInfo(eToFeature) : NULL;
	TerrainTypes eToTerrain = pToPlot->getTerrainType();
	CvTerrainInfo* pToTerrainInfo = (eToTerrain > NO_TERRAIN) ? GC.getTerrainInfo(eToTerrain) : NULL;
	FeatureTypes eFromFeature = pFromPlot->getFeatureType();

	//route preparation
	bool bRouteTo = eToPlotRoute != NO_ROUTE;
	bool bRouteFrom = eFromPlotRoute != NO_ROUTE;

	bool bMovingAlongRiver = pFromPlot->IsAlongSameRiver(pToPlot);
	bool bFakeRouteTo = (pTraits->IsRiverMovementBonus() && bMovingAlongRiver);
	bool bFakeRouteFrom = (pTraits->IsRiverMovementBonus() && bMovingAlongRiver);

	if (!MOD_SANE_UNIT_MOVEMENT_COST)
	{
		//balance patch does not require plot ownership
		bFakeRouteTo |= (pTraits->IsWoodlandMovementBonus() && (eToFeature == FEATURE_FOREST || eToFeature == FEATURE_JUNGLE) && (MOD_BALANCE_VP || pToPlot->getTeam() == eTeam));
		bFakeRouteFrom |= (pTraits->IsWoodlandMovementBonus() && (eFromFeature == FEATURE_FOREST || eFromFeature == FEATURE_JUNGLE) && (MOD_BALANCE_VP || pToPlot->getTeam() == eTeam));
	}

	//check routes
	if ((bRouteFrom || bFakeRouteFrom) &&
		(bRouteTo || bFakeRouteTo) &&
		(!bRiverCrossing || kTeam.isBridgeBuilding()))
	{
		RouteTypes eFromRouteWithFake = bFakeRouteFrom && eFromPlotRoute < ROUTE_ROAD ? ROUTE_ROAD : eFromPlotRoute;
		CvRouteInfo* pFromRouteInfo = GC.getRouteInfo(eFromRouteWithFake);
		int iFromMovementCost = pFromRouteInfo ? pFromRouteInfo->getMovementCost() : 0;

		RouteTypes eToRouteWithFake = bFakeRouteTo && eToPlotRoute < ROUTE_ROAD ? ROUTE_ROAD : eToPlotRoute;
		CvRouteInfo* pToRouteInfo = GC.getRouteInfo(eToRouteWithFake);
		int iToMovementCost = pToRouteInfo ? pToRouteInfo->getMovementCost() : 0;

		int iRouteCost = std::max(iFromMovementCost + kTeam.getRouteChange(eFromRouteWithFake), iToMovementCost + kTeam.getRouteChange(eToRouteWithFake));

		return iRouteCost;

	}

	if (pToPlot->isCity() && (!bRiverCrossing || kTeam.isBridgeBuilding()))
	{
		return iMoveDenominator;
	}
	else //normal case, check terrain and features
	{
		//global ignore terrain cost also ignores feature cost, but units may get bonuses from terrain!
		//difference to flat movement cost is that flat movement units don't get any bonuses
		bool bIgnoreCostsHere = false;

		//in some cases, we ignore terrain and feature costs if the destination tile contains a specific terrain/feature
		if (pTraits->IsFasterInHills() && pToPlot->isHills())
			bIgnoreCostsHere = true;
		else if (pTraits->IsMountainPass() && pToPlot->isMountain())
			bIgnoreCostsHere = true;

		if (bIgnoreCostsHere) // Incan UA bypasses the check for river crossings, so no need to check those defines
			iRegularCost = 1;
		else
		{
			iRegularCost = ((eToFeature == NO_FEATURE) ? (pToTerrainInfo ? pToTerrainInfo->getMovementCost() : 0) : (pToFeatureInfo ? pToFeatureInfo->getMovementCost() : 0));

			// Hill cost is hardcoded
			if (pToPlot->isHills() || pToPlot->isMountain())
			{
				iRegularCost += /*1*/ GD_INT_GET(HILLS_EXTRA_MOVEMENT);
			}

			if (bRiverCrossing)
			{
				iRegularCost += /*10*/ GD_INT_GET(RIVER_EXTRA_MOVEMENT);
			}
		}

		return iRegularCost * iMoveDenominator;
	}
}

static int GetYieldBaseModifierTimes100(YieldTypes eYield)
{
	switch (eYield)
	{
	case NO_YIELD:
		UNREACHABLE();
	case YIELD_FOOD:
		return /*180*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FOOD);
	case YIELD_PRODUCTION:
		return /*180*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_PRODUCTION);
	case YIELD_GOLD:
		return /*90*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_GOLD);
	case YIELD_SCIENCE:
		return /*240*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_SCIENCE);
	case YIELD_CULTURE:
		return /*240*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_CULTURE);
	case YIELD_FAITH:
		return /*240*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_FAITH);
	case YIELD_TOURISM:
		return /*240*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_TOURISM);
	case YIELD_CULTURE_LOCAL:
		return /*80*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_CULTURE_LOCAL);
	case YIELD_GOLDEN_AGE_POINTS:
		return /*200*/ GD_INT_GET(BUILDER_TASKING_BASELINE_ADDS_GOLDEN_AGE_POINTS);
	case YIELD_GREAT_GENERAL_POINTS:
	case YIELD_GREAT_ADMIRAL_POINTS:
	case YIELD_POPULATION:
	case YIELD_JFD_HEALTH:
	case YIELD_JFD_DISEASE:
	case YIELD_JFD_CRIME:
	case YIELD_JFD_LOYALTY:
	case YIELD_JFD_SOVEREIGNTY:
		return 20;
	default:
		return 0;
	}
}

// Only include puppet calculations for now, most other things are too dynamic
static int GetYieldCityModifierTimes100(const CvCity* pCity, const CvPlayer* pPlayer, YieldTypes eYield)
{
	int iTempMod = 0;
	int iModifier = iTempMod;
	
	if (pCity->IsPuppet())
	{
		switch (eYield)
		{
		case YIELD_SCIENCE:
			iTempMod = pPlayer->GetPuppetYieldPenaltyMod() + pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + /*-25 in CP, -80 in VP*/ GD_INT_GET(PUPPET_SCIENCE_MODIFIER);
			if (iTempMod > 0)
				iTempMod = 0;
			iModifier += iTempMod;
			break;

		case YIELD_GOLD:
			iTempMod = pPlayer->GetPuppetYieldPenaltyMod() + pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + /*0 in CP, -80 in VP*/ GD_INT_GET(PUPPET_GOLD_MODIFIER);
			if (iTempMod > 0)
				iTempMod = 0;
			iModifier += iTempMod;
			break;

		case YIELD_PRODUCTION:
			iTempMod = pPlayer->GetPuppetYieldPenaltyMod() + pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + /*0*/ GD_INT_GET(PUPPET_PRODUCTION_MODIFIER);
			if (iTempMod > 0)
				iTempMod = 0;
			iModifier += iTempMod;
			break;

		case YIELD_TOURISM:
			iTempMod = pPlayer->GetPuppetYieldPenaltyMod() + pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + /*0 in CP, -80 in VP*/ GD_INT_GET(PUPPET_TOURISM_MODIFIER);
			if (iTempMod > 0)
				iTempMod = 0;
			iModifier += iTempMod;
			break;

		case YIELD_GOLDEN_AGE_POINTS:
			iTempMod = pPlayer->GetPuppetYieldPenaltyMod() + pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + /*0 in CP, -80 in VP*/ GD_INT_GET(PUPPET_GOLDEN_AGE_MODIFIER);
			if (iTempMod > 0)
				iTempMod = 0;
			iModifier += iTempMod;
			break;

		case YIELD_CULTURE: // taken from getJONSCulturePerTurn
			iTempMod = pPlayer->GetPuppetYieldPenaltyMod() + pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + /*-25 in CP, -80 in VP*/ GD_INT_GET(PUPPET_CULTURE_MODIFIER);
			if (pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() != 0 && iTempMod > 0)
				iTempMod = 0;
			iModifier += iTempMod;
			break;

		case YIELD_FAITH:
			iTempMod = pPlayer->GetPuppetYieldPenaltyMod() + pPlayer->GetPlayerTraits()->GetPuppetPenaltyReduction() + GD_INT_GET(PUPPET_FAITH_MODIFIER);
			if (iTempMod > 0)
				iTempMod = 0;
			iModifier += iTempMod;
			break;

		case NO_YIELD:
		case YIELD_FOOD:
		case YIELD_GREAT_GENERAL_POINTS:
		case YIELD_GREAT_ADMIRAL_POINTS:
		case YIELD_POPULATION:
		case YIELD_CULTURE_LOCAL:
		case YIELD_JFD_HEALTH:
		case YIELD_JFD_DISEASE:
		case YIELD_JFD_CRIME:
		case YIELD_JFD_LOYALTY:
		case YIELD_JFD_SOVEREIGNTY:
			break; // Yield unaffected by being a puppet.
		}
	}

	return std::max(0, (iModifier + 100));
}

void CvBuilderTaskingAI::GetPathValues(const SPath& path, RouteTypes eRoute, int& iVillageBonusesIfCityConnected, int& iMovementBonus, int& iNumRoadsNeededToBuild)
{
	vector<int> aiMovingForwardCostsWithRoute(path.length() - 1);
	vector<int> aiMovingBackwardCostsWithRoute(path.length() - 1);
	vector<int> aiMovingForwardCostsWithoutRoute(path.length() - 1);
	vector<int> aiMovingBackwardCostsWithoutRoute(path.length() - 1);

	for (int i = 1; i < path.length(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if (!pPlot)
			break;

		CvPlot* pPreviousPlot = path.get(i - 1);
		// Calculate move speed bonus we will get for connecting a pair of plots
		if (pPreviousPlot)
		{
			aiMovingForwardCostsWithRoute[i - 1] = GetMoveCostWithRoute(pPreviousPlot, pPlot, eRoute, eRoute);
			aiMovingBackwardCostsWithRoute[i - 1] += GetMoveCostWithRoute(pPlot, pPreviousPlot, eRoute, eRoute);
			aiMovingForwardCostsWithoutRoute[i - 1] = GetMoveCostWithRoute(pPreviousPlot, pPlot, NO_ROUTE, NO_ROUTE);
			aiMovingBackwardCostsWithoutRoute[i - 1] += GetMoveCostWithRoute(pPlot, pPreviousPlot, NO_ROUTE, NO_ROUTE);
		}

		//don't count the cities themselves
		if (pPlot->isCity())
			continue;

		// Bonus if there is a village on the plot
		if (pPlot->getOwner() == m_pPlayer->GetID()
			&& pPlot->getEffectiveOwningCity()->IsWithinWorkRange(pPlot))
		{
			ImprovementTypes ePlotImprovement = pPlot->getImprovementType();
			if (ePlotImprovement != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkPlotImprovementInfo = GC.getImprovementInfo(ePlotImprovement);
				for (int iJ = 0; iJ <= YIELD_FAITH; iJ++)
				{
					// Use base modifier to avoid heavy computations
					int iVillageYieldBonus = pkPlotImprovementInfo->GetRouteYieldChanges(eRoute, iJ) * GetYieldBaseModifierTimes100((YieldTypes)iJ);

					if (iVillageYieldBonus != 0)
					{
						iVillageBonusesIfCityConnected += iVillageYieldBonus;
					}
				}
			}
		}

		// Bonus if a village can be built on the plot
		if (!WillNeverBuildVillageOnPlot(pPlot, eRoute, true/*bIgnoreUnowned*/))
		{
			for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
			{
				BuildTypes eBuild = (BuildTypes)iJ;
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);

				if (!pkBuildInfo)
					continue;

				ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
				if (eImprovement == NO_IMPROVEMENT)
					continue;

				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
				if (pkImprovementInfo->IsCreatedByGreatPerson())
					continue;

				if (pkImprovementInfo->IsNoTwoAdjacent())
				{
					CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pPlot);
					for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot* pAdjacentPlot = aPlotsToCheck[iI];

						if (pAdjacentPlot)
						{
							ImprovementTypes eAdjacentImprovement = pAdjacentPlot->getImprovementType();
							if (eImprovement == eAdjacentImprovement)
								continue;
						}
					}
				}

				int iVillageYieldBonus = 0;
				for (int iJ = 0; iJ <= YIELD_FAITH; iJ++)
				{
					// Use base modifier to avoid heavy computations
					iVillageYieldBonus += pkImprovementInfo->GetRouteYieldChanges(eRoute, iJ) * GetYieldBaseModifierTimes100((YieldTypes)iJ);
				}

				if (iVillageYieldBonus != 0 && m_pPlayer->canBuild(pPlot, eBuild))
				{
					// Assume we can build a village in 1/2 of tiles
					iVillageBonusesIfCityConnected += iVillageYieldBonus / 2;
				}
			}
		}

		if (!m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eRoute))
		{
			if (pPlot->getRouteType() != eRoute || pPlot->IsRoutePillaged())
				iNumRoadsNeededToBuild++;
		}
	}

	const int iMoveDenominator = GD_INT_GET(MOVE_DENOMINATOR);
	int iMovementUsedForwardsWithRoute = 0;
	int iMovementUsedBackwardsWithRoute = 0;
	int iMovementUsedForwardsWithoutRoute = 0;
	int iMovementUsedBackwardsWithoutRoute = 0;
	int iNumSteps = aiMovingForwardCostsWithRoute.size();

	for (int iI = 0; iI < iNumSteps; iI++)
	{
		// Assume unit with 2 movement points
		iMovementUsedForwardsWithRoute += min(iMoveDenominator * 2, aiMovingForwardCostsWithRoute[iI]);
		iMovementUsedForwardsWithoutRoute += min(iMoveDenominator * 2, aiMovingForwardCostsWithoutRoute[iI]);
		iMovementUsedBackwardsWithRoute += min(iMoveDenominator * 2, aiMovingBackwardCostsWithRoute[iI]);
		iMovementUsedBackwardsWithoutRoute += min(iMoveDenominator * 2, aiMovingBackwardCostsWithoutRoute[iI]);
	}

	iMovementBonus = ((iMovementUsedForwardsWithoutRoute + iMovementUsedBackwardsWithoutRoute + 1) / (iMovementUsedForwardsWithRoute + iMovementUsedBackwardsWithRoute + 1)) * 300;
}

pair<int, int> CvBuilderTaskingAI::GetPlotPair(int iPlotId1, int iPlotId2)
{
	if (iPlotId1 < iPlotId2)
		return make_pair(iPlotId1, iPlotId2);
	else
		return make_pair(iPlotId2, iPlotId1);
}

void CvBuilderTaskingAI::AddRoutePlots(CvPlot* pStartPlot, CvPlot* pTargetPlot, RouteTypes eRoute, int iValue, const SPath& path, RoutePurpose ePurpose, bool bUseRivers)
{
	vector<int> routePlots;

	for (int i = 1; i < path.length() - 1; i++)
	{
		CvPlot* pPlot = path.get(i);

		if (!pPlot)
			return;

		m_anyRoutePlanned.insert(make_pair(make_pair(eRoute, pPlot->GetPlotIndex()), ePurpose));
	}

	if (iValue <= 0)
		return;

	if (ePurpose != PURPOSE_CONNECT_CAPITAL)
	{
		for (int i = 1; i < path.length() - 1; i++)
		{
			CvPlot* pPlot = path.get(i);
			if (pPlot->isCity() && pPlot->getOwner() == m_pPlayer->GetID())
				return;
		}
	}

	PlotPair plotPair;
	PlannedRoute plannedRoute;

	for (int i = 1; i < path.length() - 1; i++)
	{
		CvPlot* pPlot = path.get(i);

		if (pPlot->isCity())
		{
			if (pPlot->getOwner() == m_pPlayer->GetID())
			{
				if (routePlots.size() != 0)
				{
					plotPair = GetPlotPair(pStartPlot->GetPlotIndex(), pPlot->GetPlotIndex());
					plannedRoute = make_pair(plotPair, make_pair(eRoute, bUseRivers));

					if (ePurpose == PURPOSE_CONNECT_CAPITAL)
						m_plannedRouteAdditiveValues[plannedRoute] += iValue;
					else
						m_plannedRouteNonAdditiveValues[plannedRoute] = max(m_plannedRouteNonAdditiveValues[plannedRoute], iValue);
					m_plannedRoutePurposes[plannedRoute] = (RoutePurpose)(m_plannedRoutePurposes[plannedRoute] | ePurpose);
					m_plannedRoutePlots[plannedRoute] = vector<int>(routePlots);
				}

				pStartPlot = pPlot;
				routePlots.clear();
			}

			continue;
		}

		routePlots.push_back(pPlot->GetPlotIndex());
	}

	if (routePlots.size() != 0)
	{
		plotPair = GetPlotPair(pStartPlot->GetPlotIndex(), pTargetPlot->GetPlotIndex());
		plannedRoute = make_pair(plotPair, make_pair(eRoute, bUseRivers));

		if (ePurpose == PURPOSE_CONNECT_CAPITAL)
			m_plannedRouteAdditiveValues[plannedRoute] += iValue;
		else
			m_plannedRouteNonAdditiveValues[plannedRoute] = max(m_plannedRouteNonAdditiveValues[plannedRoute], iValue);
		m_plannedRoutePurposes[plannedRoute] = (RoutePurpose)(m_plannedRoutePurposes[plannedRoute] | ePurpose);
		m_plannedRoutePlots[plannedRoute] = routePlots;
	}
}

static int GetPotentialHappinessFromConnection(CvPlayer* pPlayer, CvCity* pCity)
{
	if (pCity->IsRazing() || pCity->IsResistance() || pCity->IsPuppet() || pPlayer->GetPlayerTraits()->IsNoConnectionUnhappiness() || pPlayer->IsNoUnhappyIsolation())
	{
		return 0;
	}

	if (MOD_BALANCE_CORE_UNCAPPED_UNHAPPINESS)
		return pCity->GetUnhappinessFromIsolation();

	int iUnhappiness = 0;
	int iPopulation = pCity->getPopulation();

	// Some of these calls are expensive, so avoid doing them if we've already reached the cap
	// Order is also nonstandard to save on performance
	int iSource = pCity->GetUnhappinessFromOccupation();
	if (iSource > 0)
		iUnhappiness += iSource;

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetUnhappinessFromEmpire();
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetUnhappinessFromReligiousUnrest();
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetUnhappinessFromPillagedTiles();
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetPoverty(false);
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetIlliteracy(false);
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetBoredom(false);
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetUnhappinessFromFamine();
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness < iPopulation)
	{
		iSource = pCity->GetDistress(false);
		if (iSource > 0)
			iUnhappiness += iSource;
	}

	if (iUnhappiness >= iPopulation)
		return 0;

	float fUnhappiness = 0.00f;
	if (GD_FLOAT_GET(UNHAPPINESS_PER_ISOLATED_POP) > 0)
	{
		fUnhappiness += (float)pCity->getPopulation() * /*0.34f*/ GD_FLOAT_GET(UNHAPPINESS_PER_ISOLATED_POP);
	}
	int iLimit = MOD_BALANCE_CORE_UNCAPPED_UNHAPPINESS ? INT_MAX : pCity->getPopulation();

	int iPotentialUnhappinessFromIsolation = range((int)fUnhappiness, 0, iLimit);

	if (iUnhappiness + iPotentialUnhappinessFromIsolation > iPopulation)
		return iPopulation - iUnhappiness;

	return iPotentialUnhappinessFromIsolation;
}

static int GetCapitalConnectionValue(CvPlayer* pPlayer, CvCity* pCity, RouteTypes eRoute)
{
	// Compute bonus for connecting city (or keeping connected)
	int iConnectionValue = GetPotentialHappinessFromConnection(pPlayer, pCity) * /*750*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE);

	int iCityConnectionFlatValueTimes100 = pPlayer->GetTreasury()->GetCityConnectionRouteGoldTimes100(pCity);
	iConnectionValue += iCityConnectionFlatValueTimes100;
	int iGoldYieldBaseModifierTimes100 = GetYieldBaseModifierTimes100(YIELD_GOLD);

	for (int iI = 0; iI <= NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		if (eYield > YIELD_CULTURE_LOCAL)
			break;

		int iEra = pPlayer->GetCurrentEra();
		if (iEra <= 0)
			iEra = 1;
		int iYieldModifier = GetYieldBaseModifierTimes100(eYield);
		iConnectionValue += (100 * pPlayer->GetYieldChangeTradeRoute(eYield) * iYieldModifier) / iGoldYieldBaseModifierTimes100;
		iConnectionValue += (100 * pPlayer->GetPlayerTraits()->GetYieldChangeTradeRoute(eYield) * iEra * iYieldModifier) / iGoldYieldBaseModifierTimes100;
	}

	if (GC.getGame().GetIndustrialRoute() == eRoute)
	{
		int iProductionYieldBaseModifierTimes100 = GetYieldBaseModifierTimes100(YIELD_PRODUCTION);
		int iProductionYieldTimes100 = pCity->getBasicYieldRateTimes100(YIELD_PRODUCTION);
		int iProductionYieldRateModifierTimes100 = /*25 in CP, 0 in VP*/ GD_INT_GET(INDUSTRIAL_ROUTE_PRODUCTION_MOD);

		iConnectionValue += (iProductionYieldTimes100 * iProductionYieldRateModifierTimes100 * iProductionYieldBaseModifierTimes100) / 10000;

		if (MOD_BALANCE_VP)
		{
			int iGoldYieldTimes100 = pCity->getBasicYieldRateTimes100(YIELD_GOLD);
			int iGoldYieldRateModifierTimes100 = 0;

			// Target city would get a flat production bonus from the Industrial City Connection
			iConnectionValue += (iCityConnectionFlatValueTimes100 * iProductionYieldBaseModifierTimes100) / iGoldYieldBaseModifierTimes100;

			// Target city would get a production and gold boost from a coaling station.
			for (int iBuildingIndex = 0; iBuildingIndex < GC.getNumBuildingInfos(); iBuildingIndex++)
			{
				BuildingTypes eBuilding = (BuildingTypes)iBuildingIndex;
				CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);

				if (pkBuilding == NULL)
					continue;

				if (!pkBuilding->IsRequiresIndustrialCityConnection())
					continue;

				if (!pPlayer->HasTech((TechTypes)pkBuilding->GetPrereqAndTech()))
					continue;

				iProductionYieldRateModifierTimes100 = pkBuilding->GetYieldModifier(YIELD_PRODUCTION);
				iGoldYieldRateModifierTimes100 = pkBuilding->GetYieldModifier(YIELD_GOLD);

				// Assumes current modifiers are 0%, should do some proper calculations that account for bonuses being additive rather than multiplicative
				iConnectionValue += (iProductionYieldTimes100 * iProductionYieldRateModifierTimes100 * iProductionYieldBaseModifierTimes100) / 10000;
				iConnectionValue += (iGoldYieldTimes100 * iGoldYieldRateModifierTimes100 * iGoldYieldBaseModifierTimes100) / 10000;
			}
		}
	}

	return iConnectionValue;
}

void CvBuilderTaskingAI::ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, BuildTypes eBuild, RouteTypes eRoute)
{
	if(pTargetCity->IsRazing())
	{
		return;
	}

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if(!pRouteInfo)
	{
		return;
	}

	bool bUseRivers = eRoute == ROUTE_ROAD;

	// build a path between the two cities using harbors (shortcut routes are handled in the shortcut function)
	SPathFinderUserData data(m_pPlayer->GetID(), PT_BUILD_ROUTE_MIXED, eBuild, eRoute, PURPOSE_CONNECT_CAPITAL, bUseRivers);
	SPath path = GC.GetStepFinder().GetPath(pPlayerCapital->getX(), pPlayerCapital->getY(), pTargetCity->getX(), pTargetCity->getY(), data);

	if(!path)
	{
		return;
	}

	int iNumRoadsNeededToBuild = 0;

	for (int i = 0; i < path.length(); i++)
	{
		CvPlot* pPlot = path.get(i);
		if (!pPlot)
			break;

		//don't count the cities themselves
		if (pPlot->isCity())
			continue;

		if (!m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eRoute))
		{
			if (pPlot->getRouteType() < eRoute || pPlot->IsRoutePillaged())
				iNumRoadsNeededToBuild++;
		}
	}

	// for quests we might be targeting a city state ...
	bool bTargetingMinor = GET_TEAM(pTargetCity->getTeam()).isMinorCiv();

	//see if the new route makes sense economically
	int iConnectionValue = 0;
	if(bTargetingMinor)
	{
		//this is for a quest ... normal considerations don't apply
		iConnectionValue = /*1000*/ GD_INT_GET(MINOR_CIV_ROUTE_QUEST_WEIGHT);
	}
	else
	{
		// Compute bonus for connecting city (or keeping connected)
		iConnectionValue += GetCapitalConnectionValue(m_pPlayer, pTargetCity, eRoute);
	}

	iConnectionValue -= iNumRoadsNeededToBuild * 10;

	AddRoutePlots(pPlayerCapital->plot(), pTargetCity->plot(), eRoute, iConnectionValue, path, PURPOSE_CONNECT_CAPITAL, bUseRivers);
}

void CvBuilderTaskingAI::ConnectCitiesForShortcuts(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if(pCity1->getOwner() != pCity2->getOwner())
		return;

	// don't connect razing cities
	if (pCity1->IsRazing() || pCity2->IsRazing())
		return;

	int iPlotDistance = plotDistance(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY());

	if (iPlotDistance >= 10)
		return;

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if (!pRouteInfo)
		return;

	ShortcutConnectionHelper(pCity1, pCity2, eBuild, eRoute, iPlotDistance, false);
	if (MOD_RIVER_CITY_CONNECTIONS && eRoute == ROUTE_ROAD)
		ShortcutConnectionHelper(pCity1, pCity2, eBuild, eRoute, iPlotDistance, true);
}

void CvBuilderTaskingAI::ShortcutConnectionHelper(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute, int iPlotDistance, bool bUseRivers)
{

	// build a path between the two cities - this will tend to re-use existing routes, unless the new path is much shorter
	SPathFinderUserData data(m_pPlayer->GetID(), PT_BUILD_ROUTE, eBuild, eRoute, PURPOSE_SHORTCUT, bUseRivers);
	data.iMaxTurns = iPlotDistance * 2;
	SPath path = GC.GetStepFinder().GetPath(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY(), data);

	// cities are not on the same landmass? then give up
	if (!path)
		return;

	// go through the route to see how long it is and how many plots already have roads
	int iNumRoadsNeededToBuild = 0;

	int iVillageBonusesIfCityConnected = 0;
	int iMovementBonus = 0;

	GetPathValues(path, eRoute, iVillageBonusesIfCityConnected, iMovementBonus, iNumRoadsNeededToBuild);

	// If one of the cities is connected to the capital, both will be when this route is completed
	if (!pCity1->IsConnectedToCapital() && !pCity2->IsConnectedToCapital())
		iVillageBonusesIfCityConnected = 0;

	int iValue = iVillageBonusesIfCityConnected + iMovementBonus;

	// Add a small part of the capital connection bonus, it's good to have some redundancy
	if (pCity1->IsConnectedToCapital() || pCity2->IsConnectedToCapital())
	{
		if (!pCity1->isCapital())
			iValue += GetCapitalConnectionValue(m_pPlayer, pCity1, eRoute) / 20;
		if (!pCity2->isCapital())
			iValue += GetCapitalConnectionValue(m_pPlayer, pCity2, eRoute) / 20;
	}

	iValue -= iNumRoadsNeededToBuild * 10;

	AddRoutePlots(pCity1->plot(), pCity2->plot(), eRoute, iValue, path, PURPOSE_SHORTCUT, bUseRivers);
}

void CvBuilderTaskingAI::ConnectPointsForStrategy(CvCity* pOriginCity, CvPlot* pTargetPlot, BuildTypes eBuild, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if (pOriginCity->getOwner() != pTargetPlot->getOwner())
		return;

	// don't connect razing cities
	if (pOriginCity->IsRazing())
		return;

	int iPlotDistance = plotDistance(pOriginCity->getX(), pOriginCity->getY(), pTargetPlot->getX(), pTargetPlot->getY());

	if (iPlotDistance >= 6)
		return;

	int iDefensiveValue = pTargetPlot->GetStrategicValue(m_pPlayer->GetID());
	if (iDefensiveValue < 50)
		return;

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if (!pRouteInfo)
		return;

	// build a path between the two cities
	SPathFinderUserData data(m_pPlayer->GetID(), PT_BUILD_ROUTE, eBuild, eRoute, PURPOSE_STRATEGIC, false);
	data.iMaxTurns = iPlotDistance * 2;
	SPath path = GC.GetStepFinder().GetPath(pOriginCity->getX(), pOriginCity->getY(), pTargetPlot->getX(), pTargetPlot->getY(), data);

	//  if no path, then bail!
	if (!path)
		return;

	// go through the route to see how long it is and how many plots already have roads
	int iNumRoadsNeededToBuild = 0;

	int iMovementBonus = 0;

	int iDummy = 0;

	GetPathValues(path, eRoute, iDummy, iMovementBonus, iNumRoadsNeededToBuild);

	int iValue = iMovementBonus;

	iValue = (iValue * iDefensiveValue) / 100;

	iValue -= iNumRoadsNeededToBuild * 10;

	AddRoutePlots(pOriginCity->plot(), pTargetPlot, eRoute, iValue, path, PURPOSE_STRATEGIC, false);
}

void CvBuilderTaskingAI::ConnectCitiesForScenario(CvCity* pCity1, CvCity* pCity2, BuildTypes eBuild, RouteTypes eRoute)
{
	// don't connect cities from different owners
	if (pCity1->getOwner() != pCity2->getOwner())
		return;

	// if we already have a connection, bail out
	if (m_pPlayer->IsCityConnectedToCity(pCity1, pCity2, eRoute, true))
		return;

	// build a path between the two cities
	SPathFinderUserData data(m_pPlayer->GetID(), PT_BUILD_ROUTE, eBuild, eRoute, PURPOSE_SHORTCUT, false);
	SPath path = GC.GetStepFinder().GetPath(pCity1->getX(), pCity1->getY(), pCity2->getX(), pCity2->getY(), data);

	//  if no path, then bail!
	if (!path)
		return;

	AddRoutePlots(pCity1->plot(), pCity2->plot(), eRoute, 100, path, PURPOSE_SHORTCUT, false);
}

pair<RouteTypes, int> CvBuilderTaskingAI::GetBestRouteTypeAndValue(const CvPlot* pPlot) const
{
	map<int, pair<RouteTypes, int>>::const_iterator it = m_bestRouteTypeAndValue.find(pPlot->GetPlotIndex());

	if (it != m_bestRouteTypeAndValue.end())
		return it->second;

	return make_pair(NO_ROUTE, -1);
}

bool CvBuilderTaskingAI::WantCanalAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	//do not check IsWaterAreaSeparator() but wait until there are cities on both sides
	set<int>::const_iterator it = m_canalWantedPlots.find(pPlot->GetPlotIndex());
	return (it != m_canalWantedPlots.end());
}

bool CvBuilderTaskingAI::IsPlannedRouteForPurpose(const CvPlot* pPlot, RoutePurpose ePurpose) const
{
	if (!pPlot)
		return false;

	map<int, RoutePurpose>::const_iterator it = m_plotRoutePurposes.find(pPlot->GetPlotIndex());

	if (it == m_plotRoutePurposes.end())
		return false;

	if (!(it->second & ePurpose))
		return false;

	return true;
}

bool CvBuilderTaskingAI::IsMainRoutePlot(const CvPlot* pPlot) const
{
	return IsPlannedRouteForPurpose(pPlot, PURPOSE_CONNECT_CAPITAL);
}

bool CvBuilderTaskingAI::IsShortcutRoutePlot(const CvPlot* pPlot) const
{
	return IsPlannedRouteForPurpose(pPlot, PURPOSE_SHORTCUT);
}

bool CvBuilderTaskingAI::IsStrategicRoutePlot(const CvPlot* pPlot) const
{
	return IsPlannedRouteForPurpose(pPlot, PURPOSE_STRATEGIC);
}

bool CvBuilderTaskingAI::IsRoutePlanned(CvPlot* pPlot, RouteTypes eRoute, RoutePurpose ePurpose) const
{
	if (pPlot->GetPlannedRouteState(m_pPlayer->GetID()) >= ROAD_PLANNING_INCLUDE)
		return true;

	return m_anyRoutePlanned.find(make_pair(make_pair(eRoute, pPlot->GetPlotIndex()), ePurpose)) != m_anyRoutePlanned.end();
}

int CvBuilderTaskingAI::GetRouteBuildTime(PlannedRoute plannedRoute, const CvUnit* pUnit) const
{
	RouteTypes eRoute = plannedRoute.second.first;
	
	map<PlannedRoute, vector<int>>::const_iterator it = m_plannedRoutePlots.find(plannedRoute);
	if (it == m_plannedRoutePlots.end())
		return -1;

	BuildTypes eBuild = GetBuildRoute(eRoute);

	int iTotalBuildTime = 0;
	for (vector<int>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(*it2);

		if (!pPlot)
			continue;

		if (m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eRoute))
			continue;

		if (pPlot->getRouteType() == eRoute && !pPlot->IsRoutePillaged())
			continue;

		if (pPlot->getRouteType() == eRoute)
		{
			//repair road
			iTotalBuildTime += GetTurnsToBuild(pUnit, m_eRepairBuild, pPlot);
		}
		else
		{
			//build road
			iTotalBuildTime += GetTurnsToBuild(pUnit, eBuild, pPlot);
		}
	}

	return iTotalBuildTime;
}

int CvBuilderTaskingAI::GetTotalRouteBuildTime(const CvUnit* pUnit, const CvPlot* pPlot) const
{
	map<int, PlannedRoute>::const_iterator it = m_bestRouteForPlot.find(pPlot->GetPlotIndex());

	if (it == m_bestRouteForPlot.end())
		return INT_MAX;

	return GetRouteBuildTime(it->second, pUnit);
}

int CvBuilderTaskingAI::GetRouteMissingTiles(PlannedRoute plannedRoute) const
{
	RouteTypes eRoute = plannedRoute.second.first;

	map<PlannedRoute, vector<int>>::const_iterator it = m_plannedRoutePlots.find(plannedRoute);
	if (it == m_plannedRoutePlots.end())
		return 0;

	int iMissing = 0;

	for (vector<int>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(*it2);

		if (!pPlot)
			continue;

		if (m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eRoute))
			continue;

		if (pPlot->getRouteType() >= eRoute && !pPlot->IsRoutePillaged())
			continue;

		iMissing++;
	}

	return iMissing;
}

bool CvBuilderTaskingAI::WillNeverBuildVillageOnPlot(CvPlot* pPlot, RouteTypes eRoute, bool bIgnoreUnowned) const
{
	if ((pPlot->isOwned() || bIgnoreUnowned) && pPlot->getOwner() != m_pPlayer->GetID())
		return true;

	if (pPlot->getPlotType() == PLOT_MOUNTAIN)
		return true;

	if (pPlot->IsNaturalWonder())
		return true;

	if (pPlot->getFeatureType() == FEATURE_OASIS)
		return true;

	if (bIgnoreUnowned && !pPlot->getEffectiveOwningCity()->IsWithinWorkRange(pPlot))
		return true;

	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if (eResource != NO_RESOURCE)
		return true;

	// This civ has a special improvement that is assumed to be better than a village
	if (SavePlotForUniqueImprovement(pPlot) != NO_IMPROVEMENT)
		return true;

	// Building a village here removes the route in this case
	if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eRoute))
		return true;

	return false;
}

ImprovementTypes CvBuilderTaskingAI::SavePlotForUniqueImprovement(const CvPlot* pPlot) const
{
	if (m_uniqueImprovements.empty())
		return NO_IMPROVEMENT;

	FeatureTypes eFeature = pPlot->getFeatureType();
	ImprovementTypes eImprovement = NO_IMPROVEMENT;
	for (size_t i = 0; i < m_uniqueImprovements.size(); ++i)
	{
		eImprovement = m_uniqueImprovements[i];
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);

		if (((eFeature != NO_FEATURE && pkImprovementInfo->GetFeatureMakesValid(eFeature))) ||
			(pkImprovementInfo->IsAdjacentCity() && pPlot->IsAdjacentCity()) ||
			(pkImprovementInfo->IsCoastal() && pPlot->isCoastalLand()) ||
			(pkImprovementInfo->IsHillsMakesValid() && pPlot->isHills()))
		{
			if (pPlot->canHaveImprovement(eImprovement, m_pPlayer->GetID(), false, true))
				return eImprovement;
		}

		if (pkImprovementInfo->GetXSameAdjacentMakesValid() > 0)
		{
			std::tr1::unordered_map<ImprovementTypes, std::tr1::unordered_set<const CvPlot*>>::const_iterator it = m_extraPlotsForXAdjacentImprovements.find(eImprovement);
			if (it != m_extraPlotsForXAdjacentImprovements.end())
			{
				if (it->second.find(pPlot) != it->second.end())
					return eImprovement;
			}
		}
	}

	return NO_IMPROVEMENT;
}

void CvBuilderTaskingAI::UpdateCanalPlots()
{
	//not needed every turn ... simple performance optimization
	if ((GC.getGame().getGameTurn() + m_pPlayer->GetID()) % 7 == 0)
		return;

	//only majors build canals
	if (m_pPlayer->isMinorCiv())
		return;

	m_canalWantedPlots.clear();

	vector<CvPlot*> vDestPlots;
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		int iCity = 0;
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		for (CvCity* pDestCity = kLoopPlayer.firstCity(&iCity); pDestCity != NULL; pDestCity = kLoopPlayer.nextCity(&iCity))
			vDestPlots.push_back(pDestCity->plot());
	}

	int iOriginCityLoop = 0;
	for (CvCity* pOriginCity = m_pPlayer->firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = m_pPlayer->nextCity(&iOriginCityLoop))
	{
		SPathFinderUserData data(m_pPlayer->GetID(), PT_TRADE_WATER);
		//this is the important flag
		data.iFlags = CvUnit::MOVEFLAG_PRETEND_CANALS;
		//doesn't really matter but we don't want to underestimate the real range, so use the real range
		data.iMaxNormalizedDistance = m_pPlayer->GetTrade()->GetTradeRouteRange(DOMAIN_SEA, pOriginCity);

		//get all paths
		map<int, SPath> waterpaths = GC.GetStepFinder().GetMultiplePaths(pOriginCity->plot(), vDestPlots, data);
		for (map<int, SPath>::iterator it = waterpaths.begin(); it != waterpaths.end(); ++it)
		{
			//the paths may contain not-yet-existing canals but the path cost for them is very high
			//so they should only be used if there really is no other way.
			SPath& currentPath = it->second;
			for (int i = 0; i < currentPath.length(); i++)
			{
				CvPlot* pCheck = currentPath.get(i);

				if (pCheck->isWater() || pCheck->isCity())
					continue;

				//existing canal in use, remember that so we don't replace it by something else
				if (pCheck->IsImprovementPassable())
				{
					if (pCheck->getOwner() == m_pPlayer->GetID())
						m_canalWantedPlots.insert(pCheck->GetPlotIndex());
				}
				else
				{
					//this is where we must build a new canal
					if (pCheck->getOwner() == m_pPlayer->GetID())
						m_canalWantedPlots.insert(pCheck->GetPlotIndex());
				}
			}
		}
	}
}

vector<BuilderDirective> CvBuilderTaskingAI::GetDirectives()
{
	return m_directives;
}

BuilderDirective CvBuilderTaskingAI::GetAssignedDirective(CvUnit* pUnit)
{
	int iUnitID = pUnit->GetID();

	if (m_assignedDirectives.find(iUnitID) != m_assignedDirectives.end())
	{
		return m_assignedDirectives[iUnitID];
	}

	return BuilderDirective();
}

void CvBuilderTaskingAI::SetAssignedDirective(CvUnit* pUnit, BuilderDirective eDirective)
{
	m_assignedDirectives[pUnit->GetID()] = eDirective;
}

bool CvBuilderTaskingAI::CanUnitPerformDirective(CvUnit* pUnit, BuilderDirective eDirective, bool bTestEra)
{
	if (eDirective.m_eBuild == NO_BUILD)
		return false;

	CvPlot* pTargetPlot = GC.getMap().plot(eDirective.m_sX, eDirective.m_sY);

	if (!ShouldBuilderConsiderPlot(pUnit, pTargetPlot))
		return false;

	if (!pUnit->canBuild(pTargetPlot, eDirective.m_eBuild, false, true, bTestEra))
		return false;

	return true;
}

int CvBuilderTaskingAI::GetTurnsToBuild(const CvUnit* pUnit, BuildTypes eBuild, const CvPlot* pPlot) const
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iBuildLeft = pPlot->getBuildTime(eBuild, ePlayer);
	if (iBuildLeft == 0)
		return 0;

	int iBuildRate = pUnit ? pUnit->workRate(true) : 1; // If no unit, return total build time

	if (iBuildRate == 0)
	{
		//this means it will take forever under current circumstances
		return INT_MAX;
	}

	iBuildLeft -= pPlot->getBuildProgress(eBuild);
	iBuildLeft = std::max(0, iBuildLeft);

	int iTurnsLeft = (iBuildLeft / iBuildRate);
	//round up
	if (iTurnsLeft * iBuildRate < iBuildLeft)
		iTurnsLeft++;

	return iTurnsLeft;
}

//returns true if sucessful, false otherwise
bool CvBuilderTaskingAI::ExecuteWorkerMove(CvUnit* pUnit, BuilderDirective aDirective)
{
	bool bSuccessful = false;

	if (aDirective.m_eDirectiveType != BuilderDirective::NUM_DIRECTIVES)
	{
		switch (aDirective.m_eDirectiveType)
		{
		case BuilderDirective::KEEP_IMPROVEMENT:
			UNREACHABLE();
		case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
		case BuilderDirective::BUILD_IMPROVEMENT:
		case BuilderDirective::REPAIR:
		case BuilderDirective::BUILD_ROUTE:
		case BuilderDirective::REMOVE_FEATURE:
		case BuilderDirective::REMOVE_ROAD:
		{
			CvPlot* pPlot = GC.getMap().plot(aDirective.m_sX, aDirective.m_sY);
			MissionTypes eMission = CvTypes::getMISSION_MOVE_TO();
			if (pUnit->getX() == aDirective.m_sX && pUnit->getY() == aDirective.m_sY)
				eMission = CvTypes::getMISSION_BUILD();

			if (GC.getLogging() && GC.GetBuilderAILogging())
			{
				// Open the log file
				CvString strFileName = "BuilderTaskingLog.csv";
				FILogFile* pLog = NULL;
				pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

				// write in data
				CvString strLog;
				CvString strTemp;

				CvString strPlayerName;
				strPlayerName = m_pPlayer->getCivilizationShortDescription();
				strLog += strPlayerName;
				strLog += ",";

				strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
				strLog += strTemp;

				strTemp.Format("%d,", pUnit->GetID()); // unit id
				strLog += strTemp;

				switch (aDirective.m_eDirectiveType)
				{
				case BuilderDirective::KEEP_IMPROVEMENT:
					UNREACHABLE();
				case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
					strLog += "On resource,";
					break;
				case BuilderDirective::BUILD_IMPROVEMENT:
					strLog += "On plot,";
					break;
				case BuilderDirective::REPAIR:
					strLog += "Repairing,";
					break;
				case BuilderDirective::BUILD_ROUTE:
					strLog += "Building route,";
					break;
				case BuilderDirective::REMOVE_FEATURE:
					strLog += "Removing feature for production,";
					break;
				case BuilderDirective::REMOVE_ROAD:
					strLog += "Removing road,";
					break;
				}

				if (eMission == CvTypes::getMISSION_BUILD())
				{
					if (aDirective.m_eDirectiveType == BuilderDirective::REPAIR)
					{
						if (pPlot->IsImprovementPillaged())
						{
							strLog += "Repairing improvement";
						}
						else
						{
							strLog += "Repairing route";
						}
					}
					else if (aDirective.m_eDirectiveType == BuilderDirective::BUILD_ROUTE)
					{
						strLog += "Building route,";
					}
					else if (aDirective.m_eDirectiveType == BuilderDirective::BUILD_IMPROVEMENT || aDirective.m_eDirectiveType == BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE)
					{
						strLog += "Building improvement,";
					}
					else if (aDirective.m_eDirectiveType == BuilderDirective::REMOVE_FEATURE)
					{
						strLog += "Removing feature for production,";
					}
					else
					{
						strLog += "Removing road,";
					}
				}
				else
				{
					strLog += "Moving to location,";
				}

				pLog->Msg(strLog);
			}

			if (eMission == CvTypes::getMISSION_MOVE_TO())
			{
				pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), aDirective.m_sX, aDirective.m_sY,
					CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER | CvUnit::MOVEFLAG_VISIBLE_ONLY, false, false, MISSIONAI_BUILD, pPlot);

				//do we have movement left?
				if (pUnit->getMoves() > 0)
				{
					//might also have aborted the move mission
					if (pUnit->getX() == aDirective.m_sX && pUnit->getY() == aDirective.m_sY)
						eMission = CvTypes::getMISSION_BUILD();
				}
				else
					bSuccessful = true;
			}

			if (eMission == CvTypes::getMISSION_BUILD())
			{
				pUnit->PushMission(CvTypes::getMISSION_BUILD(), aDirective.m_eBuild, aDirective.m_eDirectiveType, 0, false, false, MISSIONAI_BUILD, pPlot);

				ASSERT_DEBUG(!pUnit->ReadyToMove(), "Worker did not do their mission this turn. Could cause game to hang.");
				bSuccessful = true;
			}

		}
		break;
		}
	}

	if (bSuccessful)
		SetAssignedDirective(pUnit, aDirective);

	return bSuccessful;
}

/// Looks at city connections and marks plots that can be added as routes by AddRouteDirective
void CvBuilderTaskingAI::UpdateRoutePlots(void)
{
	// updating plots that are part of the road network
	CvCityConnections* pCityConnections = m_pPlayer->GetCityConnections();

	m_plannedRouteAdditiveValues.clear();
	m_plannedRouteNonAdditiveValues.clear();
	m_plannedRoutePlots.clear();
	m_plannedRoutePurposes.clear();
	m_anyRoutePlanned.clear();
	m_bestRouteTypeAndValue.clear();
	m_plotRoutePurposes.clear();
	m_bestRouteForPlot.clear();

	// if there are fewer than 2 cities, we don't need to run this function
	std::vector<int> plotsToConnect = pCityConnections->GetPlotsToConnect();
	if(plotsToConnect.size() < 2)
	{
		return;
	}

	std::vector<OptionWithScore<int>> sortedPlots;
	for (std::vector<int>::const_iterator it = plotsToConnect.begin(); it != plotsToConnect.end(); ++it)
	{
		int iValue = 0;
		int iPlotIndex = *it;
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotIndex);
		CvCity* pCity = pPlot->getPlotCity();

		if (pCity)
		{
			int iPop = pCity->getPopulation();
			iValue = iPop;
		}

		sortedPlots.push_back(OptionWithScore<int>(iPlotIndex, iValue));
	}

	std::stable_sort(sortedPlots.begin(), sortedPlots.end());

	RouteTypes eBestRoute = m_pPlayer->getBestRoute();
	if(eBestRoute == NO_ROUTE)
	{
		return;
	}

	for(int i = GC.getNumBuildInfos() - 1; i >= 0; i--)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(!pkBuild)
		{
			continue;
		}

		RouteTypes eRoute = (RouteTypes)pkBuild->getRoute();
		if(eRoute == NO_ROUTE)
		{
			continue;
		}

		if(GC.getBuildInfo(eBuild)->getTechPrereq() != NO_TECH)
		{
			bool bHasTech = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechPrereq());
			if(!bHasTech)
			{
				continue;
			}
		}
#if defined(MOD_BALANCE_CORE)
		if(GC.getBuildInfo(eBuild)->getTechObsolete() != NO_TECH)
		{
			bool bHasTech = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechObsolete());
			if(bHasTech)
			{
				continue;
			}
		}
#endif

		for(std::vector<OptionWithScore<int>>::const_iterator it1 = sortedPlots.begin(); it1 != sortedPlots.end() - 1; ++it1)
		{
			for(std::vector<OptionWithScore<int>>::const_iterator it2 = it1 + 1; it2 != sortedPlots.end(); ++it2)
			{
				CvPlot* pFirstPlot = GC.getMap().plotByIndexUnchecked(it1->option);
				CvPlot* pSecondPlot = GC.getMap().plotByIndexUnchecked(it2->option);

				// get the two cities
				CvCity* pFirstCity  = pFirstPlot->getPlotCity();
				CvCity* pSecondCity  = pSecondPlot->getPlotCity();

				if (!pFirstCity || !pSecondCity)
					continue;

				bool bConnectOnlyCapitals = /*1*/ GD_INT_GET(CITY_CONNECTIONS_CONNECT_TO_CAPITAL) > 0;
				if (bConnectOnlyCapitals)
				{
					// connect cities for bonuses from villages and increased movement
					if (pFirstCity->getOwner() == m_pPlayer->GetID() && pSecondCity->getOwner() == m_pPlayer->GetID())
						ConnectCitiesForShortcuts(pFirstCity, pSecondCity, eBuild, eRoute);
				}
				else
				{
					ConnectCitiesForScenario(pFirstCity, pSecondCity, eBuild, eBestRoute);
				}
			}
		}

		// Connect cities to capital and plan strategic routes after the shortcut routes have been planned
		for (uint uiFirstCityIndex = 0; uiFirstCityIndex < plotsToConnect.size(); uiFirstCityIndex++)
		{
			for (uint uiSecondCityIndex = uiFirstCityIndex + 1; uiSecondCityIndex < plotsToConnect.size(); uiSecondCityIndex++)
			{
				CvPlot* pFirstPlot = GC.getMap().plotByIndexUnchecked(plotsToConnect[uiFirstCityIndex]);
				CvPlot* pSecondPlot = GC.getMap().plotByIndexUnchecked(plotsToConnect[uiSecondCityIndex]);

				// get the two cities
				CvCity* pFirstCity = pFirstPlot->getPlotCity();
				CvCity* pSecondCity = pSecondPlot->getPlotCity();
				CvCity* pPlayerCapitalCity = NULL;
				CvCity* pTargetCity = NULL;

				if (pFirstCity && !pSecondCity)
				{
					ConnectPointsForStrategy(pFirstCity, pSecondPlot, eBuild, eRoute);
					continue;
				}

				if (!pFirstCity && pSecondCity)
				{
					ConnectPointsForStrategy(pSecondCity, pFirstPlot, eBuild, eRoute);
					continue;
				}

				//we don't do citadel to citadel. or should we?
				if (!pFirstCity && !pSecondCity)
					continue;

				if ((pFirstCity->isCapital() && pFirstCity->getOwner() == m_pPlayer->GetID()) ||
						(pSecondCity->isCapital() && pSecondCity->getOwner() == m_pPlayer->GetID()))
				{
					if (pFirstCity->isCapital() && pFirstCity->getOwner() == m_pPlayer->GetID())
					{
						pPlayerCapitalCity = pFirstCity;
						pTargetCity = pSecondCity;
					}
					else if (pSecondCity->isCapital() && pSecondCity->getOwner() == m_pPlayer->GetID())
					{
						pPlayerCapitalCity = pSecondCity;
						pTargetCity = pFirstCity;
					}

					// connect city to capital for the money and happiness
					ConnectCitiesToCapital(pPlayerCapitalCity, pTargetCity, eBuild, eRoute);
				}
			}
		}
	}
}

/// Use the flavor settings to determine what to do
void CvBuilderTaskingAI::UpdateImprovementPlots()
{
	vector<OptionWithScore<BuilderDirective>> allDirectives(GetRouteDirectives());
	vector<OptionWithScore<BuilderDirective>> improvementDirectives = GetImprovementDirectives();
	allDirectives.insert(allDirectives.end(), improvementDirectives.begin(), improvementDirectives.end());

	std::stable_sort(allDirectives.begin(), allDirectives.end());
	LogDirectives(allDirectives);

	m_directives.clear();
	m_assignedDirectives.clear();
	for (vector<OptionWithScore<BuilderDirective>>::iterator it = allDirectives.begin(); it != allDirectives.end(); ++it)
		m_directives.push_back(it->option);
}

vector<OptionWithScore<BuilderDirective>> CvBuilderTaskingAI::GetRouteDirectives()
{
	vector<OptionWithScore<BuilderDirective>> aDirectives;

	map<PlotPair, map<pair<RouteTypes, bool>, pair<int, int>>> plannedRouteTypeValues;

	// Loop through all planned routes and calculate the additive and non-additive values for them
	for (map<PlannedRoute, RoutePurpose>::const_iterator it = m_plannedRoutePurposes.begin(); it != m_plannedRoutePurposes.end(); ++it)
	{
		PlannedRoute plannedRoute = it->first;
		PlotPair plotPair = plannedRoute.first;
		RouteTypes eRoute = plannedRoute.second.first;
		bool bUseRivers = plannedRoute.second.second;

		int iAdditiveValue = m_plannedRouteAdditiveValues[plannedRoute];
		int iNonAdditiveValue = m_plannedRouteNonAdditiveValues[plannedRoute];

		if (iAdditiveValue > 0 || iNonAdditiveValue > 0)
		{
			pair<int, int> oldValues = plannedRouteTypeValues[plotPair][make_pair(eRoute, bUseRivers)];
			plannedRouteTypeValues[plotPair][make_pair(eRoute, bUseRivers)] = make_pair(oldValues.first + iAdditiveValue, max(oldValues.second, iNonAdditiveValue));
		}
	}

	map<PlotPair, pair<int, int>> bestRouteValues;
	map<PlotPair, pair<RouteTypes, bool>> bestRouteType;

	// Loop through each pair of terminal plots and find the best route type and build value for this particular plot pair
	for (map<PlotPair, map<pair<RouteTypes, bool>, pair<int, int>>>::const_iterator it = plannedRouteTypeValues.begin(); it != plannedRouteTypeValues.end(); ++it)
	{
		PlotPair plotPair = it->first;

		PlannedRoute plannedRouteRoadNoRivers = make_pair(plotPair, make_pair(ROUTE_ROAD, false));
		PlannedRoute plannedRouteRoadWithRivers = make_pair(plotPair, make_pair(ROUTE_ROAD, true));
		PlannedRoute plannedRouteRailroad = make_pair(plotPair, make_pair(ROUTE_RAILROAD, false));

		int iRoadValueNoRivers = plannedRouteTypeValues[plotPair][plannedRouteRoadNoRivers.second].first + plannedRouteTypeValues[plotPair][plannedRouteRoadNoRivers.second].second;
		int iRoadValueWithRivers = plannedRouteTypeValues[plotPair][plannedRouteRoadWithRivers.second].first + plannedRouteTypeValues[plotPair][plannedRouteRoadWithRivers.second].second;
		int iRailroadValue = plannedRouteTypeValues[plotPair][plannedRouteRailroad.second].first + plannedRouteTypeValues[plotPair][plannedRouteRailroad.second].second;

		int iRoadTotalMaintenance = 0;
		if (iRoadValueNoRivers > 0)
		{
			// If the road is completed, increase its value
			iRoadValueNoRivers += iRoadValueNoRivers / (4 * (GetRouteMissingTiles(plannedRouteRoadNoRivers) + 1));

			int iRoadMaintenance = GC.getRouteInfo(ROUTE_ROAD)->GetGoldMaintenance() * (100 + m_pPlayer->GetImprovementGoldMaintenanceMod());

			vector<int> plannedRoadPlots = m_plannedRoutePlots[plannedRouteRoadNoRivers];

			// Need to recompute road maintenance length because we don't store it
			int iRoadMaintenanceTiles = 0;
			for (vector<int>::const_iterator it2 = plannedRoadPlots.begin(); it2 != plannedRoadPlots.end(); ++it2)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(*it2);
				if (!m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, ROUTE_ROAD))
					iRoadMaintenanceTiles++;
			}

			iRoadTotalMaintenance = iRoadMaintenanceTiles * iRoadMaintenance;
			iRoadValueNoRivers -= iRoadTotalMaintenance;
		}

		if (iRoadValueWithRivers > 0)
		{
			// If the road is completed, increase its value
			iRoadValueWithRivers += iRoadValueWithRivers / (4 * (GetRouteMissingTiles(plannedRouteRoadWithRivers) + 1));

			int iRoadMaintenance = GC.getRouteInfo(ROUTE_ROAD)->GetGoldMaintenance() * (100 + m_pPlayer->GetImprovementGoldMaintenanceMod());

			vector<int> plannedRoadPlots = m_plannedRoutePlots[plannedRouteRoadWithRivers];

			// Need to recompute road maintenance length because we don't store it
			int iRoadMaintenanceTiles = 0;
			for (vector<int>::const_iterator it2 = plannedRoadPlots.begin(); it2 != plannedRoadPlots.end(); ++it2)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(*it2);
				if (!m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, ROUTE_ROAD))
					iRoadMaintenanceTiles++;
			}

			iRoadTotalMaintenance = iRoadMaintenanceTiles * iRoadMaintenance;
			iRoadValueWithRivers -= iRoadTotalMaintenance;
		}

		int iRailroadTotalMaintenance = 0;
		if (iRailroadValue > 0)
		{
			// If the railroad is completed, increase its value
			iRailroadValue += iRailroadValue / (4 * (GetRouteMissingTiles(plannedRouteRailroad) + 1));

			int iRailroadMaintenance = GC.getRouteInfo(ROUTE_RAILROAD)->GetGoldMaintenance() * (100 + m_pPlayer->GetImprovementGoldMaintenanceMod());
			iRailroadTotalMaintenance = m_plannedRoutePlots[plannedRouteRailroad].size() * iRailroadMaintenance;
			iRailroadValue -= iRailroadTotalMaintenance;
		}

		if (iRoadValueNoRivers > 0 || iRoadValueWithRivers > 0 || iRailroadValue > 0)
		{
			if (iRoadValueNoRivers > iRailroadValue || iRoadValueWithRivers > iRailroadValue)
			{
				bool bUseRivers = iRoadValueWithRivers >= iRoadValueNoRivers;
				bestRouteType[plotPair] = make_pair(ROUTE_ROAD, bUseRivers);
				bestRouteValues[plotPair] = plannedRouteTypeValues[plotPair][make_pair(ROUTE_ROAD, bUseRivers)];
			}
			else
			{
				bestRouteType[plotPair] = make_pair(ROUTE_RAILROAD, false);
				bestRouteValues[plotPair] = plannedRouteTypeValues[plotPair][make_pair(ROUTE_RAILROAD, false)];
			}
		}
	}

	map<CvPlot*, pair<int, int>> plotValues;
	map<CvPlot*, RouteTypes> plotBestRouteType;
	map<CvPlot*, PlannedRoute> plotShortestRoute;
	map<CvPlot*, int> plotShortestRouteBuildTime;
	map<CvPlot*, RoutePurpose> plotPurposes;
	// Loop through all the best routes and give their respective plots the best value out of all possible paths
	for (map<PlotPair, pair<int, int>>::const_iterator it = bestRouteValues.begin(); it != bestRouteValues.end(); ++it)
	{
		PlotPair plotPair = it->first;
		pair<int, int> newValues = it->second;
		pair<RouteTypes, bool> routeTypeAndRiver = bestRouteType[plotPair];
		RouteTypes eRoute = routeTypeAndRiver.first;
		PlannedRoute plannedRoute = make_pair(plotPair, routeTypeAndRiver);

		vector<int> plots = m_plannedRoutePlots.find(plannedRoute)->second;
		RoutePurpose ePurpose = m_plannedRoutePurposes.find(plannedRoute)->second;
		for (vector<int>::const_iterator it2 = plots.begin(); it2 != plots.end(); ++it2)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(*it2);

			if (!pPlot)
				continue;

			plotPurposes[pPlot] = (RoutePurpose)(plotPurposes[pPlot] | ePurpose);

			if (eRoute >= plotBestRouteType[pPlot])
			{
				int iShortestRouteBuildTime = eRoute == plotBestRouteType[pPlot] && plotShortestRouteBuildTime.find(pPlot) != plotShortestRouteBuildTime.end() ? plotShortestRouteBuildTime[pPlot] : INT_MAX;
				int iRouteBuildTime = GetRouteBuildTime(plannedRoute);
				if (iRouteBuildTime < iShortestRouteBuildTime || eRoute > plotBestRouteType[pPlot])
				{
					plotShortestRouteBuildTime[pPlot] = iRouteBuildTime;
					plotBestRouteType[pPlot] = eRoute;
					m_bestRouteForPlot[pPlot->GetPlotIndex()] = plannedRoute;
				}
			}

			pair<int, int> oldValues = plotValues[pPlot];

			// Two routes going through the same plot, but with different destinations don't have additive value properties
			plotValues[pPlot] = make_pair(max(newValues.first, oldValues.first), max(newValues.second, oldValues.second));
		}
	}

	// Finally, add build directives to the all the chosen plots
	for (map<CvPlot*, pair<int, int>>::const_iterator it = plotValues.begin(); it != plotValues.end(); ++it)
	{
		CvPlot* pPlot = it->first;
		int iValue = it->second.first + it->second.second;
		RouteTypes eRoute = plotBestRouteType[pPlot];

		if (iValue <= 0)
			continue;

		if (!ShouldAnyBuilderConsiderPlot(pPlot))
			continue;

		AddRouteOrRepairDirective(aDirectives, pPlot, eRoute, iValue, plotPurposes[pPlot]);
	}

	return aDirectives;
}

static vector<OptionWithScore<BuilderDirective>> FilterNonOptimalNoTwoAdjacentImprovements(vector<OptionWithScore<BuilderDirective>> aDirectives, bool bIgnoreNoTwoAdjacent)
{
	vector<OptionWithScore<BuilderDirective>> aNewDirectives;

	for (vector<OptionWithScore<BuilderDirective>>::const_iterator it = aDirectives.begin(); it != aDirectives.end(); ++it)
	{
		BuildTypes eBuild = it->option.m_eBuild;
		if (eBuild == NO_BUILD)
		{
			aNewDirectives.push_back(*it);
			continue;
		}

		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if (eImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
			if(!pkImprovementInfo->IsNoTwoAdjacent())
			{
				aNewDirectives.push_back(*it);
				continue;
			}
		} 


		int iPotentialScore = it->option.GetPotentialScore();

		bool bInclude = true;
		for (vector<OptionWithScore<BuilderDirective>>::const_iterator it2 = aDirectives.begin(); it2 != aDirectives.end(); ++it2)
		{
			if (it->option == it2->option)
				continue;

			if (it->option.m_sX != it2->option.m_sX || it->option.m_sY != it2->option.m_sY)
				continue;

			int iPotentialOtherScore = it2->option.GetPotentialScore();

			if (iPotentialScore > iPotentialOtherScore)
				continue;

			BuildTypes eOtherBuild = it2->option.m_eBuild;
			if (eOtherBuild == NO_BUILD)
				continue;

			CvBuildInfo* pkOtherBuild = GC.getBuildInfo(eOtherBuild);
			ImprovementTypes eOtherImprovement = (ImprovementTypes)pkOtherBuild->getImprovement();
			if (eOtherImprovement == NO_IMPROVEMENT)
				continue;

			CvImprovementEntry* pkOtherImprovementInfo = GC.getImprovementInfo(eOtherImprovement);
			if (pkOtherImprovementInfo->IsCreatedByGreatPerson() || (bIgnoreNoTwoAdjacent && pkOtherImprovementInfo->IsNoTwoAdjacent()))
				continue;

			bInclude = false;
			break;
		}

		if (bInclude)
			aNewDirectives.push_back(*it);
	}

	return aNewDirectives;
}

static vector<OptionWithScore<BuilderDirective>> FilterNoTwoAdjacentDirectives(vector<OptionWithScore<BuilderDirective>> aDirectives)
{
	vector<OptionWithScore<BuilderDirective>> aNewDirectives;
	vector<OptionWithScore<BuilderDirective>> aNoTwoAdjacentDirectives;
	for (vector<OptionWithScore<BuilderDirective>>::iterator it = aDirectives.begin(); it != aDirectives.end(); ++it)
	{
		BuildTypes eBuild = it->option.m_eBuild;
		if (eBuild == NO_BUILD)
		{
			aNewDirectives.push_back(*it);
			continue;
		}

		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if (eImprovement == NO_IMPROVEMENT)
		{
			aNewDirectives.push_back(*it);
			continue;
		}

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (!pkImprovementInfo || !pkImprovementInfo->IsNoTwoAdjacent())
		{
			aNewDirectives.push_back(*it);
			continue;
		}

		// Reduce score if there are many adjacent tiles where we may want to build the same improvement
		CvPlot* pPlot = GC.getMap().plot(it->option.m_sX, it->option.m_sY);
		int iNeighboringSameScore = 0;
		for (vector<OptionWithScore<BuilderDirective>>::iterator it2 = aDirectives.begin(); it2 != aDirectives.end(); ++it2)
		{
			if (it->option == it2->option)
				continue;

			if (it->option.m_eBuild != it2->option.m_eBuild)
				continue;

			CvPlot* pOtherPlot = GC.getMap().plot(it2->option.m_sX, it2->option.m_sY);
			if (plotDistance(*pPlot, *pOtherPlot) != 1)
				continue;

			iNeighboringSameScore += it2->option.m_iScore;
		}

		int iNewScore = it->option.m_iScore - iNeighboringSameScore;
		it->score = iNewScore;

		aNoTwoAdjacentDirectives.push_back(*it);
	}

	std::stable_sort(aNoTwoAdjacentDirectives.begin(), aNoTwoAdjacentDirectives.end());

	// filter out non-optimal no-two-adjacent improvements
	for (vector<OptionWithScore<BuilderDirective>>::const_iterator it = aNoTwoAdjacentDirectives.begin(); it != aNoTwoAdjacentDirectives.end(); ++it)
	{
		OptionWithScore<BuilderDirective> bestDirectiveWithScore = *it;
		bestDirectiveWithScore.score = bestDirectiveWithScore.option.GetScore();
		CvPlot* pPlot = GC.getMap().plot(bestDirectiveWithScore.option.m_sX, bestDirectiveWithScore.option.m_sY);
		bool bAddDirective = true;

		for (vector<OptionWithScore<BuilderDirective>>::const_iterator it2 = aNewDirectives.begin(); it2 != aNewDirectives.end(); ++it2)
		{
			if (it->option.m_eBuild != it2->option.m_eBuild)
				continue;

			CvPlot* pOtherPlot = GC.getMap().plot(it2->option.m_sX, it2->option.m_sY);
			if (plotDistance(*pPlot, *pOtherPlot) != 1)
				continue;

			bAddDirective = false;
			break;
		}

		if (bAddDirective)
		{
			aNewDirectives.push_back(bestDirectiveWithScore);
		}
	}

	return aNewDirectives;
}

static void UpdateGreatPersonDirectives(vector<OptionWithScore<BuilderDirective>>& aDirectives)
{
	for (vector<OptionWithScore<BuilderDirective>>::iterator it = aDirectives.begin(); it != aDirectives.end(); ++it)
	{
		BuildTypes eBuild = it->option.m_eBuild;
		if (eBuild == NO_BUILD)
			continue;

		CvBuildInfo* pkBuild =  GC.getBuildInfo(eBuild);
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if (eImprovement == NO_IMPROVEMENT)
			continue;

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (!pkImprovementInfo->IsCreatedByGreatPerson())
			continue;

		int iBestScoreInPlot = 0;
		int iBestPotentialScoreInPlot = 0;

		for (vector<OptionWithScore<BuilderDirective>>::const_iterator it2 = aDirectives.begin(); it2 != aDirectives.end(); ++it2)
		{
			if (it->option == it2->option)
				continue;

			if (it->option.m_sX != it2->option.m_sX || it->option.m_sY != it2->option.m_sY)
				continue;

			BuildTypes eOtherBuild = it2->option.m_eBuild;
			if (eOtherBuild != NO_BUILD)
			{
				CvBuildInfo* pkOtherBuild = GC.getBuildInfo(eOtherBuild);
				ImprovementTypes eOtherImprovement = pkOtherBuild ? (ImprovementTypes)pkOtherBuild->getImprovement() : NO_IMPROVEMENT;
				if (eOtherImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkOtherImprovementInfo = GC.getImprovementInfo(eOtherImprovement);
					if (pkOtherImprovementInfo && pkOtherImprovementInfo->IsCreatedByGreatPerson())
						continue;

				}
			}

			int iPotentialScore = it2->option.GetPotentialScore();
			int iOtherScore = it2->option.m_iScore + it2->option.m_iPotentialBonusScore / 3;

			if (iPotentialScore > iBestPotentialScoreInPlot)
			{
				iBestScoreInPlot = iOtherScore;
				iBestPotentialScoreInPlot = iPotentialScore;
			}
		}

		it->option.m_iScorePenalty = iBestScoreInPlot;
	}
}

vector<OptionWithScore<BuilderDirective>> CvBuilderTaskingAI::GetImprovementDirectives()
{
	PlayerTypes ePlayer = m_pPlayer->GetID();
	vector<OptionWithScore<BuilderDirective>> aDirectives;

	// Check which builds we can build first
	vector<BuildTypes> aPossibleBuilds;
	for (int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;

		if (!m_pPlayer->canBuild(NULL, eBuild, true))
			continue;

		int iLoopUnit = 0;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			if (pLoopUnit->canBuild(NULL, eBuild))
			{
				aPossibleBuilds.push_back(eBuild);
				break;
			}
		}
	}

	if (aPossibleBuilds.empty())
		return aDirectives;

	//cache this
	// TODO include net gold in all gold yield/maintenance computations
	int iNetGoldTimes100 = m_pPlayer->GetTreasury()->CalculateBaseNetGoldTimes100();
	RouteTypes eBestRoute = m_pPlayer->getBestRoute();
	UpdateAllCityWorstPlots();

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);

		if (!ShouldAnyBuilderConsiderPlot(pPlot))
			continue;

		// Evaluate the improvement currently on the tile
		if (pPlot->getOwner() == ePlayer)
		{
			if (pPlot->getImprovementType() != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
			{
				pair<int,int> pScore = ScorePlotBuild(pPlot, pPlot->getImprovementType(), NO_BUILD);
				aDirectives.push_back(OptionWithScore<BuilderDirective>(BuilderDirective(BuilderDirective::KEEP_IMPROVEMENT, NO_BUILD, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), pScore.first, pScore.second), pScore.first));
			}
		}

		//action may depend on city
		CvCity* pWorkingCity = pPlot->getEffectiveOwningCity();

		if (pPlot->getOwner() == ePlayer)
		{
			UpdateCurrentPlotYields(pPlot);

			AddImprovingPlotsDirective(aDirectives, pPlot, pWorkingCity, aPossibleBuilds);
			AddScrubFalloutDirectives(aDirectives, pPlot, pWorkingCity);
			AddRepairImprovementDirective(aDirectives, pPlot, pWorkingCity);
		}
		else if (!pPlot->isOwned() && pPlot->isAdjacentPlayer(m_pPlayer->GetID()))
		{
			//some special improvements
			AddImprovingPlotsDirective(aDirectives, pPlot, pWorkingCity, aPossibleBuilds);
			AddRepairImprovementDirective(aDirectives, pPlot, pWorkingCity);
		}
		if (pPlot->GetPlayerResponsibleForRoute() == m_pPlayer->GetID())
		{
			AddRemoveRouteDirective(aDirectives, pPlot, iNetGoldTimes100);
		}
		if (pPlot->GetPlannedRouteState(m_pPlayer->GetID()) == ROAD_PLANNING_PRIORITY_CONSTRUCTION)
		{
			AddRouteOrRepairDirective(aDirectives, pPlot, eBestRoute, 1000, PURPOSE_MANUAL);
		}
	}

	// Special handling for no-two-adjacent and great person improvements
	aDirectives = FilterNonOptimalNoTwoAdjacentImprovements(aDirectives, true);
	aDirectives = FilterNoTwoAdjacentDirectives(aDirectives);
	aDirectives = FilterNonOptimalNoTwoAdjacentImprovements(aDirectives, false);
	UpdateGreatPersonDirectives(aDirectives);

	return aDirectives;
}

/// Evaluating a plot to determine what improvement could be best there
void CvBuilderTaskingAI::AddImprovingPlotsDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pCity, const vector<BuildTypes> aBuildsToConsider)
{
	ImprovementTypes eExistingImprovement = pPlot->getImprovementType();

	if (eExistingImprovement != NO_IMPROVEMENT)
	{
		// Do we have a special improvement here? (great person improvement, gifted improvement from major civ)
		if (pPlot->HasSpecialImprovement() || (m_pPlayer->isOption(PLAYEROPTION_SAFE_AUTOMATION) && m_pPlayer->isHuman()))
		{
			if (m_bLogging)
			{
				CvString strTemp;
				strTemp.Format(
					"%Weight,Improvement Blocked by Special Improvement,%s,%i,%i",
					GC.getImprovementInfo(pPlot->getImprovementType())->GetType(),
					pPlot->getX(),
					pPlot->getY()
				);
				LogInfo(strTemp, m_pPlayer);
			}
			return;
		}
	}

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	// loop through the build types to find one that we can use
	for (vector<BuildTypes>::const_iterator it = aBuildsToConsider.begin(); it != aBuildsToConsider.end(); ++it)
	{
		BuildTypes eBuild = *it;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild == NULL)
			continue;

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuild->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
			continue;

		if (eImprovement == eExistingImprovement)
			continue;

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (pkImprovementInfo == NULL)
			continue;

		//Check for test below.
		if (pkImprovementInfo->IsSpecificCivRequired() && m_pPlayer->getCivilizationType() != pkImprovementInfo->GetRequiredCivilization())
			continue;

		if (!pPlot->canBuild(eBuild, m_pPlayer->GetID()))
		{
			if (pkImprovementInfo->GetXSameAdjacentMakesValid() == 0)
				continue;

			if (m_extraPlotsForXAdjacentImprovements[eImprovement].find(pPlot) == m_extraPlotsForXAdjacentImprovements[eImprovement].end())
				continue;
		}

		ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
		bool bWillConnectResource = eResource != NO_RESOURCE && pkImprovementInfo->IsConnectsResource(eResource);

		FeatureTypes eFeature = pPlot->getFeatureType();
		bool bWillRemoveFeature = eFeature != NO_FEATURE && pkBuild->isFeatureRemove(eFeature);

		if (!bWillConnectResource && bWillRemoveFeature)
		{
			if (m_pPlayer->isOption(PLAYEROPTION_LEAVE_FORESTS) && m_pPlayer->isHuman())
			{
				if (m_bLogging) {
						CvString strTemp;
						strTemp.Format(
							"Feature Remove Blocked,%s,%s,%i,%i",
							GC.getFeatureInfo(eFeature)->GetType(),
							GC.getBuildInfo(eBuild)->GetType(),
							pPlot->getX(),
							pPlot->getY()
						);
						LogInfo(strTemp, m_pPlayer);
				}
				continue;
			}
		}

		pair<int,int> pScore = ScorePlotBuild(pPlot, eImprovement, eBuild);

		int iScore = pScore.first;
		int iPotentialScore = pScore.second;

		// if we're going backward, bail out!
		if(iScore <= 0)
		{
			continue;
		}
		
		BuilderDirective::BuilderDirectiveType eDirectiveType = bWillConnectResource ? BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE : BuilderDirective::BUILD_IMPROVEMENT;
		ResourceTypes eConnectedResource = bWillConnectResource ? eResource : NO_RESOURCE;

		BuilderDirective directive(eDirectiveType, eBuild, eConnectedResource, pkImprovementInfo->IsCreatedByGreatPerson(), pPlot->getX(), pPlot->getY(), iScore, iPotentialScore);

		if (m_bLogging)
		{
			CvString strTemp;
			strTemp.Format(
				"Weight,Directive Score Added,%s,%i,%i,%d",
				GC.getBuildInfo(eBuild)->GetType(),
				directive.m_sX,
				directive.m_sY,
				iScore
			);
			LogInfo(strTemp, m_pPlayer);
		}

		aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iScore));

		// If we can't build it yet, we may still want to prechop it
		TechTypes eRequiredTech = (TechTypes)pkBuild->getTechPrereq();
		if (eRequiredTech != NO_TECH && eFeature != NO_FEATURE && pkBuild->isFeatureRemove(eFeature) && !pkImprovementInfo->IsCreatedByGreatPerson())
		{
			if (!(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eRequiredTech)))
			{
				BuildTypes eChopBuild = NO_BUILD;
				for (int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
				{
					BuildTypes eLoopBuild = (BuildTypes)iBuildIndex;
					CvBuildInfo* pkBuild = GC.getBuildInfo(eLoopBuild);
					if (pkBuild && pkBuild->getImprovement() == NO_IMPROVEMENT && pkBuild->isFeatureRemoveOnly(eFeature) && m_pPlayer->canBuild(pPlot, eLoopBuild))
					{
						eChopBuild = eLoopBuild;
						break;
					}
				}

				if (eChopBuild != NO_BUILD)
				{
					BuilderDirective chopDirective(BuilderDirective::REMOVE_FEATURE, eChopBuild, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), iScore / 3, (iScore * 2) / 3 + iPotentialScore - 1);
					aDirectives.push_back(OptionWithScore<BuilderDirective>(chopDirective, iScore / 3));
				}
			}
		}
	}
}

/// Adds a directive if the unit can construct a road in the plot
void CvBuilderTaskingAI::AddRemoveRouteDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, int iNetGoldTimes100)
{
	//minors stay out
	if (m_pPlayer->isMinorCiv())
		return;

	//can we even remove routes?
	if (m_eRemoveRouteBuild==NO_BUILD || !m_pPlayer->canBuild(pPlot,m_eRemoveRouteBuild))
		return;

	// If "Automated Workers Don't Replace Improvements" option is enabled, don't remove roads
	if (m_pPlayer->isOption(PLAYEROPTION_SAFE_AUTOMATION) && m_pPlayer->isHuman())
		return;

	if (pPlot->GetPlannedRouteState(m_pPlayer->GetID()) >= ROAD_PLANNING_INCLUDE)
		return;

	RouteTypes eExistingRoute = pPlot->getRouteType();
	if (eExistingRoute == NO_ROUTE)
		return;

	if (pPlot->isCity())
		return;

	// we don't need to remove pillaged routes
	if (pPlot->IsRoutePillaged())
		return;

	// don't remove routes which we do not pay maintenance for
	PlayerTypes eRouteResponsible = pPlot->GetPlayerResponsibleForRoute();
	if (eRouteResponsible != m_pPlayer->GetID())
		return;

	// don't remove routes that our master built
	if (GET_TEAM(m_pPlayer->getTeam()).IsVassal(GET_PLAYER(pPlot->GetPlayerThatBuiltRoute()).getTeam()))
		return;

	// Don't remove roads far from owned territory
	if (pPlot->getOwner() != m_pPlayer->GetID() && !pPlot->isAdjacentPlayer(m_pPlayer->GetID()))
		return;

	pair<RouteTypes, int> pRouteAndValue = GetBestRouteTypeAndValue(pPlot);

	RouteTypes eNeededRoute = pRouteAndValue.first;
	int iRouteValue = pRouteAndValue.second;

	if (m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eNeededRoute))
		eNeededRoute = NO_ROUTE;

	if (eNeededRoute != NO_ROUTE && iRouteValue >= 0)
		return;

	CvRouteInfo* pExistingRouteInfo = GC.getRouteInfo(eExistingRoute);

	int iWeight = pExistingRouteInfo->GetGoldMaintenance() * (100 + m_pPlayer->GetImprovementGoldMaintenanceMod());

	//if we are losing gold, be more aggressive
	if (iNetGoldTimes100 < -1000)
		iWeight *= 10;

	BuilderDirective directive(BuilderDirective::REMOVE_ROAD, m_eRemoveRouteBuild, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), iWeight, 0);

	if (m_bLogging)
	{
		CvString strTemp;
		strTemp.Format(
			"RemoveRouteDirectives,%d,%d,%d",
			pPlot->getX(),
			pPlot->getY(),
			iWeight
		);
		LogInfo(strTemp, m_pPlayer);
	}

	aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iWeight));
}

void CvBuilderTaskingAI::AddRouteOrRepairDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, int iValue, RoutePurpose ePurpose)
{
	m_bestRouteTypeAndValue[pPlot->GetPlotIndex()] = make_pair(eRoute, iValue);
	m_plotRoutePurposes[pPlot->GetPlotIndex()] = ePurpose;

	// Reduce value if there are no adjacent routes (directive value is set to full value if a route is planned adjacent to this plot in CvHomelandAI)
	bool bAnyAdjacentRoute = false;
	CvPlot** pPlotsToCheck = GC.getMap().getNeighborsUnchecked(pPlot);
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = pPlotsToCheck[iI];

		if (!pAdjacentPlot)
			continue;

		if ((pAdjacentPlot->getRouteType() != NO_ROUTE && !pAdjacentPlot->IsRoutePillaged()) || m_pPlayer->GetSameRouteBenefitFromTrait(pAdjacentPlot, ROUTE_ROAD))
		{
			bAnyAdjacentRoute = true;
			break;
		}
	}

	if (!bAnyAdjacentRoute)
		iValue = (iValue * 4) / 5;

	if (pPlot->getRouteType() == eRoute && pPlot->IsRoutePillaged())
	{
		AddRepairRouteDirective(aDirectives, pPlot, eRoute, iValue);
	}
	else if (pPlot->getRouteType() != eRoute)
	{
		AddRouteDirective(aDirectives, pPlot, eRoute, iValue);
	}
}

/// Adds a directive if the unit can construct a road in the plot
void CvBuilderTaskingAI::AddRouteDirective(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, RouteTypes eRoute, int iValue)
{
	if (m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eRoute))
		return;

	// don't downgrade routes that our master built (or if we're a minor civ, or if we don't have to pay for the route)
	TeamTypes eMyTeam = m_pPlayer->getTeam();
	if (pPlot->getRouteType() > eRoute && !pPlot->IsRoutePillaged() &&
		(m_pPlayer->isMinorCiv() || GET_TEAM(eMyTeam).IsVassal(GET_PLAYER(pPlot->GetPlayerThatBuiltRoute()).getTeam()) || eMyTeam != GET_PLAYER(pPlot->GetPlayerResponsibleForRoute()).getTeam()))
		return;

	BuildTypes eRouteBuild = GetBuildRoute(eRoute);

	// can we even build the desired route
	if(eRouteBuild == NO_BUILD || !m_pPlayer->canBuild(pPlot, eRouteBuild))
		return;

	CvRouteInfo* pRouteInfo = GC.getRouteInfo(eRoute);
	if (!pRouteInfo)
		return;

	BuilderDirective directive(BuilderDirective::BUILD_ROUTE, eRouteBuild, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), iValue, 0);

	if(m_bLogging)
	{
		CvString strTemp;
		strTemp.Format(
			"%i,AddRouteDirectives,%d,%d", 
			pPlot->getX(),
			pPlot->getY(),
			iValue
		);
		LogInfo(strTemp, m_pPlayer);
	}

	aDirectives.push_back(OptionWithScore<BuilderDirective>(directive, iValue));
}

/// Determines if the builder should "chop" the feature in the tile
void CvBuilderTaskingAI::AddChopDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pCity)
{
	// if it's not within a city radius
	if(!pPlot->isWithinTeamCityRadius(m_pPlayer->getTeam()))
	{
		return;
	}

	if(pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		return;
	}

	if(m_pPlayer->isOption(PLAYEROPTION_LEAVE_FORESTS))
	{
		return;
	}

	if (pPlot->IsImprovementPillaged())
		return;

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	// check to see if a resource is here. If so, bail out!
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	if(eResource != NO_RESOURCE)
	{
		return;
	}

	if(!pCity)
	{
		return;
	}

	if(pCity->GetCityCitizens()->IsWorkingPlot(pPlot))
	{
		return;
	}

	FeatureTypes eFeature = pPlot->getFeatureType();
	if(eFeature == NO_FEATURE)
	{
		// no feature in this tile, so bail
		return;
	}

#if defined(MOD_BALANCE_CORE)
	//Don't cut down city connections!
	if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		if (pPlot->IsCityConnection(m_pPlayer->GetID()))
		{
			return;
		}
	}
#endif

	BuildTypes eChopBuild = NO_BUILD;
	for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(NULL != pkBuild && pkBuild->getImprovement() == NO_IMPROVEMENT && pkBuild->isFeatureRemove(eFeature) && pkBuild->getFeatureProduction(eFeature) > 0 && m_pPlayer->canBuild(pPlot, eBuild))
		{
			eChopBuild = eBuild;
			break;
		}
	}

	if(eChopBuild == NO_BUILD)
	{
		// we couldn't find a build that removed the feature without a production benefit
		return;
	}

	pCity = NULL;
	int iProduction = pPlot->getFeatureProduction(eChopBuild, m_pPlayer->GetID(), &pCity);

	if(!DoesBuildHelpRush(pPlot, eChopBuild))
	{
		return;
	}

	int iWeight = /*1000*/ GD_INT_GET(BUILDER_TASKING_BASELINE_REPAIR);
	iWeight *= iProduction; // times the amount that the plot produces from the chopping

	int iYieldDifferenceWeight = 0;
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		// calculate natural yields
		int iPreviousYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->GetID(), pPlot->getFeatureType(), pPlot->getResourceType(m_pPlayer->getTeam()), NULL);
		int iNewYield = pPlot->calculateNatureYield((YieldTypes)ui, m_pPlayer->GetID(), NO_FEATURE /*eNewFeature*/, pPlot->getResourceType(m_pPlayer->getTeam()), NULL);
		int iDeltaYield = iNewYield - iPreviousYield;

		if(iDeltaYield == 0)
		{
			continue;
		}

		for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
		{
			switch(ui)
			{
			case YIELD_FOOD:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GROWTH")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*3*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD);
				}
				break;
			case YIELD_PRODUCTION:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_PRODUCTION")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*2*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION);
				}
				break;
			case YIELD_GOLD:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GOLD")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*2*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD);
				}
				break;
			case YIELD_SCIENCE:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_SCIENCE")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*2*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE);
				}
				break;
			case YIELD_CULTURE:
				if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CULTURE")
				{
					iYieldDifferenceWeight += iDeltaYield * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * /*3*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE);
				}
				break;
			case YIELD_FAITH:
				break;
			case YIELD_TOURISM:
				break;
			case YIELD_GOLDEN_AGE_POINTS:
				break;
			}
		}
	}

	// if we are going backwards, bail
	if(iYieldDifferenceWeight < 0)
	{
		return;
	}

	iWeight += iYieldDifferenceWeight;

	if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		iWeight = iWeight / 10;
	}
	if (m_pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest() && eFeature == FEATURE_FOREST)
	{
		iWeight = iWeight / 10;
	}
	// this doesn't actually help them, but adds some historical flavor
	if (m_pPlayer->GetPlayerTraits()->IsEmbarkedAllWater() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
	{
		iWeight = iWeight * 2;
	}

	if(iWeight > 0)
	{
		aDirectives.push_back(OptionWithScore<BuilderDirective>(BuilderDirective(BuilderDirective::REMOVE_FEATURE, eChopBuild, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), iWeight, 0), iWeight));
	}
}

void CvBuilderTaskingAI::AddRepairImprovementDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, CvCity* pWorkingCity)
{
	if (!pPlot)
		return;

	bool bImprovementPillaged = pPlot->getImprovementType() != NO_IMPROVEMENT && pPlot->IsImprovementPillaged();

	if (!bImprovementPillaged)
		return;

	if (pPlot->getOwner() == NO_PLAYER || pPlot->getOwner() != m_pPlayer->GetID())
		return;

	if (pWorkingCity && pWorkingCity->IsRazing())
		return;

	pair<int,int> pScore = ScorePlotBuild(pPlot, pPlot->getImprovementType(), m_eRepairBuild);

	int iScore = pScore.first;
	int iPotentialScore = pScore.second;

	if (iScore > 0)
	{
		aDirectives.push_back(OptionWithScore<BuilderDirective>(BuilderDirective(BuilderDirective::REPAIR, m_eRepairBuild, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), iScore, iPotentialScore), iScore));
	}
}

void CvBuilderTaskingAI::AddRepairRouteDirective(vector<OptionWithScore<BuilderDirective>>& aDirectives, CvPlot* pPlot, RouteTypes eRoute, int iValue)
{
	if (!pPlot)
		return;

	bool bRoutePillaged = pPlot->getRouteType() != NO_ROUTE && pPlot->IsRoutePillaged();

	if (!bRoutePillaged)
		return;

	if (m_pPlayer->GetSameRouteBenefitFromTrait(pPlot, eRoute))
		return;

	if (iValue > 0)
	{
		aDirectives.push_back(OptionWithScore<BuilderDirective>(BuilderDirective(BuilderDirective::REPAIR, m_eRepairBuild, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), iValue, 0), iValue));
	}
}

// Everything means less than zero, hey
void CvBuilderTaskingAI::AddScrubFalloutDirectives(vector<OptionWithScore<BuilderDirective>> &aDirectives, CvPlot* pPlot, CvCity* pCity)
{
	if(m_eFalloutFeature == NO_FEATURE || m_eFalloutRemove == NO_BUILD)
	{
		return;
	}

	if (pPlot->getTeam() != m_pPlayer->getTeam())
	{
		return;
	}

	// if city owning this plot is being razed, ignore this plot
	if (pCity && pCity->IsRazing())
		return;

	if(pPlot->getFeatureType() == m_eFalloutFeature && m_pPlayer->canBuild(pPlot, m_eFalloutRemove))
	{
		int iWeight =/*20000*/ GD_INT_GET(BUILDER_TASKING_BASELINE_SCRUB_FALLOUT);

		aDirectives.push_back(OptionWithScore<BuilderDirective>(BuilderDirective(BuilderDirective::REMOVE_FEATURE, m_eFalloutRemove, NO_RESOURCE, false, pPlot->getX(), pPlot->getY(), iWeight, 0), iWeight));
	}
}

/// Evaluates all the circumstances to determine if any builder should evaluate the given plot
bool CvBuilderTaskingAI::ShouldAnyBuilderConsiderPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	if (pPlot->isCity())
		return false;

	if (!pPlot->isRevealed(m_pPlayer->getTeam()))
		return false;

	//can't build in other major player's territory (unless they are our vassal)
	if (pPlot->isOwned() && pPlot->getOwner() != m_pPlayer->GetID() && GET_PLAYER(pPlot->getOwner()).isMajorCiv() && !GET_TEAM(pPlot->getTeam()).IsVassal(m_pPlayer->getTeam()))
		return false;

	return true;
}

/// Evaluates all the circumstances to determine if this builder can and should evaluate the given plot
bool CvBuilderTaskingAI::ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot)
{
	if (!pUnit)
		return false;

	// workers should not be able to work in plots that do not match their default domain
	switch(pUnit->getDomainType())
	{
	case DOMAIN_LAND:
		// As embarked workers can now build fishing boats, we need to consider coastal plots
		if(pPlot->isDeepWater())
		{
			return false;
		}
		break;
	case DOMAIN_SEA:
		if(!pPlot->isWater())
		{
			return false;
		}
		break;
	default:
		break;
	}

	//if there is fallout, try to scrub it in spite of the danger
	if(pPlot->getFeatureType() == FEATURE_FALLOUT && !pUnit->ignoreFeatureDamage() && (pUnit->GetCurrHitPoints() < (pUnit->GetMaxHitPoints() / 2)))
	{
		if(GC.getLogging() && GC.getAILogging() && m_bLogging)
		{
			CvString strLog;
			strLog.Format(
				"%i,Bailing due to fallout,%d,%d,%d", 
				pUnit->GetID(),
				pPlot->getX(),
				pPlot->getY(),
				m_pPlayer->GetPlotDanger(*pPlot, true)
			);
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLog);
		}
		return false;
	}

	if (!pUnit->canEndTurnAtPlot(pPlot))
	{
		if(m_bLogging)
		{
			CvString strLog;
			strLog.Format(
				"%i,This tile is full with another unit - bailing,%d,%d", 
				pUnit->GetID(),
				pPlot->getX(),
				pPlot->getY()
			);
			LogInfo(strLog, m_pPlayer, true);
		}

		return false;
	}

	return true;
}

/// Return the weight of this resource
int CvBuilderTaskingAI::GetResourceWeight(ResourceTypes eResource, int iQuantity)
{
	CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
	if (!pkResource)
		return 0;

	// if this is a luxury resource the player doesn't have, provide a bonus to getting it
	if (pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY && !GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(m_pPlayer->GetID(), eResource))
	{
		int iValue = pkResource->getHappiness() * /*750*/ GD_INT_GET(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE) * iQuantity;

		return iValue;
	}
	else if (pkResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
	{
		// measure quantity
		int iValue = (iQuantity + 1) * 250;

		return iValue;
	}

	return 0;
}

/// Does this city want to rush a build?
bool CvBuilderTaskingAI::DoesBuildHelpRush(CvPlot* pPlot, BuildTypes eBuild)
{
	CvCity* pCity = NULL;
	int iProduction = pPlot->getFeatureProduction(eBuild, m_pPlayer->GetID(), &pCity);
	if(iProduction <= 0)
	{
		return false;
	}

	if(!pCity)
	{
		// this chop does not benefit any city
		return false;
	}

	if(pCity->getOrderQueueLength() <= 0)
	{
		// nothing in the build queue
		return false;
	}

	if(!(pCity->getOrderFromQueue(0)->bRush))
	{
		// this order should not be rushed
		return false;
	}

	return true;
}

static BuildTypes GetPlannedBuildInPlot(const CvPlot* pPlot, const SBuilderState& sState)
{
	map<int, pair<BuildTypes, ImprovementTypes>>::const_iterator it = sState.mChangedPlotImprovements.find(pPlot->GetPlotIndex());

	if (it != sState.mChangedPlotImprovements.end())
		return it->second.first;

	return NO_BUILD;
}

static ImprovementTypes GetPlannedImprovementInPlot(const CvPlot* pPlot, const SBuilderState& sState)
{
	map<int, pair<BuildTypes, ImprovementTypes>>::const_iterator it = sState.mChangedPlotImprovements.find(pPlot->GetPlotIndex());

	if (it != sState.mChangedPlotImprovements.end())
		return it->second.second;

	if (!pPlot->IsImprovementPillaged())
		return pPlot->getImprovementType();

	return NO_IMPROVEMENT;
}

static FeatureTypes GetPlannedFeatureInPlot(const CvPlot* pPlot, const SBuilderState& sState)
{
	map<int, FeatureTypes>::const_iterator it = sState.mChangedPlotFeatures.find(pPlot->GetPlotIndex());

	if (it != sState.mChangedPlotFeatures.end())
		return it->second;

	return pPlot->getFeatureType();
}

static fraction GetCurrentAdjacencyScoreFromImprovementsAndTerrain(const CvPlot* pPlot, const CvImprovementEntry& kImprovementInfo, YieldTypes eYield, const SBuilderState& sState)
{
	fraction fTotalAdjacencyBonus = 0;

	CvPlot** pAdjacentPlots = GC.getMap().getNeighborsUnchecked(pPlot->GetPlotIndex());
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = pAdjacentPlots[iI];

		if (!pAdjacentPlot)
			continue;

		TerrainTypes eAdjacentTerrain = pAdjacentPlot->getTerrainType();
		fraction fAdjacentTerrainYield = kImprovementInfo.GetYieldPerXAdjacentTerrain(eYield, eAdjacentTerrain);
		if (fAdjacentTerrainYield != 0)
			fTotalAdjacencyBonus += fAdjacentTerrainYield;

		ImprovementTypes eAdjacentImprovement = GetPlannedImprovementInPlot(pAdjacentPlot, sState);

		if (eAdjacentImprovement == NO_IMPROVEMENT)
			continue;

		fraction fAdjacentImprovementYield = kImprovementInfo.GetYieldPerXAdjacentImprovement(eYield, eAdjacentImprovement);
		if (fAdjacentImprovementYield != 0)
			fTotalAdjacencyBonus += fAdjacentImprovementYield;
	}

	return fTotalAdjacencyBonus;
}

static Likelyhood GetPlotTheftLikelyhood(const CvPlayer* pPlayer, const CvPlot* pPlot)
{
	bool bAnyAdjacentUnowned = false;
	const CvDiplomacyAI* pDiplomacyAI = pPlayer->GetDiplomacyAI();
	CivApproachTypes eWorstApproach = CIV_APPROACH_FRIENDLY;

	for (int iI = 1; iI < RING2_PLOTS; iI++)
	{
		if (iI >= RING1_PLOTS && !bAnyAdjacentUnowned)
			return LIKELYHOOD_IMPOSSIBLE;

		CvPlot* pAdjacentPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);

		if (!pAdjacentPlot)
			continue;

		if (iI < RING1_PLOTS && pAdjacentPlot->getTeam() != pPlayer->getTeam())
			bAnyAdjacentUnowned = true;

		if (pAdjacentPlot->getTeam() != pPlayer->getTeam())
		{
			if (pAdjacentPlot->getOwner() == NO_PLAYER)
				continue;

			if (!GET_PLAYER(pAdjacentPlot->getOwner()).isMajorCiv())
				continue;

			CivApproachTypes eApproach = pDiplomacyAI->GetVisibleApproachTowardsUs(pAdjacentPlot->getOwner());

			// Civs who are afraid of us are unlikely to steal, so treat them as friendly
			if (eApproach != CIV_APPROACH_AFRAID && eApproach < eWorstApproach)
				eWorstApproach = eApproach;
		}
	}

	switch (eWorstApproach)
	{
	case CIV_APPROACH_WAR:
	case CIV_APPROACH_HOSTILE:
		return LIKELYHOOD_CERTAIN;
	case CIV_APPROACH_GUARDED:
		return LIKELYHOOD_LIKELY;
	case CIV_APPROACH_NEUTRAL:
		return LIKELYHOOD_POSSIBLE;
	default:
		return LIKELYHOOD_UNLIKELY;
	}
}

pair<int,int> CvBuilderTaskingAI::ScorePlotBuild(CvPlot* pPlot, ImprovementTypes eImprovement, BuildTypes eBuild, const SBuilderState& sState)
{
	const CvBuildInfo* pkBuildInfo = eBuild != NO_BUILD ? GC.getBuildInfo(eBuild) : NULL;

	if (eImprovement == NO_IMPROVEMENT && pPlot->IsImprovementPillaged() && pkBuildInfo && pkBuildInfo->isRepair())
		eImprovement = pPlot->getImprovementType();

	const CvImprovementEntry* pkImprovementInfo = eImprovement != NO_IMPROVEMENT ? GC.getImprovementInfo(eImprovement) : NULL;

	const ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	const FeatureTypes eFeature = pPlot->getFeatureType();

	//Some base value.
	int iYieldScore = 0;
	int iSecondaryScore = 0;
	int iPotentialScore = 0;

	// Not building anything has some inherent value
	if (eBuild == NO_BUILD && eImprovement != NO_IMPROVEMENT)
		iSecondaryScore += 100;

	const CvCity* pOwningCity = pPlot->getEffectiveOwningCity();

	// If we are performing a culture bomb, find which city will be owning the plot
	bool bIsCultureBomb = pkImprovementInfo ? pkImprovementInfo->GetCultureBombRadius() > 0 : false;
	bool bIsTileClaim = pkImprovementInfo ? pkImprovementInfo->IsNewOwner() : false;
	if ((bIsCultureBomb || bIsTileClaim) && !pOwningCity)
	{
		int iBestCityDistance = INT_MAX;
		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			CvPlot* pLoopCityPlot = pLoopCity->plot();
			if (pLoopCityPlot)
			{
				int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

				if (iDistance < iBestCityDistance)
				{
					pOwningCity = pLoopCity;
					iBestCityDistance = iDistance;
				}
			}
		}
	}

	if (pOwningCity && pOwningCity->IsRazing())
		return make_pair(-1, 0);

	// Give a small bonus for claiming tiles
	if (bIsTileClaim && pPlot->getOwner() != m_pPlayer->GetID())
	{
		iSecondaryScore += 300;
	}

	Likelyhood plotTheftLikelyhood = GetPlotTheftLikelyhood(m_pPlayer, pPlot);
	if (pkImprovementInfo && pkImprovementInfo->IsBlockTileSteal())
	{
		switch (plotTheftLikelyhood)
		{
		case LIKELYHOOD_CERTAIN:
			iSecondaryScore += 1000;
			break;
		case LIKELYHOOD_LIKELY:
			iSecondaryScore += 500;
			break;
		case LIKELYHOOD_POSSIBLE:
			iSecondaryScore += 100;
			break;
		case LIKELYHOOD_UNLIKELY:
			iSecondaryScore += 40;
			break;
		case LIKELYHOOD_IMPOSSIBLE:
			// No bonus for blocking tile steal if theft is impossible
			break;
		}
	}
	else if (pkImprovementInfo && pkImprovementInfo->IsCreatedByGreatPerson() && !bIsCultureBomb)
	{
		switch (plotTheftLikelyhood)
		{
		case LIKELYHOOD_CERTAIN:
			iSecondaryScore -= 1000;
			break;
		case LIKELYHOOD_LIKELY:
			iSecondaryScore -= 500;
			break;
		case LIKELYHOOD_POSSIBLE:
			iSecondaryScore -= 100;
			break;
		case LIKELYHOOD_UNLIKELY:
			iSecondaryScore -= 40;
			break;
		case LIKELYHOOD_IMPOSSIBLE:
			// No penalty for great person improvements if theft is impossible
			break;
		}
	}

	// Give a flat bonus for GPP Rate (200 per 5%)
	if (pkImprovementInfo && pkImprovementInfo->GetGreatPersonRateModifier() != 0)
	{
		iSecondaryScore += pkImprovementInfo->GetGreatPersonRateModifier() * 40;
	}
	
	// Do we have or will we build a road here?
	RouteTypes eRouteNeeded = GetBestRouteTypeAndValue(pPlot).first;
	RouteTypes eForceCityConnection = eRouteNeeded;

	if (eRouteNeeded != NO_ROUTE && !IsRoutePlanned(pPlot, eRouteNeeded, PURPOSE_SHORTCUT) && !IsRoutePlanned(pPlot, eRouteNeeded, PURPOSE_CONNECT_CAPITAL))
	{
		eForceCityConnection = NO_ROUTE;
	}

	const bool bIsWithinWorkRange = pPlot->isPlayerCityRadius(m_pPlayer->GetID());

	const ResourceTypes eResourceFromImprovement = pkImprovementInfo ? (ResourceTypes)pkImprovementInfo->GetResourceFromImprovement() : NO_RESOURCE;
	const FeatureTypes eFeatureFromImprovement = pkImprovementInfo ? pkImprovementInfo->GetCreatedFeature() : NO_FEATURE;

	// Give a bonus for chopping
	if (eFeature != NO_FEATURE && pkBuildInfo && pkBuildInfo->isFeatureRemove(eFeature) && DoesBuildHelpRush(pPlot, eBuild))
	{
		int iProductionFromChop = pPlot->getFeatureProduction(eBuild, m_pPlayer->GetID(), NULL);
		if (iProductionFromChop > 0)
			iSecondaryScore += iProductionFromChop * GetYieldBaseModifierTimes100(YIELD_PRODUCTION) / 50;
	}

	// Bonuses to domain (give simple linear bonuses for now, should be fine)
	CvCity* pActualOwningCity = pPlot->getOwningCity();
	if (pkImprovementInfo && pActualOwningCity)
	{
		for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
		{
			DomainTypes eDomain = (DomainTypes)iI;

			if (eDomain == DOMAIN_SEA && !pActualOwningCity->isCoastal())
				continue;

			int iDomainProductionModifier = pkImprovementInfo->GetDomainProductionModifier(iI);
			if (iDomainProductionModifier != 0)
			{
				iSecondaryScore += iDomainProductionModifier * 50;
			}

			int iDomainFreeExperience = pkImprovementInfo->GetDomainFreeExperience(iI);
			if (iDomainFreeExperience != 0)
			{
				iSecondaryScore += iDomainFreeExperience * 20;
			}
		}
	}

	int iExtraResource = 0;
	if ((eResource != NO_RESOURCE && pkImprovementInfo && pkImprovementInfo->IsConnectsResource(eResource)) || eResourceFromImprovement != NO_RESOURCE)
	{
		map<ResourceTypes, int>::const_iterator it;
		if (eResourceFromImprovement != NO_RESOURCE)
		{
			it = sState.mExtraResources.find(eResourceFromImprovement);
			if (it != sState.mExtraResources.end())
				iExtraResource = it->second;
		}
		else
		{
			it = sState.mExtraResources.find(eResource);
			if (it != sState.mExtraResources.end())
				iExtraResource = it->second;
		}
	}

	if (bIsWithinWorkRange)
	{
		if (eBuild != NO_BUILD)
			UpdateProjectedPlotYields(pPlot, eBuild, eForceCityConnection);
		else
			UpdateCurrentPlotYields(pPlot);
	}

	vector<ImprovementTypes> bestPotentialImprovementInDirection((size_t)NUM_DIRECTION_TYPES, NO_IMPROVEMENT);

	// Calculate the best potential adjacent improvements
	if (MOD_BALANCE_VP && bIsWithinWorkRange && eImprovement != NO_IMPROVEMENT)
	{
		for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), (DirectionTypes)iDirection);

			if (!pAdjacentPlot)
				continue;

			if (!pAdjacentPlot->isPlayerCityRadius(m_pPlayer->GetID()))
				continue;

			if (pAdjacentPlot->getOwner() != NO_PLAYER && pAdjacentPlot->getOwner() != m_pPlayer->GetID())
				continue;

			if (pAdjacentPlot->isCity())
				continue;

			const ImprovementTypes eAdjacentImprovement = GetPlannedImprovementInPlot(pAdjacentPlot, sState);

			if (eAdjacentImprovement != NO_IMPROVEMENT)
				continue;

			CvCity* pAdjacentOwningCity = pAdjacentPlot->getOwningCity();

			int iBestImprovementScore = 0;

			for (int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
			{
				BuildTypes eOtherBuild = (BuildTypes)iBuild;

				if (eOtherBuild == NO_BUILD)
					continue;

				CvBuildInfo* pkOtherBuildInfo = GC.getBuildInfo(eOtherBuild);
				if (!pkOtherBuildInfo)
					continue;

				ImprovementTypes eOtherImprovement = (ImprovementTypes)pkOtherBuildInfo->getImprovement();

				if (eOtherImprovement == NO_IMPROVEMENT && pkOtherBuildInfo->isRepair() && pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT && pAdjacentPlot->IsImprovementPillaged())
					eOtherImprovement = pAdjacentPlot->getImprovementType();

				if (eOtherImprovement == NO_IMPROVEMENT)
					continue;

				CvImprovementEntry* pkOtherImprovementInfo = GC.getImprovementInfo(eOtherImprovement);
				if (pkOtherImprovementInfo->IsCreatedByGreatPerson() && !pkOtherBuildInfo->isRepair())
					continue;

				if (!m_pPlayer->canBuild(pAdjacentPlot, eOtherBuild, true))
					continue;

				int iImprovementScore = 0;

				for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
				{
					YieldTypes eYield = (YieldTypes)iYield;

					if (eYield > YIELD_CULTURE_LOCAL)
						break;

					int iYieldModifier = GetYieldBaseModifierTimes100(eYield);
					iYieldModifier *= pAdjacentOwningCity ? GetYieldCityModifierTimes100(pAdjacentOwningCity, m_pPlayer, eYield) : 100;

					if (pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eOtherImprovement) != 0)
						iImprovementScore += ((pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eOtherImprovement) * iYieldModifier) / 100).Truncate();

					if (pkOtherImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eImprovement) != 0)
						iImprovementScore += ((pkOtherImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eImprovement) * iYieldModifier) / 100).Truncate();
				}

				if (iImprovementScore > iBestImprovementScore)
				{
					iBestImprovementScore = iImprovementScore;
					bestPotentialImprovementInDirection[iDirection] = eOtherImprovement;
				}
			}
		}
	}

	for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		YieldTypes eYield = (YieldTypes)ui;

		// Simplification - errata yields not worth considering.
		if (eYield > YIELD_CULTURE_LOCAL)
			break;

		const CvYieldInfo& kYield = *GC.getYieldInfo(eYield);
		int iGoldenAgeYieldThresholdTimes100 = kYield.getGoldenAgeYieldThreshold() * 100;

		int iYieldModifier = GetYieldBaseModifierTimes100(eYield);
		int iNewYieldTimes100 = 0;
		int iProjectedNewYieldsTimes100 = eBuild != NO_BUILD ? 100 * m_aiProjectedPlotYields[ui] : 100 * m_aiCurrentPlotYields[ui];

		if (bIsWithinWorkRange)
		{
			iNewYieldTimes100 = iProjectedNewYieldsTimes100;

			// Normalize Golden Age gold yields (assume active 40% of the time)
			if (iNewYieldTimes100 >= iGoldenAgeYieldThresholdTimes100)
			{
				int iGoldenAgeYield = kYield.getGoldenAgeYield();
				if (iGoldenAgeYield != 0)
				{
					if (m_pPlayer->getGoldenAgeTurns() == 0)
						iNewYieldTimes100 += 40 * iGoldenAgeYield;
					else
						iNewYieldTimes100 -= 60 * iGoldenAgeYield;
				}
			}

			if (pkImprovementInfo)
			{
				// Assume we love the king day is active 85% of the time
				int iWLTKDYieldChange = pkImprovementInfo->GetWLTKDYieldChange(eYield);
				if (iWLTKDYieldChange != 0)
				{
					if (pOwningCity && pOwningCity->GetWeLoveTheKingDayCounter() == 0)
						iNewYieldTimes100 += 85 * iWLTKDYieldChange;
					else
						iNewYieldTimes100 -= 15 * iWLTKDYieldChange;
				}

				// Assume golden ages are active 40% of the time
				int iGoldenAgeYieldChange = pkImprovementInfo->GetGoldenAgeYieldChange(eYield);
				if (iGoldenAgeYieldChange != 0)
				{
					if (m_pPlayer->getGoldenAgeTurns() == 0)
						iNewYieldTimes100 += 40 * iGoldenAgeYieldChange;
					else
						iNewYieldTimes100 -= 60 * iGoldenAgeYieldChange;
				}
			}

			// If we are creating a feature, check if city gets any extra yield gains
			// TODO how to handle yield per X worked plots with feature Y...
			/*if (eFeatureFromImprovement != NO_FEATURE && eFeatureFromImprovement != eFeature)
			{
				iNewYieldTimes100 += pOwningCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeatureFromImprovement, eYield);

				if (pCityReligion)
					iNewYieldTimes100 += pCityReligion->m_Beliefs.GetYieldPerXFeatureTimes100(eFeatureFromImprovement, eYield);
			}

			// If we are removing a feature, check if city gets any extra yield losses
			if (eFeature != NO_FEATURE && pkBuildInfo && pkBuildInfo->isFeatureRemove(eFeature) && bIsWorkingPlot)
			{
				iNewYieldTimes100 -= pOwningCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield);

				if (pCityReligion)
					iNewYieldTimes100 -= pCityReligion->m_Beliefs.GetYieldPerXFeatureTimes100(eFeature, eYield);
			}*/

			// if we are in a modified state, we may want to change the yields in the current plot based on planned adjacent improvements
			if (MOD_BALANCE_VP && eImprovement != NO_IMPROVEMENT)
			{
				CvPlot** pAdjacentPlots = GC.getMap().getNeighborsUnchecked(pPlot->GetPlotIndex());
				fraction fNewAdjacencyYield = 0;
				fraction fOldAdjacencyYield = 0;

				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot* pAdjacentPlot = pAdjacentPlots[iI];

					if (!pAdjacentPlot)
						continue;

					FeatureTypes eOldAdjacentFeature = pAdjacentPlot->getFeatureType();
					FeatureTypes eNewAdjacentFeature = GetPlannedFeatureInPlot(pAdjacentPlot, sState);
					// The feature in an adjacent plot has been changed
					if (eNewAdjacentFeature != eOldAdjacentFeature)
					{

						if (eOldAdjacentFeature != NO_FEATURE && pkImprovementInfo->GetAdjacentFeatureYieldChanges(eOldAdjacentFeature, eYield) > 0)
						{
							iNewYieldTimes100 -= 100 * pkImprovementInfo->GetAdjacentFeatureYieldChanges(eOldAdjacentFeature, eYield);
						}
						if (eNewAdjacentFeature != NO_FEATURE && pkImprovementInfo->GetAdjacentFeatureYieldChanges(eNewAdjacentFeature, eYield) > 0)
						{
							iNewYieldTimes100 += 100 * pkImprovementInfo->GetAdjacentFeatureYieldChanges(eNewAdjacentFeature, eYield);
						}
					}

					const ImprovementTypes eOldAdjacentImprovement = !pAdjacentPlot->IsImprovementPillaged() ? pAdjacentPlot->getImprovementType() : NO_IMPROVEMENT;
					const ImprovementTypes eNewAdjacentImprovement = GetPlannedImprovementInPlot(pAdjacentPlot, sState);

					// The improvement in an adjacent plot has been changed
					if (eNewAdjacentImprovement != eOldAdjacentImprovement)
					{
						// Bonuses an adjacent improvement would give/gave to this improvement
						if (eOldAdjacentImprovement != NO_IMPROVEMENT && pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eOldAdjacentImprovement) != 0)
						{
							fOldAdjacencyYield += pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eOldAdjacentImprovement);
						}
						if (eNewAdjacentImprovement != NO_IMPROVEMENT && pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eNewAdjacentImprovement) != 0)
						{
							fNewAdjacencyYield += pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eNewAdjacentImprovement);
						}

						CvImprovementEntry* pkOldAdjacentImprovementInfo = eOldAdjacentImprovement != NO_IMPROVEMENT ? GC.getImprovementInfo(eOldAdjacentImprovement) : NULL;
						CvImprovementEntry* pkNewAdjacentImprovementInfo = eNewAdjacentImprovement != NO_IMPROVEMENT ? GC.getImprovementInfo(eNewAdjacentImprovement) : NULL;

						ResourceTypes eResourceFromOldAdjacentImprovement = pkOldAdjacentImprovementInfo ? (ResourceTypes)pkOldAdjacentImprovementInfo->GetResourceFromImprovement() : NO_RESOURCE;
						ResourceTypes eResourceFromNewAdjacentImprovement = pkNewAdjacentImprovementInfo ? (ResourceTypes)pkNewAdjacentImprovementInfo->GetResourceFromImprovement() : NO_RESOURCE;

						// Bonuses we would get based on adjacent a resource created by an adjacent improvement
						if (eResourceFromOldAdjacentImprovement != NO_RESOURCE && pkImprovementInfo->GetAdjacentResourceYieldChanges(eResourceFromOldAdjacentImprovement, eYield) > 0)
						{
							iNewYieldTimes100 -= 100 * pkImprovementInfo->GetAdjacentResourceYieldChanges(eResourceFromOldAdjacentImprovement, eYield);
						}
						if (eResourceFromNewAdjacentImprovement != NO_RESOURCE && pkImprovementInfo->GetAdjacentResourceYieldChanges(eResourceFromNewAdjacentImprovement, eYield) > 0)
						{
							iNewYieldTimes100 += 100 * pkImprovementInfo->GetAdjacentResourceYieldChanges(eResourceFromNewAdjacentImprovement, eYield);
						}
					}
				}

				int iTruncatedDeltaYield = fNewAdjacencyYield.Truncate() - fOldAdjacencyYield.Truncate();
				if (iTruncatedDeltaYield != 0)
				{
					iNewYieldTimes100 += 100 * iTruncatedDeltaYield;
				}
			}

			// Normalize Golden Age gold yields (assume active 40% of the time)
			// Yield was below the threshold when we calculated the yields, but above with the extra yields from planned adjacenct builds
			if (iNewYieldTimes100 >= iGoldenAgeYieldThresholdTimes100 && iProjectedNewYieldsTimes100 < iGoldenAgeYieldThresholdTimes100)
			{
				int iGoldenAgeYield = kYield.getGoldenAgeYield();
				if (iGoldenAgeYield != 0)
				{
					iNewYieldTimes100 += 40 * iGoldenAgeYield;
				}
			}
		}

		// Special handling for vanilla celts
		if (!MOD_BALANCE_VP && eYield == YIELD_FAITH && m_pPlayer->GetPlayerTraits()->IsFaithFromUnimprovedForest())
		{
			if (eFeature == FEATURE_FOREST && pPlot->getImprovementType() == NO_IMPROVEMENT && (eImprovement != NO_IMPROVEMENT || (pkBuildInfo && pkBuildInfo->isFeatureRemove(eFeature))))
			{
				CvCity* pNextCity = pPlot->GetAdjacentCity();
				if (pNextCity && pNextCity->getOwner() == m_pPlayer->GetID())
				{
					int iAdjacentForests = 0;

					for (int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pCityAdjacentPlot = plotDirection(pNextCity->getX(), pNextCity->getY(), ((DirectionTypes)iDirectionLoop));

						FeatureTypes eAdjacentFeature = GetPlannedFeatureInPlot(pCityAdjacentPlot, sState);
						ImprovementTypes eAdjacentImprovement = GetPlannedImprovementInPlot(pCityAdjacentPlot, sState);

						if (pCityAdjacentPlot && eAdjacentFeature == eFeature && eAdjacentImprovement == NO_IMPROVEMENT)
						{
							iAdjacentForests++;
						}
					}

					if (iAdjacentForests == 3 || (iAdjacentForests == 2 && MOD_ALTERNATE_CELTS) || iAdjacentForests == 1)
					{
						iNewYieldTimes100 -= (100 * GetYieldBaseModifierTimes100(eYield)) / GetYieldBaseModifierTimes100(YIELD_GOLD);
					}
				}
			}
		}

		// Bonus yields that this improvement provides to adjacent improvements
		if (MOD_BALANCE_VP)
		{
			CvPlot** pAdjacentPlots = GC.getMap().getNeighborsUnchecked(pPlot->GetPlotIndex());
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = pAdjacentPlots[iI];

				if (!pAdjacentPlot)
					continue;

				if (pAdjacentPlot->isCity())
					continue;

				if (pAdjacentPlot->getOwner() != m_pPlayer->GetID())
					continue;

				bool bAdjacentIsWorkable = pAdjacentPlot->isPlayerCityRadius(m_pPlayer->GetID());
				if (!bAdjacentIsWorkable)
					continue;

				CvCity* pAdjacentOwningCity = pAdjacentPlot->getOwningCity();
				if (!pAdjacentOwningCity || pAdjacentOwningCity->IsRazing())
					continue;

				int iNewAdjacentWorkedYield = 0;
				int iNewAdjacentUnworkedYield = 0;

				bool bWorkingAdjacent = pAdjacentOwningCity->GetCityCitizens()->IsWorkingPlot(pAdjacentPlot);

				// How much extra yield we give to adjacent tiles with a certain terrain
				if (pAdjacentPlot->getTerrainType() != NO_TERRAIN)
				{
					int iAdjacentTerrainYieldChange = pkImprovementInfo ? pkImprovementInfo->GetAdjacentTerrainYieldChanges(pAdjacentPlot->getTerrainType(), eYield) : 0;
					if (iAdjacentTerrainYieldChange != 0)
						if (bWorkingAdjacent)
							iNewAdjacentWorkedYield += iAdjacentTerrainYieldChange;
						else
							iNewAdjacentUnworkedYield += iAdjacentTerrainYieldChange;
				}

				ImprovementTypes eAdjacentImprovement = GetPlannedImprovementInPlot(pAdjacentPlot, sState);

				if (eAdjacentImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkAdjacentImprovementInfo = GC.getImprovementInfo(eAdjacentImprovement);

					if (pkAdjacentImprovementInfo)
					{
						// How much extra yield we give to adjacent tiles with a certain improvement
						fraction fAdjacentImprovementYield = pkAdjacentImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eImprovement);
						fraction fCurrentAdjacentImprovementYield = GetCurrentAdjacencyScoreFromImprovementsAndTerrain(pAdjacentPlot, *pkAdjacentImprovementInfo, eYield, sState);

						if (eBuild == NO_BUILD)
							fCurrentAdjacentImprovementYield -= fAdjacentImprovementYield;

						int iDeltaTruncatedYield = (fCurrentAdjacentImprovementYield + fAdjacentImprovementYield).Truncate() - fCurrentAdjacentImprovementYield.Truncate();
						if (iDeltaTruncatedYield != 0)
							if (bWorkingAdjacent)
								iNewAdjacentWorkedYield += iDeltaTruncatedYield;
							else
								iNewAdjacentUnworkedYield += iDeltaTruncatedYield;

						// How much extra yield an adjacent improvement will get if we create a resource
						if (eResourceFromImprovement != NO_RESOURCE)
						{
							int iAdjacentResourceYieldChanges = pkAdjacentImprovementInfo->GetAdjacentResourceYieldChanges(eResourceFromImprovement, eYield);
							if (iAdjacentResourceYieldChanges != 0)
								if (bWorkingAdjacent)
									iNewAdjacentWorkedYield += iAdjacentResourceYieldChanges;
								else
									iNewAdjacentUnworkedYield += iAdjacentResourceYieldChanges;
						}

						// How much extra yield an adjacent improvement will get if we create a feature (or keep the one that's here)
						if (eFeatureFromImprovement != NO_FEATURE || (eFeature != NO_FEATURE && (!pkBuildInfo || !pkBuildInfo->isFeatureRemove(eFeature))))
						{
							FeatureTypes eNewFeature = eFeatureFromImprovement != NO_FEATURE ? eFeatureFromImprovement : eFeature;
							int iAdjacentFeatureYieldChanges = pkAdjacentImprovementInfo->GetAdjacentFeatureYieldChanges(eNewFeature, eYield);
							if (iAdjacentFeatureYieldChanges != 0)
								if (bWorkingAdjacent)
									iNewAdjacentWorkedYield += iAdjacentFeatureYieldChanges;
								else
									iNewAdjacentUnworkedYield += iAdjacentFeatureYieldChanges;
						}
					}
				}

				if (iNewAdjacentWorkedYield != 0 || iNewAdjacentUnworkedYield != 0)
				{
					int iAdjacentCityYieldModifier = pAdjacentOwningCity ? GetYieldCityModifierTimes100(pAdjacentOwningCity, m_pPlayer, eYield) : 100;
					iSecondaryScore += (iNewAdjacentWorkedYield * iYieldModifier * iAdjacentCityYieldModifier) / 100;
					iPotentialScore += (iNewAdjacentUnworkedYield * iYieldModifier * iAdjacentCityYieldModifier) / 100;
				}
			}
		}

		if (iNewYieldTimes100 != 0)
		{
			int iCityYieldModifier = pOwningCity ? GetYieldCityModifierTimes100(pOwningCity, m_pPlayer, eYield) : 100;
			iYieldScore += (iNewYieldTimes100 * iYieldModifier * iCityYieldModifier) / 10000;
		}

		// Extra adjacency bonuses from potential adjacent same improvements
		if (MOD_BALANCE_VP && bIsWithinWorkRange && eImprovement != NO_IMPROVEMENT)
		{
			int iPotentialNewYieldTimes100 = 0;

			fraction fCurrentBonusToThisTile = 0;
			fraction fPotentialBonusToThisTile = 0;
			fraction fPotentialBonusToAdjacentTiles = 0;

			CvPlot** pAdjacentPlots = GC.getMap().getNeighborsUnchecked(pPlot->GetPlotIndex());
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = pAdjacentPlots[iI];

				if (!pAdjacentPlot)
					continue;

				TerrainTypes eAdjacentTerrain = pAdjacentPlot->getTerrainType();
				fraction fYieldPerXAdjacentTerrain = pkImprovementInfo->GetYieldPerXAdjacentTerrain(eYield, eAdjacentTerrain);
				if (fYieldPerXAdjacentTerrain != 0)
				{
					fCurrentBonusToThisTile += fYieldPerXAdjacentTerrain;
				}

				const ImprovementTypes eAdjacentImprovement = GetPlannedImprovementInPlot(pAdjacentPlot, sState);

				if (eAdjacentImprovement != NO_IMPROVEMENT)
				{
					fraction fYieldPerXAdjacentImprovement = pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eAdjacentImprovement);
					if (fYieldPerXAdjacentImprovement != 0)
					{
						fCurrentBonusToThisTile += fYieldPerXAdjacentImprovement;
					}
					continue;
				}

				ImprovementTypes eBestPotentialAdjacentImprovement = bestPotentialImprovementInDirection[iI];
				if (eBestPotentialAdjacentImprovement == NO_IMPROVEMENT)
					continue;

				fraction fBestBonusToAdjacentTile = 0;
				fraction fBestBonusToThisTile = 0;

				if (pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eBestPotentialAdjacentImprovement) != 0)
				{
					fPotentialBonusToThisTile += pkImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eBestPotentialAdjacentImprovement);
				}
				
				CvImprovementEntry* pkBestPotentialAdjacentImprovementInfo = GC.getImprovementInfo(eBestPotentialAdjacentImprovement);
				
				if (pkBestPotentialAdjacentImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eImprovement) != 0)
				{
					fPotentialBonusToAdjacentTiles += pkBestPotentialAdjacentImprovementInfo->GetYieldPerXAdjacentImprovement(eYield, eImprovement);
				}
			}

			int iDeltaTruncatedYieldBonusThisTile = (fPotentialBonusToThisTile + fCurrentBonusToThisTile).Truncate() - fCurrentBonusToThisTile.Truncate();

			if (iDeltaTruncatedYieldBonusThisTile != 0)
				iPotentialNewYieldTimes100 += 100 * iDeltaTruncatedYieldBonusThisTile;

			// Yield was below the threshold, but including potential adjacent improvements we get above the threshold
			if (iNewYieldTimes100 + iPotentialNewYieldTimes100 >= iGoldenAgeYieldThresholdTimes100 && iNewYieldTimes100 < iGoldenAgeYieldThresholdTimes100)
			{
				int iGoldenAgeYield = kYield.getGoldenAgeYield();
				if (iGoldenAgeYield != 0)
				{
					iPotentialNewYieldTimes100 += 40 * iGoldenAgeYield;
				}
			}

			if (fPotentialBonusToAdjacentTiles > 0)
			{
				iPotentialNewYieldTimes100 += (fPotentialBonusToAdjacentTiles * 100).Truncate();
			}

			if (iPotentialNewYieldTimes100 != 0)
			{
				int iCityYieldModifier = pOwningCity ? GetYieldCityModifierTimes100(pOwningCity, m_pPlayer, eYield) : 100;
				// Assume 2/3 of potential will happen (divide by 15000 instead of 10000)
				iPotentialScore += (iPotentialNewYieldTimes100 * iYieldModifier * iCityYieldModifier) / 15000;
			}
		}
	}

	//Improvement grants or connects resource
	//Don't give any bonus if we are creating a resource on top of an existing one
	bool bCreatesResource = eResourceFromImprovement != NO_RESOURCE;
	bool bConnectsResource = eResource != NO_RESOURCE && pkImprovementInfo && pkImprovementInfo->IsConnectsResource(eResource);
	if (pPlot->getOwner() == m_pPlayer->GetID())
	{
		if (bCreatesResource || bConnectsResource)
		{
			ResourceTypes eConnectedResource = bCreatesResource ? eResourceFromImprovement : eResource;
			int iResourceAmount = eResourceFromImprovement != NO_RESOURCE ? pkImprovementInfo->GetResourceQuantityFromImprovement() : pPlot->getNumResource();
			int iResourceWeight = GetResourceWeight(eConnectedResource, iResourceAmount);

			// If the old improvement granted the same resource, subtract the resource from this improvement
			ImprovementTypes eOldImprovement = !pPlot->IsImprovementPillaged() ? pPlot->getImprovementType() : NO_IMPROVEMENT;
			if (eOldImprovement != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkOldImprovementInfo = GC.getImprovementInfo(eOldImprovement);
				if (pkOldImprovementInfo && (pkOldImprovementInfo->IsConnectsResource(eResource) || pkOldImprovementInfo->GetResourceFromImprovement() == eResource))
					iExtraResource -= iResourceAmount;
			
			}

			int iNumResourceAvailable = m_pPlayer->getNumResourceAvailable(eConnectedResource) + iExtraResource;

			iSecondaryScore += iResourceWeight;

			// give a priority reduction if we have this resource already
			int iDownPrio = 0;
			// We have plenty to spare
			if (iNumResourceAvailable > 1)
				iDownPrio = (iResourceWeight * 2) / 3;
			// We have one already
			else if (iNumResourceAvailable > 0)
				iDownPrio = iResourceWeight / 3;

			if (iDownPrio > 0)
			{
				iSecondaryScore -= iDownPrio;
				iPotentialScore += iDownPrio;
			}

			CvResourceInfo* pkConnectedResource = GC.getResourceInfo(eConnectedResource);
			//amp up monopoly alloc!
			if (pkConnectedResource && pkConnectedResource->isMonopoly() && !(pkConnectedResource->getResourceUsage() == RESOURCEUSAGE_LUXURY && GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(m_pPlayer->GetID(), eConnectedResource)))
			{
				int iOwnedNumResource = m_pPlayer->getNumResourceTotal(eConnectedResource, false) + m_pPlayer->getResourceExport(eConnectedResource);

				int iResourceFromMinors = 0;
				if (m_pPlayer->IsCSResourcesCountMonopolies())
				{
					iResourceFromMinors += m_pPlayer->getResourceFromMinors(eConnectedResource);
				}

				int iResourceFromImports = 0;
				if (m_pPlayer->GetPlayerTraits()->IsImportsCountTowardsMonopolies())
				{
					iResourceFromImports += m_pPlayer->getResourceImportFromMajor(eConnectedResource);
				}
				int iTotalNumResource = GC.getMap().getNumResources(eConnectedResource);
				if (bCreatesResource)
					iTotalNumResource += iResourceAmount;

				int iCurrentMonopolyPercent = 0;
				int iFutureMonopolyPercent = 0;
				if (iTotalNumResource <= 0)
				{
					iCurrentMonopolyPercent = iOwnedNumResource + iResourceFromMinors + iResourceFromImports + iExtraResource > 0 ? 100 : 0;
					iFutureMonopolyPercent = iOwnedNumResource + iResourceFromMinors + iResourceFromImports + iExtraResource + iResourceAmount > 0 ? 100 : 0;
				}
				else
				{
					iCurrentMonopolyPercent = ((iOwnedNumResource + iResourceFromMinors + iResourceFromImports + iExtraResource) * 100) / iTotalNumResource;
					iFutureMonopolyPercent = ((iOwnedNumResource + iResourceFromMinors + iResourceFromImports + iExtraResource + iResourceAmount) * 100) / iTotalNumResource;
				}

				int iGlobalThreshold = /*50*/ GD_INT_GET(GLOBAL_RESOURCE_MONOPOLY_THRESHOLD);
				int iStrategicThreshold = 0;
				if (pkConnectedResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
					iStrategicThreshold = /*25*/ GD_INT_GET(STRATEGIC_RESOURCE_MONOPOLY_THRESHOLD);

				bool bCanReachGlobalMonopoly = iFutureMonopolyPercent > iGlobalThreshold || m_pPlayer->GetPlayerTraits()->IsImportsCountTowardsMonopolies() || (100 * (m_pPlayer->getResourceInOwnedPlots(eConnectedResource) + iResourceFromMinors) > iGlobalThreshold * iTotalNumResource);
				bool bCanReachStrategicMonopoly = iStrategicThreshold != 0 && (iFutureMonopolyPercent > iStrategicThreshold || m_pPlayer->GetPlayerTraits()->IsImportsCountTowardsMonopolies() || (100 * (m_pPlayer->getResourceInOwnedPlots(eConnectedResource) + iResourceFromMinors) > iStrategicThreshold * iTotalNumResource));

				if (iCurrentMonopolyPercent <= iGlobalThreshold)
				{
					if (iFutureMonopolyPercent > iGlobalThreshold)
					{
						// Improving this will give us a global monopoly
						iSecondaryScore += 2000;
					}
					else if (iStrategicThreshold != 0 && iCurrentMonopolyPercent <= iStrategicThreshold && iFutureMonopolyPercent > iStrategicThreshold)
					{
						// Improving this will give us a strategic monopoly
						iSecondaryScore += 1000;
						if (bCanReachGlobalMonopoly)
							iPotentialScore += 1000;
					}
					else if (bCanReachGlobalMonopoly)
					{
						// We can get a global monopoly eventually
						iPotentialScore += 2000;
					}
					else if (bCanReachStrategicMonopoly)
					{
						iPotentialScore += 1000;
					}
				}
			}
		}
	}

	if (MOD_BALANCE_VP)
	{
		bool bBenefitsFromRoads = false;
		for (int iI = 0; iI <= YIELD_FAITH; iI++)
		{
			if (pkImprovementInfo && pkImprovementInfo->GetRouteYieldChanges(ROUTE_ROAD, iI) > 0)
			{
				bBenefitsFromRoads = true;
				break;
			}
		}

		int iPenalty = pkImprovementInfo && pkImprovementInfo->IsCreatedByGreatPerson() ? 1000 : 100;

		if (bBenefitsFromRoads && eForceCityConnection == NO_ROUTE)
			iSecondaryScore -= iPenalty;
		else if (!bBenefitsFromRoads && (eForceCityConnection == ROUTE_ROAD || eForceCityConnection == ROUTE_RAILROAD))
			iSecondaryScore -= iPenalty;
	}

	// Don't build on antiquity sites
	if (eResource != NO_RESOURCE && eBuild != NO_BUILD)
	{
		static const BuildTypes eDigBuild = (BuildTypes)GC.getInfoTypeForString("BUILD_ARCHAEOLOGY_DIG");
		if (m_pPlayer->canBuild(pPlot, eDigBuild) && eDigBuild != eBuild)
			iSecondaryScore -= 1000;
	}

#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
	// improvement spawns resource?
	if (MOD_IMPROVEMENTS_EXTENSIONS && pOwningCity)
	{
		int iResourceChance = pkImprovementInfo ? pkImprovementInfo->GetRandomResourceChance() : 0;
		if (iResourceChance > 0 && pPlot->getResourceType(m_pPlayer->getTeam()) == NO_RESOURCE)
		{
			iSecondaryScore += 5 * iResourceChance;
		}

		// if it's Iroquois building forests give it even more weight since it connects cities.
		if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeatureFromImprovement == FEATURE_FOREST || eFeatureFromImprovement == FEATURE_JUNGLE))
		{
			iYieldScore = (iYieldScore * 3) / 2;
		}
	}
#endif

	//Is this a good spot for a defensive building?
	bool bNewIsDefensive = pkImprovementInfo && (pkImprovementInfo->GetDefenseModifier() > 0 || pkImprovementInfo->GetNearbyEnemyDamage() > 0);
	if (bNewIsDefensive)
	{
		int iDefenseBuildValue = pPlot->GetDefenseBuildValue(m_pPlayer->GetID(), eBuild, eImprovement, sState);
		if (iDefenseBuildValue != 0)
			iSecondaryScore += iDefenseBuildValue;
	}

	// How many tiles will be covered by an encampment bonus (or similar)
	CvPlayerTraits* pTraits = m_pPlayer->GetPlayerTraits();
	if (pTraits->GetCombatBonusImprovementType() == eImprovement)
	{
		int iBonusRange = pTraits->GetNearbyImprovementBonusRange();
		int iCombatBonusValue = pTraits->GetNearbyImprovementCombatBonus();
		for (int iI = 0; iI < RING_PLOTS[iBonusRange]; iI++)
		{
			CvPlot* pCoveredPlot = iterateRingPlots(pPlot, iI);
			if (!pCoveredPlot || pCoveredPlot->getOwner() != m_pPlayer->GetID())
				continue;

			// Check if any adjacent tile covers this one already
			bool bTileAlreadyCovered = false;
			for (int iJ = 0; iJ < RING_PLOTS[iBonusRange]; iJ++)
			{
				CvPlot* pPotentialOtherImprovementPlot = iterateRingPlots(pCoveredPlot, iJ);
				if (!pPotentialOtherImprovementPlot || pPotentialOtherImprovementPlot->getOwner() != m_pPlayer->GetID())
					continue;

				const ImprovementTypes eOtherImprovement = GetPlannedImprovementInPlot(pPotentialOtherImprovementPlot, sState);

				if (eOtherImprovement == eImprovement)
				{
					bTileAlreadyCovered = true;
					break;
				}
			}

			// If we are giving 20 bonus combat damage to 18 tiles (maximum in two rings), this gives 720 value
			if (!bTileAlreadyCovered)
				iSecondaryScore += iCombatBonusValue * 2;
		}
	}

	// Do we want a canal here?
	if (MOD_GLOBAL_PASSABLE_FORTS && WantCanalAtPlot(pPlot))
	{
		if (pkImprovementInfo && pkImprovementInfo->IsMakesPassable())
			iSecondaryScore += 3000;
	}

	// Currently this is only for human player recommendations.
	if (bIsCultureBomb && eBuild != NO_BUILD)
	{
		int iStealScore = 0;
		int iRange = pkImprovementInfo->GetCultureBombRadius();
		bool bStealsEnemyLand = false;

		// Culture bombs need to be quite good to be worth it
		if (pkImprovementInfo->IsCreatedByGreatPerson())
			iSecondaryScore -= 2000;

		for (int iI = 0; iI < RING_PLOTS[iRange]; iI++)
		{
			CvPlot* pAdjacentPlot = iterateRingPlots(pPlot, iI);

			if (!pAdjacentPlot)
				continue;

			PlayerTypes eOwner = pAdjacentPlot->getOwner();

			if (eOwner == m_pPlayer->GetID())
				continue;

			if (pAdjacentPlot->IsStealBlockedByImprovement())
				continue;

			if (m_pPlayer->isMajorCiv() && m_pPlayer->GetDiplomacyAI()->IsBadTheftTarget(eOwner, THEFT_TYPE_CULTURE_BOMB))
			{
				iStealScore = -10000;
				break;
			}
			else if (m_pPlayer->isMinorCiv())
			{
				if (m_pPlayer->GetMinorCivAI()->IsFriends(eOwner) || m_pPlayer->GetMinorCivAI()->IsProtectedByMajor(eOwner))
				{
					iStealScore = -10000;
					break;
				}
				PlayerTypes eAlly = m_pPlayer->GetMinorCivAI()->GetAlly();
				if (eOwner != NO_PLAYER && eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eOwner).getTeam())
				{
					iStealScore = -10000;
					break;
				}
			}

			if (pAdjacentPlot->isImpassable(m_pPlayer->getTeam()))
				continue;

			int iMultiplier = 1;
			int iTempScore = 10;

			if (pAdjacentPlot->IsChokePoint())
			{
				iTempScore += 50;
				if (eOwner != NO_PLAYER)
					iTempScore += 50;
			}

			// There's a resource on the plot
			ResourceTypes eResource = pAdjacentPlot->getResourceType(m_pPlayer->getTeam());
			if (eResource != NO_RESOURCE)
			{
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();
				if (eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
				{
					iTempScore += 50;
					if (eOwner != NO_PLAYER)
						iTempScore += 50;
				}
			}

			// These only apply when we're stealing from other players
			if (eOwner != NO_PLAYER)
			{
				bStealsEnemyLand = true;

				if (GET_PLAYER(eOwner).isMinorCiv())
				{
					iMultiplier = 3;

					// We're stealing someone else's embassy
					if (pAdjacentPlot->IsImprovementEmbassy())
					{
						// Not our own!
						if (GET_PLAYER(pAdjacentPlot->GetPlayerThatBuiltImprovement()).getTeam() == m_pPlayer->getTeam())
						{
							iStealScore = -10000;
							break;
						}

						// Or a bad target's!
						if (m_pPlayer->GetDiplomacyAI()->IsBadTheftTarget(pAdjacentPlot->GetPlayerThatBuiltImprovement(), THEFT_TYPE_EMBASSY))
						{
							iStealScore = -10000;
							break;
						}

						iTempScore += 300;
					}
				}
				// For major civs (and barbarians), it depends on how much we hate them
				else
				{
					CivOpinionTypes eOpinion = m_pPlayer->GetDiplomacyAI()->GetCivOpinion(eOwner);
					if (eOpinion < CIV_OPINION_NEUTRAL)
						iMultiplier = 8;
					else if (eOpinion == CIV_OPINION_NEUTRAL)
						iMultiplier = 4;
				}

				// We're stealing a defensive plot or Great Person improvement
				ImprovementTypes eAdjacentImprovement = pAdjacentPlot->getImprovementType();
				if (eAdjacentImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkAdjacentImprovementInfo = GC.getImprovementInfo(eAdjacentImprovement);
					if (pkAdjacentImprovementInfo->IsNoFollowUp())
						iTempScore += 20;

					if (pkAdjacentImprovementInfo->IsCreatedByGreatPerson())
						iTempScore += 50;

					iTempScore += pkAdjacentImprovementInfo->GetDefenseModifier();
				}

				// Finally we tie break with yields (only a rough count)
				for (int iYield = 0; iYield < YIELD_JFD_HEALTH; iYield++)
				{
					YieldTypes eYield = static_cast<YieldTypes>(iYield);
					iTempScore += pAdjacentPlot->getYield(eYield);
				}
			}

			if (iTempScore != 0 && iMultiplier != 0 && bStealsEnemyLand)
				iStealScore += iTempScore * iMultiplier * 10;
		}

		if (iStealScore != 0)
		{
			iSecondaryScore += iStealScore;
		}
	}

	const int iImprovementMaintenanceTimes100 = pkImprovementInfo ? pkImprovementInfo->GetGoldMaintenance() * (100 + m_pPlayer->GetImprovementGoldMaintenanceMod()) : 0;
	iSecondaryScore -= iImprovementMaintenanceTimes100;

	ResourceTypes eSpawnedResource = pkImprovementInfo ? pkImprovementInfo->SpawnsAdjacentResource() : NO_RESOURCE;

	if (eSpawnedResource != NO_RESOURCE)
	{
		int iTileClaimChance = 0;
		int iTileWorkableChance = 0;
		if (eBuild != NO_BUILD)
		{
			iTileWorkableChance = GetResourceSpawnWorkableChance(pPlot, iTileClaimChance);
		}
		else
		{
			CvPlot* pSpawnedPlot = pPlot->GetSpawnedResourcePlot();
			if (pSpawnedPlot && pSpawnedPlot->isPlayerCityRadius(m_pPlayer->GetID()))
				iTileWorkableChance = 100;
		}

		if (iTileClaimChance > 0)
		{
			iSecondaryScore += 3 * iTileClaimChance;
		}

		if (iTileWorkableChance > 0)
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				YieldTypes eYield = (YieldTypes)iI;

				if (eYield > YIELD_CULTURE_LOCAL)
					break;

				int iYieldChange = GC.getResourceInfo(eSpawnedResource)->getYieldChange(eYield);

				if (iYieldChange != 0)
				{
					int iYieldModifier = GetYieldBaseModifierTimes100(eYield);
					int iCityYieldModifier = pOwningCity ? GetYieldCityModifierTimes100(pOwningCity, m_pPlayer, eYield) : 100;
					iYieldScore += (iYieldChange * iTileWorkableChance * iYieldModifier * iCityYieldModifier) / 10000;
				}
			}
		}
	}

	if (bIsWithinWorkRange && pOwningCity)
	{
		int iSimplifiedYieldValue = GetPlotYieldValueSimplified(pPlot, eBuild);
		int iWorstWorkedValue = INT_MAX;

		int iLoopCity;
		for (const CvCity* pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			if (pLoopCity != pOwningCity && pLoopCity->IsPuppet())
				continue;

			if (!pLoopCity->IsWithinWorkRange(pPlot))
				continue;

			int iWorstCityWorkedValue = GetWorstWorkedPlotValue(pLoopCity);

			if (iWorstCityWorkedValue < iWorstWorkedValue)
				iWorstWorkedValue = iWorstCityWorkedValue;
		}

		if (iSimplifiedYieldValue <= iWorstWorkedValue)
		{
			int iPenalty = iYieldScore / 2;
			iYieldScore -= iPenalty;
			iPotentialScore += iPenalty;
		}
	}

	return make_pair(iYieldScore + iSecondaryScore, iPotentialScore);
}

int CvBuilderTaskingAI::GetResourceSpawnWorkableChance(CvPlot* pPlot, int& iTileClaimChance)
{
	vector<CvPlot*> aPossibleSpawnPlots;
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), (DirectionTypes)iI);
		if (!pAdjacentPlot)
			continue;

		if (!pAdjacentPlot->CanSpawnResource(m_pPlayer->GetID(), /*bIgnoreTech*/ false))
			continue;

		aPossibleSpawnPlots.push_back(pAdjacentPlot);
	}

	if (aPossibleSpawnPlots.empty())
	{
		iTileClaimChance = 0;
		return 0;
	}

	int iWorkableSpawnPlots = 0;
	int iNonWorkableSpawnPlots = 0;
	int iClaimTiles = 0;
	int iNonClaimTiles = 0;

	for (vector<CvPlot*>::const_iterator it = aPossibleSpawnPlots.begin(); it != aPossibleSpawnPlots.end(); ++it)
	{
		CvPlot* pPossibleSpawnPlot = *it;

		if (pPossibleSpawnPlot->isPlayerCityRadius(m_pPlayer->GetID()) && (pPossibleSpawnPlot->getOwner() == NO_PLAYER || pPossibleSpawnPlot->getOwner() == m_pPlayer->GetID()))
			iWorkableSpawnPlots++;
		else
			iNonWorkableSpawnPlots++;

		if (pPossibleSpawnPlot->getOwner() == NO_PLAYER)
			iClaimTiles++;
		else
			iNonClaimTiles++;
	}

	iTileClaimChance = (100 * iClaimTiles) / (iNonClaimTiles + iClaimTiles);

	return (100 * iWorkableSpawnPlots) / (iNonWorkableSpawnPlots + iWorkableSpawnPlots);
}

int CvBuilderTaskingAI::GetPlotYieldValueSimplified(const CvPlot* pPlot, BuildTypes eBuild)
{
	int iValue = 0;

	if (eBuild == NO_BUILD)
		UpdateCurrentPlotYields(pPlot);
	else
		UpdateProjectedPlotYields(pPlot, eBuild, NUM_ROUTE_TYPES);

	for (int ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		YieldTypes eYield = (YieldTypes)ui;

		if (eYield > YIELD_CULTURE_LOCAL)
			break;

		int iYield = eBuild == NO_BUILD ? m_aiCurrentPlotYields[ui] : m_aiProjectedPlotYields[ui];

		iValue += GetYieldBaseModifierTimes100(eYield) * iYield;
	}

	return iValue;
}

void CvBuilderTaskingAI::UpdateAllCityWorstPlots()
{
	m_worstCityPlotValues.clear();

	int iLoopCity;
	const SBuilderState& sState = SBuilderState::DefaultInstance();
	for (const CvCity* pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		UpdateCityWorstPlots(pLoopCity, sState);
	}
}

void CvBuilderTaskingAI::UpdateCityWorstPlots(const CvCity* pCity, const SBuilderState& sState)
{
	int iWorstValue = MAX_INT;

	CvCityCitizens* pCityCitizens = pCity->GetCityCitizens();
	for (int iI = 0; iI < pCity->GetNumWorkablePlots(); iI++)
	{
		CvPlot* pLoopPlot = pCityCitizens->GetCityPlotFromIndex(iI);
		if (pLoopPlot != NULL && !pLoopPlot->isCity())
		{
			if (pCity->GetCityCitizens()->IsWorkingPlot(pLoopPlot))
			{
				BuildTypes eBuild = GetPlannedBuildInPlot(pLoopPlot, sState);

				int iValue = GetPlotYieldValueSimplified(pLoopPlot, eBuild);
				if (iValue < iWorstValue)
					iWorstValue = iValue;
			}
		}
	}

	m_worstCityPlotValues[pCity] = iWorstValue;
}

int CvBuilderTaskingAI::GetWorstWorkedPlotValue(const CvCity* pCity)
{
	map<const CvCity*, int>::iterator it = m_worstCityPlotValues.find(pCity);

	if (it == m_worstCityPlotValues.end())
		return INT_MAX;

	return it->second;
}

BuildTypes CvBuilderTaskingAI::GetRepairBuild(void)
{
	for(int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);

		if(NULL != pkBuild && pkBuild->isRepair())
		{
			return eBuild;
		}
	}

	return NO_BUILD;
}

FeatureTypes CvBuilderTaskingAI::GetFalloutFeature(void)
{
	return static_cast<FeatureTypes>(GD_INT_GET(NUKE_FEATURE));
}

BuildTypes CvBuilderTaskingAI::GetFalloutRemove(void)
{
	FeatureTypes eFalloutFeature = m_eFalloutFeature;
	if(eFalloutFeature == NO_FEATURE)
	{
		eFalloutFeature = GetFalloutFeature();
	}

	if(eFalloutFeature == NO_FEATURE)
	{
		return NO_BUILD;
	}

	for(int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
	{
		BuildTypes eBuild = (BuildTypes)iBuild;
		CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);
		if(NULL != pBuildInfo && pBuildInfo->isFeatureRemove(eFalloutFeature))
		{
			return eBuild;
		}
	}

	return NO_BUILD;
}

BuildTypes CvBuilderTaskingAI::GetRemoveRoute(void)
{
	// find the remove route build
	BuildTypes eRemoveRouteBuild = NO_BUILD;

	for (int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if (pkBuild && pkBuild->IsRemoveRoute())
		{
			eRemoveRouteBuild = eBuild;
			break;
		}
	}

	return eRemoveRouteBuild;
}

BuildTypes CvBuilderTaskingAI::GetBuildRoute(RouteTypes eRoute) const
{
	// find the route build
	BuildTypes eRouteBuild = NO_BUILD;

	for(int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		BuildTypes eBuild = (BuildTypes)i;
		CvBuildInfo* pkBuild = GC.getBuildInfo(eBuild);
		if(pkBuild && pkBuild->getRoute() == eRoute)
		{
			eRouteBuild = eBuild;
			break;
		}
	}

	return eRouteBuild;
}

void CvBuilderTaskingAI::SetupExtraXAdjacentPlots()
{
	m_extraPlotsForXAdjacentImprovements.clear();

	vector<BuildTypes> aPossibleBuilds;
	for (int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;

		if (!m_pPlayer->canBuild(NULL, eBuild, true))
			continue;

		int iLoopUnit = 0;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			if (pLoopUnit->canBuild(NULL, eBuild))
			{
				aPossibleBuilds.push_back(eBuild);
				break;
			}
		}
	}

	for (vector<BuildTypes>::const_iterator it = aPossibleBuilds.begin(); it != aPossibleBuilds.end(); ++it)
	{
		BuildTypes eBuild = *it;
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);

		if (!pkBuildInfo)
			continue;

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
		if (eImprovement == NO_IMPROVEMENT)
			continue;

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (!pkImprovementInfo)
			continue;

		if (pkImprovementInfo->GetXSameAdjacentMakesValid() == 0)
			continue;

		SetupExtraXAdjacentPlotsForBuild(eBuild, eImprovement, pkImprovementInfo->GetXSameAdjacentMakesValid());
	}
}

void CvBuilderTaskingAI::SetupExtraXAdjacentPlotsForBuild(BuildTypes eBuild, ImprovementTypes eImprovement, int iAdjacencyRequirement)
{
	set<CvPlot*> sPlotsToCheck;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
		if (pPlot->isPlayerCityRadius(m_pPlayer->GetID()) && pPlot->canBuild(eBuild, m_pPlayer->GetID()))
			sPlotsToCheck.insert(pPlot);
	}

	while (!sPlotsToCheck.empty())
	{
		set<CvPlot*> sNewPlotsToCheck;
		for (set<CvPlot*>::const_iterator it = sPlotsToCheck.begin(); it != sPlotsToCheck.end(); ++it)
		{
			CvPlot* pPlot = *it;

			if (!pPlot->isPlayerCityRadius(m_pPlayer->GetID()))
				continue;

			if (pPlot->getImprovementType() == eImprovement)
				continue;

			if (pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != m_pPlayer->GetID())
				continue;

			if (!pPlot->canBuild(eBuild, m_pPlayer->GetID(), false, false, true))
				continue;

			if (pPlot->canBuild(eBuild, m_pPlayer->GetID(), false, false))
			{
				m_extraPlotsForXAdjacentImprovements[eImprovement].insert(pPlot);

				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					DirectionTypes eDirection = (DirectionTypes)iI;
					CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);

					if (!pAdjacentPlot)
						continue;

					if (m_extraPlotsForXAdjacentImprovements[eImprovement].find(pAdjacentPlot) != m_extraPlotsForXAdjacentImprovements[eImprovement].end())
						continue;

					sNewPlotsToCheck.insert(pAdjacentPlot);
				}

				continue;
			}

			int iPossibleAdjacentImprovements = 0;

			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				DirectionTypes eDirection = (DirectionTypes)iI;
				CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);

				if (!pAdjacentPlot)
					continue;

				if (pAdjacentPlot->getImprovementType() == eImprovement)
					iPossibleAdjacentImprovements++;
				else if (m_extraPlotsForXAdjacentImprovements[eImprovement].find(pAdjacentPlot) != m_extraPlotsForXAdjacentImprovements[eImprovement].end())
					iPossibleAdjacentImprovements++;
			}

			if (iPossibleAdjacentImprovements >= iAdjacencyRequirement)
			{
				m_extraPlotsForXAdjacentImprovements[eImprovement].insert(pPlot);

				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					DirectionTypes eDirection = (DirectionTypes)iI;
					CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);

					if (!pAdjacentPlot)
						continue;

					if (m_extraPlotsForXAdjacentImprovements[eImprovement].find(pAdjacentPlot) != m_extraPlotsForXAdjacentImprovements[eImprovement].end())
						continue;

					sNewPlotsToCheck.insert(pAdjacentPlot);
				}

				continue;
			}
		}
		sPlotsToCheck.swap(sNewPlotsToCheck);
	}
}

/// Central logging repository!
void CvBuilderTaskingAI::LogInfo(const CvString& strNewLogStr, CvPlayer* pPlayer, bool /*bWriteToOutput*/)
{
	if(!(GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging()))
	{
		return;
	}

	// Open the log file
	CvString strFileName = "BuilderTaskingLog.csv";
	FILogFile* pLog = NULL;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

	CvString strLog;
	CvString strTemp;

	CvString strPlayerName;
	strPlayerName = pPlayer->getCivilizationShortDescription();
	strLog += strPlayerName;

	strLog += ",";

	strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
	strLog += strTemp;
	strLog += strNewLogStr;
	pLog->Msg(strLog);
}

void CvBuilderTaskingAI::LogYieldInfo(const CvString& strNewLogStr, CvPlayer* pPlayer)
{
	if(!(GC.getLogging() && GC.getAILogging() && GC.GetBuilderAILogging()))
	{
		return;
	}

	// Open the log file
	CvString strFileName = "BuilderTaskingYieldLog.csv";
	FILogFile* pLog = NULL;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);

	CvString strLog;
	CvString strTemp;

	CvString strPlayerName;
	strPlayerName = pPlayer->getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
	strLog += strTemp;
	strLog += strNewLogStr;
	pLog->Msg(strLog);
}

/// Logs all the directives for the unit
void CvBuilderTaskingAI::LogDirectives(vector<OptionWithScore<BuilderDirective>> directives)
{
	if(!m_bLogging)
		return;

	if(directives.empty())
	{
		CvString strLog;
		CvString strTemp;

		strLog += "No directives!";
		LogInfo(strLog, m_pPlayer);
	}
	else
	{
		for(size_t i = 0; i < directives.size(); i++)
			LogDirective(directives[i].option, directives[i].score);
	}
}

void CvBuilderTaskingAI::LogDirective(BuilderDirective directive, int iWeight, bool bChosen)
{
	if(!m_bLogging)
		return;

	if (directive.m_eBuild == NO_BUILD || GC.getBuildInfo(directive.m_eBuild) == NULL)
		return;

	CvString strLog;

	strLog += "Evaluating,";

	switch(directive.m_eDirectiveType)
	{
	case BuilderDirective::BUILD_IMPROVEMENT_ON_RESOURCE:
		strLog += "BUILD_IMPROVEMENT_ON_RESOURCE,";
		break;
	case BuilderDirective::BUILD_IMPROVEMENT:
		strLog += "BUILD_IMPROVEMENT,";
		break;
	case BuilderDirective::REPAIR:
		strLog += "REPAIR,";
		break;
	case BuilderDirective::BUILD_ROUTE:
		strLog += "BUILD_ROUTE,";
		break;
	case BuilderDirective::REMOVE_FEATURE:
		strLog += "CHOP,";
		break;
	case BuilderDirective::REMOVE_ROAD:
		strLog += "REMOVE_ROAD,";
		break;
	case BuilderDirective::KEEP_IMPROVEMENT:
		strLog += "KEEP_IMPROVEMENT,";
		break;
	}

	strLog += GC.getBuildInfo(directive.m_eBuild)->GetType();
	strLog += ",";

	if(directive.m_eResource != NO_RESOURCE)
	{
		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(directive.m_eResource);
		if(pkResourceInfo)
		{
			strLog += pkResourceInfo->GetType();
			strLog += ",";
			CvPlot* pPlot = GC.getMap().plot(directive.m_sX, directive.m_sY);
			strLog += pPlot->getNumResource();
			strLog += ",";
		}
	}
	else
	{
		strLog += ",";
	}

	if(directive.m_eDirectiveType == BuilderDirective::REPAIR)
	{
		CvPlot* pPlot = GC.getMap().plot(directive.m_sX, directive.m_sY);
		if(pPlot->IsImprovementPillaged())
		{
			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
				if(pkImprovementInfo)
				{
					strLog += pkImprovementInfo->GetType();
				}
			}
		}
		else
		{
			if(pPlot->getRouteType() != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(pPlot->getRouteType());
				if(pkRouteInfo)
				{
					strLog += pkRouteInfo->GetType();
				}
			}
		}
	}
	else
	{
		CvPlot* pPlot = GC.getMap().plot(directive.m_sX, directive.m_sY);
		if(pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
			if(pkImprovementInfo)
			{
				strLog += pkImprovementInfo->GetType();
			}
		}
	}
	strLog += ",";

	CvString strTemp;
	strTemp.Format("%d,%d,", directive.m_sX, directive.m_sY);
	strLog += strTemp;

	strTemp.Format("%d,", iWeight);
	strLog += strTemp;

	if(bChosen)
	{
		strLog += (", Chosen!");
	}

	LogInfo(strLog, m_pPlayer, m_pPlayer->isHuman());
}

// looks at the current plot to see what it's worth
void CvBuilderTaskingAI::UpdateCurrentPlotYields(const CvPlot* pPlot)
{
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
#if defined(MOD_BALANCE_CORE)
		if((YieldTypes)ui <= YIELD_CULTURE_LOCAL)
		{
#endif
		m_aiCurrentPlotYields[ui] = pPlot->getYield((YieldTypes)ui);

		if(m_bLogging){
			CvString strLog;
			YieldTypes yield = (YieldTypes) ui;
			strLog.Format("Plot Yield Update, %s, %i, %i, %i", FSerialization::toString(yield).c_str(), m_aiCurrentPlotYields[ui], pPlot->getX(), pPlot->getY());
			LogYieldInfo(strLog, m_pPlayer);
		}
#if defined(MOD_BALANCE_CORE)
		}
		else
			//zero it
			m_aiCurrentPlotYields[ui] = 0;
#endif
	}
}

// looks at the current plot assuming the build to see what it's worth
void CvBuilderTaskingAI::UpdateProjectedPlotYields(const CvPlot* pPlot, BuildTypes eBuild, RouteTypes eForceCityConnection)
{
	UpdateCurrentPlotYields(pPlot);

	const CvCity* pOwningCity = pPlot->getEffectiveOwningCity();
	if (pOwningCity)
	{
		ReligionTypes eMajority = pOwningCity->GetCityReligions()->GetReligiousMajority();
		BeliefTypes eSecondaryPantheon = pOwningCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();

		const CvReligion* pReligion = (eMajority != NO_RELIGION) ? GC.getGame().GetGameReligions()->GetReligion(eMajority, pOwningCity->getOwner()) : 0;
		const CvBeliefEntry* pBelief = (eSecondaryPantheon != NO_BELIEF) ? GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon) : 0;

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
		const CvReligion* pPantheon = NULL;
		BeliefTypes ePantheonBelief = NO_BELIEF;
		// Mod for civs keeping their pantheon belief forever
		if (MOD_RELIGION_PERMANENT_PANTHEON)
		{
			pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, pOwningCity->getOwner());
			ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(pOwningCity->getOwner());
		}
#endif

		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
#if defined(MOD_BALANCE_CORE)
			if ((YieldTypes)ui <= YIELD_CULTURE_LOCAL)
			{
#endif
				m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, eForceCityConnection, m_pPlayer->GetID(), pOwningCity, pReligion, pBelief);
				m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
				if (MOD_RELIGION_PERMANENT_PANTHEON)
				{
					if (GC.getGame().GetGameReligions()->HasCreatedPantheon(m_pPlayer->GetID()))
					{
						if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
						{
							if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eMajority, m_pPlayer->GetID()))) // check that the our religion does not have our belief, to prevent double counting
							{
								m_aiProjectedPlotYields[ui] += pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, eForceCityConnection, m_pPlayer->GetID(), pOwningCity, pPantheon, NULL);
								m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);
							}
						}
					}
				}
#endif

				if (m_bLogging){
					CvString strLog;
					YieldTypes yield = (YieldTypes)ui;
					strLog.Format("Plot Projected Yield Update, %s, %i, %i, %i", FSerialization::toString(yield).c_str(), m_aiProjectedPlotYields[ui], pPlot->getX(), pPlot->getY());
					LogYieldInfo(strLog, m_pPlayer);
				}
#if defined(MOD_BALANCE_CORE)
			}
			else
				//zero it!
				m_aiProjectedPlotYields[ui] = 0;
#endif
		}
	}
	else
	{
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
#if defined(MOD_BALANCE_CORE)
			if ((YieldTypes)ui <= YIELD_CULTURE_LOCAL)
			{
#endif
				m_aiProjectedPlotYields[ui] = pPlot->getYieldWithBuild(eBuild, (YieldTypes)ui, false, eForceCityConnection, m_pPlayer->GetID(), NULL, NULL, NULL);
				m_aiProjectedPlotYields[ui] = max(m_aiProjectedPlotYields[ui], 0);

				if (m_bLogging){
					CvString strLog;
					YieldTypes yield = (YieldTypes)ui;
					strLog.Format("Plot Projected Yield Update, %s, %i, %i, %i", FSerialization::toString(yield).c_str(), m_aiProjectedPlotYields[ui], pPlot->getX(), pPlot->getY());
					LogYieldInfo(strLog, m_pPlayer);
				}
#if defined(MOD_BALANCE_CORE)
			}
			else
				//zero it!
				m_aiProjectedPlotYields[ui] = 0;
#endif
		}
	}
}