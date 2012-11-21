/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"
#include "CvArmyAI.h"
#include "CvUnit.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "ICvDLLUserInterface.h"
#include "CvAStar.h"
#include "CvInfos.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"

// include after all other headers
#include "LintFree.h"

// Public Functions...

/// Constructor
CvArmyAI::CvArmyAI()
{
	Reset(0, NO_PLAYER, true);
}

/// Destructor
CvArmyAI::~CvArmyAI()
{
	Uninit();
}

/// Initialize
void CvArmyAI::Init(int iID, PlayerTypes eOwner, int iOperationID)
{
	//--------------------------------
	// Init saved data
	Reset(iID, eOwner, iOperationID);
}

/// Deallocate memory
void CvArmyAI::Uninit()
{
	m_FormationEntries.clear();
}

/// Initializes data members that are serialize
void CvArmyAI::Reset(int iID, PlayerTypes eOwner, int iOperationID, bool /* bConstructorCall */)
{
	//--------------------------------
	// Uninit class
	Uninit();

	m_iID = iID;
	m_eOwner = eOwner;
	m_iOperationID = iOperationID;
	m_iCurrentX = -1;
	m_iCurrentY = -1;
	m_iGoalX = -1;
	m_iGoalY = -1;
	m_eDomainType = DOMAIN_LAND;
	m_iFormationIndex = NO_MUFORMATION;
	m_eAIState = NO_ARMYAISTATE;

	m_FormationEntries.clear();
}

/// Delete the army
void CvArmyAI::Kill()
{
	CvAssert(GetOwner() != NO_PLAYER);
	CvAssertMsg(GetID() != FFreeList::INVALID_INDEX, "GetID() is not expected to be equal with FFreeList::INVALID_INDEX");

	int iUnitID;
	iUnitID = GetFirstUnitID();

	while(iUnitID != ARMY_NO_UNIT)
	{
		UnitHandle pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitID);
		if(pThisUnit)
		{
			pThisUnit->setArmyID(FFreeList::INVALID_INDEX);
		}
		iUnitID = GetNextUnitID();
	}

	m_FormationEntries.clear();
}

/// Read from binary data store
void CvArmyAI::read(FDataStream& kStream)
{
	// Init saved data
	Reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iID;
	kStream >> m_eOwner;
	kStream >> m_iCurrentX;
	kStream >> m_iCurrentY;
	kStream >> m_iGoalX;
	kStream >> m_iGoalY;
	kStream >> m_eDomainType;
	kStream >> m_iFormationIndex;
	kStream >> m_eAIState;
	kStream >> m_iOperationID;

	int iEntriesToRead;
	kStream >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		CvArmyFormationSlot slot;
		kStream >> slot;
		m_FormationEntries.push_back(slot);
	}
}

/// Write to binary data store
void CvArmyAI::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iID;
	kStream << m_eOwner;
	kStream << m_iCurrentX;
	kStream << m_iCurrentY;
	kStream << m_iGoalX;
	kStream << m_iGoalY;
	kStream << m_eDomainType;
	kStream << m_iFormationIndex;
	kStream << m_eAIState;
	kStream << m_iOperationID;

	kStream << (int)m_FormationEntries.size();
	for(uint ui = 0; ui < m_FormationEntries.size(); ui++)
	{
		kStream << m_FormationEntries[ui];
	}
}

// ACCESSORS

/// Retrieve this army's ID
int CvArmyAI::GetID()
{
	return m_iID;
}

/// Set this army's ID
void CvArmyAI::SetID(int iID)
{
	m_iID = iID;
}

/// Retrieve this army's team
TeamTypes CvArmyAI::GetTeam() const
{
	if(GetOwner() != NO_PLAYER)
	{
		return GET_PLAYER(GetOwner()).getTeam();
	}

	return NO_TEAM;
}

/// Retrieve current army state
ArmyAIState CvArmyAI::GetArmyAIState() const
{
	return (ArmyAIState) m_eAIState;
}

/// Set current army state
void CvArmyAI::SetArmyAIState(ArmyAIState eNewArmyAIState)
{
	m_eAIState = (int) eNewArmyAIState;
}

/// Find average speed of units in army
int CvArmyAI::GetMovementRate()
{
	int iMovementAverage = 2;   // A reasonable default
	int iNumUnits = 0;
	int iTotalMovementAllowance = 0;
	UnitHandle pUnit;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		iNumUnits++;
		iTotalMovementAllowance += pUnit->baseMoves();
		pUnit = GetNextUnit();
	}

	if(iNumUnits > 0)
	{
		iMovementAverage = (iTotalMovementAllowance + (iNumUnits / 2)) / iNumUnits;
	}

	return iMovementAverage;
}

