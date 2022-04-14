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

#include "CvEnumMap.h"

enum DefenseState
{
    NO_DEFENSE_STATE = -1,
    DEFENSE_STATE_ENOUGH,
    DEFENSE_STATE_NEUTRAL,
    DEFENSE_STATE_NEEDED,
    DEFENSE_STATE_CRITICAL,
};

const char* ArmyTypeToString(ArmyType eType);

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
struct CvAttackTarget
{
	CvAttackTarget() : m_armyType(ARMY_TYPE_ANY), m_iMusterPlotIndex(-1), m_iStagingPlotIndex(-1), m_iTargetPlotIndex(-1), m_iPathLength(0), m_iApproachScore(0), m_bPreferred(false) {}
	ArmyType m_armyType;
	int m_iMusterPlotIndex;
	int m_iStagingPlotIndex;
	int m_iTargetPlotIndex;
	int m_iPathLength;
	int m_iApproachScore;
	bool m_bPreferred;

	void SetWaypoints(const SPath&);
	CvPlot* GetMusterPlot() const;
	CvPlot* GetStagingPlot() const;
	CvPlot* GetTargetPlot() const;
	int GetPathLength() const;
	bool IsValid() const;
	bool IsPreferred() const;
};

FDataStream& operator<<(FDataStream&, const CvAttackTarget&);
FDataStream& operator>>(FDataStream&, CvAttackTarget&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMilitaryAI
//!  \brief		Information about the AI military strategies of a single player
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvPlayer class
//!  - One instance for each civ (player or AI)
//!  - Accessed by any class that needs to check a civ's AI strategy state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMilitaryAI
{
public:
	CvMilitaryAI(void);
	~CvMilitaryAI(void);
	void Init(CvMilitaryAIStrategyXMLEntries* pAIStrategies, CvPlayer* pPlayer, CvDiplomacyAI* pDiplomacyAI);
	void Uninit();
	void Reset();
	template<typename MilitaryAI, typename Visitor>
	static void Serialize(MilitaryAI& militaryAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	CvPlayer* GetPlayer();
	CvMilitaryAIStrategyXMLEntries* GetMilitaryAIStrategies();

	bool IsUsingStrategy(MilitaryAIStrategyTypes eStrategy);
	void SetUsingStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue);
	int GetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy);
	void SetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy, int iValue);
	void DoTurn();

	DefenseState GetLandDefenseState() const
	{
		return m_eLandDefenseState;
	};
	DefenseState GetNavalDefenseState() const
	{
		return m_eNavalDefenseState;
	};

	// Requests for specific military operations
	bool RequestCityAttack(PlayerTypes eEnemy, int iNumUnitsWillingToBuild, bool bCareful=true);
	bool RequestPillageAttack(PlayerTypes eEnemy);
	bool RequestNukeAttack(PlayerTypes eEnemy);
	bool RequestBullyingOperation(PlayerTypes eEnemy);

	// Emergency purchases
	CvUnit* BuyEmergencyUnit(UnitAITypes eUnitType, CvCity* pCity);
	bool BuyEmergencyBuilding(CvCity* pCity);

	// Finding best cities to target
	bool HavePossibleAttackTarget(PlayerTypes eEnemy) const;
	bool HavePreferredAttackTarget(PlayerTypes eEnemy) const;
	bool IsPossibleAttackTarget(CvCity* pCity) const;
	bool IsPreferredAttackTarget(CvCity* pCity) const;
	bool IsExposedToEnemy(CvCity* pCity, PlayerTypes eOtherPlayer) const;

	bool IsPlayerValid(PlayerTypes eOtherPlayer) const;

	size_t UpdateAttackTargets();
	int ScoreAttackTarget(const CvAttackTarget& target);

	// Accessors to provide military data to other AI subsystems
	bool ShouldFightBarbarians() const;
	int GetNumberCivsAtWarWith(bool bIncludeMinor = true) const;
	int GetRecommendedMilitarySize() const
	{
		return m_iRecDefensiveLandUnits + m_iRecOffensiveLandUnits + m_iRecOffensiveNavalUnits;
	};

	int GetPowerOfStrongestBuildableUnit(DomainTypes eDomain);
	bool HasAirforce() const
	{
		return m_iNumAirUnits > 0;
	};
	CvUnit* FindUselessShip();
	CvUnit* FindUnitToScrap(DomainTypes eDomain, bool bCheckObsolete, int& iScore, bool bOverSupplyCap, bool bInDeficit);
	bool IsBuildingArmy(ArmyType eType) const;
	UnitTypes GetUnitTypeForArmy(CvCity* pCity) const;
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
	void LogCityCaptured(CvCity* pCity, PlayerTypes eOldOwner);
	void LogCityRazed(CvCity* pCity, PlayerTypes eOldOwner);
	void LogPeace(PlayerTypes eOpponent);
	void LogPeace(TeamTypes eOpponentTeam);
	void LogDeficitScrapUnit(CvUnit* pUnit, bool bGifted);

