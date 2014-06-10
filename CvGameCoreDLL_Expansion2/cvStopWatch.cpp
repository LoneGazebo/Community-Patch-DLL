/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//  AUTHOR:		Shaun Seckman	--	1/2010
//
//  PURPOSE:	Basic stop watch functionality for profiling.
//
#include "CvGameCoreDLLPCH.h"
#include "cvStopWatch.h"

#include "LintFree.h"

bool cvStopWatch::ms_bPerfInit = false;
LARGE_INTEGER cvStopWatch::ms_ticksPerSecond;
static int ms_nesting = 0;
//------------------------------------------------------------------------------
cvStopWatch::cvStopWatch(const char* szName, const char* szLogFile /* = NULL */, uint logFlags /* = 0 */, bool bDisable /* = false */, bool bShowNesting /* = false */):
	m_szName(szName),
	m_szLogFile(szLogFile),
	m_dtseconds(0.0),
	m_logFlags(logFlags),
	m_bDisable(bDisable),
	m_bShowNesting(bShowNesting)
{
	m_nesting = ms_nesting;
	++ms_nesting;
	StartPerfTest();
}
//------------------------------------------------------------------------------
cvStopWatch::~cvStopWatch()
{
	EndPerfTest();
	--ms_nesting;
}
//------------------------------------------------------------------------------
void cvStopWatch::InitPerfTest()
{
	if(!ms_bPerfInit)
	{
		QueryPerformanceFrequency(&ms_ticksPerSecond);
		ms_bPerfInit = true;
	}
}
//------------------------------------------------------------------------------
void cvStopWatch::StartPerfTest()
{
	InitPerfTest();
	QueryPerformanceCounter(&m_oldTimerVal);
	m_bStarted = true;
}
//------------------------------------------------------------------------------
void cvStopWatch::EndPerfTest()
{
	if(!m_bStarted)
		return;

	LARGE_INTEGER newTimerVal;
	QueryPerformanceCounter(&newTimerVal);

	double dtTime = (double)newTimerVal.QuadPart - m_oldTimerVal.QuadPart;
	double dTicksPerSecond = (double)ms_ticksPerSecond.QuadPart;

	m_dtseconds = dtTime/dTicksPerSecond;

	PerfLog(m_szName, m_dtseconds);
}
//------------------------------------------------------------------------------
double cvStopWatch::GetDeltaInSeconds() const
{
	return m_dtseconds;
}
//------------------------------------------------------------------------------
void cvStopWatch::PerfLog(const char* szName, double dtSeconds)
{
	if (!m_bDisable)
	{
		const char* szLogFile = (m_szLogFile == NULL)? "stopwatch.log" : m_szLogFile;
		if (m_bShowNesting)
		{
			if (m_nesting == 0)
				LOGFILEMGR.GetLog(szLogFile, m_logFlags)->Msg(", %d, %s, %f", m_nesting, szName, dtSeconds);
			else
			{
				const int MAX_NESTING = 20;
				static const char ms_nestingOffset[] = ",,,,,,,,,,,,,,,,,,,,";
				LOGFILEMGR.GetLog(szLogFile, m_logFlags)->Msg(", %d, %s,%s %f", m_nesting, szName, ((m_nesting < MAX_NESTING)?(&ms_nestingOffset[MAX_NESTING - m_nesting]):(&ms_nestingOffset[0])), dtSeconds);
			}
		}
		else
			LOGFILEMGR.GetLog(szLogFile, m_logFlags)->Msg(", %s, %f", szName, dtSeconds);
	}
}
