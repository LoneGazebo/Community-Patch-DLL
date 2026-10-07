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

// ---- UI command stream. _DX11 / _DX9: the code differs between the two.

EXE_SYMBOL(
	UIManager_StartupRenderer_CreateCommandStreams,
	EXE_CODE, 0x005c991b, 0x005f1a2b, 0x0044cabb,
	"83 3D ?? ?? ?? ?? 00 8B 0D ?? ?? ?? ?? 6A 00 68 00 00 10 00"
	" 68 00 04 00 00 68 00 10 00 00 74 29 E8 ?? ?? ?? ?? 6A 00"
	" 68 00 00 10 00 68 00 04 00 00 89 86 ?? ?? 00 00 8B 0D ?? ?? ?? ??"
	" 68 00 10 00 00 E8 ?? ?? ?? ?? EB 0B E8 ?? ?? ?? ??"
	" 89 86 ?? ?? 00 00 8D 8C 24 38 03 00 00 68 ?? ?? ?? ?? 51"
	" 89 86 ?? ?? 00 00 89 86 ?? ?? 00 00"
)

EXE_SYMBOL(
	UIManager_StartupRenderer_ReadUIManager,
	EXE_CODE, 0x005c9a1f, 0x005f1b2f, 0x0044cbbf,
	"8B 15 ?? ?? ?? ?? 8B 8A ?? ?? 00 00 51 8B 0D ?? ?? ?? ?? 50 E8"
)

EXE_SYMBOL(
	g_pUIManager,
	EXE_DATA, 0x02dd1d1c, 0x02dc1b1c, 0x02dd37a4,
	NULL
)

// Left and Right differ only in stereo 3D
EXE_SYMBOL(
	UIManager_CommandStream,
	EXE_OFFSET, 0x000006f0, 0x000006f0, 0x00000700,
	NULL
)

EXE_SYMBOL(
	UIManager_CommandStreamLeft,
	EXE_OFFSET, 0x000006f4, 0x000006f4, 0x00000704,
	NULL
)

EXE_SYMBOL(
	UIManager_CommandStreamRight,
	EXE_OFFSET, 0x000006f8, 0x000006f8, 0x00000708,
	NULL
)

EXE_SYMBOL(
	CCommandStream_ctor_DX11,
	EXE_FUNCTION, 0x008ce150, EXE_NONE, 0x008d0220,
	"8B 44 24 04 53 56 57 8B 7C 24 18 ?? ?? 8B 4C 24 14 89 4E 10"
	" 89 46 08 ?? ?? 83 C7 0F ?? ?? 83 E7 F0 ?? ?? BA 20 00 00 00"
	" F7 E2 6A 0C 0F 90 C1 6A 09 6A 18 68 ?? ?? ?? ?? 6A 01"
	" 89 5E 40 89 7E 34 F7 D9 ?? ?? 51 E8 ?? ?? ?? ?? 89 46 14"
)

EXE_SYMBOL(
	CCommandStream_ctor_DX9,
	EXE_FUNCTION, EXE_NONE, 0x008ed490, EXE_NONE,
	"8B 44 24 04 53 56 57 8B 7C 24 18 ?? ?? 8B 4C 24 14 89 4E 10"
	" ?? ?? 83 C7 0F 83 E7 F0 ?? ?? 89 46 08 BA 20 00 00 00"
	" F7 E2 6A 0C 0F 90 C1 6A 09 6A 3D 68 ?? ?? ?? ?? 6A 01"
	" 89 5E 40 89 7E 34 F7 D9 ?? ?? 51 E8 ?? ?? ?? ?? 89 46 14"
)

// Frees min(current, other) of each pair
EXE_SYMBOL(
	CCommandStream_dtor_DX11,
	EXE_FUNCTION, 0x008ccd20, EXE_NONE, 0x008cede0,
	"56 ?? ?? 8B 86 70 20 00 00 8B 4E 14 ?? ?? 72 02 ?? ?? 50"
	" E8 ?? ?? ?? ?? 8B 06 8B 8E 6C 20 00 00 83 C4 04 ?? ?? 72 02"
	" ?? ?? 50 E8 ?? ?? ?? ?? 8B 86 68 20 00 00 8B 4E 24 83 C4 04"
	" ?? ?? 72 02 ?? ?? 50 E8 ?? ?? ?? ??"
)

EXE_SYMBOL(
	CCommandStream_dtor_DX9,
	EXE_FUNCTION, EXE_NONE, 0x008ec1e0, EXE_NONE,
	"56 ?? ?? 8B 06 50 E8 ?? ?? ?? ?? 8B 4E 14 51 E8 ?? ?? ?? ??"
	" 8B 56 24 52 E8 ?? ?? ?? ?? 83 C4 0C 5E C3"
)

// DX11 also clears +0x207c after these
EXE_SYMBOL(
	CCommandStream_ClearStream,
	EXE_FUNCTION, 0x008cd490, 0x008eca40, 0x008cf550,
	"?? ?? 89 41 0C 89 41 04 89 41 40 89 41 44 89 41 48 89 41 30"
	" 89 41 54"
)

EXE_SYMBOL(
	CCommandStream_SetCount,
	EXE_OFFSET, 0x00000004, 0x00000004, 0x00000004,
	NULL
)

EXE_SYMBOL(
	CCommandStream_CommandCap,
	EXE_OFFSET, 0x00000008, 0x00000008, 0x00000008,
	NULL
)

EXE_SYMBOL(
	CCommandStream_CommandCount,
	EXE_OFFSET, 0x0000000c, 0x0000000c, 0x0000000c,
	NULL
)

