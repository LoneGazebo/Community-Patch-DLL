﻿/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include <algorithm>
#include "CvUnit.h"
#include "CvPlayerAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvAStar.h"
#include "CvCity.h"
#include "CvTeam.h"
#include "CvInfos.h"
#include "CvImprovementClasses.h"
#include "CvGlobals.h"
#include "psapi.h"
#include <time.h> // CvAssertDlg

#include "ICvDLLUserInterface.h"

// must be included after all other headers
#include "LintFree.h"

// CvAssertDlg and CvPreconditionDlg implementation
#ifdef CVASSERT_DEBUG_ENABLE
#ifdef WIN32

// MessageBox constants
#define MB_OK               0x00000000L
#define MB_OKCANCEL         0x00000001L
#define MB_ABORTRETRYIGNORE 0x00000002L	
#define MB_YESNOCANCEL      0x00000003L
#define MB_ICONERROR        0x00000010L
#define MB_SYSTEMMODAL      0x00001000L
#define MB_TASKMODAL        0x00002000L
#define MB_SETFOREGROUND    0x00010000L

// MessageBox default buttons
#define MB_DEFBUTTON1		0x00000000L
#define MB_DEFBUTTON2		0x00000100L
#define MB_DEFBUTTON3		0x00000200L

// MessageBox return values
#define IDOK     1
#define IDCANCEL 2
#define IDABORT  3
#define IDRETRY  4
#define IDIGNORE 5
#define IDYES    6
#define IDNO     7

// Windows functions
extern "C" {
	__declspec(dllimport) int __stdcall MessageBoxA(void* hWnd, const char* lpText, const char* lpCaption, unsigned int uType);
	__declspec(dllimport) unsigned long __stdcall GetTickCount(void);
}

// Assert tracking
struct AssertInfo {
	unsigned int count;
	unsigned int firstTime;
	unsigned int lastTime;
	unsigned int totalMemUsage;
	bool isPermanentlyIgnored;

	AssertInfo() : count(0), firstTime(0), lastTime(0), totalMemUsage(0), isPermanentlyIgnored(false) {}
};

static struct AssertTracker {
	std::map<std::string, AssertInfo> asserts;
	unsigned int sessionStart;
	unsigned int totalAsserts;
	FILE* logFile;

	AssertTracker() {
		sessionStart = GetTickCount();
		totalAsserts = 0;
		logFile = NULL;
		fopen_s(&logFile, "CvAssert.log", "a");
	}

	~AssertTracker() {
		if (logFile) {
			fclose(logFile);
		}
	}

	void LogAssert(const char* expr, const char* file, unsigned int line, const char* msg) {
		if (!logFile) return;

		time_t rawtime;
		time(&rawtime);
		char timeBuffer[26];
		ctime_s(timeBuffer, sizeof(timeBuffer), &rawtime);
		timeBuffer[24] = '\0'; // Remove newline

		fprintf(logFile, "\n[%s] Assert in %s:%u\n", timeBuffer, file, line);
		fprintf(logFile, "Expression: %s\n", expr);
		if (msg && *msg) {
			fprintf(logFile, "Message: %s\n", msg);
		}
		fflush(logFile);
	}

	std::string GetAssertKey(const char* file, unsigned int line, const char* expr) {
		char key[512];
		_snprintf_s(key, _countof(key), _TRUNCATE, "%s:%u:%s", file, line, expr);
		return std::string(key);
	}

	void UpdateStats(const std::string& key) {
		AssertInfo& info = asserts[key];
		unsigned int currentTime = GetTickCount();

		if (info.count == 0) {
			info.firstTime = currentTime;
		}

		info.count++;
		info.lastTime = currentTime;
		totalAsserts++;
	}

} g_AssertTracker;

bool CvAssertDlg(const char* expr, const char* szFile, unsigned int uiLine, bool& bIgnoreAlways, const char* msg)
{
	if (!expr) return false;

	bool bMsg = msg && msg[0] != '\0';

	// Get unique key for this assert
	std::string assertKey = g_AssertTracker.GetAssertKey(szFile, uiLine, expr);

	// Check if permanently ignored
	if (g_AssertTracker.asserts[assertKey].isPermanentlyIgnored) {
		return false;
	}

	// Check if ignored for this session
	if (bIgnoreAlways) {
		return false;
	}

	// Log the assert
	g_AssertTracker.LogAssert(expr, szFile, uiLine, msg);

	// Update statistics
	g_AssertTracker.UpdateStats(assertKey);

#if defined(VPRELEASE_ERRORMSG)
	if (GC.getGame().isReallyNetworkMultiPlayer())
	{
		char szBuffer[4096];
		_snprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
			"A code error has occurred, please report it on github. "
			"%s%s%s"
			"Expression: %s, "
			"File: %s, "
			"Line: %u",
			bMsg ? "Message: " : "", bMsg ? msg : "", bMsg ? ", " : "",
			expr, szFile, uiLine
		);
		GC.getDLLIFace()->sendChat(CvString(szBuffer), CHATTARGET_ALL, NO_PLAYER);
		return false;
	}
	else
	{
		char szBuffer[4096];
		_snprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
			"An error in the code has occurred. Please report the issue at https://github.com/LoneGazebo/Community-Patch-DLL/issues so it can be fixed.\n\n"
			"Please provide the VP version number, the list of other mods in use, and a screenshot of this message. If possible, attach a savegame from immediately before the bug occurs.\n\n"
			"You may continue playing, but unexpected behavior might occur. It is recommended to wait until a hotfix has been released that resolves the issue.\n\n"
			"==================\n"
			"Detailed information:\n"
			"%s%s%s"
			"Expression: %s\n"
			"File: %s\n"
			"Line: %u\n\n"

			"==================\n"
			"Cancel - Exit the game. \n"
			"OK - Continue playing. This warning will not be shown again in the current session.",
			bMsg ? "Message: " : "", bMsg ? msg : "", bMsg ? "\n" : "",
			expr, szFile, uiLine
		);

		// Show dialog
		int nResult = MessageBoxA(NULL, szBuffer, "Assertion Failed",
			MB_OKCANCEL | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND | MB_DEFBUTTON2);

		// Handle result
		switch (nResult)
		{
		case IDOK:      // Continue execution and ignore this error
			g_AssertTracker.asserts[assertKey].isPermanentlyIgnored = true;
			bIgnoreAlways = true;
			return false;

		default:
			BUILTIN_TRAP();
		}
	}

