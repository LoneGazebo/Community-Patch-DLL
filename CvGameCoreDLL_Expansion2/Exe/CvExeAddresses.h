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
	  dx11..     VA at the preferred base 0x400000, or EXE_NONE where the
	             build doesn't have it. Map every build that has it.
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

	DRM: the DLL never writes the EXE's .text or .rdata, only .data and
	heap objects. Steam CEG hashes about 80% of .text and two thirds of
	.rdata in the loaded image, with different coverage per build. A
	mismatch crashes the game later and far from the changed bytes. Do not
	write to either unless it is proven that CEG does not hash those bytes
	in any build.
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

// Signature covers the whole function. thiscall, no stack arguments, plain
// ret.
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

// No symbol: the first call in UnregisterForEvents. It removes one handler
// from the table of g_EventSystemLocalMachine's current channel; it starts by
// reading that channel.
EXE_SYMBOL(
	YieldIconManager_UnregisterForEvents_unk17,
	EXE_FUNCTION, 0x00874eb0, 0x0089aa50, 0x00876940,
	"A1 ?? ?? ?? ?? 83 EC 34 69 C0 00 06 00 00 F6 05"
)

// The LocalMachine event system. Its first dword is the current event
// channel.
EXE_SYMBOL(
	g_EventSystemLocalMachine,
	EXE_DATA, 0x01a0e400, 0x019fe200, 0x01a0fe80,
	NULL
)

EXE_SYMBOL(
	Civ5NetMessage_s_wantForceResync,
	EXE_DATA, 0x02dd2f68, 0x02dc2d68, 0x02dd4f50,
	NULL
)

EXE_SYMBOL(
	ResetNetMessageStatics_ClearWantForceResync,
	EXE_CODE, 0x00525bd3, 0x004e1193, 0x00781b73,
	"A1 ?? ?? ?? ?? 89 2D ?? ?? ?? ?? 89 1D ?? ?? ?? ?? 88 1D"
)

// The cvLuaScriptSystem singleton; it holds the cvTunerListener
EXE_SYMBOL(
	cvLuaScriptSystem_ms_apSingletonInstance,
	EXE_DATA, 0x02dd207c, 0x02dc1e7c, 0x02dd4064,
	NULL
)

EXE_SYMBOL(
	cvLuaScriptSystem_TunerListener,
	EXE_OFFSET, 0x00000004, 0x00000004, 0x00000004,
	NULL
)

// In cvLuaMatchmakingLibrary::lHostServerGame: the call that drops the
// tuner's listen socket on entering a multiplayer game
EXE_SYMBOL(
	lHostServerGame_EnteringMultiplayerStagingRoom,
	EXE_CODE, 0x007933f3, 0x004b2d63, 0x0051f4a3,
	"8B 0D ?? ?? ?? ?? 53 55 56 57 83 C1 04 E8"
)

// Signature covers the whole function. thiscall, no stack arguments, plain
// ret.
EXE_SYMBOL(
	cvTunerListener_ExitingMultiplayerStagingRoom,
	EXE_FUNCTION, 0x00700650, 0x006622b0, 0x00622240,
	"83 EC 10 80 3D ?? ?? ?? ?? 00 56 ?? ?? 74 2C 83 7E 20 00 75 26"
	" ?? ?? 50 8D 4C 24 08 E8 ?? ?? ?? ?? 68 DE 10 00 00 8D 44 24 08 50"
	" ?? ?? E8 ?? ?? ?? ?? 8D 4C 24 04 E8 ?? ?? ?? ?? 5E 83 C4 10 C3"
)

EXE_SYMBOL(
	cvTunerListener_ListenSocket,
	EXE_OFFSET, 0x00000020, 0x00000020, 0x00000020,
	NULL
)

// config.ini EnableTuner
EXE_SYMBOL(
	g_bEnableTuner,
	EXE_DATA, 0x00cc8e7a, 0x00cb8cfa, 0x00cca8f6,
	NULL
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
	EXE_REL32,
	YieldIconManager_UnregisterForEvents,
	0x17,
	YieldIconManager_UnregisterForEvents_unk17
)

EXE_CHECK(
	EXE_ABS32,
	YieldIconManager_UnregisterForEvents_unk17,
	0x01,
	g_EventSystemLocalMachine
)

EXE_CHECK(
	EXE_ABS32,
	ResetNetMessageStatics_ClearWantForceResync,
	0x13,
	Civ5NetMessage_s_wantForceResync
)

EXE_CHECK(
	EXE_ABS32,
	lHostServerGame_EnteringMultiplayerStagingRoom,
	0x02,
	cvLuaScriptSystem_ms_apSingletonInstance
)

EXE_CHECK(
	EXE_VAL8,
	lHostServerGame_EnteringMultiplayerStagingRoom,
	0x0C,
	cvLuaScriptSystem_TunerListener
)

EXE_CHECK(
	EXE_ABS32,
	cvTunerListener_ExitingMultiplayerStagingRoom,
	0x05,
	g_bEnableTuner
)

EXE_CHECK(
	EXE_VAL8,
	cvTunerListener_ExitingMultiplayerStagingRoom,
	0x11,
	cvTunerListener_ListenSocket
)

#endif
