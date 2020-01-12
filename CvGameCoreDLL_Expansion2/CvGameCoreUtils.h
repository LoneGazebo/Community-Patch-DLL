/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#ifndef CIV5_GAMECORE_UTILS_H
#define CIV5_GAMECORE_UTILS_H

#include "CvStructs.h"
#include "CvGlobals.h"
#include <CvLocalization.h>

#undef min

inline int sqrti(int input)
{
	return int(sqrt((float)abs(input)));
}

inline int range(int iNum, int iLow, int iHigh)
{
	CvAssertMsg(iHigh >= iLow, "High should be higher than low");

	if(iNum < iLow)
	{
		return iLow;
	}
	else if(iNum > iHigh)
	{
		return iHigh;
	}
	else
	{
		return iNum;
	}
}

inline float range(float fNum, float fLow, float fHigh)
{
	CvAssertMsg(fHigh >= fLow, "High should be higher than low");

	if(fNum < fLow)
	{
		return fLow;
	}
	else if(fNum > fHigh)
	{
		return fHigh;
	}
	else
	{
		return fNum;
	}
}

inline int wrapCoordDifference(int iDiff, uint uiRange, bool bWrap)
{
	if(bWrap)
	{
		if(iDiff > (int)(uiRange >> 1))		// Using an unsigned int so we can safely assume that value >> 1 == value / 2
		{
			return (iDiff - (int)uiRange);
		}
		else if(iDiff < -(int)(uiRange >> 1))
		{
			return (iDiff + (int)uiRange);
		}
	}

	return iDiff;
}

int dxWrap(int iDX);
int dyWrap(int iDY);

//convert odd-r offset coords to hex cube coords (only x, y is invariant)
inline int xToHexspaceX(int iX, int iY)
{
	return iX - ((iY >= 0) ? (iY>>1) : ((iY - 1)/2));
}

//convert hex cube coords to odd-r offset coords (only x, y is invariant)
inline int hexspaceXToX(int iHexspaceX, int iHexspaceY)
{
	return iHexspaceX + ((iHexspaceY >= 0) ? (iHexspaceY>>1) : ((iHexspaceY - 1)/2));
}

inline int hexDistance(int iDX, int iDY)
{
	// I'm assuming iDX and iDY are in hex-space
	if((iDX >= 0) == (iDY >= 0))  // the signs match
	{
		int iAbsDX = iDX >= 0 ? iDX : -iDX;
		int iAbsDY = iDY >= 0 ? iDY : -iDY;
		return iAbsDX + iAbsDY;
	}
	else
	{
		int iAbsDX = iDX >= 0 ? iDX : -iDX;
		int iAbsDY = iDY >= 0 ? iDY : -iDY;
		return iAbsDX >= iAbsDY ? iAbsDX : iAbsDY;
	}
}

int plotDistance(int iIndexA, int iIndexB);
int plotDistance(int iX1, int iY1, int iX2, int iY2);
int plotDistance(const CvPlot& plotA, const CvPlot& plotB);
CvPlot* plotDirection(int iX, int iY, DirectionTypes eDirection);
CvPlot* plotXY(int iX, int iY, int iDX, int iDY);
CvPlot* plotXYWithRangeCheck(int iX, int iY, int iDX, int iDY, int iRange);

inline DirectionTypes reorderedDirection(DirectionTypes initialDir, int iIndex)
{
	//normally we go clockwise so this time we go counterclockwise ...
	int iOffset = 0;
	switch (iIndex % 6)
	{
	case 0:
		return initialDir;
		break;
	case 1:
		iOffset = 6-1;
		break;
	case 2:
		iOffset = +1;
		break;
	case 3:
		iOffset = 6-2;
		break;
	case 4:
		iOffset = +2;
		break;
	case 5:
		iOffset = +3;
		break;
	}

	return (DirectionTypes)(( initialDir+iOffset ) % 6);
}

// -----------------------------------------------------------------------------
DirectionTypes estimateDirection(int iSourceX, int iSourceY, int iDestX, int iDestY);
int angularDeviation(int iStartX, int iStartY, int iDestAX, int iDestAY, int iDestBX, int iDestBY);

//	----------------------------------------------------------------------------
DirectionTypes directionXY(const CvPlot* pFromPlot, const CvPlot* pToPlot);

