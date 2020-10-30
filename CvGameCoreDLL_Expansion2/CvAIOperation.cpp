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
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT() + iElapsedTurns/2;
}

const char * CvAIOperation::GetOperationName() const
{
	switch (m_eType)
	{
	case AI_OPERATION_FOUND_CITY:
		return "OP_FOUND_CITY";
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return "OP_KILL_BARBARIANS";
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
	case AI_OPERATION_MUSICIAN_CONCERT_TOUR:
		return "OP_MUSICIAN";
	case AI_OPERATION_MERCHANT_DELEGATION:
		return "OP_MERCHANT";
	case AI_OPERATION_DIPLOMAT_DELEGATION:
		return "OP_DIPLOMAT";
	}

	return "UNKNOWN_OP_TYPE";
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
		if ( pTarget!=GetTargetPlot() )
		{
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
	case AI_OPERATION_CITY_ATTACK_LAND:
		return new CvAIOperationCityAttackLand(iID, eOwner, eEnemy);
	case AI_OPERATION_CITY_ATTACK_NAVAL:
		return new CvAIOperationCityAttackNaval(iID, eOwner, eEnemy);
	case AI_OPERATION_CITY_ATTACK_COMBINED:
		return new CvAIOperationCityAttackCombined(iID, eOwner, eEnemy);
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return new CvAIOperationAntiBarbarian(iID, eOwner, eEnemy);
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

	return 0;
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
			pAdjacentPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMusterPlot);
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
			pAdjacentPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTargetPlot);
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

	SPathFinderUserData data(m_eOwner,PT_GENERIC_REACHABLE_PLOTS,-1,GetMaximumRecruitTurns());
	ReachablePlots turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);
	vector<int> freeSlots = pThisArmy->GetOpenSlots(false);
	vector<pair<int,CvFormationSlotEntry>> freeSlotInfo;
	for (size_t i = 0; i < freeSlots.size(); i++)
		freeSlotInfo.push_back( make_pair(freeSlots[i],pThisArmy->GetSlotInfo(freeSlots[i])) );

	int iIndex = OperationalAIHelpers::IsUnitSuitableForRecruitment(pUnit, turnsFromMuster, IsNavalOperation(), pThisArmy->NeedOceanMoves(), freeSlotInfo);
	if (iIndex>=0)
	{
		pThisArmy->AddUnit(pUnit->GetID(), freeSlotInfo[iIndex].first, freeSlotInfo[iIndex].second.m_requiredSlot);
		//stupid but true
		for (std::deque<OperationSlot>::iterator it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
		{
			if (it->m_iSlotID == iIndex)
			{
				m_viListOfUnitsWeStillNeedToBuild.erase(it);
				break;
			}
		}
		return true;
	}

	return false;
}

int CvAIOperation::GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvArmyAI* pArmy)
{
	if (!pMusterPlot || !pTargetPlot || pArmy == NULL)
		return false;

	vector<int> freeSlots = pArmy->GetOpenSlots(false);
	//anything to do?
	if (freeSlots.empty())
		return true;

	vector<pair<int,CvFormationSlotEntry>> freeSlotInfo;
	for (size_t i = 0; i < freeSlots.size(); i++)
		freeSlotInfo.push_back( make_pair(freeSlots[i],pArmy->GetSlotInfo(freeSlots[i])) );

	vector<OptionWithScore<int>> choices;
	bool bMustBeDeepWaterNaval = pArmy->NeedOceanMoves();
	bool bMustEmbark = pMusterPlot->getArea() != pTargetPlot->getArea();

	//this is just a rough indication so we don't need to do pathfinding for all our units
	SPathFinderUserData data(m_eOwner, IsNavalOperation() ? PT_TRADE_WATER : PT_TRADE_LAND, 1, GetMaximumRecruitTurns()*4);
	ReachablePlots turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);

	int iLoop = 0;
	for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
	{
		if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit, turnsFromMuster, bMustEmbark, bMustBeDeepWaterNaval, freeSlotInfo) >= 0)
		{
			int iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING2 | CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ZOC;
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

	if (choices.empty() && pArmy->GetNumSlotsFilled() <= 0)
	{ 
		SetToAbort(AI_ABORT_NO_UNITS);
		return false;
	}

	std::sort(choices.begin(),choices.end());

	//rebuild the list
	m_viListOfUnitsWeStillNeedToBuild.clear();
	CvMultiUnitFormationInfo* thisFormation = pArmy->GetFormation();
	for (int iI = 0; iI < thisFormation->getNumFormationSlotEntries(); iI++)
	{
		CvArmyFormationSlot* pSlot = pArmy->GetSlotStatus(iI);
		if (!pSlot)
			return false;

		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iI);
		if (pSlot->IsFree())
		{
			OperationSlot opSlot(m_iID, pArmy->GetID(), iI);
			if (!FindBestFitReserveUnit(opSlot, choices))
			{
				if (thisSlotEntry.m_requiredSlot)
					m_viListOfUnitsWeStillNeedToBuild.push_back(opSlot);
			}
		}
	}

	return (int)m_viListOfUnitsWeStillNeedToBuild.size();
}

CvPlot* CvAIOperation::GetPlotXInStepPath(CvPlot* pCurrentPosition, CvPlot* pTarget, int iStep, bool bForward) const
{
	if (!pCurrentPosition || !pTarget || iStep<0)
		return NULL;

	//check what kind of path we need
	PathType ePathType = PT_TRADE_LAND;

	//make sure the start position is valid
	//target is handled automatically via approximate mode
	if (IsNavalOperation())
	{
		ePathType = PT_TRADE_WATER;

		if (!pCurrentPosition->isWater())
			pCurrentPosition = MilitaryAIHelpers::GetCoastalPlotNearPlot(pCurrentPosition);

		if (!pCurrentPosition)
			return NULL;
	}

	// use the step path finder to compute distance (pass type param 1 to ignore barbarian camps)
	SPathFinderUserData data(m_eOwner, ePathType, 1);
	if (GetArmy(0) && !GetArmy(0)->IsAllOceanGoing())
		data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;
	SPath path = GC.GetStepFinder().GetPath(pCurrentPosition, pTarget, data);
	if (!path)
		return NULL;

	// check if there are obstacles on the way (maybe someone founded a new city?)
	// do not check the final steps, because that's our actual target city!
	for (int i = 0; i < path.length()-3; i++)
	{
		if (GC.getGame().GetClosestCityDistanceInPlots(path.get(i)) < 3)
		{
			CvCity* pCity = GC.getGame().GetClosestCityByPlots(path.get(i));
			if (pCity && GET_PLAYER(m_eOwner).IsAtWarWith(pCity->getOwner()))
				return NULL; //this will abort the operation
		}
	}

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

	//check what kind of path we need
	PathType ePathType = PT_TRADE_LAND;

	//make sure the start position is valid
	//target is handled automatically via approximate mode
	if (IsNavalOperation())
	{
		ePathType = PT_TRADE_WATER;

		if (!pCurrentPosition->isWater())
			pCurrentPosition = MilitaryAIHelpers::GetCoastalPlotNearPlot(pCurrentPosition);

		if (!pCurrentPosition)
			return -1;
	}

	// use the step path finder to compute distance (pass type param 1 to ignore barbarian camps)
	SPathFinderUserData data(m_eOwner, ePathType, 1);
	if (GetArmy(0) && !GetArmy(0)->IsAllOceanGoing())
		data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

	return GC.GetStepFinder().GetPathLengthInPlots(pCurrentPosition, pTarget, data);
}

