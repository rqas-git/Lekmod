













#pragma once
#ifndef CVGAMECOREDLLPCH_H
#define CVGAMECOREDLLPCH_H



#pragma warning( disable : 4800 )
#pragma warning( disable : 4355 )


#if (defined(_MSC_VER) && (_MSC_VER >= 1300) && !defined(_DEBUG))
#  if !defined(_SECURE_SCL)
#    define _SECURE_SCL 0
#  endif
#  if !defined(_HAS_ITERATOR_DEBUGGING)
#    define _HAS_ITERATOR_DEBUGGING 0
#  endif


#  if (_SECURE_SCL == 0) && (_HAS_ITERATOR_DEBUGGING == 1 )
#    error "_SECURE_SCL == 0 and _HAS_ITERATOR_DEBUGGING == 1. This combination settings can cause crashes."
#  endif
#endif


#define DEBUG_VARIABLE(x) (void)(sizeof(x))

#include "CvGameCoreDLLUtil_Win32Headers.h"
#include <MMSystem.h>

#if defined _DEBUG
#include <crtdbg.h>
#endif

#include <algorithm>

#include "_Defines.h"
#include "_FruityDefines.h"

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
#define fM_PI		3.141592654f

#define STDEXT stdext
#ifdef NQM_FAST_COMP


template<class T> inline T FastMax(const T& _Left, const T& _Right) { return (_DEBUG_LT(_Left, _Right) ? _Right : _Left); }
template<class T> inline T FastMin(const T& _Left, const T& _Right) { return (_DEBUG_LT(_Right, _Left) ? _Right : _Left); }
#define MAX(a, b) FastMax(a, b)
#define MIN(a, b) FastMin(a, b)
#else
#define MAX(a, b) std::max(a, b)
#define MIN(a, b) std::min(a, b)
#endif

#if !defined(FINAL_RELEASE)
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
#include "CvCityConnections.h"
#include "CvAdvisorCounsel.h"
#include "CvAdvisorRecommender.h"

using namespace fastdelegate;

#ifdef FINAL_RELEASE

#undef OutputDebugString
#define OutputDebugString(x)
#endif

#endif
