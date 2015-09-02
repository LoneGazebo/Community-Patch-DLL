﻿/*	-------------------------------------------------------------------------------------------------------
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
CvAIOperation::CvAIOperation():m_eOwner(NO_PLAYER)
{
	Reset();
}

/// Destructor
CvAIOperation::~CvAIOperation()
{
	Uninit();
}

/// Initialize
void CvAIOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity* /*pTarget*/, CvCity* pMuster)
{
	Uninit();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	m_iDefaultArea = iDefaultArea;
	m_bShouldReplaceLossesWithReinforcements = false;

#if defined(MOD_BALANCE_CORE)
	SetStartCityPlot(pMuster ? pMuster->plot() : NULL);
#else
	SetStartCityPlot(pMuster->plot());
#endif

	// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
	BuildListOfUnitsWeStillNeedToBuild();
	GrabUnitsFromTheReserves(NULL, NULL);
}

/// Delete allocated objects
void CvAIOperation::Uninit()
{
	if(m_eOwner != NO_PLAYER)  // hopefully if this has been init'ed this should not happen
	{
		CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);

		// remove the armies (which should, in turn, free up their units for other tasks)
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* thisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
			if(thisArmy)
			{
				thisArmy->Kill();
				thisPlayer.deleteArmyAI(m_viArmyIDs[uiI]);
				DeleteArmyAI(m_viArmyIDs[uiI]);
			}
		}
	}

	// clear out the lists
	m_viArmyIDs.clear();
	m_viListOfUnitsWeStillNeedToBuild.clear();
	m_viListOfUnitsCitiesHaveCommittedToBuild.clear();

	Reset();
}

/// Reset member variables
void CvAIOperation::Reset()
{
	m_iID = 0;
	m_eOwner = NO_PLAYER;
	m_eEnemy = NO_PLAYER;
	m_iDefaultArea = -1;
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = NO_ABORT_REASON;
#if defined(MOD_BALANCE_CORE)
	m_iTargetX = INVALID_PLOT_COORD;
	m_iTargetY = INVALID_PLOT_COORD;
	m_iMusterX = INVALID_PLOT_COORD;
	m_iMusterY = INVALID_PLOT_COORD;
#else
	m_iTargetX = 0;
	m_iTargetY = 0;
	m_iMusterX = 0;
	m_iMusterY = 0;
#endif
	m_iStartCityX = -1;
	m_iStartCityY = -1;
	m_eMoveType = INVALID_AI_OPERATION_MOVE_TYPE;
	m_iLastTurnMoved = -1;
	m_viArmyIDs.clear();
}

/// How long will we wait for a recruit to show up?
int CvAIOperation::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT();
}

/// Retrieve operation's ID
int CvAIOperation::GetID() const
{
	return m_iID;
}

/// Set operation's ID
void CvAIOperation::SetID(int iID)
{
	m_iID = iID;
}

/// Set player conducting operation
void CvAIOperation::SetOwner(PlayerTypes ePlayer)
{
	m_eOwner = ePlayer;
}

/// Retrieve player conducting operation
PlayerTypes CvAIOperation::GetEnemy() const
{
	return m_eEnemy;
}

/// Set player conducting operation
void CvAIOperation::SetEnemy(PlayerTypes eEnemy)
{
	m_eEnemy = eEnemy;
}

/// Retrieve area for operation
int CvAIOperation::GetDefaultArea() const
{
	return m_iDefaultArea;
}

/// Set area for operation
void CvAIOperation::SetDefaultArea(int iDefaultArea)
{
	m_iDefaultArea = iDefaultArea;
}

/// ID of first army (-1 if no armies in operation)
int CvAIOperation::GetFirstArmyID()
{
	if(!m_viArmyIDs.empty())
	{
		return m_viArmyIDs[0];
	}
	else
	{
		return -1;
	}
}

/// Retrieve plot targeted by this operation
CvPlot* CvAIOperation::GetTargetPlot() const
{
	CvPlot* rtnValue = NULL;
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (m_iTargetX!=INVALID_PLOT_COORD && m_iTargetY!=INVALID_PLOT_COORD)
#endif
	rtnValue =  GC.getMap().plot(m_iTargetX, m_iTargetY);
	return rtnValue;
}

/// Set plot targeted by this operation
void CvAIOperation::SetTargetPlot(CvPlot* pTarget)
{
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (pTarget==NULL)
	{
		m_iTargetX = INVALID_PLOT_COORD;
		m_iTargetY = INVALID_PLOT_COORD;
		return;
	}
#endif
	m_iTargetX = pTarget->getX();
	m_iTargetY = pTarget->getY();
}

/// Retrieve plot where this operation assembles
CvPlot* CvAIOperation::GetMusterPlot() const
{
	CvPlot* rtnValue = NULL;
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (m_iMusterX!=INVALID_PLOT_COORD && m_iMusterY!=INVALID_PLOT_COORD)
#endif
	rtnValue =  GC.getMap().plot(m_iMusterX, m_iMusterY);
	return rtnValue;
}

/// Set plot where this operation assembles
void CvAIOperation::SetMusterPlot(CvPlot* pMuster)
{
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (pMuster==NULL)
	{
		m_iMusterX = INVALID_PLOT_COORD;
		m_iMusterX = INVALID_PLOT_COORD;
		return;
	}
#endif
	m_iMusterX = pMuster->getX();
	m_iMusterY = pMuster->getY();
}

/// Retrieve city where this operation starts
CvPlot* CvAIOperation::GetStartCityPlot() const
{
	CvPlot* rtnValue = NULL;
	rtnValue =  GC.getMap().plot(m_iStartCityX, m_iStartCityY);
	return rtnValue;
}

/// Set city where this operation starts
void CvAIOperation::SetStartCityPlot(CvPlot* pStartCity)
{
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (pStartCity==NULL)
	{
		m_iStartCityX = INVALID_PLOT_COORD;
		m_iStartCityX = INVALID_PLOT_COORD;
		return;
	}
#endif

	m_iStartCityX = pStartCity->getX();
	m_iStartCityY = pStartCity->getY();
}

int CvAIOperation::GetGatherTolerance(CvArmyAI* pArmy, CvPlot* pPlot) const
{
	CvTacticalAnalysisCell* pCell;
	CvTacticalAnalysisMap* pMap = GC.getGame().GetTacticalAnalysisMap();
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
		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
				pCell = pMap->GetCell(iPlotIndex);

				if((IsMixedLandNavalOperation() || IsAllNavalOperation()) && !pCell->CanUseForOperationGatheringCheckWater(true))
				{
					continue;
				}

				if((IsMixedLandNavalOperation() || IsAllNavalOperation()) && !pArmy->IsAllOceanGoing() && pCell->IsOcean())
				{
					continue;
				}

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
			iRtnValue = 3;
		}
	}

	return iRtnValue;
}

