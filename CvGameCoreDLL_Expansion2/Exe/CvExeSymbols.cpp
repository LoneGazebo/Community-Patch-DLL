#include "CvGameCoreDLLPCH.h"
#include "CvExeSymbols.h"
#include "CvExeBuild.h"

#include <string.h>

// must be included after all other headers
#include "LintFree.h"

namespace
{
enum Kind
{
	EXE_FUNCTION,
	EXE_CODE,
	EXE_DATA,
	EXE_VTABLE,
	EXE_OFFSET
};
enum CheckKind { EXE_ABS32, EXE_REL32, EXE_VAL8 };

const char* const KIND_NAMES[] =
{
	"function", "code", "data", "vtable", "offset"
};

struct SymbolDef
{
	const char* szName;
	Kind eKind;
	// indexed by ExeBuild::Type
	DWORD aValue[ExeBuild::NUM_TYPES];
	const char* szSignature;
};

const SymbolDef SYMBOLS[NUM_EXE_SYMBOLS] =
{
#define EXE_SYMBOL(name, kind, dx11, dx9, tablet, signature) \
	{ #name, kind, { dx11, dx9, tablet }, signature },
#include "CvExeAddresses.h"
#undef EXE_SYMBOL
};

struct CheckDef
{
	CheckKind eKind;
	ExeSymbol eFrom;
	DWORD dwOffset;
	ExeSymbol eTo;
};

const CheckDef CHECKS[] =
{
#define EXE_CHECK(kind, from, offset, to) \
	{ kind, EXE_##from, offset, EXE_##to },
#include "CvExeAddresses.h"
#undef EXE_CHECK
};

const unsigned int NUM_CHECKS = sizeof(CHECKS) / sizeof(CHECKS[0]);

struct SymbolState
{
	DWORD dwRuntime; // runtime address, or the offset; 0 = unresolved
	bool bOk;
	int iVouched;    // passing checks from trusted symbols pointing here
};

bool s_bResolved = false;
ExeBuild::Type s_eResolvedFor = ExeBuild::UNKNOWN;
SymbolState s_aState[NUM_EXE_SYMBOLS];

//------------------------------------------------------------------------------
int HexNibble(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}

	if (c >= 'a' && c <= 'f')
	{
		return c - 'a' + 10;
	}

	if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}

	return -1;
}

//! Bytes a pattern covers; 0 for none or a malformed one.
unsigned int PatternLength(const char* szPattern)
{
	if (szPattern == NULL)
	{
		return 0;
	}

	unsigned int uiLen = 0;

	for (const char* p = szPattern; *p != '\0'; )
	{
		if (*p == ' ')
		{
			++p;
			continue;
		}

		if (p[1] == '\0')
		{
			return 0;
		}

		++uiLen;
		p += 2;
	}

	return uiLen;
}

bool MatchPattern(DWORD dwAddr, DWORD dwVA, const char* szPattern,
	const char* szWhat)
{
	const unsigned char* pCode = reinterpret_cast<const unsigned char*>(dwAddr);
	unsigned int uiOffset = 0;

	for (const char* p = szPattern; *p != '\0'; )
	{
		if (*p == ' ')
		{
			++p;
			continue;
		}

		if (p[0] != '?' || p[1] != '?')
		{
			const int iHi = HexNibble(p[0]);
			const int iLo = HexNibble(p[1]);

			if (iHi < 0 || iLo < 0)
			{
				CUSTOMLOG("ExeSymbols: %s: malformed pattern \"%s\"",
					szWhat, szPattern);
				return false;
			}

			const unsigned int uiExpected = (unsigned int)((iHi << 4) | iLo);
			const unsigned int uiActual = pCode[uiOffset];

			if (uiActual != uiExpected)
			{
				CUSTOMLOG("ExeSymbols: %s: bytes at %08X+%u differ (%02X, expected %02X)",
					szWhat, (unsigned int)dwVA,
					uiOffset, uiActual, uiExpected);
				return false;
			}
		}

		++uiOffset;
		p += 2;
	}

	return true;
}

//------------------------------------------------------------------------------

bool CheckAbs32(DWORD dwFromVA, const SymbolDef& kTo, const char* szWhat)
{
	const DWORD dwAddr = ExeBuild::ToRuntimeAddress(dwFromVA, 4);
	const DWORD dwTarget =
		ExeBuild::ToRuntimeAddress(kTo.aValue[s_eResolvedFor], 1);

	if (dwAddr == 0 || dwTarget == 0)
	{
		return false;
	}

	const DWORD dwValue = *reinterpret_cast<const DWORD*>(dwAddr);

	if (dwValue != dwTarget)
	{
		CUSTOMLOG("ExeSymbols: check %s -> %s: address operand is %08X, expected %08X",
			szWhat, kTo.szName, (unsigned int)dwValue,
			(unsigned int)dwTarget);
		return false;
	}

	return true;
}

bool CheckRel32(DWORD dwFromVA, const SymbolDef& kTo, const char* szWhat)
{
	const DWORD dwToVA = kTo.aValue[s_eResolvedFor];
	const DWORD dwAddr = ExeBuild::ToRuntimeAddress(dwFromVA, 5);

	if (dwAddr == 0 || ExeBuild::ToRuntimeAddress(dwToVA, 1) == 0)
	{
		return false;
	}

	const unsigned char* pInstr =
		reinterpret_cast<const unsigned char*>(dwAddr);

	if (pInstr[0] != 0xE8 && pInstr[0] != 0xE9)
	{
		CUSTOMLOG("ExeSymbols: check %s -> %s: no call/jmp rel32 (%02X)",
			szWhat, kTo.szName, (unsigned int)pInstr[0]);
		return false;
	}

	LONG lRel;
	memcpy(&lRel, pInstr + 1, 4);
	// rel32 is position independent: compare VAs, not runtime addresses
	const DWORD dwTargetVA = dwFromVA + 5 + static_cast<DWORD>(lRel);

	if (dwTargetVA != dwToVA)
	{
		CUSTOMLOG("ExeSymbols: check %s -> %s: branch targets %08X, expected %08X",
			szWhat, kTo.szName, (unsigned int)dwTargetVA,
			(unsigned int)dwToVA);
		return false;
	}

	return true;
}

bool CheckValue(DWORD dwFromVA, unsigned int uiSize, const SymbolDef& kTo,
	const char* szWhat)
{
	const DWORD dwAddr = ExeBuild::ToRuntimeAddress(dwFromVA, uiSize);

	if (dwAddr == 0)
	{
		return false;
	}

	DWORD dwValue = 0;
	memcpy(&dwValue, reinterpret_cast<const void*>(dwAddr), uiSize);

	if (dwValue != kTo.aValue[s_eResolvedFor])
	{
		CUSTOMLOG("ExeSymbols: check %s -> %s: value is %X, expected %X",
			szWhat, kTo.szName, (unsigned int)dwValue,
			(unsigned int)kTo.aValue[s_eResolvedFor]);
		return false;
	}

	return true;
}

bool RunCheck(const CheckDef& kCheck, const SymbolDef& kFrom,
	const SymbolDef& kTo)
{
	const DWORD dwFromVA = kFrom.aValue[s_eResolvedFor] + kCheck.dwOffset;
	char szWhat[128];
	sprintf_s(szWhat, sizeof(szWhat), "%s+%02X", kFrom.szName,
		(unsigned int)kCheck.dwOffset);

	bool bOk = false;

	switch (kCheck.eKind)
	{
		case EXE_ABS32:
			bOk = CheckAbs32(dwFromVA, kTo, szWhat);
			break;
		case EXE_REL32:
			bOk = CheckRel32(dwFromVA, kTo, szWhat);
			break;
		case EXE_VAL8:
			bOk = CheckValue(dwFromVA, 1, kTo, szWhat);
			break;
	}

	if (!bOk)
	{
		CUSTOMLOG("ExeSymbols: check %s -> %s failed", szWhat, kTo.szName);
	}

	return bOk;
}

//------------------------------------------------------------------------------
//! Phase 1: address and own signature.
void ResolveAddresses()
{
	for (int i = 0; i < NUM_EXE_SYMBOLS; ++i)
	{
		const SymbolDef& kDef = SYMBOLS[i];
		const DWORD dwValue = kDef.aValue[s_eResolvedFor];

		if (kDef.eKind == EXE_OFFSET)
		{
			s_aState[i].dwRuntime = dwValue;
			s_aState[i].bOk = true;
			continue;
		}

		const bool bPattern = kDef.szSignature != NULL;
		const unsigned int uiLen = bPattern
			? PatternLength(kDef.szSignature) : 1;
		const DWORD dwAddr = uiLen > 0
			? ExeBuild::ToRuntimeAddress(dwValue, uiLen) : 0;

		if (dwAddr == 0)
		{
			CUSTOMLOG("ExeSymbols: %s: %08X is outside the image or the signature is malformed",
				kDef.szName, (unsigned int)dwValue);
			continue;
		}

		if (bPattern
			&& !MatchPattern(dwAddr, dwValue, kDef.szSignature, kDef.szName))
		{
			continue;
		}

		s_aState[i].dwRuntime = dwAddr;
		s_aState[i].bOk = true;
	}
}

//! Phase 2: run the checks. A failed check invalidates both symbols it
//! names.
void RunChecks(bool* aPassed)
{
	for (unsigned int c = 0; c < NUM_CHECKS; ++c)
	{
		const CheckDef& kCheck = CHECKS[c];
		const SymbolDef& kFrom = SYMBOLS[kCheck.eFrom];
		const SymbolDef& kTo = SYMBOLS[kCheck.eTo];
		aPassed[c] = RunCheck(kCheck, kFrom, kTo);

		if (!aPassed[c])
		{
			s_aState[kCheck.eFrom].bOk = false;
			s_aState[kCheck.eTo].bOk = false;
		}
	}
}

//! Phase 3: until stable, drop symbols that refer to unresolved ones, and
//! symbols without a signature that nothing vouches for.
void PropagateTrust(const bool* aPassed)
{
	bool bChanged = true;
	while (bChanged)
	{
		bChanged = false;

		for (int i = 0; i < NUM_EXE_SYMBOLS; ++i)
		{
			s_aState[i].iVouched = 0;
		}

		for (unsigned int c = 0; c < NUM_CHECKS; ++c)
		{
			const CheckDef& kCheck = CHECKS[c];

			if (!aPassed[c] || kCheck.eFrom == kCheck.eTo)
			{
				continue;
			}

			SymbolState& kFrom = s_aState[kCheck.eFrom];

			if (kFrom.bOk && !s_aState[kCheck.eTo].bOk)
			{
				CUSTOMLOG("ExeSymbols: %s: refers to unresolved %s",
					SYMBOLS[kCheck.eFrom].szName, SYMBOLS[kCheck.eTo].szName);
				kFrom.bOk = false;
				bChanged = true;
			}

			if (kFrom.bOk)
			{
				++s_aState[kCheck.eTo].iVouched;
			}
		}

		for (int i = 0; i < NUM_EXE_SYMBOLS; ++i)
		{
			SymbolState& kState = s_aState[i];
			if (kState.bOk && SYMBOLS[i].szSignature == NULL
				&& kState.iVouched == 0)
			{
				CUSTOMLOG("ExeSymbols: %s: no signature and no resolved symbol vouches for it",
					SYMBOLS[i].szName);
				kState.bOk = false;
				bChanged = true;
			}
		}
	}
}

void LogResults()
{
	int iResolved = 0;

	for (int i = 0; i < NUM_EXE_SYMBOLS; ++i)
	{
		const SymbolDef& kDef = SYMBOLS[i];
		SymbolState& kState = s_aState[i];

		if (!kState.bOk)
		{
			kState.dwRuntime = 0;
			CUSTOMLOG("ExeSymbols: %s: NOT resolved (see above)", kDef.szName);

			continue;
		}

		++iResolved;

		if (kDef.eKind == EXE_OFFSET)
		{
			CUSTOMLOG("ExeSymbols: %s: offset +%X (vouched by %d)",
				kDef.szName, (unsigned int)kState.dwRuntime, kState.iVouched);
		}
		else
		{
			CUSTOMLOG("ExeSymbols: %s: %s at %08X -> %08X (%s, vouched by %d)",
				kDef.szName, KIND_NAMES[kDef.eKind],
				(unsigned int)kDef.aValue[s_eResolvedFor],
				(unsigned int)kState.dwRuntime,
				kDef.szSignature ? "signature ok" : "no signature",
				kState.iVouched);
		}
	}

	CUSTOMLOG("ExeSymbols: %d of %d symbols resolved", iResolved,
		(int)NUM_EXE_SYMBOLS);
}
} // namespace

