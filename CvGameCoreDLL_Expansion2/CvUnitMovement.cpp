#include "CvGameCoreDLLPCH.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvUnit.h"
#include "CvGlobals.h"
#include "CvUnitMovement.h"
#include "CvGameCoreUtils.h"
//	---------------------------------------------------------------------------
void CvUnitMovement::GetCostsForMove(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iBaseMoves, int& iRegularCost, int& iRouteCost, int& iRouteFlatCost)
{
	CvPlayerAI& kPlayer = GET_PLAYER(pUnit->getOwner());
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	bool bFasterAlongRiver = pTraits->IsFasterAlongRiver();
	bool bFasterInHills = pTraits->IsFasterInHills();
	bool bIgnoreTerrainCost = pUnit->ignoreTerrainCost();
#if defined(MOD_BALANCE_CORE)
	bool bAmphibious = false;
	bool bSuperAmphibious = false;
	if(pUnit != NULL)
	{
		bSuperAmphibious = pUnit->isRiverCrossingNoPenalty() && pTraits->IsFasterAlongRiver();
		bAmphibious = pUnit->isRiverCrossingNoPenalty();
	}
#endif
	//int iBaseMoves = pUnit->baseMoves(isWater()?DOMAIN_SEA:NO_DOMAIN);
	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);
	int iMoveDenominator = GC.getMOVE_DENOMINATOR();
	bool bRiverCrossing = pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot));
	FeatureTypes eFeature = pToPlot->getFeatureType();
	CvFeatureInfo* pFeatureInfo = (eFeature > NO_FEATURE) ? GC.getFeatureInfo(eFeature) : 0;
	TerrainTypes eTerrain = pToPlot->getTerrainType();
	CvTerrainInfo* pTerrainInfo = (eTerrain > NO_TERRAIN) ? GC.getTerrainInfo(eTerrain) : 0;

	if(bIgnoreTerrainCost || (bFasterAlongRiver && pToPlot->isRiver()) || (bFasterInHills && pToPlot->isHills()))
	{
		iRegularCost = 1;
	}
#if defined(MOD_BALANCE_CORE)
	else if(pToPlot->isCity() && (pTerrainInfo->getMovementCost() > 1))
	{
		iRegularCost = 1;
	}
#endif
	else
	{
		iRegularCost = ((eFeature == NO_FEATURE) ? (pTerrainInfo ? pTerrainInfo->getMovementCost() : 0) : (pFeatureInfo ? pFeatureInfo->getMovementCost() : 0));

		// Hill cost, except for when a City is present here, then it just counts as flat land
		if(pToPlot->isHills() || pToPlot->isMountain() && !pToPlot->isCity())
		{
			iRegularCost += GC.getHILLS_EXTRA_MOVEMENT();
		}

		if(iRegularCost > 0)
		{
			iRegularCost = std::max(1, (iRegularCost - pUnit->getExtraMoveDiscount()));
		}
	}

	// Is a unit's movement consumed for entering rough terrain?
	if(pToPlot->isRoughGround() && pUnit->IsRoughTerrainEndsTurn())
	{
		iRegularCost = INT_MAX;
	}

	else
	{
		if(!(bIgnoreTerrainCost || bFasterAlongRiver) && bRiverCrossing)
		{
			iRegularCost += GC.getRIVER_EXTRA_MOVEMENT();
		}

		iRegularCost *= iMoveDenominator;

		if(pToPlot->isHills() && pUnit->isHillsDoubleMove())
		{
			iRegularCost /= 2;
		}

		else if((eFeature == NO_FEATURE) ? pUnit->isTerrainDoubleMove(eTerrain) : pUnit->isFeatureDoubleMove(eFeature))
		{
			iRegularCost /= 2;
		}

#if defined(MOD_PROMOTIONS_HALF_MOVE)
		else if((pToPlot->getFeatureType() == NO_FEATURE) ? pUnit->isTerrainHalfMove(pToPlot->getTerrainType()) : pUnit->isFeatureHalfMove(pToPlot->getFeatureType()))
		{
			iRegularCost *= 2;
		}
#endif
	}

	iRegularCost = std::min(iRegularCost, (iBaseMoves * iMoveDenominator));