/// Find the nearest "spike" direction that we are clockwise of
/// this assumes that the offsets are in hexspace
inline DirectionTypes hexspaceSpikeDirection(const int iXOffset, const int iYOffset)
{
	if(iYOffset > 0)
	{
		if(iXOffset >= 0)
		{
			return DIRECTION_NORTHEAST;
		}
		else if(-iXOffset <= iYOffset)
		{
			return DIRECTION_NORTHWEST;
		}
		else //if (-iXOffset > iYOffset)
		{
			return DIRECTION_WEST;
		}
	}
	else if(iYOffset == 0)
	{
		if(iXOffset > 0)
		{
			return DIRECTION_EAST;
		}
		else if(iXOffset == 0)
		{
			return NO_DIRECTION;
		}
		else
		{
			return DIRECTION_WEST;
		}
	}
	else// if (iYOffset < 0)
	{
		if(iXOffset > -iYOffset)
		{
			return DIRECTION_EAST;
		}
		else if(iXOffset > 0)
		{
			return DIRECTION_SOUTHEAST;
		}
		else
		{
			return DIRECTION_SOUTHWEST;
		}
	}
}

extern int RING_PLOTS[6];
CvPlot* iterateRingPlots(const CvPlot* pCenter, int iIndex);
CvPlot* iterateRingPlots(int iX, int iY, int iIndex);
int getRingIterationIndex(const CvPlot* pCenter, const CvPlot* pPlot);

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB);

CvCity* getCity(IDInfo city);
CvUnit* getUnit(const IDInfo& unit);

inline CvUnit* GetPlayerUnit(const IDInfo& unit)
{
	if((unit.eOwner >= 0) && unit.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)unit.eOwner).getUnit(unit.iID));
	}

	return NULL;
}

bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit);
bool IsPromotionValidForUnitCombatType(PromotionTypes ePromotion, UnitTypes eUnit);
bool IsPromotionValidForCivilianUnitType(PromotionTypes ePromotion, UnitTypes eUnit);
bool isPromotionValid(PromotionTypes ePromotion, UnitTypes eUnit, bool bLeader, bool bTestingPrereq=false);

int getPopulationAsset(int iPopulation);
int getLandPlotsAsset(int iLandPlots);
int getPopulationPower(int iPopulation);
int getPopulationScore(int iPopulation);
int getLandPlotsScore(int iLandPlots);
int getTechScore(TechTypes eTech);
int getWonderScore(BuildingClassTypes eWonderClass);

ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount = 0);

bool isTechRequiredForUnit(TechTypes eTech, UnitTypes eUnit);
bool isTechRequiredForBuilding(TechTypes eTech, BuildingTypes eBuilding);
bool isTechRequiredForProject(TechTypes eTech, ProjectTypes eProject);

bool isWorldUnitClass(UnitClassTypes eUnitClass);
bool isTeamUnitClass(UnitClassTypes eUnitClass);
bool isNationalUnitClass(UnitClassTypes eUnitClass);
bool isLimitedUnitClass(UnitClassTypes eUnitClass);

#if defined(MOD_BALANCE_CORE)
bool isUnitLimitPerCity(UnitClassTypes eUnitClass);
#endif

bool isWorldProject(ProjectTypes eProject);
bool isTeamProject(ProjectTypes eProject);
bool isLimitedProject(ProjectTypes eProject);

TechTypes getDiscoveryTech(UnitTypes eUnit, PlayerTypes ePlayer);

int CompareWeights(const void* a, const void* b);

