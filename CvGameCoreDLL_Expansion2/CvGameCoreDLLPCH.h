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

#if defined(__clang__)
#define CLOSED_ENUM __attribute__((enum_extensibility(closed)))
#define OPEN_ENUM __attribute__((enum_extensibility(open)))
#define FLAG_ENUM __attribute__((flag_enum))
#define ENUM_META_VALUE [[maybe_unused]]
#define BUILTIN_UNREACHABLE() __builtin_unreachable()
#define BUILTIN_TRAP() __builtin_trap()
#elif defined(_MSC_VER)
#include <intrin.h>
#define CLOSED_ENUM
#define OPEN_ENUM
#define FLAG_ENUM
#define ENUM_META_VALUE
#define BUILTIN_UNREACHABLE() __assume(0)
#define BUILTIN_TRAP() do { __ud2(); __assume(0); } while(0)
#else
#error Unrecognized compiler
#endif // defined(__clang__)

/// Informs that a location is unreachable.
///
/// In release builds the compiler may take advantage of this being unreachable to perform additional
/// optimizations. Because of this when you write code using this macro you are signing a contract
/// with the compiler that this line is truly unreachable. Programs where this line is reachable are
/// thusly ill-formed.
#define UNREACHABLE_UNCHECKED() do { CvAssertMsg(false, "Unreachable code entered"); BUILTIN_UNREACHABLE(); } while(0)

/// Weaker variant of UNREACHABLE_UNCHECKED.
///
/// This should still be used for branches that aren't expected to be reachable, but it's far preferable
/// that this be used in the majority of scenarios thanks to the compilers ability to often catch unreachable
/// code itself. In the scenario that this location is reached, the compiler is expected to emit code which
/// will terminate the program abnormally which ensures that although the program will be buggy, it is at least
/// well-formed.
#define UNREACHABLE() do { CvAssertMsg(false, "Unreachable code entered"); BUILTIN_TRAP(); } while(0)

// Take off iterator security checks
#if (defined(_MSC_VER) && (_MSC_VER >= 1300))
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

// Silences warnings about an unused variable.
#define UNUSED_VARIABLE(x) ((void)x)

// Similar to UNUSED_VARIABLE, but implies that the variable IS used in debug builds.
#define DEBUG_VARIABLE(x) UNUSED_VARIABLE(x)

#include "CvGameCoreDLLUtil_Win32Headers.h"
#include <MMSystem.h>

#include <algorithm>
#include <vector>
#include <list>
#include <math.h>
#include <tchar.h>
#include <assert.h>
#include <map>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
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

#if !defined(FINAL_RELEASE) || defined(VPDEBUG)
#define AI_PERF_LOGGING
#define AI_PERF(perfFileName, baseStringName) cvStopWatch kPerfTimer(baseStringName, perfFileName, FILogFile::kDontTimeStamp, !GC.getAIPerfLogging(), true)
#define AI_PERF_FORMAT(perfFileName, FormatValue) CvString szPerfString; szPerfString.Format##FormatValue; cvStopWatch kPerfTimer(szPerfString, perfFileName, FILogFile::kDontTimeStamp, !GC.getAIPerfLogging(), true)
#define AI_PERF_FORMAT_NESTED(perfFileName, FormatValue) CvString szPerfString2; szPerfString2.Format##FormatValue; cvStopWatch kPerfTimer2(szPerfString2, perfFileName, FILogFile::kDontTimeStamp, !GC.getAIPerfLogging(), true)
#else
#define AI_PERF(perfFileName, baseStringName) ((void)0)
#define AI_PERF_FORMAT(perfFileName, FormatValue) ((void)0)
#define AI_PERF_FORMAT_NESTED(perfFileName, FormatValue) ((void)0)
#endif

#include <FireWorks/FDefNew.h>
#include <FireWorks/FFireTypes.h>
#include <FireWorks/FAssert.h>
#include <FireWorks/FILogFile.h>
#include <FireWorks/FDataStream.h>

#include "TContainer.h"
#include "CustomMods.h"
#include "HHMods.h"

#include "CvGameDatabase.h"
#include "CvGameCoreDLLUtil.h"
#include "CvDefines.h"
#include "CvGameCoreEnums.h"
#include "CvGameCoreStructs.h"
#include "ICvDLLUtility.h"
#include "ICvDLLUserInterface.h"
#include "ICvDLLScriptSystem.h"
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
#include "CvPlotInfo.h"
#include "CvGreatPersonInfo.h"
#include "CvInfos.h"
#include "CvTeam.h"
#include "CvRandom.h"
#include "CvArea.h"
#include "CvDealClasses.h"
#include "CvCityAI.h"
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
#include "CvTradeClasses.h"
#include "CvVotingClasses.h"
#include "CvCultureClasses.h"
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
#include "CvEventLog.h"
#include "CvCityConnections.h"
#include "CvAdvisorCounsel.h"
#include "CvAdvisorRecommender.h"
#if defined(MOD_API_ACHIEVEMENTS)
#include "CvAchievementInfo.h"
#endif
#if defined(MOD_BALANCE_CORE)
#include "CvCorporationClasses.h"
#include "CvContractClasses.h"
#endif

using namespace fastdelegate;

#ifdef FINAL_RELEASE
// Undefine OutputDebugString in final release builds
#if !defined(MOD_BALANCE_CORE)
#undef OutputDebugString
#define OutputDebugString(x)
#endif
#endif //FINAL_RELEASE

#endif	// CVGAMECOREDLLPCH_H
