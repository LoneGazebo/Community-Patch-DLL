/*	--------------------------------------------------------------------------
	ExeApi - one accessor per CvExeAddresses.h entry, same name. Returns
	0 / NULL / false when the entry is not resolved; no other checks:
	arguments must be valid engine objects, used from their own thread.
	------------------------------------------------------------------------- */

#pragma once

#ifndef CV_EXE_API_H
#define CV_EXE_API_H

namespace ExeApi
{
	// ---- InterfaceBuddy: the engine's in-game UI controller. The SDK's
	// GC.GetEngineUserInterface() points at its ICvUserInterface2 base.
	DWORD InterfaceBuddy_UserInterface();
	DWORD InterfaceBuddy_YieldIconManager();
	DWORD InterfaceBuddy_vftable();
	DWORD InterfaceBuddy_UserInterface_vftable();

	// ---- YieldIconManager: tracks the camera and posts Events.ShowHexYield
	bool YieldIconManager_UnregisterForEvents(void* pThis);

	// ---- NetMessage: the RNG sync check polls and clears this, then
	// broadcasts a force-resync
	volatile BYTE* NetMessage_WantForceResync();

	// ---- UIManager
	void* const* g_pUIManager();
	DWORD UIManager_CommandStream();
	DWORD UIManager_CommandStreamLeft();
	DWORD UIManager_CommandStreamRight();

	// ---- CCommandStream: the per-frame draw list
	DWORD CCommandStream_SetCount();
	DWORD CCommandStream_CommandCap();
	DWORD CCommandStream_CommandCount();
	DWORD CCommandStream_Commands();
	DWORD CCommandStream_CommandsOther();
	DWORD CCommandStream_Data();
	DWORD CCommandStream_DataOther();
	DWORD CCommandStream_DataSize();
	DWORD CCommandStream_DataUsed();
	DWORD CCommandStream_DataUsedOther();

	// ---- Engine heap
	void* operator_new_array(unsigned int uiSize);
	bool operator_delete_array(void* p);
	void* FireMallocAlignedNoTracking(
		unsigned int uiSize,
		unsigned int uiAlign
	);
	bool FireFreeAlignedNoTracking(void* p);
}

#endif // CV_EXE_API_H
