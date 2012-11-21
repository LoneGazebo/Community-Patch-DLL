/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvReplayInfo.h"
#include "CvInfos.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvMap.h"
#include "CvReplayMessage.h"
#include "CvGameTextMgr.h"
#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "CvGameCoreUtils.h"

// include this after all other headers!
#include "LintFree.h"

int CvReplayInfo::REPLAY_VERSION = 3;

//Replay Version History
//
// * Version 3
//		Storing per-plot PlotTypes.
//		FeatureTypes are now stored per turn.
//
// * Version 2
//		Replaced fixed score tracking w/ dynamic one based on database
//
// * Version 1
//		First Version

CvReplayInfo::CvReplayInfo() :
	m_iActivePlayer(0),
	m_eWorldSize(NO_WORLDSIZE),
	m_eClimate(NO_CLIMATE),
	m_eSeaLevel(NO_SEALEVEL),
	m_eEra(NO_ERA),
	m_eGameSpeed(NO_GAMESPEED),
	m_iInitialTurn(0),
	m_iFinalTurn(0),
	m_eVictoryType(NO_VICTORY),
	m_iMapHeight(0),
	m_iMapWidth(0),
	m_iNormalizedScore(0),
	m_iStartYear(0)
{
}

CvReplayInfo::~CvReplayInfo()
{
};

