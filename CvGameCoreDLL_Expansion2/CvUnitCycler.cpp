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
CvUnitCycler::CvUnitCycler(CvPlayer& kPlayer)
{
	m_pkPlayer = &kPlayer;
	Clear();
}

//	---------------------------------------------------------------------------
void CvUnitCycler::Clear()
{
	m_iAnchorUnitID = -1;
	m_iAnchorTurn = -1;
	m_iAnchorPlotIndex = -1;
	m_eAnchorUnitType = NO_UNIT;
	m_aiShownUnits.clear();
	m_iShownTurn = -1;
	m_iRetraceUnitID = -1;
}

//	---------------------------------------------------------------------------
// Remembers where the player started giving this unit orders, so the next unit is picked near there
void CvUnitCycler::SetAnchor(const CvUnit& kUnit)
{
	// More orders for the same unit this turn keep the tile its orders began on
	int iTurn = GC.getGame().getGameTurn();
	if (kUnit.GetID() == m_iAnchorUnitID && iTurn == m_iAnchorTurn)
		return;

	m_iAnchorUnitID = kUnit.GetID();
	m_iAnchorTurn = iTurn;
	m_iAnchorPlotIndex = kUnit.plot() ? kUnit.plot()->GetPlotIndex() : -1;
	m_eAnchorUnitType = kUnit.getUnitType();
}

//	---------------------------------------------------------------------------
// The unit needing orders closest to where the player is working; the head unit itself if it still needs orders
CvUnit* CvUnitCycler::GetClosestNeedingOrders(const CvUnit* pkHeadUnit) const
{
	return FindClosest(pkHeadUnit, false, false, false);
}

//	-----------------------------------------------------------------------------------------------
// The next or previous unit key. Forward takes the closest unit needing orders that this round has not shown yet,
// and starts a new round (*pbWrap) once every unit was shown. Back retraces the units shown this round, and Next after Back
// walks forward through them again.
CvUnit* CvUnitCycler::Cycle(const CvUnit* pkHeadUnit, bool bForward, bool bWorkers, bool* pbWrap)
{
	if (pbWrap != NULL)
		*pbWrap = false;

	// A new turn starts a new round
	int iTurn = GC.getGame().getGameTurn();
	if (m_iShownTurn != iTurn)
	{
		m_aiShownUnits.clear();
		m_iShownTurn = iTurn;
		m_iRetraceUnitID = -1;
	}

	int iHeadIndex = pkHeadUnit ? FindShown(pkHeadUnit->GetID()) : -1;

	if (!bForward)
	{
		int iFrom = (iHeadIndex != -1) ? iHeadIndex : (int)m_aiShownUnits.size();
		for (int i = iFrom - 1; i >= 0; i--)
		{
			CvUnit* pLoopUnit = m_pkPlayer->getUnit(m_aiShownUnits[i]);
			if (pLoopUnit && NeedsOrders(*pLoopUnit, bWorkers))
			{
				m_iRetraceUnitID = pLoopUnit->GetID();
				return pLoopUnit;
			}
		}

		return NULL;
	}

	// Right after stepping back, forward retraces the round first. Only then: a unit the player picked by hand
	// that happens to be in this round continues with the closest unit, not with whatever the round showed after it.
	if (iHeadIndex != -1 && pkHeadUnit->GetID() == m_iRetraceUnitID)
	{
		for (int i = iHeadIndex + 1; i < (int)m_aiShownUnits.size(); i++)
		{
			CvUnit* pLoopUnit = m_pkPlayer->getUnit(m_aiShownUnits[i]);
			if (pLoopUnit && NeedsOrders(*pLoopUnit, bWorkers))
			{
				m_iRetraceUnitID = pLoopUnit->GetID();
				return pLoopUnit;
			}
		}
	}
	m_iRetraceUnitID = -1;

	if (pkHeadUnit)
		MarkShown(pkHeadUnit->GetID());

	CvUnit* pNextUnit = FindClosest(pkHeadUnit, true, bWorkers, true);
	if (pNextUnit == NULL)
	{
		// Every unit was shown: start a new round
		if (pbWrap != NULL)
			*pbWrap = true;

		m_aiShownUnits.clear();
		pNextUnit = FindClosest(pkHeadUnit, true, bWorkers, false);

		// Only the head unit is left
		if (pNextUnit == NULL && pkHeadUnit && NeedsOrders(*pkHeadUnit, bWorkers))
			pNextUnit = m_pkPlayer->getUnit(pkHeadUnit->GetID());

		if (pkHeadUnit)
			MarkShown(pkHeadUnit->GetID());
	}

	if (pNextUnit)
		MarkShown(pNextUnit->GetID());

	return pNextUnit;
}

