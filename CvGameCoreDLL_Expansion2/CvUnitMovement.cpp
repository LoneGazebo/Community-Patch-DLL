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
	int iMoveDenominator = GD_INT_GET(MOVE_DENOMINATOR);
	int iRegularCost = iMoveDenominator;
	int iRouteCost = INT_MAX; //assume no route

	PlayerTypes kPlayerId = pUnit->getOwner();
	CvPlayerAI& kPlayer = GET_PLAYER(pUnit->getOwner());
	DomainTypes domain = pUnit->getDomainType();

	//some easy checks first
	if (kPlayer.isHuman() && !pToPlot->isRevealed(pUnit->getTeam()))
	{
		//moving into unknown tiles ends the turn for humans (to prevent information leakage from the displayed path)
		return INT_MAX;
	}
	else if (domain == DOMAIN_AIR)
	{
		return iMoveDenominator;
	}

	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();

	bool bAmphibious = pUnit ? pUnit->isRiverCrossingNoPenalty() : false;
	bool bHover = pUnit ? pUnit->IsHoveringUnit() : false;

	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

	//try to avoid calling directionXY too often
	bool bRiverCrossing = pFromPlot->getRiverCrossingCount()>0 && pToPlot->getRiverCrossingCount()>0 && pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot));
	FeatureTypes eToFeature = pToPlot->getFeatureType();
	CvFeatureInfo* pToFeatureInfo = (eToFeature > NO_FEATURE) ? GC.getFeatureInfo(eToFeature) : NULL;
	TerrainTypes eToTerrain = pToPlot->getTerrainType();
	CvTerrainInfo* pToTerrainInfo = (eToTerrain > NO_TERRAIN) ? GC.getTerrainInfo(eToTerrain) : NULL;
	FeatureTypes eFromFeature = pFromPlot->getFeatureType();

	bool bToIsWater = pToPlot->isWater();
	bool bFromIsWater = pFromPlot->isWater();

	//route preparation
	bool bRouteTo = pToPlot->isValidRoute(pUnit);
	bool bRouteFrom = pFromPlot->isValidRoute(pUnit);

	//ideally there'd be a check of the river direction to make sure it's the same river
	bool bMovingAlongRiver = pToPlot->isRiver() && pFromPlot->isRiver() && !bRiverCrossing;
	bool bFakeRouteTo = (pTraits->IsRiverMovementBonus() && bMovingAlongRiver);
	bool bFakeRouteFrom = (pTraits->IsRiverMovementBonus() && bMovingAlongRiver);

	if (!MOD_SANE_UNIT_MOVEMENT_COST)
	{
		//balance patch does not require plot ownership
		bFakeRouteTo |= (pTraits->IsWoodlandMovementBonus() && (eToFeature == FEATURE_FOREST || eToFeature == FEATURE_JUNGLE) && (MOD_BALANCE_VP || pToPlot->getTeam() == eUnitTeam));
		bFakeRouteFrom |= (pTraits->IsWoodlandMovementBonus() && (eFromFeature == FEATURE_FOREST || eFromFeature == FEATURE_JUNGLE) && (MOD_BALANCE_VP || pToPlot->getTeam() == eUnitTeam));
	}

	//check routes
	if (!bHover &&
		(bRouteFrom || bFakeRouteFrom) &&
		(bRouteTo || bFakeRouteTo) &&
		(!bRiverCrossing || kUnitTeam.isBridgeBuilding() || bAmphibious))
	{
		RouteTypes eFromRoute = bFakeRouteFrom && pFromPlot->getRouteType() < ROUTE_ROAD ? ROUTE_ROAD : pFromPlot->getRouteType();
		CvRouteInfo* pFromRouteInfo = GC.getRouteInfo(eFromRoute);
		int iFromMovementCost = pFromRouteInfo ? pFromRouteInfo->getMovementCost() : 0;
		int iFromFlatMovementCost = pFromRouteInfo ? pFromRouteInfo->getFlatMovementCost() : 0;

		RouteTypes eToRoute = bFakeRouteTo && pToPlot->getRouteType() < ROUTE_ROAD ? ROUTE_ROAD : pToPlot->getRouteType();
		CvRouteInfo* pToRouteInfo = GC.getRouteInfo(eToRoute);
		int iToMovementCost = pToRouteInfo ? pToRouteInfo->getMovementCost() : 0;
		int iToFlatMovementCost = pToRouteInfo ? pToRouteInfo->getFlatMovementCost() : 0;

		//routes only on land
		int iBaseMoves = pUnit->baseMoves(false);

		int iRouteVariableCost = std::max(iFromMovementCost + kUnitTeam.getRouteChange(eFromRoute), iToMovementCost + kUnitTeam.getRouteChange(eToRoute));
		int iRouteFlatCost = std::max(iFromFlatMovementCost * iBaseMoves, iToFlatMovementCost * iBaseMoves);

		iRouteCost = std::min(iRouteVariableCost, iRouteFlatCost);

		if (pToPlot->isCity()) //don't consider terrain/feature effects for cities
			return iRouteCost;
	}

	//check embarkation
	bool bFullCostEmbarkStateChange = false;
	bool bCheapEmbarkStateChange = false;
	bool bFreeEmbarkStateChange = false;
	if ((bToIsWater != bFromIsWater) && pUnit->CanEverEmbark())
	{
		bool bFromEmbark = pFromPlot->needsEmbarkation(pUnit);
		bool bToEmbark = pToPlot->needsEmbarkation(pUnit);

		if (!bToEmbark && bFromEmbark)
		{
			// Is the unit from a civ that can disembark for just 1 MP?
			// Does it have a promotion to do so?
			if (pTraits->IsEmbarkedToLandFlatCost() || pUnit->isDisembarkFlatCost())
				bCheapEmbarkStateChange = true;

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			//If city, and player has disembark to city at reduced cost...
			if (pToPlot->isCoastalCityOrPassableImprovement(pUnit->getOwner(),true,true))
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
			if (pTraits->IsEmbarkedToLandFlatCost() || pUnit->isEmbarkFlatCost())
				bCheapEmbarkStateChange = true;

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			//If city, and player has embark from city at reduced cost...
			if (pFromPlot->isCoastalCityOrPassableImprovement(kPlayerId,true,true))
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

	//flat cost only after embarkation/disembarkation
	 if (pUnit->flatMovementCost())
		return iMoveDenominator;

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
				if (!bToIsWater && domain == DOMAIN_LAND)
				{
					return INT_MAX;
				}
			}
			//city might have special defense buildings
			CvCity* pCity = pToPlot->getOwningCity();
			if (pCity)
			{
				if (!bToIsWater && domain == DOMAIN_LAND && pCity->IsBorderObstacleLand())
				{
					return INT_MAX;
				}
				if (bToIsWater && pCity->IsBorderObstacleWater() && (domain == DOMAIN_SEA || pToPlot->needsEmbarkation(pUnit)))
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
	else if ((domain == DOMAIN_SEA && pUnit->isConvertUnit() && !bToIsWater && bFromIsWater) || 
			 (domain == DOMAIN_LAND && pUnit->isConvertUnit() && bToIsWater && !bFromIsWater) )
	{
		return INT_MAX;
	}
	//a city always counts as flat open terrain. case with route is already handled above!
	else if (pToPlot->isCity() && !kUnitTeam.isAtWar(eToTeam) && (!bRiverCrossing || kUnitTeam.isBridgeBuilding())) 
	{
		return iMoveDenominator;
	}
#if defined(MOD_CARGO_SHIPS)
	else if (pUnit->isCargo() && domain == DOMAIN_LAND &&
		pTraits->IsEmbarkedToLandFlatCost() &&
		pToPlot->isCoastalLand(1) && bFromIsWater)
	{
		iRegularCost = 0;
	}
#endif
	else //normal case, check terrain and features
	{
		//global ignore terrain cost also ignores feature cost, but units may get bonuses from terrain!
		//difference to flat movement cost is that flat movement units don't get any bonuses
		bool bIgnoreCostsHere = pUnit->ignoreTerrainCost();
		bool bNoRiverCrossingPenalty = !bRiverCrossing || bHover || bAmphibious || (bIgnoreCostsHere && /*TRUE*/ GD_INT_GET(IGNORE_GLOBAL_TERRAIN_COSTS_INCLUDES_RIVERS) > 0);

		//in some cases, we ignore terrain and feature costs if the destination tile contains a specific terrain/feature
		if (pTraits->IsFasterInHills() && (pToPlot->isHills() || pToPlot->isMountain()))
		{
			bIgnoreCostsHere = true;
			bNoRiverCrossingPenalty = true; // as specified by Incan UA
		}
		else if (pToPlot->isHills() && pUnit->isIgnoreTerrainCostIn(TERRAIN_HILL))
		{
			bIgnoreCostsHere = true;
		}
		else if (pToPlot->isMountain() && pUnit->isIgnoreTerrainCostIn(TERRAIN_MOUNTAIN))
		{
			bIgnoreCostsHere = true;
		}
		else if (pUnit->isIgnoreTerrainCostIn(eToTerrain))
		{
			bIgnoreCostsHere = true;
		}
		else if (pUnit->isIgnoreFeatureCostIn(eToFeature))
		{
			bIgnoreCostsHere = true;
		}

		if (MOD_SANE_UNIT_MOVEMENT_COST)
		{
			if (iTerrainFeatureCostAdderFromPromotions == INT_MAX)
				//we have to do it on the fly
				iTerrainFeatureCostAdderFromPromotions = CvUnitMovement::GetMovementCostChangeFromPromotions(pUnit, pToPlot);

			//cost reduction simply means ignore terrain/feature cost
			if (iTerrainFeatureCostAdderFromPromotions < 0)
			{
				bIgnoreCostsHere = true;
				iTerrainFeatureCostAdderFromPromotions = 0;
			}
		}

		// Check the define to see if ignoring specific terrain/feature costs includes rivers
		if (bIgnoreCostsHere && !bNoRiverCrossingPenalty && /*FALSE*/ GD_INT_GET(IGNORE_SPECIFIC_TERRAIN_COSTS_INCLUDES_RIVERS) > 0)
			bNoRiverCrossingPenalty = true;

		if (bIgnoreCostsHere && bNoRiverCrossingPenalty)
			iRegularCost = 1;
		else
		{
			if (bIgnoreCostsHere)
				iRegularCost = 1;
			else
			{
				// if a feature is present on a tile, the feature cost overrides the tile's base terrain cost, EXCEPT for Hills and Mountains
				if (eToFeature == NO_FEATURE)
				{
					if (pUnit->isIgnoreTerrainCostFrom(eToTerrain))
						iRegularCost = 1;
					else
						iRegularCost = pToTerrainInfo ? pToTerrainInfo->getMovementCost() : 0;
				}
				else
				{
					if (pUnit->isIgnoreFeatureCostFrom(eToFeature))
						iRegularCost = 1;
					else
						iRegularCost = pToFeatureInfo ? pToFeatureInfo->getMovementCost() : 0;
				}

				// Hill cost is hardcoded
				if ((pToPlot->isHills() && !pUnit->isIgnoreTerrainCostFrom(TERRAIN_HILL)) || (pToPlot->isMountain() && !pUnit->isIgnoreTerrainCostFrom(TERRAIN_MOUNTAIN)))
				{
					iRegularCost += /*1*/ GD_INT_GET(HILLS_EXTRA_MOVEMENT);
				}
			}

			if (!bNoRiverCrossingPenalty && domain == DOMAIN_LAND)
			{
				iRegularCost += /*10*/ GD_INT_GET(RIVER_EXTRA_MOVEMENT);
			}
		}

		if (iRegularCost > 0)
		{
			iRegularCost = std::max(1, (iRegularCost - pUnit->getExtraMoveDiscount()));
		}

		//now switch to high-precision costs
		iRegularCost *= iMoveDenominator;

		//even if the unit ignores terrain cost, it can still profit from terrain/feature bonuses
		if (!MOD_SANE_UNIT_MOVEMENT_COST)
		{
			if (iTerrainFeatureCostMultiplierFromPromotions == INT_MAX)
				//we have to do it on the fly
				iTerrainFeatureCostMultiplierFromPromotions = CvUnitMovement::GetMovementCostMultiplierFromPromotions(pUnit, pToPlot);

			//multiplicative change
			iRegularCost *= iTerrainFeatureCostMultiplierFromPromotions;
			iRegularCost /= iMoveDenominator;
		}

		if (iTerrainFeatureCostAdderFromPromotions == INT_MAX)
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
	if (/*1*/ GD_INT_GET(ZONE_OF_CONTROL_ENABLED) <= 0)
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
			//performance optimization; skip the expensive unit lookup for our own units
			if (pAdjUnitNode->eOwner == pUnit->getOwner())
			{
				pAdjUnitNode = pAdjPlot->nextUnitNode(pAdjUnitNode);
				continue;
			}

			CvUnit* pLoopUnit = (GET_PLAYER(pAdjUnitNode->eOwner).getUnit(pAdjUnitNode->iID));
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
			if (eLoopUnitTeam == BARBARIAN_TEAM || kUnitTeam.isAtWar(eLoopUnitTeam) || pLoopUnit->isAlwaysHostile(*pAdjPlot))
			{
				// Same Domain?
				DomainTypes eLoopUnitDomain = pLoopUnit->getDomainType();
				if (eLoopUnitDomain != eUnitDomain)
				{
					// hovering units always exert a ZOC
					if (pLoopUnit->IsHoveringUnit())
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
	if (/*1*/ GD_INT_GET(ZONE_OF_CONTROL_ENABLED) <= 0)
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

		// units need to be visible
		if (!pAdjPlot->isVisible(pUnit->getTeam()))
			continue;

		// Loop through all units to see if there's an enemy unit here
		IDInfo* pAdjUnitNode = pAdjPlot->headUnitNode();
		while (pAdjUnitNode != NULL)
		{
			//performance optimization; skip the expensive unit lookup for our own units
			if (pAdjUnitNode->eOwner == pUnit->getOwner())
			{
				pAdjUnitNode = pAdjPlot->nextUnitNode(pAdjUnitNode);
				continue;
			}

			CvUnit* pLoopUnit = GET_PLAYER(pAdjUnitNode->eOwner).getUnit(pAdjUnitNode->iID);
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
					if (pLoopUnit->IsHoveringUnit())
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
	int iModifier = GD_INT_GET(MOVE_DENOMINATOR);
	TerrainTypes eToTerrain = pPlot->getTerrainType();
	FeatureTypes eToFeature = pPlot->getFeatureType();

	if (pUnit->isHillsDoubleMove() && pPlot->isHills())
	{
		iModifier /= 2;
	}
	else if (pUnit->isRiverDoubleMove() && pUnit->plot()->IsAlongSameRiver(pPlot))
	{
		iModifier /= 2;
	}
	else if (pUnit->isTerrainHalfMove(TERRAIN_HILL) && pPlot->isHills())
	{
		iModifier *= 2;
	}
	else if (pUnit->isMountainsDoubleMove() && pPlot->isMountain())
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

	if (pUnit->isTerrainExtraMove(TERRAIN_HILL) && pPlot->isHills())
	{
		iModifier += (GD_INT_GET(MOVE_DENOMINATOR) * pUnit->getTerrainExtraMoveCount(TERRAIN_HILL));
	}
	else if (pUnit->isTerrainExtraMove(eToTerrain))
	{
		iModifier += (GD_INT_GET(MOVE_DENOMINATOR) * pUnit->getTerrainExtraMoveCount(eToTerrain));
	}
	else if (pUnit->isFeatureExtraMove(eToFeature))
	{
		iModifier += (GD_INT_GET(MOVE_DENOMINATOR) * pUnit->getFeatureExtraMoveCount(eToFeature));
	}

	return iModifier;
}

//alternative logic:
// return -1 for double movement; will be interpreted as ignore terrain/feature cost
// return +60 for half movement; will be interpreted as one extra move
int CvUnitMovement::GetMovementCostChangeFromPromotions(const CvUnit* pUnit, const CvPlot* pPlot)
{
	bool bIsFaster = false;
	bool bIsSlower = false;

	TerrainTypes eToTerrain = pPlot->getTerrainType();
	FeatureTypes eToFeature = pPlot->getFeatureType();

	if (pUnit->isHillsDoubleMove() && pPlot->isHills())
	{
		bIsFaster = true;
	}
	else if (pUnit->isRiverDoubleMove() && pUnit->plot()->IsAlongSameRiver(pPlot))
	{
		bIsFaster = true;
	}
	else if (pUnit->isTerrainHalfMove(TERRAIN_HILL) && pPlot->isHills())
	{
		bIsFaster = true;
	}
	else if (pUnit->isMountainsDoubleMove() && pPlot->isMountain())
	{
		bIsFaster = true;
	}
	else if (pUnit->isTerrainDoubleMove(eToTerrain) || pUnit->isFeatureDoubleMove(eToFeature))
	{
		bIsFaster = true;
	}
	else if (pUnit->isTerrainHalfMove(eToTerrain) || pUnit->isFeatureHalfMove(eToFeature))
	{
		bIsSlower = true;
	}

	if (bIsFaster)
		return -1;

	//if slow isn't slow enough, we can make you go even slower!
	int iExtraCost = bIsSlower ? GD_INT_GET(MOVE_DENOMINATOR) : 0;
	if (pUnit->isTerrainExtraMove(TERRAIN_HILL) && pPlot->isHills())
	{
		iExtraCost += GD_INT_GET(MOVE_DENOMINATOR) * pUnit->getTerrainExtraMoveCount(TERRAIN_HILL);
	}
	else if (pUnit->isTerrainExtraMove(eToTerrain))
	{
		iExtraCost += GD_INT_GET(MOVE_DENOMINATOR) * pUnit->getTerrainExtraMoveCount(eToTerrain);
	}
	else if (pUnit->isFeatureExtraMove(eToFeature))
	{
		iExtraCost += GD_INT_GET(MOVE_DENOMINATOR) * pUnit->getFeatureExtraMoveCount(eToFeature);
	}

	return iExtraCost;
}