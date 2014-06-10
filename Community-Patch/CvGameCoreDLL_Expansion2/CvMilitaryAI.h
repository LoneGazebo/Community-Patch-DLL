/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_MILITARY_AI_H
#define CIV5_MILITARY_AI_H

enum DefenseState
{
    NO_DEFENSE_STATE = -1,
    DEFENSE_STATE_ENOUGH,
    DEFENSE_STATE_NEUTRAL,
    DEFENSE_STATE_NEEDED,
    DEFENSE_STATE_CRITICAL,
};

enum ArmyType
{
    NO_ARMY_TYPE = -1,
    ARMY_TYPE_LAND,
    ARMY_TYPE_NAVAL_INVASION,
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMilitaryAIStrategyXMLEntry
//!  \brief		A single entry in the AI strategy XML file
//
//!  Key Attributes:
//!  - Populated from XML\???? (not sure what path/name you want)
//!  - Array of these contained in CvAIStretegies class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMilitaryAIStrategyXMLEntry: public CvBaseInfo
{
public:
	CvMilitaryAIStrategyXMLEntry();
	virtual ~CvMilitaryAIStrategyXMLEntry();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions
	int GetPlayerFlavorValue(int i) const;
	int GetCityFlavorValue(int i) const;

	int GetWeightThreshold() const;
	int GetPersonalityFlavorThresholdMod(int i) const;

	bool IsNoMinorCivs() const;
	bool IsOnlyMinorCivs() const;
	bool IsDontUpdateCityFlavors() const;
	bool RequiresCitySpecializationUpdate() const;

	int GetTechPrereq() const;
	int GetTechObsolete() const;
	int GetMinimumNumTurnsExecuted() const;
	int GetCheckTriggerTurnCount() const;
	int GetFirstTurnExecuted() const;

	AdvisorTypes GetAdvisor() const;
	const char* GetAdvisorCounselText() const;
	int GetAdvisorCounselImportance() const;

private:
	int* m_piPlayerFlavorValue;
	int* m_piCityFlavorValue;

	int m_iWeightThreshold;
	int* m_piPersonalityFlavorThresholdMod;

	bool m_bNoMinorCivs;
	bool m_bOnlyMinorCivs;
	bool m_bUpdateCitySpecializations;

	int m_iTechPrereq;
	int m_iTechObsolete;
	int m_iMinimumNumTurnsExecuted;
	int m_iCheckTriggerTurnCount;
	int m_iFirstTurnExecuted;

	AdvisorTypes m_eAdvisor;
	CvString m_strAdvisorCounselText;
	int m_iAdvisorCounselImportance;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMilitaryAIStrategyXMLEntries
//!  \brief		Game-wide information about possible AI strategies
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\???? (not sure what path/name you want)
//! - Contains an array of CvAIStrategyXMLEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvPlayerAIStrategy class (which stores the AI strategy state for 1 player)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMilitaryAIStrategyXMLEntries
{
public:
	CvMilitaryAIStrategyXMLEntries(void);
	~CvMilitaryAIStrategyXMLEntries(void);

	// Accessor functions
	std::vector<CvMilitaryAIStrategyXMLEntry*>& GetMilitaryAIStrategyEntries();
	int GetNumMilitaryAIStrategies();
	_Ret_maybenull_ CvMilitaryAIStrategyXMLEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvMilitaryAIStrategyXMLEntry*> m_paAIStrategyEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:     CvMilitaryTarget
//!  \brief		A possible operation target (and muster city) for evaluation
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvMilitaryTarget
{
	CvMilitaryTarget() :
		m_pTargetCity(NULL),
		m_pMusterCity(NULL),
		m_bAttackBySea(false),
		iMusterNearbyUnitPower(0),
		iTargetNearbyUnitPower(0),
		m_iPathLength(0)
	{
	}

	CvCity* m_pTargetCity;
	CvCity* m_pMusterCity;
	int iTargetNearbyUnitPower;
	int iMusterNearbyUnitPower;
	int m_iPathLength;
	bool m_bAttackBySea;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMilitaryAI
//!  \brief		Information about the AI military strategies of a single player
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvPlayer class
//!  - One instance for each civ (player or AI)
//!  - Accessed by any class that needs to check a civ's AI strategy state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum CityAttackApproaches
{
    ATTACK_APPROACH_NONE,
    ATTACK_APPROACH_RESTRICTED,
    ATTACK_APPROACH_LIMITED,
    ATTACK_APPROACH_NEUTRAL,
    ATTACK_APPROACH_OPEN,
    ATTACK_APPROACH_UNRESTRICTED,
};

class CvMilitaryAI
{
public:
	CvMilitaryAI(void);
	~CvMilitaryAI(void);
	void Init(CvMilitaryAIStrategyXMLEntries* pAIStrategies, CvPlayer* pPlayer, CvDiplomacyAI* pDiplomacyAI);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	CvPlayer* GetPlayer();
	CvMilitaryAIStrategyXMLEntries* GetMilitaryAIStrategies();

	bool IsUsingStrategy(MilitaryAIStrategyTypes eStrategy);
	void SetUsingStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue);
	int GetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy);
	void SetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy, int iValue);

	void ResetCounters()
	{
		m_iNumLandAttacksRequested = 0;
		m_iNumNavalAttacksRequested = 0;
		m_eArmyTypeBeingBuilt =  NO_ARMY_TYPE;
	}
	void DoTurn();

	DefenseState GetLandDefenseState() const
	{
		return m_eLandDefenseState;
	};
	DefenseState GetNavalDefenseState() const
	{
		return m_eNavalDefenseState;
	};

	// Requests for military operations
	bool RequestSneakAttack(PlayerTypes eEnemy);
	bool RequestShowOfForce(PlayerTypes eEnemy);
	bool RequestBasicAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild);
	bool RequestPureNavalAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild);
	bool RequestCityStateAttack(PlayerTypes eEnemy);
	bool RequestPillageAttack(PlayerTypes eEnemy);
	bool RequestNukeAttack(PlayerTypes eEnemy);
	bool RequestSpecificAttack(CvMilitaryTarget kTarget, int iNumUnitsWillingToBuild);
	CvAIOperation* GetSneakAttackOperation(PlayerTypes eEnemy);
	CvAIOperation* GetShowOfForceOperation(PlayerTypes eEnemy);
	CvAIOperation* GetBasicAttackOperation(PlayerTypes eEnemy);
	CvAIOperation* GetCityStateAttackOperation(PlayerTypes eEnemy);
	CvAIOperation* GetNukeAttackOperation(PlayerTypes eEnemy);
	CvAIOperation* GetPureNavalAttackOperation(PlayerTypes eEnemy);

	// Emergency purchases
	CvUnit* BuyEmergencyUnit(UnitAITypes eUnitType, CvCity* pCity);
	bool BuyEmergencyBuilding(CvCity* pCity);

	// Finding best cities to target
	CvMilitaryTarget FindBestAttackTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore = NULL);
	void ShouldAttackBySea(PlayerTypes eEnemy, CvMilitaryTarget& target);
	int ScoreTarget(CvMilitaryTarget& target, AIOperationTypes eAIOperationType);
	CityAttackApproaches EvaluateMilitaryApproaches(CvCity* pCity, bool bAttackByLand, bool bAttackBySea);
	CvCity* GetNearestCoastalCity(PlayerTypes eEnemy) const;
	CvPlot* GetCoastalPlotAdjacentToTarget(CvPlot *pTarget, CvArmyAI *pArmy) const;

	// Accessors to provide military data to other AI subsystems
	ThreatTypes GetHighestThreat();
	int GetThreatTotal() const
	{
		return m_iTotalThreatWeight;
	}
	int GetBarbarianThreatTotal();
	int GetThreatWeight(ThreatTypes eThreat);
	int GetNumberCivsAtWarWith() const;
	int GetRecommendedMilitarySize() const
	{
		return m_iRecommendedMilitarySize;
	};
	int GetMandatoryReserveSize() const
	{
		return m_iMandatoryReserveSize;
	};
	int GetLandReservesAvailable() const
	{
		return (m_iNumLandUnits - m_iNumLandUnitsInArmies - GetMandatoryReserveSize());
	};
	CvCity* GetMostThreatenedCity(int iIndex = 0); // pass in 0 for the most threatened city, 1 for the second most threatened, 2 for the third, etc.
	int GetPercentOfRecommendedMilitarySize() const;
	int GetPowerOfStrongestBuildableUnit(DomainTypes eDomain);
	bool HasAirforce() const
	{
		return m_iNumAirUnits > 0;
	};
	UnitHandle FindBestUnitToScrap(bool bLand, bool bDeficitForcedDisband, int& iScore);
	ArmyType GetArmyBeingBuilt() const
	{
		return m_eArmyTypeBeingBuilt;
	};
	UnitTypes GetUnitForArmy(CvCity* pCity) const;
	bool WillAirUnitRebase(CvUnit* pUnit) const;
	int GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int iRange, bool bCountFighters, bool bCountBombers) const;
	CvPlot *GetBestAirSweepTarget(CvUnit* pFighter) const;

	int GetNumberOfTimesOpsBuildSkippedOver() const
	{
		return m_iNumberOfTimesOpsBuildSkippedOver;
	}
	void BumpNumberOfTimesOpsBuildSkippedOver()
	{
		++m_iNumberOfTimesOpsBuildSkippedOver;
	}
	void ResetNumberOfTimesOpsBuildSkippedOver()
	{
		m_iNumberOfTimesOpsBuildSkippedOver = 0;
	}

	// Public logging functions
	void LogDeclarationOfWar(PlayerTypes eOpponent);
	void LogCivilizationDestroyed();
	void LogCityCaptured(CvCity* pCity, PlayerTypes eOldOwner);
	void LogCityRazed(CvCity* pCity, PlayerTypes eOldOwner);
	void LogPeace(PlayerTypes eOpponent);
	void LogPeace(TeamTypes eOpponentTeam);
	void LogDeficitScrapUnit(UnitHandle pUnit);
	void LogMilitarySummaryMessage(const CvString& strMsg);

