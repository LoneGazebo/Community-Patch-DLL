/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#ifndef CVREPLAYMESSAGE_H
#define CVREPLAYMESSAGE_H

class CvReplayMessage
{
public:
	CvReplayMessage();
	CvReplayMessage(int iTurn, ReplayMessageTypes eType = NO_REPLAY_MESSAGE, PlayerTypes ePlayer = NO_PLAYER);
	virtual ~CvReplayMessage();

	// Accessors
	void setTurn(int iTurn);
	int getTurn() const;
	void setType(ReplayMessageTypes eType);
	ReplayMessageTypes getType() const;

	void setPlayer(PlayerTypes ePlayer);
	PlayerTypes getPlayer() const;
	void setText(const CvString& strText);
	const CvString& getText() const;

	void addPlot(int iPlotX, int iPlotY);
	bool getPlot(unsigned int idx, int& iPlotX, int& iPlotY) const;
	unsigned int getNumPlots() const;
	void clearPlots();

	template<typename ReplayMessage, typename Visitor>
	static void Serialize(ReplayMessage& replayMessage, Visitor& visitor);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

private:
	int m_iTurn;
	ReplayMessageTypes m_eType;

	typedef std::pair<short, short> PlotPosition;
	typedef std::vector<PlotPosition> PlotPositionList;

	PlotPositionList m_Plots;
	PlayerTypes m_ePlayer;
	CvString m_strText;
};

FDataStream& operator<<(FDataStream&, const CvReplayMessage&);
FDataStream& operator>>(FDataStream&, CvReplayMessage&);

#endif