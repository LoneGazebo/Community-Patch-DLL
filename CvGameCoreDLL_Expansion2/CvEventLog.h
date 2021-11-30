#pragma once

#ifndef CIV5_EVENTLOG_H
#define CIV5_EVENTLOG_H

class CvPlayer;

class CvEventLog
{
public:
	CvEventLog(void);
	~CvEventLog(void);

	void Init(PlayerTypes ePlayer);
	void Uninit(void);

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	bool Add(const char* szMessage, PlayerTypes eOtherPlayerID = NO_PLAYER, int iX = -1, int iY = -1, int iData1 = -1, int iData2 = -1, int iData3 = -1, int iData4 = -1);
	
	int GetNumEvents(void) const;
	int GetTurn(int iZeroBasedIndex) const;
	CvString GetMessage(int iZeroBasedIndex) const;
	PlayerTypes GetOtherPlayer(int iZeroBasedIndex) const;
	CvPlot* GetPlot(int iZeroBasedIndex) const;
	int GetData1(int iZeroBasedIndex) const;
	int GetData2(int iZeroBasedIndex) const;
	int GetData3(int iZeroBasedIndex) const;
	int GetData4(int iZeroBasedIndex) const;

	struct EventLogEntry
	{
		void Clear();

		int m_iTurn;
		CvString m_strMessage;
		PlayerTypes m_eOtherPlayer;
		int m_iX;
		int m_iY;
		int m_iData1;
		int m_iData2;
		int m_iData3;
		int m_iData4;
	};

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	bool IsLogFull();
	void RemoveOldestEvent();
	void IncrementBeginIndex();
	void IncrementEndIndex();

	PlayerTypes m_ePlayer;

	std::vector<EventLogEntry> m_aEvents;
	int m_iEventsBeginIndex;
	int m_iEventsEndIndex;

	int m_iCurrentLookupIndex;
};

FDataStream& operator>>(FDataStream&, CvEventLog::EventLogEntry&);
FDataStream& operator<<(FDataStream&, const CvEventLog::EventLogEntry&);

#endif //CIV5_EVENTLOG_H
