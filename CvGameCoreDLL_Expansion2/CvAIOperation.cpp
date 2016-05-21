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
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
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
	LogOperationSpecialMessage("setting new target");

	if (pTarget==NULL)
	{
		m_iTargetX = INVALID_PLOT_COORD;
		m_iTargetY = INVALID_PLOT_COORD;
		return;
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

		//naval gets more space - but be careful, this number has quadratic runtime impact!
		if ( IsNavalOperation() )
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

				if(IsNavalOperation() && !pLoopPlot->isWater())
					continue;

				if(IsNavalOperation() && !pArmy->IsAllOceanGoing() && pLoopPlot->isDeepWater())
					continue;

				if (pLoopPlot->canPlaceUnit(GetOwner()))
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
	case AI_OPERATION_BASIC_CITY_ATTACK:
		return FNEW(CvAIOperationCityAttackBasic(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return FNEW(CvAIOperationOffensiveAntiBarbarian(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_FOUND_CITY:
		return FNEW(CvAIOperationCivilianFoundCity(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_MERCHANT_DELEGATION:
		return FNEW(CvAIOperationCivilianMerchantDelegation(), c_eCiv5GameplayDLL, 0);
#if defined(MOD_DIPLOMACY_CITYSTATES)
	case AI_OPERATION_DIPLOMAT_DELEGATION:
		return FNEW(CvAIOperationCivilianDiplomatDelegation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_ALLY_DEFENSE:
		return FNEW(CvAIOperationDefendAlly(), c_eCiv5GameplayDLL, 0);
#endif
	case AI_OPERATION_CONCERT_TOUR:
		return FNEW(CvAIOperationCivilianConcertTour(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_BOMBARDMENT:
		return FNEW(CvAIOperationOffensiveNavalBombardment(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return FNEW(CvAIOperationOffensiveNavalSuperiority(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PURE_NAVAL_CITY_ATTACK:
		return FNEW(CvAIOperationOffensiveNavalOnlyCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_COLONIZATION:
		return FNEW(CvAIOperationCivilianColonization(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_QUICK_COLONIZE:
		return FNEW(CvAIOperationCivilianQuickColonize(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PILLAGE_ENEMY:
		return FNEW(CvAIOperationPillageEnemy(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_CLOSE_DEFENSE:
		return FNEW(CvAIOperationDefendCity(), c_eCiv5GameplayDLL, 0);
#if defined(MOD_BALANCE_CORE)
	case AI_OPERATION_CITY_CLOSE_DEFENSE_PEACE:
		return FNEW(CvAIOperationDefendCityPeace(), c_eCiv5GameplayDLL, 0);
#endif
	case AI_OPERATION_RAPID_RESPONSE:
		return FNEW(CvAIOperationDefenseRapidResponse(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_SNEAK_CITY_ATTACK:
		return FNEW(CvAIOperationCitySneakAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_SMALL_CITY_ATTACK:
		return FNEW(CvAIOperationCityAttackSmall(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_STATE_ATTACK:
		return FNEW(CvAIOperationCityStateAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_ATTACK:
		return FNEW(CvAIOperationOffensiveNavalAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		return FNEW(CvAIOperationOffensiveNavalSneakAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_STATE_NAVAL_ATTACK:
		return FNEW(CvAIOperationOffensiveNavalCityStateAttack(), c_eCiv5GameplayDLL, 0);
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
		if(pThisArmy)
		{
			if(pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
			{
				int iThisFormationIndex = pThisArmy->GetFormationIndex();
				if(iThisFormationIndex != NO_MUFORMATION)
				{
					CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
					if(thisFormation)
					{
						for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
						{
							OperationSlot thisOperationSlot;
							thisOperationSlot.m_iOperationID = m_iID;
							thisOperationSlot.m_iArmyID = m_viArmyIDs[uiI];
							thisOperationSlot.m_iSlotID = iThisSlotIndex;
							m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);
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
}

/// Find out the next item to build for this operation
OperationSlot CvAIOperation::PeekAtNextUnitToBuild(int iAreaID)
{
	OperationSlot thisSlot;
	if(iAreaID == -1 || (GetMusterPlot() && GetMusterPlot()->getArea()==iAreaID) && !m_viListOfUnitsWeStillNeedToBuild.empty())
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
	bool bMustNaval = IsNavalOperation();
	if(bMustNaval)
	{
		bMustBeDeepWaterNaval = OperationalAIHelpers::NeedOceanMoves(m_eOwner, pMusterPlot, pTargetPlot);
	}
	if(bMustNaval)
	{
		CvPlot* pAdjacentPlot = NULL;
		if(pMusterPlot->isCoastalLand())
		{
			pAdjacentPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pMusterPlot);
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
			pAdjacentPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pTargetPlot);
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

	int iDistance = 1000;
	if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pUnit,pMusterPlot,pTargetPlot,bMustNaval,bMustBeDeepWaterNaval,iDistance))
	{
		std::deque<OperationSlot>::iterator it;
		for(it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
		{
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(it->m_iSlotID);
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(it->m_iSlotID);
			if (pSlot && pSlot->GetUnitID() == NO_UNIT)
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
						if (iDistance > GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT())
						{
							strMsg.Format("Warning: %s recruited far-away unit %d turns from muster point for a new army", GetOperationName(), iDistance);
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

bool CvAIOperation::GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot)
{
	if (!pMusterPlot || !pTargetPlot)
		return false;

	//anything to do?
	if (m_viListOfUnitsWeStillNeedToBuild.empty())
		return true;

	bool rtnValue = true;
	bool success;
	std::deque<OperationSlot>::iterator it;

	// Copy over the list
	std::deque<OperationSlot> secondList = m_viListOfUnitsWeStillNeedToBuild;

	// Clear main list
	m_viListOfUnitsWeStillNeedToBuild.clear();

	CvString strMsg;
	bool bMustNaval = IsNavalOperation();
	if(bMustNaval)
	{
		bool bMustBeDeepWaterNaval = OperationalAIHelpers::NeedOceanMoves(m_eOwner, pMusterPlot, pTargetPlot);
		CvPlot* pNavalMuster = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pMusterPlot);
		CvPlot* pNavalTarget = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pTargetPlot);
		if(pNavalMuster == NULL || pNavalTarget == NULL)
		{
			strMsg.Format("Naval Operation failed to find good plots!");
			LogOperationSpecialMessage(strMsg);
			return false;
		}

		SPathFinderUserData data(m_eOwner,PT_GENERIC_SAME_AREA,m_eEnemy);
		data.iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET;
		if (GC.GetStepFinder().DoesPathExist(pNavalMuster->getX(), pNavalMuster->getY(), pNavalTarget->getX(), pNavalTarget->getY(), data))
		{
			WeightedUnitIdVector UnitChoices;
			int iLoop = 0;
			for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
			{
				int iDistance = 1000;
				if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit,pNavalMuster,pNavalTarget,true,bMustBeDeepWaterNaval,iDistance))
				{
					// When in doubt prefer units in our own territory
					if (pLoopUnit->plot() && pLoopUnit->plot()->getOwner() != m_eOwner)
						iDistance++;

					UnitChoices.push_back(pLoopUnit->GetID(), 1000 - iDistance*10 - pLoopUnit->GetPower());
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

			if (UnitChoices.size()==0)
				return false;

			UnitChoices.SortItems();

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
						{
							pArmy->SetEstimatedTurn(it->m_iSlotID, ARMYSLOT_NOT_INCLUDING_IN_OPERATION);

						}
					}
				}
			}
		}
		else
			return false;
	}
	else
	{
		WeightedUnitIdVector UnitChoices;
		int iLoop = 0;
		for (CvUnit* pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iLoop))
		{
			int iDistance = 1000;
			if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit,pMusterPlot,pTargetPlot,false,false,iDistance))
			{
				// When in doubt prefer units in our own territory
				if (pLoopUnit->plot() && pLoopUnit->plot()->getOwner() != m_eOwner)
					iDistance++;

				UnitChoices.push_back(pLoopUnit->GetID(), 1000 - iDistance*10 - pLoopUnit->GetPower());
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

		if (UnitChoices.size()==0)
			return false;

		UnitChoices.SortItems();

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
					{
						pArmy->SetEstimatedTurn(it->m_iSlotID, ARMYSLOT_NOT_INCLUDING_IN_OPERATION);

					}
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
			pCurrentPosition = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pCurrentPosition);

		if (!pCurrentPosition)
			return NULL;
	}

	// use the step path finder to get the path
	SPathFinderUserData data(m_eOwner, ePathType, m_eEnemy);
	data.iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET;
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
			pCurrentPosition = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pCurrentPosition);

		if (!pCurrentPosition)
			return NULL;
	}

	// use the step path finder to compute distance
	SPathFinderUserData data(m_eOwner, ePathType, m_eEnemy);
	data.iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET;
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
				CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(IsNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
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
					int iTempValue = 100 - (100 * iDistanceCurrentToTarget / m_iDistanceMusterToTarget);
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
	switch (m_eCurrentState)
	{
		case AI_OPERATION_STATE_RECRUITING_UNITS:
		{
			int iTurns = GC.getGame().getGameTurn() - GetTurnStarted();
			if (iTurns > (GetMaximumRecruitTurns() * 2))
			{
				SetToAbort(AI_ABORT_TIMED_OUT);
			}
			break;
		}
		case AI_OPERATION_STATE_GATHERING_FORCES:
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			// now see how our armies are doing
			bool bAllGood = true;
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				if (pThisArmy)
					bAllGood &= VerifyOrAdjustTarget(pThisArmy);
			}

			if (!bAllGood)
			{
				SetToAbort(AI_ABORT_LOST_TARGET);
			}
			break;
		}
		// Mark units in successful operation
		case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		{
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

				UnitHandle pUnit = pArmy->GetFirstUnit();
				while(pUnit)
				{
					pUnit->SetDeployFromOperationTurn(GC.getGame().getGameTurn());
					pUnit = pArmy->GetNextUnit();
				}
			}
		}
	}

	return (m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH);
}

/// Allows an outside class to terminate the operation
void CvAIOperation::SetToAbort(AIOperationAbortReason eReason)
{
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = eReason;
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

void CvAIOperation::Move()
{
	if (m_viArmyIDs.empty())
		return;

	//single army only for now
	CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);

	pThisArmy->UpdateCheckpointTurns();
	pThisArmy->RemoveStuckUnits();
	if (ShouldAbort())
		return;

	//todo: move the relevant code from tactical AI to operations
	switch (GetMoveType())
	{
	case AI_OPERATION_MOVETYPE_ESCORT:
		GET_PLAYER(m_eOwner).GetTacticalAI()->PlotArmyMovesEscort(pThisArmy);
		break;
	case AI_OPERATION_MOVETYPE_COMBAT:
		GET_PLAYER(m_eOwner).GetTacticalAI()->PlotArmyMovesCombat(pThisArmy);
		break;
	default:
		return;
	}

	SetLastTurnMoved(GC.getGame().getGameTurn());
}

/// Update operation for the next turn
void CvAIOperation::DoTurn()
{
	LogOperationStatus(true);

	Move();
	CheckTransitionToNextStage();

	LogOperationStatus(false);
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
				if(pThisArmy->GetNumSlotsFilled() < pkFormation->getNumFormationSlotEntries() / 2)
				{
					SetToAbort( AI_ABORT_HALF_STRENGTH);
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

			CvPlot* pCenterOfMass = pArmy->GetCenterOfMass(IsNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
			if (pCenterOfMass && pGoalPlot)
			{
				//problem: center of mass may be on a mountain etc ...
				if (!pCenterOfMass->isValidMovePlot(kPlayer.GetID()))
				{
					UnitHandle pFirstUnit = pArmy->GetFirstUnit();
					if (pFirstUnit)
						pCenterOfMass = pFirstUnit->plot();
					else
						return NULL;
				}

				//update the current position
				pArmy->SetXY(pCenterOfMass->getX(), pCenterOfMass->getY());

				//get where we want to be next
				pRtnValue = GetPlotXInStepPath(pCenterOfMass,pGoalPlot,pArmy->GetMovementRate(),true);
				if (!pRtnValue)
				{
					// Can't plot a path, probably due to change of control of hexes.  Will probably abort the operation
					OutputDebugString( CvString::format( "cannot find a step path from %d,%d to %d,%d\n", pCenterOfMass->getX(), pCenterOfMass->getY(), pGoalPlot->getX(), pGoalPlot->getY() ).c_str() );
					return NULL;
				}
			}
		}
		break;
	}
	return pRtnValue;
}

bool CvAIOperation::HasOneMoreSlotToFill() const
{
	if(m_viListOfUnitsWeStillNeedToBuild.size() == 1 && m_eCurrentState  == AI_OPERATION_STATE_RECRUITING_UNITS)
	{
		return true;
	}

	return false;
}

bool CvAIOperation::BuyFinalUnit()
{
	CvCity* pCity = NULL;
	if (IsNavalOperation())
		pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCityFriendly(m_eEnemy);
	else
		pCity = GET_PLAYER(m_eOwner).GetClosestCity( GetMusterPlot() );

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
		case AI_OPERATION_BASIC_CITY_ATTACK:
		case AI_OPERATION_PILLAGE_ENEMY:
		case AI_OPERATION_SNEAK_CITY_ATTACK:
		case AI_OPERATION_SMALL_CITY_ATTACK:
		case AI_OPERATION_NAVAL_BOMBARDMENT:
		case AI_OPERATION_NAVAL_SUPERIORITY:
		case AI_OPERATION_NAVAL_ATTACK:
		case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		case AI_OPERATION_CITY_STATE_ATTACK:
		case AI_OPERATION_CITY_STATE_NAVAL_ATTACK:
		case AI_OPERATION_NUKE_ATTACK:
		case AI_OPERATION_PURE_NAVAL_CITY_ATTACK:
#if defined(MOD_DIPLOMACY_CITYSTATES)
		case AI_OPERATION_DIPLOMAT_DELEGATION:
		case AI_OPERATION_ALLY_DEFENSE:
#endif
			
			strOutBuf = CvString( GetOperationName() ) + ", ";
			strOutBuf += strTemp1 + strTemp2;
			if (m_eEnemy != NO_PLAYER)
			{
				strPlayerName = GET_PLAYER(m_eEnemy).getCivilizationShortDescription();
				strOutBuf += ", vs. " + strPlayerName;
			}
			GET_PLAYER(m_eOwner).GetMilitaryAI()->LogMilitarySummaryMessage(strOutBuf);
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
		case AI_OPERATION_STATE_ABORTED:
			strTemp.Format("Aborted, %d", m_eAbortReason);
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
					if(pSlot->GetTurnAtCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
					{
						szTemp2 = "No Info, ";
					}
					else if(pSlot->GetTurnAtCheckpoint() == ARMYSLOT_NOT_INCLUDING_IN_OPERATION)
					{
						szTemp2 = "Skipping, ";
					}
					else if(pSlot->GetUnitID() == NO_UNIT)
					{
						szTemp3.Format("Turn %d, ", pSlot->GetTurnAtCheckpoint());
						szTemp2 = "Training - " + szTemp3;
					}
					else
					{
						UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("%s %d at (%d:%d) - ETA at checkpoint %d, ", 
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
				szTemp2.Format("Gathering Forces, Army: %d, Gather X: %d, Gather Y: %d, ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY());
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while(iUnitID != ARMY_NO_UNIT)
				{
					// do something with each entry
					UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
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
				szTemp2.Format("Moving To Target, Army: %d, At X: %d, At Y: %d, To X: %d, To Y: %d, ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY(), m_iTargetX, m_iTargetY);
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while(iUnitID != ARMY_NO_UNIT)
				{
					// do something with each entry
					UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if(pThisUnit)
					{
						szTemp2.Format("%s %d at (%d:%d),", pThisUnit->getName().GetCString(), pThisUnit->GetID(), pThisUnit->getX(), pThisUnit->getY());
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

		strTemp = "Ended, ";

		switch(m_eAbortReason)
		{
		case AI_ABORT_SUCCESS:
			strTemp += "Success";
			break;
		case AI_ABORT_NO_TARGET:
			strTemp += "No target";
			break;
		case AI_ABORT_REPEAT_TARGET:
			strTemp += "Repeat target";
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
		case AI_ABORT_NO_NUKES:
			strTemp += "No nukes";
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
#if defined(MOD_BALANCE_CORE)
		case AI_ABORT_TIMED_OUT:
			strTemp += "Timed Out";
			break;
		case AI_ABORT_NO_UNITS:
			strTemp += "No Units";
			break;
#endif
		default:
			strTemp += "Unknown reason";
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		// Message to summary log?
		switch (GetOperationType())
		{
		case AI_OPERATION_BASIC_CITY_ATTACK:
		case AI_OPERATION_PILLAGE_ENEMY:
		case AI_OPERATION_SNEAK_CITY_ATTACK:
		case AI_OPERATION_SMALL_CITY_ATTACK:
		case AI_OPERATION_NAVAL_BOMBARDMENT:
		case AI_OPERATION_NAVAL_SUPERIORITY:
		case AI_OPERATION_NAVAL_ATTACK:
		case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		case AI_OPERATION_CITY_STATE_ATTACK:
		case AI_OPERATION_CITY_STATE_NAVAL_ATTACK:
		case AI_OPERATION_NUKE_ATTACK:
		case AI_OPERATION_PURE_NAVAL_CITY_ATTACK:
#if defined(MOD_DIPLOMACY_CITYSTATES)
		case AI_OPERATION_DIPLOMAT_DELEGATION:
		case AI_OPERATION_ALLY_DEFENSE:
#endif
			strOutBuf = CvString( GetOperationName() ) + ", ";
			strOutBuf += strTemp;
			GET_PLAYER(m_eOwner).GetMilitaryAI()->LogMilitarySummaryMessage(strOutBuf);
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

bool CvAIOperation::SetupWithSingleArmy(CvPlot * pMusterPlot, CvPlot * pTargetPlot, CvPlot * pDeployPlot, CvUnit* pInitialUnit)
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
	SetTargetPlot(pTargetPlot);
	SetMusterPlot(pMusterPlot);

	//this is for the army
	if (!pDeployPlot)
		pDeployPlot = GetPlotXInStepPath(pMusterPlot,pTargetPlot,GetDeployRange(),false);

	if (!pDeployPlot)
	{
		if (GC.getLogging() && GC.getAILogging())
			LogOperationSpecialMessage("Cannot set up operation - no path to target!");

		SetToAbort( AI_ABORT_LOST_PATH );
		return false;
	}

	pArmyAI->SetGoalPlot(pDeployPlot);
	pArmyAI->SetXY(pMusterPlot->getX(), pMusterPlot->getY());

	if (pInitialUnit)
		pArmyAI->AddUnit(pInitialUnit->GetID(),0);

	// Find the list of units we need to build before starting this operation in earnest
	BuildListOfUnitsWeStillNeedToBuild();

	// try to get as many units as possible from existing units that are waiting around
	if(GrabUnitsFromTheReserves(GetTargetPlot(), GetMusterPlot()))
	{
		pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
		m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
		LogOperationSpecialMessage("Initial stage is mustering");
	}
	else
	{
		pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
		m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
		LogOperationSpecialMessage("Initial stage is recruiting");
	}

	SetTurnStarted(GC.getGame().getGameTurn());
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
	if (pSlot->GetUnitID() != NO_UNIT)
	{
		UnitHandle pUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
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
// CvAIOperationOffensive
////////////////////////////////////////////////////////////////////////////////

/// How long will we wait for a recruit to show up?
int CvAIOperationOffensive::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY();
}

bool CvAIOperationOffensive::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	CvCity* pTroubleSpot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity(0,false);
	if (pTroubleSpot)
	{
		//the trouble spot is right next to us, abort the current operation
		SPathFinderUserData data(m_eOwner,PT_GENERIC_SAME_AREA,NO_PLAYER,10);
		data.iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET;
		if (GC.GetStepFinder().DoesPathExist(pTroubleSpot->plot(),pArmy->GetCenterOfMass(DOMAIN_LAND),data))
			return false;
	}

	//somebody unexpected owning the target? abort
	if (GetTargetPlot()->getOwner()!=NO_PLAYER && GetTargetPlot()->getOwner()!=m_eEnemy)
		return false;

	return true;
}

/// Kick off this operation
void CvAIOperationOffensive::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget, CvCity* pMuster)
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
// CvAIOperationCityAttackBasic
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCityAttackBasic::CvAIOperationCityAttackBasic()
{
}

/// Destructor
CvAIOperationCityAttackBasic::~CvAIOperationCityAttackBasic()
{
}

/// Kick off this operation
void CvAIOperationCityAttackBasic::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget, CvCity* pMuster)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	SetupWithSingleArmy(pMuster->plot(),pTarget->plot());
}

MultiunitFormationTypes CvAIOperationCityAttackBasic::GetFormation() const
{
	//varies with era
	return MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack();
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationCityAttackBasic::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		// See if our target city is still owned by our enemy
		if(GetTargetPlot() != NULL)
		{
			// See if our target city is still owned by our enemy
			if(GetTargetPlot()->getOwner() != m_eEnemy && GetTargetPlot()->getOwner() != NO_PLAYER)
			{
				// Success!  The city has been captured/destroyed
				return true;
			}
			if(GetTargetPlot()->getWorkingCity() != NULL)
			{
				if(GetTargetPlot()->getWorkingCity()->getOwner() != m_eEnemy)
				{
					return true;
				}
			}
		}
		else
			return true;
	}

	return rtnValue;
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

	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = eReason;
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
			if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
				m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				LogOperationSpecialMessage("Transition to mustering stage");
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
			CvPlot *pCenterOfMass = pThisArmy->GetCenterOfMass( pThisArmy->GetDomainType() );
			if(pCenterOfMass && pTarget && plotDistance(*pCenterOfMass,*pTarget) <= GetDeployRange())
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
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
				}

				//that's it. skip STATE_AT_TARGET so the army will be disbanded next turn!
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				pThisArmy->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);
				LogOperationSpecialMessage("Transition to target stage");

				bStateChanged = true;
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
// CvAIOperationOffensiveAntiBarbarian
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationOffensiveAntiBarbarian::CvAIOperationOffensiveAntiBarbarian()
{
	m_iUnitToRescue = -1;
}

/// Destructor
CvAIOperationOffensiveAntiBarbarian::~CvAIOperationOffensiveAntiBarbarian()
{
}

void CvAIOperationOffensiveAntiBarbarian::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,BARBARIAN_PLAYER);

	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster,pTarget);
}

/// How close to target do we end up?
int CvAIOperationOffensiveAntiBarbarian::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE();
}

/// Read serialized data
void CvAIOperationOffensiveAntiBarbarian::Read(FDataStream& kStream)
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
void CvAIOperationOffensiveAntiBarbarian::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
	kStream << m_iUnitToRescue;
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationOffensiveAntiBarbarian::ShouldAbort()
{
	CvString strMsg;

	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		if (GetTargetPlot()==NULL || GetTargetPlot()->getImprovementType()!=GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("No Barbarian camp target for army. Aborting");
				LogOperationSpecialMessage(strMsg);
			}
			return true;
		}
		else if(m_eCurrentState != AI_OPERATION_STATE_RECRUITING_UNITS)
		{
			// If down below strength of camp, abort
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			CvPlot* pTarget = GetTargetPlot();
			UnitHandle pCampDefender = pTarget->getBestDefender(NO_PLAYER);
			if(pCampDefender && pThisArmy->GetTotalPower() < pCampDefender->GetPower())
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Barbarian camp stronger (%d) than our units (%d). Aborting", pCampDefender->GetPower(), pThisArmy->GetTotalPower());
					LogOperationSpecialMessage(strMsg);
				}
				return true;
			}
		}
	}

	return rtnValue;
}

/// Find the barbarian camp we want to eliminate
CvPlot* CvAIOperationOffensiveAntiBarbarian::FindBestTarget(CvPlot** ppMuster) const
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

void CvAIOperationPillageEnemy::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity*, CvCity*)
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
bool CvAIOperationPillageEnemy::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	// Find best pillage target
	CvPlot* pBetterTarget = FindBestTarget(NULL);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		SetToAbort(AI_ABORT_NO_TARGET);
	}
	// If this is a new target, switch to it
	else if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
		return true;

	}

	return false;
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
		CvCity* pClosestCity = GET_PLAYER(m_eOwner).GetClosestCity(pLoopCity->plot());
		if(pClosestCity && pLoopCity->getArea() == pClosestCity->getArea())
		{
			// Initial value of target is the number of improved plots
			iValue = pLoopCity->countNumImprovedPlots();

			// Adjust value based on proximity to our start location
			iDistance = GET_PLAYER(m_eOwner).GetCityDistance(pLoopCity->plot());
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
		CvCity *pClosest = pBestTargetCity ? GET_PLAYER(m_eOwner).GetClosestCity(pBestTargetCity->plot()) : NULL;
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
void CvAIOperationCivilian::Init(int iID, PlayerTypes eOwner, PlayerTypes /* eEnemy */, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,NO_PLAYER);

	CvUnit* pOurCivilian = FindBestCivilian();
	if (!pOurCivilian)
	{
		SetToAbort( AI_ABORT_NO_UNITS );
		return;
	}

	CvPlot* pTargetSite = FindBestTargetForUnit(pOurCivilian,!WillBeEscorted());
	SetupWithSingleArmy(pOurCivilian->plot(),pTargetSite,pTargetSite,pOurCivilian);
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
			if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
				m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				LogOperationSpecialMessage("Transition to mustering stage");
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
		case ARMYAISTATE_AT_DESTINATION:
		{
			CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetFirstUnitID());
			if(pCivilian && pCivilian->plot() == GetTargetPlot())
			{
				pThisArmy->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);
				m_eCurrentState = AI_OPERATION_STATE_AT_TARGET;
				LogOperationSpecialMessage("Transition to target stage");

				//finally do whatever we came here for
				PerformMission(pCivilian);
				bStateChanged = true;
			}
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
	// Find best target
	CvPlot* pBetterTarget = FindBestTargetForUnit(pCivilian,!IsEscorted());

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		SetToAbort( AI_ABORT_NO_TARGET );
		return false;
	}
	else
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

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
	if(pSettler->canFound(pCityPlot) && pSettler->plot() == pCityPlot && pSettler->canMove())
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

		m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		return true;
	}

	return false;
}

bool CvAIOperationCivilianFoundCity::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	UnitHandle pSettler = pArmy->GetFirstUnit();
	bool bCanFound = pSettler->canFound(GetTargetPlot()) && pSettler->GeneratePath(GetTargetPlot(), CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY);

	if (!bCanFound)
		return RetargetCivilian(pSettler.pointer(),pArmy);
	else
	{
		// let's see if the target still makes sense
		CvPlot* pBetterTarget = FindBestTargetIncludingCurrent(pSettler.pointer(), !IsEscorted());

		// No targets at all!
		if(pBetterTarget == NULL)
		{
			SetToAbort(AI_ABORT_NO_TARGET);
			return false;
		}
		// If we have a better target, switch
		else if (pBetterTarget != GetTargetPlot())
		{
			int iOldPlotValue = GetTargetPlot()->getFoundValue(m_eOwner);
			int iNewPlotValue = pBetterTarget->getFoundValue(m_eOwner);

			if (plotDistance(*GetTargetPlot(),*pBetterTarget)<2 || iNewPlotValue>iOldPlotValue*1.2f)
			{
				SetTargetPlot(pBetterTarget);
				pArmy->SetGoalPlot(pBetterTarget);
			}
		}

		return true;
	}
}

/// Find the plot where we want to settle
CvPlot* CvAIOperationCivilianFoundCity::FindBestTargetIncludingCurrent(CvUnit* pUnit, bool bOnlySafePaths)
{
	//todo: better options
	//a) return a list of possible targets and find the ones that are currently reachable
	//b) if the best target is unreachable, move in the general direction and hope the block will clear up

	//ignore the current operation target when searching. default would be to suppress currently targeted plots
	bool bIsSafe = false;
	int iTargetArea = GetTargetPlot() ? GetTargetPlot()->getArea() : -1;

	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, iTargetArea, bIsSafe, this);

	//try again if the result is not good
	if (pResult == NULL || (!bIsSafe && bOnlySafePaths) )
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, -1, bIsSafe, this);

	if (pResult == NULL || (!bIsSafe && bOnlySafePaths) )
		return NULL;
	else
		return pResult;
}

CvPlot* CvAIOperationCivilianFoundCity::FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths)
{
	bool bIsSafe = false;
	int iTargetArea = GetTargetPlot() ? GetTargetPlot()->getArea() : -1;

	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, iTargetArea, bIsSafe);

	//try again if the result is not good
	if (pResult == NULL || (!bIsSafe && bOnlySafePaths) )
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, -1, bIsSafe, this);

	if (pResult == NULL || (!bIsSafe && bOnlySafePaths) )
		return NULL;
	else
		return pResult;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCivilianQuickColonize
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCivilianQuickColonize::CvAIOperationCivilianQuickColonize()
{
}

/// Destructor
CvAIOperationCivilianQuickColonize::~CvAIOperationCivilianQuickColonize()
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
	if(pMerchant->plot()->getOwner() == GetTargetPlot()->getOwner() && pMerchant->canMove() && pMerchant->canTrade(pMerchant->plot()))
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

		m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		return true;
	}

	return false;
}

/// Find the plot where we want to settler
CvPlot* CvAIOperationCivilianMerchantDelegation::FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlot(pUnit, !bOnlySafePaths /*m_bEscorted*/);
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
CvPlot* CvAIOperationCivilianDiplomatDelegation::FindBestTargetForUnit(CvUnit* pUnit, bool /*bOnlySafePaths*/)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).ChooseMessengerTargetPlot(pUnit);
}

bool CvAIOperationCivilianDiplomatDelegation::PerformMission(CvUnit* pDiplomat)
{
	if(pDiplomat->plot() == GetTargetPlot() && pDiplomat->canMove() && pDiplomat->canTrade(pDiplomat->plot()))
	{
		pDiplomat->PushMission(CvTypes::getMISSION_TRADE());

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Great Diplomat finishing Diplomatic Mission at %s", pDiplomat->plot()->GetAdjacentCity()->getName().c_str());
			LogOperationSpecialMessage(strMsg);
		}

		m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
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
void CvAIOperationDefendAlly::Init(int iID, PlayerTypes eOwner, PlayerTypes eAlly, CvCity* pTarget, CvCity* pMuster)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eAlly);

	CvPlot* pMusterPlot;
	CvPlot* pTargetPlot;
	if (pTarget && pMuster)
	{
		pMusterPlot = pMuster->plot();
		pTargetPlot = MilitaryAIHelpers::GetLandPlotAdjacentToTarget(pTarget->plot());
	}
	else
		pTargetPlot = FindBestTarget(&pMusterPlot);

	SetupWithSingleArmy(pMusterPlot,pTargetPlot);
}

bool CvAIOperationDefendAlly::VerifyOrAdjustTarget(CvArmyAI* /*pArmy*/)
{
	// See if our target city is still owned by our ally (which is stored in m_eEnemy)
	if(GetTargetPlot()->getOwner() != m_eEnemy)
	{
		// Bummer! The city has been captured/destroyed
		return false;
	}
	else if(GET_PLAYER(m_eEnemy).isMinorCiv())
	{
		// Quest is over?
		if( !GET_PLAYER(m_eEnemy).GetMinorCivAI()->IsActiveQuestForPlayer(m_eOwner, MINOR_CIV_QUEST_HORDE) && 
			!GET_PLAYER(m_eEnemy).GetMinorCivAI()->IsActiveQuestForPlayer(m_eOwner, MINOR_CIV_QUEST_REBELLION))
		{
			return false;
		}
	}

	return true;
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
CvPlot* CvAIOperationCivilianConcertTour::FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths)
{
	if(!pUnit)
		return NULL;

	return GET_PLAYER(pUnit->getOwner()).FindBestMusicianTargetPlot(pUnit, !bOnlySafePaths /*m_bEscorted*/);
}

bool CvAIOperationCivilianConcertTour::PerformMission(CvUnit* pMusician)
{
	if(pMusician->plot() == GetTargetPlot() && pMusician->canMove() && pMusician->canBlastTourism(pMusician->plot()))
	{
		pMusician->PushMission(CvTypes::getMISSION_ONE_SHOT_TOURISM());
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Great Musician performing concert tour, At X=%d, At Y=%d", pMusician->plot()->getX(), pMusician->plot()->getY());
			LogOperationSpecialMessage(strMsg);
		}

		m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationOffensiveNavalBasic
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationOffensiveNavalBasic::CvAIOperationOffensiveNavalBasic()
{
}

/// Destructor
CvAIOperationOffensiveNavalBasic::~CvAIOperationOffensiveNavalBasic()
{
}

/// Kick off this operation
void CvAIOperationOffensiveNavalBasic::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget, CvCity* pMuster)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if (!pTarget || !pMuster)
		return;

	// Target just off the coast
	CvPlot *pCoastalTarget = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pTarget->plot());
	// Muster just off the coast
	CvPlot *pCoastalMuster = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pMuster->plot());

	SetupWithSingleArmy(pCoastalMuster,pCoastalTarget);
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationOffensiveNavalBasic::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		// See if our target city is still owned by our enemy
		if((GetTargetPlot()->getOwner() != m_eEnemy) && (m_eEnemy != BARBARIAN_PLAYER))
		{
			// The city has been captured/destroyed
			return true;
		}
	}

	return rtnValue;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationOffensiveNavalBombardment
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationOffensiveNavalBombardment::CvAIOperationOffensiveNavalBombardment()
{
}

/// Destructor
CvAIOperationOffensiveNavalBombardment::~CvAIOperationOffensiveNavalBombardment()
{
}

/// Kick off this operation
void CvAIOperationOffensiveNavalBombardment::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	if(eEnemy == NO_PLAYER)
		eEnemy = BARBARIAN_PLAYER;

	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster,pTarget);
}

