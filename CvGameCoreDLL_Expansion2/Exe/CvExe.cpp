#include "CvGameCoreDLLPCH.h"
#include "CvExe.h"
#include "CvExeApi.h"
#include "CvExeBuild.h"
#include "CvExeSymbols.h"
#include "CvGameCoreUtils.h"

// must be included after all other headers
#include "LintFree.h"

namespace
{
const char* const REASON_NAMES[Exe::NUM_REASONS] =
{
	"ok",
	"bin_hooks_off",
	"unsupported_exe",
	"not_network_game",
	"not_host",
	"unavailable",
	"not_multiplayer",
	"tuner_off",
	"already_enabled",
};

// The symbols each feature uses
const ExeSymbol RESYNC_SYMBOLS[] =
{
	EXE_NetMessage_WantForceResync,
};

const ExeSymbol YIELD_ICON_MANAGER_SYMBOLS[] =
{
	EXE_InterfaceBuddy_UserInterface,
	EXE_InterfaceBuddy_vftable,
	EXE_InterfaceBuddy_UserInterface_vftable,
	EXE_InterfaceBuddy_YieldIconManager,
	EXE_YieldIconManager_UnregisterForEvents,
};

const ExeSymbol TUNER_SYMBOLS[] =
{
	EXE_Singleton_Instance,
	EXE_Singleton_TunerListener,
	EXE_TunerListener_ExitingMultiplayerStagingRoom,
	EXE_TunerListener_ListenSocket,
	EXE_Tuner_Enabled,
};

bool HasSymbols(const ExeSymbol* aSymbols, unsigned int uiCount)
{
	for (unsigned int i = 0; i < uiCount; ++i)
	{
		if (!ExeSymbols::IsResolved(aSymbols[i]))
		{
			return false;
		}
	}

	return true;
}

Exe::Reason Refuse(const char* szWhat, Exe::Reason eReason)
{
	CUSTOMLOG("Exe: %s refused: %s (%s EXE)",
		szWhat,
		Exe::GetReasonName(eReason),
		Exe::GetBuildName()
	);

	return eReason;
}

DWORD ReadVftable(DWORD dwObject)
{
	const DWORD* p = reinterpret_cast<const DWORD*>(dwObject);

	return !IsBadReadPtr(p, sizeof(DWORD)) ? *p : 0;
}

//! Engine's InterfaceBuddy, found from GC.GetEngineUserInterface(), or
//! NULL if the memory there does not match one.
void* FindInterfaceBuddy()
{
	const DWORD dwUI = reinterpret_cast<DWORD>(GC.GetEngineUserInterface());

	if (dwUI == 0)
	{
		return NULL;
	}

	const DWORD dwBuddy = dwUI - ExeApi::InterfaceBuddy_UserInterface();
	const DWORD dwVftable = ReadVftable(dwBuddy);
	const DWORD dwUIVftable = ReadVftable(dwUI);

	if (
		dwVftable != ExeApi::InterfaceBuddy_vftable()
		|| dwUIVftable != ExeApi::InterfaceBuddy_UserInterface_vftable()
	)
	{
		CUSTOMLOG(
			"Exe: %08X is not an InterfaceBuddy (vftables %08X / %08X)",
			(unsigned int)dwUI,
			(unsigned int)dwVftable,
			(unsigned int)dwUIVftable
		);
		return NULL;
	}

	return reinterpret_cast<void*>(dwBuddy);
}

//! The engine's cvTunerListener, or NULL before the engine creates it.
BYTE* FindTunerListener()
{
	void* pSingleton = *ExeApi::Singleton_Instance();

	if (pSingleton == NULL)
	{
		return NULL;
	}

	return static_cast<BYTE*>(pSingleton) + ExeApi::Singleton_TunerListener();
}

bool IsTunerListening(const BYTE* pListener)
{
	const DWORD dwSocket = *reinterpret_cast<const DWORD*>(
		pListener + ExeApi::TunerListener_ListenSocket()
	);

	return dwSocket != 0;
}

bool s_bTunerAnnouncePending = false;

void AnnounceTuner()
{
	s_bTunerAnnouncePending = false;

	GC.getDLLIFace()->sendChat(
		GetLocalizedText("TXT_KEY_VP_MP_WARNING_FIRETUNER_ENABLED"),
		CHATTARGET_ALL,
		NO_PLAYER
	);
}
} // namespace

//------------------------------------------------------------------------------
const char* Exe::GetReasonName(Reason eReason)
{
	if (eReason < 0 || eReason >= NUM_REASONS)
	{
		return "?";
	}

	return REASON_NAMES[eReason];
}

const char* Exe::GetBuildName()
{
	return ExeBuild::GetName(ExeBuild::GetType());
}