#else // VPRELEASE_ERRORMSG

	// Get assert info
	const AssertInfo& info = g_AssertTracker.asserts[assertKey];

	// Calculate timing information
	unsigned int timeSinceFirst = GetTickCount() - info.firstTime;
	unsigned int timeSinceLast = GetTickCount() - info.lastTime;
	unsigned int sessionTime = GetTickCount() - g_AssertTracker.sessionStart;

	// Format the message
	char szBuffer[4096];
	_snprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
		"Assert Failed!\n"
		"==================\n"
		"Expression: %s\n"
		"File: %s\n"
		"Line: %u\n"
		"%s%s%s\n"
		"\nStatistics:\n"
		"==================\n"
		"This assert has fired %u time(s)\n"
		"First occurrence: %.2f seconds ago\n"
		"Last occurrence: %.2f seconds ago\n"
		"Total asserts this session: %u\n"
		"Session duration: %.2f seconds\n"
		"\nOptions:\n"
		"==================\n"
		"Yes - Break into debugger\n"
		"No - Continue execution\n"
		"Cancel - Ignore this assert",
		expr, szFile, uiLine,
		bMsg ? "Message: " : "", bMsg ? msg : "", bMsg ? "\n" : "",
		info.count,
		timeSinceFirst / 1000.0f,
		timeSinceLast / 1000.0f,
		g_AssertTracker.totalAsserts,
		sessionTime / 1000.0f);

	// Show dialog
	int nResult = MessageBoxA(NULL, szBuffer, "Assertion Failed",
		MB_YESNOCANCEL | MB_ICONERROR | MB_TASKMODAL);

	// Handle result
	switch (nResult)
	{
	case IDYES:      // Break into debugger
		return true;

	case IDNO:       // Continue execution
		return false;

	default:         // Ignore always
		g_AssertTracker.asserts[assertKey].isPermanentlyIgnored = true;
		bIgnoreAlways = true;
		return false;
	}
#endif // VPRELEASE_ERRORMSG
}

#endif // WIN32
#endif // CVASSERT_DEBUG_ENABLE

void CvPreconditionDlg(const char* expr, const char* szFile, unsigned int uiLine, const char* msg)
{
	if (!expr) return;
#ifdef CVASSERT_DEBUG_ENABLE
#ifdef WIN32
#if defined(VPRELEASE_ERRORMSG)
	bool bMsg = msg && msg[0] != '\0';
	char szBuffer[4096];
	_snprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
		"An error in the code has occurred. Please report the issue at https://github.com/LoneGazebo/Community-Patch-DLL/issues so it can be fixed.\n\n"
		"Please provide the VP version number, the list of other mods in use, and a screenshot of this message. If possible, attach a savegame from immediately before the bug occurs.\n\n"
		"To prevent undefined or gamebreaking behavior, the game will now crash.\n\n"
		"==================\n"
		"Detailed information:\n"
		"Expression: %s\n"
		"File: %s\n"
		"Line: %u\n"
		"%s%s%s\n",
		expr, szFile, uiLine,
		bMsg ? "Message: " : "", bMsg ? msg : "", bMsg ? "\n" : ""
	);

	// Show dialog
	MessageBoxA(NULL, szBuffer, "Error",
		MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);

#else // VPRELEASE_ERRORMSG
	bool bIgnoreAlways = false;
	CvAssertDlg(expr, szFile, uiLine, bIgnoreAlways, msg);
#endif
#endif // WIN32
#endif // CVASSERT_DEBUG_ENABLE
}


int RING_PLOTS[6] = {RING0_PLOTS,RING1_PLOTS,RING2_PLOTS,RING3_PLOTS,RING4_PLOTS,RING5_PLOTS};

int dxWrap(int iDX)
{
	const CvMap& kMap = GC.getMap();
	return wrapCoordDifference(iDX, kMap.getGridWidth(), kMap.isWrapX());
}

int dyWrap(int iDY)
{
	const CvMap& kMap = GC.getMap();
	return wrapCoordDifference(iDY, kMap.getGridHeight(), kMap.isWrapY());
}

CvPlot* plotXY(int iX, int iY, int iDX, int iDY)
{
	// convert the start coord to hex-space coordinates
	int iStartHexX = xToHexspaceX(iX, iY);

	int iPlotHexX = iStartHexX + iDX;
	int iPlotY = iY + iDY; // Y is the same in both coordinate systems

	// convert from hex-space coordinates to the storage array
	iPlotHexX = hexspaceXToX(iPlotHexX, iPlotY);

	return GC.getMap().plot(iPlotHexX , iPlotY);
}

