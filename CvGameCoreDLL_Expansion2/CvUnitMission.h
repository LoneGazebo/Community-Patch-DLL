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
	static bool CanStartMission(CvUnit* hUnit, int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false);
	static void StartMission(CvUnit* hUnit);
	static void ContinueMission(CvUnit* hUnit, int iSteps = 0);
	static void AutoMission(CvUnit* hUnit);
	static void UpdateMission(CvUnit* hUnit);
	static CvPlot* LastMissionPlot(CvUnit* hUnit);
	static void PushMission(CvUnit* hUnit, MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);
	static void PopMission(CvUnit* hUnit);
	static void ClearMissionQueue(CvUnit* hUnit, bool bKeepPathCache, int iUnitCycleTimerOverride);
	static int  CalculateMissionTimer(CvUnit* hUnit, int iSteps = 0);
	static void UpdateMissionTimer(CvUnit* hUnit, int iSteps = 0);
	static const MissionData* IsHeadMission(CvUnit* hUnit, int iMission);
	static const MissionData* GetHeadMissionData(CvUnit* hUnit);
	static const MissionData* GetMissionData(CvUnit* hUnit, int iNode);
	/// Have a unit wait for another units missions to complete
	static void WaitFor(CvUnit* hUnit, CvUnit* hWaitForUnit);
	static bool HasCompletedMoveMission(CvUnit* hUnit);

private:
	static void						InsertAtEndMissionQueue(CvUnit* hUnit, MissionData mission, bool bStart = true);
	static MissionData*		DeleteMissionData(CvUnit* hUnit, MissionData* pNode);
	static void						ActivateHeadMission(CvUnit* hUnit);
	static void						DeactivateHeadMission(CvUnit* hUnit, int iUnitCycleTimer);

	static const MissionData*	NextMissionData(const MissionQueue& kQueue, const MissionData* pNode);
	static const MissionData*	PrevMissionData(const MissionQueue& kQueue, const MissionData* pNode);
	static const MissionData*	HeadMissionData(const MissionQueue& kQueue);
	static MissionData*		HeadMissionData(MissionQueue& kQueue);
	static const MissionData*	TailMissionData(const MissionQueue& kQueue);
	static MissionData*		TailMissionData(MissionQueue& kQueue);
	static MissionData*				GetMissionFromQueue(MissionQueue& kQueue, int iIndex);
	static int						GetLengthMissionQueue(const MissionQueue& kQueue);
};

#endif // CIV_UNIT_MISSION_H