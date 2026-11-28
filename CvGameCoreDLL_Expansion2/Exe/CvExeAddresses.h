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

	DRM: the DLL writes the EXE's .text in two places only, on the
	26577625 builds below (no CEG): the fog of war grid's whole-map loops
	(Exe::TryFixFogGridWidth) and one call that Exe::TryEnlargeCoastGridStack
	points at a thunk (TerrainSystem_Init_CoastGridInit). It puts both back
	when it unloads. Otherwise it writes only .data and heap objects.

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
	start of .text. Functions lost their inline CEG check calls; in
	cvCityBuildingSim_CreateNewCity the removed call (0x17 bytes) moves
	the CityData_Hex check from 0x126 to 0x10F (see EXE_CHECK). Live,
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

// The cvLuaScriptSystem singleton; it holds the cvTunerListener
EXE_SYMBOL(
	cvLuaScriptSystem_ms_apSingletonInstance,
	EXE_DATA, 0x02c62ec8, 0x02c52c88, 0x02c65ed0,
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
	EXE_CODE, 0x00764963, 0x00793333, 0x007661e3,
	"8B 0D ?? ?? ?? ?? 53 55 56 57 83 C1 04 E8"
)

// Signature covers the whole function. thiscall, no stack arguments, plain
// ret.
EXE_SYMBOL(
	cvTunerListener_ExitingMultiplayerStagingRoom,
	EXE_FUNCTION, 0x0077e5a0, 0x007ad8e0, 0x007800c0,
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
	EXE_DATA, 0x00b4933e, 0x00b3923e, 0x00b4c33e,
	NULL
)

// ---- UI command stream. _DX11 / _DX9: the code differs between the two.

EXE_SYMBOL(
	UIManager_StartupRenderer_CreateCommandStreams,
	EXE_CODE, 0x0068131b, 0x006b002b, 0x0068152b,
	"83 3D ?? ?? ?? ?? 00 8B 0D ?? ?? ?? ?? 6A 00 68 00 00 10 00"
	" 68 00 04 00 00 68 00 10 00 00 74 29 E8 ?? ?? ?? ?? 6A 00"
	" 68 00 00 10 00 68 00 04 00 00 89 86 ?? ?? 00 00 8B 0D ?? ?? ?? ??"
	" 68 00 10 00 00 E8 ?? ?? ?? ?? EB 0B E8 ?? ?? ?? ??"
	" 89 86 ?? ?? 00 00 8D 8C 24 38 03 00 00 68 ?? ?? ?? ?? 51"
	" 89 86 ?? ?? 00 00 89 86 ?? ?? 00 00"
)

EXE_SYMBOL(
	UIManager_StartupRenderer_ReadUIManager,
	EXE_CODE, 0x0068141f, 0x006b012f, 0x0068162f,
	"8B 15 ?? ?? ?? ?? 8B 8A ?? ?? 00 00 51 8B 0D ?? ?? ?? ?? 50 E8"
)

EXE_SYMBOL(
	g_pUIManager,
	EXE_DATA, 0x02c5941c, 0x02c491dc, 0x02c5c41c,
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
	EXE_FUNCTION, 0x008341d0, EXE_NONE, 0x00836250,
	"8B 44 24 04 53 56 57 8B 7C 24 18 ?? ?? 8B 4C 24 14 89 4E 10"
	" 89 46 08 ?? ?? 83 C7 0F ?? ?? 83 E7 F0 ?? ?? BA 20 00 00 00"
	" F7 E2 6A 0C 0F 90 C1 6A 09 6A 18 68 ?? ?? ?? ?? 6A 01"
	" 89 5E 40 89 7E 34 F7 D9 ?? ?? 51 E8 ?? ?? ?? ?? 89 46 14"
)

EXE_SYMBOL(
	CCommandStream_ctor_DX9,
	EXE_FUNCTION, EXE_NONE, 0x008530d0, EXE_NONE,
	"8B 44 24 04 53 56 57 8B 7C 24 18 ?? ?? 8B 4C 24 14 89 4E 10"
	" ?? ?? 83 C7 0F 83 E7 F0 ?? ?? 89 46 08 BA 20 00 00 00"
	" F7 E2 6A 0C 0F 90 C1 6A 09 6A 3D 68 ?? ?? ?? ?? 6A 01"
	" 89 5E 40 89 7E 34 F7 D9 ?? ?? 51 E8 ?? ?? ?? ?? 89 46 14"
)

// Frees min(current, other) of each pair
EXE_SYMBOL(
	CCommandStream_dtor_DX11,
	EXE_FUNCTION, 0x00832da0, EXE_NONE, 0x00834e10,
	"56 ?? ?? 8B 86 70 20 00 00 8B 4E 14 ?? ?? 72 02 ?? ?? 50"
	" E8 ?? ?? ?? ?? 8B 06 8B 8E 6C 20 00 00 83 C4 04 ?? ?? 72 02"
	" ?? ?? 50 E8 ?? ?? ?? ?? 8B 86 68 20 00 00 8B 4E 24 83 C4 04"
	" ?? ?? 72 02 ?? ?? 50 E8 ?? ?? ?? ??"
)