/// Factory method: you are responsible for cleaning up the operation created by this routine!
CvAIOperation* CvAIOperation::CreateOperation(AIOperationTypes eAIOperationType, PlayerTypes ePlayer)
{
	switch(eAIOperationType)
	{
	case AI_OPERATION_BASIC_CITY_ATTACK:
		return FNEW(CvAIOperationBasicCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return FNEW(CvAIOperationDestroyBarbarianCamp(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_FOUND_CITY:
		return FNEW(CvAIOperationFoundCity(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_MERCHANT_DELEGATION:
		return FNEW(CvAIOperationMerchantDelegation(), c_eCiv5GameplayDLL, 0);
#if defined(MOD_DIPLOMACY_CITYSTATES)
	case AI_OPERATION_DIPLOMAT_DELEGATION:
		return FNEW(CvAIOperationDiplomatDelegation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_ALLY_DEFENSE:
		return FNEW(CvAIOperationAllyDefense(), c_eCiv5GameplayDLL, 0);
#endif
	case AI_OPERATION_CONCERT_TOUR:
		return FNEW(CvAIOperationConcertTour(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_BOMBARDMENT:
		return FNEW(CvAIOperationNavalBombardment(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return FNEW(CvAIOperationNavalSuperiority(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PURE_NAVAL_CITY_ATTACK:
		return FNEW(CvAIOperationPureNavalCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_COLONIZE:
		return FNEW(CvAINavalEscortedOperation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_QUICK_COLONIZE:
		return FNEW(CvAIOperationQuickColonize(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PILLAGE_ENEMY:
		return FNEW(CvAIOperationPillageEnemy(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_CLOSE_DEFENSE:
		return FNEW(CvAIOperationCityCloseDefense(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_RAPID_RESPONSE:
		return FNEW(CvAIOperationRapidResponse(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_SNEAK_CITY_ATTACK:
	{
		if(GC.getGame().getGameTurn() < 50 && GET_PLAYER(ePlayer).GetDiplomacyAI()->GetBoldness() >= 5)
		{
			return FNEW(CvAIOperationQuickSneakCityAttack(), c_eCiv5GameplayDLL, 0);
		}
		return FNEW(CvAIOperationSneakCityAttack(), c_eCiv5GameplayDLL, 0);
	}
	case AI_OPERATION_SMALL_CITY_ATTACK:
		return FNEW(CvAIOperationSmallCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_STATE_ATTACK:
		return FNEW(CvAIOperationCityStateAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_ATTACK:
		return FNEW(CvAIOperationNavalAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		return FNEW(CvAIOperationNavalSneakAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_STATE_NAVAL_ATTACK:
		return FNEW(CvAIOperationNavalCityStateAttack(), c_eCiv5GameplayDLL, 0);
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
	if(iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
	}
#if defined(MOD_BALANCE_CORE)
	else
	{
		if(iAreaID != -1)
		{
			CvArea* pArea = GC.getMap().getArea(iAreaID);
			if(pArea != NULL)
			{
				if(pArea->isWater() && !m_viListOfUnitsWeStillNeedToBuild.empty())
				{
					thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
				}
			}
		}
	}
#endif
	return thisSlot;
}

/// Called by a city when it decides to build a unit
OperationSlot CvAIOperation::CommitToBuildNextUnit(int iAreaID, int iTurns, CvCity* pCity)
{
	OperationSlot thisSlot;
	if(iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
		m_viListOfUnitsWeStillNeedToBuild.pop_back();
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
		m_viListOfUnitsWeStillNeedToBuild.push_back(*iter);
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
		if(m_viListOfUnitsWeStillNeedToBuild.empty() && m_eCurrentState  == AI_OPERATION_STATE_RECRUITING_UNITS)
		{
			m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;

			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				if(pThisArmy)
				{
					pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
				}
			}
		}
		return true;
	}
	return false;
}

/// Assigns available units to our operation. Returns true if all needed units assigned.
#if defined(MOD_BALANCE_CORE)
bool CvAIOperation::FillWithUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot)
{
	bool rtnValue = true;
	bool success;
	std::vector<OperationSlot>::iterator it;
	bool bRequired;

	// Copy over the list
	std::vector<OperationSlot> secondList;
	for(it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
	{
		secondList.push_back(*it);
	}

	// Clear main list
	m_viListOfUnitsWeStillNeedToBuild.clear();

	for(it = secondList.begin(); it != secondList.end(); ++it)
	{
		success = FindBestFitRecruitUnit(*it, pMusterPlot, pTargetPlot, &bRequired);

		// If any fail, check to see if they were required
		if(!success)
		{
			if(bRequired)
			{
				// Return false to say that operation is not ready to roll yet
				rtnValue = false;

				// And add them back to the list of units needed
				m_viListOfUnitsWeStillNeedToBuild.push_back(*it);
			}
			else
			{
				CvArmyAI* pArmy;
				pArmy = GET_PLAYER(m_eOwner).getArmyAI(it->m_iArmyID);
				if(pArmy)
				{
					pArmy->SetEstimatedTurn(it->m_iSlotID, ARMYSLOT_NOT_INCLUDING_IN_OPERATION);
				}
			}
		}
	}

	return rtnValue;
}
#endif
bool CvAIOperation::GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot)
{
	bool rtnValue = true;
	bool success;
	std::vector<OperationSlot>::iterator it;
	bool bRequired;

	// Copy over the list
	std::vector<OperationSlot> secondList;
	for(it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
	{
		secondList.push_back(*it);
	}

	// Clear main list
	m_viListOfUnitsWeStillNeedToBuild.clear();

	for(it = secondList.begin(); it != secondList.end(); ++it)
	{
		success = FindBestFitReserveUnit(*it, pMusterPlot, pTargetPlot, &bRequired);

		// If any fail, check to see if they were required
		if(!success)
		{
			if(bRequired)
			{
				// Return false to say that operation is not ready to roll yet
				rtnValue = false;

				// And add them back to the list of units needed
				m_viListOfUnitsWeStillNeedToBuild.push_back(*it);
			}
			else
			{
				CvArmyAI* pArmy;
				pArmy = GET_PLAYER(m_eOwner).getArmyAI(it->m_iArmyID);
				if(pArmy)
				{
					pArmy->SetEstimatedTurn(it->m_iSlotID, ARMYSLOT_NOT_INCLUDING_IN_OPERATION);
				}
			}
		}
	}

	return rtnValue;
}

#if defined(MOD_BALANCE_CORE)
//see if the target is still current or if there is a better one
bool CvAIOperation::CheckTarget()
{
	bool retval = true;
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
		//name is misleading, this is basically hook to check if the target is still where we think it is and make adjustments if required. return value is irrelevant
		retval &= ArmyMoved(pThisArmy);
	}
	return retval;
}
#endif

/// See if armies are ready to hand off units to the tactical AI (and do so if ready)
bool CvAIOperation::CheckOnTarget()
{
	int iUnitID;
#if defined(MOD_BALANCE_CORE_SETTLER)
	CvUnit* pCivilian = NULL;
	CvUnit* pEscort = NULL;
	CvPlot* pCivilianPlot = NULL;
#else
	CvUnit* pCivilian;
	CvPlot* pCivilianPlot = NULL;
	CvPlot* pEscortPlot;
#endif

	if(GetFirstArmyID() == -1)
	{
		return false;
	}

	switch(m_eMoveType)
	{
		{
		case AI_OPERATION_MOVETYPE_SINGLE_HEX:
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
#if defined(MOD_BALANCE_CORE)
			if(pThisArmy && pThisArmy->Plot() != NULL && pThisArmy->GetGoalPlot() != NULL)
			{
				FillWithUnitsFromTheReserves(pThisArmy->Plot(), pThisArmy->GetGoalPlot());
			}
#endif
			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				iUnitID = pThisArmy->GetFirstUnitID();
				if(iUnitID != -1)
				{
					pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
#if defined(MOD_BALANCE_CORE_SETTLER)
					if(pCivilian != NULL)
					{
						pCivilianPlot = pCivilian->plot();
					}
				}
				if(pCivilianPlot != NULL && (m_eCurrentState==AI_OPERATION_STATE_MOVING_TO_TARGET || m_eCurrentState==AI_OPERATION_STATE_AT_TARGET) && pCivilianPlot == GetTargetPlot())
#else
					pCivilianPlot = pCivilian->plot();
				}
				if( m_eCurrentState == AI_OPERATION_STATE_MOVING_TO_TARGET && pCivilianPlot == GetTargetPlot())
#endif
				{
					ArmyInPosition(pThisArmy);
					return true;
				}
				else if(m_eCurrentState == AI_OPERATION_STATE_GATHERING_FORCES || m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS)
				{
					if(pThisArmy->GetNumSlotsFilled() == 1)
					{
						ArmyInPosition(pThisArmy);
						return true;
					}
					else
					{
#ifdef MOD_BALANCE_CORE_SETTLER
						pEscort = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID());
						if (pEscort && pCivilianPlot == pEscort->plot())
						{
							m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
							pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
							return true;
						}
#else
						pEscortPlot = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID())->plot();
						if(pCivilianPlot == pEscortPlot)
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
#endif
					}
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an escort operation with no units in it.  Show Ed and send save.");
			}
			break;
		}

	case AI_OPERATION_MOVETYPE_ENEMY_TERRITORY:
	{
		// Let each army perform its own check
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
#if defined(MOD_BALANCE_CORE)
			if(pThisArmy && pThisArmy->Plot() != NULL && pThisArmy->GetGoalPlot() != NULL)
			{
				FillWithUnitsFromTheReserves(pThisArmy->Plot(), pThisArmy->GetGoalPlot());
			}
#endif
			CvPlot* pCenterOfMass;

			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				switch(m_eCurrentState)
				{
				case AI_OPERATION_STATE_GATHERING_FORCES:
					{
						int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
						pCenterOfMass = pThisArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
						if(pCenterOfMass && GetMusterPlot() &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
					break;
				case AI_OPERATION_STATE_MOVING_TO_TARGET:
					{
						int iTargetTolerance = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
						pCenterOfMass = pThisArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
#if defined(MOD_BALANCE_CORE)
						if(pCenterOfMass && pThisArmy->GetGoalPlot() != NULL && 
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY()) <= iTargetTolerance &&
							pThisArmy->GetFurthestUnitDistance(pThisArmy->GetGoalPlot()) <= (iTargetTolerance * 3 / 2))
#else
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= iTargetTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3 / 2))
#endif
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
					break;
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an army operation with no units in it.  Show Ed and send save.");
			}
		}
		break;
	}

	case AI_OPERATION_MOVETYPE_NAVAL_ESCORT:
	{
		// Let each army perform its own check
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
#if defined(MOD_BALANCE_CORE)
			if(pThisArmy && pThisArmy->Plot() != NULL && pThisArmy->GetGoalPlot() != NULL)
			{
				FillWithUnitsFromTheReserves(pThisArmy->Plot(), pThisArmy->GetGoalPlot());
			}
#endif
			CvPlot* pCenterOfMass;

			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
#if defined(MOD_BALANCE_CORE)
				iUnitID = pThisArmy->GetFirstUnitID();
				if(iUnitID != -1)
				{
					pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if(pCivilian != NULL && pCivilian->isFound())
					{
						pCivilianPlot = pCivilian->plot();
					}
				}
#endif
				switch(m_eCurrentState)
				{
				case AI_OPERATION_STATE_GATHERING_FORCES:
					{
#if defined(MOD_BALANCE_CORE)
						if(pCivilian && pCivilianPlot != NULL)
						{
							//Triplicate in the event that another of the 'escorts' is here.
							pEscort = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID());
							if (pEscort && pCivilianPlot == pEscort->plot())
							{
								ArmyInPosition(pThisArmy);
								return true;
							}
							pEscort = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID());
							if (pEscort && pCivilianPlot == pEscort->plot())
							{
								ArmyInPosition(pThisArmy);
								return true;
							}
							pEscort = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID());
							if (pEscort && pCivilianPlot == pEscort->plot())
							{
								ArmyInPosition(pThisArmy);
								return true;
							}
						}
						else
						{
#endif
						int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
						pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
						if(pCenterOfMass && GetMusterPlot() &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
#if defined(MOD_BALANCE_CORE)
						}
#endif
					}
					break;
				case AI_OPERATION_STATE_MOVING_TO_TARGET:
					{
#if defined(MOD_BALANCE_CORE)
						if(pCivilianPlot != NULL && pCivilianPlot == GetTargetPlot())
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
						else
						{
#endif
						int iTargetTolerance = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
						pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
#if defined(MOD_BALANCE_CORE)
						if(pCenterOfMass && pThisArmy->GetGoalPlot() != NULL &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pThisArmy->GetGoalX(), pThisArmy->GetGoalY()) <= (iTargetTolerance  * 2) &&
							pThisArmy->GetFurthestUnitDistance(pThisArmy->GetGoalPlot()) <= (iTargetTolerance * 3))
#else
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= (iTargetTolerance  * 2) &&
							pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3))
#endif
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
#if defined(MOD_BALANCE_CORE)
						}
#endif
					}
					break;
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an army operation with no units in it.  Show Ed and send save.");
			}
		}
		break;
	}

	case AI_OPERATION_MOVETYPE_FREEFORM_NAVAL:
	{
		// Let each army perform its own check
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			CvPlot* pCenterOfMass;
			int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());

			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				switch(m_eCurrentState)
				{
				case AI_OPERATION_STATE_GATHERING_FORCES:
				case AI_OPERATION_STATE_MOVING_TO_TARGET:

					// We want to recompute a new target each turn.  So call ArmyInPosition() regardless of return status
					ArmyInPosition(pThisArmy);

					pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
#if defined(MOD_BALANCE_CORE)
					if(pCenterOfMass && pThisArmy->GetGoalPlot() != NULL &&
					        plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pThisArmy->GetGoalX(), pThisArmy->GetGoalY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(pThisArmy->GetGoalPlot()) <= (iGatherTolerance * 3 / 2))
#else
					if(pCenterOfMass &&
					        plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= iGatherTolerance &&
					        pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
#endif
					{
						return true;
					}
					break;
				}
			}
			else
			{
				CvAssertMsg(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS || m_eCurrentState == AI_OPERATION_STATE_ABORTED || m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH,
				            "Found an army operation with no units in it.  Show Ed and send save.");
			}
		}
		break;
	}
	}
	return false;
}

/// Processing if army is now at target
bool CvAIOperation::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, we're all set to move out
	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
		bStateChanged = true;
		pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	}
	break;

	// If we are moving to our target, check and see if we are there
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if(pArmy->Plot() == GetTargetPlot())
		{
			m_eCurrentState = AI_OPERATION_STATE_AT_TARGET;
			bStateChanged = true;
		}
	}
	break;
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
		break;
	};

	return bStateChanged;
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

#if defined(MOD_BALANCE_CORE)
			//this is strange, check did not work, goal plot was not null but invalid
			if (pArmy==NULL || pArmy->GetGoalX()==-1 || pArmy->GetGoalY()==-1)
				return 0;
#endif

			if (pArmy->GetGoalPlot())
			{
				int iDistanceMusterToTarget;
				int iDistanceCurrentToTarget;

				CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);

				// Use the step path finder to compute distance
				iDistanceMusterToTarget = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, GetMusterPlot(), pArmy->GetGoalPlot());
				iDistanceCurrentToTarget = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pCenterOfMass, pArmy->GetGoalPlot());

				if(iDistanceMusterToTarget <= 0)
				{
					return 0;
				}
	
				// If within 2 of the final goal, consider ourselves there
				else if (iDistanceCurrentToTarget <= 2)
				{
					return 100;
				}

				else
				{
					int iTempValue = 100 - (100 * iDistanceCurrentToTarget / iDistanceMusterToTarget);
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
	UnitHandle pUnit;

	// Mark units in successful operation
	if(m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH)
	{
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

			pUnit = pArmy->GetFirstUnit();
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
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = eReason;
}

/// Perform the deletion of this operation
void CvAIOperation::Kill(AIOperationAbortReason eReason)
{
	int iID = m_iID;
	PlayerTypes eOwner = GetOwner();

	if (m_eAbortReason == NO_ABORT_REASON)
	{
		m_eAbortReason = eReason;
	}

	LogOperationEnd();
	Uninit();
	GET_PLAYER(eOwner).deleteAIOperation(iID);
}

/// Update operation for the next turn
void CvAIOperation::DoTurn()
{
	LogOperationStatus();
}

/// Delete the operation if marked to go away
bool CvAIOperation::DoDelayedDeath()
{
	if(ShouldAbort())
	{
		if (m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH)
		{
			Kill(AI_ABORT_SUCCESS);
		}
		else
		{
			Kill(AI_ABORT_KILLED);
		}
		return true;
	}

	return false;
}

/// Find the area where our operation is occurring
CvCity* CvAIOperation::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	CvCity* pCapitalCity;
	CvArea* pLoopArea;
	int iLoop;
	int iMyCities;
	int iEnemyCities;
	int iBestTotal = 0;
	CvArea* pBestArea = NULL;
	CvCity* pBestCity = NULL;

	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);

	// Do we still have a capital?
	pCapitalCity = thisPlayer.getCapitalCity();

	if(pCapitalCity != NULL)
	{
		return pCapitalCity;
	}

	// No capital, find the area with the most combined cities between us and our enemy (and need at least 1 from each)
	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if(pLoopArea->isWater())
		{
			continue;
		}

		iMyCities = pLoopArea->getCitiesPerPlayer(GetOwner());
		if(iMyCities > 0)
		{
			if(m_eEnemy != NO_PLAYER && m_eEnemy != BARBARIAN_PLAYER)
			{
				iEnemyCities = pLoopArea->getCitiesPerPlayer(m_eEnemy);
				if(iEnemyCities == 0)
				{
					continue;
				}
			}
			else
			{
				iEnemyCities = 0;
			}

			if((iMyCities + iEnemyCities) > iBestTotal)
			{
				iBestTotal = iMyCities + iEnemyCities;
				pBestArea = pLoopArea;
			}
		}
	}

	if(pBestArea != NULL)
	{
		// Know which continent to use, now use our largest city there as the start city
		CvCity* pCity;
		iBestTotal = 0;
		for(pCity = thisPlayer.firstCity(&iLoop); pCity != NULL; pCity = thisPlayer.nextCity(&iLoop))
		{
			if(pCity->getArea() == pBestArea->GetID())
			{
				if(pCity->getPopulation() > iBestTotal)
				{
					iBestTotal = pCity->getPopulation();
					pBestCity = pCity;
				}
			}
		}
		return pBestCity;
	}
	else
	{
		return NULL;
	}
}

/// Delete an army associated with this operation (by ID)
bool CvAIOperation::DeleteArmyAI(int iID)
{
	std::vector<int>::iterator iter;
	for(iter = m_viArmyIDs.begin(); iter != m_viArmyIDs.end(); ++iter)
	{
		if(*iter == iID)
		{
			m_viArmyIDs.erase(iter);
			return true;
		}
	}
	return false;
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
	}
	break;

	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:
	{
		// If down below half strength, abort
		CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(iArmyID);
		CvMultiUnitFormationInfo* pkFormation = GC.getMultiUnitFormationInfo(pThisArmy->GetFormationIndex());
		if(pkFormation)
		{
			if(pThisArmy->GetNumSlotsFilled() <=0 ||
			        pThisArmy->GetNumSlotsFilled() < pkFormation->getNumFormationSlotEntries() / 2)
			{
				// Abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_HALF_STRENGTH;
			}
		}
	}
	break;
	};
}

/// Pick this turn's desired "center of mass" for the army
CvPlot* CvAIOperation::ComputeCenterOfMassForTurn(CvArmyAI* pArmy, CvPlot **ppClosestCurrentCOMonPath) const
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
			CvPlot* pCenterOfMass = 0;
			CvPlot* pLastTurnArmyPlot = 0;
			CvAStarNode* pNode1 = 0;
			CvAStarNode* pNode2 = 0;
			int iLastNodeIndex = 0;
			FStaticVector<CvAStarNode*, SAFE_ESTIMATE_MAX_PATH_LEN, true, c_eCiv5GameplayDLL, 0> m_NodesOnPath;

			m_NodesOnPath.clear();

			// Is goal a city and we're a naval operation?  If so, go just offshore.
			CvPlot *pGoalPlot = pArmy->GetGoalPlot();
			if (!pGoalPlot->isWater() && IsAllNavalOperation())
			{
				pGoalPlot = kPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pGoalPlot, pArmy);
#if defined(MOD_BALANCE_CORE)
				if(pGoalPlot == NULL)
				{
					for(int iDX = -2; iDX <= 2; iDX++)
					{
						for(int iDY = -2; iDY <= 2; iDY++)
						{
							CvPlot* pLoopPlot = plotXYWithRangeCheck(pGoalPlot->getX(), pGoalPlot->getY(), iDX, iDY, 2);
							if(pLoopPlot && pLoopPlot->isWater() && !pLoopPlot->isLake())
							{
								pGoalPlot = pLoopPlot;
								break;
							}
						}
					}
				}
#endif
			}

			pLastTurnArmyPlot = pArmy->Plot();
			pCenterOfMass = pArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
			if (pLastTurnArmyPlot && pCenterOfMass && pGoalPlot)
			{
				// Push center of mass forward a number of hexes equal to average movement
				GC.getStepFinder().SetData(&m_eEnemy);
				GC.getStepFinder().SetDestValidFunc(NULL); // remove the area check
				GC.getStepFinder().SetValidFunc(StepValidAnyArea); // remove the area check
				bool bFound = GC.getStepFinder().GeneratePath(pCenterOfMass->getX(), pCenterOfMass->getY(), pGoalPlot->getX(), pGoalPlot->getY(), m_eOwner, false);
				GC.getStepFinder().SetValidFunc(StepValid); // remove the area check
				GC.getStepFinder().SetDestValidFunc(StepDestValid); // restore the area check
				if (bFound)
				{
					pNode1 = GC.getStepFinder().GetLastNode();

					// Starting at the end, loop through the entire path
					while (pNode1)
					{
						m_NodesOnPath.push_back(pNode1);
						pNode1 = pNode1->m_pParent;
					}

					iLastNodeIndex = m_NodesOnPath.size() - 1;

					// Move back up path from best node a number of spaces equal to army's movement rate + 1
					int iJumpAhead = pArmy->GetMovementRate() + 1;
					int iNode1Index = max(0, iLastNodeIndex - iJumpAhead);
					int iNode2Index = min(iNode1Index + 2, iLastNodeIndex);
					pNode1 = m_NodesOnPath[iNode1Index];
					pNode2 = m_NodesOnPath[iNode2Index];
					
					pRtnValue = GC.getMap().plot(pNode1->m_iX, pNode1->m_iY);
					*ppClosestCurrentCOMonPath = GC.getMap().plot(pNode2->m_iX, pNode2->m_iY);
				}
				else
				{
					OutputDebugString( CvString::format( "cannot find a step path from %d,%d to %d,%d\n", pCenterOfMass->getX(), pCenterOfMass->getY(), pGoalPlot->getX(), pGoalPlot->getY() ).c_str() );
					// Can't plot a path, probably due to change of control of hexes.  Will probably abort the operation
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
	CvCity* pCity = GetOperationStartCity();

	if(!m_viListOfUnitsWeStillNeedToBuild.empty() && pCity != NULL && pCity->getOwner() == m_eOwner)
	{
		OperationSlot thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
		CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(pArmy->GetFormationIndex());
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisSlot.m_iSlotID);

		CvUnit* pUnit = GET_PLAYER(m_eOwner).GetMilitaryAI()->BuyEmergencyUnit((UnitAITypes)thisSlotEntry.m_primaryUnitType, pCity);
		if(pUnit != NULL)
		{
			pArmy->AddUnit(pUnit->GetID(), thisSlot.m_iSlotID);
			m_viListOfUnitsWeStillNeedToBuild.pop_back();
			if (m_viListOfUnitsWeStillNeedToBuild.size() == 0 && m_viListOfUnitsCitiesHaveCommittedToBuild.size() == 0 && m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS)
			{
				m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
				{
					CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
					if(pThisArmy)
					{
						pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					}
				}
			}
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
	kStream >> m_iDefaultArea;
	kStream >> m_bShouldReplaceLossesWithReinforcements;
	kStream >> m_eMoveType;
	kStream >> m_iTargetX;
	kStream >> m_iTargetY;
	kStream >> m_iMusterX;
	kStream >> m_iMusterY;
	kStream >> m_iStartCityX;
	kStream >> m_iStartCityY;
	kStream >> m_iLastTurnMoved;
	kStream >> m_viArmyIDs;
	kStream >> m_viListOfUnitsWeStillNeedToBuild;
	kStream >> m_viListOfUnitsCitiesHaveCommittedToBuild;
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
	kStream << m_iDefaultArea;
	kStream << m_bShouldReplaceLossesWithReinforcements;
	kStream << m_eMoveType;
	kStream << m_iTargetX;
	kStream << m_iTargetY;
	kStream << m_iMusterX;
	kStream << m_iMusterY;
	kStream << m_iStartCityX;
	kStream << m_iStartCityY;
	kStream << m_iLastTurnMoved;
	kStream << m_viArmyIDs;
	kStream << m_viListOfUnitsWeStillNeedToBuild;
	kStream << m_viListOfUnitsCitiesHaveCommittedToBuild;
}

#if defined(MOD_BALANCE_CORE)
const char* CvAIOperation::GetInfoString()
{
	CvString strTemp0, strTemp1, strTemp2, strTemp3;
	strTemp0 = GetOperationName();
	strTemp1.Format(" / Target at %d,%d / Muster at %d,%d / ", m_iTargetX, m_iTargetY, m_iMusterX, m_iMusterY );

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_ABORTED:
		strTemp2 = "Aborted";
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

	int iUnitsToBeBuilt = int(m_viListOfUnitsWeStillNeedToBuild.size() + m_viListOfUnitsCitiesHaveCommittedToBuild.size());
	int iUnitsInOperation = 0;
	int iUnitsMissing = 0;
	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* thisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
		if(thisArmy)
		{
			iUnitsMissing += thisArmy->GetNumFormationEntries() - thisArmy->GetNumSlotsFilled();
			iUnitsInOperation += thisArmy->GetNumSlotsFilled();
		}
	}
	strTemp3.Format(" (c%d-m%d-b%d)", iUnitsInOperation, iUnitsMissing, iUnitsToBeBuilt);

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
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			strTemp1.Format("Started, Army: %d, Units Recruited: %d, Max Formation Size: %d, ", pThisArmy->GetID(), pThisArmy->GetNumSlotsFilled(), pThisArmy->GetNumFormationEntries());
		}

		strOutBuf = strBaseString + strTemp1;
		switch(m_eCurrentState)
		{
		case AI_OPERATION_STATE_ABORTED:
			strTemp2 = "Aborted";
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
		case AI_OPERATION_COLONIZE:
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
			
			strOutBuf = GetOperationName() + ", ";
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
void CvAIOperation::LogOperationStatus()
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
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		switch(m_eCurrentState)
		{
		case AI_OPERATION_STATE_ABORTED:
			strTemp = "Aborted";
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp = "";
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Recruiting Units, Army: %d, Muster Turn: %d, SLOT DETAIL:, ", pThisArmy->GetID(), pThisArmy->GetTurnAtNextCheckpoint());
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
					else if(pSlot->HasStartedOnOperation())
					{
						UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("Gathering at (%d-%d), ", pThisUnit->getX(), pThisUnit->getY());
						}
					}
					else
					{
						UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if(pThisUnit)
						{
							szTemp2.Format("%s - Turn %d, ", pThisUnit->getName().GetCString(), pSlot->GetTurnAtCheckpoint());
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
						szTemp2.Format("%s at (%d-%d),", pThisUnit->getName().GetCString(), pThisUnit->getX(), pThisUnit->getY());
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
						szTemp2.Format("%s at (%d-%d),", pThisUnit->getName().GetCString(), pThisUnit->getX(), pThisUnit->getY());
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
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";
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
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

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
		case AI_OPERATION_COLONIZE:
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
			strOutBuf = GetOperationName() + ", ";
			strOutBuf += strTemp;
			GET_PLAYER(m_eOwner).GetMilitaryAI()->LogMilitarySummaryMessage(strOutBuf);
		}
	}
}

/// Build log filename
CvString CvAIOperation::GetLogFileName(CvString& playerName) const
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

/// Figure out the initial rally point
CvPlot* CvAIOperation::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
{
	CvPlot* pMusterPt = NULL;
	CvPlot* pStartCityPlot;
	CvCity* pStartCity;

	pStartCity = GetOperationStartCity();
	if(pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if(pStartCityPlot != NULL)
		{
			// Generate a step path from our start to the target
			pMusterPt = GC.getStepFinder().GetLastOwnedPlot(GetOwner(), GetEnemy(), pStartCityPlot, pThisArmy->GetGoalPlot());

			// Should find a space we own
			if(pMusterPt == NULL)
			{
				return NULL;
			}
			else
			{
				SetMusterPlot(pMusterPt);
			}
		}
	}

	return pMusterPt;
}

class CvOperationSearchUnit
{
public:
	CvOperationSearchUnit();
	CvOperationSearchUnit(CvUnit* pkUnit, int iDistance) { m_pkUnit = pkUnit; m_iDistance = iDistance; };

	bool operator<(const CvOperationSearchUnit& kUnit) const
	{
		return (GetDistance() < kUnit.GetDistance());
	}

	// Accessors
	void SetUnit(CvUnit* pkUnit)
	{
		m_pkUnit = pkUnit;
	};
	CvUnit* GetUnit() const
	{
		return m_pkUnit;
	};
	void SetDistance(int iDistance)
	{
		m_iDistance = iDistance;
	};
	int GetDistance() const
	{
		return m_iDistance;
	};
private:
	int		m_iDistance;
	CvUnit* m_pkUnit;
};

typedef FStaticVector< CvOperationSearchUnit, 128, true, c_eCiv5GameplayDLL > CvOperationSearchUnitList;

static CvUnit* GetClosestUnit(CvOperationSearchUnitList& kSearchList, CvPlot* pkMusterPlot, CvPlot* pkTarget, bool bNeedToCheckTarget)
{
#if defined(MOD_BALANCE_CORE)
	if (pkMusterPlot == NULL || pkTarget == NULL)
	{
		return NULL;
	}
	CvUnit* pkBestUnit = NULL;
	if (kSearchList.size() > 0)
	{
		std::stable_sort(kSearchList.begin(), kSearchList.end());

		int iBestDistance = MAX_INT;
		int iBestStrength = 0;
		CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();
		for (CvOperationSearchUnitList::iterator itr = kSearchList.begin(); itr != kSearchList.end(); ++itr)
		{
			CvUnit *pkLoopUnit = (*itr).GetUnit();

			if(pkLoopUnit->plot() == NULL)
			{
				continue;
			}
			CvPlot* pNavalMuster = NULL;
			if(pkLoopUnit->getDomainType() == DOMAIN_SEA)
			{		
				// Mixed naval operation targeting a city?   Change target (but only for the naval units).
				if(pkMusterPlot != NULL)
				{
					if(!pkMusterPlot->isWater())
					{
						pNavalMuster = GET_PLAYER(pkLoopUnit->getOwner()).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pkMusterPlot, NULL);
						if(pNavalMuster == NULL)
						{
							continue;
						}
					}
					else
					{
						pNavalMuster = pkMusterPlot;
					}
				}
			}
			int iPathDistance = MAX_INT;
			if(pNavalMuster != NULL)
			{
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pNavalMuster))
				{
					continue;
				}

				iPathDistance = kPathFinder.GetPathLength();
			
				if(bNeedToCheckTarget)
				{
					CvPlot* pNavalTarget = NULL;
					if(pkLoopUnit->getDomainType() == DOMAIN_SEA)
					{		
						// Mixed naval operation targeting a city?   Change target (but only for the naval units).
						if(pkTarget != NULL)
						{
							if(!pkTarget->isWater())
							{
								pNavalTarget = GET_PLAYER(pkLoopUnit->getOwner()).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pkTarget, NULL);
								if(pNavalTarget == NULL)
								{
									continue;
								}
							}
							else
							{
								pNavalTarget = pkTarget;
							}
						}
					}
					if(pNavalTarget != NULL)
					{
						if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pNavalTarget))
						{
							continue;
						}
					}
				}
			}
			else
			{
				int iPathDistance = MAX_INT;
			
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkMusterPlot))
				{
					continue;
				}

				iPathDistance = kPathFinder.GetPathLength();
			
				if(bNeedToCheckTarget)
				{
					if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkTarget))
					{
						continue;
					}
				}
			}

			if (pkLoopUnit->getDropRange() > 0)
			{
				if (plotDistance(pkLoopUnit->getX(), pkLoopUnit->getY(), pkMusterPlot->getX(), pkMusterPlot->getY()) <= pkLoopUnit->getDropRange())
				{
					if (pkLoopUnit->canParadropAt(pkMusterPlot, pkMusterPlot->getX(), pkMusterPlot->getY()))
					{
						iPathDistance = 0;
					}
					else
					{
						CvPlot* pAdjacentPlot;
						for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
						{
							pAdjacentPlot = plotDirection(pkMusterPlot->getX(), pkMusterPlot->getY(), ((DirectionTypes)jJ));
							if (pAdjacentPlot != NULL)
							{
								if (pkLoopUnit->canParadropAt(pAdjacentPlot, pAdjacentPlot->getX(), pAdjacentPlot->getY()))
								{
									iPathDistance = MIN(1, iPathDistance);
									break;
								}
							}
						}
					}
				}
			}

			if (iPathDistance < iBestDistance)
			{
				pkBestUnit = pkLoopUnit;
				iBestDistance = iPathDistance;
				iBestStrength = pkLoopUnit->GetBaseCombatStrengthConsideringDamage();
			}
			else if (iPathDistance == iBestDistance)
			{
				if (pkBestUnit)
				{
					// Civilian units compare HP
					if (!pkBestUnit->IsCombatUnit() && !pkLoopUnit->IsCombatUnit())
					{
						if (pkLoopUnit->GetCurrHitPoints() > pkBestUnit->GetCurrHitPoints())
						{
							pkBestUnit = pkLoopUnit;
						}
					}
					// Military units compare strength
					else
					{
						int iLoopStrength = pkLoopUnit->GetBaseCombatStrengthConsideringDamage();
						if (iLoopStrength > iBestStrength)
						{
							pkBestUnit = pkLoopUnit;
							iBestStrength = iLoopStrength;
						}
					}
				}
				else
				{
					pkBestUnit = pkLoopUnit;
					iBestStrength = pkLoopUnit->GetBaseCombatStrengthConsideringDamage();
				}
			}
		}
	}
	return pkBestUnit;
#else
	CvUnit* pkBestUnit = NULL;
	if (kSearchList.size())
	{
		std::stable_sort(kSearchList.begin(), kSearchList.end());

		int iBestDistance = MAX_INT;
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_GET_STRONGEST)
		int iBestStrength = 0;
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_GET_STRONGEST
		CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();
		for (CvOperationSearchUnitList::iterator itr = kSearchList.begin(); itr != kSearchList.end(); ++itr)
		{
			CvUnit *pkLoopUnit = (*itr).GetUnit();
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP)
#else
			int iDistance = (*itr).GetDistance();
#endif

			int iPathDistance = MAX_INT;
			// Now loop through the units, using the pathfinder to do the final evaluation
			if (pkMusterPlot != NULL)
			{
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkMusterPlot))
					continue;

				iPathDistance = kPathFinder.GetPathLength();
			}
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP)
			if (iPathDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE() && pkLoopUnit->getDropRange() == 0)
			{
				continue;
			}
#endif
			if(pkTarget != NULL && bNeedToCheckTarget)
			{
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkTarget))
					continue;

				if (pkMusterPlot == NULL)
					iPathDistance = kPathFinder.GetPathLength();
			}
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP)
			if (pkLoopUnit->getDropRange() > 0)
			{
				if (pkMusterPlot != NULL && plotDistance(pkLoopUnit->getX(), pkLoopUnit->getY(), pkMusterPlot->getX(), pkMusterPlot->getY()) <= pkLoopUnit->getDropRange())
				{
					if (pkLoopUnit->canParadropAt(pkMusterPlot, pkMusterPlot->getX(), pkMusterPlot->getY()))
					{
						iPathDistance = 0;
					}
					else
					{
						CvPlot* pAdjacentPlot;
						for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
						{
							pAdjacentPlot = plotDirection(pkMusterPlot->getX(), pkMusterPlot->getY(), ((DirectionTypes)jJ));
							if (pAdjacentPlot != NULL)
							{
								if (pkLoopUnit->canParadropAt(pAdjacentPlot, pAdjacentPlot->getX(), pAdjacentPlot->getY()))
								{
									iPathDistance = MIN(1, iPathDistance);
									break;
								}
							}
						}
					}
				}
				else if (pkMusterPlot == NULL && pkTarget != NULL && plotDistance(pkLoopUnit->getX(), pkLoopUnit->getY(), pkTarget->getX(), pkTarget->getY()) <= pkLoopUnit->getDropRange())
				{
					if (pkLoopUnit->canParadropAt(pkTarget, pkTarget->getX(), pkTarget->getY()))
					{
						iPathDistance = 0;
					}
					else
					{
						CvPlot* pAdjacentPlot;
						for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
						{
							pAdjacentPlot = plotDirection(pkTarget->getX(), pkTarget->getY(), ((DirectionTypes)jJ));
							if (pAdjacentPlot != NULL)
							{
								if (pkLoopUnit->canParadropAt(pAdjacentPlot, pAdjacentPlot->getX(), pAdjacentPlot->getY()))
								{
									iPathDistance = MIN(1, iPathDistance);
									break;
								}
							}
						}
					}
				}
			}
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP

#if !defined(AUI_OPERATION_GET_CLOSEST_UNIT_NO_EARLY_BREAK)
			// Reasonably close?
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP)
			if (iPathDistance <= iDistance && iPathDistance <= iBestDistance && pkLoopUnit->getDropRange() == 0)
#else
			if (iPathDistance <= iDistance && iPathDistance <= iBestDistance)
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP
			{
				pkBestUnit = pkLoopUnit;
				break;
			}
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_NO_EARLY_BREAK

			if (iPathDistance < iBestDistance)
			{
				pkBestUnit = pkLoopUnit;
				iBestDistance = iPathDistance;
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_GET_STRONGEST)
				iBestStrength = pkLoopUnit->GetBaseCombatStrengthConsideringDamage();
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_GET_STRONGEST
			}
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_GET_STRONGEST)
			else if (iPathDistance == iBestDistance)
			{
				if (pkBestUnit)
				{
					// Civilian units compare HP
					if (!pkBestUnit->IsCombatUnit() && !pkLoopUnit->IsCombatUnit())
					{
						if (pkLoopUnit->GetCurrHitPoints() > pkBestUnit->GetCurrHitPoints())
						{
							pkBestUnit = pkLoopUnit;
						}
					}
					// Military units compare strength
					else
					{
						int iLoopStrength = pkLoopUnit->GetBaseCombatStrengthConsideringDamage();
						if (iLoopStrength > iBestStrength)
						{
							pkBestUnit = pkLoopUnit;
							iBestStrength = iLoopStrength;
						}
					}
				}
				else
				{
					pkBestUnit = pkLoopUnit;
					iBestStrength = pkLoopUnit->GetBaseCombatStrengthConsideringDamage();
				}
			}
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_GET_STRONGEST

			// Were we far away?  If so, this is probably the best we are going to do
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP)
#else
			if (iDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE())
				break;
#endif
		}
	}

	return pkBestUnit;
#endif
}

