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
#if defined(MOD_BALANCE_CORE)
		m_bOcean(false),
#endif
		m_iExposureScore(100),
		m_iPathLength(0)
	{
	}

	bool operator==(const CvMilitaryTarget& rhs) const
	{
		return m_pTargetCity == rhs.m_pTargetCity && m_pMusterCity == rhs.m_pMusterCity;
	}

	CvCity* m_pTargetCity;
	CvCity* m_pMusterCity;
	int m_iExposureScore; //expected value is 100
	int m_iPathLength;
	bool m_bAttackBySea;
#if defined(MOD_BALANCE_CORE)
	bool m_bOcean;
	bool m_bNoLandPath;
#endif
};

#if defined(MOD_BALANCE_CORE_MILITARY)
struct SCachedTarget
{
	SCachedTarget() :
		iTargetCity(0),
		iMusterCity(0),
		bAttackBySea(false),
		bOcean(false),
		bNoLandPath(false),
		iScore(0),
		iTurnChosen(0)
	{
	}

	int iTargetCity;
	int iMusterCity;
	bool bAttackBySea;
	bool bOcean;
	bool bNoLandPath;
	int iScore;
	int iTurnChosen;
};

typedef std::map<PlayerTypes,std::map<AIOperationTypes,SCachedTarget>> CachedTargetsMap;
typedef std::map<CvCity*, std::map<CvCity*, int>> CachedDistancesMap;

#endif

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
	bool RequestBullyingOperation(PlayerTypes eEnemy);
	CvAIOperation* GetSneakAttackOperation(PlayerTypes eEnemy);
	CvAIOperation* GetShowOfForceOperation(PlayerTypes eEnemy);
	CvAIOperation* GetBasicAttackOperation(PlayerTypes eEnemy);
	CvAIOperation* GetCityStateAttackOperation(PlayerTypes eEnemy);
	CvAIOperation* GetPureNavalAttackOperation(PlayerTypes eEnemy);

	// Emergency purchases
	CvUnit* BuyEmergencyUnit(UnitAITypes eUnitType, CvCity* pCity);
	bool BuyEmergencyBuilding(CvCity* pCity);

	// Finding best cities to target
#if defined(MOD_BALANCE_CORE_MILITARY)
	bool HaveValidAttackTarget(PlayerTypes eEnemy);
	bool IsCurrentAttackTarget(CvCity* pCity);
	void ClearCachedTargets();
	CvMilitaryTarget FindBestAttackTargetCached(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore = NULL);
	CvMilitaryTarget FindBestAttackTargetGlobal(AIOperationTypes eAIOperationType, int* piWinningScore = NULL, bool bCheckWar = false);

	int GetCachedAttackTargetWaterDistance(CvCity* pCity, CvCity* pOtherCity, int iMaxTurnsAway);
	int GetCachedAttackTargetLandDistance(CvCity* pCity, CvCity* pOtherCity, int iMaxTurnsAway);
	void ResetDistanceCaches();
	bool PathIsGood(const SPath& path, PlayerTypes eIntendedEnemy);
	bool IsPlayerValid(PlayerTypes eOtherPlayer);
#endif
	CvMilitaryTarget FindBestAttackTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore = NULL);
	void CheckApproachFromLandAndSea(CvMilitaryTarget& target, AIOperationTypes eAIOperationType, int iMaxTurnsAway);
	int ScoreTarget(CvMilitaryTarget& target, AIOperationTypes eAIOperationType);
	CityAttackApproaches EvaluateMilitaryApproaches(CvCity* pCity, bool bAttackByLand, bool bAttackBySea);

	// Accessors to provide military data to other AI subsystems
	ThreatTypes GetHighestThreat();
	int GetThreatTotal() const
	{
		return m_iTotalThreatWeight;
	}
	int GetBarbarianThreatTotal();
	int GetThreatWeight(ThreatTypes eThreat);
#if defined(MOD_BALANCE_CORE)
	int GetNumberCivsAtWarWith(bool bMinor = true) const;
#else
	int GetNumberCivsAtWarWith() const;
#endif
	int GetRecommendedMilitarySize() const
	{
		return m_iRecommendedMilitarySize + m_iRecNavySize;
	};
#if defined(MOD_BALANCE_CORE)
	int GetLandReservesAvailable() const
	{
		//don't put all our units in armies. however, we may fall below our mandatory minimum
		return max(0, m_iNumLandUnits - m_iNumLandUnitsInArmies - m_iMandatoryReserveSize/2);
	};