void CvReplayInfo::createInfo()
{
	CvGame& game = GC.getGame();

	const PlayerTypes eActivePlayer = game.getActivePlayer();

	m_strMapScriptName = CvPreGame::mapScriptName();
	m_eWorldSize = CvPreGame::worldSize();
	m_eClimate = CvPreGame::climate();
	m_eSeaLevel = CvPreGame::seaLevel();
	m_eEra = CvPreGame::era();
	m_eGameSpeed = game.getGameSpeedType();

	m_listGameOptions.clear();
	for (int i = 0; i < NUM_GAMEOPTION_TYPES; i++)
	{
		GameOptionTypes eOption = (GameOptionTypes)i;
		if (game.isOption(eOption))
		{
			m_listGameOptions.push_back(eOption);
		}
	}

	m_listVictoryTypes.clear();
	for (int i = 0; i < GC.getNumVictoryInfos(); i++)
	{
		VictoryTypes eVictory = (VictoryTypes)i;
		if (game.isVictoryValid(eVictory))
		{
			m_listVictoryTypes.push_back(eVictory);
		}
	}

	m_eVictoryType = NO_VICTORY;
	m_iNormalizedScore = 0;
	if (NO_PLAYER != eActivePlayer)
	{
		CvPlayer& player = GET_PLAYER(eActivePlayer);
		if (game.getWinner() == player.getTeam())
		{
			m_eVictoryType = game.getVictory();
		}

		m_iNormalizedScore = player.GetScore(true, player.getTeam() == game.getWinner());
	}

	//m_eGameType = CvPreGame::getGameType();

	m_iInitialTurn = game.getStartTurn();
	m_iStartYear = game.getStartYear();
	m_iFinalTurn = game.getGameTurn();
	CvGameTextMgr::setYearStr(m_strFinalDate, m_iFinalTurn, false, game.getCalendar(), game.getStartYear(), game.getGameSpeedType());

	m_eCalendar = game.getCalendar();

	m_dataSetMap.clear();
	m_listPlayerDataSets.clear();
	m_listPlayerInfo.clear();
	m_listPlots.clear();

	std::map<int, int> mapPlayers;
	mapPlayers[-1] = -1;	//account for NO_PLAYER.

	m_iActivePlayer = -1;
	int iPlayerIndex = 0;
	for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iPlayer);
		if (player.isEverAlive())
		{
			//Ensure that final turn is stored.
			player.GatherPerTurnReplayStats(m_iFinalTurn);

			mapPlayers[iPlayer] = iPlayerIndex;
			if (iPlayer == game.getActivePlayer())
			{
				m_iActivePlayer = iPlayerIndex;
			}
			++iPlayerIndex;

			PlayerInfo playerInfo;
			playerInfo.m_eLeader = player.getLeaderType();
			playerInfo.m_eCivilization = player.getCivilizationType();

			playerInfo.m_eDifficulty = player.getHandicapType();
			playerInfo.m_strCustomLeaderName = player.getName();
			playerInfo.m_strCustomCivilizationName = player.getCivilizationDescription();
			playerInfo.m_strCustomCivilizationShortName = player.getCivilizationShortDescription();
			playerInfo.m_strCustomCivilizationAdjective = player.getCivilizationAdjective();
			playerInfo.m_ePlayerColor = player.getPlayerColor();


			TurnDataSets dataSet(m_dataSetMap.size());

			unsigned int uiNumDataSets = player.getNumReplayDataSets();
			for(unsigned int uiPlayerDataSet = 0; uiPlayerDataSet < uiNumDataSets; ++uiPlayerDataSet)
			{
				// First, Locate the index of the dataset
				std::string playerDataSetName = player.getReplayDataSetName(uiPlayerDataSet);
				unsigned int uiDataSet = 0;
				bool bFound = false;
				for(uiDataSet = 0; uiDataSet < m_dataSetMap.size(); ++uiDataSet)
				{
					const std::string& dataSetName = m_dataSetMap[uiDataSet];
					if(dataSetName == playerDataSetName)
					{
						bFound = true;
						break;
					}
				}

				// Add a new index if one doesn't exist.
				if(!bFound)
				{
					m_dataSetMap.push_back(playerDataSetName);
					dataSet.push_back(TurnData());
					uiDataSet = m_dataSetMap.size() - 1;
				}

				CvPlayer::TurnData playerData = player.getReplayDataHistory(uiPlayerDataSet);
				TurnData turnData;

				for(CvPlayer::TurnData::iterator it = playerData.begin(); it != playerData.end(); ++it)
				{
					turnData[(*it).first - m_iInitialTurn] = (*it).second;
				}

				dataSet[uiDataSet] = turnData;
			}

			m_listPlayerDataSets.push_back(dataSet);
			m_listPlayerInfo.push_back(playerInfo);
		}
	}

	m_listReplayMessages.clear();
	const uint nMessages = game.getNumReplayMessages();
	m_listReplayMessages.reserve(nMessages);

	for (uint i = 0; i < nMessages; ++i)
	{
		const CvReplayMessage* pMessage = game.getReplayMessage(i);
		if(pMessage)
		{
			CvReplayMessage modifiedMessage = (*pMessage);
			modifiedMessage.setPlayer((PlayerTypes)mapPlayers[modifiedMessage.getPlayer()]);

			m_listReplayMessages.push_back(modifiedMessage);
		}
	}

	CvMap& kMap = GC.getMap();

	m_iMapWidth = kMap.getGridWidth();
	m_iMapHeight = kMap.getGridHeight();

	int numPlots = kMap.numPlots();
	m_listPlots.clear();
	m_listPlots.reserve(numPlots);

	for(int i = 0; i < numPlots; i++)
	{
		PlotState plotState;

		CvPlot* pkPlot = kMap.plotByIndexUnchecked(i);
		plotState.m_ePlotType = pkPlot->getPlotType();
		plotState.m_eTerrain = pkPlot->getTerrainType();
		plotState.m_eFeature = pkPlot->getFeatureType();
		plotState.m_bNEOfRiver = pkPlot->isNEOfRiver();
		plotState.m_bWOfRiver = pkPlot->isWOfRiver();
		plotState.m_bNWOfRiver = pkPlot->isNWOfRiver();

		PlotStatePerTurn plotStatePerTurn;
		plotStatePerTurn[m_iFinalTurn] = plotState;

		m_listPlots.push_back(plotStatePerTurn);
	}
}

int CvReplayInfo::getNumPlayers() const
{
	return (int)m_listPlayerInfo.size();
}


bool CvReplayInfo::isValidPlayer(int i) const
{
	return (i >= 0 && i < (int)m_listPlayerInfo.size());
}

bool CvReplayInfo::isValidTurn(int i) const
{
	return (i >= m_iInitialTurn && i <= m_iFinalTurn);
}

int CvReplayInfo::getActivePlayer() const
{
	return m_iActivePlayer;
}

CivilizationTypes CvReplayInfo::getPlayerCivilization(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_eCivilization;
	}

	return NO_CIVILIZATION;
}

LeaderHeadTypes CvReplayInfo::getPlayerLeader(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_eLeader;
	}

	return NO_LEADER;
}

PlayerColorTypes CvReplayInfo::getPlayerColor(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_ePlayerColor;
	}

	return NO_PLAYERCOLOR;
}

HandicapTypes CvReplayInfo::getPlayerDifficulty(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_eDifficulty;
	}

	return NO_HANDICAP;
}

const char* CvReplayInfo::getPlayerLeaderName(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_strCustomLeaderName;
	}

	return NULL;
}

const char* CvReplayInfo::getPlayerCivDescription(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_strCustomCivilizationName;
	}

	return NULL;
}

