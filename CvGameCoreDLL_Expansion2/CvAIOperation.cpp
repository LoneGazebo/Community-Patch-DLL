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
#include "CvAIOperation.h"
#include "CvEconomicAI.h"
#include "CvDiplomacyAIEnums.h"
#include "CvMilitaryAI.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "CvDiplomacyAI.h"
#include "CvTypes.h"

#define LINT_WARNINGS_ONLY
#include "LintFree.h"
// PUBLIC FUNCTIONS

/// Constructor
CvAIOperation::CvAIOperation(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, AIOperationTypes eType, ArmyType eMoveType)
{
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	m_eType = eType;
	m_eArmyType = eMoveType;
	Reset();
}

/// Destructor
CvAIOperation::~CvAIOperation()
{
	Reset();
}

/// Delete allocated objects
void CvAIOperation::Reset()
{
	// remove the armies (which should, in turn, free up their units for other tasks)
	for(size_t uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* thisArmy = GetArmy(uiI);
		if(thisArmy)
			thisArmy->Kill();
	}

	// clear out the lists
	m_viArmyIDs.clear();
	m_viListOfUnitsWeStillNeedToBuild.clear();
	m_viListOfUnitsCitiesHaveCommittedToBuild.clear();

	m_eCurrentState = AI_OPERATION_STATE_INVALID;
	m_eAbortReason = NO_ABORT_REASON;

	m_iTargetX = INVALID_PLOT_COORD;
	m_iTargetY = INVALID_PLOT_COORD;
	m_iMusterX = INVALID_PLOT_COORD;
	m_iMusterY = INVALID_PLOT_COORD;

	m_iLastTurnMoved = -1;
	m_iTurnStarted = -1;
	m_iDistanceMusterToTarget = -1;
	m_progressToTarget.clear();
}

bool CvAIOperation::IsOffensive() const
{
	CvPlot* pTargetPlot = GetTargetPlot();
	if (!pTargetPlot)
		return false;

	return !pTargetPlot->IsFriendlyTerritory(m_eOwner);
}

/// How long will we wait for a recruit to show up?
int CvAIOperation::GetMaximumRecruitTurns() const
{
	//extend the range each turn
	int iElapsedTurns = GC.getGame().getGameTurn() - GetTurnStarted();
	return /*5*/ GD_INT_GET(AI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT) + iElapsedTurns/2;
}

const char * CvAIOperation::GetOperationName() const
{
	switch (m_eType)
	{
	case AI_OPERATION_TYPE_UNKNOWN:
		return "OP_TYPE_UNKNOWN";
	case AI_OPERATION_FOUND_CITY:
		return "OP_FOUND_CITY";
	case AI_OPERATION_PILLAGE_ENEMY:
		return "OP_PILLAGE";
	case AI_OPERATION_CITY_ATTACK_LAND:
		if (GET_PLAYER(m_eOwner).IsAtWarWith(m_eEnemy))
			return "OP_CITY_ATTACK_LAND";
		else
			return "OP_CITY_ATTACK_LAND_SNEAKY";
	case AI_OPERATION_CITY_ATTACK_NAVAL:
		if (GET_PLAYER(m_eOwner).IsAtWarWith(m_eEnemy))
			return "OP_CITY_ATTACK_NAVAL";
		else
			return "OP_CITY_ATTACK_NAVAL_SNEAKY";
	case AI_OPERATION_CITY_ATTACK_COMBINED:
		if (GET_PLAYER(m_eOwner).IsAtWarWith(m_eEnemy))
			return "OP_CITY_ATTACK_COMBINED";
		else
			return "OP_CITY_ATTACK_COMBINED_SNEAKY";
	case AI_OPERATION_CITY_DEFENSE:
		return "OP_CITY_DEFENSE";
	case AI_OPERATION_RAPID_RESPONSE:
		return "OP_RAPID_RESPONSE";
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return "OP_NAVAL_SUPERIORITY";
	case AI_OPERATION_NUKE_ATTACK:
		return "OP_NUKE";
	case AI_OPERATION_CARRIER_GROUP:
		return "OP_CSG";
	case AI_OPERATION_MUSICIAN_CONCERT_TOUR:
		return "OP_MUSICIAN";
	case AI_OPERATION_MERCHANT_DELEGATION:
		return "OP_MERCHANT";
	case AI_OPERATION_DIPLOMAT_DELEGATION:
		return "OP_DIPLOMAT";
	}

	return "<INVALID OP TYPE>";
}

CvArmyAI * CvAIOperation::GetArmy(size_t iIndex) const
{
	if (m_eOwner != NO_PLAYER && iIndex < m_viArmyIDs.size())
		return GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[iIndex]);

	return NULL;
}

/// Retrieve plot targeted by this operation
CvPlot* CvAIOperation::GetTargetPlot() const
{
	CvPlot* rtnValue = NULL;
	if (m_iTargetX!=INVALID_PLOT_COORD && m_iTargetY!=INVALID_PLOT_COORD)
		rtnValue =  GC.getMap().plot(m_iTargetX, m_iTargetY);
	return rtnValue;
}

/// Set plot targeted by this operation
void CvAIOperation::SetTargetPlot(CvPlot* pTarget)
{
	if (pTarget==NULL)
	{
		m_iTargetX = INVALID_PLOT_COORD;
		m_iTargetY = INVALID_PLOT_COORD;
	}
	else
	{
		if (pTarget->getX() == m_iTargetX && pTarget->getY() == m_iTargetY)
			return;

		if (m_eCurrentState != AI_OPERATION_STATE_INVALID)
			LogOperationSpecialMessage( CvString::format("setting new target (%d:%d)",pTarget->getX(),pTarget->getY()).c_str() );

		//have to start moving again!
		if (m_eCurrentState == AI_OPERATION_STATE_AT_TARGET)
		{
			m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GetArmy(uiI);
				if(pThisArmy)
					pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
			}
		}

		m_iTargetX = pTarget->getX();
		m_iTargetY = pTarget->getY();
	}

	//update the distance
	m_iDistanceMusterToTarget = GetStepDistanceBetweenPlots(GetMusterPlot(),GetTargetPlot());
	m_progressToTarget.clear();
}

/// Retrieve plot where this operation assembles
CvPlot* CvAIOperation::GetMusterPlot() const
{
	CvPlot* rtnValue = NULL;
	if (m_iMusterX!=INVALID_PLOT_COORD && m_iMusterY!=INVALID_PLOT_COORD)
		rtnValue =  GC.getMap().plot(m_iMusterX, m_iMusterY);
	return rtnValue;
}

/// Set plot where this operation assembles
void CvAIOperation::SetMusterPlot(CvPlot* pMuster)
{
	if (pMuster==NULL)
	{
		m_iMusterX = INVALID_PLOT_COORD;
		m_iMusterX = INVALID_PLOT_COORD;
	}
	else
	{
		m_iMusterX = pMuster->getX();
		m_iMusterY = pMuster->getY();
	}

	//update the distance
	m_iDistanceMusterToTarget = GetStepDistanceBetweenPlots(GetMusterPlot(),GetTargetPlot());
	m_progressToTarget.clear();

	//make sure we don't dump units needlessly
	CvArmyAI* pArmy = GetArmy(0);
	for(size_t iI = 0; pArmy && iI < pArmy->GetNumFormationEntries(); iI++)
		pArmy->GetSlotStatus(iI)->ResetTurnsToCheckpoint();
}

int CvAIOperation::GetGatherTolerance(CvArmyAI* pArmy, CvPlot* pPlot) const
{
	int iValidPlotsNearby = 0;
	if (!pPlot)
		return 1;

	// Find out how many units are trying to gather
	int iNumUnits = pArmy->GetNumSlotsFilled();
	int iRange = (iNumUnits < 5) ? 4 : 3;

	//naval needs more space without ocean travel.
	if (IsNavalOperation() && !GET_PLAYER(pArmy->GetOwner()).CanCrossOcean())
		iRange++;

	for (int i=0; i<RING2_PLOTS; i++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pPlot,i);
		if (!pLoopPlot)
			continue;

		if (!IsNavalOperation() && pLoopPlot->isWater())
			continue;

		if (IsNavalOperation() && !pLoopPlot->isWater())
			continue;

		if (IsNavalOperation() && !pArmy->IsAllOceanGoing() && pLoopPlot->isDeepWater())
			continue;

		if (pLoopPlot->canPlaceCombatUnit(GetOwner()))
			iValidPlotsNearby++;
	}

	// Find more valid plots than units?
	if (iValidPlotsNearby <= iNumUnits)
		iRange++;

	return iRange;
}

/// Factory method: you are responsible for cleaning up the operation created by this routine!
CvAIOperation* CreateAIOperation(AIOperationTypes eAIOperationType, int iID, PlayerTypes eOwner, PlayerTypes eEnemy)
{
	switch(eAIOperationType)
	{
	case AI_OPERATION_TYPE_UNKNOWN:
		break;
	case AI_OPERATION_CITY_ATTACK_LAND:
		return new CvAIOperationCityAttackLand(iID, eOwner, eEnemy);
	case AI_OPERATION_CITY_ATTACK_NAVAL:
		return new CvAIOperationCityAttackNaval(iID, eOwner, eEnemy);
	case AI_OPERATION_CITY_ATTACK_COMBINED:
		return new CvAIOperationCityAttackCombined(iID, eOwner, eEnemy);
	case AI_OPERATION_PILLAGE_ENEMY:
		return new CvAIOperationPillageEnemy(iID, eOwner, eEnemy);
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return new CvAIOperationNavalSuperiority(iID, eOwner, eEnemy);
	case AI_OPERATION_CITY_DEFENSE:
		return new CvAIOperationDefendCity(iID, eOwner, eEnemy);
	case AI_OPERATION_RAPID_RESPONSE:
		return new CvAIOperationDefenseRapidResponse(iID, eOwner, eEnemy);
	case AI_OPERATION_NUKE_ATTACK:
		return new CvAIOperationNukeAttack(iID, eOwner, eEnemy);
	case AI_OPERATION_CARRIER_GROUP:
		return new CvAIOperationCarrierGroup(iID, eOwner, eEnemy);
	//civilian
	case AI_OPERATION_FOUND_CITY:
		return new CvAIOperationCivilianFoundCity(iID, eOwner, eEnemy);
	case AI_OPERATION_MERCHANT_DELEGATION:
		return new CvAIOperationCivilianMerchantDelegation(iID, eOwner, eEnemy);
	case AI_OPERATION_DIPLOMAT_DELEGATION:
		return new CvAIOperationCivilianDiplomatDelegation(iID, eOwner, eEnemy);
	case AI_OPERATION_MUSICIAN_CONCERT_TOUR:
		return new CvAIOperationCivilianConcertTour(iID, eOwner, eEnemy);
	}

	return NULL;
}

/// Find out the next item to build for this operation
OperationSlot CvAIOperation::PeekAtNextUnitToBuild()
{
	if(!m_viListOfUnitsWeStillNeedToBuild.empty())
		return m_viListOfUnitsWeStillNeedToBuild.front();

	return OperationSlot();
}

/// Called by a city when it decides to build a unit
bool CvAIOperation::CommitToBuildNextUnit(OperationSlot slot)
{
	for (std::deque<OperationSlot>::iterator it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
	{
		if (*it == slot)
		{
			m_viListOfUnitsCitiesHaveCommittedToBuild.push_back(slot);
			m_viListOfUnitsWeStillNeedToBuild.erase(it);
			return true;
		}
	}

	return false;
}

/// Called by a city when it decides NOT to build a unit (that it had previously committed to)
bool CvAIOperation::UncommitToBuildUnit(OperationSlot thisOperationSlot)
{
	// find the operation in the list of committed units
	std::vector<OperationSlot>::iterator iter = find(m_viListOfUnitsCitiesHaveCommittedToBuild.begin(),m_viListOfUnitsCitiesHaveCommittedToBuild.end(),thisOperationSlot);
	// if we found it
	if(iter != m_viListOfUnitsCitiesHaveCommittedToBuild.end())
	{
		// add it to the list of stuff that needs to be built
		m_viListOfUnitsWeStillNeedToBuild.push_front(*iter);
		// remove it from the list of committed units
		m_viListOfUnitsCitiesHaveCommittedToBuild.erase(iter);
		return true;
	}
	return false;
}

/// Called by a city after a unit is done training
bool CvAIOperation::FinishedBuildingUnit(OperationSlot thisOperationSlot)
{
	// find the operation in the list of committed units
	std::vector<OperationSlot>::iterator iter = find(m_viListOfUnitsCitiesHaveCommittedToBuild.begin(),m_viListOfUnitsCitiesHaveCommittedToBuild.end(),thisOperationSlot);
	// if we found it
	if(iter != m_viListOfUnitsCitiesHaveCommittedToBuild.end())
	{
		// remove it from the list of committed units
		m_viListOfUnitsCitiesHaveCommittedToBuild.erase(iter);

		// See if our army is now complete
		if(m_viListOfUnitsWeStillNeedToBuild.empty())
			CheckTransitionToNextStage();

		return true;
	}
	return false;
}

/// Assigns available units to our operation. Returns true if all needed units assigned.
bool CvAIOperation::RecruitUnit(CvUnit* pUnit)
{
	if(!pUnit)
		return false;

	CvArmyAI* pThisArmy = GetArmy(0);
	if (!pThisArmy)
		return false;

	CvPlot* pMusterPlot = GetMusterPlot();
	CvPlot* pTargetPlot = GetTargetPlot();
	if (!pMusterPlot || !pTargetPlot)
		return false;

	if(IsNavalOperation())
	{
		CvPlot* pAdjacentPlot = NULL;
		if(pMusterPlot->isCoastalLand())
		{
			pAdjacentPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pMusterPlot);
			if(pAdjacentPlot != NULL)
			{
				pMusterPlot = pAdjacentPlot;
			}
			else
			{
				return false;
			}
		}
		if(pTargetPlot->isCoastalLand())
		{
			pAdjacentPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pTargetPlot);
			if(pAdjacentPlot != NULL)
			{
				pTargetPlot = pAdjacentPlot;
			}
			else
			{
				return false;
			}
		}
	}

	//simple heuristic
	bool bOcean = pTargetPlot->getArea() != pMusterPlot->getArea();

	ReachablePlots turnsFromMuster; //empty is ok
	std::vector<size_t> freeSlots = pThisArmy->GetOpenSlots(false);
	std::vector<std::pair<size_t,CvFormationSlotEntry>> freeSlotInfo;
	for (size_t i = 0; i < freeSlots.size(); i++)
		freeSlotInfo.push_back( std::make_pair(freeSlots[i],pThisArmy->GetSlotInfo(freeSlots[i])) );

	int iIndex = OperationalAIHelpers::IsUnitSuitableForRecruitment(pUnit, turnsFromMuster, pTargetPlot, IsNavalOperation(), bOcean, freeSlotInfo);
	if (iIndex>=0)
	{
		pThisArmy->AddUnit(pUnit->GetID(), freeSlotInfo[iIndex].first, freeSlotInfo[iIndex].second.m_requiredSlot);

		//stupid but true, missing units are maintained on operation level
		for (std::deque<OperationSlot>::iterator it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
		{
			if (it->m_iSlotID == freeSlotInfo[iIndex].first)
			{
				m_viListOfUnitsWeStillNeedToBuild.erase(it);
				break;
			}
		}
		return true;
	}

	return false;
}