/// Find the barbarian camp we want to eliminate
CvPlot* CvAIOperationOffensiveNavalBombardment::FindBestTarget(CvPlot** ppMuster) const
{
	return OperationalAIHelpers::FindBestCoastalBombardmentTarget(m_eOwner,m_eEnemy,ppMuster);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationOffensiveNavalSuperiority
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationOffensiveNavalSuperiority::CvAIOperationOffensiveNavalSuperiority()
{
}

/// Destructor
CvAIOperationOffensiveNavalSuperiority::~CvAIOperationOffensiveNavalSuperiority()
{
}

/// Kick off this operation
void CvAIOperationOffensiveNavalSuperiority::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget, CvCity* pMuster)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if (!pMuster || !pTarget)
		return;

	//this is where we gather the army
	CvPlot* pMusterPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pMuster->plot());
	//this is where the army should go
	CvPlot* pGoalPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pTarget->plot());
	if (!pMusterPlot || !pGoalPlot)
		return;

	SetupWithSingleArmy(pMusterPlot,pTarget->plot(),pGoalPlot);
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationOffensiveNavalSuperiority::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		if((FindBestTarget(NULL) == NULL) && (GetTargetPlot() == NULL))
		{
			return true;
		}
	}

	return rtnValue;
}

/// Find the nearest enemy naval unit to eliminate
CvPlot* CvAIOperationOffensiveNavalSuperiority::FindBestTarget(CvPlot** ppMuster) const
{
	CvPlot* pRefPlot = GetTargetPlot();
	if (pRefPlot)
	{
		CvPlot* pTarget = OperationalAIHelpers::FindEnemies(m_eOwner,m_eEnemy,DOMAIN_SEA,false,pRefPlot->getArea(),pRefPlot);
		if (ppMuster)
			*ppMuster = pTarget;
		return pTarget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationOffensiveNavalOnlyCityAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationOffensiveNavalOnlyCityAttack::CvAIOperationOffensiveNavalOnlyCityAttack()
{
}

/// Destructor
CvAIOperationOffensiveNavalOnlyCityAttack::~CvAIOperationOffensiveNavalOnlyCityAttack()
{
}

/// Kick off this operation
void CvAIOperationOffensiveNavalOnlyCityAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget, CvCity* pMuster)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMusterPlot = NULL;
	CvPlot* pTargetPlot = NULL;

	if(pMuster == NULL || !pMuster->isCoastal())
	{
		CvCity* pNearestCoastalCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCityFriendly(eEnemy);
		if(pNearestCoastalCity != NULL)
		{
			pMuster = pNearestCoastalCity;
		}
		if(pMuster != NULL)
		{
			pMusterPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pMuster->plot());
		}
	}

	if(pTarget == NULL || !pTarget->isCoastal())
	{
		CvCity* pNearestCoastalCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCityEnemy(eEnemy);
		if(pNearestCoastalCity != NULL)
		{
			pTarget = pNearestCoastalCity;
		}
		if(pTarget != NULL)
		{
			pTargetPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pTarget->plot());
		}
	}

	SetupWithSingleArmy(pMusterPlot,pTargetPlot);
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
	pTargetCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity(0,true);

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
void CvAIOperationDefendCity::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget, CvCity* pMuster)
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

