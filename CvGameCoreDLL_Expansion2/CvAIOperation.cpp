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
	m_bShouldReplaceLossesWithReinforcements = false;

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
	m_iStartCityX = INVALID_PLOT_COORD;
	m_iStartCityY = INVALID_PLOT_COORD;
	m_eMoveType = INVALID_AI_OPERATION_MOVE_TYPE;
	m_iLastTurnMoved = -1;
	m_viArmyIDs.clear();
#if defined(MOD_BALANCE_CORE)
	m_iTurnStarted = -1;
#endif
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
	case AI_OPERATION_NAVAL_COLONIZATION:
		return FNEW(CvAIOperationNavalColonization(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_QUICK_COLONIZE:
		return FNEW(CvAIOperationQuickColonize(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_PILLAGE_ENEMY:
		return FNEW(CvAIOperationPillageEnemy(), c_eCiv5GameplayDLL, 0);
	case AI_OPERATION_CITY_CLOSE_DEFENSE:
		return FNEW(CvAIOperationCityCloseDefense(), c_eCiv5GameplayDLL, 0);
#if defined(MOD_BALANCE_CORE)
	case AI_OPERATION_CITY_CLOSE_DEFENSE_PEACE:
		return FNEW(CvAIOperationCityCloseDefensePeace(), c_eCiv5GameplayDLL, 0);
#endif
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
	if(iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
	{
		thisSlot = m_viListOfUnitsWeStillNeedToBuild.front();
	}
	return thisSlot;
}

/// Called by a city when it decides to build a unit
OperationSlot CvAIOperation::CommitToBuildNextUnit(int iAreaID, int iTurns, CvCity* pCity)
{
	OperationSlot thisSlot;
	if(iAreaID == m_iDefaultArea && !m_viListOfUnitsWeStillNeedToBuild.empty())
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
bool CvAIOperation::RecruitUnit(CvUnit* pUnit)
{
	if(!pUnit || GetFirstArmyID() == -1)
		return false;

	CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(GetFirstArmyID());

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
	bool bMustNaval = (IsMixedLandNavalOperation() || IsAllNavalOperation());
	if(bMustNaval)
	{
		bMustBeDeepWaterNaval = OperationalAIHelpers::NeedOceanMoves(pMusterPlot, pTargetPlot);
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
						int iMusterDist = plotDistance(pMusterPlot->getX(), pMusterPlot->getY(), pUnit->getX(), pUnit->getY());
						if (iMusterDist > 12)
						{
							strMsg.Format("Warning: %s recruited far-away unit %d plots from muster point for a new army", GetOperationName().c_str(), iMusterDist);
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
#endif
bool CvAIOperation::GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot)
{
	bool rtnValue = true;
	bool success;
	std::deque<OperationSlot>::iterator it;

	// Copy over the list
	std::deque<OperationSlot> secondList = m_viListOfUnitsWeStillNeedToBuild;

	// Clear main list
	m_viListOfUnitsWeStillNeedToBuild.clear();

#if defined(MOD_BALANCE_CORE)
	CvString strMsg;
	bool bMustNaval = (IsMixedLandNavalOperation() || IsAllNavalOperation());
	if(bMustNaval)
	{
		bool bMustBeDeepWaterNaval = OperationalAIHelpers::NeedOceanMoves(pMusterPlot, pTargetPlot);
		CvPlot* pNavalMuster = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, NULL);
		CvPlot* pNavalTarget = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetPlot, NULL);

		if(GC.getLogging() && GC.getAILogging())
		{
			if(pNavalMuster != NULL && pNavalTarget != NULL)
			{
				strMsg.Format("Naval Operation targeting x=%d y=%d, target of x=%d y=%d.", pNavalMuster->getX(), pNavalMuster->getY(), pNavalTarget->getX(), pNavalTarget->getY());
				LogOperationSpecialMessage(strMsg);
			}
		}
		if(pNavalMuster == NULL || pNavalTarget == NULL)
		{
			strMsg.Format("Naval Operation failed to find good plots!");
			LogOperationSpecialMessage(strMsg);
			return false;
		}

		if (GC.GetInternationalTradeRouteWaterFinder().GeneratePath(pNavalMuster->getX(), pNavalMuster->getY(), pNavalTarget->getX(), pNavalTarget->getY(), true))
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

					UnitChoices.push_back(pLoopUnit->GetID(), 1000-iDistance);
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

				UnitChoices.push_back(pLoopUnit->GetID(), 1000-iDistance);
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
#else
	bool bRequired;
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
#endif
	return rtnValue;
}

#if defined(MOD_BALANCE_CORE)
//see if the target is still current or if there is a better one
bool CvAIOperation::VerifyTarget()
{
	bool retval = true;
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
		if (pThisArmy)
			retval = (retval && VerifyTarget(pThisArmy));
	}
	return retval;
}
#endif

CvPlot* CvAIOperation::GetPlotXInStepPath(CvArmyAI* pArmy, CvPlot* pCurrentPosition, CvPlot* pTarget, int iStep, bool bForward) const
{
	if (!pArmy || !pCurrentPosition || !pTarget || iStep<0)
		return NULL;

	// prepare the stepfinder
	int iNumUnits = pArmy->GetNumSlotsFilled();
	GC.getStepFinder().SetData(&m_eEnemy);
	GC.getStepFinder().SetDestValidFunc(NULL);
	GC.getStepFinder().SetValidFunc(iNumUnits>4 ? StepValidWideAnyArea : StepValidAnyArea);

	// use the step path finder to compute distance
	bool bFound = GC.getStepFinder().GeneratePath(pCurrentPosition->getX(), pCurrentPosition->getY(), pTarget->getX(), pTarget->getY(), m_eOwner, false);

	// restore the stepfinder
	GC.getStepFinder().SetValidFunc(StepValid);
	GC.getStepFinder().SetDestValidFunc(StepDestValid);

	if (!bFound)
		return NULL;
	
	SPath path = GC.getStepFinder().GetPath();
	if (path.vPlots.empty())
		return NULL;

	int iNodeIndex;
	if (bForward)
		iNodeIndex = std::min( (int)path.vPlots.size()-1, iStep );
	else
		iNodeIndex = std::max( 0, (int)path.vPlots.size()-1-iStep );
					
	return GC.getMap().plotCheckInvalid( path.vPlots[iNodeIndex].first,path.vPlots[iNodeIndex].second );
}

int CvAIOperation::GetStepDistanceBetweenPlots(CvArmyAI* pArmy, CvPlot* pCurrentPosition, CvPlot* pTarget) const
{
	if (!pArmy || !pCurrentPosition || !pTarget)
		return -1;

	// prepare the stepfinder
	int iNumUnits = pArmy->GetNumSlotsFilled();
	GC.getStepFinder().SetData(&m_eEnemy);
	GC.getStepFinder().SetDestValidFunc(NULL);
	GC.getStepFinder().SetValidFunc(iNumUnits>4 ? StepValidWideAnyArea : StepValidAnyArea);

	// use the step path finder to compute distance
	int iDistance = GC.getStepFinder().GetStepDistanceBetweenPoints(m_eOwner, m_eEnemy, pCurrentPosition, pTarget);

	// restore the stepfinder
	GC.getStepFinder().SetValidFunc(StepValid);
	GC.getStepFinder().SetDestValidFunc(StepDestValid);

	return iDistance;
}

/// See if armies are ready to hand off units to the tactical AI (and do so if ready)
bool CvAIOperation::CheckOnTarget()
{
	int iUnitID;
#if defined(MOD_BALANCE_CORE)
	CvUnit* pCivilian = NULL;
	CvUnit* pEscort = NULL;
	CvPlot* pCivilianPlot = NULL;

	if(GetFirstArmyID() == -1)
	{
		return false;
	}

	switch(m_eMoveType)
	{
		case AI_OPERATION_MOVETYPE_SINGLE_HEX:
		{
			// Let each army perform its own check
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				if(pThisArmy)
				{
					CvAIOperation* pAIOp = GET_PLAYER(m_eOwner).getAIOperation(pThisArmy->GetOperationID());
					//No operation? Bwa?
					if(!pAIOp)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					//No units ? Destroy.
					if(pThisArmy->GetNumSlotsFilled() <= 0)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					switch(m_eCurrentState)
					{
						case AI_OPERATION_STATE_RECRUITING_UNITS:
						{
							if(GetMusterPlot() != NULL && GetTargetPlot() != NULL && GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
							{
								pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
								m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
							}
							iUnitID = pThisArmy->GetFirstUnitID();
							if(iUnitID != -1)
							{
								pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
								if(pCivilian != NULL)
								{
									pCivilianPlot = pCivilian->plot();
									pEscort = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID());
									if (pCivilianPlot != NULL && pEscort && pEscort->plot() != NULL && pCivilianPlot == pEscort->plot())
									{
										pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
										m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
									}
									if (pCivilian && (GetMusterPlot()->getOwner() != m_eOwner || GC.getGame().getGameTurn() - GetTurnStarted() > 10)) // 10 turns and still no escort?
									{
										pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
										m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
									}
								}
							}
							break;
						}
						case AI_OPERATION_STATE_GATHERING_FORCES:
						{
							if(pThisArmy->GetNumSlotsFilled() == 1)
							{
								pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
								m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
							}
							else
							{
								iUnitID = pThisArmy->GetFirstUnitID();
								if(iUnitID != -1)
								{
									pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
									if(pCivilian != NULL)
									{
										pCivilianPlot = pCivilian->plot();
										pEscort = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID());
										if (pCivilianPlot != NULL && pEscort && pEscort->plot() != NULL && pCivilianPlot == pEscort->plot())
										{
											pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
											m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
										}
									}
								}
							}
							break;
						}
						case AI_OPERATION_STATE_MOVING_TO_TARGET:
						case AI_OPERATION_STATE_AT_TARGET:
						{
							iUnitID = pThisArmy->GetFirstUnitID();
							if(iUnitID != -1)
							{
								pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
								if(pCivilian != NULL)
								{
									pCivilianPlot = pCivilian->plot();
								}
							}
							if(pCivilianPlot != NULL && pCivilianPlot == GetTargetPlot())
							{
								pAIOp->ArmyInPosition(pThisArmy);
								return true;
							}
							break;
						}
					}
				}
			}
			break;
		}
		case AI_OPERATION_MOVETYPE_ENEMY_TERRITORY:
		{
			// Let each army perform its own check
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				CvPlot* pCenterOfMass;

				if(pThisArmy)
				{
					CvAIOperation* pAIOp = GET_PLAYER(m_eOwner).getAIOperation(pThisArmy->GetOperationID());
					//No operation? Bwa?
					if(!pAIOp)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					//No units ? Destroy.
					if(pThisArmy->GetNumSlotsFilled() <= 0)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					switch(m_eCurrentState)
					{
						case AI_OPERATION_STATE_RECRUITING_UNITS:
						{
							if(GetMusterPlot() != NULL && GetTargetPlot() != NULL && GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
							{
								pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
								m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
							}
							if (pThisArmy->GetNumSlotsFilled() > 0 && pThisArmy->GetTurnAtNextCheckpoint() > ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
							{
								pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
								m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
							}
							break;
						}
						case AI_OPERATION_STATE_GATHERING_FORCES:
						{
							int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
							pCenterOfMass = pThisArmy->GetCenterOfMass((IsAllNavalOperation() || IsMixedLandNavalOperation()) ? DOMAIN_SEA : DOMAIN_LAND);
							if(pCenterOfMass && GetMusterPlot() != NULL &&
								plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
								pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
							{
								pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
								m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
							}
							break;
						}
						case AI_OPERATION_STATE_MOVING_TO_TARGET:
						{
							int iTargetTolerance = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
							pCenterOfMass = pThisArmy->GetCenterOfMass((IsAllNavalOperation() || IsMixedLandNavalOperation()) ? DOMAIN_SEA : DOMAIN_LAND);
							if(pCenterOfMass && GetTargetPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= iTargetTolerance
							&& pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3 / 2))
							{
								pAIOp->ArmyInPosition(pThisArmy);
								return true;
							}
							break;
						}
					}
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
				CvPlot* pCenterOfMass;
				bool bCivilian = false;
				if(pThisArmy)
				{
					CvAIOperation* pAIOp = GET_PLAYER(m_eOwner).getAIOperation(pThisArmy->GetOperationID());
					//No operation? Bwa?
					if(!pAIOp)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					//No units ? Destroy.
					if(pThisArmy->GetNumSlotsFilled() <= 0)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					iUnitID = pThisArmy->GetFirstUnitID();
					if(iUnitID != -1)
					{
						pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
						if(pCivilian && pCivilian->isFound())
						{
							bCivilian = true;
						}
					}
					switch(m_eCurrentState)
					{
						case AI_OPERATION_STATE_RECRUITING_UNITS:
						{					
							if(bCivilian)
							{
								if (pThisArmy->GetNumSlotsFilled() > 0 && pThisArmy->GetTurnAtNextCheckpoint() != ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT && pThisArmy->GetTurnAtNextCheckpoint() != ARMYSLOT_NOT_INCLUDING_IN_OPERATION)
								{
									pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
									m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
								}
							}
							else
							{
								if(GetMusterPlot() != NULL && GetTargetPlot() != NULL && GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
								{
									pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
									m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
								}
								if (pThisArmy->GetNumSlotsFilled() > 0 && pThisArmy->GetTurnAtNextCheckpoint() != ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT && pThisArmy->GetTurnAtNextCheckpoint() != ARMYSLOT_NOT_INCLUDING_IN_OPERATION)
								{
									pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
									m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
								}
							}
							break;
						}
						case AI_OPERATION_STATE_GATHERING_FORCES:
						{
							if(bCivilian)
							{
								if(pCivilian)
								{
									pCivilianPlot = pCivilian->plot();
									if(pCivilianPlot != NULL)
									{
										//there may be multiple escorts ...
										for (int iSlot=1; iSlot<pThisArmy->GetNumSlotsFilled(); iSlot++)
										{
											pEscort = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetFormationSlot(iSlot)->GetUnitID());
											if (pEscort && pCivilianPlot == pEscort->plot())
											{
												int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
												pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
												if(pCenterOfMass && GetMusterPlot() &&
													plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
													pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 2))
												{
													pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
													m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
												}
											}
										}
									}
								}
							}
							else
							{
								int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
								pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
								if(pCenterOfMass && GetMusterPlot() &&
									plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
									pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
								{
									pThisArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
									m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
								}
							}
							break;
						}
						case AI_OPERATION_STATE_MOVING_TO_TARGET:
						{
							if(bCivilian)
							{
								if(pCivilian)
								{
									pCivilianPlot = pCivilian->plot();
									if(pCivilianPlot != NULL)
									{
										if(pCivilianPlot != NULL && pCivilianPlot == GetTargetPlot())
										{
											pAIOp->ArmyInPosition(pThisArmy);
											return true;
										}
									}
								}
							}
							else
							{
								int iTargetTolerance = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
								pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
								if(pCenterOfMass && GetTargetPlot() != NULL &&
									plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= iTargetTolerance &&
									pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3 / 2))
								{
									pAIOp->ArmyInPosition(pThisArmy);
									return true;
								}
							}
							break;
						}
					}
				}
			}
			break;
		}

		case AI_OPERATION_MOVETYPE_FREEFORM_NAVAL:
#if defined(MOD_BALANCE_CORE)
		case AI_OPERATION_MOVETYPE_STATIC:
#endif
		{
			// Let each army perform its own check
			for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
			{
				CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
				CvPlot* pCenterOfMass;
				int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());

				if(pThisArmy)
				{
					CvAIOperation* pAIOp = GET_PLAYER(m_eOwner).getAIOperation(pThisArmy->GetOperationID());
					//No operation? Bwa?
					if(!pAIOp)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					//No units ? Destroy.
					if(pThisArmy->GetNumSlotsFilled() <= 0)
					{
						Kill(AI_ABORT_NO_UNITS);
						return false;
					}
					switch(m_eCurrentState)
					{
						case AI_OPERATION_STATE_RECRUITING_UNITS:
						{
							if(GetMusterPlot() != NULL && GetTargetPlot() != NULL && GrabUnitsFromTheReserves(GetMusterPlot(), GetTargetPlot()))
							{
								pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
								m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
							}
							if (pThisArmy->GetNumSlotsFilled() > 0 && pThisArmy->GetTurnAtNextCheckpoint() != ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT && pThisArmy->GetTurnAtNextCheckpoint() != ARMYSLOT_NOT_INCLUDING_IN_OPERATION)
							{
								pThisArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
								m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
							}
							break;
						}
						case AI_OPERATION_STATE_GATHERING_FORCES:
						case AI_OPERATION_STATE_MOVING_TO_TARGET:
						{
							// We want to recompute a new target each turn.  So call ArmyInPosition() regardless of return status
							pAIOp->ArmyInPosition(pThisArmy);

							pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
							if(pCenterOfMass && GetTargetPlot() != NULL &&
									plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= iGatherTolerance &&
									pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iGatherTolerance * 3 / 2))
									{
										return true;
									}
							break;
						}
					}
				}
			}
			break;
		}
	}
#else
	CvUnit* pCivilian;
	CvPlot* pCivilianPlot = NULL;
	CvPlot* pEscortPlot;


	if(GetFirstArmyID() == -1)
	{
		return false;
	}

	switch(m_eMoveType)
	{
		{
		case AI_OPERATION_MOVETYPE_SINGLE_HEX:
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[0]);
			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				iUnitID = pThisArmy->GetFirstUnitID();
				if(iUnitID != -1)
				{
					pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
					pCivilianPlot = pCivilian->plot();
				}
				if( m_eCurrentState == AI_OPERATION_STATE_MOVING_TO_TARGET && pCivilianPlot == GetTargetPlot())
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
						pEscortPlot = GET_PLAYER(m_eOwner).getUnit(pThisArmy->GetNextUnitID())->plot();
						if(pCivilianPlot == pEscortPlot)
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
		for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
		{
			CvArmyAI* pThisArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
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
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= iTargetTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3 / 2))
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
			CvPlot* pCenterOfMass;

			if(pThisArmy->GetNumSlotsFilled() >= 1)
			{
				switch(m_eCurrentState)
				{
				case AI_OPERATION_STATE_GATHERING_FORCES:
					{
						int iGatherTolerance = GetGatherTolerance(pThisArmy, GetMusterPlot());
						pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
						if(pCenterOfMass && GetMusterPlot() &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetMusterPlot()->getX(), GetMusterPlot()->getY()) <= iGatherTolerance &&
							pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3))
						{
							ArmyInPosition(pThisArmy);
							return true;
						}
					}
					break;
				case AI_OPERATION_STATE_MOVING_TO_TARGET:
					{
						int iTargetTolerance = GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE();
						pCenterOfMass = pThisArmy->GetCenterOfMass(DOMAIN_SEA);
						if(pCenterOfMass &&
							plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= (iTargetTolerance  * 2) &&
							pThisArmy->GetFurthestUnitDistance(GetTargetPlot()) <= (iTargetTolerance * 3))
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
					if(pCenterOfMass &&
					        plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= iGatherTolerance &&
					        pThisArmy->GetFurthestUnitDistance(GetMusterPlot()) <= (iGatherTolerance * 3 / 2))
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
#endif
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
				CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(IsAllNavalOperation() || IsMixedLandNavalOperation() ? DOMAIN_SEA : DOMAIN_LAND);
				int iDistanceMusterToTarget = GetStepDistanceBetweenPlots( pArmy, GetMusterPlot(), pArmy->GetGoalPlot() );
				int iDistanceCurrentToTarget = GetStepDistanceBetweenPlots( pArmy, pCenterOfMass, pArmy->GetGoalPlot() );

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

#if defined(MOD_BALANCE_CORE)
	for(unsigned int uiI = 0; uiI < m_viArmyIDs.size(); uiI++)
	{
		CvArmyAI* pArmy = GET_PLAYER(m_eOwner).getArmyAI(m_viArmyIDs[uiI]);
		if(pArmy)
		{
			if(m_eCurrentState == AI_OPERATION_STATE_RECRUITING_UNITS && pArmy->GetTurnAtNextCheckpoint() == ARMYSLOT_UNKNOWN_TURN_AT_CHECKPOINT)
			{
				int iTurns = GC.getGame().getGameTurn() - GetTurnStarted();
				if (iTurns > (GetMaximumRecruitTurns() * 2))
				{
					SetToAbort(AI_ABORT_TIMED_OUT);
				}
			}
		}
	}
#endif

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
#if defined(MOD_BALANCE_CORE)
			if (!pGoalPlot->isWater() && (IsAllNavalOperation() || IsMixedLandNavalOperation()))
#else
			if (!pGoalPlot->isWater() && IsAllNavalOperation())
#endif
			{
				pGoalPlot = kPlayer.GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pGoalPlot, pArmy);
			}

			CvPlot* pCenterOfMass = pArmy->GetCenterOfMass((IsAllNavalOperation() || IsMixedLandNavalOperation()) ? DOMAIN_SEA : DOMAIN_LAND);
			if (pCenterOfMass && pGoalPlot)
			{
				//update the current position
				pArmy->SetXY(pCenterOfMass->getX(), pCenterOfMass->getY());

				//get where we want to be next
				pRtnValue = GetPlotXInStepPath(pArmy,pCenterOfMass,pGoalPlot,pArmy->GetMovementRate(),true);
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
	CvCity* pCity = GetOperationStartCity();

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
#if defined(MOD_BALANCE_CORE)
	kStream >> m_iTurnStarted;
#endif
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
#if defined(MOD_BALANCE_CORE)
	kStream << m_iTurnStarted;
#endif
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
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName().c_str(), GetID());

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
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName().c_str(), GetID());

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
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName().c_str(), GetID());
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
		strBaseString.Format("%03d, %s, %s, %d, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str(), GetOperationName().c_str(), GetID() );

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
			strOutBuf = GetOperationName() + ", ";
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
#if !defined(MOD_BALANCE_CORE)
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
#endif

#if defined(MOD_BALANCE_CORE)
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
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString str1, str2;
			getUnitAIString(str1,thisSlotEntry.m_primaryUnitType);
			getUnitAIString(str2,thisSlotEntry.m_secondaryUnitType);
			strMsg.Format("Recruiting - trying to fill slot %d (%s / %s) in operation %d, army %d", 
				thisOperationSlot.m_iSlotID, str1.c_str(), str2.c_str(), thisOperationSlot.m_iOperationID, thisOperationSlot.m_iArmyID);
			LogOperationSpecialMessage(strMsg);
		}
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
				if (GC.getLogging() && GC.getAILogging())
				{
					strMsg.Format("Recruiting - found suitable unit %s %d at index %d", pBestUnit->getName().GetCString(), pBestUnit->GetID(), iI );
					LogOperationSpecialMessage(strMsg);
				}

				//overwrite with invalid ID so we don't use it a second time
				UnitChoices.SetElement(iI,-1);

				return true;
			}
		}
	}

	return false;
