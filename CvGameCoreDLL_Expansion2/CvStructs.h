/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CVSTRUCTS_H
#define CVSTRUCTS_H

struct GameTurnInfo
{
	int iMonthIncrement;
	int iNumGameTurnsPerIncrement;
};

struct OrderData
{
	OrderData()
		: eOrderType(NO_ORDER)
		, iData1(0)
		, iData2(0)
		, bSave(false)
		, bRush(false)
	{
	}

	OrderTypes eOrderType;
	int iData1;
	int iData2;
	bool bSave;
	bool bRush;
};

//FDataStream & operator<<(FDataStream &, const OrderData &);
//FDataStream & operator>>(FDataStream &, OrderData &);

struct MissionData
{
	MissionData()
		: eMissionType(NO_MISSION)
		, iData1(0)
		, iData2(0)
		, iFlags(0)
		, iPushTurn(0)
	{
	}

	MissionTypes eMissionType;
	int iData1;
	int iData2;
	int iFlags;
	int iPushTurn;
};

//FDataStream & operator<<(FDataStream &, const MissionData &);
//FDataStream & operator>>(FDataStream &, MissionData &);

struct VoteSelectionSubData
{
	VoteSelectionSubData()
		: eVote(NO_VOTE)
		, ePlayer(NO_PLAYER)
		, iCityId(-1)
		, eOtherPlayer(NO_PLAYER)
		, strText("")
	{
	}

	VoteTypes eVote;
	PlayerTypes ePlayer;
	int iCityId;
	PlayerTypes eOtherPlayer;
	CvString strText;
};

struct VoteSelectionData
{
	VoteSelectionData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
		, aVoteOptions()
	{
	}

	int iId;
	VoteSourceTypes eVoteSource;
	std::vector<VoteSelectionSubData> aVoteOptions;

	int  GetID() const;
	void SetID(int iID);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;
};

FDataStream& operator<<(FDataStream&, const VoteSelectionData&);
FDataStream& operator>>(FDataStream&, VoteSelectionData&);

struct VoteTriggeredData
{
	VoteTriggeredData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
		, kVoteOption()
	{
	}

	int iId;
	VoteSourceTypes eVoteSource;
	VoteSelectionSubData kVoteOption;

	int  GetID() const;
	void SetID(int iID);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;
};

FDataStream& operator<<(FDataStream&, const VoteTriggeredData&);
FDataStream& operator>>(FDataStream&, VoteTriggeredData&);

struct PlotExtraYield
{
	PlotExtraYield()
		: m_iX(-1)
		, m_iY(-1)
		, m_aeExtraYield()
	{
	}

	int m_iX;
	int m_iY;
	std::vector<int> m_aeExtraYield;
};
FDataStream& operator<<(FDataStream&, const PlotExtraYield&);
FDataStream& operator>>(FDataStream&, PlotExtraYield&);

struct PlotExtraCost
{
	PlotExtraCost()
		: m_iX(-1)
		, m_iY(-1)
		, m_iCost(0)
	{
	}

	int m_iX;
	int m_iY;
	int m_iCost;
};
FDataStream& operator<<(FDataStream&, const PlotExtraCost&);
FDataStream& operator>>(FDataStream&, PlotExtraCost&);

struct BuildingYieldChange
{
	BuildingYieldChange()
		: eBuildingClass(NO_BUILDINGCLASS)
		, eYield(NO_YIELD)
		, iChange(0)
	{
	}

	BuildingClassTypes eBuildingClass;
	YieldTypes eYield;
	int iChange;
};

FDataStream& operator<<(FDataStream&, const BuildingYieldChange&);
FDataStream& operator>>(FDataStream&, BuildingYieldChange&);

struct BuildingGreatWork
{
	BuildingGreatWork()
		: eBuildingClass(NO_BUILDINGCLASS)
		, iSlot(-1)
		, iGreatWorkIndex(-1)
	{
	}

	BuildingClassTypes eBuildingClass;
	int iSlot;
	int iGreatWorkIndex;
};

FDataStream& operator<<(FDataStream&, const BuildingGreatWork&);
FDataStream& operator>>(FDataStream&, BuildingGreatWork&);