//negative return: error; otherwise number of additionally recruited units
int CvAIOperation::GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvArmyAI* pArmy)
{
	if (!pMusterPlot || !pTargetPlot || pArmy == NULL)
		return -1;

	std::vector<size_t> freeSlots = pArmy->GetOpenSlots(false);
	//anything to do?
	if (freeSlots.empty())
		return 0;

	std::vector<std::pair<size_t,CvFormationSlotEntry>> freeSlotInfo;
	for (size_t i = 0; i < freeSlots.size(); i++)
		freeSlotInfo.push_back( std::make_pair(freeSlots[i],pArmy->GetSlotInfo(freeSlots[i])) );

	//simple heuristic
	bool bOcean = pTargetPlot->getArea() != pMusterPlot->getArea();

	//this is just a rough indication so we don't need to do pathfinding for all our units
	SPathFinderUserData data(m_eOwner, PT_ARMY_MIXED, m_eEnemy, GetMaximumRecruitTurns()*4);
	//cast a wide net for recruiting - land units may come from anywhere and mixed naval ops include land units
	if (!GET_PLAYER(m_eOwner).CanCrossOcean())
		data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

	std::vector<OptionWithScore<int>> choices;
	ReachablePlots turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);
	int iLoop = 0;
	for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
	{
		if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit, turnsFromMuster, pTargetPlot, IsNavalOperation(), bOcean, freeSlotInfo) >= 0)
		{
			//now the real pathfinding
			int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF | CvUnit::MOVEFLAG_IGNORE_ZOC;
			int iTurnsToReachCheckpoint = pLoopUnit->TurnsToReachTarget(pMusterPlot, iFlags, GetMaximumRecruitTurns());
			if (iTurnsToReachCheckpoint == INT_MAX)
				continue;

			// When in doubt prefer units in our own territory
			if (pLoopUnit->plot() && pLoopUnit->plot()->getOwner() != m_eOwner)
				iTurnsToReachCheckpoint++;

			// When in doubt prefer units which don't have to embark ...
			if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->plot()->getArea() != pMusterPlot->getArea())
				iTurnsToReachCheckpoint++;

			choices.push_back(OptionWithScore<int>(pLoopUnit->GetID(), 10000 + pLoopUnit->GetPower() - iTurnsToReachCheckpoint * 30));
		}
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strMsg;
		strMsg.Format("Found %d potential units to recruit for operation %d at (%d:%d) with target at (%d:%d)",
			choices.size(), m_iID, pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
		LogOperationSpecialMessage(strMsg);
	}

	std::sort(choices.begin(),choices.end());

	//rebuild the list
	int iCount = 0;
	m_viListOfUnitsWeStillNeedToBuild.clear();
	for (size_t iI = 0; iI < pArmy->GetNumFormationEntries(); iI++)
	{
		CvArmyFormationSlot* pSlot = pArmy->GetSlotStatus(iI);
		if (!pSlot)
			return false;

		if (pSlot->IsFree())
		{
			OperationSlot opSlot(m_iID, pArmy->GetID(), iI);
			if (FindBestFitReserveUnit(opSlot, choices))
				iCount++;
			else
			{
				if (pArmy->GetSlotInfo(iI).m_requiredSlot)
					m_viListOfUnitsWeStillNeedToBuild.push_back(opSlot);
			}
		}
	}

	return iCount;
}

CvPlot* CvAIOperation::GetPlotXInStepPath(CvPlot* pCurrentPosition, CvPlot* pTarget, int iStep, bool bForward) const
{
	if (!pCurrentPosition || !pTarget || iStep<0)
		return NULL;

	//make sure the start position is valid
	//target is handled automatically via approximate mode
	if (IsNavalOperation())
	{
		if (!pCurrentPosition->isWater())
			pCurrentPosition = MilitaryAIHelpers::GetCoastalWaterNearPlot(pCurrentPosition);

		if (!pCurrentPosition)
			return NULL;
	}

	//once the army has gathered both pure and mixed naval ops can only use water plots
	//we assume we can enter the enemy player's territory even in peacetime, don't need to pass any flags
	SPathFinderUserData data(m_eOwner, IsNavalOperation() ? PT_ARMY_WATER : PT_ARMY_LAND, m_eEnemy);
	if (GetArmy(0) && !GetArmy(0)->IsAllOceanGoing())
		data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

	SPath path = GC.GetStepFinder().GetPath(pCurrentPosition, pTarget, data);
	if (!path)
		return NULL;

	int iNodeIndex;
	if (bForward)
		iNodeIndex = std::min( path.length()-1, iStep );
	else
		iNodeIndex = std::max( 0, path.length()-1-iStep );
					
	return path.get(iNodeIndex);
}

int CvAIOperation::GetStepDistanceBetweenPlots(CvPlot* pCurrentPosition, CvPlot* pTarget) const
{
	if (!pCurrentPosition || !pTarget)
		return -1;

	//make sure the start position is valid
	//target is handled automatically via approximate mode
	if (IsNavalOperation())
	{
		if (!pCurrentPosition->isWater())
			pCurrentPosition = MilitaryAIHelpers::GetCoastalWaterNearPlot(pCurrentPosition);

		if (!pCurrentPosition)
			return -1;
	}

	//use the step path finder to compute distance (pass type param 1 to ignore barbarian camps)
	//once the army has gathered both pure and mixed naval ops can only use water plots
	SPathFinderUserData data(m_eOwner, IsNavalOperation() ? PT_ARMY_WATER : PT_ARMY_LAND, m_eEnemy);
	if (GetArmy(0) && !GetArmy(0)->IsAllOceanGoing())
		data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

	return GC.GetStepFinder().GetPathLengthInPlots(pCurrentPosition, pTarget, data);
}

/// Report percentage distance traveled from muster point to target (using army that is furthest along)
int CvAIOperation::PercentFromMusterPointToTarget() const
{
	int iRtnValue = -INT_MAX;

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_INVALID:
		break;
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return 0;

	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		return 100;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		// Let each army perform its own update
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pArmy = GetArmy(uiI);
			if (pArmy && pArmy->GetGoalPlot())
			{
				CvPlot *pCenterOfMass = pArmy->GetCenterOfMass();
				int iDistanceCurrentToTarget = GetStepDistanceBetweenPlots( pCenterOfMass, pArmy->GetGoalPlot() );

				// If within 2 of the final goal, consider ourselves there
				if (iDistanceCurrentToTarget <= 2)
				{
					return 100;
				}
				else
				{
					int iTempValue = 100 - (100 * iDistanceCurrentToTarget / std::max(1,m_iDistanceMusterToTarget));
					if(iTempValue > iRtnValue)
					{
						iRtnValue = iTempValue;
					}
				}
			}
		}
	}
	break;
	}

	return iRtnValue;
}


/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperation::ShouldAbort()
{
	//failsafe in case we're stuck somehow
	int iTurns = GC.getGame().getGameTurn() - GetTurnStarted();
	if (iTurns > 42 && !IsNeverEnding())
	{
		SetToAbort(AI_ABORT_TIMED_OUT);
		return true;
	}

	if (m_eCurrentState != AI_OPERATION_STATE_SUCCESSFUL_FINISH)
	{
		// now see how our armies are doing
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GetArmy(uiI);
			AIOperationAbortReason result = pThisArmy ? VerifyOrAdjustTarget(pThisArmy) : AI_ABORT_NO_UNITS;
			if (result != NO_ABORT_REASON)
			{
				SetToAbort(result);
				break;
			}
		}
	}
	else
	{
		// Mark units in successful operation
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pArmy = GetArmy(uiI);
			CvUnit* pUnit = pArmy ? pArmy->GetFirstUnit() : NULL;
			while(pUnit)
			{
				pUnit->SetDeployFromOperationTurn(GC.getGame().getGameTurn());
				pUnit = pArmy->GetNextUnit(pUnit);
			}
		}
	}

	return (m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH);
}

/// Allows an outside class to terminate the operation
void CvAIOperation::SetToAbort(AIOperationAbortReason eReason)
{
	if (m_eCurrentState != AI_OPERATION_STATE_ABORTED)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = eReason;
	}
}

/// Perform the deletion of this operation
void CvAIOperation::Kill()
{
	//remember this
	int iID = m_iID;
	PlayerTypes eOwner = GetOwner();

	if (m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH)
	{
		m_eAbortReason = AI_ABORT_SUCCESS;
	}
	else if (m_eAbortReason == NO_ABORT_REASON)
	{
		m_eAbortReason = AI_ABORT_KILLED;
	}

	LogOperationEnd();
	Reset();

	//this calls the destructor
	GET_PLAYER(eOwner).deleteAIOperation(iID);
}

bool CvAIOperation::Move()
{
	//single army only for now
	CvArmyAI* pThisArmy = GetArmy(0);
	if (!pThisArmy)
	{
		SetToAbort(AI_ABORT_NO_UNITS);
		return false;
	}

	//recruit more units if needed
	if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
		GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot(), pThisArmy);

	//drop any units which have become stuck
	pThisArmy->UpdateCheckpointTurnsAndRemoveBadUnits();
	//this might cause us to abort
	if (ShouldAbort())
	{
		pThisArmy->ReleaseAllUnits();
		return false;
	}

	float fOldVarX,fOldVarY;
	CvPlot* pOldCOM = pThisArmy->GetCenterOfMass(false,&fOldVarX,&fOldVarY);

	//todo: move the relevant code from tactical AI to operations
	switch (GetArmyType())
	{
	case ARMY_TYPE_ESCORT_LAND:
	case ARMY_TYPE_ESCORT_SEA:
		GET_PLAYER(m_eOwner).GetTacticalAI()->PlotArmyMovesEscort(pThisArmy);
		break;
	case ARMY_TYPE_LAND:
	case ARMY_TYPE_NAVAL:
	case ARMY_TYPE_COMBINED:
		GET_PLAYER(m_eOwner).GetTacticalAI()->PlotArmyMovesCombat(pThisArmy);
		break;
	default:
		return false;
	}

	//exclude rapid response ops etc
	if (pThisArmy->GetNumSlotsFilled()>3)
	{
		float fNewVarX, fNewVarY;
		//center of mass should be moving
		CvPlot* pNewCOM = pThisArmy->GetCenterOfMass(false, &fNewVarX, &fNewVarY);
		if (pNewCOM == pOldCOM && fNewVarX >= std::max(5.f, fOldVarX) && fNewVarY >= std::max(5.f, fOldVarY))
		{
			OutputDebugString(CvString::format("Warning: Operation %d with army at (%d,%d) not making movement progress!\n", m_iID, pNewCOM->getX(), pNewCOM->getY()).c_str());
		}
	}

	SetLastTurnMoved(GC.getGame().getGameTurn());
	return true;
}

/// Update operation for the next turn
bool CvAIOperation::DoTurn()
{
	if (GetLastTurnMoved()==GC.getGame().getGameTurn())
		return true;

	LogOperationStatus(true);

	if ( Move() )
	{
		CheckTransitionToNextStage();
		LogOperationStatus(false);
		return true;
	}
	else
		return false;
}

/// Delete an army associated with this operation (by ID)
bool CvAIOperation::DeleteArmyAI(int iID)
{
	bool ret = false;

	std::vector<int>::iterator iter;
	for(iter = m_viArmyIDs.begin(); iter != m_viArmyIDs.end(); ++iter)
	{
		if(*iter == iID)
		{
			m_viArmyIDs.erase(iter);
			ret = true;
			break;
		}
	}

	if (m_viArmyIDs.empty() && m_eCurrentState!=AI_OPERATION_STATE_ABORTED && m_eCurrentState!=AI_OPERATION_STATE_SUCCESSFUL_FINISH)
		SetToAbort(AI_ABORT_NO_UNITS);

	return ret;
}