#else
bool CvAIOperation::FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool* bRequired)
{
	CvUnit* pBestUnit = NULL;
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	CvString strMsg;
	CvPlot *pTargetToUse = pTargetPlot;

	CvOperationSearchUnitList kSearchList;

	*bRequired = true;

	// All naval operation targeting a city?   Change target
	if (pTargetToUse && !pTargetToUse->isWater() && IsAllNavalOperation())
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

				// Make sure he's not needed by the tactical AI or already in an army or scouting
				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1 &&
					eLoopUnitAIType != UNITAI_EXPLORE && eLoopUnitAIType != UNITAI_EXPLORE_SEA && pLoopUnit->getDropRange() == 0 /* no paratroopers */)
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
						continue;

					// PRIMARY UNIT TYPE (ONLY)

					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_primaryUnitType))
					{
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
											if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea())
											{
												iDistance *= 2;
											}
										}
										else
										{
											if (pTargetToUse != NULL)
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pTargetToUse->getX(), pTargetToUse->getY());
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

				// Make sure he's not needed by the tactical AI or already in an army or scouting
				if(pLoopUnit->canRecruitFromTacticalAI() && pLoopUnit->getArmyID() == -1 &&
				        eLoopUnitAIType != UNITAI_EXPLORE && eLoopUnitAIType != UNITAI_EXPLORE_SEA && pLoopUnit->getDropRange() == 0 /* no paratroopers */)
				{
					// Is this unit one of the requested types?
					CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
					if(unitInfo == NULL)
						continue;

					// SECONDARY UNIT TYPE (ONLY)

					if(unitInfo->GetUnitAIType((UnitAITypes)thisSlotEntry.m_secondaryUnitType))
					{
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
											if(pMusterPlot != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pkLoopUnitPlot->getArea() != pMusterPlot->getArea())
											{
												iDistance *= 2;
											}
										}
										else
										{
											if (pTargetToUse != NULL)
												iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pTargetToUse->getX(), pTargetToUse->getY());
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
// CvAIOperationEnemyTerritory
////////////////////////////////////////////////////////////////////////////////
CvAIOperationEnemyTerritory::CvAIOperationEnemyTerritory()
{
}

CvAIOperationEnemyTerritory::~CvAIOperationEnemyTerritory()
{
}

/// How long will we wait for a recruit to show up?
int CvAIOperationEnemyTerritory::GetMaximumRecruitTurns() const
{
	return GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY();
}

/// Kick off this operation
void CvAIOperationEnemyTerritory::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /* iDefaultArea */, CvCity*, CvCity*)
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
#if defined(MOD_BALANCE_CORE)
					SetTurnStarted(GC.getGame().getGameTurn());