bool CvAIOperationDefendCity::VerifyOrAdjustTarget(CvArmyAI* /*pArmy*/)
{
	// See if our target city is still owned by us
	if(GetTargetPlot()->getOwner() != m_eOwner)
	{
		// Bummer! The city has been captured/destroyed
		return false;
	}

	return true;
}

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationDefendCity::FindBestTarget(CvPlot** ppMuster) const
{
	CvCity* pTargetCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;

	// Defend the city most under threat
	pTargetCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity(0,false);

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if(pTargetCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if(pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		pTargetCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
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
void CvAIOperationDefenseRapidResponse::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMusterPlot;
	CvPlot* pTargetPlot = FindBestTarget(&pMusterPlot);
	if (!pTargetPlot)
		return;

	SetupWithSingleArmy(pMusterPlot,pTargetPlot);
}

/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
bool CvAIOperationDefenseRapidResponse::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	// Find most threatened city
	CvPlot* pBetterTarget = FindBestTarget(NULL);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		SetToAbort(AI_ABORT_NO_TARGET);
		return false;
	}
	// If this is a new target, switch to it
	else if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	return (GetTargetPlot() != NULL);
}

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationDefenseRapidResponse::FindBestTarget(CvPlot** ppMuster) const
{
	int iRefArea = -1;
	CvPlot* pRefPlot = GetTargetPlot();
	if (pRefPlot)
		iRefArea = pRefPlot->getArea();

	CvPlot* pTarget = OperationalAIHelpers::FindEnemies(m_eOwner,m_eEnemy,DOMAIN_LAND,false,iRefArea,pRefPlot);
	if (ppMuster)
		*ppMuster = pTarget;
	return pTarget;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationOffensiveNavalEscorted
////////////////////////////////////////////////////////////////////////////////
CvAIOperationOffensiveNavalEscorted::CvAIOperationOffensiveNavalEscorted()
{
}

CvAIOperationOffensiveNavalEscorted::~CvAIOperationOffensiveNavalEscorted()
{
}

/// Kick off this operation
void CvAIOperationOffensiveNavalEscorted::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget, CvCity* pMuster)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	if (!pTarget || !pMuster)
		return;

	// Target just off the coast
	CvPlot *pCoastalTarget = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pTarget->plot());
	// Muster just off the coast
	CvPlot *pCoastalMuster = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pMuster->plot());

	SetupWithSingleArmy(pCoastalMuster,pCoastalTarget);
}