/// Report percentage distance traveled from muster point to target (using army that is furthest along)
int CvAIOperation::PercentFromMusterPointToTarget() const
{
	int iRtnValue = 0;

	switch(m_eCurrentState)
	{
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

			//this is strange, check did not work, goal plot was not null but invalid
			if (pArmy==NULL || pArmy->GetGoalX()==-1 || pArmy->GetGoalY()==-1)
				return 0;

			if (pArmy->GetGoalPlot())
			{
				CvPlot *pCenterOfMass = pArmy->GetCenterOfMass();
				int iDistanceCurrentToTarget = GetStepDistanceBetweenPlots( pCenterOfMass, pArmy->GetGoalPlot() );

				if(m_iDistanceMusterToTarget < 0 || iDistanceCurrentToTarget < 0)
				{
					//we should abort the operation?
					return 0;
				}
				// If within 2 of the final goal, consider ourselves there
				else if (iDistanceCurrentToTarget <= 2)
				{
					return 100;
				}
				else
				{
					int iTempValue = 100 - (100 * iDistanceCurrentToTarget / MAX(1,m_iDistanceMusterToTarget));
					if(iTempValue > iRtnValue)
					{
						iRtnValue = iTempValue;
					}
				}
			}
			else 
			{
				CvAssertMsg (false, "Operational AI army without a goal plot!  Send save to Ed");
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
	if (iTurns > 50)
	{
		SetToAbort(AI_ABORT_TIMED_OUT);
		return true;
	}

	if (GetEnemy() != NO_PLAYER)
	{
		CvTeam& enemyTeam = GET_TEAM(GET_PLAYER(GetEnemy()).getTeam());
		if (enemyTeam.IsVassalOfSomeone() && !enemyTeam.isAtWar(GET_PLAYER(m_eOwner).getTeam()))
		{
			SetToAbort(AI_ABORT_TARGET_NOT_VALID);
			return true;
		}
	}

	CvPlot* pMuster = GetMusterPlot();
	if (pMuster)
	{
		//for offensive ops, we even need the muster plot to be in a friendly zone
		CvTacticalDominanceZone* pZone = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pMuster);
		if (IsOffensive() && pZone && pZone->GetZoneCity() && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
			SetToAbort(AI_ABORT_TOO_DANGEROUS);
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

	if (ShouldAbort())
	{
		LogOperationSpecialMessage( "operation aborted before move" );
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
	if (pThisArmy->GetNumSlotsFilled()>3 && pThisArmy->GetArmyAIState()==ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP)
	{
		float fNewVarX,fNewVarY;
		//only care about the variance
		pThisArmy->GetCenterOfMass(false,&fNewVarX,&fNewVarY);
		if (fNewVarX>=max(5.f,fOldVarX) && fNewVarY>=max(5.f,fOldVarY))
		{
			OutputDebugString( CvString::format("Warning: Operation %d with army at (%d,%d) not making gathering progress!\n",m_iID,pThisArmy->GetX(),pThisArmy->GetY()).c_str() );
		}
	}

	if (pThisArmy->GetNumSlotsFilled()>3 && pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		float fNewVarX, fNewVarY;
		//center of mass should be moving
		CvPlot* pNewCOM = pThisArmy->GetCenterOfMass(false, &fNewVarX, &fNewVarY);
		if (pNewCOM == pOldCOM && fNewVarX >= max(5.f, fOldVarX) && fNewVarY >= max(5.f, fOldVarY))
		{
			OutputDebugString(CvString::format("Warning: Operation %d with army at (%d,%d) not making movement progress!\n", m_iID, pThisArmy->GetX(), pThisArmy->GetY()).c_str());
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
				int iNumRequiredSlots = 0;
				for (int iThisSlotIndex = 0; iThisSlotIndex < pkFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
				{
					const CvFormationSlotEntry& thisSlotEntry = pkFormation->getFormationSlotEntry(iThisSlotIndex);
					if (thisSlotEntry.m_requiredSlot)
						iNumRequiredSlots++;
				}

				if (pThisArmy->GetNumSlotsFilled() < iNumRequiredSlots / 2)
				{
					SetToAbort( AI_ABORT_HALF_STRENGTH);
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
		pCity = OperationalAIHelpers::GetNearestCoastalCityFriendly(m_eOwner, m_eEnemy);
	else
		pCity = GET_PLAYER(m_eOwner).GetClosestCityByEstimatedTurns(GetMusterPlot());

	if (!pCity)
		return false;

	if(!m_viListOfUnitsWeStillNeedToBuild.empty() && pCity != NULL && pCity->getOwner() == m_eOwner)
	{
		OperationSlot thisSlot = m_viListOfUnitsWeStillNeedToBuild.front();
		CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		if (!pArmy)
			return false;
		CvMultiUnitFormationInfo* thisFormation = pArmy->GetFormation();
		if (!thisFormation)
			return false;
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisSlot.m_iSlotID);

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

/// Read serialized data
void CvAIOperation::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iID;
	kStream >> m_eType;
	kStream >> m_eArmyType;
	kStream >> m_eOwner;
	kStream >> m_eEnemy;
	kStream >> m_eCurrentState;
	kStream >> m_eAbortReason;
	kStream >> m_iTargetX;
	kStream >> m_iTargetY;
	kStream >> m_iMusterX;
	kStream >> m_iMusterY;
	kStream >> m_iTurnStarted;
	kStream >> m_iLastTurnMoved;
	kStream >> m_viArmyIDs;
	kStream >> m_viListOfUnitsWeStillNeedToBuild;
	kStream >> m_viListOfUnitsCitiesHaveCommittedToBuild;
	kStream >> m_iDistanceMusterToTarget;
	//kStream >> m_progressToTarget;
}

/// Write serialized data
void CvAIOperation::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iID;
	kStream << m_eType;
	kStream << m_eArmyType;
	kStream << m_eOwner;
	kStream << m_eEnemy;
	kStream << m_eCurrentState;
	kStream << m_eAbortReason;
	kStream << m_iTargetX;
	kStream << m_iTargetY;
	kStream << m_iMusterX;
	kStream << m_iMusterY;
	kStream << m_iTurnStarted;
	kStream << m_iLastTurnMoved;
	kStream << m_viArmyIDs;
	kStream << m_viListOfUnitsWeStillNeedToBuild;
	kStream << m_viListOfUnitsCitiesHaveCommittedToBuild;
	kStream << m_iDistanceMusterToTarget;
	//kStream << m_progressToTarget;
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
			strTemp1.Format("Started Army %d, Units Recruited: %d, Max Formation Size: %d, ", pThisArmy->GetID(), pThisArmy->GetNumSlotsFilled(), pThisArmy->GetNumFormationEntries());
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
				szTemp2.Format("Recruiting Units for army %d, ", pThisArmy->GetID());
				strTemp += szTemp2;

				for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot* pSlot = pThisArmy->GetSlotStatus(iI);
					if(pSlot->IsFree())
					{
						szTemp2 = "Open, ";
					}
					else if(!pSlot->IsUsed())
					{
						szTemp2 = "Skipping, ";
					}
					else
					{
						CvUnit* pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("%s %d at (%d:%d) - ETA %d, ", 
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
				pThisArmy->GetCenterOfMass(false,&varX,&varY);
				szTemp2.Format("Gathering Forces for army %d, center (%d:%d), variance (%.2f:%.2f), ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY(), varX, varY);
				strTemp += szTemp2;
			}
			break;
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GetArmy(uiI);
				szTemp2.Format("Moving To Target with Army %d, center (%d:%d), target (%d:%d), Progress %d percent", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY(), m_iTargetX, m_iTargetY, PercentFromMusterPointToTarget());
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

bool CvAIOperation::SetupWithSingleArmy(CvPlot * pMusterPlot, CvPlot * pTargetPlot, CvPlot * pDeployPlot, CvUnit* pInitialUnit, bool bOceanMoves, bool bSkipRecruiting)
{
	//pDeployPlot may be null ...
	if (!pMusterPlot || !pTargetPlot)
	{
		if (GC.getLogging() && GC.getAILogging())
			LogOperationSpecialMessage("Cannot set up operation - no target or no muster!");

		SetToAbort( pTargetPlot ? AI_ABORT_NO_MUSTER : AI_ABORT_NO_TARGET );
		return false;
	}

	CvArmyAI* pArmyAI = AddArmy(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	if(!pArmyAI)
		return false;

	//this is for the operation
	SetTurnStarted(GC.getGame().getGameTurn());
	SetTargetPlot(pTargetPlot);
	SetMusterPlot(pMusterPlot);

	//this is for the army
	if (!pDeployPlot)
		pDeployPlot = pTargetPlot;

	//do not check whether the domain is correct, trust the caller
	//IsNavalOperation() is not detailed enough to judge

	pArmyAI->SetGoalPlot(pDeployPlot);
	pArmyAI->SetXY(pMusterPlot->getX(), pMusterPlot->getY());
	pArmyAI->SetOceanMoves(bOceanMoves);

	if (pInitialUnit)
		pArmyAI->AddUnit(pInitialUnit->GetID(),0,true);

	if (bSkipRecruiting && pInitialUnit)
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
	GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot(), pArmyAI);

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
bool CvAIOperation::FindBestFitReserveUnit(OperationSlot thisOperationSlot, vector<OptionWithScore<int>>& choices)
{
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;
	if(!pThisArmy)
		return false;

	CvMultiUnitFormationInfo* thisFormation = pThisArmy->GetFormation();
	if (!thisFormation)
		return false;

	CvArmyFormationSlot* pSlot = pThisArmy->GetSlotStatus(thisOperationSlot.m_iSlotID);
	if (!pSlot)
		return false;

	const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
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
// CvAIOperation
////////////////////////////////////////////////////////////////////////////////

/// How long will we wait for a recruit to show up?
int CvAIOperationMilitary::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY();
}

AIOperationAbortReason CvAIOperationMilitary::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	//should we be concentrating on defense instead?
	CvCity* pTroubleSpot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity(false, IsNavalOperation());
	if (pTroubleSpot)
	{
		CvTacticalAnalysisMap* pTactMap = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap();
		if (pTactMap->IsInEnemyDominatedZone(pTroubleSpot->plot()))
		{
			//the trouble spot is right next to us, abort the current operation
			SPathFinderUserData data(m_eOwner,PT_GENERIC_SAME_AREA,NO_PLAYER,GC.getAI_TACTICAL_RECRUIT_RANGE());
			data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
			if (GC.GetStepFinder().DoesPathExist(pTroubleSpot->plot(),pArmy->GetCenterOfMass(),data))
			{
				return AI_ABORT_CANCELLED;
			}
		}
	}

	//somebody unexpected owning the target? abort. don't check for war, that would break sneak attacks
	if (GetTargetPlot()->isOwned() && GetTargetPlot()->getOwner()!=m_eEnemy)
		return AI_ABORT_TARGET_ALREADY_CAPTURED;

	return NO_ABORT_REASON;
}

//sometimes we don't really mean it
bool CvAIOperationMilitary::IsShowOfForce() const
{
	return GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetWarGoal(GetEnemy()) == WAR_GOAL_DEMAND;
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
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return MUFORMATION_ANTI_BARBARIAN_TEAM;
	case AI_OPERATION_PILLAGE_ENEMY:
		return MUFORMATION_FAST_PILLAGERS;
	case AI_OPERATION_NUKE_ATTACK:
		return MUFORMATION_NUKE_ATTACK;

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
		OutputDebugString("warning, no formation for unexpected operation type\n");
		return NO_MUFORMATION;
	}
}

/// Kick off this operation
void CvAIOperationMilitary::Init(int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	if (!pTarget)
		return;

	if (!pMuster)
		pMuster = GET_PLAYER(m_eOwner).GetClosestCity(pTarget->plot(),23,true);

	if (!pMuster)
		return;

	SetupWithSingleArmy(pMuster->plot(),pTarget->plot());
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityAttackLand
////////////////////////////////////////////////////////////////////////////////


/// Kick off this operation
void CvAIOperationCityAttackLand::Init(int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	if (!pMuster || !pTarget)
		return;

	SetupWithSingleArmy(pMuster->plot(),pTarget->plot());
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
				if (plotDistance(*pCoM,*GetTargetPlot())<plotDistance(*GetMusterPlot(),*GetTargetPlot()) &&	pCoM->getOwner() == m_eOwner)
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

						CvPlot *pOtherTarget = NULL;
						for (int i=0; i<RING3_PLOTS; i++)
						{
							CvPlot* pTestPlot = iterateRingPlots( pCenterOfMass, i );
							if (pTestPlot && pTestPlot->getOwner()==m_eEnemy && pTestPlot->getOwningCity())
							{
								pOtherTarget = pTestPlot;
								break;
							}
						}

						if (pOtherTarget != NULL)
							pTarget = pOtherTarget;
					}
				}

				if(bInPlace)
				{
					// Notify Diplo AI we're in place for attack (unless this is just for show)
					if(!IsShowOfForce() && GET_PLAYER(m_eOwner).IsAtPeaceWith(m_eEnemy))
						GET_PLAYER(m_eOwner).GetDiplomacyAI()->SetArmyInPlaceForAttack(m_eEnemy, true);

					//that's it. skip STATE_AT_TARGET so the army will be disbanded next turn!
					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
					pThisArmy->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);
					LogOperationSpecialMessage("Transition to finished stage");

					OnSuccess();
					bStateChanged = true;
				}
			}
			break;
		}
	}

	return bStateChanged;
}

/// Find the city we want to attack
CvPlot* CvAIOperationMilitary::FindBestTarget(CvPlot** ppMuster) const
{
	//no change here
	if (ppMuster)
		*ppMuster = GetMusterPlot();

	return GetTargetPlot();
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationAntiBarbarian
////////////////////////////////////////////////////////////////////////////////

void CvAIOperationAntiBarbarian::Init(int /*iAreaID*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/, bool /*bOceanMoves */)
{
	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster,pTarget);
}

bool CvAIOperationAntiBarbarian::PreconditionsAreMet(CvPlot* pMusterPlot, int iMaxMissingUnits)
{
	//overwrite pTarget / pMuster (should be null anyway)
	CvPlot* pTargetPlot = FindBestTarget(&pMusterPlot);
	if (!pTargetPlot || !pMusterPlot)
		return false;

	if (!CvAIOperation::PreconditionsAreMet(pMusterPlot, iMaxMissingUnits))
		return false;

	return true;
}

/// How close to target do we end up?
int CvAIOperationAntiBarbarian::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE();
}

int CvAIOperationAntiBarbarian::GetMaximumRecruitTurns() const
{
	//don't block units in partial armies early on
	return 4;
}

/// Read serialized data
void CvAIOperationAntiBarbarian::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
	kStream >> m_iUnitToRescue;
}

/// Write serialized data
void CvAIOperationAntiBarbarian::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
	kStream << m_iUnitToRescue;
}

/// Find the barbarian camp we want to eliminate
CvPlot* CvAIOperationAntiBarbarian::FindBestTarget(CvPlot** ppMuster) const
{
	return OperationalAIHelpers::FindClosestBarbarianCamp(m_eOwner,ppMuster);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationPillageEnemy
////////////////////////////////////////////////////////////////////////////////

void CvAIOperationPillageEnemy::Init(int /*iAreaID*/, CvCity*, CvCity*, bool /*bOceanMoves */)
{
	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster,pTarget);
}

bool CvAIOperationPillageEnemy::PreconditionsAreMet(CvPlot* pMusterPlot, int iMaxMissingUnits)
{
	//overwrite pTarget / pMuster (should be null anyway)
	CvPlot* pTargetPlot = FindBestTarget(&pMusterPlot);
	if (!pTargetPlot || !pMusterPlot)
		return false;

	if (!CvAIOperation::PreconditionsAreMet(pMusterPlot, iMaxMissingUnits))
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
		CvCity* pClosestCity = kPlayer.GetClosestCityByEstimatedTurns(pLoopCity->plot());
		if (!pClosestCity)
			continue;
		
		if (pLoopCity->getArea() != pClosestCity->getArea() && !kPlayer.CanCrossOcean())
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
		CvCity *pClosest = pBestTargetCity ? kPlayer.GetClosestCityByEstimatedTurns(pBestTargetCity->plot()) : NULL;
		*ppMuster = pClosest ? pClosest->plot() : NULL;
	}

	return pBestTargetCity ? pBestTargetCity->plot() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilian
////////////////////////////////////////////////////////////////////////////////

/// Kick off this operation
void CvAIOperationCivilian::Init(int iAreaID, CvCity* /*pTarget*/, CvCity* /*pMuster*/, bool /*bOceanMoves */)
{
	CvUnit* pOurCivilian = FindBestCivilian();
	if (!pOurCivilian)
	{
		SetToAbort(AI_ABORT_NO_UNITS);
		return;
	}

	CvPlot* pTargetSite = FindBestTargetForUnit(pOurCivilian,iAreaID);
	if (!pTargetSite)
	{
		SetToAbort(AI_ABORT_NO_TARGET);
		return;
	}

	bool bCloseTarget = (pOurCivilian->TurnsToReachTarget(pTargetSite, CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN, 1) < 1);

	//don't wait for the escort in the wild (happens with settlers a lot)
	CvPlot* pMusterPlot = pOurCivilian->plot();
	if (IsEscorted() && !pMusterPlot->IsFriendlyTerritory(m_eOwner))
	{
		CvCity* pClosestCity = GET_PLAYER(m_eOwner).GetClosestCityByEstimatedTurns(pOurCivilian->plot());
		if (pClosestCity)
			pMusterPlot = pClosestCity->plot();
	}

	//problem: naval op but civilian not in a coastal city
	if (IsNavalOperation() && !(pMusterPlot->isCity() && pMusterPlot->getPlotCity()->isCoastal()))
	{
		CvCity* pMusterCity = OperationalAIHelpers::GetNearestCoastalCityFriendly(m_eOwner, pMusterPlot);
		if (pMusterCity)
			pMusterPlot = pMusterCity->plot();
	}

	//Let's not muster directly in the city - escort might conflict with garrison
	//todo: choose a muster plot on the way to the target!
	if(pMusterPlot->isCity())
	{
		for (int iCityPlotLoop = 1; iCityPlotLoop < RING3_PLOTS; iCityPlotLoop++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pMusterPlot->getX(), pMusterPlot->getY(), iCityPlotLoop);

			// Invalid plot or not owned by this player
			if (pLoopPlot == NULL || pLoopPlot->getOwner() != m_eOwner) 
				continue;

			//No water and not impassable
			if(IsNavalOperation() && !(pLoopPlot->isWater() && pLoopPlot->area()->getNumTiles()>GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				continue;

			if(!IsNavalOperation() && pLoopPlot->isWater())
				continue;

			if(pLoopPlot->isImpassable())
				continue;

			//Empty
			if(pLoopPlot->getNumDefenders(m_eOwner) > 0)
				continue;

			//Not dangerous
			if (pOurCivilian->GetDanger(pLoopPlot)==0)
			{
				pMusterPlot = pLoopPlot;
				break;
			}
		}
	}

	SetupWithSingleArmy(pMusterPlot,pTargetSite,pTargetSite,pOurCivilian,GET_PLAYER(m_eOwner).CanCrossOcean(),bCloseTarget);
}

bool CvAIOperationCivilian::CheckTransitionToNextStage()
{
	//only a single army right now!
	CvArmyAI* pThisArmy = GetArmy(0);
	if(!pThisArmy)
		return false;

	//we look at the army's state. the operation state is really not required
	bool bStateChanged = false;
	switch( pThisArmy->GetArmyAIState() )
	{
		case ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE:
		{
			if(OperationalAIHelpers::HaveEnoughUnits(pThisArmy->GetSlotStatus(),0))
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
			CvUnit* pCivilian = pThisArmy->GetFirstUnit();
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
			CvUnit* pCivilian = pThisArmy->GetFirstUnit();
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
			CvUnit* pCivilian = pThisArmy->GetFirstUnit();

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
		if (pLoopUnit->AI_getUnitAIType() == GetCivilianType() || (pLoopUnit->CanFoundColony() && (GetOperationType() == AI_OPERATION_FOUND_CITY)))
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
	CvPlot* pBetterTarget = FindBestTargetForUnit(pCivilian,pCurrentTarget?pCurrentTarget->getArea():-1);

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


void CvAIOperationCivilianFoundCity::Init(int iAreaID, CvCity * pTarget, CvCity * pMuster, bool bQuick)
{
	m_eFormation = bQuick ? MUFORMATION_QUICK_COLONY_SETTLER : MUFORMATION_SETTLER_ESCORT;

	CvAIOperationCivilian::Init(iAreaID, pTarget, pMuster, bQuick);
}

bool CvAIOperationCivilianFoundCity::PerformMission(CvUnit* pSettler)
{
	// If the settler made it, we don't care about the entire army
	CvPlot* pCityPlot = GetTargetPlot();
	if(pSettler && pSettler->canFoundCity(pCityPlot) && pSettler->plot() == pCityPlot && pSettler->canMove())
	{
		//check this before building the new city ...
		bool bIsFrontier = (GC.getGame().GetClosestCityDistanceInTurns(pCityPlot) < GET_PLAYER(m_eOwner).GetCityDistanceInEstimatedTurns(pCityPlot));

		pSettler->PushMission(CvTypes::getMISSION_FOUND());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvCity* pCity = pCityPlot->getPlotCity();
			if (pCity != NULL)
			{
				CvString strMsg;
				strMsg.Format("City founded (%s), At X=%d, At Y=%d, plot value %d", pCity->getName().c_str(), 
					pCityPlot->getX(), pCityPlot->getY(), pCityPlot->getFoundValue(m_eOwner));
				LogOperationSpecialMessage(strMsg);
			}
		}

		// Notify tactical AI to focus on this area
		if (bIsFrontier)
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddFocusArea( pCityPlot, 2, GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() * 3 );

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
		CvPlot* pBetterTarget = FindBestTargetIncludingCurrent(pSettler, GetTargetPlot()->getArea());

		// No targets at all!
		if(pBetterTarget == NULL)
		{
			LogOperationSpecialMessage( CvString::format("no valid city site target found (%s)!", IsEscorted() ? "with escort" : "no escort").c_str() );
			return AI_ABORT_NO_TARGET;
		}

		// swtich if we have a better target
		if (pBetterTarget != GetTargetPlot())
		{
			SetTargetPlot(pBetterTarget);
			pArmy->SetGoalPlot(pBetterTarget);
		}

		return NO_ABORT_REASON;
	}
}

/// Find the plot where we want to settle
CvPlot* CvAIOperationCivilianFoundCity::FindBestTargetIncludingCurrent(CvUnit* pUnit, int iAreaID)
{
	//todo: better options
	//a) return a list of possible targets and find the ones that are currently reachable
	//b) if the best target is unreachable, move in the general direction and hope the block will clear up

	bool bIsSafe = false;
	//ignore the current operation target when searching. default would be to suppress currently targeted plots
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, iAreaID, bIsSafe, this);
	if (!bIsSafe && !IsEscorted())
		pResult = NULL;

	//try again if the result is not good
	if (pResult == NULL && iAreaID != -1)
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, -1, bIsSafe, this);
	if (!bIsSafe && !IsEscorted())
		pResult = NULL;

	LogSettleTarget("BestWithCurrent", pResult);
	return pResult;
}

//need to have this, it's pure virtual in civilian operation
CvPlot* CvAIOperationCivilianFoundCity::FindBestTargetForUnit(CvUnit* pUnit, int iAreaID)
{
	bool bIsSafe = false;
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, iAreaID, bIsSafe);
	if (!bIsSafe && !IsEscorted())
		pResult = NULL;

	//try again if the result is not good
	if (pResult == NULL && iAreaID != -1 )
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, -1, bIsSafe);
	if (!bIsSafe && !IsEscorted())
		pResult = NULL;

	LogSettleTarget("BestNew", pResult);
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
				strMsg.Format("Great Merchant buying city-state, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
		}
		else
		{
			pMerchant->PushMission(CvTypes::getMISSION_TRADE());
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Great Merchant finishing trade mission, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
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
CvPlot* CvAIOperationCivilianMerchantDelegation::FindBestTargetForUnit(CvUnit* pUnit, int /*iAreaID*/)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).GreatMerchantWantsCash() ?
		GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlotForCash(pUnit) :
		GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlotForPuppet(pUnit);
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianDiplomatDelegation
////////////////////////////////////////////////////////////////////////////////

/// Find the plot where we want to influence
CvPlot* CvAIOperationCivilianDiplomatDelegation::FindBestTargetForUnit(CvUnit* pUnit, int /*iAreaID*/)
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
#endif

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianConcertTour
////////////////////////////////////////////////////////////////////////////////

/// Find the plot where we want to settler
CvPlot* CvAIOperationCivilianConcertTour::FindBestTargetForUnit(CvUnit* pUnit, int /*iAreaID*/)
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
			strMsg.Format("Great Musician performing concert tour, At X=%d, At Y=%d", pMusician->plot()->getX(), pMusician->plot()->getY());
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
void CvAIOperationNavalSuperiority::Init(int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	if (!pMuster || !pTarget)
		return;

	//this is where we gather the army
	CvPlot* pMusterPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMuster->plot());
	//this is where the army should go
	CvPlot* pGoalPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTarget->plot());
	if (!pMusterPlot || !pGoalPlot)
		return;

	CvPlot* pTargetPlot = FindBestTarget(&pMusterPlot);
	if (pTargetPlot != NULL)
	{
		pTargetPlot = pGoalPlot;
	}
	SetupWithSingleArmy(pMusterPlot, pTargetPlot, NULL, NULL, bOceanMoves);
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
	CvPlot* pPlot = NULL;

	// Defend the city most under threat
	CvCity* pTargetCity = GET_PLAYER(m_eOwner).GetThreatenedCityByRank();

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if (pTargetCity == NULL || !pTargetCity->isCoastal())
	{
		CvCity* pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if (pEnemyCapital != NULL)
		{
			pTargetCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, true, NO_TEAM, NO_DIRECTION, NULL);
		}
	}

	if (pTargetCity != NULL && pTargetCity->isCoastal())
	{
		pPlot = pTargetCity->plot();

		if (ppMuster)
		{
			CvCity* pMusterCity = GET_PLAYER(m_eOwner).GetClosestCity(pPlot, 23, true);
			if (!pMusterCity)
				pMusterCity = pTargetCity;

			if (pMusterCity->isCoastal())
			{
				*ppMuster = pMusterCity->plot();
			}
		}
	}

	return pPlot;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityAttackNaval
////////////////////////////////////////////////////////////////////////////////


/// Kick off this operation
void CvAIOperationCityAttackNaval::Init(int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	CvPlot* pMusterPlot = NULL;
	CvPlot* pTargetPlot = NULL;

	if(pMuster == NULL || !pMuster->isCoastal())
	{
		CvCity* pNearestCoastalCity = OperationalAIHelpers::GetNearestCoastalCityFriendly(m_eOwner,m_eEnemy);
		if(pNearestCoastalCity != NULL)
		{
			pMuster = pNearestCoastalCity;
		}
	}
	if(pMuster != NULL)
	{
		pMusterPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMuster->plot());
	}

	if(pTarget == NULL || !pTarget->isCoastal())
	{
		CvCity* pNearestCoastalCity = OperationalAIHelpers::GetNearestCoastalCityEnemy(m_eOwner,m_eEnemy);
		if(pNearestCoastalCity != NULL)
		{
			pTarget = pNearestCoastalCity;
		}
	}
	if(pTarget != NULL)
	{
		pTargetPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTarget->plot());
	}

	SetupWithSingleArmy(pMusterPlot, pTargetPlot, NULL, NULL, bOceanMoves);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDefendCity - Place holder
////////////////////////////////////////////////////////////////////////////////

/// Kick off this operation
void CvAIOperationDefendCity::Init(int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	CvPlot* pMusterPlot = pMuster->plot();
	CvPlot* pTargetPlot = pTarget->plot();
	if (!pTargetPlot || !pMusterPlot)
		return;

	SetupWithSingleArmy(pMusterPlot,pTargetPlot);
}