#endif

					CvPlot* pDeployPlot = GetPlotXInStepPath(pArmyAI,GetMusterPlot(),GetTargetPlot(),GetDeployRange(),false);
					if (!pDeployPlot)
					{
						// No path, abort
						m_eCurrentState = AI_OPERATION_STATE_ABORTED;
						m_eAbortReason = AI_ABORT_LOST_PATH;
						return;
					}

					pArmyAI->SetGoalPlot(pDeployPlot);

					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), pDeployPlot))
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
int CvAIOperationEnemyTerritory::GetDeployRange() const
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

	return CvAIOperationEnemyTerritory::SelectInitialMusterPoint(pThisArmy);
}
#endif

/// Figure out the initial rally point
CvPlot* CvAIOperationEnemyTerritory::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
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
		if(pThisArmy != NULL)
		{
			if(GET_PLAYER(pThisArmy->GetOwner()).getCapitalCity() != NULL)
			{
				SetMusterPlot(GET_PLAYER(pThisArmy->GetOwner()).getCapitalCity()->plot());
				return GetMusterPlot();
			}
		}
	}
#endif
	else
	{
		if (GC.getLogging() && GC.getAILogging() && pThisArmy)
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
#if defined(MOD_BALANCE_CORE)
			int iFormation = MilitaryAIHelpers::GetBestFormationType();
			if(iFormation != NO_MUFORMATION)
			{
				pArmyAI->SetFormationIndex(iFormation);
			}
			else
			{
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_MUSTER;
				return;
			}
#else
			pArmyAI->SetFormationIndex(GetFormation());
#endif

#if defined(MOD_BALANCE_CORE)
			if(pTarget != NULL && pMuster != NULL)
			{
				CvPlot* pDeployPt = GetPlotXInStepPath(pArmyAI,pMuster->plot(),pTarget->plot(),GetDeployRange(),false);
				if (pDeployPt)
				{
					SetTargetPlot(pTarget->plot());
					pArmyAI->SetGoalPlot(pDeployPt);
					SetMusterPlot(pMuster->plot());
					pArmyAI->SetXY(pMuster->plot()->getX(), pMuster->plot()->getY());
					SetDefaultArea(pMuster->getArea());
					SetTurnStarted(GC.getGame().getGameTurn());
					// Find the list of units we need to build before starting this operation in earnest
					BuildListOfUnitsWeStillNeedToBuild();

					// try to get as many units as possible from existing units that are waiting around
					if(GrabUnitsFromTheReserves(GetMusterPlot(), pArmyAI->GetGoalPlot()))
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
					m_eAbortReason = AI_ABORT_LOST_TARGET;
				}
#else
			if(pTarget)
			{

				SetTargetPlot(pTarget->plot());
				pArmyAI->SetGoalPlot(GetTargetPlot());
				SetMusterPlot(GetStartCityPlot());
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());
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
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_PATH;
				}
#endif
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

MultiunitFormationTypes CvAIOperationBasicCityAttack::GetFormation() const
{
#if defined(MOD_BALANCE_CORE)
	return MilitaryAIHelpers::GetBestFormationType();
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
		bool bHere = false;
		CvString strMsg;
		CvPlot* pCurrentPlot = NULL;
		if(pArmy)
		{
			UnitHandle pUnit;
			pUnit = pArmy->GetFirstUnit();
			while(pUnit && !bHere)
			{
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL)
					{
						UnitHandle pOtherUnit = pAdjacentPlot->getBestDefender(m_eEnemy);
						if((pOtherUnit && pOtherUnit->getOwner() == m_eEnemy) || pAdjacentPlot->getOwner() == m_eEnemy)
						{
							bHere = true;
							if(pAdjacentPlot->getOwner() == m_eEnemy)
							{
								pCurrentPlot = pAdjacentPlot;
							}
							// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
							if(GC.getLogging() && GC.getAILogging())
							{
								strMsg.Format("Ran into enemy during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
								LogOperationSpecialMessage(strMsg);
							}
							break;
						}
					}
				}
				pUnit = pArmy->GetNextUnit();
			}
			if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
			{
				bHere = true;
			}
			if(bHere)
			{
				// Notify Diplo AI we're in place for attack
				if(!GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
				{
					GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);
				}
				if(pCurrentPlot != NULL)
				{
					CvCity* pCity = pCurrentPlot->getWorkingCity();
					if(pCity != NULL)
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCity->getX());
						zone.SetY(pCity->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
					else
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCurrentPlot->getX());
						zone.SetY(pCurrentPlot->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
				}
				else
				{
					// Notify tactical AI to focus on this area
					CvTemporaryZone zone;
					zone.SetX(GetTargetPlot()->getX());
					zone.SetY(GetTargetPlot()->getY());
					zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
					zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				return true;
			}
		}
	}
	break;
#else
		// Are we within tactical range of our target?
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
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
#endif
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
#if defined(MOD_BALANCE_CORE)
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
#else
		if(GetTargetPlot()->getPlotCity() == NULL || GetTargetPlot()->getOwner() != m_eEnemy)

		{
			// Success!  The city has been captured/destroyed
			return true;
		}
#endif
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
	return MilitaryAIHelpers::GetBestFormationType();
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
	kStream >> m_bCivilianRescue;
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
	kStream << m_iUnitToRescue;
}

/// Same as default version except if just gathered forces, check to see if a better target has presented itself
bool CvAIOperationDestroyBarbarianCamp::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;
	switch(m_eCurrentState)
	{
		// If we were gathering forces, let's make sure a better target hasn't presented itself
	case AI_OPERATION_STATE_GATHERING_FORCES:
	{
		// First do base case processing
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

#if defined(MOD_BALANCE_CORE)
		// we're doing this in VerifyTarget() now
#else
		// Now revisit target
		CvPlot* possibleBetterTarget;
		possibleBetterTarget = FindBestTarget();
		// If no target left, abort
		if(possibleBetterTarget == NULL)
		{
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_LOST_TARGET;
		}
		else if(possibleBetterTarget != GetTargetPlot())
		{
			// If we're traveling on a single continent, set our destination to be a few plots shy of the final target
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
		// we're doing this in VerifyTarget() now
		if (GetTargetPlot()==NULL)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("No Barbarian camp target for army. Aborting");
				LogOperationSpecialMessage(strMsg);
			}
			return true;
		}
#else
		// See if our target camp is still there
		if (!m_bCivilianRescue && GetTargetPlot()->getImprovementType() != GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			// Success!  The camp is gone
			if(GC.getLogging() && GC.getAILogging())
			{
				strMsg.Format("Barbarian camp at (x=%d y=%d) no longer exists. Aborting", GetTargetPlot()->getX(), GetTargetPlot()->getY());
				LogOperationSpecialMessage(strMsg);
			}
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
#endif
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
					iCurPlotDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pLoopUnit->getX(), pLoopUnit->getY());
					
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
						iCurPlotDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
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

/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
bool CvAIOperationPillageEnemy::VerifyTarget(CvArmyAI* pArmy)
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
			return true;
#else
			// Reset our destination to be a few plots shy of the final target
			CvPlot* pDeployPt;
			pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), pBetterTarget, GC.getAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE(), false);
			pArmy->SetGoalPlot(pDeployPt);
#endif
		}
#if defined(MOD_BALANCE_CORE)
		else
		{
			return true;
		}
#endif
	}
	break;

	// In all other cases use base class version
	case AI_OPERATION_STATE_AT_TARGET:
	case AI_OPERATION_STATE_RECRUITING_UNITS:
	case AI_OPERATION_STATE_GATHERING_FORCES:
	case AI_OPERATION_STATE_ABORTED:
		return CvAIOperation::VerifyTarget(pArmy);
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
			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			return true;
#else
			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
