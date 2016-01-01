
#include "CvGameCoreDLLPCH.h"

#include "jdh\utils.h"



namespace jdh
{
	void GetDllPath(HMODULE hModule, _Out_z_cap_(bufferSize) char* buffer, int bufferSize)
	{
		GetModuleFileName(hModule, buffer, bufferSize);
		int pos = MAX((int)strrchr(buffer, '/'), (int)strrchr(buffer, '\\')) - (int)buffer;
		__analysis_assume(pos < bufferSize-1);
		if (pos > 0)
		{
			buffer[pos+1] = '\0';
		}
	}
}
