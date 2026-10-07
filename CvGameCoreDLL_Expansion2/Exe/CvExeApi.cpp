#include "CvGameCoreDLLPCH.h"
#include "CvExeApi.h"
#include "CvExeSymbols.h"
#include "LintFree.h"

namespace
{
// __thiscall as __fastcall with an unused EDX: same layout, and VC9 cannot
// declare __thiscall function pointers. __fastcall passes the third and
// later arguments on the stack, right to left, and the callee pops them, as
// __thiscall does with its stack arguments; EDX is the dummy second.
template<typename R>
R ThisCall(ExeSymbol e, void* pThis)
{
	typedef R (__fastcall* Fn)(void*, void*);

	return reinterpret_cast<Fn>(ExeSymbols::Get(e))(pThis, NULL);
}

// As CCommandStream calls them
const int FIRE_HEAP = 9;
const int FIRE_TAG_COMMANDS = 0xc;
const int FIRE_TAG_DATA = 0xe;

typedef void* (__cdecl* FireMallocFn)(
	unsigned int,
	unsigned int,
	const char*,
	int,
	int,
	int
);
typedef void (__cdecl* FireFreeFn)(void*);

template<typename R, typename A1>
R ThisCall(ExeSymbol e, void* pThis, A1 a1)
{
	typedef R (__fastcall* Fn)(void*, void*, A1);

	return reinterpret_cast<Fn>(ExeSymbols::Get(e))(pThis, NULL, a1);
}

template<typename R, typename A1, typename A2>
R ThisCall(ExeSymbol e, void* pThis, A1 a1, A2 a2)
{
	typedef R (__fastcall* Fn)(void*, void*, A1, A2);

	return reinterpret_cast<Fn>(ExeSymbols::Get(e))(pThis, NULL, a1, a2);
}
} // namespace

//------------------------------------------------------------------------------
DWORD ExeApi::InterfaceBuddy_UserInterface()
{
	return ExeSymbols::Get(EXE_InterfaceBuddy_UserInterface);
}

DWORD ExeApi::InterfaceBuddy_YieldIconManager()
{
	return ExeSymbols::Get(EXE_InterfaceBuddy_YieldIconManager);
}

DWORD ExeApi::InterfaceBuddy_vftable()
{
	return ExeSymbols::Get(EXE_InterfaceBuddy_vftable);
}

DWORD ExeApi::InterfaceBuddy_UserInterface_vftable()
{
	return ExeSymbols::Get(EXE_InterfaceBuddy_UserInterface_vftable);
}

//------------------------------------------------------------------------------
bool ExeApi::YieldIconManager_UnregisterForEvents(void* pThis)
{
	if (!ExeSymbols::IsResolved(EXE_YieldIconManager_UnregisterForEvents))
	{
		return false;
	}

	ThisCall<void>(EXE_YieldIconManager_UnregisterForEvents, pThis);

	return true;
}

//------------------------------------------------------------------------------
volatile BYTE* ExeApi::Civ5NetMessage_s_wantForceResync()
{
	return reinterpret_cast<volatile BYTE*>(
		ExeSymbols::Get(EXE_Civ5NetMessage_s_wantForceResync)
	);
}

//------------------------------------------------------------------------------
const volatile DWORD* ExeApi::g_EventSystemLocalMachine()
{
	return reinterpret_cast<const volatile DWORD*>(
		ExeSymbols::Get(EXE_g_EventSystemLocalMachine)
	);
}

//------------------------------------------------------------------------------
BYTE* ExeApi::g_cvCityBuildingSim()
{
	return reinterpret_cast<BYTE*>(ExeSymbols::Get(EXE_g_cvCityBuildingSim));
}

DWORD ExeApi::cvCityBuildingSim_DeferFlags()
{
	return ExeSymbols::Get(EXE_cvCityBuildingSim_DeferFlags);
}

DWORD ExeApi::cvCityBuildingSim_SlotGrid()
{
	return ExeSymbols::Get(EXE_cvCityBuildingSim_SlotGrid);
}

