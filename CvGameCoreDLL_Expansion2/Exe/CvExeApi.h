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

	// ---- Civ5NetMessage: the RNG sync check polls and clears this, then
	// broadcasts a force-resync
	volatile BYTE* Civ5NetMessage_s_wantForceResync();

	// ---- g_EventSystemLocalMachine: the LocalMachine event system; see
	// CvExeAddresses.h for the layout
	const volatile DWORD* g_EventSystemLocalMachine();

	// ---- cvLuaScriptSystem: holds the cvTunerListener at
	// +cvLuaScriptSystem_TunerListener
	void** cvLuaScriptSystem_ms_apSingletonInstance();
	DWORD cvLuaScriptSystem_TunerListener();

	// ---- cvTunerListener: the FireTuner server. Entering a multiplayer game
	// drops its listen socket; ExitingMultiplayerStagingRoom reopens it on
	// port 4318 if g_bEnableTuner is set and no socket is open. False if it
	// could not bind or listen on the port.
	bool cvTunerListener_ExitingMultiplayerStagingRoom(void* pThis);
	DWORD cvTunerListener_ListenSocket();
	const volatile BYTE* g_bEnableTuner();
}

#endif // CV_EXE_API_H