/// Find a unit from our reserves that could serve in this operation
#if defined(MOD_BALANCE_CORE)
bool CvAIOperation::FindBestFitRecruitUnit(OperationSlot thisOperationSlot, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool* bRequired)
{
	CvUnit* pBestUnit = NULL;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;

	CvOperationSearchUnitList kSearchList;
	*bRequired = true;
	if(!pThisArmy)
	{
		return false;
	}
	if(pTargetPlot == NULL || pMusterPlot == NULL)
	{
		return false;
	}
	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if(iThisFormationIndex != NO_MUFORMATION)
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
		if(thisFormation)
		{
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
			if(thisSlotEntry.m_requiredSlot)
			{
				*bRequired = true;
			}
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(thisOperationSlot.m_iSlotID);
			if(pSlot && pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
				if(pUnit)
				{
					if(pMusterPlot != NULL && pTargetPlot != NULL && strMsg)
					{
						strMsg.Format("Existing army slot already full for muster of x=%d y=%d, target of x=%d y=%d. Unit is: %s", pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY(), pUnit->getName().GetCString());
						LogOperationSpecialMessage(strMsg);
					}
					return true;
				}
			}
			bool bMustBeDeepWaterNaval = GET_TEAM(ownerPlayer.getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

			int iLoop = 0;
			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				// Make sure he's not needed by the tactical AI or already in an army
				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1)
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
					{
						continue;
					}

					// PRIMARY UNIT TYPE (ONLY)
					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType))
					{
						//Sanity check for AI sending CS to non-CS operations
						if(pLoopUnit->IsCityAttackOnly() && !pTargetPlot->isCity())
						{
							continue;
						}
						//NONCOMBAT LAND UNITS
						if(!pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL && pMusterPlot != NULL)
								{
									int iDistance = -1;
									if(pkLoopUnitPlot == pMusterPlot)
									{
										iDistance = 0;
									}
									else
									{
										iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
										// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
										if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
										{
											iDistance *= 2;
										}
										if(iDistance == -1)
										{
											CvAssertMsg(0, "No muster or target!");
											iDistance = MAX_INT;
										}
									}

									kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
								}
							}
							//NONCOMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pNavalTarget)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
						//COMBAT LAND UNITS
						else if(pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL)
								{
									if(pMusterPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
							//COMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}
			if(kSearchList.size() > 0 && pTargetPlot != NULL && pMusterPlot != NULL)
			{
				pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetPlot, NeedToCheckPathToTarget());
			}

			// Did we find one?
			if(pBestUnit != NULL)
			{
				if (pBestUnit->AI_getUnitAIType() != thisSlotEntry.m_primaryUnitType)
				{
					pBestUnit->AI_setUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType);
				}
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				if(GC.getLogging() && GC.getAILogging())
				{
					if(pMusterPlot != NULL && pTargetPlot != NULL && strMsg)
					{
						strMsg.Format("Recruited %s (primary) to fill in an existing army at x=%d y=%d, target of x=%d y=%d", pBestUnit->getName().GetCString(), pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				return true;
			}

			kSearchList.clear();

			iLoop = 0;
			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				// Make sure he's not needed by the tactical AI or already in an army
				if(pLoopUnit != NULL && pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1)
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
					{
						continue;
					}

					// PRIMARY UNIT TYPE (ONLY)
					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType))
					{
						//Sanity check for AI sending CS to non-CS operations
						if(pLoopUnit->IsCityAttackOnly() && !pTargetPlot->isCity())
						{
							continue;
						}
						//NONCOMBAT LAND UNITS
						if(!pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL && pMusterPlot != NULL)
								{
									int iDistance = -1;
									if(pkLoopUnitPlot == pMusterPlot)
									{
										iDistance = 0;
									}
									else
									{
										iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
										// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
										if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
										{
											iDistance *= 2;
										}
										if(iDistance == -1)
										{
											CvAssertMsg(0, "No muster or target!");
											iDistance = MAX_INT;
										}
									}

									kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
								}
							}
							//NONCOMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pNavalTarget)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
						//COMBAT LAND UNITS
						else if(pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL)
								{
									if(pMusterPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
							//COMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}
			if(kSearchList.size() > 0 && pTargetPlot != NULL && pMusterPlot != NULL)
			{
				pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetPlot, NeedToCheckPathToTarget());
			}

			// Did we find one?
			if(pBestUnit != NULL)
			{
				if (pBestUnit->AI_getUnitAIType() != thisSlotEntry.m_secondaryUnitType)
				{
					pBestUnit->AI_setUnitAIType((UnitAITypes)thisSlotEntry.m_secondaryUnitType);
				}
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				if(GC.getLogging() && GC.getAILogging())
				{
					if(pMusterPlot != NULL && pTargetPlot != NULL && strMsg)
					{
						strMsg.Format("Recruited %s (secondary) to fill in an existing army at x=%d y=%d, target of x=%d y=%d", pBestUnit->getName().GetCString(), pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				return true;
			}
		}
	}
	return false;
}
#endif
bool CvAIOperation::FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool* bRequired)
{
#if defined(MOD_BALANCE_CORE)
	CvUnit* pBestUnit = NULL;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;

	CvOperationSearchUnitList kSearchList;
	*bRequired = true;

	if(pTargetPlot == NULL || pMusterPlot == NULL)
	{
		return false;
	}
	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if(iThisFormationIndex != NO_MUFORMATION)
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
		if(thisFormation)
		{
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
			bool bMustBeDeepWaterNaval = GET_TEAM(ownerPlayer.getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

			int iLoop = 0;
			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
#if defined(MOD_BALANCE_CORE)
				//don't recruit if currently healing
				if (ownerPlayer.GetTacticalAI()->IsUnitHealing(pLoopUnit->GetID()))
					continue;
#endif

				// Make sure he's not needed by the tactical AI or already in an army
				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1)
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
					{
						continue;
					}

					// PRIMARY UNIT TYPE (ONLY)
					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType))
					{
						//Sanity check for AI sending CS to non-CS operations
						if(pLoopUnit->IsCityAttackOnly() && !pTargetPlot->isCity())
						{
							continue;
						}
						//NONCOMBAT LAND UNITS
						if(!pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL && pMusterPlot != NULL)
								{
									int iDistance = -1;
									if(pkLoopUnitPlot == pMusterPlot)
									{
										iDistance = 0;
									}
									else
									{
										iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
										// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
										if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
										{
											iDistance *= 2;
										}
										if(iDistance == -1)
										{
											CvAssertMsg(0, "No muster or target!");
											iDistance = MAX_INT;
										}
									}

									kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
								}
							}
							//NONCOMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pNavalTarget)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
						//COMBAT LAND UNITS
						else if(pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL)
								{
									if(pMusterPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
							//COMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}
			if(pTargetPlot != NULL && pMusterPlot != NULL)
			{
				pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetPlot, NeedToCheckPathToTarget());
			}

			// Did we find one?
			if(pBestUnit != NULL)
			{
				if (pBestUnit->AI_getUnitAIType() != thisSlotEntry.m_primaryUnitType)
				{
					pBestUnit->AI_setUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType);
				}
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				if(GC.getLogging() && GC.getAILogging())
				{
					if(pMusterPlot != NULL && pTargetPlot != NULL && strMsg)
					{
						strMsg.Format("Recruited %s (primary) to fill in a new army at x=%d y=%d, target of x=%d y=%d", pBestUnit->getName().GetCString(), pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				return true;
			}

			kSearchList.clear();

			iLoop = 0;
			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				// Make sure he's not needed by the tactical AI or already in an army
				if(pLoopUnit != NULL && pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1)
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
					{
						continue;
					}

					// PRIMARY UNIT TYPE (ONLY)
					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType))
					{
						//Sanity check for AI sending CS to non-CS operations
						if(pLoopUnit->IsCityAttackOnly() && !pTargetPlot->isCity())
						{
							continue;
						}
						//NONCOMBAT LAND UNITS
						if(!pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL && pMusterPlot != NULL)
								{
									int iDistance = -1;
									if(pkLoopUnitPlot == pMusterPlot)
									{
										iDistance = 0;
									}
									else
									{
										iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
										// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
										if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
										{
											iDistance *= 2;
										}
										if(iDistance == -1)
										{
											CvAssertMsg(0, "No muster or target!");
											iDistance = MAX_INT;
										}
									}

									kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
								}
							}
							//NONCOMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pNavalTarget)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
						//COMBAT LAND UNITS
						else if(pLoopUnit->IsCombatUnit())
						{
							if(pLoopUnit->getDomainType() == DOMAIN_LAND)
							{
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								if(IsMixedLandNavalOperation() && !pLoopUnit->CanEverEmbark())
								{
									continue;
								}
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								// Get raw distance to the muster point or target
								CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
								if(pkLoopUnitPlot != NULL)
								{
									if(pMusterPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
							//COMBAT SEA UNITS
							else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							{
								if(pLoopUnit->GetCurrHitPoints() < ((pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()) / 100))
								{
									continue;
								}
								if(!IsMixedLandNavalOperation() && !IsAllNavalOperation())
								{
									continue;
								}
								// Not finishing up an operation?
								if(bMustBeDeepWaterNaval && pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
								{
									continue;
								}

								if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
								{
									continue;
								}
								CvPlot* pNavalTarget = NULL;
								// Mixed naval operation targeting a city?   Change target (but only for the naval units).
								if(pMusterPlot != NULL)
								{
									if(!pMusterPlot->isWater())
									{
										pNavalTarget = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pThisArmy);
										if(pNavalTarget == NULL)
										{
											continue;
										}
									}
									else
									{
										pNavalTarget = pMusterPlot;
									}
								}
								if(pNavalTarget != NULL)
								{				
									// Get raw distance to the muster point or target.
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									if(pkLoopUnitPlot != NULL)
									{
										int iDistance = -1;
										if(pkLoopUnitPlot == pMusterPlot)
										{
											iDistance = 0;
										}
										else
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pNavalTarget->getX(), pNavalTarget->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pkLoopUnitPlot->getArea() != pNavalTarget->getArea() && pLoopUnit->getDropRange() == 0)
											{
												iDistance *= 2;
											}
											if(iDistance == -1)
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}
										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}
			if(pTargetPlot != NULL && pMusterPlot != NULL)
			{
				pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetPlot, NeedToCheckPathToTarget());
			}

			// Did we find one?
			if(pBestUnit != NULL)
			{
				if (pBestUnit->AI_getUnitAIType() != thisSlotEntry.m_secondaryUnitType)
				{
					pBestUnit->AI_setUnitAIType((UnitAITypes)thisSlotEntry.m_secondaryUnitType);
				}
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				if(GC.getLogging() && GC.getAILogging())
				{
					if(pMusterPlot != NULL && pTargetPlot != NULL && strMsg)
					{
						strMsg.Format("Recruited %s (secondary) to fill in a new army at x=%d y=%d, target of x=%d y=%d", pBestUnit->getName().GetCString(), pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				return true;
			}

			// If not required, let our calling routine know that
			if(!thisSlotEntry.m_requiredSlot)
			{
				*bRequired = false;
			}
		}
	}
	return false;
#else
	CvUnit* pBestUnit = NULL;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;
	CvPlot *pTargetToUse = pTargetPlot;

	CvOperationSearchUnitList kSearchList;

	*bRequired = true;

	// All naval operation targeting a city?   Change target
#if defined(MOD_BALANCE_CORE)
	if (pTargetToUse && !pTargetToUse->isWater() && IsAllNavalOperation() && pTargetToUse->isCoastalLand())
#else
	if (pTargetToUse && !pTargetToUse->isWater() && IsAllNavalOperation())
#endif
	{
		pTargetToUse = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetToUse, pThisArmy);
	}

	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if(iThisFormationIndex != NO_MUFORMATION)
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
		if(thisFormation)
		{
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);
			bool bMustBeDeepWaterNaval = GET_TEAM(ownerPlayer.getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

			int iLoop = 0;
			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				const UnitAITypes eLoopUnitAIType = pLoopUnit->AI_getUnitAIType();
#if defined(AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS)
				const UnitAITypes eLoopUnitDefaultAIType = (UnitAITypes)pLoopUnit->getUnitInfo().GetDefaultUnitAIType();
#endif // AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS

				// Make sure he's not needed by the tactical AI or already in an army or scouting
				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1 &&
#if defined(AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS)
					(eLoopUnitAIType != UNITAI_EXPLORE || eLoopUnitDefaultAIType != UNITAI_EXPLORE) &&
					(eLoopUnitAIType != UNITAI_EXPLORE_SEA || eLoopUnitDefaultAIType != UNITAI_EXPLORE_SEA) )
#else
					eLoopUnitAIType != UNITAI_EXPLORE && eLoopUnitAIType != UNITAI_EXPLORE_SEA && pLoopUnit->getDropRange() == 0 /* no paratroopers */)
#endif // AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
						continue;

					// PRIMARY UNIT TYPE (ONLY)

					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType))
					{
#if defined(MOD_BALANCE_CORE)
						//Sanity check for AI sending CS to non-CS operations
						if(pTargetToUse != NULL && pLoopUnit->IsCityAttackOnly() && !pTargetToUse->isCity())
						{
							continue;
						}
						// Mixed naval operation targeting a city?   Change target (but only for the naval units).
						if (unitInfo->GetDomainType() == DOMAIN_SEA && pTargetToUse && !pTargetToUse->isWater())
						{
							CvPlot* pTargetToUse2 = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetToUse, pThisArmy);
							if(pTargetToUse2 == NULL)
							{
								continue;
							}
						}
#endif
						// Is his health okay?
						if(!pLoopUnit->IsCombatUnit() || pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
						{
							// Not finishing up an operation?
							if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
							{
								if((!IsAllNavalOperation() && !IsMixedLandNavalOperation()) || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
								{
									if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
									{
										// Get raw distance to the muster point or target
										CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
										int iDistance;
										if(pMusterPlot != NULL)
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP)
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
#else
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea())
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP
											{
												iDistance *= 2;
											}
										}
										else
										{
											if (pTargetToUse != NULL)
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pTargetToUse->getX(), pTargetToUse->getY());
#if defined(MOD_BALANCE_CORE)
											//Fallback for capital - not great, and possibly draws bad units, but at least gets the muster going more quickly).
											else if(GET_PLAYER(m_eOwner).getCapitalCity() != NULL)
											{
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), GET_PLAYER(m_eOwner).getCapitalCity()->getX(), GET_PLAYER(m_eOwner).getCapitalCity()->getY());
												pMusterPlot = GET_PLAYER(m_eOwner).getCapitalCity()->plot();
											}
#endif
											else
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}

			pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetToUse, NeedToCheckPathToTarget());

			// Did we find one?
			if(pBestUnit != NULL)
			{
#if defined(AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS)
				if (pBestUnit->AI_getUnitAIType() != thisSlotEntry.m_primaryUnitType)
					pBestUnit->AI_setUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType);
#endif // AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				return true;
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Could not recruit from primary units for muster at x=%d y=%d, target of x=%d y=%d", (pMusterPlot)?pMusterPlot->getX():-1, (pMusterPlot)?pMusterPlot->getY():-1, (pTargetToUse)?pTargetToUse->getX():-1, (pTargetToUse)?pTargetToUse->getY():-1);
					LogOperationSpecialMessage(strMsg);
				}
			}

			kSearchList.clear();
			// Loop again this time through secondary units
			for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				const UnitAITypes eLoopUnitAIType = pLoopUnit->AI_getUnitAIType();
#if defined(AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS)
				const UnitAITypes eLoopUnitDefaultAIType = (UnitAITypes)pLoopUnit->getUnitInfo().GetDefaultUnitAIType();
#endif // AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS

				// Make sure he's not needed by the tactical AI or already in an army or scouting
				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1 &&
#if defined(AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS)
					(eLoopUnitAIType != UNITAI_EXPLORE || eLoopUnitDefaultAIType != UNITAI_EXPLORE) &&
					(eLoopUnitAIType != UNITAI_EXPLORE_SEA || eLoopUnitDefaultAIType != UNITAI_EXPLORE_SEA) )
#else
				        eLoopUnitAIType != UNITAI_EXPLORE && eLoopUnitAIType != UNITAI_EXPLORE_SEA && pLoopUnit->getDropRange() == 0 /* no paratroopers */)
#endif // AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
						continue;

					// SECONDARY UNIT TYPE (ONLY)

					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_secondaryUnitType))
					{
#if defined(MOD_BALANCE_CORE)
						//Sanity check for AI sending CS to non-CS operations
						if(pTargetPlot != NULL && pLoopUnit->IsCityAttackOnly() && !pTargetPlot->isCity())
						{
							continue;
						}
						// Mixed naval operation targeting a city?   Change target (but only for the naval units).
						if (unitInfo->GetDomainType() == DOMAIN_SEA && pTargetToUse && !pTargetToUse->isWater())
						{
							pTargetToUse = ownerPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetToUse, pThisArmy);
							if(pTargetToUse == NULL)
							{
								continue;
							}
						}
#endif
						// Is his health okay?
						if(!pLoopUnit->IsCombatUnit() || pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
						{
							// Not finishing up an operation?
							if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
							{
								if((!IsAllNavalOperation() && !IsMixedLandNavalOperation()) || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
								{
									if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
									{
										// Get raw distance to the muster point or target
										CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
										int iDistance;
										if(pMusterPlot != NULL)
										{
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
// This define is needed because iDistance feeds into GetClosestUnit
#if defined(AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP)
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if (pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea() && pLoopUnit->getDropRange() == 0)
#else
											// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
											if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea())
#endif // AUI_OPERATION_GET_CLOSEST_UNIT_PARADROP
											{
												iDistance *= 2;
											}
										}
										else
										{
											if (pTargetToUse != NULL)
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pTargetToUse->getX(), pTargetToUse->getY());
#if defined(MOD_BALANCE_CORE)
											//Fallback for capital - not great, and possibly draws bad units, but at least gets the muster going more quickly).
											else if(GET_PLAYER(m_eOwner).getCapitalCity() != NULL)
											{
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(),GET_PLAYER(m_eOwner).getCapitalCity()->getX(), GET_PLAYER(m_eOwner).getCapitalCity()->getY());
												pMusterPlot = GET_PLAYER(m_eOwner).getCapitalCity()->plot();
											}
#endif
											else
											{
												CvAssertMsg(0, "No muster or target!");
												iDistance = MAX_INT;
											}
										}

										kSearchList.push_back(CvOperationSearchUnit(pLoopUnit, iDistance));
									}
								}
							}
						}
					}
				}
			}

			pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetToUse, NeedToCheckPathToTarget());

			// Did we find one?
			if(pBestUnit != NULL)
			{
#if defined(AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS)
				if (pBestUnit->AI_getUnitAIType() != thisSlotEntry.m_primaryUnitType)
					pBestUnit->AI_setUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType);
#endif // AUI_OPERATION_FIX_FIND_BEST_FIT_RESERVE_CONSIDER_SCOUTING_NONSCOUTS
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				return true;
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Could not recruit from secondary units for muster at x=%d y=%d, target of x=%d y=%d", (pMusterPlot)?pMusterPlot->getX():-1, (pMusterPlot)?pMusterPlot->getY():-1, (pTargetToUse)?pTargetToUse->getX():-1, (pTargetToUse)?pTargetToUse->getY():-1);
					LogOperationSpecialMessage(strMsg);
				}
			}
			// If not required, let our calling routine know that
			if(!thisSlotEntry.m_requiredSlot)
			{
				*bRequired = false;
			}
		}
	}

	return false;
#endif
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
// CvAIEnemyTerritoryOperation
////////////////////////////////////////////////////////////////////////////////
CvAIEnemyTerritoryOperation::CvAIEnemyTerritoryOperation()
{
}

CvAIEnemyTerritoryOperation::~CvAIEnemyTerritoryOperation()
{
}

/// How long will we wait for a recruit to show up?
int CvAIEnemyTerritoryOperation::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY();
}

/// Kick off this operation
void CvAIEnemyTerritoryOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /* iDefaultArea */, CvCity*, CvCity*)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());
			// Figure out the initial rally point
			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if(SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());

					if (GetDefaultArea() != pTargetPlot->getArea())
					{
						pArmyAI->SetGoalPlot(pTargetPlot);
					}
					else
					{
#if defined(MOD_BALANCE_CORE)
						CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();	
						int iUnitID;
						CvUnit* pFirstUnit = NULL;
						iUnitID = pArmyAI->GetFirstUnitID();
						if(iUnitID != -1)
						{
							pFirstUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						}
#endif

						CvPlot* pDeployPt;
						pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), GetMusterPlot(), GetTargetPlot(), (GetDeployRange() / 2), true);
						if (pDeployPt)
						{
							pArmyAI->SetGoalPlot(pDeployPt);
						}
#if defined(MOD_BALANCE_CORE)

						else if(GetTargetPlot() != NULL && pFirstUnit != NULL && kPathFinder.DoesPathExist(*(pFirstUnit), pFirstUnit->plot(), GetTargetPlot()))
						{
							pArmyAI->SetGoalPlot(GetTargetPlot());
						}
#endif
						else
						{
							// No path, abort
							m_eCurrentState = AI_OPERATION_STATE_ABORTED;
							m_eAbortReason = AI_ABORT_LOST_PATH;
						}
					}

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), pTargetPlot))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{
					// No muster point, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}

/// How close to target do we end up?
int CvAIEnemyTerritoryOperation::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
}

#if defined(MOD_BALANCE_CORE)
CvPlot* CvAIOperationDestroyBarbarianCamp::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
{
	if(GetMusterPlot() != NULL)
	{
		return GetMusterPlot();
	}

	return CvAIEnemyTerritoryOperation::SelectInitialMusterPoint(pThisArmy);
}
#endif

/// Figure out the initial rally point
CvPlot* CvAIEnemyTerritoryOperation::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
{
	CvPlot* pMusterPt = NULL;
	CvPlot* pStartCityPlot;
	CvCity* pStartCity;
	CvPlot* pCurrentPlot;
	int iSpacesFromTarget = 0;
	CvPlot* pDeployPlot = NULL;
	int iDangerousPlots = 0;

	pStartCity = GetOperationStartCity();
#if defined(MOD_BALANCE_CORE)			
	if(pStartCity == NULL && pThisArmy != NULL)
	{
		if(GET_PLAYER(pThisArmy->GetOwner()).getCapitalCity() != NULL)
		{
			pStartCity = GET_PLAYER(pThisArmy->GetOwner()).getCapitalCity();
		}
	}
#endif
	if(pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if(pStartCityPlot != NULL)
		{
			// Different areas?  If so, just muster at start city
			if (pStartCityPlot->getArea() != pThisArmy->GetGoalPlot()->getArea())
			{
				SetMusterPlot(pStartCityPlot);
				return pStartCityPlot;
			}

			CvAStarNode* pNode;

			// Generate path
			GC.getStepFinder().SetData(&m_eEnemy);
			if(GC.getStepFinder().GeneratePath(pStartCityPlot->getX(), pStartCityPlot->getY(), pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY(), m_eOwner, false))
			{
				pNode = GC.getStepFinder().GetLastNode();

				// Starting at the end, loop until we find a plot from this owner
				while(pNode != NULL)
				{
					pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);

					// Is this the deploy point?
					if(iSpacesFromTarget == GetDeployRange())
					{
						pDeployPlot = pCurrentPlot;
					}

					// Check and see if this plot has the right owner
					if(pCurrentPlot->getOwner() == GetOwner())
					{
						pMusterPt = pCurrentPlot;
						break;
					}

					else
					{
						// Is this a dangerous plot?
						if(GET_PLAYER(m_eOwner).GetPlotDanger(*pCurrentPlot) > 0)
						{
							iDangerousPlots++;
						}
					}

					// Move to the previous plot on the path
					iSpacesFromTarget++;
					pNode = pNode->m_pParent;
				}

				// Is the path safe?  If so, let's just muster at the deploy point
				if(iSpacesFromTarget > 0 && (iDangerousPlots * 100 / iSpacesFromTarget) < GC.getAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER())
				{
					if(pDeployPlot)
					{
						pMusterPt = pDeployPlot;
					}
				}
			}
		}
	}

	if(pMusterPt != NULL)
	{
		SetMusterPlot(pMusterPt);
	}
#if defined(MOD_BALANCE_CORE)
	else if(GetMusterPlot() != NULL)
	{
		return GetMusterPlot();
	}
#endif
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("No muster point found, Operation aborting, Target was, X: %d, Y: %d", pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY());
			LogOperationSpecialMessage(szMsg);
		}
	}

	return pMusterPt;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationBasicCityAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationBasicCityAttack::CvAIOperationBasicCityAttack()
{
}

/// Destructor
CvAIOperationBasicCityAttack::~CvAIOperationBasicCityAttack()
{
}

/// Kick off this operation
void CvAIOperationBasicCityAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /* iDefaultArea */, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

#if !defined(MOD_BALANCE_CORE)
	SetStartCityPlot(pMuster->plot());
#endif

	if(iID != -1)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

