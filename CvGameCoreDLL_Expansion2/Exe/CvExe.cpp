#include "CvGameCoreDLLPCH.h"
#include "CvExe.h"
#include "CvExeApi.h"
#include "CvExeBuild.h"
#include "CvExeSymbols.h"
#include "CvGameCoreUtils.h"

#include <string.h>

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
	"not_ui_thread",
	"wrong_event_channel",
	"not_multiplayer",
	"tuner_off",
	"already_enabled",
	"busy",
};

// The symbols each feature uses
const ExeSymbol RESYNC_SYMBOLS[] =
{
	EXE_Civ5NetMessage_s_wantForceResync,
};

const ExeSymbol YIELD_ICON_MANAGER_SYMBOLS[] =
{
	EXE_InterfaceBuddy_UserInterface,
	EXE_InterfaceBuddy_vftable,
	EXE_InterfaceBuddy_UserInterface_vftable,
	EXE_InterfaceBuddy_YieldIconManager,
	EXE_YieldIconManager_UnregisterForEvents,
	EXE_g_EventSystemLocalMachine,
};

const ExeSymbol TUNER_SYMBOLS[] =
{
	EXE_cvLuaScriptSystem_ms_apSingletonInstance,
	EXE_cvLuaScriptSystem_TunerListener,
	EXE_cvTunerListener_ExitingMultiplayerStagingRoom,
	EXE_cvTunerListener_ListenSocket,
	EXE_g_bEnableTuner,
};

const ExeSymbol CITY_ART_SYMBOLS[] =
{
	EXE_g_cvCityBuildingSim,
	EXE_cvCityBuildingSim_DeferFlags,
	EXE_cvCityBuildingSim_SlotGrid,
	EXE_cvCityBuildingSim_SlotGrid_GetSlot,
	EXE_cvCityBuildingSim_Cities,
	EXE_cvCityBuildingSim_NumCities,
	EXE_cvCityBuildingSim_CityDataSize,
	EXE_cvCityBuildingSim_MarkDirty,
	EXE_CityData_Free,
	EXE_CityData_Hex,
	EXE_CityData_PalaceArtEntry,
	EXE_CityData_VisHandle,
	EXE_g_cvCityVisSystem,
	EXE_cvCityVisSystem_RemoveWonder,
};

// cvCityBuildingSim_MarkDirty flags the engine's WONDER_REMOVED handler
// passes when it changes the Palace position
const int CITY_ART_DIRTY_PALACE = 13;

// Buildings with this PreferredDisplayPosition go to the Palace position
const int CITY_ART_PALACE_POSITION = 32;

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