AIOperationAbortReason CvAIOperationDefendCity::VerifyOrAdjustTarget(CvArmyAI* /*pArmy*/)
{
	// See if our target city is still owned by us
	if(GetTargetPlot()->getOwner() != m_eOwner)
	{
		// Bummer! The city has been captured/destroyed
		return AI_ABORT_TARGET_ALREADY_CAPTURED;
	}

	return NO_ABORT_REASON;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDefenseRapidResponse
////////////////////////////////////////////////////////////////////////////////

/// Kick off this operation
void CvAIOperationDefenseRapidResponse::Init(int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	CvPlot* pMusterPlot = pMuster->plot();
	CvPlot* pTargetPlot = pTarget->plot();
	if (!pTargetPlot || !pMusterPlot)
		return;

	SetupWithSingleArmy(pMusterPlot,pTargetPlot);
}

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
	int iRefArea = -1;
	CvPlot* pRefPlot = GetTargetPlot();
	if (pRefPlot)
		iRefArea = pRefPlot->getArea();

	CvPlot* pTarget = OperationalAIHelpers::FindEnemiesNearPlot(m_eOwner,m_eEnemy,DOMAIN_LAND,true,iRefArea,pRefPlot);
	if (ppMuster)
		*ppMuster = pTarget;
	return pTarget;
}