#if defined(MOD_BALANCE_CORE)
			if(pTarget && pMuster)
			{
				SetStartCityPlot(pMuster->plot());
#else
			if(pTarget)
			{
#endif
				SetTargetPlot(pTarget->plot());
				pArmyAI->SetGoalPlot(GetTargetPlot());
				SetMusterPlot(GetStartCityPlot());
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());
#if defined(MOD_BALANCE_CORE)
				CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();	
				int iUnitID;
				CvUnit* pFirstUnit = NULL;
				iUnitID = pArmyAI->GetFirstUnitID();
				if(iUnitID != -1)
				{
					pFirstUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
				}
#endif
				// Reset our destination to be a few plots shy of the final target
				CvPlot* pDeployPt;
				pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), GetMusterPlot(), GetTargetPlot(), (GetDeployRange() / 2), true);
				if (pDeployPt)
				{
					pArmyAI->SetGoalPlot(pDeployPt);

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
#if defined(MOD_BALANCE_CORE)

				else if(GetTargetPlot() != NULL && pFirstUnit != NULL && kPathFinder.DoesPathExist(*(pFirstUnit), pFirstUnit->plot(), GetTargetPlot()))
				{
					pArmyAI->SetGoalPlot(GetTargetPlot());

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
#endif
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_PATH;
				}
			}

			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}

/// Read serialized data
void CvAIOperationBasicCityAttack::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationBasicCityAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

MultiunitFormationTypes CvAIOperationBasicCityAttack::GetFormation() const
{
#if defined(MOD_BALANCE_CORE)
	return (MultiunitFormationTypes)MilitaryAIHelpers::GetBestFormationType();
#else
	return (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
#endif
}

/// Same as default version except if just gathered forces, check to see if a better target has presented itself
bool CvAIOperationBasicCityAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		// First do base case processing
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Is target still under enemy control?
		CvPlot* pTarget = GetTargetPlot();
		if(pTarget->getOwner() != m_eEnemy)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
		}
	}
	break;

	// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_LAND);

#if defined(MOD_BALANCE_CORE)
		bool bBeenHad = false;
		CvString strMsg;
		if(pArmy && !GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
		{
			UnitHandle pUnit;
			pUnit = pArmy->GetFirstUnit();
			while(pUnit)
			{
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL)
					{
						CvUnit* pOtherUnit = pAdjacentPlot->getUnitByIndex(0);
						if(pOtherUnit != NULL && pOtherUnit->getOwner() == m_eEnemy)
						{
							// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
							if(GC.getLogging() && GC.getAILogging())
							{
								strMsg.Format("Ran into enemy unit during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
								LogOperationSpecialMessage(strMsg);
							}
							bBeenHad = true;
							break;
						}
					}
				}
				pUnit = pArmy->GetNextUnit();
			}
		}
		if(bBeenHad)
		{
			// Notify Diplo AI we're in place for attack
			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
		// Are we within tactical range of our target?
		if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
#else
		// Are we within tactical range of our target?
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
#endif
		{
			// Notify Diplo AI we're in place for attack
			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationBasicCityAttack::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		// See if our target city is still owned by our enemy
		if(GetTargetPlot()->getPlotCity() == NULL || GetTargetPlot()->getOwner() != m_eEnemy)
		{
			// Success!  The city has been captured/destroyed
			return true;
		}
	}

	return rtnValue;
}

/// Find the city we want to attack
CvPlot* CvAIOperationBasicCityAttack::FindBestTarget()
{
	CvAssertMsg(false, "Obsolete function called CvAIOperationBasicCityAttack::FindBestTarget()");

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationSneakCityAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationSneakCityAttack::CvAIOperationSneakCityAttack()
{
}
MultiunitFormationTypes CvAIOperationSneakCityAttack::GetFormation() const
{
#if defined(MOD_BALANCE_CORE)
	return (MultiunitFormationTypes)MilitaryAIHelpers::GetBestFormationType();
#else
	return (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
#endif
}

CvAIOperationQuickSneakCityAttack::CvAIOperationQuickSneakCityAttack()
{
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationSmallCityAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationSmallCityAttack::CvAIOperationSmallCityAttack()
{
}
#if defined(MOD_BALANCE_CORE)
MultiunitFormationTypes CvAIOperationSmallCityAttack::GetFormation() const
{
	return MUFORMATION_SMALL_CITY_ATTACK_FORCE;
}
#endif
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityStateAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCityStateAttack::CvAIOperationCityStateAttack()
{
}
MultiunitFormationTypes CvAIOperationCityStateAttack::GetFormation() const
{
#if defined(MOD_BALANCE_CORE)
	return MUFORMATION_CITY_STATE_ATTACK_FORCE;
#else
	return (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_CITY_STATE_ATTACK_FORCE;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDestroyBarbarianCamp
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationDestroyBarbarianCamp::CvAIOperationDestroyBarbarianCamp()
{
	m_bCivilianRescue = false;
	m_iUnitToRescue = NULL;
#if defined(MOD_BALANCE_CORE)
	m_bInitializedRun = true;
#endif
}

/// Destructor
CvAIOperationDestroyBarbarianCamp::~CvAIOperationDestroyBarbarianCamp()
{
}

/// How close to target do we end up?
int CvAIOperationDestroyBarbarianCamp::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE();
}

/// Read serialized data
void CvAIOperationDestroyBarbarianCamp::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
	m_bCivilianRescue = false;
#if defined(MOD_BALANCE_CORE)
	m_bInitializedRun = true;
#endif
	kStream >> m_bCivilianRescue;
#if defined(MOD_BALANCE_CORE)
	kStream >> m_bInitializedRun;
#endif
	kStream >> m_iUnitToRescue;
}

/// Write serialized data
void CvAIOperationDestroyBarbarianCamp::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
	kStream << m_bCivilianRescue;
#if defined(MOD_BALANCE_CORE)
	kStream << m_bInitializedRun;
#endif
	kStream << m_iUnitToRescue;
}

/// Same as default version except if just gathered forces, check to see if a better target has presented itself
bool CvAIOperationDestroyBarbarianCamp::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;
#if defined(MOD_BALANCE_CORE)
	if(m_bInitializedRun)
	{
		CvPlot* pTarget = FindBestTarget();
		if(pTarget != NULL)
		{
			SetTargetPlot(pTarget);
		}
		m_bInitializedRun = false;
	}
#endif
	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		// First do base case processing
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);
		// Now revisit target
#if defined(MOD_BALANCE_CORE)
#else
		CvPlot* possibleBetterTarget;
#endif
#if defined(MOD_BALANCE_CORE)
		CvPlot* possibleBetterTarget = GetTargetPlot();
		if(possibleBetterTarget == NULL)
		{
#endif
		possibleBetterTarget = FindBestTarget();
#if defined(MOD_BALANCE_CORE)
		}
		if(possibleBetterTarget != NULL)
		{
			SetTargetPlot(possibleBetterTarget);
		}
#endif
		// If no target left, abort
		if(possibleBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}

		// If target changed, reset to this new one
#if defined(MOD_BALANCE_CORE)
		else
#else
		else if(possibleBetterTarget != GetTargetPlot())
#endif
		{
			// If we're traveling on a single continent, set our destination to be a few plots shy of the final target
#if defined(MOD_BALANCE_CORE)
			if (pArmy->GetArea() == GetTargetPlot()->getArea())
			{
				CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), GetTargetPlot(), GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE(), false);
				if(pDeployPt != NULL)
				{
					pArmy->SetGoalPlot(pDeployPt);
					SetTargetPlot(GetTargetPlot());
				}
			}
			else
			{
				pArmy->SetGoalPlot(GetTargetPlot());
				SetTargetPlot(GetTargetPlot());
			}
		}
#else
			if (pArmy->GetArea() == possibleBetterTarget->getArea())
			{
				CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE(), false);
				if(pDeployPt != NULL)
				{
					pArmy->SetGoalPlot(pDeployPt);
					SetTargetPlot(possibleBetterTarget);
				}
			}

			// Coming in from the sea. Just head to the camp
			else
			{
				pArmy->SetGoalPlot(possibleBetterTarget);
				SetTargetPlot(possibleBetterTarget);
			}
		}
#endif
	}
	break;

	// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if (plotDistance(pArmy->GetX(), pArmy->GetY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= 1)
		{
			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_BARBARIAN_CAMP);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationDestroyBarbarianCamp::ShouldAbort()
{
	CvString strMsg;

	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
#if defined(MOD_BALANCE_CORE)
		if(GetTargetPlot() != NULL)
		{
#endif
		// See if our target camp is still there
#if defined(MOD_BALANCE_CORE)
		ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();
		if (!m_bCivilianRescue && GetTargetPlot()->getImprovementType() != eBarbCamp)
#else
		if (!m_bCivilianRescue && GetTargetPlot()->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
#endif
		{
			// Success!  The camp is gone
			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("Barbarian camp at (x=%d y=%d) no longer exists. Aborting", GetTargetPlot()->getX(), GetTargetPlot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
#if defined(MOD_BALANCE_CORE)
			CvPlot* possibleBetterTarget = NULL;
			possibleBetterTarget = FindBestTarget();
			if((possibleBetterTarget != NULL) && (possibleBetterTarget != GetTargetPlot()))
			{
				SetTargetPlot(possibleBetterTarget);
				// Success!  The camp is gone
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Barbarian camp at (x=%d y=%d) no longer exists. Moving on to a new target.", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				return false;
			}
#endif
			return true;
		}
		else if (m_bCivilianRescue)
		{
			// is the unit rescued?
			CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
			CvUnit* pUnitToRescue = BarbPlayer.getUnit(m_iUnitToRescue);
			if (!pUnitToRescue)
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format ("Civilian can no longer be rescued from barbarians. Aborting");
					LogOperationSpecialMessage(strMsg);
				}
				return true;
			}
			else
			{
				if (pUnitToRescue->GetOriginalOwner() != m_eOwner || (pUnitToRescue->AI_getUnitAIType() != UNITAI_SETTLE && pUnitToRescue->AI_getUnitAIType() != UNITAI_WORKER))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format ("Civilian can no longer be rescued from barbarians. Aborting");
						LogOperationSpecialMessage(strMsg);
					}
					return true;
				}
			}
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
#if defined(MOD_BALANCE_CORE)
		}
		else
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("No Barbarian camp target for army. Aborting");
				LogOperationSpecialMessage(strMsg);
			}
			return true;
		}
#endif
	}

	return rtnValue;
}

/// Find the barbarian camp we want to eliminate
CvPlot* CvAIOperationDestroyBarbarianCamp::FindBestTarget()
{
	int iPlotLoop;
	CvPlot* pBestPlot = NULL;
	CvPlot* pPlot;
	int iBestPlotDistance = MAX_INT;
	int iCurPlotDistance;

	m_bCivilianRescue = false;

#if defined(MOD_BALANCE_CORE)
	CvPlot* pBestMuster = NULL;
	ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();
#else
	TeamTypes eTeam = GET_PLAYER(m_eOwner).getTeam();
	ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();
	CvCity* pStartCity;
#endif
#if defined(MOD_BALANCE_CORE)
	// look for good captured civilians of ours (settlers and workers, not missionaries) 
	// these will be even more important than just a camp
	// btw - the AI will cheat here - as a human I would use a combination of memory and intuition to find these, since our current AI has neither of these...
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);

	CvUnit* pLoopUnit = NULL;
	int iLoop;
	for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
	{
		if (pLoopUnit->GetOriginalOwner() == m_eOwner && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST))
		{
			CvCity* pLoopCity;
			int iCityLoop;
			// Loop through each of our cities
			for(pLoopCity = GET_PLAYER(m_eOwner).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eOwner).nextCity(&iCityLoop))
			{
				if(pLoopCity != NULL)
				{
					if(pLoopUnit->plot()->getArea() != pLoopCity->getArea() && !GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).canEmbark())
					{
						continue;
					}
					iCurPlotDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pLoopUnit->plot(), pLoopCity->plot());
					
					if (iCurPlotDistance < iBestPlotDistance)
					{
						pBestPlot = pLoopUnit->plot();
						pBestMuster = pLoopCity->plot();
						iBestPlotDistance = iCurPlotDistance;
						m_bCivilianRescue = true;
						m_iUnitToRescue = pLoopUnit->GetID();
					}
				}
			}
		}
	}

	if (pBestPlot == NULL)
	{
		// Look at map for Barbarian camps - don't check if they are revealed ... that's the cheating part
		for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
			if (pPlot->getImprovementType() == eBarbCamp)
			{
				CvCity* pLoopCity;
				int iCityLoop;
				// Loop through each of our cities
				for(pLoopCity = GET_PLAYER(m_eOwner).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eOwner).nextCity(&iCityLoop))
				{
					if(pLoopCity != NULL)
					{
						if(pPlot->getArea() != pLoopCity->getArea() && !GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).canEmbark())
						{
							continue;
						}
						iCurPlotDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pPlot, pLoopCity->plot());
						if(pPlot->getArea() != pLoopCity->getArea())
						{
							iCurPlotDistance *= 2;
						}
						if (iCurPlotDistance < iBestPlotDistance)
						{
							pBestPlot = pPlot;
							pBestMuster = pLoopCity->plot();
							iBestPlotDistance = iCurPlotDistance;
						}
					}
				}
			}
		}
	}
	if(pBestMuster != NULL)
	{
		SetMusterPlot(pBestMuster);
	}
#else
	pStartCity = GetOperationStartCity();
	if(pStartCity != NULL)
	{

		// look for good captured civilians of ours (settlers and workers, not missionaries) 
		// these will be even more important than just a camp
		// btw - the AI will cheat here - as a human I would use a combination of memory and intuition to find these, since our current AI has neither of these...
		CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);

		CvUnit* pLoopUnit = NULL;
		int iLoop;
		for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
		{
			if (pLoopUnit->GetOriginalOwner() == m_eOwner && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST))
			{
				iCurPlotDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pLoopUnit->plot(), pStartCity->plot());
				if (iCurPlotDistance < iBestPlotDistance)
				{
					pBestPlot = pLoopUnit->plot();
					iBestPlotDistance = iCurPlotDistance;
					m_bCivilianRescue = true;
					m_iUnitToRescue = pLoopUnit->GetID();
				}
			}
		}

		if (!pBestPlot)
		{
			// Look at map for Barbarian camps
			for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
			{
				pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

				if (pPlot->isRevealed(eTeam))
				{
					if (pPlot->getRevealedImprovementType(eTeam) == eBarbCamp)
					{
						// Make sure camp is in the same area as our start city
						//if (pPlot->getArea() == pStartCity->getArea())
						{
							iCurPlotDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pPlot, pStartCity->plot());

							if (iCurPlotDistance < iBestPlotDistance)
							{
								pBestPlot = pPlot;
								iBestPlotDistance = iCurPlotDistance;
							}
						}
					}
				}
			}
		}
	}
#endif
	return pBestPlot;
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

/// How close to target do we end up?
int CvAIOperationPillageEnemy::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE();
}

/// Read serialized data
void CvAIOperationPillageEnemy::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationPillageEnemy::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
bool CvAIOperationPillageEnemy::ArmyMoved(CvArmyAI* pArmy)
{
	bool bStateChanged = false;
	CvPlot* pBetterTarget;

	switch(m_eCurrentState)
	{
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		// Find best pillage target
		pBetterTarget = FindBestTarget();

		// No targets at all!  Abort
		if(pBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}

		// If this is a new target, switch to it
		else if(pBetterTarget != GetTargetPlot())
		{
			SetTargetPlot(pBetterTarget);
			pArmy->SetGoalPlot(pBetterTarget);
#if defined(MOD_BALANCE_CORE)
#else
			// Reset our destination to be a few plots shy of the final target
			CvPlot* pDeployPt;
			pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), pBetterTarget, GC.getAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE(), false);
			pArmy->SetGoalPlot(pDeployPt);
#endif
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
		return CvAIOperation::ArmyMoved(pArmy);
		break;
	};

	return bStateChanged;
}

/// If at target, pillage improvements
bool CvAIOperationPillageEnemy::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if(pArmy->Plot() == pArmy->GetGoalPlot())
		{
#if defined(MOD_BALANCE_CORE)
			CvTemporaryZone zone;
			zone.SetX(pArmy->GetGoalPlot()->getX());
			zone.SetY(pArmy->GetGoalPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITADEL);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
#endif
			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Find the city that we want to pillage
CvPlot* CvAIOperationPillageEnemy::FindBestTarget()
{
	CvCity* pBestTargetCity = NULL;
	int iValue;
	int iBestValue;
	CvCity* pLoopCity;
	CvCity* pStartCity;
	int iDistance;
	int iLoop;

	CvPlayerAI& kEnemyPlayer = GET_PLAYER(m_eEnemy);

	if(!kEnemyPlayer.isAlive())
	{
		return NULL;
	}

	iBestValue = 0;
	pStartCity = GetOperationStartCity();
	if(pStartCity != NULL)
	{
		for(pLoopCity = kEnemyPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kEnemyPlayer.nextCity(&iLoop))
		{
			// Make sure city is in the same area as our start city
			if(pLoopCity->getArea() == pStartCity->getArea())
			{
				// Initial value of target is the number of improved plots
				iValue = pLoopCity->countNumImprovedPlots();

				// Adjust value based on proximity to our start location
				iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pLoopCity->plot(), pStartCity->plot());
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
#if defined(MOD_BALANCE_CORE)
			else if(GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).canEmbarkAllWaterPassage())
			{
				int iMaxDistance = MAX_INT;
				// Initial value of target is the number of improved plots
				iValue = pLoopCity->countNumImprovedPlots();
				if(iValue > 0)
				{
					CvAStarNode* pPathfinderNode;
					// Adjust value based on proximity to our start location
					bool bPathfinderSuccess = GC.getStepFinder().GeneratePath(pStartCity->getX(), pStartCity->getY(), pLoopCity->getX(), pLoopCity->getY(), m_eOwner, false);
					if(bPathfinderSuccess)
					{
						pPathfinderNode = GC.getStepFinder().GetLastNode();

						if(pPathfinderNode != NULL)
						{
							iDistance = (pPathfinderNode->m_iTotalCost / 100);
	
							if(iDistance < iMaxDistance)
							{
								iMaxDistance = iDistance;
								pBestTargetCity = pLoopCity;
							}
						}
					}
				}
			}
#endif
		}
	}
	if(pBestTargetCity == NULL)
	{
#if defined(MOD_BALANCE_CORE)
		if(GET_PLAYER(m_eEnemy).getCapitalCity() != NULL)
		{
			return GET_PLAYER(m_eEnemy).getCapitalCity()->plot();
		}
		else
		{
			return NULL;
		}
#else
		return GET_PLAYER(m_eEnemy).getCapitalCity()->plot();
#endif
	}
	else
	{
		return pBestTargetCity->plot();
	}
}

////////////////////////////////////////////////////////////////////////////////
// CvAIEscortedOperation
////////////////////////////////////////////////////////////////////////////////
CvAIEscortedOperation::CvAIEscortedOperation()
{
	m_bEscorted = true;
	m_iTargetArea = -1;
}

CvAIEscortedOperation::~CvAIEscortedOperation()
{
}

/// Kick off this operation
void CvAIEscortedOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes /* eEnemy */, int /* iDefaultArea */, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	CvUnit* pOurCivilian;
	CvPlot* pTargetSite, *pNewTarget;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;

	// Find the free civilian (that triggered this operation)
	pOurCivilian = FindBestCivilian();

	if(pOurCivilian != NULL && iID != -1)
	{
		// Find a destination (not worrying about safe paths)
		pTargetSite = FindBestTarget(pOurCivilian, false);

		if(pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);

			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());

				// Figure out the initial rally point - for this operation it is wherever our civilian is standing
				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot* pMusterPt = pOurCivilian->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				SetDefaultArea(pMusterPt->getArea());

				// Add the civilian to our army
				pArmyAI->AddUnit(pOurCivilian->GetID(), 0);

				// Skip escorting if this operation has to go across water
				if (pTargetSite->getArea() != pOurCivilian->getArea())
				{
					m_bEscorted = false;
				}

				if (m_bEscorted)
				{
					m_viListOfUnitsWeStillNeedToBuild.clear();
					OperationSlot thisOperationSlot;
					thisOperationSlot.m_iOperationID = m_iID;
					thisOperationSlot.m_iArmyID = pArmyAI->GetID();
					thisOperationSlot.m_iSlotID = 1;
					m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);

					// try to get the escort from existing units that are waiting around
					GrabUnitsFromTheReserves(pMusterPt, pTargetSite);
					if(pArmyAI->GetNumSlotsFilled() > 1)
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_bEscorted = false;
					}
				}

				if (!m_bEscorted)
				{
					// There was no escort immediately available.  Let's look for a "safe" city site instead
					pNewTarget = FindBestTarget(pOurCivilian, true);

					// If no better target, we'll wait it out for an escort
					if(pNewTarget == NULL)
					{
						m_bEscorted = true;

						// Need to add it back in to list of what to build (was cleared before since marked optional)
						m_viListOfUnitsWeStillNeedToBuild.clear();
						OperationSlot thisOperationSlot2;
						thisOperationSlot2.m_iOperationID = m_iID;
						thisOperationSlot2.m_iArmyID = pArmyAI->GetID();
						thisOperationSlot2.m_iSlotID = 1;
						m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot2);
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					// Send the civilian by himself to this safe location
					else
					{
						// Clear the list of units we need
						m_viListOfUnitsWeStillNeedToBuild.clear();

						// Change the muster point
						pArmyAI->SetGoalPlot(pNewTarget);
						SetMusterPlot(pOurCivilian->plot());
						pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());

						// Send the civilian directly to the target
						pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
				}
				LogOperationStart();
			}
		}

		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}

/// Read serialized data
void CvAIEscortedOperation::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_bEscorted;
	kStream >> m_eCivilianType;
	m_iTargetArea = -1;
	kStream >> m_iTargetArea;
}

/// Write serialized data
void CvAIEscortedOperation::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_bEscorted;
	kStream << m_eCivilianType;
	kStream << m_iTargetArea;
}

/// Always abort if settler is removed
void CvAIEscortedOperation::UnitWasRemoved(int /*iArmyID*/, int iSlotID)
{
	if(iSlotID == 0)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_LOST_CIVILIAN;
	}
	else
	{
		m_bEscorted = false;
	}
}

/// Find the civilian we want to use
CvUnit* CvAIEscortedOperation::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
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
bool CvAIEscortedOperation::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
{
	CvPlot* pBetterTarget;

	// Find best city site (taking into account whether or not we are escorted)
	pBetterTarget = FindBestTarget(pCivilian, !m_bEscorted);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}
#if defined(AUI_OPERATION_FIX_RETARGET_CIVILIAN_ABORT_IF_UNREACHABLE_ESCORT)
	// If this is a new target, switch to it
	else if(pBetterTarget && pBetterTarget != GetTargetPlot())
	{
		int iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(!pCivilian)
		{
			return false;
		}

		//may be null!
		CvUnit* pEscort = GET_PLAYER(m_eOwner).getUnit(pArmy->GetNextUnitID());
		if(pEscort && !pEscort->GeneratePath(pBetterTarget, MOVE_UNITS_IGNORE_DANGER, false))
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
			return false;
		}
		else
		{
#else
	// If this is a new target, switch to it
	else if(pBetterTarget != GetTargetPlot())
	{
#endif // AUI_OPERATION_FIX_RETARGET_CIVILIAN_ABORT_IF_UNREACHABLE_ESCORT
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
#if defined(AUI_OPERATION_FIX_RETARGET_CIVILIAN_ABORT_IF_UNREACHABLE_ESCORT)
		}
#endif

#if defined(MOD_BALANCE_CORE)
	}
	else if(GetTargetPlot() == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_LOST_TARGET;
		return false;
	}
#else
	}
	else
	{
		SetToAbort(AI_ABORT_REPEAT_TARGET);
		return false;
	}
#endif
	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationFoundCity
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationFoundCity::CvAIOperationFoundCity()
{
	m_eCivilianType = UNITAI_SETTLE;
}

/// Destructor
CvAIOperationFoundCity::~CvAIOperationFoundCity()
{
}