CvPlot* plotXYWithRangeCheck(int iX, int iY, int iDX, int iDY, int iRange)
{
	int hexRange = 0;

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

int plotDistance(int iX1, int iY1, int iX2, int iY2)
{
	int iX1H = xToHexspaceX(iX1,iY1);
	int iX2H = xToHexspaceX(iX2,iY2);

	//reconstruct the Z coordinate
	int iZ1H = -iX1H-iY1;
	int iZ2H = -iX2H-iY2;

	//todo: fixme. wrapping does not work correctly for large distances
	int iDX = dxWrap(iX2H - iX1H);
	int iDY = dyWrap(iY2 - iY1);
	int iDZ = dxWrap(iZ2H - iZ1H); //x and z have same range

	return (abs(iDX) + abs(iDY) + abs(iDZ)) / 2;
}

int plotDistance(const CvPlot& plotA, const CvPlot& plotB)
{
	return plotDistance(plotA.getX(),plotA.getY(),plotB.getX(),plotB.getY());
}

int plotDistance(int iIndexA, int iIndexB)
{
	CvMap& kMap = GC.getMap();
	CvPlot* pA = kMap.plotByIndex(iIndexA);
	CvPlot* pB = kMap.plotByIndex(iIndexB);
	if (pA && pB)
		return plotDistance(pA->getX(),pA->getY(),pB->getX(),pB->getY());
	else
		return INT_MAX;
}

CvPlot* plotDirection(int iX, int iY, DirectionTypes eDirection)
{
	return GC.getMap().getNeighborUnchecked(iX,iY,eDirection);
}

DirectionTypes directionXY(const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	int iSourceX = pFromPlot->getX();
	int iSourceY = pFromPlot->getY();
	int iDestX = pToPlot->getX();
	int iDestY = pToPlot->getY();

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

/// This function will return the CvPlot associated with the Index (0 to 36) of a City at iX,iY.  The lower the Index the closer the Plot is to the City (roughly)
CvPlot* iterateRingPlots(const CvPlot* pCenter, int iIndex)
{
	if (pCenter)
		return iterateRingPlots(pCenter->getX(),pCenter->getY(),iIndex);

	return NULL;
}

CvPlot* iterateRingPlots(int iX, int iY, int iIndex)
{
	int iDeltaHexX = 0;
	int iDeltaHexY = 0;

	if(iIndex < MAX_CITY_PLOTS)
	{
		iDeltaHexX = GC.getCityPlotX()[iIndex]; // getCityPlotX now uses hex-space coords
		iDeltaHexY = GC.getCityPlotY()[iIndex];
	}
	else
	{
		// loop till we find the ring this is on
		int iThisRing = 0;
		int iHighestValueOnThisRing = 0;
		int iLowestValueOnThisRing = 0;
		while(iHighestValueOnThisRing < iIndex)
		{
			iThisRing++;
			iLowestValueOnThisRing = iHighestValueOnThisRing + 1;
			iHighestValueOnThisRing += iThisRing*6;
		}
		// determine what side of the hex we are on
		int iDiff = (iIndex - iLowestValueOnThisRing);
		int iSide = iDiff / iThisRing;
		int iOffset = iDiff % iThisRing;

		switch(iSide)
		{
		case 0:
			iDeltaHexX = 0 + iOffset;
			iDeltaHexY = iThisRing - iOffset;
			break;
		case 1:
			iDeltaHexX = iThisRing;
			iDeltaHexY = 0 - iOffset;
			break;
		case 2:
			iDeltaHexX = iThisRing - iOffset;
			iDeltaHexY = -iThisRing;
			break;
		case 3:
			iDeltaHexX = 0 - iOffset;
			iDeltaHexY = -iThisRing + iOffset;
			break;
		case 4:
			iDeltaHexX = -iThisRing;
			iDeltaHexY = 0 + iOffset;
			break;
		case 5:
			iDeltaHexX = -iThisRing + iOffset;
			iDeltaHexY = iThisRing;
			break;
		default:
			return NULL;
		}

	}
	// convert the city coord to hex-space coordinates
	int iCityHexX = xToHexspaceX(iX, iY);

	int iPlotHexX = iCityHexX + iDeltaHexX;
	int iPlotY = iY + iDeltaHexY; // Y is the same in both coordinate systems

	// convert from hex-space coordinates to the storage array
	int iPlotX = hexspaceXToX(iPlotHexX, iPlotY);

	return GC.getMap().plot(iPlotX , iPlotY);
}


int getRingIterationIndex(const CvPlot* pCenter, const CvPlot* pPlot)
{
	int iWrappedDX = dxWrap(pPlot->getX() - pCenter->getX());
	int iWrappedDY = dyWrap(pPlot->getY() - pCenter->getY());

	// convert to hex-space coordinates - the coordinate system axes are E and NE (not orthogonal)
	int iCenterHexX = xToHexspaceX(pCenter->getX(), pCenter->getY());
	int iPlotHexX = xToHexspaceX(pCenter->getX() + iWrappedDX, pCenter->getY() + iWrappedDY);

	int iDX = dxWrap(iPlotHexX - iCenterHexX);
	int iDY = iWrappedDY;

	// Regardless of the working radius, we need to offset into the array by the maximum radius
	return GC.getRingIterationIndexHex((iDX + MAX_CITY_RADIUS), (iDY + MAX_CITY_RADIUS));
}

static float hexspaceDirections[6][3] = { 
	{0, 1, -1}, //ne
	{1, 0, -1}, //e
	{1, -1, 0}, //se
	{0, -1, 1}, //sw
	{-1, 0, 1}, //w
	{-1, 1, 0}, //nw
};

DirectionTypes estimateDirection(int iStartX, int iStartY, int iDestX, int iDestY)
{
	int iStartXHex = xToHexspaceX(iStartX,iStartY);
	int iDestXHex = xToHexspaceX(iDestX,iDestY);

	int iDX = dxWrap(iDestXHex - iStartXHex);
	int iDY = dyWrap(iDestY - iStartY);

	//undefined
	if (iDX==0 && iDY==0)
		return NO_DIRECTION;

	//reconstruct the Z coordinate
	int iStartZ = -iStartXHex-iStartY;
	int iDestZ = -iDestXHex-iDestY;
	int iDZ = iDestZ - iStartZ;

	float maximum = 0;
	int maximumIndex = -1;
	for(int i=0; i<6; i++)
	{
		float dotProduct = iDX * hexspaceDirections[i][0] + iDY * hexspaceDirections[i][1] + iDZ * hexspaceDirections[i][2];
		if(dotProduct > maximum)
		{
			maximum = dotProduct;
			maximumIndex = i;
		}
	}

	return (DirectionTypes)maximumIndex;
}

//compares the vector (start,destA) with (start,destB). returns a value between 0 (perfect agreement) and 60 (polar opposite direction)
int angularDeviation(int iStartX, int iStartY, int iDestAX, int iDestAY, int iDestBX, int iDestBY)
{
	//undefined
	if (iStartX==iDestAX && iStartY==iDestAY)
		return 0;
	if (iStartX==iDestBX && iStartY==iDestBY)
		return 0;

	int iStartXHex = xToHexspaceX(iStartX,iStartY);
	int iDestAXHex = xToHexspaceX(iDestAX,iDestAY);
	int iDestBXHex = xToHexspaceX(iDestBX,iDestBY);
	int iDXA = dxWrap(iDestAXHex - iStartXHex);
	int iDYA = dyWrap(iDestAY - iStartY);
	int iDXB = dxWrap(iDestBXHex - iStartXHex);
	int iDYB = dyWrap(iDestBY - iStartY);

	//reconstruct the Z coordinate
	int iStartZ = -iStartXHex-iStartY;
	int iDestAZ = -iDestAXHex-iDestAY;
	int iDestBZ = -iDestBXHex-iDestBY;
	int iDZA = iDestAZ - iStartZ;
	int iDZB = iDestBZ - iStartZ;

	float fRawDotProduct = (float) iDXA *iDXB + iDYA * iDYB + iDZA * iDZB;
	float fNormA2 = (float) iDXA*iDXA + iDYA*iDYA + iDZA*iDZA;
	float fNormB2 = (float) iDXB*iDXB + iDYB*iDYB + iDZB*iDZB;

	//this should be between -1 and +1
	float fNormDotProduct = fRawDotProduct / sqrtf( fNormA2*fNormB2 );
	//this should be between 0 and 60
	return (int)((fNormDotProduct-1)*(-30));
}

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB)
{
	if((eTeamA == NO_TEAM) || (eTeamB == NO_TEAM))
	{
		return false;
	}

#ifdef VPDEBUG
	if ((GET_TEAM(eTeamA).isAtWar(eTeamB) != GET_TEAM(eTeamB).isAtWar(eTeamA)))
		OutputDebugString("asymmetric war state!\n");
	if (GET_TEAM(eTeamA).isAtWar(eTeamA) || GET_TEAM(eTeamB).isAtWar(eTeamB))
		OutputDebugString("team is at war with itself!\n");
#endif

	return GET_TEAM(eTeamA).isAtWar(eTeamB);
}

CvCity* GetPlayerCity(IDInfo city)
{
	if((city.eOwner >= 0) && city.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER(city.eOwner).getCity(city.iID));
	}

	return NULL;
}

