#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "Fireworks/FVariableSystem.h"
#include "CvLoggerCSV.h"

// Include this after all other headers.
#define LINT_WARNINGS_ONLY
#include "LintFree.h"

void  CvLoggerCSV::WriteCSVLog(const char* strLogName, const char* strHeader)
	{

		FILogFile *pLog;

		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		CvAssert(pLog != NULL);

		pLog->Msg(strHeader);

	}

	void  CvLoggerCSV::DeleteCSV(const char* strLogName)
	{
		CvAssert(strLogName != NULL);

		FILogFile *pLog;

		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		CvAssert(pLog != NULL);

		LOGFILEMGR.DeleteLog(pLog);

	}