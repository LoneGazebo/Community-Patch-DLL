/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//	CvUnit Mission controller
//
#include "CvGameCoreDLLPCH.h"
#include "CvUnit.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "ICvDLLUserInterface.h"
#include "CvUnitMission.h"
#include "CvUnitCombat.h"
#include "CvDllUnit.h"
#include "CvTypes.h"

#if !defined(FINAL_RELEASE)
#include <sstream>

// If defined, various operations related to the movement of units will be logged.
//#define LOG_UNIT_MOVES
#endif

// include this after all other headers
#include "LintFree.h"

#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
// There are three types of mission -
//  - instantaneous (eg MISSION_FOUND_RELIGION)
//  - duration (eg MISSION_ROUTE_TO)
//  - open (eg MISSION_FORTIFY) 
// and the events (eventually) need to cope with all types
// (although for the moment we only care about instantaneous ones)

#define CUSTOM_MISSION_NO_ACTION        0
#define CUSTOM_MISSION_ACTION           1
#define CUSTOM_MISSION_DONE             2
#define CUSTOM_MISSION_ACTION_AND_DONE  3

// ANY: CustomMissionPossible(iPlayer, iUnit, iMission, iData1, iData2, iFlags=0, iTurn=-1, iPlotX, iPlotY, bTestVisible)
// ACC: CustomMissionStart(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) = CUSTOM_MISSION_ACTION
// ACC: CustomMissionSetActivity(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) = CUSTOM_MISSION_ACTION_AND_DONE
// ACC: CustomMissionDoStep(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) = CUSTOM_MISSION_ACTION_AND_DONE
// ANY: CustomMissionCompleted(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn)
// ACC: CustomMissionTargetPlot(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) = iPlotIndex
// ACC: CustomMissionCycleTime(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) = iCameraTime (0, 1, 5 or 10)
// ACC: CustomMissionTimerInc(iPlayer, iUnit, iMission, iData1, iData2, iFlags, iTurn) = iTimerInc
#endif

//	---------------------------------------------------------------------------
/// Perform automated mission
void CvUnitMission::AutoMission(CvUnit* hUnit)
{
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	const MissionData* pkMissionNode = HeadMissionData(hUnit->m_missionQueue);
	if(pkMissionNode != NULL)
	{
		if(!hUnit->IsBusy() && !hUnit->isDelayedDeath())
		{
			// Builders which are being escorted shouldn't wake up every turn... this is annoying!
			bool bEscortedBuilder = false;
			if(pkMissionNode->eMissionType == CvTypes::getMISSION_BUILD())
			{
				if(hUnit->plot()->getNumDefenders(hUnit->getOwner()) > 0)
				{
					bEscortedBuilder = true;
				}
			}

			bool bAbortMission = !hUnit->IsCombatUnit() && !bEscortedBuilder && hUnit->SentryAlert();
			if(bAbortMission)
			{
				hUnit->ClearMissionQueue();
			}
			else
			{
				if(hUnit->GetActivityType() == ACTIVITY_MISSION)
				{
					ContinueMission(hUnit);
				}
				else
				{
					StartMission(hUnit);
				}
			}
		}
	}

	hUnit->doDelayedDeath();
}

//	---------------------------------------------------------------------------
/// Queue up a new mission
void CvUnitMission::PushMission(CvUnit* hUnit, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
{
	if(CvPreGame::isHuman(hUnit->getOwner()))
	{
		CvAssertMsg(CvUnit::dispatchingNetMessage(), "Multiplayer Error! CvUnit::PushMission invoked for a human player outside of a network message!");
		if(!CvUnit::dispatchingNetMessage())
			gDLL->netMessageDebugLog("*** PROTOCOL ERROR *** : PushMission invoked for a human controlled player outside of a network message!");
	}

	MissionData mission;

	CvAssert(hUnit->getOwner() != NO_PLAYER);

	if(!bAppend)
	{
		//keep the path cache if this is a move mission!
		hUnit->ClearMissionQueue( eMission==CvTypes::getMISSION_MOVE_TO() );
	}

	if(bManual)
	{
		hUnit->SetAutomateType(NO_AUTOMATE);
	}

	// Update Builder Resource info
	if(eMission == CvTypes::getMISSION_BUILD())
	{
		const BuildTypes eBuild = (BuildTypes) iData1;
		if(eBuild != NO_BUILD)
		{
			CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
			if(pkBuildInfo)
			{
					FeatureTypes eFeature = hUnit->plot()->getFeatureType();
					if(eFeature != NO_FEATURE && pkBuildInfo->isFeatureRemove(eFeature) && pkBuildInfo->getFeatureTime(eFeature) > 0)
					{
						// Don't bother looking if this is the build that removes this feature
						if (!pkBuildInfo->isFeatureRemoveOnly(eFeature)) {
						
							// We need to find the build that will remove eFeature.
							CvBuildInfo* pRemoveBuild = NULL;
						
							// Assumes that the BuildFeatures table has an extra column RemoveOnly
							for(int iI = 0; iI < GC.getNumBuildInfos(); iI++) {
								CvBuildInfo* pRemoveBuildInfo = GC.getBuildInfo((BuildTypes) iI);
								if(pRemoveBuildInfo) {
									if(pRemoveBuildInfo->isFeatureRemoveOnly(eFeature)) {
										CvTeamTechs* pTechs = GET_TEAM(GET_PLAYER(hUnit->getOwner()).getTeam()).GetTeamTechs();
										TechTypes eObsoleteTech = (TechTypes) pRemoveBuildInfo->getFeatureObsoleteTech(eFeature);

										if (eObsoleteTech == NO_TECH || !pTechs->HasTech(eObsoleteTech)) {
											TechTypes ePrereqTech = (TechTypes) pRemoveBuildInfo->getFeatureTech(eFeature);
									
											// We have a candidate build for removing this feature
											if (ePrereqTech == NO_TECH) {
												if (pRemoveBuild == NULL) {
													pRemoveBuild = pRemoveBuildInfo;
												}
										}
										else if (pTechs->HasTech(ePrereqTech)) {
												if (pRemoveBuild == NULL) {
													pRemoveBuild = pRemoveBuildInfo;
											}
											else if (GC.getTechInfo(ePrereqTech)->GetGridX() > GC.getTechInfo((TechTypes)pRemoveBuild->getFeatureTech(eFeature))->GetGridX()) {
													pRemoveBuild = pRemoveBuildInfo;
												}
											}
										}
									}
								}
							}
						
							if (pRemoveBuild != NULL) {
								MissionData removeMission;
								removeMission.eMissionType = eMission;
								removeMission.iData1 = pRemoveBuild->GetID();
								removeMission.iData2 = iData2;
								removeMission.iFlags = iFlags;
								removeMission.iPushTurn = GC.getGame().getGameTurn();
							
								hUnit->SetMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);
								InsertAtEndMissionQueue(hUnit, removeMission, !bAppend);
								bAppend = true;
							}
						}
				}


				ImprovementTypes eImprovement = NO_IMPROVEMENT;
				RouteTypes eRoute = NO_ROUTE;

				if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
				{
					eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
				}
				else
				{
					eRoute = (RouteTypes) pkBuildInfo->getRoute();
				}

				int iNumResource = 0;

				// Update the amount of a Resource used up by popped Build
				for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
				{
					iNumResource = 0;

					if(eImprovement != NO_IMPROVEMENT)
					{
						CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
						CvAssert(pkImprovementInfo);
						if(pkImprovementInfo)
						{
							iNumResource += pkImprovementInfo->GetResourceQuantityRequirement(iResourceLoop);
						}
					}
					else if(eRoute != NO_ROUTE)
					{
						CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
						CvAssert(pkRouteInfo);
						if(pkRouteInfo)
						{
							iNumResource += pkRouteInfo->getResourceQuantityRequirement(iResourceLoop);
						}
					}

					if(iNumResource > 0)
					{
						GET_PLAYER(hUnit->getOwner()).changeNumResourceUsed((ResourceTypes) iResourceLoop, iNumResource);
					}
				}
			}
		}
	}

	mission.eMissionType = eMission;
	mission.iData1 = iData1;
	mission.iData2 = iData2;
	mission.iFlags = iFlags;
	mission.iPushTurn = GC.getGame().getGameTurn();

	hUnit->SetMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);

	InsertAtEndMissionQueue(hUnit, mission, !bAppend);

	if(bManual)
	{
		if(hUnit->getOwner() == GC.getGame().getActivePlayer())
		{
			CvMissionInfo* pMissionInfo = GC.getMissionInfo(eMission);

			if(hUnit->IsBusy() && pMissionInfo != NULL && pMissionInfo->isSound())
			{
				hUnit->PlayActionSound();
			}

			GC.GetEngineUserInterface()->setHasMovedUnit(true);
		}

		////gDLL->getEventReporterIFace()->selectionGroupPushMission(this, eMission);
		hUnit->doDelayedDeath();
	}
}