EXE_SYMBOL(
	CCommandStream_dtor_DX9,
	EXE_FUNCTION, EXE_NONE, 0x00851e20, EXE_NONE,
	"56 ?? ?? 8B 06 50 E8 ?? ?? ?? ?? 8B 4E 14 51 E8 ?? ?? ?? ??"
	" 8B 56 24 52 E8 ?? ?? ?? ?? 83 C4 0C 5E C3"
)

// DX11 also clears +0x207c after these
EXE_SYMBOL(
	CCommandStream_ClearStream,
	EXE_FUNCTION, 0x00833510, 0x00852680, 0x00835580,
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
	EXE_FUNCTION, 0x0045e0a0, 0x00473820, 0x004794d0,
	"56 8B 74 24 08 57 85 F6 75 05 BE 01 00 00 00 6A 01 E8 ?? ?? ?? ??"
)

EXE_SYMBOL(
	FireMallocAlignedNoTracking,
	EXE_FUNCTION, 0x0045ddf0, 0x00473590, 0x0045e060,
	"8B 44 24 08 85 C0 74 0F 50 8B 44 24 08 50 E8 ?? ?? ?? ??"
	" 83 C4 08 C3 8B 4C 24 04 6A 00 51 FF 15 ?? ?? ?? ?? 83 C4 08 C3"
)

// No signature: the code differs between builds; the dtors vouch for these
EXE_SYMBOL(
	operator_delete_array,
	EXE_FUNCTION, 0x0045e060, 0x00473870, 0x0045e2f0,
	NULL
)

EXE_SYMBOL(
	FireFreeAlignedNoTracking,
	EXE_FUNCTION, 0x0045e080, 0x00473050, 0x0045e2d0,
	NULL
)

// After g_EventSystemLocalMachine's handler tables comes one
// LocalMachineContainer per channel. A container is a double buffer: each
// buffer starts with a header whose first dword is the buffer's published
// size, and records follow it. The low bit of the swap counter picks the
// buffer being written. No writer checks the buffer size.
EXE_SYMBOL(
	g_EventSystemLocalMachine_Containers,
	EXE_DATA, 0x01894600, 0x01884480, 0x01897600,
	NULL
)