// PlotUnitFunc's...
bool PUF_isPlayer(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isTeam(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2);
bool PUF_isOtherPlayer(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isOtherTeam(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isEnemy(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isVisible(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isVisibleDebug(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_canSiege(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_canDeclareWar(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_canDefend(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_cannotDefend(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_canDefendEnemy(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isFighting(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isUnitType(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isDomainType(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isUnitAIType(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isSelected(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_makeInfoBarDirty(CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_isNoMission(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_isFiniteRange(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);

// Inet Stuff
void sendGameStats(char* pURL);

int baseYieldToSymbol(int iNumYieldTypes, int iYieldStack);

bool isPickableName(const char* szName);

template <class T>
void shuffleArray(T* pShuffle, size_t iNum, CvRandom& rand)
{
	for(size_t iI = 0; iI < iNum; iI++)
	{
		size_t iJ = (rand.get(iNum - iI, "shuffle") + iI);

		if(iI != iJ)
		{
			T iTemp = pShuffle[iI];
			pShuffle[iI] = pShuffle[iJ];
			pShuffle[iJ] = iTemp;
		}
	}
}

int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);
int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);

void boolsToString(const bool* pBools, int iNumBools, CvString* szOut);
void stringToBools(const char* szString, int* iNumBools, bool** ppBools);

void getDirectionTypeString(CvString& szString, DirectionTypes eDirectionType);
void getActivityTypeString(CvString& szString, ActivityTypes eActivityType);
void getMissionAIString(CvString& szString, MissionAITypes eMissionAI);
void getUnitAIString(CvString& szString, UnitAITypes eUnitAI);

bool ExtractGUID(const char* pszGUID, GUID& kGUID, UINT* puiStartIndex = NULL);
void ClearGUID(GUID& kGUID);
bool IsGUIDEmpty(const GUID& kGUID);

//The following templates are used to aide migration from the old text system to the new one.
inline CvString GetLocalizedText(const char* szString)
{
	Localization::String text = Localization::Lookup(szString);

	size_t bytes = 0;
	const char* szComposedString = text.toUTF8(bytes);
	CvString str;
	str.assign(szComposedString, bytes);

	//fallback
	if (str.empty())
		str = szString;

	return str;
}
//------------------------------------------------------------------------------
template<typename T>
inline CvString GetLocalizedText(const char* szString, T arg1)
{
	Localization::String text = Localization::Lookup(szString);
	text << arg1;

	size_t bytes = 0;
	const char* szComposedString = text.toUTF8(bytes);
	CvString str;
	str.assign(szComposedString, bytes);

	//fallback
	if (str.empty())
		str = szString;

	return str;
}
//------------------------------------------------------------------------------
template<typename T1, typename T2>
inline CvString GetLocalizedText(const char* szString, const T1& arg1, const T2& arg2)
{
	Localization::String text = Localization::Lookup(szString);
	text << arg1 << arg2;

	size_t bytes = 0;
	const char* szComposedString = text.toUTF8(bytes);
	CvString str;
	str.assign(szComposedString, bytes);

	//fallback
	if (str.empty())
		str = szString;

	return str;
}
//------------------------------------------------------------------------------
template<typename T1, typename T2, typename T3>
inline CvString GetLocalizedText(const char* szString, const T1& arg1, const T2& arg2, const T3& arg3)
{
	Localization::String text = Localization::Lookup(szString);
	text << arg1 << arg2 << arg3;

	size_t bytes = 0;
	const char* szComposedString = text.toUTF8(bytes);
	CvString str;
	str.assign(szComposedString, bytes);

	//fallback
	if (str.empty())
		str = szString;

	return str;
}
//------------------------------------------------------------------------------
template<typename T1, typename T2, typename T3, typename T4>
inline CvString GetLocalizedText(const char* szString, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
{
	Localization::String text = Localization::Lookup(szString);
	text << arg1 << arg2 << arg3 << arg4;

	size_t bytes = 0;
	const char* szComposedString = text.toUTF8(bytes);
	CvString str;
	str.assign(szComposedString, bytes);

	//fallback
	if (str.empty())
		str = szString;

	return str;
}
//------------------------------------------------------------------------------
template<typename T1, typename T2, typename T3, typename T4, typename T5>
inline CvString GetLocalizedText(const char* szString, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5)
{
	Localization::String text = Localization::Lookup(szString);
	text << arg1 << arg2 << arg3 << arg4 << arg5;

	size_t bytes = 0;
	const char* szComposedString = text.toUTF8(bytes);
	CvString str;
	str.assign(szComposedString, bytes);

	//fallback
	if (str.empty())
		str = szString;

	return str;
}

#if !defined(FINAL_RELEASE)
#define NET_MESSAGE_DEBUG(x)		gDLL->netMessageDebugLog(x)
#define NET_MESSAGE_DEBUG_OSTR(x)	{ std::ostringstream str; str << x; gDLL->netMessageDebugLog(str.str()); }
#else
#define NET_MESSAGE_DEBUG(x)		((void)0)
#define NET_MESSAGE_DEBUG_OSTR(x)	((void)0)
#endif

#define NET_MESSAGE_DEBUG_OSTR_ALWAYS(x)	{ std::ostringstream str; str << x; gDLL->netMessageDebugLog(str.str()); }

#endif

#if defined(MOD_BALANCE_CORE)
//take value and map it linearly to [0;100]. if outside of given thresholds, map to min/max. 
int MapToPercent(int iValue, int iZeroAt, int iHundredAt);

template<class T>
struct OptionWithScore
{
	T option;
	int score;
	OptionWithScore(T t, int s) : option(t), score(s) {}
	bool operator<(const OptionWithScore& rhs) const { return score > rhs.score; } //sort descending!
};

template<class T>
T PseudoRandomChoiceByWeight(vector<OptionWithScore<T>>& candidates, const T& defaultChoice, int maxCandidatesToConsider, int randomSeed)
{
	if (candidates.empty())
		return defaultChoice;

	if (candidates.size() == 1 || maxCandidatesToConsider == 1)
		return candidates[0].option;

	size_t maxCandidates = min(candidates.size(), (size_t)maxCandidatesToConsider);
	sort(candidates.begin(), candidates.end());

	int totalWeight = 0;
	for (size_t i = 0; i < maxCandidates; i++)
		totalWeight += candidates[i].score;

	int index = GC.getGame().getSmallFakeRandNum(10, randomSeed);
	int selectedWeight = (totalWeight*index) / 10;

	int weight = 0;
	for (size_t i = 0; i < maxCandidates; i++)
	{
		weight += candidates[i].score;
		if (weight > selectedWeight)
			return candidates[i].option;
	}

	return defaultChoice;
}

#endif