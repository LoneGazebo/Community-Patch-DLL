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

	// ---- Singleton: holds the cvTunerListener at +Singleton_TunerListener
	void** Singleton_Instance();
	DWORD Singleton_TunerListener();

	// ---- TunerListener: the FireTuner server. Entering a multiplayer game
	// drops its listen socket; ExitingMultiplayerStagingRoom reopens it on
	// port 4318 if Tuner_Enabled is set and no socket is open. False if the
	// port could not be opened.
	bool TunerListener_ExitingMultiplayerStagingRoom(void* pThis);
	DWORD TunerListener_ListenSocket();
	const volatile BYTE* Tuner_Enabled();
}

#endif // CV_EXE_API_H
