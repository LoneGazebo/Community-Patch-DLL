#include "CvGameCoreDLLPCH.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvUnit.h"
#include "CvGlobals.h"
#include "CvUnitMovement.h"
#include "CvGameCoreUtils.h"
//	---------------------------------------------------------------------------
int CvUnitMovement::GetCostsForMove(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iTerrainFeatureCostMultiplierFromPromotions, int iTerrainFeatureCostAdderFromPromotions)
{
	int iMoveDenominator = GC.getMOVE_DENOMINATOR();
	int iRegularCost = iMoveDenominator;
	int iRouteCost = INT_MAX; //assume no route

	//some easy checks first
	if (pUnit->isHuman() && !pToPlot->isRevealed(pUnit->getTeam()))
	{
		//moving into unknown tiles ends the turn for humans (to prevent information leakage from the displayed path)
		return INT_MAX;
	}
	else if (pUnit->flatMovementCost() || pUnit->getDomainType() == DOMAIN_AIR)
	{
		return iMoveDenominator;
	}

	CvPlayerAI& kPlayer = GET_PLAYER(pUnit->getOwner());
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	bool bFasterAlongRiver = pTraits->IsRiverMovementBonus();
	bool bFasterInHills = pTraits->IsFasterInHills();
	//ignore terrain cost also ignores feature cost, but units may get bonuses from terrain
	//difference to flat movement cost is that flat movement units don't get any bonuses
	bool bIgnoreTerrainCost = pUnit->ignoreTerrainCost();
	bool bAmphibious = pUnit ? pUnit->isRiverCrossingNoPenalty() : false;
	bool bHover = pUnit ? pUnit->IsHoveringUnit() : false;

	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

	//try to avoid calling directionXY too often
	bool bRiverCrossing = pFromPlot->getRiverCrossingCount()>0 && pToPlot->getRiverCrossingCount()>0 && pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot));
	FeatureTypes eToFeature = pToPlot->getFeatureType();
	CvFeatureInfo* pToFeatureInfo = (eToFeature > NO_FEATURE) ? GC.getFeatureInfo(eToFeature) : 0;
	TerrainTypes eToTerrain = pToPlot->getTerrainType();
	CvTerrainInfo* pToTerrainInfo = (eToTerrain > NO_TERRAIN) ? GC.getTerrainInfo(eToTerrain) : 0;

	//route preparation
	bool bRouteTo = pToPlot->isValidRoute(pUnit);
	bool bRouteFrom = pFromPlot->isValidRoute(pUnit);
	bool bFakeRouteTo = pTraits->IsWoodlandMovementBonus() && (eToFeature == FEATURE_FOREST || eToFeature == FEATURE_JUNGLE);
	bool bFakeRouteFrom = pTraits->IsWoodlandMovementBonus() && (pFromPlot->getFeatureType() == FEATURE_FOREST || pFromPlot->getFeatureType() == FEATURE_JUNGLE);
	//ideally there'd be a check of the river direction to make sure it's the same river
	bool bMovingAlongRiver = pToPlot->isRiver() && pFromPlot->isRiver() && !bRiverCrossing;
	bFakeRouteTo = bFakeRouteTo || (bFasterAlongRiver && bMovingAlongRiver);
	bFakeRouteFrom = bFakeRouteFrom || (bFasterAlongRiver && bMovingAlongRiver);

	//check routes
	if (!bHover &&
		(bRouteFrom || bFakeRouteFrom) &&
		(bRouteTo || bFakeRouteTo) &&
		(!bRiverCrossing || kUnitTeam.isBridgeBuilding() || bAmphibious))
	{
		RouteTypes eFromRoute = bFakeRouteFrom ? ROUTE_ROAD : pFromPlot->getRouteType();
		CvRouteInfo* pFromRouteInfo = GC.getRouteInfo(eFromRoute);
		int iFromMovementCost = pFromRouteInfo ? pFromRouteInfo->getMovementCost() : 0;
		int iFromFlatMovementCost = pFromRouteInfo ? pFromRouteInfo->getFlatMovementCost() : 0;

		RouteTypes eToRoute = bFakeRouteTo ? ROUTE_ROAD : pToPlot->getRouteType();
		CvRouteInfo* pToRouteInfo = GC.getRouteInfo(eToRoute);
		int iToMovementCost = pToRouteInfo ? pToRouteInfo->getMovementCost() : 0;
		int iToFlatMovementCost = pToRouteInfo ? pToRouteInfo->getFlatMovementCost() : 0;

		//routes only on land
		int iBaseMoves = pUnit->baseMoves(DOMAIN_LAND);

		int iRouteVariableCost = std::max(iFromMovementCost + kUnitTeam.getRouteChange(eFromRoute), iToMovementCost + kUnitTeam.getRouteChange(eToRoute));
		int iRouteFlatCost = std::max(iFromFlatMovementCost * iBaseMoves, iToFlatMovementCost * iBaseMoves);

		iRouteCost = std::min(iRouteVariableCost, iRouteFlatCost);

		if (pToPlot->isFriendlyCityOrPassableImprovement(pUnit->getOwner())) //don't consider terrain/feature effects for cities
			return iRouteCost;
	}

	//check embarkation
	bool bFullCostEmbarkStateChange = false;
	bool bCheapEmbarkStateChange = false;
	bool bFreeEmbarkStateChange = false;
	if (pUnit->CanEverEmbark())
	{
		bool bFromEmbark = pFromPlot->needsEmbarkation(pUnit);
		bool bToEmbark = pToPlot->needsEmbarkation(pUnit);

		if (!bToEmbark && bFromEmbark)
		{
			// Is the unit from a civ that can disembark for just 1 MP?
			// Does it have a promotion to do so?
			if (GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost() || pUnit->isDisembarkFlatCost())
				bCheapEmbarkStateChange = true;

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			//If city, and player has disembark to city at reduced cost...
			if (pToPlot->isFriendlyCityOrPassableImprovement(pUnit->getOwner()))
			{
				if (kUnitTeam.isCityNoEmbarkCost())
					bFreeEmbarkStateChange = true;
				else if (kUnitTeam.isCityLessEmbarkCost())
					bCheapEmbarkStateChange = true;
			}
#endif

			bFullCostEmbarkStateChange = !(bFreeEmbarkStateChange || bCheapEmbarkStateChange);
		}

		if (bToEmbark && !bFromEmbark)
		{
			// Is the unit from a civ that can embark for just 1 MP?
			// Does it have a promotion to do so?
			if (GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost() || pUnit->isEmbarkFlatCost())
				bCheapEmbarkStateChange = true;

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			//If city, and player has embark from city at reduced cost...
			if (pFromPlot->isFriendlyCityOrPassableImprovement(pUnit->getOwner()))
			{
				if (kUnitTeam.isCityNoEmbarkCost())
					bFreeEmbarkStateChange = true;
				else if (kUnitTeam.isCityLessEmbarkCost())
					bCheapEmbarkStateChange = true;
			}
#endif

			bFullCostEmbarkStateChange = !(bFreeEmbarkStateChange || bCheapEmbarkStateChange);
		}

		if (bFullCostEmbarkStateChange)
		{
			//normal embark/disembark ends turn
			return INT_MAX;
		}
		else if (bFreeEmbarkStateChange && !pUnit->isCargo()) //a cover charge still applies :)
		{
			return iMoveDenominator / 10;
		}
		else if (bCheapEmbarkStateChange && !pUnit->isCargo())
		{
			return iMoveDenominator;
		}
	}

	//in some cases we ignore terrain / feature cost
	if (bFasterInHills && pToPlot->isHills())
		bIgnoreTerrainCost = true;

	if (bHover)
		bIgnoreTerrainCost = true;