#if defined(MOD_BALANCE_CORE)
	WarTypes GetWarType(PlayerTypes ePlayer = NO_PLAYER);
	void UpdateWarType();
	void SetupInstantDefenses(PlayerTypes ePlayer);
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
	int GetRecommendLandArmySize() const
	{
		return m_iRecOffensiveLandUnits + m_iRecDefensiveLandUnits;
	};
	int GetRecommendNavySize() const
	{
		return m_iRecOffensiveNavalUnits;
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
	int GetNumFreeCarrier() const
	{
		return m_iNumFreeCarriers;
	}
#endif

private:

	// Functions to process a turn
	void UpdateBaseData();
	void ScanForBarbarians();
	void UpdateDefenseState();
	void UpdateMilitaryStrategies();
	void UpdateOperations();
#if defined(MOD_BALANCE_CORE)
	void DoNuke(PlayerTypes ePlayer);
	void CheckLandDefenses(PlayerTypes ePlayer, CvCity* pThreatenedCity);
	void CheckSeaDefenses(PlayerTypes ePlayer, CvCity* pThreatenedCity);
	void DoCityAttacks(PlayerTypes ePlayer);
	void SetRecommendedArmyNavySize();
#endif
	void MakeEmergencyPurchases();
	void DisbandObsoleteUnits();

	// Logging functions
	void LogStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue);
	void LogWarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState);
	void LogMilitaryStatus();
	void LogAvailableForces();
	void LogScrapUnit(CvUnit* pUnit, bool bDeficit, bool bSupply);
	void LogGiftUnit(CvUnit* pUnit, bool bDeficit, bool bSupply);
	CvString GetLogFileName(CvString& playerName, bool bSummary=false) const;

	CvPlayer* m_pPlayer;
	CvDiplomacyAI* m_pDiplomacyAI;
	CvMilitaryAIStrategyXMLEntries* m_pAIStrategies;
	bool* m_pabUsingStrategy;
	int* m_paiTurnStrategyAdopted;
	CvEnumMap<FlavorTypes, int> m_aiTempFlavors;
	int m_aiWarFocus[MAX_MAJOR_CIVS];

	// Internal calculated values - must be serialized
	int m_iNumberOfTimesOpsBuildSkippedOver;
	int m_iNumberOfTimesSettlerBuildSkippedOver;

#if defined(MOD_BALANCE_CORE_MILITARY)
	vector<CvAttackTarget> m_potentialAttackTargets; //enemy cities we might want to attack
	vector<pair<PlayerTypes,int>> m_exposedCities; //those of our cities which might be tempting to the enemies
#endif

	// Data recomputed each turn (no need to serialize)
	int m_iNumLandUnits;
	int m_iNumRangedLandUnits;
	int m_iNumMobileLandUnits;
	int m_iNumMeleeLandUnits;
	int m_iNumNavalUnits;
	int m_iNumLandUnitsInArmies;
	int m_iNumNavalUnitsInArmies;
	int m_iRecOffensiveNavalUnits;
	int m_iNumAirUnits;
	int m_iNumAntiAirUnits;
	int m_iBarbarianCampCount;
	int m_iVisibleBarbarianCount;
	int m_iRecOffensiveLandUnits;
	int m_iRecDefensiveLandUnits;
	int m_iNumFreeCarriers;

	//new unit counters
	int m_iNumArcherLandUnits;
	int m_iNumSiegeLandUnits;
	int m_iNumSkirmisherLandUnits;
	int m_iNumReconLandUnits;
	int m_iNumBomberAirUnits;
	int m_iNumFighterAirUnits;
	int m_iNumMeleeNavalUnits;
	int m_iNumRangedNavalUnits;
	int m_iNumSubmarineNavalUnits;
	int m_iNumCarrierNavalUnits;
