/*	--------------------------------------------------------------------------
	EXE addresses: everything the DLL uses in the game EXE, one column per
	build. X-macro list, no include guard: only CvExeSymbols.h/.cpp include
	it.

	ExeBuild picks the column by the EXE's PE timestamp. On first use,
	ExeSymbols moves each address to where the EXE is actually loaded,
	compares its signature, runs the checks, and writes the result to
	CustomMods.log. ExeApi must have one accessor per entry, with the same
	name.

	EXE_SYMBOL(name, kind, dx11, dx9, tablet, signature)
	  kind       EXE_FUNCTION, EXE_CODE, EXE_DATA, EXE_VTABLE, or EXE_OFFSET
	             (the columns then hold the offset)
	  dx11..     VA at the preferred base 0x400000
	  signature  bytes at the address ("83 EC ?? 56", ?? = any) or NULL.
	             Wildcard relocated addresses and register-to-register
	             instructions (mov esi,ecx is 8B F1 or 89 CE).

	EXE_CHECK(kind, from, offset, to)
	  EXE_ABS32  the dword at from+offset is the address of `to`
	  EXE_REL32  the call/jmp at from+offset targets `to`
	  EXE_VAL8   the byte at from+offset equals the offset `to`
	  EXE_VAL32  the dword at from+offset equals the offset `to`

	An entry is used only if its signature matches, every check naming it
	passes, and, without a signature, some verified entry vouches for it.

	DRM: Steam CEG checks parts of .text and .rdata and may crash the game
	if they change. Do not write to either unless it is proven that the DRM
	does not rely on that part.
	------------------------------------------------------------------------- */

#ifdef EXE_SYMBOL
// EXE_SYMBOL(name, kind, DX11, DX9, Tablet, signature)

EXE_SYMBOL(
	LoadCvGameCoreDLL_SetEngineUserInterface,
	EXE_CODE, 0x0047ba70, 0x0072b7f0, 0x004cb690,
	"8B 0D ?? ?? ?? ?? 8B 81 D0 00 00 00 ?? ?? 74 05 83"
	" C0 04 EB 02 ?? ?? 8B 16 50 8B 82 34 02 00 00 56 FF D0"
)

EXE_SYMBOL(
	InterfaceBuddy_UserInterface,
	EXE_OFFSET, 0x00000004, 0x00000004, 0x00000004,
	NULL
)

EXE_SYMBOL(
	InterfaceBuddy_dtor,
	EXE_FUNCTION, 0x006b74d0, 0x00472fc0, 0x0054d990,
	"83 EC 10 53 55 56 ?? ?? C7 06 ?? ?? ?? ?? C7 46 04"
)

EXE_SYMBOL(
	InterfaceBuddy_vftable,
	EXE_VTABLE, 0x00a3fe78, 0x00a35198, 0x00a42f00,
	NULL
)

EXE_SYMBOL(
	InterfaceBuddy_UserInterface_vftable,
	EXE_VTABLE, 0x00a04240, 0x00a00928, 0x009f3738,
	NULL
)

EXE_SYMBOL(
	InterfaceBuddy_dtor_UnregisterYieldIcons,
	EXE_CODE, 0x006b76cb, 0x004731bb, 0x0054db8b,
	"8D 4E 10 E8"
)

EXE_SYMBOL(
	InterfaceBuddy_YieldIconManager,
	EXE_OFFSET, 0x00000010, 0x00000010, 0x00000010,
	NULL
)

// Signature covers the whole function
EXE_SYMBOL(
	YieldIconManager_UnregisterForEvents,
	EXE_FUNCTION, 0x00711dc0, 0x0069dde0, 0x006bc7b0,
	"83 EC 0C 56 8D 44 24 04 ?? ?? 50"
	" C7 44 24 10 ?? ?? ?? ?? 89 74 24 0C E8 ?? ?? ?? ??"
	" 8D 4C 24 08 51 C7 44 24 14 ?? ?? ?? ?? 89 74 24 10 E8 ?? ?? ?? ??"
	" 8D 54 24 0C 52 C7 44 24 18 ?? ?? ?? ?? 89 74 24 14 E8 ?? ?? ?? ??"
	" 8D 44 24 10 50 C7 44 24 1C ?? ?? ?? ?? 89 74 24 18 E8 ?? ?? ?? ??"
	" 8D 4C 24 14 51 C7 44 24 20 ?? ?? ?? ?? 89 74 24 1C E8 ?? ?? ?? ??"
	" 8D 54 24 18 52 C7 44 24 24 ?? ?? ?? ?? 89 74 24 20 E8 ?? ?? ?? ??"
	" 83 C4 18 5E 83 C4 0C C3"
)

EXE_SYMBOL(
	NetMessage_WantForceResync,
	EXE_DATA, 0x02dd2f68, 0x02dc2d68, 0x02dd4f50,
	NULL
)

EXE_SYMBOL(
	ResetNetMessageStatics_ClearWantForceResync,
	EXE_CODE, 0x00525bd3, 0x004e1193, 0x00781b73,
	"A1 ?? ?? ?? ?? 89 2D ?? ?? ?? ?? 89 1D ?? ?? ?? ?? 88 1D"
)

// The LocalMachine event system. Its first dword is the current event
// channel; after the handler tables comes one LocalMachineContainer per
// channel. A container is a double buffer: each buffer starts with a header
// whose first dword is the buffer's published size, and records follow it.
// The low bit of the swap counter picks the buffer being written. No writer
// checks the buffer size.
EXE_SYMBOL(
	g_EventSystemLocalMachine,
	EXE_DATA, 0x01a0e400, 0x019fe200, 0x01a0fe80,
	NULL
)