/// Get center of mass of units in army (account for world wrap!)
CvPlot* CvArmyAI::GetCenterOfMass(DomainTypes eDomainRequired)
{
	CvPlot* pRtnValue = NULL;
	int iTotalX = 0;
	int iTotalY = 0;
	int iNumUnits = 0;
	UnitHandle pUnit;
	int iReferenceUnitX = -1;
	int iWorldWidth = GC.getMap().getGridWidth();

	pUnit = GetFirstUnit();
	if(pUnit)
	{
		iReferenceUnitX = pUnit->getX();
	}

	while(pUnit)
	{
		int iUnitX = pUnit->getX();

		bool bWorldWrapAdjust = false;
		int iDiff = iUnitX - iReferenceUnitX;
		if(abs(iDiff) > (iWorldWidth / 2))
		{
			bWorldWrapAdjust = true;
		}

		if(bWorldWrapAdjust)
		{
			iTotalX += iUnitX + iWorldWidth;
		}
		else
		{
			iTotalX += iUnitX;
		}
		iTotalY += pUnit->getY();
		iNumUnits++;
		pUnit = GetNextUnit();
	}

	if(iNumUnits > 0)
	{
		int iAverageX = (iTotalX + (iNumUnits / 2)) / iNumUnits;
		if(iAverageX >= iWorldWidth)
		{
			iAverageX = iAverageX - iWorldWidth;
		}
		int iAverageY = (iTotalY + (iNumUnits / 2)) / iNumUnits;
		pRtnValue = GC.getMap().plot(iAverageX, iAverageY);
	}

	// Domain check
	if (eDomainRequired != NO_DOMAIN && pRtnValue)
	{
		if (pRtnValue->isWater() && eDomainRequired == DOMAIN_LAND || !pRtnValue->isWater() && eDomainRequired == DOMAIN_SEA)
		{
			// Find an adjacent plot that works
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot *pLoopPlot = plotDirection(pRtnValue->getX(), pRtnValue->getY(), ((DirectionTypes)iI));
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater() && eDomainRequired == DOMAIN_SEA || !pLoopPlot->isWater() && eDomainRequired == DOMAIN_LAND)
					{
						return pLoopPlot;
					}
				}
			}

			// Try two plots out if really having problems
			for (int iDX = -2; iDX <= 2; iDX++)
			{
				for (int iDY = -2; iDY <= 2; iDY++)
				{
					CvPlot *pLoopPlot = plotXYWithRangeCheck(pRtnValue->getX(), pRtnValue->getY(), iDX, iDY, 2);
					if (pLoopPlot)
					{
						if (plotDistance(pRtnValue->getX(), pRtnValue->getY(), pLoopPlot->getX(), pLoopPlot->getY()) == 2)
						{
							if (pLoopPlot->isWater() && eDomainRequired == DOMAIN_SEA || !pLoopPlot->isWater() && eDomainRequired == DOMAIN_LAND)
							{
								return pLoopPlot;
							}
						}
					}
				}
			}

			// Give up - just use location of first unit
			pUnit = GetFirstUnit();
			pRtnValue = pUnit->plot();
		}
	}

	return pRtnValue;
}

/// Return distance from this plot of unit in army farthest away
int CvArmyAI::GetFurthestUnitDistance(CvPlot* pPlot)
{
	int iLargestDistance = 0;
	UnitHandle pUnit;
	int iNewDistance;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		iNewDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
		if(iNewDistance > iLargestDistance)
		{
			iLargestDistance = iNewDistance;
		}
		pUnit = GetNextUnit();
	}
	return iLargestDistance;
}

// FORMATION ACCESSORS

/// Retrieve index of the formation used by this army
int CvArmyAI::GetFormationIndex() const
{
	return m_iFormationIndex;
}

/// Set index of the formation used by this army
void CvArmyAI::SetFormationIndex(int iFormationIndex)
{
	CvArmyFormationSlot slot;

	if(m_iFormationIndex != iFormationIndex)
	{
		m_iFormationIndex = iFormationIndex;

		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(m_iFormationIndex);
		if(thisFormation)
		{
			int iNumSlots = thisFormation->getNumFormationSlotEntries();

			// Build all the formation entries
			m_FormationEntries.clear();
			for(int iI = 0; iI < iNumSlots; iI++)
			{
				slot.SetUnitID(ARMY_NO_UNIT);
				slot.SetTurnAtCheckpoint(ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT);
				m_FormationEntries.push_back(slot);
			}
		}
	}
}

