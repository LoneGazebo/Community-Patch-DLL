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

//	---------------------------------------------------------------------------
/// Perform automated mission
void CvUnitMission::AutoMission(UnitHandle hUnit)
{
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	const MissionQueueNode* pkMissionNode = HeadMissionQueueNode(hUnit->m_missionQueue);
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

			if(!bEscortedBuilder && !hUnit->IsIgnoringDangerWakeup() && !hUnit->IsCombatUnit() && GET_PLAYER(hUnit->getOwner()).IsPlotUnderImmediateThreat(*(hUnit->plot())))
			{
				hUnit->ClearMissionQueue();
				hUnit->SetIgnoreDangerWakeup(true);
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

	hUnit.ignoreDestruction(true);
	hUnit->doDelayedDeath();
}

//	---------------------------------------------------------------------------
/// Queue up a new mission
void CvUnitMission::PushMission(UnitHandle hUnit, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
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
		hUnit->ClearMissionQueue();
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
				if (GET_PLAYER(hUnit->getOwner()).IsPlotUnderImmediateThreat(*(hUnit->plot())))
				{
					if(hUnit->plot()->getNumDefenders(hUnit->getOwner()) <= 0)
					{
						hUnit->SetIgnoreDangerWakeup(true);
					}
				}

				//if (hUnit->isHuman())
				{
					FeatureTypes eFeature = hUnit->plot()->getFeatureType();
					if(eFeature != NO_FEATURE && pkBuildInfo->isFeatureRemove(eFeature) && pkBuildInfo->getFeatureTime(eFeature) > 0)
					{
						CvFeatureInfo* feature = GC.getFeatureInfo(eFeature);
						MissionData removeMission;
						removeMission.eMissionType = eMission;
						if(iData1 != 15 && strcmp(feature->GetType(), "FEATURE_FOREST") == 0)
						{
							removeMission.iData1 = 15; // todo: future proof this
							removeMission.iData2 = iData2;
							removeMission.iFlags = iFlags;
							removeMission.iPushTurn = GC.getGame().getGameTurn();
							hUnit->SetMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);
							InsertAtEndMissionQueue(hUnit, removeMission, !bAppend);
							bAppend = true;
						}
						else if(iData1 != 14 && strcmp(feature->GetType(), "FEATURE_JUNGLE") == 0)
						{
							removeMission.iData1 = 14; // todo: future proof this
							removeMission.iData2 = iData2;
							removeMission.iFlags = iFlags;
							removeMission.iPushTurn = GC.getGame().getGameTurn();
							hUnit->SetMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);
							InsertAtEndMissionQueue(hUnit, removeMission, !bAppend);
							bAppend = true;
						}
						else if(iData1 != 16 && strcmp(feature->GetType(), "FEATURE_MARSH") == 0)
						{
							removeMission.iData1 = 16; // todo: future proof this
							removeMission.iData2 = iData2;
							removeMission.iFlags = iFlags;
							removeMission.iPushTurn = GC.getGame().getGameTurn();
							hUnit->SetMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);
							InsertAtEndMissionQueue(hUnit, removeMission, !bAppend);
							bAppend = true;
						}
						else if(iData1 != 17 && strcmp(feature->GetType(), "FEATURE_FALLOUT") == 0)
						{
							removeMission.iData1 = 17; // todo: future proof this
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
		hUnit.ignoreDestruction(true);
		hUnit->doDelayedDeath();
	}
}

//	---------------------------------------------------------------------------
/// Retrieve next mission
void CvUnitMission::PopMission(UnitHandle hUnit)
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

		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit.pointer()));
		gDLL->GameplayUnitWork(pDllUnit.get(), -1);
	}

	MissionQueueNode* pTailNode = TailMissionQueueNode(hUnit->m_missionQueue);

	if(pTailNode != NULL)
	{
		DeleteMissionQueueNode(hUnit, pTailNode);
	}
}

