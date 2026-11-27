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
	"pending",
	"not_implemented",
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

const ExeSymbol EVENT_STREAM_SYMBOLS[] =
{
	EXE_g_EventSystemLocalMachine,
	EXE_g_EventSystemLocalMachine_Containers,
	EXE_g_EventSystemLocalMachine_MaxPublishedSize,
	EXE_LocalMachineContainer_Size,
	EXE_LocalMachineContainer_BufferSize,
	EXE_LocalMachineContainer_SwapCounter,
	EXE_LocalMachineContainer_BufferHeaderSize,
};

// The handler tables before the containers leave room for two channels
const DWORD NUM_EVENT_CHANNELS = 2;

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

const ExeSymbol FOG_GRID_SYMBOLS[] =
{
	EXE_FogGrid_OnHexFOWStateChanged,
	EXE_FogGrid_ClearHandle,
	EXE_FogGrid_ClearAllHighlightsLoop,
};

// The loop counts, from the symbols. Before: mov ecx,[esi+44h] (width);
// imul ecx,ecx; test ecx,ecx; jbe <past the loop>. After: the same with
// imul ecx,[esi+48h] (height).
const DWORD FOG_HEX_CHANGED_LOOP = 0x11;
const BYTE FOG_HEX_CHANGED_ORIGINAL[] =
{
	0x8B, 0x4E, 0x44, 0x0F, 0xAF, 0xC9, 0x85, 0xC9,
	0x0F, 0x86, 0x9E, 0x00, 0x00, 0x00
};
const DWORD FOG_CLEAR_HANDLE_LOOP = 0x23;
const BYTE FOG_CLEAR_HANDLE_ORIGINAL[] =
{
	0x8B, 0x4E, 0x44, 0x0F, 0xAF, 0xC9, 0x85, 0xC9, 0x76, 0x18
};
// jecxz to the jbe's target, then a nop: fits in place
const BYTE FOG_CLEAR_HANDLE_FIXED[] =
{
	0x8B, 0x4E, 0x44, 0x0F, 0xAF, 0x4E, 0x48, 0xE3, 0x19, 0x90
};
// The third loop counts in edx (jecxz does not apply); its 16 bytes before
// the loop body end in 3 bytes of padding, so the count fits in place
const DWORD FOG_CLEAR_ALL_COUNT = 0x10; // before the symbol
const BYTE FOG_CLEAR_ALL_ORIGINAL[] =
{
	0x8B, 0x51, 0x44, 0x0F, 0xAF, 0xD2, 0x85, 0xD2, 0x76, 0x16,
	0x8D, 0x41, 0x68, 0x8D, 0x49, 0x00
};
// mov edx,[ecx+44h]; imul edx,[ecx+48h]; test edx,edx; jbe <past the
// loop>; lea eax,[ecx+68h]; 2-byte nop
const BYTE FOG_CLEAR_ALL_FIXED[] =
{
	0x8B, 0x51, 0x44, 0x0F, 0xAF, 0x51, 0x48, 0x85, 0xD2, 0x76, 0x15,
	0x8D, 0x41, 0x68, 0x66, 0x90
};
// The first loop's jbe target is out of jecxz range, so its count moves to
// a thunk: this, then jbe <past the loop>, jmp <back>
const BYTE FOG_THUNK_HEAD[] =
{
	0x8B, 0x4E, 0x44, 0x0F, 0xAF, 0x4E, 0x48, 0x85, 0xC9
};
const unsigned int FOG_THUNK_SIZE = sizeof(FOG_THUNK_HEAD) + 6 + 5;
const BYTE X86_NOP = 0x90;
const BYTE X86_JMP_REL32 = 0xE9;

const ExeSymbol COAST_GRID_STACK_SYMBOLS[] =
{
	EXE_TerrainSystem_Init_CoastGridInit,
	EXE_CoastGrid_Init,
};

// TerrainSystem_Init_CoastGridInit + this = the call's E8 byte
const DWORD COAST_GRID_CALL_OFFSET = 6;