bool CvAIOperationOffensiveNavalEscorted::VerifyOrAdjustTarget(CvArmyAI * pArmy)
{
	CvCity* pTroubleSpot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity(0,false);
	if (pTroubleSpot && pTroubleSpot->isCoastal())
	{
		//the trouble spot is right next to us, abort the current operation
		SPathFinderUserData data(m_eOwner,PT_GENERIC_SAME_AREA,NO_PLAYER,15);
		data.iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET;
		if (GC.GetStepFinder().DoesPathExist(pTroubleSpot->plot(),pArmy->GetCenterOfMass(DOMAIN_SEA),data))
			return false;
	}

	return true;
}

/// Start the civilian off to a new target plot
bool CvAIOperationCivilianColonization::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
{
	// Find best city site (assuming we are escorted)
	CvPlot* pBetterTarget = FindBestTargetForUnit(pCivilian,false);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
		return false;

	// If this is a new target, switch to it
	if(pBetterTarget != GetTargetPlot())
	{
		//throw out any ships on the wrong water body
		std::vector<int> aiUnitsToRemove;
		for (UnitHandle pUnit = pArmy->GetFirstUnit(); pUnit.pointer(); pUnit = pArmy->GetNextUnit())
		{
			if (pUnit->TurnsToReachTarget(pBetterTarget) == MAX_INT)
				aiUnitsToRemove.push_back(pUnit->GetID());
		}
		for (std::vector<int>::iterator it = aiUnitsToRemove.begin(); it != aiUnitsToRemove.end(); ++it)
			pArmy->RemoveUnit((*it));

		//set the new target
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);

		//find the best muster plot
		CvPlot* pWaterPlot = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pBetterTarget);
		int iLoop = 0;
		int iBestDistance = INT_MAX;
		CvCity* pBestMuster = NULL;
		for (CvCity* pCity = GET_PLAYER(m_eOwner).firstCity(&iLoop); pCity != NULL; pCity = GET_PLAYER(m_eOwner).nextCity(&iLoop) )
		{
			if (!pCity->isMatchingArea(pWaterPlot))
				continue;

			int iDistance = plotDistance(*pCivilian->plot(),*pCity->plot());
			if (iDistance<iBestDistance)
			{
				if (pCivilian->GeneratePath(pCity->plot()))
				{
					iBestDistance = iDistance;
					pBestMuster = pCity;
				}
			}
		}

		CvPlot *pMusterPlot = pBestMuster ? MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pBestMuster->plot()) : NULL;
		if(pMusterPlot != NULL)
		{
			SetMusterPlot(pMusterPlot);
			pArmy->SetXY(pMusterPlot->getX(), pMusterPlot->getY());
			SetTurnStarted(GC.getGame().getGameTurn());
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationOffensiveNavalSneakAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationOffensiveNavalSneakAttack::CvAIOperationOffensiveNavalSneakAttack()
{
}

CvAIOperationOffensiveNavalSneakAttack::~CvAIOperationOffensiveNavalSneakAttack()
{
}

CvAIOperationNukeAttack::CvAIOperationNukeAttack()
{
}

CvAIOperationNukeAttack::~CvAIOperationNukeAttack()
{
}

void CvAIOperationNukeAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	//do this before calling any FindX methods!
	Reset(iID,eOwner,eEnemy);

	CvPlot* pMuster = NULL;
	CvPlot* pTarget = FindBestTarget(&pMuster);

	SetupWithSingleArmy(pMuster,pTarget);
}