/// Kick off this operation
void CvAIOperationCityAttackCombined::Init(int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	if (!pTarget || !pMuster)
		return;

	// Target just off the coast - important that it is owned by eEnemy - we terminate if that should ever change
	CvPlot *pCoastalTarget = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTarget->plot());
	// Muster just off the coast
	CvPlot *pCoastalMuster = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMuster->plot());

	SetupWithSingleArmy(pCoastalMuster, pCoastalTarget, NULL, NULL, bOceanMoves);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationMilitary
////////////////////////////////////////////////////////////////////////////////

void CvAIOperationMilitary::OnSuccess() const
{
	CvPlot* pPlot = GetTargetPlot();
	if (!pPlot->isCity() && m_eEnemy != NO_PLAYER)
	{
		if (GET_PLAYER(m_eEnemy).GetCityDistanceInPlots(pPlot) < 4)
		{
			CvCity* pCity = GET_PLAYER(m_eEnemy).GetClosestCityByEstimatedTurns(pPlot);
			if (pCity)
				pPlot = pCity->plot();
		}
	}

	GET_PLAYER(m_eOwner).GetTacticalAI()->AddFocusArea( pPlot, 3, GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() );
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNukeAttack
////////////////////////////////////////////////////////////////////////////////

void CvAIOperationNukeAttack::Init(int /*iAreaID*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/, bool /*bOceanMoves */)
{
	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster, pTarget, pMuster);
}