/// How many slots are there in this formation if filled
int CvArmyAI::GetNumFormationEntries() const
{
	return m_FormationEntries.size();
}

/// How many slots do we currently have filled?
int CvArmyAI::GetNumSlotsFilled() const
{
	int iRtnValue = 0;

	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if(m_FormationEntries[iI].m_iUnitID != ARMY_NO_UNIT)
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}

/// What turn will a unit arrive on target?
void CvArmyAI::SetEstimatedTurn(int iSlotID, int iTurns)
{
	int iTurnAtCheckpoint;

	if(iTurns == ARMYSLOT_NOT_INCLUDING_IN_OPERATION || iTurns == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
	{
		iTurnAtCheckpoint = iTurns;
	}
	else
	{
		iTurnAtCheckpoint = GC.getGame().getGameTurn() + iTurns;
	}

	m_FormationEntries[iSlotID].SetTurnAtCheckpoint(iTurnAtCheckpoint);
}

/// What turn will the army as a whole arrive on target?
int CvArmyAI::GetTurnAtNextCheckpoint() const
{
	int iRtnValue = ARMYSLOT_NOT_INCLUDING_IN_OPERATION;

	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if(m_FormationEntries[iI].m_iEstimatedTurnAtCheckpoint == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
		{
			return ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT;
		}
		else if(m_FormationEntries[iI].m_iEstimatedTurnAtCheckpoint > iRtnValue)
		{
			iRtnValue = m_FormationEntries[iI].m_iEstimatedTurnAtCheckpoint;
		}
	}

	return iRtnValue;
}

/// Recalculate when each unit will arrive on target
void CvArmyAI::UpdateCheckpointTurns()
{
	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		// No reestimate for units being built
		if(m_FormationEntries[iI].GetUnitID() != ARMY_NO_UNIT)
		{
			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(m_FormationEntries[iI].GetUnitID());
			CvPlot* pMusterPlot = GC.getMap().plot(GetX(), GetY());
			if(pUnit && pMusterPlot)
			{
				int iTurnsToReachCheckpoint = TurnsToReachTarget(pUnit, pMusterPlot, true /*bReusePaths*/, true, true);
				if(iTurnsToReachCheckpoint < MAX_INT)
				{
					SetEstimatedTurn(iI, iTurnsToReachCheckpoint);
				}
			}
		}
	}
}

/// How many units of this type are in army?
int CvArmyAI::GetUnitsOfType(MultiunitPositionTypes ePosition) const
{
	int iRtnValue = 0;

	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if(m_FormationEntries[iI].m_iUnitID != ARMY_NO_UNIT)
		{
			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(m_FormationEntries[iI].m_iUnitID);
			if(pUnit->getMoves() > 0)
			{
				CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(m_iFormationIndex);
				if(thisFormation)
				{
					const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iI);
					if(thisSlotEntry.m_ePositionType == ePosition)
					{
						iRtnValue++;
					}
				}
			}
		}
	}
	return iRtnValue;
}

/// Can all units in this army move on ocean?
bool CvArmyAI::IsAllOceanGoing()
{
	UnitHandle pUnit;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		if(pUnit->getDomainType() != DOMAIN_SEA && !pUnit->IsHasEmbarkAbility())
		{
			return false;
		}

		// If can move over ocean, not a coastal vessel
		if(pUnit->isTerrainImpassable(TERRAIN_OCEAN))
		{
			return false;
		}

		pUnit = GetNextUnit();
	}

	return true;
}

// UNIT STRENGTH ACCESSORS
/// Total unit power
int CvArmyAI::GetTotalPower()
{
	int iRtnValue = 0;
	int iUnitID;
	iUnitID = GetFirstUnitID();

	while(iUnitID != ARMY_NO_UNIT)
	{
		UnitHandle pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitID);
		if(pThisUnit)
		{
			iRtnValue += pThisUnit->GetPower();
		}
		iUnitID = GetNextUnitID();
	}

	return iRtnValue;
}

// POSITION ACCESSORS

/// Army's current X position
int CvArmyAI::GetX() const
{
	return m_iCurrentX;
}

/// Army's current Y position
int CvArmyAI::GetY() const
{
	return m_iCurrentY;
}

/// Set current army X position
void CvArmyAI::SetX(int iX)
{
	m_iCurrentX = iX;
}

/// Set current army Y position
void CvArmyAI::SetY(int iY)
{
	m_iCurrentY = iY;
}

/// Set current army position, passing in X and Y
void CvArmyAI::SetXY(int iX, int iY)
{
	m_iCurrentX = iX;
	m_iCurrentY = iY;
}

