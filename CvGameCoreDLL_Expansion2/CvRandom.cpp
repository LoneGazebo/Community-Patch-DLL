/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvRandom.h"
#include "CvGlobals.h"
#include "FCallStack.h"
#include "FStlContainerSerialization.h"

// include this after all other headers!
#include "LintFree.h"

#define RANDOM_A      (1103515245)
#define RANDOM_C      (12345)
#define RANDOM_SHIFT  (16)

CvRandom::CvRandom(const std::string& name) :
	m_name(name)
	, m_ullRandomSeed(0)
	, m_ulCallCount(0)
	, m_ulResetCount(0)
	, m_bSynchronous(true)
{
	reset();
}

CvRandom* CvRandom::Clone() const {
    CvRandom* newRandom = new CvRandom(m_name);
    newRandom->m_ullRandomSeed = m_ullRandomSeed;
    newRandom->m_ulCallCount = m_ulCallCount;
    newRandom->m_ulResetCount = m_ulResetCount;
    newRandom->m_bSynchronous = m_bSynchronous;
    return newRandom;
}

/*
// private
CvRandom::CvRandom(const CvRandom& source) :
	m_ullRandomSeed(source.m_ullRandomSeed)
	, m_ulCallCount(source.m_ulCallCount)
	, m_ulResetCount(source.m_ulResetCount)
	, m_bSynchronous(source.m_bSynchronous)
{
}
*/

bool CvRandom::operator==(const CvRandom& source) const
{
	return(m_ullRandomSeed == source.m_ullRandomSeed);
}

bool CvRandom::operator!=(const CvRandom& source) const
{
	return !(*this == source);
}

CvRandom::~CvRandom()
{
	uninit();
}


void CvRandom::init(unsigned long long ullSeed)
{
	//--------------------------------
	// Init saved data
	reset(ullSeed);

	//--------------------------------
	// Init non-saved data
}


void CvRandom::uninit()
{
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvRandom::reset(unsigned long long ullSeed)
{
	if (ullSeed != m_ullRandomSeed)
		reseed(ullSeed);

	m_ulCallCount = 0;
}

unsigned long CvRandom::get(unsigned long ulNum, const char* pszLog)
{
	// Thread safety check
	if (!gDLL->IsGameCoreThread() && gDLL->IsGameCoreExecuting() && m_bSynchronous)
	{
		OutputDebugString("Warning: GUI is accessing the synchronous random number generator while the game core is running.");
		ASSERT_DEBUG(false && "Invalid thread access to synchronous RNG");
	}

	//catch trivial cases
	if (ulNum < 2)
		return 0;

	m_ulCallCount++;

	unsigned long long ullNewSeed = ((RANDOM_A * m_ullRandomSeed) + RANDOM_C);
	unsigned long ul = ((unsigned long)((((ullNewSeed >> RANDOM_SHIFT) & MAX_UNSIGNED_INT) * (ulNum)) / (MAX_UNSIGNED_INT + 1LL)));

	if (GC.getLogging())
	{
		int iRandLogging = GC.getRandLogging();
		if (iRandLogging > 0 && (m_bSynchronous || (iRandLogging & RAND_LOGGING_ASYNCHRONOUS_FLAG) != 0))
		{
			CvGame& kGame = GC.getGame();
			if (kGame.getTurnSlice() > 0 || ((iRandLogging & RAND_LOGGING_PREGAME_FLAG) != 0))
			{
				FILogFile* pLog = LOGFILEMGR.GetLog("RandCalls.csv", FILogFile::kDontTimeStamp);
				if (pLog)
				{
					char szOut[1024] = { 0 };
					sprintf_s(szOut, "%s, %d, max %lu, res %lu, seed %I64u, cc %d, rc %d, %s, %s\n", m_name.c_str(), kGame.getGameTurn(),
						ulNum, ul, ullNewSeed, m_ulCallCount, m_ulResetCount, m_bSynchronous ? "sync" : "async", (pszLog != NULL) ? pszLog : "Unknown");
					pLog->Msg(szOut);

#if defined(VPDEBUG)
				if (m_ulResetCount > 100)  // Example check
				{
					char szDebugInfo[256];
					sprintf_s(szDebugInfo, "Warning: High RNG reset count: %d\n", m_ulResetCount);
					OutputDebugString(szDebugInfo);
				}
#endif
				}
			}
		}
	}

	m_ullRandomSeed = ullNewSeed;
	return ul;
}

float CvRandom::getFloat()
{
	return (((float)(get(MAX_UNSIGNED_INT))) / ((float)MAX_UNSIGNED_INT));
}


void CvRandom::reseed(unsigned long long ullNewValue)
{
	if (GC.getRandLogging())
	{
		FILogFile* pLog = LOGFILEMGR.GetLog("RandCalls.csv", FILogFile::kDontTimeStamp);
		char szOut[1024] = { 0 };
		sprintf_s(szOut, "%s reseeding: old seed seed %I64u, new seed %I64u, reseed count %d, call count %d\n",
			m_name.c_str(), m_ullRandomSeed, ullNewValue, m_ulResetCount, m_ulCallCount);
		pLog->Msg(szOut);
	}

	m_ulResetCount++;
	m_ullRandomSeed = ullNewValue;

	//do not reset the call count
}


unsigned long long CvRandom::getSeed() const
{
	return m_ullRandomSeed;
}

unsigned long CvRandom::getCallCount() const
{
	return m_ulCallCount;
}

unsigned long CvRandom::getResetCount() const
{
	return m_ulResetCount;
}

template<typename Random, typename Visitor>
void CvRandom::Serialize(Random& random, Visitor& visitor)
{
	visitor(random.m_ullRandomSeed);
	visitor(random.m_ulCallCount);
	visitor(random.m_ulResetCount);
}

void CvRandom::read(FDataStream& kStream)
{
	reset();

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}


void CvRandom::write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator<<(FDataStream& saveTo, const CvRandom& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvRandom& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

void CvRandom::CopyFrom(const CvRandom& rhs)
{
	m_ullRandomSeed = rhs.m_ullRandomSeed;
	m_ulCallCount = rhs.m_ulCallCount;
	m_ulResetCount = rhs.m_ulResetCount;
	m_bSynchronous = rhs.m_bSynchronous;
	m_name = rhs.m_name + "(Copy)";
}