private:

	// Functions to process a turn
	void UpdateBaseData();
	void ScanForBarbarians();
	void UpdateThreats();
	void ThreatIncrease(ThreatTypes eNewThreat, ThreatTypes eOldThreat);
	void ThreatDecrease(ThreatTypes eNewThreat, ThreatTypes eOldThreat);
	void UpdateWars();
	void UpdateDefenseState();
	void WarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState);
	void UpdateMilitaryStrategies();
	void UpdateOperations();
	void MakeEmergencyPurchases();
	void MakeOffensivePurchases();
	void RequestImprovements();
	void DisbandObsoleteUnits();
	bool IsAttackReady(MultiunitFormationTypes eFormation, AIOperationTypes eOperationType) const;

	// Logging functions
	void LogStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue);
	void LogWarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState);
	void LogMilitaryStatus();
	void LogAvailableForces();
	void LogScrapUnit(UnitHandle pUnit, bool bDeficit, bool bConquest);
	void LogAttackTargets(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true>& weightedTargetList);
	void LogChosenTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvMilitaryTarget& target);
	CvString GetLogFileName(CvString& playerName, bool bSummary=false) const;

	CvPlayer* m_pPlayer;
	CvDiplomacyAI* m_pDiplomacyAI;
	CvMilitaryAIStrategyXMLEntries* m_pAIStrategies;
	bool* m_pabUsingStrategy;
	int* m_paiTurnStrategyAdopted;
	int* m_aiTempFlavors;

	// Archived state of threats/wars from last turn
	int* m_paeLastTurnWarState;
	int* m_paeLastTurnMilitaryThreat;
	int* m_paeLastTurnMilitaryStrength;
	int* m_paeLastTurnTargetValue;

	// Internal calculated values - must be serialized
	int m_iTotalThreatWeight;
	ArmyType m_eArmyTypeBeingBuilt;
	int m_iNumberOfTimesOpsBuildSkippedOver;

	// Data recomputed each turn (no need to serialize)
	int m_iNumLandUnits;
	int m_iNumRangedLandUnits;
	int m_iNumMobileLandUnits;
	int m_iNumMeleeLandUnits;
	int m_iNumNavalUnits;
	int m_iNumLandUnitsInArmies;
	int m_iNumNavalUnitsInArmies;
	int m_iNumAirUnits;
	int m_iNumAntiAirUnits;
	int m_iBarbarianCampCount;
	int m_iVisibleBarbarianCount;
	int m_iRecommendedMilitarySize;
	int m_iMandatoryReserveSize;
	int m_iNumLandAttacksRequested;
	int m_iNumNavalAttacksRequested;
	DefenseState m_eLandDefenseState;
	DefenseState m_eNavalDefenseState;
};

