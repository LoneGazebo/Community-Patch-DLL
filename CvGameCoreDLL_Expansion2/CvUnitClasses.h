/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_UNIT_CLASSES_H
#define CIV5_UNIT_CLASSES_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvUnitEntry
//!  \brief		A single entry from the UnitInfo XML file
//
//!  Key Attributes:
//!  - Used to be called CvUnitInfo
//!  - Populated from XML\GameInfo\CIV5UnitInfos.xml
//!  - Array of these contained in CvUnitXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitEntry: public CvBaseInfo
{
public:
	CvUnitEntry(void);
	~CvUnitEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions (Non-Arrays)
	int GetProductionCost() const;
	int GetFaithCost() const;
	bool IsRequiresFaithPurchaseEnabled() const;
#if defined(MOD_GLOBAL_EXCLUDE_FROM_GIFTS)
	bool IsNoMinorGifts() const;
#endif
	bool IsPurchaseOnly() const;
	bool CanMoveAfterPurchase() const;
#if defined(MOD_GLOBAL_MOVE_AFTER_UPGRADE)
	bool CanMoveAfterUpgrade() const;
#endif
#if defined(MOD_GLOBAL_CANNOT_EMBARK)
	bool CannotEmbark() const;
#endif
	int GetHurryCostModifier() const;
	int GetAdvancedStartCost() const;
	int GetMinAreaSize() const;
	int GetMoves() const;
	bool IsImmobile() const;
	int GetBaseSightRange() const;
	int GetRange() const;
	int GetAirInterceptRange() const;
	int GetBaseLandAirDefense() const;
	int GetAirUnitCap() const;
	int GetNukeDamageLevel() const;
	int GetWorkRate() const;
	int GetNumFreeTechs() const;
	int GetBaseBeakersTurnsToCount() const;
	int GetBaseCultureTurnsToCount() const;
	int GetBaseTurnsForGAPToCount() const;
	int GetBaseHurry() const;
	int GetHurryMultiplier() const;
	bool IsRushBuilding() const;
	int GetBaseGold() const;
	int GetScaleFromNumGWs() const;
	int GetScaleFromNumThemes() const;
	int GetNumGoldPerEra() const;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int GetNumInfPerEra() const;
	int GetProductionCostPerEra() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetNumFreeLux() const;
	int GetBeliefUnlock() const;
	bool IsFreeUpgrade() const;
#endif
	bool IsSpreadReligion() const;
	int GetReligionSpreads() const;
	int GetReligiousStrength() const;
	bool IsFoundReligion() const;
	bool IsRequiresEnhancedReligion() const;
	bool IsProhibitsSpread() const;
	bool IsRemoveHeresy() const;
#if defined(MOD_CIV6_WORKER)
	int GetBuilderStrength() const;
#endif
	bool IsCanBuyCityState() const;
#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
	bool IsCanRepairFleet() const;
	bool IsCanChangePort() const;
#endif
	int GetCombat() const;
#if defined(MOD_GLOBAL_STACKING_RULES)
	int StackCombat() const;
#endif
#if defined(MOD_CARGO_SHIPS)
	int CargoCombat() const;
	int GetSpecialUnitCargoLoad() const;
#endif
	void SetCombat(int iNum);
	int GetCombatLimit() const;
	int GetRangedCombat() const;
	int GetRangedCombatLimit() const;
	bool IsCoastalFireOnly() const;
#if defined(MOD_UNITS_NO_SUPPLY)
	bool IsNoSupply() const;
#endif
#if defined(MOD_UNITS_MAX_HP)
	int GetMaxHitPoints() const;
#endif
	int GetXPValueAttack() const;
	int GetXPValueDefense() const;
	int GetSpecialCargo() const;
	int GetDomainCargo() const;

	int GetConscriptionValue() const;
	int GetExtraMaintenanceCost() const;
	bool IsNoMaintenance() const;
	int GetUnhappiness() const;
	int GetUnitClassType() const;
	int GetSpecialUnitType() const;
	int GetUnitCaptureClassType() const;
	int GetUnitCombatType() const;
#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
	int GetUnitPromotionType() const;
#endif
#if defined(MOD_EVENTS_CAN_MOVE_INTO)
	bool IsSendCanMoveIntoEvent() const;
#endif
	inline int CvUnitEntry::GetDomainType() const { return m_iDomainType; } //this is used very often
	int GetCivilianAttackPriority() const;
	UnitAITypes GetDefaultUnitAIType() const;
	int GetPrereqPillageTech() const;
	int GetPrereqAndTech() const;
	int GetObsoleteTech() const;
#if defined(MOD_BALANCE_CORE)
	int GetEra() const;
#endif
	int GetPolicyType() const;
	int GetGoodyHutUpgradeUnitClass() const;
	int GetGroupSize() const;			// the initial number of individuals in the unit group
	int GetGroupDefinitions() const;	// the number of UnitMeshGroups for this unit
	int GetMeleeWaveSize() const;
	int GetRangedWaveSize() const;
	int GetNumUnitNames() const;
	int GetCommandType() const;
	void SetCommandType(int iNewType);

	bool IsFoodProduction() const;
	bool IsNoBadGoodies() const;
	bool IsRivalTerritory() const;
	bool IsMilitarySupport() const;
	bool IsMilitaryProduction() const;
	bool IsPillage() const;
	bool IsFound() const;
	bool IsFoundAbroad() const;
#if defined(MOD_BALANCE_CORE_SETTLER_ADVANCED)
	bool IsFoundMid() const;
	bool IsFoundLate() const;
	int GetNumColonyFound() const;
	bool IsCityAttackSupport() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int IsGPExtra() const;
	int GetGoodyModifier() const;
	int GetSupplyCapBoost() const;
#endif
	int GetCultureBombRadius() const;
	int GetNumberOfCultureBombs() const;
	int GetGoldenAgeTurns() const;
	int GetFreePolicies() const;
	int GetOneShotTourism() const;
	int GetOneShotTourismPercentOthers() const;
	bool IsIgnoreBuildingDefense() const;
	bool IsPrereqResources() const;
	bool IsMechUnit() const;
	bool IsSuicide() const;
	bool IsCaptureWhileEmbarked() const;
	bool IsRangeAttackOnlyInDomain() const;
	bool IsTrade() const;
	int GetNumExoticGoods() const;

	float GetUnitMaxSpeed() const;
	float GetUnitPadTime() const;

	int GetProjectPrereq() const;
	int GetSpaceshipProject() const;

	int GetLeaderPromotion() const;
	int GetLeaderExperience() const;

	const char* GetUnitArtInfoTag() const;
	const bool GetUnitArtInfoCulturalVariation() const;
	const bool GetUnitArtInfoEraVariation() const;
	int GetUnitFlagIconOffset() const;
	int GetUnitPortraitOffset() const;

	const char* GetUnitNames(int i) const;
	GreatWorkType GetGreatWorks(int i) const;
	bool IsGreatWorkUnit() const;

#if defined(MOD_GLOBAL_STACKING_RULES)
	int GetNumberStackingUnits() const;
#endif

#if defined(MOD_BALANCE_CORE)
	EraTypes GetGreatPersonEra(int i) const;
	int GetResourceType() const;
	bool IsPuppetPurchaseOverride() const;
	bool IsMinorCivGift() const;
	bool IsInvalidMinorCivGift() const;
	int GetCooldown() const;
	int GetGlobalFaithCooldown() const;
	int GetLocalFaithCooldown() const;
	PromotionTypes GetFriendlyLandsPromotion() const;
	bool IsMounted() const;
	bool IsCultureFromExperienceDisbandUpgrade() const;
	bool IsUnitEraUpgrade() const;
	bool IsWarOnly() const;
	bool IsWLTKDFromBirth() const;
	bool IsGoldenAgeFromBirth() const;
	bool IsCultureBoost() const;
	bool IsExtraAttackHealthOnKill() const;
	bool IsHighSeaRaider() const;
#endif
	// Accessor Functions (Arrays)
	int GetPrereqAndTechs(int i) const;
	int GetResourceQuantityRequirement(int i) const;
	int GetResourceQuantityExpended(int i) const;
	int GetBuildingProductionModifier(BuildingTypes eBuilding) const;
	int GetYieldFromKills(YieldTypes eYield) const;
#if defined(MOD_API_UNIFIED_YIELDS)
	int GetYieldFromBarbarianKills(YieldTypes eYield) const;
#endif
	int GetProductionTraits(int i) const;
	int GetFlavorValue(int i) const;
	int GetUnitGroupRequired(int i) const;
	bool GetUpgradeUnitClass(int i) const;
	bool GetUnitAIType(int i) const;
	bool GetNotUnitAIType(int i) const;
	bool GetBuilds(int i) const;
	bool GetGreatPeoples(int i) const;
	bool GetBuildings(int i) const;
	bool GetBuildingClassRequireds(int i) const;
	int GetScalingFromOwnedImprovements(int i) const;
#if defined(MOD_BALANCE_CORE)
	bool GetBuildOnFound(int i) const;
	bool GetBuildingClassPurchaseRequireds(int i) const;
	int GetEraCombatStrength(int i) const;
	int GetUnitNewEraCombatType(int i, int j) const;
	int* GetUnitNewEraCombatTypeChangesArray(int i);
	int GetUnitNewEraPromotions(int i, int j) const;
	int* GetUnitNewEraPromotionsChangesArray(int i);
#endif'
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
	int GetResourceQuantityTotal(int i) const;
#endif
	bool GetFreePromotions(int i) const;

	// Derived fields (not in XML)
	int GetCargoSpace() const;  // (from free promotions)
	int GetPower() const;
	void DoUpdatePower();

	UnitMoveRate GetMoveRate(int numHexes) const;

private:

	int m_iProductionCost;
	int m_iFaithCost;
	bool m_bRequiresFaithPurchaseEnabled;
#if defined(MOD_GLOBAL_EXCLUDE_FROM_GIFTS)
	bool m_bNoMinorGifts;
#endif
	bool m_bPurchaseOnly;
	bool m_bMoveAfterPurchase;
#if defined(MOD_GLOBAL_MOVE_AFTER_UPGRADE)
	bool m_bMoveAfterUpgrade;
#endif
#if defined(MOD_GLOBAL_CANNOT_EMBARK)
	bool m_bCannotEmbark;
#endif
	int m_iHurryCostModifier;
	int m_iAdvancedStartCost;
	int m_iMinAreaSize;
	int m_iMoves;
	bool m_bMoves;
	int m_iBaseSightRange;
	int m_iRange;
	int m_iAirInterceptRange;
	int m_iBaseLandAirDefense;
	int m_iAirUnitCap;
	int m_iNukeDamageLevel;
	int m_iWorkRate;
	int m_iNumFreeTechs;
	int m_iBaseBeakersTurnsToCount;
	int m_iBaseCultureTurnsToCount;
	int m_iBaseTurnsForGAPToCount;
	int m_iBaseHurry;
	int m_iHurryMultiplier;
	bool m_bRushBuilding;
	int m_iBaseGold;
	int m_iScaleFromNumGWs;
	int m_iScaleFromNumThemes;
	int m_iNumGoldPerEra;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iNumInfPerEra;
	int m_iProductionCostPerEra;
#endif
#if defined(MOD_BALANCE_CORE)
	int m_iNumFreeLux;
	bool m_bFreeUpgrade;
#endif
	bool m_bSpreadReligion;
	int m_iReligionSpreads;
	int m_iReligiousStrength;
	bool m_bFoundReligion;
	bool m_bRequiresEnhancedReligion;
	bool m_bProhibitsSpread;
	bool m_bRemoveHeresy;
#if defined(MOD_CIV6_WORKER)
	int m_iBuilderStrength;
#endif
	bool m_bCanBuyCityState;
#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
	bool m_bCanRepairFleet;
	bool m_bCanChangePort;
#endif
	int m_iCombat;
#if defined(MOD_GLOBAL_STACKING_RULES)
	int m_iStackCombat;
#endif
#if defined(MOD_CARGO_SHIPS)
	int m_iCargoCombat;
	int m_iSpecialUnitCargoLoad;
#endif
	int m_iCombatLimit;
	int m_iRangedCombat;
	int m_iRangedCombatLimit;
	bool m_bCoastalFire;
#if defined(MOD_UNITS_NO_SUPPLY)
	bool m_bNoSupply;
#endif
#if defined(MOD_UNITS_MAX_HP)
	int m_iMaxHitPoints;
#endif
	int m_iXPValueAttack;
	int m_iXPValueDefense;
	int m_iSpecialCargo;

	int m_iDomainCargo;
	int m_iConscriptionValue;
	int m_iExtraMaintenanceCost;
	bool m_bNoMaintenance;
	int m_iUnhappiness;
	int m_iUnitClassType;
	int m_iSpecialUnitType;
	int m_iUnitCaptureClassType;
	int m_iUnitCombatType;
#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
	int m_iUnitPromotionType;
#endif
#if defined(MOD_EVENTS_CAN_MOVE_INTO)
	bool m_bSendCanMoveIntoEvent;
#endif
	int m_iDomainType;
	int m_iCivilianAttackPriority;
	int m_iDefaultUnitAIType;
	int m_iPrereqPillageTech;
	int m_iPrereqAndTech;
	int m_iObsoleteTech;
	int m_iPolicyType;
	int m_iGoodyHutUpgradeUnitClass;
	int m_iGroupSize;
	int m_iGroupDefinitions;
	int m_iUnitMeleeWaveSize;
	int m_iUnitRangedWaveSize;
	int m_iNumUnitNames;
	int m_iCommandType;
	int m_iLeaderExperience;

	bool m_bFoodProduction;
	bool m_bNoBadGoodies;
	bool m_bRivalTerritory;
	bool m_bMilitarySupport;
	bool m_bMilitaryProduction;
	bool m_bPillage;
	bool m_bFound;
	bool m_bFoundAbroad;
#if defined(MOD_BALANCE_CORE)
	bool m_bFoundMid;
	bool m_bFoundLate;
	int m_iFoundColony;
	bool m_bIsCityAttackSupport;
	int m_iGPExtra;
	int m_iGoodyModifier;
	int m_iSupplyCapBoost;
#endif
#if defined(MOD_BALANCE_CORE)
	int m_iResourceType;
	bool m_bPuppetPurchaseOverride;
	bool m_bMinorCivGift;
	bool m_bNoMinorCivGift;
	bool m_bIsMounted;
	int m_iCooldown;
	int m_iGlobalFaithCooldown;
	int m_iLocalFaithCooldown;
	int m_iFriendlyLandsPromotion;
	int m_iBeliefUnlock;
	bool m_bCultureFromExperienceOnDisband;
	bool m_bIsConvertUnit;
	bool m_bUnitEraUpgrade;
	bool m_bWarOnly;
	bool m_bWLTKD;
	bool m_bGoldenAge;
	bool m_bCultureBoost;
	bool m_bExtraAttackHealthOnKill;
	bool m_bHighSeaRaider;
#endif
	int m_iCultureBombRadius;
	int m_iNumberOfCultureBombs;
	int m_iGoldenAgeTurns;
	int m_iFreePolicies;
	int m_iOneShotTourism;
	int m_iOneShotTourismPercentOthers;
	bool m_bIgnoreBuildingDefense;
	bool m_bPrereqResources;
	bool m_bMechanized;
	bool m_bSuicide;
	bool m_bCaptureWhileEmbarked;
	bool m_bRangeAttackOnlyInDomain;
	int m_iProjectPrereq;
	int m_iSpaceshipProject;
	int m_iLeaderPromotion;
	bool m_bTrade;
	int m_iNumExoticGoods;

#if defined(MOD_GLOBAL_STACKING_RULES)
	int m_iNumberStackingUnits;
#endif

	// This is not loaded from XML, but cached so we don't have to recalculate every time
	int m_iCachedPower;

	int m_iUnitFlagIconOffset;
	int m_iUnitPortraitOffset;

	CvString m_strUnitArtInfoTag;
	bool m_bUnitArtInfoCulturalVariation;
	bool m_bUnitArtInfoEraVariation;

	UnitMoveRate m_unitMoveRate[12];

	// Arrays
	int* m_piPrereqAndTechs;
	int* m_piResourceQuantityRequirements;
	int* m_piResourceQuantityExpended;
	int* m_piProductionTraits;
	int* m_piFlavorValue;
	int* m_piUnitGroupRequired;
	int* m_piProductionModifierBuildings;
	int* m_piYieldFromKills;
#if defined(MOD_API_UNIFIED_YIELDS)
	int* m_piYieldFromBarbarianKills;
#endif

	bool* m_pbUpgradeUnitClass;
	bool* m_pbUnitAIType;
	bool* m_pbNotUnitAIType;
	bool* m_pbBuilds;
	bool* m_pbGreatPeoples;
	bool* m_pbBuildings;
	bool* m_pbBuildingClassRequireds;
#if defined(MOD_BALANCE_CORE)
	int* m_piScalingFromOwnedImprovements;
	bool* m_pbBuildOnFound;
	bool* m_pbBuildingClassPurchaseRequireds;
	int* m_piEraCombatStrength;
	int** m_ppiEraUnitCombatType;
	int** m_ppiEraUnitPromotions;
#endif
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
	std::map<int, int> m_piResourceQuantityTotals;
#endif
	bool* m_pbFreePromotions;

	CvString* m_paszEarlyArtDefineTags;
	CvString* m_paszLateArtDefineTags;
	CvString* m_paszMiddleArtDefineTags;
	CvString* m_paszUnitNames;
	GreatWorkType* m_paeGreatWorks;
#if defined(MOD_BALANCE_CORE)
	EraTypes* m_paeGreatPersonEra;
#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvUnitXMLEntries
//!  \brief		Game-wide information about units
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\GameInfo\CIV5UnitInfos.xml
//! - Contains an array of CvUnitEntry from the above XML file
//! - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitXMLEntries
{
public:
	CvUnitXMLEntries(void);
	~CvUnitXMLEntries(void);

	// Accessor functions
	std::vector<CvUnitEntry*>& GetUnitEntries();
	int GetNumUnits();
	_Ret_maybenull_ CvUnitEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvUnitEntry*> m_paUnitEntries;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays of variable length (based on number of units defined in game)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace UnitArrayHelpers
{
void Read(FDataStream& kStream, int* paiUnitArray);
void Write(FDataStream& kStream, int* paiUnitArray, int iArraySize);
}

#endif //CIV5_UNIT_CLASSES_H