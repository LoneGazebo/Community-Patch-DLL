/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//  FILE:    CvGameTextMgr.cpp
//
//  PURPOSE: Private implementation of CvGameTextMgr.
//
#include "CvGameCoreDLLPCH.h"
#include "CvGameTextMgr.h"
#include "CvGameCoreUtils.h"

// must be included after all other headers
#include "LintFree.h"

void CvGameTextMgr::setYearStr(CvString& strString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed)
{
	int iTurnYear = getTurnYearForGame(iGameTurn, iStartYear, eCalendar, eSpeed);

	if (iTurnYear < 0)
	{
		if (bSave)
		{
			strString = GetLocalizedText("TXT_KEY_TIME_BC_SAVE", CvString::format("%04d", -iTurnYear).GetCString(), CvString::format("%04d", iGameTurn).GetCString());
		}
		else
		{
			strString = GetLocalizedText("TXT_KEY_TIME_BC", -(iTurnYear));
		}
	}
	else if (iTurnYear > 0)
	{
		if (bSave)
		{
			strString = GetLocalizedText("TXT_KEY_TIME_AD_SAVE", CvString::format("%04d", iTurnYear).GetCString(), CvString::format("%04d", iGameTurn).GetCString());
		}
		else
		{
			strString = GetLocalizedText("TXT_KEY_TIME_AD", iTurnYear);
		}
	}
	else
	{
		if (bSave)
		{
			strString = GetLocalizedText("TXT_KEY_TIME_AD_SAVE", "0001", CvString::format("%04d", iGameTurn).GetCString());
		}
		else
		{
			strString = GetLocalizedText("TXT_KEY_TIME_AD", 1);
		}
	}
}

void CvGameTextMgr::setDateStr(CvString& strString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed)
{
	CvString strYearBuffer;
	CvString strWeekBuffer;

	setYearStr(strYearBuffer, iGameTurn, bSave, eCalendar, iStartYear, eSpeed);

	const int iNumMonths = DB.Count("Months");
	const int iNumSeasons = DB.Count("Seasons");

	switch (eCalendar)
	{
	case CALENDAR_DEFAULT:
		{
			int iTurnMonth = getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed);
			if (0 == (getTurnMonthForGame(iGameTurn + 1, iStartYear, eCalendar, eSpeed) - iTurnMonth) % iNumMonths)
			{
				strString = strYearBuffer;
			}
			else
			{
				int iMonth = iTurnMonth % iNumMonths;

				Database::SingleResult kResult;
				CvBaseInfo kInfo;
				DB.SelectAt(kResult, "Months", iMonth);
				kInfo.CacheResult(kResult);

				Localization::String lstrMonth = Localization::Lookup(kInfo.GetDescription());
				CvString strMonth = lstrMonth.toUTF8();

				if (bSave)
				{
					strString = (strYearBuffer + "-" + strMonth);
				}
				else
				{
					strString = (strMonth + CvString(", ") + strYearBuffer);
				}
			}
		}
		break;
	case CALENDAR_YEARS:
	case CALENDAR_BI_YEARLY:
		strString = strYearBuffer;
		break;

	case CALENDAR_TURNS:
		strString = GetLocalizedText("TXT_KEY_TIME_TURN", (iGameTurn + 1));
		break;

	case CALENDAR_SEASONS:
		{
			CvBaseInfo kSeasonInfo;
			Database::SingleResult kResult;
			DB.SelectAt(kResult, "Seasons", iGameTurn % iNumSeasons);
			kSeasonInfo.CacheResult(kResult);
			const char* Desc = kSeasonInfo.GetDescription();
			if (bSave)
			{
				strString = (strYearBuffer + "-" + CvString(Desc));
			}
			else
			{
				strString = (CvString(Desc) + CvString(", ") + strYearBuffer);
			}
		}

		break;

	case CALENDAR_MONTHS:
		{
			CvBaseInfo kMonthInfo;
			Database::SingleResult kResult;
			DB.SelectAt(kResult, "Months", iGameTurn % iNumMonths);
			kMonthInfo.CacheResult(kResult);

			const char* Descr = kMonthInfo.GetDescription();

			if (bSave)
			{
				strString = (strYearBuffer + "-" + CvString(Descr));
			}
			else
			{
				strString = (CvString(Descr) + CvString(", ") + strYearBuffer);
			}
		}


		break;

	case CALENDAR_WEEKS:
		{
			strWeekBuffer = GetLocalizedText("TXT_KEY_TIME_WEEK", ((iGameTurn % GC.getWEEKS_PER_MONTHS()) + 1));

			const int idx = (iGameTurn / GC.getWEEKS_PER_MONTHS()) % iNumMonths;

			CvBaseInfo kCalendarInfo;
			Database::SingleResult kResult;

			DB.SelectAt(kResult, "Months", idx);
			kCalendarInfo.CacheResult(kResult);

			const char* Descr = kCalendarInfo.GetDescription();


			if (bSave)
			{
				strString = (strYearBuffer + "-" + CvString(Descr) + "-" + strWeekBuffer);
			}
			else
			{
				strString = (strWeekBuffer + ", " + CvString(Descr) + ", " + strYearBuffer);
			}
		}
		break;

	default:
		CvAssert(false);
	}
}