/// Handles notification that a unit in this operation was lost. Can be overridden if needed
void CvAIOperation::UnitWasRemoved(int iArmyID, int iSlotID)
{
	// For now, response is based on phase of operation
	switch(m_eCurrentState)
	{
		case AI_OPERATION_STATE_INVALID:
		case AI_OPERATION_STATE_ABORTED:
		case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
			break;

		case AI_OPERATION_STATE_RECRUITING_UNITS:
		{
			// If recruiting units, add this unit to the list of what we need
			m_viListOfUnitsWeStillNeedToBuild.push_back( OperationSlot(m_iID,iArmyID,iSlotID) );
			break;
		}

		case AI_OPERATION_STATE_GATHERING_FORCES:
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
		case AI_OPERATION_STATE_AT_TARGET:
		{
			// If down below half strength, abort
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(iArmyID);
			CvMultiUnitFormationInfo* pkFormation = pThisArmy->GetFormation();
			if(pkFormation)
			{
				//need to look at the original formation ... army slots are set to not required when cleared
				size_t iNumRequiredSlots = 0;
				for (size_t iThisSlotIndex = 0; iThisSlotIndex < pkFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
				{
					const CvFormationSlotEntry& thisSlotEntry = pkFormation->getFormationSlotEntry(iThisSlotIndex);
					if (thisSlotEntry.m_requiredSlot)
						iNumRequiredSlots++;
				}

				if (pThisArmy->GetNumSlotsFilled() < iNumRequiredSlots / 2)
				{
					SetToAbort(AI_ABORT_HALF_STRENGTH);
				}

				//special for escorted ops
				if (pThisArmy->GetNumSlotsFilled() < 2 && iNumRequiredSlots == 2)
				{
					SetToAbort(AI_ABORT_HALF_STRENGTH);
				}
			}
			break;
		}
	};
}

/// Pick this turn's desired "center of mass" for the army
CvPlot* CvAIOperation::ComputeTargetPlotForThisTurn(CvArmyAI* pArmy) const
{
	CvPlot* pRtnValue = NULL;

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_INVALID:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		break;

	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_GATHERING_FORCES:
		// Just use the muster point if we're still recruiting/gathering
		pRtnValue = GetMusterPlot();
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			CvPlot *pGoalPlot = pArmy->GetGoalPlot();
			CvPlot* pCurrent = pArmy->GetCenterOfMass();
			if (pCurrent && pGoalPlot)
			{
				//problem: center of mass may be on a mountain etc ...
				if (!pCurrent->isValidMovePlot(m_eOwner))
				{
					CvUnit* pFirstUnit = pArmy->GetFirstUnit();
					if (pFirstUnit)
						pCurrent = pFirstUnit->plot();
					else
						return NULL;
				}

				//get where we want to be next. always put the carrot a little bit further out
				pRtnValue = GetPlotXInStepPath(pCurrent, pGoalPlot, pArmy->GetMovementRate()+1, true);
				if (!pRtnValue)
				{
					// Can't plot a path, probably due to change of control of hexes.  Will probably abort the operation
					OutputDebugString(CvString::format("CvAIOperation: cannot find a step path from %d,%d to %d,%d\n",
						pCurrent->getX(), pCurrent->getY(), pGoalPlot->getX(), pGoalPlot->getY()).c_str());
					return NULL;
				}
			}
		}
		break;
	}

	return pRtnValue;
}

bool CvAIOperation::BuyFinalUnit()
{
	if(m_viListOfUnitsWeStillNeedToBuild.size() != 1 || m_eCurrentState != AI_OPERATION_STATE_RECRUITING_UNITS)
		return false;

	CvCity* pCity = NULL;
	if (IsNavalOperation())
		pCity = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner,GetMusterPlot());
	else
		pCity = GET_PLAYER(m_eOwner).GetClosestCityByPathLength(GetMusterPlot());

	if(pCity && pCity->getOwner() == m_eOwner)
	{
		OperationSlot thisSlot = m_viListOfUnitsWeStillNeedToBuild.front();
		CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		if (!pArmy)
			return false;
		
		CvFormationSlotEntry thisSlotEntry = pArmy->GetSlotInfo(thisSlot.m_iSlotID);
		CvUnit* pUnit = GET_PLAYER(m_eOwner).GetMilitaryAI()->BuyEmergencyUnit(thisSlotEntry.m_primaryUnitType, pCity);
		if(pUnit != NULL)
		{
			pArmy->AddUnit(pUnit->GetID(), thisSlot.m_iSlotID, true);
			m_viListOfUnitsWeStillNeedToBuild.pop_front();

			if (m_viListOfUnitsWeStillNeedToBuild.empty() && m_viListOfUnitsCitiesHaveCommittedToBuild.empty())
				CheckTransitionToNextStage();

			return true;
		}
	}

	return false;
}

template<typename AIOperation, typename Visitor>
void CvAIOperation::Serialize(AIOperation& aiOperation, Visitor& visitor)
{
	visitor(aiOperation.m_iID);
	visitor(aiOperation.m_eType);
	visitor(aiOperation.m_eArmyType);
	visitor(aiOperation.m_eOwner);
	visitor(aiOperation.m_eEnemy);
	visitor(aiOperation.m_eCurrentState);
	visitor(aiOperation.m_eAbortReason);
	visitor(aiOperation.m_iTargetX);
	visitor(aiOperation.m_iTargetY);
	visitor(aiOperation.m_iMusterX);
	visitor(aiOperation.m_iMusterY);
	visitor(aiOperation.m_iTurnStarted);
	visitor(aiOperation.m_iLastTurnMoved);
	visitor(aiOperation.m_viArmyIDs);
	visitor(aiOperation.m_viListOfUnitsWeStillNeedToBuild);
	visitor(aiOperation.m_viListOfUnitsCitiesHaveCommittedToBuild);
	visitor(aiOperation.m_iDistanceMusterToTarget);
}

/// Read serialized data
void CvAIOperation::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Write serialized data
void CvAIOperation::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvAIOperation& aiOperation)
{
	aiOperation.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvAIOperation& aiOperation)
{
	aiOperation.Write(stream);
	return stream;
}

#if defined(MOD_BALANCE_CORE)
const char* CvAIOperation::GetInfoString()
{
	CvString strTemp0, strTemp1, strTemp2, strTemp3;
	strTemp0 = GetOperationName();
	strTemp1.Format(" (%d) / Target at %d,%d / Muster at %d,%d / ", m_iID, m_iTargetX, m_iTargetY, m_iMusterX, m_iMusterY );

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_INVALID:
		strTemp2 = "Not initialized";
		break;
	case AI_OPERATION_STATE_ABORTED:
		strTemp2.Format("Aborted, %s", AbortReasonString(m_eAbortReason));
		break;
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		strTemp2 = "Recruiting Units";
		break;
	case AI_OPERATION_STATE_GATHERING_FORCES:
		strTemp2 = "Gathering Forces";
		break;
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		strTemp2 = "Moving To Target";
		break;
	case AI_OPERATION_STATE_AT_TARGET:
		strTemp2 = "At Target";
		break;
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		strTemp2 = "Completed";
		break;
	};

	int iUnitsInOperation = 0;
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* thisArmy = GetArmy(uiI);
		if(thisArmy)
			iUnitsInOperation += thisArmy->GetNumSlotsFilled();
	}
	strTemp3.Format(" (R%d-M%d-B%d)", iUnitsInOperation, GetNumUnitsNeededToBeBuilt(), GetNumUnitsCommittedToBeBuilt());

	m_strInfoString = strTemp0+strTemp1+strTemp2+strTemp3;
	return m_strInfoString.c_str();
}
#endif

// PRIVATE FUNCTIONS

/// Log that an operation has started
void CvAIOperation::LogOperationStart() const
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp1;
		CvString strTemp2;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		strPlayerName.Replace(' ', '_'); //no spaces
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID());

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GetArmy(uiI);
			strTemp1.Format("Started Army %d; Units Recruited %d; Max Formation Size %d; ", pThisArmy->GetID(), pThisArmy->GetNumSlotsFilled(), pThisArmy->GetNumFormationEntries());
		}

		strOutBuf = strBaseString + strTemp1;
		switch(m_eCurrentState)
		{
		case AI_OPERATION_STATE_INVALID:
			strTemp2 = "Not initialized";
			break;
		case AI_OPERATION_STATE_ABORTED:
			strTemp2.Format("Aborted, %s", AbortReasonString(m_eAbortReason));
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp2 = "Recruiting Units";
			break;
		case AI_OPERATION_STATE_GATHERING_FORCES:
			strTemp2 = "Gathering Forces";
			break;
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
			strTemp2 = "Moving To Target";
			break;
		case AI_OPERATION_STATE_AT_TARGET:
			strTemp2 = "At Target";
			break;
		case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
			strTemp2 = "Completed";
			break;
		};

		strOutBuf += strTemp2;

		pLog->Msg(strOutBuf);
	}
}

/// Log current status of the operation
void CvAIOperation::LogOperationStatus(bool bPreTurn) const
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2, szTemp3;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		strPlayerName.Replace(' ', '_'); //no spaces
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, %s ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID(), bPreTurn ? "PRE" : "POST");

		switch(m_eCurrentState)
		{
		case AI_OPERATION_STATE_INVALID:
			strTemp = "Not initialized";
			break;
		case AI_OPERATION_STATE_ABORTED:
			strTemp.Format("Aborted: %s", AbortReasonString(m_eAbortReason));
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GetArmy(uiI);
				szTemp2.Format("Recruiting Units for army %d; ", pThisArmy->GetID());
				strTemp += szTemp2;

				for(size_t iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot* pSlot = pThisArmy->GetSlotStatus(iI);
					if(pSlot->IsFree())
					{
						szTemp2 = "Open; ";
					}
					else if(!pSlot->IsUsed())
					{
						szTemp2 = "Skipping; ";
					}
					else
					{
						CvUnit* pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("%s %d at (%d:%d) - ETA %d; ", 
								pThisUnit->getName().GetCString(), pThisUnit->GetID(), pThisUnit->getX(), pThisUnit->getY(), pSlot->GetTurnsToCheckpoint(0));
						}
					}

					strTemp += szTemp2;
				}
			}
			break;
		case AI_OPERATION_STATE_GATHERING_FORCES:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GetArmy(uiI);
				//we don't really need the center of mass but the variance
				float varX=-1,varY=-1;
				CvPlot* pCoM = pThisArmy->GetCenterOfMass(true,&varX,&varY);
				szTemp2.Format("Gathering Forces for army %d; center (%d:%d); variance (%.2f:%.2f); free slots %d", 
					pThisArmy->GetID(), pCoM->getX(), pCoM->getY(), varX, varY, pThisArmy->GetNumFormationEntries() - pThisArmy->GetNumSlotsFilled());
				strTemp += szTemp2;
			}
			break;
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GetArmy(uiI);
				CvPlot* pCoM = pThisArmy->GetCenterOfMass(true);
				szTemp2.Format("Moving To Target with Army %d; center (%d:%d); target (%d:%d); Progress %d percent", 
					pThisArmy->GetID(), pCoM->getX(), pCoM->getY(), m_iTargetX, m_iTargetY, PercentFromMusterPointToTarget());
				strTemp += szTemp2;
			}
			break;
		case AI_OPERATION_STATE_AT_TARGET:
			strTemp = "At Target";
			break;
		case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
			strTemp = "Completed";
			break;
		};

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

void CvAIOperation::LogOperationSpecialMessage(const CvString& strMsg) const
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		strPlayerName.Replace(' ', '_'); //no spaces
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID());
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// Log that an operation has ended
void CvAIOperation::LogOperationEnd() const
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		strPlayerName.Replace(' ', '_'); //no spaces
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID() );
		strTemp.Format("Ended: %s", AbortReasonString(m_eAbortReason));
		strOutBuf = strBaseString + strTemp;
		strTemp.Format("; Distance %d", m_iDistanceMusterToTarget);
		strOutBuf += strTemp;
		strTemp.Format("; Duration %d", GC.getGame().getGameTurn()-m_iTurnStarted);
		strOutBuf += strTemp;

		pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvAIOperation::GetLogFileName(CvString& playerName)
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "OperationalAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "OperationalAILog.csv";
	}

	return strLogName;
}

bool CvAIOperation::SetUpArmy(CvArmyAI* pArmyAI, CvPlot * pMusterPlot, CvPlot * pTargetPlot, CvPlot * pDeployPlot)
{
	//pDeployPlot may be null ...
	if (!pArmyAI || !pMusterPlot || !pTargetPlot)
	{
		if (GC.getLogging() && GC.getAILogging())
			LogOperationSpecialMessage("Cannot set up operation - no target or no muster!");

		SetToAbort( pTargetPlot ? AI_ABORT_NO_MUSTER : AI_ABORT_NO_TARGET );
		return false;
	}

	//this is for the operation
	SetTurnStarted(GC.getGame().getGameTurn());
	SetTargetPlot(pTargetPlot);
	SetMusterPlot(pMusterPlot);

	//this is for the army
	pArmyAI->SetGoalPlot(pDeployPlot?pDeployPlot:pTargetPlot);

	if (pArmyAI->GetOpenSlots(true).empty())
	{
		pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
		m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
		LogOperationSpecialMessage("Skip recruiting, start with movement stage");
	}
	else
	{
		pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
		m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
		LogOperationSpecialMessage("Initial stage is recruiting");
	}

	//recruit units
	if (GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot(), pArmyAI) < 1)
	{
		//bail if there isn't a single suitable unit
		if (pArmyAI->GetNumSlotsFilled() == 0)
		{
			SetToAbort(AI_ABORT_NO_UNITS);
			return false;
		}
	}

	//see if we can switch to gathering stage
	if (CheckTransitionToNextStage())
		//maybe we can even switch to movement stage
		CheckTransitionToNextStage();

	LogOperationStart();
	return true;
}

CvArmyAI* CvAIOperation::AddArmy(MultiunitFormationTypes eFormation)
{
	CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
	if (!pArmyAI)
		return NULL;

	m_viArmyIDs.push_back(pArmyAI->GetID());
	pArmyAI->SetOperationID(m_iID);
	pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
	pArmyAI->SetType(m_eArmyType);
	pArmyAI->SetFormation(eFormation);
	return pArmyAI;
}