void CvAIOperationFoundCity::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int iDefaultArea, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	m_iTargetArea = iDefaultArea;

	CvUnit* pOurCivilian;
	CvPlot* pTargetSite = NULL;
	CvPlot* pNewTarget = NULL;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;

	// Find the free civilian (that triggered this operation)
	pOurCivilian = FindBestCivilian();

	if(pOurCivilian != NULL && iID != -1)
	{
		// Find a destination (not worrying about safe paths)
		pTargetSite = FindBestTarget(pOurCivilian, false);

		if(pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);

			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());

				// Figure out the initial rally point - for this operation it is wherever our civilian is standing
				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot* pMusterPt = pOurCivilian->plot();

#if defined(MOD_BALANCE_CORE)
				//loop through all our cities and find the best plot for mustering
				CvPlot* pBestMusterPlot = NULL;
				int iBestTurns = MAX_INT;

				int iLoopCity = 0;
				CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
				if(kPlayer.getNumCities() == 1 && kPlayer.getCapitalCity() != NULL)
				{
					pBestMusterPlot = kPlayer.getCapitalCity()->plot();
				}
				else
				{
					for(CvCity* pLoopCity = kPlayer.firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoopCity))
					{
						if(kPlayer.getNumCities() > 1 && kPlayer.GetMilitaryAI()->GetMostThreatenedCity(0) == pLoopCity)
						{
							continue;
						}

						//todo: check if the city is in danger first?
						bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pOurCivilian, pLoopCity->getX(), pLoopCity->getY(), pTargetSite->getX(), pTargetSite->getY(), iBestTurns);

						int iTurns = bCanFindPath ? GC.getPathFinder().GetLastNode()->m_iData2 : INT_MAX;
						if(iTurns < iBestTurns)
						{
							iBestTurns = iTurns;
							pBestMusterPlot = pLoopCity->plot();
						}
					}
				}

				if(pBestMusterPlot != NULL)
				{
					pMusterPt = pBestMusterPlot;
					//If we can, let's muster next to the city - less chance of getting blocked in that way (during the escort op).
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pMusterPt->getX(), pMusterPt->getY(), ((DirectionTypes)iDirectionLoop));
						if (!pAdjacentPlot)
							continue;

						int iDanger = GET_PLAYER(m_eOwner).GetPlotDanger(*pAdjacentPlot,pOurCivilian);
						if(!pAdjacentPlot->isWater() && pAdjacentPlot->getOwner() == m_eOwner && pAdjacentPlot->getNumUnits()==0 && iDanger==0)
						{
							pMusterPt = pAdjacentPlot;
							break;
						}
					}
				}
#endif
			
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				SetDefaultArea(pMusterPt->getArea());

				// Add the settler to our army
				pArmyAI->AddUnit(pOurCivilian->GetID(), 0);

				// Add the escort as a unit we need to build
				m_viListOfUnitsWeStillNeedToBuild.clear();
				OperationSlot thisOperationSlot;
				thisOperationSlot.m_iOperationID = m_iID;
				thisOperationSlot.m_iArmyID = pArmyAI->GetID();
				thisOperationSlot.m_iSlotID = 1;
				m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);

				// try to get the escort from existing units that are waiting around
				GrabUnitsFromTheReserves(pMusterPt, pTargetSite);
				if(pArmyAI->GetNumSlotsFilled() > 1)
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					// There was no escort immediately available.  Let's look for a "safe" city site instead
#if defined(MOD_BALANCE_CORE_SETTLER)
					if (eOwner != -1)
					{
						pNewTarget = FindBestTargetIncludingCurrent(pOurCivilian, true);

						if (GET_PLAYER(eOwner).GetCityDistance(pNewTarget) > GET_PLAYER(eOwner).GetDiplomacyAI()->GetBoldness())
							pNewTarget = NULL;
					}
#else
					if (eOwner == -1 || GET_PLAYER(eOwner).getNumCities() > 1 || GET_PLAYER(eOwner).GetDiplomacyAI()->GetBoldness() > 5) // unless we'd rather play it safe
					{
						pNewTarget = FindBestTarget(pOurCivilian, true);
					}
#endif
					// If no better target, we'll wait it out for an escort
					if(pNewTarget == NULL)
					{
						// Need to add it back in to list of what to build (was cleared before since marked optional)
						m_viListOfUnitsWeStillNeedToBuild.clear();
						OperationSlot thisOperationSlot2;
						thisOperationSlot2.m_iOperationID = m_iID;
						thisOperationSlot2.m_iArmyID = pArmyAI->GetID();
						thisOperationSlot2.m_iSlotID = 1;
						m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot2);
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					// Send the settler by himself to this safe location
					else
					{
						m_bEscorted = false;

						// Clear the list of units we need
						m_viListOfUnitsWeStillNeedToBuild.clear();

						// Change the muster point
						pArmyAI->SetGoalPlot(pNewTarget);
						SetMusterPlot(pOurCivilian->plot());
						pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());

						// Send the settler directly to the target
						pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
				}
				LogOperationStart();
			}
		}

		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}


/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAIOperationFoundCity::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pSettler = 0, *pEscort = 0;
	CvString strMsg;
#if defined(MOD_BALANCE_CORE_SETTLER)
	int m_iRetargetCount = 0;
#endif
	switch(m_eCurrentState)
	{
		// If we were gathering forces, we have to insist that any escort is in the same plot as the settler.
		// If not we'll fall through and just stay in this state.
	case AI_OPERATION_STATE_GATHERING_FORCES:

		// No escort, can just let base class handle it
		if(!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}

		// More complex if we are waiting for an escort
		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if(iUnitID != -1)
			{
				pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			else
			{
				// Escort died while gathering forces.  Abort (and return TRUE since state changed)
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_ESCORT_DIED;
				return true;
			}

			if(pSettler != NULL && pEscort != NULL && pSettler->plot() == pEscort->plot())
			{
#if defined(MOD_BALANCE_CORE_SETTLER)
				// let's see if the target still makes sense (this is modified from RetargetCivilian)
				CvPlot* pBetterTarget = FindBestTargetIncludingCurrent(pSettler, false);

				// No targets at all!  Abort
				if(pBetterTarget == NULL && GetTargetPlot() == NULL)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_TARGET;
					return false;
				}
				// If we have a target
				else
				{
					SetTargetPlot(pBetterTarget);
					pArmy->SetGoalPlot(pBetterTarget);
					m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
					return true;
				}
#else
				return CvAIOperation::ArmyInPosition(pArmy);
#endif
			}
		}
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:

		// Call base class version and see if it thinks we're done
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now get the settler
		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pSettler != NULL)
		{
#if defined(MOD_BALANCE_CORE_SETTLER)
			bool bCanFound = pSettler->canFound( GetTargetPlot() );
			if(!bCanFound && (plotDistance(GetTargetPlot()->getX(),GetTargetPlot()->getY(),pSettler->getX(),pSettler->getY()) <= 3))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Retargeting. We are close, but we can no longer settle at target (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
			// If the settler made it, we don't care about the entire army
			else if(pSettler->plot() == GetTargetPlot() && pSettler->canMove())
			{
				CvPlot* pCityPlot = pSettler->plot();
				int iPlotValue = pCityPlot->getFoundValue(m_eOwner);

				//now that the neighboring tiles are guaranteed to be revealed, recheck if we are at the best plot
				//minor twist: the nearby plots are already targeted for a city. so we need to ignore this very operation when checking the plots
				CvPlot* pAltPlot = GET_PLAYER(m_eOwner).GetBestSettlePlot(pSettler, !m_bEscorted, m_iTargetArea, this, true);
				int iAltValue = pAltPlot ? pAltPlot->getFoundValue(m_eOwner) : 0;
				int iDelta = pAltPlot ? ::plotDistance(pCityPlot->getX(),pCityPlot->getY(),pAltPlot->getX(),pAltPlot->getY()) : 0;
				//Must be much better to be worth it.
				if((iAltValue * 2) <= (GetTargetPlot()->getFoundValue(m_eOwner) * 3))
				{
					iDelta = 0;
				}
				if (iDelta == 0 || iDelta > 3 || m_iRetargetCount >= 1)
				{
					pSettler->PushMission(CvTypes::getMISSION_FOUND());

					if(GC.getLogging() && GC.getAILogging())
					{
						CvArea* pArea = pCityPlot->area();
						CvCity* pCity = pCityPlot->getPlotCity();

						if (pCity != NULL)
						{
							strMsg.Format("City founded (%s), At X=%d, At Y=%d, %s, %d, %d", pCity->getName().c_str(), pCityPlot->getX(), pCityPlot->getY(), pCity->getName().GetCString(), iPlotValue, pArea->getTotalFoundValue());
							LogOperationSpecialMessage(strMsg);
						}
					}
#if defined(MOD_BALANCE_CORE)
					// Notify tactical AI to focus on this area
					CvTemporaryZone zone;
					zone.SetX(pCityPlot->getX());
					zone.SetY(pCityPlot->getY());
					zone.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
					zone.SetLastTurn(GC.getGame().getGameTurn() + (GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() * 2));
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					iUnitID = pArmy->GetNextUnitID();
					if(iUnitID != -1)
					{
						pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						pEscort->finishMoves();
					}
#endif
					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Retargeting to adjacent plot. Target was (X=%d Y=%d value=%d), new target (X=%d Y=%d value=%d)", 
							GetTargetPlot()->getX(), GetTargetPlot()->getY(), iPlotValue, pAltPlot->getX(),pAltPlot->getY(), iAltValue );
						LogOperationSpecialMessage(strMsg);
					}
					m_iRetargetCount += 1;
					//taken from RetargetCivilian()
					SetTargetPlot(pAltPlot);
					pArmy->SetGoalPlot(pAltPlot);
					pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
					m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					
					pSettler->finishMoves();
					iUnitID = pArmy->GetNextUnitID();
					if(iUnitID != -1)
					{
						pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						pEscort->finishMoves();
					}
				}
			}
		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_ABORTED:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

#else
			if((GetTargetPlot()->getOwner() != NO_PLAYER && GetTargetPlot()->getOwner() != m_eOwner) || GetTargetPlot()->IsAdjacentOwnedByOtherTeam(pSettler->getTeam()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Retargeting. Can no longer settle at target (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
			// If the settler made it, we don't care about the entire army
			else if(pSettler->plot() == GetTargetPlot() && pSettler->canMove() && pSettler->canFound(pSettler->plot()))
			{
				CvPlot* pCityPlot = pSettler->plot();
				int iPlotValue = pCityPlot->getFoundValue(m_eOwner);
				pSettler->PushMission(CvTypes::getMISSION_FOUND());

				if(GC.getLogging() && GC.getAILogging())
				{
					CvArea* pArea = pCityPlot->area();
					CvCity* pCity = pCityPlot->getPlotCity();

					if (pCity != NULL)
					{
						strMsg.Format("City founded, At X=%d, At Y=%d, %s, %d, %d", pCityPlot->getX(), pCityPlot->getY(), pCity->getName().GetCString(), iPlotValue, pArea->getTotalFoundValue());
						LogOperationSpecialMessage(strMsg);
					}
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
			// If we're at our target but can no longer found a city, might be someone else beat us to this area
			// So move back out, picking a new target
			else if(pSettler->plot() == GetTargetPlot() && !pSettler->canFound(pSettler->plot()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("At target but can no longer settle here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};
#endif

	return bStateChanged;
}

/// Find the plot where we want to settle
#if defined(MOD_BALANCE_CORE_SETTLER)
void CvAIEscortedOperation::SetEscorted(bool bValue)
{
	m_bEscorted = bValue;
}

bool CvAIEscortedOperation::IsEscorted()
{
	return m_bEscorted;
}

CvPlot* CvAIOperationFoundCity::FindBestTargetIncludingCurrent(CvUnit* pUnit, bool bOnlySafePaths)
{
	//todo: better options
	//a) return a list of possible targets and find the ones that are currently reachable
	//b) if the best target is unreachable, move in the general direction and hope the block will clear up

	//ignore the current operation target when searching. default would be to suppress currently targeted plots
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, m_iTargetArea, this);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, -1, this);
	}
	return pResult;
}

#endif

CvPlot* CvAIOperationFoundCity::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths /*m_bEscorted*/, m_iTargetArea);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths /*m_bEscorted*/, -1);
	}
	return pResult;
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationFoundCity::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		if(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS && m_bEscorted)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
#if defined(MOD_BALANCE_CORE_SETTLER)
			UnitHandle pUnit = pThisArmy->GetFirstUnit();
			if (pUnit && (GetMusterPlot()->getOwner() != m_eOwner || GC.getGame().getGameTurn() - pUnit->getGameTurnCreated() > 10)) // 10 turns and still no escort?
			{
				// try to get the escort from existing units that are waiting around
				GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot());
				if(pThisArmy->GetNumSlotsFilled() > 1)
				{
					pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Gave up trying to find an escort for settler to (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}

					// we are going by our lonesome
					m_bEscorted = false;

					// Clear the list of units we need
					m_viListOfUnitsWeStillNeedToBuild.clear();
					// Send the settler directly to the target
					pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
					m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				}
			}
			else if (pUnit && (GetMusterPlot()->getOwner() != m_eOwner || GC.getGame().getGameTurn() - pUnit->getGameTurnCreated() > 5)) // 5 turns and still no escort even being built?
#else
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(0);
			if (GetMusterPlot()->getOwner() != m_eOwner || GC.getGame().getGameTurn() - pSlot->GetTurnAtCheckpoint() > 15) // fifteen turns and still no escort even being built?
#endif
			{
				if (m_viListOfUnitsCitiesHaveCommittedToBuild.size() == 0)
				{
					// try to get the escort from existing units that are waiting around
					GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot());
					if(pThisArmy->GetNumSlotsFilled() > 1)
					{
						pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						if(GC.getLogging() && GC.getAILogging())
						{
							CvString strMsg;
							strMsg.Format("Gave up trying to find an escort for settler to (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
							LogOperationSpecialMessage(strMsg);
						}

						// we are going by our lonesome
						m_bEscorted = false;

						// Clear the list of units we need
						m_viListOfUnitsWeStillNeedToBuild.clear();
						// Send the settler directly to the target
						pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
				}
			}
		}
#if !defined(MOD_GLOBAL_STACKING_RULES)
		else if (m_eCurrentState == AI_OPERATION_STATE_GATHERING_FORCES && m_bEscorted)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			CvArmyFormationSlot* pSlot = pThisArmy->GetFormationSlot(0);
			if (pSlot->GetUnitID() != NO_UNIT)
			{
				UnitHandle pUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
#if defined(MOD_BALANCE_CORE_SETTLER)
				if(pUnit && pUnit->plot()->isWater())
#else
				if (pUnit->plot()->isWater())
#endif
				{
					// we are going by our lonesome
					m_bEscorted = false;
					pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
					m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				}
			}
		}
#endif
	}

	return rtnValue;
}


////////////////////////////////////////////////////////////////////////////////
// CvAIOperationQuickColonize
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationQuickColonize::CvAIOperationQuickColonize()
{
}

/// Destructor
CvAIOperationQuickColonize::~CvAIOperationQuickColonize()
{
}

/// Kick off this operation
void CvAIOperationQuickColonize::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int iDefaultArea, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	CvUnit* pOurCivilian;
	CvPlot* pTargetSite;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;
	m_iTargetArea = iDefaultArea;

	// Find the free civilian (that triggered this operation)
	pOurCivilian = FindBestCivilian();

	if(pOurCivilian != NULL && iID != -1)
	{
		// Find a destination (not worrying about safe paths)
		pTargetSite = FindBestTarget(pOurCivilian, false);

		if(pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);

			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetFormationIndex(GetFormation());

				// Figure out the initial rally point - for this operation it is wherever our civilian is standing
				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot* pMusterPt = pOurCivilian->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
				SetDefaultArea(pMusterPt->getArea());

				// Add the settler to our army
				pArmyAI->AddUnit(pOurCivilian->GetID(), 0);
				m_bEscorted = false;

#if defined(MOD_BALANCE_CORE)
				// try to get the escort from existing units that are waiting around
				GrabUnitsFromTheReserves(pMusterPt, pTargetSite);
				if(pArmyAI->GetNumSlotsFilled() > 1)
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					m_bEscorted = true;
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strMsg;
						strMsg.Format("Got an escort for our quick colonization: (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				else
				{
#endif

				m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
#if defined(MOD_BALANCE_CORE)
				}
#endif
				LogOperationStart();
			}
		}

		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
}

/// Find the civilian we want to use
CvUnit* CvAIOperationQuickColonize::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
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
#if !defined(MOD_BALANCE_CORE)
/// Find the plot where we want to settle
CvPlot* CvAIOperationQuickColonize::FindBestTarget(CvUnit* pUnit, bool /*bOnlySafePaths*/)
{
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, false, m_iTargetArea);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, false, -1);
	}
	return pResult;
}

#endif
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationMerchantDelegation
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationMerchantDelegation::CvAIOperationMerchantDelegation()
{
	m_eCivilianType = UNITAI_MERCHANT;
}

/// Destructor
CvAIOperationMerchantDelegation::~CvAIOperationMerchantDelegation()
{
}

/// If at target, cash in; if at muster point, merge merchant and escort and move out
bool CvAIOperationMerchantDelegation::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pMerchant = 0, *pEscort = 0;
	CvString strMsg;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, we have to insist that any escort is in the same plot as the merchant.
		// If not we'll fall through and just stay in this state.
	case AI_OPERATION_STATE_GATHERING_FORCES:

		// No escort, can just let base class handle it
		if(!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}

		// More complex if we are waiting for an escort
		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if(iUnitID != -1)
			{
				pMerchant = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			else
			{
				// Escort died while gathering forces.  Abort (and return TRUE since state changed)
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_ESCORT_DIED;
				return true;
			}
			if(pMerchant != NULL && pEscort != NULL && pMerchant->plot() == pEscort->plot())
			{
				// let's see if the target still makes sense (this is modified from RetargetCivilian)
				CvPlot* pBetterTarget = FindBestTarget(pMerchant, true);

				// No targets at all!  Abort
				if(pBetterTarget == NULL)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_TARGET;
					return false;
				}
				// If we have a target
				else
				{
					SetTargetPlot(pBetterTarget);
					pArmy->SetGoalPlot(pBetterTarget);
				}
				return CvAIOperation::ArmyInPosition(pArmy);
			}
		}
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:

		// Call base class version and see if it thinks we're done
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now get the merchant
		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pMerchant = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pMerchant != NULL)
		{
			// If the merchant made it, we don't care about the entire army
			if(pMerchant->plot() == GetTargetPlot() && pMerchant->canMove() && pMerchant->canTrade(pMerchant->plot()))
			{
				if (pMerchant->canBuyCityState(pMerchant->plot()) && !GET_PLAYER(m_eOwner).GreatMerchantWantsCash())
				{
					pMerchant->PushMission(CvTypes::getMISSION_BUY_CITY_STATE());
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Great Merchant buying city-state, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				else
				{
					pMerchant->PushMission(CvTypes::getMISSION_TRADE());
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Great Merchant finishing trade mission, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}

			// Does it look like we should be done?
			else if(pMerchant->plot() == GetTargetPlot())
			{
				// We're at our target but can no longer trade, city state was probably conquered
				if(!pMerchant->canTrade(pMerchant->plot()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("At target but can no longer trade here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
					RetargetCivilian(pMerchant, pArmy);
					pMerchant->finishMoves();
					iUnitID = pArmy->GetNextUnitID();
					if(iUnitID != -1)
					{
						pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						pEscort->finishMoves();
					}
				}
			}

#if defined(MOD_BALANCE_CORE)
			// If the merchant made it, we don't care about the entire army
			if(pMerchant->plot()->getOwner() == GetTargetPlot()->getOwner() && pMerchant->canMove() && pMerchant->canTrade(pMerchant->plot()))
			{
				if (pMerchant->canBuyCityState(pMerchant->plot()) && !GET_PLAYER(m_eOwner).GreatMerchantWantsCash())
				{
					pMerchant->PushMission(CvTypes::getMISSION_BUY_CITY_STATE());
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Great Merchant buying city-state, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}
				else
				{
					pMerchant->PushMission(CvTypes::getMISSION_TRADE());
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Great Merchant finishing trade mission, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
#endif

		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Find the plot where we want to settler
CvPlot* CvAIOperationMerchantDelegation::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvAssertMsg(pUnit, "pUnit cannot be null");
	if(!pUnit)
	{
		return NULL;
	}

	return GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlot(pUnit, !bOnlySafePaths /*m_bEscorted*/);
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDiplomatDelegation
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationDiplomatDelegation::CvAIOperationDiplomatDelegation()
{
	m_eCivilianType = UNITAI_DIPLOMAT;
}

/// Destructor
CvAIOperationDiplomatDelegation::~CvAIOperationDiplomatDelegation()
{
}

/// If at target, cash in; if at muster point, merge Diplomat and escort and move out
bool CvAIOperationDiplomatDelegation::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pDiplomat = 0, *pEscort = 0;
	CvString strMsg;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, we have to insist that any escort is in the same plot as the Diplomat.
		// If not we'll fall through and just stay in this state.
	case AI_OPERATION_STATE_GATHERING_FORCES:
		// No escort, can just let base class handle it
		if(!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}

		// More complex if we are waiting for an escort
		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if(iUnitID != -1)
			{
				pDiplomat = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			else
			{
				// Escort died while gathering forces.  Abort (and return TRUE since state changed)
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_ESCORT_DIED;
				return true;
			}
			if(pDiplomat != NULL && pEscort != NULL && pDiplomat->plot() == pEscort->plot())
			{
				// let's see if the target still makes sense (this is modified from RetargetCivilian)
				CvPlot* pBetterTarget = FindBestTarget(pDiplomat, true);

				// No targets at all!  Abort
				if(pBetterTarget == NULL)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_TARGET;
					return false;
				}
				// If we have a target
				else
				{
					SetTargetPlot(pBetterTarget);
					pArmy->SetGoalPlot(pBetterTarget);
				}
				return CvAIOperation::ArmyInPosition(pArmy);
			}
		}
		break;
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:

		// Call base class version and see if it thinks we're done
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now get the Diplomat
		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pDiplomat = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pDiplomat != NULL)
		{
			// If the Diplomat made it, we don't care about the entire army
			if(pDiplomat->plot() == GetTargetPlot() && pDiplomat->canMove() && pDiplomat->canTrade(pDiplomat->plot()))
			{
				pDiplomat->PushMission(CvTypes::getMISSION_TRADE());

				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Great Diplomat finishing Diplomatic Mission at %s", pDiplomat->plot()->GetAdjacentCity()->getName().c_str());
					LogOperationSpecialMessage(strMsg);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}

			// Does it look like we should be done?
			else if(pDiplomat->plot() == GetTargetPlot())
			{
				// We're at our target but can no longer trade, city state was probably conquered
				if(!pDiplomat->canTrade(pDiplomat->plot()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("At target but can no longer do diplomacy here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
					RetargetCivilian(pDiplomat, pArmy);
					pDiplomat->finishMoves();
					iUnitID = pArmy->GetNextUnitID();
					if(iUnitID != -1)
					{
						pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						pEscort->finishMoves();
					}
				}
			}
		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Find the plot where we want to influence
CvPlot* CvAIOperationDiplomatDelegation::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvAssertMsg(pUnit, "pUnit cannot be null");
	if(!pUnit)
	{
		return NULL;
	}
	int iTargetTurns;
	bOnlySafePaths= true;
	return GET_PLAYER(pUnit->getOwner()).ChooseMessengerTargetPlot(pUnit, &iTargetTurns);
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationAllyDefense - 
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationAllyDefense::CvAIOperationAllyDefense()
{
}

/// Destructor
CvAIOperationAllyDefense::~CvAIOperationAllyDefense()
{
}

/// Kick off this operation
void CvAIOperationAllyDefense::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{
		if(pTarget != NULL && pMuster != NULL)
		{
			SetTargetPlot(pTarget->plot());
			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());	
				
				pArmyAI->SetGoalPlot(GetTargetPlot());
				SetMusterPlot(pMuster->plot());
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());

				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
				if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}

				LogOperationStart();
			}
		}
		else
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}
	}
}
/// Same as default version except if just gathered forces and this operation never reaches a final target (just keeps attacking until dead or the operation is ended)
bool CvAIOperationAllyDefense::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		// First do base case processing
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now revisit target
		if(GetTargetPlot()->getOwner() != m_eEnemy)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}
	}
	break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}
