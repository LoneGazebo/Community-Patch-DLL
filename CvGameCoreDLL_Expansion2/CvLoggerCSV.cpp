#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "FireWorks/FVariableSystem.h"
#include "CvLoggerCSV.h"

// Include this after all other headers.
#define LINT_WARNINGS_ONLY
#include "LintFree.h"

void  CvLoggerCSV::WriteCSVLog(const char* strLogName, const char* strHeader)
	{

		FILogFile *pLog = NULL;

		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		ASSERT(pLog != NULL);

		pLog->Msg(strHeader);

	}

	void  CvLoggerCSV::DeleteCSV(const char* strLogName)
	{
		ASSERT(strLogName != NULL);

		FILogFile *pLog = NULL;

		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		ASSERT(pLog != NULL);

		LOGFILEMGR.DeleteLog(pLog);

	}