// PITBOSS related structures
//struct PBGameSetupData
//{
//	PBGameSetupData();
//
//	int iSize;
//	int iClimate;
//	int iSeaLevel;
//	int iSpeed;
//	int iEra;
//
//	int iMaxTurns;
//	int iCityElimination;
//	int iAdvancedStartPoints;
//	int iTurnTime;
//
//	int iNumCustomMapOptions;
//	int * aiCustomMapOptions;
//	int getCustomMapOption(int iOption) {return aiCustomMapOptions[iOption];}
//
//	int iNumVictories;
//	bool * abVictories;
//	bool getVictory(int iVictory) {return abVictories[iVictory];}
//
//	std::string szMapName;
//	std::string getMapName() {return szMapName;}
//
//	std::vector<bool> abOptions;
//	bool getOptionAt(int iOption) {return abOptions[iOption];}
//
//	std::vector<bool> abMPOptions;
//	bool getMPOptionAt(int iOption) {return abMPOptions[iOption];}
//};
//
//struct PBPlayerSetupData
//{
//	PBPlayerSetupData() :
//	iWho(0)
//	, iCiv(0)
//	, iLeader(0)
//	, iTeam(0)
//	, iDifficulty(0)
//	{
//	}
//
//	int iWho;
//	int iCiv;
//	int iLeader;
//	int iTeam;
//	int iDifficulty;
//
//	std::string szStatusText;
//	std::string getStatusText() {return szStatusText;}
//};
//
//struct PBPlayerAdminData
//{
//	PBPlayerAdminData() :
//	szName("")
//	, szPing("")
//	, szScore("")
//	, bHuman(false)
//	, bClaimed(false)
//	, bTurnActive(false)
//	{
//	}
//
//	std::string szName;
//	std::string getName() {return szName;}
//	std::string szPing;
//	std::string getPing() {return szPing;}
//	std::string szScore;
//	std::string getScore() {return szScore;}
//	bool bHuman;
//	bool bClaimed;
//	bool bTurnActive;
//};

class CvUnit;
class CvPlot;
class CvCity;

