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
#include "CvDiplomacyAI.h"
#include "FStlContainerSerialization.h"

// include after all other headers
#include "LintFree.h"

// Public Functions...

/// Constructor
CvArmyAI::CvArmyAI()
{
	Reset();
}

/// Destructor
CvArmyAI::~CvArmyAI()
{
	ReleaseAllUnits();
}

/// Deallocate memory
void CvArmyAI::ReleaseAllUnits()
{
	for (size_t i = 0; i<m_FormationEntries.size(); i++)
	{
		if (!m_FormationEntries[i].IsUsed())
			continue;

		RemoveUnit( m_FormationEntries[i].GetUnitID() );
	}

	m_FormationEntries.clear();
}

/// Initializes data members that are serialize
void CvArmyAI::Reset(int iID, PlayerTypes eOwner, int iOperationID)
{
	ReleaseAllUnits();

	m_iID = iID;
	m_eOwner = eOwner;
	m_iOperationID = iOperationID;
	m_iCurrentX = INVALID_PLOT_COORD;
	m_iCurrentY = INVALID_PLOT_COORD;
	m_iGoalX = INVALID_PLOT_COORD;
	m_iGoalY = INVALID_PLOT_COORD;
	m_eDomainType = DOMAIN_LAND;
	m_iFormationIndex = NO_MUFORMATION;
	m_eAIState = NO_ARMYAISTATE;
#if defined(MOD_BALANCE_CORE)
	m_bOceanMoves = false;
#endif

	m_FormationEntries.clear();
}

/// Delete the army
void CvArmyAI::Kill()
{
	CvAssert(GetOwner() != NO_PLAYER);
	CvAssertMsg(GetID() != -1, "GetID() is not expected to be equal with -1");

	ReleaseAllUnits();

	CvAIOperation* pOperation = GET_PLAYER(GetOwner()).getAIOperation(m_iOperationID);
	if (pOperation)
		pOperation->DeleteArmyAI(m_iID);

	GET_PLAYER(GetOwner()).deleteArmyAI(m_iID);
}

/// Read from binary data store
void CvArmyAI::Read(FDataStream& kStream)
{
	// Init saved data
	Reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

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
#if defined(MOD_BALANCE_CORE)
	kStream >> m_bOceanMoves;
#endif

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
void CvArmyAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

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
#if defined(MOD_BALANCE_CORE)
	kStream << m_bOceanMoves;
#endif

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
	CvUnit* pUnit;

	pUnit = GetFirstUnit();
	while(pUnit)
	{
		iNumUnits++;
		iTotalMovementAllowance += pUnit->baseMoves();
		pUnit = GetNextUnit(pUnit);
	}

	if(iNumUnits > 0)
	{
		iMovementAverage = (iTotalMovementAllowance + (iNumUnits / 2)) / iNumUnits;
	}

	return iMovementAverage;
}

/// Get center of mass of units in army (account for world wrap!)
CvPlot* CvArmyAI::GetCenterOfMass(bool bClampToUnit, float* pfVarX, float* pfVarY)
{
	int iTotalX = 0;
	int iTotalY = 0;
	int iNumUnits = 0;

	CvUnit* pUnit = GetFirstUnit();
	if (!pUnit)
		return NULL;

	int iTotalX2 = 0;
	int iTotalY2 = 0;
	int iWorldWidth = GC.getMap().getGridWidth();
	int iWorldHeight = GC.getMap().getGridHeight();

	//the first unit is our reference ...
	int iRefX = pUnit->getX();
	int iRefY = pUnit->getY();
	iNumUnits++;

	pUnit = GetNextUnit(pUnit);
	while(pUnit)
	{
		int iDX = pUnit->getX() - iRefX;
		int iDY = pUnit->getY() - iRefY;

		if (GC.getMap().isWrapX())
		{
			if( iDX > +(iWorldWidth / 2))
				iDX -= iWorldWidth;
			if( iDX < -(iWorldWidth / 2))
				iDX += iWorldWidth;
		}
		if (GC.getMap().isWrapY())
		{
			if( iDY > +(iWorldHeight / 2))
				iDY -= iWorldHeight;
			if( iDY < -(iWorldHeight / 2))
				iDY += iWorldHeight;
		}

		iTotalX += iDX;
		iTotalY += iDY;
		iTotalX2 += iDX*iDX;
		iTotalY2 += iDY*iDY;
		iNumUnits++;

		pUnit = GetNextUnit(pUnit);
	}

	//this is for debugging
	float fNUnits = (float)iNumUnits;
	float fVarX = (iTotalX2/fNUnits) - (iTotalX/fNUnits)*(iTotalX/fNUnits);
	float fVarY = (iTotalY2/fNUnits) - (iTotalY/fNUnits)*(iTotalY/fNUnits);

	//finally, compute average
	float fAvgX = (iTotalX/fNUnits) + iRefX;
	float fAvgY = (iTotalY/fNUnits) + iRefY;

	//rounding to nearest integer
	int iAvgX = fAvgX > 0 ? int(fAvgX + 0.5f) : int(fAvgX - 0.5f);
	int iAvgY = fAvgY > 0 ? int(fAvgY + 0.5f) : int(fAvgY - 0.5f);

	if (fVarX > 64 || fVarY > 64)
	{
		CvString msg = CvString::format("Warning: Army %d with %d units Center of Mass (%d,%d) has a large variance (%.2f,%.2f)\n", GetID(), iNumUnits, iAvgX, iAvgY, fVarX, fVarY);
		OutputDebugString( msg.c_str() );
	}

	//this handles wrapped coordinates
	CvPlot* pCOM = GC.getMap().plot(iAvgX, iAvgY);
	if (!pCOM)
		return NULL;

	if (pfVarX)
		*pfVarX = fVarX;
	if (pfVarY)
		*pfVarY = fVarY;

	if (bClampToUnit)
	{
		//don't return it directly but use the plot of the closest unit
		pUnit = GetFirstUnit();
		std::vector<SPlotWithScore> vPlots;
		while (pUnit)
		{
			int iDistToCOM = plotDistance(*pUnit->plot(),*pCOM);
			int iDistToTarget = plotDistance(pUnit->getX(),pUnit->getY(),GetGoalX(),GetGoalY());
			vPlots.push_back( SPlotWithScore(pUnit->plot(),iDistToCOM*100+iDistToTarget) );

			pUnit = GetNextUnit(pUnit);
		}

		if (vPlots.empty())
			return NULL;

		//this sorts ascending!
		std::sort(vPlots.begin(),vPlots.end());
		return vPlots.front().pPlot;
	}
	else
		return pCOM;
}

