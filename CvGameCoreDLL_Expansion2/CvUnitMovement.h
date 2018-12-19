#ifndef CVUNITMOVEMENT_H
#define CVUNITMOVEMENT_H

#pragma once

// A static class containing movement operations for a unit
class CvUnitMovement
{
public:

	static int MovementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iPrecomputedTerrainFeatureEffect);
	static int MovementCostSelectiveZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iPrecomputedTerrainFeatureEffect, const PlotIndexContainer& plotsToIgnore);
	static int MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iMovesRemaining, int iMaxMoves, int iPrecomputedTerrainFeatureEffect);

protected:
	static int GetCostsForMove(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iPrecomputedTerrainFeatureEffect);
	static bool IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot);
	static bool IsSlowedByZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, const CvPlot* pToPlot, const PlotIndexContainer& plotsToIgnore);
};

#endif // CVUNITMOVEMENT_H