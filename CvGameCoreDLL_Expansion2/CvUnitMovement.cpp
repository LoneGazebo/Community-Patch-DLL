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
	else
	{
		iRegularCost = ((eFeature == NO_FEATURE) ? (pTerrainInfo ? pTerrainInfo->getMovementCost() : 0) : (pFeatureInfo ? pFeatureInfo->getMovementCost() : 0));

		// Hill cost, except for when a City is present here, then it just counts as flat land
		if((PlotTypes)pToPlot->getPlotType() == PLOT_HILLS && !pToPlot->isCity())
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
	}

	iRegularCost = std::min(iRegularCost, (iBaseMoves * iMoveDenominator));

	if(pFromPlot->isValidRoute(pUnit) && pToPlot->isValidRoute(pUnit) && ((kUnitTeam.isBridgeBuilding() || !(pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot))))))
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

	if (!pUnit->isEmbarked() && (pToPlot->IsAllowsWalkWater() || pFromPlot->IsAllowsWalkWater()))
	{
		return false;
	}

	if(!pFromPlot->isValidDomainForLocation(*pUnit))
	{
		// If we are a land unit that can embark, then do further tests.
		if(pUnit->getDomainType() != DOMAIN_LAND || pUnit->IsHoveringUnit() || pUnit->canMoveAllTerrain() || !pUnit->CanEverEmbark())
			return true;
	}

	// if the unit can embark and we are transitioning from land to water or vice versa
	if(pToPlot->isWater() != pFromPlot->isWater() && pUnit->CanEverEmbark())
	{
		// Is the unit from a civ that can disembark for just 1 MP?
		if(!pToPlot->isWater() && pFromPlot->isWater() && pUnit->isEmbarked() && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
		{
			return false;	// Then no, it does not.
		}

		if(!pUnit->canMoveAllTerrain())
		{
			return true;
		}
	}

	return false;
}

//	---------------------------------------------------------------------------
bool CvUnitMovement::CostsOnlyOne(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	if(!pToPlot->isValidDomainForAction(*pUnit))
	{
		// If we are a land unit that can embark, then do further tests.
		if(pUnit->getDomainType() != DOMAIN_LAND || pUnit->IsHoveringUnit() || pUnit->canMoveAllTerrain() || !pUnit->CanEverEmbark())
			return true;
	}

	CvAssert(!pUnit->IsImmobile());

	if(pUnit->flatMovementCost() || pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}

	// Is the unit from a civ that can disembark for just 1 MP?
	if(!pToPlot->isWater() && pFromPlot->isWater() && pUnit->isEmbarked() && GET_PLAYER(pUnit->getOwner()).GetPlayerTraits()->IsEmbarkedToLandFlatCost())
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnitMovement::IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	if (pUnit->IsIgnoreZOC() || CostsOnlyOne(pUnit, pFromPlot, pToPlot))
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

		for(int iDirection0 = 0; iDirection0 < NUM_DIRECTION_TYPES; iDirection0++)
		{
			CvPlot* pAdjPlot = plotDirection(iFromPlotX, iFromPlotY, ((DirectionTypes)iDirection0));
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
							// this is valid
							if(loop_unit_domain_type == DOMAIN_SEA && unit_domain_type)
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

						// Loop through plots adjacent to the enemy unit and see if it's the same as our unit's Destination Plot
						for(int iDirection2 = 0; iDirection2 < NUM_DIRECTION_TYPES; iDirection2++)
						{
							CvPlot* pEnemyAdjPlot = plotDirection(pAdjPlot->getX(), pAdjPlot->getY(), ((DirectionTypes)iDirection2));
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