//	int m_iNumNukeUnits; CvPlayer has this
	int m_iNumMissileUnits;
	int m_iNumActiveUniqueUnits;

	DefenseState m_eLandDefenseState;
	DefenseState m_eNavalDefenseState;
};

FDataStream& operator>>(FDataStream&, CvMilitaryAI&);
FDataStream& operator<<(FDataStream&, const CvMilitaryAI&);

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
bool IsTestStrategy_AtWar(CvPlayer* pPlayer, bool bMinor = true);
bool IsTestStrategy_MinorCivGeneralDefense();
bool IsTestStrategy_MinorCivThreatElevated(CvPlayer* pPlayer);
bool IsTestStrategy_MinorCivThreatCritical(CvPlayer* pPlayer);
bool IsTestStrategy_EradicateBarbarians(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount);
bool IsTestStrategy_EradicateBarbariansCritical(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount);
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

// MOD_AI_UNIT_PRODUCTION
//VP - new strategies
bool IsTestStrategy_EnoughArcherUnits(CvPlayer* pPlayer, int iNumArcher, int iNumMelee);
bool IsTestStrategy_NeedArcherUnits(CvPlayer* pPlayer, int iNumArcher, int iNumMelee);
bool IsTestStrategy_EnoughSiegeUnits(CvPlayer* pPlayer, int iNumSiege, int iNumMelee);
bool IsTestStrategy_NeedSiegeUnits(CvPlayer* pPlayer, int iNumSiege, int iNumMelee);
bool IsTestStrategy_EnoughSkirmisherUnits(CvPlayer* pPlayer, int iNumSkirmisher, int iNumMelee);
bool IsTestStrategy_NeedSkirmisherUnits(CvPlayer* pPlayer, int iNumSkirmisher, int iNumMelee);
bool IsTestStrategy_EnoughNavalMeleeUnits(CvPlayer* pPlayer, int iNumNavalMelee, int iNumNaval);
bool IsTestStrategy_NeedNavalMeleeUnits(CvPlayer* pPlayer, int iNumNavalMelee, int iNumNaval);
bool IsTestStrategy_EnoughNavalRangedUnits(CvPlayer* pPlayer, int iNumNavalRanged, int iNumNaval);
bool IsTestStrategy_NeedNavalRangedUnits(CvPlayer* pPlayer, int iNumNavalRanged, int iNumNaval);
bool IsTestStrategy_EnoughSubmarineUnits(CvPlayer* pPlayer, int iNumSubmarine, int iNumNaval);
bool IsTestStrategy_NeedSubmarineUnits(CvPlayer* pPlayer, int iNumSubmarine, int iNumNaval);
bool IsTestStrategy_EnoughBomberUnits(CvPlayer* pPlayer, int iNumBomber, int iNumAir);
bool IsTestStrategy_NeedBomberUnits(CvPlayer* pPlayer, int iNumBomber, int iNumAir);
bool IsTestStrategy_EnoughFighterUnits(CvPlayer* pPlayer, int iNumFighter, int iNumAir);
bool IsTestStrategy_NeedFighterUnits(CvPlayer* pPlayer, int iNumFighter, int iNumAir);

MultiunitFormationTypes GetCurrentBestFormationTypeForLandAttack();
MultiunitFormationTypes GetCurrentBestFormationTypeForCombinedAttack();
MultiunitFormationTypes GetCurrentBestFormationTypeForPureNavalAttack();
CvPlot* GetCoastalWaterNearPlot(CvPlot *pTarget, bool bCheckTeam = false);

bool ArmyPathIsGood(const SPath& path, PlayerTypes eAttacker, PlayerTypes eIntendedEnemy);
int EvaluateTargetApproach(const CvAttackTarget& target, PlayerTypes ePlayer, ArmyType eArmyType);
void SetBestTargetApproach(CvAttackTarget& target, PlayerTypes ePlayer);
}

#endif //CIV5_MILITARY_AI_H