/// Read serialized data
void CvAIOperationAllyDefense::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
}

/// Write serialized data
void CvAIOperationAllyDefense::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}
/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationAllyDefense::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		// See if our target city is still owned by our enemy
		if(GetTargetPlot()->getOwner() != m_eEnemy)
		{
			// Bummer!  The city has been captured/destroyed
			return true;
		}
		else if(GET_PLAYER(m_eEnemy).isMinorCiv())
		{
			if(!GET_PLAYER(m_eEnemy).GetMinorCivAI()->IsActiveQuestForPlayer(m_eOwner, MINOR_CIV_QUEST_HORDE) && !GET_PLAYER(m_eEnemy).GetMinorCivAI()->IsActiveQuestForPlayer(m_eOwner, MINOR_CIV_QUEST_REBELLION))
			{
				return true;
			}
		}
	}

	return rtnValue;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationConcertTour
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationConcertTour::CvAIOperationConcertTour()
{
	m_eCivilianType = UNITAI_MUSICIAN;
}

/// Destructor
CvAIOperationConcertTour::~CvAIOperationConcertTour()
{
}

/// If at target, rock on! If at muster point, merge musician and escort and move out
bool CvAIOperationConcertTour::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit* pMusician = 0, *pEscort = 0;
	CvString strMsg;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, we have to insist that any escort is in the same plot as the merchant.
		// If not we'll fall through and just stay in this state.
	case AI_OPERATION_STATE_GATHERING_FORCES:

		// No escort, can just let base class handle it
		if(!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}

		// More complex if we are waiting for an escort
		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if(iUnitID != -1)
			{
				pMusician = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			else
			{
				// Escort died while gathering forces.  Abort (and return TRUE since state changed)
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_ESCORT_DIED;
				return true;
			}
			if(pMusician != NULL && pEscort != NULL && pMusician->plot() == pEscort->plot())
			{
				// let's see if the target still makes sense (this is modified from RetargetCivilian)
				CvPlot* pBetterTarget = FindBestTarget(pMusician, true);

				// No targets at all!  Abort
				if(pBetterTarget == NULL)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_TARGET;
					return false;
				}
				// If we have a target
				else
				{
					SetTargetPlot(pBetterTarget);
					pArmy->SetGoalPlot(pBetterTarget);
				}
				return CvAIOperation::ArmyInPosition(pArmy);
			}
		}
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:

		// Call base class version and see if it thinks we're done
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now get the musician
		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pMusician = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pMusician != NULL)
		{
			// If the merchant made it, we don't care about the entire army
			if(pMusician->plot() == GetTargetPlot() && pMusician->canMove() && pMusician->canBlastTourism(pMusician->plot()))
			{
				pMusician->PushMission(CvTypes::getMISSION_ONE_SHOT_TOURISM());
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Great Musician performing concert tour, At X=%d, At Y=%d", pMusician->plot()->getX(), pMusician->plot()->getY());
					LogOperationSpecialMessage(strMsg);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}

			// Does it look like we should be done?
			else if(pMusician->plot() == GetTargetPlot())
			{
				// We're at our target but can no longer perform concert, enemy city was probably conquered
				if(!pMusician->canBlastTourism(pMusician->plot()))
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("At target but can no longer hold concert here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
					RetargetCivilian(pMusician, pArmy);
					pMusician->finishMoves();
					iUnitID = pArmy->GetNextUnitID();
					if(iUnitID != -1)
					{
						pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						pEscort->finishMoves();
					}
				}
			}
		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Find the plot where we want to settler
CvPlot* CvAIOperationConcertTour::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvAssertMsg(pUnit, "pUnit cannot be null");
	if(!pUnit)
	{
		return NULL;
	}

	return GET_PLAYER(pUnit->getOwner()).FindBestMusicianTargetPlot(pUnit, !bOnlySafePaths /*m_bEscorted*/);
}

////////////////////////////////////////////////////////////////////////////////
// CvAINavalOperation
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAINavalOperation::CvAINavalOperation()
{
}

/// Destructor
CvAINavalOperation::~CvAINavalOperation()
{
}
#if defined(MOD_BALANCE_CORE)
/// Kick off this operation
void CvAINavalOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			if(pTarget)
			{
				SetTargetPlot(pTarget->plot());
				pArmyAI->SetGoalPlot(GetTargetPlot());

				// Muster just off the coast
				CvPlot *pCoastalMuster = kPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
				if (pCoastalMuster && pCoastalMuster->GetNumCombatUnits() > 0)
				{
					pCoastalMuster = kPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pCoastalMuster, pArmyAI);
				}
				else if(pCoastalMuster)
				{
					SetDefaultArea(pCoastalMuster->getArea());
					SetStartCityPlot(pCoastalMuster);
					SetMusterPlot(pCoastalMuster);
					pArmyAI->SetXY(pCoastalMuster->getX(), pCoastalMuster->getY());

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{
					// No muster point, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}

/// Same as default version except if just gathered forces and this operation never reaches a final target (just keeps attacking until dead or the operation is ended)
bool CvAINavalOperation::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
		{
			// First do base case processing
			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

			// Is target still under enemy control?
			CvPlot* pTarget = GetTargetPlot();
			if(pTarget->getOwner() != m_eEnemy)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
			}
		}
		break;

		// See if within 2 spaces of our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
#if defined(MOD_BALANCE_CORE)
			bool bBeenHad = false;
			CvString strMsg;
			if(pArmy && !GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
			{
				UnitHandle pUnit;
				pUnit = pArmy->GetFirstUnit();
				while(pUnit)
				{
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
						if(pAdjacentPlot != NULL)
						{
							CvUnit* pOtherUnit = pAdjacentPlot->getUnitByIndex(0);
							if(pOtherUnit != NULL && pOtherUnit->getOwner() == m_eEnemy)
							{
								// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
								if(GC.getLogging() && GC.getAILogging())
								{
									strMsg.Format("Ran into enemy unit during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
									LogOperationSpecialMessage(strMsg);
								}
								bBeenHad = true;
								break;
							}
						}
					}
					pUnit = pArmy->GetNextUnit();
				}
			}
			if(bBeenHad)
			{
				// Notify Diplo AI we're in place for attack
				GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

				// Notify tactical AI to focus on this area
				CvTemporaryZone zone;
				zone.SetX(GetTargetPlot()->getX());
				zone.SetY(GetTargetPlot()->getY());
				zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
				zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
				GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
#endif
			if (plotDistance(pArmy->Plot()->getX(), pArmy->Plot()->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) < 2)
			{
				// Notify tactical AI to focus on this area
				CvTemporaryZone zone;
				zone.SetX(GetTargetPlot()->getX());
				zone.SetY(GetTargetPlot()->getY());
				zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
				zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
				GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAINavalOperation::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		// See if our target city is still owned by our enemy
		if(GetTargetPlot()->getOwner() != m_eEnemy)
		{
			// Success!  The city has been captured/destroyed
			return true;
		}
	}

	return rtnValue;
}
#endif
/// Read serialized data
void CvAINavalOperation::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAINavalOperation::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// How close to target do we end up?
int CvAINavalOperation::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_NAVAL_BOMBARDMENT_DEPLOY_RANGE();
}

/// Find the port our operation will leave from
CvCity* CvAINavalOperation::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	int iLoop;
	CvCity* pCity;

	// Just find first coastal city
	for(pCity = GET_PLAYER(m_eOwner).firstCity(&iLoop); pCity != NULL; pCity = GET_PLAYER(m_eOwner).nextCity(&iLoop))
	{
		if(pCity->isCoastal())
		{
			return pCity;
		}
	}

	return NULL;
}

/// Figure out the initial rally point
CvPlot* CvAINavalOperation::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
{
	CvPlot* pMusterPt = NULL;
	CvPlot* pStartCityPlot;
	CvCity* pStartCity;
	CvPlot* pCurrentPlot;
	CvPlot* pAdjacentPlot;
	int iSpacesFromTarget = 0;
	int iDirectionLoop;
	CvAStarNode* pNode;
	bool bMusterPointFound = false;

	pStartCity = GetOperationStartCity();
	if(pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if(pStartCityPlot != NULL)
		{
			// Find an adjacent water tile
			for(iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES && !bMusterPointFound; ++iDirectionLoop)
			{
				pAdjacentPlot = plotDirection(pStartCityPlot->getX(), pStartCityPlot->getY(), ((DirectionTypes)iDirectionLoop));
				if(pAdjacentPlot != NULL && pAdjacentPlot->isWater())
				{
					// Generate path
					GC.getStepFinder().SetData(&m_eEnemy);
					if(GC.getStepFinder().GeneratePath(pAdjacentPlot->getX(), pAdjacentPlot->getY(), pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY(), m_eOwner, false))
					{
						pNode = GC.getStepFinder().GetLastNode();

						// Starting at the end, loop until we find a plot from this owner
						while(pNode != NULL)
						{
							pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);

							// Is this the deploy point?
#if defined(MOD_BALANCE_CORE)
							if(iSpacesFromTarget <= GetDeployRange())
#else
							if(iSpacesFromTarget == GetDeployRange())
#endif
							{
								pMusterPt = pCurrentPlot;
								bMusterPointFound = true;
								break;
							}

							// Move to the previous plot on the path
							iSpacesFromTarget++;
							pNode = pNode->m_pParent;
						}
					}
				}
			}
		}
	}

	if(pMusterPt != NULL)
	{
		SetMusterPlot(pMusterPt);
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("No muster point found, Naval operation aborting, Target was, X: %d, Y: %d", pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY());
			LogOperationSpecialMessage(szMsg);
		}
	}

	return pMusterPt;
}

/// Which unit would we like to use to kick off this operation?
CvUnit* CvAINavalOperation::FindInitialUnit()
{
	int iUnitLoop;
	CvUnit* pLoopUnit;

	for(pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
			{
				CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
				if(pkUnitEntry && pkUnitEntry->GetUnitAIType(UNITAI_ATTACK_SEA))
				{
					if(pLoopUnit->getArmyID() == -1)
					{
						return pLoopUnit;
					}
				}
			}
		}
	}
	return NULL;
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
void CvAIOperationNavalBombardment::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
#if defined(MOD_BALANCE_CORE)
	if(m_eEnemy == NO_PLAYER)
	{
		m_eEnemy = BARBARIAN_PLAYER;
	}
#endif

	if(iID != -1 && GetOperationStartCity())
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			// Figure out the initial rally point
			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
#if defined(MOD_BALANCE_CORE)
				//Let's just muster at our target.
				SetMusterPlot(pTargetPlot);
				if(GetMusterPlot() != NULL)
#else
				if(SelectInitialMusterPoint(pArmyAI) != NULL)
#endif
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{
					// No muster point, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
#if defined(MOD_BALANCE_CORE)
				m_eAbortReason = AI_ABORT_NO_TARGET;
#else
				m_eAbortReason = AI_ABORT_LOST_TARGET;
#endif
			}
		}
	}
}

/// Read serialized data
void CvAIOperationNavalBombardment::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationNavalBombardment::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// Same as default version except if just gathered forces, check to see if a better target has presented itself
bool CvAIOperationNavalBombardment::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		// First do base case processing
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now revisit target
#if defined(MOD_BALANCE_CORE)
#else
		CvPlot* possibleBetterTarget;
#endif
#if defined(MOD_BALANCE_CORE)
		CvPlot* possibleBetterTarget = NULL;
		if(GetTargetPlot() == NULL)
		{
#endif
		possibleBetterTarget = FindBestTarget();
#if defined(MOD_BALANCE_CORE)
		}
		if(possibleBetterTarget != NULL)
		{
			SetTargetPlot(possibleBetterTarget);
		}
#endif
		// If no target left, abort
#if defined(MOD_BALANCE_CORE)
		if(GetTargetPlot() == NULL)
#else
		if(possibleBetterTarget == NULL)
#endif
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}

		// If target changed, reset to this new one
#if defined(MOD_BALANCE_CORE)
		else
#else
		else if(possibleBetterTarget != GetTargetPlot())
#endif
		{
			// Reset our destination to be a few plots shy of the final target
#if defined(MOD_BALANCE_CORE)
			CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), GetTargetPlot(), GetDeployRange(), false);
#else
			CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GetDeployRange(), false);
#endif

			if(pDeployPt != NULL)
			{
				pArmy->SetGoalPlot(pDeployPt);
#if defined(MOD_BALANCE_CORE)
				SetMusterPlot(pDeployPt);
#else
				SetTargetPlot(possibleBetterTarget);
#endif
			}
		}
	}
	break;

	// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		if(pArmy->Plot() == pArmy->GetGoalPlot())
		{
			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_BOMBARDMENT_ZONE);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Find the barbarian camp we want to eliminate
CvPlot* CvAIOperationNavalBombardment::FindBestTarget()
{
#if defined(MOD_BALANCE_CORE)
	if(GET_PLAYER(m_eOwner).getCapitalCity() == NULL)
	{
		return NULL;
	}
	CvPlot* pBestTarget = NULL;
	int iBestPlotDistance = MAX_INT;
	int iCurrentTurns;
	CvUnit* pInitialUnit = NULL;
	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvUnit* pLoopUnit = NULL;
	int iLoop;
	if(GetFirstArmyID() != -1)
	{
		CvArmyAI* pThisArmy = owningPlayer.getArmyAI(m_viArmyIDs[0]);
		int iUnitID = pThisArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pInitialUnit = owningPlayer.getUnit(iUnitID);
		}
		else
		{
			pInitialUnit = FindInitialUnit();
		}
	}

	if(pInitialUnit != NULL)
	{
		//For minor civs
		if(!GET_PLAYER(m_eEnemy).isBarbarian())
		{
			CvCity* pCapital = GET_PLAYER(m_eOwner).getCapitalCity();
			if(!pCapital->isCoastal())
			{
				return NULL;
			}
			int iCityLoop = 0;
			CvCity* pLoopCity;
			for(pLoopCity = GET_PLAYER(m_eEnemy).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eEnemy).nextCity(&iCityLoop))
			{
				if(pLoopCity != NULL && pLoopCity->isCoastal())
				{
					CvPlot* pTestPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pLoopCity->plot(), NULL);
					CvPlot* pTestPlot2 = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pCapital->plot(), NULL);
					if(pTestPlot == NULL || pTestPlot2 == NULL)
					{
						continue;
					}
					if(pTestPlot->getArea() == pTestPlot2->getArea())
					{
						if(pInitialUnit->GeneratePath(pTestPlot, 0, false, &iCurrentTurns))
						{
							if(iCurrentTurns < iBestPlotDistance)
							{
								pBestTarget = pTestPlot;
								iBestPlotDistance = iCurrentTurns;
							}
						}
					}
				}
			}
		}
		else
		{
			int iCurrentTurns;
			for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
			{
				if (pLoopUnit != NULL)
				{
					if(pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit())
					{
						if(pInitialUnit->GeneratePath(pLoopUnit->plot(), 0, false, &iCurrentTurns))
						{
							if(iCurrentTurns < iBestPlotDistance)
							{
								pBestTarget = pLoopUnit->plot();
								iBestPlotDistance = iCurrentTurns;
							}
						}
					}
				}
			}
		}
		if(pBestTarget == NULL && GET_PLAYER(m_eEnemy).isBarbarian())
		{
			for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
			{
				if (pLoopUnit != NULL)
				{
					if(pLoopUnit->plot()->isCoastalLand() && pLoopUnit->IsCombatUnit() && pLoopUnit->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
					{
						CvPlot* pTestPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pLoopUnit->plot(), NULL);
						if(pTestPlot != NULL && pInitialUnit->GeneratePath(pTestPlot, 0, false, &iCurrentTurns))
						{
							if(iCurrentTurns < iBestPlotDistance)
							{
								pBestTarget = pTestPlot;
								iBestPlotDistance = iCurrentTurns;
							}
						}
					}
				}
			}
		}
	}
	if(pBestTarget != NULL)
	{
		if(pBestTarget->isWater())
		{
			return pBestTarget;
		}
	}
	return NULL;
#else
	int iPlotLoop, iDirectionLoop;
	CvPlot* pBestPlot = NULL;
	CvPlot* pPlot;
	CvPlot* pAdjacentPlot;
	int iBestTurns = MAX_INT;
	int iCurrentTurns;
	CvUnit* pInitialUnit;

	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);

	if(GetFirstArmyID() == -1)
	{
		pInitialUnit = FindInitialUnit();
	}
	else
	{
		CvArmyAI* pThisArmy = owningPlayer.getArmyAI(m_viArmyIDs[0]);
		int iUnitID = pThisArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pInitialUnit = owningPlayer.getUnit(iUnitID);
		}
		else
		{
			pInitialUnit = FindInitialUnit();
		}
	}

	if(pInitialUnit != NULL)
	{
		// Look at map for enemy units on the coast
		for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
			{
				if(pPlot->isCoastalLand())
				{
					// Enemy defender here? (for now let's not add cities; they fire back!)
					CvUnit* pUnit = pPlot->getVisibleEnemyDefender(m_eOwner);
					if(pUnit)
					{
						// Find an adjacent coastal water tile
						for(iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));
							if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
							{
								if(pInitialUnit->GeneratePath(pAdjacentPlot, 0, false, &iCurrentTurns))
								{
									if(iCurrentTurns < iBestTurns)
									{
										iBestTurns = iCurrentTurns;
										pBestPlot = pAdjacentPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return pBestPlot;
#endif
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
#if defined(MOD_BALANCE_CORE)
void CvAIOperationNavalSuperiority::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity* pTarget, CvCity* pMuster)
#else
void CvAIOperationNavalSuperiority::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
#endif
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_FREEFORM_NAVAL;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
#if defined(MOD_BALANCE_CORE)
	if(iID != -1)
#else
	if(iID != -1 && GetOperationStartCity())
#endif
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());
#if defined(MOD_BALANCE_CORE)
			if(pMuster != NULL)
			{
				CvPlot* pPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
				SetMusterPlot(pPlot);
			}
			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_MUSTER;
			}
			if(pTarget != NULL)
			{		
				SetTargetPlot(pTarget->plot());
				CvPlot* pPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), pArmyAI);
				if(GetTargetPlot() != NULL && GetMusterPlot() != NULL)
				{
					pArmyAI->SetGoalPlot(pPlot);
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(iDefaultArea);
				}
			}
			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
#else
			// Figure out the initial rally point
			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if(SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());
#endif
					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
#if defined(MOD_BALANCE_CORE)
					if(GrabUnitsFromTheReserves(GetTargetPlot(), GetMusterPlot()))
#else
					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
#endif
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
#if defined(MOD_BALANCE_CORE)
#else
			}
				else
				{
					// No muster point, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}
			else
			{
				// No muster point, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_MUSTER;
			}
#endif
		}
	}
}

/// Read serialized data
void CvAIOperationNavalSuperiority::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationNavalSuperiority::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// Same as default version except if just gathered forces and this operation never reaches a final target (just keeps attacking until dead or the operation is ended)
bool CvAIOperationNavalSuperiority::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		// First do base case processing
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now revisit target
		CvPlot* possibleBetterTarget;
		possibleBetterTarget = FindBestTarget();

		// If no target left, abort
		if(possibleBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}

		// If target changed, reset to this new one
		else if(possibleBetterTarget != GetTargetPlot())
		{
			pArmy->SetGoalPlot(possibleBetterTarget);
			SetTargetPlot(possibleBetterTarget);
		}
	}
	break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}
#if defined(MOD_BALANCE_CORE)
/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationNavalSuperiority::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		if((FindBestTarget() == NULL) && (GetTargetPlot() == NULL))
		{
			return true;
		}
	}

	return rtnValue;
}
#endif
typedef CvWeightedVector<CvPlot*, 1, true> WeightedPlotVector;

//	---------------------------------------------------------------------------
//	Return the first reachable plot in the weighted plot list.
//	It is assumed that the list has yet to be sorted and will do so.
static CvPlot* GetReachablePlot(UnitHandle pUnit, WeightedPlotVector& aPlots, int *iTurns)
{
	CvPlot *pFoundPlot = NULL;
	int iFoundWeight = 0;
	int iFoundTurns = 0;
	uint uiListSize;	
	if ((uiListSize = aPlots.size()) > 0)
	{
		aPlots.SortItems();

		// This will check all the plots that have the same weight.  It will mean a few more path-finds, but it will
		// be more accurate.
		for (uint i = uiListSize; i--; )		// Go backward, the CvWeightedVector sorts highest to lowest
		{
			CvPlot* pPlot = aPlots.GetElement(i);
			int iWeight = aPlots.GetWeight(i);

			if (pFoundPlot)
			{
				if (iWeight > iFoundWeight)
					break;		// Already found one of a lower weight
			
#ifdef AUI_ASTAR_TURN_LIMITER
				int iTurnsCalculated = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/, false, false, iFoundTurns);
#else
				int iTurnsCalculated = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/, false);
#endif // AUI_ASTAR_TURN_LIMITER
				if (iTurnsCalculated != MAX_INT)
				{
					if (iTurnsCalculated < iFoundTurns)
					{
						iFoundWeight = iWeight;
						pFoundPlot = pPlot;
						iFoundTurns = iTurnsCalculated;
						if (iFoundTurns == 1)
							break;		// Not getting better than this
					}
				}
			}
			else
			{
				int iTurnsCalculated = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/, false);
				if (iTurnsCalculated != MAX_INT)
				{
					iFoundWeight = iWeight;
					pFoundPlot = pPlot;
					iFoundTurns = iTurnsCalculated;
					if (iFoundTurns == 1)
						break;		// Not getting better than this
				}
			}
		}
	}

	if (pFoundPlot)
	{
		if (iTurns)
			*iTurns = iFoundTurns;
		return pFoundPlot;
	}

	return NULL;
}
/// Find the nearest enemy naval unit to eliminate
CvPlot* CvAIOperationNavalSuperiority::FindBestTarget()
{
	CvPlot* pPlot;
	CvPlot* pBestPlot = NULL;
	CvUnit* pInitialUnit;
#if defined(MOD_BALANCE_CORE)
#else
	CvCity* pCity;
	CvCity* pEnemyCoastalCity = NULL;
	CvPlot* pCoastalBarbarianCamp = NULL;
	int iClosestEnemyDistance = MAX_INT;
	int iClosestCampDistance = MAX_INT;
	int iDistance;
#endif
	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);
	if(GetFirstArmyID() == -1)
	{
		pInitialUnit = FindInitialUnit();
	}
	else
	{
		CvArmyAI* pThisArmy = owningPlayer.getArmyAI(m_viArmyIDs[0]);
		int iUnitID = pThisArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pInitialUnit = owningPlayer.getUnit(iUnitID);
		}
		else
		{
			pInitialUnit = FindInitialUnit();
		}
	}