const char* CvReplayInfo::getPlayerShortCivDescription(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_strCustomCivilizationShortName;
	}

	return NULL;
}

const char* CvReplayInfo::getPlayerCivAdjective(int iPlayer) const
{
	if(iPlayer >= 0 && iPlayer < (int)m_listPlayerInfo.size())
	{
		return m_listPlayerInfo[iPlayer].m_strCustomCivilizationAdjective;
	}

	return NULL;
}

const CvString& CvReplayInfo::getMapScriptName() const
{
	return m_strMapScriptName;
}

WorldSizeTypes CvReplayInfo::getWorldSize() const
{
	return m_eWorldSize;
}

ClimateTypes CvReplayInfo::getClimate() const
{
	return m_eClimate;
}

SeaLevelTypes CvReplayInfo::getSeaLevel() const
{
	return m_eSeaLevel;
}

EraTypes CvReplayInfo::getEra() const
{
	return m_eEra;
}

GameSpeedTypes CvReplayInfo::getGameSpeed() const
{
	return m_eGameSpeed;
}

bool CvReplayInfo::isGameOption(GameOptionTypes eOption) const
{
	for (uint i = 0; i < m_listGameOptions.size(); i++)
	{
		if (m_listGameOptions[i] == eOption)
		{
			return true;
		}
	}
	return false;
}

bool CvReplayInfo::isVictoryCondition(VictoryTypes eVictory) const
{
	for (uint i = 0; i < m_listVictoryTypes.size(); i++)
	{
		if (m_listVictoryTypes[i] == eVictory)
		{
			return true;
		}
	}
	return false;
}

VictoryTypes CvReplayInfo::getVictoryType() const
{
	return m_eVictoryType;
}

GameTypes CvReplayInfo::getGameType() const
{
	return m_eGameType;
}

void CvReplayInfo::addReplayMessage(const CvReplayMessage& pMessage)
{
	m_listReplayMessages.push_back(pMessage);
}

void CvReplayInfo::clearReplayMessageMap()
{
	m_listReplayMessages.clear();
}

uint CvReplayInfo::getNumReplayMessages() const
{
	return m_listReplayMessages.size();
}

const CvReplayMessage* CvReplayInfo::getReplayMessage(uint i) const
{
	if(i < m_listReplayMessages.size())
	{
		return &(m_listReplayMessages[i]);
	}

	return NULL;
}

int CvReplayInfo::getInitialTurn() const
{
	return m_iInitialTurn;
}

int CvReplayInfo::getStartYear() const
{
	return m_iStartYear;
}

int CvReplayInfo::getFinalTurn() const
{
	return m_iFinalTurn;
}

const char* CvReplayInfo::getFinalDate() const
{
	return m_strFinalDate;
}

CalendarTypes CvReplayInfo::getCalendar() const
{
	return m_eCalendar;
}

unsigned int CvReplayInfo::getNumPlayerDataSets() const
{
	return m_dataSetMap.size();
}

const char* CvReplayInfo::getPlayerDataSetName(unsigned int idx) const
{
	if(idx < m_dataSetMap.size())
	{
		return m_dataSetMap[idx].c_str();
	}

	return NULL;
}

bool CvReplayInfo::getPlayerDataSetValue(unsigned int uiPlayer, unsigned int uiDataSet, unsigned int iTurn, int& outValue) const
{
	const unsigned int uiAdjustedTurn = iTurn - m_iInitialTurn;
	if(uiPlayer < m_listPlayerInfo.size() && uiDataSet < m_dataSetMap.size())
	{
		const TurnData& data = m_listPlayerDataSets[uiPlayer][uiDataSet];
		TurnData::const_iterator it = data.find(uiAdjustedTurn);
		if(it != data.end())
		{
			outValue = (*it).second;
			return true;
		}
	}

	return false;
}

int CvReplayInfo::getNormalizedScore() const
{
	return m_iNormalizedScore;
}

int CvReplayInfo::getMapHeight() const
{
	return m_iMapHeight;
}

int CvReplayInfo::getMapWidth() const
{
	return m_iMapWidth;
}

bool CvReplayInfo::getPlotState(unsigned int x, unsigned int y, unsigned int uiTurn, PlotState& plotState) const
{
	size_t idx = x + y * m_iMapWidth;
	if(idx < m_listPlots.size())
	{
		const PlotStatePerTurn& plotStatePerTurn = m_listPlots[idx];
		PlotStatePerTurn::const_iterator match = plotStatePerTurn.find(uiTurn);
		if(match != plotStatePerTurn.end())
		{
			plotState = (*match).second;
			return true;
		}
	}

	return false;
}