//	---------------------------------------------------------------------------
/// Retrieve next mission
void CvUnitMission::PopMission(CvUnit* hUnit)
{
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	// Update Resource info
	if(hUnit->getBuildType() != NO_BUILD)
	{
		ImprovementTypes eImprovement = NO_IMPROVEMENT;
		RouteTypes eRoute = NO_ROUTE;

		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(hUnit->getBuildType());
		if(pkBuildInfo)
		{
			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
			}
			else if(pkBuildInfo->getRoute() != NO_ROUTE)
			{
				eRoute = (RouteTypes) pkBuildInfo->getRoute();
			}
		}

		int iNumResource;

		// Update the amount of a Resource used up by popped Build
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			iNumResource = 0;

			if(eImprovement != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
				if(pkImprovementInfo)
				{
					iNumResource += pkImprovementInfo->GetResourceQuantityRequirement(iResourceLoop);
				}
			}
			else if(eRoute != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkRouteInfo)
				{
					iNumResource += pkRouteInfo->getResourceQuantityRequirement(iResourceLoop);
				}
			}

			if(iNumResource > 0)
			{
				GET_PLAYER(hUnit->getOwner()).changeNumResourceUsed((ResourceTypes) iResourceLoop, -iNumResource);
			}
		}

		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit));
		gDLL->GameplayUnitWork(pDllUnit.get(), -1);
	}

	MissionData* pTailNode = TailMissionData(hUnit->m_missionQueue);

	if(pTailNode != NULL)
	{
		DeleteMissionData(hUnit, pTailNode);
	}
}