//! True if [p, p + uiSize) is committed, readable memory in one region.
//! Asks the memory manager instead of touching the memory, so unlike
//! IsBadReadPtr it cannot trip a guard page.
bool IsReadable(const void* p, size_t uiSize)
{
	MEMORY_BASIC_INFORMATION kInfo;

	if (VirtualQuery(p, &kInfo, sizeof(kInfo)) != sizeof(kInfo))
	{
		return false;
	}

	const DWORD dwReadable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY
		| PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	if (
		kInfo.State != MEM_COMMIT
		|| (kInfo.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0
		|| (kInfo.Protect & dwReadable) == 0
	)
	{
		return false;
	}

	const BYTE* pEnd =
		static_cast<const BYTE*>(kInfo.BaseAddress) + kInfo.RegionSize;

	return static_cast<const BYTE*>(p) + uiSize <= pEnd;
}

DWORD ReadVftable(DWORD dwObject)
{
	const void* p = reinterpret_cast<const void*>(dwObject);
	DWORD dwVftable = 0;

	// memcpy: the address is computed and not known to be aligned
	if (IsReadable(p, sizeof(dwVftable)))
	{
		memcpy(&dwVftable, p, sizeof(dwVftable));
	}

	return dwVftable;
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
	void* pSingleton = *ExeApi::cvLuaScriptSystem_ms_apSingletonInstance();

	if (pSingleton == NULL)
	{
		return NULL;
	}

	return static_cast<BYTE*>(pSingleton)
		+ ExeApi::cvLuaScriptSystem_TunerListener();
}

bool IsTunerListening(const BYTE* pListener)
{
	const DWORD dwSocket = *reinterpret_cast<const DWORD*>(
		pListener + ExeApi::cvTunerListener_ListenSocket()
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
	volatile BYTE* pFlag = ExeApi::Civ5NetMessage_s_wantForceResync();
	*pFlag = 1;

	CUSTOMLOG("Exe: Civ5NetMessage_s_wantForceResync set at %08X",
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

	// Handlers are registered per channel; the engine registered these on
	// channel 0, so on another channel the unregister would find nothing
	if (*ExeApi::g_EventSystemLocalMachine() != 0)
	{
		return REASON_WRONG_EVENT_CHANNEL;
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
Exe::Reason Exe::CanRepairCityArt()
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	if (!HasSymbols(CITY_ART_SYMBOLS, _countof(CITY_ART_SYMBOLS)))
	{
		return REASON_UNSUPPORTED_EXE;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryRepairCityArt(const CvCity& kCity)
{
	const Reason eReason = CanRepairCityArt();

	if (eReason != REASON_OK)
	{
		return Refuse("TryRepairCityArt", eReason);
	}

	// The records belong to the main thread
	if (gDLL->IsGameCoreThread())
	{
		return Refuse("TryRepairCityArt", REASON_NOT_UI_THREAD);
	}

	BYTE* pSim = ExeApi::g_cvCityBuildingSim();

	// The handlers queue their messages instead while this is set
	if ((pSim[ExeApi::cvCityBuildingSim_DeferFlags()] & 1) != 0)
	{
		return REASON_BUSY;
	}

	// The engine's hex: x shifted by half the row
	const int iY = kCity.getY();
	const int iX = kCity.getX() - (iY >> 1);
	const int aiHex[3] = { iX, iY, -(iX + iY) };

	const unsigned int uiSlot = ExeApi::cvCityBuildingSim_SlotGrid_GetSlot(
		pSim + ExeApi::cvCityBuildingSim_SlotGrid(), aiHex
	);
	const DWORD dwNumCities =
		*reinterpret_cast<const DWORD*>(
			pSim + ExeApi::cvCityBuildingSim_NumCities()
		);

	if (uiSlot == 0x7FFF || uiSlot >= dwNumCities)
	{
		CUSTOMLOG("Exe: TryRepairCityArt: no art record for %s (%d, %d)",
			kCity.getName().c_str(), kCity.getX(), kCity.getY());
		return REASON_UNAVAILABLE;
	}

	BYTE* pCities = *reinterpret_cast<BYTE**>(
		pSim + ExeApi::cvCityBuildingSim_Cities()
	);
	BYTE* pData = pCities + uiSlot * ExeApi::cvCityBuildingSim_CityDataSize();
	const short* psHex =
		reinterpret_cast<const short*>(pData + ExeApi::CityData_Hex());

	if (pData[ExeApi::CityData_Free()] != 0 || psHex[0] != iX || psHex[1] != iY)
	{
		CUSTOMLOG("Exe: TryRepairCityArt: record %u is not %s's (free %d, hex %d, %d; expected %d, %d)",
			uiSlot, kCity.getName().c_str(),
			static_cast<int>(pData[ExeApi::CityData_Free()]),
			static_cast<int>(psHex[0]), static_cast<int>(psHex[1]),
			iX, iY);
		return REASON_UNAVAILABLE;
	}

	// The engine's art entry for the building (by culture and era), not a
	// BuildingTypes
	int* piPalace =
		reinterpret_cast<int*>(pData + ExeApi::CityData_PalaceArtEntry());
	const int iArtEntry = *piPalace;

	if (iArtEntry == -1)
	{
		return REASON_OK;
	}

	// A building the city has belongs there; the engine replaces the entry
	// itself when that building's art changes
	for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(i);
		const CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);

		if (
			pkBuilding != NULL
			&& pkBuilding->GetPreferredDisplayPosition() == CITY_ART_PALACE_POSITION
			&& kCity.GetCityBuildings()->GetNumBuilding(eBuilding) > 0
		)
		{
			return REASON_OK;
		}
	}

	// What the engine does when it removes a building from its other
	// positions: clear the record, lay the city out again, remove the model
	*piPalace = -1;
	ExeApi::cvCityBuildingSim_MarkDirty(pSim, aiHex, CITY_ART_DIRTY_PALACE);
	ExeApi::cvCityVisSystem_RemoveWonder(
		ExeApi::g_cvCityVisSystem(),
		pData + ExeApi::CityData_VisHandle(),
		&iArtEntry
	);

	CUSTOMLOG("Exe: TryRepairCityArt: removed art entry %d from %s's Palace position (record %u)",
		iArtEntry, kCity.getName().c_str(), uiSlot);

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

	if (*ExeApi::g_bEnableTuner() == 0)
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

	if (!ExeApi::cvTunerListener_ExitingMultiplayerStagingRoom(pListener))
	{
		CUSTOMLOG("Exe: the tuner could not bind or listen on its port");

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