CvUnit* GetPlayerUnit(const IDInfo& unit)
{
	if((unit.eOwner >= 0) && unit.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)unit.eOwner).getUnit(unit.iID));
	}

	return NULL;
}

bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit)
{
	ASSERT_DEBUG(pFirstUnit != NULL);
	ASSERT_DEBUG(pSecondUnit != NULL);
	ASSERT_DEBUG(pFirstUnit != pSecondUnit);

	if(!pFirstUnit || !pSecondUnit)
		return false;

	if(pFirstUnit->getOwner() != pSecondUnit->getOwner())
	{
		return (pFirstUnit->getOwner() < pSecondUnit->getOwner());
	}

	if(pFirstUnit->getDomainType() != pSecondUnit->getDomainType())
	{
		return (pFirstUnit->getDomainType() < pSecondUnit->getDomainType());
	}

	if(pFirstUnit->GetBaseCombatStrength() != pSecondUnit->GetBaseCombatStrength())
	{
		return (pFirstUnit->GetBaseCombatStrength() > pSecondUnit->GetBaseCombatStrength());
	}

	if(pFirstUnit->getUnitType() != pSecondUnit->getUnitType())
	{
		return (pFirstUnit->getUnitType() > pSecondUnit->getUnitType());
	}

	if(pFirstUnit->getLevel() != pSecondUnit->getLevel())
	{
		return (pFirstUnit->getLevel() > pSecondUnit->getLevel());
	}

	if (pFirstUnit->getExperienceTimes100() != pSecondUnit->getExperienceTimes100())
	{
		return (pFirstUnit->getExperienceTimes100() > pSecondUnit->getExperienceTimes100());
	}

	return (pFirstUnit->GetID() < pSecondUnit->GetID());
}

/// Is this a valid Promotion for the UnitCombatType?
bool IsPromotionValidForUnitCombatType(PromotionTypes ePromotion, UnitTypes eUnit)
{
	CvUnitEntry* unitInfo = GC.getUnitInfo(eUnit);
	CvPromotionEntry* promotionInfo = GC.getPromotionInfo(ePromotion);

	if (!unitInfo || !promotionInfo)
		return false;

	// No combat class (civilians)
	if (unitInfo->GetUnitCombatType() == NO_UNITCOMBAT)
		return false;

	// Combat class not valid for this Promotion
	if (!promotionInfo->GetUnitCombatClass(unitInfo->GetUnitPromotionType()))
		return false;

	return true;
}

/// Is this a valid Promotion for this civilian?
bool IsPromotionValidForCivilianUnitType(PromotionTypes ePromotion, UnitTypes eUnit)
{
	ASSERT(ePromotion > NO_PROMOTION && ePromotion < GC.getNumPromotionInfos(), "ePromotion is not a valid promotion type");
	ASSERT(eUnit > NO_UNIT && eUnit < GC.getNumUnitInfos(), "eUnit is not a valid unit type");

	return GC.getPromotionInfo(ePromotion)->GetCivilianUnitType(eUnit);
}

