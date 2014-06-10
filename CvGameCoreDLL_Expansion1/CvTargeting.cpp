/*	-------------------------------------------------------------------------------------------------------
	© 1991-2013 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvGameCoreUtils.h"
#include "CvTargeting.h"

// Include this after all other headers.
#include "LintFree.h"

#if !defined(FINAL_RELEASE)
#define TARGETING_DEBUG
#endif

#if defined(TARGETING_DEBUG)
static std::vector<int> ms_lastTargetingPath;

 #define TRACK_LAST_TARGET_PATH_ENTRY(x)	if (x) ms_lastTargetingPath.push_back(x->GetPlotIndex())
 #define CLEAR_TARGET_PATH_DEBUG	ms_lastTargetingPath.clear()
#else
 #define TRACK_LAST_TARGET_PATH_ENTRY ((void)0)
 #define CLEAR_TARGET_PATH_DEBUG	((void)0)
#endif

// Access the last cached targeting path.  Debug only please!
int GetLastTargetingPathIndex(int plotIndex)
{
#if defined(TARGETING_DEBUG)
	int iIndex = 0;
	for (std::vector<int>::const_iterator itr = ms_lastTargetingPath.begin(); itr != ms_lastTargetingPath.end(); ++itr)
	{
		if ((*itr) == plotIndex)
			return iIndex;
		++iIndex;
	}
#endif

	return -1;
}

/*
	Quadrant sides
			
		 5 / \ 0
		 4|   | 1
		 3 \ / 2
*/

// Offset lookup.  The index is the direction that was blocked
// The entries are the offsets to the direction to look back to.
static int ms_LookBackX[6] = 
{  
	 0,		// 3
	-1,		// 4
	-1,		// 5  
	 0,		// 0
	 1,		// 1
	 1		// 2
};
static int ms_LookBackY[6] = 
{ 
	-1,		// 3
	 0,		// 4
	 1,		// 5
	 1,		// 0
	 0,		// 1
	-1		// 2
};

// Lookup an offset of one hex to another to get a direction.
// Since the offsets can be -1, 0, 1 on each axis, add one to the index.
static int ms_HexDirection[3][3] =
{
	{
		0/*NA*/,	4,			5
	},
	{
		3,			0 /*NA*/,	0
	},
	{
		2,			1,			0/*NA*/
	}
};