#endif
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
				iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pStartCity->getX(), pStartCity->getY());
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
					iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pStartCity->getX(), pStartCity->getY());
					if(iDistance > 0)
					{
						iValue = iValue * 100 / iDistance;
					}
					if(iDistance < iMaxDistance)
					{
						iMaxDistance = iDistance;
						pBestTargetCity = pLoopCity;
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
// CvAIOperationEscorted
////////////////////////////////////////////////////////////////////////////////
CvAIOperationEscorted::CvAIOperationEscorted()
{
	m_bEscorted = true;
	m_iTargetArea = -1;
}

CvAIOperationEscorted::~CvAIOperationEscorted()
{
}

/// Kick off this operation
void CvAIOperationEscorted::Init(int iID, PlayerTypes eOwner, PlayerTypes /* eEnemy */, int /* iDefaultArea */, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
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
#if defined(MOD_BALANCE_CORE)
				SetTurnStarted(GC.getGame().getGameTurn());
#endif
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
void CvAIOperationEscorted::Read(FDataStream& kStream)
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
void CvAIOperationEscorted::Write(FDataStream& kStream) const
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
void CvAIOperationEscorted::UnitWasRemoved(int /*iArmyID*/, int iSlotID)
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
CvUnit* CvAIOperationEscorted::FindBestCivilian()
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
bool CvAIOperationEscorted::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
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

						int iTurns = bCanFindPath ? GC.getPathFinder().GetPathLength() : INT_MAX;
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
#if defined(MOD_BALANCE_CORE)
				SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
			bool bCanFound = pSettler->canFound( GetTargetPlot() ); //move to verify target
			// If the settler made it, we don't care about the entire army
			if(bCanFound && pSettler->plot() == GetTargetPlot() && pSettler->canMove())
			{
				CvPlot* pCityPlot = pSettler->plot();
				int iPlotValue = pCityPlot->getFoundValue(m_eOwner);

				//now that the neighboring tiles are guaranteed to be revealed, recheck if we are at the best plot
				//minor twist: the nearby plots are already targeted for a city. so we need to ignore this very operation when checking the plots
				CvPlot* pAltPlot = GET_PLAYER(m_eOwner).GetBestSettlePlot(pSettler, !m_bEscorted, m_iTargetArea, this, true);
				int iAltValue = pAltPlot ? pAltPlot->getFoundValue(m_eOwner) : 0;
				int iDelta = pAltPlot ? ::plotDistance(pCityPlot->getX(),pCityPlot->getY(),pAltPlot->getX(),pAltPlot->getY()) : 0;
				//Must be much better to be worth it.
				if( iAltValue < GetTargetPlot()->getFoundValue(m_eOwner)*1.2 )
					iDelta = 0;

				bool bDoFound = false;
				if (iDelta == 0 || iDelta > 3 || m_iRetargetCount >= 1)
					//found here
					bDoFound = true;
				else
				{
					//alternative plot is better!
					m_iRetargetCount += 1;

					if(GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("Retargeting to adjacent plot. Target was (X=%d Y=%d value=%d), new target (X=%d Y=%d value=%d)", 
							GetTargetPlot()->getX(), GetTargetPlot()->getY(), iPlotValue, pAltPlot->getX(),pAltPlot->getY(), iAltValue );
						LogOperationSpecialMessage(strMsg);
					}

					if (TurnsToReachTarget(pSettler,pAltPlot)==0)
					{
						//we can both move and found this turn!
						pSettler->PushMission(CvTypes::getMISSION_MOVE_TO(),pAltPlot->getX(),pAltPlot->getY());
						bDoFound=true;
					}
					else
					{
						//taken from RetargetCivilian()
						SetTargetPlot(pAltPlot);
						pArmy->SetGoalPlot(pAltPlot);
						pArmy->SetArmyAIState(ARMYAISTATE_MOVING_TO_DESTINATION);
						m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
						//start moving right away
						pSettler->PushMission(CvTypes::getMISSION_MOVE_TO(),pAltPlot->getX(),pAltPlot->getY());
						pSettler->finishMoves();
						iUnitID = pArmy->GetNextUnitID();
						if(iUnitID != -1)
						{
							pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
							pEscort->PushMission(CvTypes::getMISSION_MOVE_TO(),pAltPlot->getX(),pAltPlot->getY());
							pEscort->finishMoves();
						}
					}
				}

				if (bDoFound)
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
void CvAIOperationEscorted::SetEscorted(bool bValue)
{
	m_bEscorted = bValue;
}

bool CvAIOperationEscorted::IsEscorted()
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
			if (pUnit && (GetMusterPlot()->getOwner() != m_eOwner || GC.getGame().getGameTurn() - GetTurnStarted() > 10)) // 10 turns and still no escort?
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
#if defined(MOD_BALANCE_CORE)
				SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
CvPlot* CvAIOperationDiplomatDelegation::FindBestTarget(CvUnit* pUnit, bool /*bOnlySafePaths*/)
{
	CvAssertMsg(pUnit, "pUnit cannot be null");
	if(!pUnit)
	{
		return NULL;
	}

	return GET_PLAYER(pUnit->getOwner()).ChooseMessengerTargetPlot(pUnit);
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
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;

	if(iID != -1)
	{
		if(pTarget != NULL && pMuster != NULL)
		{
			CvPlot* pTargetPlot = NULL;
			for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
			{
				CvPlot* pAdjacentPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iDirectionLoop));
				if(pAdjacentPlot != NULL && !pAdjacentPlot->isWater() && !pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isImpassable(GET_PLAYER(m_eOwner).getTeam()))
				{
					pTargetPlot = pAdjacentPlot;
					break;
				}
			}
			SetTargetPlot(pTargetPlot);
			// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
			CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
			if(pArmyAI && pTargetPlot)
			{
				m_viArmyIDs.push_back(pArmyAI->GetID());
				pArmyAI->Init(pArmyAI->GetID(),m_eOwner,m_iID);
				pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE);
				pArmyAI->SetFormationIndex(GetFormation());	
				
				pArmyAI->SetGoalPlot(GetTargetPlot());
				SetMusterPlot(pTargetPlot);
				pArmyAI->SetXY(pTargetPlot->getX(), pTargetPlot->getY());
				SetDefaultArea(pTargetPlot->getArea());
#if defined(MOD_BALANCE_CORE)
				SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
	case AI_OPERATION_STATE_AT_TARGET:
	{
		// First do base case processing
		bStateChanged = CvAIOperation::ArmyInPosition(pArmy);

		// Now revisit target
		if(plotDistance(pArmy->GetX(), pArmy->GetY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= 2)
		{
			CvTemporaryZone zone;
			zone.SetX(GetTargetPlot()->getX());
			zone.SetY(GetTargetPlot()->getY());
			zone.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
			zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
			GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
			m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
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
// CvAIOperationNaval
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationNaval::CvAIOperationNaval()
{
}

/// Destructor
CvAIOperationNaval::~CvAIOperationNaval()
{
}
#if defined(MOD_BALANCE_CORE)
/// Kick off this operation
void CvAIOperationNaval::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* pTarget, CvCity* pMuster)
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
#if defined(MOD_BALANCE_CORE)
					SetTurnStarted(GC.getGame().getGameTurn());
#endif
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
bool CvAIOperationNaval::ArmyInPosition(CvArmyAI* pArmy)
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
			bool bHere = false;
			CvString strMsg;
			CvPlot* pCenterOfMass = NULL;
			CvPlot* pCurrentPlot = NULL;
			if(pArmy)
			{
				pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);
				UnitHandle pUnit;
				pUnit = pArmy->GetFirstUnit();
				while(pUnit && !bHere)
				{
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
						if(pAdjacentPlot != NULL)
						{
							UnitHandle pOtherUnit = pAdjacentPlot->getBestDefender(m_eEnemy);
							if((pOtherUnit && pOtherUnit->getOwner() == m_eEnemy) || pAdjacentPlot->getOwner() == m_eEnemy)
							{
								bHere = true;
								if(pAdjacentPlot->getOwner() == m_eEnemy)
								{
									pCurrentPlot = pAdjacentPlot;
								}
								// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
								if(GC.getLogging() && GC.getAILogging())
								{
									strMsg.Format("Ran into enemy during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
									LogOperationSpecialMessage(strMsg);
								}
								break;
							}
						}
					}
					pUnit = pArmy->GetNextUnit();
				}
				if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
				{
					bHere = true;
				}
				if(bHere)
				{
					// Notify Diplo AI we're in place for attack
					if(!GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
					{
						GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);
					}
					if(pCurrentPlot != NULL)
					{
						CvCity* pCity = pCurrentPlot->getWorkingCity();
						if(pCity != NULL)
						{
							// Notify tactical AI to focus on this area
							CvTemporaryZone zone;
							zone.SetX(pCity->getX());
							zone.SetY(pCity->getY());
							zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
							zone.SetNavalInvasion(true);
							zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
							GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
						}
						else
						{
							// Notify tactical AI to focus on this area
							CvTemporaryZone zone;
							zone.SetX(pCurrentPlot->getX());
							zone.SetY(pCurrentPlot->getY());
							zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
							zone.SetNavalInvasion(true);
							zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
							GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
						}
					}
					else
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(GetTargetPlot()->getX());
						zone.SetY(GetTargetPlot()->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}

					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
					return true;
				}
			}
		}
		break;
#else
			if (plotDistance(pArmy->Plot()->getX(), pArmy->Plot()->getY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) < 4)
			{
				// Notify tactical AI to focus on this area
				CvTemporaryZone zone;
				zone.SetX(GetTargetPlot()->getX());
				zone.SetY(GetTargetPlot()->getY());
				zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
				zone.SetNavalInvasion(true);
				zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
				GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
			}
		}
		break;
#endif
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
bool CvAIOperationNaval::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		// See if our target city is still owned by our enemy
#if defined(MOD_BALANCE_CORE)
		if((GetTargetPlot()->getOwner() != m_eEnemy) && (m_eEnemy != BARBARIAN_PLAYER))
#else
		if(GetTargetPlot()->getOwner() != m_eEnemy)
#endif
		{
			// Success!  The city has been captured/destroyed
			return true;
		}
	}

	return rtnValue;
}
#endif

/// How close to target do we end up?
int CvAIOperationNaval::GetDeployRange() const
{
	return GC.getAI_OPERATIONAL_NAVAL_BOMBARDMENT_DEPLOY_RANGE();
}

/// Find the port our operation will leave from
CvCity* CvAIOperationNaval::GetOperationStartCity() const
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
CvPlot* CvAIOperationNaval::SelectInitialMusterPoint(CvArmyAI* pThisArmy)
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
CvUnit* CvAIOperationNaval::FindInitialUnit()
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
#if defined(MOD_BALANCE_CORE)
				if(pkUnitEntry && (pkUnitEntry->GetUnitAIType(UNITAI_ATTACK_SEA) || pkUnitEntry->GetUnitAIType(UNITAI_ASSAULT_SEA)))
#else
				
				if(pkUnitEntry && pkUnitEntry->GetUnitAIType(UNITAI_ATTACK_SEA))
#endif
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
#if defined(MOD_BALANCE_CORE)
	m_eMoveType = AI_OPERATION_MOVETYPE_FREEFORM_NAVAL;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	if(m_eEnemy == NO_PLAYER)
	{
		m_eEnemy = BARBARIAN_PLAYER;
	}
	if(iID != -1 && FindBestTarget() != NULL)
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
			CvPlot* pTargetPlot = GetTargetPlot();
			CvPlot* pMusterPlot = GetMusterPlot();
			if(pTargetPlot != NULL && pMusterPlot != NULL)
			{
				pArmyAI->SetGoalPlot(pTargetPlot);
				//Let's just muster at our target.
				SetMusterPlot(pMusterPlot);
				pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
				SetDefaultArea(GetMusterPlot()->getArea());
				SetTurnStarted(GC.getGame().getGameTurn());

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
	}
	else
	{
		// Lost our target, abort
		m_eCurrentState = AI_OPERATION_STATE_ABORTED;
		m_eAbortReason = AI_ABORT_NO_TARGET;
	}
#else
	m_eMoveType = AI_OPERATION_MOVETYPE_ENEMY_TERRITORY;

	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
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
				//Let's just muster at our target.
				SetMusterPlot(pTargetPlot);
				if(GetMusterPlot() != NULL)
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
				m_eAbortReason = AI_ABORT_LOST_TARGET;
			}
		}
	}