EXE_SYMBOL(
	CCommandStream_Commands,
	EXE_OFFSET, 0x00000014, 0x00000014, 0x00000014,
	NULL
)

EXE_SYMBOL(
	CCommandStream_CommandsOther,
	EXE_OFFSET, 0x00002070, EXE_NONE, 0x00002070,
	NULL
)

EXE_SYMBOL(
	CCommandStream_Data,
	EXE_OFFSET, 0x00000024, 0x00000024, 0x00000024,
	NULL
)

EXE_SYMBOL(
	CCommandStream_DataOther,
	EXE_OFFSET, 0x00002068, EXE_NONE, 0x00002068,
	NULL
)

EXE_SYMBOL(
	CCommandStream_DataSize,
	EXE_OFFSET, 0x00000034, 0x00000034, 0x00000034,
	NULL
)

EXE_SYMBOL(
	CCommandStream_DataUsed,
	EXE_OFFSET, 0x00000040, 0x00000040, 0x00000040,
	NULL
)

// The last frame's DataUsed, saved at the swap
EXE_SYMBOL(
	CCommandStream_DataUsedOther,
	EXE_OFFSET, 0x00002074, EXE_NONE, 0x00002074,
	NULL
)

// The four engine heap functions are cdecl and end in a plain ret. ExeApi
// passes the same arguments as CCommandStream; the caller pops them.
EXE_SYMBOL(
	operator_new_array,
	EXE_FUNCTION, 0x0082c090, 0x0081cae0, 0x00814ca0,
	"56 8B 74 24 08 57 85 F6 75 05 BE 01 00 00 00 6A 01 E8 ?? ?? ?? ??"
)

EXE_SYMBOL(
	FireMallocAlignedNoTracking,
	EXE_FUNCTION, 0x00813a80, 0x0081cb80, 0x00814a10,
	"8B 44 24 08 85 C0 74 0F 50 8B 44 24 08 50 E8 ?? ?? ?? ??"
	" 83 C4 08 C3 8B 4C 24 04 6A 00 51 FF 15 ?? ?? ?? ?? 83 C4 08 C3"
)

// No signature: the code differs between builds; the dtors vouch for these
EXE_SYMBOL(
	operator_delete_array,
	EXE_FUNCTION, 0x00813d10, 0x0081ce10, 0x00814cf0,
	NULL
)

EXE_SYMBOL(
	FireFreeAlignedNoTracking,
	EXE_FUNCTION, 0x00813cf0, 0x0081cdc0, 0x00814c80,
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
	UIManager_StartupRenderer_ReadUIManager,
	0x02,
	g_pUIManager
)

EXE_CHECK(
	EXE_VAL32,
	UIManager_StartupRenderer_CreateCommandStreams,
	0x50,
	UIManager_CommandStreamLeft
)

EXE_CHECK(
	EXE_VAL32,
	UIManager_StartupRenderer_CreateCommandStreams,
	0x63,
	UIManager_CommandStreamRight
)

EXE_CHECK(
	EXE_VAL32,
	UIManager_StartupRenderer_CreateCommandStreams,
	0x69,
	UIManager_CommandStream
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX11,
	0x16,
	CCommandStream_CommandCap
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX11,
	0x3F,
	CCommandStream_DataSize
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_ctor_DX11,
	0x45,
	operator_new_array
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX11,
	0x4C,
	CCommandStream_Commands
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_ctor_DX11,
	0x90,
	FireMallocAlignedNoTracking
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX11,
	0x9A,
	CCommandStream_Data
)

EXE_CHECK(
	EXE_VAL32,
	CCommandStream_ctor_DX11,
	0x9F,
	CCommandStream_DataOther
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX9,
	0x20,
	CCommandStream_CommandCap
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX9,
	0x3D,
	CCommandStream_DataSize
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_ctor_DX9,
	0x43,
	operator_new_array
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX9,
	0x4A,
	CCommandStream_Commands
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_ctor_DX9,
	0x89,
	FireMallocAlignedNoTracking
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ctor_DX9,
	0x93,
	CCommandStream_Data
)

EXE_CHECK(
	EXE_VAL32,
	CCommandStream_dtor_DX11,
	0x05,
	CCommandStream_CommandsOther
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_dtor_DX11,
	0x0B,
	CCommandStream_Commands
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_dtor_DX11,
	0x13,
	operator_delete_array
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_dtor_DX11,
	0x2A,
	operator_delete_array
)

EXE_CHECK(
	EXE_VAL32,
	CCommandStream_dtor_DX11,
	0x31,
	CCommandStream_DataOther
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_dtor_DX11,
	0x37,
	CCommandStream_Data
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_dtor_DX11,
	0x42,
	FireFreeAlignedNoTracking
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_dtor_DX9,
	0x06,
	operator_delete_array
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_dtor_DX9,
	0x0D,
	CCommandStream_Commands
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_dtor_DX9,
	0x0F,
	operator_delete_array
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_dtor_DX9,
	0x16,
	CCommandStream_Data
)

EXE_CHECK(
	EXE_REL32,
	CCommandStream_dtor_DX9,
	0x18,
	FireFreeAlignedNoTracking
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ClearStream,
	0x04,
	CCommandStream_CommandCount
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ClearStream,
	0x07,
	CCommandStream_SetCount
)

EXE_CHECK(
	EXE_VAL8,
	CCommandStream_ClearStream,
	0x0A,
	CCommandStream_DataUsed
)

EXE_CHECK(
	EXE_VAL32,
	CCommandStream_ctor_DX11,
	0xDF,
	CCommandStream_DataUsedOther
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
