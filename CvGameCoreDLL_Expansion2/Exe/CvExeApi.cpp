#include "CvGameCoreDLLPCH.h"
#include "CvExeApi.h"
#include "CvExeSymbols.h"
#include "LintFree.h"

namespace
{
// __thiscall as __fastcall with an unused EDX: same layout, and VC9 cannot
// declare __thiscall function pointers. Only for methods with no stack
// arguments that end in a plain ret: __fastcall would pass a second argument
// in EDX, not on the stack.
template<typename R>
R ThisCall(ExeSymbol e, void* pThis)
{
	typedef R (__fastcall* Fn)(void*, void*);

	return reinterpret_cast<Fn>(ExeSymbols::Get(e))(pThis, NULL);
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