EXE_SYMBOL(
	g_EventSystemLocalMachine_Containers,
	EXE_DATA, 0x01a0f080, 0x019fee80, 0x01a10b00,
	NULL
)

// The first container's max published size
EXE_SYMBOL(
	g_EventSystemLocalMachine_MaxPublishedSize,
	EXE_DATA, 0x0220f184, 0x021fef84, 0x02210c04,
	NULL
)

EXE_SYMBOL(
	LocalMachineContainer_Size,
	EXE_OFFSET, 0x00800180, 0x00800180, 0x00800180,
	NULL
)

EXE_SYMBOL(
	LocalMachineContainer_BufferSize,
	EXE_OFFSET, 0x00400080, 0x00400080, 0x00400080,
	NULL
)

EXE_SYMBOL(
	LocalMachineContainer_SwapCounter,
	EXE_OFFSET, 0x00800100, 0x00800100, 0x00800100,
	NULL
)

EXE_SYMBOL(
	LocalMachineContainer_BufferHeaderSize,
	EXE_OFFSET, 0x00000080, 0x00000080, 0x00000080,
	NULL
)

// EventTemplate<Event_Int2Type<171>, EndTurnTimerUpdateData, ...>: reserves
// a record in the current container. Called by
// InterfaceBuddy::updateEndTurnTimer.
EXE_SYMBOL(
	EndTurnTimerUpdate_Reserve,
	EXE_FUNCTION, 0x00870760, 0x00896310, 0x00872200,
	"A1 ?? ?? ?? ?? 69 C0 ?? ?? ?? ?? 56 05 ?? ?? ?? ?? 8B B0 ?? ?? ?? ??"
	" 83 E6 01 69 F6 ?? ?? ?? ?? ?? ?? 68 80 00 00 00 56 FF 15 ?? ?? ?? ??"
	" 8D 84 30 ?? ?? ?? ?? ?? ?? 5E ?? ?? 74 13 C7 00 80 00 00 00"
	" C7 40 04 AB 00 00 00"
)

// Reads the current container's max published size for the debug info
// ("LocalMachineEventStream : %d k Max Published Size")
EXE_SYMBOL(
	GameViewState_PrintDebugInfo_noAlloc_ReadMaxPublishedSize,
	EXE_CODE, 0x006a60e9, 0x007a2bb9, 0x00550409,
	"8B 0D ?? ?? ?? ?? 69 C9 ?? ?? ?? ?? 8B 91 ?? ?? ?? ?? C1 EA 0A 42 52"
)

#endif

#ifdef EXE_CHECK
// EXE_CHECK(kind, from, offset, to)

EXE_CHECK(
	EXE_VAL8,
	LoadCvGameCoreDLL_SetEngineUserInterface,
	0x12,
	InterfaceBuddy_UserInterface
)

EXE_CHECK(
	EXE_ABS32,
	InterfaceBuddy_dtor,
	0x0A,
	InterfaceBuddy_vftable
)

EXE_CHECK(
	EXE_ABS32,
	InterfaceBuddy_dtor,
	0x11,
	InterfaceBuddy_UserInterface_vftable
)

EXE_CHECK(
	EXE_VAL8,
	InterfaceBuddy_dtor_UnregisterYieldIcons,
	0x02,
	InterfaceBuddy_YieldIconManager
)

EXE_CHECK(
	EXE_REL32,
	InterfaceBuddy_dtor_UnregisterYieldIcons,
	0x03,
	YieldIconManager_UnregisterForEvents
)

EXE_CHECK(
	EXE_ABS32,
	ResetNetMessageStatics_ClearWantForceResync,
	0x13,
	NetMessage_WantForceResync
)

EXE_CHECK(
	EXE_ABS32,
	EndTurnTimerUpdate_Reserve,
	0x01,
	g_EventSystemLocalMachine
)

EXE_CHECK(
	EXE_VAL32,
	EndTurnTimerUpdate_Reserve,
	0x07,
	LocalMachineContainer_Size
)

EXE_CHECK(
	EXE_ABS32,
	EndTurnTimerUpdate_Reserve,
	0x0D,
	g_EventSystemLocalMachine_Containers
)

EXE_CHECK(
	EXE_VAL32,
	EndTurnTimerUpdate_Reserve,
	0x13,
	LocalMachineContainer_SwapCounter
)

EXE_CHECK(
	EXE_VAL32,
	EndTurnTimerUpdate_Reserve,
	0x1C,
	LocalMachineContainer_BufferSize
)

EXE_CHECK(
	EXE_VAL32,
	EndTurnTimerUpdate_Reserve,
	0x31,
	LocalMachineContainer_BufferHeaderSize
)

EXE_CHECK(
	EXE_ABS32,
	GameViewState_PrintDebugInfo_noAlloc_ReadMaxPublishedSize,
	0x02,
	g_EventSystemLocalMachine
)

EXE_CHECK(
	EXE_VAL32,
	GameViewState_PrintDebugInfo_noAlloc_ReadMaxPublishedSize,
	0x08,
	LocalMachineContainer_Size
)

EXE_CHECK(
	EXE_ABS32,
	GameViewState_PrintDebugInfo_noAlloc_ReadMaxPublishedSize,
	0x0E,
	g_EventSystemLocalMachine_MaxPublishedSize
)

#endif