namespace MilitaryAIHelpers
{
int GetWeightThresholdModifier(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer);

// Functions that check triggers to see if a strategy should be adopted/continued
bool IsTestStrategy_EnoughMilitaryUnits(CvPlayer* pPlayer);
bool IsTestStrategy_EmpireDefense(CvPlayer* pPlayer);
bool IsTestStrategy_EmpireDefenseCritical(CvPlayer* pPlayer);
bool IsTestStrategy_EnoughNavalUnits(CvPlayer* pPlayer);
bool IsTestStrategy_NeedNavalUnits(CvPlayer* pPlayer);
bool IsTestStrategy_NeedNavalUnitsCritical(CvPlayer* pPlayer);
bool IsTestStrategy_WarMobilization(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_AtWar(CvPlayer* pPlayer);
bool IsTestStrategy_MinorCivGeneralDefense();
bool IsTestStrategy_MinorCivThreatElevated(CvPlayer* pPlayer);
bool IsTestStrategy_MinorCivThreatCritical(CvPlayer* pPlayer);
bool IsTestStrategy_EradicateBarbarians(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount);
bool IsTestStrategy_WinningWars(CvPlayer* pPlayer);
bool IsTestStrategy_LosingWars(CvPlayer* pPlayer);
bool IsTestStrategy_EnoughRangedUnits(CvPlayer* pPlayer, int iNumRanged, int iNumMelee);
bool IsTestStrategy_NeedRangedUnits(CvPlayer* pPlayer, int iNumRanged, int iNumMelee);
bool IsTestStrategy_NeedRangedDueToEarlySneakAttack(CvPlayer* pPlayer);
bool IsTestStrategy_EnoughMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee);
bool IsTestStrategy_NeedMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee);
bool IsTestStrategy_EnoughAirUnits(CvPlayer* pPlayer, int iNumAir, int iNumMelee);
bool IsTestStrategy_NeedAirUnits(CvPlayer* pPlayer, int iNumAir, int iNumMelee);
bool IsTestStrategy_NeedANuke(CvPlayer* pPlayer);
bool IsTestStrategy_EnoughAntiAirUnits(CvPlayer* pPlayer, int iNumAA, int iNumMelee);
bool IsTestStrategy_NeedAntiAirUnits(CvPlayer* pPlayer, int iNumAA, int iNumMelee);
bool IsTestStrategy_NeedAirCarriers(CvPlayer* pPlayer);

// Functions that evaluate which operation to launch
int ComputeRecommendedNavySize(CvPlayer* pPlayer);
int NumberOfFillableSlots(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves=false, int* piNumberSlotsRequired=NULL, int* piNumberLandReservesUsed=NULL);
UnitAITypes FirstSlotCityCanFill(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves, bool bAtCoastalCity, bool bSecondaryUnit);
}

#endif //CIV5_MILITARY_AI_H