#endif
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
			pArmy->SetGoalPlot(GetTargetPlot());
			SetTargetPlot(GetTargetPlot());
#else
			CvPlot* pDeployPt = GC.getStepFinder().GetXPlotsFromEnd(GetOwner(), GetEnemy(), pArmy->Plot(), possibleBetterTarget, GetDeployRange(), false);


			if(pDeployPt != NULL)
			{
				pArmy->SetGoalPlot(pDeployPt);
				SetTargetPlot(possibleBetterTarget);
			}
#endif
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
	if(m_eOwner == NO_PLAYER)
	{
		return false;
	}
	if(GET_PLAYER(m_eOwner).getCapitalCity() == NULL)
	{
		return false;
	}
	CvPlot* pBestTarget = NULL;
	int iBestPlotDistance = MAX_INT;
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvPlot* pStartPlot = GET_PLAYER(m_eOwner).getCapitalCity()->plot();
	int iLoop;
	CvUnit* pLoopUnit;
	if(pStartPlot != NULL)
	{
		CvPlot* pCoastalStart = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pStartPlot, NULL);
		if(pCoastalStart != NULL)
		{
			int iCurrentTurns = -1;
			for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
			{
				if (pLoopUnit != NULL)
				{
					if(pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit())
					{
						CvAStarNode* pPathfinderNode;
						// Water path between muster point and target?
						if(GC.GetInternationalTradeRouteWaterFinder().GeneratePath(pLoopUnit->getX(), pLoopUnit->getY(), pCoastalStart->getX(), pCoastalStart->getY()))
						{
							pPathfinderNode = GC.GetInternationalTradeRouteWaterFinder().GetLastNode();
							if(pPathfinderNode != NULL)
							{
								if(pPathfinderNode != NULL)
								{
									iCurrentTurns = (pPathfinderNode->m_iTotalCost / 100);
								}
								if(iCurrentTurns == MAX_INT || iCurrentTurns == -1)
								{
									continue;
								}
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
			if(pBestTarget == NULL)
			{
				for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
				{
					if (pLoopUnit != NULL)
					{
						if(pLoopUnit->plot()->isCoastalLand() && pLoopUnit->IsCombatUnit() && pLoopUnit->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
						{
							CvPlot* pCoastal = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pLoopUnit->plot(), NULL);
							if(pCoastal != NULL)
							{
								CvAStarNode* pPathfinderNode;
								// Water path between muster point and target?
								if(GC.GetInternationalTradeRouteWaterFinder().GeneratePath(pCoastal->getX(), pCoastal->getY(), pCoastalStart->getX(), pCoastalStart->getY()))
								{
									pPathfinderNode = GC.GetInternationalTradeRouteWaterFinder().GetLastNode();
									if(pPathfinderNode != NULL)
									{
										if(pPathfinderNode != NULL)
										{
											iCurrentTurns = (pPathfinderNode->m_iTotalCost / 100);
										}
										if(iCurrentTurns == MAX_INT || iCurrentTurns == -1)
										{
											continue;
										}
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
				}
			}
			if(pBestTarget != NULL)
			{
				SetTargetPlot(pBestTarget);
				SetMusterPlot(pCoastalStart);
			}
		}
	}
	return pBestTarget;
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
	SetDefaultArea(iDefaultArea);
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
			if(pMuster != NULL)
			{
				CvPlot* pPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), NULL);
				if(pPlot != NULL)
				{
					SetMusterPlot(pPlot);
				}
				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_NO_MUSTER;
					return;
				}
			}
			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_NO_MUSTER;
				return;
			}
			if(pTarget != NULL)
			{		
				SetTargetPlot(pTarget->plot());
				CvPlot* pPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), NULL);
				if(pPlot != NULL && GetMusterPlot() != NULL)
				{
					pArmyAI->SetGoalPlot(pPlot);
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
					SetTurnStarted(GC.getGame().getGameTurn());
					SetDefaultArea(GetMusterPlot()->getArea());
					SetTargetPlot(pPlot);
				}
				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_TARGET;
					return;
				}
			}
			else
			{
				// Lost our target, abort
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_LOST_TARGET;
				return;
			}
			if(GetMusterPlot() != NULL && GetTargetPlot() != NULL)
			{
				// Find the list of units we need to build before starting this operation in earnest
				BuildListOfUnitsWeStillNeedToBuild();

				// try to get as many units as possible from existing units that are waiting around
				if(GrabUnitsFromTheReserves(GetTargetPlot(), GetMusterPlot()))
				{
					pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
					m_eCurrentState = AI_OPERATION_STATE_MOVING_TO_TARGET;
				}
				else
				{
					m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
				}

				LogOperationStart();	
			}
		}
	}
#else
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
#if defined(MOD_BALANCE_CORE)
			if(pMuster != NULL)
			{
				CvPlot* pPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), NULL);
				if(pPlot != NULL)
				{
					SetMusterPlot(pPlot);
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
				m_eAbortReason = AI_ABORT_NO_MUSTER;
			}
			if(pTarget != NULL)
			{		
				SetTargetPlot(pTarget->plot());
#if defined(MOD_BALANCE_CORE)
				CvPlot* pPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), NULL);
#else
				CvPlot* pPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), pArmyAI);
#endif
				if(pPlot != NULL && GetMusterPlot() != NULL)
				{
					pArmyAI->SetGoalPlot(pPlot);
					pArmyAI->SetXY(GetMusterPlot()->getX(), GetMusterPlot()->getY());
#if defined(MOD_BALANCE_CORE)
					SetTurnStarted(GC.getGame().getGameTurn());
					SetDefaultArea(GetMusterPlot()->getArea());
					SetTargetPlot(pPlot);
#else
					SetDefaultArea(iDefaultArea);
#endif
				}
#if defined(MOD_BALANCE_CORE)
				else
				{
					// Lost our target, abort
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_LOST_TARGET;
				}
#endif
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
#endif
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
#if defined(MOD_BALANCE_CORE)
		if(plotDistance(pArmy->GetX(), pArmy->GetY(), GetTargetPlot()->getX(), GetTargetPlot()->getY()) <= 2)
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
#endif
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
	m_eMoveType = AI_OPERATION_MOVETYPE_FREEFORM_NAVAL;
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	CvPlot* pMusterPlot = NULL;
	CvPlot* pTargetPlot = NULL;

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
		pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMuster->plot(), NULL);
	}
	if(pMusterPlot != NULL && pMusterPlot->GetNumCombatUnits() > 0)
	{
		pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterPlot, NULL);
	}

	if(pTarget == NULL || !pTarget->isCoastal())
	{
		CvCity* pNearestCoastalCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetNearestCoastalCityEnemy(eEnemy);
		if(pNearestCoastalCity != NULL)
		{
			pTarget = pNearestCoastalCity;
		}
	}
	if(pTarget != NULL)
	{
		pTargetPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTarget->plot(), NULL);
	}
	if(pTargetPlot != NULL && pTargetPlot->GetNumCombatUnits() > 0)
	{
		pTargetPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetPlot, NULL);
	}

	if(pMusterPlot != NULL && pTargetPlot != NULL)
	{
		// create the armies that are needed and set the state to ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE
		CvArmyAI* pArmyAI = GET_PLAYER(m_eOwner).addArmyAI();
		if(pArmyAI)
		{
			SetDefaultArea(pMusterPlot->getArea());
			SetStartCityPlot(pMusterPlot);
#if defined(MOD_BALANCE_CORE)
			SetTurnStarted(GC.getGame().getGameTurn());
#endif
			if(iID != -1)
			{
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
			bool bHere = false;
			CvString strMsg;
			CvPlot* pCenterOfMass = NULL;
			CvPlot* pCurrentPlot = NULL;
			if(pArmy)
			{
				pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);
				UnitHandle pUnit;
				pUnit = pArmy->GetFirstUnit();
				while(pUnit && !bHere)
				{
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
						if(pAdjacentPlot != NULL)
						{
							UnitHandle pOtherUnit = pAdjacentPlot->getBestDefender(m_eEnemy);
							if((pOtherUnit && pOtherUnit->getOwner() == m_eEnemy) || pAdjacentPlot->getOwner() == m_eEnemy)
							{
								bHere = true;
								if(pAdjacentPlot->getOwner() == m_eEnemy)
								{
									pCurrentPlot = pAdjacentPlot;
								}
								// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
								if(GC.getLogging() && GC.getAILogging())
								{
									strMsg.Format("Ran into enemy during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
									LogOperationSpecialMessage(strMsg);
								}
								break;
							}
						}
					}
					pUnit = pArmy->GetNextUnit();
				}
				if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
				{
					bHere = true;
				}
				if(bHere)
				{
					// Notify Diplo AI we're in place for attack
					if(!GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
					{
						GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);
					}
					if(pCurrentPlot != NULL)
					{
						CvCity* pCity = pCurrentPlot->getWorkingCity();
						if(pCity != NULL)
						{
							// Notify tactical AI to focus on this area
							CvTemporaryZone zone;
							zone.SetX(pCity->getX());
							zone.SetY(pCity->getY());
							zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
							zone.SetNavalInvasion(true);
							zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
							GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
						}
						else
						{
							// Notify tactical AI to focus on this area
							CvTemporaryZone zone;
							zone.SetX(pCurrentPlot->getX());
							zone.SetY(pCurrentPlot->getY());
							zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
							zone.SetNavalInvasion(true);
							zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
							GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
						}
					}
					else
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(GetTargetPlot()->getX());
						zone.SetY(GetTargetPlot()->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}

					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
					return true;
				}
			}
		}
		break;
#else
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
#endif
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
#if defined(MOD_BALANCE_CORE)
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
#else
		// See if our target city is still owned by our enemy
		if(GetTargetPlot()->getOwner() != m_eEnemy)
		{
			// Success!  The city has been captured/destroyed
			return true;
		}
#endif
	}

	return rtnValue;
}

/// Find a plot next to the city we want to attack
CvPlot* CvAIOperationPureNavalCityAttack::FindBestTarget()
{
	CvAssertMsg(false, "Obsolete function called CvAIOperationPureNavalCityAttack::FindBestTarget()");

	return NULL;
}
#if defined(MOD_BALANCE_CORE)
////////////////////////////////////////////////////////////////////////////////
// CvAIOperationCityCloseDefense - Place holder
////////////////////////////////////////////////////////////////////////////////