#if defined(MOD_BALANCE_CORE)
	if(GetTargetPlot() != NULL)
	{
		return GetTargetPlot();
	}
	else if(pInitialUnit != NULL)
	{
		WeightedPlotVector aPlotList;
		aPlotList.reserve(64);
		int iUnitX = pInitialUnit->getX();
		int iUnitY = pInitialUnit->getY();
		int iBaseMoves = pInitialUnit->baseMoves();
		for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if(pPlot->isRevealed(owningPlayer.getTeam()))
			{
				if(pPlot->isWater())
				{
					if(pPlot->getNumUnits() > 0)
					{
						int iPlotDistance = plotDistance(iUnitX, iUnitY, pPlot->getX(), pPlot->getY());
						for(int iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
						{
							CvUnit* pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
							{
								if(pLoopUnit->isEnemy(owningPlayer.getTeam()))
								{
									int iScore = iBaseMoves * iPlotDistance;
									if (pLoopUnit->isTrade()) // we want to plunder trade routes of possible
									{
										iScore /= 3;
									}
									if (pLoopUnit->isEmbarked()) // we want to take out embarked units more than ships
									{
										iScore = (iScore * 2) / 3;
									}

									aPlotList.push_back(pPlot, iScore);
								}
							}
						}
					}
				}
			}
		}
		int iBestTurns;
		pBestPlot = GetReachablePlot(pInitialUnit, aPlotList, &iBestTurns);
		// None found, patrol over near closest enemy coastal city, or if not that a water tile adjacent to a camp
		if(pBestPlot != NULL)
		{
			return pBestPlot;
		}
	}
#else
	if(pInitialUnit != NULL)
	{
		WeightedPlotVector aPlotList;
		aPlotList.reserve(64);
		int iUnitX = pInitialUnit->getX();
		int iUnitY = pInitialUnit->getY();
		int iBaseMoves = pInitialUnit->baseMoves();
		// Look at map for enemy naval units
		for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if(pPlot->isRevealed(owningPlayer.getTeam()))
			{
				if(pPlot->isWater())
				{
					if(pPlot->getNumUnits() > 0)
					{
						int iPlotDistance = plotDistance(iUnitX, iUnitY, pPlot->getX(), pPlot->getY());
						for(iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
						{
							CvUnit* pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
							{
								if(pLoopUnit->isEnemy(owningPlayer.getTeam()))
								{
									int iScore = iBaseMoves * iPlotDistance;
									if (pLoopUnit->isTrade()) // we want to plunder trade routes of possible
									{
										iScore /= 3;
									}
									if (pLoopUnit->isEmbarked()) // we want to take out embarked units more than ships
									{
										iScore = (iScore * 2) / 3;
									}

									aPlotList.push_back(pPlot, iScore);
								}
							}
						}
					}
				}

				// Backup plan is a coastal enemy city
				else if(pPlot->isCity() && pPlot->isCoastalLand())
				{
					pCity = pPlot->getPlotCity();
					if(pCity)
					{
						if(atWar(owningPlayer.getTeam(), pCity->getTeam()))
						{
							iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pCity->getX(), pCity->getY());
							if(iDistance < iClosestEnemyDistance)
							{
								iClosestEnemyDistance = iDistance;
								pEnemyCoastalCity = pCity;
							}
						}
					}
				}

				else if(pPlot->isCoastalLand() && pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance < iClosestCampDistance)
					{
						iClosestCampDistance = iDistance;
						pCoastalBarbarianCamp = pPlot;
					}
				}	
			}
		}
		int iBestTurns;

		pBestPlot = GetReachablePlot(pInitialUnit, aPlotList, &iBestTurns);
		// None found, patrol over near closest enemy coastal city, or if not that a water tile adjacent to a camp
		if(pBestPlot == NULL)
		{
			if(pEnemyCoastalCity != NULL)
			{
				// Find a coastal water tile adjacent to enemy city
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pEnemyCoastalCity->plot()->getX(), pEnemyCoastalCity->plot()->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
					{
						if(pInitialUnit->GeneratePath(pAdjacentPlot))
						{
							pBestPlot = pAdjacentPlot;
						}
					}
				}
			}
			else
			{
				if(pCoastalBarbarianCamp != NULL)
				{
					// Find a coastal water tile adjacent to camp
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pCoastalBarbarianCamp->getX(), pCoastalBarbarianCamp->getY(), ((DirectionTypes)iDirectionLoop));
						if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
						{
							if(pInitialUnit->GeneratePath(pAdjacentPlot))
							{
								pBestPlot = pAdjacentPlot;
							}
						}
					}
				}
			}
		}
	}
#endif
	return pBestPlot;
}
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationPureNavalCityAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationPureNavalCityAttack::CvAIOperationPureNavalCityAttack()
{
}

/// Destructor
CvAIOperationPureNavalCityAttack::~CvAIOperationPureNavalCityAttack()
{
}

/// Kick off this operation
void CvAIOperationPureNavalCityAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* pTarget, CvCity* pMuster)
{
#if defined(MOD_BALANCE_CORE)
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	CvPlot* pMusterPlot = NULL;
	// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
	CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
	if(pArmyAI)
	{
		if(pMuster == NULL || !pMuster->isCoastal())
		{
			CvCity* pNearestCoastalCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(eEnemy);
			if(pNearestCoastalCity != NULL)
			{
				pMuster = pNearestCoastalCity;
			}
		}
		if(pMuster != NULL)
		{
			pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
		}
		if(pMusterPlot != NULL && pMusterPlot->GetNumCombatUnits() > 0)
		{
			pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pArmyAI);
		}
		if(pMusterPlot != NULL)
		{
			SetDefaultArea(pMusterPlot->getArea());
			SetStartCityPlot(pMusterPlot);

			if(iID != -1)
			{
				if(pTarget)
				{
					CvPlot* pTargetPlot = pTargetPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), pArmyAI);
					if(pTargetPlot != NULL)
					{
						SetTargetPlot(pTargetPlot);			
						m_viArmyIDs.push_back(pArmyAI->GetID());
						pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
						pArmyAI->SetFormationIndex(GetFormation());

						pArmyAI->SetGoalPlot(GetTargetPlot());
						SetMusterPlot(pMusterPlot);
						pArmyAI->SetXY(pMusterPlot->getX(), pMusterPlot->getY());

						BuildListOfUnitsWeStillNeedToBuild();

						// Try to get as many units as possible from existing units that are waiting around
						if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
						{
							pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
							m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
						}
						else
						{
							m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
						}
						LogOperationStart();
					}
					else
					{
						// Lost our target, abort
						m_eCurrentState = AI_OPERATION_STATE_ABORTED;
						m_eAbortReason = AI_ABORT_NO_TARGET;
					}
				}

				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_TARGET;
				}
			}
		}
		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_MUSTER;
		}
	}
#else
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			if(pTarget)
			{
				SetTargetPlot(pTarget->plot());
				pArmyAI->SetGoalPlot(GetTargetPlot());

				// Muster just off the coast
				CvPlot *pCoastalMuster = kPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
				if (pCoastalMuster)
				{
					SetDefaultArea(pCoastalMuster->getArea());
					SetStartCityPlot(pCoastalMuster);
					SetMusterPlot(GetStartCityPlot());
					pArmyAI->SetXY(GetStartCityPlot()->getX(), GetStartCityPlot()->getY());

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}

					LogOperationStart();
				}
				else
				{
					// No muster point, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}

			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
#endif
}

/// Read serialized data
void CvAIOperationPureNavalCityAttack::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationPureNavalCityAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// How far out from the target city do we want to gather?
int CvAIOperationPureNavalCityAttack::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
}

/// Same as default version except if just gathered forces and this operation never reaches a final target (just keeps attacking until dead or the operation is ended)
bool CvAIOperationPureNavalCityAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
		{
			// First do base case processing
			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

			// Is target still under enemy control?
			CvPlot* pTarget = GetTargetPlot();
			if(pTarget->getOwner() != m_eEnemy)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
			}
		}
		break;

		// See if within 2 spaces of our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
#if defined(MOD_BALANCE_CORE)
			bool bBeenHad = false;
			CvString strMsg;
			if(pArmy && !GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
			{
				UnitHandle pUnit;
				pUnit = pArmy->GetFirstUnit();
				while(pUnit && !bBeenHad)
				{
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
						if(pAdjacentPlot != NULL)
						{
							CvUnit* pOtherUnit = pAdjacentPlot->getUnitByIndex(0);
							if(pOtherUnit != NULL && pOtherUnit->getOwner() == m_eEnemy)
							{
								// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
								if(GC.getLogging() && GC.getAILogging())
								{
									strMsg.Format("Ran into enemy unit during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
									LogOperationSpecialMessage(strMsg);
								}
								bBeenHad = true;
								break;
							}
						}
					}
					pUnit = pArmy->GetNextUnit();
				}
			}
			if(bBeenHad)
			{
				// Notify Diplo AI we're in place for attack
				GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

				// Notify tactical AI to focus on this area
				CvTemporaryZone zone;
				zone.SetX(GetTargetPlot()->getX());
				zone.SetY(GetTargetPlot()->getY());
				zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
				zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
				GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
#endif
			if (plotDistance(pArmy->Plot()->getX(), pArmy->Plot()->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) < 2)
			{
				// Notify tactical AI to focus on this area
				CvTemporaryZone zone;
				zone.SetX(GetTargetPlot()->getX());
				zone.SetY(GetTargetPlot()->getY());
				zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
				zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
				GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationPureNavalCityAttack::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		// See if our target city is still owned by our enemy
		if(GetTargetPlot()->getOwner() != m_eEnemy)
		{
			// Success!  The city has been captured/destroyed
			return true;
		}
	}

	return rtnValue;
}

/// Find a plot next to the city we want to attack
CvPlot* CvAIOperationPureNavalCityAttack::FindBestTarget()
{
	CvAssertMsg(false, "Obsolete function called CvAIOperationPureNavalCityAttack::FindBestTarget()");

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityCloseDefense - Place holder
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCityCloseDefense::CvAIOperationCityCloseDefense()
{
}

/// Destructor
CvAIOperationCityCloseDefense::~CvAIOperationCityCloseDefense()
{
}

/// Kick off this operation
#if defined(MOD_BALANCE_CORE)
void CvAIOperationCityCloseDefense::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* pTarget, CvCity* pMuster)
#else
void CvAIOperationCityCloseDefense::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
#endif
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());
#if defined(MOD_BALANCE_CORE)
			CvPlot* pTargetPlot = NULL;
			if(pMuster != NULL)
			{
				SetMusterPlot(pMuster->plot());
			}
			if(pTarget != NULL)
			{
				SetTargetPlot(pTarget->plot());
			}
			else
			{
				pTargetPlot = FindBestTarget();
				if(pTargetPlot != NULL)
				{
					SetTargetPlot(pTargetPlot);
				}
			}
			if(GetTargetPlot() != NULL && GetMusterPlot() != NULL)
			{
				pArmyAI->SetGoalPlot(GetTargetPlot());
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());

				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
				if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}

				LogOperationStart();
			}
			else
			{
				// No muster point, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_MUSTER;
			}
#else
			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				SetMusterPlot(pTargetPlot);  // Gather directly at the point we're trying to defend
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());

				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
				if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}

				LogOperationStart();
			}
#endif
		}
	}
}

/// Read serialized data
void CvAIOperationCityCloseDefense::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationCityCloseDefense::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationCityCloseDefense::FindBestTarget()
{
	CvCity* pCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;

	// Defend the city most under threat
	pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if(pCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if(pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		pCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
	}

	if(pCity != NULL)
	{
		pPlot = pCity->plot();
	}

	return pPlot;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationRapidResponse
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationRapidResponse::CvAIOperationRapidResponse()
{
}

/// Destructor
CvAIOperationRapidResponse::~CvAIOperationRapidResponse()
{
}

/// Kick off this operation
void CvAIOperationRapidResponse::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
#if defined(MOD_BALANCE_CORE)
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
#endif

	if(iID != -1)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				SetMusterPlot(pTargetPlot);  // Gather directly at the point we're trying to defend
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());

				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
#if defined(MOD_BALANCE_CORE)
				if(GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
#else
				if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
#endif
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}

				LogOperationStart();
			}
			else
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_TARGET;
			}
		}
	}
}

/// Read serialized data
void CvAIOperationRapidResponse::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationRapidResponse::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// If have gathered forces, check to see what the best blocking position is.
bool CvAIOperationRapidResponse::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

#if defined(MOD_BALANCE_CORE)
	if (pArmy==NULL)
		pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
#endif

	switch(m_eCurrentState)
	{
		// See if reached our target
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
#if defined(MOD_BALANCE_CORE_MILITARY)
		if (plotDistance(pArmy->GetX(), pArmy->GetY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= 2)
		{
			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_HIGH_PRIORITY_UNIT);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			bStateChanged = true;
		}
#else
		// For now never end, even at target
		bStateChanged = false;
		// ... but we might want to move to a greater threat
		RetargetDefensiveArmy(pArmy);
#endif
	}
	break;
	// In all other cases use base class version
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
bool CvAIOperationRapidResponse::ArmyMoved(CvArmyAI* pArmy)
{
	bool bStateChanged = false;

	switch(m_eCurrentState)
	{
	//muster point and target are equal ...
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		RetargetDefensiveArmy(pArmy);
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_ABORTED:
		return CvAIOperation::ArmyMoved(pArmy);
		break;
	};

	return bStateChanged;
}

/// Start the settler off to a new target plot
bool CvAIOperationRapidResponse::RetargetDefensiveArmy(CvArmyAI* pArmy)
{
	CvPlot* pBetterTarget;

	// Find most threatened city
	pBetterTarget = FindBestTarget();

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}

	// If this is a new target, switch to it
	else if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
}

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationRapidResponse::FindBestTarget()
{
#if defined(MOD_BALANCE_CORE)
	CvPlot* pBestPlot = NULL;
	int iHighestDanger = 0;
	CvPlot* pLoopPlot = NULL;
	const CvPlotsVector& aiPlots = GET_PLAYER(m_eOwner).GetPlots();
	for (uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
	{
		if(aiPlots[uiPlotIndex] == -1)
		{
			continue;
		}
		pLoopPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);

		//check all our owned land plots
		if(pLoopPlot != NULL && !pLoopPlot->isWater() && pLoopPlot->getOwner()==m_eOwner )
		{
			const UnitHandle pEnemy = pLoopPlot->getBestDefender(NO_PLAYER,m_eOwner,NULL,true);
			if (!pEnemy)
				continue;

			//a single unit is too volatile, check for a whole cluster
			int iEnemyPower = pEnemy->GetPower();
			CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pLoopPlot);
			for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
			{
				const CvPlot* pNeighborPlot = aPlotsToCheck[iCount];
				if (!pNeighborPlot)
					continue;
				pEnemy = pNeighborPlot->getBestDefender(NO_PLAYER,m_eOwner,NULL,true);
				if (pEnemy && pEnemy->getDomainType() == DOMAIN_LAND && pEnemy->IsCombatUnit())
					iEnemyPower += pEnemy->GetPower();
			}

			//we don't want to adjust our target too much
			int iDistance = (m_iTargetX>=0 && m_iTargetY>=0) ? plotDistance(m_iTargetX,m_iTargetY,pLoopPlot->getX(),pLoopPlot->getY()) : 0;
			//OutputDebugString( CvString::format("enemies at %d,%d: power is %d - old target at %d,%d - distance scale %d\n", 
			//	pLoopPlot->getX(), pLoopPlot->getY(), iEnemyPower, m_iTargetX, m_iTargetY, MapToPercent(iDistance,23,3) ).c_str() );
			iEnemyPower *= MapToPercent(iDistance,23,3);

			if(iEnemyPower > iHighestDanger)
			{
				iHighestDanger = iEnemyPower;
				pBestPlot = pLoopPlot;
			}
		}
	}
	if(pBestPlot == NULL)
	{
		CvCity* pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();
		if(pCity != NULL)
		{
			pBestPlot = pCity->plot();
		}
	}
	return pBestPlot;	
#else
	CvCity* pCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;
	// Defend the city most under threat
	pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if(pCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if(pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		pCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
	}

	if(pCity != NULL)
	{
		pPlot = pCity->plot();
	}
	return pPlot;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CvAINavalEscortedOperation
////////////////////////////////////////////////////////////////////////////////
CvAINavalEscortedOperation::CvAINavalEscortedOperation()
{
	// *** Move into subclass later?
	m_eCivilianType = UNITAI_SETTLE;
#if !defined(MOD_BALANCE_CORE)
	m_iInitialAreaID = -1;
#endif
}

CvAINavalEscortedOperation::~CvAINavalEscortedOperation()
{
}

/// Kick off this operation
#if defined(MOD_BALANCE_CORE)
void CvAINavalEscortedOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int iDefaultArea, CvCity* pTarget, CvCity* pMuster)
#else
void CvAINavalEscortedOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int iDefaultArea, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
#endif
{
	Reset();
	m_iTargetArea = iDefaultArea;
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
#if defined(MOD_BALANCE_CORE)
#else
	SetDefaultArea(iDefaultArea);   // Area settler starts in
#endif

	// Find the free civilian (that triggered this operation)
#if defined(MOD_BALANCE_CORE)
	bool bCivilianEscort = false;
	if(GetOperationType() == AI_OPERATION_COLONIZE)
	{
		bCivilianEscort = true;
	}
	if(bCivilianEscort)
	{
#endif
		CvUnit* pOurCivilian = FindBestCivilian();
#if defined(MOD_BALANCE_CORE)
		if(pOurCivilian != NULL && m_iID != -1)
#else
		m_iInitialAreaID = (pOurCivilian != NULL)? pOurCivilian->getArea() : -1;
		CvCity* pStartCity = GetOperationStartCity();
		if(pOurCivilian != NULL && iID != -1 && pStartCity)
#endif
		{
#if defined(MOD_BALANCE_CORE)
			// Find a destination (not worrying about safe paths)
			CvPlot* pTargetSite = FindBestTarget(pOurCivilian, false);
			if(pTargetSite != NULL)
			{
				SetTargetPlot(pTargetSite);
#else
			// Find a destination (not worrying about safe paths)
			CvPlot* pTargetSite = FindBestTarget(pOurCivilian);


			if(pTargetSite != NULL)
			{
				m_iTargetArea = iDefaultArea;
				SetTargetPlot(pTargetSite);
				// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
				CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
				if(pArmyAI)
				{
					m_viArmyIDs.push_back(pArmyAI->GetID());
					pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
					pArmyAI->SetFormationIndex(GetFormation());

					pArmyAI->SetGoalPlot(pTargetSite);
#endif
#if defined(MOD_BALANCE_CORE)
				CvPlot* pFinalPlot = NULL;
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pTargetSite->getX(), pTargetSite->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && !pAdjacentPlot->isLake())
					{					
						int iBestDistance = MAX_INT;
						CvArea* pArea = GC.getMap().getArea(pAdjacentPlot->getArea());
						if(pArea != NULL)
						{
							CvPlot* pLoopPlot;
							const CvPlotsVector& aiPlots = GET_PLAYER(m_eOwner).GetPlots();
							for (uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
							{
								if(aiPlots[uiPlotIndex] == -1)
								{
									continue;
								}
								pLoopPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
								if(!pLoopPlot->isWater())
								{
									continue;
								}
								if(pLoopPlot->getArea() != pAdjacentPlot->getArea())
								{
									continue;
								}
						
								if(!pOurCivilian->GeneratePath(pLoopPlot))
								{
									continue;
								}
								if(pLoopPlot->getOwner() == m_eOwner)
								{
									int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pOurCivilian->getX(), pOurCivilian->getY());
									if(iDistance < iBestDistance)
									{
										iBestDistance = iDistance;
										pFinalPlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
				if(pFinalPlot != NULL)
				{
					// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
					CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
					if(pArmyAI)
					{
						m_viArmyIDs.push_back(pArmyAI->GetID());
						pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
						pArmyAI->SetFormationIndex(GetFormation());

						// Figure out the initial rally point - for this operation it is wherever our civilian is standing
						pArmyAI->SetGoalPlot(pTargetSite);
						SetMusterPlot(pFinalPlot);
						pArmyAI->SetXY(pFinalPlot->getX(), pFinalPlot->getY());
						SetDefaultArea(pFinalPlot->getArea());

						// Add the settler to our army
						pArmyAI->AddUnit(pOurCivilian->GetID(), 0);

						// Add the escort as a unit we need to build
						m_viListOfUnitsWeStillNeedToBuild.clear();
						OperationSlot thisOperationSlot;
						thisOperationSlot.m_iOperationID = m_iID;
						thisOperationSlot.m_iArmyID = pArmyAI->GetID();
						thisOperationSlot.m_iSlotID = 1;
						m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);

						// try to get the escort from existing units that are waiting around
						BuildListOfUnitsWeStillNeedToBuild();
						
						GrabUnitsFromTheReserves(pFinalPlot, pTargetSite);
						if(pArmyAI->GetNumSlotsFilled() > 1)
						{
							pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
							m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
						}
						else
						{
							m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
						}
						LogOperationStart();
					}
				}
				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
				}
			}
			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_TARGET;
			}
#else
					CvPlot* pMusterPt = pStartCity->plot();
					SetMusterPlot(pMusterPt);
					pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());

					// Add the settler to our army
					pArmyAI->AddUnit(pOurCivilian->GetID(), 0);
#endif
					// Try to get as many units as possible from existing units that are waiting around
#if defined(MOD_BALANCE_CORE)
#else
					if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}
					LogOperationStart();

				}
			}

			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
#endif
		}
#if defined(MOD_BALANCE_CORE)
	}
	else
	{
		CvPlot* pStartPlot = pMuster->plot();

		if(pStartPlot != NULL)
		{
			// Find a destination
			CvPlot* pTargetSite = pTarget->plot();

			if(pTargetSite != NULL)
			{
				SetTargetPlot(pTargetSite);

				// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
				CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
				if(pArmyAI)
				{
					m_viArmyIDs.push_back(pArmyAI->GetID());
					pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
					pArmyAI->SetFormationIndex(GetFormation());

					pArmyAI->SetGoalPlot(pTargetSite);
					SetMusterPlot(pStartPlot);
					pArmyAI->SetXY(pStartPlot->getX(), pStartPlot->getY());

					// try to get the escort from existing units that are waiting around
					BuildListOfUnitsWeStillNeedToBuild();

					// Try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
					{
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
						m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
					}
					else
					{
						m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
					}
					LogOperationStart();
				}
			}

			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
#endif
}
#if !defined(MOD_BALANCE_CORE)
/// Find the port our operation will leave from
CvCity* CvAINavalEscortedOperation::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	CvPlayerAI& kOwner = GET_PLAYER(m_eOwner);

	int iLoop = 0;
	CvCity* pCity = NULL;

	// Find first coastal city in same area as settler
	for(pCity = kOwner.firstCity(&iLoop); pCity != NULL; pCity = kOwner.nextCity(&iLoop))
	{
		if(pCity->isCoastal())
		{
			if(pCity->getArea() == m_iInitialAreaID)
			{
				return pCity;
			}
		}
	}

	return NULL;
}
#endif
#if !defined(MOD_BALANCE_CORE)
/// Always abort if settler is removed
void CvAINavalEscortedOperation::UnitWasRemoved(int /*iArmyID*/, int iSlotID)
{
	// Assumes civilian is in the first slot of the formation
	if(iSlotID == 0)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_LOST_CIVILIAN;
	}
}
#endif
/// Find the civilian we want to use
CvUnit* CvAINavalEscortedOperation::FindBestCivilian()
{
	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;

	CvPlayerAI& kOwner = GET_PLAYER(m_eOwner);

	for(pLoopUnit = kOwner.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kOwner.nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
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

/// Read serialized data
void CvAINavalEscortedOperation::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_eCivilianType;
}
/// Write serialized data
void CvAINavalEscortedOperation::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_eCivilianType;
}
/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAINavalEscortedOperation::ArmyInPosition(CvArmyAI* pArmy)
{
	int iUnitID;
	bool bStateChanged = false;
#if defined(MOD_BALANCE_CORE)
	CvUnit* pSettler = NULL;
	CvUnit* pEscort = NULL;
#else
	CvUnit* pSettler = 0, *pEscort = 0;
#endif
	CvString strMsg;
#if defined(MOD_BALANCE_CORE)
	bool bCivilianEscort = false;
	if(GetOperationType() == AI_OPERATION_COLONIZE || GetOperationType() == AI_OPERATION_QUICK_COLONIZE || GetOperationType() == AI_OPERATION_FOUND_CITY)
	{
		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			if (pSettler && (pSettler->isFound()))
			{
				bCivilianEscort = true;
			}
			else
			{
				bCivilianEscort = false;
			}
			iUnitID = pArmy->GetNextUnitID();
			if(iUnitID != -1)
			{
				pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			if(pEscort == NULL && bCivilianEscort)
			{
				if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
				{
					iUnitID = pArmy->GetFirstUnitID();
					if(iUnitID != -1)
					{
						pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						if (pSettler && (pSettler->isFound()))
						{
							bCivilianEscort = true;
						}
						else
						{
							m_eCurrentState = AI_OPERATION_STATE_ABORTED;
							m_eAbortReason = AI_ABORT_LOST_CIVILIAN;
						}
						iUnitID = pArmy->GetNextUnitID();
						if(iUnitID != -1)
						{
							pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						}
					}
				}
			}
		}
	}
#endif
	if(bCivilianEscort)
	{
	switch(m_eCurrentState)
	{
#if defined(MOD_BALANCE_CORE)
	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		if(pSettler != NULL && pEscort != NULL && pSettler->plot() == pEscort->plot())
		{
			// let's see if the target still makes sense (this is modified from RetargetCivilian)
			CvPlot* pBetterTarget = FindBestTargetIncludingCurrent(pSettler, false);

			// No targets at all!  Abort
			if(pBetterTarget == NULL && GetTargetPlot() == NULL)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_TARGET;
				return false;
			}
			// If we have a target
			else
			{
				SetTargetPlot(pBetterTarget);
				pArmy->SetGoalPlot(pBetterTarget);
				m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
				return true;
			}
		}
		else if(pSettler != NULL && pEscort != NULL && plotDistance(pSettler->getX(), pSettler->getY(), pEscort->getX(), pEscort->getY()) <= 2)
		{
			// let's see if the target still makes sense (this is modified from RetargetCivilian)
			CvPlot* pBetterTarget = FindBestTargetIncludingCurrent(pSettler, false);

			// No targets at all!  Abort
			if(pBetterTarget == NULL && GetTargetPlot() == NULL)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_TARGET;
				return false;
			}
			// If we have a target
			else
			{
				SetTargetPlot(pBetterTarget);
				pArmy->SetGoalPlot(pBetterTarget);
				m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
				return true;
			}
		}
	}
	break;