/// Return distance from this plot of unit in army farthest away
int CvArmyAI::GetFurthestUnitDistance(CvPlot* pPlot)
{
	int iLargestDistance = 0;
	CvUnit* pUnit = GetFirstUnit();

	while(pUnit)
	{
		int iNewDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
		if(iNewDistance > iLargestDistance)
		{
			iLargestDistance = iNewDistance;
		}
		pUnit = GetNextUnit(pUnit);
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
	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iFormationIndex);
	if(m_iFormationIndex != iFormationIndex && thisFormation)
	{
		ReleaseAllUnits();

		m_iFormationIndex = iFormationIndex;
		m_FormationEntries = vector<CvArmyFormationSlot>(thisFormation->getNumFormationSlotEntries());
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
		if(m_FormationEntries[iI].IsUsed())
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}


/// What turn will the army as a whole arrive on target?
int CvArmyAI::GetTurnOfLastUnitAtNextCheckpoint() const
{
	int iRtnValue = INT_MAX;

	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if (!m_FormationEntries[iI].IsUsed())
			continue;

		if(m_FormationEntries[iI].GetTurnsToCheckpoint() < 0)
			return INT_MAX;
		
		iRtnValue = MAX(iRtnValue, m_FormationEntries[iI].GetTurnsToCheckpoint());
	}

	return iRtnValue;
}