DWORD ExeApi::cvCityBuildingSim_Cities()
{
	return ExeSymbols::Get(EXE_cvCityBuildingSim_Cities);
}

DWORD ExeApi::cvCityBuildingSim_NumCities()
{
	return ExeSymbols::Get(EXE_cvCityBuildingSim_NumCities);
}

DWORD ExeApi::cvCityBuildingSim_CityDataSize()
{
	return ExeSymbols::Get(EXE_cvCityBuildingSim_CityDataSize);
}

DWORD ExeApi::CityData_Free()
{
	return ExeSymbols::Get(EXE_CityData_Free);
}

DWORD ExeApi::CityData_Hex()
{
	return ExeSymbols::Get(EXE_CityData_Hex);
}

DWORD ExeApi::CityData_PalaceArtEntry()
{
	return ExeSymbols::Get(EXE_CityData_PalaceArtEntry);
}

DWORD ExeApi::CityData_VisHandle()
{
	return ExeSymbols::Get(EXE_CityData_VisHandle);
}

unsigned int ExeApi::cvCityBuildingSim_SlotGrid_GetSlot(void* pGrid,
	const int* aiHex)
{
	if (!ExeSymbols::IsResolved(EXE_cvCityBuildingSim_SlotGrid_GetSlot))
	{
		return 0x7FFF;
	}

	const WORD* pSlot = ThisCall<const WORD*>(
		EXE_cvCityBuildingSim_SlotGrid_GetSlot, pGrid, aiHex
	);

	return *pSlot;
}

void ExeApi::cvCityBuildingSim_MarkDirty(void* pSim, const int* aiHex,
	int iFlags)
{
	if (!ExeSymbols::IsResolved(EXE_cvCityBuildingSim_MarkDirty))
	{
		return;
	}

	ThisCall<void>(EXE_cvCityBuildingSim_MarkDirty, pSim, aiHex, iFlags);
}

//------------------------------------------------------------------------------
BYTE* ExeApi::g_cvCityVisSystem()
{
	return reinterpret_cast<BYTE*>(ExeSymbols::Get(EXE_g_cvCityVisSystem));
}

void ExeApi::cvCityVisSystem_RemoveWonder(void* pThis, const void* pHandle,
	const int* piArtEntry)
{
	if (!ExeSymbols::IsResolved(EXE_cvCityVisSystem_RemoveWonder))
	{
		return;
	}

	ThisCall<void>(EXE_cvCityVisSystem_RemoveWonder, pThis, pHandle,
		piArtEntry);
}

//------------------------------------------------------------------------------
void** ExeApi::cvLuaScriptSystem_ms_apSingletonInstance()
{
	return reinterpret_cast<void**>(
		ExeSymbols::Get(EXE_cvLuaScriptSystem_ms_apSingletonInstance)
	);
}

DWORD ExeApi::cvLuaScriptSystem_TunerListener()
{
	return ExeSymbols::Get(EXE_cvLuaScriptSystem_TunerListener);
}