#if defined(MOD_BALANCE_CORE)
	if(pFromPlot->isValidRoute(pUnit) && pToPlot->isValidRoute(pUnit) && (kUnitTeam.isBridgeBuilding() || bAmphibious || !bRiverCrossing))
#else
	if(pFromPlot->isValidRoute(pUnit) && pToPlot->isValidRoute(pUnit) && ((kUnitTeam.isBridgeBuilding() || !(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot))))))
#endif
	{
		CvRouteInfo* pFromRouteInfo = GC.getRouteInfo(pFromPlot->getRouteType());
		CvAssert(pFromRouteInfo != NULL);

		int iFromMovementCost = (pFromRouteInfo != NULL)? pFromRouteInfo->getMovementCost() : 0;
		int iFromFlatMovementCost = (pFromRouteInfo != NULL)? pFromRouteInfo->getFlatMovementCost() : 0;

		CvRouteInfo* pRouteInfo = GC.getRouteInfo(pToPlot->getRouteType());
		CvAssert(pRouteInfo != NULL);

		int iMovementCost = (pRouteInfo != NULL)? pRouteInfo->getMovementCost() : 0;
		int iFlatMovementCost = (pRouteInfo != NULL)? pRouteInfo->getFlatMovementCost() : 0;

		iRouteCost = std::max(iFromMovementCost + kUnitTeam.getRouteChange(pFromPlot->getRouteType()), iMovementCost + kUnitTeam.getRouteChange(pToPlot->getRouteType()));
		iRouteFlatCost = std::max(iFromFlatMovementCost * iBaseMoves, iFlatMovementCost * iBaseMoves);
	}
	else if(pUnit->getOwner() == pToPlot->getOwner() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE) && pTraits->IsMoveFriendlyWoodsAsRoad())
	{
		CvRouteInfo* pRoadInfo = GC.getRouteInfo(ROUTE_ROAD);
		iRouteCost = pRoadInfo->getMovementCost();
		iRouteFlatCost = pRoadInfo->getFlatMovementCost() * iBaseMoves;
	}
#if defined(MOD_BALANCE_CORE)
	else if(MOD_BALANCE_CORE && bSuperAmphibious && bRiverCrossing)
	{
		CvRouteInfo* pRoadInfo = GC.getRouteInfo(ROUTE_ROAD);
		iRouteCost = pRoadInfo->getMovementCost();
		iRouteFlatCost = (pRoadInfo->getFlatMovementCost() * iBaseMoves);
	}
	else if(MOD_BALANCE_CORE && bAmphibious && !bSuperAmphibious && bRiverCrossing)
	{
		CvRouteInfo* pRoadInfo = GC.getRouteInfo(ROUTE_ROAD);
		iRouteCost = pRoadInfo->getMovementCost() * 2;
		iRouteFlatCost = (pRoadInfo->getFlatMovementCost() * iBaseMoves) * 2;
	}
	else if (MOD_BALANCE_CORE && pTraits->IsMountainPass() && pToPlot->isMountain())
	{
		CvRouteInfo* pRoadInfo = GC.getRouteInfo(ROUTE_ROAD);
		iRouteCost = pRoadInfo->getMovementCost() * 2;
		iRouteFlatCost = (pRoadInfo->getFlatMovementCost() * iBaseMoves) * 2;
	}