bool isPromotionValid(PromotionTypes ePromotion, UnitTypes eUnit, bool bLeader, bool bTestingPrereq)
{
	CvUnitEntry* unitInfo = GC.getUnitInfo(eUnit);
	CvPromotionEntry* promotionInfo = GC.getPromotionInfo(ePromotion);

	if(unitInfo == NULL || promotionInfo == NULL)
		return false;

	// Can this Promotion not be chosen through normal leveling?
	if(!bTestingPrereq && promotionInfo->IsCannotBeChosen())
	{
		return false;
	}

	// If a Unit gets a Promotion for free then hand it out, no questions asked
	if(unitInfo->GetFreePromotions(ePromotion))
	{
		return true;
	}

	// If this isn't a combat Unit, no Promotion if not valid
	if(unitInfo->GetUnitCombatType() == NO_UNITCOMBAT)
	{
		if(!::IsPromotionValidForCivilianUnitType(ePromotion, eUnit))
		{
			return false;
		}
	}
	// Is this a valid Promotion for the UnitCombatType?
	if(unitInfo->GetUnitCombatType() != NO_UNITCOMBAT)
	{
		if(!::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
		{
			return false;
		}
	}

	if(!bLeader && promotionInfo->IsLeader())
	{
		return false;
	}

	// If the Unit only has one move then Blitz is not useful
	if(unitInfo->GetMoves() == 1)
	{
		if(promotionInfo->IsBlitz())
		{
			return false;
		}
	}

	// Promotion Prereqs
	if(NO_PROMOTION != promotionInfo->GetPrereqPromotion())
	{
		if(!isPromotionValid((PromotionTypes)promotionInfo->GetPrereqPromotion(), eUnit, bLeader, true))
		{
			return false;
		}
	}

	PromotionTypes ePrereq1 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion1();
	PromotionTypes ePrereq2 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion2();
	PromotionTypes ePrereq3 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion3();
	PromotionTypes ePrereq4 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion4();
	PromotionTypes ePrereq5 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion5();
	PromotionTypes ePrereq6 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion6();
	PromotionTypes ePrereq7 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion7();
	PromotionTypes ePrereq8 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion8();
	PromotionTypes ePrereq9 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion9();
	if(ePrereq1 != NO_PROMOTION ||
		ePrereq2 != NO_PROMOTION ||
		ePrereq3 != NO_PROMOTION ||
		ePrereq4 != NO_PROMOTION ||
		ePrereq5 != NO_PROMOTION ||
		ePrereq6 != NO_PROMOTION ||
		ePrereq7 != NO_PROMOTION ||
		ePrereq8 != NO_PROMOTION ||
		ePrereq9 != NO_PROMOTION)
	{
		bool bValid = false;
		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq1 && isPromotionValid(ePrereq1, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq2 && isPromotionValid(ePrereq2, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq3 && isPromotionValid(ePrereq3, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq4 && isPromotionValid(ePrereq4, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq5 && isPromotionValid(ePrereq5, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq6 && isPromotionValid(ePrereq6, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq7 && isPromotionValid(ePrereq7, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq8 && isPromotionValid(ePrereq8, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			if(NO_PROMOTION != ePrereq9 && isPromotionValid(ePrereq9, eUnit, bLeader, true))
			{
				bValid = true;
			}
		}

		if(!bValid)
		{
			return false;
		}
	}

	return true;
}

int getPopulationAsset(int iPopulation)
{
	return (iPopulation * 2);
}

int getLandPlotsAsset(int iLandPlots)
{
	return iLandPlots;
}

int getPopulationPower(int iPopulation)
{
	return (iPopulation / 2);
}

int getPopulationScore(int iPopulation)
{
	return iPopulation;
}

int getLandPlotsScore(int iLandPlots)
{
	return iLandPlots;
}

int getTechScore(TechTypes eTech)
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
		return 0;
	else
		return pkTechInfo->GetEra() + 1;
}

int getWonderScore(BuildingClassTypes eWonderClass)
{
	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eWonderClass);
	if(pkBuildingClassInfo)
	{
		if(isLimitedWonderClass(*pkBuildingClassInfo))
			return 5;
	}

	return 0;
}

ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount)
{
	ASSERT_DEBUG(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");

	if(iCount > GC.getNumImprovementInfos())
	{
		return NO_IMPROVEMENT;
	}

	if(GC.getImprovementInfo(eImprovement)->GetImprovementUpgrade() != NO_IMPROVEMENT)
	{
		return finalImprovementUpgrade(((ImprovementTypes)(GC.getImprovementInfo(eImprovement)->GetImprovementUpgrade())), (iCount + 1));
	}
	else
	{
		return eImprovement;
	}
}

bool isTechRequiredForUnit(TechTypes eTech, UnitTypes eUnit)
{
	CvUnitEntry* info = GC.getUnitInfo(eUnit);
	if(info == NULL)
	{
		return false;
	}

	if(info->GetPrereqAndTech() == eTech)
	{
		return true;
	}

	for(int iI = 0; iI < /*3*/ GD_INT_GET(NUM_UNIT_AND_TECH_PREREQS); iI++)
	{
		if(info->GetPrereqAndTechs(iI) == eTech)
		{
			return true;
		}
	}

	return false;
}

bool isTechRequiredForBuilding(TechTypes eTech, BuildingTypes eBuilding)
{
	CvBuildingEntry* info = GC.getBuildingInfo(eBuilding);
	if(info)
	{
		if(info->GetPrereqAndTech() == eTech)
		{
			return true;
		}

		for(int iI = 0; iI < /*3*/ GD_INT_GET(NUM_BUILDING_AND_TECH_PREREQS); iI++)
		{
			if(info->GetPrereqAndTechs(iI) == eTech)
			{
				return true;
			}
		}
	}

	return false;
}

bool isTechRequiredForProject(TechTypes eTech, ProjectTypes eProject)
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(pkProjectInfo)
	{
		if(pkProjectInfo->GetTechPrereq() == eTech)
		{
			return true;
		}
	}

	return false;
}

bool isWorldUnitClass(UnitClassTypes eUnitClass)
{
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo)
	{
		return (pkUnitClassInfo->getMaxGlobalInstances() != -1);
	}
	return false;
}

bool isTeamUnitClass(UnitClassTypes eUnitClass)
{
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo)
	{
		return (pkUnitClassInfo->getMaxTeamInstances() != -1);
	}
	return false;
}

bool isNationalUnitClass(UnitClassTypes eUnitClass)
{
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo)
	{
		return (pkUnitClassInfo->getMaxPlayerInstances() != -1);
	}
	return false;
}
#if defined(MOD_BALANCE_CORE)
bool isUnitLimitPerCity(UnitClassTypes eUnitClass)
{
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo)
	{
		return (pkUnitClassInfo->getUnitInstancePerCity() != -1);
	}
	return false;
}
#endif

bool isLimitedUnitClass(UnitClassTypes eUnitClass)
{
	return (isWorldUnitClass(eUnitClass) || isTeamUnitClass(eUnitClass) || isNationalUnitClass(eUnitClass) || isUnitLimitPerCity(eUnitClass));
}

bool isWorldProject(ProjectTypes eProject)
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(pkProjectInfo)
	{
		return (pkProjectInfo->GetMaxGlobalInstances() != -1);
	}
	return false;
}

bool isTeamProject(ProjectTypes eProject)
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(pkProjectInfo)
	{
		return (pkProjectInfo->GetMaxTeamInstances() != -1);
	}
	return false;
}

bool isLimitedProject(ProjectTypes eProject)
{
	return (isWorldProject(eProject) || isTeamProject(eProject));
}

TechTypes getDiscoveryTech(UnitTypes eUnit, PlayerTypes ePlayer)
{
	TechTypes eBestTech = NO_TECH;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo)
	{
		int iBestValue = 0;
		for(int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			const TechTypes eTech = static_cast<TechTypes>(iI);
			CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
			if(pkTechInfo)
			{
				if(kPlayer.GetPlayerTechs()->CanResearch(eTech))
				{
					int iValue = 0;

					for(int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
					{
						iValue += (pkTechInfo->GetFlavorValue(iJ) * pkUnitInfo->GetFlavorValue(iJ));
					}

					if(iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestTech = eTech;
					}
				}
			}
		}
	}

	return eBestTech;
}


bool PUF_isPlayer(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getOwner() == iData1);
}

bool PUF_isTeam(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getTeam() == iData1);
}

bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	ASSERT_DEBUG(iData2 != -1, "Invalid data argument, should be >= 0");

	return (GET_PLAYER(pUnit->getCombatOwner((TeamTypes)iData2, *(pUnit->plot()))).getTeam() == iData1 && !pUnit->isInvisible((TeamTypes)iData2, false, false));
}

bool PUF_isOtherPlayer(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getOwner() != iData1);
}

bool PUF_isOtherTeam(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	TeamTypes eTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	if(pUnit->canCoexistWithEnemyUnit(eTeam))
	{
		return false;
	}

	return (pUnit->getTeam() != eTeam);
}

bool PUF_isEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	ASSERT_DEBUG(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, *(pUnit->plot()))).getTeam();

	if(pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	return (iData2 ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));
}

bool PUF_isVisible(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return !(pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), false));
}

bool PUF_isVisibleDebug(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return !(pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), true));
}

bool PUF_canSiege(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return pUnit->canSiege(GET_PLAYER((PlayerTypes)iData1).getTeam());
}

bool PUF_canDeclareWar(const CvUnit* pUnit, int iData1, int iData2)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	ASSERT_DEBUG(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, *(pUnit->plot()))).getTeam();

	if(pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	return (iData2 ? false : GET_TEAM(eOtherTeam).canDeclareWar(eOurTeam, (PlayerTypes)iData1));
}

bool PUF_canDefend(const CvUnit* pUnit, int, int)
{
	return pUnit->IsCanDefend();
}

