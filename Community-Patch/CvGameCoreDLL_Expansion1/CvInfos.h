/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

//
//  AUTHOR:	Eric MacDonald  --  8/2003
//					Mustafa Thamer 11/2004
//					Jon Shafer - 03/2005
//
//  PURPOSE: All Civ4 info classes and the base class for them
//
#ifndef CV_INFO_H
#define CV_INFO_H

#include "CvStructs.h"

#include <unordered_map>

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class
#pragma warning( disable: 4127 )

class CvDatabaseUtility;

//------------------------------------------------------------------------------
// CvBaseInfo: New basic info adapter for game data.
//------------------------------------------------------------------------------
class CvBaseInfo
{
public:
	CvBaseInfo();

	int GetID() const
	{
		return m_iID;
	}

	const char* GetCivilopedia() const
	{
		return m_strCivilopedia.c_str();
	}

	const char* GetDescription() const
	{
		return m_strDescription.c_str();
	}

	const char* GetDescriptionKey() const
	{
		return m_strDescriptionKey.c_str();
	}

	const char* GetHelp() const
	{
		return m_strHelp.c_str();
	}

	const char* GetDisabledHelp() const
	{
		return m_strDisabledHelp.c_str();
	}

	const char* GetStrategy() const
	{
		return m_strStrategy.c_str();
	}

	const char* GetText() const;
	const char* GetTextKey() const;

	const char* GetType() const
	{
		return m_strType.c_str();
	}

	bool CacheResult(Database::Results& kResults);
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	virtual bool operator==(const CvBaseInfo&) const;

	virtual void readFrom(FDataStream&);
	virtual void writeTo(FDataStream&) const;

private:
	int m_iID;
	CvString m_strCivilopedia;
	CvString m_strDescription;
	CvString m_strDescriptionKey;
	CvString m_strHelp;
	CvString m_strDisabledHelp;
	CvString m_strStrategy;
	CvString m_strType;
	CvString m_strTextKey;
	CvString m_strText;
};

