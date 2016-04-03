#include "CvGameCoreDLLPCH.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvUnit.h"
#include "CvGlobals.h"
#include "CvUnitMovement.h"
#include "CvGameCoreUtils.h"
//	---------------------------------------------------------------------------
int CvUnitMovement::GetCostsForMove(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	CvPlayerAI& kPlayer = GET_PLAYER(pUnit->getOwner());
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	bool bFasterAlongRiver = pTraits->IsRiverMovementBonus();
	bool bFasterInHills = pTraits->IsFasterInHills();
	bool bIgnoreTerrainCost = pUnit->ignoreTerrainCost();
	bool bAmphibious = pUnit ? pUnit->isRiverCrossingNoPenalty() : false;

	TeamTypes eUnitTeam = pUnit->getTeam();
	CvTeam& kUnitTeam = GET_TEAM(eUnitTeam);
	int iMoveDenominator = GC.getMOVE_DENOMINATOR();
	int iRegularCost = iMoveDenominator;

	//check embarkation
	bool bFullCostEmbarkStateChange = false;
	bool bCheapEmbarkStateChange = false;
	bool bFreeEmbarkStateChange = false;
	if (pUnit->CanEverEmbark())
	{
		if(!pToPlot->needsEmbarkation(pUnit) && pFromPlot->needsEmbarkation(pUnit))
		{
			// Is the unit from a civ that can disembark for just 1 MP?
			if(GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
				bCheapEmbarkStateChange = true;

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			//If city, and player has disembark to city at reduced cost...
			if(pToPlot->isFriendlyCityOrPassableImprovement(pUnit->getOwner()))
			{
				if (kUnitTeam.isCityNoEmbarkCost())
					bFreeEmbarkStateChange = true;
				else if (kUnitTeam.isCityLessEmbarkCost())
					bCheapEmbarkStateChange = true;
			}
#endif

			bFullCostEmbarkStateChange = !(bFreeEmbarkStateChange || bCheapEmbarkStateChange);
		}

		if(pToPlot->needsEmbarkation(pUnit) && !pFromPlot->needsEmbarkation(pUnit))
		{
			// Is the unit from a civ that can embark for just 1 MP?
			if(GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
				bCheapEmbarkStateChange = true;

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
			//If city, and player has embark from city at reduced cost...
			if(pFromPlot->isFriendlyCityOrPassableImprovement(pUnit->getOwner()))
			{
				if (kUnitTeam.isCityNoEmbarkCost())
					bFreeEmbarkStateChange = true;
				else if (kUnitTeam.isCityLessEmbarkCost())
					bCheapEmbarkStateChange = true;
			}
#endif

			bFullCostEmbarkStateChange = !(bFreeEmbarkStateChange || bCheapEmbarkStateChange);
		}
	}

	bool bRiverCrossing = pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot));
	FeatureTypes eFeature = pToPlot->getFeatureType();
	CvFeatureInfo* pFeatureInfo = (eFeature > NO_FEATURE) ? GC.getFeatureInfo(eFeature) : 0;
	TerrainTypes eTerrain = pToPlot->getTerrainType();
	CvTerrainInfo* pTerrainInfo = (eTerrain > NO_TERRAIN) ? GC.getTerrainInfo(eTerrain) : 0;

	//ideally there'd be a check of the river direction to make sure it's the same river
	bool bMovingAlongRiver = pToPlot->isRiver() && pFromPlot->isRiver() && !bRiverCrossing;
	if (bFasterAlongRiver && bMovingAlongRiver)
		bIgnoreTerrainCost = true;

	if (bFasterInHills && pToPlot->isHills())
		bIgnoreTerrainCost = true;

	if((eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE) && pTraits->IsWoodlandMovementBonus())
		bIgnoreTerrainCost = true;

#if defined(MOD_BALANCE_CORE)
	if(MOD_BALANCE_CORE && bAmphibious && bRiverCrossing)
		bIgnoreTerrainCost = true;

	if (MOD_BALANCE_CORE && pTraits->IsMountainPass() && pToPlot->isMountain())
		bIgnoreTerrainCost = true;
#endif

	//----
	// preparation done, now here comes the interesting part
	//----

	if(pToPlot->isRoughGround() && pUnit->IsRoughTerrainEndsTurn())
	{
		// Is a unit's movement consumed for entering rough terrain?
		return INT_MAX;
	}
	else if (bFullCostEmbarkStateChange)
	{
		//embark/disembark ends turn
		return INT_MAX;
	}
	else if(!pToPlot->isRevealed(pUnit->getTeam()) && pUnit->isHuman())
	{
		//moving into unknown tiles ends the turn for humans (to prevent information leakage from the displayed path)
		return INT_MAX;
	}
	else if (bFreeEmbarkStateChange)
	{
		iRegularCost = 0;
	}
	else if (bCheapEmbarkStateChange)
	{
		iRegularCost = iMoveDenominator;
	}
	else if(pUnit->flatMovementCost() || pUnit->getDomainType() == DOMAIN_AIR)
	{
		iRegularCost = iMoveDenominator;
	}
	else if(pToPlot->isCity())
	{
		iRegularCost = iMoveDenominator;
	}
	else
	{
		//if the unit ignores terrain cost, it can still profit from feature bonuses
		if (bIgnoreTerrainCost)
			iRegularCost = 1;
		else
		{
			iRegularCost = ((eFeature == NO_FEATURE) ? (pTerrainInfo ? pTerrainInfo->getMovementCost() : 0) : (pFeatureInfo ? pFeatureInfo->getMovementCost() : 0));

			// Hill cost is hardcoded
			if(pToPlot->isHills() || pToPlot->isMountain())
			{
				iRegularCost += GC.getHILLS_EXTRA_MOVEMENT();
			}

			if(bRiverCrossing && !bAmphibious)
			{
				iRegularCost += GC.getRIVER_EXTRA_MOVEMENT();
			}
		}

		if(iRegularCost > 0)
		{
			iRegularCost = std::max(1, (iRegularCost - pUnit->getExtraMoveDiscount()));
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

	//check routes
	if(pFromPlot->isValidRoute(pUnit) && pToPlot->isValidRoute(pUnit) && (!bRiverCrossing || kUnitTeam.isBridgeBuilding() || bAmphibious))
	{
		CvRouteInfo* pFromRouteInfo = GC.getRouteInfo(pFromPlot->getRouteType());
		int iFromMovementCost = pFromRouteInfo ? pFromRouteInfo->getMovementCost() : 0;
		int iFromFlatMovementCost = pFromRouteInfo ? pFromRouteInfo->getFlatMovementCost() : 0;

		CvRouteInfo* pToRouteInfo = GC.getRouteInfo(pToPlot->getRouteType());
		int iToMovementCost = pToRouteInfo ? pToRouteInfo->getMovementCost() : 0;
		int iToFlatMovementCost = pToRouteInfo ? pToRouteInfo->getFlatMovementCost() : 0;

		//routes only on land
		int iBaseMoves = pUnit->baseMoves(DOMAIN_LAND);

		int iRouteCost = std::max(iFromMovementCost + kUnitTeam.getRouteChange(pFromPlot->getRouteType()), iToMovementCost + kUnitTeam.getRouteChange(pToPlot->getRouteType()));
		int iRouteFlatCost = std::max(iFromFlatMovementCost * iBaseMoves, iToFlatMovementCost * iBaseMoves);

		iRegularCost = std::min(iRegularCost, std::min(iRouteCost,iRouteFlatCost));
	}

	//check border obstacles
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

	return iRegularCost;
}

//	---------------------------------------------------------------------------
int CvUnitMovement::MovementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining)
{
	if(IsSlowedByZOC(pUnit, pFromPlot, pToPlot))
		return iMovesRemaining;

	return MovementCostNoZOC(pUnit,pFromPlot,pToPlot,iMovesRemaining);
}

//	---------------------------------------------------------------------------
int CvUnitMovement::MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining)
{
	int iCost = GetCostsForMove(pUnit, pFromPlot, pToPlot);

	//now, if there was a domain change, our base moves might change
	//make sure that the movement cost is always so high that we never end up with more than the base moves for the new domain
	int iLeftOverMoves = iMovesRemaining-iCost;
	int iMaxMoves = pUnit->baseMoves( pToPlot->getDomain() )*GC.getMOVE_DENOMINATOR();
	if ( iLeftOverMoves > iMaxMoves )
		iCost += (iLeftOverMoves-iMaxMoves);

	return std::min( iCost, iMovesRemaining );
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