bool CvAIOperationNukeAttack::PreconditionsAreMet(CvPlot* pMusterPlot, int iMaxMissingUnits)
{
	//overwrite pTarget / pMuster (should be null anyway)
	CvPlot* pTargetPlot = FindBestTarget(&pMusterPlot);
	if (!pTargetPlot || !pMusterPlot)
		return false;

	if (!CvAIOperation::PreconditionsAreMet(pMusterPlot, iMaxMissingUnits))
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
				int iBlastRadius = min(5,max(1,GC.getNUKE_BLAST_RADIUS()));
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
					strMsg.Format("City nuked, At X=%d, At Y=%d", pTargetPlot->getX(), pTargetPlot->getY());
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
			int iBlastRadius = min(5,max(1,GC.getNUKE_BLAST_RADIUS()));
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
							if(GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetMajorCivApproach(eUnitOwner, false) == MAJOR_CIV_APPROACH_WAR)
							{
								iThisCityValue += 500;
							}
							else if(GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetMajorCivApproach(eUnitOwner, false) == MAJOR_CIV_APPROACH_HOSTILE)
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
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, vector<OptionWithScore<int>>&)
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

//----------------------------------------------
/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
AIOperationAbortReason CvAIOperationAntiBarbarian::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	// camp might have spawned ... abort
	if (m_eCurrentState != AI_OPERATION_STATE_RECRUITING_UNITS)
		if(pArmy->GetTotalPower() < GetTargetPlot()->GetAdjacentEnemyPower(m_eOwner))
			return AI_ABORT_TOO_DANGEROUS;

	bool bNeedNewTarget = false;
	CvString strMsg;

	if(GetTargetPlot()==NULL)
		bNeedNewTarget = true;
	else if (m_iUnitToRescue<0)
	{
		// See if our target camp is still there
		ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();
		if (GetTargetPlot()->getImprovementType() != eBarbCamp)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("Barbarian camp at (x=%d y=%d) no longer exists.", GetTargetPlot()->getX(), GetTargetPlot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
			bNeedNewTarget = true;
		}
	}
	else
	{
		// is the unit rescued?
		CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
		CvUnit* pUnitToRescue = BarbPlayer.getUnit(m_iUnitToRescue);
		if (!pUnitToRescue)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format ("Civilian %d can no longer be rescued from barbarians.", m_iUnitToRescue);
				LogOperationSpecialMessage(strMsg);
			}
			bNeedNewTarget = true;
		}
	}

	if (bNeedNewTarget)
	{
		//todo: enforce area of new target?
		CvPlot* pNewTarget = FindBestTarget(NULL);
		if(pNewTarget != NULL)
		{
			CvCity* pMusterCity = GET_PLAYER(m_eOwner).GetClosestCity(pNewTarget,15,true);
			if (!pMusterCity)
				return AI_ABORT_NO_MUSTER;

			SetTargetPlot(pNewTarget);
			SetMusterPlot(pMusterCity->plot());

			CvUnit* pRescueUnit = pNewTarget->getFirstUnitOfAITypeSameTeam(BARBARIAN_TEAM, UNITAI_SETTLE);
			if (!pRescueUnit)
				pRescueUnit = pNewTarget->getFirstUnitOfAITypeSameTeam(BARBARIAN_TEAM, UNITAI_WORKER);
			if (pRescueUnit)
				m_iUnitToRescue = pRescueUnit->GetID();

			// If we're traveling on a single continent, set our destination to be a few plots shy of the final target
			if (pArmy->GetArea() == GetTargetPlot()->getArea())
			{
				SPathFinderUserData data( m_eOwner, PT_GENERIC_SAME_AREA, m_eEnemy );
				SPath path = GC.GetStepFinder().GetPath( pArmy->GetCurrentPlot(), GetTargetPlot(), data );
				data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
				if (!!path)
				{
					CvPlot* pDeployPt = PathHelpers::GetXPlotsFromEnd(path, GetDeployRange(), false);
					if(pDeployPt != NULL)
					{
						pArmy->SetGoalPlot(pDeployPt);
						SetTargetPlot(GetTargetPlot());
					}
				}
			}
			else
			{
				pArmy->SetGoalPlot(GetTargetPlot());
				SetTargetPlot(GetTargetPlot());
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("Moving on to a new target (%d,%d).", GetTargetPlot()->getX(), GetTargetPlot()->getY());
				LogOperationSpecialMessage(strMsg);
			}

			return NO_ABORT_REASON;
		}
		else
			return AI_ABORT_NO_TARGET;
	}
	else
		return NO_ABORT_REASON;
}

