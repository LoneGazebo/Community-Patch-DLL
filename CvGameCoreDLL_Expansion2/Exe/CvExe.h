#pragma once

#ifndef CV_EXE_H
#define CV_EXE_H

class CvCity;

namespace Exe
{
	//! Why a CanX / TryX refused; Lua gets GetReasonName().
	enum Reason
	{
		REASON_OK,
		REASON_BIN_HOOKS_OFF,
		REASON_UNSUPPORTED_EXE,
		REASON_NOT_NETWORK_GAME,
		REASON_NOT_HOST,
		REASON_UNAVAILABLE,
		REASON_NOT_UI_THREAD,
		REASON_WRONG_EVENT_CHANNEL,
		REASON_NOT_MULTIPLAYER,
		REASON_TUNER_OFF,
		REASON_ALREADY_ENABLED,
		REASON_PENDING,
		REASON_NOT_IMPLEMENTED,
		REASON_BUSY,
		NUM_REASONS
	};

	const char* GetReasonName(Reason eReason);

	//! Running EXE variant: "DX11", "DX9", "Tablet" or "Unknown".
	const char* GetBuildName();

	Reason CanScheduleResync();
	Reason TryScheduleResync();

	Reason CanDisableEngineYieldIconManager();
	Reason TryDisableEngineYieldIconManager();

	//! UI thread only
	Reason CanGrowUICommandStream();
	Reason TryGrowUICommandStream();

	Reason CanGetUICommandStreamUsage();
	Reason TryGetUICommandStreamUsage(
		unsigned int& uiUsed,
		unsigned int& uiCapacity
	);

	//! "LocalMachineEventStream" in the engine's debug info. The low bit of
	//! the swap counter picks the buffer being written.
	struct LocalMachineEventStreamStats
	{
		DWORD dwSwapCounter;
		DWORD adwPublishedSize[2];
		DWORD dwBufferCapacity;
		DWORD dwMaxPublishedSize;
	};

	//! Reads only, and does not log a refusal: it is called every frame.
	Reason TryReadLocalMachineEventStreamStats(
		LocalMachineEventStreamStats& kStats
	);

	Reason CanEnableTunerInMultiplayer();
	Reason TryEnableTunerInMultiplayer();

	//! A resync reloads the game but keeps the tuner open: after a load,
	//! the next AnnounceTunerAfterLoad repeats the chat announcement.
	void OnGameLoaded();
	void AnnounceTunerAfterLoad();

	//! The engine keeps a removed Palace in a city's art (its Palace branch
	//! clears the wrong field), so a captured capital kept its Palace model
	//! until a reload. TryRepairCityArt removes it the way the engine removes
	//! other buildings. Main thread only. REASON_OK: nothing (more) to do;
	//! REASON_BUSY: the engine is not taking changes now, try again later.
	Reason CanRepairCityArt();
	Reason TryRepairCityArt(const CvCity& kCity);

	//! The engine's fog of war grid counts width * width cells in its three
	//! whole-map loops, so a map wider than 181 columns writes past the grid
	//! and the game crashes while it loads. TryFixFogGridWidth makes the
	//! loops count width * height. It changes EXE code, so call it only for
	//! a map that needs it, before the engine sets up its fog for that map.
	//! REASON_OK: the loops count width * height (also if an earlier DLL
	//! load left them fixed). UndoFixFogGridWidth puts the loops back; the
	//! DLL calls it when it unloads.
	Reason CanFixFogGridWidth();
	Reason TryFixFogGridWidth();
	void UndoFixFogGridWidth();

	//! The engine's CoastGrid::Init takes about 48 bytes per plot (map plus
	//! a 2-plot border) from the main thread's 1 MB stack, so a map of more
	//! than about 21,000 plots ends the game at load. TryEnlargeCoastGridStack
	//! runs that call on a 4 MB stack of its own; once per DLL load (later
	//! calls do nothing). UndoEnlargeCoastGridStack puts the call back; the
	//! DLL calls it when it unloads.
	Reason CanEnlargeCoastGridStack();
	Reason TryEnlargeCoastGridStack();
	void UndoEnlargeCoastGridStack();
}

#endif // CV_EXE_H
