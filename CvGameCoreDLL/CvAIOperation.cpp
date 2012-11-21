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
void CvAIOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity * /*pTarget*/, CvCity *pMuster)
{
	Uninit();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	m_iDefaultArea = iDefaultArea;
	m_bShouldReplaceLossesWithReinforcements = false;

	SetStartCityPlot(pMuster->plot());

	// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
	BuildListOfUnitsWeStillNeedToBuild();
	GrabUnitsFromTheReserves(NULL, NULL);
}

/// Delete allocated objects
void CvAIOperation::Uninit()
{
	if (m_eOwner != NO_PLAYER) // hopefully if this has been init'ed this should not happen
	{
		CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);

		// remove the armies (which should, in turn, free up their units for other tasks)
		for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* thisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
			if (thisArmy)
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
	m_iDefaultArea = FFreeList::INVALID_INDEX;
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = NO_ABORT_REASON;
	m_iTargetX = 0;
	m_iTargetY = 0;
	m_iMusterX = 0;
	m_iMusterY = 0;
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
	if (!m_viArmyIDs.empty())
	{
		return m_viArmyIDs[0];
	}
	else
	{
		return -1;
	}
}

/// Retrieve plot targeted by this operation
CvPlot *CvAIOperation::GetTargetPlot() const
{
	CvPlot *rtnValue = NULL;
	rtnValue =  GC.getMap().plot(m_iTargetX, m_iTargetY);
	return rtnValue;
}

/// Set plot targeted by this operation
void CvAIOperation::SetTargetPlot(CvPlot *pTarget)
{
	m_iTargetX = pTarget->getX();
	m_iTargetY = pTarget->getY();
}

/// Retrieve plot where this operation assembles
CvPlot *CvAIOperation::GetMusterPlot() const
{
	CvPlot *rtnValue = NULL;
	rtnValue =  GC.getMap().plot(m_iMusterX, m_iMusterY);
	return rtnValue;
}

/// Set plot where this operation assembles
void CvAIOperation::SetMusterPlot(CvPlot *pMuster)
{
	m_iMusterX = pMuster->getX();
	m_iMusterY = pMuster->getY();
}

/// Retrieve city where this operation starts
CvPlot *CvAIOperation::GetStartCityPlot() const
{
	CvPlot *rtnValue = NULL;
	rtnValue =  GC.getMap().plot(m_iStartCityX, m_iStartCityY);
	return rtnValue;
}

/// Set city where this operation starts
void CvAIOperation::SetStartCityPlot(CvPlot *pStartCity)
{
	m_iStartCityX = pStartCity->getX();
	m_iStartCityY = pStartCity->getY();
}