// The first container's max published size
EXE_SYMBOL(
	g_EventSystemLocalMachine_MaxPublishedSize,
	EXE_DATA, 0x02094704, 0x02084584, 0x02097704,
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

// No symbol: EventTemplate<Event_Int2Type<171>, EndTurnTimerUpdateData, ...>
// reserving a record in the current container. Its only caller is
// InterfaceBuddy::updateEndTurnTimer, which inlines it on Linux.
EXE_SYMBOL(
	updateEndTurnTimer_Reserve,
	EXE_FUNCTION, 0x00668c10, 0x00698970, 0x00669270,
	"A1 ?? ?? ?? ?? 69 C0 ?? ?? ?? ?? 56 05 ?? ?? ?? ?? 8B B0 ?? ?? ?? ??"
	" 83 E6 01 69 F6 ?? ?? ?? ?? ?? ?? 68 80 00 00 00 56 FF 15 ?? ?? ?? ??"
	" 8D 84 30 ?? ?? ?? ?? ?? ?? 5E ?? ?? 74 13 C7 00 80 00 00 00"
	" C7 40 04 AB 00 00 00"
)

// Reads the current container's max published size for the debug info
// ("LocalMachineEventStream : %d k Max Published Size")
EXE_SYMBOL(
	GameViewState_PrintDebugInfo_noAlloc_ReadMaxPublishedSize,
	EXE_CODE, 0x004c3039, 0x004f3f79, 0x004c2ca9,
	"8B 0D ?? ?? ?? ?? 69 C9 ?? ?? ?? ?? 8B 91 ?? ?? ?? ?? C1 EA 0A 42 52"
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

// ---- Fog of war grid (cvStrategicViewTracker): the engine's per-hex
// fog and highlight state, a fixed array of 0x8000 cells of 0x188 bytes at
// +0x4C, with the map width at +0x44 and the height at +0x48. Its three
// whole-map loops count width * width cells, not width * height: past 181
// columns they write beyond the array (the game crashes while the map
// loads), and on maps taller than wide they skip the rows past the width.
// Exe::TryFixFogGridWidth makes all three count width * height.

// cvStrategicViewTracker::OnHexFOWStateChanged: thiscall, one argument (x, y, state byte at
// +8, whole-map flag at +9). The loop count starts at +0x11:
// mov ecx,[esi+44h]; imul ecx,ecx; test ecx,ecx; jbe +0x9E
EXE_SYMBOL(
	FogGrid_OnHexFOWStateChanged,
	EXE_CODE, 0x005cc730, 0x005fbd60, 0x005ccca0,
	"83 EC 18 56 57 8B 7C 24 24 80 7F 09 00 ?? ?? 74 29"
)

// cvStrategicViewTracker::OnClearHexHighlightStyle: thiscall, one argument:
// clears every cell's field +0x1C that holds the style it looks up. The loop count starts at +0x23:
// mov ecx,[esi+44h]; imul ecx,ecx; test ecx,ecx; jbe +0x18
EXE_SYMBOL(
	FogGrid_ClearHandle,
	EXE_CODE, 0x005d2f60, 0x006025a0, 0x005d3500,
	"8B 44 24 04 56 ?? ?? 8B 0D ?? ?? ?? ?? 8B 11 50 8B 42 54 FF D0"
	" ?? ?? E8 ?? ?? ?? ?? ?? ?? E8"
)

// cvStrategicViewTracker::OnClearHexHighlights: thiscall, one argument;
// clears field +0x1C of every cell. The symbol is its loop body, at +0x10,
// which the fix keeps; the 16 bytes before it count the cells:
// mov edx,[ecx+44h]; imul edx,edx; test edx,edx; jbe +0x16;
// lea eax,[ecx+68h]; lea ecx,[ecx+0] (padding)
EXE_SYMBOL(
	FogGrid_ClearAllHighlightsLoop,
	EXE_CODE, 0x005cc4e0, 0x005fbb40, 0x005cca40,
	"C7 00 00 00 00 00 05 88 01 00 00 83 EA 01 75 F0 C2 04 00"
)

// ---- TerrainSystem::Init calls CoastGrid::Init (thiscall, one stack
// argument: the hex grid; retn 4), which takes its work buffers from the
// stack with _alloca: 48 bytes per plot of the map plus a 2-plot border,
// on the main thread's 1 MB stack. Exe::TryEnlargeCoastGridStack points
// this call at a thunk that runs it on a stack of its own. The signature
// starts 6 bytes before the call: mov eax,[ebp+8]; push eax; mov ecx,esi;
// call CoastGrid_Init.
EXE_SYMBOL(
	TerrainSystem_Init_CoastGridInit,
	EXE_CODE, 0x00523391, 0x00553981, 0x00522fc1,
	"8B 45 08 50 ?? ?? E8"
)

EXE_SYMBOL(
	CoastGrid_Init,
	EXE_FUNCTION, 0x00526bb0, 0x005571a0, 0x00526790,
	"53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC"
)

// ---- FluidFOWSystem (High fog quality; DX9 turns it down to Medium, which
// has no clouds) keeps its clouds in a store per worker thread, but seeds the
// whole map into the first one, one cloud per cell of the map, without
// checking that the store is full. Exe::TryLimitFogClouds points the cloud
// function's first instruction (mov eax,[ecx+440h]: the cloud count) at a
// thunk that keeps the seeds within the store.
// FluidFOW_AddCloud: thiscall, five stack arguments (retn 14h); writes one
// 128-byte cloud at [ecx+40h] + [ecx+440h] * 128 and increments [ecx+440h].
EXE_SYMBOL(
	FluidFOW_AddCloud,
	EXE_FUNCTION, 0x005aeef0, 0x005de770, 0x005af050,
	"8B 81 40 04 00 00 8B 54 24 04 F3 0F 10 44 24 0C C1 E0 07 03 41 40 56"
)

// Where each store is allocated: push <clouds per store>; push 80h (128 bytes
// each); push edi; call. The DLL reads the store size from the first push.
EXE_SYMBOL(
	FluidFOW_CloudStoreAlloc,
	EXE_CODE, 0x005b3038, 0x005e288a, 0x005b3198,
	"68 ?? ?? ?? ?? 68 80 00 00 00 57 E8"
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
	updateEndTurnTimer_Reserve,
	0x01,
	g_EventSystemLocalMachine
)

EXE_CHECK(
	EXE_VAL32,
	updateEndTurnTimer_Reserve,
	0x07,
	LocalMachineContainer_Size
)

EXE_CHECK(
	EXE_ABS32,
	updateEndTurnTimer_Reserve,
	0x0D,
	g_EventSystemLocalMachine_Containers
)

EXE_CHECK(
	EXE_VAL32,
	updateEndTurnTimer_Reserve,
	0x13,
	LocalMachineContainer_SwapCounter
)

EXE_CHECK(
	EXE_VAL32,
	updateEndTurnTimer_Reserve,
	0x1C,
	LocalMachineContainer_BufferSize
)

EXE_CHECK(
	EXE_VAL32,
	updateEndTurnTimer_Reserve,
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

// 0x126 in the 2014 EXEs: the update removed a 0x17-byte CEG check call
// before this instruction
EXE_CHECK(
	EXE_VAL8,
	cvCityBuildingSim_CreateNewCity,
	0x10F,
	CityData_Hex
)

EXE_CHECK(
	EXE_REL32,
	TerrainSystem_Init_CoastGridInit,
	0x6,
	CoastGrid_Init
)

#endif
