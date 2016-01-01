#pragma once

#include "assert.h"

namespace jdh
{
	void GetDllPath(HMODULE hModule, _Out_z_cap_(bufferSize) char* buffer, int bufferSize);
	template<int bufferSize>
	void GetDllPath(HMODULE hModule, char(&buffer)[bufferSize])
	{
		GetDllPath(hModule, buffer, bufferSize);
	}


	inline void uiVersionCombine(_Inout_ uint& uiVersion, uint8 jdhVersion)
	{
		JDH_Assert(uiVersion < 0x00FFFFFF);
		uiVersion = (uiVersion & 0x00FFFFFF) | (jdhVersion << 24);
	}

	inline uint uiVersionCombine(uint const uiVersion, uint8 const jdhVersion)
	{
		JDH_Assert(uiVersion < 0x00FFFFFF);
		return (uiVersion & 0x00FFFFFF) | (jdhVersion << 24);
	}

	inline void uiVersionSplit(_Inout_ uint& uiVersion, _Out_ uint8& jdhVersion)
	{
		jdhVersion = uiVersion >> 24;
		uiVersion = uiVersion & 0x00FFFFFF;
	}
}