/// Recalculate when each unit will arrive at the current army position, whatever that is
void CvArmyAI::UpdateCheckpointTurnsAndRemoveBadUnits()
{
	CvAIOperation* pOperation = GET_PLAYER(GetOwner()).getAIOperation(GetOperationID());
	if (!pOperation)
		return;

	//should be updated before calling this ...
	CvPlot* pCurrentArmyPlot = GC.getMap().plot(GetX(), GetY());

	//kick out damaged units, but if we're defending or escorting, no point in running away
	if (pOperation->IsOffensive())
	{
		for (unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
		{
			if (!m_FormationEntries[iI].IsUsed())
				continue;

			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(m_FormationEntries[iI].m_iUnitID);
			if (pUnit == NULL)
				continue;

			//let tactical AI handle units which are badly hurt
			if (pUnit->shouldHeal())
				RemoveUnit(m_FormationEntries[iI].GetUnitID());
			else if (pUnit->getArmyID() != GetID())
				//failsafe - make sure the army ID is set
				pUnit->setArmyID(GetID());
		}
	}

	//update for all units in this army. ignore the ones still being built
	int iGatherTolerance = pOperation->GetGatherTolerance(this, pCurrentArmyPlot);
	for(unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if (!m_FormationEntries[iI].IsUsed())
			continue;

		CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(m_FormationEntries[iI].GetUnitID());
		if(pUnit && pCurrentArmyPlot)
		{
			if(plotDistance(*pUnit->plot(),*pCurrentArmyPlot)<iGatherTolerance)
			{
				m_FormationEntries[iI].SetTurnsToCheckpoint(0);
			}
			else
			{
				//be generous with the flags here, for some ops the muster point may be far away and intermittendly occupied by foreign units ...
				int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ZOC;
				int iTurnsToReachCheckpoint = pUnit->TurnsToReachTarget(pCurrentArmyPlot, iFlags, pOperation->GetMaximumRecruitTurns());

				//if we're already moving to target, the current army plot is moving, so we cannot check progress against ...
				if ( iTurnsToReachCheckpoint==INT_MAX ||
					 (GetArmyAIState()==ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP && !m_FormationEntries[iI].IsMakingProgressTowardsCheckpoint(iTurnsToReachCheckpoint)) )
				{
					CvString strMsg;
					strMsg.Format("Removing %s %d from army %d because no progress to checkpoint (%d:%d). ETA %d, previously %d.", 
						pUnit->getName().c_str(), m_FormationEntries[iI].GetUnitID(), GetID(), pCurrentArmyPlot->getX(), pCurrentArmyPlot->getY(),
						iTurnsToReachCheckpoint, m_FormationEntries[iI].m_iPrevEstimatedTurnsToCheckpoint);
					pOperation->LogOperationSpecialMessage(strMsg);
					RemoveUnit(m_FormationEntries[iI].GetUnitID());
				}
				else
					m_FormationEntries[iI].SetTurnsToCheckpoint(iTurnsToReachCheckpoint);
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
		if (m_FormationEntries[iI].IsUsed())
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
	return iRtnValue;
}

/// Can all units in this army move on ocean?
bool CvArmyAI::IsAllOceanGoing()
{
	CvUnit* pUnit = GetFirstUnit();
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

		pUnit = GetNextUnit(pUnit);
	}

	return true;
}

// UNIT STRENGTH ACCESSORS
/// Total unit power
int CvArmyAI::GetTotalPower()
{
	int iRtnValue = 0;

	for (size_t i = 0; i<m_FormationEntries.size(); i++)
	{
		if (!m_FormationEntries[i].IsUsed())
			continue;

		CvUnit* pThisUnit = GET_PLAYER(GetOwner()).getUnit(m_FormationEntries[i].GetUnitID());
		if (pThisUnit)
			iRtnValue += pThisUnit->GetPower();
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

/// Set current army position, passing in X and Y
void CvArmyAI::SetXY(int iX, int iY)
{
	m_iCurrentX = iX;
	m_iCurrentY = iY;
}

/// Retrieve the army's current plot
CvPlot* CvArmyAI::GetCurrentPlot() const
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
		return -1;
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
		m_iGoalX = INVALID_PLOT_COORD;
		m_iGoalY = INVALID_PLOT_COORD;
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

#if defined(MOD_BALANCE_CORE)
//Water parameters
void CvArmyAI::SetOceanMoves(bool bValue)
{
	if (bValue != m_bOceanMoves)
	{
		m_bOceanMoves = bValue;
	}
}

/// Retrieve target plot X coordinate
bool CvArmyAI::NeedOceanMoves() const
{
	return m_bOceanMoves;
}
#endif
// UNIT HANDLING

/// Add a unit to our army (and we know which slot)
void CvArmyAI::AddUnit(int iUnitID, int iSlotNum)
{
	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	CvUnit* pThisUnit = thisPlayer.getUnit(iUnitID);
	if (!pThisUnit)
		return;

	// remove this unit from an army if it is already in one
	thisPlayer.removeFromArmy(pThisUnit->getArmyID(), GetID());

	// add it to this army
	m_FormationEntries[iSlotNum] = CvArmyFormationSlot(); //reset
	m_FormationEntries[iSlotNum].SetUnitID(iUnitID);
	pThisUnit->setArmyID(GetID());

	//do this in two steps to avoid triggering the sanity check
	pThisUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
	pThisUnit->setTacticalMove(AI_TACTICAL_OPERATION);
	pThisUnit->SetTurnProcessed(true); //ignore for tactical moves from now

	// Finally, compute when we think this unit will arrive at the next checkpoint
	CvPlot* pMusterPlot = GC.getMap().plot(GetX(), GetY());
	if(pMusterPlot)
	{
		int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ZOC;
		int iTurnsToReachCheckpoint = pThisUnit->TurnsToReachTarget(pMusterPlot, iFlags, GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY());
		if(iTurnsToReachCheckpoint < MAX_INT)
		{
			m_FormationEntries[iSlotNum].SetTurnsToCheckpoint(iTurnsToReachCheckpoint);
		}

		if (GC.getLogging() && GC.getAILogging())
		{
			CvAIOperation* pOperation = GET_PLAYER(GetOwner()).getAIOperation(GetOperationID());
			if (pOperation)
			{
				CvString strMsg;
				strMsg.Format("Added %s %d to slot %d in army %d. ETA at (%d:%d) is %d ", pThisUnit->getName().c_str(),
					m_FormationEntries[iSlotNum].GetUnitID(), iSlotNum, GetID(), pMusterPlot->getX(), pMusterPlot->getY(), iTurnsToReachCheckpoint);
				pOperation->LogOperationSpecialMessage(strMsg);
			}
		}
	}
}

/// Remove a unit from the army
bool CvArmyAI::RemoveUnit(int iUnitToRemoveID)
{
	if (iUnitToRemoveID == -1)
		return false;

	for(size_t iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		CvArmyFormationSlot& slot = m_FormationEntries[iI];
		if(slot.GetUnitID() == iUnitToRemoveID)
		{
			slot.SetUnitID(-1);

			CvUnit* pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitToRemoveID);
			if(pThisUnit)
			{
				// Clears unit's army ID and erase from formation entries
				pThisUnit->setArmyID(-1);
				pThisUnit->AI_setUnitAIType(pThisUnit->getUnitInfo().GetDefaultUnitAIType());

				// Tell the associate operation that a unit was lost
				CvAIOperation* pThisOperation = GET_PLAYER(GetOwner()).getAIOperation(m_iOperationID);
				if(pThisOperation)
					pThisOperation->UnitWasRemoved(GetID(), iI);

				// Make the unit available for tactical moves
				pThisUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
				GET_PLAYER(GetOwner()).GetTacticalAI()->AddCurrentTurnUnit(pThisUnit);

				return true;
			}
		}
	}

	return false;
}

/// Retrieve units from the army - first call (CvUnit* version)
CvUnit* CvArmyAI::GetFirstUnit()
{
	//for unknown reasons we sometimes get into endless loops because the same unit is present in multiple slots
	//so we need to sanitize this
	set<int> unitsInArmy;
	for (size_t i = 0; i < m_FormationEntries.size(); i++)
	{
		if (!m_FormationEntries[i].IsUsed())
			continue;

		int iUnitID = m_FormationEntries[i].GetUnitID();
		if (unitsInArmy.find(iUnitID) == unitsInArmy.end())
			unitsInArmy.insert(iUnitID);
		else
		{
			//bad case
			m_FormationEntries[i].SetUnitID(-1);
			continue;
		}
	}

	for (size_t i = 0; i < m_FormationEntries.size(); i++)
	{
		if (!m_FormationEntries[i].IsUsed())
			continue;

		int iUnitID = m_FormationEntries[i].GetUnitID();
		CvUnit* pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitID);
		if (pThisUnit)
			return pThisUnit;
	}

	return NULL;
}

/// Retrieve units from the army - subsequent call (CvUnit* version)
CvUnit* CvArmyAI::GetNextUnit(CvUnit* pCurUnit)
{
	//inefficient but not in the hot path ...
	bool bFoundEntryPoint = false;

	for (size_t i = 0; i < m_FormationEntries.size(); i++)
	{
		if (!m_FormationEntries[i].IsUsed())
			continue;

		CvUnit* pThisUnit = GET_PLAYER(GetOwner()).getUnit(m_FormationEntries[i].GetUnitID());
		if (!pThisUnit)
			continue;
		
		if (!bFoundEntryPoint)
			bFoundEntryPoint = (pThisUnit == pCurUnit);
		else if (pThisUnit != pCurUnit) //protect against the same unit being present in multiple slots (actual bug)
			return pThisUnit;
	}

	return NULL;
}

// PER TURN PROCESSING

/// does this army need to be deleted
bool CvArmyAI::IsDelayedDeath()
{
	return (GetNumSlotsFilled() == 0 && m_eAIState != ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
}

FDataStream& operator<<(FDataStream& saveTo, const CvArmyAI& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvArmyAI& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const CvArmyFormationSlot& readFrom)
{
	saveTo << readFrom.m_iUnitID;
	saveTo << readFrom.m_iEstimatedTurnsToCheckpoint;
	saveTo << readFrom.m_iPrevEstimatedTurnsToCheckpoint;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvArmyFormationSlot& writeTo)
{
	loadFrom >> writeTo.m_iUnitID;
	loadFrom >> writeTo.m_iEstimatedTurnsToCheckpoint;
	loadFrom >> writeTo.m_iPrevEstimatedTurnsToCheckpoint;
	return loadFrom;
}

bool CvArmyFormationSlot::IsMakingProgressTowardsCheckpoint(int iNextValue)
{
	//no progress in the last two turns?
	//stagnating is ok if we're close already
	if (m_iPrevEstimatedTurnsToCheckpoint >= 2 && iNextValue >= m_iPrevEstimatedTurnsToCheckpoint)
		return false;

	return true;
}