/// Find a unit from our reserves that could serve in this operation
bool CvAIOperation::FindBestFitReserveUnit(OperationSlot thisOperationSlot, std::vector<OptionWithScore<int>>& choices)
{
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;
	if(!pThisArmy)
		return false;

	CvArmyFormationSlot* pSlot = pThisArmy->GetSlotStatus(thisOperationSlot.m_iSlotID);
	if (!pSlot)
		return false;

	if (pSlot->IsUsed())
	{
		CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
		if (pUnit)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("Recruiting - slot %d is already full! Unit is: %s %d", thisOperationSlot.m_iSlotID, pUnit->getName().GetCString(), pUnit->GetID());
				LogOperationSpecialMessage(strMsg);
			}
			return true;
		}
	}
	else
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString str1, str2;
			getUnitAIString(str1,thisSlotEntry.m_primaryUnitType);
			getUnitAIString(str2,thisSlotEntry.m_secondaryUnitType);
			strMsg.Format("Recruiting - trying to fill slot %d (%s / %s) in operation %d, army %d", 
				thisOperationSlot.m_iSlotID, str1.c_str(), str2.c_str(), thisOperationSlot.m_iOperationID, thisOperationSlot.m_iArmyID);
			LogOperationSpecialMessage(strMsg);
		}
		*/
	}

	CvFormationSlotEntry thisSlotEntry =  pThisArmy->GetSlotInfo(thisOperationSlot.m_iSlotID);
	for (size_t iI=0; iI<choices.size(); iI++)
	{
		CvUnit* pBestUnit = GET_PLAYER(m_eOwner).getUnit(choices[iI].option);
		if (pBestUnit != NULL)
		{
			CvUnitEntry* unitInfo = GC.getUnitInfo(pBestUnit->getUnitType());
			if (unitInfo->GetUnitAIType(thisSlotEntry.m_primaryUnitType) || unitInfo->GetUnitAIType(thisSlotEntry.m_secondaryUnitType))
			{
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID, thisSlotEntry.m_requiredSlot);

				//remove it so we don't use it a second time
				choices.erase(choices.begin() + iI);

				return true;
			}
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationMilitary
////////////////////////////////////////////////////////////////////////////////

/// How long will we wait for a recruit to show up?
int CvAIOperationMilitary::GetMaximumRecruitTurns() const
{
	return /*10*/ GD_INT_GET(AI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY);
}

AIOperationAbortReason CvAIOperationMilitary::VerifyOrAdjustTarget(CvArmyAI*)
{
	//somebody unexpected owning the target? abort. don't check for war, that would break sneak attacks
	if (GetTargetPlot()->isOwned() && GetTargetPlot()->getOwner()!=m_eEnemy)
		return AI_ABORT_TARGET_ALREADY_CAPTURED;

	return NO_ABORT_REASON;
}

MultiunitFormationTypes OperationalAIHelpers::GetArmyFormationForOpType(AIOperationTypes eType)
{
	switch (eType)
	{
	//civilian operations
	case AI_OPERATION_FOUND_CITY:
		return MUFORMATION_SETTLER_ESCORT;
	case AI_OPERATION_MUSICIAN_CONCERT_TOUR:
		return MUFORMATION_CONCERT_TOUR;
	case AI_OPERATION_DIPLOMAT_DELEGATION:
		return MUFORMATION_DIPLOMAT_ESCORT;
	case AI_OPERATION_MERCHANT_DELEGATION:
		return MUFORMATION_MERCHANT_ESCORT;
	
	//offensive
	case AI_OPERATION_PILLAGE_ENEMY:
		return MUFORMATION_FAST_PILLAGERS;
	case AI_OPERATION_NUKE_ATTACK:
		return MUFORMATION_NUKE_ATTACK;
	case AI_OPERATION_CARRIER_GROUP:
		return MUFORMATION_CARRIER_ESCORT;

	case AI_OPERATION_CITY_ATTACK_LAND:
		return MilitaryAIHelpers::GetCurrentBestFormationTypeForLandAttack();
	case AI_OPERATION_CITY_ATTACK_COMBINED:
		return MilitaryAIHelpers::GetCurrentBestFormationTypeForCombinedAttack();
	case AI_OPERATION_CITY_ATTACK_NAVAL:
		return MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack();

	//defensive
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return MUFORMATION_NAVAL_SQUADRON;
	case AI_OPERATION_RAPID_RESPONSE:
		return MUFORMATION_RAPID_RESPONSE_FORCE;
	case AI_OPERATION_CITY_DEFENSE:
		return MUFORMATION_CLOSE_CITY_DEFENSE;

	default:
		CUSTOMLOG("warning, no formation for unexpected operation type\n");
		return NO_MUFORMATION;
	}
}

/// Kick off this operation
void CvAIOperationMilitary::Init(CvCity* pTargetCity, CvCity* pMusterCity)
{
	CvPlot* pMuster = NULL;
	CvPlot* pTarget = pTargetCity ? pTargetCity->plot() : FindBestTarget(&pMuster);
	if (!pTarget)
		return;

	if (!pMuster)
	{
		if (!pMusterCity)
		{
			if (IsNavalOperation())
				pMusterCity = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner, pTarget);
			else
				pMusterCity = GET_PLAYER(m_eOwner).GetClosestCity(pTarget, 23, true);
		}

		if (pMusterCity)
			pMuster = pMusterCity->plot();
		else
			return;
	}

	CvArmyAI* pArmy = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmy)
		return;

	SetUpArmy(pArmy, pMuster, pTarget);
}

void CvAIOperationMilitary::OnSuccess() const
{
	CvPlot* pPlot = GetTargetPlot();
	if (!pPlot->isCity() && m_eEnemy != NO_PLAYER)
	{
		if (GET_PLAYER(m_eEnemy).GetCityDistanceInPlots(pPlot) < 4)
		{
			CvCity* pCity = GET_PLAYER(m_eEnemy).GetClosestCityByPathLength(pPlot);
			if (pCity)
				pPlot = pCity->plot();
		}
	}

	GET_PLAYER(m_eOwner).GetTacticalAI()->AddFocusArea( pPlot, 3, /*5*/ GD_INT_GET(AI_TACTICAL_MAP_TEMP_ZONE_TURNS));
}

//default version
CvPlot* CvAIOperationMilitary::FindBestTarget(CvPlot ** ppMuster) const
{
	if (ppMuster)
		*ppMuster = NULL;

	return NULL;
}

bool CvAIOperationMilitary::CheckTransitionToNextStage()
{
	//only a single army right now!
	CvArmyAI* pThisArmy = GetArmy(0);
	if(!pThisArmy)
		return false;

	bool bStateChanged = false;
	switch( pThisArmy->GetArmyAIState() )
	{
	case NO_ARMYAISTATE:
	case ARMYAISTATE_AT_DESTINATION:
		break;
	case ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE:
		{
			if(OperationalAIHelpers::HaveEnoughUnits(pThisArmy->GetSlotStatus(),0))
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
				m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				LogOperationSpecialMessage("Transition to gathering stage");
				bStateChanged = true;
			}

			//if we have an initial cluster of units, everyone should go there
			int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
			float fX = 0, fY = 0;
			CvPlot* pCoM = pThisArmy->GetCenterOfMass(true,&fX,&fY);
			if (pCoM && fX < iGatherTolerance && fY < iGatherTolerance && GetMusterPlot() && GetTargetPlot())
			{
				//be a bit careful, don't have units hanging around just anywhere
				//also don't update too frequently, because it interferes with the "progress to checkpoint" logic
				if (plotDistance(*pCoM,*GetTargetPlot())<plotDistance(*GetMusterPlot(),*GetTargetPlot()) &&	
					plotDistance(*pCoM,*GetMusterPlot())>2 &&	
					pCoM->getOwner() == m_eOwner)
					SetMusterPlot(pCoM);
			}


			break;
		}
	case ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP:
		{
			int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
			float fX = 0, fY = 0;
			CvPlot* pCoM = pThisArmy->GetCenterOfMass(true,&fX,&fY);
			if (fX < iGatherTolerance && fY < iGatherTolerance)
			{
				//put the muster plot where our units are and go straight to movement phase
				//this should only happen once ...
				SetMusterPlot(pCoM);
			}

			if(pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= iGatherTolerance)
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
				m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				LogOperationSpecialMessage("Transition to movement stage");
				bStateChanged = true;
			}
			break;
		}
	case ARMYAISTATE_MOVING_TO_DESTINATION:
		{
			//some ops are not intended to ever reach their target
			if (IsNeverEnding())
				return false;

			//check progress but tolerate intermittent stalls
			m_progressToTarget.push_front(PercentFromMusterPointToTarget());
			if (m_progressToTarget.size() > 5)
			{
				m_progressToTarget.pop_back();
				if (m_progressToTarget.front() <= m_progressToTarget.back())
					LogOperationSpecialMessage("No progress to target");
			}

			//check if we're at the target
			CvPlot *pTarget = pThisArmy->GetGoalPlot();
			CvPlot *pCenterOfMass = pThisArmy->GetCenterOfMass();
			if (pCenterOfMass && pTarget)
			{
				bool bInPlace = (plotDistance(*pCenterOfMass, *pTarget) <= GetDeployRange());

				//check for nearby enemy (for sneak attacks)
				if (!bInPlace && GET_PLAYER(m_eOwner).IsAtPeaceWith(m_eEnemy))
				{
					int nVisible = 0;
					for (CvUnit* pUnit = pThisArmy->GetFirstUnit(); pUnit; pUnit = pThisArmy->GetNextUnit(pUnit))
					{
						if (pUnit->plot()->isVisible( GET_PLAYER(m_eEnemy).getTeam() ))
							nVisible++;
					}

					//the jig is up - happens also if we run up against the enemy border
					if (nVisible>2)
					{
						LogOperationSpecialMessage("Discovered by enemy");
						bInPlace = true;
					}
				}

				if(bInPlace)
				{
					// Notify Diplo AI we're in place for attack
					if(GET_PLAYER(m_eOwner).IsAtPeaceWith(m_eEnemy))
						GET_PLAYER(m_eOwner).GetDiplomacyAI()->SetArmyInPlaceForAttack(m_eEnemy, true);

					//that's it. skip STATE_AT_TARGET so the army will be disbanded next turn!
					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
					pThisArmy->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);

					CvString strTemp;
					strTemp.Format("Transition to finished stage; unit distance near %d; far %d", pThisArmy->GetClosestUnitDistance(pTarget), pThisArmy->GetFurthestUnitDistance(pTarget));
					LogOperationSpecialMessage(strTemp);

					OnSuccess();
					bStateChanged = true;
				}
			}
			break;
		}
	}

	return bStateChanged;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationPillageEnemy
////////////////////////////////////////////////////////////////////////////////

bool CvAIOperationPillageEnemy::PreconditionsAreMet(CvPlot* pMusterPlot, CvPlot* pTargetPlot, int iMaxMissingUnits)
{
	//overwrite pTarget / pMuster (should be null anyway)
	pTargetPlot = FindBestTarget(&pMusterPlot);
	if (!pTargetPlot || !pMusterPlot)
		return false;

	if (!CvAIOperation::PreconditionsAreMet(pMusterPlot, pTargetPlot, iMaxMissingUnits))
		return false;

	return true;
}

/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
AIOperationAbortReason CvAIOperationPillageEnemy::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	// Find best pillage target
	CvPlot* pBetterTarget = FindBestTarget(NULL);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		return AI_ABORT_NO_TARGET;
	}

	// If this is a new target, switch to it
	if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	return NO_ABORT_REASON;
}

/// Find the city that we want to pillage
CvPlot* CvAIOperationPillageEnemy::FindBestTarget(CvPlot** ppMuster) const
{
	CvCity* pBestTargetCity = NULL;
	CvPlayerAI& kEnemyPlayer = GET_PLAYER(m_eEnemy);
	CvPlayerAI& kPlayer = GET_PLAYER(m_eOwner);

	int iLoop;
	int iBestValue = 0;
	for(CvCity* pLoopCity = kEnemyPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kEnemyPlayer.nextCity(&iLoop))
	{
		// Make sure city is in the same area as our potential muster point
		CvCity* pClosestCity = kPlayer.GetClosestCityByPathLength(pLoopCity->plot());
		if (!pClosestCity)
			continue;

		//only on the same continent, embarking is dangerous ...
		if (!pLoopCity->HasSharedAreaWith(pClosestCity,true,false))
			continue;

		//Only pillage in border zones
		CvTacticalDominanceZone* pLandZone = kPlayer.GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(pLoopCity, false);
		if (!pLandZone || !pLandZone->HasNeighborZone(m_eOwner))
			continue;

		int iValue = 0;
		int iNumResourceInfos = GC.getNumResourceInfos();
		for (int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes)iResourceLoop;
			ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eResource)->getResourceUsage();
			if (eResourceUsage==RESOURCEUSAGE_LUXURY || eResourceUsage==RESOURCEUSAGE_STRATEGIC)
				iValue += pLoopCity->CountNumWorkedResource(eResource);
		}

		if(iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestTargetCity = pLoopCity;
		}
	}

	if (pBestTargetCity == NULL)
		return NULL;

	if (ppMuster)
	{
		CvCity *pClosest = pBestTargetCity ? kPlayer.GetClosestCityByPathLength(pBestTargetCity->plot()) : NULL;
		*ppMuster = pClosest ? pClosest->plot() : NULL;
	}

	return pBestTargetCity ? pBestTargetCity->plot() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilian
////////////////////////////////////////////////////////////////////////////////