bool CvReplayInfo::read(FDataStream& kStream)
{
	bool bSuccess = true;

	int iVersion;
	kStream >> iVersion;
	if (iVersion < 2)
	{
		return false;
	}

	kStream >> m_iActivePlayer;
	kStream >> m_strMapScriptName;
	kStream >> m_eWorldSize;
	kStream >> m_eClimate;
	kStream >> m_eSeaLevel;
	kStream >> m_eEra;
	kStream >> m_eGameSpeed;
	kStream >> m_listGameOptions;
	kStream >> m_listVictoryTypes;
	kStream >> m_eVictoryType;
	kStream >> m_eGameType;
	kStream >> m_iInitialTurn;
	kStream >> m_iStartYear;
	kStream >> m_iFinalTurn;
	kStream >> m_strFinalDate;
	kStream >> m_eCalendar;
	kStream >> m_iNormalizedScore;

	kStream >> m_listPlayerInfo;

	kStream >> m_dataSetMap;
	kStream >> m_listPlayerDataSets;

	unsigned int uiReplayMessageVersion = 2;
	kStream >> uiReplayMessageVersion;

	unsigned int uiReplayMessageCount = 0;
	kStream >> uiReplayMessageCount;

	m_listReplayMessages.clear();
	m_listReplayMessages.reserve(uiReplayMessageCount);
	for(unsigned int ui = 0; ui < uiReplayMessageCount; ++ui)
	{
		CvReplayMessage message;
		message.read(kStream, uiReplayMessageVersion);
		m_listReplayMessages.push_back(message);
	}

	kStream >> m_iMapWidth;
	kStream >> m_iMapHeight;
	if(iVersion <= 2)
	{
		struct PlotInfo
		{
			TerrainTypes m_eTerrain;
			FeatureTypes m_eFeature;
			bool m_bNEOfRiver;
			bool m_bWOfRiver;
			bool m_bNWOfRiver;
		};

		std::vector<OldPlotState> plotList;
		kStream >> plotList;

		m_listPlots.reserve(plotList.size());
		for(std::vector<OldPlotState>::iterator it = plotList.begin(); it != plotList.end(); ++it)
		{
			const OldPlotState& oldPlotState = (*it);

			PlotState plotState;
			plotState.m_ePlotType = PLOT_LAND;
			plotState.m_eTerrain  = oldPlotState.m_eTerrain;
			plotState.m_eFeature  = oldPlotState.m_eFeature;
			plotState.m_bNEOfRiver = oldPlotState.m_bNEOfRiver;
			plotState.m_bNWOfRiver = oldPlotState.m_bNWOfRiver;
			plotState.m_bWOfRiver = oldPlotState.m_bWOfRiver;

			PlotStatePerTurn plotStatePerTurn;
			plotStatePerTurn[m_iInitialTurn] = plotState;
			m_listPlots.push_back(plotStatePerTurn);
		}
	}
	else
	{
		kStream >> m_listPlots;
	}


	return bSuccess;
}

void CvReplayInfo::write(FDataStream& kStream) const
{
	kStream << REPLAY_VERSION;
	kStream << m_iActivePlayer;
	kStream << m_strMapScriptName;
	kStream << m_eWorldSize;
	kStream << m_eClimate;
	kStream << m_eSeaLevel;
	kStream << m_eEra;
	kStream << m_eGameSpeed;
	kStream << m_listGameOptions;
	kStream << m_listVictoryTypes;
	kStream << m_eVictoryType;
	kStream << m_eGameType;
	kStream << m_iInitialTurn;
	kStream << m_iStartYear;
	kStream << m_iFinalTurn;
	kStream << m_strFinalDate;
	kStream << m_eCalendar;
	kStream << m_iNormalizedScore;

	kStream << m_listPlayerInfo;
	kStream << m_dataSetMap;
	kStream << m_listPlayerDataSets;

	kStream << CvReplayMessage::Version();
	kStream << m_listReplayMessages.size();
	for(ReplayMessageList::const_iterator it = m_listReplayMessages.begin(); it != m_listReplayMessages.end(); ++it)
	{
		(*it).write(kStream);
	}

	kStream << m_iMapWidth;
	kStream << m_iMapHeight;
	kStream << m_listPlots;
}