#else
	int GetMandatoryReserveSize() const
	{
		return m_iMandatoryReserveSize;
	};
	int GetLandReservesAvailable() const
	{
		return (m_iNumLandUnits - m_iNumLandUnitsInArmies - GetMandatoryReserveSize());
	};
#endif

	CvCity* GetMostThreatenedCity(bool bIncludeFutureThreats = true, bool bCoastalOnly = false);
	vector<CvCity*> GetThreatenedCities(bool bIncludeFutureThreats=true, bool bCoastalOnly = false);

	int GetPercentOfRecommendedMilitarySize() const;
	int GetPowerOfStrongestBuildableUnit(DomainTypes eDomain);
	bool HasAirforce() const
	{
		return m_iNumAirUnits > 0;
	};
	CvUnit* FindBestUnitToScrap(bool bLand, bool bDeficitForcedDisband, int& iScore);
	ArmyType GetArmyBeingBuilt() const
	{
		return m_eArmyTypeBeingBuilt;
	};
	UnitTypes GetUnitForArmy(CvCity* pCity) const;
#if defined(MOD_AI_SMART_AIR_TACTICS)
	int GetMaxPossibleInterceptions(CvPlot* pCenterPlot, bool bCountPercents = false) const;
#endif
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

	int GetNumberOfTimesSettlerBuildSkippedOver() const
	{
		return m_iNumberOfTimesSettlerBuildSkippedOver;
	}
	void BumpNumberOfTimesSettlerBuildSkippedOver()
	{
		++m_iNumberOfTimesSettlerBuildSkippedOver;
	}
	void ResetNumberOfTimesSettlerBuildSkippedOver()
	{
		m_iNumberOfTimesSettlerBuildSkippedOver = 0;
	}

	// Public logging functions
	void LogDeclarationOfWar(PlayerTypes eOpponent);
	void LogCivilizationDestroyed();
	void LogCityCaptured(CvCity* pCity, PlayerTypes eOldOwner);
	void LogCityRazed(CvCity* pCity, PlayerTypes eOldOwner);
	void LogPeace(PlayerTypes eOpponent);
	void LogPeace(TeamTypes eOpponentTeam);
	void LogVassalFailure(TeamTypes eOpponentTeam);
	void LogDeficitScrapUnit(CvUnit* pUnit);
	void LogMilitarySummaryMessage(const CvString& strMsg);

#if defined(MOD_BALANCE_CORE)
	WarTypes GetWarType(PlayerTypes ePlayer = NO_PLAYER);
	void UpdateWarType();
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
	int GetNumNavalAttacksRequested() const
	{
		return m_iNumNavalAttacksRequested;
	};
	int GetNumLandAttacksRequested() const
	{
		return m_iNumLandAttacksRequested;
	};
	int GetRecommendLandArmySize() const
	{
		return m_iRecommendedMilitarySize;
	};
	int GetRecommendNavySize() const
	{
		return m_iRecNavySize;
	};
	int GetNumLandUnits() const
	{
		return m_iNumLandUnits;
	};
	int GetNumNavalUnits() const
	{
		return m_iNumNavalUnits;
	};
	int GetNumAAUnits() const
	{
		return m_iNumAntiAirUnits;
	};

	void SetNumFreeCarrier(int iValue);
	int GetNumFreeCarrier() const;

	void SetNumFreeCargo(int iValue);
	int GetNumFreeCargo() const;
#endif
#if defined(MOD_BALANCE_CORE)
	void SetupDefenses(PlayerTypes ePlayer);
#endif
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
#if defined(MOD_BALANCE_CORE)
	void DoNuke(PlayerTypes ePlayer);
	void DoBarbs();
	void CheckLandDefenses(PlayerTypes ePlayer, CvCity* pThreatenedCity);
	void CheckSeaDefenses(PlayerTypes ePlayer, CvCity* pThreatenedCity);
	void DoLandAttacks(PlayerTypes ePlayer);
	void DoSeaAttacks(PlayerTypes ePlayer);
	void DoMinorCivAttacks(PlayerTypes ePlayer);
	int GetEnemyLandValue(PlayerTypes ePlayer, CvMilitaryTarget& besttarget, int iGlobalTargetScore);
	int GetEnemySeaValue(PlayerTypes ePlayer, CvMilitaryTarget& besttarget, int iGlobalTargetScore);