//------------------------------------------------------------------------------
Exe::Reason Exe::CanScheduleResync()
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	if (!HasSymbols(RESYNC_SYMBOLS, _countof(RESYNC_SYMBOLS)))
	{
		return REASON_UNSUPPORTED_EXE;
	}

	if (!GC.getGame().isNetworkMultiPlayer())
	{
		return REASON_NOT_NETWORK_GAME;
	}

	if (!gDLL->IsHost())
	{
		return REASON_NOT_HOST;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryScheduleResync()
{
	const Reason eReason = CanScheduleResync();

	if (eReason != REASON_OK)
	{
		return Refuse("TryScheduleResync", eReason);
	}

	// A byte: the next 3 bytes are not ours
	volatile BYTE* pFlag = ExeApi::NetMessage_WantForceResync();
	*pFlag = 1;

	CUSTOMLOG("Exe: NetMessage_WantForceResync set at %08X",
		(unsigned int)reinterpret_cast<DWORD>(pFlag)
	);

	GC.getDLLIFace()->sendChat(
		GetLocalizedText("TXT_KEY_VP_MP_WARNING_RESYNC_SCHEDULED"),
		CHATTARGET_ALL,
		NO_PLAYER
	);

	return REASON_OK;
}

//------------------------------------------------------------------------------
Exe::Reason Exe::CanDisableEngineYieldIconManager()
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	if (
		!HasSymbols(
			YIELD_ICON_MANAGER_SYMBOLS,
			_countof(YIELD_ICON_MANAGER_SYMBOLS)
		)
	)
	{
		return REASON_UNSUPPORTED_EXE;
	}

	if (GC.GetEngineUserInterface() == NULL)
	{
		return REASON_UNAVAILABLE;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryDisableEngineYieldIconManager()
{
	const Reason eReason = CanDisableEngineYieldIconManager();

	if (eReason != REASON_OK)
	{
		return Refuse("TryDisableEngineYieldIconManager", eReason);
	}

	void* pBuddy = FindInterfaceBuddy();

	if (pBuddy == NULL)
	{
		return Refuse("TryDisableEngineYieldIconManager", REASON_UNAVAILABLE);
	}

	void* pManager = static_cast<BYTE*>(pBuddy)
		+ ExeApi::InterfaceBuddy_YieldIconManager();

	ExeApi::YieldIconManager_UnregisterForEvents(pManager);

	CUSTOMLOG("Exe: YieldIconManager_UnregisterForEvents done for %08X",
		(unsigned int)reinterpret_cast<DWORD>(pManager)
	);

	return REASON_OK;
}

//------------------------------------------------------------------------------
Exe::Reason Exe::CanEnableTunerInMultiplayer()
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	if (!HasSymbols(TUNER_SYMBOLS, _countof(TUNER_SYMBOLS)))
	{
		return REASON_UNSUPPORTED_EXE;
	}

	if (!GC.getGame().isGameMultiPlayer())
	{
		return REASON_NOT_MULTIPLAYER;
	}

	if (*ExeApi::Tuner_Enabled() == 0)
	{
		return REASON_TUNER_OFF;
	}

	const BYTE* pListener = FindTunerListener();

	if (pListener == NULL)
	{
		return REASON_UNAVAILABLE;
	}

	if (IsTunerListening(pListener))
	{
		return REASON_ALREADY_ENABLED;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryEnableTunerInMultiplayer()
{
	const Reason eReason = CanEnableTunerInMultiplayer();

	if (eReason != REASON_OK)
	{
		return Refuse("TryEnableTunerInMultiplayer", eReason);
	}

	BYTE* pListener = FindTunerListener();

	if (!ExeApi::TunerListener_ExitingMultiplayerStagingRoom(pListener))
	{
		CUSTOMLOG("Exe: the tuner port could not be opened (in use?)");
		return Refuse("TryEnableTunerInMultiplayer", REASON_UNAVAILABLE);
	}

	if (!IsTunerListening(pListener))
	{
		return Refuse("TryEnableTunerInMultiplayer", REASON_UNAVAILABLE);
	}

	CUSTOMLOG("Exe: tuner listener %08X reopened",
		(unsigned int)reinterpret_cast<DWORD>(pListener)
	);

	AnnounceTuner();

	return REASON_OK;
}

void Exe::OnGameLoaded()
{
	s_bTunerAnnouncePending = true;
}

void Exe::AnnounceTunerAfterLoad()
{
	if (!s_bTunerAnnouncePending)
	{
		return;
	}

	s_bTunerAnnouncePending = false;

	if (CanEnableTunerInMultiplayer() == REASON_ALREADY_ENABLED)
	{
		AnnounceTuner();
	}
}
