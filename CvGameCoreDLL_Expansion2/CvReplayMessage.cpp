/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include ".\CvReplayMessage.h"
#include "CvEnumSerialization.h"

// include this after all other headers!
#include "LintFree.h"
//------------------------------------------------------------------------------
CvReplayMessage::CvReplayMessage()
	: m_iTurn(-1)
	, m_eType(NO_REPLAY_MESSAGE), m_Plots()
	, m_ePlayer(NO_PLAYER)
{
}
//------------------------------------------------------------------------------
CvReplayMessage::CvReplayMessage(int iTurn, ReplayMessageTypes eType, PlayerTypes ePlayer) :
	m_iTurn(iTurn),
	m_ePlayer(ePlayer),
	m_eType(eType)
{
}
//------------------------------------------------------------------------------
CvReplayMessage::~CvReplayMessage()
{
}
//------------------------------------------------------------------------------
void CvReplayMessage::setTurn(int iTurn)
{
	m_iTurn = iTurn;
}
//------------------------------------------------------------------------------
int CvReplayMessage::getTurn() const
{
	return m_iTurn;
}
//------------------------------------------------------------------------------
void CvReplayMessage::setType(ReplayMessageTypes eType)
{
	m_eType = eType;
}
//------------------------------------------------------------------------------
ReplayMessageTypes CvReplayMessage::getType() const
{
	return m_eType;
}
//------------------------------------------------------------------------------
void CvReplayMessage::setPlayer(PlayerTypes ePlayer)
{
	m_ePlayer = ePlayer;
}
//------------------------------------------------------------------------------
PlayerTypes CvReplayMessage::getPlayer() const
{
	return m_ePlayer;
}
//------------------------------------------------------------------------------
void CvReplayMessage::setText(const CvString& strText)
{
	m_strText = strText;
}
//------------------------------------------------------------------------------
const CvString& CvReplayMessage::getText() const
{
	return m_strText;
}
//------------------------------------------------------------------------------
void CvReplayMessage::addPlot(int iPlotX, int iPlotY)
{
	short sPlotX = (short)iPlotX;
	short sPlotY = (short)iPlotY;

	for(PlotPositionList::iterator it = m_Plots.begin(); it != m_Plots.end(); ++it)
	{
		const PlotPosition& position = (*it);
		if(position.first == sPlotX && position.second == sPlotY)
			return;
	}

	m_Plots.push_back(PlotPosition(sPlotX, sPlotY));
}
//------------------------------------------------------------------------------
bool CvReplayMessage::getPlot(unsigned int idx, int& iPlotX, int& iPlotY) const
{
	if(idx < m_Plots.size())
	{
		const PlotPosition& position = m_Plots[idx];
		iPlotX = (int)position.first;
		iPlotY = (int)position.second;
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------
unsigned int CvReplayMessage::getNumPlots() const
{
	return m_Plots.size();
}
//------------------------------------------------------------------------------
void CvReplayMessage::clearPlots()
{
	m_Plots.clear();
}
//------------------------------------------------------------------------------
template<typename ReplayMessage, typename Visitor>
void CvReplayMessage::Serialize(ReplayMessage& replayMessage, Visitor& visitor)
{
	visitor(replayMessage.m_iTurn);
	visitor(replayMessage.m_eType);
	visitor(replayMessage.m_Plots);
	visitor(replayMessage.m_ePlayer);
	visitor(replayMessage.m_strText);
}
//------------------------------------------------------------------------------
void CvReplayMessage::read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}
//------------------------------------------------------------------------------
void CvReplayMessage::write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}
//------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvReplayMessage& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}
//------------------------------------------------------------------------------
FDataStream& operator>>(FDataStream& loadFrom, CvReplayMessage& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}