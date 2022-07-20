/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "ICvDLLUserInterface.h"
#ifdef _WIN32
#include "Win32/FDebugHelper.h"
#endif
#include "CvDllContext.h"

// must be included after all other headers
#include "LintFree.h"

extern "C" {
//------------------------------------------------------------------------------
#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
ICvGameContext1* DllGetGameContext()
{
	return CvDllGameContext::GetSingleton();
}
//------------------------------------------------------------------------------
#ifndef _WIN32
__attribute__((constructor))
#endif
static void initialize()
{
	CvDllGameContext::InitializeSingleton();
}
#ifndef _WIN32
__attribute__((destructor))
#endif
static void uninitialize()
{
	CvDllGameContext::DestroySingleton();
	GC.setDLLIFace(NULL);
}
#ifdef _WIN32
#include <MMSystem.h>
BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		// The DLL is being loaded into the virtual address space of the current process as a result of the process starting up
		OutputDebugString("DLL_PROCESS_ATTACH\n");
		FDebugHelper::GetInstance().LoadSymbols((HMODULE)hModule);
		// set timer precision
		MMRESULT iTimeSet = timeBeginPeriod(1);		// set timeGetTime and sleep resolution to 1 ms, otherwise it's 10-16ms
		DEBUG_VARIABLE(iTimeSet);
		CvAssertMsg(iTimeSet==TIMERR_NOERROR, "failed setting timer resolution to 1 ms");
		initialize();
	}
	break;
	case DLL_THREAD_ATTACH:
		OutputDebugString("DLL_THREAD_ATTACH\n");
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString("DLL_THREAD_DETACH\n");
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugString("DLL_PROCESS_DETACH\n");
		timeEndPeriod(1);
		uninitialize();
		break;
	}

	return TRUE;	// success
}
#endif
}
