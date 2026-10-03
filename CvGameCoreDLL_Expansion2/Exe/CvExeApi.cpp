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

DWORD ExeApi::g_EventSystemLocalMachine_Containers()
{
	return ExeSymbols::Get(EXE_g_EventSystemLocalMachine_Containers);
}

DWORD ExeApi::g_EventSystemLocalMachine_MaxPublishedSize()
{
	return ExeSymbols::Get(EXE_g_EventSystemLocalMachine_MaxPublishedSize);
}

DWORD ExeApi::LocalMachineContainer_Size()
{
	return ExeSymbols::Get(EXE_LocalMachineContainer_Size);
}

DWORD ExeApi::LocalMachineContainer_BufferSize()
{
	return ExeSymbols::Get(EXE_LocalMachineContainer_BufferSize);
}

DWORD ExeApi::LocalMachineContainer_SwapCounter()
{
	return ExeSymbols::Get(EXE_LocalMachineContainer_SwapCounter);
}

DWORD ExeApi::LocalMachineContainer_BufferHeaderSize()
{
	return ExeSymbols::Get(EXE_LocalMachineContainer_BufferHeaderSize);
}