#endif
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:
#if defined(MOD_BALANCE_CORE)
	{
#endif

		// Call base class version and see if it thinks we're done
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);
		// Now get the settler
#if defined(MOD_BALANCE_CORE)
#else
		iUnitID = pArmy->GetFirstUnitID();
		if(iUnitID != -1)
		{
			pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}
#endif
		if(pSettler != NULL)
		{
			CvPlot* pTargetPlot = GetTargetPlot();
			const PlayerTypes pTargetPlotOwner = pTargetPlot->getOwner();

			CvPlot* pSettlerPlot = pSettler->plot();

			if((pTargetPlotOwner != NO_PLAYER && pTargetPlotOwner != m_eOwner) || pTargetPlot->IsAdjacentOwnedByOtherTeam(pSettler->getTeam()))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Not at target but can no longer settle here. Target was (X=%d Y=%d)", pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
#if defined(MOD_BALANCE_CORE)
				if(pEscort)
				{
					pEscort->finishMoves();
				}
#else
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
#endif
			}
			// If the settler made it, we don't care about the entire army
			else if(pSettlerPlot == pTargetPlot && pSettler->canMove() && pSettler->canFound(pSettlerPlot))
			{
				pSettler->PushMission(CvTypes::getMISSION_FOUND());
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("City founded, At X=%d, At Y=%d", pSettlerPlot->getX(), pSettlerPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
#if defined(MOD_BALANCE_CORE)
				// Notify tactical AI to focus on this area
				CvTemporaryZone zone;
				zone.SetX(pSettlerPlot->getX());
				zone.SetY(pSettlerPlot->getY());
				zone.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
				zone.SetLastTurn(GC.getGame().getGameTurn() + (GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() * 2));
				GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
#endif
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}

			// If we're at our target but can no longer found a city, might be someone else beat us to this area
			// So move back out, picking a new target
			else if(pSettlerPlot == GetTargetPlot() && !pSettler->canFound(pSettlerPlot))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("At target but can no longer settle here. Target was (X=%d Y=%d)", pTargetPlot->getX(), pTargetPlot->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
#if defined(MOD_BALANCE_CORE)
				if(pEscort)
				{
					pEscort->finishMoves();
				}
#else
				iUnitID = pArmy->GetNextUnitID();
				if(iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
#endif
			}
		}
#if defined(MOD_BALANCE_CORE)
	}
#endif
		break;
	// In all other cases use base class verson
#if !defined(MOD_BALANCE_CORE)
	case AI_OPERATION_STATE_GATHERING_FORCES:
#endif
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
#if defined(MOD_BALANCE_CORE)
	}
	else
	{
		switch(m_eCurrentState)
		{
			// See if reached our target, if so give control of these units to the tactical AI
			case AI_OPERATION_STATE_MOVING_TO_TARGET:
			{
				CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);

				// Are we within tactical range of our target? (larger than usual range for a naval attack)
#if defined(MOD_BALANCE_CORE)
				if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
#else
				if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
#endif
				{
					// Notify Diplo AI we're in place for attack
					GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

					// Notify tactical AI to focus on this area
					CvTemporaryZone zone;
					zone.SetX(GetTargetPlot()->getX());
					zone.SetY(GetTargetPlot()->getY());
					zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
					zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
					zone.SetNavalInvasion(true);
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
					return true;
				}
			}
			break;

			// In all other cases use base class version
			case AI_OPERATION_STATE_GATHERING_FORCES:
			case AI_OPERATION_STATE_ABORTED:
			case AI_OPERATION_STATE_RECRUITING_UNITS:
			case AI_OPERATION_STATE_AT_TARGET:
				return CvAIOperation::ArmyInPosition(pArmy);
				break;
		};
	}
#endif
	return bStateChanged;
}

/// Find the plot where we want to settle
#if defined(MOD_BALANCE_CORE_SETTLER)
CvPlot* CvAINavalEscortedOperation::FindBestTargetIncludingCurrent(CvUnit* pUnit, bool bOnlySafePaths)
{
	//todo: better options
	//a) return a list of possible targets and find the ones that are currently reachable
	//b) if the best target is unreachable, move in the general direction and hope the block will clear up

	//ignore the current operation target when searching. default would be to suppress currently targeted plots
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, m_iTargetArea, this);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, -1, this);
	}
	return pResult;
}
CvPlot* CvAINavalEscortedOperation::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, m_iTargetArea);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, -1);
	}
	return pResult;
}
#else
CvPlot* CvAINavalEscortedOperation::FindBestTarget(CvUnit* pUnit)
{
	return GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, true, -1);
}
#endif

/// Start the civilian off to a new target plot
bool CvAINavalEscortedOperation::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
{
#if defined(MOD_BALANCE_CORE)
	// Find best city site (assuming we are escorted)
	CvPlot* pBetterTarget = FindBestTarget(pCivilian, false);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}
	// If this is a new target, switch to it
	else if(pBetterTarget != GetTargetPlot())
	{
		std::vector<int> aiUnitsToRemove;
		for (UnitHandle pUnit = pArmy->GetFirstUnit(); pUnit.pointer(); pUnit = pArmy->GetNextUnit())
		{
			if (TurnsToReachTarget(pUnit, pBetterTarget) == MAX_INT)
			{
				aiUnitsToRemove.push_back(pUnit->GetID());
			}
		}
		for (std::vector<int>::iterator it = aiUnitsToRemove.begin(); it != aiUnitsToRemove.end(); ++it)
		{
			pArmy->RemoveUnit((*it));
		}
		if ((m_eCurrentState) == AI_OPERATION_STATE_ABORTED)
		{
			return false;
		}
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
		CvPlot* pFinalPlot = NULL;
		for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
		{
			CvPlot* pAdjacentPlot = plotDirection(pBetterTarget->getX(), pBetterTarget->getY(), ((DirectionTypes)iDirectionLoop));
			if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && !pAdjacentPlot->isLake())
			{					
				int iBestDistance = MAX_INT;
				CvArea* pArea = GC.getMap().getArea(pAdjacentPlot->getArea());
				if(pArea != NULL)
				{
					CvPlot* pLoopPlot;
					const CvPlotsVector& aiPlots = GET_PLAYER(m_eOwner).GetPlots();
					for (uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
					{
						if(aiPlots[uiPlotIndex] == -1)
						{
							continue;
						}
						pLoopPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
						if(!pLoopPlot->isWater())
						{
							continue;
						}
						if(pLoopPlot->getArea() != pAdjacentPlot->getArea())
						{
							continue;
						}
						
						if(!pCivilian->GeneratePath(pLoopPlot))
						{
							continue;
						}
						if(pLoopPlot->getOwner() == m_eOwner)
						{
							int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pCivilian->getX(), pCivilian->getY());
							if(iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								pFinalPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
		if(pFinalPlot != NULL)
		{
			SetMusterPlot(pFinalPlot);
			pArmy->SetXY(pFinalPlot->getX(), pFinalPlot->getY());
			SetDefaultArea(pFinalPlot->getArea());
		}
	}
	else if(GetTargetPlot() == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_LOST_TARGET;
		return false;
	}
	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
#else
	CvPlot* pBetterTarget;

	// Find best city site (taking into account whether or not we are escorted)
	pBetterTarget = FindBestTarget(pCivilian);

	// No targets at all!  Abort
	if(pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}
	// If this is a new target, switch to it
	else if(pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalAttack
////////////////////////////////////////////////////////////////////////////////
CvAIOperationNavalAttack::CvAIOperationNavalAttack()
{
	m_eCivilianType = NO_UNITAI;
}

CvAIOperationNavalAttack::~CvAIOperationNavalAttack()
{
}

/// Kick off this operation
void CvAIOperationNavalAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity* pTarget, CvCity* pMuster)
{
#if defined(MOD_BALANCE_CORE)
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	CvPlot* pMusterPlot = NULL;
	// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
	CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
	if(pArmyAI)
	{
		if(pMuster == NULL || !pMuster->isCoastal())
		{
			CvCity* pNearestCoastalCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(eEnemy);
			if(pNearestCoastalCity != NULL)
			{
				pMuster = pNearestCoastalCity;
			}
		}
		if(pMuster != NULL && GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI) != NULL)
		{
			pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
		}
		iDefaultArea = pMuster->getArea();
		SetDefaultArea(iDefaultArea);

		if(pMusterPlot != NULL)
		{
			SetStartCityPlot(pMusterPlot);

			if(iID != -1)
			{
				if(pTarget)
				{
					CvPlot* pTargetPlot = NULL;
					if(GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), pArmyAI) != NULL)
					{
						pTargetPlot = pTarget->plot();
					}
					if(pTargetPlot != NULL)
					{
						SetTargetPlot(pTargetPlot);			
						m_viArmyIDs.push_back(pArmyAI->GetID());
						pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
						pArmyAI->SetFormationIndex(GetFormation());

						pArmyAI->SetGoalPlot(GetTargetPlot());
						SetMusterPlot(GetStartCityPlot());
						pArmyAI->SetXY(GetStartCityPlot()->getX(), GetStartCityPlot()->getY());

						BuildListOfUnitsWeStillNeedToBuild();

						// Try to get as many units as possible from existing units that are waiting around
						if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
						{
							pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
							m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
						}
						else
						{
							m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
						}
						LogOperationStart();
					}
					else
					{
						// Lost our target, abort
						m_eCurrentState = AI_OPERATION_STATE_ABORTED;
						m_eAbortReason = AI_ABORT_NO_TARGET;
					}
				}

				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_TARGET;
				}
			}
		}
		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_MUSTER;
		}
	}
#else
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	SetDefaultArea(iDefaultArea);

	SetStartCityPlot(pMuster->plot());

	if(iID != -1)
	{
		if(pTarget)
		{
			SetTargetPlot(pTarget->plot());

			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());

				pArmyAI->SetGoalPlot(GetTargetPlot());
				SetMusterPlot(GetStartCityPlot());
				pArmyAI->SetXY(GetStartCityPlot()->getX(), GetStartCityPlot()->getY());

				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
				if(GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}
				LogOperationStart();
			}
		}

		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
	}
#endif
}

/// Always abort if settler is removed
void CvAIOperationNavalAttack::UnitWasRemoved(int iArmyID, int iSlotID)
{
	// Call root class version
	CvAIOperation::UnitWasRemoved(iArmyID, iSlotID);
}

/// Read serialized data
void CvAIOperationNavalAttack::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalEscortedOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationNavalAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalEscortedOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAIOperationNavalAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	switch(m_eCurrentState)
	{
		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);
#if defined(MOD_BALANCE_CORE)
		bool bBeenHad = false;
		CvString strMsg;
		if(pArmy && !GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
		{
			UnitHandle pUnit;
			pUnit = pArmy->GetFirstUnit();
			while(pUnit && !bBeenHad)
			{
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL)
					{
						CvUnit* pOtherUnit = pAdjacentPlot->getUnitByIndex(0);
						if(pOtherUnit != NULL && pOtherUnit->getOwner() == m_eEnemy)
						{
							// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
							if(GC.getLogging() && GC.getAILogging())
							{
								strMsg.Format("Ran into enemy unit during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
								LogOperationSpecialMessage(strMsg);
							}
							bBeenHad = true;
							break;						
						}
					}
				}
				pUnit = pArmy->GetNextUnit();
			}
		}
		if(bBeenHad)
		{
			// Notify Diplo AI we're in place for attack
			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
#endif
		// Are we within tactical range of our target? (larger than usual range for a naval attack)
#if defined(MOD_BALANCE_CORE)
		if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 2)
#else
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 2)
#endif
		{
			// Notify Diplo AI we're in place for attack
			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			zone.SetNavalInvasion(true);
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			return true;
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return false;
}

/// Find the port our operation will leave from
CvCity* CvAIOperationNavalAttack::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(m_eEnemy);
}

/// Find the city we want to attack
CvPlot* CvAIOperationNavalAttack::FindBestTarget()
{
	CvAssertMsg(false, "Obsolete function called CvAIOperationNavalAttack::FindBestTarget()");

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalSneakAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNavalSneakAttack::CvAIOperationNavalSneakAttack()
{
}
#if defined(MOD_BALANCE_CORE)
CvAIOperationNavalSneakAttack::~CvAIOperationNavalSneakAttack()
{
}

/// Kick off this operation
void CvAIOperationNavalSneakAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	CvPlot* pMusterPlot = NULL;
	// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
	CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
	if(pArmyAI)
	{
		if(pMuster == NULL || !pMuster->isCoastal())
		{
			CvCity* pNearestCoastalCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(eEnemy);
			if(pNearestCoastalCity != NULL)
			{
				pMuster = pNearestCoastalCity;
			}
		}
		if(pMuster != NULL)
		{
			pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
			if(pMusterPlot->GetNumCombatUnits() > 0)
			{
				pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, pArmyAI);
			}
		}
		if(pMusterPlot != NULL)
		{
			SetStartCityPlot(pMusterPlot);
			iDefaultArea = pMusterPlot->getArea();
			SetDefaultArea(iDefaultArea);

			if(iID != -1)
			{
				if(pTarget)
				{
					CvPlot* pTargetPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), pArmyAI);
					if(pTargetPlot != NULL)
					{
						SetTargetPlot(pTargetPlot);			
						m_viArmyIDs.push_back(pArmyAI->GetID());
						pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
						pArmyAI->SetFormationIndex(GetFormation());

						pArmyAI->SetGoalPlot(GetTargetPlot());
						SetMusterPlot(pMusterPlot);
						pArmyAI->SetXY(pMusterPlot->getX(), pMusterPlot->getY());

						BuildListOfUnitsWeStillNeedToBuild();

						// Try to get as many units as possible from existing units that are waiting around
						if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
						{
							pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
							m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
						}
						else
						{
							m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
						}
						LogOperationStart();
					}
					else
					{
						// Lost our target, abort
						m_eCurrentState = AI_OPERATION_STATE_ABORTED;
						m_eAbortReason = AI_ABORT_NO_TARGET;
					}
				}

				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_TARGET;
				}
			}
		}
		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_MUSTER;
		}
	}
}

/// Always abort if settler is removed
void CvAIOperationNavalSneakAttack::UnitWasRemoved(int iArmyID, int iSlotID)
{
	// Call root class version
	CvAIOperation::UnitWasRemoved(iArmyID, iSlotID);
}

/// Read serialized data
void CvAIOperationNavalSneakAttack::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalEscortedOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationNavalSneakAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalEscortedOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAIOperationNavalSneakAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	switch(m_eCurrentState)
	{
		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);
#if defined(MOD_BALANCE_CORE)
		bool bBeenHad = false;
		CvString strMsg;
		if(pArmy && !GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
		{
			UnitHandle pUnit;
			pUnit = pArmy->GetFirstUnit();
			while(pUnit)
			{
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL)
					{
						CvUnit* pOtherUnit = pAdjacentPlot->getUnitByIndex(0);
						if(pOtherUnit != NULL && pOtherUnit->getOwner() == m_eEnemy)
						{
							bBeenHad = true;
							// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
							if(GC.getLogging() && GC.getAILogging())
							{
								strMsg.Format("Ran into enemy unit during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
								LogOperationSpecialMessage(strMsg);
							}
							break;
						}
					}
				}
				pUnit = pArmy->GetNextUnit();
			}
		}
		if(bBeenHad)
		{
			// Notify Diplo AI we're in place for attack
			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
		}
#endif
		// Are we within tactical range of our target? (larger than usual range for a naval attack)
#if defined(MOD_BALANCE_CORE)
		if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
#else
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 2)
#endif
		{
			// Notify Diplo AI we're in place for attack
			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			zone.SetNavalInvasion(true);
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			return true;
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return false;
}

/// Find the port our operation will leave from
CvCity* CvAIOperationNavalSneakAttack::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(m_eEnemy);
}
#endif
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalCityStateAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNavalCityStateAttack::CvAIOperationNavalCityStateAttack()
{
}
#if defined(MOD_BALANCE_CORE)
CvAIOperationNavalCityStateAttack::~CvAIOperationNavalCityStateAttack()
{
}

/// Kick off this operation
void CvAIOperationNavalCityStateAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	CvPlot* pMusterPlot = NULL;
	// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
	CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
	if(pArmyAI)
	{
		if(pMuster == NULL || !pMuster->isCoastal())
		{
			CvCity* pNearestCoastalCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(eEnemy);
			if(pNearestCoastalCity != NULL)
			{
				pMuster = pNearestCoastalCity;
			}
		}
		if(pMuster != NULL && GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI) != NULL)
		{
			pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), pArmyAI);
		}

		if(pMusterPlot != NULL)
		{
			SetStartCityPlot(pMusterPlot);
			iDefaultArea = pMusterPlot->getArea();
			SetDefaultArea(iDefaultArea);

			if(iID != -1)
			{
				if(pTarget)
				{
					CvPlot* pTargetPlot = NULL;
					if(GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), pArmyAI) != NULL)
					{
						pTargetPlot = pTarget->plot();
					}
					if(pTargetPlot != NULL)
					{
						SetTargetPlot(pTargetPlot);			
						m_viArmyIDs.push_back(pArmyAI->GetID());
						pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
						pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
						pArmyAI->SetFormationIndex(GetFormation());

						pArmyAI->SetGoalPlot(GetTargetPlot());
						SetMusterPlot(GetStartCityPlot());
						pArmyAI->SetXY(GetStartCityPlot()->getX(), GetStartCityPlot()->getY());

						BuildListOfUnitsWeStillNeedToBuild();

						// Try to get as many units as possible from existing units that are waiting around
						if(GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
						{
							pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
							m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
						}
						else
						{
							m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
						}
						LogOperationStart();
					}
					else
					{
						// Lost our target, abort
						m_eCurrentState = AI_OPERATION_STATE_ABORTED;
						m_eAbortReason = AI_ABORT_NO_TARGET;
					}
				}

				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_TARGET;
				}
			}
		}
		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_MUSTER;
		}
	}
}

/// Always abort if settler is removed
void CvAIOperationNavalCityStateAttack::UnitWasRemoved(int iArmyID, int iSlotID)
{
	// Call root class version
	CvAIOperation::UnitWasRemoved(iArmyID, iSlotID);
}

/// Read serialized data
void CvAIOperationNavalCityStateAttack::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalEscortedOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Write serialized data
void CvAIOperationNavalCityStateAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalEscortedOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAIOperationNavalCityStateAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	switch(m_eCurrentState)
	{
		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	{
		CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);

		// Are we within tactical range of our target? (larger than usual range for a naval attack)
#if defined(MOD_BALANCE_CORE)
		if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
#else
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
#endif
		{
			// Notify Diplo AI we're in place for attack
			GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);

			// Notify tactical AI to focus on this area
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			zone.SetNavalInvasion(true);
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);

			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			return true;
		}
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return false;
}

/// Find the port our operation will leave from
CvCity* CvAIOperationNavalCityStateAttack::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(m_eEnemy);
}
#endif

CvAIOperationNukeAttack::CvAIOperationNukeAttack()
{
	m_iBestUnitID = -1;
}

CvAIOperationNukeAttack::~CvAIOperationNukeAttack()
{
}

void CvAIOperationNukeAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int , CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_STATIC; // this operation will only work when the units are already in place
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if(iID != -1)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if(pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(), m_eOwner, m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			// Figure out the target spot
			CvPlot* pTargetPlot = FindBestTarget();
			if(pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();
				// try to get as many units as possible from existing units that are waiting around
				if(GrabUnitsFromTheReserves(GC.getMap().plotCheckInvalid(m_iMusterX, m_iMusterY), pTargetPlot))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_AT_DESTINATION);
					m_eCurrentState = AI_OPERATION_STATE_AT_TARGET;
					LogOperationStart();
					ArmyInPosition(pArmyAI);
				}
				else
				{
					// No nukes, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_NUKES;
				}
			}

			else
			{
				// No target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
}


/// Read serialized data
void CvAIOperationNukeAttack::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iBestUnitID;
}

/// Write serialized data
void CvAIOperationNukeAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iBestUnitID;
}

CvCity* CvAIOperationNukeAttack::GetOperationStartCity() const
{
	if(GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GC.getMap().plotCheckInvalid(m_iMusterX, m_iMusterY)->getPlotCity();
}

bool CvAIOperationNukeAttack::ArmyInPosition(CvArmyAI* pArmy)
{
	CvPlot* pTargetPlot = GetTargetPlot();
	if(pTargetPlot)
	{
		// Now get the nuke
		int iUnitID = pArmy->GetFirstUnitID();
		CvUnit* pNuke = NULL;
		if(iUnitID != -1)
		{
			pNuke = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if(pNuke != NULL)
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
	}
	return true;
}

CvPlot* CvAIOperationNukeAttack::FindBestTarget()
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
									if(pLoopPlot->IsFeatureFallout() && pCityPlot->getOwner() == pCityPlot->getOwner())
									{
										iThisCityValue -= 1000;
									}
									//Is this city threatened? Let's target this.
									if(enemyPlayer.GetMilitaryAI()->GetMostThreatenedCity() == pLoopCity)
									{
										iThisCityValue += 250;
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
												iThisCityValue += 2;
											}
											else if (ePlotOwner != NO_PLAYER) // this will trigger a war
											{
												iThisCityValue -= 1000;
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

	if(pBestCity && pBestUnit)
	{
		m_iMusterX = pBestUnit->getX();
		m_iMusterY = pBestUnit->getY();
		m_iBestUnitID = pBestUnit->GetID();
		return pBestCity->plot();
	}

	return NULL;
}

/// Find a unit from our reserves that could serve in this operation
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot* /*pMusterPlot*/, CvPlot* /*pTargetPlot*/, bool* bRequired)
{
	// okay, this can be simplified to
	*bRequired = true;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	pThisArmy->AddUnit(m_iBestUnitID, thisOperationSlot.m_iSlotID);
	return true;
}


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