AIOperationAbortReason CvAIOperationCivilian::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	CvPlot* pTarget = GetTargetPlot();
	if (!pTarget)
		return AI_ABORT_NO_TARGET;

	//if we actually do have an escort, danger is ok
	if (IsEscorted())
		return NO_ABORT_REASON;

	CvUnit* pCivilian = pArmy->GetFirstUnit();
	if (!pCivilian)
		return AI_ABORT_LOST_CIVILIAN;

	if (pCivilian->GetDanger(pTarget) < INT_MAX && GET_PLAYER(m_eOwner).IsAtPeaceWith(pTarget->getOwner()))
	{
		return NO_ABORT_REASON;
	}
	else
		return AI_ABORT_TOO_DANGEROUS;
}

bool CvAIOperationCivilian::IsEscorted() const
{
	if (m_eCurrentState==AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_INVALID)
	{
		//be careful, don't check the army directly, it might not exist right now
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
		if (thisFormation && thisFormation->getNumFormationSlotEntriesRequired()>1)
			return true;

		return false;
	}
	else
	{
		CvArmyAI* pThisArmy = GetArmy(0);

		//the unit to be escorted is always the first one
		CvUnit* pCivilian = pThisArmy ? pThisArmy->GetFirstUnit() : NULL;
		if (!pCivilian)
			return false;
		//the second unit would be the first escort
		CvUnit* pEscort = pThisArmy->GetNextUnit(pCivilian);

		return pEscort != NULL;
	}
}

/// Read serialized data
void CvAIOperationCivilian::Read(FDataStream& kStream)
{
	CvAIOperation::Read(kStream);
	kStream >> m_eCivilianType;
	kStream >> m_eFormation;
}

/// Write serialized data
void CvAIOperationCivilian::Write(FDataStream& kStream) const
{
	CvAIOperation::Write(kStream);
	kStream << m_eCivilianType;
	kStream << m_eFormation;
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
	ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();

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
				//maybe we could reach it with embarkation but too complex to check
				//also if the camp is really close, tactical AI should be able to handle it without forming an army
				if (pPlot->getArea() != pLoopCity->getArea() && !GET_PLAYER(ePlayer).CanCrossOcean())
					continue;

				int iCurPlotDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
				if (pPlot->getArea() != pLoopCity->getArea())
					iCurPlotDistance *= 2;

				int iScore = 1000 - iCurPlotDistance + iBonus;
				if (iScore > iBestScore)
				{
					pBestPlot = pPlot;
					iBestScore = iScore;
					pClosestCity = pLoopCity;
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
	CvMultiUnitFormationInfo* thisFormation = pThisArmy->GetFormation();
	if (thisFormation)
	{
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
		return thisSlotEntry.m_requiredSlot;
	}
	return false;
}

int OperationalAIHelpers::IsUnitSuitableForRecruitment(CvUnit* pLoopUnit, const ReachablePlots& turnsFromMuster, bool bMustEmbark, bool bMustBeDeepWaterNaval, const vector<pair<int,CvFormationSlotEntry>>& availableSlots)
{
	//otherwise engaged?
	if (!pLoopUnit->canRecruitFromTacticalAI())
		return -1;

	//In an army?
	if (pLoopUnit->getArmyID() != -1)
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
	if (pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
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
	{
		return -1;
	}

	//don't pull out units from zones we need to defend
	CvTacticalDominanceZone *pZone = GET_PLAYER(pLoopUnit->getOwner()).GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pLoopUnit->plot());
	if (pZone && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
		//exception: ships in harbor
		if (pLoopUnit->getDomainType()!=DOMAIN_SEA || !pLoopUnit->plot()->isCity())
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
			iMatchingIndex = i;
			break;
		}
	}

	return iMatchingIndex;
}