FDataStream & operator<<(FDataStream & saveTo, const CvReplayInfo::PlayerInfo & readFrom)
{
	saveTo << readFrom.m_eCivilization;
	saveTo << readFrom.m_eLeader;
	saveTo << readFrom.m_ePlayerColor;
	saveTo << readFrom.m_eDifficulty;
	saveTo << readFrom.m_strCustomLeaderName;
	saveTo << readFrom.m_strCustomCivilizationName;
	saveTo << readFrom.m_strCustomCivilizationShortName;
	saveTo << readFrom.m_strCustomCivilizationAdjective;

	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, CvReplayInfo::PlayerInfo & writeTo)
{
	loadFrom >> writeTo.m_eCivilization;
	loadFrom >> writeTo.m_eLeader;
	loadFrom >> writeTo.m_ePlayerColor;
	loadFrom >> writeTo.m_eDifficulty;
	loadFrom >> writeTo.m_strCustomLeaderName;
	loadFrom >> writeTo.m_strCustomCivilizationName;
	loadFrom >> writeTo.m_strCustomCivilizationShortName;
	loadFrom >> writeTo.m_strCustomCivilizationAdjective;

	return loadFrom;
}

FDataStream & operator<<(FDataStream & saveTo, const CvReplayInfo::OldPlotState & readFrom)
{
	saveTo << (unsigned char)readFrom.m_eTerrain;
	saveTo << (unsigned char)readFrom.m_eFeature;

	unsigned char uiRiverData = 0;
	uiRiverData |= (readFrom.m_bNWOfRiver) << 1;
	uiRiverData |= (readFrom.m_bWOfRiver) << 2;
	uiRiverData |= (readFrom.m_bNEOfRiver) << 3;

	saveTo << uiRiverData;

	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, CvReplayInfo::OldPlotState & writeTo)
{
	unsigned char ucTempVal;
	loadFrom >> ucTempVal;
	writeTo.m_eTerrain = (TerrainTypes)ucTempVal;

	loadFrom >> ucTempVal;
	writeTo.m_eFeature = (FeatureTypes)ucTempVal;

	loadFrom >> ucTempVal;
	writeTo.m_bNWOfRiver = (ucTempVal & (1 << 1)) != 0;
	writeTo.m_bWOfRiver = (ucTempVal & (1 << 2)) != 0;
	writeTo.m_bNEOfRiver = (ucTempVal & (1 << 3)) != 0;

	return loadFrom;
}

FDataStream & operator<<(FDataStream & saveTo, const CvReplayInfo::PlotState & readFrom)
{
	saveTo << (unsigned char)readFrom.m_ePlotType;
	saveTo << (unsigned char)readFrom.m_eTerrain;
	saveTo << (unsigned char)readFrom.m_eFeature;

	unsigned char uiRiverData = 0;
	uiRiverData |= (readFrom.m_bNWOfRiver) << 1;
	uiRiverData |= (readFrom.m_bWOfRiver) << 2;
	uiRiverData |= (readFrom.m_bNEOfRiver) << 3;

	saveTo << uiRiverData;

	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, CvReplayInfo::PlotState & writeTo)
{
	unsigned char ucTempVal;

	loadFrom >> ucTempVal;
	writeTo.m_ePlotType = (PlotTypes)ucTempVal;

	loadFrom >> ucTempVal;
	writeTo.m_eTerrain = (TerrainTypes)ucTempVal;

	loadFrom >> ucTempVal;
	writeTo.m_eFeature = (FeatureTypes)ucTempVal;

	loadFrom >> ucTempVal;
	writeTo.m_bNWOfRiver = (ucTempVal & (1 << 1)) != 0;
	writeTo.m_bWOfRiver = (ucTempVal & (1 << 2)) != 0;
	writeTo.m_bNEOfRiver = (ucTempVal & (1 << 3)) != 0;

	return loadFrom;
}


FDataStream & operator<<(FDataStream & saveTo, const CvReplayInfo::TurnData & readFrom)
{
	const size_t count = readFrom.size();
	saveTo << count;
	for(CvReplayInfo::TurnData::const_iterator it = readFrom.begin(); it != readFrom.end(); ++it)
	{
		saveTo << (*it).first;
		saveTo << (*it).second;
	}
	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, CvReplayInfo::TurnData & writeTo)
{
	size_t uiSize;
	loadFrom >> uiSize;

	unsigned int uiTurn;
	int iValue;

	for(unsigned int i = 0; i < uiSize; ++i)
	{
		loadFrom >> uiTurn;
		loadFrom >> iValue;
		writeTo[uiTurn] = iValue;
	}

	return loadFrom;
}