/// Constructor
CvAIOperationCityCloseDefensePeace::CvAIOperationCityCloseDefensePeace()
{
}

/// Destructor
CvAIOperationCityCloseDefensePeace::~CvAIOperationCityCloseDefensePeace()
{
}

/// Kick off this operation
void CvAIOperationCityCloseDefensePeace::Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int /*iDefaultArea*/, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_iID = iID;
	m_eOwner = eOwner;
	m_eEnemy = eEnemy;
	m_eMoveType = AI_OPERATION_MOVETYPE_STATIC;

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
#if defined(MOD_BALANCE_CORE)
				SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
		}
	}
}

/// Find the best blocking position against the current threats
CvPlot* CvAIOperationCityCloseDefensePeace::FindBestTarget()
{
	CvCity* pCity;
	CvPlot* pPlot = NULL;

	// Defend the city most under threat
	pCity = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetMostThreatenedCity();

	if(pCity != NULL)
	{
		pPlot = pCity->plot();
	}

	return pPlot;
}
#endif
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
	m_eMoveType = AI_OPERATION_MOVETYPE_STATIC;

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
#if defined(MOD_BALANCE_CORE)
				SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
#if defined(MOD_BALANCE_CORE)
				SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
bool CvAIOperationRapidResponse::VerifyTarget(CvArmyAI* pArmy)
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
		return CvAIOperation::VerifyTarget(pArmy);
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
// CvAIOperationNavalEscorted
////////////////////////////////////////////////////////////////////////////////
CvAIOperationNavalEscorted::CvAIOperationNavalEscorted()
{
	m_iTargetArea = -1;
}

CvAIOperationNavalEscorted::~CvAIOperationNavalEscorted()
{
}

/// Kick off this operation
void CvAIOperationNavalEscorted::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int iDefaultArea, CvCity* pTarget, CvCity* pMuster)
{
	Reset();
	m_iTargetArea = iDefaultArea;
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;
	SetTurnStarted(GC.getGame().getGameTurn());

	if (pMuster && pTarget)
	{
		CvPlot* pStartPlot = pMuster->plot();

		if(pStartPlot != NULL)
		{
			// Find a destination
			CvPlot* pTargetSite = pTarget->plot();

			if(pTargetSite != NULL)
			{
				CvPlot* pMusterPlot = NULL;
				pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pStartPlot, NULL);
				if(pMusterPlot != NULL)
				{
					CvPlot* pFinalPlot = NULL;
					pFinalPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetSite, NULL);
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
							
							SetTargetPlot(pTargetSite);
							pArmyAI->SetGoalPlot(pTargetSite);
							SetMusterPlot(pMusterPlot);
							pArmyAI->SetXY(pMusterPlot->getX(), pMusterPlot->getY());

							// try to get the escort from existing units that are waiting around
							BuildListOfUnitsWeStillNeedToBuild();

							// Try to get as many units as possible from existing units that are waiting around
							if(GrabUnitsFromTheReserves(pMusterPlot, pFinalPlot))
							{
								pArmyAI->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
								m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
							}
							else
							{
								m_eCurrentState = AI_OPERATION_STATE_RECRUITING_UNITS;
							}
							LogOperationStart();
							return;
						}
					}
				}
			}
		}
	}

	// if anything went wrong
	m_eCurrentState = AI_OPERATION_STATE_ABORTED;
	m_eAbortReason = AI_ABORT_LOST_TARGET;
}

CvAIOperationNavalColonization::CvAIOperationNavalColonization()
{
	m_eCivilianType = UNITAI_SETTLE;
}

CvAIOperationNavalColonization::~CvAIOperationNavalColonization()
{
}

void CvAIOperationNavalColonization::Init(int iID, PlayerTypes eOwner, PlayerTypes /*eEnemy*/, int iDefaultArea, CvCity* /*pTarget*/, CvCity* /*pMuster*/)
{
	Reset();
	m_iTargetArea = iDefaultArea;
	m_eMoveType = AI_OPERATION_MOVETYPE_NAVAL_ESCORT;
	m_iID = iID;
	m_eOwner = eOwner;

	SetTurnStarted(GC.getGame().getGameTurn());

	// Find the free civilian (that triggered this operation)
	CvUnit* pOurCivilian = FindBestCivilian();
	if(pOurCivilian != NULL && m_iID != -1)
	{
		// Find a destination (not worrying about safe paths)
		CvPlot* pTargetSite = FindBestTarget(pOurCivilian, false);
		CvCity* pMusterCity = NULL;
		CvPlot* pMusterPlot = NULL;
		if(pTargetSite != NULL)
		{
			CvCity* pLoopCity;
			int iCityLoop;
			int iCurPlotDistance = 0;
			int iBestPlotDistance = MAX_INT;
			// Loop through each of our cities
			for(pLoopCity = GET_PLAYER(m_eOwner).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eOwner).nextCity(&iCityLoop))
			{
				if(pLoopCity != NULL)
				{
					if(!pLoopCity->isCoastal())
					{
						continue;
					}
					iCurPlotDistance = plotDistance(pOurCivilian->plot()->getX(), pOurCivilian->plot()->getY() , pLoopCity->plot()->getX(), pLoopCity->plot()->getY());
					if (iCurPlotDistance < iBestPlotDistance)
					{
						iBestPlotDistance = iCurPlotDistance;
						pMusterCity = pLoopCity;
					}
				}
			}
			if(pMusterCity != NULL)
			{
				pMusterPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pMusterCity->plot(), NULL);
				if(pMusterPlot != NULL)
				{
					CvPlot* pFinalPlot = NULL;
					pFinalPlot = GET_PLAYER(m_eOwner).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pTargetSite, NULL);
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
							SetTargetPlot(pTargetSite);
							SetMusterPlot(pMusterPlot);
							pArmyAI->SetXY(pMusterPlot->getX(), pMusterPlot->getY());
							SetDefaultArea(pMusterPlot->getArea());
							SetTurnStarted(GC.getGame().getGameTurn());

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
						
							GrabUnitsFromTheReserves(pMusterPlot, pFinalPlot);
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
				m_eAbortReason = AI_ABORT_NO_MUSTER;
			}
		}
		else
		{
			// Lost our target, abort
			m_eCurrentState = AI_OPERATION_STATE_ABORTED;
			m_eAbortReason = AI_ABORT_NO_TARGET;
		}
	}
}

/// Find the civilian we want to use
CvUnit* CvAIOperationNavalColonization::FindBestCivilian()
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
void CvAIOperationNavalEscorted::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperation::Read(kStream);
}

/// Write serialized data
void CvAIOperationNavalEscorted::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperation::Write(kStream);
}

/// Read serialized data
void CvAIOperationNavalColonization::Read(FDataStream& kStream)
{
	// read the base class' entries
	CvAIOperationNavalEscorted::Read(kStream);
	kStream >> m_eCivilianType;
}
/// Write serialized data
void CvAIOperationNavalColonization::Write(FDataStream& kStream) const
{
	// write the base class' entries
	CvAIOperationNavalEscorted::Write(kStream);
	kStream << m_eCivilianType;
}

/// If at target, found city; if at muster point, merge settler and escort and move out
bool CvAIOperationNavalEscorted::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;
	CvString strMsg;

	switch(m_eCurrentState)
	{
		// See if reached our target, if so give control of these units to the tactical AI
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
		{
			CvPlot *pCenterOfMass = pArmy->GetCenterOfMass(DOMAIN_SEA);

			// Are we within tactical range of our target? (larger than usual range for a naval attack)
			bool bHere = false;
			CvString strMsg;
			if(pArmy)
			{
				CvPlot* pAttackPlot = NULL;

				if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
				{
					pAttackPlot = GetTargetPlot();
					bHere = true;
				}

				UnitHandle pUnit = pArmy->GetFirstUnit();
				while(pUnit && !bHere)
				{
					for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
					{
						CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
						if(pAdjacentPlot != NULL)
						{
							UnitHandle pOtherUnit = pAdjacentPlot->getBestDefender(m_eEnemy);
							if((pOtherUnit && pOtherUnit->getOwner() == m_eEnemy) || pAdjacentPlot->getOwner() == m_eEnemy)
							{
								bHere = true;
								if(pAdjacentPlot->getOwner() == m_eEnemy)
								{
									pAttackPlot = pAdjacentPlot;
								}
								// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
								if(GC.getLogging() && GC.getAILogging())
								{
									strMsg.Format("Ran into enemy during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
									LogOperationSpecialMessage(strMsg);
								}
								break;
							}
						}
					}
					pUnit = pArmy->GetNextUnit();
				}

				if(bHere)
				{
					// Notify Diplo AI we're in place for attack
					if(!GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
					{
						GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);
					}

					CvCity* pCity = pAttackPlot ? pAttackPlot->getWorkingCity() : NULL;
					if(pCity != NULL)
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCity->getX());
						zone.SetY(pCity->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
					else
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pAttackPlot->getX());
						zone.SetY(pAttackPlot->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}

					m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
					return true;
				}
			}
			break;
		}
		// In all other cases use base class version
		case AI_OPERATION_STATE_GATHERING_FORCES:
		case AI_OPERATION_STATE_ABORTED:
		case AI_OPERATION_STATE_RECRUITING_UNITS:
		case AI_OPERATION_STATE_AT_TARGET:
			return CvAIOperation::ArmyInPosition(pArmy);
			break;
	};

	return bStateChanged;
}

bool CvAIOperationNavalColonization::ArmyInPosition(CvArmyAI* pArmy)
{
	bool bStateChanged = false;
	CvUnit* pSettler = NULL;
	CvUnit* pEscort = NULL;

	CvString strMsg;
	int iUnitID = pArmy->GetFirstUnitID();
	if(iUnitID != -1)
	{
		pSettler = GET_PLAYER(m_eOwner).getUnit(iUnitID);
		iUnitID = pArmy->GetNextUnitID();
		if(iUnitID != -1)
			pEscort = GET_PLAYER(m_eOwner).getUnit(iUnitID);
	}

	switch(m_eCurrentState)
	{
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
			break;
		}
		case AI_OPERATION_STATE_MOVING_TO_TARGET:
		case AI_OPERATION_STATE_AT_TARGET:
		{
			// Call base class version and see if it thinks we're done
			bStateChanged = CvAIOperation::ArmyInPosition(pArmy);
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
					if(pEscort)
						pEscort->finishMoves();
				}

				// If the settler made it, we don't care about the entire army
				else if(pSettlerPlot == pTargetPlot && pSettler->canMove() && pSettler->canFound(pSettlerPlot))
				{
					int iPlotValue = pSettlerPlot->getFoundValue(m_eOwner);
					pSettler->PushMission(CvTypes::getMISSION_FOUND());
					if(GC.getLogging() && GC.getAILogging())
					{
						CvArea* pArea = pSettlerPlot->area();
						CvCity* pCity = pSettlerPlot->getPlotCity();

						if (pCity != NULL)
						{
							strMsg.Format("City founded (%s), At X=%d, At Y=%d, %s, %d, %d", pCity->getName().c_str(), pSettlerPlot->getX(), pSettlerPlot->getY(), pCity->getName().GetCString(), iPlotValue, pArea->getTotalFoundValue());
							LogOperationSpecialMessage(strMsg);
						}
						LogOperationSpecialMessage(strMsg);
					}

					// Notify tactical AI to focus on this area
					CvTemporaryZone zone;
					zone.SetX(pSettlerPlot->getX());
					zone.SetY(pSettlerPlot->getY());
					zone.SetTargetType(AI_TACTICAL_TARGET_CITY_TO_DEFEND);
					zone.SetLastTurn(GC.getGame().getGameTurn() + (GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() * 2));
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
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

					if(pEscort)
						pEscort->finishMoves();
				}
			}
		break;
		}
		// In all other cases use base class verson
		case AI_OPERATION_STATE_ABORTED:
		case AI_OPERATION_STATE_RECRUITING_UNITS:
		{
			return CvAIOperation::ArmyInPosition(pArmy);
			break;
		}
	};

	return bStateChanged;
}

