#include "CvGameCoreDLLPCH.h"

#include "jdh\log.h"

#ifdef __JDHLOGFILE

namespace CvPreGame
{
	int gameTurn();
	bool gameStarted();
}

namespace jdh
{
	namespace impl
	{
		std::string& GetTempString()
		{
			static size_t s_cur = 0;
			static std::string s_strings[10];
			std::string& r = s_strings[s_cur];
			s_cur = (s_cur + 1) % ARRAYSIZE(s_strings);
			r.clear();
			return r;
		}

#ifdef MOD_ANALYZE
		LOG_LEVEL g_LogLevel = ANALYSE;
#elif defined(MOD_DEBUG)
		LOG_LEVEL g_LogLevel = DEBUG;
#else
		LOG_LEVEL g_LogLevel = INFO;
#endif

		size_t g_IdentIndex = 0;
		char g_Ident[g_IdentMax+1] = "\0                                       ";
		std::stack<LOG_LEVEL> g_LogLevelStack;

		PCSTR timestring()
		{
			static char buffer[(5+7+3+3+3+5)+5];
			double ftime = FTimer::Read();
			uint32 ntime = static_cast<uint32>(ftime);
			uint16 milliseconds = static_cast<uint>(((ftime - ntime) * 1000.0)) % 1000;
			uint8 seconds = ntime % 60;
			ntime /= 60;
			uint8 minutes = ntime % 60;
			ntime /= 60;
			uint8 hours = ntime % 24;

			CvGame* pGame = GC.getGamePointer();
			int turnSlice = pGame ? pGame->getTurnSlice() : -1;

			_snprintf_s(buffer, _TRUNCATE, "%4i.%05i: %02u:%02u:%02u.%04u ", CvPreGame::gameTurn(), turnSlice, hours, minutes, seconds, milliseconds);

			return buffer;
		}

		LogLockGuard::mutex LogLockGuard::s_m;

		std::string& LogFileBuffer(LOG_LEVEL eLevel, LogLockGuard& lock)
		{
			static std::string buffer;
			buffer.clear();
			buffer += timestring();
			append(buffer, eLevel);
			buffer += g_Ident;
			return buffer;
		}
	}
}

#endif
