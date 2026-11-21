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
	heap objects.

	The 2014 EXEs (PE timestamps 546CD0A8 DX11, 546CCB59 DX9, 546CD5F8
	Tablet) carry Steam CEG, which hashes about 80% of .text and two
	thirds of .rdata in the loaded image, with different coverage per
	build. Tested live on the 2014 DX11 EXE: one unreachable int3 padding
	byte in .text, one alignment byte in .rdata and a 5-byte inline hook
	ended the game with exit code 0x8000DEAD within 80 seconds, during the
	save load. A debug register on any thread ends it the same way.

	The EXEs of the November 2026 update (Steam build 26577625; PE
	timestamps 6AF3837D DX11, 6AF3818F DX9, 6AF385BA Tablet) have no CEG
	and nothing in its place. Static, all three: the Valve block in the
	DOS stub, the STEAM_DRM_IPC names and the 0x8000DEAD exit are gone; in
	DX11 also the CEG data in .rdata and the hash loop's reference to the
	start of .text. Functions lost their inline CEG check calls (one in
	cvCityBuildingSim_CreateNewCity, which moved its check below). Live,
	DX11 only: the same three writes, an inline hook on a hot .text
	function and hardware and software breakpoints ran through 50 AI
	turns, a save, a load and 13 more turns with no crash, and the game
	state matched an unpatched control run. So writing .text or .rdata is
	proven safe only on 6AF3837D; DX9 and Tablet were not run.
	------------------------------------------------------------------------- */

#ifdef EXE_SYMBOL
// EXE_SYMBOL(name, kind, DX11, DX9, Tablet, signature)

EXE_SYMBOL(
	LoadCvGameCoreDLL_SetEngineUserInterface,
	EXE_CODE, 0x007b35f0, 0x007e2460, 0x007b52d0,
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
	EXE_FUNCTION, 0x00672f10, 0x006a2aa0, 0x006739d0,
	"83 EC 10 53 55 56 ?? ?? C7 06 ?? ?? ?? ?? C7 46 04"
)

EXE_SYMBOL(
	InterfaceBuddy_vftable,
	EXE_VTABLE, 0x0096f490, 0x00961ce8, 0x00971568,
	NULL
)

EXE_SYMBOL(
	InterfaceBuddy_UserInterface_vftable,
	EXE_VTABLE, 0x0096f230, 0x00961a88, 0x00971308,
	NULL
)

EXE_SYMBOL(
	InterfaceBuddy_dtor_UnregisterYieldIcons,
	EXE_CODE, 0x0067310b, 0x006a2c9b, 0x00673bcb,
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
	EXE_FUNCTION, 0x0068b240, 0x006b9e00, 0x0068c790,
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
	EXE_FUNCTION, 0x00689fd0, 0x006b8b90, 0x0068b520,
	"A1 ?? ?? ?? ?? 83 EC 34 69 C0 00 06 00 00 F6 05"
)

// The LocalMachine event system. Its first dword is the current event
// channel.
EXE_SYMBOL(
	g_EventSystemLocalMachine,
	EXE_DATA, 0x01893980, 0x01883800, 0x01896980,
	NULL
)

EXE_SYMBOL(
	Civ5NetMessage_s_wantForceResync,
	EXE_DATA, 0x02c63dd8, 0x02c53b78, 0x02c66de0,
	NULL
)

EXE_SYMBOL(
	ResetNetMessageStatics_ClearWantForceResync,
	EXE_CODE, 0x007c0543, 0x007efa13, 0x007c2053,
	"A1 ?? ?? ?? ?? 89 2D ?? ?? ?? ?? 89 1D ?? ?? ?? ?? 88 1D"
)

// ---- cvCityBuildingSim: the engine's per-city building art. One record per
// city plot; the record keeps the art shown in the Palace position.

// GameViewState::UpdateSystems: mov ecx, offset g_cvCityBuildingSim, then
// call cvCityBuildingSim::Update
EXE_SYMBOL(
	GameViewState_UpdateSystems_UpdateCityBuildingSim,
	EXE_CODE, 0x004c71c6, 0x004f8146, 0x004c70b6,
	"D9 44 24 08 51 B9 ?? ?? ?? ?? D9 1C 24 E8"
)

EXE_SYMBOL(
	g_cvCityBuildingSim,
	EXE_DATA, 0x02896590, 0x02886410, 0x02899590,
	NULL
)

EXE_SYMBOL(
	cvCityBuildingSim_Update,
	EXE_CODE, 0x00601540, 0x006308b0, 0x006017f0,
	"D9 44 24 04 83 EC 24 53 56 ?? ?? 51 8D 8E ?? ?? ?? ?? D9 1C 24 E8"
)

// Same bytes in all three builds except the call targets. Its Palace branch
// clears the message's art entry instead of the record's, so the record and
// the city's art keep the removed Palace.
EXE_SYMBOL(
	cvCityBuildingSim_OnWonderRemoved,
	EXE_CODE, 0x00603010, 0x006323f0, 0x006032c0,
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
	EXE_FUNCTION, 0x005fc750, 0x0062bb10, 0x005fca40,
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
	EXE_FUNCTION, 0x005fe070, 0x0062d3e0, 0x005fe320,
	"8B 44 24 04 53 55 56 57 8B B9 ?? ?? ?? ?? ?? ?? ?? ?? 76 2A 8B 99"
)

// Writes the record's hex (short x, short y)
EXE_SYMBOL(
	cvCityBuildingSim_CreateNewCity,
	EXE_CODE, 0x00600790, 0x0062fb00, 0x00600a40,
	"81 EC 44 07 00 00 53 56 8B B4 24 50 07 00 00 0F BF 56 20 ?? ?? 0F BF 4E"
)

EXE_SYMBOL(
	CityData_Hex,
	EXE_OFFSET, 0x0000002c, 0x0000002c, 0x0000002c,
	NULL
)

EXE_SYMBOL(
	g_cvCityVisSystem,
	EXE_DATA, 0x00b562f8, 0x00b461a8, 0x00b592f8,
	NULL
)

// cvCityVisSystem::UpdateFromHandle(handle, cvCityVis_RemoveWonderData):
// thiscall, (const void* handle, const int* artEntry). Removes that
// model from the city; does nothing if the city doesn't show it.
EXE_SYMBOL(
	cvCityVisSystem_RemoveWonder,
	EXE_FUNCTION, 0x004e0910, 0x00511730, 0x004e0750,
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

// 0x126 in the 2014 EXEs: the update removed a 0x17-byte CEG check call
// before this instruction
EXE_CHECK(
	EXE_VAL8,
	cvCityBuildingSim_CreateNewCity,
	0x10F,
	CityData_Hex
)

#endif
