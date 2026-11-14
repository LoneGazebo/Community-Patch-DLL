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
