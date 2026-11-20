#pragma once

#ifndef CV_EXE_BUILD_H
#define CV_EXE_BUILD_H

namespace ExeBuild
{
	enum Type
	{
		UNKNOWN = -1,
		DX11,
		DX9,
		TABLET,
		NUM_TYPES
	};

	//! The running EXE, or UNKNOWN. Cached after the first call.
	Type GetType();

	//! "DX11", "DX9", "Tablet" or "Unknown".
	const char* GetName(Type eBuild);

	//! dwVA at the preferred base 0x400000 -> address in the running EXE, or 0
	//! if [dwVA, dwVA + uiLen) is outside the image.
	DWORD ToRuntimeAddress(DWORD dwVA, unsigned int uiLen);
}

#endif // CV_EXE_BUILD_H
