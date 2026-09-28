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

// ---- cvCityBuildingSim: the engine's per-city building art. One record per
// city plot; the record keeps the art shown in the Palace position.

// GameViewState::UpdateSystems: mov ecx, offset g_cvCityBuildingSim, then
// call cvCityBuildingSim::Update
EXE_SYMBOL(
	GameViewState_UpdateSystems_UpdateCityBuildingSim,
	EXE_CODE, 0x006fbb66, 0x00710626, 0x00648596,
	"D9 44 24 08 51 B9 ?? ?? ?? ?? D9 1C 24 E8"
)

EXE_SYMBOL(
	g_cvCityBuildingSim,
	EXE_DATA, 0x02a11010, 0x02a00e10, 0x02a12a90,
	NULL
)

EXE_SYMBOL(
	cvCityBuildingSim_Update,
	EXE_CODE, 0x00537bf0, 0x004dccc0, 0x00608be0,
	"D9 44 24 04 83 EC 24 53 56 ?? ?? 51 8D 8E ?? ?? ?? ?? D9 1C 24 E8"
)

// Same bytes in all three builds except the call targets. Its Palace branch
// clears the message's art entry instead of the record's, so the record and
// the city's art keep the removed Palace.
EXE_SYMBOL(
	cvCityBuildingSim_OnWonderRemoved,
	EXE_CODE, 0x00462620, 0x0073ad10, 0x00453970,
	"83 EC 2C 55 ?? ?? F6 85 ?? ?? ?? ?? 01 89 6C 24 08 74 2A"
)

// Bit 0 set: the handlers queue their messages instead of changing records
EXE_SYMBOL(
	cvCityBuildingSim_DeferFlags,
	EXE_OFFSET, 0x001624c4, 0x001624c4, 0x001624c4,
	NULL
)

EXE_SYMBOL(
	cvCityBuildingSim_SlotGrid,
	EXE_OFFSET, 0x00083f38, 0x00083f38, 0x00083f38,
	NULL
)

// thiscall on the slot grid, (const int hex[3]) -> WORD* record index,
// 0x7FFF if none. Other functions share these start bytes; the call in
// OnWonderRemoved vouches for the address.
EXE_SYMBOL(
	cvCityBuildingSim_SlotGrid_GetSlot,
	EXE_FUNCTION, 0x0062b5a0, 0x00664b60, 0x006f7ed0,
	"83 EC 18 8B 44 24 1C 8B 50 04 56 ?? ?? 8B 08 8B 40 08 89 4C 24 04"
)

// Pointer to the record array
EXE_SYMBOL(
	cvCityBuildingSim_Cities,
	EXE_OFFSET, 0x0000023c, 0x0000023c, 0x0000023c,
	NULL
)

EXE_SYMBOL(
	cvCityBuildingSim_NumCities,
	EXE_OFFSET, 0x00000240, 0x00000240, 0x00000240,
	NULL
)

EXE_SYMBOL(
	cvCityBuildingSim_CityDataSize,
	EXE_OFFSET, 0x000006f4, 0x000006f4, 0x000006f4,
	NULL
)

// Byte, nonzero: the record is free
EXE_SYMBOL(
	CityData_Free,
	EXE_OFFSET, 0x00000010, 0x00000010, 0x00000010,
	NULL
)

// The art entry (building by culture and era, not a BuildingTypes) in the
// Palace position (PreferredDisplayPosition 32), or -1
EXE_SYMBOL(
	CityData_PalaceArtEntry,
	EXE_OFFSET, 0x0000058c, 0x0000058c, 0x0000058c,
	NULL
)

// The city's handle in cvCityVisSystem
EXE_SYMBOL(
	CityData_VisHandle,
	EXE_OFFSET, 0x000006f0, 0x000006f0, 0x000006f0,
	NULL
)

// thiscall on the building sim, (const int hex[3], int flags): queues the
// city for a re-layout
EXE_SYMBOL(
	cvCityBuildingSim_MarkDirty,
	EXE_FUNCTION, 0x00767cf0, 0x0056faa0, 0x0073d970,
	"8B 44 24 04 53 55 56 57 8B B9 ?? ?? ?? ?? ?? ?? ?? ?? 76 2A 8B 99"
)

// Writes the record's hex (short x, short y)
EXE_SYMBOL(
	cvCityBuildingSim_CreateNewCity,
	EXE_CODE, 0x00617b50, 0x00550ac0, 0x00780d70,
	"81 EC 44 07 00 00 53 56 8B B4 24 50 07 00 00 0F BF 56 20 ?? ?? 0F BF 4E"
)

EXE_SYMBOL(
	CityData_Hex,
	EXE_OFFSET, 0x0000002c, 0x0000002c, 0x0000002c,
	NULL
)

EXE_SYMBOL(
	g_cvCityVisSystem,
	EXE_DATA, 0x00cd0d98, 0x00cc0be0, 0x00cd2818,
	NULL
)

// cvCityVisSystem::UpdateFromHandle(handle, cvCityVis_RemoveWonderData):
// thiscall, (const void* handle, const int* artEntry). Removes that
// model from the city; does nothing if the city doesn't show it.
EXE_SYMBOL(
	cvCityVisSystem_RemoveWonder,
	EXE_FUNCTION, 0x00503460, 0x006773c0, 0x0077d5c0,
	"83 EC 08 53 56 ?? ?? 83 CE FF 89 73 10 89 73 14 E8"
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

EXE_CHECK(
	EXE_ABS32,
	GameViewState_UpdateSystems_UpdateCityBuildingSim,
	0x06,
	g_cvCityBuildingSim
)

EXE_CHECK(
	EXE_REL32,
	GameViewState_UpdateSystems_UpdateCityBuildingSim,
	0x0D,
	cvCityBuildingSim_Update
)

EXE_CHECK(
	EXE_VAL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x08,
	cvCityBuildingSim_DeferFlags
)

EXE_CHECK(
	EXE_VAL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x79,
	cvCityBuildingSim_SlotGrid
)

EXE_CHECK(
	EXE_REL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x7D,
	cvCityBuildingSim_SlotGrid_GetSlot
)

EXE_CHECK(
	EXE_VAL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x8A,
	cvCityBuildingSim_NumCities
)

EXE_CHECK(
	EXE_VAL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x98,
	cvCityBuildingSim_CityDataSize
)

EXE_CHECK(
	EXE_VAL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x9F,
	cvCityBuildingSim_Cities
)

EXE_CHECK(
	EXE_VAL8,
	cvCityBuildingSim_OnWonderRemoved,
	0xA6,
	CityData_Free
)

EXE_CHECK(
	EXE_VAL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x188,
	CityData_PalaceArtEntry
)

EXE_CHECK(
	EXE_REL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x19A,
	cvCityBuildingSim_MarkDirty
)

EXE_CHECK(
	EXE_VAL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x2B3,
	CityData_VisHandle
)

EXE_CHECK(
	EXE_ABS32,
	cvCityBuildingSim_OnWonderRemoved,
	0x2B9,
	g_cvCityVisSystem
)

EXE_CHECK(
	EXE_REL32,
	cvCityBuildingSim_OnWonderRemoved,
	0x2C1,
	cvCityVisSystem_RemoveWonder
)

EXE_CHECK(
	EXE_VAL8,
	cvCityBuildingSim_CreateNewCity,
	0x126,
	CityData_Hex
)

#endif
