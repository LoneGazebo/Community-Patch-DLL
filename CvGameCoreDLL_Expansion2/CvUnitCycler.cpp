/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvUnitCycler.h"
#include "CvPlayer.h"
#include "CvUnit.h"
#include "CvGameCoreUtils.h"

//	---------------------------------------------------------------------------
CvUnitCycler::CvUnitCycler()
{
}

//	-----------------------------------------------------------------------------------------------
// Returns the next unit in the cycle...
CvUnit *CvUnitCycler::Cycle(CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap)
{
	if (!pUnit)
		return NULL;

	if (pbWrap != NULL)
		*pbWrap = false;

	//---------------------------------------------------------------------------------------------
	// just return the closest suitable unit, no bookkeeping required
	//---------------------------------------------------------------------------------------------
	int iUnitLoop;
	int iMinDist = INT_MAX;
	CvUnit* pClosestUnit = NULL;
	for (CvUnit* pLoopUnit = GET_PLAYER(pUnit->getOwner()).firstUnit(&iUnitLoop); pLoopUnit; pLoopUnit = GET_PLAYER(pUnit->getOwner()).nextUnit(&iUnitLoop))
	{
		if (pLoopUnit == pUnit || !pLoopUnit->ReadyToSelect())
			continue;

		if (bWorkers && pLoopUnit->AI_getUnitAIType() != UNITAI_WORKER && pLoopUnit->AI_getUnitAIType() != UNITAI_WORKER_SEA)
			continue;

		int iDist = plotDistance(*pUnit->plot(),*pLoopUnit->plot());

		//use forward/backward flag to introduce a slight bias
		if (bForward && pUnit->GetID() > pLoopUnit->GetID())
			iDist++;
		if (!bForward && pUnit->GetID() < pLoopUnit->GetID())
			iDist++;

		if ( iDist<iMinDist )
		{
			iMinDist = iDist;
			pClosestUnit = pLoopUnit;
		}
	}

	return pClosestUnit;
}