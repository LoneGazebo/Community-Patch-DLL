/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
CvAIOperation::CvAIOperation()
{
	Reset();
}

/// Destructor
CvAIOperation::~CvAIOperation()
{
	Reset();
}

/// Delete allocated objects
void CvAIOperation::Reset(int iID, PlayerTypes eOwner, PlayerTypes eEnemy)
{
	if(m_eOwner != NO_PLAYER)  //if this has been init'ed this should not happen
	{
		CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);

		// remove the armies (which should, in turn, free up their units for other tasks)
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* thisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
			if(thisArmy)
			{
				DeleteArmyAI(m_viArmyIDs[uiI]);
				thisArmy->Kill();
			}
		}
	}

	// clear out the lists
	m_viArmyIDs.clear();
	m_viListOfUnitsWeStillNeedToBuild.clear();
	m_viListOfUnitsCitiesHaveCommittedToBuild.clear();

	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	m_eCurrentState = INVALID_AI_OPERATION_STATE;
	m_eAbortReason = NO_ABORT_REASON;

	m_iTargetX = INVALID_PLOT_COORD;
	m_iTargetY = INVALID_PLOT_COORD;
	m_iMusterX = INVALID_PLOT_COORD;
	m_iMusterY = INVALID_PLOT_COORD;

	m_iLastTurnMoved = -1;
	m_iTurnStarted = -1;
	m_iDistanceMusterToTarget = -1;
}

/// How long will we wait for a recruit to show up?
int CvAIOperation::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT();
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
		return;
	}

	if ( pTarget!=GetTargetPlot() )
	{
		LogOperationSpecialMessage( CvString::format("setting new target (%d:%d)",pTarget->getX(),pTarget->getY()).c_str() );

		//have to start moving again!
		if (m_eCurrentState == AI_OPERATION_STATE_AT_TARGET)
		{
			m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				if(pThisArmy)
					pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
			}
		}
	}

	m_iTargetX = pTarget->getX();
	m_iTargetY = pTarget->getY();

	//update the distance
	m_iDistanceMusterToTarget = GetStepDistanceBetweenPlots(GetMusterPlot(),GetTargetPlot());
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
		return;
	}

	m_iMusterX = pMuster->getX();
	m_iMusterY = pMuster->getY();

	//update the distance
	m_iDistanceMusterToTarget = GetStepDistanceBetweenPlots(GetMusterPlot(),GetTargetPlot());
}

int CvAIOperation::GetGatherTolerance(CvArmyAI* pArmy, CvPlot* pPlot) const
{
	int iRtnValue = 1;
	int iValidPlotsNearby = 0;

	// Find out how many units are trying to gather
	int iNumUnits = pArmy->GetNumSlotsFilled();

	// If not more than 1, zero tolerance is fine (we should get the unit to the gather point)
	if(iNumUnits < 1)
	{
		iRtnValue = 0;
	}
	else
	{
		int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iNumUnits);

		//naval needs more space without ocean travel.
		if (IsNavalOperation() && !GET_PLAYER(pArmy->GetOwner()).CanCrossOcean())
			iRange++;

		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				CvPlot* pLoopPlot = GC.getMap().plot(pPlot->getX()+iX, pPlot->getY()+iY);

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
		}

		// Find more valid plots than units?
		if(iValidPlotsNearby > iNumUnits)
		{
			// If so, just use normal range for this many units
			iRtnValue = iRange;
		}
		// Something constrained here, give ourselves a lot of leeway
		else
		{
			iRtnValue = iRange + 1;
		}
	}

	return iRtnValue;
}