#endif
	else
	{
		iRouteCost = INT_MAX;
		iRouteFlatCost = INT_MAX;
	}

	TeamTypes eTeam = pToPlot->getTeam();
	if(eTeam != NO_TEAM)
	{
		CvTeam* pPlotTeam = &GET_TEAM(eTeam);
		CvPlayer* pPlotPlayer = &GET_PLAYER(pToPlot->getOwner());

		// Great Wall increases movement cost by 1
		if(pPlotTeam->isBorderObstacle() || pPlotPlayer->isBorderObstacle())
		{
			if(!pToPlot->isWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{
				// Don't apply penalty to OUR team or teams we've given open borders to
				if(eUnitTeam != eTeam && !pPlotTeam->IsAllowsOpenBordersToTeam(eUnitTeam))
				{
					iRegularCost += iMoveDenominator;
				}
			}
		}
#if defined(MOD_BALANCE_CORE)
		else if (eUnitTeam != eTeam)
		{
			//cheap checks first
			if(!pToPlot->isWater() && pUnit->getDomainType() == DOMAIN_LAND)
			{
				//Plots worked by city with movement debuff reduce movement speed.
				CvCity* pCity = pToPlot->getWorkingCity();
				if(pCity != NULL)
				{
					if(pCity->GetBorderObstacleCity() > 0)
					{
						// Don't apply penalty to OUR team or teams we've given open borders to
						if(!pPlotTeam->IsAllowsOpenBordersToTeam(eUnitTeam))
						{
							iRegularCost += iMoveDenominator;
						}
					}
				}
			}
			if(pToPlot->isWater() && (pUnit->getDomainType() == DOMAIN_SEA || pUnit->isEmbarked()))
			{
				//Plots worked by city with movement debuff reduce movement speed.
				CvCity* pCity = pToPlot->getWorkingCity();
				if(pCity != NULL)
				{
					if(pCity->GetBorderObstacleWater() > 0)
					{
						// Don't apply penalty to OUR team or teams we've given open borders to
						if(!pPlotTeam->IsAllowsOpenBordersToTeam(eUnitTeam))
						{
							iRegularCost += iMoveDenominator;
						}
					}
				}
			}
		}
#endif
	}
}