bool PUF_cannotDefend(const CvUnit* pUnit, int, int)
{
	return !(pUnit->IsCanDefend());
}

bool PUF_canDefendEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	ASSERT_DEBUG(iData2 != -1, "Invalid data argument, should be >= 0");
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isEnemy(pUnit, iData1, iData2));
}

bool PUF_isFighting(const CvUnit* pUnit, int, int)
{
	return pUnit->isFighting();
}

bool PUF_isDomainType(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getDomainType() == iData1);
}

bool PUF_isUnitType(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getUnitType() == iData1);
}

bool PUF_isUnitAIType(const CvUnit* pUnit, int iData1, int)
{
	ASSERT_DEBUG(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->AI_getUnitAIType() == iData1);
}

bool PUF_isSelected(const CvUnit* pUnit, int, int)
{
	return pUnit->IsSelected();
}

bool PUF_makeInfoBarDirty(CvUnit* pUnit, int, int)
{
	pUnit->setInfoBarDirty(true);
	return true;
}

bool PUF_isNoMission(const CvUnit* pUnit, int, int)
{
	return (pUnit->GetActivityType() != ACTIVITY_MISSION);
}

bool PUF_isFiniteRange(const CvUnit* pUnit, int, int)
{
	return ((pUnit->getDomainType() != DOMAIN_AIR) || (pUnit->getUnitInfo().GetRange() > 0));
}

bool isPickableName(const char* szName)
{
	if(szName)
	{
		int iLen = strlen(szName);

		if(!_stricmp(&szName[iLen-6], "NOPICK"))
		{
			return false;
		}
	}

	return true;
}

int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	return (getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed) / DB.Count("Months"));
}


int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	int iTurnMonth = 0;
	int iTurnCount = 0;
	int iI = 0;

	CvGameSpeedInfo* pkGameSpeedInfo = GC.getGameSpeedInfo(eSpeed);
	if(pkGameSpeedInfo == NULL)
	{
		//This function requires a valid game speed type!
		ASSERT_DEBUG(pkGameSpeedInfo);
		return 0;
	}

	const int iNumMonths = DB.Count("Months");

	iTurnMonth = iStartYear * iNumMonths;

	switch(eCalendar)
	{
	case CALENDAR_DEFAULT:
		{
			iTurnCount = 0;
			const int iGameSpeedNumTurnIncrements = pkGameSpeedInfo->getNumTurnIncrements();

			for(iI = 0; iI < iGameSpeedNumTurnIncrements; iI++)
			{
				const GameTurnInfo& gameTurnInfo = pkGameSpeedInfo->getGameTurnInfo(iI);

				if(iGameTurn > (iTurnCount + gameTurnInfo.iNumGameTurnsPerIncrement))
				{
					iTurnMonth += (gameTurnInfo.iMonthIncrement * gameTurnInfo.iNumGameTurnsPerIncrement);
					iTurnCount += gameTurnInfo.iNumGameTurnsPerIncrement;
				}
				else
				{
					iTurnMonth += (gameTurnInfo.iMonthIncrement * (iGameTurn - iTurnCount));
					iTurnCount += (iGameTurn - iTurnCount);
					break;
				}
			}

			if(iGameTurn > iTurnCount)
			{
				iTurnMonth += (pkGameSpeedInfo->getGameTurnInfo(iGameSpeedNumTurnIncrements - 1).iMonthIncrement * (iGameTurn - iTurnCount));
			}
		}
		break;

	case CALENDAR_BI_YEARLY:
		iTurnMonth += (2 * iGameTurn * iNumMonths);
		break;

	case CALENDAR_YEARS:
	case CALENDAR_TURNS:
		iTurnMonth += iGameTurn * iNumMonths;
		break;

	case CALENDAR_SEASONS:
		iTurnMonth += (iGameTurn * iNumMonths) / DB.Count("Seasons");
		break;

	case CALENDAR_MONTHS:
		iTurnMonth += iGameTurn;
		break;

	case CALENDAR_WEEKS:
		iTurnMonth += iGameTurn / /*4*/ GD_INT_GET(WEEKS_PER_MONTHS);
		break;

	default:
		ASSERT_DEBUG(false);
	}

	return iTurnMonth;
}


void boolsToString(const bool* pBools, int iNumBools, CvString* szOut)
{
	*szOut = "";
	int i = 0;
	for(i=0; i<iNumBools; i++)
	{
		*szOut += pBools[i] ? "1" : "0";
	}
}

//
// caller must call SAFE_DELETE_ARRAY on ppBools - caller should not be outside the DLL either
//
void stringToBools(const char* szString, int* iNumBools, bool** ppBools)
{
	ASSERT_DEBUG(szString, "null string");
	if(szString)
	{
		*iNumBools = strlen(szString);
		*ppBools = FNEW(bool[*iNumBools], c_eCiv5GameplayDLL, 0);
		int i = 0;
		for(i=0; i<*iNumBools; i++)
		{
			(*ppBools)[i] = (szString[i]=='1');
		}
	}
}

// these string functions should only be used under chipotle cheat code (not internationalized)

void getDirectionTypeString(CvString& strString, DirectionTypes eDirectionType)
{
	switch(eDirectionType)
	{
	case NO_DIRECTION:
		strString = "NO_DIRECTION";
		break;

		//case DIRECTION_NORTH: strString = "north"; break;
	case DIRECTION_NORTHEAST:
		strString = "northeast";
		break;
	case DIRECTION_EAST:
		strString = "east";
		break;
	case DIRECTION_SOUTHEAST:
		strString = "southeast";
		break;
		//case DIRECTION_SOUTH: strString = "south"; break;
	case DIRECTION_SOUTHWEST:
		strString = "southwest";
		break;
	case DIRECTION_WEST:
		strString = "west";
		break;
	case DIRECTION_NORTHWEST:
		strString = "northwest";
		break;

	default:
		strString = CvString::format("UNKNOWN_DIRECTION(%d)", eDirectionType);
		break;
	}
}

void getActivityTypeString(CvString& strString, ActivityTypes eActivityType)
{
	switch(eActivityType)
	{
	case NO_ACTIVITY:
		strString			= "NO_ACTIVITY";
		break;
	case ACTIVITY_AWAKE:
		strString		= "ACTIVITY_AWAKE";
		break;
	case ACTIVITY_HOLD:
		strString		= "ACTIVITY_HOLD";
		break;
	case ACTIVITY_SLEEP:
		strString		= "ACTIVITY_SLEEP";
		break;
	case ACTIVITY_HEAL:
		strString		= "ACTIVITY_HEAL";
		break;
	case ACTIVITY_SENTRY:
		strString		= "ACTIVITY_SENTRY";
		break;
	case ACTIVITY_INTERCEPT:
		strString		= "ACTIVITY_INTERCEPT";
		break;
	case ACTIVITY_MISSION:
		strString	= "ACTIVITY_MISSION";
		break;

	default:
		strString = CvString::format("UNKNOWN_ACTIVITY(%d)", eActivityType);
		break;
	}
}

