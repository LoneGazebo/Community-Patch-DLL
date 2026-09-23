#include "CvGameCoreDLLPCH.h"
#include "EngineQueueGuard.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

namespace
{

//	----------------------------------------------------------------------------------------------
//	The queue, as laid out in the three Steam EXEs (the DX9, DX11 and Tablet builds of November 2014).
//	The code is the same in all three; only the addresses differ. All offsets are RVAs; the EXE is
//	relocated at run time, so absolute addresses are read back from its code and compared, never
//	assumed. The RVAs come from a pattern search of each EXE file (110 writer prologues, 97 reserve
//	functions, one swap and one dispatcher per EXE, every reserve calling the same import slot), and
//	the import slot was cross-checked against each EXE's import table.
//	----------------------------------------------------------------------------------------------

struct KnownBuild
{
	const char* szName;
	DWORD dwTimestamp;           //!< IMAGE_FILE_HEADER::TimeDateStamp
	DWORD dwRvaChannelIndex;     //!< int: which channel the writers use (0 in every run seen)
	DWORD dwRvaChannels;         //!< channel 0; channel c is at + c * CHANNEL_STRIDE
	DWORD dwRvaIatExchangeAdd;   //!< the EXE's import slot for kernel32!InterlockedExchangeAdd
	DWORD dwRvaWriter;           //!< one of the inlined writer prologues (PATTERN_WRITER)
	DWORD dwRvaReserve;          //!< one of the reserve functions (PATTERN_RESERVE)
	DWORD dwRvaSwap;             //!< the buffer swap (PATTERN_SWAP)
	DWORD dwRvaDispatch;         //!< the dispatcher (PATTERN_DISPATCH)
};

const KnownBuild KNOWN_BUILDS[] =
{
	//  name      timestamp   index      channels   IAT slot  writer   reserve  swap      dispatch
	{ "DX11",   0x546CD0A8, 0x160E400, 0x160F080, 0x5D647C, 0x11347, 0x404C0, 0x90DF0,  0x2A5970 },   // CivilizationV_DX11.exe
	{ "DX9",    0x546CCB59, 0x15FE200, 0x15FEE80, 0x5C9488, 0x15BC5, 0x10070, 0x385420, 0x20F40 },    // CivilizationV.exe
	{ "Tablet", 0x546CD5F8, 0x160FE80, 0x1610B00, 0x5D847C, 0x14748, 0xC380,  0xACE00,  0x3980E0 },   // CivilizationV_Tablet.exe
};

const DWORD NUM_CHANNELS = 2;                //!< handler tables at this+8+c*0x600 leave room for two
const DWORD CHANNEL_STRIDE = 0x800180;       //!< two buffers, then the flip counter and high-water mark
const DWORD BUFFER_STRIDE = 0x400080;        //!< size field and header, then the records
const DWORD HEADER_BYTES = 0x80;             //!< record = buffer + HEADER_BYTES + reserved offset
const DWORD BUFFER_CAPACITY = BUFFER_STRIDE - HEADER_BYTES;

//! Code that must be byte-identical, at the build's RVA for that role, before anything is patched.
//! "??" marks an absolute address, which relocation changes; those are checked separately against
//! the build's RVAs.
// mov ecx,[index]; imul ecx,ecx,800180h; add ecx,channels - one of the inlined writer prologues
const char* const PATTERN_WRITER = "8b 0d ?? ?? ?? ?? 69 c9 80 01 80 00 81 c1 ?? ?? ?? ??";
// buffer = channel + (flip & 1) * 400080h; push 80h; push buffer; call [InterlockedExchangeAdd]
const char* const PATTERN_RESERVE = "56 8b b1 00 01 80 00 83 e6 01 69 f6 80 00 40 00 03 f1 68 80 00 00 00 56 ff 15 ?? ?? ?? ??";
// inc [flip]; buffer = channel + (flip & 1) * 400080h - the swap
const char* const PATTERN_SWAP = "ff 81 00 01 80 00 8b 81 00 01 80 00 83 e0 01 69 c0 80 00 40 00";
// the dispatcher: p = buffer + 80h, end = p + size, walk while p != end
const char* const PATTERN_DISPATCH = "8b 44 24 04 8b 08 85 c9 74 37 56 57 8d b0 80 00 00 00 8d bc 01 80 00 00 00 3b f7 74 22";

//! Where the relocated dwords sit inside the writer and reserve patterns.
const DWORD WRITER_INDEX_OPERAND = 2;
const DWORD WRITER_CHANNELS_OPERAND = 14;
const DWORD RESERVE_IAT_OPERAND = 26;

//! Records are 128 to 2,304 bytes (235 reserve sites checked). Anything larger passes through unguarded.
const LONG SCRATCH_BYTES = 64 * 1024;

//	----------------------------------------------------------------------------------------------
//	State. Written once by Install before the slot is patched; read by the thunk on any thread.
//	----------------------------------------------------------------------------------------------

typedef LONG (WINAPI* ExchangeAddFn)(LONG volatile* pAddend, LONG lValue);

ExchangeAddFn g_pfnOriginal = NULL;
DWORD g_dwChannels = 0;
char* g_pScratch = NULL;
bool g_bInstalled = false;
const char* g_szStatus = "not installed";
char g_szActiveStatus[64] = { 0 };     //!< "active (<build> build)" once installed

volatile LONG g_lDroppedRecords = 0;
volatile LONG g_lDroppedBytes = 0;
volatile LONG g_lLargestDropped = 0;
volatile LONG g_lTooLargeForScratch = 0;
volatile LONG g_lLastDropTick = 0;

//	----------------------------------------------------------------------------------------------
//	The thunk
//	----------------------------------------------------------------------------------------------

void RaiseLargest(LONG lValue)
{
	LONG lSeen = g_lLargestDropped;
	while (lValue > lSeen)
	{
		const LONG lPrevious = InterlockedCompareExchange(&g_lLargestDropped, lValue, lSeen);
		if (lPrevious == lSeen)
			break;
		lSeen = lPrevious;
	}
}

//! Reserve lValue bytes in the buffer whose size field is pSize, or divert the record if it would not
//! fit. Returns what the EXE's own reserve returns: the offset of the record from pSize + HEADER_BYTES.
LONG ReserveOrDivert(LONG volatile* pSize, LONG lValue)
{
	LONG lOld = *pSize;
	for (;;)
	{
		const DWORD dwOld = static_cast<DWORD>(lOld);
		if (dwOld > BUFFER_CAPACITY || static_cast<DWORD>(lValue) > BUFFER_CAPACITY - dwOld)
			break;

		const LONG lSeen = InterlockedCompareExchange(pSize, lOld + lValue, lOld);
		if (lSeen == lOld)
			return lOld;
		lOld = lSeen;
	}

	if (lValue > SCRATCH_BYTES)
	{
		// Never seen; the EXE's behaviour is kept rather than writing past the scratch area.
		InterlockedIncrement(&g_lTooLargeForScratch);
		return g_pfnOriginal(pSize, lValue);
	}

	InterlockedIncrement(&g_lDroppedRecords);
	InterlockedExchangeAdd(&g_lDroppedBytes, lValue);
	RaiseLargest(lValue);
	InterlockedExchange(&g_lLastDropTick, static_cast<LONG>(GetTickCount()));

	// The buffer size is left alone, so the dispatcher never reaches this record. Concurrent writers
	// may share the scratch area; nothing reads it.
	return static_cast<LONG>(reinterpret_cast<DWORD>(g_pScratch) - (reinterpret_cast<DWORD>(pSize) + HEADER_BYTES));
}

LONG WINAPI GuardedExchangeAdd(LONG volatile* pAddend, LONG lValue)
{
	// Unsigned, so an address below the channels wraps and fails the range test too.
	const DWORD dwOffset = reinterpret_cast<DWORD>(pAddend) - g_dwChannels;
	if (dwOffset < NUM_CHANNELS * CHANNEL_STRIDE && lValue > 0)
	{
		const DWORD dwInChannel = dwOffset % CHANNEL_STRIDE;
		if (dwInChannel == 0 || dwInChannel == BUFFER_STRIDE)
			return ReserveOrDivert(pAddend, lValue);
	}
	return g_pfnOriginal(pAddend, lValue);
}

//	----------------------------------------------------------------------------------------------
//	Verification
//	----------------------------------------------------------------------------------------------

int HexNibble(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

//! True if the bytes at pCode match the pattern; "??" matches anything.
bool MatchesPattern(const unsigned char* pCode, const char* szPattern)
{
	for (const char* p = szPattern; *p != '\0';)
	{
		if (*p == ' ')
		{
			p++;
			continue;
		}
		if (p[1] == '\0')
			return false;
		if (p[0] != '?' || p[1] != '?')
		{
			const int iHigh = HexNibble(p[0]);
			const int iLow = HexNibble(p[1]);
			if (iHigh < 0 || iLow < 0 || *pCode != static_cast<unsigned char>(iHigh * 16 + iLow))
				return false;
		}
		pCode++;
		p += 2;
	}
	return true;
}

//! The EXE's import slot for KERNEL32!InterlockedExchangeAdd, or NULL.
DWORD* FindImportSlot(HMODULE hExe, const IMAGE_NT_HEADERS* pNt)
{
	const IMAGE_DATA_DIRECTORY& kDir = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (kDir.VirtualAddress == 0 || kDir.Size == 0)
		return NULL;

	char* pBase = reinterpret_cast<char*>(hExe);
	const IMAGE_IMPORT_DESCRIPTOR* pImport = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(pBase + kDir.VirtualAddress);
	for (; pImport->Name != 0; pImport++)
	{
		if (_stricmp(pBase + pImport->Name, "KERNEL32.dll") != 0 || pImport->OriginalFirstThunk == 0 || pImport->FirstThunk == 0)
			continue;

		const IMAGE_THUNK_DATA* pName = reinterpret_cast<const IMAGE_THUNK_DATA*>(pBase + pImport->OriginalFirstThunk);
		IMAGE_THUNK_DATA* pIat = reinterpret_cast<IMAGE_THUNK_DATA*>(pBase + pImport->FirstThunk);
		for (; pName->u1.AddressOfData != 0; pName++, pIat++)
		{
			if (IMAGE_SNAP_BY_ORDINAL(pName->u1.Ordinal))
				continue;
			const IMAGE_IMPORT_BY_NAME* pByName = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(pBase + pName->u1.AddressOfData);
			if (strcmp(reinterpret_cast<const char*>(pByName->Name), "InterlockedExchangeAdd") == 0)
				return reinterpret_cast<DWORD*>(&pIat->u1.Function);
		}
	}
	return NULL;
}

//! Checks everything the thunk relies on. Returns NULL if the guard may be installed, or the reason not.
const char* VerifyExe(HMODULE hExe, DWORD** ppSlot, const KnownBuild** ppBuild)
{
	const IMAGE_DOS_HEADER* pDos = reinterpret_cast<const IMAGE_DOS_HEADER*>(hExe);
	if (pDos == NULL || pDos->e_magic != IMAGE_DOS_SIGNATURE)
		return "off: no EXE image";
	const IMAGE_NT_HEADERS* pNt = reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<const char*>(hExe) + pDos->e_lfanew);
	if (pNt->Signature != IMAGE_NT_SIGNATURE)
		return "off: no EXE image";
	const KnownBuild* pBuild = NULL;
	for (size_t i = 0; i < sizeof(KNOWN_BUILDS) / sizeof(KNOWN_BUILDS[0]); i++)
	{
		if (KNOWN_BUILDS[i].dwTimestamp == pNt->FileHeader.TimeDateStamp)
			pBuild = &KNOWN_BUILDS[i];
	}
	if (pBuild == NULL)
		return "off: not an EXE build this was written for (timestamp)";

	const DWORD dwBase = reinterpret_cast<DWORD>(hExe);
	const DWORD dwImageSize = pNt->OptionalHeader.SizeOfImage;
	if (pBuild->dwRvaChannels + NUM_CHANNELS * CHANNEL_STRIDE > dwImageSize)
		return "off: image smaller than the queue";

	const struct { DWORD dwRva; const char* szBytes; } aCode[] =
	{
		{ pBuild->dwRvaWriter, PATTERN_WRITER },
		{ pBuild->dwRvaReserve, PATTERN_RESERVE },
		{ pBuild->dwRvaSwap, PATTERN_SWAP },
		{ pBuild->dwRvaDispatch, PATTERN_DISPATCH },
	};
	for (size_t i = 0; i < sizeof(aCode) / sizeof(aCode[0]); i++)
	{
		if (aCode[i].dwRva + 64 > dwImageSize ||
			!MatchesPattern(reinterpret_cast<const unsigned char*>(dwBase + aCode[i].dwRva), aCode[i].szBytes))
			return "off: EXE code differs from the known layout";
	}
	const struct { DWORD dwRva; DWORD dwExpectedRva; } aAddresses[] =
	{
		{ pBuild->dwRvaWriter + WRITER_INDEX_OPERAND, pBuild->dwRvaChannelIndex },
		{ pBuild->dwRvaWriter + WRITER_CHANNELS_OPERAND, pBuild->dwRvaChannels },
		{ pBuild->dwRvaReserve + RESERVE_IAT_OPERAND, pBuild->dwRvaIatExchangeAdd },
	};
	for (size_t i = 0; i < sizeof(aAddresses) / sizeof(aAddresses[0]); i++)
	{
		const DWORD dwTarget = *reinterpret_cast<const DWORD*>(dwBase + aAddresses[i].dwRva);
		if (dwTarget != dwBase + aAddresses[i].dwExpectedRva)
			return "off: EXE addresses differ from the known layout";
	}

	DWORD* pSlot = FindImportSlot(hExe, pNt);
	if (pSlot == NULL || reinterpret_cast<DWORD>(pSlot) != dwBase + pBuild->dwRvaIatExchangeAdd)
		return "off: InterlockedExchangeAdd import slot not where the reserve code calls it";

	const LONG lIndex = *reinterpret_cast<const LONG*>(dwBase + pBuild->dwRvaChannelIndex);
	if (lIndex < 0 || lIndex >= static_cast<LONG>(NUM_CHANNELS))
		return "off: unexpected channel index";

	*ppSlot = pSlot;
	*ppBuild = pBuild;
	return NULL;
}

void AppendToLog(const char* szLine)
{
	// Relative to the install directory, like crashlogs\crashes.log. No folder, no log.
	HANDLE hLog = CreateFileA("crashlogs\\queueguard.log", FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hLog == INVALID_HANDLE_VALUE)
		return;
	DWORD dwWritten = 0;
	WriteFile(hLog, szLine, static_cast<DWORD>(strlen(szLine)), &dwWritten, NULL);
	CloseHandle(hLog);
}

void FormatTimestamp(char* szOut, size_t uiSize)
{
	SYSTEMTIME kNow;
	GetLocalTime(&kNow);
	_snprintf_s(szOut, uiSize, _TRUNCATE, "%04d-%02d-%02d %02d:%02d:%02d",
		kNow.wYear, kNow.wMonth, kNow.wDay, kNow.wHour, kNow.wMinute, kNow.wSecond);
}

}	// namespace

