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

	// ---- Fog of war grid: code addresses, for Exe::TryFixFogGridWidth
	DWORD FogGrid_OnHexFOWStateChanged();
	DWORD FogGrid_ClearHandle();
	DWORD FogGrid_ClearAllHighlightsLoop();

	// ---- TerrainSystem::Init's call of CoastGrid::Init (see
	// CvExeAddresses.h): addresses only, for Exe::TryEnlargeCoastGridStack
	BYTE* TerrainSystem_Init_CoastGridInit();
	DWORD CoastGrid_Init();

	// ---- FluidFOWSystem's cloud store (see CvExeAddresses.h): addresses
	// only, for Exe::TryLimitFogClouds
	DWORD FluidFOW_AddCloud();
	DWORD FluidFOW_CloudStoreAlloc();
}

#endif // CV_EXE_API_H