//	---------------------------------------------------------------------------
/// Have a unit wait for another units missions to complete
void CvUnitMission::WaitFor(CvUnit* hUnit, CvUnit* hWaitForUnit)
{
	if(CvPreGame::isHuman(hUnit->getOwner()))
	{
		CvAssertMsg(CvUnit::dispatchingNetMessage(), "Multiplayer Error! CvUnit::PushMission invoked for a human player outside of a network message!");
		if(!CvUnit::dispatchingNetMessage())
			gDLL->netMessageDebugLog("*** PROTOCOL ERROR *** : PushMission invoked for a human controlled player outside of a network message!");
	}

	CvAssert(hUnit->getOwner() != NO_PLAYER);

	MissionData mission;
	mission.eMissionType = CvTypes::getMISSION_WAIT_FOR();
	mission.iData1 = hWaitForUnit->getOwner();
	mission.iData2 = hWaitForUnit->GetID();
	mission.iFlags = 0;
	mission.iPushTurn = GC.getGame().getGameTurn();

	MissionQueue& kQueue = hUnit->m_missionQueue;

	//  Insert head of mission list
	kQueue.insertAtBeginning(&mission);

	CvAssert(kQueue.getLength() < 10);

	if((hUnit->getOwner() == GC.getGame().getActivePlayer()) && hUnit->IsSelected())
	{
		GC.GetEngineUserInterface()->setDirty(Waypoints_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}

//	---------------------------------------------------------------------------
/// Periodic update routine to advance the state of missions
void CvUnitMission::UpdateMission(CvUnit* hUnit)
{
	if (!hUnit)
		return;

	if(hUnit->GetMissionTimer() > 0)
	{
		hUnit->ChangeMissionTimer(-1);

		if(hUnit->GetMissionTimer() == 0)
		{
			if(hUnit->GetActivityType() == ACTIVITY_MISSION)
			{
				ContinueMission(hUnit);
			}
			else
			{
				if(hUnit->getOwner() == GC.getGame().getActivePlayer())
				{
					//if (GC.GetEngineUserInterface()->GetHeadSelectedUnit() == NULL)
					{
						GC.GetEngineUserInterface()->changeCycleSelectionCounter(1);
					}
				}
			}
		}
	}
}

//	---------------------------------------------------------------------------
/// Yes, please hit me again. I like pain.
void CvUnitMission::ContinueMission(CvUnit* hUnit, int iSteps)
{
	bool bContinueMissionRestart = true;	// to make this function no longer recursive
	while(bContinueMissionRestart)
	{
		bContinueMissionRestart = false;

		bool bDone = false;   // are we done with mission?
		bool bAction = false; // are we taking an action this turn?

		CvAssert(!hUnit->isInCombat());
		CvAssert(hUnit->HeadMissionData() != NULL);
		CvAssert(hUnit->getOwner() != NO_PLAYER);
		CvAssert(hUnit->GetActivityType() == ACTIVITY_MISSION);

		if(HeadMissionData(hUnit->m_missionQueue) == NULL)
		{
			// just in case...
			hUnit->SetActivityType(ACTIVITY_AWAKE);
			return;
		}

		CvAssert(iSteps < 100);
		if(iSteps >= 100)
		{
			OutputDebugString("warning: endless loop in ContinueMission\n");
			hUnit->ClearMissionQueue();
			return;
		}

		//tutorial hints
		const MissionData* pkMissionData = (HeadMissionData(hUnit->m_missionQueue));
		if(pkMissionData->iPushTurn == GC.getGame().getGameTurn() || (pkMissionData->iFlags & CvUnit::MOVEFLAG_IGNORE_STACKING))
		{
			if(pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO() && !hUnit->IsDoingPartialMove() && hUnit->canMove() && !hUnit->HasQueuedVisualizationMoves())
			{
				CvPlot* pDestPlot = GC.getMap().plot(pkMissionData->iData1, pkMissionData->iData2);
				if (!pDestPlot)
					return;

				if(hUnit->IsAutomated() && pDestPlot->isVisible(hUnit->getTeam()) && hUnit->canMoveInto(*pDestPlot, CvUnit::MOVEFLAG_ATTACK))
				{
					// if we're automated and try to attack, consider this move OVAH
					bDone = true;
				}
				else
				{
					bool bCityAttackInterrupt = gDLL->GetAdvisorCityAttackInterrupt();
					bool bBadAttackInterrupt = gDLL->GetAdvisorBadAttackInterrupt();
					if(hUnit->isHuman() && !CvPreGame::isNetworkMultiplayerGame() && !GC.getGame().IsCombatWarned() && (bCityAttackInterrupt || bBadAttackInterrupt))
					{
						if(hUnit->canMoveInto(*pDestPlot, CvUnit::MOVEFLAG_ATTACK) && pDestPlot->isVisible(hUnit->getTeam()))
						{
							if(pDestPlot->isCity())
							{
								if(bCityAttackInterrupt)
								{
									GC.GetEngineUserInterface()->SetDontShowPopups(false);

									if(!GC.getGame().isOption(GAMEOPTION_NO_TUTORIAL))
									{
										// do city alert
										CvPopupInfo kPopup(BUTTONPOPUP_ADVISOR_MODAL);
										kPopup.iData1 = ADVISOR_MILITARY;
										kPopup.iData2 = pDestPlot->GetPlotIndex();
										kPopup.iData3 = hUnit->plot()->GetPlotIndex();
										strcpy_s(kPopup.szText, "TXT_KEY_ADVISOR_CITY_ATTACK_BODY");
										kPopup.bOption1 = true;
										GC.GetEngineUserInterface()->AddPopup(kPopup);
										return;
									}
								}
							}
							else if(bBadAttackInterrupt)
							{
								CvUnit* pDefender = pDestPlot->getVisibleEnemyDefender(hUnit->getOwner());
								if(pDefender)
								{
									CombatPredictionTypes ePrediction = GC.getGame().GetCombatPrediction(hUnit, pDefender);
									if(ePrediction == COMBAT_PREDICTION_TOTAL_DEFEAT || ePrediction == COMBAT_PREDICTION_MAJOR_DEFEAT)
									{
										if(!GC.getGame().isOption(GAMEOPTION_NO_TUTORIAL))
										{
											GC.GetEngineUserInterface()->SetDontShowPopups(false);
											CvPopupInfo kPopup(BUTTONPOPUP_ADVISOR_MODAL);
											kPopup.iData1 = ADVISOR_MILITARY;
											kPopup.iData2 = pDestPlot->GetPlotIndex();
											kPopup.iData3 = hUnit->plot()->GetPlotIndex();
											strcpy_s(kPopup.szText, "TXT_KEY_ADVISOR_BAD_ATTACK_BODY");
											kPopup.bOption1 = false;
											GC.GetEngineUserInterface()->AddPopup(kPopup);
											return;
										}

									}
								}
							}
						}
					}
				}
			}
		}

		// extra crash protection, should never happen (but a previous bug in groupAttack was causing a NULL here)
		// while that bug is fixed, no reason to not be a little more careful
		if(HeadMissionData(hUnit->m_missionQueue) == NULL)
		{
			hUnit->SetActivityType(ACTIVITY_AWAKE);
			return;
		}

		const MissionQueue& kMissionQueue = hUnit->m_missionQueue;
		// If there are units in the selection group, they can all move, and we're not done
		//   then try to follow the mission
		if(!bDone && hUnit->canMove() && !hUnit->IsDoingPartialMove())
		{
			const MissionData& kMissionData = *HeadMissionData(kMissionQueue);

			if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_EMBARK() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_DISEMBARK())
			{
				//need to declare war first
				if(hUnit->CheckDOWNeededForMove(pkMissionData->iData1, pkMissionData->iData2))
				{
					hUnit->ClearMissionQueue();
					return;
				}

				if(hUnit->getDomainType() == DOMAIN_AIR)
				{
					int iResult = hUnit->UnitAttackWithMove(pkMissionData->iData1, pkMissionData->iData2, pkMissionData->iFlags);
					if (iResult == CvUnit::MOVE_RESULT_CANCEL)
					{
						//illegal, cannot execute attack
						hUnit->ClearMissionQueue();
						return;
					}
					else if (iResult == CvUnit::MOVE_RESULT_NO_TARGET)
					{
						//illegal, cannot execute attack and aircraft need to use the rebase mission to move
						hUnit->ClearMissionQueue();
						return;
					}
					else if (iResult == CvUnit::MOVE_RESULT_ATTACK)
					{
						//attack executed
						bAction = true;
						bDone = true;
					}
				}
				else
				{
					CvPlot* pDestPlot = GC.getMap().plot(kMissionData.iData1, kMissionData.iData2);
					if (!pDestPlot)
					{
						hUnit->ClearMissionQueue();
						return;
					}

					//only non-air units use the path cache
					if (!hUnit->VerifyCachedPath(pDestPlot, kMissionData.iFlags, INT_MAX))
					{
						hUnit->ClearMissionQueue();
						return;
					}

					int iResult = hUnit->UnitAttackWithMove(kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags);
					if (iResult == CvUnit::MOVE_RESULT_CANCEL)
					{
						//illegal, cannot execute attack
						hUnit->ClearMissionQueue();
						return;
					}
					else if (iResult == CvUnit::MOVE_RESULT_NO_TARGET)
					{
						//nothing to attack, continue movement
						int iResult = hUnit->UnitPathTo(kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags);
						if(iResult >= 0) //normal movement
						{
							bAction = true;
						}
						else if (iResult == CvUnit::MOVE_RESULT_NEXT_TURN)
						{
							bAction = true;
						}
						else //cannot move or no need to move
						{
							bDone = true;
						}
					}
					else if (iResult == CvUnit::MOVE_RESULT_ATTACK)
					{
						//attack executed
						bAction = true;
						bDone = true;
					}
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_ROUTE_TO())
			{
				if(hUnit->UnitRoadTo(kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags))
				{
					bAction = true;
				}
				else
				{
					bDone = true;
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_SWAP_UNITS())
			{
				CvPlot* pTargetPlot = GC.getMap().plot(kMissionData.iData1, kMissionData.iData2);
				if(pTargetPlot != NULL)
				{
					CvPlot* pOriginationPlot = hUnit->plot();

					// Find unit to move out
					for(int iI = 0; iI < pTargetPlot->getNumUnits(); iI++)
					{
						CvUnit* pUnit2 = pTargetPlot->getUnitByIndex(iI);
						if(!pUnit2->AreUnitsOfSameType(*hUnit))
							continue;

						if(pUnit2->ReadyToSwap())
						{
							// Start the swap
							hUnit->ClearPathCache(); //make sure there's no stale path
							pUnit2->ClearPathCache(); //make sure there's no stale path
							if (hUnit->GeneratePath(pTargetPlot, CvUnit::MOVEFLAG_IGNORE_STACKING, 0, NULL, true) && pUnit2->GeneratePath(pOriginationPlot, CvUnit::MOVEFLAG_IGNORE_STACKING, 0, NULL, true))
							{
								//move the new unit in
								int iResult = 0;
								while (iResult >= 0)
									iResult = hUnit->UnitPathTo(pTargetPlot->getX(), pTargetPlot->getY(), CvUnit::MOVEFLAG_IGNORE_STACKING);

								//make sure to delete any previous missions, there's a check later for conflicts
								pUnit2->ClearMissionQueue(true);

								//move the old unit out
								int iResult2 = 0;
								while (iResult2 >= 0)
									iResult2 = pUnit2->UnitPathTo(pOriginationPlot->getX(), pOriginationPlot->getY(), CvUnit::MOVEFLAG_IGNORE_STACKING);

								bAction = true;
								bDone = true;
								break;
							}
						}
					}

					if (!bDone)
					{
						//illegal, cannot execute swap
						hUnit->ClearMissionQueue();
						return;
					}
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT())
			{
				if((hUnit->AI_getUnitAIType() == UNITAI_DEFENSE) && hUnit->plot()->isCity() && (hUnit->plot()->getTeam() == hUnit->getTeam()))
				{
					if(hUnit->plot()->getBestDefender(hUnit->getOwner()) == hUnit)
					{
						bAction = false;
						bDone = true;
						break;
					}
				}
				CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
				if(pTargetUnit)
				{
					if(hUnit->UnitPathTo(pTargetUnit->getX(), pTargetUnit->getY(), kMissionData.iFlags) >= 0)
					{
						bAction = true;
					}
					else
					{
						bDone = true;
					}
				}
				else
				{
					bDone = true;
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_RANGE_ATTACK())
			{
				if(CvUnitCombat::AttackRanged(*hUnit, kMissionData.iData1, kMissionData.iData2, (kMissionData.iFlags &  CvUnit::MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE) != CvUnitCombat::ATTACK_ABORTED)
				{
					bDone = true;
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_NUKE())
			{
				if(CvUnitCombat::AttackNuclear(*hUnit, kMissionData.iData1, kMissionData.iData2, (kMissionData.iFlags &  CvUnit::MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE) != CvUnitCombat::ATTACK_ABORTED)
				{
					bDone = true;
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_BUILD())
			{
				if(!hUnit->UnitBuild((BuildTypes)(kMissionData.iData1)))
				{
					bDone = true;
				}
			}

#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
			else if (MOD_EVENTS_CUSTOM_MISSIONS) {
				int iValue = 0;
				if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_CustomMissionDoStep, hUnit->getOwner(), hUnit->GetID(), kMissionData.eMissionType, kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags, kMissionData.iPushTurn) == GAMEEVENTRETURN_VALUE) {
					if (iValue == CUSTOM_MISSION_ACTION ) {
						bAction = true;
					} else if (iValue == CUSTOM_MISSION_DONE ) {
						bDone = true;
					} else if (iValue == CUSTOM_MISSION_ACTION_AND_DONE ) {
						bAction = true;
						bDone = true;
					}
				}
			}
#endif
		}

		// check to see if mission is done
		if(!bDone && (hUnit->HeadMissionData() != NULL))
		{
			const MissionData& kMissionData = *HeadMissionData(kMissionQueue);

			if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_SWAP_UNITS() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_EMBARK() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_DISEMBARK())
			{
				//in case we loaded a savegame, the cached path is gone. try to regenerate it before cancelling the mission
				CvPlot* pDestPlot = GC.getMap().plot(kMissionData.iData1, kMissionData.iData2);

				//if this fails m_kLastPath will be empty
				//todo: is the non-persistency of the path a problem for reproducability?
				hUnit->VerifyCachedPath(pDestPlot, kMissionData.iFlags, INT_MAX);

				//don't check against the target plot directly in case of approximate pathfinding
				if(hUnit->m_kLastPath.empty())
				{
					bDone = true;
#ifdef LOG_UNIT_MOVES
					std::ostringstream updateMsg;
					updateMsg << "ContinueMission() : player ";
					updateMsg << GET_PLAYER(hUnit->getOwner()).getName();
					updateMsg << hUnit->getName() << " id=" << hUnit->GetID() << " reached " << kMissionData.iData1 << ", " << kMissionData.iData2;
					gDLL->netMessageDebugLog(updateMsg.str());
#endif
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_ROUTE_TO())
			{
				if(hUnit->at(kMissionData.iData1, kMissionData.iData2))
				{
					if(hUnit->GetBestBuildRoute(hUnit->plot()) == NO_ROUTE)
					{
						bDone = true;
					}
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT())
			{
				CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
				if((!pTargetUnit) ||hUnit-> plot() == pTargetUnit->plot())
				{
					bDone = true;
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_SET_UP_FOR_RANGED_ATTACK() ||
					kMissionData.eMissionType == CvTypes::getMISSION_AIRLIFT() ||
					kMissionData.eMissionType == CvTypes::getMISSION_NUKE() ||
					kMissionData.eMissionType == CvTypes::getMISSION_PARADROP() ||
					kMissionData.eMissionType == CvTypes::getMISSION_AIR_SWEEP() ||
					kMissionData.eMissionType == CvTypes::getMISSION_REBASE() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_RANGE_ATTACK() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_PILLAGE() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_FOUND() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_JOIN() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_CONSTRUCT() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_DISCOVER() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_HURRY() ||
					kMissionData.eMissionType == CvTypes::getMISSION_TRADE() ||
					kMissionData.eMissionType == CvTypes::getMISSION_BUY_CITY_STATE() ||
					kMissionData.eMissionType == CvTypes::getMISSION_REPAIR_FLEET() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_SPACESHIP() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_CULTURE_BOMB() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_FOUND_RELIGION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_GOLDEN_AGE() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_LEAD() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_DIE_ANIMATION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_SPREAD_RELIGION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_ENHANCE_RELIGION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_REMOVE_HERESY() ||
					kMissionData.eMissionType == CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE() ||
					kMissionData.eMissionType == CvTypes::getMISSION_PLUNDER_TRADE_ROUTE() ||
					kMissionData.eMissionType == CvTypes::getMISSION_GREAT_WORK() ||
					kMissionData.eMissionType == CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY() ||
					kMissionData.eMissionType == CvTypes::getMISSION_SELL_EXOTIC_GOODS() ||
					kMissionData.eMissionType == CvTypes::getMISSION_GIVE_POLICIES() ||
					kMissionData.eMissionType == CvTypes::getMISSION_ONE_SHOT_TOURISM() ||
#if defined(MOD_BALANCE_CORE)
					kMissionData.eMissionType == CvTypes::getMISSION_FREE_LUXURY() ||
#endif
					kMissionData.eMissionType == CvTypes::getMISSION_CHANGE_ADMIRAL_PORT())
			{
				bDone = true;
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_WAIT_FOR())
			{
				CvUnit* pkWaitingFor = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
				if(!pkWaitingFor || !pkWaitingFor->IsBusy())
				{
					bDone = true;
				}
				else
				{
					// Set the mission timer to 1 so we will get another UpdateMission call
					hUnit->ChangeMissionTimer(1);
				}
			}

#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
			if (MOD_EVENTS_CUSTOM_MISSIONS) {
				if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CustomMissionCompleted, hUnit->getOwner(), hUnit->GetID(), kMissionData.eMissionType, kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags, kMissionData.iPushTurn) == GAMEEVENTRETURN_TRUE) {
					bDone = true;
				}
			}
#endif
		}

		if(HeadMissionData(kMissionQueue) != NULL)
		{
			// if there is an action, if it's done or there are not moves left, and a player is watching, watch the movement
			if(bAction && (bDone || !hUnit->canMove()) && hUnit->plot()->isVisibleToWatchingHuman())
			{
				UpdateMissionTimer(hUnit, iSteps);
			}

			if(bDone)
			{
				if(hUnit->IsWork())
				{
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit));
					gDLL->GameplayUnitWork(pDllUnit.get(), -1);
				}

				if(hUnit->GetMissionTimer() == 0 && !hUnit->isInCombat())	// Was hUnit->IsBusy(), but its ok to clear the mission if the unit is just completing a move visualization
				{
					if(hUnit->getOwner() == GC.getGame().getActivePlayer() && hUnit->IsSelected())
					{
						const MissionData& kMissionData = *HeadMissionData(kMissionQueue);

						if((kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO()) ||
						        (kMissionData.eMissionType == CvTypes::getMISSION_ROUTE_TO()) ||
						        (kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT()))
						{
							// How long does the camera wait before jumping to the next item?
							int iCameraTime;

							if(GET_PLAYER(hUnit->getOwner()).isOption(PLAYEROPTION_QUICK_MOVES))
							{
								iCameraTime = 1;
							}
							// If our move revealed a Plot, camera jumps slower
							else if(GC.GetEngineUserInterface()->IsSelectedUnitRevealingNewPlots())
							{
								iCameraTime = 10;
							}
							// No plots revealed by this move, go quicker
							else
							{
								iCameraTime = 5;
							}

							GC.GetEngineUserInterface()->changeCycleSelectionCounter(iCameraTime);
						}
#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
						if (MOD_EVENTS_CUSTOM_MISSIONS) {
							int iCameraTime = 0;
							if (GAMEEVENTINVOKE_VALUE(iCameraTime, GAMEEVENT_CustomMissionCameraTime, hUnit->getOwner(), hUnit->GetID(), kMissionData.eMissionType, kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags, kMissionData.iPushTurn) == GAMEEVENTRETURN_VALUE) {
								if (iCameraTime > 0 && iCameraTime <= 10) {
									if(GET_PLAYER(hUnit->getOwner()).isOption(PLAYEROPTION_QUICK_MOVES)) {
										iCameraTime = 1;
									}

									GC.GetEngineUserInterface()->changeCycleSelectionCounter(iCameraTime);
								}
							}
						}
#endif
					}

					hUnit->PublishQueuedVisualizationMoves();

					DeleteMissionData(hUnit, HeadMissionData(hUnit->m_missionQueue));
				}
			}
			else
			{
				// if we can still act, process the mission again
				if(hUnit->canMove() && !hUnit->IsDoingPartialMove())
				{
					iSteps++;
					bContinueMissionRestart = true;	// keep looping
				}
				else if(!hUnit->IsBusy() && hUnit->getOwner() == GC.getGame().getActivePlayer() && hUnit->IsSelected())
				{
					GC.GetEngineUserInterface()->changeCycleSelectionCounter(1);
				}
			}
		}
	}
}

//	---------------------------------------------------------------------------
/// Eligible to start a new mission?
bool CvUnitMission::CanStartMission(CvUnit* hUnit, int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible)
{
	CvUnit* pTargetUnit;

	if(hUnit->IsBusy())
	{
		return false;
	}

	// Prevented by scripting?
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(hUnit->getOwner());
		args->Push(hUnit->GetID());
		args->Push(iMission);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, "CanStartMission", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	if(pPlot == NULL)
	{
		pPlot = hUnit->plot();
	}

	if(iMission == CvTypes::getMISSION_MOVE_TO() ||
	        iMission == CvTypes::getMISSION_SWAP_UNITS())
	{
		if (hUnit->IsImmobile() && hUnit->getDomainType() != DOMAIN_AIR)	// If immobile, we can't move... unless we are an air unit.
		{
			return false;
		}

		if(!(pPlot->at(iData1, iData2)))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_ROUTE_TO())
	{
		if(!(pPlot->at(iData1, iData2)) || (hUnit->GetBestBuildRoute(pPlot) != NO_ROUTE))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_MOVE_TO_UNIT())
	{
		CvAssertMsg(iData1 != NO_PLAYER, "iData1 should be a valid Player");
		CvAssertMsg(iData2 != NO_UNIT, "iData2 should be a valid Unit ID");
		if (iData1 != NO_PLAYER && iData2 != NO_UNIT)
		{
			pTargetUnit = GET_PLAYER((PlayerTypes)iData1).getUnit(iData2);

			if (pTargetUnit->IsImmobile())
			{
				return false;
			}

			if((pTargetUnit) && !(pTargetUnit->atPlot(*pPlot)))
			{
				return true;
			}
		}
		else
			return false;
	}
	else if(iMission == CvTypes::getMISSION_SKIP())
	{
		if(hUnit->canHold(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_SLEEP())
	{
		if(hUnit->canSleep(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_FORTIFY())
	{
		if(hUnit->canFortify(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_SET_UP_FOR_RANGED_ATTACK())
	{
		if(hUnit->canSetUpForRangedAttack(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_EMBARK())
	{
		if(hUnit->canEmbarkAtPlot(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_DISEMBARK())
	{
		if(hUnit->canDisembarkAtPlot(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_AIRPATROL())
	{
		if(hUnit->canAirPatrol(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_HEAL())
	{
		if(hUnit->canHeal(pPlot, bTestVisible, false))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_ALERT())
	{
		if(hUnit->canSentry(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_AIRLIFT())
	{
		if(hUnit->canAirliftAt(pPlot, iData1, iData2))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_NUKE())
	{
		if(hUnit->canNukeAt(pPlot, iData1, iData2))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_PARADROP())
	{
		if(hUnit->canParadropAt(pPlot, iData1, iData2))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_AIR_SWEEP())
	{
		if(hUnit->canAirSweepAt(iData1, iData2))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_REBASE())
	{
		if(hUnit->canRebaseAt(pPlot, iData1, iData2))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_RANGE_ATTACK())
	{
		if(hUnit->canRangeStrikeAt(iData1, iData2))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_PILLAGE())
	{
		if(hUnit->canPillage(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_FOUND())
	{
		if(hUnit->canFound(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_JOIN())
	{
		if(hUnit->canJoin(pPlot, ((SpecialistTypes)iData1)))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_CONSTRUCT())
	{
		if(hUnit->canConstruct(pPlot, ((BuildingTypes)iData1)))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_DISCOVER())
	{
		if(hUnit->canDiscover(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_HURRY())
	{
		//if (hUnit->IsCanRushBuilding(pPlot->getPlotCity(), bTestVisible))
		if(hUnit->canHurry(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_TRADE())
	{
		if(hUnit->canTrade(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_REPAIR_FLEET())
	{
		if(hUnit->canRepairFleet(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if (iMission == CvTypes::getMISSION_BUY_CITY_STATE())
	{
		if (hUnit->canBuyCityState(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_SPACESHIP())
	{
		if(hUnit->CanBuildSpaceship(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_CULTURE_BOMB())
	{
		if(hUnit->CanCultureBomb(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_FOUND_RELIGION())
	{
		if(hUnit->CanFoundReligion(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_SPREAD_RELIGION())
	{
		if(hUnit->CanSpreadReligion(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_ENHANCE_RELIGION())
	{
		if(hUnit->CanEnhanceReligion(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_REMOVE_HERESY())
	{
		if(hUnit->CanRemoveHeresy(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_GOLDEN_AGE())
	{
		//this means to play the animation only
		if(iData1 != -1)
		{
			return true;
		}

		if(hUnit->canGoldenAge(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_BUILD())
	{
		CvAssertMsg(((BuildTypes)iData1) < GC.getNumBuildInfos(), "Invalid Build");
		if(hUnit->canBuild(pPlot, ((BuildTypes)iData1), bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_LEAD())
	{
		if(hUnit->canLead(pPlot, iData1))
		{
			return true;
		}
	}
	else if (iMission == CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE())
	{
		if (hUnit->canMakeTradeRoute(pPlot))
		{
			return true;
		}
	}
	else if (iMission == CvTypes::getMISSION_PLUNDER_TRADE_ROUTE())
	{
		if (hUnit->canPlunderTradeRoute(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if (iMission == CvTypes::getMISSION_GREAT_WORK())
	{
		if (hUnit->canCreateGreatWork(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if (iMission == CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY())
	{
		if (hUnit->canChangeTradeUnitHomeCity(pPlot))
		{
			return true;
		}
	}
	else if (iMission == CvTypes::getMISSION_SELL_EXOTIC_GOODS())
	{
		if (hUnit->canSellExoticGoods(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_GIVE_POLICIES())
	{
		//this means to play the animation only
		if(iData1 != -1)
		{
			return true;
		}

		if(hUnit->canGivePolicies(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_ONE_SHOT_TOURISM())
	{
		//this means to play the animation only
		if(iData1 != -1)
		{
			return true;
		}

		if(hUnit->canBlastTourism(pPlot, bTestVisible))
		{
			return true;
		}
	}
	else if (iMission == CvTypes::getMISSION_CHANGE_ADMIRAL_PORT())
	{
		if (hUnit->canChangeAdmiralPort(pPlot))
		{
			return true;
		}
	}
#if defined(MOD_BALANCE_CORE)
	else if (iMission == CvTypes::getMISSION_FREE_LUXURY())
	{
		if (hUnit->canGetFreeLuxury())
		{
			return true;
		}
	}
#endif
#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
	if (MOD_EVENTS_CUSTOM_MISSIONS) {
		if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CustomMissionPossible, hUnit->getOwner(), hUnit->GetID(), iMission, iData1, iData2, 0, -1, pPlot->getX(), pPlot->getY(), bTestVisible) == GAMEEVENTRETURN_TRUE) {
			return true;
		}
	}
#endif

	return false;
}

//	---------------------------------------------------------------------------
/// Initiate a mission
void CvUnitMission::StartMission(CvUnit* hUnit)
{
	bool bDelete;
	bool bAction;
	bool bNotify;

	static int stackDepth = 0;
	++stackDepth; // JAR debugging

	CvAssert(stackDepth < 100);

	CvAssert(!hUnit->IsBusy());
	CvAssert(hUnit->getOwner() != NO_PLAYER);
	CvAssert(hUnit->HeadMissionData() != NULL);

	CvPlayerAI& kUnitOwner = GET_PLAYER(hUnit->getOwner());

	if(!kUnitOwner.isSimultaneousTurns())
	{
		if(!kUnitOwner.isTurnActive())
		{
			if(hUnit->getOwner() == GC.getGame().getActivePlayer())
			{
				if(hUnit->IsSelected())
				{
					GC.GetEngineUserInterface()->changeCycleSelectionCounter(1);
				}
			}

			--stackDepth; // JAR debugging
			return;
		}
	}

	if(hUnit->canMove())
	{
		hUnit->SetActivityType(ACTIVITY_MISSION);
	}
	else
	{
		hUnit->SetActivityType(ACTIVITY_HOLD);
	}

	bDelete = false;
	bAction = false;
	bNotify = false;

	const MissionData* pkQueueData = GetHeadMissionData(hUnit);
	if(!hUnit->CanStartMission(pkQueueData->eMissionType, pkQueueData->iData1, pkQueueData->iData2, hUnit->plot()))
	{
		bDelete = true;
	}
	else
	{
		CvAssertMsg(kUnitOwner.isTurnActive() || kUnitOwner.isHuman(), "It's expected that either the turn is active for this player or the player is human");

		if(pkQueueData->eMissionType == CvTypes::getMISSION_SKIP())
		{
			hUnit->SetActivityType(ACTIVITY_HOLD);
			bDelete = true;
		}

		else if(pkQueueData->eMissionType == CvTypes::getMISSION_SLEEP())
		{
			hUnit->SetActivityType(ACTIVITY_SLEEP);
			bNotify = true;
			bDelete = true;
		}

		else if(pkQueueData->eMissionType == CvTypes::getMISSION_FORTIFY())
		{
			hUnit->SetActivityType(ACTIVITY_SLEEP);
			bNotify = true;
			bDelete = true;
		}

		else if(pkQueueData->eMissionType == CvTypes::getMISSION_AIRPATROL())
		{
			hUnit->SetActivityType(ACTIVITY_INTERCEPT);
			//make sure it's immediately included in the list of interceptors!
			GET_PLAYER(hUnit->getOwner()).UpdateAreaEffectUnit(hUnit);
			bDelete = true;
		}

		else if(pkQueueData->eMissionType == CvTypes::getMISSION_HEAL())
		{
			hUnit->SetActivityType(ACTIVITY_HEAL);
			bNotify = true;
			bDelete = true;
		}

		else if(pkQueueData->eMissionType == CvTypes::getMISSION_ALERT())
		{
			hUnit->SetActivityType(ACTIVITY_SENTRY);
			bNotify = true;
			bDelete = true;
		}

#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
		if (MOD_EVENTS_CUSTOM_MISSIONS) {
			int iValue = 0;
			if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_CustomMissionSetActivity, hUnit->getOwner(), hUnit->GetID(), pkQueueData->eMissionType, pkQueueData->iData1, pkQueueData->iData2, pkQueueData->iFlags, pkQueueData->iPushTurn) == GAMEEVENTRETURN_VALUE) {
				if (iValue == CUSTOM_MISSION_ACTION ) {
					bNotify = true;
				} else if (iValue == CUSTOM_MISSION_DONE ) {
					bDelete = true;
				} else if (iValue == CUSTOM_MISSION_ACTION_AND_DONE ) {
					bNotify = true;
					bDelete = true;
				}
			}
		}
#endif

		if(bNotify)
		{
			// The entity should not futz with the missions, but...
			CvAssert(GetHeadMissionData(hUnit) == pkQueueData);
			pkQueueData = GetHeadMissionData(hUnit);
		}

		if(hUnit->canMove())
		{
			if (pkQueueData->eMissionType == CvTypes::getMISSION_FORTIFY() ||
				pkQueueData->eMissionType == CvTypes::getMISSION_HEAL() ||
				pkQueueData->eMissionType == CvTypes::getMISSION_ALERT() ||
				pkQueueData->eMissionType == CvTypes::getMISSION_SKIP() )
			{
				//start the animation right now to give feedback to the player
				if (!hUnit->IsFortified() && !hUnit->hasMoved() && hUnit->canFortify(hUnit->plot()))
					hUnit->triggerFortifyAnimation(true);
			}
			else if (hUnit->IsFortified())
			{
				// unfortify for any other mission
				hUnit->triggerFortifyAnimation(false);
			}

			// ---------- now the real missions with action -----------------------

			if( pkQueueData->eMissionType == CvTypes::getMISSION_MOVE_TO() ||
				pkQueueData->eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT() ||
				pkQueueData->eMissionType == CvTypes::getMISSION_ROUTE_TO())
			{
				//make sure the path cache is current (not for air units, their movement is actually an airstrike)
				CvPlot* pDestPlot = GC.getMap().plot(pkQueueData->iData1, pkQueueData->iData2);
				if (hUnit->getDomainType()!=DOMAIN_AIR && !hUnit->GeneratePath(pDestPlot, pkQueueData->iFlags, INT_MAX, NULL, true))
				{
					//uh? problem ... abort mission
					bDelete = true;
				}

				if(pkQueueData->eMissionType == CvTypes::getMISSION_ROUTE_TO())
				{
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit));
					gDLL->GameplayUnitWork(pDllUnit.get(), 0);
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_SET_UP_FOR_RANGED_ATTACK())
			{
				hUnit->setSetUpForRangedAttack(true);
				bAction = true;
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_EMBARK())
			{
				bAction = true;
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_DISEMBARK())
			{
				bAction = true;
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_AIRLIFT())
			{
				if(hUnit->airlift(pkQueueData->iData1, pkQueueData->iData2))
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_NUKE())
			{
				MissionData& kMissionData = *hUnit->HeadMissionData();
				if(GC.getMap().plot(kMissionData.iData1, kMissionData.iData2) == NULL || !hUnit->canNukeAt(hUnit->plot(), kMissionData.iData1, kMissionData.iData2))
				{
					// Invalid, delete the mission
					bDelete = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_PARADROP())
			{
				if(hUnit->paradrop(pkQueueData->iData1, pkQueueData->iData2))
				{
					bAction = true;
					// The Paradrop needs to have GameplayUnitMissionEnd, so if no mission timer will be started, do it now.
					if (hUnit->plot()->isActiveVisible() && (!hUnit->isHuman() || !hUnit->plot()->isVisibleToWatchingHuman() || CalculateMissionTimer(hUnit) == 0))
					{
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit));
						gDLL->GameplayUnitMissionEnd(pDllUnit.get());
					}
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_AIR_SWEEP())
			{
				if(hUnit->airSweep(pkQueueData->iData1, pkQueueData->iData2))
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_REBASE())
			{
				if(hUnit->rebase(pkQueueData->iData1, pkQueueData->iData2))
				{
					bAction = true;
					// The Rebase needs to have GameplayUnitMissionEnd, so if no mission timer will be started, do it now.
					if (hUnit->plot()->isActiveVisible() && (!hUnit->isHuman() || !hUnit->plot()->isVisibleToWatchingHuman() || CalculateMissionTimer(hUnit) == 0))
					{
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit));
						gDLL->GameplayUnitMissionEnd(pDllUnit.get());
					}
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_RANGE_ATTACK())
			{
				MissionData& kMissionData = *hUnit->HeadMissionData();
				if(GC.getMap().plot(kMissionData.iData1, kMissionData.iData2) == NULL || !hUnit->canRangeStrikeAt(kMissionData.iData1, kMissionData.iData2))
				{
					// Invalid, delete the mission
					bDelete = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_PILLAGE())
			{
				if(hUnit->pillage())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_FOUND())
			{
				if(hUnit->found())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_JOIN())
			{
				if(hUnit->join((SpecialistTypes)(hUnit->HeadMissionData()->iData1)))
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_CONSTRUCT())
			{
				if(hUnit->construct((BuildingTypes)(hUnit->HeadMissionData()->iData1)))
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_DISCOVER())
			{
				if(hUnit->discover())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_HURRY())
			{
				//if (hUnit->DoRushBuilding())
				if(hUnit->hurry())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_TRADE())
			{
				if(hUnit->trade())
				{
					bAction = true;
				}
			}

			else if (pkQueueData->eMissionType == CvTypes::getMISSION_BUY_CITY_STATE())
			{
				if (hUnit->buyCityState())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_REPAIR_FLEET())
			{
				if(hUnit->repairFleet())
				{
					bAction = true;
				}
			}
			
			else if(pkQueueData->eMissionType == CvTypes::getMISSION_SPACESHIP())
			{
				if(hUnit->DoBuildSpaceship())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_CULTURE_BOMB())
			{
				if(hUnit->DoCultureBomb())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_FOUND_RELIGION())
			{
				if(hUnit->DoFoundReligion())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_SPREAD_RELIGION())
			{
				if(hUnit->DoSpreadReligion())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_ENHANCE_RELIGION())
			{
				if(hUnit->DoEnhanceReligion())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_REMOVE_HERESY())
			{
				if(hUnit->DoRemoveHeresy())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_GOLDEN_AGE())
			{
				//just play animation, not golden age - JW
				if(hUnit->HeadMissionData()->iData1 != -1)
				{
					bAction = true;
				}
				else
				{
					if(hUnit->goldenAge())
					{
						bAction = true;
					}
				}
			}

			else if (pkQueueData->eMissionType == CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE())
			{
				CvPlot* pPlot = GC.getMap().plotByIndex(pkQueueData->iData1);
				CvAssertMsg(pPlot, "pPlot is null! OH NOES, JOEY!");
				if (pPlot)
				{
					if (GC.getGame().isNetworkMultiPlayer())
					{
						// This should fix TR/cargo unit related desyncs (different paths etc). I suspect there is still the potential for desyncs if the user opens the TR popup but something changes before they issue the command. Even just opening the popup could cause problems. I think a net message is in order for a real fix.
						GC.getGame().GetGameTrade()->InvalidateTradePathCache(hUnit->getOwner()); // although we are only interested in one trade route, we invalidate all since the originating client has updated their whole cache when opening the popup
					}

					if(hUnit->makeTradeRoute(pPlot->getX(), pPlot->getY(), (TradeConnectionType)pkQueueData->iData2))
					{
						bAction = true;
					}
				}
			}
	
			else if (pkQueueData->eMissionType == CvTypes::getMISSION_PLUNDER_TRADE_ROUTE())
			{
				if (hUnit->plunderTradeRoute())
				{
					bAction = true;
				}
			}

			else if (pkQueueData->eMissionType == CvTypes::getMISSION_GREAT_WORK())
			{
				if (hUnit->createGreatWork())
				{
					bAction = true;
				}
			}

			else if (pkQueueData->eMissionType == CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY())
			{
				if(hUnit->changeTradeUnitHomeCity(pkQueueData->iData1, pkQueueData->iData2))
				{
					bAction = true;
				}
			}

			else if (pkQueueData->eMissionType == CvTypes::getMISSION_SELL_EXOTIC_GOODS())
			{
				if (hUnit->sellExoticGoods())
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_BUILD())
			{
				BuildTypes currentBuild = (BuildTypes)(hUnit->HeadMissionData()->iData1);
				// Gold cost for Improvement construction
				kUnitOwner.GetTreasury()->ChangeGold(-kUnitOwner.getBuildCost(hUnit->plot(),currentBuild));

				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit));
				gDLL->GameplayUnitWork(pDllUnit.get(),currentBuild);
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_LEAD())
			{
				if(hUnit->lead(hUnit->HeadMissionData()->iData1))
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_DIE_ANIMATION())
			{
				bAction = true;
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_GIVE_POLICIES())
			{
				//just play animation, not golden age - JW
				if(hUnit->HeadMissionData()->iData1 != -1)
				{
					bAction = true;
				}
				else
				{
					if(hUnit->givePolicies())
					{
						bAction = true;
					}
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_ONE_SHOT_TOURISM())
			{
				//just play animation, not golden age - JW
				if(hUnit->HeadMissionData()->iData1 != -1)
				{
					bAction = true;
				}
				else
				{
					if(hUnit->blastTourism())
					{
						bAction = true;
					}
				}
			}

			else if (pkQueueData->eMissionType == CvTypes::getMISSION_CHANGE_ADMIRAL_PORT())
			{
				if(hUnit->changeAdmiralPort(pkQueueData->iData1, pkQueueData->iData2))
				{
					bAction = true;
				}
			}
#if defined(MOD_BALANCE_CORE)
			else if (pkQueueData->eMissionType == CvTypes::getMISSION_FREE_LUXURY())
			{
				if(hUnit->createFreeLuxury())
				{
					bAction = true;
				}
			}
#endif

#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
			if (MOD_EVENTS_CUSTOM_MISSIONS) {
				int iValue = 0;
				if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_CustomMissionStart, hUnit->getOwner(), hUnit->GetID(), pkQueueData->eMissionType, pkQueueData->iData1, pkQueueData->iData2, pkQueueData->iFlags, pkQueueData->iPushTurn) == GAMEEVENTRETURN_VALUE) {
					if (iValue == CUSTOM_MISSION_ACTION) {
						bAction = true;
					}
				}
			}
#endif
		}
	}

	if(hUnit->HeadMissionData() != NULL)
	{
		if(bAction)
		{
			if(hUnit->isHuman())
			{
				if(hUnit->plot()->isVisibleToWatchingHuman())
				{
					UpdateMissionTimer(hUnit);
				}
			}
		}

		if(!hUnit->IsBusy())
		{
			if(bDelete)
			{
				if(hUnit->getOwner() == GC.getGame().getActivePlayer())
				{
					if(hUnit->IsSelected())
					{
						GC.GetEngineUserInterface()->changeCycleSelectionCounter(kUnitOwner.isOption(PLAYEROPTION_QUICK_MOVES)? 1 : 2);
					}
				}

				DeleteMissionData(hUnit, hUnit->HeadMissionData());
			}
			else if(hUnit->GetActivityType() == ACTIVITY_MISSION)
			{
				ContinueMission(hUnit);
			}
		}
	}
	--stackDepth;
}

//	---------------------------------------------------------------------------
/// Where does this mission end?
CvPlot* CvUnitMission::LastMissionPlot(CvUnit* hUnit)
{
	const MissionData* pMissionNode = TailMissionData(hUnit->m_missionQueue);

	while(pMissionNode != NULL)
	{
		if(pMissionNode->eMissionType == CvTypes::getMISSION_MOVE_TO() ||
		        pMissionNode->eMissionType == CvTypes::getMISSION_ROUTE_TO() ||
		        pMissionNode->eMissionType == CvTypes::getMISSION_SWAP_UNITS())
		{
			return GC.getMap().plot(pMissionNode->iData1, pMissionNode->iData2);
		}

		else if(pMissionNode->eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT())
		{
			CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)pMissionNode->iData1).getUnit(pMissionNode->iData2);
			if(pTargetUnit)
			{
				return pTargetUnit->plot();
			}
		}

#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
		if (MOD_EVENTS_CUSTOM_MISSIONS) {
			int iPlotIndex = -1;
			if (GAMEEVENTINVOKE_VALUE(iPlotIndex, GAMEEVENT_CustomMissionTargetPlot, hUnit->getOwner(), hUnit->GetID(), pMissionNode->eMissionType, pMissionNode->iData1, pMissionNode->iData2, pMissionNode->iFlags, pMissionNode->iPushTurn) == GAMEEVENTRETURN_VALUE) {
				if (iPlotIndex >= 0 ) {
					CvPlot* pPlot = GC.getMap().plotByIndex(iPlotIndex);

					if (pPlot) {
						return pPlot;
					}
				}
			}
		}
#endif

		pMissionNode = PrevMissionData(hUnit->m_missionQueue, pMissionNode);
	}

	return hUnit->plot();
}

//	---------------------------------------------------------------------------
//	Update the mission timer to a new value based on the mission (or lack thereof) in the queue
//	KWG: The mission timer controls when the next time the unit's mission will be checked, not
//	     in absolute time, but in passes through the Game Core update loop.  Previously,
//       this was used to delay processing so that the user could see the visualization of
//		 units.  The Game Core no longer deals with visualization timing, but this system is
//		 still used to keep the units sequencing their missions with each other.
//		 i.e. each unit will get a chance to complete a mission segment, rather than a unit
//		 exhausting its mission queue all in one go.
int CvUnitMission::CalculateMissionTimer(CvUnit* hUnit, int iSteps)
{
	CvUnit* pTargetUnit;
	CvPlot* pTargetPlot;
	int iTime = 0;

	MissionData* pkMissionNode;
	if(!hUnit->isHuman())
	{
		iTime = 0;
	}
	else if((pkMissionNode = HeadMissionData(hUnit->m_missionQueue)) != NULL)
	{
		MissionData& kMissionData = *pkMissionNode;

		iTime = 1;

		if((kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO()) ||
		        (kMissionData.eMissionType == CvTypes::getMISSION_ROUTE_TO()) ||
		        (kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT()))
		{
			if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT())
			{
				pTargetUnit = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
				if(pTargetUnit)
				{
					pTargetPlot = pTargetUnit->plot();
				}
				else
				{
					pTargetPlot = NULL;
				}
			}
			else
			{
				pTargetPlot = GC.getMap().plot(kMissionData.iData1, kMissionData.iData2);
			}

			if(pTargetPlot && hUnit->atPlot(*pTargetPlot))
			{
				iTime += iSteps;
			}
			else
			{
				iTime = std::min(iTime, 2);
			}
		}
#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
		if (MOD_EVENTS_CUSTOM_MISSIONS) {
			int iValue = 0;
			if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_CustomMissionTimerInc, hUnit->getOwner(), hUnit->GetID(), kMissionData.eMissionType, kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags, kMissionData.iPushTurn) == GAMEEVENTRETURN_VALUE) {
				if (iValue != 0) {
					iTime += iValue;
				}
			}
		}
#endif

		if(hUnit->isHuman() && (hUnit->IsAutomated() /*|| (GET_PLAYER((GC.getGame().isNetworkMultiPlayer()) ? hUnit->getOwner() : GC.getGame().getActivePlayer()).isOption(PLAYEROPTION_QUICK_MOVES))*/))
		{
			iTime = std::min(iTime, 1);
		}
	}
	else
	{
		iTime = 0;
	}

	return iTime;
}

//	---------------------------------------------------------------------------
void CvUnitMission::UpdateMissionTimer(CvUnit* hUnit, int iSteps)
{
	hUnit->SetMissionTimer(CalculateMissionTimer(hUnit, iSteps));
}

//	---------------------------------------------------------------------------
/// Retrieve a specified mission index in the queue.  Can return NULL
const MissionData* CvUnitMission::GetMissionData(CvUnit* hUnit, int iNode)
{
	int iCount = 0;

	const MissionQueue& kQueue = hUnit->m_missionQueue;
	const MissionData* pMissionNode = HeadMissionData(kQueue);

	while(pMissionNode != NULL)
	{
		if(iNode == iCount)
		{
			return pMissionNode;
		}

		iCount++;

		pMissionNode = NextMissionData(kQueue, pMissionNode);
	}

	return NULL;
}

//	---------------------------------------------------------------------------
/// Push onto back end of mission queue
void CvUnitMission::InsertAtEndMissionQueue(CvUnit* hUnit, MissionData mission, bool bStart)
{
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	MissionQueue& kQueue = hUnit->m_missionQueue;
	kQueue.insertAtEnd(&mission);

	CvAssert(kQueue.getLength() < 10);

	if((GetLengthMissionQueue(kQueue) == 1) && bStart)
	{
		ActivateHeadMission(hUnit);
	}

	if((hUnit->getOwner() == GC.getGame().getActivePlayer()) && hUnit->IsSelected())
	{
		
		GC.GetEngineUserInterface()->setDirty(Waypoints_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}

//	---------------------------------------------------------------------------
/// Delete a specific mission from queue
MissionData* CvUnitMission::DeleteMissionData(CvUnit* hUnit, MissionData* pNode)
{
	MissionData* pNextMissionNode;

	CvAssertMsg(pNode != NULL, "Node is not assigned a valid value");
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	MissionQueue& kQueue = hUnit->m_missionQueue;

	if(pNode == HeadMissionData(kQueue))
	{
		DeactivateHeadMission(hUnit, /*iUnitCycleTimer*/ 1);
	}

	pNextMissionNode = kQueue.deleteNode(pNode);
	if(pNextMissionNode == NULL)
		hUnit->ClearPathCache();

	if(pNextMissionNode == HeadMissionData(kQueue))
	{
		ActivateHeadMission(hUnit);
	}

	if((hUnit->getOwner() == GC.getGame().getActivePlayer()) && hUnit->IsSelected())
	{
		GC.GetEngineUserInterface()->setDirty(Waypoints_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	}

	return pNextMissionNode;
}

//	---------------------------------------------------------------------------
/// Clear all queued missions
void CvUnitMission::ClearMissionQueue(CvUnit* hUnit, bool bKeepPathCache, int iUnitCycleTimerOverride)
{
	//VALIDATE_OBJECT
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	DeactivateHeadMission(hUnit, iUnitCycleTimerOverride);

	while(hUnit->m_missionQueue.getLength() > 0)
	{
		PopMission(hUnit);
	}

	if (!bKeepPathCache)
	{
		hUnit->ClearPathCache();
	}

	if((hUnit->getOwner() == GC.getGame().getActivePlayer()) && hUnit->IsSelected())
	{
		GC.GetEngineUserInterface()->setDirty(Waypoints_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}

//	---------------------------------------------------------------------------
/// Start our first mission
void CvUnitMission::ActivateHeadMission(CvUnit* hUnit)
{
	VALIDATE_OBJECT
	CvAssert(hUnit->getOwner() != NO_PLAYER);
	if(hUnit->GetLengthMissionQueue() != 0)
	{
		if(!hUnit->IsBusy())
		{
			StartMission(hUnit);
		}
	}
}

//	---------------------------------------------------------------------------
/// Deactivate our first mission, waking up the unit
void CvUnitMission::DeactivateHeadMission(CvUnit* hUnit, int iUnitCycleTimer)
{
	VALIDATE_OBJECT
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	if(hUnit->GetLengthMissionQueue() != 0)
	{
		if(hUnit->GetActivityType() == ACTIVITY_MISSION)
		{
			hUnit->SetActivityType(ACTIVITY_AWAKE);
		}

		hUnit->SetMissionTimer(0);

		if(hUnit->getOwner() == GC.getGame().getActivePlayer())
		{
			if(hUnit->IsSelected())
			{
				if(GET_PLAYER(hUnit->getOwner()).isOption(PLAYEROPTION_QUICK_MOVES))
					iUnitCycleTimer = min(1, iUnitCycleTimer);

				GC.GetEngineUserInterface()->changeCycleSelectionCounter(iUnitCycleTimer);
			}
		}
	}
}

//	---------------------------------------------------------------------------
/// Retrieve the mission after a specific one
const MissionData* CvUnitMission::NextMissionData(const MissionQueue& kQueue, const MissionData* pNode)
{
	return kQueue.next(pNode);
}

//	---------------------------------------------------------------------------
/// Retrieve the mission before a specific one
const MissionData* CvUnitMission::PrevMissionData(const MissionQueue& kQueue, const MissionData* pNode)
{
	return kQueue.prev(pNode);
}

//	---------------------------------------------------------------------------
/// Retrieve the first mission in the queue (const correct version)
const MissionData* CvUnitMission::HeadMissionData(const MissionQueue& kQueue)
{
	return kQueue.head();
}

//	---------------------------------------------------------------------------
/// Retrieve the first mission in the queue
MissionData* CvUnitMission::HeadMissionData(MissionQueue& kQueue)
{
	return kQueue.head();
}

//	---------------------------------------------------------------------------
/// Retrieve the last mission in the queue (const correct version)
const MissionData* CvUnitMission::TailMissionData(const MissionQueue& kQueue)
{
	return ((MissionQueue&)kQueue).tail();
}

//	---------------------------------------------------------------------------
/// Retrieve the last mission in the queue
MissionData* CvUnitMission::TailMissionData(MissionQueue& kQueue)
{
	return kQueue.tail();
}

//	---------------------------------------------------------------------------
/// Retrieve a mission from the queue by index
MissionData* CvUnitMission::GetMissionFromQueue(MissionQueue& kQueue, int iIndex)
{
	return kQueue.getAt(iIndex);
}

//	---------------------------------------------------------------------------
/// Get the length of the mission queue
int	CvUnitMission::GetLengthMissionQueue(const MissionQueue& kQueue)
{
	return kQueue.getLength();
}

//	---------------------------------------------------------------------------
const MissionData* CvUnitMission::GetHeadMissionData(CvUnit* hUnit)
{
	CvAssert(hUnit != NULL);
	if(hUnit->m_missionQueue.getLength())
		return (hUnit->m_missionQueue.head());
	return NULL;
}

//	---------------------------------------------------------------------------
const MissionData* CvUnitMission::IsHeadMission(CvUnit* hUnit, int iMission)
{
	CvAssert(hUnit != NULL);
	if(hUnit->m_missionQueue.getLength())
	{
		const MissionData& kMissionData = *hUnit->m_missionQueue.head();
		if(kMissionData.eMissionType == (MissionTypes)iMission)
			return &kMissionData;
	}
	return NULL;
}

//	---------------------------------------------------------------------------
//	Returns true if the is a move mission at the head of the unit queue and it is complete
bool CvUnitMission::HasCompletedMoveMission(CvUnit* hUnit)
{
	MissionData* pkMissionNode;
	if((pkMissionNode = HeadMissionData(hUnit->m_missionQueue)) != NULL)
	{
		MissionData& kMissionData = *pkMissionNode;
		if((kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO()) ||
		        (kMissionData.eMissionType == CvTypes::getMISSION_ROUTE_TO()) ||
		        (kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT()))
		{
			CvPlot* pTargetPlot = NULL;

			if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT())
			{
				CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
				if(pTargetUnit)
				{
					pTargetPlot = pTargetUnit->plot();
				}
				else
				{
					return true;	// Our unit is gone, assume we are done.
				}
			}
			else
			{
				pTargetPlot = GC.getMap().plot(kMissionData.iData1, kMissionData.iData2);
			}

			if(pTargetPlot && hUnit->atPlot(*pTargetPlot))
			{
				return true;
			}
		}
#if defined(MOD_EVENTS_CUSTOM_MISSIONS)
		if (MOD_EVENTS_CUSTOM_MISSIONS) {
			if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CustomMissionCompleted, hUnit->getOwner(), hUnit->GetID(), kMissionData.eMissionType, kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags, kMissionData.iPushTurn) == GAMEEVENTRETURN_TRUE) {
				return true;
			}
		}
#endif
	}

	return false;
}
