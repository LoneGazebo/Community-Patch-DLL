#include "CvGameCoreDLLPCH.h"
#include "CvEventLog.h"

// Include this after all other headers.
#include "LintFree.h"

#define MAX_EVENTS 50

FDataStream& operator>>(FDataStream& loadFrom, CvEventLog::EventLogEntry& writeTo)
{
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_iTurn;
	loadFrom >> writeTo.m_strMessage;
	loadFrom >> writeTo.m_eOtherPlayer;
	loadFrom >> writeTo.m_iX;
	loadFrom >> writeTo.m_iY;
	loadFrom >> writeTo.m_iData1;
	loadFrom >> writeTo.m_iData2;
	loadFrom >> writeTo.m_iData3;
	loadFrom >> writeTo.m_iData4;

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvEventLog::EventLogEntry& readFrom)
{
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_iTurn;
	saveTo << readFrom.m_strMessage;
	saveTo << readFrom.m_eOtherPlayer;
	saveTo << readFrom.m_iX;
	saveTo << readFrom.m_iY;
	saveTo << readFrom.m_iData1;
	saveTo << readFrom.m_iData2;
	saveTo << readFrom.m_iData3;
	saveTo << readFrom.m_iData4;

	return saveTo;
}

void CvEventLog::EventLogEntry::Clear()
{
	m_iTurn = -1;
	m_strMessage = "";
	m_eOtherPlayer = NO_PLAYER;
	m_iX = -1;
	m_iY = -1;
	m_iData1 = -1;
	m_iData2 = -1;
	m_iData3 = -1;
	m_iData4 = -1;
}

/// Constructor
CvEventLog::CvEventLog(void)
{
	Uninit();
}

/// Destructor
CvEventLog::~CvEventLog(void)
{
	Uninit();
}

/// Init
void CvEventLog::Init(PlayerTypes ePlayer)
{
	Uninit();
	m_ePlayer = ePlayer;

	m_aEvents.resize(MAX_EVENTS);
	for (uint ui = 0; ui < m_aEvents.size(); ui++)
	{
		m_aEvents[ui].Clear();
	}
	m_iEventsBeginIndex = 0;
	m_iEventsEndIndex = 0;
}

/// Uninit
void CvEventLog::Uninit(void)
{
	m_ePlayer = NO_PLAYER;
	m_iCurrentLookupIndex = 0;
	m_aEvents.clear();

	m_iEventsBeginIndex = -1;
	m_iEventsEndIndex = -1;
}

/// Serialization read
void CvEventLog::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_ePlayer;
	kStream >> m_iCurrentLookupIndex;
	kStream >> m_iEventsBeginIndex;
	kStream >> m_iEventsEndIndex;

	for (uint ui = 0; ui < MAX_EVENTS; ui++)
	{
		kStream >> m_aEvents[ui];
	}
}

/// Serialization write
void CvEventLog::Write(FDataStream& kStream) const
{
	MOD_SERIALIZE_INIT_WRITE(kStream);

	// need to serialize notification list
	kStream << m_ePlayer;
	kStream << m_iCurrentLookupIndex;
	kStream << m_iEventsBeginIndex;
	kStream << m_iEventsEndIndex;

	for (uint ui = 0; ui < MAX_EVENTS; ui++)
	{
		kStream << m_aEvents[ui];
	}
}

bool CvEventLog::Add(const char* szMessage, PlayerTypes eOtherPlayerID, int iX, int iY, int iData1, int iData2, int iData3, int iData4)
{
	// if the player is not human, do not record
	if (!GET_PLAYER(m_ePlayer).isHuman())
	{
		return false;
	}

	EventLogEntry newEvent;
	newEvent.Clear();
	newEvent.m_iTurn = GC.getGame().getGameTurn();
	newEvent.m_strMessage = szMessage;
	newEvent.m_eOtherPlayer = eOtherPlayerID;
	newEvent.m_iX = iX;
	newEvent.m_iY = iY;
	newEvent.m_iData1 = iData1;
	newEvent.m_iData2 = iData2;
	newEvent.m_iData3 = iData3;
	newEvent.m_iData4 = iData4;

	if (IsLogFull())
	{
		RemoveOldestEvent();
	}

	m_aEvents[m_iEventsEndIndex] = newEvent;

	IncrementEndIndex();

	m_iCurrentLookupIndex++;

	return true;
}

int CvEventLog::GetNumEvents(void) const
{
	if (m_iEventsEndIndex >= m_iEventsBeginIndex)
	{
		int iValue = m_iEventsEndIndex - m_iEventsBeginIndex;
		return iValue;
	}

	int iValue = (m_aEvents.size() - m_iEventsBeginIndex) + m_iEventsEndIndex;
	return iValue;
}

int CvEventLog::GetTurn(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return m_aEvents[iRealIndex].m_iTurn;
}

CvString CvEventLog::GetMessage(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return m_aEvents[iRealIndex].m_strMessage;
}

PlayerTypes CvEventLog::GetOtherPlayer(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return m_aEvents[iRealIndex].m_eOtherPlayer;
}

CvPlot* CvEventLog::GetPlot(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return GC.getMap().plot(m_aEvents[iRealIndex].m_iX, m_aEvents[iRealIndex].m_iY);
}

int CvEventLog::GetData1(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return m_aEvents[iRealIndex].m_iData1;
}

int CvEventLog::GetData2(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return m_aEvents[iRealIndex].m_iData2;
}

int CvEventLog::GetData3(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return m_aEvents[iRealIndex].m_iData3;
}

int CvEventLog::GetData4(int iZeroBasedIndex) const
{
	int iRealIndex = (m_iEventsBeginIndex + iZeroBasedIndex) % m_aEvents.size();
	return m_aEvents[iRealIndex].m_iData4;
}

//	---------------------------------------------------------------------------
bool CvEventLog::IsLogFull()
{
	int iAdjustedEndIndex = m_iEventsEndIndex + 1;
	if (iAdjustedEndIndex >= (int)m_aEvents.size())
	{
		iAdjustedEndIndex = 0;
	}

	if (iAdjustedEndIndex == m_iEventsBeginIndex)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CvEventLog::RemoveOldestEvent()
{
	m_aEvents[m_iEventsBeginIndex].Clear();
	IncrementBeginIndex();
}

void CvEventLog::IncrementBeginIndex()
{
	m_iEventsBeginIndex++;
	if (m_iEventsBeginIndex >= (int)m_aEvents.size())
	{
		m_iEventsBeginIndex = 0;
	}
}

void CvEventLog::IncrementEndIndex()
{
	m_iEventsEndIndex++;
	if (m_iEventsEndIndex >= (int)m_aEvents.size())
	{
		m_iEventsEndIndex = 0;
	}
}
