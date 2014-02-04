/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// CvUnitMission.h

#ifndef CIV_UNIT_MISSION_H
#define CIV_UNIT_MISSION_H

#include "CvUnit.h"

// Mission controller code for a CvUnit.
// Static class (used to namespace)
class CvUnitMission
{
public:
	static bool CanStartMission(UnitHandle hUnit, int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false);
	static void StartMission(UnitHandle hUnit);
	static void ContinueMission(UnitHandle hUnit, int iSteps = 0, int iETA = -1);
	static void AutoMission(UnitHandle hUnit);
	static void UpdateMission(UnitHandle& hUnit);
	static CvPlot* LastMissionPlot(UnitHandle hUnit);
	static void PushMission(UnitHandle hUnit, MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);
	static void PopMission(UnitHandle hUnit);
	static void ClearMissionQueue(UnitHandle hUnit, int iUnitCycleTimerOverride);
	static int  CalculateMissionTimer(UnitHandle hUnit, int iSteps = 0);
	static void UpdateMissionTimer(UnitHandle hUnit, int iSteps = 0);
	static const MissionData* IsHeadMission(UnitHandle hUnit, int iMission);
	static const MissionData* GetHeadMissionData(UnitHandle hUnit);
	static const MissionData* GetMissionData(UnitHandle hUnit, int iNode);
	/// Have a unit wait for another units missions to complete
	static void WaitFor(UnitHandle hUnit, UnitHandle hWaitForUnit);
	static bool HasCompletedMoveMission(UnitHandle hUnit);

private:
	static void						InsertAtEndMissionQueue(UnitHandle hUnit, MissionData mission, bool bStart = true);
	static MissionQueueNode*		DeleteMissionQueueNode(UnitHandle hUnit, MissionData* pNode);
	static void						ActivateHeadMission(UnitHandle hUnit);
	static void						DeactivateHeadMission(UnitHandle hUnit, int iUnitCycleTimer);

	static const MissionQueueNode*	NextMissionQueueNode(const MissionQueue& kQueue, const MissionQueueNode* pNode);
	static const MissionQueueNode*	PrevMissionQueueNode(const MissionQueue& kQueue, const MissionQueueNode* pNode);
	static const MissionQueueNode*	HeadMissionQueueNode(const MissionQueue& kQueue);
	static MissionQueueNode*		HeadMissionQueueNode(MissionQueue& kQueue);
	static const MissionQueueNode*	TailMissionQueueNode(const MissionQueue& kQueue);
	static MissionQueueNode*		TailMissionQueueNode(MissionQueue& kQueue);
	static MissionData*				GetMissionFromQueue(MissionQueue& kQueue, int iIndex);
	static int						GetLengthMissionQueue(const MissionQueue& kQueue);
};

#endif // CIV_UNIT_MISSION_H