//------------------------------------------------------------------------------
bool ExeApi::cvTunerListener_ExitingMultiplayerStagingRoom(void* pThis)
{
	const ExeSymbol eFunction =
		EXE_cvTunerListener_ExitingMultiplayerStagingRoom;

	if (!ExeSymbols::IsResolved(eFunction))
	{
		return false;
	}

	// The EXE throws an int when the port cannot be bound or listened on;
	// the listener is left without a listen socket then.
	try
	{
		ThisCall<void>(eFunction, pThis);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

DWORD ExeApi::cvTunerListener_ListenSocket()
{
	return ExeSymbols::Get(EXE_cvTunerListener_ListenSocket);
}

const volatile BYTE* ExeApi::g_bEnableTuner()
{
	return reinterpret_cast<const volatile BYTE*>(
		ExeSymbols::Get(EXE_g_bEnableTuner)
	);
}

//------------------------------------------------------------------------------
void* const* ExeApi::g_pUIManager()
{
	return reinterpret_cast<void* const*>(
		ExeSymbols::Get(EXE_g_pUIManager)
	);
}

DWORD ExeApi::UIManager_CommandStream()
{
	return ExeSymbols::Get(EXE_UIManager_CommandStream);
}

DWORD ExeApi::UIManager_CommandStreamLeft()
{
	return ExeSymbols::Get(EXE_UIManager_CommandStreamLeft);
}

DWORD ExeApi::UIManager_CommandStreamRight()
{
	return ExeSymbols::Get(EXE_UIManager_CommandStreamRight);
}

//------------------------------------------------------------------------------
DWORD ExeApi::CCommandStream_SetCount()
{
	return ExeSymbols::Get(EXE_CCommandStream_SetCount);
}

DWORD ExeApi::CCommandStream_CommandCap()
{
	return ExeSymbols::Get(EXE_CCommandStream_CommandCap);
}

DWORD ExeApi::CCommandStream_CommandCount()
{
	return ExeSymbols::Get(EXE_CCommandStream_CommandCount);
}

DWORD ExeApi::CCommandStream_Commands()
{
	return ExeSymbols::Get(EXE_CCommandStream_Commands);
}

DWORD ExeApi::CCommandStream_CommandsOther()
{
	return ExeSymbols::Get(EXE_CCommandStream_CommandsOther);
}

DWORD ExeApi::CCommandStream_Data()
{
	return ExeSymbols::Get(EXE_CCommandStream_Data);
}

DWORD ExeApi::CCommandStream_DataOther()
{
	return ExeSymbols::Get(EXE_CCommandStream_DataOther);
}

DWORD ExeApi::CCommandStream_DataSize()
{
	return ExeSymbols::Get(EXE_CCommandStream_DataSize);
}

DWORD ExeApi::CCommandStream_DataUsed()
{
	return ExeSymbols::Get(EXE_CCommandStream_DataUsed);
}

DWORD ExeApi::CCommandStream_DataUsedOther()
{
	return ExeSymbols::Get(EXE_CCommandStream_DataUsedOther);
}

//------------------------------------------------------------------------------
void* ExeApi::operator_new_array(unsigned int uiSize)
{
	if (!ExeSymbols::IsResolved(EXE_operator_new_array))
	{
		return NULL;
	}

	const FireMallocFn pfn = reinterpret_cast<FireMallocFn>(
		ExeSymbols::Get(EXE_operator_new_array)
	);

	return pfn(uiSize, 1, __FILE__, __LINE__, FIRE_HEAP, FIRE_TAG_COMMANDS);
}

bool ExeApi::operator_delete_array(void* p)
{
	if (!ExeSymbols::IsResolved(EXE_operator_delete_array))
	{
		return false;
	}

	reinterpret_cast<FireFreeFn>(ExeSymbols::Get(EXE_operator_delete_array))(
		p
	);

	return true;
}

void* ExeApi::FireMallocAlignedNoTracking(
	unsigned int uiSize,
	unsigned int uiAlign
)
{
	if (!ExeSymbols::IsResolved(EXE_FireMallocAlignedNoTracking))
	{
		return NULL;
	}

	const FireMallocFn pfn = reinterpret_cast<FireMallocFn>(
		ExeSymbols::Get(EXE_FireMallocAlignedNoTracking)
	);

	return pfn(uiSize, uiAlign, __FILE__, __LINE__, FIRE_HEAP, FIRE_TAG_DATA);
}

bool ExeApi::FireFreeAlignedNoTracking(void* p)
{
	if (!ExeSymbols::IsResolved(EXE_FireFreeAlignedNoTracking))
	{
		return false;
	}

	const FireFreeFn pfn = reinterpret_cast<FireFreeFn>(
		ExeSymbols::Get(EXE_FireFreeAlignedNoTracking)
	);
	pfn(p);

	return true;
}
