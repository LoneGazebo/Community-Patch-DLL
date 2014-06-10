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
#include "CvMap.h"
#include <CvLocalization.h>

#undef min

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

inline int dxWrap(int iDX)
{
	const CvMap& kMap = GC.getMap();
	return wrapCoordDifference(iDX, kMap.getGridWidth(), kMap.isWrapX());
}

inline int dyWrap(int iDY)
{
	const CvMap& kMap = GC.getMap();
	return wrapCoordDifference(iDY, kMap.getGridHeight(), kMap.isWrapY());
}

inline int xToHexspaceX(int iX, int iY)
{
	return iX - ((iY >= 0) ? (iY>>1) : ((iY - 1)/2));
}

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

//
//
//// 4 | 4 | 3 | 3 | 3 | 4 | 4
//// -------------------------
//// 4 | 3 | 2 | 2 | 2 | 3 | 4
//// -------------------------
//// 3 | 2 | 1 | 1 | 1 | 2 | 3
//// -------------------------
//// 3 | 2 | 1 | 0 | 1 | 2 | 3
//// -------------------------
//// 3 | 2 | 1 | 1 | 1 | 2 | 3
//// -------------------------
//// 4 | 3 | 2 | 2 | 2 | 3 | 4
//// -------------------------
//// 4 | 4 | 3 | 3 | 3 | 4 | 4
////
//// Returns the distance between plots according to the pattern above...
//
//// not anymore - We are using hexes now!
//
inline int plotDistance(int iX1, int iY1, int iX2, int iY2)
{
	int iDX;
	int iWrappedDX = dxWrap(iX2 - iX1);
	int iWrappedDY = dyWrap(iY2 - iY1);
	int iDY = abs(iWrappedDY);

	// convert to hex-space coordinates - the coordinate system axes are E and NE (not orthogonal)
	int iHX1 = xToHexspaceX(iX1, iY1);
	int iHX2 = xToHexspaceX(iX1 + iWrappedDX, iY1 + iWrappedDY);

	iDX = abs(dxWrap(iHX2 - iHX1));

	if((iHX2 - iHX1 >= 0) == (iWrappedDY >= 0))  // the signs match
	{
		return iDX + iDY;
	}
	else
	{
		return (std::max(iDX, iDY));
	}
}
//
//// 3 | 3 | 3 | 3 | 3 | 3 | 3
//// -------------------------
//// 3 | 2 | 2 | 2 | 2 | 2 | 3
//// -------------------------
//// 3 | 2 | 1 | 1 | 1 | 2 | 3
//// -------------------------
//// 3 | 2 | 1 | 0 | 1 | 2 | 3
//// -------------------------
//// 3 | 2 | 1 | 1 | 1 | 2 | 3
//// -------------------------
//// 3 | 2 | 2 | 2 | 2 | 2 | 3
//// -------------------------
//// 3 | 3 | 3 | 3 | 3 | 3 | 3
////
//// Returns the distance between plots according to the pattern above...
//
//// not anymore - we are using hexes now
//
inline CvPlot* plotDirection(int iX, int iY, DirectionTypes eDirection)
{
	if(eDirection == NO_DIRECTION)
	{
		return GC.getMap().plot(iX, iY);
	}
	else
	{
		// convert to hex-space coordinates - the coordinate system axes are E and NE (not orthogonal)
		iX = xToHexspaceX(iX , iY);
		iX += GC.getPlotDirectionX()[eDirection];
		iY += GC.getPlotDirectionY()[eDirection];

		// convert from hex-space coordinates to the storage array
		iX = hexspaceXToX(iX, iY);

		return GC.getMap().plot(iX, iY);
	}
}

inline CvPlot* plotXY(int iX, int iY, int iDX, int iDY)
{
	// convert the start coord to hex-space coordinates
	int iStartHexX = xToHexspaceX(iX, iY);

	int iPlotHexX = iStartHexX + iDX;
	int iPlotY = iY + iDY; // Y is the same in both coordinate systems

	// convert from hex-space coordinates to the storage array
	iPlotHexX = hexspaceXToX(iPlotHexX, iPlotY);

	return GC.getMap().plot(iPlotHexX , iPlotY);
}

