/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.
	All other marks and trademarks are the property of their respective owners.
	All rights reserved.
	------------------------------------------------------------------------------------------------------- */

#ifndef CVUNITCYCLER_H
#define CVUNITCYCLER_H
#pragma once
#include "LinkedList.h"
class CvPlayer;
class CvUnit;

// Picks which unit the human player is taken to next: always the closest unit that needs orders,
// measured from where the player is working. All state here is local UI state and is not saved.
class CvUnitCycler
{
public:

	CvUnitCycler(CvPlayer& kPlayer);

	void		Clear();
	void		SetAnchor(const CvUnit& kUnit);
	CvUnit*		GetClosestNeedingOrders(const CvUnit* pkHeadUnit) const;
	CvUnit*		Cycle(const CvUnit* pkHeadUnit, bool bForward, bool bWorkers, bool* pbWrap);

protected:
	CvUnit*		FindClosest(const CvUnit* pkHeadUnit, bool bSkipHead, bool bWorkers, bool bSkipShown) const;
	static bool	NeedsOrders(const CvUnit& kUnit, bool bWorkers);
	int			FindShown(int iUnitID) const;
	void		MarkShown(int iUnitID);

	CvPlayer* m_pkPlayer;

	// Where the player started giving the last unit orders
	int			m_iAnchorUnitID;
	int			m_iAnchorTurn;
	int			m_iAnchorPlotIndex;
	UnitTypes	m_eAnchorUnitType;

	// Units the next unit key showed or skipped this round, oldest first
	std::vector<int> m_aiShownUnits;
	int			m_iShownTurn;
	// The unit Previous or a retracing Next went to; Next from it keeps retracing the round
	int			m_iRetraceUnitID;

	friend FDataStream& operator>>(FDataStream& loadFrom, CvUnitCycler& writeTo);
	friend FDataStream& operator<<(FDataStream& saveTo, const CvUnitCycler& readFrom);
};

FDataStream& operator>>(FDataStream& loadFrom, CvUnitCycler& writeTo);
FDataStream& operator<<(FDataStream& saveTo, const CvUnitCycler& readFrom);

#endif