CvPlot* OperationalAIHelpers::FindEnemiesNearPlot(PlayerTypes ePlayer, PlayerTypes eEnemy, DomainTypes eDomain, bool bHomelandOnly, int iRefArea, CvPlot* pRefPlot)
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

		if (bHomelandOnly && pLoopPlot->getOwner()!=ePlayer && GET_PLAYER(ePlayer).GetCityDistanceInPlots(pLoopPlot)>4) 
			continue;

		if (!bHomelandOnly && pLoopPlot->getOwner() == ePlayer)
			continue;

		if (iRefArea!=-1 && pLoopPlot->getArea()!=iRefArea && !pLoopPlot->isAdjacentToArea(iRefArea))
			continue;

		//a single unit is too volatile, check for a whole cluster
		int iEnemyPower = pLoopPlot->GetAdjacentEnemyPower(ePlayer);

		//we don't want to adjust our target too much
		int iDistance = pRefPlot ? plotDistance(*pRefPlot,*pLoopPlot) : 0;
		iEnemyPower *= MapToPercent(iDistance,23,3);

		if(iEnemyPower > iMaxEnemyPower)
		{
			iMaxEnemyPower = iEnemyPower;
			pBestPlot = pLoopPlot;
		}
	}

	if(pBestPlot == NULL)
		return pRefPlot;

	return pBestPlot;	
}

/// Find our port operation operations against this enemy should leave from
CvCity* OperationalAIHelpers::GetNearestCoastalCityFriendly(PlayerTypes ePlayer, CvPlot* pRefPlot)
{
	if (ePlayer==NO_PLAYER || !pRefPlot)
		return NULL;

	CvCity* pBestCoastalCity = NULL;
	CvCity* pLoopCity;
	int iLoop;
	int iBestDistance = MAX_INT;

	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal() && pLoopCity->isAdjacentToArea(pRefPlot->getArea()))
		{
			int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pRefPlot->getX(), pRefPlot->getY());
			if(iDistance < iBestDistance)
			{
				iBestDistance = iDistance;
				pBestCoastalCity = pLoopCity;
			}
		}
	}

	return pBestCoastalCity;
}


/// Find our port operation operations against this enemy should leave from
CvCity* OperationalAIHelpers::GetNearestCoastalCityFriendly(PlayerTypes ePlayer, PlayerTypes eEnemy)
{
	if (ePlayer==NO_PLAYER || eEnemy==NO_PLAYER)
		return NULL;

	CvCity* pBestCoastalCity = NULL;
	CvCity* pLoopCity, *pEnemyCity;
	int iLoop, iEnemyLoop;
	int iBestDistance = MAX_INT;

	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal())
		{
			for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
			{
				// Check all revealed enemy cities
				if(pEnemyCity->isCoastal())
				{
					// On same body of water?
					if(pLoopCity->hasSharedAdjacentArea(pEnemyCity))
					{
						SPathFinderUserData data(ePlayer, PT_GENERIC_SAME_AREA, eEnemy);
						data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
						if (!GET_PLAYER(ePlayer).CanCrossOcean())
							data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

						int iDistance = GC.GetStepFinder().GetPathLengthInPlots(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY(), data);
						if(iDistance>0 && iDistance < iBestDistance)
						{
							iBestDistance = iDistance;
							pBestCoastalCity = pLoopCity;
						}
					}
				}
			}
		}
	}

	return pBestCoastalCity;
}

/// Find the port operations against this enemy should attack
CvCity* OperationalAIHelpers::GetNearestCoastalCityEnemy(PlayerTypes ePlayer, PlayerTypes eEnemy)
{
	if (ePlayer==NO_PLAYER || eEnemy==NO_PLAYER)
		return NULL;

	CvCity* pBestCoastalCity = NULL;
	CvCity* pLoopCity, *pEnemyCity;
	int iLoop, iEnemyLoop;
	int iBestDistance = MAX_INT;

	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal())
		{
			for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
			{
				// Check all revealed enemy cities
				if(pEnemyCity->isCoastal())
				{
					// On same body of water?
					if(pLoopCity->hasSharedAdjacentArea(pEnemyCity))
					{
						SPathFinderUserData data(ePlayer, PT_GENERIC_SAME_AREA, eEnemy);
						data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
						if (!GET_PLAYER(ePlayer).CanCrossOcean())
							data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;

						int iDistance = GC.GetStepFinder().GetPathLengthInPlots(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY(), data);
						if(iDistance>0 && iDistance < iBestDistance)
						{
							iBestDistance = iDistance;
							pBestCoastalCity = pEnemyCity;
						}
					}
				}
			}
		}
	}

	return pBestCoastalCity;
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

bool CvAIOperation::PreconditionsAreMet(CvPlot* pMusterPlot, int iMaxMissingUnits)
{
	//we check existance of suitable target in subclassed implementations

	//use all slots, not only required slots
	vector<pair<int,CvFormationSlotEntry>> freeSlots;
	vector<CvArmyFormationSlot> fakeStatus;
	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(OperationalAIHelpers::GetArmyFormationForOpType(m_eType));
	for (int i = 0; i < thisFormation->getNumFormationSlotEntries(); i++)
	{
		freeSlots.push_back( make_pair(i, thisFormation->getFormationSlotEntry(i)) );
		fakeStatus.push_back( CvArmyFormationSlot(-1,thisFormation->getFormationSlotEntry(i).m_requiredSlot) );
	}

	ReachablePlots turnsFromMuster;
	if (pMusterPlot)
	{
		//this is just a rough indication so we don't need to do pathfinding for all our units
		SPathFinderUserData data(m_eOwner, PT_GENERIC_REACHABLE_PLOTS, -1, GetMaximumRecruitTurns());
		turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);
	}

	size_t iExtraUnits = 0;
	int iLoop = 0;
	for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
	{
		//if we pass an empty turnsFromMuster, all units will be eligible no matter where they are
		int iIndex = OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit, turnsFromMuster, IsNavalOperation(), false, freeSlots);
		if (iIndex >= 0)
		{
			fakeStatus[freeSlots[iIndex].first] = CvArmyFormationSlot( pLoopUnit->GetID(), freeSlots[iIndex].second.m_requiredSlot );

			if (!freeSlots[iIndex].second.m_requiredSlot)
				iExtraUnits++;

			freeSlots.erase(freeSlots.begin() + iIndex);

			if (freeSlots.empty())
				break;
		}
	}

	//todo: check path for danger? eg embarkartion in enemy-dominated water?
	//	or check if there is enough room to deploy at the target?

	return OperationalAIHelpers::HaveEnoughUnits(fakeStatus,iMaxMissingUnits);
}