/// Retrieve the army's current plot
CvPlot* CvArmyAI::Plot() const
{
	return GC.getMap().plotCheckInvalid(m_iCurrentX, m_iCurrentY);
}

/// Retrieve the army's current area
int CvArmyAI::GetArea() const
{
	// try to find what plot we are in
	CvPlot* pPlot = GC.getMap().plotCheckInvalid(m_iCurrentX, m_iCurrentY);
	if(pPlot != NULL)
	{
		return pPlot->getArea();
	}
	else
	{
		// since there is no plot return the invalid index
		return FFreeList::INVALID_INDEX;
	}
}

/// Land or sea army?
DomainTypes CvArmyAI::GetDomainType() const
{
	return (DomainTypes) m_eDomainType;
}

/// Set whether a land or sea army
void CvArmyAI::SetDomainType(DomainTypes domainType)
{
	m_eDomainType = domainType;
}

/// Everyone in the water now?
bool CvArmyAI::AreAllInWater()
{
	UnitHandle pUnit;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		if(!pUnit->plot()->isWater())
		{
			return false;
		}
		pUnit = GetNextUnit();
	}

	return true;
}

// GOAL ACCESSORS

/// Retrieve target plot for army movement
CvPlot* CvArmyAI::GetGoalPlot() const
{
	return GC.getMap().plotCheckInvalid(m_iGoalX, m_iGoalY);
}

/// Set target plot for army movement
void CvArmyAI::SetGoalPlot(CvPlot* pGoalPlot)
{
	CvAssertMsg(pGoalPlot, "Setting army goal to a NULL plot - please show Ed and send save.");

	if(pGoalPlot)
	{
		m_iGoalX = pGoalPlot->getX();
		m_iGoalY = pGoalPlot->getY();
	}
	else
	{
		m_iGoalX = -1;
		m_iGoalY = -1;
	}
}

/// Retrieve target plot X coordinate
int CvArmyAI::GetGoalX() const
{
	return m_iGoalX;
}

/// Retrieve target plot Y coordinate
int CvArmyAI::GetGoalY() const
{
	return m_iGoalY;
}

/// Set target for army movement using X, Y coordinates
void CvArmyAI::SetGoalXY(int iX, int iY)
{
	m_iGoalX = iX;
	m_iGoalY = iY;
}

// UNIT HANDLING

/// Add a unit to our army (and we know which slot)
void CvArmyAI::AddUnit(int iUnitID, int iSlotNum)
{
	CvAssertMsg(iUnitID != ARMY_NO_UNIT,"Expect unit to be non-NULL");

	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	UnitHandle pThisUnit = thisPlayer.getUnit(iUnitID);

	// remove this unit from an army if it is already in one
	thisPlayer.removeFromArmy(pThisUnit->getArmyID(), GetID());

	m_FormationEntries[iSlotNum].SetUnitID(iUnitID);
	pThisUnit->setArmyID(GetID());

	// Finally, compute when we think this unit will arrive at the next checkpoint
	CvPlot* pMusterPlot = GC.getMap().plot(GetX(), GetY());
	if(pMusterPlot)
	{
		int iTurnsToReachCheckpoint = TurnsToReachTarget(pThisUnit, pMusterPlot, true /*bReusePaths*/, true, true);
		if(iTurnsToReachCheckpoint < MAX_INT)
		{
			SetEstimatedTurn(iSlotNum, iTurnsToReachCheckpoint);
		}
	}
}

/// Remove a unit from the army
bool CvArmyAI::RemoveUnit(int iUnitToRemoveID)
{
	bool bWasOneOrMoreRemoved = false;
	CvArmyFormationSlot slot;

	for(int iI = 0; iI < (int)m_FormationEntries.size(); iI++)
	{
		slot = m_FormationEntries[iI];
		if(slot.GetUnitID() == iUnitToRemoveID)
		{
			UnitHandle pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitToRemoveID);
			if(pThisUnit)
			{
				// Clears unit's army ID and erase from formation entries
				pThisUnit->setArmyID(FFreeList::INVALID_INDEX);
				m_FormationEntries[iI].SetUnitID(ARMY_NO_UNIT);
				bWasOneOrMoreRemoved = true;

				// Tell the associate operation that a unit was lost
				CvAIOperation* pThisOperation = GET_PLAYER(GetOwner()).getAIOperation(m_iOperationID);
				if(pThisOperation)
				{
					pThisOperation->UnitWasRemoved(GetID(), iI);
				}
			}
		}
	}

	return bWasOneOrMoreRemoved;
}

