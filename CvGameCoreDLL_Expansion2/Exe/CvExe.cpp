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
	"pending",
	"not_implemented",
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

const ExeSymbol UI_COMMAND_STREAM_SYMBOLS[] =
{
	EXE_g_pUIManager,
	EXE_UIManager_CommandStream,
	EXE_UIManager_CommandStreamLeft,
	EXE_UIManager_CommandStreamRight,
	EXE_CCommandStream_SetCount,
	EXE_CCommandStream_CommandCap,
	EXE_CCommandStream_CommandCount,
	EXE_CCommandStream_Commands,
	EXE_CCommandStream_Data,
	EXE_CCommandStream_DataSize,
	EXE_CCommandStream_DataUsed,
	EXE_operator_new_array,
	EXE_operator_delete_array,
	EXE_FireMallocAlignedNoTracking,
	EXE_FireFreeAlignedNoTracking,
};

const ExeSymbol UI_COMMAND_STREAM_USAGE_SYMBOLS[] =
{
	EXE_g_pUIManager,
	EXE_UIManager_CommandStream,
	EXE_CCommandStream_DataSize,
	EXE_CCommandStream_DataUsedOther,
};

const ExeSymbol UI_COMMAND_STREAM_HALVES_SYMBOLS[] =
{
	EXE_CCommandStream_CommandsOther,
	EXE_CCommandStream_DataOther,
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
namespace
{
// The engine's 1 MB and 4096 draws fit about 450 unit flags, unchecked
const DWORD UI_GROWTH = 4;
const DWORD UI_DATA_SIZE = UI_GROWTH * 1024 * 1024;
const DWORD UI_COMMAND_CAP = UI_GROWTH * 4096;
const DWORD COMMAND_SIZE = 0x20;
const DWORD DATA_ALIGNMENT = 0x10;

enum GrowResult
{
	GROW_DONE,
	GROW_PENDING,
	GROW_FAILED
};

//! A DX11 stream between its two steps
struct StreamGrowth
{
	BYTE* pStream;
	DWORD dwData;
	DWORD dwCommands;
	DWORD dwOldData;
	DWORD dwOldCommands;
};

// Two with stereo 3D
StreamGrowth s_aGrowth[2];

//! DX11 swaps two halves of each buffer every frame; DX9 has one buffer
bool HasHalves()
{
	return ExeSymbols::IsInBuild(EXE_CCommandStream_DataOther);
}

DWORD StreamFieldsEnd()
{
	DWORD dwEnd = std::max(
		ExeApi::CCommandStream_Data(),
		ExeApi::CCommandStream_DataUsed()
	);

	if (HasHalves())
	{
		dwEnd = std::max(dwEnd, ExeApi::CCommandStream_DataOther());
		dwEnd = std::max(dwEnd, ExeApi::CCommandStream_CommandsOther());
	}

	return dwEnd + sizeof(DWORD);
}

DWORD& Field(BYTE* pStream, DWORD dwOffset)
{
	return *reinterpret_cast<DWORD*>(pStream + dwOffset);
}

DWORD Lower(DWORD dwA, DWORD dwB)
{
	return dwA < dwB ? dwA : dwB;
}

bool IsOneBlock(DWORD dwA, DWORD dwB, DWORD dwHalf)
{
	if (dwA == 0 || dwB == 0)
	{
		return false;
	}

	return dwA + dwHalf == dwB || dwB + dwHalf == dwA;
}

bool IsGrown(BYTE* pStream)
{
	const DWORD dwDataSize =
		Field(pStream, ExeApi::CCommandStream_DataSize());
	const DWORD dwCommandCap =
		Field(pStream, ExeApi::CCommandStream_CommandCap());

	return dwDataSize >= UI_DATA_SIZE && dwCommandCap >= UI_COMMAND_CAP;
}

//! Between frames the current buffer is empty and nothing reads it
bool IsCurrentBufferFree(BYTE* pStream)
{
	return Field(pStream, ExeApi::CCommandStream_DataUsed()) == 0
		&& Field(pStream, ExeApi::CCommandStream_CommandCount()) == 0
		&& Field(pStream, ExeApi::CCommandStream_SetCount()) == 0;
}

//! Both blocks, from the allocators the engine frees them with, or neither
bool AllocateBlocks(
	BYTE* pStream,
	DWORD dwDataSize,
	DWORD dwCommandsSize,
	void*& pData,
	void*& pCommands
)
{
	pData = ExeApi::FireMallocAlignedNoTracking(dwDataSize, DATA_ALIGNMENT);
	pCommands = ExeApi::operator_new_array(dwCommandsSize);

	if (pData != NULL && pCommands != NULL)
	{
		return true;
	}

	CUSTOMLOG("Exe: CCommandStream %08X: out of memory",
		(unsigned int)reinterpret_cast<DWORD>(pStream)
	);

	if (pData != NULL)
	{
		ExeApi::FireFreeAlignedNoTracking(pData);
	}

	if (pCommands != NULL)
	{
		ExeApi::operator_delete_array(pCommands);
	}

	return false;
}

void FinishGrowth(BYTE* pStream, void* pOldData, void* pOldCommands)
{
	Field(pStream, ExeApi::CCommandStream_DataSize()) = UI_DATA_SIZE;
	Field(pStream, ExeApi::CCommandStream_CommandCap()) = UI_COMMAND_CAP;
	ExeApi::FireFreeAlignedNoTracking(pOldData);
	ExeApi::operator_delete_array(pOldCommands);

	CUSTOMLOG("Exe: CCommandStream %08X grown to %u bytes and %u commands",
		(unsigned int)reinterpret_cast<DWORD>(pStream),
		(unsigned int)UI_DATA_SIZE,
		(unsigned int)UI_COMMAND_CAP
	);
}

StreamGrowth* FindGrowth(BYTE* pStream)
{
	for (unsigned int i = 0; i < _countof(s_aGrowth); ++i)
	{
		if (s_aGrowth[i].pStream == pStream)
		{
			return &s_aGrowth[i];
		}
	}

	return NULL;
}

StreamGrowth* StartGrowth(BYTE* pStream)
{
	const DWORD dwDataSize =
		Field(pStream, ExeApi::CCommandStream_DataSize());
	const DWORD dwCommandCap =
		Field(pStream, ExeApi::CCommandStream_CommandCap());
	const DWORD dwData = Field(pStream, ExeApi::CCommandStream_Data());
	const DWORD dwDataOther =
		Field(pStream, ExeApi::CCommandStream_DataOther());
	const DWORD dwCommands =
		Field(pStream, ExeApi::CCommandStream_Commands());
	const DWORD dwCommandsOther =
		Field(pStream, ExeApi::CCommandStream_CommandsOther());

	const DWORD dwCommandsSize = dwCommandCap * COMMAND_SIZE;

	if (
		!IsOneBlock(dwData, dwDataOther, dwDataSize)
		|| !IsOneBlock(dwCommands, dwCommandsOther, dwCommandsSize)
	)
	{
		CUSTOMLOG("Exe: CCommandStream %08X has an unexpected layout",
			(unsigned int)reinterpret_cast<DWORD>(pStream)
		);
		return NULL;
	}

	StreamGrowth* pGrowth = FindGrowth(NULL);

	if (pGrowth == NULL)
	{
		return NULL;
	}

	void* pData = NULL;
	void* pCommands = NULL;

	if (
		!AllocateBlocks(
			pStream,
			2 * UI_DATA_SIZE,
			2 * UI_COMMAND_CAP * COMMAND_SIZE,
			pData,
			pCommands
		)
	)
	{
		return NULL;
	}

	pGrowth->pStream = pStream;
	pGrowth->dwData = reinterpret_cast<DWORD>(pData);
	pGrowth->dwCommands = reinterpret_cast<DWORD>(pCommands);
	pGrowth->dwOldData = Lower(dwData, dwDataOther);
	pGrowth->dwOldCommands = Lower(dwCommands, dwCommandsOther);

	return pGrowth;
}

GrowResult GrowSingleStream(BYTE* pStream)
{
	if (!IsCurrentBufferFree(pStream))
	{
		return GROW_PENDING;
	}

	void* pData = NULL;
	void* pCommands = NULL;

	if (
		!AllocateBlocks(
			pStream,
			UI_DATA_SIZE,
			UI_COMMAND_CAP * COMMAND_SIZE,
			pData,
			pCommands
		)
	)
	{
		return GROW_FAILED;
	}

	DWORD& dwData = Field(pStream, ExeApi::CCommandStream_Data());
	DWORD& dwCommands = Field(pStream, ExeApi::CCommandStream_Commands());
	void* pOldData = reinterpret_cast<void*>(dwData);
	void* pOldCommands = reinterpret_cast<void*>(dwCommands);

	dwData = reinterpret_cast<DWORD>(pData);
	dwCommands = reinterpret_cast<DWORD>(pCommands);
	FinishGrowth(pStream, pOldData, pOldCommands);

	return GROW_DONE;
}

//! DX11: one half per call, a frame apart. In between, the engine's
//! destructor would free a wrong pointer, but it only runs at exit.
GrowResult GrowStream(BYTE* pStream)
{
	if (IsGrown(pStream))
	{
		return GROW_DONE;
	}

	if (!HasHalves())
	{
		return GrowSingleStream(pStream);
	}

	StreamGrowth* pGrowth = FindGrowth(pStream);

	if (pGrowth == NULL)
	{
		pGrowth = StartGrowth(pStream);

		if (pGrowth == NULL)
		{
			return GROW_FAILED;
		}
	}

	if (!IsCurrentBufferFree(pStream))
	{
		return GROW_PENDING;
	}

	const DWORD aData[2] =
	{
		pGrowth->dwData,
		pGrowth->dwData + UI_DATA_SIZE
	};
	const DWORD aCommands[2] =
	{
		pGrowth->dwCommands,
		pGrowth->dwCommands + UI_COMMAND_CAP * COMMAND_SIZE
	};

	DWORD& dwData = Field(pStream, ExeApi::CCommandStream_Data());
	DWORD& dwCommands = Field(pStream, ExeApi::CCommandStream_Commands());
	const DWORD dwDataOther =
		Field(pStream, ExeApi::CCommandStream_DataOther());

	if (dwData != aData[0] && dwData != aData[1])
	{
		const int iHalf = dwDataOther == aData[0] ? 1 : 0;
		dwData = aData[iHalf];
		dwCommands = aCommands[iHalf];
	}

	if (dwDataOther != aData[0] && dwDataOther != aData[1])
	{
		return GROW_PENDING;
	}

	// Both halves are new: nothing refers to the old blocks any more
	FinishGrowth(
		pStream,
		reinterpret_cast<void*>(pGrowth->dwOldData),
		reinterpret_cast<void*>(pGrowth->dwOldCommands)
	);
	pGrowth->pStream = NULL;

	return GROW_DONE;
}
} // namespace

//------------------------------------------------------------------------------
Exe::Reason Exe::CanGrowUICommandStream()
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	// Its frame end also copies the data into a 1 MB GPU buffer
	if (ExeBuild::GetType() == ExeBuild::TABLET)
	{
		return REASON_NOT_IMPLEMENTED;
	}

	if (
		!HasSymbols(
			UI_COMMAND_STREAM_SYMBOLS,
			_countof(UI_COMMAND_STREAM_SYMBOLS)
		)
	)
	{
		return REASON_UNSUPPORTED_EXE;
	}

	if (
		HasHalves()
		&& !HasSymbols(
			UI_COMMAND_STREAM_HALVES_SYMBOLS,
			_countof(UI_COMMAND_STREAM_HALVES_SYMBOLS)
		)
	)
	{
		return REASON_UNSUPPORTED_EXE;
	}

	if (*ExeApi::g_pUIManager() == NULL)
	{
		return REASON_UNAVAILABLE;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryGrowUICommandStream()
{
	const Reason eReason = CanGrowUICommandStream();

	if (eReason != REASON_OK)
	{
		return Refuse("TryGrowUICommandStream", eReason);
	}

	BYTE* pUIManager = static_cast<BYTE*>(*ExeApi::g_pUIManager());
	const DWORD aOffsets[] =
	{
		ExeApi::UIManager_CommandStreamLeft(),
		ExeApi::UIManager_CommandStreamRight(),
		ExeApi::UIManager_CommandStream(),
	};
	const DWORD dwStreamSize = StreamFieldsEnd();

	BYTE* apStreams[_countof(aOffsets)];
	BYTE** const ppStreamsBegin = apStreams;
	BYTE** ppStreamsEnd = apStreams;

	for (unsigned int i = 0; i < _countof(aOffsets); ++i)
	{
		BYTE* const* ppStream =
			reinterpret_cast<BYTE* const*>(pUIManager + aOffsets[i]);

		if (IsBadReadPtr(ppStream, sizeof(BYTE*)))
		{
			return Refuse("TryGrowUICommandStream", REASON_UNAVAILABLE);
		}

		BYTE* pStream = *ppStream;

		if (
			pStream == NULL
			|| std::find(ppStreamsBegin, ppStreamsEnd, pStream) != ppStreamsEnd
		)
		{
			continue;
		}

		if (IsBadWritePtr(pStream, dwStreamSize))
		{
			return Refuse("TryGrowUICommandStream", REASON_UNAVAILABLE);
		}

		*ppStreamsEnd++ = pStream;
	}

	bool bPending = false;

	for (BYTE** pp = ppStreamsBegin; pp != ppStreamsEnd; ++pp)
	{
		const GrowResult eResult = GrowStream(*pp);

		if (eResult == GROW_FAILED)
		{
			return Refuse("TryGrowUICommandStream", REASON_UNAVAILABLE);
		}

		bPending = bPending || eResult == GROW_PENDING;
	}

	return bPending ? REASON_PENDING : REASON_OK;
}

//------------------------------------------------------------------------------
Exe::Reason Exe::CanGetUICommandStreamUsage()
{
	if (
		!HasSymbols(
			UI_COMMAND_STREAM_USAGE_SYMBOLS,
			_countof(UI_COMMAND_STREAM_USAGE_SYMBOLS)
		)
	)
	{
		return REASON_UNSUPPORTED_EXE;
	}

	if (*ExeApi::g_pUIManager() == NULL)
	{
		return REASON_UNAVAILABLE;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryGetUICommandStreamUsage(
	unsigned int& uiUsed,
	unsigned int& uiCapacity
)
{
	const Reason eReason = CanGetUICommandStreamUsage();

	if (eReason != REASON_OK)
	{
		return Refuse("TryGetUICommandStreamUsage", eReason);
	}

	BYTE* pUIManager = static_cast<BYTE*>(*ExeApi::g_pUIManager());
	BYTE* const* ppStream = reinterpret_cast<BYTE* const*>(
		pUIManager + ExeApi::UIManager_CommandStream()
	);
	const DWORD dwFieldsEnd = std::max(
		ExeApi::CCommandStream_DataSize(),
		ExeApi::CCommandStream_DataUsedOther()
	) + sizeof(DWORD);

	if (
		IsBadReadPtr(ppStream, sizeof(BYTE*))
		|| *ppStream == NULL
		|| IsBadReadPtr(*ppStream, dwFieldsEnd)
	)
	{
		return Refuse("TryGetUICommandStreamUsage", REASON_UNAVAILABLE);
	}

	uiUsed = Field(*ppStream, ExeApi::CCommandStream_DataUsedOther());
	uiCapacity = Field(*ppStream, ExeApi::CCommandStream_DataSize());

	return REASON_OK;
}
