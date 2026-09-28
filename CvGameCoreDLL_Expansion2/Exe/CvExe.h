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
}

#endif // CV_EXE_H
