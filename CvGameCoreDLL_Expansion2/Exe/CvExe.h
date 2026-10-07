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
		REASON_PENDING,
		REASON_NOT_IMPLEMENTED,
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
}

#endif // CV_EXE_H