inline CvPlot* PlotFromHex(CvMap& kMap, int iHexX, int iHexY)
{
	// NOTE: Y is the same in both hex space and grid space.
	return kMap.plot( hexspaceXToX(iHexX, iHexY), iHexY );	
}

inline CvPlot* plotXYWithRangeCheck(int iX, int iY, int iDX, int iDY, int iRange)
{
	int hexRange;

	// I'm assuming iDX and iDY are in hex-space
	if((iDX >= 0) == (iDY >= 0))  // the signs match
	{
		int iAbsDX = iDX >= 0 ? iDX : -iDX;
		int iAbsDY = iDY >= 0 ? iDY : -iDY;
		hexRange = iAbsDX + iAbsDY;
	}
	else
	{
		int iAbsDX = iDX >= 0 ? iDX : -iDX;
		int iAbsDY = iDY >= 0 ? iDY : -iDY;
		hexRange = iAbsDX >= iAbsDY ? iAbsDX : iAbsDY;
	}

	if(hexRange > iRange)
	{
		return NULL;
	}

	return plotXY(iX, iY, iDX, iDY);
}
//	----------------------------------------------------------------------------
inline DirectionTypes directionXY(int iSourceX, int iSourceY, int iDestX, int iDestY)
{
	int iSourceHexX = xToHexspaceX(iSourceX, iSourceY);
	int iDestHexX = xToHexspaceX(iDestX, iDestY);

	int iWrappedXOffset = dxWrap(iDestHexX - iSourceHexX);
	int iWrappedYOffset = dyWrap(iDestY - iSourceY);

	if(iWrappedYOffset > 0)
	{
		if(iWrappedXOffset >= 0)
		{
			return DIRECTION_NORTHEAST;
		}
		else
		{
			return DIRECTION_NORTHWEST;
		}
	}
	else if(iWrappedYOffset == 0)
	{
		if(iWrappedXOffset > 0)
		{
			return DIRECTION_EAST;
		}
		else if(iWrappedXOffset == 0)
		{
			return NO_DIRECTION;
		}
		else
		{
			return DIRECTION_WEST;
		}
	}
	else// if (iWrappedYOffset < 0)
	{
		if(iWrappedXOffset > 0)
		{
			return DIRECTION_SOUTHEAST;
		}
		else
		{
			return DIRECTION_SOUTHWEST;
		}
	}
}
//	----------------------------------------------------------------------------
inline DirectionTypes directionXY(const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	return directionXY(pFromPlot->getX(), pFromPlot->getY(),
	                   pToPlot->getX(), pToPlot->getY());

}

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

CvPlot* plotCity(int iX, int iY, int iIndex);

int plotCityXY(const CvCity* pCity, const CvPlot* pPlot);

DirectionTypes estimateDirection(int iDX, int iDY);

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB);
bool isPotentialEnemy(TeamTypes eOurTeam, TeamTypes eTheirTeam);

CvCity* getCity(IDInfo city);
CvUnit* getUnit(const IDInfo& unit);

inline CvUnit* GetPlayerUnit(IDInfo& unit)
{
	if((unit.eOwner >= 0) && unit.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)unit.eOwner).m_units.GetAt(unit.iID));
	}

	return NULL;
}

inline const CvUnit* GetPlayerUnit(const IDInfo& unit)
{
	if((unit.eOwner >= 0) && unit.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)unit.eOwner).m_units.GetAt(unit.iID));
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

bool isWorldProject(ProjectTypes eProject);
bool isTeamProject(ProjectTypes eProject);
bool isLimitedProject(ProjectTypes eProject);

bool OnSameBodyOfWater(CvCity* pCity1, CvCity* pCity2);

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
bool PUF_isPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_canDeclareWar(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_canDefend(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_cannotDefend(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_canDefendEnemy(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_canDefendPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2 = -1);
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

void shuffleArray(int* piShuffle, int iNum, CvRandom& rand);

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