/// Is this part of an operation that allows units to be poached by tactical AI?
bool CvArmyAI::CanTacticalAIInterruptUnit(int /* iUnitId */) const
{
	// If the operation is still assembling, by all means interrupt it
	if(m_eAIState == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE ||
	        m_eAIState == ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		return true;
	}

	if(m_eOwner >=0 && m_eOwner < MAX_PLAYERS)
	{
		CvAIOperation* op = GET_PLAYER(m_eOwner).getAIOperation(m_iOperationID);
		if(op)
		{
			return op->CanTacticalAIInterruptOperation();
		}
	}
	return false;
}

/// Retrieve units from the army - first call (ARMY_NO_UNIT if none found)
int CvArmyAI::GetFirstUnitID()
{
	m_CurUnitIter = m_FormationEntries.begin();

	if(m_CurUnitIter == m_FormationEntries.end())
	{
		return ARMY_NO_UNIT;
	}
	else
	{
		// First entry could not be filled yet
		while(m_CurUnitIter != m_FormationEntries.end())
		{
			if(m_CurUnitIter->GetUnitID() != ARMY_NO_UNIT)
			{
				return m_CurUnitIter->GetUnitID();
			}
			++m_CurUnitIter;
		}

		return ARMY_NO_UNIT;
	}
}

/// Retrieve units from the army - subsequent call (ARMY_NO_UNIT if none found)
int CvArmyAI::GetNextUnitID()
{
	if(m_CurUnitIter != m_FormationEntries.end())
	{
		++m_CurUnitIter;

		while(m_CurUnitIter != m_FormationEntries.end())
		{
			if(m_CurUnitIter->GetUnitID() != ARMY_NO_UNIT)
			{
				return m_CurUnitIter->GetUnitID();
			}
			++m_CurUnitIter;
		}

		return ARMY_NO_UNIT;
	}
	return ARMY_NO_UNIT;
}

/// Retrieve units from the army - first call (UnitHandle version)
UnitHandle CvArmyAI::GetFirstUnit()
{
	UnitHandle pRtnValue;

	int iUnitID = GetFirstUnitID();
	if(iUnitID != ARMY_NO_UNIT)
	{
		pRtnValue = UnitHandle(GET_PLAYER(m_eOwner).getUnit(iUnitID));

		FAssertMsg(pRtnValue, "Bogus unit in army - tell Ed");
	}

	return pRtnValue;
}

/// Retrieve units from the army - subsequent call (UnitHandle version)
UnitHandle CvArmyAI::GetNextUnit()
{
	UnitHandle pRtnValue;

	int iUnitID = GetNextUnitID();
	if(iUnitID != ARMY_NO_UNIT)
	{
		pRtnValue = UnitHandle(GET_PLAYER(m_eOwner).getUnit(iUnitID));

		FAssertMsg(pRtnValue, "Bogus unit in army - tell Ed");
	}

	return pRtnValue;
}

/// Find first unit who is sitting in this domain
UnitHandle CvArmyAI::GetFirstUnitInDomain(DomainTypes eDomain)
{
	UnitHandle pUnit, pCurrentUnit;

	pCurrentUnit = GetFirstUnit();
	while(pCurrentUnit)
	{
		if(pCurrentUnit->plot()->isWater() && eDomain == DOMAIN_SEA || !pCurrentUnit->plot()->isWater() && eDomain == DOMAIN_LAND)
		{
			return pCurrentUnit;
		}
		pCurrentUnit = GetNextUnit();
	}

	return pUnit;
}

// PER TURN PROCESSING

/// Process another turn for the army
void CvArmyAI::DoTurn()
{
	// do something with the army
	DoDelayedDeath();
}

/// Kill off the army if waiting to die (returns true if army was killed)
bool CvArmyAI::DoDelayedDeath()
{
	if(GetNumSlotsFilled() == 0 && m_eAIState != ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
	{
		Kill();
		return true;
	}

	return false;
}

FDataStream& operator<<(FDataStream& saveTo, const CvArmyAI& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvArmyAI& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const CvArmyFormationSlot& readFrom)
{
	saveTo << readFrom.m_iUnitID;
	saveTo << readFrom.m_iEstimatedTurnAtCheckpoint;
	saveTo << readFrom.m_bStartedOnOperation;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvArmyFormationSlot& writeTo)
{
	loadFrom >> writeTo.m_iUnitID;
	loadFrom >> writeTo.m_iEstimatedTurnAtCheckpoint;
	loadFrom >> writeTo.m_bStartedOnOperation;
	return loadFrom;
}