#if defined(MOD_BALANCE_CORE)
	if (MOD_BALANCE_CORE && bAmphibious && bRiverCrossing)
		bIgnoreTerrainCost = true;

	if (MOD_BALANCE_CORE && pTraits->IsMountainPass() && pToPlot->isMountain())
		bIgnoreTerrainCost = true;
#endif

	//check border obstacle - great wall ends the turn
	TeamTypes eToTeam = pToPlot->getTeam();
	TeamTypes eFromTeam = pFromPlot->getTeam();
	if (eToTeam != NO_TEAM && eUnitTeam != eToTeam && eToTeam != eFromTeam)
	{
		CvTeam& kToPlotTeam = GET_TEAM(eToTeam);
		CvPlayer& kToPlotPlayer = GET_PLAYER(pToPlot->getOwner());

		if (!kToPlotTeam.IsAllowsOpenBordersToTeam(eUnitTeam))
		{
			//only applies on land
			if (kToPlotTeam.isBorderObstacle() || kToPlotPlayer.isBorderObstacle())
			{
				if (!pToPlot->isWater() && pUnit->getDomainType() == DOMAIN_LAND)
				{
					return INT_MAX;
				}
			}
		}
	}

	if (pToPlot->isRoughGround() && pUnit->IsRoughTerrainEndsTurn() && !(bRouteFrom && bRouteTo))
	{
		// Is a unit's movement consumed for entering rough terrain?
		return INT_MAX;
	}
	// This is a special Domain unit that can disembark and becomes a land unit. End Turn like normal disembarkation.
	else if ((pUnit->getDomainType() == DOMAIN_SEA && pUnit->isConvertUnit() && !pToPlot->isWater() && pFromPlot->isWater()) || 
			 (pUnit->getDomainType() == DOMAIN_LAND && pUnit->isConvertUnit() && pToPlot->isWater() && !pFromPlot->isWater()) )
	{
		return INT_MAX;
	}
	else if (pToPlot->isFriendlyCityOrPassableImprovement(pUnit->getOwner())) //case with route is already handled above
	{
		return iMoveDenominator;
	}