void checkBattleUnitType(BattleUnitTypes unitType);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCombatInfo
//!  \brief		Information necessary for combat.
//!             If you add any data members to this class, make sure to update
//!             the copy constructor.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCombatInfo
{
public:
	CvCombatInfo();
	CvCombatInfo& operator=(const CvCombatInfo&);

	CvUnit* getUnit(BattleUnitTypes unitType) const;
	void setUnit(BattleUnitTypes unitType, CvUnit* unit);

	CvCity* getCity(BattleUnitTypes unitType) const;
	void setCity(BattleUnitTypes unitType, CvCity* pkCity);

	const CvCombatMemberEntry* getCombatMember(BattleUnitTypes unitType) const;

	CvPlot* getPlot() const;
	void setPlot(CvPlot* plot);

	bool getAttackerAdvances() const;
	void setAttackerAdvances(bool bAdvance);

	bool getDefenderRetaliates() const;
	void setDefenderRetaliates(bool bRetaliate);

	bool getAttackIsRanged() const;
	void setAttackIsRanged(bool bRanged);

	bool getAttackIsBombingMission() const;
	void setAttackIsBombingMission(bool bBombingMission);

	bool getAttackIsAirSweep() const;
	void setAttackIsAirSweep(bool bAirSweep);

	bool getDefenderCaptured() const;
	void setDefenderCaptured(bool bDefenderCaptured);

	bool getAttackIsNuclear() const;
	int getAttackNuclearLevel() const;
	void setAttackNuclearLevel(int iNuclearDamageLevel);

	int getDamageInflicted(BattleUnitTypes unitType) const;
	void setDamageInflicted(BattleUnitTypes unitType, int iDamage);

	int getFinalDamage(BattleUnitTypes unitType) const;
	void setFinalDamage(BattleUnitTypes unitType, int iFinalDamage);

	int getFearDamageInflicted(BattleUnitTypes unitType) const;
	void setFearDamageInflicted(BattleUnitTypes unitType, int iDamage);

	int getExperience(BattleUnitTypes unitType) const;
	void setExperience(BattleUnitTypes unitType, int iExperience);

	int getMaxExperienceAllowed(BattleUnitTypes unitType) const;
	void setMaxExperienceAllowed(BattleUnitTypes unitType, int iMaxExperience);

	bool getInBorders(BattleUnitTypes unitType) const;
	void setInBorders(BattleUnitTypes unitType, bool bInBorders);

	bool getUpdateGlobal(BattleUnitTypes unitType) const;
	void setUpdateGlobal(BattleUnitTypes unitType, bool bUpdateGlobal);

	bool getVisualizeCombat() const;
	void setVisualizeCombat(bool bVisualize);

	bool getAttackerAdvancedVisualization() const;
	void setAttackerAdvancedVisualization(bool bAdvance);

	CvCombatMemberEntry* getDamageMembers();
	const CvCombatMemberEntry* getDamageMembers() const;
	int getDamageMemberCount() const;
	int getMaxDamageMemberCount() const;
	void setDamageMemberCount(int iDamageMemberCount);

protected:
	CvUnit* 	m_pUnits[BATTLE_UNIT_COUNT];					//!< The units involved
	CvCity* 	m_pCities[BATTLE_UNIT_COUNT];					//!< The cities involved

	CvPlot* 	m_pTargetPlot;									//!< The plot that the attacker is attacking
	bool		m_bAttackerAdvances;							//!< Should the attacker advance?
	bool		m_bAttackIsRanged;								//!< Attack is ranged
	bool		m_bAttackIsBombingMission;						//!< Attack is a bombing mission by an airplane
	bool		m_bAttackIsAirSweep;							//!< Attack is a bombing mission by an airplane
	bool		m_bDefenderRetaliates;							//!< Defender does not retaliate (usually set with ranged attacks)
	bool		m_bDefenderCaptured;							//!< The defender is captured, not killed.

	int			m_iNuclearDamageLevel;							//!< If > 0, the attack is a nuclear attack of the specified damage level

	int			m_iFinalDamage[BATTLE_UNIT_COUNT];				//!< The units final damage value
	int			m_iDamageInflicted[BATTLE_UNIT_COUNT];			//!< How much damage this unit inflicts on the opponent
	int			m_iFearDamageInflicted[BATTLE_UNIT_COUNT];		//!< How much fear damage this unit inflicts on the opponent

	int			m_iExperienceChange[BATTLE_UNIT_COUNT];			//!< How much experience does this unit get from battle
	int			m_iMaxExperienceAllowed[BATTLE_UNIT_COUNT];		//!< Maximum experience this unit is allowed
	bool		m_bInBorders[BATTLE_UNIT_COUNT];				//!< Was this unit within its civilization borders
	bool		m_bUpdateGlobal[BATTLE_UNIT_COUNT];				//!< Should we update the global information

	bool		m_bVisualize;									//!< The combat should be visualized

	bool		m_bAttackedAdvancedVis;							//!< If true, the attacker has already advanced its visualization of the unit (happened during combat sim).

	CvCombatMemberEntry	m_kCombatMembers[BATTLE_UNIT_COUNT];
	// Units/cities damaged in the attack.  0 for most attacks that have just the normal defenders.  Primarily used with area attacks such as the various nuclear attacks.
	// If this is non-zero, the damage members will include the primary defenders so don't apply the damage twice!
	int			m_iDamageMemberCount;
	CvCombatMemberEntry	m_kDamageMembers[MAX_DAMAGE_MEMBER_COUNT];
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMissionDefinition
//!  \brief		Base mission definition struct
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionDefinition
{
public:
	CvMissionDefinition();

	MissionTypes getMissionType() const;
	void setMissionType(MissionTypes missionType);

	float getMissionTime() const;
	void setMissionTime(float time);

	CvUnit* getUnit(BattleUnitTypes unitType) const;
	void setUnit(BattleUnitTypes unitType, CvUnit* unit);

	const CvPlot* getPlot() const;
	void setPlot(const CvPlot* plot);

	const CvPlot* getSecondaryPlot() const;
	void setSecondaryPlot(const CvPlot* plot);

protected:
	MissionTypes		m_eMissionType;					//!< The type of event
	CvUnit*				m_aUnits[BATTLE_UNIT_COUNT];	//!< The units involved
	float				m_fMissionTime;					//!< The amount of time that the event will take
	const CvPlot*		m_pPlot;						//!< The plot associated with the event
	const CvPlot*		m_pSecondaryPlot;				//!< The other plot (rarely) associated with the event
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAirMissionDefinition
//!  \brief		A definition passed to CvAirMissionManager to start an air mission
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAirMissionDefinition : public CvMissionDefinition
{
public:
	CvAirMissionDefinition();
	CvAirMissionDefinition(const CvAirMissionDefinition& kCopy);

	int getDamage(BattleUnitTypes unitType) const;
	void setDamage(BattleUnitTypes unitType, int damage);
	bool isDead(BattleUnitTypes unitType) const;

private:
	int	m_aDamage[BATTLE_UNIT_COUNT];		//!< The ending damage of the units
};

#endif	// CVSTRUCTS_H