/// Factory method: you are responsible for cleaning up the operation created by this routine!
CvAIOperation* CvAIOperation::CreateOperation(AIOperationTypes eAIOperationType)
{
	switch(eAIOperationType)
	{
	case AI_OPERATION_CITY_BASIC_ATTACK:
		return FNEW(CvAIOperationCityBasicAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return FNEW(CvAIOperationAntiBarbarian(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_FOUND_CITY:
	case AI_OPERATION_FOUND_CITY_OVERSEAS: //should not be used anymore ...
		return FNEW(CvAIOperationCivilianFoundCity(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_MERCHANT_DELEGATION:
		return FNEW(CvAIOperationCivilianMerchantDelegation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_DIPLOMAT_DELEGATION:
		return FNEW(CvAIOperationCivilianDiplomatDelegation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_ALLY_DEFENSE:
		return FNEW(CvAIOperationDefendAlly(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CONCERT_TOUR:
		return FNEW(CvAIOperationCivilianConcertTour(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_BOMBARDMENT:
		return FNEW(CvAIOperationNavalBombardment(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return FNEW(CvAIOperationNavalSuperiority(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_ONLY_CITY_ATTACK:
		return FNEW(CvAIOperationNavalOnlyCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_FOUND_CITY_QUICK:
		return FNEW(CvAIOperationCivilianFoundCityQuick(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PILLAGE_ENEMY:
		return FNEW(CvAIOperationPillageEnemy(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_CLOSE_DEFENSE:
		return FNEW(CvAIOperationDefendCity(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_CLOSE_DEFENSE_PEACE:
		return FNEW(CvAIOperationDefendCityPeace(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_RAPID_RESPONSE:
		return FNEW(CvAIOperationDefenseRapidResponse(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_SNEAK_ATTACK:
		return FNEW(CvAIOperationCitySneakAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_STATE_ATTACK:
		return FNEW(CvAIOperationCityStateAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_INVASION:
		return FNEW(CvAIOperationNavalInvasion(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_INVASION_SNEAKY:
		return FNEW(CvAIOperationNavalInvasionSneaky(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_INVASION_CITY_STATE:
		return FNEW(CvAIOperationNavalInvasionCityState(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NUKE_ATTACK:
		return FNEW(CvAIOperationNukeAttack(), c_eCiv5GameplayDLL, 0);
	}

	return 0;
}

/// Compile a list of all the units we need
void CvAIOperation::BuildListOfUnitsWeStillNeedToBuild()
{
	m_viListOfUnitsCitiesHaveCommittedToBuild.clear();
	m_viListOfUnitsWeStillNeedToBuild.clear();

	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* pThisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
		// if it is still waiting on initial units
		if(pThisArmy && pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
		{
			int iThisFormationIndex = pThisArmy->GetFormationIndex();
			if(iThisFormationIndex != NO_MUFORMATION)
			{
				CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
				if(thisFormation)
				{
					for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
					{
						//is it still open?
						if (pThisArmy->GetFormationSlot(iThisSlotIndex)->GetUnitID()<0)
						{
							OperationSlot thisOperationSlot;
							thisOperationSlot.m_iOperationID = m_iID;
							thisOperationSlot.m_iArmyID = m_viArmyIDs[uiI];
							thisOperationSlot.m_iSlotID = iThisSlotIndex;
							m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);
						}
					}
				}
			}
			else
			{
#if defined(MOD_BALANCE_CORE)
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("ERROR - No Formation Type for Army!! Eek!");
					LogOperationSpecialMessage(strMsg);
				}
#endif
				// punt -- sub-class should be taking care of this
				CvAssertMsg(false,"A sub-class should have dealt with this");
			}
		}
	}
}

/// Find out the next item to build for this operation
OperationSlot CvAIOperation::PeekAtNextUnitToBuild()
{
	OperationSlot thisSlot;
	if(!m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.front();
	}
	return thisSlot;
}

/// Called by a city when it decides to build a unit
OperationSlot CvAIOperation::CommitToBuildNextUnit(int iAreaID, int iTurns, CvCity* pCity)
{
	OperationSlot thisSlot;
	if(iAreaID == -1 || (GetMusterPlot() && GetMusterPlot()->getArea()==iAreaID) && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.front();
		m_viListOfUnitsWeStillNeedToBuild.pop_front();
		m_viListOfUnitsCitiesHaveCommittedToBuild.push_back(thisSlot);

		CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		if(pArmy)
		{
			int iTurnsFromCityToMusterGuess;
			iTurnsFromCityToMusterGuess = plotDistance(pCity->getX(), pCity->getY(), pArmy->GetX(), pArmy->GetY()) / 2;
			pArmy->SetEstimatedTurn(thisSlot.m_iSlotID, iTurns + iTurnsFromCityToMusterGuess);
		}
	}
	return thisSlot;
}

/// Called by a city when it decides NOT to build a unit (that it had previously committed to)
bool CvAIOperation::UncommitToBuild(OperationSlot thisOperationSlot)
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
bool CvAIOperation::FinishedBuilding(OperationSlot thisOperationSlot)
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
	if(!pUnit || m_viArmyIDs.empty())
		return false;

	CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI( m_viArmyIDs[0] );

	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if(iThisFormationIndex == NO_MUFORMATION)
		return false;

	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
	if (!thisFormation)
		return false;

	CvUnitEntry* unitInfo = GC.getUnitInfo(pUnit->getUnitType());
	if (!unitInfo)
		return false;

	CvPlot* pMusterPlot = GetMusterPlot();
	CvPlot* pTargetPlot = GetTargetPlot();

	if (!pMusterPlot || !pTargetPlot)
		return false;

	bool bMustBeDeepWaterNaval = false;
	if(IsNavalOperation())
	{
		bMustBeDeepWaterNaval = pThisArmy->NeedOceanMoves();

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

	ReachablePlots turnsFromMuster;
	SPathFinderUserData data(m_eOwner,PT_GENERIC_REACHABLE_PLOTS,-1,GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY());
	turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);

	int iTurnDistance = INT_MAX;
	if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pUnit,pMusterPlot,turnsFromMuster,pTargetPlot,IsNavalOperation(),bMustBeDeepWaterNaval,iTurnDistance,thisFormation))
	{
		std::deque<OperationSlot>::iterator it;
		for(it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(it->m_iSlotID);
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(it->m_iSlotID);
			if (pSlot && pSlot->GetUnitID() == ARMYSLOT_NO_UNIT)
			{
				if (unitInfo->GetUnitAIType(thisSlotEntry.m_primaryUnitType) || unitInfo->GetUnitAIType(thisSlotEntry.m_secondaryUnitType))
				{
					//we have a match!
					pThisArmy->AddUnit(pUnit->GetID(), it->m_iSlotID);
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						if (pMusterPlot != NULL && pTargetPlot != NULL)
						{
							strMsg.Format("Recruited %s %d to fill in a new army at x=%d y=%d, target of x=%d y=%d", pUnit->getName().GetCString(), pUnit->GetID(), pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
							LogOperationSpecialMessage(strMsg);
						}
						if (iTurnDistance > GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT())
						{
							strMsg.Format("Warning: %s recruited far-away unit %d turns from muster point for a new army", GetOperationName(), iTurnDistance);
							LogOperationSpecialMessage(strMsg);
						}
					}

					m_viListOfUnitsWeStillNeedToBuild.erase(it);
					return true;
				}
			}
		}
	}

	return false;
}

bool CvAIOperation::GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvArmyAI* pArmy)
{
	if (!pMusterPlot || !pTargetPlot || pArmy == NULL)
		return false;

	//anything to do?
	if (m_viListOfUnitsWeStillNeedToBuild.empty())
		return true;

	bool rtnValue = true;
	bool success;
	std::deque<OperationSlot>::iterator it;
	CvString strMsg;

	//Every turn, expand our search until we fill this up (or abort).
	int iTurns = (GC.getGame().getGameTurn() - GetTurnStarted());
	iTurns += GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY();

	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(pArmy->GetFormationIndex());

	if (IsNavalOperation())
	{
		bool bMustBeDeepWaterNaval = pArmy->NeedOceanMoves();

		SPathFinderUserData data(m_eOwner,PT_GENERIC_SAME_AREA,m_eEnemy);
		data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
		if (GC.GetStepFinder().DoesPathExist(pMusterPlot, pTargetPlot, data))
		{
			ReachablePlots turnsFromMuster;
			SPathFinderUserData data(m_eOwner, PT_GENERIC_REACHABLE_PLOTS, -1, iTurns);
			turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);

			WeightedUnitIdVector UnitChoices;
			int iLoop = 0;
			for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
			{
				int iTurnDistance = INT_MAX;
				if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit,pMusterPlot,turnsFromMuster,pTargetPlot,true,bMustBeDeepWaterNaval,iTurnDistance,thisFormation,pArmy))
				{
					// When in doubt prefer units in our own territory
					if (pLoopUnit->plot() && pLoopUnit->plot()->getOwner() != m_eOwner)
						iTurnDistance++;

					UnitChoices.push_back(pLoopUnit->GetID(), 10000 + pLoopUnit->GetPower() - iTurnDistance*30);
				}
			}

			if (GC.getLogging() && GC.getAILogging())
			{
				if (pMusterPlot != NULL && pTargetPlot != NULL)
				{
					strMsg.Format("Found %d potential units to recruit for operation %d at x=%d y=%d, target of x=%d y=%d", 
						UnitChoices.size(), m_iID, pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
			}

			if (UnitChoices.size()==0 && pArmy->GetNumSlotsFilled() <= 0)
			{ 
				SetToAbort(AI_ABORT_NO_UNITS);
				return false;
			}

			UnitChoices.SortItems();

			// Copy over the list
			std::deque<OperationSlot> secondList = m_viListOfUnitsWeStillNeedToBuild;
			// Clear main list
			m_viListOfUnitsWeStillNeedToBuild.clear();
			for (it = secondList.begin(); it != secondList.end(); ++it)
			{
				success = FindBestFitReserveUnit(*it, UnitChoices);

				if (!success)
				{
					if (OperationalAIHelpers::IsSlotRequired(m_eOwner, *it))
					{
						// Return false to say that operation is not ready to roll yet
						rtnValue = false;

						// And add them back to the list of units needed
						m_viListOfUnitsWeStillNeedToBuild.push_back(*it);
					}
					else
					{
						CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(it->m_iArmyID);
						if (pArmy)
							pArmy->GetFormationSlot(it->m_iSlotID)->SetUnitID(ARMYSLOT_NOT_INCLUDING_IN_OPERATION);
					}
				}
			}
		}
		else
		{
			SetToAbort(AI_ABORT_LOST_PATH);
			return false;
		}
	}
	else //non-naval operation
	{
		ReachablePlots turnsFromMuster;
		SPathFinderUserData data(m_eOwner,PT_GENERIC_REACHABLE_PLOTS,-1,iTurns);
		turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMusterPlot, data);

		WeightedUnitIdVector UnitChoices;
		int iLoop = 0;
		for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
		{
			int iTurnDistance = INT_MAX;
			if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit,pMusterPlot,turnsFromMuster,pTargetPlot,false,false,iTurnDistance,thisFormation))
			{
				// When in doubt prefer units in our own territory
				if (pLoopUnit->plot() && pLoopUnit->plot()->getOwner() != m_eOwner)
					iTurnDistance++;

				UnitChoices.push_back(pLoopUnit->GetID(), 1000 - iTurnDistance*10 - pLoopUnit->GetPower());
			}
		}

		if (GC.getLogging() && GC.getAILogging())
		{
			if (pMusterPlot != NULL && pTargetPlot != NULL)
			{
				strMsg.Format("Found %d potential units to recruit for operation %d at x=%d y=%d, target of x=%d y=%d", 
					UnitChoices.size(), m_iID, pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
				LogOperationSpecialMessage(strMsg);
			}
		}

		if (UnitChoices.size()==0 && pArmy->GetNumSlotsFilled() <= 0)
		{ 
			SetToAbort(AI_ABORT_NO_UNITS);
			return false;
		}

		UnitChoices.SortItems();

		// Copy over the list
		std::deque<OperationSlot> secondList = m_viListOfUnitsWeStillNeedToBuild;
		// Clear main list
		m_viListOfUnitsWeStillNeedToBuild.clear();
		for (it = secondList.begin(); it != secondList.end(); ++it)
		{
			success = FindBestFitReserveUnit(*it, UnitChoices);
			if (!success)
			{
				if (OperationalAIHelpers::IsSlotRequired(m_eOwner, *it))
				{
					// Return false to say that operation is not ready to roll yet
					rtnValue = false;

					// And add them back to the list of units needed
					m_viListOfUnitsWeStillNeedToBuild.push_back(*it);
				}
				else
				{
					CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(it->m_iArmyID);
					if (pArmy)
						pArmy->GetFormationSlot(it->m_iSlotID)->SetUnitID(ARMYSLOT_NOT_INCLUDING_IN_OPERATION);
				}
			}
		}
	}

	return rtnValue;
}

CvPlot* CvAIOperation::GetPlotXInStepPath(CvPlot* pCurrentPosition, CvPlot* pTarget, int iStep, bool bForward) const
{
	if (!pCurrentPosition || !pTarget || iStep<0)
		return NULL;

	//check what kind of path we need
	PathType ePathType = PT_GENERIC_SAME_AREA;

	//large land armies need a wide path so they don't get stuck in difficult terrain
	//on the other hand they may theoretically switch areas, ie embark when moving to target
	if (!IsNavalOperation())
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(GetFormation());
		int iFormationSize = thisFormation ? thisFormation->getNumFormationSlotEntries() : 6;
		if (iFormationSize > 5 )
			ePathType = PT_GENERIC_ANY_AREA_WIDE;
		else
			ePathType = PT_GENERIC_ANY_AREA;
	}

	//make sure the start position is valid
	//target is handled automatically via approximate mode
	if (IsNavalOperation())
	{
		if (!pCurrentPosition->isWater())
			pCurrentPosition = MilitaryAIHelpers::GetCoastalPlotNearPlot(pCurrentPosition);

		if (!pCurrentPosition)
			return NULL;
	}

	// use the step path finder to get the path
	SPathFinderUserData data(m_eOwner, ePathType, m_eEnemy);
	data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
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

	//check what kind of path we need
	PathType ePathType = PT_GENERIC_SAME_AREA;

	//large land armies need a wide path so they don't get stuck in difficult terrain
	//on the other hand they may theoretically switch areas, ie embark when moving to target
	if (!IsNavalOperation())
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(GetFormation());
		int iFormationSize = thisFormation ? thisFormation->getNumFormationSlotEntries() : 6;
		if (iFormationSize > 5 )
			ePathType = PT_GENERIC_ANY_AREA_WIDE;
		else
			ePathType = PT_GENERIC_ANY_AREA;
	}

	//make sure the start position is valid
	//target is handled automatically via approximate mode
	if (IsNavalOperation())
	{
		if (!pCurrentPosition->isWater())
			pCurrentPosition = MilitaryAIHelpers::GetCoastalPlotNearPlot(pCurrentPosition);

		if (!pCurrentPosition)
			return -1;
	}

	// use the step path finder to compute distance
	SPathFinderUserData data(m_eOwner, ePathType, m_eEnemy);
	data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
	return GC.GetStepFinder().GetPathLengthInPlots(pCurrentPosition, pTarget, data);
}

/// Report percentage distance traveled from muster point to target (using army that is furthest along)
int CvAIOperation::PercentFromMusterPointToTarget()
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
			CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

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
	int iTurns = GC.getGame().getGameTurn() - GetTurnStarted();

	if (m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS && iTurns > GetMaximumRecruitTurns())
		SetToAbort(AI_ABORT_TIMED_OUT);

	if (m_eCurrentState == AI_OPERATION_STATE_MOVING_TO_TARGET && iTurns > GetMaximumRecruitTurns()*4)
		SetToAbort(AI_ABORT_TIMED_OUT);

	CvPlot* pMuster = GetMusterPlot();
	if (pMuster)
	{
		//the muster plot should be in friendly territory initially. if not, we are victims of a counterattack and should abort
		if (GET_PLAYER(m_eOwner).IsAtWarWith(pMuster->getOwner()))
			SetToAbort(AI_ABORT_TOO_DANGEROUS);

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
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
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
			CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

			CvUnit* pUnit = pArmy->GetFirstUnit();
			while(pUnit)
			{
				pUnit->SetDeployFromOperationTurn(GC.getGame().getGameTurn());
				pUnit = pArmy->GetNextUnit();
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

	GET_PLAYER(eOwner).deleteAIOperation(iID);
}

bool CvAIOperation::Move()
{
	if (m_viArmyIDs.empty())
		return false;

	//single army only for now
	CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
	if (!pThisArmy)
	{
		SetToAbort(AI_ABORT_NO_UNITS);
		return false;
	}

	pThisArmy->UpdateCheckpointTurns();
	pThisArmy->RemoveStuckAndWeakUnits();
	if (ShouldAbort())
	{
		LogOperationSpecialMessage( "operation aborted before move" );
		return false;
	}

	float fOldVarX,fOldVarY;
	CvPlot* pOldCOM = pThisArmy->GetCenterOfMass(false,&fOldVarX,&fOldVarY);

	//todo: move the relevant code from tactical AI to operations
	switch (GetMoveType())
	{
	case AI_OPERATION_MOVETYPE_ESCORT:
		GET_PLAYER(m_eOwner).GetTacticalAI()->PlotArmyMovesEscort(pThisArmy);
		break;
	case AI_OPERATION_MOVETYPE_COMBAT:
	case AI_OPERATION_MOVETYPE_COMBAT_ESCORT:
		GET_PLAYER(m_eOwner).GetTacticalAI()->PlotArmyMovesCombat(pThisArmy);
		break;
	default:
		return false;
	}

	//exclude rapid response ops etc
	if (pThisArmy->GetNumSlotsFilled()>3 && pThisArmy->GetArmyAIState()==ARMYAISTATE_MOVING_TO_DESTINATION)
	{
		float fNewVarX,fNewVarY;
		CvPlot* pNewCOM = pThisArmy->GetCenterOfMass(false,&fNewVarX,&fNewVarY);
		if (pNewCOM == pOldCOM && fNewVarX>=fOldVarX && fNewVarY>=fOldVarY)
		{
			OutputDebugString( CvString::format("Warning: Operation %d with army at (%d,%d) not making progress!\n",m_iID,pThisArmy->GetX(),pThisArmy->GetY()).c_str() );
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

	if (m_viArmyIDs.empty())
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
			// If recruiting units, read this unit to the list of what we need
			OperationSlot slotToFill;
			slotToFill.m_iOperationID = m_iID;
			slotToFill.m_iArmyID = iArmyID;
			slotToFill.m_iSlotID = iSlotID;
			m_viListOfUnitsWeStillNeedToBuild.push_back(slotToFill);
			break;
		}

		case AI_OPERATION_STATE_GATHERING_FORCES:
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
		case AI_OPERATION_STATE_AT_TARGET:
		{
			// If down below half strength, abort
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(iArmyID);
			CvMultiUnitFormationInfo* pkFormation = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
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
	CvPlayer &kPlayer = GET_PLAYER(m_eOwner);

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
			// Is goal a city and we're a naval operation?  If so, go just offshore.
			CvPlot *pGoalPlot = pArmy->GetGoalPlot();
			if (!pGoalPlot)
			{
				return NULL;
			}

			CvPlot* pCurrent = pArmy->Plot();
			if (pCurrent && pGoalPlot)
			{
				//problem: center of mass may be on a mountain etc ...
				if (!pCurrent->isValidMovePlot(kPlayer.GetID()))
				{
					CvUnit* pFirstUnit = pArmy->GetFirstUnit();
					if (pFirstUnit)
						pCurrent = pFirstUnit->plot();
					else
						return NULL;
				}

				//get where we want to be next
				pRtnValue = GetPlotXInStepPath(pCurrent,pGoalPlot,pArmy->GetMovementRate(),true);
				if (!pRtnValue)
				{
					// Can't plot a path, probably due to change of control of hexes.  Will probably abort the operation
					OutputDebugString( CvString::format( "CvAIOperation: cannot find a step path from %d,%d to %d,%d\n", 
						pCurrent->getX(), pCurrent->getY(), pGoalPlot->getX(), pGoalPlot->getY() ).c_str() );
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
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(pArmy->GetFormationIndex());
		if (!thisFormation)
			return false;
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisSlot.m_iSlotID);

		CvUnit* pUnit = GET_PLAYER(m_eOwner).GetMilitaryAI()->BuyEmergencyUnit(thisSlotEntry.m_primaryUnitType, pCity);
		if(pUnit != NULL)
		{
			pArmy->AddUnit(pUnit->GetID(), thisSlot.m_iSlotID);
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
	kStream >> m_eOwner;
	kStream >> m_eEnemy;
	kStream >> m_eCurrentState;
	kStream >> m_iTargetX;
	kStream >> m_iTargetY;
	kStream >> m_iMusterX;
	kStream >> m_iMusterY;
	kStream >> m_iLastTurnMoved;
	kStream >> m_viArmyIDs;
	kStream >> m_viListOfUnitsWeStillNeedToBuild;
	kStream >> m_viListOfUnitsCitiesHaveCommittedToBuild;
	kStream >> m_iTurnStarted;
	kStream >> m_iDistanceMusterToTarget;
}

/// Write serialized data
void CvAIOperation::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iID;
	kStream << m_eOwner;
	kStream << m_eEnemy;
	kStream << m_eCurrentState;
	kStream << m_iTargetX;
	kStream << m_iTargetY;
	kStream << m_iMusterX;
	kStream << m_iMusterY;
	kStream << m_iLastTurnMoved;
	kStream << m_viArmyIDs;
	kStream << m_viListOfUnitsWeStillNeedToBuild;
	kStream << m_viListOfUnitsCitiesHaveCommittedToBuild;
	kStream << m_iTurnStarted;
	kStream << m_iDistanceMusterToTarget;
}

#if defined(MOD_BALANCE_CORE)
const char* CvAIOperation::GetInfoString()
{
	CvString strTemp0, strTemp1, strTemp2, strTemp3;
	strTemp0 = GetOperationName();
	strTemp1.Format(" (%d) / Target at %d,%d / Muster at %d,%d / ", m_iID, m_iTargetX, m_iTargetY, m_iMusterX, m_iMusterY );

	switch(m_eCurrentState)
	{
	case INVALID_AI_OPERATION_STATE:
		strTemp2 = "Not initialized";
		break;
	case AI_OPERATION_STATE_ABORTED:
		strTemp2.Format("Aborted, %d", m_eAbortReason);
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
	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* thisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
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
void CvAIOperation::LogOperationStart()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp1;
		CvString strTemp2;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID());

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			strTemp1.Format("Started, Army: %d, Units Recruited: %d, Max Formation Size: %d, ", pThisArmy->GetID(), pThisArmy->GetNumSlotsFilled(), pThisArmy->GetNumFormationEntries());
		}

		strOutBuf = strBaseString + strTemp1;
		switch(m_eCurrentState)
		{
		case INVALID_AI_OPERATION_STATE:
			strTemp2 = "Not initialized";
			break;
		case AI_OPERATION_STATE_ABORTED:
			strTemp2.Format("Aborted, %d", m_eAbortReason);
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

		// Message to summary log?
		switch (GetOperationType())
		{
		case AI_OPERATION_CITY_BASIC_ATTACK:
		case AI_OPERATION_PILLAGE_ENEMY:
		case AI_OPERATION_CITY_SNEAK_ATTACK:
		case AI_OPERATION_NAVAL_BOMBARDMENT:
		case AI_OPERATION_NAVAL_SUPERIORITY:
		case AI_OPERATION_NAVAL_INVASION:
		case AI_OPERATION_NAVAL_INVASION_SNEAKY:
		case AI_OPERATION_CITY_STATE_ATTACK:
		case AI_OPERATION_NAVAL_INVASION_CITY_STATE:
		case AI_OPERATION_NUKE_ATTACK:
		case AI_OPERATION_NAVAL_ONLY_CITY_ATTACK:
		case AI_OPERATION_DIPLOMAT_DELEGATION:
		case AI_OPERATION_ALLY_DEFENSE:
			
			strOutBuf = CvString( GetOperationName() ) + ", ";
			strOutBuf += strTemp1 + strTemp2;
			if (m_eEnemy != NO_PLAYER)
			{
				strPlayerName = GET_PLAYER(m_eEnemy).getCivilizationShortDescription();
				strOutBuf += ", vs. " + strPlayerName;
			}
			LogOperationSpecialMessage(strOutBuf);
		}
	}
}

/// Log current status of the operation
void CvAIOperation::LogOperationStatus(bool bPreTurn)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2, szTemp3;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, %s ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID(), bPreTurn ? "PRE" : "POST");

		switch(m_eCurrentState)
		{
		case INVALID_AI_OPERATION_STATE:
			strTemp = "Not initialized";
			break;
		case AI_OPERATION_STATE_ABORTED:
			strTemp.Format("Aborted: %d", m_eAbortReason);
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Recruiting Units, Army: %d, ", pThisArmy->GetID());
				strTemp += szTemp2;

				for(int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(iI);
					if(pSlot->GetUnitID() == ARMYSLOT_NO_UNIT)
					{
						szTemp2 = "Open, ";
					}
					else if(pSlot->GetUnitID() == ARMYSLOT_NOT_INCLUDING_IN_OPERATION)
					{
						szTemp2 = "Skipping, ";
					}
					else
					{
						CvUnit* pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("%s %d at (%d:%d) - ETA %d, ", 
								pThisUnit->getName().GetCString(), pThisUnit->GetID(), pThisUnit->getX(), pThisUnit->getY(), pSlot->GetTurnAtCheckpoint());
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
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				//we don't really need the center of mass but the variance
				float varX=-1,varY=-1;
				pThisArmy->GetCenterOfMass(false,&varX,&varY);
				szTemp2.Format("Gathering Forces, Army: %d, Gather (%d:%d), variance (%.2f:%.2f), ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY(), varX, varY);
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while(iUnitID != ARMYSLOT_NO_UNIT)
				{
					// do something with each entry
					CvUnit* pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if(pThisUnit)
					{
						szTemp2.Format("%s %d at (%d:%d),", pThisUnit->getName().GetCString(), pThisUnit->GetID(), pThisUnit->getX(), pThisUnit->getY());
						strTemp += szTemp2;
					}
					iUnitID = pThisArmy->GetNextUnitID();
				}
			}
			break;
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Moving To Target, Army: %d, At (%d:%d), To (%d:%d), ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY(), m_iTargetX, m_iTargetY);
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while(iUnitID != ARMYSLOT_NO_UNIT)
				{
					// do something with each entry
					CvUnit* pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if(pThisUnit)
					{
						szTemp2.Format("%s %d at (%d:%d), ", pThisUnit->getName().GetCString(), pThisUnit->GetID(), pThisUnit->getX(), pThisUnit->getY());
						strTemp += szTemp2;
					}
					iUnitID = pThisArmy->GetNextUnitID();
				}
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

void CvAIOperation::LogOperationSpecialMessage(const CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID());
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// Log that an operation has ended
void CvAIOperation::LogOperationEnd()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName(), GetID() );

		strTemp = "Ended: ";

		switch(m_eAbortReason)
		{
		case AI_ABORT_SUCCESS:
			strTemp += "Success";
			break;
		case AI_ABORT_NO_TARGET:
			strTemp += "No target";
			break;
		case AI_ABORT_CANCELLED:
			strTemp += "Cancelled";
			break;
		case AI_ABORT_LOST_TARGET:
			strTemp += "Lost target";
			break;
		case AI_ABORT_TARGET_ALREADY_CAPTURED:
			strTemp += "Target already captured";
			break;
		case AI_ABORT_NO_ROOM_DEPLOY:
			strTemp += "No room to deploy";
			break;
		case AI_ABORT_HALF_STRENGTH:
			strTemp += "Half strength";
			break;
		case AI_ABORT_NO_MUSTER:
			strTemp += "No muster point";
			break;
		case AI_ABORT_LOST_CIVILIAN:
			strTemp += "Lost civilian";
			break;
		case AI_ABORT_ESCORT_DIED:
			strTemp += "Escort died";
			break;
		case AI_ABORT_TOO_DANGEROUS:
			strTemp += "Too dangerous";
			break;
		case AI_ABORT_KILLED:
			strTemp += "Killed";
			break;
		case AI_ABORT_WAR_STATE_CHANGE:
			strTemp += "War State Change";
			break;
		case AI_ABORT_DIPLO_OPINION_CHANGE:
			strTemp += "Diplo Opinion Change";
			break;
		case AI_ABORT_LOST_PATH:
			strTemp += "Lost Path to Target";
			break;
		case AI_ABORT_TIMED_OUT:
			strTemp += "Timed Out";
			break;
		case AI_ABORT_NO_UNITS:
			strTemp += "No Units";
			break;
		default:
			strTemp += "Unknown reason";
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		// Message to summary log?
		switch (GetOperationType())
		{
		case AI_OPERATION_CITY_BASIC_ATTACK:
		case AI_OPERATION_PILLAGE_ENEMY:
		case AI_OPERATION_CITY_SNEAK_ATTACK:
		case AI_OPERATION_NAVAL_BOMBARDMENT:
		case AI_OPERATION_NAVAL_SUPERIORITY:
		case AI_OPERATION_NAVAL_INVASION:
		case AI_OPERATION_NAVAL_INVASION_SNEAKY:
		case AI_OPERATION_CITY_STATE_ATTACK:
		case AI_OPERATION_NAVAL_INVASION_CITY_STATE:
		case AI_OPERATION_NUKE_ATTACK:
		case AI_OPERATION_NAVAL_ONLY_CITY_ATTACK:
		case AI_OPERATION_DIPLOMAT_DELEGATION:
		case AI_OPERATION_ALLY_DEFENSE:

			strOutBuf = CvString( GetOperationName() ) + ", ";
			strOutBuf += strTemp;
			LogOperationSpecialMessage(strOutBuf);
		}
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

bool CvAIOperation::SetupWithSingleArmy(CvPlot * pMusterPlot, CvPlot * pTargetPlot, CvPlot * pDeployPlot, CvUnit* pInitialUnit, bool bOceanMoves)
{
	//pDeployPlot may be null ...
	if (!pMusterPlot || !pTargetPlot)
	{
		if (GC.getLogging() && GC.getAILogging())
			LogOperationSpecialMessage("Cannot set up operation - no target or no muster!");

		SetToAbort( pTargetPlot ? AI_ABORT_NO_MUSTER : AI_ABORT_NO_TARGET );
		return false;
	}

	CvArmyAI* pArmyAI = AddArmy();
	if(!pArmyAI)
		return false;

	//this is for the operation
	LogOperationStart();
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
		pArmyAI->AddUnit(pInitialUnit->GetID(),0);

	SetTurnStarted(GC.getGame().getGameTurn());
	pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
	m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
	LogOperationSpecialMessage("Initial stage is recruiting");

	// Find the list of units we need to build before starting this operation in earnest
	BuildListOfUnitsWeStillNeedToBuild();

	//see if we can switch to gathering stage
	CheckTransitionToNextStage();
	//maybe we can even switch to movement stage
	CheckTransitionToNextStage();

	//do it again to see whether recruitment was successful
	LogOperationStart();
	return true;
}

CvArmyAI * CvAIOperation::AddArmy()
{
	CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
	if (!pArmyAI)
		return NULL;

	m_viArmyIDs.push_back(pArmyAI->GetID());
	pArmyAI->SetOperationID(m_iID);
	pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
	pArmyAI->SetDomainType(IsNavalOperation()?DOMAIN_SEA:DOMAIN_LAND);
	pArmyAI->SetFormationIndex(GetFormation());
	return pArmyAI;
}

/// Find a unit from our reserves that could serve in this operation
bool CvAIOperation::FindBestFitReserveUnit(OperationSlot thisOperationSlot, WeightedUnitIdVector& UnitChoices)
{
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;
	if(!pThisArmy)
		return false;

	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if(iThisFormationIndex == NO_MUFORMATION)
		return false;

	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
	if (!thisFormation)
		return false;

	CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(thisOperationSlot.m_iSlotID);
	if (!pSlot)
		return false;

	const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
	if (pSlot->GetUnitID() > ARMYSLOT_NO_UNIT)
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

	for (int iI=0; iI<UnitChoices.size(); iI++)
	{
		CvUnit* pBestUnit = GET_PLAYER(m_eOwner).getUnit(UnitChoices.GetElement(iI));
		if (pBestUnit != NULL)
		{
			CvUnitEntry* unitInfo = GC.getUnitInfo(pBestUnit->getUnitType());
			if (unitInfo->GetUnitAIType(thisSlotEntry.m_primaryUnitType) || unitInfo->GetUnitAIType(thisSlotEntry.m_secondaryUnitType))
			{
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				/*
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Recruiting - found suitable unit %s %d at index %d", pBestUnit->getName().GetCString(), pBestUnit->GetID(), iI );
					LogOperationSpecialMessage(strMsg);
				}
				*/

				//overwrite with invalid ID so we don't use it a second time
				UnitChoices.SetElement(iI,-1);

				return true;
			}
		}
	}

	return false;
}

FDataStream& operator<<(FDataStream& saveTo, const AIOperationMovementType& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, AIOperationMovementType& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIOperationMovementType>(v);
	return loadFrom;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperation
////////////////////////////////////////////////////////////////////////////////

/// How long will we wait for a recruit to show up?
int CvAIOperationOffensive::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY();
}

AIOperationAbortReason CvAIOperationOffensive::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	CvCity* pTroubleSpot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity(false);
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
	if (GetTargetPlot()->getOwner()!=m_eEnemy)
		return AI_ABORT_TARGET_ALREADY_CAPTURED;

	return NO_ABORT_REASON;
}

/// Kick off this operation
void CvAIOperationOffensive::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if (!pTarget)
		return;

	if (!pMuster)
		pMuster = GET_PLAYER(eOwner).GetClosestCity(pTarget->plot(),23,true);

	if (!pMuster)
		return;

	SetupWithSingleArmy(pMuster->plot(),pTarget->plot());
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityBasicAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCityBasicAttack::CvAIOperationCityBasicAttack()
{
}

/// Destructor
CvAIOperationCityBasicAttack::~CvAIOperationCityBasicAttack()
{
}

/// Kick off this operation
void CvAIOperationCityBasicAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if (!pMuster || !pTarget)
		return;

	SetupWithSingleArmy(pMuster->plot(),pTarget->plot());
}

MultiunitFormationTypes CvAIOperationCityBasicAttack::GetFormation() const
{
	//varies with era
	return MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack();
}

/// Allows an outside class to terminate the operation
void CvAIOperationMilitary::SetToAbort(AIOperationAbortReason eReason)
{
#if defined(MOD_BALANCE_CORE)
	if(eReason == AI_ABORT_LOST_TARGET || eReason == AI_ABORT_NO_ROOM_DEPLOY || eReason == AI_ABORT_NO_MUSTER || eReason == AI_ABORT_LOST_PATH)
	{
		/// Clear cached targets so we don't do this over and over.
		GET_PLAYER(GetOwner()).GetMilitaryAI()->ClearCachedTargets();
	}
#endif

	CvAIOperation::SetToAbort(eReason);
}

bool CvAIOperationMilitary::CheckTransitionToNextStage()
{
	if (m_viArmyIDs.empty())
		return false;

	//only a single army right now!
	CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
	if(!pThisArmy)
		return false;

	bool bStateChanged = false;
	switch( pThisArmy->GetArmyAIState() )
	{
		case ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE:
		{
			if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot(), pThisArmy))
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
			if(GetMusterPlot() != NULL)
			{
				int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
				if(pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= iGatherTolerance)
				{
					pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
					m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					LogOperationSpecialMessage("Transition to movement stage");
					bStateChanged = true;
				}
			}
			break;
		}
		case ARMYAISTATE_MOVING_TO_DESTINATION:
		{
			//check if we're at the target
			CvPlot *pTarget = pThisArmy->GetGoalPlot();
			CvPlot *pCenterOfMass = pThisArmy->GetCenterOfMass();
			if(pCenterOfMass && pTarget && IsOffensive())
			{
				bool bInPlace = false;
				if(plotDistance(*pCenterOfMass,*pTarget) <= GetDeployRange())
				{
					bInPlace = true;
				}

				//check for nearby enemy (for sneak attacks)
				if (!bInPlace && (GetOperationType()==AI_OPERATION_NAVAL_INVASION_SNEAKY || GetOperationType()==AI_OPERATION_CITY_SNEAK_ATTACK) ) 
				{
					int nVisible = 0;
					for (CvUnit* pUnit = pThisArmy->GetFirstUnit(); pUnit; pUnit = pThisArmy->GetNextUnit())
					{
						if (pUnit->plot()->isVisible( GET_PLAYER(m_eEnemy).getTeam() ))
							nVisible++;
					}

					//the jig is up
					if (nVisible>2)
					{
						bInPlace = true;

						CvPlot *pOtherTarget = NULL;
						for (int i=0; i<RING3_PLOTS; i++)
						{
							CvPlot* pTestPlot = iterateRingPlots( pCenterOfMass, i );
							if (pTestPlot && pTestPlot->getOwner()==m_eEnemy && pTestPlot->getWorkingCity())
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
					// Notify Diplo AI we're in place for attack
					if(!GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
					{
						GET_PLAYER(m_eOwner).GetDiplomacyAI()->SetMusteringForAttack(m_eEnemy, true);
					}

					// Notify tactical AI to focus on this area
					if (GetTargetType()!=AI_TACTICAL_TARGET_NONE)
					{
						CvTemporaryZone zone;
						if(pTarget->getWorkingCity() != NULL && pTarget->getWorkingCity()->getOwner() == m_eEnemy)
						{
							zone.SetX(pTarget->getWorkingCity()->getX());
							zone.SetY(pTarget->getWorkingCity()->getY());
						}
						else
						{
							zone.SetX(pTarget->getX());
							zone.SetY(pTarget->getY());
						}

						zone.SetTargetType( GetTargetType() );
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						if (IsNavalOperation() && !IsCivilianOperation())
						{
							zone.SetNavalInvasion(true);
						}
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}

					//that's it. skip STATE_AT_TARGET so the army will be disbanded next turn!
					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
					pThisArmy->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);
					LogOperationSpecialMessage("Transition to finished stage");

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

/// Constructor
CvAIOperationAntiBarbarian::CvAIOperationAntiBarbarian()
{
	m_iUnitToRescue = -1;
}

/// Destructor
CvAIOperationAntiBarbarian::~CvAIOperationAntiBarbarian()
{
}

void CvAIOperationAntiBarbarian::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int /*iAreaID*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,BARBARIAN_PLAYER);

	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster,pTarget);
}

/// How close to target do we end up?
int CvAIOperationAntiBarbarian::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE();
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
	return OperationalAIHelpers::FindBestBarbCamp(m_eOwner,ppMuster);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationPillageEnemy
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationPillageEnemy::CvAIOperationPillageEnemy()
{
}

/// Destructor
CvAIOperationPillageEnemy::~CvAIOperationPillageEnemy()
{
}

void CvAIOperationPillageEnemy::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity*, CvCity*, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster,pTarget);
}

/// How close to target do we end up?
int CvAIOperationPillageEnemy::GetDeployRange() const
{
	return 1;
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
	int iValue;
	int iBestValue;
	CvCity* pLoopCity;
	int iDistance;
	int iLoop;

	CvPlayerAI& kEnemyPlayer = GET_PLAYER(m_eEnemy);

	if(!kEnemyPlayer.isAlive())
	{
		return NULL;
	}

	iBestValue = 0;
	for(pLoopCity = kEnemyPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kEnemyPlayer.nextCity(&iLoop))
	{
		// Make sure city is in the same area as our potential muster point
		CvCity* pClosestCity = GET_PLAYER(m_eOwner).GetClosestCityByEstimatedTurns(pLoopCity->plot());
		if(pClosestCity && pLoopCity->getArea() == pClosestCity->getArea())
		{
			// Initial value of target is the number of improved plots
			iValue = pLoopCity->countNumImprovedPlots();

			// Adjust value based on proximity to our start location
			iDistance = GET_PLAYER(m_eOwner).GetCityDistanceInEstimatedTurns(pLoopCity->plot());
			if(iDistance > 0)
			{
				iValue = iValue * 100 / iDistance;
			}

			if(iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestTargetCity = pLoopCity;
			}
		}
	}

	if (pBestTargetCity == NULL)
		pBestTargetCity = GET_PLAYER(m_eEnemy).getCapitalCity();

	if (ppMuster)
	{
		CvCity *pClosest = pBestTargetCity ? GET_PLAYER(m_eOwner).GetClosestCityByEstimatedTurns(pBestTargetCity->plot()) : NULL;
		*ppMuster = pClosest ? pClosest->plot() : NULL;
	}

	return pBestTargetCity ? pBestTargetCity->plot() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilian
////////////////////////////////////////////////////////////////////////////////
CvAIOperationCivilian::CvAIOperationCivilian()
{
}

CvAIOperationCivilian::~CvAIOperationCivilian()
{
}

/// Kick off this operation
void CvAIOperationCivilian::Init(int iID, PlayerTypes eOwner, PlayerTypes /* eEnemy */, int iAreaID, CvCity* /*pTarget*/, CvCity* /*pMuster*/, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,NO_PLAYER);

	CvUnit* pOurCivilian = FindBestCivilian();
	if (!pOurCivilian)
	{
		SetToAbort(AI_ABORT_NO_UNITS);
		return;
	}

	CvPlot* pTargetSite = FindBestTargetForUnit(pOurCivilian,iAreaID,!IsEscorted());

	CvPlot* pMusterPlot = pOurCivilian->plot();
	//don't wait for the escort in the wild (happens with settlers a lot)
	if (IsEscorted() && !pMusterPlot->IsFriendlyTerritory(eOwner))
	{
		CvCity* pClosestCity = GET_PLAYER(eOwner).GetClosestCityByEstimatedTurns(pOurCivilian->plot());
		if (pClosestCity)
			pMusterPlot = pClosestCity->plot();
	}

	//problem: naval op but civilian not in a coastal city
	if (IsNavalOperation() && !(pMusterPlot->isCity() && pMusterPlot->getPlotCity()->isCoastal()))
	{
		CvCity* pMusterCity = OperationalAIHelpers::GetNearestCoastalCityFriendly(eOwner, pMusterPlot);
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
			if (pLoopPlot == NULL || pLoopPlot->getOwner() != eOwner) 
				continue;

			//No water and not impassable
			if(IsNavalOperation() && !(pLoopPlot->isWater() && pLoopPlot->area()->getNumTiles()>GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				continue;

			if(!IsNavalOperation() && pLoopPlot->isWater())
				continue;

			if(pLoopPlot->isImpassable())
				continue;

			//Empty
			if(pLoopPlot->getNumDefenders(eOwner) > 0)
				continue;

			//Not dangerous
			if (pOurCivilian->GetDanger(pLoopPlot)==0)
			{
				pMusterPlot = pLoopPlot;
				break;
			}
		}
	}

	SetupWithSingleArmy(pMusterPlot,pTargetSite,pTargetSite,pOurCivilian);
}

bool CvAIOperationCivilian::CheckTransitionToNextStage()
{
	if (m_viArmyIDs.empty())
		return false;

	//only a single army right now!
	CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
	if(!pThisArmy)
		return false;

	//we look at the army's state. the operation state is really not required
	bool bStateChanged = false;
	switch( pThisArmy->GetArmyAIState() )
	{
		case ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE:
		{
			if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot(), pThisArmy))
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
			CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetFirstUnitID());
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
			CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetFirstUnitID());
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
			CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetFirstUnitID());

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
	CvAIOperation::UnitWasRemoved(iArmyID,iSlotID);

	//the civilian slot is special - without it there's nothing we can do
	if(iSlotID == 0)
		SetToAbort( AI_ABORT_LOST_CIVILIAN );
}

/// Find the civilian we want to use
CvUnit* CvAIOperationCivilian::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == GetCivilianType())
			{
				if(pLoopUnit->getArmyID() == -1)
				{
					return pLoopUnit;
				}
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
	CvPlot* pBetterTarget = FindBestTargetForUnit(pCivilian,pCurrentTarget?pCurrentTarget->getArea():-1,!IsEscorted());

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

/// Constructor
CvAIOperationCivilianFoundCity::CvAIOperationCivilianFoundCity()
{
}

/// Destructor
CvAIOperationCivilianFoundCity::~CvAIOperationCivilianFoundCity()
{
}

bool CvAIOperationCivilianFoundCity::PerformMission(CvUnit* pSettler)
{
	// If the settler made it, we don't care about the entire army
	CvPlot* pCityPlot = GetTargetPlot();
	if(pSettler && pSettler->canFound(pCityPlot) && pSettler->plot() == pCityPlot && pSettler->canMove())
	{
		pSettler->PushMission(CvTypes::getMISSION_FOUND());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvArea* pArea = pCityPlot->area();
			CvCity* pCity = pCityPlot->getPlotCity();

			if (pCity != NULL)
			{
				CvString strMsg;
				strMsg.Format("City founded (%s), At X=%d, At Y=%d, plot value %d, area value %d", pCity->getName().c_str(), 
					pCityPlot->getX(), pCityPlot->getY(), pCityPlot->getFoundValue(m_eOwner), pArea->getTotalFoundValue());
				LogOperationSpecialMessage(strMsg);
			}
		}

		// Notify tactical AI to focus on this area
		CvTemporaryZone zone;
		zone.SetX(pCityPlot->getX());
		zone.SetY(pCityPlot->getY());
		zone.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
		zone.SetLastTurn(GC.getGame().getGameTurn() + (GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() * 2));
		GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

		return true;
	}

	return false;
}

AIOperationAbortReason CvAIOperationCivilianFoundCity::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	CvUnit* pSettler = pArmy->GetFirstUnit();
	if (!pSettler)
		return AI_ABORT_NO_UNITS;

	bool bCanFound = pSettler->canFound(GetTargetPlot());
	bool bHavePath = pSettler->GeneratePath(GetTargetPlot(),CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY|CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED);

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
		CvPlot* pBetterTarget = FindBestTargetIncludingCurrent(pSettler, !IsEscorted());

		// No targets at all!
		if(pBetterTarget == NULL)
		{
			LogOperationSpecialMessage( CvString::format("no valid city site target found (%s)!", IsEscorted() ? "with escort" : "no escort").c_str() );
			return AI_ABORT_NO_TARGET;
		}

		// If we have a better target, switch
		if (pBetterTarget != GetTargetPlot())
		{
			int iOldPlotValue = GetTargetPlot()->getFoundValue(m_eOwner);
			int iNewPlotValue = pBetterTarget->getFoundValue(m_eOwner);

			if (plotDistance(*GetTargetPlot(),*pBetterTarget)<2 || iNewPlotValue>iOldPlotValue*1.2f)
			{
				SetTargetPlot(pBetterTarget);
				pArmy->SetGoalPlot(pBetterTarget);
			}
		}

		return NO_ABORT_REASON;
	}
}

/// Find the plot where we want to settle
CvPlot* CvAIOperationCivilianFoundCity::FindBestTargetIncludingCurrent(CvUnit* pUnit, bool bOnlySafeTargets)
{
	//todo: better options
	//a) return a list of possible targets and find the ones that are currently reachable
	//b) if the best target is unreachable, move in the general direction and hope the block will clear up

	bool bIsSafe; //dummy
	int iTargetArea = GetTargetPlot() ? GetTargetPlot()->getArea() : -1;
	//ignore the current operation target when searching. default would be to suppress currently targeted plots
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, iTargetArea, bOnlySafeTargets, bIsSafe, this);

	//try again if the result is not good
	if (pResult == NULL && iTargetArea != -1)
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, -1, bOnlySafeTargets, bIsSafe, this);

	return pResult;
}

//need to have this, it's pure virtual in civilian operation
CvPlot* CvAIOperationCivilianFoundCity::FindBestTargetForUnit(CvUnit* pUnit, int iAreaID, bool bOnlySafeTargets)
{
	bool bIsSafe; //dummy
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, iAreaID, bOnlySafeTargets, bIsSafe);

	//try again if the result is not good
	if (pResult == NULL && iAreaID != -1 )
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, -1, bOnlySafeTargets, bIsSafe);

	return pResult;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianFoundCityQuick
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCivilianFoundCityQuick::CvAIOperationCivilianFoundCityQuick()
{
}

/// Destructor
CvAIOperationCivilianFoundCityQuick::~CvAIOperationCivilianFoundCityQuick()
{
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianMerchantDelegation
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCivilianMerchantDelegation::CvAIOperationCivilianMerchantDelegation()
{
}

/// Destructor
CvAIOperationCivilianMerchantDelegation::~CvAIOperationCivilianMerchantDelegation()
{
}

/// If at target, cash in; if at muster point, merge merchant and escort and move out
bool CvAIOperationCivilianMerchantDelegation::PerformMission(CvUnit* pMerchant)
{
	// If the merchant made it, we don't care about the entire army
	if(pMerchant && pMerchant->plot()->getOwner() == GetTargetPlot()->getOwner() && pMerchant->canMove() && pMerchant->canTrade(pMerchant->plot()))
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

	return false;
}

/// Find the plot where we want to settler
CvPlot* CvAIOperationCivilianMerchantDelegation::FindBestTargetForUnit(CvUnit* pUnit, int /*iAreaID*/, bool /*bOnlySafeTargets*/)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlot(pUnit);
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianDiplomatDelegation
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCivilianDiplomatDelegation::CvAIOperationCivilianDiplomatDelegation()
{
}

/// Destructor
CvAIOperationCivilianDiplomatDelegation::~CvAIOperationCivilianDiplomatDelegation()
{
}

/// Find the plot where we want to influence
CvPlot* CvAIOperationCivilianDiplomatDelegation::FindBestTargetForUnit(CvUnit* pUnit, int /*iAreaID*/, bool /*bOnlySafeTargets*/)
{
	if(!pUnit)
		return NULL;

	//this mission is for diplomacy bomb, constructing embassies is handled in homeland AI
	return GET_PLAYER(pUnit->getOwner()).ChooseMessengerTargetPlot(pUnit);
}

bool CvAIOperationCivilianDiplomatDelegation::PerformMission(CvUnit* pDiplomat)
{
	if(pDiplomat && pDiplomat->plot() == GetTargetPlot() && pDiplomat->canMove() && pDiplomat->canTrade(pDiplomat->plot()))
	{
		pDiplomat->PushMission(CvTypes::getMISSION_TRADE());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Great Diplomat finishing Diplomatic Mission at %s", pDiplomat->plot()->GetAdjacentCity()->getName().c_str());
			LogOperationSpecialMessage(strMsg);
		}
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDefendAlly - 
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationDefendAlly::CvAIOperationDefendAlly()
{
}

/// Destructor
CvAIOperationDefendAlly::~CvAIOperationDefendAlly()
{
}

/// Kick off this operation
void CvAIOperationDefendAlly::Init(int iID, PlayerTypes eOwner, PlayerTypes eAlly, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eAlly);

	CvPlot* pMusterPlot;
	CvPlot* pTargetPlot;
	if (pTarget && pMuster)
	{
		pMusterPlot = pMuster->plot();
		pTargetPlot = pTarget->plot();
	}
	else
		pTargetPlot = FindBestTarget(&pMusterPlot);

	SetupWithSingleArmy(pMusterPlot,pTargetPlot);
}

AIOperationAbortReason CvAIOperationDefendAlly::VerifyOrAdjustTarget(CvArmyAI* /*pArmy*/)
{
	// See if our target city is still owned by our ally (which is stored in m_eEnemy)
	if(GetTargetPlot()->getOwner() != m_eEnemy)
	{
		// Bummer! The city has been captured/destroyed
		return AI_ABORT_TARGET_ALREADY_CAPTURED;
	}
	else if(GET_PLAYER(m_eEnemy).isMinorCiv())
	{
		// Quest is over?
		if( !GET_PLAYER(m_eEnemy).GetMinorCivAI()->IsActiveQuestForPlayer(m_eOwner, MINOR_CIV_QUEST_HORDE) && 
			!GET_PLAYER(m_eEnemy).GetMinorCivAI()->IsActiveQuestForPlayer(m_eOwner, MINOR_CIV_QUEST_REBELLION))
		{
			return AI_ABORT_CANCELLED;
		}
	}

	return NO_ABORT_REASON;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianConcertTour
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCivilianConcertTour::CvAIOperationCivilianConcertTour()
{
}

/// Destructor
CvAIOperationCivilianConcertTour::~CvAIOperationCivilianConcertTour()
{
}

/// Find the plot where we want to settler
CvPlot* CvAIOperationCivilianConcertTour::FindBestTargetForUnit(CvUnit* pUnit, int /*iAreaID*/, bool /*bOnlySafeTargets*/)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).FindBestMusicianTargetPlot(pUnit);
}

bool CvAIOperationCivilianConcertTour::PerformMission(CvUnit* pMusician)
{
	if(pMusician && pMusician->plot() == GetTargetPlot() && pMusician->canMove() && pMusician->canBlastTourism(pMusician->plot()))
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

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalOnlyBasic
////////////////////////////////////////////////////////////////////////////////

/// Kick off this operation
void CvAIOperationNavalOnlyBasic::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if (!pMuster || !pTarget)
		return;

	//this is where we gather the army
	CvPlot* pMusterPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMuster->plot());
	//this is where the army should go
	CvPlot* pGoalPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTarget->plot());
	if (!pMusterPlot || !pGoalPlot)
		return;

	SetupWithSingleArmy(pMusterPlot,pTarget->plot(),pGoalPlot, NULL, bOceanMoves);
}
MultiunitFormationTypes CvAIOperationNavalOnlyBasic::GetFormation() const
{
	//varies with era
	return MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack();
}
/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
AIOperationAbortReason CvAIOperationNavalOnlyBasic::VerifyOrAdjustTarget(CvArmyAI* pArmy)
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


////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalBombardment
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNavalBombardment::CvAIOperationNavalBombardment()
{
}

/// Destructor
CvAIOperationNavalBombardment::~CvAIOperationNavalBombardment()
{
}

/// Kick off this operation
void CvAIOperationNavalBombardment::Init(int iID, PlayerTypes eOwner, PlayerTypes eAlly, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	//do this before calling any FindX methods!
	Reset(iID, eOwner, eAlly);

	if (!pMuster || !pTarget)
		return;

	//this is where we gather the army
	CvPlot* pMusterPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMuster->plot());
	//this is where the army should go
	CvPlot* pGoalPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTarget->plot());
	if (!pMusterPlot || !pGoalPlot)
		return;

	CvPlot* pTargetPlot = FindBestTarget(&pMusterPlot);
	if (pTargetPlot == NULL)
	{
		pTargetPlot = pGoalPlot;
	}
	SetupWithSingleArmy(pMusterPlot, pTargetPlot, NULL, NULL, bOceanMoves);
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
AIOperationAbortReason CvAIOperationNavalBombardment::VerifyOrAdjustTarget(CvArmyAI* pArmy)
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
/// Find the barbarian camp we want to eliminate
CvPlot* CvAIOperationNavalBombardment::FindBestTarget(CvPlot** ppMuster) const
{
	CvPlot* pRefPlot = GetTargetPlot();
	if (pRefPlot)
	{
		CvPlot* pTarget = OperationalAIHelpers::FindEnemiesNearPlot(m_eOwner, m_eEnemy, DOMAIN_SEA, false, pRefPlot->getArea(), pRefPlot);
		if (ppMuster)
			*ppMuster = pTarget;
		return pTarget;
	}

	return NULL;
	//return OperationalAIHelpers::FindBestCoastalBombardmentTarget(m_eOwner,m_eEnemy,ppMuster);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalSuperiority
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNavalSuperiority::CvAIOperationNavalSuperiority()
{
}

/// Destructor
CvAIOperationNavalSuperiority::~CvAIOperationNavalSuperiority()
{
}

/// Kick off this operation
void CvAIOperationNavalSuperiority::Init(int iID, PlayerTypes eOwner, PlayerTypes eAlly, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	//do this before calling any FindX methods!
	Reset(iID, eOwner, eAlly);

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
	CvCity* pTargetCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;

	// Defend the city most under threat
	pTargetCity = GET_PLAYER(m_eOwner).GetThreatenedCityByRank();

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if (pTargetCity == NULL || !pTargetCity->isCoastal())
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if (pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
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
// CvAIOperationNavalOnlyCityAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNavalOnlyCityAttack::CvAIOperationNavalOnlyCityAttack()
{
}

/// Destructor
CvAIOperationNavalOnlyCityAttack::~CvAIOperationNavalOnlyCityAttack()
{
}

/// Kick off this operation
void CvAIOperationNavalOnlyCityAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMusterPlot = NULL;
	CvPlot* pTargetPlot = NULL;

	if(pMuster == NULL || !pMuster->isCoastal())
	{
		CvCity* pNearestCoastalCity = OperationalAIHelpers::GetNearestCoastalCityFriendly(m_eOwner,eEnemy);
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
		CvCity* pNearestCoastalCity = OperationalAIHelpers::GetNearestCoastalCityEnemy(m_eOwner,eEnemy);
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

/// Constructor
CvAIOperationDefendCityPeace::CvAIOperationDefendCityPeace()
{
}

/// Destructor
CvAIOperationDefendCityPeace::~CvAIOperationDefendCityPeace()
{
}

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationDefendCityPeace::FindBestTarget(CvPlot** ppMuster) const
{
	CvCity* pTargetCity;
	CvPlot* pPlot = NULL;

	// Defend the city most under threat
	pTargetCity = GET_PLAYER(m_eOwner).GetThreatenedCityByRank();

	if(pTargetCity != NULL)
	{
		pPlot = pTargetCity->plot();

		if (ppMuster)
		{
			CvCity* pMusterCity = GET_PLAYER(m_eOwner).GetClosestCity(pPlot,23,true);
			if (!pMusterCity)
				pMusterCity = pTargetCity;
			*ppMuster = pMusterCity->plot();
		}
	}

	return pPlot;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDefendCity - Place holder
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationDefendCity::CvAIOperationDefendCity()
{
}

/// Destructor
CvAIOperationDefendCity::~CvAIOperationDefendCity()
{
}

/// Kick off this operation
void CvAIOperationDefendCity::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMusterPlot;
	CvPlot* pTargetPlot;
	if (pTarget && pMuster)
	{
		pMusterPlot = pMuster->plot();
		pTargetPlot = pTarget->plot();
	}
	else
		pTargetPlot = FindBestTarget(&pMusterPlot);

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

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationDefendCity::FindBestTarget(CvPlot** ppMuster) const
{
	CvCity* pTargetCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;

	// Defend the city most under threat
	pTargetCity = GET_PLAYER(m_eOwner).GetThreatenedCityByRank();

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if (pTargetCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if (pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		if (pEnemyCapital != NULL)
		{
			pTargetCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
		}
	}

	if(pTargetCity != NULL)
	{
		pPlot = pTargetCity->plot();

		if (ppMuster)
		{
			CvCity* pMusterCity = GET_PLAYER(m_eOwner).GetClosestCity(pPlot,23,true);
			if (!pMusterCity)
				pMusterCity = pTargetCity;
			*ppMuster = pMusterCity->plot();
		}
	}

	return pPlot;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDefenseRapidResponse
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationDefenseRapidResponse::CvAIOperationDefenseRapidResponse()
{
}

/// Destructor
CvAIOperationDefenseRapidResponse::~CvAIOperationDefenseRapidResponse()
{
}

/// Kick off this operation
void CvAIOperationDefenseRapidResponse::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget, CvCity* pMuster, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if(iAreaID == -1)
		return;

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
void CvAIOperationNavalInvasion::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity* pTarget, CvCity* pMuster, bool bOceanMoves)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if (!pTarget || !pMuster)
		return;

	// Target just off the coast - important that it is owned by eEnemy - we terminate if that should ever change
	CvPlot *pCoastalTarget = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTarget->plot());
	// Muster just off the coast
	CvPlot *pCoastalMuster = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMuster->plot());

	SetupWithSingleArmy(pCoastalMuster, pCoastalTarget, NULL, NULL, bOceanMoves);
}
MultiunitFormationTypes CvAIOperationNavalInvasion::GetFormation() const
{
	//varies with era
	return MilitaryAIHelpers::GetCurrentBestFormationTypeForNavalAttack();
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalInvasionSneaky
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNavalInvasionSneaky::CvAIOperationNavalInvasionSneaky()
{
}

CvAIOperationNavalInvasionSneaky::~CvAIOperationNavalInvasionSneaky()
{
}

CvAIOperationNukeAttack::CvAIOperationNukeAttack()
{
}

CvAIOperationNukeAttack::~CvAIOperationNukeAttack()
{
}

void CvAIOperationNukeAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iAreaID*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/, bool /*bOceanMoves */)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster, pTarget, pMuster);
}

bool CvAIOperationNukeAttack::CheckTransitionToNextStage()
{
	if (m_viArmyIDs.empty())
		return false;

	CvPlot* pTargetPlot = GetTargetPlot();
	CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);

	//don't care about the intermediate niceties ... just nuke away!
	if(pTargetPlot && pArmy)
	{
		if(pArmy->GetNumSlotsFilled()>0 || GrabUnitsFromTheReserves(GetMusterPlot(),pTargetPlot, pArmy))
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
						CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
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

			//don't nuke if we're about to capture it
			if (pLoopCity->isInDangerOfFalling())
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
					pInnerLoopUnit = ::getUnit(*pUnitNode);
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
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, WeightedUnitIdVector&)
{
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);

	// check all of our units to find the nuke
	int iUnitLoop;
	for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iUnitLoop))
	{
		if(pLoopUnit && pLoopUnit->canNuke() && pLoopUnit->plot()==GetMusterPlot() && pLoopUnit->getArmyID()==-1)
		{
			CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
			pThisArmy->AddUnit(pLoopUnit->GetID(), thisOperationSlot.m_iSlotID);
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
			SetTargetPlot(pNewTarget);

			CvCity* pMusterCity = GET_PLAYER(m_eOwner).GetClosestCity(pNewTarget,15,true);
			if (!pMusterCity)
				return AI_ABORT_NO_MUSTER;

			SetMusterPlot( pMusterCity->plot() );

			CvUnit* pRescueUnit = pNewTarget->getFirstUnitOfAITypeSameTeam(BARBARIAN_TEAM, UNITAI_SETTLE);
			if (!pRescueUnit)
				pRescueUnit = pNewTarget->getFirstUnitOfAITypeSameTeam(BARBARIAN_TEAM, UNITAI_WORKER);
			if (pRescueUnit)
				m_iUnitToRescue = pRescueUnit->GetID();

			// If we're traveling on a single continent, set our destination to be a few plots shy of the final target
			if (pArmy->GetArea() == GetTargetPlot()->getArea())
			{
				SPathFinderUserData data( m_eOwner, PT_GENERIC_SAME_AREA, m_eEnemy );
				SPath path = GC.GetStepFinder().GetPath( pArmy->Plot(), GetTargetPlot(), data );
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

bool CvAIOperationCivilian::IsEscorted()
{
	if (m_eCurrentState==AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == INVALID_AI_OPERATION_STATE)
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo( GetFormation() );

		if (thisFormation && thisFormation->getNumFormationSlotEntries()>1)
			return true;

		return false;
	}
	else
	{
		if (m_viArmyIDs.empty())
			return false;

		CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);

		//the unit to be escorted is always the first one
		CvUnit* pCivilian = pThisArmy->GetFirstUnit();
		if (!pCivilian)
			return false;
		//the second unit would be the first escort
		CvUnit* pEscort = pThisArmy->GetNextUnit();

		return pEscort != NULL;
	}
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

int OperationalAIHelpers::GetGatherRangeForXUnits(int iTotalUnits)
{
	if(iTotalUnits < 5)
		return 3;

	return 4;
}

/// Find the barbarian camp we want to eliminate
CvPlot* OperationalAIHelpers::FindBestCoastalBombardmentTarget(PlayerTypes ePlayer, PlayerTypes eEnemy, CvPlot** ppMuster)
{
	CvCity* pBestStart = NULL;
	CvPlot* pBestTarget = NULL;
	int iBestDistance = INT_MAX;

	CvCity* pLoopCity;
	int iCityLoop;
	// Loop through each of our cities
	for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
	{
		if (pLoopCity->isCoastal())
		{
			std::vector<int> vAreas = pLoopCity->plot()->getAllAdjacentAreas();
			for (size_t i=0; i<vAreas.size(); i++)
			{
				//they should all be water, but doesn't hurt to check
				if (GC.getMap().getArea(vAreas[i])->isWater())
				{
					CvPlot* pBestTargetHere = OperationalAIHelpers::FindEnemiesNearPlot(ePlayer,eEnemy,DOMAIN_LAND,false,vAreas[i],pLoopCity->plot());
					if (pBestTargetHere)
					{
						int iDistance = plotDistance(*pBestTargetHere,*pLoopCity->plot());
						if (iDistance < iBestDistance)
						{
							pBestStart = pLoopCity;
							pBestTarget = pBestTargetHere;
							iBestDistance = iDistance;
						}
					}
				}
			}
		}
	}

	if (ppMuster && pBestStart)
		*ppMuster = MilitaryAIHelpers::GetCoastalPlotNearPlot(pBestStart->plot());

	return pBestTarget;
}

/// Find the barbarian camp we want to eliminate
CvPlot* OperationalAIHelpers::FindBestBarbCamp(PlayerTypes ePlayer, CvPlot** ppMuster)
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
				if (pPlot->getArea() != pLoopCity->getArea() && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canEmbark())
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
	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if (iThisFormationIndex != NO_MUFORMATION)
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
		if (thisFormation)
		{
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
			return thisSlotEntry.m_requiredSlot;
		}
	}
	return false;
}

bool OperationalAIHelpers::IsUnitSuitableForRecruitment(CvUnit* pLoopUnit, CvPlot* pMusterPlot, const ReachablePlots& turnsFromMuster, 
														CvPlot* pTargetPlot, bool bMustNaval, bool bMustBeDeepWaterNaval, int& iTurnDistance, CvMultiUnitFormationInfo* thisFormation, CvArmyAI* pArmy)
{
	if (pLoopUnit->IsCivilianUnit() && pLoopUnit->AI_getUnitAIType() != UNITAI_GENERAL && pLoopUnit->AI_getUnitAIType() != UNITAI_ADMIRAL && pLoopUnit->AI_getUnitAIType() != UNITAI_CITY_SPECIAL)
		return false;

	if (!pLoopUnit->canRecruitFromTacticalAI())
	{
		//Is it a garrison we can spare?
		if (pLoopUnit->IsGarrisoned())
		{
			CvTacticalDominanceZone* pZone = GET_PLAYER(pLoopUnit->getOwner()).GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(pLoopUnit->plot()->getPlotCity(),false);
			if (!pZone || pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY || pZone->GetBorderScore()>2)
				return false;
		}
		else
			return false;
	}

	//In an army?
	if (pLoopUnit->getArmyID() != -1)
		return false;

	CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
	if (!pkLoopUnitPlot || !pMusterPlot || !pTargetPlot)
		return false;

	//don't recruit if currently healing
	if (GET_PLAYER(pLoopUnit->getOwner()).GetTacticalAI()->IsUnitHealing(pLoopUnit->GetID()))
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit: Healing. Unit is: %s", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
		return false;
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
		return false;
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
		return false;
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
		return false;
	}
	CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
	
	//Check formation entry here.
	if(thisFormation != NULL)
	{
		if (pArmy)
		{
			bool bGood = false;
			for (int iI = 0; iI < thisFormation->getNumFormationSlotEntries(); iI++)
			{
				CvArmyFormationSlot* pSlot = pArmy->GetFormationSlot(iI);
				if (!pSlot)
					return false;

				const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iI);
				if (pSlot->GetUnitID() == ARMYSLOT_NO_UNIT)
				{
					if (unitInfo->GetUnitAIType(thisSlotEntry.m_primaryUnitType) || unitInfo->GetUnitAIType(thisSlotEntry.m_secondaryUnitType))
					{
						bGood = true;
						break;
					}
				}
			}
			if (!bGood)
			{
				return false;
			}
		}
	}

	//check if the unit is engaged with the enemy outside of our lands...
	if (pLoopUnit->plot()->getOwner() != pLoopUnit->getOwner() && pLoopUnit->IsEnemyInMovementRange())
		return false;

	//don't pull out units from zones we need to defend
	CvTacticalDominanceZone *pZone = GET_PLAYER(pLoopUnit->getOwner()).GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(pLoopUnit->plot());
	if (pZone && pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
		//exception: ships in harbor
		if (pLoopUnit->getDomainType()!=DOMAIN_SEA || !pLoopUnit->plot()->isCity())
			return false;

	//don't take explorers
	if (unitInfo == NULL || pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
	{
		return false;
	}

	if (pLoopUnit->getDomainType() == DOMAIN_LAND)
	{
		if (bMustNaval && !pLoopUnit->CanEverEmbark())
		{
			/*
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Cannot recruit unit %s: Cannot Embark.", pLoopUnit->getName().GetCString());
				LogOperationSpecialMessage(strMsg);
			}
			*/
			return false;
		}
	}

	// Special for paratroopers
	if (pLoopUnit->getDropRange() > 0)
	{
		if (iTurnDistance <= pLoopUnit->getDropRange()) //this is not really correct ...
		{
			if (pLoopUnit->canParadropAt(pMusterPlot, pMusterPlot->getX(), pMusterPlot->getY()))
			{
				iTurnDistance = 0;
			}
			else
			{
				CvPlot* pAdjacentPlot;
				for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
				{
					pAdjacentPlot = plotDirection(pMusterPlot->getX(), pMusterPlot->getY(), ((DirectionTypes)jJ));
					if (pAdjacentPlot != NULL)
					{
						if (pLoopUnit->canParadropAt(pAdjacentPlot, pAdjacentPlot->getX(), pAdjacentPlot->getY()))
						{
							iTurnDistance = 1;
							break;
						}
					}
				}
			}
		}
	}
	else
	{ 
		// finally, if the unit is too far away, no deal
		ReachablePlots::const_iterator it = turnsFromMuster.find(pLoopUnit->plot()->GetPlotIndex());
		if (it==turnsFromMuster.end())
		{
			/*/
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Cannot recruit unit: Too far away. Unit is: %s", pLoopUnit->getName().GetCString());
				LogOperationSpecialMessage(strMsg);
			}
			*/
			return false;
		}
		else
		{
			if (it->iTurns > iTurnDistance)
			{
				iTurnDistance = MAX_INT;
				return false;
			}
			if (bMustNaval)
			{
				if (!pMusterPlot->isWater())
					pMusterPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMusterPlot);
				if (!pTargetPlot->isWater())
					pTargetPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pTargetPlot);

				if (pMusterPlot != NULL && pTargetPlot != NULL)
				{
					//naval units are fast and terrain is easy. if there's a path we assume we can attack it
					//we enforce the same area for naval ops, everything else leads to problems later
					if ((pMusterPlot->getArea() != pLoopUnit->plot()->getArea()) || (pMusterPlot->getArea() != pTargetPlot->getArea()))
					{
						iTurnDistance = INT_MAX;
						return false;
					}
					else
					{
						iTurnDistance = it->iTurns;
					}
				}
			}
			else
			{
				iTurnDistance = it->iTurns;
			}
		}
	}

	return true;
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
						if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getEmbarkedAllWaterPassage())
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
						if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getEmbarkedAllWaterPassage())
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
