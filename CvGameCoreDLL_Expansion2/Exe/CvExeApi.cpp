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