//------------------------------------------------------------------------------
void ExeSymbols::Resolve()
{
	if (s_bResolved)
	{
		return;
	}

	s_bResolved = true;

	for (int i = 0; i < NUM_EXE_SYMBOLS; ++i)
	{
		s_aState[i].dwRuntime = 0;
		s_aState[i].bOk = false;
		s_aState[i].iVouched = 0;
	}

	s_eResolvedFor = ExeBuild::GetType();
	const char* szBuild = ExeBuild::GetName(s_eResolvedFor);

	if (s_eResolvedFor == ExeBuild::UNKNOWN)
	{
		CUSTOMLOG("ExeSymbols: unknown EXE, no symbols resolved");
		return;
	}

	CUSTOMLOG("ExeSymbols: resolving %d symbols for the %s EXE",
		(int)NUM_EXE_SYMBOLS, szBuild);

	bool aPassed[NUM_CHECKS > 0 ? NUM_CHECKS : 1];
	ResolveAddresses();
	RunChecks(aPassed);
	PropagateTrust(aPassed);
	LogResults();
}

//------------------------------------------------------------------------------
bool ExeSymbols::IsResolved(ExeSymbol eSymbol)
{
	Resolve();

	if (eSymbol < 0 || eSymbol >= NUM_EXE_SYMBOLS)
	{
		return false;
	}

	return s_aState[eSymbol].bOk;
}

DWORD ExeSymbols::Get(ExeSymbol eSymbol)
{
	return IsResolved(eSymbol) ? s_aState[eSymbol].dwRuntime : 0;
}