#if defined(MOD_CARGO_SHIPS)
	else if (pUnit->isCargo() && pUnit->getDomainType() == DOMAIN_LAND &&
		GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost() &&
		pToPlot->isCoastalLand(1) && pFromPlot->isWater())
	{
		iRegularCost = 0;
	}
#endif
	else
	{
		//if the unit ignores terrain cost, it can still profit from feature bonuses
		if (bIgnoreTerrainCost)
			iRegularCost = 1;
		else
		{
			iRegularCost = ((eToFeature == NO_FEATURE) ? (pToTerrainInfo ? pToTerrainInfo->getMovementCost() : 0) : (pToFeatureInfo ? pToFeatureInfo->getMovementCost() : 0));

			// Hill cost is hardcoded
			if (pToPlot->isHills() || pToPlot->isMountain())
			{
				iRegularCost += GC.getHILLS_EXTRA_MOVEMENT();
			}

			if (bRiverCrossing && !bAmphibious)
			{
				iRegularCost += GC.getRIVER_EXTRA_MOVEMENT();
			}
		}

		if (iRegularCost > 0)
		{
			iRegularCost = std::max(1, (iRegularCost - pUnit->getExtraMoveDiscount()));
		}

		//now switch to high-precision costs
		iRegularCost *= iMoveDenominator;

		if (iTerrainFeatureCostMultiplierFromPromotions < 0)
			//we have to do it on the fly
			iTerrainFeatureCostMultiplierFromPromotions = CvUnitMovement::GetMovementCostMultiplierFromPromotions(pUnit, pToPlot);

		//multiplicative change
		iRegularCost *= iTerrainFeatureCostMultiplierFromPromotions;
		iRegularCost /= iMoveDenominator;

		if (iTerrainFeatureCostAdderFromPromotions < 0)
			//we have to do it on the fly
			iTerrainFeatureCostAdderFromPromotions = CvUnitMovement::GetMovementCostAdderFromPromotions(pUnit, pToPlot);

		//additive change
		iRegularCost += iTerrainFeatureCostAdderFromPromotions;

		//extra movement cost in some instances
		bool bSlowDown = false;
		if (eToTeam != NO_TEAM && eUnitTeam != eToTeam)
		{
			if (!pToPlot->IsFriendlyTerritory(kPlayer.GetID()))
			{
				//unit itself may have a negative trait ...
				bSlowDown = pUnit->isSlowInEnemyLand();

				if (!bSlowDown)
				{
					//city might have special defense buildings
					CvCity* pCity = pToPlot->getOwningCity();
					if (pCity)
					{
						if (!pToPlot->isWater() && pUnit->getDomainType() == DOMAIN_LAND)
						{
							bSlowDown = (pCity->GetBorderObstacleCity() > 0);
						}
						if (pToPlot->isWater() && (pUnit->getDomainType() == DOMAIN_SEA || pToPlot->needsEmbarkation(pUnit)))
						{
							bSlowDown = (pCity->GetBorderObstacleWater() > 0);
						}
					}
				}
			}
		}

		if (bSlowDown)
		{
			iRegularCost += iMoveDenominator;
		}
	}

	//sometimes the route cost can be higher than what we get with promotions
	return min(iRegularCost,iRouteCost);
}