bool CvAIOperationNukeAttack::CheckTransitionToNextStage()
{
	if (m_viArmyIDs.empty())
		return false;

	CvPlot* pTargetPlot = GetTargetPlot();
	CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);

	//don't care about the operation state ... just nuke away!
	if(pTargetPlot && pArmy)
	{
		// Now get the nuke
		UnitHandle pNuke = pArmy->GetFirstUnit();
		if(pNuke)
		{
			if(pNuke->canMove() && pNuke->canNukeAt(pNuke->plot(),pTargetPlot->getX(),pTargetPlot->getY()))
			{
				pNuke->PushMission(CvTypes::getMISSION_NUKE(), pTargetPlot->getX(), pTargetPlot->getY());
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("City nuked, At X=%d, At Y=%d", pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
		}

		return true;
	}

	return false;
}

CvPlot* CvAIOperationNukeAttack::FindBestTarget(CvPlot** ppMuster) const
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit = NULL;
	CvCity* pBestCity = NULL;
	int iBestCity = 0;
	int iUnitLoop = 0;
	int iCityLoop = 0;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	TeamTypes eTeam = ownerPlayer.getTeam();
	CvTeam& ourTeam = GET_TEAM(eTeam);
	CvPlayerAI& enemyPlayer = GET_PLAYER(m_eEnemy);

	int iBlastRadius = GC.getNUKE_BLAST_RADIUS();

	// check all of our units to find the nukes
	for(pLoopUnit = ownerPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iUnitLoop))
	{
		if(pLoopUnit && pLoopUnit->canNuke(NULL))
		{
			int iUnitRange = pLoopUnit->GetRange();
			// for all cities of this enemy
			CvCity* pLoopCity;
			for(pLoopCity = enemyPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = enemyPlayer.nextCity(&iCityLoop))
			{
				if(pLoopCity)
				{
					if(plotDistance(pLoopUnit->getX(),pLoopUnit->getY(),pLoopCity->getX(),pLoopCity->getY()) <= iUnitRange)
					{
						CvPlot* pCityPlot = pLoopCity->plot();
						int iThisCityValue = pLoopCity->getPopulation();
						iThisCityValue -= pLoopCity->getDamage() / 5; // No point nuking a city that is already trashed unless it is good city

						// check to see if there is anything good or bad in the radius that we should account for

						for(int iDX = -iBlastRadius; iDX <= iBlastRadius; iDX++)
						{
							for(int iDY = -iBlastRadius; iDY <= iBlastRadius; iDY++)
							{
								CvPlot* pLoopPlot = plotXYWithRangeCheck(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY, iBlastRadius);
								if(pLoopPlot)
								{
									// who owns this plot?
									PlayerTypes ePlotOwner = pLoopPlot->getOwner();
									TeamTypes ePlotTeam = pLoopPlot->getTeam();
									// are we at war with them (or are they us)
#if defined(MOD_BALANCE_CORE)
									//Nukes have hit here already, let's not target this place again.
									if(pLoopPlot->IsFeatureFallout() && pCityPlot->getOwner() == pLoopPlot->getOwner())
									{
										iThisCityValue -= 1000;
									}
#endif
									if(ePlotOwner == m_eOwner)
									{
										iThisCityValue -= 1;
										if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
										{
											if(!pLoopPlot->IsImprovementPillaged())
											{
												iThisCityValue -= 5;
												if(pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE)  // we aren't nuking our own resources
												{
													iThisCityValue -= 1000;
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
												iThisCityValue += 2;
												if(pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE)  // we like nuking our their resources
												{
													iThisCityValue += 5;
												}
											}
										}
									}
									else if (ePlotOwner != NO_PLAYER) // this will trigger a war
									{
										iThisCityValue -= 1000;
									}

									// will we hit any units here?

									// Do we want a visibility check here?  We shouldn't know they are here.

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
											// are we at war with them (or are they us)
											if(eUnitOwner == m_eOwner)
											{
#if defined(MOD_BALANCE_CORE)
												//Let's not nuke our own units.
												iThisCityValue -= 25;
#else
												iThisCityValue -= 2;
#endif
											}
											else if(ourTeam.isAtWar(eUnitTeam))
											{
#if defined(MOD_BALANCE_CORE)
												iThisCityValue += 10;
#else
												iThisCityValue += 2;
#endif
											}
											else if (ePlotOwner != NO_PLAYER) // this will trigger a war
											{
#if defined(MOD_BALANCE_CORE)
												if(GET_PLAYER(ePlotOwner).isMajorCiv() && GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetMajorCivApproach(ePlotOwner, false) == MAJOR_CIV_APPROACH_WAR)
												{
													iThisCityValue += 100;
												}
												else if(GET_PLAYER(ePlotOwner).isMajorCiv() && GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetMajorCivApproach(ePlotOwner, false) == MAJOR_CIV_APPROACH_HOSTILE)
												{
													iThisCityValue += 10;
												}
												else
												{
#endif

												iThisCityValue -= 1000;
#if defined(MOD_BALANCE_CORE)
												}
#endif
											}
										}
									}
								}
							}
						}

						// if this is the capital
						if(pLoopCity->isCapital())
						{
							iThisCityValue *= 2;
						}

						if(iThisCityValue > iBestCity)
						{
							pBestUnit = pLoopUnit;
							pBestCity = pLoopCity;
							iBestCity = iThisCityValue;
						}
					}
				}
			}
		}
	}

	if (ppMuster)
		*ppMuster = pBestUnit ? pBestUnit->plot() : NULL;

	return pBestCity->plot();
}