// CoastGrid::Init needs about 48 * (width + 4) * (height + 4) bytes: 1.6 MB
// at most on the maps the fog of war grid allows (32,768 plots, at most 252
// columns)
const SIZE_T COAST_GRID_STACK_SIZE = 4 * 1024 * 1024;
const SIZE_T COAST_GRID_THUNK_SIZE = 4096;

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

DWORD ReadDword(DWORD dwAddress)
{
	return *reinterpret_cast<const volatile DWORD*>(dwAddress);
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

DWORD Rel32(DWORD dwFrom, DWORD dwTo)
{
	return dwTo - dwFrom;
}

bool WriteCode(DWORD dwAddress, const BYTE* pBytes, unsigned int uiSize)
{
	void* p = reinterpret_cast<void*>(dwAddress);
	DWORD dwOld = 0;

	if (!VirtualProtect(p, uiSize, PAGE_EXECUTE_READWRITE, &dwOld))
	{
		return false;
	}

	memcpy(p, pBytes, uiSize);
	VirtualProtect(p, uiSize, dwOld, &dwOld);
	FlushInstructionCache(GetCurrentProcess(), p, uiSize);

	return true;
}

//! The first loop already jumps to a thunk of ours (a DLL load that did not
//! put the loops back fixed it).
bool IsFogHexChangedFixed(DWORD dwLoop)
{
	const BYTE* p = reinterpret_cast<const BYTE*>(dwLoop);

	if (p[0] != X86_JMP_REL32)
	{
		return false;
	}

	for (unsigned int i = 5; i < sizeof(FOG_HEX_CHANGED_ORIGINAL); ++i)
	{
		if (p[i] != X86_NOP)
		{
			return false;
		}
	}

	DWORD dwRel = 0;
	memcpy(&dwRel, p + 1, sizeof(dwRel));
	const DWORD dwThunk = dwLoop + 5 + dwRel;

	return IsReadable(reinterpret_cast<const void*>(dwThunk), FOG_THUNK_SIZE)
		&& memcmp(reinterpret_cast<const void*>(dwThunk), FOG_THUNK_HEAD,
			sizeof(FOG_THUNK_HEAD)) == 0;
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

// While this DLL has the fog of war loops fixed: whether it does, and the
// first loop's thunk if this DLL made it
bool s_bFogGridFixed = false;
BYTE* s_pFogGridThunk = NULL;

// While the CoastGrid_Init call is redirected: its thunk, the stack the
// thunk switches to, and the call's original rel32
BYTE* s_pCoastGridThunk = NULL;
BYTE* s_pCoastGridStack = NULL;
DWORD s_dwCoastGridCallRel = 0;

void PutBytes(BYTE*& pOut, const char* szBytes, unsigned int uiCount)
{
	memcpy(pOut, szBytes, uiCount);
	pOut += uiCount;
}

void PutDword(BYTE*& pOut, DWORD dwValue)
{
	memcpy(pOut, &dwValue, sizeof(dwValue));
	pOut += sizeof(dwValue);
}

//! Writes the thunk the CoastGrid_Init call is pointed at: it runs the
//! call (thiscall, one stack argument, retn 4) on [dwBase, dwTop] and
//! returns its result. The TEB's StackBase, StackLimit and
//! DeallocationStack follow the switch, as _alloca's stack probe and
//! exception dispatch check them. Returns the thunk's size.
unsigned int WriteCoastGridThunk(BYTE* pThunk, DWORD dwTop, DWORD dwBase, DWORD dwTarget)
{
	BYTE* p = pThunk;
	PutBytes(p, "\x55\x8B\xEC", 3);                         // push ebp; mov ebp,esp
	PutBytes(p, "\x64\xFF\x35\x04\x00\x00\x00", 7);     // push fs:[4] (StackBase)
	PutBytes(p, "\x64\xFF\x35\x08\x00\x00\x00", 7);     // push fs:[8] (StackLimit)
	PutBytes(p, "\x64\xFF\x35\x0C\x0E\x00\x00", 7);     // push fs:[0xE0C] (DeallocationStack)
	PutBytes(p, "\x8B\x55\x08", 3);                         // mov edx,[ebp+8] (the argument)
	PutBytes(p, "\xB8", 1); PutDword(p, dwTop);                // mov eax,top
	PutBytes(p, "\x64\xA3\x04\x00\x00\x00", 6);          // mov fs:[4],eax
	PutBytes(p, "\xB8", 1); PutDword(p, dwBase);               // mov eax,base
	PutBytes(p, "\x64\xA3\x08\x00\x00\x00", 6);          // mov fs:[8],eax
	PutBytes(p, "\x64\xA3\x0C\x0E\x00\x00", 6);          // mov fs:[0xE0C],eax
	PutBytes(p, "\xBC", 1); PutDword(p, dwTop);                // mov esp,top
	PutBytes(p, "\x52", 1);                                   // push edx
	PutBytes(p, "\xE8", 1);                                   // call CoastGrid_Init (ecx = this, as passed in)
	PutDword(p, dwTarget - (reinterpret_cast<DWORD>(p) + 4));
	PutBytes(p, "\x8B\x55\xFC\x64\x89\x15\x04\x00\x00\x00", 10); // restore fs:[4]
	PutBytes(p, "\x8B\x55\xF8\x64\x89\x15\x08\x00\x00\x00", 10); // restore fs:[8]
	PutBytes(p, "\x8B\x55\xF4\x64\x89\x15\x0C\x0E\x00\x00", 10); // restore fs:[0xE0C]
	PutBytes(p, "\x8B\xE5\x5D\xC2\x04\x00", 6);          // mov esp,ebp; pop ebp; ret 4 (eax kept)
	return static_cast<unsigned int>(p - pThunk);
}

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
Exe::Reason Exe::CanFixFogGridWidth()
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	if (!HasSymbols(FOG_GRID_SYMBOLS, _countof(FOG_GRID_SYMBOLS)))
	{
		return REASON_UNSUPPORTED_EXE;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryFixFogGridWidth()
{
	const Reason eReason = CanFixFogGridWidth();

	if (eReason != REASON_OK)
	{
		return Refuse("TryFixFogGridWidth", eReason);
	}

	const DWORD dwLoop1 = ExeApi::FogGrid_OnHexFOWStateChanged()
		+ FOG_HEX_CHANGED_LOOP;
	const DWORD dwLoop2 = ExeApi::FogGrid_ClearHandle() + FOG_CLEAR_HANDLE_LOOP;
	const DWORD dwLoop3 = ExeApi::FogGrid_ClearAllHighlightsLoop()
		- FOG_CLEAR_ALL_COUNT;
	const void* p1 = reinterpret_cast<const void*>(dwLoop1);
	const void* p2 = reinterpret_cast<const void*>(dwLoop2);
	const void* p3 = reinterpret_cast<const void*>(dwLoop3);

	if (
		!IsReadable(p1, sizeof(FOG_HEX_CHANGED_ORIGINAL))
		|| !IsReadable(p2, sizeof(FOG_CLEAR_HANDLE_ORIGINAL))
		|| !IsReadable(p3, sizeof(FOG_CLEAR_ALL_ORIGINAL))
	)
	{
		return Refuse("TryFixFogGridWidth", REASON_UNAVAILABLE);
	}

	const bool bFixed1 = IsFogHexChangedFixed(dwLoop1);
	const bool bFixed2 = memcmp(p2, FOG_CLEAR_HANDLE_FIXED,
		sizeof(FOG_CLEAR_HANDLE_FIXED)) == 0;
	const bool bFixed3 = memcmp(p3, FOG_CLEAR_ALL_FIXED,
		sizeof(FOG_CLEAR_ALL_FIXED)) == 0;

	if (
		(!bFixed1 && memcmp(p1, FOG_HEX_CHANGED_ORIGINAL,
			sizeof(FOG_HEX_CHANGED_ORIGINAL)) != 0)
		|| (!bFixed2 && memcmp(p2, FOG_CLEAR_HANDLE_ORIGINAL,
			sizeof(FOG_CLEAR_HANDLE_ORIGINAL)) != 0)
		|| (!bFixed3 && memcmp(p3, FOG_CLEAR_ALL_ORIGINAL,
			sizeof(FOG_CLEAR_ALL_ORIGINAL)) != 0)
	)
	{
		CUSTOMLOG("Exe: fog grid loops at %08X / %08X / %08X are not the expected code",
			(unsigned int)dwLoop1,
			(unsigned int)dwLoop2,
			(unsigned int)dwLoop3
		);
		return Refuse("TryFixFogGridWidth", REASON_UNAVAILABLE);
	}

	if (!bFixed2 && !WriteCode(dwLoop2, FOG_CLEAR_HANDLE_FIXED,
		sizeof(FOG_CLEAR_HANDLE_FIXED)))
	{
		return Refuse("TryFixFogGridWidth", REASON_UNAVAILABLE);
	}

	if (!bFixed3 && !WriteCode(dwLoop3, FOG_CLEAR_ALL_FIXED,
		sizeof(FOG_CLEAR_ALL_FIXED)))
	{
		return Refuse("TryFixFogGridWidth", REASON_UNAVAILABLE);
	}

	if (!bFixed1)
	{
		// Freed by UndoFixFogGridWidth after the jump to it is gone
		BYTE* pThunk = static_cast<BYTE*>(VirtualAlloc(NULL, FOG_THUNK_SIZE,
			MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

		if (pThunk == NULL)
		{
			return Refuse("TryFixFogGridWidth", REASON_UNAVAILABLE);
		}

		const DWORD dwThunk = reinterpret_cast<DWORD>(pThunk);
		const DWORD dwBack = dwLoop1 + sizeof(FOG_HEX_CHANGED_ORIGINAL);
		DWORD dwPast = 0;
		memcpy(&dwPast, static_cast<const BYTE*>(p1) + 10, sizeof(dwPast));
		dwPast += dwBack;

		BYTE* pAt = pThunk;
		memcpy(pAt, FOG_THUNK_HEAD, sizeof(FOG_THUNK_HEAD));
		pAt += sizeof(FOG_THUNK_HEAD);
		// jbe rel32
		pAt[0] = 0x0F;
		pAt[1] = 0x86;
		const DWORD dwPastRel = Rel32(reinterpret_cast<DWORD>(pAt) + 6, dwPast);
		memcpy(pAt + 2, &dwPastRel, sizeof(dwPastRel));
		pAt += 6;
		pAt[0] = X86_JMP_REL32;
		const DWORD dwBackRel = Rel32(reinterpret_cast<DWORD>(pAt) + 5, dwBack);
		memcpy(pAt + 1, &dwBackRel, sizeof(dwBackRel));

		DWORD dwOld = 0;
		VirtualProtect(pThunk, FOG_THUNK_SIZE, PAGE_EXECUTE_READ, &dwOld);
		FlushInstructionCache(GetCurrentProcess(), pThunk, FOG_THUNK_SIZE);

		BYTE aJump[sizeof(FOG_HEX_CHANGED_ORIGINAL)];
		memset(aJump, X86_NOP, sizeof(aJump));
		aJump[0] = X86_JMP_REL32;
		const DWORD dwThunkRel = Rel32(dwLoop1 + 5, dwThunk);
		memcpy(aJump + 1, &dwThunkRel, sizeof(dwThunkRel));

		if (!WriteCode(dwLoop1, aJump, sizeof(aJump)))
		{
			VirtualFree(pThunk, 0, MEM_RELEASE);
			return Refuse("TryFixFogGridWidth", REASON_UNAVAILABLE);
		}

		s_pFogGridThunk = pThunk;
	}

	s_bFogGridFixed = true;

	CUSTOMLOG("Exe: fog grid loops count width * height (%s)",
		bFixed1 && bFixed2 && bFixed3 ? "already fixed" : "fixed now");

	return REASON_OK;
}

void Exe::UndoFixFogGridWidth()
{
	if (!s_bFogGridFixed)
	{
		return;
	}

	// The first loop first: after it nothing jumps to the thunk
	const DWORD dwLoop1 = ExeApi::FogGrid_OnHexFOWStateChanged() + FOG_HEX_CHANGED_LOOP;
	const DWORD dwLoop2 = ExeApi::FogGrid_ClearHandle() + FOG_CLEAR_HANDLE_LOOP;
	const DWORD dwLoop3 = ExeApi::FogGrid_ClearAllHighlightsLoop() - FOG_CLEAR_ALL_COUNT;

	if (
		!WriteCode(dwLoop1, FOG_HEX_CHANGED_ORIGINAL, sizeof(FOG_HEX_CHANGED_ORIGINAL))
		|| !WriteCode(dwLoop2, FOG_CLEAR_HANDLE_ORIGINAL, sizeof(FOG_CLEAR_HANDLE_ORIGINAL))
		|| !WriteCode(dwLoop3, FOG_CLEAR_ALL_ORIGINAL, sizeof(FOG_CLEAR_ALL_ORIGINAL))
	)
	{
		// Keep the thunk: the first loop may still jump there
		CUSTOMLOG("Exe: UndoFixFogGridWidth could not restore the fog grid loops");
		return;
	}

	if (s_pFogGridThunk != NULL)
	{
		VirtualFree(s_pFogGridThunk, 0, MEM_RELEASE);
	}
	s_pFogGridThunk = NULL;
	s_bFogGridFixed = false;

	CUSTOMLOG("Exe: fog grid loops restored (width * width)");
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

//------------------------------------------------------------------------------
Exe::Reason Exe::TryReadLocalMachineEventStreamStats(
	LocalMachineEventStreamStats& kStats
)
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	if (!HasSymbols(EVENT_STREAM_SYMBOLS, _countof(EVENT_STREAM_SYMBOLS)))
	{
		return REASON_UNSUPPORTED_EXE;
	}

	const DWORD dwChannel = *ExeApi::g_EventSystemLocalMachine();

	if (dwChannel >= NUM_EVENT_CHANNELS)
	{
		return REASON_UNAVAILABLE;
	}

	const DWORD dwOffset = dwChannel * ExeApi::LocalMachineContainer_Size();
	const DWORD dwContainer =
		ExeApi::g_EventSystemLocalMachine_Containers() + dwOffset;
	const DWORD dwBufferSize = ExeApi::LocalMachineContainer_BufferSize();

	kStats.dwSwapCounter = ReadDword(
		dwContainer + ExeApi::LocalMachineContainer_SwapCounter()
	);
	kStats.adwPublishedSize[0] = ReadDword(dwContainer);
	kStats.adwPublishedSize[1] = ReadDword(dwContainer + dwBufferSize);
	kStats.dwBufferCapacity =
		dwBufferSize - ExeApi::LocalMachineContainer_BufferHeaderSize();
	kStats.dwMaxPublishedSize = ReadDword(
		ExeApi::g_EventSystemLocalMachine_MaxPublishedSize() + dwOffset
	);

	return REASON_OK;
}

//------------------------------------------------------------------------------
Exe::Reason Exe::CanEnlargeCoastGridStack()
{
	if (!MOD_BIN_HOOKS)
	{
		return REASON_BIN_HOOKS_OFF;
	}

	if (!HasSymbols(COAST_GRID_STACK_SYMBOLS, _countof(COAST_GRID_STACK_SYMBOLS)))
	{
		return REASON_UNSUPPORTED_EXE;
	}

	if (s_pCoastGridThunk != NULL)
	{
		return REASON_ALREADY_ENABLED;
	}

	return REASON_OK;
}

Exe::Reason Exe::TryEnlargeCoastGridStack()
{
	const Reason eReason = CanEnlargeCoastGridStack();

	if (eReason == REASON_ALREADY_ENABLED)
	{
		return REASON_OK;
	}

	if (eReason != REASON_OK)
	{
		return Refuse("TryEnlargeCoastGridStack", eReason);
	}

	BYTE* pStack = static_cast<BYTE*>(
		VirtualAlloc(NULL, COAST_GRID_STACK_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)
	);
	BYTE* pThunk = static_cast<BYTE*>(
		VirtualAlloc(NULL, COAST_GRID_THUNK_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)
	);

	const DWORD dwCall = reinterpret_cast<DWORD>(ExeApi::TerrainSystem_Init_CoastGridInit()) + COAST_GRID_CALL_OFFSET;
	DWORD dwOldRel = 0;
	memcpy(&dwOldRel, reinterpret_cast<const void*>(dwCall + 1), sizeof(dwOldRel));
	bool bDone = false;

	if (pStack != NULL && pThunk != NULL)
	{
		// 64 bytes below the end; VirtualAlloc returns 64 KB aligned memory
		const DWORD dwBase = reinterpret_cast<DWORD>(pStack);
		const DWORD dwTop = dwBase + COAST_GRID_STACK_SIZE - 64;
		const unsigned int uiSize = WriteCoastGridThunk(pThunk, dwTop, dwBase, ExeApi::CoastGrid_Init());

		DWORD dwOldProtect = 0;
		if (VirtualProtect(pThunk, COAST_GRID_THUNK_SIZE, PAGE_EXECUTE_READ, &dwOldProtect))
		{
			FlushInstructionCache(GetCurrentProcess(), pThunk, uiSize);
			const DWORD dwNewRel = Rel32(dwCall + 5, reinterpret_cast<DWORD>(pThunk));
			bDone = WriteCode(dwCall + 1, reinterpret_cast<const BYTE*>(&dwNewRel), sizeof(dwNewRel));
		}
	}

	if (!bDone)
	{
		if (pThunk != NULL)
		{
			VirtualFree(pThunk, 0, MEM_RELEASE);
		}
		if (pStack != NULL)
		{
			VirtualFree(pStack, 0, MEM_RELEASE);
		}

		return Refuse("TryEnlargeCoastGridStack", REASON_UNAVAILABLE);
	}

	s_pCoastGridThunk = pThunk;
	s_pCoastGridStack = pStack;
	s_dwCoastGridCallRel = dwOldRel;

	CUSTOMLOG("Exe: CoastGrid_Init call at %08X runs on a %u MB stack at %08X (thunk %08X)",
		(unsigned int)dwCall,
		(unsigned int)(COAST_GRID_STACK_SIZE / (1024 * 1024)),
		(unsigned int)reinterpret_cast<DWORD>(pStack),
		(unsigned int)reinterpret_cast<DWORD>(pThunk)
	);

	return REASON_OK;
}

void Exe::UndoEnlargeCoastGridStack()
{
	if (s_pCoastGridThunk == NULL)
	{
		return;
	}

	// Put the call back before the thunk goes away
	const DWORD dwCall = reinterpret_cast<DWORD>(ExeApi::TerrainSystem_Init_CoastGridInit()) + COAST_GRID_CALL_OFFSET;
	if (!WriteCode(dwCall + 1, reinterpret_cast<const BYTE*>(&s_dwCoastGridCallRel), sizeof(s_dwCoastGridCallRel)))
	{
		// Keep the thunk and stack: the call still points there
		CUSTOMLOG("Exe: UndoEnlargeCoastGridStack could not restore the call at %08X",
			(unsigned int)dwCall
		);
		return;
	}

	VirtualFree(s_pCoastGridThunk, 0, MEM_RELEASE);
	VirtualFree(s_pCoastGridStack, 0, MEM_RELEASE);
	s_pCoastGridThunk = NULL;
	s_pCoastGridStack = NULL;
	s_dwCoastGridCallRel = 0;

	CUSTOMLOG("Exe: CoastGrid_Init call at %08X restored",
		(unsigned int)dwCall
	);
}
