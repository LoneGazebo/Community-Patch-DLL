/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

//! \author		Multiple
//! \brief		Implementation of basic Civ5 structures

#include "CvGameCoreDLLPCH.h"
#include "CvUnit.h"
//#include "CvStructs.h"
#include "CvGameCoreEnumSerialization.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "CvTypes.h"

// include this after all other headers!
#include "LintFree.h"

int VoteSelectionData::GetID() const
{
	return iId;
}

void VoteSelectionData::SetID(int iID)
{
	iId = iID;
}

void VoteSelectionData::read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> iId;
	kStream >> eVoteSource;
	size_t iSize;
	kStream >> iSize;
	size_t i = 0;
	for(i = 0; i < iSize; ++i)
	{
		VoteSelectionSubData kData;
		kStream >> kData.eVote;
		kStream >> kData.ePlayer;
		kStream >> kData.iCityId;
		kStream >> kData.eOtherPlayer;
		kStream >> kData.strText;
		aVoteOptions.push_back(kData);
	}
}
FDataStream& operator>>(FDataStream& loadFrom, VoteSelectionData& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

void VoteSelectionData::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << iId;
	kStream << eVoteSource;
	kStream << aVoteOptions.size();
	for(std::vector<VoteSelectionSubData>::const_iterator it = aVoteOptions.begin(); it != aVoteOptions.end(); ++it)
	{
		kStream << (*it).eVote;
		kStream << (*it).ePlayer;
		kStream << (*it).iCityId;
		kStream << (*it).eOtherPlayer;
		kStream << (*it).strText;
	}
}
FDataStream& operator<<(FDataStream& saveTo, const VoteSelectionData& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

int VoteTriggeredData::GetID() const
{
	return iId;
}

void VoteTriggeredData::SetID(int iID)
{
	iId = iID;
}

void VoteTriggeredData::read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> iId;
	kStream >> eVoteSource;
	kStream >> kVoteOption.eVote;
	kStream >> kVoteOption.ePlayer;
	kStream >> kVoteOption.iCityId;
	kStream >> kVoteOption.eOtherPlayer;
	kStream >> kVoteOption.strText;
}
FDataStream& operator>>(FDataStream& loadFrom, VoteTriggeredData& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

void VoteTriggeredData::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << iId;
	kStream << eVoteSource;
	kStream << kVoteOption.eVote;
	kStream << kVoteOption.ePlayer;
	kStream << kVoteOption.iCityId;
	kStream << kVoteOption.eOtherPlayer;
	kStream << kVoteOption.strText;
}
FDataStream& operator<<(FDataStream& saveTo, const VoteTriggeredData& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

FDataStream& operator>>(FDataStream& kStream, BuildingYieldChange& writeTo)
{
	kStream >> writeTo.eBuildingClass;
	kStream >> writeTo.eYield;
	kStream >> writeTo.iChange;
	return kStream;
}

FDataStream& operator<<(FDataStream& kStream, const BuildingYieldChange& readFrom)
{
	kStream << readFrom.eBuildingClass;
	kStream << readFrom.eYield;
	kStream << readFrom.iChange;
	return kStream;
}

void checkBattleUnitType(BattleUnitTypes unitType)
{
	DEBUG_VARIABLE(unitType);
	CvAssertMsg((unitType >= 0) && (unitType < BATTLE_UNIT_COUNT), "Invalid battle unit type.");
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvCombatInfo::CvCombatInfo
//! \brief      Constructor
//------------------------------------------------------------------------------------------------
CvCombatInfo::CvCombatInfo() :
	m_pTargetPlot(NULL),
	m_bAttackerAdvances(false),
	m_bAttackIsRanged(false),
	m_bAttackIsBombingMission(false),
	m_bAttackIsAirSweep(false),
	m_bDefenderRetaliates(true),
	m_bDefenderCaptured(false),
	m_iNuclearDamageLevel(0),
	m_bVisualize(false),
	m_bAttackedAdvancedVis(false),
	m_iDamageMemberCount(0)
{
	for(int i=0; i<BATTLE_UNIT_COUNT; i++)
	{
		m_pUnits[i] = NULL;
		m_pCities[i] = NULL;
		m_iFinalDamage[i] = 0;
		m_iDamageInflicted[i] = 0;
		m_iFearDamageInflicted[i] = 0;
		m_iExperienceChange[i] = 0;
		m_iMaxExperienceAllowed[i] = 0;
		m_bInBorders[i] = false;
		m_bUpdateGlobal[i] = false;
	}
}

CvCombatInfo& CvCombatInfo::operator=(const CvCombatInfo& rhs)
{
	m_pTargetPlot = rhs.m_pTargetPlot;
	m_bAttackerAdvances = rhs.m_bAttackerAdvances;
	m_bAttackIsRanged = rhs.m_bAttackIsRanged;
	m_bAttackIsBombingMission = rhs.m_bAttackIsBombingMission;
	m_bAttackIsAirSweep = rhs.m_bAttackIsAirSweep;
	m_bDefenderCaptured = rhs.m_bDefenderCaptured;
	m_bDefenderRetaliates = rhs.m_bDefenderRetaliates;
	m_iNuclearDamageLevel = rhs.m_iNuclearDamageLevel;
	m_bVisualize = rhs.m_bVisualize;
	m_bAttackedAdvancedVis = rhs.m_bAttackedAdvancedVis;

	for(int i=0; i<BATTLE_UNIT_COUNT; i++)
	{
		m_pUnits[i] = rhs.m_pUnits[i];

		m_iFinalDamage[i] = rhs.m_iFinalDamage[i];
		m_iDamageInflicted[i] = rhs.m_iDamageInflicted[i];
		m_iFearDamageInflicted[i] = rhs.m_iFearDamageInflicted[i];

		m_iExperienceChange[i] = rhs.m_iExperienceChange[i];
		m_iMaxExperienceAllowed[i] = rhs.m_iMaxExperienceAllowed[i];
		m_bInBorders[i] = rhs.m_bInBorders[i];
		m_bUpdateGlobal[i] = rhs.m_bUpdateGlobal[i];
	}

	m_iDamageMemberCount = rhs.m_iDamageMemberCount;
	for(int i=0; i<m_iDamageMemberCount; ++i)
	{
		m_kDamageMembers[i] = rhs.m_kDamageMembers[i];
	}
	return (*this);
}


CvUnit* CvCombatInfo::getUnit(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_pUnits[unitType];
}
void CvCombatInfo::setUnit(BattleUnitTypes unitType, CvUnit* unit)
{
	checkBattleUnitType(unitType);
	m_pUnits[unitType] = unit;
}

CvCity* CvCombatInfo::getCity(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	if(m_pCities[unitType])
		return m_pCities[unitType];
	else if(unitType == BATTLE_UNIT_DEFENDER && m_pTargetPlot)
	{
		if(m_pTargetPlot->isCity())
			return m_pTargetPlot->getPlotCity();
	}
	return NULL;
}

void CvCombatInfo::setCity(BattleUnitTypes unitType, CvCity* pkCity)
{
	checkBattleUnitType(unitType);
	m_pCities[unitType] = pkCity;
}

CvPlot* CvCombatInfo::getPlot() const
{
	return m_pTargetPlot;
}
void CvCombatInfo::setPlot(CvPlot* plot)
{
	m_pTargetPlot = plot;
}

bool CvCombatInfo::getAttackerAdvances() const
{
	return m_bAttackerAdvances;
}

void CvCombatInfo::setAttackerAdvances(bool bAdvance)
{
	m_bAttackerAdvances = bAdvance;
}

bool CvCombatInfo::getDefenderRetaliates() const
{
	return m_bDefenderRetaliates;
}

void CvCombatInfo::setDefenderRetaliates(bool bDefenderRetaliates)
{
	m_bDefenderRetaliates = bDefenderRetaliates;
}

void CvCombatInfo::setAttackIsRanged(bool bRanged)
{
	m_bAttackIsRanged = bRanged;
}

bool CvCombatInfo::getAttackIsRanged() const
{
	return m_bAttackIsRanged;
}

void CvCombatInfo::setAttackIsBombingMission(bool bBombingMission)
{
	m_bAttackIsBombingMission = bBombingMission;
}

bool CvCombatInfo::getAttackIsBombingMission() const
{
	return m_bAttackIsBombingMission;
}

void CvCombatInfo::setAttackIsAirSweep(bool bAirSweep)
{
	m_bAttackIsAirSweep = bAirSweep;
}

bool CvCombatInfo::getAttackIsAirSweep() const
{
	return m_bAttackIsAirSweep;
}

void CvCombatInfo::setDefenderCaptured(bool bDefenderCaptured)
{
	m_bDefenderCaptured = bDefenderCaptured;
}

bool CvCombatInfo::getDefenderCaptured() const
{
	return m_bDefenderCaptured;
}

int CvCombatInfo::getDamageInflicted(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_iDamageInflicted[unitType];
}
void CvCombatInfo::setDamageInflicted(BattleUnitTypes unitType, int iDamage)
{
	checkBattleUnitType(unitType);
	m_iDamageInflicted[unitType] = iDamage;
}

int CvCombatInfo::getFinalDamage(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_iFinalDamage[unitType];
}
void CvCombatInfo::setFinalDamage(BattleUnitTypes unitType, int iFinalDamage)
{
	checkBattleUnitType(unitType);
	m_iFinalDamage[unitType] = iFinalDamage;
}


int CvCombatInfo::getFearDamageInflicted(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_iFearDamageInflicted[unitType];
}
void CvCombatInfo::setFearDamageInflicted(BattleUnitTypes unitType, int iDamage)
{
	checkBattleUnitType(unitType);
	m_iFearDamageInflicted[unitType] = iDamage;
}

int CvCombatInfo::getExperience(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_iExperienceChange[unitType];
}
void CvCombatInfo::setExperience(BattleUnitTypes unitType, int iExperience)
{
	checkBattleUnitType(unitType);
	m_iExperienceChange[unitType] = iExperience;
}

int CvCombatInfo::getMaxExperienceAllowed(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_iMaxExperienceAllowed[unitType];
}
void CvCombatInfo::setMaxExperienceAllowed(BattleUnitTypes unitType, int iMaxExperience)
{
	checkBattleUnitType(unitType);
	m_iMaxExperienceAllowed[unitType] = iMaxExperience;
}

bool CvCombatInfo::getInBorders(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_bInBorders[unitType];
}
void CvCombatInfo::setInBorders(BattleUnitTypes unitType, bool bInBorders)
{
	checkBattleUnitType(unitType);
	m_bInBorders[unitType] = bInBorders;
}

bool CvCombatInfo::getUpdateGlobal(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_bUpdateGlobal[unitType];
}
void CvCombatInfo::setUpdateGlobal(BattleUnitTypes unitType, bool bUpdateGlobal)
{
	checkBattleUnitType(unitType);
	m_bUpdateGlobal[unitType] = bUpdateGlobal;
}

bool CvCombatInfo::getVisualizeCombat() const
{
	return m_bVisualize;
}

void CvCombatInfo::setVisualizeCombat(bool bVisualize)
{
	m_bVisualize = bVisualize;
}

bool CvCombatInfo::getAttackerAdvancedVisualization() const
{
	return m_bAttackedAdvancedVis;
}

void CvCombatInfo::setAttackerAdvancedVisualization(bool bAdvance)
{
	m_bAttackedAdvancedVis = bAdvance;
}

bool CvCombatInfo::getAttackIsNuclear() const
{
	return m_iNuclearDamageLevel > 0;
}

int CvCombatInfo::getAttackNuclearLevel() const
{
	return m_iNuclearDamageLevel;
}

void CvCombatInfo::setAttackNuclearLevel(int iNuclearDamageLevel)
{
	m_iNuclearDamageLevel = iNuclearDamageLevel;
}

const CvCombatMemberEntry* CvCombatInfo::getCombatMember(BattleUnitTypes unitType) const
{
	if((int)unitType < (int)BATTLE_UNIT_COUNT)
		return &m_kCombatMembers[unitType];
	return NULL;
}

CvCombatMemberEntry* CvCombatInfo::getDamageMembers()
{
	return &m_kDamageMembers[0];
}

const CvCombatMemberEntry* CvCombatInfo::getDamageMembers() const
{
	return &m_kDamageMembers[0];
}

int CvCombatInfo::getDamageMemberCount() const
{
	return m_iDamageMemberCount;
}

int CvCombatInfo::getMaxDamageMemberCount() const
{
	return MAX_DAMAGE_MEMBER_COUNT;
}

void CvCombatInfo::setDamageMemberCount(int iDamageMemberCount)
{
	CvAssertMsg(iDamageMemberCount >=0 && iDamageMemberCount <= MAX_DAMAGE_MEMBER_COUNT, "Invalid damage member count!");
	m_iDamageMemberCount = std::min(iDamageMemberCount, (int)MAX_DAMAGE_MEMBER_COUNT);
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvMissionDefinition::CvMissionDefinition
//! \brief      Default constructor.
//------------------------------------------------------------------------------------------------
CvMissionDefinition::CvMissionDefinition() :
	m_fMissionTime(0.0f),
	m_eMissionType(NO_MISSION),
	m_pPlot(NULL),
	m_pSecondaryPlot(NULL)
{
	for(int i=0; i<BATTLE_UNIT_COUNT; i++)
		m_aUnits[i] = NULL;
}

MissionTypes CvMissionDefinition::getMissionType() const
{
	return m_eMissionType;
}

void CvMissionDefinition::setMissionType(MissionTypes missionType)
{
	m_eMissionType = missionType;
}

float CvMissionDefinition::getMissionTime() const
{
	return m_fMissionTime;
}

void CvMissionDefinition::setMissionTime(float time)
{
	m_fMissionTime = time;
}

CvUnit* CvMissionDefinition::getUnit(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aUnits[unitType];
}

void CvMissionDefinition::setUnit(BattleUnitTypes unitType, CvUnit* unit)
{
	checkBattleUnitType(unitType);
	m_aUnits[unitType] = unit;
}

const CvPlot* CvMissionDefinition::getPlot() const
{
	return m_pPlot;
}

void CvMissionDefinition::setPlot(const CvPlot* plot)
{
	m_pPlot = plot;
}

const CvPlot* CvMissionDefinition::getSecondaryPlot() const
{
	return m_pSecondaryPlot;
}

void CvMissionDefinition::setSecondaryPlot(const CvPlot* plot)
{
	m_pSecondaryPlot = plot;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvAirMissionDefinition::CvAirMissionDefinition
//! \brief      Constructor
//------------------------------------------------------------------------------------------------
CvAirMissionDefinition::CvAirMissionDefinition() :
	CvMissionDefinition()
{
	m_fMissionTime = 0.0f;
	m_eMissionType = CvTypes::getMISSION_AIRPATROL();
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvAirMissionDefinition::CvAirMissionDefinition
//! \brief      Copy constructor
//! \param      kCopy The object to copy
//------------------------------------------------------------------------------------------------
CvAirMissionDefinition::CvAirMissionDefinition(const CvAirMissionDefinition& kCopy)
{
	m_fMissionTime = kCopy.m_fMissionTime;
	m_eMissionType = kCopy.m_eMissionType;
	m_pPlot = kCopy.m_pPlot;

	for(int i=0; i<BATTLE_UNIT_COUNT; i++)
	{
		m_aDamage[i] = kCopy.m_aDamage[i];
		m_aUnits[i] = kCopy.m_aUnits[i];
	}
}

int CvAirMissionDefinition::getDamage(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aDamage[unitType];
}

void CvAirMissionDefinition::setDamage(BattleUnitTypes unitType, int damage)
{
	checkBattleUnitType(unitType);
	m_aDamage[unitType] = damage;
}

bool CvAirMissionDefinition::isDead(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	CvAssertMsg(getUnit(unitType) != NULL, "[Jason] Invalid battle unit type.");
	if(getDamage(unitType) >= getUnit(unitType)->GetMaxHitPoints())
		return true;
	else
		return false;
}

//PBGameSetupData::PBGameSetupData() :
//iSize(0)
//, iClimate(0)
//, iSeaLevel(0)
//, iSpeed(0)
//, iEra(0)
//, iMaxTurns(0)
//, iCityElimination(0)
//, iAdvancedStartPoints(0)
//, iTurnTime(0)
//, iNumCustomMapOptions(0)
//, aiCustomMapOptions(0)
//, iNumVictories(0)
//, abVictories(0)
//, szMapName("")
//, abOptions()
//, abMPOptions()
//{
//	for (int i = 0; i < NUM_GAMEOPTION_TYPES; i++)
//	{
//		abOptions.push_back(false);
//	}
//	for (int i = 0; i < NUM_MPOPTION_TYPES; i++)
//	{
//		abMPOptions.push_back(false);
//	}
//}

FDataStream& operator<<(FDataStream& saveTo, const OrderData& readFrom)
{
	saveTo << readFrom.eOrderType;
	saveTo << readFrom.iData1;
	saveTo << readFrom.iData2;
	saveTo << readFrom.bSave;
	saveTo << readFrom.bRush;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, OrderData& writeTo)
{
	loadFrom >> writeTo.eOrderType;
	loadFrom >> writeTo.iData1;
	loadFrom >> writeTo.iData2;
	loadFrom >> writeTo.bSave;
	loadFrom >> writeTo.bRush;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const PlotExtraYield& readFrom)
{
	saveTo << readFrom.m_iX;
	saveTo << readFrom.m_iY;
	saveTo << readFrom.m_aeExtraYield;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, PlotExtraYield& writeTo)
{
	loadFrom >> writeTo.m_iX;
	loadFrom >> writeTo.m_iY;
	loadFrom >> writeTo.m_aeExtraYield;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const PlotExtraCost& readFrom)
{
	saveTo << readFrom.m_iX;
	saveTo << readFrom.m_iY;
	saveTo << readFrom.m_iCost;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, PlotExtraCost& writeTo)
{
	loadFrom >> writeTo.m_iX;
	loadFrom >> writeTo.m_iY;
	loadFrom >> writeTo.m_iCost;
	return loadFrom;
}


FDataStream& operator<<(FDataStream& saveTo, const IDInfo& readFrom)
{
	saveTo << readFrom.eOwner;
	saveTo << readFrom.iID;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, IDInfo& writeTo)
{
	loadFrom >> writeTo.eOwner;
	loadFrom >> writeTo.iID;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MissionData& readFrom)
{
	saveTo << readFrom.eMissionType;
	saveTo << readFrom.iData1;
	saveTo << readFrom.iData2;
	saveTo << readFrom.iFlags;
	saveTo << readFrom.iPushTurn;
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MissionData& writeTo)
{
	loadFrom >> writeTo.eMissionType;
	loadFrom >> writeTo.iData1;
	loadFrom >> writeTo.iData2;
	loadFrom >> writeTo.iFlags;
	loadFrom >> writeTo.iPushTurn;
	return loadFrom;
}


