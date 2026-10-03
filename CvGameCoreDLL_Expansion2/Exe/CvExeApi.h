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
	DWORD g_EventSystemLocalMachine_Containers();
	DWORD g_EventSystemLocalMachine_MaxPublishedSize();
	DWORD LocalMachineContainer_Size();
	DWORD LocalMachineContainer_BufferSize();
	DWORD LocalMachineContainer_SwapCounter();
	DWORD LocalMachineContainer_BufferHeaderSize();

	// ---- cvCityBuildingSim: per-city building art, one record per city
	// plot. Main thread only.
	BYTE* g_cvCityBuildingSim();
	DWORD cvCityBuildingSim_DeferFlags();
	DWORD cvCityBuildingSim_SlotGrid();
	DWORD cvCityBuildingSim_Cities();
	DWORD cvCityBuildingSim_NumCities();
	DWORD cvCityBuildingSim_CityDataSize();
	DWORD CityData_Free();
	DWORD CityData_Hex();
	DWORD CityData_PalaceArtEntry();
	DWORD CityData_VisHandle();

	//! Index of the record for aiHex (x, y, -x-y), 0x7FFF if none.
	unsigned int cvCityBuildingSim_SlotGrid_GetSlot(void* pGrid,
		const int* aiHex);
	void cvCityBuildingSim_MarkDirty(void* pSim, const int* aiHex,
		int iFlags);

	// ---- cvCityVisSystem: draws the city models
	BYTE* g_cvCityVisSystem();
	void cvCityVisSystem_RemoveWonder(void* pThis, const void* pHandle,
		const int* piArtEntry);
}

#endif // CV_EXE_API_H
