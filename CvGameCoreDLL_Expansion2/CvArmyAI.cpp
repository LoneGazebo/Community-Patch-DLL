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

		RemoveUnit( m_FormationEntries[i].GetUnitID(), false );
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
	m_iGoalX = INVALID_PLOT_COORD;
	m_iGoalY = INVALID_PLOT_COORD;
	m_eType = ARMY_TYPE_ANY;
	m_eFormation = NO_MUFORMATION;
	m_eAIState = NO_ARMYAISTATE;
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

	//this calls the destructor
	GET_PLAYER(GetOwner()).deleteArmyAI(m_iID);
}

template<typename ArmyAI, typename Visitor>
void CvArmyAI::Serialize(ArmyAI& armyAI, Visitor& visitor)
{
	visitor(armyAI.m_iID);
	visitor(armyAI.m_eOwner);
	visitor(armyAI.m_iGoalX);
	visitor(armyAI.m_iGoalY);
	visitor(armyAI.m_eType);
	visitor(armyAI.m_eFormation);
	visitor(armyAI.m_eAIState);
	visitor(armyAI.m_iOperationID);
	visitor(armyAI.m_FormationEntries);
}

/// Read from binary data store
void CvArmyAI::Read(FDataStream& kStream)
{
	// Init saved data
	Reset();

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Write to binary data store
void CvArmyAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
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
	return m_eAIState;
}

