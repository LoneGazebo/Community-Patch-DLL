/*	-------------------------------------------------------------------------------------------------------
	EngineQueueGuard - keep the EXE's game-to-UI message queue from overrunning its buffers.

	THE BUG

	The game EXE passes gameplay events to the UI thread through a double-buffered queue in its
	.data section: two buffers of 4 MB each, a flip counter and a high-water mark, all at fixed addresses.
	A writer reserves room for a record with InterlockedExchangeAdd(&buffer.size, recordSize) and writes
	the record at buffer + 0x80 + old size - with no capacity check. The main thread swaps the buffers and
	dispatches the full one about a hundred times a second, so in normal play the buffer never gets past a
	few percent. While a save is being loaded, though, the main thread stops swapping for minutes while
	records keep arriving. On a large late game (a 180x113 map with ~2,700 units, 2026-09-21) that is
	9-20 MB of records, and whatever arrives during the longest pause - 4 to 7.5 MB measured - lands in
	one 4 MB buffer. The overrun runs into the other buffer and onto the flip counter, and the dispatcher,
	which walks records until it reaches the recorded end, then calls a garbage or null handler (access
	violation at CivilizationV_DX11+0x2a59a5), or meets a zero-length record and loops forever. Loads of
	the same save crash, hang or survive depending on how long the main thread happened to pause. The
	DX9 and Tablet EXEs are built from the same source and have the same queue at other addresses.

	THE GUARD

	Every queue writer reserves through the same kernel32 import slot of the EXE (97 reserve functions
	inlined into ~235 call sites, all of the form "reserve, then record = buffer + 0x80 + old").
	Install() points that slot at a thunk. For the
	four buffer size fields the thunk reserves with compare-and-swap and refuses a record that would not
	fit: it returns an offset that places the record in a private scratch area instead, and leaves the
	buffer size alone, so the dispatcher never sees it. Every other caller of the import - the EXE's own
	reference counts - is forwarded untouched.

	The cost of a dropped record is a stale visual (a unit or tile shows its previous state until its
	next update); the alternative was memory corruption. Nothing changes unless a buffer is actually full.

	The layout is verified before anything is patched: the EXE's timestamp selects one of the three
	known builds (DX11, DX9, Tablet), then the bytes of one writer, one reserve function, the swap and
	the dispatcher are compared at that build's addresses, with their relocated operands cross-checked
	against the build's queue and import-slot addresses. Any other EXE (another version, a patched
	one) is left alone. Like the DLL's other binary hooks it is opt-in through the BIN_HOOKS custom
	mod option, and VP_QUEUEGUARD=0 in the environment disables it regardless.
	------------------------------------------------------------------------------------------------------- */

#pragma once

#ifndef CIV5_ENGINE_QUEUE_GUARD_H
#define CIV5_ENGINE_QUEUE_GUARD_H

namespace EngineQueueGuard
{
//! Verifies the EXE and patches its import slot when bEnabled (the BIN_HOOKS custom mod option, known
//! once the database is cached). Safe to call more than once: only the first enabled call does the
//! work, and a disabled call just records why the guard is off. Not from DllMain (reads other
//! modules), and before any save can be loaded, which is when the queue overruns.
void Install(bool bEnabled);

struct Stats
{
	Stats();

	bool bInstalled;
	const char* szStatus;        //!< Why it is or is not active, for logs and crash reports.
	long lDroppedRecords;        //!< Records diverted to scratch because their buffer was full.
	long lDroppedBytes;
	long lLargestDropped;        //!< Largest single record diverted, in bytes.
	long lTooLargeForScratch;    //!< Records bigger than the scratch area, passed through unguarded.
};

void GetStats(Stats& kOut);

//! Appends one line to crashlogs\queueguard.log when records were dropped since the last call and none
//! for the last two seconds - i.e. once per load. Cheap when nothing happened. Game-core thread.
void ReportDrops();

//! One line for crashes.log: status and counters.
void FormatCrashLine(char* szOut, size_t uiSize);
}

#endif // CIV5_ENGINE_QUEUE_GUARD_H
