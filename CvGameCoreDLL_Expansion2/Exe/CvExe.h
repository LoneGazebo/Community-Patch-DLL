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
		REASON_NOT_MULTIPLAYER,
		REASON_TUNER_OFF,
		REASON_ALREADY_ENABLED,
		NUM_REASONS
	};

	const char* GetReasonName(Reason eReason);

	//! Running EXE variant: "DX11", "DX9", "Tablet" or "Unknown".
	const char* GetBuildName();

	Reason CanScheduleResync();
	Reason TryScheduleResync();

	Reason CanDisableEngineYieldIconManager();
	Reason TryDisableEngineYieldIconManager();

	Reason CanEnableTunerInMultiplayer();
	Reason TryEnableTunerInMultiplayer();

	//! A resync reloads the game but keeps the tuner open: after a load,
	//! the next AnnounceTunerAfterLoad repeats the chat announcement.
	void OnGameLoaded();
	void AnnounceTunerAfterLoad();
}

#endif // CV_EXE_H