/// Kick off this operation
void CvAIOperationCivilian::Init(CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	CvUnit* pOurCivilian = FindBestCivilian();
	if (!pOurCivilian)
	{
		SetToAbort(AI_ABORT_NO_UNITS);
		return;
	}

	CvPlot* pTargetSite = FindBestTargetForUnit(pOurCivilian);
	if (!pTargetSite)
	{
		SetToAbort(AI_ABORT_NO_TARGET);
		return;
	}

	//don't wait for the escort in the wild
	CvPlot* pMusterPlot = pOurCivilian->plot();
	if (IsEscorted() && pMusterPlot->getOwner() != m_eOwner)
	{
		CvCity* pClosestCity = NULL;
		if (IsNavalOperation())
			pClosestCity = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner, pOurCivilian->plot());
		else if (!pMusterPlot->IsFriendlyTerritory(m_eOwner))
			pClosestCity = GET_PLAYER(m_eOwner).GetClosestCityByPathLength(pOurCivilian->plot());

		if (pClosestCity)
			pMusterPlot = pClosestCity->plot();
	}

	CvArmyAI* pArmy = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmy)
		return;

	//we know the civilian is slot 0
	pArmy->AddUnit(pOurCivilian->GetID(),0,true);

	//a little hack - set the escort slot to not required if we can reach the target this turn
	if (pOurCivilian->TurnsToReachTarget(pTargetSite, CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN, 1) < 1)
		pArmy->GetSlotStatus(1)->Clear();

	SetUpArmy(pArmy,pMusterPlot,pTargetSite,NULL);
}

bool CvAIOperationCivilian::CheckTransitionToNextStage()
{
	//only a single army right now!
	CvArmyAI* pThisArmy = GetArmy(0);
	if(!pThisArmy)
		return false;

	//we look at the army's state. the operation state is really not required
	bool bStateChanged = false;
	CvUnit* pCivilian = pThisArmy->GetFirstUnit();
	switch( pThisArmy->GetArmyAIState() )
	{
	case NO_ARMYAISTATE:
		break;
	case ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE:
		{
			if(OperationalAIHelpers::HaveEnoughUnits(pThisArmy->GetSlotStatus(),0) || (pCivilian && pCivilian->TurnsToReachTarget(GetTargetPlot(),0,1)<1))
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
				m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				LogOperationSpecialMessage("Transition to gathering stage");
				bStateChanged = true;
			}
			break;
		}
	case ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP:
		{
			if (pCivilian && pThisArmy->GetFurthestUnitDistance(pCivilian->plot())<GetGatherTolerance(pThisArmy,pCivilian->plot()))
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
				m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				LogOperationSpecialMessage("Transition to movement stage");
				bStateChanged = true;
			}
			break;
		}
	case ARMYAISTATE_MOVING_TO_DESTINATION:
		{
			if(pCivilian && pCivilian->plot() == GetTargetPlot())
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);
				m_eCurrentState = AI_OPERATION_STATE_AT_TARGET;
				LogOperationSpecialMessage("Transition to target stage");
				bStateChanged = true;

				//maybe we can finish right now?
				if (PerformMission(pCivilian))
				{
					LogOperationSpecialMessage("Transition to finished stage");
					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				}
			}
			break;
		}
	case ARMYAISTATE_AT_DESTINATION:
		{
			if (pCivilian && PerformMission(pCivilian))
			{
				LogOperationSpecialMessage("Transition to finished stage");
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				bStateChanged = true;
			}

			//don't abort in case the mission couldn't be performed, that case is handled in ShouldAbort()

			break;
		}
	}

	return bStateChanged;
}

/// Always abort if settler is removed
void CvAIOperationCivilian::UnitWasRemoved(int iArmyID, int iSlotID)
{
	//the civilian slot is special - without it there's nothing we can do
	if(iSlotID == 0)
		SetToAbort( AI_ABORT_LOST_CIVILIAN );

	//now the default handling
	CvAIOperation::UnitWasRemoved(iArmyID,iSlotID);
}

/// Find the civilian we want to use
CvUnit* CvAIOperationCivilian::FindBestCivilian()
{
	int iUnitLoop;
	for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if (pLoopUnit->AI_getUnitAIType() == GetCivilianType())
		{
			if(pLoopUnit->getArmyID() == -1)
			{
				return pLoopUnit;
			}
		}
	}

	return NULL;
}

/// Start the civilian off to a new target plot
bool CvAIOperationCivilian::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
{
	CvPlot* pCurrentTarget = GetTargetPlot();

	// Find best target
	CvPlot* pBetterTarget = FindBestTargetForUnit(pCivilian);

	// No targets at all!
	if(pBetterTarget == NULL)
	{
		LogOperationSpecialMessage( CvString::format("found no suitable target in area %d",pCurrentTarget?pCurrentTarget->getArea():-1).c_str() );
		SetToAbort( AI_ABORT_NO_TARGET );
		return false;
	}

	SetTargetPlot(pBetterTarget);
	pArmy->SetGoalPlot(pBetterTarget);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianFoundCity
////////////////////////////////////////////////////////////////////////////////

CvUnit* CvAIOperationCivilianFoundCity::FindBestCivilian()
{
	int iUnitLoop;
	for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if (pLoopUnit->getArmyID() != -1)
			continue;
		if (pLoopUnit->canFoundCity(NULL,true,true))
			return pLoopUnit;
	}

	return NULL;
}

bool CvAIOperationCivilianFoundCity::PerformMission(CvUnit* pSettler)
{
	// If the settler made it, we don't care about the entire army
	CvPlot* pCityPlot = GetTargetPlot();
	if(pSettler && pSettler->canFoundCity(pCityPlot) && pSettler->plot() == pCityPlot && pSettler->canMove())
	{
		//check this before building the new city ...
		bool bIsFrontier = (GC.getGame().GetClosestCityDistancePathLength(pCityPlot) < GET_PLAYER(m_eOwner).GetCityDistancePathLength(pCityPlot));

		pSettler->PushMission(CvTypes::getMISSION_FOUND());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvCity* pCity = pCityPlot->getPlotCity();
			if (pCity != NULL)
			{
				CvString strMsg;
				strMsg.Format("City founded (%s) at (%d:%d), plot value %d", pCity->getName().c_str(), 
					pCityPlot->getX(), pCityPlot->getY(), pCityPlot->getFoundValue(m_eOwner));
				LogOperationSpecialMessage(strMsg);
			}
		}

		// Notify tactical AI to focus on this area
		if (bIsFrontier)
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddFocusArea( pCityPlot, 2, /*15*/ GD_INT_GET(AI_TACTICAL_MAP_TEMP_ZONE_TURNS) * 3);

		return true;
	}

	return false;
}

AIOperationAbortReason CvAIOperationCivilianFoundCity::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	CvUnit* pSettler = pArmy->GetFirstUnit();
	if (!pSettler)
		return AI_ABORT_NO_UNITS;

	int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED;
	if (!IsEscorted())
		iFlags |= CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER;

	bool bCanFound = pSettler->canFoundCity(GetTargetPlot());
	bool bHavePath = pSettler->GeneratePath(GetTargetPlot(),iFlags);

	if (!bCanFound)
		LogOperationSpecialMessage( CvString::format("cannot found city at (%d:%d)",m_iTargetX,m_iTargetY).c_str() );

	if (!bHavePath)
		LogOperationSpecialMessage( CvString::format("no path to city site (%d:%d)",m_iTargetX,m_iTargetY).c_str() );

	if (!bCanFound || !bHavePath)
	{
		if (RetargetCivilian(pSettler,pArmy))
			return NO_ABORT_REASON;
		else
			return AI_ABORT_NO_TARGET;
	}
	else
	{
		// let's see if the target still makes sense
		CvPlot* pBetterTarget = GET_PLAYER(m_eOwner).GetBestSettlePlot(pSettler, this);

		// No targets at all!
		if(pBetterTarget == NULL)
		{
			LogOperationSpecialMessage( CvString::format("no valid city site target found (%s)!", IsEscorted() ? "with escort" : "no escort").c_str() );
			return AI_ABORT_NO_TARGET;
		}

		// switch if we have a better target
		if (pBetterTarget != GetTargetPlot())
		{
			SetTargetPlot(pBetterTarget);
			pArmy->SetGoalPlot(pBetterTarget);

			//refresh the path
			pSettler->GeneratePath(pBetterTarget,iFlags);
		}

		// make sure we're not heading for disaster
		CvPlot* pWaypoint = pSettler->GetPathEndFirstTurnPlot();
		if (!pWaypoint && pSettler->plot()!=pBetterTarget)
			return AI_ABORT_LOST_PATH;

		if (pSettler->IsCurrentPathUnsafe())
			return AI_ABORT_TOO_DANGEROUS;

		return NO_ABORT_REASON;
	}
}

//need to have this, it's pure virtual in civilian operation
CvPlot* CvAIOperationCivilianFoundCity::FindBestTargetForUnit(CvUnit* pUnit)
{
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit);
	return pResult;
}

void CvAIOperationCivilianFoundCity::LogSettleTarget(const char* hint, CvPlot* pTarget) const
{
	if (GC.getLogging() && GC.getAILogging())
	{
		FILogFile* pLog = LOGFILEMGR.GetLog("SettleLog.csv", FILogFile::kDontTimeStamp);
		CvString strMsg;
		if (pTarget)
			strMsg.Format("%03d, %s, op %d, %s, target %d:%d, score %d, ", GC.getGame().getElapsedGameTurns(), GET_PLAYER(m_eOwner).getName(),
				m_iID, hint, pTarget->getX(), pTarget->getY(), GET_PLAYER(m_eOwner).getPlotFoundValue(pTarget->getX(), pTarget->getY()));
		else
			strMsg.Format("%03d, %s, op %d, %s, no target, ", GC.getGame().getElapsedGameTurns(), GET_PLAYER(m_eOwner).getName(), m_iID, hint);

		pLog->Msg(strMsg.c_str());
	}
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianMerchantDelegation
////////////////////////////////////////////////////////////////////////////////

/// If at target, cash in; if at muster point, merge merchant and escort and move out
bool CvAIOperationCivilianMerchantDelegation::PerformMission(CvUnit* pMerchant)
{
	//we don't actually have to be exactly at the target plot
	//in fact we cannot go there if it's a city
	if (!pMerchant || plotDistance(*pMerchant->plot(), *GetTargetPlot()) > 1 || !pMerchant->canMove())
		return false;

	// If the merchant made it, we don't care about the entire army
	if(pMerchant->canTrade(pMerchant->plot()))
	{
		if (pMerchant->canBuyCityState(pMerchant->plot()) && !GET_PLAYER(m_eOwner).GreatMerchantWantsCash())
		{
			pMerchant->PushMission(CvTypes::getMISSION_BUY_CITY_STATE());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Great Merchant buying city-state at (%d:%d)", pMerchant->plot()->getX(), pMerchant->plot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
		}
		else
		{
			pMerchant->PushMission(CvTypes::getMISSION_TRADE());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Great Merchant finishing trade mission at (%d:%d)", pMerchant->plot()->getX(), pMerchant->plot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
		}

		return true;
	}
	else
	{
		//uh, this should not happen? look for a neighboring plot
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pMerchant->plot());
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			if (aNeighbors[iI] && pMerchant->canTrade(aNeighbors[iI]))
			{
				pMerchant->PushMission(CvTypes::getMISSION_MOVE_TO(), aNeighbors[iI]->getX(), aNeighbors[iI]->getY());
				return false; //try again next turn
			}
		}
	}

	SetToAbort(AI_ABORT_NO_TARGET);
	return false;}

/// Find the plot where we want to settler
CvPlot* CvAIOperationCivilianMerchantDelegation::FindBestTargetForUnit(CvUnit* pUnit)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).GreatMerchantWantsCash() ?
		GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlotForCash(pUnit) :
		GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlotForPuppet(pUnit);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianDiplomatDelegation
////////////////////////////////////////////////////////////////////////////////

/// Find the plot where we want to influence
CvPlot* CvAIOperationCivilianDiplomatDelegation::FindBestTargetForUnit(CvUnit* pUnit)
{
	if(!pUnit)
		return NULL;

	//this mission is for diplomacy bomb, constructing embassies is handled in homeland AI
	//therefore use the messenger target selection
	return GET_PLAYER(pUnit->getOwner()).ChooseMessengerTargetPlot(pUnit);
}

bool CvAIOperationCivilianDiplomatDelegation::PerformMission(CvUnit* pDiplomat)
{
	//we don't actually have to be exactly at the target plot
	//in fact we cannot go there if it's a city
	if (!pDiplomat || plotDistance(*pDiplomat->plot(), *GetTargetPlot()) > 1 || !pDiplomat->canMove())
		return false;

	if(pDiplomat->canTrade(pDiplomat->plot()))
	{
		//this is not an embassy, this is for influence
		pDiplomat->PushMission(CvTypes::getMISSION_TRADE());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			CvCity* pTargetCity = GET_PLAYER(pDiplomat->plot()->getOwner()).getCapitalCity();
			strMsg.Format("Great Diplomat finishing Diplomatic Mission at %s", pTargetCity ? pTargetCity->getName().c_str() : "UNKNOWN");
			LogOperationSpecialMessage(strMsg);
		}
		return true;
	}
	else
	{
		//uh, this should not happen? look for a neighboring plot
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pDiplomat->plot());
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			if (aNeighbors[iI] && pDiplomat->canTrade(aNeighbors[iI]))
			{
				pDiplomat->PushMission(CvTypes::getMISSION_MOVE_TO(), aNeighbors[iI]->getX(), aNeighbors[iI]->getY());
				return false; //try again next turn
			}
		}
	}

	SetToAbort(AI_ABORT_NO_TARGET);
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianConcertTour
////////////////////////////////////////////////////////////////////////////////

/// Find the plot where we want to settler
CvPlot* CvAIOperationCivilianConcertTour::FindBestTargetForUnit(CvUnit* pUnit)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).FindBestMusicianTargetPlot(pUnit);
}

