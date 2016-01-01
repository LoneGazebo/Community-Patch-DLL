#include "CvGameCoreDLLPCH.h"

#include "jdh\tostring.h"


namespace jdh
{
	PCSTR szprintf(PCSTR fmt, ...)
	{
		static size_t s_cur = 0;
		typedef char buffer[1024];
		static buffer s_cstr[2];
		buffer& cstr = s_cstr[s_cur];
		s_cur = (s_cur + 1) % ARRAYSIZE(s_cstr);
		cstr[0] = '\0';
		va_list args;
		va_start(args, fmt);
		vsnprintf_s(cstr, ARRAYSIZE(cstr), _TRUNCATE, fmt, args);
		return cstr;
	}
}