//	---------------------------------------------------------------------------
int CvUnitMovement::MovementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iTerrainFeatureCostMultiplierFromPromotions, int iTerrainFeatureCostAdderFromPromotions)
{
	if (IsSlowedByZOC(pUnit, pFromPlot, pToPlot))
		return iMovesRemaining;

	return MovementCostNoZOC(pUnit, pFromPlot, pToPlot, iMovesRemaining, iMaxMoves, iTerrainFeatureCostMultiplierFromPromotions, iTerrainFeatureCostAdderFromPromotions);
}

//	---------------------------------------------------------------------------
int CvUnitMovement::MovementCostSelectiveZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iTerrainFeatureCostMultiplierFromPromotions, int iTerrainFeatureCostAdderFromPromotions, const PlotIndexContainer& plotsToIgnore)
{
	if (IsSlowedByZOC(pUnit, pFromPlot, pToPlot, plotsToIgnore))
		return iMovesRemaining;

	return MovementCostNoZOC(pUnit, pFromPlot, pToPlot, iMovesRemaining, iMaxMoves, iTerrainFeatureCostMultiplierFromPromotions, iTerrainFeatureCostAdderFromPromotions);
}

//	---------------------------------------------------------------------------
int CvUnitMovement::MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iTerrainFeatureCostMultiplierFromPromotions, int iTerrainFeatureCostAdderFromPromotions)
{
	int iCost = GetCostsForMove(pUnit, pFromPlot, pToPlot, iTerrainFeatureCostMultiplierFromPromotions, iTerrainFeatureCostAdderFromPromotions);

	//now, if there was a domain change, our base moves might change
	//make sure that the movement cost is always so high that we never end up with more than the base moves for the new domain
	int iLeftOverMoves = iMovesRemaining - iCost;
	if (iLeftOverMoves > iMaxMoves)
		iCost += (iLeftOverMoves - iMaxMoves);

	return std::min(iCost, iMovesRemaining);
}