/// Set current army state
void CvArmyAI::SetArmyAIState(ArmyAIState eNewArmyAIState)
{
	m_eAIState = eNewArmyAIState;
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
		iTotalMovementAllowance += pUnit->baseMoves(pUnit->isEmbarked());
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

	//finally, compute average
	float fNUnits = (float)iNumUnits;
	float fAvgX = (iTotalX/fNUnits) + iRefX;
	float fAvgY = (iTotalY/fNUnits) + iRefY;

	//rounding to nearest integer
	int iAvgX = fAvgX > 0 ? int(fAvgX + 0.5f) : int(fAvgX - 0.5f);
	int iAvgY = fAvgY > 0 ? int(fAvgY + 0.5f) : int(fAvgY - 0.5f);

	//this handles wrapped coordinates
	CvPlot* pCOM = GC.getMap().plot(iAvgX, iAvgY);
	if (!pCOM)
		return NULL;

	if (pfVarX)
	{
		float fVarX = (iTotalX2/fNUnits) - (iTotalX/fNUnits)*(iTotalX/fNUnits);
		*pfVarX = fVarX;
	}
	if (pfVarY)
	{
		float fVarY = (iTotalY2/fNUnits) - (iTotalY/fNUnits)*(iTotalY/fNUnits);
		*pfVarY = fVarY;
	}

	if (bClampToUnit)
	{
		//don't return it directly but use the plot of the closest unit
		pUnit = GetFirstUnit();
		std::vector<SPlotWithScore> vPlots;
		CvPlot* pGoal = GetGoalPlot();
		while (pUnit)
		{
			int iDistToCOM = plotDistance(*pUnit->plot(),*pCOM);
			int iDistToTarget = pGoal ? plotDistance(*pUnit->plot(),*pGoal) : 0;
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
int CvArmyAI::GetClosestUnitDistance(CvPlot* pPlot)
{
	if (!pPlot)
		return INT_MAX;

	int iSmallestDistance = INT_MAX;
	CvUnit* pUnit = GetFirstUnit();

	while(pUnit)
	{
		int iNewDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
		if(iNewDistance < iSmallestDistance)
		{
			iSmallestDistance = iNewDistance;
		}
		pUnit = GetNextUnit(pUnit);
	}

	return iSmallestDistance;
}

/// Return distance from this plot of unit in army farthest away
int CvArmyAI::GetFurthestUnitDistance(CvPlot* pPlot)
{
	if (!pPlot)
		return INT_MAX;

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
CvMultiUnitFormationInfo* CvArmyAI::GetFormation() const
{
	return m_eFormation != NO_MUFORMATION ? GC.getMultiUnitFormationInfo(m_eFormation) : NULL;
}

/// Set index of the formation used by this army
void CvArmyAI::SetFormation(MultiunitFormationTypes eFormation)
{
	if(m_eFormation != eFormation)
	{
		ReleaseAllUnits();

		m_eFormation = eFormation;
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(eFormation);
		for (size_t i=0; i<thisFormation->getNumFormationSlotEntries(); i++)
			m_FormationEntries.push_back( CvArmyFormationSlot(-1,thisFormation->getFormationSlotEntry(i).m_requiredSlot) );
	}
}

/// How many slots are there in this formation if filled
size_t CvArmyAI::GetNumFormationEntries() const
{
	return m_FormationEntries.size();
}

/// How many slots do we currently have filled?
size_t CvArmyAI::GetNumSlotsFilled() const
{
	size_t iRtnValue = 0;

	for(size_t iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		if(m_FormationEntries[iI].IsUsed())
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}

/// Recalculate when each unit will arrive at the current army position, whatever that is
void CvArmyAI::UpdateCheckpointTurnsAndRemoveBadUnits()
{
	CvAIOperation* pOperation = GET_PLAYER(GetOwner()).getAIOperation(GetOperationID());
	if (!pOperation)
		return;

	CvPlot* pCurrentArmyPlot = GetCurrentPlot();

	//kick out damaged units, but if we're defending or escorting, no point in running away
	if (pOperation->IsOffensive())
	{
		for (unsigned int iI = 0; iI < m_FormationEntries.size(); iI++)
		{
			if (!m_FormationEntries[iI].IsUsed())
				continue;

			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(m_FormationEntries[iI].GetUnitID());
			if (pUnit == NULL)
				continue;

			//let tactical AI handle units which are badly hurt
			if (pUnit->shouldHeal())
				RemoveUnit(m_FormationEntries[iI].GetUnitID(),false);
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
				m_FormationEntries[iI].SetCurrentTurnsToCheckpoint(0);
			}
			else
			{
				//be generous with the flags here, for some ops the muster point may be far away and intermittendly occupied by foreign units ...
				int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ZOC;
				int iTurnsToReachCheckpoint = pUnit->TurnsToReachTarget(pCurrentArmyPlot, iFlags, pOperation->GetMaximumRecruitTurns());
				m_FormationEntries[iI].SetCurrentTurnsToCheckpoint(iTurnsToReachCheckpoint);

				//if we're already moving to target, the current army plot is moving, so we cannot check progress against ...
				if (!m_FormationEntries[iI].IsMakingProgressTowardsCheckpoint())
				{
					CvString strMsg;
					strMsg.Format("Removing %s %d from army %d because no progress to checkpoint (%d:%d). ETA %d; prev %d; prev %d", 
						pUnit->getName().c_str(), m_FormationEntries[iI].GetUnitID(), GetID(), pCurrentArmyPlot->getX(), pCurrentArmyPlot->getY(),
						m_FormationEntries[iI].GetTurnsToCheckpoint(0), m_FormationEntries[iI].GetTurnsToCheckpoint(1), m_FormationEntries[iI].GetTurnsToCheckpoint(2));
					pOperation->LogOperationSpecialMessage(strMsg);
					RemoveUnit(m_FormationEntries[iI].GetUnitID(),false);
				}
			}
		}
	}
}

/// Can all units in this army move on ocean?
bool CvArmyAI::IsAllOceanGoing()
{
	CvUnit* pUnit = GetFirstUnit();
	while(pUnit)
	{
		if(pUnit->getDomainType() != DOMAIN_SEA)
		{
			if (!pUnit->IsHasEmbarkAbility() || !GET_PLAYER(pUnit->getOwner()).CanCrossOcean())
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

CvFormationSlotEntry CvArmyAI::GetSlotInfo(size_t iSlotID) const
{
	CvMultiUnitFormationInfo* thisFormation = GetFormation();
	if (thisFormation && iSlotID >= 0 && iSlotID < thisFormation->getNumFormationSlotEntries())
		return thisFormation->getFormationSlotEntry(iSlotID);

	return CvFormationSlotEntry();
}

vector<size_t> CvArmyAI::GetOpenSlots(bool bRequiredOnly) const
{
	vector<size_t> result;
	for (size_t i = 0; i<m_FormationEntries.size(); i++)
		if (m_FormationEntries[i].IsFree())
			if (!bRequiredOnly || m_FormationEntries[i].IsRequired())
				result.push_back(i);

	return result;
}

CvPlot* CvArmyAI::GetCurrentPlot()
{
	switch (m_eAIState)
	{
	case NO_ARMYAISTATE:
		break;
	case ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE:
	case ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP:
		//stupid but true
		return GET_PLAYER(m_eOwner).getAIOperation(m_iOperationID)->GetMusterPlot();
	case ARMYAISTATE_MOVING_TO_DESTINATION:
		return GetCenterOfMass(true);
	case ARMYAISTATE_AT_DESTINATION:
		return GetGoalPlot();
	}

	return NULL;
}

/// Land or sea army?
DomainTypes CvArmyAI::GetDomainType() const
{
	switch (m_eType)
	{
	case ARMY_TYPE_LAND:
		return DOMAIN_LAND;
	case ARMY_TYPE_NAVAL:
	case ARMY_TYPE_COMBINED:
		return DOMAIN_SEA;
	default:
		return NO_DOMAIN;
	}
}

void CvArmyAI::SetType(ArmyType eType)
{
	m_eType = eType;
}

ArmyType CvArmyAI::GetType() const
{
	return m_eType;
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

// UNIT HANDLING
/// Add a unit to our army (and we know which slot)
void CvArmyAI::AddUnit(int iUnitID, int iSlotNum, bool bIsRequired)
{
	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	CvUnit* pThisUnit = thisPlayer.getUnit(iUnitID);
	if (!pThisUnit || iSlotNum<0 || iSlotNum>=(int)m_FormationEntries.size())
		return;

	// uh, slot already used?
	if (GetSlotStatus(iSlotNum)->IsUsed())
	{
		CUSTOMLOG("warning, trying to assign unit to non-free slot\n");
		return;
	}

	// remove this unit from an army if it is already in one
	thisPlayer.removeFromArmy(pThisUnit->getArmyID(), GetID());

	// add it to this army
	m_FormationEntries[iSlotNum] = CvArmyFormationSlot(iUnitID, bIsRequired); //reset
	pThisUnit->setArmyID(GetID());

	//do this in two steps to avoid triggering the sanity check
	pThisUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
	pThisUnit->setTacticalMove(AI_TACTICAL_OPERATION);

	// Finally, compute when we think this unit will arrive at the next checkpoint
	CvPlot* pMusterPlot = GetCurrentPlot();
	if(pMusterPlot)
	{
		int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ZOC;
		int iTurnsToReachCheckpoint = pThisUnit->TurnsToReachTarget(pMusterPlot, iFlags, /*10*/ GD_INT_GET(AI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY));
		m_FormationEntries[iSlotNum].SetCurrentTurnsToCheckpoint(iTurnsToReachCheckpoint);

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
int CvArmyAI::RemoveUnit(int iUnitToRemoveID, bool bTempOnly)
{
	if (iUnitToRemoveID == -1)
		return -1;

	for(size_t iI = 0; iI < m_FormationEntries.size(); iI++)
	{
		CvArmyFormationSlot& slot = m_FormationEntries[iI];
		if(slot.GetUnitID() == iUnitToRemoveID)
		{
			slot.Clear();

			CvUnit* pThisUnit = GET_PLAYER(GetOwner()).getUnit(iUnitToRemoveID);
			if(pThisUnit)
			{
				// Clears unit's army ID and erase from formation entries
				pThisUnit->setArmyID(-1);
				pThisUnit->AI_setUnitAIType(pThisUnit->getUnitInfo().GetDefaultUnitAIType());

				// Tell the associated operation that a unit was lost
				// This might lead to an abort, so sometimes we want to suppress it
				if (!bTempOnly)
				{
					CvAIOperation* pThisOperation = GET_PLAYER(GetOwner()).getAIOperation(m_iOperationID);
					if (pThisOperation)
						pThisOperation->UnitWasRemoved(GetID(), iI);
				}

				// Make the unit available for tactical moves
				pThisUnit->setTacticalMove(AI_TACTICAL_MOVE_NONE);
				if (!pThisUnit->isDelayedDeath())
					GET_PLAYER(GetOwner()).GetTacticalAI()->AddCurrentTurnUnit(pThisUnit);

				return iI;
			}
		}
	}

	return -1;
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
			m_FormationEntries[i].Clear();
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
	saveTo << readFrom.m_estTurnsToCheckpoint;
	saveTo << readFrom.m_bIsRequired;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvArmyFormationSlot& writeTo)
{
	loadFrom >> writeTo.m_iUnitID;
	loadFrom >> writeTo.m_estTurnsToCheckpoint;
	loadFrom >> writeTo.m_bIsRequired;
	return loadFrom;
}

//----------------------------------------------

FDataStream& operator<<(FDataStream& saveTo, const MultiunitFormationTypes& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, MultiunitFormationTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<MultiunitFormationTypes>(v);
	return loadFrom;
}

void CvArmyFormationSlot::ResetTurnsToCheckpoint()
{
	//useful if eg the muster point was updated
	m_estTurnsToCheckpoint.clear();
}

bool CvArmyFormationSlot::IsMakingProgressTowardsCheckpoint() const
{
	//sometimes we get into a fight or there is a temporary block so we need some history
	if (m_estTurnsToCheckpoint.size() < 3)
		return true;

	// can't get much better than this
	if (m_estTurnsToCheckpoint.front() < 2)
		return true;

	// regular case, see if we made progress over several turns
	if (m_estTurnsToCheckpoint.front() >= m_estTurnsToCheckpoint.back())
		return false;

	return true;
}

bool OperationalAIHelpers::HaveEnoughUnits(const vector<CvArmyFormationSlot>& slotStatus, int iMaxMissingUnits)
{
	int iFreeRequired = 0;
	int iPresentRequired = 0;
	int iPresentOptional = 0;

	for (vector<CvArmyFormationSlot>::const_iterator it = slotStatus.begin(); it != slotStatus.end(); ++it)
	{
		if (it->IsFree() && it->IsRequired())
			iFreeRequired++;
		else if (it->IsUsed() && it->IsRequired())
			iPresentRequired++;
		else if (it->IsUsed() && !it->IsRequired())
			iPresentOptional++;
	}

	// make up for missing required units with additional optional units
	return iPresentRequired > 0 && iFreeRequired <= iPresentOptional/2 + iMaxMissingUnits;
}
