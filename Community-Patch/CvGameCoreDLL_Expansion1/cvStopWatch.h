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
#pragma once
#ifndef CVSTOPWATCH_H
#define CVSTOPWATCH_H

class cvStopWatch
{
public:
	cvStopWatch(const char* Name, const char* LogFile = NULL, uint logFlags = 0, bool bDisable = false);
	~cvStopWatch();

	static void InitPerfTest();

	void StartPerfTest();
	void EndPerfTest();
	void SetText(const char* szName) { m_szName = szName; }

	double GetDeltaInSeconds() const;

protected:
	void PerfLog(const char* szName, double dtSeconds);

private:
	static bool ms_bPerfInit;
	static LARGE_INTEGER ms_ticksPerSecond;

	const char* m_szName;
	const char* m_szLogFile;
	double m_dtseconds;
	uint m_logFlags;
	bool m_bStarted;
	bool m_bDisable;
	LARGE_INTEGER m_oldTimerVal;
};

#if !defined(FINAL_RELEASE)
#define CVSTOPWATCH(x)	cvStopwatch(x)
#define CVSTOPWATCH_STR(x)	std::ostringstream stopwatchstr; stopwatchstr << x; cvStopwatch(stopwatchstr.str())
#else
#define CVSTOPWATCH(x)	((void)0)
#define CVSTOPWATCH_STR(x)	((void)0)
#endif

#endif //CVSTOPWATCH_H