#include "CvGameCoreDLLPCH.h"
#include "CvExeApi.h"
#include "CvExeSymbols.h"
#include "LintFree.h"

namespace
{
// __thiscall as __fastcall with an unused EDX: same layout, and VC9 cannot
// declare __thiscall function pointers.
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
volatile BYTE* ExeApi::NetMessage_WantForceResync()
{
	return reinterpret_cast<volatile BYTE*>(
		ExeSymbols::Get(EXE_NetMessage_WantForceResync)
	);
}

//------------------------------------------------------------------------------
void** ExeApi::Singleton_Instance()
{
	return reinterpret_cast<void**>(ExeSymbols::Get(EXE_Singleton_Instance));
}

DWORD ExeApi::Singleton_TunerListener()
{
	return ExeSymbols::Get(EXE_Singleton_TunerListener);
}

//------------------------------------------------------------------------------
bool ExeApi::TunerListener_ExitingMultiplayerStagingRoom(void* pThis)
{
	const ExeSymbol eFunction = EXE_TunerListener_ExitingMultiplayerStagingRoom;

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

DWORD ExeApi::TunerListener_ListenSocket()
{
	return ExeSymbols::Get(EXE_TunerListener_ListenSocket);
}

const volatile BYTE* ExeApi::Tuner_Enabled()
{
	return reinterpret_cast<const volatile BYTE*>(
		ExeSymbols::Get(EXE_Tuner_Enabled)
	);
}
