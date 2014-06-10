/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvGameCoreDLLPCH.h
//!  \brief     Precompiled header for GameCore DLL.
//!
//!		This includes all common header files and defines used by GameCore.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVGAMECOREDLLPCH_H
#define CVGAMECOREDLLPCH_H

//  [5/18/2009 dbaker]
// Disables warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )
#pragma warning( disable : 4355 )

// Take off iterator security checks in release mode
#if (defined(_MSC_VER) && (_MSC_VER >= 1300) && !defined(_DEBUG))
#  if !defined(_SECURE_SCL)
#    define _SECURE_SCL 0
#  endif
#  if !defined(_HAS_ITERATOR_DEBUGGING)
#    define _HAS_ITERATOR_DEBUGGING 0
#  endif

// Safety check. If _SECURE_SCL is off, and _HAS_ITERATOR_DEBUGGING is on, you will crash.
#  if (_SECURE_SCL == 0) && (_HAS_ITERATOR_DEBUGGING == 1 )
#    error "_SECURE_SCL == 0 and _HAS_ITERATOR_DEBUGGING == 1. This combination settings can cause crashes."
#  endif
#endif

//Similar to UNUSED_VARIABLE, but implies that the variable IS used in debug builds.
#define DEBUG_VARIABLE(x) (void)(sizeof(x))

#include "CvGameCoreDLLUtil_Win32Headers.h"
#include <MMSystem.h>

#if defined _DEBUG
#include <crtdbg.h>
#endif

#include <algorithm>
#include <vector>
#include <list>
#include <math.h>
#include <tchar.h>
#include <assert.h>
#include <map>
#include <hash_map>
#include <limits>
#include <unordered_set>

#define DllExport   __declspec( dllexport )

typedef unsigned char    byte;
typedef unsigned int     uint;
typedef wchar_t          wchar;

#define LIMIT_RANGE(low, value, high) value = (value < low ? low : (value > high ? high : value));
#define M_PI       3.14159265358979323846
#define fM_PI		3.141592654f		//!< Pi (float)

#define STDEXT stdext
#define MAX(a, b) std::max(a, b)
#define MIN(a, b) std::min(a, b)

#if !defined(FINAL_RELEASE)
#define AI_PERF_LOGGING
#define AI_PERF(perfFileName, baseStringName) cvStopWatch kPerfTimer(baseStringName, perfFileName, FILogFile::kDontTimeStamp, !GC.getAIPerfLogging())
#define AI_PERF_FORMAT(perfFileName, FormatValue) CvString szPerfString; szPerfString.Format##FormatValue; cvStopWatch kPerfTimer(szPerfString, perfFileName, FILogFile::kDontTimeStamp, !GC.getAIPerfLogging())
#else
#define AI_PERF(perfFileName, baseStringName) ((void)0)
#define AI_PERF_FORMAT(perfFileName, FormatValue) ((void)0)
#endif

#include <FireWorks/FDefNew.h>
#include <FireWorks/FFireTypes.h>
#include <FireWorks/FAssert.h>
#include <Fireworks/FILogFile.h>
#include <Fireworks/FFreeListArrayBase.h>
#include <Fireworks/FDataStream.h>
#include <Fireworks/FFreeListTrashArray.h>
#include <Fireworks/FVariableSystem.h>
#include <FireWorks/FAStarNode.h>
#include <Fireworks/Win32/FKBInputDevice.h>
#include <Fireworks/FFastList.h>

#include "CvGameDatabase.h"
#include "CvGameCoreDLLUtil.h"
#include "CvPoint.h"
#include "CvDefines.h"
#include "CvGameCoreEnums.h"
#include "CvStructs.h"
#include "ICvDLLUtility.h"
#include "ICvDllUserInterface.h"
#include "Lua/CvLuaSupport.h"

#include "CvPlayerAI.h"
#include "CvTreasury.h"
#include "CvMap.h"
#include "CvSiteEvaluationClasses.h"
#include "CvPlot.h"
#include "CvTacticalAnalysisMap.h"
#include "CvTacticalAI.h"
#include "CvHomelandAI.h"
#include "CvUnit.h"
#include "CvGlobals.h"
#include "CvCity.h"
#include "CvInfos.h"
#include "CvTeam.h"
#include "CvRandom.h"
#include "CvArea.h"
#include "CvDealClasses.h"
#include "ICvDLLScriptSystem.h"
#include "CvCityAI.h"
#include "CvUnit.h"
#include "CvFlavorManager.h"
#include "CvTechClasses.h"
#include "CvPolicyClasses.h"
#include "CvBuildingClasses.h"
#include "CvUnitClasses.h"
#include "CvProjectClasses.h"
#include "CvPromotionClasses.h"
#include "CvImprovementClasses.h"
#include "CvEmphasisClasses.h"
#include "CvTraitClasses.h"
#include "CvBeliefClasses.h"
#include "CvReligionClasses.h"
#include "CvEspionageClasses.h"
#include "CvNotificationClasses.h"
#include "CvBuildingProductionAI.h"
#include "CvUnitProductionAI.h"
#include "CvProjectProductionAI.h"
#include "CvProcessProductionAI.h"
#include "CvCityStrategyAI.h"
#include "CvCityCitizens.h"
#include "CvGame.h"
#include "CvAStar.h"
#include "CvBuilderTaskingAI.h"
#include "CvNotifications.h"
#include "CvCityConnections.h"
#include "CvAdvisorCounsel.h"
#include "CvAdvisorRecommender.h"

using namespace fastdelegate;

#ifdef FINAL_RELEASE
// Undefine OutputDebugString in final release builds
#undef OutputDebugString
#define OutputDebugString(x)
#endif //FINAL_RELEASE

#endif	// CVGAMECOREDLLPCH_H