bool CvAIOperationNukeAttack::VerifyOrAdjustTarget(CvArmyAI* /*pArmy*/)
{
	// See if our target is still owned by our enemy
	if(GetTargetPlot()->getOwner() != m_eEnemy)
		return false;

	return true;
}

/// Find a unit from our reserves that could serve in this operation
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, WeightedUnitIdVector&)
{
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);

	// check all of our units to find the nuke
	int iUnitLoop;
	for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iUnitLoop))
	{
		if(pLoopUnit && pLoopUnit->canNuke(NULL) && pLoopUnit->plot()==GetMusterPlot() && pLoopUnit->getArmyID()==-1)
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
bool CvAIOperationOffensiveAntiBarbarian::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	if (!pArmy)
		return false;

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

			CvCity* pMusterCity = GET_PLAYER(m_eOwner).GetClosestCity(pNewTarget,12,true);
			if (!pMusterCity)
				return false;

			SetMusterPlot( pMusterCity->plot() );

			int iUnitID = 0;
			if (pNewTarget->getNumUnitsOfAIType(UNITAI_SETTLE,iUnitID))
				m_iUnitToRescue = iUnitID;
			else if (pNewTarget->getNumUnitsOfAIType(UNITAI_WORKER,iUnitID))
				m_iUnitToRescue = iUnitID;

			// If we're traveling on a single continent, set our destination to be a few plots shy of the final target
			if (pArmy->GetArea() == GetTargetPlot()->getArea())
			{
				SPathFinderUserData data( m_eOwner, PT_GENERIC_SAME_AREA, m_eEnemy );
				SPath path = GC.GetStepFinder().GetPath( pArmy->Plot(), GetTargetPlot(), data );
				data.iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET;
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

			return true;
		}
		else
			return false;
	}
	else
		return true;
}