void getMissionAIString(CvString& strString, MissionAITypes eMissionAI)
{
	switch(eMissionAI)
	{
	case NO_MISSIONAI:
		strString = "NO_MISSIONAI";
		break;
	case MISSIONAI_BUILD:
		strString = "AI_BUILD";
		break;
	case MISSIONAI_TACTMOVE:
		strString = "AI_TACTMOVE";
		break;
	case MISSIONAI_EXPLORE:
		strString = "AI_EXPLORE";
		break;
	case MISSIONAI_HOMEMOVE:
		strString = "AI_HOMEMOVE";
		break;
	case MISSIONAI_OPMOVE:
		strString = "AI_OPMOVE";
		break;

	default:
		strString = CvString::format("UNKOWN_MISSION_AI(%d)", eMissionAI);
		break;
	}
}

void getUnitAIString(CvString& strString, UnitAITypes eUnitAI)
{
	// note, GC.getUnitAIInfo(eUnitAI).getDescription() is a international friendly way to get string (but it will be longer)

	switch(eUnitAI)
	{
	case NO_UNITAI:
		strString = "no_unitAI";
		break;

	case UNITAI_UNKNOWN:
		strString = "unknown";
		break;
	case UNITAI_SETTLE:
		strString = "settle";
		break;
	case UNITAI_WORKER:
		strString = "worker";
		break;
	case UNITAI_ATTACK:
		strString = "attack";
		break;
	case UNITAI_CITY_BOMBARD:
		strString = "bombard_city";
		break;
	case UNITAI_FAST_ATTACK:
		strString = "fast_attack";
		break;
	case UNITAI_DEFENSE:
		strString = "defense";
		break;
	case UNITAI_COUNTER:
		strString = "counter";
		break;
	case UNITAI_RANGED:
		strString = "ranged";
		break;
	case UNITAI_CITY_SPECIAL:
		strString = "city_special";
		break;
	case UNITAI_EXPLORE:
		strString = "explore";
		break;
	case UNITAI_ARTIST:
		strString = "artist";
		break;
	case UNITAI_SCIENTIST:
		strString = "scientist";
		break;
	case UNITAI_GENERAL:
		strString = "general";
		break;
	case UNITAI_DIPLOMAT:
		strString = "diplomat";
		break;
	case UNITAI_MESSENGER:
		strString = "messenger";
		break;
	case UNITAI_MERCHANT:
		strString = "merchant";
		break;
	case UNITAI_ENGINEER:
		strString = "engineer";
		break;
	case UNITAI_ICBM:
		strString = "icbm";
		break;
	case UNITAI_WORKER_SEA:
		strString = "worker_sea";
		break;
	case UNITAI_ATTACK_SEA:
		strString = "attack_sea";
		break;
	case UNITAI_RESERVE_SEA:
		strString = "reserve_sea";
		break;
	case UNITAI_ESCORT_SEA:
		strString = "escort_sea";
		break;
	case UNITAI_EXPLORE_SEA:
		strString = "explore_sea";
		break;
	case UNITAI_ASSAULT_SEA:
		strString = "assault_sea";
		break;
	case UNITAI_SETTLER_SEA:
		strString = "settler_sea";
		break;
	case UNITAI_CARRIER_SEA:
		strString = "carrier_sea";
		break;
	case UNITAI_MISSILE_CARRIER_SEA:
		strString = "missile_carrier";
		break;
	case UNITAI_PIRATE_SEA:
		strString = "pirate_sea";
		break;
	case UNITAI_ATTACK_AIR:
		strString = "attack_air";
		break;
	case UNITAI_DEFENSE_AIR:
		strString = "defense_air";
		break;
	case UNITAI_CARRIER_AIR:
		strString = "carrier_air";
		break;
	case UNITAI_MISSILE_AIR:
		strString = "missile_air";
		break;
	case UNITAI_PARADROP:
		strString = "paradrop";
		break;
	case UNITAI_SPACESHIP_PART:
		strString = "spaceship_part";
		break;
	case UNITAI_TREASURE:
		strString = "treasure";
		break;
	case UNITAI_PROPHET:
		strString = "prophet";
		break;
	case UNITAI_MISSIONARY:
		strString = "missionary";
		break;
	case UNITAI_INQUISITOR:
		strString = "inquisitor";
		break;
	case UNITAI_ADMIRAL:
		strString = "admiral";
		break;
	case UNITAI_TRADE_UNIT:
		strString = "trade_unit";
		break;
	case UNITAI_ARCHAEOLOGIST:
		strString = "archaeologist";
		break;
	case UNITAI_WRITER:
		strString = "writer";
		break;
	case UNITAI_MUSICIAN:
		strString = "musician";
		break;
	// AI_UNIT_PRODUCTION
	case UNITAI_SKIRMISHER:
		strString = "skirmisher";
		break;
	case UNITAI_SUBMARINE:
		strString = "submarine";
		break;

	default:
		strString = CvString::format("unknown(%d)", eUnitAI);
		break;
	}
}

//	---------------------------------------------------------------------------
static uint SkipGUIDSeparators(const char* pszGUID, uint uiStartIndex)
{
	UINT uiLength = strlen(pszGUID);
	if(uiStartIndex < uiLength)
	{
		do
		{
			char ch = pszGUID[uiStartIndex];
			if(ch == '{' || ch == '-' || ch == ' ')
				++uiStartIndex;
			else
				break;

		}
		while(uiStartIndex < uiLength);
	}
	return uiStartIndex;
}