#endif
	void MakeEmergencyPurchases();
	void RequestImprovements();
	void DisbandObsoleteUnits(int iMaxUnits=2);
	bool IsAttackReady(MultiunitFormationTypes eFormation, AIOperationTypes eOperationType) const;

	// Logging functions
	void LogStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue);
	void LogWarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState);
	void LogMilitaryStatus();
	void LogAvailableForces();
	void LogScrapUnit(CvUnit* pUnit, bool bDeficit, bool bSupply);
	void LogGiftUnit(CvUnit* pUnit, bool bDeficit, bool bSupply);
	void LogChosenTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvMilitaryTarget& target);
	CvString GetLogFileName(CvString& playerName, bool bSummary=false) const;

	CvPlayer* m_pPlayer;
	CvDiplomacyAI* m_pDiplomacyAI;
	CvMilitaryAIStrategyXMLEntries* m_pAIStrategies;
	bool* m_pabUsingStrategy;
	int* m_paiTurnStrategyAdopted;
	int* m_aiTempFlavors;
	int* m_aiWarFocus;

	// Archived state of threats/wars from last turn
	int* m_paeLastTurnWarState;
	int* m_paeLastTurnMilitaryThreat;
	int* m_paeLastTurnMilitaryStrength;
	int* m_paeLastTurnTargetValue;

	// Internal calculated values - must be serialized
	int m_iTotalThreatWeight;
	ArmyType m_eArmyTypeBeingBuilt;
	int m_iNumberOfTimesOpsBuildSkippedOver;
	int m_iNumberOfTimesSettlerBuildSkippedOver;

#if defined(MOD_BALANCE_CORE_MILITARY)
	CachedTargetsMap m_cachedTargets;
	CachedDistancesMap m_cachedWaterDistances; //not serialized. regenerated every turn
	CachedDistancesMap m_cachedLandDistances; //not serialized. regenerated every turn
#endif

	// Data recomputed each turn (no need to serialize)
	int m_iNumLandUnits;
	int m_iNumRangedLandUnits;
	int m_iNumMobileLandUnits;
	int m_iNumMeleeLandUnits;
	int m_iNumNavalUnits;
	int m_iNumLandUnitsInArmies;
	int m_iNumNavalUnitsInArmies;
	int m_iRecNavySize;
	int m_iNumAirUnits;
	int m_iNumAntiAirUnits;
	int m_iBarbarianCampCount;
	int m_iVisibleBarbarianCount;
	int m_iRecommendedMilitarySize;
	int m_iMandatoryReserveSize;
	int m_iNumLandAttacksRequested;
	int m_iNumNavalAttacksRequested;
#if defined(MOD_BALANCE_CORE)
	int m_iFreeCarrier;
	int m_iFreeCargo;
#endif
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
#if defined(MOD_BALANCE_CORE)
bool IsTestStrategy_AtWar(CvPlayer* pPlayer, bool bMinor = true);
#else
bool IsTestStrategy_AtWar(CvPlayer* pPlayer);
#endif
bool IsTestStrategy_MinorCivGeneralDefense();
bool IsTestStrategy_MinorCivThreatElevated(CvPlayer* pPlayer);
bool IsTestStrategy_MinorCivThreatCritical(CvPlayer* pPlayer);
bool IsTestStrategy_EradicateBarbarians(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount);
#if defined(MOD_BALANCE_CORE)
bool IsTestStrategy_EradicateBarbariansCritical(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount);
#endif
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
int ComputeRecommendedNavySize(CvPlayer* pPlayer, int iMinSize);
int NumberOfFillableSlots(CvPlayer* pPlayer, PlayerTypes pEnemy, MultiunitFormationTypes formation, bool bRequiresNavalMoves=false, bool bMustBeDeepWaterNaval=false, CvPlot* pMuster=NULL, CvPlot* pTarget=NULL, int* piNumberSlotsRequired=NULL, int* piNumberLandReservesUsed=NULL);
UnitAITypes FirstSlotCityCanFill(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves, bool bAtCoastalCity, bool bSecondaryUnit);
MultiunitFormationTypes GetCurrentBestFormationTypeForCityAttack();
MultiunitFormationTypes GetCurrentBestFormationTypeForNavalAttack();
MultiunitFormationTypes GetCurrentBestFormationTypeForPureNavalAttack();
CvPlot* GetCoastalPlotNearPlot(CvPlot *pTarget, bool bCheckTeam = false);
}

#endif //CIV5_MILITARY_AI_H