bool CvAIOperationCivilian::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	if (!pArmy)
		return false;

	CvPlot* pTarget = GetTargetPlot();
	if (!pTarget)
		return false;

	//if we actually do have an escort, danger is ok
	if (IsEscorted())
		return true;

	UnitHandle pCivilian = pArmy->GetFirstUnit();
	if (!pCivilian)
		return false;

	return GET_PLAYER(m_eOwner).GetPlotDanger(*pTarget,pCivilian.pointer()) < INT_MAX;
}

bool CvAIOperationCivilian::WillBeEscorted()
{
	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo( GetFormation() );

	if (thisFormation && thisFormation->getNumFormationSlotEntries()>1)
		return true;

	return false;
}

bool CvAIOperationCivilian::IsEscorted()
{
	if (m_viArmyIDs.empty())
		return false;

	CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);

	//the unit to be escorted is always the first one
	UnitHandle pCivilian = pThisArmy->GetFirstUnit();
	//the second unit would be the first escort
	UnitHandle pEscort = pThisArmy->GetNextUnit();

	return pEscort;
}

bool CvAIOperationCivilianMerchantDelegation::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	if (!CvAIOperationCivilian::VerifyOrAdjustTarget(pArmy))
		return false;

	if (!pArmy)
		return false;

	int iUnitID = pArmy->GetFirstUnitID();
	if(iUnitID == -1)
		return false;

	CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);

	if (GetTargetPlot()==NULL || !pCivilian->canTrade( GetTargetPlot() ))
		RetargetCivilian(pCivilian, pArmy);

	return (GetTargetPlot() != NULL);
}

bool CvAIOperationCivilianDiplomatDelegation::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	if (!CvAIOperationCivilian::VerifyOrAdjustTarget(pArmy))
		return false;

	if (!pArmy)
		return false;

	int iUnitID = pArmy->GetFirstUnitID();
	if(iUnitID == -1)
		return false;

	CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);

	if (GetTargetPlot()==NULL || !pCivilian->canTrade( GetTargetPlot() ))
		RetargetCivilian(pCivilian, pArmy);

	return (GetTargetPlot() != NULL);
}

