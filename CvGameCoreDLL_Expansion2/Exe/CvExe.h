#pragma once

#ifndef CV_EXE_H
#define CV_EXE_H

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
		NUM_REASONS
	};

	const char* GetReasonName(Reason eReason);

	//! Running EXE variant: "DX11", "DX9", "Tablet" or "Unknown".
	const char* GetBuildName();

	Reason CanScheduleResync();
	Reason TryScheduleResync();

	Reason CanDisableEngineYieldIconManager();
	Reason TryDisableEngineYieldIconManager();

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
}

#endif // CV_EXE_H