bool CvAIOperationCivilianConcertTour::PerformMission(CvUnit* pMusician)
{
	//we don't actually have to be exactly at the target plot
	//in fact we cannot go there if it's a city
	if (!pMusician || plotDistance(*pMusician->plot(), *GetTargetPlot()) > 1 || !pMusician->canMove())
		return false;

	if(pMusician->canBlastTourism(pMusician->plot()))
	{
		pMusician->PushMission(CvTypes::getMISSION_ONE_SHOT_TOURISM());
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Great Musician performing concert tour at (%d:%d)", pMusician->plot()->getX(), pMusician->plot()->getY());
			LogOperationSpecialMessage(strMsg);
		}
		return true;
	}
	else
	{
		//uh, this should not happen? look for a neighboring plot
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pMusician->plot());
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			if (aNeighbors[iI] && pMusician->canBlastTourism(aNeighbors[iI]))
			{
				pMusician->PushMission(CvTypes::getMISSION_MOVE_TO(), aNeighbors[iI]->getX(), aNeighbors[iI]->getY());
				return false; //try again next turn
			}
		}
	}

	SetToAbort(AI_ABORT_NO_TARGET);
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalSuperiority
////////////////////////////////////////////////////////////////////////////////

/// Kick off this operation
void CvAIOperationNavalSuperiority::Init(CvCity* pTarget, CvCity* pMuster)
{
	if (!pTarget)
		return;

	if (!pMuster)
		pMuster = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner, pTarget->plot());

	if (!pMuster)
		pMuster = pTarget;

	//this is where we gather the army
	CvPlot* pMusterPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pMuster->plot());
	//this is where the army should go
	CvPlot* pTargetPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pTarget->plot());

	if (!pMusterPlot || !pTargetPlot)
		return;

	CvArmyAI* pArmy = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmy)
		return;

	SetUpArmy(pArmy, pMusterPlot, pTargetPlot, NULL);
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
AIOperationAbortReason CvAIOperationNavalSuperiority::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	CvPlot* pNewTarget = FindBestTarget(NULL);

	if (pNewTarget)
	{
		SetTargetPlot(pNewTarget);
		pArmy->SetGoalPlot(pNewTarget);
		return NO_ABORT_REASON;
	}

	return AI_ABORT_NO_TARGET;
}
/// Find the nearest enemy naval unit to eliminate
CvPlot* CvAIOperationNavalSuperiority::FindBestTarget(CvPlot** ppMuster) const
{
	CvPlot* pCurrent = GetArmy(0) ? GetArmy(0)->GetCurrentPlot() : NULL;
	CvPlot* pTarget = NULL;
	int iClostestDistance = INT_MAX;

	//once the army has gathered both pure and mixed naval ops can only use water plots
	SPathFinderUserData data(m_eOwner, PT_ARMY_WATER, m_eEnemy);
	if (!GET_PLAYER(m_eOwner).CanCrossOcean())
		data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

	std::vector<CvCity*> coastCities = GET_PLAYER(m_eOwner).GetThreatenedCities(true);
	for (size_t i = 0; i < coastCities.size() && i < 3; i++)
	{
		if (!pCurrent)
			return MilitaryAIHelpers::GetCoastalWaterNearPlot(coastCities[i]->plot(), true);

		SPath path = GC.GetStepFinder().GetPath(pCurrent, coastCities[i]->plot(), data);
		if (!!path && path.length() < iClostestDistance)
		{
			iClostestDistance = path.length();
			pTarget = MilitaryAIHelpers::GetCoastalWaterNearPlot(coastCities[i]->plot(), true);
		}
	}

	if (ppMuster)
	{
		CvCity* pMusterCity = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner,pTarget);
		if (pMusterCity && pMusterCity->isCoastal())
			*ppMuster = pMusterCity->plot();
		else
			*ppMuster = pTarget;
	}

	return pTarget;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCarrierGroup
////////////////////////////////////////////////////////////////////////////////

/// Kick off this operation
void CvAIOperationCarrierGroup::Init(CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	//find ourselves an idle carrier
	CvUnit* pCarrier = NULL;
	int iLoop;
	for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
	{
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA && pLoopUnit->getArmyID() == -1 && !pLoopUnit->shouldHeal())
		{
			pCarrier = pLoopUnit;
			break;
		}
	}

	if (!pCarrier)
		return;

	//now where should we go
	CvPlot* pBestTarget = NULL;
	CvPlot* pBestMuster = NULL;
	std::set<int> vTargetZones = GetPossibleDeploymentZones();

	//peacetime
	if (vTargetZones.empty())
	{
		pBestMuster = pCarrier->plot();
		if (pBestMuster->isCity())
			pBestMuster = MilitaryAIHelpers::GetCoastalWaterNearPlot(pBestMuster, true);

		//good enough for now, will update if war breaks out
		pBestTarget = pBestMuster;
	}
	else
	{
		//take the deployment zone that is closest to home
		int iClosestDistance = INT_MAX;
		CvTacticalAnalysisMap* pTactMap = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap();
		for (std::set<int>::const_iterator it = vTargetZones.begin(); it != vTargetZones.end(); ++it)
		{
			CvTacticalDominanceZone* pTargetZone = pTactMap->GetZoneByID(*it);
			CvPlot* pCenterPlot = GC.getMap().plot(pTargetZone->GetCenterX(), pTargetZone->GetCenterY());
			int iDistance = GET_PLAYER(m_eOwner).GetCityDistancePathLength(pCenterPlot);
			if (iDistance < iClosestDistance)
			{
				CvCity* pMusterCity = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner, pCenterPlot);
				if (!pMusterCity)
					continue;

				iClosestDistance = iDistance;
				pBestTarget = pCenterPlot;
				pBestMuster = pMusterCity->plot();
			}
		}
	}

	CvArmyAI* pArmy = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmy)
		return;

	//we know the carrier is slot 0
	pArmy->AddUnit(pCarrier->GetID(),0,true);

	SetUpArmy(pArmy, pBestMuster, pBestTarget, NULL);
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
AIOperationAbortReason CvAIOperationCarrierGroup::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	if (GetOperationState() < AI_OPERATION_STATE_MOVING_TO_TARGET)
		return NO_ABORT_REASON;

	//carrier should retreat
	if (GetArmy(0)->GetFirstUnit()->isProjectedToDieNextTurn())
		return AI_ABORT_HALF_STRENGTH;

	CvPlot* pCurrentPosition = GetArmy(0)->GetCurrentPlot();
	CvPlot* pNewTarget = NULL;

	//this includes the zone we are currently targeting
	std::set<int> vTargetZones = GetPossibleDeploymentZones();

	//take the one that is closest to us
	int iClosestDistance = INT_MAX;
	CvTacticalAnalysisMap* pTactMap = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap();
	for (std::set<int>::const_iterator it = vTargetZones.begin(); it != vTargetZones.end(); ++it)
	{
		CvTacticalDominanceZone* pTargetZone = pTactMap->GetZoneByID(*it);
		CvPlot* pCenterPlot = GC.getMap().plot(pTargetZone->GetCenterX(), pTargetZone->GetCenterY());

		//simplification
		if (pCenterPlot->getArea() != pCurrentPosition->getArea())
			continue;

		int iDistance = plotDistance(*pCenterPlot,*pCurrentPosition);
		if (iDistance < iClosestDistance)
		{
			iClosestDistance = iDistance;
			pNewTarget = pCenterPlot;
		}
	}

	if (pNewTarget==NULL)
	{
		CvCity* pHomeCity = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner, pCurrentPosition);
		if (!pHomeCity)
			return AI_ABORT_NO_TARGET;

		pNewTarget = MilitaryAIHelpers::GetCoastalWaterNearPlot(pHomeCity->plot(), true);
	}

	//no-op if new target is same as old
	SetTargetPlot(pNewTarget);
	pArmy->SetGoalPlot(pNewTarget);
	return NO_ABORT_REASON;
}

std::set<int> CvAIOperationCarrierGroup::GetPossibleDeploymentZones() const
{
	std::set<int> vTargetZones;

	CvTacticalAnalysisMap* pTactMap = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap();
	for (int iI = 0; iI < pTactMap->GetNumZones(); iI++)
	{
		CvTacticalDominanceZone* pZone = pTactMap->GetZoneByIndex(iI);

		if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
		{
			for (std::vector<int>::const_iterator it = pZone->GetNeighboringZones().begin(); it != pZone->GetNeighboringZones().end(); ++it)
			{
				CvTacticalDominanceZone* pNeighborZone = pTactMap->GetZoneByID(*it);
				if (pNeighborZone->IsWater() && pNeighborZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_ENEMY)
				{
					//make sure we can go there
					CvPlot* pCenterPlot = GC.getMap().plot(pNeighborZone->GetCenterX(), pNeighborZone->GetCenterY());
					if (!pCenterPlot->isValidMovePlot(m_eOwner))
						continue;

					//see if it's already targeted by another carrier
					bool bSkip = false;
					for (size_t i=0; i<GET_PLAYER(m_eOwner).getNumAIOperations(); i++)
					{
						CvAIOperation* pOp = GET_PLAYER(m_eOwner).getAIOperationByIndex(i);
						if (pOp->GetOperationType() != AI_OPERATION_CARRIER_GROUP)
							continue;

						//this is the interesting check
						CvTacticalDominanceZone* pTargetedZone = pTactMap->GetZoneByPlot(pOp->GetTargetPlot());
						if (pTargetedZone && pTargetedZone->GetZoneID() == *it)
						{
							bSkip = true;
							break;
						}
					}

					if (!bSkip)
						vTargetZones.insert(*it);
				}
			}
		}
	}
	
	return vTargetZones;
}

/// Handles notification that a unit in this operation was lost. Can be overridden if needed
void CvAIOperationCarrierGroup::UnitWasRemoved(int iArmyID, int iSlotID)
{
	//the carrier slot is special - without it there's nothing we can do
	if(iSlotID == 0)
		SetToAbort( AI_ABORT_CANCELLED );

	//now the default handling
	CvAIOperation::UnitWasRemoved(iArmyID,iSlotID);
}

///////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityAttackNaval
////////////////////////////////////////////////////////////////////////////////


/// Kick off this operation
void CvAIOperationCityAttackNaval::Init(CvCity* pTarget, CvCity* pMuster)
{
	if(pTarget == NULL || !pTarget->isCoastal())
	{
		std::pair<CvCity*,CvCity*> mustertarget = OperationalAIHelpers::GetClosestCoastalCityPair(m_eOwner,m_eEnemy);
		if(mustertarget.first != NULL)
		{
			pMuster = mustertarget.first;
			pTarget = mustertarget.second;
		}
	}

	if(pMuster == NULL || !pMuster->isCoastal())
	{
		pMuster = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner,pTarget->plot());
	}

	CvPlot* pMusterPlot = NULL;
	CvPlot* pTargetPlot = NULL;

	if(pTarget != NULL)
	{
		pTargetPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pTarget->plot());
	}

	if(pMuster != NULL)
	{
		pMusterPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pMuster->plot());
	}

	CvArmyAI* pArmy = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmy)
		return;

	SetUpArmy(pArmy, pMusterPlot, pTargetPlot, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDefendCity - Place holder
////////////////////////////////////////////////////////////////////////////////

AIOperationAbortReason CvAIOperationDefendCity::VerifyOrAdjustTarget(CvArmyAI* /*pArmy*/)
{
	// See if our target city is still owned by us
	if(GetTargetPlot()->getOwner() != m_eOwner)
	{
		// Bummer! The city has been captured by somebody else?
		return AI_ABORT_TARGET_ALREADY_CAPTURED;
	}

	return NO_ABORT_REASON;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDefenseRapidResponse
////////////////////////////////////////////////////////////////////////////////

/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
AIOperationAbortReason CvAIOperationDefenseRapidResponse::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	// Find most threatened city
	CvPlot* pBetterTarget = FindBestTarget(NULL);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		return AI_ABORT_NO_TARGET;
	}

	// If this is a significantly different target, switch to it (else tactical AI should do the rest)
	if( plotDistance(pBetterTarget->getX(),pBetterTarget->getY(),m_iTargetX,m_iTargetY)>5 )
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	return NO_ABORT_REASON;
}

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationDefenseRapidResponse::FindBestTarget(CvPlot** ppMuster) const
{
	CvPlot* pRefPlot = GetTargetPlot();
	CvPlot* pTarget = OperationalAIHelpers::FindEnemiesNearHomelandPlot(m_eOwner,m_eEnemy,DOMAIN_LAND,pRefPlot,5);
	if (ppMuster)
		*ppMuster = pTarget;
	return pTarget;
}

/// Kick off this operation
void CvAIOperationCityAttackCombined::Init(CvCity* pTarget, CvCity* pMuster)
{
	if (!pTarget)
		return;

	if (!pMuster)
		pMuster = OperationalAIHelpers::GetClosestFriendlyCoastalCity(m_eOwner, pTarget->plot());

	if (!pMuster)
		return;

	// Target just off the coast - important that it is owned by eEnemy - we terminate if that should ever change
	CvPlot *pCoastalTarget = MilitaryAIHelpers::GetCoastalWaterNearPlot(pTarget->plot());
	// Muster just off the coast
	CvPlot *pCoastalMuster = MilitaryAIHelpers::GetCoastalWaterNearPlot(pMuster->plot());

	CvArmyAI* pArmy = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmy)
		return;

	SetUpArmy(pArmy, pCoastalMuster, pCoastalTarget, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNukeAttack
////////////////////////////////////////////////////////////////////////////////

void CvAIOperationNukeAttack::Init(CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	CvArmyAI* pArmy = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmy)
		return;

	SetUpArmy(pArmy, pMuster, pTarget);
}

