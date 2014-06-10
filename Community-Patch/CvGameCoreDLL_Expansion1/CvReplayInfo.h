/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#ifndef CvReplayInfo_H
#define CvReplayInfo_H

#pragma once

#include "CvEnums.h"

class CvReplayMessage;


class CvReplayInfo
{
public:
	CvReplayInfo();
	virtual ~CvReplayInfo();

	void createInfo();

	struct PlotState
	{
		PlotTypes m_ePlotType;
		TerrainTypes m_eTerrain;
		FeatureTypes m_eFeature;
		bool m_bNEOfRiver;
		bool m_bWOfRiver;
		bool m_bNWOfRiver;
	};

	int getActivePlayer() const;
	const CvString& getMapScriptName() const;
	WorldSizeTypes getWorldSize() const;
	ClimateTypes getClimate() const;
	SeaLevelTypes getSeaLevel() const;
	EraTypes getEra() const;
	GameSpeedTypes getGameSpeed() const;
	bool isGameOption(GameOptionTypes eOption) const;
	bool isVictoryCondition(VictoryTypes eVictory) const;
	VictoryTypes getVictoryType() const;
	GameTypes getGameType() const;

	void addReplayMessage(const CvReplayMessage& kMessage);
	void clearReplayMessageMap();
	uint getNumReplayMessages() const;
	const CvReplayMessage* getReplayMessage(uint i) const;

	int getInitialTurn() const;
	int getFinalTurn() const;
	int getStartYear() const;
	const char* getFinalDate() const;
	CalendarTypes getCalendar() const;
	int getNumPlayers() const;

	CivilizationTypes getPlayerCivilization(int iPlayer) const;
	LeaderHeadTypes getPlayerLeader(int iPlayer) const;
	PlayerColorTypes getPlayerColor(int iPlayer) const;
	HandicapTypes getPlayerDifficulty(int iPlayer) const;
	const char* getPlayerLeaderName(int iPlayer) const;
	const char* getPlayerCivDescription(int iPlayer) const;
	const char* getPlayerShortCivDescription(int iPlayer) const;
	const char* getPlayerCivAdjective(int iPlayer) const;

	unsigned int getNumPlayerDataSets() const;
	const char* getPlayerDataSetName(unsigned int idx) const;
	bool getPlayerDataSetValue(unsigned int uiPlayer, unsigned int uiDataSet, unsigned int uiTurn, int& value) const;

	int getNormalizedScore() const;

	int getMapHeight() const;
	int getMapWidth() const;

	bool getPlotState(unsigned int x, unsigned int y, unsigned int uiTurn, PlotState& plotState) const;

	bool read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

protected:
	bool isValidPlayer(int i) const;
	bool isValidTurn(int i) const;

	static int REPLAY_VERSION;

	int m_iActivePlayer;

	CvString m_strMapScriptName;
	WorldSizeTypes m_eWorldSize;
	ClimateTypes m_eClimate;
	SeaLevelTypes m_eSeaLevel;
	EraTypes m_eEra;
	GameSpeedTypes m_eGameSpeed;
	std::vector<GameOptionTypes> m_listGameOptions;
	std::vector<VictoryTypes> m_listVictoryTypes;
	VictoryTypes m_eVictoryType;
	GameTypes m_eGameType;

	int m_iInitialTurn;
	int m_iFinalTurn;
	int m_iStartYear;
	CvString m_strFinalDate;
	CalendarTypes m_eCalendar;
	int m_iNormalizedScore;

	struct PlayerInfo
	{
		CivilizationTypes m_eCivilization;
		LeaderHeadTypes m_eLeader;
		PlayerColorTypes m_ePlayerColor;
		HandicapTypes m_eDifficulty;
		CvString m_strCustomLeaderName;
		CvString m_strCustomCivilizationName;
		CvString m_strCustomCivilizationShortName;
		CvString m_strCustomCivilizationAdjective;
	};

	typedef std::vector<PlayerInfo> PlayerInfoList;
	PlayerInfoList m_listPlayerInfo;

	typedef std::vector<CvReplayMessage> ReplayMessageList;
	ReplayMessageList m_listReplayMessages;

	typedef std::vector<std::string> DataSetMap;
	DataSetMap m_dataSetMap;

	typedef std::map<unsigned int, int> TurnData;			//Turn#, value
	typedef std::vector<TurnData> TurnDataSets;				//index is DataSet Index resolved by DataSetMap

	typedef std::vector<TurnDataSets> PlayerTurnDataSets;	//index is Player
	PlayerTurnDataSets m_listPlayerDataSets;

	int m_iMapHeight;
	int m_iMapWidth;

	struct OldPlotState
	{
		TerrainTypes m_eTerrain;
		FeatureTypes m_eFeature;
		bool m_bNEOfRiver;
		bool m_bWOfRiver;
		bool m_bNWOfRiver;
	};

	typedef std::map<unsigned int, PlotState> PlotStatePerTurn;

	typedef std::vector<PlotStatePerTurn> ListOfPlots;

	ListOfPlots m_listPlots;

	// serialization support
	friend FDataStream& operator<<(FDataStream& saveTo, const CvReplayInfo::PlayerInfo& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvReplayInfo::PlayerInfo& writeTo);
	friend FDataStream& operator<<(FDataStream& saveTo, const CvReplayInfo::TurnData& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvReplayInfo::TurnData& writeTo);
	friend FDataStream& operator<<(FDataStream& saveTo, const CvReplayInfo::OldPlotState& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvReplayInfo::OldPlotState& writeTo);
	friend FDataStream& operator<<(FDataStream& saveTo, const CvReplayInfo::PlotState& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvReplayInfo::PlotState& writeTo);
	friend FDataStream& operator<<(FDataStream& saveTo, const CvReplayInfo::TurnData& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvReplayInfo::TurnData& writeTo);
};

#endif