//	--------------------------------------------------------------------------------
bool CvUnitMovement::IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, const PlotIndexContainer& plotsToIgnore)
{
	if (pUnit->IsIgnoreZOC())
		return false;

	// Zone of Control
	if (GC.getZONE_OF_CONTROL_ENABLED() <= 0)
		return false;

	TeamTypes eUnitTeam = pUnit->getTeam();
	DomainTypes eUnitDomain = pUnit->getDomainType();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

	//there are only two plots we need to check
	DirectionTypes moveDir = directionXY(pFromPlot, pToPlot);
	int eRight = (int(moveDir) + 1) % 6;
	int eLeft = (int(moveDir) + 5) % 6;
	CvPlot* aPlotsToCheck[2];
	aPlotsToCheck[0] = plotDirection(pFromPlot->getX(), pFromPlot->getY(), (DirectionTypes)eRight);
	aPlotsToCheck[1] = plotDirection(pFromPlot->getX(), pFromPlot->getY(), (DirectionTypes)eLeft);
	for (int iCount = 0; iCount<2; iCount++)
	{
		CvPlot* pAdjPlot = aPlotsToCheck[iCount];
		if (!pAdjPlot)
			continue;

		//this is the only difference to the regular version below
		if ( std::find(plotsToIgnore.begin(),plotsToIgnore.end(),pAdjPlot->GetPlotIndex()) != plotsToIgnore.end() )
			continue;

		// check city zone of control
		if (pAdjPlot->isEnemyCity(*pUnit))
			return true;

		// Loop through all units to see if there's an enemy unit here
		IDInfo* pAdjUnitNode = pAdjPlot->headUnitNode();
		while (pAdjUnitNode != NULL)
		{
			CvUnit* pLoopUnit = NULL;
			if ((pAdjUnitNode->eOwner >= 0) && pAdjUnitNode->eOwner < MAX_PLAYERS)
				pLoopUnit = (GET_PLAYER(pAdjUnitNode->eOwner).getUnit(pAdjUnitNode->iID));

			pAdjUnitNode = pAdjPlot->nextUnitNode(pAdjUnitNode);

			if (!pLoopUnit)
				continue;

			if (pLoopUnit->isInvisible(eUnitTeam, false))
				continue;

			// Combat unit?
			if (!pLoopUnit->IsCombatUnit())
				continue;

			// Embarked units don't have ZOC
			if (pLoopUnit->isEmbarked())
				continue;

			// At war with this unit's team?
			TeamTypes eLoopUnitTeam = pLoopUnit->getTeam();
			if (eLoopUnitTeam == BARBARIAN_TEAM || kUnitTeam.isAtWar(eLoopUnitTeam) || pLoopUnit->isAlwaysHostile(*pAdjPlot))
			{
				// Same Domain?
				DomainTypes eLoopUnitDomain = pLoopUnit->getDomainType();
				if (eLoopUnitDomain != eUnitDomain)
				{
					// hovering units always exert a ZOC
					if (pLoopUnit->IsHoveringUnit() || eLoopUnitDomain == DOMAIN_HOVER)
					{
						// continue on
					}
					// water unit can ZoC embarked land unit
					else if (eLoopUnitDomain == DOMAIN_SEA && (pToPlot->needsEmbarkation(pUnit) || pFromPlot->needsEmbarkation(pUnit)))
					{
						// continue on
					}
					else
					{
						// ignore this unit
						continue;
					}
				}
				else
				{
					//land units don't ZoC embarked units (if they stay embarked)
					if (eLoopUnitDomain == DOMAIN_LAND && pToPlot->needsEmbarkation(pUnit) && pFromPlot->needsEmbarkation(pUnit))
					{
						continue;
					}
				}

				//ok, all conditions fulfilled
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnitMovement::IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	if (pUnit->IsIgnoreZOC())
		return false;

	// Zone of Control
	if (GC.getZONE_OF_CONTROL_ENABLED() <= 0)
		return false;

	TeamTypes eUnitTeam = pUnit->getTeam();
	DomainTypes eUnitDomain = pUnit->getDomainType();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

	//there are only two plots we need to check
	DirectionTypes moveDir = directionXY(pFromPlot, pToPlot);
	int eRight = (int(moveDir) + 1) % 6;
	int eLeft = (int(moveDir) + 5) % 6;
	CvPlot* aPlotsToCheck[2];
	aPlotsToCheck[0] = plotDirection(pFromPlot->getX(), pFromPlot->getY(), (DirectionTypes)eRight);
	aPlotsToCheck[1] = plotDirection(pFromPlot->getX(), pFromPlot->getY(), (DirectionTypes)eLeft);
	for (int iCount = 0; iCount<2; iCount++)
	{
		CvPlot* pAdjPlot = aPlotsToCheck[iCount];
		if (!pAdjPlot)
			continue;

		// check city zone of control
		if (pAdjPlot->isEnemyCity(*pUnit))
			return true;

		// Loop through all units to see if there's an enemy unit here
		IDInfo* pAdjUnitNode = pAdjPlot->headUnitNode();
		while (pAdjUnitNode != NULL)
		{
			CvUnit* pLoopUnit = NULL;
			if ((pAdjUnitNode->eOwner >= 0) && pAdjUnitNode->eOwner < MAX_PLAYERS)
				pLoopUnit = (GET_PLAYER(pAdjUnitNode->eOwner).getUnit(pAdjUnitNode->iID));

			pAdjUnitNode = pAdjPlot->nextUnitNode(pAdjUnitNode);

			if (!pLoopUnit || pLoopUnit->isDelayedDeath())
				continue;

			if (pLoopUnit->isInvisible(eUnitTeam, false))
				continue;

			// Combat unit?
			if (!pLoopUnit->IsCombatUnit())
				continue;

			// Embarked units don't have ZOC
			if (pLoopUnit->isEmbarked())
				continue;

			// At war with this unit's team?
			TeamTypes eLoopUnitTeam = pLoopUnit->getTeam();
			if (kUnitTeam.isAtWar(eLoopUnitTeam) || pLoopUnit->isAlwaysHostile(*pAdjPlot))
			{
				// Same Domain?
				DomainTypes eLoopUnitDomain = pLoopUnit->getDomainType();
				if (eLoopUnitDomain != eUnitDomain)
				{
					// hovering units always exert a ZOC
					if (pLoopUnit->IsHoveringUnit() || eLoopUnitDomain == DOMAIN_HOVER)
					{
						// continue on
					}
					// water unit can ZoC embarked land unit
					else if (eLoopUnitDomain == DOMAIN_SEA && (pToPlot->needsEmbarkation(pUnit) || pFromPlot->needsEmbarkation(pUnit)))
					{
						// continue on
					}
					else
					{
						// ignore this unit
						continue;
					}
				}
				else
				{
					//land units don't ZoC embarked units (if they stay embarked)
					if (eLoopUnitDomain == DOMAIN_LAND && pToPlot->needsEmbarkation(pUnit) && pFromPlot->needsEmbarkation(pUnit))
					{
						continue;
					}
				}

				//ok, all conditions fulfilled
				return true;
			}
		}
	}

	return false;
}

//base value is 60. so < 60 actually means easier movement
int CvUnitMovement::GetMovementCostMultiplierFromPromotions(const CvUnit* pUnit, const CvPlot* pPlot)
{
	int iModifier = GC.getMOVE_DENOMINATOR();
	TerrainTypes eToTerrain = pPlot->getTerrainType();
	FeatureTypes eToFeature = pPlot->getFeatureType();

	if (pPlot->isHills() && pUnit->isHillsDoubleMove())
	{
		iModifier /= 2;
	}
	else if (pPlot->isHills() && pUnit->isTerrainHalfMove(TERRAIN_HILL))
	{
		iModifier *= 2;
	}
	else if (pPlot->isMountain() && pUnit->isMountainsDoubleMove())
	{
		iModifier /= 2;
	}
	else if (pUnit->isTerrainDoubleMove(eToTerrain) || pUnit->isFeatureDoubleMove(eToFeature))
	{
		iModifier /= 2;
	}
	else if (pUnit->isTerrainHalfMove(eToTerrain) || pUnit->isFeatureHalfMove(eToFeature))
	{
		iModifier *= 2;
	}

	return iModifier;
}


//base value is 0, any value >0 means movement is harder
int CvUnitMovement::GetMovementCostAdderFromPromotions(const CvUnit* pUnit, const CvPlot* pPlot)
{
	int iModifier = 0;
	TerrainTypes eToTerrain = pPlot->getTerrainType();
	FeatureTypes eToFeature = pPlot->getFeatureType();

	if (pPlot->isHills() && pUnit->isTerrainExtraMove(TERRAIN_HILL))
	{
		iModifier += (GC.getMOVE_DENOMINATOR() * pUnit->getTerrainExtraMoveCount(TERRAIN_HILL));
	}
	else if (pUnit->isTerrainExtraMove(eToTerrain))
	{
		iModifier += (GC.getMOVE_DENOMINATOR() * pUnit->getTerrainExtraMoveCount(eToTerrain));
	}
	else if (pUnit->isFeatureExtraMove(eToFeature))
	{
		iModifier += (GC.getMOVE_DENOMINATOR() * pUnit->getFeatureExtraMoveCount(eToFeature));
	}

	return iModifier;
}