//	---------------------------------------------------------------------------
// How unlike two unit types are, in tiles of extra distance: 0 same type; 1 same combat class or role, or same domain
// and both military or both civilian; 2 otherwise
static int GetUnitTypeUnlikeness(UnitTypes eA, UnitTypes eB)
{
	if (eA == eB && eA != NO_UNIT)
		return 0;

	CvUnitEntry* pkA = (eA != NO_UNIT) ? GC.getUnitInfo(eA) : NULL;
	CvUnitEntry* pkB = (eB != NO_UNIT) ? GC.getUnitInfo(eB) : NULL;
	if (!pkA || !pkB)
		return 2;

	if ((pkA->GetUnitCombatType() != NO_UNITCOMBAT && pkA->GetUnitCombatType() == pkB->GetUnitCombatType()) || pkA->GetDefaultUnitAIType() == pkB->GetDefaultUnitAIType())
		return 1;

	bool bMilitaryA = pkA->GetCombat() > 0 || pkA->GetRangedCombat() > 0;
	bool bMilitaryB = pkB->GetCombat() > 0 || pkB->GetRangedCombat() > 0;
	if (pkA->GetDomainType() == pkB->GetDomainType() && bMilitaryA == bMilitaryB)
		return 1;

	return 2;
}

//	---------------------------------------------------------------------------
// The unit needing orders closest to where the player is working: the tile the head unit's orders began on this turn, not
// where its moves took it. With no head unit, the last anchor or the capital stands in. A unit unlike the head unit counts as up to two tiles further,
// so after the last bomber on a carrier the bombers on the next carrier come before the carrier itself. Ties go to the
// closer unit, then to the oldest.
CvUnit* CvUnitCycler::FindClosest(const CvUnit* pkHeadUnit, bool bSkipHead, bool bWorkers, bool bSkipShown) const
{
	const CvPlot* pkAnchorPlot = GC.getMap().plotByIndex(m_iAnchorPlotIndex);
	const CvPlot* pkFrom = NULL;
	UnitTypes eLikeType = NO_UNIT;

	if (pkHeadUnit)
	{
		bool bOrderedThisTurn = pkHeadUnit->GetID() == m_iAnchorUnitID && m_iAnchorTurn == GC.getGame().getGameTurn();
		pkFrom = (bOrderedThisTurn && pkAnchorPlot) ? pkAnchorPlot : pkHeadUnit->plot();
		eLikeType = pkHeadUnit->getUnitType();
	}
	else if (pkAnchorPlot)
	{
		pkFrom = pkAnchorPlot;
		eLikeType = m_eAnchorUnitType;
	}
	else if (m_pkPlayer->getCapitalCity())
	{
		pkFrom = m_pkPlayer->getCapitalCity()->plot();
	}

	if (!pkFrom)
		return NULL;

	int iBestScore = MAX_INT;
	CvUnit* pBestUnit = NULL;
	int iLoop = 0;
	for (CvUnit* pLoopUnit = m_pkPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pkPlayer->nextUnit(&iLoop))
	{
		if (!NeedsOrders(*pLoopUnit, bWorkers) || !pLoopUnit->plot())
			continue;

		if (bSkipHead && pLoopUnit == pkHeadUnit)
			continue;

		if (bSkipShown && FindShown(pLoopUnit->GetID()) != -1)
			continue;

		int iDistance = plotDistance(*pkFrom, *pLoopUnit->plot());

		// Distance plus unlikeness decides; between equals the closer unit wins (no map is 1000 tiles across)
		int iScore = (iDistance + GetUnitTypeUnlikeness(eLikeType, pLoopUnit->getUnitType())) * 1000 + iDistance;
		if (iScore < iBestScore)
		{
			iBestScore = iScore;
			pBestUnit = pLoopUnit;
		}
	}

	return pBestUnit;
}

//	---------------------------------------------------------------------------
// The same units that keep the end turn button asking for unit orders
bool CvUnitCycler::NeedsOrders(const CvUnit& kUnit, bool bWorkers)
{
	if (!kUnit.ReadyToSelect() || kUnit.TurnProcessed())
		return false;

	return !bWorkers || kUnit.AI_getUnitAIType() == UNITAI_WORKER || kUnit.AI_getUnitAIType() == UNITAI_WORKER_SEA;
}

//	---------------------------------------------------------------------------
int CvUnitCycler::FindShown(int iUnitID) const
{
	for (int i = 0; i < (int)m_aiShownUnits.size(); i++)
	{
		if (m_aiShownUnits[i] == iUnitID)
			return i;
	}

	return -1;
}

//	---------------------------------------------------------------------------
void CvUnitCycler::MarkShown(int iUnitID)
{
	int iIndex = FindShown(iUnitID);
	if (iIndex != -1)
		m_aiShownUnits.erase(m_aiShownUnits.begin() + iIndex);

	m_aiShownUnits.push_back(iUnitID);
}

//	---------------------------------------------------------------------------
// Saves still carry the turn-start route the cycler used to follow; it is read and dropped, and written empty
FDataStream& operator>>(FDataStream& loadFrom, CvUnitCycler& writeTo)
{
	writeTo.Clear();
	CLinkList<int> kOldRoute;
	loadFrom >> kOldRoute;
	return loadFrom;
}

//	---------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvUnitCycler&)
{
	CLinkList<int> kNoRoute;
	saveTo << kNoRoute;
	return saveTo;
}