namespace EngineQueueGuard
{

Stats::Stats()
	: bInstalled(false)
	, szStatus("")
	, lDroppedRecords(0)
	, lDroppedBytes(0)
	, lLargestDropped(0)
	, lTooLargeForScratch(0)
{
}

void Install(bool bEnabled)
{
	static bool s_bTried = false;
	if (s_bTried)
		return;

	char szTime[32];
	FormatTimestamp(szTime, sizeof(szTime));
	char szLine[256];

	if (!bEnabled)
	{
		// Not final: the option cache is rebuilt whenever the database is, and a later call may enable it.
		static const char* const szOff = "off: BIN_HOOKS custom mod option is 0";
		if (g_szStatus != szOff)
		{
			g_szStatus = szOff;
			_snprintf_s(szLine, sizeof(szLine), _TRUNCATE, "%s  UI message queue guard %s\r\n", szTime, g_szStatus);
			AppendToLog(szLine);
		}
		return;
	}
	s_bTried = true;

	char szDisable[8] = { 0 };
	if (GetEnvironmentVariableA("VP_QUEUEGUARD", szDisable, sizeof(szDisable)) != 0 && szDisable[0] == '0')
	{
		g_szStatus = "off: VP_QUEUEGUARD=0";
		_snprintf_s(szLine, sizeof(szLine), _TRUNCATE, "%s  %s\r\n", szTime, g_szStatus);
		AppendToLog(szLine);
		return;
	}

	HMODULE hExe = GetModuleHandleA(NULL);
	DWORD* pSlot = NULL;
	const KnownBuild* pBuild = NULL;
	const char* szProblem = VerifyExe(hExe, &pSlot, &pBuild);
	if (szProblem == NULL)
	{
		g_pScratch = static_cast<char*>(VirtualAlloc(NULL, SCRATCH_BYTES, MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE));
		if (g_pScratch == NULL)
			szProblem = "off: could not allocate the scratch area";
	}
	if (szProblem == NULL)
	{
		DWORD dwOldProtect = 0;
		if (!VirtualProtect(pSlot, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect))
			szProblem = "off: import slot not writable";
		else
		{
			// Everything the thunk reads is in place before the first call can reach it; the exchange is
			// a full barrier, and a caller already past the slot still holds the original, which works.
			g_dwChannels = reinterpret_cast<DWORD>(hExe) + pBuild->dwRvaChannels;
			g_pfnOriginal = reinterpret_cast<ExchangeAddFn>(*pSlot);
			InterlockedExchange(reinterpret_cast<LONG volatile*>(pSlot), reinterpret_cast<LONG>(&GuardedExchangeAdd));
			DWORD dwIgnored = 0;
			VirtualProtect(pSlot, sizeof(DWORD), dwOldProtect, &dwIgnored);
			g_bInstalled = true;
		}
	}

	if (g_bInstalled)
	{
		_snprintf_s(g_szActiveStatus, sizeof(g_szActiveStatus), _TRUNCATE, "active (%s build)", pBuild->szName);
		g_szStatus = g_szActiveStatus;
	}
	else
		g_szStatus = szProblem;
	_snprintf_s(szLine, sizeof(szLine), _TRUNCATE, "%s  UI message queue guard %s\r\n", szTime, g_szStatus);
	AppendToLog(szLine);
}

void GetStats(Stats& kOut)
{
	kOut.bInstalled = g_bInstalled;
	kOut.szStatus = g_szStatus;
	kOut.lDroppedRecords = g_lDroppedRecords;
	kOut.lDroppedBytes = g_lDroppedBytes;
	kOut.lLargestDropped = g_lLargestDropped;
	kOut.lTooLargeForScratch = g_lTooLargeForScratch;
}

void ReportDrops()
{
	static LONG s_lReportedRecords = 0;
	static LONG s_lReportedBytes = 0;

	const LONG lRecords = g_lDroppedRecords;
	if (lRecords == s_lReportedRecords)
		return;
	// Wait for a quiet two seconds, so one load produces one line.
	if (GetTickCount() - static_cast<DWORD>(g_lLastDropTick) < 2000)
		return;

	const LONG lBytes = g_lDroppedBytes;
	char szTime[32];
	FormatTimestamp(szTime, sizeof(szTime));
	char szLine[320];
	_snprintf_s(szLine, sizeof(szLine), _TRUNCATE,
		"%s  dropped %ld records (%ld KB) that did not fit the UI message queue; %ld records (%ld KB) since launch, largest %ld bytes, %ld too large to divert\r\n",
		szTime, lRecords - s_lReportedRecords, (lBytes - s_lReportedBytes) >> 10, lRecords, lBytes >> 10,
		static_cast<LONG>(g_lLargestDropped), static_cast<LONG>(g_lTooLargeForScratch));
	AppendToLog(szLine);

	s_lReportedRecords = lRecords;
	s_lReportedBytes = lBytes;
}

void FormatCrashLine(char* szOut, size_t uiSize)
{
	_snprintf_s(szOut, uiSize, _TRUNCATE, "UI message queue guard: %s, %ld records (%ld KB) dropped, %ld too large\n",
		g_szStatus, static_cast<LONG>(g_lDroppedRecords), static_cast<LONG>(g_lDroppedBytes) >> 10,
		static_cast<LONG>(g_lTooLargeForScratch));
}

}	// namespace EngineQueueGuard