//	---------------------------------------------------------------------------
/// Have a unit wait for another units missions to complete
void CvUnitMission::WaitFor(UnitHandle hUnit, UnitHandle hWaitForUnit)
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
void CvUnitMission::UpdateMission(UnitHandle& hUnit)
{
	CvAssert(hUnit->getOwner() != NO_PLAYER);

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
void CvUnitMission::ContinueMission(UnitHandle hUnit, int iSteps, int iETA)
{
	hUnit->ClearPathCache();		// At the start of the continue, clear any cached path.  The cache will be rebuilt and reused while the mission loops in this method

	bool bContinueMissionRestart = true;	// to make this function no longer recursive
	while(bContinueMissionRestart)
	{
		bContinueMissionRestart = false;

		bool bDone = false;   // are we done with mission?
		bool bAction = false; // are we taking an action this turn?

		// slewis - important modification!
		// This function may call the pathfinder multiple times.
		// That can cause partial moves to be impossible in certain circumstances.
		// We need a way to determine if the pathfinder can be used without breaking the currently built path.
		// I added unit flags to accomplish this. (While this flag does not necessarily need to be serialized,
		// the variable is serialized for future proofing.)
		hUnit->m_iFlags = hUnit->m_iFlags | CvUnit::UNITFLAG_EVALUATING_MISSION;

		// slewis - more important info!
		// I replaced all the "return"s in this function with "goto"s.
		// This ensures that every way this function is exited, it always clears out the unit flag evaluation mission.
		// I know gotos are frowned upon, but if anyone else wants to re-write and test this function, be my guest.

		CvAssert(!hUnit->isInCombat());
		CvAssert(hUnit->HeadMissionQueueNode() != NULL);
		CvAssert(hUnit->getOwner() != NO_PLAYER);
		CvAssert(hUnit->GetActivityType() == ACTIVITY_MISSION);

		if(HeadMissionQueueNode(hUnit->m_missionQueue) == NULL)
		{
			// just in case...
			hUnit->SetActivityType(ACTIVITY_AWAKE);
			goto ContinueMissionExit;
		}

		CvAssert(iSteps < 100);
		if(iSteps >= 100)
			goto ContinueMissionExit;

		const MissionData* pkMissionData = (HeadMissionQueueNode(hUnit->m_missionQueue));

		if(pkMissionData->iPushTurn == GC.getGame().getGameTurn() || (pkMissionData->iFlags & MOVE_UNITS_THROUGH_ENEMY))
		{
			if(pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO() && !hUnit->IsDoingPartialMove() && hUnit->canMove() && hUnit->m_unitMoveLocs.size() == 0)
			{
				CvPlot* pPlot = GC.getMap().plot(pkMissionData->iData1, pkMissionData->iData2);
				if(hUnit->IsAutomated() && pPlot->isVisible(hUnit->getTeam()) && hUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_ATTACK))
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
						if(hUnit->canMoveInto(*pPlot, CvUnit::MOVEFLAG_ATTACK) && pPlot->isVisible(hUnit->getTeam()))
						{
							if(pPlot->isCity())
							{
								if(bCityAttackInterrupt)
								{
									GC.GetEngineUserInterface()->SetDontShowPopups(false);

									if(!GC.getGame().isOption(GAMEOPTION_NO_TUTORIAL))
									{
										// do city alert
										CvPopupInfo kPopup(BUTTONPOPUP_ADVISOR_MODAL);
										kPopup.iData1 = ADVISOR_MILITARY;
										kPopup.iData2 = pPlot->GetPlotIndex();
										kPopup.iData3 = hUnit->plot()->GetPlotIndex();
										strcpy_s(kPopup.szText, "TXT_KEY_ADVISOR_CITY_ATTACK_BODY");
										kPopup.bOption1 = true;
										GC.GetEngineUserInterface()->AddPopup(kPopup);
										goto ContinueMissionExit;
									}
								}
							}
							else if(bBadAttackInterrupt)
							{
								CvUnit* pDefender = pPlot->getVisibleEnemyDefender(hUnit->getOwner());
								if(pDefender)
								{
									CombatPredictionTypes ePrediction = GC.getGame().GetCombatPrediction(hUnit.pointer(), pDefender);
									if(ePrediction == COMBAT_PREDICTION_TOTAL_DEFEAT || ePrediction == COMBAT_PREDICTION_MAJOR_DEFEAT)
									{
										if(!GC.getGame().isOption(GAMEOPTION_NO_TUTORIAL))
										{
											GC.GetEngineUserInterface()->SetDontShowPopups(false);
											CvPopupInfo kPopup(BUTTONPOPUP_ADVISOR_MODAL);
											kPopup.iData1 = ADVISOR_MILITARY;
											kPopup.iData2 = pPlot->GetPlotIndex();
											kPopup.iData3 = hUnit->plot()->GetPlotIndex();
											strcpy_s(kPopup.szText, "TXT_KEY_ADVISOR_BAD_ATTACK_BODY");
											kPopup.bOption1 = false;
											GC.GetEngineUserInterface()->AddPopup(kPopup);
											goto ContinueMissionExit;
										}

									}
								}
							}
						}
					}

					if(hUnit->UnitAttack(pkMissionData->iData1, pkMissionData->iData2, pkMissionData->iFlags, iSteps))
					{
						bDone = true;
					}
				}
			}
		}

		// extra crash protection, should never happen (but a previous bug in groupAttack was causing a NULL here)
		// while that bug is fixed, no reason to not be a little more careful
		if(HeadMissionQueueNode(hUnit->m_missionQueue) == NULL)
		{
			hUnit->SetActivityType(ACTIVITY_AWAKE);
			goto ContinueMissionExit;
		}

		const MissionQueue& kMissionQueue = hUnit->m_missionQueue;
		// If there are units in the selection group, they can all move, and we're not done
		//   then try to follow the misision
		if(!bDone && hUnit->canMove() && !hUnit->IsDoingPartialMove())
		{
			const MissionData& kMissionData = *HeadMissionQueueNode(kMissionQueue);

			if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_EMBARK() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_DISEMBARK())
			{
				if(hUnit->getDomainType() == DOMAIN_AIR)
				{
					hUnit->UnitPathTo(kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags);
					bDone = true;
				}
				else
				{
					int iThisETA = hUnit->UnitPathTo(kMissionData.iData1, kMissionData.iData2, kMissionData.iFlags, iETA);
					if(iThisETA > 0)
					{
						bAction = true;
					}
					else
					{
						bDone = true;
					}

					// Save off the initial ETA, we will feed it back into the UnitPathTo so it can check to see if our ETA grows while we are in the loop.
					// This can happen as terrain gets revealed.
					if(iSteps == 0)
					{
						iETA = iThisETA;
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
				CvPlot* pOriginationPlot;
				CvPlot* pTargetPlot;

				// Get target plot
				pTargetPlot = GC.getMap().plot(kMissionData.iData1, kMissionData.iData2);

				if(pTargetPlot != NULL)
				{
					pOriginationPlot = hUnit->plot();

					if(pTargetPlot->getNumUnits() < 1)
					{
						bAction = false;
						bDone = true;
						break;
					}

					// Find unit to move out
					for(int iI = 0; iI < pTargetPlot->getNumUnits(); iI++)
					{
						CvUnit* pUnit2 = pTargetPlot->getUnitByIndex(iI);

						if(pUnit2->AreUnitsOfSameType(*(hUnit)) && pUnit2->ReadyToMove())
						{
							// Start the swap
							hUnit->UnitPathTo(HeadMissionQueueNode(kMissionQueue)->iData1, HeadMissionQueueNode(kMissionQueue)->iData2, MOVE_IGNORE_STACKING);

							// Move the other unit back out
							pUnit2->UnitPathTo(pOriginationPlot->getX(), pOriginationPlot->getY(), 0);
							bDone = true;
						}
					}
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT())
			{
				if((hUnit->AI_getUnitAIType() == UNITAI_DEFENSE) && hUnit->plot()->isCity() && (hUnit->plot()->getTeam() == hUnit->getTeam()))
				{
					if(hUnit->plot()->getBestDefender(hUnit->getOwner()) == hUnit.pointer())
					{
						bAction = false;
						bDone = true;
						break;
					}
				}
				UnitHandle pTargetUnit = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
				if(pTargetUnit)
				{
					if(hUnit->GetMissionAIType() != MISSIONAI_SHADOW && hUnit->GetMissionAIType() != MISSIONAI_GROUP)
					{
						if(!hUnit->plot()->isOwned() || hUnit->plot()->getOwner() == hUnit->getOwner())
						{
							CvPlot* pMissionPlot = pTargetUnit->GetMissionAIPlot();
							if(pMissionPlot != NULL && NO_TEAM != pMissionPlot->getTeam())
							{
								if(pMissionPlot->isOwned() && pTargetUnit->isPotentialEnemy(pMissionPlot->getTeam(), pMissionPlot))
								{
									bAction = false;
									bDone = true;
									break;
								}
							}
						}
					}

					if(hUnit->UnitPathTo(pTargetUnit->getX(), pTargetUnit->getY(), kMissionData.iFlags) > 0)
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
				if(CvUnitCombat::AttackRanged(*hUnit, kMissionData.iData1, kMissionData.iData2, (kMissionData.iFlags &  MISSION_MODIFIER_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE) != CvUnitCombat::ATTACK_ABORTED)
				{
					bDone = true;
				}
			}

			else if(kMissionData.eMissionType == CvTypes::getMISSION_NUKE())
			{
				if(CvUnitCombat::AttackNuclear(*hUnit, kMissionData.iData1, kMissionData.iData2, (kMissionData.iFlags &  MISSION_MODIFIER_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE) != CvUnitCombat::ATTACK_ABORTED)
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
		}

		// check to see if mission is done
		if(!bDone && (hUnit->HeadMissionQueueNode() != NULL))
		{
			const MissionData& kMissionData = *HeadMissionQueueNode(kMissionQueue);

			if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_SWAP_UNITS() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_EMBARK() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_DISEMBARK())
			{
				if(hUnit->at(kMissionData.iData1, kMissionData.iData2))
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
				UnitHandle pTargetUnit = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
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
					kMissionData.eMissionType == CvTypes::getMISSION_REPAIR_FLEET() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_SPACESHIP() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_CULTURE_BOMB() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_FOUND_RELIGION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_GOLDEN_AGE() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_LEAD() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_DIE_ANIMATION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_SPREAD_RELIGION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_ENHANCE_RELIGION() ||
			        kMissionData.eMissionType == CvTypes::getMISSION_REMOVE_HERESY())
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
		}

		if(HeadMissionQueueNode(kMissionQueue) != NULL)
		{
			// if there is an action, if it's done or there are not moves left, and a player is watching, watch the movement
			if(bAction && (bDone || !hUnit->canMove()) && hUnit->plot()->isVisibleToWatchingHuman())
			{
				UpdateMissionTimer(hUnit, iSteps);

				if(hUnit->ShowMoves() && GC.getGame().getActivePlayer() != NO_PLAYER && hUnit->getOwner() != GC.getGame().getActivePlayer() && hUnit->plot()->isActiveVisible(false))
				{
					auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(hUnit->plot());
					GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
				}
			}

			if(bDone)
			{
				if(hUnit->IsWork())
				{
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit.pointer()));
					gDLL->GameplayUnitWork(pDllUnit.get(), -1);
				}

				if(hUnit->GetMissionTimer() == 0 && !hUnit->isInCombat())	// Was hUnit->IsBusy(), but its ok to clear the mission if the unit is just completing a move visualization
				{
					if(hUnit->getOwner() == GC.getGame().getActivePlayer() && hUnit->IsSelected())
					{
						const MissionData& kMissionData = *HeadMissionQueueNode(kMissionQueue);

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
					}

					if(hUnit->m_unitMoveLocs.size() > 0)
					{
						hUnit->PublishQueuedVisualizationMoves();
					}

					DeleteMissionQueueNode(hUnit, HeadMissionQueueNode(hUnit->m_missionQueue));
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
ContinueMissionExit: // goto destination to clean up the flag value
	hUnit->m_iFlags =hUnit-> m_iFlags & ~CvUnit::UNITFLAG_EVALUATING_MISSION;
	GC.getGame().SetCombatWarned(false);
}

//	---------------------------------------------------------------------------
/// Eligible to start a new mission?
bool CvUnitMission::CanStartMission(UnitHandle hUnit, int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible)
{
	UnitHandle pTargetUnit;

	if(hUnit->IsBusy())
	{
		return false;
	}

	if(pPlot == NULL)
	{
		pPlot = hUnit->plot();
	}

	if(iMission == CvTypes::getMISSION_MOVE_TO() ||
	        iMission == CvTypes::getMISSION_SWAP_UNITS())
	{
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
		if(hUnit->canEmbark(pPlot))
		{
			return true;
		}
	}
	else if(iMission == CvTypes::getMISSION_DISEMBARK())
	{
		if(hUnit->canDisembark(pPlot))
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
		if(hUnit->canHeal(pPlot, bTestVisible))
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

	return false;
}

//	---------------------------------------------------------------------------
/// Initiate a mission
void CvUnitMission::StartMission(UnitHandle hUnit)
{
	bool bDelete;
	bool bAction;
	bool bNotify;

	static int stackDepth = 0;
	++stackDepth; // JAR debugging

	CvAssert(stackDepth < 100);

	CvAssert(!hUnit->IsBusy());
	CvAssert(hUnit->getOwner() != NO_PLAYER);
	CvAssert(hUnit->HeadMissionQueueNode() != NULL);

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

	hUnit->ClearPathCache();

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


		if(bNotify)
		{
			// The entity should not futz with the missions, but...
			CvAssert(GetHeadMissionData(hUnit) == pkQueueData);
			pkQueueData = GetHeadMissionData(hUnit);
		}

		if(hUnit->canMove())
		{
			if(pkQueueData->eMissionType == CvTypes::getMISSION_FORTIFY())
			{
				hUnit->SetFortifiedThisTurn(true);
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_HEAL() ||
			        pkQueueData->eMissionType == CvTypes::getMISSION_ALERT())
			{
				hUnit->SetFortifiedThisTurn(true);
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
				MissionData& kMissionData = *hUnit->HeadMissionQueueNode();
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
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit.pointer()));
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
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit.pointer()));
						gDLL->GameplayUnitMissionEnd(pDllUnit.get());
					}
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_RANGE_ATTACK())
			{
				MissionData& kMissionData = *hUnit->HeadMissionQueueNode();
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
				if(hUnit->join((SpecialistTypes)(hUnit->HeadMissionQueueNode()->iData1)))
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_CONSTRUCT())
			{
				if(hUnit->construct((BuildingTypes)(hUnit->HeadMissionQueueNode()->iData1)))
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
				if(hUnit->HeadMissionQueueNode()->iData1 != -1)
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

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_BUILD())
			{
				// Gold cost for Improvement construction
				kUnitOwner.GetTreasury()->ChangeGold(-(kUnitOwner.getBuildCost(hUnit->plot(), (BuildTypes)(hUnit->HeadMissionQueueNode()->iData1))));

				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit.pointer()));
				gDLL->GameplayUnitWork(pDllUnit.get(), (hUnit->HeadMissionQueueNode()->iData1));
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_ROUTE_TO())
			{
				auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(hUnit.pointer()));
				gDLL->GameplayUnitWork(pDllUnit.get(), 0);
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_LEAD())
			{
				if(hUnit->lead(hUnit->HeadMissionQueueNode()->iData1))
				{
					bAction = true;
				}
			}

			else if(pkQueueData->eMissionType == CvTypes::getMISSION_DIE_ANIMATION())
			{
				bAction = true;
			}

		}
	}

	if(hUnit->HeadMissionQueueNode() != NULL)
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

				DeleteMissionQueueNode(hUnit, hUnit->HeadMissionQueueNode());
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
CvPlot* CvUnitMission::LastMissionPlot(UnitHandle hUnit)
{
	const MissionQueueNode* pMissionNode = TailMissionQueueNode(hUnit->m_missionQueue);

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
			UnitHandle pTargetUnit = GET_PLAYER((PlayerTypes)pMissionNode->iData1).getUnit(pMissionNode->iData2);
			if(pTargetUnit)
			{
				return pTargetUnit->plot();
			}
		}

		pMissionNode = PrevMissionQueueNode(hUnit->m_missionQueue, pMissionNode);
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
int CvUnitMission::CalculateMissionTimer(UnitHandle hUnit, int iSteps)
{
	UnitHandle pTargetUnit;
	CvPlot* pTargetPlot;
	int iTime = 0;

	MissionQueueNode* pkMissionNode;
	if(!hUnit->isHuman() && !hUnit->ShowMoves())
	{
		iTime = 0;
	}
	else if((pkMissionNode = HeadMissionQueueNode(hUnit->m_missionQueue)) != NULL)
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
void CvUnitMission::UpdateMissionTimer(UnitHandle hUnit, int iSteps)
{
	hUnit->SetMissionTimer(CalculateMissionTimer(hUnit, iSteps));
}

//	---------------------------------------------------------------------------
/// Retrieve a specified mission index in the queue.  Can return NULL
const MissionData* CvUnitMission::GetMissionData(UnitHandle hUnit, int iNode)
{
	int iCount = 0;

	const MissionQueue& kQueue = hUnit->m_missionQueue;
	const MissionQueueNode* pMissionNode = HeadMissionQueueNode(kQueue);

	while(pMissionNode != NULL)
	{
		if(iNode == iCount)
		{
			return pMissionNode;
		}

		iCount++;

		pMissionNode = NextMissionQueueNode(kQueue, pMissionNode);
	}

	return NULL;
}

//	---------------------------------------------------------------------------
/// Push onto back end of mission queue
void CvUnitMission::InsertAtEndMissionQueue(UnitHandle hUnit, MissionData mission, bool bStart)
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
MissionData* CvUnitMission::DeleteMissionQueueNode(UnitHandle hUnit, MissionData* pNode)
{
	MissionQueueNode* pNextMissionNode;

	CvAssertMsg(pNode != NULL, "Node is not assigned a valid value");
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	MissionQueue& kQueue = hUnit->m_missionQueue;

	if(pNode == HeadMissionQueueNode(kQueue))
	{
		DeactivateHeadMission(hUnit, /*iUnitCycleTimer*/ 1);
	}

	pNextMissionNode = kQueue.deleteNode(pNode);
	if(pNextMissionNode == NULL)
		hUnit->ClearPathCache();

	if(pNextMissionNode == HeadMissionQueueNode(kQueue))
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
void CvUnitMission::ClearMissionQueue(UnitHandle hUnit, int iUnitCycleTimerOverride)
{
	//VALIDATE_OBJECT
	CvAssert(hUnit->getOwner() != NO_PLAYER);

	DeactivateHeadMission(hUnit, iUnitCycleTimerOverride);

	while(hUnit->m_missionQueue.getLength() > 0)
	{
		PopMission(hUnit);
	}

	hUnit->ClearPathCache();
//	hUnit->m_missionQueue.clear();

	if((hUnit->getOwner() == GC.getGame().getActivePlayer()) && hUnit->IsSelected())
	{
		GC.GetEngineUserInterface()->setDirty(Waypoints_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}

//	---------------------------------------------------------------------------
/// Start our first mission
void CvUnitMission::ActivateHeadMission(UnitHandle hUnit)
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
void CvUnitMission::DeactivateHeadMission(UnitHandle hUnit, int iUnitCycleTimer)
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
const MissionQueueNode* CvUnitMission::NextMissionQueueNode(const MissionQueue& kQueue, const MissionQueueNode* pNode)
{
	return kQueue.next(pNode);
}

//	---------------------------------------------------------------------------
/// Retrieve the mission before a specific one
const MissionQueueNode* CvUnitMission::PrevMissionQueueNode(const MissionQueue& kQueue, const MissionQueueNode* pNode)
{
	return kQueue.prev(pNode);
}

//	---------------------------------------------------------------------------
/// Retrieve the first mission in the queue (const correct version)
const MissionQueueNode* CvUnitMission::HeadMissionQueueNode(const MissionQueue& kQueue)
{
	return kQueue.head();
}

//	---------------------------------------------------------------------------
/// Retrieve the first mission in the queue
MissionQueueNode* CvUnitMission::HeadMissionQueueNode(MissionQueue& kQueue)
{
	return kQueue.head();
}

//	---------------------------------------------------------------------------
/// Retrieve the last mission in the queue (const correct version)
const MissionQueueNode* CvUnitMission::TailMissionQueueNode(const MissionQueue& kQueue)
{
	return ((MissionQueue&)kQueue).tail();
}

//	---------------------------------------------------------------------------
/// Retrieve the last mission in the queue
MissionQueueNode* CvUnitMission::TailMissionQueueNode(MissionQueue& kQueue)
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
const MissionData* CvUnitMission::GetHeadMissionData(UnitHandle hUnit)
{
	CvAssert(hUnit.pointer() != NULL);
	if(hUnit->m_missionQueue.getLength())
		return (hUnit->m_missionQueue.head());
	return NULL;
}

//	---------------------------------------------------------------------------
const MissionData* CvUnitMission::IsHeadMission(UnitHandle hUnit, int iMission)
{
	CvAssert(hUnit.pointer() != NULL);
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
bool CvUnitMission::HasCompletedMoveMission(UnitHandle hUnit)
{
	MissionQueueNode* pkMissionNode;
	if((pkMissionNode = HeadMissionQueueNode(hUnit->m_missionQueue)) != NULL)
	{
		MissionData& kMissionData = *pkMissionNode;
		if((kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO()) ||
		        (kMissionData.eMissionType == CvTypes::getMISSION_ROUTE_TO()) ||
		        (kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT()))
		{
			CvPlot* pTargetPlot = NULL;

			if(kMissionData.eMissionType == CvTypes::getMISSION_MOVE_TO_UNIT())
			{
				UnitHandle pTargetUnit = GET_PLAYER((PlayerTypes)kMissionData.iData1).getUnit(kMissionData.iData2);
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
	}

	return false;
}