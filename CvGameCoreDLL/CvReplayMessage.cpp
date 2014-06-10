/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include ".\cvreplaymessage.h"
#include "CvEnumSerialization.h"

// include this after all other headers!
#include "LintFree.h"

//------------------------------------------------------------------------------
unsigned int CvReplayMessage::Version()
{
	return 2;
}
//------------------------------------------------------------------------------
CvReplayMessage::CvReplayMessage()
: m_iTurn(-1)
, m_eType(NO_REPLAY_MESSAGE)
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
void CvReplayMessage::read(FDataStream& kStream, unsigned int uiVersion)
{
	// Version number to maintain backwards compatibility
	if(uiVersion < 2)
	{
		uint uiDummyVersion;
		kStream >> uiDummyVersion;
	}

	kStream >> m_iTurn;
	kStream >> m_eType;

	if(uiVersion < 2)
	{
		int iPlotX = -1;
		int iPlotY = -1;
		kStream >> iPlotX;
		kStream >> iPlotY;
		if(iPlotX != -1 || iPlotY != -1)
		{
			m_Plots.reserve(1);
			m_Plots.push_back(PlotPosition((short)iPlotX, (short)iPlotY));
		}
	}
	else
	{
		int nPlots = -1;
		kStream >> nPlots;
		if(nPlots > 0)
		{
			m_Plots.reserve(nPlots);
			for(int i = 0; i < nPlots; ++i)
			{
				short sPlotX, sPlotY;
				kStream >> sPlotX;
				kStream >> sPlotY;
				m_Plots.push_back(PlotPosition(sPlotX, sPlotY));
			}
		}
	}

	kStream >> m_ePlayer;
	kStream >> m_strText;

	if(uiVersion < 2)
	{
		int iDummy;	//Was color
		kStream >>  iDummy;
	}
}
//------------------------------------------------------------------------------
void CvReplayMessage::write(FDataStream& kStream) const
{
	kStream << m_iTurn;
	kStream << m_eType;

	kStream << (int)m_Plots.size();
	for(PlotPositionList::const_iterator it = m_Plots.begin(); it != m_Plots.end(); ++it)
	{
		kStream << (*it).first;
		kStream << (*it).second;
	}

	kStream << m_ePlayer;
	kStream << m_strText;
}
//------------------------------------------------------------------------------
