/*	--------------------------------------------------------------------------
	ExeSymbols - resolves CvExeAddresses.h for the running EXE. Only reads
	memory.
	------------------------------------------------------------------------- */

#pragma once

#ifndef CV_EXE_SYMBOLS_H
#define CV_EXE_SYMBOLS_H

enum ExeSymbol
{
#define EXE_SYMBOL(name, kind, dx11, dx9, tablet, signature) EXE_##name,
#include "CvExeAddresses.h"
#undef EXE_SYMBOL
	NUM_EXE_SYMBOLS
};

namespace ExeSymbols
{
	//! Once per DLL load.
	void Resolve();

	//! Resolves on first use.
	bool IsResolved(ExeSymbol eSymbol);

	//! Whether the running build has it at all (its column isn't EXE_NONE),
	//! resolved or not.
	bool IsInBuild(ExeSymbol eSymbol);

	//! Runtime address (or, for EXE_OFFSET, the offset). 0 if the symbol is
	//! not resolved.
	DWORD Get(ExeSymbol eSymbol);
}

#endif // CV_EXE_SYMBOLS_H