//	---------------------------------------------------------------------------
static bool GetHexDigitValue(char ch, uint& uiValue)
{
	if(ch >= '0' && ch <= '9')
		uiValue = (uint)(ch - '0');
	else if(ch >= 'a' && ch <= 'f')
		uiValue = (uint)((ch - 'a') + 10);
	else if(ch >= 'A' && ch <= 'F')
		uiValue = (uint)((ch - 'A') + 10);
	else
		return false;

	return true;
}
//	---------------------------------------------------------------------------
template <class T>
bool GetGUIDSegment(const char* pszGUID, uint* puiIndex, T& kDest)
{
	*puiIndex = SkipGUIDSeparators(pszGUID, *puiIndex);

	kDest = 0;
	UINT uiLength = strlen(pszGUID);
	if(*puiIndex < uiLength)
	{
		UINT uiDigitCount = sizeof(T) * 2;
		do
		{
			UINT uiValue = 0;
			if(GetHexDigitValue(pszGUID[*puiIndex], uiValue))
				kDest = (kDest * 16) + uiValue;
			else
				return false;
			*puiIndex += 1;
		}
		while(*puiIndex < uiLength  && --uiDigitCount);

		return true;
	}
	else
		return false;
}
//	---------------------------------------------------------------------------
bool ExtractGUID(const char* pszGUID, GUID& kGUID, UINT* puiStartIndex /* = NULL */)
{
	if(pszGUID)
	{
		UINT uiIndex = (puiStartIndex != NULL)?(*puiStartIndex):0;

		if(GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data1))
		{
			if(GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data2))
			{
				if(GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data3))
				{
					for(int iByte = 0; iByte < 8; ++iByte)
					{
						if(!GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data4[iByte]))
						{
							return false;
						}
					}

					if(puiStartIndex)
						*puiStartIndex = uiIndex;
					return true;
				}
			}
		}
	}

	return false;
}

//	---------------------------------------------------------------------------
void ClearGUID(GUID& kGUID)
{
	memset(&kGUID, 0, sizeof(GUID));
}

//	---------------------------------------------------------------------------
bool IsGUIDEmpty(const GUID& kGUID)
{
	return kGUID.Data1 == 0 && kGUID.Data2 == 0 && kGUID.Data3 == 0 && *(INT32*)&kGUID.Data4[0] == 0 && *(INT32*)&kGUID.Data4[4] == 0;
}

#if defined(MOD_BALANCE_CORE)
//take value and map it linearly to [0;100]. if outside of given thresholds, map to min/max. 
int MapToPercent(int iValue, int iZeroAt, int iHundredAt)
{
	if (iHundredAt!=iZeroAt)
		return MIN( 100, MAX( 0, (iValue-iZeroAt)*100/(iHundredAt-iZeroAt) ) );
	else if (iValue>iHundredAt)
		return 100;
	else if (iValue<iZeroAt)
		return 0;
	else
		return 50;
}

//------------------------------------------------------------------------------
fraction& fraction::operator+=(const fraction &rhs)
{
	// N / D = nA/dA + nB/dB
	//       = (nA*dB + nB*dA) / (dB*dA)
	if (den == rhs.den)
	{
		num += rhs.num;
	}
	else
	{
		num *= rhs.den;
		num += rhs.num * den;
		den *= rhs.den;
	}

	return *this;
}
fraction& fraction::operator-=(const fraction &rhs)
{
	// N / D = nA/dA - nB/dB
	//       = (nA*dB + -nB*dA) / (dB*dA)
	fraction neg(-rhs.num, rhs.den);
	return *this += neg;
}
fraction& fraction::operator*=(const fraction &rhs)
{
	// N / D = nA/dA * nB/dB
	//       = (nA*nB) / (dA*dB)
	num *= rhs.num;
	den *= rhs.den;
	return *this;
}
fraction& fraction::operator/=(const fraction &rhs)
{
	// N / D = nA/dA / nB/dB
	//       = (nA*dB) / (dA*nB)
	ASSERT_DEBUG(rhs.num != 0);
	num *= rhs.den;
	den *= rhs.num;
	return *this;
}

fraction fraction::operator+(const fraction &rhs)
{
	fraction lhs = *this;
	return lhs += rhs;
}
fraction fraction::operator-(const fraction &rhs)
{
	fraction lhs = *this;
	return lhs -= rhs;
}
fraction fraction::operator*(const fraction &rhs)
{
	fraction lhs = *this;
	return lhs *= rhs;
}
fraction fraction::operator/(const fraction &rhs)
{
	fraction lhs = *this;
	return lhs /= rhs;
}

bool fraction::operator==(const fraction &rhs) const
{
	fraction lhs = *this;
	return lhs.num * rhs.den == rhs.num * lhs.den;
}
bool fraction::operator<(const fraction &rhs) const
{
	fraction lhs = *this;
	return lhs.num * rhs.den < rhs.num * lhs.den;
}

fraction abs(const fraction &lhs)
{
	return fraction(abs(lhs.num), abs(lhs.den));
}
bool operator==(const int lhs, const fraction &rhs)
{
	return lhs * rhs.den == rhs.num;
}
bool operator!=(const int lhs, const fraction &rhs)
{
	return !operator==(lhs, rhs);
}

// checks to see if an operation will put the above the max, and if so, reduce the divisor & dividend
fraction fraction::checkOperands(const fraction &rhs)
{
	if(abs(num) >= abs(INT_MAX / rhs.num / rhs.den - den))
	{
		fraction lhsN = *this;
		fraction rhsN = rhs;fraction lhsO = *this;
		fraction rhsO = rhs;
		if (lhsN < rhsN)
		{
			lhsN = rhsO;
			rhsN = lhsO;
		}
		
		lhsN.Reduce();
		rhsN.Reduce();
		// if reduce failed, start fresh
		if (lhsN.isIdentical(lhsO) && rhsN.isIdentical(rhsO))
		{
			if (lhsN.num >= lhsN.den)
			{
				lhsN.num /= lhsN.den;
				lhsN.den = 1;
			}
			else
			{
				lhsN.num = 1;
				lhsN.den /= lhsN.num;
			}
			*this = lhsN;
			return rhsN;
		}
		rhsN = lhsN.checkOperands(rhsN);
		*this = lhsN;
		return rhsN;
	}
	return rhs;
}
bool fraction::isIdentical(const fraction &rhs) const
{
	return num == rhs.num && den == rhs.den;
}
// find the greatest common denominator and reduce the fraction
fraction& fraction::Reduce()
{
	int cd = gcd(num, den);
	num /= cd;
	den /= cd;
	return *this;
}
int fraction::gcd(int a, int b)
{
	if (b == 0)
		return a;

	return gcd(b, a % b);
}

FDataStream& operator<<(FDataStream& saveTo, const fraction& readFrom)
{
	saveTo << readFrom.num;
	saveTo << readFrom.den;
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, fraction& writeTo)
{
	loadFrom >> writeTo.num;
	loadFrom >> writeTo.den;
	return loadFrom;
}
#endif

//------------------------------------------------------------------------------
void PrintMemoryInfo(const char* hint)
{
	DWORD processID = GetCurrentProcessId();

	// Print information about the memory usage of the process.
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return;

	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		CUSTOMLOG("%s:\tWorkingSetSize\t%08u kB\tPrivateUsage\t%08u kB", hint, pmc.WorkingSetSize/1024, pmc.PagefileUsage/1024);

	CloseHandle(hProcess);
}