bool CvAIOperationNukeAttack::PreconditionsAreMet(CvPlot* pMusterPlot, CvPlot* pTargetPlot, int iMaxMissingUnits)
{
	//overwrite pTarget / pMuster (should be null anyway)
	pTargetPlot = FindBestTarget(&pMusterPlot);
	if (!pTargetPlot || !pMusterPlot)
		return false;

	if (!CvAIOperation::PreconditionsAreMet(pMusterPlot, pTargetPlot, iMaxMissingUnits))
		return false;

	return true;
}

bool CvAIOperationNukeAttack::CheckTransitionToNextStage()
{
	CvPlot* pTargetPlot = GetTargetPlot();
	CvArmyAI* pArmy = GetArmy(0);

	//don't care about the intermediate niceties ... just nuke away!
	if(pTargetPlot && pArmy)
	{
		if(OperationalAIHelpers::HaveEnoughUnits(pArmy->GetSlotStatus(),0))
		{
			// Now get the nuke
			CvUnit* pNuke = pArmy->GetFirstUnit();
			if(pNuke && pNuke->canMove() && pNuke->canNukeAt(pNuke->plot(),pTargetPlot->getX(),pTargetPlot->getY()))
			{
				//try to save any units we have nearby
				int iBlastRadius = std::min(5,std::max(1,/*2*/ GD_INT_GET(NUKE_BLAST_RADIUS)));
				for (int i=0; i<RING_PLOTS[iBlastRadius]; i++)
				{
					CvPlot* pLoopPlot = iterateRingPlots(pTargetPlot,i);
					if (!pLoopPlot)
						continue;

					const IDInfo* pUnitNode = pLoopPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
						if (pLoopUnit && pLoopUnit->getOwner() == m_eOwner && pLoopUnit->canMove())
						{
							CvPlot* pBestPlot = NULL;
							int iBestDanger = 0;
							ReachablePlots reachablePlots = pLoopUnit->GetAllPlotsInReachThisTurn();
							for (ReachablePlots::iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
							{
								CvPlot* pFleePlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
								if (plotDistance(*pFleePlot, *pTargetPlot) > iBlastRadius)
								{
									if (!pBestPlot || iBestDanger < pLoopUnit->GetDanger(pFleePlot))
									{
										iBestDanger = pLoopUnit->GetDanger(pLoopPlot);
										pBestPlot = pFleePlot;
									}
								}
							}

							if (pBestPlot)
								pLoopUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pBestPlot->getX(), pBestPlot->getY());
						}
					}
				}

				//watch out, here it goes
				pNuke->PushMission(CvTypes::getMISSION_NUKE(), pTargetPlot->getX(), pTargetPlot->getY());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("City nuked at (%d:%d)", pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				return true;
			}
		}
	}

	return false;
}

CvPlot* CvAIOperationNukeAttack::FindBestTarget(CvPlot** ppMuster) const
{
	CvUnit* pBestUnit = NULL;
	CvCity* pBestCity = NULL;
	int iBestCity = 0;
	int iUnitLoop = 0;
	int iCityLoop = 0;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	TeamTypes eTeam = ownerPlayer.getTeam();
	CvTeam& ourTeam = GET_TEAM(eTeam);
	CvPlayerAI& enemyPlayer = GET_PLAYER(m_eEnemy);

	// check all of our units to find the nukes
	for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iUnitLoop))
	{
		if (!pLoopUnit || !pLoopUnit->canNuke())
			continue;

		// for all cities of this enemy
		for(CvCity* pLoopCity = enemyPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = enemyPlayer.nextCity(&iCityLoop))
		{
			//in range?
			if (!pLoopCity || plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pLoopCity->getX(), pLoopCity->getY()) > pLoopUnit->GetRange())
				continue;

			//don't nuke if we're about to capture it or if it was captured from us
			if (pLoopCity->isInDangerOfFalling() || pLoopCity->getOriginalOwner()==m_eOwner)
				continue;

			CvPlot* pCityPlot = pLoopCity->plot();
			int iThisCityValue = 0;

			// check to see if there is anything good or bad in the radius that we should account for
			int iBlastRadius = std::min(5,std::max(1,/*2*/ GD_INT_GET(NUKE_BLAST_RADIUS)));
			for (int i=0; i<RING_PLOTS[iBlastRadius]; i++)
			{
				CvPlot* pLoopPlot = iterateRingPlots(pCityPlot,i);
				if (!pLoopPlot)
					continue;

				// who owns this plot?
				PlayerTypes ePlotOwner = pLoopPlot->getOwner();
				TeamTypes ePlotTeam = pLoopPlot->getTeam();

				//Nukes have hit here already, let's not target this place again.
				if(pLoopPlot->IsFeatureFallout() && pCityPlot->getOwner() == pLoopPlot->getOwner())
				{
					iThisCityValue -= 5000;
				}

				if(ePlotOwner == m_eOwner)
				{
					iThisCityValue -= 1;
					if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
					{
						if(!pLoopPlot->IsImprovementPillaged())
						{
							iThisCityValue -= 50;
							if(pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE)  // we aren't nuking our own resources
							{
								iThisCityValue -= 100;
							}
						}
					}
				}
				else if(ePlotTeam != NO_TEAM && ourTeam.isAtWar(ePlotTeam))
				{
					iThisCityValue += 1;
					if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
					{
						if(!pLoopPlot->IsImprovementPillaged())
						{
							iThisCityValue += 20;
							if(pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE)  // we like nuking our their resources
							{
								iThisCityValue += 100;
							}
						}
					}
				}
				else if (ePlotOwner != NO_PLAYER) // this will trigger a war
				{
					iThisCityValue -= 10000;
				}
	
				// will we hit any units here?
				const IDInfo* pUnitNode = pLoopPlot->headUnitNode();
				const CvUnit* pInnerLoopUnit;
				while(pUnitNode != NULL)
				{
					pInnerLoopUnit = ::GetPlayerUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
					if(pInnerLoopUnit != NULL)
					{
						PlayerTypes eUnitOwner = pInnerLoopUnit->getOwner();
						TeamTypes eUnitTeam = pInnerLoopUnit->getTeam();
						//Let's not nuke our own units.
						if(eUnitOwner == m_eOwner)
						{
							iThisCityValue -= 200;
						}
						// visibility check for enemies
						else if(ourTeam.isAtWar(eUnitTeam) && pLoopPlot->isVisible(eTeam))
						{
							iThisCityValue += 200;
						}
						else if (eUnitOwner != NO_PLAYER && GET_PLAYER(eUnitOwner).isMajorCiv()) // this will trigger a war
						{
							if(GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetCivApproach(eUnitOwner) == CIV_APPROACH_WAR)
							{
								iThisCityValue += 500;
							}
							else if(GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetCivApproach(eUnitOwner) == CIV_APPROACH_HOSTILE)
							{
								iThisCityValue += 200;
							}
							else
							{
								iThisCityValue -= 10000;
							}
						}
					}
				}
			}
			
			if (iThisCityValue <= 0)
				continue;

			iThisCityValue += pLoopCity->getEconomicValue(enemyPlayer.GetID()) + pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage();

			// if this is the capital
			if(pLoopCity->isCapital())
				iThisCityValue *= 2;

			//de-emphasise if we're winning anyway
			CvTacticalDominanceZone* pLandZone = ownerPlayer.GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(pLoopCity, false);
			CvTacticalDominanceZone* pWaterZone = ownerPlayer.GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(pLoopCity, true);
			if (pLandZone && pLandZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				iThisCityValue /= 2;
			if (pWaterZone && pWaterZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				iThisCityValue /= 2;

			if(iThisCityValue > iBestCity)
			{
				pBestUnit = pLoopUnit;
				pBestCity = pLoopCity;
				iBestCity = iThisCityValue;
			}
		}
	}

	if (ppMuster)
		*ppMuster = pBestUnit ? pBestUnit->plot() : NULL;

	return pBestCity ? pBestCity->plot() : NULL;
}

AIOperationAbortReason CvAIOperationNukeAttack::VerifyOrAdjustTarget(CvArmyAI* /*pArmy*/)
{
	// See if our target is still owned by our enemy
	if(GetTargetPlot()->getOwner() != m_eEnemy)
		return AI_ABORT_TARGET_ALREADY_CAPTURED;

	return NO_ABORT_REASON;
}

/// Find a unit from our reserves that could serve in this operation
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, std::vector<OptionWithScore<int>>&)
{
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);

	// check all of our units to find the nuke
	int iUnitLoop;
	for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iUnitLoop))
	{
		if(pLoopUnit && pLoopUnit->canNuke() && pLoopUnit->plot()==GetMusterPlot() && pLoopUnit->getArmyID()==-1)
		{
			CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
			pThisArmy->AddUnit(pLoopUnit->GetID(), thisOperationSlot.m_iSlotID,true);
			return true;
		}
	}

	return false;
}

AIOperationAbortReason CvAIOperationCivilian::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	CvPlot* pTarget = GetTargetPlot();
	if (!pTarget)
		return AI_ABORT_NO_TARGET;

	CvUnit* pCivilian = pArmy->GetFirstUnit();
	if (!pCivilian)
		return AI_ABORT_LOST_CIVILIAN;

	//allow some fog danger
	if (pCivilian->GetDanger(pTarget) > 20 && !IsEscorted())
		return AI_ABORT_TOO_DANGEROUS;
		
	if (GET_PLAYER(m_eOwner).IsAtWarWith(pTarget->getOwner()))
		return AI_ABORT_TOO_DANGEROUS;

	return NO_ABORT_REASON;
}

bool CvAIOperationCivilian::IsEscorted() const
{
	CvArmyAI* pThisArmy = GetArmy(0);
	if (m_eCurrentState==AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_INVALID)
	{
		//be careful, don't check the army directly, it might not exist right now
		if (pThisArmy)
			return pThisArmy->GetNumSlotsFilled() + pThisArmy->GetOpenSlots(true).size() > 1;
		else
		{
			CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
			if (thisFormation && thisFormation->getNumFormationSlotEntriesRequired() > 1)
				return true;
		}

		return false;
	}
	else
	{
		//the unit to be escorted is always the first one
		CvUnit* pCivilian = pThisArmy ? pThisArmy->GetFirstUnit() : NULL;
		if (!pCivilian)
			return false;
		//the second unit would be the first escort
		return (pThisArmy->GetNextUnit(pCivilian) != NULL);
	}
}

/// Read serialized data
void CvAIOperationCivilian::Read(FDataStream& kStream)
{
	CvAIOperation::Read(kStream);
	kStream >> m_eCivilianType;
}

/// Write serialized data
void CvAIOperationCivilian::Write(FDataStream& kStream) const
{
	CvAIOperation::Write(kStream);
	kStream << m_eCivilianType;
}


AIOperationAbortReason CvAIOperationCivilianMerchantDelegation::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	AIOperationAbortReason result = CvAIOperationCivilian::VerifyOrAdjustTarget(pArmy);

	if (result != NO_ABORT_REASON)
		return result;

	CvUnit* pCivilian = pArmy->GetFirstUnit();
	if (!pCivilian)
		return AI_ABORT_LOST_CIVILIAN;

	if (GetTargetPlot()==NULL || !pCivilian->canTrade( GetTargetPlot() ))
		RetargetCivilian(pCivilian, pArmy);

	return (GetTargetPlot() != NULL) ? NO_ABORT_REASON : AI_ABORT_NO_TARGET;
}

AIOperationAbortReason CvAIOperationCivilianDiplomatDelegation::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	AIOperationAbortReason result = CvAIOperationCivilian::VerifyOrAdjustTarget(pArmy);

	if (result != NO_ABORT_REASON)
		return result;

	CvUnit* pCivilian = pArmy->GetFirstUnit();
	if (!pCivilian)
		return AI_ABORT_LOST_CIVILIAN;

	if (GetTargetPlot()==NULL || !pCivilian->canTrade( GetTargetPlot() ))
		RetargetCivilian(pCivilian, pArmy);

	CvCity* pTargetCity = GetTargetPlot()->getOwningCity();
	if (pTargetCity == NULL || GET_PLAYER(pCivilian->getOwner()).ScoreCityForMessenger(pTargetCity, pCivilian) <= 0)
		RetargetCivilian(pCivilian, pArmy);

	return (GetTargetPlot() != NULL) ? NO_ABORT_REASON : AI_ABORT_NO_TARGET;
}

AIOperationAbortReason CvAIOperationCivilianConcertTour::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	AIOperationAbortReason result = CvAIOperationCivilian::VerifyOrAdjustTarget(pArmy);

	if (result != NO_ABORT_REASON)
		return result;

	CvUnit* pCivilian = pArmy->GetFirstUnit();
	if (!pCivilian)
		return AI_ABORT_LOST_CIVILIAN;

	if (GetTargetPlot()==NULL || !pCivilian->canBlastTourism( GetTargetPlot() ))
		RetargetCivilian(pCivilian, pArmy);

	return (GetTargetPlot() != NULL) ? NO_ABORT_REASON : AI_ABORT_NO_TARGET;
}

//----------------------------------------------

FDataStream& operator<<(FDataStream& saveTo, const AIOperationTypes& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, AIOperationTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIOperationTypes>(v);
	return loadFrom;
}

//----------------------------------------------

FDataStream& operator<<(FDataStream& saveTo, const AIOperationAbortReason& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, AIOperationAbortReason& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIOperationAbortReason>(v);
	return loadFrom;
}

//----------------------------------------------

FDataStream& operator<<(FDataStream& saveTo, const ArmyAIState& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, ArmyAIState& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<ArmyAIState>(v);
	return loadFrom;
}

//----------------------------------------------

FDataStream& operator<<(FDataStream& saveTo, const ArmyType& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, ArmyType& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<ArmyType>(v);
	return loadFrom;
}

//----------------------------------------------