FDataStream& operator<<(FDataStream&, const CvBaseInfo&);
FDataStream& operator>>(FDataStream&, CvBaseInfo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvHotkeyInfo
//!  \brief		Holds the hotkey info for an info class.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHotKeyInfo : public CvBaseInfo
{
public:
	CvHotKeyInfo();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	int getActionInfoIndex() const;
	void setActionInfoIndex(int i);

	int getHotKeyVal() const;
	void setHotKeyVal(int i);
	int getHotKeyPriority() const;
	void setHotKeyPriority(int i);
	int getHotKeyValAlt() const;
	void setHotKeyValAlt(int i);
	int getHotKeyPriorityAlt() const;
	void setHotKeyPriorityAlt(int i);
	int getOrderPriority() const;
	void setOrderPriority(int i);

	bool isAltDown() const;
	void setAltDown(bool b);
	bool isShiftDown() const;
	void setShiftDown(bool b);
	bool isCtrlDown() const;
	void setCtrlDown(bool b);
	bool isAltDownAlt() const;
	void setAltDownAlt(bool b);
	bool isShiftDownAlt() const;
	void setShiftDownAlt(bool b);
	bool isCtrlDownAlt() const;
	void setCtrlDownAlt(bool b);

	const char* getHotKey() const;
	void setHotKey(const char* szVal);

	const char* getHelp() const;
	const char* getDisabledHelp() const;

	std::string getHotKeyDescription() const;
	const char* getHotKeyString() const;
	void setHotKeyDescription(const char* swzHotKeyDescKey, const char* szHotKeyAltDescKey, const char* szHotKeyString);

	static CvString CreateHotKeyFromDescription(const char* pszHotKey, bool bShift, bool bAlt, bool bCtrl);
	static CvString CreateKeyStringFromKBCode(const char* pszHotKey);

protected:
	int GetHotKeyInt(const char* pszHotKeyVal);

	int m_iActionInfoIndex;

	int m_iHotKeyVal;
	int m_iHotKeyPriority;
	int m_iHotKeyValAlt;
	int m_iHotKeyPriorityAlt;
	int m_iOrderPriority;

	bool m_bAltDown;
	bool m_bShiftDown;
	bool m_bCtrlDown;
	bool m_bAltDownAlt;
	bool m_bShiftDownAlt;
	bool m_bCtrlDownAlt;

	CvString m_strHotKey;
	CvString m_strHotKeyDescriptionKey;
	CvString m_strHotKeyAltDescriptionKey;
	CvString m_strHotKeyString;
	CvString m_strHelp;
	CvString m_strDisabledHelp;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSpecialistInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialistInfo : public CvHotKeyInfo
{
public:

	CvSpecialistInfo();
	virtual ~CvSpecialistInfo();

	int getCost() const;

	int getGreatPeopleUnitClass() const;
	int getGreatPeopleRateChange() const;
	int getCulturePerTurn() const;
	int getMissionType() const;
	void setMissionType(int iNewType);
	int getExperience() const;

	bool isVisible() const;

	// Arrays
	int getYieldChange(int i) const;
	const int* getYieldChangeArray() const;
	int getFlavorValue(int i) const;

	const char* getTexture() const;
	void setTexture(const char* szVal);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iCost;

	int m_iGreatPeopleUnitClass;
	int m_iGreatPeopleRateChange;
	int m_iCulturePerTurn;
	int m_iMissionType;
	int m_iExperience;

	bool m_bVisible;

	CvString m_strTexture;

	// Arrays

	int* m_piYieldChange;
	int* m_piFlavorValue;

private:
	CvSpecialistInfo(const CvSpecialistInfo&);
	CvSpecialistInfo& operator=(const CvSpecialistInfo&);

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvMissionInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionInfo : public CvHotKeyInfo
{
public:
	CvMissionInfo();

	int getTime() const;

	bool isSound() const;
	bool isTarget() const;
	bool isBuild() const;
	bool getVisible() const;
	EntityEventTypes getEntityEvent() const;

	const char* getWaypoint() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iTime;

	bool m_bSound;
	bool m_bTarget;
	bool m_bBuild;
	bool m_bVisible;
	EntityEventTypes m_eEntityEvent;

	CvString m_strWaypoint;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvControlInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvControlInfo : public CvHotKeyInfo
{
	//Empty?
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvCommandInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommandInfo : public CvHotKeyInfo
{
public:
	CvCommandInfo();

	int getAutomate() const;
	void setAutomate(int i);

	bool getConfirmCommand() const;
	bool getVisible() const;
	bool getAll() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
	bool m_bAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvAutomateInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAutomateInfo : public CvHotKeyInfo
{
public:
	CvAutomateInfo();

	int getCommand() const;
	void setCommand(int i);
	int getAutomate() const;
	void setAutomate(int i);

	bool getConfirmCommand() const;
	void setConfirmCommand(bool bVal);
	bool getVisible() const;
	void setVisible(bool bVal);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iCommand;
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvActionInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvActionInfo
{
public:
	CvActionInfo();

	int getMissionData() const;
	int getCommandData() const;

	int getAutomateType() const;
	int getInterfaceModeType() const;
	int getMissionType() const;
	int getCommandType() const;
	int getControlType() const;
	int getOriginalIndex() const;
	void setOriginalIndex(int i);

	bool isConfirmCommand() const;
	bool isVisible() const;
	ActionSubTypes getSubType() const;
	void setSubType(ActionSubTypes eSubType);

	// functions to replace the CvInfoBase calls
	const char* GetType() const;
	const char* GetDescription() const;
	const char* GetCivilopedia() const;
	const char* GetHelp() const;
	const char* GetDisabledHelp() const;
	const char* GetStrategy() const;
	const char* GetTextKey() const;

	// functions to replace the CvHotkey calls
	int getActionInfoIndex() const;
	int getHotKeyVal() const;
	int getHotKeyPriority() const;
	int getHotKeyValAlt() const;
	int getHotKeyPriorityAlt() const;
	int getOrderPriority() const;

	bool isAltDown() const;
	bool isShiftDown() const;
	bool isCtrlDown() const;
	bool isAltDownAlt() const;
	bool isShiftDownAlt() const;
	bool isCtrlDownAlt() const;

	const char* getHotKey() const;

	std::string getHotKeyDescription() const;
	const char* getHotKeyString() const;

protected:
	int m_iOriginalIndex;
	ActionSubTypes m_eSubType;

private:
	CvHotKeyInfo* getHotkeyInfo() const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvMultiUnitFormationInfo
//
// \brief	: Holds information relating to a formation of multiple units (for AI use)
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvFormationSlotEntry
{
public:
	CvFormationSlotEntry():
		m_primaryUnitType(NO_UNITAI),
		m_secondaryUnitType(NO_UNITAI),
		m_ePositionType(NO_MUPOSITION),
		m_requiredSlot(false)
	{
	}

	int m_primaryUnitType;
	int m_secondaryUnitType;
	MultiunitPositionTypes m_ePositionType;
	bool m_requiredSlot;
};

class CvMultiUnitFormationInfo : public CvBaseInfo
{
public:
	const char* getFormationName() const;
	int getNumFormationSlotEntries() const;
	const CvFormationSlotEntry& getFormationSlotEntry(int index) const;
	void addFormationSlotEntry(const CvFormationSlotEntry& slotEntry);
	bool IsRequiresNavalUnitConsistency() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	CvString m_strFormationName;
	std::vector<CvFormationSlotEntry> m_vctSlotEntries;
	bool m_bRequiresNavalUnitConsistency;
};



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSpecialUnitInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialUnitInfo : public CvBaseInfo
{
public:
	CvSpecialUnitInfo();
	virtual ~CvSpecialUnitInfo();

	bool isValid() const;
	bool isCityLoad() const;

	// Arrays
	bool isCarrierUnitAIType(int i) const;
	int getProductionTraits(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	bool m_bValid;
	bool m_bCityLoad;

	// Arrays
	bool* m_pbCarrierUnitAITypes;
	int* m_piProductionTraits;

private:
	CvSpecialUnitInfo(const CvSpecialUnitInfo&);
	CvSpecialUnitInfo& operator=(const CvSpecialUnitInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvUnitClassInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitClassInfo :	public CvBaseInfo
{
public:
	CvUnitClassInfo();

	int getMaxGlobalInstances() const;
	int getMaxTeamInstances() const;
	int getMaxPlayerInstances() const;
	int getInstanceCostModifier() const;
	int getDefaultUnitIndex() const;
	void setDefaultUnitIndex(int i);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	int m_iInstanceCostModifier;
	int m_iDefaultUnitIndex;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvBuildingClassInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingClassInfo :	public CvBaseInfo
{
public:

	CvBuildingClassInfo();
	virtual ~CvBuildingClassInfo();

	int getMaxGlobalInstances() const;
	int getMaxTeamInstances() const;
	int getMaxPlayerInstances() const;
	int getExtraPlayerInstances() const;
	int getDefaultBuildingIndex() const;
	void setDefaultBuildingIndex(int i);

	bool isNoLimit() const;
	bool isMonument() const;

	// Arrays
	int getVictoryThreshold(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	int m_iExtraPlayerInstances;
	int m_iDefaultBuildingIndex;

	bool m_bNoLimit;
	bool m_bMonument;

	// Arrays
	int* m_piVictoryThreshold;

private:
	CvBuildingClassInfo(const CvBuildingClassInfo&);
	CvBuildingClassInfo& operator=(const CvBuildingClassInfo&);
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays of variable length
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace BuildingClassArrayHelpers
{
void Read(FDataStream& kStream, int* paiArray);
void Write(FDataStream& kStream, int* paiArray, int iArraySize);
}
namespace UnitClassArrayHelpers
{
void Read(FDataStream& kStream, int* paiArray);
void Write(FDataStream& kStream, int* paiArray, int iArraySize);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvCivilizationBaseInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCivilizationBaseInfo : public CvBaseInfo
{
public:
	CvCivilizationBaseInfo();
	virtual		~CvCivilizationBaseInfo();

	bool isAIPlayable() const;
	bool isPlayable() const;

	const char* getShortDescription() const;
	void setShortDescriptionKey(const char* szVal);
	const char* getShortDescriptionKey() const;

	const GUID& getPackageID() const;

	virtual	bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:

	bool		m_bAIPlayable;
	bool		m_bPlayable;

	CvString	m_strShortDescription;
	CvString	m_strShortDescriptionKey;

	GUID		m_kPackageID;

private:
	CvCivilizationBaseInfo(const CvCivilizationBaseInfo&);
	CvCivilizationBaseInfo& operator=(const CvCivilizationBaseInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvCivilizationInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCivilizationInfo : public CvCivilizationBaseInfo
{
public:
	CvCivilizationInfo();
	virtual ~CvCivilizationInfo();

	int getDefaultPlayerColor() const;
	int getArtStyleType() const;
	int getNumCityNames() const;
	int getNumSpyNames() const;
	const char* getArtStylePrefix() const;
	const char* getArtStyleSuffix() const;

	int getNumLeaders() const;

	const char* GetDawnOfManAudio() const;

	const char* getAdjective() const;
	void setAdjectiveKey(const char* szVal);
	const char* getAdjectiveKey() const;

	const char* getFlagTexture() const;
	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	void setArtStylePrefix(const char* szVal);
	void setArtStyleSuffix(const char* szVal);

	ReligionTypes GetReligion() const;

	// Arrays
	int getCivilizationBuildings(int i) const;
	int getCivilizationUnits(int i) const;
	bool isCivilizationBuildingOverridden(int i) const;
	bool isCivilizationUnitOverridden(int i) const;
	int getCivilizationFreeUnitsClass(int i) const;
	int getCivilizationFreeUnitsDefaultUnitAI(int i) const;

	bool isLeaders(int i) const;
	bool isCivilizationFreeBuildingClass(int i) const;
	bool isCivilizationFreeTechs(int i) const;
	bool isCivilizationDisableTechs(int i) const;

	bool isCoastalCiv() const;

	const char* getCityNames(int i) const;
	const char* getSpyNames(int i) const;

	const char* getSoundtrackKey() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	// allocate and initialize the civilization's default buildings
	void InitBuildingDefaults(int*& piDefaults, CvDatabaseUtility& kUtility);

	// allocate and initialize the civilization's default units
	void InitUnitDefaults(int*& piDefaults, CvDatabaseUtility& kUtility);

	int m_iDefaultPlayerColor;
	int m_iArtStyleType;
	int m_iNumLeaders;				 // the number of leaders the Civ has, this is needed so that random leaders can be generated easily

	CvString m_strArtDefineTag;
	CvString m_strArtStylePrefix;
	CvString m_strArtStyleSuffix;

	CvString m_strAdjective;
	CvString m_strAdjectiveKey;

	CvString m_strDawnOfManAudio;

	CvString m_strSoundtrackKey;

	// Arrays
	int* m_piCivilizationBuildings;
	int* m_piCivilizationUnits;
	int* m_piCivilizationFreeUnitsClass;
	int* m_piCivilizationFreeUnitsDefaultUnitAI;

	bool* m_pbLeaders;
	bool* m_pbCivilizationFreeBuildingClass;
	bool* m_pbCivilizationFreeTechs;
	bool* m_pbCivilizationDisableTechs;
	bool* m_pbReligions;
	bool m_bCoastalCiv;

	std::vector<CvString> m_vCityNames;
	std::vector<CvString> m_vSpyNames;

	std::vector<bool> m_CivilizationBuildingOverridden;
	std::vector<bool> m_CivilizationUnitOverridden;

private:
	CvCivilizationInfo(const CvCivilizationInfo&);
	CvCivilizationInfo& operator=(const CvCivilizationInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvVictoryInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVictoryInfo : public CvBaseInfo
{
public:
	CvVictoryInfo();
	virtual ~CvVictoryInfo();

	int getPopulationPercentLead() const;
	int getLandPercent() const;
	int getMinLandPercent() const;
	int getCityCulture() const;
	int getNumCultureCities() const;
	int getTotalCultureRatio() const;
	int getVictoryDelayTurns() const;

	bool IsWinsGame() const;
	bool isTargetScore() const;
	bool isEndScore() const;
	bool isConquest() const;
	bool isDiploVote() const;
	bool isPermanent() const;
	bool IsReligionInAllCities() const;
	bool IsFindAllNaturalWonders() const;

	const char* getMovie() const;

	// Accessor Functions (Arrays)
	int GetVictoryPointAward(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iPopulationPercentLead;
	int m_iLandPercent;
	int m_iMinLandPercent;
	int m_iCityCulture;
	int m_iNumCultureCities;
	int m_iTotalCultureRatio;
	int m_iVictoryDelayTurns;

	bool m_bWinsGame;
	bool m_bTargetScore;
	bool m_bEndScore;
	bool m_bConquest;
	bool m_bDiploVote;
	bool m_bPermanent;
	bool m_bReligionInAllCities;
	bool m_bFindAllNaturalWonders;

	CvString m_strMovie;

	int* m_piVictoryPointAwards;

private:
	CvVictoryInfo(const CvVictoryInfo&);
	CvVictoryInfo& operator=(const CvVictoryInfo&);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSmallAwardInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSmallAwardInfo : public CvBaseInfo
{
public:
	CvSmallAwardInfo();
	virtual ~CvSmallAwardInfo();

	CvString GetNotificationString() const;
	CvString GetTeamNotificationString() const;

	int GetNumVictoryPoints() const;

	int GetNumCities() const;
	int GetCityPopulation() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	CvString m_szNotification;
	CvString m_szTeamNotification;

	int m_iNumVictoryPoints;

	int m_iNumCities;
	int m_iCityPopulation;

private:
	CvSmallAwardInfo(const CvSmallAwardInfo&);
	CvSmallAwardInfo& operator=(const CvSmallAwardInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvHurryInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHurryInfo :	public CvBaseInfo
{
public:
	CvHurryInfo();

	int getPolicyPrereq() const;
	int getGoldPerProduction() const;
	int getProductionPerPopulation() const;
	int getGoldPerBeaker() const;
	int getGoldPerCulture() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:

	int m_iPolicyPrereq;
	int m_iGoldPerProduction;
	int m_iProductionPerPopulation;
	int m_iGoldPerBeaker;
	int m_iGoldPerCulture;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvHandicapInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHandicapInfo : public CvBaseInfo
{
public:
	CvHandicapInfo();
	virtual ~CvHandicapInfo();

	int getStartingLocationPercent() const;
	int getAdvancedStartPointsMod() const;
	int getStartingPolicyPoints() const;
	int getHappinessDefault() const;
	int getExtraHappinessPerLuxury() const;
	int getNumCitiesUnhappinessMod() const;
	int getPopulationUnhappinessMod() const;
	int getStartingGold() const;
	int getGoldFreeUnits() const;
	int getProductionFreeUnits() const;
	int getProductionFreeUnitsPerCity() const;
	int getProductionFreeUnitsPopulationPercent() const;
	int getRouteCostPercent() const;
	int getUnitCostPercent() const;
	int getBuildingCostPercent() const;
	int getResearchPercent() const;
	int getPolicyPercent() const;
	int getImprovementCostPercent() const;
	int GetCityProductionNumOptions() const;
	int GetTechNumOptions() const;
	int GetPolicyNumOptions() const;
	int getInflationPercent() const;
	int getFreeCulturePerTurn() const;
	int getAttitudeChange() const;
	int getNoTechTradeModifier() const;
	int getTechTradeKnownModifier() const;
	int getBarbCampGold() const;
	int getBarbSpawnMod() const;
	int getBarbarianCombatModifier() const;
	int getAIBarbarianCombatModifier() const;
	int getEarliestBarbarianReleaseTurn() const;
	int getBarbarianLandTargetRange() const;
	int getBarbarianSeaTargetRange() const;

	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
	int getAIStartingUnitMultiplier() const;
	int getAIStartingDefenseUnits() const;
	int getAIStartingWorkerUnits() const;
	int getAIStartingExploreUnits() const;
	int getAIDeclareWarProb() const;
	int getAIWorkRateModifier() const;
	int getAIUnhappinessPercent() const;
	int getAIGrowthPercent() const;
	int getAITrainPercent() const;
	int getAIWorldTrainPercent() const;
	int getAIConstructPercent() const;
	int getAIWorldConstructPercent() const;
	int getAICreatePercent() const;
	int getAIWorldCreatePercent() const;
	int getAIBuildingCostPercent() const;
	int getAIUnitCostPercent() const;
	int getAIUnitSupplyPercent() const;
	int getAIUnitUpgradePercent() const;
	int getAIInflationPercent() const;
	int getAIPerEraModifier() const;
	int getAIAdvancedStartPercent() const;
	int getNumGoodies() const;

	// Arrays
	int getGoodies(int i) const;
	int isFreeTechs(int i) const;
	int isAIFreeTechs(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iStartingLocationPercent;
	int m_iAdvancedStartPointsMod;
	int m_iStartingPolicyPoints;
	int m_iHappinessDefault;
	int m_iExtraHappinessPerLuxury;
	int m_iNumCitiesUnhappinessMod;
	int m_iPopulationUnhappinessMod;
	int m_iStartingGold;
	int m_iGoldFreeUnits;
	int m_iProductionFreeUnits;
	int m_iProductionFreeUnitsPerCity;
	int m_iProductionFreeUnitsPopulationPercent;
	int m_iRouteCostPercent;
	int m_iUnitCostPercent;
	int m_iBuildingCostPercent;
	int m_iResearchPercent;
	int m_iPolicyPercent;
	int m_iImprovementCostPercent;
	int m_iCityProductionNumOptions;
	int m_iTechNumOptions;
	int m_iPolicyNumOptions;
	int m_iInflationPercent;
	int m_iFreeCulturePerTurn;
	int m_iAttitudeChange;
	int m_iNoTechTradeModifier;
	int m_iTechTradeKnownModifier;
	int m_iBarbCampGold;
	int m_iBarbSpawnMod;
	int m_iBarbarianCombatModifier;
	int m_iAIBarbarianCombatModifier;
	int m_iEarliestBarbarianReleaseTurn;
	int m_iBarbarianLandTargetRange;
	int m_iBarbarianSeaTargetRange;

	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
	int m_iAIStartingUnitMultiplier;
	int m_iAIStartingDefenseUnits;
	int m_iAIStartingWorkerUnits;
	int m_iAIStartingExploreUnits;
	int m_iAIDeclareWarProb;
	int m_iAIWorkRateModifier;
	int m_iAIUnhappinessPercent;
	int m_iAIGrowthPercent;
	int m_iAITrainPercent;
	int m_iAIWorldTrainPercent;
	int m_iAIConstructPercent;
	int m_iAIWorldConstructPercent;
	int m_iAICreatePercent;
	int m_iAIWorldCreatePercent;
	int m_iAIBuildingCostPercent;
	int m_iAIUnitCostPercent;
	int m_iAIUnitSupplyPercent;
	int m_iAIUnitUpgradePercent;
	int m_iAIInflationPercent;
	int m_iAIPerEraModifier;
	int m_iAIAdvancedStartPercent;
	int m_iNumGoodies;

	CvString m_strHandicapName;

	// Arrays
	int* m_piGoodies;
	bool* m_pbFreeTechs;
	bool* m_pbAIFreeTechs;

private:
	CvHandicapInfo(const CvHandicapInfo&);
	CvHandicapInfo& operator=(const CvHandicapInfo&);

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvGameSpeedInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameSpeedInfo :	public CvBaseInfo
{
public:
	CvGameSpeedInfo();
	virtual ~CvGameSpeedInfo();

	int GetDealDuration() const;
	int getGrowthPercent() const;
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getCreatePercent() const;
	int getResearchPercent() const;
	int getGoldPercent() const;
	int getGoldGiftMod() const;
	int getBuildPercent() const;
	int getImprovementPercent() const;
	int getGreatPeoplePercent() const;
	int getCulturePercent() const;
	int getFaithPercent() const;
	int getBarbPercent() const;
	int getFeatureProductionPercent() const;
	int getUnitDiscoverPercent() const;
	int getUnitHurryPercent() const;
	int getUnitTradePercent() const;
	int getGoldenAgePercent() const;
	int getHurryPercent() const;
	int getInflationOffset() const;
	int getReligiousPressureAdjacentCity() const;
	int getInflationPercent() const;
	int getVictoryDelayPercent() const;
	int getMinorCivElectionFreqMod() const;
	int getOpinionDurationPercent() const;
	int getSpyRatePercent() const;
	int getPeaceDealDuration() const;
	int getRelationshipDuration() const;
	int getNumTurnIncrements() const;

	GameTurnInfo& getGameTurnInfo(int iIndex) const;
	void allocateGameTurnInfos(const int iSize);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iDealDuration;
	int m_iGrowthPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iCreatePercent;
	int m_iResearchPercent;
	int m_iGoldPercent;
	int m_iGoldGiftMod;
	int m_iBuildPercent;
	int m_iImprovementPercent;
	int m_iGreatPeoplePercent;
	int m_iCulturePercent;
	int m_iFaithPercent;
	int m_iBarbPercent;
	int m_iFeatureProductionPercent;
	int m_iUnitDiscoverPercent;
	int m_iUnitHurryPercent;
	int m_iUnitTradePercent;
	int m_iGoldenAgePercent;
	int m_iHurryPercent;
	int m_iInflationOffset;
	int m_iReligiousPressureAdjacentCity;
	int m_iInflationPercent;
	int m_iVictoryDelayPercent;
	int m_iMinorCivElectionFreqMod;
	int m_iOpinionDurationPercent;
	int m_iSpyRatePercent;
	int m_iPeaceDealDuration;
	int m_iRelationshipDuration;

	int m_iNumTurnIncrements;

	CvString m_strGameSpeedName;
	GameTurnInfo* m_pGameTurnInfo;

private:
	CvGameSpeedInfo(const CvGameSpeedInfo&);
	CvGameSpeedInfo& operator=(const CvGameSpeedInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CvTurnTimerInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTurnTimerInfo :	public CvBaseInfo
{
public:
	CvTurnTimerInfo();

	int getBaseTime() const;
	int getCityResource() const;
	int getUnitResource() const;
	int getFirstTurnMultiplier() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);
	virtual bool operator==(const CvTurnTimerInfo&) const;

	virtual void readFrom(FDataStream& readFrom);
	virtual void writeTo(FDataStream& saveTo) const;

protected:
	int m_iBaseTime;
	int m_iCityResource;
	int m_iUnitResource;
	int m_iFirstTurnMultiplier;
};

FDataStream& operator<<(FDataStream&, const CvTurnTimerInfo&);
FDataStream& operator>>(FDataStream&, CvTurnTimerInfo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvBuildInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildInfo : public CvHotKeyInfo
{
public:
	CvBuildInfo();
	virtual ~CvBuildInfo();

	int getTime() const;
	int getCost() const;
	int getCostIncreasePerImprovement() const;
	int getTechPrereq() const;
	int getImprovement() const;
	int getRoute() const;
	int getEntityEvent() const;
	int getMissionType() const;
	void setMissionType(int iNewType);

	bool isKill() const;
	bool isRepair() const;
	bool IsRemoveRoute() const;
	bool IsWater() const;

	// Arrays
	int getFeatureTech(int i) const;
	int getFeatureTime(int i) const;
	int getFeatureProduction(int i) const;
	int getFeatureCost(int i) const;
	int getTechTimeChange(int i) const;

	bool isFeatureRemove(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iTime;
	int m_iCost;
	int m_iCostIncreasePerImprovement;
	int m_iTechPrereq;
	int m_iImprovement;
	int m_iRoute;
	int m_iEntityEvent;
	int m_iMissionType;

	bool m_bKill;
	bool m_bRepair;
	bool m_bRemoveRoute;
	bool m_bWater;

	// Arrays
	int* m_paiFeatureTech;
	int* m_paiFeatureTime;
	int* m_paiFeatureProduction;
	int* m_paiFeatureCost;
	int* m_paiTechTimeChange;
	bool* m_pabFeatureRemove;

private:
	CvBuildInfo(const CvBuildInfo&);
	CvBuildInfo& operator=(const CvBuildInfo&);
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays of variable length (based on number of builds defined in game)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace BuildArrayHelpers
{
void Read(FDataStream& kStream, short* paiBuildArray);
void Write(FDataStream& kStream, short* paiBuildArray, int iArraySize);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CvGoodyInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGoodyInfo :	public CvBaseInfo
{
public:
	CvGoodyInfo();

	int getGold() const;
	int getNumGoldRandRolls() const;
	int getGoldRandAmount() const;
	int getMapOffset() const;
	int getMapRange() const;
	int getMapProb() const;
	int getExperience() const;
	int getHealing() const;
	int getDamagePrereq() const;
	int getPopulation() const;
	int getCulture() const;
	int getFaith() const;
	int getProphetPercent() const;
	int getRevealNearbyBarbariansRange() const;
	int getBarbarianUnitProb() const;
	int getMinBarbarians() const;
	int getUnitClassType() const;
	int getBarbarianUnitClass() const;

	bool isTech() const;
	bool isRevealUnknownResource() const;
	bool isUpgradeUnit() const;
	bool isPantheonFaith() const;
	bool isBad() const;

	const char* getSound() const;
	void setSound(const char* szVal);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iGold;
	int m_iNumGoldRandRolls;
	int m_iGoldRandAmount;
	int m_iMapOffset;
	int m_iMapRange;
	int m_iMapProb;
	int m_iExperience;
	int m_iHealing;
	int m_iDamagePrereq;
	int m_iPopulation;
	int m_iCulture;
	int m_iFaith;
	int m_iProphetPercent;
	int m_iRevealNearbyBarbariansRange;
	int m_iBarbarianUnitProb;
	int m_iMinBarbarians;
	int m_iUnitClassType;
	int m_iBarbarianUnitClass;

	bool m_bTech;
	bool m_bRevealUnknownResource;
	bool m_bUpgradeUnit;
	bool m_bPantheonFaith;
	bool m_bBad;

	CvString m_strSound;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvRouteInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteInfo :	public CvBaseInfo
{
public:
	CvRouteInfo();
	virtual ~CvRouteInfo();

	int GetGoldMaintenance() const;
	int getAdvancedStartCost() const;

	int getValue() const;
	int getMovementCost() const;
	int getFlatMovementCost() const;

	bool IsIndustrial() const;

	// Arrays
	int getYieldChange(int i) const;
	int getTechMovementChange(int i) const;
	int getResourceQuantityRequirement(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iGoldMaintenance;
	int m_iAdvancedStartCost;
	int m_iValue;
	int m_iMovementCost;
	int m_iFlatMovementCost;

	int m_bIndustrial;

	// Arrays
	int* m_piYieldChange;
	int* m_piTechMovementChange;
	int* m_piResourceQuantityRequirements;

private:
	CvRouteInfo(const CvRouteInfo&);
	CvRouteInfo& operator=(const CvRouteInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvResourceClassInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvResourceClassInfo :	public CvBaseInfo
{
public:
	CvResourceClassInfo();

	int getUniqueRange() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);
protected:
	int m_iUniqueRange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvResourceInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvResourceInfo : public CvBaseInfo
{
public:
	CvResourceInfo();
	virtual ~CvResourceInfo();

	int getResourceClassType() const;
	int getChar() const;
	void setChar(int i);
	int getTechReveal() const;
	int getTechCityTrade() const;
	int getTechObsolete() const;
	int getAIStopTradingEra() const;
	int getStartingResourceQuantity() const;
	int getAITradeModifier() const;
	int getAIObjective() const;
	int getHappiness() const;
	int getWonderProductionMod() const;
	int getMinAreaSize() const;
	int getMinLatitude() const;
	int getMaxLatitude() const;
	int getPlacementOrder() const;
	int getConstAppearance() const;
	int getRandAppearance1() const;
	int getRandAppearance2() const;
	int getRandAppearance3() const;
	int getRandAppearance4() const;
	int getPercentPerPlayer() const;
	int getTilesPer() const;
	int getMinLandPercent() const;
	int getUniqueRange() const;
	int getGroupRange() const;
	int getGroupRand() const;

	bool isPresentOnAllValidPlots() const;
	bool isOneArea() const;
	bool isHills() const;
	bool isFlatlands() const;
	bool isNoRiverSide() const;
	bool isNormalize() const;
	bool isOnlyMinorCivs() const;

	ResourceUsageTypes getResourceUsage() const;

	const char* GetIconString() const;
	void SetIconString(const char* szVal);

	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	const char* getArtDefineTagHeavy() const;
	void setArtDefineTagHeavy(const char* szVal);

	const char* getAltArtDefineTag() const;
	void setAltArtDefineTag(const char* szVal);
	const char* getAltArtDefineTagHeavy() const;
	void setAltArtDefineTagHeavy(const char* szVal);

	// Arrays
	int getYieldChange(int i) const;
	int* getYieldChangeArray();
	int getImprovementChange(int i) const;

	int getResourceQuantityType(int i) const;

	bool isTerrain(int i) const;
	bool isFeature(int i) const;
	bool isFeatureTerrain(int i) const;

	int getFlavorValue(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iResourceClassType;
	int m_iChar;
	int m_iTechReveal;
	int m_iTechCityTrade;
	int m_iTechObsolete;
	int m_iAIStopTradingEra;
	int m_iStartingResourceQuantity;
	int m_iAITradeModifier;
	int m_iAIObjective;
	int m_iHappiness;
	int m_iWonderProductionMod;
	int m_iMinAreaSize;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iPlacementOrder;
	int m_iConstAppearance;
	int m_iRandAppearance1;
	int m_iRandAppearance2;
	int m_iRandAppearance3;
	int m_iRandAppearance4;
	int m_iPercentPerPlayer;
	int m_iTilesPer;
	int m_iMinLandPercent;
	int m_iUniqueRange;
	int m_iGroupRange;
	int m_iGroupRand;

	bool m_bPresentOnAllValidPlots;
	bool m_bOneArea;
	bool m_bHills;
	bool m_bFlatlands;
	bool m_bNoRiverSide;
	bool m_bNormalize;
	bool m_bOnlyMinorCivs;

	ResourceUsageTypes m_eResourceUsage;

	CvString m_strIconString;
	CvString m_strArtDefineTag;
	CvString m_strArtDefineTagHeavy;
	CvString m_strAltArtDefineTag;
	CvString m_strAltArtDefineTagHeavy;

	// Arrays
	int* m_piYieldChange;
	int* m_piResourceQuantityTypes;
	int* m_piImprovementChange;
	int* m_piFlavor;

	bool* m_pbTerrain;
	bool* m_pbFeature;
	bool* m_pbFeatureTerrain;

private:
	CvResourceInfo(const CvResourceInfo&);
	CvResourceInfo& operator=(const CvResourceInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvFeatureInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvFeatureInfo : public CvBaseInfo
{
public:
	CvFeatureInfo();
	virtual ~CvFeatureInfo();

	int getStartingLocationWeight() const;
	int getMovementCost() const;
	int getSeeThroughChange() const;
	int getAppearanceProbability() const;
	int getDisappearanceProbability() const;
	int getGrowthProbability() const;
	int getGrowthTerrainType() const;
	int getDefenseModifier() const;
	int getInfluenceCost() const;
	int getAdvancedStartRemoveCost() const;
	int getTurnDamage() const;
	int getFirstFinderGold() const;
	int getInBorderHappiness() const;
	int getOccurrenceFrequency() const;
	int getAdjacentUnitFreePromotion() const;

	bool isYieldNotAdditive() const;
	bool isNoCoast() const;
	bool isNoRiver() const;
	bool isNoAdjacent() const;
	bool isRequiresFlatlands() const;
	bool isRequiresRiver() const;
	bool isAddsFreshWater() const;
	bool isImpassable() const;
	bool isNoCity() const;
	bool isNoImprovement() const;
	bool isVisibleAlways() const;
	bool isNukeImmune() const;
	bool IsRough() const;
	bool IsNaturalWonder() const;

	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szTag);

	int getWorldSoundscapeScriptId() const;
	const char* getEffectTypeTag() const;

	const char* getEffectType() const;
	int getEffectProbability() const;


	int getYieldChange(int i) const;
	int getRiverYieldChange(int i) const;
	int getHillsYieldChange(int i) const;
	int get3DAudioScriptFootstepIndex(int i) const;

	bool isTerrain(int i) const;

	// Set each time the game is started
	bool IsClearable() const;
	void SetClearable(bool bValue);

	// Other
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iStartingLocationWeight;
	int m_iMovementCost;
	int m_iSeeThroughChange;
	int m_iAppearanceProbability;
	int m_iDisappearanceProbability;
	int m_iGrowthProbability;
	int m_iGrowthTerrainType;
	int m_iDefenseModifier;
	int m_iInfluenceCost;
	int m_iAdvancedStartRemoveCost;
	int m_iTurnDamage;
	int m_iFirstFinderGold;
	int m_iInBorderHappiness;
	int m_iOccurrenceFrequency;
	int m_iAdjacentUnitFreePromotion;

	bool m_bYieldNotAdditive;
	bool m_bNoCoast;
	bool m_bNoRiver;
	bool m_bNoAdjacent;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiver;
	bool m_bAddsFreshWater;
	bool m_bImpassable;
	bool m_bNoCity;
	bool m_bNoImprovement;
	bool m_bVisibleAlways;
	bool m_bNukeImmune;
	bool m_bRough;
	bool m_bNaturalWonder;

	// Set each time the game is started
	bool m_bClearable;

	int m_iWorldSoundscapeScriptId;

	CvString m_strEffectType;
	int m_iEffectProbability;
	CvString m_strEffectTypeTag;		// Effect type for effect macros

	// Arrays
	int* m_piYieldChange;
	int* m_piRiverYieldChange;
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;
	bool* m_pbTerrain;

private:
	CvString m_strArtDefineTag;

private:
	CvFeatureInfo(const CvFeatureInfo&);
	CvFeatureInfo& operator=(const CvFeatureInfo&);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvYieldInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvYieldInfo : public CvBaseInfo
{
public:
	CvYieldInfo();

	int getHillsChange() const;
	int getMountainChange() const;
	int getLakeChange() const;
	int getCityChange() const;
	int getPopulationChangeOffset() const;
	int getPopulationChangeDivisor() const;
	int getMinCity() const;
	int getGoldenAgeYield() const;
	int getGoldenAgeYieldThreshold() const;
	int getGoldenAgeYieldMod() const;
	int getAIWeightPercent() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iHillsChange;
	int m_iMountainChange;
	int m_iLakeChange;
	int m_iCityChange;
	int m_iPopulationChangeOffset;
	int m_iPopulationChangeDivisor;
	int m_iMinCity;
	int m_iGoldenAgeYield;
	int m_iGoldenAgeYieldThreshold;
	int m_iGoldenAgeYieldMod;
	int m_iAIWeightPercent;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvTerrainInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTerrainInfo : public CvBaseInfo
{
public:

	CvTerrainInfo();
	virtual ~CvTerrainInfo();

	int getMovementCost() const;
	int getSeeFromLevel() const;
	int getSeeThroughLevel() const;
	int getBuildModifier() const;
	int getDefenseModifier() const;
	int getInfluenceCost() const;

	bool isWater() const;
	bool isImpassable() const;
	bool isFound() const;
	bool isFoundCoast() const;
	bool isFoundFreshWater() const;

	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szTag);

	int getWorldSoundscapeScriptId() const;
	const char* getEffectTypeTag() const;

	// Arrays
	int getYield(int i) const;
	int getRiverYieldChange(int i) const;
	int getHillsYieldChange(int i) const;
	int get3DAudioScriptFootstepIndex(int i) const;

	// Other
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iMovementCost;
	int m_iSeeFromLevel;
	int m_iSeeThroughLevel;
	int m_iBuildModifier;
	int m_iDefenseModifier;
	int m_iInfluenceCost;

	bool m_bWater;
	bool m_bImpassable;
	bool m_bFound;
	bool m_bFoundCoast;
	bool m_bFoundFreshWater;

	int m_iWorldSoundscapeScriptId;

	// Arrays
	int* m_piYields;
	int* m_piRiverYieldChange;
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;

	CvString m_strEffectTypeTag;		// Effect type for effect macros

private:
	CvString m_strArtDefineTag;

private:
	CvTerrainInfo(const CvTerrainInfo&);
	CvTerrainInfo& operator=(const CvTerrainInfo&);

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvInterfaceModeInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInterfaceModeInfo :	public CvHotKeyInfo
{
public:
	CvInterfaceModeInfo();

	int getCursorIndex() const;
	int getMissionType() const;

	bool getVisible() const;
	bool getHighlightPlot() const;
	bool getSelectType() const;
	bool getSelectAll() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iCursorIndex;
	int m_iMissionType;

	bool m_bVisible;
	bool m_bHighlightPlot;
	bool m_bSelectType;
	bool m_bSelectAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvLeaderHeadInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeaderHeadInfo : public CvBaseInfo
{
public:
	CvLeaderHeadInfo();
	virtual ~CvLeaderHeadInfo();

	int GetVictoryCompetitiveness() const;
	int GetWonderCompetitiveness() const;
	int GetMinorCivCompetitiveness() const;
	int GetBoldness() const;
	int GetDiploBalance() const;
	int GetWarmongerHate() const;
	int GetDenounceWillingness() const;
	int GetDoFWillingness() const;
	int GetLoyalty() const;
	int GetNeediness() const;
	int GetForgiveness() const;
	int GetChattiness() const;
	int GetMeanness() const;

	int GetMajorCivApproachBias(int i) const;
	int GetMinorCivApproachBias(int i) const;

	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);

	// Arrays
	bool hasTrait(int i) const;
	int getFlavorValue(int i) const;

	// Other
	const char* getLeaderHead() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iVictoryCompetitiveness;
	int m_iWonderCompetitiveness;
	int m_iMinorCivCompetitiveness;
	int m_iBoldness;
	int m_iDiploBalance;
	int m_iWarmongerHate;
	int m_iDenounceWillingness;
	int m_iDoFWillingness;
	int m_iLoyalty;
	int m_iNeediness;
	int m_iForgiveness;
	int m_iChattiness;
	int m_iMeanness;

	int* m_piMajorCivApproachBiases;
	int* m_piMinorCivApproachBiases;

	CvString m_strArtDefineTag;

	// Arrays
	bool* m_pbTraits;

	int* m_piFlavorValue;

private:
	CvLeaderHeadInfo(const CvLeaderHeadInfo&);
	CvLeaderHeadInfo& operator=(const CvLeaderHeadInfo&);

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvWorldInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldInfo :	public CvBaseInfo
{
public:
	CvWorldInfo();
	int getDefaultPlayers() const;
	int getDefaultMinorCivs() const;
	int getFogTilesPerBarbarianCamp() const;
	int getNumNaturalWonders() const;
	int getUnitNameModifier() const;
	int getTargetNumCities() const;
	int getNumFreeBuildingResources() const;
	int getBuildingClassPrereqModifier() const;
	int getMaxConscriptModifier() const;
	int getGridWidth() const;
	int getGridHeight() const;
	int getMaxActiveReligions() const;
	int getTerrainGrainChange() const;
	int getFeatureGrainChange() const;
	int getResearchPercent() const;
	int getAdvancedStartPointsMod() const;
	int getNumCitiesUnhappinessPercent() const;
	int GetNumCitiesPolicyCostMod() const;

	static CvWorldInfo CreateCustomWorldSize(const CvWorldInfo& kTemplate, int iWidth, int iHeight);
	static CvWorldInfo CreateCustomWorldSize(const CvWorldInfo& kTemplate, int iWidth, int iHeight, int iPlayers, int iMinorCivs);

	//Cached result overrides.
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);
	virtual void readFrom(FDataStream&);
	virtual void writeTo(FDataStream&) const;
	virtual bool operator==(const CvWorldInfo&) const;
	bool operator != (const CvWorldInfo&) const;

	// Class specific

	// A special method to read the serialized info before there was any versioning
	void readFromVersion0(FDataStream&);


protected:
	int m_iDefaultPlayers;
	int m_iDefaultMinorCivs;
	int m_iFogTilesPerBarbarianCamp;
	int m_iNumNaturalWonders;
	int m_iUnitNameModifier;
	int m_iTargetNumCities;
	int m_iNumFreeBuildingResources;
	int m_iBuildingClassPrereqModifier;
	int m_iMaxConscriptModifier;
	int m_iGridWidth;
	int m_iGridHeight;
	int m_iMaxActiveReligions;
	int m_iTerrainGrainChange;
	int m_iFeatureGrainChange;
	int m_iResearchPercent;
	int m_iAdvancedStartPointsMod;
	int m_iNumCitiesUnhappinessPercent;
	int m_iNumCitiesPolicyCostMod;
};

FDataStream& operator<<(FDataStream&, const CvWorldInfo&);
FDataStream& operator>>(FDataStream&, CvWorldInfo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvClimateInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvClimateInfo: public CvBaseInfo
{
public:
	CvClimateInfo();

	int getDesertPercentChange() const
	{
		return m_iDesertPercentChange;
	}

	int getJungleLatitude() const
	{
		return m_iJungleLatitude;
	}

	int getHillRange() const
	{
		return m_iHillRange;
	}

	int getMountainPercent() const
	{
		return m_iMountainPercent;
	}

	float getSnowLatitudeChange() const
	{
		return m_fDesertBottomLatitudeChange;
	}

	float getTundraLatitudeChange() const
	{
		return m_fTundraLatitudeChange;
	}

	float getGrassLatitudeChange() const
	{
		return m_fGrassLatitudeChange;
	}

	float getDesertBottomLatitudeChange() const
	{
		return m_fDesertBottomLatitudeChange;
	}

	float getDesertTopLatitudeChange() const
	{
		return m_fDesertTopLatitudeChange;
	}

	float getIceLatitude() const
	{
		return m_fIceLatitude;
	}

	float getRandIceLatitude() const
	{
		return m_fRandIceLatitude;
	}

	//Cached result overrides.
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	virtual void readFrom(FDataStream&);
	virtual void writeTo(FDataStream&) const;

protected:
	int m_iDesertPercentChange;
	int m_iJungleLatitude;
	int m_iHillRange;
	int m_iMountainPercent;

	float m_fSnowLatitudeChange;
	float m_fTundraLatitudeChange;
	float m_fGrassLatitudeChange;
	float m_fDesertBottomLatitudeChange;
	float m_fDesertTopLatitudeChange;
	float m_fIceLatitude;
	float m_fRandIceLatitude;
};

FDataStream& operator<<(FDataStream&, const CvClimateInfo&);
FDataStream& operator>>(FDataStream&, CvClimateInfo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSeaLevelInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSeaLevelInfo :	public CvBaseInfo
{
public:
	CvSeaLevelInfo();

	int getSeaLevelChange() const
	{
		return m_iSeaLevelChange;
	}

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	virtual void readFrom(FDataStream&);
	virtual void writeTo(FDataStream&) const;

private:
	int m_iSeaLevelChange;
};

FDataStream& operator<<(FDataStream&, const CvSeaLevelInfo&);
FDataStream& operator>>(FDataStream&, CvSeaLevelInfo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvProcessInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProcessInfo : public CvBaseInfo
{
public:
	CvProcessInfo();
	virtual ~CvProcessInfo();

	int getTechPrereq() const;

	// Arrays
	int getProductionToYieldModifier(int i) const;
	int GetFlavorValue(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iTechPrereq;

	// Arrays
	int* m_paiProductionToYieldModifier;
	int* m_paiFlavorValue;

private:
	CvProcessInfo(const CvProcessInfo&);
	CvProcessInfo& operator=(const CvProcessInfo&);

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvVoteInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteInfo : public CvBaseInfo
{
public:
	CvVoteInfo();
	virtual ~CvVoteInfo();

	int getPopulationThreshold() const;
	int getMinVoters() const;
	bool isSecretaryGeneral() const;
	bool isVictory() const;
	bool isNoNukes() const;
	bool isCityVoting() const;
	bool isCivVoting() const;
	bool isDefensivePact() const;
	bool isOpenBorders() const;
	bool isForcePeace() const;
	bool isForceNoTrade() const;
	bool isForceWar() const;
	bool isAssignCity() const;
	bool isVoteSourceType(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iPopulationThreshold;
	int m_iMinVoters;
	bool m_bSecretaryGeneral;
	bool m_bVictory;
	bool m_bNoNukes;
	bool m_bCityVoting;
	bool m_bCivVoting;
	bool m_bDefensivePact;
	bool m_bOpenBorders;
	bool m_bForcePeace;
	bool m_bForceNoTrade;
	bool m_bForceWar;
	bool m_bAssignCity;
	bool* m_abVoteSourceTypes;

private:
	CvVoteInfo(const CvVoteInfo&);
	CvVoteInfo& operator=(const CvVoteInfo&);

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvEntityEventInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEntityEventInfo : public CvBaseInfo
{
public:
	CvEntityEventInfo();

	AnimationPathTypes getAnimationPathType(int iIndex = 0) const;
	int getAnimationPathCount() const;

	bool getUpdateFormation() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

private:
	std::vector<AnimationPathTypes>	m_vctAnimationPathType;
	bool							m_bUpdateFormation;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvEraInfo
//  DESC:   Used to manage different types of Art Styles
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEraInfo : public CvBaseInfo
{
public:
	CvEraInfo();
	virtual ~CvEraInfo();

	int getStartingUnitMultiplier() const;
	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
	int getResearchAgreementCost() const;
	int getEmbarkedUnitDefense() const;
	int getStartingGold() const;
	int getStartingCulture() const;
	int getFreePopulation() const;
	int getLaterEraBuildingConstructMod() const;
	int getStartPercent() const;
	int getBuildingMaintenancePercent() const;
	int getGrowthPercent() const;
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getCreatePercent() const;
	int getResearchPercent() const;
	int getBuildPercent() const;
	int getImprovementPercent() const;
	int getGreatPeoplePercent() const;
	int getEventChancePerTurn() const;
	int getSpiesGrantedForPlayer() const;
	int getSpiesGrantedForEveryone() const;
	int getFaithCostMultiplier() const;
	int getDiploEmphasisReligion() const;
	int getDiplpEmphasisLatePolicies() const;

	const char* GetCityBombardEffectTag() const;
	uint GetCityBombardEffectTagHash() const;

	const char* getAudioUnitVictoryScript() const;
	const char* getAudioUnitDefeatScript() const;

	int GetNumEraVOs() const;
	const char* GetEraVO(int iIndex);

	const char* getArtPrefix() const;

	bool isNoGoodies() const;
	bool isNoBarbUnits() const;
	bool isNoReligion() const;

	// Arrays


	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iStartingUnitMultiplier;
	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
	int m_iResearchAgreementCost;
	int m_iEmbarkedUnitDefense;
	int m_iStartingGold;
	int m_iStartingCulture;
	int m_iFreePopulation;
	int m_iLaterEraBuildingConstructMod;
	int m_iStartPercent;
	int m_iBuildingMaintenancePercent;
	int m_iGrowthPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iCreatePercent;
	int m_iResearchPercent;
	int m_iBuildPercent;
	int m_iImprovementPercent;
	int m_iGreatPeoplePercent;
	int m_iEventChancePerTurn;
	int m_iSpiesGrantedForPlayer;
	int m_iSpiesGrantedForEveryone;
	int m_iFaithCostMultiplier;
	int m_iDiploEmphasisReligion;
	int m_iDiplpEmphasisLatePolicies;

	CvString m_strCityBombardEffectTag;
	CvString m_strAudioUnitVictoryScript;
	CvString m_strAudioUnitDefeatScript;
	CvString m_strArtPrefix;

	uint m_uiCityBombardEffectTagHash;

	bool m_bNoGoodies;
	bool m_bNoBarbUnits;
	bool m_bNoReligion;

	std::vector<CvString> m_vEraVOs;

private:
	CvEraInfo(const CvEraInfo&);
	CvEraInfo& operator=(const CvEraInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvColorInfo
//  DESC:   Used to manage different types of Art Styles
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvColorInfo : public CvBaseInfo
{
public:
	const CvColorA& GetColor() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	CvColorA m_Color;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvPlayerColorInfo
//  DESC:   Used to manage different types of Art Styles
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerColorInfo : public CvBaseInfo
{
public:
	CvPlayerColorInfo();

	int GetColorTypePrimary() const;
	int GetColorTypeSecondary() const;
	int GetColorTypeText() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtilty);

private:
	int m_iColorTypePrimary;
	int m_iColorTypeSecondary;
	int m_iColorTypeText;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvGameOptionInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameOptionInfo : public CvBaseInfo
{
public:
	CvGameOptionInfo();

	bool getDefault() const;
	bool getVisible() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

private:
	bool m_bDefault;
	bool m_bVisible;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvMPOptionInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMPOptionInfo : public CvBaseInfo
{
public:
	CvMPOptionInfo();

	bool getDefault() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvPlayerOptionInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerOptionInfo : public CvBaseInfo
{
public:
	CvPlayerOptionInfo();

	bool getDefault() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

private:
	bool m_bDefault;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvVoteSourceInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteSourceInfo : public CvBaseInfo
{
public:
	CvVoteSourceInfo();
	virtual ~CvVoteSourceInfo();

	int getVoteInterval() const;
	int getFreeSpecialist() const;
	int getPolicy() const;
	const CvString& getPopupText() const;
	const CvString& getSecretaryGeneralText() const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iVoteInterval;
	int m_iFreeSpecialist;
	int m_iPolicy;

	CvString m_strPopupText;
	CvString m_strSecretaryGeneralText;

private:
	CvVoteSourceInfo(const CvVoteSourceInfo&);
	CvVoteSourceInfo& operator=(const CvVoteSourceInfo&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvDomainInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef CvBaseInfo CvDomainInfo;

#endif
