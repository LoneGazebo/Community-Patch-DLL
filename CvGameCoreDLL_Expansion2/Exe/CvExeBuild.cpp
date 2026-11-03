#include "CvGameCoreDLLPCH.h"
#include "CvExeBuild.h"
#include "LintFree.h"

namespace
{

const DWORD PREFERRED_BASE = 0x400000;

// Indexed by ExeBuild::Type
const struct
{
	const char* szName;
	DWORD dwTimestamp;
}
BUILDS[ExeBuild::NUM_TYPES] =
{
	{ "DX11",   0x546CD0A8 }, // CivilizationV_DX11.exe
	{ "DX9",    0x546CCB59 }, // CivilizationV.exe
	{ "Tablet", 0x546CD5F8 }, // CivilizationV_Tablet.exe
};

bool s_bDetected = false;
ExeBuild::Type s_eType = ExeBuild::UNKNOWN;

DWORD GetImageBase()
{
	return reinterpret_cast<DWORD>(GetModuleHandleA(NULL));
}

const IMAGE_NT_HEADERS* GetNtHeaders()
{
	const DWORD dwBase = GetImageBase();

	if (dwBase == 0)
	{
		return NULL;
	}

	const IMAGE_DOS_HEADER* pDos =
		reinterpret_cast<const IMAGE_DOS_HEADER*>(dwBase);

	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return NULL;
	}

	const IMAGE_NT_HEADERS* pNt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
		dwBase + pDos->e_lfanew
	);

	return pNt->Signature == IMAGE_NT_SIGNATURE ? pNt : NULL;
}

ExeBuild::Type Detect()
{
	const IMAGE_NT_HEADERS* pNt = GetNtHeaders();
	const DWORD dwTimestamp = pNt ? pNt->FileHeader.TimeDateStamp : 0;

	for (int i = 0; i < ExeBuild::NUM_TYPES; ++i)
	{
		if (BUILDS[i].dwTimestamp == dwTimestamp)
		{
			CUSTOMLOG("ExeBuild: %s EXE at %08X",
				BUILDS[i].szName,
				(unsigned int)GetImageBase()
			);
			return static_cast<ExeBuild::Type>(i);
		}
	}

	CUSTOMLOG("ExeBuild: unknown EXE (timestamp %08X)", (unsigned int)dwTimestamp);

	return ExeBuild::UNKNOWN;
}
} // namespace

//------------------------------------------------------------------------------
ExeBuild::Type ExeBuild::GetType()
{
	if (!s_bDetected)
	{
		s_eType = Detect();
		s_bDetected = true;
	}

	return s_eType;
}

const char* ExeBuild::GetName(Type eBuild)
{
	if (eBuild < 0 || eBuild >= NUM_TYPES)
	{
		return "Unknown";
	}
	return BUILDS[eBuild].szName;
}

DWORD ExeBuild::ToRuntimeAddress(DWORD dwVA, unsigned int uiLen)
{
	const IMAGE_NT_HEADERS* pNt = GetNtHeaders();

	if (pNt == NULL || dwVA < PREFERRED_BASE || uiLen == 0)
	{
		return 0;
	}

	// RVA first: the EXE can be loaded below PREFERRED_BASE
	const DWORD dwRva = dwVA - PREFERRED_BASE;
	const DWORD dwSize = pNt->OptionalHeader.SizeOfImage;

	if (dwRva >= dwSize || uiLen > dwSize - dwRva)
	{
		return 0;
	}

	return GetImageBase() + dwRva;
}