#define STRICT_LOOSE_CUTOFF	2
//	---------------------------------------------------------------------------
//	Test if the team can see from the supplied hex to a displacement in HEXSPACE
//	from the start hex.
//	This is the strict version that prevents you from seeing through partially blocked hexes
//	after a certain distance threshold is reached.  Even when below the distance threshold
//	more than one blockage will cause the LOS to fail.
static bool CanSeeDisplacementPlot_Strict(int startX, int startY, int dx, int dy, int fromLevel)
{
	int originalDX = dx;
	int originalDY = dy;

	CvMap& kMap = GC.getMap();

	// Use Bresenham's line algorithm, with a slight modification, to step through the cells.
	// The modification is that if we are stepping in the upper-right or lower-left quadrant,
	// and we need to take a step on the 'short' axis, we will check the plot with just the
	// adjustment to the X axis, before we do the normal check for the step on the X and Y
	// axis.  This is needed because in those quadrants a change on both the X and Y axis 
	// is not contiguous.

	// Because the hex grid is low-resolution, checks are also made to allow the ray to continue 
	// around a blockage if that blockage obscures only part of the next hex cell along the ray.
	// This is done by 'looking back' at the alternate path the ray could have taken as it is 
	// traversing the hex grid.

	// Make DX and DY positive and adjust the step constant for the opposite axis.
	int stepY;
	if (dy < 0) 
	{ 
		dy = -dy;  
		stepY = -1; 
	} 
	else 
		stepY = 1; 

	int stepX;
	if (dx < 0) 
	{ 
		dx = -dx;  
		stepX = -1; 
	} 
	else 
		stepX = 1;

	dy <<= 1;
	dx <<= 1;

	int currentDX = 0;
	int currentDY = 0;

	// If in an odd quadrant, stepping down on the short axis requires two checks to be contiguous
	bool oddQuadrant = stepX != stepY;
	// Is the line going to go such that it travels through the centers of the hexes?
	// If so, we don't have to worry about testing to see if we can look around blocked hexes.
	bool straightThrough = dx == 0 || dy == 0;

	// Start from the next cell after the source plot
	int lastDX = currentDX;
	int lastDY = currentDY;
	int lookbackDirection = -1;
	int stepCount = 0;
	int iBlockedCount = 0;

	// If dx is greater than dy, scan along the x axis, adjusting the Y as needed
	if (dx > dy) 
	{
		int shortAxisStepFactor = (dx >> 1); 
		while (currentDX != originalDX) 
		{
			shortAxisStepFactor += dy;

			if (shortAxisStepFactor >= dx) 
			{
				shortAxisStepFactor -= dx;

				if (oddQuadrant)
				{
					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{
					currentDX += stepX;

					// Look back to see if we were possibly blocked by the alternate route to this hex.  We will ignore the check if inside the rings that allow the user to look around obstacles.
					if (!straightThrough && lookbackDirection != -1)
					{
						CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						{
							++iBlockedCount;
							if (stepCount > STRICT_LOOSE_CUTOFF || iBlockedCount >= 2)
								return false;
						}
					}

					// Don't test the destination, we only want to test the plots between the start and the destination
					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						return false;

					lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

					++stepCount;
					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDX += stepX;

			// Look back to see if we were possibly blocked by the alternate route to this hex.
			if (!straightThrough && lookbackDirection != -1)
			{
				CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
				TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
				if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				{
					if (stepCount > STRICT_LOOSE_CUTOFF || ++iBlockedCount >= 2)
						return false;
				}
			}

			// Don't test the destination, we only want to test the plots between the start and the destination
			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				return false;

			lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

			++stepCount;
			lastDX = currentDX; lastDY = currentDY;
		}
	} 
	else 
	{
		// Y axis runs are longer, scan along them and adjust the X as needed.
		// The short axis step is primed slightly differently if the step ratio is 2:1
		// so that the initial step goes in a more favorable direction
		int shortAxisStepFactor = (dy!=dx && dx != 0 && (dy>>1) == dx)?((dy >> 1) - dx):(dy >> 1);
		//int fraction = (dy >> 1);
		while (currentDY != originalDY) 
		{
			shortAxisStepFactor += dx;

			if (shortAxisStepFactor >= dy) 
			{
				shortAxisStepFactor -= dy;

				if (oddQuadrant)
				{
					// A change in the X and Y axis will yield a contiguous line
					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{
					// Adjust just the X axis, then check so that the line is contiguous.
					currentDX += stepX;

					// Look back to see if we were possibly blocked by the alternate route to this hex.
					if (!straightThrough && lookbackDirection != -1)
					{
						CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						{
							if (stepCount > STRICT_LOOSE_CUTOFF || ++iBlockedCount >= 2)
								return false;
						}
					}

					// Don't test the destination, we only want to test the plots between the start and the destination
					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						return false;

					lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

					++stepCount;
					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDY += stepY;

			// Look back to see if we were possibly blocked by the alternate route to this hex.
			if (!straightThrough && lookbackDirection != -1)
			{
				CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
				TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
				if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				{
					if (stepCount > STRICT_LOOSE_CUTOFF || ++iBlockedCount >= 2)
						return false;
				}
			}

			// Don't test the destination, we only want to test the plots between the start and the destination
			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				return false;

			lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

			++stepCount;
			lastDX = currentDX; lastDY = currentDY;
		}
	}

	return true;
}

//	---------------------------------------------------------------------------
//	Test if the team can see from the supplied hex to a displacement in HEXSPACE
//	from the start hex.
//	This is the 'loose' version that lets you see around partially blocked hexes
static bool CanSeeDisplacementPlot_Loose(int startX, int startY, int dx, int dy, int fromLevel)
{
	int originalDX = dx;
	int originalDY = dy;

	CvMap& kMap = GC.getMap();

	// Use Bresenham's line algorithm, with a slight modification, to step through the cells.
	// The modification is that if we are stepping in the upper-right or lower-left quadrant,
	// and we need to take a step on the 'short' axis, we will check the plot with just the
	// adjustment to the X axis, before we do the normal check for the step on the X and Y
	// axis.  This is needed because in those quadrants a change on both the X and Y axis 
	// is not contiguous.

	// Because the hex grid is low-resolution, checks are also made to allow the ray to continue 
	// around a blockage if that blockage obscures only part of the next hex cell along the ray.
	// This is done by 'looking back' at the alternate path the ray could have taken as it is 
	// traversing the hex grid.

	// Make DX and DY positive and adjust the step constant for the opposite axis.
	int stepY;
	if (dy < 0) 
	{ 
		dy = -dy;  
		stepY = -1; 
	} 
	else 
		stepY = 1; 

	int stepX;
	if (dx < 0) 
	{ 
		dx = -dx;  
		stepX = -1; 
	} 
	else 
		stepX = 1;

	dy <<= 1;
	dx <<= 1;

	int currentDX = 0;
	int currentDY = 0;

	bool bBlocked = false;
	// If in an odd quadrant, stepping down on the short axis requires two checks to be contiguous
	bool oddQuadrant = stepX != stepY;
	// Is the line going to go such that it travels through the centers of the hexes?
	// If so, we don't have to worry about testing to see if we can look around blocked hexes.
	bool straightThrough = dx == 0 || dy == 0;

	// Start from the next cell after the source plot
	int lastDX = currentDX;
	int lastDY = currentDY;
	int blockedDirection = 0;

	// If dx is greater than dy, scan along the x axis, adjusting the Y as needed
	if (dx > dy) 
	{
		int shortAxisStepFactor = (dx >> 1); 
		while (currentDX != originalDX) 
		{
			shortAxisStepFactor += dy;

			if (shortAxisStepFactor >= dx) 
			{
				shortAxisStepFactor -= dx;

				if (oddQuadrant)
				{
					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{
					currentDX += stepX;

					// If we were blocked by the previous hex, see if we are able to look around the adjacent one if we split between them
					if (bBlocked)
					{
						if (!straightThrough)
						{
							CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
							if(passThroughPlot)
							{
								TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
								bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
							}

							if (bBlocked)
								return false;
						}
						else
							return false;
					}

					// Don't test the destination, we only want to test the plots between the start and the destination
					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
					{
						bBlocked = true;
						blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
					}

					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDX += stepX;

			// If we were blocked by the previous hex, see if we are able to look around the adjacent one if we split between them
			if (bBlocked)
			{
				if (!straightThrough)	// If going along a single axis, we are looking directly down the middle of a hex so there is nothing to look around.
				{
					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
					if(passThroughPlot)
					{
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
					}

					if (bBlocked)
						return false;
				}
				else
					return false;
			}

			// Don't test the destination, we only want to test the plots between the start and the destination
			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
			{
				bBlocked = true;
				blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
			}

			lastDX = currentDX; lastDY = currentDY;
		}
	} 
	else 
	{
		// Y axis runs are longer, scan along them and adjust the X as needed.
		// The short axis step is primed slightly differently if the step ratio is 2:1
		// so that the initial step goes in a more favorable direction
		int shortAxisStepFactor = (dy!=dx && dx != 0 && (dy>>1) == dx)?((dy >> 1) - dx):(dy >> 1);
		//int fraction = (dy >> 1);
		while (currentDY != originalDY) 
		{
			shortAxisStepFactor += dx;

			if (shortAxisStepFactor >= dy) 
			{
				shortAxisStepFactor -= dy;

				if (oddQuadrant)
				{
					// A change in the X and Y axis will yield a contiguous line
					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{
					// Adjust just the X axis, then check so that the line is contiguous.
					currentDX += stepX;

					// If we were blocked by the previous hex, see if we are able to look around the adjacent one if we split between them
					if (bBlocked)
					{
						if (!straightThrough)	// If going along a single axis, we are looking directly down the middle of a hex so there is nothing to look around.
						{
							CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
							if(passThroughPlot)
							{
								TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
								bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
							}

							if (bBlocked)
								return false;
						}
						else
							return false;
					}

					// Don't test the destination, we only want to test the plots between the start and the destination
					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
					{
						bBlocked = true;
						blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
					}

					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDY += stepY;

			// If we were blocked by the previous hex, see if we are able to look around the adjacent one if we split between them
			if (bBlocked)
			{
				if (!straightThrough)	// If going along a single axis, we are looking directly down the middle of a hex so there is nothing to look around.
				{
					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
					if(passThroughPlot)
					{
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
					}

					if (bBlocked)
						return false;
				}
				else
					return false;
			}

			// Don't test the destination, we only want to test the plots between the start and the destination
			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
			{
				bBlocked = true;
				blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
			}

			lastDX = currentDX; lastDY = currentDY;
		}
	}

	return true;
}

//	---------------------------------------------------------------------------
//	Test if the team can see from the supplied plot to a displacement in HEXSPACE
//	from the start plot.
//
//	This combines two ways of doing the LOS calculation to mimic the way the 
//	a unit calculates what plots it can actually see.  The reason we don't just use the
//	vision calculation is that it employs a flood fill system from each plot in the unit's
//	max vision range to build up the completed visible plot list and updates the total visual
//	status for the player, there is no per-unit data.  Using a direct ray-cast for targeting
//	queries is faster, though it requires some special casing to return the same results.
//	The visual calculations allow the unit to 'look-around' partial blockages that are within the first
//	ring of hexes around a unit.  Hexes after the second ring are blocked by any obstruction.

bool CvTargeting::CanSeeDisplacementPlot(int startX, int startY, int dx, int dy, int fromLevel)
{
	CLEAR_TARGET_PATH_DEBUG;

	// convert the start coord to hex-space coordinates
	// No need to convert the Y coord, it is the same in both systems
	startX = xToHexspaceX(startX, startY);

	CvMap& kMap = GC.getMap();
	CvPlot* pPlot = PlotFromHex(kMap, startX + dx, startY + dy);
	if(pPlot != NULL)
	{
		int iHexDistanceBetweenPlots = hexDistance(dx, dy);
		// assume that we can always see our plot and the plot next door
		if(iHexDistanceBetweenPlots <= 1)
			return true;

		// If over STRICT_LOOSE_CUTOFF away, any blockage past the STRICT_LOOSE_CUTOFF will result in not being able to target.
		if (iHexDistanceBetweenPlots > STRICT_LOOSE_CUTOFF /*&& GC.getGame().isOption("STRICT_RANGE_TARGETING")*/)
			return CanSeeDisplacementPlot_Strict(startX, startY, dx, dy, fromLevel);
		else
			return CanSeeDisplacementPlot_Loose(startX, startY, dx, dy, fromLevel);

	}

	return false;
}