bool CvAIOperationCivilianConcertTour::VerifyOrAdjustTarget(CvArmyAI* pArmy)
{
	if (!CvAIOperationCivilian::VerifyOrAdjustTarget(pArmy))
		return false;

	if (!pArmy)
		return false;

	int iUnitID = pArmy->GetFirstUnitID();
	if(iUnitID == -1)
		return false;

	CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);

	if (GetTargetPlot()==NULL || !pCivilian->canBlastTourism( GetTargetPlot() ))
		RetargetCivilian(pCivilian, pArmy);

	return (GetTargetPlot() != NULL);
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
	int iRange = 0;

#if defined(MOD_BALANCE_CORE)
	if(iTotalUnits <= 2)
	{
		iRange = 2;
	}
	else if(iTotalUnits <= 6)
	{
		iRange = 3;
	}
	else if(iTotalUnits <= 10)
	{
		iRange = 4;
	}
	else
	{
		iRange = 5;
	}
#else
	if(iTotalUnits <= 2)
	{
		iRange = 1;
	}
	else if(iTotalUnits <= 6)
	{
		iRange = 2;
	}
	else if(iTotalUnits <= 10)
	{
		iRange = 3;
	}
	else
	{
		iRange = 4;
	}
#endif

	return iRange;
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
					CvPlot* pBestTargetHere = OperationalAIHelpers::FindEnemies(ePlayer,eEnemy,DOMAIN_LAND,false,vAreas[i],pLoopCity->plot());
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
		*ppMuster = MilitaryAIHelpers::GetCoastalPlotAdjacentToTarget(pBestStart->plot());

	return pBestTarget;
}

/// Find the barbarian camp we want to eliminate
CvPlot* OperationalAIHelpers::FindBestBarbCamp(PlayerTypes ePlayer, CvPlot** ppMuster)
{
	int iPlotLoop;
	CvPlot* pBestPlot = NULL;
	CvCity* pClosestCity = NULL;
	int iBestPlotDistance = MAX_INT;

	ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();

	// look for good captured civilians of ours (settlers and workers, not missionaries) 
	// these will be even more important than just a camp
	// btw - the AI will cheat here - as a human I would use a combination of memory and intuition to find these, since our current AI has neither of these...
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);

	CvUnit* pLoopUnit = NULL;
	int iLoop;
	for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
	{
		if (pLoopUnit->GetOriginalOwner() != BARBARIAN_PLAYER && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER))
		{
			CvCity* pLoopCity;
			int iCityLoop;
			// Loop through each of our cities
			for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
			{
				if(pLoopUnit->plot()->getArea() != pLoopCity->getArea() && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canEmbark())
				{
					continue;
				}

				int iCurPlotDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pLoopUnit->getX(), pLoopUnit->getY());
					
				if (iCurPlotDistance < iBestPlotDistance)
				{
					pBestPlot = pLoopUnit->plot();
					iBestPlotDistance = iCurPlotDistance;
					pClosestCity = pLoopCity;
				}
			}
		}
	}

	if (pBestPlot == NULL)
	{
		// Look at map for Barbarian camps - don't check if they are revealed ... that's the cheating part
		for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
			if (pPlot->getImprovementType() == eBarbCamp)
			{
				CvCity* pLoopCity;
				int iCityLoop;
				// Loop through each of our cities
				for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
				{
					if(pLoopCity != NULL)
					{
						if(pPlot->getArea() != pLoopCity->getArea() && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canEmbark())
						{
							continue;
						}

						int iCurPlotDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
						if(pPlot->getArea() != pLoopCity->getArea())
						{
							iCurPlotDistance *= 2;
						}

						if (iCurPlotDistance < iBestPlotDistance)
						{
							pBestPlot = pPlot;
							iBestPlotDistance = iCurPlotDistance;
							pClosestCity = pLoopCity;
						}
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

bool OperationalAIHelpers::IsUnitSuitableForRecruitment(CvUnit* pLoopUnit, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool bMustNaval, bool bMustBeDeepWaterNaval, int& iDistance)
{
	if (!pLoopUnit->canRecruitFromTacticalAI() || pLoopUnit->getArmyID() != -1)
		return false;

	// Get raw distance to the muster point (pathfinder is too expensive)
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
	if (pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
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

	//check if the unit is engaged with the enemy ...
	if (pLoopUnit->IsEnemyInMovementRange())
		return false;

	//don't take explorers
	CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
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
	iDistance = INT_MAX;
	if (pLoopUnit->getDropRange() > 0)
	{
		if (iDistance <= pLoopUnit->getDropRange())
		{
			if (pLoopUnit->canParadropAt(pMusterPlot, pMusterPlot->getX(), pMusterPlot->getY()))
			{
				iDistance = 0;
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
							iDistance = 1;
							break;
						}
					}
				}
			}
		}
	}
	
	if (iDistance==INT_MAX)
	{
		// finally, if the unit is too far away, no deal
		int iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET | CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ZOC;
		iDistance = pLoopUnit->TurnsToReachTarget(pMusterPlot,iFlags,GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY());
		if (iDistance == INT_MAX)
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
	}

	return true;
}

CvPlot* OperationalAIHelpers::FindEnemies(PlayerTypes ePlayer, PlayerTypes eEnemy, DomainTypes eDomain, bool bHomelandOnly, int iRefArea, CvPlot* pRefPlot)
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

		if (!pLoopUnit->isInvisible(GET_PLAYER(ePlayer).getTeam(),false))
			continue;

		if (bHomelandOnly && pLoopPlot->getOwner()!=ePlayer)
			continue;

		if (iRefArea!=-1 && pLoopPlot->getArea()!=iRefArea && !pLoopPlot->isAdjacentToArea(iRefArea))
			continue;


		//a single unit is too volatile, check for a whole cluster
		int iEnemyPower = 0;

		//also take into account cargo ships, e.g. carriers
		const IDInfo* pUnitNode = pLoopPlot->headUnitNode();
		const CvUnit* pInnerLoopUnit;
		while(pUnitNode != NULL)
		{
			pInnerLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
			if(pInnerLoopUnit != NULL)
			{
				iEnemyPower += pInnerLoopUnit->GetPower();
			}
		}

		CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pLoopPlot);
		for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
		{
			const CvPlot* pNeighborPlot = aPlotsToCheck[iCount];
			if (!pNeighborPlot || !pNeighborPlot->isVisible(GET_PLAYER(ePlayer).getTeam()))
				continue;

			UnitHandle pEnemy = pNeighborPlot->getBestDefender(eEnemy,ePlayer,NULL,true);
			if (pEnemy && pEnemy->getDomainType() == DOMAIN_LAND && pEnemy->IsCombatUnit())
				iEnemyPower += pEnemy->GetPower();
		}

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
	{
		CvCity* pCity = GET_PLAYER(ePlayer).GetMilitaryAI()->GetMostThreatenedCity(0,false);
		if(pCity != NULL)
		{
			pBestPlot = pCity->plot();
		}
	}

	return pBestPlot;	
}


bool OperationalAIHelpers::NeedOceanMoves(PlayerTypes ePlayer, CvPlot* pMusterPlot, CvPlot* pTargetPlot)
{
	SPathFinderUserData data( ePlayer, PT_GENERIC_SAME_AREA, NO_PLAYER );
	data.iFlags = CvUnit::MOVEFLAG_APPROXIMATE_TARGET;
	int iOceanLength = GC.GetStepFinder().GetPathLengthInPlots(pMusterPlot, pTargetPlot, data);

	if (iOceanLength<0)
		//oh, what now? this really shouldn't happen		
		return false;

	//now try without ocean for comparison
	data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;
	int iNoOceanLength = GC.GetStepFinder().GetPathLengthInPlots(pMusterPlot, pTargetPlot, data);

	//obvious
	if (iNoOceanLength<0)
		return true;

	//let's define it this way
	if (iNoOceanLength > iOceanLength*2)
		return true;

	return false;
}