//	---------------------------------------------------------------------------
int CvUnitMovement::MovementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iBaseMoves, int iMaxMoves, int iMovesRemaining /*= 0*/)
{
	int iRegularCost;
	int iRouteCost;
	int iRouteFlatCost;

	CvAssertMsg(pToPlot->getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if(ConsumesAllMoves(pUnit, pFromPlot, pToPlot))
	{
		if (iMovesRemaining > 0)
			return iMovesRemaining;
		else
			return iMaxMoves;
	}
	else if(CostsOnlyOne(pUnit, pFromPlot, pToPlot))
	{
		return GC.getMOVE_DENOMINATOR();
	}
	else if(IsSlowedByZOC(pUnit, pFromPlot, pToPlot))
	{
		if (iMovesRemaining > 0)
			return iMovesRemaining;
		else
			return iMaxMoves;
	}

	GetCostsForMove(pUnit, pFromPlot, pToPlot, iBaseMoves, iRegularCost, iRouteCost, iRouteFlatCost);

	return std::max(1, std::min(iRegularCost, std::min(iRouteCost, iRouteFlatCost)));
}

//	---------------------------------------------------------------------------
int CvUnitMovement::MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iBaseMoves, int iMaxMoves, int iMovesRemaining /*= 0*/)
{
	int iRegularCost;
	int iRouteCost;
	int iRouteFlatCost;

	CvAssertMsg(pToPlot->getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if(ConsumesAllMoves(pUnit, pFromPlot, pToPlot))
	{
		if (iMovesRemaining > 0)
			return iMovesRemaining;
		else
			return iMaxMoves;
	}
	else if(CostsOnlyOne(pUnit, pFromPlot, pToPlot))
	{
		return GC.getMOVE_DENOMINATOR();
	}

	GetCostsForMove(pUnit, pFromPlot, pToPlot, iBaseMoves, iRegularCost, iRouteCost, iRouteFlatCost);

	return std::max(1, std::min(iRegularCost, std::min(iRouteCost, iRouteFlatCost)));
}

//	---------------------------------------------------------------------------
bool CvUnitMovement::ConsumesAllMoves(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	if(!pToPlot->isRevealed(pUnit->getTeam()) && pUnit->isHuman())
	{
		return true;
	}

	if(!pFromPlot->isValidDomainForLocation(*pUnit))
	{
		return true;
	}

	// if the unit can (or has to) embark and we are transitioning from land to water or vice versa
	if(pUnit->CanEverEmbark())
	{
		bool bCanMoveFreely = false;
		TeamTypes eUnitTeam = pUnit->getTeam();
		CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);

		//disembarking?
		if (!pToPlot->needsEmbarkation(pUnit) && pFromPlot->needsEmbarkation(pUnit))
		{
			//trait?
			if(GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
				return false;

			//If city, and player has disembark to city at no cost...
			else if(pToPlot->isCity() && (pToPlot->getOwner() == pUnit->getOwner()) )
			{
				if (kUnitTeam.isCityNoEmbarkCost() || kUnitTeam.isCityLessEmbarkCost())
					return false;
			}

			return true;
		}

		//embarkation?
		if (pToPlot->needsEmbarkation(pUnit) && !pFromPlot->needsEmbarkation(pUnit))
		{
			//If city, and player has embark from city at no cost...
			if(pFromPlot->isCity() && (pFromPlot->getOwner() == pUnit->getOwner()) )
			{
				if (kUnitTeam.isCityNoEmbarkCost() || kUnitTeam.isCityLessEmbarkCost())
					return false;
			}

			return true;
		}
	}

	return false;
}

//	---------------------------------------------------------------------------
bool CvUnitMovement::CostsOnlyOne(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	CvAssert(!pUnit->IsImmobile());

	if(pUnit->flatMovementCost() || pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}

	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);
	if (pUnit->CanEverEmbark())
	{
		if(!pToPlot->needsEmbarkation(pUnit) && pFromPlot->needsEmbarkation(pUnit))
		{
			// Is the unit from a civ that can disembark for just 1 MP?
			if(GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
			{
				return true;
			}

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			//If city, and player has disembark to city at reduced cost...
			if(pToPlot->isCity() && (pToPlot->getOwner() == pUnit->getOwner()) && kUnitTeam.isCityNoEmbarkCost())
			{
				return true;
			}
			//If city, and player has disembark to city at reduced cost...
			else if(pToPlot->isCity() && (pToPlot->getOwner() == pUnit->getOwner()) && kUnitTeam.isCityLessEmbarkCost())
			{
				return true;
			}
		}

		if(pToPlot->needsEmbarkation(pUnit) && !pFromPlot->needsEmbarkation(pUnit))
		{
			//If city, and player has disembark to city at reduced cost...
			if(pFromPlot->isCity() && (pFromPlot->getOwner() == pUnit->getOwner()) && kUnitTeam.isCityNoEmbarkCost())
			{
				return true;
			}
			//If city, and player has disembark to city at reduced cost...
			if(pFromPlot->isCity() && (pFromPlot->getOwner() == pUnit->getOwner()) && kUnitTeam.isCityLessEmbarkCost())
			{
				return true;
			}
		}
#else
		}
#endif
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnitMovement::IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	if (pUnit->IsIgnoreZOC())
	{
		return false;
	}

	// Zone of Control
	if(GC.getZONE_OF_CONTROL_ENABLED() > 0)
	{
		IDInfo* pAdjUnitNode;
		CvUnit* pLoopUnit;

		int iFromPlotX = pFromPlot->getX();
		int iFromPlotY = pFromPlot->getY();
		int iToPlotX = pToPlot->getX();
		int iToPlotY = pToPlot->getY();
		TeamTypes unit_team_type     = pUnit->getTeam();
		DomainTypes unit_domain_type = pUnit->getDomainType();
		bool bIsVisibleEnemyUnit     = pToPlot->isVisibleEnemyUnit(pUnit);
		CvTeam& kUnitTeam = GET_TEAM(unit_team_type);

#if defined(MOD_BALANCE_CORE)
		CvPlot** aPlotsToCheck1 = GC.getMap().getNeighborsUnchecked(pFromPlot);
		for(int iCount1=0; iCount1<NUM_DIRECTION_TYPES; iCount1++)
		{
			CvPlot* pAdjPlot = aPlotsToCheck1[iCount1];
#else
		for(int iDirection0 = 0; iDirection0 < NUM_DIRECTION_TYPES; iDirection0++)
		{
			CvPlot* pAdjPlot = plotDirection(iFromPlotX, iFromPlotY, ((DirectionTypes)iDirection0));
#endif
			if(NULL != pAdjPlot)
			{
				// check city zone of control
				if(pAdjPlot->isEnemyCity(*pUnit))
				{
					// Loop through plots adjacent to the enemy city and see if it's the same as our unit's Destination Plot
					for(int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
					{
						CvPlot* pEnemyAdjPlot = plotDirection(pAdjPlot->getX(), pAdjPlot->getY(), ((DirectionTypes)iDirection));
						if(NULL != pEnemyAdjPlot)
						{
							// Destination adjacent to enemy city?
							if(pEnemyAdjPlot->getX() == iToPlotX && pEnemyAdjPlot->getY() == iToPlotY)
							{
								return true;
							}
						}
					}
				}

				pAdjUnitNode = pAdjPlot->headUnitNode();
				// Loop through all units to see if there's an enemy unit here
				while(pAdjUnitNode != NULL)
				{
					if((pAdjUnitNode->eOwner >= 0) && pAdjUnitNode->eOwner < MAX_PLAYERS)
					{
						pLoopUnit = (GET_PLAYER(pAdjUnitNode->eOwner).getUnit(pAdjUnitNode->iID));
					}
					else
					{
						pLoopUnit = NULL;
					}

					pAdjUnitNode = pAdjPlot->nextUnitNode(pAdjUnitNode);

					if(!pLoopUnit) continue;

					TeamTypes unit_loop_team_type = pLoopUnit->getTeam();

					if(pLoopUnit->isInvisible(unit_team_type,false)) continue;

					// Combat unit?
					if(!pLoopUnit->IsCombatUnit())
					{
						continue;
					}

					// At war with this unit's team?
					if(unit_loop_team_type == BARBARIAN_TEAM || kUnitTeam.isAtWar(unit_loop_team_type))
					{
						// Same Domain?
						DomainTypes loop_unit_domain_type = pLoopUnit->getDomainType();
						if(loop_unit_domain_type != unit_domain_type)
						{
#if defined(MOD_BALANCE_CORE)
							// hovering units always exert a ZOC
							if (pLoopUnit->IsHoveringUnit() || pLoopUnit->canMoveAllTerrain())
							{
								// continue on
							}
							// water unit can ZoC embarked land unit
							else if(loop_unit_domain_type == DOMAIN_SEA && pUnit->isEmbarked())
#else
							// water unit can ZoC land unit
							else if(loop_unit_domain_type == DOMAIN_SEA && unit_domain_type)
#endif
							{
								// continue on
							}
							else
							{
								continue;
							}
						}

						// Embarked?
						if(unit_domain_type == DOMAIN_LAND && pLoopUnit->isEmbarked())
						{
							continue;
						}
						if(loop_unit_domain_type == DOMAIN_LAND && pUnit->isEmbarked())
						{
							continue;
						}

						// Loop through plots adjacent to the enemy unit and see if it's the same as our unit's Destination Plot
#if defined(MOD_BALANCE_CORE)
						CvPlot** aPlotsToCheck2 = GC.getMap().getNeighborsUnchecked(pAdjPlot);
						for(int iCount2=0; iCount2<NUM_DIRECTION_TYPES; iCount2++)
						{
							const CvPlot* pEnemyAdjPlot = aPlotsToCheck2[iCount2];
#else
						for(int iDirection2 = 0; iDirection2 < NUM_DIRECTION_TYPES; iDirection2++)
						{
							CvPlot* pEnemyAdjPlot = plotDirection(pAdjPlot->getX(), pAdjPlot->getY(), ((DirectionTypes)iDirection2));
#endif
							if(!pEnemyAdjPlot)
							{
								continue;
							}

							// Don't check Enemy Unit's plot
							if(!bIsVisibleEnemyUnit)
							{
								// Destination adjacent to enemy unit?
								if(pEnemyAdjPlot->getX() == iToPlotX && pEnemyAdjPlot->getY() == iToPlotY)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
