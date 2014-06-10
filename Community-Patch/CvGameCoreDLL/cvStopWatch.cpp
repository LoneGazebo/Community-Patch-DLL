/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//  FILE:		cvStopWatch.cpp
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
//------------------------------------------------------------------------------
cvStopWatch::cvStopWatch(const char* szName, const char* szLogFile /* = NULL */, uint logFlags /* = 0 */, bool bDisable /* = false */):
m_szName(szName),
m_szLogFile(szLogFile),
	m_dtseconds(0.0),
	m_logFlags(logFlags),
	m_bDisable(bDisable)
{
	StartPerfTest();
}
//------------------------------------------------------------------------------
cvStopWatch::~cvStopWatch()
{
	EndPerfTest();
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
		LOGFILEMGR.GetLog(szLogFile, m_logFlags)->Msg(", %s, %f", szName, dtSeconds);
	}
}