FDataStream& operator<<(FDataStream& saveTo, const AIOperationState& readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, AIOperationState& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIOperationState>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const OperationSlot& readFrom)
{
	saveTo << readFrom.m_iOperationID;
	saveTo << readFrom.m_iArmyID;
	saveTo << readFrom.m_iSlotID;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, OperationSlot& writeTo)
{
	loadFrom >> writeTo.m_iOperationID;
	loadFrom >> writeTo.m_iArmyID;
	loadFrom >> writeTo.m_iSlotID;
	return loadFrom;
}

/// Find the barbarian camp we want to eliminate
CvPlot* OperationalAIHelpers::FindClosestBarbarianCamp(PlayerTypes ePlayer, CvPlot** ppMuster)
{
	CvPlot* pBestPlot = NULL;
	CvCity* pClosestCity = NULL;
	int iBestScore = 0;
	ImprovementTypes eBarbCamp = (ImprovementTypes)GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT);

	// Look at map for Barbarian camps
	for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pPlot->getImprovementType() == eBarbCamp || (pPlot->isCity() && pPlot->isBarbarian()))
		{
			// slight AI cheating - enough if adjacent plot is revealed
			if (!pPlot->isRevealed(GET_PLAYER(ePlayer).getTeam()) && !pPlot->isAdjacentRevealed(GET_PLAYER(ePlayer).getTeam()))
				continue;

			// bonus for captured civilians (settlers and workers, not missionaries)
			int iBonus = pPlot->getNumUnitsOfAIType(UNITAI_SETTLE) * 3 + pPlot->getNumUnitsOfAIType(UNITAI_WORKER) * 2;

			int iCityLoop;
			// Loop through each of our cities
			for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
			{
				int iCurPlotDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());

				//prefer camps in our own area
				if (!pLoopCity->HasAccessToArea(pPlot->getArea()))
					iCurPlotDistance *= 2;

				int iScore = 1000 - iCurPlotDistance + iBonus;
				if (iScore > iBestScore)
				{
					//check if we can go there, some camps are remote in mountains or behind foreign territory
					SPathFinderUserData data(ePlayer, PT_ARMY_LAND, BARBARIAN_PLAYER, 23);
					SPath path = GC.GetStepFinder().GetPath(pLoopCity->plot(), pBestPlot, data);
					if (!!path)
					{
						pBestPlot = pPlot;
						iBestScore = iScore;
						pClosestCity = pLoopCity;
					}
				}
			}
		}
	}

	if (ppMuster)
		*ppMuster = pClosestCity ? pClosestCity->plot() : NULL;

	return pBestPlot;
}

bool OperationalAIHelpers::IsSlotRequired(PlayerTypes ePlayer, const OperationSlot& thisOperationSlot)
{
	const CvArmyAI* pThisArmy = GET_PLAYER(ePlayer).getArmyAI(thisOperationSlot.m_iArmyID);
	if (pThisArmy)
		return pThisArmy->GetSlotInfo(thisOperationSlot.m_iSlotID).m_requiredSlot;

	return false;
}

int OperationalAIHelpers::IsUnitSuitableForRecruitment(CvUnit* pLoopUnit, const ReachablePlots& turnsFromMuster, CvPlot* pTarget, bool bMustEmbark, bool bMustBeDeepWaterNaval, const std::vector<std::pair<size_t,CvFormationSlotEntry>>& availableSlots)
{
	//otherwise engaged?
	if (!pLoopUnit->canUseForAIOperation())
		return -1;

	//don't recruit if currently healing
	if (pLoopUnit->shouldHeal(false))
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit: Healing. Unit is: %s", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
		return -1;
	}
	if (pTarget && plotDistance(*pLoopUnit->plot(),*pTarget)<TACTICAL_COMBAT_MAX_TARGET_DISTANCE)
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit %s: Already close to target.", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
		return -1;
	}
	if (pLoopUnit->IsRecentlyDeployedFromOperation())
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit %s: Too Soon.", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
		return -1;
	}
	if (pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * /*70*/ GD_INT_GET(AI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION)) / 100))
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit %s: Not Healthy.", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
		return -1;
	}
	if (bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit %s: Cannot Ocean Embark.", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
		return -1;
	}

	//skip pure explorers
	if (pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_EXPLORE || pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
		return -1;

	if (pLoopUnit->getDomainType() == DOMAIN_LAND)
	{
		if (bMustEmbark && !pLoopUnit->CanEverEmbark())
		{
			/*
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Cannot recruit unit %s: Cannot Embark.", pLoopUnit->getName().GetCString());
				LogOperationSpecialMessage(strMsg);
			}
			*/
			return -1;
		}
	}

	// finally, if the unit is too far away, no deal
	ReachablePlots::const_iterator it = turnsFromMuster.find(pLoopUnit->plot()->GetPlotIndex());
	if (it==turnsFromMuster.end() && !turnsFromMuster.empty())
	{
		/*/
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit: Too far away. Unit is: %s", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
		return -1;
	}

	//finally check formation entries here
	int iMatchingIndex = -1;
	CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
	for (size_t i=0; i<availableSlots.size(); i++)
	{
		CvFormationSlotEntry slot = availableSlots[i].second;
		if (slot.m_primaryUnitType==NO_UNITAI || //wildcard
			pLoopUnit->AI_getUnitAIType() == slot.m_primaryUnitType ||
			pLoopUnit->AI_getUnitAIType() == slot.m_secondaryUnitType ||
			unitInfo->GetUnitAIType(slot.m_primaryUnitType) || 
			unitInfo->GetUnitAIType(slot.m_secondaryUnitType))
		{
			iMatchingIndex = (int)i;
			break;
		}
	}

	return iMatchingIndex;
}

CvPlot* OperationalAIHelpers::FindEnemiesNearHomelandPlot(PlayerTypes ePlayer, PlayerTypes eEnemy, DomainTypes eDomain, CvPlot* pRefPlot, int iMaxDistance)
{
	CvPlot* pBestPlot = NULL;
	int iMaxEnemyPower = 0;

	CvPlayer& enemyPlayer = GET_PLAYER(eEnemy);
	int iUnitLoop;
	for (CvUnit* pLoopUnit=enemyPlayer.firstUnit(&iUnitLoop); pLoopUnit; pLoopUnit=enemyPlayer.nextUnit(&iUnitLoop))
	{
		CvPlot *pLoopPlot = pLoopUnit->plot();

		if(eDomain!=NO_DOMAIN && pLoopUnit->getDomainType()!=eDomain)
			continue;

		if (!pLoopPlot->isVisible(GET_PLAYER(ePlayer).getTeam()))
			continue;

		if (pLoopUnit->isInvisible(GET_PLAYER(ePlayer).getTeam(),false))
			continue;

		if (pLoopPlot->getOwner()!=ePlayer && GET_PLAYER(ePlayer).GetCityDistancePathLength(pLoopPlot)>4) 
			continue;

		//a single unit is too volatile, check for a whole cluster
		std::pair<int,int> localPower = pLoopPlot->GetLocalUnitPower(ePlayer,2,true);

		//we don't want to adjust our target too much
		int iDistance = pRefPlot ? plotDistance(*pRefPlot,*pLoopPlot) : 0;
		int iEnemyPower = localPower.second * MapToPercent(iDistance,std::max(iMaxDistance,3),3);

		if(iEnemyPower > iMaxEnemyPower)
		{
			iMaxEnemyPower = iEnemyPower;
			pBestPlot = pLoopPlot;
		}
	}

	return pBestPlot;	
}

/// Find our port operation operations against this enemy should leave from
CvCity* OperationalAIHelpers::GetClosestFriendlyCoastalCity(PlayerTypes ePlayer, const CvPlot* pRefPlot)
{
	if (ePlayer==NO_PLAYER || !pRefPlot)
		return NULL;

	CvCity* pBestCoastalCity = NULL;
	int iBestDistance = MAX_INT;
	int iLoop;

	//todo: use a simple water path length lookup once we have it
	for(CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal() && pLoopCity->HasAccessToArea(pRefPlot->getArea()))
		{
			int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pRefPlot->getX(), pRefPlot->getY());
			if(iDistance >= 0 && iDistance < iBestDistance)
			{
				iBestDistance = iDistance;
				pBestCoastalCity = pLoopCity;
			}
		}
	}

	return pBestCoastalCity;
}

/// Find our port operation operations against this enemy should leave from
std::pair<CvCity*,CvCity*> OperationalAIHelpers::GetClosestCoastalCityPair(PlayerTypes ePlayerA, PlayerTypes ePlayerB)
{
	std::pair<CvCity*, CvCity*> result(NULL, NULL);
	if (ePlayerA==NO_PLAYER || ePlayerB==NO_PLAYER)
		return result;

	int iBestDistance = MAX_INT;
	int iLoopA, iLoopB;
	for(CvCity* pLoopCityA = GET_PLAYER(ePlayerA).firstCity(&iLoopA); pLoopCityA != NULL; pLoopCityA = GET_PLAYER(ePlayerA).nextCity(&iLoopA))
	{
		if(pLoopCityA->isCoastal())
		{
			for(CvCity* pLoopCityB = GET_PLAYER(ePlayerB).firstCity(&iLoopB); pLoopCityB != NULL; pLoopCityB = GET_PLAYER(ePlayerB).nextCity(&iLoopB))
			{
				// Check all revealed enemy cities
				if(pLoopCityB->isCoastal())
				{
					// On same body of water?
					if(pLoopCityA->HasSharedAreaWith(pLoopCityB,false,true))
					{
						//pathfinding for player A, not entirely symmetric ...
						SPathFinderUserData data(ePlayerA, PT_ARMY_WATER, ePlayerB);
						if (!GET_PLAYER(ePlayerA).CanCrossOcean())
							data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

						int iDistance = GC.GetStepFinder().GetPathLengthInPlots(pLoopCityA->getX(), pLoopCityA->getY(), pLoopCityB->getX(), pLoopCityB->getY(), data);
						if(iDistance>0 && iDistance<iBestDistance)
						{
							iBestDistance = iDistance;
							result = std::make_pair(pLoopCityA,pLoopCityB);
						}
					}
				}
			}
		}
	}

	return result;
}

const char* AbortReasonString(AIOperationAbortReason eReason)
{
	switch(eReason)
	{
	case AI_ABORT_SUCCESS:
		return "Success";
		break;
	case AI_ABORT_NO_TARGET:
		return "NoTarget";
		break;
	case AI_ABORT_CANCELLED:
		return "Cancelled";
		break;
	case AI_ABORT_LOST_TARGET:
		return "LostTarget";
		break;
	case AI_ABORT_TARGET_ALREADY_CAPTURED:
		return "TargetAlreadyCaptured";
		break;
	case AI_ABORT_NO_ROOM_DEPLOY:
		return "NoRoomToDeploy";
		break;
	case AI_ABORT_HALF_STRENGTH:
		return "HalfStrength";
		break;
	case AI_ABORT_NO_MUSTER:
		return "NoMusterPoint";
		break;
	case AI_ABORT_LOST_CIVILIAN:
		return "LostCivilian";
		break;
	case AI_ABORT_TARGET_NOT_VALID:
		return "TargetNotValid";
		break;
	case AI_ABORT_TOO_DANGEROUS:
		return "TooDangerous";
		break;
	case AI_ABORT_KILLED:
		return "Killed";
		break;
	case AI_ABORT_WAR_STATE_CHANGE:
		return "WarStateChange";
		break;
	case AI_ABORT_DIPLO_OPINION_CHANGE:
		return "DiploOpinionChange";
		break;
	case AI_ABORT_LOST_PATH:
		return "LostPath";
		break;
	case AI_ABORT_TIMED_OUT:
		return "TimedOut";
		break;
	case AI_ABORT_NO_UNITS:
		return "NoUnits";
		break;
	default:
		return "UnknownReason";
	}
}

bool CvAIOperation::PreconditionsAreMet(CvPlot* pMusterPlot, CvPlot* pTargetPlot, int iMaxMissingUnits)
{
	//we check existance of suitable target in subclassed implementations

	//use all slots, not only required slots
	std::vector<std::pair<size_t,CvFormationSlotEntry>> freeSlots;
	std::vector<CvArmyFormationSlot> fakeStatus;
	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	for (size_t i = 0; i < thisFormation->getNumFormationSlotEntries(); i++)
	{
		freeSlots.push_back( std::make_pair(i, thisFormation->getFormationSlotEntry(i)) );
		fakeStatus.push_back( CvArmyFormationSlot(-1,thisFormation->getFormationSlotEntry(i).m_requiredSlot) );
	}

	bool bOcean = true;
	ReachablePlots turnsFromMuster;
	if (pMusterPlot && pTargetPlot)
	{
		bOcean = pTargetPlot->getArea() != pMusterPlot->getArea();

		//this is just a rough indication so we don't need to do pathfinding for all our units
		SPathFinderUserData data(m_eOwner, PT_ARMY_MIXED, m_eEnemy, GetMaximumRecruitTurns()*4);
		//cast a wide net for recruiting - land units may come from anywhere and mixed naval ops include land units
		if (!GET_PLAYER(m_eOwner).CanCrossOcean())
			data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;
		turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);
	}

	int iLoop = 0;
	for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
	{
		//if we pass an empty turnsFromMuster, all units will be eligible no matter where they are
		int iIndex = OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit, turnsFromMuster, pTargetPlot, IsNavalOperation(), bOcean, freeSlots);
		if (iIndex >= 0)
		{
			fakeStatus[freeSlots[iIndex].first] = CvArmyFormationSlot( pLoopUnit->GetID(), freeSlots[iIndex].second.m_requiredSlot );
			freeSlots.erase(freeSlots.begin() + iIndex);
			if (freeSlots.empty())
				break;
		}
	}

	//todo: check path for danger? eg embarkation in enemy-dominated water?
	//	or check if there is enough room to deploy at the target? --> no visibility

	return OperationalAIHelpers::HaveEnoughUnits(fakeStatus,iMaxMissingUnits);
}