int CvAIOperation::GetGatherTolerance(CvArmyAI *pArmy, CvPlot *pPlot) const
{
	CvTacticalAnalysisCell *pCell;
	CvTacticalAnalysisMap *pMap = GC.getGame().GetTacticalAnalysisMap();
	int iRtnValue = 1;
	int iValidPlotsNearby = 0;

	// Find out how many units are trying to gather
	int iNumUnits = pArmy->GetNumSlotsFilled();

	// If not more than 1, zero tolerance is fine (we should get the unit to the gather point)
	if (iNumUnits < 1)
	{
		iRtnValue = 0;
	}
	else
	{
		int iRange = OperationalAIHelpers::GetGatherRangeForXUnits(iNumUnits);
		for (int iX = -iRange; iX <= iRange; iX++)
		{
			for (int iY = -iRange; iY <= iRange; iY++)
			{
				int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
				pCell = pMap->GetCell(iPlotIndex);

				if ((IsMixedLandNavalOperation() && pCell->CanUseForOperationGathering()) || pCell->CanUseForOperationGatheringCheckWater(IsAllNavalOperation()))
				{
					if ((IsMixedLandNavalOperation() || IsAllNavalOperation()) && !pArmy->IsAllOceanGoing() && pCell->IsOcean())
					{
						continue;
					}
					iValidPlotsNearby++;
				}
			}
		}

		// Find more valid plots than units?
		if (iValidPlotsNearby > iNumUnits)
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
	switch (eAIOperationType)
	{
	case AI_OPERATION_BASIC_CITY_ATTACK:
		return FNEW(CvAIOperationBasicCityAttack(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_DESTROY_BARBARIAN_CAMP:
		return FNEW(CvAIOperationDestroyBarbarianCamp(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_FOUND_CITY:
		return FNEW(CvAIOperationFoundCity(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_MERCHANT_DELEGATION:
		return FNEW(CvAIOperationMerchantDelegation(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_BOMBARDMENT:
		return FNEW(CvAIOperationNavalBombardment(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_NAVAL_SUPERIORITY:
		return FNEW(CvAIOperationNavalSuperiority(), c_eCiv5GameplayDLL, 0);
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
			if (GC.getGame().getGameTurn() < 50 && GET_PLAYER(ePlayer).GetDiplomacyAI()->GetBoldness() >= 5)
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
	case AI_OPERATION_NOT_SO_QUICK_COLONIZE:
		return FNEW(CvAIOperationNotSoQuickColonize(), c_eCiv5GameplayDLL, 0);
	}

	return 0;
}

/// Compile a list of all the units we need
void CvAIOperation::BuildListOfUnitsWeStillNeedToBuild()
{
	m_viListOfUnitsCitiesHaveCommittedToBuild.clear();
	m_viListOfUnitsWeStillNeedToBuild.clear();
	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);
	for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* pThisArmy = thisPlayer.getArmyAI(m_viArmyIDs[uiI]);
		// if it is still waiting on initial units
		if (pThisArmy)
		{
			if (pThisArmy->GetArmyAIState() == ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE)
			{
				int iThisFormationIndex = pThisArmy->GetFormationIndex();
				if (iThisFormationIndex != NO_MUFORMATION)
				{
					CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
					if(thisFormation)
					{
						for (int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
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
	if (iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
	}
	return thisSlot;
}

/// Called by a city when it decides to build a unit
OperationSlot CvAIOperation::CommitToBuildNextUnit(int iAreaID, int iTurns, CvCity *pCity)
{
	OperationSlot thisSlot;
	if (iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
		m_viListOfUnitsWeStillNeedToBuild.pop_back();
		m_viListOfUnitsCitiesHaveCommittedToBuild.push_back(thisSlot);

		CvArmyAI *pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		if (pArmy)
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
	if (iter != m_viListOfUnitsCitiesHaveCommittedToBuild.end())
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
	if (iter != m_viListOfUnitsCitiesHaveCommittedToBuild.end())
	{
		// remove it from the list of committed units
		m_viListOfUnitsCitiesHaveCommittedToBuild.erase(iter);

		// See if our army is now complete
		if (m_viListOfUnitsWeStillNeedToBuild.empty() && m_eCurrentState  == AI_OPERATION_STATE_RECRUITING_UNITS)
		{
			m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;

			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				if (pThisArmy)
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
bool CvAIOperation::GrabUnitsFromTheReserves(CvPlot *pMusterPlot, CvPlot *pTargetPlot)
{
	bool rtnValue = true;
	bool success;
	std::vector<OperationSlot>::iterator it;
	bool bRequired;

	// Copy over the list
	std::vector<OperationSlot> secondList;
	for (it = m_viListOfUnitsWeStillNeedToBuild.begin(); it != m_viListOfUnitsWeStillNeedToBuild.end(); ++it)
	{
		secondList.push_back(*it);
	}

	// Clear main list
	m_viListOfUnitsWeStillNeedToBuild.clear();

	for (it = secondList.begin(); it != secondList.end(); ++it)
	{
		success = FindBestFitReserveUnit(*it, pMusterPlot, pTargetPlot, &bRequired);

		// If any fail, check to see if they were required
		if (!success)
		{
			if (bRequired)
			{
				// Return false to say that operation is not ready to roll yet
				rtnValue = false;

				// And add them back to the list of units needed
				m_viListOfUnitsWeStillNeedToBuild.push_back(*it);
			}
			else
			{
				CvArmyAI *pArmy;
				pArmy = GET_PLAYER(m_eOwner).getArmyAI(it->m_iArmyID);
				if (pArmy)
				{
					pArmy->SetEstimatedTurn(it->m_iSlotID, ARMYSLOT_NOT_INCLUDING_IN_OPERATION);
				}
			}
		}
	}

	return rtnValue;
}

/// See if armies are ready to hand off units to the tactical AI (and do so if ready)
bool CvAIOperation::CheckOnTarget()
{
	int iUnitID;
	CvUnit *pCivilian;
	CvPlot *pCivilianPlot = NULL;
	CvPlot *pEscortPlot;

	if (GetFirstArmyID() == -1)
	{
		return false;
	}

	switch (m_eMoveType)
	{
		{
	case AI_OPERATION_MOVETYPE_SINGLE_HEX:
		CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
		if (pThisArmy->GetNumSlotsFilled() >= 1)
		{
			iUnitID = pThisArmy->GetFirstUnitID();
			if (iUnitID != -1)
			{
				pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
				pCivilianPlot = pCivilian->plot();
			}
			if (m_eCurrentState == AI_OPERATION_STATE_MOVING_TO_TARGET && pCivilianPlot == GetTargetPlot())
			{
				ArmyInPosition(pThisArmy);
				return true;
			}
			else if (m_eCurrentState == AI_OPERATION_STATE_GATHERING_FORCES)
			{
				if (pThisArmy->GetNumSlotsFilled() == 1)
				{
					ArmyInPosition(pThisArmy);
					return true;
				}
				else
				{
					pEscortPlot = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID())->plot();
					if (pCivilianPlot == pEscortPlot)
					{
						ArmyInPosition(pThisArmy);
						return true;
					}
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
			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				CvPlot *pCenterOfMass;
				int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());

				if (pThisArmy->GetNumSlotsFilled() >= 1)
				{
					switch (m_eCurrentState)
					{
					case AI_OPERATION_STATE_GATHERING_FORCES:
						pCenterOfMass = pThisArmy->GetCenterOfMass();
						if (pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
						break;
					case AI_OPERATION_STATE_MOVING_TO_TARGET:
						pCenterOfMass = pThisArmy->GetCenterOfMass();
						if (pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pThisArmy->GetGoalX(), pThisArmy->GetGoalY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(pThisArmy->GetGoalPlot()) <= (iGatherTolerance * 3 / 2))
						{
							ArmyInPosition(pThisArmy);
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

	case AI_OPERATION_MOVETYPE_NAVAL_ESCORT:
		{
			// Let each army perform its own check
			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				CvPlot *pCenterOfMass;
				int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());

				if (pThisArmy->GetNumSlotsFilled() >= 1)
				{
					switch (m_eCurrentState)
					{
					case AI_OPERATION_STATE_GATHERING_FORCES:
						pCenterOfMass = pThisArmy->GetCenterOfMass();
						if (pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
						break;
					case AI_OPERATION_STATE_MOVING_TO_TARGET:
						pCenterOfMass = pThisArmy->GetCenterOfMass();
						if (pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pThisArmy->GetGoalX(), pThisArmy->GetGoalY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(pThisArmy->GetGoalPlot()) <= (iGatherTolerance * 3))
						{
							ArmyInPosition(pThisArmy);
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

	case AI_OPERATION_MOVETYPE_FREEFORM_NAVAL:
		{
			// Let each army perform its own check
			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				CvPlot *pCenterOfMass;
				int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());

				if (pThisArmy->GetNumSlotsFilled() >= 1)
				{
					switch (m_eCurrentState)
					{
					case AI_OPERATION_STATE_GATHERING_FORCES:
						pCenterOfMass = pThisArmy->GetCenterOfMass();
						if (pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
						break;
					case AI_OPERATION_STATE_MOVING_TO_TARGET:
						break;  // Never in a final position
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
bool CvAIOperation::ArmyInPosition(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
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
			if (pArmy->Plot() == GetTargetPlot())
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

	switch (m_eCurrentState)
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
			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

				int iDistanceMusterToTarget;
				int iDistanceCurrentToTarget;

				// Use the step path finder to compute distance
				iDistanceMusterToTarget = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, GetMusterPlot(), pArmy->GetGoalPlot());
				iDistanceCurrentToTarget = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pArmy->Plot(), pArmy->GetGoalPlot());

				if (iDistanceMusterToTarget <= 0)
				{
					return 0;
				}
				else
				{
					int iTempValue = 100 - (100 * iDistanceCurrentToTarget / iDistanceMusterToTarget);
					if (iTempValue > iRtnValue)
					{
						iRtnValue = iTempValue;
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
	UnitHandle pUnit;

	// Mark units in successful operation
	if (m_eCurrentState == AI_OPERATION_STATE_SUCCESSFUL_FINISH)
	{
		for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);

			pUnit = pArmy->GetFirstUnit();
			while (pUnit)
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
void CvAIOperation::Kill()
{
	int iID = m_iID;
	PlayerTypes eOwner = GetOwner();
	if (m_eAbortReason == NO_ABORT_REASON)
	{
		m_eAbortReason = AI_ABORT_KILLED;
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
	if (ShouldAbort())
	{
		Kill();
		return true;
	}

	return false;
}

/// Find the area where our operation is occurring
CvCity *CvAIOperation::GetOperationStartCity() const
{
	if (GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	CvCity* pCapitalCity;
	CvArea* pLoopArea;
	int iLoop;
	int iMyCities;
	int iEnemyCities;
	int iBestTotal = 0;
	CvArea *pBestArea = NULL;
	CvCity *pBestCity = NULL;

	CvPlayer& thisPlayer = GET_PLAYER(m_eOwner);

	// Do we still have a capital?
	pCapitalCity = thisPlayer.getCapitalCity();

	if (pCapitalCity != NULL)
	{
		return pCapitalCity;
	}

	// No capital, find the area with the most combined cities between us and our enemy (and need at least 1 from each)
	for (pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if (pLoopArea->isWater())
		{
			continue;
		}

		iMyCities = pLoopArea->getCitiesPerPlayer(GetOwner());
		if (iMyCities > 0)
		{
			if (m_eEnemy != NO_PLAYER && m_eEnemy != BARBARIAN_PLAYER)
			{
				iEnemyCities = pLoopArea->getCitiesPerPlayer(m_eEnemy);
				if (iEnemyCities == 0)
				{
					continue;
				}
			}
			else
			{
				iEnemyCities = 0;
			}

			if ((iMyCities + iEnemyCities) > iBestTotal)
			{
				iBestTotal = iMyCities + iEnemyCities;
				pBestArea = pLoopArea;
			}
		}
	}

	if (pBestArea != NULL)
	{
		// Know which continent to use, now use our largest city there as the start city
		CvCity *pCity;
		iBestTotal = 0;
		for (pCity = thisPlayer.firstCity(&iLoop); pCity != NULL; pCity = thisPlayer.nextCity(&iLoop))
		{
			if (pCity->getArea() == pBestArea->GetID())
			{
				if (pCity->getPopulation() > iBestTotal)
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
	for (iter = m_viArmyIDs.begin(); iter != m_viArmyIDs.end(); ++iter)
	{
		if (*iter == iID)
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
	switch (m_eCurrentState)
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
				if (pThisArmy->GetNumSlotsFilled() <=0 ||
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
CvPlot *CvAIOperation::ComputeCenterOfMassForTurn(CvArmyAI *pArmy) const
{
	CvPlot *pRtnValue = NULL;

	switch (m_eCurrentState)
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
			CvPlot *pCenterOfMass = 0;
			CvPlot *pLastTurnArmyPlot = 0;
			CvAStarNode *pNode = 0;
			int iBestDistanceToCOM = MAX_INT;
			int iDistanceToCOM = 0;
			int iBestNodeIndex = 0;
			FStaticVector<CvAStarNode *, SAFE_ESTIMATE_MAX_PATH_LEN, true, c_eCiv5GameplayDLL, 0> m_NodesOnPath;

			m_NodesOnPath.clear();

			pLastTurnArmyPlot = pArmy->Plot();
			pCenterOfMass = pArmy->GetCenterOfMass();
			if (pLastTurnArmyPlot && pCenterOfMass)
			{
				// Push center of mass forward a number of hexes equal to average movement
				GC.getStepFinder().SetData(&m_eEnemy);
				if (GC.getStepFinder().GeneratePath(pLastTurnArmyPlot->getX(), pLastTurnArmyPlot->getY(), pArmy->GetGoalPlot()->getX(), pArmy->GetGoalPlot()->getY(), m_eOwner, false))
				{
					pNode = GC.getStepFinder().GetLastNode();

					// Starting at the end, loop until we find the plot closest to center of mass
					while (pNode)
					{
						m_NodesOnPath.push_back(pNode);
						iDistanceToCOM = plotDistance(pNode->m_iX, pNode->m_iY, pCenterOfMass->getX(), pCenterOfMass->getY());
						if (iDistanceToCOM < iBestDistanceToCOM)
						{
							iBestDistanceToCOM = iDistanceToCOM;
							iBestNodeIndex = m_NodesOnPath.size() - 1;
						}
						pNode = pNode->m_pParent;
					}

					// Now move back up path from best node a number of spaces equal to army's movement rate
					if (iBestNodeIndex > pArmy->GetMovementRate())
					{
						pNode = m_NodesOnPath[iBestNodeIndex - pArmy->GetMovementRate()];
					}
					else
					{
						pNode = m_NodesOnPath[0];
					}
					pRtnValue = GC.getMap().plot(pNode->m_iX, pNode->m_iY);
				}
			}
		}
		break;
	};

	return pRtnValue;
}

bool CvAIOperation::HasOneMoreSlotToFill() const
{
	if (m_viListOfUnitsWeStillNeedToBuild.size() == 1 && m_eCurrentState  == AI_OPERATION_STATE_RECRUITING_UNITS)
	{
		return true;
	}

	return false;
}

bool CvAIOperation::BuyFinalUnit()
{
	CvCity *pCity = GetOperationStartCity();

	if (!m_viListOfUnitsWeStillNeedToBuild.empty() && pCity != NULL && pCity->getOwner() == m_eOwner)
	{
		OperationSlot thisSlot = m_viListOfUnitsWeStillNeedToBuild.back();
		CvArmyAI *pArmy = GET_PLAYER(m_eOwner).getArmyAI(thisSlot.m_iArmyID);
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(pArmy->GetFormationIndex());
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry (thisSlot.m_iSlotID);

		CvUnit *pUnit = GET_PLAYER(m_eOwner).GetMilitaryAI()->BuyEmergencyUnit((UnitAITypes)thisSlotEntry.m_primaryUnitType, pCity);
		if (pUnit != NULL)
		{
			pArmy->AddUnit(pUnit->GetID(), thisSlot.m_iSlotID);
			m_viListOfUnitsWeStillNeedToBuild.pop_back();
			return true;
		}
	}

	return false;
}

/// Read serialized data
void CvAIOperation::Read(FDataStream &kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

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
	if (uiVersion >= 2)
	{
		kStream >> m_iStartCityX;
		kStream >> m_iStartCityY;
	}
	else
	{
		m_iStartCityX = -1;
		m_iStartCityY = -1;
	}
	kStream >> m_iLastTurnMoved;
	kStream >> m_viArmyIDs;
	kStream >> m_viListOfUnitsWeStillNeedToBuild;
	kStream >> m_viListOfUnitsCitiesHaveCommittedToBuild;
}

/// Write serialized data
void CvAIOperation::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

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

// PRIVATE FUNCTIONS

/// Log that an operation has started
void CvAIOperation::LogOperationStart()
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
			strTemp.Format("Started, Army: %d, Units Recruited: %d, Max Formation Size: %d, ", pThisArmy->GetID(), pThisArmy->GetNumSlotsFilled(), pThisArmy->GetNumFormationEntries());
		}

		strOutBuf = strBaseString + strTemp;
		switch (m_eCurrentState)
		{
		case AI_OPERATION_STATE_ABORTED:
			strTemp = "Aborted";
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp = "Recruiting Units";
			break;
		case AI_OPERATION_STATE_GATHERING_FORCES:
			strTemp = "Gathering Forces";
			break;
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
			strTemp = "Moving To Target";
			break;
		case AI_OPERATION_STATE_AT_TARGET:
			strTemp = "At Target";
			break;
		case AI_OPERATION_STATE_SUCCESSFUL_FINISH:
			strTemp = "Completed";
			break;
		};

		strOutBuf += strTemp;

		pLog->Msg(strOutBuf);
	}
}

/// Log current status of the operation
void CvAIOperation::LogOperationStatus()
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2, szTemp3;
		CvString strPlayerName;
		FILogFile *pLog;

		strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		switch (m_eCurrentState)
		{
		case AI_OPERATION_STATE_ABORTED:
			strTemp = "Aborted";
			break;
		case AI_OPERATION_STATE_RECRUITING_UNITS:
			strTemp = "";
			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Recruiting Units, Army: %d, Muster Turn: %d, SLOT DETAIL:, ", pThisArmy->GetID(), pThisArmy->GetTurnAtNextCheckpoint());
				strTemp += szTemp2;

				for (int iI = 0; iI < pThisArmy->GetNumFormationEntries(); iI++)
				{
					CvArmyFormationSlot *pSlot = pThisArmy->GetFormationSlot(iI);
					if (pSlot->GetTurnAtCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
					{
						szTemp2 = "No Info, ";
					}
					else if (pSlot->GetTurnAtCheckpoint() == ARMYSLOT_NOT_INCLUDING_IN_OPERATION)
					{
						szTemp2 = "Skipping, ";
					}
					else if (pSlot->GetUnitID() == NO_UNIT)
					{
						szTemp3.Format ("Turn %d, ", pSlot->GetTurnAtCheckpoint());
						szTemp2 = "Training - " + szTemp3;
					}
					else if (pSlot->HasStartedOnOperation())
					{
						UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if (pThisUnit)
						{
							szTemp2.Format ("Gathering at (%d-%d), ", pThisUnit->getX(), pThisUnit->getY());
						}
					}
					else
					{
						UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(pSlot->GetUnitID());
						if (pThisUnit)
						{
							szTemp2.Format ("%s - Turn %d, ", pThisUnit->getName().GetCString(), pSlot->GetTurnAtCheckpoint());
						}
					}
					strTemp += szTemp2;
				}
			}
			break;
		case AI_OPERATION_STATE_GATHERING_FORCES:
			strTemp = "";
			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Gathering Forces, Army: %d, Gather X: %d, Gather Y: %d, ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY());
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while (iUnitID != ARMY_NO_UNIT)
				{
					// do something with each entry
					UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if (pThisUnit)
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
			for (unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				szTemp2.Format("Moving To Target, Army: %d, At X: %d, At Y: %d, To X: %d, To Y: %d, ", pThisArmy->GetID(), pThisArmy->GetX(), pThisArmy->GetY(), m_iTargetX, m_iTargetY);
				strTemp += szTemp2;
				int iUnitID;
				iUnitID = pThisArmy->GetFirstUnitID();
				while (iUnitID != ARMY_NO_UNIT)
				{
					// do something with each entry
					UnitHandle pThisUnit = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					if (pThisUnit)
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
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// Log that an operation has ended
void CvAIOperation::LogOperationEnd()
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;

		CvString strPlayerName = GET_PLAYER(m_eOwner).getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", " + GetOperationName() + ", ";

		strTemp = "Ended, ";

		switch (m_eAbortReason)
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
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvAIOperation::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if (GC.getPlayerAndCityAILogSplit())
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
CvPlot *CvAIOperation::SelectInitialMusterPoint (CvArmyAI* pThisArmy)
{
	CvPlot *pMusterPt = NULL;
	CvPlot *pStartCityPlot;
	CvCity *pStartCity;

	pStartCity = GetOperationStartCity();
	if (pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if (pStartCityPlot != NULL)
		{
			// Generate a step path from our start to the target
			pMusterPt = GC.getStepFinder().GetLastOwnedPlot(GetOwner(), GetEnemy(), pStartCityPlot, pThisArmy->GetGoalPlot());

			// Should find a space we own
			if (pMusterPt == NULL)
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
	CvUnit* pkBestUnit = NULL;
	if (kSearchList.size())
	{
		std::stable_sort(kSearchList.begin(), kSearchList.end());

		int iBestDistance = MAX_INT;
		CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();
		for (CvOperationSearchUnitList::iterator itr = kSearchList.begin(); itr != kSearchList.end(); ++itr)
		{
			CvUnit *pkLoopUnit = (*itr).GetUnit();
			int iDistance = (*itr).GetDistance();

			int iPathDistance = MAX_INT;
			// Now loop through the units, using the pathfinder to do the final evaluation
			if (pkMusterPlot != NULL)
			{
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkMusterPlot))
					continue;

				iPathDistance = kPathFinder.GetPathLength();
			}

			if(pkTarget != NULL && bNeedToCheckTarget)
			{
				if (!kPathFinder.DoesPathExist(*pkLoopUnit, pkLoopUnit->plot(), pkTarget))
					continue;

				if (pkMusterPlot == NULL)
					iPathDistance = kPathFinder.GetPathLength();
			}

			// Reasonably close?
			if (iPathDistance <= iDistance && iPathDistance <= iBestDistance)
			{
				pkBestUnit = pkLoopUnit;
				break;
			}

			if (iPathDistance < iBestDistance)
			{
				pkBestUnit = pkLoopUnit;
				iBestDistance = iPathDistance;
			}

			// Were we far away?  If so, this is probably the best we are going to do
			if (iDistance >= GC.getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE())
				break;
		}
	}

	return pkBestUnit;
}

/// Find a unit from our reserves that could serve in this operation
bool CvAIOperation::FindBestFitReserveUnit (OperationSlot thisOperationSlot, CvPlot *pMusterPlot, CvPlot *pTargetPlot, bool *bRequired)
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit = NULL;
	int iLoop = 0;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;

	CvOperationSearchUnitList kSearchList;

	*bRequired = true;

	int iThisFormationIndex = pThisArmy->GetFormationIndex();
	if (iThisFormationIndex != NO_MUFORMATION)
	{
		CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
		if(thisFormation)
		{
			const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry (thisOperationSlot.m_iSlotID);

			for (pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
			{
				// Make sure he's not needed by the tactical AI or already in an army or scouting
				if (pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX &&
					pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if (unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType) ||
						unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_secondaryUnitType))
					{
						// Is his health okay?
						if (!pLoopUnit->IsCombatUnit() || pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
						{
							// Not finishing up an operation?
							if (pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
							{
								if ((!IsAllNavalOperation() && !IsMixedLandNavalOperation()) || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
								{
									// Get raw distance to the muster point or target
									CvPlot* pkLoopUnitPlot = pLoopUnit->plot();
									int iDistance;
									if(pMusterPlot != NULL)
									{
										iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
										// Double the distance if this is a land unit on a different landmass (it's dangerous to go over water!)
										if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea())
										{
											iDistance *= 2;
										}
									}
									else
									{
										if (pTargetPlot != NULL)
											iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY());
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

			pBestUnit = GetClosestUnit(kSearchList, pMusterPlot, pTargetPlot, NeedToCheckPathToTarget());

			// Did we find one?
			if (pBestUnit != NULL)
			{
				pThisArmy->AddUnit(pBestUnit->GetID(), thisOperationSlot.m_iSlotID);
				return true;
			}
			else
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Could not recruit from primary units for muster at x=%d y=%d, target of x=%d y=%d", (pMusterPlot)?pMusterPlot->getX():-1, (pMusterPlot)?pMusterPlot->getY():-1, (pTargetPlot)?pTargetPlot->getX():-1, (pTargetPlot)?pTargetPlot->getY():-1);
					LogOperationSpecialMessage(strMsg);
				}
			}

			// If not required, let our calling routine know that
			if (!thisSlotEntry.m_requiredSlot)
			{
				*bRequired = false;
			}
		}
	}

	return false;
}

FDataStream & operator<<(FDataStream & saveTo, const AIOperationMovementType & readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream & operator>>(FDataStream & loadFrom, AIOperationMovementType & writeTo)
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
void CvAIEnemyTerritoryOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /* iDefaultArea */, CvCity *, CvCity *)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if (iID != -1 )
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if (pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			// Figure out the initial rally point
			CvPlot *pTargetPlot = FindBestTarget();
			if (pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if (SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());

					// Reset our destination to be a few plots shy of the final target
					CvPlot *pDeployPt;
					pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), GetMusterPlot(), GetTargetPlot(), GetDeployRange());
					pArmyAI->SetGoalPlot(pDeployPt);

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if (GrabUnitsFromTheReserves(GetMusterPlot(), pTargetPlot))
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

/// Figure out the initial rally point
CvPlot *CvAIEnemyTerritoryOperation::SelectInitialMusterPoint (CvArmyAI* pThisArmy)
{
	CvPlot *pMusterPt = NULL;
	CvPlot *pStartCityPlot;
	CvCity *pStartCity;
	CvPlot *pCurrentPlot;
	int iSpacesFromTarget = 0;
	CvPlot *pDeployPlot = NULL;
	int iDangerousPlots = 0;

	pStartCity = GetOperationStartCity();
	if (pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if (pStartCityPlot != NULL)
		{
			CvAStarNode* pNode;

			// Generate path
			GC.getStepFinder().SetData(&m_eEnemy);
			if (GC.getStepFinder().GeneratePath(pStartCityPlot->getX(), pStartCityPlot->getY(), pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY(), m_eOwner, false))
			{
				pNode = GC.getStepFinder().GetLastNode();

				// Starting at the end, loop until we find a plot from this owner
				while (pNode != NULL)
				{
					pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);

					// Is this the deploy point?
					if (iSpacesFromTarget == GetDeployRange())
					{
						pDeployPlot = pCurrentPlot;
					}

					// Check and see if this plot has the right owner
					if (pCurrentPlot->getOwner() == GetOwner())
					{
						pMusterPt = pCurrentPlot;
						break;
					}

					else
					{
						// Is this a dangerous plot?
						if (GET_PLAYER(m_eOwner).GetPlotDanger(*pCurrentPlot) > 0)
						{
							iDangerousPlots++;
						}
					}

					// Move to the previous plot on the path
					iSpacesFromTarget++;
					pNode = pNode->m_pParent;
				}

				// Is the path safe?  If so, let's just muster at the deploy point
				if (iSpacesFromTarget > 0 && (iDangerousPlots * 100 / iSpacesFromTarget) < GC.getAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER())
				{
					if (pDeployPlot)
					{
						pMusterPt = pDeployPlot;
					}
				}
			}
		}
	}

	if (pMusterPt != NULL)
	{
		SetMusterPlot(pMusterPt);
	}
	else
	{
		if (GC.getLogging() && GC.getAILogging())
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
void CvAIOperationBasicCityAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /* iDefaultArea */, CvCity *pTarget, CvCity *pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	SetStartCityPlot(pMuster->plot());

	if (iID != -1 )
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if (pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			if (pTarget)
			{
				SetTargetPlot(pTarget->plot());
				pArmyAI->SetGoalPlot(GetTargetPlot());
				if (SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());

					// Reset our destination to be a few plots shy of the final target
					CvPlot *pDeployPt;
					pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), GetMusterPlot(), GetTargetPlot(), GetDeployRange());
					pArmyAI->SetGoalPlot(pDeployPt);

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if (GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
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

/// Read serialized data
void CvAIOperationBasicCityAttack::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
}

/// Write serialized data
void CvAIOperationBasicCityAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// Same as default version except if just gathered forces, check to see if a better target has presented itself
bool CvAIOperationBasicCityAttack::ArmyInPosition(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
		{
			// First do base case processing
			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

			// Is target still under enemy control?
			CvPlot *pTarget = GetTargetPlot();
			if (pTarget->getOwner() != m_eEnemy)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
			}
		}
		break;

		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			if (pArmy->Plot() == pArmy->GetGoalPlot())
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

	if (!rtnValue)
	{
		// See if our target city is still owned by our enemy
		if (GetTargetPlot()->getPlotCity() == NULL || GetTargetPlot()->getOwner() != m_eEnemy)
		{
			// Success!  The city has been captured/destroyed
			return true;
		}
	}

	return rtnValue;
}

/// Find the city we want to attack
CvPlot *CvAIOperationBasicCityAttack::FindBestTarget()
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

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityStateAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCityStateAttack::CvAIOperationCityStateAttack()
{
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationDestroyBarbarianCamp
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationDestroyBarbarianCamp::CvAIOperationDestroyBarbarianCamp()
{
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
}

/// Write serialized data
void CvAIOperationDestroyBarbarianCamp::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// Same as default version except if just gathered forces, check to see if a better target has presented itself
bool CvAIOperationDestroyBarbarianCamp::ArmyInPosition(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
		{
			// First do base case processing
			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

			// Now revisit target
			CvPlot *possibleBetterTarget;
			possibleBetterTarget = FindBestTarget();

			// If no target left, abort
			if (possibleBetterTarget == NULL)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}

			// If target changed, reset to this new one
			else if (possibleBetterTarget != GetTargetPlot())
			{
				// Reset our destination to be a few plots shy of the final target
				CvPlot *pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GC.getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE());
				if (pDeployPt != NULL)
				{
					pArmy->SetGoalPlot(pDeployPt);
					SetTargetPlot(possibleBetterTarget);
				}
			}
		}
		break;

		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			if (pArmy->Plot() == pArmy->GetGoalPlot())
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

	if (!rtnValue)
	{
		// See if our target camp is still there
		if (GetTargetPlot()->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			// Success!  The camp is gone
			if (GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format ("Barbarian camp at (x=%d y=%d) no longer exists. Aborting", GetTargetPlot()->getX(), GetTargetPlot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
			return true;
		}

		else if (m_eCurrentState != AI_OPERATION_STATE_RECRUITING_UNITS)
		{
			// If down below strength of camp, abort
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			CvPlot *pTarget = GetTargetPlot();
			UnitHandle pCampDefender = pTarget->getBestDefender(NO_PLAYER);
			if (pCampDefender && pThisArmy->GetTotalPower() < pCampDefender->GetPower())
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format ("Barbarian camp stronger (%d) than our units (%d). Aborting", pCampDefender->GetPower(), pThisArmy->GetTotalPower());
					LogOperationSpecialMessage(strMsg);
				}
				return true;
			}
		}
	}

	return rtnValue;
}

/// Find the barbarian camp we want to eliminate
CvPlot *CvAIOperationDestroyBarbarianCamp::FindBestTarget()
{
	int iPlotLoop;
	CvPlot *pBestPlot = NULL;
	CvPlot *pPlot;
	int iBestPlotDistance = MAX_INT;
	int iCurPlotDistance;

	TeamTypes eTeam = GET_PLAYER(m_eOwner).getTeam();
	ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();

	CvCity *pStartCity;
	pStartCity = GetOperationStartCity();
	if (pStartCity != NULL)
	{
		// Look at map for Barbarian camps and units
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
						//bLeaveForAnotherPlayer = false;
						iCurPlotDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pPlot, pStartCity->plot());

						if (/*!bLeaveForAnotherPlayer && */iCurPlotDistance < iBestPlotDistance)
						{
							pBestPlot = pPlot;
							iBestPlotDistance = iCurPlotDistance;
						}
					}
				}
			}
		}
	}

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
}

/// Write serialized data
void CvAIOperationPillageEnemy::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
bool CvAIOperationPillageEnemy::ArmyMoved(CvArmyAI *pArmy)
{
	bool bStateChanged = false;
	CvPlot *pBetterTarget;

	switch (m_eCurrentState)
	{
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			// Find best pillage target
			pBetterTarget = FindBestTarget();

			// No targets at all!  Abort
			if (pBetterTarget == NULL)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_TARGET;
			}

			// If this is a new target, switch to it
			else if (pBetterTarget != GetTargetPlot())
			{
				SetTargetPlot(pBetterTarget);
				pArmy->SetGoalPlot(pBetterTarget);

				// Reset our destination to be a few plots shy of the final target
				CvPlot *pDeployPt;
				pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), pBetterTarget, GC.getAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE());
				pArmy->SetGoalPlot(pDeployPt);
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
bool CvAIOperationPillageEnemy::ArmyInPosition(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
	{
		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			if (pArmy->Plot() == pArmy->GetGoalPlot())
			{
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
CvPlot *CvAIOperationPillageEnemy::FindBestTarget()
{
	CvCity *pBestTargetCity = NULL;
	int iValue;
	int iBestValue;
	CvCity *pLoopCity;
	CvCity *pStartCity;
	int iDistance;
	int iLoop;

	CvPlayerAI& kEnemyPlayer = GET_PLAYER(m_eEnemy);

	if (!kEnemyPlayer.isAlive())
	{
		return NULL;
	}

	iBestValue = 0;
	pStartCity = GetOperationStartCity();
	if (pStartCity != NULL)
	{
		for (pLoopCity = kEnemyPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kEnemyPlayer.nextCity(&iLoop))
		{
			// Make sure city is in the same area as our start city
			if (pLoopCity->getArea() == pStartCity->getArea())
			{
				// Initial value of target is the number of improved plots
				iValue = pLoopCity->countNumImprovedPlots();

				// Adjust value based on proximity to our start location
				iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pLoopCity->plot(), pStartCity->plot());
				if (iDistance > 0)
				{
					iValue = iValue * 100 / iDistance;
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestTargetCity = pLoopCity;
				}
			}
		}
	}

	if (pBestTargetCity == NULL)
	{
		return NULL;
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
}

CvAIEscortedOperation::~CvAIEscortedOperation()
{
}

/// Kick off this operation
void CvAIEscortedOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes /* eEnemy */, int /* iDefaultArea */, CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	CvUnit *pOurCivilian;
	CvPlot *pTargetSite, *pNewTarget;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;

	// Find the free civilian (that triggered this operation)
	pOurCivilian = FindBestCivilian();

	if (pOurCivilian != NULL && iID != -1 )
	{
		// Find a destination (not worrying about safe paths)
		pTargetSite = FindBestTarget(pOurCivilian, false);

		if (pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);

			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if (pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());

				// Figure out the initial rally point - for this operation it is wherever our civilian is standing
				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot *pMusterPt = pOurCivilian->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				SetDefaultArea(pMusterPt->getArea());

				// Add the settler to our army
				pArmyAI->AddUnit (pOurCivilian->GetID(), 0);

				// Add the escort as a unit we need to build
				m_viListOfUnitsWeStillNeedToBuild.clear();
				OperationSlot thisOperationSlot;
				thisOperationSlot.m_iOperationID = m_iID;
				thisOperationSlot.m_iArmyID = pArmyAI->GetID();
				thisOperationSlot.m_iSlotID = 1;
				m_viListOfUnitsWeStillNeedToBuild.push_back(thisOperationSlot);

				// try to get the escort from existing units that are waiting around
				GrabUnitsFromTheReserves(pMusterPt, pTargetSite);
				if (pArmyAI->GetNumSlotsFilled() > 1)
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
				}
				else
				{
					// There was no escort immediately available.  Let's look for a "safe" city site instead
					pNewTarget = FindBestTarget(pOurCivilian, true);

					// If no better target, we'll wait it out for an escort
					if (pNewTarget == NULL)
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

/// Read serialized data
void CvAIEscortedOperation::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_bEscorted;
	kStream >> m_eCivilianType;
}

/// Write serialized data
void CvAIEscortedOperation::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_bEscorted;
	kStream << m_eCivilianType;
}

/// Always abort if settler is removed
void CvAIEscortedOperation::UnitWasRemoved(int /*iArmyID*/, int iSlotID)
{
	// Assumes civilian is in the first slot of the formation
	if (iSlotID == 0)
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
CvUnit *CvAIEscortedOperation::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit *pLoopUnit;

	for (pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
			{
				if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					CvCity *pCapitalCity = GET_PLAYER(m_eOwner).getCapitalCity();
					if (pCapitalCity != NULL)
					{
						int iCapitalID = pCapitalCity->getArea();
						{
							if (pLoopUnit->getArea() == iCapitalID)
							{
								return pLoopUnit;
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

/// Start the civilian off to a new target plot
bool CvAIEscortedOperation::RetargetCivilian(CvUnit *pCivilian, CvArmyAI *pArmy)
{
	CvPlot *pBetterTarget;

	// Find best city site (taking into account whether or not we are escorted)
	pBetterTarget = FindBestTarget(pCivilian, !m_bEscorted);

	// No targets at all!  Abort
	if (pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}
	// If this is a new target, switch to it
	else if (pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}
	else
	{
		SetToAbort(AI_ABORT_REPEAT_TARGET);
		return false;
	}

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

/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAIOperationFoundCity::ArmyInPosition(CvArmyAI *pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit *pSettler = 0, *pEscort = 0;
	CvString strMsg;

	switch (m_eCurrentState)
	{
		// If we were gathering forces, we have to insist that any escort is in the same plot as the settler.
		// If not we'll fall through and just stay in this state.
	case AI_OPERATION_STATE_GATHERING_FORCES:

		// No escort, can just let base class handle it
		if (!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}

		// More complex if we are waiting for an escort
		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if (iUnitID != -1)
			{
				pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if (iUnitID != -1)
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
			if (pSettler != NULL && pEscort != NULL && pSettler->plot() == pEscort->plot())
			{
				// let's see if the target still makes sense (this is modified from RetargetCivilian)
				CvPlot* pBetterTarget = FindBestTarget(pSettler, true);

				// No targets at all!  Abort
				if (pBetterTarget == NULL)
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

		// Now get the settler
		iUnitID = pArmy->GetFirstUnitID();
		if (iUnitID != -1)
		{
			pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if (pSettler != NULL)
		{
			if ((GetTargetPlot()->getOwner() != NO_PLAYER && GetTargetPlot()->getOwner() != m_eOwner) || GetTargetPlot()->IsAdjacentOwnedByOtherTeam(pSettler->getTeam()))
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("No at target but can no longer settle here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if (iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
			// If the settler made it, we don't care about the entire army
			else if (pSettler->plot() == GetTargetPlot() && pSettler->canMove() && pSettler->canFound(pSettler->plot()))
			{
				pSettler->PushMission(CvTypes::getMISSION_FOUND());
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("City founded, At X=%d, At Y=%d", pSettler->plot()->getX(), pSettler->plot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}

			// If we're at our target but can no longer found a city, might be someone else beat us to this area
			// So move back out, picking a new target
			else if (pSettler->plot() == GetTargetPlot() && !pSettler->canFound(pSettler->plot()))
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("At target but can no longer settle here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if (iUnitID != -1)
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

	return bStateChanged;
}

/// Find the plot where we want to settle
CvPlot *CvAIOperationFoundCity::FindBestTarget(CvUnit *pUnit, bool bOnlySafePaths)
{
	return GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, pUnit->getArea());
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
void CvAIOperationQuickColonize::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int /*iDefaultArea*/, CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	CvUnit *pOurCivilian;
	CvPlot *pTargetSite;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_SINGLE_HEX;
	m_iID = iID;
	m_eOwner = eOwner;

	// Find the free civilian (that triggered this operation)
	pOurCivilian = FindBestCivilian();

	if (pOurCivilian != NULL && iID != -1 )
	{
		// Find a destination (not worrying about safe paths)
		pTargetSite = FindBestTarget(pOurCivilian, false);

		if (pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);

			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if (pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetFormationIndex(GetFormation());

				// Figure out the initial rally point - for this operation it is wherever our civilian is standing
				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot *pMusterPt = pOurCivilian->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());
				pArmyAI->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
				SetDefaultArea(pMusterPt->getArea());

				// Add the settler to our army
				pArmyAI->AddUnit (pOurCivilian->GetID(), 0);
				m_bEscorted = false;

				m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
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
CvUnit *CvAIOperationQuickColonize::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit *pLoopUnit;

	for (pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
			{
				if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					return pLoopUnit;
				}
			}
		}
	}
	return NULL;
}

/// Find the plot where we want to settle
CvPlot *CvAIOperationQuickColonize::FindBestTarget(CvUnit *pUnit, bool /*bOnlySafePaths*/)
{
	return GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, false, -1);
}


CvAIOperationNotSoQuickColonize::CvAIOperationNotSoQuickColonize()
{
}

/// Destructor
CvAIOperationNotSoQuickColonize::~CvAIOperationNotSoQuickColonize()
{
}


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
bool CvAIOperationMerchantDelegation::ArmyInPosition(CvArmyAI *pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit *pMerchant = 0, *pEscort = 0;
	CvString strMsg;

	switch (m_eCurrentState)
	{
		// If we were gathering forces, we have to insist that any escort is in the same plot as the merchant.
		// If not we'll fall through and just stay in this state.
	case AI_OPERATION_STATE_GATHERING_FORCES:

		// No escort, can just let base class handle it
		if (!m_bEscorted)
		{
			return CvAIOperation::ArmyInPosition(pArmy);
		}

		// More complex if we are waiting for an escort
		else
		{
			iUnitID = pArmy->GetFirstUnitID();
			if (iUnitID != -1)
			{
				pMerchant = GET_PLAYER(m_eOwner).getUnit(iUnitID);
			}
			iUnitID = pArmy->GetNextUnitID();
			if (iUnitID != -1)
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
			if (pMerchant != NULL && pEscort != NULL && pMerchant->plot() == pEscort->plot())
			{
				RetargetCivilian(pMerchant, pArmy);
				bStateChanged = true;
			}
		}
		break;

	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:

		// Call base class version and see if it thinks we're done
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now get the merchant
		iUnitID = pArmy->GetFirstUnitID();
		if (iUnitID != -1)
		{
			pMerchant = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if (pMerchant != NULL)
		{
			// If the merchant made it, we don't care about the entire army
			if (pMerchant->plot() == GetTargetPlot() && pMerchant->canMove() && pMerchant->canTrade(pMerchant->plot()))
			{
				pMerchant->PushMission(CvTypes::getMISSION_TRADE());
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Great Merchant cashing in trade, At X=%d, At Y=%d", pMerchant->plot()->getX(), pMerchant->plot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}

			// Does it look like we should be done?
			else if (pMerchant->plot() == GetTargetPlot())
			{
				// We're at our target but can no longer trade, city state was probably conquered
				if (!pMerchant->canTrade(pMerchant->plot()))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("At target but can no longer trade here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
						LogOperationSpecialMessage(strMsg);
					}
					RetargetCivilian(pMerchant, pArmy);
					pMerchant->finishMoves();
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
CvPlot *CvAIOperationMerchantDelegation::FindBestTarget(CvUnit *pUnit, bool bOnlySafePaths)
{
	CvAssertMsg(pUnit, "pUnit cannot be null");
	if (!pUnit)
	{
		return NULL;
	}

	return GET_PLAYER(pUnit->getOwner()).FindBestMerchantTargetPlot (pUnit, bOnlySafePaths);
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

/// Read serialized data
void CvAINavalOperation::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
}

/// Write serialized data
void CvAINavalOperation::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// How close to target do we end up?
int CvAINavalOperation::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_NAVAL_BOMBARDMENT_DEPLOY_RANGE();
}

/// Find the port our operation will leave from
CvCity *CvAINavalOperation::GetOperationStartCity() const
{
	if (GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	int iLoop;
	CvCity *pCity;

	// Just find first coastal city
	for (pCity = GET_PLAYER(m_eOwner).firstCity(&iLoop); pCity != NULL; pCity = GET_PLAYER(m_eOwner).nextCity(&iLoop))
	{
		if (pCity->isCoastal())
		{
			return pCity;
		}
	}

	return NULL;
}

/// Figure out the initial rally point
CvPlot *CvAINavalOperation::SelectInitialMusterPoint (CvArmyAI* pThisArmy)
{
	CvPlot *pMusterPt = NULL;
	CvPlot *pStartCityPlot;
	CvCity *pStartCity;
	CvPlot *pCurrentPlot;
	CvPlot *pAdjacentPlot;
	int iSpacesFromTarget = 0;
	int iDirectionLoop;
	CvAStarNode* pNode;
	bool bMusterPointFound = false;

	pStartCity = GetOperationStartCity();
	if (pStartCity)
	{
		pStartCityPlot = pStartCity->plot();

		if (pStartCityPlot != NULL)
		{
			// Find an adjacent water tile
			for (iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES && !bMusterPointFound; ++iDirectionLoop)
			{
				pAdjacentPlot = plotDirection(pStartCityPlot->getX(), pStartCityPlot->getY(), ((DirectionTypes)iDirectionLoop));
				if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
				{
					// Generate path
					GC.getStepFinder().SetData(&m_eEnemy);
					if (GC.getStepFinder().GeneratePath(pAdjacentPlot->getX(), pAdjacentPlot->getY(), pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY(), m_eOwner, false))
					{
						pNode = GC.getStepFinder().GetLastNode();

						// Starting at the end, loop until we find a plot from this owner
						while (pNode != NULL)
						{
							pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);

							// Is this the deploy point?
							if (iSpacesFromTarget == GetDeployRange())
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

	if (pMusterPt != NULL)
	{
		SetMusterPlot(pMusterPt);
	}
	else
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("No muster point found, Naval operation aborting, Target was, X: %d, Y: %d", pThisArmy->GetGoalPlot()->getX(), pThisArmy->GetGoalPlot()->getY());
			LogOperationSpecialMessage(szMsg);
		}
	}

	return pMusterPt;
}

/// Which unit would we like to use to kick off this operation?
CvUnit *CvAINavalOperation::FindInitialUnit()
{
	int iUnitLoop;
	CvUnit *pLoopUnit;

	for (pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
			{
				CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
				if(pkUnitEntry && pkUnitEntry->GetUnitAIType(UNITAI_ATTACK_SEA))
				{
					if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
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
void CvAIOperationNavalBombardment::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if (iID != -1 && GetOperationStartCity())
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if (pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			// Figure out the initial rally point
			CvPlot *pTargetPlot = FindBestTarget();
			if (pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if (SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if (GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
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

/// Read serialized data
void CvAIOperationNavalBombardment::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
}

/// Write serialized data
void CvAIOperationNavalBombardment::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// Same as default version except if just gathered forces, check to see if a better target has presented itself
bool CvAIOperationNavalBombardment::ArmyInPosition(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
		{
			// First do base case processing
			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

			// Now revisit target
			CvPlot *possibleBetterTarget;
			possibleBetterTarget = FindBestTarget();

			// If no target left, abort
			if (possibleBetterTarget == NULL)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_TARGET;
			}

			// If target changed, reset to this new one
			else if (possibleBetterTarget != GetTargetPlot())
			{
				// Reset our destination to be a few plots shy of the final target
				CvPlot *pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GetDeployRange());
				if (pDeployPt != NULL)
				{
					pArmy->SetGoalPlot(pDeployPt);
					SetTargetPlot(possibleBetterTarget);
				}
			}
		}
		break;

		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			if (pArmy->Plot() == pArmy->GetGoalPlot())
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
CvPlot *CvAIOperationNavalBombardment::FindBestTarget()
{
	int iPlotLoop, iDirectionLoop;
	CvPlot *pBestPlot = NULL;
	CvPlot *pPlot;
	CvPlot *pAdjacentPlot;
	int iBestTurns = MAX_INT;
	int iCurrentTurns;
	CvUnit *pInitialUnit;

	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);

	if (GetFirstArmyID() == -1)
	{
		pInitialUnit = FindInitialUnit();
	}
	else
	{
		CvArmyAI* pThisArmy = owningPlayer.getArmyAI(m_viArmyIDs[0]);
		int iUnitID = pThisArmy->GetFirstUnitID();
		if (iUnitID != -1)
		{
			pInitialUnit = owningPlayer.getUnit(iUnitID);
		}
		else
		{
			pInitialUnit = FindInitialUnit();
		}
	}

	if (pInitialUnit != NULL)
	{
		// Look at map for enemy units on the coast
		for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if (pPlot->isRevealed(owningPlayer.getTeam()))
			{
				if (pPlot->isCoastalLand())
				{
					// Enemy defender here? (for now let's not add cities; they fire back!)
					CvUnit *pUnit = pPlot->getVisibleEnemyDefender(m_eOwner);
					if (pUnit)
					{
						// Find an adjacent coastal water tile
						for (iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));
							if (pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
							{
								if (pInitialUnit->GeneratePath(pAdjacentPlot, 0, false, &iCurrentTurns))
								{
									if (iCurrentTurns < iBestTurns)
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
void CvAIOperationNavalSuperiority::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_FREEFORM_NAVAL;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if (iID != -1 && GetOperationStartCity())
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if (pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			// Figure out the initial rally point
			CvPlot *pTargetPlot = FindBestTarget();
			if (pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				if (SelectInitialMusterPoint(pArmyAI) != NULL)
				{
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetDefaultArea(GetMusterPlot()->getArea());

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if (GrabUnitsFromTheReserves(GetMusterPlot(), GetMusterPlot()))
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

/// Read serialized data
void CvAIOperationNavalSuperiority::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAINavalOperation::Read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
}

/// Write serialized data
void CvAIOperationNavalSuperiority::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// Same as default version except if just gathered forces and this operation never reaches a final target (just keeps attacking until dead or the operation is ended)
bool CvAIOperationNavalSuperiority::ArmyInPosition(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
		{
			// First do base case processing
			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

			// Now revisit target
			CvPlot *possibleBetterTarget;
			possibleBetterTarget = FindBestTarget();

			// If no target left, abort
			if (possibleBetterTarget == NULL)
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_TARGET;
			}

			// If target changed, reset to this new one
			else if (possibleBetterTarget != GetTargetPlot())
			{
				// Reset our destination to be a few plots shy of the final target
				CvPlot *pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GetDeployRange());
				if (pDeployPt != NULL)
				{
					pArmy->SetGoalPlot(pDeployPt);
					SetTargetPlot(possibleBetterTarget);
				}
			}
		}
		break;

		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		break;   // Never finish!

		// In all other cases use base class version
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_AT_TARGET:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Find the nearest enemy naval unit to eliminate
CvPlot *CvAIOperationNavalSuperiority::FindBestTarget()
{
	int iPlotLoop, iUnitLoop;
	CvPlot *pPlot;
	CvPlot *pBestPlot = NULL;
	int iBestTurns = MAX_INT;
	int iCurrentTurns;
	CvUnit *pInitialUnit;
	CvCity *pCity;
	CvCity *pEnemyCoastalCity = NULL;
	CvPlot *pCoastalBarbarianCamp = NULL;
	int iClosestEnemyDistance = MAX_INT;
	int iClosestCampDistance = MAX_INT;
	int iDistance;
	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);

	if (GetFirstArmyID() == -1)
	{
		pInitialUnit = FindInitialUnit();
	}
	else
	{
		CvArmyAI* pThisArmy = owningPlayer.getArmyAI(m_viArmyIDs[0]);
		int iUnitID = pThisArmy->GetFirstUnitID();
		if (iUnitID != -1)
		{
			pInitialUnit = owningPlayer.getUnit(iUnitID);
		}
		else
		{
			pInitialUnit = FindInitialUnit();
		}
	}

	if (pInitialUnit != NULL)
	{
		// Look at map for enemy naval units
		for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if (pPlot->isRevealed(owningPlayer.getTeam()))
			{
				if (pPlot->isWater())
				{
					if (pPlot->getNumUnits() > 0)
					{
						for (iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
						{
							CvUnit *pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
							{
								if (pLoopUnit->isEnemy(owningPlayer.getTeam()))
								{
									if (pInitialUnit->GeneratePath(pLoopUnit->plot(), 0, false, &iCurrentTurns))
									{
										if (iCurrentTurns < iBestTurns)
										{
											iBestTurns = iCurrentTurns;
											pBestPlot = pLoopUnit->plot();
										}
									}
								}
							}
						}
					}
				}

			    // Backup plan is a coastal enemy city
				else if (pPlot->isCity() && pPlot->isCoastalLand())
				{
					pCity = pPlot->getPlotCity();
					if (pCity)
					{
						if (atWar (owningPlayer.getTeam(), pCity->getTeam()))
						{
							iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pCity->getX(), pCity->getY());
							if (iDistance < iClosestEnemyDistance)
							{
								iClosestEnemyDistance = iDistance;
								pEnemyCoastalCity = pCity;
							}
						}
					}
				}

				else if (pPlot->isCoastalLand() && pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pPlot->getX(), pPlot->getY());
					if (iDistance < iClosestCampDistance)
					{
						iClosestCampDistance = iDistance;
						pCoastalBarbarianCamp = pPlot;
					}
				}
			}
		}

		// None found, patrol over near closest enemy coastal city, or if not that a water tile adjacent to a camp
		if (pBestPlot == NULL)
		{
			if (pEnemyCoastalCity != NULL)
			{
				// Find a coastal water tile adjacent to enemy city
				for (int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot *pAdjacentPlot = plotDirection(pEnemyCoastalCity->plot()->getX(), pEnemyCoastalCity->plot()->getY(), ((DirectionTypes)iDirectionLoop));
					if (pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
					{
						if (pInitialUnit->GeneratePath(pAdjacentPlot))
						{
							pBestPlot = pAdjacentPlot;
						}
					}
				}
			}

			else
			{
				if (pCoastalBarbarianCamp != NULL)
				{
					// Find a coastal water tile adjacent to camp
					for (int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot *pAdjacentPlot = plotDirection(pCoastalBarbarianCamp->getX(), pCoastalBarbarianCamp->getY(), ((DirectionTypes)iDirectionLoop));
						if (pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
						{
							if (pInitialUnit->GeneratePath(pAdjacentPlot))
							{
								pBestPlot = pAdjacentPlot;
							}
						}
					}
				}
			}
		}
	}

	return pBestPlot;
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
void CvAIOperationCityCloseDefense::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if (iID != -1 )
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
		if (pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			CvPlot *pTargetPlot = FindBestTarget();
			if (pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				SetMusterPlot(pTargetPlot);  // Gather directly at the point we're trying to defend
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());

				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
				if (GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
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
}

/// Write serialized data
void CvAIOperationCityCloseDefense::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// Find the best blocking position against the current threats
CvPlot *CvAIOperationCityCloseDefense::FindBestTarget()
{
	CvCity* pCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;

	// Defend the city most under threat
	pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if (pCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if (pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		pCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
	}

	if (pCity != NULL)
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
void CvAIOperationRapidResponse::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if (iID != -1 )
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
		if (pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			CvPlot *pTargetPlot = FindBestTarget();
			if (pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				SetMusterPlot(pTargetPlot);  // Gather directly at the point we're trying to defend
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());

				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
				if (GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
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
}

/// Write serialized data
void CvAIOperationRapidResponse::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// If have gathered forces, check to see what the best blocking position is.
bool CvAIOperationRapidResponse::ArmyInPosition(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
	{
		// See if reached our target
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			// For now never end, even at target
			bStateChanged = false;

			// ... but we might want to move to a greater threat
			RetargetDefensiveArmy(pArmy);
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
bool CvAIOperationRapidResponse::ArmyMoved(CvArmyAI *pArmy)
{
	bool bStateChanged = false;

	switch (m_eCurrentState)
	{
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			RetargetDefensiveArmy(pArmy);
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

/// Start the settler off to a new target plot
bool CvAIOperationRapidResponse::RetargetDefensiveArmy(CvArmyAI *pArmy)
{
	CvPlot *pBetterTarget;

	// Find most threatened city
	pBetterTarget = FindBestTarget();

	// No targets at all!  Abort
	if (pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}

	// If this is a new target, switch to it
	else if (pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
}

/// Find the best blocking position against the current threats
CvPlot *CvAIOperationRapidResponse::FindBestTarget()
{
	CvCity* pCity;
	CvPlot* pPlot = NULL;
	CvCity* pEnemyCapital;
	int iLoop;

	// Defend the city most under threat
	pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();

	// If no city is threatened just defend whichever of our cities is closest to the enemy capital
	if (pCity == NULL)
	{
		pEnemyCapital = GET_PLAYER(m_eEnemy).getCapitalCity();
		if (pEnemyCapital == NULL)
		{
			pEnemyCapital = GET_PLAYER(m_eEnemy).firstCity(&iLoop);
		}
		pCity = GC.getMap().findCity(pEnemyCapital->getX(), pEnemyCapital->getY(), m_eOwner, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, NULL);
	}

	if (pCity != NULL)
	{
		pPlot = pCity->plot();
	}

	return pPlot;
}

////////////////////////////////////////////////////////////////////////////////
// CvAINavalEscortedOperation
////////////////////////////////////////////////////////////////////////////////
CvAINavalEscortedOperation::CvAINavalEscortedOperation()
{
	// *** Move into subclass later?
	m_eCivilianType = UNITAI_SETTLE;
	m_iInitialAreaID = -1;
}

CvAINavalEscortedOperation::~CvAINavalEscortedOperation()
{
}

/// Kick off this operation
void CvAINavalEscortedOperation::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int iDefaultArea, CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	CvUnit *pOurCivilian;
	CvPlot *pTargetSite;
	CvCity *pStartCity;

	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	SetDefaultArea(iDefaultArea);   // Area settler starts in

	// Find the free civilian (that triggered this operation)
	pOurCivilian = FindBestCivilian();
	m_iInitialAreaID = pOurCivilian->getArea();
	pStartCity = GetOperationStartCity();

	if (pOurCivilian != NULL && iID != -1 && pStartCity)
	{
		// Find a destination (not worrying about safe paths)
		pTargetSite = FindBestTarget(pOurCivilian);

		if (pTargetSite != NULL)
		{
			SetTargetPlot(pTargetSite);

			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if (pArmyAI)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());

				pArmyAI->SetGoalPlot(pTargetSite);
				CvPlot *pMusterPt = pStartCity->plot();
				SetMusterPlot(pMusterPt);
				pArmyAI->SetXY(pMusterPt->getX(), pMusterPt->getY());

				// Add the settler to our army
				pArmyAI->AddUnit (pOurCivilian->GetID(), 0);

				// try to get the escort from existing units that are waiting around
				BuildListOfUnitsWeStillNeedToBuild();

				// Try to get as many units as possible from existing units that are waiting around
				if (GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
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

/// Find the port our operation will leave from
CvCity *CvAINavalEscortedOperation::GetOperationStartCity() const
{
	if (GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	int iLoop;
	CvCity *pCity;

	// Find first coastal city in same area as settler
	for (pCity = GET_PLAYER(m_eOwner).firstCity(&iLoop); pCity != NULL; pCity = GET_PLAYER(m_eOwner).nextCity(&iLoop))
	{
		if (pCity->isCoastal())
		{
			if (pCity->getArea() == m_iInitialAreaID)
			{
				return pCity;
			}
		}
	}

	return NULL;
}

/// Always abort if settler is removed
void CvAINavalEscortedOperation::UnitWasRemoved(int /*iArmyID*/, int iSlotID)
{
	// Assumes civilian is in the first slot of the formation
	if (iSlotID == 0)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_LOST_CIVILIAN;
	}
}

/// Find the civilian we want to use
CvUnit *CvAINavalEscortedOperation::FindBestCivilian()
{
	int iUnitLoop;
	CvUnit *pLoopUnit;

	for (pLoopUnit = GET_PLAYER(m_eOwner).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(m_eOwner).nextUnit(&iUnitLoop))
	{
		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->AI_getUnitAIType() == m_eCivilianType)
			{
				if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
				{
					//if (pLoopUnit->getArea() == GetDefaultArea())
					{
						return pLoopUnit;
					}
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

	kStream << m_eCivilianType;
}

/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAINavalEscortedOperation::ArmyInPosition(CvArmyAI *pArmy)
{
	int iUnitID = 0;
	bool bStateChanged = false;
	CvUnit *pSettler = 0, *pEscort = 0;
	CvString strMsg;

	switch (m_eCurrentState)
	{
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
	case AI_OPERATION_STATE_AT_TARGET:

		// Call base class version and see if it thinks we're done
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now get the settler
		iUnitID = pArmy->GetFirstUnitID();
		if (iUnitID != -1)
		{
			pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if (pSettler != NULL)
		{
			if ((GetTargetPlot()->getOwner() != NO_PLAYER && GetTargetPlot()->getOwner() != m_eOwner) || GetTargetPlot()->IsAdjacentOwnedByOtherTeam(pSettler->getTeam()))
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Not at target but can no longer settle here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if (iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
			// If the settler made it, we don't care about the entire army
			else if (pSettler->plot() == GetTargetPlot() && pSettler->canMove() && pSettler->canFound(pSettler->plot()))
			{
				pSettler->PushMission(CvTypes::getMISSION_FOUND());
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("City founded, At X=%d, At Y=%d", pSettler->plot()->getX(), pSettler->plot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}

			// If we're at our target but can no longer found a city, might be someone else beat us to this area
			// So move back out, picking a new target
			else if (pSettler->plot() == GetTargetPlot() && !pSettler->canFound(pSettler->plot()))
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("At target but can no longer settle here. Target was (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
					LogOperationSpecialMessage(strMsg);
				}
				RetargetCivilian(pSettler, pArmy);
				pSettler->finishMoves();
				iUnitID = pArmy->GetNextUnitID();
				if (iUnitID != -1)
				{
					pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pEscort->finishMoves();
				}
			}
		}
		break;

		// In all other cases use base class version
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
		return CvAIOperation::ArmyInPosition(pArmy);
		break;
	};

	return bStateChanged;
}

/// Find the plot where we want to settle
CvPlot *CvAINavalEscortedOperation::FindBestTarget(CvUnit *pUnit)
{
	return GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, true, -1);
}

/// Start the civilian off to a new target plot
bool CvAINavalEscortedOperation::RetargetCivilian(CvUnit *pCivilian, CvArmyAI *pArmy)
{
	CvPlot *pBetterTarget;

	// Find best city site (taking into account whether or not we are escorted)
	pBetterTarget = FindBestTarget(pCivilian);

	// No targets at all!  Abort
	if (pBetterTarget == NULL)
	{
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
		return false;
	}

	// If this is a new target, switch to it
	else if (pBetterTarget != GetTargetPlot())
	{
		SetTargetPlot(pBetterTarget);
		pArmy->SetGoalPlot(pBetterTarget);
	}

	pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
	m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;

	return true;
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
void CvAIOperationNavalAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea, CvCity *pTarget, CvCity *pMuster)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	SetDefaultArea(iDefaultArea);

	SetStartCityPlot(pMuster->plot());

	if (iID != -1)
	{
		if (pTarget)
		{
			SetTargetPlot(pTarget->plot());

			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if (pArmyAI)
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
				if (GrabUnitsFromTheReserves(GetMusterPlot(), NULL))
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
}

/// Write serialized data
void CvAIOperationNavalAttack::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAINavalEscortedOperation::Write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAIOperationNavalAttack::ArmyInPosition(CvArmyAI *pArmy)
{
	switch (m_eCurrentState)
	{
		// See if reached our target, if so give control of these units to the tactical AI
	case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			// Are we within tactical range of our target?
			if (plotDistance (pArmy->GetX(), pArmy->GetY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
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
CvCity *CvAIOperationNavalAttack::GetOperationStartCity() const
{
	if (GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCity(m_eEnemy);
}

/// Find the city we want to attack
CvPlot *CvAIOperationNavalAttack::FindBestTarget()
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

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalCityStateAttack
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNavalCityStateAttack::CvAIOperationNavalCityStateAttack()
{
}


CvAIOperationNukeAttack::CvAIOperationNukeAttack()
{
	m_iBestUnitID = -1;
}

CvAIOperationNukeAttack::~CvAIOperationNukeAttack()
{
}

void CvAIOperationNukeAttack::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int , CvCity * /*pTarget*/, CvCity * /*pMuster*/)
{
	Reset();
	m_eMoveType = AI_OPERATION_MOVETYPE_STATIC; // this operation will only work when the units are already in place
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;

	if (iID != -1 )
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
		CvArmyAI* pArmyAI = kPlayer.addArmyAI();
		if (pArmyAI)
		{
			m_viArmyIDs.push_back(pArmyAI->GetID());
			pArmyAI->Init(pArmyAI->GetID(), m_eOwner, m_iID);
			pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
			pArmyAI->SetFormationIndex(GetFormation());

			// Figure out the target spot
			CvPlot *pTargetPlot = FindBestTarget();
			if (pTargetPlot != NULL)
			{
				SetTargetPlot(pTargetPlot);
				pArmyAI->SetGoalPlot(pTargetPlot);
				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();
				// try to get as many units as possible from existing units that are waiting around
				if (GrabUnitsFromTheReserves(GC.getMap().plotCheckInvalid(m_iMusterX, m_iMusterY), pTargetPlot))
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

	kStream << m_iBestUnitID;
}

CvCity *CvAIOperationNukeAttack::GetOperationStartCity() const
{
	if (GetStartCityPlot())
	{
		return GetStartCityPlot()->getPlotCity();
	}

	return GC.getMap().plotCheckInvalid(m_iMusterX, m_iMusterY)->getPlotCity();
}

bool CvAIOperationNukeAttack::ArmyInPosition(CvArmyAI *pArmy)
{
	CvPlot* pTargetPlot = GetTargetPlot();
	if (pTargetPlot)
	{
		// Now get the nuke
		int iUnitID = pArmy->GetFirstUnitID();
		CvUnit* pNuke = NULL;
		if (iUnitID != -1)
		{
			pNuke = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		}

		if (pNuke != NULL)
		{
			if (pNuke->canMove() && pNuke->canNukeAt(pNuke->plot(),pTargetPlot->getX(),pTargetPlot->getY()))
			{
				pNuke->PushMission(CvTypes::getMISSION_NUKE(), pTargetPlot->getX(), pTargetPlot->getY());
				if (GC.getLogging() && GC.getAILogging())
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
	for (pLoopUnit = ownerPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iUnitLoop))
	{
		if (pLoopUnit && pLoopUnit->canNuke(NULL))
		{
			int iUnitRange = pLoopUnit->GetRange();
			// for all cities of this enemy
			CvCity* pLoopCity;
			for (pLoopCity = enemyPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = enemyPlayer.nextCity(&iCityLoop))
			{
				if (pLoopCity)
				{
					if (plotDistance(pLoopUnit->getX(),pLoopUnit->getY(),pLoopCity->getX(),pLoopCity->getY()) <= iUnitRange)
					{
						CvPlot* pCityPlot = pLoopCity->plot();
						int iThisCityValue = pLoopCity->getPopulation();
						iThisCityValue -= pLoopCity->getDamage() / 5; // No point nuking a city that is already trashed unless it is good city

						// check to see if there is anything good or bad in the radius that we should account for

						for (int iDX = -iBlastRadius; iDX <= iBlastRadius; iDX++)
						{
							for (int iDY = -iBlastRadius; iDY <= iBlastRadius; iDY++)
							{
								CvPlot* pLoopPlot = plotXYWithRangeCheck(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY, iBlastRadius);
								if (pLoopPlot)
								{
									// who owns this plot?
									PlayerTypes ePlotOwner = pLoopPlot->getOwner();
									TeamTypes ePlotTeam = pLoopPlot->getTeam();
									// are we at war with them (or are they us)
									if (ePlotOwner == m_eOwner)
									{
										iThisCityValue -= 1;
										if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
										{
											if (!pLoopPlot->IsImprovementPillaged())
											{
												iThisCityValue -= 5;
												if (pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE) // we aren't nuking our own resources
												{
													iThisCityValue -= 1000;
												}
											}
										}
									}
									else if (ePlotTeam != NO_TEAM && ourTeam.isAtWar(ePlotTeam))
									{
										iThisCityValue += 1;
										if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
										{
											if (!pLoopPlot->IsImprovementPillaged())
											{
												iThisCityValue += 2;
												if (pLoopPlot->getResourceType(ePlotTeam) != NO_RESOURCE) // we like nuking our their resources
												{
													iThisCityValue += 5;
												}
											}
										}
									}
									else // this will trigger a war
									{
										iThisCityValue -= 1000;
									}

									// will we hit any units here?

									// Do we want a visibility check here?  We shouldn't know they are here.

									const IDInfo* pUnitNode = pLoopPlot->headUnitNode();
									const CvUnit* pInnerLoopUnit;
									while (pUnitNode != NULL)
									{
										pInnerLoopUnit = ::getUnit(*pUnitNode);
										pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
										if (pInnerLoopUnit != NULL)
										{
											PlayerTypes eUnitOwner = pInnerLoopUnit->getOwner();
											TeamTypes eUnitTeam = pInnerLoopUnit->getTeam();
											// are we at war with them (or are they us)
											if (eUnitOwner == m_eOwner)
											{
												iThisCityValue -= 2;
											}
											else if (ourTeam.isAtWar(eUnitTeam))
											{
												iThisCityValue += 2;
											}
											else // this will trigger a war
											{
												iThisCityValue -= 1000;
											}
										}
									}
								}
							}
						}

						// if this is the capital
						if (pLoopCity->isCapital())
						{
							iThisCityValue *= 2;
						}

						if (iThisCityValue > iBestCity)
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

	if (pBestCity && pBestUnit)
	{
		m_iMusterX = pBestUnit->getX();
		m_iMusterY = pBestUnit->getY();
		m_iBestUnitID = pBestUnit->GetID();
		return pBestCity->plot();
	}

	return NULL;
}

/// Find a unit from our reserves that could serve in this operation
bool CvAIOperationNukeAttack::FindBestFitReserveUnit (OperationSlot thisOperationSlot, CvPlot* /*pMusterPlot*/, CvPlot* /*pTargetPlot*/, bool* bRequired)
{

	// okay, this can be simplified to
	*bRequired = true;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	pThisArmy->AddUnit(m_iBestUnitID, thisOperationSlot.m_iSlotID);
	return true;

	//CvUnit* pLoopUnit;
	//CvUnit* pBestUnit = NULL;
	//int iPathDistance;
	//int iBestDistance = MAX_INT;
	//int iLoop = 0;
	//CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	//CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	//CvString strMsg;

	//*bRequired = true;

	//int iThisFormationIndex = pThisArmy->GetFormationIndex();
	//if (iThisFormationIndex != NO_MUFORMATION)
	//{
	//	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iThisFormationIndex);
	//	if(thisFormation)
	//	{
	//		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry (thisOperationSlot.m_iSlotID);

	//		for (pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
	//		{
	//			// Make sure he's not needed by the tactical AI or already in an army or scouting
	//			if (pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
	//			{
	//				// Is this unit one of the requested types?
	//				CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
	//				if (unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType) ||
	//					unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_secondaryUnitType))
	//				{
	//					// Not finishing up an operation?
	//					if (pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
	//					{
	//						// If happens to be at muster point, just return this unit
	//						if (pLoopUnit->GetID() = m_iBestUnitID && pLoopUnit->plot() == pMusterPlot)
	//						{
	//							pThisArmy->AddUnit(pLoopUnit->GetID(), thisOperationSlot.m_iSlotID);
	//							return true;
	//						}
	//					}
	//				}
	//			}
	//		}

	//		// If not required, let our calling routine know that
	//		if (!thisSlotEntry.m_requiredSlot)
	//		{
	//			*bRequired = false;
	//		}
	//	}
	//}

	//return false;
}










FDataStream & operator<<(FDataStream & saveTo, const AIOperationState & readFrom)
{
	int v = static_cast<int>(readFrom);
	saveTo << v;
	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, AIOperationState & writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<AIOperationState>(v);
	return loadFrom;
}

FDataStream & operator<<(FDataStream & saveTo, const OperationSlot & readFrom)
{
	saveTo << readFrom.m_iOperationID;
	saveTo << readFrom.m_iArmyID;
	saveTo << readFrom.m_iSlotID;
	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, OperationSlot & writeTo)
{
	loadFrom >> writeTo.m_iOperationID;
	loadFrom >> writeTo.m_iArmyID;
	loadFrom >> writeTo.m_iSlotID;
	return loadFrom;
}

int OperationalAIHelpers::GetGatherRangeForXUnits(int iTotalUnits)
{
	int iRange = 0;

	if (iTotalUnits <= 2)
	{
		iRange = 1;
	}
	else if (iTotalUnits <= 6)
	{
		iRange = 2;
	}
	else
	{
		iRange = 3;
	}

	return iRange;
}