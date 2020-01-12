#ifndef CVUNITMOVEMENT_H
#define CVUNITMOVEMENT_H

#pragma once

// A static class containing movement operations for a unit
class CvUnitMovement
{
public:

	static int MovementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iTerrainFeatureCostMultiplierFromPromotions = -1, int iTerrainFeatureCostAdderFromPromotions =  -1);
	static int MovementCostSelectiveZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, 
										int iTerrainFeatureCostMultiplierFromPromotions = -1, int iTerrainFeatureCostAdderFromPromotions = -1, const PlotIndexContainer& plotsToIgnore = PlotIndexContainer());
	static int MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iTerrainFeatureCostMultiplierFromPromotions = -1, int iTerrainFeatureCostAdderFromPromotions = -1);
	static int GetMovementCostMultiplierFromPromotions(const CvUnit* pUnit, const CvPlot* pPlot);
	static int GetMovementCostAdderFromPromotions(const CvUnit* pUnit, const CvPlot* pPlot);

	static int GetCostsForMove(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iTerrainFeatureCostMultiplierFromPromotions = -1, int iTerrainFeatureCostAdderFromPromotions = -1);
	static bool IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot);
	static bool IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, const PlotIndexContainer& plotsToIgnore);
};

#endif // CVUNITMOVEMENT_H