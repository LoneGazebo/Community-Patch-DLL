#include "CvGameCoreDLLPCH.h"
#include "CvEventLog.h"

// Include this after all other headers.
#include "LintFree.h"

#define MAX_EVENTS 50

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
CvEventLog::CvEventLog(void) : m_aEvents(), m_iEventsBeginIndex(), m_iEventsEndIndex(), m_iCurrentLookupIndex()
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