//same as for CvAIOperationFoundCity
bool CvAIOperationNavalColonization::VerifyTarget(CvArmyAI* pArmy)
{
	if (!pArmy)
		return false;

	int iUnitID = pArmy->GetFirstUnitID();
	if(iUnitID == -1)
		return false;

	CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
	CvPlot* pTargetPlot = GetTargetPlot();

	//cannot settle at the moment
	if (GET_PLAYER(m_eOwner).IsEmpireVeryUnhappy())
	{
		//return to muster plot ...
		pArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
		m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
	}
	else if ( pTargetPlot==NULL || 
		!pCivilian->canFound(pTargetPlot,false,true) || //don't check happiness!
		!pCivilian->canMoveInto(*pTargetPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE) )
		//don't check the whole path here - it is very expensive and usually not a problem
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Retargeting. We can no longer settle at target (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
			LogOperationSpecialMessage(strMsg);
		}
		RetargetCivilian(pCivilian, pArmy);
	}

	return (GetTargetPlot() != NULL);
}


/// Find the plot where we want to settle
CvPlot* CvAIOperationNavalColonization::FindBestTargetIncludingCurrent(CvUnit* pUnit, bool bOnlySafePaths)
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

CvPlot* CvAIOperationNavalColonization::FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths)
{
	CvPlot* pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, m_iTargetArea);
	if (pResult == NULL)
	{
		m_iTargetArea = -1;
		pResult = GET_PLAYER(m_eOwner).GetBestSettlePlot(pUnit, bOnlySafePaths, -1);
	}
	return pResult;
}

/// Start the civilian off to a new target plot
bool CvAIOperationNavalColonization::RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy)
{
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
			SetTurnStarted(GC.getGame().getGameTurn());
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
}

////////////////////////////////////////////////////////////////////////////////
// CvAIOperationNavalAttack
////////////////////////////////////////////////////////////////////////////////
CvAIOperationNavalAttack::CvAIOperationNavalAttack()
{
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

		if (pMusterPlot != NULL)
		{
			iDefaultArea = pMusterPlot->getArea();
			SetDefaultArea(iDefaultArea);
#if defined(MOD_BALANCE_CORE)
			SetTurnStarted(GC.getGame().getGameTurn());
#endif
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
		bool bHere = false;
		CvString strMsg;
		CvPlot* pCurrentPlot = NULL;
		if(pArmy)
		{
			UnitHandle pUnit;
			pUnit = pArmy->GetFirstUnit();
			while(pUnit && !bHere)
			{
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL)
					{
						UnitHandle pOtherUnit = pAdjacentPlot->getBestDefender(m_eEnemy);
						if((pOtherUnit && pOtherUnit->getOwner() == m_eEnemy) || pAdjacentPlot->getOwner() == m_eEnemy)
						{
							bHere = true;
							if(pAdjacentPlot->getOwner() == m_eEnemy)
							{
								pCurrentPlot = pAdjacentPlot;
							}
							// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
							if(GC.getLogging() && GC.getAILogging())
							{
								strMsg.Format("Ran into enemy during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
								LogOperationSpecialMessage(strMsg);
							}
							break;
						}
					}
				}
				pUnit = pArmy->GetNextUnit();
			}
			if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
			{
				bHere = true;
			}
			if(bHere)
			{
				// Notify Diplo AI we're in place for attack
				if(!GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
				{
					GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);
				}
				if(pCurrentPlot != NULL)
				{
					CvCity* pCity = pCurrentPlot->getWorkingCity();
					if(pCity != NULL)
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCity->getX());
						zone.SetY(pCity->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
					else
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCurrentPlot->getX());
						zone.SetY(pCurrentPlot->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
				}
				else
				{
					// Notify tactical AI to focus on this area
					CvTemporaryZone zone;
					zone.SetX(GetTargetPlot()->getX());
					zone.SetY(GetTargetPlot()->getY());
					zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
					zone.SetNavalInvasion(true);
					zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				return true;
			}
		}
	}
	break;
#else
		// Are we within tactical range of our target? (larger than usual range for a naval attack)
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 2)
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
#endif
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
		}
		if(pMusterPlot != NULL)
		{
			SetStartCityPlot(pMusterPlot);
			iDefaultArea = pMusterPlot->getArea();
			SetDefaultArea(iDefaultArea);
#if defined(MOD_BALANCE_CORE)
			SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
#if defined(MOD_BALANCE_CORE)
/// Returns true when we should abort the operation totally (besides when we have lost all units in it)
bool CvAIOperationNavalSneakAttack::ShouldAbort()
{
	// If parent says we're done, don't even check anything else
	bool rtnValue = CvAIOperation::ShouldAbort();

	if(!rtnValue)
	{
		if(GetTargetPlot() != NULL)
		{
			// See if our target city is still owned by our enemy
			if(GetTargetPlot()->getOwner() != m_eEnemy && GetTargetPlot()->getOwner() != NO_PLAYER)
			{
				// Success!  The city has been captured/destroyed
				m_eCurrentState = AI_OPERATION_STATE_ABORTED;
				m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
				return true;
			}
			if(GetTargetPlot()->getWorkingCity() != NULL)
			{
				if(GetTargetPlot()->getWorkingCity()->getOwner() != m_eEnemy)
				{
					m_eCurrentState = AI_OPERATION_STATE_ABORTED;
					m_eAbortReason = AI_ABORT_TARGET_ALREADY_CAPTURED;
					return true;
				}
			}
		}
	}

	return rtnValue;
}
#endif

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
		bool bHere = false;
		CvString strMsg;
		CvPlot* pCurrentPlot = NULL;
		if(pArmy)
		{
			UnitHandle pUnit;
			pUnit = pArmy->GetFirstUnit();
			while(pUnit && !bHere)
			{
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL)
					{
						UnitHandle pOtherUnit = pAdjacentPlot->getBestDefender(m_eEnemy);
						if((pOtherUnit && pOtherUnit->getOwner() == m_eEnemy) || pAdjacentPlot->getOwner() == m_eEnemy)
						{
							bHere = true;
							if(pAdjacentPlot->getOwner() == m_eEnemy)
							{
								pCurrentPlot = pAdjacentPlot;
							}
							// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
							if(GC.getLogging() && GC.getAILogging())
							{
								strMsg.Format("Ran into enemy during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
								LogOperationSpecialMessage(strMsg);
							}
							break;
						}
					}
				}
				pUnit = pArmy->GetNextUnit();
			}
			if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
			{
				bHere = true;
			}
			if(bHere)
			{
				// Notify Diplo AI we're in place for attack
				if(!GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
				{
					GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);
				}
				if(pCurrentPlot != NULL)
				{
					CvCity* pCity = pCurrentPlot->getWorkingCity();
					if(pCity != NULL)
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCity->getX());
						zone.SetY(pCity->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
					else
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCurrentPlot->getX());
						zone.SetY(pCurrentPlot->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
				}
				else
				{
					// Notify tactical AI to focus on this area
					CvTemporaryZone zone;
					zone.SetX(GetTargetPlot()->getX());
					zone.SetY(GetTargetPlot()->getY());
					zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
					zone.SetNavalInvasion(true);
					zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				return true;
			}
		}
	}
	break;
#else
		// Are we within tactical range of our target? (larger than usual range for a naval attack)
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE() * 2)
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
#endif
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
#if defined(MOD_BALANCE_CORE)
			SetTurnStarted(GC.getGame().getGameTurn());
#endif

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
		bool bHere = false;
		CvString strMsg;
		CvPlot* pCurrentPlot = NULL;
		if(pArmy)
		{
			UnitHandle pUnit;
			pUnit = pArmy->GetFirstUnit();
			while(pUnit && !bHere)
			{
				for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
				{
					CvPlot* pAdjacentPlot = plotDirection(pUnit->getX(), pUnit->getY(), ((DirectionTypes)iDirectionLoop));
					if(pAdjacentPlot != NULL)
					{
						UnitHandle pOtherUnit = pAdjacentPlot->getBestDefender(m_eEnemy);
						if((pOtherUnit && pOtherUnit->getOwner() == m_eEnemy) || pAdjacentPlot->getOwner() == m_eEnemy)
						{
							bHere = true;
							if(pAdjacentPlot->getOwner() == m_eEnemy)
							{
								pCurrentPlot = pAdjacentPlot;
							}
							// We ran into a potential enemy unit duing a sneak attack. The jig is probably up, so let's DOW.
							if(GC.getLogging() && GC.getAILogging())
							{
								strMsg.Format("Ran into enemy during sneak attack on (x=%d y=%d). Time to fight!", GetTargetPlot()->getX(), GetTargetPlot()->getY());
								LogOperationSpecialMessage(strMsg);
							}
							break;
						}
					}
				}
				pUnit = pArmy->GetNextUnit();
			}
			if(pCenterOfMass && pArmy->GetGoalPlot() != NULL && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), pArmy->GetGoalX(), pArmy->GetGoalY()) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
			{
				bHere = true;
			}
			if(bHere)
			{
				// Notify Diplo AI we're in place for attack
				if(!GET_TEAM(GET_PLAYER(GetOwner()).getTeam()).isAtWar(GET_PLAYER(m_eEnemy).getTeam()))
				{
					GET_PLAYER(GetOwner()).GetDiplomacyAI()->SetMusteringForAttack(GetEnemy(), true);
				}
				if(pCurrentPlot != NULL)
				{
					CvCity* pCity = pCurrentPlot->getWorkingCity();
					if(pCity != NULL)
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCity->getX());
						zone.SetY(pCity->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
					else
					{
						// Notify tactical AI to focus on this area
						CvTemporaryZone zone;
						zone.SetX(pCurrentPlot->getX());
						zone.SetY(pCurrentPlot->getY());
						zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
						zone.SetNavalInvasion(true);
						zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
						GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
					}
				}
				else
				{
					// Notify tactical AI to focus on this area
					CvTemporaryZone zone;
					zone.SetX(GetTargetPlot()->getX());
					zone.SetY(GetTargetPlot()->getY());
					zone.SetTargetType(AI_TACTICAL_TARGET_CITY);
					zone.SetNavalInvasion(true);
					zone.SetLastTurn(GC.getGame().getGameTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS());
					GET_PLAYER(m_eOwner).GetTacticalAI()->AddTemporaryZone(zone);
				}

				m_eCurrentState = AI_OPERATION_STATE_SUCCESSFUL_FINISH;
				return true;
			}
		}
	}
	break;
#else
		if(pCenterOfMass && plotDistance(pCenterOfMass->getX(), pCenterOfMass->getY(), m_iTargetX, m_iTargetY) <= GC.getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE())
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
#endif
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
												if(GET_PLAYER(ePlotOwner).isMajorCiv() && GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetMajorCivOpinion(ePlotOwner) == MAJOR_CIV_OPINION_UNFORGIVABLE)
												{
													iThisCityValue -= 10;
												}
												else if(GET_PLAYER(ePlotOwner).isMajorCiv() && GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetMajorCivOpinion(ePlotOwner) == MAJOR_CIV_OPINION_ENEMY)
												{
													iThisCityValue -= 100;
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
#if defined(MOD_BALANCE_CORE)
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, WeightedUnitIdVector&)
{
#else
bool CvAIOperationNukeAttack::FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot* /*pMusterPlot*/, CvPlot* /*pTargetPlot*/, bool* bRequired)
{
	// okay, this can be simplified to
	*bRequired = true;
#endif
	CvPlayerAI& ownerPlayer = GET_PLAYER(m_eOwner);
	CvArmyAI* pThisArmy = ownerPlayer.getArmyAI(thisOperationSlot.m_iArmyID);
	pThisArmy->AddUnit(m_iBestUnitID, thisOperationSlot.m_iSlotID);
	return true;
}

//----------------------------------------------
/// Every time the army moves on its way to the destination lets double-check that we don't have a better target
bool CvAIOperationDestroyBarbarianCamp::VerifyTarget(CvArmyAI* pArmy)
{
	if (!pArmy)
		return false;

	bool bNeedNewTarget = false;
	CvString strMsg;

	if(GetTargetPlot()==NULL)
		bNeedNewTarget = true;
	else if (!m_bCivilianRescue)
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
	else if (m_bCivilianRescue)
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
		CvPlot* newTarget = FindBestTarget();
		if(newTarget != NULL)
		{
			SetTargetPlot(newTarget);

			// If we're traveling on a single continent, set our destination to be a few plots shy of the final target
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

bool CvAIOperationFoundCity::VerifyTarget(CvArmyAI* pArmy)
{
	if (!pArmy)
		return false;

	int iUnitID = pArmy->GetFirstUnitID();
	if(iUnitID == -1)
		return false;

	CvUnit* pCivilian = GET_PLAYER(m_eOwner).getUnit(iUnitID);
	CvPlot* pTargetPlot = GetTargetPlot();

	//cannot settle at the moment
	if (GET_PLAYER(m_eOwner).IsEmpireVeryUnhappy())
	{
		//return to muster plot ...
		pArmy->SetArmyAIState(ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP);
		m_eCurrentState = AI_OPERATION_STATE_GATHERING_FORCES;
	}
	else if ( pTargetPlot==NULL || 
		!pCivilian->canFound(pTargetPlot,false,true) || //don't check happiness!
		!pCivilian->canMoveInto(*pTargetPlot, CvUnit::MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE) || 
		!pCivilian->GeneratePath(pTargetPlot, MOVE_TERRITORY_NO_ENEMY, false) )
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Retargeting. We can no longer settle at target (X=%d Y=%d)", GetTargetPlot()->getX(), GetTargetPlot()->getY());
			LogOperationSpecialMessage(strMsg);
		}
		RetargetCivilian(pCivilian, pArmy);
	}

	return (GetTargetPlot() != NULL);
}

bool CvAIOperationMerchantDelegation::VerifyTarget(CvArmyAI* pArmy)
{
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

bool CvAIOperationDiplomatDelegation::VerifyTarget(CvArmyAI* pArmy)
{
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

bool CvAIOperationConcertTour::VerifyTarget(CvArmyAI* pArmy)
{
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
#if defined(MOD_BALANCE_CORE)
/// Find the barbarian camp we want to eliminate
CvPlot* OperationalAIHelpers::FindBestBombardmentTarget(PlayerTypes ePlayer)
{
	if(ePlayer == NO_PLAYER)
	{
		return false;
	}
	if(GET_PLAYER(ePlayer).getCapitalCity() == NULL)
	{
		return false;
	}
	CvPlot* pBestTarget = NULL;
	int iBestPlotDistance = MAX_INT;
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvPlot* pStartPlot = GET_PLAYER(ePlayer).getCapitalCity()->plot();
	int iLoop;
	CvUnit* pLoopUnit;
	if(pStartPlot != NULL)
	{
		CvPlot* pCoastalStart = GET_PLAYER(ePlayer).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pStartPlot, NULL);
		if(pCoastalStart != NULL)
		{
			int iCurrentTurns = -1;
			for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
			{
				if (pLoopUnit != NULL)
				{
					if(pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit())
					{
						CvAStarNode* pPathfinderNode;
						// Water path between muster point and target?
						if(GC.GetInternationalTradeRouteWaterFinder().GeneratePath(pLoopUnit->getX(), pLoopUnit->getY(), pCoastalStart->getX(), pCoastalStart->getY()))
						{
							pPathfinderNode = GC.GetInternationalTradeRouteWaterFinder().GetLastNode();
							if(pPathfinderNode != NULL)
							{
								if(pPathfinderNode != NULL)
								{
									iCurrentTurns = (pPathfinderNode->m_iTotalCost / 100);
								}
								if(iCurrentTurns == MAX_INT || iCurrentTurns == -1)
								{
									continue;
								}
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
			if(pBestTarget == NULL)
			{
				for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
				{
					if (pLoopUnit != NULL)
					{
						if(pLoopUnit->plot()->isCoastalLand() && pLoopUnit->IsCombatUnit() && pLoopUnit->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
						{
							CvPlot* pCoastal = GET_PLAYER(ePlayer).GetMilitaryAI()->GetCoastalPlotAdjacentToTarget(pLoopUnit->plot(), NULL);
							if(pCoastal != NULL)
							{
								CvAStarNode* pPathfinderNode;
								// Water path between muster point and target?
								if(GC.GetInternationalTradeRouteWaterFinder().GeneratePath(pCoastal->getX(), pCoastal->getY(), pCoastalStart->getX(), pCoastalStart->getY()))
								{
									pPathfinderNode = GC.GetInternationalTradeRouteWaterFinder().GetLastNode();
									if(pPathfinderNode != NULL)
									{
										if(pPathfinderNode != NULL)
										{
											iCurrentTurns = (pPathfinderNode->m_iTotalCost / 100);
										}
										if(iCurrentTurns == MAX_INT || iCurrentTurns == -1)
										{
											continue;
										}
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
				}
			}
		}
	}
	if(pBestTarget != NULL)
	{
		return pBestTarget;
	}
	return NULL;
}

/// Find the barbarian camp we want to eliminate
bool OperationalAIHelpers::FindBestBarbCamp(PlayerTypes ePlayer)
{
	int iPlotLoop;

	CvPlot* pPlot;
	int iBestPlotDistance = MAX_INT;
	int iCurPlotDistance;

	if(ePlayer == NO_PLAYER)
	{
		return false;
	}
	if(GET_PLAYER(ePlayer).getCapitalCity() == NULL)
	{
		return false;
	}

	CvPlot* pStartPlot = GET_PLAYER(ePlayer).getCapitalCity()->plot();
	if(pStartPlot != NULL)
	{
		ImprovementTypes eBarbCamp = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();

		// Look at map for Barbarian camps - don't check if they are revealed ... that's the cheating part
		for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
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
						iCurPlotDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
						if(pPlot->getArea() != pLoopCity->getArea())
						{
							iCurPlotDistance *= 2;
						}
						if (iCurPlotDistance < iBestPlotDistance)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
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

	iDistance = plotDistance(pkLoopUnitPlot->getX(), pkLoopUnitPlot->getY(), pMusterPlot->getX(), pMusterPlot->getY());
	if (iDistance > 18)
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
	CvTacticalAnalysisCell* pCell = GC.getGame().GetTacticalAnalysisMap()->GetCell( pLoopUnit->plot()->GetPlotIndex() );
	CvTacticalDominanceZone* pZone = GC.getGame().GetTacticalAnalysisMap()->GetZoneByID( pCell->GetDominanceZone() );
	if (pZone && pZone->GetClosestCity()!=NULL && pZone->GetDominanceFlag()!=TACTICAL_DOMINANCE_FRIENDLY)
		return false;

	//don't take explorers
	CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
	if (unitInfo == NULL || pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
	{
		return false;
	}

	//Sanity checks
	if (pLoopUnit->IsCityAttackOnly() && pTargetPlot->getPlotCity() == NULL)
	{
		/*
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strMsg;
			strMsg.Format("Cannot recruit unit %s: No City Attack.", pLoopUnit->getName().GetCString());
			LogOperationSpecialMessage(strMsg);
		}
		*/
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

	//Land units
	if (pLoopUnit->getDomainType() == DOMAIN_LAND)
	{
		if (bMustNaval)
		{
			if (pMusterPlot->isWater())
			{
				return true;
			}
			else
			{
				/*
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Cannot recruit unit %s: Wrong Target Domain.", pLoopUnit->getName().GetCString());
					LogOperationSpecialMessage(strMsg);
				}
				*/
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	//sea units
	else if (pLoopUnit->getDomainType() == DOMAIN_SEA)
	{
		if (pMusterPlot->isWater())
		{
			//make sure we're on the correct water body
			if (pkLoopUnitPlot->getArea() == pMusterPlot->getArea())
			{
				return true;
			}
			else
			{
				/*
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strMsg;
					strMsg.Format("Cannot recruit unit %s: Cannot Reach.", pLoopUnit->getName().GetCString());
					LogOperationSpecialMessage(strMsg);
				}
				*/
				return false;
			}
		}
		else
		{
			/*
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strMsg;
				strMsg.Format("Cannot recruit unit %s: Wrong Target Domain.", pLoopUnit->getName().GetCString());
				LogOperationSpecialMessage(strMsg);
			}
			*/
			return false;
		}
	}

	return false;
}


bool OperationalAIHelpers::NeedOceanMoves(CvPlot* pMusterPlot, CvPlot* pTargetPlot)
{
	CvAStarNode* pPathfinderNode;
	// Water path between muster point and target?
	if(GC.GetInternationalTradeRouteWaterFinder().GeneratePath(pMusterPlot->getX(), pMusterPlot->getY(), pTargetPlot->getX(), pTargetPlot->getY()))
	{
		pPathfinderNode = GC.GetInternationalTradeRouteWaterFinder().GetLastNode();
		// Starting at the end, loop until we find a plot from this owner
		while(pPathfinderNode != NULL)
		{
			CvPlot* pCurrentPlot = NULL;
			pCurrentPlot = GC.getMap().plotCheckInvalid(pPathfinderNode->m_iX, pPathfinderNode->m_iY);

			// Check and see if this plot is ocean
			if(pCurrentPlot->isWater() && !pCurrentPlot->isShallowWater() && !pCurrentPlot->isLake())
			{
				return true;
			}

			pPathfinderNode = pPathfinderNode->m_pParent;
		}
	}
	else
	{
		return true;
	}
	return false;
}
#endif